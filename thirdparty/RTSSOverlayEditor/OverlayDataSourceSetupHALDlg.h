// OverlayDataSourceSetupHALDlg.h: interface for the COverlayDataSourceSetupHALDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "OverlayDataSourceHAL.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupHALDlg dialog
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceSetupHALDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayDataSourceSetupHALDlg)

public:
	COverlayDataSourceSetupHALDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayDataSourceSetupHALDlg();

	void SetSource(COverlayDataSourceHAL* lpSource); 
	BOOL ValidateSource();

	CString m_strSensorID;
	CString m_strSourceName;
	CString m_strSourceUnits;
	CString m_strSourceFormat;
	CString m_strSourceFormula;

// Dialog Data
	enum { IDD = IDD_OVERLAY_DATA_SOURCE_SETUP_HAL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlayDataSourceHAL*	m_lpSource;
	COverlayDataSourceHAL	m_source;
	HBRUSH					m_hBrush;

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
