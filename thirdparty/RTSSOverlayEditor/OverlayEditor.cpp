// OverlayEditor.cpp : Defines the initialization routines for the DLL.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayEditorDlg.h"
#include "RTSSSharedMemory.h"
#include "DynamicColor.h"
#include "DynamicColorPickerDlg.h"
/////////////////////////////////////////////////////////////////////////////
#include <afxwin.h>
#include <afxdllx.h>
#include <direct.h>
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/////////////////////////////////////////////////////////////////////////////
static AFX_EXTENSION_MODULE OverlayEditorDLL = { NULL, NULL };
/////////////////////////////////////////////////////////////////////////////
HINSTANCE					g_hModule						= 0;

GET_HOST_APP_PROPERTY_PROC	g_pGetHostAppProperty			= NULL;
LOCALIZEWND_PROC			g_pLocalizeWnd					= NULL;
LOCALIZEMENU_PROC			g_pLocalizeMenu					= NULL;
LOCALIZESTR_PROC			g_pLocalizeStr					= NULL;
PICKCOLOR_PROC				g_pPickColor					= NULL;
PICKCOLOREX_PROC			g_pPickColorEx					= NULL;
GETCOLORPREVIEW_PROC		g_pGetColorPreview				= NULL;

DWORD						g_dwHeaderBgndColor				= 0x700000;
DWORD						g_dwHeaderTextColor				= 0xFFFFFF;

CRTCoreDriver				g_driver;
CHAL						g_hal;
COverlayEditorClientWnd		g_clientWnd;

