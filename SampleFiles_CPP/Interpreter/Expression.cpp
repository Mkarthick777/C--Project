//#include "stdafx.h"
#include "pch.h"
#include "ExpParser.h"
#include "Expression.h"
#include "GrammarNodeVisitor.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CExpression::CExpression()
{
	SetNodeType(NODE_TYPE_EXPRESSION);
	m_pExpression = NULL;	//TSRPRASAD 09MAR2004 Fix the memory leaks
}

CExpression::~CExpression()
{
	DELETE_PTR(m_pExpression);	//TSRPRASAD 09MAR2004 Fix the memory leaks
}

_INT32 CExpression::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{

	return pVisitor->visitExpression(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}

_INT32 CExpression::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CExpParser expParser;
	CToken* pToken=0;
	try
	{
		m_pExpression = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_ASSIGN);
		if(!m_pExpression)
		{
			//ADD_ERROR(C_IF_ERROR_MISSINGEXP);
		}

//Munch a <;> 
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsEOS())
		{
			DELETE_PTR(pToken);
			throw(C_RS_ERROR_MISSINGSC);
		}
		
		DELETE_PTR(pToken);

		return PARSE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
		plexAnal->SynchronizeTo(EXPRESSION,pSymbolTable);
	}
	return PARSE_SUCCESS;
}

void CExpression::Identify(
			_CHAR* szData)
{

}

_INT32 CExpression::GetLineNumber()
{
	return -1;
}
