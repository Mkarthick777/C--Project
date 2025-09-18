#if !defined(AFX_DECLARATION_H__AA58ED1E_00C9_4E80_A0A0_36129355E4F7__INCLUDED_)
#define AFX_DECLARATION_H__AA58ED1E_00C9_4E80_A0A0_36129355E4F7__INCLUDED_

#include "GrammarNode.h"

class INTER_VARIANT;
class CSymbolTable;
class CDeclaration : public CGrammarNode  
{
public:
	CDeclaration();
	virtual ~CDeclaration();
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

};

#endif