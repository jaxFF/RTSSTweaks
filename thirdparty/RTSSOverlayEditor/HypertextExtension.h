// HypertextExtension.h: interface for the CHypertextExtension class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "HypertextParser.h"
//////////////////////////////////////////////////////////////////////
#define DEFAULT_GRAPH_WIDTH										-16
#define DEFAULT_GRAPH_HEIGHT									-2
#define DEFAULT_GRAPH_MARGIN									1
#define DEFAULT_GRAPH_MIN										0
#define DEFAULT_GRAPH_MAX										100
#define DEFAULT_GRAPH_FLAGS										0
//////////////////////////////////////////////////////////////////////
#define DEFAULT_ANIMATED_IMAGE_WIDTH							-2
#define DEFAULT_ANIMATED_IMAGE_HEIGHT							-2
#define DEFAULT_ANIMATED_IMAGE_MIN								0
#define DEFAULT_ANIMATED_IMAGE_MAX								100
#define DEFAULT_ANIMATED_IMAGE_SPRITE_X							0
#define DEFAULT_ANIMATED_IMAGE_SPRITE_Y							0
#define DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH						128
#define DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT					128
#define DEFAULT_ANIMATED_IMAGE_SPRITES_NUM						1
#define DEFAULT_ANIMATED_IMAGE_SPRITES_PER_LINE					0
#define DEFAULT_ANIMATED_IMAGE_BIAS								0
#define DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE					0
/////////////////////////////////////////////////////////////////////////////
#define OBJ_UNKNOWN												-1
#define OBJ_GRAPH												0
#define OBJ_IMAGE												1
#define OBJ_ANIMATED_IMAGE										2
#define OBJ_MACRO												3
#define OBJ_TEXT_TABLE											4
#define OBJ_TEXT_TAG											5
/////////////////////////////////////////////////////////////////////////////
#define	GRAPH_TAG												"<G"
#define GRAPH_TAG_LEN											2
#define	IMAGE_TAG												"<I"
#define IMAGE_TAG_LEN											2
#define	ANIMATED_IMAGE_TAG										"<AI"
#define ANIMATED_IMAGE_TAG_LEN									3
#define FILE_TAG												"<F"
#define FILE_TAG_LEN											2
#define	TEXT_TABLE_TAG											"<TT"
#define TEXT_TABLE_TAG_LEN										3

#define GRAPH_MIN_TAG											"<GRMIN>"
#define GRAPH_MIN_TAG_LEN										7
#define GRAPH_MAX_TAG											"<GRMAX>"
#define GRAPH_MAX_TAG_LEN										7
#define FRAMERATE_TAG											"<FR>"
#define FRAMERATE_TAG_LEN										4
#define FRAMERATE_MIN_TAG										"<FRMIN>"
#define FRAMERATE_MIN_TAG_LEN									7
#define FRAMERATE_AVG_TAG										"<FRAVG>"
#define FRAMERATE_AVG_TAG_LEN									7
#define FRAMERATE_MAX_TAG										"<FRMAX>"
#define FRAMERATE_MAX_TAG_LEN									7
#define FRAMERATE_10L_TAG										"<FR10L>"
#define FRAMERATE_10L_TAG_LEN									7
#define FRAMERATE_01L_TAG										"<FR01L>"
#define FRAMERATE_01L_TAG_LEN									7
#define FRAMETIME_TAG											"<FT>"
#define FRAMETIME_TAG_LEN										4
#define FRAME_COUNT_TAG											"<FC>"
#define FRAME_COUNT_TAG_LEN										4
#define APP_TAG													"<APP>"
#define APP_TAG_LEN												5
#define API_TAG													"<API>"
#define API_TAG_LEN												5
#define EXE_TAG													"<EXE>"
#define EXE_TAG_LEN												5
#define RESOLUTION_TAG											"<RES>"
#define RESOLUTION_TAG_LEN										5
#define ARCHITECTURE_TAG										"<ARCH>"
#define ARCHITECTURE_TAG_LEN									6
#define TIME_TAG												"<TIME"
#define TIME_TAG_LEN											5
#define BENCHMARK_TIME_TAG										"<BTIME>"
#define BENCHMARK_TIME_TAG_LEN									7
/////////////////////////////////////////////////////////////////////////////
class COverlay;
class CHypertextExtension : public CHypertextParser
{
public:
	CHypertextExtension();
	virtual ~CHypertextExtension();

	CString	TranslateExtensionString(COverlay* lpOverlay, LPCSTR lpString, BOOL bOptimize, BOOL bEmbed, LPBYTE lpEmbeddedObjectBuffer, DWORD dwEmbeddedObjectBufferSize, DWORD& dwEmbeddedObjectOffset, DWORD dwColor, int nSize, BOOL bContextHighlighting = TRUE);
		// This function perfoms the following things:

