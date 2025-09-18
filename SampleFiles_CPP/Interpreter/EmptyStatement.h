#ifndef EMPTYSTATEMENT_H
#define EMPTYSTATEMENT_H

#include "Statement.h"
#include "Expression.h"
#include "Variable.h"

class CSymbolTable;
class COMServiceExpression;
class CEmptyStatement : public CStatement  
{
public:
	CEmptyStatement();
	virtual ~CEmptyStatement();

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
		CLexicalAnalyzer*	plexAnal, 
		CSymbolTable*		pSymbolTable,
		ERROR_VEC*			pvecErrors
		);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();
};

#endif
