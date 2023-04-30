// OverlayEditorClientWnd.cpp: implementation of the COverlayEditorClientWnd class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayEditorClientWnd.h"
#include "RTSSSharedMemoryInterface.h"

#include <shlwapi.h>
//////////////////////////////////////////////////////////////////////
#define CLIENT_TIMER_ID				'CTID'
//////////////////////////////////////////////////////////////////////
// COverlayEditorClientWnd
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayEditorClientWnd, CWnd)
//////////////////////////////////////////////////////////////////////
COverlayEditorClientWnd::COverlayEditorClientWnd()
{
	m_lpOverlayMessageHandler	= NULL;
	m_dwTimerPeriod				= 0;
	m_bEnableOverlay			= TRUE;
}
//////////////////////////////////////////////////////////////////////
COverlayEditorClientWnd::~COverlayEditorClientWnd()
{
}
//////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayEditorClientWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////
// COverlayEditorClientWnd message handlers
//////////////////////////////////////////////////////////////////////
int COverlayEditorClientWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Load();
	m_overlay.Load();
	InitTimer();

	return 0;
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::InitTimer()
{
	DWORD dwTimerPeriod = m_overlay.GetTimerPeriod();

	if (m_dwTimerPeriod != dwTimerPeriod)
	{
		m_dwTimerPeriod = dwTimerPeriod;

		m_mmTimer.Create(m_dwTimerPeriod, 16, 0, m_hWnd, WM_TIMER, CLIENT_TIMER_ID, 0);
	}
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::OnDestroy()
{
	m_mmTimer.Kill();

	CRTSSSharedMemoryInterface sharedMemoryInterface;
	sharedMemoryInterface.ReleaseOSD(OSD_OWNER_NAME);

	CWnd::OnDestroy();
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == CLIENT_TIMER_ID)
	{
		if (m_bEnableOverlay)
			m_overlay.OnTimer(TRUE, 16);

		m_mmTimer.EatMessages();
	}

	CWnd::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::EnableOverlay(BOOL bEnableOverlay)
{
	m_bEnableOverlay = bEnableOverlay;
}
//////////////////////////////////////////////////////////////////////
CString COverlayEditorClientWnd::GetCfgPath()
{
	if (!strlen(m_strCfgPath))
	{
		char szCfgPath[MAX_PATH];
		GetModuleFileName(g_hModule, szCfgPath, MAX_PATH);
		PathRenameExtension(szCfgPath, ".cfg");

		m_strCfgPath = szCfgPath;
	}

	return m_strCfgPath;
}
//////////////////////////////////////////////////////////////////////
CString COverlayEditorClientWnd::GetConfigStr(LPCSTR lpSection, LPCSTR lpName, LPCTSTR lpDefault)
{
	char szBuf[CHAR_BUF_SIZE];
	GetPrivateProfileString(lpSection, lpName, lpDefault, szBuf, CHAR_BUF_SIZE, GetCfgPath());

	return szBuf;
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::SetConfigStr(LPCSTR lpSection, LPCSTR lpName, LPCSTR lpValue)
{
	WritePrivateProfileString(lpSection, lpName, lpValue, GetCfgPath());
}
//////////////////////////////////////////////////////////////////////
int	COverlayEditorClientWnd::GetConfigInt(LPCSTR lpSection, LPCSTR lpName, int nDefault)
{
	return GetPrivateProfileInt(lpSection, lpName, nDefault, GetCfgPath());
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::SetConfigInt(LPCSTR lpSection, LPCSTR lpName, int nValue)
{
	char szValue[MAX_PATH];
	sprintf_s(szValue, sizeof(szValue), "%d", nValue);

	WritePrivateProfileString(lpSection, lpName, szValue, GetCfgPath());
}
//////////////////////////////////////////////////////////////////////
DWORD COverlayEditorClientWnd::GetConfigHex(LPCSTR lpSection, LPCSTR lpName, DWORD dwDefault)
{
	char szValue[MAX_PATH];
	GetPrivateProfileString(lpSection, lpName, "", szValue, MAX_PATH, GetCfgPath());

	DWORD dwResult = dwDefault; 
	sscanf_s(szValue, "%08X", &dwResult);

	return dwResult;
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::SetConfigHex(LPCSTR lpSection, LPCSTR lpName, DWORD dwValue)
{
	char szValue[MAX_PATH];
	sprintf_s(szValue, sizeof(szValue), "%08X", dwValue);

	WritePrivateProfileString(lpSection, lpName, szValue, GetCfgPath());
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::Load()
{
	g_hal.SetCpuUsageCalc(GetConfigInt("Settings", "CpuUsageCalc", 1));
	g_hal.EnableSmart(GetConfigInt("Settings", "SMART", 0) != 0);
	g_hal.SetSmartPollingInterval(GetConfigInt("Settings", "SMARTPollingInterval", 30000));
	g_hal.Init(GetConfigInt("Settings", "Driver", 1));

	CString strLayout = GetConfigStr("Settings", "Layout", "default.ovl");

	m_overlay.SetFilename(strLayout);
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::Save()
{
	SetConfigStr("Settings", "Layout", m_overlay.GetFilename());
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::SetOverlayMessageHandler(COverlayMessageHandler* lpOverlayMessageHandler)
{
	m_lpOverlayMessageHandler = lpOverlayMessageHandler;
}
//////////////////////////////////////////////////////////////////////
void COverlayEditorClientWnd::OnOverlayMessage(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams)
{
	if (m_lpOverlayMessageHandler)
	{
		m_lpOverlayMessageHandler->OnOverlayMessage(lpMessage, lpLayer, lpParams);
	}
	else
	{
		m_overlay.AddMessageToQueue(lpMessage, lpLayer, lpParams);

		PostMessage(UM_OVERLAY_MESSAGE);
	}
}
//////////////////////////////////////////////////////////////////////
LRESULT COverlayEditorClientWnd::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == UM_OVERLAY_MESSAGE)
	{
		if (m_overlay.FlushMessageQueue(this))
			m_overlay.Update(FALSE, TRUE);
	}

	if (message == UM_OVERLAY_LOADED)
	{
		m_overlay.Update(TRUE, TRUE);

		InitTimer();
	}

	return CWnd::DefWindowProc(message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////
