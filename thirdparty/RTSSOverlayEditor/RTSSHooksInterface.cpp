// RTSSHooksInterface.cpp: implementation of the CRTSSHooksInterface class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "RTSSHooksInterface.h"
//////////////////////////////////////////////////////////////////////
CRTSSHooksInterface::CRTSSHooksInterface()
{
}
//////////////////////////////////////////////////////////////////////
CRTSSHooksInterface::~CRTSSHooksInterface()
{
}
//////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::InitPreview(LPCSTR lpProfile)
{
	DWORD	dwResult	= 0;

	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		INITPREVIEW pInitPreview = (INITPREVIEW)GetProcAddress(hRTSSHooks, "InitPreview");

		if (pInitPreview)
			pInitPreview(lpProfile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::RenderPreviewBeginD3D9(LPVOID lpDevice)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		RENDERPREVIEWBEGIND3D9 pRenderPreviewBeginD3D9 = (RENDERPREVIEWBEGIND3D9)GetProcAddress(hRTSSHooks, "RenderPreviewBeginD3D9");

		if (pRenderPreviewBeginD3D9)
			pRenderPreviewBeginD3D9(lpDevice);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::RenderPreviewEnd()
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		RENDERPREVIEWEND pRenderPreviewEnd = (RENDERPREVIEWEND)GetProcAddress(hRTSSHooks, "RenderPreviewEnd");

		if (pRenderPreviewEnd)
			pRenderPreviewEnd();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::CleanupPreview()
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		CLEANUPPREVIEW pCleanupPreview = (CLEANUPPREVIEW)GetProcAddress(hRTSSHooks, "CleanupPreview");

		if (pCleanupPreview)
			pCleanupPreview();
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRTSSHooksInterface::GetPreviewStats(DWORD dwStats, DWORD dwParam, LPBYTE lpData, DWORD dwSize)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		GETPREVIEWSTATS pGetPreviewStats = (GETPREVIEWSTATS)GetProcAddress(hRTSSHooks, "GetPreviewStats");

		if (pGetPreviewStats)
			return pGetPreviewStats(dwStats, dwParam, lpData, dwSize);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CRTSSHooksInterface::GetFontTextureWidth()
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		GETFONTTEXTUREWIDTH pGetFontTextureWidth = (GETFONTTEXTUREWIDTH)GetProcAddress(hRTSSHooks, "GetFontTextureWidth");

		if (pGetFontTextureWidth)
			return pGetFontTextureWidth();
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::CopyFontTexture(DWORD dwWidth, DWORD dwHeight, LPBYTE lpImage)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		COPYFONTTEXTURE pCopyFontTexture = (COPYFONTTEXTURE)GetProcAddress(hRTSSHooks, "CopyFontTexture");

		if (pCopyFontTexture)
			pCopyFontTexture(dwWidth, dwHeight, lpImage);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::LoadProfile(LPCSTR lpProfile)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		LOADPROFILE pLoadProfile = (LOADPROFILE)GetProcAddress(hRTSSHooks, "LoadProfile");

		if (pLoadProfile)
			pLoadProfile(lpProfile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::SaveProfile(LPCSTR lpProfile)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		SAVEPROFILE pSaveProfile = (SAVEPROFILE)GetProcAddress(hRTSSHooks, "SaveProfile");

		if (pSaveProfile)
			pSaveProfile(lpProfile);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRTSSHooksInterface::GetProfileProperty(LPCSTR lpPropertyName, LPBYTE lpPropertyData, DWORD dwPropertySize)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		GETPROFILEPROPERTY pGetProfileProperty = (GETPROFILEPROPERTY)GetProcAddress(hRTSSHooks, "GetProfileProperty");

		if (pGetProfileProperty)
			return pGetProfileProperty(lpPropertyName, lpPropertyData, dwPropertySize);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRTSSHooksInterface::SetProfileProperty(LPCSTR lpPropertyName, LPBYTE lpPropertyData, DWORD dwPropertySize)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		SETPROFILEPROPERTY pSetProfileProperty = (SETPROFILEPROPERTY)GetProcAddress(hRTSSHooks, "SetProfileProperty");

		if (pSetProfileProperty)
			return pSetProfileProperty(lpPropertyName, lpPropertyData, dwPropertySize);
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::DeleteProfile(LPCSTR lpProfile)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		DELETEPROFILE pDeleteProfile = (DELETEPROFILE)GetProcAddress(hRTSSHooks, "DeleteProfile");

		if (pDeleteProfile)
			pDeleteProfile(lpProfile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::ResetProfile(LPCSTR lpProfile)
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		RESETPROFILE pResetProfile = (RESETPROFILE)GetProcAddress(hRTSSHooks, "ResetProfile");

		if (pResetProfile)
			pResetProfile(lpProfile);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSHooksInterface::UpdateProfiles()
{
	HMODULE hRTSSHooks	= GetModuleHandle("RTSSHooks.dll");

	if (hRTSSHooks)
	{
		UPDATEPROFILES pUpdateProfiles = (UPDATEPROFILES)GetProcAddress(hRTSSHooks, "UpdateProfiles");

		if (pUpdateProfiles)
			pUpdateProfiles();
	}
}
/////////////////////////////////////////////////////////////////////////////
