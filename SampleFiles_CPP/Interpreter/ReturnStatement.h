#ifndef RETURNSTATEMENT_H
#define RETURNSTATEMENT_H

#include "Statement.h"
#include "Expression.h"//Anil Octobet 5 2005 for handling Method Calling Method
#include "Variable.h"//Anil Octobet 5 2005 for handling Method Calling Method


class CSymbolTable;
class COMServiceExpression;
class CReturnStatement: public CStatement  
{
public:
	CReturnStatement();
	virtual ~CReturnStatement();

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
		ERROR_VEC*			pvecErrors
		);

	_INT32 CreateParseSubTree(
		CLexicalAnalyzer*	plexAnal, 
		CSymbolTable*		pSymbolTable,
		ERROR_VEC*			pvecErrors,
		STATEMENT_TYPE		stmt_type
		);

	CExpression*			GetExpression();//Anil Octobet 5 2005 for handling Method Calling Method

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();
	CExpression*			m_pExpression;//Anil Octobet 5 2005 for handling Method Calling Method

protected:
	int	i32LineNumber;
};

#endif