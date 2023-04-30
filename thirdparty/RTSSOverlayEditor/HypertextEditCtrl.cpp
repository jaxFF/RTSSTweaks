// HypertextEditCtrl.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "HypertextEditCtrl.h"
#include "Overlay.h"
#include "OverlayDataSourceInternal.h"
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CHypertextEditCtrl
/////////////////////////////////////////////////////////////////////////////
CHypertextEditCtrl::CHypertextEditCtrl()
{
	m_lpOverlay					= NULL;

	m_strGraphSource			= "";
	m_dwGraphWidth				= DEFAULT_GRAPH_WIDTH;
	m_dwGraphHeight				= DEFAULT_GRAPH_HEIGHT;
	m_dwGraphMargin				= DEFAULT_GRAPH_MARGIN;
	m_fltGraphMin				= DEFAULT_GRAPH_MIN;
	m_fltGraphMax				= DEFAULT_GRAPH_MAX;
	m_dwGraphFlags				= DEFAULT_GRAPH_FLAGS;
	m_nGraphParams				= 0;

	m_strImageSource			= "";
	m_dwImageWidth				= DEFAULT_ANIMATED_IMAGE_WIDTH;
	m_dwImageHeight				= DEFAULT_ANIMATED_IMAGE_HEIGHT;
	m_fltImageMin				= DEFAULT_ANIMATED_IMAGE_MIN;
	m_fltImageMax				= DEFAULT_ANIMATED_IMAGE_MAX;
	m_dwImageSpriteX			= DEFAULT_ANIMATED_IMAGE_SPRITE_X;
	m_dwImageSpriteY			= DEFAULT_ANIMATED_IMAGE_SPRITE_Y;
	m_dwImageSpriteWidth		= DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH;
	m_dwImageSpriteHeight		= DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT;
	m_dwImageSpritesNum			= DEFAULT_ANIMATED_IMAGE_SPRITES_NUM;
	m_dwImageSpritesPerLine		= DEFAULT_ANIMATED_IMAGE_SPRITES_PER_LINE;
	m_fltImageBias				= DEFAULT_ANIMATED_IMAGE_BIAS;
	m_nImageRotationAngleMin	= DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
	m_nImageRotationAngleMax	= DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
	m_nImageParams				= 0;

	m_strTextTable				= "";

	m_nChar						= 0;
}
/////////////////////////////////////////////////////////////////////////////
CHypertextEditCtrl::~CHypertextEditCtrl()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHypertextEditCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CHypertextEditCtrl)
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_WM_CHAR()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CHypertextEditCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
void CHypertextEditCtrl::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextEditCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRichEditCtrl::OnLButtonUp(nFlags, point);

	UpdateContextHighlighting();
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextEditCtrl::UpdateContextHighlighting()
{
	SetRedraw(FALSE);
		//we'll temporarily alter text selection inside this function, so we
		//need to disable redraw in order to avoid flickering

	POINT pt;
	SendMessage(EM_GETSCROLLPOS, 0, (LPARAM)&pt);
		//selection change will alter CRichEditCtrl scroll position, so we 
		//need to save it

	BOOL bHighlight = FALSE;

	CString strText;
	GetWindowText(strText);

	long nFrom, nTo;
	GetSel(nFrom, nTo);
		//we'll temporarily alter text selection inside this function, so we
		//need to save original selection

	SetPos(nFrom);
		//set cursor position for hypertext extensions

	DWORD	dwUnused;	
	TranslateExtensionString(m_lpOverlay, strText, FALSE, FALSE, NULL, 0, dwUnused, DEFAULT_COLOR, DEFAULT_SIZE);
		//translate hypertext extension string without actually embedding 
		//objects into the buffer

		//after this call hypertext extension can return embedded object 
		//type and its text boundaries for cursor position

	CHARFORMAT cf;
	cf.cbSize		= sizeof(cf);
	cf.dwMask		= CFM_COLOR;
	cf.dwEffects	= 0;
	cf.crTextColor	= 0;
	SetSel(0, -1);
	SetWordCharFormat(cf);
		//set default color for whole text

	int nObj = GetObj();
		//get embedded object type for the cursor position

	if (nObj == OBJ_TEXT_TABLE)
		//we're on embedded text table object
	{
		m_strTextTable = "";

		if (ScanTextTableTag((LPCSTR)strText + GetObjFrom() + TEXT_TABLE_TAG_LEN, m_strTextTable))
			//scan embedded graph tag to get graph template properties
		{
			CHARFORMAT cf;
			cf.cbSize		= sizeof(cf);
			cf.dwMask		= CFM_COLOR;
			cf.dwEffects	= 0;
	
			BOOL bError		= FALSE;

			if (!m_lpOverlay->FindTextTable(m_strTextTable))
				//validate text table name, specifed in tag
				bError		= TRUE;

			if (bError)
				//highlight text table tag with red color if definition is invalid
				cf.crTextColor = RGB(0xFF, 0, 0);
			else
				//otherwise highlight text table tag with green color
				cf.crTextColor = RGB(0, 0xC0, 0);

			SetSel(GetObjFrom(), GetObjTo());
			SetWordCharFormat(cf);
		}
	}
	else
	if (nObj == OBJ_GRAPH)
		//we're on embedded graph object
	{
		m_strGraphSource	= "";
		m_dwGraphWidth		= DEFAULT_GRAPH_WIDTH;
		m_dwGraphHeight		= DEFAULT_GRAPH_HEIGHT;
		m_dwGraphMargin		= DEFAULT_GRAPH_MARGIN;
		m_fltGraphMin		= DEFAULT_GRAPH_MIN;
		m_fltGraphMax		= DEFAULT_GRAPH_MAX;
		m_dwGraphFlags		= DEFAULT_GRAPH_FLAGS;
		m_nGraphParams		= 0;

		if (ScanGraphTag((LPCSTR)strText + GetObjFrom() + GRAPH_TAG_LEN, m_strGraphSource, m_dwGraphWidth, m_dwGraphHeight, m_dwGraphMargin, m_fltGraphMin, m_fltGraphMax, m_dwGraphFlags, m_nGraphParams))
			//scan embedded graph tag to get graph template properties
		{
			CHARFORMAT cf;
			cf.cbSize		= sizeof(cf);
			cf.dwMask		= CFM_COLOR;
			cf.dwEffects	= 0;

			BOOL bError		= FALSE;

			if (!m_lpOverlay->FindDataSource(m_strGraphSource))
				//validate data source, specifed in graph tag
				bError		= TRUE;

			if (bError)
				//highlight graph tag with red color if definition is invalid
				cf.crTextColor = RGB(0xFF, 0, 0);
			else
				//otherwise highlight graph tag with green color
				cf.crTextColor = RGB(0, 0xC0, 0);

			SetSel(GetObjFrom(), GetObjTo());
			SetWordCharFormat(cf);
		}
	}
	else
	if (nObj == OBJ_IMAGE)
		//we're on embedded image object
	{
		m_dwImageWidth			= DEFAULT_ANIMATED_IMAGE_WIDTH;
		m_dwImageHeight			= DEFAULT_ANIMATED_IMAGE_HEIGHT;
		m_dwImageSpriteX		= DEFAULT_ANIMATED_IMAGE_SPRITE_X;
		m_dwImageSpriteY		= DEFAULT_ANIMATED_IMAGE_SPRITE_Y;
		m_dwImageSpriteWidth	= DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH;
		m_dwImageSpriteHeight	= DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT;
		m_nImageRotationAngle	= DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;

		if (ScanImageTag((LPCSTR)strText + GetObjFrom() + IMAGE_TAG_LEN, m_dwImageWidth, m_dwImageHeight, m_dwImageSpriteX, m_dwImageSpriteY, m_dwImageSpriteWidth, m_dwImageSpriteHeight, m_nImageRotationAngle))
			//scan embedded image tag to get image template properties
		{
			CHARFORMAT cf;
			cf.cbSize		= sizeof(cf);
			cf.dwMask		= CFM_COLOR;
			cf.dwEffects	= 0;

			BOOL bError		= FALSE;

			if (!m_dwImageSpriteWidth || 
				!m_dwImageSpriteHeight)
				//validate sprite dimensions, both width and height must be non-zero
				bError		= TRUE;

			if (bError)
				//highlight image tag with red color if definition is invalide
				cf.crTextColor = RGB(0xFF, 0, 0);
			else
				//otherwise highlight image tag with green color
				cf.crTextColor = RGB(0, 0xC0, 0);

			SetSel(GetObjFrom(), GetObjTo());
			SetWordCharFormat(cf);
		}
	}
	else
	if (nObj == OBJ_ANIMATED_IMAGE)
		//we're on embedded animated image object
	{
		m_strImageSource		= "";
		m_dwImageWidth			= DEFAULT_ANIMATED_IMAGE_WIDTH;
		m_dwImageHeight			= DEFAULT_ANIMATED_IMAGE_HEIGHT;
		m_fltImageMin			= DEFAULT_ANIMATED_IMAGE_MIN;
		m_fltImageMax			= DEFAULT_ANIMATED_IMAGE_MAX;
		m_dwImageSpriteX		= DEFAULT_ANIMATED_IMAGE_SPRITE_X;
		m_dwImageSpriteY		= DEFAULT_ANIMATED_IMAGE_SPRITE_Y;
		m_dwImageSpriteWidth	= DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH;
		m_dwImageSpriteHeight	= DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT;
		m_dwImageSpritesNum		= DEFAULT_ANIMATED_IMAGE_SPRITES_NUM;
		m_dwImageSpritesPerLine	= DEFAULT_ANIMATED_IMAGE_SPRITES_PER_LINE;
		m_fltImageBias			= DEFAULT_ANIMATED_IMAGE_BIAS;
		m_nImageRotationAngleMin = DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
		m_nImageRotationAngleMax = DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
		m_nImageParams			= 0;

		if (ScanAnimatedImageTag((LPCSTR)strText + GetObjFrom() + ANIMATED_IMAGE_TAG_LEN, m_strImageSource, m_dwImageWidth, m_dwImageHeight, m_fltImageMin, m_fltImageMax, m_dwImageSpriteX, m_dwImageSpriteY, m_dwImageSpriteWidth, m_dwImageSpriteHeight, m_dwImageSpritesNum, m_dwImageSpritesPerLine, m_fltImageBias, m_nImageRotationAngleMin, m_nImageRotationAngleMax, m_nImageParams))
			//scan embedded animated image tag to get animated image template properties
		{
			CHARFORMAT cf;
			cf.cbSize		= sizeof(cf);
			cf.dwMask		= CFM_COLOR;
			cf.dwEffects	= 0;

			BOOL bError		= FALSE;

			if (!m_lpOverlay->FindDataSource(m_strImageSource))
				//validate data source, specifed in animated tag
				bError		= TRUE;

			if (!m_dwImageSpriteWidth	|| 
				!m_dwImageSpriteHeight	||
				!m_dwImageSpritesNum)
				//validate sprite dimensions and sprites number, both width and height must be non-zero and sprites number must be non-zero too
				bError		= TRUE;

			if (bError)
				//highlight animated image tag with red color if definition is invalid
				cf.crTextColor = RGB(0xFF, 0, 0);
			else
				//otherwise highlight animated image tag with green color
				cf.crTextColor = RGB(0, 0xC0, 0);

			SetSel(GetObjFrom(), GetObjTo());
			SetWordCharFormat(cf);
		}
	}
	else
	if (nObj == OBJ_MACRO)
		//we're on macro
	{
		CHARFORMAT cf;
		cf.cbSize		= sizeof(cf);
		cf.dwMask		= CFM_COLOR;
		cf.dwEffects	= 0;

		cf.crTextColor = RGB(0xC0, 0, 0xC0);
			//highlight macro with purple color

		SetSel(GetObjFrom(), GetObjTo());
		SetWordCharFormat(cf);
	}
	else
	if (nObj == OBJ_TEXT_TAG)
		//we're on hypertext tag
	{
		CHARFORMAT cf;
		cf.cbSize		= sizeof(cf);
		cf.dwMask		= CFM_COLOR;
		cf.dwEffects	= 0;

		if (GetObjParams() < 0)
			//highlight text tag with red color if definition is invalid
			//NOTE: we use params counter to pass info about tags with syntax errors (e.g. time tag with invalid time format)
			cf.crTextColor = RGB(0xFF, 0, 0);
		else
			//otherwise highlight text tag with green color
			cf.crTextColor = RGB(0, 0xC0, 0);

		SetSel(GetObjFrom(), GetObjTo());
		SetWordCharFormat(cf);
	}

	SendMessage(EM_SETSCROLLPOS, 0, (LPARAM)&pt);
	SetSel(nFrom, nTo);
	SetRedraw(TRUE);
		//restore everything we saved, enable redraw and redraw the control

	RedrawWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextEditCtrl::SetWindowTextEx(LPCSTR lpText)
{
	SetWindowText(lpText);

	UpdateContextHighlighting();
}
/////////////////////////////////////////////////////////////////////////////
CString CHypertextEditCtrl::GetGraphSource()
{
	return m_strGraphSource;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetGraphWidth()
{
	return m_dwGraphWidth;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetGraphHeight()
{
	return m_dwGraphHeight;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetGraphMargin()
{
	return m_dwGraphMargin;
}
/////////////////////////////////////////////////////////////////////////////
float CHypertextEditCtrl::GetGraphMin()
{
	return m_fltGraphMin;
}
/////////////////////////////////////////////////////////////////////////////
float CHypertextEditCtrl::GetGraphMax()
{
	return m_fltGraphMax;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetGraphFlags()
{
	return m_dwGraphFlags;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextEditCtrl::GetGraphParams()
{
	return m_nGraphParams;
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextEditCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_RETURN)
		//restrict Ctrl+Enter handling, we want to embed newline symbols manually
	{
		if (GetAsyncKeyState(VK_CONTROL) < 0)
			return;
	}

	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	//NOTE: we do not update context highlighting while selecting the text from keyboard with <Shift> key because
	//context highlighting is internally altering text selection, which conflicts with selectoin from keyboard
	//To avoid this we simply update context highlighting as soon as <Shift> is released

	if (GetAsyncKeyState(VK_SHIFT) >= 0)
		UpdateContextHighlighting();
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextEditCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	//NOTE: we do not update context highlighting while selecting the text from keyboard with <Shift> key because
	//context highlighting is internally altering text selection, which conflicts with selectoin from keyboard
	//To avoid this we simply update context highlighting as soon as <Shift> is released

	if (GetAsyncKeyState(VK_SHIFT) >= 0)
		UpdateContextHighlighting();
}
/////////////////////////////////////////////////////////////////////////////
CString CHypertextEditCtrl::GetImageSource()
{
	return m_strImageSource;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetImageWidth()
{
	return m_dwImageWidth;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetImageHeight()
{
	return m_dwImageHeight;
}
/////////////////////////////////////////////////////////////////////////////
float CHypertextEditCtrl::GetImageMin()
{
	return m_fltImageMin;
}
/////////////////////////////////////////////////////////////////////////////
float CHypertextEditCtrl::GetImageMax()
{
	return m_fltImageMax;
}
/////////////////////////////////////////////////////////////////////////////
float CHypertextEditCtrl::GetImageBias()
{
	return m_fltImageBias;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextEditCtrl::GetImageRotationAngleMin()
{
	return m_nImageRotationAngleMin;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextEditCtrl::GetImageRotationAngleMax()
{
	return m_nImageRotationAngleMax;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextEditCtrl::GetImageRotationAngle()
{
	return m_nImageRotationAngle;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetImageSpriteX()
{
	return m_dwImageSpriteX;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetImageSpriteY()
{
	return m_dwImageSpriteY;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetImageSpriteWidth()
{
	return m_dwImageSpriteWidth;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetImageSpriteHeight()
{
	return m_dwImageSpriteHeight;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetImageSpritesNum()
{
	return m_dwImageSpritesNum;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CHypertextEditCtrl::GetImageSpritesPerLine()
{
	return m_dwImageSpritesPerLine;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextEditCtrl::GetImageParams()
{
	return m_nImageParams;
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextEditCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//NOTE: we do not call CRichEditCtrl::OnLButtonDblClk on purpose in order to
	//block its double click word select functionality

	GetParent()->PostMessageA(UM_EDIT_EMBEDDED_OBJECT, 0, 0);
}
/////////////////////////////////////////////////////////////////////////////
CString CHypertextEditCtrl::GetTextTable()
{
	return m_strTextTable; 
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((m_nChar == '<') && (nChar == '>'))
		//open tag browser menu after typing in <> sequence
		GetParent()->PostMessageA(UM_BROWSE_MACRO, BROWSEMACRO_MODE_TAG, 0);

	if ((m_nChar == '%') && (nChar == '%'))
		//open macro browser menu after typing in %% sequence
	{
		GetParent()->PostMessageA(UM_BROWSE_MACRO, BROWSEMACRO_MODE_MACRO, 0);

		nChar = 0;
			//reset previous char to avoid displaying macro browser menu after typing in %%% sequence
	}

	m_nChar = nChar;

	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
