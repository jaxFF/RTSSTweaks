// OverlaySettingsDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlaySettingsDlg.h"
/////////////////////////////////////////////////////////////////////////////
// COverlaySettingsDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlaySettingsDlg, CDialog)

COverlaySettingsDlg::COverlaySettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlaySettingsDlg::IDD, pParent)
	, m_strPingAddr(_T(""))
{
	m_lpOverlay			= NULL;

	m_strName			= "";
	m_nRefreshPeriod	= 1000;
	m_strEmbeddedImage	= "";
}
/////////////////////////////////////////////////////////////////////////////
COverlaySettingsDlg::~COverlaySettingsDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlaySettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT, m_strName);
	DDX_Text(pDX, IDC_REFRESH_PERIOD_EDIT, m_nRefreshPeriod);
	DDX_Text(pDX, IDC_EMBEDDED_IMAGE_EDIT, m_strEmbeddedImage);
	DDV_MinMaxInt(pDX, m_nRefreshPeriod, 100, 60000);
	DDX_Text(pDX, IDC_PING_ADDR_EDIT, m_strPingAddr);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlaySettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_BROWSE_FOR_EMBEDDED_IMAGE, &COverlaySettingsDlg::OnBnClickedBrowseForEmbeddedImage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_REFRESH_PERIOD_SPIN, &COverlaySettingsDlg::OnDeltaposRefreshPeriodSpin)
	ON_BN_CLICKED(IDC_MASTER_SETTINGS, &COverlaySettingsDlg::OnBnClickedMasterSettings)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void COverlaySettingsDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
void COverlaySettingsDlg::OnBnClickedBrowseForEmbeddedImage()
{
	CFileDialog dlg(TRUE, "ovl", NULL, OFN_HIDEREADONLY, LocalizeStr("PNG image files (*.png)|*.png||"), AfxGetMainWnd());

	CString strInitialDir		= m_lpOverlay->GetCfgFolder();
	dlg.m_ofn.lpstrInitialDir	= strInitialDir ;

	if (IDOK == dlg.DoModal())
	{
		m_strEmbeddedImage = dlg.GetFileName();

		UpdateData(FALSE);

	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlaySettingsDlg::OnDeltaposRefreshPeriodSpin(NMHDR *pNMHDR, LRESULT *pResult)
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

	if (m_nRefreshPeriod < 100)
		m_nRefreshPeriod = 100;
	if (m_nRefreshPeriod > 60000)
		m_nRefreshPeriod = 60000;

	UpdateData(FALSE);

	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlaySettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlaySettingsDlg::OnBnClickedMasterSettings()
{
	if (IDYES == MessageBox(LocalizeStr("Warning! This will set your On-Screen Display rendering mode, raster font and zoom settings to layout creator's ones. Do you want to continue?"), LocalizeStr("Master settings"), MB_YESNOCANCEL))
		m_lpOverlay->OnMasterSettings();
}
/////////////////////////////////////////////////////////////////////////////
