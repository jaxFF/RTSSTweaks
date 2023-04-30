// NVAPIWrapper.h: interface for the CNVAPIWrapper class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#define NVAPI_NDA_DEVELOPER_SDK_AVAILABLE
//////////////////////////////////////////////////////////////////////
//
// WARNING!
//
// Some NVAPI functions used in this plugin are only available in NDA
// developer version of NVAPI SDK
//
// So you'll be able to compile the plugin with full NVAPI support 
// only if you get NDA developer version of NVAPI SDK
//
// Otherwise you may just comment NVAPI_NDA_DEVELOPER_SDK_AVAILABLE 
// define a few lines above to build the plugin with limited NVAPI
// support
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "nvapi.h"
#include "NVAPIInterface.h"
//////////////////////////////////////////////////////////////////////
#define NVAPIWRAPPER_MAX_GPU								256
#define NVAPIWRAPPER_INVALID_GPU							0xFFFFFFFF
#define NVAPIWRAPPER_INVALID_UTILIZATION					0xFFFFFFFF
#define NVAPIWRAPPER_INVALID_MEMORY_USAGE					0xFFFFFFFF
#define NVAPIWRAPPER_INVALID_TEMPERATURE					INT_MAX
//////////////////////////////////////////////////////////////////////
#define NVAPIWRAPPER_CAPS_GpuGetAllClockFrequencies			0x00000002
#define NVAPIWRAPPER_CAPS_GpuGetVoltageDomainsStatus		0x00000004
#define NVAPIWRAPPER_CAPS_GpuClientFanCoolersGetStatus		0x00000040
#define NVAPIWRAPPER_CAPS_GpuClientVoltRailsGetStatus		0x00000080
#define NVAPIWRAPPER_CAPS_GpuPowerMonitorGetStatus			0x00000100
#define NVAPIWRAPPER_CAPS_GpuThermChannelGetStatus			0x00000200
//////////////////////////////////////////////////////////////////////
#define NVAPI_INIT_STRUCT(s,v) { ZeroMemory(&s,sizeof(s)); s.version = sizeof(s) | (v<<16); }
//////////////////////////////////////////////////////////////////////
class CNVAPIWrapper
{
public:
	CNVAPIWrapper();
	virtual ~CNVAPIWrapper();

	void	Init();
	void	Uninit();
	LPVOID	QueryInterface(DWORD dwIID);

	DWORD	GetGpuCount();
	
	DWORD	GetVersion();
	BOOL	GetFullName(DWORD dwGpu, CString& strName);
	BOOL	GetMemoryInfo(DWORD dwGpu, DWORD* lpDedicated, DWORD* lpDedicatedAvailable, DWORD* lpSystem, DWORD* lpShared, DWORD* lpDedicatedAvailableCur);
	BOOL	GetLocation(DWORD dwGpu, DWORD* lpBus, DWORD* lpDev, DWORD* lpFn);

	BOOL	GetCurrentUtilizations(DWORD dwGpu, DWORD* lpGraphicsEngine, DWORD* lpFramebuffer, DWORD* lpVideoEngine, DWORD* lpBus);
	BOOL	GetCurrentTemperatures(DWORD dwGpu, LONG* lpCore, LONG* lpMemory, LONG* lpBoard);
	BOOL	GetCurrentTemperaturesEx(DWORD dwGpu, LONG* lpCore, LONG* lpHotspot, LONG* lpBoard, LONG* lpMemory, LONG* lpPower);
	BOOL	GetCurrentClocks(DWORD dwGpu, DWORD* lpCore, DWORD* lpMem, DWORD* lpShader);
	BOOL	GetEffectiveClocks(DWORD dwGpu, DWORD* lpCore, DWORD* lpMem, DWORD* lpShader);
	BOOL	GetCurrentVoltage(DWORD dwGpu, DWORD* lpVoltage);
	BOOL	GetCurrentRelPower(DWORD dwGpu, DWORD* lpPower);
	BOOL	GetCurrentAbsPower(DWORD dwGpu, DWORD* lpPower);

	DWORD	GetFanCount(DWORD dwGpu);
	BOOL	GetCurrentFanSpeeds(DWORD dwGpu, DWORD* lpSpeed, DWORD* lpSpeedRPM, DWORD* lpSpeed2, DWORD* lpSpeedRPM2);

protected:
	BOOL GetTemperature(NV_GPU_THERMAL_SETTINGS_V2* lpThermalSettings, DWORD dwTarget, LONG* lpTemperature);
	BOOL GetTemperature(NV_GPU_THERMAL_THERM_CHANNEL_INFO_PARAMS_V2* lpInfo, NV_GPU_THERMAL_THERM_CHANNEL_STATUS_PARAMS_V2* lpStatus, DWORD dwType, LONG* lpTemperature);

	BOOL ClientVoltRails_GetStatus(NvPhysicalGpuHandle hGpu, DWORD* lpStatus);

	BOOL ClientFanCoolers_IsSupported(DWORD dwGpu);
	BOOL ClientFanCoolers_GetStatus(NvPhysicalGpuHandle hGpu, DWORD* lpCount, DWORD* lpTach0, DWORD* lpMin0, DWORD* lpMax0, DWORD* lpTarget0, DWORD* lpTach1, DWORD* lpMin1, DWORD* lpMax1, DWORD* lpTarget1);

	DWORD					m_dwVersion;
	NvAPI_ShortString		m_szBranch;

	DWORD					m_dwGpuCount;

	NvPhysicalGpuHandle		m_hGpu[NVAPIWRAPPER_MAX_GPU];
	DWORD					m_dwBus[NVAPIWRAPPER_MAX_GPU];
	DWORD					m_dwDev[NVAPIWRAPPER_MAX_GPU];
	DWORD					m_dwCaps[NVAPIWRAPPER_MAX_GPU];

	CNVAPIInterface			m_interface;
		//we'll use this object to directly query some private NVAPI interfaces and access some 
		//undocumented NVAPI functionality (such as absolute GPU power monitoring in Watts)
};
//////////////////////////////////////////////////////////////////////
