// OverlayDataSourceSetupHALDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayDataSourceSetupHALDlg.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupHALDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayDataSourceSetupHALDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceSetupHALDlg::COverlayDataSourceSetupHALDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayDataSourceSetupHALDlg::IDD, pParent)
{
	m_lpSource			= NULL;
	m_hBrush			= NULL;

	m_strSensorID		= "";
	m_strSourceName		= "";
	m_strSourceUnits	= "";
	m_strSourceFormat	= "";
	m_strSourceFormula	= "";
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceSetupHALDlg::~COverlayDataSourceSetupHALDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHALDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SENSOR_ID_EDIT, m_strSensorID);
	DDX_Text(pDX, IDC_SOURCE_NAME_EDIT, m_strSourceName);
	DDX_Text(pDX, IDC_SOURCE_UNITS_EDIT, m_strSourceUnits);
	DDX_Text(pDX, IDC_SOURCE_FORMAT_EDIT, m_strSourceFormat);
	DDX_Text(pDX, IDC_FORMULA_EDIT, m_strSourceFormula);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayDataSourceSetupHALDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_SOURCE_FORMAT_EDIT, &COverlayDataSourceSetupHALDlg::OnEnChangeSourceFormatEdit)
	ON_EN_CHANGE(IDC_FORMULA_EDIT, &COverlayDataSourceSetupHALDlg::OnEnChangeFormulaEdit)
	ON_BN_CLICKED(IDOK, &COverlayDataSourceSetupHALDlg::OnBnClickedOk)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupHALDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHALDlg::SetSource(COverlayDataSourceHAL* lpSource)
{
	m_lpSource = lpSource;

	if (lpSource)
	{
		m_source.Copy(lpSource);

		m_source.SetOverlay(lpSource->GetOverlay());
		m_source.SetDataSourcesList(lpSource->GetDataSourcesList());
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceSetupHALDlg::ValidateSource()
{
	m_strSensorID.Trim();

	if (m_strSensorID.IsEmpty())
	{
		GetDlgItem(IDC_SENSOR_ID_EDIT)->SetFocus();
		return FALSE;
	}

	m_strSourceName.Trim();

	if (m_strSourceName.IsEmpty())
	{
		GetDlgItem(IDC_SOURCE_NAME_EDIT)->SetFocus();
		return FALSE;
	}

	m_strSourceFormat.Trim();

	if (!m_strSourceFormat.IsEmpty() && !m_lpSource->ValidateFormat(m_strSourceFormat))
	{
		GetDlgItem(IDC_SOURCE_FORMAT_EDIT)->SetFocus();
		return FALSE;
	}

	m_strSourceFormula.Trim();

	if (!m_strSourceFormula.IsEmpty() && !m_source.IsValidFormula())
	{
		GetDlgItem(IDC_FORMULA_EDIT)->SetFocus();
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceSetupHALDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	if (m_lpSource)
	{
		//sensor ID

		m_strSensorID = m_lpSource->GetID();

		//data source name

		m_strSourceName = m_lpSource->GetName();

		//data source units

		m_strSourceUnits = m_lpSource->GetUnits();

		//data source output format

		m_strSourceFormat = m_lpSource->GetFormat();

		//data source correction formula

		m_strSourceFormula		= m_lpSource->GetFormula();
		m_source.SetFormula(m_strSourceFormula);

		UpdateData(FALSE);
	}
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHALDlg::OnDestroy()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);

		m_hBrush = NULL;
	}

	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH COverlayDataSourceSetupHALDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	COLORREF clrBk		= g_dwHeaderBgndColor;
	COLORREF clrText	= g_dwHeaderTextColor;

	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT nID = pWnd->GetDlgCtrlID();

	if ((nID == IDC_SENSOR_PROPERTIES_HEADER) ||
		(nID == IDC_SOURCE_PROPERTIES_HEADER))
	{
		if (!m_hBrush)
 			m_hBrush = CreateSolidBrush(clrBk);

		pDC->SetBkColor(clrBk);
		pDC->SetTextColor(clrText);

		return m_hBrush;
	}
	else
	if (nID == IDC_SOURCE_FORMAT_EDIT)
	{
		if (!m_strSourceFormat.IsEmpty() && !m_lpSource->ValidateFormat(m_strSourceFormat))
			pDC->SetTextColor(0xFF);
	}
	else 
	if (nID == IDC_FORMULA_EDIT)
	{
		if (!m_source.IsValidFormula())
			pDC->SetTextColor(0xFF);

		return hbr;
	}

	return hbr;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHALDlg::OnEnChangeSourceFormatEdit()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_SOURCE_FORMAT_EDIT)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHALDlg::OnEnChangeFormulaEdit()
{
	UpdateData(TRUE);

	m_source.SetFormula(m_strSourceFormula);

	GetDlgItem(IDC_FORMULA_EDIT)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHALDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (!ValidateSource())
	{
		MessageBeep(MB_ICONERROR);
		return;
	}

	if (m_lpSource)
	{
		//sensor ID

		m_lpSource->SetID(m_strSensorID);

		//data source name

		m_lpSource->SetName(m_strSourceName);

		//data source units

		m_lpSource->SetUnits(m_strSourceUnits);

		//data source output format

		m_lpSource->SetFormat(m_strSourceFormat);

		//data source correction formula

		m_lpSource->SetFormula(m_strSourceFormula);
	}

	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
