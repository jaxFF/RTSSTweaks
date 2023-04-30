// StringLite.cpp: implementation of the CStringLite class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "StringLite.h"
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStringLite::CStringLite()
{
	m_lpszBuf = NULL;
	helper_InitBuf("");
}
//////////////////////////////////////////////////////////////////////
CStringLite::CStringLite(LPCSTR str)
{
	m_lpszBuf = NULL;
	helper_InitBuf(str);
}
//////////////////////////////////////////////////////////////////////
CStringLite::CStringLite(LPCSTR str, int nLength)
{
	m_lpszBuf = NULL;
	helper_InitBuf(str, nLength);
}
//////////////////////////////////////////////////////////////////////
CStringLite::CStringLite(const CStringLite &str)
{
	m_lpszBuf = NULL;
	helper_InitBuf(str);
}
//////////////////////////////////////////////////////////////////////
CStringLite::~CStringLite()
{
	helper_FreeBuf();
}
//////////////////////////////////////////////////////////////////////
void CStringLite::helper_InitBuf(char ch)
{
	helper_FreeBuf();

	LPSTR lpBuf = new char[2];
	lpBuf[0]	= ch;
	lpBuf[1]	= 0;

	helper_FreeBuf();
	m_lpszBuf = lpBuf;
}
//////////////////////////////////////////////////////////////////////
void CStringLite::helper_InitBuf(LPCSTR str)
{
	if (str)
	{
		LPSTR lpBuf = new char[strlen(str) + 1];
		strcpy_s(lpBuf, strlen(str) + 1, str);

		helper_FreeBuf();
		m_lpszBuf = lpBuf;
	}
	else
		helper_InitBuf("");
}
//////////////////////////////////////////////////////////////////////
void CStringLite::helper_InitBuf(LPCSTR str, int nLength)
{
	if (str)
	{	
		int nSize			= min(nLength, (int)strlen(str));
		LPSTR lpBuf			= new char[nSize + 1];
		strncpy_s(lpBuf, nSize + 1, str, nLength);
		lpBuf[nSize]		= 0;

		helper_FreeBuf();
		m_lpszBuf = lpBuf;
	}
	else
		helper_InitBuf("");
}
//////////////////////////////////////////////////////////////////////
void CStringLite::helper_FreeBuf()
{
	if (m_lpszBuf)
		delete [] m_lpszBuf;

	m_lpszBuf = NULL;
}
//////////////////////////////////////////////////////////////////////
void CStringLite::helper_AppendBuf(char ch)
{
	int len	= 0;
	
	if (m_lpszBuf)
		len = strlen(m_lpszBuf);

	LPSTR lpBuf = new char[len + 2];

	if (m_lpszBuf)
		strcpy_s(lpBuf, len + 2, m_lpszBuf);

	lpBuf[len		] = ch;
	lpBuf[len + 1	] = 0;

	helper_FreeBuf();

	m_lpszBuf = lpBuf;
}
//////////////////////////////////////////////////////////////////////
void CStringLite::helper_AppendBuf(LPCSTR str)
{
	if (str)
	{
		int len	= 0;
		
		if (m_lpszBuf)
			len = strlen(m_lpszBuf);

		LPSTR lpBuf = new char[len + strlen(str) + 1];

		if (m_lpszBuf)
			strcpy_s(lpBuf, len + strlen(str) + 1, m_lpszBuf);
		strcat_s(lpBuf, len + strlen(str) + 1, str);

		helper_FreeBuf();

		m_lpszBuf = lpBuf;
	}
}
//////////////////////////////////////////////////////////////////////
const CStringLite& CStringLite::operator += (char ch)
{
	helper_AppendBuf(ch);
	return *this;
}
//////////////////////////////////////////////////////////////////////
const CStringLite& CStringLite::operator += (LPCSTR str)
{
	helper_AppendBuf(str);
	return *this;
}
//////////////////////////////////////////////////////////////////////
const CStringLite& CStringLite::operator += (const CStringLite& str)
{
	helper_AppendBuf(str);
	return *this;
}
//////////////////////////////////////////////////////////////////////
const CStringLite& CStringLite::operator = (char ch)
{
	helper_InitBuf(ch);
	return *this;
}
//////////////////////////////////////////////////////////////////////
const CStringLite& CStringLite::operator = (LPCSTR str)
{
	helper_InitBuf(str);
	return *this;
}
//////////////////////////////////////////////////////////////////////
const CStringLite& CStringLite::operator = (const CStringLite& str)
{
	helper_InitBuf(str);
	return *this;
}
//////////////////////////////////////////////////////////////////////
int CStringLite::GetLength() const
{
	if (m_lpszBuf)
		return strlen(m_lpszBuf);

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CStringLite::IsEmpty() const
{
	return GetLength() == 0;
}
//////////////////////////////////////////////////////////////////////
void CStringLite::Format(LPCSTR lpszFormat, ...)
{
	va_list argList;
	va_start(argList, lpszFormat);

	int sizeBufMax	= 0x10000;
	int sizeBuf;
	
	do
	{
		LPSTR lpBuf = new char[sizeBufMax];
		sizeBuf		= _vsnprintf_s(lpBuf, sizeBufMax, sizeBufMax-1, lpszFormat, argList);

		if (sizeBuf < 0)
			sizeBufMax = sizeBufMax << 1;
		else
		{
			helper_FreeBuf();

			m_lpszBuf	= new char[sizeBuf + 1];
			strcpy_s(m_lpszBuf, sizeBuf + 1, lpBuf);
		}

		delete [] lpBuf;
	}
	while (sizeBuf < 0);

	va_end(argList);
}
//////////////////////////////////////////////////////////////////////
int CStringLite::Find(LPCSTR lpszSub) const
{
	return Find(lpszSub, 0);
}
//////////////////////////////////////////////////////////////////////
int CStringLite::Find(LPCSTR lpszSub, int nStart) const
{
	if (nStart < 0)
		return -1;

	int nSizeSrc = strlen(m_lpszBuf);
	int nSizeSub = strlen(lpszSub);

	if (nStart + nSizeSub > nSizeSrc)
		return -1;

	LPSTR lpsz = strstr(m_lpszBuf + nStart, lpszSub);

	return (lpsz == NULL) ? -1 : lpsz - m_lpszBuf;

}
//////////////////////////////////////////////////////////////////////
int CStringLite::FindNoCase(LPCSTR lpszSub) const
{
	return FindNoCase(lpszSub, 0);
}
//////////////////////////////////////////////////////////////////////
int CStringLite::FindNoCase(LPCSTR lpszSub, int nStart) const
{
	if (nStart < 0)
		return -1;

	int nSizeSrc = strlen(m_lpszBuf);
	int nSizeSub = strlen(lpszSub);

	if (nStart + nSizeSub > nSizeSrc)
		return -1;

	for (int nChar = nStart; nChar < nSizeSrc - nSizeSub; nChar++)
		if (!_strnicmp(m_lpszBuf + nChar, lpszSub, nSizeSub))
			return nChar;

	return -1;
}
//////////////////////////////////////////////////////////////////////
CStringLite CStringLite::Left(int nCount) const
{
	if (nCount < 0)
		nCount = 0;
	if (nCount >= (int)strlen(m_lpszBuf))
		return *this;

	return CStringLite(m_lpszBuf, nCount);
}
//////////////////////////////////////////////////////////////////////
CStringLite CStringLite::Right(int nCount) const
{
	int nLen = strlen(m_lpszBuf);
	
	if (nCount < 0)
		nCount = 0;
	if (nCount >= nLen)
		return *this;

	return CStringLite(m_lpszBuf + nLen - nCount, nCount);
}
//////////////////////////////////////////////////////////////////////
CStringLite CStringLite::Mid(int nFirst, int nCount) const
{
	int nLen = strlen(m_lpszBuf);
	
	if (nCount < 0)
		nCount = 0;
	if (nFirst + nCount > nLen)
		nCount = nLen - nFirst;

	if (nCount >= nLen)
		return *this;

	return CStringLite(m_lpszBuf + nFirst, nCount);
}
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4706)

