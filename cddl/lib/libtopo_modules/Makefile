# $FreeBSD$

.PATH: ${.CURDIR}/../../../cddl/contrib/opensolaris/lib/fm/topo/modules/

LIB=chip
SRCS=	chip.c \
	chip_label.c \
	chip_subr.c

SHLIB_NAME=${LIB}.so
PLUGINDIR=	${LIBDIR}/fm/topo/plugins
# These FILES qualify as libraries for the purpose of LIBRARIES_ONLY.
.undef LIBRARIES_ONLY


WARNS?=	0
CFLAGS+= -I${.CURDIR}/../../../cddl/contrib/opensolaris/lib/fm/topo/common
CFLAGS+= -I${.CURDIR}/../../../sys/cddl/contrib/opensolaris/uts/common
CFLAGS+= -I${.CURDIR}/../../../sys/cddl/compat/opensolaris
CFLAGS+= -I${.CURDIR}/../../../cddl/compat/opensolaris/include
CFLAGS+= -I${.CURDIR}/../../../cddl/compat/opensolaris/lib/libumem
CFLAGS+= -I${.CURDIR}/../../../cddl/contrib/opensolaris/lib/libnvpair
CFLAGS+= -I${.CURDIR}/../../../cddl/contrib/opensolaris/lib/libuutil/common
CFLAGS+= -I/usr/local/include/libxml2

install: ${SHLIB_NAME}
	mkdir -p ${PLUGINDIR}
	${INSTALL} -C -o ${LIBOWN} -g ${LIBGRP} -m ${LIBMODE} ${SHLIB_NAME} ${PLUGINDIR}


.include <bsd.lib.mk>
