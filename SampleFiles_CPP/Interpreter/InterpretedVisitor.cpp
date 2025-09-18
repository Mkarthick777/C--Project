//#include "stdafx.h"
#pragma warning (disable : 4786)
#include "pch.h"
#include "InterpretedVisitor.h"
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
#include "RuleServiceStatement.h"
#include "OMServiceExpression.h"
#include "FunctionExpression.h"
#include "IFExpression.h"

#include "HART_Builtins.h"
#include "MEE.h" //Vibhor 070705: Added
//Anil August 26 2005 
#ifndef RETURNCODE
/* error codes */
   #define RETURNCODE  int

   #define SUCCESS			(0)
   #define FAILURE			(1)
#endif

/* Define for the maximum number of loops that can be run */
#define MAX_LOOPS	USHRT_MAX
typedef vector<INTER_VARIANT> INTERVARIANT_VECTOR;

CInterpretedVisitor::CInterpretedVisitor()
{
	m_IsLValue = false;
	// memset(m_fnTable,0,BYTE);//WS - 21may07 - wrong size
	memset(m_fnTable,0,MAX_BYTE*sizeof(PFN_INTERPRETER));// right size
		// WS - 9apr07 - 2005 checkin - fully define functions
	m_fnTable[RUL_PLUS_PLUS]		= &CInterpretedVisitor::uplusplus;
	m_fnTable[RUL_MINUS_MINUS]		= &CInterpretedVisitor::uminusminus;
	m_fnTable[RUL_PRE_PLUS_PLUS]	= &CInterpretedVisitor::upreplusplus;
	m_fnTable[RUL_PRE_MINUS_MINUS]	= &CInterpretedVisitor::upreminusminus;
	m_fnTable[RUL_UPLUS]			= &CInterpretedVisitor::uplus;
	m_fnTable[RUL_UMINUS]			= &CInterpretedVisitor::uminus;
	m_fnTable[RUL_BIT_AND]			= &CInterpretedVisitor::bit_and;
	m_fnTable[RUL_BIT_OR]			= &CInterpretedVisitor::bit_or;
	m_fnTable[RUL_BIT_XOR]			= &CInterpretedVisitor::bitxor;
	m_fnTable[RUL_BIT_NOT]			= &CInterpretedVisitor::bitnot;
	m_fnTable[RUL_BIT_RSHIFT]		= &CInterpretedVisitor::bitrshift;
	m_fnTable[RUL_BIT_LSHIFT]		= &CInterpretedVisitor::bitlshift;
	m_fnTable[RUL_PLUS]				= &CInterpretedVisitor::add;
	m_fnTable[RUL_MINUS]			= &CInterpretedVisitor::sub;
	m_fnTable[RUL_MUL]				= &CInterpretedVisitor::mul;
	m_fnTable[RUL_DIV]				= &CInterpretedVisitor::div;
	m_fnTable[RUL_MOD]				= &CInterpretedVisitor::mod;
	m_fnTable[RUL_EXP]				= &CInterpretedVisitor::exp;
	m_fnTable[RUL_NOT_EQ]			= &CInterpretedVisitor::neq;
	m_fnTable[RUL_LT]				= &CInterpretedVisitor::lt;
	m_fnTable[RUL_GT]				= &CInterpretedVisitor::gt;
	m_fnTable[RUL_EQ]				= &CInterpretedVisitor::eq;
	m_fnTable[RUL_GE]				= &CInterpretedVisitor::ge;
	m_fnTable[RUL_LE]				= &CInterpretedVisitor::le;
	m_fnTable[RUL_LOGIC_AND]		= &CInterpretedVisitor::land;
	m_fnTable[RUL_LOGIC_OR]			= &CInterpretedVisitor::lor;
	m_fnTable[RUL_LOGIC_NOT]		= &CInterpretedVisitor::lnot;
	m_fnTable[RUL_RPAREN]			= &CInterpretedVisitor::rparen;

	m_fnTable[RUL_ASSIGN]			= &CInterpretedVisitor::assign;
	m_fnTable[RUL_PLUS_ASSIGN]		= &CInterpretedVisitor::plusassign;
	m_fnTable[RUL_MINUS_ASSIGN]		= &CInterpretedVisitor::minusassign;
	m_fnTable[RUL_DIV_ASSIGN]		= &CInterpretedVisitor::divassign;
	m_fnTable[RUL_MOD_ASSIGN]		= &CInterpretedVisitor::modassign;
	m_fnTable[RUL_MUL_ASSIGN]		= &CInterpretedVisitor::mulassign;

	m_fnTable[RUL_BIT_AND_ASSIGN]	= &CInterpretedVisitor::bitandassign;
	m_fnTable[RUL_BIT_OR_ASSIGN]	= &CInterpretedVisitor::bitorassign;
	m_fnTable[RUL_BIT_XOR_ASSIGN]	= &CInterpretedVisitor::bitxorassign;
	m_fnTable[RUL_BIT_RSHIFT_ASSIGN]	= &CInterpretedVisitor::rshiftassign;
	m_fnTable[RUL_BIT_LSHIFT_ASSIGN]	= &CInterpretedVisitor::lshiftassign;
}

CInterpretedVisitor::~CInterpretedVisitor()
{

}

_INT32 CInterpretedVisitor::visitArrayExpression(
			CArrayExpression* pArrExp,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	assert(pvar!=0);

	_INT32 i32Idx			= pArrExp->GetToken()->GetSymbolTableIndex();
	CVariable* pVariable	= pSymbolTable->GetAt(i32Idx);
	INTER_SAFEARRAY* prgsa	= pVariable->GetValue().GetValue().prgsa;

	//evaluate the expressions...
	EXPR_VECTOR* pvecExpressions = pArrExp->GetExpressions();
	_INT32 i32Count = pvecExpressions->size();
	INTER_VARIANT var;
	vector<_INT32> vecDims;
	

	prgsa->GetDims(&vecDims);
	assert( vecDims.size() == i32Count );// added 18feb08
	_INT32 i32mem = prgsa->MemoryAllocated();
	_INT32 i32loc=0;

	for(_INT32 i=0;i<i32Count;i++)
	{
		if( (_INT32)vecDims.size() >= i+1 )// WS:EPM 17jul07
		{
			i32mem = i32mem/vecDims[i];
		}
		var.Clear();
		((*pvecExpressions)[i])->Execute(this,pSymbolTable,&var);
		i32loc = i32loc + i32mem* (int)var;
	}

	if(m_IsLValue)
	{
		m_IsLValue = false;
		//lvalue
		prgsa->SetElement(i32loc,pvar);
	}
	else
	{
		//rvalue
		pvar->Clear();
		prgsa->GetElement(i32loc,pvar);
	}
	return VISIT_NORMAL;
}

