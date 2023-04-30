// OverlayDataSource.h: interface for the COverlayDataSource class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "PropertyListCtrl.h"
#include "PerfCounterObjects.h"
#include "afxcmn.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceAddDlg dialog
/////////////////////////////////////////////////////////////////////////////
#define PARSE_SENSOR_CONTEXT_INIT									1
#define PARSE_SENSOR_CONTEXT_UPDATE									2
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceAddDlg : public CDialog, public CAIDA64Parser
{
	DECLARE_DYNAMIC(COverlayDataSourceAddDlg)

public:
	COverlayDataSourceAddDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayDataSourceAddDlg();

	void SetSources(COverlayDataSourcesList* lpSources);

	void InitSourcesList();
	void UpdateSourcesList();
	void UpdateProviderControls();

	void InitSourcesListHAL();
	void InitSourcesListAIDA();
	void InitSourcesListHwInfo();
	void InitSourcesListMSIAB();
	void InitSourcesListPerfCounter();

	void UpdateSourcesListHAL();
	void UpdateSourcesListAIDA();
	void UpdateSourcesListHwInfo();
	void UpdateSourcesListMSIAB();
	void UpdateSourcesListPerfCounter();

	CString ExportHAL();
	CString ExportAIDA();
	CString ExportHwInfo();
	CString ExportMSIAB();
	CString ExportPerfCounter();

	void AddSourceHAL(PLC_ITEM_DESC* lpItemDesc);
	void AddSourceAIDA(PLC_ITEM_DESC* lpItemDesc);
	void AddSourceHwInfo(PLC_ITEM_DESC* lpItemDesc);
	void AddSourceMSIAB(PLC_ITEM_DESC* lpItemDesc);
	void AddSourcePerfCounter(PLC_ITEM_DESC* lpItemDesc);

	void AddProvider(CString strName, int nIcon);

	CString GetGroupName(LPMAHM_SHARED_MEMORY_ENTRY lpEntry, BOOL bUseInstance);
	LPMAHM_SHARED_MEMORY_ENTRY FindEntry(LPMAHM_SHARED_MEMORY_HEADER lpHeader, LPCSTR lpSrcName);
	PHWiNFO_SENSORS_READING_ELEMENT FindReadingElement(PHWiNFO_SENSORS_SHARED_MEM2 pMem, LPCSTR lpSensorName, LPCSTR lpReadingName, DWORD dwReadingType);

	PLC_ITEM_DESC* GetRootItem(LPCSTR lpText);

	//CAIDA64Parser virtual functions

	virtual void ParseSensor(LPCSTR lpType, LPCSTR lpID, LPCSTR lpLabel, LPCSTR lpValue, DWORD dwContext);

	CPropertyListCtrl	m_sourcesListCtrl;
	CIconImageList		m_imageList;
	int					m_nProvider;

	CComboBoxEx			m_ctrlProviderCombo;

// Dialog Data
	enum { IDD = IDD_OVERLAY_DATA_SOURCE_ADD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlayDataSourcesList*	m_lpSources;
	UINT						m_nTimerID;
	CStringList					m_cache;

	CPerfCounterObjects			m_objects;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeProviderCombo();
	afx_msg void OnBnClickedExportButton();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedDataProviderSetup();
};
/////////////////////////////////////////////////////////////////////////////
