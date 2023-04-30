// RecentDynamicColorsWnd.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "RecentDynamicColorsWnd.h"
/////////////////////////////////////////////////////////////////////////////
// CRecentDynamicColorsWnd
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CRecentDynamicColorsWnd, CWnd)
/////////////////////////////////////////////////////////////////////////////
CRecentDynamicColorsWnd::CRecentDynamicColorsWnd()
{
	ZeroMemory(&m_recentColors, sizeof(m_recentColors));

}
/////////////////////////////////////////////////////////////////////////////
CRecentDynamicColorsWnd::~CRecentDynamicColorsWnd()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRecentDynamicColorsWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CRecentDynamicColorsWnd message handlers
/////////////////////////////////////////////////////////////////////////////
void CRecentDynamicColorsWnd::SetRecentColors(LPDYNAMIC_COLOR_DESC lpRecentColors)
{
	CopyMemory(&m_recentColors, lpRecentColors, sizeof(m_recentColors));
}
/////////////////////////////////////////////////////////////////////////////
void CRecentDynamicColorsWnd::Draw(CDC* pDC)
{
	PrepareMemDC(pDC);

	CBitmap* pBitmap = m_memDC.SelectObject(&m_bitmap);

	CRect cr; GetClientRect(&cr);

	m_memDC.FillSolidRect(&cr, GetSysColor(COLOR_BTNFACE));

	for (DWORD dwColor=0; dwColor<32; dwColor++)
	{
		int x = dwColor * RECENT_DYNAMIC_COLOR_CELL;

		DWORD dwRanges = m_recentColors[dwColor].dwCount;

		for (DWORD dwRange=0; dwRange<dwRanges; dwRange++)
		{
			m_memDC.FillSolidRect(x + dwRange * 4 		, dwRange * 4		, 18, 18, GetSysColor(COLOR_APPWORKSPACE));
			m_memDC.FillSolidRect(x + dwRange * 4 + 1	, dwRange * 4 + 1	, 16, 16, COverlayLayer::SwapRGB(m_recentColors[dwColor].ranges[dwRange].dwColor & 0xFFFFFF));
		}
	}

	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), &m_memDC, 0, 0, SRCCOPY);

	m_memDC.SelectObject(pBitmap);
}
/////////////////////////////////////////////////////////////////////////////
void CRecentDynamicColorsWnd::OnPaint()
{
	CPaintDC dc(this); 

	Draw(&dc);
}
/////////////////////////////////////////////////////////////////////////////
void CRecentDynamicColorsWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	int x = point.x / RECENT_DYNAMIC_COLOR_CELL;
	int y = point.y / RECENT_DYNAMIC_COLOR_CELL;

	if ((x < 32) && !y)
		GetParent()->PostMessage(UM_SELECT_RECENT_DYNAMIC_COLOR, x, 0);

	CWnd::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CRecentDynamicColorsWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_hWnd)
	{
		DestroyMemDC();

		CDC* pDC = GetDC();	
		PrepareMemDC(pDC); 
		ReleaseDC(pDC);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRecentDynamicColorsWnd::OnDestroy()
{
	DestroyMemDC();

	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CRecentDynamicColorsWnd::PrepareMemDC(CDC *pDC)
{
	if (m_memDC.GetSafeHdc())
		return;

	CRect clientRect; GetClientRect(&clientRect);

	m_memDC.CreateCompatibleDC(pDC);
	m_bitmap.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
}
/////////////////////////////////////////////////////////////////////////////
void CRecentDynamicColorsWnd::DestroyMemDC()
{
	if (m_memDC.GetSafeHdc())
	{
		if (m_memDC.GetSafeHdc())
			VERIFY(m_memDC.DeleteDC());
	}

	if (m_bitmap.GetSafeHandle())
		VERIFY(m_bitmap.DeleteObject());
}
/////////////////////////////////////////////////////////////////////////////
