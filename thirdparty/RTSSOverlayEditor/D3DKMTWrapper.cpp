// D3DKMTWrapper.cpp: implementation of the CD3DKMTWrapper class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "D3DKMTWrapper.h"
#include "Log.h"
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#pragma warning (disable: 4996) 
//////////////////////////////////////////////////////////////////////
CD3DKMTWrapper::CD3DKMTWrapper()
{
	m_osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&m_osVersion);
}
//////////////////////////////////////////////////////////////////////
#pragma warning (default: 4996) 
//////////////////////////////////////////////////////////////////////
CD3DKMTWrapper::~CD3DKMTWrapper()
{
}
//////////////////////////////////////////////////////////////////////
CD3DKMTDeviceList* CD3DKMTWrapper::GetDeviceList()
{
	return &m_deviceList;
}
//////////////////////////////////////////////////////////////////////
BOOL CD3DKMTWrapper::Init()
{
	APPEND_LOG("Initializing D3DKMT wrapper");

	if (m_interface.Init())
	{
		m_deviceList.Init();

		APPEND_LOG1("%d D3DKMT devices detected", m_deviceList.GetCount());

		POSITION pos = m_deviceList.GetHeadPosition();

		DWORD dwDevice = 0;

		while (pos)
		{
			LPD3DKMTDEVICE_DESC lpDesc = m_deviceList.GetNext(pos);

			if (strlen(lpDesc->DeviceDesc))
			{
				APPEND_LOG6("Device %d on bus %d, dev %d, fn %d : %s, %s", dwDevice, lpDesc->Bus, lpDesc->Dev, lpDesc->Fn, lpDesc->HardwareID, lpDesc->DeviceDesc);
			}
			else
			{
				APPEND_LOG5("Device %d on bus %d, dev %d, fn %d : %s", dwDevice, lpDesc->Bus, lpDesc->Dev, lpDesc->Fn, lpDesc->DeviceName);
			}


			dwDevice++;
		}

		return TRUE;
	}
	else
	{
		APPEND_LOG("Failed to initialize D3DKMT interface");
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetVideomemoryUsage(LPCSTR lpDisplayName, DWORD dwBus, DWORD dwDev, DWORD dwFn, LPDWORD lpDedicatedVideomemoryUsage, LPDWORD lpSharedVideomemoryUsage, HANDLE hProcess)
{
	POSITION pos = m_deviceList.GetHeadPosition();

	while (pos)
	{
		LPD3DKMTDEVICE_DESC lpDesc = m_deviceList.GetNext(pos);

		if ((dwBus == lpDesc->Bus) &&
			(dwDev == lpDesc->Dev) &&
			(dwFn  == lpDesc->Fn))
		{
			GetVideomemoryUsageByDeviceName(lpDesc->DeviceName, lpDedicatedVideomemoryUsage, lpSharedVideomemoryUsage, hProcess);

			return;
		}
	}

	GetVideomemoryUsageByGdiDisplayName(lpDisplayName, lpDedicatedVideomemoryUsage, lpSharedVideomemoryUsage, hProcess);
}
//////////////////////////////////////////////////////////////////////
DWORD CD3DKMTWrapper::GetStatisticsBlockSize(_D3DKMT_STATISTICS_VISTA* pStatistics, DWORD dwIndex)
{
	if (pStatistics->Reserved[dwIndex] == 1)
		return pStatistics->Reserved[dwIndex + 1];

	return 0;
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetVideomemoryUsageVista(D3DKMT_HANDLE hAdapter, LPDWORD lpDedicatedVideomemoryUsage, LPDWORD /*lpSharedVideomemoryUsage*/, HANDLE /*hProcess*/)
{
	_D3DKMT_QUERYSTATISTICS_VISTA queryStatistics0;
	ZeroMemory(&queryStatistics0, sizeof(queryStatistics0));

	_D3DKMT_STATISTICS_VISTA statistics;
	ZeroMemory(&statistics, sizeof(statistics));

	queryStatistics0.Type						= D3DKMT_QUERYSTATISTICS_ADAPTER;
	queryStatistics0.pBuffer					= &statistics;
	queryStatistics0.BufferSize					= 32;
	queryStatistics0.u.QueryAdapter.hAdapter	= hAdapter;

	DWORDLONG qwDedicatedVideomemoryUsage		= VIDEOMEMORY_USAGE_INVALID64;

	if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatistics0)))
	{
		DWORD dwBufferSize = GetStatisticsBlockSize(&statistics, 0);

		if (dwBufferSize <= sizeof(statistics))
		{
			queryStatistics0.BufferSize = dwBufferSize;

			if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatistics0)))
			{
				DWORD dwIndex			= 8;
				DWORD dwHeaderBlockSize = GetStatisticsBlockSize(&statistics, dwIndex);

				if (dwHeaderBlockSize)
				{
					dwIndex = dwIndex + (dwHeaderBlockSize>>2);

					for (DWORD dwItem=0; dwItem<statistics.Reserved[4]; dwItem++)
					{
						DWORDLONG* lpVideomemoryUsage = &qwDedicatedVideomemoryUsage;

						if (lpVideomemoryUsage)
						{
							if (*lpVideomemoryUsage == VIDEOMEMORY_USAGE_INVALID64)
								*lpVideomemoryUsage = statistics.Reserved[dwIndex + 4];
							else
								*lpVideomemoryUsage = *lpVideomemoryUsage + statistics.Reserved[dwIndex + 4];
						}

						DWORD dwItemBlockSize = GetStatisticsBlockSize(&statistics, dwIndex);

						if (dwItemBlockSize)
							dwIndex = dwIndex + (dwItemBlockSize>>2);
						else
							break;
					}
				}
			}
		}
	}

	if (lpDedicatedVideomemoryUsage)
	{
		if (qwDedicatedVideomemoryUsage != VIDEOMEMORY_USAGE_INVALID64)
			*lpDedicatedVideomemoryUsage = (DWORD)(qwDedicatedVideomemoryUsage>>10L);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetVideomemoryUsageWin7(LUID AdapterLuid, LPDWORD lpDedicatedVideomemoryUsage, LPDWORD lpSharedVideomemoryUsage, HANDLE hProcess)
{
	D3DKMT_QUERYSTATISTICS queryStatisticsAdapter;
	ZeroMemory(&queryStatisticsAdapter, sizeof(queryStatisticsAdapter));

	queryStatisticsAdapter.Type				= D3DKMT_QUERYSTATISTICS_ADAPTER;
	queryStatisticsAdapter.AdapterLuid		= AdapterLuid;

	DWORDLONG qwDedicatedVideomemoryUsage	= VIDEOMEMORY_USAGE_INVALID64;
	DWORDLONG qwSharedVideomemoryUsage		= VIDEOMEMORY_USAGE_INVALID64;
 
	if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatisticsAdapter)))
	{
		for (DWORD dwSegment=0; dwSegment<queryStatisticsAdapter.QueryResult.AdapterInformation.NbSegments; dwSegment++)
		{
			D3DKMT_QUERYSTATISTICS queryStatisticsSegment;
			ZeroMemory(&queryStatisticsSegment, sizeof(queryStatisticsSegment));

			queryStatisticsSegment.Type						= D3DKMT_QUERYSTATISTICS_SEGMENT;
			queryStatisticsSegment.AdapterLuid				= AdapterLuid;
			queryStatisticsSegment.QuerySegment.SegmentId	= dwSegment;

			if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatisticsSegment)))
			{
				DWORDLONG* lpVideomemoryUsage = NULL;

				switch (queryStatisticsSegment.QueryResult.SegmentInformationV1.Aperture)
				{
				case 0:
					lpVideomemoryUsage = &qwDedicatedVideomemoryUsage;
					break;
				case 1:
					lpVideomemoryUsage = &qwSharedVideomemoryUsage;
					break;
				}

				if (hProcess)
				{
					D3DKMT_QUERYSTATISTICS queryStatisticsProcessSegment;
					ZeroMemory(&queryStatisticsProcessSegment, sizeof(queryStatisticsProcessSegment));

					queryStatisticsProcessSegment.Type							= D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT;
					queryStatisticsProcessSegment.hProcess						= hProcess;
					queryStatisticsProcessSegment.AdapterLuid					= AdapterLuid;
					queryStatisticsProcessSegment.QueryProcessSegment.SegmentId	= dwSegment;

					if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatisticsProcessSegment)))
					{
						if (lpVideomemoryUsage)
						{
							if (*lpVideomemoryUsage == VIDEOMEMORY_USAGE_INVALID64)
								*lpVideomemoryUsage = queryStatisticsProcessSegment.QueryResult.ProcessSegmentInformation.BytesCommitted;
							else
								*lpVideomemoryUsage = *lpVideomemoryUsage + queryStatisticsProcessSegment.QueryResult.ProcessSegmentInformation.BytesCommitted;
						}
					}
				}
				else
				{
					if (lpVideomemoryUsage)
					{
						if (*lpVideomemoryUsage == VIDEOMEMORY_USAGE_INVALID64)
							*lpVideomemoryUsage = queryStatisticsSegment.QueryResult.SegmentInformationV1.BytesResident;
						else
							*lpVideomemoryUsage = *lpVideomemoryUsage + queryStatisticsSegment.QueryResult.SegmentInformationV1.BytesResident;
					}
				}
			}
		}
	}

	if (lpDedicatedVideomemoryUsage)
	{
		if (qwDedicatedVideomemoryUsage != VIDEOMEMORY_USAGE_INVALID64)
			*lpDedicatedVideomemoryUsage = (DWORD)(qwDedicatedVideomemoryUsage>>10L);
	}

	if (lpSharedVideomemoryUsage)
	{
		if (qwSharedVideomemoryUsage != VIDEOMEMORY_USAGE_INVALID64)
			*lpSharedVideomemoryUsage = (DWORD)(qwSharedVideomemoryUsage>>10L);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetVideomemoryUsageWin8(LUID AdapterLuid, LPDWORD lpDedicatedVideomemoryUsage, LPDWORD lpSharedVideomemoryUsage, HANDLE hProcess)
{
	D3DKMT_QUERYSTATISTICS queryStatisticsAdapter;
	ZeroMemory(&queryStatisticsAdapter, sizeof(queryStatisticsAdapter));

	queryStatisticsAdapter.Type				= D3DKMT_QUERYSTATISTICS_ADAPTER;
	queryStatisticsAdapter.AdapterLuid		= AdapterLuid;

	DWORDLONG qwDedicatedVideomemoryUsage	= VIDEOMEMORY_USAGE_INVALID64;
	DWORDLONG qwSharedVideomemoryUsage		= VIDEOMEMORY_USAGE_INVALID64;

	if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatisticsAdapter)))
	{
		for (DWORD dwSegment=0; dwSegment<queryStatisticsAdapter.QueryResult.AdapterInformation.NbSegments; dwSegment++)
		{
			D3DKMT_QUERYSTATISTICS queryStatisticsSegment;
			ZeroMemory(&queryStatisticsSegment, sizeof(queryStatisticsSegment));

			queryStatisticsSegment.Type						= D3DKMT_QUERYSTATISTICS_SEGMENT;
			queryStatisticsSegment.AdapterLuid				= AdapterLuid;
			queryStatisticsSegment.QuerySegment.SegmentId	= dwSegment;

			if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatisticsSegment)))
			{
				DWORDLONG* lpVideomemoryUsage = NULL;

				switch (queryStatisticsSegment.QueryResult.SegmentInformation.Aperture)
				{
				case 0:
					lpVideomemoryUsage = &qwDedicatedVideomemoryUsage;
					break;
				case 1:
					lpVideomemoryUsage = &qwSharedVideomemoryUsage;
					break;
				}

				if (hProcess)
				{
					D3DKMT_QUERYSTATISTICS queryStatisticsProcessSegment;
					ZeroMemory(&queryStatisticsProcessSegment, sizeof(queryStatisticsProcessSegment));

					queryStatisticsProcessSegment.Type							= D3DKMT_QUERYSTATISTICS_PROCESS_SEGMENT;
					queryStatisticsProcessSegment.hProcess						= hProcess;
					queryStatisticsProcessSegment.AdapterLuid					= AdapterLuid;
					queryStatisticsProcessSegment.QueryProcessSegment.SegmentId	= dwSegment;

					if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatisticsProcessSegment)))
					{
						if (lpVideomemoryUsage)
						{
							if (*lpVideomemoryUsage == VIDEOMEMORY_USAGE_INVALID64)
								*lpVideomemoryUsage = queryStatisticsProcessSegment.QueryResult.ProcessSegmentInformation.BytesCommitted;
							else
								*lpVideomemoryUsage = *lpVideomemoryUsage + queryStatisticsProcessSegment.QueryResult.ProcessSegmentInformation.BytesCommitted;
						}
					}
				}
				else
				{
					if (lpVideomemoryUsage)
					{
						if (*lpVideomemoryUsage == VIDEOMEMORY_USAGE_INVALID64)
							*lpVideomemoryUsage = queryStatisticsSegment.QueryResult.SegmentInformation.BytesResident;
						else
							*lpVideomemoryUsage = *lpVideomemoryUsage + queryStatisticsSegment.QueryResult.SegmentInformation.BytesResident;
					}
				}
			}
		}
	}

	if (lpDedicatedVideomemoryUsage)
	{
		if (qwDedicatedVideomemoryUsage != VIDEOMEMORY_USAGE_INVALID64)
			*lpDedicatedVideomemoryUsage = (DWORD)(qwDedicatedVideomemoryUsage>>10L);
	}

	if (lpSharedVideomemoryUsage)
	{
		if (qwSharedVideomemoryUsage != VIDEOMEMORY_USAGE_INVALID64)
			*lpSharedVideomemoryUsage = (DWORD)(qwSharedVideomemoryUsage>>10L);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetGpuRunningTimeVista(D3DKMT_HANDLE hAdapter, LARGE_INTEGER* lpNodeRunningTimeArr, LPDWORD lpNodeCount)
{
	_D3DKMT_QUERYSTATISTICS_VISTA queryStatistics0;
	ZeroMemory(&queryStatistics0, sizeof(queryStatistics0));

	_D3DKMT_STATISTICS_VISTA statistics;
	ZeroMemory(&statistics, sizeof(statistics));

	queryStatistics0.Type						= D3DKMT_QUERYSTATISTICS_ADAPTER;
	queryStatistics0.pBuffer					= &statistics;
	queryStatistics0.BufferSize					= 32;
	queryStatistics0.u.QueryAdapter.hAdapter	= hAdapter;

	if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatistics0)))
	{
		DWORD dwBufferSize = GetStatisticsBlockSize(&statistics, 0);

		if (dwBufferSize <= sizeof(statistics))
		{
			queryStatistics0.BufferSize = dwBufferSize;

			if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatistics0)))
			{
				DWORD dwIndex			= 8;
				DWORD dwHeaderBlockSize = GetStatisticsBlockSize(&statistics, dwIndex);

				if (dwHeaderBlockSize)
				{
					dwIndex = dwIndex + (dwHeaderBlockSize>>2);

					for (DWORD dwItem=0; dwItem<statistics.Reserved[4]; dwItem++)
					{
						DWORD dwItemBlockSize = GetStatisticsBlockSize(&statistics, dwIndex);

						if (dwItemBlockSize)
							dwIndex = dwIndex + (dwItemBlockSize>>2);
						else
							break;
					}

					for (DWORD dwItem=0; dwItem<statistics.Reserved[5]; dwItem++)
					{
						LARGE_INTEGER qwNodeRunningTime;

						qwNodeRunningTime.LowPart	= statistics.Reserved[dwIndex + 6];
						qwNodeRunningTime.HighPart	= statistics.Reserved[dwIndex + 7];

						if (dwItem < *lpNodeCount)
							lpNodeRunningTimeArr[dwItem] = qwNodeRunningTime;

						dwIndex = dwIndex + 50;
					}

					*lpNodeCount = statistics.Reserved[5];
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetGpuRunningTimeWin7(LUID AdapterLuid, LARGE_INTEGER* lpNodeRunningTimeArr, LPDWORD lpNodeCount)
{
	D3DKMT_QUERYSTATISTICS queryStatisticsAdapter;
	ZeroMemory(&queryStatisticsAdapter, sizeof(queryStatisticsAdapter));

	queryStatisticsAdapter.Type				= D3DKMT_QUERYSTATISTICS_ADAPTER;
	queryStatisticsAdapter.AdapterLuid		= AdapterLuid;

	if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatisticsAdapter)))
	{
		for (DWORD dwNode=0; dwNode<queryStatisticsAdapter.QueryResult.AdapterInformation.NodeCount; dwNode++)
		{
			D3DKMT_QUERYSTATISTICS queryStatisticsNode;
			ZeroMemory(&queryStatisticsNode, sizeof(queryStatisticsNode));

			queryStatisticsNode.Type					= D3DKMT_QUERYSTATISTICS_NODE;
			queryStatisticsNode.AdapterLuid				= AdapterLuid;
			queryStatisticsNode.QueryNode.NodeId		= dwNode;

			if (SUCCEEDED(m_interface.D3DKMTQueryStatistics(&queryStatisticsNode)))
			{
				if (dwNode < *lpNodeCount)
					lpNodeRunningTimeArr[dwNode] = queryStatisticsNode.QueryResult.NodeInformation.GlobalInformation.RunningTime;
			}
		}

		*lpNodeCount = queryStatisticsAdapter.QueryResult.AdapterInformation.NodeCount;
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetGpuRunningTimeWin8(LUID AdapterLuid, LARGE_INTEGER* lpNodeRunningTimeArr, LPDWORD lpNodeCount)
{
	GetGpuRunningTimeWin7(AdapterLuid, lpNodeRunningTimeArr, lpNodeCount);
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetVideomemoryUsageByGdiDisplayName(LPCSTR lpDisplayName, LPDWORD lpDedicatedVideomemoryUsage, LPDWORD lpSharedVideomemoryUsage, HANDLE hProcess)
{
	if (lpDedicatedVideomemoryUsage)
		*lpDedicatedVideomemoryUsage = VIDEOMEMORY_USAGE_INVALID;

	if (lpSharedVideomemoryUsage)
		*lpSharedVideomemoryUsage = VIDEOMEMORY_USAGE_INVALID;

	_D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME openAdapter;
	ZeroMemory(&openAdapter, sizeof(openAdapter));

	size_t result;
	mbstowcs_s(&result, openAdapter.DeviceName, sizeof(openAdapter.DeviceName)/sizeof(openAdapter.DeviceName[0]), lpDisplayName, strlen(lpDisplayName));   
	
	if (SUCCEEDED(m_interface.D3DKMTOpenAdapterFromGdiDisplayName(&openAdapter)))
	{
		if (m_osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if ((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion == 0))
				GetVideomemoryUsageVista(openAdapter.hAdapter, lpDedicatedVideomemoryUsage, lpSharedVideomemoryUsage, hProcess);
			else
				if ((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion == 1))
					GetVideomemoryUsageWin7(openAdapter.AdapterLuid, lpDedicatedVideomemoryUsage, lpSharedVideomemoryUsage, hProcess);
				else
					if (((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion >= 2)) || (m_osVersion.dwMajorVersion > 6))
						GetVideomemoryUsageWin8(openAdapter.AdapterLuid, lpDedicatedVideomemoryUsage, lpSharedVideomemoryUsage, hProcess);
		}

		_D3DKMT_CLOSEADAPTER closeAdapter;
		ZeroMemory(&closeAdapter, sizeof(closeAdapter));

		closeAdapter.hAdapter = openAdapter.hAdapter;

		m_interface.D3DKMTCloseAdapter(&closeAdapter);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetVideomemoryUsageByDeviceName(LPCSTR lpDeviceName, LPDWORD lpDedicatedVideomemoryUsage, LPDWORD lpSharedVideomemoryUsage, HANDLE hProcess)
{
	if (lpDedicatedVideomemoryUsage)
		*lpDedicatedVideomemoryUsage = VIDEOMEMORY_USAGE_INVALID;

	if (lpSharedVideomemoryUsage)
		*lpSharedVideomemoryUsage = VIDEOMEMORY_USAGE_INVALID;

	WCHAR DeviceName[MAX_PATH] = { 0 };
	
	size_t result;
	mbstowcs_s(&result, DeviceName, sizeof(DeviceName)/sizeof(DeviceName[0]), lpDeviceName, strlen(lpDeviceName));   

	_D3DKMT_OPENADAPTERFROMDEVICENAME openAdapter;
	ZeroMemory(&openAdapter, sizeof(openAdapter));

	openAdapter.pDeviceName = DeviceName;

	if (SUCCEEDED(m_interface.D3DKMTOpenAdapterFromDeviceName(&openAdapter)))
	{
		if (m_osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if ((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion == 0))
				GetVideomemoryUsageVista(openAdapter.hAdapter, lpDedicatedVideomemoryUsage, lpSharedVideomemoryUsage, hProcess);
			else
				if ((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion == 1))
					GetVideomemoryUsageWin7(openAdapter.AdapterLuid, lpDedicatedVideomemoryUsage, lpSharedVideomemoryUsage, hProcess);
				else
					if (((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion >= 2)) || (m_osVersion.dwMajorVersion > 6))
						GetVideomemoryUsageWin8(openAdapter.AdapterLuid, lpDedicatedVideomemoryUsage, lpSharedVideomemoryUsage, hProcess);
		}

		_D3DKMT_CLOSEADAPTER closeAdapter;
		ZeroMemory(&closeAdapter, sizeof(closeAdapter));

		closeAdapter.hAdapter = openAdapter.hAdapter;

		m_interface.D3DKMTCloseAdapter(&closeAdapter);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetGpuRunningTime(LPCSTR lpDisplayName, DWORD dwBus, DWORD dwDev, DWORD dwFn, LARGE_INTEGER* lpNodeRunningTimeArr, LPDWORD lpNodeCount)
{
	POSITION pos = m_deviceList.GetHeadPosition();

	while (pos)
	{
		LPD3DKMTDEVICE_DESC lpDesc = m_deviceList.GetNext(pos);

		if ((dwBus == lpDesc->Bus) &&
			(dwDev == lpDesc->Dev) &&
			(dwFn  == lpDesc->Fn))
		{
			GetGpuRunningTimeByDeviceName(lpDesc->DeviceName, lpNodeRunningTimeArr, lpNodeCount);

			return;
		}
	}

	GetGpuRunningTimeByGdiDisplayName(lpDisplayName, lpNodeRunningTimeArr, lpNodeCount);
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetGpuRunningTimeByGdiDisplayName(LPCSTR lpDisplayName, LARGE_INTEGER* lpNodeRunningTimeArr, LPDWORD lpNodeCount)
{
	_D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME openAdapter;
	ZeroMemory(&openAdapter, sizeof(openAdapter));

	size_t result;
	mbstowcs_s(&result, openAdapter.DeviceName, sizeof(openAdapter.DeviceName)/sizeof(openAdapter.DeviceName[0]), lpDisplayName, strlen(lpDisplayName));   
	
	if (SUCCEEDED(m_interface.D3DKMTOpenAdapterFromGdiDisplayName(&openAdapter)))
	{
		if (m_osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if ((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion == 0))
				GetGpuRunningTimeVista(openAdapter.hAdapter, lpNodeRunningTimeArr, lpNodeCount);
			else
				if ((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion == 1))
					GetGpuRunningTimeWin7(openAdapter.AdapterLuid, lpNodeRunningTimeArr, lpNodeCount);
				else
					if (((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion >= 2)) || (m_osVersion.dwMajorVersion > 6))
						GetGpuRunningTimeWin8(openAdapter.AdapterLuid, lpNodeRunningTimeArr, lpNodeCount);
		}

		_D3DKMT_CLOSEADAPTER closeAdapter;
		ZeroMemory(&closeAdapter, sizeof(closeAdapter));

		closeAdapter.hAdapter = openAdapter.hAdapter;

		m_interface.D3DKMTCloseAdapter(&closeAdapter);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetGpuRunningTimeByDeviceName(LPCSTR lpDeviceName, LARGE_INTEGER* lpNodeRunningTimeArr, LPDWORD lpNodeCount)
{
	WCHAR DeviceName[MAX_PATH] = { 0 };
	
	size_t result;
	mbstowcs_s(&result, DeviceName, sizeof(DeviceName)/sizeof(DeviceName[0]), lpDeviceName, strlen(lpDeviceName));   

	_D3DKMT_OPENADAPTERFROMDEVICENAME openAdapter;
	ZeroMemory(&openAdapter, sizeof(openAdapter));

	openAdapter.pDeviceName = DeviceName;

	if (SUCCEEDED(m_interface.D3DKMTOpenAdapterFromDeviceName(&openAdapter)))
	{
		if (m_osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if ((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion == 0))
				GetGpuRunningTimeVista(openAdapter.hAdapter, lpNodeRunningTimeArr, lpNodeCount);
			else
				if ((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion == 1))
					GetGpuRunningTimeWin7(openAdapter.AdapterLuid, lpNodeRunningTimeArr, lpNodeCount);
				else
					if (((m_osVersion.dwMajorVersion == 6) && (m_osVersion.dwMinorVersion >= 2)) || (m_osVersion.dwMajorVersion > 6))
						GetGpuRunningTimeWin8(openAdapter.AdapterLuid, lpNodeRunningTimeArr, lpNodeCount);
		}

		_D3DKMT_CLOSEADAPTER closeAdapter;
		ZeroMemory(&closeAdapter, sizeof(closeAdapter));

		closeAdapter.hAdapter = openAdapter.hAdapter;

		m_interface.D3DKMTCloseAdapter(&closeAdapter);
	}
}
//////////////////////////////////////////////////////////////////////
LPD3DKMTDEVICE_DESC CD3DKMTWrapper::FindDevice(DWORD dwBus, DWORD dwDev, DWORD dwFn)
{
	POSITION pos = m_deviceList.GetHeadPosition();

	while (pos)
	{
		LPD3DKMTDEVICE_DESC lpDesc = m_deviceList.GetNext(pos);

		if ((dwBus == lpDesc->Bus) &&
			(dwDev == lpDesc->Dev) &&
			(dwFn  == lpDesc->Fn))
			return lpDesc;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetLUIDByDeviceName(LPCSTR lpDeviceName, LUID* lpLUID)
{
	WCHAR DeviceName[MAX_PATH] = { 0 };
	
	size_t result;
	mbstowcs_s(&result, DeviceName, sizeof(DeviceName)/sizeof(DeviceName[0]), lpDeviceName, strlen(lpDeviceName));   

	_D3DKMT_OPENADAPTERFROMDEVICENAME openAdapter;
	ZeroMemory(&openAdapter, sizeof(openAdapter));

	openAdapter.pDeviceName = DeviceName;

	if (SUCCEEDED(m_interface.D3DKMTOpenAdapterFromDeviceName(&openAdapter)))
	{
		memcpy(lpLUID, &openAdapter.AdapterLuid, sizeof(LUID));

		_D3DKMT_CLOSEADAPTER closeAdapter;
		ZeroMemory(&closeAdapter, sizeof(closeAdapter));

		closeAdapter.hAdapter = openAdapter.hAdapter;

		m_interface.D3DKMTCloseAdapter(&closeAdapter);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetLUIDByGdiDisplayName(LPCSTR lpDisplayName, LUID* lpLUID)
{
	_D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME openAdapter;
	ZeroMemory(&openAdapter, sizeof(openAdapter));

	size_t result;
	mbstowcs_s(&result, openAdapter.DeviceName, sizeof(openAdapter.DeviceName)/sizeof(openAdapter.DeviceName[0]), lpDisplayName, strlen(lpDisplayName));   

	if (SUCCEEDED(m_interface.D3DKMTOpenAdapterFromGdiDisplayName(&openAdapter)))
	{
		memcpy(lpLUID, &openAdapter.AdapterLuid, sizeof(LUID));

		_D3DKMT_CLOSEADAPTER closeAdapter;
		ZeroMemory(&closeAdapter, sizeof(closeAdapter));

		closeAdapter.hAdapter = openAdapter.hAdapter;

		m_interface.D3DKMTCloseAdapter(&closeAdapter);
	}
}
//////////////////////////////////////////////////////////////////////
LPD3DKMTDEVICE_DESC CD3DKMTWrapper::FindDevice(LUID luid)
{
	POSITION pos = m_deviceList.GetHeadPosition();

	while (pos)
	{
		LPD3DKMTDEVICE_DESC lpDesc = m_deviceList.GetNext(pos);

		LUID curLuid;
		GetLUIDByDeviceName(lpDesc->DeviceName, &curLuid);

		if (!memcmp(&luid, &curLuid, sizeof(LUID)))
			return lpDesc;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetTotalVideomemoryByDeviceName(LPCSTR lpDeviceName, LPDWORD lpTotalDedicatedVideomemory, LPDWORD lpTotalSharedVideomemory)
{
	if (lpTotalDedicatedVideomemory)
		*lpTotalDedicatedVideomemory = VIDEOMEMORY_USAGE_INVALID;

	if (lpTotalSharedVideomemory)
		*lpTotalSharedVideomemory = VIDEOMEMORY_USAGE_INVALID;

	WCHAR DeviceName[MAX_PATH] = { 0 };
	
	size_t result;
	mbstowcs_s(&result, DeviceName, sizeof(DeviceName)/sizeof(DeviceName[0]), lpDeviceName, strlen(lpDeviceName));   

	_D3DKMT_OPENADAPTERFROMDEVICENAME openAdapter;
	ZeroMemory(&openAdapter, sizeof(openAdapter));

	openAdapter.pDeviceName = DeviceName;

	if (SUCCEEDED(m_interface.D3DKMTOpenAdapterFromDeviceName(&openAdapter)))
	{
		D3DKMT_SEGMENTSIZEINFO segmentSizeInfo;
		ZeroMemory(&segmentSizeInfo, sizeof(segmentSizeInfo));

		D3DKMT_QUERYADAPTERINFO queryAdapterInfo;
		ZeroMemory(&queryAdapterInfo, sizeof(queryAdapterInfo));
		queryAdapterInfo.hAdapter				= openAdapter.hAdapter;
		queryAdapterInfo.Type					= 3;
		queryAdapterInfo.pPrivateDriverData		= &segmentSizeInfo;
		queryAdapterInfo.PrivateDriverDataSize	= sizeof(segmentSizeInfo);

		if (SUCCEEDED(m_interface.D3DKMTQueryAdapterInfo(&queryAdapterInfo)))
		{
			if (lpTotalDedicatedVideomemory)
				*lpTotalDedicatedVideomemory = (DWORD)(segmentSizeInfo.DedicatedVideoMemorySize>>10L);

			if (lpTotalSharedVideomemory)
				*lpTotalSharedVideomemory = (DWORD)(segmentSizeInfo.SharedSystemMemorySize>>10L);
		}

		_D3DKMT_CLOSEADAPTER closeAdapter;
		ZeroMemory(&closeAdapter, sizeof(closeAdapter));

		closeAdapter.hAdapter = openAdapter.hAdapter;

		m_interface.D3DKMTCloseAdapter(&closeAdapter);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetTotalVideomemoryByGdiDisplayName(LPCSTR lpDisplayName, LPDWORD lpTotalDedicatedVideomemory, LPDWORD lpTotalSharedVideomemory)
{
	if (lpTotalDedicatedVideomemory)
		*lpTotalDedicatedVideomemory = VIDEOMEMORY_USAGE_INVALID;

	if (lpTotalSharedVideomemory)
		*lpTotalSharedVideomemory = VIDEOMEMORY_USAGE_INVALID;

	_D3DKMT_OPENADAPTERFROMGDIDISPLAYNAME openAdapter;
	ZeroMemory(&openAdapter, sizeof(openAdapter));

	size_t result;
	mbstowcs_s(&result, openAdapter.DeviceName, sizeof(openAdapter.DeviceName)/sizeof(openAdapter.DeviceName[0]), lpDisplayName, strlen(lpDisplayName));   

	if (SUCCEEDED(m_interface.D3DKMTOpenAdapterFromGdiDisplayName(&openAdapter)))
	{
		D3DKMT_SEGMENTSIZEINFO segmentSizeInfo;
		ZeroMemory(&segmentSizeInfo, sizeof(segmentSizeInfo));

		D3DKMT_QUERYADAPTERINFO queryAdapterInfo;
		ZeroMemory(&queryAdapterInfo, sizeof(queryAdapterInfo));
		queryAdapterInfo.hAdapter				= openAdapter.hAdapter;
		queryAdapterInfo.Type					= 3;
		queryAdapterInfo.pPrivateDriverData		= &segmentSizeInfo;
		queryAdapterInfo.PrivateDriverDataSize	= sizeof(segmentSizeInfo);

		if (SUCCEEDED(m_interface.D3DKMTQueryAdapterInfo(&queryAdapterInfo)))
		{
			if (lpTotalDedicatedVideomemory)
				*lpTotalDedicatedVideomemory = (DWORD)(segmentSizeInfo.DedicatedVideoMemorySize>>10L);

			if (lpTotalSharedVideomemory)
				*lpTotalSharedVideomemory = (DWORD)(segmentSizeInfo.SharedSystemMemorySize>>10L);
		}

		_D3DKMT_CLOSEADAPTER closeAdapter;
		ZeroMemory(&closeAdapter, sizeof(closeAdapter));

		closeAdapter.hAdapter = openAdapter.hAdapter;

		m_interface.D3DKMTCloseAdapter(&closeAdapter);
	}
}
//////////////////////////////////////////////////////////////////////
void CD3DKMTWrapper::GetTotalVideomemory(LPCSTR lpDisplayName, DWORD dwBus, DWORD dwDev, DWORD dwFn, LPDWORD lpTotalDedicatedVideomemory, LPDWORD lpTotalSharedVideomemory)
{
	POSITION pos = m_deviceList.GetHeadPosition();

	while (pos)
	{
		LPD3DKMTDEVICE_DESC lpDesc = m_deviceList.GetNext(pos);

		if ((dwBus == lpDesc->Bus) &&
			(dwDev == lpDesc->Dev) &&
			(dwFn  == lpDesc->Fn))
		{
			GetTotalVideomemoryByDeviceName(lpDesc->DeviceName, lpTotalDedicatedVideomemory, lpTotalSharedVideomemory);

			return;
		}
	}

	GetTotalVideomemoryByGdiDisplayName(lpDisplayName, lpTotalDedicatedVideomemory, lpTotalSharedVideomemory);
}
//////////////////////////////////////////////////////////////////////

