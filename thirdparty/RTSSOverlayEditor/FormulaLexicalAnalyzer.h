// FormulaLexicalAnalyzer.h: interface for the CFormulaLexicalAnalyzer class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _FORMULALEXICALANALYZER_H_INCLUDED_
#define _FORMULALEXICALANALYZER_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
#ifndef _LEXICALANALYZER_H_INCLUDED_
#include "LexicalAnalyzer.h"
#endif
/////////////////////////////////////////////////////////////////////
#define LONG_AS_FLOAT(d) (*(FLOAT*)&d)
#define FLOAT_AS_LONG(f) (*(LONG*)&f)
#define FLT_MAX_AS_LONG	 0x7F7FFFFF
//////////////////////////////////////////////////////////////////////
class CFormulaLexicalAnalyzer : public CLexicalAnalyzer  
{
public:
	void Init();
		//initializes lexical analyzer's formula specific jump table

	BOOL IsVarLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is variable
	BOOL IsOperationCommaLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is comma operation
	BOOL IsOperationCloseBracketLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is close bracket operation
	BOOL IsOperationOpenBracketLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is open bracket operation
	BOOL IsOperationBracketLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is open or close bracket operation
	BOOL IsConstFltLexeme(LPLEXEME_DESC pDesc);
		//return TRUE if lexeme is floating point constant
	BOOL IsConstIntLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is integer constant
	BOOL IsConstLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is integer of floating point constant
	BOOL IsOperationLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is operation (+, -, *, /, %, (, ), ^, % etc)
	BOOL IsFnLexeme(LPLEXEME_DESC pDesc);
		//returns TRUE if lexeme is function

	CFormulaLexicalAnalyzer();
	virtual ~CFormulaLexicalAnalyzer();

private:
	virtual HRESULT PreAnalyze();
	virtual HRESULT PostAnalyze(WORD endState, BOOL bSuccessful);
	virtual HRESULT OnStateChange(WORD srcState, WORD dstState, char ch);

	HRESULT AppendLexemesList();
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
