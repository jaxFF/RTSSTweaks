// CPUInfo.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "CPUInfo.h"

#include <intrin.h>
#include <float.h>

#include "Log.h"
#include "OverlayEditor.h"
#include "ThreadAffinityMask.h"
/////////////////////////////////////////////////////////////////////////////
#define DWORD_AS_FLOAT(d) (*(FLOAT*)&d)
#define FLOAT_AS_DWORD(f) (*(DWORD*)&f)
/////////////////////////////////////////////////////////////////////////////
CCPUInfo::CCPUInfo()
{
	m_dwTimestamp					= 0;

	m_dwCpuCount					= 0;
	m_lpNtQuerySystemInformation	= NULL;

	m_dwCpuStepping					= 0;
	m_dwCpuModel					= 0;
	m_dwCpuModelExt					= 0;
	m_dwCpuFamily					= 0;
	m_dwCpuFamilyExt				= 0;
	ZeroMemory(m_szCpuString		, sizeof(m_szCpuString		));
	ZeroMemory(m_szCpuBrandString	, sizeof(m_szCpuBrandString	));
	m_dwCpuArch						= 0;

	m_dwCpuUsageCalc				= 0;

	m_hQuery						= NULL;

	for (DWORD dwCpu=0; dwCpu<MAX_CPU; dwCpu++)
	{
		m_idleTime[dwCpu].QuadPart				= 0;

		m_fltCpuUsage[dwCpu]					= FLT_MAX;
		m_dwTickCount[dwCpu]					= 0;

		m_hCounterCpuUtil[dwCpu]				= NULL;
		m_hCounterCpuPerf[dwCpu]				= NULL;

		m_dwTjmax[dwCpu]						= 0;

		m_dwMiscControlDevFn[dwCpu]				= 0;

		m_fltCorePower[dwCpu]					= FLT_MAX;
		m_dwCorePowerConsumed[dwCpu]			= 0;
		m_dwCorePowerConsumedTimestamp[dwCpu]	= 0;

		m_pcPerfTimestampCounter[dwCpu].QuadPart= 0;
		m_qwMPerf[dwCpu]						= 0;
		m_qwAPerf[dwCpu]						= 0;

		m_fltCpuClock[dwCpu]					= FLT_MAX;
	}

	m_pcTimestampCounter.QuadPart				= 0;
	m_qwTimestampCounter						= 0;
	m_qwTimestampClock							= 0;
	m_pcBaseClockCounter.QuadPart				= 0;
	m_qwBaseClockCounter						= 0;
	m_qwBaseClock								= 0;
	m_fltBusClock								= FLT_MAX;

	m_bMPerfSupported							= FALSE;

	m_dwTimestampCrystalClock					= 0;
	m_dwTimestampCrystalClockMul				= 0;
	m_dwTimestampCrystalClockDiv				= 0;

	m_dwNominalBaseClock						= 0;
	m_dwNominalMaxClock							= 0;
	m_dwNominalBusClock							= 0;

	m_dwMailboxBusClock							= 0;

	m_fltPowerUnit								= FLT_MAX;
	m_fltPackagePower							= FLT_MAX;
	m_dwPackagePowerConsumed					= 0;
	m_dwPackagePowerConsumedTimestamp			= 0;
}
/////////////////////////////////////////////////////////////////////////////
CCPUInfo::~CCPUInfo()
{
	if (m_hQuery)
		PdhCloseQuery(m_hQuery);

	m_hQuery = NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CCPUInfo::SetTimestamp(DWORD dwTimestamp)
{
	m_dwTimestamp = dwTimestamp;
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::AddPerfCounter(LPCSTR lpObjectName, LPCSTR lpInstanceName, LPCSTR lpCounterName, PDH_HCOUNTER* lpCounter)
{
	char	szLocalizedObjectName	[MAX_PATH];
	char	szLocalizedInstanceName	[MAX_PATH];
	char	szLocalizedCounterName	[MAX_PATH];

	m_objects.GetLocalizedName(lpObjectName		, szLocalizedObjectName		, sizeof(szLocalizedObjectName));
	m_objects.GetLocalizedName(lpInstanceName	, szLocalizedInstanceName	, sizeof(szLocalizedInstanceName));
	m_objects.GetLocalizedName(lpCounterName	, szLocalizedCounterName	, sizeof(szLocalizedCounterName));

	PDH_COUNTER_PATH_ELEMENTS elements;
	ZeroMemory(&elements, sizeof(elements));
	elements.szMachineName		= NULL;
	elements.szObjectName		= szLocalizedObjectName;
	elements.szInstanceName		= szLocalizedInstanceName;
	elements.szParentInstance	= NULL;
	elements.szCounterName		= szLocalizedCounterName;

	char	szCounterPath			[MAX_PATH];

	DWORD dwSize = sizeof(szCounterPath);
	PdhMakeCounterPath(&elements, szCounterPath, &dwSize, 0);

	PdhAddCounter(m_hQuery, szCounterPath, 0, lpCounter);
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::Init()
{
	//init CPU count 

	SYSTEM_INFO info;
	GetSystemInfo(&info); 

	m_dwCpuCount					= info.dwNumberOfProcessors;
	m_lpNtQuerySystemInformation	= (NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle("NTDLL"), "NtQuerySystemInformation");

	//init CPU performance counters

	if (PdhOpenQuery(0, 0, &m_hQuery) == ERROR_SUCCESS)
	{
		m_objects.Init();

		for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
		{
			char szInstanceName[MAX_PATH];
			sprintf_s(szInstanceName, sizeof(szInstanceName), "%d,%d", 0, dwCpu);

			AddPerfCounter("Processor Information", szInstanceName, "% Processor Utility"		, &m_hCounterCpuUtil[dwCpu]);
			AddPerfCounter("Processor Information", szInstanceName, "% Processor Performance"	, &m_hCounterCpuPerf[dwCpu]);
		}
	}

	//init CPU model related variables

	int cpuInfo[4] = { 0 };

	__cpuid(cpuInfo, 0);

	int nMax = cpuInfo[0];

    *((int*)(m_szCpuString		))	= cpuInfo[1];
	*((int*)(m_szCpuString + 4	))	= cpuInfo[3];
	*((int*)(m_szCpuString + 8	))	= cpuInfo[2];

	__cpuid(cpuInfo, 1);

	m_dwCpuStepping					= (cpuInfo[0]	 ) & 0x0F;
	m_dwCpuModel					= (cpuInfo[0]>>4 ) & 0x0F;
	m_dwCpuFamily					= (cpuInfo[0]>>8 ) & 0x0F;

	m_dwCpuModelExt					= (cpuInfo[0]>>16) & 0x0F;
	m_dwCpuFamilyExt				= (cpuInfo[0]>>20) & 0xFF;

	__cpuid(cpuInfo, 6);

	if (cpuInfo[2] & 1)
		m_bMPerfSupported = TRUE;

	__cpuid(cpuInfo, 0x80000000);

	char szCpuBrandString[0x40] = { 0 };

	int nMaxEx = cpuInfo[0];

	if  (nMaxEx >= 0x80000002)
	{
		__cpuid(cpuInfo, 0x80000002);
		CopyMemory(m_szCpuBrandString, cpuInfo, sizeof(cpuInfo));

	}

	if  (nMaxEx >= 0x80000003)
	{
		__cpuid(cpuInfo, 0x80000003);
		CopyMemory(m_szCpuBrandString + 16, cpuInfo, sizeof(cpuInfo));
	}

	if  (nMaxEx >= 0x80000004)
	{
		__cpuid(cpuInfo, 0x80000004);
		CopyMemory(m_szCpuBrandString + 32, cpuInfo, sizeof(cpuInfo));
	}

	//init CPU topology

	m_cpuTopology.Init();

	APPEND_LOG("Initializing CPU info");
	APPEND_LOG1("%s", GetCpuBrandString());
	APPEND_LOG4("%s family %X model %X stepping %X", GetCpuString(), GetCpuFamily(), GetCpuModel(), GetCpuStepping());
	APPEND_LOG1("%d logical cores", m_dwCpuCount);

	if (m_dwCpuCount > 1)
	{
		for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
		{
			APPEND_LOG3("CPU%d on package %d, core %d", dwCpu, GetPckg(dwCpu), GetCore(dwCpu));
		}
	}

	//init CPU model specific variables 

	if (IsIntelCpu())
	{
		APPEND_LOG("Initializing Intel CPU info");	

		InitIntelCpu();
	}

	if (IsAMDCpu())
	{
		APPEND_LOG("Initializing AMD CPU info");	

		InitAMDCpu();
	}

	APPEND_LOG1("CPU architecure %X detected", m_dwCpuArch);

	//calculate timestamp and bus clocks

	CalcBusClock();

	APPEND_LOG1("Estimated timestamp clock   : %.3f MHz", m_qwTimestampClock / 1000.0f);

	if (m_qwBaseClock)
	{
		APPEND_LOG1("Estimated base clock        : %.3f MHz", m_qwBaseClock / 1000.0f);
	}

	if (m_fltBusClock != FLT_MAX)
	{
 		APPEND_LOG1("Estimated bus clock         : %.3f MHz", m_fltBusClock);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsIntelCpu()
{
	return !_stricmp(m_szCpuString, "GenuineIntel");
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsAMDCpu()
{
	return !_stricmp(m_szCpuString, "AuthenticAMD");
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CCPUInfo::GetCpuString()
{
	return m_szCpuString;
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CCPUInfo::GetCpuBrandString()
{
	return m_szCpuBrandString;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetCpuFamily()
{
	if (IsIntelCpu())
	//	http://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf
	//
	// The Extended Family ID needs to be examined only when the Family ID is 0FH. Integrate the fields into a display
	// using the following rule:
	// IF Family_ID != 0FH
	// THEN DisplayFamily = Family_ID;
	// ELSE DisplayFamily = Extended_Family_ID + Family_ID;
	// (* Right justify and zero-extend 4-bit field. *)
	// FI;
	// (* Show DisplayFamily as HEX field. *)
	{
		if (m_dwCpuFamily != 0x0F)
			return m_dwCpuFamily;

		return m_dwCpuFamily + m_dwCpuFamilyExt;
	}

	if (IsAMDCpu())
	// http://support.amd.com/TechDocs/25481.pdf
	//
	// The Family is an 8-bit value and is defined as: Family[7:0] = ({0000b,BaseFamily[3:0]} + ExtendedFamily[7:0]).
	// For example, if BaseFamily[3:0] = Fh and ExtendedFamily[7:0] = 01h, then Family[7:0] = 10h. If
	// BaseFamily[3:0] is less than Fh then ExtendedFamily[7:0] is reserved and Family is equal to BaseFamily[3:0].
	{
		if (m_dwCpuFamily < 0x0F)
			return m_dwCpuFamily;

		return m_dwCpuFamily + m_dwCpuFamilyExt;
	}

	return m_dwCpuFamily;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetCpuModel()
{
	if (IsIntelCpu())
	//	http://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf
	//
	// The Extended Model ID needs to be examined only when the Family ID is 06H or 0FH. Integrate the field into a
	// display using the following rule:
	// IF (Family_ID = 06H or Family_ID = 0FH)
	// THEN DisplayModel = (Extended_Model_ID < 4) + Model_ID;
	// (* Right justify and zero-extend 4-bit field; display Model_ID as HEX field.*)
	// ELSE DisplayModel = Model_ID;
	// FI;
	// (* Show DisplayModel as HEX field. *)
	{
		if ((m_dwCpuFamily == 0x06) ||
			(m_dwCpuFamily == 0x0F))
			return m_dwCpuModel + (m_dwCpuModelExt<<4);

		return m_dwCpuModel;
	}

	if (IsAMDCpu())
	// http://support.amd.com/TechDocs/25481.pdf
	//
	//Model is an 8-bit value and is defined as: Model[7:0] = {ExtendedModel[3:0],BaseModel[3:0]}. For example,
	//if ExtendedModel[3:0] = Eh and BaseModel[3:0] = 8h, then Model[7:0] = E8h. If BaseFamily[3:0] is less than
	//0Fh then ExtendedModel[3:0] is reserved and Model is equal to BaseModel[3:0].
	{
		if (m_dwCpuFamily < 0x0F)
			return m_dwCpuModel;

		return m_dwCpuModel + (m_dwCpuModelExt<<4);
	}

	return m_dwCpuModel;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetCpuStepping()
{
	return m_dwCpuStepping;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetCpuCount()
{
	return m_dwCpuCount;
}
/////////////////////////////////////////////////////////////////////////////
void CCPUInfo::CalcCpuUsage_SystemProcessorPerformanceInformation()
{
	DWORD dwTickCount = GetTickCount();

	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION info[MAX_CPU];

	if (SUCCEEDED(m_lpNtQuerySystemInformation(SystemProcessorPerformanceInformation, &info, sizeof(info), NULL)))
			//query CPU usage
	{
		for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
		{
			if (m_idleTime[dwCpu].QuadPart)
				//ensure that this function was already called at least once
				//and we have the previous idle time value
			{
				m_fltCpuUsage[dwCpu] = 100.0f - 0.01f * (info[dwCpu].IdleTime.QuadPart - m_idleTime[dwCpu].QuadPart) / (dwTickCount - m_dwTickCount[dwCpu]);
					//calculate new CPU usage value by estimating amount of time
					//CPU was in idle during the last second
					
				//clip calculated CPU usage to [0-100] range to filter calculation non-ideality

				if (m_fltCpuUsage[dwCpu] < 0.0f)
					m_fltCpuUsage[dwCpu] = 0.0f;

				if (m_fltCpuUsage[dwCpu] > 100.0f)
					m_fltCpuUsage[dwCpu] = 100.0f;
			}

			m_idleTime[dwCpu]		= info[dwCpu].IdleTime;
				//save new idle time for specified CPU
			m_dwTickCount[dwCpu]	= dwTickCount;
				//save new tick count for specified CPU
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCPUInfo::CalcCpuUsage_SystemProcessorIdleInformation()
{
	DWORD dwTickCount = GetTickCount();

	SYSTEM_PROCESSOR_IDLE_INFORMATION info[MAX_CPU];

	if (SUCCEEDED(m_lpNtQuerySystemInformation(SystemProcessorIdleInformation, &info, sizeof(info), NULL)))
			//query CPU usage
	{
		for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
		{
			if (m_idleTime[dwCpu].QuadPart)
				//ensure that this function was already called at least once
				//and we have the previous idle time value
			{
				m_fltCpuUsage[dwCpu] = 100.0f - 0.01f * (info[dwCpu].IdleTime - m_idleTime[dwCpu].QuadPart) / (dwTickCount - m_dwTickCount[dwCpu]);
					//calculate new CPU usage value by estimating amount of time
					//CPU was in idle during the last second
					
				//clip calculated CPU usage to [0-100] range to filter calculation non-ideality

				if (m_fltCpuUsage[dwCpu] < 0.0f)
					m_fltCpuUsage[dwCpu] = 0.0f;

				if (m_fltCpuUsage[dwCpu] > 100.0f)
					m_fltCpuUsage[dwCpu] = 100.0f;
			}

			m_idleTime[dwCpu].QuadPart	= info[dwCpu].IdleTime;
				//save new idle time for specified CPU
			m_dwTickCount[dwCpu]		= dwTickCount;
				//save new tick count for specified CPU
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCPUInfo::CalcCpuUsage_ProcessorUtility()
{
	DWORD dwTickCount = GetTickCount();

	if (m_hQuery)
	{
		if (PdhCollectQueryData(m_hQuery) == ERROR_SUCCESS)
		{
			for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
			{
				float fltUtil = FLT_MAX;

				DWORD dwType;

				PDH_FMT_COUNTERVALUE value;
				ZeroMemory(&value, sizeof(value));

				if (PdhGetFormattedCounterValue(m_hCounterCpuUtil[dwCpu], PDH_FMT_DOUBLE, &dwType, &value) == ERROR_SUCCESS)
					fltUtil = (float)value.doubleValue;

				if (fltUtil != FLT_MAX)
				{
					m_fltCpuUsage[dwCpu] = fltUtil;

					if (m_fltCpuUsage[dwCpu] < 0.0f)
						m_fltCpuUsage[dwCpu] = 0.0f;

					if (m_fltCpuUsage[dwCpu] > 100.0f)
						m_fltCpuUsage[dwCpu] = 100.0f;

					m_dwTickCount[dwCpu] = dwTickCount;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCPUInfo::CalcCpuUsage_ProcessorUtilityNormalized()
{
	DWORD dwTickCount = GetTickCount();

	if (m_hQuery)
	{
		if (PdhCollectQueryData(m_hQuery) == ERROR_SUCCESS)
		{
			for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
			{
				float fltUtil = FLT_MAX;
				float fltPerf = FLT_MAX;

				DWORD dwType;

				PDH_FMT_COUNTERVALUE value;
				ZeroMemory(&value, sizeof(value));

				if (PdhGetFormattedCounterValue(m_hCounterCpuUtil[dwCpu], PDH_FMT_DOUBLE, &dwType, &value) == ERROR_SUCCESS)
					fltUtil = (float)value.doubleValue;

				if (PdhGetFormattedCounterValue(m_hCounterCpuPerf[dwCpu], PDH_FMT_DOUBLE, &dwType, &value) == ERROR_SUCCESS)
					fltPerf = (float)value.doubleValue;

				if ((fltPerf != FLT_MAX) && 
					(fltUtil != FLT_MAX))
				{	
					m_fltCpuUsage[dwCpu] = 100.0f * fltUtil / fltPerf;

					if (m_fltCpuUsage[dwCpu] < 0.0f)
						m_fltCpuUsage[dwCpu] = 0.0f;

					if (m_fltCpuUsage[dwCpu] > 100.0f)
						m_fltCpuUsage[dwCpu] = 100.0f;

					m_dwTickCount[dwCpu] = dwTickCount;
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
float CCPUInfo::GetCpuUsage(DWORD dwCpu)
{
	//validate NtQuerySystemInformation pointer and return FLT_MAX on error

	if (!m_lpNtQuerySystemInformation)
		return FLT_MAX;

	//validate specified CPU index and return FLT_MAX on error

	if (dwCpu >=  m_dwCpuCount)
		return FLT_MAX;

	DWORD dwTickCount = GetTickCount();
		//get standard timer tick count

	if (dwTickCount - m_dwTickCount[dwCpu] >= 100)
		//update usage once per 100 ms
	{
		switch (m_dwCpuUsageCalc)
		{
		case 0:
			CalcCpuUsage_SystemProcessorPerformanceInformation();
			break;
		case 1:
			CalcCpuUsage_SystemProcessorIdleInformation();
			break;
		case 2:
			CalcCpuUsage_ProcessorUtility();
			break;
		case 3:
			CalcCpuUsage_ProcessorUtilityNormalized();
			break;
		}
	}

	return m_fltCpuUsage[dwCpu];
}
/////////////////////////////////////////////////////////////////////////////
float CCPUInfo::GetTotalCpuUsage()
{
	float result = FLT_MAX;

	for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
	{
		float usage = GetCpuUsage(dwCpu);

		if (usage != FLT_MAX)
		{
			if (result != FLT_MAX)
				result = result + usage;
			else
				result = usage;
		}
	}

	if (result != FLT_MAX)
	{
		if (m_dwCpuCount)
			result = result / m_dwCpuCount;
	}

	return result;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetMaxCpuUsage()
{
	float result = 0.0f;

	for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
	{
		float usage = GetCpuUsage(dwCpu);

		if (usage != FLT_MAX)
		{
			if (result < usage)
				result = usage;
		}
	}

	return result;
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::Delay1us(DWORD dwNumCycles)
{
	LARGE_INTEGER pf;

	if (QueryPerformanceFrequency(&pf))
	{
		__int64 ticks = pf.QuadPart * dwNumCycles / 1000000;

		LARGE_INTEGER pc0;
		QueryPerformanceCounter(&pc0);
		LARGE_INTEGER pc1;
		do 
			QueryPerformanceCounter(&pc1);
		while (pc1.QuadPart - pc0.QuadPart < ticks);
	}
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::Delay1ms(DWORD dwNumCycles)
{
	LARGE_INTEGER pf;

	if (QueryPerformanceFrequency(&pf))
	{
		__int64 ticks = pf.QuadPart * dwNumCycles / 1000;

		LARGE_INTEGER pc0;
		QueryPerformanceCounter(&pc0);
		LARGE_INTEGER pc1;
		do 
			QueryPerformanceCounter(&pc1);
		while (pc1.QuadPart - pc0.QuadPart < ticks);
	}
}
//////////////////////////////////////////////////////////////////////
unsigned __int64 CCPUInfo::CalcTimestampClock(DWORD dwPeriod)
{
	CThreadAffinityMask affinity(m_dwCpuCount, 0);

	LARGE_INTEGER pf;

	if (QueryPerformanceFrequency(&pf))
	{
		unsigned __int64 qwTimestampCounter = __rdtsc();

		LARGE_INTEGER pc;

		QueryPerformanceCounter(&pc); 

		if (m_pcTimestampCounter.QuadPart)
		{
			double time = 1000.0f * (pc.QuadPart - m_pcTimestampCounter.QuadPart) / pf.QuadPart;

			if (time >= dwPeriod)
			{
				m_qwTimestampClock				= (unsigned __int64)((qwTimestampCounter - m_qwTimestampCounter) / time);

				m_pcTimestampCounter.QuadPart	= pc.QuadPart;
				m_qwTimestampCounter			= qwTimestampCounter;
			}
		}
		else
		{
			m_pcTimestampCounter.QuadPart	= pc.QuadPart;
			m_qwTimestampCounter			= qwTimestampCounter;

			Delay1ms(10);

			return CalcTimestampClock(0);
		}
	}

	return m_qwTimestampClock;
}
//////////////////////////////////////////////////////////////////////
unsigned __int64 CCPUInfo::CalcBaseClock(DWORD dwPeriod)
{
	if (m_bMPerfSupported)
	{
		CThreadAffinityMask affinity(m_dwCpuCount, 0);

		LARGE_INTEGER pf;

		if (QueryPerformanceFrequency(&pf))
		{
			unsigned __int64 qwBaseClockCounter = 0;

			QueryMPerf(&qwBaseClockCounter);

			LARGE_INTEGER pc;

			QueryPerformanceCounter(&pc); 

			if (m_pcBaseClockCounter.QuadPart)
			{
				double time = 1000.0f * (pc.QuadPart - m_pcBaseClockCounter.QuadPart) / pf.QuadPart;

				if (time >= dwPeriod)
				{
					//TSC clock is no longer multiple of BCLK on Intel CPUs starting from Skylake, it is derived from fixed crystal clock (e.g 24MHz
					//crystal clock on i7 10700K and fixed 316 * 24MHZ / 2 = 3792MHz TSC clock)
					//Due to such clocking TSC clock is close to but not exactly equal to the base CPU clock for nominal BCLK (e.g. 3792MHz vs 3800MHz
					//on i7 10700K on nominal 100MHz BCLK). So we can still use TSC clock to estimate relatively accurate BCLK from TSC clock on such
					//CPU, but overclocked BCLK detection won't work. 
					//But we can alternately estimate base clock and try to calculate overclocked BCLK from it instead of fixed TSC clock

					unsigned __int64 qwBaseClock	= (unsigned __int64)((qwBaseClockCounter - m_qwBaseClockCounter) / time);
						//calculate base clock from MPERF delta and absolute timings

					//Base clock estimation is tricky: MPERF is incremented in C0 only, so we filter results below TSC clock, which mean that either
					//core was sleeping during estimation or BCLK is below nominal (so downclocked BCLK detection is not supported)

					//NOTE: there is explicit busy wait inside Delay1ms during the very first base clock estimation step, which should provide us
					//immediate initial result

					if (qwBaseClock > m_qwTimestampClock)
						m_qwBaseClock				= qwBaseClock;

					m_pcBaseClockCounter.QuadPart	= pc.QuadPart;
					m_qwBaseClockCounter			= qwBaseClockCounter;
				}
			}
			else
			{
				m_pcBaseClockCounter.QuadPart	= pc.QuadPart;
				m_qwBaseClockCounter			= qwBaseClockCounter;

				Delay1ms(100);

				return CalcBaseClock(0);
			}
		}
	}

	return m_qwBaseClock;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::CalcBusClock()
{
	CalcTimestampClock();
	CalcBaseClock();

	if (IsIntelCpu())
		m_fltBusClock = GetIntelBusClock();

	if (IsAMDCpu())
		m_fltBusClock = GetAMDBusClock();

	return m_fltBusClock;
}
//////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetPckg(DWORD dwCpu)
{
	return m_cpuTopology.GetPckg(dwCpu);
}
//////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetCore(DWORD dwCpu)
{
	return m_cpuTopology.GetCore(dwCpu);
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::InitIntelCpu()
{
	DWORD dwCpuFamily	= GetCpuFamily();
	DWORD dwCpuModel	= GetCpuModel();
	DWORD dwCpuStepping	= GetCpuStepping();

	if (IsIntelCpu())
	{
		for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
		{
			switch (dwCpuFamily)	
			{
			case 0x06:
				switch (dwCpuModel)
					{
					case 0x0F: // Intel Core 2 (65nm)
						m_dwCpuArch = CPU_ARCH_INTEL_CORE;
						switch (dwCpuStepping)
						{
						case 0x06:	//B2
							switch (m_dwCpuCount)
							{
							case 2:
								m_dwTjmax[dwCpu] = 90;
								break;
							case 4:
								m_dwTjmax[dwCpu] = 100;
								break;
							default:
								m_dwTjmax[dwCpu] = 95;
								break;
							}
							break;
						case 0x0B:	//G0
							m_dwTjmax[dwCpu] = 100;
							break;
						case 0x0D:	//M0
							m_dwTjmax[dwCpu] = 95;
							break;
						default:
							m_dwTjmax[dwCpu] = 95;
							break;
						}
						break;
					case 0x17:	//Intel Core 2 (45nm) 
						m_dwCpuArch = CPU_ARCH_INTEL_CORE;
						m_dwTjmax[dwCpu] = 100;
						break;
					case 0x1C:	//Intel Atom (45nm) 
						m_dwCpuArch = CPU_ARCH_INTEL_ATOM;
						switch (dwCpuStepping)
						{
						case 0x02:	//C0
							m_dwTjmax[dwCpu] = 90;
							break;
						case 0x0A:	//A0, B0
							m_dwTjmax[dwCpu] = 100;
							break;
						default:
							m_dwTjmax[dwCpu] = 90;
							break;
						}
						break;
					case 0x1A: // Intel Core i7 LGA1366 (45nm)
					case 0x1E: // Intel Core i5, i7 LGA1156 (45nm)
					case 0x1F: // Intel Core i5, i7 
					case 0x25: // Intel Core i3, i5, i7 LGA1156 (32nm)
					case 0x2C: // Intel Core i7 LGA1366 (32nm) 6 Core
					case 0x2E: // Intel Xeon Processor 7500 series (45nm)
					case 0x2F: // Intel Xeon Processor (32nm)
						m_dwCpuArch = CPU_ARCH_INTEL_NEHALEM;
						break;
					case 0x2A: // Intel Core i5, i7 2xxx LGA1155 (32nm)
					case 0x2D: // Next Generation Intel Xeon, i7 3xxx LGA2011 (32nm)
						m_dwCpuArch = CPU_ARCH_INTEL_SANDYBRIDGE;
						break;
					case 0x3A: // Intel Core i5, i7 3xxx LGA1155 (22nm)
					case 0x3E: // Intel Core i7 4xxx LGA2011 (22nm)
						m_dwCpuArch = CPU_ARCH_INTEL_IVYBRIDGE;
						break;
					case 0x3C: // Intel Core i5, i7 4xxx LGA1150 (22nm)              
					case 0x3F: // Intel Xeon E5-2600/1600 v3, Core i7-59xx
							 // LGA2011-v3, Haswell-E (22nm)
					case 0x45: // Intel Core i5, i7 4xxxU (22nm)
					case 0x46: 
						m_dwCpuArch = CPU_ARCH_INTEL_HASWELL;
						break;
					case 0x3D: // Intel Core M-5xxx (14nm)
					case 0x47: // Intel i5, i7 5xxx, Xeon E3-1200 v4 (14nm)
					case 0x4F: // Intel Xeon E5-26xx v4
					case 0x56: // Intel Xeon D-15xx
						m_dwCpuArch = CPU_ARCH_INTEL_BROADWELL;
						break;
					case 0x36: // Intel Atom S1xxx, D2xxx, N2xxx (32nm)
						m_dwCpuArch = CPU_ARCH_INTEL_ATOM;
						break;
					case 0x37: // Intel Atom E3xxx, Z3xxx (22nm)
					case 0x4A:
					case 0x4D: // Intel Atom C2xxx (22nm)
					case 0x5A:
					case 0x5D:
						m_dwCpuArch = CPU_ARCH_INTEL_SILVERMONT;
						break;
					case 0x4E:
					case 0x5E: // Intel Core i5, i7 6xxxx LGA1151 (14nm)
					case 0x55: // Intel Core i7, i9 7xxxx LGA2066 (14nm)
						m_dwCpuArch = CPU_ARCH_INTEL_SKYLAKE;
						break;
					case 0x4C:
						m_dwCpuArch = CPU_ARCH_INTEL_AIRMONT;
						break;
					case 0x8E: 
					case 0x9E: // Intel Core i5, i7 7xxxx (14nm)
						m_dwCpuArch = CPU_ARCH_INTEL_KABYLAKE;
						break;
					case 0x5C: // Intel Atom processors (Apollo Lake) (14nm)
					case 0x5F: // Intel Atom processors (Denverton) (14nm)
						m_dwCpuArch = CPU_ARCH_INTEL_GOLDMONT;
						break; 
					case 0x7A: // Intel Atom processors (14nm)
						m_dwCpuArch = CPU_ARCH_INTEL_GOLDMONTPLUS;
						break;
					case 0x66: // Intel Core i3 8121U (10nm)
						m_dwCpuArch = CPU_ARCH_INTEL_CANNONLAKE;
		                break;
					case 0x7D: // Intel Core i3, i5, i7 10xxGx (10nm) 
					case 0x7E: 
					case 0x6A: // Intel Xeon (10nm)
					case 0x6C:
						m_dwCpuArch = CPU_ARCH_INTEL_ICELAKE;
						break;
					case 0xA5:
					case 0xA6: // Intel Core i3, i5, i7 10xxx (14nm)
						m_dwCpuArch = CPU_ARCH_INTEL_COMETLAKE;
						break;
					case 0xA7: // Intel Core i3, i5, i7, i9 11xxx (14nm)
						m_dwCpuArch = CPU_ARCH_INTEL_ROCKETLAKE;
						break;
					case 0x97: //  Intel Core i3, i5, i7, i9 12xxx (10nm)
					case 0x9A:
					case 0xBE:
						m_dwCpuArch = CPU_ARCH_INTEL_ALDERLAKE;
						break;
					case 0xB7: //  Intel Core i3, i5, i7, i9 13xxx (7nm)
					case 0xBA:
					case 0xBF:
						m_dwCpuArch = CPU_ARCH_INTEL_RAPTORLAKE;
						break;
					case 0x86: // Intel Atom processors
						m_dwCpuArch = CPU_ARCH_INTEL_TREMONT;
						break;
					case 0x8C: // Intel processors (10nm++)
					case 0x8D:
						m_dwCpuArch = CPU_ARCH_INTEL_TIGERLAKE;
						break;
					}
				break;
			case 0x0F:
				{
					switch (dwCpuModel)
					{
					case 0x00: // Pentium 4 (180nm)
					case 0x01: // Pentium 4 (130nm)
					case 0x02: // Pentium 4 (130nm) 
					case 0x03: // Pentium 4, Celeron D (90nm)  
					case 0x04: // Pentium 4, Pentium D, Celeron D (90nm) 
					case 0x06: // Pentium 4, Pentium D, Celeron D (65nm)
						m_dwCpuArch = CPU_ARCH_INTEL_NETBURST;
						m_dwTjmax[dwCpu] = 100;
						break;
					default:
						m_dwTjmax[dwCpu] = 100;
						break;
					}
				}
				break;
			default:
				m_dwTjmax[dwCpu] = 0;
				break;
			}

			if (!m_dwTjmax[dwCpu])
			{
				CThreadAffinityMask affinity(m_dwCpuCount, dwCpu);

				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex = 0x1A2;	//MSR_TEMPERATURE_TARGET 
				if (g_driver.ReadMSR(&info))
					m_dwTjmax[dwCpu] = (info.dwLow>>16) & 0x7F;

				APPEND_LOG2("CPU%d Tjmax : %d°C", dwCpu, m_dwTjmax[dwCpu]);
			}
			else
			{
				APPEND_LOG2("CPU%d Tjmax : %d°C hardcoded", dwCpu, m_dwTjmax[dwCpu]);
			}

			//init power reporting units

			if ((m_dwCpuArch == CPU_ARCH_INTEL_SANDYBRIDGE	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_IVYBRIDGE	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_HASWELL		) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_BROADWELL	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_SKYLAKE		) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_SILVERMONT	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_AIRMONT		) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_KABYLAKE		) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_GOLDMONT		) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_GOLDMONTPLUS	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_CANNONLAKE	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_ICELAKE		) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_COMETLAKE	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_TREMONT		) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_TIGERLAKE	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_ROCKETLAKE	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_ALDERLAKE	) ||
				(m_dwCpuArch == CPU_ARCH_INTEL_RAPTORLAKE	))
			{
				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex = 0x606; //MSR_RAPL_POWER_UNIT

				if (g_driver.ReadMSR(&info))
				{
					switch (m_dwCpuArch)
					{
					case CPU_ARCH_INTEL_SILVERMONT:
					case CPU_ARCH_INTEL_AIRMONT:
						m_fltPowerUnit = 1.0e-6f * (1 << (int)((info.dwLow >> 8) & 0x1F));
						break;
					default:
						m_fltPowerUnit = 1.0f / (1 << (int)((info.dwLow >> 8) & 0x1F));
						break;
					}
				}
			}
		}

		int cpuInfo[4] = { 0 };

		__cpuid(cpuInfo, 0);

		int nMax = cpuInfo[0];

		if (nMax >= 0x15)
		{
			__cpuid(cpuInfo, 0x15);

			APPEND_LOG4("CPUID.15H : %08X %08X %08X %08X", cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
				//crystal clock divisor, crystal clock multipler, crystal clock in Hz (optional, may be zero)

			m_dwTimestampCrystalClockDiv	= cpuInfo[0];			
			m_dwTimestampCrystalClockMul	= cpuInfo[1];			
			m_dwTimestampCrystalClock		= cpuInfo[2] / 1000;	//convert to KHz from Hz	

			APPEND_LOG1("Timestamp crystal clock div : %d"		, m_dwTimestampCrystalClockDiv);
			APPEND_LOG1("Timestamp crystal clock mul : %d"		, m_dwTimestampCrystalClockMul);
			APPEND_LOG1("Timestamp crystal clock     : %.3f MHz", m_dwTimestampCrystalClock / 1000.0f);
		}

		if (nMax >= 0x16)
		{
			__cpuid(cpuInfo, 0x16);

			APPEND_LOG4("CPUID.16H : %08X %08X %08X %08X", cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
				//nominal base clock in MHz, nominal max clock in MHz, nominal bus clock in MHz

			m_dwNominalBaseClock			= cpuInfo[0] * 1000;	//convert to KHz from MHz	
			m_dwNominalMaxClock				= cpuInfo[1] * 1000;	//convert to KHz from MHz
			m_dwNominalBusClock				= cpuInfo[2] * 1000;	//convert to KHz from MHz

			APPEND_LOG1("Nominal base clock          : %.3f MHz"	, m_dwNominalBaseClock / 1000.0f);
			APPEND_LOG1("Nominal max clock           : %.3f MHz"	, m_dwNominalMaxClock / 1000.0f);
			APPEND_LOG1("Nominal bus clock           : %.3f MHz"	, m_dwNominalBusClock / 1000.0f);

			if (!m_dwTimestampCrystalClock && m_dwTimestampCrystalClockMul && m_dwTimestampCrystalClockDiv)
				//if crystal clock is not specified in CPUID.15H, we can reconstruct it from nominal base clock and timestamp crystal clock mul/div
			{
				m_dwTimestampCrystalClock = GetNearestCrystalClock(m_dwNominalBaseClock * m_dwTimestampCrystalClockDiv / m_dwTimestampCrystalClockMul);

				if (m_dwTimestampCrystalClock)
				{
					APPEND_LOG1("Timestamp crystal clock     : %.3f MHz", m_dwTimestampCrystalClock / 1000.0f);
				}
			}

			//Skylake and newer CPUs can report BCLK via OC mailbox interface

			DWORD dwBusClock = 0;

			if (WriteCommandToOCMailbox(5 /*read BCLK command*/, 0, &dwBusClock))
			{
				APPEND_LOG1("OC mailbox bus clock        : %.3f MHz", dwBusClock / 1000.0f);

				//NOTE: OC Mailbox interface is not available if HVCI is enabled, abnormally low BCLK can be reported in this case so we filter result
				//and check if BCLK is in [50%-200%] range of nominal 

				if ((dwBusClock >= m_dwNominalBusClock / 2) &&
					(dwBusClock <= m_dwNominalBusClock * 2))
					m_dwMailboxBusClock = dwBusClock;
			}
		}

		if (m_dwTimestampCrystalClock && m_dwTimestampCrystalClockMul && m_dwTimestampCrystalClockDiv)
		{
			APPEND_LOG1("Timestamp clock             : %.3f MHz", (m_dwTimestampCrystalClock * m_dwTimestampCrystalClockMul / m_dwTimestampCrystalClockDiv) / 1000.0f);
		}
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetNearestCrystalClock(DWORD dwClock)
{
	LONG dwCrystalClock			= 0;
	LONG dwCrystalClockDelta	= INT_MAX;
	LONG dwCrystalClocks[]		= { 24000, 25000, 19200 };

	for (DWORD dwIndex=0; dwIndex<_countof(dwCrystalClocks); dwIndex++)
	{	
		LONG dwCrystalClockCur		= dwCrystalClocks[dwIndex];
		LONG dwCrystalClockDeltaCur	= abs((LONG)dwClock - dwCrystalClockCur);

		if (dwCrystalClockDeltaCur < dwCrystalClockDelta)
		{
			dwCrystalClockDelta = dwCrystalClockDeltaCur;
			dwCrystalClock		= dwCrystalClockCur;
		}
	}

	if (abs(dwCrystalClockDelta) < 1000)
		return dwCrystalClock;

	return 0;
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::InitAMDCpu()
{
	DWORD dwCpuFamily	= GetCpuFamily();
	DWORD dwCpuModel	= GetCpuModel();
	DWORD dwCpuStepping	= GetCpuStepping();

	if (IsAMDCpu())
	{
		DWORD dwDeviceID = 0;

		switch (dwCpuFamily)
		{
		case 0x10:
			m_dwCpuArch = CPU_ARCH_AMD_FAMILY_10H;
			dwDeviceID	= 0x12031022;
			break;
		case 0x11:
			m_dwCpuArch = CPU_ARCH_AMD_FAMILY_11H;
			dwDeviceID	= 0x13031022;
			break;
		case 0x12:
			m_dwCpuArch = CPU_ARCH_AMD_FAMILY_12H;
			dwDeviceID	= 0x17031022;
			break;
		case 0x14:
			m_dwCpuArch = CPU_ARCH_AMD_FAMILY_14H;
			dwDeviceID	= 0x17031022;
			break;
		case 0x15:
			switch (dwCpuModel & 0xF0)
			{
			case 0x00:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_15H_MODEL_00;
				dwDeviceID	= 0x16031022;
				break;
			case 0x10:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_15H_MODEL_10;
				dwDeviceID	= 0x14031022;
				break;
			case 0x30:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_15H_MODEL_30;
				dwDeviceID	= 0x141D1022;
				break;
			}
			break;
		case 0x16:
			switch (dwCpuModel & 0xF0)
			{
			case 0x00:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_16H_MODEL_00;
				dwDeviceID	= 0x15331022;
				break;
			case 0x30:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_16H_MODEL_30;
				dwDeviceID	= 0x15831022;
				break;
			}
			break;
		case 0x17:
			switch (dwCpuModel & 0xF0)
			{
			case 0x00:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_17H_MODEL_00;	//Ryzen 1xxx/2xxx
				break;
			case 0x10:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_17H_MODEL_10;	//Ryzen 2xxx/3xxx APU/mobile
				break;
			case 0x30:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_17H_MODEL_30;	//Threadripper 3xxx
				break;
			case 0x60:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_17H_MODEL_60;	//Ryzen 4xxx APU/mobile
				break;
			case 0x70:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_17H_MODEL_70;	//Ryzen 3xxx 
				break;
			}
			break;
		case 0x19:
			switch (dwCpuModel & 0xF0)
			{
			case 0x20:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_19H_MODEL_20;	//Ryzen 5xxx
				break;
			case 0x40:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_19H_MODEL_40; //Ryzen 6xxx
				break;
			case 0x50:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_19H_MODEL_50;	//Ryzen 5xxx APU/mobile
				break;
			case 0x60:
				m_dwCpuArch = CPU_ARCH_AMD_FAMILY_19H_MODEL_60; //Ryzen 7xxx
				break;
			}
		}

		APPEND_LOG1("DeviceID : %X", dwDeviceID);

		for (DWORD dwCpu=0; dwCpu<min(m_dwCpuCount, 8); dwCpu++)
		{
			m_dwMiscControlDevFn[dwCpu] = 0;

			if (dwDeviceID)
			{
				DWORD dwDev			= 0x18 + dwCpu;
				DWORD dwFn			= 0x03;

				DWORD dwDevFn		= (dwDev<<3) | dwFn;

				DWORD dwCpuDeviceID = ReadBusDataUlong(0, dwDevFn, 0);

				if (dwCpuDeviceID == dwDeviceID)
					m_dwMiscControlDevFn[dwCpu] = dwDevFn;

				APPEND_LOG4("CPU%d bus 0, device %X, function %X : %X", dwCpu, dwDev, dwFn, dwCpuDeviceID);

				if (dwCpuDeviceID == 0xFFFFFFFF)
					break;
			}
		}

		if (IsAMDZenArchitecture())
		{
			MSR_INFO info;
			ZeroMemory(&info, sizeof(info));

			info.dwIndex = 0xC0010299; //AMD_F17_MSR_RAPL_POWER_UNIT

			if (g_driver.ReadMSR(&info))
				m_fltPowerUnit = 1.0f / (1 << (int)((info.dwLow >> 8) & 0x1F));
		}
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::ReadBusDataUlong(DWORD bus, DWORD devFn, DWORD offset)
{
	HANDLE hMutex	= CreateMutex(NULL, FALSE, "Global\\Access_PCI");
	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);

	BUSDATA_INFO info;

	info.dwBus			= bus;
	info.dwDev			= devFn>>3;
	info.dwFn			= devFn & 7;
	info.dwOffset		= offset;
	info.dwDataSize		= 4;
	info.dwData			= 0xFFFFFFFF;

	DWORD dwResult;

	if (g_driver.GetBusData(&info))
		dwResult = info.dwData;
	else
		dwResult = 0xFFFFFFFF;

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	return dwResult;
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::WriteBusDataUlong(DWORD bus, DWORD devFn, DWORD offset, DWORD data)
{
	HANDLE hMutex	= CreateMutex(NULL, FALSE, "Global\\Access_PCI");
	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);

	BUSDATA_INFO info;

	info.dwBus			= bus;
	info.dwDev			= devFn>>3;
	info.dwFn			= devFn & 7;
	info.dwOffset		= offset;
	info.dwDataSize		= 4;
	info.dwData			= data;

	g_driver.SetBusData(&info);

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsAMDZenArchitecture()
{
	if ((m_dwCpuArch == CPU_ARCH_AMD_FAMILY_17H_MODEL_00) ||
		(m_dwCpuArch == CPU_ARCH_AMD_FAMILY_17H_MODEL_10) ||
		(m_dwCpuArch == CPU_ARCH_AMD_FAMILY_17H_MODEL_30) ||
		(m_dwCpuArch == CPU_ARCH_AMD_FAMILY_17H_MODEL_60) ||
		(m_dwCpuArch == CPU_ARCH_AMD_FAMILY_17H_MODEL_70) ||
		(m_dwCpuArch == CPU_ARCH_AMD_FAMILY_19H_MODEL_20) ||
		(m_dwCpuArch == CPU_ARCH_AMD_FAMILY_19H_MODEL_40) ||
		(m_dwCpuArch == CPU_ARCH_AMD_FAMILY_19H_MODEL_50) ||
		(m_dwCpuArch == CPU_ARCH_AMD_FAMILY_19H_MODEL_60))
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsCpuTemperatureReportingSupported(DWORD dwCpu)
{
	if (g_driver.GetHandle() == INVALID_HANDLE_VALUE)
		return FALSE;

	if (IsIntelCpu())
		return IsIntelCpuTemperatureReportingSupported(dwCpu);

	if (IsAMDCpu())
	{	
		if (dwCpu == 0xFFFFFFFF)
			//package temperature
			return FALSE;

		dwCpu = m_cpuTopology.GetPckg(dwCpu);

		return IsAMDCpuTemperatureReportingSupported(dwCpu);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsIntelCpuTemperatureReportingSupported(DWORD dwCpu)
{
	if (IsIntelCpu())
	{
		if (dwCpu == 0xFFFFFFFF)
			//package temperature
		{
			int cpuInfo[4]	= { 0 };
			__cpuid(cpuInfo, 6);

			if (cpuInfo[0] & 0x40)
				return TRUE;
		}
		else
			//core temperature
		{
			CThreadAffinityMask affinity(m_dwCpuCount, dwCpu);

			int cpuInfo[4]	= { 0 };
			__cpuid(cpuInfo, 6);

			if (cpuInfo[0] & 1)
				return TRUE;
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsAMDCpuTemperatureReportingSupported(DWORD dwCpu)
{
	if (IsAMDCpu())
	{
		if (dwCpu == 0xFFFFFFFF)
			//package temperature
		{
			return FALSE;
		}
		else
		{
			if (IsAMDZenArchitecture())
			{
				if (!dwCpu)
					return TRUE;

				return FALSE;
			}
			else
			{
				if (m_dwMiscControlDevFn[dwCpu])
					return TRUE;

				return FALSE;
			}
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetCpuTemperature(DWORD dwCpu)
{
	if (IsIntelCpu())
		return GetIntelCpuTemperature(dwCpu);

	if (IsAMDCpu())
	{
		//On AMD CPUs there are no per-core thermal sensors, so we duplicate readings of the same single 
		//per-package temperature sensor for all cores. It is not necessary to reread the sensor for each
		//core, so we can save a lot of CPU time if we physically read it just once per polling iteration
		//and return precached values on subsequent calls for the rest cores.

		dwCpu = m_cpuTopology.GetPckg(dwCpu);

		//We can use HAL timestamp, which is updated by it on each new hardware polling iteration, so we 
		//can always determine if we need to reread the sensor or can return previously cached value

		if (m_dwTimestamp)
		{
			LPTIMESTAMPED_DATA lpData = m_cpuTemperatureCache.GetData(dwCpu, m_dwTimestamp);

			if (lpData)
				//precached data is available for this timestamp, so we can just return it
				return DWORD_AS_FLOAT(lpData->dwData);

			//otherwise we need to reread the sensor and add it to cache

			float fltTemperature = GetAMDCpuTemperature(dwCpu);

			m_cpuTemperatureCache.SetData(dwCpu, m_dwTimestamp, FLOAT_AS_DWORD(fltTemperature));

			return fltTemperature;
		}

		return GetAMDCpuTemperature(dwCpu);
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetMaxCpuTemperature()
{
	float result = 0.0f;

	for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
	{
		float temperature = GetCpuTemperature(dwCpu);

		if (temperature != FLT_MAX)
		{
			if (result < temperature)
				result = temperature;
		}
	}

	return result;
}
//////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetTjmax(DWORD dwCpu)
{
	return m_dwTjmax[dwCpu];
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetIntelCpuTemperature(DWORD dwCpu)
{
	if (IsIntelCpu())
	{
		float result = FLT_MAX;

		DWORD dwTjmax = GetTjmax((dwCpu == 0xFFFFFFFF) ? 0 : dwCpu);	//special case for package temperature

		if (dwTjmax)
		{
			if (dwCpu == 0xFFFFFFFF)
				//package temperature
			{
				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				DWORD dwTdist = 0;

				info.dwIndex = 0x1B1;	//IA32_PACKAGE_THERM_STATUS 
				if (g_driver.ReadMSR(&info))
					dwTdist = (info.dwLow>>16) & 0x7F;

				result = (float)(dwTjmax - dwTdist);
			}
			else
				//core temperature
			{
				CThreadAffinityMask affinity(m_dwCpuCount, dwCpu);

				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				DWORD dwTdist = 0;

				info.dwIndex = 0x19C;	//IA32_THERM_STATUS 
				if (g_driver.ReadMSR(&info))
					dwTdist = (info.dwLow>>16) & 0x7F;

				result = (float)(dwTjmax - dwTdist);
			}
		}

		return result;
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
FLOAT CCPUInfo::GetAMDCpuTemperature(DWORD dwCpu)
{
	if (IsAMDCpu())
	{
		float result = FLT_MAX;

		if (dwCpu == 0xFFFFFFFF)
			//package temperature
		{
		}
		else
		{
			if (IsAMDZenArchitecture())
			{
				if (!dwCpu)
				{
					DWORD dwTempControl = GetAMDSMNRegister(0x59800 /*F17H_M01H_REPORTED_TEMP_CTRL_OFFSET*/);

					if ((dwTempControl & 0x80000) == 0x80000)
						result = ((dwTempControl>>21) & 0x7FF) / 8.0f - 49.0f;
					else
						result = ((dwTempControl>>21) & 0x7FF) / 8.0f;

					if (strstr(m_szCpuBrandString, "AMD Ryzen 7 2700X"))
						result = result - 10.0f;

					if (strstr(m_szCpuBrandString, "AMD Ryzen 7 1800X") ||
						strstr(m_szCpuBrandString, "AMD Ryzen 7 1700X") ||
						strstr(m_szCpuBrandString, "AMD Ryzen 5 1600X"))
						result = result - 20.0f;

					if (strstr(m_szCpuBrandString, "AMD Ryzen Threadripper 19") ||
						strstr(m_szCpuBrandString, "AMD Ryzen Threadripper 29"))
						result = result - 27.0f;
				}
			}
			else
			{
				if (m_dwMiscControlDevFn[dwCpu])
				{
					DWORD dwCpuFamily = GetCpuFamily();

					DWORD dwTempControl = 0;

					switch (dwCpuFamily)
					{
					case 0x10:
					case 0x11:
					case 0x12:
					case 0x14:
					case 0x15:
					case 0x16:
						dwTempControl = ReadBusDataUlong(0, m_dwMiscControlDevFn[dwCpu], 0xA4);
						break;
					}

					switch (dwCpuFamily)
					{
					case 0x10:
					case 0x11:
					case 0x12:
					case 0x14:
						result = ((dwTempControl>>21) & 0x7FF) / 8.0f;
						break;
					case 0x15:
						if ((dwTempControl & 0x30000) == 0x30000) 
							result = ((dwTempControl>>21) & 0x7FC) / 8.0f - 49.0f;
						else
							result = ((dwTempControl>>21) & 0x7FF) / 8.0f;
						break;
					case 0x16:
						if (((dwTempControl & 0x30000) == 0x30000) || 
							((dwTempControl & 0x80000) == 0x80000))
							result = ((dwTempControl>>21) & 0x7FF) / 8.0f - 49.0f;
						else
							result = ((dwTempControl>>21) & 0x7FF) / 8.0f;
						break;
					}
				}
			}
		}

		return result;
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetCcdCount()
{
	if (IsIntelCpu())
		return 0;

	if (IsAMDCpu())
	{
		if (IsAMDZenArchitecture())
		{
			if (m_dwCpuArch == CPU_ARCH_AMD_FAMILY_17H_MODEL_00)
				//not supported on Zen/Zen+
				return 0;

			return 8;
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsCcdTemperatureReportingSupported(DWORD dwCcd)
{
	if (g_driver.GetHandle() == INVALID_HANDLE_VALUE)
		return FALSE;

	if (IsIntelCpu())
		return FALSE;

	if (IsAMDCpu())
		return IsAMDCcdTemperatureReportingSupported(dwCcd);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsAMDCcdTemperatureReportingSupported(DWORD dwCcd)
{
	if (IsAMDCpu())
	{
		if (IsAMDZenArchitecture())
		{
			DWORD dwCcdTemp= GetAMDSMNRegister(0x59954 /*F17H_M70H_CCD_TEMP_OFFSET*/ + 4 * dwCcd);

			if (dwCcdTemp & 0x800 /*F17H_M70H_CCD_TEMP_VALID*/)
				return TRUE;
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetCcdTemperature(DWORD dwCcd)
{
	if (IsIntelCpu())
		return FLT_MAX;

	if (IsAMDCpu())
		return GetAMDCcdTemperature(dwCcd);

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetAMDCcdTemperature(DWORD dwCcd)
{
	if (IsAMDCpu())
	{
		if (IsAMDZenArchitecture())
		{
			DWORD dwCcdTemp= GetAMDSMNRegister(0x59954 /*F17H_M70H_CCD_TEMP_OFFSET*/ + 4 * dwCcd);

			if (dwCcdTemp & 0x800)
				return (dwCcdTemp & 0x7FF) / 8.0f - 49;
		}
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
DWORD CCPUInfo::GetAMDSMNRegister(DWORD dwRegister)
{
	HANDLE hMutex	= CreateMutex(NULL, FALSE, "Global\\Access_PCI");
	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);

	WriteBusDataUlong(0, 0, 0x60, dwRegister);
	DWORD dwResult = ReadBusDataUlong(0, 0, 0x64);

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	return dwResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsCpuPowerReportingSupported(DWORD dwCpu)
{
	if (g_driver.GetHandle() == INVALID_HANDLE_VALUE)
		return FALSE;

	if (IsIntelCpu())
		return IsIntelCpuPowerReportingSupported(dwCpu);

	if (IsAMDCpu())
		return IsAMDCpuPowerReportingSupported(dwCpu);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsIntelCpuPowerReportingSupported(DWORD dwCpu)
{
	if (IsIntelCpu())
	{
		if (dwCpu == 0xFFFFFFFF)
				//package power reporting is not supported if we failed to detect
				//power units during initialization
			return (m_fltPowerUnit != FLT_MAX);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsAMDCpuPowerReportingSupported(DWORD dwCpu)
{
	if (IsAMDCpu())
	{
		//core/package power reporting is not supported if we failed to detect
		//power units during initialization

		return (m_fltPowerUnit != FLT_MAX);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetCpuPower(DWORD dwCpu)
{
	if (IsIntelCpu())
		return GetIntelCpuPower(dwCpu);

	if (IsAMDCpu())
		return GetAMDCpuPower(dwCpu);

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetIntelCpuPower(DWORD dwCpu)
{
	if (IsIntelCpu())
	{
		if (dwCpu == 0xFFFFFFFF)
			//package power
		{
			if (m_fltPowerUnit != FLT_MAX)
				//package power reporting is not supported if we failed to detect
				//power units during initialization
			{
				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex = 0x611; //MSR_PKG_ENERY_STATUS;

				if (g_driver.ReadMSR(&info))
				{
					DWORD dwPowerConsumed	= info.dwLow;
					DWORD dwTimestamp		= GetTickCount();

					if (m_dwPackagePowerConsumedTimestamp)
					{
						if (dwTimestamp - m_dwPackagePowerConsumedTimestamp > 100)
						{
							m_fltPackagePower					= 1000 * m_fltPowerUnit * (dwPowerConsumed - m_dwPackagePowerConsumed) / (dwTimestamp - m_dwPackagePowerConsumedTimestamp);
							m_dwPackagePowerConsumed			= dwPowerConsumed;
							m_dwPackagePowerConsumedTimestamp	= dwTimestamp;
						}
					}
					else
					{
							m_fltPackagePower					= 0.0f;
							m_dwPackagePowerConsumed			= dwPowerConsumed;
							m_dwPackagePowerConsumedTimestamp	= dwTimestamp;
					}

				}
			}

			return m_fltPackagePower;
		}
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetAMDCpuPower(DWORD dwCpu)
{
	if (IsAMDCpu())
	{
		if (dwCpu == 0xFFFFFFFF)
			//package power
		{
			if (m_fltPowerUnit != FLT_MAX)
				//package power reporting is not supported if we failed to detect
				//power units during initialization
			{
				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex =  0xC001029B; //AMD_F17_MSR_PKG_ENERY_STATUS

				if (g_driver.ReadMSR(&info))
				{
					DWORD dwPowerConsumed	= info.dwLow;
					DWORD dwTimestamp		= GetTickCount();

					if (m_dwPackagePowerConsumedTimestamp)
					{
						if (dwTimestamp - m_dwPackagePowerConsumedTimestamp > 100)
						{
							m_fltPackagePower					= 1000 * m_fltPowerUnit * (dwPowerConsumed - m_dwPackagePowerConsumed) / (dwTimestamp - m_dwPackagePowerConsumedTimestamp);
							m_dwPackagePowerConsumed			= dwPowerConsumed;
							m_dwPackagePowerConsumedTimestamp	= dwTimestamp;
						}
					}
					else
					{
							m_fltPackagePower					= 0.0f;
							m_dwPackagePowerConsumed			= dwPowerConsumed;
							m_dwPackagePowerConsumedTimestamp	= dwTimestamp;
					}

				}
			}

			return m_fltPackagePower;
		}
		else
			//core power
		{
			if (m_fltPowerUnit != FLT_MAX)
				//package power reporting is not supported if we failed to detect
				//power units during initialization
			{
				CThreadAffinityMask affinity(m_dwCpuCount, dwCpu);

				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex =  0xC001029A; //AMD_F17_MSR_CORE_ENERY_STATUS

				if (g_driver.ReadMSR(&info))
				{
					DWORD dwPowerConsumed	= info.dwLow;
					DWORD dwTimestamp		= GetTickCount();

					if (m_dwCorePowerConsumedTimestamp[dwCpu])
					{
						if (dwTimestamp - m_dwCorePowerConsumedTimestamp[dwCpu] > 100)
						{
							m_fltCorePower[dwCpu]					= 1000 * m_fltPowerUnit * (dwPowerConsumed - m_dwCorePowerConsumed[dwCpu]) / (dwTimestamp - m_dwCorePowerConsumedTimestamp[dwCpu]);
							m_dwCorePowerConsumed[dwCpu]			= dwPowerConsumed;
							m_dwCorePowerConsumedTimestamp[dwCpu]	= dwTimestamp;
						}
					}
					else
					{
							m_fltCorePower[dwCpu]					= 0.0f;
							m_dwCorePowerConsumed[dwCpu]			= dwPowerConsumed;
							m_dwCorePowerConsumedTimestamp[dwCpu]	= dwTimestamp;
					}

				}
			}

			return m_fltCorePower[dwCpu];
		}
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsBusClockReportingSupported()
{
	if (g_driver.GetHandle() == INVALID_HANDLE_VALUE)
		return FALSE;

	return m_fltBusClock != FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetBusClock()
{
	return m_fltBusClock;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsCpuClockReportingSupported(DWORD dwCpu)
{
	if (g_driver.GetHandle() == INVALID_HANDLE_VALUE)
		return FALSE;

	if (IsIntelCpu())
		return IsIntelCpuClockReportingSupported(dwCpu);

	if (IsAMDCpu())
		return IsAMDCpuClockReportingSupported(dwCpu);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsIntelCpuClockReportingSupported(DWORD dwCpu)
{
	if (IsIntelCpu())
		return GetIntelCpuClock(dwCpu) != FLT_MAX;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsAMDCpuClockReportingSupported(DWORD dwCpu)
{
	if (IsAMDCpu())
		return GetAMDCpuClock(dwCpu) != FLT_MAX;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetCpuClock(DWORD dwCpu)
{
	if (IsIntelCpu())
		return GetIntelCpuClock(dwCpu);

	if (IsAMDCpu())
		return GetAMDCpuClock(dwCpu);

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetCpuClockAsync(DWORD dwCpu)
{
	return m_fltCpuClock[dwCpu];
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetMaxCpuClockAsync()
{
	float result = 0.0f;

	for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
	{
		float clock = GetCpuClockAsync(dwCpu);

		if (clock != FLT_MAX)
		{
			if (result < clock)
				result = clock;
		}
	}

	return result;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::CalcCpuClock(DWORD dwCpu)
{
	m_fltCpuClock[dwCpu] = GetCpuClock(dwCpu);

	return m_fltCpuClock[dwCpu];
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::CalcCpuClocks()
{
	CalcBusClock();

	for (DWORD dwCpu=0; dwCpu<m_dwCpuCount; dwCpu++)
		CalcCpuClock(dwCpu);
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetIntelCpuClock(DWORD dwCpu)
{
	if (IsIntelCpu())
	{
		float result = FLT_MAX;

		if (m_fltBusClock != FLT_MAX)
		{
			float fltClockMul = GetIntelClockMul(dwCpu);

			if (fltClockMul != FLT_MAX)
				result =  m_fltBusClock * fltClockMul;
		}

		return result;
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetAMDCpuClock(DWORD dwCpu)
{
	if (IsAMDCpu())
	{
		float result = FLT_MAX;

		if (m_fltBusClock != FLT_MAX)
		{
			float fltClockMul = GetAMDClockMul(dwCpu);

			if (fltClockMul != FLT_MAX)
				result =  m_fltBusClock * fltClockMul;
		}

		return result;
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetIntelBusClock()
{
	if (IsIntelCpu())
	{
		if (m_dwMailboxBusClock)
			//Skylake and newer CPUs can report BCLK via OC mailbox interface, use this path if we detected BCLK via OC mailbox during initialization
		{
			DWORD dwBusClock = 0;

			if (WriteCommandToOCMailbox(5 /*read BCLK command*/, 0, &dwBusClock))
			{
				//NOTE: OC Mailbox interface is not available if HVCI is enabled, abnormally low BCLK can be reported in this case so we filter result
				//and check if BCLK is in [50%-200%] range of nominal 

				if ((dwBusClock >= m_dwNominalBusClock / 2) &&
					(dwBusClock <= m_dwNominalBusClock * 2))
					m_dwMailboxBusClock = dwBusClock;
			}

			return (float)((m_dwMailboxBusClock + 500) / 1000);
				//BCLK values reported by mailbox can fluctuate, so we round it to 1MHz precision
		}

		if (m_qwTimestampClock || m_qwBaseClock)
		{		
			float fltTimestampClockMul = 0.0f;

			switch (m_dwCpuArch)
			{
			case CPU_ARCH_INTEL_NETBURST:
			case CPU_ARCH_INTEL_ATOM:
			case CPU_ARCH_INTEL_CORE:
				{
					MSR_INFO info;
					ZeroMemory(&info, sizeof(info));

					info.dwIndex = 0x198; //IA32_PERF_STATUS 

					if (g_driver.ReadMSR(&info))
						fltTimestampClockMul = ((info.dwHigh >> 8) & 0x1f) + 0.5f * ((info.dwHigh >> 14) & 1);
				}
				break;
			case CPU_ARCH_INTEL_NEHALEM:
			case CPU_ARCH_INTEL_SANDYBRIDGE:
			case CPU_ARCH_INTEL_IVYBRIDGE:
			case CPU_ARCH_INTEL_HASWELL: 
			case CPU_ARCH_INTEL_BROADWELL:
			case CPU_ARCH_INTEL_SILVERMONT:
			case CPU_ARCH_INTEL_SKYLAKE:
			case CPU_ARCH_INTEL_AIRMONT:
			case CPU_ARCH_INTEL_KABYLAKE:
			case CPU_ARCH_INTEL_GOLDMONT:
			case CPU_ARCH_INTEL_GOLDMONTPLUS:
			case CPU_ARCH_INTEL_CANNONLAKE:
			case CPU_ARCH_INTEL_ICELAKE:
			case CPU_ARCH_INTEL_COMETLAKE:
			case CPU_ARCH_INTEL_TREMONT:
			case CPU_ARCH_INTEL_TIGERLAKE:
			case CPU_ARCH_INTEL_ROCKETLAKE:
			case CPU_ARCH_INTEL_ALDERLAKE:
			case CPU_ARCH_INTEL_RAPTORLAKE:
				{
					MSR_INFO info;
					ZeroMemory(&info, sizeof(info));

					info.dwIndex = 0xCE; //MSR_PLATFORM_INFO

					if (g_driver.ReadMSR(&info))
						fltTimestampClockMul = (float)((info.dwLow>>8) & 0xff);
				}
				break;
			}

			if (fltTimestampClockMul != 0.0f)
			{
				if (m_qwBaseClock)
					//TSC clock is not affected by BCLK overclocking on modern Intel CPUs, so we additionally estimate base clock via MPERF 
					//performance counter and can use it instead of TSC clock for BCLK calculation
					return (m_qwBaseClock / fltTimestampClockMul) / 1000.0f;

				return (m_qwTimestampClock / fltTimestampClockMul) / 1000.0f;
			}
		}
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetAMDBusClock()
{
	if (IsAMDCpu())
	{
		if (m_qwTimestampClock)
		{		
			float fltTimestampClockMul = 0.0f;

			if (IsAMDZenArchitecture())
			{
				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex = 0xC0010064;
				if (g_driver.ReadMSR(&info))
				{
					if ((info.dwLow>>8) & 0x3F)
						fltTimestampClockMul = 2.0f * (info.dwLow & 0xFF) / ((info.dwLow>>8) & 0x3F);
				}
			}

			if (fltTimestampClockMul != 0.0f)
				return (m_qwTimestampClock / fltTimestampClockMul) / 1000.0f;
		}
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetIntelClockMul(DWORD dwCpu)
{
	if (IsIntelCpu())
	{
		float result = FLT_MAX;

		CThreadAffinityMask affinity(m_dwCpuCount, dwCpu);

		switch (m_dwCpuArch)
		{
		case CPU_ARCH_INTEL_NEHALEM:
			{
				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex = 0x198; //IA32_PERF_STATUS 

				if (g_driver.ReadMSR(&info))
					result = (float)(info.dwLow & 0xff);
			}
			break;
		case CPU_ARCH_INTEL_SANDYBRIDGE:
		case CPU_ARCH_INTEL_IVYBRIDGE:
		case CPU_ARCH_INTEL_HASWELL: 
		case CPU_ARCH_INTEL_BROADWELL:
		case CPU_ARCH_INTEL_SILVERMONT:
		case CPU_ARCH_INTEL_SKYLAKE:
		case CPU_ARCH_INTEL_KABYLAKE:
		case CPU_ARCH_INTEL_GOLDMONT:
		case CPU_ARCH_INTEL_GOLDMONTPLUS:
		case CPU_ARCH_INTEL_CANNONLAKE:
		case CPU_ARCH_INTEL_ICELAKE:
		case CPU_ARCH_INTEL_COMETLAKE:
		case CPU_ARCH_INTEL_TREMONT:
		case CPU_ARCH_INTEL_TIGERLAKE:
		case CPU_ARCH_INTEL_ROCKETLAKE:
		case CPU_ARCH_INTEL_ALDERLAKE:
		case CPU_ARCH_INTEL_RAPTORLAKE:
			{
				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex = 0x198; //IA32_PERF_STATUS 

				if (g_driver.ReadMSR(&info))
					result = (float)((info.dwLow >> 8) & 0xff);
			}
			break;
		default:
			{
				MSR_INFO info;
				ZeroMemory(&info, sizeof(info));

				info.dwIndex = 0x198; //IA32_PERF_STATUS 

				if (g_driver.ReadMSR(&info))
					result = ((info.dwLow >> 8) & 0x1f) + 0.5f * ((info.dwLow >> 14) & 1);
			}
			break;
		}

		return result;
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetAMDClockMul(DWORD dwCpu)
{
	if (IsAMDCpu())
	{
		float result = FLT_MAX;

		CThreadAffinityMask affinity(m_dwCpuCount, dwCpu);

		if (IsAMDZenArchitecture())
		{
			MSR_INFO info;
			ZeroMemory(&info, sizeof(info));

			info.dwIndex = 0x0C0010293;
			if (g_driver.ReadMSR(&info))
			{
				if ((info.dwLow >> 8) & 0x3F)
					result = 2.0f * (info.dwLow & 0xff) / ((info.dwLow>>8) & 0x3f);
			}
		}

		return result;
	}

	return FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::IsEffectiveCpuClockReportingSupported(DWORD dwCpu, BOOL bUnhalted)
{
	if (g_driver.GetHandle() == INVALID_HANDLE_VALUE)
		return FALSE;

	GetEffectiveCpuClock(dwCpu, bUnhalted);
		//NOTE: GetEffectiveCpuClock uses performance counter deltas from the previous function call, so ensure that it was called at least once before checking support

	return GetEffectiveCpuClock(dwCpu, bUnhalted) != FLT_MAX;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::QueryMPerf(unsigned __int64* lpMPerf, unsigned __int64* lpAPerf)
{
	if (m_bMPerfSupported)
	{
		MSR_INFO info;
		ZeroMemory(&info, sizeof(info));

		if (IsIntelCpu())
			//read architectural IA32_MPERF/IA32_APERF on Intel CPUs
		{
			if (lpMPerf)
			{
				info.dwIndex = 0xE7;	// IA32_MPERF 
				if (g_driver.ReadMSR(&info))
					*lpMPerf = info.dwLow | ((unsigned __int64)info.dwHigh<<32);
			}

			if (lpAPerf)
			{
				info.dwIndex = 0xE8;	// IA32_APERF 
				if (g_driver.ReadMSR(&info)) 
					*lpAPerf = info.dwLow | ((unsigned __int64)info.dwHigh<<32);
			}

			return TRUE;
		}

		if (IsAMDCpu())
		{
			if (IsAMDZenArchitecture())
				//read nonarchitectural read only MPERF/APERF on AMD Ryzen CPUs
			{
				if (lpMPerf)
				{
					info.dwIndex = 0xC00000E7;
					if (g_driver.ReadMSR(&info))
						*lpMPerf = info.dwLow | ((unsigned __int64)info.dwHigh<<32);
				}

				if (lpAPerf)
				{
					info.dwIndex = 0xC00000E8;
					if (g_driver.ReadMSR(&info))
						*lpAPerf = info.dwLow | ((unsigned __int64)info.dwHigh<<32);
				}

				return TRUE;
			}
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
float CCPUInfo::GetEffectiveCpuClock(DWORD dwCpu, BOOL bUnhalted)
{
	float result = FLT_MAX;

	CThreadAffinityMask affinity(m_dwCpuCount, dwCpu);

	unsigned __int64 qwMPerf = 0;
	unsigned __int64 qwAPerf = 0;

	QueryMPerf(&qwMPerf, &qwAPerf);
		//query MPerf/APerf counters

	if (bUnhalted)
		//effective clock estimation based on TSC clock and APERF/MPERF deltas which only increment in C0
	{
		if (m_qwTimestampClock && m_qwMPerf[dwCpu] && m_qwAPerf[dwCpu])
			result = m_qwTimestampClock * (qwAPerf - m_qwAPerf[dwCpu]) / (qwMPerf - m_qwMPerf[dwCpu]) / 1000.0f;	
	}
	else
		//effective clock estimation based on APERF delta and absolute timings so it takes sleep states into account
	{
		LARGE_INTEGER pf;

		if (QueryPerformanceFrequency(&pf))
		{
			LARGE_INTEGER pc;

			QueryPerformanceCounter(&pc);

			if (m_pcPerfTimestampCounter[dwCpu].QuadPart)
			{
				double time = 1000000.0f * (pc.QuadPart - m_pcPerfTimestampCounter[dwCpu].QuadPart) / pf.QuadPart;

				if (m_qwAPerf[dwCpu])
					result = (float)((qwAPerf - m_qwAPerf[dwCpu]) / time);
			}

			m_pcPerfTimestampCounter[dwCpu].QuadPart = pc.QuadPart;
		}
	}
	
	m_qwMPerf[dwCpu] = qwMPerf;
	m_qwAPerf[dwCpu] = qwAPerf;

	return result;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::WaitForResponseFromOCMailbox(DWORD* lpResponse)
{
	MSR_INFO info;
	ZeroMemory(&info, sizeof(info));

	info.dwIndex = 0x150;	//MSR_OC_MAILBOX	

	for (int iRetry=0; iRetry<100; iRetry++)
		//wait for mailbox resonse during 1 millisecond (100 retry attempts with 10 microseconds delays)
	{
		if (!g_driver.ReadMSR(&info))
			return FALSE;

		if (!(info.dwHigh & 0x80000000))
			//busy bit is not set, so mailbox finished processing a command
		{
			if (!(info.dwHigh & 0xFF))
				//check error code
			{
				if (lpResponse)
					*lpResponse = info.dwLow;

				return TRUE;
			}
		}

		Delay1us(10);
			//10 microseconds delay
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CCPUInfo::WriteCommandToOCMailbox(DWORD dwCommand, DWORD dwData, DWORD* lpResponse)
{
	if (WaitForResponseFromOCMailbox())
		//ensure that mailbox is not processing a command now
	{
		MSR_INFO info;
		ZeroMemory(&info, sizeof(info));

		info.dwIndex	= 0x150;					//MSR_OC_MAILBOX	
		info.dwHigh		= dwCommand | 0x80000000;	//set busy bit
		info.dwLow		= dwData;

		if (g_driver.WriteMSR(&info))
			//write command and wait for response
			return WaitForResponseFromOCMailbox(lpResponse);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CCPUInfo::SetCpuUsageCalc(DWORD dwCalc)
{
	m_dwCpuUsageCalc = dwCalc;
}
//////////////////////////////////////////////////////////////////////