#ifdef TEST_VERSION
CLog						g_log;
#endif
/////////////////////////////////////////////////////////////////////////////
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("OverlayEditor.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(OverlayEditorDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(OverlayEditorDLL);

		g_hModule = hInstance;

#ifdef TEST_VERSION
		g_log.Enable(LOG_ENABLE_DEBUGVIEW);
#endif
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("OverlayEditor.DLL Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(OverlayEditorDLL);
	}
	return 1;   // ok
}
/////////////////////////////////////////////////////////////////////////////
OVERLAYEDITOR_API BOOL Start()
{
	if (!IsWindow(g_clientWnd.GetSafeHwnd()))
	{
		g_clientWnd.CreateEx(0, AfxRegisterWndClass(0,::LoadCursor(NULL,IDC_ARROW)), "RTSSOverlayEditorClientWnd", WS_POPUP, 0, 0, 0, 0, NULL, NULL);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
OVERLAYEDITOR_API void Stop()
{
	if (IsWindow(g_clientWnd.GetSafeHwnd()))
	{
		g_hal.UninitPingThread();
		g_hal.UninitPollingThread();

		g_clientWnd.DestroyWindow();
	}
}
//////////////////////////////////////////////////////////////////////
OVERLAYEDITOR_API BOOL Setup(HWND hWnd)
{
	if (!hWnd)
		//host will call Setup with hWnd set to NULL to verify if configuration is supported by plugin, so we just return TRUE to indicate
		//that our plugin supports configuratoin
		return TRUE;

	//init host applicaiton API

	HMODULE hHost = GetModuleHandle(NULL);

	g_pGetHostAppProperty	= (GET_HOST_APP_PROPERTY_PROC)GetProcAddress(hHost, "GetHostAppProperty");
	g_pLocalizeWnd			= (LOCALIZEWND_PROC)GetProcAddress(hHost, "LocalizeWnd");
	g_pLocalizeMenu			= (LOCALIZEMENU_PROC)GetProcAddress(hHost, "LocalizeMenu");
	g_pLocalizeStr			= (LOCALIZESTR_PROC)GetProcAddress(hHost, "LocalizeStr");
	g_pPickColor			= (PICKCOLOR_PROC)GetProcAddress(hHost, "PickColor");
	g_pPickColorEx			= (PICKCOLOREX_PROC)GetProcAddress(hHost, "PickColorEx");
	g_pGetColorPreview		= (GETCOLORPREVIEW_PROC)GetProcAddress(hHost, "GetColorPreview");

	//get host application color scheme defined by currently selected host skin

	if (g_pGetHostAppProperty)
	{
		g_pGetHostAppProperty(HOST_APP_PROPERTY_SKIN_COLOR_HEADER_BGND, &g_dwHeaderBgndColor, sizeof(DWORD));
		g_pGetHostAppProperty(HOST_APP_PROPERTY_SKIN_COLOR_HEADER_TEXT, &g_dwHeaderTextColor, sizeof(DWORD));
	}

	g_clientWnd.Load();
	g_clientWnd.EnableOverlay(FALSE);
	g_clientWnd.m_overlay.Load();

	COverlayEditorDlg dlg;

	dlg.m_overlay.Copy(&g_clientWnd.m_overlay);

	g_clientWnd.SetOverlayMessageHandler(&dlg);

	dlg.DoModal();

	g_clientWnd.SetOverlayMessageHandler(NULL);

	g_clientWnd.m_overlay.Copy(&dlg.m_overlayLast);
	g_clientWnd.InitTimer();
	g_clientWnd.Save();
	
	g_clientWnd.EnableOverlay(TRUE);

	if (!IsWindow(g_clientWnd.GetSafeHwnd()))
	{
		g_hal.UninitPingThread();
		g_hal.UninitPollingThread();
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
OVERLAYEDITOR_API void PostOverlayMessage(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParam)
{
	g_clientWnd.OnOverlayMessage(lpMessage, lpLayer, lpParam);
}
//////////////////////////////////////////////////////////////////////
LPCSTR LocalizeStr(LPCSTR lpStr)
{
	if (g_pLocalizeStr)
		return g_pLocalizeStr(lpStr);

	return lpStr;
}
/////////////////////////////////////////////////////////////////////////////
void LocalizeWnd(HWND hWnd)
{
	if (g_pLocalizeWnd)
		g_pLocalizeWnd(hWnd);
}
/////////////////////////////////////////////////////////////////////////////
void LocalizeMenu(HMENU hMenu)
{
	if (g_pLocalizeMenu)
		g_pLocalizeMenu(hMenu);
}
/////////////////////////////////////////////////////////////////////////////
DWORD PickColor(HWND hWnd, DWORD dwColor, LPDWORD lpRecentColors)
{
	if (g_pPickColor)
		return g_pPickColor(hWnd, dwColor, lpRecentColors);

	return dwColor;
}
/////////////////////////////////////////////////////////////////////////////
INT_PTR PickColorEx(HWND hWnd, DWORD dwColor, LPDWORD lpRecentColors, DWORD dwFlags, LPDWORD lpResult)
{
	if (g_pPickColorEx)
		return g_pPickColorEx(hWnd, dwColor, lpRecentColors, dwFlags, lpResult);

	return -1;
}
/////////////////////////////////////////////////////////////////////////////
BOOL GetColorPreview(DWORD dwColor, DWORD dwWidth, DWORD dwHeight, DWORD dwSize, LPBYTE lpImage)
{
	if (g_pGetColorPreview)
		return g_pGetColorPreview(dwColor, dwWidth, dwHeight, dwSize, lpImage);

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CString PickDynamicColor(HWND hWnd, LPCSTR lpColor, LPDWORD lpRecentColors, LPDYNAMIC_COLOR_DESC lpRecentDynamicColors, COverlay* lpOverlay)
{
	CDynamicColor color;
	color.Scan(lpColor);

	enum PICKMODE 
	{
		PICKMODE_NONE		= 0,
		PICKMODE_STATIC		= 1,
		PICKMODE_DYNAMIC	= 2,
	};

	PICKMODE pickMode = color.GetRangeCount() ? PICKMODE_DYNAMIC : PICKMODE_STATIC;

	while (pickMode != PICKMODE_NONE)
	{
		switch (pickMode)
		{
		case PICKMODE_STATIC:
			{
				DWORD dwColor = COverlayLayer::UnpackRGBA(color.m_dwColor);

				INT_PTR nResult = PickColorEx(hWnd, dwColor, lpRecentColors, PICKCOLOREX_FLAG_MORE_BUTTTON, &dwColor);

				color.m_dwColor = COverlayLayer::PackRGBA(dwColor);

				if (IDCONTINUE == nResult)
					pickMode = PICKMODE_DYNAMIC;
				else
				{
					pickMode = PICKMODE_NONE; 

					if (IDOK == nResult)
						color.SetStatic(color.m_dwColor);
				}
			}
			break;
		case PICKMODE_DYNAMIC:
			{
				CDynamicColorPickerDlg dlg;
				dlg.SetOverlay(lpOverlay);
				dlg.SetRecentColors(lpRecentDynamicColors);
				dlg.m_color.Copy(&color);

				INT_PTR nResult = dlg.DoModal();

				if (IDCONTINUE == nResult)
					pickMode = PICKMODE_STATIC;
				else
				{
					pickMode = PICKMODE_NONE;

					if (IDOK == nResult)
						color.Copy(&dlg.m_color);
				}
			}
			break;
		}
	}

	return color.Format();
}
/////////////////////////////////////////////////////////////////////////////
typedef int (WINAPI *SHCREATEDIRECTORYEX)(HWND hwndOwner, LPTSTR lpszPath, SECURITY_ATTRIBUTES *psa);
/////////////////////////////////////////////////////////////////////////////
int CreateDirectory(LPCSTR lpDirectory)
{
	int iResult = 0;

	HMODULE  hModule = LoadLibrary("shell32.dll");

	if (hModule)
	{
		SHCREATEDIRECTORYEX pSHCreateDirectoryEx = (SHCREATEDIRECTORYEX)GetProcAddress(hModule, "SHCreateDirectoryExA");

		if (pSHCreateDirectoryEx)
			iResult = pSHCreateDirectoryEx(NULL, (LPSTR)lpDirectory, NULL);
		else
			iResult = _mkdir(lpDirectory);

		FreeLibrary(hModule);
	}
	else
		iResult = _mkdir(lpDirectory);

	return iResult;
}
/////////////////////////////////////////////////////////////////////////////
void AdjustWindowPos(CWnd* pWnd, CWnd* pParent)
{
	CRect wndRect; pWnd->GetWindowRect(&wndRect);

	if (pParent)
	{
		CRect parentRect; pParent->GetWindowRect(&parentRect);

		wndRect = CRect(parentRect.left, parentRect.top, parentRect.left + wndRect.Width(), parentRect.top + wndRect.Height());
	}

	wndRect.OffsetRect(20, 20);
	
	HMONITOR hMonitor = MonitorFromPoint(wndRect.TopLeft(), MONITOR_DEFAULTTONULL);

	MONITORINFO mi; mi.cbSize = sizeof(MONITORINFO);

	CRect rcWork;

	SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID) &rcWork, 0);

	if (GetMonitorInfo(hMonitor, &mi))
		rcWork = mi.rcWork;
	else
		SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID) &rcWork, 0);

	if (wndRect.bottom > rcWork.bottom)
		wndRect.OffsetRect(0, rcWork.bottom - wndRect.bottom);

	if (wndRect.right > rcWork.right)
		wndRect.OffsetRect(rcWork.right - wndRect.right, 0);

	pWnd->MoveWindow(&wndRect);
}
/////////////////////////////////////////////////////////////////////////////
HANDLE OpenForegroundProcess()
{
	HWND	hWnd		= GetForegroundWindow();
	DWORD	dwProcessId	= 0;

	if (hWnd)
	{
		if (IsFrameWindow(hWnd))
			hWnd = GetCoreWindow(hWnd);

		GetWindowThreadProcessId(hWnd, &dwProcessId);
	}

	if (dwProcessId)
		return OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, dwProcessId);

	return NULL;
}
//////////////////////////////////////////////////////////////////////
BOOL IsFrameWindow(HWND hFrameWnd)
{
	char szClassName[MAX_PATH] = { 0 };

	if (!GetClassName(hFrameWnd, szClassName, sizeof(szClassName)))
		return FALSE;

	return strcmp(szClassName, "ApplicationFrameWindow") == 0;

}
//////////////////////////////////////////////////////////////////////
HWND GetCoreWindow(HWND hFrameWnd)
{
	DWORD dwFrameProcessID = 0;
	GetWindowThreadProcessId(hFrameWnd, &dwFrameProcessID);

	HWND hCoreWnd = FindWindowEx(hFrameWnd, NULL, NULL, NULL);

	while (hCoreWnd) 
	{
		DWORD dwCoreProcessID = 0;
		GetWindowThreadProcessId(hCoreWnd, &dwCoreProcessID);

		if (dwCoreProcessID != dwFrameProcessID)
			return hCoreWnd;

		hCoreWnd = FindWindowEx(hFrameWnd, hCoreWnd, NULL, NULL);
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
