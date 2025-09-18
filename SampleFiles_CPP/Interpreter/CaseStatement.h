#ifndef CASESTATEMENT_H
#define CASESTATEMENT_H

#include "Statement.h"
#include "CompoundStatement.h"
#include "StatementList.h"
#include "Expression.h"

class CSymbolTable;

class CCASEStatement  : public CStatement
{
public:
	CCASEStatement();
	virtual ~CCASEStatement();

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
		CLexicalAnalyzer*		plexAnal, 
		CSymbolTable*			pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CStatementList* GetStatement();

	CExpression* GetExpression()
	{
		return m_pExpression;
	}

	_BOOL	IsDefaultStatement()
	{
		return m_bIsDefaultCase;
	}
protected:
	CExpression* m_pExpression;
	CStatementList *m_pStatementList;;
	_BOOL		m_bIsDefaultCase;
};

#endif