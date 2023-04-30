// GraphSettingsDlg.h: interface for the CGraphSettingsDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "afxwin.h"
//////////////////////////////////////////////////////////////////////
// CGraphSettingsDlg dialog
//////////////////////////////////////////////////////////////////////
class CGraphSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CGraphSettingsDlg)

public:
	CGraphSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGraphSettingsDlg();

	void SetOverlay(COverlay* lpOverlay);

	void UpdateGraphControls();
	void UpdateRangeControls();

	CString m_strSource;
	CString m_strText;

	int		m_nGraphTemplate;
	BOOL	m_bResizeToExtent;
	int		m_nGraphWidth;
	int		m_nGraphHeight;
	int		m_nGraphMargin;
	float	m_fltGraphMin;
	float	m_fltGraphMax;
	BOOL	m_bGraphAutoscale;
	DWORD	m_dwGraphFlags;

	int		m_nGraphStyle;
	int		m_nGraphBgnd;
	int		m_nGraphOrientation;
	int		m_nGraphDirection;

	CString m_strGraphMin;
	CString m_strGraphMax;

	CComboBox m_ctrlDataSourcesCombo;

// Dialog Data
	enum { IDD = IDD_GRAPH_PROPERTIES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*	m_lpOverlay;
	HBRUSH		m_hBrush;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeGraphTemplateCombo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedAddGraphCheck();
	afx_msg void OnDeltaposGraphWidthSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposGraphHeightSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposGraphMarginSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedResizeToExtent();
	afx_msg void OnBnClickedGraphAutoscaleCheck();
};
//////////////////////////////////////////////////////////////////////
