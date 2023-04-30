// OverlaySettingsDlg.h: interface for the COverlaySettingsDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "Overlay.h"
//////////////////////////////////////////////////////////////////////
// COverlaySettingsDlg dialog
//////////////////////////////////////////////////////////////////////
class COverlaySettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlaySettingsDlg)

public:
	COverlaySettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlaySettingsDlg();

	void SetOverlay(COverlay* lpOverlay);

	CString m_strName;
	int		m_nRefreshPeriod;
	CString m_strEmbeddedImage;
	CString m_strPingAddr;

// Dialog Data
	enum { IDD = IDD_OVERLAY_PROPERTIES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay* m_lpOverlay;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBrowseForEmbeddedImage();
	afx_msg void OnDeltaposRefreshPeriodSpin(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMasterSettings();
};
//////////////////////////////////////////////////////////////////////
