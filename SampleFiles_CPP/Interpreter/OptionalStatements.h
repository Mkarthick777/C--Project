
#if !defined(INCLUDE_785BC665_EE22_4589_B1F7_F79E88361CC5)
#define INCLUDE_785BC665_EE22_4589_B1F7_F79E88361CC5

#include "GrammarNode.h"


class CStatementList;
class INTER_VARIANT;

class COptionalStatements : public CGrammarNode  
{
public:
	COptionalStatements();
	virtual ~COptionalStatements();

//	Identify self
	virtual void Identify(
		_CHAR* szData);

//	Allow Visitors to do different operations on the node.
	virtual _INT32 Execute(
		CGrammarNodeVisitor* pVisitor,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

//	Create as much of the parse tree as possible.
	virtual _INT32 CreateParseSubTree(
		CLexicalAnalyzer* plexAnal, 
		CSymbolTable* pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CStatementList* GetStatementList();

protected:

	CStatementList* pStmtList;
};

#endif