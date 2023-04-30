// ColorPreviewWnd.cpp: implementation of the CColorPreviewWnd class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "ColorPreviewWnd.h"
//////////////////////////////////////////////////////////////////////
// CColorPreviewWnd
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CColorPreviewWnd, CWnd)
//////////////////////////////////////////////////////////////////////
CColorPreviewWnd::CColorPreviewWnd()
{
	m_dwColor	= 0;

	m_dwWidth	= 0;
	m_dwPitch	= 0;
	m_dwHeight	= 0;
	m_dwSize	= 0;

	m_lpImage	= NULL;
}
//////////////////////////////////////////////////////////////////////
CColorPreviewWnd::~CColorPreviewWnd()
{
	DestroyImage();
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CColorPreviewWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// CColorPreviewWnd message handlers
//////////////////////////////////////////////////////////////////////
void CColorPreviewWnd::CreateImage(DWORD dwWidth, DWORD dwHeight)
{
	DestroyImage();

	m_dwWidth		= dwWidth;
	m_dwPitch		= dwWidth * 3;
	if (m_dwPitch & 3)
		m_dwPitch = 4 + m_dwPitch & 0xfffffffc; 
	m_dwHeight		= dwHeight;
	m_dwSize		= m_dwPitch * m_dwHeight;

	if (m_dwSize)
		m_lpImage	= new BYTE[m_dwSize];
}
//////////////////////////////////////////////////////////////////////
void CColorPreviewWnd::DestroyImage()
{
	m_dwWidth	= 0;
	m_dwPitch	= 0;
	m_dwHeight	= 0;
	m_dwSize	= 0;

	if (m_lpImage)
		delete [] m_lpImage;
	m_lpImage = NULL;
}
//////////////////////////////////////////////////////////////////////
int CColorPreviewWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreateImage(lpCreateStruct->cx, lpCreateStruct->cy);

	m_bi.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
    m_bi.bmiHeader.biWidth		    = m_dwWidth;
    m_bi.bmiHeader.biHeight			= m_dwHeight;
    m_bi.bmiHeader.biPlanes			= 1;
    m_bi.bmiHeader.biBitCount	    = 24;
    m_bi.bmiHeader.biCompression	= BI_RGB;
    m_bi.bmiHeader.biSizeImage		= 0;
    m_bi.bmiHeader.biXPelsPerMeter	= 0;
    m_bi.bmiHeader.biYPelsPerMeter	= 0;
    m_bi.bmiHeader.biClrUsed		= 0;
    m_bi.bmiHeader.biClrImportant	= 0;

	return 0;
}
//////////////////////////////////////////////////////////////////////
void CColorPreviewWnd::OnPaint()
{
	CPaintDC dc(this); 

	if (m_lpImage)
		StretchDIBits(dc.m_hDC, 0, 0, m_dwWidth, m_dwHeight, 0, 0, m_dwWidth, m_dwHeight, m_lpImage, &m_bi, DIB_RGB_COLORS, SRCCOPY);
}
//////////////////////////////////////////////////////////////////////
void CColorPreviewWnd::SetColor(DWORD dwColor)
{
	m_dwColor = dwColor;

	GetColorPreview(m_dwColor, m_dwWidth, m_dwHeight, m_dwSize, m_lpImage);

	RedrawWindow();
}
//////////////////////////////////////////////////////////////////////
