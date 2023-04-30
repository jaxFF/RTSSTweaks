// RTCoreErrors.h: error codes for RTCore library
// 
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _RTCOREERRORS_H_INCLUDED_
#define _RTCOREERRORS_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#define RTCORE_E(code) MAKE_HRESULT(1, FACILITY_ITF, code)
#define RTCORE_S(code) MAKE_HRESULT(0, FACILITY_ITF, code)

// CRTCoreDriver error codes

#define RTCOREDRIVER_E_FILE_NOT_FOUND									RTCORE_E(0x0400)
#define RTCOREDRIVER_E_INVALID_SCM_HANDLE								RTCORE_E(0x0401)
#define	RTCOREDRIVER_E_CANNOT_QUERY_SERVICE_STATUS						RTCORE_E(0x0402)
#define RTCOREDRIVER_E_CANNOT_INSTALL_OR_OPEN_SERVICE					RTCORE_E(0x0403)
#define RTCOREDRIVER_E_CANNOT_OPEN_DRIVER								RTCORE_E(0x0404)

//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
