# $FreeBSD$

.PATH: ${.CURDIR}/../../../cddl/contrib/opensolaris/lib/fm/topo/common

LIB=	topo
SRCS=	cpu.c \
	mem.c \
	hc.c \
	legacy_hc.c \
	topo_2xml.c \
	topo_alloc.c \
	topo_builtin.c \
	topo_file.c \
	topo_fmri.c \
	topo_error.c \
	topo_list.c \
	topo_method.c \
	topo_mod.c \
	topo_module.c \
	topo_node.c \
	topo_nvl.c \
	topo_parse.c \
	topo_prop.c \
	topo_protocol.c \
	topo_rtld.c \
	topo_snap.c \
	topo_string.c \
	topo_subr.c \
	topo_tables.c \
	topo_tree.c \
	topo_xml.c


MAPDIR=	/usr/lib/fm/topo/maps
DTDDIR=	/usr/share/lib/xml/dtd
MK_DEBUG_FILES=no
WARNS?=	0
CFLAGS+= -I${.CURDIR}/../../../cddl/contrib/opensolaris/lib/fm/topo/common
CFLAGS+= -I${.CURDIR}/../../../sys/cddl/contrib/opensolaris/uts/common
CFLAGS+= -I${.CURDIR}/../../../sys/cddl/compat/opensolaris
CFLAGS+= -I${.CURDIR}/../../../cddl/compat/opensolaris/include
CFLAGS+= -I${.CURDIR}/../../../cddl/compat/opensolaris/lib/libumem
CFLAGS+= -I${.CURDIR}/../../../cddl/contrib/opensolaris/lib/libnvpair
CFLAGS+= -I${.CURDIR}/../../../cddl/contrib/opensolaris/lib/libuutil/common
CFLAGS+= -I/usr/local/include/libxml2
.include <bsd.lib.mk>

install:
	mkdir -p ${MAPDIR}
	mkdir -p ${DTDDIR}
	${INSTALL} -m ${CONFMODE} ${.CURDIR}/../../../cddl/contrib/opensolaris/lib/fm/topo/maps/hc-topology.xml ${MAPDIR}
	${INSTALL} -m ${CONFMODE} ${.CURDIR}/../../../cddl/contrib/opensolaris/lib/fm/topo/maps/topology.dtd.1 ${DTDDIR}
