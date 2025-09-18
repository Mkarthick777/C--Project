//#include "stdafx.h"
#include "pch.h"
#include "ErrorDefinitions.h"
#include "TypeCheckVisitor.h"
#include "Variable.h"
#include "Expression.h"
#include "PrimaryExpression.h"
#include "CompoundExpression.h"
#include "INTER_SAFEARRAY.h"
#include "assert.h"

#ifdef _FULL_RULE_ENGINE

#include "Program.h"
#include "Declarations.h"
#include "StatementList.h"
#include "Statement.h"
#include "SelectionStatement.h"
#include "SwitchStatement.h"
#include "BreakStatement.h"
#include "ReturnStatement.h"
#include "ContinueStatement.h"
#include "AssignmentStatement.h"
#include "INTER_VARIANT.h"
#include "SymbolTable.h"
#include "IterationStatement.h"
#include "IterationDoWhile.h"
#include "IterationFor.h"
#include "CompoundStatement.h"
#include "ELSEStatement.h"
#include "CASEStatement.h"
#include "ArrayExpression.h"
#include "ComplexDDExpression.h"//Anil August 26 2005 For handling DD variable and Expression
#include "RuleServiceStatement.h"
#include "OMServiceExpression.h"
#include "FunctionExpression.h"
//#include "RIDEError.h"
#include "ErrorDefinitions.h"

#endif

void CTypeCheckVisitor::InitializeTable()
{
	m_TypeMapper[RUL_INT_CONSTANT]		= RUL_INT;
	m_TypeMapper[RUL_CHAR_CONSTANT]		= RUL_CHAR;
	m_TypeMapper[RUL_UNSIGNED_INTEGER_DECL]		= RUL_UINT;
	m_TypeMapper[RUL_INTEGER_DECL]		= RUL_INT;
	m_TypeMapper[RUL_LONG_DECL]			= RUL_INT;
	m_TypeMapper[RUL_LONG_LONG_DECL]	= RUL_LONGLONG;
	m_TypeMapper[RUL_UNSIGNED_SHORT_INTEGER_DECL]= RUL_USHORT;
	m_TypeMapper[RUL_SHORT_INTEGER_DECL]= RUL_SHORT;
	m_TypeMapper[RUL_REAL_CONSTANT]		= RUL_FLOAT;
	m_TypeMapper[RUL_REAL_DECL]			= RUL_FLOAT;
	m_TypeMapper[RUL_DOUBLE_DECL]		= RUL_DOUBLE;
	m_TypeMapper[RUL_BOOL_CONSTANT]		= RUL_BOOL;
	m_TypeMapper[RUL_BOOLEAN_DECL]		= RUL_BOOL;
	m_TypeMapper[RUL_STRING_CONSTANT]	= RUL_CHARPTR;
	m_TypeMapper[RUL_STRING_DECL]		= RUL_CHARPTR;
	m_TypeMapper[RUL_UNSIGNED_CHAR_DECL]= RUL_UNSIGNED_CHAR;

	m_fnTypeCheckTable[RUL_UPLUS]		= &CTypeCheckVisitor::tc_uplus;	// added &CTypeCheckVIsitor:: PAW 03/03/09
	m_fnTypeCheckTable[RUL_UMINUS]		= &CTypeCheckVisitor::tc_uminus;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_PLUS]		= &CTypeCheckVisitor::tc_add;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_MINUS]		= &CTypeCheckVisitor::tc_sub;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_MUL]			= &CTypeCheckVisitor::tc_mul;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_DIV]			= &CTypeCheckVisitor::tc_div;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_MOD]			= &CTypeCheckVisitor::tc_mod;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_EXP]			= &CTypeCheckVisitor::tc_exp;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_NOT_EQ]		= &CTypeCheckVisitor::tc_neq;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_LT]			= &CTypeCheckVisitor::tc_lt;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_GT]			= &CTypeCheckVisitor::tc_gt;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_EQ]			= &CTypeCheckVisitor::tc_eq;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_GE]			= &CTypeCheckVisitor::tc_ge;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_LE]			= &CTypeCheckVisitor::tc_le;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_LOGIC_AND]	= &CTypeCheckVisitor::tc_land;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_LOGIC_OR]	= &CTypeCheckVisitor::tc_lor;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_LOGIC_NOT]	= &CTypeCheckVisitor::tc_lnot;// added &CTypeCheckVIsitor:: PAW
	m_fnTypeCheckTable[RUL_RPAREN]		= &CTypeCheckVisitor::tc_rparen;// added &CTypeCheckVIsitor:: PAW
}

CTypeCheckVisitor::CTypeCheckVisitor()
{
	m_pszRuleName = 0;
	InitializeTable();
}

CTypeCheckVisitor::CTypeCheckVisitor(_CHAR* pszRuleName)
{
	m_pszRuleName = new _CHAR[strlen(pszRuleName)+1];
#ifdef MODIFIED_BY_SS
	std::string szName(pszRuleName);
	//strcpy(m_pszRuleName,pszRuleName);
	std::copy(szName.begin(), szName.end(), m_pszRuleName);
	InitializeTable();
#endif
}

CTypeCheckVisitor::~CTypeCheckVisitor()
{
	DELETE_ARR( m_pszRuleName );
}

#ifdef _FULL_RULE_ENGINE

