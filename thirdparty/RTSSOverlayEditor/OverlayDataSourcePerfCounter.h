// OverlayDataSourcePerfCounter.h: interface for the COverlayDataSourcePerfCounter class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataSource.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourcePerfCounter : public COverlayDataSource
{
public:
	COverlayDataSourcePerfCounter();
	virtual ~COverlayDataSourcePerfCounter();

	//COverlayDataSource virtual functions

	virtual CString GetProviderName();

	virtual void Save();
	virtual void Load();
	virtual void Copy(COverlayDataSource* lpSrc);
	virtual BOOL IsEqual(COverlayDataSource* lpSrc);

	//COverlayDataSourcePerfCounter specific functions

	CString	GetObjectName();
	void SetObjectName(LPCSTR lpObjectName);

	CString	GetInstanceName();
	void SetInstanceName(LPCSTR lpInstanceName);

	DWORD GetInstanceIndex();
	void SetInstanceIndex(LONG dwInstanceIndex);

	CString	GetCounterName();
	void SetCounterName(LPCSTR lpCounterName);

	BOOL IsDynamic();
	void SetDynamic(BOOL bDynamic);

protected:
	CString m_strObjectName;
	CString	m_strInstanceName;
	LONG	m_dwInstanceIndex;
	CString	m_strCounterName;

	BOOL	m_bDynamic;
};
/////////////////////////////////////////////////////////////////////////////
