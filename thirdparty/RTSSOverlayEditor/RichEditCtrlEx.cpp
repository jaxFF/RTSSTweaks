// RichEditCtrlEx.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "RichEditCtrlEx.h"
#include "Overlay.h"
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx
/////////////////////////////////////////////////////////////////////////////
#define MARKER_BEG_BOLD						0x00000001
#define MARKER_END_BOLD						0x00000002
#define MARKER_BEG_COLOR					0x00000004
#define MARKER_END_COLOR					0x00000008
/////////////////////////////////////////////////////////////////////////////
CRichEditCtrlEx::CRichEditCtrlEx()
{
}
/////////////////////////////////////////////////////////////////////////////
CRichEditCtrlEx::~CRichEditCtrlEx()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRichEditCtrlEx, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEditCtrlEx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx message handlers
/////////////////////////////////////////////////////////////////////////////
void CRichEditCtrlEx::SetWindowTextEx(LPCSTR lpText)
{
	SetRedraw(FALSE);

	CString strText	= "";

	DWORD dwLen			= strlen(lpText);
	DWORD dwIndexDst	= 0;

	if (dwLen)
	{
		LPDWORD lpMarkers = new DWORD[dwLen];
		ZeroMemory(lpMarkers, dwLen*sizeof(DWORD));

		for (DWORD dwIndex=0; dwIndex<dwLen; dwIndex++)
		{
			if (!_strnicmp((LPCSTR)lpText + dwIndex, "[b]", min(3, dwLen-dwIndex)))
			{
				dwIndex += 2;
				lpMarkers[dwIndexDst] |= MARKER_BEG_BOLD;
				continue;
			}

			if (!_strnicmp((LPCSTR)lpText + dwIndex, "[/b]", min(4, dwLen-dwIndex)))
			{
				dwIndex += 3;
				if (lpMarkers[dwIndexDst] & MARKER_BEG_BOLD)
					lpMarkers[dwIndexDst] &= ~MARKER_BEG_BOLD;
				else
					lpMarkers[dwIndexDst] |= MARKER_END_BOLD;
				continue;
			}

			DWORD dwColor = 0;

			if (!_strnicmp((LPCSTR)lpText + dwIndex, "[c", min(2, dwLen-dwIndex)))
			{
				dwIndex++;
				while (ToHex(lpText[dwIndex])>=0)
				{
					dwColor = (dwColor<<4) + ToHex(lpText[dwIndex]);
					dwIndex++;
				}
			
				lpMarkers[dwIndexDst] |= MARKER_BEG_COLOR | (dwColor<<8);
				continue;
			}

			if (!_strnicmp((LPCSTR)lpText + dwIndex, "[/c]", min(4, dwLen-dwIndex)))
			{
				dwIndex += 3;
				if (lpMarkers[dwIndexDst] & MARKER_BEG_COLOR)
				{
					lpMarkers[dwIndexDst] &= ~MARKER_BEG_COLOR;
					lpMarkers[dwIndexDst] &= ~0xFFFFFF00;
				}
				else
					lpMarkers[dwIndexDst] |= MARKER_END_COLOR;
				continue;
			}

			strText += lpText[dwIndex];
			dwIndexDst++;
		}

		SetWindowText(strText);

		DWORD dwBoldFrom		= 0;
		DWORD dwBoldRefCount	= 0;

		DWORD dwColor			= 0;
		DWORD dwColorFrom		= 0;
		DWORD dwColorRefCount	= 0;

		for (DWORD dwIndex=0; dwIndex<=strlen(strText); dwIndex++)
		{
			if (lpMarkers[dwIndex] & MARKER_END_BOLD)
			{
				if (dwBoldRefCount)
				{
					dwBoldRefCount--;

					if (!dwBoldRefCount)
					{
						CHARFORMAT cf;
						cf.cbSize		= sizeof(cf);
						cf.dwMask		= CFM_BOLD;
						cf.dwEffects	= CFE_BOLD;
						SetSel(dwBoldFrom, dwIndex);
						SetWordCharFormat(cf);
					}
				}
			}

			if (lpMarkers[dwIndex] & MARKER_END_COLOR)
			{
				if (dwColorRefCount)
				{
					dwColorRefCount--;

					if (!dwColorRefCount)
					{
						CHARFORMAT cf;
						cf.cbSize		= sizeof(cf);
						cf.dwMask		= CFM_COLOR;
						cf.dwEffects	= 0;
						cf.crTextColor	= RGB(0xff & (dwColor>>16), 0xff & (dwColor>>8), 0xff & dwColor);

						SetSel(dwColorFrom, dwIndex);
						SetWordCharFormat(cf);
					}
				}
			}

			if (lpMarkers[dwIndex] & MARKER_BEG_BOLD)
			{
				if (!dwBoldRefCount)
					dwBoldFrom = dwIndex;
				dwBoldRefCount++;
			}

			if (lpMarkers[dwIndex] & MARKER_BEG_COLOR)
			{
				if (!dwColorRefCount)
				{
					dwColorFrom = dwIndex;
					dwColor		= lpMarkers[dwIndex]>>8;
				}
				dwColorRefCount++;
			}
		}

		delete [] lpMarkers;
	}

	SetRedraw(TRUE);

	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
int CRichEditCtrlEx::ToHex(char c)
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
