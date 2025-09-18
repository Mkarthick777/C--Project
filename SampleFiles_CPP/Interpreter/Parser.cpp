#include "pch.h"
//#include "stdafx.h"
#pragma warning (disable : 4786)
#include "Parser.h"
//#include "CompiledVisitor.h"
#include "TypeCheckVisitor.h"

#include "ErrorDefinitions.h"

#include "MEE.h" //Vibhor 010705: Added

CParser::CParser()
{
	pBuiltInLib = NULL;
	m_pMEE		= NULL; //Vibhor 010705: Added
	m_bIsRoutine =false;//Anil Octobet 5 2005 for handling Method Calling Method
}

CParser::~CParser()
{

}

#ifdef STANDALONE_PARSER
bool CParser::Initialize(CBuiltInLib *pBuiltInLibParam)
#else
bool CParser::Initialize(CHart_Builtins *pBuiltInLibParam,MEE *pMEE) //Vibhor 010705: Added
#endif
{
	if (pBuiltInLibParam)
	{
		pBuiltInLib = pBuiltInLibParam;
		m_pMEE = pMEE; //Vibhor 010705: Added
		interpretor.Initialize (pBuiltInLibParam,m_pMEE);
		//Anil Octobet 5 2005 for handling Method Calling Method
		//This is required to differentite whether it is called method or it is a method called from menu
		interpretor.SetIsRoutineFlag(m_bIsRoutine);
		if(lexAnal.InitMeeInterface(m_pMEE))
		return true;
		else
			return false;

	}
	else
	{
		return false;
	}
}


_INT32 CParser::BuildParseTree(
			_UCHAR* pszSource,
			_UCHAR*	pszRuleName,
			_CHAR* szData,
			_CHAR* szSymbolTable)
{
	CExpParser::InitializePrecedenceTable();

	lexAnal.Load(
		pszSource,
		pszRuleName,
		0);

	pgm.CreateParseSubTree(
		&lexAnal,
		&SymbolTable,
		0);

	pgm.Execute(&typeChecker,&SymbolTable,0);
	
	pgm.Identify(szData);

	return 0;
}

// This function builds a parse tree and executes the parse tree directly.
// This is done by CInterpretedVisitor.
_INT32 CParser::Execute(
			_UCHAR* pszSource,
			_UCHAR*	pszRuleName,
			_CHAR* szData,
			_CHAR* szSymbolTable)
{
	CExpParser::InitializePrecedenceTable();
LOGIT(CLOG_LOG,"CParser.Execute");
	lexAnal.Load(
		(_UCHAR*)pszSource,
		pszRuleName,
		0);

	CLexicalAnalyzer	lexAnalPredefinedDeclarations;
	char pchDecSource[] = "{int _bi_rc;}";

	lexAnalPredefinedDeclarations.Load 
		(
			(_UCHAR*)pchDecSource,
			pszRuleName,
			0
		);
#ifdef _DEBUG
	int point = 0;
#endif
	try
	{
		/*<START>TSRPRASAD 09MAR2004 Fix the memory leaks	*/
#ifdef _DEBUG
	point = 1;
#endif
		CProgram	pgm1;
LOGIT(CLOG_LOG,"CParser.Execute pgm1.CreateParseSubTree");
		int iRet32 = pgm1.CreateParseSubTree
									(
										&lexAnalPredefinedDeclarations
										, &SymbolTable
									);
		/*<END>TSRPRASAD 09MAR2004 Fix the memory leaks	*/
#ifdef _DEBUG
	point = 2;
#endif
		if (iRet32 == PARSE_FAIL)
		{
			return INTERPRETER_STATUS_PARSE_ERROR;
		}

LOGIT(CLOG_LOG,"CParser.Execute pgm.CreateParseSubTree");
		iRet32 = pgm.CreateParseSubTree
									(
										&lexAnal
										, &SymbolTable
									);
#ifdef _DEBUG
	point = 3;
#endif
		if (iRet32 == PARSE_FAIL)
		{
			return INTERPRETER_STATUS_PARSE_ERROR;
		}
#ifdef STANDALONE_PARSER
		pgm.Identify(szData);
#endif
#ifdef _DEBUG
	point = 4;
#endif
	}
	catch(...)
	{
LOGIT(CLOG_LOG,"CParser.Execute catch 1");
		return INTERPRETER_STATUS_PARSE_ERROR;
	}

#ifdef PARSER_TEST
	return INTERPRETER_STATUS_OK;
#endif /* PARSER_TEST */
// The following code visits all the nodes in the Parse tree
// and tries to interpret it...

	try
	{
LOGIT(CLOG_LOG,"CParser.Execute pgm.Execute");
		pgm.Execute(
			&interpretor,
			&SymbolTable,
			0);
LOGIT(CLOG_LOG,"CParser.Execute Post pgm.Execute");


#ifdef STANDALONE_PARSER
		SymbolTable.TraceDump(szSymbolTable);
#endif

	}
	catch(...)
	{
LOGIT(CLOG_LOG,"CParser.Execute catch 2");
		return INTERPRETER_STATUS_EXECUTION_ERROR;
	}

	return INTERPRETER_STATUS_OK;
}

