// OverlayDataSource.h: interface for the COverlayDataSource class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#include "FormulaTranslatorVariableProvider.h"
/////////////////////////////////////////////////////////////////////////////
#define PROVIDER_HAL			"HAL"
#define PROVIDER_MSIAB			"MSI Afterburner"
#define PROVIDER_HWINFO			"HwInfo"
#define PROVIDER_AIDA			"AIDA"
#define PROVIDER_PERFCOUNTER	"PerfCounter"
#define PROVIDER_INTERNAL		""
/////////////////////////////////////////////////////////////////////////////
#define RING_BUFFER_SIZE	512	
	//must be power of 2!!!
/////////////////////////////////////////////////////////////////////////////
#define STAT_BUFFER_SIZE	8
	//must be power of 2!!!
/////////////////////////////////////////////////////////////////////////////
class COverlay;
class COverlayDataProvider;
class COverlayDataSourcesList;
class COverlayDataSource  : public CFormulaTranslatorVariableProvider
{
public:
	COverlayDataSource();
	virtual ~COverlayDataSource();

	void	SetOverlay(COverlay* lpOverlay);
		//sets overlay ptr
	COverlay* GetOverlay();
		//returns overlay ptr

	void	SetDataSourcesList(COverlayDataSourcesList* lpDataSourcesList);
		//sets data sources list ptr
	COverlayDataSourcesList* GetDataSourcesList();
		//returns data sources list ptr

	virtual CString GetDataStr();
		//returns formatted string with data 
	virtual CString	GetDataTag();
		//returns data tag (only for internal data sources)
	virtual CString GetProviderName() = 0;
		//returns data provider name

	COverlayDataProvider* GetProvider();
		//returns ptr to data provider
	void SetProvider(COverlayDataProvider* lpProvider);
		//sets ptr ro data provider

	virtual void Save();
		//saves data source
	virtual void Load();
		//loads data source
	virtual void Copy(COverlayDataSource* lpSrc);
		//copies data source
	virtual BOOL IsEqual(COverlayDataSource* lpSrc);
		//compares two data sources

	DWORD	GetSourceID();
		//returns data source ID
	void	SetSourceID(DWORD dwSourceID);
		//sets data source ID

	float	GetData();
		//returns last sampled data or FLT_MAX if data is invalid or not accesible
	void	SetData(float fltData);
		//sets last sampled data and adds it to ring data buffer
	void	CalcStat(float fltData);
		//calculates min/avg/max statistics

	float	GetStatMin();
		//returns statistics, minimum raw value
	float	GetStatAvg();
		//returns statistics, average raw value
	float	GetStatMax();
		//returns statistics, maximum raw value
	float	GetStatSwa();
		//returns statistics, raw value smoothed with sliding window average
	void	ResetStat();
		//resets min/avg/max statistics

	float	GetFormulaData();
		//returns formula input data
	void	SetFormulaData(float fltFormulaData);
		//sets formula input data

	CString GetName();
		//returns data source name
	void	SetName(LPCSTR lpName);
		//sets data source name

	CString GetUnits();
		//returns data soruce units
	void	SetUnits(LPCSTR lpUnits);
		//sets data source units

	CString GetFormat();
		//returns output format (e.g. %.4f)
	void	SetFormat(LPCSTR lpFormat);
		//sets output format
	BOOL	IsIntFormat();
		//returns TRUE if output format is integer (e.g %03d)

	CString GetFormula();
		//returns correction formula
	void	SetFormula(LPCSTR lpFormula);
		//sets and translates correction formula or retranslates currently set formula if lpFormula is NULL
	BOOL	IsValidFormula();
		//validates correction formula

	float*	GetBuffer();
		//returns ring buffer
	DWORD	GetBufferPos();
		//returns ring buffer position

	int	GetPriority();
		//returns priority for AIDA/HwInfo data sources
	void SetPriority(int nPriority);
		//sets priority for AIDA/HwInfo data sources
	void SetDataWithPriority(float fltData, int nPriority);
		//sets last sampled data and adds it to ring data buffer if specified nPriority is less than current priority

	BOOL IsDirty();
		//returns TRUE if dirty flag is set
	void SetDirty(BOOL bDirty);
		//sets/resets dirty flag
	
	BOOL	m_bSelected;

	static BOOL	ValidateFormat(LPCSTR lpLine);
	static BOOL	ValidateFloatFormat(LPCSTR lpLine);
	static BOOL	ValidateIntFormat(LPCSTR lpLine);
	static BOOL	ValidateFloat(LPCSTR lpLine);
	static CString FormatFloat(float fltVal, int nPrecision = 3);

protected:
	//CFormulaTranslatorVariableProvider virtual functions	

	virtual BOOL GetVariable(LPCSTR lpName, LPFORMULATRANSLATOR_VALUE_DESC pDesc);

	COverlay*					m_lpOverlay;
	COverlayDataSourcesList*	m_lpDataSourcesList;
	COverlayDataProvider*		m_lpProvider;
	DWORD						m_dwSourceID;

	CString						m_strName;
	CString						m_strUnits;
	CString						m_strFormat;
	BOOL						m_bIntFormat;
	CString						m_strFormula;

	CFormulaTranslator			m_translator;
	BOOL						m_bValidFormula;

	float						m_fltData;
	float						m_fltFormulaData;
	float						m_fltBuffer[RING_BUFFER_SIZE];
	DWORD						m_dwBufferPos;

	int							m_nPriority;

	BOOL						m_bDirty;

	DWORD						m_dwStatCount;
	float						m_fltStatBuf[STAT_BUFFER_SIZE];
	float						m_fltStatMin;
	float						m_fltStatAvg;
	float						m_fltStatMax;
};
/////////////////////////////////////////////////////////////////////////////
