#ifndef nvapi_cooler_enums_h
#define nvapi_cooler_enums_h

// NOTE: Thanks user "mathstlaurent" from NVIDIA Developer Forums for leaking the NV_COOLER_* enums:
//  https://forums.developer.nvidia.com/t/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/243652
// NOTE: Thanks user "just me" from AHK Forums for leaking the GPU_COOLER_SETTINGS defines and structures:
//  https://www.autohotkey.com/boards/viewtopic.php?t=4469&start=40
// NOTE: Thanks to EVGA PX1
typedef enum NV_COOLER_TYPE NV_COOLER_TYPE;
enum NV_COOLER_TYPE {
    NVAPI_COOLER_TYPE_NONE = 0,
    NVAPI_COOLER_TYPE_FAN,
    NVAPI_COOLER_TYPE_WATER,
    NVAPI_COOLER_TYPE_LIQUID_NO2,
};

typedef enum NV_COOLER_CONTROLLER NV_COOLER_CONTROLLER;
enum NV_COOLER_CONTROLLER {
    NVAPI_COOLER_CONTROLLER_NONE = 0,
    NVAPI_COOLER_CONTROLLER_ADI,
    NVAPI_COOLER_CONTROLLER_INTERNAL,
};

typedef enum NV_COOLER_POLICY NV_COOLER_POLICY;
enum NV_COOLER_POLICY {
    NVAPI_COOLER_POLICY_NONE = 0,
    NVAPI_COOLER_POLICY_MANUAL,                     // Manual adjustment of cooler level. Gets applied right away independent of temperature or performance level.
    NVAPI_COOLER_POLICY_PERF,                       // GPU performance controls the cooler level.
    NVAPI_COOLER_POLICY_TEMPERATURE_DISCRETE = 4,   // Discrete thermal levels control the cooler level.
    NVAPI_COOLER_POLICY_TEMPERATURE_CONTINUOUS = 8, // Cooler level adjusted at continuous thermal levels.
    NVAPI_COOLER_POLICY_HYBRID,                     // Hybrid of performance and temperature levels.
};

typedef enum NV_COOLER_TARGET NV_COOLER_TARGET;
enum NV_COOLER_TARGET {
    NVAPI_COOLER_TARGET_NONE = 0,
    NVAPI_COOLER_TARGET_GPU,
    NVAPI_COOLER_TARGET_MEMORY,
    NVAPI_COOLER_TARGET_POWER_SUPPLY = 4,
    NVAPI_COOLER_TARGET_ALL = 7                    // This cooler cools all of the components related to its target gpu.
};

typedef enum NV_COOLER_CONTROL NV_COOLER_CONTROL;
enum NV_COOLER_CONTROL {
    NVAPI_COOLER_CONTROL_NONE = 0,
    NVAPI_COOLER_CONTROL_TOGGLE, // ON/OFF
    NVAPI_COOLER_CONTROL_VARIABLE, // Suppports variable control.
};

typedef enum NV_COOLER_ACTIVITY_LEVEL NV_COOLER_ACTIVITY_LEVEL;
enum NV_COOLER_ACTIVITY_LEVEL {
    NVAPI_INACTIVE = 0, // inactive or unsupported
    NVAPI_ACTIVE = 1, // active and spinning in case of fan
};

#endif
