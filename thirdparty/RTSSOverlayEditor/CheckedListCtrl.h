// CheckedListCtrl.h : header file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#ifndef _CHECKEDLISTCTRL_H_INCLUDED_
#define _CHECKEDLISTCTRL_H_INCLUDED_
/////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CCheckedListCtrl window
/////////////////////////////////////////////////////////////////////////////
#define UM_DROP_ITEM			WM_APP + 100
#define UM_DBLCLICK_ITEM		WM_APP + 101
#define UM_SEL_CHANGED			WM_APP + 103
#define UM_CHECK_CHANGED		WM_APP + 104
 /////////////////////////////////////////////////////////////////////////////
#include <afxtempl.h>

#include "IconImageList.h"
/////////////////////////////////////////////////////////////////////////////  
class CCheckedListCtrl : public CListCtrl
{
// Construction  
public: 
	int  SaveScrollPos();
	void RestoreScrollPos(int nScrollPos, BOOL bEnsureVisible);

	void Init();
	
	void EnableSecondColumn(BOOL bEnable);
	void EnableItemSelection(BOOL bEnable);
	void EnableMultiSelection(BOOL bEnable);
	void DrawIcons(BOOL bDrawIcons);

	void OptimalResizeColumn();

	int	GetFocusedItem();

	void SetResourceHandle(HINSTANCE hModule);
	void AddIcon(int nIconID);

	void RedrawVisibleItems();

	BOOL IsHeaderDragging();

	void SetDragImageColumn(int nColumn);

	CCheckedListCtrl();
	virtual ~CCheckedListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckedListCtrl)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	void PrepareMemDC(CDC* pDC);
	void DestroyMemDC();

	void DrawItem(int iItem, DWORD itemState,  CDC *pDC, CRect itemRect);

	int HitTestOnItem(CPoint point);
	int HitTestOnCheck(CPoint point);
	int HitTestOnItemEx(CPoint point);

	void InvalidateBgndRgn();
	void GetBgndRgn(CRgn &bgndRgn);

	CRect GetItemsRect();

	DWORD Blend(DWORD dwColor0, DWORD dwColor1, float fltRatio);
	LPCTSTR MakeShortString(CDC *pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);

	void EndDragDrop();

	CImageList* CreateDragImage(int iItem);

	BOOL m_bEnableSecondColumn;
	BOOL m_bEnableItemSelection;
	BOOL m_bEnableMultiSelection;

	CList<int, int>	m_iconsList;
	CIconImageList m_imageList;

	int m_nSelectedItem;
	int	m_nDragItem;	
	int	m_nDropItem;	
	int m_nDragImageColumn;

	BOOL m_bIsDragging;
	BOOL m_bDrawIcons;

	BOOL m_bHeaderDragging;

	CBitmap	m_bitmap;
	CDC		m_memDC;
	
	//{{AFX_MSG(CCheckedListCtrl)
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
/////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////
