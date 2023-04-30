// OverlayLayer.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "Overlay.h"
#include "OverlayLayer.h"
#include "HypertextExtension.h"
#include "DynamicColor.h"

#include "RTSSSharedMemoryInterface.h"
#include "RTSSSharedMemory.h"
/////////////////////////////////////////////////////////////////////////////
COverlayLayer::COverlayLayer()
{
	m_lpOverlay				= NULL;
	m_dwLayerID				= 0;

	m_strName				= "";
	m_strText				= "";
	m_nPositionX			= 0;
	m_nPositionY			= 0;
	m_nPositionSticky		= -1;
	m_nExtentX				= 0;
	m_nExtentY				= 0;
	m_nExtentOrigin			= 0;
	m_nAlignment			= DEFAULT_ALIGNMENT;
	m_nSize					= DEFAULT_SIZE;
	m_strTextColor			= "";
	m_strBgndColor			= "";
	m_strBorderColor		= "";
	m_nBorderSize			= 0;

	m_bSelected				= FALSE;

	m_rect					= CRect(0, 0, 0, 0);

	m_strTextFile			= "";
	m_nTextFileLine			= 0;

	m_dwRefreshPeriod		= 0;
	m_dwRefreshTimestamp	= 0;

	m_nMarginLeft			= 0;
	m_nMarginTop			= 0;
	m_nMarginRight			= 0;
	m_nMarginBottom			= 0;
}
/////////////////////////////////////////////////////////////////////////////
COverlayLayer::~COverlayLayer()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayLayer::GetHypertext(BOOL bOptimize, LPBYTE lpEmbeddedObjectBuffer, DWORD dwEmbeddedObjectBufferSize, DWORD& dwEmbeddedObjectOffset)
{
	CString strMarginTag;
	if (m_nMarginLeft || m_nMarginTop || m_nMarginRight || m_nMarginBottom)
		strMarginTag.Format("<M=%d,%d,%d,%d>", m_nMarginLeft, m_nMarginTop, m_nMarginRight, m_nMarginBottom);

	CString strPositionTag;
	if (m_nPositionSticky < 0)
		strPositionTag.Format("<P=%d,%d>", m_nPositionX, m_nPositionY);
	else
		strPositionTag.Format("<P%d>", m_nPositionSticky);
		//format position tag

	CString strExtentTag;
	if (m_nExtentX && m_nExtentY)
	{
		if (m_nExtentOrigin)
			strExtentTag.Format("<E=%d,%d,%d>", m_nExtentX, m_nExtentY, m_nExtentOrigin);
		else
			strExtentTag.Format("<E=%d,%d>", m_nExtentX, m_nExtentY);
	}
		//format extent tag

	CString strLayerTag;
	strLayerTag.Format("<L%d>", m_dwLayerID);
		//format layer ID tag

	CString strAlignmentTag;
	if (m_nAlignment != DEFAULT_ALIGNMENT)
	{
		if (bOptimize)
			strAlignmentTag = m_lpOverlay->GetAlignmentsLibrary()->FormatAttrTag(m_nAlignment, "A", FALSE);
		else
			strAlignmentTag = COverlayAttributesLibrary::FormatExplicitAttrTag(m_nAlignment, "A", FALSE);

		if (m_bFixedAlignment)
			strAlignmentTag += "\r";
				//alignment tag uses current character's font size, so in case of fixed alignment we simply embed invisible character with
				//default font size into hypertext immediately after alignment tag
	}
		//format alignment tag

	CString strSizeTag;
	if (m_nSize != DEFAULT_SIZE)
	{
		if (bOptimize)
			strSizeTag = m_lpOverlay->GetSizesLibrary()->FormatAttrTag(m_nSize, "S", FALSE);
		else
			strSizeTag = COverlayAttributesLibrary::FormatExplicitAttrTag(m_nSize, "S", FALSE);
	}
		//format size tag

	CDynamicColor dynamicTextColor;

	DWORD dwTextColor = GetDynamicTextColor(dynamicTextColor);

	CString strTextColorTag;

	if (dynamicTextColor.IsInternalSource() || 
		dynamicTextColor.IsEmbedded())
	{
		if (bOptimize)
			strTextColorTag = m_lpOverlay->GetColorsLibraryStr()->FormatAttrTag(dynamicTextColor.FormatRanges(), dynamicTextColor.FormatParams(TRUE), "C", m_lpOverlay->GetColorsLibrary()->GetSize());
		else
			strTextColorTag = COverlayAttributesLibraryStr::FormatExplicitAttrTag(dynamicTextColor.FormatRanges(), dynamicTextColor.FormatParams(TRUE), "C");
	}
	else
	{
		if (dwTextColor != DEFAULT_COLOR)
		{
			if (bOptimize)
				strTextColorTag = m_lpOverlay->GetColorsLibrary()->FormatAttrTag(dwTextColor, "C", TRUE);
			else
				strTextColorTag = COverlayAttributesLibrary::FormatExplicitAttrTag(dwTextColor, "C", TRUE);
		}
	}
		//format color tag

	CDynamicColor dynamicBgndColor;

	DWORD dwBgndColor = GetDynamicBgndColor(dynamicBgndColor);

	CString strBgndColorTag;

	if (dynamicBgndColor.IsInternalSource() ||
		dynamicBgndColor.IsEmbedded())
	{
		if (bOptimize)
			strBgndColorTag = m_lpOverlay->GetColorsLibraryStr()->FormatAttrTag(dynamicBgndColor.FormatRanges(), dynamicBgndColor.FormatParams(TRUE), "C", m_lpOverlay->GetColorsLibrary()->GetSize());
		else
			strBgndColorTag = COverlayAttributesLibraryStr::FormatExplicitAttrTag(dynamicBgndColor.FormatRanges(), dynamicBgndColor.FormatParams(TRUE), "C");

		strBgndColorTag += "<B=0,0>\b<C>";
	}
	else
	{
		if (dwBgndColor != DEFAULT_COLOR)
		{
			if (bOptimize)
				strBgndColorTag = m_lpOverlay->GetColorsLibrary()->FormatAttrTag(dwBgndColor, "C", TRUE);
			else
				strBgndColorTag = COverlayAttributesLibrary::FormatExplicitAttrTag(dwBgndColor, "C", TRUE);

			strBgndColorTag += "<B=0,0>\b<C>";
		}
	}
		//format bgnd color fill tag

	CDynamicColor dynamicBorderColor;

	DWORD dwBorderColor = GetDynamicBorderColor(dynamicBorderColor);

	CString strBorderColorTag;

	if (m_nBorderSize)
	{
		if (dynamicBorderColor.IsInternalSource() ||
			dynamicBorderColor.IsEmbedded())
		{
			if (bOptimize)
				strBorderColorTag = m_lpOverlay->GetColorsLibraryStr()->FormatAttrTag(dynamicBorderColor.FormatRanges(), dynamicBorderColor.FormatParams(TRUE), "C", m_lpOverlay->GetColorsLibrary()->GetSize());
			else
				strBorderColorTag = COverlayAttributesLibraryStr::FormatExplicitAttrTag(dynamicBorderColor.FormatRanges(), dynamicBorderColor.FormatParams(TRUE), "C");

			CString strBuf;
			strBuf.Format("<B=0,0,%d>\b<C>", m_nBorderSize);

			strBorderColorTag += strBuf;
		}
		else
		{
			if (dwBorderColor != DEFAULT_COLOR)
			{
				if (bOptimize)
					strBorderColorTag = m_lpOverlay->GetColorsLibrary()->FormatAttrTag(dwBorderColor, "C", TRUE);
				else
					strBorderColorTag = COverlayAttributesLibrary::FormatExplicitAttrTag(dwBorderColor, "C", TRUE);

				CString strBuf;
				strBuf.Format("<B=0,0,%d>\b<C>", m_nBorderSize);

				strBorderColorTag += strBuf;
			}
		}
	}
		//format border color fill tag

	CString	strResult;

	strResult = TranslateExtensionString(m_lpOverlay, m_strText, bOptimize, TRUE, lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, dwTextColor, m_nSize);

	if (strResult.IsEmpty())
		//if the layer contents are empty, its' attibutes (e.g. extent) can be applied to the next one, so we simply add space to flush attributes
		strResult += " ";

	ApplyTag(strResult, strTextColorTag	, "<C>");
	ApplyTag(strResult, strSizeTag		, "<S>");
	ApplyTag(strResult, strAlignmentTag	, "<A>");

	return strPositionTag + strMarginTag + strLayerTag  + strBgndColorTag + strBorderColorTag + strExtentTag + strResult;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::GetLayerID()
{
	return m_dwLayerID;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::SetLayerID(DWORD dwLayerID)
{
	m_dwLayerID = dwLayerID;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::Copy(COverlayLayer* lpSrc)
{
	m_strName			= lpSrc->m_strName;
	m_strText			= lpSrc->m_strText;
	m_nPositionX		= lpSrc->m_nPositionX;
	m_nPositionY		= lpSrc->m_nPositionY;
	m_nPositionSticky	= lpSrc->m_nPositionSticky;
	m_nExtentX			= lpSrc->m_nExtentX;
	m_nExtentY			= lpSrc->m_nExtentY;
	m_nExtentOrigin		= lpSrc->m_nExtentOrigin;
	m_nAlignment		= lpSrc->m_nAlignment;
	m_bFixedAlignment	= lpSrc->m_bFixedAlignment;
	m_nSize				= lpSrc->m_nSize;
	m_strTextColor		= lpSrc->m_strTextColor;
	m_strBgndColor		= lpSrc->m_strBgndColor;
	m_strBorderColor	= lpSrc->m_strBorderColor;
	m_nBorderSize		= lpSrc->m_nBorderSize;
	m_nMarginLeft		= lpSrc->m_nMarginLeft;
	m_nMarginTop		= lpSrc->m_nMarginTop;
	m_nMarginRight		= lpSrc->m_nMarginRight;
	m_nMarginBottom		= lpSrc->m_nMarginBottom;

	m_bSelected			= lpSrc->m_bSelected;

	m_strTextFile		= lpSrc->m_strTextFile;
	m_nTextFileLine		= lpSrc->m_nTextFileLine;

	POSITION pos = lpSrc->m_textFileLines.GetHeadPosition();
	while (pos)
		m_textFileLines.AddTail(lpSrc->m_textFileLines.GetNext(pos));

	m_dwRefreshPeriod	= lpSrc->m_dwRefreshPeriod;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayLayer::IsEqual(COverlayLayer* lpSrc)
{
	if (strcmp(m_strName, lpSrc->m_strName))
		return FALSE;
	if (strcmp(m_strText, lpSrc->m_strText))
		return FALSE;
	if (m_nPositionX != lpSrc->m_nPositionX)
		return FALSE;
	if (m_nPositionSticky != lpSrc->m_nPositionSticky)
		return FALSE;
	if (m_nPositionY != lpSrc->m_nPositionY)
		return FALSE;
	if (m_nExtentX != lpSrc->m_nExtentX)
		return FALSE;
	if (m_nExtentOrigin != lpSrc->m_nExtentOrigin)
		return FALSE;
	if (m_nExtentY != lpSrc->m_nExtentY)
		return FALSE;
	if (m_nAlignment != lpSrc->m_nAlignment)
		return FALSE;
	if (m_bFixedAlignment != lpSrc->m_bFixedAlignment)
		return FALSE;
	if (m_nSize != lpSrc->m_nSize)
		return FALSE;
	if (m_strTextColor != lpSrc->m_strTextColor)
		return FALSE;
	if (m_strBgndColor != lpSrc->m_strBgndColor)
		return FALSE;
	if (m_strBorderColor != lpSrc->m_strBorderColor)
		return FALSE;
	if (m_nBorderSize != lpSrc->m_nBorderSize)
		return FALSE;
	if (m_dwRefreshPeriod != lpSrc->m_dwRefreshPeriod)
		return FALSE;
	if (m_nMarginLeft != lpSrc->m_nMarginLeft)
		return FALSE;
	if (m_nMarginTop != lpSrc->m_nMarginTop)
		return FALSE;
	if (m_nMarginRight != lpSrc->m_nMarginRight)
		return FALSE;
	if (m_nMarginBottom != lpSrc->m_nMarginBottom)
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::ApplyTag(CString& str, CString strBeginTag, CString strEndTag)
{
	if (!strBeginTag.IsEmpty())
		str = strBeginTag + str + strEndTag;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::Load()
{
	if (m_lpOverlay)
	{
		CString strLayer;
		strLayer.Format("Layer%d", m_dwLayerID);

		m_strName			= m_lpOverlay->GetConfigStr(strLayer, "Name"			, ""				);
		m_strText			= m_lpOverlay->GetConfigStr(strLayer, "Text"			, ""				);
		m_nPositionX		= m_lpOverlay->GetConfigInt(strLayer, "PositionX"		, 0					);
		m_nPositionY		= m_lpOverlay->GetConfigInt(strLayer, "PositionY"		, 0					);
		m_nPositionSticky	= m_lpOverlay->GetConfigInt(strLayer, "PositionSticky"	, -1				);
		m_nExtentX			= m_lpOverlay->GetConfigInt(strLayer, "ExtentX"			, 0					);
		m_nExtentY			= m_lpOverlay->GetConfigInt(strLayer, "ExtentY"			, 0					);
		m_nExtentOrigin		= m_lpOverlay->GetConfigInt(strLayer, "ExtentOrigin"	, 0					);
		m_nAlignment		= m_lpOverlay->GetConfigInt(strLayer, "Alignment"		, DEFAULT_ALIGNMENT	);
		m_bFixedAlignment	= m_lpOverlay->GetConfigInt(strLayer, "FixedAlignment"	, 0					) != 0;
		m_nSize				= m_lpOverlay->GetConfigInt(strLayer, "Size"			, DEFAULT_SIZE		);
		m_strTextColor		= m_lpOverlay->GetConfigStr(strLayer, "TextColor"		, ""				);
		m_strBgndColor		= m_lpOverlay->GetConfigStr(strLayer, "BgndColor"		, ""				);
		m_strBorderColor	= m_lpOverlay->GetConfigStr(strLayer, "BorderColor"		, ""				);
		m_nBorderSize		= m_lpOverlay->GetConfigInt(strLayer, "BorderSize"		, 0					);
		m_dwRefreshPeriod	= m_lpOverlay->GetConfigInt(strLayer, "RefreshPeriod"	, 0					);
		m_nMarginLeft		= m_lpOverlay->GetConfigInt(strLayer, "MarginLeft"		, 0					);
		m_nMarginTop		= m_lpOverlay->GetConfigInt(strLayer, "MarginTop"		, 0					);
		m_nMarginRight		= m_lpOverlay->GetConfigInt(strLayer, "MarginRight"		, 0					);
		m_nMarginBottom		= m_lpOverlay->GetConfigInt(strLayer, "MarginBottom"	, 0					);

		m_strText			= DecodeSpecialChars(m_strText);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::Save()
{
	if (m_lpOverlay)
	{
		CString strLayer;
		strLayer.Format("Layer%d", m_dwLayerID);

		m_lpOverlay->SetConfigStr(strLayer, "Name"				, m_strName						);
		m_lpOverlay->SetConfigStr(strLayer, "Text"				, EncodeSpecialChars(m_strText)	);
		if (m_nPositionSticky < 0)
		{
			m_lpOverlay->SetConfigInt(strLayer, "PositionX"		, m_nPositionX					);
			m_lpOverlay->SetConfigInt(strLayer, "PositionY"		, m_nPositionY					);
		}
		else
			m_lpOverlay->SetConfigInt(strLayer, "PositionSticky", m_nPositionSticky				);
		m_lpOverlay->SetConfigInt(strLayer, "ExtentX"			, m_nExtentX					);
		m_lpOverlay->SetConfigInt(strLayer, "ExtentY"			, m_nExtentY					);
		m_lpOverlay->SetConfigInt(strLayer, "ExtentOrigin"		, m_nExtentOrigin				);

		if (m_nAlignment != DEFAULT_ALIGNMENT)
			m_lpOverlay->SetConfigInt(strLayer, "Alignment"		, m_nAlignment					);
		if (m_bFixedAlignment)
			m_lpOverlay->SetConfigInt(strLayer, "FixedAlignment", m_bFixedAlignment ? 1 : 0		);
		if (m_nSize != DEFAULT_SIZE)
			m_lpOverlay->SetConfigInt(strLayer, "Size"			, m_nSize						);
		if (strlen(m_strTextColor))
			m_lpOverlay->SetConfigStr(strLayer, "TextColor"		, m_strTextColor				);
		if (strlen(m_strBgndColor))
			m_lpOverlay->SetConfigStr(strLayer, "BgndColor"		, m_strBgndColor				);
		if (strlen(m_strBorderColor))
		{
			m_lpOverlay->SetConfigStr(strLayer, "BorderColor"	, m_strBorderColor				);

			if (m_nBorderSize)
				m_lpOverlay->SetConfigInt(strLayer, "BorderSize", m_nBorderSize					);
		}

		if (m_dwRefreshPeriod)
			m_lpOverlay->SetConfigInt(strLayer, "RefreshPeriod"	, m_dwRefreshPeriod				);

		if (m_nMarginLeft)
			m_lpOverlay->SetConfigInt(strLayer, "MarginLeft"	, m_nMarginLeft					);
		if (m_nMarginTop)
			m_lpOverlay->SetConfigInt(strLayer, "MarginTop"		, m_nMarginTop					);
		if (m_nMarginRight)
			m_lpOverlay->SetConfigInt(strLayer, "MarginRight"	, m_nMarginRight				);
		if (m_nMarginBottom)
			m_lpOverlay->SetConfigInt(strLayer, "MarginBottom"	, m_nMarginBottom				);
	}
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::SwapRGB(DWORD dwColor)
{
	return (0xFF & (dwColor>>16)) | (dwColor & 0xFF00) | ((dwColor & 0xFF)<<16);
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::UnpackRGBA(DWORD dwColor)
{
	if (dwColor & 0xFF000000)
		return dwColor;

	return dwColor | 0xFF000000;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::UnpackRGBA(LPCSTR lpColor)
{
	DWORD dwColor;

	if (sscanf_s(lpColor, "%08X", &dwColor) == 1)
		return UnpackRGBA(dwColor);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::PackRGBA(DWORD dwColor)
{
	if ((dwColor & 0xFF000000) == 0xFF000000)
		return dwColor & 0xFFFFFF;

	return dwColor;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayLayer::DecodeSpecialChars(CString str)
{
	str.Replace("\\t", "\t");
	str.Replace("\\r", "\r");
	str.Replace("\\n", "\n");
	str.Replace("\\b", "\b");

	return str;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayLayer::EncodeSpecialChars(CString str)
{
	str.Replace("\t", "\\t");
	str.Replace("\r", "\\r");
	str.Replace("\n", "\\n");
	str.Replace("\b", "\\b");

	return str;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::HitTest(POINT pt)
{
	DWORD dwMask = 0;

	CRect rect = m_rect;

	if (rect.PtInRect(pt))
	{
		dwMask = HIT_TEST_MASK_BODY;

		if (abs(pt.x - rect.left) < HIT_TEST_SIZING_BORDER)
			dwMask = dwMask | HIT_TEST_MASK_LEFT;

		if (abs(pt.x - rect.right) < HIT_TEST_SIZING_BORDER)
			dwMask = dwMask | HIT_TEST_MASK_RIGHT;

		if (abs(pt.y - rect.top) < HIT_TEST_SIZING_BORDER)
			dwMask = dwMask | HIT_TEST_MASK_TOP;

		if (abs(pt.y - rect.bottom) < HIT_TEST_SIZING_BORDER)
			dwMask = dwMask | HIT_TEST_MASK_BOTTOM;

		return dwMask;
	}

	rect.InflateRect(HIT_TEST_SIZING_BORDER, HIT_TEST_SIZING_BORDER);

	if (rect.PtInRect(pt))
	{
		dwMask = 0;

		if (abs(pt.x - rect.left) < HIT_TEST_SIZING_BORDER)
			dwMask = dwMask | HIT_TEST_MASK_LEFT;

		if (abs(pt.x - rect.right) < HIT_TEST_SIZING_BORDER)
			dwMask = dwMask | HIT_TEST_MASK_RIGHT;

		if (abs(pt.y - rect.top) < HIT_TEST_SIZING_BORDER)
			dwMask = dwMask | HIT_TEST_MASK_TOP;

		if (abs(pt.y - rect.bottom) < HIT_TEST_SIZING_BORDER)
			dwMask = dwMask | HIT_TEST_MASK_BOTTOM;

		return dwMask;
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayLayer::GetTextFile()
{
	return m_strTextFile;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayLayer::GetTextFromFile(LPCSTR lpFile)
{
	if (strcmp(m_strTextFile, lpFile))
	{
		m_strTextFile = lpFile;

		LoadTextFile(TRUE);
	}

	POSITION pos = m_textFileLines.FindIndex(m_nTextFileLine);

	if (pos)
		return DecodeSpecialChars(m_textFileLines.GetAt(pos));

	return "";
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::LoadTextFile(BOOL bResetLine)
{
	m_textFileLines.RemoveAll();

	if (!m_strTextFile.IsEmpty())
	{
		CStdioFile file;	

		if (file.Open(m_lpOverlay->GetCfgFolder() + m_strTextFile, CFile::modeRead|CFile::shareDenyWrite))
		{
			CString strLine;

			while (file.ReadString(strLine))
				m_textFileLines.AddTail(strLine);
		}
	}

	if (bResetLine || (m_nTextFileLine > m_textFileLines.GetCount() - 1))
		m_nTextFileLine = 0;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::ViewTextFile()
{
	if (!m_strTextFile.IsEmpty())
	{
		ShellExecute(NULL, "open", m_lpOverlay->GetCfgFolder() + m_strTextFile, NULL, NULL, SW_SHOWNORMAL );
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::MoveToNextTextFileLine()
{
	int nLines = m_textFileLines.GetCount();

	if (nLines)
	{
		m_nTextFileLine++;

		if (m_nTextFileLine > nLines - 1)
			m_nTextFileLine = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayLayer::MoveToPrevTextFileLine()
{
	int nLines = m_textFileLines.GetCount();

	if (nLines)
	{
		m_nTextFileLine--;

		if (m_nTextFileLine < 0)
			m_nTextFileLine = nLines - 1;
	}
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::GetDynamicColor(CString strColor, CDynamicColor& color)
{
	if (strlen(strColor))
	{
		color.Scan(strColor);
		color.Update(m_lpOverlay);
	}
	else
		color.SetStatic(DEFAULT_COLOR);

	return color.m_dwColor;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::GetDynamicTextColor(CDynamicColor& color)
{
	return GetDynamicColor(m_strTextColor, color);
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::GetDynamicBgndColor(CDynamicColor& color)
{
	return GetDynamicColor(m_strBgndColor, color);
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayLayer::GetDynamicBorderColor(CDynamicColor& color)
{
	return GetDynamicColor(m_strBorderColor, color);
}
/////////////////////////////////////////////////////////////////////////////
