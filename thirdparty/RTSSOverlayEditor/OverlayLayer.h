// OverlayLayer.h: interface for the COverlayLayer class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "HypertextExtension.h"
#include "DynamicColor.h"
/////////////////////////////////////////////////////////////////////////////
#define DEFAULT_ALIGNMENT										0
#define DEFAULT_SIZE											100
#define DEFAULT_COLOR											0xDEADBEEF
#define DEFAULT_TEXT											"Text layer"
/////////////////////////////////////////////////////////////////////////////
#define OSD_OWNER_NAME											"RTSSOverlayEditor"
/////////////////////////////////////////////////////////////////////////////
#define HIT_TEST_SIZING_BORDER									4
/////////////////////////////////////////////////////////////////////////////
#define HIT_TEST_MASK_BODY										1
#define HIT_TEST_MASK_LEFT										2
#define HIT_TEST_MASK_RIGHT										4
#define HIT_TEST_MASK_TOP										8
#define HIT_TEST_MASK_BOTTOM									16
/////////////////////////////////////////////////////////////////////////////
class COverlay;
class COverlayLayer : public CHypertextExtension
{
public:
	COverlayLayer();
	virtual ~COverlayLayer();

	void	SetOverlay(COverlay* lpOverlay);
		//sets overlay ptr

	void	Load();
		//loads overlay layer
	void	Save();
		//save overlay layer
	void	Copy(COverlayLayer* lpSrc);
		//copies overlay layer
	BOOL	IsEqual(COverlayLayer* lpSrc);
		//compares two overlay layers

	CString	GetHypertext(BOOL bOptimize, LPBYTE lpEmbeddedObjectBuffer, DWORD dwEmbeddedObjectBufferSize, DWORD& dwEmbeddedObjectOffset);	
		//returns fully formatted layer hypertext and embeds all objects into the buffer
	void	ApplyTag(CString& str, CString strBeginTag, CString strEndTag);
		//applies surrounding tags to the text

	DWORD	HitTest(POINT pt);
		//hittests overlay rectangle and returns hitzone mask

	DWORD	GetLayerID();
		//returns layer ID
	void	SetLayerID(DWORD dwLayerID);
		//sets layer ID

	virtual CString GetTextFromFile(LPCSTR lpFile);
		//returns the text from external file for file hypertext extension tag
	CString	GetTextFile();
		//returns name of text file specified in file hypertext extension tag 
	void	LoadTextFile(BOOL bResetLine);
		//reloads text file specified in file hypertexte extension tag and optionally resets line index
	void	ViewTextFile();
		//opens text file specified in file hypertext extension tag
	void	MoveToNextTextFileLine();
		//moves to the next line of text file specified in file hypertext extension tag
	void	MoveToPrevTextFileLine();
		//moves to the previous line of text file specified in file hypertext extension tag

	static DWORD SwapRGB(DWORD dwColor);
		//converts RGB to BGR and vice versa
	static DWORD PackRGBA(DWORD dwColor);
		//packs RGBA, removes alpha if color is opaque
	static DWORD UnpackRGBA(DWORD dwColor);
		//unpacks RGBA, adds opaque alpha if it is not specified (i.e 0x112233 is treated as 0xFF112233)
	static DWORD UnpackRGBA(LPCSTR lpColor);
		//unpacks RGBA string, adds opaque alpha if it is not specified (i.e 112233 is treated as FF112233)

	static CString DecodeSpecialChars(CString str);
		//decodes special chars (replaces \\n with \n ans so on)
	static CString EncodeSpecialChars(CString str);
		//encodes special chars (replaces \n with \\n ans so on)

	DWORD GetDynamicColor(CString strColor, CDynamicColor& color);
	DWORD GetDynamicTextColor(CDynamicColor& color);
	DWORD GetDynamicBgndColor(CDynamicColor& color);
	DWORD GetDynamicBorderColor(CDynamicColor& color);

	CString		m_strName;
	CString		m_strText;
	int			m_nPositionX;
	int			m_nPositionY;
	int			m_nPositionSticky;
	int			m_nExtentX;
	int			m_nExtentY;
	int			m_nExtentOrigin;
	int			m_nAlignment;
	BOOL		m_bFixedAlignment;
	int			m_nSize;
	CString		m_strTextColor;
	CString		m_strBgndColor;
	CString		m_strBorderColor;
	int			m_nBorderSize;
	RECT		m_rect;
	DWORD		m_dwRefreshPeriod;
	DWORD		m_dwRefreshTimestamp;
	int			m_nMarginLeft;
	int			m_nMarginTop;
	int			m_nMarginRight;
	int			m_nMarginBottom;

	BOOL		m_bSelected;

protected:
	COverlay*	m_lpOverlay;
	DWORD		m_dwLayerID;

	CString		m_strTextFile;
	int			m_nTextFileLine;
	CStringList	m_textFileLines;
};
/////////////////////////////////////////////////////////////////////////////
