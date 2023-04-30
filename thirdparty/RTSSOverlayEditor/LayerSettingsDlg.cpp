// LayerSettingsDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayLayer.h"
#include "RTSSSharedMemory.h"
#include "LayerSettingsDlg.h"
#include "AddEmbeddedObjectDlg.h"
#include "GraphSettingsDlg.h"
#include "ImageSettingsDlg.h"
#include "AnimatedImageSettingsDlg.h"
#include "OverlayMacroList.h"
#include "TextTableSettingsDlg.h"
#include "DynamicColor.h"
/////////////////////////////////////////////////////////////////////////////
// CLayerSettingsDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CLayerSettingsDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
#define DEFAULT_TEXT_COLOR		0xFFFF8000
#define DEFAULT_BGND_COLOR		0x80000000
/////////////////////////////////////////////////////////////////////////////
#define ID_SELECTMACRO			40000
#define ID_SELECTTAG			50000
/////////////////////////////////////////////////////////////////////////////
CLayerSettingsDlg::CLayerSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayerSettingsDlg::IDD, pParent)
{
	m_strName					= "";
	m_nRefreshPeriod			= 0;
	m_strText					= "";
	m_nSize						= 0;
	m_nAlignment				= 0;
	m_bFixedAlignment			= FALSE;
	m_bUseCustomTextColor		= FALSE;
	m_strTextColor				= "";
	m_bUseCustomBgndColor		= FALSE;
	m_strBgndColor				= "";
	m_bBorder					= FALSE;
	m_nBorderSize				= 0;
	m_strBorderColor			= "";
	m_nExtentX					= 0;
	m_nExtentY					= 0;
	m_nExtentOrigin				= 0;
	m_bResizeToContent			= FALSE;
	m_bPosSticky				= FALSE;
	m_nPosSticky				= 0;

	m_nMarginLeft				= 0;
	m_nMarginTop				= 0;
	m_nMarginRight				= 0;
	m_nMarginBottom				= 0;

	m_bResetInitialSelection	= TRUE;

	m_lpOverlay					= NULL;
	m_lpLayer					= NULL;
	m_lpEditorWnd				= NULL;
	m_hBrushTextColorPreview	= NULL;
	m_hBrushBgndColorPreview	= NULL;
	m_hBrushBorderColorPreview	= NULL;
}
/////////////////////////////////////////////////////////////////////////////
CLayerSettingsDlg::~CLayerSettingsDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
	DDX_Text(pDX, IDC_ALIGNMENT_EDIT, m_nAlignment);
	DDV_MinMaxInt(pDX, m_nAlignment, -500, 500);
	DDX_Text(pDX, IDC_SIZE_EDIT, m_nSize);
	DDV_MinMaxInt(pDX, m_nSize, -500, 500);
	DDX_Check(pDX, IDC_USE_CUSTOM_TEXT_COLOR_CHECK, m_bUseCustomTextColor);
	DDX_Check(pDX, IDC_USE_CUSTOM_BGND_COLOR_CHECK, m_bUseCustomBgndColor);
	DDX_Check(pDX, IDC_FIXED_ALIGNMENT_CHECK, m_bFixedAlignment);
	DDX_Check(pDX, IDC_RESIZE_TO_CONTENT_CHECK, m_bResizeToContent);
	DDX_Text(pDX, IDC_REFRESH_PERIOD_EDIT, m_nRefreshPeriod);
	DDV_MinMaxInt(pDX, m_nRefreshPeriod, 0, 60000);
	DDX_Check(pDX, IDC_POS_STICKY_CHECK, m_bPosSticky);
	DDX_Text(pDX, IDC_MARGIN_LEFT_EDIT, m_nMarginLeft);
	DDX_Text(pDX, IDC_MARGIN_TOP_EDIT, m_nMarginTop);
	DDX_Text(pDX, IDC_MARGIN_RIGHT_EDIT, m_nMarginRight);
	DDX_Text(pDX, IDC_MARGIN_BOTTOM_EDIT, m_nMarginBottom);
	DDX_Text(pDX, IDC_BORDER_SIZE_EDIT, m_nBorderSize);
	DDX_Check(pDX, IDC_BORDER_COLOR_CHECK, m_bBorder);
	DDV_MinMaxInt(pDX, m_nBorderSize, -500, 500);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLayerSettingsDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_TEXT_COLOR_PREVIEW, &CLayerSettingsDlg::OnStnClickedTextColorPreview)
	ON_STN_CLICKED(IDC_BGND_COLOR_PREVIEW, &CLayerSettingsDlg::OnStnClickedBgndColorPreview)
	ON_BN_CLICKED(IDC_USE_CUSTOM_TEXT_COLOR_CHECK, &CLayerSettingsDlg::OnBnClickedUseCustomTextColorCheck)
	ON_BN_CLICKED(IDC_USE_CUSTOM_BGND_COLOR_CHECK, &CLayerSettingsDlg::OnBnClickedUseCustomBgndColorCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SIZE_SPIN, &CLayerSettingsDlg::OnDeltaposSizeSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_ALIGNMENT_SPIN, &CLayerSettingsDlg::OnDeltaposAlignmentSpin)
	ON_BN_CLICKED(IDC_ADD_EMBEDDED_OBJECT, &CLayerSettingsDlg::OnBnClickedAddEmbeddedObject)
	ON_BN_CLICKED(IDC_EDIT_EMBEDDED_OBJECT, &CLayerSettingsDlg::OnBnClickedEditEmbeddedObject)
	ON_COMMAND_RANGE(ID_SELECTMACRO, ID_SELECTMACRO + 99, OnSelectMacro)
	ON_COMMAND_RANGE(ID_SELECTTAG, ID_SELECTTAG + 99, OnSelectTag)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_0, &CLayerSettingsDlg::OnBnClickedExtentOrigin0)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_1, &CLayerSettingsDlg::OnBnClickedExtentOrigin1)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_2, &CLayerSettingsDlg::OnBnClickedExtentOrigin2)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_3, &CLayerSettingsDlg::OnBnClickedExtentOrigin3)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_4, &CLayerSettingsDlg::OnBnClickedExtentOrigin4)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_5, &CLayerSettingsDlg::OnBnClickedExtentOrigin5)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_6, &CLayerSettingsDlg::OnBnClickedExtentOrigin6)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_7, &CLayerSettingsDlg::OnBnClickedExtentOrigin7)
	ON_BN_CLICKED(IDC_EXTENT_ORIGIN_8, &CLayerSettingsDlg::OnBnClickedExtentOrigin8)
	ON_BN_CLICKED(IDC_RESIZE_TO_CONTENT_CHECK, &CLayerSettingsDlg::OnBnClickedResizeToContentCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REFRESH_PERIOD_SPIN, &CLayerSettingsDlg::OnDeltaposRefreshPeriodSpin)
	ON_BN_CLICKED(IDC_POS_STICKY_0, &CLayerSettingsDlg::OnBnClickedPosSticky0)
	ON_BN_CLICKED(IDC_POS_STICKY_1, &CLayerSettingsDlg::OnBnClickedPosSticky1)
	ON_BN_CLICKED(IDC_POS_STICKY_2, &CLayerSettingsDlg::OnBnClickedPosSticky2)
	ON_BN_CLICKED(IDC_POS_STICKY_3, &CLayerSettingsDlg::OnBnClickedPosSticky3)
	ON_BN_CLICKED(IDC_POS_STICKY_4, &CLayerSettingsDlg::OnBnClickedPosSticky4)
	ON_BN_CLICKED(IDC_POS_STICKY_5, &CLayerSettingsDlg::OnBnClickedPosSticky5)
	ON_BN_CLICKED(IDC_POS_STICKY_6, &CLayerSettingsDlg::OnBnClickedPosSticky6)
	ON_BN_CLICKED(IDC_POS_STICKY_7, &CLayerSettingsDlg::OnBnClickedPosSticky7)
	ON_BN_CLICKED(IDC_POS_STICKY_8, &CLayerSettingsDlg::OnBnClickedPosSticky8)
	ON_BN_CLICKED(IDC_POS_STICKY_CHECK, &CLayerSettingsDlg::OnBnClickedPosStickyCheck)
	ON_BN_CLICKED(ID_APPLY, &CLayerSettingsDlg::OnBnClickedApply)
	ON_STN_CLICKED(IDC_BORDER_COLOR_PREVIEW, &CLayerSettingsDlg::OnStnClickedBorderColorPreview)
	ON_BN_CLICKED(IDC_BORDER_COLOR_CHECK, &CLayerSettingsDlg::OnBnClickedBorderColorCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BORDER_SIZE_SPIN, &CLayerSettingsDlg::OnDeltaposBorderSizeSpin)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CLayerSettingsDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CLayerSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	m_strText = COverlayLayer::EncodeSpecialChars(m_strText);

	m_bUseCustomTextColor	= !m_strTextColor.IsEmpty();
	m_bUseCustomBgndColor	= !m_strBgndColor.IsEmpty();
	m_bBorder				= !m_strBorderColor.IsEmpty() && (m_nBorderSize != 0);

	CWnd* pPlaceholder = GetDlgItem(IDC_TEXT_EDIT_PLACEHOLDER);

	if (pPlaceholder)
	{
		m_ctrlTextEdit.SetOverlay(m_lpOverlay);

		CRect rect; 
		pPlaceholder->GetWindowRect(&rect);
		ScreenToClient(&rect);

		if (!m_ctrlTextEdit.Create(WS_VISIBLE|WS_TABSTOP|ES_SUNKEN|ES_MULTILINE, rect, this, IDC_TEXT_EDIT))
			return FALSE;

		m_ctrlTextEdit.SetFont(GetFont());
		m_ctrlTextEdit.SetWindowTextEx(m_strText);
		m_ctrlTextEdit.SetEventMask(ENM_SELCHANGE);

		m_ctrlTextEdit.SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	}

	m_bResizeToContent = (!m_nExtentX || !m_nExtentY);

	m_bPosSticky = (m_nPosSticky != -1);
	
	UpdateData(FALSE);

	UpdateColorControls();
	UpdateObjectControls();
	UpdateExtentControls();
	UpdateStickyControls();

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::UpdateColorControls()
{
	CDynamicColor dynamicTextColor;
	CDynamicColor dynamicBgndColor;
	CDynamicColor dynamicBorderColor;

	DWORD dwTextColor	= GetDynamicTextColor(dynamicTextColor);
	DWORD dwBgndColor	= GetDynamicBgndColor(dynamicBgndColor);
	DWORD dwBorderColor = GetDynamicBorderColor(dynamicBorderColor);

	if (dwTextColor == DEFAULT_COLOR)
		dwTextColor = COverlayLayer::SwapRGB(GetSysColor(COLOR_BTNFACE));
	if (dwBgndColor == DEFAULT_COLOR)
		dwBgndColor = COverlayLayer::SwapRGB(GetSysColor(COLOR_BTNFACE));
	if (dwBorderColor == DEFAULT_COLOR)
		dwBorderColor = COverlayLayer::SwapRGB(GetSysColor(COLOR_BTNFACE));

	GetDlgItem(IDC_TEXT_COLOR_PREVIEW)->SetWindowText((m_bUseCustomTextColor && dynamicTextColor.GetRangeCount()) ? "D" : "");
	GetDlgItem(IDC_BGND_COLOR_PREVIEW)->SetWindowText((m_bUseCustomBgndColor && dynamicBgndColor.GetRangeCount()) ? "D" : "");
	GetDlgItem(IDC_BORDER_COLOR_PREVIEW)->SetWindowText((m_bBorder && dynamicBorderColor.GetRangeCount()) ? "D" : "");

	GetDlgItem(IDC_TEXT_COLOR_PREVIEW)->EnableWindow(m_bUseCustomTextColor);
	GetDlgItem(IDC_BGND_COLOR_PREVIEW)->EnableWindow(m_bUseCustomBgndColor);
	GetDlgItem(IDC_BORDER_COLOR_PREVIEW)->EnableWindow(m_bBorder);
	GetDlgItem(IDC_BORDER_SIZE_EDIT)->EnableWindow(m_bBorder);
	GetDlgItem(IDC_BORDER_SIZE_SPIN)->EnableWindow(m_bBorder);

	if (m_hBrushTextColorPreview)
		DeleteObject(m_hBrushTextColorPreview);
	m_hBrushTextColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(dwTextColor) & 0xFFFFFF);

	GetDlgItem(IDC_TEXT_COLOR_PREVIEW)->RedrawWindow();

	if (m_hBrushBgndColorPreview)
		DeleteObject(m_hBrushBgndColorPreview);

	m_hBrushBgndColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(dwBgndColor) & 0xFFFFFF);

	GetDlgItem(IDC_BGND_COLOR_PREVIEW)->RedrawWindow();

	if (m_hBrushBorderColorPreview)
		DeleteObject(m_hBrushBorderColorPreview);

	m_hBrushBorderColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(dwBorderColor) & 0xFFFFFF);

	GetDlgItem(IDC_BORDER_COLOR_PREVIEW)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnDestroy()
{
	if (m_hBrushTextColorPreview)
		DeleteObject(m_hBrushTextColorPreview);

	if (m_hBrushBgndColorPreview)
		DeleteObject(m_hBrushBgndColorPreview);

	if (m_hBrushBorderColorPreview)
		DeleteObject(m_hBrushBorderColorPreview);

	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH CLayerSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{	
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT nID = pWnd->GetDlgCtrlID();

	if (nID == IDC_TEXT_COLOR_PREVIEW)
	{
		pDC->SetBkMode(TRANSPARENT);

		if (GetDlgItem(IDC_TEXT_COLOR_PREVIEW)->IsWindowEnabled())
			return m_hBrushTextColorPreview;
	}

	if (nID == IDC_BGND_COLOR_PREVIEW)
	{
		pDC->SetBkMode(TRANSPARENT);

		if (GetDlgItem(IDC_BGND_COLOR_PREVIEW)->IsWindowEnabled())
			return m_hBrushBgndColorPreview;
	}

	if (nID == IDC_BORDER_COLOR_PREVIEW)
	{
		pDC->SetBkMode(TRANSPARENT);

		if (GetDlgItem(IDC_BORDER_COLOR_PREVIEW)->IsWindowEnabled())
			return m_hBrushBorderColorPreview;
	}


	return hbr;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnStnClickedTextColorPreview()
{
	DWORD				dwRecentColors[32]		= { 0 };
	DYNAMIC_COLOR_DESC	recentDynamicColors[32]	= { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors, recentDynamicColors);

	m_strTextColor = PickDynamicColor(m_hWnd, m_strTextColor, dwRecentColors, recentDynamicColors, m_lpOverlay);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnStnClickedBgndColorPreview()
{
	DWORD				dwRecentColors[32]		= { 0 };
	DYNAMIC_COLOR_DESC	recentDynamicColors[32]	= { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors, recentDynamicColors);

	m_strBgndColor = PickDynamicColor(m_hWnd, m_strBgndColor, dwRecentColors, recentDynamicColors, m_lpOverlay);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnStnClickedBorderColorPreview()
{
	DWORD				dwRecentColors[32]		= { 0 };
	DYNAMIC_COLOR_DESC	recentDynamicColors[32]	= { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors, recentDynamicColors);

	m_strBorderColor = PickDynamicColor(m_hWnd, m_strBorderColor, dwRecentColors, recentDynamicColors, m_lpOverlay);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedUseCustomTextColorCheck()
{
	UpdateData(TRUE);

	if (m_bUseCustomTextColor && m_strTextColor.IsEmpty())
		m_strTextColor.Format("%X", DEFAULT_TEXT_COLOR);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedUseCustomBgndColorCheck()
{
	UpdateData(TRUE);

	if (m_bUseCustomBgndColor && m_strBgndColor.IsEmpty())
		m_strBgndColor.Format("%X", DEFAULT_BGND_COLOR);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::PreApply()
{
	UpdateData(TRUE);

	m_ctrlTextEdit.GetWindowText(m_strText);

	m_strText = COverlayLayer::DecodeSpecialChars(m_strText);

	UpdateData(FALSE);

	if (!m_bUseCustomTextColor)
		m_strTextColor = "";
	if (!m_bUseCustomBgndColor)
		m_strBgndColor = "";
	if (!m_bBorder)
		m_strBorderColor = "";

	if (m_bResizeToContent)
	{
		m_nExtentX		= 0;
		m_nExtentY		= 0;
	}
	else
	{
		if (!m_nExtentX)
			m_nExtentX = -1;
		if (!m_nExtentY)
			m_nExtentY = -1;
	}

	if (!m_bPosSticky)
		m_nPosSticky = -1;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnOK()
{
	PreApply();

	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnDeltaposSizeSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSize = m_nSize - pNMUpDown->iDelta;

	if (nSize < -500)
		nSize = -500;

	if (nSize > 500)
		nSize = 500;

	m_nSize = nSize;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnDeltaposAlignmentSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nAlignment = m_nAlignment - pNMUpDown->iDelta;

	if (nAlignment < -500)
		nAlignment = -500;

	if (nAlignment > 500)
		nAlignment = 500;

	m_nAlignment = nAlignment;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedAddEmbeddedObject()
{
	CAddEmbeddedObjectDlg dlg;

	dlg.SetOverlay(m_lpOverlay);

	if (IDOK == dlg.DoModal())
	{
		m_ctrlTextEdit.GetWindowText(m_strText);

		if (!strcmp(m_strText, DEFAULT_TEXT))
			m_strText = dlg.m_strText;
		else
			m_strText += dlg.m_strText;

		m_ctrlTextEdit.SetWindowTextEx(m_strText);
		m_ctrlTextEdit.SetSel(strlen(m_strText), strlen(m_strText));
		m_ctrlTextEdit.SetFocus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::SetLayer(COverlayLayer* lpLayer)
{
	m_lpLayer = lpLayer;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::SetEditorWnd(COverlayEditorWnd* lpEditorWnd)
{
	m_lpEditorWnd = lpEditorWnd;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBrowseMacro(int nMode)
{
	CMenu menu; 
	menu.LoadMenu(IDR_MACRO_MENU);
	LocalizeMenu(menu.m_hMenu);

	menu.GetSubMenu(0)->GetSubMenu(0)->RemoveMenu(0, MF_BYPOSITION);
	menu.GetSubMenu(0)->GetSubMenu(1)->RemoveMenu(0, MF_BYPOSITION);
	menu.GetSubMenu(0)->GetSubMenu(2)->RemoveMenu(0, MF_BYPOSITION);
	menu.GetSubMenu(0)->GetSubMenu(3)->RemoveMenu(0, MF_BYPOSITION);

	COverlayMacroList* lpMacroList = m_lpOverlay->GetMacroList();

	if (lpMacroList)
	{	
		int iItem = 0;

		POSITION pos = lpMacroList->GetHeadPosition();

		while (pos)
		{
			CString strMacroName = lpMacroList->GetNext(pos);

			if (!pos)
				break;

			CString strMacroValue = lpMacroList->GetNext(pos);

			CString strMenu;
			strMenu.Format("%s\t%s", strMacroName, strMacroValue);

			switch (lpMacroList->GetType(iItem))
			{
			case MACROTYPE_MACRO:
				menu.GetSubMenu(0)->GetSubMenu(0)->AppendMenu(MF_STRING, ID_SELECTMACRO + iItem, strMenu);
				break;
			case MACROTYPE_INTERNAL_SOURCE:
				menu.GetSubMenu(0)->GetSubMenu(1)->AppendMenu(MF_STRING, ID_SELECTMACRO + iItem, strMenu);
				break;
			case MACROTYPE_EXTERNAL_SOURCE:
				menu.GetSubMenu(0)->GetSubMenu(2)->AppendMenu(MF_STRING, ID_SELECTMACRO + iItem, strMenu);
				break;
			}

			iItem++;
		}
	}

	CStringList* lpTagList = m_lpOverlay->GetTagList();

	if (lpTagList)
	{	
		int iItem = 0;

		POSITION pos = lpTagList->GetHeadPosition();

		while (pos)
		{
			CString strTagName = lpTagList->GetNext(pos);

			if (!pos)
				break;

			CString strTagDesc = lpTagList->GetNext(pos);

			CString strMenu;
			strMenu.Format("%s\t%s", strTagName, LocalizeStr(strTagDesc));

			menu.GetSubMenu(0)->GetSubMenu(3)->AppendMenu(MF_STRING, ID_SELECTTAG + iItem, strMenu);

			iItem++;
		}
	}

	CPoint cp;

	switch (nMode)
	{
	case BROWSEMACRO_MODE_ALL:
		GetCursorPos(&cp); 
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_NONOTIFY, cp.x, cp.y, this);
		break;
	case BROWSEMACRO_MODE_MACRO:
		cp = m_ctrlTextEdit.GetCaretPos();
		m_ctrlTextEdit.ClientToScreen(&cp);
		menu.GetSubMenu(0)->RemoveMenu(3, MF_BYPOSITION);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_NONOTIFY, cp.x, cp.y, this);
		break;
	case BROWSEMACRO_MODE_TAG:
		cp = m_ctrlTextEdit.GetCaretPos();
		m_ctrlTextEdit.ClientToScreen(&cp);
		menu.GetSubMenu(0)->GetSubMenu(3)->TrackPopupMenu(TPM_NONOTIFY, cp.x, cp.y, this);
		break;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedEditEmbeddedObject()
{
	int nObj = m_ctrlTextEdit.GetObj();

	if (nObj == OBJ_TEXT_TABLE)
	{
		CString strTable = m_ctrlTextEdit.GetTextTable();

		CTextTable* lpTable = m_lpOverlay->FindTextTable(strTable);

		if (lpTable)
		{
			CTextTableSettingsDlg dlg;

			dlg.SetOverlay(m_lpOverlay);
			dlg.m_table.Copy(lpTable);

			if (IDOK == dlg.DoModal())
			{
				lpTable->Copy(&dlg.m_table);

				m_ctrlTextEdit.SetSel(m_ctrlTextEdit.GetObjFrom(), m_ctrlTextEdit.GetObjTo());
				m_ctrlTextEdit.ReplaceSel(dlg.m_strText);
				m_ctrlTextEdit.SetFocus();

			}
		}
	}
	else
	if ((nObj == OBJ_MACRO		) ||
		(nObj == OBJ_TEXT_TAG	))
		OnBrowseMacro();
	else
	if (nObj == OBJ_IMAGE)
	{
		DWORD	dwWidth				= m_ctrlTextEdit.GetImageWidth();
		DWORD	dwHeight			= m_ctrlTextEdit.GetImageHeight();
		DWORD	dwSpriteX			= m_ctrlTextEdit.GetImageSpriteX();
		DWORD	dwSpriteY			= m_ctrlTextEdit.GetImageSpriteY();
		DWORD	dwSpriteWidth		= m_ctrlTextEdit.GetImageSpriteWidth();
		DWORD	dwSpriteHeight		= m_ctrlTextEdit.GetImageSpriteHeight();
		int		nImageRotationAngle = m_ctrlTextEdit.GetImageRotationAngle();

		CImageSettingsDlg dlg;

		dlg.m_nImageWidth		= dwWidth;
		dlg.m_nImageHeight		= dwHeight;
		dlg.m_nSpriteX			= dwSpriteX;
		dlg.m_nSpriteY			= dwSpriteY;
		dlg.m_nSpriteWidth		= dwSpriteWidth;
		dlg.m_nSpriteHeight		= dwSpriteHeight;
		dlg.m_nImageRotationAngle = nImageRotationAngle;

		dlg.SetOverlay(m_lpOverlay);

		if (IDOK == dlg.DoModal())
		{
			if (dlg.m_bResizeToExtent)
			{
				m_bResizeToContent = FALSE;

				UpdateData(FALSE);
			}

			m_ctrlTextEdit.SetSel(m_ctrlTextEdit.GetObjFrom(), m_ctrlTextEdit.GetObjTo());
			m_ctrlTextEdit.ReplaceSel(dlg.m_strText);
			m_ctrlTextEdit.SetFocus();
		}
	}
	else
	if (nObj == OBJ_ANIMATED_IMAGE)
	{
		CString strSource			= m_ctrlTextEdit.GetImageSource();

		DWORD	dwWidth				= m_ctrlTextEdit.GetImageWidth();
		DWORD	dwHeight			= m_ctrlTextEdit.GetImageHeight();
		float	fltMin				= m_ctrlTextEdit.GetImageMin();
		float	fltMax				= m_ctrlTextEdit.GetImageMax();
		DWORD	dwSpriteX			= m_ctrlTextEdit.GetImageSpriteX();
		DWORD	dwSpriteY			= m_ctrlTextEdit.GetImageSpriteY();
		DWORD	dwSpriteWidth		= m_ctrlTextEdit.GetImageSpriteWidth();
		DWORD	dwSpriteHeight		= m_ctrlTextEdit.GetImageSpriteHeight();
		DWORD	dwSpritesNum		= m_ctrlTextEdit.GetImageSpritesNum();
		DWORD	dwSpritesPerLine	= m_ctrlTextEdit.GetImageSpritesPerLine();
		float	fltBias				= m_ctrlTextEdit.GetImageBias();
		int		nRotationAngleMin	= m_ctrlTextEdit.GetImageRotationAngleMin();
		int		nRotationAngleMax	= m_ctrlTextEdit.GetImageRotationAngleMax();

		int		nParams				= m_ctrlTextEdit.GetImageParams();

		CAnimatedImageSettingsDlg dlg;

		dlg.m_strSource			=  strSource;

		if (nParams > 1)
			dlg.m_nImageTemplate = 1;

		dlg.m_nImageWidth		= dwWidth;
		dlg.m_nImageHeight		= dwHeight;
		dlg.m_fltImageMin		= fltMin;
		dlg.m_fltImageMax		= fltMax;
		dlg.m_nSpriteX			= dwSpriteX;
		dlg.m_nSpriteY			= dwSpriteY;
		dlg.m_nSpriteWidth		= dwSpriteWidth;
		dlg.m_nSpriteHeight		= dwSpriteHeight;
		dlg.m_nSpritesNum		= dwSpritesNum;
		dlg.m_nSpritesPerLine	= dwSpritesPerLine;
		dlg.m_fltImageBias		= fltBias;
		dlg.m_nImageRotationAngleMin = nRotationAngleMin;
		dlg.m_nImageRotationAngleMax = nRotationAngleMax;

		dlg.SetOverlay(m_lpOverlay);

		if (IDOK == dlg.DoModal())
		{
			if (dlg.m_bResizeToExtent)
			{
				m_bResizeToContent = FALSE;

				UpdateData(FALSE);
			}

			m_ctrlTextEdit.SetSel(m_ctrlTextEdit.GetObjFrom(), m_ctrlTextEdit.GetObjTo());
			m_ctrlTextEdit.ReplaceSel(dlg.m_strText);
			m_ctrlTextEdit.SetFocus();
		}
	}
	else
	if (nObj == OBJ_GRAPH)
	{
		CString strSource		= m_ctrlTextEdit.GetGraphSource();

		DWORD	dwWidth			= m_ctrlTextEdit.GetGraphWidth();
		DWORD	dwHeight		= m_ctrlTextEdit.GetGraphHeight();
		DWORD	dwMargin		= m_ctrlTextEdit.GetGraphMargin();
		float	fltMin			= m_ctrlTextEdit.GetGraphMin();
		float	fltMax			= m_ctrlTextEdit.GetGraphMax();
		DWORD	dwFlags			= m_ctrlTextEdit.GetGraphFlags();
		int		nParams			= m_ctrlTextEdit.GetGraphParams();

		CGraphSettingsDlg dlg;

		dlg.m_strSource			=  strSource;

		if (nParams > 1)
			dlg.m_nGraphTemplate = 1;

		dlg.m_nGraphWidth		= dwWidth;
		dlg.m_nGraphHeight		= dwHeight;
		dlg.m_nGraphMargin		= dwMargin;
		dlg.m_fltGraphMin		= fltMin;
		dlg.m_fltGraphMax		= fltMax;
		dlg.m_dwGraphFlags		= dwFlags;

		dlg.SetOverlay(m_lpOverlay);

		if (IDOK == dlg.DoModal())
		{
			if (dlg.m_bResizeToExtent)
			{
				m_bResizeToContent = FALSE;

				UpdateData(FALSE);
			}

			m_ctrlTextEdit.SetSel(m_ctrlTextEdit.GetObjFrom(), m_ctrlTextEdit.GetObjTo());
			m_ctrlTextEdit.ReplaceSel(dlg.m_strText);
			m_ctrlTextEdit.SetFocus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CLayerSettingsDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = (NMHDR*)lParam;

	if (pNMHDR->idFrom == IDC_TEXT_EDIT)
	{
		if (m_bResetInitialSelection)
		{
			m_bResetInitialSelection = FALSE;

			m_ctrlTextEdit.SetSel(0,0);
		}

		if (pNMHDR->code == EN_SELCHANGE)
			UpdateObjectControls();
	}

	return CDialog::OnNotify(wParam, lParam, pResult);
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::UpdateObjectControls()
{
	int nObj = m_ctrlTextEdit.GetObj();

	GetDlgItem(IDC_EDIT_EMBEDDED_OBJECT)->EnableWindow(nObj != OBJ_UNKNOWN);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CLayerSettingsDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_EDIT_EMBEDDED_OBJECT)
		OnBnClickedEditEmbeddedObject();

	if (message == UM_BROWSE_MACRO)
	{
		long nFrom, nTo;
		m_ctrlTextEdit.GetSel(nFrom, nTo);
		m_ctrlTextEdit.SetObj(nFrom - 2, nFrom);

		OnBrowseMacro(wParam);
	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnSelectMacro(UINT nID)
{
	COverlayMacroList* lpMacroList = m_lpOverlay->GetMacroList();

	if (lpMacroList)
	{	
		POSITION pos = lpMacroList->FindIndex((nID - ID_SELECTMACRO) * 2);

		if (pos)
		{
			CString strMacroName = lpMacroList->GetNext(pos);

			m_ctrlTextEdit.SetSel(m_ctrlTextEdit.GetObjFrom(), m_ctrlTextEdit.GetObjTo());
			m_ctrlTextEdit.ReplaceSel(strMacroName);
			m_ctrlTextEdit.UpdateContextHighlighting();
			m_ctrlTextEdit.SetFocus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnSelectTag(UINT nID)
{
	CStringList* lpTagList = m_lpOverlay->GetTagList();

	if (lpTagList)
	{	
		POSITION pos = lpTagList->FindIndex((nID - ID_SELECTTAG) * 2);

		if (pos)
		{
			CString strTagName = lpTagList->GetNext(pos);

			m_ctrlTextEdit.SetSel(m_ctrlTextEdit.GetObjFrom(), m_ctrlTextEdit.GetObjTo());
			m_ctrlTextEdit.ReplaceSel(strTagName);
			m_ctrlTextEdit.UpdateContextHighlighting();
			m_ctrlTextEdit.SetFocus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin0()
{
	m_nExtentOrigin = 0;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin1()
{
	m_nExtentOrigin = 1;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin2()
{
	m_nExtentOrigin = 2;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin3()
{
	m_nExtentOrigin = 3;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin4()
{
	m_nExtentOrigin = 4;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin5()
{
	m_nExtentOrigin = 5;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin6()
{
	m_nExtentOrigin = 6;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin7()
{
	m_nExtentOrigin = 7;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedExtentOrigin8()
{
	m_nExtentOrigin = 8;

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::UpdateExtentControls()
{
	CString strMark = "+";

	GetDlgItem(IDC_EXTENT_ORIGIN_0)->SetWindowText((m_nExtentOrigin == 0) ? strMark : "");
	GetDlgItem(IDC_EXTENT_ORIGIN_1)->SetWindowText((m_nExtentOrigin == 1) ? strMark : "");
	GetDlgItem(IDC_EXTENT_ORIGIN_2)->SetWindowText((m_nExtentOrigin == 2) ? strMark : "");
	GetDlgItem(IDC_EXTENT_ORIGIN_3)->SetWindowText((m_nExtentOrigin == 3) ? strMark : "");
	GetDlgItem(IDC_EXTENT_ORIGIN_4)->SetWindowText((m_nExtentOrigin == 4) ? strMark : "");
	GetDlgItem(IDC_EXTENT_ORIGIN_5)->SetWindowText((m_nExtentOrigin == 5) ? strMark : "");
	GetDlgItem(IDC_EXTENT_ORIGIN_6)->SetWindowText((m_nExtentOrigin == 6) ? strMark : "");
	GetDlgItem(IDC_EXTENT_ORIGIN_7)->SetWindowText((m_nExtentOrigin == 7) ? strMark : "");
	GetDlgItem(IDC_EXTENT_ORIGIN_8)->SetWindowText((m_nExtentOrigin == 8) ? strMark : "");

	BOOL bEnable = !m_bResizeToContent;

	GetDlgItem(IDC_EXTENT_ORIGIN_CAPTION	)->EnableWindow(bEnable);

	GetDlgItem(IDC_EXTENT_ORIGIN_0			)->EnableWindow(bEnable);
	GetDlgItem(IDC_EXTENT_ORIGIN_1			)->EnableWindow(bEnable);
	GetDlgItem(IDC_EXTENT_ORIGIN_2			)->EnableWindow(bEnable);
	GetDlgItem(IDC_EXTENT_ORIGIN_3			)->EnableWindow(bEnable);
	GetDlgItem(IDC_EXTENT_ORIGIN_4			)->EnableWindow(bEnable);
	GetDlgItem(IDC_EXTENT_ORIGIN_5			)->EnableWindow(bEnable);
	GetDlgItem(IDC_EXTENT_ORIGIN_6			)->EnableWindow(bEnable);
	GetDlgItem(IDC_EXTENT_ORIGIN_7			)->EnableWindow(bEnable);
	GetDlgItem(IDC_EXTENT_ORIGIN_8			)->EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedResizeToContentCheck()
{
	UpdateData(TRUE);

	UpdateExtentControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnDeltaposRefreshPeriodSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	LONG dwWeight = 1000;

	if (m_nRefreshPeriod - pNMUpDown->iDelta < 100)
		dwWeight = 10;
	else
		if (m_nRefreshPeriod - pNMUpDown->iDelta < 1000)
			dwWeight = 100;

	m_nRefreshPeriod -= pNMUpDown->iDelta * dwWeight;

	if (m_nRefreshPeriod < 0)
		m_nRefreshPeriod = 0;
	if (m_nRefreshPeriod > 60000)
		m_nRefreshPeriod = 60000;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky0()
{
	m_nPosSticky = 0;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky1()
{
	m_nPosSticky = 1;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky2()
{
	m_nPosSticky = 2;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky3()
{
	m_nPosSticky = 3;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky4()
{
	m_nPosSticky = 4;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky5()
{
	m_nPosSticky = 5;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky6()
{
	m_nPosSticky = 6;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky7()
{
	m_nPosSticky = 7;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosSticky8()
{
	m_nPosSticky = 8;

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedPosStickyCheck()
{
	UpdateData(TRUE);

	if (m_bPosSticky)
	{
		if (m_nPosSticky < 0)
			m_nPosSticky = 0;
	}

	UpdateStickyControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::UpdateStickyControls()
{
	CString strMark = "+";

	GetDlgItem(IDC_POS_STICKY_0)->SetWindowText((m_nPosSticky == 0) ? strMark : "");
	GetDlgItem(IDC_POS_STICKY_1)->SetWindowText((m_nPosSticky == 1) ? strMark : "");
	GetDlgItem(IDC_POS_STICKY_2)->SetWindowText((m_nPosSticky == 2) ? strMark : "");
	GetDlgItem(IDC_POS_STICKY_3)->SetWindowText((m_nPosSticky == 3) ? strMark : "");
	GetDlgItem(IDC_POS_STICKY_4)->SetWindowText((m_nPosSticky == 4) ? strMark : "");
	GetDlgItem(IDC_POS_STICKY_5)->SetWindowText((m_nPosSticky == 5) ? strMark : "");
	GetDlgItem(IDC_POS_STICKY_6)->SetWindowText((m_nPosSticky == 6) ? strMark : "");
	GetDlgItem(IDC_POS_STICKY_7)->SetWindowText((m_nPosSticky == 7) ? strMark : "");
	GetDlgItem(IDC_POS_STICKY_8)->SetWindowText((m_nPosSticky == 8) ? strMark : "");

	BOOL bEnable = m_bPosSticky;

	GetDlgItem(IDC_POS_STICKY_0			)->EnableWindow(bEnable);
	GetDlgItem(IDC_POS_STICKY_1			)->EnableWindow(bEnable);
	GetDlgItem(IDC_POS_STICKY_2			)->EnableWindow(bEnable);
	GetDlgItem(IDC_POS_STICKY_3			)->EnableWindow(bEnable);
	GetDlgItem(IDC_POS_STICKY_4			)->EnableWindow(bEnable);
	GetDlgItem(IDC_POS_STICKY_5			)->EnableWindow(bEnable);
	GetDlgItem(IDC_POS_STICKY_6			)->EnableWindow(bEnable);
	GetDlgItem(IDC_POS_STICKY_7			)->EnableWindow(bEnable);
	GetDlgItem(IDC_POS_STICKY_8			)->EnableWindow(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedApply()
{
	if (m_lpEditorWnd)
	{
		PreApply();

		m_lpEditorWnd->OnApplyLayerSettings(m_lpLayer, this);
	}
}
/////////////////////////////////////////////////////////////////////////////
DWORD CLayerSettingsDlg::GetDynamicColor(CString strColor, CDynamicColor& color)
{
	if (strlen(strColor))
	{
		color.Scan(strColor);
		color.Update(m_lpOverlay);
	}
	else
		color.SetStatic(DEFAULT_COLOR);

	return color.m_dwColor;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CLayerSettingsDlg::GetDynamicTextColor(CDynamicColor& color)
{
	return GetDynamicColor(m_strTextColor, color);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CLayerSettingsDlg::GetDynamicBgndColor(CDynamicColor& color)
{
	return GetDynamicColor(m_strBgndColor, color);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CLayerSettingsDlg::GetDynamicBorderColor(CDynamicColor& color)
{
	return GetDynamicColor(m_strBorderColor, color);
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnBnClickedBorderColorCheck()
{
	UpdateData(TRUE);

	if (m_bBorder && m_strBorderColor.IsEmpty())
		m_strBorderColor.Format("%X", DEFAULT_TEXT_COLOR);

	if (m_bBorder && (m_nBorderSize == 0))
	{
		m_nBorderSize = 1;

		UpdateData(FALSE);
	}

	if (!m_bBorder && (m_nBorderSize != 0))
	{
		m_nBorderSize = 0;

		UpdateData(FALSE);
	}

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CLayerSettingsDlg::OnDeltaposBorderSizeSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nSize = m_nBorderSize - pNMUpDown->iDelta;

	if (nSize < -500)
		nSize = -500;

	if (nSize > 500)
		nSize = 500;

	m_nBorderSize = nSize;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
