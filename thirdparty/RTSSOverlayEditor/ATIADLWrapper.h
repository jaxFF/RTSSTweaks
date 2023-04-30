// ATIADLWrapper.h: interface for the CATIADLWrapper class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "ATIADLInterface.h"
//////////////////////////////////////////////////////////////////////
#define ATIADLWRAPPER_MAX_ADAPTERS							256
#define ATIADLWRAPPER_MAX_ODN_EXT_FEATURES					32
#define ATIADLWRAPPER_MAX_OD8_FEATURES						64
#define ATIADLIWRAPPER_INVALID_ADAPTER						0xFFFFFFFF
#define ATIADLIWRAPPER_INVALID_GPU							0xFFFFFFFF
//////////////////////////////////////////////////////////////////////
class CATIADLWrapper
{
public:
	CATIADLWrapper();
	virtual ~CATIADLWrapper();

	void	Init();
	void	Uninit();
	DWORD	GetAdapterCount();
	DWORD	FindAdapter(DWORD dwGpu);
	DWORD	FindMasterAdapter();

	CString GetVersionStr();
	CString GetEditionStr();

	void	SetTimestamp(DWORD dwTimestamp);

	DWORD	GetGpuCount();
	BOOL	GetName(DWORD dwGpu, CString& strName);
	BOOL	GetLocation(DWORD dwGpu, DWORD* lpBus, DWORD* lpDev, DWORD* lpFn);

	DWORD	GetOverdriveVersion(DWORD dwGpu);

	DWORD	GetCoreTemperatureSensorCount(DWORD dwGpu);
	DWORD	GetMemoryTemperatureSensorCount(DWORD dwGpu);
	DWORD	GetVRMTemperatureSensorCount(DWORD dwGpu);

	BOOL	GetCurrentCoreTemperature(DWORD dwGpu, DWORD dwSensor, LONG* lpTemperature);
	BOOL	GetCurrentMemoryTemperature(DWORD dwGpu, DWORD dwSensor, LONG* lpTemperature);
	BOOL	GetCurrentVRMTemperature(DWORD dwGpu, DWORD dwSensor, LONG* lpTemperature);

	BOOL	GetCurrentActivity(DWORD dwGpu, DWORD* lpEngineClock, DWORD* lpMemoryClock, DWORD* lpVddc, DWORD* lpActivity);
	BOOL	GetCurrentFanSpeed(DWORD dwGpu, DWORD* lpSpeed);
	BOOL	GetCurrentFanSpeedRPM(DWORD dwGpu, DWORD* lpSpeed);
	BOOL	GetCurrentAbsPower(DWORD dwGpu, DWORD* lpPower);

	BOOL	GetCurrentMemoryVoltage(DWORD dwGpu, DWORD* lpVoltage);
	BOOL	GetCurrentMemoryControllerUsage(DWORD dwGpu, DWORD* lpUsage);

	BOOL	GetTotalVideomemory(DWORD dwGpu, DWORD* lpTotalVideomemory);

protected:
	void	RetreivePMLogData(DWORD dwAdapter);
	BOOL	GetPMLogSensorData(DWORD dwAdapter, DWORD dwSensor, LPDWORD lpData);

	DWORD	EnumPMLogCoreTemperatureSensors(DWORD dwAdapter, LPDWORD lpSensorsArr);
	DWORD	EnumPMLogMemoryTemperatureSensors(DWORD dwAdapter, LPDWORD lpSensorsArr);
	DWORD	EnumPMLogVRMTemperatureSensors(DWORD dwAdapter, LPDWORD lpSensorsArr);

	CString GetRegistryStr(HKEY hRoot, LPCSTR lpKey, LPCSTR lpValue, LPCSTR lpDefault);

	static void* __stdcall ADL_Main_Memory_Alloc(int iSize);
	static void  __stdcall ADL_Main_Memory_Free(void** lpBuffer);

	CATIADLInterface		m_interface;
	AdapterInfo				m_adapters[ATIADLWRAPPER_MAX_ADAPTERS];	
	DWORD					m_dwAdapterCount;

	DWORD					m_dwGpuCount;
	DWORD					m_dwGpuBus[ATIADLWRAPPER_MAX_ADAPTERS];
	DWORD					m_dwGpuDev[ATIADLWRAPPER_MAX_ADAPTERS];
	DWORD					m_dwGpuFn [ATIADLWRAPPER_MAX_ADAPTERS];

	int						m_iOverdriveSupported[ATIADLWRAPPER_MAX_ADAPTERS];
	int						m_iOverdriveEnabled[ATIADLWRAPPER_MAX_ADAPTERS];
	int						m_iOverdriveVersion[ATIADLWRAPPER_MAX_ADAPTERS];

	DWORD					m_dwActivityMonitoringDelay;

	//Overdrive Next / Overdrive 7

	BOOL					m_bODNCaps[ATIADLWRAPPER_MAX_ADAPTERS];
	ADLODNCapabilities		m_odnCaps[ATIADLWRAPPER_MAX_ADAPTERS];

	BOOL					m_bODNCapsX2[ATIADLWRAPPER_MAX_ADAPTERS];
	ADLODNCapabilitiesX2	m_odnCapsX2[ATIADLWRAPPER_MAX_ADAPTERS];

	int						m_iODNExtCapabilities[ATIADLWRAPPER_MAX_ADAPTERS];
	int						m_iNumberOfODNExtFeatures[ATIADLWRAPPER_MAX_ADAPTERS];
	ADLODNExtSingleInitSetting m_odnExtSingleInitSettings[ATIADLWRAPPER_MAX_ADAPTERS][ATIADLWRAPPER_MAX_ODN_EXT_FEATURES];

	//Overdrive 8

	BOOL					m_bOD8InitSetting[ATIADLWRAPPER_MAX_ADAPTERS];
	ADLOD8InitSetting		m_od8InitSetting[ATIADLWRAPPER_MAX_ADAPTERS];

	int						m_iOD8Capabilities[ATIADLWRAPPER_MAX_ADAPTERS];
	int						m_iNumberOfOD8Features[ATIADLWRAPPER_MAX_ADAPTERS];
	ADLOD8SingleInitSetting m_od8SingleInitSettings[ATIADLWRAPPER_MAX_ADAPTERS][ATIADLWRAPPER_MAX_OD8_FEATURES];

	BOOL					m_bPMLogDataOutput[ATIADLWRAPPER_MAX_ADAPTERS];
	ADLPMLogDataOutput		m_pmLogDataOutput[ATIADLWRAPPER_MAX_ADAPTERS];
	DWORD					m_dwPMLogDataTimestamp[ATIADLWRAPPER_MAX_ADAPTERS];

	DWORD					m_dwTimestamp;

	BOOL					m_bMainControlCreated;
	BOOL					m_bMainControlCreated2;
	ADL_CONTEXT_HANDLE		m_context;
};
//////////////////////////////////////////////////////////////////////
