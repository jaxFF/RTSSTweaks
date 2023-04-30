// HALImplementationATIADL.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HALImplementationATIADL.h"
//////////////////////////////////////////////////////////////////////
#define CACHE_ID_ENGINE_CLOCK										0
#define CACHE_ID_MEMORY_CLOCK										1
#define CACHE_ID_VDDC												2
#define CACHE_ID_ACTIVITY											3
//////////////////////////////////////////////////////////////////////
CHALImplementationATIADL::CHALImplementationATIADL()
{
	m_lpATIADLWrapper	= NULL;
	m_dwGpu				= 0;
}
//////////////////////////////////////////////////////////////////////
CHALImplementationATIADL::~CHALImplementationATIADL()
{
}
//////////////////////////////////////////////////////////////////////
void CHALImplementationATIADL::SetATIADLWrapper(CATIADLWrapper* lpATIADLWrapper)
{
	m_lpATIADLWrapper = lpATIADLWrapper;
}
//////////////////////////////////////////////////////////////////////
void CHALImplementationATIADL::SetGpu(DWORD dwGpu)
{
	m_dwGpu = dwGpu;
}
//////////////////////////////////////////////////////////////////////
void CHALImplementationATIADL::SetTimestamp(DWORD dwTimestamp)
{
	CHALImplementation::SetTimestamp(dwTimestamp);

	m_lpATIADLWrapper->SetTimestamp(dwTimestamp);
}
//////////////////////////////////////////////////////////////////////
CString	CHALImplementationATIADL::GetDriverStr()
{
	CString strEdition = m_lpATIADLWrapper->GetEditionStr();
	CString strVersion = m_lpATIADLWrapper->GetVersionStr();

	if (!strEdition.IsEmpty())
		return strEdition + " " + strVersion;

	return strVersion;
}
//////////////////////////////////////////////////////////////////////
CString	CHALImplementationATIADL::GetName()
{
	CString strName;

	if (m_lpATIADLWrapper->GetName(m_dwGpu, strName))
		return strName;

	return "";
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::GetLocation(DWORD* lpBus, DWORD* lpDev, DWORD* lpFn)
{
	return m_lpATIADLWrapper->GetLocation(m_dwGpu, lpBus, lpDev, lpFn);
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsCoreClockReportingSupported()
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsMemoryClockReportingSupported()
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentCoreClock()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_ENGINE_CLOCK);

	if (lpData)
		return lpData->dwData;

	DWORD dwEngineClock	= 0;
	DWORD dwMemoryClock	= 0;
	DWORD dwVDDC		= 0;
	DWORD dwActivity	= 0;

	if (m_lpATIADLWrapper->GetCurrentActivity(m_dwGpu, &dwEngineClock, &dwMemoryClock, &dwVDDC, &dwActivity))
	{
		SetCacheData(CACHE_ID_ENGINE_CLOCK	, dwEngineClock	);
		SetCacheData(CACHE_ID_MEMORY_CLOCK	, dwMemoryClock	);
		SetCacheData(CACHE_ID_VDDC			, dwVDDC		);
		SetCacheData(CACHE_ID_ACTIVITY		, dwActivity	);

		return dwEngineClock;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentMemoryClock()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_MEMORY_CLOCK);

	if (lpData)
		return lpData->dwData;

	DWORD dwEngineClock	= 0;
	DWORD dwMemoryClock	= 0;
	DWORD dwVDDC		= 0;
	DWORD dwActivity	= 0;

	if (m_lpATIADLWrapper->GetCurrentActivity(m_dwGpu, &dwEngineClock, &dwMemoryClock, &dwVDDC, &dwActivity))
	{
		SetCacheData(CACHE_ID_ENGINE_CLOCK	, dwEngineClock	);
		SetCacheData(CACHE_ID_MEMORY_CLOCK	, dwMemoryClock	);
		SetCacheData(CACHE_ID_VDDC			, dwVDDC		);
		SetCacheData(CACHE_ID_ACTIVITY		, dwActivity	);

		return dwMemoryClock;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsCoreVoltageReportingSupported()
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsMemoryVoltageReportingSupported()
{
	return m_lpATIADLWrapper->GetCurrentMemoryVoltage(m_dwGpu, NULL);
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentCoreVoltage()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_VDDC);

	if (lpData)
		return lpData->dwData;

	DWORD dwEngineClock	= 0;
	DWORD dwMemoryClock	= 0;
	DWORD dwVDDC		= 0;
	DWORD dwActivity	= 0;

	if (m_lpATIADLWrapper->GetCurrentActivity(m_dwGpu, &dwEngineClock, &dwMemoryClock, &dwVDDC, &dwActivity))
	{
		SetCacheData(CACHE_ID_ENGINE_CLOCK	, dwEngineClock	);
		SetCacheData(CACHE_ID_MEMORY_CLOCK	, dwMemoryClock	);
		SetCacheData(CACHE_ID_VDDC			, dwVDDC		);
		SetCacheData(CACHE_ID_ACTIVITY		, dwActivity	);

		return dwVDDC;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentMemoryVoltage()
{
	DWORD dwVoltage;

	if (m_lpATIADLWrapper->GetCurrentMemoryVoltage(m_dwGpu, &dwVoltage))
		return dwVoltage;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCoreTemperatureSensorCount()
{
	return m_lpATIADLWrapper->GetCoreTemperatureSensorCount(m_dwGpu);
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetMemoryTemperatureSensorCount()
{
	return m_lpATIADLWrapper->GetMemoryTemperatureSensorCount(m_dwGpu);
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetVRMTemperatureSensorCount()
{
	return m_lpATIADLWrapper->GetVRMTemperatureSensorCount(m_dwGpu);
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationATIADL::GetCurrentCoreTemperature(int index)
{
	LONG dwTemperature;

	if (m_lpATIADLWrapper->GetCurrentCoreTemperature(m_dwGpu, index, &dwTemperature))
		return dwTemperature;

	return 0;
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationATIADL::GetCurrentMemoryTemperature(int index)
{
	LONG dwTemperature;

	if (m_lpATIADLWrapper->GetCurrentMemoryTemperature(m_dwGpu, index, &dwTemperature))
		return dwTemperature;

	return 0;
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationATIADL::GetCurrentVRMTemperature(int index)
{
	LONG dwTemperature;

	if (m_lpATIADLWrapper->GetCurrentVRMTemperature(m_dwGpu, index, &dwTemperature))
		return dwTemperature;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetFanCount()
{
	return 1;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsFanSpeedReportingSupported(int index)
{
	return m_lpATIADLWrapper->GetCurrentFanSpeed(m_dwGpu, NULL);
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsFanSpeedRPMReportingSupported(int index)
{
	return m_lpATIADLWrapper->GetCurrentFanSpeedRPM(m_dwGpu, NULL);
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentFanSpeed(int index)
{
	DWORD dwFanSpeed;

	if (m_lpATIADLWrapper->GetCurrentFanSpeed(m_dwGpu, &dwFanSpeed))
		return dwFanSpeed;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentFanSpeedRPM(int index)
{
	DWORD dwFanSpeedRPM;

	if (m_lpATIADLWrapper->GetCurrentFanSpeedRPM(m_dwGpu, &dwFanSpeedRPM))
		return dwFanSpeedRPM;

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsGraphicsEngineUsageReportingSupported()
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsMemoryControllerUsageReportingSupported()
{
	return m_lpATIADLWrapper->GetCurrentMemoryControllerUsage(m_dwGpu, NULL);
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsVideoEngineUsageReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsBusUsageReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentGraphicsEngineUsage()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_ACTIVITY);

	if (lpData)
		return lpData->dwData;

	DWORD dwEngineClock	= 0;
	DWORD dwMemoryClock	= 0;
	DWORD dwVDDC		= 0;
	DWORD dwActivity	= 0;

	if (m_lpATIADLWrapper->GetCurrentActivity(m_dwGpu, &dwEngineClock, &dwMemoryClock, &dwVDDC, &dwActivity))
	{
		SetCacheData(CACHE_ID_ENGINE_CLOCK	, dwEngineClock	);
		SetCacheData(CACHE_ID_MEMORY_CLOCK	, dwMemoryClock	);
		SetCacheData(CACHE_ID_VDDC			, dwVDDC		);
		SetCacheData(CACHE_ID_ACTIVITY		, dwActivity	);

		return dwActivity;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentMemoryControllerUsage()
{
	DWORD dwUsage;

	if (m_lpATIADLWrapper->GetCurrentMemoryControllerUsage(m_dwGpu, &dwUsage))
		return dwUsage;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentVideoEngineUsage()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentBusUsage()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsVideomemoryUsageReportingSupported()
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetTotalVideomemory()
{
	if (m_dwTotalVideomemory)
		return m_dwTotalVideomemory;

	if (m_lpATIADLWrapper->GetTotalVideomemory(m_dwGpu, &m_dwTotalVideomemory))
		return m_dwTotalVideomemory;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentVideomemoryUsage()
{
	return GetD3DKMTVideomemoryUsage(FALSE, TRUE, 0);
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsRelPowerReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsAbsPowerReportingSupported()
{
	return m_lpATIADLWrapper->GetCurrentAbsPower(m_dwGpu, NULL);
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentRelPower()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetCurrentAbsPower()
{
	DWORD dwPower;

	if (m_lpATIADLWrapper->GetCurrentAbsPower(m_dwGpu, &dwPower))
		return dwPower;

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsEffectiveCoreClockReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationATIADL::IsEffectiveMemoryClockReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetEffectiveCoreClock()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationATIADL::GetEffectiveMemoryClock()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
