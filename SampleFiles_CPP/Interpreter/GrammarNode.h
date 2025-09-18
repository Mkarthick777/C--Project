
#if !defined(INCLUDE_BA3E4BD7_51E6_4BFF_8877_A4F53533F0DB)
#define INCLUDE_BA3E4BD7_51E6_4BFF_8877_A4F53533F0DB

#include "LexicalAnalyzer.h"

class CGrammarNodeVisitor;
class CSymbolTable;
class INTER_VARIANT;

// The return codes for the Execute function calls
#define		VISIT_ERROR		0
#define		VISIT_BREAK		1
#define		VISIT_CONTINUE	2
#define		VISIT_RETURN	3
#define		VISIT_NORMAL	4

//Anil 240107 Defined this when declaration come after some statement list
//Basically for the scope of the variable
#define     VISIT_SCOPE_VAR 5
	

enum GRAMMAR_NODE_TYPE
{
	NODE_TYPE_INVALID
	, NODE_TYPE_ASSIGN
	, NODE_TYPE_EXPRESSION
};

class CGrammarNode  
{
private:
	GRAMMAR_NODE_TYPE m_NodeType;
	_INT32			m_CurrentScopeIndex;//SCR26200 Felix

public:
	CGrammarNode();
	virtual ~CGrammarNode();

//	Identify self
	virtual void Identify(
		_CHAR* szData);

	virtual _INT32 Execute(
		CGrammarNodeVisitor*	pVisitor,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN	);//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 CreateParseSubTree(
		CLexicalAnalyzer*	plexAnal, 
		CSymbolTable*		pSymbolTable,
		ERROR_VEC*			pvecErrors);

	virtual _INT32 GetLineNumber();

	void SetNodeType(GRAMMAR_NODE_TYPE nodeType)
	{
		m_NodeType = nodeType;

	}

	GRAMMAR_NODE_TYPE GetNodeType()
	{
		return m_NodeType;
	}

		//This will return the nested depth of the symbols---Felix
	_INT32 GetScopeIndex();
	void SetScopeIndex(_INT32 nSymTblScpIdx);


};

#endif
