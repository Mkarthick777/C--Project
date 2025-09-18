#if !defined(INCLUDE_3550CA09_D5A1_4920_8788_B7819C7E0CE7)
#define INCLUDE_3550CA09_D5A1_4920_8788_B7819C7E0CE7

#include "GrammarNode.h"
#include <vector>

using namespace std;
class CStatement;

typedef vector<CStatement*> STATEMENT_LIST;
class CSymbolTable;

class CStatementList : public CGrammarNode  
{
public:
	CStatementList();
	virtual ~CStatementList();

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

	STATEMENT_LIST* GetStmtList();

	bool	AddStatement(CGrammarNode* pStmt);

protected:

	STATEMENT_LIST m_stmtList;
	STATEMENT_LIST::iterator stmtListIterator;	//TSRPRASAD 09MAR2004 Fix the memory leaks

	
};

#endif