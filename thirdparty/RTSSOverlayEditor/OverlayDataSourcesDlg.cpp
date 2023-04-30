// OverlayDataSourcesDlg.cpp : implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayDataSourcesDlg.h"
#include "OverlayDataSource.h"
#include "OverlayDataSourceAIDA.h"
#include "OverlayDataSourceHwInfo.h"
#include "OverlayDataSourceMSIAB.h"
#include "OverlayDataSourceAddDlg.h"
#include "OverlayDataSourceSetupAIDADlg.h"
#include "OverlayDataSourceSetupHwInfoDlg.h"
#include "OverlayDataSourceSetupMSIABDlg.h"
#include "OverlayDataSourceSetupHALDlg.h"
#include "OverlayDataSourceSetupPerfCounterDlg.h"

#include <float.h>
//////////////////////////////////////////////////////////////////////
// COverlayDataSourcesDlg dialog
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayDataSourcesDlg, CDialog)

COverlayDataSourcesDlg::COverlayDataSourcesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayDataSourcesDlg::IDD, pParent)
{
	m_lpOverlay = NULL;
	m_nTimerID	= 0;
}
//////////////////////////////////////////////////////////////////////
COverlayDataSourcesDlg::~COverlayDataSourcesDlg()
{
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOURCES_LIST, m_sourcesListCtrl);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayDataSourcesDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SETUP_SOURCE_BUTTON, &COverlayDataSourcesDlg::OnBnClickedSetupSourceButton)
	ON_BN_CLICKED(IDC_ADD_SOURCE_BUTTON, &COverlayDataSourcesDlg::OnBnClickedAddSourceButton)
	ON_BN_CLICKED(IDC_REMOVE_SOURCE_BUTTON, &COverlayDataSourcesDlg::OnBnClickedRemoveSourceButton)
	ON_BN_CLICKED(IDOK, &COverlayDataSourcesDlg::OnBnClickedOk)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// COverlayDataSourcesDlg message handlers