int CStringLite::Replace(LPCTSTR lpszOld, LPCTSTR lpszNew)
{
	if (!lpszOld)
		return 0;
			//return if NULL replcement target string is specified
	int nSrcLen = strlen(lpszOld);
		//get replacement target length

	if (!nSrcLen)
		return 0;
			//return if empty replacement target string is specified

	int nRepLen = lpszNew ? strlen(lpszNew) : 0;
		//get replacement length (NULL ptr is treated as emptry string)

	int nCount	= 0;
		//init replacements counter

	int nStrLen	= strlen(m_lpszBuf);
		//get source string length

	LPCSTR lpszBeg	= m_lpszBuf;
	LPCSTR lpszEnd	= lpszBeg + nStrLen;
	LPCSTR lpszTgt;

	while (lpszBeg < lpszEnd)
		//determine amount of replacements
	{
		while (lpszTgt = strstr(lpszBeg, lpszOld))
		{
			nCount++;
			lpszBeg = lpszTgt + nSrcLen;
		}

		lpszBeg += strlen(lpszBeg) + 1;
	}

	if (nCount)
		//perform actual replacement if necessary
	{
		int nNewLen = nStrLen + nCount * (nRepLen - nSrcLen) + 1;
			//calc size of new string 

		LPSTR lpNewBuf = new char[nNewLen];
			//allocate memory for new string
		LPSTR lpCurPtr = lpNewBuf;

		lpszBeg	= m_lpszBuf;
		lpszEnd	= lpszBeg + nStrLen;

		while (lpszBeg < lpszEnd)
		{
			int nChunk;

			while (lpszTgt = strstr(lpszBeg, lpszOld))
			{
				nChunk = lpszTgt - lpszBeg;

				if (nChunk)
				{
					memcpy(lpCurPtr, lpszBeg, nChunk);
					lpCurPtr += nChunk;
				}

				if (lpszNew)
				{
					memcpy(lpCurPtr, lpszNew, nRepLen);
					lpCurPtr += nRepLen;
				}

				lpszBeg = lpszTgt + nSrcLen;
			}

			nChunk = strlen(lpszBeg);

			if (nChunk)
			{
				memcpy(lpCurPtr, lpszBeg, nChunk);
				lpCurPtr += nChunk;
			}

			lpszBeg += strlen(lpszBeg) + 1;
		}

		*lpCurPtr = 0;

		helper_FreeBuf();

		m_lpszBuf = lpNewBuf;
	}

	return nCount;
}

