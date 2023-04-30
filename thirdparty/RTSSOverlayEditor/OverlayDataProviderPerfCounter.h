// OverlayDataProviderPerfCounter.h: interface for the COverlayDataProviderPerfCounter class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataProvider.h"
#include "OverlayDataSourcePerfCounter.h"
#include "PerfCounterObjects.h"
/////////////////////////////////////////////////////////////////////////////
typedef struct PERF_COUNTER_DESC 
{
	COverlayDataSourcePerfCounter*	lpSource;
	HCOUNTER						hCounter;
	char							szCounterPath[MAX_PATH];
} PERF_COUNTER_DESC, *LPPERF_COUNTER_DESC;
/////////////////////////////////////////////////////////////////////////////
class COverlayDataProviderPerfCounter : public COverlayDataProvider
{
public:
	COverlayDataProviderPerfCounter();
	virtual ~COverlayDataProviderPerfCounter();

	//COverlayDataProvider virtual functions

	virtual void UpdateSources(DWORD dwTimestamp);
	virtual void RegisterSource(COverlayDataSource* lpSource);
	virtual void UnregisterSource(COverlayDataSource* lpSource);

	LPPERF_COUNTER_DESC FindCounter(COverlayDataSourcePerfCounter* lpSource);
	void InitCounter(LPPERF_COUNTER_DESC lpDesc);

	void AddCounter(COverlayDataSourcePerfCounter* lpSource);
	void ReinitCounter(COverlayDataSourcePerfCounter* lpSource);
	void RemoveCounter(COverlayDataSourcePerfCounter* lpSource);

protected:
	CPerfCounterObjects	m_objects;
	PDH_HQUERY			m_hQuery;
	CList<LPPERF_COUNTER_DESC, LPPERF_COUNTER_DESC> m_countersList;

	BOOL Init();
	void Uninit();
};
/////////////////////////////////////////////////////////////////////////////
