// FormulaLexicalAnalyzer.cpp: implementation of the CFormulaLexicalAnalyzer class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FormulaLexicalAnalyzer.h"
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
#ifndef _RTFCERRORS_H_INCLUDED_
#include "RTFCErrors.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define LEXEME_CLASS_OPERATION	0
#define LEXEME_CLASS_CONST_INT	1
#define LEXEME_CLASS_CONST_FLT	2
#define LEXEME_CLASS_VAR		3
#define LEXEME_CLASS_FN			4
//////////////////////////////////////////////////////////////////////
#define STATE_START				0
#define STATE_OPERATION			1
#define STATE_CONST_INT			2
#define STATE_CONST_DOT			3
#define STATE_CONST_FLT			4
#define STATE_VAR				5
#define STATE_VAR_QUOTED		6

#define STATE_LAST_VALID		7
//////////////////////////////////////////////////////////////////////
CFormulaLexicalAnalyzer::CFormulaLexicalAnalyzer()
{
	Init();
}
//////////////////////////////////////////////////////////////////////
CFormulaLexicalAnalyzer::~CFormulaLexicalAnalyzer()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CFormulaLexicalAnalyzer::OnStateChange(WORD srcState, WORD dstState, char ch)
{
	HRESULT errCode;

	switch (srcState)
	{
	case STATE_START:
		switch (dstState)
		{
		case STATE_START:
			ResetLexemeStr();
			break;
		case STATE_CONST_INT:
			CreateLexemeDesc(LEXEME_CLASS_CONST_INT);
			SetLexemeStr(ch);
			break;
		case STATE_CONST_DOT:
			CreateLexemeDesc(LEXEME_CLASS_CONST_FLT);
			SetLexemeStr(ch);
			break;
		case STATE_OPERATION:
			CreateLexemeDesc(LEXEME_CLASS_OPERATION);
			SetLexemeStr(ch);
			break;
		case STATE_VAR:
			CreateLexemeDesc(LEXEME_CLASS_VAR);
			SetLexemeStr(ch);
			break;
		case STATE_VAR_QUOTED:
			CreateLexemeDesc(LEXEME_CLASS_VAR);
			ResetLexemeStr();
			break;
		}
		break;
	case STATE_OPERATION:
		if (FAILED(errCode = AppendLexemesList()))
			return errCode;
		break;
	case STATE_CONST_INT:
		switch (dstState)
		{
		case STATE_CONST_INT:
			AppendLexemeStr(ch);
			break;
		case STATE_CONST_DOT:
			AppendLexemeStr(ch);
			break;
		case STATE_START:
			if (FAILED(errCode = AppendLexemesList()))
				return errCode;
			break;
		}
		break;
	case STATE_CONST_DOT:
		switch (dstState)
		{
		case STATE_START:
			return -1;
		case STATE_CONST_FLT:
			LPLEXEME_DESC pLexeme = GetUnhandledLexeme();
			if (pLexeme)
				pLexeme->dwClass = LEXEME_CLASS_CONST_FLT;
			AppendLexemeStr(ch);
			break;
		}
		break;
	case STATE_CONST_FLT:
		switch (dstState)
		{
		case STATE_CONST_FLT:
			AppendLexemeStr(ch);
			break;
		case STATE_START:
			if (FAILED(errCode = AppendLexemesList()))
				return errCode;
			break;
		}
		break;
	case STATE_VAR:
		switch (dstState)
		{
		case STATE_VAR:
			AppendLexemeStr(ch);
			break;
		case STATE_START:
			if (FAILED(errCode = AppendLexemesList()))
				return errCode;
			break;
		}
		break;
	case STATE_VAR_QUOTED:
		switch (dstState)
		{
		case STATE_VAR_QUOTED:
			AppendLexemeStr(ch);
			break;
		case STATE_START:
			if (FAILED(errCode = AppendLexemesList()))
				return errCode;
			break;
		}
		break;
	}

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
HRESULT CFormulaLexicalAnalyzer::PreAnalyze()
{
	DestroyLexemes();

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
HRESULT CFormulaLexicalAnalyzer::PostAnalyze(WORD endState, BOOL bSuccessful)
{
	HRESULT errCode;
	
	if (bSuccessful)
	{
		if (endState == STATE_VAR_QUOTED)
			//if we finished analysis in quoted variable state, then closing quote mark is missing
			return FORMULATRANSLATOR_E_QUOTE_EXPECTED;

		if (GetUnhandledLexeme())
			if (FAILED(errCode = AppendLexemesList()))
				return errCode;

		//on the final pass, we process parsed lexemes list and detect unary minus and function lexemes

		LPLEXEME_DESC pLexeme		= GetHeadLexeme();
		LPLEXEME_DESC pLexemePrev	= NULL;

		while (pLexeme)
		{
			if (IsOperationLexeme(pLexeme))
			{
				if (pLexeme->dwData == '-')
				{
					if (!pLexemePrev || (IsOperationLexeme(pLexemePrev) && !IsOperationCloseBracketLexeme(pLexemePrev)))
						//unary minus lexeme is identified by lexeme operand counter set to 1, for binary operations it is 0 by default
						pLexeme->dwCounter = 1;
				}

				if (IsOperationOpenBracketLexeme(pLexeme) && IsVarLexeme(pLexemePrev))
					//function lexeme
					pLexemePrev->dwClass = LEXEME_CLASS_FN;
			}

			pLexemePrev	= pLexeme;
			pLexeme		= GetNextLexeme();
		}
	}
	else
		DestroyLexemes();

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
void CFormulaLexicalAnalyzer::Init()
{
	UninitCharset();
	AppendCharset(" \t+-*/%^().,0123456789");
	AppendCharset('a','z');
	AppendCharset('A','Z');
	AppendCharset('"');

	InitJumpTable(STATE_LAST_VALID + 1);

	AppendJumpTable(STATE_START		, STATE_START			, 0	, " \t");
	AppendJumpTable(STATE_START		, STATE_CONST_INT		, 0	, '0', '9');
	AppendJumpTable(STATE_START		, STATE_CONST_DOT		, 0	, '.');
	AppendJumpTable(STATE_START		, STATE_OPERATION		, 0 , "+-*/%()^,");
	AppendJumpTable(STATE_START		, STATE_VAR				, 0 , 'a', 'z');
	AppendJumpTable(STATE_START		, STATE_VAR				, 0 , 'A', 'Z');
	AppendJumpTable(STATE_START		, STATE_VAR_QUOTED		, 0 , '"');

	AppendJumpTable(STATE_OPERATION	, STATE_START			, JTF_DONT_INC_CHAR_INDEX);

	AppendJumpTable(STATE_CONST_INT	, STATE_START			, JTF_DONT_INC_CHAR_INDEX);
	AppendJumpTable(STATE_CONST_INT	, STATE_CONST_INT		, 0	, '0', '9');
	AppendJumpTable(STATE_CONST_INT	, STATE_CONST_DOT		, 0	, '.');

	AppendJumpTable(STATE_CONST_DOT	, STATE_START			, 0);
	AppendJumpTable(STATE_CONST_DOT	, STATE_CONST_FLT		, 0	, '0', '9');

	AppendJumpTable(STATE_CONST_FLT	, STATE_START			, JTF_DONT_INC_CHAR_INDEX);
	AppendJumpTable(STATE_CONST_FLT	, STATE_CONST_FLT		, 0	, '0','9');

	AppendJumpTable(STATE_VAR		, STATE_START			, JTF_DONT_INC_CHAR_INDEX);
	AppendJumpTable(STATE_VAR		, STATE_VAR				, 0	, 'a', 'z');
	AppendJumpTable(STATE_VAR		, STATE_VAR				, 0	, 'A', 'Z');
	AppendJumpTable(STATE_VAR		, STATE_VAR				, 0	, '0', '9');

	AppendJumpTable(STATE_VAR_QUOTED, STATE_START			, 0 , '"');
	AppendJumpTable(STATE_VAR_QUOTED, STATE_VAR_QUOTED		, 0	, " \t");
	AppendJumpTable(STATE_VAR_QUOTED, STATE_VAR_QUOTED		, 0	, 'a', 'z');
	AppendJumpTable(STATE_VAR_QUOTED, STATE_VAR_QUOTED		, 0	, 'A', 'Z');
	AppendJumpTable(STATE_VAR_QUOTED, STATE_VAR_QUOTED		, 0	, '0', '9');

	ValidateJumpTable();
}
//////////////////////////////////////////////////////////////////////
HRESULT CFormulaLexicalAnalyzer::AppendLexemesList()
{
	LPLEXEME_DESC pLexeme = GetUnhandledLexeme();

	if (!pLexeme)
		return FORMULALEXICALANALYZER_E_NO_LEXEME;

	LPCSTR lpszLexeme = GetLexemeStr();

	switch (pLexeme->dwClass)
	{
	case LEXEME_CLASS_OPERATION:
		if (sscanf_s(lpszLexeme," %c", &pLexeme->dwData, sizeof(char)) != 1)
			return -1;
		break;
	case LEXEME_CLASS_CONST_INT:
		if (sscanf_s(lpszLexeme, "%d", &pLexeme->dwData) != 1)
			return -1;
		break;
	case LEXEME_CLASS_CONST_FLT:
		if (sscanf_s(lpszLexeme, "%f", &pLexeme->dwData) != 1)
			return -1;
		break;
	case LEXEME_CLASS_VAR:
		pLexeme->dwData = strlen(lpszLexeme) + 1;
		pLexeme->lpData = new BYTE[pLexeme->dwData];
		strcpy_s((LPSTR)pLexeme->lpData, pLexeme->dwData, lpszLexeme);
		break;
	case LEXEME_CLASS_FN:
		pLexeme->dwData = strlen(lpszLexeme) + 1;
		pLexeme->lpData = new BYTE[pLexeme->dwData];
		strcpy_s((LPSTR)pLexeme->lpData, pLexeme->dwData, lpszLexeme);
		break;
	}

	AddUnhandledLexeme();

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsOperationLexeme(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		return (pDesc->dwClass == LEXEME_CLASS_OPERATION);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsConstLexeme(LPLEXEME_DESC pDesc)
{
	return IsConstIntLexeme(pDesc) || IsConstFltLexeme(pDesc);
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsConstIntLexeme(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		return (pDesc->dwClass == LEXEME_CLASS_CONST_INT);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsConstFltLexeme(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		return (pDesc->dwClass == LEXEME_CLASS_CONST_FLT);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsOperationBracketLexeme(LPLEXEME_DESC pDesc)
{
	return IsOperationOpenBracketLexeme(pDesc) || IsOperationCloseBracketLexeme(pDesc);
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsOperationOpenBracketLexeme(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		return (pDesc->dwClass == LEXEME_CLASS_OPERATION) && (pDesc->dwData == '(');

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsOperationCloseBracketLexeme(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		return (pDesc->dwClass == LEXEME_CLASS_OPERATION) && (pDesc->dwData == ')');

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsOperationCommaLexeme(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		return (pDesc->dwClass == LEXEME_CLASS_OPERATION) && (pDesc->dwData == ',');

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsVarLexeme(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		return (pDesc->dwClass == LEXEME_CLASS_VAR);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaLexicalAnalyzer::IsFnLexeme(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		return (pDesc->dwClass == LEXEME_CLASS_FN);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
