// OverlayDataSourceHAL.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataSourceHAL.h"
#include "Overlay.h"
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceHAL::COverlayDataSourceHAL()
{
	m_strID = "";
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceHAL::~COverlayDataSourceHAL()
{
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceHAL::GetProviderName()
{
	return PROVIDER_HAL;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceHAL::GetID()
{
	return m_strID;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHAL::SetID(LPCSTR lpID)
{
	m_strID = lpID;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHAL::Save()
{	
	COverlayDataSource::Save();

	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_lpOverlay->SetConfigStr(strSource, "ID"	, m_strID	);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHAL::Load()
{
	COverlayDataSource::Load();

	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_strID = m_lpOverlay->GetConfigStr(strSource, "ID"	, ""	);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHAL::Copy(COverlayDataSource* lpSrc)
{
	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_HAL))
	{
		COverlayDataSourceHAL* lpSrcHAL = dynamic_cast<COverlayDataSourceHAL*>(lpSrc);

		if (lpSrcHAL)
		{
			COverlayDataSource::Copy(lpSrc);

			m_strID	= lpSrcHAL->m_strID;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceHAL::IsEqual(COverlayDataSource* lpSrc)
{
	if (!COverlayDataSource::IsEqual(lpSrc))
		return FALSE;

	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_HAL))
	{
		COverlayDataSourceHAL* lpSrcHAL = dynamic_cast<COverlayDataSourceHAL*>(lpSrc);

		if (lpSrcHAL)
		{
			if (strcmp(m_strID, lpSrcHAL->m_strID))
				return FALSE;
		}
		else
			return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
