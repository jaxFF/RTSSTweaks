// OverlayEditorDlg.cpp : implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayEditorDlg.h"
//////////////////////////////////////////////////////////////////////
#define RENDER_TIMER_ID				'RTID'
//////////////////////////////////////////////////////////////////////
static UINT BASED_CODE g_statusBarIndicators[] =
{
    IDS_UNDO_REDO,
    IDS_LAYER_INFO,
    IDS_HYPERTEXT_INFO
};
//////////////////////////////////////////////////////////////////////
// COverlayEditorDlg dialog
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayEditorDlg, CDialog)
//////////////////////////////////////////////////////////////////////
COverlayEditorDlg::COverlayEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayEditorDlg::IDD, pParent)
{
	m_bShowHypertext		= FALSE;
	m_hIcon					= NULL;
}
//////////////////////////////////////////////////////////////////////
COverlayEditorDlg::~COverlayEditorDlg()
{
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayEditorDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// COverlayEditorDlg message handlers
//////////////////////////////////////////////////////////////////////
BOOL COverlayEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_hIcon = LoadIcon(g_hModule, MAKEINTRESOURCE(IDI_RTSS));

	SetIcon(m_hIcon, TRUE	);
	SetIcon(m_hIcon, FALSE	);

	CRect cr;
	GetClientRect(&cr);

	m_mainMenu.LoadMenu(IDR_MAIN_MENU);
	LocalizeMenu(m_mainMenu.m_hMenu);
	SetMenu(&m_mainMenu);

	m_statusBar.Create(this);
	m_statusBar.SetIndicators(g_statusBarIndicators, 3);
	m_statusBar.SetPaneInfo(0, IDS_UNDO_REDO		, 0					, 200	);      
	m_statusBar.SetPaneInfo(1, IDS_LAYER_INFO		, 0/*SBPS_STRETCH*/	, 325	);
	m_statusBar.SetPaneInfo(2, IDS_HYPERTEXT_INFO	, 0/*SBPS_STRETCH*/	, 500	);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, IDS_UNDO_REDO);

	CRect statusRect;
	m_statusBar.GetWindowRect(&statusRect);

	cr.bottom -= statusRect.Height() + GetSystemMetrics(SM_CYMENU);

	m_editorWnd.CreateEx(0, AfxRegisterWndClass(CS_OWNDC|CS_DBLCLKS,::LoadCursor(NULL,IDC_ARROW)), "", WS_CHILD | WS_VISIBLE, cr, this, 0);

	if (!m_editorWnd.InitRenderer())
	{
		MessageBox(LocalizeStr("Failed to initialize Direct3D!"), LocalizeStr("Overlay editor"), MB_ICONERROR);

		EndDialog(IDCANCEL);

		return FALSE;
	}
		
	m_editorWnd.SetOverlay(&m_overlay);

	if (!m_ctrlDebugView.Create(WS_VISIBLE|WS_DISABLED|ES_SUNKEN|ES_READONLY|ES_MULTILINE/*|ES_AUTOHSCROLL*/, CRect(0,0,1,1), this, 0))
		return FALSE;
	m_ctrlDebugView.SetFont(GetFont());

	m_editorWnd.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	m_mmTimer.Create(16, 0, 0, m_hWnd, WM_TIMER, RENDER_TIMER_ID, 0);

	QueryPerformanceFrequency(&m_pf);
	QueryPerformanceCounter(&m_pc);

	UpdateMenu();

	m_overlay.Update(TRUE, m_editorWnd.IsOptimized());

	OnSaveLastState();
	OnUpdateStatus();

	return TRUE;  
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnDestroy()
{
	m_mmTimer.Kill();
	m_editorWnd.UninitRenderer();
	m_editorWnd.DestroyWindow();

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	RepositionChildren(cx, cy);
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == RENDER_TIMER_ID)
	{
		if (m_editorWnd.IsRenderingPaused())
			m_mmTimer.EatMessages();
		else
		{
			m_editorWnd.Render();

			m_mmTimer.EatMessages();

			if (m_overlay.OnTimer(m_editorWnd.IsOptimized(), 0))
			{
				if (m_bShowHypertext)
					m_ctrlDebugView.SetWindowTextEx(COverlayLayer::EncodeSpecialChars(m_overlay.GetHypertextForDebugger()));
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnShowHypertext()
{
	m_bShowHypertext = !m_bShowHypertext;

	if (m_bShowHypertext)
		m_ctrlDebugView.SetWindowTextEx(COverlayLayer::EncodeSpecialChars(m_overlay.GetHypertextForDebugger()));

	CRect cr;
	GetClientRect(&cr);

	RepositionChildren(cr.Width(), cr.Height());

	UpdateMenu();
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::RepositionChildren(int cx, int cy)
{
	if (cx && cy)
	{
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDS_UNDO_REDO);

		CRect statusRect;
		m_statusBar.GetWindowRect(&statusRect);
		ScreenToClient(&statusRect);

		if (m_bShowHypertext)
		{
			if (IsWindow(m_ctrlDebugView.GetSafeHwnd()))
				m_ctrlDebugView.MoveWindow(0, cy - statusRect.Height() - 200, cx, 200);

			if (IsWindow(m_editorWnd.GetSafeHwnd()))
				m_editorWnd.MoveWindow(0, 0, cx, cy - statusRect.Height() - 200);
		}
		else
		{
			m_ctrlDebugView.MoveWindow(0, 0, 1, 1);

			if (IsWindow(m_editorWnd.GetSafeHwnd()))
				m_editorWnd.MoveWindow(0, 0, cx, cy - statusRect.Height());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnUpdateStatus()
{
	int nUndo = m_editorWnd.GetUndoStackSize();
	int nRedo = m_editorWnd.GetRedoStackSize();

	CString strStatus;
	CString strCaption;

	CString strFilename = m_overlay.GetFilename();

	if (strFilename.IsEmpty())
		strFilename = LocalizeStr("nameless");

	strCaption.Format(LocalizeStr("Overlay editor - %s"), strFilename);

	SetWindowText(strCaption);

	if ((nUndo  > 1) || nRedo)
	{
		if (nRedo)
			strStatus.Format(LocalizeStr("Undo / Redo : %d / %d"), nUndo - 1, nRedo);
		else
			strStatus.Format(LocalizeStr("Undo : %d"), nUndo - 1);

		strCaption += " *";

	}
	else
		strStatus = LocalizeStr("Undo / Redo");

	m_statusBar.SetPaneText(0, strStatus);

	COverlayLayer* lpFocusedLayer = m_overlay.GetFocusedLayer();

	if (lpFocusedLayer)
	{
		CString strX, strY;
		
		if (lpFocusedLayer->m_nPositionSticky < 0)
		{
			strX.Format("%d", lpFocusedLayer->m_nPositionX);
			strY.Format("%d", lpFocusedLayer->m_nPositionY);
		}
		else
		{
			switch (lpFocusedLayer->m_nPositionSticky % 3)
			{
			case 0:
				strX = LocalizeStr("left");
				break;
			case 1:
				strX = LocalizeStr("center");
				break;
			case 2:
				strX = LocalizeStr("right");
				break;
			}

			switch (lpFocusedLayer->m_nPositionSticky / 3)
			{
			case 0:
				strY = LocalizeStr("top");
				break;
			case 1:
				strY = LocalizeStr("center");
				break;
			case 2:
				strY = LocalizeStr("bottom");
				break;
			}
		}

		strStatus.Format(LocalizeStr("Layer %d of %d\tPos (%s, %s)\tExt (%d, %d)"), lpFocusedLayer->GetLayerID(), m_overlay.GetCount(), strX, strY, lpFocusedLayer->m_nExtentX, lpFocusedLayer->m_nExtentY);
	}
	else
		strStatus = LocalizeStr("Layer info");

	m_statusBar.SetPaneText(1, strStatus);

	strStatus.Format(LocalizeStr("\tHypertext %d / %d\tBuffer %d / %d"), m_overlay.GetHypertextSize(), 32768, m_overlay.GetHypertextBufferSize(), 262144);

	m_statusBar.SetPaneText(2, strStatus);

}
//////////////////////////////////////////////////////////////////////
BOOL COverlayEditorDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (nCode == CN_COMMAND)
	{
		switch (nID)
		{
		case ID_LAYOUTS_NEW:
			m_editorWnd.OnNew();
			return TRUE;
		case ID_LAYOUTS_EDIT:
			m_editorWnd.OnEditOverlay();
			return TRUE;
		case ID_LAYOUTS_LOAD:
			m_editorWnd.OnLoad();
			return TRUE;
		case ID_LAYOUTS_SAVE:
			m_editorWnd.OnSave();
			return TRUE;
		case ID_LAYOUTS_SAVEAS:
			m_editorWnd.OnSaveAs();
			return TRUE;
		case ID_LAYOUTS_IMPORT:
			m_editorWnd.OnImport();
			return TRUE;
		case ID_LAYOUTS_EXPORT:
			m_editorWnd.OnExport();
			return TRUE;

		case ID_LAYOUTS_UNDO:
			m_editorWnd.OnUndo();
			return TRUE;
		case ID_LAYOUTS_REDO:
			m_editorWnd.OnRedo();
			return TRUE;

		case ID_DATASOURCES_EDIT:
			m_editorWnd.OnSetupSources();
			return TRUE;

		case ID_TABLES_EDIT:
			m_editorWnd.OnSetupTables();
			return TRUE;

		case ID_LAYERS_ADD:
			m_editorWnd.OnAddLayer();
			m_editorWnd.SaveUndo();
			return TRUE;
		case ID_LAYERS_REMOVE:
			m_editorWnd.OnRemoveLayer();
			m_editorWnd.SaveUndo();
			return TRUE;
		case ID_LAYERS_EDIT:
			m_editorWnd.OnEditLayer();
			return TRUE;
		case ID_LAYERS_EDITLIST:
			m_editorWnd.OnEditLayersList();
			return TRUE;
		case ID_LAYERS_MOVETOTOP:
			m_editorWnd.OnMoveToTop();
			m_editorWnd.SaveUndo();
			return TRUE;
		case ID_LAYERS_MOVEUP:
			m_editorWnd.OnMoveUp();
			m_editorWnd.SaveUndo();
			return TRUE;
		case ID_LAYERS_MOVEDOWN:
			m_editorWnd.OnMoveDown();
			m_editorWnd.SaveUndo();
			return TRUE;
		case ID_LAYERS_MOVETOBOTTOM:
			m_editorWnd.OnMoveToBottom();
			m_editorWnd.SaveUndo();
			return TRUE;
		case ID_LAYERS_COPY:
			m_editorWnd.OnCopy();
			return TRUE;
		case ID_LAYERS_PASTE:
			m_editorWnd.OnPaste();
			m_editorWnd.SaveUndo();
			return TRUE;

		case ID_VIEW_RESETVIEWZOOM:
			m_editorWnd.ResetViewScale();
			break;
		case ID_VIEW_SNAPTOGRID:
			{
				BOOL bEnable = !m_editorWnd.IsSnapToGridEnabled();

				m_editorWnd.EnableSnapToGrid(bEnable);

				UpdateMenu();
			}
			break;
		case ID_VIEW_SHOWGRID:
			{
				BOOL bEnable = !m_editorWnd.IsRenderGridEnabled();

				m_editorWnd.EnableRenderGrid(bEnable);
				if (bEnable)
					m_editorWnd.EnableRenderCheckerboard(FALSE);

				UpdateMenu();
			}
			break;
		case ID_VIEW_SHOWCHECKERBOARD:
			{
				BOOL bEnable = !m_editorWnd.IsRenderCheckerboardEnabled();

				m_editorWnd.EnableRenderCheckerboard(bEnable);
				if (bEnable)
					m_editorWnd.EnableRenderGrid(FALSE);

				UpdateMenu();
			}
			break;

		case ID_TOOLS_SHOWHYPERTEXT:
			OnShowHypertext();
			return TRUE;
		case ID_TOOLS_OPTIMIZEHYPERTEXT:
			OnOptimizeHypertext();
			return TRUE;
		}
	}

	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnOK()
{
	if (m_editorWnd.SavePrompt())
		CDialog::OnOK();
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnCancel()
{
	if (m_editorWnd.SavePrompt())
		CDialog::OnCancel();
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnOptimizeHypertext()
{
	m_editorWnd.OnOptimize();

	if (m_bShowHypertext)
		m_ctrlDebugView.SetWindowTextEx(COverlayLayer::EncodeSpecialChars(m_overlay.GetHypertextForDebugger()));

	UpdateMenu();
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::UpdateMenu()
{
	m_mainMenu.CheckMenuItem(ID_VIEW_SNAPTOGRID			, MF_BYCOMMAND | (m_editorWnd.IsSnapToGridEnabled()			? MF_CHECKED : MF_UNCHECKED));
	m_mainMenu.CheckMenuItem(ID_VIEW_SHOWGRID			, MF_BYCOMMAND | (m_editorWnd.IsRenderGridEnabled()			? MF_CHECKED : MF_UNCHECKED));
	m_mainMenu.CheckMenuItem(ID_VIEW_SHOWCHECKERBOARD	, MF_BYCOMMAND | (m_editorWnd.IsRenderCheckerboardEnabled()	? MF_CHECKED : MF_UNCHECKED));

	m_mainMenu.CheckMenuItem(ID_TOOLS_SHOWHYPERTEXT		, MF_BYCOMMAND | (m_bShowHypertext			? MF_CHECKED : MF_UNCHECKED));
	m_mainMenu.CheckMenuItem(ID_TOOLS_OPTIMIZEHYPERTEXT	, MF_BYCOMMAND | (m_editorWnd.IsOptimized()	? MF_CHECKED : MF_UNCHECKED));
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnOverlayMessage(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams)
{
	m_overlay.AddMessageToQueue(lpMessage, lpLayer, lpParams);

	PostMessage(UM_OVERLAY_MESSAGE);
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorDlg::OnSaveLastState()
{
	m_overlayLast.Copy(&m_overlay);
}
//////////////////////////////////////////////////////////////////////
LRESULT COverlayEditorDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_OVERLAY_MESSAGE)
	{
		if (m_overlay.FlushMessageQueue(this))
			m_overlay.Update(FALSE, m_editorWnd.IsOptimized());
	}

	if (message == UM_SHOW_HYPERTEXT)
		OnShowHypertext();

	if (message == UM_UPDATE_STATUS)
		OnUpdateStatus();

	if (message == UM_SAVE_LAST_STATE)
		OnSaveLastState();

	if (message == UM_OVERLAY_LOADED)
	{
		m_editorWnd.SaveUndo(TRUE);

		m_overlay.Update(TRUE, m_editorWnd.IsOptimized());
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////