//////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourcesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	//initialize local sources list

	m_sources.SetOverlay(m_lpOverlay);
	m_sources.Copy(m_lpOverlay->GetExternalDataSources());

	//init sources list control

	m_sourcesListCtrl.ShowScrollBar(SB_VERT, TRUE);

	CRect cr; m_sourcesListCtrl.GetClientRect(&cr);

	DWORD dwWidth		= cr.Width() / 2;
	DWORD dwWidthChunk	= cr.Width() % 2;

	m_sourcesListCtrl.SetResourceHandle(g_hModule);
	m_sourcesListCtrl.InsertColumn(0, LocalizeStr("Source")	, LVCFMT_LEFT, dwWidth);
	m_sourcesListCtrl.InsertColumn(1, LocalizeStr("Value")	, LVCFMT_LEFT, dwWidth + dwWidthChunk);

	CHeaderCtrl* pHeader = m_sourcesListCtrl.GetHeaderCtrl();

	DWORD dwStyle = GetWindowLong(pHeader->m_hWnd ,GWL_STYLE );
	dwStyle &= ~HDS_FULLDRAG;
	SetWindowLong(pHeader->m_hWnd  , GWL_STYLE, dwStyle );

	m_sourcesListCtrl.EnableSecondColumn(TRUE);
	m_sourcesListCtrl.EnableItemSelection(FALSE);
	m_sourcesListCtrl.EnableMultiSelection(TRUE);
	m_sourcesListCtrl.DrawIcons(TRUE);

	m_sourcesListCtrl.AddIcon(IDI_RTSS);
	m_sourcesListCtrl.AddIcon(IDI_AIDA);
	m_sourcesListCtrl.AddIcon(IDI_HWINFO);
	m_sourcesListCtrl.AddIcon(IDI_MSIAB);
	m_sourcesListCtrl.AddIcon(IDI_PERFCOUNTER);

	InitSourcesList();

	m_nTimerID = SetTimer(0x1234, 1000, NULL);

	UpdateButtons();

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::OnDestroy()
{
	if (m_nTimerID)
		KillTimer(m_nTimerID);

	m_nTimerID = NULL;

	MSG msg; 
	while (PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE));

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::UpdateButtons()
{
	BOOL bEnable = FALSE;

	int nSelectedItem = m_sourcesListCtrl.GetFocusedItem();

	if (nSelectedItem != -1)
		bEnable = TRUE;

	GetDlgItem(IDC_SETUP_SOURCE_BUTTON)->EnableWindow(bEnable);
	GetDlgItem(IDC_REMOVE_SOURCE_BUTTON)->EnableWindow(bEnable);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::InitSourcesList()
{
	m_sourcesListCtrl.SetRedraw(FALSE);
	m_sourcesListCtrl.DeleteAllItems();

	DWORD dwSource = 0;

	POSITION pos = m_sources.GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = m_sources.GetNext(pos);

		CString strName			= lpSource->GetName();
		CString strProvider		= lpSource->GetProviderName();

		DWORD dwIcon = 0;

		if (!strcmp(strProvider, PROVIDER_HAL))
			dwIcon = 2;
		else
		if (!strcmp(strProvider, PROVIDER_AIDA))
			dwIcon = 3;
		else
		if (!strcmp(strProvider, PROVIDER_HWINFO))
			dwIcon = 4;
		else
		if (!strcmp(strProvider, PROVIDER_MSIAB))
			dwIcon = 5;
		else
		if (!strcmp(strProvider, PROVIDER_PERFCOUNTER))
			dwIcon = 6;
		else
		if (!strcmp(strProvider, PROVIDER_INTERNAL))
			continue;

		int iItem = m_sourcesListCtrl.InsertItem(dwSource, strName);

		m_sourcesListCtrl.SetItemData(iItem, dwSource + (dwIcon<<24));

		dwSource++;
	}

	m_sourcesListCtrl.Init();
	SelectSources();
	m_sourcesListCtrl.SetRedraw(TRUE);
	UpdateSourcesList();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::SelectSources()
{
	int iFocus = -1;

	int iItems = m_sourcesListCtrl.GetItemCount();

	for (int iItem=0; iItem<iItems; iItem++)
	{
		COverlayDataSource* lpSource = GetItemSource(iItem);

		if (lpSource)
		{
			if (lpSource->m_bSelected)
			{
				if (iFocus == -1)
				{
					m_sourcesListCtrl.SetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

					iFocus = iItem;
				}
				else
					m_sourcesListCtrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);

				lpSource->m_bSelected = FALSE;
			}
		}
	}

	if (iFocus != -1)
		m_sourcesListCtrl.EnsureVisible(iFocus, FALSE);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::UpdateSourcesList()
{
	m_sourcesListCtrl.SetRedraw(FALSE);

	m_sources.UpdateSources();

	for (int iItem=0; iItem<m_sourcesListCtrl.GetItemCount(); iItem++)
	{
		COverlayDataSource*	lpSource = GetItemSource(iItem);

		if (lpSource)
		{
			FLOAT fltData = lpSource->GetData();

			CString strData;

			if (fltData != FLT_MAX)
			{
				strData = lpSource->GetDataStr();
				strData += " ";
				strData += lpSource->GetUnits();
			}
			else
				strData = LocalizeStr("N/A");

			m_sourcesListCtrl.SetItemText(iItem, 0, lpSource->GetName());
			m_sourcesListCtrl.SetItemText(iItem, 1, strData);
		}
	}

	m_sourcesListCtrl.SetRedraw(TRUE);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (!m_sourcesListCtrl.IsHeaderDragging())
		UpdateSourcesList();

	CDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::OnBnClickedSetupSourceButton()
{
	int iSelectedItem = m_sourcesListCtrl.GetFocusedItem();

	if (iSelectedItem != -1)
	{
		COverlayDataSource* lpSource = GetItemSource(iSelectedItem);

		if (lpSource)
		{
			if (!strcmp(lpSource->GetProviderName(), PROVIDER_HAL))
			{
				COverlayDataSourceHAL* lpSourceHAL = dynamic_cast<COverlayDataSourceHAL*>(lpSource);

				if (lpSourceHAL)
				{
					COverlayDataSourceSetupHALDlg dlg;
					dlg.SetSource(lpSourceHAL);

					if (IDOK == dlg.DoModal())
						UpdateSourcesList();
				}
			}
			else
			if (!strcmp(lpSource->GetProviderName(), PROVIDER_AIDA))
			{
				COverlayDataSourceAIDA* lpSourceAIDA = dynamic_cast<COverlayDataSourceAIDA*>(lpSource);

				if (lpSourceAIDA)
				{
					COverlayDataSourceSetupAIDADlg dlg;
					dlg.SetSource(lpSourceAIDA);

					if (IDOK == dlg.DoModal())
						UpdateSourcesList();
				}
			}
			else
			if (!strcmp(lpSource->GetProviderName(), PROVIDER_HWINFO))
			{
				COverlayDataSourceHwInfo* lpSourceHwInfo = dynamic_cast<COverlayDataSourceHwInfo*>(lpSource);

				if (lpSourceHwInfo)
				{
#ifdef HWINFO_SDK_AVAILABLE
					COverlayDataSourceSetupHwInfoDlg dlg;
					dlg.SetSource(lpSourceHwInfo);

					if (IDOK == dlg.DoModal())
						UpdateSourcesList();
#endif
				}
			}
			else
			if (!strcmp(lpSource->GetProviderName(), PROVIDER_MSIAB))
			{
				COverlayDataSourceMSIAB* lpSourceMSIAB = dynamic_cast<COverlayDataSourceMSIAB*>(lpSource);

				if (lpSourceMSIAB)
				{
					COverlayDataSourceSetupMSIABDlg dlg;
					dlg.SetSource(lpSourceMSIAB);

					if (IDOK == dlg.DoModal())
						UpdateSourcesList();
				}
			}
			else
			if (!strcmp(lpSource->GetProviderName(), PROVIDER_PERFCOUNTER))
			{
				COverlayDataSourcePerfCounter* lpSourcePerfCounter = dynamic_cast<COverlayDataSourcePerfCounter*>(lpSource);

				if (lpSourcePerfCounter)
				{
					COverlayDataSourceSetupPerfCounterDlg dlg;
					dlg.SetSource(lpSourcePerfCounter);

					if (IDOK == dlg.DoModal())
						UpdateSourcesList();
				}

				COverlayDataProvider* lpProvider = lpSourcePerfCounter->GetProvider();

				COverlayDataProviderPerfCounter* lpProviderPerfCounter = dynamic_cast<COverlayDataProviderPerfCounter*>(lpProvider);

				if (lpProviderPerfCounter)
					lpProviderPerfCounter->ReinitCounter(lpSourcePerfCounter);

			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::OnBnClickedAddSourceButton()
{
	if (GetAsyncKeyState(VK_CONTROL) < 0)
	{
		int iAdded	= 0;
		int iItems	= m_sourcesListCtrl.GetItemCount();

		for (int iItem=0; iItem<iItems; iItem++)
		{
			if (m_sourcesListCtrl.GetItemState(iItem, LVIS_SELECTED))
			{
				COverlayDataSource* lpSource = GetItemSource(iItem);

				if (lpSource)
				{
					if (!strcmp(lpSource->GetProviderName(), PROVIDER_HAL))
					{
						COverlayDataSourceHAL* lpSourceHAL	= dynamic_cast<COverlayDataSourceHAL*>(lpSource);
						COverlayDataSourceHAL* lpSourceCopy	= new COverlayDataSourceHAL;

						lpSourceCopy->Copy(lpSourceHAL);
						lpSourceCopy->m_bSelected = TRUE;

						m_sources.AddSource(lpSourceCopy);
						iAdded++;
					}
					else
					if (!strcmp(lpSource->GetProviderName(), PROVIDER_AIDA))
					{
						COverlayDataSourceAIDA* lpSourceAIDA = dynamic_cast<COverlayDataSourceAIDA*>(lpSource);
						COverlayDataSourceAIDA* lpSourceCopy = new COverlayDataSourceAIDA;

						lpSourceCopy->Copy(lpSourceAIDA);

						lpSourceCopy->Copy(lpSourceAIDA);
						lpSourceCopy->m_bSelected = TRUE;

						m_sources.AddSource(lpSourceCopy);
						iAdded++;
					}
					else
					if (!strcmp(lpSource->GetProviderName(), PROVIDER_HWINFO))
					{
						COverlayDataSourceHwInfo* lpSourceHwInfo	= dynamic_cast<COverlayDataSourceHwInfo*>(lpSource);
						COverlayDataSourceHwInfo* lpSourceCopy		= new COverlayDataSourceHwInfo;

						lpSourceCopy->Copy(lpSourceHwInfo);
						lpSourceCopy->m_bSelected = TRUE;

						if (GetAsyncKeyState(VK_SHIFT) < 0)
						{
							DWORD dwInstance = lpSourceCopy->GetSensorInst();

							if (dwInstance != 0xFFFFFFFF)
								lpSourceCopy->SetSensorInst(dwInstance + 1);
						}

						m_sources.AddSource(lpSourceCopy);
						iAdded++;
					}
					else
					if (!strcmp(lpSource->GetProviderName(), PROVIDER_MSIAB))
					{
						COverlayDataSourceMSIAB* lpSourceMSIAB	= dynamic_cast<COverlayDataSourceMSIAB*>(lpSource);
						COverlayDataSourceMSIAB* lpSourceCopy	= new COverlayDataSourceMSIAB;

						lpSourceCopy->Copy(lpSourceMSIAB);
						lpSourceCopy->m_bSelected = TRUE;

						if (GetAsyncKeyState(VK_SHIFT) < 0)
						{
							DWORD dwInstance = lpSourceCopy->GetGpu();

							if (dwInstance != 0xFFFFFFFF)
								lpSourceCopy->SetGpu(dwInstance + 1);
						}

						m_sources.AddSource(lpSourceCopy);
						iAdded++;
					}
					else
					if (!strcmp(lpSource->GetProviderName(), PROVIDER_PERFCOUNTER))
					{
						COverlayDataSourcePerfCounter* lpSourcePerfCounter	= dynamic_cast<COverlayDataSourcePerfCounter*>(lpSource);
						COverlayDataSourcePerfCounter* lpSourceCopy			= new COverlayDataSourcePerfCounter;
 
						lpSourceCopy->Copy(lpSourcePerfCounter);
						lpSourceCopy->m_bSelected = TRUE;

						if (GetAsyncKeyState(VK_SHIFT) < 0)
						{
							DWORD dwInstance = lpSourceCopy->GetInstanceIndex();

							if (dwInstance != 0xFFFFFFFF)
								lpSourceCopy->SetInstanceIndex(dwInstance + 1);
						}

						m_sources.AddSource(lpSourceCopy);
						iAdded++;
					}
				}
			}
		}

		if (iAdded)
			InitSourcesList();
	}
	else
	{
		COverlayDataSourceAddDlg dlg;

		dlg.SetSources(&m_sources);

		if (IDOK == dlg.DoModal())
		{
			InitSourcesList();
			UpdateButtons();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::OnBnClickedRemoveSourceButton()
{
	int iItems	= m_sourcesListCtrl.GetItemCount();

	for (int iItem=iItems-1; iItem>=0; iItem--)
	{
		if (m_sourcesListCtrl.GetItemState(iItem, LVIS_SELECTED))
		{
			COverlayDataSource* lpSource = GetItemSource(iItem);

			m_sources.RemoveSource(lpSource);

			m_sourcesListCtrl.DeleteItem(iItem);
		}
	}

	m_sources.UpdateSourceIDs();

	int iFocus = m_sourcesListCtrl.GetFocusedItem();

	InitSourcesList();

	if (iFocus != -1)
		m_sourcesListCtrl.SetItemState(iFocus, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	UpdateButtons();
}
//////////////////////////////////////////////////////////////////////
LRESULT COverlayDataSourcesDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_SEL_CHANGED)
		UpdateButtons();

	if (message == UM_DBLCLICK_ITEM)
		OnBnClickedSetupSourceButton();

	if (message == UM_DROP_ITEM)
	{	
		//create reordering list 

		CList<COverlayDataSource*, COverlayDataSource*> order;

		int iItems = m_sourcesListCtrl.GetItemCount();
	
		for (int iItem=0; iItem<iItems; iItem++)
		{
			COverlayDataSource* lpSource = GetItemSource(iItem);

			if (m_sourcesListCtrl.GetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED))
				lpSource->m_bSelected = TRUE;

			order.AddTail(lpSource);
		}

		//reorder data sources 

		m_sources.Reorder(&order);

		InitSourcesList();
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourcesDlg::OnBnClickedOk()
{
	m_lpOverlay->GetExternalDataSources()->Copy(&m_sources);

	OnOK();
}
//////////////////////////////////////////////////////////////////////
COverlayDataSource* COverlayDataSourcesDlg::GetItemSource(int iItem)
{
	return m_sources.FindSource(m_sourcesListCtrl.GetItemData(iItem) & 0xFFFFFF);
}
//////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourcesDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_DELETE)
		{
			if (pMsg->hwnd == m_sourcesListCtrl.m_hWnd)
				OnBnClickedRemoveSourceButton();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
//////////////////////////////////////////////////////////////////////
