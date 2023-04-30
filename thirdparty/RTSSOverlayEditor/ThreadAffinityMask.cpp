// CThreadAffinityMask.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ThreadAffinityMask.h"
/////////////////////////////////////////////////////////////////////////////
CThreadAffinityMask::CThreadAffinityMask(DWORD dwCpuCount, DWORD dwCpu)
{
	m_dwCpuCount		= dwCpuCount;
	m_dwCpu				= dwCpu;

	m_hThread			= GetCurrentThread();

	if (m_dwCpuCount > 1)
		m_dwThreadAffinityMask = SetThreadAffinityMask(m_hThread, 1<<m_dwCpu);
}
/////////////////////////////////////////////////////////////////////////////
CThreadAffinityMask::~CThreadAffinityMask()
{
	if (m_dwCpuCount > 1)
		SetThreadAffinityMask(m_hThread, m_dwThreadAffinityMask);
}
/////////////////////////////////////////////////////////////////////////////
