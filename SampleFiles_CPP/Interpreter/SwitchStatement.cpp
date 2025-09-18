//#include "stdafx.h"
#include "pch.h"
#include "SwitchStatement.h"
#include "SymbolTable.h"
#include "ParserBuilder.h"
#include "Statement.h"
#include "CASEStatement.h"
#include "ExpParser.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CSwitchStatement::CSwitchStatement()
{
	m_pExpression = 0;
	m_pExpressionStatement = NULL;	//TSRPRASAD 09MAR2004 Fix the memory leaks
	m_pStatement = 0;
	m_iNumberOfCasesPresent = 0;
	for (int iLoopVar = 0;iLoopVar < MAX_CASE_STATEMENTS;iLoopVar++)
	{
		m_pCase[iLoopVar] = NULL;
	}
	m_pDefaultCase = NULL;
	m_bIsDefaultPresent = false;
}

CSwitchStatement::~CSwitchStatement()
{
	DELETE_PTR(m_pExpression);
	DELETE_PTR(m_pExpressionStatement);	//TSRPRASAD 09MAR2004 Fix the memory leaks
	DELETE_PTR(m_pStatement);

	for (int iLoopVar = 0;iLoopVar < MAX_CASE_STATEMENTS;iLoopVar++)
	{
		DELETE_PTR(m_pCase[iLoopVar]);
	}
	if (m_pDefaultCase)
	{
		delete m_pDefaultCase;
	}
	m_pDefaultCase = NULL;	//TSRPRASAD 09MAR2004 Fix the memory leaks
}
#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
//_INT32 CSwitchStatement::Execute(
//			CGrammarNodeVisitor* pVisitor,
//			CSymbolTable* pSymbolTable,
//			INTER_VARIANT* pvar,
//			ERROR_VEC*	pvecErrors,
//			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
//{
//	return pVisitor->visitSwitchStatement(
//		this,
//		pSymbolTable,
//		pvar,
//		pvecErrors,
//		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
//}
#endif
#endif
// Selection Statement is of the form
//	<Switch><(><Expression><)> <Case ><:><Statement> ;
_INT32 CSwitchStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken=0;
	try
	{
//Munch a <SWITCH>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (pToken->GetSubType() != RUL_SWITCH))
		{
			DELETE_PTR(pToken);//dont leak memory in error conditions
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
		CParserBuilder builder2;
		CExpParser expParser;
		CGrammarNode *pNode = NULL;

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
					= expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_FOR);

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
			|| (pToken->GetSubType() != RUL_RPAREN))
		{
			//ADD_ERROR(C_IF_ERROR_MISSINGRP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch a <{>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (pToken->GetSubType() != RUL_LBRACK))
		{
			//ADD_ERROR(C_IF_ERROR_MISSINGRP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//See if you can snatch a "case"
		CParserBuilder builder;
		int iNumberOfCaseStatements = 0;
		while(0!=(m_pCase[iNumberOfCaseStatements] 
					= (CCASEStatement*)builder.CreateParser(plexAnal,STMT_SELECTION)))
		{
			m_pCase[iNumberOfCaseStatements]->CreateParseSubTree
				(
					plexAnal,
					pSymbolTable
				);
			if (m_pCase[iNumberOfCaseStatements]->IsDefaultStatement())
			{
				m_bIsDefaultPresent = true;
				m_pDefaultCase = m_pCase[iNumberOfCaseStatements];
			}
			else
			{
				iNumberOfCaseStatements++;
			}
		}
		m_iNumberOfCasesPresent = iNumberOfCaseStatements;

//Munch a <}>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (pToken->GetSubType() != RUL_RBRACK))
		{
			//ADD_ERROR(C_IF_ERROR_MISSINGRP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

		/* VMKP Commented on 140404,  For INOR device 
			Sensor selection method is crashing with this Fix */
	/*	if (pNode)	//TSRPRASAD 09MAR2004 Fix the memory leaks
		{
			delete pNode;
			pNode = NULL;
		}*/
		/* VMKP Commented on 140404 */

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

void CSwitchStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,"SWITCHStatement");
	strcat(szData,">");

	strcat(szData,"</");
	strcat(szData,"SWITCHStatement");
	strcat(szData,">");*/
	std::string result(szData);

	result += "<SWITCHStatement>";
	result += "</SWITCHStatement>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif
}

CExpression* CSwitchStatement::GetExpression()
{
	return m_pExpression;
}

CAssignmentStatement* CSwitchStatement::GetExpressionStatement()
{
	return m_pExpressionStatement;
}

CStatement* CSwitchStatement::GetStatement()
{
	return m_pStatement;
}

/*CELSEStatement* CSwitchStatement::GetELSEStatement()
{
	return m_pElse;
}
*/
_INT32 CSwitchStatement::GetLineNumber()
{
	return m_pStatement->GetLineNumber();
}
