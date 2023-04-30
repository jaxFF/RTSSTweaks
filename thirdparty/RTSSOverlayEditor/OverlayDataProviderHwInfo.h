// OverlayDataProviderHwInfo.h: interface for the COverlayDataProviderAIDA class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "OverlayDataProvider.h"
#include "HwInfoWrapper.h"
/////////////////////////////////////////////////////////////////////////////
class COverlayDataProviderHwInfo : public COverlayDataProvider
{
public:
	COverlayDataProviderHwInfo();
	virtual ~COverlayDataProviderHwInfo();

	//COverlayDataProvider virtual functions

	virtual void UpdateSources(DWORD dwTimestamp);

protected:
	void ParseSharedMemory(PHWiNFO_SENSORS_SHARED_MEM2 pHWiNFOMemory);
		//parses HwINFO64 shared memory
	void UpdateSource(DWORD dwSensorInst, DWORD dwReadingType, LPCSTR lpReadingName, float fltData);
		//updates all registered data sources with matching ID
};
/////////////////////////////////////////////////////////////////////////////
