#ifndef ITERATIONFOR_H
#define ITERATIONFOR_H

#include "Statement.h"
#include "Expression.h"
#include "BreakStatement.h"
#include "ReturnStatement.h"
#include "ContinueStatement.h"
#include "AssignmentStatement.h"

class CSymbolTable;
class CIterationForStatement : public CStatement  
{
public:
	CIterationForStatement();
	virtual ~CIterationForStatement();

//	Identify self
	virtual void Identify(
		_CHAR* szData);

//	Allow Visitors to do different operations on the node.
	//virtual _INT32 Execute(
	//	CGrammarNodeVisitor*	pVisitor,
	//	CSymbolTable*			pSymbolTable,
	//	INTER_VARIANT*		pvar=0,
	//	ERROR_VEC*				pvecErrors=0,
	//	RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

//	Create as much of the parse tree as possible.
	virtual _INT32 CreateParseSubTree(
		CLexicalAnalyzer*	plexAnal, 
		CSymbolTable*	pSymbolTable,
		ERROR_VEC*		pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CExpression*			GetExpression();
	CAssignmentStatement*	GetExpressionStatement();

	CStatement*		GetStatement();

	CStatement*		GetInitializationStatement();
	CStatement*		GetIncrementStatement();
	CExpression*	GetIncrementExpression();
	
	GRAMMAR_NODE_TYPE GetIncrementNodeType()
	{
		return incrementNodeType;
	}

	GRAMMAR_NODE_TYPE GetExpressionNodeType()
	{
		return expressionNodeType;
	}

protected:
	CExpression* m_pExpression;
	CAssignmentStatement*	 m_pExpressionStatement;

	CAssignmentStatement* m_pInitializationStatement;
	CAssignmentStatement* m_pIncrementStatement;
	CExpression* m_pIncrementExpression;
	CStatement* m_pStatement;

	GRAMMAR_NODE_TYPE incrementNodeType;
	GRAMMAR_NODE_TYPE expressionNodeType;
};

#endif
