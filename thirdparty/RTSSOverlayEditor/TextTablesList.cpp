// TextTablesList.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TextTablesList.h"
#include "Overlay.h"
//////////////////////////////////////////////////////////////////////
CTextTablesList::CTextTablesList()
{
	m_lpOverlay = NULL;
}
//////////////////////////////////////////////////////////////////////
CTextTablesList::~CTextTablesList()
{
	Destroy();
}
//////////////////////////////////////////////////////////////////////
void CTextTablesList::Destroy()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		delete GetNext(pos);

	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
void CTextTablesList::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetOverlay(lpOverlay);
}
//////////////////////////////////////////////////////////////////////
void CTextTablesList::AddTable(CTextTable* lpTable)
{
	lpTable->SetTableID(GetCount());
	lpTable->SetOverlay(m_lpOverlay);
	AddTail(lpTable);
}
/////////////////////////////////////////////////////////////////////
void CTextTablesList::RemoveTable(CTextTable* lpTable)
{
	POSITION pos = Find(lpTable);

	if (pos)
	{
		RemoveAt(pos);

		delete lpTable;
	}
}
/////////////////////////////////////////////////////////////////////
CTextTable* CTextTablesList::FindTable(LPCSTR lpName)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CTextTable* lpTable	= GetNext(pos);

		if (!strcmp(lpTable->m_strName, lpName))
			return lpTable;
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////
CTextTable* CTextTablesList::FindTable(DWORD dwTableID)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CTextTable* lpTable	= GetNext(pos);

		if (lpTable->GetTableID() == dwTableID)
			return lpTable;
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////
void CTextTablesList::Copy(CTextTablesList* lpSrc)
{
	Destroy();

	POSITION pos = lpSrc->GetHeadPosition();

	while (pos)
	{
		CTextTable* lpTable		= lpSrc->GetNext(pos);
		CTextTable* lpTableCopy	= new CTextTable;

		lpTableCopy->Copy(lpTable);

		AddTable(lpTableCopy);	
	}
}
/////////////////////////////////////////////////////////////////////
BOOL CTextTablesList::IsEqual(CTextTablesList* lpSrc)
{
	POSITION posSrc = lpSrc->GetHeadPosition();
	POSITION posOwn	= GetHeadPosition();

	while (posSrc && posOwn)
	{
		CTextTable* lpSrcTable = lpSrc->GetNext(posSrc);
		CTextTable* lpOwnTable = GetNext(posOwn);

		if (!lpOwnTable->IsEqual(lpSrcTable))
			return FALSE;
	}

	if (posSrc || posOwn)
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////
void CTextTablesList::Load()
{
	Destroy();

	if (m_lpOverlay)
	{
		int nTables = m_lpOverlay->GetConfigInt("General", "Tables", 0);

		for (int nTable=0; nTable<nTables; nTable++)
		{
			CTextTable* lpTable = new CTextTable;

			lpTable->SetTableID(nTable);
			lpTable->SetOverlay(m_lpOverlay);
			lpTable->Load();

			AddTable(lpTable);
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CTextTablesList::Save()
{
	if (m_lpOverlay)
	{
		m_lpOverlay->SetConfigInt("General", "Tables", GetCount());

		POSITION pos = GetHeadPosition();

		while (pos)
		{
			CTextTable* lpTable = GetNext(pos);

			lpTable->Save();
		}
	}
}
/////////////////////////////////////////////////////////////////////
void CTextTablesList::Reorder(CList<CTextTable*, CTextTable*>* lpOrder)
{
	//remove tables ptr without deleting the objects

	RemoveAll();

	//reorder the list

	POSITION pos = lpOrder->GetHeadPosition();

	while (pos)
	{
		CTextTable* lpTable = lpOrder->GetNext(pos);

		AddTable(lpTable);
	}
}
/////////////////////////////////////////////////////////////////////
void CTextTablesList::UpdateTableIDs()
{
	DWORD dwTableID = 0;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetTableID(dwTableID++);
}
/////////////////////////////////////////////////////////////////////////////

