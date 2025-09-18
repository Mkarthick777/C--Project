#ifndef IFEXPRESSION_H
#define IFEXPRESSION_H

#include "Expression.h"
#include "VMConstants.h"

#include <vector>
using namespace std;

class IFExpression : public CExpression  
{
public:
	IFExpression();
	virtual ~IFExpression();

//	Identify self
	virtual void Identify(
		_CHAR* szData);

//	Allow Visitors to do different operations on the node.
	//virtual _INT32 Execute(
	//	CGrammarNodeVisitor* pVisitor, 
	//	CSymbolTable* pSymbolTable,
	//	INTER_VARIANT* pvar=0,
	//	ERROR_VEC*				pvecErrors=0,
	//	RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

//	Create as much of the parse tree as possible.
	virtual _INT32 CreateParseSubTree(
		CLexicalAnalyzer* plexAnal, 
		CSymbolTable* pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	void GetExpressions(
						CExpression* &pIfExpression
						, CExpression* &pTrueExpression
						, CExpression* &pFalseExpression
						)
	{
		pIfExpression = m_pIfExpression;
		pTrueExpression = m_pTrueExpression;
		pFalseExpression = m_pFalseExpression;
	}

protected:
	CExpression*	m_pIfExpression;
	CExpression*	m_pTrueExpression;
	CExpression*	m_pFalseExpression;
};

#endif /* IFEXPRESSION_H */
