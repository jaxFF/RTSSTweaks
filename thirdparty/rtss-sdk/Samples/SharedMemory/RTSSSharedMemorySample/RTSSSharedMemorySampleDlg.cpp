// RTSSSharedMemorySampleDlg.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "RTSSSharedMemory.h"
#include "RTSSSharedMemorySample.h"
#include "RTSSSharedMemorySampleDlg.h"
#include "GroupedString.h"
/////////////////////////////////////////////////////////////////////////////
#include <shlwapi.h>
#include <float.h>
#include <io.h>
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
/////////////////////////////////////////////////////////////////////////////
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CRTSSSharedMemorySampleDlg dialog
/////////////////////////////////////////////////////////////////////////////
CRTSSSharedMemorySampleDlg::CRTSSSharedMemorySampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRTSSSharedMemorySampleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRTSSSharedMemorySampleDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	m_hIcon						= AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_dwNumberOfProcessors		= 0;
	m_pNtQuerySystemInformation	= NULL;
	m_strStatus					= "";
	m_strInstallPath			= "";

	m_bMultiLineOutput			= TRUE;
	m_bFormatTags				= TRUE;
	m_bFillGraphs				= FALSE;
	m_bConnected				= FALSE;
	m_bStickyLayers				= FALSE;
	m_bAbsolutePosition			= FALSE;

	m_dwHistoryPos				= 0;
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTSSSharedMemorySampleDlg)
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CRTSSSharedMemorySampleDlg, CDialog)
	//{{AFX_MSG_MAP(CRTSSSharedMemorySampleDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CRTSSSharedMemorySampleDlg message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CRTSSSharedMemorySampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon


	CWnd* pPlaceholder = GetDlgItem(IDC_PLACEHOLDER);

	if (pPlaceholder)
	{
		CRect rect; 
		pPlaceholder->GetClientRect(&rect);

		if (!m_richEditCtrl.Create(WS_VISIBLE|ES_READONLY|ES_MULTILINE|ES_AUTOHSCROLL|WS_HSCROLL|ES_AUTOVSCROLL|WS_VSCROLL, rect, this, 0))
			return FALSE;

		m_font.CreateFont(-11, 0, 0, 0, FW_REGULAR, 0, 0, 0, BALTIC_CHARSET, 0, 0, 0, 0, "Courier New");
		m_richEditCtrl.SetFont(&m_font);
	}

	//init CPU usage calculation related variables	

	SYSTEM_INFO info;
	GetSystemInfo(&info); 

	m_dwNumberOfProcessors		= info.dwNumberOfProcessors;
	m_pNtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandle("NTDLL"), "NtQuerySystemInformation");

	for (DWORD dwCpu=0; dwCpu<MAX_CPU; dwCpu++)
	{
		m_idleTime[dwCpu].QuadPart			= 0;
		m_fltCpuUsage[dwCpu]				= FLT_MAX;
		m_dwTickCount[dwCpu]				= 0;

		for (DWORD dwPos=0; dwPos<MAX_HISTORY; dwPos++)
			m_fltCpuUsageHistory[dwCpu][dwPos] = FLT_MAX;
	}

	//init RAM usage history

	for (DWORD dwPos=0; dwPos<MAX_HISTORY; dwPos++)
		m_fltRamUsageHistory[dwPos] = FLT_MAX;


	//init timer

	m_nTimerID = SetTimer(0x1234, 1000, NULL);

	Refresh();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
