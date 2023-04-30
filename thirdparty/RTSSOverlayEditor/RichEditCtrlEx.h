// RichEditCtrlEx.h: interface for the CRichEditCtrlEx class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _RICHEDITCTRLEX_H_INCLUDED_
#define _RICHEDITCTRLEX_H_INCLUDED_
/////////////////////////////////////////////////////////////////////////////
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx window
/////////////////////////////////////////////////////////////////////////////
class CRichEditCtrlEx : public CRichEditCtrl
{
// Construction
public:
	CRichEditCtrlEx();
	virtual ~CRichEditCtrlEx();
	
	void SetWindowTextEx(LPCSTR lpText);
		//sets control text and allows applying bold text style or custom text color with
		//[B]..[/B] and [C<RGB>]...[/C] tag pairs

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditCtrlEx)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CRichEditCtrlEx)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int	ToHex(char c);
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
/////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////
