// OverlayMacroList.h: interface for the COverlayMacroList class.
//
// created by Unwinder
/////////////////////////////////////////////////////////////////////////////
#pragma once
/////////////////////////////////////////////////////////////////////////////
#define MACROTYPE_MACRO													0
#define MACROTYPE_INTERNAL_SOURCE										1
#define MACROTYPE_EXTERNAL_SOURCE										2
/////////////////////////////////////////////////////////////////////////////
class COverlay;
class COverlayDataSourcesList;
class COverlayMacroList : public CStringList
{
public:
	COverlayMacroList();
	virtual ~COverlayMacroList();

	void	Init(COverlay* lpOverlay);
		//fills macro list
	void	Append(LPCSTR lpMacroName, LPCSTR lpMacroValue, int type = MACROTYPE_MACRO);
		//adds macro to the list
	void	Append(CStringList* lpMacroList, int type = MACROTYPE_MACRO);
		//add macroses from precreated list
	void	Append(COverlayDataSourcesList* lpSourcesList, int type);	
		//add macroses from data sources list (%NAME% = VALUE)

	void	SetTimer(LPCSTR lpParams);
		//set timer, params specify positive value for increasing or negative 
		//value for decreasing target in seconds
	CString	FormatTimer();
		//formats increasing or decreasing timer value
	BOOL	GetTimerFlashing(DWORD dwPeriod);
		//returns current timer flashing state (used for %TimerFlashing% macro)
	BOOL	GetTimerVisibility();
		//returns current timer visibility state (used for %TimerHiding% macro)

	int GetType(int index);
		//returns macro type by index

protected:
	CArray<DWORD, DWORD> m_types;

	DWORD	m_dwTimerTimestampBeg;
	DWORD	m_dwTimerTimestampEnd;
	LONG	m_dwTimerTarget;
};
/////////////////////////////////////////////////////////////////////////////
