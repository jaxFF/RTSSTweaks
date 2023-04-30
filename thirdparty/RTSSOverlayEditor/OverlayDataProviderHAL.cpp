// OverlayDataProviderHAL.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataProviderHAL.h"
#include "OverlayDataSourceHAL.h"
#include "OverlayEditor.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderHAL::COverlayDataProviderHAL()
{
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderHAL::~COverlayDataProviderHAL()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderHAL::UpdateSources(DWORD dwTimestamp)
{
	g_hal.SetTimestamp(dwTimestamp);

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		COverlayDataSourceHAL* lpSourceHAL = dynamic_cast<COverlayDataSourceHAL*>(lpSource);

		if (lpSourceHAL)
		{
			CHALDataSource* lpHALSource = g_hal.FindSource(lpSourceHAL->GetID());
			
			if (lpHALSource)
			{
				lpSource->SetData(lpHALSource->GetData());

				lpHALSource->SetDirty(TRUE);
			}
			else
				lpSource->SetData(FLT_MAX);
		}
	}

	g_hal.UpdateSourcesAsync();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderHAL::UpdateTimerDerivedSources()
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		COverlayDataSourceHAL* lpSourceHAL = dynamic_cast<COverlayDataSourceHAL*>(lpSource);

		if (lpSourceHAL)
		{
			CHALDataSource* lpHALSource = g_hal.FindSource(lpSourceHAL->GetID());
			
			if (lpHALSource && (lpHALSource->GetID() == HALDATASOURCE_ID_TIMER))
			{
				lpHALSource->Poll();

				lpSource->SetData(lpHALSource->GetData());
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
