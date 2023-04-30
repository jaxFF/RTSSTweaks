// HALDataSourcesList.h: interface for the CHALDataSourcesList class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include <afxtempl.h>

#include "HALDataSource.h"
//////////////////////////////////////////////////////////////////////
class CHAL;
class CHALDataSourcesList : public CList<CHALDataSource*, CHALDataSource*>
{
public:
	CHALDataSourcesList();
	virtual ~CHALDataSourcesList();

	void Init(CHAL* lpHAL);
	void Destroy();

	CHALDataSource* FindSource(LPCSTR lpName);

protected:
	CHAL* m_lpHAL;

	void AddSource(LPCSTR lpName, LPCSTR lpUnits, LPCSTR lpGroup, DWORD dwID, DWORD dwParam0 = 0, DWORD dwParam1 = 0);
};
//////////////////////////////////////////////////////////////////////
