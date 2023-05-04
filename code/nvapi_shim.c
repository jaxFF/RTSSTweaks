#include <stdbool.h>
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN
#include <process.h>
#include <windows.h>

#if _WIN32 || _WIN64
#define NvAPIInitalize_Address 0x0150E828
#define NvAPIEnumPhysicalGPUs_Address 0xE5AC921F
#define GetVoltageDomainsStatusAddress 0x0A4DFD3F2
#endif

#if !defined(NvAPIInitalize_Address)
#error Missing or undefined NvAPI NDA-exclusive function addresses
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t s32;

#define Breakpoint __asm int 3
#define Assert(Expr) do { if (!(Expr)) { *(volatile int*)(0) = (0); } } while (0)

// Undocumented NDA-exclusive NVAPI

typedef u32 NvStatus;

#ifndef _NVAPI_H
typedef u32 NvAPI_Status;

typedef u8 NvU8;
typedef u16 NvU16;
typedef u32 NvU32;
typedef u64 NvU64;
typedef s32 NvS32;
typedef bool NvBool;

typedef struct NvPhysicalGpuHandle__ NvPhysicalGpuHandle__;
struct NvPhysicalGpuHandle__ { u32 U32; };
typedef NvPhysicalGpuHandle__ NvPhysicalGpuHandle;
#endif

#ifdef NVAPI_INIT_STRUCT
#undef NVAPI_INIT_STRUCT
#endif

#define NVAPI_INIT_STRUCT(Struct, Version) NV_MAKE_STRUCT_VERSION(Struct, Version);
#define NV_MAKE_STRUCT_VERSION(Struct, Version) (sizeof(Struct) | (Version << 16))

#include "nvapi_cooler_enums.h"

#ifndef _NVAPI_H
typedef struct NV_GPU_CLOCK_INFO_V2 NV_GPU_CLOCK_INFO_V2;
struct NV_GPU_CLOCK_INFO_V2 {
    u32 version;
    struct {
        u32 effectiveFrequency;
        u32 Unk[1];
    } extendedDomain[32];

    u32 Unk[224];
};

enum NV_GPU_CLOCK_FREQUENCIES_CLOCK_TYPE { // Directly from NVIDIA public docs
    NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ = 0, 
    NV_GPU_CLOCK_FREQUENCIES_BASE_CLOCK = 1, 
    NV_GPU_CLOCK_FREQUENCIES_BOOST_CLOCK = 2, 
    NV_GPU_CLOCK_FREQUENCIES_CLOCK_TYPE_NUM = 32 
};
#endif

typedef struct NV_GPU_CLOCK_FREQUENCY_INFO_V2 NV_GPU_CLOCK_FREQUENCY_INFO_V2;
struct NV_GPU_CLOCK_FREQUENCY_INFO_V2 { // version = 131336, likely version 2
    u32 version;
    u32 ClockType; 
    struct {
        u32 Unk;
        u32 frequency;
    } domain[32];
};

typedef struct NV_GPU_DYNAMIC_PSTATES_INFO_V1 NV_GPU_DYNAMIC_PSTATES_INFO_V1;
struct NV_GPU_DYNAMIC_PSTATES_INFO_V1 {
    u32 version;
    u32 Unk;
    struct {
        u32 bIsPresent; // ?? (TODO: reckon this is part of a bitfield inside of this u32, needs confirming)
        u32 percentage;
    } Entries[8];
};

#ifndef _NVAPI_H
typedef NV_GPU_DYNAMIC_PSTATES_INFO_V1 NV_GPU_DYNAMIC_PSTATES_INFO_EX; // NOTE: overlayeditor actually uses this variant 
#endif

#define NV_GPU_CLIENT_VOLT_DOMAIN_CORE 0xFF // TODO: Determine this actual value
#define NV_GPU_CLIENT_VOLT_DOMAIN_MAX_ENTRIES 8

typedef struct NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1 NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1;
struct NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1 {
    u32 version;
    struct {
        u8 railId;
        u8 Unk[2];
        u32 currVoltuV;
    } rails[8];
    u32 U32[2];
};

