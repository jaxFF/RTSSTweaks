// TextTableLine.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TextTableLine.h"
#include "OverlayLayer.h"
#include "Overlay.h"
//////////////////////////////////////////////////////////////////////
CTextTableLine::CTextTableLine()
{
	m_lpOverlay		= NULL;

	m_strName		= "";
	m_strColor		= "";

	m_dwTableID		= 0;
	m_dwLineID		= 0;

	m_bSelected		= FALSE;
}
//////////////////////////////////////////////////////////////////////
CTextTableLine::~CTextTableLine()
{
	Destroy();
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay	= lpOverlay;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetOverlay(lpOverlay);
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::Destroy()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		delete GetNext(pos);

	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::AddCell(CTextTableCell* lpCell)
{
	lpCell->SetTableID(m_dwTableID);
	lpCell->SetLineID(m_dwLineID);
	lpCell->SetCellID(GetCount());
	lpCell->SetOverlay(m_lpOverlay);
	AddTail(lpCell);
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::RemoveCell(CTextTableCell* lpCell)
{
	POSITION pos = Find(lpCell);

	if (pos)
	{
		RemoveAt(pos);

		delete lpCell;
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableLine::GetTableID()
{
	return m_dwTableID;
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::SetTableID(DWORD dwTableID)
{
	m_dwTableID = dwTableID;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetTableID(dwTableID);
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableLine::GetLineID()
{
	return m_dwLineID;
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::SetLineID(DWORD dwLineID)
{
	m_dwLineID = dwLineID;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetLineID(dwLineID);
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::Copy(CTextTableLine* lpSrc)
{
	Destroy();

	m_strName	= lpSrc->m_strName;
	m_strColor	= lpSrc->m_strColor;

	POSITION pos = lpSrc->GetHeadPosition();

	while (pos)
	{
		CTextTableCell* lpCell		= lpSrc->GetNext(pos);
		CTextTableCell* lpCellCopy	= new CTextTableCell;

		lpCellCopy->Copy(lpCell);

		AddCell(lpCellCopy);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CTextTableLine::IsEqual(CTextTableLine* lpSrc)
{
	if (strcmp(m_strName, lpSrc->m_strName))
		return FALSE;
	if (strcmp(m_strColor, lpSrc->m_strColor))
		return FALSE;

	POSITION posSrc = lpSrc->GetHeadPosition();
	POSITION posOwn	= GetHeadPosition();

	while (posSrc && posOwn)
	{
		CTextTableCell* lpSrcCell = lpSrc->GetNext(posSrc);
		CTextTableCell* lpOwnCell = GetNext(posOwn);

		if (!lpOwnCell->IsEqual(lpSrcCell))
			return FALSE;
	}

	if (posSrc || posOwn)
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::Load()
{
	Destroy();

	if (m_lpOverlay)
	{
		CString strTable;
		strTable.Format("Table%d", m_dwTableID);

		CString strLine;
		strLine.Format("Line%d", m_dwLineID);

		m_strName	= m_lpOverlay->GetConfigStr(strTable, strLine + "Name"	, ""							);
		m_strColor	= m_lpOverlay->GetConfigStr(strTable, strLine + "Color"	, ""							);

		int nCells	= m_lpOverlay->GetConfigInt(strTable, strLine + "Cells"	, 0								);

		for (int nCell=0; nCell<nCells; nCell++)
		{
			CTextTableCell* lpCell = new CTextTableCell;

			lpCell->SetTableID(m_dwTableID);
			lpCell->SetLineID(m_dwLineID);
			lpCell->SetCellID(nCell);
			lpCell->SetOverlay(m_lpOverlay);
			lpCell->Load();

			AddCell(lpCell);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::Save()
{
	if (m_lpOverlay)
	{
		CString strTable;
		strTable.Format("Table%d", m_dwTableID);

		CString strLine;
		strLine.Format("Line%d", m_dwLineID);

		m_lpOverlay->SetConfigStr(strTable, strLine + "Name"			, m_strName		);

		if (strlen(m_strColor))
			m_lpOverlay->SetConfigStr(strTable, strLine + "Color"		, m_strColor	);

		m_lpOverlay->SetConfigInt(strTable, strLine + "Cells"			, GetCount()	);

		POSITION pos = GetHeadPosition();

		while (pos)
		{
			CTextTableCell* lpCell = GetNext(pos);

			lpCell->Save();
		}
	}
}
//////////////////////////////////////////////////////////////////////
CTextTableCell* CTextTableLine::FindCell(DWORD dwCellID)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CTextTableCell* lpCell = GetNext(pos);

		if (lpCell->GetCellID() == dwCellID)
			return lpCell;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void CTextTableLine::UpdateCellIDs()
{
	DWORD dwCellID = 0;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetCellID(dwCellID++);
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableLine::GetDynamicColor(CDynamicColor& color)
{
	if (strlen(m_strColor))
	{
		color.Scan(m_strColor);
		color.Update(m_lpOverlay);
	}
	else
		color.SetStatic(DEFAULT_COLOR);

	return color.m_dwColor;
}
//////////////////////////////////////////////////////////////////////
