// ThreadAffinityMask.h: interface for the CThreadAffinityMask class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
class CThreadAffinityMask
{
public:
	CThreadAffinityMask(DWORD dwCpuCount, DWORD dwCpu);
	~CThreadAffinityMask();

protected:
	DWORD	m_dwCpuCount;
	DWORD	m_dwCpu;
	HANDLE	m_hThread;
	DWORD	m_dwThreadAffinityMask;
};
//////////////////////////////////////////////////////////////////////
