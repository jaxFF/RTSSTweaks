// WOW64.h: interface for the CWOW64 class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _WOW64_H_INCLUDED_
#define _WOW64_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
class CWOW64  
{
public:
	static BOOL IsWow64Process(HANDLE hProcess, PBOOL pResult);
		//returns TRUE under WOW64
	static UINT GetSystemWow64Directory(LPTSTR lpBuffer, UINT uSize);
		//returns path to WOW64 directory
	static BOOL Wow64DisableWow64FsRedirection(PVOID* pOld);
		//disables WOW64 file system redirection
	static BOOL Wow64RevertWow64FsRedirection(PVOID Old);
		//reverts WOW64 file system redirection

	CWOW64();
	virtual ~CWOW64();
};
//////////////////////////////////////////////////////////////////////
#endif 
//////////////////////////////////////////////////////////////////////
