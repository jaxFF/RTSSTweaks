#ifndef _RTCORESYS_H_INCLUDED_
#define _RTCORESYS_H_INCLUDED_
////////////////////////////////////////////////////////////////////////////////
#define FILE_DEVICE_RTCORESYS  0x00008000
#define RTCORESYS_IOCTL_INDEX  0x800
////////////////////////////////////////////////////////////////////////////////
#define IOCTL_RTCORESYS_VERSION					CTL_CODE(FILE_DEVICE_RTCORESYS, RTCORESYS_IOCTL_INDEX + 10, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define RTCORESYS_VERSION_MAJOR	0x00000001
#define RTCORESYS_VERSION_MINOR	0x00000008

typedef struct
{
	ULONG			dwMajor;
	ULONG			dwMinor;
} VERSION_INFO, *PVERSION_INFO;
////////////////////////////////////////////////////////////////////////////////
#define IOCTL_RTCORESYS_REFERENCE_COUNT			CTL_CODE(FILE_DEVICE_RTCORESYS, RTCORESYS_IOCTL_INDEX + 11, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define REFERENCE_COUNT_INVALID 0x80000000

typedef struct
{
	ULONG			dwCount;
	ULONG			dwDelta;
} REFERENCE_COUNT_INFO, *PREFERENCE_COUNT_INFO;
////////////////////////////////////////////////////////////////////////////////
#define IOCTL_RTCORESYS_READ_MSR				CTL_CODE(FILE_DEVICE_RTCORESYS, RTCORESYS_IOCTL_INDEX + 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RTCORESYS_WRITE_MSR				CTL_CODE(FILE_DEVICE_RTCORESYS, RTCORESYS_IOCTL_INDEX + 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
	ULONG			dwIndex;
	ULONG			dwHigh;
	ULONG			dwLow;
} MSR_INFO, *PMSR_INFO;
////////////////////////////////////////////////////////////////////////////////
#define IOCTL_RTCORESYS_GETBUSDATA					CTL_CODE(FILE_DEVICE_RTCORESYS, RTCORESYS_IOCTL_INDEX + 20,  METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_RTCORESYS_SETBUSDATA					CTL_CODE(FILE_DEVICE_RTCORESYS, RTCORESYS_IOCTL_INDEX + 21,  METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct
{
    ULONG			dwBus;
    ULONG			dwDev;
	ULONG			dwFn;
	ULONG			dwOffset;
	ULONG			dwDataSize;
	ULONG			dwData;
} BUSDATA_INFO, *PBUSDATA_INFO;

#endif
////////////////////////////////////////////////////////////////////////////////