		//- translates hypertext extension string and replaces all extension tags with native RTSS hypertext tags
		//- optionally embeds translated objects into the buffer
		//- recognizes embedded object tag in cursor position and saves object type and text boundaries for context highlighting

	LPCSTR	ScanGraphTag(LPCSTR lpsz, CString& strSource, DWORD& dwWidth, DWORD& dwHeight, DWORD& dwMargin, float& fltMin, float& fltMax, DWORD& dwFlags, int& nParams);
		//scans graph hypertext extension tag, returns pointer to the end of tag or NULL if tag is not recogninzed
	LPCSTR	ScanImageTag(LPCSTR lpsz, DWORD& dwWidth, DWORD& dwHeight, DWORD& dwSpriteX, DWORD& dwSpriteY, DWORD& dwSpriteWidth, DWORD& dwSpriteHeight, int& nRotationAngle);
		//scans image hypertext extension tag, returns pointer to the end of tag or NULL if tag is not recogninzed
	LPCSTR	ScanAnimatedImageTag(LPCSTR lpsz, CString& strSource, DWORD& dwWidth, DWORD& dwHeight, float& fltMin, float& fltMax, DWORD& dwSpriteX, DWORD& dwSpriteY, DWORD& dwSpriteWidth, DWORD& dwSpriteHeight, DWORD& dwSpritesNum, DWORD& dwSpritesPerLine, float& fltBias, int& nRotationAngleMin, int& nRotationAngleMax, int& nParams);
		//scans animated image hypertext extension tag, returns pointer to the end of tag or NULL if tag is not recogninzed
	LPCSTR	ScanFileTag(LPCSTR lpsz, CString& strFile);
		//scans file hypertext extension tag, returns pointer to the end of tag or NULL if tag is not recogninzed
	LPCSTR	ScanTextTableTag(LPCSTR lpsz, CString& strTable);
		//scans text table hypertext extension tag, returns pointer to the end of tag or NULL if tag is not recogninzed

	LPCSTR	ScanTimeTag(LPCSTR lpsz, CString& strTimeFormat);
		//scans hypertext time tag, returns pointer to the end of tag or NULL if tag is not recogninzed
	BOOL	ValidateTimeFormat(LPCSTR lpFormat);
		//validates time format for hypertext time tag

	virtual CString GetTextFromFile(LPCSTR lpFile);
		//returns the text from external file for file hypertext extension tag

	CString FormatGraphTag(BOOL bCustomTemplate, LPCSTR lpSource, DWORD dwWidth, DWORD dwHeight, DWORD dwMargin, float fltMin, float fltMax, DWORD dwFlags);
		//formats graph hypertext extension tag
	CString	FormatAnimatedImageTag(BOOL bCustomTemplate, LPCSTR lpSource, DWORD dwWidth, DWORD dwHeight, float fltMin, float fltMax, DWORD dwSpriteX, DWORD dwSpriteY, DWORD dwSpriteWidth, DWORD dwSpriteHeight, DWORD dwSpritesNum, DWORD dwSpritesPerLine, float fltBias, int nRotationAngleMin, int nRotationAngleMax);
		//formats animated image hypertext extension tag
	CString	FormatImageTag(DWORD dwWidth, DWORD dwHeight, DWORD dwSpriteX, DWORD dwSpriteY, DWORD dwSpriteWidth, DWORD dwSpriteHeight, int nRotationAngle);
		//formats image hypertext extension tag
	CString	FormatImageTag(DWORD dwWidth, DWORD dwHeight, LPCSTR lpSpriteX, LPCSTR lpSpriteY, DWORD dwSpriteWidth, DWORD dwSpriteHeight, LPCSTR lpRotationAngle);
		//formats image hypertext extension tag using dynamic variables

	CString	FormatVarSpriteFnTag(DWORD dwVariable, LPCSTR lpSource, float fltMin, float fltMax, float fltBias, DWORD dwSpriteX, DWORD dwSpriteY, DWORD dwSpriteWidth, DWORD dwSpriteHeight, DWORD dwSpritesNum, DWORD dwSpritesPerLine);
		//formats variable assignment tag using sprite function
	CString	FormatMapFn(LPCSTR lpSource, float fltMin, float fltMax, float fltDstMin, float fltDstMax);
		//formats map function

	void	SetPos(int nPos);
	int		GetPos();	
		//sets/returns cursor position
		
		//NOTE: cursor position is used inside TranslateExtensionString for context highlighting 

	int		GetObj();
	int		GetObjFrom();
	int		GetObjTo();
	int		GetObjParams();
		//returns the last recognized embedded object type and its text boundaries

	void	SetObj(int nObjFrom, int nObjTo);

protected:
	int		m_nPos;

	int		m_nObj;
	int		m_nObjFrom;
	int		m_nObjTo;
	int		m_nObjParams;

	DWORD	m_dwColor;
	int		m_nSize;
};
//////////////////////////////////////////////////////////////////////
