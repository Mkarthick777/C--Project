
#pragma warning (disable : 4786)
//#include "stdafx.h"
#include "pch.h"
#include "LexicalAnalyzer.h"
#include "SymbolTable.h"
#include "Variable.h"
//#include "RIDEError.h"
#include <ctype.h>

#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

#include "MEE.h"

//jad vs2017
#ifdef ANDROID
	#include <cmath>
	#include <cstdlib>
#endif

CLexicalAnalyzer::CLexicalAnalyzer()
{
	m_i32CurrentPos		=	0;
	m_i32PrevPos		=	0;
	m_i32LAPosition		=	0;
	m_i32LineNo			=	1;
	m_pvecErr			=	0;
	m_i32CurLineNo		=	1;
	m_i32PrevLineNo		=	1;
	m_pMEE = NULL;
	m_nSymbolTableScopeIndex=0;//SCR26200 Felix
	m_nLastSymbolTableScopeIndex=0;
	m_nPrevSymbolTableScopeIndex=0;
}

CLexicalAnalyzer::~CLexicalAnalyzer()
{
	m_pvecErr			=	0;
}

_INT32 CLexicalAnalyzer::nextToken(
			CToken**	ppToken,
			_INT32&		i32NewPos,
			_CHAR*		pszBuffer
			,bool       isLookAhead )// stevev 25apr13
{
	const _CHAR*		pszSource = (const _CHAR*)m_pszSource;
	RUL_TOKEN_TYPE		Type;
	RUL_TOKEN_SUBTYPE	SubType;
	bool				bIsFound = false;
	bool				bIsObject_or_Service = false;
	_INT32				i=0;
	COMPOUND_DATA		CmpData; memset(&CmpData,0,sizeof(COMPOUND_DATA));

	try
	{
		for(i = m_i32CurrentPos; (pszSource[i]) && !bIsFound; )
		{
	//munch all prepended white space of a token...
			if(isSpace(pszSource,i,Type,SubType,pszBuffer))
			{
				;
			}
			else if(isTerminal(pszSource,i,Type,SubType,pszBuffer))
			{
				if(Type == RUL_COMMENT)
				{
					while(pszSource[i] && pszSource[i++] != '\n')
						;
					continue;
				}

				char pchBuffer1[100];
#ifdef MODIFIED_BY_SS
				std::string strTemp(pszBuffer);
				//strcpy(pchBuffer1, pszBuffer);
				std::copy(strTemp.begin(), strTemp.end(), pchBuffer1);
				pchBuffer1[strTemp.length() - 1] = '\0';
#endif
				if ((Type == RUL_SYMBOL) && (SubType == RUL_DOT))
				{
					if(isNumber(pszSource,i,Type,SubType,pszBuffer))
					{
						//Get the numeric constant...
						
#ifdef MODIFIED_BY_SS
						/*strcat (pchBuffer1, pszBuffer);
						strcpy(pszBuffer,pchBuffer1);*/
						strTemp.assign(pchBuffer1);
						strTemp.append(pszBuffer);
						std::copy(strTemp.begin(), strTemp.end(), pszBuffer);
						pszBuffer[strTemp.length() - 1] = '\0';
#endif
						bIsFound = true;
						break;
					}
				}
				//SCR26200 Felix
				else if (pchBuffer1[0] == LCUR) 
				{
					if ( ! isLookAhead )// stevev 25apr13
					{
					this->m_nSymbolTableScopeIndex++;
					}
					pchBuffer1[0]=' ';
#ifdef hold_4_stevev
					if 	(m_nSymbolTableScopeIndex==m_nLastSymbolTableScopeIndex)
					{
						m_nSymbolTableScopeIndex++;			
						m_nLastSymbolTableScopeIndex=m_nSymbolTableScopeIndex;
					}

					if 	(m_nSymbolTableScopeIndex>m_nLastSymbolTableScopeIndex)
					{
						m_nLastSymbolTableScopeIndex=m_nSymbolTableScopeIndex;	//save the last highest Scope index
					}
#endif // hold4stevev

				}
				else if (pchBuffer1[0] == RCUR) 
				{
					if (this->m_nSymbolTableScopeIndex >= 1 && ! isLookAhead)
					{// we need to unscope variables at this->m_nSymbolTableScopeIndex
						this->m_nSymbolTableScopeIndex--;
					}
					pchBuffer1[0]=' ';
				}

				bIsFound = true;
	//The next operator is Unary + or - operator if the current Token is 
	//a.	Operator
	//b.	Symbol
	//c.	None
				if( (m_CurToken.GetType() == RUL_TYPE_NONE)
					|| (m_CurToken.IsOperator()) 
					|| (m_CurToken.IsSymbol() && m_CurToken.GetSubType() != RUL_RPAREN
							 && m_CurToken.GetSubType() != RUL_RBOX)
					|| (m_CurToken.GetType() == RUL_KEYWORD) 
					)
				{
					switch(SubType)
					{
					case RUL_PLUS:
						SubType = RUL_UPLUS;
						bIsFound = true;
						break;
					case RUL_MINUS:
						SubType = RUL_UMINUS;
						bIsFound = true;
						break;
					case RUL_PLUS_PLUS:
						SubType = RUL_PRE_PLUS_PLUS;
						bIsFound = true;
						break;
					case RUL_MINUS_MINUS:
						SubType = RUL_PRE_MINUS_MINUS;
						bIsFound = true;
						break;
					}
				}
			}
			else if(isNumber(pszSource,i,Type,SubType,pszBuffer))
			{
				//Get the numeric constant...
				bIsFound = true;
			}
			else if(isString(pszSource,i,Type,SubType,pszBuffer))
			{
				//Get the string constant...
				bIsFound = true;
			}
			else if(isChar(pszSource,i,Type,SubType,pszBuffer))
			{
				//Get the char constant...
				bIsFound = true;
			}
			else if(isService(pszSource,i,Type,SubType,pszBuffer,CmpData))
			{
				bIsFound = true;
				bIsObject_or_Service = true;
			}
			else if(isIdentifier(pszSource,i,Type,SubType,pszBuffer))
			{
				if(pszSource[i] == LBOX)
				{
					Type = RUL_ARRAY_VARIABLE;
				}
				else
				{
					int iSavedPos = i;
					while (pszSource[iSavedPos] == ' ')
					{
						iSavedPos++;
					}
					if(pszSource[iSavedPos] == LBOX)
					{
						Type = RUL_ARRAY_VARIABLE;
						i = iSavedPos;
					}
				}
				bIsFound = true;
			}
			else
			{
				i++;
				bIsFound = true;
				Type = RUL_TYPE_ERROR;
				SubType = RUL_SUBTYPE_ERROR;

				throw(C_LEX_ERROR_ILLEGALCHAR);
			}
		}
		if(bIsFound)
		{
			i32NewPos = i;
			if(bIsObject_or_Service)
				return Tokenize(i,Type,SubType,ppToken,pszBuffer,CmpData);
			return Tokenize(i,Type,SubType,ppToken,pszBuffer);
		}
	}
	catch(CRIDEError* perr)
	{					
		i32NewPos = i;
		*ppToken = 0;
		throw perr;
	}
	catch(...)	
	{								
		i32NewPos = i;
		throw(C_UM_ERROR_UNKNOWNERROR);
	}

	return LEX_FAIL;
}

