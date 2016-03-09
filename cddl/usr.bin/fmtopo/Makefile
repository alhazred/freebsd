# $FreeBSD$

.PATH: ${.CURDIR}/../../contrib/opensolaris/cmd/fmtopo

PROG=	fmtopo
MAN=

WARNS?=	0
MK_DEBUG_FILES=	no
CFLAGS+= -I${.CURDIR}/../../../sys/cddl/compat/opensolaris
CFLAGS+= -I${.CURDIR}/../../compat/opensolaris/lib/libumem

CFLAGS+= -I${.CURDIR}/../../compat/opensolaris/include
CFLAGS+= -I${.CURDIR}/../../contrib/opensolaris/lib/fm/topo/common
CFLAGS+= -I${.CURDIR}/../../contrib/opensolaris/lib/libnvpair
CFLAGS+= -I${.CURDIR}/../../../sys/cddl/contrib/opensolaris/uts/common
CFLAGS+= -I${.CURDIR}/../../contrib/opensolaris/head
CFLAGS+= -I${.CURDIR}/../../lib/libumem

LIBADD=nvpair uutil umem  topo xml2 thr

.include <bsd.prog.mk>
