#if !defined(AFX_INTERPRETEDVISITOR_H__CAFBA8E8_B9AB_43A3_B9C2_344BACCAD4B8__INCLUDED_)
#define AFX_INTERPRETEDVISITOR_H__CAFBA8E8_B9AB_43A3_B9C2_344BACCAD4B8__INCLUDED_

#include "GrammarNode.h"
#include "GrammarNodeVisitor.h"
#include "ParserDeclarations.h"
#include "VMConstants.h"
#include "Program.h"
#include "Declarations.h"
#include "StatementList.h"
#include "Statement.h"
#include "SelectionStatement.h"
#include "SwitchStatement.h"
#include "AssignmentStatement.h"
#include "BreakStatement.h"
#include "ReturnStatement.h"
#include "ContinueStatement.h"
#include "Variable.h"
#include "Expression.h"
#include "INTER_VARIANT.h"
#include "SymbolTable.h"
#include "PrimaryExpression.h"
#include "CompoundExpression.h"
#include "IterationStatement.h"
#include "IterationDoWhile.h"
#include "IterationFor.h"
#include "CompoundStatement.h"
#include "ELSEStatement.h"
#include "CASEStatement.h"
#include "INTER_SAFEARRAY.h"
#include "assert.h"
#include "ArrayExpression.h"
#include "ComplexDDExpression.h"//Anil August 26 2005
#include "RuleServiceStatement.h"
#include "OMServiceExpression.h"
#include "FunctionExpression.h"
#include "IFExpression.h" // stevev 07mar07

class CHart_Builtins;
class MEE;	//Vibhor 070705: Added

class CInterpretedVisitor : public CGrammarNodeVisitor  
{
private:
#ifdef STANDALONE_PARSER

CBuiltInLib  *m_pBuiltInLib;

#else

CHart_Builtins *m_pBuiltInLib;

#endif

MEE *m_pMEE; //Vibhor 070705: Added
bool m_bIsRoutine;//Anil Octobet 5 2005 for handling Method Calling Method
public:
	CInterpretedVisitor();
	virtual ~CInterpretedVisitor();
	void Initialize(CHart_Builtins  *pBuiltInLib, MEE* pMEE) //Vibhor 070705: Modified
	{
		m_pBuiltInLib = pBuiltInLib;
		m_pMEE		  = pMEE;
	}

	_INT32 visitArrayExpression(
		CArrayExpression*		pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2
	_INT32 visitComplexDDExpression(
		CComplexDDExpression*		pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*			pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitAssignment(
		CAssignmentStatement*	pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitBreakStatement(
		CBreakStatement*		pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitReturnStatement(
		CReturnStatement*		pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitContinueStatement(
		CContinueStatement*		pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitCompoundStatement(
		CCompoundStatement*		pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitIterationStatement(
		CIterationStatement*	pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitIterationStatement(
		CIterationDoWhileStatement*	pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN); //Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitIterationStatement(
		CIterationForStatement*	pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitSelectionStatement(
		CSelectionStatement*	pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitSwitchStatement(
		CSwitchStatement*		pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitELSEStatement(
		CELSEStatement*			pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitCASEStatement(
		CCASEStatement*			pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitCompoundExpression(
		CCompoundExpression*	pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitPrimaryExpression(
		CPrimaryExpression*		pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitProgram(
		CProgram*				pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitExpression(
		CExpression*			pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitStatement(
		CStatement*				pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitStatementList(
		CStatementList*			pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitDeclarations(
		CDeclarations*			pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitRuleService(
		CRuleServiceStatement*	pStatement,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitOMExpression(
		COMServiceExpression*	pExpression,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitFunctionExpression(
		FunctionExpression*		pExpression,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

	_INT32 visitIFExpression(
		IFExpression*		pExpression,
		CSymbolTable*			pSymbolTable,
		INTER_VARIANT*		pvar=0,
		ERROR_VEC*				pvecErrors=0,
		RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

		void SetIsRoutineFlag(bool bisRoutine);//Anil Octobet 5 2005 for handling Method Calling Method
		bool GetIsRoutineFlag();//Anil Octobet 5 2005 for handling Method Calling Method


protected:

 	typedef _INT32 (CInterpretedVisitor::*PFN_INTERPRETER)(INTER_VARIANT&,INTER_VARIANT&,INTER_VARIANT&);
	_INT32 uplusplus(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 uminusminus(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 upreplusplus(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 upreminusminus(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 uplus(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 uminus(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);
		
	_INT32 bit_and(     //linux defines bitand & bitor
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);
		
	_INT32 bit_or(     //linux defines bitand & bitor
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 bitxor(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 bitnot(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 bitrshift(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 bitlshift(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 add(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 sub(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 mul(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 div(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 mod(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 exp(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 neq(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 lt(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 gt(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 eq(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 ge(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 le(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 land(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 lor(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 lnot(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 rparen(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 assign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 plusassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 minusassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 divassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 modassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 mulassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 bitandassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 bitorassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 bitxorassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 rshiftassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);

	_INT32 lshiftassign(
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		INTER_VARIANT& v3);


	PFN_INTERPRETER m_fnTable[MAX_BYTE];

	bool			m_IsLValue;

};

#endif 