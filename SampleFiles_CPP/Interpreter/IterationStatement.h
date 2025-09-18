#if !defined(INCLUDE_F074C2FC_9693_4C79_9EAD_5563DD9A6E6A)
#define INCLUDE_F074C2FC_9693_4C79_9EAD_5563DD9A6E6A

#include "Statement.h"
#include "AssignmentStatement.h"
#include "Expression.h"

class CSymbolTable;
class CIterationStatement : public CStatement  
{
public:
	CIterationStatement();
	virtual ~CIterationStatement();

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

	GRAMMAR_NODE_TYPE GetExpressionNodeType()
	{
		return expressionNodeType;
	}

protected:
	CExpression* m_pExpression;
	CAssignmentStatement*	 m_pExpressionStatement;

	CStatement* m_pStatement;

	GRAMMAR_NODE_TYPE expressionNodeType;
};

#endif