_INT32 CTypeCheckVisitor::visitArrayExpression(
				CArrayExpression* pArrExp,
				CSymbolTable* pSymbolTable,
				INTER_VARIANT* pvar,
				ERROR_VEC*	pvecErrors,
				RUL_TOKEN_SUBTYPE	AssignType)
{
	try
	{
		assert(pvar!=0);

		_INT32 i32Idx			= pArrExp->GetToken()->GetSymbolTableIndex();
		CVariable* pVariable	= pSymbolTable->GetAt(i32Idx);
		INTER_SAFEARRAY* prgsa	= pVariable->GetValue().GetValue().prgsa;

	//evaluate the expressions...
		EXPR_VECTOR* pvecExpressions = pArrExp->GetExpressions();
		vector<_INT32> vecDims;
		prgsa->GetDims(&vecDims);

		if(vecDims.size() != pvecExpressions->size())
		{
			throw(C_TC_ERROR_DIM_MISMATCH,pArrExp);
		}
		switch( (VARIANT_TYPE)prgsa->Type())
		{
			case RUL_CHAR:
				*pvar = (char)' ';
				break;
			case RUL_INT:
				*pvar = (_INT32)0;// a long isn't always the same as an int
				break;
			case RUL_BOOL:
				*pvar = (bool)false;
				break;
			case RUL_FLOAT:
				*pvar = (float)0.0;//WS:EPM 10aug07
				break;
			case RUL_DOUBLE:
				*pvar = (double)0.0;//WS:EPM 10aug07
				break;
			case RUL_CHARPTR:
			case RUL_SAFEARRAY:
			case RUL_DD_STRING:
			case RUL_UNSIGNED_CHAR:
				*pvar = (char *)"";
				break;
		}
		return TYPE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pArrExp);
	}
	return TYPE_FAILURE;
}

_INT32 CTypeCheckVisitor::visitComplexDDExpression(
				CComplexDDExpression* pArrExp,
				CSymbolTable* pSymbolTable,
				INTER_VARIANT* pvar,
				ERROR_VEC*	pvecErrors,
				RUL_TOKEN_SUBTYPE	AssignType)
{
	try
	{
		assert(pvar!=0);

		_INT32 i32Idx			= pArrExp->GetToken()->GetSymbolTableIndex();
		CVariable* pVariable	= pSymbolTable->GetAt(i32Idx);
		INTER_SAFEARRAY* prgsa	= pVariable->GetValue().GetValue().prgsa;

	//evaluate the expressions...
		EXPR_VECTOR* pvecExpressions = pArrExp->GetExpressions();
		vector<_INT32> vecDims;
		prgsa->GetDims(&vecDims);

		if(vecDims.size() != pvecExpressions->size())
		{
			throw(C_TC_ERROR_DIM_MISMATCH,pArrExp);
		}
		switch( (VARIANT_TYPE)prgsa->Type())
		{
			case RUL_CHAR:
				*pvar = (char)' ';
				break;
			case RUL_INT:
				*pvar = (_INT32)0;// a long is not an int
				break;
			case RUL_BOOL:
				*pvar = (bool)false;
				break;
			case RUL_FLOAT:
				*pvar = (float)0.0;//WS:EPM 10aug07
				break;
			case RUL_DOUBLE:
				*pvar = (double)0.0;//WS:EPM 10aug07
				break;
			case RUL_CHARPTR:
			case RUL_SAFEARRAY:
			case RUL_DD_STRING:
			case RUL_UNSIGNED_CHAR:
				*pvar = (char *)"";
				break;
		}
		return TYPE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pArrExp);
	}
	return TYPE_FAILURE;
}

// We have to check whether the LHS and the RHS types
// are the same. If not, we have a problem on our hands

_INT32 CTypeCheckVisitor::visitAssignment(
				CAssignmentStatement* pAssStmt,
				CSymbolTable* pSymbolTable,
				INTER_VARIANT* pvar,
				ERROR_VEC*	pvecErrors,
				RUL_TOKEN_SUBTYPE	AssignType)
{
	CToken* pVariable = 0;
	CExpression* pExp = 0;
	CExpression* pArray=0;
	_INT32	i32Ret=TYPE_SUCCESS;

	try
	{
		if(pAssStmt)
		{
			pVariable = pAssStmt->GetVariable();
			pExp = pAssStmt->GetExpression();
			pArray = pAssStmt->GetArrayExp();

			INTER_VARIANT var;
			if(pExp)
			{
				i32Ret = pExp->Execute(this,pSymbolTable,&var,pvecErrors);
			}

			if(pVariable)
			{
				if(m_TypeMapper[pVariable->GetSubType()] == var.GetVarType())
				{
					// no probs here as the types are the same.
				}
				else
				{
					// we got to check whether the lhs is a int and the 
					//rhs is a float or the other way around
					//in which case our language permits us to 
					// do a downcast or an upcast.
					if((m_TypeMapper[pVariable->GetSubType()] == RUL_INT)
						&& var.GetVarType() == RUL_FLOAT)
					{
						//introduce a downcast node f2i.
						CCompoundExpression* pf2iNode = new CCompoundExpression(pExp,0,RUL_F2I);
						pAssStmt->m_pExpression = pf2iNode;

					}
					else if(
						(m_TypeMapper[pVariable->GetSubType()] == RUL_FLOAT)
						&& var.GetVarType() == RUL_INT)
					{
						//introduce a upcast node i2f.
						CCompoundExpression* pi2fNode = new CCompoundExpression(pExp,0,RUL_I2F);
						pAssStmt->m_pExpression = pi2fNode;
					}
					else
					{

						INTER_VARIANT var1;
						var1 = m_TypeMapper[pVariable->GetSubType()];
						_INT32 i32Operand=0;
						RUL_TOKEN_SUBTYPE outOperator = RUL_ASSIGN;
						RUL_TOKEN_SUBTYPE castOperator;
						INTER_VARIANT v3;
						needCastNode(pAssStmt,
							var1,var,outOperator,i32Operand,castOperator,v3);
					}
				}
			}
			else if(pArray)
			{
				_INT32 i32Idx			= ((CArrayExpression*)pArray)->GetToken()->GetSymbolTableIndex();
				CVariable* pVariable	= pSymbolTable->GetAt(i32Idx);
				INTER_SAFEARRAY* prgsa	= pVariable->GetValue().GetValue().prgsa;

				_INT32 i32Type = prgsa->Type();
				if(i32Type == var.GetVarType())
				{
					// no probs here as the types are the same.
				}
				else
				{
					// we got to check whether the lhs is a int and the 
					//rhs is a float or the other way around
					//in which case our language permits us to 
					// do a downcast or an upcast.
					if((i32Type == RUL_INT) && var.GetVarType() == RUL_FLOAT)
					{
						//introduce a downcast node f2i.
						CCompoundExpression* pf2iNode = new CCompoundExpression(pExp,0,RUL_F2I);
						pAssStmt->m_pExpression = pf2iNode;

					}
					else if((i32Type == RUL_FLOAT) && var.GetVarType() == RUL_INT)
					{
						//introduce a upcast node i2f.
						CCompoundExpression* pi2fNode = new CCompoundExpression(pExp,0,RUL_I2F);
						pAssStmt->m_pExpression = pi2fNode;
					}
				}
			}		
		}
		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pAssStmt);
	}

	return TYPE_FAILURE;
}

