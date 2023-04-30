// HAL.h: interface for the CHAL class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include <afxtempl.h>

#include "HALImplementation.h"
#include "CPUInfo.h"
#include "SMART.h"
#include "CPUTopology.h"
#include "PingThread.h"
#include "D3DKMTWrapper.h"
#include "NVAPIWrapper.h"
#include "ATIADLWrapper.h"
#include "IGCLWrapper.h"
#include "HALDataSourcesList.h"
#include "HALPollingThread.h"
//////////////////////////////////////////////////////////////////////
class CHAL : public CList<CHALImplementation*, CHALImplementation*>
{
public:
	CHAL();
	virtual ~CHAL();

	//HAL initialization/uninitialization

	void	Init(DWORD dwDriver);
	void	Uninit();

	void	EnableSmart(BOOL bEnable);
		//enables SMART based sensors monitoring
	void	SetSmartPollingInterval(DWORD dwPollingInverval);
		//sets min polling interval for SMART based sensors (HDD temperatures)

	void	SetCpuUsageCalc(DWORD dwCalc);
		//sets CPU usage calculation mode:

		//0 - native Win32 API mode based on inverted idle time reported by NtQuerySystemInformation(SystemProcessorPerformanceInformation)
		//	  NOTE: idle time reporting in NtQuerySystemInformation(SystemProcessorPerformanceInformation) seem to be broken on initial 22H2 builds
		//1 - native Win32 API mode based on inverted idle time reported by NtQuerySystemInformation(SystemProcessorIdleInformation)
		//2 - based on "\Processor Information\% Processor Utility" performance counter
		//3 - based on "\Processor Information\% Processor Utility" performance counter (normalized with "\Processor Information\% Processor Performance")

	void	SetTimestamp(DWORD dwTimestamp);
		//sets hardware polling timestamp, HAL implementations may use it to precache some hardware sensor data if we request it more than once
		//on the same hardware polling iteration

	//GPU monitoring

	DWORD	GetGpuCount();
		//returns number of physical GPUs
	CHALImplementation* GetGpu(DWORD dwGpu);
		//returns GPU implementation
	CHALImplementation* FindGpu(DWORD dwBus, DWORD dwDev, DWORD dwFn);
		//finds GPU implementation by location
	
	//CPU monitoring

	DWORD	GetCpuCount();
		//returns number of logical CPUs
	BOOL	IsIntelCpu();
		//returns TRUE if CPU is Intel
	BOOL	IsAMDCpu();
		//returns TRUE if CPU is AMD
	LPCSTR	GetCpuString();
		//returns CPU vendor string
	LPCSTR	GetCpuBrandString();
		//returns CPU brand string
	DWORD	GetCpuFamily();
		//returns CPU family
	DWORD	GetCpuModel();
		//returns CPU model
	DWORD	GetCpuStepping();
		//returns CPU stepping
	float	GetCpuUsage(DWORD dwCpu);
		//calculates and returns current CPU usage for specified logical CPU
	float	GetTotalCpuUsage();
		//calculates and returns total CPU usage
	float	GetMaxCpuUsage();
		//calculates and returns max CPU usage across all cores
	BOOL IsCpuTemperatureReportingSupported(DWORD dwCpu);
		//returns TRUE is CPU temperature reporting is supported
	BOOL IsCpuPowerReportingSupported(DWORD dwCpu);
		//returns TRUE is CPU power reporting is supported
	BOOL IsCpuClockReportingSupported(DWORD dwCpu);
		//returns TRUE is CPU clock reporting is supported
	float GetCpuTemperature(DWORD dwCpu);
		//returns CPU temperature
	float GetMaxCpuTemperature();
		//returns maximum CPU temperature
	float GetCpuPower(DWORD dwCpu);
		//returns CPU power
	float GetCpuClock(DWORD dwCpu);
		//returns CPU clock
	float GetCpuClockAsync(DWORD dwCpu);
		//returns CPU clock asynchronously
	float GetMaxCpuClockAsync();
		//returns maximum CPU clock asynchronously
	BOOL IsCalcCpuClocksRequired();
		//returns TRUE if at least one HALDATASOURCE_ID_CPU_CLOCK source in the list is dirty
	BOOL IsCpuBusClockReportingSupported();
		//returns TRUE if CPU bus clock reporting is supported
	float GetCpuBusClock();
		//returns CPU bus clock
	BOOL IsEffectiveCpuClockReportingSupported(DWORD dwCpu, BOOL bUnhalted);
		//returns TRUE is effective CPU clock reporting is supported
	float GetEffectiveCpuClock(DWORD dwCpu, BOOL bUnhalted);
		//returns effective CPU clock
		//NOTE: bUnhalted can be set to TRUE to estimate clock frequency changes in active C0 state only, 
		//otherwise reported clock can be much lower due to entering CPU sleep states

