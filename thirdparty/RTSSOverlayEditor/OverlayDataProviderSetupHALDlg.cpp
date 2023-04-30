// OverlayDataProviderSetupHALDlg.h: interface for the COverlayDataProviderSetupHALDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayDataProviderSetupHALDlg.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataProviderSetupHALDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayDataProviderSetupHALDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderSetupHALDlg::COverlayDataProviderSetupHALDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayDataProviderSetupHALDlg::IDD, pParent)
{
	m_bEnableLowLevelIODriver	= FALSE;
	m_bEnableSMARTMonitoring	= FALSE;

	m_bRestartRequired			= FALSE;
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProviderSetupHALDlg::~COverlayDataProviderSetupHALDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderSetupHALDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_ENABLE_LOW_LEVEL_IO_DRIVER_CHECK, m_bEnableLowLevelIODriver);
	DDX_Check(pDX, IDC_ENABLE_SMART_MONITORING_CHECK, m_bEnableSMARTMonitoring);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayDataProviderSetupHALDlg, CDialog)
	ON_BN_CLICKED(IDC_ENABLE_LOW_LEVEL_IO_DRIVER_CHECK, &COverlayDataProviderSetupHALDlg::OnBnClickedEnableLowLevelIoDriverCheck)
	ON_BN_CLICKED(IDC_ENABLE_SMART_MONITORING_CHECK, &COverlayDataProviderSetupHALDlg::OnBnClickedEnableSMARTMonitoringCheck)
	ON_BN_CLICKED(IDOK, &COverlayDataProviderSetupHALDlg::OnBnClickedOk)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COverlayDataProviderSetupHALDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderSetupHALDlg::OnBnClickedEnableLowLevelIoDriverCheck()
{
	m_bRestartRequired = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderSetupHALDlg::OnBnClickedEnableSMARTMonitoringCheck()
{
	m_bRestartRequired = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataProviderSetupHALDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	m_bEnableLowLevelIODriver	= (g_clientWnd.GetConfigInt("Settings", "Driver"	, 1) != 0);
	m_bEnableSMARTMonitoring	= (g_clientWnd.GetConfigInt("Settings", "SMART"		, 0) != 0);

	UpdateData(FALSE);
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataProviderSetupHALDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_bRestartRequired)
	{
		DWORD dwImplementation = IMPLEMENTATION_MINI;

		if (g_driver.IsImplementationAvailable(g_hModule, IMPLEMENTATION_MINI))
			dwImplementation = IMPLEMENTATION_MINI;
		else
		if (g_driver.IsImplementationAvailable(g_hModule, IMPLEMENTATION_FULL))
			dwImplementation = IMPLEMENTATION_FULL;

		g_clientWnd.SetConfigInt("Settings", "Driver"	, m_bEnableLowLevelIODriver ? dwImplementation	: 0);
		g_clientWnd.SetConfigInt("Settings", "SMART"	, m_bEnableSMARTMonitoring	? 1					: 0);

		MessageBox(LocalizeStr("The settings will be applied after restarting the application!"), LocalizeStr("Overlay editor"), MB_ICONINFORMATION|MB_OK);
	}

	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
