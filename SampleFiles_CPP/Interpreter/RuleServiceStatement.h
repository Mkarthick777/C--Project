#if !defined(AFX_RULESERVICESTATEMENT_H__8BCEAD8D_75FF_40A9_B747_ACDF4BEA4485__INCLUDED_)
#define AFX_RULESERVICESTATEMENT_H__8BCEAD8D_75FF_40A9_B747_ACDF4BEA4485__INCLUDED_

#include "Statement.h"
#include "Variable.h"


class CRuleServiceStatement : public CStatement  
{
public:
	CRuleServiceStatement();
	virtual ~CRuleServiceStatement();

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

	CToken* GetRuleToken() 
	{
		return m_pRuleName;
	};

	void GetRuleName(
		_CHAR* szData);

protected:

	CToken* m_pRuleName;
};

#endif
