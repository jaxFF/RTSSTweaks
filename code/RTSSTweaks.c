#include <stdint.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <nvapi.h>

#if _WIN32 || _WIN64
#define NvAPIInitalize_Address 0x0150E828
#define NvAPIEnumPhysicalGPUs_Address 0xE5AC921F
#define GetVoltageDomainsStatusAddress 0x0A4DFD3F2
#endif

#if !defined(NvAPIInitalize_Address)
#error Missing or undefined NvAPI NDA-exclusive function addresses
#endif

typedef uint32_t u32;

#define Assert(Expr) do { if (!(Expr)) { *(volatile int*)(0) = (0); } } while (0)

// NVAPI hidden stuff

typedef u32 NvStatus;
typedef u32 NvPhysicalGPUHandle;

#define NV_MAKE_STRUCT_VERSION(Struct, Version) (sizeof(Struct) | (Version << 16))

typedef struct NV_GPU_CLOCK_INFO_V2 NV_GPU_CLOCK_INFO_V2;
struct NV_GPU_CLOCK_INFO_V2 {
    u32 Version;
    struct {
        u32 Frequency;
        u32 Unk1[1];
    } extendedDomain[32];
    u32 Unk1[224];
};

typedef struct NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1 NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1;
struct NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1 {
    u32 U32[19];
};

typedef struct NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1 NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1;
struct NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1 {
    u32 Version;
    u32 CoolerCount;
    struct {
        u32 Unk1[8];
        u32 CoolerIndex;
        u32 CurrentRpm;
        u32 LevelMin;
        u32 LevelMax;
        u32 LevelTarget;
    } Entries[20];
    u32 Unk1[164]; // Why the fuck are all these bytes used to make the version? This is super inefficent.
                   // TODO: Remove these bytes from the struct and manually calculate the version with them.
};

typedef struct NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 NV_GPU_VOLTAGE_DOMAINS_STATUS_V1;
struct NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 {
    u32 Version;
    u32 Unk[15];
};

typedef void* (*NvAPI_QueryInterface_t)(u32);
typedef NvStatus (*NvAPI_Initalize_t)();
typedef NvStatus (*NvAPI_EnumPhysicalGPUs_t)(NvPhysicalGPUHandle[64], u32*);
typedef NvStatus (*NvAPI_GPU_GetAllClocks_t)(NvPhysicalGPUHandle, NV_GPU_CLOCK_INFO_V2*);
typedef NvStatus (*NvAPI_GPU_ClientVoltRailsGetStatus_t)(NvPhysicalGPUHandle, NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1*);
typedef NvStatus (*NvAPI_GPU_ClientFanCoolersGetStatus_t)(NvPhysicalGPUHandle, NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1*);
typedef NvStatus (*NvAPI_GPU_GetVoltageDomainsStatus_t)(NvPhysicalGPUHandle, NV_GPU_VOLTAGE_DOMAINS_STATUS_V1*);

int main() {
    // NOTE, TODO: Ensure this test function is compiled with a 32-bit version of cl.exe

    HMODULE NvAPIHandle = 0;
    NvAPIHandle = LoadLibrary("nvapi.dll");

    NvAPI_QueryInterface_t NvAPI_QueryInterface = 0;
    NvAPI_QueryInterface = (NvAPI_QueryInterface_t)GetProcAddress(NvAPIHandle, "nvapi_QueryInterface");

    NvAPI_Initalize_t NvAPI_Initalize = 0;
    NvAPI_Initalize = (NvAPI_Initalize_t)NvAPI_QueryInterface(NvAPIInitalize_Address);

    NvAPI_EnumPhysicalGPUs_t NvAPI_EnumPhysicalGPUs = 0;
    NvAPI_EnumPhysicalGPUs = NvAPI_QueryInterface(NvAPIEnumPhysicalGPUs_Address);

    NvAPI_Initalize();

    u32 GpuCount = 0;
    NvPhysicalGPUHandle GpuHandles[64] = {0};
    NvAPI_EnumPhysicalGPUs(GpuHandles, &GpuCount);

    NvPhysicalGPUHandle GpuHandle = GpuHandles[0];

    NvAPI_GPU_GetAllClocks_t NvAPI_GPU_GetAllClocks = 0;
    NvAPI_GPU_GetAllClocks = NvAPI_QueryInterface(0x1BD69F49);

    NV_GPU_CLOCK_INFO_V2 ClocksInfo = {0};
    ClocksInfo.Version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLOCK_INFO_V2, 2);
    Assert(NvAPI_GPU_GetAllClocks(GpuHandle, &ClocksInfo) == 0); // core clock = 1800000, memory clock = 10702000

    NvAPI_GPU_ClientVoltRailsGetStatus_t NvAPI_GPU_ClientVoltRailsGetStatus = 0;
    NvAPI_GPU_ClientVoltRailsGetStatus = NvAPI_QueryInterface(0x465f9bcf);

    NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1 VoltRailStatus = {0};
    VoltRailStatus.U32[0] = NV_MAKE_STRUCT_VERSION(NV_GPU_CLIENT_VOLT_RAILS_STATUS_V1, 1);
    Assert(NvAPI_GPU_ClientVoltRailsGetStatus(GpuHandle, &VoltRailStatus) == 0);

    NvAPI_GPU_ClientFanCoolersGetStatus_t NvAPI_GPU_ClientFanCoolersGetStatus = 0;
    NvAPI_GPU_ClientFanCoolersGetStatus = NvAPI_QueryInterface(0x35AED5E8);

    NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1 FanStatus = {0};
    FanStatus.Version = NV_MAKE_STRUCT_VERSION(NV_GPU_CLIENT_FAN_COOLERS_STATUS_V1, 1);
    Assert(NvAPI_GPU_ClientFanCoolersGetStatus(GpuHandle, &FanStatus) == 0);

    NvAPI_GPU_GetVoltageDomainsStatus_t NvAPI_GPU_GetVoltageDomainsStatus = 0;
    NvAPI_GPU_GetVoltageDomainsStatus = NvAPI_QueryInterface(0x0C16C7E2C); // Oops 0A4DFD3F2 is NvAPI_GPU_ClientPowerTopologyGetInfo 

    NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 Status = {0};
//    Status.Version = sizeof(NV_GPU_VOLTAGE_DOMAINS_STATUS_V1) | (1 << 16);;
    Status.Version = 65676;
    NvStatus Ret = NvAPI_GPU_GetVoltageDomainsStatus(GpuHandle, &Status);
    

    return 0;
}
