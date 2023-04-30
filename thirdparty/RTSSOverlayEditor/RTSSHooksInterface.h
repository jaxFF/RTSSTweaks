// RTSSHooksInterface.h: interface for the CRTSSHooksInterface class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "Overlay.h"
//////////////////////////////////////////////////////////////////////
typedef	void	(*INITPREVIEW				)(LPCSTR);	
typedef	void	(*RENDERPREVIEWBEGIND3D9	)(LPVOID);	
typedef	void	(*RENDERPREVIEWEND			)();	
typedef	void	(*CLEANUPPREVIEW			)();	
typedef	BOOL	(*GETPREVIEWSTATS			)(DWORD, DWORD, LPBYTE, DWORD);	
typedef	DWORD	(*GETFONTTEXTUREWIDTH		)();
typedef void	(*COPYFONTTEXTURE			)(DWORD, DWORD, LPBYTE);
typedef	void	(*LOADPROFILE				)(LPCSTR);
typedef	void	(*SAVEPROFILE				)(LPCSTR);
typedef	BOOL	(*GETPROFILEPROPERTY		)(LPCSTR, LPBYTE, DWORD);
typedef	BOOL	(*SETPROFILEPROPERTY		)(LPCSTR, LPBYTE, DWORD);
typedef	void	(*DELETEPROFILE				)(LPCSTR);
typedef	void	(*RESETPROFILE				)(LPCSTR);
typedef	void	(*UPDATEPROFILES			)();	
//////////////////////////////////////////////////////////////////////
#define PREVIEWSTATS_PIXEL_WEIGHT									0
#define PREVIEWSTATS_CHAR_WIDTH										1
#define PREVIEWSTATS_CHAR_HEIGHT									2
#define PREVIEWSTATS_LAYER_RECT										3
//////////////////////////////////////////////////////////////////////
class CRTSSHooksInterface
{
public:
	CRTSSHooksInterface();
	virtual ~CRTSSHooksInterface();

	//OSD preview API functionaliry

	void	InitPreview(LPCSTR lpProfile);
		//initializes OSD preview with settings loaded from specified profile or global profile if lpProfile is empty string

		//NOTE: OSD position specified in the profile is ignored, OSD coorditate space origin is always the top left corner
		void	RenderPreviewBeginD3D9(LPVOID lpDevice);
		//begins OSD preview rendering in back buffer of specified Direct3D9 device
	void	RenderPreviewEnd();
		//ends OSD preview rendering, must be called immediately after presenting the frame

		//NOTE: RTSSHooks calculates frametime/framerate performance counters and performs framerate limiting inside this call
	void	CleanupPreview();
		//uninitializes OSD preview infrastructure
	BOOL	GetPreviewStats(DWORD dwStats, DWORD dwParam, LPBYTE lpData, DWORD dwSize);
		//returns preview statistics, calculated during the last RenderPreviewBeginD3D call. The statistics includes pixel
		//weight (OSD zoom ratio), char width and height in pixels for currently selected font and each rendered layer screen rect

	//font texture access functionality

	DWORD	GetFontTextureWidth();
		//returns font texture width in pixels, texture is always square so height is equal to width
	void	CopyFontTexture(DWORD dwWidth, DWORD dwHeight, LPBYTE lpImage);
		//copies font texture data to user specified buffer
		
		//NOTE: pixel format is always 24-bit RGB, pitch must be DWORD aligned and buffer must contain pitch*height bytes

	//profile control functionality

	void LoadProfile(LPCSTR lpProfile);
		//load application specific or global profile, set lpProfile to empty string to load global profile
	void SaveProfile(LPCSTR lpProfile);
		//save application specific or global profile, set lpProfile to empty string to save global profile
	BOOL GetProfileProperty(LPCSTR lpPropertyName, LPBYTE lpPropertyData, DWORD dwPropertySize);
		//get profile property, return FALSE if specified property name is invalid

		//NOTE: you may call this function without previously loding the profile to validate property name

	BOOL SetProfileProperty(LPCSTR lpPropertyName, LPBYTE lpPropertyData, DWORD dwPropertySize);
		//set profile property, return FALSE if specified property name is invalid or value is out of range 

	void DeleteProfile(LPCSTR lpProfile);
		//delete profile
	void ResetProfile(LPCSTR lpProfile);
		//reset profile settings to defaults
	void UpdateProfiles();	
		//force all currently running 3D applications to reload the profiles
};
//////////////////////////////////////////////////////////////////////
