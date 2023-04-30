// ATIADLInterface.h: interface for the CATIADLInterface class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _ATIADLINTERFACE_H_INCLUDED_
#define _ATIADLINTERFACE_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#include "adl_sdk.h"
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
typedef int (*ADL_MAIN_CONTROL_CREATE				)(ADL_MAIN_MALLOC_CALLBACK, int);
typedef int (*ADL_MAIN_CONTROL_DESTROY				)();
typedef int (*ADL_MAIN_CONTROL_REFRESH				)();
typedef int (*ADL_ADAPTER_NUMBEROFADAPTERS_GET		)(int*);
typedef int (*ADL_ADAPTER_ADAPTERINFO_GET			)(LPAdapterInfo, int);
typedef int (*ADL_ADAPTER_ACTIVE_GET				)(int, int*);
typedef int (*ADL_ADAPTER_ID_GET					)(int, int*);
typedef	int (*ADL_ADAPTER_MEMORYINFO_GET			)(int, ADLMemoryInfo*);

typedef int (*ADL_ADAPTER_VIDEOBIOSINFO_GET			)(int, ADLBiosInfo*);
typedef int (*ADL_ADAPTER_ACCESSIBILITY_GET			)(int, int*);
typedef int (*ADL_DISPLAY_WRITEANDREADI2C			)(int, ADLI2C*);
typedef int (*ADL_DISPLAY_DDCBLOCKACCESS_GET		)(int, int, int, int, int, char*, int*, char*);
typedef	int (*ADL_DISPLAY_MVPUCAPS_GET				)(int, ADLMVPUCaps*);	 
typedef	int (*ADL_DISPLAY_MVPUSTATUS_GET			)(int, ADLMVPUStatus*);	 

typedef int (*ADL_OVERDRIVE_CAPS					)(int iAdapterIndex, int *iSupported, int *iEnabled, int *iVersion);

typedef int (*ADL_OVERDRIVE5_CURRENTACTIVITY_GET	)(int, ADLPMActivity*);
typedef int (*ADL_OVERDRIVE5_THERMALDEVICES_ENUM	)(int, int, ADLThermalControllerInfo*);
typedef int (*ADL_OVERDRIVE5_TEMPERATURE_GET		)(int, int, ADLTemperature*);
typedef int (*ADL_OVERDRIVE5_FANSPEEDINFO_GET		)(int, int, ADLFanSpeedInfo*);
typedef int (*ADL_OVERDRIVE5_FANSPEED_GET			)(int, int, ADLFanSpeedValue*);
typedef int (*ADL_OVERDRIVE5_FANSPEED_SET			)(int, int, ADLFanSpeedValue*);
typedef int (*ADL_OVERDRIVE5_FANSPEEDTODEFAULT_SET	)(int, int);
typedef int (*ADL_OVERDRIVE5_ODPARAMETERS_GET		)(int, ADLODParameters*);
typedef int (*ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET)(int, int, ADLODPerformanceLevels*);
typedef int (*ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET)(int, ADLODPerformanceLevels*);
typedef int (*ADL_OVERDRIVE5_POWERCONTROLINFO_GET	)(int, ADLPowerControlInfo*);
typedef int (*ADL_OVERDRIVE5_POWERCONTROL_GET		)(int, int*, int*);
typedef int (*ADL_OVERDRIVE5_POWERCONTROL_SET		)(int, int);

