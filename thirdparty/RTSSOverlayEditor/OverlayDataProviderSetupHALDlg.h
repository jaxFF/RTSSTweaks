// OverlayDataProviderSetupHALDlg.h: interface for the COverlayDataSourceSetupAIDADlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataProviderSetupHALDlg dialog
/////////////////////////////////////////////////////////////////////////////
class COverlayDataProviderSetupHALDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayDataProviderSetupHALDlg)

public:
	COverlayDataProviderSetupHALDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayDataProviderSetupHALDlg();

	BOOL m_bEnableLowLevelIODriver;
	BOOL m_bEnableSMARTMonitoring;
	BOOL m_bRestartRequired;

// Dialog Data
	enum { IDD = IDD_OVERLAY_DATA_PROVIDER_SETUP_HAL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedEnableLowLevelIoDriverCheck();
	afx_msg void OnBnClickedEnableSMARTMonitoringCheck();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
/////////////////////////////////////////////////////////////////////////////
