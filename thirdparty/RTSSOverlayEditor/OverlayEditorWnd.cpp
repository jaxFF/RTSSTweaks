// OverlayEditorWnd.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayEditor.h"
#include "OverlayEditorWnd.h"
#include "OverlayEditorDlg.h"
#include "RTSSSharedMemoryInterface.h"
#include "LayerSettingsDlg.h"
#include "OverlayDataSourcesDlg.h"
#include "TextTablesDlg.h"
#include "OverlaySettingsDlg.h"
#include "OverlayLayersDlg.h"
/////////////////////////////////////////////////////////////////////////////
// COverlayEditorWnd
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COverlayEditorWnd, CWnd)
/////////////////////////////////////////////////////////////////////////////
#define SAFE_RELEASE(ptr) if (ptr) { ptr->Release(); ptr = NULL; } 
/////////////////////////////////////////////////////////////////////////////
COverlayEditorWnd::COverlayEditorWnd()
{
	m_lpOverlay			= NULL;

	m_lpd3d				= NULL; 
	m_lpd3dDevice		= NULL; 

	m_dwPixelWeight		= 0;
	m_dwCharWidth		= 0;
	m_dwCharHeight		= 0;

	m_dwGridColor		= 0x10FFFFFF;
	m_dwBgndColor		= 0x00000000;
	m_dwFocusedColor	= 0x80808080;
	m_dwSelectionColor	= 0x80404080;

	m_dwGridVBSize		= 0;
	m_lpGridVB			= NULL;

	m_bCaptured			= FALSE;
	m_bModified			= FALSE;
	m_nCapturedX		= 0;
	m_nCapturedY		= 0;
	m_dwCapturedMask	= 0;
	m_rcCapturedRect	= CRect(0,0,0,0);

	m_bSelection		= FALSE;
	m_nSelectionX		= 0;
	m_nSelectionY		= 0;
	m_rcSelectionRect	= CRect(0,0,0,0);

	m_bUndoableKeyDown	= FALSE;

	m_hCursorSizeNESW	= LoadCursor(NULL, IDC_SIZENESW	);
	m_hCursorSizeNS		= LoadCursor(NULL, IDC_SIZENS	);
	m_hCursorSizeNWSE	= LoadCursor(NULL, IDC_SIZENWSE	);
	m_hCursorSizeWE		= LoadCursor(NULL, IDC_SIZEWE	);
	m_hCursorHand		= LoadCursor(NULL, IDC_HAND		);

	m_bOptimize			= TRUE;

	m_bCapturedView		= FALSE;
	m_nCapturedViewX	= 0;
	m_nCapturedViewY	= 0;

	m_nViewX			= 0;
	m_nViewY			= 0;
	m_nViewScale		= 100;
	m_nViewScaleTarget	= 100;
	m_nViewScaleStep	= 1;

	m_bPauseRendering	= FALSE;
	m_bRenderGrid		= TRUE;
	m_bRenderCheckerboard = FALSE;
	m_bSnapToGrid		= TRUE;

	m_lpLayersDlg		= NULL;	
}
/////////////////////////////////////////////////////////////////////////////
COverlayEditorWnd::~COverlayEditorWnd()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COverlayEditorWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETDLGCODE()
	ON_WM_SETCURSOR()
	ON_WM_KEYUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COverlayEditorWnd message handlers
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::SetOverlay(COverlay* lpOverlay)
{
	m_lpOverlay = lpOverlay;

	SaveUndo(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::InitRenderer()
{
	UninitRenderer();

	m_lpd3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_lpd3d)
		return FALSE;

	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
	m_d3dpp.Windowed				= TRUE;
	m_d3dpp.SwapEffect				= D3DSWAPEFFECT_COPY;
	m_d3dpp.BackBufferFormat		= D3DFMT_UNKNOWN;
	m_d3dpp.EnableAutoDepthStencil	= TRUE;
	m_d3dpp.AutoDepthStencilFormat	= D3DFMT_D16;
	m_d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	m_d3dpp.MultiSampleType			= D3DMULTISAMPLE_NONE;

	if (FAILED(m_lpd3d->CreateDevice(0, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_lpd3dDevice)))
	{
		SAFE_RELEASE(m_lpd3d);

		return FALSE;
	}

	m_lpd3dDevice->SetRenderState(D3DRS_SRCBLEND			, D3DBLEND_SRCALPHA);
	m_lpd3dDevice->SetRenderState(D3DRS_DESTBLEND			, D3DBLEND_INVSRCALPHA);
	m_lpd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE	, TRUE);

	m_rtssInterface.InitPreview("");

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::Render(BOOL bUpdateLayerRects)
{
	if (m_bPauseRendering)
		return;

	//animate view scale change

	if (m_nViewScale < m_nViewScaleTarget)
	{
		m_nViewScale += m_nViewScaleStep;

		if (m_nViewScale > m_nViewScaleTarget)
			m_nViewScale = m_nViewScaleTarget;
	}

	if (m_nViewScale > m_nViewScaleTarget)
	{
		m_nViewScale -= m_nViewScaleStep;

		if (m_nViewScale < m_nViewScaleTarget)
			m_nViewScale = m_nViewScaleTarget;
	}

	HRESULT err;

	if (m_lpd3dDevice)
	{
		m_lpd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, m_dwBgndColor, 1.0f, 0);

		//try to init grid vertex buffer

		//NOTE: We need at least one OSD preview frame to be rendered by RTSSHooks so it can report back char width/height, 
		//which we use as a grid cell size

		if (m_bRenderGrid)
			InitGridVB();

		if (SUCCEEDED(m_lpd3dDevice->BeginScene()))
		{
			m_lpd3dDevice->SetFVF(D3DFVF_SOLIDVERTEX);

			//render grid if we've already initialized grid vertex buffer 

			if (m_bRenderCheckerboard)
				RenderCheckerboard();

			if (m_bRenderGrid)
			{
				if (m_dwGridVBSize && m_lpGridVB)
					m_lpd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_dwGridVBSize, m_lpGridVB, sizeof(SOLIDVERTEX));
			}

			m_rtssInterface.RenderPreviewBeginD3D9(m_lpd3dDevice);	
				//render actual overlay on top of our D3D9 framebuffer via RTSSHooks OSD preview interface

			//once OSD preview is rendered by RTSSHooks, we can request back preview statistics:
			//
			// - pixel weight for preciese OSD postioning 
			// - char width/height for our grid rendering
			// - layer rects in screen coordinates for layers hittesting, bounding frames and focus rect rendering

			m_rtssInterface.GetPreviewStats(PREVIEWSTATS_PIXEL_WEIGHT	, 0, (LPBYTE)&m_dwPixelWeight	, sizeof(LONG));
			m_rtssInterface.GetPreviewStats(PREVIEWSTATS_CHAR_WIDTH		, 0, (LPBYTE)&m_dwCharWidth		, sizeof(LONG));
			m_rtssInterface.GetPreviewStats(PREVIEWSTATS_CHAR_HEIGHT	, 0, (LPBYTE)&m_dwCharHeight	, sizeof(LONG));

			if (bUpdateLayerRects)
			{
				if (m_lpOverlay)
				{
					POSITION pos = m_lpOverlay->GetHeadPosition();

					while (pos)
					{
						COverlayLayer* lpLayer = m_lpOverlay->GetNext(pos);

						m_rtssInterface.GetPreviewStats(PREVIEWSTATS_LAYER_RECT, lpLayer->GetLayerID(), (LPBYTE)&lpLayer->m_rect, sizeof(RECT));
					}
				}
			}

			//now we can render bounding frames for all layers 

			if (m_lpOverlay)
			{
				POSITION pos = m_lpOverlay->GetHeadPosition();

				while (pos)
				{
					COverlayLayer* lpLayer = m_lpOverlay->GetNext(pos);

					RenderFrame(lpLayer->m_rect.left, lpLayer->m_rect.top, lpLayer->m_rect.right, lpLayer->m_rect.bottom, m_dwFocusedColor);

					if (lpLayer->m_bSelected)
						RenderSolidRect(lpLayer->m_rect.left, lpLayer->m_rect.top, lpLayer->m_rect.right, lpLayer->m_rect.bottom, m_dwSelectionColor);
				}

				//highlight selection rect

				if (m_bSelection)
					RenderSolidRect(m_rcSelectionRect.left, m_rcSelectionRect.top, m_rcSelectionRect.right, m_rcSelectionRect.bottom, m_dwSelectionColor);

				//highlight currently focused layer

				COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

				if (lpFocusedLayer)
					RenderSolidRect(lpFocusedLayer->m_rect.left, lpFocusedLayer->m_rect.top, lpFocusedLayer->m_rect.right, lpFocusedLayer->m_rect.bottom, m_dwFocusedColor);
			}

			m_lpd3dDevice->EndScene();
		}

		//now our framebuffer contains: our own rendered grid + overlay preview rendered on top of it by RTSSHooks + our bounding frames 
		//and focus trect rendered on top of it by us. We can present it now!

		CRect cr;
		GetClientRect(&cr);

		//clip view rect to ensure that it is covering entire client rect

		if (m_nViewX < 0)
			m_nViewX = 0;
		if (m_nViewX > cr.Width() - Scale(cr.Width()))
			m_nViewX = cr.Width() - Scale(cr.Width());

		if (m_nViewY < 0)
			m_nViewY = 0;
		if (m_nViewY > cr.Height() - Scale(cr.Height()))
			m_nViewY = cr.Height() - Scale(cr.Height());

		CRect srcRect(m_nViewX, m_nViewY, m_nViewX + Scale(cr.Width()), m_nViewY + Scale(cr.Height()));

		err = m_lpd3dDevice->Present(&srcRect, NULL, NULL, NULL);

		//finalize preview rendering API, framerate/frametime statistics is collected by RTSSHooks inside this call so we invoke it
		//to ensure that our FPS and framertime overlay is working properly

		if (SUCCEEDED(err))
			m_rtssInterface.RenderPreviewEnd();

		if ((err == D3DERR_DEVICELOST) || 
			(err = D3DERR_INVALIDCALL))
			//restore lost device
		{
			if (m_lpd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				ResetRenderer(cr.Width(), cr.Height());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::UninitRenderer()
{
	m_rtssInterface.CleanupPreview();

	UninitGridVB();

	SAFE_RELEASE(m_lpd3dDevice);
	SAFE_RELEASE(m_lpd3d);

	if (!IsWindow(g_clientWnd.GetSafeHwnd()))
	{
		CRTSSSharedMemoryInterface sharedMemoryInterface;
		sharedMemoryInterface.ReleaseOSD(OSD_OWNER_NAME);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnPaint()
{
	CPaintDC dc(this); 

	Render();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::InitGridVB()
{
	CRect cr;
	GetClientRect(&cr);

	int w	= cr.Width();
	int h	= cr.Height();

	int nx	= m_dwCharWidth		? (1 + (w / m_dwCharWidth	)) : 0;
	int ny	= m_dwCharHeight	? (1 + (h / m_dwCharHeight	)) : 0;

	DWORD dwPrimitives = nx + ny;

	if (m_dwGridVBSize != dwPrimitives)
	{
		UninitGridVB();

		if (dwPrimitives)
		{
			m_dwGridVBSize		= dwPrimitives;
			m_lpGridVB			= new SOLIDVERTEX[dwPrimitives * 2];

			SOLIDVERTEX* lpVertices = m_lpGridVB;

			for (int x=0; x<w; x+=m_dwCharWidth)
			{
				lpVertices[0].x		= (float)x;
				lpVertices[0].y		= 0;
				lpVertices[0].z		= 0;
				lpVertices[0].h		= 1;
				lpVertices[0].color	= m_dwGridColor;

				lpVertices[1].x		= (float)x;
				lpVertices[1].y		= (float)h;
				lpVertices[1].z		= 0;
				lpVertices[1].h		= 1;
				lpVertices[1].color	= m_dwGridColor;

				lpVertices			+= 2;
			}
			
			for (int y=0; y<h; y+=m_dwCharHeight)
			{
				lpVertices[0].x		= 0;
				lpVertices[0].y		= (float)y;
				lpVertices[0].z		= 0;
				lpVertices[0].h		= 1;
				lpVertices[0].color	= m_dwGridColor;

				lpVertices[1].x		= (float)w;
				lpVertices[1].y		= (float)y;
				lpVertices[1].z		= 0;
				lpVertices[1].h		= 1;
				lpVertices[1].color	= m_dwGridColor;

				lpVertices			+= 2;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::UninitGridVB()
{
	if (m_lpGridVB)
	{
		delete [] m_lpGridVB;

		m_lpGridVB = NULL;
	}

	m_dwGridVBSize = 0;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::ResetRenderer(int nWidth, int nHeight)
{
	m_rtssInterface.CleanupPreview();

	if (m_lpd3dDevice)
	{
		m_d3dpp.BackBufferWidth		= nWidth;
		m_d3dpp.BackBufferHeight	= nHeight;

		m_lpd3dDevice->Reset(&m_d3dpp);

		m_lpd3dDevice->SetRenderState(D3DRS_SRCBLEND			, D3DBLEND_SRCALPHA);
		m_lpd3dDevice->SetRenderState(D3DRS_DESTBLEND			, D3DBLEND_INVSRCALPHA);
		m_lpd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE	, TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	ResetRenderer(cx, cy);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::RenderSolidRect(int left, int top, int right, int bottom, DWORD color)
{
	if (m_lpd3dDevice)
	{
		SOLIDVERTEX vertices[4];

		vertices[0].x		= (float)right;
		vertices[0].y		= (float)top;
		vertices[0].z		= 0;
		vertices[0].h		= 1;
		vertices[0].color	= color;

		vertices[1].x		= (float)right;
		vertices[1].y		= (float)bottom;
		vertices[1].z		= 0;
		vertices[1].h		= 1;
		vertices[1].color	= color;

		vertices[2].x		= (float)left;
		vertices[2].y		= (float)top;
		vertices[2].z		= 0;
		vertices[2].h		= 1;
		vertices[2].color	= color;

		vertices[3].x		= (float)left;
		vertices[3].y		= (float)bottom;
		vertices[3].z		= 0;
		vertices[3].h		= 1;
		vertices[3].color	= color;

		m_lpd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(SOLIDVERTEX));
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::RenderFrame(int left, int top, int right, int bottom, DWORD color)
{
	if (m_lpd3dDevice)
	{
		SOLIDVERTEX vertices[5];

		vertices[0].x		= (float)left;
		vertices[0].y		= (float)top;
		vertices[0].z		= 0;
		vertices[0].h		= 1;
		vertices[0].color	= color;

		vertices[1].x		= (float)right;
		vertices[1].y		= (float)top;
		vertices[1].z		= 0;
		vertices[1].h		= 1;
		vertices[1].color	= color;

		vertices[2].x		= (float)right;
		vertices[2].y		= (float)bottom;
		vertices[2].z		= 0;
		vertices[2].h		= 1;
		vertices[2].color	= color;

		vertices[3].x		= (float)left;
		vertices[3].y		= (float)bottom;
		vertices[3].z		= 0;
		vertices[3].h		= 1;
		vertices[3].color	= color;

		vertices[4].x		= (float)left;
		vertices[4].y		= (float)top;
		vertices[4].z		= 0;
		vertices[4].h		= 1;
		vertices[4].color	= color;

		m_lpd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vertices, sizeof(SOLIDVERTEX));
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint point0 = point;
	ClientToView(&point);

	BOOL bControl = (GetAsyncKeyState(VK_CONTROL) < 0);

	if (m_lpOverlay)
	{
		DWORD dwMask;

		COverlayLayer* lpLayer = m_lpOverlay->HitTest(point, GetHitTestDepth(), &dwMask);

		if (lpLayer)
		{
			if (bControl)
			{
				lpLayer->m_bSelected = !lpLayer->m_bSelected;

				Render();
			}
			else
			{
				m_lpOverlay->SetFocusedLayer(lpLayer);

				if (!lpLayer->m_bSelected)
					m_lpOverlay->SelectAllLayers(FALSE);

				if (!m_bCaptured)
				{	
					SetCapture();

					m_bCaptured			= TRUE;
					m_nCapturedX		= point.x - lpLayer->m_rect.left;
					m_nCapturedY		= point.y - lpLayer->m_rect.top;
					m_dwCapturedMask	= dwMask;
					m_rcCapturedRect	= lpLayer->m_rect;

					UpdateStatus();
					UpdateLayersDlg();

					Render();
				}
			}
		}

		if (!bControl)
		{
			if (!m_bCaptured)
			{
				if (!m_bSelection)
				{
					SetCapture();

					m_bSelection		= TRUE;
					m_nSelectionX		= point.x;
					m_nSelectionY		= point.y;
					m_rcSelectionRect	= CRect(0,0,0,0);
				}
			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point0);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bCaptured)
	{
		ReleaseCapture();

		Render();
	}

	if (m_bSelection)
	{
		if (m_lpOverlay)
			m_lpOverlay->SelectLayers(m_rcSelectionRect);

		ReleaseCapture();

		Render();
	}

	CWnd::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::ReleaseCapture()
{
	if (m_bCaptured || m_bSelection || m_bCapturedView)
	{
		if (m_bModified)
			SaveUndo();

		m_bCaptured		= FALSE;
		m_bModified		= FALSE;
		m_bSelection	= FALSE;
		m_bCapturedView	= FALSE;

		::ReleaseCapture();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::MoveSelection(int nPositionX, int nPositionY)
{
	int nMinX = 0;
	int nMinY = 0;

	if (m_lpOverlay)
	{
		BOOL bMoved = FALSE;

		COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

		if (lpFocusedLayer)
		{
			POSITION pos = m_lpOverlay->GetHeadPosition();

			int nMinSelectedX = lpFocusedLayer->m_rect.left;
			int nMinSelectedY = lpFocusedLayer->m_rect.top;

			while (pos)
			{
				COverlayLayer* lpLayer = m_lpOverlay->GetNext(pos);

				if (lpLayer->m_bSelected)
				{
					if (nMinSelectedX > lpLayer->m_rect.left)
						nMinSelectedX = lpLayer->m_rect.left;
					if (nMinSelectedY > lpLayer->m_rect.top)
						nMinSelectedY = lpLayer->m_rect.top;
				}
			}

			nMinX = lpFocusedLayer->m_rect.left - nMinSelectedX;
			nMinY = lpFocusedLayer->m_rect.top	- nMinSelectedY;

			if (nPositionX < nMinX)
				nPositionX = nMinX;
			if (nPositionY < nMinY)
				nPositionY = nMinY;

			MoveLayer(lpFocusedLayer, nPositionX, nPositionY);

			bMoved = TRUE;
		}

		if (bMoved)
		{
			POSITION pos = m_lpOverlay->GetHeadPosition();

			while (pos)
			{
				COverlayLayer* lpLayer = m_lpOverlay->GetNext(pos);

				if (lpLayer->m_bSelected && (lpLayer != lpFocusedLayer))
				{
					int nDeltaX = lpLayer->m_rect.left	- lpFocusedLayer->m_rect.left;
					int nDeltaY = lpLayer->m_rect.top	- lpFocusedLayer->m_rect.top;

					MoveLayer(lpLayer, nPositionX + nDeltaX, nPositionY + nDeltaY);
				}
			}

			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::MoveLayer(COverlayLayer* lpLayer, int nPositionX, int nPositionY)
{
	if (!IsSnapToGridActive())
	{
		nPositionX = nPositionX / m_dwPixelWeight;
		nPositionY = nPositionY / m_dwPixelWeight;
	}
	else
	{
		nPositionX = -nPositionX / m_dwCharWidth;
		nPositionY = -nPositionY / m_dwCharHeight;
	}

	lpLayer->m_nPositionX = nPositionX;
	lpLayer->m_nPositionY = nPositionY;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint point0 = point;
	ClientToView(&point);

	BOOL bRender = FALSE;

	if (m_bCapturedView)
	{
		m_nViewX = Scale(m_nCapturedViewX - point0.x);
		m_nViewY = Scale(m_nCapturedViewY - point0.y);

		bRender = TRUE;
	}

	if (m_bSelection)
	{
		m_rcSelectionRect = CRect(min(m_nSelectionX, point.x), min(m_nSelectionY, point.y), max(m_nSelectionX, point.x), max(m_nSelectionY, point.y));

		bRender = TRUE;
	}

	if (m_bCaptured)
	{
		m_bModified = TRUE;

		if (m_dwCapturedMask != HIT_TEST_MASK_BODY)
			//resize layer
		{
			BOOL bResize = TRUE;

			int nPositionX	= m_rcCapturedRect.left;
			int nPositionY	= m_rcCapturedRect.top;
			int nExtentX	= m_rcCapturedRect.Width();
			int nExtentY	= m_rcCapturedRect.Height();

			if (m_dwCapturedMask & HIT_TEST_MASK_LEFT)
			{
				nPositionX	= point.x;
				nExtentX	= m_rcCapturedRect.right - point.x;

				if ((nPositionX < 0) || (nExtentX <= 0))
					bResize = FALSE;
			}

			if (m_dwCapturedMask & HIT_TEST_MASK_RIGHT)
			{
				nExtentX	 = point.x - m_rcCapturedRect.left;

				if (nExtentX <= 0)
					bResize = FALSE;
			}

			if (m_dwCapturedMask & HIT_TEST_MASK_TOP)
			{
				nPositionY	= point.y;
				nExtentY	= m_rcCapturedRect.bottom - point.y;

				if ((nPositionY < 0) || (nExtentY <= 0))
					bResize = FALSE;
			}

			if (m_dwCapturedMask & HIT_TEST_MASK_BOTTOM)
			{
				nExtentY	 = point.y - m_rcCapturedRect.top;

				if (nExtentY <= 0)
					bResize = FALSE;
			}

			if (!IsSnapToGridActive())
			{
				int nPositionChunkX = nPositionX % m_dwPixelWeight;
				int nPositionChunkY = nPositionY % m_dwPixelWeight;

				nPositionX	= nPositionX / m_dwPixelWeight;
				nPositionY	= nPositionY / m_dwPixelWeight;

				nExtentX	= (nExtentX + nPositionChunkX) / m_dwPixelWeight;
				nExtentY	= (nExtentY + nPositionChunkY) / m_dwPixelWeight;
			}
			else
			{
				int nPositionChunkX = nPositionX % m_dwCharWidth;
				int nPositionChunkY = nPositionY % m_dwCharHeight;

				nPositionX = -nPositionX / m_dwCharWidth;
				nPositionY = -nPositionY / m_dwCharHeight;

				nExtentX = -(nExtentX + nPositionChunkX) / m_dwCharWidth;
				nExtentY = -(nExtentY + nPositionChunkY) / m_dwCharHeight;

				if (!nExtentX)
					nExtentX = -1;
				if (!nExtentY)
					nExtentY = -1;
			}

			if (!nExtentX || !nExtentY)
				bResize = FALSE;

			if (bResize)
			{
				if (m_lpOverlay)
				{
					COverlayLayer* lpLayer = m_lpOverlay->GetFocusedLayer();

					if (lpLayer)
					{

						lpLayer->m_nPositionX	= nPositionX;
						lpLayer->m_nExtentX		= nExtentX;

						lpLayer->m_nPositionY	= nPositionY;
						lpLayer->m_nExtentY		= nExtentY;
			
						m_lpOverlay->Update(FALSE, m_bOptimize);

						Render();
					}
				}
			}
		}
		else
			//move layer
			MoveSelection(point.x - m_nCapturedX, point.y - m_nCapturedY);

		UpdateStatus();
	}

	CWnd::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::RenderCheckerboard()
{
	CRect cr;
	GetClientRect(&cr);

	int nWidth	= cr.Width();
	int nHeight	= cr.Height();

	int nCellsX	= m_dwCharWidth		? (1 + (nWidth	/ m_dwCharWidth	)) : 0;
	int nCellsY	= m_dwCharHeight	? (1 + (nHeight	/ m_dwCharHeight)) : 0;

	int y = 0;

	for (int cy=0; cy<nCellsY; cy++)
	{
		int x = 0;

		for (int cx=0; cx<nCellsX; cx++)
		{
			if ((cx & 1) ^ (cy & 1))
				RenderSolidRect(x, y, x + m_dwCharWidth, y + m_dwCharHeight, m_dwGridColor);

			x += m_dwCharWidth;
		}

		y += m_dwCharHeight;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::SavePrompt()
{
	CString strFilename = m_lpOverlay->GetFilename();

	if (strFilename.IsEmpty() || (m_undoStack.GetCount() > 1))
	{	
		CString strMessage;
		strMessage.Format(LocalizeStr("Do you want to save changes in %s overlay layout?"), strFilename.IsEmpty() ? LocalizeStr("nameless") : strFilename);

		int nResult = MessageBox(strMessage, LocalizeStr("Overlay editor"), MB_YESNOCANCEL|MB_ICONQUESTION);

		if (nResult == IDCANCEL)
			return FALSE;

		if (nResult == IDYES)
		{
			if (strFilename.IsEmpty())
				OnSaveAs();
			else
				OnSave();
		}
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnNew()
{
	if (SavePrompt())
	{
		if (m_lpOverlay)
		{
			m_lpOverlay->New();

			SaveUndo(TRUE);

			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render();

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::IsOptimized()
{
	return m_bOptimize;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnOptimize()
{
	m_bOptimize = !m_bOptimize;

	if (m_lpOverlay)
	{
		m_lpOverlay->Update(FALSE, m_bOptimize);

		Render();

		UpdateStatus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnLoad()
{
	if (m_lpOverlay)
	{
		CFileDialog dlg(TRUE, "ovl", NULL, OFN_HIDEREADONLY, LocalizeStr("Overlay layout files (*.ovl)|*.ovl||"), AfxGetMainWnd());

		CString strInitialDir		= m_lpOverlay->GetCfgFolder();
		dlg.m_ofn.lpstrInitialDir	= strInitialDir;

		if (IDOK == dlg.DoModal())
		{
			m_lpOverlay->SetFilename(dlg.GetFileName());
			m_lpOverlay->Load();

			SaveUndo(TRUE);

			m_lpOverlay->Update(TRUE, m_bOptimize);

			Render();

			UpdateStatus();
			UpdateLayersDlg();

			SaveLastState();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnSave()
{
	if (m_lpOverlay)
	{
		m_lpOverlay->Save();

		SaveUndo(TRUE);

		UpdateStatus();

		SaveLastState();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnSaveAs()
{
	if (m_lpOverlay)
	{
		CFileDialog dlg(FALSE, "ovl", NULL, OFN_HIDEREADONLY, LocalizeStr("Overlay layout files (*.ovl)|*.ovl||"), AfxGetMainWnd());

		CString strInitialDir		= m_lpOverlay->GetCfgFolder();
		dlg.m_ofn.lpstrInitialDir	= strInitialDir;

		if (IDOK == dlg.DoModal())
		{
			m_lpOverlay->SetFilename(dlg.GetFileName());
			m_lpOverlay->Save();

			SaveUndo(TRUE);

			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render();

			UpdateStatus();

			SaveLastState();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnImport()
{
	if (m_lpOverlay)
	{
		CFileDialog dlg(TRUE, "ovx", NULL, OFN_HIDEREADONLY, LocalizeStr("Overlay layout exchange files (*.ovx)|*.ovx||"), AfxGetMainWnd());

		CString strInitialDir		= m_lpOverlay->GetCfgFolder();
		dlg.m_ofn.lpstrInitialDir	= strInitialDir;

		if (IDOK == dlg.DoModal())
		{
			m_lpOverlay->Import(dlg.GetFileName());

			SaveUndo(TRUE);

			m_lpOverlay->Update(TRUE, m_bOptimize);

			Render();

			UpdateStatus();
			UpdateLayersDlg();

			SaveLastState();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnExport()
{
	if (m_lpOverlay)
	{
		CFileDialog dlg(FALSE, "ovx", NULL, OFN_HIDEREADONLY, LocalizeStr("Overlay layout exchange files (*.ovx)|*.ovx||"), AfxGetMainWnd());

		CString strInitialDir		= m_lpOverlay->GetCfgFolder();
		dlg.m_ofn.lpstrInitialDir	= strInitialDir;

		if (IDOK == dlg.DoModal())
		{
			m_lpOverlay->Export(dlg.GetFileName());

			SaveUndo(TRUE);

			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render();

			UpdateStatus();

			SaveLastState();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnUndo()
{
	if (m_undoStack.GetCount() > 1)
	{
		if (m_undoStack.Pop(m_lpOverlay))
		{
			m_redoStack.Push(m_lpOverlay);
			m_undoStack.Peek(m_lpOverlay);

			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render();

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnRedo()
{
	if (m_redoStack.Pop(m_lpOverlay))
	{
		m_undoStack.Push(m_lpOverlay);

		m_lpOverlay->Update(FALSE, m_bOptimize);

		Render();

		UpdateStatus();
		UpdateLayersDlg();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnCopy()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
		m_clipboard.Copy(lpFocusedLayer);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnPaste()
{
	COverlayLayer* lpLayer = new COverlayLayer;

	lpLayer->Copy(&m_clipboard);

	if (m_bSelection)
		ResizeToSelectionRect(lpLayer);
	else
	{
		COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

		if (lpFocusedLayer)
		{
			int x = lpFocusedLayer->m_nPositionX;
			int y = lpFocusedLayer->m_nPositionY;

			if (x < 0)
				x--;
			else
				x += m_dwCharWidth / m_dwPixelWeight;

			if (y < 0)
				y--;
			else
				y += m_dwCharHeight / m_dwPixelWeight;

			lpLayer->m_nPositionX = x;
			lpLayer->m_nPositionY = y;
		}
	}

	m_lpOverlay->AddLayer(lpLayer);

	m_lpOverlay->SetFocusedLayer(lpLayer);

	m_lpOverlay->Update(FALSE, m_bOptimize);

	Render();

	UpdateStatus();
	UpdateLayersDlg();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnPositionUp()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (!IsSnapToGridActive())
			MoveSelection(lpFocusedLayer->m_rect.left, lpFocusedLayer->m_rect.top - m_dwPixelWeight);
		else
			MoveSelection(lpFocusedLayer->m_rect.left, lpFocusedLayer->m_rect.top - m_dwCharHeight);

		UpdateStatus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnPositionDown()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (!IsSnapToGridActive())
			MoveSelection(lpFocusedLayer->m_rect.left, lpFocusedLayer->m_rect.top + m_dwPixelWeight);
		else
			MoveSelection(lpFocusedLayer->m_rect.left, lpFocusedLayer->m_rect.top + m_dwCharHeight);

		UpdateStatus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnPositionLeft()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (!IsSnapToGridActive())
			MoveSelection(lpFocusedLayer->m_rect.left - m_dwPixelWeight, lpFocusedLayer->m_rect.top);
		else
			MoveSelection(lpFocusedLayer->m_rect.left - m_dwCharWidth, lpFocusedLayer->m_rect.top);

		UpdateStatus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnPositionRight()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (!IsSnapToGridActive())
			MoveSelection(lpFocusedLayer->m_rect.left + m_dwPixelWeight, lpFocusedLayer->m_rect.top);
		else
			MoveSelection(lpFocusedLayer->m_rect.left + m_dwCharWidth, lpFocusedLayer->m_rect.top);

		UpdateStatus();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnMoveToTop()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (m_lpOverlay->MoveLayerToTop(lpFocusedLayer))
		{
			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render(FALSE);

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnMoveToBottom()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (m_lpOverlay->MoveLayerToBottom(lpFocusedLayer))
		{
			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render(FALSE);

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnMoveUp()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (m_lpOverlay->MoveLayerUp(lpFocusedLayer))
		{
			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render(FALSE);

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnMoveDown()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (m_lpOverlay->MoveLayerDown(lpFocusedLayer))
		{
			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render(FALSE);

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnSetFocused(COverlayLayer* lpLayer)
{
	m_lpOverlay->SetFocusedLayer(lpLayer);

	m_lpOverlay->Update(FALSE, m_bOptimize);

	Render(FALSE);

	UpdateStatus();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnNextFocused()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (m_lpOverlay->SetNextFocusedLayer())
		{
			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render(FALSE);

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnPrevFocused()
{
	COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

	if (lpFocusedLayer)
	{
		if (m_lpOverlay->SetPrevFocusedLayer())
		{
			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render(FALSE);

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnReorder(CList<COverlayLayer*, COverlayLayer*>* lpOrder)
{
	m_lpOverlay->Reorder(lpOrder);

	m_lpOverlay->Update(FALSE, m_bOptimize);

	Render(FALSE);

	UpdateStatus();
	UpdateLayersDlg();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnSetupSources()
{
	COverlayDataSourcesDlg dlg;

	PauseRendering(TRUE);

	dlg.SetOverlay(m_lpOverlay);

	dlg.DoModal();

	PauseRendering(FALSE);

	SaveUndo();

	UpdateStatus();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnSetupTables()
{
	CTextTablesDlg dlg;

	dlg.SetOverlay(m_lpOverlay);

	dlg.DoModal();

	SaveUndo();

	UpdateStatus();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::ResizeToSelectionRect(COverlayLayer* lpLayer)
{
	CRect rcSelectionRect = m_rcSelectionRect;

	if (rcSelectionRect.left < 0)
		rcSelectionRect.left = 0;
	if (rcSelectionRect.top < 0)
		rcSelectionRect.top = 0;

	int nPositionX	= rcSelectionRect.left;
	int nPositionY	= rcSelectionRect.top;
	int nExtentX	= rcSelectionRect.Width();
	int nExtentY	= rcSelectionRect.Height();

	if (!IsSnapToGridActive())
	{
		int nPositionChunkX	= nPositionX % m_dwPixelWeight;
		int nPositionChunkY	= nPositionY % m_dwPixelWeight;
		
		nPositionX	= nPositionX / m_dwPixelWeight;
		nPositionY	= nPositionY / m_dwPixelWeight;

		nExtentX	= (nExtentX + nPositionChunkX) / m_dwPixelWeight;
		nExtentY	= (nExtentY + nPositionChunkY) / m_dwPixelWeight;
	}
	else
	{
		int nPositionChunkX	= nPositionX % m_dwCharWidth;
		int nPositionChunkY	= nPositionY % m_dwCharHeight;

		nPositionX	= - nPositionX / m_dwCharWidth;
		nPositionY	= - nPositionY / m_dwCharHeight;
		nExtentX	= - (nExtentX + nPositionChunkX + (m_dwCharWidth	/ 2)) / m_dwCharWidth;
		nExtentY	= - (nExtentY + nPositionChunkY + (m_dwCharHeight	/ 2)) / m_dwCharHeight;
	}

	lpLayer->m_nPositionX	= nPositionX;
	lpLayer->m_nPositionY	= nPositionY;
	lpLayer->m_nExtentX		= nExtentX;
	lpLayer->m_nExtentY		= nExtentY;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnAddLayer()
{
	if (m_lpOverlay)
	{
		COverlayLayer* lpLayer = new COverlayLayer;

		if (m_bSelection)
			ResizeToSelectionRect(lpLayer);
		else
		{
			lpLayer->m_strText = DEFAULT_TEXT;

			COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

			if (lpFocusedLayer)
			{
				int x = lpFocusedLayer->m_nPositionX;
				int y = lpFocusedLayer->m_nPositionY;

				if (x <= 0)
					x--;
				else
					x += m_dwCharWidth / m_dwPixelWeight;

				if (y <= 0)
					y--;
				else
					y += m_dwCharHeight / m_dwPixelWeight;

				lpLayer->m_nPositionX = x;
				lpLayer->m_nPositionY = y;
			}
		}

		m_lpOverlay->AddLayer(lpLayer);

		m_lpOverlay->SetFocusedLayer(lpLayer);

		m_lpOverlay->Update(FALSE, m_bOptimize);

		Render(FALSE);

		UpdateStatus();
		UpdateLayersDlg();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnRemoveLayer()
{
	if (m_lpOverlay)
	{
		BOOL bRemoved = FALSE;

		COverlayLayer* lpFocusedLayer = m_lpOverlay->GetFocusedLayer();

		if (lpFocusedLayer)
		{
			m_lpOverlay->RemoveLayer(lpFocusedLayer);

			bRemoved = TRUE;
		}

		POSITION pos = m_lpOverlay->GetTailPosition();

		while (pos)
		{
			COverlayLayer* lpLayer = m_lpOverlay->GetPrev(pos);

			if (lpLayer->m_bSelected)
			{
				m_lpOverlay->RemoveLayer(lpLayer);

				bRemoved = TRUE;
			}
		}

		if (bRemoved)
		{
			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render(FALSE);

			UpdateStatus();
			UpdateLayersDlg();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnEditOverlay()
{
	if (m_lpOverlay)
	{
		COverlaySettingsDlg dlg;

		dlg.SetOverlay(m_lpOverlay);

		dlg.m_strName			= m_lpOverlay->m_strName;
		dlg.m_nRefreshPeriod	= m_lpOverlay->m_dwRefreshPeriod;
		dlg.m_strEmbeddedImage	= m_lpOverlay->m_strEmbeddedImage;
		dlg.m_strPingAddr		= m_lpOverlay->m_strPingAddr;

		if (IDOK == dlg.DoModal())
		{
			m_lpOverlay->m_strName			= dlg.m_strName;
			m_lpOverlay->m_dwRefreshPeriod	= dlg.m_nRefreshPeriod;
			m_lpOverlay->m_strEmbeddedImage	= dlg.m_strEmbeddedImage;
			m_lpOverlay->m_strPingAddr		= dlg.m_strPingAddr;

			m_lpOverlay->Update(FALSE, m_bOptimize);

			Render();

			SaveUndo();

			UpdateStatus();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnEditLayer(COverlayLayer* lpLayer)
{
	if (m_lpOverlay)
	{
		if (!lpLayer)
			lpLayer = m_lpOverlay->GetFocusedLayer();

		if (lpLayer)
		{
			CLayerSettingsDlg dlg;

			dlg.SetOverlay(m_lpOverlay);
			dlg.SetLayer(lpLayer);
			dlg.SetEditorWnd(this);

			dlg.m_strName			= lpLayer->m_strName;
			dlg.m_nRefreshPeriod	= lpLayer->m_dwRefreshPeriod;
			dlg.m_strText			= lpLayer->m_strText;
			dlg.m_strTextColor		= lpLayer->m_strTextColor;
			dlg.m_strBgndColor		= lpLayer->m_strBgndColor;
			dlg.m_strBorderColor	= lpLayer->m_strBorderColor;
			dlg.m_nBorderSize		= lpLayer->m_nBorderSize;
			dlg.m_nAlignment		= lpLayer->m_nAlignment;
			dlg.m_bFixedAlignment	= lpLayer->m_bFixedAlignment;
			dlg.m_nSize				= lpLayer->m_nSize;
			dlg.m_nExtentX			= lpLayer->m_nExtentX;
			dlg.m_nExtentY			= lpLayer->m_nExtentY;
			dlg.m_nExtentOrigin		= lpLayer->m_nExtentOrigin;
			dlg.m_nPosSticky		= lpLayer->m_nPositionSticky;
			dlg.m_nMarginLeft		= lpLayer->m_nMarginLeft;
			dlg.m_nMarginTop		= lpLayer->m_nMarginTop;
			dlg.m_nMarginRight		= lpLayer->m_nMarginRight;
			dlg.m_nMarginBottom		= lpLayer->m_nMarginBottom;
					
			if (IDOK == dlg.DoModal())
				OnApplyLayerSettings(lpLayer, &dlg);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnApplyLayerSettings(COverlayLayer* lpLayer, CLayerSettingsDlg* lpDlg)
{
	if (lpLayer && lpDlg)
	{
		lpLayer->m_strName			= lpDlg->m_strName;
		lpLayer->m_dwRefreshPeriod	= lpDlg->m_nRefreshPeriod;
		lpLayer->m_strText			= lpDlg->m_strText;
		lpLayer->m_strTextColor		= lpDlg->m_strTextColor;
		lpLayer->m_strBgndColor		= lpDlg->m_strBgndColor;
		lpLayer->m_strBorderColor	= lpDlg->m_strBorderColor;
		lpLayer->m_nBorderSize		= lpDlg->m_nBorderSize;
		lpLayer->m_nExtentOrigin	= lpDlg->m_nExtentOrigin;
		lpLayer->m_nAlignment		= lpDlg->m_nAlignment;
		lpLayer->m_bFixedAlignment	= lpDlg->m_bFixedAlignment;
		lpLayer->m_nSize			= lpDlg->m_nSize;
		lpLayer->m_nExtentX			= lpDlg->m_nExtentX;
		lpLayer->m_nExtentY			= lpDlg->m_nExtentY;
		lpLayer->m_nExtentOrigin	= lpDlg->m_nExtentOrigin;
		lpLayer->m_nPositionSticky	= lpDlg->m_nPosSticky;
		lpLayer->m_nMarginLeft		= lpDlg->m_nMarginLeft;
		lpLayer->m_nMarginTop		= lpDlg->m_nMarginTop;
		lpLayer->m_nMarginRight		= lpDlg->m_nMarginRight;
		lpLayer->m_nMarginBottom	= lpDlg->m_nMarginBottom;

		m_lpOverlay->Update(FALSE, m_bOptimize);

		Render();

		SaveUndo();

		UpdateStatus();
		UpdateLayersDlg();
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnEditLayersList()
{
	if (m_lpOverlay)
	{
		if (m_lpLayersDlg)
			m_lpLayersDlg->ShowWindow(SW_SHOW);
		else
		{
			m_lpLayersDlg = new COverlayLayersDlg;

			m_lpLayersDlg->SetOverlay(m_lpOverlay);
			m_lpLayersDlg->SetEditorWnd(this);

			m_lpLayersDlg->Create(IDD_OVERLAY_LAYERS_DIALOG, this);
			m_lpLayersDlg->ShowWindow(SW_SHOW);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bCtrl	= (GetAsyncKeyState(VK_CONTROL	) < 0);
	BOOL bShift	= (GetAsyncKeyState(VK_SHIFT	) < 0);
	BOOL bAlt	= (GetAsyncKeyState(VK_MENU		) < 0);

	if (nChar == VK_ESCAPE)
	{
		GetParent()->PostMessage(WM_COMMAND, IDCANCEL);
	}
	else
	if (nChar == VK_ADD)
	{
		AdjustViewScale(50);
	}
	else
	if (nChar == VK_SUBTRACT)
	{
		AdjustViewScale(-50);
	}
	else
	if (nChar == 'M')
	{
		if (bCtrl && bShift)
			m_lpOverlay->OnMasterSettings();
	}
	else
	if (nChar == 'N')
	{
		if (bCtrl)
		{
			OnNew();
		}
	}
	else
	if (nChar == 'L')
	{
		if (bCtrl)
		{
			if (bShift)
				OnEditLayersList();
			else
				OnLoad();
		}
	}
	else
	if (nChar == 'O')
	{
		if (bCtrl)
		{
			if (bShift)
				OnOptimize();
			else
				OnLoad();
		}
	}
	else
	if (nChar == 'S')
	{
		if (bCtrl)
		{
			if (bShift || !strlen(m_lpOverlay->GetFilename()))
				OnSaveAs();
			else
				OnSave();
		}
	}
	else
	if (nChar == 'Z')
	{
		if (bCtrl)
		{
			if (bShift)
				OnRedo();
			else
				OnUndo();
		}
	}
	else
	if (nChar == 'Y')
	{
		if (bCtrl)
			OnRedo();
	}
	else
	if (nChar == 'C')
	{
		if (bCtrl)
		{
			OnCopy();

			m_bUndoableKeyDown = TRUE;
		}
	}
	else
	if (nChar == 'V')
	{
		if (bCtrl)
		{
			OnPaste();

			m_bUndoableKeyDown = TRUE;
		}
	}
	else
	if (nChar == VK_UP)
	{
		OnPositionUp();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_DOWN)
	{
		OnPositionDown();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_LEFT)
	{
		OnPositionLeft();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_RIGHT)
	{
		OnPositionRight();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_HOME)
	{
		OnMoveToTop();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_END)
	{
		OnMoveToBottom();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_PRIOR)
	{
		OnMoveUp();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_NEXT)
	{
		OnMoveDown();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_TAB)
	{
		if (bShift)
			OnPrevFocused();
		else
			OnNextFocused();
	}
	else
	if (nChar == 'H')
	{
		if (bCtrl && bShift)
		{
			CWnd* pParent = GetParent();

			if (pParent)
				pParent->PostMessage(UM_SHOW_HYPERTEXT);
		}
	}
	else
	if (nChar == 'D')
	{
		if (bCtrl)
			OnSetupSources();
	}
	else
	if (nChar == 'T')
	{
		if (bCtrl)
			OnSetupTables();
	}
	else
	if (nChar == VK_INSERT)
	{
		OnAddLayer();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_DELETE)
	{
		OnRemoveLayer();

		m_bUndoableKeyDown = TRUE;
	}
	else
	if (nChar == VK_RETURN)
	{
		if (bCtrl)
			OnEditOverlay();
		else
			OnEditLayer();
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CPoint point0 = point;
	ClientToView(&point);

	if (m_lpOverlay)
	{
		DWORD dwMask;

		COverlayLayer* lpLayer = m_lpOverlay->HitTest(point, 0, &dwMask);

		if (lpLayer)
			OnEditLayer(lpLayer);

	}

	CWnd::OnLButtonDblClk(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
UINT COverlayEditorWnd::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	ClientToView(&point);

	if (m_lpOverlay)
	{
		DWORD dwMask;

		if (m_lpOverlay->HitTest(point, 0, &dwMask))
		{
			if (dwMask & HIT_TEST_MASK_TOP)
			{
				if (dwMask & HIT_TEST_MASK_LEFT)
					::SetCursor(m_hCursorSizeNWSE);
				else
				if (dwMask & HIT_TEST_MASK_RIGHT)
					::SetCursor(m_hCursorSizeNESW);
				else
					::SetCursor(m_hCursorSizeNS);
				return TRUE;
			}

			if (dwMask & HIT_TEST_MASK_BOTTOM)
			{
				if (dwMask & HIT_TEST_MASK_LEFT)
					::SetCursor(m_hCursorSizeNESW);
				else
				if (dwMask & HIT_TEST_MASK_RIGHT)
					::SetCursor(m_hCursorSizeNWSE);
				else
					::SetCursor(m_hCursorSizeNS);
				return TRUE;
			}

			if (dwMask & HIT_TEST_MASK_LEFT)
			{
				::SetCursor(m_hCursorSizeWE);
				return TRUE;
			}

			if (dwMask & HIT_TEST_MASK_RIGHT)
			{
				::SetCursor(m_hCursorSizeWE);
				return TRUE;
			}
		}
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::SaveUndo(BOOL bReset)
{
	m_redoStack.Destroy();

	if (bReset)
		m_undoStack.Destroy();

	COverlay overlay;

	if (m_undoStack.Peek(&overlay))
	{
		if (!m_lpOverlay->IsEqual(&overlay))
			m_undoStack.Push(m_lpOverlay);
	}
	else
		m_undoStack.Push(m_lpOverlay);

	UpdateStatus();
}
/////////////////////////////////////////////////////////////////////////////
int COverlayEditorWnd::GetUndoStackSize()
{
	return m_undoStack.GetCount();
}
/////////////////////////////////////////////////////////////////////////////
int COverlayEditorWnd::GetRedoStackSize()
{
	return m_redoStack.GetCount();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::UpdateStatus()
{
	CWnd* pParent = GetParent();

	if (pParent)
		pParent->PostMessage(UM_UPDATE_STATUS);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::SaveLastState()
{
	CWnd* pParent = GetParent();

	if (pParent)
		pParent->SendMessage(UM_SAVE_LAST_STATE);	
			//we use SendMessage instead of PostMessage on purpose to allow it to be processsed when called form parent's OnOK/OnCancel
													
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_bUndoableKeyDown)
	{
		SaveUndo();

		m_bUndoableKeyDown = FALSE;
	}

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::ResetViewScale()
{
	AdjustViewScale(100 - m_nViewScale);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::AdjustViewScale(int nDelta)
{
	m_nViewScaleTarget += nDelta;

	if (m_nViewScaleTarget < 100)
		m_nViewScaleTarget = 100;
	if (m_nViewScaleTarget > 500)
		m_nViewScaleTarget = 500;

	m_nViewScaleStep = max(1, abs(m_nViewScaleTarget - m_nViewScale) / 15);

	Render();
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	AdjustViewScale(50 * zDelta / WHEEL_DELTA);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
/////////////////////////////////////////////////////////////////////////////
int COverlayEditorWnd::Scale(int x)
{
	return x * 100 / m_nViewScale;
}
/////////////////////////////////////////////////////////////////////////////
int COverlayEditorWnd::Unscale(int x)
{
	return x * m_nViewScale / 100;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::ClientToView(CPoint* lpPoint)
{
	lpPoint->x = Scale(lpPoint->x) + m_nViewX;
	lpPoint->y = Scale(lpPoint->y) + m_nViewY;
}
/////////////////////////////////////////////////////////////////////////////
DWORD COverlayEditorWnd::GetHitTestDepth()
{
	for (int i='1'; i<'9'; i++)
	{
		if (GetAsyncKeyState(i) < 0)
			return i - '1' + 1; 
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnRButtonDown(UINT nFlags, CPoint point)
{	
	if (!m_bCapturedView)
	{	
		SetCapture();

		SetCursor(m_hCursorHand);

		m_bCapturedView		= TRUE;
		m_nCapturedViewX	= Unscale(m_nViewX) + point.x;
		m_nCapturedViewY	= Unscale(m_nViewY) + point.y;

		Render();
	}

	CWnd::OnRButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_bCapturedView)
	{	
		ReleaseCapture();

		Render();
	}

	CWnd::OnRButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::PauseRendering(BOOL bPause)
{
	m_bPauseRendering = bPause;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::IsRenderingPaused()
{
	return m_bPauseRendering;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::EnableRenderGrid(BOOL bEnable)
{
	m_bRenderGrid = bEnable;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::IsRenderGridEnabled()
{
	return m_bRenderGrid;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::EnableSnapToGrid(BOOL bEnable)
{
	m_bSnapToGrid = bEnable;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::IsSnapToGridEnabled()
{
	return m_bSnapToGrid;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::IsSnapToGridActive()
{
	BOOL bSnapToGrid = IsSnapToGridEnabled();
	
	return GetAsyncKeyState(VK_SHIFT) ? !bSnapToGrid : bSnapToGrid;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::EnableRenderCheckerboard(BOOL bEnable)
{
	m_bRenderCheckerboard = bEnable;
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayEditorWnd::IsRenderCheckerboardEnabled()
{
	return m_bRenderCheckerboard;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::OnDestroy()
{
	if (m_lpLayersDlg)
	{
		m_lpLayersDlg->DestroyWindow();

		delete m_lpLayersDlg;

		m_lpLayersDlg = NULL;
	}

	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void COverlayEditorWnd::UpdateLayersDlg()
{
	if (m_lpLayersDlg)
		m_lpLayersDlg->Update();
}
/////////////////////////////////////////////////////////////////////////////
