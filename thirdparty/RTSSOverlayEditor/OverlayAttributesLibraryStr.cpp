// OverlayAttributesLibraryStrStr.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayAttributesLibraryStr.h"
//////////////////////////////////////////////////////////////////////
COverlayAttributesLibraryStr::COverlayAttributesLibraryStr()
{
}
//////////////////////////////////////////////////////////////////////
COverlayAttributesLibraryStr::~COverlayAttributesLibraryStr()
{
}
//////////////////////////////////////////////////////////////////////
void COverlayAttributesLibraryStr::AddAttr(CString strAttr, DWORD dwCount)
{
	DWORD dwIndex;
	DWORD dwUsageCount;

	if (GetAttr(strAttr, dwIndex, dwUsageCount))
		m_usageCounters.SetAt(dwIndex, dwUsageCount + dwCount);
	else
	{
		m_map.SetAt(strAttr, GetCount());

		Add(strAttr);

		m_usageCounters.Add(dwCount);
	}

}
//////////////////////////////////////////////////////////////////////
BOOL COverlayAttributesLibraryStr::GetAttr(CString strAttr, DWORD& dwIndex, DWORD& dwUsageCount)
{
	if (m_map.Lookup(strAttr, dwIndex))
	{
		dwUsageCount	= m_usageCounters.GetAt(dwIndex);

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void COverlayAttributesLibraryStr::Destroy()
{
	RemoveAll();

	m_usageCounters.RemoveAll();

	m_map.RemoveAll();
}
//////////////////////////////////////////////////////////////////////
CString COverlayAttributesLibraryStr::FormatHeaderTags(CString strTagName, DWORD dwBase)
{
	CString strHeader;

	DWORD dwCount = GetCount();

	for (DWORD dwIndex=0; dwIndex<dwCount; dwIndex++)
	{
		CString strAttr		= GetAt(dwIndex);
		DWORD dwUsageCount	= m_usageCounters.GetAt(dwIndex);

		if (dwUsageCount > 1)
		{
			CString strTag;

			strTag.Format("<%s%d=%s>", strTagName, dwBase + dwIndex, strAttr);

			strHeader += strTag;
		}
	}

	return strHeader;
}
//////////////////////////////////////////////////////////////////////
CString COverlayAttributesLibraryStr::FormatAttrTag(CString strAttr, CString strParam, CString strTagName, DWORD dwBase)
{
	DWORD dwIndex;
	DWORD dwUsageCount;

	if (GetAttr(strAttr, dwIndex, dwUsageCount))
	{
		if (dwUsageCount > 1)
		{
			CString strTag;
			strTag.Format("<%s%d%s>", strTagName, dwBase + dwIndex, strParam);

			return strTag;
		}
	}

	return FormatExplicitAttrTag(strAttr, strParam, strTagName);
}
//////////////////////////////////////////////////////////////////////
CString COverlayAttributesLibraryStr::FormatExplicitAttrTag(CString strAttr, CString strParam, CString strTagName)
{
	CString strTag;
	strTag.Format("<%s=%s%s>", strTagName, strAttr, strParam);

	return strTag;
}
//////////////////////////////////////////////////////////////////////

