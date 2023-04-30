// PingThread.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PingThread.h"
#include "Log.h"

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>
#include <float.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
/////////////////////////////////////////////////////////////////////////////
//#define DEBUG_PING
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CPingThread
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CPingThread, CWinThread)
/////////////////////////////////////////////////////////////////////////////
CPingThread::CPingThread()
{
}
/////////////////////////////////////////////////////////////////////////////
CPingThread::CPingThread(LPCSTR lpAddr)
{
	m_hEventKill		= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventAsyncPing	= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bAutoDelete		= FALSE;

	m_fltPing			= FLT_MAX;

	SetAddr(lpAddr);
}
/////////////////////////////////////////////////////////////////////////////
CPingThread::~CPingThread()
{
}
/////////////////////////////////////////////////////////////////////////////
void CPingThread::UpdatePing()
{
    HANDLE	hIcmpFile		= INVALID_HANDLE_VALUE;
    IPAddr	addr			= INADDR_NONE;
    DWORD	dwRetVal		= 0;
    DWORD	dwError			= 0;
    char	szSendData[]	= "PING";
    LPVOID	lpReplyBuffer	= NULL;
    DWORD	dwReplySize		= 0;

    addr = inet_addr(m_strAddr);

    if (addr == INADDR_NONE) 
	{
#ifdef DEBUG_PING
		APPEND_LOG1("Failed to translate IP address %s", m_strAddr);
#endif
		return;
	}

    hIcmpFile = IcmpCreateFile();

    if (hIcmpFile == INVALID_HANDLE_VALUE) 
	{
#ifdef DEBUG_PING
		APPEND_LOG("Failed to open ICMP file");
#endif
		return;
	}
	
    dwReplySize		= sizeof(ICMP_ECHO_REPLY) + sizeof(szSendData) + 8;
    lpReplyBuffer	= new BYTE[dwReplySize];

    dwRetVal = IcmpSendEcho2(hIcmpFile, NULL, NULL, NULL, addr, szSendData, sizeof(szSendData), NULL, lpReplyBuffer, dwReplySize, 2000);

    if (dwRetVal) 
	{
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)lpReplyBuffer;

        struct in_addr replyAddr;
        replyAddr.S_un.S_addr = pEchoReply->Address;

#ifdef DEBUG_PING
		APPEND_LOG1("Sent ICMP message to %s", m_strAddr);
		APPEND_LOG1("Received %d ICMP message response(s)", dwRetVal);
		APPEND_LOG1("  Received from %s", inet_ntoa(replyAddr));
        APPEND_LOG1("  Status %d", pEchoReply->Status);
#endif
        switch (pEchoReply->Status) 
		{
        case IP_DEST_HOST_UNREACHABLE:
#ifdef DEBUG_PING
            APPEND_LOG("  (destination host was unreachable)");
#endif            break;
        case IP_DEST_NET_UNREACHABLE:
#ifdef DEBUG_PING
            APPEND_LOG("  (destination network was unreachable)");
#endif
            break;
        case IP_REQ_TIMED_OUT:
#ifdef DEBUG_PING
            APPEND_LOG("  (request timed out)");
#endif
            break;
        }

#ifdef DEBUG_PING
        APPEND_LOG1("  Roundtrip time = %d ms", pEchoReply->RoundTripTime);
#endif

		if (!pEchoReply->Status)
			m_fltPing = (float)pEchoReply->RoundTripTime;
		else
			m_fltPing = FLT_MAX;
	} 
	else 
	{
#ifdef DEBUG_PING
		APPEND_LOG("Failed to call IcmpSendEcho2");
#endif

        dwError = GetLastError();

        switch (dwError) 
		{
        case IP_BUF_TOO_SMALL:
#ifdef DEBUG_PING
			APPEND_LOG("Reply buffer size too small");
#endif
            break;
        case IP_REQ_TIMED_OUT:
#ifdef DEBUG_PING
            APPEND_LOG("Request timed out");
            break;
#endif
        default:
#ifdef DEBUG_PING
            APPEND_LOG1("Extended error %d", dwError);
#endif
            break;
        }

		m_fltPing = FLT_MAX;
    }

	delete [] lpReplyBuffer;

	IcmpCloseHandle(hIcmpFile);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPingThread::InitInstance()
{
	HANDLE waitObj[2];

	waitObj[0]			= m_hEventKill;
	waitObj[1]			= m_hEventAsyncPing;

	BOOL bProcess		= TRUE;

	while (bProcess)
	{
		DWORD dwResult = WaitForMultipleObjects(2, waitObj, FALSE, INFINITE);

		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			bProcess = FALSE;
			break;

		case WAIT_OBJECT_0 + 1:
			ResetEvent(m_hEventAsyncPing);	
			UpdatePing();
			break;
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CPingThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CPingThread, CWinThread)
	//{{AFX_MSG_MAP(CPingThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CPingThread message handlers
/////////////////////////////////////////////////////////////////////////////
void CPingThread::Kill()
{
	SetEvent(m_hEventKill);
		//set KILL event (it will terminate InitInstance) 
	SetThreadPriority(THREAD_PRIORITY_HIGHEST);
		//allow thread to run at high priority during killing

	WaitForSingleObject(m_hThread		, INFINITE);
		//wait until thread is dead
	Destroy();
		//delete thread
}
/////////////////////////////////////////////////////////////////////////////
void CPingThread::Destroy()
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CPingThread::UpdatePingAsync()
{
	SetEvent(m_hEventAsyncPing);	
}
/////////////////////////////////////////////////////////////////////////////
void CPingThread::SetAddr(LPCSTR lpAddr)
{
	m_strAddr = lpAddr;

	if (inet_addr(m_strAddr) == INADDR_NONE) 
	{
#ifdef DEBUG_PING
		APPEND_LOG1("Resolving address %s", m_strAddr);
#endif

		WSADATA wsaData;

		int result = WSAStartup(MAKEWORD(2,2), &wsaData);

		if (!result)
		{
			HOSTENT* lpHost = gethostbyname(m_strAddr); 

			if (lpHost && lpHost->h_addr_list[0])
			{
				m_strAddr = inet_ntoa(*(in_addr*)lpHost->h_addr_list[0]);

#ifdef DEBUG_PING
				APPEND_LOG1("Resolved address is %s", m_strAddr);
#endif
			}
			else
			{
#ifdef DEBUG_PING
				APPEND_LOG1("Failed to get host by name", m_strAddr);
#endif
			}

			WSACleanup();
		}
		else
		{
#ifdef DEBUG_PING
			APPEND_LOG1("WSAStartup failed with error code %d", result);
#endif
		}
	}
	else
	{
#ifdef DEBUG_PING
		APPEND_LOG1("Setting address %s", m_strAddr);
#endif
	}

	m_fltPing = FLT_MAX;
}
/////////////////////////////////////////////////////////////////////////////
float CPingThread::GetPing()
{
	return m_fltPing;
}
/////////////////////////////////////////////////////////////////////////////

