
//#include "stdafx.h"
//#include <windows.h>

//jad vs2017 arm64 debug
//#include "ddbGeneral.h"

//#include <ALPHAOPS.H>
#include "pch.h"
#include <limits.h>
#include <float.h>
#include "ddbdevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"
#include "MEE.h"
#include "messageUI.h"

/***** Start Abort builtins *****/
int CHart_Builtins::abort()
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	bool	bSetAbortFlag=false;
	
	structUIData.userInterfaceDataType=TEXT_MESSAGE;
/* Walt 01may07 - buffer doesn't always exist.***
	strcpy(structUIData.textMessage.pchTextMessage,"Method aborted");
 ****/
/* stevev 01may07 - modified Walt's Solution **/
	if(structUIData.textMessage.pchTextMessage)
	{
	  delete[] structUIData.textMessage.pchTextMessage; 
	  structUIData.textMessage.pchTextMessage = NULL;
	}
	structUIData.textMessage.pchTextMessage = new tchar[_tstrlen(M_METHOD_ABORTED) + 1];
	structUIData.textMessage.iTextMessageLength = _tstrlen(M_METHOD_ABORTED);
#ifdef MODIFIED_BY_SS
	//_tstrcpy(structUIData.textMessage.pchTextMessage ,M_METHOD_ABORTED);
	std::copy(M_METHOD_ABORTED, M_METHOD_ABORTED + wcslen(M_METHOD_ABORTED),
		structUIData.textMessage.pchTextMessage);
#endif 
	structUIData.bUserAcknowledge=true;
/*Vibhor 030304: Start of Code*/
	structUIData.bEnableAbortOnly = false; // just defensive
/*Vibhor 030304: End of Code*/

/*Vibhor 040304: Start of Code*/
	structUIData.uDelayTime = 0;// just defensive
/*Vibhor 040304: End of Code*/

/*Vibhor 040304: Comment: Added the second condition below*/

	if(m_pMeth->GetMethodAbortStatus() && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
	structUIData.bDisplayDynamic = false; //Added by Prashant 20FEB2004
	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	else
	{
		m_pMeth->abort ();
	}

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
	}
	
	if(structUIData.textMessage.pchTextMessage != NULL)
	{
		delete[] structUIData.textMessage.pchTextMessage; 
		structUIData.textMessage.pchTextMessage = NULL;
	}

	return (METHOD_ABORTED);
	
}

int CHart_Builtins::process_abort()
{
	m_pMeth->process_abort ();
	return (METHOD_ABORTED);
}

int CHart_Builtins::_add_abort_method(long lMethodId)
{
	int nRetVal = BI_SUCCESS;

	nRetVal = m_pMeth->_add_abort_method (lMethodId);

	return nRetVal;
}	

int CHart_Builtins::_remove_abort_method(long lMethodId)
{
	int nRetVal = BI_SUCCESS;
	
	nRetVal = m_pMeth->_remove_abort_method (lMethodId);

	return nRetVal;
}

int CHart_Builtins::remove_all_abort()
{
	int nRetVal = BI_SUCCESS;

	nRetVal = m_pMeth->remove_all_abort();

	return (nRetVal);
}

/*Arun 190505 Start of code*/

int CHart_Builtins::push_abort_method(long lMethodId)
{
	int nRetVal = BI_SUCCESS;

	nRetVal = m_pMeth->_push_abort_method(lMethodId);

	return (nRetVal);
}

int CHart_Builtins::pop_abort_method()
{	
	int nRetVal = BI_SUCCESS;

	nRetVal = m_pMeth->_pop_abort_method();

	return (nRetVal);
}

/*End of code*/
/* End Abort builtins */
