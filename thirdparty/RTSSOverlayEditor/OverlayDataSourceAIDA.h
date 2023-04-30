// OverlayDataSourceAIDA.h: interface for the COverlayDataSourceAIDA class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataSource.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceAIDA : public COverlayDataSource
{
public:
	COverlayDataSourceAIDA();
	virtual ~COverlayDataSourceAIDA();

	//COverlayDataSource virtual functions

	virtual CString GetProviderName();

	virtual void Save();
	virtual void Load();
	virtual void Copy(COverlayDataSource* lpSrc);
	virtual BOOL IsEqual(COverlayDataSource* lpSrc);

	//COverlayDataSourceAIDA specific functions

	CString	GetID();
	void SetID(LPCSTR lpID);

protected:
	CString m_strID;
};
/////////////////////////////////////////////////////////////////////////////
