// OverlayDataSourceMSIAB.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Overlay.h"
#include "OverlayDataSourceMSIAB.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceMSIAB::COverlayDataSourceMSIAB()
{
	m_dwSrcId		= 0;
	m_dwGpu			= 0;
	m_strSrcName	= "";
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceMSIAB::~COverlayDataSourceMSIAB()
{
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceMSIAB::GetProviderName()
{
	return PROVIDER_MSIAB;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceMSIAB::SetSrcId(DWORD dwSrcId)
{
	m_dwSrcId = dwSrcId;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayDataSourceMSIAB::GetSrcId()
{
	return m_dwSrcId;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceMSIAB::SetGpu(DWORD dwGpu)
{
	m_dwGpu = dwGpu;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayDataSourceMSIAB::GetGpu()
{
	return m_dwGpu;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceMSIAB::SetSrcName(LPCSTR lpSrcName)
{
	m_strSrcName = lpSrcName;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceMSIAB::GetSrcName()
{
	return m_strSrcName;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceMSIAB::Save()
{	
	COverlayDataSource::Save();

	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_lpOverlay->SetConfigHex(strSource, "SrcId"	, m_dwSrcId		);
		m_lpOverlay->SetConfigHex(strSource, "Gpu"		, m_dwGpu		);
		m_lpOverlay->SetConfigStr(strSource, "SrcName"	, m_strSrcName	);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceMSIAB::Load()
{
	COverlayDataSource::Load();

	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_dwSrcId		= m_lpOverlay->GetConfigHex(strSource, "SrcId"		, MONITORING_SOURCE_ID_UNKNOWN	);
		m_dwGpu			= m_lpOverlay->GetConfigHex(strSource, "Gpu"		, 0xFFFFFFFF					);
		m_strSrcName	= m_lpOverlay->GetConfigStr(strSource, "SrcName"	, ""							);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceMSIAB::Copy(COverlayDataSource* lpSrc)
{
	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_MSIAB))
	{
		COverlayDataSourceMSIAB* lpSrcMSIAB = dynamic_cast<COverlayDataSourceMSIAB*>(lpSrc);

		if (lpSrcMSIAB)
		{
			COverlayDataSource::Copy(lpSrc);

			m_dwSrcId		= lpSrcMSIAB->m_dwSrcId;
			m_dwGpu			= lpSrcMSIAB->m_dwGpu;
			m_strSrcName	= lpSrcMSIAB->m_strSrcName;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceMSIAB::IsEqual(COverlayDataSource* lpSrc)
{
	if (!COverlayDataSource::IsEqual(lpSrc))
		return FALSE;

	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_MSIAB))
	{
		COverlayDataSourceMSIAB* lpSrcMSIAB = dynamic_cast<COverlayDataSourceMSIAB*>(lpSrc);

		if (lpSrcMSIAB)
		{
			if (m_dwSrcId != lpSrcMSIAB->m_dwSrcId)
				return FALSE;
			if (m_dwGpu != lpSrcMSIAB->m_dwGpu)
				return FALSE;
			if (strcmp(m_strSrcName, lpSrcMSIAB->m_strSrcName))
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
