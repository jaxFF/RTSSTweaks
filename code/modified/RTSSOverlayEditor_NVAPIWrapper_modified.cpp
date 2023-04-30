// NVAPIWrapper.cpp: implementation of the CNVAPIWrapper class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "NVAPIWrapper.h"

#include "Log.h"
//////////////////////////////////////////////////////////////////////
#pragma comment(lib, "nvapi.lib") 
//////////////////////////////////////////////////////////////////////
CNVAPIWrapper::CNVAPIWrapper()
{
	m_dwVersion			= 0;
	ZeroMemory(&m_szBranch, sizeof(m_szBranch));

	m_dwGpuCount		= 0;

	ZeroMemory(m_hGpu	, sizeof(m_hGpu));
	ZeroMemory(m_dwBus	, sizeof(m_dwBus));
	ZeroMemory(m_dwDev	, sizeof(m_dwDev));
	ZeroMemory(m_dwCaps	, sizeof(m_dwCaps));
}
//////////////////////////////////////////////////////////////////////
CNVAPIWrapper::~CNVAPIWrapper()
{
	Uninit();
}
//////////////////////////////////////////////////////////////////////
DWORD CNVAPIWrapper::GetGpuCount()
{
	return m_dwGpuCount;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetLocation(DWORD dwGpu, DWORD* lpBus, DWORD* lpDev, DWORD* lpFn)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if (lpBus)
		*lpBus = m_dwBus[dwGpu];

	if (lpDev)
		*lpDev = m_dwDev[dwGpu];

	if (lpFn)
		*lpFn = 0;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
#pragma warning (disable: 4996) 
//////////////////////////////////////////////////////////////////////
void CNVAPIWrapper::Init()
{
	APPEND_LOG("Initializing NVAPI wrapper");

	//init API

	NvAPI_Initialize();

	//enumerate traditional and compute GPUs

	if (NvAPI_EnumPhysicalGPUs(m_hGpu, &m_dwGpuCount) == NVAPI_OK)
	{
		NvPhysicalGpuHandle tccGpuArr[NVAPI_MAX_PHYSICAL_GPUS]	= { 0 };
		NvU32				tccGpuArrSize						= 0;

		if (NvAPI_EnumTCCPhysicalGPUs(tccGpuArr, &tccGpuArrSize) == NVAPI_OK)
		{
			if (tccGpuArrSize)
			{
				CopyMemory(m_hGpu + m_dwGpuCount, tccGpuArr, tccGpuArrSize * sizeof(NvPhysicalGpuHandle));

				m_dwGpuCount += tccGpuArrSize;
			}
		}
	}
	else
		NvAPI_EnumTCCPhysicalGPUs(m_hGpu, &m_dwGpuCount);

	if (m_dwGpuCount)
	{
		APPEND_LOG1("%d NVIDIA GPUs detected", m_dwGpuCount);
	}
	else
	{
		APPEND_LOG("There are no NVIDIA GPUs detected");

		return;
	}

	//init driver and branch version

	if (NvAPI_SYS_GetDriverAndBranchVersion(&m_dwVersion, m_szBranch) != NVAPI_OK)
		//use deprecated driver and branch version detection for legacy drivers
	{
		NvDisplayHandle hDisplay;

		if (NvAPI_EnumNvidiaDisplayHandle(0, &hDisplay) == NVAPI_OK)
		{
			NV_DISPLAY_DRIVER_VERSION driverVersion;
			NVAPI_INIT_STRUCT(driverVersion, 1);

			if (NvAPI_GetDisplayDriverVersion(hDisplay, &driverVersion))
			{
				 m_dwVersion = driverVersion.drvVersion;

				 strcpy_s(m_szBranch, sizeof(m_szBranch), driverVersion.szBuildBranchString);
			}
		}
	}

	APPEND_LOG2("NVIDIA driver version %d.%02d", m_dwVersion / 100, m_dwVersion % 100);

	//init GPU locations

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		if ((NvAPI_GPU_GetBusId		(m_hGpu[dwGpu], &m_dwBus[dwGpu]) == NVAPI_OK) &&
			(NvAPI_GPU_GetBusSlotId	(m_hGpu[dwGpu], &m_dwDev[dwGpu]) == NVAPI_OK))
		{
		}
		else
		{
			//location decoding from GPUID on legacy display drivers is not supported
		}
	}

	//sort enumerated GPUs by location

	NvPhysicalGpuHandle	hGpu[NVAPIWRAPPER_MAX_GPU];

	DWORD				dwBus[NVAPIWRAPPER_MAX_GPU];
	DWORD				dwDev[NVAPIWRAPPER_MAX_GPU];

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		DWORD dwGpu0 = NVAPIWRAPPER_INVALID_GPU;

		for (DWORD dwGpu1=0; dwGpu1<m_dwGpuCount; dwGpu1++)
		{
			if (m_dwBus[dwGpu1])
			{
				if (dwGpu0 != NVAPIWRAPPER_INVALID_GPU)
				{
					DWORD dwGpuId0 = (m_dwBus[dwGpu0]<<16) | (m_dwDev[dwGpu0]<<8);
					DWORD dwGpuId1 = (m_dwBus[dwGpu1]<<16) | (m_dwDev[dwGpu1]<<8);

					if (dwGpuId1 < dwGpuId0)
						dwGpu0 = dwGpu1;
				}
				else
					dwGpu0 = dwGpu1;
			}
		}

		if (dwGpu0 != NVAPIWRAPPER_INVALID_GPU)
		{
			hGpu[dwGpu]			= m_hGpu[dwGpu0];
			dwBus[dwGpu]		= m_dwBus[dwGpu0];
			dwDev[dwGpu]		= m_dwDev[dwGpu0];

			m_hGpu[dwGpu0]		= 0;
			m_dwBus[dwGpu0]		= 0;
			m_dwDev[dwGpu0]		= 0;
		}
	}

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		m_hGpu[dwGpu]			= hGpu[dwGpu];
		m_dwBus[dwGpu]			= dwBus[dwGpu];
		m_dwDev[dwGpu]			= dwDev[dwGpu];
	}

	//init GPU caps

	//NOTE: Some functionality (e.g. fan or voltage monitoring) use different APIs depending on GPU family, so we use capabilities 
	//bitmask to determine which subset of API calls is supported by GPU

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		CString strName;
		GetFullName(dwGpu, strName);

		APPEND_LOG5("GPU%d on bus %d, dev %d, fn %d : %s", dwGpu, m_dwBus[dwGpu], m_dwDev[dwGpu], 0, strName);

		m_dwCaps[dwGpu] = 0;

		//NVAPIWRAPPER_CAPS_GpuGetAllClockFrequencies

		NV_GPU_CLOCK_FREQUENCIES_V2 clocks;
		NVAPI_INIT_STRUCT(clocks, 2)
    
		clocks.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;

		if (NvAPI_GPU_GetAllClockFrequencies(m_hGpu[dwGpu], &clocks) == NVAPI_OK)
		{
			m_dwCaps[dwGpu] |= NVAPIWRAPPER_CAPS_GpuGetAllClockFrequencies;

			APPEND_LOG("NVAPIWRAPPER_CAPS_GpuGetAllClockFrequencies");
		}

		//NVAPIWRAPPER_CAPS_GpuGetVoltageDomainsStatus

