// ForumaTranslator.cpp: implementation of the CFormulaTranslator class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FormulaTranslator.h"
//////////////////////////////////////////////////////////////////////
#include <math.h>
//////////////////////////////////////////////////////////////////////
#ifndef _RTFCERRORS_H_INCLUDED_
#include "RTFCErrors.h"
#endif
#ifndef _FORMULALEXICALANALYZER_H_INCLUDED_
#include "FormulaLexicalAnalyzer.h"
#endif

#include "FormulaTranslatorVariableProvider.h"

#include <float.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFormulaTranslator::CFormulaTranslator()
{
	m_pVariableProvider = NULL;

	Init();
}
//////////////////////////////////////////////////////////////////////
CFormulaTranslator::~CFormulaTranslator()
{
	DestroyOperandsStack();
}
//////////////////////////////////////////////////////////////////////
HRESULT CFormulaTranslator::Translate(LPCSTR lpFormula)
{
	DestroyOperandsStack();
		//destroy operands stack, we'll use it as operation stack during translation

	DWORD dwIndex	= 0;
		//init lexeme sort index

	HRESULT errCode = Analyze(lpFormula);
		//do lexical analysis

	if (FAILED(errCode))
		return errCode;
			//return error if lexical analysis failed

	LPLEXEME_DESC pLexeme = GetHeadLexeme();

	while (pLexeme)
		//process lexemes list and translate formula to RPN
	{
		if (IsFnLexeme(pLexeme))
			//if it is function lexeme, we simply push it in operations stack
		{
			InitLexemePriority(pLexeme);

			m_stack.Push(pLexeme);
		}
		else
		if (IsOperationLexeme(pLexeme))
			//if it is operation lexeme, we'll push it in operations stack
		{
			if (IsOperationCommaLexeme(pLexeme))
				//if it is comma operation lexeme, i.e function operand separator, we'll pop operands from stack till function lexeme 
				//is on top of stack and increment operand counter for it
			{
				LPLEXEME_DESC pLexemeFromStack = NULL;

				while (!m_stack.IsEmpty())
				{
					pLexemeFromStack = (LPLEXEME_DESC)m_stack.Peek();
					
					if (IsFnLexeme(pLexemeFromStack))
						break;
					else
					{
						pLexemeFromStack = (LPLEXEME_DESC)m_stack.Pop();

						pLexemeFromStack->dwReserved = dwIndex++;
					}
				}

				//if there is no function lexeme on top of stack, then formula is invalid

				if (m_stack.IsEmpty())
				{
					m_stack.RemoveAll();
						//don't forget to clean up operations stack before exit

					return FORMULATRANSLATOR_E_BRACKET_EXPECTED;
				}

				pLexemeFromStack = (LPLEXEME_DESC)m_stack.Peek();
					
				if (!IsFnLexeme(pLexemeFromStack))
				{
					m_stack.RemoveAll();
						//don't forget to clean up operations stack before exit

					return FORMULATRANSLATOR_E_BRACKET_EXPECTED;
				}

				//increment operand counter for function lexeme

				pLexemeFromStack->dwCounter++;
			}
			else
			{
				DWORD dwPriority = InitLexemePriority(pLexeme);
					//try to init operation priority 

				if (dwPriority != 0xffffffff)
					//if operation priority is initialized properly, push it
				{
					if (m_stack.IsEmpty())
						//if the stack is empty, simply push the operation
						m_stack.Push(pLexeme);
					else
						//otherwise analyze operation type and process brackets
					{
						if (IsOperationOpenBracketLexeme(pLexeme))
							//if it is open bracket, simply push it if the previous lexeme was not function, otherwise ignore it 
						{
							LPLEXEME_DESC pPrevLexeme = PeekLexeme(-1);

							if (IsFnLexeme(pPrevLexeme))
								//previous lexeme is function, so we're ignoring this open bracket
							{
								LPLEXEME_DESC pNextLexeme = PeekLexeme(1);

								//if the next lexeme is close bracket operation lexeme, then it is function with no operands

								if (IsOperationCloseBracketLexeme(pNextLexeme))
									pPrevLexeme->dwCounter = 0;
								else
									pPrevLexeme->dwCounter = 1;
							}
							else	
								//we're simply pushing this open bracket into operations stack
								m_stack.Push(pLexeme);
						}
						else
							if (IsOperationCloseBracketLexeme(pLexeme))
								//if it is close bracket, pop all operations until
								//open bracket or function is poped
							{
								LPLEXEME_DESC pLexemeFromStack = NULL;

								while (!m_stack.IsEmpty())
								{
									pLexemeFromStack = (LPLEXEME_DESC)m_stack.Pop();
									
									if (IsOperationOpenBracketLexeme(pLexemeFromStack) || IsFnLexeme(pLexemeFromStack))
										break;

									pLexemeFromStack->dwReserved = dwIndex++;
								}

								if (!pLexemeFromStack || (!IsOperationOpenBracketLexeme(pLexemeFromStack) && !IsFnLexeme(pLexemeFromStack)))
									//if we haven't found open bracket, then formula is invalid
								{
									m_stack.RemoveAll();
										//don't forget to clean up operations stack before exit

									return FORMULATRANSLATOR_E_UNEXPECTED_BRACKET;
									//if we have not met close bracket, then formula is invalid
								}

								if (IsFnLexeme(pLexemeFromStack))
									//if we're popping function lexeme instead of close bracket, do not forger to set sort index for it
									//because unlike bracket it needs to get to output
									pLexemeFromStack->dwReserved = dwIndex++;
							}
							else
								//otherwise we'll pop all the operations from the stack
								//until operation with less or equal priority is at stack
							{

								LPLEXEME_DESC pLexemeFromStack = (LPLEXEME_DESC)m_stack.Peek();	

								if (pLexemeFromStack->dwReserved < dwPriority)
									m_stack.Push(pLexeme);
								else
								{
									while (pLexemeFromStack->dwReserved > dwPriority)
									{
										pLexemeFromStack				= (LPLEXEME_DESC)m_stack.Pop();
										pLexemeFromStack->dwReserved	= dwIndex++;

										pLexemeFromStack				= (LPLEXEME_DESC)m_stack.Peek();	

										if (!pLexemeFromStack)
											break;
									}

									m_stack.Push(pLexeme);
								}
							}
					}
				}
				else
				{
					m_stack.RemoveAll();
						//don't forget to clean up operations stack before exit

					return FORMULATRANSLATOR_E_UNKNOWN_OPERATION;
				}
			}
		}
		else
			//otherwise it is operand, and we simply set its' lexeme sort index
			pLexeme->dwReserved = dwIndex++; 

		pLexeme = GetNextLexeme();
	}

	while (!m_stack.IsEmpty())
		//pop operations from the stack and return error if there are any bracket or function
		//operations left
	{
		LPLEXEME_DESC pLexemeFromStack	= (LPLEXEME_DESC)m_stack.Pop();
		pLexemeFromStack->dwReserved	= dwIndex++;	

		if (IsOperationBracketLexeme(pLexemeFromStack) || IsFnLexeme(pLexemeFromStack))
		{
			m_stack.RemoveAll();
				//don't forget to clean up operations stack before exit

			return FORMULATRANSLATOR_E_BRACKET_EXPECTED;
		}
	}

	m_stack.RemoveAll();
		//clean up operations stack

	//loop through the list of lexemes and remove all bracket and comma operation lexemes

	pLexeme = GetHeadLexeme();

	while (pLexeme)
		if (IsOperationBracketLexeme(pLexeme) || IsOperationCommaLexeme(pLexeme))
			pLexeme = DestroyThisLexeme();
		else
			pLexeme = GetNextLexeme();

	m_lexemesList.Sort(TRUE, SortLexemesHelper);
		//sort lexemes list using lexeme indices we set

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
DWORD CFormulaTranslator::InitLexemePriority(LPLEXEME_DESC lpLexeme)
{
	DWORD dwPriority = 0xFFFFFFFF;

	if (IsFnLexeme(lpLexeme))
		dwPriority = 0;

	if (IsOperationLexeme(lpLexeme))
	{
		switch (lpLexeme->dwData)
		{
		case '(':
			dwPriority = 0;
			break;
		case ')':
			dwPriority = 1;
			break;
		case '+':
		case '-':
			if (lpLexeme->dwCounter == 1)
				//unary minus lexeme is identified by lexeme operand counter set to 1, for binary operations it is 0 by default
				dwPriority = 5;
			else
				dwPriority = 2;
			break;
		case '*':
		case '/':
		case '%':
			dwPriority = 3;
			break;
		case '^':
			dwPriority = 4;
			break;
		}
	}

	if (dwPriority != 0xFFFFFFFF)
		lpLexeme->dwReserved = dwPriority;

	return dwPriority;
}
//////////////////////////////////////////////////////////////////////
int CFormulaTranslator::SortLexemesHelper(LPPTR_LIST_NODE pLexemeNode0, LPPTR_LIST_NODE pLexemeNode1)
{
	LPLEXEME_DESC pLexeme0 = (LPLEXEME_DESC)pLexemeNode0->pData;
	LPLEXEME_DESC pLexeme1 = (LPLEXEME_DESC)pLexemeNode1->pData;

	if (pLexeme0->dwReserved > pLexeme1->dwReserved)
		return 1;

	if (pLexeme0->dwReserved < pLexeme1->dwReserved)
		return -1;

	return 0;
}
//////////////////////////////////////////////////////////////////////
CStringLite CFormulaTranslator::Format()
{
	CStringLite strResult;

	LPLEXEME_DESC pLexeme = GetHeadLexeme();

	while (pLexeme)
	{
		CStringLite strLexeme;

		if (IsOperationLexeme(pLexeme))
		{
			if (pLexeme->dwCounter)
				//unary minus lexeme is identified by lexeme operand counter set to 1, for binary operations it is 0 by default
				strLexeme.Format("%c@%d", pLexeme->dwData, pLexeme->dwCounter);
			else
				strLexeme.Format("%c", pLexeme->dwData);
		}

		if (IsConstIntLexeme(pLexeme))
			strLexeme.Format("%d", pLexeme->dwData);

		if (IsConstFltLexeme(pLexeme))
			strLexeme.Format("%f", LONG_AS_FLOAT(pLexeme->dwData));

		if (IsVarLexeme(pLexeme))
			strLexeme.Format("%s", pLexeme->lpData);

		if (IsFnLexeme(pLexeme))
			strLexeme.Format("%s@%d", pLexeme->lpData, pLexeme->dwCounter);

		if (!strResult.IsEmpty())
			strResult += " ";

		strResult += strLexeme;

		pLexeme = GetNextLexeme();

	}

	return strResult;
}
//////////////////////////////////////////////////////////////////////
HRESULT CFormulaTranslator::Calc()
{
	DestroyOperandsStack();

	LPLEXEME_DESC pLexeme = GetHeadLexeme();

	while (pLexeme)
	{
		if (IsConstIntLexeme(pLexeme))
			PushVal(FORMULATRANSLATOR_VALUE_TYPE_INT, pLexeme->dwData);

		if (IsConstFltLexeme(pLexeme))
			PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, pLexeme->dwData);

		if (IsVarLexeme(pLexeme))
		{
			if (!PushVar((LPSTR)pLexeme->lpData))
				return FORMULATRANSLATOR_E_UNDECLARED_VARIABLE;
		}

		if (IsFnLexeme(pLexeme))
		{
			int nOperandCount = GetFnOperandCount((LPSTR)pLexeme->lpData);

			if (nOperandCount == -1)
				return FORMULATRANSLATOR_E_UNDECLARED_FN;

			if (nOperandCount < pLexeme->dwCounter)
				return FORMULATRANSLATOR_E_UNEXPECTED_COMMA;

			if (nOperandCount > pLexeme->dwCounter)
				return FORMULATRANSLATOR_E_OPERAND_EXPECTED;

			LPFORMULATRANSLATOR_VALUE_DESC pOperand1 = NULL;
			LPFORMULATRANSLATOR_VALUE_DESC pOperand0 = NULL;

			if (nOperandCount > 1)
			{
				pOperand1 = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Pop();

				if (!pOperand1)
					return FORMULATRANSLATOR_E_OPERAND_EXPECTED;
			}

			if (nOperandCount > 0)
			{
				pOperand0 = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Pop();

				if (!pOperand0)
				{
					if (pOperand1)
						delete pOperand1;

					return FORMULATRANSLATOR_E_OPERAND_EXPECTED;
				}
			}

			if (!CalcFn((LPSTR)pLexeme->lpData, pOperand0, pOperand1))
			{
				if (pOperand0)
					delete pOperand0;

				if (pOperand1)
					delete pOperand1;

				return FORMULATRANSLATOR_E_UNDECLARED_FN;
			}

			if (pOperand0)
				delete pOperand0;

			if (pOperand1)
				delete pOperand1;
		}

		if (IsOperationLexeme(pLexeme))
		{
			if ((pLexeme->dwData == '-') && (pLexeme->dwCounter == 1))
				//unary minus lexeme is identified by lexeme operand counter set to 1, for binary operations it is 0 by default
			{
				LPFORMULATRANSLATOR_VALUE_DESC pOperand = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Pop();

				if (!pOperand)
					return FORMULATRANSLATOR_E_OPERAND_EXPECTED;

				CalcUnaryMinus(pOperand);

				if (pOperand)
					delete pOperand;
			}
			else
			{
				LPFORMULATRANSLATOR_VALUE_DESC pOperand1 = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Pop();
				LPFORMULATRANSLATOR_VALUE_DESC pOperand0 = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Pop();

				if (!pOperand0 || !pOperand1)
				{
					if (pOperand0)
						delete pOperand0;

					if (pOperand1)
						delete pOperand1;

					return FORMULATRANSLATOR_E_OPERAND_EXPECTED;
				}

				switch (pLexeme->dwData)
				{
				case '+':
					CalcAdd(pOperand0, pOperand1);
					break;
				case '-':
					CalcSub(pOperand0, pOperand1);
					break;
				case '*':
					CalcMul(pOperand0, pOperand1);
					break;
				case '/':
					CalcDiv(pOperand0, pOperand1);
					break;
				case '%':
					CalcRem(pOperand0, pOperand1);
					break;
				case '^':
					CalcPow(pOperand0, pOperand1);
					break;
				}

				if (pOperand0)
					delete pOperand0;

				if (pOperand1)
					delete pOperand1;
			}
		}

		pLexeme = GetNextLexeme();
	}

	if (m_stack.GetCount() > 1)
		return FORMULATRANSLATOR_E_OPERATION_EXPECTED;

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
void CFormulaTranslator::PushVal(DWORD dwType, LONG dwData)
{
	LPFORMULATRANSLATOR_VALUE_DESC pDesc = new FORMULATRANSLATOR_VALUE_DESC;

	pDesc->dwType = dwType;
	pDesc->dwData = dwData;

	m_stack.Push(pDesc);
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::PushVar(LPCSTR lpName)
{
	if (!m_pVariableProvider)
		return FALSE;

	LPFORMULATRANSLATOR_VALUE_DESC pDesc = new FORMULATRANSLATOR_VALUE_DESC;

	if (!m_pVariableProvider->GetVariable(lpName, pDesc))
	{
		delete pDesc;
		return FALSE;
	}

	m_stack.Push(pDesc);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcUnaryMinus(LPFORMULATRANSLATOR_VALUE_DESC pOperand)
{
	if (IsIntValue(pOperand))
	{
		LONG val	= -GetIntValue(pOperand);

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_INT, val);
	}
	else
	{
		FLOAT val	= (GetFltValue(pOperand) == FLT_MAX) ? FLT_MAX : -GetFltValue(pOperand);

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLOAT_AS_LONG(val));
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcAdd(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1)
{
	if (IsIntValue(pOperand0) && IsIntValue(pOperand1))
	{
		LONG val0	= GetIntValue(pOperand0);
		LONG val1	= GetIntValue(pOperand1);
		LONG val	= val0 + val1;

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_INT, val);
	}
	else
	{
		FLOAT val0	= GetFltValue(pOperand0);
		FLOAT val1	= GetFltValue(pOperand1);
		FLOAT val	= ((val0 == FLT_MAX) || (val1 == FLT_MAX)) ? FLT_MAX : val0 + val1;

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLOAT_AS_LONG(val));
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcSub(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1)
{
	if (IsIntValue(pOperand0) && IsIntValue(pOperand1))
	{
		LONG val0	= GetIntValue(pOperand0);
		LONG val1	= GetIntValue(pOperand1);
		LONG val	= val0 - val1;

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_INT, val);
	}
	else
	{
		FLOAT val0	= GetFltValue(pOperand0);
		FLOAT val1	= GetFltValue(pOperand1);
		FLOAT val	= ((val0 == FLT_MAX) || (val1 == FLT_MAX)) ? FLT_MAX : val0 - val1;

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLOAT_AS_LONG(val));
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcMul(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1)
{
	if (IsIntValue(pOperand0) && IsIntValue(pOperand1))
	{
		LONG val0	= GetIntValue(pOperand0);
		LONG val1	= GetIntValue(pOperand1);
		LONG val	= val0 * val1;

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_INT, val);
	}
	else
	{
		FLOAT val0	= GetFltValue(pOperand0);
		FLOAT val1	= GetFltValue(pOperand1);
		FLOAT val	= ((val0 == FLT_MAX) || (val1 == FLT_MAX)) ? FLT_MAX : val0 * val1;

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLOAT_AS_LONG(val));
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcDiv(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1)
{
	if (IsIntValue(pOperand0) && IsIntValue(pOperand1))
	{
		LONG val0	= GetIntValue(pOperand0);
		LONG val1	= GetIntValue(pOperand1);

		if (val1 == 0)
			PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLT_MAX_AS_LONG);
		else
		{
			LONG val	= val0 / val1;

			PushVal(FORMULATRANSLATOR_VALUE_TYPE_INT, val);
		}
	}
	else
	{
		FLOAT val0	= GetFltValue(pOperand0);
		FLOAT val1	= GetFltValue(pOperand1);
		FLOAT val	= ((val0 == FLT_MAX) || (val1 == FLT_MAX) || (val1 == 0)) ? FLT_MAX : val0 / val1;

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLOAT_AS_LONG(val));
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcRem(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1)
{
	if (IsIntValue(pOperand0) && IsIntValue(pOperand1))
	{
		LONG val0	= GetIntValue(pOperand0);
		LONG val1	= GetIntValue(pOperand1);

		if (val1 == 0)
			PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLT_MAX_AS_LONG);
		else
		{
			LONG val	= val0 % val1;

			PushVal(FORMULATRANSLATOR_VALUE_TYPE_INT, val);
		}
	}
	else
	{
		FLOAT val0	= GetFltValue(pOperand0);
		FLOAT val1	= GetFltValue(pOperand1);
		FLOAT val	= ((val0 == FLT_MAX) || (val1 == FLT_MAX) || (val1 == 0)) ? FLT_MAX : (LONG)val0 % (LONG)val1;

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLOAT_AS_LONG(val));
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcPow(LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1)
{
	if (IsIntValue(pOperand0) && IsIntValue(pOperand1))
	{
		LONG val0	= GetIntValue(pOperand0);
		LONG val1	= GetIntValue(pOperand1);
		LONG val	= (LONG)pow((float)val0, (float)val1);

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_INT, val);
	}
	else
	{
		FLOAT val0	= GetFltValue(pOperand0);
		FLOAT val1	= GetFltValue(pOperand1);
		FLOAT val	= ((val0 == FLT_MAX) || (val1 == FLT_MAX)) ? FLT_MAX : (FLOAT)pow(val0, val1);

		PushVal(FORMULATRANSLATOR_VALUE_TYPE_FLT, FLOAT_AS_LONG(val));
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::IsIntValue(LPFORMULATRANSLATOR_VALUE_DESC pDesc)
{
	if (pDesc)
		return pDesc->dwType == FORMULATRANSLATOR_VALUE_TYPE_INT;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::IsFltValue(LPFORMULATRANSLATOR_VALUE_DESC pDesc)
{
	if (pDesc)
		return pDesc->dwType == FORMULATRANSLATOR_VALUE_TYPE_FLT;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
LONG CFormulaTranslator::GetIntValue(LPFORMULATRANSLATOR_VALUE_DESC pDesc)
{
	if (IsIntValue(pDesc))
		return pDesc->dwData;

	if (IsFltValue(pDesc))
		return (LONG)(*(FLOAT*)&pDesc->dwData);

	return 0;
}
//////////////////////////////////////////////////////////////////////
FLOAT CFormulaTranslator::GetFltValue(LPFORMULATRANSLATOR_VALUE_DESC pDesc)
{
	if (IsIntValue(pDesc))
		return (FLOAT)pDesc->dwData;

	if (IsFltValue(pDesc))
		return *(FLOAT*)&pDesc->dwData;

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::IsIntValue()
{
	LPFORMULATRANSLATOR_VALUE_DESC pDesc = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Peek();

	if (pDesc)
		return IsIntValue(pDesc);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::IsFltValue()
{
	LPFORMULATRANSLATOR_VALUE_DESC pDesc = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Peek();

	if (pDesc)
		return IsFltValue(pDesc);

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
LONG CFormulaTranslator::GetIntValue()
{
	LPFORMULATRANSLATOR_VALUE_DESC pDesc = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Peek();

	if (pDesc)
		return GetIntValue(pDesc);

	return 0;
}
//////////////////////////////////////////////////////////////////////
FLOAT CFormulaTranslator::GetFltValue()
{
	LPFORMULATRANSLATOR_VALUE_DESC pDesc = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Peek();

	if (pDesc)
		return GetFltValue(pDesc);

	return 0;
}
//////////////////////////////////////////////////////////////////////
void CFormulaTranslator::DestroyOperandsStack()
{
	while (!m_stack.IsEmpty())
	{
		LPFORMULATRANSLATOR_VALUE_DESC pDesc = (LPFORMULATRANSLATOR_VALUE_DESC)m_stack.Pop();

		if (pDesc)
			delete pDesc;
	}

	m_stack.RemoveAll();
}
//////////////////////////////////////////////////////////////////////
void CFormulaTranslator::SetVariableProvider(CFormulaTranslatorVariableProvider *pVariableProvider)
{
	m_pVariableProvider = pVariableProvider;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcFn(LPCSTR lpName, LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1)
{
	LPFORMULATRANSLATOR_VALUE_DESC pDesc = new FORMULATRANSLATOR_VALUE_DESC;

	if (!CalcFn(lpName, pOperand0, pOperand1, pDesc))
	{
		delete pDesc;
		return FALSE;
	}

	m_stack.Push(pDesc);

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
int CFormulaTranslator::GetFnOperandCount(LPCSTR lpName)
{
	//2 operand functions

	if (!(_stricmp(lpName, "min")))
		return 2;
	if (!(_stricmp(lpName, "max")))
		return 2;

	//1 operand functions

	if (!(_stricmp(lpName, "int")))
		return 1;
	if (!(_stricmp(lpName, "flt")))
		return 1;
	if (!(_stricmp(lpName, "floor")))
		return 1;
	if (!(_stricmp(lpName, "ceil")))
		return 1;
	if (!(_stricmp(lpName, "round")))
		return 1;

	return -1;
}
//////////////////////////////////////////////////////////////////////
BOOL CFormulaTranslator::CalcFn(LPCSTR lpName, LPFORMULATRANSLATOR_VALUE_DESC pOperand0, LPFORMULATRANSLATOR_VALUE_DESC pOperand1, LPFORMULATRANSLATOR_VALUE_DESC pDesc)
{
	//2 operand functions

	if (!(_stricmp(lpName, "min")))
	{
		if (IsIntValue(pOperand0) && IsIntValue(pOperand1))
		{
			LONG data = min(pOperand0->dwData, pOperand1->dwData);

			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_INT;
			pDesc->dwData = data;
		}
		else
		{
			FLOAT data = min(GetFltValue(pOperand0), GetFltValue(pOperand1));

			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
			pDesc->dwData = FLOAT_AS_LONG(data);
		}

		return TRUE;
	}

	if (!(_stricmp(lpName, "max")))
	{
		if (IsIntValue(pOperand0) && IsIntValue(pOperand1))
		{
			LONG data = max(pOperand0->dwData, pOperand1->dwData);

			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_INT;
			pDesc->dwData = data;
		}
		else
		{
			FLOAT data = max(GetFltValue(pOperand0), GetFltValue(pOperand1));

			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
			pDesc->dwData = FLOAT_AS_LONG(data);
		}

		return TRUE;
	}

	//1 operand functions

	if (!(_stricmp(lpName, "int")))
	{
		LONG data = GetIntValue(pOperand0);

		pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_INT;
		pDesc->dwData = data;

		return TRUE;
	}

	if (!(_stricmp(lpName, "flt")))
	{
		FLOAT data = GetFltValue(pOperand0);

		pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_FLT;
		pDesc->dwData = FLOAT_AS_LONG(data);

		return TRUE;
	}

	if (!(_stricmp(lpName, "ceil")))
	{
		if (IsFltValue(pOperand0))
		{
			FLOAT data = GetFltValue(pOperand0);

			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_INT;
			pDesc->dwData = (LONG)ceil(data);
		}
		else
		{
			pDesc->dwType = pOperand0->dwType;
			pDesc->dwData = pOperand0->dwData;
		}

		return TRUE;
	}

	if (!(_stricmp(lpName, "floor")))
	{
		if (IsFltValue(pOperand0))
		{
			FLOAT data = GetFltValue(pOperand0);

			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_INT;
			pDesc->dwData = (LONG)floor(data);
		}
		else
		{
			pDesc->dwType = pOperand0->dwType;
			pDesc->dwData = pOperand0->dwData;
		}

		return TRUE;
	}

	if (!(_stricmp(lpName, "round")))
	{
		if (IsFltValue(pOperand0))
		{
			FLOAT data = GetFltValue(pOperand0);

			pDesc->dwType = FORMULATRANSLATOR_VALUE_TYPE_INT;
			pDesc->dwData = (data < 0.0f) ? (LONG)(data - 0.5f) : (LONG)(data + 0.5f);
		}
		else
		{
			pDesc->dwType = pOperand0->dwType;
			pDesc->dwData = pOperand0->dwData;
		}

		return TRUE;
	}

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
