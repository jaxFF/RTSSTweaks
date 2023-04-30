// OverlayDataSourceInternal.cpp : implementation file
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "OverlayDataSourceInternal.h"
#include "Overlay.h"

#include <float.h>
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceInternal::COverlayDataSourceInternal()
{
}
/////////////////////////////////////////////////////////////////////////////
COverlayDataSourceInternal::~COverlayDataSourceInternal()
{
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceInternal::GetProviderName()
{
	return PROVIDER_INTERNAL;
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceInternal::Save()
{	
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceInternal::Load()
{
}
/////////////////////////////////////////////////////////////////////////////
void COverlayDataSourceInternal::Copy(COverlayDataSource* lpSrc)
{
	COverlayDataSource::Copy(lpSrc);
}
/////////////////////////////////////////////////////////////////////////////
BOOL COverlayDataSourceInternal::IsEqual(COverlayDataSource* lpSrc)
{
	if (!COverlayDataSource::IsEqual(lpSrc))
		return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceInternal::GetDataStr()
{
	if (!strcmp(m_strName, INTERNAL_SOURCE_FRAMERATE_HISTORY) ||
		!strcmp(m_strName, INTERNAL_SOURCE_FRAMETIME_HISTORY))
		return COverlayDataSource::GetDataStr();

	return GetDataTag();
}
/////////////////////////////////////////////////////////////////////////////
CString COverlayDataSourceInternal::GetDataTag()
{
	if (!strcmp(m_strName, INTERNAL_SOURCE_FRAMERATE))
		return "<FR>";
	if (!strcmp(m_strName, INTERNAL_SOURCE_FRAMETIME))
		return "<FT>";
	if (!strcmp(m_strName, INTERNAL_SOURCE_FRAMERATE_MIN))
		return "<FRMIN>";
	if (!strcmp(m_strName, INTERNAL_SOURCE_FRAMERATE_AVG))
		return "<FRAVG>";
	if (!strcmp(m_strName, INTERNAL_SOURCE_FRAMERATE_MAX))
		return "<FRMAX>";
	if (!strcmp(m_strName, INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW))
		return "<FR10L>";
	if (!strcmp(m_strName, INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW))
		return "<FR01L>";

	return COverlayDataSource::GetDataTag();
}
/////////////////////////////////////////////////////////////////////////////
