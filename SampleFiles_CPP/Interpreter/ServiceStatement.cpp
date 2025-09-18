//#include "stdafx.h"
#include "pch.h"
#include "ServiceStatement.h"
#include "Token.h"
#include "GrammarNodeVisitor.h"
#include "SymbolTable.h"

#include "ErrorDefinitions.h"

CServiceStatement::CServiceStatement()
{
	m_pToken=NULL;
}

CServiceStatement::~CServiceStatement()
{
	DELETE_PTR(m_pToken);
}


_INT32 CServiceStatement::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return 0;
}

// Service Statement is of the form
_INT32 CServiceStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken=0;
	CToken* pToken1=0;

//Munch a <Service::Server.Call()>
	if((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)) 
		&& pToken
		&& pToken->IsService())
	{
		DELETE_PTR(m_pToken);
		//Having got this far, doesn't it make sense to store the token...
		m_pToken = new CToken(*pToken);
		DELETE_PTR(pToken);
	}
	else
	{
		DELETE_PTR(pToken);
	}
	if((LEX_FAIL != plexAnal->GetNextToken(&pToken1,pSymbolTable)) 
		&& pToken1
		&& (RUL_SEMICOLON == pToken1->GetSubType()))
	{
		// let's see what to do here...
	}
	else
	{
		// error -- There should be a <;>
	}
	DELETE_PTR(pToken1);
	return 0;

}

void CServiceStatement::Identify(
		_CHAR* szData)
{
}

_INT32 CServiceStatement::GetLineNumber()
{
	return -1;
}