typedef struct NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1 NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1;
struct NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1 {
    u32 version;
    u32 numCoolers;
    struct {
        u32 Unk[8];
        u32 CoolerIndex;
        u32 rpmCurr;
        u32 levelMin;
        u32 levelMax;
        u32 levelTarget;
    } coolers[20];
    u32 Unk1[164]; // Why the fuck are all these bytes used to make the version? This is super inefficent.
                   //  SURELY they aren't all indexed as fan entires here, no?
                   // TODO: Remove these bytes from the struct and manually calculate the version with them.
};

#define NV_GPU_CLIENT_POWER_TOPOLOGY_CHANNEL_ID_POWER 0 // TODO: Verify this one ??
#define NV_GPU_CLIENT_POWER_TOPOLOGY_CHANNEL_ID_NORMALIZED_TOTAL_POWER 1

typedef struct NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1 NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1;
struct NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1 { // Comparing HWiNFO64 values helped a *ton* in reversing this struct
    u32 version;
    u32 numChannels;
    struct {
        u32 channelId;
        u32 Unk1;
        struct {
            u32 mp;
        } power;
        u32 Unk2;
    } channels[4];
};

#ifndef _NVAPI_H
#define NVAPI_MAX_GPU_PERF_VOLTAGES 16
#endif

typedef struct NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 NV_GPU_VOLTAGE_DOMAINS_STATUS_V1;
struct NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 {
    u32 version;
    u32 U32;
    u32 numDomains;
    struct {
        u32 domainId;
        u32 voltageuV;
    } domains[NVAPI_MAX_GPU_PERF_VOLTAGES];
};

#define NVAPI_MAX_COOLERS_PER_GPU_V1 3
#define NVAPI_MAX_COOLERS_PER_GPU_V2 20
#define NVAPI_MAX_COOLERS_PER_GPU_V3 NVAPI_MAX_COOLERS_PER_GPU_V2

#define NVAPI_MAX_COOLERS_PER_GPU NVAPI_MAX_COOLERS_PER_GPU_V3
#define NVAPI_MIN_COOLER_LEVEL 0
#define NVAPI_MAX_COOLER_LEVEL 100
#define NVAPI_MAX_COOLER_LEVELS 24

typedef struct NV_GPU_GETCOOLER_SETTINGS NV_GPU_GETCOOLER_SETTINGS;
#pragma pack(push, 8)
struct NV_GPU_GETCOOLER_SETTINGS {
    u32 version; // structure version
    u32 count; // number of associated coolers with the selected GPU
    struct {
        NV_COOLER_TYPE type; // type of cooler - FAN, WATER, LIQUID_NO2...
        NV_COOLER_CONTROLLER controller; // internal, ADI
        u32 defaultMinLevel; // the min default value % of the cooler
        u32 defaultMaxLevel; // the max default value % of the cooler
        u32 currentMinLevel; // the current allowed min value % of the cooler
        u32 currentMaxLevel; // the current allowed max value % of the cooler
        u32 currentLevel; // the current value % of the cooler
        NV_COOLER_POLICY defaultPolicy; // cooler control policy - auto-perf, auto-thermal, manual, hybrid...
        NV_COOLER_POLICY currentPolicy; // cooler control policy - auto-perf, auto-thermal, manual, hybrid...
        NV_COOLER_TARGET target; // cooling target - GPU, memory, chipset, powersupply, Visual Computing Device...
        NV_COOLER_CONTROL controlType; // toggle or variable?
        NV_COOLER_ACTIVITY_LEVEL active; // is the cooler active - fan spinning...
    } cooler[NVAPI_MAX_COOLERS_PER_GPU];
};
#pragma pack(pop)

typedef struct NV_COOLER_TACHOMETER NV_COOLER_TACHOMETER;
struct NV_COOLER_TACHOMETER {
    u32 speedRPM; // current tachometer reading in RPM
    u8 bSupported; // cooler supports tach functionality?
    u32 maxSpeedRPM; // Maximum RPM corresponding to 100% defaultMaxLevel
    u32 minSpeedRPM; // Minimum RPM corresponding to 100% defaultMinLevel
};

