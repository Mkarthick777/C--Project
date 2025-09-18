//#include "stdafx.h"
#include "pch.h"
#include "CompoundStatement.h"
#include "OptionalStatements.h"
#include "SymbolTable.h"
#include "StatementList.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CCompoundStatement::CCompoundStatement()
{
	m_pStmtList = 0;
}

CCompoundStatement::~CCompoundStatement()
{
	DELETE_PTR(m_pStmtList);
}
#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CCompoundStatement::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitCompoundStatement(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif
// Selection Statement is of the form
//	<{> <StatementList> <}>
_INT32 CCompoundStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken = 0;

	try
	{
//Munch a <{>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable))
			|| !pToken
			||(pToken->GetSubType() != RUL_LBRACK)
			)
		{
			if (pToken->GetSubType() != RUL_COLON)
			{
				DELETE_PTR(pToken);
				throw(C_UM_ERROR_INTERNALERR);
			}
		}
		DELETE_PTR(pToken);

//Munch List of statments...
		try
		{
			m_pStmtList= new CStatementList;
			m_pStmtList->CreateParseSubTree(
				plexAnal,
				pSymbolTable,
				pvecErrors);
		}
		catch(CRIDEError* perr)
		{
			pvecErrors->push_back(perr);
			plexAnal->MoveTo(
				RUL_SYMBOL,
				RUL_RBRACK,
				pSymbolTable);
		}

//Munch a <}>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable))
			|| !pToken
			|| (pToken->GetSubType() != RUL_RBRACK))
		{
			//ADD_ERROR(C_CS_ERROR_MISSINGRBRACK);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);
		return PARSE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
		plexAnal->MovePast(
			RUL_SYMBOL,
			RUL_SEMICOLON,
			pSymbolTable);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return PARSE_FAIL;
}

void CCompoundStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,"CompoundStatement");
	strcat(szData,">");

	if(m_pStmtList)
		m_pStmtList->Identify(szData);

	strcat(szData,"</");
	strcat(szData,"CompoundStatement");
	strcat(szData,">");*/

	std::string result(szData); // Initialize result with szData

	result += "<CompoundStatement>";
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string

	if (m_pStmtList)
		m_pStmtList->Identify(szData);

	result += "</CompoundStatement>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif 
}

CStatementList* CCompoundStatement::GetStatementList()
{
	return m_pStmtList;
}

_INT32 CCompoundStatement::GetLineNumber()
{
	return m_pStmtList->GetLineNumber();
}
