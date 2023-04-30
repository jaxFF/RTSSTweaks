// OverlayDataProviderAIDA.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataProviderAIDA.h"
#include "OverlayDataSourceAIDA.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderAIDA::COverlayDataProviderAIDA()
{
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderAIDA::~COverlayDataProviderAIDA()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderAIDA::UpdateSources(DWORD /*dwTimestamp*/)
{
	SetSourcesDirty();
		//mark all sources as dirty
	InvalidatePriorities();
		//invalidate data source priorities

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "AIDA64_SensorValues");
		//try to open AIDA64 shared memory mapping file

	if (hMapFile)
	{
		LPCSTR pMapAddr = (LPCSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			//try to map view of file to get access to AIDA64 XML-styled shared memory
		
		if (pMapAddr)
		{
			CString strCache = pMapAddr;
				//precache AIDA64 XML-styled shared memory

			ParseXML(strCache);
				//parse precached AIDA64 XML-styled shared memory
			InvalidateDirtySources();
				//invalidate dirty sources, which were not updated by the previous call
			
			UnmapViewOfFile(pMapAddr);
		}
		else
		{
			InvalidateSources();
				//invalidate sources if we cannot map view of file (AIDA64 is shutting down)
		}

		CloseHandle(hMapFile);
	}
	else
	{
		InvalidateSources();
			//invalidate sources if we cannot map view of file (AIDA64 is not running)
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderAIDA::ParseSensor(LPCSTR lpType, LPCSTR lpID, LPCSTR lpLabel, LPCSTR lpValue, DWORD dwContext)
{
	UpdateSource(lpID, lpValue);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderAIDA::UpdateSource(LPCSTR lpID, LPCSTR lpValue)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		if (!strcmp(lpSource->GetProviderName(), PROVIDER_AIDA))
		{
			COverlayDataSourceAIDA* lpSourceAIDA = dynamic_cast<COverlayDataSourceAIDA*>(lpSource);

			if (lpSourceAIDA)
			{
				CString strTargetID	= lpSourceAIDA->GetID();

				int iIndex = FindSubstring(strTargetID, lpID);
					//NOTE: AIDA64 may use different IDs for the same sensor depending on the vendor, e.g. "TCPU" and "TCPUPKG",
					//so we allow specifying multiple comma separated IDs in data source settings and search for lpID substring inside it

				if (iIndex != -1)
					//we'll use substring index as a data priority, the first substring is the highest priority one
				{
					float fltData;

					if (sscanf_s(lpValue, "%f", &fltData) == 1)
					{
						lpSourceAIDA->SetDataWithPriority(fltData, iIndex);
					}
					else
					{
						lpSourceAIDA->SetDataWithPriority(FLT_MAX, iIndex);
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////

