// OverlayDataProviderMSIAB.h: interface for the COverlayDataProviderMSIAB class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataProvider.h"
#include "MAHMSharedMemory.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataProviderMSIAB : public COverlayDataProvider
{
public:
	COverlayDataProviderMSIAB();
	virtual ~COverlayDataProviderMSIAB();

	//COverlayDataProvider virtual functions

	virtual void UpdateSources(DWORD dwTimestamp);
	
protected:
	void ParseSharedMemory(LPMAHM_SHARED_MEMORY_HEADER lpHeader);
		//parses MSI Afterburner shared memory
	void UpdateSource(DWORD dwSrcId, DWORD dwGpu, LPCSTR lpSrcName, float fltData);
		//updates all registered data sources with matching ID
};
/////////////////////////////////////////////////////////////////////////////
