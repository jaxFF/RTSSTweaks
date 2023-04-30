// ATIADLInterface.cpp: implementation of the CATIADLInterface class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ATIADLInterface.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CATIADLInterface::CATIADLInterface()
{
	m_hATIADLXX									= NULL;

	m_pADL_Main_Control_Create					= NULL;
	m_pADL_Main_Control_Refresh					= NULL;
	m_pADL_Main_Control_Destroy					= NULL;

	m_pADL_Adapter_NumberOfAdapters_Get			= NULL;
	m_pADL_Adapter_AdapterInfo_Get				= NULL;
	m_pADL_Adapter_VideoBiosInfo_Get			= NULL;
	m_pADL_Adapter_Accessibility_Get			= NULL;
	m_pADL_Adapter_Active_Get					= NULL;
	m_pADL_Adapter_ID_Get						= NULL;
	m_pADL_Adapter_MemoryInfo_Get				= NULL;

	m_pADL_Display_WriteAndReadI2C				= NULL;
	m_pADL_Display_DDCBlockAccess_Get			= NULL;
	m_pADL_Display_MVPUCaps_Get					= NULL;
	m_pADL_Display_MVPUStatus_Get				= NULL;

	m_pADL_Overdrive_Caps						= NULL;

	m_pADL_Overdrive5_CurrentActivity_Get		= NULL;
	m_pADL_Overdrive5_ThermalDevices_Enum		= NULL;
	m_pADL_Overdrive5_Temperature_Get			= NULL;
	m_pADL_Overdrive5_FanSpeedInfo_Get			= NULL;
	m_pADL_Overdrive5_FanSpeed_Get				= NULL;
	m_pADL_Overdrive5_FanSpeed_Set				= NULL;
	m_pADL_Overdrive5_FanSpeedToDefault_Set		= NULL;
	m_pADL_Overdrive5_ODParameters_Get			= NULL;
	m_pADL_Overdrive5_ODPerformanceLevels_Get	= NULL;
	m_pADL_Overdrive5_ODPerformanceLevels_Set	= NULL;
	m_pADL_Overdrive5_PowerControlInfo_Get		= NULL;
	m_pADL_Overdrive5_PowerControl_Get			= NULL;
	m_pADL_Overdrive5_PowerControl_Set			= NULL;

	m_pADL_Overdrive6_CurrentStatus_Get			= NULL;
	m_pADL_Overdrive6_VoltageControlInfo_Get	= NULL;
	m_pADL_Overdrive6_VoltageControl_Get		= NULL;
	m_pADL_Overdrive6_VoltageControl_Set		= NULL;
	m_pADL_Overdrive6_FanSpeed_Get				= NULL;
	m_pADL_Overdrive6_FanSpeed_Set				= NULL;
	m_pADL_Overdrive6_FanSpeed_Reset			= NULL;
	m_pADL_Overdrive6_ThermalController_Caps	= NULL;
	m_pADL_Overdrive6_Temperature_Get			= NULL;
	m_pADL_Overdrive6_Capabilities_Get			= NULL;
	m_pADL_Overdrive6_PowerControl_Caps			= NULL;
	m_pADL_Overdrive6_PowerControlInfo_Get		= NULL;
	m_pADL_Overdrive6_PowerControl_Get			= NULL;
	m_pADL_Overdrive6_PowerControl_Set			= NULL;
	m_pADL_Overdrive6_StateInfo_Get				= NULL;
	m_pADL_Overdrive6_State_Set					= NULL;

	m_pADL2_Main_Control_Create					= NULL;
	m_pADL2_Main_Control_Refresh				= NULL;
	m_pADL2_Main_Control_Destroy				= NULL;

	m_pADL2_OverdriveN_Capabilities_Get			= NULL;
	m_pADL2_OverdriveN_SystemClocks_Get			= NULL;
	m_pADL2_OverdriveN_SystemClocks_Set			= NULL;
	m_pADL2_OverdriveN_MemoryClocks_Get			= NULL;
	m_pADL2_OverdriveN_MemoryClocks_Set			= NULL;
	m_pADL2_OverdriveN_FanControl_Get			= NULL;
	m_pADL2_OverdriveN_FanControl_Set			= NULL;
	m_pADL2_OverdriveN_PowerLimit_Get			= NULL;
	m_pADL2_OverdriveN_PowerLimit_Set			= NULL;
	m_pADL2_OverdriveN_Temperature_Get			= NULL;
	m_pADL2_OverdriveN_PerformanceStatus_Get	= NULL;
	m_pADL2_OverdriveN_Test_Set					= NULL;
	m_pADL2_OverdriveN_CountOfEvents_Get		= NULL;
	m_pADL2_OverdriveN_CapabilitiesX2_Get		= NULL;
	m_pADL2_OverdriveN_SystemClocksX2_Get		= NULL;
	m_pADL2_OverdriveN_SystemClocksX2_Set		= NULL;
	m_pADL2_OverdriveN_MemoryClocksX2_Get		= NULL;
	m_pADL2_OverdriveN_MemoryClocksX2_Set		= NULL;
	m_pADL2_OverdriveN_SettingsExt_Get			= NULL;
	m_pADL2_OverdriveN_SettingsExt_Set			= NULL;

	m_pADL2_Overdrive8_Init_Setting_Get			= NULL;
	m_pADL2_Overdrive8_Current_Setting_Get		= NULL;
	m_pADL2_Overdrive8_Setting_Set				= NULL;
	m_pADL2_New_QueryPMLogData_Get				= NULL;
	m_pADL2_Overdrive8_Init_SettingX2_Get		= NULL;
	m_pADL2_Overdrive8_Current_SettingX2_Get	= NULL;
}
//////////////////////////////////////////////////////////////////////
CATIADLInterface::~CATIADLInterface()
{
	Uninit();
}
//////////////////////////////////////////////////////////////////////
CString CATIADLInterface::GetSystemFilePath(LPCSTR lpFilename)
{
	char szPath[MAX_PATH];
	GetSystemDirectory(szPath, MAX_PATH);

	strcat_s(szPath, sizeof(szPath), "\\");
	strcat_s(szPath, sizeof(szPath), lpFilename);

	return szPath;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLInterface::Init()
{
	Uninit();

	BOOL bResult = FALSE;

	m_hATIADLXX = LoadLibrary(GetSystemFilePath("atiadlxx.dll"));
		//NOTE: use absolute path to prevent DLL hijacking

    if (!m_hATIADLXX)
		m_hATIADLXX = LoadLibrary(GetSystemFilePath("atiadlxy.dll"));

	if (m_hATIADLXX)
	{
		m_pADL_Main_Control_Create					= (ADL_MAIN_CONTROL_CREATE					)GetProcAddress(m_hATIADLXX, "ADL_Main_Control_Create"					);
		m_pADL_Main_Control_Refresh					= (ADL_MAIN_CONTROL_REFRESH					)GetProcAddress(m_hATIADLXX, "ADL_Main_Control_Refresh"					);
		m_pADL_Main_Control_Destroy					= (ADL_MAIN_CONTROL_DESTROY					)GetProcAddress(m_hATIADLXX, "ADL_Main_Control_Destroy"					);

		m_pADL_Adapter_NumberOfAdapters_Get			= (ADL_ADAPTER_NUMBEROFADAPTERS_GET			)GetProcAddress(m_hATIADLXX, "ADL_Adapter_NumberOfAdapters_Get"			);
		m_pADL_Adapter_AdapterInfo_Get				= (ADL_ADAPTER_ADAPTERINFO_GET				)GetProcAddress(m_hATIADLXX, "ADL_Adapter_AdapterInfo_Get"				);
		m_pADL_Adapter_VideoBiosInfo_Get			= (ADL_ADAPTER_VIDEOBIOSINFO_GET			)GetProcAddress(m_hATIADLXX, "ADL_Adapter_VideoBiosInfo_Get"			);
		m_pADL_Adapter_Accessibility_Get			= (ADL_ADAPTER_ACCESSIBILITY_GET			)GetProcAddress(m_hATIADLXX, "ADL_Adapter_Accessibility_Get"			);
		m_pADL_Adapter_Active_Get					= (ADL_ADAPTER_ACTIVE_GET					)GetProcAddress(m_hATIADLXX, "ADL_Adapter_Active_Get"					);
		m_pADL_Adapter_ID_Get						= (ADL_ADAPTER_ID_GET						)GetProcAddress(m_hATIADLXX, "ADL_Adapter_ID_Get"						);
		m_pADL_Adapter_MemoryInfo_Get				= (ADL_ADAPTER_MEMORYINFO_GET				)GetProcAddress(m_hATIADLXX, "ADL_Adapter_MemoryInfo_Get"				);


		m_pADL_Display_WriteAndReadI2C				= (ADL_DISPLAY_WRITEANDREADI2C				)GetProcAddress(m_hATIADLXX, "ADL_Display_WriteAndReadI2C"				);
		m_pADL_Display_DDCBlockAccess_Get			= (ADL_DISPLAY_DDCBLOCKACCESS_GET			)GetProcAddress(m_hATIADLXX, "ADL_Display_DDCBlockAccess_Get"			);
		m_pADL_Display_MVPUCaps_Get					= (ADL_DISPLAY_MVPUCAPS_GET					)GetProcAddress(m_hATIADLXX, "ADL_Display_MVPUCaps_Get"					);
		m_pADL_Display_MVPUStatus_Get				= (ADL_DISPLAY_MVPUSTATUS_GET				)GetProcAddress(m_hATIADLXX, "ADL_Display_MVPUStatus_Get"				);

		m_pADL_Overdrive_Caps						= (ADL_OVERDRIVE_CAPS						)GetProcAddress(m_hATIADLXX, "ADL_Overdrive_Caps"						);

		m_pADL_Overdrive5_CurrentActivity_Get		= (ADL_OVERDRIVE5_CURRENTACTIVITY_GET		)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_CurrentActivity_Get"		);
		m_pADL_Overdrive5_ThermalDevices_Enum		= (ADL_OVERDRIVE5_THERMALDEVICES_ENUM		)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_ThermalDevices_Enum"		);
		m_pADL_Overdrive5_Temperature_Get			= (ADL_OVERDRIVE5_TEMPERATURE_GET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_Temperature_Get"			);
		m_pADL_Overdrive5_FanSpeedInfo_Get			= (ADL_OVERDRIVE5_FANSPEEDINFO_GET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_FanSpeedInfo_Get"			);
		m_pADL_Overdrive5_FanSpeed_Get				= (ADL_OVERDRIVE5_FANSPEED_GET				)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_FanSpeed_Get"				);
		m_pADL_Overdrive5_FanSpeed_Set				= (ADL_OVERDRIVE5_FANSPEED_SET				)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_FanSpeed_Set"				);
		m_pADL_Overdrive5_FanSpeedToDefault_Set		= (ADL_OVERDRIVE5_FANSPEEDTODEFAULT_SET		)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_FanSpeedToDefault_Set"		);
		m_pADL_Overdrive5_ODParameters_Get			= (ADL_OVERDRIVE5_ODPARAMETERS_GET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_ODParameters_Get"			);
		m_pADL_Overdrive5_ODPerformanceLevels_Get	= (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_GET	)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_ODPerformanceLevels_Get"	);
		m_pADL_Overdrive5_ODPerformanceLevels_Set	= (ADL_OVERDRIVE5_ODPERFORMANCELEVELS_SET	)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_ODPerformanceLevels_Set"	);
		m_pADL_Overdrive5_PowerControlInfo_Get		= (ADL_OVERDRIVE5_POWERCONTROLINFO_GET		)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_PowerControlInfo_Get"		);
		m_pADL_Overdrive5_PowerControl_Get			= (ADL_OVERDRIVE5_POWERCONTROL_GET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_PowerControl_Get"			);
		m_pADL_Overdrive5_PowerControl_Set			= (ADL_OVERDRIVE5_POWERCONTROL_SET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive5_PowerControl_Set"			);

		m_pADL_Overdrive6_CurrentStatus_Get			= (ADL_OVERDRIVE6_CURRENTSTATUS_GET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_CurrentStatus_Get"			);
		m_pADL_Overdrive6_VoltageControlInfo_Get	= (ADL_OVERDRIVE6_VOLTAGECONTROLINFO_GET	)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_VoltageControlInfo_Get"	);
		m_pADL_Overdrive6_VoltageControl_Get		= (ADL_OVERDRIVE6_VOLTAGECONTROL_GET		)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_VoltageControl_Get"		);
		m_pADL_Overdrive6_VoltageControl_Set		= (ADL_OVERDRIVE6_VOLTAGECONTROL_SET		)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_VoltageControl_Set"		);   
		m_pADL_Overdrive6_FanSpeed_Get				= (ADL_OVERDRIVE6_FANSPEED_GET				)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_FanSpeed_Get"				);
		m_pADL_Overdrive6_FanSpeed_Set				= (ADL_OVERDRIVE6_FANSPEED_SET				)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_FanSpeed_Set"				);
		m_pADL_Overdrive6_FanSpeed_Reset			= (ADL_OVERDRIVE6_FANSPEED_RESET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_FanSpeed_Reset"			);
		m_pADL_Overdrive6_ThermalController_Caps	= (ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS	)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_ThermalController_Caps"	);
		m_pADL_Overdrive6_Temperature_Get			= (ADL_OVERDRIVE6_TEMPERATURE_GET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_Temperature_Get"			);
		m_pADL_Overdrive6_Capabilities_Get			= (ADL_OVERDRIVE6_CAPABILITIES_GET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_Capabilities_Get"			);
		m_pADL_Overdrive6_PowerControl_Caps			= (ADL_OVERDRIVE6_POWERCONTROL_CAPS			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_PowerControl_Caps"			);
		m_pADL_Overdrive6_PowerControlInfo_Get		= (ADL_OVERDRIVE6_POWERCONTROLINFO_GET		)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_PowerControlInfo_Get"		);
		m_pADL_Overdrive6_PowerControl_Get			= (ADL_OVERDRIVE6_POWERCONTROL_GET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_PowerControl_Get"			);
		m_pADL_Overdrive6_PowerControl_Set			= (ADL_OVERDRIVE6_POWERCONTROL_SET			)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_PowerControl_Set"			);
		m_pADL_Overdrive6_StateInfo_Get				= (ADL_OVERDRIVE6_STATEINFO_GET				)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_StateInfo_Get"				);
		m_pADL_Overdrive6_State_Set					= (ADL_OVERDRIVE6_STATE_SET					)GetProcAddress(m_hATIADLXX, "ADL_Overdrive6_State_Set"					);

		m_pADL2_Main_Control_Create					= (ADL2_MAIN_CONTROL_CREATE					)GetProcAddress(m_hATIADLXX, "ADL2_Main_Control_Create"					);
		m_pADL2_Main_Control_Refresh				= (ADL2_MAIN_CONTROL_REFRESH				)GetProcAddress(m_hATIADLXX, "ADL2_Main_Control_Refresh"				);
		m_pADL2_Main_Control_Destroy				= (ADL2_MAIN_CONTROL_DESTROY				)GetProcAddress(m_hATIADLXX, "ADL2_Main_Control_Destroy"				);

		m_pADL2_OverdriveN_Capabilities_Get			= (ADL2_OVERDRIVEN_CAPABILITIES_GET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_Capabilities_Get"			);
		m_pADL2_OverdriveN_SystemClocks_Get			= (ADL2_OVERDRIVEN_SYSTEMCLOCKS_GET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_SystemClocks_Get"			);
		m_pADL2_OverdriveN_SystemClocks_Set			= (ADL2_OVERDRIVEN_SYSTEMCLOCKS_SET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_SystemClocks_Set"			);
		m_pADL2_OverdriveN_MemoryClocks_Get			= (ADL2_OVERDRIVEN_MEMORYCLOCKS_GET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_MemoryClocks_Get"			);
		m_pADL2_OverdriveN_MemoryClocks_Set			= (ADL2_OVERDRIVEN_MEMORYCLOCKS_SET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_MemoryClocks_Set"			);
		m_pADL2_OverdriveN_FanControl_Get			= (ADL2_OVERDRIVEN_FANCONTROL_GET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_FanControl_Get"			);
		m_pADL2_OverdriveN_FanControl_Set			= (ADL2_OVERDIRVEN_FANCONTROL_SET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_FanControl_Set"			);
		m_pADL2_OverdriveN_PowerLimit_Get			= (ADL2_OVERDRIVEN_POWERLIMIT_GET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_PowerLimit_Get"			);
		m_pADL2_OverdriveN_PowerLimit_Set			= (ADL2_OVERDRIVEN_POWERLIMIT_SET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_PowerLimit_Set"			);
		m_pADL2_OverdriveN_Temperature_Get			= (ADL2_OVERDRIVEN_TEMPERATURE_GET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_Temperature_Get"			);
		m_pADL2_OverdriveN_PerformanceStatus_Get	= (ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET	)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_PerformanceStatus_Get"	);
		m_pADL2_OverdriveN_Test_Set					= (ADL2_OVERDRIVEN_TEST_SET					)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_Test_Set"					);
		m_pADL2_OverdriveN_CountOfEvents_Get		= (ADL2_OVERDRIVEN_COUNTOFEVENTS_GET		)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_CountOfEvents_Get"		);

		m_pADL2_OverdriveN_CapabilitiesX2_Get		= (ADL2_OVERDRIVEN_CAPABILITIESX2_GET		)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_CapabilitiesX2_Get"		);
		m_pADL2_OverdriveN_SystemClocksX2_Get		= (ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_GET		)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_SystemClocksX2_Get"		);
		m_pADL2_OverdriveN_SystemClocksX2_Set		= (ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_SET		)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_SystemClocksX2_Set"		);
		m_pADL2_OverdriveN_MemoryClocksX2_Get		= (ADL2_OVERDRIVEN_MEMORYCLOCKSX2_GET		)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_MemoryClocksX2_Get"		);
		m_pADL2_OverdriveN_MemoryClocksX2_Set		= (ADL2_OVERDRIVEN_MEMORYCLOCKSX2_SET		)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_MemoryClocksX2_Set"		);

		m_pADL2_OverdriveN_SettingsExt_Get			= (ADL2_OVERDRIVEN_SETTINGSEXT_GET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_SettingsExt_Get"			);
		m_pADL2_OverdriveN_SettingsExt_Set			= (ADL2_OVERDRIVEN_SETTINGSEXT_SET			)GetProcAddress(m_hATIADLXX, "ADL2_OverdriveN_SettingsExt_Set"			);

		m_pADL2_Overdrive8_Init_Setting_Get			= (ADL2_OVERDRIVE8_INIT_SETTING_GET			)GetProcAddress(m_hATIADLXX, "ADL2_Overdrive8_Init_Setting_Get"			);
		m_pADL2_Overdrive8_Current_Setting_Get		= (ADL2_OVERDRIVE8_CURRENT_SETTING_GET		)GetProcAddress(m_hATIADLXX, "ADL2_Overdrive8_Current_Setting_Get"		);
		m_pADL2_Overdrive8_Setting_Set				= (ADL2_OVERDRIVE8_SETTING_SET				)GetProcAddress(m_hATIADLXX, "ADL2_Overdrive8_Setting_Set"				);
		m_pADL2_New_QueryPMLogData_Get				= (ADL2_NEW_QUERYPMLOGDATA_GET				)GetProcAddress(m_hATIADLXX, "ADL2_New_QueryPMLogData_Get"				);
		m_pADL2_Overdrive8_Init_SettingX2_Get		= (ADL2_OVERDRIVE8_INIT_SETTINGX2_GET		)GetProcAddress(m_hATIADLXX, "ADL2_Overdrive8_Init_SettingX2_Get"		);
		m_pADL2_Overdrive8_Current_SettingX2_Get	= (ADL2_OVERDRIVE8_CURRENT_SETTINGX2_GET	)GetProcAddress(m_hATIADLXX, "ADL2_Overdrive8_Current_SettingX2_Get"	);

		if (m_pADL_Main_Control_Create && m_pADL_Main_Control_Destroy)
			bResult = TRUE;
	}

	if (!bResult)
		Uninit();
	
	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLInterface::IsInitialized()
{
	return (m_hATIADLXX != NULL);
}
//////////////////////////////////////////////////////////////////////
void CATIADLInterface::Uninit()
{
	if (m_hATIADLXX)
		FreeLibrary(m_hATIADLXX);

	m_hATIADLXX									= NULL;

	m_pADL_Main_Control_Create					= NULL;
	m_pADL_Main_Control_Refresh					= NULL;
	m_pADL_Main_Control_Destroy					= NULL;

	m_pADL_Adapter_NumberOfAdapters_Get			= NULL;
	m_pADL_Adapter_AdapterInfo_Get				= NULL;
	m_pADL_Adapter_VideoBiosInfo_Get			= NULL;
	m_pADL_Adapter_Accessibility_Get			= NULL;
	m_pADL_Adapter_Active_Get					= NULL;
	m_pADL_Adapter_ID_Get						= NULL;
	m_pADL_Adapter_MemoryInfo_Get				= NULL;

	m_pADL_Display_WriteAndReadI2C				= NULL;
	m_pADL_Display_DDCBlockAccess_Get			= NULL;
	m_pADL_Display_MVPUCaps_Get					= NULL;
	m_pADL_Display_MVPUStatus_Get				= NULL;

	m_pADL_Overdrive_Caps						= NULL;

	m_pADL_Overdrive5_CurrentActivity_Get		= NULL;
	m_pADL_Overdrive5_ThermalDevices_Enum		= NULL;
	m_pADL_Overdrive5_Temperature_Get			= NULL;
	m_pADL_Overdrive5_FanSpeedInfo_Get			= NULL;
	m_pADL_Overdrive5_FanSpeed_Get				= NULL;
	m_pADL_Overdrive5_FanSpeed_Set				= NULL;
	m_pADL_Overdrive5_FanSpeedToDefault_Set		= NULL;
	m_pADL_Overdrive5_ODParameters_Get			= NULL;
	m_pADL_Overdrive5_ODPerformanceLevels_Get	= NULL;
	m_pADL_Overdrive5_ODPerformanceLevels_Set	= NULL;
	m_pADL_Overdrive5_PowerControlInfo_Get		= NULL;
	m_pADL_Overdrive5_PowerControl_Get			= NULL;
	m_pADL_Overdrive5_PowerControl_Set			= NULL;

	m_pADL_Overdrive6_CurrentStatus_Get			= NULL;
	m_pADL_Overdrive6_VoltageControlInfo_Get	= NULL;
	m_pADL_Overdrive6_VoltageControl_Get		= NULL;
	m_pADL_Overdrive6_VoltageControl_Set		= NULL;   
	m_pADL_Overdrive6_FanSpeed_Get				= NULL;
	m_pADL_Overdrive6_FanSpeed_Set				= NULL;
	m_pADL_Overdrive6_FanSpeed_Reset			= NULL;
	m_pADL_Overdrive6_ThermalController_Caps	= NULL;
	m_pADL_Overdrive6_Temperature_Get			= NULL;
	m_pADL_Overdrive6_Capabilities_Get			= NULL;
	m_pADL_Overdrive6_PowerControl_Caps			= NULL;
	m_pADL_Overdrive6_PowerControlInfo_Get		= NULL;
	m_pADL_Overdrive6_PowerControl_Get			= NULL;
	m_pADL_Overdrive6_PowerControl_Set			= NULL;
	m_pADL_Overdrive6_StateInfo_Get				= NULL;
	m_pADL_Overdrive6_State_Set					= NULL;

	m_pADL2_Main_Control_Create					= NULL;
	m_pADL2_Main_Control_Refresh				= NULL;
	m_pADL2_Main_Control_Destroy				= NULL;

	m_pADL2_OverdriveN_Capabilities_Get			= NULL;
	m_pADL2_OverdriveN_SystemClocks_Get			= NULL;
	m_pADL2_OverdriveN_SystemClocks_Set			= NULL;
	m_pADL2_OverdriveN_MemoryClocks_Get			= NULL;
	m_pADL2_OverdriveN_MemoryClocks_Set			= NULL;
	m_pADL2_OverdriveN_FanControl_Get			= NULL;
	m_pADL2_OverdriveN_FanControl_Set			= NULL;
	m_pADL2_OverdriveN_PowerLimit_Get			= NULL;
	m_pADL2_OverdriveN_PowerLimit_Set			= NULL;
	m_pADL2_OverdriveN_Temperature_Get			= NULL;
	m_pADL2_OverdriveN_PerformanceStatus_Get	= NULL;
	m_pADL2_OverdriveN_Test_Set					= NULL;
	m_pADL2_OverdriveN_CountOfEvents_Get		= NULL;
	m_pADL2_OverdriveN_CapabilitiesX2_Get		= NULL;
	m_pADL2_OverdriveN_SystemClocksX2_Get		= NULL;
	m_pADL2_OverdriveN_SystemClocksX2_Set		= NULL;
	m_pADL2_OverdriveN_MemoryClocksX2_Get		= NULL;
	m_pADL2_OverdriveN_MemoryClocksX2_Set		= NULL;
	m_pADL2_OverdriveN_SettingsExt_Get			= NULL;
	m_pADL2_OverdriveN_SettingsExt_Set			= NULL;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters)
{
	if (m_pADL_Main_Control_Create)
		return m_pADL_Main_Control_Create(callback, iEnumConnectedAdapters);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Main_Control_Refresh()
{
	if (m_pADL_Main_Control_Refresh)
		return m_pADL_Main_Control_Refresh();

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Main_Control_Destroy()
{
	if (m_pADL_Main_Control_Destroy)
		return m_pADL_Main_Control_Destroy();

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Adapter_NumberOfAdapters_Get(int *lpNumAdapters)
{
	if (m_pADL_Adapter_NumberOfAdapters_Get)
		return m_pADL_Adapter_NumberOfAdapters_Get(lpNumAdapters);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Adapter_AdapterInfo_Get(LPAdapterInfo lpInfo, int iInputSize)
{
	if (m_pADL_Adapter_AdapterInfo_Get)
		return m_pADL_Adapter_AdapterInfo_Get(lpInfo, iInputSize);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Display_WriteAndReadI2C(int iAdapterIndex,  ADLI2C *  lpI2C)
{
	if (m_pADL_Display_WriteAndReadI2C)
		return m_pADL_Display_WriteAndReadI2C(iAdapterIndex, lpI2C);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Display_MVPUCaps_Get(int iAdapterIndex, ADLMVPUCaps* lpMvpuCaps)
{
	if (m_pADL_Display_MVPUCaps_Get)
		return m_pADL_Display_MVPUCaps_Get(iAdapterIndex, lpMvpuCaps);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Display_MVPUStatus_Get(int iAdapterIndex, ADLMVPUStatus* lpMvpuStatus)
{
	if (m_pADL_Display_MVPUStatus_Get)
		return m_pADL_Display_MVPUStatus_Get(iAdapterIndex, lpMvpuStatus);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_CurrentActivity_Get(int iAdapterIndex, ADLPMActivity *lpActivity)
{
	if (m_pADL_Overdrive5_CurrentActivity_Get)
		return m_pADL_Overdrive5_CurrentActivity_Get(iAdapterIndex, lpActivity);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_ThermalDevices_Enum(int iAdapterIndex, int iThermalControllerIndex, ADLThermalControllerInfo *lpThermalControllerInfo)
{
	if (m_pADL_Overdrive5_ThermalDevices_Enum)
		return m_pADL_Overdrive5_ThermalDevices_Enum(iAdapterIndex, iThermalControllerIndex, lpThermalControllerInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_Temperature_Get(int iAdapterIndex, int iThermalControllerIndex, ADLTemperature *lpTemperature)
{
	if (m_pADL_Overdrive5_Temperature_Get)
		return m_pADL_Overdrive5_Temperature_Get(iAdapterIndex, iThermalControllerIndex, lpTemperature);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_FanSpeedInfo_Get(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedInfo *lpFanSpeedInfo)
{
	if (m_pADL_Overdrive5_FanSpeedInfo_Get)
		return m_pADL_Overdrive5_FanSpeedInfo_Get(iAdapterIndex, iThermalControllerIndex, lpFanSpeedInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_FanSpeed_Get(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue)
{
	if (m_pADL_Overdrive5_FanSpeed_Get)
		return m_pADL_Overdrive5_FanSpeed_Get(iAdapterIndex, iThermalControllerIndex, lpFanSpeedValue);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_FanSpeed_Set(int iAdapterIndex, int iThermalControllerIndex, ADLFanSpeedValue *lpFanSpeedValue)
{
	if (m_pADL_Overdrive5_FanSpeed_Set)
		return m_pADL_Overdrive5_FanSpeed_Set(iAdapterIndex, iThermalControllerIndex, lpFanSpeedValue);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_FanSpeedToDefault_Set(int iAdapterIndex, int iThermalControllerIndex)
{
	if (m_pADL_Overdrive5_FanSpeedToDefault_Set)
		return m_pADL_Overdrive5_FanSpeedToDefault_Set(iAdapterIndex, iThermalControllerIndex);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_ODParameters_Get(int iAdapterIndex, ADLODParameters *lpOdParameters)
{
	if (m_pADL_Overdrive5_ODParameters_Get)
		return m_pADL_Overdrive5_ODParameters_Get(iAdapterIndex, lpOdParameters);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_ODPerformanceLevels_Get(int iAdapterIndex, int iDefault, ADLODPerformanceLevels *lpOdPerformanceLevels)
{
	if (m_pADL_Overdrive5_ODPerformanceLevels_Get)
		return m_pADL_Overdrive5_ODPerformanceLevels_Get(iAdapterIndex, iDefault, lpOdPerformanceLevels);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_ODPerformanceLevels_Set(int iAdapterIndex, ADLODPerformanceLevels *lpOdPerformanceLevels)
{
	if (m_pADL_Overdrive5_ODPerformanceLevels_Set)
		return m_pADL_Overdrive5_ODPerformanceLevels_Set(iAdapterIndex, lpOdPerformanceLevels);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Adapter_VideoBiosInfo_Get(int iAdapterIndex, ADLBiosInfo *lpBiosInfo)
{
	if (m_pADL_Adapter_VideoBiosInfo_Get)
		return m_pADL_Adapter_VideoBiosInfo_Get(iAdapterIndex, lpBiosInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Adapter_Accessibility_Get(int iAdapterIndex, int *lpAccessibility)
{
	if (m_pADL_Adapter_Accessibility_Get)
		return m_pADL_Adapter_Accessibility_Get(iAdapterIndex, lpAccessibility);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Adapter_Active_Get(int iAdapterIndex, int *lpStatus)
{
	if (m_pADL_Adapter_Active_Get)
		return m_pADL_Adapter_Active_Get(iAdapterIndex, lpStatus);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Adapter_ID_Get(int iAdapterIndex, int *lpAdapterID)
{
	if (m_pADL_Adapter_ID_Get)
		return m_pADL_Adapter_ID_Get(iAdapterIndex, lpAdapterID);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Adapter_MemoryInfo_Get(int iAdapterIndex, ADLMemoryInfo *lpMemoryInfo)
{
	if (m_pADL_Adapter_MemoryInfo_Get)
		return m_pADL_Adapter_MemoryInfo_Get(iAdapterIndex, lpMemoryInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Display_DDCBlockAccess_Get(int iAdapterIndex, int iDisplayIndex, int iOption, int iCommandIndex, int iSendMsgLen, char *lpucSendMsgBuf, int *lpulRecvMsgLen, char *lpucRecvMsgBuf)
{
	if (m_pADL_Display_DDCBlockAccess_Get)
		return m_pADL_Display_DDCBlockAccess_Get(iAdapterIndex, iDisplayIndex, iOption, iCommandIndex, iSendMsgLen, lpucSendMsgBuf, lpulRecvMsgLen, lpucRecvMsgBuf);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_PowerControlInfo_Get(int iAdapterIndex, ADLPowerControlInfo* lpPowerControlInfo)
{
	if (m_pADL_Overdrive5_PowerControlInfo_Get)
		return m_pADL_Overdrive5_PowerControlInfo_Get(iAdapterIndex, lpPowerControlInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_PowerControl_Get(int iAdapterIndex, int* lpValue, int* lpReserved)
{
	if (m_pADL_Overdrive5_PowerControl_Get)
		return m_pADL_Overdrive5_PowerControl_Get(iAdapterIndex, lpValue, lpReserved);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive5_PowerControl_Set(int iAdapterIndex, int iValue)
{
	if (m_pADL_Overdrive5_PowerControl_Set)
		return m_pADL_Overdrive5_PowerControl_Set(iAdapterIndex, iValue);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive_Caps(int iAdapterIndex, int *iSupported, int *iEnabled, int *iVersion)
{
	if (m_pADL_Overdrive_Caps)
		return m_pADL_Overdrive_Caps(iAdapterIndex, iSupported, iEnabled, iVersion);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_CurrentStatus_Get(int iAdapterIndex, ADLOD6CurrentStatus *lpCurrentStatus)
{
	if (m_pADL_Overdrive6_CurrentStatus_Get)
		return m_pADL_Overdrive6_CurrentStatus_Get(iAdapterIndex, lpCurrentStatus);;

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_VoltageControlInfo_Get(int iAdapterIndex, ADLOD6VoltageControlInfo* lpVoltageControlInfo)
{
	if (m_pADL_Overdrive6_VoltageControlInfo_Get)
		return m_pADL_Overdrive6_VoltageControlInfo_Get(iAdapterIndex, lpVoltageControlInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_VoltageControl_Get(int iAdapterIndex, int*  lpCurrentValue, int* lpDefaultValue)
{
	if (m_pADL_Overdrive6_VoltageControl_Get)
		return m_pADL_Overdrive6_VoltageControl_Get(iAdapterIndex, lpCurrentValue, lpDefaultValue);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_VoltageControl_Set(int iAdapterIndex, int iValue)
{
	if (m_pADL_Overdrive6_VoltageControl_Set)
		return m_pADL_Overdrive6_VoltageControl_Set(iAdapterIndex, iValue);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_FanSpeed_Get(int iAdapterIndex, ADLOD6FanSpeedInfo *lpFanSpeedInfo)
{
	if (m_pADL_Overdrive6_FanSpeed_Get)
		return m_pADL_Overdrive6_FanSpeed_Get(iAdapterIndex, lpFanSpeedInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_FanSpeed_Set(int iAdapterIndex, ADLOD6FanSpeedValue *lpFanSpeedValue)
{
	if (m_pADL_Overdrive6_FanSpeed_Set)
		return m_pADL_Overdrive6_FanSpeed_Set(iAdapterIndex, lpFanSpeedValue);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_FanSpeed_Reset(int iAdapterIndex)
{
	if (m_pADL_Overdrive6_FanSpeed_Reset)
		return m_pADL_Overdrive6_FanSpeed_Reset(iAdapterIndex);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_ThermalController_Caps(int iAdapterIndex, ADLOD6ThermalControllerCaps *lpThermalControllerCaps)
{
	if (m_pADL_Overdrive6_ThermalController_Caps)
		return m_pADL_Overdrive6_ThermalController_Caps(iAdapterIndex, lpThermalControllerCaps);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_Temperature_Get(int iAdapterIndex, int *lpTemperature)
{
	if (m_pADL_Overdrive6_Temperature_Get)
		return m_pADL_Overdrive6_Temperature_Get(iAdapterIndex, lpTemperature);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_Capabilities_Get(int iAdapterIndex, ADLOD6Capabilities *lpODCapabilities)
{
	if (m_pADL_Overdrive6_Capabilities_Get)
		return m_pADL_Overdrive6_Capabilities_Get(iAdapterIndex, lpODCapabilities);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_PowerControl_Caps(int iAdapterIndex, int *lpSupported)
{
	if (m_pADL_Overdrive6_PowerControl_Caps)
		return m_pADL_Overdrive6_PowerControl_Caps(iAdapterIndex, lpSupported);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_PowerControlInfo_Get(int iAdapterIndex, ADLOD6PowerControlInfo *lpPowerControlInfo)
{
	if (m_pADL_Overdrive6_PowerControlInfo_Get)
		return m_pADL_Overdrive6_PowerControlInfo_Get(iAdapterIndex, lpPowerControlInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_PowerControl_Get(int iAdapterIndex, int *lpCurrentValue, int *lpDefaultValue)
{
	if (m_pADL_Overdrive6_PowerControl_Get)
		return m_pADL_Overdrive6_PowerControl_Get(iAdapterIndex, lpCurrentValue, lpDefaultValue);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_PowerControl_Set(int iAdapterIndex, int iValue)
{
	if (m_pADL_Overdrive6_PowerControl_Set)
		return m_pADL_Overdrive6_PowerControl_Set(iAdapterIndex, iValue);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_StateInfo_Get(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo)
{
	if (m_pADL_Overdrive6_StateInfo_Get)
		return m_pADL_Overdrive6_StateInfo_Get(iAdapterIndex, iStateType, lpStateInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL_Overdrive6_State_Set(int iAdapterIndex, int iStateType, ADLOD6StateInfo *lpStateInfo)
{
	if (m_pADL_Overdrive6_State_Set)
		return m_pADL_Overdrive6_State_Set(iAdapterIndex, iStateType, lpStateInfo);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_Main_Control_Create(ADL_MAIN_MALLOC_CALLBACK callback, int iEnumConnectedAdapters, ADL_CONTEXT_HANDLE* lpContext)
{
	if (m_pADL2_Main_Control_Create)
		return m_pADL2_Main_Control_Create(callback, iEnumConnectedAdapters, lpContext);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_Main_Control_Refresh(ADL_CONTEXT_HANDLE context)
{
	if (m_pADL2_Main_Control_Refresh)
		return m_pADL2_Main_Control_Refresh(context);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_Main_Control_Destroy(ADL_CONTEXT_HANDLE context)
{
	if (m_pADL2_Main_Control_Destroy)
		return m_pADL2_Main_Control_Destroy(context);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_Capabilities_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNCapabilities *lpODCapabilities)
{
	if (m_pADL2_OverdriveN_Capabilities_Get)
		return m_pADL2_OverdriveN_Capabilities_Get(context, iAdapterIndex, lpODCapabilities);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_SystemClocks_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels)
{
	if (m_pADL2_OverdriveN_SystemClocks_Get)
		return m_pADL2_OverdriveN_SystemClocks_Get(context, iAdapterIndex, lpODPerformanceLevels);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_SystemClocks_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels)
{
	if (m_pADL2_OverdriveN_SystemClocks_Set)
		return m_pADL2_OverdriveN_SystemClocks_Set(context, iAdapterIndex, lpODPerformanceLevels);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_MemoryClocks_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels)
{
	if (m_pADL2_OverdriveN_MemoryClocks_Get)
		return m_pADL2_OverdriveN_MemoryClocks_Get(context, iAdapterIndex, lpODPerformanceLevels);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_MemoryClocks_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevels *lpODPerformanceLevels)
{
	if (m_pADL2_OverdriveN_MemoryClocks_Set)
		return m_pADL2_OverdriveN_MemoryClocks_Set(context, iAdapterIndex, lpODPerformanceLevels);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_FanControl_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNFanControl *lpODFanSpeed)
{
	if (m_pADL2_OverdriveN_FanControl_Get)
		return m_pADL2_OverdriveN_FanControl_Get(context, iAdapterIndex, lpODFanSpeed);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_FanControl_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNFanControl *lpODFanControl)
{
	if (m_pADL2_OverdriveN_FanControl_Set)
		return m_pADL2_OverdriveN_FanControl_Set(context, iAdapterIndex, lpODFanControl);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_PowerLimit_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPowerLimitSetting *lpODPowerLimit)
{
	if (m_pADL2_OverdriveN_PowerLimit_Get)
		return m_pADL2_OverdriveN_PowerLimit_Get(context, iAdapterIndex, lpODPowerLimit);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_PowerLimit_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPowerLimitSetting *lpODPowerLimit)
{
	if (m_pADL2_OverdriveN_PowerLimit_Set)
		return m_pADL2_OverdriveN_PowerLimit_Set(context, iAdapterIndex, lpODPowerLimit);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_Temperature_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iTemperatureType, int *iTemperature)
{
	if (m_pADL2_OverdriveN_Temperature_Get)
		return m_pADL2_OverdriveN_Temperature_Get(context, iAdapterIndex, iTemperatureType, iTemperature);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_PerformanceStatus_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceStatus *lpODPerformanceStatus)
{
	if (m_pADL2_OverdriveN_PerformanceStatus_Get)
		return m_pADL2_OverdriveN_PerformanceStatus_Get(context, iAdapterIndex, lpODPerformanceStatus);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_Test_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iEnabled)
{
	if (m_pADL2_OverdriveN_Test_Set)
		return m_pADL2_OverdriveN_Test_Set(context, iAdapterIndex, iEnabled);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_CountOfEvents_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int eventcounterType, int *eventCount)
{
	if (m_pADL2_OverdriveN_CountOfEvents_Get)
		return m_pADL2_OverdriveN_CountOfEvents_Get(context, iAdapterIndex, eventcounterType, eventCount);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_CapabilitiesX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNCapabilitiesX2 *lpODCapabilitiesX2)
{
	if (m_pADL2_OverdriveN_CapabilitiesX2_Get)
		return m_pADL2_OverdriveN_CapabilitiesX2_Get(context, iAdapterIndex, lpODCapabilitiesX2);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_SystemClocksX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2)
{
	if (m_pADL2_OverdriveN_SystemClocksX2_Get)
		return m_pADL2_OverdriveN_SystemClocksX2_Get(context, iAdapterIndex, lpODPerformanceLevelsX2);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_SystemClocksX2_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2)
{
	if (m_pADL2_OverdriveN_SystemClocksX2_Set)
		return m_pADL2_OverdriveN_SystemClocksX2_Set(context, iAdapterIndex, lpODPerformanceLevelsX2);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_MemoryClocksX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2)
{
	if (m_pADL2_OverdriveN_MemoryClocksX2_Get)
		return m_pADL2_OverdriveN_MemoryClocksX2_Get(context, iAdapterIndex, lpODPerformanceLevelsX2);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_MemoryClocksX2_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLODNPerformanceLevelsX2 *lpODPerformanceLevelsX2)
{
	if (m_pADL2_OverdriveN_MemoryClocksX2_Set)
		return m_pADL2_OverdriveN_MemoryClocksX2_Set(context, iAdapterIndex, lpODPerformanceLevelsX2);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_SettingsExt_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpOverdriveNExtCapabilities, int *lpNumberOfODNExtFeatures, ADLODNExtSingleInitSetting** lppInitSettingList, int** lppCurrentSettingList)
{
	if (m_pADL2_OverdriveN_SettingsExt_Get)
		return m_pADL2_OverdriveN_SettingsExt_Get(context, iAdapterIndex, lpOverdriveNExtCapabilities, lpNumberOfODNExtFeatures, lppInitSettingList, lppCurrentSettingList);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_OverdriveN_SettingsExt_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iNumberOfODNExtFeatures, int* itemValueValidList, int* lpItemValueList)
{
	if (m_pADL2_OverdriveN_SettingsExt_Set)
		return m_pADL2_OverdriveN_SettingsExt_Set(context, iAdapterIndex, iNumberOfODNExtFeatures, itemValueValidList, lpItemValueList);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
BOOL CATIADLInterface::ADL2_OverdriveN_IsClockControlX2Supported()
{
	if (m_pADL2_OverdriveN_SystemClocksX2_Get &&
		m_pADL2_OverdriveN_SystemClocksX2_Set &&
		m_pADL2_OverdriveN_MemoryClocksX2_Get &&
		m_pADL2_OverdriveN_MemoryClocksX2_Set)
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_Overdrive8_Init_Setting_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8InitSetting *lpInitSetting)
{
	if (m_pADL2_Overdrive8_Init_Setting_Get)
		return m_pADL2_Overdrive8_Init_Setting_Get(context, iAdapterIndex, lpInitSetting);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_Overdrive8_Current_Setting_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8CurrentSetting *lpCurrentSetting)
{
	if (m_pADL2_Overdrive8_Current_Setting_Get)
		return m_pADL2_Overdrive8_Current_Setting_Get(context, iAdapterIndex, lpCurrentSetting);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_Overdrive8_Setting_Set(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLOD8SetSetting *lpSetSetting, ADLOD8CurrentSetting *lpCurrentSetting)
{
	if (m_pADL2_Overdrive8_Setting_Set)
		return m_pADL2_Overdrive8_Setting_Set(context, iAdapterIndex, lpSetSetting, lpCurrentSetting);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_New_QueryPMLogData_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, ADLPMLogDataOutput *lpDataOutput)
{
	if (m_pADL2_New_QueryPMLogData_Get)
		return m_pADL2_New_QueryPMLogData_Get(context, iAdapterIndex, lpDataOutput);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_Overdrive8_Init_SettingX2_Get(ADL_CONTEXT_HANDLE context, int iAdapterIndex, int *lpOverdrive8Capabilities, int *lpNumberOfFeatures, ADLOD8SingleInitSetting **lppInitSettingList)
{
	if (m_pADL2_Overdrive8_Init_SettingX2_Get)
		return m_pADL2_Overdrive8_Init_SettingX2_Get(context, iAdapterIndex, lpOverdrive8Capabilities, lpNumberOfFeatures, lppInitSettingList);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////
int CATIADLInterface::ADL2_Overdrive8_Current_SettingX2_Get (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int *lpNumberOfFeatures, int **lppCurrentSettingList)
{
	if (m_pADL2_Overdrive8_Current_SettingX2_Get)
		return m_pADL2_Overdrive8_Current_SettingX2_Get(context, iAdapterIndex, lpNumberOfFeatures, lppCurrentSettingList);

	return ADL_ERR_NOT_SUPPORTED;
}
//////////////////////////////////////////////////////////////////////

