// OverlayAttributesLibraryStr.h: interface for the COverlayAttributesLibraryStr class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "afxtempl.h"
//////////////////////////////////////////////////////////////////////
class COverlayAttributesLibraryStr : public CArray<CString, LPCSTR>
{
public:
	COverlayAttributesLibraryStr();
	virtual ~COverlayAttributesLibraryStr();

	//NOTE: Attributes library is used to make hypertext more compact

	//Some attributes (e.g. layers colors) can be reused multiple times inside the hypertext,
	//so instead of using <C=FF00FF>....<C=FF00FF>....<C=FF00FF> it is much more compact to
	//define a color variable and simply reuse it later, for example:
	//<C5=FF00FF>....<C5>....<C5>
	
	void AddAttr(CString strAttr, DWORD dwCount = 1);
		//adds new attribute to the library or increases attribute usage count for existing attribute
	BOOL GetAttr(CString strAttr, DWORD& dwIndex, DWORD& dwUsageCount);
		//returns index and usage count for existing attribute
	void Destroy();
		//destroys the library

	CString FormatHeaderTags(CString strTagName, DWORD dwBase);
		//formats header variable definition tags for all attributes with usage count greater than 1

	CString FormatAttrTag(CString strAttr, CString strParam, CString strTagName, DWORD dwBase);
		//formats compact attribute tag for attributes with usage count grater than 1 
		//or explicit attibute tag for attributes used just once
	static CString FormatExplicitAttrTag(CString strAttr, CString strParam, CString strTagName);
		//formats explicit attribute tag

protected:
	CArray<DWORD, DWORD>					m_usageCounters;
	CMap<CString, LPCSTR, DWORD, DWORD>		m_map;
};
//////////////////////////////////////////////////////////////////////
