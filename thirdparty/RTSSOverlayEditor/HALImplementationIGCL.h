// HALImplementationIGCL.h: interface for the CHALImplementationIGCL class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include "HALImplementation.h"
#include "IGCLWrapper.h"
//////////////////////////////////////////////////////////////////////
class CHALImplementationIGCL : public CHALImplementation
{
public:
	CHALImplementationIGCL();
	virtual ~CHALImplementationIGCL();

	void SetIGCLWrapper(CIGCLWrapper* lpIGCLWrapper);
	void SetGpu(DWORD dwGpu);

	virtual void SetTimestamp(DWORD dwTimestamp);

	//driver version

	virtual CString GetDriverStr();

	//GPU name

	virtual CString	GetName();

	//PCI location

	virtual BOOL GetLocation(DWORD* lpBus, DWORD* lpDev, DWORD* lpFn);

	//clock frequency monitoring

	virtual BOOL	IsCoreClockReportingSupported();
	virtual BOOL	IsMemoryClockReportingSupported();

	virtual DWORD	GetCurrentCoreClock();
	virtual DWORD	GetCurrentMemoryClock();

	virtual BOOL	IsEffectiveCoreClockReportingSupported();
	virtual BOOL	IsEffectiveMemoryClockReportingSupported();

	virtual DWORD	GetEffectiveCoreClock();
	virtual DWORD	GetEffectiveMemoryClock();

	//voltage monitoring

	virtual BOOL	IsCoreVoltageReportingSupported();
	virtual BOOL	IsMemoryVoltageReportingSupported();

	virtual DWORD	GetCurrentCoreVoltage();
	virtual DWORD	GetCurrentMemoryVoltage();

	//temperature monitoring

	virtual DWORD	GetCoreTemperatureSensorCount();
	virtual DWORD	GetMemoryTemperatureSensorCount();
	virtual DWORD	GetVRMTemperatureSensorCount();

	virtual LONG	GetCurrentCoreTemperature(int index);
	virtual LONG	GetCurrentMemoryTemperature(int index);
	virtual LONG	GetCurrentVRMTemperature(int index);

	//fan speed monitoring

	virtual DWORD	GetFanCount();
	virtual BOOL	IsFanSpeedReportingSupported(int index);
	virtual BOOL	IsFanSpeedRPMReportingSupported(int index);

	virtual DWORD	GetCurrentFanSpeed(int index);
	virtual DWORD	GetCurrentFanSpeedRPM(int index);

	//gpu usage monitoring

	virtual BOOL	IsGraphicsEngineUsageReportingSupported();
	virtual BOOL	IsMemoryControllerUsageReportingSupported();
	virtual BOOL	IsVideoEngineUsageReportingSupported();
	virtual BOOL	IsBusUsageReportingSupported();

	virtual DWORD	GetCurrentGraphicsEngineUsage();
	virtual DWORD	GetCurrentMemoryControllerUsage();
	virtual DWORD	GetCurrentVideoEngineUsage();
	virtual DWORD	GetCurrentBusUsage();

	//videomemory usage monitoring

	virtual BOOL	IsVideomemoryUsageReportingSupported();

	virtual DWORD	GetTotalVideomemory();
	virtual DWORD	GetCurrentVideomemoryUsage();

	//power monitoring

	virtual BOOL	IsRelPowerReportingSupported();
	virtual BOOL	IsAbsPowerReportingSupported();

	virtual DWORD	GetCurrentRelPower();
	virtual DWORD	GetCurrentAbsPower();

protected:
	CIGCLWrapper*	m_lpIGCLWrapper;
	DWORD			m_dwGpu;

	double GetTelemetryValue(ctl_oc_telemetry_item_t item);
	double GetTelemetryDelta(ctl_oc_telemetry_item_t item1, ctl_oc_telemetry_item_t item0, ctl_oc_telemetry_item_t time1, ctl_oc_telemetry_item_t time0);
};
//////////////////////////////////////////////////////////////////////
