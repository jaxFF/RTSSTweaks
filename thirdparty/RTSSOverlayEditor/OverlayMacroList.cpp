// OverlayMacroList.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayMacroList.h"
#include "OverlayEditor.h"
#include "Overlay.h"
/////////////////////////////////////////////////////////////////////////////
COverlayMacroList::COverlayMacroList()
{
	m_dwTimerTarget			= 0;
	m_dwTimerTimestampBeg	= 0;
	m_dwTimerTimestampEnd	= 0;
}
/////////////////////////////////////////////////////////////////////////////
COverlayMacroList::~COverlayMacroList()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayMacroList::Init(COverlay* lpOverlay)
{
	RemoveAll();

	m_types.RemoveAll();

	CTime t	= CTime::GetCurrentTime();

	Append("%Timer%"		, FormatTimer());
	Append("%TimerFlashing%", GetTimerFlashing(max(lpOverlay->m_dwRefreshPeriod, 1000)) ? FormatTimer() : "");
	Append("%TimerHiding%"	, GetTimerVisibility()										? FormatTimer() : "");
	Append("%Time12%"		, t.Format("%I:%M:%S %p"	));
	Append("%Time24%"		, t.Format("%H:%M:%S"		));
	Append("%Date%"			, t.Format("%d.%m.%y"		));

	Append(g_hal.GetMacroList());

	Append("%PingAddr%"		, g_hal.GetPingAddr());

	CString strPollingTime;
	strPollingTime.Format("%.1f", lpOverlay->GetPollingTime());
	Append("%PollingTime0%"	, strPollingTime);
	strPollingTime.Format("%.1f", g_hal.GetPollingTime());
	Append("%PollingTime1%"	, strPollingTime);

	Append(lpOverlay->GetInternalDataSources(), MACROTYPE_INTERNAL_SOURCE);
	Append(lpOverlay->GetExternalDataSources(), MACROTYPE_EXTERNAL_SOURCE);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayMacroList::Append(LPCSTR lpMacroName, LPCSTR lpMacroValue, int type)
{
	AddTail(lpMacroName);
	AddTail(lpMacroValue);

	m_types.Add(type);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayMacroList::Append(CStringList* lpMacroList, int type)
{
	POSITION pos = lpMacroList->GetHeadPosition();

	while (pos)
	{
		CString strMacroName = lpMacroList->GetNext(pos);

		if (pos)
		{
			CString strMacroValue = lpMacroList->GetNext(pos);

			Append(strMacroName, strMacroValue, type);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayMacroList::Append(COverlayDataSourcesList* lpSourcesList, int type)
{
	POSITION pos = lpSourcesList->GetHeadPosition();

	while (pos)
	{
		COverlayDataSource* lpSource = lpSourcesList->GetNext(pos);

		CString strMacroName	= "%" + lpSource->GetName() + "%";
		CString strMacroValue	= lpSource->GetDataStr();

		POSITION pos = Find(strMacroName);

		if (pos)
		{
			GetNext(pos);

			if (pos)
			{
				if (!strMacroValue.IsEmpty())
					SetAt(pos, strMacroValue);
					
			}
		}
		else
			Append(strMacroName, strMacroValue, type);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayMacroList::SetTimer(LPCSTR lpParams)
{
	DWORD dwTimestamp = GetTickCount();

	if (sscanf_s(lpParams, "%d", &m_dwTimerTarget) == 1)
	{
		m_dwTimerTimestampBeg	= dwTimestamp;
		m_dwTimerTimestampEnd	= dwTimestamp + 1000 * abs(m_dwTimerTarget);
	}
}
/////////////////////////////////////////////////////////////////////////////
CString	COverlayMacroList::FormatTimer()
{
	DWORD dwTimestamp = GetTickCount();

	DWORD dwTime;
	
	if (m_dwTimerTarget < 0)
	{
		if (dwTimestamp < m_dwTimerTimestampEnd)
			dwTime = (m_dwTimerTimestampEnd - dwTimestamp + 500) / 1000;
		else
			dwTime = 0;
	}
	else
	{
		if (dwTimestamp < m_dwTimerTimestampEnd)
			dwTime = (dwTimestamp - m_dwTimerTimestampBeg + 500) / 1000;
		else
			dwTime = m_dwTimerTarget;
	}

	CString strTimer;
	strTimer.Format("%d:%02d:%02d", dwTime / 3600, (dwTime / 60) % 60, dwTime % 60);

	return strTimer;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayMacroList::GetTimerFlashing(DWORD dwPeriod)
{
	DWORD dwTimestamp = GetTickCount();

	if (dwTimestamp > m_dwTimerTimestampEnd)
	{
		if (((dwTimestamp - m_dwTimerTimestampEnd) / dwPeriod) & 1) 
			return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayMacroList::GetTimerVisibility()
{
	DWORD dwTimestamp = GetTickCount();

	if (dwTimestamp > m_dwTimerTimestampEnd)
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int COverlayMacroList::GetType(int index)
{
	if (index < m_types.GetCount())
		return m_types.GetAt(index);

	return -1;
}
/////////////////////////////////////////////////////////////////////////////