_INT32 CTypeCheckVisitor::visitBreakStatement(
				CBreakStatement* pAssStmt,
				CSymbolTable* pSymbolTable,
				INTER_VARIANT* pvar,
				ERROR_VEC*	pvecErrors,
				RUL_TOKEN_SUBTYPE	AssignType)
{
	return VISIT_BREAK;
}

_INT32 CTypeCheckVisitor::visitReturnStatement(
				CReturnStatement* pAssStmt,
				CSymbolTable* pSymbolTable,
				INTER_VARIANT* pvar,
				ERROR_VEC*	pvecErrors,
				RUL_TOKEN_SUBTYPE	AssignType)
{
	return VISIT_RETURN;
}

_INT32 CTypeCheckVisitor::visitContinueStatement(
				CContinueStatement* pAssStmt,
				CSymbolTable* pSymbolTable,
				INTER_VARIANT* pvar,
				ERROR_VEC*	pvecErrors,
				RUL_TOKEN_SUBTYPE	AssignType)
{
	return VISIT_CONTINUE;
}

_INT32 CTypeCheckVisitor::visitIterationStatement(
			CIterationStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)

{
	CExpression* pExp=0;
	CStatement* pStmt=0;
	_INT32	i32Ret = TYPE_SUCCESS;
	try
	{
		if(pItnStmt)
		{
			pExp = pItnStmt->GetExpression();
			pStmt = pItnStmt->GetStatement();

			INTER_VARIANT var1;
			if(	pExp 
				&& (TYPE_FAILURE != (i32Ret = pExp->Execute(
								this,
								pSymbolTable,
								&var1,
								pvecErrors)) )
				&& pStmt)
			{
				i32Ret = pStmt->Execute(
							this,
							pSymbolTable,
							&var1,
							pvecErrors);
			}
		}

		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pItnStmt);
	}
	return TYPE_FAILURE;
}

_INT32 CTypeCheckVisitor::visitIterationStatement(
			CIterationDoWhileStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CExpression* pExp=0;
	CStatement* pStmt=0;
	_INT32	i32Ret = TYPE_SUCCESS;
	try
	{
		if(pItnStmt)
		{
			pExp = pItnStmt->GetExpression();
			pStmt = pItnStmt->GetStatement();

			INTER_VARIANT var1;
			if(	pExp 
				&& (TYPE_FAILURE != (i32Ret = pExp->Execute(
								this,
								pSymbolTable,
								&var1,
								pvecErrors)) )
				&& pStmt)
			{
				i32Ret = pStmt->Execute(
							this,
							pSymbolTable,
							&var1,
							pvecErrors);
			}
		}

		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pItnStmt);
	}
	return TYPE_FAILURE;
}

_INT32 CTypeCheckVisitor::visitIterationStatement(
			CIterationForStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CExpression* pExp=0;
	CStatement* pStmt=0;
	CStatement* pInitStmt = 0;
	CStatement* pIncrStmt=0;
	_INT32	i32Ret = TYPE_SUCCESS;
	try
	{
		if(pItnStmt)
		{
			pExp = pItnStmt->GetExpression();
			pStmt = pItnStmt->GetStatement();
			pInitStmt = pItnStmt->GetInitializationStatement();
			pIncrStmt = pItnStmt->GetIncrementStatement();

			INTER_VARIANT var1;
			if(	pExp 
				&& (TYPE_FAILURE != (i32Ret = pExp->Execute(
								this,
								pSymbolTable,
								&var1,
								pvecErrors)) )
				&& pStmt)
			{
				i32Ret = pStmt->Execute(
							this,
							pSymbolTable,
							&var1,
							pvecErrors);
			}
		}

		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pItnStmt);
	}
	return TYPE_FAILURE;
}

