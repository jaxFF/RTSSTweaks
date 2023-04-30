// OverlayDataSourceHwInfo.h: interface for the COverlayDataSourceHwInfo class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataSource.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceHwInfo : public COverlayDataSource
{
public:
	COverlayDataSourceHwInfo();
	virtual ~COverlayDataSourceHwInfo();

	//COverlayDataSource virtual functions

	virtual CString GetProviderName();

	virtual void Save();
	virtual void Load();
	virtual void Copy(COverlayDataSource* lpSrc);
	virtual BOOL IsEqual(COverlayDataSource* lpSrc);

	//COverlayDataSourceHwInfo specific functions

	DWORD	GetSensorInst();
	void	SetSensorInst(DWORD dwSensorInst);

	DWORD	GetReadingType();
	void	SetReadingType(DWORD dwReadingType);

	CString GetReadingName();
	void	SetReadingName(LPCSTR lpReadingName);

protected:
	DWORD	m_dwSensorInst;
	DWORD	m_dwReadingType;
	CString	m_strReadingName;
};
/////////////////////////////////////////////////////////////////////////////
