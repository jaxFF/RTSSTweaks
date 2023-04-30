// AddEmbeddedObjectDlg.h: interface for the CAddEmbeddedObjectDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "afxwin.h"
//////////////////////////////////////////////////////////////////////
// CAddEmbeddedObjectDlg dialog
//////////////////////////////////////////////////////////////////////
class CAddEmbeddedObjectDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddEmbeddedObjectDlg)

public:
	CAddEmbeddedObjectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddEmbeddedObjectDlg();

	void	SetOverlay(COverlay* lpOverlay);

	void	InitDataSourcesList();
	void	UpdateGraphControls();
	void	UpdateImageControls();
	void	UpdateTableControls();

	BOOL	m_bTable;

	CString m_strSource;
	CString m_strTable;
	CString m_strText;

	BOOL	m_bAddLabel;
	BOOL	m_bAddCurrentValue;
	BOOL	m_bAddGraph;
	BOOL	m_bAddImage;

	int		m_nGraphTemplate;
	int		m_nGraphStyle;
	int		m_nGraphWidth;
	int		m_nGraphHeight;
	int		m_nGraphMargin;
	float	m_fltGraphMin;
	float	m_fltGraphMax;
	DWORD	m_dwGraphFlags;

	int		m_nImageTemplate;
	int		m_nImageWidth;
	int		m_nImageHeight;
	float	m_fltImageMin;
	float	m_fltImageMax;
	int		m_nImageSpriteX;
	int		m_nImageSpriteY;
	int		m_nImageSpriteWidth;
	int		m_nImageSpriteHeight;
	int		m_nImageRotationAngle;
	int		m_nImageSpritesNum;
	int		m_nImageSpritesPerLine;
	float	m_fltImageBias;
	int		m_nImageRotationAngleMin;
	int		m_nImageRotationAngleMax;

	CComboBox m_ctrlDataSourcesCombo;

// Dialog Data
	enum { IDD = IDD_ADD_EMBEDDED_OBJECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*	m_lpOverlay;
	HBRUSH		m_hBrush;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeGraphTemplateCombo();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedAddGraphCheck();
	afx_msg void OnDeltaposGraphWidthSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposGraphHeightSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposGraphMarginSpin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAddImageCheck();
	afx_msg void OnCbnSelchangeImageTemplateCombo();
	afx_msg void OnBnClickedEditEmbeddedGraph();
	afx_msg void OnBnClickedEditEmbeddedImage();
	afx_msg void OnBnClickedDataSourceRadio();
	afx_msg void OnBnClickedTableRadio();
};
//////////////////////////////////////////////////////////////////////