typedef struct NV_GPU_GETCOOLER_SETTINGS_V3 NV_GPU_GETCOOLER_SETTINGS_V3;
struct NV_GPU_GETCOOLER_SETTINGS_V3 { // 1288 bytes..
    u32 version; // structure version
    u32 count; // number of associated coolers with the selected GPU
    struct {
        NV_COOLER_TYPE type; // type of cooler - FAN, WATER, LIQUID_NO2...
        NV_COOLER_CONTROLLER controller; // internal, ADI
        u32 defaultMinLevel; // the min default value % of the cooler
        u32 defaultMaxLevel; // the max default value % of the cooler
        u32 currentMinLevel; // the current allowed min value % of the cooler
        u32 currentMaxLevel; // the current allowed max value % of the cooler
        u32 currentLevel; // the current value % of the cooler
        NV_COOLER_POLICY defaultPolicy; // cooler control policy - auto-perf, auto-thermal, manual, hybrid...
        NV_COOLER_POLICY currentPolicy; // cooler control policy - auto-perf, auto-thermal, manual, hybrid...
        NV_COOLER_TARGET target; // cooling target - GPU, memory, chipset, powersupply, Visual Computing Device...
        NV_COOLER_CONTROL controlType; // toggle or variable?
        NV_COOLER_ACTIVITY_LEVEL active; // is the cooler active - fan spinning...
        NV_COOLER_TACHOMETER tachometer; // cooler tachometer info
    } cooler[NVAPI_MAX_COOLERS_PER_GPU];
};

struct NV_GPU_GETCOOLER_SETTINGS_V4; // TODO: PX1 uses 1368 bytes here. Reversing from here should be ez, *if I had a GTX card on hand*

typedef void* (*NvAPI_QueryInterface_t)(u32);
typedef NvStatus (*NvAPI_Initialize_t)();
typedef NvStatus (*NvAPI_EnumPhysicalGPUs_t)(NvPhysicalGpuHandle[64], u32*);

#ifndef _NVAPI_H
typedef NvStatus (*NvAPI_GPU_GetAllClocks_t)(NvPhysicalGpuHandle, NV_GPU_CLOCK_INFO_V2*);
#endif
typedef NvStatus (*NvAPI_GPU_GetAllClockFrequencies_t)(NvPhysicalGpuHandle, NV_GPU_CLOCK_FREQUENCY_INFO_V2*); // TODO: Unrequired, public facing.
// DEPRECATED: typedef NvStatus (*NvAPI_GPU_GetDynamicPstates_t)(NvPhysicalGpuHandle, NV_GPU_DYNAMIC_PSTATES_INFO_V1*); // 0x60DED2ED
typedef NvStatus (*NvAPI_GPU_GetDynamicPstatesInfoEx_t)(NvPhysicalGpuHandle, NV_GPU_DYNAMIC_PSTATES_INFO_EX*); // 0x60DED2ED
 
typedef NvStatus (*NvAPI_GPU_ClientVoltRailsGetStatus_t)(NvPhysicalGpuHandle, NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1*);
typedef NvStatus (*NvAPI_GPU_ClientFanCoolersGetStatus_t)(NvPhysicalGpuHandle, NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1*);
typedef NvStatus (*NvAPI_GPU_ClientPowerTopologyGetStatus_t)(NvPhysicalGpuHandle, NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1*);

typedef NvStatus (*NvAPI_GPU_GetCoolerSettings_t)(NvPhysicalGpuHandle, NV_COOLER_TARGET, NV_GPU_GETCOOLER_SETTINGS*); // 0xDA141340
typedef NvStatus (*NvAPI_GPU_GetVoltageDomainsStatus_t)(NvPhysicalGpuHandle, NV_GPU_VOLTAGE_DOMAINS_STATUS_V1*); // 0xC16C7E2C  Maxwell only, Assert that the gpu is > maxwell I guess.
                                                                                                                 
#ifndef OVERLAYEDITOR_BUILD 
#define DllApi __declspec(dllexport)
#else
#define DllApi __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#ifndef OVERLAYEDITOR_BUILD // This function definition *has* to be excluded from the "header", because it's exposed in the public NvAPI.
    DllApi NvAPI_Initialize_t NvAPI_Initialize;
#endif
#endif
    DllApi NvAPI_GPU_ClientVoltRailsGetStatus_t NvAPI_GPU_ClientVoltRailsGetStatus;
    DllApi NvAPI_GPU_ClientFanCoolersGetStatus_t NvAPI_GPU_ClientFanCoolersGetStatus;
    DllApi NvAPI_GPU_ClientPowerTopologyGetStatus_t NvAPI_GPU_ClientPowerTopologyGetStatus;
    DllApi NvAPI_GPU_GetCoolerSettings_t NvAPI_GPU_GetCoolerSettings;
    DllApi NvAPI_GPU_GetVoltageDomainsStatus_t NvAPI_GPU_GetVoltageDomainsStatus;
