// DynamicColorPickerDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "DynamicColorPickerDlg.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
// CDynamicColorPickerDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDynamicColorPickerDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
#define DEFAULT_RANGE_COLOR		0xFFFF8000
/////////////////////////////////////////////////////////////////////////////
CDynamicColorPickerDlg::CDynamicColorPickerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDynamicColorPickerDlg::IDD, pParent)
{
	m_lpOverlay					= NULL;

	m_dwRange0Color				= DEFAULT_RANGE_COLOR;
	m_dwRange1Color				= DEFAULT_RANGE_COLOR;
	m_dwRange2Color				= DEFAULT_RANGE_COLOR;
	m_dwRange3Color				= DEFAULT_RANGE_COLOR;
	m_dwRange4Color				= DEFAULT_RANGE_COLOR;

	m_strRange0Min				= "";
	m_strRange0Max				= "";
	m_strRange1Min				= "";
	m_strRange1Max				= "";
	m_strRange2Min				= "";
	m_strRange2Max				= "";
	m_strRange3Min				= "";
	m_strRange3Max				= "";
	m_strRange4Min				= "";
	m_strRange4Max				= "";

	m_hBrushRange0ColorPreview	= NULL;
	m_hBrushRange1ColorPreview	= NULL;
	m_hBrushRange2ColorPreview	= NULL;
	m_hBrushRange3ColorPreview	= NULL;
	m_hBrushRange4ColorPreview	= NULL;

	m_nDynamicColorPreviewPos	= 0;

	m_bBlendColors				= FALSE;
	m_bEmbedFormula				= FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CDynamicColorPickerDlg::~CDynamicColorPickerDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_SOURCES_COMBO, m_ctrlDataSourcesCombo);
	DDX_Text(pDX, IDC_RANGE_0_MIN_EDIT, m_strRange0Min);
	DDX_Text(pDX, IDC_RANGE_0_MAX_EDIT, m_strRange0Max);
	DDX_Text(pDX, IDC_RANGE_1_MIN_EDIT, m_strRange1Min);
	DDX_Text(pDX, IDC_RANGE_1_MAX_EDIT, m_strRange1Max);
	DDX_Text(pDX, IDC_RANGE_2_MIN_EDIT, m_strRange2Min);
	DDX_Text(pDX, IDC_RANGE_2_MAX_EDIT, m_strRange2Max);
	DDX_Text(pDX, IDC_RANGE_3_MIN_EDIT, m_strRange3Min);
	DDX_Text(pDX, IDC_RANGE_3_MAX_EDIT, m_strRange3Max);
	DDX_Text(pDX, IDC_RANGE_4_MIN_EDIT, m_strRange4Min);
	DDX_Text(pDX, IDC_RANGE_4_MAX_EDIT, m_strRange4Max);
	DDX_Control(pDX, IDC_DYNAMIC_COLOR_SCROLLBAR, m_ctrlDynamicColorPreviewScrollbar);
	DDX_Check(pDX, IDC_BLEND_COLORS_CHECK, m_bBlendColors);
	DDX_Check(pDX, IDC_EMBED_FORMULA_CHECK, m_bEmbedFormula);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDynamicColorPickerDlg, CDialog)
	ON_BN_CLICKED(IDC_LESS_BUTTON, &CDynamicColorPickerDlg::OnBnClickedLessButton)
	ON_BN_CLICKED(IDOK, &CDynamicColorPickerDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_RANGE_0_MIN_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange0MinEdit)
	ON_EN_CHANGE(IDC_RANGE_0_MAX_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange0MaxEdit)
	ON_EN_CHANGE(IDC_RANGE_1_MIN_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange1MinEdit)
	ON_EN_CHANGE(IDC_RANGE_1_MAX_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange1MaxEdit)
	ON_EN_CHANGE(IDC_RANGE_2_MIN_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange2MinEdit)
	ON_EN_CHANGE(IDC_RANGE_2_MAX_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange2MaxEdit)
	ON_EN_CHANGE(IDC_RANGE_3_MIN_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange3MinEdit)
	ON_EN_CHANGE(IDC_RANGE_3_MAX_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange3MaxEdit)
	ON_EN_CHANGE(IDC_RANGE_4_MIN_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange4MinEdit)
	ON_EN_CHANGE(IDC_RANGE_4_MAX_EDIT, &CDynamicColorPickerDlg::OnEnChangeRange4MaxEdit)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_RANGE_0_COLOR_PREVIEW, &CDynamicColorPickerDlg::OnStnClickedRange0ColorPreview)
	ON_STN_CLICKED(IDC_RANGE_1_COLOR_PREVIEW, &CDynamicColorPickerDlg::OnStnClickedRange1ColorPreview)
	ON_STN_CLICKED(IDC_RANGE_2_COLOR_PREVIEW, &CDynamicColorPickerDlg::OnStnClickedRange2ColorPreview)
	ON_STN_CLICKED(IDC_RANGE_3_COLOR_PREVIEW, &CDynamicColorPickerDlg::OnStnClickedRange3ColorPreview)
	ON_STN_CLICKED(IDC_RANGE_4_COLOR_PREVIEW, &CDynamicColorPickerDlg::OnStnClickedRange4ColorPreview)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BLEND_COLORS_CHECK, &CDynamicColorPickerDlg::OnBnClickedBlendColorsCheck)
	ON_CBN_SELCHANGE(IDC_DATA_SOURCES_COMBO, &CDynamicColorPickerDlg::OnCbnSelchangeDataSourcesCombo)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDynamicColorPickerDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnBnClickedLessButton()
{
	EndDialog(IDCONTINUE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDynamicColorPickerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);

	CWnd* pPlaceholder = GetDlgItem(IDC_DYNAMIC_COLOR_PREVIEW_PLACEHOLDER);

	if (pPlaceholder)
	{
		CRect rc;
		pPlaceholder->GetWindowRect(&rc);
		ScreenToClient(&rc);

		m_dynamicColorPreviewWnd.Create(AfxRegisterWndClass(CS_OWNDC|CS_DBLCLKS,::LoadCursor(NULL,IDC_ARROW)), "", WS_CHILD|WS_VISIBLE, rc, this, IDC_DYNAMIC_COLOR_PREVIEW);
	}

	pPlaceholder = GetDlgItem(IDC_RECENT_DYNAMIC_COLORS_PLACEHOLDER);

	if (pPlaceholder)
	{
		CRect rect; 
		pPlaceholder->GetWindowRect(&rect);
		ScreenToClient(&rect);

		m_recentDynamicColorsWnd.SetRecentColors(m_recentColors);

		if (!m_recentDynamicColorsWnd.CreateEx(0, AfxRegisterWndClass(CS_OWNDC|CS_DBLCLKS, ::LoadCursor(NULL,IDC_ARROW)), NULL, WS_CHILD|WS_CLIPSIBLINGS|WS_VISIBLE, rect, this, 0))
			return FALSE;
	}

	if (m_lpOverlay)
	{
		InitDataSourcesList();
		InitRangeControls(&m_color);
		UpdateRangeControls();
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::SetRecentColors(LPDYNAMIC_COLOR_DESC lpRecentColors)
{
	CopyMemory(&m_recentColors, lpRecentColors, sizeof(m_recentColors));
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	VarsToColor(&m_color);
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::InitDataSourcesList()
{
	m_ctrlDataSourcesCombo.ResetContent();

	if (m_lpOverlay)
	{
		COverlayDataSourcesList* lpInternalDataSources = m_lpOverlay->GetInternalDataSources();

		POSITION pos = lpInternalDataSources->GetHeadPosition();

		while (pos)
		{
			COverlayDataSource* lpSource = lpInternalDataSources->GetNext(pos);

			CString strSource = lpSource->GetName();

			if (m_ctrlDataSourcesCombo.FindString(-1, strSource) == -1)
			{
				int iItem = m_ctrlDataSourcesCombo.AddString(strSource);

				if (!strcmp(m_color.m_strSource, strSource))
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

				if (!strcmp(m_color.m_strSource, strSource))
					m_ctrlDataSourcesCombo.SetCurSel(iItem);
			}
		}

		if (m_ctrlDataSourcesCombo.GetCurSel() == -1)
			m_ctrlDataSourcesCombo.SetCurSel(0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::ColorToVars(CDynamicColor* lpColor)
{
	DWORD* lpRangeColorVars[] = {	&m_dwRange0Color,
									&m_dwRange1Color,
									&m_dwRange2Color,
									&m_dwRange3Color,
									&m_dwRange4Color };

	CString* lpRangeMinVars[] = {	&m_strRange0Min,
									&m_strRange1Min,
									&m_strRange2Min,
									&m_strRange3Min,
									&m_strRange4Min	};

	CString* lpRangeMaxVars[] = {	&m_strRange0Max,
									&m_strRange1Max,
									&m_strRange2Max,
									&m_strRange3Max,
									&m_strRange4Max	};

	m_bBlendColors	= (lpColor->m_dwFlags & DYNAMIC_COLOR_FLAGS_BLEND);
	m_bEmbedFormula = (lpColor->m_dwFlags & DYNAMIC_COLOR_FLAGS_EMBED);

	for (DWORD dwRange=0; dwRange<5; dwRange++)
	{
		if (dwRange < lpColor->m_desc.dwCount)
		{
			*lpRangeColorVars[dwRange]	= lpColor->m_desc.ranges[dwRange].dwColor;
			*lpRangeMinVars[dwRange]	= lpColor->FormatRangeMin(dwRange);
			*lpRangeMaxVars[dwRange]	= lpColor->FormatRangeMax(dwRange);
		}
		else
		{
			*lpRangeColorVars[dwRange]	= DEFAULT_RANGE_COLOR;
			*lpRangeMinVars[dwRange]	= "";
			*lpRangeMaxVars[dwRange]	= "";
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::VarsToColor(CDynamicColor* lpColor)
{
	lpColor->Destroy();

	int iSelection = m_ctrlDataSourcesCombo.GetCurSel();

	if (iSelection != -1)
	{
		CString strSource;
		m_ctrlDataSourcesCombo.GetLBText(iSelection, strSource);

		lpColor->m_strSource = strSource;
	}

	if (m_bBlendColors)
		lpColor->m_dwFlags |= DYNAMIC_COLOR_FLAGS_BLEND;
	if (m_bEmbedFormula)
		lpColor->m_dwFlags |= DYNAMIC_COLOR_FLAGS_EMBED;

	lpColor->AddRange(m_dwRange0Color, m_strRange0Min, m_strRange0Max);

	if (!m_strRange1Min.IsEmpty() || !m_strRange1Max.IsEmpty())
		lpColor->AddRange(m_dwRange1Color, m_strRange1Min, m_strRange1Max);

	if (!m_strRange2Min.IsEmpty() || !m_strRange2Max.IsEmpty())
		lpColor->AddRange(m_dwRange2Color, m_strRange2Min, m_strRange2Max);

	if (!m_strRange3Min.IsEmpty() || !m_strRange3Max.IsEmpty())
		lpColor->AddRange(m_dwRange3Color, m_strRange3Min, m_strRange3Max);

	if (!m_strRange4Min.IsEmpty() || !m_strRange4Max.IsEmpty())
		lpColor->AddRange(m_dwRange4Color, m_strRange4Min, m_strRange4Max);
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::InitRangeControls(CDynamicColor* lpColor)
{
	ColorToVars(lpColor);

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::UpdateRangeControls()
{
	BOOL bRange0 = !m_strRange0Min.IsEmpty() || !m_strRange0Max.IsEmpty();
	BOOL bRange1 = !m_strRange1Min.IsEmpty() || !m_strRange1Max.IsEmpty();
	BOOL bRange2 = !m_strRange2Min.IsEmpty() || !m_strRange2Max.IsEmpty();
	BOOL bRange3 = !m_strRange3Min.IsEmpty() || !m_strRange3Max.IsEmpty();

	if (!bRange0)
		bRange1 = FALSE;

	if (!bRange1)
		bRange2 = FALSE;

	if (!bRange2)
		bRange3 = FALSE;

	GetDlgItem(IDC_RANGE_1_MIN_EDIT		)->EnableWindow(bRange0);
	GetDlgItem(IDC_RANGE_1_MAX_EDIT		)->EnableWindow(bRange0);
	GetDlgItem(IDC_RANGE_1_COLOR_PREVIEW)->EnableWindow(bRange0);

	GetDlgItem(IDC_RANGE_2_MIN_EDIT		)->EnableWindow(bRange1);
	GetDlgItem(IDC_RANGE_2_MAX_EDIT		)->EnableWindow(bRange1);
	GetDlgItem(IDC_RANGE_2_COLOR_PREVIEW)->EnableWindow(bRange1);

	GetDlgItem(IDC_RANGE_3_MIN_EDIT		)->EnableWindow(bRange2);
	GetDlgItem(IDC_RANGE_3_MAX_EDIT		)->EnableWindow(bRange2);
	GetDlgItem(IDC_RANGE_3_COLOR_PREVIEW)->EnableWindow(bRange2);

	GetDlgItem(IDC_RANGE_4_MIN_EDIT		)->EnableWindow(bRange3);
	GetDlgItem(IDC_RANGE_4_MAX_EDIT		)->EnableWindow(bRange3);
	GetDlgItem(IDC_RANGE_4_COLOR_PREVIEW)->EnableWindow(bRange3);

	if (!bRange0)
	{
		m_strRange1Min = "";
		m_strRange1Max = "";
	}
		
	if (!bRange1)
	{
		m_strRange2Min = "";
		m_strRange2Max = "";
	}

	if (!bRange2)
	{
		m_strRange3Min = "";
		m_strRange3Max = "";
	}

	if (!bRange3)
	{
		m_strRange4Min = "";
		m_strRange4Max = "";
	}

	UpdateData(FALSE);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::UpdateColorControls()
{
	if (m_hBrushRange0ColorPreview)
		DeleteObject(m_hBrushRange0ColorPreview);

	m_hBrushRange0ColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(m_dwRange0Color) & 0xFFFFFF);

	GetDlgItem(IDC_RANGE_0_COLOR_PREVIEW)->RedrawWindow();

	if (m_hBrushRange1ColorPreview)
		DeleteObject(m_hBrushRange1ColorPreview);

	m_hBrushRange1ColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(m_dwRange1Color) & 0xFFFFFF);

	GetDlgItem(IDC_RANGE_1_COLOR_PREVIEW)->RedrawWindow();

	if (m_hBrushRange2ColorPreview)
		DeleteObject(m_hBrushRange2ColorPreview);

	m_hBrushRange2ColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(m_dwRange2Color) & 0xFFFFFF);

	GetDlgItem(IDC_RANGE_2_COLOR_PREVIEW)->RedrawWindow();

	if (m_hBrushRange3ColorPreview)
		DeleteObject(m_hBrushRange3ColorPreview);

	m_hBrushRange3ColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(m_dwRange3Color) & 0xFFFFFF);

	GetDlgItem(IDC_RANGE_3_COLOR_PREVIEW)->RedrawWindow();

	if (m_hBrushRange4ColorPreview)
		DeleteObject(m_hBrushRange4ColorPreview);

	m_hBrushRange4ColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(m_dwRange4Color) & 0xFFFFFF);

	GetDlgItem(IDC_RANGE_4_COLOR_PREVIEW)->RedrawWindow();

	UpdatePreviewControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange0MinEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange0MaxEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange1MinEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange1MaxEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange2MinEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange2MaxEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange3MinEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange3MaxEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange4MinEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnEnChangeRange4MaxEdit()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnDestroy()
{
	if (m_hBrushRange0ColorPreview)
		DeleteObject(m_hBrushRange0ColorPreview);

	if (m_hBrushRange1ColorPreview)
		DeleteObject(m_hBrushRange1ColorPreview);

	if (m_hBrushRange2ColorPreview)
		DeleteObject(m_hBrushRange2ColorPreview);

	if (m_hBrushRange3ColorPreview)
		DeleteObject(m_hBrushRange3ColorPreview);

	if (m_hBrushRange4ColorPreview)
		DeleteObject(m_hBrushRange4ColorPreview);

	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH CDynamicColorPickerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT nID = pWnd->GetDlgCtrlID();

	if (nID == IDC_RANGE_0_COLOR_PREVIEW)
	{
		if (GetDlgItem(IDC_RANGE_0_COLOR_PREVIEW)->IsWindowEnabled())
			return m_hBrushRange0ColorPreview;
	}

	if (nID == IDC_RANGE_1_COLOR_PREVIEW)
	{
		if (GetDlgItem(IDC_RANGE_1_COLOR_PREVIEW)->IsWindowEnabled() && (!m_strRange1Min.IsEmpty() || !m_strRange1Max.IsEmpty()))
			return m_hBrushRange1ColorPreview;
	}

	if (nID == IDC_RANGE_2_COLOR_PREVIEW)
	{
		if (GetDlgItem(IDC_RANGE_2_COLOR_PREVIEW)->IsWindowEnabled() && (!m_strRange2Min.IsEmpty() || !m_strRange2Max.IsEmpty()))
			return m_hBrushRange2ColorPreview;
	}

	if (nID == IDC_RANGE_3_COLOR_PREVIEW)
	{
		if (GetDlgItem(IDC_RANGE_3_COLOR_PREVIEW)->IsWindowEnabled() && (!m_strRange3Min.IsEmpty() || !m_strRange3Max.IsEmpty()))
			return m_hBrushRange3ColorPreview;
	}

	if (nID == IDC_RANGE_4_COLOR_PREVIEW)
	{
		if (GetDlgItem(IDC_RANGE_4_COLOR_PREVIEW)->IsWindowEnabled() && (!m_strRange4Min.IsEmpty() || !m_strRange4Max.IsEmpty()))
			return m_hBrushRange4ColorPreview;
	}

	return hbr;
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnStnClickedRange0ColorPreview()
{
	DWORD dwRecentColors[32] = { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors);

	m_dwRange0Color = PickColor(m_hWnd, m_dwRange0Color, dwRecentColors);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnStnClickedRange1ColorPreview()
{
	DWORD dwRecentColors[32] = { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors);

	m_dwRange1Color = PickColor(m_hWnd, m_dwRange1Color, dwRecentColors);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnStnClickedRange2ColorPreview()
{
	DWORD dwRecentColors[32] = { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors);

	m_dwRange2Color = PickColor(m_hWnd, m_dwRange2Color, dwRecentColors);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnStnClickedRange3ColorPreview()
{
	DWORD dwRecentColors[32] = { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors);

	m_dwRange3Color = PickColor(m_hWnd, m_dwRange3Color, dwRecentColors);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnStnClickedRange4ColorPreview()
{
	DWORD dwRecentColors[32] = { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors);

	m_dwRange4Color = PickColor(m_hWnd, m_dwRange4Color, dwRecentColors);

	UpdateColorControls();
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::UpdatePreviewControls()
{
	CDynamicColor color;
	VarsToColor(&color);

	float fltMin;
	float fltMax;

	color.GetRange(fltMin, fltMax);

	if ((fltMin != FLT_MAX) &&
		(fltMax != FLT_MAX))
	{
		GetDlgItem(IDC_DYNAMIC_COLOR_PREVIEW_CAPTION	)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DYNAMIC_COLOR_PREVIEW_TEXT		)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DYNAMIC_COLOR_PREVIEW			)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DYNAMIC_COLOR_SCROLLBAR			)->ShowWindow(SW_SHOW);

		fltMin = fltMin - 1;
		fltMax = fltMax + 1;

		m_ctrlDynamicColorPreviewScrollbar.SetScrollRange(0, 100);
		m_ctrlDynamicColorPreviewScrollbar.SetScrollPos(m_nDynamicColorPreviewPos);

		float fltValue = fltMin + (fltMax - fltMin) * m_nDynamicColorPreviewPos / 100;

		color.Update(fltValue);
		m_dynamicColorPreviewWnd.SetColor(color.m_dwColor);

		COverlayDataSource* lpDataSource = m_lpOverlay->FindDataSource(color.m_strSource);

		if (lpDataSource)
		{
			CString strFormat	= lpDataSource->GetFormat();
			CString strUnits	= lpDataSource->GetUnits();

			CString strValue;
			if (strlen(strFormat))
			{
				if (lpDataSource->IsIntFormat())
					strValue.Format(strFormat, (int)fltValue);
				else
					strValue.Format(strFormat, fltValue);
			}
			else
				strValue.Format("%.0f", fltValue);

			strValue += " ";
			strValue += strUnits;

			GetDlgItem(IDC_DYNAMIC_COLOR_PREVIEW_TEXT)->SetWindowText(strValue);
		}
	}
	else
	{
		GetDlgItem(IDC_DYNAMIC_COLOR_PREVIEW_CAPTION	)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DYNAMIC_COLOR_PREVIEW_TEXT		)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DYNAMIC_COLOR_PREVIEW			)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DYNAMIC_COLOR_SCROLLBAR			)->ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == &m_ctrlDynamicColorPreviewScrollbar)
	{
		if (nSBCode == SB_THUMBTRACK)
		{
			m_nDynamicColorPreviewPos = nPos;

			UpdatePreviewControls();
		}

		if (nSBCode == SB_LINELEFT)
		{
			m_nDynamicColorPreviewPos--;

			if (m_nDynamicColorPreviewPos < 0)
				m_nDynamicColorPreviewPos = 100;

			UpdatePreviewControls();
		}

		if (nSBCode == SB_LINERIGHT)
		{
			m_nDynamicColorPreviewPos++;

			if (m_nDynamicColorPreviewPos > 100)
				m_nDynamicColorPreviewPos = 0;

			UpdatePreviewControls();
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnBnClickedBlendColorsCheck()
{
	UpdateData(TRUE);

	UpdatePreviewControls();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CDynamicColorPickerDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_SELECT_RECENT_DYNAMIC_COLOR)
	{
		if (wParam < 32)
		{
			CDynamicColor color;
			color.m_desc = m_recentColors[wParam];

			if (color.GetRangeCount())
			{
				UpdateData(TRUE);

				if (m_bBlendColors)
					color.m_dwFlags |= DYNAMIC_COLOR_FLAGS_BLEND;
				if (m_bEmbedFormula)
					color.m_dwFlags |= DYNAMIC_COLOR_FLAGS_EMBED;

				InitRangeControls(&color);
				UpdateRangeControls();
				UpdatePreviewControls();
			}
		}
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColorPickerDlg::OnCbnSelchangeDataSourcesCombo()
{
	UpdateData(TRUE);

	UpdatePreviewControls();
}
/////////////////////////////////////////////////////////////////////////////