/////////////////////////////////////////////////////////////////////////////
HCURSOR CRTSSSharedMemorySampleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
// we'll update sample's window on timer once per second
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::OnTimer(UINT nIDEvent) 
{
	Refresh();
	
	CDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::OnDestroy() 
{
	if (m_nTimerID)
		KillTimer(m_nTimerID);

	m_nTimerID = NULL;

	MSG msg; 
	while (PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE));

	ReleaseOSD();

	CDialog::OnDestroy();	
}
/////////////////////////////////////////////////////////////////////////////
DWORD CRTSSSharedMemorySampleDlg::GetSharedMemoryVersion()
{
	DWORD dwResult	= 0;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr				= MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		LPRTSS_SHARED_MEMORY pMem	= (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020000))
				dwResult = pMem->dwVersion;

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return dwResult;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CRTSSSharedMemorySampleDlg::EmbedGraph(DWORD dwOffset, FLOAT* lpBuffer, DWORD dwBufferPos, DWORD dwBufferSize, LONG dwWidth, LONG dwHeight, LONG dwMargin, FLOAT fltMin, FLOAT fltMax, DWORD dwFlags)
{
	DWORD dwResult	= 0;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr				= MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		LPRTSS_SHARED_MEMORY pMem	= (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020000))
			{
				for (DWORD dwPass=0; dwPass<2; dwPass++)
					//1st pass : find previously captured OSD slot
					//2nd pass : otherwise find the first unused OSD slot and capture it
				{
					for (DWORD dwEntry=1; dwEntry<pMem->dwOSDArrSize; dwEntry++)
						//allow primary OSD clients (i.e. EVGA Precision / MSI Afterburner) to use the first slot exclusively, so third party
						//applications start scanning the slots from the second one
					{
						RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY)((LPBYTE)pMem + pMem->dwOSDArrOffset + dwEntry * pMem->dwOSDEntrySize);

						if (dwPass)
						{
							if (!strlen(pEntry->szOSDOwner))
								strcpy_s(pEntry->szOSDOwner, sizeof(pEntry->szOSDOwner), "RTSSSharedMemorySample");
						}

						if (!strcmp(pEntry->szOSDOwner, "RTSSSharedMemorySample"))
						{
							if (pMem->dwVersion >= 0x0002000c)
								//embedded graphs are supported for v2.12 and higher shared memory
							{
								if (dwOffset + sizeof(RTSS_EMBEDDED_OBJECT_GRAPH) + dwBufferSize * sizeof(FLOAT) > sizeof(pEntry->buffer))
									//validate embedded object offset and size and ensure that we don't overrun the buffer
								{
									UnmapViewOfFile(pMapAddr);

									CloseHandle(hMapFile);

									return 0;
								}

								LPRTSS_EMBEDDED_OBJECT_GRAPH lpGraph = (LPRTSS_EMBEDDED_OBJECT_GRAPH)(pEntry->buffer + dwOffset);
									//get pointer to object in buffer

								lpGraph->header.dwSignature	= RTSS_EMBEDDED_OBJECT_GRAPH_SIGNATURE;
								lpGraph->header.dwSize		= sizeof(RTSS_EMBEDDED_OBJECT_GRAPH) + dwBufferSize * sizeof(FLOAT);
								lpGraph->header.dwWidth		= dwWidth;
								lpGraph->header.dwHeight	= dwHeight;
								lpGraph->header.dwMargin	= dwMargin;
								lpGraph->dwFlags			= dwFlags;
								lpGraph->fltMin				= fltMin;
								lpGraph->fltMax				= fltMax;
								lpGraph->dwDataCount		= dwBufferSize;

								if (lpBuffer && dwBufferSize)
								{
									for (DWORD dwPos=0; dwPos<dwBufferSize; dwPos++)
									{
										FLOAT fltData = lpBuffer[dwBufferPos];

										lpGraph->fltData[dwPos] = (fltData == FLT_MAX) ? 0 : fltData;

										dwBufferPos = (dwBufferPos + 1) & (dwBufferSize - 1);
									}
								}

								dwResult = lpGraph->header.dwSize;
							}

							break;
						}
					}

					if (dwResult)
						break;
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return dwResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRTSSSharedMemorySampleDlg::UpdateOSD(LPCSTR lpText)
{
	BOOL bResult	= FALSE;

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr				= MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		LPRTSS_SHARED_MEMORY pMem	= (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020000))
			{
				for (DWORD dwPass=0; dwPass<2; dwPass++)
					//1st pass : find previously captured OSD slot
					//2nd pass : otherwise find the first unused OSD slot and capture it
				{
					for (DWORD dwEntry=1; dwEntry<pMem->dwOSDArrSize; dwEntry++)
						//allow primary OSD clients (i.e. EVGA Precision / MSI Afterburner) to use the first slot exclusively, so third party
						//applications start scanning the slots from the second one
					{
						RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY)((LPBYTE)pMem + pMem->dwOSDArrOffset + dwEntry * pMem->dwOSDEntrySize);

						if (dwPass)
						{
							if (!strlen(pEntry->szOSDOwner))
								strcpy_s(pEntry->szOSDOwner, sizeof(pEntry->szOSDOwner), "RTSSSharedMemorySample");
						}

						if (!strcmp(pEntry->szOSDOwner, "RTSSSharedMemorySample"))
						{
							if (pMem->dwVersion >= 0x00020007)
								//use extended text slot for v2.7 and higher shared memory, it allows displaying 4096 symbols
								//instead of 256 for regular text slot
							{
								if (pMem->dwVersion >= 0x0002000e)	
									//OSD locking is supported on v2.14 and higher shared memory
								{
									DWORD dwBusy = _interlockedbittestandset(&pMem->dwBusy, 0);
										//bit 0 of this variable will be set if OSD is locked by renderer and cannot be refreshed
										//at the moment

									if (!dwBusy)
									{
										strncpy_s(pEntry->szOSDEx, sizeof(pEntry->szOSDEx), lpText, sizeof(pEntry->szOSDEx) - 1);	

										pMem->dwBusy = 0;
									}
								}
								else
									strncpy_s(pEntry->szOSDEx, sizeof(pEntry->szOSDEx), lpText, sizeof(pEntry->szOSDEx) - 1);	

							}
							else
								strncpy_s(pEntry->szOSD, sizeof(pEntry->szOSD), lpText, sizeof(pEntry->szOSD) - 1);

							pMem->dwOSDFrame++;

							bResult = TRUE;

							break;
						}
					}

					if (bResult)
						break;
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::ReleaseOSD()
{
	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		LPRTSS_SHARED_MEMORY pMem = (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020000))
			{
				for (DWORD dwEntry=1; dwEntry<pMem->dwOSDArrSize; dwEntry++)
				{
					RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY)((LPBYTE)pMem + pMem->dwOSDArrOffset + dwEntry * pMem->dwOSDEntrySize);

					if (!strcmp(pEntry->szOSDOwner, "RTSSSharedMemorySample"))
					{
						memset(pEntry, 0, pMem->dwOSDEntrySize);
						pMem->dwOSDFrame++;
					}
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}
}
/////////////////////////////////////////////////////////////////////////////
DWORD CRTSSSharedMemorySampleDlg::GetClientsNum()
{
	DWORD dwClients = 0;	

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "RTSSSharedMemoryV2");

	if (hMapFile)
	{
		LPVOID pMapAddr = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		LPRTSS_SHARED_MEMORY pMem = (LPRTSS_SHARED_MEMORY)pMapAddr;

		if (pMem)
		{
			if ((pMem->dwSignature == 'RTSS') && 
				(pMem->dwVersion >= 0x00020000))
			{
				for (DWORD dwEntry=0; dwEntry<pMem->dwOSDArrSize; dwEntry++)
				{
					RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY pEntry = (RTSS_SHARED_MEMORY::LPRTSS_SHARED_MEMORY_OSD_ENTRY)((LPBYTE)pMem + pMem->dwOSDArrOffset + dwEntry * pMem->dwOSDEntrySize);

					if (strlen(pEntry->szOSDOwner))
						dwClients++;
				}
			}

			UnmapViewOfFile(pMapAddr);
		}

		CloseHandle(hMapFile);
	}

	return dwClients;
}
/////////////////////////////////////////////////////////////////////////////
float CRTSSSharedMemorySampleDlg::CalcCpuUsage(DWORD dwCpu)
{
	//validate NtQuerySystemInformation pointer and return FLT_MAX on error

	if (!m_pNtQuerySystemInformation)
		return FLT_MAX;

	//validate specified CPU index and return FLT_MAX on error

	if (dwCpu >=  m_dwNumberOfProcessors)
		return FLT_MAX;

	DWORD dwTickCount = GetTickCount();
		//get standard timer tick count

	if (dwTickCount - m_dwTickCount[dwCpu] >= 1000)
		//update usage once per second
	{
		SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION info[MAX_CPU];

		if (SUCCEEDED(m_pNtQuerySystemInformation(SystemProcessorPerformanceInformation, &info, sizeof(info), NULL)))
				//query CPU usage
		{
			if (m_idleTime[dwCpu].QuadPart)
				//ensure that this function was already called at least once
				//and we have the previous idle time value
			{
				m_fltCpuUsage[dwCpu] = 100.0f - 0.01f * (info[dwCpu].IdleTime.QuadPart - m_idleTime[dwCpu].QuadPart) / (dwTickCount - m_dwTickCount[dwCpu]);
					//calculate new CPU usage value by estimating amount of time
					//CPU was in idle during the last second

				//clip calculated CPU usage to [0-100] range to filter calculation non-ideality

				if (m_fltCpuUsage[dwCpu] < 0.0f)
					m_fltCpuUsage[dwCpu] = 0.0f;

				if (m_fltCpuUsage[dwCpu] > 100.0f)
					m_fltCpuUsage[dwCpu] = 100.0f;
			}

			m_idleTime[dwCpu]		= info[dwCpu].IdleTime;
				//save new idle time for specified CPU
			m_dwTickCount[dwCpu]	= dwTickCount;
				//save new tick count for specified CPU
		}
	}

	return m_fltCpuUsage[dwCpu];
}
/////////////////////////////////////////////////////////////////////////////
DWORD CRTSSSharedMemorySampleDlg::GetPhysicalMemoryUsage()
{
	MEMORYSTATUSEX status; 
	status.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&status);

	return (DWORD)((__int64)(status.ullTotalPhys - status.ullAvailPhys) / 1048576);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CRTSSSharedMemorySampleDlg::GetTotalPhysicalMemory()
{
	MEMORYSTATUSEX status; 
	status.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&status);

	return (DWORD)((__int64)status.ullTotalPhys / 1048576);
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::Refresh()
{
	//init RivaTuner Statistics Server installation path

	if (m_strInstallPath.IsEmpty())
	{
		HKEY hKey;

		if (ERROR_SUCCESS == RegOpenKey(HKEY_LOCAL_MACHINE, "Software\\Unwinder\\RTSS", &hKey))
		{
			char buf[MAX_PATH];

			DWORD dwSize = MAX_PATH;
			DWORD dwType;

			if (ERROR_SUCCESS == RegQueryValueEx(hKey, "InstallPath", 0, &dwType, (LPBYTE)buf, &dwSize))
			{
				if (dwType == REG_SZ)
					m_strInstallPath = buf;
			}

			RegCloseKey(hKey);
		}
	}

	//validate RivaTuner Statistics Server installation path

	if (_taccess(m_strInstallPath, 0))
		m_strInstallPath = "";

	//init profile interface 

	if (!m_strInstallPath.IsEmpty())
	{
		if (!m_profileInterface.IsInitialized())
			m_profileInterface.Init(m_strInstallPath);
	}

	//init shared memory version

	DWORD dwSharedMemoryVersion = GetSharedMemoryVersion();

	//init max OSD text size, we'll use extended text slot for v2.7 and higher shared memory, 
	//it allows displaying 4096 symbols /instead of 256 for regular text slot

	DWORD dwMaxTextSize = (dwSharedMemoryVersion >= 0x00020007) ? sizeof(RTSS_SHARED_MEMORY::RTSS_SHARED_MEMORY_OSD_ENTRY().szOSDEx) : sizeof(RTSS_SHARED_MEMORY::RTSS_SHARED_MEMORY_OSD_ENTRY().szOSD);

	CGroupedString groupedString(dwMaxTextSize - 1);
		// RivaTuner based products use similar CGroupedString object for convenient OSD text formatting and length control
		// You may use it to format your OSD similar to RivaTuner's one or just use your own routines to format OSD text

	BOOL bFormatTagsSupported	= (dwSharedMemoryVersion >= 0x0002000b);
		//text format tags are supported for shared memory v2.11 and higher
	BOOL bObjTagsSupported		= (dwSharedMemoryVersion >= 0x0002000c);
		//embedded object tags are supporoted for shared memory v2.12 and higher

	CString strOSD;

	if (bFormatTagsSupported && m_bFormatTags)
	{
		if (m_bStickyLayers)
		{
			strOSD += "<P0><L0>+";
				//define layer 0 located in sticky position 0 (top left corner)
			strOSD += "<P2><L1>+";
				//define layer 1 located in sticky position 1 (top right corner)
			strOSD += "<P6><L2>+";
				//define layer 2 located in sticky position 2 (bottom left corner)
			strOSD += "<P8><L3>+";
				//define layer 3 located in sticky position 3 (bottom right corner)
		}

		if ((GetClientsNum() == 1) || m_bStickyLayers || m_bAbsolutePosition)
			strOSD += "<P=10,10>";
			//move to position 10,10 (in zoomed pixel units)

			//Note: take a note that position is specified in absolute coordinates so use this tag with caution because your text may
			//overlap with text slots displayed by other applications, so in this demo we explicitly disable this tag usage if more than
			//one client is currently rendering something in OSD and we don't use sticky layers
			
		strOSD += "<A0=-5>";
			//define align variable A[0] as right alignment by 5 symbols (positive is left, negative is right)
		strOSD += "<A1=4>";
			//define align variable A[1] as left alignment by 4 symbols (positive is left, negative is right)
		strOSD += "<C0=FFA0A0>";
			//define color variable C[0] as R=FF,G=A0 and B=A0
		strOSD += "<C1=FF00A0>";
			//define color variable C[1] as R=FF,G=00 and B=A0
		strOSD += "<C2=FFFFFF>";
			//define color variable C[1] as R=FF,G=FF and B=FF
		strOSD += "<S0=-50>";
			//define size variable S[0] as 50% subscript (positive is superscript, negative is subscript)
		strOSD += "<S1=50>";
			//define size variable S[0] as 50% supercript (positive is superscript, negative is subscript)

		strOSD += "\r";
			//add \r just for this demo to make tagged text more readable in demo preview window, OSD ignores \r anyway
			
		//Note: we could apply explicit alignment,size and color definitions when necerrary (e.g. <C=FFFFFF>, however
		//variables usage makes tagged text more compact and readable
	}
	else
		strOSD = "";

	for (DWORD dwCpu=0; dwCpu<min(m_dwNumberOfProcessors, MAX_CPU); dwCpu++)
	{
		float fltCpuUsage = CalcCpuUsage(dwCpu);

		if (fltCpuUsage != FLT_MAX)
		{
			CString strValue;
			if (bFormatTagsSupported && m_bFormatTags) 
				strValue.Format("<A0>%.0f<A><A1><S1> %%<S><A>", fltCpuUsage);	
					//align the text surrounded by alignment tags
			else
				strValue.Format("%.0f %%", fltCpuUsage);

			CString strGroup;
			if ((m_dwNumberOfProcessors > 1) && m_bMultiLineOutput)
			{
				if (bFormatTagsSupported && m_bFormatTags) 
					strGroup.Format("<C0>CPU<S0>%d<S><C>", dwCpu + 1);	
					//set color and size usin previously defined by C[0] and S[0] then restore defaults
				else
					strGroup.Format("CPU%d", dwCpu + 1);
			}
			else
			{
				if (bFormatTagsSupported && m_bFormatTags) 
					strGroup = "<C0>CPU<C>";	
					//set color using previously defined by C[0] then restore defaults
				else
					strGroup = "CPU";
			}

			if (bFormatTagsSupported && m_bFormatTags) 
				groupedString.Add(strValue, strGroup, "\n", m_bFormatTags ? " " : ", ");
			else
				groupedString.Add(strValue, strGroup, "\n", m_bFormatTags ? " " : ", ");
		}

		m_fltCpuUsageHistory[dwCpu][m_dwHistoryPos] = fltCpuUsage;
			//store CPU usage in history ring buffer
	}

	DWORD dwRamUsage = GetPhysicalMemoryUsage();
		//get physical memory usage
	DWORD dwTotalRam = GetTotalPhysicalMemory();
		//get total physical memory

	CString strGroup;
	if (bFormatTagsSupported && m_bFormatTags) 
		strGroup = "<C1>RAM<C>";	
		//set color previously defined by C[1] then restore defaults
	else
		strGroup = "RAM";

	CString strValue;
	if (bFormatTagsSupported && m_bFormatTags) 
		strValue.Format("<A0>%d<A><A1><S1> MB<S><A>", dwRamUsage);	
			//align the text surrounded by alignment tags
	else
		strValue.Format("%d MB", dwRamUsage);

	groupedString.Add(strValue, strGroup, "\n", m_bFormatTags ? " " : ", ");

	m_fltRamUsageHistory[m_dwHistoryPos] = (FLOAT)dwRamUsage;
		//store RAM usage in history ring buffer

	m_dwHistoryPos = (m_dwHistoryPos + 1) & (MAX_HISTORY - 1);
		//modify ring buffer position (assuming that MAX_HISTORY is a power of 2!)

	if (bFormatTagsSupported && m_bFormatTags)
	{
		groupedString.Add("<A0><FR><A><A1><S1> FPS<S><A>"	, "<C2><APP><C>", "\n", m_bFormatTags ? " " : ", ");	
		groupedString.Add("<A0><FT><A><A1><S1> ms<S><A>"	, "<C2><APP><C>", "\n", m_bFormatTags ? " " : ", ");	
			//print application-specific 3D API, framerate and frametime using tags
	}
	else
	{
		groupedString.Add("%FRAMERATE%", "", "\n");
		groupedString.Add("%FRAMETIME%", "", "\n");
			//print application-specific 3D API, framerate and frametime using deprecated macro
	}
																									
	BOOL bTruncated	= FALSE;

	strOSD += groupedString.Get(bTruncated, FALSE, m_bFormatTags ? "\t" : " \t: ");

	if (bObjTagsSupported && m_bFormatTags)
	{
		strOSD += "\n";

		DWORD dwObjectOffset	= 0;
		DWORD dwObjectSize		= 0;
		DWORD dwFlags			= m_bFillGraphs ? RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FILLED : 0;

		for (DWORD dwCpu=0; dwCpu<min(m_dwNumberOfProcessors, MAX_CPU); dwCpu++)
		{
			CString strObj;

			dwObjectSize = EmbedGraph(dwObjectOffset, &m_fltCpuUsageHistory[dwCpu][0], m_dwHistoryPos, MAX_HISTORY, -32, -2, 1, 0.0f, 100.0f, dwFlags);
				//embed CPU usage graph object into the buffer

			if (dwObjectSize)
			{
				strObj.Format("<C0><OBJ=%08X><A0><S1>%.0f<A> %%<S><C>\n", dwObjectOffset, (m_fltCpuUsage[dwCpu] == FLT_MAX) ? 0 : m_fltCpuUsage[dwCpu]);
				strOSD += strObj;
					//print embedded object
				dwObjectOffset += dwObjectSize;
					//modify object offset
			}
		}

		CString strObj;

		dwObjectSize = EmbedGraph(dwObjectOffset, &m_fltRamUsageHistory[0], m_dwHistoryPos, MAX_HISTORY, -32, -2, 1, 0.0f, (FLOAT)dwTotalRam, dwFlags);
			//embed RAM usage graph object into the buffer

		if (dwObjectSize)
		{
			strObj.Format("<C1><OBJ=%08X><A0><S1>%d<A> MB<S><C>\n", dwObjectOffset, dwRamUsage);
			strOSD += strObj;
				//print embedded object
			dwObjectOffset += dwObjectSize;
				//modify object offset
		}

		dwObjectSize = EmbedGraph(dwObjectOffset, NULL, 0, 0, -32, -2, 1, 0.0f, 200.0f, dwFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMERATE);
			//embed framerate graph object into the buffer

		if (dwObjectSize)
		{
			strObj.Format("<C2><OBJ=%08X><A0><S1><FR><A> FPS<S><C>\n", dwObjectOffset);
			strOSD += strObj;
				//print embedded object
			dwObjectOffset += dwObjectSize;
				//modify object offset
		}

		dwObjectSize = EmbedGraph(dwObjectOffset, NULL, 0, 0, -32, -2, 1, 0.0f, 50000.0f, dwFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMETIME);
			//embed frametime graph object into the buffer

		if (dwObjectSize)
		{
			strObj.Format("<C2><OBJ=%08X><A0><S1><FT><A> ms<S><C>\n", dwObjectOffset);
			strOSD += strObj;
				//print embedded object
			dwObjectOffset += dwObjectSize;
				//modify object offset
		}
	}
	if (!strOSD.IsEmpty())
	{
		BOOL bResult = UpdateOSD(strOSD);

		m_bConnected = bResult;

		if (bResult)
		{
			m_strStatus = "The following text is being forwarded to OSD:\n\n" + strOSD;

			if (bTruncated)
				m_strStatus += "\n\nWarning!\nThe text is too long to be displayed in OSD, some info has been truncated!";

			m_strStatus += "\n\nHints:\n-Press <Space> to toggle multi-line OSD text formatting";

			if (bFormatTagsSupported)
				m_strStatus += "\n-Press <F> to toggle OSD text formatting tags";

			if (bFormatTagsSupported)
				m_strStatus += "\n-Press <I> to toggle graphs fill mode";

			if (bFormatTagsSupported)
				m_strStatus += "\n-Press <S> to toggle sticky layers";

			if (bFormatTagsSupported)
				m_strStatus += "\n-Press <P> to toggle absolute position";
		}
		else
		{

			if (m_strInstallPath.IsEmpty())
				m_strStatus = "Failed to connect to RTSS shared memory!\n\nHints:\n-Install RivaTuner Statistics Server";
			else
				m_strStatus = "Failed to connect to RTSS shared memory!\n\nHints:\n-Press <Space> to start RivaTuner Statistics Server";
		}

		if (m_profileInterface.IsInitialized())
		{
			m_strStatus += "\n-Press <Up>,<Down>,<Left>,<Right> to change OSD position in global profile";
			m_strStatus += "\n-Press <R>,<G>,<B> to change OSD color in global profile";
		}

		m_richEditCtrl.SetWindowText(m_strStatus);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CRTSSSharedMemorySampleDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case 'P':
			if (m_bConnected)
			{
				m_bAbsolutePosition = !m_bAbsolutePosition;
				Refresh();
			}
			break;

		case 'S':
			if (m_bConnected)
			{
				m_bStickyLayers = !m_bStickyLayers;
				Refresh();
			}
			break;
		case 'F':
			if (m_bConnected)
			{
				m_bFormatTags = !m_bFormatTags;
				Refresh();
			}
			break;
		case 'I':
			if (m_bConnected)
			{
				m_bFillGraphs = !m_bFillGraphs;
				Refresh();
			}
			break;
		case ' ':
			if (m_bConnected)
			{
				m_bMultiLineOutput = !m_bMultiLineOutput;
				Refresh();
			}
			else
			{
				if (!m_strInstallPath.IsEmpty())
					ShellExecute(GetSafeHwnd(), "open", m_strInstallPath, NULL, NULL, SW_SHOWNORMAL);
			}
			return TRUE;
		case VK_UP:
			IncProfileProperty("", "PositionY", -1);
			return TRUE;
		case VK_DOWN:
			IncProfileProperty("", "PositionY", +1);
			return TRUE;
		case VK_LEFT:
			IncProfileProperty("", "PositionX", -1);
			return TRUE;
		case VK_RIGHT:
			IncProfileProperty("", "PositionX", +1);
			return TRUE;
		case 'R':
			SetProfileProperty("", "BaseColor", 0xFF0000);
			return TRUE;
		case 'G':
			SetProfileProperty("", "BaseColor", 0x00FF00);
			return TRUE;
		case 'B':
			SetProfileProperty("", "BaseColor", 0x0000FF);
			return TRUE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::IncProfileProperty(LPCSTR lpProfile, LPCSTR lpProfileProperty, LONG dwIncrement)
{
	if (m_profileInterface.IsInitialized())
	{
		m_profileInterface.LoadProfile(lpProfile);

		LONG dwProperty = 0;

		if (m_profileInterface.GetProfileProperty(lpProfileProperty, (LPBYTE)&dwProperty, sizeof(dwProperty)))
		{
			dwProperty += dwIncrement;

			m_profileInterface.SetProfileProperty(lpProfileProperty, (LPBYTE)&dwProperty, sizeof(dwProperty));
			m_profileInterface.SaveProfile(lpProfile);
			m_profileInterface.UpdateProfiles();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRTSSSharedMemorySampleDlg::SetProfileProperty(LPCSTR lpProfile, LPCSTR lpProfileProperty, DWORD dwProperty)
{
	if (m_profileInterface.IsInitialized())
	{
		m_profileInterface.LoadProfile(lpProfile);
		m_profileInterface.SetProfileProperty(lpProfileProperty, (LPBYTE)&dwProperty, sizeof(dwProperty));
		m_profileInterface.SaveProfile(lpProfile);
		m_profileInterface.UpdateProfiles();
	}
}
/////////////////////////////////////////////////////////////////////////////