	DWORD GetCcdCount();
		//returns CCD count
	BOOL IsCcdTemperatureReportingSupported(DWORD dwCcd);
		//returns TRUE if CCD temperature reporting is supported
	float GetCcdTemperature(DWORD dwCcd);
		//returns CCD temperature

	//RAM monitoring

	ULONGLONG GetTotalRAM();
		//returns total installed RAM in kilobytes
	ULONGLONG GetPhysicalMemoryUsage();
		//returns current physical memory usage in megabytes
	float GetPhysicalMemoryUsagePercent();
		//returns current physical memory usage in percents
	ULONGLONG GetCommitCharge();
		//returns current commit charge in megabytes
	DWORD GetProcessMemoryUsage();
		//returns foreground process memory usage in megabytes
	float GetProcessMemoryUsagePercent();
		//returns foreground process memory usage in percents

	//Ping monitoring

	void	InitPingThread(LPCSTR lpAddr);
		//initializes asynchronous ping monitoring thread
	void	UninitPingThread();
		//uninitializes asynchronous ping monitoring thread
	CString	GetPingAddr();	
		//returns ping URL or IP address
	float	GetPing();	
		//returns last polled ping in milliseconds

	//SMART monitoring

	BOOL	IsHddTemperatureReportingSupported(DWORD dwDrive, DWORD dwSensor);
		//returns TRUE if HDD temperature reporting is supported
	float	GetHddTemperature(DWORD dwDrive, DWORD dwSensor);
		//returns current HDD temeprature

	//Timers

	DWORD	GetLifetime();
		//returns HAL object lifetime in milliseconds

	//HAL data sources 

	CHALDataSourcesList* GetDataSourcesList();
		//returns list of HAL data sources
	CHALDataSource* FindSource(LPCSTR lpSource);
		//finds HAL data source by name
	void UpdateSources();
		//syncronically polls and updates HAL data sources
	void UpdateSourcesAsync();
		//asynchronically polls and updates HAL data sources

	void InitPollingThread();
		//initializes asynchronous HAL data source polling thread
	void UninitPollingThread();
		//uninitializes asynchronous HAL data soruce polling thread
	float GetPollingTime();
		//returns last HAL polling time in milliseconds

	//HAL macro list

	CStringList* GetMacroList();
		//returns HAL macro list (preinitialized macro variables containing CPU, RAM and GPU specs)

protected:
	CString PackCPUString(LPCSTR lpString, BOOL bStripClockFrequency);
	CString PackGPUString(LPCSTR lpString);
	CString Trim(LPCSTR lpString);

	BOOL			m_bInitialized;

	CCPUInfo		m_cpuInfo;
	CSMART			m_smartInfo;

	ULONGLONG		m_qwTotalRAM;

	CPingThread*	m_lpPingThread;
	CString			m_strPingAddr;

	CD3DKMTWrapper	m_d3dkmtWrapper;
	CNVAPIWrapper	m_nvapiWrapper;
	CATIADLWrapper	m_atiadlWrapper;
	CIGCLWrapper	m_igclWrapper;

	CHALDataSourcesList m_dataSourcesList;

	CHALPollingThread* m_lpPollingThread;
	float			m_fltPollingTime;

	CStringList		m_strMacroList;

	DWORD			m_dwCreationTimestamp;
};
//////////////////////////////////////////////////////////////////////
