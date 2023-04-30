// OverlayEditorDlg.h: interface for the COverlayEditorDlg class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "OverlayEditorWnd.h"
#include "OverlayMessageHandler.h"
#include "RichEditCtrlEx.h"
#include "MMTimer.h"
#include "resource.h"
//////////////////////////////////////////////////////////////////////
// COverlayEditorDlg dialog
//////////////////////////////////////////////////////////////////////
class COverlayEditorDlg : public CDialog, public COverlayMessageHandler
{
	DECLARE_DYNAMIC(COverlayEditorDlg)

public:
	COverlayEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayEditorDlg();

	virtual void OnOverlayMessage(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams);

	void UpdateMenu();
	void OnSaveLastState();
	void OnOptimizeHypertext();
	void OnShowHypertext();
	void OnUpdateStatus();

	void RepositionChildren(int cx, int cy);
	
	COverlay m_overlay;
	COverlay m_overlayLast;

// Dialog Data
	enum { IDD = IDD_OVERLAY_EDITOR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	CMenu				m_mainMenu;
	COverlayEditorWnd	m_editorWnd;
	CRichEditCtrlEx		m_ctrlDebugView;
	CStatusBar			m_statusBar;
	CMMTimer			m_mmTimer;

	LARGE_INTEGER		m_pf;
	LARGE_INTEGER		m_pc;

	HICON				m_hIcon;

	BOOL				m_bShowHypertext;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};
//////////////////////////////////////////////////////////////////////
