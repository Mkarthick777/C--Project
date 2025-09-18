#if !defined(AFX_ARRAYEXPRESSION_H__03A445C9_BE9D_4D03_9870_939CC06BE1D5__INCLUDED_)
#define AFX_ARRAYEXPRESSION_H__03A445C9_BE9D_4D03_9870_939CC06BE1D5__INCLUDED_

#include "Expression.h"
#include "Token.h"

#include <vector>
using namespace std;

typedef vector<CExpression*> EXPR_VECTOR;

class CArrayExpression : public CExpression  
{
public:
	CArrayExpression();
	CArrayExpression(CToken* pToken);
	virtual ~CArrayExpression();

	void	Identify(
		_CHAR*	szData);

//	Allow Visitors to do different operations on the node.
	//virtual _INT32	Execute(
	//	CGrammarNodeVisitor*	pVisitor,
	//	CSymbolTable*			pSymbolTable,
	//	INTER_VARIANT*		pvar=0,
	//	ERROR_VEC*				pvecErrors=0,
	//	RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

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

#endif