#ifdef __cplusplus
}
#endif

// TODO: Structure this into an actual shim library that intercepts the API calls
//  RTSSOverlayEditor is interested in. We can redirect the NvAPI_Initialize call to our 
//  own version, which will do whatever we need from main() test code, and then call the 
//  correct NvAPI_Initialize. This will be relevant for every NvAPI funtion call here.
//
// We wish to achieve OverlayEditor.dll executing our shim functions before 
//  hitting the correct NvAPI call. We will export the functions through the nvapi
//  shim we build, which will just have to be a shim / proxy / wrapper to inject
//  code without actually having to use an injector. We just beat NVIDIA by being
//  loaded first based on paths, doing our stuff first, then executing the correct NVAPI call.

// TODO: Write the custom data sources for RTSSOverlayEditor.dll
// TODO: NVIDIA Reflex API: Render Latency as a data source

//#include "nvapi_shim_loader.c"

#ifndef __cplusplus
DWORD WINAPI Load(LPVOID Data) {
    MessageBoxA(0, "proxying works !", "nvapi_shim.dll", MB_OK);
    return 1;
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hinstDLL);
            CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Load, NULL, 0, NULL);
        } break;
        case DLL_PROCESS_DETACH: {
// TODO:            FreeLibrary()
        } break;
    }
    return true;
}

#endif

#ifdef OVERLAYEDITOR_BUILD
#pragma comment(lib, "../nvapi_shim.lib") 
#endif

#ifndef OVERLAYEDITOR_BUILD

extern NvStatus NvAPI_Initialize() {
    MessageBoxA(0, "NvAPI_Initalize hooked!", "", MB_OK);
    return 0;
}

int main() {
    // NOTE, TODO: Ensure this test function is compiled with a 32-bit version of cl.exe

    HMODULE NvAPIHandle = 0;
    NvAPIHandle = LoadLibrary("nvapi.dll");

    NvAPI_Initialize_t NvAPI_Initialize = 0;
    NvAPI_QueryInterface_t NvAPI_QueryInterface = 0;
    NvAPI_EnumPhysicalGPUs_t NvAPI_EnumPhysicalGPUs = 0;
    NvAPI_GPU_GetAllClocks_t NvAPI_GPU_GetAllClocks = 0;
    NvAPI_GPU_GetDynamicPstatesInfoEx_t NvAPI_GPU_GetDynamicPstatesInfoEx = {0};

    NvAPI_QueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(NvAPIHandle, "nvapi_QueryInterface");

    NvAPI_Initialize = (NvAPI_Initialize_t)NvAPI_QueryInterface(NvAPIInitalize_Address);

    NvAPI_EnumPhysicalGPUs = NvAPI_QueryInterface(NvAPIEnumPhysicalGPUs_Address);

    NvAPI_Initialize();

    u32 GpuCount = 0;
    NvPhysicalGpuHandle GpuHandles[64] = { 0 };
    NvAPI_EnumPhysicalGPUs(GpuHandles, &GpuCount);

    NvPhysicalGpuHandle GpuHandle = GpuHandles[0];

    NvAPI_GPU_GetAllClocks = NvAPI_QueryInterface(0x1BD69F49);

    NV_GPU_CLOCK_INFO_V2 ClocksInfo = { 0 };
    ClocksInfo.version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLOCK_INFO_V2, 2);
    Assert(NvAPI_GPU_GetAllClocks(GpuHandle, &ClocksInfo) == 0); // core clock = 1800000, memory clock = 10702000

    NvAPI_GPU_GetAllClockFrequencies_t NvAPI_GPU_GetAllClockFrequencies = { 0 };
    NvAPI_GPU_GetAllClockFrequencies = NvAPI_QueryInterface(0xDCB616C3);
    
    { // Same results.
        NV_GPU_CLOCK_FREQUENCY_INFO_V2 ClockFrequencies1 = { 0 };
        ClockFrequencies1.version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLOCK_FREQUENCY_INFO_V2, 2);
        Assert(NvAPI_GPU_GetAllClockFrequencies(GpuHandle, &ClockFrequencies1) == 0);

        NV_GPU_CLOCK_FREQUENCY_INFO_V2 ClockFrequencies2 = { 0 };
        ClockFrequencies2.version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLOCK_FREQUENCY_INFO_V2, 2);
        //ClockFrequencies2.Unk = NV_GPU_CLOCK_FREQUENCIES_CURRENT_FREQ;
        Assert(NvAPI_GPU_GetAllClockFrequencies(GpuHandle, &ClockFrequencies2) == 0);
