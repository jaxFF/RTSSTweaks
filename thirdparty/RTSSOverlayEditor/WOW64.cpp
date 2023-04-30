// WOW64.cpp: implementation of the CWOW64 class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WOW64.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWOW64::CWOW64()
{
}
//////////////////////////////////////////////////////////////////////
CWOW64::~CWOW64()
{
}
//////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI* PWOW64DISABLEWOW64FSREDIRECTION)(PVOID*);
//////////////////////////////////////////////////////////////////////
BOOL CWOW64::Wow64DisableWow64FsRedirection(PVOID *pOld)
{
	HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");

	if (hKernel)
	{
		PWOW64DISABLEWOW64FSREDIRECTION pWow64DisableWow64FsRedirection = (PWOW64DISABLEWOW64FSREDIRECTION)GetProcAddress(hKernel, "Wow64DisableWow64FsRedirection");

		if (pWow64DisableWow64FsRedirection)
			return pWow64DisableWow64FsRedirection(pOld);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI* PWOW64REVERTWOW64FSREDIRECTION)(PVOID);
//////////////////////////////////////////////////////////////////////
BOOL CWOW64::Wow64RevertWow64FsRedirection(PVOID Old)
{
	HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");

	if (hKernel)
	{
		PWOW64REVERTWOW64FSREDIRECTION pWow64RevertWow64FsRedirection = (PWOW64REVERTWOW64FSREDIRECTION)GetProcAddress(hKernel, "Wow64RevertWow64FsRedirection");

		if (pWow64RevertWow64FsRedirection)
			return pWow64RevertWow64FsRedirection(Old);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
typedef BOOL (WINAPI* PISWOW64PROCESS)(HANDLE, PBOOL);
//////////////////////////////////////////////////////////////////////
BOOL CWOW64::IsWow64Process(HANDLE hProcess, PBOOL pResult)
{
	HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");

	if (hKernel)
	{
		PISWOW64PROCESS pIsWow64Process = (PISWOW64PROCESS)GetProcAddress(hKernel, "IsWow64Process");

		if (pIsWow64Process)
			return pIsWow64Process(hProcess, pResult);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
typedef UINT (WINAPI* PGETSYSTEMWOW64DIRECTORY)(LPTSTR, UINT);
//////////////////////////////////////////////////////////////////////
UINT CWOW64::GetSystemWow64Directory(LPTSTR lpBuffer, UINT uSize)
{
	HINSTANCE hKernel = GetModuleHandle("KERNEL32.DLL");

	if (hKernel)
	{
		PGETSYSTEMWOW64DIRECTORY pGetSystemWow64Directory = (PGETSYSTEMWOW64DIRECTORY)GetProcAddress(hKernel, "GetSystemWow64DirectoryA");

		if (pGetSystemWow64Directory)
			return GetSystemWow64Directory(lpBuffer, uSize);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////