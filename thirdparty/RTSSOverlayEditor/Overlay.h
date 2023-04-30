// Overlay.h: interface for the COverlay class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayLayer.h"
#include "OverlayDataSourcesList.h"
#include "OverlayEditorMessages.h"
#include "OverlayAttributesLibrary.h"
#include "OverlayAttributesLibraryStr.h"
#include "OverlayMacroList.h"
#include "RTSSHooksInterface.h"
#include "TextTablesList.h"
#include <afxtempl.h>
/////////////////////////////////////////////////////////////////////////////
#define MAX_LAYERS												256
#define CHAR_BUF_SIZE											4096
/////////////////////////////////////////////////////////////////////////////
class COverlay : public CList<COverlayLayer*, COverlayLayer*>
{
public:
	COverlay();
	virtual ~COverlay();

	void New();
		//creates new overlay 
	void Destroy();
		//destroys overlay layers without altering the rest overlay properties
	void Load();
		//loads overlay
	void Save();
		//saves overlay
	void Import(LPCSTR lpFilename);
		//imports overlay and its dependent files
	void Export(LPCSTR lpFilename);
		//exports overlay and its dependent files
	void EnumFiles(CStringList* lpList);
		//enumerates all overlay dependent files (including main layout file)

	void Copy(COverlay* lpSource);
		//copies overlay
	BOOL IsEqual(COverlay* lpSource);
		//compares two overlays

	CString GetHypertext(BOOL bOptimize);
		//returns fully formatted overlay hypertext and embeds all objects into the buffer
	CString	GetHypertextForDebugger();
		//returns last fully formatted copy of overlay hypertext with color highlighting applied to the header and focused layer hypertext
	DWORD	GetHypertextSize();
		//returns size of the last fully formatted overlay hypertext
	DWORD	GetHypertextBufferSize();
		//returns size of embedded object buffer for the last fully formatted overlay hypertext
	void	Update(BOOL bPollDataSources, BOOL bOptimize);
		//optionally polls data sources, updates overlay hypertext and sends it to RTSS 
	void	UpdateTimerDerivedSources();
		//updates data sources derived from HAL timer 
		
	BOOL	AddLayer(COverlayLayer* lpLayer);
		//adds layer
	BOOL	RemoveLayer(COverlayLayer* lpLayer);
		//removes layer
	void	UpdateLayerIDs();
		//recalcluates and updates layer identifiers

	void	SelectLayers(CRect rcSelection);
		//selects all layers intersecting with specified rectangle
	void	SelectAllLayers(BOOL bSelect);
		//selects/unselects all layers

	COverlayLayer*	GetFocusedLayer();
		//get focused layer
	void	SetFocusedLayer(COverlayLayer* lpLayer);
		//sets focused layer
	BOOL	SetNextFocusedLayer();
		//selects next layer
	BOOL	SetPrevFocusedLayer();
		//selects previous layer 

	BOOL	MoveLayerUp(COverlayLayer* lpLayer);
		//moves layer up in Z-order
	BOOL	MoveLayerDown(COverlayLayer* lpLayer);
		//moves layer down in Z-order
	BOOL	MoveLayerToTop(COverlayLayer* lpLayer);
		//moves layer to the top of Z-order
	BOOL	MoveLayerToBottom(COverlayLayer* lpLayer);
		//moves layer to the bottom of Z-order
	void	Reorder(CList<COverlayLayer*, COverlayLayer*>* lpOrder);
		//reorder layers

	BOOL	IsLayerTopmost(COverlayLayer* lpLayer);
		//checks if layer is topmost
	BOOL	IsLayerBottommost(COverlayLayer* lpLayer);
		//checks if layer is bottommost

	COverlayAttributesLibrary* GetSizesLibrary();
		//returns size attributes library
	COverlayAttributesLibrary* GetColorsLibrary();
		//returns color attributes library
	COverlayAttributesLibraryStr* GetColorsLibraryStr();
		//returns color attributes library (string version for dynamic colors)
	COverlayAttributesLibrary* GetAlignmentsLibrary();
		//returns alignment  attributes library
	void UpdateAttributesLibraries();
		//updates all attributes libraries
	void GetRecentColors(LPDWORD lpRecentColors, LPDYNAMIC_COLOR_DESC lpRecentDynamicColors = NULL);
		//fills array of the last 32 recent colors for color picker dialog

	void	SetFilename(LPCSTR lpFilename);
		//sets overlay filename
	CString	GetFilename();
		//gets overlay filename

