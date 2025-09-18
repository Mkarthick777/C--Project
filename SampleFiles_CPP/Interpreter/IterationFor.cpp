//#include "stdafx.h"
#include "pch.h"
#include "IterationFor.h"
#include "SymbolTable.h"
#include "ExpParser.h"
#include "ParserBuilder.h"
#include "Statement.h"
#include "GrammarNodeVisitor.h"
#include "PrimaryExpression.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CIterationForStatement::CIterationForStatement()
{
	m_pExpression = 0;
	m_pStatement = 0;
	m_pIncrementExpression = 0;//Walt:EPM 16aug07 checkin
	m_pIncrementStatement = NULL;
	m_pInitializationStatement = NULL;
}

CIterationForStatement::~CIterationForStatement()
{
	DELETE_PTR(m_pExpression);
	DELETE_PTR(m_pStatement);
	DELETE_PTR(m_pIncrementExpression);//Walt:EPM 16aug07 checkin
	DELETE_PTR(m_pIncrementStatement);
	DELETE_PTR(m_pInitializationStatement);
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CIterationForStatement::Execute(
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
//	<FOR><(><Initialization Statement><;><(><Expression><)><;><Increment Statement><)><Statement>;
_INT32 CIterationForStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken=0;
	try
	{
//Munch a <FOR>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsFORStatement())
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

//Munch a Initialization Statement...
		CParserBuilder builder;
		if(0!=(m_pInitializationStatement = (CAssignmentStatement*)builder.CreateParser(plexAnal,STMT_asic)))
		{
			m_pInitializationStatement->CreateParseSubTree(
				plexAnal,
				pSymbolTable,
				pvecErrors);
		}
		else
		{
//Munch a <;>
			if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
				|| !pToken
				|| !(pToken->GetSubType() == RUL_SEMICOLON))
			{
				//ADD_ERROR(C_WHILE_ERROR_MISSINGLP);
				plexAnal->UnGetToken();
			}
			DELETE_PTR(pToken);
		}

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
					STMT_asic
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
			else if (expressionNodeType == NODE_TYPE_INVALID)
			{
				CToken* pToken = 0;

				try
				{
					if( (LEX_FAIL != (plexAnal->LookAheadToken(&pToken))) && pToken)
					{
						if ( (pToken->GetType() == RUL_SYMBOL) && (pToken->GetSubType() == RUL_SEMICOLON) )
						{
							// This code is to handle for(;;) for Yokagawa EJX
							expressionNodeType = NODE_TYPE_EXPRESSION;
							CToken *pToken2 = new CToken("1");
							pToken2->SetType(RUL_NUMERIC_CONSTANT);
							m_pExpression = new CPrimaryExpression(pToken2);
						}
						DELETE_PTR(pToken);
					}
				}
				catch(...)
				{
				}
			}
			DELETE_PTR(pNode);
		}
		else
		{
			//ADD_ERROR(C_WHILE_ERROR_MISSINGSTMT);
		}

//Munch a <;>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !(pToken->GetSubType() == RUL_SEMICOLON))
		{
			//ADD_ERROR(C_WHILE_ERROR_MISSINGLP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch a Increment Statement...
		pNode = builder2.CreateParser(plexAnal,STMT_asic);

		m_pIncrementExpression = NULL;
		m_pIncrementStatement = NULL;
		if (NULL != pNode)
		{
			incrementNodeType = pNode->GetNodeType();
			if (incrementNodeType == NODE_TYPE_ASSIGN)
			{
				incrementNodeType = NODE_TYPE_ASSIGN;
				m_pIncrementStatement = (CAssignmentStatement*)pNode;

				m_pIncrementStatement->CreateParseSubTree(
					plexAnal,
					pSymbolTable,
					pvecErrors,
					STMT_ASSIGNMENT_FOR
					);
			}
			else if (incrementNodeType == NODE_TYPE_EXPRESSION)
			{
				incrementNodeType = NODE_TYPE_EXPRESSION;
				m_pIncrementExpression 
					= expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_FOR);

				if(!m_pIncrementExpression)
				{
					//ADD_ERROR(C_WHILE_ERROR_MISSINGEXP);
				}
			DELETE_PTR(pNode);
			}
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
			//ADD_ERROR(C_WHILE_ERROR_MISSINGLP);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch the statement
		CParserBuilder builder3;
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

void CIterationForStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,"FORStatement");
	strcat(szData,">");

	if (m_pStatement)
		m_pStatement->Identify(szData);

	strcat(szData,"<");
	strcat(szData,"Expression");
	strcat(szData,">");
	if (m_pExpression)
		m_pExpression->Identify(szData);
	strcat(szData,"</");
	strcat(szData,"Expression");
	strcat(szData,">");
	strcat(szData,"</");

	strcat(szData,"FORStatement");
	strcat(szData,">");*/
	std::string result;

	// Append opening tag for FORStatement
	result += "<FORStatement>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
	// Identify the statement
	if (m_pStatement)
		m_pStatement->Identify(szData);

	// Append opening tag for Expression
	result += "<Expression>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
	// Identify the expression if it exists
	if (m_pExpression)
		m_pExpression->Identify(szData);

	// Append closing tag for Expression
	result += "</Expression>";

	// Append closing tag for FORStatement
	result += "</FORStatement>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif

}

CExpression* CIterationForStatement::GetExpression()
{
	return m_pExpression;
}

CAssignmentStatement* CIterationForStatement::GetExpressionStatement()
{
	return m_pExpressionStatement;
}

CStatement* CIterationForStatement::GetStatement()
{
	return m_pStatement;
}

CStatement* CIterationForStatement::GetInitializationStatement()
{
	return m_pInitializationStatement;
}

CStatement* CIterationForStatement::GetIncrementStatement()
{
	return m_pIncrementStatement;
}

CExpression* CIterationForStatement::GetIncrementExpression()
{
	return m_pIncrementExpression;
}

_INT32 CIterationForStatement::GetLineNumber()
{
	return m_pStatement->GetLineNumber();
}
