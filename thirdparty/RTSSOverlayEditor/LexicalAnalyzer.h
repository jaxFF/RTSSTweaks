// LexicalAnalyzer.h: interface for the CLexicalAnalyzer class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _LEXICALANALYZER_H_INCLUDED_
#define _LEXICALANALYZER_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
#define JTF_DONT_INC_CHAR_INDEX							0x00010000
#define JTF_BREAK										0x00020000

#define JTF_RESERVED									0x80000000
//////////////////////////////////////////////////////////////////////
#ifndef _STRINGLITE_H_INCLUDED_
#include "StringLite.h"	
#endif
#ifndef _PTRLISTLITE_H_INCLUDED_
#include "PtrListLite.h"
#endif
//////////////////////////////////////////////////////////////////////
typedef struct LEXEME_DESC
{
	DWORD	dwClass;
	DWORD	dwData;
	LPVOID	lpData;
	LONG	dwCounter;
	DWORD	dwReserved;
} LEXEME_DESC, *LPLEXEME_DESC;
//////////////////////////////////////////////////////////////////////
class CLexicalAnalyzer  
{
public:

	HRESULT Analyze(LPCSTR lpString);
		//ananlyzes specified string

	//pure virtual methods

	virtual HRESULT PreAnalyze() = 0;
		//pre analyze callback
	virtual HRESULT OnStateChange(WORD srcState, WORD dstState, char ch) = 0;
		//state change callback
	virtual HRESULT PostAnalyze(WORD endState, BOOL bSuccessful) = 0;
		//post analyze callback

	//charset manipulation functions

	WORD GetCharsetSize();
		//returns charset size
	void UninitCharset();
		//uninitializes charset
	HRESULT AppendCharset(char	ch);
		//adds specified character to charset
	HRESULT AppendCharset(char	chFrom, char chTo);
		//adds specified range of characters to charset
	HRESULT AppendCharset(LPCSTR lpCharset);
		//adds specified set of characters to charset

		//WARNING!!! All charset modification functions destroy current jump table

	//jump table manipulation functions

	HRESULT InitJumpTable(WORD wStates);
		//allocates and initializes jump table
	void UninitJumpTable();
		//uninitializes jump table

	HRESULT AppendJumpTable(WORD srcState, WORD dstState, DWORD dwFlags);
		//appends jump table with srcState->dstState jumps for all characters of the
		//charset
	HRESULT AppendJumpTable(WORD srcState, WORD dstState, DWORD dwFlags, char ch);
		//appends jump table with srcState->dstState jump for specified character
	HRESULT AppendJumpTable(WORD srcState, WORD dstState, DWORD dwFlags, char chFrom, char chTo);
		//appends jump table with srcState->dstState jump for specified range of chars
	HRESULT AppendJumpTable(WORD srcState, WORD dstState, DWORD dwFlags, LPCSTR lpCharset);
		//appends jump table with srcState->dstState jumps for specified characters
	HRESULT ValidateJumpTable();
		//checks states reachability and validates jump table

	//misc functions

	static CStringLite GetErrName(HRESULT errCode);
		//returns name for the specified error code

	//helper functions for lexeme string processing

	void ResetLexemeStr();
		//resets lexeme string
	void SetLexemeStr(char ch);
		//initializes lexeme string with specified character
	void AppendLexemeStr(char ch);
		//appends lexeme string with specified character

	LPCSTR GetLexemeStr();
		//returns lexeme string

	//helper functions for lexemes list processing

	HRESULT CreateLexemeDesc(DWORD dwClass);
		//creates lexeme descriptor of specified class, fails if unhandled lexeme descriptor already exists
		//Warning: descriptor remains unhandled and not added to the list until calling AddUnhandledLexeme
	void AllocLexemeDescData(DWORD dwData, LPBYTE lpData = NULL);
		//allocates / initializes data and attaches it to unandled lexeme descriptor
	LPLEXEME_DESC GetUnhandledLexeme();
		//returns ptr to the current unhandled lexeme, or NULL if there is no unhandled lexeme
	void AddUnhandledLexeme();
		//adds unhandled lexeme to the list
	void DestroyLexemes();
		//destroys lexemes list
	void DestroyLexemeDesc(LPLEXEME_DESC pLexeme);
		//destroys lexeme descriptor

	LPLEXEME_DESC GetHeadLexeme();
		//returns ptr to the first lexeme in the list or NULL if the list is empty
	LPLEXEME_DESC GetTailLexeme();
		//returns ptr to the last lexeme in the list or NULL if the list is empty

	LPLEXEME_DESC PeekLexeme(LONG dwDepth);
		//returns ptr to the adjecent lexeme in the list without modifying current lexeme pointer 
		//or NULL if GetHeadLexeme / GetTailLexeme were not called before using this method

	LPLEXEME_DESC GetNextLexeme();
		//returns ptr to the next lexeme in the list or NULL if GetHeadLexeme / GetTailLexeme were not called
		//before using this method or we've reached the end of the list with GetNextLexeme / GetPrevLexeme
	LPLEXEME_DESC GetPrevLexeme();
		//returns ptr to the previous lexeme in the list or NULL if GetHeadLexeme / GetTailLexeme were not called
		//before using this method or we've reached the end of the list with GetNextLexeme / GetPrevLexeme
	LPLEXEME_DESC GetThisLexeme();
		//returns ptr to the current lexeme in the list or NULL if GetHeadLexeme / GetTailLexeme were not called
		//before using this method or we've reached the end of the list with GetNextLexeme / GetPrevLexeme
	LPLEXEME_DESC DestroyThisLexeme();
		//destroys current lexeme, returns ptr to the next lexeme or NULL if the
		//last lexeme is destroyed or if GetHeadLexeme / GetTailLexeme were not called
		//before using this method

	CLexicalAnalyzer();
	virtual ~CLexicalAnalyzer();

protected:

	CPtrListLite	m_lexemesList;
		//list of lexemes in the current line

private:

	WORD	m_charset[256];
		//character mapping table (contains character index in the charset of 0xffff
		//if the character doesn't belong to charset)
	WORD	m_wCharsetSize;
		//size of charset (number of mapped entries in the charset mapping table)

	LPDWORD	m_pJumpTable;
		//ptr to jump table 
	WORD	m_wStates;
		//number of states in the jump table

	void MarkReachabilityHelper(LPBYTE pReachabilityArr, WORD state);	
		//helper for ValidateJumpTable


	LPLEXEME_DESC	m_pLexeme;
		//pointer to current lexeme descriptor (used during lexemes list creation)
	LPPTR_LIST_NODE	m_pNode;
		//pointer to current node in lexemes list
	CStringLite		m_strLexeme;
		//string containing current lexeme 
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
