//#include "stdafx.h"

//#if _MSC_VER >= 1300  // HOMZ - port to 2003, VS7
//stevev 20feb07-merge- contents moved to ddbGeneral #include "..\DevServices\stdafx.h"   // HOMZ
//#endif
#include "pch.h"
#include "ddbGeneral.h"	// stevev 20feb07 - merge to get rid of 'stdafx.h'
/* comutil.h uses Bill's TRUE/FALSE that the general erroneously defines on purpose */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

#include "FunctionExpression.h"
#include "GrammarNodeVisitor.h"
#include "ErrorDefinitions.h"
#include "ExpParser.h"
#if _MSC_VER < 1400	/* stevev 23feb10 - 2005 mods, these are not needed */
//#include <comdef.h>
//#include "OMServiceExpression.h"
//#include "SymbolTable.h"

#endif
FunctionExpression::FunctionExpression()
{
	m_pchFunctionName =	0;
	m_i32ParameterCount = -1;
	/*<START>TSRPRASAD 09MAR2004 Fix the memory leaks	*/
	for (int iLoopVar = 0;iLoopVar < MAX_NUMBER_OF_FUNCTION_PARAMETERS;iLoopVar++)
	{
		m_pExpression[iLoopVar] = NULL;
		m_pConstantTokens[iLoopVar] = NULL;
	}
	/*<END>TSRPRASAD 09MAR2004 Fix the memory leaks	*/
}

FunctionExpression::~FunctionExpression()
{
	if(m_pchFunctionName)
	{
		delete [] m_pchFunctionName;
		m_pchFunctionName = 0;
	}
	
	/*<START>TSRPRASAD 09MAR2004 Fix the memory leaks	*/
	for (int iLoopVar = 0;iLoopVar < m_i32ParameterCount;iLoopVar++)
	{
		DELETE_PTR(m_pExpression[iLoopVar]);
		DELETE_PTR(m_pConstantTokens[iLoopVar]);
	}
	/*<END>TSRPRASAD 09MAR2004 Fix the memory leaks	*/

	m_i32ParameterCount = -1;
}
#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 FunctionExpression::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitFunctionExpression(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif
_INT32 FunctionExpression::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken=0;
	try
	{
//Munch a <FUNCTION NAME>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsFunctionToken())
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}

		m_pchFunctionName = new _UCHAR[strlen(pToken->GetLexeme()) + 1];
#ifdef MODIFIED_BY_SS		
		// strcpy((char *)m_pchFunctionName, pToken->GetLexeme());

		std::string result(pToken->GetLexeme());
		std::copy(result.begin(), result.end(), m_pchFunctionName );
		m_pchFunctionName[result.size()] = '\0';
#endif
		/* Now get the details of the function */
		Function_Signatures Func;
		if (GetFunctionDetails(pToken,&Func) == 0)
		{
#ifdef GE_BUILD
			MessageBox(NULL,_T("Function name Not Found"), NULL, 0);	// PAW 03/03/09 _T added
#endif 
			return PARSE_FAIL;		
		}
		
		DELETE_PTR(pToken);

