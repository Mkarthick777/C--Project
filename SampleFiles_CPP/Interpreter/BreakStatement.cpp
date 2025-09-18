//#include "stdafx.h"
#include "pch.h"
#include "BreakStatement.h"
#include "SymbolTable.h"
#include "ExpParser.h"
#include "GrammarNodeVisitor.h"
#include "OMServiceExpression.h"
#include "FunctionExpression.h"

#include "ErrorDefinitions.h"
//#include "RIDEError.h"
#include "SynchronisationSet.h"

CBreakStatement::CBreakStatement()
{
}

CBreakStatement::~CBreakStatement()
{
}

_INT32 CBreakStatement::Execute(
			CGrammarNodeVisitor*	pVisitor,
			CSymbolTable*			pSymbolTable,
			INTER_VARIANT*		pvar,
			ERROR_VEC*				pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return VISIT_BREAK;
}

// Assignment BREAK Statement is of the form
//	<break><;>
_INT32 CBreakStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors
			)
{
	CToken* pToken=0;
	try
	{
//Munch a <break>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken->IsBREAKStatement())
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);
//Munch a <;>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| pToken->GetSubType() != RUL_SEMICOLON)
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);
		return PARSE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
		plexAnal->SynchronizeTo(EXPRESSION,pSymbolTable);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return PARSE_FAIL;
}

// Assignment BREAK Statement is of the form
//	<break><;>
_INT32 CBreakStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors,
			STATEMENT_TYPE		stmt_type
			)
{
	return PARSE_SUCCESS;
}

void CBreakStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,"BREAK");
	strcat(szData,">");
	strcat(szData,"</");
	strcat(szData,"BREAK");
	strcat(szData,">");*/

	std::string result(szData); // Convert szData to std::string

	result += "<BREAK>";
	result += "</BREAK>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif
}

_INT32 CBreakStatement::GetLineNumber()
{
	return i32LineNumber;
}
