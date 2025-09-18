#if !defined(AFX_EXPPARSER_H__F1AD6F8B_76EE_4668_8863_422774062590__INCLUDED_)
#define AFX_EXPPARSER_H__F1AD6F8B_76EE_4668_8863_422774062590__INCLUDED_

#pragma warning(disable:4786)

#include "ParserDeclarations.h"
#include "Token.h"
#include "Expression.h"

using namespace std;

#include <stack>
#include <map>

struct Precedence
{
	_INT32 StackTop;	//used by function f
	_INT32 Incoming;	//used by function g
	_INT32 n;	//number of operands.
	_INT32 t;	//number of terminals to be lifted.

	Precedence()
	{
		StackTop=0;
		Incoming=0;
		n=0;
		t=0;
	};
};

typedef stack<CToken*> TERMINALS_STACK;
typedef stack<CExpression*> NON_TERMINALS_STACK;
typedef map<_INT32,Precedence> PRECEDENCE_TABLE;


class CLexicalAnalyzer;
class CSymbolTable;

//This class implements a Operator precedence 
//parsing algorithm with precedence functions...

class CExpParser  
{
public:
	CExpParser();
	virtual ~CExpParser();

	CExpression* ParseExpression(
		CLexicalAnalyzer*	plexAnal, 
		CSymbolTable*		pSymbolTable,
		STMT_EXPR_TYPE		expr,
		ERROR_VEC*			pvecErrors=0,
		bool				bLookForQMark=true);

	void Reduce(
		RUL_TOKEN_SUBTYPE&	SubType,
		CSymbolTable*		pSymbolTable,
		CLexicalAnalyzer* plexAnal);//Anil August 26 2005 for //Handling DD variable and Expression. to get the MEE ptr throgh plexAnal

	static	void InitializePrecedenceTable();
protected:
	TERMINALS_STACK			m_Terminals;
	NON_TERMINALS_STACK		m_NonTerminals;
	static PRECEDENCE_TABLE	s_PrecedenceTable;
	CToken					m_Dollar;

	void	EmptyStacks(
		bool IsTerminal, 
		bool IsNonTerminal);

	_INT32	IncomingTokenPrecedence(
				RUL_TOKEN_SUBTYPE SubType);

	_INT32	StackTopTokenPrecedence(
				RUL_TOKEN_SUBTYPE SubType);

	CToken*	GetActualToken(
		CLexicalAnalyzer* plexAnal,
		CSymbolTable*		pSymbolTable,
		STMT_EXPR_TYPE expr,
		_INT32	i32BrackCount,
		CToken** ppNextToken,
		bool& bIsShiftOver);
private:
	//Anil August 26 2005
	//for handling DD variable and Expression
	//This is to get any expression within [] and to isert in to DDcomplexExp class
	bool InsertDDExpr(  
		const char* pszComplexDDExpre,
		CExpression* CddExp,
		CSymbolTable* pSymbolTable,
		CLexicalAnalyzer* plexAnal,
		CToken* pToken
		);
};

#endif