// AnimatedImageSettingsDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "AnimatedImageSettingsDlg.h"
#include "SelectSpriteDlg.h"
#include "RTSSHooksInterface.h"
/////////////////////////////////////////////////////////////////////////////
// CAnimatedImageSettingsDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CAnimatedImageSettingsDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
CAnimatedImageSettingsDlg::CAnimatedImageSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimatedImageSettingsDlg::IDD, pParent)
{
	m_lpOverlay			= NULL;
	m_hBrush			= NULL;

	m_strSource			= "";
	m_strText			= "";

	m_nImageTemplate	= 0;
	m_bResizeToExtent	= FALSE;
	m_nImageWidth		= DEFAULT_ANIMATED_IMAGE_WIDTH;
	m_nImageHeight		= DEFAULT_ANIMATED_IMAGE_HEIGHT;
	m_fltImageMin		= DEFAULT_ANIMATED_IMAGE_MIN;
	m_fltImageMax		= DEFAULT_ANIMATED_IMAGE_MAX;
	m_nSpriteX			= DEFAULT_ANIMATED_IMAGE_SPRITE_X;
	m_nSpriteY			= DEFAULT_ANIMATED_IMAGE_SPRITE_Y;
	m_nSpriteWidth		= DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH;
	m_nSpriteHeight		= DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT;
	m_nSpritesNum		= DEFAULT_ANIMATED_IMAGE_SPRITES_NUM;
	m_nSpritesPerLine	= DEFAULT_ANIMATED_IMAGE_SPRITES_PER_LINE;
	m_fltImageBias		= DEFAULT_ANIMATED_IMAGE_BIAS;
	m_nImageRotationAngleMin = DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
	m_nImageRotationAngleMax = DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
	m_nSpritePreview	= 0;

	m_strImageMin		= "";
	m_strImageMax		= "";
	m_strImageBias		= "";
}
/////////////////////////////////////////////////////////////////////////////
CAnimatedImageSettingsDlg::~CAnimatedImageSettingsDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_SOURCES_COMBO, m_ctrlDataSourcesCombo);
	DDX_Text(pDX, IDC_IMAGE_WIDTH_EDIT, m_nImageWidth);
	DDX_Text(pDX, IDC_IMAGE_HEIGHT_EDIT, m_nImageHeight);
	DDX_Text(pDX, IDC_IMAGE_MIN_EDIT, m_strImageMin);
	DDX_Text(pDX, IDC_IMAGE_MAX_EDIT, m_strImageMax);
	DDX_Text(pDX, IDC_SPRITE_X_EDIT, m_nSpriteX);
	DDX_Text(pDX, IDC_SPRITE_Y_EDIT, m_nSpriteY);
	DDX_Text(pDX, IDC_SPRITE_WIDTH_EDIT, m_nSpriteWidth);
	DDX_Text(pDX, IDC_SPRITE_HEIGHT_EDIT, m_nSpriteHeight);
	DDX_Text(pDX, IDC_SPRITES_NUM_EDIT, m_nSpritesNum);
	DDX_Text(pDX, IDC_SPRITES_PER_LINE_EDIT, m_nSpritesPerLine);
	DDX_CBIndex(pDX, IDC_IMAGE_TEMPLATE_COMBO, m_nImageTemplate);
	DDX_Check(pDX, IDC_RESIZE_TO_EXTENT, m_bResizeToExtent);
	DDX_Control(pDX, IDC_SPRITE_X_EDIT, m_ctrlSpriteXEdit);
	DDX_Control(pDX, IDC_SPRITE_Y_EDIT, m_ctrlSpriteYEdit);
	DDX_Control(pDX, IDC_SPRITE_WIDTH_EDIT, m_ctrlSpriteWidthEdit);
	DDX_Control(pDX, IDC_SPRITE_HEIGHT_EDIT, m_ctrlSpriteHeightEdit);
	DDX_Control(pDX, IDC_SPRITES_NUM_EDIT, m_ctrlSpritesNumEdit);
	DDX_Control(pDX, IDC_SPRITES_PER_LINE_EDIT, m_ctrlSpritesPerLineEdit);
	DDX_Control(pDX, IDC_ANIMATION_SCROLLBAR, m_ctrlAnimationScrollbar);
	DDX_Text(pDX, IDC_IMAGE_BIAS_EDIT, m_strImageBias);
	DDX_Text(pDX, IDC_ROTATION_ANGLE_MIN_EDIT, m_nImageRotationAngleMin);
	DDX_Text(pDX, IDC_ROTATION_ANGLE_MAX_EDIT, m_nImageRotationAngleMax);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAnimatedImageSettingsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAnimatedImageSettingsDlg::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_IMAGE_TEMPLATE_COMBO, &CAnimatedImageSettingsDlg::OnCbnSelchangeImageTemplateCombo)
	ON_NOTIFY(UDN_DELTAPOS, IDC_IMAGE_WIDTH_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposImageWidthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_IMAGE_HEIGHT_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposImageHeightSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITE_X_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposSpriteXSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITE_Y_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposSpriteYSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITE_WIDTH_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposSpriteWidthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITE_HEIGHT_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposSpriteHeightSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITES_NUM_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposSpritesNumSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITES_PER_LINE_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposSpritesPerLineSpin)
	ON_BN_CLICKED(IDC_RESIZE_TO_EXTENT, &CAnimatedImageSettingsDlg::OnBnClickedResizeToExtent)
	ON_BN_CLICKED(IDC_SELECT_SPRITE, &CAnimatedImageSettingsDlg::OnBnClickedSelectSprite)
	ON_EN_CHANGE(IDC_SPRITE_X_EDIT, &CAnimatedImageSettingsDlg::OnEnChangeSpriteXEdit)
	ON_EN_CHANGE(IDC_SPRITE_Y_EDIT, &CAnimatedImageSettingsDlg::OnEnChangeSpriteYEdit)
	ON_EN_CHANGE(IDC_SPRITE_WIDTH_EDIT, &CAnimatedImageSettingsDlg::OnEnChangeSpriteWidthEdit)
	ON_EN_CHANGE(IDC_SPRITE_HEIGHT_EDIT, &CAnimatedImageSettingsDlg::OnEnChangeSpriteHeightEdit)
	ON_EN_CHANGE(IDC_SPRITES_NUM_EDIT, &CAnimatedImageSettingsDlg::OnEnChangeSpritesNumEdit)
	ON_EN_CHANGE(IDC_SPRITES_PER_LINE_EDIT, &CAnimatedImageSettingsDlg::OnEnChangeSpritesPerLineEdit)
	ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_ROTATION_ANGLE_MIN_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposRotationAngleMinSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ROTATION_ANGLE_MAX_SPIN, &CAnimatedImageSettingsDlg::OnDeltaposRotationAngleMaxSpin)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CAnimatedImageSettingsDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::UpdateImageControls()
{
	BOOL bEnable = (m_nImageTemplate != 0);

	GetDlgItem(IDC_RESIZE_TO_EXTENT			)->EnableWindow(bEnable);
	
	GetDlgItem(IDC_IMAGE_WIDTH_CAPTION		)->EnableWindow(bEnable && !m_bResizeToExtent);
	GetDlgItem(IDC_IMAGE_WIDTH_EDIT			)->EnableWindow(bEnable && !m_bResizeToExtent);

	GetDlgItem(IDC_IMAGE_HEIGHT_CAPTION		)->EnableWindow(bEnable && !m_bResizeToExtent);
	GetDlgItem(IDC_IMAGE_HEIGHT_EDIT		)->EnableWindow(bEnable && !m_bResizeToExtent);

	GetDlgItem(IDC_IMAGE_MIN_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_IMAGE_MIN_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_IMAGE_MAX_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_IMAGE_MAX_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_IMAGE_BIAS_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_IMAGE_BIAS_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_ROTATION_ANGLE_MIN_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_ROTATION_ANGLE_MIN_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_ROTATION_ANGLE_MAX_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_ROTATION_ANGLE_MAX_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_SPRITE_X_CAPTION			)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPRITE_X_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_SPRITE_Y_CAPTION			)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPRITE_Y_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_SPRITE_WIDTH_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPRITE_WIDTH_EDIT		)->EnableWindow(bEnable);

	GetDlgItem(IDC_SPRITE_HEIGHT_CAPTION	)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPRITE_HEIGHT_EDIT		)->EnableWindow(bEnable);

	GetDlgItem(IDC_SPRITES_NUM_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPRITES_NUM_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_SPRITES_PER_LINE_CAPTION	)->EnableWindow(bEnable);
	GetDlgItem(IDC_SPRITES_PER_LINE_EDIT	)->EnableWindow(bEnable);

	GetDlgItem(IDC_SELECT_SPRITE			)->EnableWindow(bEnable);

	m_imageWnd.EnableWindow(bEnable);
	m_ctrlAnimationScrollbar.EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAnimatedImageSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	CWnd* pPlaceholder = GetDlgItem(IDC_IMAGE_PREVIEW_PLACEHOLDER);

	if (pPlaceholder)
	{
		CRect rect; 
		pPlaceholder->GetWindowRect(&rect);
		ScreenToClient(&rect);

		CRTSSHooksInterface rtssInterface;

		DWORD dwWidth = rtssInterface.GetFontTextureWidth();

		if (dwWidth)
		{
			m_imageWnd.CreateImage(dwWidth, dwWidth);

			rtssInterface.CopyFontTexture(dwWidth, dwWidth, m_imageWnd.GetImage());
		}

		if (!m_imageWnd.CreateEx(WS_EX_CLIENTEDGE, AfxRegisterWndClass(CS_OWNDC|CS_DBLCLKS, ::LoadCursor(NULL,IDC_ARROW)), NULL, WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE, rect, this, 0))
			return FALSE;
	}

	if (m_lpOverlay)
	{
		if (!m_nImageWidth && !m_nImageHeight)
			m_bResizeToExtent = TRUE;

		COverlayDataSourcesList* lpInternalDataSources = m_lpOverlay->GetInternalDataSources();

		POSITION pos = lpInternalDataSources->GetHeadPosition();

		while (pos)
		{
			COverlayDataSource* lpSource = lpInternalDataSources->GetNext(pos);

			CString strSource = lpSource->GetName();

			if (m_ctrlDataSourcesCombo.FindString(-1, strSource) == -1)
			{
				int iItem = m_ctrlDataSourcesCombo.AddString(strSource);

				if (!strcmp(m_strSource, strSource))
					m_ctrlDataSourcesCombo.SetCurSel(iItem);
			}
		}

		COverlayDataSourcesList* lpExternalDataSources = m_lpOverlay->GetExternalDataSources();

		pos = lpExternalDataSources->GetHeadPosition();

		while (pos)
		{
			COverlayDataSource* lpSource = lpExternalDataSources->GetNext(pos);

			CString strSource = lpSource->GetName();

			if (m_ctrlDataSourcesCombo.FindString(-1, strSource) == -1)
			{
				int iItem = m_ctrlDataSourcesCombo.AddString(strSource);

				if (!strcmp(m_strSource, strSource))
					m_ctrlDataSourcesCombo.SetCurSel(iItem);
			}
		}

		if (m_ctrlDataSourcesCombo.GetCurSel() == -1)
			m_ctrlDataSourcesCombo.SetCurSel(0);

		m_strImageMin	= COverlayDataSource::FormatFloat(m_fltImageMin);
		m_strImageMax	= COverlayDataSource::FormatFloat(m_fltImageMax);
		m_strImageBias	= COverlayDataSource::FormatFloat(m_fltImageBias);

		UpdateImageControls();
		UpdateImageWnd();

		UpdateData(FALSE);
	}

	return TRUE; 
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	int iSelection = m_ctrlDataSourcesCombo.GetCurSel();

	if (iSelection != -1)
	{
		if (m_bResizeToExtent)
		{
			m_nImageWidth	= 0;
			m_nImageHeight	= 0;

			UpdateData(FALSE);
		}

		m_ctrlDataSourcesCombo.GetLBText(iSelection, m_strSource);

		sscanf_s(m_strImageMin	, "%f", &m_fltImageMin);
		sscanf_s(m_strImageMax	, "%f", &m_fltImageMax);
		sscanf_s(m_strImageBias	, "%f", &m_fltImageBias);

		CHypertextExtension extension;

		m_strText = extension.FormatAnimatedImageTag(m_nImageTemplate ? TRUE : FALSE, m_strSource, m_nImageWidth, m_nImageHeight, m_fltImageMin, m_fltImageMax, m_nSpriteX, m_nSpriteY, m_nSpriteWidth, m_nSpriteHeight, m_nSpritesNum, m_nSpritesPerLine, m_fltImageBias, m_nImageRotationAngleMin, m_nImageRotationAngleMax);
	}

	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDestroy()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);

		m_hBrush = NULL;
	}

	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH CAnimatedImageSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	COLORREF clrBk		= g_dwHeaderBgndColor;
	COLORREF clrText	= g_dwHeaderTextColor;

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT nID = pWnd->GetDlgCtrlID();

	if (nID == IDC_IMAGE_TEMPLATE_PROPERTIES_HEADER)
	{
		if (!m_hBrush)
 			m_hBrush = CreateSolidBrush(clrBk);

		pDC->SetBkColor(clrBk);
		pDC->SetTextColor(clrText);

		return m_hBrush;
	}

	return hbr;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnCbnSelchangeImageTemplateCombo()
{
	UpdateData(TRUE);

	UpdateImageControls();
	UpdateImageWnd();
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposImageWidthSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nImageWidth = (m_nImageWidth < 0) ? (m_nImageWidth + pNMUpDown->iDelta) : (m_nImageWidth - pNMUpDown->iDelta);

	if (nImageWidth < -5000)
		nImageWidth = -5000;

	if (nImageWidth > 5000)
		nImageWidth = 5000;

	m_nImageWidth = nImageWidth;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposImageHeightSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nImageHeight = (m_nImageHeight < 0) ? (m_nImageHeight + pNMUpDown->iDelta) : (m_nImageHeight - pNMUpDown->iDelta);

	if (nImageHeight < -5000)
		nImageHeight = -5000;

	if (nImageHeight > 5000)
		nImageHeight = 5000;

	m_nImageHeight = nImageHeight;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposSpriteXSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSpriteX = m_nSpriteX - pNMUpDown->iDelta * m_nSpriteWidth;

	if (nSpriteX < 0)
		nSpriteX = 0;

	if (nSpriteX > 2048)
		nSpriteX = 2048;

	m_nSpriteX = nSpriteX;

	UpdateData(FALSE);

	UpdateImageWnd();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposSpriteYSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSpriteY = m_nSpriteY - pNMUpDown->iDelta * m_nSpriteHeight;

	if (nSpriteY < 0)
		nSpriteY = 0;

	if (nSpriteY > 2048)
		nSpriteY = 2048;

	m_nSpriteY = nSpriteY;

	UpdateData(FALSE);

	UpdateImageWnd();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposSpriteWidthSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSpriteWidth = m_nSpriteWidth - pNMUpDown->iDelta;

	if (nSpriteWidth < 0)
		nSpriteWidth = 0;

	if (nSpriteWidth > 2048)
		nSpriteWidth = 2048;

	m_nSpriteWidth = nSpriteWidth;

	UpdateData(FALSE);

	UpdateImageWnd();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposSpriteHeightSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSpriteHeight = m_nSpriteHeight - pNMUpDown->iDelta;

	if (nSpriteHeight < 0)
		nSpriteHeight = 0;

	if (nSpriteHeight > 2048)
		nSpriteHeight = 2048;

	m_nSpriteHeight = nSpriteHeight;

	UpdateData(FALSE);

	UpdateImageWnd();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposSpritesNumSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSpritesNum = m_nSpritesNum - pNMUpDown->iDelta;

	if (nSpritesNum < 0)
		nSpritesNum = 0;

	if (nSpritesNum > 256)
		nSpritesNum = 256;

	m_nSpritesNum = nSpritesNum;

	UpdateData(FALSE);

	UpdateImageWnd();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposSpritesPerLineSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSpritesPerLine = m_nSpritesPerLine - pNMUpDown->iDelta;

	if (nSpritesPerLine < 0)
		nSpritesPerLine = 0;

	if (nSpritesPerLine > 256)
		nSpritesPerLine = 256;

	m_nSpritesPerLine = nSpritesPerLine;

	UpdateData(FALSE);

	UpdateImageWnd();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnBnClickedResizeToExtent()
{
	UpdateData(TRUE);

	if (!m_bResizeToExtent)
	{
		if (!m_nImageWidth)
			m_nImageWidth = DEFAULT_ANIMATED_IMAGE_WIDTH;
		if (!m_nImageHeight)
			m_nImageHeight = DEFAULT_ANIMATED_IMAGE_HEIGHT;

		UpdateData(FALSE);
	}

	UpdateImageControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::UpdateImageWnd()
{
	int nSpritesNum		= m_nSpritesNum;
	int nSpritesPerLine	= m_nSpritesPerLine;

	if (!nSpritesNum)
		nSpritesNum = 1;

	if (!nSpritesPerLine)
		nSpritesPerLine = nSpritesNum;

	if (m_nSpritePreview < 0)
		m_nSpritePreview = 0;

	if (m_nSpritePreview > nSpritesNum - 1)
		m_nSpritePreview = nSpritesNum - 1;

	int nSpriteIndexX	= m_nSpritePreview % nSpritesPerLine;
	int nSpriteIndexY	= m_nSpritePreview / nSpritesPerLine;

	if (m_nSpritesNum > 1)
		m_ctrlAnimationScrollbar.ShowWindow(SW_SHOW);
	else
		m_ctrlAnimationScrollbar.ShowWindow(SW_HIDE);

	m_ctrlAnimationScrollbar.SetScrollRange(0, nSpritesNum);
	m_ctrlAnimationScrollbar.SetScrollPos(m_nSpritePreview);

	m_imageWnd.SetImageRect(m_nSpriteX + nSpriteIndexX * m_nSpriteWidth, m_nSpriteY + nSpriteIndexY * m_nSpriteHeight, m_nSpriteWidth, m_nSpriteHeight);
	m_imageWnd.DrawImage();
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnBnClickedSelectSprite()
{
	CSelectSpriteDlg dlg;

	dlg.m_nSpriteX			= m_nSpriteX;
	dlg.m_nSpriteY			= m_nSpriteY;
	dlg.m_nSpriteWidth		= m_nSpriteWidth;
	dlg.m_nSpriteHeight		= m_nSpriteHeight;
	dlg.m_nSpritesNum		= m_nSpritesNum;
	dlg.m_nSpritesPerLine	= m_nSpritesPerLine;

	dlg.DoModal();

	if (!dlg.IsCanceled())
	{
		m_nSpriteX		= dlg.m_nSpriteX;
		m_nSpriteY		= dlg.m_nSpriteY;
		m_nSpriteWidth	= dlg.m_nSpriteWidth;
		m_nSpriteHeight	= dlg.m_nSpriteHeight;

		UpdateData(FALSE);
		UpdateImageWnd();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnEnChangeSpriteXEdit()
{
	CString strText;
	m_ctrlSpriteXEdit.GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		UpdateData(TRUE);
		UpdateImageWnd();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnEnChangeSpriteYEdit()
{
	CString strText;
	m_ctrlSpriteYEdit.GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		UpdateData(TRUE);
		UpdateImageWnd();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnEnChangeSpriteWidthEdit()
{
	CString strText;
	m_ctrlSpriteWidthEdit.GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		UpdateData(TRUE);
		UpdateImageWnd();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnEnChangeSpriteHeightEdit()
{
	CString strText;
	m_ctrlSpriteHeightEdit.GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		UpdateData(TRUE);
		UpdateImageWnd();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnEnChangeSpritesNumEdit()
{
	CString strText;
	m_ctrlSpritesNumEdit.GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		UpdateData(TRUE);
		UpdateImageWnd();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnEnChangeSpritesPerLineEdit()
{
	CString strText;
	m_ctrlSpritesPerLineEdit.GetWindowText(strText);

	if (!strText.IsEmpty())
	{
		UpdateData(TRUE);
		UpdateImageWnd();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == &m_ctrlAnimationScrollbar)
	{
		if (nSBCode == SB_THUMBTRACK)
		{
			m_nSpritePreview = nPos;

			UpdateImageWnd();
		}

		if (nSBCode == SB_LINELEFT)
		{
			m_nSpritePreview--;

			int nSpritesNum	= m_nSpritesNum;

			if (!nSpritesNum)
				nSpritesNum = 1;

			if (m_nSpritePreview < 0)
				m_nSpritePreview = nSpritesNum - 1;

			UpdateImageWnd();
		}

		if (nSBCode == SB_LINERIGHT)
		{
			m_nSpritePreview++;

			int nSpritesNum	= m_nSpritesNum;

			if (!nSpritesNum)
				nSpritesNum = 1;

			if (m_nSpritePreview > nSpritesNum - 1)
				m_nSpritePreview = 0;

			UpdateImageWnd();
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposRotationAngleMinSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nRotationAngle = m_nImageRotationAngleMin - pNMUpDown->iDelta;

	if (nRotationAngle < -360)
		nRotationAngle = -360;

	if (nRotationAngle > 360)
		nRotationAngle = 360;

	m_nImageRotationAngleMin = nRotationAngle;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CAnimatedImageSettingsDlg::OnDeltaposRotationAngleMaxSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nRotationAngle = m_nImageRotationAngleMax - pNMUpDown->iDelta;

	if (nRotationAngle < -360)
		nRotationAngle = -360;

	if (nRotationAngle > 360)
		nRotationAngle = 360;

	m_nImageRotationAngleMax = nRotationAngle;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
