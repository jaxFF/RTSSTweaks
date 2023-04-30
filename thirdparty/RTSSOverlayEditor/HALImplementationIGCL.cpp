// HALImplementationIGCL.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HALImplementationIGCL.h"
//////////////////////////////////////////////////////////////////////
CHALImplementationIGCL::CHALImplementationIGCL()
{
	m_lpIGCLWrapper			= NULL;
	m_dwGpu					= 0;
}
//////////////////////////////////////////////////////////////////////
CHALImplementationIGCL::~CHALImplementationIGCL()
{
}
//////////////////////////////////////////////////////////////////////
void CHALImplementationIGCL::SetIGCLWrapper(CIGCLWrapper* lpIGCLWrapper)
{
	m_lpIGCLWrapper = lpIGCLWrapper;
}
//////////////////////////////////////////////////////////////////////
void CHALImplementationIGCL::SetGpu(DWORD dwGpu)
{
	m_dwGpu = dwGpu;
}
//////////////////////////////////////////////////////////////////////
void CHALImplementationIGCL::SetTimestamp(DWORD dwTimestamp)
{
	CHALImplementation::SetTimestamp(dwTimestamp);

	m_lpIGCLWrapper->SetTimestamp(dwTimestamp);
}
//////////////////////////////////////////////////////////////////////
CString	CHALImplementationIGCL::GetDriverStr()
{
	DWORD dwVersion = m_lpIGCLWrapper->GetVersion();

	CString strVersion;
	strVersion.Format("%d", dwVersion);

	return strVersion;
}
//////////////////////////////////////////////////////////////////////
CString	CHALImplementationIGCL::GetName()
{
	CString strName;

	if (m_lpIGCLWrapper->GetName(m_dwGpu, strName))
		return strName;

	return "";
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::GetLocation(DWORD* lpBus, DWORD* lpDev, DWORD* lpFn)
{
	return m_lpIGCLWrapper->GetLocation(m_dwGpu, lpBus, lpDev, lpFn);
}
//////////////////////////////////////////////////////////////////////
double CHALImplementationIGCL::GetTelemetryValue(ctl_oc_telemetry_item_t item)
{
	switch (item.type)
	{
	case CTL_DATA_TYPE_INT8:
		return item.value.data8;
	case CTL_DATA_TYPE_UINT8:
		return item.value.datau8;
	case CTL_DATA_TYPE_INT16:
		return item.value.data16;
	case CTL_DATA_TYPE_UINT16:
		return item.value.datau16;
	case CTL_DATA_TYPE_INT32:
		return item.value.data32;
	case CTL_DATA_TYPE_UINT32:
		return item.value.datau32;
	case CTL_DATA_TYPE_INT64:
		return (double)item.value.data64;
	case CTL_DATA_TYPE_UINT64:
		return (double)item.value.datau64;
	case CTL_DATA_TYPE_FLOAT:
		return item.value.datafloat;
	case CTL_DATA_TYPE_DOUBLE:
		return item.value.datadouble;
	}

	return 0.0;
}
//////////////////////////////////////////////////////////////////////
double CHALImplementationIGCL::GetTelemetryDelta(ctl_oc_telemetry_item_t item1, ctl_oc_telemetry_item_t item0, ctl_oc_telemetry_item_t time1, ctl_oc_telemetry_item_t time0)
{
	double dv = GetTelemetryValue(item1) - GetTelemetryValue(item0);
	double dt = GetTelemetryValue(time1) - GetTelemetryValue(time0);

	if (dt != 0.0)
		return dv / dt;

	return 0.0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsCoreClockReportingSupported()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->gpuCurrentClockFrequency.bSupported)
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsMemoryClockReportingSupported()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->vramCurrentClockFrequency.bSupported)
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentCoreClock()
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (DWORD)(GetTelemetryValue(pTelemetry->gpuCurrentClockFrequency) * 1000);

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentMemoryClock()
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (DWORD)(GetTelemetryValue(pTelemetry->vramCurrentClockFrequency) * 1000);

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsCoreVoltageReportingSupported()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->gpuVoltage.bSupported)
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsMemoryVoltageReportingSupported()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->vramVoltage.bSupported)
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentCoreVoltage()
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (DWORD)(GetTelemetryValue(pTelemetry->gpuVoltage) * 1000);

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentMemoryVoltage()
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (DWORD)(GetTelemetryValue(pTelemetry->vramVoltage) * 1000);

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCoreTemperatureSensorCount()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->gpuCurrentTemperature.bSupported)
		return 1;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetMemoryTemperatureSensorCount()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->vramCurrentTemperature.bSupported)
		return 1;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetVRMTemperatureSensorCount()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationIGCL::GetCurrentCoreTemperature(int index)
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (LONG)GetTelemetryValue(pTelemetry->gpuCurrentTemperature);

	return 0;
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationIGCL::GetCurrentMemoryTemperature(int index)
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (LONG)GetTelemetryValue(pTelemetry->vramCurrentTemperature);

	return 0;
}
//////////////////////////////////////////////////////////////////////
LONG CHALImplementationIGCL::GetCurrentVRMTemperature(int index)
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetFanCount()
{
	return CTL_FAN_COUNT;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsFanSpeedReportingSupported(int index)
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsFanSpeedRPMReportingSupported(int index)
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->fanSpeed[index].bSupported)
		return TRUE;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentFanSpeed(int index)
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentFanSpeedRPM(int index)
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (DWORD)GetTelemetryValue(pTelemetry->fanSpeed[index]);

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsGraphicsEngineUsageReportingSupported()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->globalActivityCounter.bSupported)
		return TRUE;

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsMemoryControllerUsageReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsVideoEngineUsageReportingSupported()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->mediaActivityCounter.bSupported)
		return TRUE;

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsBusUsageReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentGraphicsEngineUsage()
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry		= m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);
	ctl_power_telemetry_t* pTelemetryPrev	= m_lpIGCLWrapper->GetPreviousPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (DWORD)(GetTelemetryDelta(pTelemetry->globalActivityCounter, pTelemetryPrev->globalActivityCounter, pTelemetry->timeStamp, pTelemetryPrev->timeStamp) * 100);

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentMemoryControllerUsage()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentVideoEngineUsage()
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry		= m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);
	ctl_power_telemetry_t* pTelemetryPrev	= m_lpIGCLWrapper->GetPreviousPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (DWORD)(GetTelemetryDelta(pTelemetry->mediaActivityCounter, pTelemetryPrev->mediaActivityCounter, pTelemetry->timeStamp, pTelemetryPrev->timeStamp) * 100);

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentBusUsage()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsVideomemoryUsageReportingSupported()
{
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetTotalVideomemory()
{
	if (m_dwTotalVideomemory)
		return m_dwTotalVideomemory;

	if (m_lpIGCLWrapper->GetTotalVideomemory(m_dwGpu, m_dwTotalVideomemory))
		return m_dwTotalVideomemory;

	//total videomemory detection is currently not supported in x86 IGCL, so if it fails we try to use D3DKMT fallback

	DWORD dwTotalVideomemory = GetD3DKMTTotalVideomemory(FALSE, TRUE);

	if (dwTotalVideomemory != VIDEOMEMORY_USAGE_INVALID)
	{
		m_dwTotalVideomemory = dwTotalVideomemory;

		return m_dwTotalVideomemory;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentVideomemoryUsage()
{
	return GetD3DKMTVideomemoryUsage(FALSE, TRUE, 0);
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsRelPowerReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsAbsPowerReportingSupported()
{
	ctl_power_telemetry_t* pTelemetry = m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);

	if (pTelemetry && pTelemetry->gpuEnergyCounter.bSupported)
		return TRUE;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentRelPower()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetCurrentAbsPower()
{
	m_lpIGCLWrapper->RetreivePowerTelemetry(m_dwGpu);

	ctl_power_telemetry_t* pTelemetry		= m_lpIGCLWrapper->GetCurrentPowerTelemetry(m_dwGpu);
	ctl_power_telemetry_t* pTelemetryPrev	= m_lpIGCLWrapper->GetPreviousPowerTelemetry(m_dwGpu);

	if (pTelemetry)
		return (DWORD)(GetTelemetryDelta(pTelemetry->gpuEnergyCounter, pTelemetryPrev->gpuEnergyCounter, pTelemetry->timeStamp, pTelemetryPrev->timeStamp) * 1000);

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsEffectiveCoreClockReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementationIGCL::IsEffectiveMemoryClockReportingSupported()
{
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetEffectiveCoreClock()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementationIGCL::GetEffectiveMemoryClock()
{
	return 0;
}
//////////////////////////////////////////////////////////////////////