_INT32 CTypeCheckVisitor::visitSelectionStatement(
			CSelectionStatement* pSelStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CExpression* pExp=0;
	CStatement* pStmt=0;
	CELSEStatement* pElse=0;
	_INT32		i32Ret = TYPE_SUCCESS;

	try
	{
		if(pSelStmt)
		{
			pExp = pSelStmt->GetExpression();
			pStmt = pSelStmt->GetStatement();
			pElse = pSelStmt->GetELSEStatement();

			INTER_VARIANT var1;
			if(pExp 
				&& (TYPE_FAILURE != (i32Ret = pExp->Execute(
								this,
								pSymbolTable,
								&var1,
								pvecErrors)))
						&& pStmt)
			{
				i32Ret = pStmt->Execute(this,pSymbolTable,&var1,pvecErrors);
				var1.Clear();
				if(pElse)
				{
					_INT32 i32Temp=0;
					i32Temp = pElse->Execute(
								this,
								pSymbolTable,
								&var1,
								pvecErrors);
					i32Ret = (i32Temp == TYPE_FAILURE)?i32Temp:i32Ret;
				}
			}
		}

		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pSelStmt);
	}
	return TYPE_FAILURE;
}

_INT32 CTypeCheckVisitor::visitSwitchStatement(
			CSwitchStatement* pSelStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
/*	CExpression* pExp=0;
	CStatement* pStmt=0;
	CELSEStatement* pElse=0;
	_INT32		i32Ret = TYPE_SUCCESS;

	try
	{
		if(pSelStmt)
		{
			pExp = pSelStmt->GetExpression();
			pStmt = pSelStmt->GetStatement();
			pElse = pSelStmt->GetELSEStatement();

			INTER_VARIANT var1;
			if(pExp 
				&& (TYPE_FAILURE != (i32Ret = pExp->Execute(
								this,
								pSymbolTable,
								&var1,
								pvecErrors)))
						&& pStmt)
			{
				i32Ret = pStmt->Execute(this,pSymbolTable,&var1,pvecErrors);
				var1.Clear();
				if(pElse)
				{
					_INT32 i32Temp=0;
					i32Temp = pElse->Execute(
								this,
								pSymbolTable,
								&var1,
								pvecErrors);
					i32Ret = (i32Temp == TYPE_FAILURE)?i32Temp:i32Ret;
				}
			}
		}

		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pSelStmt);
	}
	return TYPE_FAILURE;*/
	return 0;
}

_INT32 CTypeCheckVisitor::visitELSEStatement(
			CELSEStatement* pELSE,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CStatement* pStmt=0;
	INTER_VARIANT var;
	_INT32	i32Ret = TYPE_SUCCESS;
	try
	{
		if(pELSE)
		{
			pStmt = pELSE->GetStatement();
			if(pStmt)
			{
				i32Ret = pStmt->Execute(this,pSymbolTable,&var,pvecErrors);
			}
		}
		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pELSE);
	}
	return TYPE_FAILURE;
}

_INT32 CTypeCheckVisitor::visitCASEStatement(
			CCASEStatement* pELSE,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CStatementList* pStmt=0;
	INTER_VARIANT var;
	_INT32	i32Ret = TYPE_SUCCESS;
	try
	{
		if(pELSE)
		{
			pStmt = pELSE->GetStatement();
			if(pStmt)
			{
				i32Ret = pStmt->Execute(this,pSymbolTable,&var,pvecErrors);
			}
		}
		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pELSE);
	}
	return TYPE_FAILURE;
}

#endif

