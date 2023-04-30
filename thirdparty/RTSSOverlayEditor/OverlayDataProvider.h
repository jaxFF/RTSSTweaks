// OverlayDataProvider.h: interface for the COverlayDataProvider class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataSource.h"
#include "afxtempl.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataProvider : public CList<COverlayDataSource*, COverlayDataSource*>
{
public:
	COverlayDataProvider();
	virtual ~COverlayDataProvider();

	//NOTE: Data providers represent external applications, which can supply data to
	//overlay data sources. Each provider contains the list of registered data sources
	//and update all of them in UpdateSources call

	virtual void RegisterSource(COverlayDataSource* lpSource);
	virtual void UnregisterSource(COverlayDataSource* lpSource);

	virtual void UpdateSources(DWORD dwTimestamp) = 0;

	void SetSourcesDirty();
	void InvalidateSources();
	void InvalidatePriorities();
	void InvalidateDirtySources();

	int FindSubstring(LPCSTR lpString, LPCSTR lpSubstring, LPCSTR lpSeparator = ",");
};
/////////////////////////////////////////////////////////////////////////////
