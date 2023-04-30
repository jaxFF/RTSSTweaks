#ifndef _PINGTHREAD_H_
#define _PINGTHREAD_H_
/////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CPingThread thread
/////////////////////////////////////////////////////////////////////////////
class CPingThread : public CWinThread
{
	DECLARE_DYNCREATE(CPingThread)
protected:
	CPingThread();           // protected constructor used by dynamic creation

// Attributes
public:
	CPingThread(LPCSTR lpAddr);           // protected constructor used by dynamic creation

// Operations
public:
	void	Kill();
	void	Destroy();

	void	SetAddr(LPCSTR lpAddr);

	void	UpdatePingAsync();
	void	UpdatePing();
	float	GetPing();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPingThread)
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CString		m_strAddr;
	HANDLE		m_hEventKill;
	HANDLE		m_hEventAsyncPing;
	float		m_fltPing;

	virtual ~CPingThread();

	// Generated message map functions
	//{{AFX_MSG(CPingThread)
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
