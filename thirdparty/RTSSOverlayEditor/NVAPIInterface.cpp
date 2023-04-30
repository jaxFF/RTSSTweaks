// NVAPIInterface.cpp: implementation of the CNVAPIInterface class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "NVAPIInterface.h"
//////////////////////////////////////////////////////////////////////
CNVAPIInterface::CNVAPIInterface()
{
}
//////////////////////////////////////////////////////////////////////
CNVAPIInterface::~CNVAPIInterface()
{
}
//////////////////////////////////////////////////////////////////////
LPVOID CNVAPIInterface::QueryInterface(DWORD dwIID)
{
	HINSTANCE hNVAPI = GetModuleHandle("nvapi.dll");

	if (hNVAPI)
	{
		LPNVAPI_QUERYINTERFACE lpQueryInterface = (LPNVAPI_QUERYINTERFACE)GetProcAddress(hNVAPI, "nvapi_QueryInterface");

		if (lpQueryInterface)
			return lpQueryInterface(dwIID);
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
NvAPI_Status CNVAPIInterface::NvAPI_GPU_PowerMonitorGetInfo(NvPhysicalGpuHandle hGpu, NV_GPU_POWER_MONITOR_GET_INFO* lpInfo)
{
	LPNVAPI_GPU_POWERMONITORGETINFO lpNvAPI_GPU_PowerMonitorGetInfo = (LPNVAPI_GPU_POWERMONITORGETINFO)QueryInterface(NVAPIIID_GPU_PowerMonitorGetInfo);

	if (lpNvAPI_GPU_PowerMonitorGetInfo)
		return lpNvAPI_GPU_PowerMonitorGetInfo(hGpu, lpInfo);

	return NVAPI_NO_IMPLEMENTATION;
}
//////////////////////////////////////////////////////////////////////
NvAPI_Status CNVAPIInterface::NvAPI_GPU_PowerMonitorGetStatus(NvPhysicalGpuHandle hGpu, NV_GPU_POWER_MONITOR_GET_STATUS* lpStatus)
{
	LPNVAPI_GPU_POWERMONITORGETSTATUS lpNvAPI_GPU_PowerMonitorGetStatus = (LPNVAPI_GPU_POWERMONITORGETSTATUS)QueryInterface(NVAPIIID_GPU_PowerMonitorGetStatus);

	if (lpNvAPI_GPU_PowerMonitorGetStatus)
		return lpNvAPI_GPU_PowerMonitorGetStatus(hGpu, lpStatus);

	return NVAPI_NO_IMPLEMENTATION;
}
//////////////////////////////////////////////////////////////////////
NvAPI_Status CNVAPIInterface::NvAPI_GPU_ThermChannelGetInfo(NvPhysicalGpuHandle hGpu, NV_GPU_THERMAL_THERM_CHANNEL_INFO_PARAMS* lpInfo)
{
	LPNVAPI_GPU_THERMCHANNELGETINFO lpNvAPI_GPU_ThermChannelGetInfo = (LPNVAPI_GPU_THERMCHANNELGETINFO)QueryInterface(NVAPIIID_GPU_ThermChannelGetInfo);

	if (lpNvAPI_GPU_ThermChannelGetInfo)
		return lpNvAPI_GPU_ThermChannelGetInfo(hGpu, lpInfo);

	return NVAPI_NO_IMPLEMENTATION;
}
//////////////////////////////////////////////////////////////////////
NvAPI_Status CNVAPIInterface::NvAPI_GPU_ThermChannelGetStatus(NvPhysicalGpuHandle hGpu, NV_GPU_THERMAL_THERM_CHANNEL_STATUS_PARAMS* lpStatus)
{
	LPNVAPI_GPU_THERMCHANNELGETSTATUS lpNvAPI_GPU_ThermChannelGetStatus = (LPNVAPI_GPU_THERMCHANNELGETSTATUS)QueryInterface(NVAPIIID_GPU_ThermChannelGetStatus);

	if (lpNvAPI_GPU_ThermChannelGetStatus)
		return lpNvAPI_GPU_ThermChannelGetStatus(hGpu, lpStatus);

	return NVAPI_NO_IMPLEMENTATION;
}
//////////////////////////////////////////////////////////////////////
NvAPI_Status CNVAPIInterface::NvAPI_GPU_GetAllClocks(NvPhysicalGpuHandle hGpu, NV_GPU_CLOCK_INFO* lpInfo)
{
	LPNVAPI_GPU_GETALLCLOCKS lpNvAPI_GPU_GetAllClocks = (LPNVAPI_GPU_GETALLCLOCKS)QueryInterface(NVAPIIID_GPU_GetAllClocks);
	if (lpNvAPI_GPU_GetAllClocks)
		return lpNvAPI_GPU_GetAllClocks(hGpu, lpInfo);

	return NVAPI_NO_IMPLEMENTATION;
}
//////////////////////////////////////////////////////////////////////

