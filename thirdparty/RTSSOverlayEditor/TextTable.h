// TextTable.h: interface for the CTextTable class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include <afxtempl.h>

#include "TextTableLine.h"
//////////////////////////////////////////////////////////////////////
#define DEFAULT_TABLE_VALUE_SIZE		100
#define DEFAULT_TABLE_VALUE_ALIGNMENT	-4
#define DEFAULT_TABLE_UNITS_SIZE		50
#define DEFAULT_TABLE_UNITS_ALIGNMENT	5
//////////////////////////////////////////////////////////////////////
class COverlay;
class CTextTable : public CList<CTextTableLine*, CTextTableLine*>
{
public:
	CTextTable();
	virtual ~CTextTable();

	void	SetOverlay(COverlay* lpOverlay);

	CString	GetHypertext(BOOL bOptimize, DWORD dwColor, int nSize);
	void	ApplyTag(CString& str, CString strBeginTag, CString strEndTag);

	void	AddLine(CTextTableLine* lpLine);
	void	RemoveLine(CTextTableLine* lpLine);
	void	Destroy();

	DWORD	GetTableID();
	void	SetTableID(DWORD dwTableID);

	void	Load();
	void	Save();
	void	Copy(CTextTable* lpSrc);
	BOOL	IsEqual(CTextTable* lpSrc);

	void	UpdateLineIDs(BOOL bUpdateCellIDs);

	CTextTableLine* FindLine(DWORD dwLineID);
	CTextTableCell* FindCell(DWORD dwLineID, DWORD dwCellID);
	CTextTableCell* GetCell(DWORD dwIndex);

	void	Reorder(CList<CTextTableLine*, CTextTableLine*>* lpOrder);
	void	Reorder(CList<CTextTableCell*, CTextTableCell*>* lpOrder);
	void	RemoveEmptyLines();

	CString m_strName;

	int		m_nValueSize;
	int		m_nValueAlignment;

	int		m_nUnitsSize;
	int		m_nUnitsAlignment;

	BOOL	m_bSelected;

protected:
	COverlay* m_lpOverlay;

	DWORD	m_dwTableID;
};
//////////////////////////////////////////////////////////////////////
