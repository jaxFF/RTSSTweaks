// Log.h: interface for the CLog class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _LOG_H_INCLUDED_
#define _LOG_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
#define LOGHEADERFLAG_TIME									0x00000001
#define LOGHEADERFLAG_PROCESSID								0x00000002
//////////////////////////////////////////////////////////////////////
//#define LOG_USE_MFC
//////////////////////////////////////////////////////////////////////
#ifdef LOG_USE_MFC
#include <afxtempl.h>
#endif
//////////////////////////////////////////////////////////////////////
#ifdef _WIN64
#define LOG_PTR "%016I64X"
#else
#define LOG_PTR "%08X"
#endif

#define LOG_PTR32 "%08X"
#define LOG_PTR64 "%016I64X"
//////////////////////////////////////////////////////////////////////
class CLog;
//////////////////////////////////////////////////////////////////////
#define LOG_ENABLE_FILE												1
#define LOG_ENABLE_DEBUGVIEW										2
//////////////////////////////////////////////////////////////////////
#ifdef  LOG_OBJECT

extern	CLog LOG_OBJECT;

#define APPEND_LOG(s) { LOG_OBJECT.Append(s, TRUE); }	
#define APPEND_LOG1(s, p1) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG2(s, p1, p2) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG3(s, p1, p2, p3) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG4(s, p1, p2, p3, p4) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG5(s, p1, p2, p3, p4, p5) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG6(s, p1, p2, p3, p4, p5, p6) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG7(s, p1, p2, p3, p4, p5, p6, p7) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6, p7); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG8(s, p1, p2, p3, p4, p5, p6, p7, p8) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6, p7, p8); LOG_OBJECT.Append(czLog, TRUE); }

#define APPEND_LOG_ENTER(s) { LOG_OBJECT.Append(s, TRUE); LOG_OBJECT.AddIndent(2); }	
#define APPEND_LOG_ENTER1(s, p1) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1); LOG_OBJECT.Append(czLog, TRUE); LOG_OBJECT.AddIndent(2); }
#define APPEND_LOG_ENTER2(s, p1, p2) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2); LOG_OBJECT.Append(czLog, TRUE); LOG_OBJECT.AddIndent(2); }
#define APPEND_LOG_ENTER3(s, p1, p2, p3) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3); LOG_OBJECT.Append(czLog, TRUE); LOG_OBJECT.AddIndent(2); }
#define APPEND_LOG_ENTER4(s, p1, p2, p3, p4) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4); LOG_OBJECT.Append(czLog, TRUE); LOG_OBJECT.AddIndent(2); }
#define APPEND_LOG_ENTER5(s, p1, p2, p3, p4, p5) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5); LOG_OBJECT.Append(czLog, TRUE); LOG_OBJECT.AddIndent(2); }
#define APPEND_LOG_ENTER6(s, p1, p2, p3, p4, p5, p6) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6); LOG_OBJECT.Append(czLog, TRUE); LOG_OBJECT.AddIndent(2); }
#define APPEND_LOG_ENTER7(s, p1, p2, p3, p4, p5, p6, p7) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6, p7); LOG_OBJECT.Append(czLog, TRUE); LOG_OBJECT.AddIndent(2); }
#define APPEND_LOG_ENTER8(s, p1, p2, p3, p4, p5, p6, p7, p8) { char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6, p7, p8); LOG_OBJECT.Append(czLog, TRUE); LOG_OBJECT.AddIndent(2); }

#define APPEND_LOG_LEAVE(s) { LOG_OBJECT.SubIndent(2); LOG_OBJECT.Append(s, TRUE); }	
#define APPEND_LOG_LEAVE1(s, p1) { LOG_OBJECT.SubIndent(2); char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG_LEAVE2(s, p1, p2) { LOG_OBJECT.SubIndent(2); char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG_LEAVE3(s, p1, p2, p3) { LOG_OBJECT.SubIndent(2); char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG_LEAVE4(s, p1, p2, p3, p4) { LOG_OBJECT.SubIndent(2); char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG_LEAVE5(s, p1, p2, p3, p4, p5) { LOG_OBJECT.SubIndent(2); char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG_LEAVE6(s, p1, p2, p3, p4, p5, p6) { LOG_OBJECT.SubIndent(2); char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG_LEAVE7(s, p1, p2, p3, p4, p5, p6, p7) { LOG_OBJECT.SubIndent(2); char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6, p7); LOG_OBJECT.Append(czLog, TRUE); }
#define APPEND_LOG_LEAVE8(s, p1, p2, p3, p4, p5, p6, p7, p8) { LOG_OBJECT.SubIndent(2); char czLog[256]; sprintf_s(czLog, sizeof(czLog), s, p1, p2, p3, p4, p5, p6, p7, p8); LOG_OBJECT.Append(czLog, TRUE); }

