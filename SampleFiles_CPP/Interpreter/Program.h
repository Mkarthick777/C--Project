
#if !defined(INCLUDE_0C665ED6_449C_4A9E_BBAB_86D368D7BFCB)
#define INCLUDE_0C665ED6_449C_4A9E_BBAB_86D368D7BFCB

#include "GrammarNode.h"


class INTER_VARIANT;
class CDeclarations;
class CStatementList;
class CSymbolTable;

class CProgram : public CGrammarNode  
{
public:
	CProgram();
	virtual ~CProgram();

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

	CDeclarations* GetDeclarations();
	CStatementList* GetStatementList();

protected:

	CDeclarations* m_pDeclarations;
	CStatementList* m_pStmtList;
};

#endif