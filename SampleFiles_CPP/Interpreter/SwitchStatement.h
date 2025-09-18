#ifndef SWITCHSTATEMENT_H
#define SWITCHSTATEMENT_H

#include "Statement.h"
#include "AssignmentStatement.h"
#include "Expression.h"

#define	 MAX_CASE_STATEMENTS	255

class CCASEStatement;
class CSymbolTable;
class CSwitchStatement : public CStatement  
{
public:
	CSwitchStatement();
	virtual ~CSwitchStatement();

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
		CSymbolTable*		pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CExpression*			GetExpression();
	CAssignmentStatement*	GetExpressionStatement();

	GRAMMAR_NODE_TYPE GetExpressionNodeType()
	{
		return expressionNodeType;
	}

	CStatement*		GetStatement();
	CCASEStatement* GetCaseStatement(int iIndex)
	{
		if (iIndex > m_iNumberOfCasesPresent - 1)
		{
			return NULL;
		}
		else
		{
			return m_pCase[iIndex];
		}
	}

	int		GetNumberOfCaseStatements()
	{
		return m_iNumberOfCasesPresent;
	}

	_BOOL	IsDefaultPresent()
	{
		return m_bIsDefaultPresent;
	}

	CCASEStatement* GetDefaultStatement()
	{
		return m_pDefaultCase;
	}


protected:

	CExpression* m_pExpression;
	CAssignmentStatement*	 m_pExpressionStatement;

	//CStatement*		m_pExpStatement; Commented by TSRPrasad 09MAR2004 to fix memory leaks
	CStatement*		m_pStatement;
	CCASEStatement*	m_pCase[MAX_CASE_STATEMENTS];
	CCASEStatement*	m_pDefaultCase;
	int				m_iNumberOfCasesPresent;
	_BOOL			m_bIsDefaultPresent;

	GRAMMAR_NODE_TYPE expressionNodeType;
};

#endif

