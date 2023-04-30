// FormulaTranslatorVariableProvider.h: interface for the CFormulaTranslatorVariableProvider class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _FORMULATRANSLATORVARIABLEPROVIDER_H_INCLUDED_
#define _FORMULATRANSLATORVARIABLEPROVIDER_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#include "FormulaTranslator.h"
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
class CFormulaTranslatorVariableProvider  
{
public:
	virtual BOOL GetVariable(LPCSTR lpName, LPFORMULATRANSLATOR_VALUE_DESC pDesc) = 0;
		//pure virtual function, called by formula translator to get variable value

	CFormulaTranslatorVariableProvider();
	virtual ~CFormulaTranslatorVariableProvider();
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
