// HALImplementation.h: interface for the CHALImplementation class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "D3DKMTWrapper.h"
#include "D3DKMTGPUUsage.h"
#include "TimestampedDataCache.h"
//////////////////////////////////////////////////////////////////////
#define POWER_REPORTING_UNITS_PERCENT								0
#define POWER_REPORTING_UNITS_MW									1
//////////////////////////////////////////////////////////////////////
class CHALImplementation
{
public:
	CHALImplementation();
	virtual ~CHALImplementation();

	//timestamped cache

	virtual void SetTimestamp(DWORD dwTimestamp);
	LPTIMESTAMPED_DATA SetCacheData(DWORD dwKey, DWORD dwData);
	LPTIMESTAMPED_DATA GetCacheData(DWORD dwKey);

	//unified D3DKMT based monitoring

	void	SetD3DKMTWrapper(CD3DKMTWrapper* lpD3DKMTWrapper);		
	void	SetD3DKMTDeviceDesc(LPD3DKMTDEVICE_DESC lpD3DKMTDeviceDesc);
	BOOL	BindD3DKMTDeviceDesc();

	DWORD	GetD3DKMTGpuUsage(DWORD dwNodeMask);
	DWORD	GetD3DKMTVideomemoryUsage(BOOL bShared, BOOL bDedicated, HANDLE hProcess);
	DWORD	GetD3DKMTTotalVideomemory(BOOL bShared, BOOL bDedicated);

	//driver version

	virtual CString GetDriverStr() = 0;

	//GPU name

	virtual CString	GetName() = 0;

	//PCI location

	virtual BOOL	GetLocation(DWORD* lpBus, DWORD* lpDev, DWORD* lpFn) = 0;

	//clock frequency monitoring

	virtual BOOL	IsCoreClockReportingSupported() = 0;
	virtual BOOL	IsMemoryClockReportingSupported() = 0;

	virtual DWORD	GetCurrentCoreClock() = 0;
	virtual DWORD	GetCurrentMemoryClock() = 0;

	virtual BOOL	IsEffectiveCoreClockReportingSupported() = 0;
	virtual BOOL	IsEffectiveMemoryClockReportingSupported() = 0;

	virtual DWORD	GetEffectiveCoreClock() = 0;
	virtual DWORD	GetEffectiveMemoryClock() = 0;

	//voltage monitoring

	virtual BOOL	IsCoreVoltageReportingSupported() = 0;
	virtual BOOL	IsMemoryVoltageReportingSupported() = 0;

	virtual DWORD	GetCurrentCoreVoltage() = 0;
	virtual DWORD	GetCurrentMemoryVoltage() = 0;

	//temperature monitoring

	virtual DWORD	GetCoreTemperatureSensorCount()		= 0;
	virtual DWORD	GetMemoryTemperatureSensorCount()	= 0;
	virtual DWORD	GetVRMTemperatureSensorCount()		= 0;

	virtual LONG	GetCurrentCoreTemperature(int index) = 0;
	virtual LONG	GetCurrentMemoryTemperature(int index) = 0;
	virtual LONG	GetCurrentVRMTemperature(int index) = 0;

	//fan speed monitoring

	virtual DWORD	GetFanCount() = 0;
	virtual BOOL	IsFanSpeedReportingSupported(int index) = 0;
	virtual BOOL	IsFanSpeedRPMReportingSupported(int index) = 0;

	virtual DWORD	GetCurrentFanSpeed(int index) = 0;
	virtual DWORD	GetCurrentFanSpeedRPM(int index) = 0;

	//gpu usage monitoring

	virtual BOOL	IsGraphicsEngineUsageReportingSupported() = 0;
	virtual BOOL	IsMemoryControllerUsageReportingSupported() = 0;
	virtual BOOL	IsVideoEngineUsageReportingSupported() = 0;
	virtual BOOL	IsBusUsageReportingSupported() = 0;

	virtual DWORD	GetCurrentGraphicsEngineUsage() = 0;
	virtual DWORD	GetCurrentMemoryControllerUsage() = 0;
	virtual DWORD	GetCurrentVideoEngineUsage() = 0;
	virtual DWORD	GetCurrentBusUsage() = 0;

	//videomemory usage monitoring

	virtual BOOL	IsVideomemoryUsageReportingSupported() = 0;

	virtual DWORD	GetTotalVideomemory() = 0;
	virtual DWORD	GetCurrentVideomemoryUsage() = 0;

	float	GetCurrentVideomemoryUsagePercent();
	DWORD	GetProcessVideomemoryUsage();
	float	GetProcessVideomemoryUsagePercent();

	//power monitoring

	virtual BOOL	IsRelPowerReportingSupported() = 0;
	virtual BOOL	IsAbsPowerReportingSupported() = 0;

	virtual DWORD	GetCurrentRelPower() = 0;
	virtual DWORD	GetCurrentAbsPower() = 0;

protected:
	CD3DKMTWrapper*		m_lpD3DKMTWrapper;		
	LPD3DKMTDEVICE_DESC m_lpD3DKMTDeviceDesc;

	CD3DKMTGPUUsage		m_d3dkmtGpuUsage;

	CTimestampedDataCache m_cache;

	DWORD				m_dwTimestamp;

	DWORD				m_dwTotalVideomemory;
};
//////////////////////////////////////////////////////////////////////
