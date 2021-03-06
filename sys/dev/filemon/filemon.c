/*-
 * Copyright (c) 2011, David E. O'Brien.
 * Copyright (c) 2009-2011, Juniper Networks, Inc.
 * Copyright (c) 2015, EMC Corp.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY JUNIPER NETWORKS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL JUNIPER NETWORKS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "opt_compat.h"

#include <sys/param.h>
#include <sys/file.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/capsicum.h>
#include <sys/condvar.h>
#include <sys/conf.h>
#include <sys/fcntl.h>
#include <sys/ioccom.h>
#include <sys/kernel.h>
#include <sys/lock.h>
#include <sys/malloc.h>
#include <sys/module.h>
#include <sys/poll.h>
#include <sys/proc.h>
#include <sys/queue.h>
#include <sys/sx.h>
#include <sys/syscall.h>
#include <sys/sysent.h>
#include <sys/sysproto.h>
#include <sys/uio.h>

#include "filemon.h"

#if defined(COMPAT_IA32) || defined(COMPAT_FREEBSD32) || defined(COMPAT_ARCH32)
#include <compat/freebsd32/freebsd32_syscall.h>
#include <compat/freebsd32/freebsd32_proto.h>

extern struct sysentvec ia32_freebsd_sysvec;
#endif

extern struct sysentvec elf32_freebsd_sysvec;
extern struct sysentvec elf64_freebsd_sysvec;

static d_close_t	filemon_close;
static d_ioctl_t	filemon_ioctl;
static d_open_t		filemon_open;

static struct cdevsw filemon_cdevsw = {
	.d_version	= D_VERSION,
	.d_close	= filemon_close,
	.d_ioctl	= filemon_ioctl,
	.d_open		= filemon_open,
	.d_name		= "filemon",
};

MALLOC_DECLARE(M_FILEMON);
MALLOC_DEFINE(M_FILEMON, "filemon", "File access monitor");

struct filemon {
	TAILQ_ENTRY(filemon) link;	/* Link into the in-use list. */
	struct sx	lock;		/* Lock mutex for this filemon. */
	struct file	*fp;		/* Output file pointer. */
	struct proc     *p;		/* The process being monitored. */
	char		fname1[MAXPATHLEN]; /* Temporary filename buffer. */
	char		fname2[MAXPATHLEN]; /* Temporary filename buffer. */
	char		msgbufr[1024];	/* Output message buffer. */
};

static TAILQ_HEAD(, filemon) filemons_inuse = TAILQ_HEAD_INITIALIZER(filemons_inuse);
static TAILQ_HEAD(, filemon) filemons_free = TAILQ_HEAD_INITIALIZER(filemons_free);
static struct sx access_lock;

static struct cdev *filemon_dev;

#include "filemon_lock.c"
#include "filemon_wrapper.c"

static void
filemon_comment(struct filemon *filemon)
{
	int len;
	struct timeval now;

	getmicrotime(&now);

	len = snprintf(filemon->msgbufr, sizeof(filemon->msgbufr),
	    "# filemon version %d\n# Target pid %d\n# Start %ju.%06ju\nV %d\n",
	    FILEMON_VERSION, curproc->p_pid, (uintmax_t)now.tv_sec,
	    (uintmax_t)now.tv_usec, FILEMON_VERSION);

	filemon_output(filemon, filemon->msgbufr, len);
}

static void
filemon_dtr(void *data)
{
	struct filemon *filemon = data;

	if (filemon != NULL) {
		struct file *fp;

		/* Follow same locking order as filemon_pid_check. */
		filemon_lock_write();
		sx_xlock(&filemon->lock);

		/* Remove from the in-use list. */
		TAILQ_REMOVE(&filemons_inuse, filemon, link);

		fp = filemon->fp;
		filemon->fp = NULL;
		filemon->p = NULL;

		/* Add to the free list. */
		TAILQ_INSERT_TAIL(&filemons_free, filemon, link);

		/* Give up write access. */
		sx_xunlock(&filemon->lock);
		filemon_unlock_write();

		if (fp != NULL)
			fdrop(fp, curthread);
	}
}

