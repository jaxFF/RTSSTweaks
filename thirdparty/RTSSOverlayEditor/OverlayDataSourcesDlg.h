// OverlayDataSourcesDlg.h: interface for the COverlayDataSourcesDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "afxcmn.h"
#include "CheckedListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourcesDlg dialog
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourcesDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayDataSourcesDlg)

public:
	COverlayDataSourcesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayDataSourcesDlg();

	void SetOverlay(COverlay* lpOverlay);

	COverlayDataSource* GetItemSource(int iItem);

	void InitSourcesList();
	void UpdateSourcesList();
	void SelectSources();
	void UpdateButtons();

	CCheckedListCtrl m_sourcesListCtrl;

// Dialog Data
	enum { IDD = IDD_OVERLAY_DATA_SOURCES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlayDataSourcesList		m_sources;
	COverlay*					m_lpOverlay;
	UINT						m_nTimerID;

	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSetupSourceButton();
	afx_msg void OnBnClickedAddSourceButton();
	afx_msg void OnBnClickedRemoveSourceButton();
	afx_msg void OnBnClickedOk();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
/////////////////////////////////////////////////////////////////////////////
