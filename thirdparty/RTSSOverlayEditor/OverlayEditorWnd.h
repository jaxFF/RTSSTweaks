// OverlayEditorWnd.h: interface for the COverlayEditorWnd class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "RTSSHooksInterface.h"
#include "Overlay.h"
#include "OverlayUndoStack.h"
#include "OverlayLayersDlg.h"

#include <d3d9.h>
/////////////////////////////////////////////////////////////////////////////
// COverlayEditorWnd
/////////////////////////////////////////////////////////////////////////////
struct SOLIDVERTEX
{
    float			x, y, z, h;
    D3DCOLOR		color;
};
/////////////////////////////////////////////////////////////////////////////
#define D3DFVF_SOLIDVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
/////////////////////////////////////////////////////////////////////////////
const UINT UM_SHOW_HYPERTEXT	= ::RegisterWindowMessage("UM_SHOW_HYPERTEXT"	);
const UINT UM_UPDATE_STATUS		= ::RegisterWindowMessage("UM_UPDATE_STATUS"	);
const UINT UM_SAVE_LAST_STATE	= ::RegisterWindowMessage("UM_SAVE_LAST_STATE"	);
/////////////////////////////////////////////////////////////////////////////
class CLayerSettingsDlg;
class COverlayEditorWnd : public CWnd
{
	DECLARE_DYNAMIC(COverlayEditorWnd)

public:
	COverlayEditorWnd();
	virtual ~COverlayEditorWnd();

	void SetOverlay(COverlay* lpOverlay);
		//sets overlay ptr
	void SaveLastState();
		//saves last applied overlay state

	BOOL InitRenderer();	
		//initializes Direct3D renderer
	void ResetRenderer(int nWidth, int nHeight);
		//resets rendered state on device lost events or on window resize
	void Render(BOOL bUpdateLayerRects = TRUE);
		//renders editor workspace and optionally updates hittest rects for the layers
	void UninitRenderer();
		//uninitializes renderer
	void InitGridVB();
		//initializes grid vertext buffer
	void UninitGridVB();
		//uninitializes grid vertex buffer
	void RenderCheckerboard();
		//renders checkerboard 
	void RenderFrame(int left, int top, int right, int bottom, DWORD color);
		//renders frame rect
	void RenderSolidRect(int left, int top, int right, int bottom, DWORD color);
		//renders solid rect

	void PauseRendering(BOOL bPause);
		//pauses rendering
	BOOL IsRenderingPaused();
		//checks if rendering is paused
	void EnableRenderGrid(BOOL bEnable);
		//enables grid rendering
	BOOL IsRenderGridEnabled();
		//checks if grid rendering is enabled
	void EnableSnapToGrid(BOOL bEnable);
		//enables snap to grid 
	BOOL IsSnapToGridEnabled();
		//checks if snap to grid is enabled
	BOOL IsSnapToGridActive();
		//checks if snap to grid is active (<SHIFT> button can be used to toggle snap to grid mode dynamically while resizing/moving layers)

	void EnableRenderCheckerboard(BOOL bEnable);
		//enabled checkerboard rendering
	BOOL IsRenderCheckerboardEnabled();
		//checks if checkerboard rendering is enabled
	
	void ResizeToSelectionRect(COverlayLayer* lpLayer);
		//resizes layer to selection rect
	void MoveLayer(COverlayLayer* lpLayer, int nPositionX, int nPositionY);
		//moves layer to new position
	void MoveSelection(int nPositionX, int nPositionY);
		//moves group of selected layers to new position

	void SaveUndo(BOOL bReset = FALSE);
		//saves undo step
	int GetUndoStackSize();
		//returns undo stack depth
	int GetRedoStackSize();
		//returns redo stack depth
	void UpdateStatus();
		//updates status bar
	void UpdateLayersDlg();
		//updates modeless layers list dialog

	void ReleaseCapture();
		//releases mouse capture and ends selection or layer move operation

	void OnNew();
		//creates new overlay
	void OnEditOverlay();
		//edits overlay properties
	void OnLoad();
		//loads overlay
	void OnSave();
		//saves overlay
	void OnSaveAs();
		//saves overlay under new filename
	void OnImport();
		//imports overlay and its dependent files
	void OnExport();
		//exports overlay and its dependent files
	BOOL SavePrompt();
		//display save prompt on closing window

	void OnUndo();
		//undoes changes
	void OnRedo();
		//redoes changes

	void OnCopy();
		//copies focused layer to clipboard
	void OnPaste();
		//pastes new layer from clipboard

