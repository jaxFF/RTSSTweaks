// OverlayLayersDlg.cpp : implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayEditorWnd.h"
#include "OverlayLayersDlg.h"
//////////////////////////////////////////////////////////////////////
// COverlayLayersDlg dialog
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayLayersDlg, CDialog)
//////////////////////////////////////////////////////////////////////
COverlayLayersDlg::COverlayLayersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayLayersDlg::IDD, pParent)
{
	m_lpOverlay		= NULL;
	m_lpEditorWnd	= NULL;
}
//////////////////////////////////////////////////////////////////////
COverlayLayersDlg::~COverlayLayersDlg()
{
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAYERS_LIST, m_layersListCtrl);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayLayersDlg, CDialog)
	ON_BN_CLICKED(IDC_MOVE_LAYER_TO_TOP_BUTTON, &COverlayLayersDlg::OnBnClickedMoveLayerToTopButton)
	ON_BN_CLICKED(IDC_MOVE_LAYER_UP_BUTTON, &COverlayLayersDlg::OnBnClickedMoveLayerUpButton)
	ON_BN_CLICKED(IDC_MOVE_LAYER_DOWN_BUTTON, &COverlayLayersDlg::OnBnClickedMoveLayerDownButton)
	ON_BN_CLICKED(IDC_MOVE_LAYER_TO_BOTTOM_BUTTON, &COverlayLayersDlg::OnBnClickedMoveLayerToBottomButton)
	ON_BN_CLICKED(IDC_REMOVE_LAYER_BUTTON, &COverlayLayersDlg::OnBnClickedRemoveLayerButton)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// COverlayLayersDlg message handlers
