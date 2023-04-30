// ImageWnd.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "ImageWnd.h"
/////////////////////////////////////////////////////////////////////////////
// CImageWnd
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CImageWnd, CWnd)
/////////////////////////////////////////////////////////////////////////////
CImageWnd::CImageWnd()
{
	m_lpImage			= NULL;
	m_dwWidth			= 0;
	m_dwPitch			= 0;
	m_dwHeight			= 0;

	m_bSelectMode		= FALSE;
	m_bCancel			= FALSE;

	m_nSpritesNum		= 0;	
	m_nSpritesPerLine	= 0;

	m_bSelection		= FALSE;
	m_nSelectionX		= 0;
	m_nSelectionY		= 0;
}
/////////////////////////////////////////////////////////////////////////////
CImageWnd::~CImageWnd()
{
	DestroyImage();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CImageWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CImageWnd message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CImageWnd::CreateImage(DWORD dwWidth, DWORD dwHeight)
{
	DestroyImage();

	DWORD dwPitch = dwWidth * 3;

	if (dwPitch & 3)
		dwPitch = 4 + m_dwPitch & 0xfffffffc; 

	DWORD dwSize = dwPitch * dwHeight;

	if (dwSize)
	{
		m_dwWidth		= dwWidth;
		m_dwPitch		= dwPitch;
		m_dwHeight		= dwHeight;

		m_lpImage		= new BYTE[dwSize];
		ZeroMemory(m_lpImage, dwSize);

		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::DestroyImage()
{
	if (m_lpImage)
		delete [] m_lpImage;
	m_lpImage	= NULL;

	m_dwWidth	= 0;
	m_dwPitch	= 0;
	m_dwHeight	= 0;
}
/////////////////////////////////////////////////////////////////////////////
LPBYTE CImageWnd::GetImage()
{
	return m_lpImage;
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::DrawImage(CDC* pDC)
{
	PrepareMemDC(pDC);

	CBitmap* pBitmap = m_memDC.SelectObject(&m_bitmap);

	CRect cr; GetClientRect(&cr);

	m_memDC.FillSolidRect(&cr, GetSysColor(COLOR_APPWORKSPACE));

	if (m_lpImage)
	{
		LPBITMAPINFO lpbi				= (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
		lpbi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		lpbi->bmiHeader.biWidth		    = m_dwWidth;
		lpbi->bmiHeader.biHeight		= m_dwHeight;
		lpbi->bmiHeader.biPlanes		= 1;
		lpbi->bmiHeader.biBitCount	    = 24;
		lpbi->bmiHeader.biCompression	= BI_RGB;
		lpbi->bmiHeader.biSizeImage		= 0;
		lpbi->bmiHeader.biXPelsPerMeter	= 0;
		lpbi->bmiHeader.biYPelsPerMeter	= 0;
		lpbi->bmiHeader.biClrUsed		= 0;
		lpbi->bmiHeader.biClrImportant	= 0;

		SetStretchBltMode(m_memDC.m_hDC, HALFTONE);

		if (IsWindowEnabled())
		{
			if (m_bSelectMode)
			{	
				StretchDIBits(m_memDC.m_hDC, 0, 0, m_dwWidth, m_dwHeight, 0, 0, m_dwWidth, m_dwHeight, m_lpImage, lpbi, DIB_RGB_COLORS, SRCCOPY);

				if (!m_rcImage.IsRectEmpty())
					m_memDC.DrawFocusRect(&m_rcImage);

				int nSpritesNum		= m_nSpritesNum;
				int nSpritesPerLine	= m_nSpritesPerLine;

				if (!nSpritesNum)
					nSpritesNum = 1;

				if (!nSpritesPerLine)
					nSpritesPerLine = nSpritesNum;

				if (nSpritesNum > 1)
				{

					CFont* pFont = m_memDC.SelectObject(GetParent()->GetFont());

					for (int nSprite=0; nSprite<nSpritesNum; nSprite++)
					{
						int nSpriteIndexX	= nSprite % nSpritesPerLine;
						int nSpriteIndexY	= nSprite / nSpritesPerLine;

						CRect rcSprite = m_rcImage;
						rcSprite.OffsetRect(nSpriteIndexX * m_rcImage.Width(), nSpriteIndexY * m_rcImage.Height());

						if (nSprite > 0)
							m_memDC.DrawFocusRect(&rcSprite);

						CString strSprite;
						strSprite.Format("%d", nSprite);

						CSize te = m_memDC.GetTextExtent(strSprite);

						if ((te.cx <= rcSprite.Width()) &&
							(te.cy <= rcSprite.Height()))
							m_memDC.TextOut(rcSprite.left + 1, rcSprite.top + 1, strSprite);
					}

					if (pFont)
						m_memDC.SelectObject(pFont);
				}

			}
			else
			{
				if (!m_rcImage.IsRectEmpty())
				{
					CRect srcRect0	= cr;
					CRect srcRect1	= m_rcImage;

					CRect dstRect	= FitRect(srcRect0, srcRect1);
					dstRect.OffsetRect((cr.Width() - dstRect.Width()) / 2, (cr.Height() - dstRect.Height()) / 2);

					StretchDIBits(m_memDC.m_hDC, dstRect.left, dstRect.top, dstRect.Width(), dstRect.Height(), m_rcImage.left, m_dwHeight - m_rcImage.top - m_rcImage.Height(), m_rcImage.Width(), m_rcImage.Height(), m_lpImage, lpbi, DIB_RGB_COLORS, SRCCOPY);
				}
			}
		}

		delete [] lpbi;
	}

	pDC->BitBlt(0, 0, cr.Width(), cr.Height(), &m_memDC, 0, 0, SRCCOPY);

	m_memDC.SelectObject(pBitmap);
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::DrawImage()
{
	CDC* pDC=GetDC();
	DrawImage(pDC);
	ReleaseDC(pDC);
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::OnPaint()
{
	CPaintDC dc(this); 

	DrawImage(&dc);
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::SetImageRect(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight)
{
	m_rcImage = CRect(dwX, dwY, dwX + dwWidth, dwY + dwHeight);
}
/////////////////////////////////////////////////////////////////////////////
CRect CImageWnd::GetImageRect()
{
	return m_rcImage;
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bSelectMode)
	{
		if (!m_bSelection)
		{	
			SetCapture();

			m_bSelection		= TRUE;
			m_nSelectionX	= point.x;
			m_nSelectionY	= point.y;

			DrawImage();
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bSelectMode)
	{
		if (m_bSelection)
		{
			ReleaseCapture();

			if ((abs(m_nSelectionX - point.x) <= SELECTION_SENSITIVITY) &&
				(abs(m_nSelectionY - point.y) <= SELECTION_SENSITIVITY))
			{
				int dx = (point.x - m_rcImage.left	) / m_rcImage.Width(); 
				int dy = (point.y - m_rcImage.top	) / m_rcImage.Height(); 

				if (point.x < m_rcImage.left)
					dx--;
				if (point.y < m_rcImage.top)
					dy--;

				dx = dx * m_rcImage.Width();
				dy = dy * m_rcImage.Height();

				AdjustImageRect(dx, dy);
			}

			m_bSelection	= FALSE;

			DrawImage();
		}
	}

	CWnd::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bSelectMode)
	{
		if (m_bSelection)
		{
			if ((abs(m_nSelectionX - point.x) > SELECTION_SENSITIVITY) &&
				(abs(m_nSelectionY - point.y) > SELECTION_SENSITIVITY))
				m_rcImage = CRect(min(m_nSelectionX, point.x), min(m_nSelectionY, point.y), max(m_nSelectionX, point.x), max(m_nSelectionY, point.y));

			if (m_rcImage.left < 0)
				m_rcImage.left = 0;
			if (m_rcImage.left > m_dwWidth - 1)
				m_rcImage.left = m_dwWidth - 1;

			if (m_rcImage.right < 0)
				m_rcImage.right = 0;
			if (m_rcImage.right > m_dwWidth - 1)
				m_rcImage.right = m_dwWidth - 1;

			if (m_rcImage.top < 0)
				m_rcImage.top = 0;
			if (m_rcImage.top > m_dwHeight - 1)
				m_rcImage.top = m_dwHeight - 1;

			if (m_rcImage.bottom < 0)
				m_rcImage.bottom = 0;
			if (m_rcImage.bottom > m_dwHeight - 1)
				m_rcImage.bottom = m_dwHeight - 1;

			DrawImage();
		}

		GetParent()->PostMessage(UM_UPDATE_IMAGE_WND_STATUS, 0, 0);
	}

	CWnd::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
CRect CImageWnd::FitRect(CRect &rect0, CRect &rect1)
{
	CRect rect(0,0,0,0);
		//result variable

	if (rect0.IsRectEmpty() || rect1.IsRectEmpty())
		//return empty rectangle if one of specified rectangles is empty
		return rect;
	
	rect0.OffsetRect(-rect0.left, -rect0.top);
	rect1.OffsetRect(-rect1.left, -rect1.top);
		//move rectangles to (0,0) point

	if ((rect0.Width()  >= rect1.Width()) && (rect0.Height() >= rect1.Height()))
		//return rect1 if it lies inside rect0
		return rect1;
	
	FLOAT HWRatio0 = (FLOAT)rect0.bottom/rect0.right;
	FLOAT HWRatio1 = (FLOAT)rect1.bottom/rect1.right;

	if (HWRatio1 > HWRatio0)
	{
		rect.right  = (long)(rect0.bottom / HWRatio1);
		rect.bottom = rect0.bottom;
	}
	else
	{
		rect.right  = rect0.right;
		rect.bottom = (long)(rect0.right * HWRatio1);
	}

	return rect;
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::OnSize(UINT nType, int cx, int cy)
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
void CImageWnd::OnDestroy()
{
	DestroyMemDC();

	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::PrepareMemDC(CDC *pDC)
{
	if (m_memDC.GetSafeHdc())
		return;

	CRect clientRect; GetClientRect(&clientRect);

	m_memDC.CreateCompatibleDC(pDC);
	m_bitmap.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::DestroyMemDC()
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
void CImageWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_bSelectMode)
	{
		BOOL bCtrl	= GetAsyncKeyState(VK_CONTROL	) < 0;
		BOOL bShift = GetAsyncKeyState(VK_SHIFT		) < 0;

		if (nChar == VK_RETURN)
		{
			GetParent()->PostMessage(WM_COMMAND, IDOK, 0);
		}
		else
		if (nChar == VK_ESCAPE)
		{
			m_bCancel = TRUE;

			GetParent()->PostMessage(WM_COMMAND, IDCANCEL, 0);
		}
		else
		if (nChar == VK_UP)
		{
			if (bCtrl) 
				AdjustImageRect(0, -m_rcImage.Height());
			else 
			if (bShift)
				AdjustImageRect(0, 0, 0, -1);
			else
				AdjustImageRect(0, -1);

			DrawImage();

			GetParent()->PostMessage(UM_UPDATE_IMAGE_WND_STATUS, 0, 0);
		}
		else
		if (nChar == VK_DOWN)
		{
			if (bCtrl) 
				AdjustImageRect(0, m_rcImage.Height());
			else 
			if (bShift)
				AdjustImageRect(0, 0, 0, 1);
			else
				AdjustImageRect(0, 1);

			DrawImage();

			GetParent()->PostMessage(UM_UPDATE_IMAGE_WND_STATUS, 0, 0);
		}
		else
		if (nChar == VK_LEFT)
		{
			if (bCtrl) 
				AdjustImageRect(-m_rcImage.Width(), 0);
			else 
			if (bShift)
				AdjustImageRect(0, 0, -1, 0);
			else
				AdjustImageRect(-1, 0);

			DrawImage();

			GetParent()->PostMessage(UM_UPDATE_IMAGE_WND_STATUS, 0, 0);
		}
		else
		if (nChar == VK_RIGHT)
		{
			if (bCtrl) 
				AdjustImageRect(m_rcImage.Width(), 0);
			else 
			if (bShift)
				AdjustImageRect(0, 0, 1, 0);
			else
				AdjustImageRect(1, 0);

			DrawImage();

			GetParent()->PostMessage(UM_UPDATE_IMAGE_WND_STATUS, 0, 0);
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CImageWnd::AdjustImageRect(int dx, int dy, int dcx, int dcy)
{
	if (m_rcImage.left + dx < 0)
		dx = 0;
	if (m_rcImage.top + dy < 0)
		dy = 0;

	m_rcImage.OffsetRect(dx, dy);

	if (m_rcImage.Width() + dcx < 0)
		dcx = 0;
	if (m_rcImage.Height() + dcy < 0)
		dcy = 0;

	m_rcImage.right		+= dcx;
	m_rcImage.bottom	+= dcy;
}
/////////////////////////////////////////////////////////////////////////////
UINT CImageWnd::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}
/////////////////////////////////////////////////////////////////////////////
