// OverlayDataSourceSetupHwInfoDlg.h: interface for the COverlayDataSourceSetupHwInfoDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "OverlayDataSourceHwInfo.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupHwInfoDlg dialog
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceSetupHwInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayDataSourceSetupHwInfoDlg)

public:
	COverlayDataSourceSetupHwInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayDataSourceSetupHwInfoDlg();

	void SetSource(COverlayDataSourceHwInfo* lpSource); 
	BOOL ValidateSource();

	CComboBox	m_readingTypeCombo;

	CString		m_strSensorInstance;
	CString		m_strReadingName;
	CString		m_strSourceName;
	CString		m_strSourceUnits;
	CString		m_strSourceFormat;
	CString		m_strSourceFormula;

// Dialog Data
	enum { IDD = IDD_OVERLAY_DATA_SOURCE_SETUP_HWINFO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlayDataSourceHwInfo*	m_lpSource;
	COverlayDataSourceHwInfo	m_source;
	HBRUSH						m_hBrush;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeFormulaEdit();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg void OnEnChangeSourceFormatEdit();
};
/////////////////////////////////////////////////////////////////////////////
