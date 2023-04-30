// ImageSettingsDlg.h: interface for the CImageSettingsDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "Overlay.h"
#include "ImageWnd.h"
#include "resource.h"
#include "afxwin.h"
/////////////////////////////////////////////////////////////////////////////
// CImageSettingsDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CImageSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CImageSettingsDlg)

public:
	CImageSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImageSettingsDlg();

	void	SetOverlay(COverlay* lpOverlay);

	void	UpdateImageControls();
	void	UpdateImageWnd();

	CString m_strText;

	int		m_nImageWidth;
	int		m_nImageHeight;
	int		m_nSpriteX;
	int		m_nSpriteY;
	int		m_nSpriteWidth;
	int		m_nSpriteHeight;
	int		m_nImageRotationAngle;

	BOOL	m_bResizeToExtent;

	CEdit	m_ctrlSpriteXEdit;
	CEdit	m_ctrlSpriteYEdit;
	CEdit	m_ctrlSpriteWidthEdit;
	CEdit	m_ctrlSpriteHeightEdit;

// Dialog Data
	enum { IDD = IDD_IMAGE_PROPERTIES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*	m_lpOverlay;
	CImageWnd	m_imageWnd;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDeltaposImageWidthSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposImageHeightSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpriteXSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpriteYSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpriteWidthSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpriteHeightSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedResizeToExtent();
	afx_msg void OnEnChangeSpriteXEdit();
	afx_msg void OnEnChangeSpriteYEdit();
	afx_msg void OnEnChangeSpriteWidthEdit();
	afx_msg void OnEnChangeSpriteHeightEdit();
	afx_msg void OnBnClickedSelectSprite();
	afx_msg void OnDeltaposRotationAngleSpin(NMHDR *pNMHDR, LRESULT *pResult);
};
/////////////////////////////////////////////////////////////////////////////
