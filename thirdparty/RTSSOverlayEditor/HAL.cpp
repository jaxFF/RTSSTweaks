// HAL.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HAL.h"

#include "OverlayEditor.h"
#include "HALImplementationNVAPI.h"
#include "HALImplementationATIADL.h"
#include "HALImplementationIGCL.h"
#include "HALImplementationUnified.h"
#include "RTSSsharedMemoryInterface.h"
#include "Log.h"

#include <intrin.h>
#include <float.h>
#include <io.h>
//////////////////////////////////////////////////////////////////////
#define ALIGN(X,N) (X+N-1)&~(N-1)
//////////////////////////////////////////////////////////////////////
CHAL::CHAL()
{
	m_bInitialized					= FALSE;

	m_qwTotalRAM					= 0;

	m_lpPingThread					= NULL;
	m_strPingAddr					= "";

	m_lpPollingThread				= NULL;
	m_fltPollingTime				= 0;

	m_dwCreationTimestamp			= GetTickCount();
}
//////////////////////////////////////////////////////////////////////
CHAL::~CHAL()
{
	Uninit();
}
//////////////////////////////////////////////////////////////////////
void CHAL::Uninit()
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CHALImplementation* lpImplementation = GetNext(pos);

		delete lpImplementation;
	}

	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
void CHAL::Init(DWORD dwDriver)
{
	if (!m_bInitialized)
	{
		if (!dwDriver)
		{
			APPEND_LOG("Driver is disabled");
		}
		else
		{
			g_driver.SetImplementation(dwDriver);
				//select full RTCore or RTCoreMini driver implementation

			HRESULT errCode = g_driver.Load(g_hModule);

			if (SUCCEEDED(errCode))
			{
				APPEND_LOG1("%s driver loaded", g_driver.GetName());
			}
			else
			{
				APPEND_LOG2("Failed to load %s driver, error code %X", g_driver.GetName(), errCode);
			}
		}

		//init CPU monitoring/identification related objects

		m_cpuInfo.Init();

		//init HDD monitoring related objects

		if (m_smartInfo.IsEnabled())
		{
			APPEND_LOG1("SMART polling interval is set to %dms", m_smartInfo.GetPollingInterval());

			m_smartInfo.Init();
		}
		else
		{
			APPEND_LOG("SMART polling is disabled");
		}

		//init total RAM

		m_qwTotalRAM	= GetTotalRAM();

		//init GPU API wrappers

		m_d3dkmtWrapper.Init();
		m_nvapiWrapper.Init();
		m_atiadlWrapper.Init();
		m_igclWrapper.Init(&m_d3dkmtWrapper);

		//populate GPU implementations list

		APPEND_LOG("Populating HAL GPU implementations");

		for (DWORD dwGpu=0; dwGpu<m_nvapiWrapper.GetGpuCount(); dwGpu++)
			//popluate NVAPI implementations list
		{
			CHALImplementationNVAPI* lpImplementation = new CHALImplementationNVAPI;

			lpImplementation->SetD3DKMTWrapper(&m_d3dkmtWrapper);
			lpImplementation->SetNVAPIWrapper(&m_nvapiWrapper);
			lpImplementation->SetGpu(dwGpu);
			lpImplementation->BindD3DKMTDeviceDesc();

			DWORD dwBus;
			DWORD dwDev;
			DWORD dwFn;

			lpImplementation->GetLocation(&dwBus, &dwDev, &dwFn);

			APPEND_LOG4("GPU%d : NVAPI implementation on bus %d, dev %d, fn %d", GetCount(), dwBus, dwDev, dwFn);

			AddTail(lpImplementation);
		}

		for (DWORD dwGpu=0; dwGpu<m_atiadlWrapper.GetGpuCount(); dwGpu++)
			//popluate ATIADL implementations list
		{
			CHALImplementationATIADL* lpImplementation = new CHALImplementationATIADL;

			lpImplementation->SetD3DKMTWrapper(&m_d3dkmtWrapper);
			lpImplementation->SetATIADLWrapper(&m_atiadlWrapper);
			lpImplementation->SetGpu(dwGpu);
			lpImplementation->BindD3DKMTDeviceDesc();

			DWORD dwBus;
			DWORD dwDev;
			DWORD dwFn;

			lpImplementation->GetLocation(&dwBus, &dwDev, &dwFn);

			APPEND_LOG4("GPU%d : ATIADL implementation on bus %d, dev %d, fn %d", GetCount(), dwBus, dwDev, dwFn);

			AddTail(lpImplementation);
		}

		for (DWORD dwGpu=0; dwGpu<m_igclWrapper.GetGpuCount(); dwGpu++)
			//popluate IGCL implementations list
		{
			CHALImplementationIGCL* lpImplementation = new CHALImplementationIGCL;

			lpImplementation->SetD3DKMTWrapper(&m_d3dkmtWrapper);
			lpImplementation->SetIGCLWrapper(&m_igclWrapper);
			lpImplementation->SetGpu(dwGpu);
			lpImplementation->BindD3DKMTDeviceDesc();

			DWORD dwBus;
			DWORD dwDev;
			DWORD dwFn;

			lpImplementation->GetLocation(&dwBus, &dwDev, &dwFn);

			APPEND_LOG4("GPU%d : IGCL implementation on bus %d, dev %d, fn %d", GetCount(), dwBus, dwDev, dwFn);

			AddTail(lpImplementation);
		}

		POSITION pos = m_d3dkmtWrapper.GetDeviceList()->GetHeadPosition();

		while (pos)
			//populate unified implementations list for D3DKMT devices unsupported by NVAPI/ATIADL (e.g. Intel iGPUs)
		{
			LPD3DKMTDEVICE_DESC lpDesc = m_d3dkmtWrapper.GetDeviceList()->GetNext(pos);

			if (lpDesc->Bus || lpDesc->Dev)
				//ensure that we're processing actual hardware - skip basic render, which has no PCI location info
			{
				if (!FindGpu(lpDesc->Bus, lpDesc->Dev, lpDesc->Fn))
					//check if current D3DKMT device is already mapped to existing NVAPI/ATIADL implementations
				{
					CHALImplementationUnified* lpImplementation = new CHALImplementationUnified;

					lpImplementation->SetD3DKMTWrapper(&m_d3dkmtWrapper);
					lpImplementation->SetD3DKMTDeviceDesc(lpDesc);

					APPEND_LOG4("GPU%d : unified implementation on bus %d, dev %d, fn %d", GetCount(), lpDesc->Bus, lpDesc->Dev, lpDesc->Fn);

					AddTail(lpImplementation);
				}
			}
		}

		//init data sources list

		m_dataSourcesList.Init(this);

		//init HAL macro list with system specs

		CString strCPUFull	= Trim(GetCpuBrandString());
		CString strCPU		= PackCPUString(strCPUFull, FALSE);
		CString	strCPUShort	= PackCPUString(strCPUFull, TRUE);

		m_strMacroList.AddTail("%CPU%");
		m_strMacroList.AddTail(strCPU);
		m_strMacroList.AddTail("%CPUFull%");
		m_strMacroList.AddTail(strCPUFull);
		m_strMacroList.AddTail("%CPUShort%");
		m_strMacroList.AddTail(strCPUShort);

		CString strRAM;
		ULONGLONG qwTotalRAM	= GetTotalRAM();
		DWORD dwGB				= (DWORD)(qwTotalRAM / 1048576);
		DWORD dwGBChunk			= (DWORD)(qwTotalRAM % 1048576) * 100 / 1048576;
		if (dwGBChunk)
			strRAM.Format("%d.%02dGB", dwGB, dwGBChunk);
		else
			strRAM.Format("%dGB", dwGB);

		m_strMacroList.AddTail("%RAM%");
		m_strMacroList.AddTail(strRAM);

		if (GetGpuCount())
		{
			CHALImplementation* lpImplementation = GetHead();

			CString strGPUFull	= lpImplementation->GetName();
			CString strGPU		= PackGPUString(strGPUFull);

			m_strMacroList.AddTail("%GPU%");
			m_strMacroList.AddTail(strGPU);
			m_strMacroList.AddTail("%GPUFull%");
			m_strMacroList.AddTail(strGPUFull);

			CString strVRAM;
			DWORD dwTotalVRAM		= ALIGN(lpImplementation->GetTotalVideomemory(), 1048576 / 2);
				//some GPUs can report slightly less total VRAM than it is actually installed, so we align total VRAM to 0.5GB boundary
			DWORD dwGB				= (dwTotalVRAM / 1048576);
			DWORD dwGBChunk			= (dwTotalVRAM % 1048576) * 100 / 1048576;

			if (dwGBChunk)
				strRAM.Format("%d.%02dGB", dwGB, dwGBChunk);
			else
				strRAM.Format("%dGB", dwGB);

			m_strMacroList.AddTail("%VRAM%");
			m_strMacroList.AddTail(strRAM);

			CString strDriver	= lpImplementation->GetDriverStr();

			m_strMacroList.AddTail("%Driver%");
			m_strMacroList.AddTail(strDriver);
		}

		m_bInitialized	= TRUE;
	}
}
//////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI* PGETPHYSICALLYINSTALLEDSYSTEMMEMORY)(PULONGLONG TotalMemoryInKilobytes);
//////////////////////////////////////////////////////////////////////
ULONGLONG CHAL::GetTotalRAM()
{
	if (m_qwTotalRAM)
		return m_qwTotalRAM;

	ULONGLONG qwTotalRAM;

	HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");

	if (hKernel)
	{
		PGETPHYSICALLYINSTALLEDSYSTEMMEMORY pGetPhysicallyInstalledSystemMemory = (PGETPHYSICALLYINSTALLEDSYSTEMMEMORY)GetProcAddress(hKernel, "GetPhysicallyInstalledSystemMemory");

		if (pGetPhysicallyInstalledSystemMemory)
		{
			if (pGetPhysicallyInstalledSystemMemory(&qwTotalRAM))
				return qwTotalRAM;
		}
	}

	MEMORYSTATUSEX status; 
	status.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&status);

	return status.ullTotalPhys / 1024;
}
//////////////////////////////////////////////////////////////////////
CString CHAL::Trim(LPCSTR lpString)
{
	CString strResult = lpString;

	while (strResult.Replace("  ", " "))
	{
	}

	strResult.Trim();

	return strResult;
}
/////////////////////////////////////////////////////////////////////////////
CString CHAL::PackCPUString(LPCSTR lpString, BOOL bStripClockFrequency)
{
	CString strResult = lpString;

	strResult.Replace("AMD"				, "");
	strResult.Replace("Quad-Core"		, "");
	strResult.Replace("Six-Core"		, "");
	strResult.Replace("Eight-Core"		, "");
	strResult.Replace("8-Core"			, "");
	strResult.Replace("12-Core"			, "");
	strResult.Replace("16-Core"			, "");
	strResult.Replace("24-Core"			, "");
	strResult.Replace("32-Core"			, "");
	strResult.Replace("Processor"		, "");
	strResult.Replace("Intel(R)"		, "");
	strResult.Replace("Core(TM)2 Duo"	, "");
	strResult.Replace("Core(TM)2 Quad"	, "");
	strResult.Replace("Core(TM)"		, "");
	strResult.Replace("CPU"				, "");
	strResult.Replace("11th Gen"		, "");
	strResult.Replace("12th Gen"		, "");

	if (bStripClockFrequency)
	{
		int nPos = strResult.Find("@");

		if (nPos != -1)
			strResult = strResult.Left(nPos);
	}


	return Trim(strResult);
}
//////////////////////////////////////////////////////////////////////
CString CHAL::PackGPUString(LPCSTR lpString)
{
	CString strResult = lpString;

	strResult.Replace("Intel(R)"	, "");
	strResult.Replace("Graphics"	, "");
	strResult.Replace("NVIDIA"		, "");
	strResult.Replace("GeForce"		, "");
	strResult.Replace("ATI"			, "");
	strResult.Replace("AMD"			, "");
	strResult.Replace("Radeon"		, "");
	strResult.Replace("Series"		, "");
	strResult.Replace("(TM)"		, "");
	strResult.Replace("Laptop GPU"	, "");

	return Trim(strResult);
}
//////////////////////////////////////////////////////////////////////
DWORD CHAL::GetCpuCount()
{
	return m_cpuInfo.GetCpuCount();
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetCpuUsage(DWORD dwCpu)
{
	return m_cpuInfo.GetCpuUsage(dwCpu);
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetTotalCpuUsage()
{
	return m_cpuInfo.GetTotalCpuUsage();
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetMaxCpuUsage()
{
	return m_cpuInfo.GetMaxCpuUsage();
}
//////////////////////////////////////////////////////////////////////
BOOL CHAL::IsCpuTemperatureReportingSupported(DWORD dwCpu)
{
	return m_cpuInfo.IsCpuTemperatureReportingSupported(dwCpu);
}
//////////////////////////////////////////////////////////////////////
BOOL CHAL::IsCpuPowerReportingSupported(DWORD dwCpu)
{
	return m_cpuInfo.IsCpuPowerReportingSupported(dwCpu);
}
//////////////////////////////////////////////////////////////////////
BOOL CHAL::IsCpuBusClockReportingSupported()
{
	return m_cpuInfo.IsBusClockReportingSupported();
}
//////////////////////////////////////////////////////////////////////
BOOL CHAL::IsCpuClockReportingSupported(DWORD dwCpu)
{
	return m_cpuInfo.IsCpuClockReportingSupported(dwCpu);
}
//////////////////////////////////////////////////////////////////////
BOOL CHAL::IsEffectiveCpuClockReportingSupported(DWORD dwCpu, BOOL bUnhalted)
{
	return m_cpuInfo.IsEffectiveCpuClockReportingSupported(dwCpu, bUnhalted);
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetCpuTemperature(DWORD dwCpu)
{
	return m_cpuInfo.GetCpuTemperature(dwCpu);
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetMaxCpuTemperature()
{
	return m_cpuInfo.GetMaxCpuTemperature();
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetCpuPower(DWORD dwCpu)
{
	return m_cpuInfo.GetCpuPower(dwCpu);
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetCpuClock(DWORD dwCpu)
{
	return m_cpuInfo.GetCpuClock(dwCpu);
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetCpuClockAsync(DWORD dwCpu)
{
	return m_cpuInfo.GetCpuClockAsync(dwCpu);
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetMaxCpuClockAsync()
{
	return m_cpuInfo.GetMaxCpuClockAsync();
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetCpuBusClock()
{
	return m_cpuInfo.GetBusClock();
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetEffectiveCpuClock(DWORD dwCpu, BOOL bUnhalted)
{
	return m_cpuInfo.GetEffectiveCpuClock(dwCpu, bUnhalted);
}
//////////////////////////////////////////////////////////////////////
DWORD CHAL::GetCcdCount()
{
	return m_cpuInfo.GetCcdCount();
}
//////////////////////////////////////////////////////////////////////
BOOL CHAL::IsCcdTemperatureReportingSupported(DWORD dwCcd)
{
	return m_cpuInfo.IsCcdTemperatureReportingSupported(dwCcd);
}
//////////////////////////////////////////////////////////////////////
float CHAL::GetCcdTemperature(DWORD dwCcd)
{
	return m_cpuInfo.GetCcdTemperature(dwCcd);
}
//////////////////////////////////////////////////////////////////////
ULONGLONG CHAL::GetPhysicalMemoryUsage()
{
	MEMORYSTATUSEX status; 
	status.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&status);

	return (status.ullTotalPhys - status.ullAvailPhys) / 1048576;
}
/////////////////////////////////////////////////////////////////////////////
ULONGLONG CHAL::GetCommitCharge()
{
	MEMORYSTATUSEX status; 
	status.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&status);

	return (status.ullTotalPageFile - status.ullAvailPageFile) / 1048576;
}
/////////////////////////////////////////////////////////////////////////////
float CHAL::GetPhysicalMemoryUsagePercent()
{
	MEMORYSTATUSEX status; 
	status.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&status);

	return 100.0f * (status.ullTotalPhys - status.ullAvailPhys) / status.ullTotalPhys;
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::InitPingThread(LPCSTR lpAddr)
{
	m_strPingAddr = lpAddr;

	if (strlen(lpAddr))
	{
		if (m_lpPingThread)
			m_lpPingThread->SetAddr(lpAddr);
		else
		{
			m_lpPingThread = new CPingThread(lpAddr);

			m_lpPingThread->CreateThread(CREATE_SUSPENDED);
			m_lpPingThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
			m_lpPingThread->ResumeThread();
		}
	}
	else
		UninitPingThread();
}
/////////////////////////////////////////////////////////////////////////////
CString	CHAL::GetPingAddr()
{
	return m_strPingAddr;
}
/////////////////////////////////////////////////////////////////////////////
float CHAL::GetPing()
{
	if (m_lpPingThread)
	{
		m_lpPingThread->UpdatePingAsync();

		return m_lpPingThread->GetPing();
	}

	return FLT_MAX;
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::UninitPingThread()
{
	if (m_lpPingThread)
		m_lpPingThread->Kill();
	m_lpPingThread = NULL;

	m_strPingAddr = "";
}
/////////////////////////////////////////////////////////////////////////////
CHALImplementation* CHAL::FindGpu(DWORD dwBus, DWORD dwDev, DWORD dwFn)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CHALImplementation* lpImplementation = GetNext(pos);

		DWORD dwImplementationBus;
		DWORD dwImplementationDev;
		DWORD dwImplementationFn;

		if (lpImplementation->GetLocation(&dwImplementationBus, &dwImplementationDev, &dwImplementationFn))
		{
			if ((dwBus == dwImplementationBus) &&
				(dwDev == dwImplementationDev) &&
				(dwFn  == dwImplementationFn ))
				return lpImplementation;
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHAL::GetGpuCount()
{
	return GetCount();
}
/////////////////////////////////////////////////////////////////////////////
CHALImplementation* CHAL::GetGpu(DWORD dwGpu)
{
	POSITION pos = FindIndex(dwGpu);

	if (pos)
		return GetAt(pos);

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CHALDataSourcesList* CHAL::GetDataSourcesList()
{
	return &m_dataSourcesList;
}
/////////////////////////////////////////////////////////////////////////////
CHALDataSource* CHAL::FindSource(LPCSTR lpSource)
{
	return m_dataSourcesList.FindSource(lpSource);
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::SetTimestamp(DWORD dwTimestamp)
{
	m_cpuInfo.SetTimestamp(dwTimestamp);

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CHALImplementation* lpImplementation = GetNext(pos);

		lpImplementation->SetTimestamp(dwTimestamp);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHAL::IsCalcCpuClocksRequired()
{
	POSITION pos = m_dataSourcesList.GetHeadPosition();

	while (pos)
	{
		CHALDataSource* lpSource = m_dataSourcesList.GetNext(pos);

		if (lpSource->IsDirty())
		{
			switch (lpSource->GetID())
			{
			case HALDATASOURCE_ID_CPU_CLOCK:
			case HALDATASOURCE_ID_CPU_BUS_CLOCK:
				return TRUE;
			}

		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::UpdateSources()
{
	BOOL bCalcCpuClocks = IsCalcCpuClocksRequired();

	if (bCalcCpuClocks)
		//sleep at least 100ms before calculating CPU clocks to avoid observer effect and ensure that we read correct idle clocks, 
		//do it BEFORE locking the mutex
		Sleep(100);

	HANDLE hMutex = CreateMutex(NULL, FALSE, "Global\\Access_hw_poll");

	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);

	TIMING_INIT
	TIMING_BEGIN		

	if (bCalcCpuClocks)
		m_cpuInfo.CalcCpuClocks();

	POSITION pos = m_dataSourcesList.GetHeadPosition();

	while (pos)
	{
		CHALDataSource* lpSource = m_dataSourcesList.GetNext(pos);

		if (lpSource->IsDirty())
		{
			lpSource->Poll();

			lpSource->SetDirty(FALSE);
		}
	}

	TIMING_END

	m_fltPollingTime = timing;

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::UpdateSourcesAsync()
{
	InitPollingThread();

	if (m_lpPollingThread)
		m_lpPollingThread->AsyncPoll();
	else
		UpdateSources();
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::InitPollingThread()
{
	if (!m_lpPollingThread)
	{
		m_lpPollingThread = new CHALPollingThread(this);

		m_lpPollingThread->CreateThread(CREATE_SUSPENDED);
		m_lpPollingThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
		m_lpPollingThread->ResumeThread();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::UninitPollingThread()
{
	if (m_lpPollingThread)
		m_lpPollingThread->Kill();
	m_lpPollingThread = NULL;
}
/////////////////////////////////////////////////////////////////////////////
float CHAL::GetPollingTime()
{
	return m_fltPollingTime;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHAL::IsIntelCpu()
{
	return m_cpuInfo.IsIntelCpu();
}
//////////////////////////////////////////////////////////////////////
BOOL CHAL::IsAMDCpu()
{
	return m_cpuInfo.IsAMDCpu();
}
//////////////////////////////////////////////////////////////////////
LPCSTR CHAL::GetCpuString()
{
	return m_cpuInfo.GetCpuString();
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CHAL::GetCpuBrandString()
{
	return m_cpuInfo.GetCpuBrandString();
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHAL::GetCpuFamily()
{
	return m_cpuInfo.GetCpuFamily();
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHAL::GetCpuModel()
{
	return m_cpuInfo.GetCpuModel();
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHAL::GetCpuStepping()
{
	return m_cpuInfo.GetCpuStepping();
}
/////////////////////////////////////////////////////////////////////////////
CStringList* CHAL::GetMacroList()
{
	return &m_strMacroList;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHAL::GetLifetime()
{
	return GetTickCount() - m_dwCreationTimestamp;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHAL::GetProcessMemoryUsage()
{
	DWORD dwUsage = 0;

/*
	//We could use GetProcessMemoryInfo to read process memory usage directly, but it won't work for the applications protected with
	//BattlEye/EAC due to restricted OpenProcess, so commented part is provided as an example of alternate inplace implementation 
	//for third party developers
	//We'll use RTSS process specific performance counters access interface instead, it will do the same in context of protected process

	HANDLE hProcess = OpenForegroundProcess();

	if (hProcess)
	{
		PROCESS_MEMORY_COUNTERS pmc;

		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(PROCESS_MEMORY_COUNTERS)))
			dwUsage = pmc.WorkingSetSize>>10L;

		CloseHandle(hProcess);
	}
*/

	CRTSSSharedMemoryInterface sharedMemoryInterface;
	sharedMemoryInterface.GetProcessPerfCounter(0, 1000, PROCESS_PERF_COUNTER_ID_RAM_USAGE, 0, &dwUsage);

	return dwUsage;
}
/////////////////////////////////////////////////////////////////////////////
float CHAL::GetProcessMemoryUsagePercent()
{
	DWORD dwUsage = (DWORD)GetProcessMemoryUsage();
	DWORD dwTotal = (DWORD)GetTotalRAM();

	if (dwTotal)
		return 100.0f * dwUsage / dwTotal;

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::SetCpuUsageCalc(DWORD dwCalc)
{
	m_cpuInfo.SetCpuUsageCalc(dwCalc);
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::EnableSmart(BOOL bEnable)
{
	m_smartInfo.Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CHAL::SetSmartPollingInterval(DWORD dwPollingInterval)
{
	m_smartInfo.SetPollingInterval(dwPollingInterval);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHAL::IsHddTemperatureReportingSupported(DWORD dwDrive, DWORD dwSensor)
{
	return m_smartInfo.IsTemperatureReportingSupported(dwDrive, dwSensor);
}
/////////////////////////////////////////////////////////////////////////////
float CHAL::GetHddTemperature(DWORD dwDrive, DWORD dwSensor)
{
	return m_smartInfo.GetTemperature(dwDrive, dwSensor);
}
/////////////////////////////////////////////////////////////////////////////
