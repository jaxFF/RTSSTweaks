// TextTableCellAddDlg.h: interface for the CTextTableCellAddDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "resource.h"

#include "Overlay.h"
#include "TextTable.h"
#include "CheckedListCtrl.h"
#include "afxcmn.h"
//////////////////////////////////////////////////////////////////////
// CTextTableCellAddDlg dialog
//////////////////////////////////////////////////////////////////////
class CTextTableCellAddDlg : public CDialog
{
	DECLARE_DYNAMIC(CTextTableCellAddDlg)

public:
	CTextTableCellAddDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextTableCellAddDlg();

	void SetOverlay(COverlay* lpOverlay);
	void SetTable(CTextTable* lpTable);

	void InitSourcesList();
	void UpdateSourcesList();
	void UpdateLineControls();

	int FindItem(LPCSTR lpItemText);

	DWORD	m_dwLineID;
	BOOL	m_bAddToNewLine;
	CString m_strLineName;

	CCheckedListCtrl m_sourcesListCtrl;

// Dialog Data
	enum { IDD = IDD_TEXT_TABLE_CELL_ADD_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*	m_lpOverlay;
	CTextTable* m_lpTable;
	UINT		m_nTimerID;

	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedAddToNewLineCheck();
};
//////////////////////////////////////////////////////////////////////
