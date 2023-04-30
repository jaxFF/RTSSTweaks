// Log.cpp: implementation of the CLog class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Log.h"

#include <stdio.h>
#include <stdlib.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLog::CLog()
{
	memset(m_szFilename, 0, sizeof(m_szFilename));

	m_dwHeaderFlags	= 0;
	m_dwIndent		= 0;

	m_lpBuffer		= NULL;
	m_dwBufferSize	= 0;

	m_dwEnabled		= LOG_ENABLE_FILE;
}
//////////////////////////////////////////////////////////////////////
CLog::~CLog()
{
	Append("", TRUE, TRUE);

	DestroyBuffer();
}
//////////////////////////////////////////////////////////////////////
BOOL CLog::AppendFile(LPCSTR lpLog, BOOL bAppend, BOOL bFlush)
{
	char szName	[_MAX_FNAME];	
	_splitpath_s(m_szFilename, NULL, 0, NULL, 0, szName, sizeof(szName), NULL, 0);

	char szMutex[MAX_PATH];
	sprintf_s(szMutex, sizeof(szMutex), "Access_File_%s", szName);

	HANDLE hMutex	= CreateMutex(NULL, FALSE, szMutex);
	WaitForSingleObject(hMutex, INFINITE);

	HFILE		hFile;
	OFSTRUCT	ofs;

	if (bAppend)
	{
		hFile = OpenFile(m_szFilename, &ofs, OF_WRITE | OF_SHARE_DENY_WRITE);

		if (hFile == HFILE_ERROR)
			hFile = OpenFile(m_szFilename, &ofs, OF_CREATE | OF_WRITE | OF_SHARE_DENY_WRITE);

		if (hFile == HFILE_ERROR)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);

			return FALSE;
		}
	}
	else
	{
		hFile = OpenFile(m_szFilename, &ofs, OF_CREATE | OF_WRITE | OF_SHARE_DENY_WRITE);

		if (hFile == HFILE_ERROR)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);

			return FALSE;
		}
	}

	SetFilePointer((HANDLE)hFile, NULL, NULL, FILE_END);

	char szLine[MAX_PATH] = { 0 };

	FormatLine(lpLog, szLine, sizeof(szLine));

	DWORD dwBytesWritten;

	if (bFlush)
	{
		if (m_lpBuffer && m_dwBufferSize)
			WriteFile((HANDLE)hFile, m_lpBuffer, strlen(m_lpBuffer), &dwBytesWritten, 0);
	}
	else
	{
		if (m_lpBuffer && m_dwBufferSize)
		{
			if (strlen(szLine) + strlen(m_lpBuffer) > m_dwBufferSize - 1)
			{
				WriteFile((HANDLE)hFile, m_lpBuffer, strlen(m_lpBuffer), &dwBytesWritten, 0);

				strcpy_s(m_lpBuffer, m_dwBufferSize, szLine);
			}
			else
				strcat_s(m_lpBuffer, m_dwBufferSize, szLine);
		}
		else
			WriteFile((HANDLE)hFile, szLine, strlen(szLine), &dwBytesWritten, 0);
	}

	_lclose(hFile);

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CLog::AppendDebugView(LPCSTR lpLog)
{
	char szLine[MAX_PATH] = { 0 };

	FormatLine(lpLog, szLine, sizeof(szLine));
	OutputDebugString(szLine);
}
//////////////////////////////////////////////////////////////////////
void CLog::FormatLine(LPCSTR lpLog, LPSTR lpLine, DWORD dwSize)
{
	SYSTEMTIME sysTime;	GetLocalTime(&sysTime);

	char szTemp[MAX_PATH] = { 0 };

	if (m_dwHeaderFlags & LOGHEADERFLAG_TIME)
	{
		sprintf_s(szTemp, sizeof(szTemp), "%.2d-%.2d-%.2d, %.2d:%.2d:%.2d ", sysTime.wDay, sysTime.wMonth, sysTime.wYear, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		strcat_s(lpLine, dwSize, szTemp);
	} 

	if (m_dwHeaderFlags & LOGHEADERFLAG_PROCESSID)
	{
		sprintf_s(szTemp, sizeof(szTemp), "%08X %08X ", GetCurrentProcessId(), GetCurrentThreadId());
#ifdef LOG_USE_MFC
		CString strAlias;

		if (m_aliasMap.Lookup(szTemp, strAlias))
			sprintf_s(szTemp, sizeof(szTemp), "%-17s ", strAlias);
#endif
		strcat_s(lpLine, dwSize, szTemp);
	}

	for (DWORD dwSpace=0; dwSpace<m_dwIndent; dwSpace++)
		strcat_s(lpLine, dwSize, " ");

	strcat_s(lpLine, dwSize, lpLog);
	strcat_s(lpLine, dwSize, "\r\n");
}
//////////////////////////////////////////////////////////////////////
void CLog::Append(LPCSTR lpLog, BOOL bAppend, BOOL bFlush)
{
	if (m_dwEnabled & LOG_ENABLE_FILE)
	{
		if (!AppendFile(lpLog, bAppend, bFlush))
		{
			if (m_dwEnabled & LOG_ENABLE_DEBUGVIEW)
				AppendDebugView(lpLog);
		}
	}
	else
		if (m_dwEnabled & LOG_ENABLE_DEBUGVIEW)
			AppendDebugView(lpLog);
}
//////////////////////////////////////////////////////////////////////
DWORD CLog::GetIndent()
{
	return m_dwIndent;
}
//////////////////////////////////////////////////////////////////////
void CLog::SetIndent(DWORD dwIndent)
{
	m_dwIndent = dwIndent;
}
//////////////////////////////////////////////////////////////////////
DWORD CLog::AddIndent(DWORD dwIndent)
{
	m_dwIndent += dwIndent;

	return m_dwIndent;
}
//////////////////////////////////////////////////////////////////////
DWORD CLog::SubIndent(DWORD dwIndent)
{
	m_dwIndent -= dwIndent;

	return m_dwIndent;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CLog::GetFilename()
{
	return m_szFilename;
}
//////////////////////////////////////////////////////////////////////
void CLog::SetFilename(LPCSTR lpFilename)
{
	strcpy_s(m_szFilename, sizeof(m_szFilename), lpFilename);
}
//////////////////////////////////////////////////////////////////////
DWORD CLog::GetHeaderFlags()
{
	return m_dwHeaderFlags;
}
//////////////////////////////////////////////////////////////////////
void CLog::SetHeaderFlags(DWORD dwHeaderFlags)
{
	m_dwHeaderFlags = dwHeaderFlags;
}
//////////////////////////////////////////////////////////////////////
void CLog::InitBuffer(DWORD dwBufferSize)
{
	DestroyBuffer();

	if (dwBufferSize)
	{
		m_lpBuffer		= new char[dwBufferSize];
		m_dwBufferSize	= dwBufferSize;

		memset(m_lpBuffer, 0, dwBufferSize);
	}
}
//////////////////////////////////////////////////////////////////////
void CLog::DestroyBuffer()
{
	if (m_lpBuffer)
	{
		delete [] m_lpBuffer;

		m_lpBuffer		= NULL;
		m_dwBufferSize	= 0;
	}
}
//////////////////////////////////////////////////////////////////////
#ifdef LOG_USE_MFC
void CLog::SetThreadAlias(LPCSTR lpAlias)
{
	char szTemp[MAX_PATH] = { 0 };

	sprintf_s(szTemp, sizeof(szTemp), "%08X %08X ", GetCurrentProcessId(), GetCurrentThreadId());

	m_aliasMap.SetAt(szTemp, lpAlias);
}
#endif
//////////////////////////////////////////////////////////////////////
void CLog::Enable(DWORD dwEnable)
{
	m_dwEnabled = dwEnable;
}
//////////////////////////////////////////////////////////////////////
