//#include "stdafx.h"
#include "pch.h"
#include "IterationStatement.h"
#include "SymbolTable.h"
#include "ExpParser.h"
#include "ParserBuilder.h"
#include "Statement.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CIterationStatement::CIterationStatement()
{
	m_pExpression = 0;
	m_pStatement = 0;
}

CIterationStatement::~CIterationStatement()
{
	DELETE_PTR(m_pExpression);
	DELETE_PTR(m_pStatement);
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CIterationStatement::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitIterationStatement(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif
// Iteration Statement is of the form
//	<WHILE><(><Expression><)><Statement>;
_INT32 CIterationStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken=0;
	try
	{
//Munch a <WHILE>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsWHILEStatement())
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);

//Munch a <(>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !(pToken->GetSubType() == RUL_LPAREN))
		{
			//ADD_ERROR(C_WHILE_ERROR_MISSINGLP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch & Parse the expression.
//we got to give the expression string to the expression parser.
//	
		CParserBuilder builder2;
		CExpParser expParser;
		CGrammarNode *pNode;

		pNode = builder2.CreateParser(plexAnal,STMT_asic);

		m_pExpression = NULL;
		m_pExpressionStatement = NULL;
		if (NULL != pNode)
		{
			expressionNodeType = pNode->GetNodeType();
			if (expressionNodeType == NODE_TYPE_ASSIGN)
			{
				expressionNodeType = NODE_TYPE_ASSIGN;
				m_pExpressionStatement = (CAssignmentStatement*)pNode;

				m_pExpressionStatement->CreateParseSubTree(
					plexAnal,
					pSymbolTable,
					pvecErrors,
					STMT_ASSIGNMENT_FOR
					);
			}
			else if (expressionNodeType == NODE_TYPE_EXPRESSION)
			{
				expressionNodeType = NODE_TYPE_EXPRESSION;
				m_pExpression 
					= expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_WHILE);//Vibhor 150305: Changed from EXPR_FOR

				if(!m_pExpression)
				{
					//ADD_ERROR(C_WHILE_ERROR_MISSINGEXP);
				}
			}
			DELETE_PTR(pNode);
		}
		else
		{
			//ADD_ERROR(C_WHILE_ERROR_MISSINGSTMT);
		}

//Munch a <)>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !(pToken->GetSubType() == RUL_RPAREN))
		{
			//ADD_ERROR(C_WHILE_ERROR_MISSINGRP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch a Statement...
		CParserBuilder builder;
		if(0!=(m_pStatement = (CStatement*)builder.CreateParser(plexAnal,STMT_asic)))
		{
			m_pStatement->CreateParseSubTree(
				plexAnal,
				pSymbolTable,
				pvecErrors);
		}
		else
		{
			//ADD_ERROR(C_WHILE_ERROR_MISSINGSTMT);
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

void CIterationStatement::Identify(
		_CHAR* szData)
{

#ifdef MODIFIED_BY_SS

	/*strcat(szData,"<");
	strcat(szData,"WHILEStatement");
	strcat(szData,">");

	strcat(szData,"<");
	strcat(szData,"Expression");
	strcat(szData,">");
	if (expressionNodeType == NODE_TYPE_EXPRESSION)
	{
		m_pExpression->Identify(szData);
	}
	else
	{
		m_pExpressionStatement->Identify(szData);
	}
	strcat(szData,"</");
	strcat(szData,"Expression");
	strcat(szData,">");
	m_pStatement->Identify(szData);

	strcat(szData,"</");
	strcat(szData,"WHILEStatement");
	strcat(szData,">");*/

	std::string result;

	// Append opening tag for WHILEStatement
	result += "<WHILEStatement>";

	// Append opening tag for Expression
	result += "<Expression>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string

	// Identify the expression or expression statement based on node type
	if (expressionNodeType == NODE_TYPE_EXPRESSION)
	{
		if (m_pExpression)
			m_pExpression->Identify(szData);
	}
	else
	{
		if (m_pExpressionStatement)
			m_pExpressionStatement->Identify(szData);
	}

	// Append closing tag for Expression
	result += "</Expression>";

	// Identify the statement
	if (m_pStatement)
		m_pStatement->Identify(szData);

	// Append closing tag for WHILEStatement
	result += "</WHILEStatement>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string


#endif
}

CExpression* CIterationStatement::GetExpression()
{
	return m_pExpression;
}

CAssignmentStatement* CIterationStatement::GetExpressionStatement()
{
	return m_pExpressionStatement;
}

CStatement* CIterationStatement::GetStatement()
{
	return m_pStatement;
}

_INT32 CIterationStatement::GetLineNumber()
{
	return m_pStatement->GetLineNumber();
}
