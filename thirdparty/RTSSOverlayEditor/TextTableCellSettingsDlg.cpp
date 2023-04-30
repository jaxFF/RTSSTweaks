// TextTableCellSettingsDlg.cpp : implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "TextTableCellSettingsDlg.h"
//////////////////////////////////////////////////////////////////////
// CTextTableCellSettingsDlg dialog
//////////////////////////////////////////////////////////////////////
#define DEFAULT_TEXT_COLOR		0xFFFF8000
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CTextTableCellSettingsDlg, CDialog)
//////////////////////////////////////////////////////////////////////
CTextTableCellSettingsDlg::CTextTableCellSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTextTableCellSettingsDlg::IDD, pParent)
{
	m_lpOverlay					= NULL;
	m_lpLine					= NULL;
	m_lpCell					= NULL;
	m_hBrushLineColorPreview	= NULL;
	m_hBrushCellColorPreview	= NULL;

	m_strLineColor				= "";
	m_strCellColor				= "";

	m_strLineName				= "";
	m_bUseCustomLineColor		= FALSE;
	m_strCellSource				= "";
	m_strCellText				= "";
	m_bUseCustomCellColor		= FALSE;
}
//////////////////////////////////////////////////////////////////////
CTextTableCellSettingsDlg::~CTextTableCellSettingsDlg()
{
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LINE_NAME_EDIT, m_strLineName);
	DDX_Check(pDX, IDC_USE_CUSTOM_LINE_COLOR_CHECK, m_bUseCustomLineColor);
	DDX_Control(pDX, IDC_DATA_SOURCES_COMBO, m_ctrlDataSourcesCombo);
	DDX_Text(pDX, IDC_CELL_TEXT_EDIT, m_strCellText);
	DDX_Check(pDX, IDC_USE_CUSTOM_CELL_COLOR_CHECK, m_bUseCustomCellColor);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTextTableCellSettingsDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_LINE_COLOR_PREVIEW, &CTextTableCellSettingsDlg::OnStnClickedLineColorPreview)
	ON_STN_CLICKED(IDC_CELL_COLOR_PREVIEW, &CTextTableCellSettingsDlg::OnStnClickedCellColorPreview)
	ON_BN_CLICKED(IDC_USE_CUSTOM_LINE_COLOR_CHECK, &CTextTableCellSettingsDlg::OnBnClickedUseCustomLineColorCheck)
	ON_BN_CLICKED(IDC_USE_CUSTOM_CELL_COLOR_CHECK, &CTextTableCellSettingsDlg::OnBnClickedUseCustomCellColorCheck)
	ON_CBN_SELCHANGE(IDC_DATA_SOURCES_COMBO, &CTextTableCellSettingsDlg::OnCbnSelchangeDataSourcesCombo)
	ON_EN_CHANGE(IDC_CELL_TEXT_EDIT, &CTextTableCellSettingsDlg::OnEnChangeCellTextEdit)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// CTextTableCellSettingsDlg message handlers
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::SetLine(CTextTableLine* lpLine)
{
	m_lpLine = lpLine;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::SetCell(CTextTableCell* lpCell)
{
	m_lpCell = lpCell;
}
//////////////////////////////////////////////////////////////////////
BOOL CTextTableCellSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	if (m_lpOverlay && 
		m_lpLine	&&
		m_lpCell)
	{
		m_strLineName	= m_lpLine->m_strName;
		m_strCellSource	= m_lpCell->m_strSource;
		m_strCellText	= m_lpCell->m_strText;

		m_strLineColor	= m_lpLine->m_strColor;
		m_strCellColor	= m_lpCell->m_strColor;

		m_bUseCustomLineColor = !m_strLineColor.IsEmpty();
		m_bUseCustomCellColor = !m_strCellColor.IsEmpty();

		m_ctrlDataSourcesCombo.AddString(LocalizeStr("<no binding>"));

		COverlayDataSourcesList* lpInternalDataSources = m_lpOverlay->GetInternalDataSources();

		POSITION pos = lpInternalDataSources->GetHeadPosition();

		while (pos)
		{
			COverlayDataSource* lpSource = lpInternalDataSources->GetNext(pos);

			CString strSource = lpSource->GetName();

			if (m_ctrlDataSourcesCombo.FindString(-1, strSource) == -1)
			{
				int iItem = m_ctrlDataSourcesCombo.AddString(strSource);

				if (!strcmp(m_strCellSource, strSource))
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

				if (!strcmp(m_strCellSource, strSource))
					m_ctrlDataSourcesCombo.SetCurSel(iItem);
			}
		}

		if (m_ctrlDataSourcesCombo.GetCurSel() == -1)
			m_ctrlDataSourcesCombo.SetCurSel(m_strCellSource.IsEmpty() ? 0 : 1);

		UpdateColorControls();

		UpdateData(FALSE);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::OnDestroy()
{
	if (m_hBrushLineColorPreview)
		DeleteObject(m_hBrushLineColorPreview);

	if (m_hBrushCellColorPreview)
		DeleteObject(m_hBrushCellColorPreview);

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
HBRUSH CTextTableCellSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT nID = pWnd->GetDlgCtrlID();

	if (nID == IDC_LINE_COLOR_PREVIEW)
	{
		pDC->SetBkMode(TRANSPARENT);

		if (GetDlgItem(IDC_LINE_COLOR_PREVIEW)->IsWindowEnabled())
			return m_hBrushLineColorPreview;
	}

	if (nID == IDC_CELL_COLOR_PREVIEW)
	{
		pDC->SetBkMode(TRANSPARENT);

		if (GetDlgItem(IDC_CELL_COLOR_PREVIEW)->IsWindowEnabled())
			return m_hBrushCellColorPreview;
	}

	return hbr;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::UpdateColorControls()
{
	CDynamicColor dynamicLineColor;
	CDynamicColor dynamicCellColor;

	DWORD dwLineColor = GetDynamicLineColor(dynamicLineColor);
	DWORD dwCellColor = GetDynamicCellColor(dynamicCellColor);

	if (dwLineColor == DEFAULT_COLOR)
		dwLineColor = COverlayLayer::SwapRGB(GetSysColor(COLOR_BTNFACE));
	if (dwCellColor == DEFAULT_COLOR)
		dwCellColor = COverlayLayer::SwapRGB(GetSysColor(COLOR_BTNFACE));

	GetDlgItem(IDC_LINE_COLOR_PREVIEW)->SetWindowText((m_bUseCustomLineColor && dynamicLineColor.GetRangeCount()) ? "D" : "");
	GetDlgItem(IDC_CELL_COLOR_PREVIEW)->SetWindowText((m_bUseCustomCellColor && dynamicCellColor.GetRangeCount()) ? "D" : "");

	GetDlgItem(IDC_LINE_COLOR_PREVIEW)->EnableWindow(m_bUseCustomLineColor);
	GetDlgItem(IDC_CELL_COLOR_PREVIEW)->EnableWindow(m_bUseCustomCellColor);

	if (m_hBrushLineColorPreview)
		DeleteObject(m_hBrushLineColorPreview);

	m_hBrushLineColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(dwLineColor) & 0xFFFFFF);

	GetDlgItem(IDC_LINE_COLOR_PREVIEW)->RedrawWindow();

	if (m_hBrushCellColorPreview)
		DeleteObject(m_hBrushCellColorPreview);

	m_hBrushCellColorPreview = CreateSolidBrush(COverlayLayer::SwapRGB(dwCellColor) & 0xFFFFFF);

	GetDlgItem(IDC_CELL_COLOR_PREVIEW)->RedrawWindow();
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::OnStnClickedLineColorPreview()
{
	DWORD				dwRecentColors[32]		= { 0 };
	DYNAMIC_COLOR_DESC	recentDynamicColors[32]	= { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors, recentDynamicColors);

	m_strLineColor = PickDynamicColor(m_hWnd, m_strLineColor, dwRecentColors, recentDynamicColors, m_lpOverlay);

	UpdateColorControls();
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::OnStnClickedCellColorPreview()
{
	DWORD				dwRecentColors[32]		= { 0 };
	DYNAMIC_COLOR_DESC	recentDynamicColors[32]	= { 0 };

	if (m_lpOverlay)
		m_lpOverlay->GetRecentColors(dwRecentColors, recentDynamicColors);

	m_strCellColor = PickDynamicColor(m_hWnd, m_strCellColor, dwRecentColors, recentDynamicColors, m_lpOverlay);

	UpdateColorControls();
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::OnBnClickedUseCustomLineColorCheck()
{
	UpdateData(TRUE);

	if (m_bUseCustomLineColor && m_strLineColor.IsEmpty())
		m_strLineColor.Format("%X", DEFAULT_TEXT_COLOR);

	UpdateColorControls();
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::OnBnClickedUseCustomCellColorCheck()
{
	UpdateData(TRUE);

	if (m_bUseCustomCellColor && m_strCellColor.IsEmpty())
		m_strCellColor.Format("%X", DEFAULT_TEXT_COLOR);

	UpdateColorControls();
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::OnOK()
{
	if (m_lpOverlay && 
		m_lpLine	&&
		m_lpCell)
	{
		UpdateData(TRUE);

		m_lpLine->m_strName = m_strLineName;

		if (!m_bUseCustomLineColor)
			m_lpLine->m_strColor = "";
		else
			m_lpLine->m_strColor = m_strLineColor;

		if (!m_bUseCustomCellColor)
			m_lpCell->m_strColor = "";
		else
			m_lpCell->m_strColor = m_strCellColor;

		int iSelection = m_ctrlDataSourcesCombo.GetCurSel();

		if (iSelection != -1)
		{
			if (iSelection)
				m_ctrlDataSourcesCombo.GetLBText(iSelection, m_lpCell->m_strSource);
			else
				m_lpCell->m_strSource = "";
		}

		m_lpCell->m_strText		= m_strCellText;

	}

	CDialog::OnOK();
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::OnCbnSelchangeDataSourcesCombo()
{
	if (m_ctrlDataSourcesCombo.GetCurSel())
		m_strCellText = "";

	UpdateData(FALSE);
}
//////////////////////////////////////////////////////////////////////
void CTextTableCellSettingsDlg::OnEnChangeCellTextEdit()
{
	UpdateData(TRUE);

	m_ctrlDataSourcesCombo.SetCurSel(0);
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableCellSettingsDlg::GetDynamicColor(CString strColor, CDynamicColor& color)
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
//////////////////////////////////////////////////////////////////////
DWORD CTextTableCellSettingsDlg::GetDynamicLineColor(CDynamicColor& color)
{
	return GetDynamicColor(m_strLineColor, color);
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableCellSettingsDlg::GetDynamicCellColor(CDynamicColor& color)
{
	return GetDynamicColor(m_strCellColor, color);
}
//////////////////////////////////////////////////////////////////////
