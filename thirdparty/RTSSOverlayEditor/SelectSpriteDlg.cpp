// SelectSpriteDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "SelectSpriteDlg.h"
#include "RTSSHooksInterface.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectSpriteDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSelectSpriteDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
#define MIN_STATUS_UPDATE_PERIOD 33
/////////////////////////////////////////////////////////////////////////////
static UINT BASED_CODE g_statusBarIndicators[] =
{
    IDS_SPRITE_DIM
};
/////////////////////////////////////////////////////////////////////////////
CSelectSpriteDlg::CSelectSpriteDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectSpriteDlg::IDD, pParent)
{
	m_nSpriteX					= 0;
	m_nSpriteY					= 0;
	m_nSpriteWidth				= 0;
	m_nSpriteHeight				= 0;
	m_nSpritesNum				= 0;
	m_nSpritesPerLine			= 0;

	m_dwStatusUpdateTimestamp	= 0;
}
/////////////////////////////////////////////////////////////////////////////
CSelectSpriteDlg::~CSelectSpriteDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSelectSpriteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSelectSpriteDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSelectSpriteDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CSelectSpriteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	CRTSSHooksInterface rtssInterface;

	DWORD dwWidth = rtssInterface.GetFontTextureWidth();

	if (dwWidth)
	{
		m_imageWnd.CreateImage(dwWidth, dwWidth);

		rtssInterface.CopyFontTexture(dwWidth, dwWidth, m_imageWnd.GetImage());
	}

	m_imageWnd.m_bSelectMode		= TRUE;
	m_imageWnd.m_nSpritesNum		= m_nSpritesNum;
	m_imageWnd.m_nSpritesPerLine	= m_nSpritesPerLine;
	m_imageWnd.SetImageRect(m_nSpriteX, m_nSpriteY, m_nSpriteWidth, m_nSpriteHeight);

	CRect cr;
	GetClientRect(&cr);

	m_statusBar.Create(this);
	m_statusBar.SetIndicators(g_statusBarIndicators, 1);
	m_statusBar.SetPaneInfo(0, IDS_SPRITE_DIM, 0, 400);      

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, IDS_UNDO_REDO);

	CRect statusRect;
	m_statusBar.GetWindowRect(&statusRect);

	cr.bottom -= statusRect.Height();

	if (!m_imageWnd.CreateEx(WS_EX_CLIENTEDGE, AfxRegisterWndClass(CS_OWNDC|CS_DBLCLKS, ::LoadCursor(NULL,IDC_ARROW)), NULL, WS_CHILD|WS_VISIBLE, cr, this, 0))
		return FALSE;

	m_imageWnd.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

	UpdateStatus();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSelectSpriteDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDS_UNDO_REDO);

	CRect statusRect;
	m_statusBar.GetWindowRect(&statusRect);
	ScreenToClient(&statusRect);

	if (IsWindow(m_imageWnd.GetSafeHwnd()))
		m_imageWnd.MoveWindow(0, 0, cx, cy - statusRect.Height());
}
/////////////////////////////////////////////////////////////////////////////
void CSelectSpriteDlg::OnOK()
{
	CRect rect = m_imageWnd.GetImageRect();

	m_nSpriteX		= rect.left;
	m_nSpriteY		= rect.top;
	m_nSpriteWidth	= rect.Width();
	m_nSpriteHeight	= rect.Height();

	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CSelectSpriteDlg::OnCancel()
{
	CRect rect = m_imageWnd.GetImageRect();

	m_nSpriteX		= rect.left;
	m_nSpriteY		= rect.top;
	m_nSpriteWidth	= rect.Width();
	m_nSpriteHeight	= rect.Height();

	CDialog::OnCancel();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSelectSpriteDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_UPDATE_IMAGE_WND_STATUS)
		UpdateStatus();

	return CDialog::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CSelectSpriteDlg::UpdateStatus()
{
	DWORD dwTimestamp = GetTickCount();

	if (dwTimestamp - m_dwStatusUpdateTimestamp > MIN_STATUS_UPDATE_PERIOD)
	{
		m_dwStatusUpdateTimestamp = dwTimestamp;

		CPoint ptCursor;
		GetCursorPos(&ptCursor);
		m_imageWnd.ScreenToClient(&ptCursor);

		CRect rcImage = m_imageWnd.GetImageRect();

		CString strStatus;
		strStatus.Format(LocalizeStr("Cur (%d, %d)\tPos (%d, %d)\tExt (%d, %d)"), ptCursor.x, ptCursor.y, rcImage.left, rcImage.top, rcImage.Width(), rcImage.Height());

		m_statusBar.SetPaneText(0, strStatus);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSelectSpriteDlg::IsCanceled()
{
	return m_imageWnd.m_bCancel;
}
/////////////////////////////////////////////////////////////////////////////
