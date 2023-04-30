// OverlayEditorClientWnd.h: interface for the COverlayEditorClientWnd class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "Overlay.h"
#include "OverlayMessageHandler.h"
#include "MMTimer.h"
//////////////////////////////////////////////////////////////////////
// COverlayEditorClientWnd
//////////////////////////////////////////////////////////////////////
class COverlayEditorClientWnd : public CWnd, public COverlayMessageHandler
{
	DECLARE_DYNAMIC(COverlayEditorClientWnd)

public:
	COverlayEditorClientWnd();
	virtual ~COverlayEditorClientWnd();

	//COverlayMessageHandler virtual functions

	virtual void OnOverlayMessage(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams);

	
	void	Load();
		//loads client config (currently selcted overlay layout and so on)
	void	Save();
		//saves client config (currently selcted overlay layout and so on)

	void	SetOverlayMessageHandler(COverlayMessageHandler* lpOverlayMessageHandler);
		//allows redirecting overlay message to a different handler, we use it to redirect
		//message to editor window when it is open

	void	EnableOverlay(BOOL bEnableOverlay);
		//allows disabling overly update, we use it during editing the overlay

	CString GetCfgPath();
		//returns fully qualified path to the client configuration file

	CString GetConfigStr(LPCSTR lpSection, LPCSTR lpName, LPCTSTR lpDefault);
	void	SetConfigStr(LPCSTR lpSection, LPCSTR lpName, LPCSTR lpValue);	
		//string variable save/load 

	int		GetConfigInt(LPCSTR lpSection, LPCSTR lpName, int nDefault);
	void	SetConfigInt(LPCSTR lpSection, LPCSTR lpName, int nValue);
		//int variable save/load 

	DWORD	GetConfigHex(LPCSTR lpSection, LPCSTR lpName, DWORD dwDefault);
	void	SetConfigHex(LPCSTR lpSection, LPCSTR lpName, DWORD dwValue);
		//hex variable save/load 

	void	InitTimer();
		//reinitializes multimedia timer

	COverlay m_overlay;

protected:
	DECLARE_MESSAGE_MAP()

	CString		m_strCfgPath;
	CMMTimer	m_mmTimer;
	DWORD		m_dwTimerPeriod;

	BOOL		m_bEnableOverlay;

	COverlayMessageHandler*	m_lpOverlayMessageHandler;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
//////////////////////////////////////////////////////////////////////


