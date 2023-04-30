// LexicalAnalyzer.cpp: implementation of the CLexicalAnalyzer class.
// 
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "LexicalAnalyzer.h"
//////////////////////////////////////////////////////////////////////
#ifndef _RTFCERRORS_H_INCLUDED_
#include "RTFCErrors.h"
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLexicalAnalyzer::CLexicalAnalyzer()
{
	m_pJumpTable	= NULL;

	UninitCharset();

	m_pLexeme		= NULL; 
	m_pNode			= NULL;

	m_strLexeme		= "";
}
//////////////////////////////////////////////////////////////////////
CLexicalAnalyzer::~CLexicalAnalyzer()
{
	UninitJumpTable();
	DestroyLexemes();
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::Analyze(LPCSTR lpString)
{
	HRESULT errCode;

	if (FAILED(errCode = PreAnalyze()))
		return errCode;
		//call virtual PreAnalyze

	if (!lpString)
		return LEXICALANALYZER_E_NULL;
			//return error if invalid ptr was specified

	if (!GetCharsetSize())
		//return error if charset is empty
		return LEXICALANALYZER_E_EMPTY_CHARSET;

	if (!m_pJumpTable)
		//return error if jump table is not initialized
		return LEXICALANALYZER_E_JUMP_TABLE_NOT_INITIALIZED;

	WORD curState = 0;
		//current state index

	for (int iChar=0; iChar<(int)strlen(lpString);)
	{
		char ch = lpString[iChar];
			//get character from input string

		WORD index = m_charset[(BYTE)ch];
			//get character index

		if (index == 0xffff)
			return LEXICALANALYZER_E_INVALID_CHAR;
				//return error if specified character doesn't belong to charset

		DWORD jumpTableEntry = m_pJumpTable[curState * m_wCharsetSize + index];
			//get jump table entry

		if (jumpTableEntry & JTF_RESERVED)
			return LEXICALANALYZER_E_INVALID_STATE;

		if (jumpTableEntry & JTF_BREAK)
			return S_OK;

		HRESULT errCode = OnStateChange(curState, (WORD)(jumpTableEntry & 0xffff), ch);
			//call virtual OnStateChange

		if (FAILED(errCode))
		{
			PostAnalyze(curState, FALSE);

			return errCode;
		}

		curState = (WORD)(jumpTableEntry & 0xffff);
			//change current state

		if (!(jumpTableEntry & JTF_DONT_INC_CHAR_INDEX))
		 iChar++;
	}

	if (FAILED(errCode = PostAnalyze(curState, TRUE)))
		return errCode;
		//call virtual PostAnalyze

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::UninitCharset()
{
	FillMemory(m_charset, 256 * sizeof(WORD), 0xff);
		//reset charset mapping table
	m_wCharsetSize = 0;			
		//force charset mapping table reindexation

	UninitJumpTable();
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::AppendCharset(char ch)
{
	return AppendCharset(ch, ch);
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::AppendCharset(char chFrom, char chTo)
{
	for (WORD ch=chFrom; ch<=(chTo & 0xff); ch++)
		if (m_charset[ch] == 0xffff)
		{
			m_charset[ch]	= 0;
				//map specified character
			m_wCharsetSize	= 0;			
				//force charset mapping table reindexation

			UninitJumpTable();
		}

	return m_wCharsetSize ? S_OK : LEXICALANALYZER_E_CHAR_ALREADY_DEFINED;
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::AppendCharset(LPCSTR lpCharset)
{
	if (!lpCharset)
		return LEXICALANALYZER_E_NULL;

	if (lpCharset)
		for (int iChar=0; iChar<(int)strlen(lpCharset); iChar++)
			if (m_charset[lpCharset[iChar]] == 0xffff)
			{
				m_charset[lpCharset[iChar]]	= 0;
					//map current character
				m_wCharsetSize				= 0;			
					//force charset mapping table reindexation

				UninitJumpTable();
			}

	return m_wCharsetSize ? S_OK : LEXICALANALYZER_E_CHAR_ALREADY_DEFINED;
}
//////////////////////////////////////////////////////////////////////
WORD CLexicalAnalyzer::GetCharsetSize()
{
	if (!m_wCharsetSize)
	{
		for (int iChar=0; iChar<256; iChar++)
			if (m_charset[iChar] != 0xffff)
				m_charset[iChar] = m_wCharsetSize++;
	}

	return m_wCharsetSize;
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::UninitJumpTable()
{
	if (m_pJumpTable)
	{
		delete [] m_pJumpTable;

		m_pJumpTable = NULL;
	}

	m_wStates = 0;
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::InitJumpTable(WORD wStates)
{
	UninitJumpTable();
		//uninit the previous jump table

	if (!wStates)
		//return error if specified number of states is invalid
		return LEXICALANALYZER_E_NULL;
		
	WORD wCharsetSize = GetCharsetSize();
		//reindex charset if necessary and get charset size

	if (!wCharsetSize)
		//return error is charset is empty
		return LEXICALANALYZER_E_EMPTY_CHARSET;

	DWORD dwEntries = wStates * wCharsetSize;	
		//get total number of entries in jump table we're about to allocate

	m_pJumpTable = new DWORD[dwEntries];
		//allocate memory for jump table
	FillMemory(m_pJumpTable, dwEntries * sizeof(DWORD), 0xff);
		//reset jump table entries

	m_wStates = wStates;
		//save numper of states

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::AppendJumpTable(WORD srcState, WORD dstState, DWORD dwFlags)
{
	WORD wCharsetSize = GetCharsetSize();
		//reindex charset if necessary and get charset size

	if (!wCharsetSize)
		//return error if charset is empty
		return LEXICALANALYZER_E_EMPTY_CHARSET;
			
	if ((srcState >= m_wStates) || 
		(dstState >= m_wStates))
		//return error if either source or destination state index is invalid
		return LEXICALANALYZER_E_INVALID_STATE;

	if (!m_pJumpTable)
		//return error if jump table is not initialized
		return LEXICALANALYZER_E_JUMP_TABLE_NOT_INITIALIZED;

	for (int index=0; index<wCharsetSize; index++)
		m_pJumpTable[srcState * m_wCharsetSize + index] = dstState | dwFlags;
			//set jump table entry

	return S_OK;	
}
//////////////////////////////////////////////////////////////////////										 
HRESULT CLexicalAnalyzer::AppendJumpTable(WORD srcState, WORD dstState, DWORD dwFlags, char ch)
{
	return AppendJumpTable(srcState, dstState, dwFlags, ch, ch);
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::AppendJumpTable(WORD srcState, WORD dstState, DWORD dwFlags, char chFrom, char chTo)
{
	if ((srcState >= m_wStates) || 
		(dstState >= m_wStates))
		//return error if either source or destination state index is invalid
		return LEXICALANALYZER_E_INVALID_STATE;

	if (!GetCharsetSize())
		//return error if charset is empty
		return LEXICALANALYZER_E_EMPTY_CHARSET;

	if (!m_pJumpTable)
		//return error if jump table is not initialized
		return LEXICALANALYZER_E_JUMP_TABLE_NOT_INITIALIZED;

	for (WORD ch=chFrom; ch<=(chTo & 0xff); ch++)
	{
		WORD index = m_charset[ch];
			//get character index

		if (index == 0xffff)
			return LEXICALANALYZER_E_INVALID_CHAR;
				//return error if specified character doesn't belong to charset

		m_pJumpTable[srcState * m_wCharsetSize + index] = dstState | dwFlags;
			//set jump table entry
	}

	return S_OK;	
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::AppendJumpTable(WORD srcState, WORD dstState, DWORD dwFlags, LPCSTR lpCharset)
{
	if ((srcState >= m_wStates) || 
		(dstState >= m_wStates))
		//return error if either source or destination state index is invalid
		return LEXICALANALYZER_E_INVALID_STATE;

	if (!lpCharset)
		return LEXICALANALYZER_E_NULL;

	if (!GetCharsetSize())
		//return error if charset is empty
		return LEXICALANALYZER_E_EMPTY_CHARSET;

	if (!m_pJumpTable)
		//return error if jump table is not initialized
		return LEXICALANALYZER_E_JUMP_TABLE_NOT_INITIALIZED;

	for (int iChar=0; iChar<(int)strlen(lpCharset); iChar++)
	{
		WORD index = m_charset[lpCharset[iChar]];
			//get character index

		if (index == 0xffff)
			return LEXICALANALYZER_E_INVALID_CHAR;
				//return error if specified character doesn't belong to charset

		m_pJumpTable[srcState * m_wCharsetSize + index] = dstState | dwFlags;
			//set jump table entry
	}

	return S_OK;	
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::ValidateJumpTable()
{
	if (!GetCharsetSize())
		//return error if charset is empty
		return LEXICALANALYZER_E_EMPTY_CHARSET;

	if (!m_pJumpTable)
		//return error if jump table is not initialized
		return LEXICALANALYZER_E_JUMP_TABLE_NOT_INITIALIZED;

	DWORD dwEntries = m_wStates * m_wCharsetSize;	
		//get total number of entries in jump table

	for (DWORD dwEntry=0; dwEntry<dwEntries; dwEntry++)
		if (m_pJumpTable[dwEntry] == 0xffff)
			return LEXICALANALYZER_E_INCOMPLETE_JUMP_TABLE;

	LPBYTE pReachabilityArr = new BYTE[m_wStates];
		//allocate reachability array
	ZeroMemory(pReachabilityArr, m_wStates);
		//mark all states as unreachable

	MarkReachabilityHelper(pReachabilityArr, 0);
		//mark all reachable states

	for (WORD index=0; index<m_wStates; index++)
		if (!pReachabilityArr[index])
		{
			delete [] pReachabilityArr;
				//deallocate reachability array

			return LEXICALANALYZER_E_UNREACHABLE_STATES;
		}

	delete [] pReachabilityArr;
		//deallocate reachability array

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::MarkReachabilityHelper(LPBYTE pReachabilityArr, WORD state)
{
	if (!pReachabilityArr)
		return;
			//return if invalid reachability array was specified

	if (state >= m_wStates)
		return;
			//return if invalid state index was specified

	if (pReachabilityArr[state])
		return;
			//return if state is already marked as reachable

	pReachabilityArr[state] = 1;
		//mark state as reachable

	for (WORD index=0; index<m_wCharsetSize; index++)
		//mark all states that can be reached from this state as reachable too
	{
		WORD newState = (WORD)m_pJumpTable[m_wCharsetSize * state + index];

		if (newState != 0xffff)
			MarkReachabilityHelper(pReachabilityArr, newState);
	}
}
//////////////////////////////////////////////////////////////////////
CStringLite CLexicalAnalyzer::GetErrName(HRESULT errCode)
{
	switch (errCode)
	{
	case LEXICALANALYZER_E_CHAR_ALREADY_DEFINED:
		return "LEXICALANALYZER_E_CHAR_ALREADY_DEFINED";
	case LEXICALANALYZER_E_NULL:
		return "LEXICALANALYZER_E_NULL";
	case LEXICALANALYZER_E_EMPTY_CHARSET:
		return "LEXICALANALYZER_E_EMPTY_CHARSET";
	case LEXICALANALYZER_E_INVALID_STATE:
		return "LEXICALANALYZER_E_INVALID_STATE";
	case LEXICALANALYZER_E_INVALID_CHAR:
		return "LEXICALANALYZER_E_INVALID_CHAR";
	case LEXICALANALYZER_E_JUMP_TABLE_NOT_INITIALIZED:
		return "LEXICALANALYZER_E_JUMP_TABLE_NOT_INITIALIZED";
	case LEXICALANALYZER_E_INCOMPLETE_JUMP_TABLE:
		return "LEXICALANALYZER_E_INCOMPLETE_JUMP_TABLE";
	case LEXICALANALYZER_E_UNREACHABLE_STATES:
		return "LEXICALANALYZER_E_UNREACHABLE_STATES";
	}

	return "";
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::DestroyLexemes()
{
	LPPTR_LIST_NODE pNode = m_lexemesList.GetHead();
		//get ptr to first node in the list

	while (pNode)
	{
		LPLEXEME_DESC pDesc = (LPLEXEME_DESC)pNode->pData;
			//get ptr to lexeme associated with this node

		DestroyLexemeDesc(pDesc);
			//destroy lexeme descriptor

		pNode = pNode->pNext;
	}

	m_lexemesList.RemoveAll();
		//remove all nodes in the list

	if (m_pLexeme)
		//destroy unhandled lexeme if any
	{
		if (m_pLexeme->lpData)
			delete [] m_pLexeme->lpData;

		delete m_pLexeme;

		m_pLexeme = NULL;
	}

	m_pNode = NULL;
		//reset current lexeme ptr
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::DestroyLexemeDesc(LPLEXEME_DESC pDesc)
{
	if (pDesc)
		//if lexeme descriptor is valid we must deallocate it
	{
		if (pDesc->lpData)
			delete [] pDesc->lpData;
				//if descriptor contains valid ptr to lexeme specific data
				//we must deallocte it too

		delete pDesc;
	}
}
//////////////////////////////////////////////////////////////////////
HRESULT CLexicalAnalyzer::CreateLexemeDesc(DWORD dwClass)
{
	if (m_pLexeme)
		return LEXICALANALYZER_E_UNHANDLED_LEXEME;
			//return error if we're trying to allocate new lexeme without handling
			//the previous one

	m_pLexeme = new LEXEME_DESC;
		//allocate memory for new lexeme descriptor

	m_pLexeme->dwClass		= dwClass;
	m_pLexeme->dwData		= 0;
	m_pLexeme->lpData		= NULL;
	m_pLexeme->dwCounter	= 0;
	m_pLexeme->dwReserved	= 0;
		//init lexeme descriptor fields

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::AllocLexemeDescData(DWORD dwData, LPBYTE lpData)
{
	if (m_pLexeme)
	{
		if (lpData)
			//if lpData is specified then dwData is a size of buffer pointed by lpData
		{
			m_pLexeme->lpData = new BYTE[dwData];
				//allocate required buffer
			memcpy(m_pLexeme->lpData, lpData, dwData);
				//copy data from specifeid buffer to allocated buffer
		}

		m_pLexeme->dwData = dwData;
			//save dwData
	}
}
//////////////////////////////////////////////////////////////////////
LPLEXEME_DESC CLexicalAnalyzer::GetHeadLexeme()
{
	m_pNode = m_lexemesList.GetHead();

	if (m_pNode)
		return (LPLEXEME_DESC)m_pNode->pData;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPLEXEME_DESC CLexicalAnalyzer::GetTailLexeme()
{
	m_pNode = m_lexemesList.GetTail();

	if (m_pNode)
		return (LPLEXEME_DESC)m_pNode->pData;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPLEXEME_DESC CLexicalAnalyzer::GetNextLexeme()
{
	if (m_pNode)
		m_pNode = m_pNode->pNext;

	if (m_pNode)
		return (LPLEXEME_DESC)m_pNode->pData;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPLEXEME_DESC CLexicalAnalyzer::GetPrevLexeme()
{
	if (m_pNode)
		m_pNode = m_pNode->pPrev;

	if (m_pNode)
		return (LPLEXEME_DESC)m_pNode->pData;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPLEXEME_DESC CLexicalAnalyzer::PeekLexeme(LONG dwDepth)
{
	LPPTR_LIST_NODE pNode = m_pNode;

	while (pNode && dwDepth)
	{
		if (dwDepth > 0)
		{
			pNode = pNode->pNext;

			dwDepth--;
		}
		else
		{
			pNode = pNode->pPrev;

			dwDepth++;
		}
	}

	if (pNode)
		return (LPLEXEME_DESC)pNode->pData;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPLEXEME_DESC CLexicalAnalyzer::GetThisLexeme()
{
	if (m_pNode)
		return (LPLEXEME_DESC)m_pNode->pData;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::AddUnhandledLexeme()
{
	if (m_pLexeme)
	{
		m_lexemesList.AddTail(m_pLexeme);
			//add current lexeme to lexemes list
		m_pLexeme = NULL;
			//reset current lexeme ptr
	}
}
//////////////////////////////////////////////////////////////////////
LPLEXEME_DESC CLexicalAnalyzer::GetUnhandledLexeme()
{
	return m_pLexeme;
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::ResetLexemeStr()
{
	m_strLexeme = "";
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::AppendLexemeStr(char c)
{
	m_strLexeme += c;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CLexicalAnalyzer::GetLexemeStr()
{
	return m_strLexeme;
}
//////////////////////////////////////////////////////////////////////
void CLexicalAnalyzer::SetLexemeStr(char ch)
{
	m_strLexeme = ch;
}
//////////////////////////////////////////////////////////////////////
LPLEXEME_DESC CLexicalAnalyzer::DestroyThisLexeme()
{
	LPPTR_LIST_NODE pNode = m_pNode;

	if (m_pNode)
		m_pNode = m_pNode->pNext;

	if (pNode)
	{
		DestroyLexemeDesc((LPLEXEME_DESC)pNode->pData);

		m_lexemesList.RemoveAt(pNode);
	}

	if (m_pNode)
		return (LPLEXEME_DESC)m_pNode->pData;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
