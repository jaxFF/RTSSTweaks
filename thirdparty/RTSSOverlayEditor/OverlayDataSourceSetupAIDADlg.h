// OverlayDataSourceSetupAIDADlg.h: interface for the COverlayDataSourceSetupAIDADlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "OverlayDataSourceAIDA.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupAIDADlg dialog
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceSetupAIDADlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayDataSourceSetupAIDADlg)

public:
	COverlayDataSourceSetupAIDADlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayDataSourceSetupAIDADlg();

	void SetSource(COverlayDataSourceAIDA* lpSource); 
	BOOL ValidateSource();

	CString m_strSensorID;
	CString m_strSourceName;
	CString m_strSourceUnits;
	CString m_strSourceFormat;
	CString m_strSourceFormula;

// Dialog Data
	enum { IDD = IDD_OVERLAY_DATA_SOURCE_SETUP_AIDA_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlayDataSourceAIDA*	m_lpSource;
	COverlayDataSourceAIDA	m_source;
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
