// FormulaTranslator.h: interface for the CFormulaTranslator class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _FORMULATRANSLATOR_H_INCLUDED_
#define _FORMULATRANSLATOR_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
#ifndef _PTRSTACK_H_INCLUDED_
#include "PtrStack.h"
#endif
#ifndef _STRINGLITE_H_INCLUDED_
#include "StringLite.h"
#endif
#ifndef _PTRLISTLITE_H_INCLUDED_
#include "PtrListLite.h"
#endif
#ifndef _FORMULALEXICALANALYZER_H_INCLUDED_
#include "FormulaLexicalAnalyzer.h"
#endif
//////////////////////////////////////////////////////////////////////
#define FORMULATRANSLATOR_VALUE_TYPE_INT		0
#define FORMULATRANSLATOR_VALUE_TYPE_FLT		1
//////////////////////////////////////////////////////////////////////
typedef struct FORMULATRANSLATOR_VALUE_DESC
{
	DWORD	dwType;
	LONG	dwData;
} FORMULATRANSLATOR_VALUE_DESC, *LPFORMULATRANSLATOR_VALUE_DESC;
//////////////////////////////////////////////////////////////////////
class CFormulaTranslatorVariableProvider;
//////////////////////////////////////////////////////////////////////
class CFormulaTranslator : public CFormulaLexicalAnalyzer 
{
public:
	HRESULT Translate(LPCSTR lpFormula);
		//Performs lexical analysis for the specified formula and translates it
		//to revese polish notation. Once the formula is translated, it can be
		//calculated with Calc function
		//Warning! Lexical analyser must be initialized with Init function before
		//translation
	HRESULT Calc();
		//Calculates previously translated formula. Once a formula is calculated,
		//you can use IsIntValue / IsFltValue functions to check result type and 
		//GetIntValue / GetFltValue funtions to retreive it

	void SetVariableProvider(CFormulaTranslatorVariableProvider* pVariableProvider);
		//sets variable provider

		//variable provider must be set as soon as the formula contains any 
		//variables

	BOOL IsIntValue();
		//returns TRUE if calculated result is integer
	BOOL IsFltValue();
		//returns TRUE if calculated result if float

	LONG GetIntValue();
		//returns integer result
	FLOAT GetFltValue();
		//returns float result

	void DestroyOperandsStack();
		//destroys operands stack

	CStringLite Format();
		//formats string with reverse polish notation of the last translated formula

	CFormulaTranslator();
	virtual ~CFormulaTranslator();

private:

	//helpers for converting lexemes list to reverse polish notation

	DWORD InitLexemePriority(LPLEXEME_DESC pLexeme);
		//initializes operation lexeme priority

	static int SortLexemesHelper(LPPTR_LIST_NODE pLexemeNode0, LPPTR_LIST_NODE pLexemeNode1);
		//helper for sorting lexeme descriptors (using lexeme sort index)

	//helpers for reverse polish notation calculation

	BOOL CalcUnaryMinus(LPFORMULATRANSLATOR_VALUE_DESC pOperand);
	BOOL CalcFn(LPCSTR lpName, LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1);
	BOOL CalcFn(LPCSTR lpName, LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1, LPFORMULATRANSLATOR_VALUE_DESC pDesc);
	int  GetFnOperandCount(LPCSTR lpName);

	BOOL CalcAdd(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1);
	BOOL CalcSub(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1);
	BOOL CalcMul(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1);
	BOOL CalcDiv(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1);
	BOOL CalcRem(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1);
	BOOL CalcPow(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1);
		//calculation helpers (do required calc then push result to stack)

	void PushVal(DWORD dwType, LONG dwValue);
		//pushes constant in operands stack
	BOOL PushVar(LPCSTR lpName);
		//requests variable via provider and pushes it in operands
		//stack as a constant

	static BOOL IsIntValue(LPFORMULATRANSLATOR_VALUE_DESC pDesc);
	static BOOL IsFltValue(LPFORMULATRANSLATOR_VALUE_DESC pDesc);
		//operand type verification helpers

	static LONG  GetIntValue(LPFORMULATRANSLATOR_VALUE_DESC pDesc);
	static FLOAT GetFltValue(LPFORMULATRANSLATOR_VALUE_DESC pDesc);
		//operands access helpers

	CPtrStack m_stack;
		//operands/operations stack

	CFormulaTranslatorVariableProvider* m_pVariableProvider;
		//ptr to assocuited variable provider
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
