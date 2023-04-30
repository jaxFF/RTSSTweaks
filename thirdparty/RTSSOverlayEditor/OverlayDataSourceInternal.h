// OverlayDataSourceInternal.h: interface for the COverlayDataSourceInternal class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataSource.h"
/////////////////////////////////////////////////////////////////////////////
#define INTERNAL_SOURCE_FRAMERATE						"Framerate"
#define INTERNAL_SOURCE_FRAMETIME						"Frametime"
#define INTERNAL_SOURCE_FRAMERATE_MIN					"Framerate Min"
#define INTERNAL_SOURCE_FRAMERATE_AVG					"Framerate Avg"
#define INTERNAL_SOURCE_FRAMERATE_MAX					"Framerate Max"
#define INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW		"Framerate 1% Low"
#define INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW		"Framerate 0.1% Low"
#define INTERNAL_SOURCE_FRAMERATE_HISTORY				"Framerate history"
#define INTERNAL_SOURCE_FRAMETIME_HISTORY				"Frametime history"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataSourceInternal : public COverlayDataSource
{
public:
	COverlayDataSourceInternal();
	virtual ~COverlayDataSourceInternal();

	//COverlayDataSource virtual functions

	virtual CString GetProviderName();
	virtual CString GetDataStr();
	virtual CString	GetDataTag();

	virtual void Save();
	virtual void Load();
	virtual void Copy(COverlayDataSource* lpSrc);
	virtual BOOL IsEqual(COverlayDataSource* lpSrc);
};
/////////////////////////////////////////////////////////////////////////////