#define TIMING_INIT		LARGE_INTEGER pf, pc0, pc1; QueryPerformanceFrequency(&pf); FLOAT timing;
#define TIMING_BEGIN	QueryPerformanceCounter(&pc0);	
#define TIMING_END		QueryPerformanceCounter(&pc1); if (pf.QuadPart) timing = 1000.0f * (pc1.QuadPart - pc0.QuadPart) / pf.QuadPart;

#else

#define APPEND_LOG(s)
#define APPEND_LOG1(s, p1)
#define APPEND_LOG2(s, p1, p2)
#define APPEND_LOG3(s, p1, p2, p3)
#define APPEND_LOG4(s, p1, p2, p3, p4)
#define APPEND_LOG5(s, p1, p2, p3, p4, p5)
#define APPEND_LOG6(s, p1, p2, p3, p4, p5, p6)
#define APPEND_LOG7(s, p1, p2, p3, p4, p5, p6, p7)
#define APPEND_LOG8(s, p1, p2, p3, p4, p5, p6, p7, p8)

#define APPEND_LOG_ENTER(s)
#define APPEND_LOG_ENTER1(s, p1)
#define APPEND_LOG_ENTER2(s, p1, p2)
#define APPEND_LOG_ENTER3(s, p1, p2, p3)
#define APPEND_LOG_ENTER4(s, p1, p2, p3, p4)
#define APPEND_LOG_ENTER5(s, p1, p2, p3, p4, p5)
#define APPEND_LOG_ENTER6(s, p1, p2, p3, p4, p5, p6)
#define APPEND_LOG_ENTER7(s, p1, p2, p3, p4, p5, p6, p7)
#define APPEND_LOG_ENTER8(s, p1, p2, p3, p4, p5, p6, p7, p8)

#define APPEND_LOG_LEAVE(s)
#define APPEND_LOG_LEAVE1(s, p1)
#define APPEND_LOG_LEAVE2(s, p1, p2)
#define APPEND_LOG_LEAVE3(s, p1, p2, p3)
#define APPEND_LOG_LEAVE4(s, p1, p2, p3, p4)
#define APPEND_LOG_LEAVE5(s, p1, p2, p3, p4, p5)
#define APPEND_LOG_LEAVE6(s, p1, p2, p3, p4, p5, p6)
#define APPEND_LOG_LEAVE7(s, p1, p2, p3, p4, p5, p6, p7)
#define APPEND_LOG_LEAVE8(s, p1, p2, p3, p4, p5, p6, p7, p8)

#define TIMING_INIT
#define TIMING_BEGIN
#define TIMING_END

#endif 
//////////////////////////////////////////////////////////////////////
class CLog  
{
public:
	void	Enable(DWORD dwEnable);
#ifdef LOG_USE_MFC
	void	SetThreadAlias(LPCSTR lpAlias);
#endif
	void	InitBuffer(DWORD dwBufferSize);
	void	DestroyBuffer();

	void	Append(LPCSTR lpLog, BOOL bAppend, BOOL bFlush = FALSE);

	LPCSTR	GetFilename();
	void	SetFilename(LPCSTR lpFilename);

	DWORD	GetIndent();
	void	SetIndent(DWORD dwIndent);
	DWORD	AddIndent(DWORD dwIndent);
	DWORD	SubIndent(DWORD dwIndent);

	DWORD	GetHeaderFlags();
	void	SetHeaderFlags(DWORD dwHeaderFlags);

	CLog();
	virtual ~CLog();

protected:
	void				FormatLine(LPCSTR lpLog, LPSTR lpLine, DWORD dwSize);
	BOOL				AppendFile(LPCSTR lpLog, BOOL bAppend, BOOL bFlush = FALSE);
	void				AppendDebugView(LPCSTR lpLog);

	char				m_szFilename[MAX_PATH];
	DWORD				m_dwHeaderFlags;
	DWORD				m_dwIndent;

	LPSTR				m_lpBuffer;
	DWORD				m_dwBufferSize;

	DWORD				m_dwEnabled;

#ifdef LOG_USE_MFC
	CMapStringToString	m_aliasMap;
#endif
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
