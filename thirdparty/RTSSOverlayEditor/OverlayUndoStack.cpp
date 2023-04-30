// OverlayUndoStack.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OverlayUndoStack.h"
//////////////////////////////////////////////////////////////////////
COverlayUndoStack::COverlayUndoStack()
{
}
//////////////////////////////////////////////////////////////////////
COverlayUndoStack::~COverlayUndoStack()
{
	Destroy();
}
//////////////////////////////////////////////////////////////////////
void COverlayUndoStack::Push(COverlay* lpOverlay)
{
	COverlay* lpCopy = new COverlay;

	lpCopy->Copy(lpOverlay);

	AddHead(lpCopy);

	while (GetCount() > OVERLAY_UNDO_STACK_DEPTH)
	{
		POSITION pos = GetTailPosition();

		if (pos)
		{
			COverlay* lpOverlay = GetAt(pos);

			delete lpOverlay;

			RemoveAt(pos);
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL COverlayUndoStack::Peek(COverlay* lpOverlay)
{
	POSITION pos = GetHeadPosition();

	if (pos)
	{
		COverlay* lpHead = GetAt(pos);

		lpOverlay->Copy(lpHead);

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL COverlayUndoStack::Pop(COverlay* lpOverlay)
{
	POSITION pos = GetHeadPosition();

	if (pos)
	{
		COverlay* lpHead = GetAt(pos);

		lpOverlay->Copy(lpHead);

		delete lpHead;

		RemoveAt(pos);

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void COverlayUndoStack::Destroy()
{
	POSITION pos = GetHeadPosition();

	while (pos)
	{
		COverlay* lpOverlay = GetNext(pos);

		delete lpOverlay;
	}

	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
