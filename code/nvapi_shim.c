#include <stdbool.h>
#include <stdint.h>
#define WIN32_LEAN_AND_MEAN
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
typedef u8 NvU8;
typedef u16 NvU16;
typedef u32 NvU32;
typedef u64 NvU64;
typedef s32 NvS32;
typedef bool NvBool;

typedef u32 NvPhysicalGpuHandle;
#endif

#ifdef NVAPI_INIT_STRUCT
#undef NVAPI_INIT_STRUCT
#endif

#define NVAPI_INIT_STRUCT(Struct, Version) NV_MAKE_STRUCT_VERSION(Struct, Version)
#define NV_MAKE_STRUCT_VERSION(Struct, Version) (sizeof(Struct) | (Version << 16))

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

typedef struct NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 NV_GPU_VOLTAGE_DOMAINS_STATUS_V1;
struct NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 {
    u32 version;
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

typedef void* (*NvAPI_QueryInterface_t)(u32);
typedef NvStatus (*NvAPI_Initalize_t)();
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

typedef NvStatus (*NvAPI_GPU_GetVoltageDomainsStatus_t)(NvPhysicalGpuHandle, NV_GPU_VOLTAGE_DOMAINS_STATUS_V1*); // 0xC16C7E2C  Maxwell only, Assert that the gpu is > maxwell I guess.

NvAPI_GPU_ClientVoltRailsGetStatus_t NvAPI_GPU_ClientVoltRailsGetStatus = 0;
NvAPI_GPU_ClientFanCoolersGetStatus_t NvAPI_GPU_ClientFanCoolersGetStatus = 0;
NvAPI_GPU_ClientPowerTopologyGetStatus_t NvAPI_GPU_ClientPowerTopologyGetStatus = {0};
NvAPI_GPU_GetVoltageDomainsStatus_t NvAPI_GPU_GetVoltageDomainsStatus = 0;

#ifndef OVERLAYEDITOR_BUILD
int main() {
    // NOTE, TODO: Ensure this test function is compiled with a 32-bit version of cl.exe

    HMODULE NvAPIHandle = 0;
    NvAPIHandle = LoadLibrary("nvapi.dll");

    NvAPI_QueryInterface_t NvAPI_QueryInterface = 0;
    NvAPI_Initalize_t NvAPI_Initalize = 0;
    NvAPI_EnumPhysicalGPUs_t NvAPI_EnumPhysicalGPUs = 0;
    NvAPI_GPU_GetAllClocks_t NvAPI_GPU_GetAllClocks = 0;
    NvAPI_GPU_GetDynamicPstatesInfoEx_t NvAPI_GPU_GetDynamicPstatesInfoEx = {0};

    NvAPI_QueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(NvAPIHandle, "nvapi_QueryInterface");

    NvAPI_Initalize = (NvAPI_Initalize_t)NvAPI_QueryInterface(NvAPIInitalize_Address);

    NvAPI_EnumPhysicalGPUs = NvAPI_QueryInterface(NvAPIEnumPhysicalGPUs_Address);

    NvAPI_Initalize();

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
        Breakpoint;
    }

    NvAPI_GPU_GetDynamicPstatesInfoEx = NvAPI_QueryInterface(0x60DED2ED);

    NV_GPU_DYNAMIC_PSTATES_INFO_EX Pstates = {0};
    Pstates.version = NV_MAKE_STRUCT_VERSION(NV_GPU_DYNAMIC_PSTATES_INFO_V1, 1);
    Assert(NvAPI_GPU_GetDynamicPstatesInfoEx(GpuHandle, &Pstates) == 0);
    Breakpoint;

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

    Breakpoint;

    NvAPI_GPU_ClientFanCoolersGetStatus = NvAPI_QueryInterface(0x35AED5E8);

    NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1 FanStatus = {0};
    FanStatus.version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1, 1);
    Assert(NvAPI_GPU_ClientFanCoolersGetStatus(GpuHandle, &FanStatus) == 0);

    NvAPI_GPU_ClientPowerTopologyGetStatus = NvAPI_QueryInterface(0xEDCF624E);

    NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1 PowerTopology = {0}; 
    PowerTopology.version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLIENT_POWER_TOPOLOGY_STATUS_V1, 1);
    Assert(NvAPI_GPU_ClientPowerTopologyGetStatus(GpuHandle, &PowerTopology) == 0);

    NvAPI_GPU_GetVoltageDomainsStatus = NvAPI_QueryInterface(0x0C16C7E2C); // Oops 0A4DFD3F2 is NvAPI_GPU_ClientPowerTopologyGetInfo 

    NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 Status = {0};
    Status.version = NV_MAKE_STRUCT_VERSION(NV_GPU_VOLTAGE_DOMAINS_STATUS_V1, 1);
//    Assert(NvAPI_GPU_GetVoltageDomainsStatus(GpuHandle, &Status) == 0);
    

    return 0;
}
#endif
