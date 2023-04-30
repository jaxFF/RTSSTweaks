// D3DKMTDeviceList.h: interface for the CD3DKMTDeviceList class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _D3DKMTDEVICELIST_H_INCLUDED_
#define _D3DKMTDEVICELIST_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#include <afxtempl.h>
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
typedef struct D3DKMTDEVICE_DESC
{
	char			DeviceName[MAX_PATH];
	char			DeviceDesc[MAX_PATH];
	char			HardwareID[MAX_PATH];
	DWORD			Bus;
	DWORD			Dev;
	DWORD			Fn;
	DWORD			VendorID;
	DWORD			DeviceID;
	DWORD			SubsysID;
	DWORD			Revision;
} D3DKMTDEVICE_DESC, *LPD3DKMTDEVICE_DESC;
//////////////////////////////////////////////////////////////////////
class CD3DKMTDeviceList : public CList<LPD3DKMTDEVICE_DESC, LPD3DKMTDEVICE_DESC>
{
public:
	CD3DKMTDeviceList();
	virtual ~CD3DKMTDeviceList();

	BOOL Init();
	void Uninit();
};
//////////////////////////////////////////////////////////////////////
#endif 
//////////////////////////////////////////////////////////////////////
