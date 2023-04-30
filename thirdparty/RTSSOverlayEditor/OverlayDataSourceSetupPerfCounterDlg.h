// OverlayDataSourceSetupPerfCounterDlg.h: interface for the COverlayDataSourceSetupPerfCounterDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "OverlayDataSourcePerfCounter.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupPerfCounterDlg dialog
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceSetupPerfCounterDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayDataSourceSetupPerfCounterDlg)

public:
	COverlayDataSourceSetupPerfCounterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayDataSourceSetupPerfCounterDlg();

	void SetSource(COverlayDataSourcePerfCounter* lpSource); 
	BOOL ValidateSource();
	BOOL ValidateInt(LPCSTR lpLine);

	CString m_strObjectName;
	CString m_strObjectInstance;
	CString m_strCounterName;

	BOOL	m_bDynamic;

	CString m_strSourceName;
	CString m_strSourceUnits;
	CString m_strSourceFormat;
	CString m_strSourceFormula;

// Dialog Data
	enum { IDD = IDD_OVERLAY_DATA_SOURCE_SETUP_PERFCOUNTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlayDataSourcePerfCounter*	m_lpSource;
	COverlayDataSourcePerfCounter	m_source;
	HBRUSH							m_hBrush;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeSourceFormatEdit();
	afx_msg void OnEnChangeFormulaEdit();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelectInstanceButton();
};
/////////////////////////////////////////////////////////////////////////////
