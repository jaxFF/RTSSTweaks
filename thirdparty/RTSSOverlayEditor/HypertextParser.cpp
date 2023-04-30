// HypertextParser.cpp: implementation of the CHypertextParser class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HypertextParser.h"
//////////////////////////////////////////////////////////////////////
CHypertextParser::CHypertextParser()
{
} 
//////////////////////////////////////////////////////////////////////
CHypertextParser::~CHypertextParser()
{
}
//////////////////////////////////////////////////////////////////////
int CHypertextParser::ToDec(char c)
{
	if ((c >= '0') &&
		(c <= '9'))
		return c - '0';

	return -1;
}
//////////////////////////////////////////////////////////////////////
int CHypertextParser::ToHex(char c)
{
	if ((c >= '0') &&
		(c <= '9'))
		return c - '0';

	if ((c >= 'a') &&
		(c <= 'f'))
		return c - 'a' + 10;

	if ((c >= 'A') &&
		(c <= 'F'))
		return c - 'A' + 10;

	return -1;
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CHypertextParser::ScanDec(LPCSTR lpsz, DWORD& dwResult)
{
	int s = 1;

	if (*lpsz == '-')
	{
		s = -1;
		lpsz++;
	}

	char c	= *lpsz;
	int  d	= ToDec(c);	

	if (d != -1)
	{
		dwResult = 0;

		for (int i=0; i<10; i++)
		{
			dwResult = (dwResult * 10) + d;

			lpsz++;

			c = *lpsz;
			d = ToDec(c);	

			if (d == -1)
				break;
		}

		if (s < 0)
			dwResult = -(LONG)dwResult;

		return lpsz;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CHypertextParser::ScanHex(LPCSTR lpsz, DWORD& dwResult)
{	
	char c	= *lpsz;
	int  h	= ToHex(c);	

	if (h != -1)
	{
		dwResult = 0;

		for (int i=0; i<8; i++)
		{
			dwResult = (dwResult<<4) | h;

			lpsz++;

			c = *lpsz;
			h = ToHex(c);	

			if (h == -1)
				break;
		}

		return lpsz;
	}

	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CHypertextParser::ScanFlt(LPCSTR lpsz, FLOAT& fltResult)
{
	int s = 1;
	int m = 0;
	
	if (*lpsz == '-')
	{
		s = -1;
		lpsz++;
	}

	char c	= *lpsz;
	int  d	= ToDec(c);	

	if (d != -1)
	{
		fltResult  = 0;
		m		= 1;

		for (int i=0; i<10; i++)
		{
			fltResult = (fltResult * 10) + d;

			lpsz++;

			c = *lpsz;
			d = ToDec(c);	

			if (d == -1)
				break;
		}
	}

	if (*lpsz == '.')
	{
		lpsz++;

		c = *lpsz;
		d = ToDec(c);	

		if (d != -1)
		{
			if (!m)
			{
				fltResult	= 0;
				m			= 1;
			}

			for (int i=0; i<10; i++)
			{
				fltResult	= (fltResult * 10) + d;
				m			= m * 10;

				lpsz++;

				c = *lpsz;
				d = ToDec(c);	

				if (d == -1)
					break;
			}
		}
		else
			return NULL;
	}

	if (!m)
		return NULL;

	fltResult = fltResult / m;

	if (s < 0)
		fltResult = -fltResult;

	return lpsz;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CHypertextParser::ScanStr(LPCSTR lpsz, CString& strResult)
{
	strResult = "";

	while (*lpsz)
	{
		char c  = *lpsz;

		if ((c == ',') ||
			(c == '<') ||
			(c == '>'))
			break;

		strResult += c;

		lpsz++;
	}

	return lpsz;
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CHypertextParser::ScanStr(LPCSTR lpsz, LPSTR lpResult, DWORD dwSize)
{
	while (*lpsz && (dwSize > 1))
	{
		char c  = *lpsz;

		if ((c == ',') ||
			(c == '<') ||
			(c == '>'))
			break;

		*lpResult = c;

		lpsz++;
		lpResult++;
		dwSize--;
	}

	*lpResult = 0;

	return lpsz;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHypertextParser::IsEmptyTagParam(LPCSTR lpsz)
{
	char c  = *lpsz;

	if ((c == ',') ||
		(c == '<') ||
		(c == '>') ||
		(c == '(') ||
		(c == ')') ||
		(c == 0))
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CHypertextParser::IsQuotingRequired(LPCSTR lpsz)
{
	while (*lpsz)
	{
		char c  = *lpsz;

		if ((c == ',') ||
			(c == '<') ||
			(c == '>') || 
			(c == '(') ||
			(c == ')'))
			return TRUE;

		lpsz++;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
