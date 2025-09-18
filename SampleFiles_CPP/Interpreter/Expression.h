#if !defined(INCLUDE_3AB452BE_7B70_4430_BD0B_BC9DB5EED5C2)
#define INCLUDE_3AB452BE_7B70_4430_BD0B_BC9DB5EED5C2

#include "GrammarNode.h"
#include "ParserDeclarations.h"

class INTER_VARIANT;
class CSymbolTable;
class CExpression : public CGrammarNode  
{
public:
	CExpression();
	virtual ~CExpression();
//	Identify self
	virtual void Identify(
		_CHAR* szData);

//	Allo\w Visitors to do different operations on the node.
	virtual _INT32 Execute(
		CGrammarNodeVisitor*	pVisitor,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

//	Create as much of the parse tree as possible.
	virtual _INT32 CreateParseSubTree(
		CLexicalAnalyzer*		plexAnal, 
		CSymbolTable*			pSymbolTable,
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CExpression* GetExpression()
	{
		return m_pExpression;
	}

protected:

	CExpression*	m_pExpression;


};

#endif