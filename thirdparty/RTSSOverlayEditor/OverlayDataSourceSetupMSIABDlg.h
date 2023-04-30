// OverlayDataSourceSetupMSIABDlg.h: interface for the COverlayDataSourceSetupMSIABDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "OverlayDataSourceMSIAB.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupMSIABDlg dialog
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceSetupMSIABDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayDataSourceSetupMSIABDlg)

public:
	COverlayDataSourceSetupMSIABDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayDataSourceSetupMSIABDlg();

	void SetSource(COverlayDataSourceMSIAB* lpSource); 
	BOOL ValidateSource();

	CComboBox m_sensorTypeCombo;

	CString m_strSensorInstance;
	CString m_strSensorName;
	CString m_strSourceName;
	CString m_strSourceUnits;
	CString m_strSourceFormat;
	CString m_strSourceFormula;

// Dialog Data
	enum { IDD = IDD_OVERLAY_DATA_SOURCE_SETUP_MSIAB_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlayDataSourceMSIAB*	m_lpSource;
	COverlayDataSourceMSIAB		m_source;
	HBRUSH						m_hBrush;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeSourceFormatEdit();
	afx_msg void OnEnChangeFormulaEdit();
	afx_msg void OnBnClickedOk();
};
/////////////////////////////////////////////////////////////////////////////