	void OnMasterSettings();
		//sets master settings (overlay creator's original font and zoom ratio)

	CString	GetCfgFolder();
		//gets overlay configuration files folder
	CString	GetCfgPath();	
		//gets fully qualified overlay configuration file path

	CString GetConfigStr(LPCSTR lpSection, LPCSTR lpName, LPCTSTR lpDefault);
	void	SetConfigStr(LPCSTR lpSection, LPCSTR lpName, LPCSTR lpValue);
		//string variable save/load 

	int		GetConfigInt(LPCSTR lpSection, LPCSTR lpName, int nDefault);
	void	SetConfigInt(LPCSTR lpSection, LPCSTR lpName, int nValue);
		//int variable save/load 

	DWORD	GetConfigHex(LPCSTR lpSection, LPCSTR lpName, DWORD dwDefault);
	void	SetConfigHex(LPCSTR lpSection, LPCSTR lpName, DWORD dwValue);
		//hex variable save/load 

	void	AddMessageToQueue(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams);
		//adds overlay message to the queue
	BOOL	FlushMessageQueue(CWnd* pWnd);
		//flushes messages from overlay message queue
	BOOL	ProcessMessage(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams, CWnd* pWnd);
		//handles overlay message

	COverlayLayer*	HitTest(POINT pt, DWORD dwDepth, DWORD* lpMask);
		//overlay hittesting functionality, returns hit layer and fills hittest zone mask

		//NOTE: hittesting skips top dwDepth hit layers, so for example you can select a layer right above the topmost one by specifying dwDepth=1
	
	COverlayDataSourcesList* GetInternalDataSources();
		//returns the list of internal data source
	COverlayDataSourcesList* GetExternalDataSources();
		//returns the list of external data sources
	COverlayDataSource*	 FindDataSource(LPCSTR lpName);
		//finds data source by name

	float GetPollingTime();
		//returns synchronous polling time for both internal and external data sources

	CTextTablesList* GetTextTablesList();
		//returns text tables list
	CTextTable* FindTextTable(LPCSTR lpName);
		//finds text table by name

	COverlayMacroList* GetMacroList();
		//returns macro list
	CStringList* GetTagList();
		//returns tag list

	BOOL OnTimer(BOOL bOptimize, DWORD dwTimerRes);
		//timer callback, updates whole overlay or some layers only depending on timestamps, returns TRUE on full update
	DWORD GetTimerPeriod();
		//returns timer period 
	
		//NOTE: timer period is caluclated as minimum of overlay refresh period and all layer specific non-zero refresh periods

	BOOL IsRefreshRequired(DWORD dwTimestamp);
		//returns TRUE if some layers (which may have custom refresh periods) require refresh at the moment

		//NOTE: this call also updates refresh timestamps for such layers

	void AllocEmbeddedObjectBuffer(DWORD dwSize);
		//allocates embedded object buffer
	void FreeEmbeddedObjectBuffer();
		//frees embedded object buffer

	CString m_strName;
	CString	m_strEmbeddedImage;
	DWORD	m_dwRefreshPeriod;
	DWORD	m_dwRefreshTimestamp;
	CString m_strPingAddr;

protected:
	COverlayLayer* m_lpFocusedLayer;

	CString	m_strCfgFolder;
	CString	m_strFilename;

	CString	m_strHypertextForDebugger;
	DWORD	m_dwHypertextSize;
	DWORD	m_dwHypertextBufferSize;

	CString	m_strQueuedMessage;
	CString	m_strQueuedMessageLayer;
	CString	m_strQueuedMessageParams;

	COverlayAttributesLibrary		m_sizesLibrary;
	COverlayAttributesLibrary		m_colorsLibrary;
	COverlayAttributesLibraryStr	m_colorsLibraryStr;
	COverlayAttributesLibrary		m_alignmentsLibrary;

	COverlayDataSourcesList m_internalDataSources;
	COverlayDataSourcesList m_externalDataSources;

	CTextTablesList m_textTables;

	COverlayMacroList m_macroList;
	CStringList m_tagList;

	LPBYTE	m_lpEmbeddedObjectBuffer;
	DWORD	m_dwEmbeddedObjectBufferSize;
	DWORD	m_dwEmbeddedObjectBufferPos;
		//local copy of hypertext embedded object buffer, we'll use it for flicker free OSD update
};
/////////////////////////////////////////////////////////////////////////////
