#include "stdafx.h"
#include "OverlayStack.h"
//////////////////////////////////////////////////////////////////////
COverlayStack::COverlayStack()
{
}
//////////////////////////////////////////////////////////////////////
COverlayStack::~COverlayStack()
{
	Destroy();
}
//////////////////////////////////////////////////////////////////////
void COverlayStack::Push(COverlay& overlay)
{
	COverlay* pOverlay = new COverlay;

	pOverlay->Copy(&overlay);

	AddHead(pOverlay);

	while (GetCount() > OVERLAY_STACK_DEPTH)
	{
		POSITION pos = GetTailPosition();

		if (pos)
		{
			COverlay* pOverlay = GetAt(pos);

			delete pOverlay;

			RemoveAt(pos);
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL COverlayStack::Peek(COverlay& overlay)
{
	POSITION pos = GetHeadPosition();

	if (pos)
	{
		COverlay* pOverlay = GetAt(pos);

		overlay.Copy(pOverlay);

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL COverlayStack::Pop(COverlay& overlay)
{
	POSITION pos = GetHeadPosition();

	if (pos)
	{
		COverlay* pOverlay = GetAt(pos);

		overlay.Copy(pOverlay);

		delete pOverlay;

		RemoveAt(pos);

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void COverlayStack::Destroy()
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlay* pOverlay = GetNext(pos);

		delete pOverlay;
	}

	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
