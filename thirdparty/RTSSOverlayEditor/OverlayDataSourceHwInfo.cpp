// OverlayDataSourceHwInfo.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataSourceHwInfo.h"
#include "Overlay.h"
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceHwInfo::COverlayDataSourceHwInfo()
{
	m_dwSensorInst		= 0;
	m_dwReadingType		= 0;
	m_strReadingName	= "";
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceHwInfo::~COverlayDataSourceHwInfo()
{
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceHwInfo::GetProviderName()
{
	return PROVIDER_HWINFO;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayDataSourceHwInfo::GetSensorInst()
{
	return m_dwSensorInst;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHwInfo::SetSensorInst(DWORD dwSensorInst)
{
	m_dwSensorInst = dwSensorInst;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayDataSourceHwInfo::GetReadingType()
{
	return m_dwReadingType;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHwInfo::SetReadingType(DWORD dwReadingType)
{
	m_dwReadingType = dwReadingType;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceHwInfo::GetReadingName()
{
	return m_strReadingName;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHwInfo::SetReadingName(LPCSTR lpReadingName)
{
	m_strReadingName = lpReadingName;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHwInfo::Save()
{	
	COverlayDataSource::Save();

	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_lpOverlay->SetConfigInt(strSource, "SensorInst"	, m_dwSensorInst	);
		m_lpOverlay->SetConfigInt(strSource, "ReadingType"	, m_dwReadingType	);
		m_lpOverlay->SetConfigStr(strSource, "ReadingName"	, m_strReadingName	);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHwInfo::Load()
{
	COverlayDataSource::Load();

	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_dwSensorInst		= m_lpOverlay->GetConfigInt(strSource, "SensorInst"		, 0xFFFFFFFF	);
		m_dwReadingType		= m_lpOverlay->GetConfigInt(strSource, "ReadingType"	, 0				);
		m_strReadingName	= m_lpOverlay->GetConfigStr(strSource, "ReadingName"	, ""			);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceHwInfo::Copy(COverlayDataSource* lpSrc)
{
	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_HWINFO))
	{
		COverlayDataSourceHwInfo* lpSrcHwInfo = dynamic_cast<COverlayDataSourceHwInfo*>(lpSrc);

		if (lpSrcHwInfo)
		{
			COverlayDataSource::Copy(lpSrc);

			m_dwSensorInst		= lpSrcHwInfo->m_dwSensorInst;
			m_dwReadingType		= lpSrcHwInfo->m_dwReadingType;
			m_strReadingName	= lpSrcHwInfo->m_strReadingName;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceHwInfo::IsEqual(COverlayDataSource* lpSrc)
{
	if (!COverlayDataSource::IsEqual(lpSrc))
		return FALSE;

	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_HWINFO))
	{
		COverlayDataSourceHwInfo* lpSrcHwInfo = dynamic_cast<COverlayDataSourceHwInfo*>(lpSrc);

		if (lpSrcHwInfo)
		{
			if (m_dwSensorInst != lpSrcHwInfo->m_dwSensorInst)
				return FALSE;
			if (m_dwReadingType	!= lpSrcHwInfo->m_dwReadingType)
				return FALSE;
			if (strcmp(m_strReadingName, lpSrcHwInfo->m_strReadingName))
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
