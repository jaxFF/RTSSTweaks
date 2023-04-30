// OverlayDataProviderMSIAB.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataProviderMSIAB.h"
#include "OverlayDataSourceMSIAB.h"
#include "OverlayEditor.h"
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderMSIAB::COverlayDataProviderMSIAB()
{
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderMSIAB::~COverlayDataProviderMSIAB()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderMSIAB::UpdateSources(DWORD /*dwTimestamp*/)
{	
	SetSourcesDirty();
		//mark all sources as dirty

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MAHMSharedMemory");
		//try to open MSI AB monitoring shared memory mapping file

	if (hMapFile)
	{
		LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			//try to map view of file to get access to MSI AB monitoring shared memory

		if (pMapAddr)
		{
			ParseSharedMemory((LPMAHM_SHARED_MEMORY_HEADER)pMapAddr);
				//parse MSI AB monitoring shared memory
			InvalidateDirtySources();
				//invalidate dirty sources, which were not updated by the previous call

			UnmapViewOfFile(pMapAddr);
		}
		else
		{
			InvalidateSources();
				//invalidate sources if we cannot map view of file (MSI AB is shutting down)
		}

		CloseHandle(hMapFile);
	}
	else
	{
		InvalidateSources();
			//invalidate sources if we cannot map view of file (MSI AB is not running)
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderMSIAB::ParseSharedMemory(LPMAHM_SHARED_MEMORY_HEADER lpHeader)
{
		if (lpHeader->dwSignature == 'MAHM')
		{
			DWORD dwSources = lpHeader->dwNumEntries;

			for (DWORD dwSource=0; dwSource<dwSources; dwSource++)
			{
				LPMAHM_SHARED_MEMORY_ENTRY	lpEntry	= (LPMAHM_SHARED_MEMORY_ENTRY)((LPBYTE)lpHeader + lpHeader->dwHeaderSize + dwSource * lpHeader->dwEntrySize);

				UpdateSource(lpEntry->dwSrcId, lpEntry->dwGpu, lpEntry->szSrcName, lpEntry->data);
			}
		}
		else
		{
			InvalidateSources();
				//invalidate sources if MSI AB is shutting down
		}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderMSIAB::UpdateSource(DWORD dwSrcId, DWORD dwGpu, LPCSTR lpSrcName, float fltData)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		if (!strcmp(lpSource->GetProviderName(), PROVIDER_MSIAB))
		{
			COverlayDataSourceMSIAB* lpSourceMSIAB = dynamic_cast<COverlayDataSourceMSIAB*>(lpSource);

			if (lpSourceMSIAB)
			{
				DWORD	dwTargetSrcId		= lpSourceMSIAB->GetSrcId();
				DWORD	dwTargetGpu			= lpSourceMSIAB->GetGpu();
				CString strTargetSrcName	= lpSourceMSIAB->GetSrcName();

				if (strlen(strTargetSrcName))
				{
					if (!_stricmp(strTargetSrcName, lpSrcName))
					{
						lpSourceMSIAB->SetData(fltData);
					}
				}
				else
				{
					if ((dwTargetSrcId == dwSrcId) && (dwTargetGpu	== dwGpu))
					{
						lpSourceMSIAB->SetData(fltData);
					}
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////



