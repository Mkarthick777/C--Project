#if !defined(INCLUDE_6A0B964F_7C3C_443C_8ABA_7D449E588CCE)
#define INCLUDE_6A0B964F_7C3C_443C_8ABA_7D449E588CCE

#include "Statement.h"
#include "Expression.h"

class CELSEStatement;
class CSymbolTable;
class CSelectionStatement : public CStatement  
{
public:
	CSelectionStatement();
	virtual ~CSelectionStatement();

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
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CExpression*	GetExpression();
	CStatement*		GetStatement();
	CELSEStatement* GetELSEStatement();

protected:

	CExpression*	m_pExpression;
	CStatement*		m_pStatement;
	CELSEStatement*	m_pElse;

};

#endif