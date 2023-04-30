// ATIADLWrapper.cpp: implementation of the CATIADLWrapper class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "ATIADLWrapper.h"
#include "Log.h"

#include <math.h>
//////////////////////////////////////////////////////////////////////
#define ALIGN(X,N) (X+N-1)&~(N-1)
//////////////////////////////////////////////////////////////////////
CATIADLWrapper::CATIADLWrapper()
{
	ZeroMemory(&m_adapters, sizeof(m_adapters));
	m_dwAdapterCount		= 0;

	m_dwGpuCount			= 0;
	ZeroMemory(&m_dwGpuBus	, sizeof(m_dwGpuBus	));
	ZeroMemory(&m_dwGpuDev	, sizeof(m_dwGpuDev	));
	ZeroMemory(&m_dwGpuFn	, sizeof(m_dwGpuFn	));

	ZeroMemory(&m_iOverdriveSupported		, sizeof(m_iOverdriveSupported		));
	ZeroMemory(&m_iOverdriveEnabled			, sizeof(m_iOverdriveEnabled		));
	ZeroMemory(&m_iOverdriveVersion			, sizeof(m_iOverdriveVersion		));

	m_dwActivityMonitoringDelay = 5;

	ZeroMemory(&m_bODNCaps					, sizeof(m_bODNCaps					));
	ZeroMemory(&m_odnCaps					, sizeof(m_odnCaps					));

	ZeroMemory(&m_bODNCapsX2				, sizeof(m_bODNCapsX2				));
	ZeroMemory(&m_odnCapsX2					, sizeof(m_odnCapsX2				));

	ZeroMemory(&m_iODNExtCapabilities		, sizeof(m_iODNExtCapabilities		));
	ZeroMemory(&m_iNumberOfODNExtFeatures	, sizeof(m_iNumberOfODNExtFeatures	));
	ZeroMemory(&m_odnExtSingleInitSettings	, sizeof(m_odnExtSingleInitSettings	));

	ZeroMemory(&m_bOD8InitSetting			, sizeof(m_bOD8InitSetting			));
	ZeroMemory(&m_od8InitSetting			, sizeof(m_od8InitSetting			));

	ZeroMemory(&m_iOD8Capabilities			, sizeof(m_iOD8Capabilities			));
	ZeroMemory(&m_iNumberOfOD8Features		, sizeof(m_iNumberOfOD8Features		));
	ZeroMemory(&m_od8SingleInitSettings		, sizeof(m_od8SingleInitSettings	));

	ZeroMemory(&m_bPMLogDataOutput			, sizeof(m_bPMLogDataOutput			));
	ZeroMemory(&m_pmLogDataOutput			, sizeof(m_pmLogDataOutput			));
	ZeroMemory(&m_dwPMLogDataTimestamp		, sizeof(m_dwPMLogDataTimestamp		));

	m_dwTimestamp			= 0;

	m_bMainControlCreated	= FALSE;
	m_bMainControlCreated2	= FALSE;
	m_context				= NULL;
}
//////////////////////////////////////////////////////////////////////
CATIADLWrapper::~CATIADLWrapper()
{
	Uninit();
}
//////////////////////////////////////////////////////////////////////
void* __stdcall CATIADLWrapper::ADL_Main_Memory_Alloc(int iSize)
{
    void* lpBuffer = new BYTE[iSize];

    return lpBuffer;
}
//////////////////////////////////////////////////////////////////////
void __stdcall CATIADLWrapper::ADL_Main_Memory_Free(void** lpBuffer)
{
	if (*lpBuffer)
	{
		delete [] *lpBuffer;

		*lpBuffer = NULL;
	}
}
//////////////////////////////////////////////////////////////////////
void CATIADLWrapper::Init()
{
	APPEND_LOG("Initializing ATIADL wrapper");

	BOOL bInitialized = FALSE;

	if (m_interface.Init())
		//init interface, enumerate ATIADL adapters and build GPUs array
	{
		if (ADL_OK == m_interface.ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1))
		{
			m_bMainControlCreated = TRUE;

			if (ADL_OK == m_interface.ADL2_Main_Control_Create(ADL_Main_Memory_Alloc, 1, &m_context))
				m_bMainControlCreated2 = TRUE;

			if (ADL_OK == m_interface.ADL_Adapter_NumberOfAdapters_Get((int*)&m_dwAdapterCount))
			{
				if (m_dwAdapterCount < ATIADLWRAPPER_MAX_ADAPTERS)
				{
					if (ADL_OK == m_interface.ADL_Adapter_AdapterInfo_Get(m_adapters, m_dwAdapterCount * sizeof(AdapterInfo)))
					{
						for (DWORD dwAdapter=0; dwAdapter<m_dwAdapterCount; dwAdapter++)
						{
							if (((m_adapters[dwAdapter].iVendorID & 0xFFFF) !=   1002) &&
								((m_adapters[dwAdapter].iVendorID & 0xFFFF) != 0x1002))
								//there is VendorID parsing bug inside ATIADL so ATI Vendor ID is 1002 dec instead of hex
								continue;

							DWORD dwBus = m_adapters[dwAdapter].iBusNumber;
							DWORD dwDev = m_adapters[dwAdapter].iDeviceNumber;
							DWORD dwFn	= m_adapters[dwAdapter].iFunctionNumber;

							BOOL bExistingGpu = FALSE;

							for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
								//ATIADL enumerates display adapters so there are multiple adapters representing the same physical GPU
								//and sharing the same location

								//So we manually build the list of physical GPUs having unique location
							{
								if ((dwBus == m_dwGpuBus[dwGpu]) &&
									(dwDev == m_dwGpuDev[dwGpu]) &&
									(dwFn  == m_dwGpuFn[dwGpu]))
								{
									bExistingGpu = TRUE;
									break;
								}
							}
							
							if (bExistingGpu)
								continue;
							
							m_dwGpuBus[m_dwGpuCount] = dwBus;
							m_dwGpuDev[m_dwGpuCount] = dwDev;
							m_dwGpuFn [m_dwGpuCount] = dwFn ;

							m_dwGpuCount++;
						}

						bInitialized = TRUE;
					}
				}
			}
	   }
	}

	if (m_dwAdapterCount)
	{
		APPEND_LOG1("%d ATIADL adapters detected", m_dwAdapterCount);
	}
	else
	{
		APPEND_LOG("There are no ATIADL adapters detected");
	}

	if (m_dwGpuCount)
	{
		APPEND_LOG1("%d ATIADL GPUs detected", m_dwGpuCount);
	}
	else
	{
		if (m_dwAdapterCount)
		{
			APPEND_LOG("There are no ATIADL GPUs detected");
		}

		Uninit();

		return;
	}

	//sort enumerated GPUs by location

	DWORD dwGpuBus[ATIADLWRAPPER_MAX_ADAPTERS];
	DWORD dwGpuDev[ATIADLWRAPPER_MAX_ADAPTERS];
	DWORD dwGpuFn [ATIADLWRAPPER_MAX_ADAPTERS];

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		DWORD dwGpu0 = ATIADLIWRAPPER_INVALID_GPU;

		for (DWORD dwGpu1=0; dwGpu1<m_dwGpuCount; dwGpu1++)
		{
			if (m_dwGpuBus[dwGpu1])
			{
				if (dwGpu0 != ATIADLIWRAPPER_INVALID_GPU)
				{
					DWORD dwGpuId0 = (m_dwGpuBus[dwGpu0]<<16) | (m_dwGpuDev[dwGpu0]<<8) | m_dwGpuFn[dwGpu0];
					DWORD dwGpuId1 = (m_dwGpuBus[dwGpu1]<<16) | (m_dwGpuDev[dwGpu1]<<8) | m_dwGpuFn[dwGpu1];

					if (dwGpuId1 < dwGpuId0)
						dwGpu0 = dwGpu1;
				}
				else
					dwGpu0 = dwGpu1;
			}
		}

		if (dwGpu0 != ATIADLIWRAPPER_INVALID_GPU)
		{
			dwGpuBus[dwGpu]				= m_dwGpuBus[dwGpu0];
			dwGpuDev[dwGpu]				= m_dwGpuDev[dwGpu0];
			dwGpuFn [dwGpu]				= m_dwGpuFn [dwGpu0];

			m_dwGpuBus[dwGpu0]			= 0;
			m_dwGpuDev[dwGpu0]			= 0;
			m_dwGpuFn [dwGpu0]			= 0;
		}
	}

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		m_dwGpuBus[dwGpu]				= dwGpuBus[dwGpu];
		m_dwGpuDev[dwGpu]				= dwGpuDev[dwGpu];
		m_dwGpuFn [dwGpu]				= dwGpuFn [dwGpu];
	}

	//init Overdrive versions and caps for enumerated adapters

	for (DWORD dwAdapter=0; dwAdapter<m_dwAdapterCount; dwAdapter++)
	{
		m_interface.ADL_Overdrive_Caps(m_adapters[dwAdapter].iAdapterIndex, &m_iOverdriveSupported[dwAdapter], &m_iOverdriveEnabled[dwAdapter], &m_iOverdriveVersion[dwAdapter]);

		APPEND_LOG5("Adapter %d on bus %d, dev %d, fn %d : %s", dwAdapter, m_adapters[dwAdapter].iBusNumber, m_adapters[dwAdapter].iDeviceNumber, m_adapters[dwAdapter].iFunctionNumber, m_adapters[dwAdapter].strAdapterName);

		APPEND_LOG1("Overdrive version %d", m_iOverdriveVersion[dwAdapter]);

		m_bODNCaps[dwAdapter]					= FALSE;
		m_bODNCapsX2[dwAdapter]					= FALSE;

		m_iODNExtCapabilities[dwAdapter]		= 0;
		m_iNumberOfODNExtFeatures[dwAdapter]	= 0;

		m_bOD8InitSetting[dwAdapter]			= FALSE;

		m_iOD8Capabilities[dwAdapter]			= 0;
		m_iNumberOfOD8Features[dwAdapter]		= 0;

		m_bPMLogDataOutput[dwAdapter]			= FALSE;
		
		if (m_iOverdriveVersion[dwAdapter] >= 8)
		{
			if (ADL_OK == m_interface.ADL2_Overdrive8_Init_Setting_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &m_od8InitSetting[dwAdapter]))
				m_bOD8InitSetting[dwAdapter] = TRUE;

			int overdrive8Capabilities					= 0;
			int numberOfOD8Features						= OD8_COUNT;
			ADLOD8SingleInitSetting* lpInitSettingList	= NULL;

			if (ADL_OK == m_interface.ADL2_Overdrive8_Init_SettingX2_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &overdrive8Capabilities, &numberOfOD8Features, &lpInitSettingList))
			{
				m_iOD8Capabilities[dwAdapter]			= overdrive8Capabilities;
				m_iNumberOfOD8Features[dwAdapter]		= numberOfOD8Features;

				for (int iFeature=0; iFeature<numberOfOD8Features; iFeature++)
					m_od8SingleInitSettings[dwAdapter][iFeature] = lpInitSettingList[iFeature];
			}

			ADL_Main_Memory_Free((void**)&lpInitSettingList);

			if (ADL_OK == m_interface.ADL2_New_QueryPMLogData_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &m_pmLogDataOutput[dwAdapter]))
				m_bPMLogDataOutput[dwAdapter] = TRUE;
		}
		if (m_iOverdriveVersion[dwAdapter] >= 7)
		{
			if (ADL_OK == m_interface.ADL2_OverdriveN_Capabilities_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &m_odnCaps[dwAdapter]))
				m_bODNCaps[dwAdapter] = TRUE;

			if (ADL_OK == m_interface.ADL2_OverdriveN_CapabilitiesX2_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &m_odnCapsX2[dwAdapter]))
				m_bODNCapsX2[dwAdapter]	= TRUE;

			int overdriveNExtCapabilities					= 0;
			int numberOfODNExtFeatures						= ODN_COUNT;
			ADLODNExtSingleInitSetting* lpInitSettingList	= NULL;
			int* lpCurrentSettingList						= NULL;

			if (ADL_OK == m_interface.ADL2_OverdriveN_SettingsExt_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &overdriveNExtCapabilities, &numberOfODNExtFeatures, &lpInitSettingList, &lpCurrentSettingList))
			{
				m_iODNExtCapabilities[dwAdapter]			= overdriveNExtCapabilities;
				m_iNumberOfODNExtFeatures[dwAdapter]		= numberOfODNExtFeatures;

				for (int iFeature=0; iFeature<numberOfODNExtFeatures; iFeature++)
					m_odnExtSingleInitSettings[dwAdapter][iFeature] = lpInitSettingList[iFeature];
			}

			ADL_Main_Memory_Free((void**)&lpInitSettingList);
			ADL_Main_Memory_Free((void**)&lpCurrentSettingList);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CATIADLWrapper::Uninit()
{
	if (m_bMainControlCreated2)
	{
		m_interface.ADL2_Main_Control_Destroy(m_context);

		m_bMainControlCreated2	= FALSE;
		m_context				= NULL;
	}

	if (m_bMainControlCreated)
	{
		m_interface.ADL_Main_Control_Destroy();

		m_bMainControlCreated	= FALSE;
	}

	m_interface.Uninit();

	m_dwAdapterCount	= 0;
	m_dwGpuCount		= 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::GetAdapterCount()
{
	return m_dwAdapterCount;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::GetGpuCount()
{
	return m_dwGpuCount;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::FindAdapter(DWORD dwGpu)
{
	for (DWORD dwAdapter=0; dwAdapter<m_dwAdapterCount; dwAdapter++)
	{
		if ((m_adapters[dwAdapter].iBusNumber		== (int)m_dwGpuBus[dwGpu]) &&
			(m_adapters[dwAdapter].iDeviceNumber	== (int)m_dwGpuDev[dwGpu]) &&
			(m_adapters[dwAdapter].iFunctionNumber	== (int)m_dwGpuFn [dwGpu]))
			return dwAdapter;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::FindMasterAdapter()
{
	DWORD dwMasterAdapter = ATIADLIWRAPPER_INVALID_ADAPTER;

	for (DWORD dwAdapter=0; dwAdapter<m_dwAdapterCount; dwAdapter++)
	{
		if (((m_adapters[dwAdapter].iVendorID & 0xFFFF) ==   1002) ||
			((m_adapters[dwAdapter].iVendorID & 0xFFFF) == 0x1002))
		{
			if (m_adapters[dwAdapter].iPresent)
			{
				int status;

				if (ADL_OK == m_interface.ADL_Adapter_Active_Get(m_adapters[dwAdapter].iAdapterIndex, &status))
				{
					if (status != ADL_TRUE)
						continue;
				}

				if (dwMasterAdapter == ATIADLIWRAPPER_INVALID_ADAPTER)
					dwMasterAdapter = dwAdapter;
				else
				{
					if (m_adapters[dwAdapter].iOSDisplayIndex < m_adapters[dwMasterAdapter].iOSDisplayIndex)
						dwMasterAdapter = dwAdapter;
				}
			}
		}
	}

	return dwMasterAdapter;
}
//////////////////////////////////////////////////////////////////////
CString CATIADLWrapper::GetRegistryStr(HKEY hRoot, LPCSTR lpKey, LPCSTR lpValue, LPCSTR lpDefault)
{
	CString strResult = lpDefault;

	if (!_strnicmp(lpKey, "\\Registry\\Machine\\", 18))
	{
		hRoot	= HKEY_LOCAL_MACHINE;
		lpKey	= lpKey + 18;
	}

	if (!_strnicmp(lpKey, "\\Registry\\User\\", 15))
	{
		hRoot	= HKEY_CURRENT_USER;
		lpKey	= lpKey + 15;
	}

	HKEY hKey;

	if (ERROR_SUCCESS == RegOpenKey(hRoot, lpKey, &hKey))
	{
		char buf[MAX_PATH];

		DWORD dwSize = MAX_PATH;
		DWORD dwType;

		if (ERROR_SUCCESS == RegQueryValueEx(hKey, lpValue, 0, &dwType, (LPBYTE)buf, &dwSize))
		{
			if (dwType == REG_SZ)
				strResult = buf;
		}

		RegCloseKey(hKey);
	}

	return strResult;
}
//////////////////////////////////////////////////////////////////////
CString CATIADLWrapper::GetVersionStr()
{
	DWORD dwMasterAdapter = FindMasterAdapter();

	if (dwMasterAdapter != ATIADLIWRAPPER_INVALID_ADAPTER)
	{
		CString strVersion = GetRegistryStr(HKEY_LOCAL_MACHINE, m_adapters[dwMasterAdapter].strDriverPath, "RadeonSoftwareVersion", "");

		if (strVersion.IsEmpty())
			strVersion = GetRegistryStr(HKEY_LOCAL_MACHINE, m_adapters[dwMasterAdapter].strDriverPath, "Catalyst_Version", "");

		return strVersion;
	}

	return "";
}
//////////////////////////////////////////////////////////////////////
CString CATIADLWrapper::GetEditionStr()
{
	DWORD dwMasterAdapter = FindMasterAdapter();

	if (dwMasterAdapter != ATIADLIWRAPPER_INVALID_ADAPTER)
	{
		CString strEdition = GetRegistryStr(HKEY_LOCAL_MACHINE, m_adapters[dwMasterAdapter].strDriverPath, "RadeonSoftwareEdition", "");

		if (strEdition.IsEmpty())
			strEdition = "Catalyst";

		return strEdition;
	}

	return "";
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetName(DWORD dwGpu, CString& strName)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter = FindAdapter(dwGpu);

	strName = m_adapters[dwAdapter].strAdapterName;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetLocation(DWORD dwGpu, DWORD* lpBus, DWORD* lpDev, DWORD* lpFn)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter = FindAdapter(dwGpu);

	if (lpBus)
		*lpBus = m_adapters[dwAdapter].iBusNumber;

	if (lpDev)
		*lpDev = m_adapters[dwAdapter].iDeviceNumber;

	if (lpFn)
		*lpFn = m_adapters[dwAdapter].iFunctionNumber;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::GetOverdriveVersion(DWORD dwGpu)
{
	if (dwGpu >= m_dwGpuCount)
		return 0;

	DWORD dwAdapter = FindAdapter(dwGpu);

	return m_iOverdriveVersion[dwAdapter];
}
//////////////////////////////////////////////////////////////////////
void CATIADLWrapper::RetreivePMLogData(DWORD dwAdapter)
{
	if (m_dwPMLogDataTimestamp[dwAdapter] != m_dwTimestamp)
	{
		m_dwPMLogDataTimestamp[dwAdapter] = m_dwTimestamp;

		//artifical delay in current activity readback fix broken GPU usage/clock sensor (may report bogus data
		//if other ADL functions were called before the call)

		if (m_dwActivityMonitoringDelay)
			Sleep(m_dwActivityMonitoringDelay);

		m_interface.ADL2_New_QueryPMLogData_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &m_pmLogDataOutput[dwAdapter]);
	}
}
//////////////////////////////////////////////////////////////////////
void CATIADLWrapper::SetTimestamp(DWORD dwTimestamp)
{
	m_dwTimestamp = dwTimestamp;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetPMLogSensorData(DWORD dwAdapter, DWORD dwSensor, LPDWORD lpData)
{
	if (m_bPMLogDataOutput[dwAdapter])
	{
		if (m_pmLogDataOutput[dwAdapter].sensors[dwSensor].supported)
		{
			if (lpData)
				*lpData = m_pmLogDataOutput[dwAdapter].sensors[dwSensor].value;

			return  TRUE;
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
#ifndef PMLOG_TEMPERATURE_HOTSPOT
#define PMLOG_TEMPERATURE_HOTSPOT 27
#endif
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::EnumPMLogCoreTemperatureSensors(DWORD dwAdapter, LPDWORD lpSensorsArr)
{
	DWORD dwSensors = 0;

	if (GetPMLogSensorData(dwAdapter, PMLOG_TEMPERATURE_EDGE, NULL))
		lpSensorsArr[dwSensors++] = PMLOG_TEMPERATURE_EDGE;

	if (GetPMLogSensorData(dwAdapter, PMLOG_TEMPERATURE_HOTSPOT, NULL))
		lpSensorsArr[dwSensors++] = PMLOG_TEMPERATURE_HOTSPOT;

	return dwSensors;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::EnumPMLogMemoryTemperatureSensors(DWORD dwAdapter, LPDWORD lpSensorsArr)
{
	DWORD dwSensors = 0;

	if (GetPMLogSensorData(dwAdapter, PMLOG_TEMPERATURE_MEM, NULL))
		lpSensorsArr[dwSensors++] = PMLOG_TEMPERATURE_MEM;

	return dwSensors;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::EnumPMLogVRMTemperatureSensors(DWORD dwAdapter, LPDWORD lpSensorsArr)
{
	DWORD dwSensors = 0;

	if (GetPMLogSensorData(dwAdapter, PMLOG_TEMPERATURE_VRVDDC, NULL))
		lpSensorsArr[dwSensors++] = PMLOG_TEMPERATURE_VRVDDC;

	if (GetPMLogSensorData(dwAdapter, PMLOG_TEMPERATURE_VRMVDD, NULL))
		lpSensorsArr[dwSensors++] = PMLOG_TEMPERATURE_VRMVDD;

	if (GetPMLogSensorData(dwAdapter, PMLOG_TEMPERATURE_VRSOC, NULL))
		lpSensorsArr[dwSensors++] = PMLOG_TEMPERATURE_VRSOC;

	if (GetPMLogSensorData(dwAdapter, PMLOG_TEMPERATURE_VRMVDD0, NULL))
		lpSensorsArr[dwSensors++] = PMLOG_TEMPERATURE_VRMVDD0;

	if (GetPMLogSensorData(dwAdapter, PMLOG_TEMPERATURE_VRMVDD1, NULL))
		lpSensorsArr[dwSensors++] = PMLOG_TEMPERATURE_VRMVDD1;

	return dwSensors;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::GetCoreTemperatureSensorCount(DWORD dwGpu)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter	= FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		DWORD dwSensors[ADL_PMLOG_MAX_SENSORS] = { 0 };

		return EnumPMLogCoreTemperatureSensors(dwAdapter, dwSensors);
	}

	if (GetCurrentCoreTemperature(dwGpu, 0, NULL))
		return 1;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::GetMemoryTemperatureSensorCount(DWORD dwGpu)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter	= FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		DWORD dwSensors[ADL_PMLOG_MAX_SENSORS] = { 0 };

		return EnumPMLogMemoryTemperatureSensors(dwAdapter, dwSensors);
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CATIADLWrapper::GetVRMTemperatureSensorCount(DWORD dwGpu)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter	= FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		DWORD dwSensors[ADL_PMLOG_MAX_SENSORS] = { 0 };

		return EnumPMLogVRMTemperatureSensors(dwAdapter, dwSensors);
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentCoreTemperature(DWORD dwGpu, DWORD dwSensor, LONG* lpTemperature)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter	= FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_bPMLogDataOutput[dwAdapter])
		{
			RetreivePMLogData(dwAdapter);

			DWORD dwSensors[ADL_PMLOG_MAX_SENSORS] = { 0 };

			if (dwSensor < EnumPMLogCoreTemperatureSensors(dwAdapter, dwSensors))
			{
				if (lpTemperature)
					GetPMLogSensorData(dwAdapter, dwSensors[dwSensor], (LPDWORD)lpTemperature);

				return TRUE;
			}
		}

		return FALSE;
	}

	if (dwOverdriveVersion >= 7)
	{
		int temperature;

		if (ADL_OK == m_interface.ADL2_OverdriveN_Temperature_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, 1/*TEMPERATURE_TYPE_EDGE*/, &temperature))
		{
			if (lpTemperature)
				*lpTemperature = temperature / 1000;

			return TRUE;
		}

		return FALSE;
	}

	if (dwOverdriveVersion >= 6)
	{
		int temperature;

		if (ADL_OK == m_interface.ADL_Overdrive6_Temperature_Get(m_adapters[dwAdapter].iAdapterIndex, &temperature))
		{
			if (lpTemperature)
				*lpTemperature = temperature / 1000;

			return TRUE;
		}

		return FALSE;
	}
		

	ADLTemperature temperature;
	ATIADL_INIT_STRUCT(temperature);

	if (ADL_OK == m_interface.ADL_Overdrive5_Temperature_Get(m_adapters[dwAdapter].iAdapterIndex, 0, &temperature))
	{
		if (lpTemperature)
			*lpTemperature = temperature.iTemperature / 1000;

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentMemoryTemperature(DWORD dwGpu, DWORD dwSensor, LONG* lpTemperature)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter	= FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_bPMLogDataOutput[dwAdapter])
		{
			RetreivePMLogData(dwAdapter);

			DWORD dwSensors[ADL_PMLOG_MAX_SENSORS] = { 0 };

			if (dwSensor < EnumPMLogMemoryTemperatureSensors(dwAdapter, dwSensors))
			{
				if (lpTemperature)
					GetPMLogSensorData(dwAdapter, dwSensors[dwSensor], (LPDWORD)lpTemperature);

				return TRUE;
			}
		}

		return FALSE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentVRMTemperature(DWORD dwGpu, DWORD dwSensor, LONG* lpTemperature)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter	= FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_bPMLogDataOutput[dwAdapter])
		{
			RetreivePMLogData(dwAdapter);

			DWORD dwSensors[ADL_PMLOG_MAX_SENSORS] = { 0 };

			if (dwSensor < EnumPMLogVRMTemperatureSensors(dwAdapter, dwSensors))
			{
				if (lpTemperature)
					GetPMLogSensorData(dwAdapter, dwSensors[dwSensor], (LPDWORD)lpTemperature);

				return TRUE;
			}
		}

		return FALSE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentFanSpeed(DWORD dwGpu, DWORD* lpSpeed)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter	= FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_iOD8Capabilities[dwAdapter] & ADL_OD8_FAN_CURVE)
		{
			BOOL bResult = FALSE;

			int overdrive8Capabilities					= 0;
			int numberOfOD8Features						= OD8_COUNT;
			int* lpCurrentSettingList					= NULL;

			if (ADL_OK == m_interface.ADL2_Overdrive8_Current_SettingX2_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &numberOfOD8Features, &lpCurrentSettingList))
			{

				if ((lpCurrentSettingList[OD8_FAN_CURVE_SPEED_1] == lpCurrentSettingList[OD8_FAN_CURVE_SPEED_2]) &&
					(lpCurrentSettingList[OD8_FAN_CURVE_SPEED_2] == lpCurrentSettingList[OD8_FAN_CURVE_SPEED_3]) &&
					(lpCurrentSettingList[OD8_FAN_CURVE_SPEED_3] == lpCurrentSettingList[OD8_FAN_CURVE_SPEED_4]) &&
					(lpCurrentSettingList[OD8_FAN_CURVE_SPEED_4] == lpCurrentSettingList[OD8_FAN_CURVE_SPEED_5]))
				{
					if (lpSpeed)
						*lpSpeed = lpCurrentSettingList[OD8_FAN_CURVE_SPEED_1];

					bResult = TRUE;
				}
				else
				{
					if (m_bPMLogDataOutput[dwAdapter])
					{
						RetreivePMLogData(dwAdapter);

						if (GetPMLogSensorData(dwAdapter, PMLOG_FAN_PERCENTAGE, lpSpeed))
							bResult = TRUE;
						else
						if (m_od8SingleInitSettings[dwAdapter][OD8_FAN_MIN_SPEED].maxValue)
						{
							DWORD dwCurrentFanSpeedRPM;

							if (GetPMLogSensorData(dwAdapter, PMLOG_FAN_RPM, &dwCurrentFanSpeedRPM))
							{
								DWORD dwCurrentFanSpeed = (DWORD)ceil(100.0 * dwCurrentFanSpeedRPM / m_od8SingleInitSettings[dwAdapter][OD8_FAN_MIN_SPEED].maxValue);

								if (dwCurrentFanSpeed > 100)
									dwCurrentFanSpeed = 100;

								if (lpSpeed)
									*lpSpeed = dwCurrentFanSpeed;

								bResult = TRUE;
							}
						}
					}
				}
			}

			ADL_Main_Memory_Free((void**)&lpCurrentSettingList);

			if (bResult)
				return TRUE;
		}

		return FALSE;
	}

	if (dwOverdriveVersion >= 7)
	{
		if (m_iODNExtCapabilities[dwAdapter] & ADL_ODN_EXT_FEATURE_FAN_CURVE)
		{
			BOOL bResult = FALSE;

			int overdriveNExtCapabilities					= 0;
			int numberOfODNExtFeatures						= ODN_COUNT;
			ADLODNExtSingleInitSetting* lpInitSettingList	= NULL;
			int* lpCurrentSettingList						= NULL;

			if (ADL_OK == m_interface.ADL2_OverdriveN_SettingsExt_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &overdriveNExtCapabilities, &numberOfODNExtFeatures, &lpInitSettingList, &lpCurrentSettingList))
			{
				if ((lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_1] == lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_2]) &&
					(lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_2] == lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_3]) &&
					(lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_3] == lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_4]) &&
					(lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_4] == lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_5]))
				{
					if (lpSpeed)
						*lpSpeed = lpCurrentSettingList[ADL_ODN_PARAMETER_FAN_CURVE_SPEED_1];

					bResult = TRUE;
				}
			}

			ADL_Main_Memory_Free((void**)&lpInitSettingList);
			ADL_Main_Memory_Free((void**)&lpCurrentSettingList);

			if (bResult)
				return TRUE;
		}

		if (m_bODNCapsX2[dwAdapter])
		{
			if (m_odnCapsX2[dwAdapter].fanSpeed.iMax > m_odnCapsX2[dwAdapter].fanSpeed.iMin)
			{
				ADLODNFanControl control;
				ZeroMemory(&control, sizeof(control));

				if (ADL_OK == m_interface.ADL2_OverdriveN_FanControl_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &control))
				{
					int iFanSpeed = 0;

					if (control.iCurrentFanSpeed < m_odnCapsX2[dwAdapter].fanSpeed.iMin)
						iFanSpeed = 0;
					else
					if (control.iCurrentFanSpeed > m_odnCapsX2[dwAdapter].fanSpeed.iMax)
						iFanSpeed = 100;
					else
					{
						if (control.iMinFanLimit == control.iTargetFanSpeed)
							iFanSpeed = (DWORD)ceil(100.0 * control.iTargetFanSpeed / m_odnCapsX2[dwAdapter].fanSpeed.iMax);
						else
							iFanSpeed = (DWORD)ceil(100.0 * control.iCurrentFanSpeed / m_odnCapsX2[dwAdapter].fanSpeed.iMax);
					}

					if (lpSpeed)
						*lpSpeed = iFanSpeed;

					return TRUE;
				}
			}
		}
		else
		if (m_bODNCaps[dwAdapter])
		{
			if (m_odnCaps[dwAdapter].fanSpeed.iMax > m_odnCaps[dwAdapter].fanSpeed.iMin)
			{
				ADLODNFanControl control;
				ZeroMemory(&control, sizeof(control));

				if (ADL_OK == m_interface.ADL2_OverdriveN_FanControl_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &control))
				{
					int iFanSpeed = 0;

					if (control.iCurrentFanSpeed < m_odnCaps[dwAdapter].fanSpeed.iMin)
						iFanSpeed = 0;
					else
					if (control.iCurrentFanSpeed > m_odnCaps[dwAdapter].fanSpeed.iMax)
						iFanSpeed = 100;
					else
					{
						if (control.iMinFanLimit == control.iTargetFanSpeed)
							iFanSpeed = (DWORD)(100.0 * control.iTargetFanSpeed / m_odnCaps[dwAdapter].fanSpeed.iMax);
						else
							iFanSpeed = (DWORD)(100.0 * control.iCurrentFanSpeed / m_odnCaps[dwAdapter].fanSpeed.iMax);
					}

					if (lpSpeed)
						*lpSpeed = iFanSpeed;

					return TRUE;
				}
			}
		}

		return FALSE;
	}

	if (dwOverdriveVersion >= 6)
	{
		ADLOD6FanSpeedInfo info;
		ZeroMemory(&info, sizeof(info));

		if (ADL_OK == m_interface.ADL_Overdrive6_FanSpeed_Get(m_adapters[dwAdapter].iAdapterIndex, &info))
		{
			if (lpSpeed)
				*lpSpeed = info.iFanSpeedPercent;

			return TRUE;
		}

		return FALSE;
	}

	ADLFanSpeedValue fanSpeedValue;
	ATIADL_INIT_STRUCT(fanSpeedValue);

	fanSpeedValue.iSpeedType =  ADL_DL_FANCTRL_SPEED_TYPE_PERCENT;

	if (ADL_OK == m_interface.ADL_Overdrive5_FanSpeed_Get(m_adapters[dwAdapter].iAdapterIndex, 0, &fanSpeedValue))
	{
		if (lpSpeed)
			*lpSpeed = fanSpeedValue.iFanSpeed;

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentFanSpeedRPM(DWORD dwGpu, DWORD* lpSpeed)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter	= FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_bPMLogDataOutput[dwAdapter])
		{
			RetreivePMLogData(dwAdapter);

			if (lpSpeed)
				GetPMLogSensorData(dwAdapter, PMLOG_FAN_RPM, lpSpeed);

			return TRUE;
		}
	}

	if (dwOverdriveVersion >= 7)
	{
		ADLODNFanControl control;
		ZeroMemory(&control, sizeof(control));

		if (ADL_OK == m_interface.ADL2_OverdriveN_FanControl_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &control))
		{
			if (lpSpeed)
				*lpSpeed = control.iCurrentFanSpeed;

			return TRUE;
		}

		return FALSE;
	}

	if (dwOverdriveVersion >= 6)
	{
		ADLOD6FanSpeedInfo info;
		ZeroMemory(&info, sizeof(info));

		if (ADL_OK == m_interface.ADL_Overdrive6_FanSpeed_Get(m_adapters[dwAdapter].iAdapterIndex, &info))
		{
			if (lpSpeed)
				*lpSpeed = info.iFanSpeedRPM;

			return TRUE;
		}

		return FALSE;
	}

	ADLFanSpeedValue fanSpeedValue;
	ATIADL_INIT_STRUCT(fanSpeedValue);

	fanSpeedValue.iSpeedType =  ADL_DL_FANCTRL_SPEED_TYPE_RPM;

	if (ADL_OK == m_interface.ADL_Overdrive5_FanSpeed_Get(m_adapters[dwAdapter].iAdapterIndex, 0, &fanSpeedValue))
	{
		if (lpSpeed)
			*lpSpeed = fanSpeedValue.iFanSpeed;

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentActivity(DWORD dwGpu, DWORD* lpEngineClock, DWORD* lpMemoryClock, DWORD* lpVddc, DWORD* lpActivity)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter = FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_bPMLogDataOutput[dwAdapter])
		{
			RetreivePMLogData(dwAdapter);

			if (lpEngineClock)
			{
				DWORD dwEngineClock;

				if (GetPMLogSensorData(dwAdapter, PMLOG_CLK_GFXCLK, &dwEngineClock))
					*lpEngineClock = dwEngineClock * 1000;
			}

			if (lpMemoryClock)
			{
				DWORD dwMemoryClock;

				if (GetPMLogSensorData(dwAdapter, PMLOG_CLK_MEMCLK, &dwMemoryClock))
					*lpMemoryClock = dwMemoryClock * 1000;
			}

			if (lpVddc)
				GetPMLogSensorData(dwAdapter, PMLOG_GFX_VOLTAGE, lpVddc);

			if (lpActivity)
				GetPMLogSensorData(dwAdapter, PMLOG_INFO_ACTIVITY_GFX, lpActivity);

			return TRUE;
		}
	}

	//artifical delay in current activity readback fix broken GPU usage/clock sensor (may report bogus data
	//if other ADL functions were called before the call)

	//NOTE: for Overdrive8 delay is performed inside RetreivePMLogData

	if (m_dwActivityMonitoringDelay)
		Sleep(m_dwActivityMonitoringDelay);

	if (dwOverdriveVersion >= 7)
	{
		ADLODNPerformanceStatus status;
		ZeroMemory(&status, sizeof(status));

		if (ADL_OK == m_interface.ADL2_OverdriveN_PerformanceStatus_Get(m_context, m_adapters[dwAdapter].iAdapterIndex, &status))
		{
			if (lpEngineClock)
				*lpEngineClock = status.iCoreClock * 10;

			if (lpMemoryClock)
				*lpMemoryClock = status.iMemoryClock * 10;

			if (lpVddc)
				*lpVddc = 0;

			if (lpActivity)
				*lpActivity = status.iGPUActivityPercent;

			return TRUE;
		}
	}

	if (dwOverdriveVersion >= 6)
	{
		ADLOD6CurrentStatus status;
		ZeroMemory(&status, sizeof(status));

		if (ADL_OK == m_interface.ADL_Overdrive6_CurrentStatus_Get(m_adapters[dwAdapter].iAdapterIndex, &status))
		{
			if (lpEngineClock)
				*lpEngineClock = status.iEngineClock * 10;

			if (lpMemoryClock)
				*lpMemoryClock = status.iMemoryClock * 10;

			if (lpVddc)
				*lpVddc = 0;

			if (lpActivity)
				*lpActivity = status.iActivityPercent;

			return TRUE;
		}
	}

	ADLPMActivity activity; 
	ATIADL_INIT_STRUCT(activity);

	if (ADL_OK == m_interface.ADL_Overdrive5_CurrentActivity_Get(m_adapters[dwAdapter].iAdapterIndex, &activity))
	{
		if (lpEngineClock)
			*lpEngineClock = activity.iEngineClock * 10;

		if (lpMemoryClock)
			*lpMemoryClock = activity.iMemoryClock * 10;

		if (lpVddc)
			*lpVddc = activity.iVddc;

		if (lpActivity)
			*lpActivity = activity.iActivityPercent;

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentAbsPower(DWORD dwGpu, DWORD* lpPower)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter = FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_bPMLogDataOutput[dwAdapter])
		{
			RetreivePMLogData(dwAdapter);

			DWORD power;

			if (GetPMLogSensorData(dwAdapter, PMLOG_ASIC_POWER, &power))
			{
				if (lpPower)
					*lpPower = power * 1000;

				return TRUE;
			}
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentMemoryVoltage(DWORD dwGpu, DWORD* lpVoltage)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter = FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_bPMLogDataOutput[dwAdapter])
		{
			RetreivePMLogData(dwAdapter);

			DWORD dwVoltage;

			if (GetPMLogSensorData(dwAdapter, PMLOG_MEM_VOLTAGE, &dwVoltage))
			{
				if (lpVoltage)
					*lpVoltage = dwVoltage;

				return TRUE;
			}
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetCurrentMemoryControllerUsage(DWORD dwGpu, DWORD* lpUsage)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter = FindAdapter(dwGpu);

	DWORD dwOverdriveVersion = GetOverdriveVersion(dwGpu);

	if (dwOverdriveVersion >= 8)
	{
		if (m_bPMLogDataOutput[dwAdapter])
		{
			RetreivePMLogData(dwAdapter);

			DWORD dwUsage;

			if (GetPMLogSensorData(dwAdapter, PMLOG_INFO_ACTIVITY_MEM, &dwUsage))
			{
				if (lpUsage)
					*lpUsage = dwUsage;

				return TRUE;
			}
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLWrapper::GetTotalVideomemory(DWORD dwGpu, DWORD* lpTotalVideomemory)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	DWORD dwAdapter = FindAdapter(dwGpu);

	ADLMemoryInfo info; 
	ZeroMemory(&info, sizeof(info));

	if (ADL_OK == m_interface.ADL_Adapter_MemoryInfo_Get(m_adapters[dwAdapter].iAdapterIndex, &info))
	{
		if (lpTotalVideomemory)
			*lpTotalVideomemory = ALIGN(info.iMemorySize / 1024, 1048576);

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
