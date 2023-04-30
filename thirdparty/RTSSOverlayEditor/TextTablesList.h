// TextTablesList.h: interface for the CTextTablesList class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include <afxtempl.h>

#include "TextTable.h"
//////////////////////////////////////////////////////////////////////
class COverlay;
class CTextTablesList : public CList<CTextTable*, CTextTable*>
{
public:
	CTextTablesList();
	virtual ~CTextTablesList();

	void SetOverlay(COverlay* lpOverlay);

	void AddTable(CTextTable* lpTable);
	void RemoveTable(CTextTable* lpTable);
	void UpdateTableIDs();
	void Destroy();

	CTextTable* FindTable(LPCSTR lpName);
	CTextTable* FindTable(DWORD dwTableID);

	void Load();
	void Save();
	void Copy(CTextTablesList* lpSrc);
	BOOL IsEqual(CTextTablesList* lpSrc);

	void Reorder(CList<CTextTable*, CTextTable*>* lpOrder);

protected:
	COverlay* m_lpOverlay;
};
//////////////////////////////////////////////////////////////////////