_INT32 CTypeCheckVisitor::tc_uplus(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_uminus(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_add(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_sub(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::tc_mul(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_div(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_mod(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_exp(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_neq(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::tc_eq(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::tc_lt(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::tc_gt(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_ge(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_le(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_land(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_lor(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}
_INT32 CTypeCheckVisitor::tc_lnot(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::tc_rparen(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::visitCompoundExpression(
			CCompoundExpression* pCompStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)

{
	CExpression*		pFirstExp = 0;
	CExpression*		pSecondExp = 0;
	RUL_TOKEN_SUBTYPE	Operator = RUL_SUBTYPE_NONE;
	INTER_VARIANT	var1;
	INTER_VARIANT	var2;
	_INT32	i32Ret=TYPE_SUCCESS;

	try
	{
		if(pCompStmt)
		{
			pFirstExp = pCompStmt->GetFirstExpression();
			pSecondExp = pCompStmt->GetSecondExpression();
			Operator = pCompStmt->GetOperator();

			if(pFirstExp)
			{
				i32Ret = pFirstExp->Execute(
					this,
					pSymbolTable,
					&var1,
					pvecErrors);
			}
			if(pSecondExp)
			{
				_INT32 i32Temp = 0;
				i32Temp = pSecondExp->Execute(
					this,
					pSymbolTable,
					&var2,
					pvecErrors);
				i32Ret = (i32Temp == TYPE_FAILURE)?i32Temp:i32Ret;
			}

			if(pvar)
			{
				pvar->Clear();
			}

			TypeCast(
				pCompStmt,
				var1,
				var2,
				*pvar);
		}

		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pCompStmt);
	}
	return TYPE_FAILURE;
}

extern RUL_TOKEN_SUBTYPE GetSubType(
			CToken* pToken);

_INT32 CTypeCheckVisitor::visitPrimaryExpression(
			CPrimaryExpression* pPrimStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CToken* pToken=0;
	try
	{
//#ifdef _FULL_RULE_ENGINE
//		if(pPrimStmt)
//		{
//			pToken = pPrimStmt->GetToken();
//			if(pToken)
//			{
//				RUL_TOKEN_SUBTYPE SubType = pToken->GetSubType();
//				*pvar = m_TypeMapper[SubType];
//			}
//		}
//		return TYPE_SUCCESS;
//#else
		if(pPrimStmt)
		{
			pToken = pPrimStmt->GetToken();
			if(pToken)
			{
				RUL_TOKEN_SUBTYPE SubType;
#ifdef MODIFIED_BY_SS
				//if(!pToken->IsNumeric())
				//{
				//	SubType = GetSubType(pToken);
				//}
				//else
				//{
				//	SubType = pToken->GetSubType();
				//}
				SubType = pToken->GetSubType();
#endif
				*pvar = m_TypeMapper[SubType];
			}
		}
		return TYPE_SUCCESS;
//#endif
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pPrimStmt);
	}
	return TYPE_FAILURE;
}

#ifdef _FULL_RULE_ENGINE

_INT32 CTypeCheckVisitor::visitProgram(
			CProgram* pProgram,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CDeclarations* pDecl = 0;
	CStatementList* pStmtList = 0;
	_INT32 i32Ret = TYPE_SUCCESS;

	try
	{
		if(pProgram)
		{
			pDecl=pProgram->GetDeclarations();
			pStmtList=pProgram->GetStatementList();

			if(pDecl)
			{
				i32Ret = pDecl->Execute(this,pSymbolTable,pvar,pvecErrors);
			}
			if(pStmtList)
			{
				_INT32 i32Temp;
				i32Temp = pStmtList->Execute(this,pSymbolTable,pvar,pvecErrors);
				i32Ret = (i32Temp == TYPE_FAILURE)?i32Temp:i32Ret;
			}
		}
		return i32Ret;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR,pProgram);
	}
	return TYPE_FAILURE;
}

_INT32 CTypeCheckVisitor::visitCompoundStatement(
			CCompoundStatement* pCompStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CStatementList* pStmtLst=0;
	if(pCompStmt)
	{
		pStmtLst = pCompStmt->GetStatementList();
	}
	return visitStatementList(pStmtLst,pSymbolTable,pvar,pvecErrors);
}

#endif

_INT32 CTypeCheckVisitor::visitExpression(
			CExpression* pStatement,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	return TYPE_SUCCESS;
}

#ifdef _FULL_RULE_ENGINE

_INT32 CTypeCheckVisitor::visitStatement(
			CStatement* pStatement,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::visitStatementList(
			CStatementList* pStmtList,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	STATEMENT_LIST* pStmtCol=0;
	CStatement* pStmt=0;
	_INT32 i32Ret = TYPE_SUCCESS;

	if(pStmtList)
	{
		pStmtCol = pStmtList->GetStmtList();
		_INT32 nSize = pStmtCol->size();
		for(_INT32 i=0;i<nSize;i++)
		{
			pStmt = (*pStmtCol)[i];
			_INT32 i32Temp = TYPE_FAILURE;
			i32Temp = pStmt->Execute(
						this,
						pSymbolTable,
						pvar,
						pvecErrors);
			i32Ret = (i32Temp == TYPE_FAILURE)?i32Temp:i32Ret;
		}
	}
	return i32Ret;
}

_INT32 CTypeCheckVisitor::visitDeclarations(
			CDeclarations* pStatement,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::visitRuleService(
			CRuleServiceStatement* pStatement,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	return TYPE_SUCCESS;
}

#endif

_INT32 CTypeCheckVisitor::TypeCast(
			CCompoundExpression* pexpr,
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	RUL_TOKEN_SUBTYPE outOperator;
	RUL_TOKEN_SUBTYPE castOperator;
	_INT32 i32Operand=0;
	outOperator = pexpr->GetOperator();
	if(needCastNode(
			pexpr,
			v1,
			v2,
			outOperator,
			i32Operand,
			castOperator,
			v3))
	{
		CExpression* pTypeExpr=0;
		if(i32Operand == 1)
		{
			pTypeExpr = pexpr->GetFirstExpression();
		}
		else if(i32Operand == 2)
		{
			pTypeExpr = pexpr->GetSecondExpression();
		}
		CCompoundExpression* pnewNode = new CCompoundExpression(
												pTypeExpr,
												0,
												castOperator);
		if(i32Operand == 1)
		{
			pexpr->m_pFirstExp = pnewNode;
		}
		else if(i32Operand == 2)
		{
			pexpr->m_pSecondExp = pnewNode;
		}
	}
	if(pexpr->GetOperator() != outOperator)
	{
		pexpr->m_Operator = outOperator;
	}
	return TYPE_SUCCESS;
}

#ifdef _FULL_RULE_ENGINE

_INT32 CTypeCheckVisitor::visitOMExpression(
			COMServiceExpression* pExpression,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	return TYPE_SUCCESS;
}


_INT32 CTypeCheckVisitor::visitFunctionExpression(
			FunctionExpression* pExpression,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	return TYPE_SUCCESS;
}

_INT32 CTypeCheckVisitor::visitIFExpression(
			IFExpression* pExpression,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	return TYPE_SUCCESS;
}

#endif


bool CTypeCheckVisitor::needCastNode(
		CGrammarNode* pexpr,
		INTER_VARIANT& v1,
		INTER_VARIANT& v2,
		RUL_TOKEN_SUBTYPE& outOperator,
		_INT32&			   i32Operand,
		RUL_TOKEN_SUBTYPE& castOperator,
		INTER_VARIANT& v3)
{
#ifdef _FULL_RULE_ENGINE

	bool bShouldTypeCast = true;
	bool bNeedTypeNode = false;
	if(v1.GetVarType() == RUL_INT && v2.GetVarType() == RUL_INT)
	{
		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
		switch(Operator)
		{
		case RUL_UPLUS:
		case RUL_UMINUS:
		case RUL_PLUS:
			break;
		case RUL_MINUS:
			break;
		case RUL_MUL: 
			break;
		case RUL_DIV: 
			break;
		case RUL_MOD:
			break;
		case RUL_LT:
			bShouldTypeCast = false;
			break;
		case RUL_GT:
			bShouldTypeCast = false;
			break;
		case RUL_EQ:
			bShouldTypeCast = false;
			break;
		case RUL_NOT_EQ:
			bShouldTypeCast = false;
			break;
		case RUL_LE:
			bShouldTypeCast = false;
			break;
		case RUL_GE:
			bShouldTypeCast = false;
			break;
		case RUL_LOGIC_AND:
			bShouldTypeCast = false;
			break;
		case RUL_LOGIC_OR:
			bShouldTypeCast = false;
			break;
		default:
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
		}
		if(bShouldTypeCast)
		{
			v3 = (_INT32)0;// a long is not an int
		}
		else
		{
			v3 = (_INT32)0;// a long is not an int 
		}
	}
	else if(v1.GetVarType() == RUL_FLOAT && v2.GetVarType() == RUL_INT)
	{
		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
		RUL_TOKEN_SUBTYPE Op1;
		switch(Operator)
		{
		case RUL_UPLUS:
		case RUL_UMINUS:
		case RUL_PLUS:
			Op1 = RUL_FPLUS;
			break;
		case RUL_MINUS:
			Op1 = RUL_FMINUS;
			break;
		case RUL_MUL: 
			Op1 = RUL_FMUL;
			break;
		case RUL_DIV: 
			Op1 = RUL_FDIV;
			break;
		case RUL_MOD:
			Op1 = RUL_FMOD;
			break;
		case RUL_NOT_EQ:
			Op1 = RUL_NOT_FEQ;
			bShouldTypeCast = false;
			break;
		case RUL_LT:
			Op1 = RUL_FLT;
			bShouldTypeCast = false;
			break;
		case RUL_GT:
			Op1 = RUL_FGT;
			bShouldTypeCast = false;
			break;
		case RUL_EQ:
			Op1 = RUL_FEQ;
			bShouldTypeCast = false;
			break;
		case RUL_LE:
			Op1 = RUL_FLE;
			bShouldTypeCast = false;
			break;
		case RUL_GE:
			Op1 = RUL_FGE;
			bShouldTypeCast = false;
			break;
		case RUL_LOGIC_AND:
			bShouldTypeCast = false;
			break;
		case RUL_LOGIC_OR:
			bShouldTypeCast = false;
			break;
		default:
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
		}
		//Introduce a i2f node
		outOperator = Op1;
		if(bShouldTypeCast)
		{
			bNeedTypeNode = true;
			castOperator = RUL_I2F;
			i32Operand = 2;
			v3 = (float)0.0;
		}
		else
		{
			v3 = (_INT32)0;// a long is not an int
		}
	}
	else if(v1.GetVarType() == RUL_INT && v2.GetVarType() == RUL_FLOAT)
	{
		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
		RUL_TOKEN_SUBTYPE Op1;
		switch(Operator)
		{
		case RUL_UPLUS:
		case RUL_UMINUS:
		case RUL_PLUS:
			Op1 = RUL_FPLUS;
			break;
		case RUL_MINUS:
			Op1 = RUL_FMINUS;
			break;
		case RUL_MUL: 
			Op1 = RUL_FMUL;
			break;
		case RUL_DIV: 
			Op1 = RUL_FDIV;
			break;
		case RUL_MOD:
			Op1 = RUL_FMOD;
			break;
		case RUL_NOT_EQ:
			Op1 = RUL_NOT_FEQ;
			bShouldTypeCast = false;
			break;
		case RUL_LT:
			Op1 = RUL_FLT;
			bShouldTypeCast = false;
			break;
		case RUL_GT:
			Op1 = RUL_FGT;
			bShouldTypeCast = false;
			break;
		case RUL_EQ:
			Op1 = RUL_FEQ;
			bShouldTypeCast = false;
			break;
		case RUL_LE:
			Op1 = RUL_FLE;
			bShouldTypeCast = false;
			break;
		case RUL_GE:
			Op1 = RUL_FGE;
			bShouldTypeCast = false;
			break;
		case RUL_LOGIC_AND:
			bShouldTypeCast = false;
			break;
		case RUL_LOGIC_OR:
			bShouldTypeCast = false;
			break;
		default:
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
		}	
			//Introduce a i2f node
		outOperator = Op1;
		if(bShouldTypeCast)
		{
			bNeedTypeNode = true;
			castOperator = RUL_I2F;
			i32Operand = 1;
			v3 = (float)0.0;
		}
		else
		{
			v3 = (_INT32)0;// a long is not an int
		}
	}
	else if(v1.GetVarType() == RUL_FLOAT && v2.GetVarType() == RUL_FLOAT)
	{
		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
		RUL_TOKEN_SUBTYPE Op1;
		switch(Operator)
		{
		case RUL_UPLUS:
		case RUL_UMINUS:
		case RUL_PLUS:
			Op1 = RUL_FPLUS;
			break;
		case RUL_MINUS:
			Op1 = RUL_FMINUS;
			break;
		case RUL_MUL: 
			Op1 = RUL_FMUL;
			break;
		case RUL_DIV: 
			Op1 = RUL_FDIV;
			break;
		case RUL_MOD:
			Op1 = RUL_FMOD;
			break;
		case RUL_NOT_EQ:
			Op1 = RUL_NOT_FEQ;
			bShouldTypeCast = false;
			break;
		case RUL_LT:
			Op1 = RUL_FLT;
			bShouldTypeCast = false;
			break;
		case RUL_GT:
			Op1 = RUL_FGT;
			bShouldTypeCast = false;
			break;
		case RUL_EQ:
			Op1 = RUL_FEQ;
			bShouldTypeCast = false;
			break;
		case RUL_LE:
			Op1 = RUL_FLE;
			bShouldTypeCast = false;
			break;
		case RUL_GE:
			Op1 = RUL_FGE;
			bShouldTypeCast = false;
			break;
		case RUL_LOGIC_AND:
			bShouldTypeCast = false;
			break;
		case RUL_LOGIC_OR:
			bShouldTypeCast = false;
			break;
		default:
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
		}	
		outOperator = Op1;
		if(bShouldTypeCast)
		{
			v3=(float)0.0;
		}
		else
		{
			v3=(_INT32)0;// a long is not an int
		}
	}
	else if(v1.GetVarType() == RUL_SAFEARRAY || v2.GetVarType() == RUL_SAFEARRAY)
	{
		throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
	}
	else if(v1.GetVarType() == RUL_CHARPTR && v2.GetVarType() == RUL_CHARPTR)
	{
		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
	 	RUL_TOKEN_SUBTYPE Op1;
		switch(Operator)
		{
		case RUL_UPLUS:
		case RUL_UMINUS:
		case RUL_PLUS:
			Op1 = RUL_SPLUS;
			break;
		case RUL_MINUS:
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
			break;
		case RUL_MUL: 
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
			break;
		case RUL_DIV: 
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
			break;
		case RUL_MOD:
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
			break;
		case RUL_NOT_EQ:
			Op1 = RUL_NOT_SEQ;
			break;
		case RUL_LT:
			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
			bShouldTypeCast = false;
			break;
		case RUL_GT:
			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
			bShouldTypeCast = false;
			break;
		case RUL_EQ:
			Op1 = RUL_SEQ;
			bShouldTypeCast = false;
			break;
		case RUL_LE:
			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
			bShouldTypeCast = false;
			break;
		case RUL_GE:
			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
			bShouldTypeCast = false;
			break;
		default:
			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
		}
		outOperator = Op1;
		if(bShouldTypeCast)
		{		
			v3 = (char *)"";
		}
		else
		{
			v3=(_INT32)0;// a long is not an int
		}
	
	}
	else if((outOperator == RUL_RPAREN) && (v2.GetVarType() == RUL_NULL))
	{
		switch( v1.GetVarType() )
		{
			case RUL_CHAR:
				v3 = (char)' ';
				break;
			case RUL_INT:
				v3 = (_INT32)0;// a long is not an int
				break;
			case RUL_BOOL:
				v3 = (bool)false;
				break;
			case RUL_FLOAT:
				v3 = (float)0.0;//WS:EPM 10aug07
				break;
			case RUL_DOUBLE:
				v3 = (double)0.0;//WS:EPM 10aug07
				break;
			case RUL_CHARPTR:
			case RUL_SAFEARRAY:
			case RUL_DD_STRING:
			case RUL_UNSIGNED_CHAR:
				v3 = (char *)"";
				break;
		}
	}
	else
	{
		throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
	}

//#else
//#ifdef MODIFIED_BY_SS
//	if(pexpr->m_Operator == RUL_RPAREN)
//	{
//		v3.varType = v1.varType;
//	}
//	if(v1.varType == RUL_INT && v2.varType == RUL_INT)
//	{
//		v3.varType = RUL_INT;
//	}
//	else if(v1.varType == RUL_FLOAT && v2.varType == RUL_INT)
//	{
//		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
//		RUL_TOKEN_SUBTYPE Op1;
//		switch(Operator)
//		{
//		case RUL_UPLUS:
//		case RUL_UMINUS:
//		case RUL_PLUS:
//			Op1 = RUL_FPLUS;
//			break;
//		case RUL_MINUS:
//			Op1 = RUL_FMINUS;
//			break;
//		case RUL_MUL: 
//			Op1 = RUL_FMUL;
//			break;
//		case RUL_DIV: 
//			Op1 = RUL_FDIV;
//			break;
//		case RUL_MOD:
//			Op1 = RUL_FMOD;
//			break;
//		case RUL_NOT_EQ:
//			Op1 = RUL_NOT_FEQ;
//			bShouldTypeCast = false;
//			break;
//		case RUL_LT:
//			Op1 = RUL_FLT;
//			break;
//		case RUL_GT:
//			Op1 = RUL_FGT;
//			break;
//		case RUL_EQ:
//			Op1 = RUL_FEQ;
//			break;
//		case RUL_LE:
//			Op1 = RUL_FLE;
//			break;
//		case RUL_GE:
//			Op1 = RUL_FGE;
//			break;
//		case RUL_LOGIC_AND:
//			bShouldTypeCast = false;
//			break;
//		case RUL_LOGIC_OR:
//			bShouldTypeCast = false;
//			break;
//		default:
//			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//		}
//		//Introduce a i2f node
//		CCompoundExpression* pi2fNode = new CCompoundExpression(pexpr->GetSecondExpression(),0,RUL_I2F);
//		pexpr->m_pSecondExp = pi2fNode;
//		pexpr->m_Operator = Op1;
//		v3.varType = RUL_FLOAT;
//		
//	}
//	else if(v1.varType == RUL_INT && v2.varType == RUL_FLOAT)
//	{
//		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
//		RUL_TOKEN_SUBTYPE Op1;
//		switch(Operator)
//		{
//		case RUL_UPLUS:
//		case RUL_UMINUS:
//		case RUL_PLUS:
//			Op1 = RUL_FPLUS;
//			break;
//		case RUL_MINUS:
//			Op1 = RUL_FMINUS;
//			break;
//		case RUL_MUL: 
//			Op1 = RUL_FMUL;
//			break;
//		case RUL_DIV: 
//			Op1 = RUL_FDIV;
//			break;
//		case RUL_MOD:
//			Op1 = RUL_FMOD;
//			break;
//		case RUL_NOT_EQ:
//			Op1 = RUL_NOT_FEQ;
//			bShouldTypeCast = false;
//			break;
//		case RUL_LT:
//			Op1 = RUL_FLT;
//			break;
//		case RUL_GT:
//			Op1 = RUL_FGT;
//			break;
//		case RUL_EQ:
//			Op1 = RUL_FEQ;
//			break;
//		case RUL_LE:
//			Op1 = RUL_FLE;
//			break;
//		case RUL_GE:
//			Op1 = RUL_FGE;
//			break;
//		case RUL_LOGIC_AND:
//			bShouldTypeCast = false;
//			break;
//		case RUL_LOGIC_OR:
//			bShouldTypeCast = false;
//			break;
//		default:
//			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//		}	
//			//Introduce a i2f node
//		CCompoundExpression* pi2fNode = new CCompoundExpression(pexpr->GetFirstExpression(),0,RUL_I2F);
//		pexpr->m_pFirstExp = pi2fNode;
//		pexpr->m_Operator = Op1;
//		v3.varType = RUL_FLOAT;
//	}
//	else if(v1.varType == RUL_FLOAT && v2.varType == RUL_FLOAT)
//	{
//		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
//		RUL_TOKEN_SUBTYPE Op1;
//		switch(Operator)
//		{
//		case RUL_UPLUS:
//		case RUL_UMINUS:
//		case RUL_PLUS:
//			Op1 = RUL_FPLUS;
//			break;
//		case RUL_MINUS:
//			Op1 = RUL_FMINUS;
//			break;
//		case RUL_MUL: 
//			Op1 = RUL_FMUL;
//			break;
//		case RUL_DIV: 
//			Op1 = RUL_FDIV;
//			break;
//		case RUL_MOD:
//			Op1 = RUL_FMOD;
//			break;
//		case RUL_NOT_EQ:
//			Op1 = RUL_NOT_FEQ;
//			bShouldTypeCast = false;
//			break;
//		case RUL_LT:
//			Op1 = RUL_FLT;
//			break;
//		case RUL_GT:
//			Op1 = RUL_FGT;
//			break;
//		case RUL_EQ:
//			Op1 = RUL_FEQ;
//			break;
//		case RUL_LE:
//			Op1 = RUL_FLE;
//			break;
//		case RUL_GE:
//			Op1 = RUL_FGE;
//			break;
//		case RUL_LOGIC_AND:
//			bShouldTypeCast = false;
//			break;
//		case RUL_LOGIC_OR:
//			bShouldTypeCast = false;
//			break;
//		default:
//			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//		}	
//		pexpr->m_Operator = Op1;
//		v3.varType = RUL_FLOAT;
//	}
//	else if(v1.varType == RUL_SAFEARRAY || v2.varType == RUL_SAFEARRAY)
//	{
//		throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//	}
//	else if(v1.varType == RUL_CHARPTR || v2.varType == RUL_CHARPTR)
//	{
//		RUL_TOKEN_SUBTYPE Operator = outOperator;//pexpr->GetOperator();
//		RUL_TOKEN_SUBTYPE Op1;
//		switch(Operator)
//		{
//		case RUL_UPLUS:
//		case RUL_UMINUS:
//		case RUL_PLUS:
//			Op1 = RUL_SPLUS;
//			break;
//		case RUL_MINUS:
//			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//			break;
//		case RUL_MUL: 
//			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//			break;
//		case RUL_DIV: 
//			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//			break;
//		case RUL_MOD:
//			throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//			break;
//		case RUL_NOT_EQ:
//			Op1 = RUL_NOT_SEQ;
//			break;
//		case RUL_LT:
//			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
//			break;
//		case RUL_GT:
//			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
//			break;
//		case RUL_EQ:
//			Op1 = RUL_SEQ;
//			break;
//		case RUL_LE:
//			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
//			break;
//		case RUL_GE:
//			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
//			break;
//		default:
//			throw(C_TC_ERROR_TYP_NOTIMPL,pexpr);
//		}	
//		pexpr->m_Operator = Op1;
//		v3.varType = RUL_CHARPTR;
//	}
//	else
//	{
//		throw(C_TC_ERROR_TYP_MISMATCH,pexpr);
//	}
//#endif
#endif

	return 	false;
}