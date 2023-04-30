// RTCoreDriver.h: interface for the CRTCoreDriver class.
// 
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _RTCOREDRIVER_H_INCLUDED_
#define _RTCOREDRIVER_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4201)
#include <winioctl.h>
#pragma warning (default : 4201)
//////////////////////////////////////////////////////////////////////
typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;
//////////////////////////////////////////////////////////////////////
//
// CRTCoreDriver error codes
//
//////////////////////////////////////////////////////////////////////
#ifndef _RTCORESYS_H_INCLUDED_
#include "RTCoreSys.h"
#endif
//////////////////////////////////////////////////////////////////////
#define UNINSTALL_MODE_AUTO				0
#define UNINSTALL_MODE_NEVER			1
#define UNINSTALL_MODE_ALWAYS			2

#define UNINSTALL_MODE_LAST_VALID		2
//////////////////////////////////////////////////////////////////////
#define IMPLEMENTATION_MINI				1
#define IMPLEMENTATION_FULL				2
//////////////////////////////////////////////////////////////////////
class CRTCoreDriver  
{
public:
	HRESULT Load(HMODULE hModule = NULL);
		//installs/loads driver
	HRESULT Unload(BOOL bUninstall);
		//unloads and optionally uninstalls driver
		//Note: this function is automatically called by destructor, so normally there is no need to
		//uninstall it manually
	BOOL SetUninstallMode(DWORD dwMode);
		//Sets destructor-level driver uninstallation mode. Returns FALSE if invalid mode is specified.
		//You can use one of the following modes:

		//UNINSTALL_MODE_AUTO	- driver is uninstalled by destructor only if IOCTL_RTCORESYS_REFERENCE_COUNT
		//is supported and requested reference count is equal to 1
		//UNINSTALL_MODE_NEVER	- driver is never uninstalled by destructor
		//UNINSTALL_MODE_ALWAYS - driver is always uninstalled by destructor

		//Note: this method affects destructor-level driver uninstallation only. You may always manually
		//uninstall driver using Unload(TRUE)

	HANDLE	GetHandle();
		//returns driver handle
	CString GetName();
		//returns driver name
	CString GetPath(HMODULE hModule);
		//returns path to driver file

	void SetImplementation(DWORD dwImplementation);
		//sets full RTCore or RTCoreMini driver implementation
	DWORD GetImplementation();
		//returns implementation
	BOOL IsImplementationAvailable(HMODULE hModule, DWORD dwImplementation);
		//checks if driver implementation binary is available

	//reference count related functions

	BOOL ReferenceCountGet(DWORD& dwCount);
		//returns current reference count
	BOOL ReferenceCountIncrease();
		//increases reference count
	BOOL ReferenceCountDecrease();
		//decreases reference count
	BOOL ReferenceCountReset();
		//resets reference count

	BOOL ReferenceCountHelper(PREFERENCE_COUNT_INFO pInfo);
		//helper for reading/writing reference count
	
	//driver version related functions

	BOOL GetVersion(PVERSION_INFO pInfo);
		//fills VERSION_INFO structure
	BOOL IsCorrectVersionLoaded();
		//returns TRUE if correct version of RTCore.sys driver 
		//(i.e. RTCORESYS_VERSION_MAJOR.RTCORESYS_VERSION_MINOR) is loaded 

	//MSR related functions	

	BOOL ReadMSR(PMSR_INFO pInfo);
		//reads Model-Specific Register
	BOOL WriteMSR(PMSR_INFO pInfo);
		//writes Model-Specific Register

	//PCI configuration space related functions

	BOOL GetBusData(PBUSDATA_INFO pInfo);
		//reads PCR
	BOOL SetBusData(PBUSDATA_INFO pInfo);
		//writes PCR

	CRTCoreDriver();
	virtual ~CRTCoreDriver();

private:
	HRESULT LoadHelper(HMODULE hModule);
		//helper for loading driver

	HANDLE	m_hDriver;
		//driver handle
	DWORD	m_dwUninstallMode;
		//driver unsinstallation mode
	DWORD	m_dwImplementation;
		//full RTCore or RTCoreMini driver implementation
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
