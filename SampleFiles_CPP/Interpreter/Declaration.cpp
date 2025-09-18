#include "pch.h"
//#include "stdafx.h"
#include "Declaration.h"
#include "SymbolTable.h"
#include "INTER_SAFEARRAY.h"
#include <assert.h>

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

//jad vs2017
#ifdef ANDROID
	#include <cmath>
	#include <cstdlib>
#endif

void	TokenType_to_VariantType(
			RUL_TOKEN_TYPE token,
			RUL_TOKEN_SUBTYPE subtoken,
			VARIANT_TYPE& vt);

CDeclaration::CDeclaration()
{

}

CDeclaration::~CDeclaration()
{

}

_INT32 CDeclaration::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	//Anil 180107 Commented Below. The call come here where we are executing the Statement list which has declaration
	//This is a bug fix to get rid of the variable which is declared withing the scope
	//the below Dd Method code was not handled
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
	//return 0;
	return VISIT_SCOPE_VAR;
}

/*

Declaration =>	Type Id_List;
Id_List		=>	Single_Elt, Id_List | Single_Elt
Single_Elt	=>	Id Static_Dim_List | Id
Id			=>	letter(letter|digit)*

This is implemented as a State Machine.
Initial State
=============
	lboxState = rboxState = numState = commaState = false
	idState = true

Final State
===========
	lboxState = X
	rboxState = numState = idState = false
	commaState = true;
	
*/
_INT32 CDeclaration::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken = 0;
	CToken* pSymToken = 0;

	try
	{
		if((LEX_FAIL ==plexAnal->GetNextToken(&pToken,pSymbolTable))
			|| !pToken)
		{
			DELETE_PTR(pToken);//clean up memory, even on errors
			throw(C_UM_ERROR_INTERNALERR);
		}

		RUL_TOKEN_SUBTYPE SubType = pToken->GetSubType();
		DELETE_PTR(pToken);

		bool lboxState=false;
		bool rboxState = false;
		bool numState = false;
		bool idState = true;
		bool commaState = false;
		CToken* pArrToken=0;
		int dimCnt = 0; // stevev 25apr13 - we have to reuse bracket scoped arrays

		while((LEX_FAIL !=plexAnal->GetNextVarToken(&pToken,pSymbolTable,SubType))
				&& pToken 
				&&(!pToken->IsEOS()))
		{
			if((!pToken->IsSymbol()
				&& (pSymToken = pSymbolTable->Find(pToken->GetLexeme())) )
 				||  (RUL_LBOX == pToken->GetSubType())
				||  (RUL_RBOX == pToken->GetSubType())
				||	(pToken->IsNumeric()))
			{
				if(pToken->IsArrayVar())
				{
					lboxState = true;
					rboxState = false;
					numState = false;
					idState = false;
					commaState = false;

					pSymToken->SetSubType(SubType);
					//Make a copy of the array Token
					DELETE_PTR(pArrToken);
					pArrToken = new CToken(*pToken);
				}
				else if(lboxState)
				{
					if(pToken->GetSubType() != RUL_LBOX)
					{
						//ADD_ERROR(C_DECL_ERROR_LBOX);
						plexAnal->SynchronizeTo(
							DECLARATION,
							pSymbolTable);
					}
					lboxState = false;
					rboxState = true;
					numState = false;
					idState = false;
					commaState = false;
				}
				else if(rboxState)
				{
					if((0==pArrToken) || !pToken->IsNumeric())
					{
						//ADD_ERROR(C_DECL_ERROR_NUM);
						plexAnal->SynchronizeTo(
							DECLARATION,
							pSymbolTable);
					}

					lboxState = false;
					rboxState = false;
					numState  = true;
					idState   = false;
					commaState= false;
			//This is a number and with pArrToken get the symbol table token 

					_INT32 i32Idx = pArrToken->GetSymbolTableIndex(); 
					CVariable* pVar = pSymbolTable->GetAt(i32Idx);
					INTER_VARIANT& varArray = pVar->GetValue();
					VARIANT_TYPE vtSafeArray;

			//increment the dimension and set the limit of that dimension
					if(varArray.GetVarType() != RUL_SAFEARRAY
						|| dimCnt == 0                      )// stevev 25apr13
					{
						// WS:EMP-17jul07:varArray.Clear();
						// WS:EMP-17jul07:varArray.varType = RUL_SAFEARRAY;
						// WS:EMP-17jul07:__VAL& val = (__VAL&)varArray.GetValue();
						// stevev-14feb08:make it more flexible...INTER_SAFEARRAYBOUND rgbound[1] = {atoi(pToken->GetLexeme())};
						INTER_SAFEARRAYBOUND rgbound[1] = { strtoul(pToken->GetLexeme(),NULL,0) };
						INTER_SAFEARRAY sa; // WS:EMP-17jul07 was::>val.prgsa = new INTER_SAFEARRAY();
						_USHORT cDims = 0;  // WS:EMP-17jul07 was::>_USHORT cDims = (val.prgsa)->GetDims();

						TokenType_to_VariantType(pToken->GetType(),	SubType, vtSafeArray);
							
						// WS:EMP-17jul07 was::>(val.prgsa)->SetAllocationParameters(vtSafeArray, ++cDims, rgbound);
						sa.SetAllocationParameters(vtSafeArray, ++cDims, rgbound);
						sa.Allocate(); // stevev 11jun09 - get rid of error message, destructor will deallocate
						varArray = &sa; // added WS:EMP-17jul07
						dimCnt = 1;
					}
					else// isa RUL_SAFEARRAY && dimCnt > 0
					{
						__VAL& val = (__VAL&)varArray.GetValue();
						INTER_SAFEARRAYBOUND rgbound[1] = { strtoul(pToken->GetLexeme(),NULL,0) };
						(val.prgsa)->AddDim(rgbound);
						dimCnt++;
					}
					
				}
				else if(numState)
				{
					if(pToken->GetSubType() != RUL_RBOX)
					{
						//ADD_ERROR(C_DECL_ERROR_RBOX);
						plexAnal->SynchronizeTo(
							DECLARATION,
							pSymbolTable);
					}

					//accept a Right box.
					lboxState = true;
					rboxState = false;
					numState = false;
					commaState = true;
					idState	= false;
				}
				else
				{
					if(idState)
					{
						if(pToken->GetType() != RUL_SIMPLE_VARIABLE)
						{
							//ADD_ERROR(C_DECL_ERROR_IDMISSING);
							plexAnal->SynchronizeTo(
								DECLARATION,
								pSymbolTable);
						}
						pSymToken->SetSubType(SubType);
					}
					else
					{
						//ADD_ERROR(C_DECL_ERROR_COMMAMISSING);
						plexAnal->SynchronizeTo(
							DECLARATION,
							pSymbolTable);
					}
					lboxState = false;
					rboxState = false;
					numState = false;
					idState = false;
					commaState = true;
				}
			}
			else if(commaState)
			{
				if(pToken->GetSubType() != RUL_COMMA)
				{
					//ADD_ERROR(C_DECL_ERROR_COMMAMISSING);
					plexAnal->SynchronizeTo(
						DECLARATION,
						pSymbolTable);
				}

				DELETE_PTR(pArrToken);
				idState = true;
				commaState = false;
				lboxState = false;
				rboxState = false;
				numState = false;
			}
			else
			{
			//Of course, this is a problem case. 
			//Unfortunately, expressions in the declarations are not handled
				//ADD_ERROR(C_DECL_ERROR_EXPRESSION);
				plexAnal->SynchronizeTo(DECLARATION,
					pSymbolTable);

				//accept a Right box. //VMKP added on 030404
				/*  Synchronizing was not proper when an expression
				 present in the variable declaration, With that the 
				 below lines one declaration next to the expression
				 declaration is skipping */
				lboxState = true;
				rboxState = false;
				numState = false;
				commaState = true;
				idState	= false;
			}
			DELETE_PTR(pToken);//delete token within the while loop
		}//end of while loop

//Validate the exit criteria...
		if(!(rboxState == numState == idState == false)
			|| !(commaState == true))
		{
			//ADD_ERROR(C_DECL_ERROR_UNKNOWN);
			plexAnal->SynchronizeTo(DECLARATION,
				pSymbolTable);
		}

		DELETE_PTR(pToken);//delete the token outside the while loop
		DELETE_PTR(pArrToken);//delete the token outside the while loop
		return PARSE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
		plexAnal->MovePast(
			RUL_SYMBOL,
			RUL_SEMICOLON,
			pSymbolTable);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR);
	}

	return PARSE_FAIL;
}

void CDeclaration::Identify(
		_CHAR* szData)
{
}

_INT32 CDeclaration::GetLineNumber()
{
	return -1;
}