//////////////////////////////////////////////////////////////////////
BOOL COverlayLayersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	m_layersListCtrl.ShowScrollBar(SB_VERT, TRUE);

	CRect cr; m_layersListCtrl.GetClientRect(&cr);

	DWORD dwWidth		= cr.Width() / 2;
	DWORD dwWidthChunk	= cr.Width() % 2;

	m_layersListCtrl.SetResourceHandle(g_hModule);
	m_layersListCtrl.InsertColumn(0, LocalizeStr("Name"		)	, LVCFMT_LEFT, dwWidth);
	m_layersListCtrl.InsertColumn(1, LocalizeStr("Hypertext")	, LVCFMT_LEFT, dwWidth + dwWidthChunk);

	CHeaderCtrl* pHeader = m_layersListCtrl.GetHeaderCtrl();

	DWORD dwStyle = GetWindowLong(pHeader->m_hWnd ,GWL_STYLE );
	dwStyle &= ~HDS_FULLDRAG;
	SetWindowLong(pHeader->m_hWnd  , GWL_STYLE, dwStyle );

	m_layersListCtrl.EnableSecondColumn(TRUE);
	m_layersListCtrl.EnableItemSelection(FALSE);
	m_layersListCtrl.EnableMultiSelection(FALSE);
	m_layersListCtrl.DrawIcons(FALSE);

	SetButtonIcon(IDC_MOVE_LAYER_TO_TOP_BUTTON		, IDI_MOVE_TO_TOP	);
	SetButtonIcon(IDC_MOVE_LAYER_UP_BUTTON			, IDI_MOVE_UP		);
	SetButtonIcon(IDC_MOVE_LAYER_DOWN_BUTTON		, IDI_MOVE_DOWN		);
	SetButtonIcon(IDC_MOVE_LAYER_TO_BOTTOM_BUTTON	, IDI_MOVE_TO_BOTTOM);
	SetButtonIcon(IDC_REMOVE_LAYER_BUTTON			, IDI_REMOVE		);

	InitLayersList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::SetEditorWnd(COverlayEditorWnd* lpEditorWnd)
{
	m_lpEditorWnd = lpEditorWnd;
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::SetButtonIcon(UINT nControlId, UINT nIconId)
{
	CButton* pButton = (CButton*)GetDlgItem(nControlId);

	if (pButton)
	{
		HICON hIcon = (HICON)LoadImage(g_hModule, MAKEINTRESOURCE(nIconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

		if (hIcon)
		{
			pButton->SetIcon(hIcon);

			DestroyIcon(hIcon);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::Update()
{
	InitLayersList();
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::InitLayersList()
{
	if (m_lpOverlay)
	{
		int nScrollPos = m_layersListCtrl.SaveScrollPos();

		m_layersListCtrl.SetRedraw(FALSE);
		m_layersListCtrl.DeleteAllItems();

		DWORD dwLayer = 0;

		POSITION pos = m_lpOverlay->GetHeadPosition();

		COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

		while (pos)
		{
			COverlayLayer* lpLayer = m_lpOverlay->GetNext(pos);

			CString strName			= lpLayer->m_strName;

			if (strName.IsEmpty())
				strName.Format(LocalizeStr("nameless"), dwLayer);

			CString strHypertext	= COverlayLayer::EncodeSpecialChars(lpLayer->m_strText);

			int iItem = m_layersListCtrl.InsertItem(dwLayer, strName);

			m_layersListCtrl.SetItemText(iItem, 1, strHypertext);
			m_layersListCtrl.SetItemData(iItem, dwLayer);

			if (lpLayer == lpFocusedLayer)
			{
				m_layersListCtrl.SetSelectionMark(dwLayer);
				m_layersListCtrl.SetItemState(dwLayer, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			}

			dwLayer++;
		}

		m_layersListCtrl.Init();
		m_layersListCtrl.RestoreScrollPos(nScrollPos, TRUE);
		m_layersListCtrl.SetRedraw(TRUE);

		UpdateButtons();
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::UpdateButtons()
{
	COverlayLayer* lpFocusedLayer = GetItemLayer(m_layersListCtrl.GetSelectionMark());

	if (lpFocusedLayer)
	{
		BOOL bTopmost		= m_lpOverlay->IsLayerTopmost(lpFocusedLayer);
		BOOL bBottommost	= m_lpOverlay->IsLayerBottommost(lpFocusedLayer);

		GetDlgItem(IDC_MOVE_LAYER_TO_TOP_BUTTON		)->EnableWindow(!bBottommost);
		GetDlgItem(IDC_MOVE_LAYER_UP_BUTTON			)->EnableWindow(!bBottommost);
		GetDlgItem(IDC_MOVE_LAYER_DOWN_BUTTON		)->EnableWindow(!bTopmost);
		GetDlgItem(IDC_MOVE_LAYER_TO_BOTTOM_BUTTON	)->EnableWindow(!bTopmost);
		GetDlgItem(IDC_REMOVE_LAYER_BUTTON			)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_MOVE_LAYER_TO_TOP_BUTTON		)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVE_LAYER_UP_BUTTON			)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVE_LAYER_DOWN_BUTTON		)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOVE_LAYER_TO_BOTTOM_BUTTON	)->EnableWindow(FALSE);
		GetDlgItem(IDC_REMOVE_LAYER_BUTTON			)->EnableWindow(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////
LRESULT COverlayLayersDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_SEL_CHANGED)
	{
		COverlayLayer* lpFocusedLayer = GetItemLayer(m_layersListCtrl.GetSelectionMark());

		if (lpFocusedLayer)
		{
			if (m_lpEditorWnd)
			{
				m_lpEditorWnd->SaveUndo();
				m_lpEditorWnd->OnSetFocused(lpFocusedLayer);
				UpdateButtons();
			}
		}
	}

	if (message == UM_DBLCLICK_ITEM)
	{
		COverlayLayer* lpFocusedLayer = GetItemLayer(m_layersListCtrl.GetSelectionMark());

		if (lpFocusedLayer)
		{
			if (m_lpEditorWnd)
				m_lpEditorWnd->OnEditLayer(lpFocusedLayer);
		}
	}

	if (message == UM_DROP_ITEM)
	{	
		if (m_lpEditorWnd)
		{
			//create reordering list 

			CList<COverlayLayer*, COverlayLayer*> order;

			int iItems = m_layersListCtrl.GetItemCount();
		
			for (int iItem=0; iItem<iItems; iItem++)
			{
				COverlayLayer* lpLayer = GetItemLayer(iItem);

				order.AddTail(lpLayer);
			}

			//reorder data sources 

			m_lpEditorWnd->SaveUndo();
			m_lpEditorWnd->OnReorder(&order);
		}
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////
COverlayLayer* COverlayLayersDlg::GetItemLayer(int iItem)
{
	if (iItem != -1)
	{
		POSITION pos = m_lpOverlay->FindIndex(m_layersListCtrl.GetItemData(iItem));

		if (pos)
			return m_lpOverlay->GetAt(pos);
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::OnBnClickedMoveLayerToTopButton()
{
	if (m_lpEditorWnd)
	{
		m_lpEditorWnd->OnMoveToBottom();	//Z-order and display order are inverted
		m_lpEditorWnd->SaveUndo();
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::OnBnClickedMoveLayerUpButton()
{
	if (m_lpEditorWnd)
	{
		m_lpEditorWnd->OnMoveDown();	//Z-order and display order are inverted
		m_lpEditorWnd->SaveUndo();
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::OnBnClickedMoveLayerDownButton()
{
	if (m_lpEditorWnd)
	{
		m_lpEditorWnd->OnMoveUp();	//Z-order and display order are inverted
		m_lpEditorWnd->SaveUndo();
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::OnBnClickedMoveLayerToBottomButton()
{
	if (m_lpEditorWnd)
	{
		m_lpEditorWnd->OnMoveToTop();	//Z-order and display order are inverted
		m_lpEditorWnd->SaveUndo();
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayLayersDlg::OnBnClickedRemoveLayerButton()
{
	COverlayLayer* lpFocusedLayer = GetItemLayer(m_layersListCtrl.GetSelectionMark());

	if (lpFocusedLayer)
	{
		if (m_lpEditorWnd)
		{
			m_lpEditorWnd->OnRemoveLayer();
			m_lpEditorWnd->SaveUndo();
		}
	}
}
//////////////////////////////////////////////////////////////////////
