// OverlayDataProviderInternal.h: interface for the COverlayDataProviderInternal class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataProvider.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataProviderInternal : public COverlayDataProvider
{
public:
	COverlayDataProviderInternal();
	virtual ~COverlayDataProviderInternal();

	//COverlayDataProvider virtual functions

	virtual void UpdateSources(DWORD dwTimestamp);

protected:
	void UpdateSource(LPCSTR lpName, float fltData);
		//updates all registered data sources with matching ID

	void InvalidateRTSSSources(BOOL bAll);
		//invalidates all internal RTSS framerate related or benchmark related sources only
};
/////////////////////////////////////////////////////////////////////////////
