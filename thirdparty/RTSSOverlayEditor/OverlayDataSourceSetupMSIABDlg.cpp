// OverlayDataSourceSetupMSIABDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayDataSourceSetupMSIABDlg.h"
/////////////////////////////////////////////////////////////////////////////
typedef struct DATA_SOURCE_ID_DESC
{
	DWORD	dwID;
	LPCSTR	lpName;
} DATA_SOURCE_ID_DESC, *LPDATA_SOURCE_ID_DESC;
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupMSIABDlg dialog
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayDataSourceSetupMSIABDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceSetupMSIABDlg::COverlayDataSourceSetupMSIABDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayDataSourceSetupMSIABDlg::IDD, pParent)
{
	m_lpSource			= NULL;
	m_hBrush			= NULL;

	m_strSensorInstance	= "";
	m_strSensorName		= "";
	m_strSourceName		= "";
	m_strSourceUnits	= "";
	m_strSourceFormat	= "";
	m_strSourceFormula	= "";
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceSetupMSIABDlg::~COverlayDataSourceSetupMSIABDlg()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupMSIABDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SENSOR_TYPE_COMBO, m_sensorTypeCombo);
	DDX_Text(pDX, IDC_SENSOR_INSTANCE_EDIT, m_strSensorInstance);
	DDX_Text(pDX, IDC_SENSOR_NAME_EDIT, m_strSensorName);
	DDX_Text(pDX, IDC_SOURCE_NAME_EDIT, m_strSourceName);
	DDX_Text(pDX, IDC_SOURCE_UNITS_EDIT, m_strSourceUnits);
	DDX_Text(pDX, IDC_SOURCE_FORMAT_EDIT, m_strSourceFormat);
	DDX_Text(pDX, IDC_FORMULA_EDIT, m_strSourceFormula);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayDataSourceSetupMSIABDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_SOURCE_FORMAT_EDIT, &COverlayDataSourceSetupMSIABDlg::OnEnChangeSourceFormatEdit)
	ON_EN_CHANGE(IDC_FORMULA_EDIT, &COverlayDataSourceSetupMSIABDlg::OnEnChangeFormulaEdit)
	ON_BN_CLICKED(IDOK, &COverlayDataSourceSetupMSIABDlg::OnBnClickedOk)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COverlayDataSourceSetupMSIABDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupMSIABDlg::SetSource(COverlayDataSourceMSIAB* lpSource)
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
BOOL COverlayDataSourceSetupMSIABDlg::ValidateSource()
{
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
BOOL COverlayDataSourceSetupMSIABDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	if (m_lpSource)
	{
		//sensor type

		DATA_SOURCE_ID_DESC sourceIds[] = 
		{
			{ MONITORING_SOURCE_ID_GPU_TEMPERATURE				, "GPU temperature"			},
			{ MONITORING_SOURCE_ID_PCB_TEMPERATURE				, "PCB temperature"			},
			{ MONITORING_SOURCE_ID_MEM_TEMPERATURE				, "Memory temperature"		},
			{ MONITORING_SOURCE_ID_VRM_TEMPERATURE				, "VRM temperature"			},
			{ MONITORING_SOURCE_ID_FAN_SPEED					, "Fan speed"				},
			{ MONITORING_SOURCE_ID_FAN_TACHOMETER				, "Fan tachometer"			},
			{ MONITORING_SOURCE_ID_CORE_CLOCK					, "Core clock"				},
			{ MONITORING_SOURCE_ID_SHADER_CLOCK					, "Shader clock"			},
			{ MONITORING_SOURCE_ID_MEMORY_CLOCK					, "Memory clock"			},
			{ MONITORING_SOURCE_ID_GPU_USAGE					, "GPU usage"				},
			{ MONITORING_SOURCE_ID_MEMORY_USAGE					, "Memory usage"			},
			{ MONITORING_SOURCE_ID_FB_USAGE						, "FB usage"				},
			{ MONITORING_SOURCE_ID_VID_USAGE					, "VID usage"				},
			{ MONITORING_SOURCE_ID_BUS_USAGE					, "BUS usage"				},
			{ MONITORING_SOURCE_ID_GPU_VOLTAGE					, "GPU voltage"				},
			{ MONITORING_SOURCE_ID_AUX_VOLTAGE					, "Aux voltage"				},
			{ MONITORING_SOURCE_ID_MEMORY_VOLTAGE				, "Memory voltage"			},
			{ MONITORING_SOURCE_ID_FRAMERATE					, "Framerate"				},
			{ MONITORING_SOURCE_ID_FRAMETIME					, "Frametime"				},
			{ MONITORING_SOURCE_ID_FRAMERATE_MIN				, "Framerate Min"			},
			{ MONITORING_SOURCE_ID_FRAMERATE_AVG				, "Framerate Avg"			},
			{ MONITORING_SOURCE_ID_FRAMERATE_MAX				, "Framerate Max"			},
			{ MONITORING_SOURCE_ID_FRAMERATE_1DOT0_PERCENT_LOW	, "Framerate 1% Low"		},
			{ MONITORING_SOURCE_ID_FRAMERATE_0DOT1_PERCENT_LOW	, "Framerate 0.1% Low"		},
			{ MONITORING_SOURCE_ID_GPU_REL_POWER				, "Power percent"			},
			{ MONITORING_SOURCE_ID_GPU_ABS_POWER				, "Power"					},
			{ MONITORING_SOURCE_ID_GPU_TEMP_LIMIT				, "Temp limit"				},
			{ MONITORING_SOURCE_ID_GPU_POWER_LIMIT				, "Power limit"				},
			{ MONITORING_SOURCE_ID_GPU_VOLTAGE_LIMIT			, "Voltage limit"			},
			{ MONITORING_SOURCE_ID_GPU_UTIL_LIMIT				, "No load limit"			},
			{ MONITORING_SOURCE_ID_CPU_USAGE					, "CPU usage"				},
			{ MONITORING_SOURCE_ID_RAM_USAGE					, "RAM usage"				},
			{ MONITORING_SOURCE_ID_PAGEFILE_USAGE				, "Commit charge"			},
			{ MONITORING_SOURCE_ID_CPU_TEMPERATURE				, "CPU temperature"			},
			{ MONITORING_SOURCE_ID_GPU_SLI_SYNC_LIMIT			, "SLI sync limit"			},
			{ MONITORING_SOURCE_ID_CPU_CLOCK					, "CPU clock"				},
			{ MONITORING_SOURCE_ID_AUX2_VOLTAGE					, "Aux2 voltage"			},
			{ MONITORING_SOURCE_ID_GPU_TEMPERATURE2				, "GPU temperature 2"		},
			{ MONITORING_SOURCE_ID_PCB_TEMPERATURE2				, "PCB temperature 2"		},
			{ MONITORING_SOURCE_ID_MEM_TEMPERATURE2				, "Memory temperature 2"	},
			{ MONITORING_SOURCE_ID_VRM_TEMPERATURE2				, "VRM temperature 2"		},
			{ MONITORING_SOURCE_ID_GPU_TEMPERATURE3				, "GPU temperature 3"		},
			{ MONITORING_SOURCE_ID_PCB_TEMPERATURE3				, "PCB temperature 3"		},
			{ MONITORING_SOURCE_ID_MEM_TEMPERATURE3				, "Memory temperature 3"	},
			{ MONITORING_SOURCE_ID_VRM_TEMPERATURE3				, "VRM temperature 3"		},
			{ MONITORING_SOURCE_ID_GPU_TEMPERATURE4				, "GPU temperature 4"		},
			{ MONITORING_SOURCE_ID_PCB_TEMPERATURE4				, "PCB temperature 4"		},
			{ MONITORING_SOURCE_ID_MEM_TEMPERATURE4				, "Memory temperature 4"	},
			{ MONITORING_SOURCE_ID_VRM_TEMPERATURE4				, "VRM temperature 4"		},
			{ MONITORING_SOURCE_ID_GPU_TEMPERATURE5				, "GPU temperature 5"		},
			{ MONITORING_SOURCE_ID_PCB_TEMPERATURE5				, "PCB temperature 5"		},
			{ MONITORING_SOURCE_ID_MEM_TEMPERATURE5				, "Memory temperature 5"	},
			{ MONITORING_SOURCE_ID_VRM_TEMPERATURE5				, "VRM temperature 5"		},
			{ MONITORING_SOURCE_ID_PLUGIN_GPU					, "<GPU plugin>"			},
			{ MONITORING_SOURCE_ID_PLUGIN_CPU					, "<CPU plugin>"			},
			{ MONITORING_SOURCE_ID_PLUGIN_MOBO					, "<motherboard plugin>"	},
			{ MONITORING_SOURCE_ID_PLUGIN_RAM					, "<RAM plugin>"			},
			{ MONITORING_SOURCE_ID_PLUGIN_HDD					, "<HDD plugin>"			},
			{ MONITORING_SOURCE_ID_PLUGIN_NET					, "<NET plugin>"			},
			{ MONITORING_SOURCE_ID_PLUGIN_PSU					, "<PSU plugin>"			},
			{ MONITORING_SOURCE_ID_PLUGIN_UPS					, "<UPS plugin>"			},
			{ MONITORING_SOURCE_ID_PLUGIN_MISC					, "<miscellaneous plugin>"	},
			{ MONITORING_SOURCE_ID_CPU_POWER					, "CPU power"				}
		};

		m_sensorTypeCombo.SetCurSel(0);

		for (int iIndex=0; iIndex<_countof(sourceIds); iIndex++)
		{
			int iItem = m_sensorTypeCombo.AddString(LocalizeStr(sourceIds[iIndex].lpName));

			m_sensorTypeCombo.SetItemData(iItem, sourceIds[iIndex].dwID);

			if (m_lpSource->GetSrcId() == sourceIds[iIndex].dwID)
				m_sensorTypeCombo.SetCurSel(iItem);
		}

		//sensor instance

		if (m_lpSource->GetGpu() != 0xFFFFFFFF)
			m_strSensorInstance.Format("%d", m_lpSource->GetGpu());
		else
			m_strSensorInstance = "";

		//sensor name

		m_strSensorName = m_lpSource->GetSrcName();

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
void COverlayDataSourceSetupMSIABDlg::OnDestroy()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);

		m_hBrush = NULL;
	}

	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
HBRUSH COverlayDataSourceSetupMSIABDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
void COverlayDataSourceSetupMSIABDlg::OnEnChangeSourceFormatEdit()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_SOURCE_FORMAT_EDIT)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupMSIABDlg::OnEnChangeFormulaEdit()
{
	UpdateData(TRUE);

	m_source.SetFormula(m_strSourceFormula);

	GetDlgItem(IDC_FORMULA_EDIT)->RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceSetupMSIABDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (!ValidateSource())
	{
		MessageBeep(MB_ICONERROR);
		return;
	}

	if (m_lpSource)
	{
		//sensor type

		int iItem = m_sensorTypeCombo.GetCurSel();

		if (iItem != -1)
			m_lpSource->SetSrcId(m_sensorTypeCombo.GetItemData(iItem));
		else
			m_lpSource->SetSrcId(MONITORING_SOURCE_ID_UNKNOWN);

		//sensor instance

		DWORD dwSensorInst;

		if (sscanf_s(m_strSensorInstance, "%d", &dwSensorInst) != 1)
			dwSensorInst = 0xFFFFFFFF;

		m_lpSource->SetGpu(dwSensorInst);

		//sensor name

		m_lpSource->SetSrcName(m_strSensorName);

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
