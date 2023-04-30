// OverlayDataSourceAddDlg.cpp : implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayDataSourceAddDlg.h"
#include "OverlayDataSourceHAL.h"
#include "OverlayDataSourceAIDA.h"
#include "OverlayDataSourceHwInfo.h"
#include "OverlayDataSourceMSIAB.h"
#include "OverlayDataSourcePerfCounter.h"
#include "MultiString.h"
#include "OverlayDataProviderSetupHALDlg.h"

#include <shlwapi.h>
#include <float.h>
//////////////////////////////////////////////////////////////////////
#define PROVIDER_INDEX_HAL			0
#define PROVIDER_INDEX_AIDA			1
#define PROVIDER_INDEX_HWINFO		2
#define PROVIDER_INDEX_MSIAB		3
#define PROVIDER_INDEX_PERFCOUNTER	4
//////////////////////////////////////////////////////////////////////
// COverlayDataSourceAddDlg dialog
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayDataSourceAddDlg, CDialog)
//////////////////////////////////////////////////////////////////////
COverlayDataSourceAddDlg::COverlayDataSourceAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COverlayDataSourceAddDlg::IDD, pParent)
{
	m_lpSources = NULL;
	m_nTimerID	= 0;

	m_nProvider	= 0;
}
//////////////////////////////////////////////////////////////////////
COverlayDataSourceAddDlg::~COverlayDataSourceAddDlg()
{
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOURCES_LIST, m_sourcesListCtrl);
	DDX_CBIndex(pDX, IDC_PROVIDER_COMBO, m_nProvider);
	DDX_Control(pDX, IDC_PROVIDER_COMBO, m_ctrlProviderCombo);
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayDataSourceAddDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_PROVIDER_COMBO, &COverlayDataSourceAddDlg::OnCbnSelchangeProviderCombo)
	ON_BN_CLICKED(IDC_EXPORT_BUTTON, &COverlayDataSourceAddDlg::OnBnClickedExportButton)
	ON_BN_CLICKED(IDOK, &COverlayDataSourceAddDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_DATA_PROVIDER_SETUP, &COverlayDataSourceAddDlg::OnBnClickedDataProviderSetup)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// COverlayDataSourceAddDlg message handlers
