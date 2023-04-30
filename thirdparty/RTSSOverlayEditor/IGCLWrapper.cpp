// IGCLWrapper.cpp: implementation of the CIGCLWrapper class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "IGCLWrapper.h"

#include "Log.h"
//////////////////////////////////////////////////////////////////////
#include "cApiWrapper.cpp"
//////////////////////////////////////////////////////////////////////
CIGCLWrapper::CIGCLWrapper()
{
	m_hApi				= NULL;

	m_dwVersion			= 0;
	m_dwGpuCount		= 0;

	ZeroMemory(m_hGpu			, sizeof(m_hGpu));
	ZeroMemory(m_dwBus			, sizeof(m_dwBus));
	ZeroMemory(m_dwDev			, sizeof(m_dwDev));

	m_dwTimestamp		= 0;

	ZeroMemory(m_dwPowerTelemetryTimestamp	, sizeof(m_dwPowerTelemetryTimestamp));
	ZeroMemory(m_powerTelemetryCurrent		, sizeof(m_powerTelemetryCurrent	));
	ZeroMemory(m_powerTelemetryPrevious		, sizeof(m_powerTelemetryPrevious	));
}
//////////////////////////////////////////////////////////////////////
CIGCLWrapper::~CIGCLWrapper()
{
	Uninit();
}
//////////////////////////////////////////////////////////////////////
DWORD CIGCLWrapper::GetGpuCount()
{
	return m_dwGpuCount;
}
//////////////////////////////////////////////////////////////////////
BOOL CIGCLWrapper::GetLocation(DWORD dwGpu, DWORD* lpBus, DWORD* lpDev, DWORD* lpFn)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if (lpBus)
		*lpBus = m_dwBus[dwGpu];

	if (lpDev)
		*lpDev = m_dwDev[dwGpu];

	if (lpFn)
		*lpFn = 0;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
