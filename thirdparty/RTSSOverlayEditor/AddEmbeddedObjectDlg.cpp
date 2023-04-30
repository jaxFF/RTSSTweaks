// AddEmbeddedObjectDlg.cpp : implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "AddEmbeddedObjectDlg.h"
#include "HypertextExtension.h"
#include "OverlayDataSourceInternal.h"
#include "GraphSettingsDlg.h"
#include "ImageSettingsDlg.h"
#include "AnimatedImageSettingsDlg.h"

#include "RTSSSharedMemory.h"
//////////////////////////////////////////////////////////////////////
// CAddEmbeddedObjectDlg dialog
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CAddEmbeddedObjectDlg, CDialog)
//////////////////////////////////////////////////////////////////////
CAddEmbeddedObjectDlg::CAddEmbeddedObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddEmbeddedObjectDlg::IDD, pParent)
{
	m_lpOverlay				= NULL;
	m_hBrush				= NULL;

	m_bTable				= FALSE;
	m_strSource				= "";
	m_strTable				= "";
	m_strText				= "";

	m_bAddLabel				= FALSE;
	m_bAddGraph				= FALSE;
	m_bAddCurrentValue		= FALSE;
	m_bAddImage				= FALSE;

	m_nGraphTemplate		= 0;
	m_nGraphWidth			= DEFAULT_GRAPH_WIDTH;
	m_nGraphHeight			= DEFAULT_GRAPH_HEIGHT;
	m_nGraphMargin			= DEFAULT_GRAPH_MARGIN;
	m_fltGraphMin			= DEFAULT_GRAPH_MIN;
	m_fltGraphMax			= DEFAULT_GRAPH_MAX;
	m_dwGraphFlags			= DEFAULT_GRAPH_FLAGS;
	
	m_nImageTemplate		= 2;
	m_nImageWidth			= DEFAULT_ANIMATED_IMAGE_WIDTH;
	m_nImageHeight			= DEFAULT_ANIMATED_IMAGE_HEIGHT;
	m_fltImageMin			= DEFAULT_ANIMATED_IMAGE_MIN;
	m_fltImageMax			= DEFAULT_ANIMATED_IMAGE_MAX;
	m_nImageSpriteX			= DEFAULT_ANIMATED_IMAGE_SPRITE_X;
	m_nImageSpriteY			= DEFAULT_ANIMATED_IMAGE_SPRITE_Y;
	m_nImageSpriteWidth		= DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH;
	m_nImageSpriteHeight	= DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT;
	m_nImageRotationAngle	= DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
	m_nImageSpritesNum		= DEFAULT_ANIMATED_IMAGE_SPRITES_NUM;
	m_nImageSpritesPerLine	= DEFAULT_ANIMATED_IMAGE_SPRITES_PER_LINE;
	m_fltImageBias			= DEFAULT_ANIMATED_IMAGE_BIAS;
	m_nImageRotationAngleMin = DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
	m_nImageRotationAngleMax = DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
}
//////////////////////////////////////////////////////////////////////
CAddEmbeddedObjectDlg::~CAddEmbeddedObjectDlg()
{
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::DoDataExchange(CDataExchange* pDX)

{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATA_SOURCES_COMBO, m_ctrlDataSourcesCombo);
	DDX_Check(pDX, IDC_ADD_LABEL_CHECK, m_bAddLabel);
	DDX_Check(pDX, IDC_ADD_GRAPH_CHECK, m_bAddGraph);
	DDX_Check(pDX, IDC_ADD_CURRENT_VALUE_CHECK, m_bAddCurrentValue);
	DDX_CBIndex(pDX, IDC_GRAPH_TEMPLATE_COMBO, m_nGraphTemplate);
	DDX_Check(pDX, IDC_ADD_IMAGE_CHECK, m_bAddImage);
	DDX_CBIndex(pDX, IDC_IMAGE_TEMPLATE_COMBO, m_nImageTemplate);
	DDX_Radio(pDX, IDC_DATA_SOURCE_RADIO, m_bTable);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAddEmbeddedObjectDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddEmbeddedObjectDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_GRAPH_TEMPLATE_COMBO, &CAddEmbeddedObjectDlg::OnCbnSelchangeGraphTemplateCombo)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_ADD_GRAPH_CHECK, &CAddEmbeddedObjectDlg::OnBnClickedAddGraphCheck)
	ON_BN_CLICKED(IDC_ADD_IMAGE_CHECK, &CAddEmbeddedObjectDlg::OnBnClickedAddImageCheck)
	ON_CBN_SELCHANGE(IDC_IMAGE_TEMPLATE_COMBO, &CAddEmbeddedObjectDlg::OnCbnSelchangeImageTemplateCombo)
	ON_BN_CLICKED(IDC_EDIT_EMBEDDED_GRAPH, &CAddEmbeddedObjectDlg::OnBnClickedEditEmbeddedGraph)
	ON_BN_CLICKED(IDC_EDIT_EMBEDDED_IMAGE, &CAddEmbeddedObjectDlg::OnBnClickedEditEmbeddedImage)
	ON_BN_CLICKED(IDC_DATA_SOURCE_RADIO, &CAddEmbeddedObjectDlg::OnBnClickedDataSourceRadio)
	ON_BN_CLICKED(IDC_TABLE_RADIO, &CAddEmbeddedObjectDlg::OnBnClickedTableRadio)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// CAddEmbeddedObjectDlg message handlers
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
//////////////////////////////////////////////////////////////////////
BOOL CAddEmbeddedObjectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	if (m_lpOverlay)
	{
		GetDlgItem(IDC_TABLE_RADIO)->EnableWindow(m_lpOverlay->GetTextTablesList()->GetCount());

		InitDataSourcesList();

		UpdateGraphControls();
		UpdateImageControls();

		UpdateData(FALSE);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	int iSelection = m_ctrlDataSourcesCombo.GetCurSel();

	if (iSelection != -1)
	{
		CString strSource;
		m_ctrlDataSourcesCombo.GetLBText(iSelection, strSource);

		if (m_bTable)
		{
			if (!m_lpOverlay->FindTextTable(strSource))
				strSource = "";

			m_strText.Format("<TT=%s>", strSource);
		}
		else
		{
			if (m_bAddLabel)
				m_strText += strSource;

			if (m_bAddGraph)
			{
				if (m_bAddLabel)
					m_strText += "\\n";

				CHypertextExtension extension;

				m_strText += extension.FormatGraphTag(m_nGraphTemplate ? TRUE : FALSE, strSource, m_nGraphWidth, m_nGraphHeight, m_nGraphMargin, m_fltGraphMin, m_fltGraphMax, m_dwGraphFlags);
			}

			if (m_bAddCurrentValue)
			{
				if (m_bAddLabel || m_bAddGraph)
					m_strText += " ";

				COverlayDataSource* lpSource = m_lpOverlay->FindDataSource(strSource);

				if (lpSource)
				{
					if (!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE)					|| 
						!strcmp(strSource, INTERNAL_SOURCE_FRAMETIME)					||
						!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_MIN)				||
						!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_AVG)				||
						!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_MAX)				||
						!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW)	||
						!strcmp(strSource, INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW))
						m_strText += lpSource->GetDataStr();
					else
					{
						m_strText += "%";
						m_strText += strSource;
						m_strText += "%";
					}

					m_strText += " ";
					m_strText += lpSource->GetUnits();
				}
			}

			if (m_bAddImage)
			{
				if (m_bAddLabel || m_bAddGraph || m_bAddCurrentValue)
					m_strText += "\\n";

				CHypertextExtension extension;

				switch (m_nImageTemplate)
				{
				case 0:
					m_strText += extension.FormatAnimatedImageTag(FALSE, strSource, m_nImageWidth, m_nImageHeight, m_fltImageMin, m_fltImageMax, m_nImageSpriteX, m_nImageSpriteY, m_nImageSpriteWidth, m_nImageSpriteHeight, m_nImageSpritesNum, m_nImageSpritesPerLine, m_fltImageBias, m_nImageRotationAngleMin, m_nImageRotationAngleMax);
					break;
				case 1:
					m_strText += extension.FormatAnimatedImageTag(TRUE, strSource, m_nImageWidth, m_nImageHeight, m_fltImageMin, m_fltImageMax, m_nImageSpriteX, m_nImageSpriteY, m_nImageSpriteWidth, m_nImageSpriteHeight, m_nImageSpritesNum, m_nImageSpritesPerLine, m_fltImageBias, m_nImageRotationAngleMin, m_nImageRotationAngleMax);
					break;
				case 2:
					m_strText += extension.FormatImageTag(m_nImageWidth, m_nImageHeight, m_nImageSpriteX, m_nImageSpriteY, m_nImageSpriteWidth, m_nImageSpriteHeight, 0);
					break;
				}
			}
		}
	}

	OnOK();
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::UpdateGraphControls()
{
	GetDlgItem(IDC_GRAPH_TEMPLATE_COMBO)->EnableWindow(m_bAddGraph);
	GetDlgItem(IDC_EDIT_EMBEDDED_GRAPH)->EnableWindow(m_bAddGraph && m_nGraphTemplate);
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::UpdateImageControls()
{
	GetDlgItem(IDC_IMAGE_TEMPLATE_COMBO)->EnableWindow(m_bAddImage);
	GetDlgItem(IDC_EDIT_EMBEDDED_IMAGE)->EnableWindow(m_bAddImage && m_nImageTemplate);
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnCbnSelchangeGraphTemplateCombo()
{
	UpdateData(TRUE);

	UpdateGraphControls();
}
//////////////////////////////////////////////////////////////////////
HBRUSH CAddEmbeddedObjectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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
void CAddEmbeddedObjectDlg::OnDestroy()
{
	if (m_hBrush)
	{
		DeleteObject(m_hBrush);

		m_hBrush = NULL;
	}

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnBnClickedAddGraphCheck()
{
	UpdateData(TRUE);

	UpdateGraphControls();
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnBnClickedAddImageCheck()
{
	UpdateData(TRUE);

	UpdateImageControls();
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnCbnSelchangeImageTemplateCombo()
{
	UpdateData(TRUE);

	UpdateImageControls();
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnBnClickedEditEmbeddedGraph()
{
	int iSelection = m_ctrlDataSourcesCombo.GetCurSel();

	if (iSelection != -1)
	{
		CString strSource;
		m_ctrlDataSourcesCombo.GetLBText(iSelection, strSource);

		CGraphSettingsDlg dlg;

		dlg.m_strSource			= strSource;
		dlg.m_nGraphTemplate	= m_nGraphTemplate;

		dlg.m_nGraphWidth		= m_nGraphWidth;
		dlg.m_nGraphHeight		= m_nGraphHeight;
		dlg.m_nGraphMargin		= m_nGraphMargin;
		dlg.m_fltGraphMin		= m_fltGraphMin;
		dlg.m_fltGraphMax		= m_fltGraphMax;
		dlg.m_dwGraphFlags		= m_dwGraphFlags;

		dlg.SetOverlay(m_lpOverlay);

		if (IDOK == dlg.DoModal())
		{
			iSelection = m_ctrlDataSourcesCombo.FindString(-1, dlg.m_strSource);
			if (iSelection != -1)
				m_ctrlDataSourcesCombo.SetCurSel(iSelection);

			m_nGraphTemplate	= dlg.m_nGraphTemplate;

			m_nGraphWidth		= dlg.m_nGraphWidth;
			m_nGraphHeight		= dlg.m_nGraphHeight;
			m_nGraphMargin		= dlg.m_nGraphMargin;
			m_fltGraphMin		= dlg.m_fltGraphMin;
			m_fltGraphMax		= dlg.m_fltGraphMax;
			m_dwGraphFlags		= dlg.m_dwGraphFlags;

			UpdateData(FALSE);

			UpdateGraphControls();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnBnClickedEditEmbeddedImage()
{
	int iSelection = m_ctrlDataSourcesCombo.GetCurSel();

	if (iSelection != -1)
	{
		CString strSource;
		m_ctrlDataSourcesCombo.GetLBText(iSelection, strSource);

		if (m_nImageTemplate == 2)
		{
			CImageSettingsDlg dlg;

			dlg.m_nImageWidth			= m_nImageWidth;
			dlg.m_nImageHeight			= m_nImageHeight;
			dlg.m_nSpriteX				= m_nImageSpriteX;
			dlg.m_nSpriteY				= m_nImageSpriteY;
			dlg.m_nSpriteWidth			= m_nImageSpriteWidth;
			dlg.m_nSpriteHeight			= m_nImageSpriteHeight;
			dlg.m_nImageRotationAngle	= m_nImageRotationAngle;

			dlg.SetOverlay(m_lpOverlay);

			if (IDOK == dlg.DoModal())
			{
				m_nImageWidth			= dlg.m_nImageWidth;
				m_nImageHeight			= dlg.m_nImageHeight;
				m_nImageSpriteX			= dlg.m_nSpriteX;
				m_nImageSpriteY			= dlg.m_nSpriteY;
				m_nImageSpriteWidth		= dlg.m_nSpriteWidth;
				m_nImageSpriteHeight	= dlg.m_nSpriteHeight;
				m_nImageRotationAngle	= dlg.m_nImageRotationAngle;

				UpdateData(FALSE);

				UpdateImageControls();
			}
		}
		else
		{
			CAnimatedImageSettingsDlg dlg;

			dlg.m_strSource				= strSource;
			dlg.m_nImageTemplate		= m_nImageTemplate;

			dlg.m_nImageWidth			= m_nImageWidth;
			dlg.m_nImageHeight			= m_nImageHeight;
			dlg.m_fltImageMin			= m_fltImageMin;
			dlg.m_fltImageMax			= m_fltImageMax;
			dlg.m_nSpriteX				= m_nImageSpriteX;
			dlg.m_nSpriteY				= m_nImageSpriteY;
			dlg.m_nSpriteWidth			= m_nImageSpriteWidth;
			dlg.m_nSpriteHeight			= m_nImageSpriteHeight;
			dlg.m_nSpritesNum			= m_nImageSpritesNum;
			dlg.m_nSpritesPerLine		= m_nImageSpritesPerLine;
			dlg.m_fltImageBias			= m_fltImageBias;
			dlg.m_nImageRotationAngleMin = m_nImageRotationAngleMin;
			dlg.m_nImageRotationAngleMax = m_nImageRotationAngleMax;

			dlg.SetOverlay(m_lpOverlay);

			if (IDOK == dlg.DoModal())
			{
				iSelection = m_ctrlDataSourcesCombo.FindString(-1, dlg.m_strSource);
				if (iSelection != -1)
					m_ctrlDataSourcesCombo.SetCurSel(iSelection);

				m_nImageTemplate		= dlg.m_nImageTemplate;

				m_nImageWidth			= dlg.m_nImageWidth;
				m_nImageHeight			= dlg.m_nImageHeight;
				m_fltImageMin			= dlg.m_fltImageMin;
				m_fltImageMax			= dlg.m_fltImageMax;
				m_nImageSpriteX			= dlg.m_nSpriteX;
				m_nImageSpriteY			= dlg.m_nSpriteY;
				m_nImageSpriteWidth		= dlg.m_nSpriteWidth;
				m_nImageSpriteHeight	= dlg.m_nSpriteHeight;
				m_nImageSpritesNum		= dlg.m_nSpritesNum;
				m_nImageSpritesPerLine	= dlg.m_nSpritesPerLine;
				m_fltImageBias			= dlg.m_fltImageBias;
				m_nImageRotationAngleMin = dlg.m_nImageRotationAngleMin;
				m_nImageRotationAngleMax = dlg.m_nImageRotationAngleMax;

				UpdateData(FALSE);

				UpdateImageControls();
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnBnClickedDataSourceRadio()
{
	UpdateData(TRUE);

	InitDataSourcesList();

	UpdateTableControls();
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::OnBnClickedTableRadio()
{
	UpdateData(TRUE);

	InitDataSourcesList();

	UpdateTableControls();
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::InitDataSourcesList()
{
	m_ctrlDataSourcesCombo.ResetContent();

	if (m_lpOverlay)
	{
		if (m_bTable)
		{
			CTextTablesList* lpTextTables = m_lpOverlay->GetTextTablesList();

			POSITION pos = lpTextTables->GetHeadPosition();

			while (pos)
			{
				CTextTable* lpTextTable = lpTextTables->GetNext(pos);

				CString strTable = lpTextTable->m_strName;

				if (m_ctrlDataSourcesCombo.FindString(-1, strTable) == -1)
				{
					int iItem = m_ctrlDataSourcesCombo.AddString(strTable.IsEmpty() ? LocalizeStr("<nameless>") : strTable);

					m_ctrlDataSourcesCombo.SetItemData(iItem, lpTextTable->GetTableID());

					if (!strcmp(m_strTable, strTable))
						m_ctrlDataSourcesCombo.SetCurSel(iItem);
				}	
			}
		}
		else
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
		}

		if (m_ctrlDataSourcesCombo.GetCurSel() == -1)
			m_ctrlDataSourcesCombo.SetCurSel(0);
	}
}
//////////////////////////////////////////////////////////////////////
void CAddEmbeddedObjectDlg::UpdateTableControls()
{
	BOOL bShow = !m_bTable;

	GetDlgItem(IDC_ADD_LABEL_CHECK			)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_ADD_CURRENT_VALUE_CHECK	)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_ADD_GRAPH_CHECK			)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_ADD_IMAGE_CHECK			)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_EDIT_EMBEDDED_GRAPH		)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_EDIT_EMBEDDED_IMAGE		)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);

	GetDlgItem(IDC_GRAPH_TEMPLATE_COMBO		)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_IMAGE_TEMPLATE_COMBO		)->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
}
//////////////////////////////////////////////////////////////////////