//////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceAddDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	LocalizeWnd(m_hWnd);
	AdjustWindowPos(this, GetParent());

	m_sourcesListCtrl.ShowScrollBar(SB_VERT, TRUE);

	CRect cr; m_sourcesListCtrl.GetClientRect(&cr);

	DWORD dwWidth		= cr.Width() / 2;
	DWORD dwWidthChunk	= cr.Width() % 2;

	m_sourcesListCtrl.SetResourceHandle(g_hModule);
	m_sourcesListCtrl.InsertColumn(0, LocalizeStr("Sensor")	, LVCFMT_LEFT, dwWidth);
	m_sourcesListCtrl.InsertColumn(1, LocalizeStr("Value")	, LVCFMT_LEFT, dwWidth + dwWidthChunk);

	CHeaderCtrl* pHeader = m_sourcesListCtrl.GetHeaderCtrl();

	DWORD dwStyle = GetWindowLong(pHeader->m_hWnd ,GWL_STYLE );
	dwStyle &= ~HDS_FULLDRAG;
	SetWindowLong(pHeader->m_hWnd  , GWL_STYLE, dwStyle );

	m_sourcesListCtrl.DrawRootButtons(TRUE);
	m_sourcesListCtrl.EnableMultiSelection(TRUE);

	InitSourcesList();

	m_imageList.SetResourceHandle(g_hModule);
	m_imageList.Create();
	m_imageList.Add(IDI_RTSS);
	m_imageList.Add(IDI_AIDA);
	m_imageList.Add(IDI_HWINFO);
	m_imageList.Add(IDI_MSIAB);
	m_imageList.Add(IDI_PERFCOUNTER);
	m_ctrlProviderCombo.SetImageList(&m_imageList);

	AddProvider(LocalizeStr("Internal HAL")					, PROVIDER_INDEX_HAL		);
	AddProvider("AIDA64"									, PROVIDER_INDEX_AIDA		);
	AddProvider("HwINFO64"									, PROVIDER_INDEX_HWINFO		);
	AddProvider("MSI Afterburner"							, PROVIDER_INDEX_MSIAB		);
	AddProvider(LocalizeStr("Windows performance counter")	, PROVIDER_INDEX_PERFCOUNTER);

	m_ctrlProviderCombo.SetCurSel(0);

	UpdateProviderControls();

	m_nTimerID = SetTimer(0x1234, 1000, NULL);

	return TRUE; 
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::AddProvider(CString strName, int nIcon)
{
	COMBOBOXEXITEM cbi;
	ZeroMemory(&cbi, sizeof(cbi));

	cbi.mask			= CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;
	cbi.iItem			= m_ctrlProviderCombo.GetCount();
	cbi.pszText			= (LPTSTR)(LPCTSTR)strName;
	cbi.cchTextMax		= strName.GetLength();
	cbi.iImage			= nIcon;
	cbi.iSelectedImage	= nIcon;
	cbi.iOverlay		= nIcon;

	m_ctrlProviderCombo.InsertItem(&cbi);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::OnDestroy()
{
	if (m_nTimerID)
		KillTimer(m_nTimerID);

	m_nTimerID = NULL;

	MSG msg; 
	while (PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE));

	CDialog::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (!m_sourcesListCtrl.IsHeaderDragging())
	{
		if (m_sourcesListCtrl.GetItemCount())
			UpdateSourcesList();
		else
			InitSourcesList();
	}

	CDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::SetSources(COverlayDataSourcesList* lpSources)
{
	m_lpSources = lpSources;
}
//////////////////////////////////////////////////////////////////////
PLC_ITEM_DESC* COverlayDataSourceAddDlg::GetRootItem(LPCSTR lpText)
{
	CList<PLC_ITEM_DESC*,PLC_ITEM_DESC*>* lpItemsList = m_sourcesListCtrl.GetItemsList();
	
	POSITION pos = lpItemsList->GetHeadPosition();

	while (pos)
	{
		PLC_ITEM_DESC* lpItem = lpItemsList->GetNext(pos);

		if (!_stricmp(lpItem->szText, lpText))
			return lpItem;
	}

	return m_sourcesListCtrl.InsertItem(PLC_ITEM_FLAG_EXPANDED, lpText, "", NULL, 0, 0, 0, 0);	
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::InitSourcesList()
{
	m_sourcesListCtrl.DeleteAllItems();
	m_sourcesListCtrl.Uninit();

	switch (m_nProvider)
	{
	case PROVIDER_INDEX_HAL:
		InitSourcesListHAL();
		break;
	case PROVIDER_INDEX_AIDA:
		InitSourcesListAIDA();
		break;
	case PROVIDER_INDEX_HWINFO:
		InitSourcesListHwInfo();
		break;
	case PROVIDER_INDEX_MSIAB:
		InitSourcesListMSIAB();
		break;
	case PROVIDER_INDEX_PERFCOUNTER:
		InitSourcesListPerfCounter();
		break;
	}
}
//////////////////////////////////////////////////////////////////////
typedef struct AIDA64_SENSOR_TYPE_DESC
{
	LPCSTR lpType;
	LPCSTR lpName;
	LPCSTR lpUnits;
} AIDA64_SENSOR_TYPE_DESC, *LPAIDA64_SENSOR_TYPE_DESC;
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::ParseSensor(LPCSTR lpType, LPCSTR lpID, LPCSTR lpLabel, LPCSTR lpValue, DWORD dwContext)
{
	switch (dwContext)
	{
	case PARSE_SENSOR_CONTEXT_INIT:
		{
			CString strName		= lpType;
			CString strUnits	= "";

			AIDA64_SENSOR_TYPE_DESC sensorTypes[] = 
			{
				{ "temp"			, "Temperature"		, "°C"	},
				{ "fan"				, "Fan tachometer"	, "RPM"	},
				{ "duty"			, "Fan speed"		, "%"	},
				{ "volt"			, "Voltage"			, "V"	},
				{ "pwr"				, "Power"			, "W"	}
			};

			for (int iIndex=0; iIndex<_countof(sensorTypes); iIndex++)
			{
				if (!_stricmp(sensorTypes[iIndex].lpType, lpType))
				{
					strName		= sensorTypes[iIndex].lpName;
					strUnits	= sensorTypes[iIndex].lpUnits;
					break;
				}
			}

			PLC_ITEM_DESC* lpRoot	= GetRootItem(strName);
			LPCSTR lpCachedID		= m_cache.GetAt(m_cache.AddTail(lpID));
			LPCSTR lpCachedUnits	= m_cache.GetAt(m_cache.AddTail(strUnits));

			CString strValue;
			strValue.Format("%s %s", lpValue, strUnits);

			m_sourcesListCtrl.InsertItem(PLC_ITEM_FLAG_PROPERTY_COLUMN, lpLabel, strValue, lpRoot, (DWORD)lpCachedID, (DWORD)lpCachedUnits, 0, 0);
		}
		break;
	case PARSE_SENSOR_CONTEXT_UPDATE:
		{
			CList<PLC_ITEM_DESC*,PLC_ITEM_DESC*>* lpItemsList = m_sourcesListCtrl.GetItemsList();
			
			POSITION pos = lpItemsList->GetHeadPosition();

			while (pos)
			{
				PLC_ITEM_DESC* lpItemDesc = lpItemsList->GetNext(pos);

				LPCSTR lpItemID		= (LPCSTR)lpItemDesc->dwUserData0;
				LPCSTR lpItemUnits	= (LPCSTR)lpItemDesc->dwUserData1;

				if (lpItemID)
				{
					if (!_stricmp(lpID, lpItemID))
					{
						CString strValue;
						strValue.Format("%s %s", lpValue, lpItemUnits);
	
						strcpy_s(lpItemDesc->szPropertyText, sizeof(lpItemDesc->szPropertyText), strValue);
					}
				}
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::InitSourcesListHAL()
{
	CHALDataSourcesList* lpDataSources = g_hal.GetDataSourcesList();

	POSITION pos = lpDataSources->GetHeadPosition();

	while (pos)
	{
		CHALDataSource* lpSource = lpDataSources->GetNext(pos);

		FLOAT data = lpSource->GetData();

		CString strValue;

		if (data != FLT_MAX)
			strValue.Format("%.1f %s", data, lpSource->GetUnits());
		else
			strValue = LocalizeStr("N/A");

		PLC_ITEM_DESC* lpRoot = GetRootItem(lpSource->GetGroup());

		m_sourcesListCtrl.InsertItem(PLC_ITEM_FLAG_PROPERTY_COLUMN, lpSource->GetName(), strValue, lpRoot, (DWORD)lpSource, 0, 0, 0);

		lpSource->SetDirty(TRUE);
	}

	g_hal.UpdateSourcesAsync();

	m_sourcesListCtrl.Init();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::InitSourcesListAIDA()
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "AIDA64_SensorValues");

	if (hMapFile)
	{
		LPCSTR pMapAddr = (LPCSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pMapAddr)
		{
			CString strCache = pMapAddr;

			ParseXML(strCache, PARSE_SENSOR_CONTEXT_INIT);

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	m_sourcesListCtrl.Init();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::InitSourcesListHwInfo()
{
#ifdef HWINFO_SDK_AVAILABLE
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, HWiNFO_SENSORS_MAP_FILE_NAME2);

	if (hMapFile)
	{
		PHWiNFO_SENSORS_SHARED_MEM2 pMapAddr = (PHWiNFO_SENSORS_SHARED_MEM2)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pMapAddr)
		{
			for (DWORD dwSensor=0; dwSensor<pMapAddr->dwNumSensorElements; dwSensor++)
			{
				PHWiNFO_SENSORS_SENSOR_ELEMENT pSensor = (PHWiNFO_SENSORS_SENSOR_ELEMENT)((BYTE*)pMapAddr + pMapAddr->dwOffsetOfSensorSection + (pMapAddr->dwSizeOfSensorElement * dwSensor));

				PLC_ITEM_DESC* pRoot = m_sourcesListCtrl.InsertItem(PLC_ITEM_FLAG_EXPANDED, pSensor->szSensorNameOrig, "", NULL, pSensor->dwSensorInst, 0, 0, 0);

				for (DWORD dwReading=0; dwReading<pMapAddr->dwNumReadingElements; dwReading++)
				{
					PHWiNFO_SENSORS_READING_ELEMENT pReading = (PHWiNFO_SENSORS_READING_ELEMENT)((BYTE*)pMapAddr + pMapAddr->dwOffsetOfReadingSection + (pMapAddr->dwSizeOfReadingElement * dwReading));

					LPCSTR lpCachedUnits = m_cache.GetAt(m_cache.AddTail(pReading->szUnit));

					CString strValue;
					strValue.Format("%.1f %s", pReading->Value, lpCachedUnits);

					if (pReading->dwSensorIndex == dwSensor)
						m_sourcesListCtrl.InsertItem(PLC_ITEM_FLAG_PROPERTY_COLUMN, pReading->szLabelOrig, strValue, pRoot, pSensor->dwSensorInst, pReading->tReading, (DWORD)lpCachedUnits, 0);
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	m_sourcesListCtrl.Init();
#endif
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::InitSourcesListMSIAB()
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MAHMSharedMemory");

	if (hMapFile)
	{
		LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);;
		
		if (pMapAddr)
		{
			LPMAHM_SHARED_MEMORY_HEADER lpHeader = (LPMAHM_SHARED_MEMORY_HEADER)pMapAddr;

			if (lpHeader->dwSignature == 'MAHM')
			{
				DWORD dwSources = lpHeader->dwNumEntries;

				for (DWORD dwSource=0; dwSource<dwSources; dwSource++)
				{
					LPMAHM_SHARED_MEMORY_ENTRY lpEntry = (LPMAHM_SHARED_MEMORY_ENTRY)((LPBYTE)lpHeader + lpHeader->dwHeaderSize + dwSource * lpHeader->dwEntrySize);

					LPCSTR lpCachedUnits = m_cache.GetAt(m_cache.AddTail(lpEntry->szSrcUnits));

					CString strValue;

					if (lpEntry->data != FLT_MAX)
						strValue.Format("%.1f %s", lpEntry->data, lpCachedUnits);
					else
						strValue = LocalizeStr("N/A");

					PLC_ITEM_DESC* lpRoot = GetRootItem(GetGroupName(lpEntry, FALSE));
	
					m_sourcesListCtrl.InsertItem(PLC_ITEM_FLAG_PROPERTY_COLUMN, lpEntry->szSrcName, strValue, lpRoot, lpEntry->dwSrcId, lpEntry->dwGpu, (DWORD)lpCachedUnits, 0);
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	m_sourcesListCtrl.Init();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::InitSourcesListPerfCounter()
{
	CWaitCursor waitCursor;

	m_sourcesListCtrl.RedrawWindow();

	if (m_objects.IsEmpty())
	{
		m_objects.Enum();
		m_objects.Sort(FALSE);
	}

	POSITION pos = m_objects.GetHeadPosition();

	while (pos)
	{
		LPPERFCOUNTER_OBJECT_DESC lpDesc = m_objects.GetNext(pos);

		char szEnglishObjectName[MAX_PATH];
		m_objects.GetEnglishName(lpDesc->szName, szEnglishObjectName, sizeof(szEnglishObjectName));
			//system enumerates object names in localized format, so we convert them back to English to provide language independent 
			//plugin settings

		PLC_ITEM_DESC* pRoot = m_sourcesListCtrl.InsertItem(0, szEnglishObjectName, "", NULL, 0, 0, 0, 0);

		if (lpDesc->lpCounters)
		{
			CMultiString mstr(lpDesc->lpCounters);

			LPCSTR lpCounter = mstr.GetNext();

			while (lpCounter)
			{
				char szEnglishCounterName[MAX_PATH];
				m_objects.GetEnglishName(lpCounter, szEnglishCounterName, sizeof(szEnglishCounterName));
					//system enumerates counter names in localized format, so we convert them back to English to provide language independent 
					//plugin settings

				m_sourcesListCtrl.InsertItem(0, szEnglishCounterName, "", pRoot, (DWORD)lpDesc, 0, 0, 0);

				lpCounter = mstr.GetNext();
			}
		}
	}

	waitCursor.Restore();

	m_sourcesListCtrl.Init();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::UpdateSourcesList()
{
	switch (m_nProvider)
	{
	case PROVIDER_INDEX_HAL:
		UpdateSourcesListHAL();
		break;
	case PROVIDER_INDEX_AIDA:
		UpdateSourcesListAIDA();
		break;
	case PROVIDER_INDEX_HWINFO:
		UpdateSourcesListHwInfo();
		break;
	case PROVIDER_INDEX_MSIAB:
		UpdateSourcesListMSIAB();
		break;
	case PROVIDER_INDEX_PERFCOUNTER:
		UpdateSourcesListPerfCounter();
		break;
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::UpdateSourcesListHAL()
{
	CList<PLC_ITEM_DESC*, PLC_ITEM_DESC*>* lpItemsList = m_sourcesListCtrl.GetItemsList();

	POSITION pos = lpItemsList->GetHeadPosition();

	while (pos)
	{
		PLC_ITEM_DESC* lpItemDesc = lpItemsList->GetNext(pos);

		if (lpItemDesc->pRoot)
		{
			CHALDataSource* lpSource = (CHALDataSource*)lpItemDesc->dwUserData0;

			if (GetAsyncKeyState(VK_CONTROL) < 0)
			{
				CString strTime;

				strTime.Format("%.1f ms", lpSource->GetPollingTime());

				strcpy_s(lpItemDesc->szPropertyText, sizeof(lpItemDesc->szPropertyText), strTime);
			}
			else
			{
				FLOAT data = lpSource->GetData();

				CString strValue;

				if (data != FLT_MAX)
					strValue.Format("%.1f %s", data, lpSource->GetUnits());
				else
					strValue = LocalizeStr("N/A");

				strcpy_s(lpItemDesc->szPropertyText, sizeof(lpItemDesc->szPropertyText), strValue);
			}

			lpSource->SetDirty(TRUE);
		}

		m_sourcesListCtrl.RedrawVisibleItems();
	}

	g_hal.UpdateSourcesAsync();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::UpdateSourcesListAIDA()
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "AIDA64_SensorValues");

	if (hMapFile)
	{
		LPCSTR pMapAddr = (LPCSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pMapAddr)
		{
			CString strCache = pMapAddr;

			ParseXML(strCache, PARSE_SENSOR_CONTEXT_UPDATE);

			UnmapViewOfFile(pMapAddr);

			m_sourcesListCtrl.RedrawVisibleItems();
		}

		CloseHandle(hMapFile);
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::UpdateSourcesListHwInfo()
{
#ifdef HWINFO_SDK_AVAILABLE
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, HWiNFO_SENSORS_MAP_FILE_NAME2);

	if (hMapFile)
	{
		PHWiNFO_SENSORS_SHARED_MEM2 pMapAddr = (PHWiNFO_SENSORS_SHARED_MEM2)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pMapAddr)
		{
			CList<PLC_ITEM_DESC*, PLC_ITEM_DESC*>* lpItemsList = m_sourcesListCtrl.GetItemsList();

			POSITION pos = lpItemsList->GetHeadPosition();

			while (pos)
			{
				PLC_ITEM_DESC* lpItemDesc = lpItemsList->GetNext(pos);

				if (lpItemDesc->pRoot)
				{
					LPCSTR	lpSensorName	= lpItemDesc->pRoot->szText;
					LPCSTR	lpReadingName	= lpItemDesc->szText;
					DWORD	dwReadingType	= lpItemDesc->dwUserData1;

					PHWiNFO_SENSORS_READING_ELEMENT lpReading = FindReadingElement(pMapAddr, lpSensorName, lpReadingName, dwReadingType);

					CString strValue;

					if (lpReading)
						strValue.Format("%.1f %s", lpReading->Value, lpReading->szUnit);
					else
						strValue = LocalizeStr("N/A");

					strcpy_s(lpItemDesc->szPropertyText, sizeof(lpItemDesc->szPropertyText), strValue);
				}
			}

			UnmapViewOfFile(pMapAddr);

			m_sourcesListCtrl.RedrawVisibleItems();
		}

		CloseHandle(hMapFile);
	}
#endif
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::UpdateSourcesListMSIAB()
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MAHMSharedMemory");

	if (hMapFile)
	{
		LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);;
		
		if (pMapAddr)
		{
			LPMAHM_SHARED_MEMORY_HEADER lpHeader = (LPMAHM_SHARED_MEMORY_HEADER)pMapAddr;

			if (lpHeader->dwSignature == 'MAHM')
			{
				CList<PLC_ITEM_DESC*, PLC_ITEM_DESC*>* lpItemsList = m_sourcesListCtrl.GetItemsList();

				POSITION pos = lpItemsList->GetHeadPosition();

				while (pos)
				{
					PLC_ITEM_DESC* lpItemDesc = lpItemsList->GetNext(pos);

					if (lpItemDesc->pRoot)
					{
						LPCSTR	lpSrcName = lpItemDesc->szText;

						LPMAHM_SHARED_MEMORY_ENTRY lpEntry = FindEntry(lpHeader, lpSrcName);

						CString strValue;

						if (lpEntry && (lpEntry->data != FLT_MAX))
							strValue.Format("%.1f %s", lpEntry->data, lpEntry->szSrcUnits);
						else
							strValue = LocalizeStr("N/A");

						strcpy_s(lpItemDesc->szPropertyText, sizeof(lpItemDesc->szPropertyText), strValue);
					}
				}

				m_sourcesListCtrl.RedrawVisibleItems();
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::UpdateSourcesListPerfCounter()
{
}
//////////////////////////////////////////////////////////////////////
PHWiNFO_SENSORS_READING_ELEMENT COverlayDataSourceAddDlg::FindReadingElement(PHWiNFO_SENSORS_SHARED_MEM2 pMem, LPCSTR lpSensorName, LPCSTR lpReadingName, DWORD dwReadingType)
{
#ifdef HWINFO_SDK_AVAILABLE
	for (DWORD dwReading=0; dwReading<pMem->dwNumReadingElements; dwReading++)
	{
		PHWiNFO_SENSORS_READING_ELEMENT pReading = (PHWiNFO_SENSORS_READING_ELEMENT)((BYTE*)pMem + pMem->dwOffsetOfReadingSection + (pMem->dwSizeOfReadingElement * dwReading));

		PHWiNFO_SENSORS_SENSOR_ELEMENT pSensor = (PHWiNFO_SENSORS_SENSOR_ELEMENT)((BYTE*)pMem + pMem->dwOffsetOfSensorSection + (pMem->dwSizeOfSensorElement * pReading->dwSensorIndex));

		if (!strcmp(pSensor->szSensorNameOrig, lpSensorName) && !strcmp(pReading->szLabelOrig, lpReadingName) && (pReading->tReading == dwReadingType))
			return pReading;
	}
#endif

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CString COverlayDataSourceAddDlg::GetGroupName(LPMAHM_SHARED_MEMORY_ENTRY lpEntry, BOOL bUseInstance)
{
	CString strGroup;

	switch (lpEntry->dwSrcId)
	{
	case MONITORING_SOURCE_ID_GPU_TEMPERATURE:
	case MONITORING_SOURCE_ID_GPU_TEMPERATURE2:
	case MONITORING_SOURCE_ID_GPU_TEMPERATURE3:
	case MONITORING_SOURCE_ID_GPU_TEMPERATURE4:
	case MONITORING_SOURCE_ID_GPU_TEMPERATURE5:
	case MONITORING_SOURCE_ID_FAN_SPEED:
	case MONITORING_SOURCE_ID_FAN_SPEED2:
	case MONITORING_SOURCE_ID_FAN_SPEED3:
	case MONITORING_SOURCE_ID_FAN_TACHOMETER:
	case MONITORING_SOURCE_ID_FAN_TACHOMETER2:
	case MONITORING_SOURCE_ID_FAN_TACHOMETER3:
	case MONITORING_SOURCE_ID_CORE_CLOCK:
	case MONITORING_SOURCE_ID_SHADER_CLOCK:
	case MONITORING_SOURCE_ID_GPU_USAGE:
	case MONITORING_SOURCE_ID_FB_USAGE:
	case MONITORING_SOURCE_ID_VID_USAGE:
	case MONITORING_SOURCE_ID_BUS_USAGE:
	case MONITORING_SOURCE_ID_GPU_VOLTAGE:
	case MONITORING_SOURCE_ID_AUX_VOLTAGE:
	case MONITORING_SOURCE_ID_AUX2_VOLTAGE:
	case MONITORING_SOURCE_ID_GPU_REL_POWER:
	case MONITORING_SOURCE_ID_GPU_ABS_POWER:
	case MONITORING_SOURCE_ID_PLUGIN_GPU:
		if (bUseInstance)
			strGroup.Format("GPU%d", lpEntry->dwGpu + 1);
		else
			strGroup = "GPU";
		break;
	case MONITORING_SOURCE_ID_PCB_TEMPERATURE:
	case MONITORING_SOURCE_ID_PCB_TEMPERATURE2:
	case MONITORING_SOURCE_ID_PCB_TEMPERATURE3:
	case MONITORING_SOURCE_ID_PCB_TEMPERATURE4:
	case MONITORING_SOURCE_ID_PCB_TEMPERATURE5:
		if (bUseInstance)
			strGroup.Format("PCB%d", lpEntry->dwGpu + 1);
		else
			strGroup = "PCB";
		break;
	case MONITORING_SOURCE_ID_MEM_TEMPERATURE:
	case MONITORING_SOURCE_ID_MEM_TEMPERATURE2:
	case MONITORING_SOURCE_ID_MEM_TEMPERATURE3:
	case MONITORING_SOURCE_ID_MEM_TEMPERATURE4:
	case MONITORING_SOURCE_ID_MEM_TEMPERATURE5:
	case MONITORING_SOURCE_ID_MEMORY_CLOCK:
	case MONITORING_SOURCE_ID_MEMORY_USAGE:
	case MONITORING_SOURCE_ID_MEMORY_VOLTAGE:
		if (bUseInstance)
			strGroup.Format("MEM%d", lpEntry->dwGpu + 1);
		else
			strGroup = "MEM";
		break;
	case MONITORING_SOURCE_ID_VRM_TEMPERATURE:
	case MONITORING_SOURCE_ID_VRM_TEMPERATURE2:
	case MONITORING_SOURCE_ID_VRM_TEMPERATURE3:
	case MONITORING_SOURCE_ID_VRM_TEMPERATURE4:
	case MONITORING_SOURCE_ID_VRM_TEMPERATURE5:
		if (bUseInstance)
			strGroup.Format("VRM%d", lpEntry->dwGpu + 1);
		else
			strGroup = "VRM";
		break;
	case MONITORING_SOURCE_ID_FRAMERATE:
	case MONITORING_SOURCE_ID_FRAMETIME:
	case MONITORING_SOURCE_ID_FRAMERATE_MIN:
	case MONITORING_SOURCE_ID_FRAMERATE_AVG:
	case MONITORING_SOURCE_ID_FRAMERATE_MAX:
	case MONITORING_SOURCE_ID_FRAMERATE_1DOT0_PERCENT_LOW:
	case MONITORING_SOURCE_ID_FRAMERATE_0DOT1_PERCENT_LOW:
		strGroup = "APP";
		break;
	case MONITORING_SOURCE_ID_GPU_TEMP_LIMIT:
	case MONITORING_SOURCE_ID_GPU_POWER_LIMIT:
	case MONITORING_SOURCE_ID_GPU_VOLTAGE_LIMIT:
	case MONITORING_SOURCE_ID_GPU_UTIL_LIMIT:
	case MONITORING_SOURCE_ID_GPU_SLI_SYNC_LIMIT:
		if (bUseInstance)
			strGroup.Format("LIM%d", lpEntry->dwGpu + 1);
		else
			strGroup = "LIM";
		break;
	case MONITORING_SOURCE_ID_CPU_TEMPERATURE:
	case MONITORING_SOURCE_ID_CPU_USAGE:
	case MONITORING_SOURCE_ID_CPU_CLOCK:
	case MONITORING_SOURCE_ID_CPU_POWER:
	case MONITORING_SOURCE_ID_PLUGIN_CPU:
		if (bUseInstance)
		{
			if (lpEntry->dwGpu == 0xFFFFFFFF)
				strGroup = "CPU";
			else
				strGroup.Format("CPU%d", lpEntry->dwGpu + 1);
		}
		else
			strGroup = "CPU";
		break;
	case MONITORING_SOURCE_ID_RAM_USAGE:
	case MONITORING_SOURCE_ID_PAGEFILE_USAGE:
	case MONITORING_SOURCE_ID_PLUGIN_RAM:
		strGroup = "RAM";
		break;
	case MONITORING_SOURCE_ID_PLUGIN_MOBO:
		strGroup = "MOBO";
		break;
	case MONITORING_SOURCE_ID_PLUGIN_HDD:
		if (bUseInstance)
			strGroup.Format("HDD%d", lpEntry->dwGpu + 1);
		else
			strGroup = "HDD";
		break;
	case MONITORING_SOURCE_ID_PLUGIN_NET:
		if (bUseInstance)
			strGroup.Format("NET%d", lpEntry->dwGpu + 1);
		else
			strGroup = "NET";
		break;
	case MONITORING_SOURCE_ID_PLUGIN_PSU:
		strGroup = "PSU";
		break;
	case MONITORING_SOURCE_ID_PLUGIN_UPS:
		strGroup = "UPS";
		break;
	case MONITORING_SOURCE_ID_PLUGIN_MISC:
	default:
		strGroup = "MISC";
		break;
	}

	return strGroup;
}
//////////////////////////////////////////////////////////////////////
LPMAHM_SHARED_MEMORY_ENTRY COverlayDataSourceAddDlg::FindEntry(LPMAHM_SHARED_MEMORY_HEADER lpHeader, LPCSTR lpSrcName)
{
	DWORD dwSources = lpHeader->dwNumEntries;

	for (DWORD dwSource=0; dwSource<dwSources; dwSource++)
	{
		LPMAHM_SHARED_MEMORY_ENTRY lpEntry = (LPMAHM_SHARED_MEMORY_ENTRY)((LPBYTE)lpHeader + lpHeader->dwHeaderSize + dwSource * lpHeader->dwEntrySize);

		if (!strcmp(lpEntry->szSrcName, lpSrcName))
			return lpEntry;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::OnCbnSelchangeProviderCombo()
{
	UpdateData(TRUE);

	InitSourcesList();

	UpdateProviderControls();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::OnBnClickedExportButton()
{
	CString strExport;

	switch (m_nProvider)
	{
	case PROVIDER_INDEX_HAL:
		strExport = ExportHAL();
		break;
	case PROVIDER_INDEX_AIDA:
		strExport = ExportAIDA();
		break;
	case PROVIDER_INDEX_HWINFO:
		strExport = ExportHwInfo();
		break;
	case PROVIDER_INDEX_MSIAB:
		strExport = ExportMSIAB();
		break;
	case PROVIDER_INDEX_PERFCOUNTER:
		strExport = ExportPerfCounter();
		break;
	}

	char szLogPath[MAX_PATH];
	GetModuleFileName(g_hModule, szLogPath, MAX_PATH);
	PathRemoveFileSpec(szLogPath);

	switch (m_nProvider)
	{
	case PROVIDER_INDEX_HAL:
		strcat_s(szLogPath, sizeof(szLogPath), "\\OverlayEditor_HAL.log");
		break;
	case PROVIDER_INDEX_AIDA:
		strcat_s(szLogPath, sizeof(szLogPath), "\\OverlayEditor_AIDA.log");
		break;
	case PROVIDER_INDEX_HWINFO:
		strcat_s(szLogPath, sizeof(szLogPath), "\\OverlayEditor_HwInfo.log");
		break;
	case PROVIDER_INDEX_MSIAB:
		strcat_s(szLogPath, sizeof(szLogPath), "\\OverlayEditor_MSIAB.log");
		break;
	case PROVIDER_INDEX_PERFCOUNTER:
		strcat_s(szLogPath, sizeof(szLogPath), "\\OverlayEditor_PerfCounter.log");
		break;
	}

	CStdioFile log;
	if (log.Open(szLogPath, CFile::modeCreate | CFile::modeWrite))
		log.WriteString(strExport);

	CString strMessage;
	strMessage.Format(LocalizeStr("Sensors exported to %s!"), szLogPath),
	MessageBox(strMessage, LocalizeStr("Export"), MB_OK);
}
//////////////////////////////////////////////////////////////////////
CString COverlayDataSourceAddDlg::ExportHAL()
{
	CString strExport;

	DWORD dwSource = 0;

	CHALDataSourcesList* lpDataSources = g_hal.GetDataSourcesList();

	POSITION pos = lpDataSources->GetHeadPosition();

	while (pos)
	{
		CHALDataSource* lpSource = lpDataSources->GetNext(pos);

		FLOAT data = lpSource->GetData();

		CString strBuf;

		strBuf.Format("Source%d\n"		, dwSource);
		strExport += strBuf;
		strBuf.Format("  Name\t\t%s\n"	, lpSource->GetName());
		strExport += strBuf;
		if (data != FLT_MAX)
			strBuf.Format("  Value\t\t%f\n"	, data);
		else
			strBuf.Format("  Value\t\t%s\n"	, "N/A");
		strExport += strBuf;
		strBuf.Format("  Units\t\t%s\n"	, lpSource->GetUnits());
		strExport += strBuf;

		dwSource++;
	}

	return strExport;
}
//////////////////////////////////////////////////////////////////////
CString COverlayDataSourceAddDlg::ExportAIDA()
{
	CString strExport;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "AIDA64_SensorValues");

	if (hMapFile)
	{
		LPCSTR pMapAddr = (LPCSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pMapAddr)
		{
			strExport = pMapAddr;

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return strExport;
}
//////////////////////////////////////////////////////////////////////
CString COverlayDataSourceAddDlg::ExportHwInfo()
{
	CString strExport;

#ifdef HWINFO_SDK_AVAILABLE
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, HWiNFO_SENSORS_MAP_FILE_NAME2);

	if (hMapFile)
	{
		PHWiNFO_SENSORS_SHARED_MEM2 pMapAddr = (PHWiNFO_SENSORS_SHARED_MEM2)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		
		if (pMapAddr)
		{
			for (DWORD dwSensor=0; dwSensor<pMapAddr->dwNumSensorElements; dwSensor++)
			{
				PHWiNFO_SENSORS_SENSOR_ELEMENT pSensor = (PHWiNFO_SENSORS_SENSOR_ELEMENT)((BYTE*)pMapAddr + pMapAddr->dwOffsetOfSensorSection + (pMapAddr->dwSizeOfSensorElement * dwSensor));

				CString strBuf;

				strBuf.Format("Sensor%d\n"		, dwSensor);
				strExport += strBuf;
				strBuf.Format("  Inst\t\t%d\n"	, pSensor->dwSensorInst);
				strExport += strBuf;
				strBuf.Format("  Name\t\t%s\n"	, pSensor->szSensorNameOrig);
				strExport += strBuf;

				for (DWORD dwReading=0; dwReading<pMapAddr->dwNumReadingElements; dwReading++)
				{
					PHWiNFO_SENSORS_READING_ELEMENT pReading = (PHWiNFO_SENSORS_READING_ELEMENT)((BYTE*)pMapAddr + pMapAddr->dwOffsetOfReadingSection	+ (pMapAddr->dwSizeOfReadingElement * dwReading));

					if (pReading->dwSensorIndex == dwSensor)
					{
						strBuf.Format("  Reading%d\n"	, dwReading);
						strExport += strBuf;
						strBuf.Format("    Type\t%d\n"	, pReading->tReading);
						strExport += strBuf;
						strBuf.Format("    Name\t%s\n"	, pReading->szLabelOrig);
						strExport += strBuf;
						strBuf.Format("    Value\t%f\n"	, pReading->Value);
						strExport += strBuf;
						strBuf.Format("    Units\t%s\n"	, pReading->szUnit);
						strExport += strBuf;
					}
				}
			}
				
			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}
#endif

	return strExport;
}
//////////////////////////////////////////////////////////////////////
CString COverlayDataSourceAddDlg::ExportMSIAB()
{
	CString strExport;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "MAHMSharedMemory");

	if (hMapFile)
	{
		LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);;
		
		if (pMapAddr)
		{
			LPMAHM_SHARED_MEMORY_HEADER lpHeader = (LPMAHM_SHARED_MEMORY_HEADER)pMapAddr;

			if (lpHeader->dwSignature == 'MAHM')
			{
				DWORD dwSources = lpHeader->dwNumEntries;

				for (DWORD dwSource=0; dwSource<dwSources; dwSource++)
				{
					LPMAHM_SHARED_MEMORY_ENTRY lpEntry = (LPMAHM_SHARED_MEMORY_ENTRY)((LPBYTE)lpHeader + lpHeader->dwHeaderSize + dwSource * lpHeader->dwEntrySize);

					CString strBuf;

					strBuf.Format("Source%d\n"		, dwSource);
					strExport += strBuf;
					strBuf.Format("  Name\t\t%s\n"	, lpEntry->szSrcName);
					strExport += strBuf;
					strBuf.Format("  SrcId\t\t%08X\n", lpEntry->dwSrcId);
					strExport += strBuf;
					strBuf.Format("  Gpu\t\t%d\n"	, lpEntry->dwGpu);
					strExport += strBuf;
					if (lpEntry->data != FLT_MAX)
						strBuf.Format("  Value\t\t%f\n"	, lpEntry->data);
					else
						strBuf.Format("  Value\t\t%s\n"	, "N/A");
					strExport += strBuf;
					strBuf.Format("  Units\t\t%s\n"	, lpEntry->szSrcUnits);
					strExport += strBuf;
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return strExport;
}
//////////////////////////////////////////////////////////////////////
CString COverlayDataSourceAddDlg::ExportPerfCounter()
{
	CWaitCursor waitCursor;

	CString strExport;

	POSITION pos = m_objects.GetHeadPosition();

	while (pos)
	{
		LPPERFCOUNTER_OBJECT_DESC lpDesc = m_objects.GetNext(pos);

		char szEnglishObjectName[MAX_PATH];
		m_objects.GetEnglishName(lpDesc->szName, szEnglishObjectName, sizeof(szEnglishObjectName));
			//system enumerates object names in localized format, so we convert them back to English to provide language independent 
			//plugin settings
			
		CString strBuf;

		strBuf.Format("Object %s\n", szEnglishObjectName);
		strExport += strBuf;

		if (lpDesc->lpCounters)
		{
			strExport += "  Counters\n";

			CMultiString mstr(lpDesc->lpCounters);

			LPCSTR lpCounter = mstr.GetNext();

			while (lpCounter)
			{
				char szEnglishCounterName[MAX_PATH];
				m_objects.GetEnglishName(lpCounter, szEnglishCounterName, sizeof(szEnglishCounterName));
					//system enumerates counter names in localized format, so we convert them back to English to provide language independent 
					//plugin settings

				strBuf.Format("    %s\n", szEnglishCounterName);
				strExport += strBuf;

				lpCounter = mstr.GetNext();
			}
		}

		if (lpDesc->lpInstances)
		{
			strExport += "  Instances\n";

			CMultiString mstr(lpDesc->lpInstances);

			LPCSTR lpInstance = mstr.GetNext();

			while (lpInstance)
			{
				char szEnglishInstanceName[MAX_PATH];
				m_objects.GetEnglishName(lpInstance, szEnglishInstanceName, sizeof(szEnglishInstanceName));
					//system enumerates instamce names in localized format, so we convert them back to English to provide language independent 
					//plugin settings

				strBuf.Format("    %s\n", szEnglishInstanceName);
				strExport += strBuf;

				lpInstance = mstr.GetNext();
			}
		}
	}

	waitCursor.Restore();

	return strExport;
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::OnBnClickedOk()
{
	int				nSelCount			= m_sourcesListCtrl.GetSelCount();
	int				nFocusedItem		= m_sourcesListCtrl.GetFocusedItem();
	PLC_ITEM_DESC*	lpFocusedItemDesc	= NULL;

	if (!nSelCount && (nFocusedItem != -1))
	{
		lpFocusedItemDesc = (PLC_ITEM_DESC*)m_sourcesListCtrl.GetItemData(nFocusedItem);

		if (lpFocusedItemDesc->dwFlags & PLC_ITEM_FLAG_ROOT)
			lpFocusedItemDesc = NULL;
	}

	if (m_lpSources)
	{
		CList<PLC_ITEM_DESC*, PLC_ITEM_DESC*>* lpItemsList = m_sourcesListCtrl.GetItemsList();

		POSITION pos = lpItemsList->GetHeadPosition();

		while (pos)
		{
			PLC_ITEM_DESC* lpItemDesc = lpItemsList->GetNext(pos);

			if ((lpItemDesc->dwFlags & PLC_ITEM_FLAG_SELECTED) || (lpItemDesc == lpFocusedItemDesc))
			{
				switch (m_nProvider)
				{
				case PROVIDER_INDEX_HAL:
					AddSourceHAL(lpItemDesc);
					break;
				case PROVIDER_INDEX_AIDA:
					AddSourceAIDA(lpItemDesc);
					break;
				case PROVIDER_INDEX_HWINFO:
					AddSourceHwInfo(lpItemDesc);
					break;
				case PROVIDER_INDEX_MSIAB:
					AddSourceMSIAB(lpItemDesc);
					break;
				case PROVIDER_INDEX_PERFCOUNTER:
					AddSourcePerfCounter(lpItemDesc);
					break;
				}
			}
		}
	}

	OnOK();
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::AddSourceHAL(PLC_ITEM_DESC* lpItemDesc)
{
	COverlayDataSourceHAL* lpSource = new COverlayDataSourceHAL;

	//HAL specific properties

	CHALDataSource* lpHALSource = (CHALDataSource*)lpItemDesc->dwUserData0;

	lpSource->SetID(lpHALSource->GetName());

	//common properties

	lpSource->SetName(lpHALSource->GetName());
	lpSource->SetUnits(lpHALSource->GetUnits());
	lpSource->m_bSelected = TRUE;

	m_lpSources->AddSource(lpSource);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::AddSourceAIDA(PLC_ITEM_DESC* lpItemDesc)
{
	COverlayDataSourceAIDA* lpSource = new COverlayDataSourceAIDA;

	//AIDA specific properties

	lpSource->SetID((LPCSTR)lpItemDesc->dwUserData0);

	//common properties

	lpSource->SetName(lpItemDesc->szText);
	lpSource->SetUnits((LPCSTR)lpItemDesc->dwUserData1);
	lpSource->m_bSelected = TRUE;

	m_lpSources->AddSource(lpSource);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::AddSourceHwInfo(PLC_ITEM_DESC* lpItemDesc)
{
	COverlayDataSourceHwInfo* lpSource = new COverlayDataSourceHwInfo;

	//HwInfo specific properties

	lpSource->SetSensorInst(lpItemDesc->dwUserData0);
	lpSource->SetReadingType(lpItemDesc->dwUserData1);
	lpSource->SetReadingName(lpItemDesc->szText);

	//common properties

	lpSource->SetName(lpItemDesc->szText);
	lpSource->SetUnits((LPCSTR)lpItemDesc->dwUserData2);
	lpSource->m_bSelected = TRUE;

	m_lpSources->AddSource(lpSource);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::AddSourceMSIAB(PLC_ITEM_DESC* lpItemDesc)
{
	COverlayDataSourceMSIAB* lpSource = new COverlayDataSourceMSIAB;

	//MSI AB specific properties

	lpSource->SetSrcId(lpItemDesc->dwUserData0);
	lpSource->SetGpu(lpItemDesc->dwUserData1);

	switch (lpItemDesc->dwUserData0)
	{
	case MONITORING_SOURCE_ID_PLUGIN_GPU:
	case MONITORING_SOURCE_ID_PLUGIN_CPU:
	case MONITORING_SOURCE_ID_PLUGIN_MOBO:
	case MONITORING_SOURCE_ID_PLUGIN_RAM:
	case MONITORING_SOURCE_ID_PLUGIN_HDD:
	case MONITORING_SOURCE_ID_PLUGIN_NET:
	case MONITORING_SOURCE_ID_PLUGIN_PSU:
	case MONITORING_SOURCE_ID_PLUGIN_UPS:
	case MONITORING_SOURCE_ID_PLUGIN_MISC:
		lpSource->SetSrcName(lpItemDesc->szText);
		break;
	}

	//common properties

	lpSource->SetName(lpItemDesc->szText);
	lpSource->SetUnits((LPCSTR)lpItemDesc->dwUserData2);
	lpSource->m_bSelected = TRUE;

	m_lpSources->AddSource(lpSource);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::AddSourcePerfCounter(PLC_ITEM_DESC* lpItemDesc)
{
	COverlayDataSourcePerfCounter* lpSource = new COverlayDataSourcePerfCounter;

	//Perf counter specific properties

	LPPERFCOUNTER_OBJECT_DESC lpObjectDesc = (LPPERFCOUNTER_OBJECT_DESC)lpItemDesc->dwUserData0;

	char szEnglishObjectName[MAX_PATH];
	m_objects.GetEnglishName(lpObjectDesc->szName, szEnglishObjectName, sizeof(szEnglishObjectName));
		//system enumerates object names in localized format, so we convert them back to English to provide language independent 
		//plugin settings
	lpSource->SetObjectName(szEnglishObjectName);

	if (lpObjectDesc->lpInstances)
	{
		char szEnglishInstanceName[MAX_PATH];
		m_objects.GetEnglishName(lpObjectDesc->lpInstances, szEnglishInstanceName, sizeof(szEnglishInstanceName));
			//system enumerates instamce names in localized format, so we convert them back to English to provide language independent 
			//plugin settings
		lpSource->SetInstanceName(szEnglishInstanceName);
		lpSource->SetInstanceIndex(-1);
	}

	lpSource->SetCounterName(lpItemDesc->szText);

	//common properties

	lpSource->SetName(lpItemDesc->szText);
	lpSource->SetUnits((LPCSTR)lpItemDesc->dwUserData2);
	lpSource->m_bSelected = TRUE;

	m_lpSources->AddSource(lpSource);
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::OnBnClickedDataProviderSetup()
{
	switch (m_nProvider)
	{
	case PROVIDER_INDEX_HAL:
		{
			COverlayDataProviderSetupHALDlg dlg;

			dlg.DoModal();
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayDataSourceAddDlg::UpdateProviderControls()
{
	switch (m_nProvider)
	{
	case PROVIDER_INDEX_HAL:
		GetDlgItem(IDC_DATA_PROVIDER_SETUP)->EnableWindow(TRUE);
		break;
	default:
		GetDlgItem(IDC_DATA_PROVIDER_SETUP)->EnableWindow(FALSE);
	}
}
//////////////////////////////////////////////////////////////////////