	void OnPositionUp();
		//moves focused layer position up
	void OnPositionDown();
		//moves focused layer position down
	void OnPositionLeft();
		//moves focused layer position left
	void OnPositionRight();
		//moves focused layer position right

	void OnMoveToTop();
		//moves focused layer to top of Z-order
	void OnMoveUp();
		//moves focused layer up in Z-order
	void OnMoveDown();
		//moves focused layer down in Z-order
	void OnMoveToBottom();
		//moves focused layer to bottom of Z-order
	
	void OnSetFocused(COverlayLayer* lpLayer);
		//sets focused layer
	void OnNextFocused();
		//selects the next focused layer
	void OnPrevFocused();
		//selects the previous focused layer

	void OnReorder(CList<COverlayLayer*, COverlayLayer*>* lpOrder);
		//reorders layers

	void OnSetupSources();
		//displays data soruces setup dialog

	void OnSetupTables();
		//displays tables setup dialog

	void OnAddLayer();
		//adds new layer
	void OnRemoveLayer();
		//removes focused layer
	void OnEditLayer(COverlayLayer* lpLayer = NULL);
		//edits layer
	void OnApplyLayerSettings(COverlayLayer* lpLayer, CLayerSettingsDlg* lpDlg);
		//applies layer settings
	void OnEditLayersList();
		//edits layers list Z-order

	void OnOptimize();
		//enables/disables hypertext optimization
	
		//NOTE: when optimization is enabled, the following trick are applied to hypertext formatting:

		//- Color, size and alignment tags are using attribute libraries to define variables for multiple used attributes, header includes
		//  variable definitions and attributes use variables instead of explicit color, size or alignment tags
		//- Embedded object tags do not specify the offset explicitly and look like <OBJ> instead of <OBJ=00100>, in this case RTSS hypertext
		//  parser simply parses embedded objects one by one since the very beginning of the buffer
	BOOL	IsOptimized();
		//return TRUE if hypertext 

	int Scale(int x);
		//scales client value to view value
	int Unscale(int x);
		//unscales view value to client value
	void ClientToView(CPoint* lpPoint);
		//converts point from client to view coordinates
	void AdjustViewScale(int nDelta);
		//adjusts view scale 
	void ResetViewScale();
		//resets view scale

	DWORD GetHitTestDepth();
		//returns hittesting depth, defined by currently pressed <1> ... <9> keys

protected:
	DECLARE_MESSAGE_MAP()

	COverlay*				m_lpOverlay;
	COverlayLayer			m_clipboard;
	COverlayUndoStack		m_undoStack;
	COverlayUndoStack		m_redoStack;

	BOOL					m_bPauseRendering;
	BOOL					m_bRenderGrid;
	BOOL					m_bRenderCheckerboard;
	BOOL					m_bSnapToGrid;

	BOOL					m_bOptimize;

	LPDIRECT3D9				m_lpd3d; 
	LPDIRECT3DDEVICE9		m_lpd3dDevice; 
	D3DPRESENT_PARAMETERS	m_d3dpp;

	LONG					m_dwPixelWeight;
	LONG					m_dwCharWidth;
	LONG					m_dwCharHeight;

	DWORD					m_dwGridColor;
	DWORD					m_dwBgndColor;
	DWORD					m_dwFocusedColor;
	DWORD					m_dwSelectionColor;

	DWORD					m_dwGridVBSize;
	SOLIDVERTEX*			m_lpGridVB;

	CRTSSHooksInterface		m_rtssInterface;

	BOOL					m_bCaptured;
	BOOL					m_bModified;
	int						m_nCapturedX;
	int						m_nCapturedY;
	DWORD					m_dwCapturedMask;
	CRect					m_rcCapturedRect;

	BOOL					m_bSelection;
	int						m_nSelectionX;
	int						m_nSelectionY;
	CRect					m_rcSelectionRect;

	BOOL					m_bUndoableKeyDown;

	HCURSOR					m_hCursorSizeNESW;
	HCURSOR					m_hCursorSizeNS;
	HCURSOR					m_hCursorSizeNWSE;
	HCURSOR					m_hCursorSizeWE;
	HCURSOR					m_hCursorHand;

	BOOL					m_bCapturedView;
	int						m_nCapturedViewX;
	int						m_nCapturedViewY;
	int						m_nViewX;
	int						m_nViewY;
	int						m_nViewScale;
	int						m_nViewScaleTarget;
	int						m_nViewScaleStep;

	COverlayLayersDlg*		m_lpLayersDlg;

public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
};
/////////////////////////////////////////////////////////////////////////////
