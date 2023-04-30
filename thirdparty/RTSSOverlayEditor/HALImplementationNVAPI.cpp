// HALImplementationNVAPI.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HALImplementationNVAPI.h"
#include "Log.h"
//////////////////////////////////////////////////////////////////////
#define CACHE_ID_CORE_CLOCK											0
#define CACHE_ID_MEMORY_CLOCK										1

#define CACHE_ID_FAN_SPEED											2
#define CACHE_ID_FAN_SPEED_RPM										3
#define CACHE_ID_FAN_SPEED_2										4
#define CACHE_ID_FAN_SPEED_RPM_2									5

#define CACHE_ID_GRAPHICS_ENGINE_USAGE								6
#define CACHE_ID_MEMORY_CONTROLLER_USAGE							7
#define CACHE_ID_VIDEO_ENGINE_USAGE									8
#define CACHE_ID_BUS_USAGE											9

#define CACHE_ID_HOTSPOT_TEMPERATURE								10
#define CACHE_ID_MEMORY_TEMPERATURE									11

#define CACHE_ID_EFFECTIVE_CORE_CLOCK								12
#define CACHE_ID_EFFECTIVE_MEMORY_CLOCK								13
//////////////////////////////////////////////////////////////////////
CHALImplementationNVAPI::CHALImplementationNVAPI()
{
	m_lpNVAPIWrapper	= NULL;
	m_dwGpu				= 0;
}
//////////////////////////////////////////////////////////////////////
CHALImplementationNVAPI::~CHALImplementationNVAPI()
{
}
//////////////////////////////////////////////////////////////////////
void CHALImplementationNVAPI::SetNVAPIWrapper(CNVAPIWrapper* lpNVAPIWrapper)
{
	m_lpNVAPIWrapper = lpNVAPIWrapper;
}
//////////////////////////////////////////////////////////////////////
void CHALImplementationNVAPI::SetGpu(DWORD dwGpu)
{
	m_dwGpu = dwGpu;
}
//////////////////////////////////////////////////////////////////////
CString	CHALImplementationNVAPI::GetDriverStr()
{
	DWORD dwVersion = m_lpNVAPIWrapper->GetVersion();

	CString strVersion;
	strVersion.Format("%d.%02d", dwVersion / 100, dwVersion % 100);

	return strVersion;
}
//////////////////////////////////////////////////////////////////////
CString CHALImplementationNVAPI::GetName()
{
	CString strName;

	if (m_lpNVAPIWrapper->GetFullName(m_dwGpu, strName))
		return strName;

	return "";
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::GetLocation(DWORD* lpBus, DWORD* lpDev, DWORD* lpFn)
{
	return m_lpNVAPIWrapper->GetLocation(m_dwGpu, lpBus, lpDev, lpFn);
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsCoreClockReportingSupported()
{
	return GetCurrentCoreClock() != 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsMemoryClockReportingSupported()
{
	return GetCurrentMemoryClock() != 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentCoreClock()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_CORE_CLOCK);

	if (lpData)
		return lpData->dwData;

	DWORD dwCoreClock	= 0;
	DWORD dwMemoryClock	= 0;

	if (m_lpNVAPIWrapper->GetCurrentClocks(m_dwGpu, &dwCoreClock, &dwMemoryClock, NULL))
	{
		SetCacheData(CACHE_ID_CORE_CLOCK	, dwCoreClock	);
		SetCacheData(CACHE_ID_MEMORY_CLOCK	, dwMemoryClock	);

		return dwCoreClock;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentMemoryClock()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_MEMORY_CLOCK);

	if (lpData)
		return lpData->dwData;

	DWORD dwCoreClock	= 0;
	DWORD dwMemoryClock	= 0;

	if (m_lpNVAPIWrapper->GetCurrentClocks(m_dwGpu, &dwCoreClock, &dwMemoryClock, NULL))
	{
		SetCacheData(CACHE_ID_CORE_CLOCK	, dwCoreClock	);
		SetCacheData(CACHE_ID_MEMORY_CLOCK	, dwMemoryClock	);

		return dwMemoryClock;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsCoreVoltageReportingSupported()
{
	return m_lpNVAPIWrapper->GetCurrentVoltage(m_dwGpu, NULL);
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsMemoryVoltageReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentCoreVoltage()
{
	DWORD dwVoltage = 0;

	if (m_lpNVAPIWrapper->GetCurrentVoltage(m_dwGpu, &dwVoltage))
		return dwVoltage;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentMemoryVoltage()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCoreTemperatureSensorCount()
{
	if (m_lpNVAPIWrapper->GetCurrentTemperatures(m_dwGpu, NULL, NULL, NULL))
	{
		LONG dwTemperature = NVAPIWRAPPER_INVALID_TEMPERATURE;

		if (m_lpNVAPIWrapper->GetCurrentTemperaturesEx(m_dwGpu, NULL, &dwTemperature, NULL, NULL, NULL))
		{
			if (dwTemperature != NVAPIWRAPPER_INVALID_TEMPERATURE)
				return 2;
		}

		return 1;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetMemoryTemperatureSensorCount()
{
	LONG dwTemperature = NVAPIWRAPPER_INVALID_TEMPERATURE;

	if (m_lpNVAPIWrapper->GetCurrentTemperaturesEx(m_dwGpu, NULL, NULL, NULL, &dwTemperature, NULL))
	{
		if (dwTemperature != NVAPIWRAPPER_INVALID_TEMPERATURE)
			return 1;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetVRMTemperatureSensorCount()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationNVAPI::GetCurrentCoreTemperature(int index)
{
	LONG dwTemperature = 0;

	if (!index)
	{
		if (m_lpNVAPIWrapper->GetCurrentTemperatures(m_dwGpu, &dwTemperature, NULL, NULL))
			return dwTemperature;
	}
	else
	{
		LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_HOTSPOT_TEMPERATURE);

		if (lpData)
			return lpData->dwData;

		LONG dwHotspotTemperature	= NVAPIWRAPPER_INVALID_TEMPERATURE;
		LONG dwMemoryTemperature	= NVAPIWRAPPER_INVALID_TEMPERATURE;

		if (m_lpNVAPIWrapper->GetCurrentTemperaturesEx(m_dwGpu, NULL, &dwHotspotTemperature, NULL, &dwMemoryTemperature, NULL))
		{
			SetCacheData(CACHE_ID_HOTSPOT_TEMPERATURE	, dwHotspotTemperature	);
			SetCacheData(CACHE_ID_MEMORY_TEMPERATURE	, dwMemoryTemperature	);

			return dwHotspotTemperature;
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationNVAPI::GetCurrentMemoryTemperature(int index)
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_MEMORY_TEMPERATURE);

	if (lpData)
		return lpData->dwData;

	LONG dwHotspotTemperature	= NVAPIWRAPPER_INVALID_TEMPERATURE;
	LONG dwMemoryTemperature	= NVAPIWRAPPER_INVALID_TEMPERATURE;

	if (m_lpNVAPIWrapper->GetCurrentTemperaturesEx(m_dwGpu, NULL, &dwHotspotTemperature, NULL, &dwMemoryTemperature, NULL))
	{
		SetCacheData(CACHE_ID_HOTSPOT_TEMPERATURE	, dwHotspotTemperature	);
		SetCacheData(CACHE_ID_MEMORY_TEMPERATURE	, dwMemoryTemperature	);

		return dwMemoryTemperature;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationNVAPI::GetCurrentVRMTemperature(int index)
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetFanCount()
{
	return m_lpNVAPIWrapper->GetFanCount(m_dwGpu);
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsFanSpeedReportingSupported(int index)
{
	if (index < (int)GetFanCount())
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsFanSpeedRPMReportingSupported(int index)
{
	if (index < (int)GetFanCount())
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentFanSpeed(int index)
{
	switch (index)
	{
	case 0:
		{
			LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_FAN_SPEED);

			if (lpData)
				return lpData->dwData;
		}
		break;
	case 1:
		{
			LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_FAN_SPEED_2);

			if (lpData)
				return lpData->dwData;
		}
		break;
	}

	DWORD dwFanSpeed;
	DWORD dwFanSpeedRPM;
	DWORD dwFanSpeed2;
	DWORD dwFanSpeedRPM2;

	if (m_lpNVAPIWrapper->GetCurrentFanSpeeds(m_dwGpu, &dwFanSpeed, &dwFanSpeedRPM, &dwFanSpeed2, &dwFanSpeedRPM2))
	{
		SetCacheData(CACHE_ID_FAN_SPEED			, dwFanSpeed		);
		SetCacheData(CACHE_ID_FAN_SPEED_RPM		, dwFanSpeedRPM		);
		SetCacheData(CACHE_ID_FAN_SPEED_2		, dwFanSpeed2		);
		SetCacheData(CACHE_ID_FAN_SPEED_RPM_2	, dwFanSpeedRPM2	);

		switch (index)
		{
		case 0:
			return dwFanSpeed;
		case 1:
			return dwFanSpeed2;
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentFanSpeedRPM(int index)
{
	switch (index)
	{
	case 0:
		{
			LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_FAN_SPEED_RPM);

			if (lpData)
				return lpData->dwData;
		}
		break;
	case 1:
		{
			LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_FAN_SPEED_RPM_2);

			if (lpData)
				return lpData->dwData;
		}
		break;
	}

	DWORD dwFanSpeed;
	DWORD dwFanSpeedRPM;
	DWORD dwFanSpeed2;
	DWORD dwFanSpeedRPM2;

	if (m_lpNVAPIWrapper->GetCurrentFanSpeeds(m_dwGpu, &dwFanSpeed, &dwFanSpeedRPM, &dwFanSpeed2, &dwFanSpeedRPM2))
	{
		SetCacheData(CACHE_ID_FAN_SPEED			, dwFanSpeed		);
		SetCacheData(CACHE_ID_FAN_SPEED_RPM		, dwFanSpeedRPM		);
		SetCacheData(CACHE_ID_FAN_SPEED_2		, dwFanSpeed2		);
		SetCacheData(CACHE_ID_FAN_SPEED_RPM_2	, dwFanSpeedRPM2	);

		switch (index)
		{
		case 0:
			return dwFanSpeedRPM;
		case 1:
			return dwFanSpeedRPM2;
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsGraphicsEngineUsageReportingSupported()
{
	return GetCurrentGraphicsEngineUsage() != NVAPIWRAPPER_INVALID_UTILIZATION;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsMemoryControllerUsageReportingSupported()
{
	return GetCurrentMemoryControllerUsage() != NVAPIWRAPPER_INVALID_UTILIZATION;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsVideoEngineUsageReportingSupported()
{
	return GetCurrentVideoEngineUsage() != NVAPIWRAPPER_INVALID_UTILIZATION;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsBusUsageReportingSupported()
{
	return GetCurrentBusUsage() != NVAPIWRAPPER_INVALID_UTILIZATION;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentGraphicsEngineUsage()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_GRAPHICS_ENGINE_USAGE);

	if (lpData)
		return lpData->dwData;

	DWORD dwGraphicsEngineUsage		= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwMemoryControllerUsage	= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwVideoEngineUsage		= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwBusUsage				= NVAPIWRAPPER_INVALID_UTILIZATION;

	if (m_lpNVAPIWrapper->GetCurrentUtilizations(m_dwGpu, &dwGraphicsEngineUsage, &dwMemoryControllerUsage, &dwVideoEngineUsage, &dwBusUsage))
	{
		SetCacheData(CACHE_ID_GRAPHICS_ENGINE_USAGE			, dwGraphicsEngineUsage		);
		SetCacheData(CACHE_ID_MEMORY_CONTROLLER_USAGE		, dwMemoryControllerUsage	);
		SetCacheData(CACHE_ID_VIDEO_ENGINE_USAGE			, dwVideoEngineUsage		);
		SetCacheData(CACHE_ID_BUS_USAGE						, dwBusUsage				);

		return dwGraphicsEngineUsage;
	}

	return NVAPIWRAPPER_INVALID_UTILIZATION;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentMemoryControllerUsage()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_MEMORY_CONTROLLER_USAGE);

	if (lpData)
		return lpData->dwData;

	DWORD dwGraphicsEngineUsage		= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwMemoryControllerUsage	= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwVideoEngineUsage		= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwBusUsage				= NVAPIWRAPPER_INVALID_UTILIZATION;

	if (m_lpNVAPIWrapper->GetCurrentUtilizations(m_dwGpu, &dwGraphicsEngineUsage, &dwMemoryControllerUsage, &dwVideoEngineUsage, &dwBusUsage))
	{
		SetCacheData(CACHE_ID_GRAPHICS_ENGINE_USAGE			, dwGraphicsEngineUsage		);
		SetCacheData(CACHE_ID_MEMORY_CONTROLLER_USAGE		, dwMemoryControllerUsage	);
		SetCacheData(CACHE_ID_VIDEO_ENGINE_USAGE			, dwVideoEngineUsage		);
		SetCacheData(CACHE_ID_BUS_USAGE						, dwBusUsage				);

		return dwMemoryControllerUsage;
	}

	return NVAPIWRAPPER_INVALID_UTILIZATION;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentVideoEngineUsage()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_VIDEO_ENGINE_USAGE);

	if (lpData)
		return lpData->dwData;

	DWORD dwGraphicsEngineUsage		= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwMemoryControllerUsage	= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwVideoEngineUsage		= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwBusUsage				= NVAPIWRAPPER_INVALID_UTILIZATION;

	if (m_lpNVAPIWrapper->GetCurrentUtilizations(m_dwGpu, &dwGraphicsEngineUsage, &dwMemoryControllerUsage, &dwVideoEngineUsage, &dwBusUsage))
	{
		SetCacheData(CACHE_ID_GRAPHICS_ENGINE_USAGE			, dwGraphicsEngineUsage		);
		SetCacheData(CACHE_ID_MEMORY_CONTROLLER_USAGE		, dwMemoryControllerUsage	);
		SetCacheData(CACHE_ID_VIDEO_ENGINE_USAGE			, dwVideoEngineUsage		);
		SetCacheData(CACHE_ID_BUS_USAGE						, dwBusUsage				);

		return dwVideoEngineUsage;
	}

	return NVAPIWRAPPER_INVALID_UTILIZATION;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentBusUsage()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_BUS_USAGE);

	if (lpData)
		return lpData->dwData;

	DWORD dwGraphicsEngineUsage		= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwMemoryControllerUsage	= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwVideoEngineUsage		= NVAPIWRAPPER_INVALID_UTILIZATION;
	DWORD dwBusUsage				= NVAPIWRAPPER_INVALID_UTILIZATION;

	if (m_lpNVAPIWrapper->GetCurrentUtilizations(m_dwGpu, &dwGraphicsEngineUsage, &dwMemoryControllerUsage, &dwVideoEngineUsage, &dwBusUsage))
	{
		SetCacheData(CACHE_ID_GRAPHICS_ENGINE_USAGE			, dwGraphicsEngineUsage		);
		SetCacheData(CACHE_ID_MEMORY_CONTROLLER_USAGE		, dwMemoryControllerUsage	);
		SetCacheData(CACHE_ID_VIDEO_ENGINE_USAGE			, dwVideoEngineUsage		);
		SetCacheData(CACHE_ID_BUS_USAGE						, dwBusUsage				);

		return dwBusUsage;
	}

	return NVAPIWRAPPER_INVALID_UTILIZATION;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsVideomemoryUsageReportingSupported()
{
	return GetCurrentVideomemoryUsage() != NVAPIWRAPPER_INVALID_MEMORY_USAGE; 
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetTotalVideomemory()
{
	if (m_dwTotalVideomemory)
		return m_dwTotalVideomemory;

	if (m_lpNVAPIWrapper->GetMemoryInfo(m_dwGpu, &m_dwTotalVideomemory, NULL, NULL, NULL, NULL))
		return m_dwTotalVideomemory;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentVideomemoryUsage()
{
	DWORD	dwDedicated; 
	DWORD	dwDedicatedAvailable;
	DWORD	dwSystem;
	DWORD	dwShared; 
	DWORD	dwDedicatedAvailableCur;

	if (m_lpNVAPIWrapper->GetMemoryInfo(m_dwGpu, &dwDedicated, &dwDedicatedAvailable, &dwSystem, &dwShared, &dwDedicatedAvailableCur))
		return dwDedicated - dwDedicatedAvailableCur;
			//NOTE: dwDedicated - dwDedicatedAvailableCur includes chunk of memory reserved by NV driver and unavailable for allocation by system,
			//we can also use dwDedicatedAvailable - dwDedicatedAvailableCur to exclude this chunk from usage reporting

	return NVAPIWRAPPER_INVALID_MEMORY_USAGE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsRelPowerReportingSupported()
{
	return (m_lpNVAPIWrapper->GetCurrentRelPower(m_dwGpu, NULL));
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsAbsPowerReportingSupported()
{
	return (m_lpNVAPIWrapper->GetCurrentAbsPower(m_dwGpu, NULL));
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentRelPower()
{
	DWORD dwPower = 0;

	if (m_lpNVAPIWrapper->GetCurrentRelPower(m_dwGpu, &dwPower))
		return dwPower;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetCurrentAbsPower()
{
	DWORD dwPower = 0;

	if (m_lpNVAPIWrapper->GetCurrentAbsPower(m_dwGpu, &dwPower))
		return dwPower;

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsEffectiveCoreClockReportingSupported()
{
	return GetEffectiveCoreClock() != 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationNVAPI::IsEffectiveMemoryClockReportingSupported()
{
	return GetEffectiveMemoryClock() != 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetEffectiveCoreClock()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_EFFECTIVE_CORE_CLOCK);

	if (lpData)
		return lpData->dwData;

	DWORD dwCoreClock	= 0;
	DWORD dwMemoryClock	= 0;

	if (m_lpNVAPIWrapper->GetEffectiveClocks(m_dwGpu, &dwCoreClock, &dwMemoryClock, NULL))
	{
		SetCacheData(CACHE_ID_EFFECTIVE_CORE_CLOCK	, dwCoreClock	);
		SetCacheData(CACHE_ID_EFFECTIVE_MEMORY_CLOCK, dwMemoryClock	);

		return dwCoreClock;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationNVAPI::GetEffectiveMemoryClock()
{
	LPTIMESTAMPED_DATA lpData = GetCacheData(CACHE_ID_EFFECTIVE_MEMORY_CLOCK);

	if (lpData)
		return lpData->dwData;

	DWORD dwCoreClock	= 0;
	DWORD dwMemoryClock	= 0;

	if (m_lpNVAPIWrapper->GetEffectiveClocks(m_dwGpu, &dwCoreClock, &dwMemoryClock, NULL))
	{
		SetCacheData(CACHE_ID_EFFECTIVE_CORE_CLOCK	, dwCoreClock	);
		SetCacheData(CACHE_ID_EFFECTIVE_MEMORY_CLOCK, dwMemoryClock	);

		return dwMemoryClock;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
