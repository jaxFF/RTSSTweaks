// SelectSpriteDlg.h: interface for the CSelectSpriteDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "ImageWnd.h"
/////////////////////////////////////////////////////////////////////////////
// CSelectSpriteDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CSelectSpriteDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectSpriteDlg)

public:
	CSelectSpriteDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectSpriteDlg();

	void UpdateStatus();
	BOOL IsCanceled();

	int m_nSpriteX;
	int m_nSpriteY;
	int m_nSpriteWidth;
	int m_nSpriteHeight;
	int m_nSpritesNum;
	int m_nSpritesPerLine;

// Dialog Data
	enum { IDD = IDD_SPRITE_SELECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CImageWnd	m_imageWnd;
	CStatusBar	m_statusBar;

	DWORD		m_dwStatusUpdateTimestamp;

	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
/////////////////////////////////////////////////////////////////////////////
