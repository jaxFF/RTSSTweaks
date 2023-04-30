// OverlayDataProviderPerfCounter.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataProviderPerfCounter.h"
#include "OverlayDataSourcePerfCounter.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderPerfCounter::COverlayDataProviderPerfCounter()
{
	m_hQuery = NULL;
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderPerfCounter::~COverlayDataProviderPerfCounter()
{
	Uninit();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderPerfCounter::RegisterSource(COverlayDataSource* lpSource)
{
	COverlayDataProvider::RegisterSource(lpSource);

	COverlayDataSourcePerfCounter* lpSourcePerfCounter = dynamic_cast<COverlayDataSourcePerfCounter*>(lpSource);

	if (lpSourcePerfCounter)
		AddCounter(lpSourcePerfCounter);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderPerfCounter::UnregisterSource(COverlayDataSource* lpSource)
{
	COverlayDataSourcePerfCounter* lpSourcePerfCounter = dynamic_cast<COverlayDataSourcePerfCounter*>(lpSource);

	if (lpSourcePerfCounter)
		RemoveCounter(lpSourcePerfCounter);

	COverlayDataProvider::UnregisterSource(lpSource);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderPerfCounter::UpdateSources(DWORD /*dwTimestamp*/)
{
	if (!m_hQuery)
		Init();

	if (m_hQuery)
	{
		if (PdhCollectQueryData(m_hQuery) == ERROR_SUCCESS)
		{
			POSITION pos = m_countersList.GetHeadPosition();

			while (pos)
			{
				LPPERF_COUNTER_DESC lpDesc = m_countersList.GetNext(pos);

				DWORD dwType;

				PDH_FMT_COUNTERVALUE value;
				ZeroMemory(&value, sizeof(value));

				if (PdhGetFormattedCounterValue(lpDesc->hCounter, PDH_FMT_DOUBLE, &dwType, &value) == ERROR_SUCCESS)
					lpDesc->lpSource->SetData((float)value.doubleValue);
				else
					lpDesc->lpSource->SetData(FLT_MAX);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataProviderPerfCounter::Init()
{
	Uninit();

	//init objects to populate English and localized names map

	m_objects.Init();

	//open query

	if (PdhOpenQuery(0, 0, &m_hQuery) != ERROR_SUCCESS)
		return FALSE;

	//populate counters for all registered sources

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = GetNext(pos);

		COverlayDataSourcePerfCounter* lpSourcePerfCounter = dynamic_cast<COverlayDataSourcePerfCounter*>(lpSource);

		if (lpSourcePerfCounter)
			AddCounter(lpSourcePerfCounter);
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderPerfCounter::Uninit()
{
	//remove all performance counters

	POSITION pos = m_countersList.GetHeadPosition();

	while (pos)
	{
		LPPERF_COUNTER_DESC lpDesc = m_countersList.GetNext(pos);

		if (lpDesc->hCounter)
			PdhRemoveCounter(lpDesc->hCounter);
		lpDesc->hCounter = NULL;
	}

	m_countersList.RemoveAll();

	//close query

	if (m_hQuery)
		PdhCloseQuery(m_hQuery);
	m_hQuery = NULL;
}
/////////////////////////////////////////////////////////////////////////////
LPPERF_COUNTER_DESC COverlayDataProviderPerfCounter::FindCounter(COverlayDataSourcePerfCounter* lpSource)
{
	POSITION pos = m_countersList.GetHeadPosition();

	while (pos)
	{
		LPPERF_COUNTER_DESC lpDesc = m_countersList.GetNext(pos);

		if (lpDesc->lpSource == lpSource)
			return lpDesc;
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderPerfCounter::InitCounter(LPPERF_COUNTER_DESC lpDesc)
{
	if (m_hQuery)
	{
		char	szLocalizedObjectName	[MAX_PATH];
		char	szLocalizedInstanceName	[MAX_PATH];
		char	szLocalizedCounterName	[MAX_PATH];

		LONG	dwInstanceIndex	= lpDesc->lpSource->GetInstanceIndex();

		m_objects.GetLocalizedName(lpDesc->lpSource->GetObjectName()	, szLocalizedObjectName		, sizeof(szLocalizedObjectName));
		m_objects.GetLocalizedName(lpDesc->lpSource->GetInstanceName()	, szLocalizedInstanceName	, sizeof(szLocalizedInstanceName));
		m_objects.GetLocalizedName(lpDesc->lpSource->GetCounterName()	, szLocalizedCounterName	, sizeof(szLocalizedCounterName));
			//get localized object, instance and counter names

		if (!strlen(szLocalizedInstanceName) && (dwInstanceIndex != -1))
			//if instance name is not specified, then we'll treat specified index as the index in the list of enumerated
			//object's system specific enumerated instance names
		{
			LPCSTR lpInstanceName = m_objects.GetInstance(szLocalizedObjectName, dwInstanceIndex);

			if (lpInstanceName && _stricmp(lpInstanceName, "_Total"))
				//do not allow enumerated _Total instance to be selected by index
			{
				strcpy_s(szLocalizedInstanceName, sizeof(szLocalizedInstanceName), lpInstanceName);

				dwInstanceIndex = -1;
			}
		}

		PDH_COUNTER_PATH_ELEMENTS elements;
		ZeroMemory(&elements, sizeof(elements));

		elements.szMachineName		= NULL;
		elements.szObjectName		= szLocalizedObjectName;
		if (strlen(szLocalizedInstanceName))
			elements.szInstanceName	= szLocalizedInstanceName;
		elements.szParentInstance	= NULL;
		elements.dwInstanceIndex	= dwInstanceIndex;
		elements.szCounterName		= szLocalizedCounterName;
			//prepare counter path elements

		DWORD dwSize = sizeof(lpDesc->szCounterPath);

		PdhMakeCounterPath(&elements, lpDesc->szCounterPath, &dwSize, 0);
			//format full localized counter path

		if (strlen(lpDesc->szCounterPath))
			//try to add counter to query if we succesfully formatted the path
		{
			if (lpDesc->lpSource->IsDynamic() || (PdhValidatePath(lpDesc->szCounterPath) == ERROR_SUCCESS))
				//attempts to add a counter with invalid path are VERY SLOW, so we validate the path before trying to add the counter.
				//Dynamic sources (e.g. process specific ones) are the exception, we always try to add them without path validation
				PdhAddCounter(m_hQuery, lpDesc->szCounterPath, 0, &lpDesc->hCounter);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderPerfCounter::AddCounter(COverlayDataSourcePerfCounter* lpSource)
{
	if (m_hQuery)
	{
		LPPERF_COUNTER_DESC lpDesc = new PERF_COUNTER_DESC;
		ZeroMemory(lpDesc, sizeof(PERF_COUNTER_DESC));

		lpDesc->lpSource = lpSource;

		m_countersList.AddTail(lpDesc);

		InitCounter(lpDesc);

	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderPerfCounter::ReinitCounter(COverlayDataSourcePerfCounter* lpSource)
{
	if (m_hQuery)
	{
		LPPERF_COUNTER_DESC lpDesc = FindCounter(lpSource);

		if (lpDesc)
		{
			if (lpDesc->hCounter)
				PdhRemoveCounter(lpDesc->hCounter);
			lpDesc->hCounter = NULL;

			InitCounter(lpDesc);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderPerfCounter::RemoveCounter(COverlayDataSourcePerfCounter* lpSource)
{
	LPPERF_COUNTER_DESC lpDesc = FindCounter(lpSource);

	if (lpDesc)
	{
		if (lpDesc->hCounter)
			PdhRemoveCounter(lpDesc->hCounter);
		lpDesc->hCounter = NULL;

		POSITION pos = m_countersList.Find(lpDesc);

		if (pos)
		{
			m_countersList.RemoveAt(pos);

			delete lpDesc;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
