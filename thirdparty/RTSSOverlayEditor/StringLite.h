// StringLite.h: interface for the CStringLite class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _STRINGLITE_H_INCLUDED_
#define _STRINGLITE_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
class CStringLite  
{
public:
	CStringLite();
		//empty constuctor
	CStringLite(const CStringLite& str);
		//copy constructor
	CStringLite(LPCSTR str);
		//copy constuctor (from ANSI string)
	CStringLite(LPCSTR str, int nLength);
		//copy constuctor (from ANSI string)

	//LPCSTR type cast operator
	operator LPCSTR() const	{ return m_lpszBuf; }

	//assignment operators
	const CStringLite& operator =  (char ch);
	const CStringLite& operator =  (LPCSTR str);
	const CStringLite& operator =  (const CStringLite& str);

	//unary concatenation operators
	const CStringLite& operator += (char ch);
	const CStringLite& operator += (LPCSTR str);
	const CStringLite& operator += (const CStringLite& str);

	//binary concatenation operators
	friend CStringLite operator + (const CStringLite& str0, const CStringLite& str1);
	friend CStringLite operator + (const CStringLite& str0, LPCSTR str1);
	friend CStringLite operator + (LPCSTR str0, const CStringLite& str1);

	//searching
	int Find(LPCSTR lpszSub) const;
	int Find(LPCSTR lpszSub, int nStart) const;
	int FindNoCase(LPCSTR lpszSub) const;
	int FindNoCase(LPCSTR lpszSub, int nStart) const;

	//comparison with wildcard
	int CompareWithWildcard(LPCSTR str, char wildcard);
		//compares string with wildcard char, for example CompareWithWildcard("s?cker", "?") will return TRUE
		//for both "socker" and "sucker"
	static int CompareWithWildcard(LPCSTR str0, LPCSTR str1, char wildcard);
		//static helper for comparing two null-terminated strings using wildcard char

	//conversion
	void MakeLower();
		//converts string to lowercase
	void MakeUpper();
		//converts string to uppercase

	//trimming
	void TrimLeft();
	void TrimLeft(char chTarget);
	void TrimLeft(LPCSTR lpszTargets);
	void TrimRight();
	void TrimRight(char chTarget);
	void TrimRight(LPCSTR lpszTargets);

	//extraction
	CStringLite Left(int nCount) const;
	CStringLite Right(int nCount) const;
	CStringLite Mid(int nFirst, int nCount) const;

	//replace
	int Replace(LPCTSTR lpszOld, LPCTSTR lpszNew);

	//string length related functions
	int GetLength() const;
	BOOL IsEmpty() const; 

	//printf styled formatting
	void Format(LPCSTR lpszFormat, ...);

	//string copy allocation
	LPSTR AllocateCopy();
		//allocates string copy
		//Warning! Caller is responsible for deallocation
	static LPSTR AllocateCopy(LPCSTR str);
		//static helper for allocatng string copy 
		//Warning! Caller is responsible for deallocation

	virtual ~CStringLite();

protected:

	void helper_InitBuf(char ch);
		//helper for buffer initialization
	void helper_InitBuf(LPCSTR str);
		//helper for buffer initialization
	void helper_InitBuf(LPCSTR str, int nLength);
		//helper for buffer initialization with maximum length limitation
	void helper_AppendBuf(char ch);
		//helper for adding specified char to buffer
	void helper_AppendBuf(LPCSTR str);
		//helper for adding specified string to buffer
	void helper_FreeBuf();
		//helper for buffer deallocation

	LPSTR m_lpszBuf;
		//pointer to string buffer
};

#endif
