// OverlayDataProviderHwInfo.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataProviderHwInfo.h"
#include "OverlayDataSourceHwInfo.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderHwInfo::COverlayDataProviderHwInfo()
{
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderHwInfo::~COverlayDataProviderHwInfo()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderHwInfo::UpdateSources(DWORD /*dwTimestamp*/)
{
#ifdef HWINFO_SDK_AVAILABLE
	SetSourcesDirty();
		//mark all sources as dirty
	InvalidatePriorities();
		//invalidate data source priorities

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, HWiNFO_SENSORS_MAP_FILE_NAME2);
		//try to open HwInfo shared memory mapping file

	if (hMapFile)
	{
		PHWiNFO_SENSORS_SHARED_MEM2 pMapAddr = (PHWiNFO_SENSORS_SHARED_MEM2)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			//try to map view of file to get access to HwInfo shared memory
		
		if (pMapAddr)
		{
			ParseSharedMemory(pMapAddr);
				//parse HwInfo shared memory
			InvalidateDirtySources();
				//invalidate dirty sources, which were not updated by the previous call

			UnmapViewOfFile(pMapAddr);
		}
		else
		{
			InvalidateSources();
				//invalidate sources if we cannot map view of file (HwInfo is shutting down)
		}

		CloseHandle(hMapFile);
	}
	else
	{
		InvalidateSources();
			//invalidate sources if we cannot map view of file (HwInfo is not running)
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderHwInfo::ParseSharedMemory(PHWiNFO_SENSORS_SHARED_MEM2 pHWiNFOMemory)
{
#ifdef HWINFO_SDK_AVAILABLE
	for (DWORD dwSensor=0; dwSensor<pHWiNFOMemory->dwNumSensorElements; dwSensor++)
	{
		PHWiNFO_SENSORS_SENSOR_ELEMENT pSensor = (PHWiNFO_SENSORS_SENSOR_ELEMENT)((BYTE*)pHWiNFOMemory + pHWiNFOMemory->dwOffsetOfSensorSection + (pHWiNFOMemory->dwSizeOfSensorElement * dwSensor));

		for (DWORD dwReading=0; dwReading<pHWiNFOMemory->dwNumReadingElements; dwReading++)
		{
			PHWiNFO_SENSORS_READING_ELEMENT pReading = (PHWiNFO_SENSORS_READING_ELEMENT)((BYTE*)pHWiNFOMemory + pHWiNFOMemory->dwOffsetOfReadingSection	+ (pHWiNFOMemory->dwSizeOfReadingElement * dwReading));

			if (pReading->dwSensorIndex == dwSensor)
				UpdateSource(pSensor->dwSensorInst, pReading->tReading, pReading->szLabelOrig, (float)pReading->Value);
		}
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderHwInfo::UpdateSource(DWORD dwSensorInst, DWORD dwReadingType, LPCSTR lpReadingName, float fltData)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		if (!strcmp(lpSource->GetProviderName(), PROVIDER_HWINFO))
		{
			COverlayDataSourceHwInfo* lpSourceHwInfo = dynamic_cast<COverlayDataSourceHwInfo*>(lpSource);

			if (lpSourceHwInfo)
			{
				DWORD	dwTargetSensorInst		= lpSourceHwInfo->GetSensorInst();
				DWORD	dwTargetReadingType		= lpSourceHwInfo->GetReadingType();
				CString strTargetReadingName	= lpSourceHwInfo->GetReadingName();

				if (((dwTargetSensorInst == -1) || (dwTargetSensorInst == dwSensorInst)) && (dwTargetReadingType == dwReadingType))
				{
					int iIndex = FindSubstring(strTargetReadingName, lpReadingName);
						//NOTE: HwINFO64 may use different reading names for the same sensor depending on the vendor, e.g. "GPU Core Load" and "GPU Utilization",
						//so we allow specifying multiple comma separated reading names in data source settings and search for lpReadingName substring inside it

					if (iIndex != -1)
					//we'll use substring index as a data priority, the first substring is the highest priority one
						lpSourceHwInfo->SetDataWithPriority(fltData, iIndex);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////


