// LayerSettingsDlg.h: interface for the CLayerSettingsDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"
#include "Overlay.h"
#include "HypertextEditCtrl.h"
#include "OverlayEditorWnd.h"
/////////////////////////////////////////////////////////////////////////////
// CLayerSettingsDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CLayerSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CLayerSettingsDlg)

public:
	CLayerSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLayerSettingsDlg();

	DWORD	GetDynamicColor(CString strColor, CDynamicColor& color);
	DWORD	GetDynamicTextColor(CDynamicColor& color);
	DWORD	GetDynamicBgndColor(CDynamicColor& color);
	DWORD	GetDynamicBorderColor(CDynamicColor& color);

	void	SetOverlay(COverlay* lpOverlay);
	void	SetLayer(COverlayLayer* lpLayer);
	void	SetEditorWnd(COverlayEditorWnd* lpEditorWnd);

	void	UpdateColorControls();
	void	UpdateObjectControls();
	void	UpdateExtentControls();
	void	UpdateStickyControls();

	void	PreApply();

	void	OnBrowseMacro(int nMode = 0);

	CString m_strName;
	int		m_nRefreshPeriod;
	CString m_strText;
	CString	m_strTextColor;
	CString	m_strBgndColor;
	CString	m_strBorderColor;
	int		m_nSize;
	int		m_nAlignment;
	BOOL	m_bFixedAlignment;
	int		m_nExtentX;
	int		m_nExtentY;
	int		m_nExtentOrigin;

	BOOL	m_bPosSticky;
	int		m_nPosSticky;

	BOOL	m_bUseCustomTextColor;
	BOOL	m_bUseCustomBgndColor;
	BOOL	m_bBorder;
	BOOL	m_bResizeToContent;

	int		m_nBorderSize;

	BOOL	m_bResetInitialSelection;

	CHypertextEditCtrl m_ctrlTextEdit;

	int m_nMarginLeft;
	int m_nMarginTop;
	int m_nMarginRight;
	int m_nMarginBottom;
	
// Dialog Data
	enum { IDD = IDD_LAYER_PROPERTIES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	COverlay*			m_lpOverlay;
	COverlayLayer*		m_lpLayer;
	COverlayEditorWnd*	m_lpEditorWnd;
	HBRUSH				m_hBrushTextColorPreview;
	HBRUSH				m_hBrushBgndColorPreview;
	HBRUSH				m_hBrushBorderColorPreview;

	virtual void OnOK();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedTextColorPreview();
	afx_msg void OnStnClickedBgndColorPreview();
	afx_msg void OnBnClickedUseCustomTextColorCheck();
	afx_msg void OnBnClickedUseCustomBgndColorCheck();
	afx_msg void OnDeltaposSizeSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposAlignmentSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddEmbeddedObject();
	afx_msg void OnBnClickedEditEmbeddedObject();
	afx_msg void OnSelectMacro(UINT nID);
	afx_msg void OnSelectTag(UINT nID);
	afx_msg void OnBnClickedExtentOrigin0();
	afx_msg void OnBnClickedExtentOrigin1();
	afx_msg void OnBnClickedExtentOrigin2();
	afx_msg void OnBnClickedExtentOrigin3();
	afx_msg void OnBnClickedExtentOrigin4();
	afx_msg void OnBnClickedExtentOrigin5();
	afx_msg void OnBnClickedExtentOrigin6();
	afx_msg void OnBnClickedExtentOrigin7();
	afx_msg void OnBnClickedExtentOrigin8();
	afx_msg void OnBnClickedResizeToContentCheck();
	afx_msg void OnDeltaposRefreshPeriodSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPosSticky0();
	afx_msg void OnBnClickedPosSticky1();
	afx_msg void OnBnClickedPosSticky2();
	afx_msg void OnBnClickedPosSticky3();
	afx_msg void OnBnClickedPosSticky4();
	afx_msg void OnBnClickedPosSticky5();
	afx_msg void OnBnClickedPosSticky6();
	afx_msg void OnBnClickedPosSticky7();
	afx_msg void OnBnClickedPosSticky8();
	afx_msg void OnBnClickedPosStickyCheck();
	afx_msg void OnBnClickedApply();
	afx_msg void OnStnClickedBorderColorPreview();
	afx_msg void OnBnClickedBorderColorCheck();
	afx_msg void OnDeltaposBorderSizeSpin(NMHDR *pNMHDR, LRESULT *pResult);
};
/////////////////////////////////////////////////////////////////////////////
