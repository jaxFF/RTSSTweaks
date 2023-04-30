// OverlayDataProviderAIDA.h: interface for the COverlayDataProviderAIDA class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataProvider.h"
#include "AIDA64Parser.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataProviderAIDA : public COverlayDataProvider, public CAIDA64Parser
{
public:
	COverlayDataProviderAIDA();
	virtual ~COverlayDataProviderAIDA();

	//COverlayDataProvider virtual functions

	virtual void UpdateSources(DWORD dwTimestamp);

	//CAIDA64Parser virtual functions

	virtual void ParseSensor(LPCSTR lpType, LPCSTR lpID, LPCSTR lpLabel, LPCSTR lpValue, DWORD dwContext);

protected:
	void UpdateSource(LPCSTR lpID, LPCSTR lpValue);	
		//updates all registered data sources with matching ID
};
/////////////////////////////////////////////////////////////////////////////