#ifdef NVAPI_NDA_DEVELOPER_SDK_AVAILABLE
		NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 status;
		NVAPI_INIT_STRUCT(status, 1);

		if (NvAPI_GPU_GetVoltageDomainsStatus(m_hGpu[dwGpu], &status) == NVAPI_OK)
		{
			m_dwCaps[dwGpu] |= NVAPIWRAPPER_CAPS_GpuGetVoltageDomainsStatus;

			APPEND_LOG("NVAPIWRAPPER_CAPS_GpuGetVoltageDomainsStatus");
		}
#endif
		//NVAPIWRAPPER_CAPS_GpuClientFanCoolersGetStatus

		DWORD dwCoolerCount;

		if (ClientFanCoolers_GetStatus(m_hGpu[dwGpu], &dwCoolerCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
		{
			if (dwCoolerCount)
			{
				m_dwCaps[dwGpu] |= NVAPIWRAPPER_CAPS_GpuClientFanCoolersGetStatus;

				APPEND_LOG("NVAPIWRAPPER_CAPS_GpuClientFanCoolersGetStatus");
			}
		}

		//NVAPIWRAPPER_CAPS_GpuClientVoltRailsGetStatus

		DWORD dwStatus;

		if (ClientVoltRails_GetStatus(m_hGpu[dwGpu], &dwStatus))
		{
			if (dwStatus)
			{
				m_dwCaps[dwGpu] |= NVAPIWRAPPER_CAPS_GpuClientVoltRailsGetStatus;

				APPEND_LOG("NVAPIWRAPPER_CAPS_GpuClientVoltRailsGetStatus");
			}
		}

		//NVAPIWRAPPER_CAPS_GpuPowerMonitorGetStatus

		{
			NV_GPU_POWER_MONITOR_GET_INFO_V2 info;
			NVAPI_INIT_STRUCT(info, 1);

			if (m_interface.NvAPI_GPU_PowerMonitorGetInfo(m_hGpu[dwGpu], &info) == NVAPI_OK)
			{
				if (info.bSupported)
				{
					m_dwCaps[dwGpu] |= NVAPIWRAPPER_CAPS_GpuPowerMonitorGetStatus;

					APPEND_LOG("NVAPIWRAPPER_CAPS_GpuPowerMonitorGetStatus");
				}
			}
		}

		//NVAPIWRAPPER_CAPS_GpuThermChannelGetStatus

		{
			NV_GPU_THERMAL_THERM_CHANNEL_INFO_PARAMS_V2 info;
			NVAPI_INIT_STRUCT(info, 2);

			if (m_interface.NvAPI_GPU_ThermChannelGetInfo(m_hGpu[dwGpu], &info) == NVAPI_OK)
			{
				if (info.channelMask)
				{
					m_dwCaps[dwGpu]	|= NVAPIWRAPPER_CAPS_GpuThermChannelGetStatus;

					APPEND_LOG("NVAPIWRAPPER_CAPS_GpuThermChannelGetStatus");
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
#pragma warning (default: 4996) 
//////////////////////////////////////////////////////////////////////
void CNVAPIWrapper::Uninit()
{
	NvAPI_Unload();
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::ClientFanCoolers_IsSupported(DWORD dwGpu)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	return (m_dwCaps[dwGpu] & NVAPIWRAPPER_CAPS_GpuClientFanCoolersGetStatus) != 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::ClientFanCoolers_GetStatus(NvPhysicalGpuHandle hGpu, DWORD* lpCount, DWORD* lpTach0, DWORD* lpMin0, DWORD* lpMax0, DWORD* lpTarget0, DWORD* lpTach1, DWORD* lpMin1, DWORD* lpMax1, DWORD* lpTarget1)
{
#ifdef NVAPI_NDA_DEVELOPER_SDK_AVAILABLE
    NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1 status;
	NVAPI_INIT_STRUCT(status, 1);

	if (NvAPI_GPU_ClientFanCoolersGetStatus(hGpu, &status) == NVAPI_OK)
	{
		if (lpCount)
			*lpCount = status.numCoolers;

		if (status.numCoolers > 0)
		{
			if (lpTach0)
				*lpTach0 = status.coolers[0].rpmCurr;

			if (lpMin0)
				*lpMin0 = status.coolers[0].levelMin;

			if (lpMax0)
				*lpMax0 = status.coolers[0].levelMax;

			if (lpTarget0)
				*lpTarget0 = status.coolers[0].levelTarget;

			if (status.numCoolers > 1)
			{
				if (lpTach1)
					*lpTach1 = status.coolers[1].rpmCurr;

				if (lpMin1)
					*lpMin1 = status.coolers[1].levelMin;

				if (lpMax1)
					*lpMax1 = status.coolers[1].levelMax;

				if (lpTarget1)
					*lpTarget1 = status.coolers[1].levelTarget;
			}
		}

		return TRUE;
	}
#endif

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetCurrentClocks(DWORD dwGpu, DWORD* lpCore, DWORD* lpMem, DWORD* lpShader)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if ((m_dwCaps[dwGpu] & NVAPIWRAPPER_CAPS_GpuGetAllClockFrequencies) != 0)
	{
		NV_GPU_CLOCK_FREQUENCIES_V2 clocks;
		NVAPI_INIT_STRUCT(clocks, 2)
    
		clocks.ClockType = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;

		if (NvAPI_GPU_GetAllClockFrequencies(m_hGpu[dwGpu], &clocks) == NVAPI_OK)
		{
			if (lpCore)
				*lpCore = clocks.domain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].frequency;

			if (lpMem)
				*lpMem = clocks.domain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].frequency;

			if (lpShader)
				*lpShader = clocks.domain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].frequency;

			return TRUE;
		}
	}

	//P-state based clock monitoring for legacy cards is not supported

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::ClientVoltRails_GetStatus(NvPhysicalGpuHandle hGpu, DWORD* lpStatus)
{
#ifdef NVAPI_NDA_DEVELOPER_SDK_AVAILABLE
	NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1 status;
	NVAPI_INIT_STRUCT(status, 1);

	if (NvAPI_GPU_ClientVoltRailsGetStatus(hGpu, &status) == NVAPI_OK)
	{
		for (DWORD iRail=0; iRail<NV_GPU_CLIENT_VOLT_DOMAIN_MAX_ENTRIES; iRail++)
		{
			if (status.rails[iRail].railId == NV_GPU_CLIENT_VOLT_DOMAIN_CORE)
			{
				if (lpStatus)
					*lpStatus = status.rails[iRail].currVoltuV / 1000;

				return TRUE;
			}
		}
	}
#endif

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetCurrentVoltage(DWORD dwGpu, DWORD* lpVoltage)
{
#ifdef NVAPI_NDA_DEVELOPER_SDK_AVAILABLE
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if ((m_dwCaps[dwGpu] & NVAPIWRAPPER_CAPS_GpuClientVoltRailsGetStatus) != 0)
	{
		DWORD dwStatus = 0;

		if (ClientVoltRails_GetStatus(m_hGpu[dwGpu], &dwStatus))
		{
			if (dwStatus)
			{
				if (lpVoltage)
					*lpVoltage = dwStatus;

				return TRUE;
			}
		}
	}

	if ((m_dwCaps[dwGpu] & NVAPIWRAPPER_CAPS_GpuGetVoltageDomainsStatus) != 0)
	{
		NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 status;
		NVAPI_INIT_STRUCT(status, 1);

		if (NvAPI_GPU_GetVoltageDomainsStatus(m_hGpu[dwGpu], &status))
		{
			for (DWORD iDomain=0; iDomain<status.numDomains; iDomain++)
			{
				if (status.domains[iDomain].domainId == NVAPI_GPU_PERF_VOLTAGE_INFO_DOMAIN_CORE)
				{
					if (lpVoltage)
					{
						*lpVoltage = status.domains[iDomain].voltageuV / 1000;

						return TRUE;
					}
				}
			}
		}
	}

	//P-state based VID monitoring for legacy cards is not supported
#endif

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetTemperature(NV_GPU_THERMAL_SETTINGS_V2* lpThermalSettings, DWORD dwTarget, LONG* lpTemperature)
{
	for (DWORD dwSensor=0; dwSensor<lpThermalSettings->count; dwSensor++)
	{
		if (lpThermalSettings->sensor[dwSensor].target == dwTarget)
		{
			if (lpTemperature)
				*lpTemperature = lpThermalSettings->sensor[dwSensor].currentTemp;

			return TRUE;
		}
	}

	if (lpTemperature)
		*lpTemperature = NVAPIWRAPPER_INVALID_TEMPERATURE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetCurrentTemperatures(DWORD dwGpu, LONG* lpCore, LONG* lpMemory, LONG* lpBoard)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	NV_GPU_THERMAL_SETTINGS_V2 thermalSettings;
	NVAPI_INIT_STRUCT(thermalSettings, 2);

	if (NvAPI_GPU_GetThermalSettings(m_hGpu[dwGpu], NVAPI_THERMAL_TARGET_ALL, (NV_GPU_THERMAL_SETTINGS*)&thermalSettings) == NVAPI_OK)
	{
		GetTemperature(&thermalSettings, NVAPI_THERMAL_TARGET_GPU	, lpCore	);
		GetTemperature(&thermalSettings, NVAPI_THERMAL_TARGET_MEMORY, lpMemory	);
		GetTemperature(&thermalSettings, NVAPI_THERMAL_TARGET_BOARD	, lpBoard	);

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetTemperature(NV_GPU_THERMAL_THERM_CHANNEL_INFO_PARAMS_V2* lpInfo, NV_GPU_THERMAL_THERM_CHANNEL_STATUS_PARAMS_V2* lpStatus, DWORD dwType, LONG* lpTemperature)
{
	DWORD dwChannel = lpInfo->priChIdx[dwType];

	if (dwChannel < NV_GPU_THERMAL_THERM_CHANNEL_MAX)
	{
		if (lpTemperature)
			*lpTemperature = lpStatus->channel[dwChannel] / 256;

		return TRUE;
	}

	if (lpTemperature)
		*lpTemperature = NVAPIWRAPPER_INVALID_TEMPERATURE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetCurrentTemperaturesEx(DWORD dwGpu, LONG* lpCore, LONG* lpHotspot, LONG* lpBoard, LONG* lpMemory, LONG* lpPower)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if ((m_dwCaps[dwGpu] & NVAPIWRAPPER_CAPS_GpuThermChannelGetStatus) != 0)
	{
		NV_GPU_THERMAL_THERM_CHANNEL_INFO_PARAMS_V2 info;
		NVAPI_INIT_STRUCT(info, 2);

		if (m_interface.NvAPI_GPU_ThermChannelGetInfo(m_hGpu[dwGpu], &info) == NVAPI_OK)
		{
			if (info.channelMask)
			{
				NV_GPU_THERMAL_THERM_CHANNEL_STATUS_PARAMS_V2 status;
				NVAPI_INIT_STRUCT(status, 2);

				status.channelMask = info.channelMask;

				if (m_interface.NvAPI_GPU_ThermChannelGetStatus(m_hGpu[dwGpu], &status) == NVAPI_OK)
				{
					GetTemperature(&info, &status, NV_GPU_THERMAL_THERM_CHANNEL_TYPE_GPU_AVG	, lpCore	);
					GetTemperature(&info, &status, NV_GPU_THERMAL_THERM_CHANNEL_TYPE_GPU_MAX	, lpHotspot	);
					GetTemperature(&info, &status, NV_GPU_THERMAL_THERM_CHANNEL_TYPE_BOARD		, lpBoard	);
					GetTemperature(&info, &status, NV_GPU_THERMAL_THERM_CHANNEL_TYPE_MEMORY		, lpMemory	);
					GetTemperature(&info, &status, NV_GPU_THERMAL_THERM_CHANNEL_TYPE_PWR_SUPPLY	, lpPower	);

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CNVAPIWrapper::GetFanCount(DWORD dwGpu)
{
#ifdef NVAPI_NDA_DEVELOPER_SDK_AVAILABLE
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if (ClientFanCoolers_IsSupported(dwGpu))
	{
		DWORD dwCount = 0;

		if (ClientFanCoolers_GetStatus(m_hGpu[dwGpu], &dwCount, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL))
			return dwCount;
	}

	return 1;
#else
	return 0;
#endif
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetCurrentFanSpeeds(DWORD dwGpu, DWORD* lpSpeed, DWORD* lpSpeedRPM, DWORD* lpSpeed2, DWORD* lpSpeedRPM2)
{
#ifdef NVAPI_NDA_DEVELOPER_SDK_AVAILABLE
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if (ClientFanCoolers_IsSupported(dwGpu))
	{
		return ClientFanCoolers_GetStatus(m_hGpu[dwGpu], NULL, lpSpeedRPM, NULL, NULL, lpSpeed, lpSpeedRPM2, NULL, NULL, lpSpeed2);
	}
	else
	{
		NV_GPU_GETCOOLER_SETTINGS_V3 coolerSettings;
		NVAPI_INIT_STRUCT(coolerSettings, 3);

		if (NvAPI_GPU_GetCoolerSettings(m_hGpu[dwGpu], NVAPI_COOLER_TARGET_ALL, (NV_GPU_GETCOOLER_SETTINGS*)&coolerSettings) == NVAPI_OK)
		{
			if (!coolerSettings.count)
				return FALSE;

			if (lpSpeed)
				*lpSpeed = coolerSettings.cooler[0].currentLevel;

			if (lpSpeedRPM)
				*lpSpeedRPM = coolerSettings.cooler[0].tachometer.bSupported ? coolerSettings.cooler[0].tachometer.speedRPM : 0;

			return TRUE;
		}
	}
#endif

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetCurrentRelPower(DWORD dwGpu, DWORD* lpPower)
{
#ifdef NVAPI_NDA_DEVELOPER_SDK_AVAILABLE
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1 status;
	NVAPI_INIT_STRUCT(status, 1);

	if (NvAPI_GPU_ClientPowerTopologyGetStatus(m_hGpu[dwGpu], &status) == NVAPI_OK)
	{
		for (int iChannel=0; iChannel<status.numChannels; iChannel++)
		{
			if (status.channels[iChannel].channelId == NV_GPU_CLIENT_POWER_TOPOLOGY_CHANNEL_ID_NORMALIZED_TOTAL_POWER)
			{
				if (lpPower)
					*lpPower = status.channels[iChannel].power.mp / 1000;

				return TRUE;
			}
		}
	}
#endif

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
#ifndef NVAPI_GPU_UTILIZATION_DOMAIN_GPU
#define NVAPI_GPU_UTILIZATION_DOMAIN_GPU 0
#endif
#ifndef NVAPI_GPU_UTILIZATION_DOMAIN_FB
#define NVAPI_GPU_UTILIZATION_DOMAIN_FB 1
#endif
#ifndef NVAPI_GPU_UTILIZATION_DOMAIN_VID
#define NVAPI_GPU_UTILIZATION_DOMAIN_VID 2
#endif
#ifndef NVAPI_GPU_UTILIZATION_DOMAIN_BUS
#define NVAPI_GPU_UTILIZATION_DOMAIN_BUS 3
#endif
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetCurrentUtilizations(DWORD dwGpu, DWORD* lpGraphicsEngine, DWORD* lpFramebuffer, DWORD* lpVideoEngine, DWORD* lpBus)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	NV_GPU_DYNAMIC_PSTATES_INFO_EX info;
	NVAPI_INIT_STRUCT(info, 1);

	if (NvAPI_GPU_GetDynamicPstatesInfoEx(m_hGpu[dwGpu], &info) == NVAPI_OK)
	{
		if (lpGraphicsEngine)
		{
			if (info.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_GPU].bIsPresent)
				*lpGraphicsEngine = info.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_GPU].percentage;
			else
				*lpGraphicsEngine = NVAPIWRAPPER_INVALID_UTILIZATION;
		}

		if (lpFramebuffer)
		{
			if (info.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_FB].bIsPresent)
				*lpFramebuffer = info.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_FB].percentage;
			else
				*lpFramebuffer = NVAPIWRAPPER_INVALID_UTILIZATION;
		}

		if (lpVideoEngine)
		{
			if (info.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_VID].bIsPresent)
				*lpVideoEngine = info.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_VID].percentage;
			else
				*lpVideoEngine = NVAPIWRAPPER_INVALID_UTILIZATION;
		}

		if (lpBus)
		{
			if (info.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_BUS].bIsPresent)
				*lpBus = info.utilization[NVAPI_GPU_UTILIZATION_DOMAIN_BUS].percentage;
			else
				*lpBus = NVAPIWRAPPER_INVALID_UTILIZATION;
		}

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetFullName(DWORD dwGpu, CString& strName)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	NvAPI_ShortString szName;

	if (NvAPI_GPU_GetFullName(m_hGpu[dwGpu], szName) == NVAPI_OK)
	{
		strName = szName;

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetMemoryInfo(DWORD dwGpu, DWORD* lpDedicated, DWORD* lpDedicatedAvailable, DWORD* lpSystem, DWORD* lpShared, DWORD* lpDedicatedAvailableCur)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	NV_DISPLAY_DRIVER_MEMORY_INFO_V2 info;
	NVAPI_INIT_STRUCT(info, 2);
	
	if (NvAPI_GPU_GetMemoryInfo(m_hGpu[dwGpu], (NV_DISPLAY_DRIVER_MEMORY_INFO*)&info) == NVAPI_OK)
	{
		if (lpDedicated)
			*lpDedicated = info.dedicatedVideoMemory;

		if (lpDedicatedAvailable)
			*lpDedicatedAvailable = info.availableDedicatedVideoMemory;

		if (lpSystem)
			*lpSystem = info.systemVideoMemory;

		if (lpShared)
			*lpShared = info.sharedSystemMemory;

		if (lpDedicatedAvailableCur)
			*lpDedicatedAvailableCur = info.curAvailableDedicatedVideoMemory;

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CNVAPIWrapper::GetVersion()
{
	return m_dwVersion;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetCurrentAbsPower(DWORD dwGpu, DWORD* lpPower)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if ((m_dwCaps[dwGpu] & NVAPIWRAPPER_CAPS_GpuPowerMonitorGetStatus) != 0)
	{
		NV_GPU_POWER_MONITOR_GET_STATUS_V2 status;
		NVAPI_INIT_STRUCT(status, 1);

		if (m_interface.NvAPI_GPU_PowerMonitorGetStatus(m_hGpu[dwGpu], &status) == NVAPI_OK)
		{
			if (lpPower)
				*lpPower = status.totalGpuPowermW;

			return TRUE;
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CNVAPIWrapper::GetEffectiveClocks(DWORD dwGpu, DWORD* lpCore, DWORD* lpMem, DWORD* lpShader)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	NV_GPU_CLOCK_INFO_V2 info;
	NVAPI_INIT_STRUCT(info, 2);

	if (m_interface.NvAPI_GPU_GetAllClocks(m_hGpu[dwGpu], &info) == NVAPI_OK)
	{
		if (lpCore)
			*lpCore = info.extendedDomain[NVAPI_GPU_PUBLIC_CLOCK_GRAPHICS].effectiveFrequency;

		if (lpMem)
			*lpMem = info.extendedDomain[NVAPI_GPU_PUBLIC_CLOCK_MEMORY].effectiveFrequency;

		if (lpShader)
			*lpShader = info.extendedDomain[NVAPI_GPU_PUBLIC_CLOCK_PROCESSOR].effectiveFrequency;

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
