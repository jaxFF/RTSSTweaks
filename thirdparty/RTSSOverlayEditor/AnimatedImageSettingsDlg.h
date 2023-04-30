// AnimatedImageSettingsDlg.h: interface for the CAnimatedImageSettingsDlg class.
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
// CAnimatedImageSettingsDlg dialog
/////////////////////////////////////////////////////////////////////////////
class CAnimatedImageSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnimatedImageSettingsDlg)

public:
	CAnimatedImageSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnimatedImageSettingsDlg();

	void	SetOverlay(COverlay* lpOverlay);

	void	UpdateImageControls();
	void	UpdateImageWnd();

	CString m_strSource;
	CString m_strText;

	int		m_nImageTemplate;
	BOOL	m_bResizeToExtent;
	int		m_nImageWidth;
	int		m_nImageHeight;
	float	m_fltImageMin;
	float	m_fltImageMax;
	int		m_nSpriteX;
	int		m_nSpriteY;
	int		m_nSpriteWidth;
	int		m_nSpriteHeight;
	int		m_nSpritesNum;
	int		m_nSpritesPerLine;
	int		m_nSpritePreview;
	float	m_fltImageBias;
	int		m_nImageRotationAngleMin;
	int		m_nImageRotationAngleMax;

	CString m_strImageMin;
	CString m_strImageMax;
	CString m_strImageBias;

	CComboBox m_ctrlDataSourcesCombo;

	CEdit	m_ctrlSpriteXEdit;
	CEdit	m_ctrlSpriteYEdit;
	CEdit	m_ctrlSpriteWidthEdit;
	CEdit	m_ctrlSpriteHeightEdit;
	CEdit	m_ctrlSpritesNumEdit;
	CEdit	m_ctrlSpritesPerLineEdit;

	CScrollBar m_ctrlAnimationScrollbar;

// Dialog Data
	enum { IDD = IDD_ANIMATED_IMAGE_PROPERTIES_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*	m_lpOverlay;
	HBRUSH		m_hBrush;

	CImageWnd	m_imageWnd;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnCbnSelchangeImageTemplateCombo();
	afx_msg void OnDeltaposImageWidthSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposImageHeightSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpriteXSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpriteYSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpriteWidthSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpriteHeightSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpritesNumSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpritesPerLineSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedResizeToExtent();
	afx_msg void OnBnClickedSelectSprite();
	afx_msg void OnEnChangeSpriteXEdit();
	afx_msg void OnEnChangeSpriteYEdit();
	afx_msg void OnEnChangeSpriteWidthEdit();
	afx_msg void OnEnChangeSpriteHeightEdit();
	afx_msg void OnEnChangeSpritesNumEdit();
	afx_msg void OnEnChangeSpritesPerLineEdit();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposRotationAngleMinSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRotationAngleMaxSpin(NMHDR *pNMHDR, LRESULT *pResult);
};
/////////////////////////////////////////////////////////////////////////////
