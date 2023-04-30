// ColorPreviewWnd.h: interface for the CColorPreviewWnd class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
// CColorPreviewWnd
//////////////////////////////////////////////////////////////////////
class CColorPreviewWnd : public CWnd
{
	DECLARE_DYNAMIC(CColorPreviewWnd)

	void SetColor(DWORD dwColor);

	void CreateImage(DWORD dwWidth, DWORD dwHeight);
	void DestroyImage();
	
public:
	CColorPreviewWnd();
	virtual ~CColorPreviewWnd();

protected:
	DECLARE_MESSAGE_MAP()

	DWORD		m_dwColor;

	BITMAPINFO	m_bi;

	DWORD		m_dwWidth;
	DWORD		m_dwPitch;
	DWORD		m_dwHeight;
	DWORD		m_dwSize;
	LPBYTE		m_lpImage;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
};
//////////////////////////////////////////////////////////////////////


