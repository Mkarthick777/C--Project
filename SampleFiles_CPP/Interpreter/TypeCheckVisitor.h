#if !defined(AFX_TYPECHECKVISITOR_H__BFE1F2B3_79A6_4ED2_A81E_AFDB3EC389C8__INCLUDED_)
#define AFX_TYPECHECKVISITOR_H__BFE1F2B3_79A6_4ED2_A81E_AFDB3EC389C8__INCLUDED_

#include "LexicalAnalyzer.h"
#include "GrammarNode.h"
#include "GrammarNodeVisitor.h"
#include "ParserDeclarations.h"
#include "VMConstants.h"

enum VARIANT_TYPE;

class CTypeCheckVisitor : public CGrammarNodeVisitor  
{
public:
	CTypeCheckVisitor();
	CTypeCheckVisitor(_CHAR* pszRuleName);
	virtual ~CTypeCheckVisitor();

	_INT32 visitCompoundExpression(
		CCompoundExpression*		pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2


	_INT32 visitPrimaryExpression(
		CPrimaryExpression*			pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitExpression(
		CExpression*				pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

#ifdef _FULL_RULE_ENGINE

	_INT32 visitArrayExpression(
		CArrayExpression*			pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitComplexDDExpression(
		CComplexDDExpression*			pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2


	_INT32 visitAssignment(
		CAssignmentStatement*		pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitBreakStatement(
		CBreakStatement*			pAssStmt,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitReturnStatement(
		CReturnStatement*			pAssStmt,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitContinueStatement(
		CContinueStatement*			pAssStmt,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitCompoundStatement(
		CCompoundStatement*			pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitIterationStatement(
		CIterationStatement*		pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitIterationStatement(
		CIterationDoWhileStatement* pItnStmt,
		CSymbolTable* pSymbolTable,
		INTER_VARIANT* pvar,
		ERROR_VEC*	pvecErrors,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitIterationStatement(
		CIterationForStatement*		pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitSelectionStatement(
		CSelectionStatement*		pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitSwitchStatement(
		CSwitchStatement*		pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitELSEStatement(
		CELSEStatement*				pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitCASEStatement(
		CCASEStatement*				pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitProgram(
		CProgram*					pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitStatement(
		CStatement*					pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitStatementList(
		CStatementList*				pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitDeclarations(
		CDeclarations*				pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitRuleService(
		CRuleServiceStatement*		pStatement,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2


	_INT32 visitOMExpression(
		COMServiceExpression*		pExpression,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitFunctionExpression(
		FunctionExpression*		pExpression,
		CSymbolTable*				pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitIFExpression(
		IFExpression*		pExpression,
		CSymbolTable*		pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*					pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

#endif

protected:
	typedef _INT32 (CTypeCheckVisitor::*PFN_TYPECHECKER)(INTER_VARIANT&,INTER_VARIANT&,INTER_VARIANT&);
	VARIANT_TYPE	m_TypeMapper[MAX_BYTE];
	_CHAR*			m_pszRuleName;
	void			InitializeTable();

	PFN_TYPECHECKER m_fnTypeCheckTable[MAX_BYTE];
	_INT32 tc_uplus(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_uminus(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_add(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_sub(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_mul(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_div(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_mod(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_exp(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_neq(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_lt(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_gt(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_eq(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_ge(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_le(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_land(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_lor(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_lnot(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 tc_rparen(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 TypeCast(
		CCompoundExpression* pexpr,
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	bool needCastNode(
		CGrammarNode* pexpr,
		INTER_VARIANT& v1,			//Input Type
		INTER_VARIANT& v2,			//Input Type
		RUL_TOKEN_SUBTYPE& outOperator,	//Output Operator -- for RUL_EQ becomes RUL_FEQ if the operands are float
		_INT32&			   i32Operand,	//first or second
		RUL_TOKEN_SUBTYPE& castOperator,	//cast Operator -- RUL_I2F or RUL_F2I
		INTER_VARIANT& v3);			//Output Type

};

#endif