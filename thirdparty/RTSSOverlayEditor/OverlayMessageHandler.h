// OverlayMessageHandler.h: interface for the COverlayMessageHandler class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
const UINT UM_OVERLAY_MESSAGE	= ::RegisterWindowMessage("UM_OVERLAY_MESSAGE");
const UINT UM_OVERLAY_LOADED	= ::RegisterWindowMessage("UM_OVERLAY_LOADED");
//////////////////////////////////////////////////////////////////////
class COverlayMessageHandler
{
public:
	COverlayMessageHandler();
	virtual ~COverlayMessageHandler();

	virtual void OnOverlayMessage(LPCSTR lpMessage, LPCSTR lpLayer, LPCSTR lpParams) = 0;
};
//////////////////////////////////////////////////////////////////////
