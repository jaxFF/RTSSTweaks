// IGCLWrapper.h: interface for the CIGCLWrapper class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "igcl_api.h"
#include "D3DKMTWrapper.h"
//////////////////////////////////////////////////////////////////////
#define IGCLWRAPPER_MAX_GPU								256
#define	IGCLWRAPPER_INVALID_GPU							0xFFFFFFFF
//////////////////////////////////////////////////////////////////////
class CIGCLWrapper
{
public:
	CIGCLWrapper();
	virtual ~CIGCLWrapper();

	void	Init(CD3DKMTWrapper* lpD3DKMTWrapper);
	void	Uninit();

	DWORD	GetGpuCount();
	
	DWORD	GetVersion();
	BOOL	GetName(DWORD dwGpu, CString& strName);
	BOOL	GetLocation(DWORD dwGpu, DWORD* lpBus, DWORD* lpDev, DWORD* lpFn);
	BOOL	GetLUID(DWORD dwGpu, LUID& luid);

	BOOL	GetTotalVideomemory(DWORD dwGpu, DWORD& dwTotalVideomemory);

	BOOL	RetreivePowerTelemetry(DWORD dwGpu);

	void	SetTimestamp(DWORD dwTimestamp);

	ctl_power_telemetry_t* GetCurrentPowerTelemetry(DWORD dwGpu);
	ctl_power_telemetry_t* GetPreviousPowerTelemetry(DWORD dwGpu);

protected:
	DWORD						m_dwVersion;

	DWORD						m_dwGpuCount;

	ctl_api_handle_t			m_hApi;
	ctl_device_adapter_handle_t	m_hGpu[IGCLWRAPPER_MAX_GPU];
	DWORD						m_dwBus[IGCLWRAPPER_MAX_GPU];
	DWORD						m_dwDev[IGCLWRAPPER_MAX_GPU];

	DWORD						m_dwTimestamp;
	DWORD						m_dwPowerTelemetryTimestamp[IGCLWRAPPER_MAX_GPU];

	ctl_power_telemetry_t		m_powerTelemetryCurrent [IGCLWRAPPER_MAX_GPU];
	ctl_power_telemetry_t		m_powerTelemetryPrevious[IGCLWRAPPER_MAX_GPU];
};
//////////////////////////////////////////////////////////////////////
