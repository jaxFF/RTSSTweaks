// HypertextParser.h: interface for the CHypertextParser class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
class CHypertextParser
{
public:
	CHypertextParser();
	virtual ~CHypertextParser();

	static int		ToDec(char c);
	static int		ToHex(char c);

	static LPCSTR	ScanStr(LPCSTR lpsz, CString& strResult);
	static LPCSTR	ScanStr(LPCSTR lpsz, LPSTR lpResult, DWORD dwSize);
	static LPCSTR	ScanHex(LPCSTR lpsz, DWORD& dwResult);
	static LPCSTR	ScanDec(LPCSTR lpsz, DWORD& dwResult);
	static LPCSTR	ScanFlt(LPCSTR lpsz, float& fltResult);

	static BOOL		IsEmptyTagParam(LPCSTR lpsz);
	static BOOL		IsQuotingRequired(LPCSTR lpsz);
};
//////////////////////////////////////////////////////////////////////
