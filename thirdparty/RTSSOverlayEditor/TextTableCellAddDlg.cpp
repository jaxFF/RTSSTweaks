// TextTableCellAddDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayDataSourceInternal.h"
#include "TextTableCellAddDlg.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
// CTextTableCellAddDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTextTableCellAddDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
CTextTableCellAddDlg::CTextTableCellAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextTableCellAddDlg::IDD, pParent)
{
	m_lpOverlay		= NULL;
	m_lpTable		= NULL;
	m_nTimerID		= 0;

	m_dwLineID		= 0;
	m_bAddToNewLine	= FALSE;
	m_strLineName	= "";
}
/////////////////////////////////////////////////////////////////////////////
CTextTableCellAddDlg::~CTextTableCellAddDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ADD_TO_NEW_LINE_CHECK, m_bAddToNewLine);
	DDX_Text(pDX, IDC_LINE_NAME_EDIT, m_strLineName);
	DDX_Control(pDX, IDC_SOURCES_LIST, m_sourcesListCtrl);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTextTableCellAddDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ADD_TO_NEW_LINE_CHECK, &CTextTableCellAddDlg::OnBnClickedAddToNewLineCheck)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CTextTableCellAddDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::SetTable(CTextTable* lpTable)
{
	m_lpTable = lpTable;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CTextTableCellAddDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	//init source list control

	m_sourcesListCtrl.ShowScrollBar(SB_VERT, TRUE);

	CRect cr; m_sourcesListCtrl.GetClientRect(&cr);

	DWORD dwWidth		= cr.Width() / 2;
	DWORD dwWidthChunk	= cr.Width() % 2;

	m_sourcesListCtrl.SetResourceHandle(g_hModule);
	m_sourcesListCtrl.InsertColumn(0, LocalizeStr("Source")	, LVCFMT_LEFT, dwWidth);
	m_sourcesListCtrl.InsertColumn(1, LocalizeStr("Value")	, LVCFMT_LEFT, dwWidth + dwWidthChunk);

	CHeaderCtrl* pHeader = m_sourcesListCtrl.GetHeaderCtrl();

	DWORD dwStyle = GetWindowLong(pHeader->m_hWnd ,GWL_STYLE );
	dwStyle &= ~HDS_FULLDRAG;
	SetWindowLong(pHeader->m_hWnd  , GWL_STYLE, dwStyle );

	m_sourcesListCtrl.EnableSecondColumn(TRUE);
	m_sourcesListCtrl.EnableItemSelection(TRUE);
	m_sourcesListCtrl.EnableMultiSelection(TRUE);

	CTextTableLine* lpLine = m_lpTable->FindLine(m_dwLineID);

	if (lpLine)
		m_strLineName = lpLine->m_strName;

	UpdateData(FALSE);

	InitSourcesList();
	UpdateLineControls();

	m_nTimerID = SetTimer(0x1234, 1000, NULL);

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::OnOK()
{
	UpdateData(TRUE);

	CTextTableLine* lpLine = m_lpTable->FindLine(m_dwLineID);

	if (m_bAddToNewLine || !lpLine)
	{
		lpLine = new CTextTableLine;

		lpLine->m_strName = m_strLineName;

		m_lpTable->AddLine(lpLine);
	}

	int nCount = 0;

	int iItems = m_sourcesListCtrl.GetItemCount();

	for (int iItem=0; iItem<iItems; iItem++)
	{
		if (m_sourcesListCtrl.GetItemData(iItem) & 0x80000000)
		{
			CTextTableCell* lpCell = new CTextTableCell;

			lpCell->m_strSource = m_sourcesListCtrl.GetItemText(iItem, 0);
			lpCell->m_bSelected	= TRUE;

			lpLine->AddCell(lpCell);

			nCount++;
		}
	}

	if (!nCount)
	{
		int iFocus = m_sourcesListCtrl.GetFocusedItem();

		if (iFocus != -1)
		{
			CTextTableCell* lpCell = new CTextTableCell;

			lpCell->m_strSource = m_sourcesListCtrl.GetItemText(iFocus, 0);
			lpCell->m_bSelected	= TRUE;

			lpLine->AddCell(lpCell);

			nCount++;
		}

	}

	if (!nCount)
	{
		CTextTableCell* lpCell = new CTextTableCell;

		lpCell->m_bSelected	= TRUE;

		lpLine->AddCell(lpCell);
	}

	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
int CTextTableCellAddDlg::FindItem(LPCSTR lpItemText)
{
	int iItems = m_sourcesListCtrl.GetItemCount();

	for (int iItem=0; iItem<iItems; iItem++)
	{
		CString strItemText = m_sourcesListCtrl.GetItemText(iItem, 0);

		if (!_stricmp(strItemText, lpItemText))
			return iItem;
	}

	return -1;
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::InitSourcesList()
{
	m_sourcesListCtrl.SetRedraw(FALSE);
	m_sourcesListCtrl.DeleteAllItems();

	DWORD dwSource = 0;

	COverlayDataSourcesList* lpInternalDataSources = m_lpOverlay->GetInternalDataSources();

	POSITION pos = lpInternalDataSources->GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = lpInternalDataSources->GetNext(pos);

		CString strSource = lpSource->GetName();

		if (FindItem(strSource) == -1)
			m_sourcesListCtrl.InsertItem(dwSource++, lpSource->GetName());
	}

	COverlayDataSourcesList* lpExternalDataSources = m_lpOverlay->GetExternalDataSources();

	pos = lpExternalDataSources->GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = lpInternalDataSources->GetNext(pos);

		CString strSource = lpSource->GetName();

		if (FindItem(strSource) == -1)
			m_sourcesListCtrl.InsertItem(dwSource++, lpSource->GetName());
	}

	m_sourcesListCtrl.Init();
	m_sourcesListCtrl.SetRedraw(TRUE);
	UpdateSourcesList();
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::UpdateSourcesList()
{
	m_sourcesListCtrl.SetRedraw(FALSE);

	for (int iItem=0; iItem<m_sourcesListCtrl.GetItemCount(); iItem++)
	{
		CString strSource = m_sourcesListCtrl.GetItemText(iItem, 0);

		COverlayDataSource*	lpSource = m_lpOverlay->FindDataSource(strSource);

		if (lpSource)
		{
			CString strData;

			FLOAT fltData = lpSource->GetData();

			if (fltData != FLT_MAX)
			{
				if (!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE)					|| 
					!strcmp(strSource, INTERNAL_SOURCE_FRAMETIME)					||
					!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_MIN)				||
					!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_AVG)				||
					!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_MAX)				||
					!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW)	||
					!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW))
					strData.Format("%.0f", fltData);
				else
					strData = lpSource->GetDataStr();

				strData += " ";
				strData += lpSource->GetUnits();
			}
			else
				strData = LocalizeStr("N/A");

			m_sourcesListCtrl.SetItemText(iItem, 1, strData);
		}
	}

	m_sourcesListCtrl.SetRedraw(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::OnDestroy()
{
	if (m_nTimerID)
		KillTimer(m_nTimerID);

	m_nTimerID = NULL;

	MSG msg; 
	while (PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE));

	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (!m_sourcesListCtrl.IsHeaderDragging())
		UpdateSourcesList();

	CDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::UpdateLineControls()
{
	GetDlgItem(IDC_LINE_NAME_EDIT)->EnableWindow(m_bAddToNewLine);
}
/////////////////////////////////////////////////////////////////////////////
void CTextTableCellAddDlg::OnBnClickedAddToNewLineCheck()
{
	UpdateData(TRUE);

	if (m_bAddToNewLine)
		m_strLineName = "";
	else
	{
		CTextTableLine* lpLine = m_lpTable->FindLine(m_dwLineID);

		if (lpLine)
			m_strLineName = lpLine->m_strName;
	}

	UpdateData(FALSE);

	UpdateLineControls();
}
/////////////////////////////////////////////////////////////////////////////