typedef int	(*ADL_OVERDRIVE6_CURRENTSTATUS_GET		)(int iAdapterIndex, ADLOD6CurrentStatus* lpCurrentStatus);
typedef int (*ADL_OVERDRIVE6_VOLTAGECONTROLINFO_GET )(int iAdapterIndex, ADLOD6VoltageControlInfo* lpVoltageControlInfo);
typedef int (*ADL_OVERDRIVE6_VOLTAGECONTROL_GET		)(int iAdapterIndex, int*  lpCurrentValue, int* lpDefaultValue);
typedef int (*ADL_OVERDRIVE6_VOLTAGECONTROL_SET		)(int iAdapterIndex, int iValue);
typedef int (*ADL_OVERDRIVE6_FANSPEED_GET			)(int iAdapterIndex, ADLOD6FanSpeedInfo *lpFanSpeedInfo);
typedef int (*ADL_OVERDRIVE6_FANSPEED_SET			)(int iAdapterIndex, ADLOD6FanSpeedValue *lpFanSpeedValue);
typedef int (*ADL_OVERDRIVE6_FANSPEED_RESET			)(int iAdapterIndex);

typedef int (*ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS )(int iAdapterIndex, ADLOD6ThermalControllerCaps *lpThermalControllerCaps);
typedef int (*ADL_OVERDRIVE6_TEMPERATURE_GET		)(int iAdapterIndex, int *lpTemperature);
typedef int (*ADL_OVERDRIVE6_CAPABILITIES_GET		)(int iAdapterIndex, ADLOD6Capabilities *lpODCapabilities);
typedef int (*ADL_OVERDRIVE6_POWERCONTROL_CAPS		)(int iAdapterIndex, int *lpSupported);
typedef int (*ADL_OVERDRIVE6_POWERCONTROLINFO_GET	)(int iAdapterIndex, ADLOD6PowerControlInfo *lpPowerControlInfo);
typedef int (*ADL_OVERDRIVE6_POWERCONTROL_GET		)(int iAdapterIndex, int *lpCurrentValue, int *lpDefaultValue);
typedef int (*ADL_OVERDRIVE6_POWERCONTROL_SET		)(int iAdapterIndex, int iValue);
typedef int (*ADL_OVERDRIVE6_STATEINFO_GET			)(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo);
typedef int (*ADL_OVERDRIVE6_STATE_SET				)(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo);

typedef int (*ADL2_MAIN_CONTROL_CREATE				)(ADL_MAIN_MALLOC_CALLBACK, int, ADL_CONTEXT_HANDLE*);
typedef int (*ADL2_MAIN_CONTROL_REFRESH				)(ADL_CONTEXT_HANDLE);
typedef int (*ADL2_MAIN_CONTROL_DESTROY				)(ADL_CONTEXT_HANDLE);

typedef int (*ADL2_OVERDRIVEN_CAPABILITIES_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNCapabilities *lpODCapabilities); 
typedef int (*ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels);
typedef int (*ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels); 
typedef int (*ADL2_OVERDRIVEN_MEMORYCLOCKS_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels); 
typedef int (*ADL2_OVERDRIVEN_MEMORYCLOCKS_SET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels);
typedef int (*ADL2_OVERDRIVEN_FANCONTROL_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNFanControl *lpODFanSpeed);
typedef int (*ADL2_OVERDIRVEN_FANCONTROL_SET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNFanControl *lpODFanControl);
typedef int (*ADL2_OVERDRIVEN_POWERLIMIT_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPowerLimitSetting *lpODPowerLimit);
typedef int (*ADL2_OVERDRIVEN_POWERLIMIT_SET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPowerLimitSetting *lpODPowerLimit);
typedef int (*ADL2_OVERDRIVEN_TEMPERATURE_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iTemperatureType, int *iTemperature);
typedef int (*ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET )(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceStatus *lpODPerformanceStatus);
typedef int (*ADL2_OVERDRIVEN_TEST_SET				)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iEnabled);
typedef int (*ADL2_OVERDRIVEN_COUNTOFEVENTS_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int eventcounterType, int *eventCount);

typedef int (*ADL2_OVERDRIVEN_CAPABILITIESX2_GET	)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNCapabilitiesX2 *lpODCapabilitiesX2);
typedef int (*ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_GET	)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2);
typedef int (*ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_SET	)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2);
typedef int (*ADL2_OVERDRIVEN_MEMORYCLOCKSX2_GET	)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2);
typedef int (*ADL2_OVERDRIVEN_MEMORYCLOCKSX2_SET	)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2);

