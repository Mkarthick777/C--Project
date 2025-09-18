
#include "pch.h"
//#include "stdafx.h"
//#include <windows.h>
#include "ddbGeneral.h"
//#include <ALPHAOPS.H>
#include <limits.h>
#include <float.h>

#pragma warning (disable : 4786)
#include "ddbdevice.h"
#include "INTER_VARIANT.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"
#include <algorithm>
#include <string>

#define UINT DWORD
template<typename T>
const T& custom_min(const T& a, const T& b) {
	return (a < b) ? a : b;
}

/************************Vibhor 230204: NOTE Start ************************
The following changes are being made to the _?assign family and
?setval family of builtin calls

1.For _?assign family if the first param is not a long (garbage itemId),
 then the call returns a false

2.For both the families if the data type of value being assigned (the second arg for _?assign,
and the lone arg for ?setval family) is being ignored, bcoz there are DDs which use ,
non matching data value in builtins. eg. fsetval(10), and _iassign(16483,2.5)

The implementations of the builtins do check if the destination (and source if any) var
is atleast a Numeric.

*************************Vibhor 230204: Note End   ************************/

// stevev 25jun07 - add a helper # define 
#define RETURN_AT_NOT_NUMERIC( idx) {if (! pVarParameters[idx].isNumeric()) return false;}

// Define a helper function to convert a wide-character string to a multibyte string


