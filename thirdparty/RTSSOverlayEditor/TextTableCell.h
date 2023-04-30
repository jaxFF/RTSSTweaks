// TextTableCell.h: interface for the CTextTableCell class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "DynamicColor.h"
//////////////////////////////////////////////////////////////////////
class COverlay;
class CTextTableCell
{
public:
	CTextTableCell();
	virtual ~CTextTableCell();

	DWORD GetDynamicColor(CDynamicColor& color);

	void SetOverlay(COverlay* lpOverlay);

	DWORD GetTableID();
	void SetTableID(DWORD dwTableID);

	DWORD GetLineID();
	void SetLineID(DWORD dwLineID);

	DWORD GetCellID();
	void SetCellID(DWORD dwCellID);
	
	void Load();
	void Save();
	void Copy(CTextTableCell* lpSrc);
	BOOL IsEqual(CTextTableCell* lpSrc);

	CString m_strSource;
	CString m_strText;
	CString	m_strColor;

	BOOL	m_bSelected;

protected:
	COverlay* m_lpOverlay;

	DWORD m_dwTableID;
	DWORD m_dwLineID;
	DWORD m_dwCellID;
};
//////////////////////////////////////////////////////////////////////
