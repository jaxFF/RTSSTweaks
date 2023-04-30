// OverlayDataSourceSetupHwInfoDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayDataSourceSetupHwInfoDlg.h"
#include "HwInfoWrapper.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupHwInfoDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayDataSourceSetupHwInfoDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
typedef struct HWINFO_READING_TYPE_DESC
{
	DWORD	dwType;
	LPCSTR	lpName;
} HWINFO_READING_TYPE_DESC, *LPHWINFO_READING_TYPE_DESC;
//////////////////////////////////////////////////////////////////////
COverlayDataSourceSetupHwInfoDlg::COverlayDataSourceSetupHwInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayDataSourceSetupHwInfoDlg::IDD, pParent)
{
	m_lpSource	= NULL;
	m_hBrush	= NULL;

	m_strSensorInstance	= "";
	m_strReadingName	= "";
	m_strSourceName		= "";
	m_strSourceUnits	= "";
	m_strSourceFormat	= "";
	m_strSourceFormula	= "";
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceSetupHwInfoDlg::~COverlayDataSourceSetupHwInfoDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHwInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_READING_TYPE_COMBO, m_readingTypeCombo);
	DDX_Text(pDX, IDC_SENSOR_INSTANCE_EDIT, m_strSensorInstance);
	DDX_Text(pDX, IDC_READING_NAME_EDIT, m_strReadingName);
	DDX_Text(pDX, IDC_SOURCE_NAME_EDIT, m_strSourceName);
	DDX_Text(pDX, IDC_SOURCE_UNITS_EDIT, m_strSourceUnits);
	DDX_Text(pDX, IDC_SOURCE_FORMAT_EDIT, m_strSourceFormat);
	DDX_Text(pDX, IDC_FORMULA_EDIT, m_strSourceFormula);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayDataSourceSetupHwInfoDlg, CDialog)
	ON_EN_CHANGE(IDC_FORMULA_EDIT, &COverlayDataSourceSetupHwInfoDlg::OnEnChangeFormulaEdit)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &COverlayDataSourceSetupHwInfoDlg::OnBnClickedOk)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_SOURCE_FORMAT_EDIT, &COverlayDataSourceSetupHwInfoDlg::OnEnChangeSourceFormatEdit)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupHwInfoDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHwInfoDlg::SetSource(COverlayDataSourceHwInfo* lpSource)
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
BOOL COverlayDataSourceSetupHwInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

#ifdef HWINFO_SDK_AVAILABLE
	if (m_lpSource)
	{
		//reading type

		HWINFO_READING_TYPE_DESC readingTypes[] = 
		{
			{ SENSOR_TYPE_NONE		, "None"		},
			{ SENSOR_TYPE_TEMP		, "Temperature"	},
			{ SENSOR_TYPE_VOLT		, "Voltage"		},
			{ SENSOR_TYPE_FAN		, "Fan speed"	},
			{ SENSOR_TYPE_CURRENT	, "Current"		},
			{ SENSOR_TYPE_POWER		, "Power"		},
			{ SENSOR_TYPE_CLOCK		, "Clock"		},
			{ SENSOR_TYPE_USAGE		, "Usage"		},
			{ SENSOR_TYPE_OTHER		, "Other"		}
		};

		m_readingTypeCombo.SetCurSel(0);

		for (int iIndex=0; iIndex<_countof(readingTypes); iIndex++)
		{
			int iItem = m_readingTypeCombo.AddString(LocalizeStr(readingTypes[iIndex].lpName));

			m_readingTypeCombo.SetItemData(iItem, readingTypes[iIndex].dwType);

			if (m_lpSource->GetReadingType() == readingTypes[iIndex].dwType)
				m_readingTypeCombo.SetCurSel(iItem);
		}

		//sensor instance

		if (m_lpSource->GetSensorInst() != 0xFFFFFFFF)
			m_strSensorInstance.Format("%d", m_lpSource->GetSensorInst());
		else
			m_strSensorInstance = "";

		//reading name

		m_strReadingName = m_lpSource->GetReadingName();

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
#endif

	return TRUE; 
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHwInfoDlg::OnEnChangeFormulaEdit()
{
	UpdateData(TRUE);

	m_source.SetFormula(m_strSourceFormula);

	GetDlgItem(IDC_FORMULA_EDIT)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH COverlayDataSourceSetupHwInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
void COverlayDataSourceSetupHwInfoDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (!ValidateSource())
	{
		MessageBeep(MB_ICONERROR);
		return;
	}

#ifdef HWINFO_SDK_AVAILABLE
	if (m_lpSource)
	{
		//reading type

		int iItem = m_readingTypeCombo.GetCurSel();

		if (iItem != -1)
			m_lpSource->SetReadingType(m_readingTypeCombo.GetItemData(iItem));
		else
			m_lpSource->SetReadingType(SENSOR_TYPE_NONE);

		//sensor instance

		DWORD dwSensorInst;

		if (sscanf_s(m_strSensorInstance, "%d", &dwSensorInst) != 1)
			dwSensorInst = 0xFFFFFFFF;

		m_lpSource->SetSensorInst(dwSensorInst);

		//reading name

		m_lpSource->SetReadingName(m_strReadingName);

		//data source name

		m_lpSource->SetName(m_strSourceName);

		//data source units

		m_lpSource->SetUnits(m_strSourceUnits);

		//data source output format

		m_lpSource->SetFormat(m_strSourceFormat);

		//data source correction formula

		m_lpSource->SetFormula(m_strSourceFormula);
	}
#endif

	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceSetupHwInfoDlg::ValidateSource()
{
	m_strReadingName.Trim();

	if (m_strReadingName.IsEmpty())
	{
		GetDlgItem(IDC_READING_NAME_EDIT)->SetFocus();
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
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHwInfoDlg::OnDestroy()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);

		m_hBrush = NULL;
	}

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupHwInfoDlg::OnEnChangeSourceFormatEdit()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_SOURCE_FORMAT_EDIT)->RedrawWindow();
}
//////////////////////////////////////////////////////////////////////
