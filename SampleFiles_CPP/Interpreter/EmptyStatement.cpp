
//#include "stdafx.h"
#include "pch.h"
#include "EmptyStatement.h"
#include "SymbolTable.h"
#include "ExpParser.h"
#include "GrammarNodeVisitor.h"
#include "OMServiceExpression.h"
#include "FunctionExpression.h"

#include "ErrorDefinitions.h"
//#include "RIDEError.h"
#include "SynchronisationSet.h"

CEmptyStatement::CEmptyStatement()
{
}

CEmptyStatement::~CEmptyStatement()
{
}

_INT32 CEmptyStatement::Execute(
			CGrammarNodeVisitor*	pVisitor,
			CSymbolTable*			pSymbolTable,
			INTER_VARIANT*			pvar,
			ERROR_VEC*				pvecErrors,
			RUL_TOKEN_SUBTYPE		AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	if (pvar != NULL)	// stevev 19nov09 - make 'for(;;)' statement work
		*pvar = (short int)1;		// stevev 19nov09 - make 'for(;;)' statement work
	return VISIT_NORMAL;
}

// Assignment Statement is of the form
//<;>
_INT32 CEmptyStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors
			)
{
	CToken* pToken=0;
	try
	{
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			 || (RUL_SEMICOLON != pToken->GetSubType()))
		{
			DELETE_PTR(pToken);
			return PARSE_FAIL;
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

void CEmptyStatement::Identify(
		_CHAR* szData)
{
}

_INT32 CEmptyStatement::GetLineNumber()
{
	_INT32 i32LineNumber = 0;
	return i32LineNumber;
}