#pragma warning (disable: 4996) 
//////////////////////////////////////////////////////////////////////
void CIGCLWrapper::Init(CD3DKMTWrapper* lpD3DKMTWrapper)
{
	APPEND_LOG("Initializing IGCL wrapper");

	//init API

    ctl_init_args_t args;
	ZeroMemory(&args, sizeof(args));
    args.AppVersion = CTL_MAKE_VERSION(CTL_IMPL_MAJOR_VERSION, CTL_IMPL_MINOR_VERSION);
    args.flags      = CTL_INIT_FLAG_USE_LEVEL_ZERO;
    args.Size       = sizeof(args);
    args.Version    = 0;

	if (ctlInit(&args, &m_hApi) != CTL_RESULT_SUCCESS)
	{
		APPEND_LOG("Failed to initialize IGCL API!");

		return;
	}

	//enumerate GPUs

	uint32_t dwGpuCount										= 0;
	ctl_device_adapter_handle_t	hGpu[IGCLWRAPPER_MAX_GPU]	= { 0 };

	if (ctlEnumerateDevices(m_hApi, &dwGpuCount, NULL) != CTL_RESULT_SUCCESS)
		//get device count on the first call
	{
		APPEND_LOG("Failed to enumerate IGCL devices!");

		return;
	}

	if (ctlEnumerateDevices(m_hApi, &dwGpuCount, hGpu) != CTL_RESULT_SUCCESS)
		//enumerate devices on the second call
	{
		APPEND_LOG("Failed to enumerate IGCL devices!");

		return;
	}

	APPEND_LOG1("%d IGCL devices detected", dwGpuCount);

	m_dwGpuCount = 0;

	for (DWORD dwGpu=0; dwGpu<dwGpuCount; dwGpu++)
	{
		ctl_device_adapter_properties_t adapterProperties;
		ZeroMemory(&adapterProperties, sizeof(adapterProperties));
		adapterProperties.Size = sizeof(adapterProperties);

		if (ctlGetDeviceProperties(hGpu[dwGpu], &adapterProperties) == CTL_RESULT_SUCCESS)
		{
			if (adapterProperties.device_type != CTL_DEVICE_TYPE_GRAPHICS)
				//skip non-graphics devices
				continue;

			if (adapterProperties.pci_vendor_id != 0x8086)
				//skip non-Intel devices
				continue;

			if (!m_dwGpuCount)
				//init Intel driver version for the first Intel GPU
			{
				m_dwVersion = adapterProperties.driver_version & 0xFFFF;
		
				APPEND_LOG1("Intel GPU driver version %d", m_dwVersion);
			}

			m_hGpu[m_dwGpuCount++] = hGpu[dwGpu];
		}
	}

	if (m_dwGpuCount)
	{
		APPEND_LOG1("%d Intel GPUs detected", m_dwGpuCount);
	}
	else
	{
		APPEND_LOG("There are no Intel GPUs detected");

		return;
	}

	//init GPU locations

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		ctl_pci_properties_t pciProperties;
		ZeroMemory(&pciProperties, sizeof(pciProperties));
		pciProperties.Size = sizeof(pciProperties);

		if (ctlPciGetProperties(m_hGpu[dwGpu], &pciProperties) == CTL_RESULT_SUCCESS)
		{
			m_dwBus[dwGpu]	= pciProperties.address.bus;
			m_dwDev[dwGpu]	= pciProperties.address.device;
		}
		else
			//x86 version of IGCL may fail to return PCI location, so we optionally try to detect it via D3DKMT wrapper by LUID
		{
			APPEND_LOG1("ctlPciGetProperties failed on GPU%d, using D3DKMT to detect location", dwGpu);

			LUID luid;

			if (GetLUID(dwGpu, luid))
			{
				LPD3DKMTDEVICE_DESC lpDesc = lpD3DKMTWrapper->FindDevice(luid);

				if (lpDesc)
				{
					m_dwBus[dwGpu] = lpDesc->Bus;
					m_dwDev[dwGpu] = lpDesc->Dev;
				}
			}
		}
	}

	//sort enumerated GPUs by location

	DWORD						dwBus[IGCLWRAPPER_MAX_GPU];
	DWORD						dwDev[IGCLWRAPPER_MAX_GPU];

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		DWORD dwGpu0 = IGCLWRAPPER_INVALID_GPU;

		for (DWORD dwGpu1=0; dwGpu1<m_dwGpuCount; dwGpu1++)
		{
			if (m_dwBus[dwGpu1])
			{
				if (dwGpu0 != IGCLWRAPPER_INVALID_GPU)
				{
					DWORD dwGpuId0 = (m_dwBus[dwGpu0]<<16) | (m_dwDev[dwGpu0]<<8);
					DWORD dwGpuId1 = (m_dwBus[dwGpu1]<<16) | (m_dwDev[dwGpu1]<<8);

					if (dwGpuId1 < dwGpuId0)
						dwGpu0 = dwGpu1;
				}
				else
					dwGpu0 = dwGpu1;
			}
			else
				//x86 version of IGCL may fail to return PCI location, so ensure that we handle the case with uninitialized location
				dwGpu0 = dwGpu1;
		}

		if (dwGpu0 != IGCLWRAPPER_INVALID_GPU)
		{
			hGpu[dwGpu]			= m_hGpu[dwGpu0];
			dwBus[dwGpu]		= m_dwBus[dwGpu0];
			dwDev[dwGpu]		= m_dwDev[dwGpu0];

			m_hGpu[dwGpu0]		= 0;
			m_dwBus[dwGpu0]		= 0;
			m_dwDev[dwGpu0]		= 0;
		}
	}

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
	{
		m_hGpu[dwGpu]			= hGpu[dwGpu];
		m_dwBus[dwGpu]			= dwBus[dwGpu];
		m_dwDev[dwGpu]			= dwDev[dwGpu];
	}

	//init power telemetry caps for each GPU

	SetTimestamp(GetTickCount());
		//do not forget to set timestamp to allow retreiving power telemetry

	for (DWORD dwGpu=0; dwGpu<m_dwGpuCount; dwGpu++)
		RetreivePowerTelemetry(dwGpu);
}
//////////////////////////////////////////////////////////////////////
#pragma warning (default: 4996) 
//////////////////////////////////////////////////////////////////////
void CIGCLWrapper::Uninit()
{
	if (m_hApi)
		ctlClose(m_hApi);

	m_hApi = NULL;
}
//////////////////////////////////////////////////////////////////////
DWORD CIGCLWrapper::GetVersion()
{
	return m_dwVersion;
}
//////////////////////////////////////////////////////////////////////
BOOL CIGCLWrapper::RetreivePowerTelemetry(DWORD dwGpu)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	if (m_dwPowerTelemetryTimestamp[dwGpu] != m_dwTimestamp)
	{
		m_dwPowerTelemetryTimestamp[dwGpu]	= m_dwTimestamp;

		m_powerTelemetryPrevious[dwGpu]		= m_powerTelemetryCurrent[dwGpu];

		ZeroMemory(&m_powerTelemetryCurrent[dwGpu], sizeof(m_powerTelemetryCurrent[dwGpu]));

		m_powerTelemetryCurrent[dwGpu].Size = sizeof(ctl_power_telemetry_t);

		if (ctlPowerTelemetryGet(m_hGpu[dwGpu], &m_powerTelemetryCurrent[dwGpu]) == CTL_RESULT_SUCCESS)
			return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
ctl_power_telemetry_t* CIGCLWrapper::GetCurrentPowerTelemetry(DWORD dwGpu)
{
	if (dwGpu >= m_dwGpuCount)
		return NULL;

	return &m_powerTelemetryCurrent[dwGpu];
}
//////////////////////////////////////////////////////////////////////
ctl_power_telemetry_t* CIGCLWrapper::GetPreviousPowerTelemetry(DWORD dwGpu)
{
	if (dwGpu >= m_dwGpuCount)
		return NULL;

	return &m_powerTelemetryPrevious[dwGpu];
}
//////////////////////////////////////////////////////////////////////
void CIGCLWrapper::SetTimestamp(DWORD dwTimestamp)
{
	m_dwTimestamp = dwTimestamp;
}
//////////////////////////////////////////////////////////////////////
BOOL CIGCLWrapper::GetName(DWORD dwGpu, CString& strName)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	ctl_device_adapter_properties_t adapterProperties;
	ZeroMemory(&adapterProperties, sizeof(adapterProperties));
	adapterProperties.Size = sizeof(adapterProperties);

	if (ctlGetDeviceProperties(m_hGpu[dwGpu], &adapterProperties) == CTL_RESULT_SUCCESS)
	{
		strName = adapterProperties.name;

		return TRUE;
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CIGCLWrapper::GetLUID(DWORD dwGpu, LUID& luid)
{
	if (dwGpu >= m_dwGpuCount)
		return FALSE;

	ctl_device_adapter_properties_t adapterProperties;
	ZeroMemory(&adapterProperties, sizeof(adapterProperties));
	adapterProperties.Size				= sizeof(adapterProperties);
	adapterProperties.pDeviceID			= &luid;
	adapterProperties.device_id_size	= sizeof(luid);

	if (ctlGetDeviceProperties(m_hGpu[dwGpu], &adapterProperties) == CTL_RESULT_SUCCESS)
		return TRUE;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CIGCLWrapper::GetTotalVideomemory(DWORD dwGpu, DWORD& dwTotalVideomemory)
{
	//total videomemory detection is not supported in x86 IGCL

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
