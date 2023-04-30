// OverlayDataSourcesList.h: interface for the COverlayDataSourcesList class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataSource.h"
#include "OverlayDataProviderHAL.h"
#include "OverlayDataProviderMSIAB.h"
#include "OverlayDataProviderHwInfo.h"
#include "OverlayDataProviderAIDA.h"
#include "OverlayDataProviderPerfCounter.h"
#include "OverlayDataProviderInternal.h"
/////////////////////////////////////////////////////////////////////////////
class COverlay;
class COverlayDataSourcesList : public CList<COverlayDataSource*, COverlayDataSource*>
{
public:
	COverlayDataSourcesList();
	virtual ~COverlayDataSourcesList();

	void SetOverlay(COverlay* lpOverlay);

	void Load();
	void Save();
	void Copy(COverlayDataSourcesList* lpSrc);
	BOOL IsEqual(COverlayDataSourcesList* lpSrc);
	void Destroy();

	void CreateInternalSources();
	COverlayDataSource* CreateSource(LPCSTR lpProvider);
	void AddSource(COverlayDataSource* lpSource);
	void RemoveSource(COverlayDataSource* lpSource);
	void UpdateSourceIDs();
	void UpdateSources();
	void UpdateTimerDerivedSources();
	void UpdateFormulas();

	COverlayDataSource* GetSource(int nIndex);
	COverlayDataSource* FindSource(LPCSTR lpName);
	COverlayDataSource* FindSource(DWORD dwID);

	void Reorder(CList<COverlayDataSource*, COverlayDataSource*>* lpOrder);

	float GetPollingTime();

	void ResetStat();

protected:
	COverlay*							m_lpOverlay;
	float								m_fltPollingTime;

	COverlayDataProviderHAL*			m_lpProviderHAL;
	COverlayDataProviderMSIAB*			m_lpProviderMSIAB;
	COverlayDataProviderHwInfo*			m_lpProviderHwInfo;
	COverlayDataProviderAIDA*			m_lpProviderAIDA;
	COverlayDataProviderPerfCounter*	m_lpProviderPerfCounter;
	COverlayDataProviderInternal*		m_lpProviderInternal;
};
/////////////////////////////////////////////////////////////////////////////
