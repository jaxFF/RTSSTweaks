// TextTableSettingsDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "TextTableSettingsDlg.h"
#include "TextTableCellAddDlg.h"
#include "TextTableCellSettingsDlg.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
// CTextTableSettingsDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTextTableSettingsDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
CTextTableSettingsDlg::CTextTableSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextTableSettingsDlg::IDD, pParent)
	, m_bEditCells(FALSE)
{
	m_lpOverlay			= NULL;

	m_strName			= "";
	m_nValueSize		= DEFAULT_TABLE_VALUE_SIZE;
	m_nValueAlignment	= DEFAULT_TABLE_VALUE_ALIGNMENT;
	m_nUnitsSize		= DEFAULT_TABLE_UNITS_SIZE;
	m_nUnitsAlignment	= DEFAULT_TABLE_UNITS_ALIGNMENT;

	m_bEditCells		= TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CTextTableSettingsDlg::~CTextTableSettingsDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
	DDX_Text(pDX, IDC_VALUE_SIZE_EDIT, m_nValueSize);
	DDV_MinMaxInt(pDX, m_nValueSize, -500, 500);
	DDX_Text(pDX, IDC_VALUE_ALIGNMENT_EDIT, m_nValueAlignment);
	DDV_MinMaxInt(pDX, m_nValueAlignment, -500, 500);
	DDX_Text(pDX, IDC_UNITS_SIZE_EDIT, m_nUnitsSize);
	DDV_MinMaxInt(pDX, m_nUnitsSize, -500, 500);
	DDX_Text(pDX, IDC_UNITS_ALIGNMENT_EDIT, m_nUnitsAlignment);
	DDV_MinMaxInt(pDX, m_nUnitsAlignment, -500, 500);
	DDX_Control(pDX, IDC_CELLS_LIST, m_cellsListCtrl);
	DDX_Radio(pDX, IDC_LINES_RADIO, m_bEditCells);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTextTableSettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_SETUP_CELL_BUTTON, &CTextTableSettingsDlg::OnBnClickedSetupCellButton)
	ON_BN_CLICKED(IDC_ADD_CELL_BUTTON, &CTextTableSettingsDlg::OnBnClickedAddCellButton)
	ON_BN_CLICKED(IDC_REMOVE_CELL_BUTTON, &CTextTableSettingsDlg::OnBnClickedRemoveCellButton)
	ON_NOTIFY(UDN_DELTAPOS, IDC_VALUE_SIZE_SPIN, &CTextTableSettingsDlg::OnDeltaposValueSizeSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_VALUE_ALIGNMENT_SPIN, &CTextTableSettingsDlg::OnDeltaposValueAlignmentSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UNITS_SIZE_SPIN, &CTextTableSettingsDlg::OnDeltaposUnitsSizeSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_UNITS_ALIGNMENT_SPIN, &CTextTableSettingsDlg::OnDeltaposUnitsAlignmentSpin)
	ON_BN_CLICKED(IDC_LINES_RADIO, &CTextTableSettingsDlg::OnBnClickedLinesRadio)
	ON_BN_CLICKED(IDC_CELLS_RADIO, &CTextTableSettingsDlg::OnBnClickedCellsRadio)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CTextTableSettingsDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTextTableSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	//init cells list control

	m_cellsListCtrl.ShowScrollBar(SB_VERT, TRUE);

	CRect cr; m_cellsListCtrl.GetClientRect(&cr);

	DWORD dwWidth		= cr.Width() / 2;
	DWORD dwWidthChunk	= cr.Width() % 2;

	m_cellsListCtrl.SetResourceHandle(g_hModule);
	m_cellsListCtrl.InsertColumn(0, LocalizeStr("Line")	, LVCFMT_LEFT, dwWidth);
	m_cellsListCtrl.InsertColumn(1, LocalizeStr("Cell")	, LVCFMT_LEFT, dwWidth + dwWidthChunk);

	CHeaderCtrl* pHeader = m_cellsListCtrl.GetHeaderCtrl();

	DWORD dwStyle = GetWindowLong(pHeader->m_hWnd ,GWL_STYLE );
	dwStyle &= ~HDS_FULLDRAG;
	SetWindowLong(pHeader->m_hWnd  , GWL_STYLE, dwStyle );

	m_cellsListCtrl.EnableSecondColumn(TRUE);
	m_cellsListCtrl.EnableItemSelection(FALSE);
	m_cellsListCtrl.EnableMultiSelection(TRUE);
	m_cellsListCtrl.SetDragImageColumn(1);

	InitCellsList();
	UpdateButtons();

	m_strName			= m_table.m_strName;
	m_nValueSize		= m_table.m_nValueSize;
	m_nValueAlignment	= m_table.m_nValueAlignment;
	m_nUnitsSize		= m_table.m_nUnitsSize;
	m_nUnitsAlignment	= m_table.m_nUnitsAlignment;

	UpdateData(FALSE);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CTextTableSettingsDlg::GetLineID(DWORD dwItemData)
{
	return (dwItemData>>8) & 0xFF;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CTextTableSettingsDlg::GetCellID(DWORD dwItemData)
{
	return dwItemData & 0xFF;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CTextTableSettingsDlg::GetItemData(DWORD dwLineID, DWORD dwCellID)
{
	return (dwLineID<<8) + dwCellID;
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::InitCellsList()
{
	m_cellsListCtrl.SetRedraw(FALSE);
	m_cellsListCtrl.DeleteAllItems();

	DWORD dwIndex = 0;

	POSITION posLine = m_table.GetHeadPosition();

	while (posLine)
	{
		CTextTableLine* lpLine = m_table.GetNext(posLine);

		DWORD dwLineID = lpLine->GetLineID();

		CString strLine = strlen(lpLine->m_strName) ? lpLine->m_strName : LocalizeStr("<nameless>");
		CString strCells;

		POSITION posCell = lpLine->GetHeadPosition();

		while (posCell)
		{
			CTextTableCell* lpCell = lpLine->GetNext(posCell);

			DWORD dwCellID = lpCell->GetCellID();

			CString strCell;

			CString strSource = lpCell->m_strSource;

			if (!strSource.IsEmpty())
			{
				COverlayDataSource* lpSource = m_lpOverlay->FindDataSource(strSource);

				if (lpSource)
					strCell = strSource;
				else
					strCell = LocalizeStr("<invalid source>");
			}
			else
			{
				if (strlen(lpCell->m_strText))
					strCell = lpCell->m_strText;
				else
					strCell = LocalizeStr("<separator>");
			}

			if (m_bEditCells)
			{
				int iItem = m_cellsListCtrl.InsertItem(dwIndex, dwCellID ? "" : strLine);

				m_cellsListCtrl.SetItemText(iItem, 1, strCell);

				m_cellsListCtrl.SetItemData(iItem, GetItemData(dwLineID, dwCellID));

				dwIndex++;
			}
			else
			{
				if (!strCells.IsEmpty())
					strCells += ", ";

				strCells += strCell;
			}
		}

		if (!m_bEditCells)
		{
			int iItem = m_cellsListCtrl.InsertItem(dwIndex, strLine);

			m_cellsListCtrl.SetItemText(iItem, 1, strCells);

			m_cellsListCtrl.SetItemData(iItem, GetItemData(dwLineID, 0));

			dwIndex++;
		}
	}

	m_cellsListCtrl.Init();
	SelectCells();
	m_cellsListCtrl.SetRedraw(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::SelectCells()
{
	int iFocus = -1;

	int iItems = m_cellsListCtrl.GetItemCount();

	if (m_bEditCells)
	{
		for (int iItem=0; iItem<iItems; iItem++)
		{
			CTextTableCell* lpCell = GetItemCell(iItem);

			if (lpCell)
			{
				if (lpCell->m_bSelected)
				{
					if (iFocus == -1)
					{
						m_cellsListCtrl.SetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

						iFocus = iItem;
					}
					else
						m_cellsListCtrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);

					lpCell->m_bSelected = FALSE;
				}
			}
		}
	}
	else
	{
		for (int iItem=0; iItem<iItems; iItem++)
		{
			CTextTableLine* lpLine = GetItemLine(iItem);

			if (lpLine)
			{
				if (lpLine->m_bSelected)
				{
					if (iFocus == -1)
					{
						m_cellsListCtrl.SetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

						iFocus = iItem;
					}
					else
						m_cellsListCtrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);

					lpLine->m_bSelected = FALSE;
				}
			}
		}
	}

	if (iFocus != -1)
		m_cellsListCtrl.EnsureVisible(iFocus, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::UpdateButtons()
{
	BOOL bEnable = FALSE;

	int nSelectedItem = m_cellsListCtrl.GetFocusedItem();

	if (nSelectedItem != -1)
		bEnable = TRUE;

	GetDlgItem(IDC_SETUP_CELL_BUTTON)->EnableWindow(bEnable);
	GetDlgItem(IDC_REMOVE_CELL_BUTTON)->EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
CTextTableLine* CTextTableSettingsDlg::GetItemLine(int iItem)
{
	DWORD dwItemData = m_cellsListCtrl.GetItemData(iItem);

	return m_table.FindLine(GetLineID(dwItemData));
}
/////////////////////////////////////////////////////////////////////////////
CTextTableCell* CTextTableSettingsDlg::GetItemCell(int iItem)
{
	DWORD dwItemData = m_cellsListCtrl.GetItemData(iItem);

	return m_table.FindCell(GetLineID(dwItemData), GetCellID(dwItemData));
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CTextTableSettingsDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_SEL_CHANGED)
		UpdateButtons();

	if (message == UM_DBLCLICK_ITEM)
		OnBnClickedSetupCellButton();

	if (message == UM_DROP_ITEM)
	{	
		SaveUndo();

		if (m_bEditCells)
		{
			DWORD dwItemData = (DWORD)lParam;
				//get item data of our drop target (i.e. the item we dropped our selection on), it contains new line ID for all dropped items

			//create cells reordering list
			
			CList<CTextTableCell*, CTextTableCell*> order;

			int iItems = m_cellsListCtrl.GetItemCount();

			for (int iItem=0; iItem<iItems; iItem++)
			{
				CTextTableCell* lpCell = GetItemCell(iItem);
			
				if (m_cellsListCtrl.GetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED))
				{
					lpCell->SetLineID(GetLineID(dwItemData));

					lpCell->m_bSelected = TRUE;
				}

				order.AddTail(lpCell);
			}

			//reorder table

			m_table.Reorder(&order);

			InitCellsList();
		}
		else
		{
			//create lines reordering list

			CList<CTextTableLine*, CTextTableLine*> order;

			int iItems = m_cellsListCtrl.GetItemCount();

			for (int iItem=0; iItem<iItems; iItem++)
			{
				CTextTableLine* lpLine = GetItemLine(iItem);
			
				if (m_cellsListCtrl.GetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED))
					lpLine->m_bSelected = TRUE;

				order.AddTail(lpLine);
			}

			//reorder table

			m_table.Reorder(&order);

			InitCellsList();
		}
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnBnClickedSetupCellButton()
{
	SaveUndo();
	
	int iFocus = m_cellsListCtrl.GetFocusedItem();

	if (iFocus != -1)
	{
		CTextTableCellSettingsDlg dlg;

		dlg.SetOverlay(m_lpOverlay);
		dlg.SetLine(GetItemLine(iFocus));
		dlg.SetCell(GetItemCell(iFocus));

		if (IDOK == dlg.DoModal())
		{
			InitCellsList();

			m_cellsListCtrl.SetItemState(iFocus, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnBnClickedAddCellButton()
{
	SaveUndo();

	CTextTableCellAddDlg dlg;

	dlg.SetOverlay(m_lpOverlay);
	dlg.SetTable(&m_table);

	int iFocus = m_cellsListCtrl.GetFocusedItem();

	if (iFocus != -1)
		dlg.m_dwLineID = GetLineID(m_cellsListCtrl.GetItemData(iFocus));

	if (IDOK == dlg.DoModal())
	{
		InitCellsList();
		UpdateButtons();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnBnClickedRemoveCellButton()
{
	SaveUndo();

	int iItems	= m_cellsListCtrl.GetItemCount();

	if (m_bEditCells)
	{
		for (int iItem=iItems-1; iItem>=0; iItem--)
		{
			if (m_cellsListCtrl.GetItemState(iItem, LVIS_SELECTED))
			{
				CTextTableLine* lpLine = GetItemLine(iItem);
				CTextTableCell* lpCell = GetItemCell(iItem);

				lpLine->RemoveCell(lpCell);

				if (!lpLine->GetCount())
					m_table.RemoveLine(lpLine);

				m_cellsListCtrl.DeleteItem(iItem);
			}
		}

		m_table.UpdateLineIDs(TRUE);
	}
	else
	{
		for (int iItem=iItems-1; iItem>=0; iItem--)
		{
			if (m_cellsListCtrl.GetItemState(iItem, LVIS_SELECTED))
			{
				CTextTableLine* lpLine = GetItemLine(iItem);

				m_table.RemoveLine(lpLine);

				m_cellsListCtrl.DeleteItem(iItem);
			}
		}

		m_table.UpdateLineIDs(FALSE);
	}

	int iFocus = m_cellsListCtrl.GetFocusedItem();

	InitCellsList();

	if (iFocus != -1)
		m_cellsListCtrl.SetItemState(iFocus, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	UpdateButtons();
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnOK()
{
	UpdateData(TRUE);

	m_table.m_strName			= m_strName;
	m_table.m_nValueSize		= m_nValueSize;
	m_table.m_nValueAlignment	= m_nValueAlignment;
	m_table.m_nUnitsSize		= m_nUnitsSize;
	m_table.m_nUnitsAlignment	= m_nUnitsAlignment;

	m_strText.Format("<TT=%s>", m_strName);

	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnDeltaposValueSizeSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSize = m_nValueSize - pNMUpDown->iDelta;

	if (nSize < -500)
		nSize = -500;

	if (nSize > 500)
		nSize = 500;

	m_nValueSize = nSize;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnDeltaposValueAlignmentSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nAlignment = m_nValueAlignment - pNMUpDown->iDelta;

	if (nAlignment < -500)
		nAlignment = -500;

	if (nAlignment > 500)
		nAlignment = 500;

	m_nValueAlignment = nAlignment;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnDeltaposUnitsSizeSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSize = m_nUnitsSize - pNMUpDown->iDelta;

	if (nSize < -500)
		nSize = -500;

	if (nSize > 500)
		nSize = 500;

	m_nUnitsSize = nSize;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnDeltaposUnitsAlignmentSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nAlignment = m_nUnitsAlignment - pNMUpDown->iDelta;

	if (nAlignment < -500)
		nAlignment = -500;

	if (nAlignment > 500)
		nAlignment = 500;

	m_nUnitsAlignment = nAlignment;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnBnClickedLinesRadio()
{
	UpdateData(TRUE);

	m_cellsListCtrl.SetDragImageColumn(0);

	InitCellsList();
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::OnBnClickedCellsRadio()
{
	UpdateData(TRUE);

	m_cellsListCtrl.SetDragImageColumn(1);

	InitCellsList();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTextTableSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		BOOL bCtrl = GetAsyncKeyState(VK_CONTROL) < 0;

		if (pMsg->wParam == 'Z')
		{
			if (bCtrl)
				Undo();
		}
		else
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
			if (pMsg->hwnd == m_cellsListCtrl.m_hWnd)
				OnBnClickedRemoveCellButton();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::Undo()
{
	if (m_tableUndo.GetCount())
	{
		m_table.Copy(&m_tableUndo);

		InitCellsList();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::SaveUndo()
{
	m_tableUndo.Copy(&m_table);
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::Copy()
{
	int iFocus = m_cellsListCtrl.GetFocusedItem();

	if (iFocus != -1)
	{
		if (m_bEditCells)
			m_clipboardCell.Copy(GetItemCell(iFocus));
		else
			m_clipboardLine.Copy(GetItemLine(iFocus));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableSettingsDlg::Paste()
{
	if (m_bEditCells)
	{
		int iFocus = m_cellsListCtrl.GetFocusedItem();

		if (iFocus != -1)
		{
			CTextTableLine* lpLine = GetItemLine(iFocus);
			CTextTableCell* lpCell = new CTextTableCell;

			lpCell->Copy(&m_clipboardCell);

			lpCell->m_bSelected = TRUE;

			lpLine->AddCell(lpCell);

			InitCellsList();
		}
	}
	else
	{
		if (m_clipboardLine.GetCount())
		{
			CTextTableLine* lpLine = new CTextTableLine;

			lpLine->Copy(&m_clipboardLine);

			lpLine->m_bSelected = TRUE;

			m_table.AddLine(lpLine);

			InitCellsList();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
