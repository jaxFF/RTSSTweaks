// HALDataSource.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HAL.h"
#include "HALDataSource.h"
#include "RTSSSharedMemoryInterface.h"
#include "Log.h"

#include <float.h>
#include <psapi.h>
//////////////////////////////////////////////////////////////////////
CHALDataSource::CHALDataSource(LPCSTR lpName, LPCSTR lpUnits, LPCSTR lpGroup, DWORD dwID, DWORD dwParam0, DWORD dwParam1)
{
	m_lpHAL				= NULL;

	m_bDirty			= FALSE;

	m_fltData			= FLT_MAX;
	m_fltPollingTime	= 0;

	m_strName			= lpName;
	m_strUnits			= lpUnits;
	m_strGroup			= lpGroup;

	m_dwID				= dwID;
	m_dwParam0			= dwParam0;
	m_dwParam1			= dwParam1;
}
//////////////////////////////////////////////////////////////////////
CHALDataSource::~CHALDataSource()
{
}
//////////////////////////////////////////////////////////////////////
void CHALDataSource::SetHAL(CHAL* lpHAL)
{
	m_lpHAL = lpHAL;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALDataSource::GetID()
{
	return m_dwID;
}
//////////////////////////////////////////////////////////////////////
void CHALDataSource::Poll()
{
	TIMING_INIT
	TIMING_BEGIN

	switch (m_dwID)
	{
	case HALDATASOURCE_ID_CPU_USAGE:
		m_fltData = m_lpHAL->GetCpuUsage(m_dwParam0);
		break;
	case HALDATASOURCE_ID_CPU_USAGE_TOTAL:
		m_fltData = m_lpHAL->GetTotalCpuUsage();
		break;
	case HALDATASOURCE_ID_CPU_USAGE_MAX:
		m_fltData = m_lpHAL->GetMaxCpuUsage();
		break;
	case HALDATASOURCE_ID_CPU_TEMPERATURE:
		m_fltData = m_lpHAL->GetCpuTemperature(m_dwParam0);
		break;
	case HALDATASOURCE_ID_CCD_TEMPERATURE:
		m_fltData = m_lpHAL->GetCcdTemperature(m_dwParam0);
		break;
	case HALDATASOURCE_ID_CPU_TEMPERATURE_MAX:
		m_fltData = m_lpHAL->GetMaxCpuTemperature();
		break;
	case HALDATASOURCE_ID_CPU_POWER:
		m_fltData = m_lpHAL->GetCpuPower(m_dwParam0);
		break;
	case HALDATASOURCE_ID_CPU_BUS_CLOCK:
		m_fltData = m_lpHAL->GetCpuBusClock();
		break;
	case HALDATASOURCE_ID_CPU_EFF_CLOCK:
		m_fltData = m_lpHAL->GetEffectiveCpuClock(m_dwParam0, FALSE);
		break;
	case HALDATASOURCE_ID_CPU_CLOCK:
		if (m_dwParam0 == 0xFFFFFFFF)
			m_fltData = m_lpHAL->GetMaxCpuClockAsync();
		else
			m_fltData = m_lpHAL->GetCpuClockAsync(m_dwParam0);
		break;

	case HALDATASOURCE_ID_RAM_USAGE:
		m_fltData = (float)m_lpHAL->GetPhysicalMemoryUsage();
		break;
	case HALDATASOURCE_ID_RAM_USAGE_PERCENT:
		m_fltData = m_lpHAL->GetPhysicalMemoryUsagePercent();
		break;
	case HALDATASOURCE_ID_COMMIT_CHARGE:
		m_fltData = (float)m_lpHAL->GetCommitCharge();
		break;
	case HALDATASOURCE_ID_RAM_TOTAL:
		m_fltData = m_lpHAL->GetTotalRAM() / 1024.0f;
		break;
	case HALDATASOURCE_ID_RAM_USAGE_PROCESS:
		m_fltData = m_lpHAL->GetProcessMemoryUsage() / 1024.0f;
		break;
	case HALDATASOURCE_ID_RAM_USAGE_PERCENT_PROCESS:
		m_fltData = m_lpHAL->GetProcessMemoryUsagePercent();
		break;

	case HALDATASOURCE_ID_GPU_CLOCK_CORE:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetCurrentCoreClock() / 1000.0f;
		break;
	case HALDATASOURCE_ID_GPU_CLOCK_MEMORY:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetCurrentMemoryClock() / 1000.0f;
		break;
	case HALDATASOURCE_ID_GPU_EFFECTIVE_CLOCK_CORE:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetEffectiveCoreClock() / 1000.0f;
		break;
	case HALDATASOURCE_ID_GPU_EFFECTIVE_CLOCK_MEMORY:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetEffectiveMemoryClock() / 1000.0f;
		break;

	case HALDATASOURCE_ID_GPU_VOLTAGE_CORE:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentCoreVoltage();
		break;
	case HALDATASOURCE_ID_GPU_VOLTAGE_MEMORY:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentMemoryVoltage();
		break;

	case HALDATASOURCE_ID_GPU_TEMPERATURE_CORE:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentCoreTemperature(m_dwParam1);
		break;
	case HALDATASOURCE_ID_GPU_TEMPERATURE_MEMORY:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentMemoryTemperature(m_dwParam1);
		break;
	case HALDATASOURCE_ID_GPU_TEMPERATURE_VRM:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentVRMTemperature(m_dwParam1);
		break;

	case HALDATASOURCE_ID_GPU_FAN_SPEED:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentFanSpeed(m_dwParam1);
		break;
	case HALDATASOURCE_ID_GPU_FAN_SPEED_RPM:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentFanSpeedRPM(m_dwParam1);
		break;

	case HALDATASOURCE_ID_GPU_GRAPHICS_ENGINE_USAGE:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentGraphicsEngineUsage();
		break;
	case HALDATASOURCE_ID_GPU_MEMORY_CONTROLLER_USAGE:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentMemoryControllerUsage();
		break;
	case HALDATASOURCE_ID_GPU_VIDEO_ENGINE_USAGE:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentVideoEngineUsage();
		break;
	case HALDATASOURCE_ID_GPU_BUS_USAGE:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentBusUsage();
		break;

	case HALDATASOURCE_ID_GPU_REL_POWER:
		m_fltData = (float)m_lpHAL->GetGpu(m_dwParam0)->GetCurrentRelPower();
		break;
	case HALDATASOURCE_ID_GPU_ABS_POWER:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetCurrentAbsPower() / 1000.0f;
		break;

	case HALDATASOURCE_ID_GPU_VIDEOMEMORY_USAGE:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetCurrentVideomemoryUsage() / 1024.0f;
		break;
	case HALDATASOURCE_ID_GPU_VIDEOMEMORY_USAGE_PERCENT:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetCurrentVideomemoryUsagePercent();
		break;
	case HALDATASOURCE_ID_GPU_VIDEOMEMORY_TOTAL:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetTotalVideomemory() / 1024.0f;
		break;
	case HALDATASOURCE_ID_GPU_VIDEOMEMORY_USAGE_PROCESS:
		{
			DWORD dwUsage = m_lpHAL->GetGpu(m_dwParam0)->GetProcessVideomemoryUsage();

			if (dwUsage == VIDEOMEMORY_USAGE_INVALID)
				m_fltData = FLT_MAX;
			else
			m_fltData = dwUsage / 1024.0f;
		}
		break;
	case HALDATASOURCE_ID_GPU_VIDEOMEMORY_USAGE_PERCENT_PROCESS:
		m_fltData = m_lpHAL->GetGpu(m_dwParam0)->GetProcessVideomemoryUsagePercent();
		break;

	case HALDATASOURCE_ID_PING:
		m_fltData = (float)m_lpHAL->GetPing();
		break;

	case HALDATASOURCE_ID_TIMER:
		m_fltData = (float)m_lpHAL->GetLifetime();
		break;
	case HALDATASOURCE_ID_STUB:
		//do nothing for stub source, it is not connected to any physical sensor and intended to represent virtual sensors implemented via
		//correction formulas (e.g. "CPU power" + "GPU1 power" + Delta for virtual sensor approximating total system power consumption)
		break;
	case HALDATASOURCE_ID_HDD_TEMPERATURE:
		m_fltData = m_lpHAL->GetHddTemperature(m_dwParam0, m_dwParam1);
		break;
	}

	TIMING_END

	m_fltPollingTime = timing;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALDataSource::IsDirty()
{
	return m_bDirty;
}
//////////////////////////////////////////////////////////////////////
void CHALDataSource::SetDirty(BOOL bDirty)
{
	m_bDirty = bDirty;
}
//////////////////////////////////////////////////////////////////////
float CHALDataSource::GetData()
{
	return m_fltData;
}
//////////////////////////////////////////////////////////////////////
float CHALDataSource::GetPollingTime()
{
	return m_fltPollingTime;
}
//////////////////////////////////////////////////////////////////////
CString	CHALDataSource::GetName()
{
	return m_strName;
}
//////////////////////////////////////////////////////////////////////
CString CHALDataSource::GetUnits()
{
	return m_strUnits;
}
//////////////////////////////////////////////////////////////////////
CString CHALDataSource::GetGroup()
{
	return m_strGroup;
}
//////////////////////////////////////////////////////////////////////
