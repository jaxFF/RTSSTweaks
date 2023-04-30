// RTSSSharedMemoryInterface.h: interface for the CRTSSSharedMemoryInterface class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "RTSSSharedMemory.h"
//////////////////////////////////////////////////////////////////////
class CRTSSSharedMemoryInterface
{
public:
	CRTSSSharedMemoryInterface();
	virtual ~CRTSSSharedMemoryInterface();

	BOOL	UpdateOSD(LPCSTR lpOSDOwner, LPCSTR lpText, LPBYTE lpBuffer, DWORD dwBufferSize);
		//connects to RTSS shared memory and copies lpText to OSD text slot owned by our application, optionally copies lpBuffer to 
		//embedded object buffer

		//NOTE: this function captues OSD text slot if we don not own one yet
	void	ReleaseOSD(LPCSTR lpOSDOwner);
		//connects to RTSS shared memory and releases OSD text slot owned by our application
	DWORD	EmbedGraph(LPCSTR lpOSDOwner, DWORD dwOffset, FLOAT* lpBuffer, DWORD dwBufferPos, DWORD dwBufferSize, LONG dwWidth, LONG dwHeight, LONG dwMargin, FLOAT fltMin, FLOAT fltMax, DWORD dwFlags);
		//connects to RTSS shared memory and embeds new graph into object buffer of OSD text slot owned by our application, returns embedded object size
	DWORD	EmbedGraphInObjBuffer(LPBYTE lpObjBuffer, DWORD dwObjBufferSize, DWORD dwOffset, FLOAT* lpBuffer, DWORD dwBufferPos, DWORD dwBufferSize, LONG dwWidth, LONG dwHeight, LONG dwMargin, FLOAT fltMin, FLOAT fltMax, DWORD dwFlags);
		//embeds new graph object into specified object buffer, returns embedded object size

		//NOTE: this function captues OSD text slot if we don not own one yet

	BOOL	GetProcessPerfCounter(DWORD dwProcessID, DWORD dwSamplingPeriod, DWORD dwPerfCounterID, DWORD dwPerfCounterParam, LPDWORD lpPerfCounter);
		//connects to RTSS shared memory, enables asynchronous process specific performance counters collection with desired sampling period
		//for specified process (or for foreground process if dwProcessID is 0) and returns the last asynchronically sampled value for a counter
		//identified by dwPerfCounterID/dwPerfCounterParam pair
};
//////////////////////////////////////////////////////////////////////
