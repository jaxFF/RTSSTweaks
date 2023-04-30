// TextTable.cpp: implementation file
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TextTable.h"
#include "OverlayLayer.h"
#include "Overlay.h"

#include <float.h>
//////////////////////////////////////////////////////////////////////
CTextTable::CTextTable()
{
	m_lpOverlay			= NULL;

	m_strName			= "";

	m_nValueSize		= DEFAULT_TABLE_VALUE_SIZE;
	m_nValueAlignment	= DEFAULT_TABLE_VALUE_ALIGNMENT;

	m_nUnitsSize		= DEFAULT_TABLE_UNITS_SIZE;
	m_nUnitsAlignment	= DEFAULT_TABLE_UNITS_ALIGNMENT;

	m_dwTableID			= 0;

	m_bSelected			= FALSE;
}
//////////////////////////////////////////////////////////////////////
CTextTable::~CTextTable()
{
	Destroy();
}
//////////////////////////////////////////////////////////////////////
void CTextTable::Destroy()
{
	POSITION pos = GetHeadPosition();

	while (pos)
		delete GetNext(pos);

	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
void CTextTable::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetOverlay(lpOverlay);
}
//////////////////////////////////////////////////////////////////////
void CTextTable::AddLine(CTextTableLine* lpLine)
{
	lpLine->SetTableID(m_dwTableID);
	lpLine->SetLineID(GetCount());
	lpLine->SetOverlay(m_lpOverlay);
	AddTail(lpLine);
}
//////////////////////////////////////////////////////////////////////
void CTextTable::RemoveLine(CTextTableLine* lpLine)
{
	POSITION pos = Find(lpLine);

	if (pos)
	{
		RemoveAt(pos);

		delete lpLine;
	}
}
//////////////////////////////////////////////////////////////////////
void CTextTable::Copy(CTextTable* lpSrc)
{
	Destroy();

	m_strName			= lpSrc->m_strName;

	m_nValueSize		= lpSrc->m_nValueSize;
	m_nValueAlignment	= lpSrc->m_nValueAlignment;

	m_nUnitsSize		= lpSrc->m_nUnitsSize;
	m_nUnitsAlignment	= lpSrc->m_nUnitsAlignment;

	POSITION pos = lpSrc->GetHeadPosition();

	while (pos)
	{
		CTextTableLine* lpLine		= lpSrc->GetNext(pos);
		CTextTableLine* lpLineCopy	= new CTextTableLine;

		lpLineCopy->Copy(lpLine);

		AddLine(lpLineCopy);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CTextTable::IsEqual(CTextTable* lpSrc)
{
	if (strcmp(m_strName, lpSrc->m_strName))
		return FALSE;

	if (m_nValueSize != lpSrc->m_nValueSize)
		return FALSE;
	if (m_nValueAlignment != lpSrc->m_nValueAlignment)
		return FALSE;

	if (m_nUnitsSize != lpSrc->m_nUnitsSize)
		return FALSE;
	if (m_nUnitsAlignment != lpSrc->m_nUnitsAlignment)
		return FALSE;

	POSITION posSrc = lpSrc->GetHeadPosition();
	POSITION posOwn	= GetHeadPosition();

	while (posSrc && posOwn)
	{
		CTextTableLine* lpSrcLine = lpSrc->GetNext(posSrc);
		CTextTableLine* lpOwnLine = GetNext(posOwn);

		if (!lpOwnLine->IsEqual(lpSrcLine))
			return FALSE;
	}

	if (posSrc || posOwn)
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
DWORD CTextTable::GetTableID()
{
	return m_dwTableID;
}
//////////////////////////////////////////////////////////////////////
void CTextTable::SetTableID(DWORD dwTableID)
{
	m_dwTableID = dwTableID;

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->SetTableID(dwTableID);
}
//////////////////////////////////////////////////////////////////////
void CTextTable::Load()
{
	Destroy();

	if (m_lpOverlay)
	{
		CString strTable;
		strTable.Format("Table%d", m_dwTableID);

		m_strName			= m_lpOverlay->GetConfigStr(strTable, "Name"			, ""							);
		m_nValueSize		= m_lpOverlay->GetConfigInt(strTable, "ValueSize"		, DEFAULT_TABLE_VALUE_SIZE		);
		m_nValueAlignment	= m_lpOverlay->GetConfigInt(strTable, "ValueAlignment"	, DEFAULT_TABLE_VALUE_ALIGNMENT	);
		m_nUnitsSize		= m_lpOverlay->GetConfigInt(strTable, "UnitsSize"		, DEFAULT_TABLE_UNITS_SIZE		);
		m_nUnitsAlignment	= m_lpOverlay->GetConfigInt(strTable, "UnitsAlignment"	, DEFAULT_TABLE_UNITS_ALIGNMENT	);

		int nLines			= m_lpOverlay->GetConfigInt(strTable, "Lines"			, 0								);

		for (int nLine=0; nLine<nLines; nLine++)
		{
			CTextTableLine* lpLine = new CTextTableLine;

			lpLine->SetTableID(m_dwTableID);
			lpLine->SetLineID(nLine);
			lpLine->SetOverlay(m_lpOverlay);
			lpLine->Load();

			AddLine(lpLine);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CTextTable::Save()
{
	if (m_lpOverlay)
	{
		CString strTable;
		strTable.Format("Table%d", m_dwTableID);

		m_lpOverlay->SetConfigStr(strTable, "Name"				, m_strName			);

		if (m_nValueSize != DEFAULT_TABLE_VALUE_SIZE)
			m_lpOverlay->SetConfigInt(strTable, "ValueSize"		, m_nValueSize		);
		if (m_nValueAlignment != DEFAULT_TABLE_VALUE_ALIGNMENT)
			m_lpOverlay->SetConfigInt(strTable, "ValueAlignment", m_nValueAlignment	);
		if (m_nUnitsSize != DEFAULT_TABLE_UNITS_SIZE)
			m_lpOverlay->SetConfigInt(strTable, "UnitsSize"		, m_nUnitsSize		);
		if (m_nUnitsAlignment != DEFAULT_TABLE_UNITS_ALIGNMENT)
			m_lpOverlay->SetConfigInt(strTable, "UnitsAlignment", m_nUnitsAlignment	);

		m_lpOverlay->SetConfigInt(strTable, "Lines"				, GetCount()		);

		POSITION pos = GetHeadPosition();

		while (pos)
		{
			CTextTableLine* lpLine = GetNext(pos);

			lpLine->Save();
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CTextTable::ApplyTag(CString& str, CString strBeginTag, CString strEndTag)
{
	if (!strBeginTag.IsEmpty())
		str = strBeginTag + str + strEndTag;
}
/////////////////////////////////////////////////////////////////////////////
CString	CTextTable::GetHypertext(BOOL bOptimize, DWORD dwColor, int nSize)
{
	CString strValueAlignmentTag;
	if (m_nValueAlignment != DEFAULT_ALIGNMENT)
	{
		if (bOptimize)
			strValueAlignmentTag = m_lpOverlay->GetAlignmentsLibrary()->FormatAttrTag(m_nValueAlignment, "A", FALSE);
		else
			strValueAlignmentTag = COverlayAttributesLibrary::FormatExplicitAttrTag(m_nValueAlignment, "A", FALSE);
	}
		//format value alignment tag

	CString strValueSizeTag;
	if (m_nValueSize != nSize)
	{
		if (bOptimize)
			strValueSizeTag = m_lpOverlay->GetSizesLibrary()->FormatAttrTag(m_nValueSize, "S", FALSE);
		else
			strValueSizeTag = COverlayAttributesLibrary::FormatExplicitAttrTag(m_nValueSize, "S", FALSE);
	}
		//format value size tag

	CString strUnitsAlignmentTag;
	if (m_nUnitsAlignment != DEFAULT_ALIGNMENT)
	{
		if (bOptimize)
			strUnitsAlignmentTag = m_lpOverlay->GetAlignmentsLibrary()->FormatAttrTag(m_nUnitsAlignment, "A", FALSE);
		else
			strUnitsAlignmentTag = COverlayAttributesLibrary::FormatExplicitAttrTag(m_nUnitsAlignment, "A", FALSE);
	}
		//format units alignment tag

	CString strUnitsSizeTag;
	if (m_nUnitsSize != m_nValueSize)
	{
		if (bOptimize)
			strUnitsSizeTag = m_lpOverlay->GetSizesLibrary()->FormatAttrTag(m_nUnitsSize, "S", FALSE);
		else
			strUnitsSizeTag = COverlayAttributesLibrary::FormatExplicitAttrTag(m_nUnitsSize, "S", FALSE);
	}
		//format units size tag

	CString strAlignmentEndTag = "<A>";

	if ((m_nValueAlignment != DEFAULT_ALIGNMENT) &&
		(m_nUnitsAlignment != DEFAULT_ALIGNMENT))
	{
		if (bOptimize)
			strAlignmentEndTag = "";
	}
		//format alignment end tag

	//NOTE in case of hypertext optimization we skip alignment end tag if both value and size columns use non-default alignment. In this case the alignment
	//ends on the next alignment tag or on new line

	CString	strResult;

	POSITION posLine = GetHeadPosition();

	while (posLine)
	{
		CString strLine;

		CTextTableLine* lpLine = GetNext(posLine);

		CDynamicColor dynamicLineColor;

		DWORD dwLineColor = lpLine->GetDynamicColor(dynamicLineColor);

		CString strLineColorTag;

		if (dynamicLineColor.IsInternalSource() ||
			dynamicLineColor.IsEmbedded())
		{
			if (bOptimize)
				strLineColorTag = m_lpOverlay->GetColorsLibraryStr()->FormatAttrTag(dynamicLineColor.FormatRanges(), dynamicLineColor.FormatParams(TRUE), "C", m_lpOverlay->GetColorsLibrary()->GetSize());
			else
				strLineColorTag = COverlayAttributesLibraryStr::FormatExplicitAttrTag(dynamicLineColor.FormatRanges(), dynamicLineColor.FormatParams(TRUE), "C");
		}
		else
		{
			if (dwLineColor != DEFAULT_COLOR)
			{
				if (bOptimize)
					strLineColorTag = m_lpOverlay->GetColorsLibrary()->FormatAttrTag(dwLineColor, "C", TRUE);
				else
					strLineColorTag = COverlayAttributesLibrary::FormatExplicitAttrTag(dwLineColor, "C", TRUE);
			}
		}
			//format line color tag

		strLine += COverlayLayer::DecodeSpecialChars(lpLine->m_strName);
		strLine += " \t";

		ApplyTag(strLine, strLineColorTag		, "<C>");

		POSITION posCell = lpLine->GetHeadPosition();

		while (posCell)
		{
			CString strCell;

			CTextTableCell* lpCell = lpLine->GetNext(posCell);

			CDynamicColor dynamicCellColor;

			DWORD dwCellColor = lpCell->GetDynamicColor(dynamicCellColor);

			CString strCellColorTag;

			if (dynamicCellColor.IsInternalSource() ||
				dynamicCellColor.IsEmbedded())
			{
				if (bOptimize)
					strCellColorTag = m_lpOverlay->GetColorsLibraryStr()->FormatAttrTag(dynamicCellColor.FormatRanges(), dynamicCellColor.FormatParams(TRUE), "C", m_lpOverlay->GetColorsLibrary()->GetSize());
				else
					strCellColorTag = COverlayAttributesLibraryStr::FormatExplicitAttrTag(dynamicCellColor.FormatRanges(), dynamicCellColor.FormatParams(TRUE), "C");
			}
			else
			{
				if (dwCellColor != DEFAULT_COLOR)
				{
					if (bOptimize)
						strCellColorTag = m_lpOverlay->GetColorsLibrary()->FormatAttrTag(dwCellColor, "C", TRUE);
					else
						strCellColorTag = COverlayAttributesLibrary::FormatExplicitAttrTag(dwCellColor, "C", TRUE);
				}
			}
				//format cell color tag

			CString strValue;
			CString strUnits;

			if (!lpCell->m_strSource.IsEmpty())
			{
				COverlayDataSource* lpSource = m_lpOverlay->FindDataSource(lpCell->m_strSource);

				if (lpSource)
				{
					FLOAT data = lpSource->GetData();

					if (data != FLT_MAX)
					{
						strValue = lpSource->GetDataStr();
						strUnits = lpSource->GetUnits();
					}
				}
			}
			else
				strValue = COverlayLayer::DecodeSpecialChars(lpCell->m_strText);

			strValue += " ";
			strUnits += " ";

			ApplyTag(strValue, strValueAlignmentTag	, strAlignmentEndTag);

			ApplyTag(strUnits, strUnitsSizeTag		, "<S>");
			ApplyTag(strUnits, strUnitsAlignmentTag	, strAlignmentEndTag);

			strCell = strValue + strUnits;

			ApplyTag(strCell, strCellColorTag		, "<C>");

			strLine += strCell;
		}

		if (!strResult.IsEmpty())
			strResult += "\n";

		strResult += strLine;
	}

	if ((dwColor != DEFAULT_COLOR) || (nSize != DEFAULT_SIZE) || (m_nValueSize != DEFAULT_SIZE))
		ApplyTag(strResult, "<D>", "<D>");

	ApplyTag(strResult, strValueSizeTag, "<S>");

	return strResult;
}
//////////////////////////////////////////////////////////////////////
CTextTableLine* CTextTable::FindLine(DWORD dwLineID)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CTextTableLine* lpLine = GetNext(pos);

		if (lpLine->GetLineID() == dwLineID)
			return lpLine;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CTextTableCell* CTextTable::FindCell(DWORD dwLineID, DWORD dwCellID)
{
	CTextTableLine* lpLine = FindLine(dwLineID);

	if (lpLine)
		return lpLine->FindCell(dwCellID);

	return NULL;
}
//////////////////////////////////////////////////////////////////////
CTextTableCell* CTextTable::GetCell(DWORD dwIndex)
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CTextTableLine* lpLine = GetNext(pos);

		DWORD dwCount = lpLine->GetCount();

		if (dwIndex < dwCount)
			return lpLine->GetAt(lpLine->FindIndex(dwIndex));

		dwIndex = dwIndex - dwCount;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void CTextTable::Reorder(CList<CTextTableLine*, CTextTableLine*>* lpOrder)
{
	//remove line pointers without deleting the objects

	RemoveAll();

	//reoder lines

	POSITION pos = lpOrder->GetHeadPosition();

	while (pos)
	{
		CTextTableLine* lpLine = lpOrder->GetNext(pos);
		
		AddLine(lpLine);
	}
}
//////////////////////////////////////////////////////////////////////
void CTextTable::Reorder(CList<CTextTableCell*, CTextTableCell*>* lpOrder)
{
	//remove cell pointers for all lines without deleting the objects

	POSITION pos = GetHeadPosition();

	while (pos)
		GetNext(pos)->RemoveAll();

	//reoder/remap cells to new lines

	pos = lpOrder->GetHeadPosition();

	while (pos)
	{
		CTextTableCell* lpCell = lpOrder->GetNext(pos);
		CTextTableLine* lpLine = FindLine(lpCell->GetLineID());
		
		lpLine->AddCell(lpCell);
	}

	//remove empty lines left after reordering

	RemoveEmptyLines();
}
//////////////////////////////////////////////////////////////////////
void CTextTable::RemoveEmptyLines()
{
	POSITION pos = GetTailPosition();

	while (pos)
	{
		CTextTableLine* lpLine = GetPrev(pos);

		if (!lpLine->GetCount())
			RemoveLine(lpLine);
	}
}
//////////////////////////////////////////////////////////////////////
void CTextTable::UpdateLineIDs(BOOL bUpdateCellIDs)
{
	DWORD dwLineID = 0;

	POSITION pos = GetHeadPosition();

	while (pos)
	{
		CTextTableLine* lpLine = GetNext(pos);

		lpLine->SetLineID(dwLineID++);

		if (bUpdateCellIDs)
			lpLine->UpdateCellIDs();
	}
}
//////////////////////////////////////////////////////////////////////
