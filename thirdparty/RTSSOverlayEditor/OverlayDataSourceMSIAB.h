// OverlayDataSourceMSIAB.h: interface for the COverlayDataSourceMSIAB class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataSource.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceMSIAB :	public COverlayDataSource
{
public:
	COverlayDataSourceMSIAB();
	virtual ~COverlayDataSourceMSIAB();

	//COverlayDataSource virtual functions

	virtual CString GetProviderName();

	virtual void Save();
	virtual void Load();
	virtual void Copy(COverlayDataSource* lpSrc);
	virtual BOOL IsEqual(COverlayDataSource* lpSrc);

	//COverlayDataSourceMSIAB specific functions

	DWORD	GetSrcId();
	void	SetSrcId(DWORD dwSrcId);

	DWORD	GetGpu();
	void	SetGpu(DWORD dwGpu);

	CString GetSrcName();
	void	SetSrcName(LPCSTR lpSrcName);

protected:
	DWORD	m_dwSrcId;
	DWORD	m_dwGpu;
	CString m_strSrcName;
};
/////////////////////////////////////////////////////////////////////////////
