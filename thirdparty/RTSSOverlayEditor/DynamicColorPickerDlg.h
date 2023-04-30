// DynamicColorPickerDlg.h: interface for the CDynamicColorPickerDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "afxwin.h"
#include "DynamicColor.h"
#include "ColorPreviewWnd.h"
#include "RecentDynamicColorsWnd.h"
/////////////////////////////////////////////////////////////////////////////
// CDynamicColorPickerDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CDynamicColorPickerDlg : public CDialog
{
	DECLARE_DYNAMIC(CDynamicColorPickerDlg)

public:
	CDynamicColorPickerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDynamicColorPickerDlg();

	void	SetOverlay(COverlay* lpOverlay);
	void	ColorToVars(CDynamicColor* lpColor);
	void	VarsToColor(CDynamicColor* lpColor);
	void	SetRecentColors(LPDYNAMIC_COLOR_DESC lpRecentColors);

	void	InitDataSourcesList();
	void	InitRangeControls(CDynamicColor* lpColor);
	void	UpdateRangeControls();
	void	UpdateColorControls();
	void	UpdatePreviewControls();
	
	CDynamicColor				m_color;
	CColorPreviewWnd			m_dynamicColorPreviewWnd;
	CRecentDynamicColorsWnd		m_recentDynamicColorsWnd;

	CComboBox					m_ctrlDataSourcesCombo;
	CScrollBar					m_ctrlDynamicColorPreviewScrollbar;
	int							m_nDynamicColorPreviewPos;

	DYNAMIC_COLOR_DESC			m_recentColors[32];

	DWORD m_dwRange0Color;
	DWORD m_dwRange1Color;
	DWORD m_dwRange2Color;
	DWORD m_dwRange3Color;
	DWORD m_dwRange4Color;

	CString m_strRange0Min;
	CString m_strRange0Max;
	CString m_strRange1Min;
	CString m_strRange1Max;
	CString m_strRange2Min;
	CString m_strRange2Max;
	CString m_strRange3Min;
	CString m_strRange3Max;
	CString m_strRange4Min;
	CString m_strRange4Max;

	HBRUSH	m_hBrushRange0ColorPreview;
	HBRUSH	m_hBrushRange1ColorPreview;
	HBRUSH	m_hBrushRange2ColorPreview;
	HBRUSH	m_hBrushRange3ColorPreview;
	HBRUSH	m_hBrushRange4ColorPreview;

// Dialog Data
	enum { IDD = IDD_DYNAMIC_COLOR_PICKER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*	m_lpOverlay;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedLessButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeRange0MinEdit();
	afx_msg void OnEnChangeRange0MaxEdit();
	afx_msg void OnEnChangeRange1MinEdit();
	afx_msg void OnEnChangeRange1MaxEdit();
	afx_msg void OnEnChangeRange2MinEdit();
	afx_msg void OnEnChangeRange2MaxEdit();
	afx_msg void OnEnChangeRange3MinEdit();
	afx_msg void OnEnChangeRange3MaxEdit();
	afx_msg void OnEnChangeRange4MinEdit();
	afx_msg void OnEnChangeRange4MaxEdit();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedRange0ColorPreview();
	afx_msg void OnStnClickedRange1ColorPreview();
	afx_msg void OnStnClickedRange2ColorPreview();
	afx_msg void OnStnClickedRange3ColorPreview();
	afx_msg void OnStnClickedRange4ColorPreview();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	BOOL m_bBlendColors;
	BOOL m_bEmbedFormula;
	afx_msg void OnBnClickedBlendColorsCheck();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnCbnSelchangeDataSourcesCombo();
};
/////////////////////////////////////////////////////////////////////////////
