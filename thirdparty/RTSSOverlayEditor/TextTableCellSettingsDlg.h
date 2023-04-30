// TextTableCellSettingsDlg.h: interface for the CTextTableCellSettingsDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "Overlay.h"
#include "afxwin.h"
//////////////////////////////////////////////////////////////////////
// CTextTableCellSettingsDlg dialog
//////////////////////////////////////////////////////////////////////
class CTextTableCellSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CTextTableCellSettingsDlg)

public:
	CTextTableCellSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextTableCellSettingsDlg();

	DWORD GetDynamicColor(CString strColor, CDynamicColor& color);
	DWORD GetDynamicLineColor(CDynamicColor& color);
	DWORD GetDynamicCellColor(CDynamicColor& color);

	void SetOverlay(COverlay* lpOverlay);
	void SetLine(CTextTableLine* lpLine);
	void SetCell(CTextTableCell* lpCell);

	void UpdateColorControls();

	CString m_strLineName;
	BOOL	m_bUseCustomLineColor;
	
	CString m_strCellSource;
	CString m_strCellText;
	BOOL	m_bUseCustomCellColor;

	CComboBox m_ctrlDataSourcesCombo;

// Dialog Data
	enum { IDD = IDD_TEXT_TABLE_CELL_PROPERTIES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*		m_lpOverlay;
	CTextTableLine*	m_lpLine;
	CTextTableCell*	m_lpCell;
	HBRUSH			m_hBrushLineColorPreview;
	HBRUSH			m_hBrushCellColorPreview;

	CString			m_strLineColor;
	CString			m_strCellColor;

	DECLARE_MESSAGE_MAP()

	virtual void OnOK();

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedLineColorPreview();
	afx_msg void OnStnClickedCellColorPreview();
	afx_msg void OnBnClickedUseCustomLineColorCheck();
	afx_msg void OnBnClickedUseCustomCellColorCheck();
	afx_msg void OnCbnSelchangeDataSourcesCombo();
	afx_msg void OnEnChangeCellTextEdit();
};
//////////////////////////////////////////////////////////////////////