typedef int (*ADL2_OVERDRIVEN_SETTINGSEXT_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpOverdriveNExtCapabilities, int *lpNumberOfODNExtFeatures, ADLODNExtSingleInitSetting** lppInitSettingList, int** lppCurrentSettingList);
typedef int (*ADL2_OVERDRIVEN_SETTINGSEXT_SET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iNumberOfODNExtFeatures, int* itemValueValidList, int* lpItemValueList);

typedef int (*ADL2_OVERDRIVE8_INIT_SETTING_GET		)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8InitSetting* lpInitSetting);
typedef int (*ADL2_OVERDRIVE8_CURRENT_SETTING_GET	)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8CurrentSetting* lpCurrentSetting);
typedef int (*ADL2_OVERDRIVE8_SETTING_SET			)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8SetSetting* lpSetSetting, ADLOD8CurrentSetting* lpCurrentSetting);
typedef int (*ADL2_NEW_QUERYPMLOGDATA_GET			)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLPMLogDataOutput* lpDataOutput);
typedef int (*ADL2_OVERDRIVE8_INIT_SETTINGX2_GET	)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int *lpOverdrive8Capabilities, int *lpNumberOfFeatures, ADLOD8SingleInitSetting **lppInitSettingList);
typedef int (*ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET	)(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int *lpNumberOfFeatures, int **lppCurrentSettingList);

//////////////////////////////////////////////////////////////////////
#define ATIADL_INIT_STRUCT(s)		{ ZeroMemory(&s,sizeof(s)); s.iSize = sizeof(s); }
#define ATIADL_INIT_STRUCT_EX(s,x)	{ ZeroMemory(&s,sizeof(s)); s.iSize = sizeof(s) + x; }
//////////////////////////////////////////////////////////////////////
class CATIADLInterface  
{
public:
	int ADL_Adapter_Accessibility_Get(int iAdapterIndex, int *lpAccessibility);
	int ADL_Adapter_Active_Get(int iAdapterIndex, int *lpStatus);
	int ADL_Adapter_ID_Get(int iAdapterIndex, int *lpAdapterID);
	int ADL_Adapter_MemoryInfo_Get(int iAdapterIndex, ADLMemoryInfo *lpMemoryInfo);