//        Breakpoint;
    }

    NvAPI_GPU_GetDynamicPstatesInfoEx = NvAPI_QueryInterface(0x60DED2ED);

    NV_GPU_DYNAMIC_PSTATES_INFO_EX Pstates = {0};
    Pstates.version = NV_MAKE_STRUCT_VERSION(NV_GPU_DYNAMIC_PSTATES_INFO_V1, 1);
    Assert(NvAPI_GPU_GetDynamicPstatesInfoEx(GpuHandle, &Pstates) == 0);
//    Breakpoint;

    NvAPI_GPU_ClientVoltRailsGetStatus = NvAPI_QueryInterface(0x465F9BCF);

    NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1 VoltRailStatus = {0};
    VoltRailStatus.version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1, 1);
    Assert(NvAPI_GPU_ClientVoltRailsGetStatus(GpuHandle, &VoltRailStatus) == 0);

    // NOTE: We'll have to write shim-code for this function because the nvapi.dll 
    //  result doesn't match the NDA sdk that RTSSOverlayEditor uses.
    //  Basically, NVIDIA sucks.
#ifndef NV_GPU_CLIENT_VOLT_DOMAIN_CORE
#define NV_GPU_CLIENT_VOLT_DOMAIN_CORE 0xFF // TODO: Determine this actual value
#endif
    VoltRailStatus.rails[0x4].railId = NV_GPU_CLIENT_VOLT_DOMAIN_CORE;

    // Usage code
    for (u8 iRail = 0; iRail < 8; iRail++) {
        if (VoltRailStatus.rails[iRail].railId == NV_GPU_CLIENT_VOLT_DOMAIN_CORE) {
            u32 GPUCoreVoltage = VoltRailStatus.rails[iRail].currVoltuV / 1000;
            //Breakpoint;
        }
    }

//    Breakpoint;

    NvAPI_GPU_ClientFanCoolersGetStatus = NvAPI_QueryInterface(0x35AED5E8);

    NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1 FanStatus = {0};
    FanStatus.version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1, 1);
    Assert(NvAPI_GPU_ClientFanCoolersGetStatus(GpuHandle, &FanStatus) == 0);

    NvAPI_GPU_ClientPowerTopologyGetStatus = NvAPI_QueryInterface(0xEDCF624E);

    NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1 PowerTopology = {0}; 
    PowerTopology.version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1, 1);
    Assert(NvAPI_GPU_ClientPowerTopologyGetStatus(GpuHandle, &PowerTopology) == 0);
    
    NvAPI_GPU_GetCoolerSettings = NvAPI_QueryInterface(0xDA141340);

    NV_GPU_GETCOOLER_SETTINGS_V3 CoolerSettings = {0};
    CoolerSettings.version = NV_MAKE_STRUCT_VERSION(NV_GPU_GETCOOLER_SETTINGS_V3, 3);
    Assert(NvAPI_GPU_GetCoolerSettings(GpuHandle, 1, (NV_GPU_GETCOOLER_SETTINGS*)&CoolerSettings) == 4294967192 
            && "This function is deprecated, it only works on Pascal cards."); // *from what I can tell!* It always returns this value.

    NvAPI_GPU_GetVoltageDomainsStatus = NvAPI_QueryInterface(0x0C16C7E2C); // Oops 0A4DFD3F2 is NvAPI_GPU_ClientPowerTopologyGetInfo 

    NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 Status = {0};
    Status.version = NV_MAKE_STRUCT_VERSION(NV_GPU_VOLTAGE_DOMAINS_STATUS_V1, 1);
    Assert(NvAPI_GPU_GetVoltageDomainsStatus(GpuHandle, &Status) == 4294967192 
            && "This function is deprecated, it only works on Maxwell cards.");

    return 0;
}
#endif
