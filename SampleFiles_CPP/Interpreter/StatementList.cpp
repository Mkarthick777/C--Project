//#include "stdafx.h"
#include "pch.h"
#include "Statement.h"
#include "SelectionStatement.h"
#include "SwitchStatement.h"
#include "BreakStatement.h"
#include "ReturnStatement.h"
#include "ContinueStatement.h"
#include "AssignmentStatement.h"
#include "IterationStatement.h"
#include "IterationDoWhile.h"
#include "IterationFor.h"
#include "StatementList.h"
#include "SymbolTable.h"
#include "ParserBuilder.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CStatementList::CStatementList()
{

}

CStatementList::~CStatementList()
{
	_INT32 nSize = m_stmtList.size();

	/*<START>TSRPRASAD 09MAR2004 Fix the memory leaks*/
	stmtListIterator = m_stmtList.begin ();
	while (0 != m_stmtList.size())
	{
		CStatement*pStmt = NULL;
		pStmt = m_stmtList.back();
		//pStmt = (CStatement*)*stmtListIterator;
		if (pStmt != NULL) 
	{
			
			if (pStmt->GetNodeType () == NODE_TYPE_EXPRESSION)
			{
				delete (CExpression *)pStmt;
			}
			else
			{
				delete pStmt;
			}
			pStmt = NULL;
		}
		m_stmtList.pop_back ();
		//m_stmtList;

		
	}


	/*for(_INT32 i=0;i<nSize;i++)
	{
		delete m_stmtList[i];
	}*/
	/*<END>TSRPRASAD 09MAR2004 Fix the memory leaks*/
	
	m_stmtList.clear();
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CStatementList::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitStatementList(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif

//1.	Query the builder for the next parser.
//2.	Ask the parser to build the corrsponding parse tree.
_INT32 CStatementList::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CParserBuilder builder;
	CGrammarNode* pStmt=0;

	int i = 0;

	try
	{
		while(true)
		{
			i++;
			pStmt = builder.CreateParser(plexAnal,STMT_asic);
			if (pStmt == 0)
			{
				if (plexAnal->IsEndOfSource())
				{
					return PARSE_SUCCESS;
				}
				else
				{
					CToken* pToken = 0;

					if((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable))
						&& pToken)
					{
						if (pToken->GetSubType() == RUL_SEMICOLON)
						{
							DELETE_PTR(pToken);
							continue;
						}
						else
						{
							plexAnal->UnGetToken ();
						}
					}
					
					DELETE_PTR(pToken);
					return PARSE_FAIL;
				}
			}
			pStmt->SetScopeIndex(plexAnal->GetSymbolTableScopeIndex());			//SCR26200 Felix
			m_stmtList.push_back((CStatement*)pStmt);
			pStmt->CreateParseSubTree(
				plexAnal,
				pSymbolTable,
				pvecErrors);
		}//end of while loop
		
		return PARSE_SUCCESS;
	}
	catch(...)
	{
		return PARSE_FAIL;
	}
}

void CStatementList::Identify(
			_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*_CHAR szNum1[15];
	_CHAR szNum2[15];
	strcat(szData,"<StatementList>");
	_INT32 nSize = m_stmtList.size();
	for(_INT32 i=0;i<nSize;i++)
	{
		memset(szNum1,0,15);
		memset(szNum2,0,15);
		sprintf(szNum1,"<Statement%02d>",i);
		sprintf(szNum2,"</Statement%02d>",i);
		strcat(szData,szNum1);
		m_stmtList[i]->Identify(szData);
		strcat(szData,szNum2);
	}
	strcat(szData,"</StatementList>");*/
	std::string result(szData);

	result += "<StatementList>";

	for (_INT32 i = 0; i < m_stmtList.size(); i++)
	{
		result += "<Statement";
		result += std::to_string(i);
		result += ">";
		// Copy the result back to szData
		std::copy(result.begin(), result.end(), szData);
		szData[result.size()] = '\0'; // Null-terminate the string

		m_stmtList[i]->Identify(szData);

		result += "</Statement";
		result += std::to_string(i);
		result += ">";
	}

	result += "</StatementList>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif

}

STATEMENT_LIST* CStatementList::GetStmtList()
{
	return &m_stmtList;
}

_INT32 CStatementList::GetLineNumber()
{
	return m_stmtList[m_stmtList.size()-1]->GetLineNumber();
}

bool CStatementList::AddStatement(CGrammarNode* pStmt)
{
	m_stmtList.push_back((CStatement*)pStmt);
	return true;
}