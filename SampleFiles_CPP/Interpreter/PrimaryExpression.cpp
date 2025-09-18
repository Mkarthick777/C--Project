//#include "stdafx.h"
#include "pch.h"
#include "PrimaryExpression.h"
#include "GrammarNodeVisitor.h"

CPrimaryExpression::CPrimaryExpression()
{
	m_pToken = 0;
}

CPrimaryExpression::CPrimaryExpression(
		CToken* pToken)
		:m_pToken(pToken)
{
}

CPrimaryExpression::~CPrimaryExpression()
{
	DELETE_PTR(m_pToken);
}

_INT32 CPrimaryExpression::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitPrimaryExpression(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}

_INT32 CPrimaryExpression::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	return 0;
}

void CPrimaryExpression::Identify(
			_CHAR* szData)
{
	/*strcat(szData,"<");
	if(!m_pToken->IsNumeric())
	{
		strcat(szData,m_pToken->GetLexeme());
	}
	else
	{
		strcat(szData,"NUM_");
		strcat(szData,m_pToken->GetLexeme());
	}
	strcat(szData,">");

	if(m_pToken->GetCompoundData())
	{
		strcat(szData,m_pToken->GetCompoundData()->m_szName);
		strcat(szData,",");
		strcat(szData,m_pToken->GetCompoundData()->m_szAttribute);
	} 

	strcat(szData,"</");
	if(!m_pToken->IsNumeric())
	{
		strcat(szData,m_pToken->GetLexeme());
	}
	else
	{
		strcat(szData,"NUM_");
		strcat(szData,m_pToken->GetLexeme());
	}
	strcat(szData,">");*/

	std::string result;

	result += "<";
	if (!m_pToken->IsNumeric())
	{
		result += m_pToken->GetLexeme();
	}
	else
	{
		result += "NUM_";
		result += m_pToken->GetLexeme();
	}
	result += ">";

	if (m_pToken->GetCompoundData())
	{
		result += m_pToken->GetCompoundData()->m_szName;
		result += ",";
		result += m_pToken->GetCompoundData()->m_szAttribute;
	}

	result += "</";
	if (!m_pToken->IsNumeric())
	{
		result += m_pToken->GetLexeme();
	}
	else
	{
		result += "NUM_";
		result += m_pToken->GetLexeme();
	}
	result += ">";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string


}

CToken* CPrimaryExpression::GetToken()
{
	return m_pToken;
}

_INT32 CPrimaryExpression::GetLineNumber()
{
	return m_pToken->GetLineNumber();
}
