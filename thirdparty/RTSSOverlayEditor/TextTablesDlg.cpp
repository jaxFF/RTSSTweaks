// TextTablesDlg.cpp : implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "TextTablesDlg.h"
#include "TextTableSettingsDlg.h"
//////////////////////////////////////////////////////////////////////
// CTextTablesDlg dialog
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTextTablesDlg, CDialog)
//////////////////////////////////////////////////////////////////////
CTextTablesDlg::CTextTablesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextTablesDlg::IDD, pParent)
{
	m_lpOverlay = NULL;
}
//////////////////////////////////////////////////////////////////////
CTextTablesDlg::~CTextTablesDlg()
{
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABLES_LIST, m_tablesListCtrl);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTextTablesDlg, CDialog)
	ON_BN_CLICKED(IDC_SETUP_TABLE_BUTTON, &CTextTablesDlg::OnBnClickedSetupTableButton)
	ON_BN_CLICKED(IDC_ADD_TABLE_BUTTON, &CTextTablesDlg::OnBnClickedAddTableButton)
	ON_BN_CLICKED(IDC_REMOVE_TABLE_BUTTON, &CTextTablesDlg::OnBnClickedRemoveTableButton)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// CTextTablesDlg message handlers
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
//////////////////////////////////////////////////////////////////////
BOOL CTextTablesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	if (m_lpOverlay)
	{
		//initialize local tables list

		m_tables.SetOverlay(m_lpOverlay);
		m_tables.Copy(m_lpOverlay->GetTextTablesList());

		//init tables list control

		m_tablesListCtrl.ShowScrollBar(SB_VERT, TRUE);

		CRect cr; m_tablesListCtrl.GetClientRect(&cr);

		DWORD dwWidth		= cr.Width();

		m_tablesListCtrl.SetResourceHandle(g_hModule);
		m_tablesListCtrl.InsertColumn(0, LocalizeStr("Table")	, LVCFMT_LEFT, dwWidth);

		CHeaderCtrl* pHeader = m_tablesListCtrl.GetHeaderCtrl();

		DWORD dwStyle = GetWindowLong(pHeader->m_hWnd ,GWL_STYLE );
		dwStyle &= ~HDS_FULLDRAG;
		SetWindowLong(pHeader->m_hWnd  , GWL_STYLE, dwStyle );

		m_tablesListCtrl.EnableItemSelection(FALSE);
		m_tablesListCtrl.EnableMultiSelection(TRUE);

		InitTablesList();

		UpdateButtons();
	}
	
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::OnOK()
{
	if (m_lpOverlay)
	{
		m_lpOverlay->GetTextTablesList()->Copy(&m_tables);
	}

	CDialog::OnOK();
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::InitTablesList()
{
	m_tablesListCtrl.SetRedraw(FALSE);
	m_tablesListCtrl.DeleteAllItems();

	DWORD dwTable = 0;

	POSITION pos = m_tables.GetHeadPosition();

	while (pos)
	{
		CTextTable* lpTable = m_tables.GetNext(pos);

		int iItem = m_tablesListCtrl.InsertItem(dwTable, lpTable->m_strName.IsEmpty() ? LocalizeStr("<nameless>") : lpTable->m_strName);

		m_tablesListCtrl.SetItemData(iItem, lpTable->GetTableID());

		dwTable++;
	}

	m_tablesListCtrl.Init();
	SelectTables();
	m_tablesListCtrl.SetRedraw(TRUE);
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::UpdateButtons()
{
	BOOL bEnable = FALSE;

	int nSelectedItem = m_tablesListCtrl.GetFocusedItem();

	if (nSelectedItem != -1)
		bEnable = TRUE;

	GetDlgItem(IDC_SETUP_TABLE_BUTTON)->EnableWindow(bEnable);
	GetDlgItem(IDC_REMOVE_TABLE_BUTTON)->EnableWindow(bEnable);
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::SelectTables()
{
	int iFocus = -1;

	int iItems = m_tablesListCtrl.GetItemCount();

	for (int iItem=0; iItem<iItems; iItem++)
	{
		CTextTable* lpTable = GetItemTable(iItem);

		if (lpTable)
		{
			if (lpTable->m_bSelected)
			{
				if (iFocus == -1)
				{
					m_tablesListCtrl.SetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

					iFocus = iItem;
				}
				else
					m_tablesListCtrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);

				lpTable->m_bSelected = FALSE;
			}
		}
	}

	if (iFocus != -1)
		m_tablesListCtrl.EnsureVisible(iFocus, FALSE);
}
//////////////////////////////////////////////////////////////////////
CTextTable* CTextTablesDlg::GetItemTable(int iItem)
{
	return m_tables.FindTable(m_tablesListCtrl.GetItemData(iItem) & 0xFFFFFF);
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::OnBnClickedSetupTableButton()
{
	int iFocus = m_tablesListCtrl.GetFocusedItem();

	if (iFocus != -1)
	{
		CTextTable* lpTable = GetItemTable(iFocus);

		if (lpTable)
		{
			CTextTableSettingsDlg dlg;

			dlg.SetOverlay(m_lpOverlay);
			dlg.m_table.Copy(lpTable);

			if (IDOK == dlg.DoModal())
			{
				lpTable->Copy(&dlg.m_table);

				InitTablesList();

				m_tablesListCtrl.SetItemState(iFocus, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::OnBnClickedAddTableButton()
{
	CTextTable* lpTable = new CTextTable;

	CTextTableSettingsDlg dlg;

	dlg.SetOverlay(m_lpOverlay);
	dlg.m_table.Copy(lpTable);

	if (IDOK == dlg.DoModal())
	{
		lpTable->Copy(&dlg.m_table);

		lpTable->m_bSelected = TRUE;

		m_tables.AddTable(lpTable);

		InitTablesList();
	}
	else
		delete lpTable;
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::OnBnClickedRemoveTableButton()
{
	int iItems	= m_tablesListCtrl.GetItemCount();

	for (int iItem=iItems-1; iItem>=0; iItem--)
	{
		if (m_tablesListCtrl.GetItemState(iItem, LVIS_SELECTED))
		{
			CTextTable* lpTable = GetItemTable(iItem);

			m_tables.RemoveTable(lpTable);

			m_tablesListCtrl.DeleteItem(iItem);
		}
	}

	m_tables.UpdateTableIDs();

	int iFocus = m_tablesListCtrl.GetFocusedItem();

	InitTablesList();

	if (iFocus != -1)
		m_tablesListCtrl.SetItemState(iFocus, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	UpdateButtons();
}
//////////////////////////////////////////////////////////////////////
LRESULT CTextTablesDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_SEL_CHANGED)
		UpdateButtons();

	if (message == UM_DBLCLICK_ITEM)
		OnBnClickedSetupTableButton();

	if (message == UM_DROP_ITEM)
	{	
		//create reordering list 

		CList<CTextTable*, CTextTable*> order;

		int iItems = m_tablesListCtrl.GetItemCount();
	
		for (int iItem=0; iItem<iItems; iItem++)
		{
			CTextTable* lpTable = GetItemTable(iItem);

			if (m_tablesListCtrl.GetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED))
				lpTable->m_bSelected = TRUE;

			order.AddTail(lpTable);
		}

		//reorder data sources 

		m_tables.Reorder(&order);

		InitTablesList();
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////
BOOL CTextTablesDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		BOOL bCtrl = GetAsyncKeyState(VK_CONTROL) < 0;

		if (pMsg->wParam == 'C')
		{
			if (bCtrl)
				Copy();
		}
		else
		if (pMsg->wParam == 'V')
		{
			if (bCtrl)
				Paste();
		}
		else
		if (pMsg->wParam == VK_DELETE)
		{
			if (pMsg->hwnd == m_tablesListCtrl.m_hWnd)
				OnBnClickedRemoveTableButton();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::Copy()
{
	int iFocus = m_tablesListCtrl.GetFocusedItem();

	if (iFocus != -1)
		m_clipboard.Copy(GetItemTable(iFocus));
}
//////////////////////////////////////////////////////////////////////
void CTextTablesDlg::Paste()
{
	if (m_clipboard.GetCount())
	{
		CTextTable* lpTable = new CTextTable;

		lpTable->Copy(&m_clipboard);

		lpTable->m_bSelected = TRUE;

		m_tables.AddTable(lpTable);

		InitTablesList();
	}
}
//////////////////////////////////////////////////////////////////////
