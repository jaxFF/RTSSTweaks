// OverlayDataSourceSetupPerfCounterDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayDataSourceSetupPerfCounterDlg.h"
#include "PerfCounterSelectInstanceDlg.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupPerfCounterDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayDataSourceSetupPerfCounterDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceSetupPerfCounterDlg::COverlayDataSourceSetupPerfCounterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayDataSourceSetupPerfCounterDlg::IDD, pParent)
{
	m_lpSource			= NULL;
	m_hBrush			= NULL;

	m_strObjectName		= "";
	m_strObjectInstance	= "";
	m_strCounterName	= "";
	m_bDynamic			= FALSE;

	m_strSourceName		= "";
	m_strSourceUnits	= "";
	m_strSourceFormat	= "";
	m_strSourceFormula	= "";
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceSetupPerfCounterDlg::~COverlayDataSourceSetupPerfCounterDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupPerfCounterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_OBJECT_NAME_EDIT, m_strObjectName);
	DDX_Text(pDX, IDC_OBJECT_INSTANCE_EDIT, m_strObjectInstance);
	DDX_Text(pDX, IDC_COUNTER_NAME_EDIT, m_strCounterName);
	DDX_Check(pDX, IDC_DYNAMIC_CHECK, m_bDynamic);
	DDX_Text(pDX, IDC_SOURCE_NAME_EDIT, m_strSourceName);
	DDX_Text(pDX, IDC_SOURCE_UNITS_EDIT, m_strSourceUnits);
	DDX_Text(pDX, IDC_SOURCE_FORMAT_EDIT, m_strSourceFormat);
	DDX_Text(pDX, IDC_FORMULA_EDIT, m_strSourceFormula);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayDataSourceSetupPerfCounterDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_SOURCE_FORMAT_EDIT, &COverlayDataSourceSetupPerfCounterDlg::OnEnChangeSourceFormatEdit)
	ON_EN_CHANGE(IDC_FORMULA_EDIT, &COverlayDataSourceSetupPerfCounterDlg::OnEnChangeFormulaEdit)
	ON_BN_CLICKED(IDOK, &COverlayDataSourceSetupPerfCounterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SELECT_INSTANCE_BUTTON, &COverlayDataSourceSetupPerfCounterDlg::OnBnClickedSelectInstanceButton)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupPerfCounterDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupPerfCounterDlg::SetSource(COverlayDataSourcePerfCounter* lpSource)
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
BOOL COverlayDataSourceSetupPerfCounterDlg::ValidateSource()
{
	m_strObjectName.Trim();

	if (m_strObjectName.IsEmpty())
	{
		GetDlgItem(IDC_OBJECT_NAME_EDIT)->SetFocus();
		return FALSE;
	}

	m_strCounterName.Trim();

	if (m_strCounterName.IsEmpty())
	{
		GetDlgItem(IDC_COUNTER_NAME_EDIT)->SetFocus();
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
BOOL COverlayDataSourceSetupPerfCounterDlg::ValidateInt(LPCSTR lpLine)
{
	BOOL bValid = FALSE;

	if ((*lpLine == '+') || (*lpLine == '-'))
		lpLine++;

	while ((*lpLine >= '0') && (*lpLine <= '9'))
	{
		lpLine++;
		bValid = TRUE;
	}

	if (*lpLine)
		return FALSE;

	return bValid;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceSetupPerfCounterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	if (m_lpSource)
	{
		//object name

		m_strObjectName = m_lpSource->GetObjectName();

		//instance name

		CString strInstance = m_lpSource->GetInstanceName();

		if (strlen(strInstance))
			m_strObjectInstance = m_lpSource->GetInstanceName();
		else
		{
			LONG dwInstanceIndex = m_lpSource->GetInstanceIndex();

			if (dwInstanceIndex != -1)
				m_strObjectInstance.Format("%d", dwInstanceIndex);
			else
				m_strObjectInstance = "";
		}

		//counter name

		m_strCounterName = m_lpSource->GetCounterName();

		//dynamic mode

		m_bDynamic = m_lpSource->IsDynamic();

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
void COverlayDataSourceSetupPerfCounterDlg::OnDestroy()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);

		m_hBrush = NULL;
	}

	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH COverlayDataSourceSetupPerfCounterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
void COverlayDataSourceSetupPerfCounterDlg::OnEnChangeSourceFormatEdit()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_SOURCE_FORMAT_EDIT)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupPerfCounterDlg::OnEnChangeFormulaEdit()
{
	UpdateData(TRUE);

	m_source.SetFormula(m_strSourceFormula);

	GetDlgItem(IDC_FORMULA_EDIT)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupPerfCounterDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (!ValidateSource())
	{
		MessageBeep(MB_ICONERROR);
		return;
	}

	if (m_lpSource)
	{
		//object name

		m_lpSource->SetObjectName(m_strObjectName);

		//instance name or index

		if (ValidateInt(m_strObjectInstance))
		{
			LONG dwInstance;
			sscanf_s(m_strObjectInstance, "%d", &dwInstance);

			m_lpSource->SetInstanceIndex(dwInstance);
			m_lpSource->SetInstanceName("");
		}
		else
		{
			m_lpSource->SetInstanceIndex(-1);
			m_lpSource->SetInstanceName(m_strObjectInstance);
		}

		//counter name

		m_lpSource->SetCounterName(m_strCounterName);

		//dynamic mode

		m_lpSource->SetDynamic(m_bDynamic);

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
void COverlayDataSourceSetupPerfCounterDlg::OnBnClickedSelectInstanceButton()
{
	UpdateData(TRUE);

	CPerfCounterSelectInstanceDlg dlg;

	dlg.m_strObjectName		= m_strObjectName;
	dlg.m_strObjectInstance	= m_strObjectInstance;

	if (IDOK == dlg.DoModal())
	{
		m_strObjectInstance = dlg.m_strObjectInstance;

		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
