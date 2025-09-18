#if !defined(INCLUDE_5FEA1679_DB06_4EEF_9E16_6D22E641226C)
#define INCLUDE_5FEA1679_DB06_4EEF_9E16_6D22E641226C

#include "Statement.h"
#include "Expression.h"
#include "Variable.h"

class CSymbolTable;
class COMServiceExpression;
class CAssignmentStatement : public CStatement  
{
public:
	CAssignmentStatement();
	virtual ~CAssignmentStatement();

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
		ERROR_VEC*			pvecErrors
		);

	_INT32 CreateParseSubTree(
		CLexicalAnalyzer*	plexAnal, 
		CSymbolTable*		pSymbolTable,
		ERROR_VEC*			pvecErrors,
		STATEMENT_TYPE		stmt_type
		);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	CToken*					GetVariable();
	CExpression*			GetArrayExp();
	CExpression*			GetExpression();
	CExpression*			GetOMExpression();

	CExpression*			m_pExpression;
	CExpression*			GetComplexDDExp();//Anil August 23 2005 for Handling DD variable and Expression

	RUL_TOKEN_SUBTYPE	GetAssignmentType()
	{
		return m_AssignType;
	}
protected:
	CToken*					m_pVariable;
	CExpression*			m_pArrayExp;
	COMServiceExpression*	m_pOMExp;
	bool					m_bLvalueIsArray;
	RUL_TOKEN_SUBTYPE		m_AssignType;
	CExpression*			m_pComplexDDExp;//Added By Anil August 23 2005
	bool					m_bLvalueIsComplexDD;//23 2005 Handling DD variable and Expression
};

#endif