bool CParser::GetVariableValue
				(
					char *pchVariableName
					, INTER_VARIANT &varValue
				)
{/* note: stevev 31may07 - array access does not work.
    a) preprocessor does no substitution inside a format string (ie %{cmd_status[STATUS_COMM_STATUS]}) 
	b) we need a way to execute the expression inside the '[' ']' 
	c) we need a way to look up that element inside the variable array
	none of which are immediatly available ***/
	CVariable *pVar = SymbolTable.Find(pchVariableName);
	if (pVar == NULL)
	{
		return false;
	}
	else
	{
		varValue = pVar->GetValue();
		return true;
	}
}

bool CParser::SetVariableValue
				(
					char *pchVariableName
					, INTER_VARIANT &varValue
				)
{
	CVariable *pVar = SymbolTable.Find(pchVariableName);
	if (pVar == NULL)
	{
		return false;
	}
	else
	{
		pVar->GetValue() = varValue;
		return true;
	}
}

//This function builds a parse tree and compiles into a java byte-code format
//This is done by CCompiledVisitor
_INT32 CParser::Compile(
			_UCHAR* pszSource,
			_UCHAR*	pszRuleName,
			_UCHAR** ppszByteCode,
			_INT32* pi32ByteCodeSize,
			_INT32	i32Console)
{
	CExpParser::InitializePrecedenceTable();

	CLexicalAnalyzer	lexAnal;
	CSymbolTable		SymbolTable;
	CProgram			pgm;
	CTypeCheckVisitor	typeChecker((_CHAR*)pszRuleName);
/*	CCompiledVisitor	codeGenerator((_CHAR*)pszRuleName);
	ERROR_VEC*			pvecError = (ERROR_VEC*)i32Console;

	lexAnal.Load(
		pszSource,
		pszRuleName,
		pvecError);

	pgm.CreateParseSubTree(
		&lexAnal,
		&SymbolTable,
		pvecError);

//Add the Type Checking Visitor later here.
	pgm.Execute(
		&typeChecker,
		&SymbolTable,
		0,
		pvecError);*/

/*	if(pvecError->size()>0)
		throw new CRIDEError((_CHAR*)pszRuleName,C_CP_ERROR_HASERRORS,-1);**/
// The following code visits all the nodes in the Parse tree
// and tries to interpret it...

/*	pgm.Execute(
		&codeGenerator,
		&SymbolTable,
		0,
		pvecError);

	*pi32ByteCodeSize = codeGenerator.GetByteCode().size();*/
/*	*ppszByteCode = ucharHeapManager_Allocate(codeGenerator.GetByteCode().size());
	memset(*ppszByteCode,0,*pi32ByteCodeSize);

	for(_INT32 i=0;i<*pi32ByteCodeSize;i++)
	{
		(*ppszByteCode)[i] = codeGenerator.GetByteCode()[i];
	}*/

	return PARSE_SUCCESS;
}


//This is an OverLoaded function for the Methods calling Methods
//This will be called only by MEE in the " called Method" case
//METHOD_ARG_INFO_VECTOR is a vector of Method argument, which contains all the related info regarding the Method parameter
//Here even before calling pgm.CreateParseSubTree, We need to push the Method argument to the Symbol
//Table. If it is passed by the Value, then this need to be initilized with the corresponding value
//in the case of Simple variable, Just create tokemn and Push it on symbol table and then push the vale
//In case of Array variables, Create the variable name with dadta type dynamically and execute the Declaration list
//and initilize with its value----also do not Excute this Declaration list for these variables


