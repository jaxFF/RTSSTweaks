// Overlay.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Overlay.h"
#include "OverlayEditor.h"
#include "RTSSSharedMemoryInterface.h"
#include "SZArray.h"
#include "CRC32.h"

#include <shlwapi.h>
#include <float.h>
#include <io.h>
/////////////////////////////////////////////////////////////////////////////
COverlay::COverlay()
{	
	m_lpFocusedLayer				= NULL;

	m_strName						= "";
	m_strEmbeddedImage				= "";
	m_strPingAddr					= "";
	m_dwRefreshPeriod				= 1000;
	m_dwRefreshTimestamp			= 0;

	m_strCfgFolder					= "";
	m_strFilename					= "default.ovl";

	m_strHypertextForDebugger		= "";
	m_dwHypertextSize				= 0;
	m_dwHypertextBufferSize			= 0;

	m_strQueuedMessage				= "";
	m_strQueuedMessageLayer			= "";	
	m_strQueuedMessageParams		= "";

	m_internalDataSources.SetOverlay(this);
	m_externalDataSources.SetOverlay(this);
	m_textTables.SetOverlay(this);

	m_lpEmbeddedObjectBuffer		= NULL;
	m_dwEmbeddedObjectBufferSize	= 0;
	m_dwEmbeddedObjectBufferPos		= 0;
}
/////////////////////////////////////////////////////////////////////////////
COverlay::~COverlay()
{
	FreeEmbeddedObjectBuffer();

	Destroy();
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::Destroy()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		delete GetNext(pos);

	RemoveAll();

	m_lpFocusedLayer = NULL;
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::AllocEmbeddedObjectBuffer(DWORD dwSize)
{
	FreeEmbeddedObjectBuffer();

	if (dwSize)
	{
		m_lpEmbeddedObjectBuffer		= new BYTE[dwSize];
		m_dwEmbeddedObjectBufferSize	= dwSize;
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::FreeEmbeddedObjectBuffer()
{
	if (m_lpEmbeddedObjectBuffer)
		delete [] m_lpEmbeddedObjectBuffer;

	m_lpEmbeddedObjectBuffer		= NULL;
	m_dwEmbeddedObjectBufferSize	= 0;
	m_dwEmbeddedObjectBufferPos		= 0;
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::New()
{
	Destroy();

	m_strFilename		= "";
	m_strEmbeddedImage	= "";
	m_strPingAddr		= "";
	m_dwRefreshPeriod	= 1000;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlay::GetHypertext(BOOL bOptimize)
{
	if (bOptimize)
		UpdateAttributesLibraries();

	CString strLoadImageTag;
	if (!m_strEmbeddedImage.IsEmpty())
	{
		if (CHypertextParser::IsQuotingRequired(m_strEmbeddedImage))
			strLoadImageTag.Format("<LI=\"Plugins\\Client\\Overlays\\%s\">", m_strEmbeddedImage);
		else
			strLoadImageTag.Format("<LI=Plugins\\Client\\Overlays\\%s>", m_strEmbeddedImage);
	}

	CString strHeader = strLoadImageTag;

	if (bOptimize)
	{
		strHeader += m_sizesLibrary.FormatHeaderTags("S", FALSE);
		strHeader += m_colorsLibrary.FormatHeaderTags("C", TRUE);
		strHeader += m_colorsLibraryStr.FormatHeaderTags("C", m_colorsLibrary.GetSize());
		strHeader += m_alignmentsLibrary.FormatHeaderTags("A", FALSE);
	}

	DWORD dwEmbeddedObjectOffset = 0;

	if (!m_lpEmbeddedObjectBuffer)
		AllocEmbeddedObjectBuffer(262144);

	CString strHypertext			= strHeader;
	CString strHypertextForDebugger	= "[CA0]" + strHeader + "[/C]";

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = GetNext(pos);

		BOOL bFocused = (lpLayer == m_lpFocusedLayer);
		
		CString strLayerHypertext = lpLayer->GetHypertext(bOptimize, m_lpEmbeddedObjectBuffer, m_dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset);

		strHypertext += strLayerHypertext;

		if (bFocused)
			strHypertextForDebugger += "[CFF]" + strLayerHypertext + "[/C]";
		else
			strHypertextForDebugger += strLayerHypertext;
	}

	m_dwEmbeddedObjectBufferPos		= dwEmbeddedObjectOffset;

	m_strHypertextForDebugger		= strHypertextForDebugger;
	m_dwHypertextSize				= strHypertext.GetLength();
	m_dwHypertextBufferSize			= dwEmbeddedObjectOffset;

	return strHypertext;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::AddLayer(COverlayLayer* lpLayer)
{
	if (GetCount() < MAX_LAYERS)
	{
		lpLayer->SetOverlay(this);
		lpLayer->SetLayerID(GetCount());
		AddTail(lpLayer);

		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
COverlayLayer* COverlay::HitTest(POINT pt, DWORD dwDepth, DWORD* lpMask)
{
	if (m_lpFocusedLayer)
	{
		DWORD dwMask = m_lpFocusedLayer->HitTest(pt);

		if (dwMask)
		{
			if (dwMask != HIT_TEST_MASK_BODY)
			{
				if (lpMask)
					*lpMask = dwMask;

				return m_lpFocusedLayer;
			}
		}
	}
	
	POSITION pos = GetTailPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = GetPrev(pos);

		DWORD dwMask = lpLayer->HitTest(pt);

		if (dwMask)
		{
			if (lpMask)
				*lpMask = dwMask;

			if (!dwDepth)
				return lpLayer;

			dwDepth--;
		}
	}

	if (lpMask)
		*lpMask = 0;

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
COverlayLayer* COverlay::GetFocusedLayer()
{
	return m_lpFocusedLayer;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::MoveLayerUp(COverlayLayer* lpLayer)
{
	if (lpLayer)
	{
		POSITION srcPos = Find(lpLayer);

		if (srcPos)
		{
			POSITION dstPos = srcPos;

			GetNext(dstPos);

			if (dstPos)
			{
				RemoveAt(srcPos);
				InsertAfter(dstPos, lpLayer);

				UpdateLayerIDs();

				return TRUE;
			}
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::MoveLayerDown(COverlayLayer* lpLayer)
{
	if (lpLayer)
	{
		POSITION srcPos = Find(lpLayer);

		if (srcPos)
		{
			POSITION dstPos = srcPos;

			GetPrev(dstPos);

			if (dstPos)
			{
				RemoveAt(srcPos);
				InsertBefore(dstPos, lpLayer);

				UpdateLayerIDs();

				return TRUE;
			}
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::IsLayerTopmost(COverlayLayer* lpLayer)
{
	if (lpLayer)
	{
		POSITION srcPos = Find(lpLayer);

		if (srcPos)
		{
			POSITION dstPos = GetTailPosition();

			return (srcPos == dstPos);
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::IsLayerBottommost(COverlayLayer* lpLayer)
{
	if (lpLayer)
	{
		POSITION srcPos = Find(lpLayer);

		if (srcPos)
		{
			POSITION dstPos = GetHeadPosition();

			return (srcPos == dstPos);
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::MoveLayerToTop(COverlayLayer* lpLayer)
{
	if (lpLayer)
	{
		POSITION srcPos = Find(lpLayer);

		if (srcPos)
		{
			POSITION dstPos = GetTailPosition();

			if (srcPos != dstPos)
			{
				RemoveAt(srcPos);
				InsertAfter(dstPos, lpLayer);

				UpdateLayerIDs();

				return TRUE;
			}
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::MoveLayerToBottom(COverlayLayer* lpLayer)
{
	if (lpLayer)
	{
		POSITION srcPos = Find(lpLayer);

		if (srcPos)
		{
			POSITION dstPos = GetHeadPosition();

			if (srcPos != dstPos)
			{
				RemoveAt(srcPos);
				InsertBefore(dstPos, lpLayer);

				UpdateLayerIDs();

				return TRUE;
			}
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::SetNextFocusedLayer()
{
	if (m_lpFocusedLayer)
	{
		POSITION pos = Find(m_lpFocusedLayer);

		if (pos)
			GetNext(pos);

		if (pos)
		{
			m_lpFocusedLayer = GetAt(pos);

			return TRUE;
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL COverlay::SetPrevFocusedLayer()
{
	if (m_lpFocusedLayer)
	{
		POSITION pos = Find(m_lpFocusedLayer);

		if (pos)
			GetPrev(pos);

		if (pos)
		{
			m_lpFocusedLayer = GetAt(pos);

			return TRUE;
		}
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void COverlay::SetFocusedLayer(COverlayLayer* lpLayer)
{
	m_lpFocusedLayer = lpLayer;
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::UpdateTimerDerivedSources()
{
	m_externalDataSources.UpdateTimerDerivedSources();
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::Update(BOOL bPollDataSources, BOOL bOptimize)
{
	if (bPollDataSources)
	{
		if (strcmp(g_hal.GetPingAddr(), m_strPingAddr))
			g_hal.InitPingThread(m_strPingAddr);

		m_internalDataSources.UpdateSources();
		m_externalDataSources.UpdateSources();

		m_macroList.Init(this);
	}

	CRTSSSharedMemoryInterface sharedMemoryInterface;
	sharedMemoryInterface.UpdateOSD(OSD_OWNER_NAME, GetHypertext(bOptimize), m_lpEmbeddedObjectBuffer, m_dwEmbeddedObjectBufferPos);
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::Copy(COverlay* lpSrc)
{
	Destroy();

	m_strName			= lpSrc->m_strName;
	m_dwRefreshPeriod	= lpSrc->m_dwRefreshPeriod;
	m_strEmbeddedImage	= lpSrc->m_strEmbeddedImage;
	m_strPingAddr		= lpSrc->m_strPingAddr;

	m_strFilename		= lpSrc->m_strFilename;

	m_internalDataSources.Copy(&lpSrc->m_internalDataSources);
	m_externalDataSources.Copy(&lpSrc->m_externalDataSources);

	m_textTables.Copy(&lpSrc->m_textTables);

	POSITION pos = lpSrc->GetHeadPosition();

	while (pos)
	{ 
		COverlayLayer* lpSrcLayer = lpSrc->GetNext(pos);
		COverlayLayer* lpNewLayer = new COverlayLayer;

		lpNewLayer->Copy(lpSrcLayer);

		AddLayer(lpNewLayer);

		if (lpSrcLayer == lpSrc->m_lpFocusedLayer)
			SetFocusedLayer(lpNewLayer);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::IsEqual(COverlay* lpSrc)
{
	if (strcmp(m_strName, lpSrc->m_strName))
		return FALSE;
	if (m_dwRefreshPeriod != lpSrc->m_dwRefreshPeriod)
		return FALSE;
	if (strcmp(m_strEmbeddedImage, lpSrc->m_strEmbeddedImage))
		return FALSE;
	if (strcmp(m_strPingAddr, lpSrc->m_strPingAddr))
		return FALSE;

	if (!m_externalDataSources.IsEqual(&lpSrc->m_externalDataSources))
		return FALSE;

	if (!m_textTables.IsEqual(&lpSrc->m_textTables))
		return FALSE;

	POSITION posSrc = lpSrc->GetHeadPosition();
	POSITION posOwn	= GetHeadPosition();

	while (posSrc && posOwn)
	{
		COverlayLayer* lpSrcLayer	= lpSrc->GetNext(posSrc);
		COverlayLayer* lpOwnLayer	= GetNext(posOwn);

		if (!lpOwnLayer->IsEqual(lpSrcLayer))
			return FALSE;
	}

	if (posSrc || posOwn)
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void COverlay::Load()
{
	Destroy();

	m_strName			= GetConfigStr("Settings", "Name"			, "");
	m_dwRefreshPeriod	= GetConfigInt("Settings", "RefreshPeriod"	, 1000);
	if (m_dwRefreshPeriod < 100)
		m_dwRefreshPeriod = 100;
	if (m_dwRefreshPeriod > 60000)
		m_dwRefreshPeriod = 60000;
	m_strEmbeddedImage	= GetConfigStr("Settings", "EmbeddedImage"	, "");
	m_strPingAddr		= GetConfigStr("Settings", "PingAddr"		, "");

	m_internalDataSources.CreateInternalSources();
	m_externalDataSources.Load();
	m_textTables.Load();

	int nLayers = GetConfigInt("General", "Layers", 0);

	for (int nLayer=0; nLayer<nLayers; nLayer++)
	{
		COverlayLayer* lpLayer = new COverlayLayer;

		lpLayer->SetLayerID(nLayer);
		lpLayer->SetOverlay(this);
		lpLayer->Load();

		AddLayer(lpLayer);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::Save()
{
	CreateDirectory(GetCfgFolder());

	try
	{
		CFile::Remove(GetCfgPath());
	}
	catch(CFileException* /*exception*/)
	{
	}

	CRTSSHooksInterface rtssInterface;

	rtssInterface.LoadProfile("");

	DWORD	dwImplementation		= 2;
	char	szFontFace[MAX_PATH]	= "Unispace";
	LONG	dwFontHeight			= -9;
	DWORD	dwFontWeight			= 400;
	DWORD	dwZoomRatio				= 1;

	rtssInterface.GetProfileProperty("Implementation"	, (LPBYTE)&dwImplementation	, sizeof(dwImplementation));
	rtssInterface.GetProfileProperty("FontFace"			, (LPBYTE)szFontFace		, MAX_PATH);
	rtssInterface.GetProfileProperty("FontHeight"		, (LPBYTE)&dwFontHeight		, sizeof(dwFontHeight));
	rtssInterface.GetProfileProperty("FontWeight"		, (LPBYTE)&dwFontWeight		, sizeof(dwFontWeight));
	rtssInterface.GetProfileProperty("ZoomRatio"		, (LPBYTE)&dwZoomRatio		, sizeof(dwZoomRatio));

	SetConfigInt("Master", "Implementation"	, dwImplementation);
	SetConfigStr("Master", "FontFace"		, szFontFace);
	SetConfigInt("Master", "FontHeight"		, dwFontHeight);
	SetConfigInt("Master", "FontWeight"		, dwFontWeight);
	SetConfigInt("Master", "ZoomRatio"		, dwZoomRatio);

	SetConfigStr("Settings", "Name"			, m_strName);
	SetConfigInt("Settings", "RefreshPeriod", m_dwRefreshPeriod);
	SetConfigStr("Settings", "EmbeddedImage", m_strEmbeddedImage);
	SetConfigStr("Settings", "PingAddr"		, m_strPingAddr);

	m_externalDataSources.Save();
	m_textTables.Save();

	SetConfigInt("General", "Layers", GetCount());

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->Save();
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::Import(LPCSTR lpFilename)
{
	CSZArray container;

	if (container.Load(lpFilename))
		//try to load import container
	{
		for (DWORD dwEntry=0; dwEntry<container.GetNumEntries(); dwEntry++)
			//process container entries one by one, each entry contains independent file to be imported
		{
			LPSZARRAY_ENTRY_HEADER lpHeader = container.FindEntryHeaderByIndex(dwEntry);
				//get ptr to current entry header

			LPCSTR	lpFile	= container.GetEntryStringByHeader(lpHeader);
				//entry string contains associated file name
			LPVOID	lpData	= container.GetEntryDataByHeader(lpHeader);
				//entry data contains associated file data
			DWORD	dwSize	= container.GetEntryDataSizeByHeader(lpHeader);
				//entry data size contains size of associated file

			BOOL bImport = TRUE;

			if (!_taccess(GetCfgFolder() + lpFile, 0))
				//ensure that we don't modify existing file without asking user
			{
				CCRC32 crc;

				if (crc.Calc(0, (LPBYTE)lpData, dwSize) == crc.Calc(0, GetCfgFolder() + lpFile))
					//use checksum to verify if we're extracting exactly the same copy of the file, skip import in this case
					bImport = FALSE;

				if (bImport)
				{
					CString strMessage;
					strMessage.Format(LocalizeStr("Different version of %s file already exists, do you want to overwrite it?"), lpFile);

					if (MessageBox(NULL, strMessage, LocalizeStr("Overlay editor"), MB_ICONQUESTION|MB_YESNO) != IDYES)
						bImport = FALSE;
				}
			}

			if (bImport)
			{
				CFile file;

				if (file.Open(GetCfgFolder() + lpFile, CFile::modeCreate|CFile::modeWrite))
				{
					TRY
					{
						file.Write(lpData, dwSize);
					}
					CATCH_ALL(exception)
					{
						CString strMessage;
						strMessage.Format(LocalizeStr("Failed to extract %s file!"), lpFile);

						MessageBox(NULL, strMessage, LocalizeStr("Overlay editor"), MB_ICONERROR|MB_OK);
					}
					END_CATCH_ALL
				}
			}

			if (!dwEntry)
				//the first entry always contains overlay layout file
				m_strFilename = lpFile;
		}

		Load();
	}
	else
	{
		CString strMessage;
		strMessage.Format(LocalizeStr("Failed to import %s file!"), lpFilename);

		MessageBox(NULL, strMessage, LocalizeStr("Overlay editor"), MB_ICONERROR|MB_OK);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::Export(LPCSTR lpFilename)
{
	CStringList filesList;

	EnumFiles(&filesList);
		//enumerate all files to be exported (includes main overlay layout file and dependent files like embedded images)

	CSZArray container;

	POSITION pos = filesList.GetHeadPosition();

	while (pos)
	{
		CString strFile = filesList.GetNext(pos);

		container.AddEntryFromFile(strFile, GetCfgFolder() + strFile);
			//add dependent files to export container file one by one
	}

	TRY
	{
		container.Save(lpFilename, TRUE);
			//compress and save export container
	}
	CATCH_ALL(exception)
	{
		CString strMessage;
		strMessage.Format(LocalizeStr("Failed to export %s file!"), lpFilename);

		MessageBox(NULL, strMessage, LocalizeStr("Overlay editor"), MB_ICONERROR|MB_OK);
	}
	END_CATCH_ALL
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::EnumFiles(CStringList* lpList)
{
	lpList->RemoveAll();

	if (!m_strFilename.IsEmpty())
		//export main overlay layout file
		lpList->AddTail(m_strFilename);

	if (!m_strEmbeddedImage.IsEmpty())
		//export embedded image file if it is specified
		lpList->AddTail(m_strEmbeddedImage);

	POSITION pos = GetHeadPosition();

	while (pos)
		//export additional files, which can be associated with layers
	{
		COverlayLayer* lpLayer = GetNext(pos);

		CString strTextFile = lpLayer->GetTextFile();
			//dynamic layers may contain references to external text files, defined with file hypertext extension tag

		if (!strTextFile.IsEmpty())
			lpList->AddTail(strTextFile);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlay::RemoveLayer(COverlayLayer* lpLayer)
{
	if (m_lpFocusedLayer == lpLayer)
		m_lpFocusedLayer = NULL;

	POSITION pos = Find(lpLayer);

	if (pos)
	{
		RemoveAt(pos);

		delete lpLayer;

		UpdateLayerIDs();

		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlay::UpdateLayerIDs()
{
	DWORD dwLayerID = 0;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetLayerID(dwLayerID++);
}
/////////////////////////////////////////////////////////////////////////////
CString COverlay::GetCfgFolder()
{
	if (!strlen(m_strCfgFolder))
	{
		char szCfgFolder[MAX_PATH];
		GetModuleFileName(g_hModule, szCfgFolder, MAX_PATH);
		PathRemoveFileSpec(szCfgFolder);

		strcat_s(szCfgFolder, MAX_PATH, "\\Overlays\\"); 

		m_strCfgFolder = szCfgFolder;
	}

	return m_strCfgFolder;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlay::GetCfgPath()
{
	return GetCfgFolder() + m_strFilename;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlay::GetConfigStr(LPCSTR lpSection, LPCSTR lpName, LPCTSTR lpDefault)
{
	char szBuf[CHAR_BUF_SIZE];
	GetPrivateProfileString(lpSection, lpName, lpDefault, szBuf, CHAR_BUF_SIZE, GetCfgPath());

	return szBuf;
}
//////////////////////////////////////////////////////////////////////
void COverlay::SetConfigStr(LPCSTR lpSection, LPCSTR lpName, LPCSTR lpValue)
{
	int nLen = strlen(lpValue);

	if (nLen)
	{
		char chFirst	= *(lpValue);
		char chLast		= *(lpValue + nLen - 1);

		if ((chFirst == ' ') || (chLast == ' ') || ((chFirst == '"') && (chLast == '"')))
			//GetPrivateProfileString forcibly trims and unquotes result, so we'll quote  it
			//if string contains heading/trailing space or if it is supposed to be quoted 
		{
			CString strValue;
			strValue.Format("\"%s\"", lpValue);

			WritePrivateProfileString(lpSection, lpName, strValue, GetCfgPath());

			return;
		}
	}

	WritePrivateProfileString(lpSection, lpName, lpValue, GetCfgPath());
}
//////////////////////////////////////////////////////////////////////
int	COverlay::GetConfigInt(LPCSTR lpSection, LPCSTR lpName, int nDefault)
{
	return GetPrivateProfileInt(lpSection, lpName, nDefault, GetCfgPath());
}
//////////////////////////////////////////////////////////////////////
void COverlay::SetConfigInt(LPCSTR lpSection, LPCSTR lpName, int nValue)
{
	char szValue[MAX_PATH];
	sprintf_s(szValue, sizeof(szValue), "%d", nValue);

	WritePrivateProfileString(lpSection, lpName, szValue, GetCfgPath());
}
//////////////////////////////////////////////////////////////////////
DWORD COverlay::GetConfigHex(LPCSTR lpSection, LPCSTR lpName, DWORD dwDefault)
{
	char szValue[MAX_PATH];
	GetPrivateProfileString(lpSection, lpName, "", szValue, MAX_PATH, GetCfgPath());

	DWORD dwResult = dwDefault; 
	sscanf_s(szValue, "%08X", &dwResult);

	return dwResult;
}
//////////////////////////////////////////////////////////////////////
void COverlay::SetConfigHex(LPCSTR lpSection, LPCSTR lpName, DWORD dwValue)
{
	char szValue[MAX_PATH];
	sprintf_s(szValue, sizeof(szValue), "%08X", dwValue);

	WritePrivateProfileString(lpSection, lpName, szValue, GetCfgPath());
}
//////////////////////////////////////////////////////////////////////
void COverlay::SetFilename(LPCSTR lpFilename)
{
	m_strFilename = lpFilename;
}
//////////////////////////////////////////////////////////////////////
CString COverlay::GetFilename()
{
	return m_strFilename;
}
//////////////////////////////////////////////////////////////////////
COverlayDataSource* COverlay::FindDataSource(LPCSTR lpName)
{
	COverlayDataSource* lpResult = NULL;

	POSITION pos;
	
	pos = m_externalDataSources.GetTailPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = m_externalDataSources.GetPrev(pos);

		if (!_stricmp(lpSource->GetName(), lpName))
		{
			if (lpSource->GetData() != FLT_MAX)
				return lpSource;
			else
				lpResult = lpSource;
		}
	}

	pos = m_internalDataSources.GetTailPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = m_internalDataSources.GetPrev(pos);

		if (!_stricmp(lpSource->GetName(), lpName))
		{
			if (lpSource->GetData() != FLT_MAX)
				return lpSource;
			else
				lpResult = lpSource;
		}
	}



	return lpResult;
}
//////////////////////////////////////////////////////////////////////
COverlayDataSourcesList* COverlay::GetInternalDataSources()
{
	return &m_internalDataSources;
}
//////////////////////////////////////////////////////////////////////
COverlayDataSourcesList* COverlay::GetExternalDataSources()
{
	return &m_externalDataSources;
}
//////////////////////////////////////////////////////////////////////
float COverlay::GetPollingTime()
{
	return m_internalDataSources.GetPollingTime() + m_externalDataSources.GetPollingTime();
}
//////////////////////////////////////////////////////////////////////
CString	COverlay::GetHypertextForDebugger()
{
	return m_strHypertextForDebugger;
}
//////////////////////////////////////////////////////////////////////
DWORD COverlay::GetHypertextSize()
{
	return m_dwHypertextSize;
}
//////////////////////////////////////////////////////////////////////
DWORD COverlay::GetHypertextBufferSize()
{
	return m_dwHypertextBufferSize;
}
//////////////////////////////////////////////////////////////////////
void COverlay::AddMessageToQueue(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams)
{
	HANDLE hMutex	= CreateMutex(NULL, FALSE, "Global\\Access_RTSSOverlayEditorMessageQueue");

	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);

	m_strQueuedMessage			= lpMessage;
	m_strQueuedMessageLayer		= lpLayer;
	m_strQueuedMessageParams	= lpParams;

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL COverlay::FlushMessageQueue(CWnd* pWnd)
{
	HANDLE hMutex	= CreateMutex(NULL, FALSE, "Global\\Access_RTSSOverlayEditorMessageQueue");

	if (hMutex)
		WaitForSingleObject(hMutex, INFINITE);

	BOOL bResult = FALSE;

	if (!m_strQueuedMessage.IsEmpty())
	{
		if (!ProcessMessage(m_strQueuedMessage, m_strQueuedMessageLayer, m_strQueuedMessageParams, pWnd))
			MessageBeep(MB_ICONERROR);

		m_strQueuedMessage			= "";
		m_strQueuedMessageLayer		= "";
		m_strQueuedMessageParams	= "";

		bResult = TRUE;
	}

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL COverlay::ProcessMessage(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams, CWnd* pWnd)
{
	if (!strcmp(lpMessage, OVERLAYEDITORMESSAGE_LOAD))
	{
		SetFilename(lpParams);
		Load();

		if (pWnd)
			pWnd->SendMessage(UM_OVERLAY_LOADED);
				//we use SendMessage instead of PostMessage on purpose to force the overlay to be updated synchronically

		return TRUE;
	}

	if (!strcmp(lpMessage, OVERLAYEDITORMESSAGE_LOAD_TEXT))
	{
		POSITION pos = GetHeadPosition();

		while (pos)
		{
			COverlayLayer* lpLayerCur = GetNext(pos);

			if (!strlen(lpLayer) || !strcmp(lpLayer, lpLayerCur->m_strName))
				lpLayerCur->LoadTextFile(FALSE);
		}

		return TRUE;
	}

	if (!strcmp(lpMessage, OVERLAYEDITORMESSAGE_VIEW_TEXT))
	{
		POSITION pos = GetHeadPosition();

		while (pos)
		{
			COverlayLayer* lpLayerCur = GetNext(pos);

			if (!strlen(lpLayer) || !strcmp(lpLayer, lpLayerCur->m_strName))
				lpLayerCur->ViewTextFile();
		}

		return TRUE;
	}

	if (!strcmp(lpMessage, OVERLAYEDITORMESSAGE_NEXT_TEXT_LINE))
	{
		POSITION pos = GetHeadPosition();

		while (pos)
		{
			COverlayLayer* lpLayerCur = GetNext(pos);

			if (!strlen(lpLayer) || !strcmp(lpLayer, lpLayerCur->m_strName))
				lpLayerCur->MoveToNextTextFileLine();
		}

		return TRUE;
	}

	if (!strcmp(lpMessage, OVERLAYEDITORMESSAGE_PREV_TEXT_LINE))
	{
		POSITION pos = GetHeadPosition();

		while (pos)
		{
			COverlayLayer* lpLayerCur = GetNext(pos);

			if (!strlen(lpLayer) || !strcmp(lpLayer, lpLayerCur->m_strName))
				lpLayerCur->MoveToPrevTextFileLine();
		}

		return TRUE;
	}

	if (!strcmp(lpMessage, OVERLAYEDITORMESSAGE_SET_TIMER))
	{
		m_macroList.SetTimer(lpParams);

		return TRUE;
	}

	if (!strcmp(lpMessage, OVERLAYEDITORMESSAGE_SET_PING_ADDR))
	{
		m_strPingAddr = lpParams;

		return TRUE;
	}

	if (!strcmp(lpMessage, OVERLAYEDITORMESSAGE_RESET_STAT))
	{
		m_externalDataSources.ResetStat();

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void COverlay::UpdateAttributesLibraries()
{
	m_sizesLibrary.Destroy();
	m_colorsLibrary.Destroy();
	m_colorsLibraryStr.Destroy();
	m_alignmentsLibrary.Destroy();

	//add layer specific attributes

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = GetNext(pos);

		if (lpLayer->m_nSize != DEFAULT_SIZE)
			m_sizesLibrary.AddAttr(lpLayer->m_nSize);

		CDynamicColor dynamicTextColor;
		CDynamicColor dynamicBgndColor;
		CDynamicColor dynamicBorderColor;

		DWORD dwTextColor = lpLayer->GetDynamicTextColor(dynamicTextColor);
		DWORD dwBgndColor = lpLayer->GetDynamicBgndColor(dynamicBgndColor);
		DWORD dwBorderColor = lpLayer->GetDynamicBorderColor(dynamicBorderColor);

		if (dynamicTextColor.IsInternalSource() || 
			dynamicTextColor.IsEmbedded())
			m_colorsLibraryStr.AddAttr(dynamicTextColor.FormatRanges());
		else
		{
			if (dwTextColor != DEFAULT_COLOR)
				m_colorsLibrary.AddAttr(dwTextColor);
		}

		if (dynamicBgndColor.IsInternalSource() || 
			dynamicBgndColor.IsEmbedded())
			m_colorsLibraryStr.AddAttr(dynamicBgndColor.FormatRanges());
		else
		{
			if (dwBgndColor != DEFAULT_COLOR)
				m_colorsLibrary.AddAttr(dwBgndColor);
		}

		if (dynamicBorderColor.IsInternalSource() || 
			dynamicBorderColor.IsEmbedded())
			m_colorsLibraryStr.AddAttr(dynamicBorderColor.FormatRanges());
		else
		{
			if (dwBorderColor != DEFAULT_COLOR)
				m_colorsLibrary.AddAttr(dwBorderColor);
		}

		if (lpLayer->m_nAlignment != DEFAULT_ALIGNMENT)
			m_alignmentsLibrary.AddAttr(lpLayer->m_nAlignment);
	}

	//add text table specific attributes

	POSITION posTable = m_textTables.GetHeadPosition();
	
	while (posTable)
		//process tables
	{
		DWORD dwCellsCount = 0;
			//total cells count in table, we'll use to calculate proper attribute usage count increment for table specific sizes/alignments

		CTextTable* lpTable = m_textTables.GetNext(posTable);

		POSITION posLine = lpTable->GetHeadPosition();

		while (posLine)
			//process table lines
		{
			CTextTableLine* lpLine = lpTable->GetNext(posLine);

			CDynamicColor dynamicLineColor;

			DWORD dwLineColor = lpLine->GetDynamicColor(dynamicLineColor);

			if (dynamicLineColor.IsInternalSource() ||
				dynamicLineColor.IsEmbedded())
				m_colorsLibraryStr.AddAttr(dynamicLineColor.FormatRanges());
			else
			{
				if (dwLineColor != DEFAULT_COLOR)
					m_colorsLibrary.AddAttr(dwLineColor);
			}

			POSITION posCell = lpLine->GetHeadPosition();

			while (posCell)
				//process table line cells
			{
				CTextTableCell* lpCell = lpLine->GetNext(posCell);

				CDynamicColor dynamicCellColor;

				DWORD dwCellColor = lpCell->GetDynamicColor(dynamicCellColor);

				if (dynamicCellColor.IsInternalSource() ||
					dynamicCellColor.IsEmbedded())
					m_colorsLibraryStr.AddAttr(dynamicCellColor.FormatRanges());
				else
				{
					if (dwCellColor != DEFAULT_COLOR)
						m_colorsLibrary.AddAttr(dwCellColor);
				}

				dwCellsCount++;
			}
		}

		if (lpTable->m_nValueSize != DEFAULT_SIZE)
			m_sizesLibrary.AddAttr(lpTable->m_nValueSize, dwCellsCount);

		if (lpTable->m_nValueAlignment != DEFAULT_ALIGNMENT)
			m_alignmentsLibrary.AddAttr(lpTable->m_nValueAlignment, dwCellsCount);

		if (lpTable->m_nUnitsSize != DEFAULT_SIZE)
			m_sizesLibrary.AddAttr(lpTable->m_nUnitsSize, dwCellsCount);

		if (lpTable->m_nUnitsAlignment != DEFAULT_ALIGNMENT)
			m_alignmentsLibrary.AddAttr(lpTable->m_nUnitsAlignment, dwCellsCount);
	}
}
//////////////////////////////////////////////////////////////////////
COverlayAttributesLibrary* COverlay::GetSizesLibrary()
{
	return &m_sizesLibrary;
}
//////////////////////////////////////////////////////////////////////
COverlayAttributesLibrary* COverlay::GetColorsLibrary()
{
	return &m_colorsLibrary;
}
//////////////////////////////////////////////////////////////////////
COverlayAttributesLibraryStr* COverlay::GetColorsLibraryStr()
{
	return &m_colorsLibraryStr;
}
//////////////////////////////////////////////////////////////////////
COverlayAttributesLibrary* COverlay::GetAlignmentsLibrary()
{
	return &m_alignmentsLibrary;
}
//////////////////////////////////////////////////////////////////////
void COverlay::GetRecentColors(LPDWORD lpRecentColors, LPDYNAMIC_COLOR_DESC lpRecentDynamicColors)
{
	COverlayAttributesLibrary		colorsLibrary;
	COverlayAttributesLibraryStr	dynamicColorsLibrary;

	//add layer specific colors

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = GetNext(pos);

		CDynamicColor dynamicTextColor;
		CDynamicColor dynamicBgndColor;

		DWORD dwTextColor = lpLayer->GetDynamicTextColor(dynamicTextColor);
		DWORD dwBgndColor = lpLayer->GetDynamicBgndColor(dynamicBgndColor);

		DWORD dwDynamicTextColorRanges = dynamicTextColor.GetRangeCount();

		if (dwDynamicTextColorRanges)
		{
			for (DWORD dwRange=0; dwRange<dwDynamicTextColorRanges; dwRange++)
				colorsLibrary.AddAttr(COverlayLayer::PackRGBA(dynamicTextColor.m_desc.ranges[dwRange].dwColor));

			dynamicColorsLibrary.AddAttr(dynamicTextColor.FormatRanges());
		}
		else
		{
			if (dwTextColor != DEFAULT_COLOR)
				colorsLibrary.AddAttr(dwTextColor);
		}

		DWORD dwDynamicBgndColorRanges = dynamicBgndColor.GetRangeCount();

		if (dwDynamicBgndColorRanges)
		{
			for (DWORD dwRange=0; dwRange<dwDynamicBgndColorRanges; dwRange++)
				colorsLibrary.AddAttr(COverlayLayer::PackRGBA(dynamicBgndColor.m_desc.ranges[dwRange].dwColor));

			dynamicColorsLibrary.AddAttr(dynamicBgndColor.FormatRanges());
		}
		else
		{
			if (dwBgndColor != DEFAULT_COLOR)
				colorsLibrary.AddAttr(dwBgndColor);
		}
	}

	//add table specific colors

	POSITION posTable = m_textTables.GetHeadPosition();
	
	while (posTable)
	{
		CTextTable* lpTable = m_textTables.GetNext(posTable);

		//add line specific colors

		POSITION posLine = lpTable->GetHeadPosition();

		while (posLine)
		{
			CTextTableLine* lpLine = lpTable->GetNext(posLine);

			CDynamicColor dynamicLineColor;

			DWORD dwLineColor = lpLine->GetDynamicColor(dynamicLineColor);

			DWORD dwDynamicLineColorRanges = dynamicLineColor.GetRangeCount();

			if (dwDynamicLineColorRanges)
			{
				for (DWORD dwRange=0; dwRange<dwDynamicLineColorRanges; dwRange++)
					colorsLibrary.AddAttr(COverlayLayer::PackRGBA(dynamicLineColor.m_desc.ranges[dwRange].dwColor));

				dynamicColorsLibrary.AddAttr(dynamicLineColor.FormatRanges());
			}
			else
			{
				if (dwLineColor != DEFAULT_COLOR)
					colorsLibrary.AddAttr(dwLineColor);
			}

			//add cell specific colors

			POSITION posCell = lpLine->GetHeadPosition();

			while (posCell)
			{
				CTextTableCell* lpCell = lpLine->GetNext(posCell);

				CDynamicColor dynamicCellColor;

				DWORD dwCellColor = lpCell->GetDynamicColor(dynamicCellColor);

				DWORD dwDynamicCellColorRanges = dynamicCellColor.GetRangeCount();

				if (dwDynamicCellColorRanges)
				{
					for (DWORD dwRange=0; dwRange<dwDynamicCellColorRanges; dwRange++)
						colorsLibrary.AddAttr(COverlayLayer::PackRGBA(dynamicCellColor.m_desc.ranges[dwRange].dwColor));

					dynamicColorsLibrary.AddAttr(dynamicCellColor.FormatRanges());
				}
				else
				{
					if (dwCellColor != DEFAULT_COLOR)
						colorsLibrary.AddAttr(dwCellColor);
				}
			}
		}
	}

	//exctract recent colors from temporary library

	if (lpRecentColors)
	{
		DWORD dwColors = colorsLibrary.GetSize();

		for (DWORD dwColor=0; dwColor<32; dwColor++)
		{
			if (dwColor < dwColors)
				lpRecentColors[dwColor] = COverlayLayer::UnpackRGBA(colorsLibrary.GetAt(dwColor));
			else
				lpRecentColors[dwColor] = COverlayLayer::UnpackRGBA(GetSysColor(COLOR_BTNFACE));
		}
	}

	//extract recent dynamic colors from temporary library

	if (lpRecentDynamicColors)
	{
		DWORD dwDynamicColors = dynamicColorsLibrary.GetSize();

		for (DWORD dwDynamicColor=0; dwDynamicColor<32; dwDynamicColor++)
		{
			if (dwDynamicColor < dwDynamicColors)
			{
				CDynamicColor color;
				color.Scan(dynamicColorsLibrary.GetAt(dwDynamicColor));

				lpRecentDynamicColors[dwDynamicColor] = color.m_desc;
			}
			else
				ZeroMemory(&lpRecentDynamicColors[dwDynamicColor], sizeof(DYNAMIC_COLOR_DESC));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void COverlay::SelectLayers(CRect rcSelection)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = GetNext(pos);

		CRect rcIntersect;

		if (IntersectRect(rcIntersect, rcSelection, &lpLayer->m_rect))
			lpLayer->m_bSelected = TRUE;
		else
			lpLayer->m_bSelected = FALSE;
	}
}
//////////////////////////////////////////////////////////////////////
void COverlay::SelectAllLayers(BOOL bSelect)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = GetNext(pos);

		lpLayer->m_bSelected = bSelect;
	}
}
//////////////////////////////////////////////////////////////////////
COverlayMacroList* COverlay::GetMacroList()
{
	return &m_macroList;
}
//////////////////////////////////////////////////////////////////////
CStringList* COverlay::GetTagList()
{
	if (m_tagList.IsEmpty())
	{
		m_tagList.AddTail("<GRMIN>");
		m_tagList.AddTail("graph minimum");

		m_tagList.AddTail("<GRMAX>");
		m_tagList.AddTail("graph maximum");

		m_tagList.AddTail("<FR>");
		m_tagList.AddTail("framerate");

		m_tagList.AddTail("<FRMIN>");
		m_tagList.AddTail("minimum framerate");

		m_tagList.AddTail("<FRAVG>");
		m_tagList.AddTail("average framerate");

		m_tagList.AddTail("<FRMAX>");
		m_tagList.AddTail("maximum framerate");

		m_tagList.AddTail("<FR10L>");
		m_tagList.AddTail("1% low framerate");

		m_tagList.AddTail("<FR01L>");
		m_tagList.AddTail("0.1% low framerate");

		m_tagList.AddTail("<FT>");
		m_tagList.AddTail("frametime");

		m_tagList.AddTail("<FC>");
		m_tagList.AddTail("frame counter");

		m_tagList.AddTail("<APP>");
		m_tagList.AddTail("abbreviated 3D API");

		m_tagList.AddTail("<API>");
		m_tagList.AddTail("3D API");

		m_tagList.AddTail("<EXE>");
		m_tagList.AddTail("executable");

		m_tagList.AddTail("<RES>");
		m_tagList.AddTail("resolution");

		m_tagList.AddTail("<ARCH>");
		m_tagList.AddTail("architecture");

		m_tagList.AddTail("<TIME=%H:%M:%S>");
		m_tagList.AddTail("time");

		m_tagList.AddTail("<BTIME>");
		m_tagList.AddTail("benchmark time");
	}

	return &m_tagList;
}
//////////////////////////////////////////////////////////////////////
CTextTablesList* COverlay::GetTextTablesList()
{
	return &m_textTables;
}
//////////////////////////////////////////////////////////////////////
CTextTable* COverlay::FindTextTable(LPCSTR lpName)
{
	return m_textTables.FindTable(lpName);
}
//////////////////////////////////////////////////////////////////////
DWORD COverlay::GetTimerPeriod()
{
	DWORD dwRefreshPeriod = m_dwRefreshPeriod;

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = GetNext(pos);

		if (lpLayer->m_dwRefreshPeriod && (lpLayer->m_dwRefreshPeriod < dwRefreshPeriod))
			dwRefreshPeriod = lpLayer->m_dwRefreshPeriod;
	}

	return max(dwRefreshPeriod, 16);
}
//////////////////////////////////////////////////////////////////////
BOOL COverlay::IsRefreshRequired(DWORD dwTimestamp)
{
	BOOL bResult = FALSE;

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = GetNext(pos);

		if (lpLayer->m_dwRefreshPeriod && (dwTimestamp - lpLayer->m_dwRefreshTimestamp >= lpLayer->m_dwRefreshPeriod))
		{
			bResult = TRUE;

			lpLayer->m_dwRefreshTimestamp = dwTimestamp;
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL COverlay::OnTimer(BOOL bOptimize, DWORD dwTimerRes)
{
	DWORD dwTimestamp = GetTickCount();

	BOOL bUpdate	= FALSE;
	BOOL bPoll		= FALSE;

	if (dwTimestamp - m_dwRefreshTimestamp >= m_dwRefreshPeriod - dwTimerRes)
	{
		bUpdate = TRUE;
		bPoll	= TRUE;

		m_dwRefreshTimestamp = dwTimestamp;
	}
	else
	{
		if (IsRefreshRequired(dwTimestamp))
		{
			UpdateTimerDerivedSources();

			bUpdate = TRUE;
		}
	}

	if (bUpdate)
		Update(bPoll, bOptimize);

	return bPoll;
}
//////////////////////////////////////////////////////////////////////
void COverlay::Reorder(CList<COverlayLayer*, COverlayLayer*>* lpOrder)
{
	//remove layers ptr without deleting the objects

	RemoveAll();

	//reorder the list

	POSITION pos = lpOrder->GetHeadPosition();

	while (pos)
	{
		COverlayLayer* lpLayer = lpOrder->GetNext(pos);

		AddLayer(lpLayer);
	}
}
//////////////////////////////////////////////////////////////////////
void COverlay::OnMasterSettings()
{
	CRTSSHooksInterface rtssInterface;

	rtssInterface.LoadProfile("");

	DWORD	dwImplementation	= GetConfigInt("Master", "Implementation"	, 2);
	CString	strFontFace			= GetConfigStr("Master", "FontFace"			, "Unispace");
	DWORD	dwFontHeight		= GetConfigInt("Master", "FontHeight"		, -9);
	DWORD	dwFontWeight		= GetConfigInt("Master", "FontWeight"		, 400);
	DWORD	dwZoomRatio			= GetConfigInt("Master", "ZoomRatio"		, 1);

	rtssInterface.SetProfileProperty("Implementation"	, (LPBYTE)&dwImplementation		, sizeof(dwImplementation	));
	rtssInterface.SetProfileProperty("FontFace"			, (LPBYTE)(LPCSTR)strFontFace	, strlen(strFontFace		));
	rtssInterface.SetProfileProperty("FontHeight"		, (LPBYTE)&dwFontHeight			, sizeof(dwFontHeight		));
	rtssInterface.SetProfileProperty("FontWeight"		, (LPBYTE)&dwFontWeight			, sizeof(dwFontWeight		));
	rtssInterface.SetProfileProperty("ZoomRatio"		, (LPBYTE)&dwZoomRatio			, sizeof(dwZoomRatio		));

	rtssInterface.SaveProfile("");
	rtssInterface.UpdateProfiles();
}
//////////////////////////////////////////////////////////////////////
