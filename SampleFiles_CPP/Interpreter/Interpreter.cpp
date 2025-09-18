#include "pch.h"
#include "Interpreter.h"
#include "MEE.h" //Vibhor 010705: Added

CInterpreter::CInterpreter():m_bInitialized(false)
					, m_intStatus(INTERPRETER_STATUS_INVALID)
					, m_pchSourceCode(NULL)
					, m_pParser(NULL)
					, m_pMEE(NULL)	//Vibhor 010705: Added
{
}

INTERPRETER_STATUS CInterpreter::ExecuteCode
							(
#ifdef STANDALONE_PARSER
								CBuiltInLib  *pBuiltInLibParam
#else
								CHart_Builtins *pBuiltInLibParam
#endif
								, char *pchSource
								, char *pchSourceName
								, char *pchCodeData
								, char *pchSymbolDump
								, MEE  *pMEE			//Vibhor 010705: Added
							)
{
	if (m_bInitialized == true)
	{
		m_bInitialized = false;

		m_intStatus = INTERPRETER_STATUS_INVALID;

		if (m_pParser)
		{
LOGIT(CLOG_LOG,"ExecuteCode Deleting existing Parser.\n");
			delete m_pParser;
		}
	}

	if (pchSource == NULL)
	{
LOGIT(CLOG_LOG,"ExecuteCode  Parser Error.\n");
		return INTERPRETER_STATUS_PARSE_ERROR;
	}
	else
	{
		m_pParser = new CParser;
		if (NULL == m_pParser)
		{
LOGIT(CLOG_LOG,"ExecuteCode No mem Parser.\n");
			return INTERPRETER_STATUS_UNKNOWN_ERROR;
		}

		m_pMEE = pMEE;	//Vibhor 010705: Added

		if (false == m_pParser->Initialize (pBuiltInLibParam, m_pMEE))
		{
LOGIT(CLOG_LOG,"ExecuteCode  Parser.Initialize failed\n");
			return INTERPRETER_STATUS_UNKNOWN_ERROR;
		}

		m_pchSourceCode = pchSource;
		m_bInitialized = true;


LOGIT(CLOG_LOG,"ExecuteCode  Parser.Execute call.\n");
		m_intStatus = (INTERPRETER_STATUS)m_pParser->Execute 
											(
												(unsigned char *)pchSource
												, (unsigned char *)pchSourceName
												, pchCodeData
												, pchSymbolDump
											);
LOGIT(CLOG_LOG,"ExecuteCode  Post Parser.Execute call.\n");
		return m_intStatus;
	}
}

bool CInterpreter::GetVariableValue
				(
					char *pchVariableName
					, INTER_VARIANT &varValue
					, hCVar**     ppDevObjVar
				)
{
	if( false == m_pParser->GetVariableValue(pchVariableName,varValue) )
																//actually 'GetLocalVariableValue'
	{
		long int lCount =0;
		long int lCurrentPos = 0;
		while( (pchVariableName[lCount] != '[')   && 
			   (pchVariableName[lCount] != '.' )  &&
			   (pchVariableName[lCount] != '\0') )
		{
			lCount++;
		}

		int iLen = lCount - lCurrentPos +1;

		//allocate string
		char* szTokenName = new char[iLen];
		memset(szTokenName,0,(iLen));
#ifdef MODIFIED_BY_SS
		//strncpy(szTokenName,pchVariableName, iLen);
		std::copy(pchVariableName, pchVariableName + iLen, szTokenName);

#endif
		szTokenName[iLen -1] = '\0'; 
		hCitemBase *pIB = NULL;
		int iretCode = m_pMEE->ResolveDDExpForBuiltin((const char*)pchVariableName,
								(const char*) szTokenName,    &varValue,  &pIB);
		LOGIT(CLOG_LOG, "GetVariableValue::szTokenName=%s, retCode=%d, varValue=%d\n", szTokenName,iretCode,(int)varValue );
		if( ppDevObjVar )
		{
		if ( pIB != NULL && pIB->IsVariable() ) 
			{
			*ppDevObjVar = (hCVar*) pIB;
			}
		else
			{
			*ppDevObjVar = NULL;
			}
		}

		//Now release the string. 
        DELETE_ARR( szTokenName ); //WaltS:EPM 24aug07 - plug the memory leak 

		if(iretCode == SUCCESS)
		{
			return true;
		}
		else
		{
			return false;
		}   
	}
	else
	{// use the varValue that determined by the GetLocalVariableValue
		if (ppDevObjVar)// we were given a pointer to fill
			*ppDevObjVar = NULL;// tell it we have no DeviceObjectPointer
	}
	return true;
//	return m_pParser->GetVariableValue(pchVariableName,varValue);
}

bool CInterpreter::SetVariableValue
				(
					char *pchVariableName
					, INTER_VARIANT &varValue
				)
{
	return m_pParser->SetVariableValue(pchVariableName,varValue);
}

CInterpreter::~CInterpreter()
{
	m_bInitialized = false;

	m_intStatus = INTERPRETER_STATUS_INVALID;

	if (m_pParser)
	{
		delete m_pParser;
	}
}


//this is an OverLoaded function for the Methods calling Methods
//this will be called only by MEE in the " called Method" case
//METHOD_ARG_INFO_VECTOR is a vector of Method argument, which contails all the related info regarding the Method parameter
//vectInterVar-- Value of the each Argument passed
//should be filled by by interpreted vistor if it is passed by reference

INTERPRETER_STATUS CInterpreter::ExecuteCode
							(
#ifdef STANDALONE_PARSER
								CBuiltInLib  *pBuiltInLibParam
#else
								CHart_Builtins *pBuiltInLibParam
#endif
								, char *pchSource
								, char *pchSourceName
								, char *pchCodeData
								, char *pchSymbolDump
								, MEE  *pMEE
								,METHOD_ARG_INFO_VECTOR* vectMethArg
								,vector<INTER_VARIANT>* vectInterVar
							)
{
	if (m_bInitialized == true)
	{
		m_bInitialized = false;

		m_intStatus = INTERPRETER_STATUS_INVALID;

		if (m_pParser)
		{
			delete m_pParser;
		}
	}

	if (pchSource == NULL)
	{
		return INTERPRETER_STATUS_PARSE_ERROR;
	}
	else
	{
		m_pParser = new CParser;
		if (NULL == m_pParser)
		{
			return INTERPRETER_STATUS_UNKNOWN_ERROR;
		}

		m_pMEE = pMEE;	//Vibhor 010705: Added
		//This is required to differentite whether it is called method or it is a method called from menu
		m_pParser->SetIsRoutineFlag(true);

		if (false == m_pParser->Initialize (pBuiltInLibParam, m_pMEE))
		{
			return INTERPRETER_STATUS_UNKNOWN_ERROR;
		}

		
		m_pchSourceCode = pchSource;
		m_bInitialized = true;


		m_intStatus = (INTERPRETER_STATUS)m_pParser->Execute 
											(
												(unsigned char *)pchSource
												, (unsigned char *)pchSourceName
												, pchCodeData
												, pchSymbolDump
												,vectMethArg
												,vectInterVar
											);
		return m_intStatus;
	}
}


