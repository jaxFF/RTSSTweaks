#ifndef _HALPOLLINGTHREAD_H_
#define _HALPOLLINGTHREAD_H_
/////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CHALPollingThread thread
/////////////////////////////////////////////////////////////////////////////
class CHAL;
class CHALPollingThread : public CWinThread
{
	DECLARE_DYNCREATE(CHALPollingThread)
protected:
	CHALPollingThread();           // protected constructor used by dynamic creation

// Attributes
public:
	CHALPollingThread(CHAL* lpHAL);           // protected constructor used by dynamic creation

// Operations
public:
	void	Kill();
	void	Destroy();

	void	AsyncPoll();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHALPollingThread)
	public:
	virtual BOOL	InitInstance();
	virtual int	ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CHAL*		m_lpHAL;
	HANDLE		m_hEventKill;
	HANDLE		m_hEventAsyncPoll;

	virtual ~CHALPollingThread();

	// Generated message map functions
	//{{AFX_MSG(CHALPollingThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
/////////////////////////////////////////////////////////////////////////////
#endif 
/////////////////////////////////////////////////////////////////////////////
