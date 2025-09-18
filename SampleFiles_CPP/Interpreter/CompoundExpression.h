#if !defined(AFX_COMPOUNDEXPRESSION_H__6FBAB873_AB71_4718_8E62_0083DA60AE82__INCLUDED_)
#define AFX_COMPOUNDEXPRESSION_H__6FBAB873_AB71_4718_8E62_0083DA60AE82__INCLUDED_

#include "Expression.h"

class INTER_VARIANT;
class CCompoundExpression : public CExpression  
{
public:
	CCompoundExpression();

	CCompoundExpression(
		CExpression* f,
		CExpression* s,
		RUL_TOKEN_SUBTYPE Op);

	virtual ~CCompoundExpression();

	void Identify(
		_CHAR* szData);

//	Allow Visitors to do different operations on the node.
	virtual _INT32 Execute(
		CGrammarNodeVisitor*	pVisitor,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

//	Create as much of the parse tree as possible.
	virtual _INT32 CreateParseSubTree(
		CLexicalAnalyzer*		plexAnal, 
		CSymbolTable*			pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CExpression*		GetFirstExpression();
	CExpression*		GetSecondExpression();
	RUL_TOKEN_SUBTYPE	GetOperator();

	CExpression*		m_pFirstExp;
	CExpression*		m_pSecondExp;
	RUL_TOKEN_SUBTYPE	m_Operator;

protected:

};

#endif