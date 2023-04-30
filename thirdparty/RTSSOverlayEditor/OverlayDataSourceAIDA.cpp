// OverlayDataSourceAIDA.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataSourceAIDA.h"
#include "Overlay.h"
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceAIDA::COverlayDataSourceAIDA()
{
	m_strID = "";
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceAIDA::~COverlayDataSourceAIDA()
{
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceAIDA::GetProviderName()
{
	return PROVIDER_AIDA;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceAIDA::GetID()
{
	return m_strID;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceAIDA::SetID(LPCSTR lpID)
{
	m_strID = lpID;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceAIDA::Save()
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
void COverlayDataSourceAIDA::Load()
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
void COverlayDataSourceAIDA::Copy(COverlayDataSource* lpSrc)
{
	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_AIDA))
	{
		COverlayDataSourceAIDA* lpSrcAIDA = dynamic_cast<COverlayDataSourceAIDA*>(lpSrc);

		if (lpSrcAIDA)
		{
			COverlayDataSource::Copy(lpSrc);

			m_strID	= lpSrcAIDA->m_strID;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceAIDA::IsEqual(COverlayDataSource* lpSrc)
{
	if (!COverlayDataSource::IsEqual(lpSrc))
		return FALSE;

	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_AIDA))
	{
		COverlayDataSourceAIDA* lpSrcAIDA = dynamic_cast<COverlayDataSourceAIDA*>(lpSrc);

		if (lpSrcAIDA)
		{
			if (strcmp(m_strID, lpSrcAIDA->m_strID))
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