//Execute the expression
//Assign the value from the expression to the variable in the Symbol Table.
_INT32 CInterpretedVisitor::visitAssignment(
			CAssignmentStatement* pAssStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CToken* pVariable = 0;
	CExpression* pExp = 0;
	CExpression* pArray=0;
	CExpression* pComplexDDExp=0;//Added By Anil August 23 2005
	_INT32 iRetValue = VISIT_NORMAL; 

	if(pAssStmt)
	{
		pVariable = pAssStmt->GetVariable();
		pExp	= pAssStmt->GetExpression();
		pArray = pAssStmt->GetArrayExp();
		pComplexDDExp = pAssStmt->GetComplexDDExp();//Added By Anil August 23 2005
	}
	else
	{
		//error -- no assign statement
		return VISIT_ERROR;
	}

	INTER_VARIANT var;

/*Vibhor 110205: Start of Code*/
	if(pVariable)
	{
		switch(pVariable->GetSubType())
		{
			case RUL_CHAR_DECL:
				{
		//			var = (char)' ';  //WS:EPM 10aug07::Leave this as RUL_NULL because it could be a RUL_CHAR or a SafeArray of RUL_CHAR's
				}
				break;
			case RUL_LONG_LONG_DECL:
				{
					var = (INT64)0;
				}
				break;
			// Walt EPM 08sep08 - start insert
			case RUL_SHORT_INTEGER_DECL:
				{
					var = (short)0;
				}
				break;
			case RUL_UNSIGNED_SHORT_INTEGER_DECL:
				{
					var = (wchar_t)0;
				}
				break;
			case RUL_UNSIGNED_INTEGER_DECL:
				{
					var = (unsigned int)0;
				}
				break;
			// Walt EPM 08sep08 - end insert
			case RUL_INTEGER_DECL:
			case RUL_LONG_DECL:
				{
				//	var = (long)0;// you don't have any operations on a long (only in windows is a long the same as an int)
				    var = (long)0;
				}
				break;
			case RUL_REAL_DECL:
				{
					var = (float)0.0;
				}
				break;
			case RUL_DOUBLE_DECL:
				{
					var = (double)0.0;//WS:EPM 10aug07
				}
				break;
			case RUL_BOOLEAN_DECL:
				{
					var = (bool)false;
				}
				break;
			case RUL_STRING_DECL:
				{
					var = (char *)"";
				}
				break;
			case RUL_ARRAY_DECL:
				{
					var = (char *)"";
				}
				break;
			//Added By Anil June 15 2005 --starts here
			case RUL_DD_STRING_DECL :
				{
					var = (char *)"";
				}
				break;
			//Added By Anil June 15 2005 --Ends here
			case RUL_UNSIGNED_CHAR_DECL :
				{
					//var = (char *)"";  //WS:EPM 10aug07::Leave this as RUL_NULL because it could be a RUL_UNSIGNED_CHAR or a SafeArray of RUL_UNSIGNED_CHAR's
				}
				break;
		


			default:
				break; //var.varType == RUL_NULL;
		} 
	}
/*Vibhor 110205: End of Code*/
	if(pExp)
	{
		iRetValue = pExp->Execute(this,pSymbolTable,&var);

		LOGIT(CLOG_LOG,"interpreted visitor, visitAssignment, iRetValue = %d",iRetValue);


		if (iRetValue == VISIT_RETURN || iRetValue ==  VISIT_ERROR)// sded error checkin april2013
		{
			return iRetValue;
		}
	}
	else
	{
		//error -- no expression in assingment statement
		return VISIT_ERROR;
	}

	if(pVariable)
	{
		_INT32 nIdx = -1;
		CVariable* pStore=0;

		nIdx = pVariable->GetSymbolTableIndex();
// commented out code removed 17jul07 
		pStore = pSymbolTable->GetAt(nIdx);
		if( pStore )
		{
			switch (pAssStmt->GetAssignmentType())
			{
				case RUL_ASSIGN:
					{
						pStore->GetValue() = var;
						break;
					}
				case RUL_PLUS_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() + var;
						break;
					}
				case RUL_MINUS_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() - var;
						break;
					}
				case RUL_DIV_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() / var;
						break;
					}
				case RUL_MUL_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() * var;
						break;
					}
				case RUL_MOD_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() % var;					
						break;
					}			
				case RUL_BIT_AND_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() & var;					
						break;
					}
				case RUL_BIT_OR_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() | var;					
						break;
					}
				case RUL_BIT_XOR_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() ^ var;					
						break;
					}
				case RUL_BIT_RSHIFT_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() >> var;					
						break;
					}
				case RUL_BIT_LSHIFT_ASSIGN:
					{
						pStore->GetValue() = pStore->GetValue() << var;					
						break;
					}
			
			}//end switch
		}//end of if statement
		// else - NULL pStore - error?? logit???

		// Set the display Value of the variable if its a Global (DD Variable)
		//Anil August 26 2005 Comented By Anil as DD item is No more Variable
	/*	if(pVariable->m_bIsGlobal)
		{
			switch(pStore->GetSubType())
			{
				case	RUL_INTEGER_DECL:
						m_pMEE->SetVariableValue(pStore->GetLexeme(),(int)(pStore->GetValue()));
						break;
				case	RUL_REAL_DECL:
						m_pMEE->SetVariableValue(pStore->GetLexeme(),(float)(pStore->GetValue()));
						break;
				case	RUL_DOUBLE_DECL:
						m_pMEE->SetVariableValue(pStore->GetLexeme(),(double)(pStore->GetValue()));
						break;
				case	RUL_STRING_DECL:
						m_pMEE->SetVariableValue(pStore->GetLexeme(),(char*)(pStore->GetValue()));
						break;
				case	RUL_BOOLEAN_DECL:
						m_pMEE->SetVariableValue(pStore->GetLexeme(),(bool)(pStore->GetValue()));
						break;
		}

		}*/
	}//endif pVariable
	else if(pArray)
	{
		m_IsLValue = true;
		pArray->Execute(this,pSymbolTable,&var,0,pAssStmt->GetAssignmentType()); //Anil August 26 2005 //for Fixing a[10] += 5;
	}
	//Added By Anil August 23 2005 --starts here 
	//This below is handle if it is of type	ComplexDD expression
	else if(pComplexDDExp)
	{
		m_IsLValue = true;		
		_INT32 iTemp = pComplexDDExp->Execute(this,pSymbolTable,&var,0,pAssStmt->GetAssignmentType());
		if( iTemp == VISIT_ERROR)
			return VISIT_ERROR;
	}
	//Added By Anil August 23 2005 --Ends here
	else
	{
		//error -- no variable in assingment statement
		return VISIT_ERROR;
	}
	if (pvar)
		*pvar = var;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitBreakStatement(
			CBreakStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return VISIT_BREAK;
}

_INT32 CInterpretedVisitor::visitReturnStatement(
			CReturnStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	//Added:Anil Octobet 5 2005 for handling Method Calling Method
	//this is a Double check that Return statement is only from the Called method..
	//ie  method which does not sit on the Menu
	if(false == m_bIsRoutine)
	{
		//If so return ; ie Do not execute the return statement
	return VISIT_RETURN;
	}
	CExpression* pExp = 0;
	pExp	= pItnStmt->GetExpression();
	//if pExp, then it is return Void statement ..so no need to Execute this
	if(NULL == pExp)
	{
		return VISIT_RETURN;//This is the case of Void return statement ie return;

	}
	//Other wise we may need to Execute this
	else
	{
		int iSizeSymbTa = pSymbolTable->GetSymbTableSize();
		INTER_VARIANT var;
		// removed WS:EPM 17jul07 var.varType = RUL_NULL;
		_INT32 iRetValue = VISIT_NORMAL; 
		bool bRetValFound = false;
		int iRetVarIndex = 0;
		//Now Loop through the Symbol table and Get the variable 
		//which we pushed as the Return variable in the starting
		//This variable will have m_bIsReturnToken as true..
		//None other var in symbol table should have this flag set

		for(int iCount = 0; iCount < iSizeSymbTa; iCount++)
		{
			CVariable *pCVariable = pSymbolTable->GetAt(iCount);
			if(NULL != pCVariable )
			{
				if(pCVariable->m_bIsReturnToken == true)
				{
					//Once we get that Fill its variable type depending on the Return type declared in the method
					iRetVarIndex = iCount;
					bRetValFound = true;
					switch(pCVariable->GetSubType())
					{
						case RUL_CHAR_DECL:
							{
								var = (char)' ';
							}
							break;
						case RUL_LONG_LONG_DECL:
							{
								var = (INT64)0;
							}
							break;
						// Walt EPM 08sep08 - start insert
						case RUL_UNSIGNED_SHORT_INTEGER_DECL:
							{
								var = (wchar_t)0;
							}
							break;
						case RUL_SHORT_INTEGER_DECL:
							{
								var = (short)0;
							}
							break;
						case RUL_UNSIGNED_INTEGER_DECL:
							{
								var = (unsigned int)0;
							}
							break;
						// Walt EPM 08sep08 - end insert
						case RUL_INTEGER_DECL:
						case RUL_LONG_DECL:
							{
							//	var = (long)0;
								var = static_cast <long>(0);
							}
							break;
						case RUL_REAL_DECL:
							{
								var = (float)0.0;
							}
							break;
						case RUL_DOUBLE_DECL:
							{
								var = (double)0.0;//WS:EPM 10aug07
							}
							break;
						case RUL_BOOLEAN_DECL:
							{
								var = (bool)false;
							}
							break;
						case RUL_DD_STRING_DECL :
							{
								var=(char *)"";
							}
							break;

						default:
							break; //var.varType == RUL_NULL;
					} //End of switch
					break;

				}//End of pCVariable->m_bIsReturnToken == true

			}//end of NULL != pCVariable 
		}//End of for loop	
	
		
		if(pExp)
		{
			//Execute this return statement and get the value out of that and then store that in the Return variable
			iRetValue = pExp->Execute(this,pSymbolTable,&var);
			CVariable *pCVariable = pSymbolTable->GetAt(iRetVarIndex);
			pCVariable->GetValue() = var;
			return VISIT_RETURN;
		}
		else
		{
			//error -- no expression in assingment statement
			return VISIT_ERROR;
		}
	}
	return VISIT_RETURN;

}

_INT32 CInterpretedVisitor::visitContinueStatement(
			CContinueStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return VISIT_CONTINUE;
}

