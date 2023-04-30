// HALDataSourcesList.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HAL.h"
#include "HALDataSourcesList.h"
//////////////////////////////////////////////////////////////////////
CHALDataSourcesList::CHALDataSourcesList()
{
	m_lpHAL = NULL;
}
//////////////////////////////////////////////////////////////////////
CHALDataSourcesList::~CHALDataSourcesList()
{
	Destroy();
}
//////////////////////////////////////////////////////////////////////
void CHALDataSourcesList::Init(CHAL* lpHAL)
{
	Destroy();

	m_lpHAL = lpHAL;

	//CPU

	DWORD dwCpuCount = lpHAL->GetCpuCount();

		//usage

	for (DWORD dwCpu=0; dwCpu<dwCpuCount; dwCpu++)
	{
		CString strSource;
		strSource.Format("CPU%d usage", dwCpu + 1);

		AddSource(strSource, "%", "CPU", HALDATASOURCE_ID_CPU_USAGE, dwCpu);
	}

	AddSource("CPU usage"					, "%", "CPU", HALDATASOURCE_ID_CPU_USAGE_TOTAL				);
	AddSource("CPU usage max"				, "%", "CPU", HALDATASOURCE_ID_CPU_USAGE_MAX				);

		//temperature

	BOOL bCpuTemperatureReportingSupported = FALSE;

	for (DWORD dwCpu=0; dwCpu<dwCpuCount; dwCpu++)
	{
		CString strSource;
		strSource.Format("CPU%d temperature", dwCpu + 1);

		if (lpHAL->IsCpuTemperatureReportingSupported(dwCpu))
		{
			AddSource(strSource, "°C", "CPU", HALDATASOURCE_ID_CPU_TEMPERATURE, dwCpu);

			bCpuTemperatureReportingSupported = TRUE;
		}
	}

	if (lpHAL->IsCpuTemperatureReportingSupported(0xFFFFFFFF))
		AddSource("CPU temperature"			, "°C", "CPU", HALDATASOURCE_ID_CPU_TEMPERATURE, 0xFFFFFFFF);
	else
	{
		if (bCpuTemperatureReportingSupported && (dwCpuCount > 1))
			AddSource("CPU temperature"		, "°C", "CPU", HALDATASOURCE_ID_CPU_TEMPERATURE_MAX);
	}

		//CCD temeprature

	for (DWORD dwCcd=0; dwCcd<m_lpHAL->GetCcdCount(); dwCcd++)
	{
		CString strSource;
		strSource.Format("CCD%d temperature", dwCcd + 1);

		if (lpHAL->IsCcdTemperatureReportingSupported(dwCcd))
			AddSource(strSource, "°C", "CPU", HALDATASOURCE_ID_CCD_TEMPERATURE, dwCcd);
	}

		//power

	for (DWORD dwCpu=0; dwCpu<dwCpuCount; dwCpu++)
	{
		CString strSource;
		strSource.Format("CPU%d power", dwCpu + 1);

		if (lpHAL->IsCpuPowerReportingSupported(dwCpu))
			AddSource(strSource, "W", "CPU", HALDATASOURCE_ID_CPU_POWER, dwCpu);
	}

	if (lpHAL->IsCpuPowerReportingSupported(0xFFFFFFFF))
		AddSource("CPU power"			, "W", "CPU", HALDATASOURCE_ID_CPU_POWER, 0xFFFFFFFF);

		//clock

	BOOL bCpuClockReportingSupported = FALSE;

	for (DWORD dwCpu=0; dwCpu<dwCpuCount; dwCpu++)
	{
		CString strSource;
		strSource.Format("CPU%d clock", dwCpu + 1);

		if (lpHAL->IsCpuClockReportingSupported(dwCpu))
		{
			AddSource(strSource, "MHz", "CPU", HALDATASOURCE_ID_CPU_CLOCK, dwCpu);

			bCpuClockReportingSupported = TRUE;
		}
	}

	if (bCpuClockReportingSupported && (dwCpuCount > 1))
		AddSource("CPU clock"				, "MHz", "CPU", HALDATASOURCE_ID_CPU_CLOCK, 0xFFFFFFFF);

		//effective clock

	for (DWORD dwCpu=0; dwCpu<dwCpuCount; dwCpu++)
	{
		CString strSource;
		strSource.Format("CPU%d effective clock", dwCpu + 1);

		if (lpHAL->IsEffectiveCpuClockReportingSupported(dwCpu, FALSE))
			AddSource(strSource, "MHz", "CPU", HALDATASOURCE_ID_CPU_EFF_CLOCK, dwCpu);
	}

		//bus clock

	if (lpHAL->IsCpuBusClockReportingSupported())
		AddSource("Bus clock", "MHz", "CPU", HALDATASOURCE_ID_CPU_BUS_CLOCK);

	//RAM

	AddSource("RAM usage"					, "MB"	, "RAM", HALDATASOURCE_ID_RAM_USAGE					);
	AddSource("RAM usage percent"			, "%"	, "RAM", HALDATASOURCE_ID_RAM_USAGE_PERCENT			);
	AddSource("Commit charge"				, "MB"	, "RAM", HALDATASOURCE_ID_COMMIT_CHARGE				);
	AddSource("Total RAM"					, "MB"	, "RAM", HALDATASOURCE_ID_RAM_TOTAL					);
	AddSource("RAM usage \\ process"		, "MB"	, "RAM", HALDATASOURCE_ID_RAM_USAGE_PROCESS			);
	AddSource("RAM usage percent \\ process", "%"	, "RAM", HALDATASOURCE_ID_RAM_USAGE_PERCENT_PROCESS );

	//NET

	AddSource("Ping"				, "ms"	, "NET", HALDATASOURCE_ID_PING				);

	//SYS

	AddSource("Timer"				, "ms"	, "SYS", HALDATASOURCE_ID_TIMER				);
	AddSource("Stub"				, ""	, "SYS", HALDATASOURCE_ID_STUB				);

	//HDD

	for (DWORD dwHdd=0; dwHdd<MAX_DRIVE; dwHdd++)
	{
		CString strSource;
		strSource.Format("HDD%d temperature", dwHdd + 1);

		if (lpHAL->IsHddTemperatureReportingSupported(dwHdd, 0))
			AddSource(strSource, "°C", "HDD", HALDATASOURCE_ID_HDD_TEMPERATURE, dwHdd, 0);

		strSource.Format("HDD%d temperature 2", dwHdd + 1);

		if (lpHAL->IsHddTemperatureReportingSupported(dwHdd, 1))
			AddSource(strSource, "°C", "HDD", HALDATASOURCE_ID_HDD_TEMPERATURE, dwHdd, 1);
	}

	//GPU

	DWORD dwGpuCount = lpHAL->GetGpuCount();

	for (DWORD dwGpu=0; dwGpu<dwGpuCount; dwGpu++)
	{
		CHALImplementation* lpGpu = lpHAL->GetGpu(dwGpu);

		if (lpGpu->IsCoreClockReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d clock", dwGpu + 1);

			AddSource(strSource, "MHz", "GPU", HALDATASOURCE_ID_GPU_CLOCK_CORE, dwGpu);
		}

		if (lpGpu->IsMemoryClockReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d memory clock", dwGpu + 1);

			AddSource(strSource, "MHz", "GPU", HALDATASOURCE_ID_GPU_CLOCK_MEMORY, dwGpu);
		}

		if (lpGpu->IsEffectiveCoreClockReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d effective clock", dwGpu + 1);

			AddSource(strSource, "MHz", "GPU", HALDATASOURCE_ID_GPU_EFFECTIVE_CLOCK_CORE, dwGpu);
		}

		if (lpGpu->IsEffectiveMemoryClockReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d effective memory clock", dwGpu + 1);

			AddSource(strSource, "MHz", "GPU", HALDATASOURCE_ID_GPU_EFFECTIVE_CLOCK_MEMORY, dwGpu);
		}

		if (lpGpu->IsCoreVoltageReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d voltage", dwGpu + 1);

			AddSource(strSource, "mV", "GPU", HALDATASOURCE_ID_GPU_VOLTAGE_CORE, dwGpu);
		}

		if (lpGpu->IsMemoryVoltageReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d memory voltage", dwGpu + 1);

			AddSource(strSource, "mV", "GPU", HALDATASOURCE_ID_GPU_VOLTAGE_MEMORY, dwGpu);
		}

		DWORD dwSensorCount = lpGpu->GetCoreTemperatureSensorCount();

		for (DWORD dwSensor=0; dwSensor<dwSensorCount; dwSensor++)
		{
			CString strSource;

			if (dwSensor)
				strSource.Format("GPU%d temperature %d", dwGpu + 1, dwSensor + 1);
			else
				strSource.Format("GPU%d temperature", dwGpu + 1);

			AddSource(strSource, "°C", "GPU", HALDATASOURCE_ID_GPU_TEMPERATURE_CORE, dwGpu, dwSensor);
		}

		dwSensorCount = lpGpu->GetMemoryTemperatureSensorCount();

		for (DWORD dwSensor=0; dwSensor<dwSensorCount; dwSensor++)
		{
			CString strSource;

			if (dwSensor)
				strSource.Format("GPU%d memory temperature %d", dwGpu + 1, dwSensor + 1);
			else
				strSource.Format("GPU%d memory temperature", dwGpu + 1);

			AddSource(strSource, "°C", "GPU", HALDATASOURCE_ID_GPU_TEMPERATURE_MEMORY, dwGpu, dwSensor);
		}

		dwSensorCount = lpGpu->GetVRMTemperatureSensorCount();

		for (DWORD dwSensor=0; dwSensor<dwSensorCount; dwSensor++)
		{
			CString strSource;

			if (dwSensor)
				strSource.Format("GPU%d VRM temperature %d", dwGpu + 1, dwSensor + 1);
			else
				strSource.Format("GPU%d VRM temperature", dwGpu + 1);

			AddSource(strSource, "°C", "GPU", HALDATASOURCE_ID_GPU_TEMPERATURE_VRM, dwGpu, dwSensor);
		}

		DWORD dwFanCount = lpGpu->GetFanCount();

		for (DWORD dwFan=0; dwFan<dwFanCount; dwFan++)
		{
			if (lpGpu->IsFanSpeedReportingSupported(dwFan))
			{
				CString strSource;

				if (dwFan)
					strSource.Format("GPU%d fan speed %d", dwGpu + 1, dwFan + 1);
				else
					strSource.Format("GPU%d fan speed", dwGpu + 1);

				AddSource(strSource, "%", "GPU", HALDATASOURCE_ID_GPU_FAN_SPEED, dwGpu, dwFan);
			}

			if (lpGpu->IsFanSpeedRPMReportingSupported(dwFan))
			{
				CString strSource;

				if (dwFan)
					strSource.Format("GPU%d fan tachometer %d", dwGpu + 1, dwFan + 1);
				else
					strSource.Format("GPU%d fan tachometer", dwGpu + 1);

				AddSource(strSource, "RPM", "GPU", HALDATASOURCE_ID_GPU_FAN_SPEED_RPM, dwGpu, dwFan);
			}
		}

		if (lpGpu->IsGraphicsEngineUsageReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d usage", dwGpu + 1);

			AddSource(strSource, "%", "GPU", HALDATASOURCE_ID_GPU_GRAPHICS_ENGINE_USAGE, dwGpu);
		}

		if (lpGpu->IsMemoryControllerUsageReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d memory controller usage", dwGpu + 1);

			AddSource(strSource, "%", "GPU", HALDATASOURCE_ID_GPU_MEMORY_CONTROLLER_USAGE, dwGpu);
		}

		if (lpGpu->IsVideoEngineUsageReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d video engine usage", dwGpu + 1);

			AddSource(strSource, "%", "GPU", HALDATASOURCE_ID_GPU_VIDEO_ENGINE_USAGE, dwGpu);
		}

		if (lpGpu->IsBusUsageReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d bus usage", dwGpu + 1);

			AddSource(strSource, "%", "GPU", HALDATASOURCE_ID_GPU_BUS_USAGE, dwGpu);
		}

		if (lpGpu->IsAbsPowerReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d power", dwGpu + 1);

			AddSource(strSource, "W", "GPU", HALDATASOURCE_ID_GPU_ABS_POWER, dwGpu);
		}

		if (lpGpu->IsRelPowerReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d power percent", dwGpu + 1);

			AddSource(strSource, "%", "GPU", HALDATASOURCE_ID_GPU_REL_POWER, dwGpu);
		}

		if (lpGpu->IsVideomemoryUsageReportingSupported())
		{
			CString strSource;
			strSource.Format("GPU%d memory usage", dwGpu + 1);

			AddSource(strSource, "MB", "GPU", HALDATASOURCE_ID_GPU_VIDEOMEMORY_USAGE, dwGpu);

			strSource.Format("GPU%d memory usage percent", dwGpu + 1);

			AddSource(strSource, "%", "GPU", HALDATASOURCE_ID_GPU_VIDEOMEMORY_USAGE_PERCENT, dwGpu);

			strSource.Format("GPU%d total memory", dwGpu + 1);

			AddSource(strSource, "MB", "GPU", HALDATASOURCE_ID_GPU_VIDEOMEMORY_TOTAL, dwGpu);

			strSource.Format("GPU%d memory usage \\ process", dwGpu + 1);

			AddSource(strSource, "MB", "GPU", HALDATASOURCE_ID_GPU_VIDEOMEMORY_USAGE_PROCESS, dwGpu);

			strSource.Format("GPU%d memory usage percent \\ process", dwGpu + 1);

			AddSource(strSource, "%", "GPU", HALDATASOURCE_ID_GPU_VIDEOMEMORY_USAGE_PERCENT_PROCESS, dwGpu);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CHALDataSourcesList::Destroy()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		delete GetNext(pos);

	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
void CHALDataSourcesList::AddSource(LPCSTR lpName, LPCSTR lpUnits, LPCSTR lpGroup, DWORD dwID, DWORD dwParam0, DWORD dwParam1)
{
	CHALDataSource* lpSource = new CHALDataSource(lpName, lpUnits, lpGroup, dwID, dwParam0, dwParam1);

	lpSource->SetHAL(m_lpHAL);

	AddTail(lpSource);
}
//////////////////////////////////////////////////////////////////////
CHALDataSource* CHALDataSourcesList::FindSource(LPCSTR lpName)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CHALDataSource* lpSource = GetNext(pos);

		if (!_stricmp(lpSource->GetName(), lpName))
			return lpSource;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
