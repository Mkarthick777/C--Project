#if !defined(AFX_SERVICESTATEMENT_H__F3B18E5B_8E75_46DB_B543_5A3D47A7374B__INCLUDED_)
#define AFX_SERVICESTATEMENT_H__F3B18E5B_8E75_46DB_B543_5A3D47A7374B__INCLUDED_

#include "Statement.h"

class CServiceStatement : public CStatement  
{
public:
	CServiceStatement();
	virtual ~CServiceStatement();

//	Identify self
	virtual void Identify(
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

protected:
	CToken* m_pToken;	//this contains the service.
};

#endif