_INT32 CInterpretedVisitor::visitIterationStatement(
			CIterationStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CExpression* pExp=0;
	CStatement* pExpStmt=0;
	CStatement* pStmt=0;

	if(pItnStmt)
	{
		if (pItnStmt->GetExpressionNodeType() == NODE_TYPE_EXPRESSION)
		{
			pExp = pItnStmt->GetExpression();
		}
		else
		{
			pExpStmt = pItnStmt->GetExpressionStatement();
		}
		pStmt = pItnStmt->GetStatement();
	}
	else
	{
		//error -- no Iteration statement.
		return VISIT_ERROR;
	}

	INTER_VARIANT var1,var2;
	if(pExp || pExpStmt)
	{
		unsigned long ulongLoopCount = 0;
		while(true)
		{
			ulongLoopCount++;
			if (ulongLoopCount >= MAX_LOOPS)
			{
				break;
			}

			int iRetValue;

			if (pItnStmt->GetExpressionNodeType() == NODE_TYPE_EXPRESSION)
			{
				iRetValue = pExp->Execute(this,pSymbolTable,&var1);
			}
			else
			{
				iRetValue = pExpStmt->Execute(this,pSymbolTable,&var1);
			}
			
			if (iRetValue == VISIT_RETURN)
			{
				return iRetValue;
			}

			if (!(iRetValue && (int)var1 && pStmt))
			{
				break;
			}

			var1.Clear();
			int iVisitReturnType = pStmt->Execute(this,pSymbolTable,&var2);
			switch(iVisitReturnType)
			{
				case VISIT_BREAK:
					return VISIT_NORMAL;
				case VISIT_CONTINUE:
					continue;
				case VISIT_RETURN:
					return VISIT_RETURN;
			}
			var2.Clear();
		}
	}
	else
	{
		//error -- no expression in while statement
		return VISIT_ERROR;
	}
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitIterationStatement(
			CIterationDoWhileStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CExpression* pExp=0;
	CStatement* pExpStmt=0;
	CStatement* pStmt=0;

	if(pItnStmt)
	{
		if (pItnStmt->GetExpressionNodeType() == NODE_TYPE_EXPRESSION)
		{
			pExp = pItnStmt->GetExpression();
		}
		else
		{
			pExpStmt = pItnStmt->GetExpressionStatement();
		}
		
		pStmt = pItnStmt->GetStatement();
	}
	else
	{
		//error -- no Iteration statement.
		return VISIT_ERROR;
	}

	INTER_VARIANT var1,var2;
	if(pExp || pExpStmt)
	{
		unsigned long ulongLoopCount = 0;
		do
		{
			ulongLoopCount++;
			if (ulongLoopCount >= MAX_LOOPS)
			{
				break;
			}

			var1.Clear();
			int iVisitReturnType = pStmt->Execute(this,pSymbolTable,&var2);
			switch(iVisitReturnType)
			{
				case VISIT_BREAK:
					return VISIT_NORMAL;
				case VISIT_CONTINUE:
					continue;
				case VISIT_RETURN:
					return VISIT_RETURN;
			}
			var2.Clear();

			int iRetValue;

			if (pItnStmt->GetExpressionNodeType() == NODE_TYPE_EXPRESSION)
			{
				iRetValue = pExp->Execute(this,pSymbolTable,&var1);
			}
			else
			{
				iRetValue = pExpStmt->Execute(this,pSymbolTable,&var1);
			}
			
			if (iRetValue == VISIT_RETURN)
			{
				return iRetValue;
			}

			if (iRetValue && (int)var1 && pStmt)
			{
				continue;
			}
			else
			{
				break;
			}
		}while( true);
	}
	else
	{
		//error -- no expression in while statement
		return VISIT_ERROR;
	}
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitIterationStatement(
			CIterationForStatement* pItnStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CExpression* pExp=0;
	CStatement* pExpStmt=0;
	CStatement* pStmt=0;
	CStatement* pInitStmt = 0;
	CStatement* pIncrStmt=0;
	CExpression* pIncrExp=0;

	if(pItnStmt)
	{
		if (pItnStmt->GetExpressionNodeType() == NODE_TYPE_EXPRESSION)
		{
			pExp = pItnStmt->GetExpression();
		}
		else
		{
			pExpStmt = pItnStmt->GetExpressionStatement();
		}
		pStmt = pItnStmt->GetStatement();
		pInitStmt = pItnStmt->GetInitializationStatement();
		pIncrStmt = pItnStmt->GetIncrementStatement();
		pIncrExp = pItnStmt->GetIncrementExpression();
	}
	else
	{
		//error -- no Iteration statement.
		return VISIT_ERROR;
	}

	INTER_VARIANT var1,var2,var3,var4;
	if(pExp || pExpStmt)
	{
		if(pInitStmt)
				pInitStmt->Execute(this,pSymbolTable,&var3);

		int iLoopVar=0;
		unsigned long ulongLoopCount = 0;
		for(;;)
		{
			ulongLoopCount++;
			if (ulongLoopCount >= MAX_LOOPS)
			{
				break;
			}

			if (iLoopVar != 0)
			{
				if (pIncrStmt)
					pIncrStmt->Execute(this,pSymbolTable,&var4);
				else if (pIncrExp)
					pIncrExp->Execute(this,pSymbolTable,&var4);

			}
			iLoopVar++;

			int iRetValue;

			if (pItnStmt->GetExpressionNodeType() == NODE_TYPE_EXPRESSION)
			{
				iRetValue = pExp->Execute(this,pSymbolTable,&var1);
			}
			else
			{
				iRetValue = pExpStmt->Execute(this,pSymbolTable,&var1);
			}
			
			if (iRetValue == VISIT_RETURN)
			{
				return iRetValue;
			}

			if (!(iRetValue && (int)var1 && pStmt))
			{
				break;
			}

			var1.Clear();
			var3.Clear();
			var4.Clear();
			int iVisitReturnType = pStmt->Execute(this,pSymbolTable,&var2);
			switch(iVisitReturnType)
			{
				case VISIT_BREAK:
					return VISIT_NORMAL;
				case VISIT_CONTINUE:
					continue;
				case VISIT_RETURN:
					return VISIT_RETURN;
			}
			var2.Clear();
		}
	}
	else
	{
		//error -- no expression in while statement
		return VISIT_ERROR;
	}
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitSelectionStatement(
			CSelectionStatement* pSelStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CExpression* pExp=0;
	CStatement* pStmt=0;
	CELSEStatement* pElse=0;

	if(pSelStmt)
	{
		pExp = pSelStmt->GetExpression();
		pStmt = pSelStmt->GetStatement();
		pElse = pSelStmt->GetELSEStatement();
	}
	else
	{
		//error -- no selection statement..
		return VISIT_ERROR;
	}

	INTER_VARIANT var1,var2;
	if(pExp)
	{
		_INT32 iRetValue;

		// Gowtham 260306: Start of Code Modifications
		// Split the if condition to see if the return value is VISIT_RETURN.
		iRetValue = pExp->Execute(this,pSymbolTable,&var1);
		if (iRetValue == VISIT_RETURN)
		{
			return iRetValue;
		}
		if(iRetValue && (bool)var1 && pStmt)
		{
		// Gowtham 260306: End of Code Modifications
			if (iRetValue == VISIT_RETURN)
			{
				return iRetValue;
			}

			var1.Clear();
			int iVisitReturnType = pStmt->Execute(this,pSymbolTable,&var2);
			switch(iVisitReturnType)
			{
				case VISIT_BREAK:
					return VISIT_BREAK;
				case VISIT_CONTINUE:
					return VISIT_CONTINUE;
				case VISIT_RETURN:
					return VISIT_RETURN;
			}
			var2.Clear();
		}
		else if(false == (bool)var1)
		{
			if(pElse)
			{
				int iVisitReturnType = pElse->Execute(this,pSymbolTable,&var2);
				switch(iVisitReturnType)
				{
					case VISIT_BREAK:
						return VISIT_BREAK;
					case VISIT_CONTINUE:
						return VISIT_CONTINUE;
					case VISIT_RETURN:
						return VISIT_RETURN;
				}
			}
			var2.Clear();
		}
		else
		{
			//either there is no statement or execute failed.
			return VISIT_ERROR;
		}
	}
	else
	{
		//error -- no expression in if statement...
	}
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitSwitchStatement(
			CSwitchStatement* pSelStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CExpression* pExp=0;
	CStatement* pExpStmt=0;
	CStatement* pStmt=0;
	CCASEStatement* pCaseStatement=0;
	int iNumberOfCasesPresent=0;
	_BOOL bIsDefaultPresent=false;

	if(pSelStmt)
	{
		if (pSelStmt->GetExpressionNodeType() == NODE_TYPE_EXPRESSION)
		{
			pExp = pSelStmt->GetExpression();
		}
		else
		{
			pExpStmt = pSelStmt->GetExpressionStatement();
		}
		pStmt = pSelStmt->GetStatement();
		iNumberOfCasesPresent = pSelStmt->GetNumberOfCaseStatements();
		bIsDefaultPresent=pSelStmt->IsDefaultPresent();
	}
	else
	{
		//error -- no selection statement..
		return VISIT_ERROR;
	}

	if ((iNumberOfCasesPresent <= 0) && (bIsDefaultPresent == false))
	{
		return VISIT_NORMAL;

	}

	INTER_VARIANT var1,var2,var3;
	if(pExp || pExpStmt)
	{
		int iRetValue;
		if (pSelStmt->GetExpressionNodeType() == NODE_TYPE_EXPRESSION)
		{
			iRetValue = pExp->Execute(this,pSymbolTable,&var1);
		}
		else
		{
			iRetValue = pExpStmt->Execute(this,pSymbolTable,&var1);
		}		

		if (iRetValue == VISIT_RETURN)
		{
			return iRetValue;
		}

		_BOOL bMatchFound = false;

		for (int iLoopVar=0;iLoopVar <iNumberOfCasesPresent;iLoopVar++)
		{
			pCaseStatement = pSelStmt->GetCaseStatement(iLoopVar);

			CExpression*pCaseExp = pCaseStatement->GetExpression();

			pCaseExp->Execute(this,pSymbolTable,&var2);

			if ( ((int)var1 == (int)var2) || bMatchFound)
			{
				bMatchFound = true;
				int iVisitReturnType 
						= pCaseStatement->Execute (this,pSymbolTable,&var3);
				switch(iVisitReturnType)
				{
					case VISIT_BREAK:
						return VISIT_NORMAL;
					case VISIT_CONTINUE:
						return VISIT_CONTINUE;
					case VISIT_RETURN:
						return VISIT_RETURN;
				}
			}
			var2.Clear ();
		}
		if (bIsDefaultPresent)
		{
			pCaseStatement = pSelStmt->GetDefaultStatement();

			int iVisitReturnType 
					= pCaseStatement->Execute (this,pSymbolTable,&var3);
			switch(iVisitReturnType)
			{
				case VISIT_BREAK:
					return VISIT_NORMAL;
				case VISIT_CONTINUE:
					return VISIT_CONTINUE;
				case VISIT_RETURN:
					return VISIT_RETURN;
			}
			var2.Clear ();
		}
		return VISIT_NORMAL;
	}
	else
	{
		//error -- no expression in if statement...
	}
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitELSEStatement(
			CELSEStatement* pELSE,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CStatement* pStmt=0;
	INTER_VARIANT var;
	if(pELSE)
	{
		pStmt = pELSE->GetStatement();
	}
	else
	{
		//error -- no else statement.
		return VISIT_ERROR;
	}
	if(pStmt)
	{
		return pStmt->Execute(this,pSymbolTable,&var);
	}
	else
	{
		//error -- no Statement in Else
		return VISIT_ERROR;
	}

	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitCASEStatement(
			CCASEStatement* pCase,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CStatementList* pStmtList=0;
	INTER_VARIANT var;
	if(pCase)
	{
		pStmtList = pCase->GetStatement();
	}
	else
	{
		//error -- no else statement.
		return VISIT_ERROR;
	}
	if(pStmtList)
	{
		return pStmtList->Execute(this,pSymbolTable,&var);
	}
	else
	{
		//error -- no Statement in Else
		return VISIT_ERROR;
	}

	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::uplusplus(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	INTER_VARIANT temp;
	int i = 1;
	temp.SetValue ((void *)&i, RUL_INT);
	v3 = v1;
	v1 = v1 + temp;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::uminusminus(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	INTER_VARIANT temp;
	int i = 1;
	temp.SetValue ((void *)&i, RUL_INT);
	v3 = v1;
	v1 = v1 - temp;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::upreplusplus(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	INTER_VARIANT temp;
	int i = 1;
	temp.SetValue ((void *)&i, RUL_INT);
	v3 = v1 + temp;
	v1 = v1 + temp;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::upreminusminus(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	INTER_VARIANT temp;
	int i = 1;
	temp.SetValue ((void *)&i, RUL_INT);
	v3 = v1 - temp;
	v1 = v1 - temp;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::uplus(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	switch (v1.GetVarType())
	{
		case RUL_CHAR:
				char ch;
				v1.GetValue((void*)&ch, RUL_CHAR);
				ch = +(ch);
				v3.SetValue((void*)&ch, RUL_CHAR);
				break;
		case RUL_UNSIGNED_CHAR:
				{
					unsigned char i;
					v1.GetValue((void*)&i, RUL_UNSIGNED_CHAR);
					i = +(i);
					v3.SetValue((void*)&i, RUL_UNSIGNED_CHAR);
				}
				break;
		case RUL_SHORT:
				{
					short i;
					v1.GetValue((void*)&i, RUL_SHORT);
					i = +(i);
					v3.SetValue((void*)&i, RUL_SHORT);
				}
				break;
		case RUL_USHORT:
				{
					unsigned short i;
					v1.GetValue((void*)&i, RUL_USHORT);
					i = +(i);
					v3.SetValue((void*)&i, RUL_USHORT);
				}
				break;
		case RUL_INT:
				{
				int i;
				v1.GetValue((void*)&i, RUL_INT);
				i = +(i);
				v3.SetValue((void*)&i, RUL_INT);
				}
				break;
		case RUL_UINT:
				{
					unsigned long i=0;
					v1.GetValue((void*)&i, RUL_UINT);
					i = +(i);
					v3.SetValue((void*)&i, RUL_UINT);
				}
				break;
		case RUL_LONGLONG:
				{
					__int64 i=0;
					v1.GetValue((void*)&i, RUL_LONGLONG);
					i = +(i);
					v3.SetValue((void*)&i, RUL_LONGLONG);
				}
				break;
		case RUL_ULONGLONG:
				{
					unsigned __int64 i=0;
					v1.GetValue((void*)&i, RUL_ULONGLONG);
					i = +(i);
					v3.SetValue((void*)&i, RUL_ULONGLONG);
				}
				break;
		case RUL_FLOAT:
				float f;
				v1.GetValue((void*)&f, RUL_FLOAT);
				f = +(f);
				v3.SetValue((void*)&f, RUL_FLOAT);
				break;
		case RUL_DOUBLE:
				double d;
				v1.GetValue((void*)&d, RUL_DOUBLE);
				d = +(d);
				v3.SetValue((void*)&d, RUL_DOUBLE);
				break;
	}
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::uminus(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	switch (v1.GetVarType())
	{
		case RUL_CHAR:
				char ch;
				v1.GetValue((void*)&ch, RUL_CHAR);
				ch = -(ch);
				v3.SetValue((void*)&ch, RUL_CHAR);
				break;
		case RUL_UNSIGNED_CHAR://negative of an unsigned... we need to promote this to an integer
				{
					unsigned char i=0;
					v1.GetValue((void*)&i, RUL_UNSIGNED_CHAR);
					int j = -(int)(i);
					v3.SetValue((void*)&j, RUL_INT);
				}
				break;
		case RUL_SHORT:
				{
					short i=0;
					v1.GetValue((void*)&i, RUL_SHORT);
					i = -(i);
					v3.SetValue((void*)&i, RUL_SHORT);
				}
				break;
		case RUL_USHORT://negative of an unsigned... we need to promote this to an integer
				{
					short i=0;
					v1.GetValue((void*)&i, RUL_USHORT);
					int j = -(int)(i);
					v3.SetValue((void*)&j, RUL_INT);
				}
				break;
		case RUL_INT:
				{
					int i=0;
					v1.GetValue((void*)&i, RUL_INT);
					i = -(i);
					v3.SetValue((void*)&i, RUL_INT);
				}
				break;
		case RUL_UINT://negative of an unsigned long... we need to promote this to a long long
				{
					unsigned long i=0;
					v1.GetValue((void*)&i, RUL_UINT);
					__int64 j = -(__int64)(i);
					v3.SetValue((void*)&j, RUL_LONGLONG);
				}
				break;
		case RUL_LONGLONG:
				{
					__int64 i=0;
					v1.GetValue((void*)&i, RUL_LONGLONG);
					i = -(i);
					v3.SetValue((void*)&i, RUL_LONGLONG);
				}
				break;
		case RUL_ULONGLONG://negative of an unsigned long long... we need to promote this to a double
				{
					unsigned __int64 i=0;
					v1.GetValue((void*)&i, RUL_ULONGLONG);
					double j = -(double)(__int64)(i);
					v3.SetValue((void*)&j, RUL_DOUBLE);
				}
				break;
		case RUL_FLOAT:
				{
					float f=0.0;
					v1.GetValue((void*)&f, RUL_FLOAT);
					f = -(f);
					v3.SetValue((void*)&f, RUL_FLOAT);
				}
				break;
		case RUL_DOUBLE:
				{
					double d=0.0;
					v1.GetValue((void*)&d, RUL_DOUBLE);
					d = -(d);
					v3.SetValue((void*)&d, RUL_DOUBLE);
				}
				break;
	}
	//v3 = -(v1);
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bit_and(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 & v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bit_or(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 | v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bitxor(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 ^ v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bitnot(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = ~v1;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bitrshift(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 >> v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bitlshift(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 << v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::add(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 + v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::sub(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 - v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::mul(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 * v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::div(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 / v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::mod(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 % v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::exp(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 ^ v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::neq(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = (v1 != v2);
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::lt(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = (v1 < v2);
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::gt(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = (v1 > v2);
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::eq(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 == v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::ge(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = (v1 >= v2);
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::le(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 <= v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::land(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 && v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::lor(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 || v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::lnot(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = !v1;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::rparen(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::assign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::plusassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 + v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::minusassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 - v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::divassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 / v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::modassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 % v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::mulassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 * v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bitandassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 & v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bitorassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 | v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::bitxorassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 ^ v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::rshiftassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 >> v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::lshiftassign(
			INTER_VARIANT& v1,
			INTER_VARIANT& v2,
			INTER_VARIANT& v3)
{
	v3 = v1 << v2;
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitCompoundExpression(
			CCompoundExpression* pCompStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CExpression*		pFirstExp = 0;
	CExpression*		pSecondExp = 0;
	RUL_TOKEN_SUBTYPE	Operator = RUL_SUBTYPE_NONE;
	INTER_VARIANT		var1;
	INTER_VARIANT		var2;

	if(pCompStmt)
	{
		pFirstExp = pCompStmt->GetFirstExpression();
		pSecondExp = pCompStmt->GetSecondExpression();
		Operator = pCompStmt->GetOperator();
	}
	else
	{
		//error
		return VISIT_ERROR;
	}

	if(pFirstExp)
	{
		int iVisitReturnType = pFirstExp->Execute(this,pSymbolTable,&var1);
		switch(iVisitReturnType)
		{
			case VISIT_BREAK:
				return VISIT_BREAK;
			case VISIT_CONTINUE:
				return VISIT_NORMAL;
			case VISIT_RETURN:
				return VISIT_RETURN;
		}
	}
	else
	{
		//error -- no first operand
		return VISIT_ERROR;
	}
	// emerson checkin april2013
	// evaluate OR and AND for the first expression
	// For OR, if the first expression is true, we are done and return true
	// For AND, if the first expression is false, we are done and return false
	if (Operator == RUL_LOGIC_AND)
	{
		if ((bool)var1 == false)
		{
			pvar->Clear();
			*pvar = false;
			return VISIT_NORMAL;
		}
	}
	else if (Operator == RUL_LOGIC_OR)
	{
		if ((bool)var1 == true)
		{
			pvar->Clear();
			*pvar = true;
			return VISIT_NORMAL;
		}
	}

	if(pSecondExp)
	{
		int iVisitReturnType = pSecondExp->Execute(this,pSymbolTable,&var2);
		switch(iVisitReturnType)
		{
			case VISIT_BREAK:
				return VISIT_BREAK;
			case VISIT_CONTINUE:
				return VISIT_NORMAL;
			case VISIT_RETURN:
				return VISIT_RETURN;
		}
	}

	pvar->Clear();
	if(m_fnTable[Operator])
	{
    if (		(Operator == RUL_ASSIGN) 
		|| (Operator == RUL_PLUS_ASSIGN) 
		|| (Operator == RUL_MINUS_ASSIGN) 
		|| (Operator == RUL_DIV_ASSIGN) 
		|| (Operator == RUL_MOD_ASSIGN) 
		|| (Operator == RUL_MUL_ASSIGN) 
		|| (Operator == RUL_BIT_AND_ASSIGN) 
		|| (Operator == RUL_BIT_OR_ASSIGN) 
		|| (Operator == RUL_BIT_XOR_ASSIGN) 
		|| (Operator == RUL_BIT_RSHIFT_ASSIGN) 
		|| (Operator == RUL_BIT_LSHIFT_ASSIGN) 
    )
    {
		  (this->*m_fnTable[Operator])(var2,var1,*pvar);
    }
    else
    {
		(this->*m_fnTable[Operator])(var1,var2,*pvar);
    }
	}
	if ( (Operator == RUL_PLUS_PLUS)
		|| (Operator == RUL_MINUS_MINUS) 
		|| (Operator == RUL_PRE_PLUS_PLUS) 
		|| (Operator == RUL_PRE_MINUS_MINUS) 
		)
	{
		if(pFirstExp)
		{
			CToken *pExpToken = ((CPrimaryExpression *)pFirstExp)->GetToken();
			CVariable* pStore=0; //Anil Moved above
			if(pExpToken && pExpToken->IsVariable())
			{
				_INT32 nIdx = pExpToken->GetSymbolTableIndex();
				pStore = pSymbolTable->GetAt(nIdx);
				
				if(pStore)
					pStore->GetValue() = var1;
			}
			//Added By Anil August 25 2005 --starts here
			//Check whether it is of type DD item
			if(pExpToken && pExpToken->IsDDItem())
			{
				_INT32 nIdx = pExpToken->GetSymbolTableIndex();
				pStore = m_pMEE->m_GlobalSymTable.GetAt(nIdx);
				if(pStore)
					pStore->GetValue() = var1;
				RETURNCODE iReturnValue = m_pMEE->ResolveNUpdateDDExp(pExpToken->GetLexeme(), pExpToken->GetDDItemName(),&var1);
				if(	iReturnValue == FAILURE)
				{
					return VISIT_ERROR;
				}				
			}
			//Added By Anil August 25 2005 --Ends here
		}
	}

	if (
		(Operator == RUL_ASSIGN) 
		|| (Operator == RUL_PLUS_ASSIGN) 
		|| (Operator == RUL_MINUS_ASSIGN) 
		|| (Operator == RUL_DIV_ASSIGN) 
		|| (Operator == RUL_MOD_ASSIGN) 
		|| (Operator == RUL_MUL_ASSIGN) 
		|| (Operator == RUL_BIT_AND_ASSIGN) 
		|| (Operator == RUL_BIT_OR_ASSIGN) 
		|| (Operator == RUL_BIT_XOR_ASSIGN) 
		|| (Operator == RUL_BIT_RSHIFT_ASSIGN) 
		|| (Operator == RUL_BIT_LSHIFT_ASSIGN) 
		)
	{
		if(pFirstExp)
		{
			CToken *pExpToken = ((CPrimaryExpression *)pFirstExp)->GetToken();
			CVariable* pStore=0;//Anil Moved up
			if(pExpToken && pExpToken->IsVariable())
			{
				_INT32 nIdx = pExpToken->GetSymbolTableIndex();
				pStore = pSymbolTable->GetAt(nIdx);
				if(pStore)
					pStore->GetValue() = *pvar;
			}
			//Added By Anil August 25 2005 --starts here
			//Check whether it is of type DD item
			if(pExpToken && pExpToken->IsDDItem())
			{
				_INT32 nIdx = pExpToken->GetSymbolTableIndex();
				pStore = m_pMEE->m_GlobalSymTable.GetAt(nIdx);
				if(pStore)
					pStore->GetValue() = var1;
				RETURNCODE iReturnValue = m_pMEE->ResolveNUpdateDDExp(pExpToken->GetLexeme(), pExpToken->GetDDItemName(),pvar);
				if(	iReturnValue == FAILURE)
				{
					return VISIT_ERROR;
				}				
			}
			//Added By Anil August 25 2005 --Ends here
		}
	}
	return VISIT_NORMAL;
}

//Return the value
_INT32 CInterpretedVisitor::visitPrimaryExpression(
			CPrimaryExpression* pPrimStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	CToken* pToken=0;
	if(pPrimStmt)
	{
		pToken = pPrimStmt->GetToken();
	}
	else
	{
		//error -- no primary statement...
		return VISIT_ERROR;
	}
	if(pToken && pToken->IsVariable())
	{
		_INT32 nIdx = pToken->GetSymbolTableIndex();
		CVariable* pStore=0;
		if(nIdx >=0)
		{
		pStore = pSymbolTable->GetAt(nIdx);
			
		if(pStore)
			*pvar = pStore->GetValue();
		}
		/* pwink 01 - log the internal fetch, value is alwats empty???
		char ValStr[1024];
		char* pValStr = &(ValStr[0]); memset(pValStr,0,1024);
		pvar->GetStringValue(&pValStr);
		LOGIT(CLOG_LOG,"--PrimaryExpression:Getting var value lexeme: %s  value: %s symbolIndex: %d\n",pToken->GetLexeme(),pValStr,nIdx);
		if ( pvar->GetVarType() == RUL_FLOAT )
		{
			float L;
			pvar->GetValue(&L,RUL_FLOAT);
			LOGIT(CLOG_LOG,"--PrimaryExpression: Float Value: %f  8E: %.8E g:%g\n",L,L,L);
		}
		// end pwink 01 **/
		
	}
	else if(pToken->IsNumeric())
	{
		INTER_VARIANT temp(true,pToken->GetLexeme());
		*pvar = temp;
		/* pwink 28 - log the internal fetch, value is alwats empty???
		/*char ValStr[1024];
		char* pValStr = &(ValStr[0]); memset(pValStr,0,1024);
		pvar->GetStringValue(&pValStr);
		LOGIT(CLOG_LOG,"--PrimaryExpression:Getting numeric value lexeme: %s  value: %s\n",pToken->GetLexeme(),pValStr);*/
		// end pwink 28 **/
	}
	else if(pToken->IsConstant())
	{
		//Got to fill it this up...
		if(RUL_STRING_CONSTANT == pToken->GetSubType())
		{
			*pvar = (_CHAR*)pToken->GetLexeme();
		}
		else if(RUL_CHAR_CONSTANT == pToken->GetSubType())
		{
			char *pchChar = (_CHAR*)pToken->GetLexeme(); 
			*pvar = pchChar[0];
		}
		/* pwink 01 - log the internal fetch, value is alwats empty???
		char ValStr[1024];
		char* pValStr = &(ValStr[0]); memset(pValStr,0,1024);
		pvar->GetStringValue(&pValStr);
		LOGIT(CLOG_LOG,"--PrimaryExpression:Getting constant value lexeme: %s  value: %s\n",pToken->GetLexeme(),pValStr);
		// end pwink 01 **/
	}
	else
	{
		//error -- no token in a primary statement.
		return VISIT_ERROR;
	}
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitProgram(
			CProgram* pProgram,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CDeclarations* pDecl = 0;
	CStatementList* pStmtList = 0;

	if(pProgram)
	{
		pDecl=pProgram->GetDeclarations();
		pStmtList=pProgram->GetStatementList();
	}
	else
	{
		//error -- no program to execute.
		return VISIT_ERROR;
	}
	if(pDecl)
	{
LOGIT(CLOG_LOG,"CInterpretedVisitor::visitProgram")	;
		pDecl->Execute(this,pSymbolTable);
LOGIT(CLOG_LOG,"CInterpretedVisitor::visitProgram - after")	;
	}
	else
	{
//		return VISIT_ERROR;
	}
	if(pStmtList)
	{
		return pStmtList->Execute(this,pSymbolTable);
	}
	else
	{
		//error -- no statements in the program...
		return VISIT_ERROR;
	}

	return 1;
}

_INT32 CInterpretedVisitor::visitCompoundStatement(
			CCompoundStatement* pCompStmt,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CStatementList* pStmtLst=0;

	if(pCompStmt)
	{
		pStmtLst = pCompStmt->GetStatementList();
	}
	else
	{
		//error -- no compound statement.
		return VISIT_ERROR;
	}

	return visitStatementList(pStmtLst,pSymbolTable,pvar);
}

_INT32 CInterpretedVisitor::visitExpression(
			CExpression* pExpression,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	CExpression* pExp=0;

	if(pExpression)
	{
		pExp = pExpression->GetExpression();
	}
	else
	{
		return VISIT_ERROR;
	}

	if (0 == pExp)
	{
		return VISIT_ERROR;
	}
	INTER_VARIANT var1;
	
	return pExp->Execute(this,pSymbolTable,&var1);
}

_INT32 CInterpretedVisitor::visitStatement(
			CStatement* pStatement,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return VISIT_ERROR;
}

_INT32 CInterpretedVisitor::visitStatementList(
			CStatementList* pStmtList,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	STATEMENT_LIST* pStmtCol=0;
	CStatement* pStmt=0;

	if(pStmtList)
	{
		pStmtCol = pStmtList->GetStmtList();
		_INT32 nSize = pStmtCol->size();
		for(_INT32 i=0;i<nSize;i++)
		{
			pStmt = (*pStmtCol)[i];
			pSymbolTable->m_nCurrentScope= pStmt->GetScopeIndex();			//SCR26200 Felix
			int iVisitReturnType = pStmt->Execute(this,pSymbolTable,0);//stevev 19nov09 -force it null in release mode too
			switch(iVisitReturnType)
			{
				case VISIT_BREAK:
					return VISIT_BREAK;
				case VISIT_CONTINUE:
					return VISIT_CONTINUE;
				case VISIT_RETURN:
					return VISIT_RETURN;
			}
			//Anil 180107 if VISIT_SCOPE_VAR == iVisitReturnType means  we are executing 
			//the Statement list which has declaration
			//This is a bug fix to get rid of the variable which is declared within the scope
			//the below DD Method code was not handled
			/*MethodDefination 
			{
				int x;
				x = 0;
				if(int x == 0)
				{
					int y; 
					ACKNOWLEDGE("This was not executing");
				}
				

			}*/
			if(VISIT_SCOPE_VAR == iVisitReturnType )
			{
				//Anil 240107 Fool the interpreter that you have executed this statement( which is declaration)
				continue;
			}
			if( iVisitReturnType == VISIT_ERROR)//Anil Added September 12 2005
				return VISIT_ERROR;
		}
	}
	else
	{
		//error -- no statements in the list.
		return VISIT_ERROR;
	}
	return VISIT_NORMAL;
}

//	Modify this to allocate memory for array variables by traversing 
//	the list of vars in the Symbol table.
//	from the symbol table get the index and from that get the variant (var). 
//	now we know that the variant must be an array.  allocate memory.
_INT32 CInterpretedVisitor::visitDeclarations(
			CDeclarations* pDeclarations,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{

	_INT32 i32Count = pSymbolTable->GetCount();
	CVariable* pVariable=0;
	
	for(_INT32 i=0;i<i32Count; i++)
	{
		pVariable = pSymbolTable->GetAt(i);
		INTER_VARIANT& var = pVariable->GetValue();
		
		//Bug Fix for PAR 570. Initilize DD_STRING to null character during the declaration execution
		//Below conditio gives it is DD_STRING
		if( 
			(RUL_DD_STRING_DECL ==pVariable->GetSubType() ) && 
			(RUL_SIMPLE_VARIABLE ==pVariable->GetType()   )  && 
		// feb08	(RUL_CHARPTR == var.GetVarType())&&
			(RUL_WIDECHARPTR == var.GetVarType())&&
			!(pVariable->m_bIsRoutineToken)
		  )
		{
			wstring tmpStr = _T("");			
			var.SetValue((wchar_t*)tmpStr.c_str(),RUL_DD_STRING);

		}

		if(RUL_SAFEARRAY == var.GetVarType() && !(pVariable->m_bIsRoutineToken))
		{
			(var.GetValue().prgsa)->Allocate();
		}
	}
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitRuleService(
			CRuleServiceStatement* pStatement,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return VISIT_ERROR;
}

_INT32 CInterpretedVisitor::visitOMExpression(
			COMServiceExpression* pExpression,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return VISIT_NORMAL;
}

_INT32 CInterpretedVisitor::visitIFExpression(
			IFExpression* pIfExp,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	if (pIfExp == NULL)
	{
		return VISIT_ERROR;
	}
	CExpression *pExp;
	CExpression *pTrueExp;
	CExpression *pFalseExp;

	pIfExp->GetExpressions(pExp, pTrueExp, pFalseExp);

	if (pExp)
	{
		INTER_VARIANT Var;
		_INT32 iRetValue;

		iRetValue = pExp->Execute (this, pSymbolTable,&Var,pvecErrors);
		if (iRetValue == VISIT_RETURN)
		{
			return iRetValue;
		}

		if ((bool)Var)
		{
			if (pTrueExp)
			{
				iRetValue = pTrueExp->Execute (this, pSymbolTable,&Var,pvecErrors);
				if (iRetValue == VISIT_RETURN)
				{
					return iRetValue;
				}
				*pvar = Var;
			}
			else
			{
				return VISIT_ERROR;
			}
		}
		else
		{
			if (pFalseExp)
			{
				iRetValue = pFalseExp->Execute (this, pSymbolTable,&Var,pvecErrors);
				if (iRetValue == VISIT_RETURN)
				{
					return iRetValue;
				}
				*pvar = Var;
			}
			else
			{
				return VISIT_ERROR;
			}
		}
		return VISIT_NORMAL;
	}
	else
	{
		return VISIT_ERROR;
	}
}

_INT32 CInterpretedVisitor::visitFunctionExpression(
			FunctionExpression* pFuncExp,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	if (NULL == pFuncExp)
	{
		return VISIT_ERROR;
	}

	INTER_VARIANT pVarParams[MAX_NUMBER_OF_FUNCTION_PARAMETERS];
	
	for (int iLoopVar = 0;iLoopVar < pFuncExp->GetParameterCount();iLoopVar++)
	{
		if (pFuncExp->GetParameterType(iLoopVar) != RUL_STR_CONSTANT)
		{
			CExpression *pExp = pFuncExp->GetExpParameter(iLoopVar);
			if (NULL == pExp)
			{// message added emerson checkin april2013
				char szMessage[1024]={0};
				sprintf_s( szMessage, sizeof(szMessage), "***FAILURE*** Missing Argument %d of %s\n", iLoopVar+1, pFuncExp->GetFunctionName() );
				LOGIT(COUT_LOG,szMessage);

				return VISIT_ERROR;
			}
			_INT32 iRetValue;

			iRetValue = 
				pExp->Execute(this,pSymbolTable,&pVarParams[iLoopVar],pvecErrors);
			if (iRetValue == VISIT_RETURN)
			{
				return iRetValue;
			}
		}
		else
		{
			CToken *pToken = pFuncExp->GetConstantParameter(iLoopVar);
			//pVarParams[iLoopVar].SetValue((void *)pToken->GetLexeme(), RUL_CHARPTR);
            if( pToken )  //WaltS - 04may07 this check for NULL pointer 
            { 
				pVarParams[iLoopVar] = (char *)pToken->GetLexeme(); 
            } 
            else   //WaltS - 04may07
            { 
            	// expressiona as parameters added in emerson checkin april2013
				CExpression *pExp = pFuncExp->GetExpParameter(iLoopVar);
				if (NULL == pExp)
				{
					char szMessage[1024]={0};
					sprintf_s( szMessage,sizeof(szMessage), "***FAILURE*** Missing Argument %d of %s\n", iLoopVar+1, pFuncExp->GetFunctionName() );
					LOGIT(COUT_LOG,szMessage);

				return VISIT_ERROR; 
            } 
				_INT32 iRetValue;

				iRetValue = 
					pExp->Execute(this,pSymbolTable,&pVarParams[iLoopVar],pvecErrors);
				if (iRetValue == VISIT_RETURN)
				{
					return iRetValue;
				}				
            } 
		}
	}

	int iReturnStatus = BUILTIN_SUCCESS;
	bool bRetValue = m_pBuiltInLib->InvokeFunction
									(
										(char*)pFuncExp->GetFunctionName()
										, pFuncExp->GetParameterCount()
										, pVarParams
										, pvar
										, &iReturnStatus
										, pFuncExp			// added WS:EPM 17jul07
									);
	if (bRetValue)
	{
		if (iReturnStatus == BUILTIN_ABORT)
		{
			return VISIT_RETURN;
		}
		else
		{
			CVariable *p_bi_rc = pSymbolTable->Find("_bi_rc");
			if (p_bi_rc != NULL)
			{
				p_bi_rc->GetValue() = *pvar;
			}

			return VISIT_NORMAL;
		}
	}
	else
	{
		return VISIT_ERROR;
	}
}

#define MAX_INT_DIGITS  10

//Anil August 26 2005 For handling DD variable and Expression
_INT32 CInterpretedVisitor::visitComplexDDExpression(
			CComplexDDExpression* pArrExp,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)
{
	assert(pvar!=0);

	_INT32 i32Idx			= pArrExp->GetToken()->GetSymbolTableIndex();
	CVariable* pVariable	= m_pMEE->m_GlobalSymTable.GetAt(i32Idx);	

	//evaluate the expressions...
	EXPR_VECTOR* pvecExpressions = pArrExp->GetExpressions();
	_INT32 i32Count = pvecExpressions->size();
	INTER_VARIANT var;	

	//Added:Anil Octobet 5 2005 for handling Method Calling Method
	//Added the code for the DD Methos Execution
	if( RUL_DD_METHOD == pVariable->GetSubType() )
	{
		// Fill the current values of all the Method Agrument
		INTERVARIANT_VECTOR vectInterVar;
		_INT32 i=0;	// WS - 9apr07 - 2005 checkin
		for(i=0;i<i32Count;i++)// WS - 9apr07 - 2005 checkin
		{
			var.Clear();
			((*pvecExpressions)[i])->Execute(this,pSymbolTable,&var);
			vectInterVar.push_back(var);		
		}	

		//Fill all the methos Agrument info like pchCallerArgName and ..._TYPE and ..._SUBTYPE
		int iParamCount = 0;
		char szDDitemName[1024];
		char pszComplexDDExpre[1024];
		std::string strDDitemName = pVariable->GetDDItemName();
		std::string strComplexDDExpre = pVariable->GetLexeme();
#ifdef MODIFIED_BY_SS
		/*strcpy(szDDitemName,(const char*)pVariable->GetDDItemName() );
		strcpy(pszComplexDDExpre,(const char*)pVariable->GetLexeme() );*/
		std::copy(strDDitemName.begin(),
			strDDitemName.end(),
			szDDitemName);
		std::copy(strComplexDDExpre.begin(),
			strComplexDDExpre.end(),
			pszComplexDDExpre); 

#endif
		unsigned NoOfParams = 0;

		METHOD_ARG_INFO_VECTOR vectMethArgInfo;
		//From here just extrac the Arg list and fill it out
		{
			int iLeftPeranthis = 0;
			bool bValidMethodCall = false;
			long int i = strlen(szDDitemName);
			//Checkk for the Valid Method call, 
			//ie Method call should Start and end with open and Close Parenthesis respectively
			for(; i < (int)strlen(pszComplexDDExpre); i++)  // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
			{
				if(pszComplexDDExpre[i] == '(')
				{
					bValidMethodCall = true;
					iLeftPeranthis =1;
					i++;
					break;
				}
			}
			if( bValidMethodCall ==  false)
			{
				return VISIT_ERROR;
			}		
			//Now strat extracting the each Argument name and push it in the strvCallerArgList vector
			int iNoOfchar = 0;
			long int lstlen = strlen(pszComplexDDExpre);
			for(; i< (lstlen) ; i++)
			{
				//Look for the space and do not count
				if( (' ' != pszComplexDDExpre[i]) )					
				{
					iNoOfchar++ ;
				}			
				//If u find ')', reduce the iLeftPeranthis and 
				//when u go out of this loop iLeftPeranthis dhould be zero		
				if(')' == pszComplexDDExpre[i])
				{
					iLeftPeranthis--;

				}
				//If u find '(', increase the iLeftPeranthis and 
				//when u go out of this loop iLeftPeranthis dhould be zero		
				if('(' == pszComplexDDExpre[i])
				{
					iLeftPeranthis++;

				}
				//if it is ',' or last ")", AND there is an arg name (no args, skip to else)
				if( ((pszComplexDDExpre[i] == ',') || (0 == iLeftPeranthis)) && (iNoOfchar >1))
				{
					//do insert here, Get the start pos of the arg name
					iParamCount++;
					//do insert here				
					int istartPosOfPassedItem = 0 ;
					iNoOfchar--;//Because ; or ) is included
					int iNoOfSpaces = 0;
					//It may so happen that for the arg there are space before , or ')'
					//EG: ( ArgnameOne                  ,   argName2        ), thats why this below loop			
					for(int x = i-1; ;x--)
					{
						if(' ' == pszComplexDDExpre[x])
						{
							iNoOfSpaces++;

						}
						else
						{
							break;
						}

					}
					//Get the strating position and Char count
					istartPosOfPassedItem = i - iNoOfchar - iNoOfSpaces ;
					int iCount = iNoOfchar + 1 ;
					char* pchDecSource = new char[ iCount ];// +1 for Null Char +1 for ; -1 for as it had counted ]
					memset(pchDecSource,0,iCount);
#ifdef MODIFIED_BY_SS
					//strncpy(pchDecSource,(const char*)&pszComplexDDExpre[istartPosOfPassedItem],iNoOfchar);
					std::copy(&pszComplexDDExpre[istartPosOfPassedItem],
						&pszComplexDDExpre[istartPosOfPassedItem + iNoOfchar], pchDecSource); // Using std::copy for string copying

#endif					
					pchDecSource[iCount - 1] = '\0';
					//Find it in Synbol table??
					//I got the Arg name from here So find it in Symbol table,
					//If it is DD item, or DD expressiom then pCVariable will be null 
					//TODO handle the DD item case
					CVariable* pCVariable = pSymbolTable->Find(pchDecSource);
					METHOD_ARG_INFO stMethArg;
					//stMethArg
					if( ( pCVariable!= NULL ) && ( ( RUL_ARRAY_VARIABLE == pCVariable->GetType() ) ))// || (RUL_SAFEARRAY == pCVariable->GetType() )))
					{
						INTER_VARIANT pvar = pCVariable->GetValue();
						vectInterVar[NoOfParams] = pCVariable->GetValue();

					}
					if(pCVariable == NULL)
					{
						//This can be a case of Constant passed or from the DD item
						//So have to Diifferenrialte between two
						//Call mee to know wether it is DD item
						bool bIsComplex = false;

						// warning C4288: nonstandard extension used : 'iCount' : loop control 
						//  variable declared in the for-loop is used outside the for-loop scope;
						//  it conflicts with the declaration in the outer scope
						// HOMZ - solution: Move int iCount outside the loop...
						// stevev..already defined higher up.... int iCount = 0;
						int acharCnt = 0;// this could identify a float constant, preclude that
						for(iCount = 0; iCount < (int)strlen(pchDecSource) ; iCount++) 
						// warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
						{
							// stevev 20feb09 - preclude float strings
							if ( ! strchr("0123456789+-eE.",pchDecSource[iCount]) )
							{// non float format char
								acharCnt++;
							}
							if( ( '.' == pchDecSource[iCount] && acharCnt > 0 ) || 
								( '[' == pchDecSource[iCount])  )
							{
								bIsComplex = true;
								break;
							}
						}
						char* szDDitemName = new char[iCount + 1];
#ifdef MODIFIED_BY_SS
				//		strncpy(szDDitemName,(const char*)&pchDecSource[0],iCount);
						std::copy(&pszComplexDDExpre[0],
							&pszComplexDDExpre[iCount], szDDitemName);
#endif
						szDDitemName[iCount] = '\0';

						//Check for the case of DD item...
						if(m_pMEE->IsDDItem(szDDitemName))
						{
							
							stMethArg.SetCallerArgName(pchDecSource);
							stMethArg.SetType( RUL_DD_ITEM );
							stMethArg.SetSubType( RUL_DD_COMPLEX );						
							vectMethArgInfo.push_back(stMethArg);
							//it may so happen that  after resolving this DD expressionn it is 
							//          again a Constant parameter
							INTER_VARIANT pvar;
							RETURNCODE iReturnValue = m_pMEE->ResolveDDExp(
								      (const char*)pchDecSource,(const char*)szDDitemName,&pvar);
							if(	iReturnValue == FAILURE)
							{
								//  it is Surely an DD item reference
							}
							else
							{
								//it may or may not DD item, it may be Constant also...refere below Example
								/*
								Eg;
								M1(DD_ITEM &X);
								M2(DD_STRING X);
								M3
								{
									M1(tag);//in this case Tag is passed as a DD item
									M2(tag);//this case Tag is passes by its value
								}
								*/

								vectInterVar[NoOfParams] = pvar;
							}


						}
						else
						{
							//This is surely a Constant Passed to Method
							//RUL_NUMERIC_CONSTANT, 
							//RUL_STR_CONSTANT,
							//RUL_CHR_CONSTANT,	
							if( NoOfParams < vectInterVar.size() )//dont go beyond the end of the array- Walt EPM 08sep08
							{
							INTER_VARIANT varTemp = vectInterVar[NoOfParams];
							stMethArg.SetCallerArgName("PassedByConstant");
							stMethArg.SetType( RUL_SIMPLE_VARIABLE );
							stMethArg.SetSubType( RUL_SUBTYPE_NONE );						
							switch(varTemp.GetVarType())
							{
								case RUL_CHAR:										
										stMethArg.SetSubType( RUL_CHAR_DECL );
										break;
								case RUL_UNSIGNED_CHAR:										
										stMethArg.SetSubType( RUL_UNSIGNED_CHAR_DECL );
										break;
									case RUL_USHORT:	
										stMethArg.SetSubType( RUL_UNSIGNED_SHORT_INTEGER_DECL );
										break;
								case RUL_SHORT:	
										stMethArg.SetSubType( RUL_SHORT_INTEGER_DECL );
										break;
								case RUL_INT:										
										stMethArg.SetSubType( RUL_INTEGER_DECL );
										break;
								case RUL_UINT:
										stMethArg.SetSubType( RUL_UNSIGNED_INTEGER_DECL );
										break;
								case RUL_LONGLONG:									
										stMethArg.SetSubType( RUL_LONG_LONG_DECL );
										break;

								case RUL_BOOL:										
										stMethArg.SetSubType( RUL_BOOLEAN_DECL );
										break;

								case RUL_FLOAT:										
										stMethArg.SetSubType( RUL_REAL_DECL );
										break;

								case RUL_ULONGLONG:
								case RUL_DOUBLE:
										stMethArg.SetSubType( RUL_DOUBLE_DECL );
										break;

								case RUL_CHARPTR:		
										stMethArg.SetSubType( RUL_DD_STRING_DECL );
										break;
									default:
										return FAILURE; //we made a bad assumption above.

							}							
							vectMethArgInfo.push_back(stMethArg);
							}
							else
							{
								return FAILURE; //we made a bad assumption above.
							}
						}
						
						if(szDDitemName)// memory leak plug emerson checkin april2013
						{
							delete szDDitemName;
							szDDitemName = NULL;

						}
						
					}
					else
					{
						stMethArg.SetCallerArgName(pchDecSource);
						stMethArg.SetType( pCVariable->GetType() );
						stMethArg.SetSubType( pCVariable->GetSubType() );
						vectMethArgInfo.push_back(stMethArg);

					}
					NoOfParams++;
					if(pchDecSource)
					{
						delete[] pchDecSource;
						pchDecSource = NULL;
					}
					iNoOfchar = 0;

				}
				//else (not ',' and not last ')') or no chars encountered so far <eg meth(,)>
				if(0 == iLeftPeranthis)// no more parameters <final ')' found>..get out
					break;
			}// next character (i) in parameter list string
		}// end of block

		//When I come here, vectMethArgInfo is filled with the Parameter that are passed to 
		//the called method  and  vectInterVar is filled with its corresponding values
		RETURNCODE iReturnValue = m_pMEE->ResolveMethodExp(
										(const char*)pVariable->GetLexeme(),
										(const char*)pVariable->GetDDItemName(),
										pvar,			&vectInterVar,			&vectMethArgInfo);
		if(iReturnValue == FAILURE)
		{
			// return FAILURE;  pwink 17JUNE'21 - substituted VISIT_RETURN  trying to abort from called method
			return VISIT_RETURN;// 
		}
		
		//Once we Execute this method, We need to again fill the values of those parameter, 
		//	which are passed by reference
		int iNoOfArgs = vectMethArgInfo.size();
		for(i =0; i<iNoOfArgs; i++)
		{	
			//Check for the Parameter which are passed by reference other than return 
			//For return it is self generated parameter for temporory purpose which are
			//	always passed by reference
			if( (vectMethArgInfo[i].ePassedType == DD_METH_AGR_PASSED_BYREFERENCE )&& 
				 !(vectMethArgInfo[i].m_IsReturnVar) )				
			{
				//Check for Simple var , in which case it is direct value assgnment
				if( RUL_SIMPLE_VARIABLE == vectMethArgInfo[i].GetType() )
				{
					CVariable* pStore=0;
					pStore = pSymbolTable->Find(vectMethArgInfo[i].GetCallerArgName());
					INTER_VARIANT fdftemp = vectInterVar[i];
					pStore->GetValue() = vectInterVar[i];			
				}//End of Simple var
				//Check for array, where ass is not direct...Assignment is by index basis
				else 
				if(RUL_ARRAY_VARIABLE == vectMethArgInfo[i].GetType())
				{

					CVariable* pVariable = 
						pSymbolTable->Find( vectMethArgInfo[i].GetCallerArgName());
					
					INTER_VARIANT* vartemp = &vectInterVar[i];

					INTER_SAFEARRAY* prgsaCalled = vartemp->GetValue().prgsa;

					//Extract the dimention and assign each of them
					vector<_INT32> vecDims;
					prgsaCalled->GetDims(&vecDims);
					_INT32 i32mem = prgsaCalled->MemoryAllocated();
					int iMemsize = i32mem/vecDims[0];
					int iArraysize = vecDims[0];
				
					 
					//Get the called and caller and assign individually
					INTER_SAFEARRAY* prgsaCaller= pVariable->GetValue().GetValue().prgsa;

					for(int iCount = 0;iCount<iArraysize ; iCount++)
					{
						INTER_VARIANT VarTemp;
						VarTemp.Clear();
						prgsaCalled->GetElement(iMemsize*iCount,&VarTemp);
						prgsaCaller->SetElement(iMemsize*iCount,&VarTemp);							
					
					}

				}//end of Array var		

			}//End of Non Return bar
			
			//Check for the return type var, Exclude the return void Statement
			if( (vectMethArgInfo[i].ePassedType == DD_METH_AGR_PASSED_BYREFERENCE )&& 
				(vectMethArgInfo[i].m_IsReturnVar)&&
				(vectMethArgInfo[i].GetSubType() != RUL_SUBTYPE_NONE)
				)
			{

				INTER_VARIANT vaTem = vectInterVar[i];
				// removed WS:EPM 17jul07  pvar->varType  = vaTem.varType;
				*pvar = vaTem;
			}//End of return var			
		}		
	}//End of Method RUL_DD_METHOD == pVariable->GetSubType()
	else
	{
		int* lTempArray = new int[i32Count];

		
		_INT32 i=0;// WS - 9apr07 - 2005 checkin
		for(i=0;i<i32Count;i++)// WS - 9apr07 - 2005 checkin
		{
			var.Clear();
			((*pvecExpressions)[i])->Execute(this,pSymbolTable,&var);
			lTempArray[i] = (int)var;		
		}
		// emerson april2013 uses pARRExp->GetToken() for its pointer
		// historically pVariable was used
#define ComplexExprHolder   pArrExp->GetToken()  /* historically pVariable */
		long int lStrlen = strlen((const char*)ComplexExprHolder->GetLexeme());

		char* szTempLexeme = new char[lStrlen+1];
		memset(szTempLexeme,0,lStrlen + 1);				

		//stevev 04jan07 - overrun if > 1 digit index
		// change char* szActualstring = new char[lStrlen+1];	
		// change memset(szActualstring,0,lStrlen + 1);
		//Anil: 050107  I would prefer declaring it as string with dynamic allocation:
		string szActualstring = "";
		//char* szActualstring = new char[lStrlen + MAX_INT_DIGITS + 1];	
		//memset(szActualstring,0,lStrlen + MAX_INT_DIGITS + 1);		
		
		//Here is slight Confusion ,
		//Funda: Complex DD Expression is Actually stored in m_pszLexeme which is got by 
		//	pVariable->GetLexeme().  Where as Actual token is Stored in m_pszComplexDDExpre 
		//	which i got by pVariable->GetDDItemName()
#ifdef MODIFIED_BY_SS			
		std::string strLexeme(ComplexExprHolder->GetLexeme());
		//strcpy(szTempLexeme,(const char*)ComplexExprHolder->GetLexeme());
		std::copy(strLexeme.begin(), strLexeme.end(), szTempLexeme);
#endif
		long int lCout = 0;
		int iNoOfBrackExpre = 0;

		for(long int iTemp =0; iTemp< lStrlen ; iTemp++)
		{			
			if(szTempLexeme[iTemp] == '[')
			{
				
				//szActualstring[lCout++] = szTempLexeme[iTemp];
				szActualstring += szTempLexeme[iTemp];
				iTemp++;
				char szBuf[MAX_INT_DIGITS + 1] ;
#ifdef linux
				strcpy(szBuf, int2str(lTempArray[iNoOfBrackExpre]));
#else
#ifdef MODIFIED_BY_SS
				//_itoa(lTempArray[iNoOfBrackExpre],szBuf,10);
				std::string str = std::to_string(lTempArray[iNoOfBrackExpre]);
				std::copy(str.begin(), str.end(), szBuf);
#endif
#endif
				//strcat(szActualstring,szBuf);
				szActualstring += szBuf;
				//strcat(szActualstring,"]");
				szActualstring += "]";
				lCout += strlen(szBuf) + 1;

				iNoOfBrackExpre++;
				long int iPos = i;
				int iLeftBrackCount = 1;
				long int iCount = 0;
				while( (iLeftBrackCount!=0) && (iTemp<lStrlen) )
				{
					if(szTempLexeme[iTemp] == '[')
						iLeftBrackCount++;
					if(szTempLexeme[iTemp] == ']')
						iLeftBrackCount--;
					iTemp++;				
				}
				iTemp--;
			}
			else
			{
				szActualstring += szTempLexeme[iTemp];
				//szActualstring[lCout++] = szTempLexeme[iTemp];
			}
		}
		// added WS:EPM 17jul07
		if( lTempArray )
		{
			delete [] lTempArray;
			lTempArray = NULL;
		}


		//szActualstring[lCout] = '\0';
		if(szTempLexeme)
		{
			delete[] szTempLexeme;
			szTempLexeme = NULL;
		}


		//Here is slight Confusion ,
		//Funda: Comple DD Expression is Actually stored in m_pszLexeme which is got by 
		//	pVariable->GetLexeme().  Where as Actual token is Stored in m_pszComplexDDExpre 
		//	which i got by pVariable->GetDDItemName()
		
		if(m_IsLValue)
		{
			m_IsLValue = false;
			RETURNCODE iReturnValue = m_pMEE->ResolveNUpdateDDExp(
														(const char*)szActualstring.c_str(),
														(const char*)ComplexExprHolder->GetDDItemName(),
														pvar,		AssignType);
			if(	iReturnValue == FAILURE)
			{
					/*if(szActualstring)
					{
						delete[] szActualstring;
						szActualstring = NULL;
					}*/
				return VISIT_ERROR;
			}
		}
		else
		{
			RETURNCODE iReturnValue = m_pMEE->ResolveDDExp(
														(const char*)szActualstring.c_str(),
														(const char*)ComplexExprHolder->GetDDItemName(),
														pvar);
			if(	iReturnValue == FAILURE)
			{
					/*if(szActualstring)
					{
						delete[] szActualstring;
						szActualstring = NULL;
					}*/
				return VISIT_ERROR;
			}
		}	

		/*if(szActualstring)
		{
			delete[] szActualstring;
			szActualstring = NULL;
		}*/
	}//end of Else for Checking for method type
	
	return  VISIT_NORMAL;
}

//Added:Anil Octobet 5 2005 for handling Method Calling Method
//Function to set the FLAG that whether it is a routine call( ie Called method), ie it is not called by the menu
void CInterpretedVisitor::SetIsRoutineFlag(bool bIsRoutine)
{
	m_bIsRoutine = bIsRoutine;
	return;
}
//Added:Anil Octobet 5 2005 for handling Method Calling Method
//To get the Routine flag
bool CInterpretedVisitor::GetIsRoutineFlag()
{
	return m_bIsRoutine;
}