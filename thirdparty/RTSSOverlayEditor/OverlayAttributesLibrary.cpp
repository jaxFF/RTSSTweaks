// OverlayAttributesLibrary.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayAttributesLibrary.h"
//////////////////////////////////////////////////////////////////////
COverlayAttributesLibrary::COverlayAttributesLibrary()
{
}
//////////////////////////////////////////////////////////////////////
COverlayAttributesLibrary::~COverlayAttributesLibrary()
{
}
//////////////////////////////////////////////////////////////////////
void COverlayAttributesLibrary::AddAttr(DWORD dwAttr, DWORD dwCount)
{
	DWORD dwIndex;
	DWORD dwUsageCount;

	if (GetAttr(dwAttr, dwIndex, dwUsageCount))
		m_usageCounters.SetAt(dwIndex, dwUsageCount + dwCount);
	else
	{
		m_map.SetAt(dwAttr, GetCount());

		Add(dwAttr);

		m_usageCounters.Add(dwCount);
	}

}
//////////////////////////////////////////////////////////////////////
BOOL COverlayAttributesLibrary::GetAttr(DWORD dwAttr, DWORD& dwIndex, DWORD& dwUsageCount)
{
	if (m_map.Lookup(dwAttr, dwIndex))
	{
		dwUsageCount = m_usageCounters.GetAt(dwIndex);

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void COverlayAttributesLibrary::Destroy()
{
	RemoveAll();

	m_usageCounters.RemoveAll();

	m_map.RemoveAll();
}
//////////////////////////////////////////////////////////////////////
CString COverlayAttributesLibrary::FormatHeaderTags(CString strTagName, BOOL bHex)
{
	CString strHeader;

	DWORD dwCount = GetCount();

	for (DWORD dwIndex=0; dwIndex<dwCount; dwIndex++)
	{
		DWORD dwAttr		= GetAt(dwIndex);
		DWORD dwUsageCount	= m_usageCounters.GetAt(dwIndex);

		if (dwUsageCount > 1)
		{
			CString strTag;

			if (bHex)
				strTag.Format("<%s%d=%X>", strTagName, dwIndex, dwAttr);
			else
				strTag.Format("<%s%d=%d>", strTagName, dwIndex, dwAttr);

			strHeader += strTag;
		}
	}

	return strHeader;
}
//////////////////////////////////////////////////////////////////////
CString COverlayAttributesLibrary::FormatAttrTag(DWORD dwAttr, CString strTagName, BOOL bHex)
{
	DWORD dwIndex;
	DWORD dwUsageCount;

	if (GetAttr(dwAttr, dwIndex, dwUsageCount))
	{
		if (dwUsageCount > 1)
		{
			CString strTag;
			strTag.Format("<%s%d>", strTagName, dwIndex);

			return strTag;
		}
	}

	return FormatExplicitAttrTag(dwAttr, strTagName, bHex);
}
//////////////////////////////////////////////////////////////////////
CString COverlayAttributesLibrary::FormatExplicitAttrTag(DWORD dwAttr, CString strTagName, BOOL bHex)
{
	CString strTag;

	if (bHex)
		strTag.Format("<%s=%X>", strTagName, dwAttr);
	else
		strTag.Format("<%s=%d>", strTagName, dwAttr);

	return strTag;
}
//////////////////////////////////////////////////////////////////////
CString COverlayAttributesLibrary::FormatExplicitAttrTag(CString strAttr, CString strTagName)
{
	CString strTag;
	strTag.Format("<%s=%s>", strTagName, strAttr);

	return strTag;
}
//////////////////////////////////////////////////////////////////////