	int ADL_Adapter_VideoBiosInfo_Get(int iAdapterIndex,  ADLBiosInfo* lpBiosInfo );
	int ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters);
	int ADL_Main_Control_Refresh();
	int ADL_Main_Control_Destroy();

	int ADL_Adapter_NumberOfAdapters_Get(int *lpNumAdapters);
	int ADL_Adapter_AdapterInfo_Get(LPAdapterInfo lpInfo, int iInputSize);

	int ADL_Display_WriteAndReadI2C(int iAdapterIndex,  ADLI2C *  lpI2C);
	int ADL_Display_DDCBlockAccess_Get(int iAdapterIndex, int iDisplayIndex, int iOption, int iCommandIndex,int iSendMsgLen, char *lpucSendMsgBuf, int *lpulRecvMsgLen, char *lpucRecvMsgBuf);
	int ADL_Display_MVPUCaps_Get(int iAdapterIndex, ADLMVPUCaps* lpMvpuCaps);	 
	int ADL_Display_MVPUStatus_Get(int iAdapterIndex, ADLMVPUStatus* lpMvpuStatus);	 

	int ADL_Overdrive_Caps(int iAdapterIndex, int *iSupported, int *iEnabled, int *iVersion);

	int ADL_Overdrive5_CurrentActivity_Get(int iAdapterIndex, ADLPMActivity *lpActivity);
	int ADL_Overdrive5_ThermalDevices_Enum(int iAdapterIndex, int iThermalControllerIndex, ADLThermalControllerInfo *lpThermalControllerInfo);
	int ADL_Overdrive5_Temperature_Get(int iAdapterIndex, int iThermalControllerIndex, ADLTemperature *lpTemperature);
	int ADL_Overdrive5_FanSpeedInfo_Get(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedInfo *lpFanSpeedInfo);
	int ADL_Overdrive5_FanSpeed_Get(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue);
	int ADL_Overdrive5_FanSpeed_Set(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue);
	int ADL_Overdrive5_FanSpeedToDefault_Set(int iAdapterIndex, int iThermalControllerIndex);
	int ADL_Overdrive5_ODParameters_Get(int iAdapterIndex, ADLODParameters *lpOdParameters);
	int ADL_Overdrive5_ODPerformanceLevels_Get(int iAdapterIndex, int iDefault, ADLODPerformanceLevels *lpOdPerformanceLevels);
	int ADL_Overdrive5_ODPerformanceLevels_Set(int iAdapterIndex, ADLODPerformanceLevels *lpOdPerformanceLevels);
	int ADL_Overdrive5_PowerControlInfo_Get(int iAdapterIndex, ADLPowerControlInfo* lpPowerControlInfo);
	int ADL_Overdrive5_PowerControl_Get(int iAdapterIndex, int* lpValue, int* lpReserved);
	int ADL_Overdrive5_PowerControl_Set(int iAdapterIndex, int iValue);

	int ADL_Overdrive6_CurrentStatus_Get(int iAdapterIndex, ADLOD6CurrentStatus *lpCurrentStatus);
	int ADL_Overdrive6_VoltageControlInfo_Get(int iAdapterIndex, ADLOD6VoltageControlInfo* lpVoltageControlInfo);
	int ADL_Overdrive6_VoltageControl_Get(int iAdapterIndex, int*  lpCurrentValue, int* lpDefaultValue);
	int ADL_Overdrive6_VoltageControl_Set(int iAdapterIndex, int iValue);
	int ADL_Overdrive6_FanSpeed_Get(int iAdapterIndex, ADLOD6FanSpeedInfo *lpFanSpeedInfo);
	int ADL_Overdrive6_FanSpeed_Set(int iAdapterIndex, ADLOD6FanSpeedValue *lpFanSpeedValue);
	int ADL_Overdrive6_FanSpeed_Reset(int iAdapterIndex);
	int ADL_Overdrive6_ThermalController_Caps(int iAdapterIndex, ADLOD6ThermalControllerCaps *lpThermalControllerCaps);
	int ADL_Overdrive6_Temperature_Get(int iAdapterIndex, int *lpTemperature);
	int ADL_Overdrive6_Capabilities_Get(int iAdapterIndex, ADLOD6Capabilities *lpODCapabilities);
	int ADL_Overdrive6_PowerControl_Caps(int iAdapterIndex, int *lpSupported);
	int ADL_Overdrive6_PowerControlInfo_Get(int iAdapterIndex, ADLOD6PowerControlInfo *lpPowerControlInfo);
	int ADL_Overdrive6_PowerControl_Get(int iAdapterIndex, int *lpCurrentValue, int *lpDefaultValue);
	int ADL_Overdrive6_PowerControl_Set(int iAdapterIndex, int iValue);
	int ADL_Overdrive6_StateInfo_Get(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo);
	int ADL_Overdrive6_State_Set(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo);

	int ADL2_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADL_CONTEXT_HANDLE* lpContext);
	int ADL2_Main_Control_Refresh(ADL_CONTEXT_HANDLE context);
	int ADL2_Main_Control_Destroy(ADL_CONTEXT_HANDLE context);

	int  ADL2_OverdriveN_Capabilities_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNCapabilities *lpODCapabilities); 
	int  ADL2_OverdriveN_SystemClocks_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels);
	int  ADL2_OverdriveN_SystemClocks_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels); 
	int  ADL2_OverdriveN_MemoryClocks_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels); 
	int  ADL2_OverdriveN_MemoryClocks_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels); 
	int  ADL2_OverdriveN_FanControl_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNFanControl *lpODFanSpeed); 
	int  ADL2_OverdriveN_FanControl_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNFanControl *lpODFanControl); 
	int  ADL2_OverdriveN_PowerLimit_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPowerLimitSetting *lpODPowerLimit); 
	int  ADL2_OverdriveN_PowerLimit_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPowerLimitSetting *lpODPowerLimit); 
	int  ADL2_OverdriveN_Temperature_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iTemperatureType, int *iTemperature); 
	int  ADL2_OverdriveN_PerformanceStatus_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceStatus *lpODPerformanceStatus); 
	int  ADL2_OverdriveN_Test_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iEnabled); 
	int  ADL2_OverdriveN_CountOfEvents_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int eventcounterType, int *eventCount); 
	int  ADL2_OverdriveN_CapabilitiesX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNCapabilitiesX2 *lpODCapabilitiesX2); 
	int  ADL2_OverdriveN_SystemClocksX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2);
	int  ADL2_OverdriveN_SystemClocksX2_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2); 
	int  ADL2_OverdriveN_MemoryClocksX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2); 
	int  ADL2_OverdriveN_MemoryClocksX2_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2); 
	BOOL ADL2_OverdriveN_IsClockControlX2Supported();
	int ADL2_OverdriveN_SettingsExt_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpOverdriveNExtCapabilities, int *lpNumberOfODNExtFeatures, ADLODNExtSingleInitSetting** lppInitSettingList, int** lppCurrentSettingList);
	int ADL2_OverdriveN_SettingsExt_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iNumberOfODNExtFeatures, int* itemValueValidList, int* lpItemValueList);

	int ADL2_Overdrive8_Init_Setting_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8InitSetting *lpInitSetting);
	int ADL2_Overdrive8_Current_Setting_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8CurrentSetting *lpCurrentSetting);
	int ADL2_Overdrive8_Setting_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8SetSetting *lpSetSetting, ADLOD8CurrentSetting *lpCurrentSetting);
	int ADL2_New_QueryPMLogData_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLPMLogDataOutput *lpDataOutput);
	int ADL2_Overdrive8_Init_SettingX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int *lpOverdrive8Capabilities, int *lpNumberOfFeatures, ADLOD8SingleInitSetting **lppInitSettingList);
	int ADL2_Overdrive8_Current_SettingX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int *lpNumberOfFeatures, int **lppCurrentSettingList);

	BOOL Init();
	void Uninit();
	BOOL IsInitialized();

	CString GetSystemFilePath(LPCSTR lpFilename);

	CATIADLInterface();
	virtual ~CATIADLInterface();

