//#include "stdafx.h"
#include "pch.h"
#include "CompoundExpression.h"
#include "GrammarNodeVisitor.h"

CCompoundExpression::CCompoundExpression()
{
	m_pFirstExp = 0;
	m_pSecondExp = 0;
	m_Operator = RUL_SUBTYPE_NONE;

}
CCompoundExpression::CCompoundExpression(
				CExpression* f,
				CExpression* s,
				RUL_TOKEN_SUBTYPE Op)
				:m_pFirstExp(f),m_pSecondExp(s),m_Operator(Op)
{
}

CCompoundExpression::~CCompoundExpression()
{
	DELETE_PTR(m_pFirstExp);
	DELETE_PTR(m_pSecondExp);
}

_INT32 CCompoundExpression::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitCompoundExpression(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}

_INT32 CCompoundExpression::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	return 0;
}

void CCompoundExpression::Identify(
		_CHAR* szData)
{

	try
	{
#ifdef MODIFIED_BY_SS
		/*_CHAR* sz1 = (_CHAR*)"BRACK";
		_CHAR* sz2;
		if(m_Operator == RUL_RPAREN)
			sz2 = sz1;
		else 
			sz2 = (char*)szTokenSubstrings[m_Operator];

		strcat(szData,"<");
		strcat(szData,sz2);	
		strcat(szData,">");
		if(m_pFirstExp)
		{
			m_pFirstExp->Identify(szData);
		}
		if(m_pSecondExp)
		{
			m_pSecondExp->Identify(szData);
		}
		strcat(szData,"</");
		strcat(szData,sz2);	
		strcat(szData,">");*/
#endif
		_CHAR* sz1 = (_CHAR*)"BRACK";
		_CHAR* sz2 = m_Operator == RUL_RPAREN ? sz1 : (char*)szTokenSubstrings[m_Operator];

		std::string data(szData);

		// Append opening tag
		data += "<";
		data += sz2;
		data += ">";

		// Recursively identify child expressions
		if (m_pFirstExp)
		{
			m_pFirstExp->Identify(szData);
		}
		if (m_pSecondExp)
		{
			m_pSecondExp->Identify(szData);
		}

		// Append closing tag
		data += "</";
		data += sz2;
		data += ">";

		// Copy the updated string back to szData
		size_t dataSize = data.size();
		memcpy(szData, data.c_str(), dataSize);
		szData[dataSize] = '\0'; // Ensure null-termination

	}
	catch(...)
	{
		return;
	}
}

CExpression* CCompoundExpression::GetFirstExpression()
{
	return m_pFirstExp;
}

CExpression* CCompoundExpression::GetSecondExpression()
{
	return m_pSecondExp;
}

RUL_TOKEN_SUBTYPE CCompoundExpression::GetOperator()
{
	return m_Operator;
}

_INT32 CCompoundExpression::GetLineNumber()
{
	return m_pSecondExp->GetLineNumber();
}