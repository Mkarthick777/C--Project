//#include "stdafx.h"
#include "pch.h"
#include "OptionalStatements.h"
#include "SymbolTable.h"

COptionalStatements::COptionalStatements()
{

}

COptionalStatements::~COptionalStatements()
{

}

_INT32 COptionalStatements::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return 0;
}

_INT32 COptionalStatements::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	return 0;
}

void COptionalStatements::Identify(
			_CHAR* szData)
{
}

CStatementList* COptionalStatements::GetStatementList()
{
	return pStmtList;
}

_INT32 COptionalStatements::GetLineNumber()
{
	return -1;
}
