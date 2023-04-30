// DynamicColor.h: interface for the CCPUInfo class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "HypertextParser.h"
/////////////////////////////////////////////////////////////////////////////
#define DYNAMIC_COLOR_RANGES_MAX							5		
#define DYNAMIC_COLOR_FLAGS_BLEND							0x00000001
#define DYNAMIC_COLOR_FLAGS_EMBED							0x00000002
//////////////////////////////////////////////////////////////////////
typedef struct DYNAMIC_COLOR_RANGE_DESC
{
	DWORD						dwColor;
	float						fltMin;
	float						fltMax;
} DYNAMIC_COLOR_RANGE_DESC,*LPDYNAMIC_COLOR_RANGE_DESC;
//////////////////////////////////////////////////////////////////////
typedef struct DYNAMIC_COLOR_DESC
{
	DWORD						dwCount;
	DYNAMIC_COLOR_RANGE_DESC	ranges[DYNAMIC_COLOR_RANGES_MAX];
} DYNAMIC_COLOR_DESC,*LPDYNAMIC_COLOR_DESC;
//////////////////////////////////////////////////////////////////////
class COverlay;
class CDynamicColor : public CHypertextParser
{
public:
	CDynamicColor();
	virtual ~CDynamicColor();

	DWORD	GetRangeCount();
	BOOL	IsInternalSource();
	BOOL	IsEmbedded();

	void	SetStatic(DWORD dwColor);
	void	AddRange(DWORD dwColor, LPCSTR lpMin, LPCSTR lpMax);
	void	Destroy();

	void	GetRange(float& fltMin, float& fltMax);
	CString	FormatRangeMin(DWORD dwRange);
	CString	FormatRangeMax(DWORD dwRange);

	CString FormatRanges();
	CString FormatParams(BOOL bTag);
	CString Format(BOOL bTag = FALSE);
	void	Scan(LPCSTR lpsz);
	void	Copy(CDynamicColor* lpSrc);

	void	Update(float fltData);
	void	Update(COverlay* lpOverlay);

	DWORD	m_dwColor;
	CString	m_strSource;
	DWORD	m_dwFlags;

	float	m_fltData;

	DYNAMIC_COLOR_DESC m_desc;

protected:
	LPCSTR	ScanDynamicColorTag(LPCSTR lpsz, DYNAMIC_COLOR_DESC& color, CString& strSource, DWORD& dwFlags);
	LPCSTR	ScanDynamicColorParams(LPCSTR lpsz, CString& strSource, DWORD& dwFlags);

	DWORD	CalcDynamicColor(float fltData, DYNAMIC_COLOR_DESC color, DWORD dwFlags);
	BOOL	IsInRange(float fltData, DYNAMIC_COLOR_DESC color, DWORD dwRange);
	BOOL	IsRangeClosed(DYNAMIC_COLOR_DESC color, DWORD dwRange);

	DWORD	Blend(DWORD dwColor0, DWORD dwColor1, float fltRatio);
};
/////////////////////////////////////////////////////////////////////////////
