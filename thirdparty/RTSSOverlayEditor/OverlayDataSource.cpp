// OverlayDataSource.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataSource.h"
#include "OverlayEditor.h"
#include "Overlay.h"
#include "RTFCErrors.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataSource::COverlayDataSource()
{
	m_lpOverlay			= NULL;
	m_lpDataSourcesList	= NULL;
	m_lpProvider		= NULL;
	m_dwSourceID		= 0;

	m_strName			= "";
	m_strUnits			= "";
	m_strFormat			= "";
	m_bIntFormat		= FALSE;
	m_strFormula		= "";
	m_fltData			= FLT_MAX;
	m_fltFormulaData	= FLT_MAX;

	for (int i=0; i<RING_BUFFER_SIZE; i++)
		m_fltBuffer[i]= FLT_MAX;
	m_dwBufferPos		= 0;

	m_translator.SetVariableProvider(this);
	m_bValidFormula		= FALSE;

	m_nPriority			= INT_MAX;

	m_bDirty			= FALSE;

	m_bSelected			= FALSE;

	m_dwStatCount		= 0;
	
	for (DWORD dwPos=0; dwPos<STAT_BUFFER_SIZE; dwPos++)
		m_fltStatBuf[dwPos] = FLT_MAX;

	m_fltStatMin		= FLT_MAX;
	m_fltStatAvg		= FLT_MAX;
	m_fltStatMax		= FLT_MAX;
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSource::~COverlayDataSource()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
COverlay* COverlayDataSource::GetOverlay()
{
	return m_lpOverlay;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetDataSourcesList(COverlayDataSourcesList* lpDataSourcesList)
{
	m_lpDataSourcesList = lpDataSourcesList;
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourcesList* COverlayDataSource::GetDataSourcesList()
{
	return m_lpDataSourcesList;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayDataSource::GetSourceID()
{
	return m_dwSourceID;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetSourceID(DWORD dwSourceID)
{
	m_dwSourceID = dwSourceID;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSource::GetName()
{
	return m_strName;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSource::GetUnits()
{
	return m_strUnits;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSource::GetFormat()
{
	return m_strFormat;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::IsIntFormat()
{
	return m_bIntFormat;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSource::GetFormula()
{
	return m_strFormula;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetName(LPCSTR lpName)
{
	m_strName = lpName;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetUnits(LPCSTR lpUnits)
{
	m_strUnits = lpUnits;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetFormat(LPCSTR lpFormat)
{
	if (ValidateFloatFormat(lpFormat))
	{
		m_strFormat		= lpFormat;
		m_bIntFormat	= FALSE;
	}
	else
	if (ValidateIntFormat(lpFormat))
	{
		m_strFormat		= lpFormat;
		m_bIntFormat	= TRUE;
	}
	else
		m_strFormat = "";
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetFormula(LPCSTR lpFormula)
{
	if (lpFormula)
		m_strFormula = lpFormula;

	m_bValidFormula = FALSE;
		//reset formula validation flag to allow GetVariable perform variable validation

	if (!m_strFormula.IsEmpty())
	{
		if (SUCCEEDED(m_translator.Translate(m_strFormula)))
		{
			if (SUCCEEDED(m_translator.Calc()))
				m_bValidFormula = TRUE;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::IsValidFormula()
{
	return m_bValidFormula;
}
/////////////////////////////////////////////////////////////////////////////
float COverlayDataSource::GetData()
{
	return m_fltData;
}
/////////////////////////////////////////////////////////////////////////////
float COverlayDataSource::GetFormulaData()
{
	return m_fltFormulaData;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSource::GetDataStr()
{
	float data = GetData();	

	CString strResult;
	if (data != FLT_MAX)
	{
		CString strFormat = GetFormat();

		if (strlen(strFormat))
		{
			if (IsIntFormat())
				strResult.Format(strFormat, (int)data);
			else
				strResult.Format(strFormat, data);
		}
		else
			strResult.Format("%.0f", data);
	}

	return strResult;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSource::GetDataTag()
{
	return "";
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetData(float fltData)
{
	CalcStat(fltData);
		//calculate statistics on raw values before applying correction formula

	if (!m_strFormula.IsEmpty() && m_bValidFormula)
	{
		SetFormulaData(fltData);

		if (SUCCEEDED(m_translator.Calc()))
			fltData = m_translator.GetFltValue();
	}

	m_fltData					= fltData;
	m_fltBuffer[m_dwBufferPos]	= fltData;

	m_dwBufferPos = (m_dwBufferPos + 1) & (RING_BUFFER_SIZE - 1);
		//modify ring buffer position (assuming that RING_BUFFER_SIZE is a power of 2!)

	SetDirty(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetFormulaData(float fltFormulaData)
{
	m_fltFormulaData = fltFormulaData;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::Save()
{
	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_lpOverlay->SetConfigStr(strSource, "Name"		, m_strName			);
		m_lpOverlay->SetConfigStr(strSource, "Units"	, m_strUnits		);
		m_lpOverlay->SetConfigStr(strSource, "Format"	, m_strFormat		);
		m_lpOverlay->SetConfigStr(strSource, "Formula"	, m_strFormula		);


		m_lpOverlay->SetConfigStr(strSource, "Provider"	, GetProviderName()	);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::Load()
{
	if (m_lpOverlay)
	{
		CString strSource;
		strSource.Format("Source%d", m_dwSourceID);

		m_strName			= m_lpOverlay->GetConfigStr(strSource, "Name"			, ""				);
		m_strUnits			= m_lpOverlay->GetConfigStr(strSource, "Units"			, ""				);
		CString strFormat	= m_lpOverlay->GetConfigStr(strSource, "Format"			, ""				);
		SetFormat(strFormat);
		CString strFormula	= m_lpOverlay->GetConfigStr(strSource, "Formula"		, ""				);
		SetFormula(strFormula);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::Copy(COverlayDataSource* lpSrc)
{
	m_strName		= lpSrc->m_strName;
	m_strUnits		= lpSrc->m_strUnits;
	m_strFormat		= lpSrc->m_strFormat;
	m_bIntFormat	= lpSrc->m_bIntFormat;

	SetFormula(lpSrc->GetFormula());

	m_fltData		= lpSrc->m_fltData;

	memcpy(m_fltBuffer, lpSrc->m_fltBuffer, sizeof(m_fltBuffer));
	m_dwBufferPos	= lpSrc->m_dwBufferPos;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::IsEqual(COverlayDataSource* lpSrc)
{
	if (strcmp(GetProviderName(), lpSrc->GetProviderName()))
		return FALSE;

	if (strcmp(m_strName, lpSrc->m_strName))
		return FALSE;
	if (strcmp(m_strUnits, lpSrc->m_strUnits))
		return FALSE;
	if (strcmp(m_strFormat, lpSrc->m_strFormat))
		return FALSE;
	if (strcmp(m_strFormula, lpSrc->m_strFormula))
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
float* COverlayDataSource::GetBuffer()
{
	return m_fltBuffer;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayDataSource::GetBufferPos()
{
	return m_dwBufferPos;
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataProvider* COverlayDataSource::GetProvider()
{
	return m_lpProvider;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetProvider(COverlayDataProvider* lpProvider)
{
	m_lpProvider = lpProvider;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::GetVariable(LPCSTR lpName, LPFORMULATRANSLATOR_VALUE_DESC pDesc)
{
	float data = GetFormulaData();

	if (!(_stricmp(lpName, "x")))
	{
		if ((data != FLT_MAX) || !m_bValidFormula)
			//if m_bValidFormula is not set then we're performing variable validation 
		{
			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
			pDesc->dwData = FLOAT_AS_LONG(data);

			return TRUE;
		}
	}
	else
	if (!(_stricmp(lpName, "xmin")))
	{
		float stat = GetStatMin();

		if ((stat != FLT_MAX) || !m_bValidFormula)
			//if m_bValidFormula is not set then we're performing variable validation 
		{
			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
			pDesc->dwData = FLOAT_AS_LONG(stat);

			return TRUE;
		}
	}
	else
	if (!(_stricmp(lpName, "xavg")))
	{
		float stat = GetStatAvg();

		if ((stat != FLT_MAX) || !m_bValidFormula)
			//if m_bValidFormula is not set then we're performing variable validation 
		{
			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
			pDesc->dwData = FLOAT_AS_LONG(stat);

			return TRUE;
		}
	}
	else
	if (!(_stricmp(lpName, "xmax")))
	{
		float stat = GetStatMax();

		if ((stat != FLT_MAX) || !m_bValidFormula)
			//if m_bValidFormula is not set then we're performing variable validation 
		{
			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
			pDesc->dwData = FLOAT_AS_LONG(stat);

			return TRUE;
		}
	}
	else
	if (!(_stricmp(lpName, "xswa")))
	{
		float stat = GetStatSwa();

		if ((stat != FLT_MAX) || !m_bValidFormula)
			//if m_bValidFormula is not set then we're performing variable validation 
		{
			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
			pDesc->dwData = FLOAT_AS_LONG(stat);

			return TRUE;
		}
	}
	else
	{
		if (m_lpDataSourcesList)
		{
			if (!m_bValidFormula)
				//if m_bValidFormula is not set then we're performing variable validation 
			{
				if (m_lpDataSourcesList->FindSource(lpName))
					return TRUE;
			}
			else
			{
				COverlayDataSource* pSource = m_lpDataSourcesList->FindSource(lpName);

				if (pSource)
				{
					data = pSource->GetData();

					pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
					pDesc->dwData = FLOAT_AS_LONG(data);

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::ValidateFloatFormat(LPCSTR lpLine)
{
	BOOL bValid = FALSE;

	if (*lpLine == '%')
	{
		lpLine++;

		if (*lpLine == 'f')
		{
			lpLine++;
			bValid = TRUE;
		}
		else
		{
			if ((*lpLine == '+') || (*lpLine == '-'))
				lpLine++;

			while ((*lpLine >= '0') && (*lpLine <= '9'))
			{
				lpLine++;
				bValid = TRUE;
			}

			if (*lpLine == '.')
			{
				lpLine++;

				bValid = FALSE;

				while ((*lpLine >= '0') && (*lpLine <= '9'))
				{
					lpLine++;
					bValid = TRUE;
				}
			}

			if (*lpLine == 'f')
				lpLine++;
			else
				bValid = FALSE;
		}
	}

	if (*lpLine)
		return FALSE;

	return bValid;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::ValidateIntFormat(LPCSTR lpLine)
{
	BOOL bValid = FALSE;

	if (*lpLine == '%')
	{
		lpLine++;

		if (*lpLine == 'd') 
		{
			lpLine++;
			bValid = TRUE;
		}
		else
		{
			if ((*lpLine == '+') || (*lpLine == '-'))
				lpLine++;

			while ((*lpLine >= '0') && (*lpLine <= '9'))
			{
				lpLine++;
				bValid = TRUE;
			}

			if (*lpLine == 'd') 
				lpLine++;
			else
				bValid = FALSE;
		}
	}

	if (*lpLine)
		return FALSE;

	return bValid;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::ValidateFormat(LPCSTR lpLine)
{
	return ValidateFloatFormat(lpLine) || ValidateIntFormat(lpLine);
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::ValidateFloat(LPCSTR lpLine)
{
	BOOL bValid = FALSE;

	if ((*lpLine == '+') || (*lpLine == '-'))
		lpLine++;

	while ((*lpLine >= '0') && (*lpLine <= '9'))
	{
		lpLine++;
		bValid = TRUE;
	}

	if (*lpLine == '.')
	{
		lpLine++;

		bValid = FALSE;

		while ((*lpLine >= '0') && (*lpLine <= '9'))
		{
			lpLine++;
			bValid = TRUE;
		}
	}

	if (*lpLine)
		return FALSE;

	return bValid;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSource::FormatFloat(float fltVal, int nPrecision)
{
	char szBuf[MAX_PATH];
	sprintf_s(szBuf, sizeof(szBuf), "%.*f", nPrecision, fltVal);

	if (nPrecision)
	{
		int nLen = strlen(szBuf);

		if (nLen)
		{
			int nPos = nLen - 1;

			while (szBuf[nPos] == '0' && nPos)
				nPos--;
			if (szBuf[nPos] == '.' && nPos)
				nPos--;

			szBuf[nPos + 1] = 0;
		}
	}

	return szBuf;
}
/////////////////////////////////////////////////////////////////////////////
int	COverlayDataSource::GetPriority()
{
	return m_nPriority;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetPriority(int nPriority)
{
	m_nPriority = nPriority;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetDataWithPriority(float fltData, int nPriority)
{
	if (nPriority < GetPriority())
	{
		SetData(fltData);

		SetPriority(nPriority);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSource::IsDirty()
{
	return m_bDirty;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::SetDirty(BOOL bDirty)
{
	m_bDirty = bDirty;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::CalcStat(float fltData)
{
	if (fltData != FLT_MAX)
	{
		if (m_dwStatCount)
		{
			if (m_fltStatMin > fltData)
				m_fltStatMin = fltData;

			m_fltStatAvg += fltData;

			if (m_fltStatMax < fltData)
				m_fltStatMax = fltData;
		}
		else
		{
			m_fltStatMin = fltData;
			m_fltStatAvg = fltData;
			m_fltStatMax = fltData;
		}

		m_fltStatBuf[m_dwStatCount & (STAT_BUFFER_SIZE - 1)] = fltData;

		m_dwStatCount++;
	}
}
/////////////////////////////////////////////////////////////////////////////
float COverlayDataSource::GetStatMin()
{
	if (m_dwStatCount)
		return m_fltStatMin;

	return FLT_MAX;
}
/////////////////////////////////////////////////////////////////////////////
float COverlayDataSource::GetStatAvg()
{
	if (m_dwStatCount)
		return m_fltStatAvg / m_dwStatCount;

	return FLT_MAX;
}
/////////////////////////////////////////////////////////////////////////////
float COverlayDataSource::GetStatMax()
{
	if (m_dwStatCount)
		return m_fltStatMax;

	return FLT_MAX;
}
/////////////////////////////////////////////////////////////////////////////
float COverlayDataSource::GetStatSwa()
{
	if (m_dwStatCount)
	{
		float fltResult	= 0.0f;

		DWORD dwCount	= min(m_dwStatCount, STAT_BUFFER_SIZE);

		for (DWORD dwPos=0; dwPos<dwCount; dwPos++)
			fltResult += m_fltStatBuf[dwPos];

		return fltResult / dwCount;
	}

	return FLT_MAX;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSource::ResetStat()
{
	m_dwStatCount		= 0;

	for (DWORD dwPos=0; dwPos<STAT_BUFFER_SIZE; dwPos++)
		m_fltStatBuf[dwPos] = FLT_MAX;

	m_fltStatMin		= FLT_MAX;
	m_fltStatAvg		= FLT_MAX;
	m_fltStatMax		= FLT_MAX;
}
/////////////////////////////////////////////////////////////////////////////

		