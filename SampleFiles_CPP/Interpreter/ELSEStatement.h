#if !defined(AFX_ELSESTATEMENT_H__83334E43_2730_440A_BB8D_CD78575070CB__INCLUDED_)
#define AFX_ELSESTATEMENT_H__83334E43_2730_440A_BB8D_CD78575070CB__INCLUDED_

#include "Statement.h"
class CSymbolTable;

class CELSEStatement  : public CStatement
{
public:
	CELSEStatement();
	virtual ~CELSEStatement();

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
		CLexicalAnalyzer*		plexAnal, 
		CSymbolTable*			pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CStatement* GetStatement();

protected:
	CStatement* m_pStatement;
};

#endif