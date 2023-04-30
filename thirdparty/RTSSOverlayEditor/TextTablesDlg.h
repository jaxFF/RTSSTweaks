// TextTablesDlg.h: interface for the CTextTablesDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "CheckedListCtrl.h"
#include "afxcmn.h"
//////////////////////////////////////////////////////////////////////
// CTextTablesDlg dialog
//////////////////////////////////////////////////////////////////////
class CTextTablesDlg : public CDialog
{
	DECLARE_DYNAMIC(CTextTablesDlg)

public:
	CTextTablesDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextTablesDlg();

	void SetOverlay(COverlay* lpOverlay);

	void InitTablesList();
	void SelectTables();
	void UpdateButtons();

	void Copy();
	void Paste();

	CTextTable* GetItemTable(int iItem);

	CTextTablesList m_tables;
	CTextTable		m_clipboard;

	CCheckedListCtrl m_tablesListCtrl;

// Dialog Data
	enum { IDD = IDD_TEXT_TABLES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnOK();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	COverlay* m_lpOverlay;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSetupTableButton();
	afx_msg void OnBnClickedAddTableButton();
	afx_msg void OnBnClickedRemoveTableButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
//////////////////////////////////////////////////////////////////////