_INT32 CLexicalAnalyzer::nextAnyToken(
			CToken**	ppToken,
			_INT32&		i32NewPos,
			_CHAR*		pszBuffer)
{
	const _CHAR*		pszSource = (const _CHAR*)m_pszSource;
	RUL_TOKEN_TYPE		Type;
	RUL_TOKEN_SUBTYPE	SubType;
	bool				bIsFound = false;
	bool				bIsObject_or_Service = false;
	_INT32				i=0;
	COMPOUND_DATA		CmpData; memset(&CmpData,0,sizeof(COMPOUND_DATA));

	try
	{
		for(i = m_i32CurrentPos; (pszSource[i]) && !bIsFound; )
		{
	//munch all prepended white space of a token...
			if(isSpace(pszSource,i,Type,SubType,pszBuffer))
			{
				;
			}
			else if(isTerminal(pszSource,i,Type,SubType,pszBuffer))
			{
				bIsFound = true;
			}
			else
			{
				i++;
				bIsFound = true;
				Type = RUL_TYPE_ERROR;
				SubType = RUL_SUBTYPE_ERROR;
			}
		}
		if(bIsFound)
		{
			i32NewPos = i;
			if(bIsObject_or_Service)
				return Tokenize(i,Type,SubType,ppToken,pszBuffer,CmpData);
			return Tokenize(i,Type,SubType,ppToken,pszBuffer);
		}
	}
	catch(CRIDEError* perr)
	{					
		i32NewPos = i;
		*ppToken = 0;
		throw perr;
	}
	catch(...)	
	{								
		i32NewPos = i;
		throw(C_UM_ERROR_UNKNOWNERROR);
	}

	return LEX_FAIL;
}

_INT32 CLexicalAnalyzer::LookAheadToken(
			CToken** ppToken)
{
	try
	{
		_INT32	i32NewPos=0;
		_CHAR	szBuffer[BUFFER_SIZE];

		_INT32 i32Ret = nextToken(ppToken,i32NewPos,szBuffer, true);//stevev 25apr13
		m_i32LineNo = m_i32CurLineNo;

		return i32Ret;
	}
	catch(CRIDEError* perr)	
	{						
		m_pvecErr->push_back(perr);
	}
	catch(...)	
	{								
		throw(C_UM_ERROR_UNKNOWNERROR);
	}

	return LEX_FAIL;
}

