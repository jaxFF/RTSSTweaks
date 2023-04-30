// OverlayDataSourceHAL.h: interface for the COverlayDataSourceHAL class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataSource.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceHAL : public COverlayDataSource
{
public:
	COverlayDataSourceHAL();
	virtual ~COverlayDataSourceHAL();

	//COverlayDataSource virtual functions

	virtual CString GetProviderName();

	virtual void Save();
	virtual void Load();
	virtual void Copy(COverlayDataSource* lpSrc);
	virtual BOOL IsEqual(COverlayDataSource* lpSrc);

	//COverlayDataSourceHAL specific functions

	CString	GetID();
	void SetID(LPCSTR lpID);

protected:
	CString m_strID;
};
/////////////////////////////////////////////////////////////////////////////
