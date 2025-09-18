
//#include "stdafx.h"
#include "pch.h"
#include "ParserDeclarations.h"
#include "SafeVar.h"

#ifdef _FULL_RULE_ENGINE

/*BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}*/

#endif

_CHAR*	szTokenStrings[] = 	{	
									(_CHAR *)"NONE",
									(_CHAR *)"KEYWORD",
									(_CHAR *)"SYMBOL",
									(_CHAR *)"ARITHMETIC_OPERATOR",
									(_CHAR *)"ASSIGNMENT_OPERATOR",
									(_CHAR *)"RELATIONAL_OPERATOR",
									(_CHAR *)"LOGICAL_OPERATOR",
									(_CHAR *)"NUMERIC_CONSTANT", 
									(_CHAR *)"STRING_CONSTANT",
									(_CHAR *)"SIMPLE_VARIABLE", 
									(_CHAR *)"ARRAY_VARIABLE", 
									(_CHAR *)"SERVICE",
									(_CHAR *)"TYPE_ERROR",
									(_CHAR *)"COMMENT",
									(_CHAR *)"EOF"

							};


_CHAR*	szTokenSubstrings[] = {
									(_CHAR *)"SUBTYPE_NONE",
									//INT Operators
									(_CHAR *)"UPLUS", 
									(_CHAR *)"UMINUS", 
									(_CHAR *)"PLUS", 
									(_CHAR *)"MINUS", 
									(_CHAR *)"MUL", 
									(_CHAR *)"DIV", 
									(_CHAR *)"MOD",
									(_CHAR *)"EXP",
									(_CHAR *)"NOT_EQ",  
									(_CHAR *)"LT",
									(_CHAR *)"GT",
									(_CHAR *)"EQ",  
									(_CHAR *)"GE", 
									(_CHAR *)"LE",
									(_CHAR *)"LOGIC_AND",
									(_CHAR *)"LOGIC_OR",
									(_CHAR *)"LOGIC_NOT",
									(_CHAR *)"ASSIGN",
									//FLOAT Operators
									(_CHAR *)"FUPLUS", 
									(_CHAR *)"FUMINUS", 
									(_CHAR *)"FPLUS", 
									(_CHAR *)"FMINUS", 
									(_CHAR *)"FMUL", 
									(_CHAR *)"FDIV",
									(_CHAR *)"FMOD",
#ifdef XMTR
									(_CHAR *)"FRAND",
#endif
									(_CHAR *)"FEXP",
									(_CHAR *)"I2F",
									(_CHAR *)"F2I",
									(_CHAR *)"NOT_FEQ",
									(_CHAR *)"FLT",
									(_CHAR *)"FGT",
									(_CHAR *)"FEQ",  
									(_CHAR *)"FGE", 
									(_CHAR *)"FLE",
									//String Operators
									(_CHAR *)"SPLUS",
									(_CHAR *)"SEQ",
									(_CHAR *)"NOT_SEQ",
									//Keywords
									(_CHAR *)"IF",
									(_CHAR *)"ELSE",
									(_CHAR *)"WHILE",
									(_CHAR *)"CHAR_DECL",
									(_CHAR *)"INTEGER_DECL",
									(_CHAR *)"REAL_DECL", 
									(_CHAR *)"BOOLEAN_DECL", 
									(_CHAR *)"STRING_DECL",	
									//Symbols
									(_CHAR *)"LPAREN",
									(_CHAR *)"RPAREN",
									(_CHAR *)"LBRACK",
									(_CHAR *)"RBRACK",
									(_CHAR *)"LBOX",
									(_CHAR *)"RBOX",
									(_CHAR *)"SEMICOLON",
									(_CHAR *)"COMMA",
									(_CHAR *)"DOT",
									(_CHAR *)"SCOPE",
									//Constants
									(_CHAR *)"CHAR_CONSTANT", 
									(_CHAR *)"INT_CONSTANT", 
									(_CHAR *)"REAL_CONSTANT",
									(_CHAR *)"BOOL_CONSTANT",
									(_CHAR *)"STRING_CONSTANT",
									//Service SubTypes
									(_CHAR *)"SERVICE_INVOKE",
									(_CHAR *)"SERVICE_ATTRIBUTE",
									//Rule Self Invoke
									(_CHAR *)"RULE_ENGINE",
									(_CHAR *)"INVOKE",
									//Object manager
									(_CHAR *)"OM",
									//General
									(_CHAR *)"DOLLAR",
									(_CHAR *)"SUBTYPE_ERROR"
								};

void	TokenType_to_VariantType(
			RUL_TOKEN_TYPE token,
			RUL_TOKEN_SUBTYPE subtoken,
			VARIANT_TYPE& vt)
{
	switch(subtoken)
	{
	case RUL_CHAR_CONSTANT:
	case RUL_CHAR_DECL:
		vt = RUL_CHAR;
		break;
	case RUL_LONG_LONG_DECL:
		vt = RUL_LONGLONG;
		break;
		// Walt EPM 08sep08 - added
	case RUL_UNSIGNED_SHORT_INTEGER_DECL:
		vt = RUL_USHORT;
		break;
	case RUL_SHORT_INTEGER_DECL:
		vt = RUL_SHORT;
		break;
	case RUL_UNSIGNED_INTEGER_DECL:
		vt = RUL_UINT;
		break;
		// end add  Walt EPM 08sep08
	case RUL_INT_CONSTANT:
	case RUL_INTEGER_DECL:
	case RUL_LONG_DECL:
		vt = RUL_INT;
		break;
	case RUL_REAL_CONSTANT:
	case RUL_REAL_DECL:
		vt = RUL_FLOAT;
		break;
	case RUL_DOUBLE_DECL:
		vt = RUL_DOUBLE;
		break;
	case RUL_UNSIGNED_CHAR_DECL:
		vt = RUL_UNSIGNED_CHAR;
		break;
	}
}