//Munch a <(>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (RUL_SYMBOL != pToken->GetType())
			|| RUL_LPAREN != pToken->GetSubType())
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGLPAREN);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

		/* Now get the parameters */
		m_i32ParameterCount = Func.iNumberOfParameters;

		for (int iLoopVar = 0; iLoopVar < (int)Func.iNumberOfParameters;iLoopVar++)  // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
		{
			m_pTokenType[iLoopVar] = Func.piParameterType[iLoopVar];
			switch(Func.piParameterType[iLoopVar])
			{
				case RUL_NUMERIC_CONSTANT:
				case RUL_SIMPLE_VARIABLE:
				case RUL_ARRAY_VARIABLE:
				case RUL_DD_ITEM:			//Vibhor 140705: Added
				{
					bool bParenPresent = false;
					/* Check if there is a <(>*/
					/*if((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)) 
						&& pToken
						&& RUL_SYMBOL == pToken->GetType()
						&& RUL_LPAREN == pToken->GetSubType())
					{
						bParenPresent = true;
					}
					else
					{
						plexAnal->UnGetToken();
					}
					DELETE_PTR(pToken);*/

					CExpParser expParser;
					m_pExpression[iLoopVar]
						= expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_WHILE);
					if(!m_pExpression[iLoopVar])
					{
						//ADD_ERROR(C_WHILE_ERROR_MISSINGEXP);
					}

					/*if <(> was present, check for <)>*/
					if (bParenPresent)
					{
						/* Check if there is a <(>*/
						if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
							|| !pToken
							|| (RUL_SYMBOL != pToken->GetType())
							|| RUL_RPAREN != pToken->GetSubType())
						{
							plexAnal->UnGetToken();
						}
						DELETE_PTR(pToken);
					}

					break;
				}
				case RUL_STR_CONSTANT:
				{
					bool bParenPresent = false;
					bool bEnterWhile = true;
					int iCountLeftParenthisis = 0;
					/* Check if there is a <(>*/
					//Anil 16 November 2005
					//This is for handling the Multiplle left parathissi come oin the Built in calls
					while(bEnterWhile)
					{
						if((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)) 
							&& pToken
							&& RUL_SYMBOL == pToken->GetType()
							&& RUL_LPAREN == pToken->GetSubType())
						{
							bParenPresent = true;
							iCountLeftParenthisis++;
							bEnterWhile = true;
						}
						else
						{
							plexAnal->UnGetToken();
							bEnterWhile = false;
						}
						DELETE_PTR(pToken);
					}

					if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
						|| !pToken
						|| !pToken->IsConstant())
					{
						if (pToken->IsVariable())
						{
							plexAnal->UnGetToken();
							m_pTokenType[iLoopVar] = RUL_ARRAY_VARIABLE;
							CExpParser expParser;
							m_pExpression[iLoopVar]
								= expParser.ParseExpression
													(
														plexAnal
														, pSymbolTable
														, EXPR_WHILE
													);
							if(!m_pExpression[iLoopVar])
							{
								//ADD_ERROR(C_WHILE_ERROR_MISSINGEXP);
							}
						}
						// stevev 30jan08 - added to handle function-as-string
						else
						if (pToken->IsFunctionToken())
						{
							plexAnal->UnGetToken();
							m_pTokenType[iLoopVar] = RUL_ARRAY_VARIABLE;// may need RUL_STR_CONSTANT
							CExpParser expParser;
							m_pExpression[iLoopVar]
								= expParser.ParseExpression
													(
														plexAnal
														, pSymbolTable
														, EXPR_WHILE
													);
							if(!m_pExpression[iLoopVar])
							{
								//ADD_ERROR(C_WHILE_ERROR_MISSINGEXP);
							}
						}
						// else - we don't handle other possibilities...
						DELETE_PTR(pToken);
					}
					else
					{
						m_pConstantTokens[iLoopVar] = new CToken;
						*m_pConstantTokens[iLoopVar] = *pToken;

						DELETE_PTR(pToken);
					}

					/*if <(> was present, check for <)>*/
					if (bParenPresent)
					{
						/* Check if there is a <(>*/
						//Anil 16 November 2005						
						//This is for handling the Multiplle left parathissi come oin the Built in calls					
						for(int iCount = 0;  iCount<iCountLeftParenthisis ; iCount++)
						{
							if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
								|| !pToken
								|| (RUL_SYMBOL != pToken->GetType())
								|| RUL_RPAREN != pToken->GetSubType())
							{
								plexAnal->UnGetToken();
							}
							DELETE_PTR(pToken);
						}
					}
					break;
				}
				default:
					return PARSE_FAIL;
					break;
			}
//Munch a <,>
			if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
				|| !pToken
				|| (RUL_SYMBOL != pToken->GetType())
				|| RUL_COMMA != pToken->GetSubType())
			{
				//ADD_ERROR(C_RS_ERROR_MISSINGLPAREN);
				plexAnal->UnGetToken();
			}
			DELETE_PTR(pToken);
		}// next function parameter

//Munch a <)>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (RUL_SYMBOL != pToken->GetType())
			|| RUL_RPAREN != pToken->GetSubType())
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGLPAREN);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

/*		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (RUL_SYMBOL != pToken->GetType())
			|| RUL_SEMICOLON != pToken->GetSubType())
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGLPAREN);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);*/

		return PARSE_SUCCESS;

	}
	catch (...)
	{

	}

	return 0;
}

void FunctionExpression::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,(const char*)m_pchFunctionName);
	strcat(szData,">");

	strcat(szData,"</");
	strcat(szData,(const char*)m_pchFunctionName);
	strcat(szData,">");*/
	// Convert m_pchFunctionName to a std::string
	std::string functionName(reinterpret_cast<const char*>(m_pchFunctionName));

	// Append opening tag to szData
	std::string openingTag = "<" + functionName + ">";
	std::copy(openingTag.begin(), openingTag.end(), szData);
	szData[openingTag.size()] = '\0'; // Null-terminate the string

	// Append closing tag to szData
	std::string closingTag = "</" + functionName + ">";
	std::copy(closingTag.begin(), closingTag.end(), szData + openingTag.size());
	szData[openingTag.size() + closingTag.size()] = '\0';
#endif
}

//#include "RuleOMItfExports.h"
//#include "RuleObjMgrInterface.h"
//#include "OMObject.h"

_INT32 OIDConvertIntToChar2(
			_INT32 i32OID, 
			_CHAR* chOID)
{
	union unionOID
	{
		_CHAR				chOID[5];
		struct strOID
		{
			_INT32			nOID;
			_UCHAR			chOID;
		}sOID;
	};
	unionOID uOID;
	uOID.sOID.nOID	=	i32OID;
	uOID.sOID.chOID	=	0;
	memcpy(chOID,uOID.chOID,5);

	return 0;
}

_INT32 FunctionExpression::GetLineNumber()
{
	return -1;
}

_INT32 FunctionExpression::GetFunctionDetails(
											  CToken *pToken
											  , Function_Signatures *pFunc
											  )
{
	int iNumberOfFunctions = sizeof(functionsDefs)/sizeof(Function_Signatures);

	for (int iLoopVar = 0;iLoopVar < iNumberOfFunctions;iLoopVar++)
	{
		if (strcmp(pToken->GetLexeme(), functionsDefs[iLoopVar].szWord) == 0)
		{
			*pFunc = (Function_Signatures)functionsDefs[iLoopVar];
			return 1;
		}
	}
	return 0;
}