static int
filemon_ioctl(struct cdev *dev, u_long cmd, caddr_t data, int flag __unused,
    struct thread *td)
{
	int error = 0;
	struct filemon *filemon;
	struct proc *p;
	cap_rights_t rights;

	if ((error = devfs_get_cdevpriv((void **) &filemon)) != 0)
		return (error);

	sx_xlock(&filemon->lock);

	switch (cmd) {
	/* Set the output file descriptor. */
	case FILEMON_SET_FD:
		if (filemon->fp != NULL)
			fdrop(filemon->fp, td);

		error = fget_write(td, *(int *)data,
		    cap_rights_init(&rights, CAP_PWRITE),
		    &filemon->fp);
		if (error == 0)
			/* Write the file header. */
			filemon_comment(filemon);
		break;

	/* Set the monitored process ID. */
	case FILEMON_SET_PID:
		error = pget(*((pid_t *)data), PGET_CANDEBUG | PGET_NOTWEXIT,
		    &p);
		if (error == 0) {
			filemon->p = p;
			PROC_UNLOCK(p);
		}
		break;

	default:
		error = EINVAL;
		break;
	}

	sx_xunlock(&filemon->lock);
	return (error);
}

static int
filemon_open(struct cdev *dev, int oflags __unused, int devtype __unused,
    struct thread *td __unused)
{
	struct filemon *filemon;

	/* Get exclusive write access. */
	filemon_lock_write();

	if ((filemon = TAILQ_FIRST(&filemons_free)) != NULL)
		TAILQ_REMOVE(&filemons_free, filemon, link);

	/* Give up write access. */
	filemon_unlock_write();

	if (filemon == NULL) {
		filemon = malloc(sizeof(struct filemon), M_FILEMON,
		    M_WAITOK | M_ZERO);
		sx_init(&filemon->lock, "filemon");
	}

	devfs_set_cdevpriv(filemon, filemon_dtr);

	/* Get exclusive write access. */
	filemon_lock_write();

	/* Add to the in-use list. */
	TAILQ_INSERT_TAIL(&filemons_inuse, filemon, link);

	/* Give up write access. */
	filemon_unlock_write();

	return (0);
}

static int
filemon_close(struct cdev *dev __unused, int flag __unused, int fmt __unused,
    struct thread *td __unused)
{

	return (0);
}

static void
filemon_load(void *dummy __unused)
{
	sx_init(&access_lock, "filemons_inuse");

	/* Install the syscall wrappers. */
	filemon_wrapper_install();

	filemon_dev = make_dev(&filemon_cdevsw, 0, UID_ROOT, GID_WHEEL, 0666,
	    "filemon");
}

static int
filemon_unload(void)
{
 	struct filemon *filemon;
	int error = 0;

	/* Get exclusive write access. */
	filemon_lock_write();

	if (TAILQ_FIRST(&filemons_inuse) != NULL)
		error = EBUSY;
	else {
		destroy_dev(filemon_dev);

		/* Deinstall the syscall wrappers. */
		filemon_wrapper_deinstall();
	}

	/* Give up write access. */
	filemon_unlock_write();

	if (error == 0) {
		/* free() filemon structs free list. */
		filemon_lock_write();
		while ((filemon = TAILQ_FIRST(&filemons_free)) != NULL) {
			TAILQ_REMOVE(&filemons_free, filemon, link);
			sx_destroy(&filemon->lock);
			free(filemon, M_FILEMON);
		}
		filemon_unlock_write();

		sx_destroy(&access_lock);
	}

	return (error);
}

static int
filemon_modevent(module_t mod __unused, int type, void *data)
{
	int error = 0;

	switch (type) {
	case MOD_LOAD:
		filemon_load(data);
		break;

	case MOD_UNLOAD:
		error = filemon_unload();
		break;

	case MOD_QUIESCE:
		/*
		 * The wrapper implementation is unsafe for reliable unload.
		 * Require forcing an unload.
		 */
		error = EBUSY;
		break;

	case MOD_SHUTDOWN:
		break;

	default:
		error = EOPNOTSUPP;
		break;

	}

	return (error);
}

DEV_MODULE(filemon, filemon_modevent, NULL);
MODULE_VERSION(filemon, 1);