_INT32 CParser::Execute(
			_UCHAR* pszSource,
			_UCHAR*	pszRuleName,
			_CHAR* szData,
			_CHAR* szSymbolTable,	
			METHOD_ARG_INFO_VECTOR* vectMethArg,
			vector<INTER_VARIANT>* vectInterVar)
{
	CExpParser::InitializePrecedenceTable();

	lexAnal.Load(
		(_UCHAR*)pszSource,
		pszRuleName,
		0);

	//As some of the Dd have this variacle
	CLexicalAnalyzer	lexAnalPredefinedDeclarations;
	char pchDecSource[] = "{int _bi_rc;}";

	lexAnalPredefinedDeclarations.Load 
		(
			(_UCHAR*)pchDecSource,
			pszRuleName,
			0
		);

	try
	{		
		CProgram	pgm1;
		int iRet32 = pgm1.CreateParseSubTree
									(
										&lexAnalPredefinedDeclarations
										, &SymbolTable
									);
	

		if (iRet32 == PARSE_FAIL)
		{
			return INTERPRETER_STATUS_PARSE_ERROR;
		}
			
		int iNoOfArgs = vectMethArg->size();
		//Loop through each ofthe argument, including return 
		for(int i =0; i<iNoOfArgs; i++)
		{
			
			METHOD_ARG_INFO* pMethArgInfo;
			pMethArgInfo = &(*vectMethArg)[i];
			//For each of the  Function Parameter, Get the token type and Token Sub type
			RUL_TOKEN_TYPE	   tokenType = pMethArgInfo->GetType();
			RUL_TOKEN_SUBTYPE  tokenSubType = pMethArgInfo->GetSubType();

			
			//check for the array type, if not it should be Simple or DD item
			if(RUL_ARRAY_VARIABLE != tokenType)
			{
				//Create a Token with the function Arg name
				// changed to below WS:EPM 17jul07 CToken* pToken = new CToken(pMethArgInfo->GetCalledArgName(),
				CToken localToken(pMethArgInfo->GetCalledArgName(),
											tokenType,
											tokenSubType,
											0);
				//Push it on to the Symbol table--
				//after this it is similar to   declaration int Varname;
				int nIndx = SymbolTable.Insert(localToken);//symbolTable::Insert makes a copy of this token.
				

				//Assign the value-- similar to excution of Varname = 10;
				CVariable* pStore=0;
				pStore = SymbolTable.GetAt(nIndx);
				//Initilize this to the routine token to Distiguish
				pStore->m_bIsRoutineToken = true;
				pStore->m_bIsReturnToken = pMethArgInfo->m_IsReturnVar;
				//Assigh the value that is in the Vector of variant
				pStore->GetValue() = (*vectInterVar)[i];
			}
			else // (RUL_ARRAY_VARIABLE == tokenType)
			{
				//In this case u need to Execute the Declaration list 
				//becasue each of the arry element has to be assigned seperatey as safe arry		
				
				//Extract the caller array value
				
				INTER_VARIANT* vartemp = &(*vectInterVar)[i] ;//&(vectInterVar->at(i));
				INTER_SAFEARRAY* prgsaCaller = vartemp->GetValue().prgsa;
				vector<_INT32> vecDims;
				//vecDims[0] gives the dimension of the array
				prgsaCaller->GetDims(&vecDims);
				_INT32 i32mem = prgsaCaller->MemoryAllocated();
				
				//iMemsize is the each element size in the array
				int iMemsize = i32mem/vecDims[0];
				int iArraysize = vecDims[0];
				char szBufSize[50] ;
#ifdef MODIFIED_BY_SS
				//sprintf(szBufSize, "%d", iArraysize);
				std::string strSize = std::to_string(iArraysize);
				strncpy_s(szBufSize, sizeof(szBufSize), strSize.c_str(), _TRUNCATE);
				szBufSize[sizeof(szBufSize) - 1] = '\0'; 
#endif
				//_itoa(iArraysize,szBufSize,10);				


				//We got array size noe, We need to form a declaration statement lik say int Paramname[10]; 
				//and execute this declaration statement	

				char szDataTypeIdentifier[20] = {0};
				std::string strTemp;

				switch(tokenSubType)
				{
					// Walt EPM 08sep08- added
					case RUL_CHAR_DECL:			
						{
#ifdef MODIFIED_BY_SS
							//strcpy(szDataTypeIdentifier ,"char");	
						strTemp.assign("char");
						std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
						szDataTypeIdentifier[strTemp.length()] = '\0';
#endif
						}
						break;
					case RUL_UNSIGNED_CHAR_DECL:			
						{
							
#ifdef MODIFIED_BY_SS
						
						//strcpy(szDataTypeIdentifier ,"unsigned char");
						strTemp.assign("unsigned char");
						std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
						szDataTypeIdentifier[strTemp.length()] = '\0';
#endif
						}
						break;
					case RUL_SHORT_INTEGER_DECL:			
						{
								
#ifdef MODIFIED_BY_SS
						//strcpy(szDataTypeIdentifier ,"short");
						strTemp.assign("short");
						std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
						szDataTypeIdentifier[strTemp.length()] = '\0';
#endif 						
						}
						break;
					case RUL_UNSIGNED_SHORT_INTEGER_DECL:			
						{	
#ifdef MODIFIED_BY_SS
						//strcpy(szDataTypeIdentifier ,"unsigned short");
						strTemp.assign("unsigned short");
						std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
						szDataTypeIdentifier[strTemp.length()] = '\0';
#endif						
						}
						break;
					case RUL_INTEGER_DECL:
						{
							
#ifdef MODIFIED_BY_SS
							//strcpy(szDataTypeIdentifier, "int");
							strTemp.assign("int");
							std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
							szDataTypeIdentifier[strTemp.length()] = '\0';
#endif	
						}
						break;
					case RUL_LONG_DECL:
						{
							
#ifdef MODIFIED_BY_SS
						//strcpy(szDataTypeIdentifier ,"long");
						strTemp.assign("long");
						std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
						szDataTypeIdentifier[strTemp.length()] = '\0';
#endif

						}
						break;
					case RUL_UNSIGNED_INTEGER_DECL:
						{
#ifdef MODIFIED_BY_SS
						//strcpy(szDataTypeIdentifier ,"unsigned int");
						strTemp.assign("unsigned int");
						std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
						szDataTypeIdentifier[strTemp.length()] = '\0';
#endif					
						}
						break;
					case RUL_LONG_LONG_DECL:
						{							
#ifdef MODIFIED_BY_SS
							//strcpy(szDataTypeIdentifier ,"long long");
							strTemp.assign("long long");
							std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
							szDataTypeIdentifier[strTemp.length()] = '\0';
#endif
						}
						break;
					case RUL_DD_STRING_DECL:			
						{							
#ifdef MODIFIED_BY_SS
							//strcpy(szDataTypeIdentifier ,"DD_STRING");
							strTemp.assign("DD_STRING");
							std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
							szDataTypeIdentifier[strTemp.length()] = '\0';
#endif
						}
						break;
					// Walt EPM 08sep08 - end added
					case RUL_REAL_DECL:
						{
							
#ifdef MODIFIED_BY_SS
						//strcpy(szDataTypeIdentifier ,"float");
						strTemp.assign("float");
						std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
						szDataTypeIdentifier[strTemp.length()] = '\0';
#endif							// 							
						}
						break;

					case RUL_DOUBLE_DECL:
						{
								
#ifdef MODIFIED_BY_SS
							//strcpy(szDataTypeIdentifier ,"double");	
							strTemp.assign("double");
							std::copy(strTemp.begin(), strTemp.end(), szDataTypeIdentifier);
							szDataTypeIdentifier[strTemp.length()] = '\0';
#endif	
						}
						break;

					default:
						{
							DEBUGLOG(CLOG_LOG,"Unhandled sub-token type\n");
							return INTERPRETER_STATUS_PARSE_ERROR;
							//TO DO We may need to validate the   whether it id DD item and Check accordingle
						//error!!!!
						}
						break;

				}//end switch


				//Following is the way to Declarethe arry variable Dynamically--Please follow properly
				//3- for  "{  "
				//szDataTypeIdentifier 
				//3- for "   "
				//pMethArgInfo->GetCalledArgName()
				//1-for "["
				//szBufSize
				//3- "];}"
				//1- NULL
				int ilen = 3 + strlen(szDataTypeIdentifier) + 3 + 
					           strlen( pMethArgInfo->GetCalledArgName() ) + 1 + 
							   strlen(szBufSize) + 
							   strlen(szDataTypeIdentifier) + 3 +1;			

#ifdef MODIFIED_BY_SS			
				////form the dynamic declaration for array
				char* pchVarDecl;
				pchVarDecl = new char[ilen];

				//strcpy(pchVarDecl, "{  ");
				//strcat(pchVarDecl, szDataTypeIdentifier);
				//strcat(pchVarDecl, "   ");

				//strcat(pchVarDecl ,pMethArgInfo->GetCalledArgName());
				//strcat(pchVarDecl ,"[");
				//strcat(pchVarDecl ,szBufSize);
				//strcat(pchVarDecl ,"];}");

				std::string strpchVarDecl = "{  " +
					std::string(szDataTypeIdentifier) +
					"   " +
					pMethArgInfo->GetCalledArgName() +
					"[" +
					szBufSize +
					"];}";
				std::copy(strpchVarDecl.begin(), strpchVarDecl.end(), pchVarDecl);
				pchVarDecl[strpchVarDecl.length() - 1] = '\0';
#endif 

				
				_UCHAR	pszRuleName[] = "Test";
				CLexicalAnalyzer	lexAnalPredefinedDeclarations;

				//Noe Execute this by loading in to Temporory CLexicalAnalyzer
				lexAnalPredefinedDeclarations.Load((_UCHAR*)pchVarDecl,pszRuleName,0);
				CProgram	pgm1;
				int iRet32 = pgm1.CreateParseSubTree(&lexAnalPredefinedDeclarations, &SymbolTable);

			
				
				//Below is the the way to initilize any arry value in the interpreter
				CVariable* pVariable = SymbolTable.Find( pMethArgInfo->GetCalledArgName());
				pVariable->m_bIsRoutineToken = true;
				pVariable->m_bIsReturnToken = pMethArgInfo->m_IsReturnVar;
				
				
				 
				//Now intitialize each element of the arry to the passed value
				INTER_VARIANT& var = pVariable->GetValue();
				if(RUL_SAFEARRAY == var.GetVarType())
				{
					(var.GetValue().prgsa)->Allocate();
				}
				//Get the called 
				INTER_SAFEARRAY* prgsaCalled= pVariable->GetValue().GetValue().prgsa;

				for(int iCount = 0;iCount<iArraysize ; iCount++)
				{
					INTER_VARIANT VarTemp;
					VarTemp.Clear();
					//Get the element and then set the element--This is the way to initilize any arry value in the interpreter
					prgsaCaller->GetElement(iMemsize*iCount,&VarTemp);
					prgsaCalled->SetElement(iMemsize*iCount,&VarTemp);			
				
				}	

			}// end else RUL_ARRAY_VARIABLE == tokenType
		}// next arg

		//When I come here, I am assured that all the  
		//Parameters that are passed in the function are in the symbol table with its value initilized to the
		//the passed value

		iRet32 = pgm.CreateParseSubTree
									(
										&lexAnal
										, &SymbolTable
									);
		if (iRet32 == PARSE_FAIL)
		{
			return INTERPRETER_STATUS_PARSE_ERROR;
		}
#ifdef STANDALONE_PARSER
		pgm.Identify(szData);
#endif
	}
	catch(...)
	{
		return INTERPRETER_STATUS_PARSE_ERROR;
	}

#ifdef PARSER_TEST
	return INTERPRETER_STATUS_OK;
#endif /* PARSER_TEST */
// The following code visits all the nodes in the Parse tree
// and tries to interpret it...

	try
	{
		pgm.Execute(
			&interpretor,
			&SymbolTable,
			0);

#ifdef STANDALONE_PARSER
		SymbolTable.TraceDump(szSymbolTable);
#endif


		//Update the return value and also the parameter which are passed by reference
		//
		int iNoOfArgs = vectMethArg->size();
		for(int i =0; i<iNoOfArgs; i++)
		{
			
			METHOD_ARG_INFO* pMethArgInfo;
			pMethArgInfo = &(*vectMethArg)[i];
			if(pMethArgInfo->ePassedType == DD_METH_AGR_PASSED_BYREFERENCE)
			{
				CVariable* pStore=0;
				pStore = SymbolTable.Find(pMethArgInfo->GetCalledArgName());
				(*vectInterVar)[i] = pStore->GetValue();			

			}	

			
		}
		

	}
	catch(...)
	{
		return INTERPRETER_STATUS_EXECUTION_ERROR;
	}

	return INTERPRETER_STATUS_OK;
}

//Function to set the FLAG that whether it is a routine call( ie Called method), ie it is not called by the menu
void CParser::SetIsRoutineFlag(bool bIsRoutine)
{
	m_bIsRoutine = bIsRoutine;
	return;
}
//To get the Routine flag
bool CParser::GetIsRoutineFlag()
{
	return m_bIsRoutine;
}