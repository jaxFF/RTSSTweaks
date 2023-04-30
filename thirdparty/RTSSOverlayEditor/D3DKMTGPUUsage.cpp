// D3DKMTGPUUsage.cpp: implementation of the CD3DKMTGPUUsage class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D3DKMTGPUUsage.h"
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CD3DKMTGPUUsage::CD3DKMTGPUUsage()
{
	m_dwTickCount = 0;
	m_dwNodeCount = 0;

	for (DWORD dwNode=0; dwNode<MAX_NODES; dwNode++)
	{
		m_qwNodeRunningTime[dwNode].QuadPart	= 0;
		m_dwGpuUsage[dwNode]					= 0;
	}
}
//////////////////////////////////////////////////////////////////////
CD3DKMTGPUUsage::~CD3DKMTGPUUsage()
{
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTGPUUsage::CalcGpuUsage(DWORD dwTickCount, LARGE_INTEGER* lpNodeRunningTimeArr, DWORD dwNodeCount)
{
	if (m_dwTickCount)
	{
		if (dwTickCount - m_dwTickCount >= 1000)
		{
			for (DWORD dwNode=0; dwNode<dwNodeCount; dwNode++)
			{
				LONG dwGpuUsage = (LONG)((lpNodeRunningTimeArr[dwNode].QuadPart - m_qwNodeRunningTime[dwNode].QuadPart) / (100 * (dwTickCount - m_dwTickCount)));

				if (dwGpuUsage < 0)
					dwGpuUsage = 0;

				if (dwGpuUsage > 100)
					dwGpuUsage = 100;

				m_qwNodeRunningTime[dwNode].QuadPart	= lpNodeRunningTimeArr[dwNode].QuadPart;
				m_dwGpuUsage[dwNode]					= dwGpuUsage;
			}

			m_dwTickCount = dwTickCount;
			m_dwNodeCount = dwNodeCount;
		}
	}
	else
	{
		for (DWORD dwNode=0; dwNode<dwNodeCount; dwNode++)
			m_qwNodeRunningTime[dwNode].QuadPart	= lpNodeRunningTimeArr[dwNode].QuadPart;

		m_dwTickCount = dwTickCount;
		m_dwNodeCount = dwNodeCount;
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CD3DKMTGPUUsage::GetGpuUsage(DWORD dwNode)
{
	if (dwNode < m_dwNodeCount)
		return m_dwGpuUsage[dwNode];

	return 0xFFFFFFFF;
}
//////////////////////////////////////////////////////////////////////
DWORD CD3DKMTGPUUsage::GetNodeCount()
{
	return m_dwNodeCount;
}
//////////////////////////////////////////////////////////////////////
