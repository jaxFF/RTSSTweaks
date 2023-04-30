// OverlayDataSourcePerfCounter.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataSourcePerfCounter.h"
#include "Overlay.h"
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourcePerfCounter::COverlayDataSourcePerfCounter()
{
	m_strObjectName		= "";
	m_strInstanceName	= "";
	m_dwInstanceIndex	= -1;
	m_strCounterName	= "";

	m_bDynamic			= FALSE;
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourcePerfCounter::~COverlayDataSourcePerfCounter()
{
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourcePerfCounter::GetProviderName()
{
	return PROVIDER_PERFCOUNTER;
}
/////////////////////////////////////////////////////////////////////////////
CString	COverlayDataSourcePerfCounter::GetObjectName()
{
	return m_strObjectName;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourcePerfCounter::SetObjectName(LPCSTR lpObjectName)
{
	m_strObjectName = lpObjectName;
}
/////////////////////////////////////////////////////////////////////////////
CString	COverlayDataSourcePerfCounter::GetInstanceName()
{
	return m_strInstanceName;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourcePerfCounter::SetInstanceName(LPCSTR lpInstanceName)
{
	m_strInstanceName = lpInstanceName;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayDataSourcePerfCounter::GetInstanceIndex()
{
	return m_dwInstanceIndex;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourcePerfCounter::SetInstanceIndex(LONG dwInstanceIndex)
{
	m_dwInstanceIndex = dwInstanceIndex;
}
/////////////////////////////////////////////////////////////////////////////
CString	COverlayDataSourcePerfCounter::GetCounterName()
{
	return m_strCounterName;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourcePerfCounter::SetCounterName(LPCSTR lpCounterName)
{
	m_strCounterName = lpCounterName;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourcePerfCounter::IsDynamic()
{
	return m_bDynamic;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourcePerfCounter::SetDynamic(BOOL bDynamic)
{
	m_bDynamic = bDynamic;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourcePerfCounter::Save()
{	
	COverlayDataSource::Save();

	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_lpOverlay->SetConfigStr(strSource, "ObjectName"	, m_strObjectName	);

		if (m_dwInstanceIndex != -1)
			m_lpOverlay->SetConfigInt(strSource, "InstanceIndex", m_dwInstanceIndex);
		else
		{
			if (strlen(m_strInstanceName))
				m_lpOverlay->SetConfigStr(strSource, "InstanceName"	, m_strInstanceName);
		}

		m_lpOverlay->SetConfigStr(strSource, "CounterName"	, m_strCounterName	);
		m_lpOverlay->SetConfigInt(strSource, "Dynamic"		, m_bDynamic ? 1 : 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourcePerfCounter::Load()
{
	COverlayDataSource::Load();

	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_strObjectName		= m_lpOverlay->GetConfigStr(strSource, "ObjectName"		, ""	);
		m_strInstanceName	= m_lpOverlay->GetConfigStr(strSource, "InstanceName"	, ""	);
		m_dwInstanceIndex	= m_lpOverlay->GetConfigInt(strSource, "InstanceIndex"	, -1	);
		m_strCounterName	= m_lpOverlay->GetConfigStr(strSource, "CounterName"	, ""	);		
		m_bDynamic			= m_lpOverlay->GetConfigInt(strSource, "Dynamic"		, 0		);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourcePerfCounter::Copy(COverlayDataSource* lpSrc)
{
	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_PERFCOUNTER))
	{
		COverlayDataSourcePerfCounter* lpSrcPerfCounter = dynamic_cast<COverlayDataSourcePerfCounter*>(lpSrc);

		if (lpSrcPerfCounter)
		{
			COverlayDataSource::Copy(lpSrc);

			m_strObjectName		= lpSrcPerfCounter->m_strObjectName;
			m_strInstanceName	= lpSrcPerfCounter->m_strInstanceName;
			m_dwInstanceIndex	= lpSrcPerfCounter->m_dwInstanceIndex;
			m_strCounterName	= lpSrcPerfCounter->m_strCounterName;
			m_bDynamic			= lpSrcPerfCounter->m_bDynamic;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourcePerfCounter::IsEqual(COverlayDataSource* lpSrc)
{
	if (!COverlayDataSource::IsEqual(lpSrc))
		return FALSE;

	if (!strcmp(lpSrc->GetProviderName(), PROVIDER_PERFCOUNTER))
	{
		COverlayDataSourcePerfCounter* lpSrcPerfCounter = dynamic_cast<COverlayDataSourcePerfCounter*>(lpSrc);

		if (lpSrcPerfCounter)
		{
			if (strcmp(m_strObjectName, lpSrcPerfCounter->m_strObjectName))
				return FALSE;
			if (strcmp(m_strInstanceName, lpSrcPerfCounter->m_strInstanceName))
				return FALSE;
			if (m_dwInstanceIndex != lpSrcPerfCounter->m_dwInstanceIndex)
				return FALSE;
			if (strcmp(m_strObjectName, lpSrcPerfCounter->m_strObjectName))
				return FALSE;
			if (m_bDynamic != lpSrcPerfCounter->m_bDynamic)
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
