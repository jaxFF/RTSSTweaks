// ImageWnd.h: interface for the CImageWnd class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
// CImageWnd
/////////////////////////////////////////////////////////////////////////////
const UINT UM_UPDATE_IMAGE_WND_STATUS	= ::RegisterWindowMessage("UM_UPDATE_IMAGE_WND_STATUS");
/////////////////////////////////////////////////////////////////////////////
#define SELECTION_SENSITIVITY 4
//////////////////////////////////////////////////////////////////////
class CImageWnd : public CWnd
{
	DECLARE_DYNAMIC(CImageWnd)

public:
	CImageWnd();
	virtual ~CImageWnd();

	BOOL	CreateImage(DWORD dwWidth, DWORD dwHeight);
	void	DestroyImage();
	LPBYTE	GetImage();

	void	SetImageRect(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight);
	CRect	GetImageRect();
	void	AdjustImageRect(int dx, int dy, int dcx = 0, int dcy = 0);

	void	DrawImage();
	void	DrawImage(CDC* pDC);

	CRect	FitRect(CRect &rect0, CRect &rect1);

	void	PrepareMemDC(CDC *pDC);
	void	DestroyMemDC();

	BOOL	m_bSelectMode;
	BOOL	m_bCancel;

	int		m_nSpritesNum;
	int		m_nSpritesPerLine;

protected:
	DECLARE_MESSAGE_MAP()

	LPBYTE	m_lpImage;
	LONG	m_dwWidth;
	LONG	m_dwPitch;
	LONG	m_dwHeight;

	CRect	m_rcImage;

	BOOL	m_bSelection;
	int		m_nSelectionX;
	int		m_nSelectionY;

	CDC		m_memDC;
	CBitmap	m_bitmap;

public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
};
/////////////////////////////////////////////////////////////////////////////


