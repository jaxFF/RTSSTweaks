// HALPollingThread.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HAL.h"
#include "HALPollingThread.h"
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CHALPollingThread
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CHALPollingThread, CWinThread)
/////////////////////////////////////////////////////////////////////////////
CHALPollingThread::CHALPollingThread()
{
}
/////////////////////////////////////////////////////////////////////////////
CHALPollingThread::CHALPollingThread(CHAL* lpHAL)
{
	m_lpHAL				= lpHAL;
	m_hEventKill		= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventAsyncPoll	= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bAutoDelete		= FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CHALPollingThread::~CHALPollingThread()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHALPollingThread::InitInstance()
{
	HANDLE waitObj[2];

	waitObj[0]			= m_hEventKill;
	waitObj[1]			= m_hEventAsyncPoll;

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
			ResetEvent(m_hEventAsyncPoll);	

			if (m_lpHAL)
				m_lpHAL->UpdateSources();
			break;
		}
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CHALPollingThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CHALPollingThread, CWinThread)
	//{{AFX_MSG_MAP(CHALPollingThread)
		// NOTE - the ClassWizard will add and remove mapHALPolling macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CHALPollingThread message handlers
/////////////////////////////////////////////////////////////////////////////
void CHALPollingThread::Kill()
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
void CHALPollingThread::Destroy()
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CHALPollingThread::AsyncPoll()
{
	SetEvent(m_hEventAsyncPoll);	
}
/////////////////////////////////////////////////////////////////////////////

