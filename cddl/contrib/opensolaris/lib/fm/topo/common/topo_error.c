/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#include <strings.h>
#include <topo_error.h>
#include <topo_mod.h>


static const char *const _topo_errstrs[] = {
	"unknown libtopo error",
	"memory limit exceeded",
	"module detected or caused an error",
	"failed to initialize module",
	"failed to uninitialize module",
	"specified module is already loaded",
	"specified module is not loaded",
	"module registered with invalid ABI version",
	"module invalid argument",
	"module duplicate node entry",
	"module failed to register",
	"module path invalid",
	"unable to read topology map file",
	"unable to enumerate from a topology map file",
	"enumerator not supported in this module",
	"module version mismatch while loading",
	"rtld failed to open shared library plug-in",
	"shared library plug-in does not define _topo_init",
	"memory limit exceeded when opening shared library",
	"built-in plug-in name not found in definition list",
	"built-in plug-in does not define init function",
	"plugin compiled using an obsolete topo ABI",
	"plugin is compiled using a newer topo ABI",
	"partial enumeration completed for client",
	"no topology map file for enumeration",
	"fatal enumeration error",
	"recursive enumertation detected",
	"invalid nvlist function argument",
	"no topology file found",
	"unrecognized grouping",
	"unable to interpret attribute numerically",
	"non-sensical range",
	"unrecognized scheme",
	"unrecognized stability",
	"unrecognized property value type",
	"tag missing attribute",
	"topology xml file not found",
	"range missing enum-method",
	"properties as nvlist missing crucial field",
	"node instance out of declared range",
	"failed to register property method",
	"empty topology",
	"scheme based topology not found",
	"no facility node of specified type found",
	"end of custom errno list (to ease auto-merge)",
};

static const int _topo_nerrstrs =
    sizeof (_topo_errstrs) / sizeof (_topo_errstrs[0]);


int
topo_hdl_errno(topo_hdl_t *thp)
{
	return (thp->th_errno);
}

int
topo_hdl_seterrno(topo_hdl_t *thp, int err)
{
	thp->th_errno = err;
	return (-1);
}

const char *
topo_hdl_errmsg(topo_hdl_t *thp)
{
	return (topo_strerror(thp->th_errno));
}

static const char *const _topo_properrstrs[] = {
	"unknown topo prop error",
	"undefined property or property group",
	"static property already defined",
	"memory limit exceeded during property allocation",
	"invalid property type",
	"invalid property name",
	"can not inherit property",
	"malformed property nvlist",
	"get property method failed",
	"end of prop errno list (to ease auto-merge)",
};

static const int _topo_nproperrstrs =
    sizeof (_topo_properrstrs) / sizeof (_topo_properrstrs[0]);

static const char *const _topo_methoderrstrs[] = {
	"unknown topo method error",
	"invalid method registration",
	"method not supported",
	"method failed",
	"app is compiled to use obsolete method",
	"app is compiled to use obsolete method",
	"memory limit exceeded during method op",
	"method op already defined",
	"end of method errno list",
};

static const int _topo_nmethoderrstrs =
    sizeof (_topo_methoderrstrs) / sizeof (_topo_methoderrstrs[0]);

static const char *const _topo_fmrierrstrs[] = {
	"unknown topo fmri error",
	"nvlist allocation failure for FMRI",
	"invalid FMRI scheme version",
	"malformed FMRI",
	"memory limit exceeded",
	"end of fmri errno list",
};

static const int _topo_nfmrierrstrs =
    sizeof (_topo_fmrierrstrs) / sizeof (_topo_fmrierrstrs[0]);

static const char *const _topo_hdlerrstrs[] = {
	"unknown topo handle error",
	"handle opened with invalid ABI version",
	"snapshot already taken",
	"invalid argument specified",
	"uuid already set",
	"memory limit exceeded",
	"end of handle errno list",
};

static const int _topo_nhdlerrstrs =
    sizeof (_topo_hdlerrstrs) / sizeof (_topo_hdlerrstrs[0]);

static const char *const _topo_moderrstrs[] = {
	"unknown libtopo error",
	"module memory limit exceeded",
	"module completed partial enumeration",
	"method arguments invalid",
	"method not supported",
	"nvlist allocation failure for FMRI",
	"invalid FMRI scheme version",
	"malformed FMRI",
	"node already bound",
	"duplicate node",
	"node not found",
	"invalid node range",
	"registered with invalid ABI version",
	"attempt to load obsolete module",
	"attempt to load a newer module",
	"invalid nvlist",
	"non-canonical component name requested",
	"module lookup failed",
	"unknown enumeration error",
	"end of mod errno list (to ease auto-merge)",
};
static const int _topo_nmoderrstrs =
    sizeof (_topo_moderrstrs) / sizeof (_topo_moderrstrs[0]);


int
topo_mod_errno(topo_mod_t *mp)
{
	return (mp->tm_errno);
}

int
topo_mod_seterrno(topo_mod_t *mp, int err)
{
	mp->tm_errno = err;
	return (-1);
}

const char *
topo_mod_errmsg(topo_mod_t *mp)
{
	return (topo_strerror(mp->tm_errno));
}

const char *
topo_strerror(int err)
{
	const char *s;

	if (err >= ETOPO_UNKNOWN && (err - ETOPO_UNKNOWN) < _topo_nerrstrs)
		s = _topo_errstrs[err - ETOPO_UNKNOWN];
	else if (err >= EMOD_UNKNOWN && (err - EMOD_UNKNOWN) <
	    _topo_nmoderrstrs)
		s = _topo_moderrstrs[err - EMOD_UNKNOWN];
	else if (err >= ETOPO_PROP_UNKNOWN && (err - ETOPO_PROP_UNKNOWN) <
	    _topo_nproperrstrs)
		s = _topo_properrstrs[err - ETOPO_PROP_UNKNOWN];
	else if (err >= ETOPO_METHOD_UNKNOWN && (err - ETOPO_METHOD_UNKNOWN) <
	    _topo_nmethoderrstrs)
		s = _topo_methoderrstrs[err - ETOPO_METHOD_UNKNOWN];
	else if (err >= ETOPO_HDL_UNKNOWN && (err - ETOPO_HDL_UNKNOWN) <
	    _topo_nhdlerrstrs)
		s = _topo_hdlerrstrs[err - ETOPO_HDL_UNKNOWN];
	else if (err >= ETOPO_FMRI_UNKNOWN && (err - ETOPO_FMRI_UNKNOWN) <
	    _topo_nfmrierrstrs)
		s = _topo_fmrierrstrs[err - ETOPO_FMRI_UNKNOWN];
	else
		s = _topo_errstrs[0];

	return (s);
}
