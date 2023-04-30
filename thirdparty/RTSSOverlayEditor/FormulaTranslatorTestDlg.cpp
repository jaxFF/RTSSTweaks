
// FormulaTranslatorTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FormulaTranslatorTest.h"
#include "FormulaTranslatorTestDlg.h"
#include "RTFCErrors.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFormulaTranslatorTestDlg dialog




CFormulaTranslatorTestDlg::CFormulaTranslatorTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFormulaTranslatorTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_strFormula = "ceil(1.6^2)+max(1000,2000)";
}

void CFormulaTranslatorTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FORMULA_EDIT, m_strFormula);
}

BEGIN_MESSAGE_MAP(CFormulaTranslatorTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CALC_BUTTON, &CFormulaTranslatorTestDlg::OnBnClickedCalcButton)
END_MESSAGE_MAP()


// CFormulaTranslatorTestDlg message handlers

BOOL CFormulaTranslatorTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_translator.SetVariableProvider(this);
	m_translator.Init();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFormulaTranslatorTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFormulaTranslatorTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFormulaTranslatorTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CFormulaTranslatorTestDlg::GetVariable(LPCSTR lpName, LPFORMULATRANSLATOR_VALUE_DESC pDesc)
{
	if (!(_stricmp(lpName, "x")))
	{
		float data = 1977.0f;

		pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
		pDesc->dwData = FLOAT_AS_LONG(data);

		return TRUE;
	}

	return FALSE;
}

CString CFormulaTranslatorTestDlg::GetErrStr(HRESULT err)
{
	CString strResult;
	strResult.Format("error %x", err);

	switch (err)
	{
	case LEXICALANALYZER_E_INVALID_CHAR:
		strResult += ", ";
		strResult += "invalid character";
		break;
	case FORMULATRANSLATOR_E_UNKNOWN_OPERATION:
		strResult += ", ";
		strResult += "unknown operation";
		break;
	case FORMULATRANSLATOR_E_UNEXPECTED_BRACKET:
		strResult += ", ";
		strResult += "unexpected bracket";
		break;
	case FORMULATRANSLATOR_E_BRACKET_EXPECTED:
		strResult += ", ";
		strResult += "bracket expected";
		break;
	case FORMULATRANSLATOR_E_UNDECLARED_VARIABLE:
		strResult += ", ";
		strResult += "undeclared variable";
		break;
	case FORMULATRANSLATOR_E_OPERAND_EXPECTED:
		strResult += ", ";
		strResult += "operand expected";
		break;
	case FORMULATRANSLATOR_E_OPERATION_EXPECTED:
		strResult += ", ";
		strResult += "operation expected";
		break;
	case FORMULATRANSLATOR_E_QUOTE_EXPECTED:
		strResult += ", ";
		strResult += "quote expected";
		break;
	case FORMULATRANSLATOR_E_UNDECLARED_FN:
		strResult += ", ";
		strResult += "undeclared function";
		break;
	case FORMULATRANSLATOR_E_UNEXPECTED_COMMA:
		strResult += ", ";
		strResult += "unexpected comma";
		break;
	}

	return strResult;
}

void CFormulaTranslatorTestDlg::OnBnClickedCalcButton()
{
	UpdateData(TRUE);

	CString strResult;

	HRESULT err = m_translator.Translate(m_strFormula);

	if (SUCCEEDED(err))
	{
		err = m_translator.Calc();

		if (SUCCEEDED(err))
		{
			FLOAT fltResult = m_translator.GetFltValue();

			strResult.Format("Formula\t : %s\nRPN\t : %s\nResult\t : %f", m_strFormula, (LPCSTR)m_translator.Format(), fltResult);
			MessageBox(strResult);
		}
		else
		{
			strResult.Format("Formula\t : %s\nRPN\t : %s\nResult\t : %s", m_strFormula, (LPCSTR)m_translator.Format(), GetErrStr(err));
			MessageBox(strResult);
		}
	}
	else
	{
		strResult.Format("Formula\t : %s\nRPN\t : %s", m_strFormula, GetErrStr(err));
		MessageBox(strResult);
	}
}
