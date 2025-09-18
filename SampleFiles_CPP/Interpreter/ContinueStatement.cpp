//#include "stdafx.h"
#pragma warning (disable : 4786) 
#include "pch.h"
#include "ContinueStatement.h"
#include "SymbolTable.h"
#include "ExpParser.h"
#include "GrammarNodeVisitor.h"
#include "OMServiceExpression.h"
#include "FunctionExpression.h"

#include "ErrorDefinitions.h"
//#include "RIDEError.h"
#include "SynchronisationSet.h"

CContinueStatement::CContinueStatement()
{
}

CContinueStatement::~CContinueStatement()
{
}

_INT32 CContinueStatement::Execute(
			CGrammarNodeVisitor*	pVisitor,
			CSymbolTable*			pSymbolTable,
			INTER_VARIANT*		pvar,
			ERROR_VEC*				pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return VISIT_CONTINUE;
}

// Assignment CONTINUE Statement is of the form
//	<continue><;>
_INT32 CContinueStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors
			)
{
	CToken* pToken=0;
	try
	{
//Munch a <CONTINUE>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken->IsCONTINUEStatement())
		{
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

// Assignment CONTINUE Statement is of the form
//	<continue><;>
_INT32 CContinueStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors,
			STATEMENT_TYPE		stmt_type
			)
{
	return PARSE_SUCCESS;
}

void CContinueStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,"CONTINUE");
	strcat(szData,">");
	strcat(szData,"</");
	strcat(szData,"CONTINUE");
	strcat(szData,">");*/
	std::string result;

	result += "<CONTINUE>";
	result += "</CONTINUE>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif
}

_INT32 CContinueStatement::GetLineNumber()
{
	return i32LineNumber;
}
