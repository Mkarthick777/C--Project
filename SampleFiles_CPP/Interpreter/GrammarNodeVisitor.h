
#if !defined(AFX_GRAMMARNODEVISITOR_H__5CFC5FE0_CED8_46B5_BAD9_1E8668FFA30B__INCLUDED_)
#define AFX_GRAMMARNODEVISITOR_H__5CFC5FE0_CED8_46B5_BAD9_1E8668FFA30B__INCLUDED_

#include "typedefs.h"
#include "ParserDeclarations.h"

#ifdef _FULL_RULE_ENGINE

class CStatement;
class CAssignmentStatement;
class CIterationStatement;
class CIterationDoWhileStatement;
class CIterationForStatement;
class CProgram;
class CCompoundStatement;
class CStatementList;
class CDeclarations;
class CELSEStatement;
class CCASEStatement;
class CSelectionStatement;
class CSwitchStatement;
class CArrayExpression;
class CComplexDDExpression;//Anil August 26 2005
class CRuleServiceStatement;
class COMServiceExpression;
class FunctionExpression;
class IFExpression;
class CBreakStatement;
class CContinueStatement;
class CReturnStatement;

#endif

class CExpression;
class CPrimaryExpression;
class CCompoundExpression;
class CSymbolTable;
class INTER_VARIANT;

class CGrammarNodeVisitor  
{
public:
	CGrammarNodeVisitor();
	virtual ~CGrammarNodeVisitor();

	virtual _INT32 visitCompoundExpression(
		CCompoundExpression* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitPrimaryExpression(
		CPrimaryExpression* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitExpression(
		CExpression* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

#ifdef _FULL_RULE_ENGINE

	virtual _INT32 visitArrayExpression(
		CArrayExpression* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitComplexDDExpression(
		CComplexDDExpression* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2


	virtual _INT32 visitAssignment(
		CAssignmentStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitCompoundStatement(
		CCompoundStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitIterationStatement(
		CIterationStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitIterationStatement(
		CIterationDoWhileStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitIterationStatement(
		CIterationForStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitSelectionStatement(
		CSelectionStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitSwitchStatement(
		CSwitchStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitELSEStatement(
		CELSEStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitCASEStatement(
		CCASEStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitProgram(
		CProgram* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitStatement(
		CStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitBreakStatement(
		CBreakStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitReturnStatement(
		CReturnStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitContinueStatement(
		CContinueStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitStatementList(
		CStatementList* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitDeclarations(
		CDeclarations* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitRuleService(
		CRuleServiceStatement* pStatement,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitOMExpression(
		COMServiceExpression* pExpression,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitFunctionExpression(
		FunctionExpression* pExpression,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

	virtual _INT32 visitIFExpression(
		IFExpression* pExpression,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar=0,
		ERROR_VEC*	pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN)	=0;//Anil August 26 2005 to Fix a[exp1] += exp2

#endif
};

#endif