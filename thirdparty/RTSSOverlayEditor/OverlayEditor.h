#ifdef OVERLAYEDITOR_EXPORTS
#define OVERLAYEDITOR_API extern "C" __declspec(dllexport)
#else
#define OVERLAYEDITOR_API extern "C" __declspec(dllimport)
#endif
//////////////////////////////////////////////////////////////////////
#include "RTSSExports.h"
#include "RTCoreDriver.h"
#include "HAL.h"
#include "OverlayEditorClientWnd.h"
#include "Log.h"
//////////////////////////////////////////////////////////////////////
extern HINSTANCE					g_hModule;

//host API functions

extern GET_HOST_APP_PROPERTY_PROC	g_pGetHostAppProperty;
extern LOCALIZEWND_PROC				g_pLocalizeWnd;
extern LOCALIZEMENU_PROC			g_pLocalizeMenu;
extern LOCALIZESTR_PROC				g_pLocalizeStr;
extern PICKCOLOR_PROC				g_pPickColor;
extern PICKCOLOREX_PROC				g_pPickColorEx;
extern GETCOLORPREVIEW_PROC			g_pGetColorPreview;

//host skin color scheme 

extern DWORD						g_dwHeaderBgndColor;
extern DWORD						g_dwHeaderTextColor;

//main client window

extern CRTCoreDriver				g_driver;
extern CHAL							g_hal;
extern COverlayEditorClientWnd		g_clientWnd;

//debug logger

#ifdef TEST_VERSION
extern CLog							g_log;
#endif
//////////////////////////////////////////////////////////////////////
int		CreateDirectory(LPCSTR lpDirectory);
	//helper for screen/video capture folder creation
LPCSTR	LocalizeStr(LPCSTR lpStr);
void	LocalizeWnd(HWND hWnd);
void	LocalizeMenu(HMENU hMenu);
	//wrapppers for host localization API
DWORD	PickColor(HWND hWnd, DWORD dwColor, LPDWORD lpRecentColors);
	//wrapper for host color picker functionality
INT_PTR	PickColorEx(HWND hWnd, DWORD dwColor, LPDWORD lpRecentColors, DWORD dwFlags, LPDWORD lpResult);
	//wrapper for host extended color picker functionality
BOOL GetColorPreview(DWORD dwColor, DWORD dwWidth, DWORD dwHeight, DWORD dwSize, LPBYTE lpImage);
	//wrapper for host color preview functionality
CString PickDynamicColor(HWND hWnd, LPCSTR lpColor, LPDWORD lpRecentColors, LPDYNAMIC_COLOR_DESC lpRecentDynamicColors, COverlay* lpOverlay);
	//dynamic color picker implementation
void	AdjustWindowPos(CWnd* pWnd, CWnd* pParent);
	//helper for child window position adjustment
HANDLE	OpenForegroundProcess();
	//helper for opening foreground process handle
BOOL IsFrameWindow(HWND hFrameWnd);
	//helper for checking if window is UWP frame window
HWND GetCoreWindow(HWND hFrameWnd);
	//helper for returning UWP core window from UWP frame window
//////////////////////////////////////////////////////////////////////

