//#include "stdafx.h"
#include "pch.h"
#include "CaseStatement.h"
#include "ExpParser.h"
#include "ParserBuilder.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CCASEStatement::CCASEStatement()
{
	m_pStatementList = 0;
	m_pExpression = 0;	//TSRPRASAD 09MAR2004 Fix the memory leaks
	m_bIsDefaultCase = false;
}

CCASEStatement::~CCASEStatement()
{
	DELETE_PTR(m_pExpression);	//TSRPRASAD 09MAR2004 Fix the memory leaks
	DELETE_PTR(m_pStatementList);
}
#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CCASEStatement::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitCASEStatement(
			this,
			pSymbolTable,
			pvar,
			pvecErrors,
			AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif

// Selection Statement is of the form
//	<CASE> <EXPRESSION><:> <Statement>;
_INT32 CCASEStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
//Eat a Statement...
	CToken* pToken=0;
	try
	{
//Munch a <CASE>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (pToken->GetSubType() != RUL_CASE)
			)
		{
			if (pToken->GetSubType() != RUL_DEFAULT)
			{
				DELETE_PTR(pToken);
				throw(C_UM_ERROR_INTERNALERR);
			}
			else
			{
				m_bIsDefaultCase = true;
			}
		}
		DELETE_PTR(pToken);

//Munch & Parse the expression.
//we got to give the expression string to the expression parser.
//	
		if (m_bIsDefaultCase != true)
		{
			CExpParser expParser;
			try
			{
				m_pExpression = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_CASE);
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
		}

//Munch a <:>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (pToken->GetSubType() != RUL_COLON))
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);
			
		if((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			&& pToken
			&& (pToken->GetSubType() == RUL_CASE)
			)
		{
			plexAnal->UnGetToken();
			m_pStatementList = NULL;
			DELETE_PTR(pToken);
			return PARSE_SUCCESS;
		}
		plexAnal->UnGetToken();
		DELETE_PTR(pToken);


//Look for a statement
		m_pStatementList = new CStatementList;
/*		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (pToken->GetSubType() != RUL_LBRACK)
			)
		{
			plexAnal->UnGetToken();
		}
		else
		{
			bCompoundStatement = true;
		}
		DELETE_PTR(pToken);*/

		bool bCompoundStatement = false;
		int iBrackCount = 0;
		while(true)
		{
			if(LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			{
				if ( (pToken->GetSubType() == RUL_LBRACK) &&
					(bCompoundStatement) )
				{
					bCompoundStatement = true;
					iBrackCount++;

					DELETE_PTR(pToken);
				}
				if ( (pToken->GetSubType() == RUL_RBRACK) &&
					(bCompoundStatement) )
				{
					if (bCompoundStatement == true)
					{
						iBrackCount--;
						if (iBrackCount == 0)
						{
							bCompoundStatement = false;
						}
					}
					DELETE_PTR(pToken);
				}

				if ( (pToken->GetSubType() == RUL_CASE)
					|| (pToken->GetSubType() == RUL_DEFAULT))
				{
					plexAnal->UnGetToken();
					DELETE_PTR(pToken);
					break;
				}

				plexAnal->UnGetToken();
				DELETE_PTR(pToken);
			}
			DELETE_PTR(pToken);

			CGrammarNode* pStmt=0;
			CParserBuilder builder;

			pStmt = builder.CreateParser(plexAnal,STMT_asic);
			if (pStmt == 0)
			{
				if (plexAnal->IsEndOfSource())
				{
					return PARSE_SUCCESS;
				}
				else
				{
					return PARSE_FAIL;
				}
			}
			m_pStatementList->AddStatement((CStatement*)pStmt);
			_INT32 i32Ret = pStmt->CreateParseSubTree
										(
											plexAnal,
											pSymbolTable,
											pvecErrors
										);
			if(i32Ret == PARSE_FAIL)
			{
				//ADD_ERROR(C_ES_ERROR_MISSINGSTMT);
			}
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

void CCASEStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*if (m_bIsDefaultCase)
	{
		strcat(szData,"<");
		strcat(szData,"DEFAULT");
		strcat(szData,">");
	}
	else
	{
		strcat(szData,"<");
		strcat(szData,"CASEStatement");
		strcat(szData,">");
	}


	if (m_pExpression)
	{
		m_pExpression->Identify (szData);
	}

	if(m_pStatementList)
	{
		m_pStatementList->Identify(szData);
	}

	if (m_bIsDefaultCase)
	{
		strcat(szData,"</");
		strcat(szData,"DEFAULT");
		strcat(szData,">");
	}
	else
	{
		strcat(szData,"</");
		strcat(szData,"CASEStatement");
		strcat(szData,">");
	}*/
	std::string result; // Create an empty std::string

	if (m_bIsDefaultCase)
	{
		result += "<DEFAULT>";
	}
	else
	{
		result += "<CASEStatement>";
	}
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
	if (m_pExpression)
	{
		m_pExpression->Identify(szData);
	}

	if (m_pStatementList)
	{
		m_pStatementList->Identify(szData);
	}

	if (m_bIsDefaultCase)
	{
		result += "</DEFAULT>";
	}
	else
	{
		result += "</CASEStatement>";
	}

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif
}

CStatementList* CCASEStatement::GetStatement()
{
	return m_pStatementList;
}

_INT32 CCASEStatement::GetLineNumber()
{
	return m_pStatementList->GetLineNumber();
}
