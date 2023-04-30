// RTCoreDriver.cpp: implementation of the CRTCoreDriver class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RTCoreDriver.h"
//////////////////////////////////////////////////////////////////////
#include <io.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsvc.h>

#include <shlwapi.h>
//////////////////////////////////////////////////////////////////////
#ifndef _RTCOREERRORS_H_INCLUDED_
#include "RTCoreErrors.h"
#endif
#ifndef _WOW64_H_INCLUDED_
#include "WOW64.h"
#endif
//////////////////////////////////////////////////////////////////////
#define MAKE_PULONG(p) ((PULONG)(ULONG64)p)
#define MAKE_PUCHAR(p) ((PUCHAR)(ULONG64)p)
////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRTCoreDriver::CRTCoreDriver()
{
	m_hDriver				= INVALID_HANDLE_VALUE;
	m_dwUninstallMode		= UNINSTALL_MODE_AUTO;
	m_dwImplementation		= IMPLEMENTATION_MINI;
}
//////////////////////////////////////////////////////////////////////
CRTCoreDriver::~CRTCoreDriver()
{
	switch (m_dwUninstallMode)
	{
	case UNINSTALL_MODE_AUTO:
		DWORD dwReferenceCount;

		if (ReferenceCountGet(dwReferenceCount) && (dwReferenceCount == 1))
			Unload(TRUE);
		else
			Unload(FALSE);
		break;

	case UNINSTALL_MODE_NEVER:
		Unload(FALSE);
		break;

	case UNINSTALL_MODE_ALWAYS:
		Unload(TRUE);
		break;
	}
}
//////////////////////////////////////////////////////////////////////
HRESULT CRTCoreDriver::Load(HMODULE hModule)
{
	if (FAILED(LoadHelper(hModule)))
		//try to load driver.
	{
		Unload(TRUE);

		HRESULT errCode = LoadHelper(hModule);

		if (FAILED(errCode))
		{
			return errCode;
		}
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
HRESULT CRTCoreDriver::Unload(BOOL bUninstall)
{
	CString strName = GetName();

	CString strMutex;
	strMutex.Format("Global\\Access_%s_sys", (LPCSTR)strName);

	HANDLE hMutex	= CreateMutex(NULL, FALSE, strMutex);
	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);

	if (m_hDriver != INVALID_HANDLE_VALUE)
	{
		ReferenceCountDecrease();
			//decrease reference count

			//reference count decrement may fail in case of old driver usage, so we don't
			//check result purposely!!!

		CloseHandle(m_hDriver);
			//unload driver file

		m_hDriver = INVALID_HANDLE_VALUE;
			//invalidate driver hadle
	}

	if (bUninstall)
	{
		SC_HANDLE hManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			//open service control manager

		if (!hManager)
			//return error if we cannot open service control manager
		{
			if (hMutex)
			{
				ReleaseMutex(hMutex);
				CloseHandle(hMutex);
			}

			return RTCOREDRIVER_E_INVALID_SCM_HANDLE;
		}

		WCHAR wzName[MAX_PATH]			= { 0 };
	
		size_t result;
		mbstowcs_s(&result, wzName, sizeof(wzName)/sizeof(wzName[0]), strName, strlen(strName));   

		SC_HANDLE hService = OpenServiceW(hManager, wzName, SERVICE_ALL_ACCESS);
			//open service

		if (!hService)
		{
			if (hMutex)
			{
				ReleaseMutex(hMutex);
				CloseHandle(hMutex);
			}

			return RTCOREDRIVER_E_CANNOT_INSTALL_OR_OPEN_SERVICE;
		}

		SERVICE_STATUS  serviceStatus;
		ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
			//stopping service

		DeleteService(hService);
			//uninstalling service

		CloseServiceHandle(hService);
		CloseServiceHandle(hManager);
	}

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4189)

HRESULT CRTCoreDriver::LoadHelper(HMODULE hModule)
{
	CString strName = GetName();

	CString strMutex;
	strMutex.Format("Global\\Access_%s_sys", (LPCSTR)strName);

	HANDLE hMutex	= CreateMutex(NULL, FALSE, strMutex);
	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);

	m_hDriver = CreateFile("\\\\.\\" + strName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_hDriver != INVALID_HANDLE_VALUE)
	{
		ReferenceCountIncrease();
			//reference count increment may fail in case of old driver usage, so we don't
			//check result purposely

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}

		return S_OK;
	}

	CString strDriverPath = GetPath(hModule);	

	if (_access(strDriverPath, 0))
		//check driver file presence
	{
		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}

		return RTCOREDRIVER_E_FILE_NOT_FOUND;
	}
	
	SC_HANDLE hManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		//open service control manager

	if (!hManager)
		//return error if we cannot open service control manager
	{
		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}

		return RTCOREDRIVER_E_INVALID_SCM_HANDLE;
	}

	WCHAR wzName[MAX_PATH]			= { 0 };
	WCHAR wzDriverPath[MAX_PATH]	= { 0 };
	
	size_t result;

	mbstowcs_s(&result, wzName, sizeof(wzName)/sizeof(wzName[0]), strName, strlen(strName));   
	mbstowcs_s(&result, wzDriverPath, sizeof(wzDriverPath)/sizeof(wzDriverPath[0]), strDriverPath, strlen(strDriverPath));   

	SC_HANDLE hService
		= CreateServiceW(hManager					// SCManager database
			            , wzName					// name of service
				        , wzName					// name to display
					    , SERVICE_ALL_ACCESS		// desired access
						, SERVICE_KERNEL_DRIVER		// service type
						, SERVICE_DEMAND_START		// start type
						, SERVICE_ERROR_NORMAL		// error control type
						, wzDriverPath				// service's binary
						, NULL						// no load ordering group
						, NULL						// no tag identifier
						, NULL						// no dependencies
						, NULL						// LocalSystem account
						, NULL						// no password
						);
		//install service

	if (hService)
	{
		CloseServiceHandle(hService);
	}

	hService = OpenServiceW(hManager, wzName, SERVICE_ALL_ACCESS);

	if (hService)
	{
		StartServiceW(hService, 0, NULL);

		SERVICE_STATUS	ssStatus; 
		DWORD			dwOldCheckPoint; 
		DWORD			dwStartTickCount;
		DWORD			dwWaitTime;
 
	    // Check the status until the service is no longer start pending. 
 
		if (!QueryServiceStatus( 
				hService,   // handle to service 
				&ssStatus) )  // address of status information structure
		{
			if (hMutex)
			{
				ReleaseMutex(hMutex);
				CloseHandle(hMutex);
			}

			return RTCOREDRIVER_E_CANNOT_QUERY_SERVICE_STATUS;
		}
 
		// Save the tick count and initial checkpoint.

		dwStartTickCount	= GetTickCount();
		dwOldCheckPoint		= ssStatus.dwCheckPoint;

		while (ssStatus.dwCurrentState == SERVICE_START_PENDING) 
		{ 
			// Do not wait longer than the wait hint. A good interval is 
			// one tenth the wait hint, but no less than 1 second and no 
			// more than 10 seconds. 
 
			dwWaitTime = ssStatus.dwWaitHint / 10;

			if( dwWaitTime < 1000 )
				dwWaitTime = 1000;
			else if ( dwWaitTime > 10000 )
				dwWaitTime = 10000;

			Sleep( dwWaitTime );

			// Check the status again. 
 
			if (!QueryServiceStatus( 
					hService,   // handle to service 
					&ssStatus) )  // address of structure
			{
				break; 
			}
 
			if ( ssStatus.dwCheckPoint > dwOldCheckPoint )
			{
				// The service is making progress.

				dwStartTickCount	= GetTickCount();
				dwOldCheckPoint		= ssStatus.dwCheckPoint;
			}
			else
			{
				if(GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
				{
					break;
				}
			}
		} 

		CloseServiceHandle(hService);
	}
	else
	{
		CloseServiceHandle(hManager);

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}

		return RTCOREDRIVER_E_CANNOT_INSTALL_OR_OPEN_SERVICE;
	}

	m_hDriver = CreateFile("\\\\.\\" + strName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		//try to open driver

	if (m_hDriver == INVALID_HANDLE_VALUE)
	{
		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}

		return RTCOREDRIVER_E_CANNOT_OPEN_DRIVER;
	}

	ReferenceCountIncrease();
		//reference count increment may fail in case of old driver usage, so we don't
		//check result purposely

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	return S_OK;
}

