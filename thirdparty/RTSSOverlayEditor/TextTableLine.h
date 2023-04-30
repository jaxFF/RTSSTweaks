// TextTableLine.h: interface for the CTextTableLine class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include <afxtempl.h>

#include "TextTableCell.h"
#include "DynamicColor.h"
//////////////////////////////////////////////////////////////////////
class COverlay;
class CTextTableLine : public CList<CTextTableCell*, CTextTableCell*>
{
public:
	CTextTableLine();
	virtual ~CTextTableLine();

	DWORD GetDynamicColor(CDynamicColor& color);

	void SetOverlay(COverlay* lpOverlay);

	void AddCell(CTextTableCell* lpCell);
	void RemoveCell(CTextTableCell* lpCell);
	void Destroy();

	DWORD GetTableID();
	void SetTableID(DWORD dwTableID);

	DWORD GetLineID();
	void SetLineID(DWORD dwLineID);

	void Load();
	void Save();
	void Copy(CTextTableLine* lpSrc);
	BOOL IsEqual(CTextTableLine* lpSrc);

	void UpdateCellIDs();

	CTextTableCell* FindCell(DWORD dwCellID);

	CString m_strName;
	CString	m_strColor;

	BOOL	m_bSelected;

protected:
	COverlay* m_lpOverlay;

	DWORD	m_dwTableID;
	DWORD	m_dwLineID;
};
//////////////////////////////////////////////////////////////////////
