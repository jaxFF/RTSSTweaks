// OverlayDataSourcesList.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Overlay.h"
#include "OverlayDataSourcesList.h"
#include "OverlayDataSourceHAL.h"
#include "OverlayDataSourceAIDA.h"
#include "OverlayDataSourceHwInfo.h"
#include "OverlayDataSourceMSIAB.h"
#include "OverlayDataSourcePerfCounter.h"
#include "OverlayDataSourceInternal.h"
#include "OverlayEditor.h"
#include "MAHMSharedMemory.h"
//////////////////////////////////////////////////////////////////////
COverlayDataSourcesList::COverlayDataSourcesList()
{
	m_lpOverlay				= NULL;
	m_fltPollingTime		= 0;

	m_lpProviderHAL			= NULL;
	m_lpProviderMSIAB		= NULL;
	m_lpProviderHwInfo		= NULL;
	m_lpProviderAIDA		= NULL;
	m_lpProviderPerfCounter	= NULL;
	m_lpProviderInternal	= NULL;
}
//////////////////////////////////////////////////////////////////////
COverlayDataSourcesList::~COverlayDataSourcesList()
{
	Destroy();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::Destroy()
{
	//destroy sources

	POSITION pos = GetHeadPosition();

	while (pos)
		delete GetNext(pos);

	RemoveAll();

	//destroy providers

	if (m_lpProviderHAL)
	{
		delete m_lpProviderHAL;

		m_lpProviderHAL = NULL;
	}

	if (m_lpProviderMSIAB)
	{
		delete m_lpProviderMSIAB;

		m_lpProviderMSIAB = NULL;
	}

	if (m_lpProviderHwInfo)
	{
		delete m_lpProviderHwInfo;

		m_lpProviderHwInfo = NULL;
	}

	if (m_lpProviderAIDA)
	{
		delete m_lpProviderAIDA;

		m_lpProviderAIDA = NULL;
	}

	if (m_lpProviderPerfCounter)
	{
		delete m_lpProviderPerfCounter;

		m_lpProviderPerfCounter = NULL;
	}

	if (m_lpProviderInternal)
	{
		delete m_lpProviderInternal;

		m_lpProviderInternal = NULL;
	}
}//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetOverlay(lpOverlay);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::CreateInternalSources()
{
	Destroy();

	COverlayDataSourceInternal* lpSource;

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMERATE);
	lpSource->SetUnits("FPS");
	AddSource(lpSource);

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMETIME);
	lpSource->SetUnits("ms");
	lpSource->SetFormat("%.1f");
	AddSource(lpSource);

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMERATE_MIN);
	lpSource->SetUnits("FPS");
	AddSource(lpSource);

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMERATE_AVG);
	lpSource->SetUnits("FPS");
	AddSource(lpSource);

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMERATE_MAX);
	lpSource->SetUnits("FPS");
	AddSource(lpSource);

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW);
	lpSource->SetUnits("FPS");
	AddSource(lpSource);

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW);
	lpSource->SetUnits("FPS");
	AddSource(lpSource);

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMERATE_HISTORY);
	lpSource->SetUnits("FPS");
	AddSource(lpSource);

	lpSource = new COverlayDataSourceInternal;
	lpSource->SetName(INTERNAL_SOURCE_FRAMETIME_HISTORY);
	lpSource->SetUnits("ms");
	lpSource->SetFormat("%.1f");
	AddSource(lpSource);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::UpdateFormulas()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetFormula(NULL);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::Load()
{
	Destroy();

	if (m_lpOverlay)
	{
		int nSources = m_lpOverlay->GetConfigInt("General", "Sources", 0);

		for (int nSource=0; nSource<nSources; nSource++)
		{
			CString strSource;
			strSource.Format("Source%d", nSource);

			CString strProvider = m_lpOverlay->GetConfigStr(strSource, "Provider", "");

			COverlayDataSource* lpSource = CreateSource(strProvider);

			if (lpSource)
			{
				AddSource(lpSource);

				lpSource->Load();
			}
		}
	}

	UpdateFormulas();
		//retranslate all formulas to provide support for forward declared cross-refrenced data sources
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::Copy(COverlayDataSourcesList* lpSrc)
{
	Destroy();

	POSITION pos = lpSrc->GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource		= lpSrc->GetNext(pos);
		COverlayDataSource* lpSourceCopy	= CreateSource(lpSource->GetProviderName());

		AddSource(lpSourceCopy);
			//NOTE: it is important to add source BEFORE calling Copy because the source's ptr to parent list must be initialized before setting up the formula
			//(for parsing variables containing references to different data sources)

		lpSourceCopy->Copy(lpSource);
	}

	UpdateFormulas();
		//retranslate all formulas to provide support for forward declared cross-refrenced data sources
}
//////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourcesList::IsEqual(COverlayDataSourcesList* lpSrc)
{
	POSITION posSrc = lpSrc->GetHeadPosition();
	POSITION posOwn	= GetHeadPosition();

	while (posSrc && posOwn)
	{
		COverlayDataSource* lpSrcSource		= lpSrc->GetNext(posSrc);
		COverlayDataSource* lpOwnSource		= GetNext(posOwn);

		if (!lpOwnSource->IsEqual(lpSrcSource))
			return FALSE;
	}

	if (posSrc || posOwn)
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::Save()
{
	if (m_lpOverlay)
	{
		m_lpOverlay->SetConfigInt("General", "Sources", GetCount());

		POSITION pos = GetHeadPosition();

		while (pos)
		{
			COverlayDataSource* lpSource = GetNext(pos);

			lpSource->Save();
		}
	}
}
//////////////////////////////////////////////////////////////////////
COverlayDataSource* COverlayDataSourcesList::CreateSource(LPCSTR lpProvider)
{
	if (!strcmp(lpProvider, PROVIDER_HAL))
		return new COverlayDataSourceHAL;

	if (!strcmp(lpProvider, PROVIDER_MSIAB))
		return new COverlayDataSourceMSIAB;

	if (!strcmp(lpProvider, PROVIDER_HWINFO))
		return new COverlayDataSourceHwInfo;

	if (!strcmp(lpProvider, PROVIDER_AIDA))
		return new COverlayDataSourceAIDA;

	if (!strcmp(lpProvider, PROVIDER_PERFCOUNTER))
		return new COverlayDataSourcePerfCounter;

	if (!strcmp(lpProvider, PROVIDER_INTERNAL))
		return new COverlayDataSourceInternal;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::AddSource(COverlayDataSource* lpSource)
{
	CString strProvider = lpSource->GetProviderName();

	if (!strcmp(strProvider, PROVIDER_HAL))
	{
		if (!m_lpProviderHAL)
			m_lpProviderHAL = new COverlayDataProviderHAL;

		m_lpProviderHAL->RegisterSource(lpSource);
	}
	else
	if (!strcmp(strProvider, PROVIDER_MSIAB))
	{
		if (!m_lpProviderMSIAB)
			m_lpProviderMSIAB = new COverlayDataProviderMSIAB;

		m_lpProviderMSIAB->RegisterSource(lpSource);
	}
	else
	if (!strcmp(strProvider, PROVIDER_HWINFO))
	{
		if (!m_lpProviderHwInfo)
			m_lpProviderHwInfo = new COverlayDataProviderHwInfo;

		m_lpProviderHwInfo->RegisterSource(lpSource);
	}
	else
	if (!strcmp(strProvider, PROVIDER_AIDA))
	{
		if (!m_lpProviderAIDA)
			m_lpProviderAIDA = new COverlayDataProviderAIDA;

		m_lpProviderAIDA->RegisterSource(lpSource);
	}
	else
	if (!strcmp(strProvider, PROVIDER_PERFCOUNTER))
	{
		if (!m_lpProviderPerfCounter)
			m_lpProviderPerfCounter = new COverlayDataProviderPerfCounter;

		m_lpProviderPerfCounter->RegisterSource(lpSource);
	}
	else
	if (!strcmp(strProvider, PROVIDER_INTERNAL))
	{
		if (!m_lpProviderInternal)
			m_lpProviderInternal = new COverlayDataProviderInternal;

		m_lpProviderInternal->RegisterSource(lpSource);
	}

	lpSource->SetOverlay(m_lpOverlay);
	lpSource->SetDataSourcesList(this);
	lpSource->SetSourceID(GetCount());
	AddTail(lpSource);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::UpdateTimerDerivedSources()
{
	if (m_lpProviderHAL)
		m_lpProviderHAL->UpdateTimerDerivedSources();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::UpdateSources()
{
	TIMING_INIT
	TIMING_BEGIN

	DWORD dwTimestamp = GetTickCount();

	if (m_lpProviderHAL)
		m_lpProviderHAL->UpdateSources(dwTimestamp);

	if (m_lpProviderMSIAB)
		m_lpProviderMSIAB->UpdateSources(dwTimestamp);

	if (m_lpProviderHwInfo)
		m_lpProviderHwInfo->UpdateSources(dwTimestamp);

	if (m_lpProviderAIDA)
		m_lpProviderAIDA->UpdateSources(dwTimestamp);

	if (m_lpProviderPerfCounter)
		m_lpProviderPerfCounter->UpdateSources(dwTimestamp);

	if (m_lpProviderInternal)
		m_lpProviderInternal->UpdateSources(dwTimestamp);

	TIMING_END

	m_fltPollingTime = timing;
}
//////////////////////////////////////////////////////////////////////
COverlayDataSource* COverlayDataSourcesList::GetSource(int nIndex)
{
	POSITION pos = FindIndex(nIndex);

	if (pos)
		return GetAt(pos);

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::UpdateSourceIDs()
{
	DWORD dwSourceID = 0;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetSourceID(dwSourceID++);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::RemoveSource(COverlayDataSource* lpSource)
{
	if (lpSource)
	{
		POSITION pos = Find(lpSource);

		if (pos)
			RemoveAt(pos);

		lpSource->GetProvider()->UnregisterSource(lpSource);
	}
}
//////////////////////////////////////////////////////////////////////
COverlayDataSource* COverlayDataSourcesList::FindSource(LPCSTR lpName)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		if (!strcmp(lpSource->GetName(), lpName))
			return lpSource;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
COverlayDataSource* COverlayDataSourcesList::FindSource(DWORD dwID)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		if (lpSource->GetSourceID() == dwID)
			return lpSource;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::Reorder(CList<COverlayDataSource*, COverlayDataSource*>* lpOrder)
{
	//remove data sources ptr without deleting the objects

	RemoveAll();

	//reorder the list

	POSITION pos = lpOrder->GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = lpOrder->GetNext(pos);

		AddSource(lpSource);
	}
}
//////////////////////////////////////////////////////////////////////
float COverlayDataSourcesList::GetPollingTime()
{
	return m_fltPollingTime;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesList::ResetStat()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->ResetStat();
}
//////////////////////////////////////////////////////////////////////
