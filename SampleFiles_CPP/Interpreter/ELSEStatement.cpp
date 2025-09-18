//#include "stdafx.h"
#include "pch.h"
#include "ELSEStatement.h"
#include "ParserBuilder.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CELSEStatement::CELSEStatement()
{
	m_pStatement = 0;
}

CELSEStatement::~CELSEStatement()
{
	DELETE_PTR(m_pStatement);
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CELSEStatement::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitELSEStatement(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif
// Selection Statement is of the form
//	<ELSE> <Statement>;
_INT32 CELSEStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
//Eat a Statement...
	CToken* pToken=0;
	try
	{
//Munch a <ELSE>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsELSEStatement())
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);

//Look for a statement
		CParserBuilder builder;
		if(0!=(m_pStatement = (CStatement*)builder.CreateParser(plexAnal,STMT_asic)))
		{
			_INT32 i32Ret = m_pStatement->CreateParseSubTree(
				plexAnal,
				pSymbolTable,
				pvecErrors);
			if(i32Ret == PARSE_FAIL)
			{
				//ADD_ERROR(C_ES_ERROR_MISSINGSTMT);
			}
			else
			{
				return PARSE_SUCCESS;
			}
		}
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

void CELSEStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,"ELSEStatement");
	strcat(szData,">");

	if(m_pStatement)
	{
		m_pStatement->Identify(szData);
	}
	strcat(szData,"</");
	strcat(szData,"ELSEStatement");
	strcat(szData,">");*/
	std::string result(szData); // Initialize result with szData

	result += "<ELSEStatement>";
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string

	if (m_pStatement)
		m_pStatement->Identify(szData);

	result += "</ELSEStatement>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif
}

CStatement* CELSEStatement::GetStatement()
{
	return m_pStatement;
}

_INT32 CELSEStatement::GetLineNumber()
{
	return m_pStatement->GetLineNumber();
}