protected:
	HINSTANCE								m_hATIADLXX;

	ADL_MAIN_CONTROL_CREATE					m_pADL_Main_Control_Create;
	ADL_MAIN_CONTROL_REFRESH				m_pADL_Main_Control_Refresh;
	ADL_MAIN_CONTROL_DESTROY				m_pADL_Main_Control_Destroy;


	ADL_ADAPTER_NUMBEROFADAPTERS_GET		m_pADL_Adapter_NumberOfAdapters_Get;
	ADL_ADAPTER_ADAPTERINFO_GET				m_pADL_Adapter_AdapterInfo_Get;
	ADL_ADAPTER_VIDEOBIOSINFO_GET			m_pADL_Adapter_VideoBiosInfo_Get;
	ADL_ADAPTER_ACCESSIBILITY_GET			m_pADL_Adapter_Accessibility_Get;
	ADL_ADAPTER_ACTIVE_GET					m_pADL_Adapter_Active_Get;
	ADL_ADAPTER_ID_GET						m_pADL_Adapter_ID_Get;
	ADL_ADAPTER_MEMORYINFO_GET				m_pADL_Adapter_MemoryInfo_Get;

	ADL_DISPLAY_WRITEANDREADI2C				m_pADL_Display_WriteAndReadI2C;

	ADL_DISPLAY_DDCBLOCKACCESS_GET			m_pADL_Display_DDCBlockAccess_Get;
	ADL_DISPLAY_MVPUCAPS_GET				m_pADL_Display_MVPUCaps_Get;
	ADL_DISPLAY_MVPUSTATUS_GET				m_pADL_Display_MVPUStatus_Get;

	ADL_OVERDRIVE_CAPS						m_pADL_Overdrive_Caps;

	ADL_OVERDRIVE5_CURRENTACTIVITY_GET		m_pADL_Overdrive5_CurrentActivity_Get;
	ADL_OVERDRIVE5_THERMALDEVICES_ENUM		m_pADL_Overdrive5_ThermalDevices_Enum;
	ADL_OVERDRIVE5_TEMPERATURE_GET			m_pADL_Overdrive5_Temperature_Get;
	ADL_OVERDRIVE5_FANSPEEDINFO_GET			m_pADL_Overdrive5_FanSpeedInfo_Get;
	ADL_OVERDRIVE5_FANSPEED_GET				m_pADL_Overdrive5_FanSpeed_Get;
	ADL_OVERDRIVE5_FANSPEED_SET				m_pADL_Overdrive5_FanSpeed_Set;
	ADL_OVERDRIVE5_FANSPEEDTODEFAULT_SET	m_pADL_Overdrive5_FanSpeedToDefault_Set;
	ADL_OVERDRIVE5_ODPARAMETERS_GET			m_pADL_Overdrive5_ODParameters_Get;
	ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	m_pADL_Overdrive5_ODPerformanceLevels_Get;
	ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET	m_pADL_Overdrive5_ODPerformanceLevels_Set;
	ADL_OVERDRIVE5_POWERCONTROLINFO_GET		m_pADL_Overdrive5_PowerControlInfo_Get;
	ADL_OVERDRIVE5_POWERCONTROL_GET			m_pADL_Overdrive5_PowerControl_Get;
	ADL_OVERDRIVE5_POWERCONTROL_SET			m_pADL_Overdrive5_PowerControl_Set;

	ADL_OVERDRIVE6_CURRENTSTATUS_GET		m_pADL_Overdrive6_CurrentStatus_Get;
	ADL_OVERDRIVE6_VOLTAGECONTROLINFO_GET	m_pADL_Overdrive6_VoltageControlInfo_Get;
	ADL_OVERDRIVE6_VOLTAGECONTROL_GET		m_pADL_Overdrive6_VoltageControl_Get;
	ADL_OVERDRIVE6_VOLTAGECONTROL_SET		m_pADL_Overdrive6_VoltageControl_Set;   
	ADL_OVERDRIVE6_FANSPEED_GET				m_pADL_Overdrive6_FanSpeed_Get;
	ADL_OVERDRIVE6_FANSPEED_SET				m_pADL_Overdrive6_FanSpeed_Set;
	ADL_OVERDRIVE6_FANSPEED_RESET			m_pADL_Overdrive6_FanSpeed_Reset;
	ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS	m_pADL_Overdrive6_ThermalController_Caps;
	ADL_OVERDRIVE6_TEMPERATURE_GET			m_pADL_Overdrive6_Temperature_Get;
	ADL_OVERDRIVE6_CAPABILITIES_GET			m_pADL_Overdrive6_Capabilities_Get;
	ADL_OVERDRIVE6_POWERCONTROL_CAPS		m_pADL_Overdrive6_PowerControl_Caps;
	ADL_OVERDRIVE6_POWERCONTROLINFO_GET		m_pADL_Overdrive6_PowerControlInfo_Get;
	ADL_OVERDRIVE6_POWERCONTROL_GET			m_pADL_Overdrive6_PowerControl_Get;
	ADL_OVERDRIVE6_POWERCONTROL_SET			m_pADL_Overdrive6_PowerControl_Set;
	ADL_OVERDRIVE6_STATEINFO_GET			m_pADL_Overdrive6_StateInfo_Get;
	ADL_OVERDRIVE6_STATE_SET				m_pADL_Overdrive6_State_Set;

	ADL2_MAIN_CONTROL_CREATE				m_pADL2_Main_Control_Create;
	ADL2_MAIN_CONTROL_REFRESH				m_pADL2_Main_Control_Refresh;
	ADL2_MAIN_CONTROL_DESTROY				m_pADL2_Main_Control_Destroy;

	ADL2_OVERDRIVEN_CAPABILITIES_GET		m_pADL2_OverdriveN_Capabilities_Get;
	ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET		m_pADL2_OverdriveN_SystemClocks_Get;
	ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET		m_pADL2_OverdriveN_SystemClocks_Set;
	ADL2_OVERDRIVEN_MEMORYCLOCKS_GET		m_pADL2_OverdriveN_MemoryClocks_Get;
	ADL2_OVERDRIVEN_MEMORYCLOCKS_SET		m_pADL2_OverdriveN_MemoryClocks_Set;
	ADL2_OVERDRIVEN_FANCONTROL_GET			m_pADL2_OverdriveN_FanControl_Get;
	ADL2_OVERDIRVEN_FANCONTROL_SET			m_pADL2_OverdriveN_FanControl_Set;
	ADL2_OVERDRIVEN_POWERLIMIT_GET			m_pADL2_OverdriveN_PowerLimit_Get;
	ADL2_OVERDRIVEN_POWERLIMIT_SET			m_pADL2_OverdriveN_PowerLimit_Set;
	ADL2_OVERDRIVEN_TEMPERATURE_GET			m_pADL2_OverdriveN_Temperature_Get;
	ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET	m_pADL2_OverdriveN_PerformanceStatus_Get;
	ADL2_OVERDRIVEN_TEST_SET				m_pADL2_OverdriveN_Test_Set;
	ADL2_OVERDRIVEN_COUNTOFEVENTS_GET		m_pADL2_OverdriveN_CountOfEvents_Get;
	ADL2_OVERDRIVEN_CAPABILITIESX2_GET		m_pADL2_OverdriveN_CapabilitiesX2_Get;
	ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_GET		m_pADL2_OverdriveN_SystemClocksX2_Get;
	ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_SET		m_pADL2_OverdriveN_SystemClocksX2_Set;
	ADL2_OVERDRIVEN_MEMORYCLOCKSX2_GET		m_pADL2_OverdriveN_MemoryClocksX2_Get;
	ADL2_OVERDRIVEN_MEMORYCLOCKSX2_SET		m_pADL2_OverdriveN_MemoryClocksX2_Set;
	ADL2_OVERDRIVEN_SETTINGSEXT_GET			m_pADL2_OverdriveN_SettingsExt_Get;
	ADL2_OVERDRIVEN_SETTINGSEXT_SET			m_pADL2_OverdriveN_SettingsExt_Set;

	ADL2_OVERDRIVE8_INIT_SETTING_GET		m_pADL2_Overdrive8_Init_Setting_Get;
	ADL2_OVERDRIVE8_CURRENT_SETTING_GET		m_pADL2_Overdrive8_Current_Setting_Get;
	ADL2_OVERDRIVE8_SETTING_SET				m_pADL2_Overdrive8_Setting_Set;
	ADL2_NEW_QUERYPMLOGDATA_GET				m_pADL2_New_QueryPMLogData_Get;
	ADL2_OVERDRIVE8_INIT_SETTINGX2_GET		m_pADL2_Overdrive8_Init_SettingX2_Get;
	ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET	m_pADL2_Overdrive8_Current_SettingX2_Get;
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
