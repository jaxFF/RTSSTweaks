// CheckedListCtrl.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "CheckedListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CCheckedListCtrl
/////////////////////////////////////////////////////////////////////////////
CCheckedListCtrl::CCheckedListCtrl()
{
	m_bEnableSecondColumn	= FALSE;
	m_bEnableItemSelection	= FALSE;
	m_bEnableMultiSelection	= FALSE;
	m_bDrawIcons			= FALSE;
	
	m_nSelectedItem			= -1;

	m_nDragItem				= -1;
	m_nDropItem				= -1;
	m_bIsDragging			= FALSE;
	m_nDragImageColumn		= 0;

	m_bHeaderDragging		= FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CCheckedListCtrl::~CCheckedListCtrl()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCheckedListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CCheckedListCtrl)
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CCheckedListCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::GetBgndRgn(CRgn &bgndRgn)
{
	CRect clientRect; GetClientRect(&clientRect);
		//get client rectangle
	bgndRgn.CreateRectRgn(clientRect.left, clientRect.top, clientRect.right, clientRect.bottom);
		//create client region

	CRect viewRect = GetItemsRect();
		//get view rectangle

	if (!viewRect.IsRectEmpty())
	{
		CRgn viewRgn; viewRgn.CreateRectRgn(viewRect.left, viewRect.top, viewRect.right, viewRect.bottom);
			//create view region
		bgndRgn.CombineRgn(&bgndRgn, &viewRgn, RGN_DIFF);
			//create bgnd region
	}

	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
		//try to get header control
	
	if (pHeaderCtrl)
		//we must exclude header rect from bgnd region
	{
		CRect headerRect; pHeaderCtrl->GetWindowRect(&headerRect); ScreenToClient(&headerRect);
			//get header rect
		CRgn headerRgn;
		headerRgn.CreateRectRgn(headerRect.left, headerRect.top, headerRect.right, headerRect.bottom);
			//create client region form header rect

		bgndRgn.CombineRgn(&bgndRgn, &headerRgn, RGN_DIFF);
			//exclude header rect form bgnd region
	}
}
/////////////////////////////////////////////////////////////////////////////
CRect CCheckedListCtrl::GetItemsRect()
{
	int iItems = GetItemCount();
		//get items count

	if (iItems)
	{
		CRect firstItemRect; GetItemRect(0, firstItemRect, LVIR_BOUNDS);
			//get first item rect
		CRect viewRect; GetItemRect(iItems - 1, viewRect, LVIR_BOUNDS);
			//get last item rect
		viewRect.top  = firstItemRect.top;
		viewRect.left = firstItemRect.left;
			//get view rect

		return viewRect;
	}
	else
		return CRect(0,0,0,0);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (lpDrawItemStruct->CtlType != ODT_LISTVIEW)
        return;
	
	if(lpDrawItemStruct->itemAction == ODA_DRAWENTIRE)
	{
		int iItem = lpDrawItemStruct->itemID;
			//get item ID
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
			//get DC
		CRect itemRect = lpDrawItemStruct->rcItem;
			//get clipped item rect
		DrawItem(iItem, lpDrawItemStruct->itemState, pDC, itemRect);
	} 
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::DrawItem(int iItem, DWORD itemState, CDC *pDC, CRect itemRect)
{ 
	//init local variables

	CRect boundsRect;  
	GetItemRect(iItem, boundsRect, LVIR_BOUNDS);
		//get bounds rectangle
	CRect labelRect;
	GetItemRect(iItem, labelRect, LVIR_LABEL);
		//get labal rectangle
	CRect iconRect;
	GetItemRect(iItem, iconRect, LVIR_ICON);
		//get icon rectangle
	CRect clientRect; GetClientRect(clientRect);
		//get client rectangle
	int cw0 = GetColumnWidth(0);
		//get columns width

	CRgn clipRgn; 
	clipRgn.CreateRectRgn(boundsRect.left, boundsRect.top, boundsRect.left + cw0 - 1, boundsRect.bottom);
		//create clipper region for child items

	PrepareMemDC(pDC);
		//prepare memory DC

	CPen* pOldPen = m_memDC.GetCurrentPen();
		//get old pen
		
	COLORREF cellBorderColor = GetSysColor (COLOR_WINDOW);
		//get cell border color
	COLORREF cellColor0 = RGB(GetRValue(cellBorderColor)*0.90f,
							  GetGValue(cellBorderColor)*0.90f,
							  GetBValue(cellBorderColor)*0.90f);
		//get cell0 color
	COLORREF cellColor1 = RGB(GetRValue(cellBorderColor)*0.80f,
							  GetGValue(cellBorderColor)*0.80f,
							  GetBValue(cellBorderColor)*0.80f);
		//get cell1 color

	CRect cellRect	= boundsRect;
	cellRect.right  -= 1; 
	cellRect.bottom -= 1;
		//get cell rect
	
	CString s = GetItemText(iItem,0);

	CBrush cellBrush0(cellColor0);
	CBrush cellBrush1(cellColor1);

	if (s.IsEmpty())
		m_memDC.FillRect(cellRect, &cellBrush1);
			//draw cell
	else
		m_memDC.FillRect(cellRect, &cellBrush0);
			//draw cell

    CPen cellBorderPen(PS_SOLID, 1, cellBorderColor);
		//create cell border pen
	m_memDC.SelectObject(&cellBorderPen);
		//select cell border pen into DC

	m_memDC.MoveTo(cellRect.left , cellRect.bottom );
	m_memDC.LineTo(cellRect.right, cellRect.bottom );
	m_memDC.LineTo(cellRect.right, cellRect.top - 1);
		//draw cell border

	m_memDC.SelectObject(pOldPen);
		//select previous pen

	m_memDC.SelectClipRgn(&clipRgn);
		//select clipper region into memory DC

	COLORREF bgndColor, textColor;
		//background and text colors

	if (itemState & ODS_FOCUS)
	{
		bgndColor = GetSysColor (COLOR_HIGHLIGHT);
			//get bgnd color for focused item
		textColor = GetSysColor (COLOR_HIGHLIGHTTEXT);
			//get text color for focused item
	}
	else
	if (m_bEnableMultiSelection && (itemState & ODS_SELECTED))
	{
		bgndColor = Blend(GetSysColor (COLOR_HIGHLIGHT), GetSysColor(COLOR_WINDOW), 0.3f);
			//get bgnd color for focused item
		textColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
			//get text color for focused item
	}
	else
	{
		bgndColor = GetSysColor (COLOR_WINDOW);
			//get bgnd color for normal items

		textColor = GetSysColor (COLOR_WINDOWTEXT);
			//get text color for normal items
	}

	itemRect.right--;
		//adjust right boundary

	CBrush bgndBrush(bgndColor);

	if ((itemState & ODS_FOCUS) || (m_bEnableMultiSelection && (itemState & ODS_SELECTED)))
		//draw focus rect if item is focused
		m_memDC.FillRect(itemRect,&bgndBrush);


	LPCTSTR pszText;

	if (m_bEnableItemSelection || m_bDrawIcons)
	{
		itemRect.left += 16;

		int dy = (itemRect.Height() > 16) ? (itemRect.Height() - 16) / 2	: 0;

		if (m_bEnableItemSelection)
		{
			if (GetItemData(iItem) & 0x80000000)
				ImageList_DrawEx((HIMAGELIST)m_imageList, 0, m_memDC.m_hDC, itemRect.left - 16, itemRect.top - 2 + dy, 16, 16, CLR_DEFAULT, CLR_DEFAULT, ILD_TRANSPARENT);
			else
				ImageList_DrawEx((HIMAGELIST)m_imageList, 1, m_memDC.m_hDC, itemRect.left - 16, itemRect.top - 2 + dy, 16, 16, CLR_DEFAULT, CLR_DEFAULT, ILD_TRANSPARENT);
		}
		else
		if (m_bDrawIcons)
		{
			ImageList_DrawEx((HIMAGELIST)m_imageList, (GetItemData(iItem)>>24) & 0x7F, m_memDC.m_hDC, itemRect.left - 16 + 2, itemRect.top - 2 + dy, 16, 16, CLR_DEFAULT, CLR_DEFAULT, ILD_TRANSPARENT);
		}
	}

	itemRect.left += 10;
	itemRect.right -= 2;
		//adjust item rect for text output

	itemRect.right	= GetColumnWidth(0);
	pszText			= MakeShortString(&m_memDC, (LPCTSTR)s, itemRect.Width(), 4);
		//get clipped text for root items

	m_memDC.SetBkColor  (bgndColor);
	m_memDC.SetTextColor(textColor);
	m_memDC.SetBkMode(TRANSPARENT);
		//set text attributes

	m_memDC.DrawText(pszText,-1, itemRect,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
		//draw item text

	m_memDC.SelectClipRgn(NULL);
		//reset clipper region

	if (m_bEnableSecondColumn)
	{
		s = GetItemText(iItem,1);

		CRect subItemRect; GetSubItemRect(iItem, 1, 0, subItemRect);
			//get subitem rectangle

		if ((itemState & ODS_FOCUS) || (m_bEnableMultiSelection && (itemState & ODS_SELECTED)))
			//draw focus rect if subitem is focused
		{
			CRect focusRect = subItemRect; 
				//get subitem rect
			focusRect.left--;
				//adjust right boundary

			CBrush bgndBrush(bgndColor);

			m_memDC.FillRect(focusRect, &bgndBrush);
				//fill it with background color
		}

		pszText = MakeShortString(&m_memDC, (LPCTSTR)s, subItemRect.Width(), 4);
			//get clipped text for root items

		subItemRect.left  += 10;
		subItemRect.right -= 2;
			//adjust item rect for text output
			
		m_memDC.DrawText(pszText,-1, subItemRect,DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
			//draw item text
	}

	pDC->BitBlt(boundsRect.left, boundsRect.top, boundsRect.Width(), boundsRect.Height(), &m_memDC,boundsRect.left, boundsRect.top,SRCCOPY);
		//blt drawn item from memory DC
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::PrepareMemDC(CDC *pDC)
{
	if (m_memDC.GetSafeHdc())
		return;
			//return if memory DC is already created

	CRect clientRect; GetClientRect(&clientRect);
		//get client rect

	m_memDC.CreateCompatibleDC(pDC);
		//create compatible memory DC
	m_bitmap.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
		//create compatible bitmap
	m_memDC.SelectObject(&m_bitmap);
		//select created bitmap into memory DC
	m_memDC.SelectObject(pDC->GetCurrentFont());
		//select font onto memory DC
}
/////////////////////////////////////////////////////////////////////////////
LPCTSTR CCheckedListCtrl::MakeShortString(CDC *pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szThreeDots[]=_T("...");
		//this string will be added to clipped string

	int nStringLen=lstrlen(lpszLong);
		//get string length

	if((nStringLen==0) || (pDC->GetTextExtent(lpszLong,nStringLen).cx + nOffset < nColumnLen))
		return(lpszLong);
			//return if screen fits into specified width and it must not be clipped

	static _TCHAR szShort[MAX_PATH]; lstrcpy(szShort,lpszLong);
		//target string

	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;
		//size of szThreeDots in pixels

	for(int i=nStringLen-1; i > 0; i--)
	{
		szShort[i]=0;
			//terminate last character into target string
		if(pDC->GetTextExtent(szShort,i).cx + nOffset + nAddLen < nColumnLen)
			break;
				//break loop if target string fits into specified width
	} 

	lstrcat(szShort,szThreeDots);
		//add ... to target string

	return(szShort);

}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::DestroyMemDC()
{
	if (m_memDC.GetSafeHdc())
		VERIFY(m_memDC.DeleteDC());
			//destroy memory DC
	if (m_bitmap.GetSafeHandle())
		VERIFY(m_bitmap.DeleteObject());
		//delete bitmap

}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();

	DestroyMemDC();
		//destroy memory DC
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CCheckedListCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (m_bIsDragging)
		if (this != GetCapture())
			EndDragDrop();

	if ((message == WM_NOTIFY) && !wParam)
		//handle hearer notifications
	{
		NMHDR* pNMHDR = (NMHDR*)lParam;
			//get header notification header :)

		if (pNMHDR->code == HDN_DIVIDERDBLCLICKW)
			return 0;
				//skip header double clicks handling

		if (pNMHDR->code == HDN_BEGINTRACKW)
		{
			m_bHeaderDragging = TRUE;
		}

		if (pNMHDR->code == HDN_ENDTRACKW)
		{
			m_bHeaderDragging = FALSE;

			RedrawVisibleItems();

			DestroyMemDC();
			InvalidateBgndRgn();
			RedrawVisibleItems();
		}
	}
	
	return CListCtrl::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCheckedListCtrl::IsHeaderDragging()
{
	return m_bHeaderDragging;
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::RedrawVisibleItems()
{
	int iItems = GetItemCount();
	int iTopIndex = GetTopIndex();
	int iLastIndex = iTopIndex + GetCountPerPage() + 1;

	if (iLastIndex > iItems)
		iLastIndex = iItems;

	RedrawItems(iTopIndex, iLastIndex);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::InvalidateBgndRgn()
{
	CRgn bgndRgn; GetBgndRgn(bgndRgn);
		//get background region

	InvalidateRgn(&bgndRgn, TRUE);
		//invalidate it
}
/////////////////////////////////////////////////////////////////////////////
int CCheckedListCtrl::HitTestOnItem(CPoint point)
{
	int iItem = HitTest(point);
		//get hitted item

	CRect viewRect  = GetItemsRect();
		//get rect which covers all visible items

	if (m_bEnableItemSelection)
		viewRect.left += 16;

	if (!viewRect.PtInRect(point))
		return -1;
			//return -1 if specified point lies outside
			//this rect

	return iItem;
}
/////////////////////////////////////////////////////////////////////////////
int CCheckedListCtrl::HitTestOnItemEx(CPoint point)
{
	int iItem = HitTest(point);
		//get hitted item

	CRect viewRect  = GetItemsRect();
		//get rect which covers all visible items
	if (viewRect.PtInRect(point))
		return iItem;

	if ((point.x >= viewRect.left ) &&
		(point.x <= viewRect.right) &&
		(point.y >= viewRect.bottom))
		return GetItemCount();

	return -1;
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bIsDragging)
	{
		int delta = 0;
		CRect cr; GetClientRect(&cr);

		if (point.y > cr.Height())
			delta = point.y - cr.Height();
		if (point.y < 0)
			delta = point.y;

		if (delta)
		{
			CImageList::DragShowNolock(FALSE);
			Scroll(CSize(0, delta));
			RedrawWindow(NULL, NULL, RDW_UPDATENOW);
			CImageList::DragShowNolock(TRUE);
		}

		int oldDropItem = m_nDropItem;
		m_nDropItem		= HitTestOnItemEx(point);
	
		CImageList::DragMove(point);
	}
	
	CListCtrl::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if(pNMListView->iItem!=-1)
	{
		CRect itemRect;

		GetItemRect(pNMListView->iItem, &itemRect, LVIR_BOUNDS);

		m_nDragItem = pNMListView->iItem;

		CImageList* pDragImage = CreateDragImage(m_nDragItem);

		if(pDragImage)
		{
			CPoint ptHot(	pNMListView->ptAction.x - itemRect.left,
							pNMListView->ptAction.y - itemRect.top);


			pDragImage->BeginDrag(0, ptHot);
			pDragImage->DragEnter(this, pNMListView->ptAction);

			SetCapture();

			m_bIsDragging = TRUE;
		}

		delete pDragImage;
	}
	
	*pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
CImageList* CCheckedListCtrl::CreateDragImage(int iItem)
{
    CImageList *pList = new CImageList;          

	LV_ITEM lvItem;
	lvItem.mask =  LVIF_IMAGE;
	lvItem.iItem = iItem;
	lvItem.iSubItem = 0;
	GetItem(&lvItem);

	CRect rc;
	GetItemRect(iItem, &rc, LVIR_BOUNDS);         

	CString str;
	str = GetItemText(iItem, 0);
	CFont *pFont = GetFont();

	rc.OffsetRect(-rc.left, -rc.top);            
	rc.right = GetColumnWidth(0);       
	if (m_nDragImageColumn)
		rc.right += GetColumnWidth(1);

	pList->Create(rc.Width(), rc.Height(), ILC_COLOR32 | ILC_MASK , 1, 1);

	CDC *pDC = GetDC();                          

	if (pDC) 
	{
		CDC dc;	      
		dc.CreateCompatibleDC(pDC);      
		CBitmap bmpMap;

		bmpMap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		CBitmap *pOldBmp = dc.SelectObject(&bmpMap);
		CFont *pOldFont = dc.SelectObject(pFont);
		dc.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));

		COLORREF cellBorderColor = GetSysColor (COLOR_WINDOW);
			//get cell border color
		COLORREF cellColor0 = RGB(GetRValue(cellBorderColor)*0.90f,
								  GetGValue(cellBorderColor)*0.90f,
								  GetBValue(cellBorderColor)*0.90f);

		CRect cellRect	= rc;
			//get cell rect

		if (m_nDragImageColumn)
			rc.left += GetColumnWidth(0);
	
		CString s = GetItemText(iItem, m_nDragImageColumn);

		dc.FillSolidRect(cellRect, cellColor0);

		COLORREF textColor = GetSysColor (COLOR_WINDOWTEXT);
			//get text color for normal items

		LPCTSTR pszText;
			
		pszText = MakeShortString(&dc, (LPCTSTR)s, rc.Width(), 4);
			//get clipped text for root items

		rc.left  += 10;
		rc.right -= 2;
			//adjust item rect for text output
			
		dc.SetTextColor(textColor);
		dc.SetBkMode(TRANSPARENT);
			//set text attributes

		dc.DrawText(pszText,-1, rc, DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_EXTERNALLEADING);
			//draw item text

		dc.SelectObject(pOldFont);
		dc.SelectObject(pOldBmp);       

		pList->Add(&bmpMap, cellColor0);

		ReleaseDC(pDC);   
	}   

	return pList;
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bIsDragging)
		EndDragDrop();
	
	CListCtrl::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::EndDragDrop()
{
	if (m_bIsDragging)
	{
		m_bIsDragging = FALSE;

		CImageList::DragLeave(this);
		CImageList::EndDrag();
		
		ReleaseCapture();

		CPoint ptCursor; GetCursorPos(&ptCursor); ScreenToClient(&ptCursor);

		int	iTarget		= HitTestOnItemEx(ptCursor);
		int iDropItem	= iTarget;

		if ((iTarget != -1) && (iTarget != m_nDragItem))
		{
			DWORD dwDropItemData = GetItemData((iDropItem < GetItemCount()) ? iDropItem : iDropItem - 1);

			if (m_bEnableMultiSelection)
			{
				SetRedraw(FALSE);

				CArray<DWORD,DWORD> selectedIndices;
				CStringArray		selectedItems;
				CStringArray		selectedSubitems;
				CArray<DWORD,DWORD>	selectedItemData;
				CArray<DWORD,DWORD>	selectedStates;

				for (int iItem=GetItemCount()-1; iItem>=0; iItem--)
				{
					DWORD dwItemState = GetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED);

					if (dwItemState)
					{
						selectedIndices.Add(iItem);
						selectedItems.Add(GetItemText(iItem, 0));
						selectedSubitems.Add(GetItemText(iItem, 1));
						selectedItemData.Add(GetItemData(iItem));
						selectedStates.Add(dwItemState);

						if (iTarget > iItem)
							iTarget--;
					}
				}

				for (int iIndex=0; iIndex<selectedIndices.GetCount(); iIndex++)
					DeleteItem(selectedIndices.GetAt(iIndex));

				for (int iIndex=0; iIndex<selectedItems.GetCount(); iIndex++)
				{
					CString itemText	= selectedItems.GetAt(iIndex);
					CString subItemText = selectedSubitems.GetAt(iIndex);
					DWORD itemData		= selectedItemData.GetAt(iIndex);
					DWORD	itemState	= selectedStates.GetAt(iIndex);

					InsertItem(iTarget, itemText);

					SetItemText(iTarget, 1, subItemText);
					SetItemData(iTarget, itemData);

					SetItemState(iTarget, itemState, LVIS_SELECTED|LVIS_FOCUSED);
					SetSelectionMark(iTarget);
				}

				SetRedraw(TRUE);
			}
			else
			{
				if (iTarget > m_nDragItem)
					iTarget--;

				int iOldSel = GetSelectionMark();

				DWORD dwSelID = 0;

				if (iOldSel != -1)
					dwSelID = GetItemData(iOldSel);

				CString itemText	= GetItemText(m_nDragItem, 0);
				CString subItemText = GetItemText(m_nDragItem, 1);
				DWORD	itemData	= GetItemData(m_nDragItem);

				SetRedraw(FALSE);

				DeleteItem(m_nDragItem);

				InsertItem(iTarget, itemText);
				SetItemText(iTarget, 1, subItemText);
				SetItemData(iTarget, itemData);

				if (iOldSel != -1)
				{
					for (int iItem=0; iItem<GetItemCount(); iItem++)
					{
						if (dwSelID == GetItemData(iItem))
						{
							SetSelectionMark(iItem);
							SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

							break;
						}
					}
				}

				SetRedraw(TRUE);
			}

			GetParent()->PostMessage(UM_DROP_ITEM, iDropItem, dwDropItemData);
		}

		m_nDropItem = -1;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::Init()
{
	ListView_SetExtendedListViewStyle(m_hWnd, LVS_EX_DOUBLEBUFFER);

	m_imageList.Create();
	m_imageList.Add(IDI_SELECT);
	m_imageList.Add(IDI_SELECT_PLACEHOLDER);

	POSITION pos = m_iconsList.GetHeadPosition();

	while (pos)
		m_imageList.Add(m_iconsList.GetNext(pos));
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (!pScrollBar)
		InvalidateBgndRgn();
			//invalidate background
	
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (!pScrollBar)
		InvalidateBgndRgn();
			//invalidate background
	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCheckedListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	InvalidateBgndRgn();
		//invalidate background
	
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OptimalResizeColumn()
{
	DestroyMemDC();

	CRect rc; 
	GetClientRect(&rc);
	SetColumnWidth(0, rc.Width());
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (HitTestOnCheck(point) != -1)
		OnLButtonDown(nFlags, point);
			//hadnle double clicks at +/- and check sings as as single clicks
	else
	{
		GetParent()->PostMessage(UM_DBLCLICK_ITEM, point.x, point.y);

		CListCtrl::OnLButtonDblClk(nFlags, point);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CCheckedListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	int nSelectedItem = GetFocusedItem();
		//get current selected item

	if (nSelectedItem != m_nSelectedItem)
		//check if selected item was changed
	{
		m_nSelectedItem = nSelectedItem;
			//save new selected item

		GetParent()->PostMessage(UM_SEL_CHANGED, nSelectedItem, 0);
			//send UM_SELECTION_CHANGED to parent
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int iCheckItem = HitTestOnCheck(point);

	if (iCheckItem != -1)
	{
		BOOL bApplyToAll		= (GetAsyncKeyState(VK_SHIFT	) < 0);
		BOOL bApplyToSelected	= !(GetAsyncKeyState(VK_CONTROL	) < 0);

		if (bApplyToAll)
		{
			DWORD dwItemData = 0x80000000 ^ (GetItemData(iCheckItem) & 0x80000000);

			SetRedraw(FALSE);

			for (int iItem=0; iItem<GetItemCount(); iItem++)
			{

				SetItemData(iItem, (GetItemData(iItem) & 0x7FFFFFFF) | dwItemData);
				Update(iItem);
				GetParent()->PostMessage(UM_CHECK_CHANGED, iItem, (iItem == iCheckItem) ? 1 : 0);
			}

			SetRedraw(TRUE);
		}
		else
		if (bApplyToSelected)
		{
			DWORD dwItemData = 0x80000000 ^ (GetItemData(iCheckItem) & 0x80000000);

			SetRedraw(FALSE);

			if (GetSelectedCount() > 1)
			{
				for (int iItem=0; iItem<GetItemCount(); iItem++)
				{
					if ((iItem != iCheckItem) && GetItemState(iItem, LVIS_SELECTED))
					{
						SetItemData(iItem, (GetItemData(iItem) & 0x7FFFFFFF) | dwItemData);
						Update(iItem);
						GetParent()->PostMessage(UM_CHECK_CHANGED, iItem, (iItem == iCheckItem) ? 1 : 0);
					}
				}
			}

			SetItemData(iCheckItem, (GetItemData(iCheckItem) & 0x7FFFFFFF) | dwItemData);
			Update(iCheckItem);

			GetParent()->PostMessage(UM_CHECK_CHANGED, iCheckItem, 1);

			SetRedraw(TRUE);
		}
		else
		{
			SetItemData(iCheckItem, 0x80000000 ^ GetItemData(iCheckItem));
			Update(iCheckItem);

			GetParent()->PostMessage(UM_CHECK_CHANGED, iCheckItem, 1);
		}
		
		return;
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
int CCheckedListCtrl::HitTestOnCheck(CPoint point)
{
	if (!m_bEnableItemSelection)
		return -1;

	int iItem = HitTest(point);
		//get hitted item

	CRect viewRect  = GetItemsRect();
		//get rect which covers all visible items

	viewRect.right = viewRect.left + 16;

	if (!viewRect.PtInRect(point))
		return -1;
			//return -1 if specified point lies outside
			//this rect

	return iItem;
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::EnableItemSelection(BOOL bEnable)
{
	m_bEnableItemSelection = bEnable;
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::EnableSecondColumn(BOOL bEnable)
{
	m_bEnableSecondColumn = bEnable;
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::EnableMultiSelection(BOOL bEnable)
{
	m_bEnableMultiSelection = bEnable;
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::DrawIcons(BOOL bDrawIcons)
{
	m_bDrawIcons = bDrawIcons;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CCheckedListCtrl::Blend(DWORD dwColor0, DWORD dwColor1, float fltRatio)
{
	LONG r0	= 0xFF & (dwColor0);
	LONG g0	= 0xFF & (dwColor0>>8);
	LONG b0	= 0xFF & (dwColor0>>16);

	LONG r1	= 0xFF & (dwColor1);
	LONG g1	= 0xFF & (dwColor1>>8);
	LONG b1	= 0xFF & (dwColor1>>16);

	LONG r		= min(255, (DWORD)(r0 + (r1 - r0) * fltRatio));
	LONG g		= min(255, (DWORD)(g0 + (g1 - g0) * fltRatio));
	LONG b		= min(255, (DWORD)(b0 + (b1 - b0) * fltRatio));

	return (r | (g<<8) | (b<<16));
}
/////////////////////////////////////////////////////////////////////////////
int CCheckedListCtrl::GetFocusedItem()
{
	if (m_bEnableMultiSelection)
	{
		for (int iItem=0; iItem<GetItemCount(); iItem++)
		{
			if (GetItemState(iItem, LVIS_FOCUSED))
				return iItem;
		}
	}

	return GetSelectionMark();
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::SetResourceHandle(HINSTANCE hModule)
{
	m_imageList.SetResourceHandle(hModule);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::AddIcon(int nIconID)
{
	m_iconsList.AddTail(nIconID);
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::SetDragImageColumn(int nColumn)
{
	m_nDragImageColumn = nColumn;
}
/////////////////////////////////////////////////////////////////////////////
int CCheckedListCtrl::SaveScrollPos()
{
	CRect rc;
	GetItemRect(0, rc, LVIR_BOUNDS);

	return GetTopIndex() * rc.Height();
}
/////////////////////////////////////////////////////////////////////////////
void CCheckedListCtrl::RestoreScrollPos(int nScrollPos, BOOL bEnsureVisible)
{
	Scroll(CSize(0, nScrollPos));

	if (bEnsureVisible)
	{
		int iItem = GetSelectionMark();
		if (iItem != -1)
			EnsureVisible(iItem, FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