bool CHart_Builtins::InvokeFunction(
				char *pchFunctionName
				, int iNumberOfParameters
				, INTER_VARIANT *pVarParameters
				, INTER_VARIANT *pVarReturnValue
				, int	*pBuiltinReturnCode
				, FunctionExpression* pFuncExp
				)
{
	string strFunName;
	strFunName = pchFunctionName;
	m_MapBuiltinFunNameToEnumIter = m_MapBuiltinFunNameToEnum.find(strFunName);
	if(m_MapBuiltinFunNameToEnumIter == m_MapBuiltinFunNameToEnum.end())
	{
		return false; 
	}

LOGIT(CLOG_LOG,"Invoke function %s\n",pchFunctionName)	;
	
	BUILTIN_NAME enumBuiltinFunValue = m_MapBuiltinFunNameToEnumIter->second;

	
switch(enumBuiltinFunValue)
	{
	//if (strcmp("delay",pchFunctionName)==0)
	case BUILTIN_delay:
	{
		int iTimeinSecs=0;
		tchar pchString[MAX_DD_STRING]={0};
		int  pSize = MAX_DD_STRING;


		if (pVarParameters[0].isNumeric()) 
		{
			iTimeinSecs=(int)pVarParameters[0];
		}
		else
		{
			return false;
		}	
		
		if ( ! GetStringParam(pchString,pSize,pVarParameters,1) )
		{
			return false;
		}
	
		long pLongItemIds[100]={0};
		int iNumberOfItemIds = 0;

		if (pVarParameters[2].GetVarType() == RUL_SAFEARRAY)
		{
			GetLongArray(pVarParameters[2],pLongItemIds,iNumberOfItemIds);
		}
		else
		{
			iNumberOfItemIds = 0;
		}		

		int iReturnValue = delay(iTimeinSecs,pchString, pLongItemIds, iNumberOfItemIds);
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		}	
		return true;
	}
	break;
	//else
	//if (strcmp("DELAY",pchFunctionName)==0)
	case BUILTIN_DELAY:
	{
		tchar pchString[MAX_DD_STRING]={0};
		int pSize = MAX_DD_STRING;
		int TimeInSecs=0;

		if (pVarParameters[0].isNumeric()) 
		{
			TimeInSecs=(int)pVarParameters[0];
		}
		else	
		{	
			return false;
		}		
		
		if ( ! GetStringParam(pchString,pSize,pVarParameters,1) )
		{
			return false;
		}
	
		int iReturnValue = DELAY(TimeInSecs, pchString);

		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		}
		return true;
	}
	break;
	//else
	//if (strcmp("DELAY_TIME",pchFunctionName)==0)
	case BUILTIN_DELAY_TIME:
	{
		int TimeInSecs=0;

		if (pVarParameters[0].isNumeric()) 
		{
			TimeInSecs=(int)pVarParameters[0];	
		}	
		else
		{
			return false;
		}
		
		int iReturnValue = DELAY_TIME(TimeInSecs);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	/*Arun 200505 Start of code*/
	break;
	//else
	//if (strcmp("BUILD_MESSAGE",pchFunctionName)==0)
	case BUILTIN_BUILD_MESSAGE:
	{
		tchar pchDestString[MAX_DD_STRING]={0};
		tchar pchMessageString[MAX_DD_STRING]={0};
		int pSize = MAX_DD_STRING;
// notUsed		int iReturnValue;			
		
		if ( ! GetStringParam(pchMessageString,pSize,pVarParameters,0) )
		{
			return false;
		}
		        
        if( BUILD_MESSAGE( pchDestString, pchMessageString ) == NULL ) 
        { 
                *pBuiltinReturnCode = BUILTIN_ABORT; 
        } 
        else 
        { 
                pVarReturnValue->SetValue( pchDestString, RUL_DD_STRING ); 
        } 
        return true;
	}
	/*End of code*/
	break;
	//else
	//if (strcmp("PUT_MESSAGE",pchFunctionName)==0)
	case BUILTIN_PUT_MESSAGE:
	{
		tchar pchString[MAX_DD_STRING]={0};
		int pSize = MAX_DD_STRING;
	
		if ( ! GetStringParam(pchString,pSize,pVarParameters,0) )
		{
			return false;
		}
		
		int iReturnValue = PUT_MESSAGE(pchString);

		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		}
		return true;
	}
	break;
	//else
	//if (strcmp("put_message",pchFunctionName)==0)
	case BUILTIN_put_message:
	{
		tchar pchString[MAX_DD_STRING]={0};
		int pSize = MAX_DD_STRING;

		if ( ! GetStringParam(pchString,pSize,pVarParameters,0) )
		{
			return false;
		}
		
		long pLongItemIds[100]={0};
		int iNumberOfItemIds = 0;
		if (pVarParameters[1].GetVarType() == RUL_SAFEARRAY)
		{
			GetLongArray(pVarParameters[1],pLongItemIds,iNumberOfItemIds);
		}
		else
		{
				iNumberOfItemIds = 0;
		}


		int iReturnValue = put_message(pchString, pLongItemIds, iNumberOfItemIds);

		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		}
		return true;
	}
	break;
	//else
	//if (strcmp("ACKNOWLEDGE",pchFunctionName)==0)
	case BUILTIN_ACKNOWLEDGE:
	{
		wchar_t pchString[MAX_DD_STRING]={0};
		int  pSize = MAX_DD_STRING;

		LOGIT(CLOG_LOG, _T("** ACKNOWLEDGE pchString = %s"), pchString);

		if ( ! GetStringParam(pchString,pSize,pVarParameters,0) )
		{
			return false;
		}
	
		int iReturnValue = ACKNOWLEDGE(pchString);

		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);		
		}
		return true;
	}
	break;
	//else
	//if (strcmp("acknowledge",pchFunctionName)==0)
	case BUILTIN_acknowledge:
	{
		tchar pchString[MAX_DD_STRING]={0};
		int  pSize = MAX_DD_STRING;
		
		if ( ! GetStringParam(pchString,pSize,pVarParameters,0) )
		{
			return false;
		}

		long pLongItemIds[100]={0};
		int iNumberOfItemIds = 0;
		if (pVarParameters[1].GetVarType() == RUL_SAFEARRAY)
		{
			GetLongArray(pVarParameters[1],pLongItemIds,iNumberOfItemIds);
		}
		else
		{
			iNumberOfItemIds = 0;
		}

		int iReturnValue = acknowledge(pchString, pLongItemIds, iNumberOfItemIds);
		
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);		
		}
		return true;
	}
	break;
	//else
	//if (strcmp("_get_dev_var_value",pchFunctionName)==0)
	case BUILTIN__get_dev_var_value:
	{
		tchar pchString[MAX_DD_STRING]={0};
		int pSize = 0;

		if ( ! GetStringParam(pchString,pSize,pVarParameters,0) )
		{
			return false;
		}


/*
LOGIT(CLOG_LOG,_T("** pchString = %s"),pchString);
LOGIT(CLOG_LOG,"** pchString[1] = %s",pchString++);
LOGIT(CLOG_LOG,"** pchString[2] = %s",pchString++);
LOGIT(CLOG_LOG,"** pchString[3] = %s",pchString++);
LOGIT(CLOG_LOG,"** pchString[4] = %s",pchString++);
LOGIT(CLOG_LOG,"** pchString[5] = %s",pchString++);
LOGIT(CLOG_LOG,"** pchString[6] = %s",pchString++);
LOGIT(CLOG_LOG,"** pchString[7] = %s",pchString++);
LOGIT(CLOG_LOG,"** pchString[8] = %s",pchString++);
LOGIT(CLOG_LOG,"** pchString[9] = %s",pchString++);
*/


		long pLongItemIds[100] = { 0 };
		int iNumberOfItemIds = 0;
		long lItemId=0;

		if (pVarParameters[1].GetVarType() == RUL_SAFEARRAY)
		{
			GetLongArray(pVarParameters[1],pLongItemIds,iNumberOfItemIds);
		}
		else
		{
			iNumberOfItemIds = 0;
		}

		if (pVarParameters[2].isNumeric())
		{
			lItemId=(int)pVarParameters[2];
		}
		else
		{
			return false;
		}

		int iReturnValue = _get_dev_var_value(pchString, pLongItemIds, iNumberOfItemIds,lItemId);

		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);		
		}	
		return true;
	}
	break;
	//else
	//if (strcmp("_get_local_var_value",pchFunctionName)==0)
	case BUILTIN__get_local_var_value:
	{

		LOGIT(CERR_LOG, "BUILTIN__get_local_var_value");


		tchar pchString[MAX_DD_STRING]={0};
		int  pSize = MAX_DD_STRING;
		tchar wide_var_name[MAX_DD_STRING];
		char var_name[MAX_DD_STRING]={0};
		char xlated_var_name[MAX_DD_STRING]={0};
		LOGIT(CERR_LOG, "GetStringParam");
		if ( ! GetStringParam(pchString,pSize,pVarParameters,0) )
		{
			return false;
		}

		long pLongItemIds[100];
		int iNumberOfItemIds = 0;
		LOGIT(CERR_LOG, "pVarParameters");
		if (pVarParameters[1].GetVarType() == RUL_SAFEARRAY)
		{
			LOGIT(CERR_LOG, "GetLongArray");
			GetLongArray(pVarParameters[1],pLongItemIds,iNumberOfItemIds);
		}
		else
		{
			iNumberOfItemIds = 0;
		}
		//                     use an alias, should be type char so it'll go in
		LOGIT(CERR_LOG, "GetStringParam");
		if ( ! GetStringParam(wide_var_name,pSize,pVarParameters,2) )
		{
			return false;
		}
#ifdef MODIFIED_BY_SS
		LOGIT(CERR_LOG, "wcstombs = %s",wide_var_name);
		std::string multi_byte_str = wideCharToMultiByte(wide_var_name);
		//wcstombs( var_name, wide_var_name, MAX_DD_STRING );
		// Copy the multibyte string to the char array
		std::copy(multi_byte_str.begin(), multi_byte_str.end(), var_name);// , multi_byte_str.c_str(), MAX_DD_STRING);

		// Ensure the string is null-terminated
		var_name[MAX_DD_STRING - 1] = '\0';
#endif 
		LOGIT(CERR_LOG, "get_string_translation");
		m_pDevice->dictionary->get_string_translation(var_name, xlated_var_name, MAX_DD_STRING );
		LOGIT(CERR_LOG, "before _get_local_var_value");
		int iReturnValue = _get_local_var_value(pchString, pLongItemIds, iNumberOfItemIds, xlated_var_name);
		LOGIT(CERR_LOG, "after _get_local_var_value");
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		}
		LOGIT(CERR_LOG, "BUILTIN__get_local_var_value done");
		return true;
	}
	break;
	//else
	//if( strcmp("_display_xmtr_status",pchFunctionName)==0)
	case BUILTIN__display_xmtr_status:
	{
		long lItemId=0;
		int iStatusval=0;

		if (pVarParameters[0].isNumeric()) // was '=' stevev 30may07
		{
			lItemId=(int)pVarParameters[0];
		}
		else// added else - stevev 30may07
		{
			return false;
		}

		if (pVarParameters[1].isNumeric()) // was '=' stevev 30may07
		{
			iStatusval=(int)pVarParameters[1];
		}
		else// added else - stevev 30may07
		{
			return false;
		}

		int iReturnValue = _display_xmtr_status(lItemId, iStatusval);
		
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		}				
		return true;
	
	}
	break;
	//else
	//if( strcmp("display_response_status",pchFunctionName)==0)
	case BUILTIN_display_response_status:
	{
		long lCommandVal=0;
		int iStatusval=0;

		if (pVarParameters[0].isNumeric())// was '=' stevev 30may07
		{
			lCommandVal=(int)pVarParameters[0];
		}
		else// added else - stevev 30may07
		{
			return false;
		}

		// changed 05jun07;;;if (pVarParameters[1].varType == RUL_INT)// was '=' stevev 30may07
		RETURN_AT_NOT_NUMERIC(1);
		iStatusval=(int)pVarParameters[1];

		int iReturnValue = display_response_status(lCommandVal, iStatusval);
		
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);	
		}
			
		return true;

	
	}
	break;
	//else
	//if (strcmp("display",pchFunctionName)==0)
	case BUILTIN_display:
	{
		tchar pchString[MAX_DD_STRING]={0};
		int  pSize = MAX_DD_STRING;

		if ( ! GetStringParam(pchString,pSize,pVarParameters,0) )
		{
			return false;
		}

		long pLongItemIds[100]={0};
		int iNumberOfItemIds = 0;

		if (pVarParameters[1].GetVarType() == RUL_SAFEARRAY)
		{
			GetLongArray(pVarParameters[1],pLongItemIds,iNumberOfItemIds);
		}
		else
		{
			iNumberOfItemIds = 0;
		}


		int iReturnValue = display(pchString, pLongItemIds, iNumberOfItemIds);

		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		}

		return true;
	}
	break;
	//else
	//if (strcmp("SELECT_FROM_LIST",pchFunctionName)==0)
	case BUILTIN_SELECT_FROM_LIST:
	{
		tchar pchString_01[MAX_DD_STRING]={0},pchString_02[MAX_DD_STRING]={0};
		int  pSize = MAX_DD_STRING;
		
		if ( ! GetStringParam(pchString_01,pSize,pVarParameters,0) )
		{
			return false;
		}
		
		if ( ! GetStringParam(pchString_02,pSize,pVarParameters,1) )
		{
			return false;
		}

		int iReturnValue = SELECT_FROM_LIST(pchString_01,pchString_02 );

		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);		
		}
		return true;
	}
	break;
	//else
	//if (strcmp("select_from_list",pchFunctionName)==0)
	case BUILTIN_select_from_list:
	{
		tchar pchString_01[MAX_DD_STRING]={0},pchString_02[MAX_DD_STRING]={0};
		int  pSize = MAX_DD_STRING;

		if ( ! GetStringParam(pchString_01,pSize,pVarParameters,0) )
		{
			return false;
		}


		long pLongItemIds[100]={0};
		int iNumberOfItemIds = 0;

		if (pVarParameters[1].GetVarType() == RUL_SAFEARRAY)
		{
			GetLongArray(pVarParameters[1],pLongItemIds,iNumberOfItemIds);
		}
		else
		{
			iNumberOfItemIds = 0;
		}
		
		if ( ! GetStringParam(pchString_02,pSize,pVarParameters,2) )
		{
			return false;
		}

		int iReturnValue = select_from_list(pchString_01, pLongItemIds,iNumberOfItemIds,pchString_02);
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		else
		{
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);		
		}
		return true;
	}
	break;
	//else
	//if (strcmp("_vassign",pchFunctionName)==0)
	case BUILTIN__vassign:
	{
		long lItemIdDest=0;
		long lItemIdSrc=0;
	
		if (pVarParameters[0].isNumeric())
		{
			lItemIdDest=(int)pVarParameters[0];	
		}
/*Vibhor 230204: Start of Code*/
		else
		{
			return false;
		}
/*Vibhor 230204: End of Code*/

		if (pVarParameters[1].isNumeric())
		{
			lItemIdSrc=(int)pVarParameters[1];
		}
/*Vibhor 230204: Start of Code*/
		else
		{
			return false;
		}
/*Vibhor 230204: End of Code*/
	
		int iReturnValue = _vassign(lItemIdDest,lItemIdSrc);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_dassign",pchFunctionName)==0)
	case BUILTIN__dassign:
	{
		long lItemId=0;
		double dVal=0.0;
	
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
/*Vibhor 230204: Start of Code*/
		else
		{
			return false;
		}

		RETURN_AT_NOT_NUMERIC(1);// added stevev 18feb08		
		dVal=(double)pVarParameters[1];
		
/*Vibhor 230204: End of Code*/
	
		int iReturnValue = _dassign(lItemId,dVal);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_fassign",pchFunctionName)==0)
	case BUILTIN__fassign:	
	{
		long lItemId=0;
		float fVal=0.0;
	
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
/*Vibhor 230204: Start of Code*/
		else
		{
			return false;
		}

		RETURN_AT_NOT_NUMERIC(1);// stevev added 18feb08
		
		fVal=(float)pVarParameters[1];
	
/*Vibhor 230204: End of Code*/
	
		int iReturnValue = _fassign(lItemId,fVal);
		LOGIT(CLOG_LOG,"fassign sets 0x%04x to %f\n",lItemId,fVal);// pwink 27
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_lassign",pchFunctionName)==0)
	case BUILTIN__lassign:
	{
		long lItemId=0;
		INT64 lVal=0;
	
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
/*Vibhor 230204: Start of Code*/
		else
		{
			return false;
		}

		RETURN_AT_NOT_NUMERIC(1);// stevev added 18feb08
		lVal=(INT64)pVarParameters[1];
		
/*Vibhor 230204: End of Code*/	
	
		int iReturnValue = _lassign(lItemId,lVal);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_iassign",pchFunctionName)==0)
	case BUILTIN__iassign:
	{
		long lItemId=0;
		INT64  iVal=0;
	
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
/*Vibhor 230204: Start of Code*/
		else
		{
			return false;
		}
		
		RETURN_AT_NOT_NUMERIC(1);// stevev added 18feb08
		iVal=(INT64)pVarParameters[1];
	
/*Vibhor 230204: End of Code*/	
		int iReturnValue = _iassign(lItemId,iVal);
			LOGIT(CLOG_LOG,"                         0x%04x value: %lld",lItemId,iVal);//pwink 01

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_fvar_value",pchFunctionName)==0)
	case BUILTIN__fvar_value:
	{
		unsigned lItemId=0;// pwink 27mar21 - long changes from 32 to 64 with build change
	
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(unsigned)pVarParameters[0];	
		}

		float fReturnValue = _fvar_value(lItemId);
		LOGIT(CLOG_LOG,"fvar_value returns 0x%04x as %f\n",lItemId, fReturnValue);// pwink 27
		pVarReturnValue->SetValue(&fReturnValue, RUL_FLOAT);
		return true;
	}
	break;
	//else
	//if (strcmp("_ivar_value",pchFunctionName)==0)
	case BUILTIN__ivar_value:
	{
		long lItemId=0;
	
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
		else// added else - stevev 30may07
		{
			return false;
		}

		INT64 iReturnValue = _ivar_value(lItemId);
		LOGIT(CLOG_LOG, "                             got: %lld from 0x%04x\n",iReturnValue,lItemId);// pwink 01

		//jad debug
		//remove for production!!!!
//		if (lItemId == 0x4085  && iReturnValue == 0)
//			iReturnValue = 0xff;
//		LOGIT(CLOG_LOG, "                             got: %lld from 0x%04x\n", iReturnValue, lItemId);// pwink 01

		pVarReturnValue->SetValue(&iReturnValue, RUL_LONGLONG);
		return true;
	}
	break;
	//else
	//if (strcmp("_lvar_value",pchFunctionName)==0)
	case BUILTIN__lvar_value:
	{
		long lItemId=0;
	
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
		else// added else - stevev 30may07
		{
			return false;
		}

	
		INT64 iReturnValue = _lvar_value(lItemId);

		pVarReturnValue->SetValue(&iReturnValue, RUL_LONGLONG);
		return true;
	}
	break;
	//Added By Anil June 20 2005 --starts here
	//else
	//if (strcmp("svar_value",pchFunctionName)==0)
	case BUILTIN_svar_value:
	{
		long lItemId = 0;
		char* string_return_var = NULL;
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
		else// added else - stevev 30may07
		{
			return false;
		}
		
		string_return_var = svar_value(lItemId);

		pVarReturnValue->SetValue(string_return_var, RUL_DD_STRING);

		if(string_return_var)
		{
			delete[] string_return_var;
			string_return_var = NULL;
		}
		
		return true;
	}
	break;
	//else
	//if (strcmp("sassign",pchFunctionName)==0)
	case BUILTIN_sassign:
	{
		int iReturnValue = BI_ERROR;//default to failure
		long lItemId = 0;
		char* szValue = NULL;

		if (pVarParameters[0].isNumeric())//do we have an ItemID?
		{
			lItemId=(int)pVarParameters[0];	
		}
		else// added else - stevev 30may07
		{
			return false;
		}

		//Get the String value 
		pVarParameters[1].GetStringValue( &szValue, RUL_DD_STRING );
		
		if( szValue )//do we have a valid string?
		{
			char szLang[5] = {0};
			bool bLangPresent=false;
			//		Remove the Language code , if it was prepended <a tokenizer bug>
			GetLanguageCode( szValue, szLang, &bLangPresent );//remove language code if present
		
			iReturnValue = sassign( lItemId, szValue );//do operation and save return value.

			delete[] szValue;//clean up memory
			szValue = NULL;
		}

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);//set return value

		return true;
	}
	break;
	//Added By Anil June 20 2005 --Ends here

	//else
	//if (strcmp("save_values",pchFunctionName)==0)
	case BUILTIN_save_values:
	{
		int iReturnValue = save_values();

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	
	//Added By Anil July 01 2005 --starts here
	case BUILTIN_discard_on_exit:
	{
		int iReturnValue = discard_on_exit();

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//Added By Anil July 01 2005 --Ends here
	//else
	//if (strcmp("get_more_status",pchFunctionName)==0)
	case BUILTIN_get_more_status:
	{// stevev 25dec07 - this returns the raw bytes - NOT strings!
		//uchar pch_RespCode[STATUS_SIZE]={0};
		//uchar pch_MoreStatusCode[MAX_XMTR_STATUS_LEN]={0};
		int  iReturnValue, moreInfoSize=MAX_XMTR_STATUS_LEN;
		BYTE_STRING( status, STATUS_SIZE );
		BYTE_STRING( info,   MAX_XMTR_STATUS_LEN);

		//iReturnValue = get_more_status(pch_RespCode,pch_MoreStatusCode,moreInfoSize);
		iReturnValue = get_more_status(status.bs,info.bs,moreInfoSize) ;		
		info.bsLen   = moreInfoSize;

		//if ( ! SetByteStringParam(pFuncExp, pVarParameters, 0, pch_RespCode, STATUS_SIZE) )//more_data_status
		if ( ! SetByteStringParam(pFuncExp, pVarParameters, 0, status) )
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( info );
			return false;
		}	
	
		//if ( ! SetByteStringParam(pFuncExp, pVarParameters, 1, pch_MoreStatusCode, moreInfoSize) )//more_data_info
		if ( ! SetByteStringParam(pFuncExp, pVarParameters, 1, info) )//more_data_info
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( info );
			return false;
		}	

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);	
		//jad added from fcg repo
		//issue was method would not abort if veariable could not be set
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}

		DEL_BYTE_STR( status );
		DEL_BYTE_STR( info );
		return true;
	}
	break;
	//else
	//if (strcmp("_get_status_code_string",pchFunctionName)==0)
	case BUILTIN__get_status_code_string:
	{
		long lItemId=0;
		tchar pchString[MAX_DD_STRING]={0};
		int  pSize=MAX_DD_STRING;

		int iStatusCode=0;
		int iStatusStringlength = MAX_DD_STRING;
	
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}

		if (pVarParameters[1].isNumeric())
		{
			iStatusCode=(int)pVarParameters[1];	
		}
			
		if (pVarParameters[3].isNumeric())
		{
			iStatusStringlength=(int)pVarParameters[3];	
		}		
		
		int iReturnValue = _get_status_code_string(lItemId, iStatusCode, pchString,
																			iStatusStringlength);
		if ( ! SetStringParam(pFuncExp, pVarParameters, 2, pchString) )
		{
			return false;
		}

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}	
	break;
	/*Arun 200505 Start of code*/
	//else
	//if (strcmp("get_enum_string",pchFunctionName)==0)
	case BUILTIN_get_enum_string:
	{	
		long lItemId=0;
		int iVariableValue=0;
		int iMaxStringLength=MAX_DD_STRING;
		tchar pchString[MAX_DD_STRING+1] = {0};
		int iReturnValue=0;

		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];
		}

		if (pVarParameters[1].isNumeric())
		{
			iVariableValue=(int)pVarParameters[1];
		}

		//if (pVarParameters[3].varType == RUL_INT)//get_enum_string only takes 3 arguments WHS
		//{
		//	iMaxStringLength=(int)pVarParameters[3];
		//}

		iReturnValue = get_enum_string( lItemId,iVariableValue,pchString, iMaxStringLength);
		//get_enum_string already truncated to iMaxStringLength

		if ( ! SetStringParam(pFuncExp, pVarParameters, 2, pchString) )
		{
			return false;
		}
		
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	/*End of code*/
	//else
	//if (strcmp("_get_dictionary_string",pchFunctionName)==0)
	case BUILTIN__get_dictionary_string:
	{	
		tchar pchString[MAX_DD_STRING]={0};
		int  pSize=MAX_DD_STRING;
		long lItemId=0;
		int iMaxStringLength = MAX_DD_STRING;

		if (iNumberOfParameters != 3)
		{
			return false;
		}

		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];
		}

		if (pVarParameters[2].isNumeric())
		{
			iMaxStringLength=(int)pVarParameters[2];
		}

		int iReturnValue = _get_dictionary_string(lItemId, pchString, iMaxStringLength);
		//_get_dictionary_string already truncated to iMaxStringLength

		if ( ! SetStringParam(pFuncExp, pVarParameters, 1, pchString) )
		{
			return false;
		}
		
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//Anil 22 December 2005 for dictionary_string built in
	case BUILTIN__dictionary_string:
	{	
		tchar *pchString = NULL;	
		long lItemId=0;	//WS:EPM 24may07	
		
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];
		}

		int iReturnValue = _dictionary_string(lItemId, &pchString);

		if(pchString)
		{
			pVarReturnValue->SetValue(pchString, RUL_DD_STRING);//WS:EPM 24may07
			delete[]    pchString;
			pchString = NULL;
		}
		return true;
	}
	break;
	//else
	//if (strcmp("resolve_array_ref",pchFunctionName)==0)
	case BUILTIN_resolve_array_ref:
	{
		long lItemId=0;
		int iIndex=0;
		
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		if (pVarParameters[1].isNumeric())
		{
			iIndex=pVarParameters[1];	
		}
		else
		{
			return false;
		}	

		int iReturnValue = resolve_array_ref(lItemId,iIndex);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}	
	break;
	//else
	//if (strcmp("resolve_record_ref",pchFunctionName)==0)
	case BUILTIN_resolve_record_ref:
	{
		long lItemId=0;
		int iIndex=0;
		
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		if (pVarParameters[1].isNumeric())
		{
			iIndex=(int)pVarParameters[1];	
		}
		else
		{
LOGIT(CERR_LOG,"resolve_record_ref returns false. an error\n");
			return false;
		}	

		int iReturnValue = resolve_record_ref(lItemId,iIndex);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
LOGIT(CERR_LOG,"resolve_record_ref returns true. success\n");
		return true;
	}
	break;
	//else
	//if (strcmp("resolve_param_ref",pchFunctionName)==0)
	case BUILTIN_resolve_param_ref:
	{
		long lItemId=0;
		
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}	

		int iReturnValue = resolve_param_ref(lItemId);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;


	case BUILTIN_resolve_local_ref:
	{	
		long lItemId=0;
		
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(unsigned int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		pVarReturnValue->SetValue(&lItemId, RUL_INT);

		return true;
	}
	break;
	//else
	//if (strcmp("rspcode_string",pchFunctionName)==0)
	case BUILTIN_rspcode_string:
	{
		int  iCmdNumber=-1;
		int  iRespCode=0;
		tchar szwLocaVarName[MAX_DD_STRING+1]={0};
		char szLocaVarName[MAX_DD_STRING+1]={0};
		tchar pchString[MAX_DD_STRING+1]={0};
		int  pSize=MAX_DD_STRING;
		int  iRespCodeLength=0;
		
		if (pVarParameters[0].isNumeric())
		{
			iCmdNumber=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		if (pVarParameters[1].isNumeric())
		{
			iRespCode=pVarParameters[1];	
		}
		else
		{
			return false;
		}
	
		if (pVarParameters[3].isNumeric())
		{
			iRespCodeLength=(int)pVarParameters[3];	
		}
		else
		{
			return false;
		}
		
		int iReturnValue = rspcode_string(iCmdNumber,iRespCode,pchString,iRespCodeLength);
		// GetStringParam is overloaded to get a char string if type is RUL_CHARPTR
		if ( ! GetStringParam(szwLocaVarName, pSize, pVarParameters, 2) )
		{
			return false;
		}
#ifdef MODIFIED_BY_SS
		//wcstombs(szLocaVarName, szwLocaVarName, MAX_DD_STRING);
		std::string multi_byte_str = wideCharToMultiByte(szwLocaVarName);

		// Copy the multibyte string to the char array
		std::copy(multi_byte_str.begin(), multi_byte_str.end(), szLocaVarName);// , multi_byte_str.c_str(), MAX_DD_STRING);

		// Ensure the string is null-terminated
		szLocaVarName[MAX_DD_STRING - 1] = '\0';
#endif
		INTER_VARIANT varTemp;
		char szLang[5] = {0};
		bool bLangPresent=false;
		//		Remove the Language code , if it was appended <a tokenizer bug>
		GetLanguageCode(szLocaVarName,szLang,&bLangPresent);			
		varTemp.SetValue(pchString,RUL_DD_STRING);	
		//		Update the DD local var szLocaVarName with the value lselection
		int x = m_pInterpreter->SetVariableValue(szLocaVarName,varTemp);
		
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_comm_status",pchFunctionName)==0)
	case BUILTIN__set_comm_status:
	{
		int icomm_status=0;
		int iAbortIgnoreRetry=0;
		
		// stevev 11feb08  if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			icomm_status=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		// stevev 11feb08  if (pVarParameters[1].GetVarType() == RUL_INT)
		if (pVarParameters[1].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[1];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_comm_status(icomm_status,iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_device_status",pchFunctionName)==0)
	case BUILTIN__set_device_status:
	{
		int idev_status=0;
		int iAbortIgnoreRetry=0;
		
		//stevev 11feb08 if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			idev_status=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		//stevev 11feb08 if (pVarParameters[1].GetVarType() == RUL_INT)
		if (pVarParameters[1].isNumeric())
		{
			iAbortIgnoreRetry=pVarParameters[1];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_device_status(idev_status,iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_resp_code",pchFunctionName)==0)
	case BUILTIN__set_resp_code:
	{
		int iResp_code;
		int iAbortIgnoreRetry;
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iResp_code=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		// stevev 11feb08   if (pVarParameters[1].GetVarType() == RUL_INT)
		if (pVarParameters[1].isNumeric())
		{
			iAbortIgnoreRetry=pVarParameters[1];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_resp_code(iResp_code,iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_all_resp_code",pchFunctionName)==0)
	case BUILTIN__set_all_resp_code:
	{
		int iAbortIgnoreRetry=0;
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_all_resp_code(iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_no_device",pchFunctionName)==0)
	case BUILTIN__set_no_device:
	{
		int iAbortIgnoreRetry=0;
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_no_device(iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("SET_NUMBER_OF_RETRIES",pchFunctionName)==0)
	case BUILTIN_SET_NUMBER_OF_RETRIES:
	{
		int iNo_of_retries=0;
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iNo_of_retries=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = SET_NUMBER_OF_RETRIES(iNo_of_retries);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_xmtr_comm_status",pchFunctionName)==0)
	case BUILTIN__set_xmtr_comm_status:
	{
		int iCommStatus=0;
		int iAbortIgnoreRetry=0;
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iCommStatus=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		// stevev 11feb08   if (pVarParameters[1].GetVarType() == RUL_INT)
		if (pVarParameters[1].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[1];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_xmtr_comm_status(iCommStatus, iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_xmtr_device_status",pchFunctionName)==0)
	case BUILTIN__set_xmtr_device_status:
	{
		int iDeviceStatus=0;
		int iAbortIgnoreRetry=0;
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iDeviceStatus=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}
		
		LOGIT(CLOG_LOG, "_set_xmtr_device_status = %d\n", iDeviceStatus);

		// stevev 11feb08   if (pVarParameters[1].GetVarType() == RUL_INT)
		if (pVarParameters[1].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[1];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_xmtr_device_status(iDeviceStatus, iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_xmtr_resp_code",pchFunctionName)==0)
	case BUILTIN__set_xmtr_resp_code:
	{
		int iRespCode=0;
		int iAbortIgnoreRetry=0;
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iRespCode=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}

		// stevev 11feb08   if (pVarParameters[1].GetVarType() == RUL_INT)
		if (pVarParameters[1].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[1];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_xmtr_resp_code(iRespCode, iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_xmtr_all_resp_code",pchFunctionName)==0)
	case BUILTIN__set_xmtr_all_resp_code:
	{
		int iAbortIgnoreRetry=0;
		
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[0];	
		}		
		else
		{
			return false;
		}	
	
		int iReturnValue = _set_xmtr_all_resp_code( iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_xmtr_no_device",pchFunctionName)==0)
	case BUILTIN__set_xmtr_no_device:
	{
		int iAbortIgnoreRetry=0;
		
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}
			
		int iReturnValue = _set_xmtr_no_device( iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_xmtr_all_data",pchFunctionName)==0)
	case BUILTIN__set_xmtr_all_data:
	{
		int iAbortIgnoreRetry=0;
		
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[0];	
		}		
		else
		{
			return false;
		}		
		
		int iReturnValue = _set_xmtr_all_data( iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_set_xmtr_data",pchFunctionName)==0)
	case BUILTIN__set_xmtr_data:
	{
		int iByteCode=0;
		int iBitMask=0;
		int iAbortIgnoreRetry=0;
		
		
		// stevev 11feb08   if (pVarParameters[0].GetVarType() == RUL_INT)
		if (pVarParameters[0].isNumeric())
		{
			iByteCode=(int)pVarParameters[0];	
		}
		else
		{
			return false;
		}
		
		// stevev 11feb08   if (pVarParameters[1].GetVarType() == RUL_INT)
		if (pVarParameters[1].isNumeric())
		{
			iBitMask=(int)pVarParameters[1];	
		}
		else
		{
			return false;
		}

		// stevev 11feb08   if (pVarParameters[2].GetVarType() == RUL_INT)
		if (pVarParameters[2].isNumeric())
		{
			iAbortIgnoreRetry=(int)pVarParameters[2];	
		}
		else
		{
			return false;
		}
	
		int iReturnValue = _set_xmtr_data( iByteCode,iBitMask,iAbortIgnoreRetry);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("abort",pchFunctionName)==0)
	case BUILTIN_abort:
	{
		int iReturnValue = abort();
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		return true;
	}
	break;
	//else
	//if (strcmp("process_abort",pchFunctionName)==0)
	case BUILTIN_process_abort:
	{
		int iReturnValue = process_abort();
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		return true;
	}
	break;
	//else
	//if (strcmp("_add_abort_method",pchFunctionName)==0)
	case BUILTIN__add_abort_method:
	{
		long lMethodId=0;

		if (pVarParameters[0].isNumeric()) 
		{
			lMethodId=(int)pVarParameters[0];
		}
		else
		{
			return false;
		}
		
		int iReturnValue = _add_abort_method(lMethodId);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("_remove_abort_method",pchFunctionName)==0)
	case BUILTIN__remove_abort_method:
	{
		long lMethodId=0;

		if (pVarParameters[0].isNumeric()) 
		{
			lMethodId=(int)pVarParameters[0];
		}
		else
		{
			return false;
		}
		
		int iReturnValue = _remove_abort_method(lMethodId);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("remove_all_abort",pchFunctionName)==0)
	case BUILTIN_remove_all_abort:
	{
		int iReturnValue = remove_all_abort();
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
/*Arun 190505 Start of code*/
	break;
	//else
	//if (strcmp("push_abort_method",pchFunctionName)==0)
	case BUILTIN_push_abort_method:
	{
		long lMethodId=0;

		if (pVarParameters[0].isNumeric()) 
		{
			lMethodId=(int)pVarParameters[0];
		}
		else
		{
			return false;
		}

		int iReturnValue = push_abort_method(lMethodId);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("pop_abort_method",pchFunctionName)==0)
	case BUILTIN_pop_abort_method:
	{
		int iReturnValue = pop_abort_method();
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
/*End of code*/
	
	//else
	//if (strcmp("NaN_value",pchFunctionName)==0)
	case BUILTIN_NaN_value:
	{	
		UINT fReturnValue = 0;
		fReturnValue = NaN_value();
		pVarReturnValue->SetValue(&fReturnValue, RUL_FLOAT);
		return true;
	}	
	break;
	/* stevev - 26jun07 - add stub outs for required builtins */
	case BUILTIN_nan:
	{	
	/*	char pchString[MAX_DD_STRING]={0};
		int  pSize   = MAX_DD_STRING;

		if ( ! GetStringParam(pchString, pSize, pVarParameters, 0) )
		{
			return false;
		}

		double dReturnValue = nan(pchString);

		pVarReturnValue->SetValue(dReturnValue, RUL_DOUBLE);
		
		return true;
	*/
		return false;
	}	
	break;
	case BUILTIN_nanf:
	{		
	/*	char pchString[MAX_DD_STRING]={0};
		int  pSize   = MAX_DD_STRING;

		if ( ! GetStringParam(pchString, pSize, pVarParameters, 0) )
		{
			return false;
		}

		float fReturnValue = nanf(pchString);

		pVarReturnValue->SetValue(fReturnValue, RUL_FLOAT);
		
		return true;
	*/
		return false;
	}	
	break;
	case BUILTIN_fpclassify:
	{	
	/*	float fValue  = 0.0;
		double dValue = 0.0;
		int    retVal = 0;
		
		if (pVarParameters[0].varType == RUL_FLOAT)
		{
			fValue=(float)pVarParameters[0];
			retVal = _fpclassifyf(fValue);	
		}
		else
		if (pVarParameters[0].varType == RUL_DOUBLE)
		{
			dValue=(double)pVarParameters[0];
			retVal = _fpclassifyd(dValue);	
		}
		else
		{
			return false;
		}
		pVarReturnValue->SetValue(&retVal, RUL_INT);
		return true;
	***/
		return false;
	}	
	break;
	/* stevev 25jun07 - end stubouts ***/
	//else
	//if (strcmp("isetval",pchFunctionName)==0)
	case BUILTIN_isetval:
	{	
		INT64 iValue=0;

/*Vibhor 230204: Start of Code*/

		if (pVarParameters[0].isNumeric())
		{
		iValue=(INT64)pVarParameters[0];	
		}
		else
		{
			return false;
		}	
		
/*Vibhor 230204: End of Code*/

		int iReturnValue = isetval(iValue);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("lsetval",pchFunctionName)==0)
	case BUILTIN_lsetval:
	{	
		INT64 lValue=0;

/*Vibhor 230204: Start of Code*/

		if (pVarParameters[0].isNumeric())
		{
			lValue=(INT64)pVarParameters[0];	
		}
		else
		{
			return false;
		}

/*Vibhor 230204: End of Code*/
		int iReturnValue = lsetval(lValue);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("fsetval",pchFunctionName)==0)
	case BUILTIN_fsetval:
	{	
		float fValue=0.0;

/*Vibhor 230204: Start of Code*/

		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		fValue=(float)pVarParameters[0];	

/*Vibhor 230204: End of Code*/

		int iReturnValue = fsetval(fValue);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("dsetval",pchFunctionName)==0)
	case BUILTIN_dsetval:
	{	
		double dValue=0.0;

/*Vibhor 230204: Start of Code*/

		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		dValue=(double)pVarParameters[0];	

/*Vibhor 230204: End of Code*/

		int iReturnValue = dsetval(dValue);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("igetvalue",pchFunctionName)==0)
	case BUILTIN_igetvalue:
	{	
		INT64 iReturnValue = igetvalue();
		pVarReturnValue->SetValue(&iReturnValue, RUL_LONGLONG);
		return true;
	}
	break;
	//else
	//if (strcmp("igetval",pchFunctionName)==0)
	case BUILTIN_igetval:
	{	
		INT64 iReturnValue = igetval();
		pVarReturnValue->SetValue(&iReturnValue, RUL_LONGLONG);
		return true;
	}
	break;
	//else
	//if (strcmp("lgetval",pchFunctionName)==0)
	case BUILTIN_lgetval:
	{	
		INT64 lReturnValue = lgetval();
		pVarReturnValue->SetValue(&lReturnValue, RUL_LONGLONG);
		return true;
	}
	break;
	//else
	//if (strcmp("fgetval",pchFunctionName)==0)
	case BUILTIN_fgetval:
	{	
		float fReturnValue = fgetval();
		pVarReturnValue->SetValue(&fReturnValue, RUL_FLOAT);
		return true;
	}
	break;
	//else
	//if (strcmp("dgetval",pchFunctionName)==0)
	case BUILTIN_dgetval:
	{	
		double dReturnValue =(float)dgetval();
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}	
	break;
	/*Arun 200505 Start of code */
	//else
	//if (strcmp("sgetval",pchFunctionName)==0)
	case BUILTIN_sgetval:
	{	
		char pchString[MAX_DD_STRING]={0};
	//	int iStringLength;
		int iReturnValue=0;

		/* stevev 30may07 - sgetval only has one parameter
		if (pVarParameters[1].varType == RUL_INT)
		{
			iStringLength= pVarParameters[1];
		}
		else
		{
			return false;
		}
		*/

		iReturnValue = sgetval(pchString,MAX_DD_STRING);
		if ( ! SetCharStringParam(pFuncExp, pVarParameters, 0, pchString, MAX_DD_STRING) )
		{
			return false;
		}

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	//else
	//if (strcmp("ssetval",pchFunctionName)==0)
	case BUILTIN_ssetval:
	{	
		tchar pchString[MAX_DD_STRING]={0};
		int  pSize = MAX_DD_STRING;
		char* pchReturnValue = NULL;
		// only wide char is available from method literal strings
		if ( ! GetStringParam(pchString, pSize, pVarParameters, 0) )
		{
			return false;
		}

		wstring lW(pchString);
		 string lS;
		 lS = TStr2AStr(lW);
		pchReturnValue = ssetval(lS.c_str());

		pVarReturnValue->SetValue(pchReturnValue, RUL_CHARPTR);
		
		return true;
	}	
	break;
	/*End of code*/
	//else
	//if (strcmp("send",pchFunctionName)==0)
	case BUILTIN_send:
	{	
		int iCmd_no=-1;
		//uchar pchString[MAX_DD_STRING]={0};
		BYTE_STRING( byteString,MAX_DD_STRING);

		if (pVarParameters[0].isNumeric())
		{
			iCmd_no=(int)pVarParameters[0];
		}
		else
		{
			DEL_BYTE_STR( byteString );
			return false;
		}

		//int iReturnValue = send(iCmd_no,pchString);
		int iReturnValue = send(iCmd_no,byteString.bs);
		byteString.bsLen = STATUS_SIZE;// adjust to onlly the desired bytes
		
		if ( ! SetByteStringParam(pFuncExp, pVarParameters, 1, byteString) )
		{
			DEL_BYTE_STR( byteString );
			return false;
		}

		//jad fcg repo has METHOD_ABORTED only
		/*
				if ( (iReturnValue == BI_ABORT)
					|| (iReturnValue == BI_NO_DEVICE)
					|| (iReturnValue == BI_COMM_ERR)
								|| (iReturnValue == METHOD_ABORTED)   )
		*/
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		DEL_BYTE_STR( byteString );
		return true;
	}	
	break;
	//else
	//if (strcmp("send_command",pchFunctionName)==0)
	case BUILTIN_send_command:
	{	
		int iCmd_no;
	
		if (pVarParameters[0].isNumeric())
		{
				iCmd_no=(int)pVarParameters[0];
		}
		else
		{
			return false;
		}

				
		int iReturnValue = send_command(iCmd_no);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		//jad fcg repo has METHOD_ABORTED only
		/*
				if ( (iReturnValue == BI_ABORT)
					|| (iReturnValue == BI_NO_DEVICE)
					|| (iReturnValue == BI_COMM_ERR)
								|| (iReturnValue == METHOD_ABORTED)   )
		*/
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		return true;
	}	
	break;
	//else
	//if (strcmp("send_command_trans",pchFunctionName)==0)
	case BUILTIN_send_command_trans:
	{	
		int iCmd_no=-1;
		int iTrans_no=-1;
		
		if (pVarParameters[0].isNumeric())
		{
				iCmd_no=(int)pVarParameters[0];
		}
		else
		{
			return false;
		}

		if (pVarParameters[1].isNumeric())
		{
				iTrans_no=(int)pVarParameters[1];
		}	
		else
		{
			return false;
		}	
				
		int iReturnValue = send_command_trans(iCmd_no,iTrans_no);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		//jad fcg repo has METHOD_ABORTED only
		/*
				if ( (iReturnValue == BI_ABORT)
					|| (iReturnValue == BI_NO_DEVICE)
					|| (iReturnValue == BI_COMM_ERR)
								|| (iReturnValue == METHOD_ABORTED)   )
		*/
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		return true;
	}	
	break;
	//else
	//if (strcmp("send_trans",pchFunctionName)==0)
	case BUILTIN_send_trans:
	{	
		int iCmd_no=-1;
		int iTrans_no=-1;
		//uchar pchString[MAX_DD_STRING]={0};
		BYTE_STRING( byteString, MAX_DD_STRING);
		
		if (pVarParameters[0].isNumeric())
		{
			iCmd_no=(int)pVarParameters[0];
		}
		else
		{
			DEL_BYTE_STR( byteString );
			return false;
		}

		if (pVarParameters[1].isNumeric())
		{
			iTrans_no=(int)pVarParameters[1];
		}	
		else
		{
			DEL_BYTE_STR( byteString );
			return false;
		}

				
		int iReturnValue = send_trans(iCmd_no,iTrans_no,byteString.bs);//was  pchString);
		byteString.bsLen = STATUS_SIZE;// adjust to onlly the desired bytes
		
		if ( ! SetByteStringParam(pFuncExp, pVarParameters, 2, byteString) )
		{
			DEL_BYTE_STR( byteString );
			return false;
		}

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		//jad fcg repo has METHOD_ABORTED only
		/*
				if ( (iReturnValue == BI_ABORT)
					|| (iReturnValue == BI_NO_DEVICE)
					|| (iReturnValue == BI_COMM_ERR)
								|| (iReturnValue == METHOD_ABORTED)   )
		*/
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		DEL_BYTE_STR( byteString );
		return true;
	}	
	break;
	//else
	//if (strcmp("ext_send_command",pchFunctionName)==0)
	case BUILTIN_ext_send_command:
	{	
		int iCmd_no=-1;
		//uchar pchString_RespStatus[MAX_DD_STRING]={0};
		//uchar pchString_MoreDataStatus[MAX_DD_STRING]={0};
		//uchar pchString_MoreDataInfo[MAX_DD_STRING]={0};
		BYTE_STRING( status,     MAX_DD_STRING);
		BYTE_STRING( morestatus, MAX_DD_STRING);
		BYTE_STRING( info,       MAX_DD_STRING);

		int  InfoSize = 0;

		
		if (pVarParameters[0].isNumeric())
		{
				iCmd_no=(int)pVarParameters[0];
		}
		else
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}

						
		//int iReturnValue = ext_send_command(iCmd_no, pchString_RespStatus, pchString_MoreDataStatus,
		//													 pchString_MoreDataInfo,moreInfoSize);
		int iReturnValue = ext_send_command(iCmd_no, status.bs, morestatus.bs,info.bs,InfoSize);
		info.bsLen = InfoSize;
		
		if ( ! SetByteStringParam(pFuncExp, pVarParameters, 1, status) )
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}
		
		if ( ! SetByteStringParam(pFuncExp, pVarParameters, 2, morestatus) )
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}
		
		if ( ! SetByteStringParam(pFuncExp, pVarParameters, 3, info) )
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		//jad fcg repo has METHOD_ABORTED only
		/*
				if ( (iReturnValue == BI_ABORT)
					|| (iReturnValue == BI_NO_DEVICE)
					|| (iReturnValue == BI_COMM_ERR)
								|| (iReturnValue == METHOD_ABORTED)   )
		*/
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		DEL_BYTE_STR( status );
		DEL_BYTE_STR( morestatus );
		DEL_BYTE_STR( info );
		return true;
	}	
	break;
	//else
	//if (strcmp("ext_send_command_trans",pchFunctionName)==0)
	case BUILTIN_ext_send_command_trans:
	{	
		int iCmd_no=-1;
		int iTrans_no=-1;
		//int pSizeInfo = 0;
		//uchar pchString_RespStatus[MAX_DD_STRING]={0};
		//uchar pchString_MoreDataStatus[MAX_DD_STRING]={0};
		//uchar pchString_MoreDataInfo[MAX_DD_STRING]={0};
		BYTE_STRING( status,     MAX_DD_STRING);
		BYTE_STRING( morestatus, MAX_DD_STRING);
		BYTE_STRING( info,       MAX_DD_STRING);

		int  InfoSize = 0;


		
		if (pVarParameters[0].isNumeric())// cmd number
		{
				iCmd_no=(int)pVarParameters[0];
		}
		else
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}

		if (pVarParameters[1].isNumeric())// transaction number
		{
				iTrans_no=(int)pVarParameters[1];
		}
		else
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}

						
		//int iReturnValue = ext_send_command_trans(iCmd_no,iTrans_no,pchString_RespStatus,
		//	                                                        pchString_MoreDataStatus,
		//															pchString_MoreDataInfo,pSizeInfo);
		int iReturnValue = ext_send_command_trans(iCmd_no,iTrans_no,status.bs,morestatus.bs,
																	info.bs,InfoSize);
		info.bsLen = InfoSize;
	
		if ( !SetByteStringParam(pFuncExp, pVarParameters, 2, status) )
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}
		
		if ( !SetByteStringParam(pFuncExp, pVarParameters, 3, morestatus) )
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}
		
		if ( !SetByteStringParam(pFuncExp, pVarParameters, 4, info) )
		{
			DEL_BYTE_STR( status );
			DEL_BYTE_STR( morestatus );
			DEL_BYTE_STR( info );
			return false;
		}

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		//jad fcg repo has METHOD_ABORTED only
		/*
				if ( (iReturnValue == BI_ABORT)
					|| (iReturnValue == BI_NO_DEVICE)
					|| (iReturnValue == BI_COMM_ERR)
								|| (iReturnValue == METHOD_ABORTED)   )
		*/
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		DEL_BYTE_STR( status );
		DEL_BYTE_STR( morestatus );
		DEL_BYTE_STR( info );
		return true;
	}	
	break;
	//else
	//if (strcmp("tsend_command",pchFunctionName)==0)
	case BUILTIN_tsend_command:
	{	
		int iCmd_no=-1;
	
		if (pVarParameters[0].isNumeric())
		{
			iCmd_no=(int)pVarParameters[0];
		}
		else
		{
			return false;
		}

				
		int iReturnValue = tsend_command(iCmd_no);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		//jad fcg repo has METHOD_ABORTED only
		/*
				if ( (iReturnValue == BI_ABORT)
					|| (iReturnValue == BI_NO_DEVICE)
					|| (iReturnValue == BI_COMM_ERR)
								|| (iReturnValue == METHOD_ABORTED)   )
		*/
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		return true;
	}	
	break;
	//else
	//if (strcmp("tsend_command_trans",pchFunctionName)==0)
	case BUILTIN_tsend_command_trans:
	{	
		int iCmd_no=-1;
		int iTrans_no=-1;
		
		if (pVarParameters[0].isNumeric())
		{
				iCmd_no=(int)pVarParameters[0];
		}
		else
		{
			return false;
		}

		if (pVarParameters[1].isNumeric())
		{
				iTrans_no=(int)pVarParameters[1];
		}	
		else
		{
			return false;
		}	
				
		int iReturnValue = tsend_command_trans(iCmd_no,iTrans_no);
		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		//jad fcg repo has METHOD_ABORTED only
		/*
				if ( (iReturnValue == BI_ABORT)
					|| (iReturnValue == BI_NO_DEVICE)
					|| (iReturnValue == BI_COMM_ERR)
								|| (iReturnValue == METHOD_ABORTED)   )
		*/
		if (iReturnValue == METHOD_ABORTED)
		{
			*pBuiltinReturnCode = BUILTIN_ABORT;
		}
		return true;
	}
	break;
		
	// Anil December 16 2005 deleted the Plot builtins case

/*Arun 110505 Start of code*/
/*****************************************Math Builtions (eDDL) ****************************/
	//else
	//if (strcmp("abs",pchFunctionName)==0)
	case BUILTIN_abs:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =(double)abs(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("acos",pchFunctionName)==0)
	case BUILTIN_acos:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =acos(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("asin",pchFunctionName)==0)
	case BUILTIN_asin:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =asin(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("atan",pchFunctionName)==0)
	case BUILTIN_atan:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =atan(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("cbrt",pchFunctionName)==0)
	case BUILTIN_cbrt:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =cbrt(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("ceil",pchFunctionName)==0)
	case BUILTIN_ceil:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =ceil(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("cos",pchFunctionName)==0)
	case BUILTIN_cos:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =cos(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("cosh",pchFunctionName)==0)
	case BUILTIN_cosh:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =cosh(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("exp",pchFunctionName)==0)
	case BUILTIN_exp:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =exp(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("floor",pchFunctionName)==0)
	case BUILTIN_floor:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =floor(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("fmod",pchFunctionName)==0)
	case BUILTIN_fmod:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValueX=(double)pVarParameters[0];	
		RETURN_AT_NOT_NUMERIC(1);// added stevev 18feb08		
		double dValueY=(double)pVarParameters[1];	
		double dReturnValue =fmod(dValueX,dValueY);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	case BUILTIN_frand:
	{
		double dReturnValue =frand();
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("log",pchFunctionName)==0)
	case BUILTIN_log:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =log(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("log10",pchFunctionName)==0)
	case BUILTIN_log10:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =log10(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;
	}
	break;
	//else
	//if (strcmp("log2",pchFunctionName)==0)
	case BUILTIN_log2:	
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =log2(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("pow",pchFunctionName)==0)
	case BUILTIN_pow:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValueX=(double)pVarParameters[0];	
		RETURN_AT_NOT_NUMERIC(1);// added stevev 18feb08		
		double dValueY=(double)pVarParameters[1];	
		double dReturnValue =pow(dValueX,dValueY);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("round",pchFunctionName)==0)
	case BUILTIN_round:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =round(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("sin",pchFunctionName)==0)
	case BUILTIN_sin:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =sin(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("sinh",pchFunctionName)==0)
	case BUILTIN_sinh:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =sinh(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("sqrt",pchFunctionName)==0)
	case BUILTIN_sqrt:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =sqrt(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("tan",pchFunctionName)==0)
	case BUILTIN_tan:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =tan(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("tanh",pchFunctionName)==0)
	case BUILTIN_tanh:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =tanh(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("trunc",pchFunctionName)==0)
	case BUILTIN_trunc:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		double dValue=(double)pVarParameters[0];	
		double dReturnValue =trunc(dValue);
		pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
		return true;	
	}
	break;
	//else
	//if (strcmp("atof",pchFunctionName)==0)
	case BUILTIN_atof:
	{// stevev 11feb08 -results of inter_varient rework
	//	char* dValue=(char*)pVarParameters[0];	
		char* dValue = NULL;
		pVarParameters[0].GetStringValue(&dValue);
		if (dValue)
		{
			double dReturnValue =atof(dValue);
			pVarReturnValue->SetValue(&dReturnValue, RUL_DOUBLE);
			delete[] dValue;
			return true;
		}
		return false;
	}
	break;
	//else
	//if (strcmp("atoi",pchFunctionName)==0)
	case BUILTIN_atoi:
	{// stevev 11feb08 -results of inter_varient rework
	//	char* dValue=(char*)pVarParameters[0];	
		char* dValue = NULL;
		pVarParameters[0].GetStringValue(&dValue);
		if (dValue)
		{
			int dReturnValue =atoi(dValue);
			pVarReturnValue->SetValue(&dReturnValue, RUL_INT);
			delete[] dValue;
			return true;
		}
		return false;
	}
	break;
	//else
	//if (strcmp("itoa",pchFunctionName)==0)
	case BUILTIN_itoa:
	{
		RETURN_AT_NOT_NUMERIC(0);// added stevev 18feb08		
		int dValue1=(int)pVarParameters[0];	

//jad debug
//LOGIT(COUT_LOG,"dValue1 = %d",dValue1);

		wchar_t dValue2[MAX_DD_STRING+1]={0};// NOT Unicode
		RETURN_AT_NOT_NUMERIC(2);// added stevev 18feb08		
		int dValue3=(int)pVarParameters[2];	

//jad debug
//LOGIT(COUT_LOG,"dValue3 = %d",dValue3);


		wchar_t* dReturnValue =itoa(dValue1,dValue2,dValue3);
//jad debug
//LOGIT(COUT_LOG,"dValue2 = %d",dValue2);
//jad debug
//LOGIT(COUT_LOG,"dReturnValue = %s",dReturnValue);

		//Anil 250407 The variable is the out Param so we should get the LocalVariable name and 
		//then update this value through interpreter pointer 
		//Method.h should have #define itoa (a,b,c)          itoa((a), LOCALVAR (b), (c))
		char szLocaVarName[MAX_DD_STRING];
		int  pSize = MAX_DD_STRING;
		
		if ( ! GetCharStringParam(szLocaVarName, pSize, pVarParameters, 1) )
		{
			return false;
		}

		INTER_VARIANT varTemp;
		char szLang[5] = {0};
		bool bLangPresent=false;
		//Remove the Language code , if it was apended <a tokenizer bug>
		GetLanguageCode(szLocaVarName,szLang,&bLangPresent);			

//jad honeywell bug
//use return value
//		varTemp.SetValue( dValue2, RUL_DD_STRING );	
		varTemp.SetValue( dReturnValue, RUL_DD_STRING );	
		//Update the DD local var szLocaVarName with the value lselection
		int x = m_pInterpreter->SetVariableValue(szLocaVarName,varTemp);

//jad honeywell bug with DELAY text
//anil, WRONG!
//need to use return value

		//anil 250407 Return value shoul be set to dValue2 and not dReturnValue
//		pVarReturnValue->SetValue(dValue2, RUL_DD_STRING);
		pVarReturnValue->SetValue(dReturnValue, RUL_DD_STRING);

		return true;	
	}
	break;
/*****************************************End of Math Builtins (eDDL) *********************/

/*End of code*/

/* Arun 160505 Start of code */

/****************************************Date Time Builtins (eDDL)*************************/
	//else
	//if (strcmp("YearMonthDay_to_Date",pchFunctionName)==0)
//	case BUILTIN_YearMonthDay_to_Date:/* WS:EPM Not a builtin-25jun07 */
//	{
//		int dValue1=(int)pVarParameters[0];	
//		int dValue2=(int)pVarParameters[1];	
//		int dValue3=(int)pVarParameters[2];	
//		long dReturnValue =YearMonthDay_to_Date(dValue1,dValue2,dValue3);
//		pVarReturnValue->SetValue(&dReturnValue, RUL_INT);
//		return true;	
//	}
//	break;
	//else
	//if (strcmp("Date_to_Year",pchFunctionName)==0)
	case BUILTIN_Date_to_Year:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		long lValue=(int)pVarParameters[0];	/* WS:EPM-changed types-25jun07*/
		int lReturnValue =Date_to_Year(lValue);// WS - 9apr07 - 2005 checkin
		pVarReturnValue->SetValue(&lReturnValue, RUL_INT);
		return true;	
	}
	break;
	//else
	//if (strcmp("Date_to_Month",pchFunctionName)==0)
	case BUILTIN_Date_to_Month:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		long lValue=(int)pVarParameters[0];	/* WS:EPM-changed types-25jun07*/
		int lReturnValue =Date_to_Month(lValue); // WS - 9apr07 - 2005 checkin
		pVarReturnValue->SetValue(&lReturnValue, RUL_INT);
		return true;	
	}
	break;
	//else
	//if (strcmp("Date_to_DayOfMonth",pchFunctionName)==0)
	case BUILTIN_Date_to_DayOfMonth:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		long lValue=(int)pVarParameters[0];	/* WS:EPM-changed types-25jun07*/
		int lReturnValue =Date_to_DayOfMonth(lValue); // WS - 9apr07 - 2005 checkin
		pVarReturnValue->SetValue(&lReturnValue, RUL_INT);
		return true;	
	}
	break;
	//else
	//if (strcmp("GetCurrentDate",pchFunctionName)==0)
	//case BUILTIN_GetCurrentDate:/* WS:EPM Not a builtin-25jun07 */
	//{
	//	long dReturnValue =GetCurrentDate();
	//	pVarReturnValue->SetValue(&dReturnValue, RUL_INT);
	//	return true;	
	//}
	break;
	//else
	//if (strcmp("GetCurrentTime",pchFunctionName)==0)
	case BUILTIN_GetCurrentTime:
	{
		long lReturnValue=GetCurrentTime();/* WS:EPM-changed types-25jun07*/
		pVarReturnValue->SetValue(&lReturnValue, RUL_INT);
		return true;	
	}
	break;
	//else
	//if (strcmp("GetCurrentDateAndTime",pchFunctionName)==0)
	//case BUILTIN_GetCurrentDateAndTime:/* WS:EPM Not a builtin-25jun07 */
	//{
	//	float dReturnValue =GetCurrentDateAndTime();
	//	pVarReturnValue->SetValue(&dReturnValue, RUL_FLOAT);
	//	return true;	
	//}
	//break;
	//else
	//if (strcmp("To_Date_and_Time",pchFunctionName)==0)
	//case BUILTIN_To_Date_and_Time:/* WS:EPM Not a builtin-25jun07 */
	//{
	//	int dValue1=(int)pVarParameters[0];	
	//	int dValue2=(int)pVarParameters[0];	
	//	int dValue3=(int)pVarParameters[0];	
	//	int dValue4=(int)pVarParameters[0];	
	//	int dValue5=(int)pVarParameters[0];	
	//	float dReturnValue =To_Date_and_Time(dValue1,dValue2,dValue3,dValue4,dValue5);
	//	pVarReturnValue->SetValue(&dReturnValue, RUL_FLOAT);
	//	return true;	
	//}
	//break;
	
/***************************************Date Time Builtins (eDDL)**************************/


/****************************Start of DD_STRING  Builtins  (eDDL) ********************/
//Added By Anil June 17 2005 --starts here
	//else
	//if (strcmp("STRSTR",pchFunctionName)==0)
	case BUILTIN_strstr:
	{
		wchar_t* string_var = NULL;
		wchar_t* substring_to_find = NULL;
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		
		//Get the String value 
		pVarParameters[0].GetStringValue(&string_var,RUL_DD_STRING);
		pVarParameters[1].GetStringValue(&substring_to_find,RUL_DD_STRING);
		
		//For Second string strip off the Language Code. Ideally SPeaking For first also we need to strip off and then Campare and then APend it banck
		
		GetLanguageCode(substring_to_find,szLangCode,&bLanCodePrese);
		//Append the Language code  When ur Returning
		GetLanguageCode(string_var,szLangCode,&bLanCodePrese);	
		
		wchar_t* szTemp = STRSTR(string_var,substring_to_find);
		//As per Vibhor's Suggestion if the string Not Found this is made ad terminated string
		if(szTemp == NULL)
		{
			pVarReturnValue->SetValue((void*)(_T("\0")), RUL_DD_STRING);
		}
		else
		{
			if(bLanCodePrese ==true)

			{	
#ifdef MODIFIED_BY_SS
				_INT32 nLen = wcslen(szTemp) + 1 + 4;//Length + 1 + Language Code
				wchar_t *szTempValue = new wchar_t[nLen]; 
				// error.....memset(szTempValue,0,sizeof(szTempValue));stevev 29dec11
				//memset(szTempValue, 0, sizeof(wchar_t) * nLen);
				/*wcscpy(szTempValue,szLangCode);
				wcscat(szTempValue,szTemp);*/
				
				std::wstring strTempValue = szLangCode;
				strTempValue += szTemp;

				// If you want to resize the string szTempValue to nLen characters
				if (strTempValue.size() < nLen)
					strTempValue.resize(nLen);
				std::copy(strTempValue.begin(), strTempValue.end(), szTempValue);
				// Alternatively, if you want to ensure it's exactly nLen characters
				//szTempValue.resize(nLen - 1, L' '); // fill with spaces

				
#endif
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTempValue, RUL_DD_STRING);
				if(szTempValue)
				{
					delete[] szTempValue;
					szTempValue = NULL;
				}
			}
			else
			{
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTemp, RUL_DD_STRING);

			}				
		}		
		//Delete all the Memory Allocated;
		if(string_var)
		{
			delete[] string_var;
			string_var = NULL;
		}
		if(substring_to_find)
		{
			delete[] substring_to_find;
			substring_to_find = NULL;
		}
		return true;	
	}
	break;

	//else
	//if (strcmp("STRUPR",pchFunctionName)==0)
	case BUILTIN_strupr:
	{
		wchar_t* string_var = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		
		//Get the String value 
		pVarParameters[0].GetStringValue(&string_var,RUL_DD_STRING);		
		
		//For Second string strip off the Language Code. Ideally SPeaking For first also we need to strip off and then Campare and then APend it banck
		
		//Append the Language code  When ur Returning
		GetLanguageCode(string_var,szLangCode,&bLanCodePrese);	
		
		wchar_t* szTemp = STRUPR(string_var);
		//As per Vibhor's Suggestion if the string Not Found this is made ad terminated string
		if(szTemp == NULL)
		{
			pVarReturnValue->SetValue((void*)(_T("\0")), RUL_DD_STRING);
		}
		else
		{
			if(bLanCodePrese ==true)

			{	_INT32 nLen = wcslen(szTemp)+1+4;//Length + 1 + Language Code
				wchar_t *szTempValue = new wchar_t[nLen]; 
				// error...memset(szTempValue,0,sizeof(szTempValue));stevev 29dec11
				memset(szTempValue,0,sizeof(wchar_t)*nLen);
#ifdef MODIFIED_BY_SS
			/*	wcscpy(szTempValue, szLangCode);
				wcscat(szTempValue, szTemp);*/
				std::wstring str(szLangCode);
				str.append(szTemp);
				str.copy(szTempValue, sizeof(wchar_t)* nLen);
#endif
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTempValue, RUL_DD_STRING);
				if(szTempValue)
				{
					delete[] szTempValue;
					szTempValue	= NULL;
				}

			}
			else
			{
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTemp, RUL_DD_STRING);

			}	
			
		}		
		//Delete all the Memory Allocated;
		if(string_var)
		{
			delete[] string_var;
			string_var= NULL;
		}
		return true;	
	}
	break;

	//else
	//if (strcmp("STRLWR",pchFunctionName)==0)
	case BUILTIN_strlwr:
	{
		wchar_t* string_var = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		
		//Get the String value 
		pVarParameters[0].GetStringValue(&string_var,RUL_DD_STRING);		
		
		//For Second string strip off the Language Code. Ideally SPeaking For first also we need to strip off and then Campare and then APend it banck
		
		//Append the Language code  When ur Returning
		GetLanguageCode(string_var,szLangCode,&bLanCodePrese);	
		
		wchar_t* szTemp = STRLWR(string_var);
		//As per Vibhor's Suggestion if the string Not Found this is made ad terminated string
		if(szTemp == NULL)
		{
			pVarReturnValue->SetValue((void*)_T("\0"), RUL_DD_STRING);
		}
		else
		{
			if(bLanCodePrese ==true)

			{	_INT32 nLen = wcslen(szTemp)+1+4;//Length + 1 + Language Code
				wchar_t *szTempValue = new wchar_t[nLen]; 
				//error...memset(szTempValue,0,sizeof(szTempValue));stevev 29dec11
				memset(szTempValue,0,sizeof(wchar_t)*nLen);
#ifdef MODIFIED_BY_SS
				/*wcscpy(szTempValue,szLangCode);
				wcscat(szTempValue,szTemp);*/
#ifdef MODIFIED_BY_SS
				/*	wcscpy(szTempValue, szLangCode);
					wcscat(szTempValue, szTemp);*/
				std::wstring str(szLangCode);
				str.append(szTemp);
				str.copy(szTempValue, sizeof(wchar_t)* nLen);
#endif

#endif 
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTempValue, RUL_DD_STRING);
				if(szTempValue)
				{
					delete[] szTempValue;
					szTempValue = NULL;
				}

			}
			else
			{
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTemp, RUL_DD_STRING);

			}	
			
		}		
		//Delete all the Memory Allocated;
		if(string_var)
		{
			delete[] string_var;
			string_var = NULL;
		}
		return true;	
	}
	break;

	//else
	//if (strcmp("STRLEN",pchFunctionName)==0)
	case BUILTIN_strlen:
	{
		wchar_t* string_var = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		
		//Get the String value 
		pVarParameters[0].GetStringValue(&string_var,RUL_DD_STRING);		
		
		//For Second string strip off the Language Code. Ideally SPeaking For first also we need to strip off and then Campare and then APend it banck
		
		//Append the Language code  When ur Returning
		GetLanguageCode(string_var,szLangCode,&bLanCodePrese);	
		
		int istrLen = STRLEN(string_var);
		pVarReturnValue->SetValue(&istrLen, RUL_INT);
		if(string_var)
		{
			delete[] string_var;
			string_var = NULL;
		}
		return true;	
	}
	break;


	//else
	//if (strcmp("STRCMP",pchFunctionName)==0)
	case BUILTIN_strcmp:
	{
		wchar_t* string_var1 = NULL;
		wchar_t* string_var2 = NULL;
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		
		//Get the String value 
		pVarParameters[0].GetStringValue(&string_var1,RUL_DD_STRING);
		pVarParameters[1].GetStringValue(&string_var2,RUL_DD_STRING);
		
		//For Second string strip off the Language Code. Ideally SPeaking For first also we need to strip off and then Campare and then APend it banck
		
		GetLanguageCode(string_var1,szLangCode,&bLanCodePrese);	
		GetLanguageCode(string_var2,szLangCode,&bLanCodePrese);	
		
		int iCmp = STRCMP(string_var1,string_var2);
		pVarReturnValue->SetValue(&iCmp, RUL_INT);
		//Delete all the Memory Allocated;
		if(string_var1)
		{
			delete[] string_var1;
			string_var1 = NULL;
		}
		if(string_var2)
		{
			delete[] string_var2;
			string_var2 = NULL;
		}
		return true;
	}
	break;

	//else
	//if (strcmp("STRTRIM",pchFunctionName)==0)
	case BUILTIN_strtrim:
	{
		wchar_t* string_var = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		
		//Get the String value 
		pVarParameters[0].GetStringValue(&string_var,RUL_DD_STRING);		
		
		//For Second string strip off the Language Code. Ideally SPeaking For first also we need to strip off and then Campare and then APend it banck
		
		//Append the Language code  When ur Returning
		GetLanguageCode(string_var,szLangCode,&bLanCodePrese);	
		
		wchar_t* szTemp = STRTRIM(string_var);
		//As per Vibhor's Suggestion if the string Not Found this is made ad terminated string
		if(szTemp == NULL)
		{
			pVarReturnValue->SetValue((void*)_T("\0"), RUL_DD_STRING);
		}
		else
		{
			if(bLanCodePrese ==true)

			{	_INT32 nLen = wcslen(szTemp)+1+4;//Length + 1 + Language Code
				wchar_t *szTempValue = new wchar_t[nLen]; 
				// error...memset(szTempValue,0,sizeof(szTempValue));stevev 29dec11
				memset(szTempValue,0,sizeof(wchar_t)*nLen);
#ifdef MODIFIED_BY_SS
				/*	wcscpy(szTempValue, szLangCode);
					wcscat(szTempValue, szTemp);*/
				std::wstring str(szLangCode);
				str.append(szTemp);
				str.copy(szTempValue, sizeof(wchar_t)* nLen);
#endif
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTempValue, RUL_DD_STRING);
				if(szTempValue)
				{
					delete[] szTempValue;
					szTempValue = NULL;
				}

			}
			else
			{
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTemp, RUL_DD_STRING);

			}	
			
		}
		//Delete all the Memory Allocated;
		if(string_var)
		{
			delete[] string_var;
			string_var = NULL;
		}
		return true;	
	}
	break;
	
	//else
	//if (strcmp("STRMID",pchFunctionName)==0)
	case BUILTIN_strmid:
	{
		wchar_t* string_var = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		
		//Get the String value 
		pVarParameters[0].GetStringValue(&string_var,RUL_DD_STRING);
		
		//Append the Language code  When ur Returning
		GetLanguageCode(string_var,szLangCode,&bLanCodePrese);
		RETURN_AT_NOT_NUMERIC(1);// added stevev 18feb08		
		int iStrat=(int)pVarParameters[1];	
		RETURN_AT_NOT_NUMERIC(2);// added stevev 18feb08		
		int iLen=(int)pVarParameters[2];	
		
		wchar_t* szTemp = STRMID(string_var,iStrat,iLen);
		//As per Vibhor's Suggestion if the string Not Found this is made ad terminated string
		if(szTemp == NULL)
		{
			pVarReturnValue->SetValue((void*)_T("\0"), RUL_DD_STRING);
		}
		else
		{
			if(bLanCodePrese ==true)

			{	_INT32 nLen = wcslen(szTemp)+1+4;//Length + 1 + Language Code
				wchar_t *szTempValue = new wchar_t[nLen]; 
				// error...memset(szTempValue,0,sizeof(szTempValue));stevev 29dec11
				memset(szTempValue,0,sizeof(wchar_t)*nLen);
#ifdef MODIFIED_BY_SS
				/*	wcscpy(szTempValue, szLangCode);
					wcscat(szTempValue, szTemp);*/
				std::wstring str(szLangCode);
				str.append(szTemp);
				str.copy(szTempValue, sizeof(wchar_t)* nLen);
#endif
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTempValue, RUL_DD_STRING);
				if(szTempValue) //Uncomment latter Boss as it was commnted to make it compile as nesting was going beyond limit
				{
					delete[] szTempValue;				
					szTempValue = NULL;
				}
			}
			else
			{
				//Set this vlue in the variant
				pVarReturnValue->SetValue(szTemp, RUL_DD_STRING);

			}	
			
		}		
	
		//Delete all the Memory Allocated;
		if(string_var)
		{
			delete[] string_var;
			string_var = NULL;
		}
		
		if(szTemp)
		{
			delete[] szTemp;
			szTemp = NULL;
		}

		return true;	
	}
	break;
/*Vibhor 200905: Start of Code*/
	case BUILTIN__ListInsert:
		{
			long lListId = 0;
			int iIndx = -1;
			long lItemId = 0;
			//Get the List Id
			if(pVarParameters[0].isNumeric())
			{
				lListId = (int)pVarParameters[0];
			}

			//Get the Index at which the insertion is required
			if(pVarParameters[1].isNumeric())
			{
				iIndx = (int)pVarParameters[1];
			}

			//Get the Id of the item which needs to be inserted in the list
			if(pVarParameters[2].isNumeric())
			{
				lItemId = (int)pVarParameters[2];
			}

			int iReturnValue = _ListInsert(lListId,iIndx,lItemId);
				
			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
	
			return true;
		}
		break;

	case BUILTIN__ListDeleteElementAt:
		{
			long lListId = 0;
			int iIndx = -1;

			//Get the List Id
			if(pVarParameters[0].isNumeric())
			{
				lListId = (int)pVarParameters[0];
			}

			//Get the Index of the element which needs to be deleted.
			if(pVarParameters[1].isNumeric())
			{
				iIndx = (int)pVarParameters[1];
			}

			int iReturnValue = _ListDeleteElementAt(lListId,iIndx);

			pVarReturnValue->SetValue(&iReturnValue, RUL_INT);

			return true;
		}
		break;
/*Vibhor 200905: End of Code*/

	//Anil September 26 2005 added MenuDisplay Start of Code
	case BUILTIN__MenuDisplay:
		{
			tchar szOptionList[MAX_DD_STRING];
			int  pSize = MAX_DD_STRING;
			long lMenuId = 0;
			char szLocaVarName[MAX_DD_STRING];
			long lselection = 0;

			//Get the Menu Id
			if(pVarParameters[0].isNumeric())
			{
				lMenuId = (int)pVarParameters[0];
			}
			if ( ! GetStringParam(szOptionList, pSize, pVarParameters, 1) )
			{
				return false;
			}

			//This is the DD Local Varible which is passes by reference
			//So when we go out of this function, We need to update this value with the out param of
			//_MenuDisplay below
			if ( ! GetCharStringParam(szLocaVarName, pSize, pVarParameters, 2) )
			{
				return false;
			}
			
			int iReturnValue = _MenuDisplay(lMenuId,szOptionList,&lselection);
			
			//Vibhor 221106: 
			if (iReturnValue == METHOD_ABORTED)
			{
				LOGIT(CLOG_LOG,"iReturnValue == METHOD_ABORTED");

				*pBuiltinReturnCode = BUILTIN_ABORT;
			}
			else
			{	//We got the selction which,actually has to be stored in szLocaVarName
				//Hence the below additional code
				//Create a inter varinat with value equal to lselection and data type as RUL_INT
				INTER_VARIANT varTemp;
				char szLang[5] = {0};
				bool bLangPresent=false;
				//		Remove the Language code , if it was apended
				GetLanguageCode(szLocaVarName,szLang,&bLangPresent);			
				varTemp.SetValue(&lselection,RUL_INT);	
				//		Update the DD local var szLocaVarName with the value lselection
				int x = m_pInterpreter->SetVariableValue(szLocaVarName,varTemp);

				pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
			}
			return true;
		}
		break;
	//Anil September 26 2005 added MenuDisplay End of Code

	case BUILTIN_DiffTime:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(1);/* stevev-added check-25jun07*/
		// WS - 25jun07 - changed data types, fixed index //
		long time_t1=(int)pVarParameters[0];// WS - 9apr07 - 2005 checkin	
		long time_t0=(int)pVarParameters[1];// WS - 9apr07 - 2005 checkin	

		double dDiffTime =DiffTime(time_t1,time_t0);// WS - 9apr07 - 2005 checkin

		pVarReturnValue->SetValue(&dDiffTime, RUL_DOUBLE);
		return true;	
	}
	break;

	case BUILTIN_AddTime:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(1);/* stevev-added check-25jun07*/
		// WS - 25jun07 - changed data types //
		long time_t1 =(int)pVarParameters[0];// WS - 9apr07 - 2005 checkin	
		long lseconds=(int)pVarParameters[1];	
		long lAddedTime =AddTime(time_t1,lseconds);
		pVarReturnValue->SetValue(&lAddedTime, RUL_INT);
		return true;	
	}
	break;

	case BUILTIN_Make_Time:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(1);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(2);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(3);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(4);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(5);/* stevev-added check-25jun07*/
		int year		= (int)pVarParameters[0];
		int month		= (int)pVarParameters[1];
		int dayofmonth	= (int)pVarParameters[2];
		int hour		= (int)pVarParameters[3]; 
		int minute		= (int)pVarParameters[4]; 
		int second		= (int)pVarParameters[5]; 
		int isDST		= (int)pVarParameters[6];

		long lConvTime	=	Make_Time(year,month , dayofmonth, hour, minute, second ,isDST);

		pVarReturnValue->SetValue(&lConvTime, RUL_INT);//WS-fixed return type 25jun07
		return true;	
	}
	break;

	case BUILTIN_To_Time:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(1);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(2);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(3);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(4);/* stevev-added check-25jun07*/
		long date		= (int)pVarParameters[0];// WS - 9apr07 - 2005 checkin 
		int hour		= (int)pVarParameters[1]; 
		int minute		= (int)pVarParameters[2]; 
		int second		= (int)pVarParameters[3]; 
		int isDST		= (int)pVarParameters[4];

		long lConvTime	=	To_Time(date, hour, minute, second, isDST);
		pVarReturnValue->SetValue(&lConvTime, RUL_INT);//WS-fixed return type 25jun07
		return true;	
	}
	break;

	case BUILTIN_Date_To_Time:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		long date = (int)pVarParameters[0];// WS - 9apr07 - 2005 checkin 

		long lConvTime =Date_To_Time (date);
		pVarReturnValue->SetValue(&lConvTime, RUL_INT);//WS-fixed return type 25jun07
		return true;	
	}
	break;

	case BUILTIN_To_Date:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(1);/* stevev-added check-25jun07*/
		RETURN_AT_NOT_NUMERIC(2);/* stevev-added check-25jun07*/
		int Year		= (int)pVarParameters[0]; 
		int month		= (int)pVarParameters[1]; 
		int DayOfMonth	= (int)pVarParameters[2]; 

		long lConvDate =To_Date(Year, month, DayOfMonth);
		pVarReturnValue->SetValue(&lConvDate, RUL_INT);
		return true;	
	}
	break;

	case BUILTIN_Time_To_Date:
	{
		RETURN_AT_NOT_NUMERIC(0);/* stevev-added check-25jun07*/
		long time_t1 = (int)pVarParameters[0];// WS - 9apr07 - 2005 checkin	
		
		long lConvDate =Time_To_Date(time_t1);
		pVarReturnValue->SetValue(&lConvDate, RUL_INT);// WS-fixed return type 25jun07
		return true;	
	}
	break;
/*=========================== date/time functions - 16jul14 =============================================*/

	case BUILTIN_From_DATE_AND_TIME_VALUE:
	{
		RETURN_AT_NOT_NUMERIC(0);
		RETURN_AT_NOT_NUMERIC(1);
		long lValue=(int)pVarParameters[0];
		unsigned long ulValue=(unsigned int)pVarParameters[1];
		long lReturnValue = From_DATE_AND_TIME_VALUE(lValue, ulValue);
		pVarReturnValue->SetValue(&lReturnValue, RUL_INT);
		return true;	
	}
	break;
	case BUILTIN_From_TIME_VALUE:
	{
		RETURN_AT_NOT_NUMERIC(0);
		unsigned long ulValue=(unsigned int)pVarParameters[0];
		long lReturnValue = From_TIME_VALUE(ulValue);
		pVarReturnValue->SetValue(&lReturnValue, RUL_INT);
		return true;	
	}
	break;

	case BUILTIN_DATE_to_days:
	{
		RETURN_AT_NOT_NUMERIC(0);
		RETURN_AT_NOT_NUMERIC(1);
		long lValue0=(int)pVarParameters[0];
		long lValue1=(int)pVarParameters[1];
		long lReturnValue = DATE_to_days(lValue0, lValue1);
		pVarReturnValue->SetValue(&lReturnValue, RUL_INT);
		return true;	
	}
	break;
	case BUILTIN_days_to_DATE:
	{
		RETURN_AT_NOT_NUMERIC(0);
		RETURN_AT_NOT_NUMERIC(1);
		long lValue0=(int)pVarParameters[0];
		long lValue1=(int)pVarParameters[1];
		long lReturnValue = days_to_DATE(lValue0, lValue1);
		pVarReturnValue->SetValue(&lReturnValue, RUL_INT);
		return true;	
	}
	break;

	case BUILTIN_seconds_to_TIME_VALUE:
	{
		RETURN_AT_NOT_NUMERIC(0);
		double seconds = (double)pVarParameters[0];	
		
		unsigned long time_value = seconds_to_TIME_VALUE(seconds);
		pVarReturnValue->SetValue(&time_value, RUL_UINT);
		return true;	
	}
	break;
	case BUILTIN_TIME_VALUE_to_seconds:
	{
		RETURN_AT_NOT_NUMERIC(0);
		unsigned long time_value = (unsigned int)pVarParameters[0];	
		
		double seconds = TIME_VALUE_to_seconds(time_value);
		pVarReturnValue->SetValue(&seconds, RUL_DOUBLE);
		return true;	
	}
	break;

	case BUILTIN_TIME_VALUE_to_Hour:
	{
		RETURN_AT_NOT_NUMERIC(0);
		unsigned long time_value = (unsigned int)pVarParameters[0];	
		
		int hour = TIME_VALUE_to_Hour(time_value);
		pVarReturnValue->SetValue(&hour, RUL_INT);
		return true;	
	}
	break;
	case BUILTIN_TIME_VALUE_to_Minute:
	{
		RETURN_AT_NOT_NUMERIC(0);
		unsigned long time_value = (unsigned int)pVarParameters[0];	
		
		int min = TIME_VALUE_to_Minute(time_value);
		pVarReturnValue->SetValue(&min, RUL_INT);
		return true;	
	}
	break;
	case BUILTIN_TIME_VALUE_to_Second:
	{
		RETURN_AT_NOT_NUMERIC(0);
		unsigned long time_value = (unsigned int)pVarParameters[0];	
		
		int second = TIME_VALUE_to_Second(time_value);
		pVarReturnValue->SetValue(&second, RUL_INT);
		return true;	
	}
	break;

	case BUILTIN_DATE_AND_TIME_VALUE_to_string:
	{
		wchar_t* format = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;

		//Get the String value 
		pVarParameters[1].GetStringValue(&format, RUL_DD_STRING);		
		//For Second string strip off the Language Code. 
		GetLanguageCode(format, szLangCode, &bLanCodePrese);
		
		RETURN_AT_NOT_NUMERIC(2);
		long date = (int)pVarParameters[2];
		RETURN_AT_NOT_NUMERIC(3);
		unsigned long time_value = (unsigned int)pVarParameters[3];
		
		
		wchar_t output_str[MAX_DD_STRING];
		int iSize = 0;
		iSize = DATE_AND_TIME_VALUE_to_string(output_str, format, date, time_value);

		//Update the output_str to user
		if ( ! SetStringParam(pFuncExp, pVarParameters, 0, output_str) )
		{
			return false;
		}
		pVarReturnValue->SetValue(&iSize, RUL_INT);

		//Delete all the Memory Allocated;
		if(format)
		{
			delete[] format;
			format= NULL;
		}
		return true;	
	}
	break;

	case BUILTIN_DATE_to_string:
	{
		wchar_t* format = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		wchar_t date_str[MAX_DD_STRING];
		int iSize = 0;

		RETURN_AT_NOT_NUMERIC(2);
		long date = (int)pVarParameters[2];
		
		//Get the String value 
		pVarParameters[1].GetStringValue(&format, RUL_DD_STRING);		
		
		//For Second string strip off the Language Code. Ideally SPeaking For first also we need to strip off and then Campare and then APend it banck
		//Remove the Language code from the first argument to the second argument
		GetLanguageCode(format, szLangCode, &bLanCodePrese);

		iSize = DATE_to_string(date_str, format, date);

		//Update the time_value_str to user
		if ( ! SetStringParam(pFuncExp, pVarParameters, 0, date_str) )
		{
			return false;
		}
		pVarReturnValue->SetValue(&iSize, RUL_INT);

		//Delete all the Memory Allocated;
		if(format)
		{
			delete[] format;
			format= NULL;
		}
		return true;	
	}
	break;

	case BUILTIN_TIME_VALUE_to_string:
	{
		wchar_t* format = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;

		//Get the String value 
		pVarParameters[1].GetStringValue(&format, RUL_DD_STRING);		
		//For Second string strip off the Language Code. 
		GetLanguageCode(format, szLangCode, &bLanCodePrese);
		
		RETURN_AT_NOT_NUMERIC(2);
		unsigned long time_value = (unsigned int)pVarParameters[2];
		
		
		wchar_t time_value_str[MAX_DD_STRING];
		int iSize = 0;
		iSize = TIME_VALUE_to_string(time_value_str, format, time_value);

		//Update the time_value_str to user
		if ( ! SetStringParam(pFuncExp, pVarParameters, 0, time_value_str) )
		{
			return false;
		}
		pVarReturnValue->SetValue(&iSize, RUL_INT);

		//Delete all the Memory Allocated;
		if(format)
		{
			delete[] format;
			format= NULL;
		}
		return true;	
	}
	break;

	case BUILTIN_timet_to_string:
	{
		wchar_t* format = NULL;		
		wchar_t szLangCode[5] = {0};
		bool bLanCodePrese = false;
		wchar_t time_t_str[MAX_DD_STRING];
		int iSize = 0;

		RETURN_AT_NOT_NUMERIC(2);
		long time_t = (int)pVarParameters[2];
		
		//Get the String value 
		pVarParameters[1].GetStringValue(&format, RUL_DD_STRING);		
		
		//For Second string strip off the Language Code. Ideally SPeaking For first also we need to strip off and then Campare and then APend it banck
		//Remove the Language code from the first argument to the second argument
		GetLanguageCode(format, szLangCode, &bLanCodePrese);

		iSize = timet_to_string(time_t_str, format, time_t);

		//Update the time_value_str to user
		if ( ! SetStringParam(pFuncExp, pVarParameters, 0, time_t_str) )
		{
			return false;
		}
		pVarReturnValue->SetValue(&iSize, RUL_INT);

		//Delete all the Memory Allocated;
		if(format)
		{
			delete[] format;
			format= NULL;
		}
		return true;	
	}
	break;

	case BUILTIN_timet_to_TIME_VALUE:
	{
		RETURN_AT_NOT_NUMERIC(0);
		long timet_value = (int)pVarParameters[0];	
		
		unsigned long time_value = timet_to_TIME_VALUE(timet_value);
		pVarReturnValue->SetValue(&time_value, RUL_UINT);
		return true;	
	}
	break;
	case BUILTIN_To_TIME_VALUE:
	{
		RETURN_AT_NOT_NUMERIC(0);
		RETURN_AT_NOT_NUMERIC(1);
		RETURN_AT_NOT_NUMERIC(2);
		int hours = (int)pVarParameters[0];	
		int minutes = (int)pVarParameters[1];	
		int seconds = (int)pVarParameters[2];	
		
		unsigned long time_value = To_TIME_VALUE(hours, minutes, seconds);
		pVarReturnValue->SetValue(&time_value, RUL_UINT);
		return true;	
	}
	break;
/*=============================================end date/time 16jul14 ===========================*/

	//stevev 29jan08 for literal strings in the methods
	case BUILTIN_literal_string:
	{	
		tchar *pchString = NULL;	
		long lItemId=0;	//WS:EPM 24may07	
LOGIT(CERR_LOG, "BUILTIN_literal_string");
		if (pVarParameters[0].isNumeric())
		{
			lItemId=(int)pVarParameters[0];
		}
		//LOGIT(CERR_LOG, "before literal_string");

		int iReturnValue = literal_string(lItemId, &pchString);
		//LOGIT(CERR_LOG, "after literal_string");
		if(pchString)
		{
			//LOGIT(CERR_LOG, "before SetValue");
			pVarReturnValue->SetValue(pchString, RUL_DD_STRING);
			//LOGIT(CERR_LOG, "after SetValue");
			delete[]    pchString;
			pchString = NULL;
		}

		LOGIT(CERR_LOG, "BUILTIN_literal_string done");
		return true;
	}
	break;
/**************************** Begin Block Transfer Builtins (eDDL) ********************/
	case BUILTIN_openTransferPort:
	{
		RETURN_AT_NOT_NUMERIC(0);// port number is only parameter	
		int iportNumber=(int)pVarParameters[0];
		
		int iReturnValue = openPort(iportNumber);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	case BUILTIN_closeTransferPort:
	{
		RETURN_AT_NOT_NUMERIC(0);// port number is only parameter	
		int iportNumber=(int)pVarParameters[0];
		
		int iReturnValue = closePort(iportNumber);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	case BUILTIN_abortTransferPort:
	{
		RETURN_AT_NOT_NUMERIC(0);// port number is only parameter	
		int iportNumber=(int)pVarParameters[0];
		
		int iReturnValue = abortPort(iportNumber);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	////////////////////////////////////////////////////////////////////////////////////
	case BUILTIN_writeItem2Port:
	{
		RETURN_AT_NOT_NUMERIC(0);// port number is first parameter	
		int iportNumber=(int)pVarParameters[0];
		RETURN_AT_NOT_NUMERIC(1);// item number is last parameter	
		unsigned iItemNumber=(unsigned)pVarParameters[1];
		
		int iReturnValue = write2Port(iportNumber,iItemNumber);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	case BUILTIN_readItemfromPort:
	{
		RETURN_AT_NOT_NUMERIC(0);// port number is first parameter	
		int iportNumber=(int)pVarParameters[0];
		RETURN_AT_NOT_NUMERIC(1);// item number is last parameter	
		unsigned iItemNumber=(unsigned)pVarParameters[1];
		
		int iReturnValue = readFromPort(iportNumber,iItemNumber);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;
	case BUILTIN_getTransferStatus:
	{
		RETURN_AT_NOT_NUMERIC(0);// port number is first parameter	
		int iportNumber=(int)pVarParameters[0];
		RETURN_AT_NOT_NUMERIC(1);// item number is last parameter	
		int iDirection=(int)pVarParameters[1];
		
		long pLongItemIds[10];
		int iNumberOfItemIds = 0;

		if (pVarParameters[2].GetVarType() == RUL_SAFEARRAY)
		{
			GetLongArray(pVarParameters[2],pLongItemIds,iNumberOfItemIds);
		}
		else
		{
			iNumberOfItemIds = 0;
		}
		
		int iReturnValue = getTransferStatus(iportNumber,iDirection,pLongItemIds,iNumberOfItemIds);

		pVarReturnValue->SetValue(&iReturnValue, RUL_INT);
		return true;
	}
	break;

/****************************  End Block Transfer Builtins (eDDL) *********************/
/****************************  Debug builtins (eddl) from emerson 16jul14 *************/


	case BUILTIN__ERROR:
	{
		wchar_t pchInputString[MAX_DD_STRING]=L"ERROR:";
		int pSize = MAX_DD_STRING;
		if ( ! GetStringParam(&(pchInputString[6]), pSize, pVarParameters, 0) )
		{
			//return false;
		}

		LOGIT(CERR_LOG, pchInputString);

		return true;
	}
	break;
	case BUILTIN__WARNING:
	{
		wchar_t pchInputString[MAX_DD_STRING]=L"WARNING:";
		int pSize = MAX_DD_STRING;
		if ( ! GetStringParam(&(pchInputString[8]), pSize, pVarParameters, 0) )
		{
			//return false;
		}

		LOGIT(CERR_LOG, pchInputString);

		return true;
	}
	break;
	case BUILTIN__TRACE:
	{
		wchar_t pchInputString[MAX_DD_STRING]=L"TRACE:";
		int pSize = MAX_DD_STRING;
		if ( ! GetStringParam(&(pchInputString[6]), pSize, pVarParameters, 0) )
		{
			//return false;
		}

		LOGIT(CLOG_LOG, pchInputString);

		return true;
	}
	break;
		
	default:
		break;

//Added By Anil June 17 2005 --Ends here
/****************************End of DD_STRING  Builtins (eDDL) ********************/
}


/* End of code */

	return false;
}


//Added By Walter Sigtermans --starts here
//This is the utility Function to take care of the Language Code while doing string operations
// for unicode strings
void CHart_Builtins::GetLanguageCode(wchar_t* szString, wchar_t* szLanguageCode, bool* bLangCodePresent)
{
	if( szString )
	{
		*bLangCodePresent =false;
		if( (szString[0] == _T('|')) &&  (szString[3] == _T('|')) )
		{
			*bLangCodePresent = true;
			int count,itemp = wcslen(szString);// WS - 9apr07 - 2005 checkin
			for(count = 4; count <itemp ;count++)// WS - 9apr07 - 2005 checkin
			{
				if(count<8)
				{
					szLanguageCode[count-4] = szString[count-4];
				}
				szString[count-4] = szString[count];
			}
			szString[count-4] = _T('\0');
			szLanguageCode[4] = _T('\0');
		}
	}
	return;

}


//Added By Anil June 17 2005 --starts here
//This is the utility Function to take care of the Language Code while doing string operations
void CHart_Builtins::GetLanguageCode(char* szString, char* szLanguageCode, bool* bLangCodePresent)
{
	if( szString )
	{
		*bLangCodePresent =false;
		if( (szString[0] == '|') &&  (szString[3] == '|') )
		{
			*bLangCodePresent = true;
			int count,itemp = strlen(szString);// WS - 9apr07 - 2005 checkin
			for(count = 4; count <itemp ;count++)// WS - 9apr07 - 2005 checkin
			{
				if(count<8)
					szLanguageCode[count-4] = szString[count-4];
				szString[count-4] = szString[count];
			}
			szString[count-4] = '\0';
			szLanguageCode[4] = '\0';
		}
	}
	return;

}

//Added By Anil June 17 2005 --Ends here

// added by stevev 30may07 - start
// This is a utility to get all this widely used coding in one place
//   returns success=true, failure = false
bool CHart_Builtins::GetStringParam(wchar_t* retString, int retStringLen/*in wide chars*/,
									                  INTER_VARIANT *pParamArray, int paramNumber)
{
	int strLength = MAX_DD_STRING;
	int maxLength = 0;
	char* pC = (char*) retString;
	bool ret = true;
/* 11 feb08 - most of this processing is now done in the INTER_VARIENT */
	wchar_t *pRet = NULL;
	
	pParamArray[paramNumber].GetStringValue(&pRet);
	if ( pRet != NULL )// allocated in GetString
	{
		retStringLen  = min(wcslen(pRet),MAX_DD_STRING) + 1;
#ifdef MODIFIED_BY_SS
		std::wstring strTemp(pRet);
		strTemp.copy(retString, retStringLen);
		//wcsncpy(retString,pRet,retStringLen);
#endif
		delete[] pRet;
	}
/* instead of the following...
	if ((pParamArray[paramNumber].GetVarType() == RUL_CHARPTR)  || 
		(pParamArray[paramNumber].GetVarType() == RUL_UNSIGNED_CHAR)  )
	{
		maxLength = min((retStringLen*sizeof(tchar)),MAX_DD_STRING);
		strLength = strlen((char*)(pParamArray[paramNumber]));
		if (strLength > maxLength)
		{
			strncpy(pC,pParamArray[paramNumber],maxLength-1);
			retString[maxLength-1] = '\0';
		}
		else
			strcpy(pC,pParamArray[paramNumber]);
	}
	else
	if ((pParamArray[paramNumber].GetVarType() == RUL_WIDECHARPTR)  || 
		(pParamArray[paramNumber].GetVarType() == RUL_DD_STRING)  )
	{
		maxLength = min(retStringLen,MAX_DD_STRING);
		strLength = _tstrlen(pParamArray[paramNumber]);
		if (strLength > maxLength)
		{
			_tstrncpy(retString,pParamArray[paramNumber],maxLength-1);
			retString[maxLength-1] = _T('\0');
		}
		else
			_tstrcpy(retString,pParamArray[paramNumber]);
	}
	else 
	if (pParamArray[paramNumber].GetVarType() == RUL_SAFEARRAY)
	{
		GetWCharArray(pParamArray[paramNumber], retString, strLength);
	}
	else
	{
		return false;
	}
***** end replaced code *****/
	return true;
}
// ByteString is NOT required to be null terminated and is L bytes long
// CharString is used to null terminator or L length, which ever comes first

bool CHart_Builtins::GetCharStringParam(char* retString, int retStringLen,INTER_VARIANT *pParamArray, int paramNumber)
{
	int strLength = MAX_DD_STRING;
	int maxLength = 0;
	char* pC = (char*) retString;
	bool ret = true;

	wchar_t *pRet = NULL;

	pParamArray[paramNumber].GetStringValue(&pRet);
	if ( pRet != NULL )// allocated in GetString
	{
#ifdef MODIFIED_BY_SS
		//wcstombs(retString,pRet,retStringLen);
		 // Convert wide string to std::wstring
		std::wstring wideStr(pRet);

		// Convert std::wstring to std::string
		std::string multibyteStr(wideStr.begin(), wideStr.end());

		// Copy to retString with length checking
#ifdef MODIFIED_BY_SS
		//size_t copyLen = std::min((retStringLen - 1), static_cast<int>(multibyteStr.length()));
		size_t copyLen = custom_min((retStringLen - 1), static_cast<int>(multibyteStr.length()));
#endif

		std::copy_n(multibyteStr.begin(), copyLen, retString);
		retString[copyLen] = '\0'; // Null-terminate the string

		delete[] pRet; // Clean up allocated memory


#endif 
		delete[] pRet;

	}
	return true;
}

//
//
//
bool CHart_Builtins::SetCharStringParam(FunctionExpression* pFuncExp, INTER_VARIANT *pParamArray, int paramNumber, char* paramString, int L)
{
	bool ret = true;
	INTER_VARIANT * pParam =  &(pParamArray[paramNumber]);
	if ((pParam->GetVarType() == RUL_CHARPTR)	  ||	/* needs L */
		(pParam->GetVarType() == RUL_WIDECHARPTR) ||	/* needs L */ 
		(pParam->GetVarType() == RUL_DD_STRING)	  ||	/* needs L */ 
	    (pParam->GetVarType() == RUL_BYTE_STRING) ||
		(pParam->GetVarType() == RUL_SAFEARRAY)    )
	{
		pParamArray[paramNumber] = paramString;//will convert destination type as required
		ret = OutputParameterValue( pFuncExp, paramNumber, pParamArray[paramNumber],strlen(paramString) );// added WS:EPM 17jul07
	}
	else 
	{// string to numeric unsupported
		ret = false;
	}
	return ret;
}

//
//
//
bool CHart_Builtins::SetByteStringParam(FunctionExpression* pFuncExp, INTER_VARIANT *pParamArray, 
																int paramNumber, _BYTE_STRING& bsS)
{// must handle nulls in the byte string!!
	int strLength = MAX_DD_STRING;
	int maxLength = 0;
//	char* pC = (char*) paramString;
	bool ret = true;
	INTER_VARIANT * pParam =  &(pParamArray[paramNumber]);

	if ((pParam->GetVarType() == RUL_CHARPTR)	  ||	/* needs L */
		(pParam->GetVarType() == RUL_WIDECHARPTR) ||	/* needs L */ 
		(pParam->GetVarType() == RUL_DD_STRING)	  ||	/* needs L */ 
	    (pParam->GetVarType() == RUL_BYTE_STRING) ||
		(pParam->GetVarType() == RUL_SAFEARRAY)    )
	{
		pParamArray[paramNumber] = bsS;//will convert destination type as required
		ret = OutputParameterValue( pFuncExp, paramNumber, pParamArray[paramNumber],bsS.bsLen );// added WS:EPM 17jul07
	}
	else 
	{// string to numeric unsupported
		ret = false;
	}
	return ret;
}

bool CHart_Builtins::GetByteStringParam(uchar* retString,  int retStringLen, INTER_VARIANT *pParamArray, int paramNumber)
{
	cerr<<"ERROR: *** CHart_Builtins::GetByteStringParam is Not implemented!!"<<endl;
	return false;
}

bool CHart_Builtins::SetStringParam(FunctionExpression* pFuncExp, INTER_VARIANT *pParamArray, 
									int paramNumber,    wchar_t* paramString)
{
	int strLength = wcslen(paramString);
//	char* pC = (char*) paramString;
	bool ret = true;

	if ((pParamArray[paramNumber].GetVarType() == RUL_CHARPTR)	   ||
		(pParamArray[paramNumber].GetVarType() == RUL_WIDECHARPTR) || 
		(pParamArray[paramNumber].GetVarType() == RUL_DD_STRING)   || 
	    (pParamArray[paramNumber].GetVarType() == RUL_BYTE_STRING) || 
		(pParamArray[paramNumber].GetVarType() == RUL_SAFEARRAY)    )
	{
		pParamArray[paramNumber] = paramString; //will convert to pParamArray type
		ret = OutputParameterValue( pFuncExp, paramNumber, pParamArray[paramNumber],strLength);
																			// added WS:EPM 17jul07
	}
	else
	{
		ret = false;// we don't do string to numeric here
	}
	return ret;
}

/* added by WS:EPM 17jul07 checkin */
// stevev 11feb08 - added length to deal with byte-strings
bool CHart_Builtins::OutputParameterValue( FunctionExpression* pFuncExp, 
										  int nParamNumber, INTER_VARIANT &NewVarValue, int L)
{
	bool bRetVal = true;
	char szLocalVarName[MAX_PATH]={0};

	CComplexDDExpression *pExp = (CComplexDDExpression*)pFuncExp->GetExpParameter(nParamNumber);
	if (!pExp)
	{
		bRetVal = false;
	}

	if( bRetVal )
	{
		CToken* pToken = pExp->GetToken();
		if( pToken )
		{
			const char* szVarName = pToken->GetLexeme();
			if (szVarName)
			{
#ifdef MODIFIED_BY_SS
				std::string strTemp(szVarName);
				std::copy(strTemp.begin(), strTemp.end(), szLocalVarName);
				//strcpy( szLocalVarName, szVarName );
#endif
			}
			else
			{
				bRetVal = false;
			}
		}
		else
		{
			bRetVal = false;
		}
	}

	if( bRetVal )
	{
		char szLang[5] = {0};
		bool bLangPresent=false;
		//		Remove the Language code , if it was appended <a tokenizer bug>
		GetLanguageCode( szLocalVarName, szLang, &bLangPresent );
		//		Update the DD local var szLocaVarName with the value lselection
		m_pInterpreter->SetVariableValue( szLocalVarName, NewVarValue );
	}

	return bRetVal;
}
/* added by WS:EPM 17jul07 - ends */
// added by stevev 30may07 - ends