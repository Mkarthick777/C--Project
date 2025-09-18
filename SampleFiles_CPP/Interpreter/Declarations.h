#if !defined(INCLUDE_5BF3D011_C354_45CF_A633_AF2DA38A136A)
#define INCLUDE_5BF3D011_C354_45CF_A633_AF2DA38A136A

#include "GrammarNode.h"
#include <vector>

using namespace std;
class CDeclaration;

typedef vector<CDeclaration*> DECL_LIST;
class INTER_VARIANT;
class CSymbolTable;
class CDeclarations : public CGrammarNode  
{
public:
	CDeclarations();
	virtual ~CDeclarations();

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

	DECL_LIST* GetDeclarations();
protected:
	DECL_LIST m_declList;
};

#endif