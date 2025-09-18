//#include "stdafx.h"
#include "pch.h"
#include "ArrayExpression.h"
#include "GrammarNodeVisitor.h"
#include "SymbolTable.h"

CArrayExpression::CArrayExpression()
{
	m_pToken = 0;
}

CArrayExpression::CArrayExpression(CToken* pToken)
		:m_pToken(pToken)
{
}

CArrayExpression::~CArrayExpression()
{
	DELETE_PTR(m_pToken);

	_INT32 nSize = m_vecExpressions.size();
	for(_INT32 i=0;i<nSize;i++)
	{
		delete m_vecExpressions[i];
	}
	m_vecExpressions.clear();
}
#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CArrayExpression::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{

	return pVisitor->visitArrayExpression(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2

}
#endif
#endif
_INT32 CArrayExpression::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	return 0;
}

void CArrayExpression::Identify(
			_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,m_pToken->GetLexeme());
	strcat(szData,">");

	_INT32 nSize = m_vecExpressions.size();
	for(_INT32 i=0;i<nSize;i++)
	{
		m_vecExpressions[i]->Identify(szData);
	}

	strcat(szData,"</");
	strcat(szData,m_pToken->GetLexeme());
	strcat(szData,">");*/

	std::string result(szData); // Convert szData to std::string

	result += "<";
	result += m_pToken->GetLexeme();
	result += ">";

	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0';

	_INT32 nSize = m_vecExpressions.size();
	for (_INT32 i = 0; i < nSize; i++)
	{
		m_vecExpressions[i]->Identify(szData);
	}

	result += "</";
	result += m_pToken->GetLexeme();
	result += ">";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);

#endif
}

CToken* CArrayExpression::GetToken()
{
	return m_pToken;
}

EXPR_VECTOR* CArrayExpression::GetExpressions()
{
	return &m_vecExpressions;
}

void CArrayExpression::AddDimensionExpr(
			CExpression* pExpr)
{
	m_vecExpressions.push_back(pExpr);
}

_INT32 CArrayExpression::GetLineNumber()
{
	return m_vecExpressions[m_vecExpressions.size()-1]->GetLineNumber();
}
