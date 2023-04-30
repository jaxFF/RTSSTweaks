// OverlayDataProvider.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataProvider.h"
#include "TokenString.h"

#include <float.h>
#include <shlwapi.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataProvider::COverlayDataProvider()
{
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProvider::~COverlayDataProvider()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProvider::InvalidateSources()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetData(FLT_MAX);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProvider::InvalidatePriorities()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetPriority(INT_MAX);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProvider::SetSourcesDirty()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetDirty(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProvider::InvalidateDirtySources()
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		if (lpSource->IsDirty())
			lpSource->SetData(FLT_MAX);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProvider::RegisterSource(COverlayDataSource* lpSource)
{
	if (lpSource)
	{
		if (Find(lpSource))
			return;

		AddTail(lpSource);

		lpSource->SetProvider(this);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProvider::UnregisterSource(COverlayDataSource* lpSource)
{
	if (lpSource)
	{
		POSITION pos = Find(lpSource);

		if (pos)
			RemoveAt(pos);
		
		lpSource->SetProvider(NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
int COverlayDataProvider::FindSubstring(LPCSTR lpString, LPCSTR lpSubstring, LPCSTR lpSeparator)
{
	int index = 0;

	CTokenString ts;

	LPCSTR lpToken = ts.strtok(lpString, lpSeparator);

	while (lpToken)
	{
		if (PathMatchSpec(lpSubstring, lpToken))
			return index;

		lpToken = ts.strtok(NULL, lpSeparator);

		index++;
	}

	return -1;
}
/////////////////////////////////////////////////////////////////////////////
