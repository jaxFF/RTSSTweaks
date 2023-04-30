// HALImplementation.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HALImplementation.h"
#include "RTSSSharedMemoryInterface.h"

#include <float.h>
//////////////////////////////////////////////////////////////////////
CHALImplementation::CHALImplementation()
{
	m_lpD3DKMTWrapper		= NULL;		
	m_lpD3DKMTDeviceDesc	= NULL;
	m_dwTimestamp			= 0;
	m_dwTotalVideomemory	= 0;
}
//////////////////////////////////////////////////////////////////////
CHALImplementation::~CHALImplementation()
{
}
//////////////////////////////////////////////////////////////////////
void CHALImplementation::SetD3DKMTWrapper(CD3DKMTWrapper* lpD3DKMTWrapper)
{
	m_lpD3DKMTWrapper = lpD3DKMTWrapper;
}
//////////////////////////////////////////////////////////////////////
void CHALImplementation::SetD3DKMTDeviceDesc(LPD3DKMTDEVICE_DESC lpD3DKMTDeviceDesc)
{
	m_lpD3DKMTDeviceDesc = lpD3DKMTDeviceDesc;
}
//////////////////////////////////////////////////////////////////////
void CHALImplementation::SetTimestamp(DWORD dwTimestamp)
{
	m_dwTimestamp = dwTimestamp;
}
//////////////////////////////////////////////////////////////////////
BOOL CHALImplementation::BindD3DKMTDeviceDesc()
{
	if (m_lpD3DKMTWrapper)
	{
		DWORD dwBus;
		DWORD dwDev;
		DWORD dwFn;

		if (GetLocation(&dwBus, &dwDev, &dwFn))
		{
			LPD3DKMTDEVICE_DESC lpDesc = m_lpD3DKMTWrapper->FindDevice(dwBus, dwDev, dwFn);

			if (lpDesc)
			{
				SetD3DKMTDeviceDesc(lpDesc);

				return TRUE;
			}
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementation::GetD3DKMTGpuUsage(DWORD dwNodeMask)
{
	if (m_lpD3DKMTWrapper && m_lpD3DKMTDeviceDesc)
	{
		LARGE_INTEGER	qwNodeRunningTimeArr[MAX_NODES] = { 0 };
		DWORD			dwNodeCount						= MAX_NODES;

		m_lpD3DKMTWrapper->GetGpuRunningTime("", m_lpD3DKMTDeviceDesc->Bus, m_lpD3DKMTDeviceDesc->Dev, m_lpD3DKMTDeviceDesc->Fn, qwNodeRunningTimeArr, &dwNodeCount);

		if (dwNodeCount == MAX_NODES)
			return GPU_USAGE_INVALID;

		m_d3dkmtGpuUsage.CalcGpuUsage(GetTickCount(), qwNodeRunningTimeArr, dwNodeCount);

		DWORD dwGpuUsage = GPU_USAGE_INVALID;

		for (DWORD dwNode=0; dwNode<m_d3dkmtGpuUsage.GetNodeCount(); dwNode++)
		{
			if (dwNodeMask & (1ULL<<dwNode))
			{
				DWORD dwGpuUsageNode = m_d3dkmtGpuUsage.GetGpuUsage(dwNode);

				if (dwGpuUsageNode != GPU_USAGE_INVALID)
				{
					if (dwGpuUsage == GPU_USAGE_INVALID)
						dwGpuUsage = dwGpuUsageNode;
					else
						dwGpuUsage = max(dwGpuUsage, dwGpuUsageNode);
				}
			}
		}

		return dwGpuUsage;
	}

	return GPU_USAGE_INVALID;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementation::GetD3DKMTVideomemoryUsage(BOOL bShared, BOOL bDedicated, HANDLE hProcess)
{
	if (m_lpD3DKMTWrapper && m_lpD3DKMTDeviceDesc)
	{
		DWORD dwVideomemoryUsage			= VIDEOMEMORY_USAGE_INVALID;
		DWORD dwDedicatedVideomemoryUsage	= VIDEOMEMORY_USAGE_INVALID;
		DWORD dwSharedVideomemoryUsage		= VIDEOMEMORY_USAGE_INVALID;

		m_lpD3DKMTWrapper->GetVideomemoryUsage("", m_lpD3DKMTDeviceDesc->Bus, m_lpD3DKMTDeviceDesc->Dev, m_lpD3DKMTDeviceDesc->Fn, bDedicated ? &dwDedicatedVideomemoryUsage : NULL, bShared ? &dwSharedVideomemoryUsage : NULL, hProcess);

		if (dwDedicatedVideomemoryUsage != VIDEOMEMORY_USAGE_INVALID)
		{
			if (dwVideomemoryUsage == VIDEOMEMORY_USAGE_INVALID)
				dwVideomemoryUsage = dwDedicatedVideomemoryUsage;
			else
				dwVideomemoryUsage = dwVideomemoryUsage + dwDedicatedVideomemoryUsage;
		}

		if (dwSharedVideomemoryUsage != VIDEOMEMORY_USAGE_INVALID)
		{
			if (dwVideomemoryUsage == VIDEOMEMORY_USAGE_INVALID)
				dwVideomemoryUsage = dwSharedVideomemoryUsage;
			else
				dwVideomemoryUsage = dwVideomemoryUsage + dwSharedVideomemoryUsage;
		}

		return dwVideomemoryUsage;
	}

	return VIDEOMEMORY_USAGE_INVALID;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementation::GetD3DKMTTotalVideomemory(BOOL bShared, BOOL bDedicated)
{
	if (m_lpD3DKMTWrapper && m_lpD3DKMTDeviceDesc)
	{
		DWORD dwTotalVideomemory			= VIDEOMEMORY_USAGE_INVALID;
		DWORD dwTotalDedicatedVideomemory	= VIDEOMEMORY_USAGE_INVALID;
		DWORD dwTotalSharedVideomemory		= VIDEOMEMORY_USAGE_INVALID;

		m_lpD3DKMTWrapper->GetTotalVideomemory("", m_lpD3DKMTDeviceDesc->Bus, m_lpD3DKMTDeviceDesc->Dev, m_lpD3DKMTDeviceDesc->Fn, bDedicated ? &dwTotalDedicatedVideomemory : NULL, bShared ? &dwTotalSharedVideomemory : NULL);

		if (dwTotalDedicatedVideomemory != VIDEOMEMORY_USAGE_INVALID)
		{
			if (dwTotalVideomemory == VIDEOMEMORY_USAGE_INVALID)
				dwTotalVideomemory = dwTotalDedicatedVideomemory;
			else
				dwTotalVideomemory = dwTotalVideomemory + dwTotalDedicatedVideomemory;
		}

		if (dwTotalSharedVideomemory != VIDEOMEMORY_USAGE_INVALID)
		{
			if (dwTotalVideomemory == VIDEOMEMORY_USAGE_INVALID)
				dwTotalVideomemory = dwTotalSharedVideomemory;
			else
				dwTotalVideomemory = dwTotalVideomemory + dwTotalSharedVideomemory;
		}

		return dwTotalVideomemory;
	}

	return VIDEOMEMORY_USAGE_INVALID;
}
//////////////////////////////////////////////////////////////////////
LPTIMESTAMPED_DATA CHALImplementation::SetCacheData(DWORD dwKey, DWORD dwData)
{
	return m_cache.SetData(dwKey, m_dwTimestamp, dwData);
}
//////////////////////////////////////////////////////////////////////
LPTIMESTAMPED_DATA CHALImplementation::GetCacheData(DWORD dwKey)
{
	return m_cache.GetData(dwKey, m_dwTimestamp);
}
//////////////////////////////////////////////////////////////////////
float CHALImplementation::GetCurrentVideomemoryUsagePercent()
{
	DWORD dwUsage = GetCurrentVideomemoryUsage();
	DWORD dwTotal = GetTotalVideomemory();

	if (dwTotal)
		return 100.0f * dwUsage / dwTotal;

	return 0.0f;
}
//////////////////////////////////////////////////////////////////////
DWORD CHALImplementation::GetProcessVideomemoryUsage()
{
	DWORD dwUsage = 0;

/*
	//We could use D3DKMT to read process videomemory usage directly, but it won't work for the applications protected with
	//BattlEye/EAC due to restricted OpenProcess, so commented part is provided as an example of alternate inplace implementation 
	//for third party developers
	//We'll use RTSS process specific performance counters access interface instead, it will do the same in context of protected process

	HANDLE hProcess = OpenForegroundProcess();

	if (hProcess)
	{
		dwUsage = GetD3DKMTVideomemoryUsage(FALSE, TRUE, hProcess);

		CloseHandle(hProcess);
	}
*/

	DWORD dwBus			= 0;
	DWORD dwDev			= 0;
	DWORD dwFn			= 0;
	GetLocation(&dwBus,  &dwDev, &dwFn);

	CRTSSSharedMemoryInterface sharedMemoryInterface;

	sharedMemoryInterface.GetProcessPerfCounter(0, 1000, PROCESS_PERF_COUNTER_ID_D3DKMT_VRAM_USAGE_LOCAL, (dwBus<<16) | (dwDev<<8) | dwFn, &dwUsage);

	return dwUsage;
}
//////////////////////////////////////////////////////////////////////
float CHALImplementation::GetProcessVideomemoryUsagePercent()
{
	DWORD dwUsage = GetProcessVideomemoryUsage();

	if (dwUsage != VIDEOMEMORY_USAGE_INVALID)
	{
		DWORD dwTotal = GetTotalVideomemory();

		if (dwTotal)
			return 100.0f * dwUsage / dwTotal;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
