// ImageSettingsDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "ImageSettingsDlg.h"
#include "RTSSHooksInterface.h"
#include "SelectSpriteDlg.h"
/////////////////////////////////////////////////////////////////////////////
// CImageSettingsDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CImageSettingsDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
CImageSettingsDlg::CImageSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageSettingsDlg::IDD, pParent)
	, m_nImageRotationAngle(0)
{
	m_lpOverlay			= NULL;

	m_strText			= "";

	m_bResizeToExtent	= FALSE;
	m_nImageWidth		= DEFAULT_ANIMATED_IMAGE_WIDTH;
	m_nImageHeight		= DEFAULT_ANIMATED_IMAGE_HEIGHT;
	m_nSpriteX			= DEFAULT_ANIMATED_IMAGE_SPRITE_X;
	m_nSpriteY			= DEFAULT_ANIMATED_IMAGE_SPRITE_Y;
	m_nSpriteWidth		= DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH;
	m_nSpriteHeight		= DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT;
	m_nImageRotationAngle = DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
}
/////////////////////////////////////////////////////////////////////////////
CImageSettingsDlg::~CImageSettingsDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CImageSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IMAGE_WIDTH_EDIT, m_nImageWidth);
	DDX_Text(pDX, IDC_IMAGE_HEIGHT_EDIT, m_nImageHeight);
	DDX_Text(pDX, IDC_SPRITE_X_EDIT, m_nSpriteX);
	DDX_Text(pDX, IDC_SPRITE_Y_EDIT, m_nSpriteY);
	DDX_Text(pDX, IDC_SPRITE_WIDTH_EDIT, m_nSpriteWidth);
	DDX_Text(pDX, IDC_SPRITE_HEIGHT_EDIT, m_nSpriteHeight);
	DDX_Check(pDX, IDC_RESIZE_TO_EXTENT, m_bResizeToExtent);
	DDX_Control(pDX, IDC_SPRITE_X_EDIT, m_ctrlSpriteXEdit);
	DDX_Control(pDX, IDC_SPRITE_Y_EDIT, m_ctrlSpriteYEdit);
	DDX_Control(pDX, IDC_SPRITE_WIDTH_EDIT, m_ctrlSpriteWidthEdit);
	DDX_Control(pDX, IDC_SPRITE_HEIGHT_EDIT, m_ctrlSpriteHeightEdit);
	DDX_Text(pDX, IDC_ROTATION_ANGLE_EDIT, m_nImageRotationAngle);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CImageSettingsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CImageSettingsDlg::OnBnClickedOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_IMAGE_WIDTH_SPIN, &CImageSettingsDlg::OnDeltaposImageWidthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_IMAGE_HEIGHT_SPIN, &CImageSettingsDlg::OnDeltaposImageHeightSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITE_X_SPIN, &CImageSettingsDlg::OnDeltaposSpriteXSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITE_Y_SPIN, &CImageSettingsDlg::OnDeltaposSpriteYSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITE_WIDTH_SPIN, &CImageSettingsDlg::OnDeltaposSpriteWidthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPRITE_HEIGHT_SPIN, &CImageSettingsDlg::OnDeltaposSpriteHeightSpin)
	ON_BN_CLICKED(IDC_RESIZE_TO_EXTENT, &CImageSettingsDlg::OnBnClickedResizeToExtent)
	ON_EN_CHANGE(IDC_SPRITE_X_EDIT, &CImageSettingsDlg::OnEnChangeSpriteXEdit)
	ON_EN_CHANGE(IDC_SPRITE_Y_EDIT, &CImageSettingsDlg::OnEnChangeSpriteYEdit)
	ON_EN_CHANGE(IDC_SPRITE_WIDTH_EDIT, &CImageSettingsDlg::OnEnChangeSpriteWidthEdit)
	ON_EN_CHANGE(IDC_SPRITE_HEIGHT_EDIT, &CImageSettingsDlg::OnEnChangeSpriteHeightEdit)
	ON_BN_CLICKED(IDC_SELECT_SPRITE, &CImageSettingsDlg::OnBnClickedSelectSprite)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ROTATION_ANGLE_SPIN, &CImageSettingsDlg::OnDeltaposRotationAngleSpin)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CImageSettingsDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void CImageSettingsDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CImageSettingsDlg::OnInitDialog()
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

	if (!m_nImageWidth && !m_nImageHeight)
		m_bResizeToExtent = TRUE;

	UpdateImageControls();
	UpdateImageWnd();

	UpdateData(FALSE);

	return TRUE; 
}
/////////////////////////////////////////////////////////////////////////////
void CImageSettingsDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_bResizeToExtent)
	{
		m_nImageWidth	= 0;
		m_nImageHeight	= 0;

		UpdateData(FALSE);
	}

	CHypertextExtension extension;

	m_strText = extension.FormatImageTag(m_nImageWidth, m_nImageHeight, m_nSpriteX, m_nSpriteY, m_nSpriteWidth, m_nSpriteHeight, m_nImageRotationAngle);

	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CImageSettingsDlg::OnDeltaposImageWidthSpin(NMHDR *pNMHDR, LRESULT *pResult)
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

	UpdateImageWnd();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CImageSettingsDlg::OnDeltaposImageHeightSpin(NMHDR *pNMHDR, LRESULT *pResult)
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

	UpdateImageWnd();

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CImageSettingsDlg::OnDeltaposSpriteXSpin(NMHDR *pNMHDR, LRESULT *pResult)
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
void CImageSettingsDlg::OnDeltaposSpriteYSpin(NMHDR *pNMHDR, LRESULT *pResult)
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
void CImageSettingsDlg::OnDeltaposSpriteWidthSpin(NMHDR *pNMHDR, LRESULT *pResult)
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
void CImageSettingsDlg::OnDeltaposSpriteHeightSpin(NMHDR *pNMHDR, LRESULT *pResult)
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
void CImageSettingsDlg::UpdateImageControls()
{
	GetDlgItem(IDC_IMAGE_WIDTH_CAPTION		)->EnableWindow(!m_bResizeToExtent);
	GetDlgItem(IDC_IMAGE_WIDTH_EDIT			)->EnableWindow(!m_bResizeToExtent);

	GetDlgItem(IDC_IMAGE_HEIGHT_CAPTION		)->EnableWindow(!m_bResizeToExtent);
	GetDlgItem(IDC_IMAGE_HEIGHT_EDIT		)->EnableWindow(!m_bResizeToExtent);
}
/////////////////////////////////////////////////////////////////////////////
void CImageSettingsDlg::OnBnClickedResizeToExtent()
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
void CImageSettingsDlg::UpdateImageWnd()
{
	m_imageWnd.SetImageRect(m_nSpriteX, m_nSpriteY, m_nSpriteWidth, m_nSpriteHeight);
	m_imageWnd.DrawImage();
}
/////////////////////////////////////////////////////////////////////////////
void CImageSettingsDlg::OnEnChangeSpriteXEdit()
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
void CImageSettingsDlg::OnEnChangeSpriteYEdit()
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
void CImageSettingsDlg::OnEnChangeSpriteWidthEdit()
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
void CImageSettingsDlg::OnEnChangeSpriteHeightEdit()
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
void CImageSettingsDlg::OnBnClickedSelectSprite()
{
	CSelectSpriteDlg dlg;

	dlg.m_nSpriteX		= m_nSpriteX;
	dlg.m_nSpriteY		= m_nSpriteY;
	dlg.m_nSpriteWidth	= m_nSpriteWidth;
	dlg.m_nSpriteHeight	= m_nSpriteHeight;

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
void CImageSettingsDlg::OnDeltaposRotationAngleSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nRotationAngle = m_nImageRotationAngle - pNMUpDown->iDelta;

	if (nRotationAngle < -360)
		nRotationAngle = -360;

	if (nRotationAngle > 360)
		nRotationAngle = 360;

	m_nImageRotationAngle = nRotationAngle;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
