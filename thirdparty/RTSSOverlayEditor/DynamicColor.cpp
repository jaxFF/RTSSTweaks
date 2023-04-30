// DynamicColor.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "TokenString.h"
#include "DynamicColor.h"
#include "Overlay.h"
#include "OverlayDataSourceInternal.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
CDynamicColor::CDynamicColor()
{
	m_dwColor	= 0;
	m_strSource	= "";
	m_dwFlags	= 0;

	m_fltData	= FLT_MAX;

	ZeroMemory(&m_desc, sizeof(m_desc));
}
/////////////////////////////////////////////////////////////////////////////
CDynamicColor::~CDynamicColor()
{
	Destroy();
}
/////////////////////////////////////////////////////////////////////////////
DWORD CDynamicColor::GetRangeCount()
{
	return m_desc.dwCount;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDynamicColor::IsInternalSource()
{
	if (GetRangeCount())
	{
		if (!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE)						|| 
			!strcmp(m_strSource, INTERNAL_SOURCE_FRAMETIME)						||
			!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_MIN)					||
			!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_AVG)					||
			!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_MAX)					||
			!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW)	||
			!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW))
			return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDynamicColor::IsEmbedded()
{
	return (m_dwFlags & DYNAMIC_COLOR_FLAGS_EMBED) != 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColor::Destroy()
{
	m_dwColor	= 0;
	m_strSource	= "";
	m_dwFlags	= 0;

	ZeroMemory(&m_desc, sizeof(m_desc));
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColor::SetStatic(DWORD dwColor)
{
	Destroy();

	m_dwColor = dwColor;
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColor::AddRange(DWORD dwColor, LPCSTR lpMin, LPCSTR lpMax)
{
	if (m_desc.dwCount < DYNAMIC_COLOR_RANGES_MAX)
	{
		m_desc.ranges[m_desc.dwCount].dwColor = dwColor;

		if (sscanf_s(lpMin, "%f", &m_desc.ranges[m_desc.dwCount].fltMin) != 1)
			m_desc.ranges[m_desc.dwCount].fltMin = FLT_MAX;

		if (sscanf_s(lpMax, "%f", &m_desc.ranges[m_desc.dwCount].fltMax) != 1)
			m_desc.ranges[m_desc.dwCount].fltMax = FLT_MAX;

		m_desc.dwCount++;
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CDynamicColor::Format(BOOL bTag)
{
	return FormatRanges() + FormatParams(bTag);
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColor::Scan(LPCSTR lpsz)
{
	Destroy();

	lpsz = ScanHex(lpsz, m_dwColor);
	if (!lpsz)
		return;

	if (*lpsz == ',')
	{
		m_desc.ranges[0].dwColor = m_dwColor;

		lpsz = ScanDynamicColorTag(lpsz, m_desc, m_strSource, m_dwFlags);
	}

	 for (DWORD dwRange=0; dwRange<m_desc.dwCount; dwRange++)
		 m_desc.ranges[dwRange].dwColor = COverlayLayer::UnpackRGBA(m_desc.ranges[dwRange].dwColor);
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColor::Update(float fltData)
{
	m_fltData = fltData;

	m_dwColor = CalcDynamicColor(fltData, m_desc, m_dwFlags);
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColor::Update(COverlay* lpOverlay)
{
	if (lpOverlay)
	{
		if (!m_strSource.IsEmpty())
		{
			COverlayDataSource* lpDataSource = lpOverlay->FindDataSource(m_strSource);

			if (lpDataSource)
				Update(lpDataSource->GetData());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColor::Copy(CDynamicColor* lpSrc)
{
	Destroy();

	m_dwColor	= lpSrc->m_dwColor;
	m_strSource	= lpSrc->m_strSource;
	m_dwFlags	= lpSrc->m_dwFlags;

	CopyMemory(&m_desc, &lpSrc->m_desc, sizeof(m_desc));
}
/////////////////////////////////////////////////////////////////////////////
void CDynamicColor::GetRange(float& fltMin, float& fltMax)
{
	fltMin = FLT_MAX;
	fltMax = FLT_MAX;

	for (DWORD dwRange=0; dwRange<m_desc.dwCount; dwRange++)
	{
		float fltRangeMin = m_desc.ranges[dwRange].fltMin;

		if (fltRangeMin != FLT_MAX)
		{
			if (fltMin == FLT_MAX)
				fltMin = fltRangeMin;
			else
				if (fltMin > fltRangeMin)
					fltMin = fltRangeMin;

			if (fltMax == FLT_MAX)
				fltMax = fltRangeMin;
			else
				if (fltMax < fltRangeMin)
					fltMax = fltRangeMin;
		}

		float fltRangeMax = m_desc.ranges[dwRange].fltMax;

		if (fltRangeMax != FLT_MAX)
		{
			if (fltMin == FLT_MAX)
				fltMin = fltRangeMax;
			else
				if (fltMin > fltRangeMax)
					fltMin = fltRangeMax;

			if (fltMax == FLT_MAX)
				fltMax = fltRangeMax;
			else
				if (fltMax < fltRangeMax)
					fltMax = fltRangeMax;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CString CDynamicColor::FormatRanges()
{
	CString strResult;

	if (m_desc.dwCount)
	{
		for (DWORD dwRange=0; dwRange<m_desc.dwCount; dwRange++)
		{
			CString strRange;
			strRange.Format("%X,%s,%s", COverlayLayer::PackRGBA(m_desc.ranges[dwRange].dwColor), FormatRangeMin(dwRange), FormatRangeMax(dwRange));

			if (!strResult.IsEmpty())
				strResult += ",";
			strResult += strRange;
		}
	}
	else
		strResult.Format("%X", COverlayLayer::PackRGBA(m_dwColor));

	return strResult;
}
/////////////////////////////////////////////////////////////////////////////
CString CDynamicColor::FormatParams(BOOL bTag)
{
	if (m_desc.dwCount)
	{
		CString strParams;

		if (bTag)
		{
			CString strParam;

			if (!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE))
				strParam = "FR";
			else
			if (!strcmp(m_strSource, INTERNAL_SOURCE_FRAMETIME))
				strParam = "FT";
			else
			if (!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_MIN))
				strParam = "FRMIN";
			else
			if (!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_AVG))
				strParam = "FRAVG";
			else
			if (!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_MAX))
				strParam = "FRMAX";
			else
			if (!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW))
				strParam = "FR10L";
			else
			if (!strcmp(m_strSource, INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW))
				strParam = "FR01L";
			else
			{
				if (m_fltData != FLT_MAX)
					strParam = COverlayDataSource::FormatFloat(m_fltData);
			}

			strParams.Format("%s,%X", strParam, m_dwFlags);
		}
		else
			strParams.Format("%s,%X", m_strSource, m_dwFlags);

		return "(" + strParams + ")";
	}

	return "";
}
/////////////////////////////////////////////////////////////////////////////
CString	CDynamicColor::FormatRangeMin(DWORD dwRange)
{
	CString strResult;

	if (m_desc.ranges[dwRange].fltMin != FLT_MAX)
		strResult = COverlayDataSource::FormatFloat(m_desc.ranges[dwRange].fltMin);

	return strResult;
}
/////////////////////////////////////////////////////////////////////////////
CString	CDynamicColor::FormatRangeMax(DWORD dwRange)
{
	CString strResult;

	if (m_desc.ranges[dwRange].fltMax != FLT_MAX)
		strResult = COverlayDataSource::FormatFloat(m_desc.ranges[dwRange].fltMax);

	return strResult;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CDynamicColor::Blend(DWORD dwColor0, DWORD dwColor1, float fltRatio)
{
	LONG r0	= 0xFF & (dwColor0);
	LONG g0	= 0xFF & (dwColor0>>8);
	LONG b0	= 0xFF & (dwColor0>>16);
	LONG a0 = 0xFF & (dwColor0>>24);

	LONG r1	= 0xFF & (dwColor1);
	LONG g1	= 0xFF & (dwColor1>>8);
	LONG b1	= 0xFF & (dwColor1>>16);
	LONG a1	= 0xFF & (dwColor1>>24);

	LONG r	= min(255, (DWORD)(r0 + (r1 - r0) * fltRatio));
	LONG g	= min(255, (DWORD)(g0 + (g1 - g0) * fltRatio));
	LONG b	= min(255, (DWORD)(b0 + (b1 - b0) * fltRatio));
	LONG a	= min(255, (DWORD)(a0 + (a1 - a0) * fltRatio));

	return (r | (g<<8) | (b<<16) | (a<<24));
}
//////////////////////////////////////////////////////////////////////
BOOL CDynamicColor::IsRangeClosed(DYNAMIC_COLOR_DESC color, DWORD dwRange)
{
	return ((color.ranges[dwRange].fltMin != FLT_MAX) && (color.ranges[dwRange].fltMax != FLT_MAX));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CDynamicColor::IsInRange(float fltData, DYNAMIC_COLOR_DESC color, DWORD dwRange)
{
	if ((color.ranges[dwRange].fltMin != FLT_MAX) && (fltData <  color.ranges[dwRange].fltMin))
		return FALSE;

	if ((color.ranges[dwRange].fltMax != FLT_MAX) && (fltData >= color.ranges[dwRange].fltMax))
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CDynamicColor::CalcDynamicColor(float fltData, DYNAMIC_COLOR_DESC color, DWORD dwFlags)
{
	if (!color.dwCount)
		return color.ranges[0].dwColor;

	if (fltData != FLT_MAX)
	{
		for (DWORD dwRange=0; dwRange<color.dwCount; dwRange++)
		{
			if (IsInRange(fltData, color, dwRange))
			{
				if (dwFlags & DYNAMIC_COLOR_FLAGS_BLEND)
				{
					if (IsRangeClosed(color, dwRange))
					{
						if (dwRange < color.dwCount - 1)
						{
							if (color.ranges[dwRange].fltMax == color.ranges[dwRange + 1].fltMin)
							{
								float fltRatio = (fltData - color.ranges[dwRange].fltMin) / (color.ranges[dwRange].fltMax - color.ranges[dwRange].fltMin);

								return Blend(color.ranges[dwRange].dwColor, color.ranges[dwRange + 1].dwColor, fltRatio);
							}
						}
					}
				}

				return color.ranges[dwRange].dwColor;
			}
		}
	}

	return DEFAULT_COLOR;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CDynamicColor::ScanDynamicColorParams(LPCSTR lpsz, CString& strSource, DWORD& dwFlags)
{
	lpsz = ScanStr(lpsz, strSource);
	if (!lpsz)
		return NULL;

	if (*lpsz == ',')
		lpsz = ScanHex(lpsz + 1, dwFlags);
	if (!lpsz)
		return NULL;

	if (*lpsz != ')')
		return NULL;

	return lpsz + 1;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CDynamicColor::ScanDynamicColorTag(LPCSTR lpsz, DYNAMIC_COLOR_DESC& color, CString& strSource, DWORD& dwFlags)
{
	color.dwCount = 0;

	while (color.dwCount < DYNAMIC_COLOR_RANGES_MAX)
	{
		if (color.dwCount)
		{
			if (*lpsz == ',')
				lpsz = ScanHex(lpsz + 1, color.ranges[color.dwCount].dwColor);
			if (!lpsz)
				return NULL;
		}

		if (*lpsz == ',')
		{
			if (IsEmptyTagParam(lpsz + 1))
			{
				color.ranges[color.dwCount].fltMin = FLT_MAX;
				lpsz++;
			}
			else
				lpsz = ScanFlt(lpsz + 1, color.ranges[color.dwCount].fltMin);
		}
		if (!lpsz)
			return NULL;

		if (*lpsz == ',')
		{
			if (IsEmptyTagParam(lpsz + 1))
			{
				color.ranges[color.dwCount].fltMax = FLT_MAX;
				lpsz++;
			}
			else
				lpsz = ScanFlt(lpsz + 1, color.ranges[color.dwCount].fltMax);
		}
		if (!lpsz)
			return NULL;

		color.dwCount++;

		if (*lpsz == 0)
			return lpsz;

		if (*lpsz == '(')
			return ScanDynamicColorParams(lpsz + 1, strSource, dwFlags);
	}

	return lpsz;
}
//////////////////////////////////////////////////////////////////////
