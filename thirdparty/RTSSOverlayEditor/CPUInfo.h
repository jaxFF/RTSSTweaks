// CPUInfo.h: interface for the CCPUInfo class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "CPUTopology.h"
#include "TimestampedDataCache.h"
#include "PerfCounterObjects.h"
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#define SystemProcessorPerformanceInformation							0x08
#define SystemProcessorIdleInformation									0x2A
/////////////////////////////////////////////////////////////////////////////
typedef HRESULT (WINAPI *NTQUERYSYSTEMINFORMATION)(UINT, PVOID, ULONG, PULONG);
/////////////////////////////////////////////////////////////////////////////
typedef struct SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION 
{
    LARGE_INTEGER	IdleTime;
    LARGE_INTEGER	KernelTime;
    LARGE_INTEGER	UserTime;
    LARGE_INTEGER	Reserved1[2];
    ULONG			Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;
/////////////////////////////////////////////////////////////////////////////
typedef struct SYSTEM_PROCESSOR_IDLE_INFORMATION
{
	ULONGLONG		IdleTime;
	ULONGLONG		C1Time;
	ULONGLONG		C2Time;
	ULONGLONG		C3Time;
	ULONG			C1Transitions;
	ULONG			C2Transitions;
	ULONG			C3Transitions;
	ULONG			Padding;
} SYSTEM_PROCESSOR_IDLE_INFORMATION;
/////////////////////////////////////////////////////////////////////////////
typedef struct PCI_DEVICE_LOCATION
{
	DWORD	dwBus;
	DWORD	dwDevFn;
} PCI_DEVICE_LOCATION, *LPPCI_DEVICE_LOCATION;
//////////////////////////////////////////////////////////////////////
#define MAX_CPU 256
//////////////////////////////////////////////////////////////////////
#define CPU_ARCH_UNKNOWN									0x00000000
#define CPU_ARCH_INTEL_NETBURST								0x80860001		
#define CPU_ARCH_INTEL_CORE									0x80860002
#define CPU_ARCH_INTEL_ATOM									0x80860003
#define CPU_ARCH_INTEL_NEHALEM								0x80860004
#define CPU_ARCH_INTEL_SANDYBRIDGE							0x80860005
#define CPU_ARCH_INTEL_IVYBRIDGE							0x80860006
#define CPU_ARCH_INTEL_HASWELL								0x80860007 
#define CPU_ARCH_INTEL_BROADWELL							0x80860008
#define CPU_ARCH_INTEL_SILVERMONT							0x80860009
#define CPU_ARCH_INTEL_SKYLAKE								0x8086000A
#define CPU_ARCH_INTEL_AIRMONT								0x8086000B
#define CPU_ARCH_INTEL_KABYLAKE								0x8086000C
#define CPU_ARCH_INTEL_GOLDMONT								0x8086000D
#define CPU_ARCH_INTEL_GOLDMONTPLUS							0x8086000E
#define CPU_ARCH_INTEL_CANNONLAKE							0x8086000F
#define CPU_ARCH_INTEL_ICELAKE								0x80860010
#define CPU_ARCH_INTEL_COMETLAKE							0x80860011
#define CPU_ARCH_INTEL_TREMONT								0x80860012
#define CPU_ARCH_INTEL_TIGERLAKE							0x80860013
#define CPU_ARCH_INTEL_ROCKETLAKE							0x80860014
#define CPU_ARCH_INTEL_ALDERLAKE							0x80860015
#define CPU_ARCH_INTEL_RAPTORLAKE							0x80860016

#define CPU_ARCH_AMD_FAMILY_10H								0x10220001		
#define CPU_ARCH_AMD_FAMILY_11H								0x10220002		
#define CPU_ARCH_AMD_FAMILY_12H								0x10220003		
#define CPU_ARCH_AMD_FAMILY_14H								0x10220004		
#define CPU_ARCH_AMD_FAMILY_15H_MODEL_00					0x10220005		
#define CPU_ARCH_AMD_FAMILY_15H_MODEL_10					0x10220006		
#define CPU_ARCH_AMD_FAMILY_15H_MODEL_30					0x10220007		
#define CPU_ARCH_AMD_FAMILY_16H_MODEL_00					0x10220008		
#define CPU_ARCH_AMD_FAMILY_16H_MODEL_30					0x10220009		
#define CPU_ARCH_AMD_FAMILY_17H_MODEL_00					0x1022000A		
#define CPU_ARCH_AMD_FAMILY_17H_MODEL_10					0x1022000B		
#define CPU_ARCH_AMD_FAMILY_17H_MODEL_30					0x1022000C		
#define CPU_ARCH_AMD_FAMILY_17H_MODEL_60					0x1022000D		
#define CPU_ARCH_AMD_FAMILY_17H_MODEL_70					0x1022000E		
#define CPU_ARCH_AMD_FAMILY_19H_MODEL_20					0x1022000F		
#define CPU_ARCH_AMD_FAMILY_19H_MODEL_40					0x10220010		
#define CPU_ARCH_AMD_FAMILY_19H_MODEL_50					0x10220011	
#define CPU_ARCH_AMD_FAMILY_19H_MODEL_60					0x10220012	
//////////////////////////////////////////////////////////////////////
class CCPUInfo
{
public:
	CCPUInfo();
	virtual ~CCPUInfo();

	void	SetCpuUsageCalc(DWORD dwCalc);

	void	SetTimestamp(DWORD dwTimestamp);
	void	Init();

	BOOL	IsIntelCpu();
	BOOL	IsAMDCpu();

	DWORD	GetCpuCount();
	void	CalcCpuUsage_SystemProcessorPerformanceInformation();
	void	CalcCpuUsage_SystemProcessorIdleInformation();
	void	CalcCpuUsage_ProcessorUtility();
	void	CalcCpuUsage_ProcessorUtilityNormalized();
	float	GetCpuUsage(DWORD dwCpu);
	float	GetTotalCpuUsage();
	float	GetMaxCpuUsage();

	LPCSTR	GetCpuString();
	LPCSTR	GetCpuBrandString();
	DWORD	GetCpuFamily();
	DWORD	GetCpuModel();
	DWORD	GetCpuStepping();

	DWORD	GetPckg(DWORD dwCpu);
	DWORD	GetCore(DWORD dwCpu);

	DWORD	GetTjmax(DWORD dwCpu);

	BOOL	IsCpuTemperatureReportingSupported(DWORD dwCpu);
	float	GetCpuTemperature(DWORD dwCpu);
	float	GetMaxCpuTemperature();

	DWORD	GetCcdCount();
	BOOL	IsCcdTemperatureReportingSupported(DWORD dwCcd);
	float	GetCcdTemperature(DWORD dwCcd);

	BOOL	IsCpuPowerReportingSupported(DWORD dwCpu);
	float	GetCpuPower(DWORD dwCpu);

	BOOL	IsBusClockReportingSupported();
	float	GetBusClock();

	BOOL	IsCpuClockReportingSupported(DWORD dwCpu);
	float	GetCpuClock(DWORD dwCpu);
	float	GetCpuClockAsync(DWORD dwCpu);
	float	GetMaxCpuClockAsync();

	void	CalcCpuClocks();
	float	CalcCpuClock(DWORD dwCpu);

	BOOL	IsEffectiveCpuClockReportingSupported(DWORD dwCpu, BOOL bUnhalted);
	float	GetEffectiveCpuClock(DWORD dwCpu, BOOL bUnhalted);
	float	GetEffectiveCpuPerf(DWORD dwCpu);

protected:
	void	InitIntelCpu();
	BOOL	IsIntelCpuTemperatureReportingSupported(DWORD dwCpu);
	float	GetIntelCpuTemperature(DWORD dwCpu);
	BOOL	IsIntelCpuPowerReportingSupported(DWORD dwCpu);
	float	GetIntelCpuPower(DWORD dwCpu);
	BOOL	IsIntelCpuClockReportingSupported(DWORD dwCpu);
	float	GetIntelCpuClock(DWORD dwCpu);
	float	GetIntelBusClock();
	float	GetIntelClockMul(DWORD dwCpu);

	void	InitAMDCpu();
	BOOL	IsAMDCpuTemperatureReportingSupported(DWORD dwCpu);
	float	GetAMDCpuTemperature(DWORD dwCpu);
	BOOL	IsAMDCpuPowerReportingSupported(DWORD dwCpu);
	float	GetAMDCpuPower(DWORD dwCpu);
	BOOL	IsAMDCpuClockReportingSupported(DWORD dwCpu);
	float	GetAMDCpuClock(DWORD dwCpu);
	float	GetAMDBusClock();
	float	GetAMDClockMul(DWORD dwCpu);

	DWORD	GetAMDCcdCount();
	BOOL	IsAMDCcdTemperatureReportingSupported(DWORD dwCcd);
	float	GetAMDCcdTemperature(DWORD dwCcd);

	void	Delay1us(DWORD dwNumCycles);
	void	Delay1ms(DWORD dwNumCycles);

	BOOL	QueryMPerf(unsigned __int64* lpMPerf, unsigned __int64* lpAPerf = NULL);

	unsigned __int64 CalcTimestampClock(DWORD dwPeriod = 1000);
	unsigned __int64 CalcBaseClock(DWORD dwPeriod = 1000);
	float	CalcBusClock();

	DWORD	ReadBusDataUlong(DWORD bus, DWORD devFn, DWORD offset);
	void	WriteBusDataUlong(DWORD bus, DWORD devFn, DWORD offset, DWORD data);

	BOOL	WaitForResponseFromOCMailbox(DWORD* lpResponse = NULL);
	BOOL	WriteCommandToOCMailbox(DWORD dwCommand, DWORD dwData, DWORD* lpResponse = NULL);

	BOOL	IsAMDZenArchitecture();
	DWORD	GetAMDSMNRegister(DWORD dwRegister);

	DWORD	GetNearestCrystalClock(DWORD dwClock);

	void	AddPerfCounter(LPCSTR lpObjectName, LPCSTR lpInstanceName, LPCSTR lpCounterName, PDH_HCOUNTER* lpCounter);

	//HAL timestamp

	DWORD	m_dwTimestamp;

	//CPU count

	DWORD	m_dwCpuCount;
	NTQUERYSYSTEMINFORMATION m_lpNtQuerySystemInformation;

	//CPU model related variables

	DWORD	m_dwCpuStepping;
	DWORD	m_dwCpuModel;
	DWORD	m_dwCpuModelExt;
	DWORD	m_dwCpuFamily;
	DWORD	m_dwCpuFamilyExt;
	char	m_szCpuString[0x20];
	char	m_szCpuBrandString[0x40];
	DWORD	m_dwCpuArch;

	//CPU usage related variables

	DWORD				m_dwTickCount[MAX_CPU];
	LARGE_INTEGER		m_idleTime[MAX_CPU];
	float				m_fltCpuUsage[MAX_CPU];

	DWORD				m_dwCpuUsageCalc;

	CPerfCounterObjects	m_objects;
	PDH_HQUERY			m_hQuery;
	PDH_HCOUNTER		m_hCounterCpuUtil[MAX_CPU];
	PDH_HCOUNTER		m_hCounterCpuPerf[MAX_CPU];

	//estimated timestamp, base and bus clock frequency related variables

	LARGE_INTEGER		m_pcTimestampCounter;
	unsigned __int64	m_qwTimestampCounter;
	unsigned __int64	m_qwTimestampClock;
	LARGE_INTEGER		m_pcBaseClockCounter;
	unsigned __int64	m_qwBaseClockCounter;
	unsigned __int64	m_qwBaseClock;
	float				m_fltBusClock;

	//CPU clock related variables

	float				m_fltCpuClock[MAX_CPU];

	//effective CPU clock related variables

	BOOL				m_bMPerfSupported;
	LARGE_INTEGER		m_pcPerfTimestampCounter[MAX_CPU];
	unsigned __int64	m_qwMPerf[MAX_CPU];
	unsigned __int64	m_qwAPerf[MAX_CPU];

	//timestamp crystal clock and nominal clock related variables

	DWORD				m_dwTimestampCrystalClock;
	DWORD				m_dwTimestampCrystalClockMul;
	DWORD				m_dwTimestampCrystalClockDiv;

	DWORD				m_dwNominalBaseClock;
	DWORD				m_dwNominalMaxClock;
	DWORD				m_dwNominalBusClock;

	DWORD				m_dwMailboxBusClock;

	//CPU topology

	CCPUTopology		m_cpuTopology;

	//timestamped cache for temperatures 

	CTimestampedDataCache m_cpuTemperatureCache;

	//Intel/AMD specific variables

	DWORD				m_dwTjmax[MAX_CPU];
	DWORD				m_dwPackageTemperatureMonitoring;

	FLOAT				m_fltPowerUnit;
	FLOAT				m_fltPackagePower;
	DWORD				m_dwPackagePowerConsumed;
	DWORD				m_dwPackagePowerConsumedTimestamp;

	FLOAT				m_fltCorePower[MAX_CPU];
	DWORD				m_dwCorePowerConsumed[MAX_CPU];
	DWORD				m_dwCorePowerConsumedTimestamp[MAX_CPU];

	DWORD				m_dwMiscControlDevFn[MAX_CPU];
};
/////////////////////////////////////////////////////////////////////////////
