#if !defined(INCLUDE_217B4D8A_300A_40CB_9AD1_44FE76A7F24B)
#define INCLUDE_217B4D8A_300A_40CB_9AD1_44FE76A7F24B

#include "Statement.h"

class INTER_VARIANT;
class CSymbolTable;
class CStatementList;
class CCompoundStatement : public CStatement  
{
public:
	CCompoundStatement();
	virtual ~CCompoundStatement();

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

	CStatementList* GetStatementList();
protected:
	CStatementList*		m_pStmtList;

};

#endif