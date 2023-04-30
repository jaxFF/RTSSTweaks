// OverlayDataProviderInternal.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataProviderInternal.h"
#include "OverlayDataSourceInternal.h"
#include "OverlayEditor.h"
#include "RTSSSharedMemory.h"
#include <RTSSTweaks.h>

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderInternal::COverlayDataProviderInternal()
{
// Testing!!!sss
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderInternal::~COverlayDataProviderInternal()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderInternal::UpdateSources(DWORD /*dwTimestamp*/)
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		LPRTSS_SHARED_MEMORY pMem = (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && (pMem->dwVersion >= 0x00020000))
			{
				DWORD dwAppEntry = pMem->dwLastForegroundApp;

				if (dwAppEntry < pMem->dwAppArrSize)
				{
					RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_APP_ENTRY)((LPBYTE)pMem + pMem->dwAppArrOffset + dwAppEntry * pMem->dwAppEntrySize);

					if (pEntry->dwProcessID)
					{
						if (pEntry->dwStatCount)
						{
							UpdateSource(INTERNAL_SOURCE_FRAMERATE_MIN, pEntry->dwStatFramerateMin / 10.0f);
							UpdateSource(INTERNAL_SOURCE_FRAMERATE_AVG, pEntry->dwStatFramerateAvg / 10.0f);
							UpdateSource(INTERNAL_SOURCE_FRAMERATE_MAX, pEntry->dwStatFramerateMax / 10.0f);

							UpdateSource(INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW, pEntry->dwStatFramerate1Dot0PercentLow / 10.0f);
							UpdateSource(INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW, pEntry->dwStatFramerate0Dot1PercentLow / 10.0f);
						}
						else				
							InvalidateRTSSSources(FALSE);

						FLOAT fltFramerate = 0;

						if (pEntry->dwTime1)
						{
							if (GetTickCount() - pEntry->dwTime1 <= 2000)
								fltFramerate = pEntry->dwStatFrameTimeBufFramerate / 10.0f;
						}

						UpdateSource(INTERNAL_SOURCE_FRAMERATE			, fltFramerate);
						UpdateSource(INTERNAL_SOURCE_FRAMERATE_HISTORY	, fltFramerate);

						FLOAT fltFrametime = 0;

						if (pEntry->dwStatFrameTimeCount)
						{
							fltFrametime = pEntry->dwStatFrameTimeMax / 1000.0f;

							pEntry->dwStatFrameTimeCount = 0;
						}

						UpdateSource(INTERNAL_SOURCE_FRAMETIME			, fltFrametime);
						UpdateSource(INTERNAL_SOURCE_FRAMETIME_HISTORY	, fltFrametime);
					}
				}
			}

			UnmapViewOfFile(pMapAddr);
		}
		else
			InvalidateRTSSSources(TRUE);

		CloseHandle(hMapFile);
	}
	else
		InvalidateRTSSSources(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderInternal::InvalidateRTSSSources(BOOL bAll)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		CString strName = lpSource->GetName();

		if (bAll)
		{
			if (!strcmp(strName, INTERNAL_SOURCE_FRAMERATE						) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMETIME						) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_MIN					) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_AVG					) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_MAX					) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW	) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW	))
				lpSource->SetData(FLT_MAX);
		}
		else
		{
			if (!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_MIN					) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_AVG					) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_MAX					) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW	) ||
				!strcmp(strName, INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW	))
				lpSource->SetData(FLT_MAX);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderInternal::UpdateSource(LPCSTR lpName, float fltData)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		if (!strcmp(lpSource->GetName(), lpName))
		{
			lpSource->SetData(fltData);

			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
