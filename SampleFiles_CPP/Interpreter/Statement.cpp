//#include "stdafx.h"
#include "pch.h"
#include "Statement.h"
#include "SymbolTable.h"
#include "GrammarNodeVisitor.h"

CStatement::CStatement()
{

}

CStatement::~CStatement()
{

}

_INT32 CStatement::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return 0;
}

_INT32 CStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors
			)
{
	return 0;
}

void CStatement::Identify(
			_CHAR* szData)
{
}

_INT32 CStatement::GetLineNumber()
{
	return -1;
}
