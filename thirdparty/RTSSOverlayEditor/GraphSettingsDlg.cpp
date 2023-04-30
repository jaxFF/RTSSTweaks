// GraphSettingsDlg.cpp : implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "GraphSettingsDlg.h"
#include "HypertextExtension.h"
#include "OverlayDataSourceInternal.h"

#include "RTSSSharedMemory.h"
//////////////////////////////////////////////////////////////////////
// CGraphSettingsDlg dialog
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CGraphSettingsDlg, CDialog)
//////////////////////////////////////////////////////////////////////
CGraphSettingsDlg::CGraphSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGraphSettingsDlg::IDD, pParent)
{
	m_lpOverlay			= NULL;
	m_hBrush			= NULL;

	m_strSource			= "";
	m_strText			= "";

	m_nGraphTemplate	= 0;
	m_bResizeToExtent	= FALSE;
	m_nGraphWidth		= DEFAULT_GRAPH_WIDTH;
	m_nGraphHeight		= DEFAULT_GRAPH_HEIGHT;
	m_nGraphMargin		= DEFAULT_GRAPH_MARGIN;
	m_fltGraphMin		= DEFAULT_GRAPH_MIN;
	m_fltGraphMax		= DEFAULT_GRAPH_MAX;
	m_bGraphAutoscale	= FALSE;
	m_dwGraphFlags		= DEFAULT_GRAPH_FLAGS;

	m_strGraphMin		= "";
	m_strGraphMax		= "";

	m_nGraphStyle		= 0;
	m_nGraphBgnd		= 0;
	m_nGraphOrientation	= 0;
	m_nGraphDirection	= 0;
}
//////////////////////////////////////////////////////////////////////
CGraphSettingsDlg::~CGraphSettingsDlg()
{
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_SOURCES_COMBO, m_ctrlDataSourcesCombo);
	DDX_CBIndex(pDX, IDC_GRAPH_TEMPLATE_COMBO, m_nGraphTemplate);
	DDX_CBIndex(pDX, IDC_GRAPH_STYLE_COMBO, m_nGraphStyle);
	DDX_Text(pDX, IDC_GRAPH_WIDTH_EDIT, m_nGraphWidth);
	DDX_Text(pDX, IDC_GRAPH_HEIGHT_EDIT, m_nGraphHeight);
	DDX_Text(pDX, IDC_GRAPH_MARGIN_EDIT, m_nGraphMargin);
	DDX_Text(pDX, IDC_GRAPH_MIN_EDIT, m_strGraphMin);
	DDX_Text(pDX, IDC_GRAPH_MAX_EDIT, m_strGraphMax);
	DDX_CBIndex(pDX, IDC_GRAPH_BGND_COMBO, m_nGraphBgnd);
	DDX_CBIndex(pDX, IDC_GRAPH_ORIENTATION_COMBO, m_nGraphOrientation);
	DDX_CBIndex(pDX, IDC_GRAPH_DIRECTION_COMBO, m_nGraphDirection);
	DDX_Check(pDX, IDC_RESIZE_TO_EXTENT, m_bResizeToExtent);
	DDX_Check(pDX, IDC_GRAPH_AUTOSCALE_CHECK, m_bGraphAutoscale);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGraphSettingsDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CGraphSettingsDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_GRAPH_TEMPLATE_COMBO, &CGraphSettingsDlg::OnCbnSelchangeGraphTemplateCombo)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD_GRAPH_CHECK, &CGraphSettingsDlg::OnBnClickedAddGraphCheck)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GRAPH_WIDTH_SPIN, &CGraphSettingsDlg::OnDeltaposGraphWidthSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GRAPH_HEIGHT_SPIN, &CGraphSettingsDlg::OnDeltaposGraphHeightSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GRAPH_MARGIN_SPIN, &CGraphSettingsDlg::OnDeltaposGraphMarginSpin)
	ON_BN_CLICKED(IDC_RESIZE_TO_EXTENT, &CGraphSettingsDlg::OnBnClickedResizeToExtent)
	ON_BN_CLICKED(IDC_GRAPH_AUTOSCALE_CHECK, &CGraphSettingsDlg::OnBnClickedGraphAutoscaleCheck)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// CGraphSettingsDlg message handlers
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
//////////////////////////////////////////////////////////////////////
BOOL CGraphSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	if (m_lpOverlay)
	{
		if (!m_nGraphWidth && !m_nGraphHeight)
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

		m_strGraphMin		= COverlayDataSource::FormatFloat(m_fltGraphMin);
		m_strGraphMax		= COverlayDataSource::FormatFloat(m_fltGraphMax);
		m_bGraphAutoscale	= (m_dwGraphFlags & RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_AUTOSCALE	) ? TRUE : FALSE;

		if (m_dwGraphFlags & RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FILLED)
			m_nGraphStyle = 1;
		else
		if (m_dwGraphFlags & RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_BAR)
			m_nGraphStyle = 2;
		else
			m_nGraphStyle = 0;

		m_nGraphBgnd		= (m_dwGraphFlags & RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_BGND		) ? 1 : 0;
		m_nGraphOrientation	= (m_dwGraphFlags & RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_VERTICAL	) ? 1 : 0;
		m_nGraphDirection	= (m_dwGraphFlags & RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_MIRRORED	) ? 1 : 0;

		UpdateGraphControls();
		UpdateRangeControls();

		UpdateData(FALSE);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	int iSelection = m_ctrlDataSourcesCombo.GetCurSel();

	if (iSelection != -1)
	{
		if (m_bResizeToExtent)
		{
			m_nGraphWidth	= 0;
			m_nGraphHeight	= 0;

			UpdateData(FALSE);
		}

		m_ctrlDataSourcesCombo.GetLBText(iSelection, m_strSource);

		sscanf_s(m_strGraphMin, "%f", &m_fltGraphMin);
		sscanf_s(m_strGraphMax, "%f", &m_fltGraphMax);

		m_dwGraphFlags = 0;

		if (m_bGraphAutoscale)
			m_dwGraphFlags |= RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_AUTOSCALE;

		switch(m_nGraphStyle)
		{
		case 1:
			m_dwGraphFlags |= RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FILLED;
			break;
		case 2:
			m_dwGraphFlags |= RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_BAR;
			break;
		}

		if (m_nGraphBgnd)
			m_dwGraphFlags |= RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_BGND;

		if (m_nGraphOrientation)
			m_dwGraphFlags |= RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_VERTICAL;

		if (m_nGraphDirection)
			m_dwGraphFlags |= RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_MIRRORED;

		CHypertextExtension extension;

		m_strText = extension.FormatGraphTag(m_nGraphTemplate ? TRUE : FALSE, m_strSource, m_nGraphWidth, m_nGraphHeight, m_nGraphMargin, m_fltGraphMin, m_fltGraphMax, m_dwGraphFlags);
	}

	OnOK();
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::UpdateRangeControls()
{
	if (m_bGraphAutoscale)
	{
		GetDlgItem(IDC_GRAPH_MIN_CAPTION		)->SetWindowText(LocalizeStr("Range minimum, in data source units"));
		GetDlgItem(IDC_GRAPH_MAX_CAPTION		)->SetWindowText(LocalizeStr("Range maximum, in data source units"));
	}
	else
	{
		GetDlgItem(IDC_GRAPH_MIN_CAPTION		)->SetWindowText(LocalizeStr("Graph minimum, in data source units"));
		GetDlgItem(IDC_GRAPH_MAX_CAPTION		)->SetWindowText(LocalizeStr("Graph maximum, in data source units"));
	}
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::UpdateGraphControls()
{
	BOOL bEnable = (m_nGraphTemplate != 0);

	GetDlgItem(IDC_RESIZE_TO_EXTENT			)->EnableWindow(bEnable);
	
	GetDlgItem(IDC_GRAPH_WIDTH_CAPTION		)->EnableWindow(bEnable && !m_bResizeToExtent);
	GetDlgItem(IDC_GRAPH_WIDTH_EDIT			)->EnableWindow(bEnable && !m_bResizeToExtent);

	GetDlgItem(IDC_GRAPH_HEIGHT_CAPTION		)->EnableWindow(bEnable && !m_bResizeToExtent);
	GetDlgItem(IDC_GRAPH_HEIGHT_EDIT		)->EnableWindow(bEnable && !m_bResizeToExtent);

	GetDlgItem(IDC_GRAPH_MARGIN_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_GRAPH_MARGIN_EDIT		)->EnableWindow(bEnable);

	GetDlgItem(IDC_GRAPH_MIN_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_GRAPH_MIN_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_GRAPH_MAX_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_GRAPH_MAX_EDIT			)->EnableWindow(bEnable);

	GetDlgItem(IDC_GRAPH_AUTOSCALE_CHECK	)->EnableWindow(bEnable);

	GetDlgItem(IDC_GRAPH_STYLE_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_GRAPH_STYLE_COMBO		)->EnableWindow(bEnable);

	GetDlgItem(IDC_GRAPH_BGND_CAPTION		)->EnableWindow(bEnable);
	GetDlgItem(IDC_GRAPH_BGND_COMBO			)->EnableWindow(bEnable);

	GetDlgItem(IDC_GRAPH_ORIENTATION_CAPTION)->EnableWindow(bEnable);
	GetDlgItem(IDC_GRAPH_ORIENTATION_COMBO	)->EnableWindow(bEnable);

	GetDlgItem(IDC_GRAPH_DIRECTION_CAPTION	)->EnableWindow(bEnable);
	GetDlgItem(IDC_GRAPH_DIRECTION_COMBO	)->EnableWindow(bEnable);
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnCbnSelchangeGraphTemplateCombo()
{
	UpdateData(TRUE);

	UpdateGraphControls();
}
//////////////////////////////////////////////////////////////////////
HBRUSH CGraphSettingsDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	COLORREF clrBk		= g_dwHeaderBgndColor;
	COLORREF clrText	= g_dwHeaderTextColor;

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT nID = pWnd->GetDlgCtrlID();

	if (nID == IDC_GRAPH_TEMPLATE_PROPERTIES_HEADER)
	{
		if (!m_hBrush)
 			m_hBrush = CreateSolidBrush(clrBk);

		pDC->SetBkColor(clrBk);
		pDC->SetTextColor(clrText);

		return m_hBrush;
	}

	return hbr;
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnDestroy()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);

		m_hBrush = NULL;
	}

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnBnClickedAddGraphCheck()
{
	UpdateData(TRUE);

	UpdateGraphControls();
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnDeltaposGraphWidthSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nGraphWidth = (m_nGraphWidth < 0) ? (m_nGraphWidth + pNMUpDown->iDelta) : (m_nGraphWidth - pNMUpDown->iDelta);

	if (nGraphWidth < -5000)
		nGraphWidth = -5000;

	if (nGraphWidth > 5000)
		nGraphWidth = 5000;

	m_nGraphWidth = nGraphWidth;

	UpdateData(FALSE);

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnDeltaposGraphHeightSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nGraphHeight = (m_nGraphHeight < 0) ? (m_nGraphHeight + pNMUpDown->iDelta) : (m_nGraphHeight - pNMUpDown->iDelta);

	if (nGraphHeight < -5000)
		nGraphHeight = -5000;

	if (nGraphHeight > 5000)
		nGraphHeight = 5000;

	m_nGraphHeight = nGraphHeight;

	UpdateData(FALSE);

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnDeltaposGraphMarginSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	UpdateData(TRUE);

	int nGraphMargin = m_nGraphMargin - pNMUpDown->iDelta;

	if (nGraphMargin < -100)
		nGraphMargin = -100;

	if (nGraphMargin > 100)
		nGraphMargin = 100;

	m_nGraphMargin = nGraphMargin;

	UpdateData(FALSE);

	*pResult = 0;
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnBnClickedResizeToExtent()
{
	UpdateData(TRUE);

	if (!m_bResizeToExtent)
	{
		if (!m_nGraphWidth)
			m_nGraphWidth = DEFAULT_GRAPH_WIDTH;
		if (!m_nGraphHeight)
			m_nGraphHeight = DEFAULT_GRAPH_HEIGHT;

		UpdateData(FALSE);
	}

	UpdateGraphControls();
}
//////////////////////////////////////////////////////////////////////
void CGraphSettingsDlg::OnBnClickedGraphAutoscaleCheck()
{
	UpdateData(TRUE);

	UpdateRangeControls();
}
//////////////////////////////////////////////////////////////////////
