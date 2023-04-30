#pragma once
//////////////////////////////////////////////////////////////////////
#define HWINFO_SDK_AVAILABLE
//////////////////////////////////////////////////////////////////////
//
// WARNING!
//
// HwINFO64 shared memory layout defined in hwisenssm2.h is not publicly 
// avaiable to everyone per HwINFO64 developer decision
//
// So you'll be able to compile the plugin with full HwINFO64 support 
// only if you get this header file directly from HwINFO64 developer
//
// Otherwise you may just comment HWINFO_SDK_AVAILABLE define a few lines 
// above to build the plugin without HWiNFO64 data sources support
//////////////////////////////////////////////////////////////////////
#ifdef HWINFO_SDK_AVAILABLE
#include "hwisenssm2.h"		// ^^^ read above if compiler fails on this
#else
#ifndef PHWiNFO_SENSORS_SHARED_MEM2
#define PHWiNFO_SENSORS_SHARED_MEM2 LPVOID
#endif
#ifndef PHWiNFO_SENSORS_READING_ELEMENT
#define PHWiNFO_SENSORS_READING_ELEMENT LPVOID
#endif
#endif
//////////////////////////////////////////////////////////////////////