#pragma warning (default : 4706)
//////////////////////////////////////////////////////////////////////
void CStringLite::TrimLeft()
{
	TrimLeft(" \t\n\r");
}
//////////////////////////////////////////////////////////////////////
void CStringLite::TrimLeft(char chTarget)
{
	int nLen	= strlen(m_lpszBuf);
	int i		= 0;

	while ((i<nLen) && (m_lpszBuf[i] == chTarget))
		i++;

	helper_InitBuf(m_lpszBuf + i); 
}
//////////////////////////////////////////////////////////////////////
void CStringLite::TrimLeft(LPCSTR lpszTargets)
{
	int nLen	= strlen(m_lpszBuf);
	int i		= 0;

	while ((i<nLen) && (strchr(lpszTargets, m_lpszBuf[i])))
		i++;

	helper_InitBuf(m_lpszBuf + i); 
}
//////////////////////////////////////////////////////////////////////
void CStringLite::TrimRight()
{
	TrimRight(" \t\n\r");
}
//////////////////////////////////////////////////////////////////////
void CStringLite::TrimRight(char chTarget)
{
	int nLen	= strlen(m_lpszBuf);
	int i		= nLen;

	while (i && (m_lpszBuf[i-1] == chTarget))
		i--;

	helper_InitBuf(m_lpszBuf, i); 
}
//////////////////////////////////////////////////////////////////////
void CStringLite::TrimRight(LPCSTR lpszTargets)
{
	int nLen	= strlen(m_lpszBuf);
	int i		= nLen;

	while (i && (strchr(lpszTargets, m_lpszBuf[i-1])))
		i--;

	helper_InitBuf(m_lpszBuf, i); 
}
//////////////////////////////////////////////////////////////////////
// friend operators
//
//////////////////////////////////////////////////////////////////////
CStringLite operator + (const CStringLite& str0, const CStringLite& str1)
{
	CStringLite result(str0);
	result.helper_AppendBuf(str1);

	return result;
}
//////////////////////////////////////////////////////////////////////
CStringLite operator + (const CStringLite& str0, LPCSTR str1)
{
	CStringLite result(str0);
	result.helper_AppendBuf(str1);

	return result;
}
//////////////////////////////////////////////////////////////////////
CStringLite operator + (LPCSTR str0, const CStringLite& str1)
{
	CStringLite result(str0);
	result.helper_AppendBuf(str1);

	return result;
}
//////////////////////////////////////////////////////////////////////
void CStringLite::MakeUpper()
{
	_strupr_s(m_lpszBuf, strlen(m_lpszBuf) + 1);
}
//////////////////////////////////////////////////////////////////////
void CStringLite::MakeLower()
{
	_strlwr_s(m_lpszBuf, strlen(m_lpszBuf) + 1);
}
//////////////////////////////////////////////////////////////////////
LPSTR CStringLite::AllocateCopy()
{
	return AllocateCopy(m_lpszBuf);
}
//////////////////////////////////////////////////////////////////////
int CStringLite::CompareWithWildcard(LPCSTR str0, LPCSTR str1, char wildcard)
{
	int nLen0	= strlen(str0);
	int nLen1	= strlen(str1);

	if (nLen0 > nLen1)
		return 1;

	if (nLen0 < nLen1)
		return -1;

	for (int i=0; i<nLen0; i++)
	{
		char c0 = str0[i];
		char c1 = str1[i];

		if ((c0 != wildcard) &&
			(c1 != wildcard))
			if (c0 != c1)
				return (c0 > c1) ? 1 : -1;
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////
int CStringLite::CompareWithWildcard(LPCSTR str, char wildcard)
{
	return CompareWithWildcard(m_lpszBuf, str, wildcard);
}
//////////////////////////////////////////////////////////////////////
LPSTR CStringLite::AllocateCopy(LPCSTR str)
{
	LPSTR lpCopy = new char[strlen(str) + 1];
	strcpy_s(lpCopy, strlen(str) + 1, str);

	return lpCopy;
}
//////////////////////////////////////////////////////////////////////
