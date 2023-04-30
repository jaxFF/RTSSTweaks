// TextTableCell.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TextTableCell.h"
#include "OverlayLayer.h"
#include "Overlay.h"
//////////////////////////////////////////////////////////////////////
CTextTableCell::CTextTableCell()
{
	m_lpOverlay	= NULL;
		
	m_strSource	= "";
	m_strText	= "";
	m_strColor	= "";

	m_dwTableID	= 0;
	m_dwLineID	= 0;
	m_dwCellID	= 0;

	m_bSelected	= FALSE;
}
//////////////////////////////////////////////////////////////////////
CTextTableCell::~CTextTableCell()
{
}
//////////////////////////////////////////////////////////////////////
void CTextTableCell::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay		= lpOverlay;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCell::Copy(CTextTableCell* lpSrc)
{
	m_strSource		= lpSrc->m_strSource;
	m_strText		= lpSrc->m_strText;
	m_strColor		= lpSrc->m_strColor;
}
//////////////////////////////////////////////////////////////////////
BOOL CTextTableCell::IsEqual(CTextTableCell* lpSrc)
{
	if (strcmp(m_strSource, lpSrc->m_strSource))
		return FALSE;
	if (strcmp(m_strText, lpSrc->m_strText))
		return FALSE;
	if (strcmp(m_strColor, lpSrc->m_strColor))
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableCell::GetTableID()
{
	return m_dwTableID;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCell::SetTableID(DWORD dwTableID)
{
	m_dwTableID = dwTableID;
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableCell::GetLineID()
{
	return m_dwLineID;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCell::SetLineID(DWORD dwLineID)
{
	m_dwLineID = dwLineID;
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableCell::GetCellID()
{
	return m_dwCellID;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCell::SetCellID(DWORD dwCellID)
{
	m_dwCellID = dwCellID;
}
//////////////////////////////////////////////////////////////////////
void CTextTableCell::Load()
{
	if (m_lpOverlay)
	{
		CString strTable;
		strTable.Format("Table%d", m_dwTableID);

		CString strCell;
		strCell.Format("Line%dCell%d", m_dwLineID, m_dwCellID);

		m_strText			= m_lpOverlay->GetConfigStr(strTable, strCell + "Text"		, ""	);
		m_strSource			= m_lpOverlay->GetConfigStr(strTable, strCell + "Source"	, ""	);
		m_strColor			= m_lpOverlay->GetConfigStr(strTable, strCell + "Color"		, ""	);
	}
}
//////////////////////////////////////////////////////////////////////
void CTextTableCell::Save()
{
	if (m_lpOverlay)
	{
		CString strTable;
		strTable.Format("Table%d", m_dwTableID);

		CString strCell;
		strCell.Format("Line%dCell%d", m_dwLineID, m_dwCellID);

		if (m_strSource.IsEmpty())
			m_lpOverlay->SetConfigStr(strTable, strCell + "Text"	, m_strText			);
		else
			m_lpOverlay->SetConfigStr(strTable, strCell + "Source"	, m_strSource		);

		if (strlen(m_strColor))
			m_lpOverlay->SetConfigStr(strTable, strCell + "Color"	, m_strColor		);
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTableCell::GetDynamicColor(CDynamicColor& color)
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
