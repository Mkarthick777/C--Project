// ComplexDDExpression.h: interface for the CComplexDDExpression class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DDARRAYEXPRESSION_H__A03448D3_A019_4E30_B938_87BE8186A027__INCLUDED_)
#define AFX_DDARRAYEXPRESSION_H__A03448D3_A019_4E30_B938_87BE8186A027__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Expression.h"
#include "Token.h"

#include <vector>
using namespace std;

typedef vector<CExpression*> EXPR_VECTOR;

class CComplexDDExpression : public CExpression  
{
public:
	CComplexDDExpression();
	CComplexDDExpression(CToken* pToken);
	virtual ~CComplexDDExpression();
		void	Identify(
		_CHAR*	szData);

//	Allow Visitors to do different operations on the node.
	/*virtual _INT32	Execute(
		CGrammarNodeVisitor*	pVisitor,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);*/

//	Create as much of the parse tree as possible.
	virtual _INT32	CreateParseSubTree(
		CLexicalAnalyzer*		plexAnal, 
		CSymbolTable*			pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CToken*			GetToken();
	EXPR_VECTOR*	GetExpressions();

	void	AddDimensionExpr(
		CExpression* pExpr);
protected:
	CToken*		m_pToken;
	EXPR_VECTOR m_vecExpressions;	//each dim in actual array corresponds 
									//to one elt in the vector

};

#endif // !defined(AFX_DDARRAYEXPRESSION_H__A03448D3_A019_4E30_B938_87BE8186A027__INCLUDED_)
