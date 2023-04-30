// TextTableSettingsDlg.h: interface for the CTextTableSettingsDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "resource.h"
//////////////////////////////////////////////////////////////////////
// CTextTableSettingsDlg dialog
//////////////////////////////////////////////////////////////////////
#include "TextTable.h"
#include "CheckedListCtrl.h"
#include "afxcmn.h"
//////////////////////////////////////////////////////////////////////
class CTextTableSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CTextTableSettingsDlg)

public:
	CTextTableSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTextTableSettingsDlg();

	void SetOverlay(COverlay* lpOverlay);

	void InitCellsList();
	void SelectCells();

	void UpdateButtons();

	CTextTableLine* GetItemLine(int iItem);
	CTextTableCell* GetItemCell(int iItem);

	DWORD GetLineID(DWORD dwItemData);
	DWORD GetCellID(DWORD dwItemData);
	DWORD GetItemData(DWORD dwLineID, DWORD dwCellID);

	void Undo();
	void SaveUndo();

	void Copy();
	void Paste();

	CTextTable	m_table;
	CTextTable	m_tableUndo;

	CTextTableLine m_clipboardLine;
	CTextTableCell m_clipboardCell;

	CString		m_strName;
	CString		m_strText;

	int			m_nValueSize;
	int			m_nValueAlignment;
	int			m_nUnitsSize;
	int			m_nUnitsAlignment;

	BOOL		m_bEditCells;

	CCheckedListCtrl m_cellsListCtrl;

// Dialog Data
	enum { IDD = IDD_TEXT_TABLE_PROPERTIES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*	m_lpOverlay;

	virtual void OnOK();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSetupCellButton();
	afx_msg void OnBnClickedAddCellButton();
	afx_msg void OnBnClickedRemoveCellButton();
	afx_msg void OnDeltaposValueSizeSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposValueAlignmentSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposUnitsSizeSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposUnitsAlignmentSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedLinesRadio();
	afx_msg void OnBnClickedCellsRadio();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
//////////////////////////////////////////////////////////////////////
