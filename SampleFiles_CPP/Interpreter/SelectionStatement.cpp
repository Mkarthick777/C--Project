//#include "stdafx.h"
#include "pch.h"
#include "SelectionStatement.h"
#include "SymbolTable.h"
#include "ParserBuilder.h"
#include "Statement.h"
#include "ELSEStatement.h"
#include "CASEStatement.h"
#include "ExpParser.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CSelectionStatement::CSelectionStatement()
{
	m_pExpression = 0;
	m_pStatement = 0;
	m_pElse = 0;
}

CSelectionStatement::~CSelectionStatement()
{
	DELETE_PTR(m_pExpression);
	DELETE_PTR(m_pStatement);
	DELETE_PTR(m_pElse);
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CSelectionStatement::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitSelectionStatement(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif
// Selection Statement is of the form
//	<IF><(><Expression><)><Statement> [<ELSEStatement>];
_INT32 CSelectionStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken=0;
	try
	{
//Munch a <IF>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsIFStatement())
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);

//Munch a <(>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (pToken->GetSubType() != RUL_LPAREN))
		{
			//ADD_ERROR(C_IF_ERROR_MISSINGLP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch & Parse the expression.
//we got to give the expression string to the expression parser.
//	
		CExpParser expParser;
		try
		{
			m_pExpression = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_IF);
			if(!m_pExpression)
			{
				//ADD_ERROR(C_IF_ERROR_MISSINGEXP);
			}
		}
		catch(CRIDEError* perr)
		{
			pvecErrors->push_back(perr);
			plexAnal->SynchronizeTo(EXPRESSION,pSymbolTable);
		}

//Munch a <)>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (pToken->GetSubType() != RUL_RPAREN))
		{
			//ADD_ERROR(C_IF_ERROR_MISSINGRP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Eat a Statement...
		CParserBuilder builder;
		if(0!=(m_pStatement = (CStatement*)builder.CreateParser(plexAnal,STMT_asic)))
		{
			_INT32 i32Ret = m_pStatement->CreateParseSubTree(
				plexAnal,
				pSymbolTable,
				pvecErrors);
			if(i32Ret == PARSE_FAIL)
			{
				//ADD_ERROR(C_IF_ERROR_MISSINGSTMT);
			}
		}
		else
		{
			//ADD_ERROR(C_IF_ERROR_MISSINGSTMT);
		}

//See if you can snatch a "else"
		if(0!=(m_pElse = (CELSEStatement*)builder.CreateParser(plexAnal,STMT_SELECTION)))
		{
			m_pElse->CreateParseSubTree(
				plexAnal,
				pSymbolTable);
		}
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

void CSelectionStatement::Identify(
		_CHAR* szData)
{
	//strcat(szData,"<");
	//strcat(szData,"IFStatement");
	//strcat(szData,">");

	//strcat(szData,"<");
	//strcat(szData,"Expression");
	//strcat(szData,">");
	//if (m_pExpression)
	//	m_pExpression->Identify(szData);
	//strcat(szData,"</");
	//strcat(szData,"Expression");
	//strcat(szData,">");
	//if (m_pStatement)
	//	m_pStatement->Identify(szData);

	//strcat(szData,"</");
	//strcat(szData,"IFStatement");
	//strcat(szData,">");

	//if(m_pElse)
	//	m_pElse->Identify(szData);
#ifdef MODIFIED_BY_SS
	std::string result(szData);

	result += "<IFStatement>";

	result += "<Expression>";

	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
	if (m_pExpression)
		m_pExpression->Identify(szData);

	result = szData;
	result += "</Expression>";

	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string

	if (m_pStatement)
		m_pStatement->Identify(szData);

	result = szData;
	result += "</IFStatement>";
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string

	if (m_pElse)
		m_pElse->Identify(szData);

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string

#endif

}

CExpression* CSelectionStatement::GetExpression()
{
	return m_pExpression;
}

CStatement* CSelectionStatement::GetStatement()
{
	return m_pStatement;
}

CELSEStatement* CSelectionStatement::GetELSEStatement()
{
	return m_pElse;
}

_INT32 CSelectionStatement::GetLineNumber()
{
	return m_pStatement->GetLineNumber();
}