#pragma warning (default : 4189)
//////////////////////////////////////////////////////////////////////
HANDLE CRTCoreDriver::GetHandle()
{
	return m_hDriver;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::GetVersion(PVERSION_INFO pInfo)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD cbReturned;

	if (DeviceIoControl(m_hDriver, (DWORD)IOCTL_RTCORESYS_VERSION, pInfo, sizeof(VERSION_INFO), pInfo, sizeof(VERSION_INFO), &cbReturned, 0))
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::ReferenceCountHelper(PREFERENCE_COUNT_INFO pInfo)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD cbReturned;

	if (DeviceIoControl(m_hDriver, (DWORD)IOCTL_RTCORESYS_REFERENCE_COUNT, pInfo, sizeof(REFERENCE_COUNT_INFO), pInfo, sizeof(REFERENCE_COUNT_INFO), &cbReturned, 0))
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::ReferenceCountIncrease()
{
	REFERENCE_COUNT_INFO info;

	info.dwCount = REFERENCE_COUNT_INVALID;
	info.dwDelta = 1;

	return ReferenceCountHelper(&info);
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::ReferenceCountDecrease()
{
	REFERENCE_COUNT_INFO info;

	info.dwCount = REFERENCE_COUNT_INVALID;
	info.dwDelta = 0xffffffff;

	return ReferenceCountHelper(&info);
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::ReferenceCountGet(DWORD &dwCount)
{
	REFERENCE_COUNT_INFO info;

	info.dwCount = REFERENCE_COUNT_INVALID;
	info.dwDelta = 0;

	if (ReferenceCountHelper(&info))
	{
		dwCount	= info.dwCount;
		return TRUE;
	}

	dwCount = REFERENCE_COUNT_INVALID;
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::IsCorrectVersionLoaded()
{
	VERSION_INFO info;

	if (!GetVersion(&info))
		return FALSE;

	return (info.dwMajor == RTCORESYS_VERSION_MAJOR) && (info.dwMinor == RTCORESYS_VERSION_MINOR);
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::ReferenceCountReset()
{
	REFERENCE_COUNT_INFO info;

	info.dwCount = 0;
	info.dwDelta = 0;

	return ReferenceCountHelper(&info);
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::SetUninstallMode(DWORD dwMode)
{
	if (dwMode > UNINSTALL_MODE_LAST_VALID)
		return FALSE;

	m_dwUninstallMode = dwMode;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::ReadMSR(PMSR_INFO pInfo)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD  cbReturned;

	if (DeviceIoControl(m_hDriver, (DWORD)IOCTL_RTCORESYS_READ_MSR, pInfo, sizeof(MSR_INFO), pInfo, sizeof(MSR_INFO), &cbReturned, 0))
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::WriteMSR(PMSR_INFO pInfo)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD  cbReturned;

	if (DeviceIoControl(m_hDriver, (DWORD)IOCTL_RTCORESYS_WRITE_MSR, pInfo, sizeof(MSR_INFO), pInfo, sizeof(MSR_INFO), &cbReturned, 0))
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
CString CRTCoreDriver::GetName()
{
	BOOL bWow64 = FALSE;
	CWOW64::IsWow64Process(GetCurrentProcess(), &bWow64);
		//check if we're under WOW64

	CString strName;
	if (m_dwImplementation == IMPLEMENTATION_MINI)
		strName.Format("RTCoreMini%d", bWow64 ? 64 : 32);
	else
		strName.Format("RTCore%d", bWow64 ? 64 : 32);

	return strName;
}
//////////////////////////////////////////////////////////////////////
CString CRTCoreDriver::GetPath(HMODULE hModule)
{
	char szPath[MAX_PATH];
	GetModuleFileName(hModule, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);

	strcat_s(szPath, sizeof(szPath), "\\");
	strcat_s(szPath, sizeof(szPath), GetName());
	strcat_s(szPath, sizeof(szPath), ".sys");

	return szPath;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::GetBusData(PBUSDATA_INFO pInfo)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD cbReturned;

	if (DeviceIoControl(m_hDriver, (DWORD)IOCTL_RTCORESYS_GETBUSDATA, pInfo, sizeof(BUSDATA_INFO), pInfo, sizeof(BUSDATA_INFO), &cbReturned, 0))
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::SetBusData(PBUSDATA_INFO pInfo)
{
	if (m_hDriver == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD cbReturned;

	if (DeviceIoControl(m_hDriver, (DWORD)IOCTL_RTCORESYS_SETBUSDATA, pInfo, sizeof(BUSDATA_INFO), pInfo, sizeof(BUSDATA_INFO), &cbReturned, 0))
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CRTCoreDriver::SetImplementation(DWORD dwImplementation)
{
	m_dwImplementation = dwImplementation;
}
//////////////////////////////////////////////////////////////////////
DWORD CRTCoreDriver::GetImplementation()
{
	return m_dwImplementation;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTCoreDriver::IsImplementationAvailable(HMODULE hModule, DWORD dwImplementation)
{
	CRTCoreDriver driver;
	driver.SetImplementation(dwImplementation);

	if (_access(driver.GetPath(hModule), 0))
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
