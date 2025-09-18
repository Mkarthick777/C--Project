
//#include "stdafx.h"
#include "pch.h"
#include "ParserBuilder.h"
#include "LexicalAnalyzer.h"
#include "Token.h"
#include "GrammarNode.h"
#include "BreakStatement.h"
#include "ReturnStatement.h"
#include "ContinueStatement.h"
#include "AssignmentStatement.h"
#include "Statement.h"
#include "BreakStatement.h"
#include "ReturnStatement.h"
#include "ContinueStatement.h"
#include "AssignmentStatement.h"
#include "EmptyStatement.h"
#include "CompoundStatement.h"
#include "IterationStatement.h"
#include "IterationDoWhile.h"
#include "IterationFor.h"
#include "SelectionStatement.h"
#include "SwitchStatement.h"
#include "Declaration.h"
#include "ParserDeclarations.h"
#include "ELSEStatement.h"
#include "CASEStatement.h"
#include "ServiceStatement.h"
#include "RuleServiceStatement.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CParserBuilder::CParserBuilder()
{

}

CParserBuilder::~CParserBuilder()
{

}

//Check whether the LOOKAHEAD symbol is a FIRST of a Statement:
//	a.	Assignment Statement
//	b.	Selection Statement
//	c.	Iteration Statement
//	d.	Compound Statement
//	e.	Declaration Statement
//	f.	Else Statement
//	g.	Rule Service Statment
//	h.	Service Statement
CGrammarNode* CParserBuilder::CreateParser(
					CLexicalAnalyzer* plexAnal,
					STATEMENT_TYPE stmt_type)
{
	CGrammarNode* pNode = 0;
	CToken* pToken = 0;

	try
	{
		if((LEX_FAIL != (plexAnal->LookAheadToken(&pToken)))
			&& pToken)
		{
			if (
				((stmt_type == STMT_DECL) || (stmt_type == STMT_asic))
				&& ((pToken->GetType() == RUL_SYMBOL) && (pToken->GetSubType() == RUL_SEMICOLON))
				)
			{
				pNode = new CEmptyStatement;
			}
			else
			if (
				((stmt_type == STMT_DECL) || (stmt_type == STMT_asic))
				&& ((pToken->GetType() == RUL_SYMBOL) && (pToken->GetSubType() == RUL_LPAREN))
				)
			{
				pNode = new CExpression;
			}
			else
			if(((stmt_type == STMT_DECL) || (stmt_type == STMT_asic)) && pToken->IsDeclaration())			//Declaration Statement
			{
				pNode = new CDeclaration;
			}
			else if(((stmt_type == STMT_asic ))
					&& (RUL_KEYWORD== pToken->GetType())
					&& !pToken->IsIteration() 
					&& !pToken->IsFunctionToken())
			{
				if (pToken->GetSubType() == RUL_IF)// IF Selection Statement
				{
					pNode = new CSelectionStatement;
				}
				else
				if (pToken->GetSubType() == RUL_SWITCH)//SWITCH Statement
				{
					pNode = new CSwitchStatement;
				}
				else
				if (RUL_KEYWORD== pToken->GetType() 
					&& RUL_RULE_ENGINE == pToken->GetSubType())		//Rule Statement
				{
					pNode = new CRuleServiceStatement();
				}
				else
				if (RUL_KEYWORD== pToken->GetType() 
					&& RUL_BREAK == pToken->GetSubType())		//break Statement
				{
					pNode = new CBreakStatement;
				}
				else
				if (RUL_KEYWORD== pToken->GetType() 
					&& RUL_CONTINUE == pToken->GetSubType())		//continue Statement
				{
					pNode = new CContinueStatement;
				}
				else
				if (RUL_KEYWORD== pToken->GetType() 
					&& RUL_RETURN == pToken->GetSubType())		//continue Statement
				{
					pNode = new CReturnStatement;
				}
			}
			else if(((stmt_type == STMT_ITERATION) || (stmt_type == STMT_asic ))
					&& pToken->IsIteration())								//Iteration Statement
			{
				if (pToken->IsWHILEStatement())
				{
					pNode = new CIterationStatement;
				}
				else if (pToken->IsDOStatement())
				{
					pNode = new CIterationDoWhileStatement;
				}
				else if (pToken->IsFORStatement())
				{
					pNode = new CIterationForStatement;
				}
			}
			else if(((stmt_type == STMT_ASSIGNMENT)|| (stmt_type == STMT_asic ))
					&& (pToken->IsFunctionToken()
						))		
						//Assignment Statement
			{
				pNode = new CExpression;
			}
			else if(((stmt_type == STMT_ASSIGNMENT)|| (stmt_type == STMT_asic ))
					&& (pToken->IsVariable()	|| pToken->IsArrayVar() || pToken->IsDDItem()//Added By Anil August 4 2005
						|| pToken->IsOMToken() || pToken->IsNumeric()
						|| pToken->IsConstant() || pToken->IsFunctionToken()
						|| pToken->IsOperator() 
						|| ((pToken->GetType() == RUL_SYMBOL) && (pToken->GetSubType() == RUL_LPAREN))
						))		
						//Assignment Statement
			{
				CToken* pNewToken = 0;	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/

				bool bLineIsAssignment = false;
				if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_PLUS_ASSIGN, &pNewToken)	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_MINUS_ASSIGN, &pNewToken)	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_DIV_ASSIGN, &pNewToken)		//TSRPRASAD 09MAR2004 Fix the memory leaks	*/			
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_MOD_ASSIGN, &pNewToken)		//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}

				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_MUL_ASSIGN, &pNewToken) 	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_BIT_AND_ASSIGN, &pNewToken)	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_BIT_OR_ASSIGN, &pNewToken)	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_BIT_XOR_ASSIGN, &pNewToken)	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_BIT_RSHIFT_ASSIGN, &pNewToken)	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_BIT_LSHIFT_ASSIGN, &pNewToken)	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}
				else if (
						plexAnal->ScanLineForToken(RUL_ASSIGNMENT_OPERATOR, RUL_ASSIGN, &pNewToken)	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/
					)
				{
					bLineIsAssignment = true;
				}

				DELETE_PTR(pNewToken);	//TSRPRASAD 09MAR2004 Fix the memory leaks	*/

				if (bLineIsAssignment)
					pNode = new CAssignmentStatement;
				else
					pNode = new CExpression;
			}
			else if(((stmt_type == STMT_ASSIGNMENT)|| (stmt_type == STMT_asic ))
					&& pToken->IsFunctionToken())							//Assignment Statement
			{
				pNode = new CAssignmentStatement;
			}
			else if(((stmt_type == STMT_COMPOUND) || (stmt_type == STMT_asic ))
					&& pToken->IsCompound())								//Compound Statement
			{
				pNode = new CCompoundStatement;
			}
			else if(((stmt_type == STMT_SERVICE) || (stmt_type == STMT_asic ) || (stmt_type == STMT_ASSIGNMENT_FOR))
					&& pToken->IsService())								//Service Statement
			{
				pNode = new CServiceStatement;
			}
			else if(((stmt_type == STMT_SELECTION))
					&& (RUL_KEYWORD== pToken->GetType()))
			{
				if ( (pToken->GetSubType() == RUL_CASE)
					|| (pToken->GetSubType() == RUL_DEFAULT) )//CASE or DEFAULT Statement
				{
					pNode = new CCASEStatement;
				}
				else
				if (pToken->GetSubType() == RUL_ELSE)//ELSE Statement
				{
					pNode = new CELSEStatement;
				}
			}
			else
			{
				//error
				//the natural control flow is allowed to take care of this 
				//erroneous condition.
			}
			DELETE_PTR(pToken);
		}
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return pNode;
}
