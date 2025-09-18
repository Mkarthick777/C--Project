#if !defined(INCLUDE_08369099_8019_418F_98C1_8EC18EF35649)
#define INCLUDE_08369099_8019_418F_98C1_8EC18EF35649

#include "GrammarNode.h"

class INTER_VARIANT;
class CSymbolTable;
class CStatement : public CGrammarNode  
{
public:
	CStatement();
	virtual ~CStatement();

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
		ERROR_VEC*			pvecErrors=0
		);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();
};

#endif