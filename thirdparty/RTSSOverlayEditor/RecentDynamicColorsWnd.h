// RecentDynamicColorsWnd.h: interface for the CRecentDynamicColorsWnd class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
// CRecentDynamicColorsWnd
/////////////////////////////////////////////////////////////////////////////
const UINT UM_SELECT_RECENT_DYNAMIC_COLOR	= ::RegisterWindowMessage("UM_SELECT_RECENT_DYNAMIC_COLOR");
/////////////////////////////////////////////////////////////////////////////
#define RECENT_DYNAMIC_COLOR_CELL 32
/////////////////////////////////////////////////////////////////////////////
class CRecentDynamicColorsWnd : public CWnd
{
	DECLARE_DYNAMIC(CRecentDynamicColorsWnd)

public:
	CRecentDynamicColorsWnd();
	virtual ~CRecentDynamicColorsWnd();

	void	SetRecentColors(LPDYNAMIC_COLOR_DESC lpRecentColors);

	void	Draw(CDC* pDC);

	void	PrepareMemDC(CDC *pDC);
	void	DestroyMemDC();

protected:
	DECLARE_MESSAGE_MAP()

	CDC		m_memDC;
	CBitmap	m_bitmap;

	DYNAMIC_COLOR_DESC m_recentColors[32];

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
};
/////////////////////////////////////////////////////////////////////////////


