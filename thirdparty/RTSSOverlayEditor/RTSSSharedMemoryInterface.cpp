// RTSSSharedMemoryInterface.cpp: implementation of the CRTSSSharedMemoryInterface class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RTSSSharedMemory.h"
#include "RTSSSharedMemoryInterface.h"

#include <float.h>
//////////////////////////////////////////////////////////////////////
CRTSSSharedMemoryInterface::CRTSSSharedMemoryInterface()
{
}
//////////////////////////////////////////////////////////////////////
CRTSSSharedMemoryInterface::~CRTSSSharedMemoryInterface()
{
}
//////////////////////////////////////////////////////////////////////
BOOL CRTSSSharedMemoryInterface::UpdateOSD(LPCSTR lpOSDOwner, LPCSTR lpText, LPBYTE lpBuffer, DWORD dwBufferSize)
{
	BOOL bResult	= FALSE;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr				= MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		LPRTSS_SHARED_MEMORY pMem	= (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020000))
			{
				for (DWORD dwPass=0; dwPass<2; dwPass++)
					//1st pass : find previously captured OSD slot
					//2nd pass : otherwise find the first unused OSD slot and capture it
				{
					for (DWORD dwEntry=1; dwEntry<pMem->dwOSDArrSize; dwEntry++)
						//allow primary OSD clients (i.e. EVGA Precision / MSI Afterburner) to use the first slot exclusively, so third party
						//applications start scanning the slots from the second one
					{
						RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY)((LPBYTE)pMem + pMem->dwOSDArrOffset + dwEntry * pMem->dwOSDEntrySize);

						if (dwPass)
						{
							if (!strlen(pEntry->szOSDOwner))
								strcpy_s(pEntry->szOSDOwner, sizeof(pEntry->szOSDOwner), lpOSDOwner);
						}

						if (!strcmp(pEntry->szOSDOwner, lpOSDOwner))
						{
							if (pMem->dwVersion >= 0x00020007)
								//use extended text slot for v2.7 and higher shared memory, it allows displaying 4096 symbols
								//instead of 256 for regular text slot
							{
								if (pMem->dwVersion >= 0x0002000e)	
									//OSD locking is supported on v2.14 and higher shared memory
								{
									DWORD dwBusy = _interlockedbittestandset(&pMem->dwBusy, 0);
										//bit 0 of this variable will be set if OSD is locked by renderer and cannot be refreshed
										//at the moment

									if (dwBusy) 
										//if OSD is currently locked by renderer, we'll try to wait 100 ms to let 3D application finish rendering
									{
										DWORD dwTimestamp = GetTickCount();
			
										while (dwBusy)
										{
											dwBusy = _interlockedbittestandset(&pMem->dwBusy, 0);

											if (GetTickCount() - dwTimestamp > 100)
												//OSD stays locked for more than 100ms, that's not a normal case and most likely other OSD
												//client didn't unlock it properly or crashed, so we forcibly unlock it now
											{
												dwBusy = 0;

												break;
											}
										}
									}

									if (!dwBusy)
									{
										if (pMem->dwVersion >= 0x00020014)	
											//extended 32KB OSD text slot is supported on v2.20 and higher shared memory
											strncpy_s(pEntry->szOSDEx2, sizeof(pEntry->szOSDEx2), lpText, sizeof(pEntry->szOSDEx2) - 1);	
										else
											strncpy_s(pEntry->szOSDEx2, sizeof(pEntry->szOSDEx), lpText, sizeof(pEntry->szOSDEx) - 1);	

										if (lpBuffer && dwBufferSize)
											//copy embedded object buffer if it was specified
											CopyMemory(pEntry->buffer, lpBuffer, min(dwBufferSize, sizeof(pEntry->buffer)));

										pMem->dwBusy = 0;
									}
								}
								else
									strncpy_s(pEntry->szOSDEx, sizeof(pEntry->szOSDEx), lpText, sizeof(pEntry->szOSDEx) - 1);	

							}
							else
								strncpy_s(pEntry->szOSD, sizeof(pEntry->szOSD), lpText, sizeof(pEntry->szOSD) - 1);

							pMem->dwOSDFrame++;

							bResult = TRUE;

							break;
						}
					}

					if (bResult)
						break;
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
void CRTSSSharedMemoryInterface::ReleaseOSD(LPCSTR lpOSDOwner)
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		LPRTSS_SHARED_MEMORY pMem = (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020000))
			{
				for (DWORD dwEntry=1; dwEntry<pMem->dwOSDArrSize; dwEntry++)
				{
					RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY)((LPBYTE)pMem + pMem->dwOSDArrOffset + dwEntry * pMem->dwOSDEntrySize);

					if (!strcmp(pEntry->szOSDOwner, lpOSDOwner))
					{
						memset(pEntry, 0, pMem->dwOSDEntrySize);
						pMem->dwOSDFrame++;
					}
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CRTSSSharedMemoryInterface::EmbedGraph(LPCSTR lpOSDOwner, DWORD dwOffset, FLOAT* lpBuffer, DWORD dwBufferPos, DWORD dwBufferSize, LONG dwWidth, LONG dwHeight, LONG dwMargin, FLOAT fltMin, FLOAT fltMax, DWORD dwFlags)
{
	DWORD dwResult	= 0;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr				= MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		LPRTSS_SHARED_MEMORY pMem	= (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020000))
			{
				for (DWORD dwPass=0; dwPass<2; dwPass++)
					//1st pass : find previously captured OSD slot
					//2nd pass : otherwise find the first unused OSD slot and capture it
				{
					for (DWORD dwEntry=1; dwEntry<pMem->dwOSDArrSize; dwEntry++)
						//allow primary OSD clients (i.e. EVGA Precision / MSI Afterburner) to use the first slot exclusively, so third party
						//applications start scanning the slots from the second one
					{
						RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY)((LPBYTE)pMem + pMem->dwOSDArrOffset + dwEntry * pMem->dwOSDEntrySize);

						if (dwPass)
						{
							if (!strlen(pEntry->szOSDOwner))
								strcpy_s(pEntry->szOSDOwner, sizeof(pEntry->szOSDOwner), lpOSDOwner);
						}

						if (!strcmp(pEntry->szOSDOwner, lpOSDOwner))
						{
							if (pMem->dwVersion >= 0x0002000c)
								//embedded graphs are supported for v2.12 and higher shared memory
								dwResult = EmbedGraphInObjBuffer(pEntry->buffer, sizeof(pEntry->buffer), dwOffset, lpBuffer, dwBufferPos, dwBufferSize, dwWidth, dwHeight, dwMargin, fltMin, fltMax, dwFlags);
						}
					}

					if (dwResult)
						break;
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return dwResult;
}
//////////////////////////////////////////////////////////////////////
DWORD CRTSSSharedMemoryInterface::EmbedGraphInObjBuffer(LPBYTE lpObjBuffer, DWORD dwObjBufferSize, DWORD dwOffset, FLOAT* lpBuffer, DWORD dwBufferPos, DWORD dwBufferSize, LONG dwWidth, LONG dwHeight, LONG dwMargin, FLOAT fltMin, FLOAT fltMax, DWORD dwFlags)
{
	DWORD dwResult = 0;

	if (dwOffset + sizeof(RTSS_EMBEDDED_OBJECT_GRAPH) + dwBufferSize * sizeof(FLOAT) > dwObjBufferSize)
		//validate embedded object offset and size and ensure that we don't overrun the buffer
		return 0;


	LPRTSS_EMBEDDED_OBJECT_GRAPH lpGraph = (LPRTSS_EMBEDDED_OBJECT_GRAPH)(lpObjBuffer + dwOffset);
		//get pointer to object in buffer

	lpGraph->header.dwSignature	= RTSS_EMBEDDED_OBJECT_GRAPH_SIGNATURE;
	lpGraph->header.dwSize		= sizeof(RTSS_EMBEDDED_OBJECT_GRAPH) + dwBufferSize * sizeof(FLOAT);
	lpGraph->header.dwWidth		= dwWidth;
	lpGraph->header.dwHeight	= dwHeight;
	lpGraph->header.dwMargin	= dwMargin;
	lpGraph->dwFlags			= dwFlags;
	lpGraph->fltMin				= fltMin;
	lpGraph->fltMax				= fltMax;
	lpGraph->dwDataCount		= dwBufferSize;

	if (lpBuffer && dwBufferSize)
	{
		for (DWORD dwPos=0; dwPos<dwBufferSize; dwPos++)
		{
			FLOAT fltData = lpBuffer[dwBufferPos];

			lpGraph->fltData[dwPos] = (fltData == FLT_MAX) ? 0 : fltData;

			dwBufferPos = (dwBufferPos + 1) & (dwBufferSize - 1);
		}
	}

	dwResult = lpGraph->header.dwSize;

	return dwResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CRTSSSharedMemoryInterface::GetProcessPerfCounter(DWORD dwProcessID, DWORD dwSamplingPeriod, DWORD dwPerfCounterID, DWORD dwPerfCounterParam, LPDWORD lpPerfCounter)
{
	BOOL bResult	= FALSE;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr				= MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		LPRTSS_SHARED_MEMORY pMem	= (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020012))
				//process specific performance counters are supported for v2.18 and higher shared memory
			{
				DWORD dwAppEntry = 0;

				if (dwProcessID)
					//processID is specified, so we search for it in the process entries array
				{
					for (DWORD dwEntry=0; dwEntry<pMem->dwAppArrSize; dwEntry++)
					{
						RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY)((LPBYTE)pMem + pMem->dwAppArrOffset + dwEntry * pMem->dwAppEntrySize);
		
						if (pEntry->dwProcessID == dwProcessID)
						{
							dwAppEntry = dwEntry;
							break;
						}
					}
				}
				else
					//process ID is not specified, so we use last foreground process entry
					dwAppEntry = pMem->dwLastForegroundApp;

				RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY)((LPBYTE)pMem + pMem->dwAppArrOffset + dwAppEntry * pMem->dwAppEntrySize);

				if (pEntry->dwProcessID)
					//validate process entry
				{
					if (!pEntry->dwProcessPerfCountersSamplingPeriod)
						//process perf counters sampling is not enabled yet, enable it now
						pEntry->dwProcessPerfCountersSamplingPeriod = dwSamplingPeriod;
					else
					{
						for (DWORD dwCounter=0; dwCounter<pEntry->dwProcessPerfCountersCount; dwCounter++)
						{
							RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_PROCESS_PERF_COUNTER_ENTRY pCounter = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_PROCESS_PERF_COUNTER_ENTRY)((LPBYTE)pEntry + pMem->dwProcessPerfCountersArrOffset + dwCounter * pMem->dwProcessPerfCountersEntrySize);

							if ((pCounter->dwID		== dwPerfCounterID		) &&
								(pCounter->dwParam	== dwPerfCounterParam	))
							{
								if (lpPerfCounter)
									*lpPerfCounter = pCounter->dwData;

								bResult = TRUE;

								break;
							}
						}
					}
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
