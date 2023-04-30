// OverlayLayersDlg.h: interface for the COverlayDataSourcesDlg class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"
#include "CheckedListCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayLayersDlg dialog
/////////////////////////////////////////////////////////////////////////////
class COverlayEditorWnd;
class COverlayLayersDlg : public CDialog
{
	DECLARE_DYNAMIC(COverlayLayersDlg)

public:
	COverlayLayersDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COverlayLayersDlg();

	void SetOverlay(COverlay* lpOverlay);
	void SetEditorWnd(COverlayEditorWnd* lpEditorWnd);

	void SetButtonIcon(UINT nControlId, UINT nIconId);

	void InitLayersList();
	void UpdateButtons();
	void Update();

	COverlayLayer* GetItemLayer(int iItem);

	CCheckedListCtrl m_layersListCtrl;

// Dialog Data
	enum { IDD = IDD_OVERLAY_LAYERS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	COverlay*			m_lpOverlay;
	COverlayEditorWnd*	m_lpEditorWnd;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedMoveLayerToTopButton();
	afx_msg void OnBnClickedMoveLayerUpButton();
	afx_msg void OnBnClickedMoveLayerDownButton();
	afx_msg void OnBnClickedMoveLayerToBottomButton();
	afx_msg void OnBnClickedRemoveLayerButton();
};
/////////////////////////////////////////////////////////////////////////////
