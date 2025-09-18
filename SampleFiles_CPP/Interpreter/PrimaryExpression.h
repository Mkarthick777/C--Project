#if !defined(AFX_PRIMARYEXPRESSION_H__1584F642_7B7E_4B56_9B45_A0101D3480E0__INCLUDED_)
#define AFX_PRIMARYEXPRESSION_H__1584F642_7B7E_4B56_9B45_A0101D3480E0__INCLUDED_

#include "Expression.h"
#include "Token.h"

class CPrimaryExpression : public CExpression  
{
public:
	CPrimaryExpression();
	CPrimaryExpression(CToken* pToken);
	virtual ~CPrimaryExpression();

	void Identify(
		_CHAR* szData);

//	Allow Visitors to do different operations on the node.
	virtual _INT32 Execute(
		CGrammarNodeVisitor* pVisitor,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 For handling DD variable and Expression

//	Create as much of the parse tree as possible.
	virtual _INT32 CreateParseSubTree(
		CLexicalAnalyzer* plexAnal, 
		CSymbolTable* pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CToken* GetToken();

protected:
	
	CToken* m_pToken;
};

#endif