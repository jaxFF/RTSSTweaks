// RTFCErrors.h: error codes for RTFC library
// 
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _RTFCERRORS_H_INCLUDED_
#define _RTFCERRORS_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#define RTFC_E(code) MAKE_HRESULT(1, FACILITY_ITF, code)
#define RTFC_S(code) MAKE_HRESULT(0, FACILITY_ITF, code)

// CLexicalAnalyzer error codes

#define LEXICALANALYZER_E_CHAR_ALREADY_DEFINED							RTFC_E(0x0200)
#define LEXICALANALYZER_E_NULL											RTFC_E(0x0201)	
#define LEXICALANALYZER_E_EMPTY_CHARSET									RTFC_E(0x0202)
#define LEXICALANALYZER_E_INVALID_STATE									RTFC_E(0x0203)
#define LEXICALANALYZER_E_INVALID_CHAR									RTFC_E(0x0204)
#define LEXICALANALYZER_E_JUMP_TABLE_NOT_INITIALIZED					RTFC_E(0x0205)
#define LEXICALANALYZER_E_INCOMPLETE_JUMP_TABLE							RTFC_E(0x0206)
#define LEXICALANALYZER_E_UNREACHABLE_STATES							RTFC_E(0x0207)
#define LEXICALANALYZER_E_UNHANDLED_LEXEME								RTFC_E(0x0208)

// CFormulaLexicalAnalyzer error codes

#define FORMULALEXICALANALYZER_E_NO_LEXEME								RTFC_E(0x0300)

// CFormulaTranslator error codes

#define FORMULATRANSLATOR_E_UNKNOWN_OPERATION							RTFC_E(0x0400)
#define FORMULATRANSLATOR_E_UNEXPECTED_BRACKET							RTFC_E(0x0401)
#define FORMULATRANSLATOR_E_BRACKET_EXPECTED							RTFC_E(0x0402)
#define FORMULATRANSLATOR_E_UNDECLARED_VARIABLE							RTFC_E(0x0403)
#define FORMULATRANSLATOR_E_OPERAND_EXPECTED							RTFC_E(0x0404)
#define FORMULATRANSLATOR_E_OPERATION_EXPECTED							RTFC_E(0x0405)
#define FORMULATRANSLATOR_E_QUOTE_EXPECTED								RTFC_E(0x0406)
#define FORMULATRANSLATOR_E_UNDECLARED_FN								RTFC_E(0x0407)
#define FORMULATRANSLATOR_E_UNEXPECTED_COMMA							RTFC_E(0x0408)

//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