_BOOL CLexicalAnalyzer::ScanLineForToken(
								RUL_TOKEN_TYPE tokenType
								, RUL_TOKEN_SUBTYPE tokenSubType
								, CToken** ppToken
								)
{
	_INT32	i32NewPos=0, i32CurrentPos;
	_INT32	iPreviousCurrentPos = m_i32CurrentPos;

	_CHAR	pszRulString[BUFFER_SIZE];

	/* Get the rule string */
	if (!GetRulString(tokenType, tokenSubType, pszRulString))
	{
		return false;
	}

	i32CurrentPos = i32NewPos;
	char *pSubStringStartAddress =                      // WS - 9apr07 - VS2005 checkin
				(char*)strstr((const char*)&m_pszSource[m_i32CurrentPos], pszRulString);

	/* Check if it is a == operator */
	if ((tokenSubType == RUL_ASSIGN) && (pSubStringStartAddress != NULL))
	{
		if ((*pSubStringStartAddress == '=') && (*(pSubStringStartAddress + 1) == '='))
		{
			pSubStringStartAddress = NULL;
		}
		else
		if ((*pSubStringStartAddress == '=') && (*(pSubStringStartAddress - 1) == '>'))
		{
			pSubStringStartAddress = NULL;
		}
		else
		if ((*pSubStringStartAddress == '=') && (*(pSubStringStartAddress - 1) == '<'))
		{
			pSubStringStartAddress = NULL;
		}
		else
		if ((*pSubStringStartAddress == '=') && (*(pSubStringStartAddress - 1) == '!'))
		{
			pSubStringStartAddress = NULL;
		}
	}

	if (pSubStringStartAddress == NULL)
	{
		return false;
	}

	char *pSubEOSStartAddress = NULL;

	pSubEOSStartAddress =    // WS - 9apr07 - VS2005 checkin
		(char*)strstr((const char*)&m_pszSource[m_i32CurrentPos], ")");
	if (pSubEOSStartAddress)
	{
		char *pSubEOSStartAddressOpenBracket =  // WS - 9apr07 - VS2005 checkin
			(char*)strstr((const char*)&m_pszSource[m_i32CurrentPos], "(");
		if (pSubEOSStartAddressOpenBracket != NULL)
		{
			if (pSubEOSStartAddressOpenBracket < pSubEOSStartAddress)
			{
				pSubEOSStartAddress = NULL;
			}
		}
	}

	char *pSubSemiColonStartAddress;
	pSubSemiColonStartAddress =                // WS - 9apr07 - VS2005 checkin
				(char*)strstr((const char*)&m_pszSource[m_i32CurrentPos], ";");
	if (pSubSemiColonStartAddress != NULL) 
	{
		if (pSubEOSStartAddress != NULL)
		{
			if (pSubSemiColonStartAddress < pSubEOSStartAddress)
			{
				pSubEOSStartAddress = pSubSemiColonStartAddress;
			}
		}
		else
		{
			pSubEOSStartAddress = pSubSemiColonStartAddress;
		}
	}

	if (pSubEOSStartAddress != NULL)
	{
		if (pSubStringStartAddress > pSubEOSStartAddress)
		{
			return false;
		}
	}

	char *pSubDoubleQuoteStartAddress = NULL;
	pSubDoubleQuoteStartAddress =               // WS - 9apr07 - VS2005 checkin
				(char*)strstr((const char*)&m_pszSource[m_i32CurrentPos], "\"");
	if ( (pSubDoubleQuoteStartAddress != NULL) 
		&& (pSubDoubleQuoteStartAddress < pSubStringStartAddress)
		)
	{
		char *pSubDoubleQuoteEndAddress = NULL;
		pSubDoubleQuoteEndAddress =               // WS - 9apr07 - VS2005 checkin
				(char*)strstr((const char*)pSubDoubleQuoteStartAddress + 1, "\"");
		if (pSubDoubleQuoteEndAddress > pSubStringStartAddress)
		{
			/* The symbol is within a string */
			return false;
		}
	}

	if (pSubStringStartAddress != NULL)
	{
		int iReturnValue = TokenizeWithoutSave
		(
			0
			, tokenType
			, tokenSubType
			, ppToken
			, (char *)pszRulString
		);

		if (iReturnValue)
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}

_INT32 CLexicalAnalyzer::GetNextToken(
			CToken**		ppToken,
			CSymbolTable*	pSymbolTable)
{
	_INT32	i32NewPos=0;
	try
	{
		_CHAR	szBuffer[BUFFER_SIZE];
		if(LEX_FAIL != nextToken(ppToken,i32NewPos,szBuffer))
		{
			SaveState(i32NewPos);

#ifdef _FULL_RULE_ENGINE
		//Before u exit, make an entry in the Symbol Table
			if((*ppToken)->IsVariable()
				|| (*ppToken)->IsArrayVar())
			{
/*Vibhor 010705: Start of Code*/
/*Following change will prevent an undeclared variable to be used in a method
& consequently the Interpreter will throw a parsing error.

If we come here with a Variable token, and the same is not available in symbol Table
we possibly have a Global (DD) Variable. In that case, search it in the device and add
it to the Global symbol table. If its not found there too send a LEX_FAIL to the caller.

*/
				//_INT32 i32Idx = pSymbolTable->Insert(**ppToken);
				//_INT32 i32Idx = pSymbolTable->GetIndex((*ppToken)->GetLexeme()); 
				_INT32 i32Idx = pSymbolTable->GetIndex((*ppToken)->GetLexeme(),this->GetSymbolTableScopeIndex()); //SCR26200 Felix
				if(-1 == i32Idx)
				{
					//Anil August 26 2005 For handling DD variable and Expression
					bool bIsVariableItem = true; //default is variable
					//Anil Octobet 5 2005 for handling Method Calling Method
					DD_ITEM_TYPE DDitemType;		
					i32Idx = m_pMEE->FindGlobalToken((*ppToken)->GetLexeme(),GetLineNumber(),DDitemType);			
					if(-1 != i32Idx)
					{					
						UnGetToken();
						(*ppToken)->m_bIsGlobal = true;
						(*ppToken)->SetSymbolTableIndex(i32Idx);
						(*ppToken)->SetType   (m_pMEE->m_GlobalSymTable.GetAt(i32Idx)->GetType());
						(*ppToken)->SetSubType(m_pMEE->m_GlobalSymTable.GetAt(i32Idx)->GetSubType());
						char* szDotExpression = NULL;
						// stevev 16mar09 - method calling item other than method....  eg    menuItem();// makes a mess
						if (m_pMEE->m_GlobalSymTable.GetAt(i32Idx)->GetSubType() != RUL_DD_METHOD && 
							m_pszSource[i32NewPos] == '(' )
						{
							(*ppToken)->SetType   (RUL_TYPE_ERROR);
							(*ppToken)->SetSubType(RUL_SUBTYPE_ERROR);
							LOGIT(CERR_LOG|CLOG_LOG|UI_LOG,"ERROR: calling a non-method item.\n");

						//	throw(C_LEX_ERROR_ILLEGALITEM);
						}
						else
						//Anil Octobet 5 2005 for handling Method Calling Method
						if(GetComplexDotExp((i32NewPos),&szDotExpression,DDitemType))
						{
							char* szFullLexeme = NULL;
							bool bResetGlobalSymbolTable = true;// emerson checkin april2013
							if(szDotExpression)
							{
								i32NewPos+= strlen(szDotExpression);
								szFullLexeme = new char[ strlen((*ppToken)->GetLexeme()) + strlen(szDotExpression) + 1];
								(*ppToken)->SetDDItemName((*ppToken)->GetLexeme());
								strcpy(szFullLexeme,(*ppToken)->GetLexeme());
								strcat(szFullLexeme,szDotExpression);
								(*ppToken)->SetLexeme((const char*)szFullLexeme);

                                delete [] szDotExpression;      //WHS - June5-2007 - plug memory leak

                                szDotExpression = NULL;         //WHS - June5-2007 - plug memory leak

								if(DDitemType == DD_ITEM_VAR )// emerson checkin april2013
								{	
									bResetGlobalSymbolTable = false; // do reset the global symbols table for this
								}
							}
							else
							{
								(*ppToken)->SetDDItemName((*ppToken)->GetLexeme());

							}
							// bResetGlobalSymbolTable qualifiers //adde @ emerson checkin april2013
							if((*ppToken)->GetLexeme() != NULL  && bResetGlobalSymbolTable)
							{
								m_pMEE->m_GlobalSymTable.GetAt(i32Idx)->SetLexeme((*ppToken)->GetLexeme());
							}
							if((*ppToken)->GetDDItemName() != NULL  && bResetGlobalSymbolTable)
							{
								m_pMEE->m_GlobalSymTable.GetAt(i32Idx)->SetDDItemName((*ppToken)->GetDDItemName());
							}
							SaveState(i32NewPos);								
							CToken* pTokenTemp=0;
							//Anil Octobet 5 2005 for handling Method Calling Method
							//Changed the Condition for DD var and DD method
							if(DDitemType == DD_ITEM_VAR)
							{
								Tokenize(i32NewPos,RUL_DD_ITEM,RUL_DD_SIMPLE,&pTokenTemp,(char*)(*ppToken)->GetLexeme());
							}
							//Anil Octobet 5 2005 for handling Method Calling Method
							else if(DDitemType == DD_ITEM_NONVAR)
							{
								Tokenize(i32NewPos,RUL_DD_ITEM,RUL_DD_COMPLEX,&pTokenTemp,(char*)(*ppToken)->GetLexeme());
							}
							else if(DDitemType == DD_ITEM_METHOD)
							{
								Tokenize(i32NewPos,RUL_DD_ITEM,RUL_DD_METHOD,&pTokenTemp,(char*)(*ppToken)->GetLexeme());
							}
							DELETE_ARR( szFullLexeme );
							DELETE_PTR(pTokenTemp);//clean up memory
						}//get complex dot was false
					}
					else
					{
						LOGIT(CERR_LOG|CLOG_LOG|UI_LOG,"Method Parsing Error: Identifier '%s' "
						" was not found.  (%s:Line %d)\n",
						(*ppToken)->GetLexeme(),m_pMEE->methodNameString.c_str(),GetLineNumber());
						return LEX_FAIL;
					}

				}
				else// global token not found i32Idx == -1
				{
					//SCR26200 Felix
					(*ppToken)->SetSymbolTableIndex(i32Idx);
					(*ppToken)->SetSymbolTableScopeIndex(this->GetSymbolTableScopeIndex());		//SCR26200 Felix
				(*ppToken)->SetSubType(pSymbolTable->GetAt(i32Idx)->GetSubType());
				}
/*Vibhor 010705: End of Code*/
			}
			else if(RUL_STRING_CONSTANT == (*ppToken)->GetSubType())
			{
				_INT32 i32Idx = pSymbolTable->InsertConstant(**ppToken);
				if( i32Idx >= 0 )// emerson checkin april2013
				{
					(*ppToken)->SetConstantIndex(i32Idx);
				}
				else
                {
                    LOGIT(CERR_LOG,"Unable to insert string constant into symbol table\n");
                }
			}
			else if(RUL_CHAR_CONSTANT == (*ppToken)->GetSubType())
			{
				_INT32 i32Idx = pSymbolTable->InsertConstant(**ppToken);
				if( i32Idx >= 0 )// emerson checkin april2013
				{
					(*ppToken)->SetConstantIndex(i32Idx);
				}
				else
                {
                    LOGIT(CERR_LOG,"Unable to insert char constant into symbol table\n");
                }
			}
#endif
			return LEX_SUCCESS;
		}
	}
	catch(CRIDEError* perr)
	{					
		SaveState(i32NewPos);
		m_pvecErr->push_back(perr);
	}
	catch(...)
	{								
		SaveState(i32NewPos);
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return LEX_FAIL;
}

_INT32 CLexicalAnalyzer::GetNextVarToken(
			CToken**		ppToken,
			CSymbolTable*	pSymbolTable,
			RUL_TOKEN_SUBTYPE SubType)
{
	_INT32	i32NewPos=0;
	try
	{
		_CHAR	szBuffer[BUFFER_SIZE];
		if(LEX_FAIL != nextToken(ppToken,i32NewPos,szBuffer))
		{
			SaveState(i32NewPos);

#ifdef _FULL_RULE_ENGINE
		//Before u exit, make an entry in the Symbol Table
			if((*ppToken)->IsVariable())
			{
				(*ppToken)->SetSubType(SubType);
				_INT32 i32Idx = pSymbolTable->Insert(**ppToken,this->GetSymbolTableScopeIndex());
				if( i32Idx >= 0 )//check if a valid index before crashing.
				{
				(*ppToken)->SetSymbolTableIndex(i32Idx);
				(*ppToken)->SetSymbolTableScopeIndex(this->GetSymbolTableScopeIndex());
				(*ppToken)->SetSubType(pSymbolTable->GetAt(i32Idx)->GetSubType());
				}
				else
                {
                    LOGIT(CERR_LOG,"Unable to insert variable into symbol table\n");
                }
			}
			else
			if((*ppToken)->IsArrayVar())
			{
				(*ppToken)->SetSubType(RUL_ARRAY_DECL);
				_INT32 i32Idx = pSymbolTable->Insert(**ppToken,this->GetSymbolTableScopeIndex());
				if( i32Idx >= 0 )// emerson checkin april2013
				{
					(*ppToken)->SetSymbolTableIndex(i32Idx);
					(*ppToken)->SetSymbolTableScopeIndex(this->GetSymbolTableScopeIndex());
					(*ppToken)->SetSubType(pSymbolTable->GetAt(i32Idx)->GetSubType());
				}
				else
                {
                    LOGIT(CERR_LOG,"Unable to insert array into symbol table\n");
                }
			}
			else if(RUL_STRING_CONSTANT == (*ppToken)->GetSubType())
			{
				_INT32 i32Idx = pSymbolTable->InsertConstant(**ppToken);
				if( i32Idx >= 0 )// emerson checkin april2013
				{
					(*ppToken)->SetConstantIndex(i32Idx);
				}
				else
                {
                    LOGIT(CERR_LOG,"Unable to insert string constant into symbol table\n");
                }

			}
			else if(RUL_CHAR_CONSTANT == (*ppToken)->GetSubType())
			{
				_INT32 i32Idx = pSymbolTable->InsertConstant(**ppToken);
				if( i32Idx >= 0 )// emerson checkin april2013
				{
					(*ppToken)->SetConstantIndex(i32Idx);
				}
				else
                {
                    LOGIT(CERR_LOG,"Unable to insert character constant into symbol table\n");
                }

			}
#endif
			return LEX_SUCCESS;
		}
	}
	catch(CRIDEError* perr)
	{					
		SaveState(i32NewPos);
		m_pvecErr->push_back(perr);
	}
	catch(...)
	{								
		SaveState(i32NewPos);
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return LEX_FAIL;
}

_INT32 CLexicalAnalyzer::UnGetToken()
{
	m_CurToken		= m_PrevToken;
	m_i32CurrentPos = m_i32PrevPos;

	m_i32LineNo	= m_i32PrevLineNo;
	m_i32CurLineNo	= m_i32PrevLineNo;
	m_nLastSymbolTableScopeIndex = m_nPrevSymbolTableScopeIndex;//stevev 25apr13
	m_nSymbolTableScopeIndex     = m_nPrevSymbolTableScopeIndex;
	return LEX_SUCCESS;
}

_INT32 CLexicalAnalyzer::Load(
		_UCHAR*		pszRule,
		_UCHAR*		pszRuleName,
		ERROR_VEC*	pvecErrors)
{
	m_pszSource = pszRule;
	m_pvecErr = pvecErrors;
	//strcpy((_CHAR*)m_szRuleName,(const _CHAR*)pszRuleName);

	return LEX_SUCCESS;
}

_INT32  CLexicalAnalyzer::SaveState(
			_INT32 i32CurState)
{
	m_i32PrevPos	= m_i32CurrentPos;
	m_i32CurrentPos = i32CurState;
	m_i32PrevLineNo = m_i32CurLineNo;
	m_i32CurLineNo = m_i32LineNo;
	m_nPrevSymbolTableScopeIndex = m_nLastSymbolTableScopeIndex;
	m_nLastSymbolTableScopeIndex = m_nSymbolTableScopeIndex;//stevev 25apr13(reused LastSymbol)
	return LEX_SUCCESS;
}


bool CLexicalAnalyzer::isSpace(
			const _CHAR*		pszSource,
			_INT32&				i, 
			RUL_TOKEN_TYPE&		Type, 
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer)
{
	if(pszSource[i] == ' ' || pszSource[i] == '\t' || pszSource[i] == '\r')
	{
		//munch all prepended white space of a token...
		i++;
	}
	else if(pszSource[i] == '\n')
	{
		m_i32LineNo++;
		i++;
	}
	else
	{
		return false;
	}
	return true;
}

//match for the regular expression 
//	digit	-->		[0-9]
//	Number	-->		digit+ (.digit+)
bool	CLexicalAnalyzer::isNumber(
			const _CHAR*		pszSource,
			_INT32&				i32CurPos,
			RUL_TOKEN_TYPE&		Type,
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer)
{
	memset(pszBuffer,0,BUFFER_SIZE);
	if(isdigit(pszSource[i32CurPos]) || (pszSource[i32CurPos] == '.'))
	{
		_INT32		i=0;
		bool	bIsFloat = false;
		bool	bIsHex = false;
		for( ;pszSource[i32CurPos+i]; i++ )
		{
			if (i==0)
			{
				if(isdigit(pszSource[i32CurPos+i]))
				{
					pszBuffer[i] = pszSource[i32CurPos+i];
					pszBuffer[i + 1] = 0;
					if ((atoi(pszBuffer) ==  0) && 
						((pszSource[i32CurPos+i+1] == 'x') || (pszSource[i32CurPos+i+1] == 'X')))
					{
						pszBuffer[i + 1] = pszSource[i32CurPos+i+1];
						bIsHex = true;
						i++;
					}
					continue;
				}
			}
			if(isdigit(pszSource[i32CurPos+i]))
			{
				pszBuffer[i] = pszSource[i32CurPos+i];
				continue;
			}
			else if(('.' == pszSource[i32CurPos+i]) && !bIsFloat)
			{
				pszBuffer[i] = pszSource[i32CurPos+i];
				bIsFloat = true;
				continue;
			}
			else 
			if ( bIsFloat && 
				 ( ('e' == pszSource[i32CurPos+i]) ||
				   ('E' == pszSource[i32CurPos+i]) 
				 )  )
			{
				if ( ('+' == pszSource[i32CurPos+i+1]) || ('-' == pszSource[i32CurPos+i+1]) || 
				     isdigit(pszSource[i32CurPos+i+1]))//WHS EP June17-2008 support implicit + sign for scientific notation
				{
					pszBuffer[i] = pszSource[i32CurPos+i];
					pszBuffer[i+1] = pszSource[i32CurPos+i+1];
					i++;
					continue;
				}
			}
			else if(bIsHex) 
			{
				bool bUnknownSymbolFound = false;
				switch(pszSource[i32CurPos+i])
				{
					case 'a':
					case 'b':
					case 'c':
					case 'd':
					case 'e':
					case 'f':
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':
						{
							pszBuffer[i] = pszSource[i32CurPos+i];
							continue;
						}
					default:
						{
							bUnknownSymbolFound = true;
							break;
						}
				}
				if (bUnknownSymbolFound)
					break;
			}
			else
			{
				if (!bIsHex && !bIsFloat)
				{
					if(
					('e' == pszSource[i32CurPos+i]) 
					|| ('E' == pszSource[i32CurPos+i]) 
					)
					{
						bIsFloat = true;
						pszBuffer[i] = pszSource[i32CurPos+i];
						if ( ('+' == pszSource[i32CurPos+i+1]) || ('-' == pszSource[i32CurPos+i+1]) || 
							 isdigit(pszSource[i32CurPos+i+1]))//WHS EP June17-2008 support implicit + sign for scientific notation
						{
							pszBuffer[i+1] = pszSource[i32CurPos+i+1];
							i++;
						}
						continue;
					}

				}
				break;
			}
		}
		if(i >= 0)	//if found a number
		{
			i32CurPos	+=	i;
			Type		=	RUL_NUMERIC_CONSTANT;
			SubType		=	bIsFloat? RUL_REAL_CONSTANT:RUL_INT_CONSTANT;
			return true;
		}

	}
	return false;
}

bool CLexicalAnalyzer::isString(
			const _CHAR*		pszSource,
			_INT32&				i32CurPos,
			RUL_TOKEN_TYPE&		Type,
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer)
{
	memset(pszBuffer,0,BUFFER_SIZE);
	if('"' == pszSource[i32CurPos])
	{
		_INT32		i=1;
		for(;'"' != pszSource[i32CurPos+i];i++)
		{
			pszBuffer[i-1] = pszSource[i32CurPos+i];
			if ((pszSource[i32CurPos+i] == '\\') && (pszSource[i32CurPos+i+1] == '"')
				&& (pszSource[i32CurPos+i-1] != '\\'))
			{
				pszBuffer[i-1 + 1] = pszSource[i32CurPos+i + 1];
				i++;
			}
		}
		if('"' == pszSource[i32CurPos+i])
		{
			i++;
			i32CurPos	+= i;
			Type		= RUL_STR_CONSTANT;
			SubType		= RUL_STRING_CONSTANT;
			return true;
		}
	}
	return false;
}

bool CLexicalAnalyzer::isChar(
			const _CHAR*		pszSource,
			_INT32&				i32CurPos,
			RUL_TOKEN_TYPE&		Type,
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer)
{
	memset(pszBuffer,0,BUFFER_SIZE);
	if('\'' == pszSource[i32CurPos])
	{
		_INT32		i=1;
		for(;'\'' != pszSource[i32CurPos+i];i++)
		{
			pszBuffer[i-1] = pszSource[i32CurPos+i];
		}
		if('\'' == pszSource[i32CurPos+i])
		{
			i++;
			i32CurPos	+= i;
			Type		= RUL_CHR_CONSTANT;
			SubType		= RUL_CHAR_CONSTANT;
			return true;
		}
	}
	return false;
}

//Store the previous token for identifying the 
//unary- operator

_INT32	CLexicalAnalyzer::Tokenize(
			_INT32				i32CurState, 
			RUL_TOKEN_TYPE		Type, 
			RUL_TOKEN_SUBTYPE	SubType,
			CToken**			ppToken,
			_CHAR*				pszBuffer)
{
	try
	{
		*ppToken	= new CToken(pszBuffer,Type,SubType,GetLineNumber());
		if(0 == *ppToken)
		{
			throw(C_UM_ERROR_LOWMEMORY);
		}
		m_PrevToken = m_CurToken;
		m_CurToken	= **ppToken;

		if (Type == RUL_TYPE_ERROR)
		{
#ifdef MODIFIED_BY_SS
			std::string strTemp = "Error in Lexical Analysis";
			//strcpy(pszBuffer,"Error in Lexical Analysis");
			std::copy(strTemp.begin(), strTemp.end(), pszBuffer);

#endif
		}
		return LEX_SUCCESS;
	}
	catch(CRIDEError* perr)
	{					
		DELETE_PTR(*ppToken);
		throw perr;
	}
	catch(...)	
	{								
		DELETE_PTR(*ppToken);
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return LEX_FAIL;
}

//Store the previous token for identifying the 
//unary- operator

_INT32	CLexicalAnalyzer::Tokenize(
			_INT32				i32CurState, 
			RUL_TOKEN_TYPE		Type, 
			RUL_TOKEN_SUBTYPE	SubType,
			CToken**			ppToken,
			_CHAR*				pszBuffer,
			COMPOUND_DATA&		cmpData)
{
	try
	{
		memset(pszBuffer,0,BUFFER_SIZE);
#ifdef MODIFIED_BY_SS
		std::string strTemp(cmpData.m_szName);
		//strcat(pszBuffer,cmpData.m_szName);
		std::copy(strTemp.begin(), strTemp.end(),pszBuffer);
		pszBuffer[strTemp.length() - 1] = '\0';
#endif

		*ppToken	= new CToken(pszBuffer,Type,SubType,cmpData,GetLineNumber());
		if(0 == *ppToken)
		{
			throw(C_UM_ERROR_LOWMEMORY);
		}
		m_PrevToken = m_CurToken;
		m_CurToken	= **ppToken;

		if (Type == RUL_TYPE_ERROR)
		{
#ifdef MODIFIED_BY_SS
			std::string strTemp = "Error in Lexical Analysis";
			//strcpy(pszBuffer,"Error in Lexical Analysis");
			std::copy(strTemp.begin(), strTemp.end(), pszBuffer);

#endif
		}

		return LEX_SUCCESS;
	}
	catch(CRIDEError* perr)	
	{						
		DELETE_PTR(*ppToken);
		throw perr;	
	}
	catch(...)	
	{								
		DELETE_PTR(*ppToken);
		throw(C_UM_ERROR_UNKNOWNERROR);
	}

	return LEX_FAIL;
}

_INT32	CLexicalAnalyzer::TokenizeWithoutSave(
			_INT32				i32CurState, 
			RUL_TOKEN_TYPE		Type, 
			RUL_TOKEN_SUBTYPE	SubType,
			CToken**			ppToken,
			_CHAR*				pszBuffer
			)
{
	try
	{
		*ppToken	= new CToken(pszBuffer,Type,SubType,GetLineNumber());
		if(0 == *ppToken)
		{
			throw(C_UM_ERROR_LOWMEMORY);
		}

		if(Type == RUL_TYPE_ERROR)
		{
#ifdef MODIFIED_BY_SS
			std::string strTemp = "Error in Lexical Analysis";
			//strcpy(pszBuffer,"Error in Lexical Analysis");
			std::copy(strTemp.begin(), strTemp.end(), pszBuffer);

#endif
		}

		return LEX_SUCCESS;
	}
	catch(CRIDEError* perr)
	{					
		DELETE_PTR(*ppToken);
		throw perr;
	}
	catch(...)	
	{								
		DELETE_PTR(*ppToken);
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return LEX_FAIL;
}

bool	CLexicalAnalyzer::MatchGrammarTerminals(
			const _CHAR*		pszSource,
			_INT32&				i32CurPos, 
			RUL_TOKEN_TYPE&		Type, 
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer)
{
	_INT32 i32Size = sizeof(State)/sizeof(DFA_State);
	_INT32 i = 0, j = 0;

	for(i=0; i<i32Size;i++)
	{
		_INT32 i32Len = strlen(State[i].szWord);
		memset(pszBuffer,0,BUFFER_SIZE);
		for(j=0;(pszSource[i32CurPos+j]) && (j<i32Len); j++)
		{
			pszBuffer[j] = pszSource[i32CurPos+j];

			if(State[i].szWord[j] != pszSource[i32CurPos+j])
				break;
		}

		if ((j == i32Len) && (pszSource[i32CurPos+j] == '_')
			&& (State[i].Type == RUL_KEYWORD))
		{
			continue;
		}

		if(
			(j == i32Len) 
			&& ((State[i].Type != RUL_KEYWORD) || ((State[i].Type == RUL_KEYWORD) 
						&& !isalnum(pszSource[i32CurPos+j]))))	//match found
		{
			i32CurPos	+=	j;
			Type		=	State[i].Type;
			SubType		=	State[i].SubType;
			return true;
		}
	}
	
	return false;
}

bool	CLexicalAnalyzer::MatchOMService(
			const _CHAR*		pszSource,
			_INT32&				i32CurPos, 
			RUL_TOKEN_TYPE&		Type, 
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer)
{
	_INT32 nSize = sizeof(OM_Service)/sizeof(DFA_State);
	_INT32 i = 0, j = 0;

	for(i=0; i<nSize;i++)
	{
		_INT32 nLen = strlen(OM_Service[i].szWord);
		memset(pszBuffer,0,BUFFER_SIZE);
		for(j=0;(pszSource[i32CurPos+j]) && (j<nLen); j++)
		{
			pszBuffer[j] = pszSource[i32CurPos+j];

			if(OM_Service[i].szWord[j] != pszSource[i32CurPos+j])
				break;
		}
		if((j == nLen) 
			&& ((OM_Service[i].Type != RUL_KEYWORD) 
					|| ((OM_Service[i].Type == RUL_KEYWORD) && !isalnum(pszSource[i32CurPos+j]))))	//match found
		{
			i32CurPos	+= j;
			Type		= OM_Service[i].Type;
			SubType		= OM_Service[i].SubType;
			return true;
		}
	}
	
	return false;
}


bool	CLexicalAnalyzer::isTerminal(
			const _CHAR*		szSource,
			_INT32&				i32CurPos, 
			RUL_TOKEN_TYPE&		Type, 
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer)
{
	return MatchGrammarTerminals(szSource,i32CurPos,Type,SubType,pszBuffer);
}

bool isPointChar(
		unsigned char ch)
{
	return (isalnum(ch) || ch == '.');
}

//	match for the regular expression 
//	letter		--> [a-zA-Z]
//	digit		--> [0-9]
//	identifier	-->	letter(letter|digit)*
bool CLexicalAnalyzer::isIdentifier(
			const _CHAR*		pszSource,
			_INT32&				i32CurPos,
			RUL_TOKEN_TYPE&		Type,
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer)
{
	try
	{
#ifdef _FULL_RULE_ENGINE
		if(isalpha(pszSource[i32CurPos]) || (pszSource[i32CurPos] == '_'))
		{
	//Get the Identifier
			_INT32 i=0;
			memset(pszBuffer,0,BUFFER_SIZE);
			for(i=0; 
					(pszSource[i32CurPos+i]) 
					&& ( isalnum(pszSource[i32CurPos+i]) 
							|| pszSource[i32CurPos+i] == '_')
					&& (i<BUFFER_SIZE);
				i++)
			{
				pszBuffer[i] = pszSource[i32CurPos+i];
			}
	//Variable -- pszBuffer -- It's got to be a variable now.
			i32CurPos	+= i;
			Type		= RUL_SIMPLE_VARIABLE;//Keywords[i].Type;
			SubType		= RUL_SUBTYPE_NONE;//Keywords[i].SubType;
			if(i>=BUFFER_SIZE)
			{
				throw(C_LEX_ERROR_IDLONG);
			}
			return LEX_SUCCESS;
		}

#else
#ifdef MODIFIED_BY_SS
	//	if(pszSource[i32CurPos] == '#')
	//		++nCurPos;
	//	else
	//		return LEX_FAIL;
	//	if(isalnum(pszSource[i32CurPos]))
	//	{
	////Get the Identifier
	//		_INT32 i=0;
	//		memset(pszBuffer,0,BUFFER_SIZE);
	//		for(i=0; (pszSource[i32CurPos+i]) && isPointChar(pszSource[i32CurPos+i]);i++)
	//		{
	//			pszBuffer[i] = pszSource[i32CurPos+i];
	//		}

	////Variable -- pszBuffer -- It's got to be a variable now.
	//		nCurPos += i;
	//		Type	= RUL_SIMPLE_VARIABLE;//Keywords[i].Type;
	//		SubType = RUL_SUBTYPE_NONE;//Keywords[i].SubType;

	//		_WORD	wPointDataType;
	//		_UINT32 wErrCode = g_PointDatabase->GetPointDataType(pszBuffer, wPointDataType);
	//		if(wErrCode != DCAP_SUCCESS)
	//			return LEX_FAIL;
	//		else
	//			SubType = (enum RUL_TOKEN_SUBTYPE)wPointDataType ; //RUL_INTEGER_DECL;
	//		return LEX_SUCCESS;
	//	}
#endif
#endif
	}
	catch(CRIDEError* perr)
	{					
		UNUSED_LOCAL(perr);		
		throw;	
	}
	catch(...)
	{								
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return LEX_FAIL;
}

_INT32 CLexicalAnalyzer::GetLineNumber()
{
	return m_i32CurLineNo;
}

//The Object access is of the form
// <ObjectManager::><Id(.Id)*>
bool CLexicalAnalyzer::isObject(
			const _CHAR*		pszSource,
			_INT32&				i32CurPos,
			RUL_TOKEN_TYPE&		Type,
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer,
			COMPOUND_DATA&		cmpData)
{
	_INT32 i32Temp = i32CurPos;
	return LEX_FAIL;
}

bool CLexicalAnalyzer::isService(
			const _CHAR*		pszSource,
			_INT32&				i32CurPos,
			RUL_TOKEN_TYPE&		Type,
			RUL_TOKEN_SUBTYPE&	SubType,
			_CHAR*				pszBuffer,
			COMPOUND_DATA&		cmpData)
{
	_INT32 i32Temp = i32CurPos;
	if(MatchOMService(pszSource,i32Temp,Type,SubType,pszBuffer)  && (Type == RUL_SERVICE))
	{
		i32CurPos = i32Temp;	//if the token is a desired one, then update the cur pointer.
		RUL_TOKEN_TYPE		newType;
		RUL_TOKEN_SUBTYPE	newSubType;
		memset(&cmpData,0,sizeof(COMPOUND_DATA));
		_INT32				i=i32CurPos;

		EAT_SPACE;
		if(isTerminal(pszBuffer,i,newType,newSubType,pszBuffer) && (newSubType==RUL_SCOPE))
		{
			EAT_SPACE;
			if(isIdentifier(pszSource,i,newType,newSubType,pszBuffer))
			{
		//Now go on looking for (.Id)*
				//1.	Copy the buffer into the Compound Data.
				//2.	Go on looking for <alphanum> and <.>
				i32CurPos = i;
				_INT32 nLen = strlen(pszBuffer);
				memset(cmpData.m_szName,0,nLen+1);// this was cleared earlier
				memcpy(cmpData.m_szName,pszBuffer,nLen);

				if(isTerminal(pszSource,i,newType,newSubType,pszBuffer) && (newSubType==RUL_DOT))
				{
					if(isIdentifier(pszSource,i,newType,newSubType,pszBuffer))
					{
#ifdef MODIFIED_BY_SS
						/*strcat(cmpData.m_szAttribute, ".");
						strcat(cmpData.m_szAttribute, pszBuffer);*/
						std::string strTemp(cmpData.m_szAttribute);
						strTemp.append(".");
						strTemp.append(pszBuffer);
						std::copy(strTemp.begin(), strTemp.end(), cmpData.m_szAttribute);
#endif
						i32CurPos = i;
						if(isTerminal(pszSource,i,newType,newSubType,pszBuffer) && (newSubType==RUL_LPAREN))
						{
							if(isTerminal(pszSource,i,newType,newSubType,pszBuffer) && (newSubType==RUL_RPAREN))
							{
								SubType = RUL_SERVICE_INVOKE;
								i32CurPos = i;
							}
							else
							{
								//error -- mismatched parenthesis
								return LEX_FAIL;
							}
						}
						return LEX_SUCCESS;
					}
					else
					{
						//error -- no identifier after <.>
						return LEX_FAIL;
					}
				}
				else
				{
					return LEX_SUCCESS;
				}
			}
			else
			{
				//error --  no identifier after <::>
				return 0;
			}
		}
	}
	return LEX_FAIL;
}

_CHAR* CLexicalAnalyzer::GetRuleName()
{
//	return (_CHAR*)m_szRuleName;
	return NULL;
}

_INT32	CLexicalAnalyzer::MoveTo(
			RUL_TOKEN_TYPE		Type, 
			RUL_TOKEN_SUBTYPE	SubType,
			CSymbolTable*		pSymbolTable)
{
	CToken* pToken=0;
	_INT32	i32Ret = LEX_FAIL;
	bool	bIsFound = false;	
	while(LEX_SUCCESS == (i32Ret = GetNextToken(&pToken,pSymbolTable))
		&& pToken)
	{
		RUL_TOKEN_TYPE newType			= pToken->GetType();
		RUL_TOKEN_SUBTYPE newSubType	= pToken->GetSubType();
		DELETE_PTR(pToken);

		if(newType == Type	&& newSubType == SubType)
		{
			bIsFound = true;
			break;
		}
	}
	DELETE_PTR(pToken);
	if(bIsFound)
	{
		UnGetToken();
	}
	return i32Ret;
}

_INT32	CLexicalAnalyzer::MovePast(
			RUL_TOKEN_TYPE		Type, 
			RUL_TOKEN_SUBTYPE	SubType,
			CSymbolTable*		pSymbolTable)
{
	CToken* pToken=0;
	_INT32 i32Ret = LEX_FAIL;
	while(LEX_SUCCESS == (i32Ret = GetNextToken(&pToken,pSymbolTable))
		&& pToken)
	{
		RUL_TOKEN_TYPE newType			= pToken->GetType();
		RUL_TOKEN_SUBTYPE newSubType	= pToken->GetSubType();
		DELETE_PTR(pToken);

		if(newType == Type	&& newSubType == SubType)
		{
			break;
		}
	}
	DELETE_PTR(pToken);
	return i32Ret;
}

_INT32	CLexicalAnalyzer::SynchronizeTo(
		PRODUCTION				production,
		CSymbolTable*		pSymbolTable)
{
	CToken* pToken=0;
	_INT32 i32Ret = LEX_FAIL;
	bool	bIsFound = false;	

	while(LEX_SUCCESS == (i32Ret = GetNextToken(&pToken,pSymbolTable))
		&& pToken)
	{
		RUL_TOKEN_TYPE newType			= pToken->GetType();
		RUL_TOKEN_SUBTYPE newSubType	= pToken->GetSubType();
		DELETE_PTR(pToken);

		if(g_follow_set.IsPresent(production,newType,newSubType))
		{
			bIsFound = true;
			break;
		}
	}
	if(bIsFound)
	{
		UnGetToken();
	}

	return i32Ret;
}

_BOOL CLexicalAnalyzer::GetRulString(
							RUL_TOKEN_TYPE tokenType
							, RUL_TOKEN_SUBTYPE tokenSubType
							, _CHAR*		pszRulString
							)
{
	_INT32 i32Size = sizeof(State)/sizeof(DFA_State);

	for (int iLoopVar = 0;iLoopVar < i32Size;iLoopVar++)
	{
		if ( (State[iLoopVar].Type == tokenType) &&
			 (State[iLoopVar].SubType  == tokenSubType) )
		{

			//strcpy(pszRulString, State[iLoopVar].szWord );
			return true;

		}
	}
	return false;
}





//This fuction gets the Expression followed by the Firs dot Operatot which we
//May need to resolve while executing the statement
//Anil changed Function Prototype Octobet 5 2005 for handling Method Calling Method
bool CLexicalAnalyzer::GetComplexDotExp(int iPosOfDot, char** szDotExpression,DD_ITEM_TYPE DDitemType)//Changed the Function type Anil September 16 2005
{
	
	//If it is not variable type then Do the below chwcking
	bool bIsVariableItem = true;
	if(DDitemType == DD_ITEM_NONVAR)
	{
		bIsVariableItem = false;
	}

	if(bIsVariableItem == false)
	{
		if(!( 
			 (m_pszSource[iPosOfDot] == '.') || 
			 (m_pszSource[iPosOfDot] == '[')
			 )
			 
		  )
			return false;
	}

	//Get the Expression followe by first dot (.)
	bool bIsEnd = false;
	const _CHAR*		pszSource = (const _CHAR*)m_pszSource;
	int iLeftBrackCount = 0;
	RUL_TOKEN_TYPE		Type;
	RUL_TOKEN_SUBTYPE	SubType;
	char pszBuffer[100];
	long int lCount = 0;
	bool nNonSpacePresent = false;

	//Added to take care of Methos DD item Anil September 16 2005
	if(DDitemType == DD_ITEM_METHOD)
	{
		_INT32 i = iPosOfDot;
		while(isSpace(pszSource,i,Type,SubType,&(pszBuffer[0])));
		if(pszSource[i] != '(')
		{
			return false;
		}
		int iLeftBrace = 0;
		i = iPosOfDot;
		for(; (pszSource[i]) && !bIsEnd;)
		{
			if(isSpace(pszSource,i,Type,SubType,pszBuffer))
			{
				lCount++;
				continue;
			}
			if((iLeftBrace == 0) && 
				IsEndofComDotOp(pszSource[i],i)
				)
			{
				break;
			}			
			if(pszSource[i] == '(')
			{
				iLeftBrace++;			

			}
			if(pszSource[i] == ')')
			{
				iLeftBrace--;			

			}
			lCount++;
			i++;
		}

		if( iLeftBrace != 0)
		{
			return false;
		}

		if( lCount>0)
		{
			int iTemp = lCount;
			//This is because , it may happen Space follwed by the Comple DD expression is present, Just remove that
			for( long i = iPosOfDot + iTemp -1; i> iPosOfDot ; i--)
			{
			
				_INT32 lTemp = i;
				if(isSpace(pszSource,lTemp,Type,SubType,pszBuffer))		
				{
					lCount--;
					continue;		
				}
				break;
			}
			*szDotExpression = new char[lCount + 1];
			strncpy_s(*szDotExpression,lCount, (const char*)&m_pszSource[iPosOfDot],_TRUNCATE);
			(*szDotExpression)[lCount] = '\0';
		}
		return true;

	}
	//Loop through and get the total count of the Expression
	for(_INT32 i = iPosOfDot; (pszSource[i]) && !bIsEnd;)
	{
		if(isSpace(pszSource,i,Type,SubType,pszBuffer))
		{
			lCount++;
			continue;
		}
		if((iLeftBrackCount == 0) && 
			IsEndofComDotOp(pszSource[i],i)
			)
		{
			break;
		}
		nNonSpacePresent =true;
		if(pszSource[i] == '[')
		{
			iLeftBrackCount++;			

		}
		if(pszSource[i] == ']')
		{
			iLeftBrackCount--;			

		}
		lCount++;
		i++;
	}
	
	if(bIsVariableItem == false && lCount == 0)
	{
		return false;
	}
	
	if((lCount>0) && (nNonSpacePresent == true))
	{
		int iTemp = lCount;
		//This is because , it may happen Space follwed by the Comple DD expression is present, Just remove that
		for( long i = iPosOfDot + iTemp -1; i> iPosOfDot ; i--)
		{
		
			_INT32 lTemp = i;
			if(isSpace(pszSource,lTemp,Type,SubType,pszBuffer))		
			{
				lCount--;
				continue;		
			}
			break;
		}
		*szDotExpression = new char[lCount + 1];
		strncpy_s(*szDotExpression,lCount, (const char*)&m_pszSource[iPosOfDot], _TRUNCATE);
		(*szDotExpression)[lCount] = '\0';
	}
	return true;

}
//Helper Function to know whether DD Express is terminated
bool CLexicalAnalyzer::IsEndofComDotOp(char ch, long int i)
{
	if( (m_pszSource[i] == '+')||
		(m_pszSource[i] == '-')||
		(m_pszSource[i] == '/')||
		(m_pszSource[i] == '*')||
		(m_pszSource[i] == ';')||
		(m_pszSource[i] == '=')||
		(m_pszSource[i] == '>')||	/* added as per Anil 24oct05 */
		(m_pszSource[i] == '<')||	/* added as per Anil 24oct05 */
		(m_pszSource[i] == '!')||	/* added as per Anil 24oct05 */
		(m_pszSource[i] == '&')||	/* added as per Anil 24oct05 */
		(m_pszSource[i] == '|')||	/* added as per Anil 24oct05 */
		(m_pszSource[i] == '^')||	/* added as per Anil 24oct05 */
		(m_pszSource[i] == ',')||
		//Anil 040806 Bug fix 562 
		//If the DD expression follwed by ")", which is the case in if statement
		(m_pszSource[i] == ')')
		
		)
	{	
		
		return true;
	}
	return false;
}
//SCR26200 Felix
void CLexicalAnalyzer::SetSymbolTableScopeIndex(
	_INT32 nSymTblScpIdx)
{
	m_nSymbolTableScopeIndex = nSymTblScpIdx;
}

_INT32 CLexicalAnalyzer::GetSymbolTableScopeIndex()	//SCR26200 Felix
{
	return m_nSymbolTableScopeIndex;
}