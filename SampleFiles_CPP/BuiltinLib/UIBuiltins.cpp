//#include "stdafx.h"
//#include <windows.h>
#include "pch.h"
#include "ddbGeneral.h"
//#include <ALPHAOPS.H>
#include <limits.h>
#include <float.h>
#include "ddbdevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"
#include "MEE.h"

#include <ctype.h>

#define MAX_LEN_ALLOC		MAX_DD_STRING  /* stevev 11feb08 = was 1024 */
template<typename T>
const T& custom_min(const T& a, const T& b) {
	return (a < b) ? a : b;
}


/* also in ddbvarnumeric.cpp **/
#ifdef linux
#define ULL_ZERO    0x0000000000000000ull
#define ULL_ONE     0x0000000000000001ull
#define _LL_SIGN    0x8000000000000000ull
#define ULL_MAX     __UINTMAX_MAX__

//jad debug
//#ifdef APPLE
//#define _LL_MAX     9223372036854775807ll
//#else
#define _LL_MAX     __INT64_MAX__
//#endif

#define _LL_MIN     (-9223372036854775807ll - 1)
#define __L_MAX     __INT32_MAX__
#define _UL_MAX     __UINT32_MAX__

#else
/* in limits.h */
#define ULL_ZERO    0x0000000000000000ui64
#define ULL_ONE     0x0000000000000001ui64
#define _LL_SIGN    0x8000000000000000ui64
#define ULL_MAX     _UI64_MAX   /* ffffffffffffffff */
#define _LL_MAX     _I64_MAX    /* 7fffffffffffffff */
#define _LL_MIN     _I64_MIN
#define __L_MAX     _I32_MAX
#define _UL_MAX     _UI32_MAX
#endif






 // replace Sleep
void  systemSleep(DWORD mSecs);	
#define LOOP_SLEEP  systemSleep(600)

extern unsigned long ddbGetTickCount(void);// force this to be compiled outside the devobject

long long UI_id = 0x100; // 27jun14 a unique ID for a given UI instance

/***************** static helper functions *****************************************************/
static
void add_textMsg( ACTION_UI_DATA& aud, tchar* pMsg)
{
//LOGIT(CLOG_LOG,"*** add_textMsg: start");
	int len = _tstrlen(pMsg);

	if(aud.textMessage.pchTextMessage != NULL)
	{
LOGIT(CLOG_LOG,"add_textMsg: deleting entry text message\n");
		delete[] aud.textMessage.pchTextMessage; 
		         aud.textMessage.pchTextMessage = NULL; 
	}
	aud.textMessage.iTextMessageLength = 0;

//jad, removed in pc version...
	aud.userInterfaceDataType = TEXT_MESSAGE;
	if(len > 0)
	{
		aud.textMessage.pchTextMessage     = new tchar[len + 1];
		aud.textMessage.iTextMessageLength = len;
#ifdef MODIFIED_BY_SS
	//	_tstrncpy(aud.textMessage.pchTextMessage ,pMsg, len);
	//	aud.textMessage.pchTextMessage[len] = 0;
		std::wstring pMsgStr = pMsg; // Convert pMsg to std::wstring
		size_t copyLen = min(pMsgStr.length(), static_cast<size_t>(len));
		
		// Copy characters
		std::copy_n(pMsgStr.begin(), copyLen, aud.textMessage.pchTextMessage);
		aud.textMessage.pchTextMessage[copyLen] = L'\0'; // Ensur
#endif
		
	}
	else
	{
		aud.textMessage.pchTextMessage     = NULL;
		aud.textMessage.iTextMessageLength = 0;
	}
}
static
void add_optionList( ACTION_UI_DATA& aud, tchar* pListString)
{
	tchar* strList = NULL;
	int c, i, inLen = _tstrlen(pListString);

	if (pListString == NULL)  return;

	if (inLen > 0)
	{
		strList = new tchar[inLen + 1];
#ifdef MODIFIED_BY_SS
		//_tstrcpy(strList,pListString);
		std::wstring pMsgStr = pListString; // Convert pMsg to std::wstring
		size_t copyLen = custom_min(pMsgStr.length(), static_cast<size_t>(inLen));

		// Copy characters
		std::copy_n(pMsgStr.begin(), copyLen, strList);
		strList[copyLen] = L'\0'; // Ensur

#endif
		for ( c = 0, i = 0; i < inLen; i++ ) 	
		{
			if ((strList[i] == _T(';'))   || 
				(strList[i] != _T(';')   &&  strList[i+1] == 0) )
			{
				c++;
			}
		}
// CW fix begin 30nov11, email of 11/24/11
        aud.userInterfaceDataType = COMBO;
// CW fix end
		aud.ComboBox.pchComboElementText = new tchar[wcslen(strList) + 1];
#ifdef MODIFIED_BY_SS
		//_tstrcpy(aud.ComboBox.pchComboElementText,strList);
		wcsncpy_s(aud.ComboBox.pchComboElementText, wcslen(strList), strList, _TRUNCATE);
#endif
		aud.ComboBox.iNumberOfComboElements = c;
	
		delete [] strList;
	
	}
	else
	{	//When no string is passed to combo box, make it blank by allocating 0ne byte
		aud.ComboBox.pchComboElementText = new tchar[1];
		aud.ComboBox.pchComboElementText = 0;
		aud.ComboBox.iNumberOfComboElements = 0;
	}
}

int clearVarientArray(CValueVarient	*pVarients, unsigned asize = 50 )
{
	for (unsigned f = 0; f < asize; f++)
	{
		pVarients[f].clear();
	}
	return asize;
}
/***********************************************************************************************/

/*********************************Vibhor 270204:Start of Comment*********************************
Signature of Read() has been modified to pass a bool flag, telling whether the reqd value 
needs to be scaled or not.
All the calls in this file REQUIRE scaling so "true" is passed as the third argument
*********************************Vibhor 270204:End of Comment***********************************/

int CHart_Builtins::DELAY
					(
						int iTimeInSeconds
						, tchar *prompt
					)
{
	
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar	out_buf[MAX_LEN_ALLOC]={0};
	tchar	out_prompt[MAX_LEN_ALLOC]={0};
	bool	bSetAbortFlag=false;
/*<START>14FEB04  Added by ANOOP for dynamic variables %0 */	
	CValueVarient 	pDynVarVals[50];	//16APR2004Added by ANOOP 
	clearVarientArray(pDynVarVals);
	bool	bDynVarValsChanged=false;
	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>14FEB04  Added by ANOOP for dynamic variables %0 */	


	int rc=m_pDevice->dictionary->get_string_translation(prompt,out_prompt,MAX_LEN_ALLOC);
	int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,
						   out_prompt,NULL,0,pDynVarVals,bDynVarValsChanged);

	if(structUIData.textMessage.pchTextMessage != NULL)
	{
		delete[] structUIData.textMessage.pchTextMessage; 
		structUIData.textMessage.pchTextMessage = NULL; 
	}
	structUIData.userInterfaceDataType=TEXT_MESSAGE;
	if(_tstrlen(out_buf)>0)
	{
		structUIData.textMessage.pchTextMessage = new tchar[_tstrlen(out_buf) + 1];
		structUIData.textMessage.iTextMessageLength = _tstrlen(out_buf);
		
#ifdef MODIFIED_BY_SS
		////_tstrcpy(structUIData.textMessage.pchTextMessage ,out_buf);
		wcsncpy_s(structUIData.textMessage.pchTextMessage, wcslen(out_buf), out_buf, _TRUNCATE);
#endif
	}
	else
		structUIData.textMessage.iTextMessageLength = 0;
	
	structUIData.bUserAcknowledge=false;
/*Vibhor 030304: Start of Code*/
	if(!(this->m_AbortInProgress))
		structUIData.bEnableAbortOnly = true;
	else
	{
		structUIData.bEnableAbortOnly = false;
	}
		// Need the abort button
/*Vibhor 030304: End of Code*/
/*Vibhor 040304: Start of Code*/
	structUIData.uDelayTime = iTimeInSeconds * 1000 ;
/*Vibhor 040304: End of Code*/

/*Vibhor 040304: Comment: Added the second condition below*/

	if(m_pMeth->GetMethodAbortStatus() && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
		
	// stevev 02Jun14 - now set in bltin_format_string
	// structUIData.bDisplayDynamic = false;  //Added by ANOOP 20FEB2004
	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	else
/*Vibhor 040304: Start of Code*/
/*	else // Now this is handled in MethSupport.cpp
	{
		Sleep((unsigned int)iTimeInSeconds * 1000);
	}
*/
/*Vibhor 040304: End of Code*/

	/*<START>Added by ANOOP 20APR2004 Delay Builtin shud support dynamic vars	*/
	if(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay && !bSetAbortFlag)
	{		
		unsigned long dwStartTime = ddbGetTickCount();//GetTickCount();
		unsigned long dwEndTime   = dwStartTime;
#ifdef MODIFIED_BY_SS
		//_tstrcpy(out_buf, mt_String);
		wcsncpy_s(out_buf, wcslen(mt_String), mt_String, _TRUNCATE);
#endif
		
		while(  ( dwEndTime - dwStartTime ) < (structUIData.uDelayTime ) &&
			     true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay )
		{			
			bDynVarValsChanged=false;
			out_buf[0] = '\0';
			int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,out_prompt,
											NULL,0,pDynVarVals,bDynVarValsChanged);		
			if( true == bDynVarValsChanged )	
			  {
				add_textMsg(structUIData, out_buf);			
			  }
//=============================================================================================
				// has to be after the format so bEnableDynamicDisplay is preserved 4 while test
			if(false == 
			   m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
			  {
				bSetAbortFlag=true;// must be after the bltin_format_string is called or they will be reset
				break;
		}
			systemSleep(100); // use 100 due to timing - will give 100 mS time jitter
			dwEndTime = ddbGetTickCount();
		}//wend till disabled
	}
	/*<END>Added by ANOOP 20APR2004 Delay Builtin shud support dynamic vars	*/	
	
	if(NULL != structUIData.textMessage.pchTextMessage )	
	{
		delete[] structUIData.textMessage.pchTextMessage;
		structUIData.textMessage.pchTextMessage = NULL; 
	}

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}	
} 

int CHart_Builtins::delay
			(
				int iTimeInSeconds
				, tchar *pchDisplayString	/* DD_STRING */
				, long *lItemId
				, int iNumberOfItemIds
			)
{
	
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar	out_buf[MAX_LEN_ALLOC]={0};
	bool	bSetAbortFlag=false;
/*<START>14FEB04  Added by ANOOP for dynamic variables %0 */
	CValueVarient 	pDynVarVals[50];	//16APR2004Added by ANOOP 
	clearVarientArray(pDynVarVals);
	bool	bDynVarValsChanged=false;
	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>14FEB04  Added by ANOOP for dynamic variables %0 */
	// stevev 26dec07 - the prompt string may be more than one language (not a dictStr)
	//					as such, one translation will alwauys be shorter than the whole.
	int rc = m_pDevice->dictionary->get_string_translation(
								pchDisplayString,pchDisplayString,_tstrlen(pchDisplayString) +1);
	int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC, UPDATE_NORMAL,pchDisplayString,
					lItemId,iNumberOfItemIds,pDynVarVals,bDynVarValsChanged);

	add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
	structUIData.userInterfaceDataType = TEXT_MESSAGE;

	structUIData.bUserAcknowledge=false;
/*Vibhor 030304: Start of Code*/
	if(!(this->m_AbortInProgress))
	{
	structUIData.bEnableAbortOnly = true; // Need the abort button
	}
	else
	{
		structUIData.bEnableAbortOnly = false;
	}
/*Vibhor 030304: End of Code*/
/*Vibhor 040304: Start of Code*/
	structUIData.uDelayTime = iTimeInSeconds * 1000 ;
/*Vibhor 040304: End of Code*/

/*Vibhor 040304: Comment: Added the second condition below*/

	if(m_pMeth->GetMethodAbortStatus() && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
	
	// stevev 02Jun14 - now set in bltin_format_string
	// structUIData.bDisplayDynamic = false;	//Added by ANOOP 20FEB2004

	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}

	/*<START>Added by ANOOP 20APR2004 Delay Builtin shud support dynamic vars	*/
	if(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay)
	{
		
		unsigned long dwStartTime = ddbGetTickCount();//**** This will wrap every 47 days that the computer runs!!!
		unsigned long dwEndTime = dwStartTime;

		while( structUIData.uDelayTime > ( dwEndTime - dwStartTime )  &&
			   true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay  )
			{
			bDynVarValsChanged=false;
			out_buf[0] = '\0';
			int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC, UPDATE_NORMAL,
								pchDisplayString,lItemId,iNumberOfItemIds,pDynVarVals,
								bDynVarValsChanged);
	
			if(true == bDynVarValsChanged )	
			{	
				add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code			
			}
			if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
			{
				bSetAbortFlag=true;
				break;
		}
			systemSleep(100);// we need to give some time to the winMain to update its display
			dwEndTime = ddbGetTickCount();
		}//wend till disabled
	}
	/*<END>Added by ANOOP 20APR2004 Delay Builtin shud support dynamic vars	*/


	if(NULL != structUIData.textMessage.pchTextMessage )	
	{
		delete[] structUIData.textMessage.pchTextMessage;
	    structUIData.textMessage.pchTextMessage = NULL; 
	}

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}
}

int CHart_Builtins::DELAY_TIME
			(
				int iTimeInSeconds
			)
{

//	m_pDevice->m_pMethSupportInterface->SleepWithMessageLoop((unsigned int)iTimeInSeconds * 1000);
	systemSleep((unsigned int)iTimeInSeconds * 1000);
		
	//Sleep((unsigned int)iTimeInSeconds * 1000);

	return (BI_SUCCESS);
}

int CHart_Builtins::put_message(tchar *message,long *glob_var_ids,int iNumberOfItemIds)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar out_buf[MAX_LEN_ALLOC]={0};
	unsigned int iTimeInSeconds=PUT_MESSAGE_SLEEP_TIME;  //preset Time for the message to be displayed
	bool	bSetAbortFlag=false;
/*<START>14FEB04  Added by ANOOP for dynamic variables %0 */
	CValueVarient 	pDynVarVals[50];	//16APR2004Added by ANOOP 
	clearVarientArray(pDynVarVals);
	bool	bDynVarValsChanged=false;
	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>14FEB04  Added by ANOOP for dynamic variables %0 */
	
	int rc=m_pDevice->dictionary->get_string_translation(message,message,_tstrlen(message) +1);
	int retval= bltin_format_string (out_buf, MAX_LEN_ALLOC,UPDATE_NORMAL,
					message,glob_var_ids,iNumberOfItemIds,NULL,bDynVarValsChanged);

	add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
	structUIData.userInterfaceDataType = TEXT_MESSAGE;

	structUIData.bUserAcknowledge=false;
	structUIData.bEnableAbortOnly = false; // just defensive
	structUIData.uDelayTime = 0;// just defensive

	if(m_pMeth->GetMethodAbortStatus() && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
	structUIData.bDisplayDynamic = false;	//Added by ANOOP 20FEB2004
	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	//Sleep(1000);
	//m_pDevice->m_pMethSupportInterface->SleepWithMessageLoop(1000);		
	systemSleep(1000);

	if(structUIData.textMessage.pchTextMessage)
	{
		delete[] structUIData.textMessage.pchTextMessage;
	    structUIData.textMessage.pchTextMessage = NULL; 
	}

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}
}

int  CHart_Builtins::PUT_MESSAGE(tchar *message)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar out_buf[MAX_LEN_ALLOC]={0};
	unsigned int iTimeInSeconds=PUT_MESSAGE_SLEEP_TIME;  //preset Time for the message to be displayed
	bool	bSetAbortFlag=false;
/*<START>14FEB04  Added by ANOOP for dynamic variables %0 */
	CValueVarient 	pDynVarVals[50];	//16APR2004Added by ANOOP 
	clearVarientArray(pDynVarVals);
	bool	bDynVarValsChanged=false;
	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>14FEB04  Added by ANOOP for dynamic variables %0 */	

	int rc=m_pDevice->dictionary->get_string_translation(message,message,_tstrlen(message) +1);
	int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC/*was::>strlen(message)*/,
		up_DONOT_UPDATE,message,NULL,0,NULL,bDynVarValsChanged);

	add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
	structUIData.userInterfaceDataType = TEXT_MESSAGE;

	structUIData.bUserAcknowledge=false;
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
	structUIData.bDisplayDynamic = false;// stevev always...	//Added by ANOOP 20FEB2004
	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	//Sleep(1000);
	//m_pDevice->m_pMethSupportInterface->SleepWithMessageLoop(1000);		
	systemSleep(1000);
	
	if(structUIData.textMessage.pchTextMessage)
	{
		delete[] structUIData.textMessage.pchTextMessage;
	    structUIData.textMessage.pchTextMessage = NULL; 
	}
	
	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}

}

int CHart_Builtins::ACKNOWLEDGE(tchar *message)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar	out_buf[MAX_DD_STRING]={0};
	bool	bSetAbortFlag=false;
/*<START>14FEB04  Added by ANOOP for dynamic variables %0 */
	CValueVarient 	pDynVarVals[50];
	clearVarientArray(pDynVarVals);
	bool			bDynaVarValChanged=false;
	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>14FEB04  Added by ANOOP for dynamic variables %0 */

	int rc=m_pDevice->dictionary->get_string_translation(message,message,_tstrlen(message) +1);
	int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,
								message,NULL,0,pDynVarVals,bDynaVarValChanged);
			
	add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
	structUIData.userInterfaceDataType = TEXT_MESSAGE;

	//Anil September 23 2005 has added this as return value was not checked,
	if( retval == BI_ERROR)
	{
		//TODO what to return as 
		//     this Function is not handling any return value other than METHOD_ABORTED
	}

	structUIData.bUserAcknowledge=true; 
	/*Vibhor 030304: Start of Code*/
	structUIData.bEnableAbortOnly = false; // just defensive
	/*Vibhor 030304: End of Code*/
	/*Vibhor 040304: Start of Code*/
	structUIData.uDelayTime = 0;// just defensive
	/*Vibhor 040304: End of Code*/
/*Vibhor 040304: Comment: Added the second condition below*/
//                                                 always true
	if(m_pMeth->GetMethodAbortStatus() && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
	// stevev 02Jun14 - now set in bltin_format_string
	// structUIData.bDisplayDynamic = false;	//Added by ANOOP 20FEB2004
	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	else
	{
	//_tstrcpy(out_buf,mt_String);
#ifdef MODIFIED_BY_SS
		//_tstrcpy(out_buf, mt_String);
		wcsncpy_s(out_buf, wcslen(mt_String), mt_String, _TRUNCATE);
#endif
		while(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay )
	{	
			bDynaVarValChanged=false;
		out_buf[0] = '\0';
			int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,
									message,NULL,0,pDynVarVals,bDynaVarValChanged);

		if(true == bDynaVarValChanged )	
		{
			add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code		
			}
//=============================================================================================
				// has to be after the format so bEnableDynamicDisplay is preserved 4 while test
			if(false == 
			   m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
		{
			bSetAbortFlag=true;
				break;
		}
			LOOP_SLEEP;	
		}//wend till disabled
	}

	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
    structUIData.bDisplayDynamic = false;  //Added by ANOOP 200204
	if(structUIData.textMessage.pchTextMessage)
	{
		delete[] structUIData.textMessage.pchTextMessage;  
	    structUIData.textMessage.pchTextMessage = NULL;
	}

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}
}



int CHart_Builtins::acknowledge(tchar *message,long * glob_var_ids,int iNumberOfItemIds)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar disp_msg[MAX_LEN_ALLOC]={0};
	bool	bSetAbortFlag=false;
/*<START>Added by ANOOP for dynamic vars %0 */
	CValueVarient 	pDynVarVals[50];
	clearVarientArray(pDynVarVals);
	bool			bRefreshDynamicVars=true;
	bool			bDynaVarValChanged=false;
	
	// stevev 02Jun14 - now set in bltin_format_string
	// m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
	/*<END>Added by ANOOP for dynamic vars %0 */

	int rc=m_pDevice->dictionary->get_string_translation(message,message,_tstrlen(message)+1);
	int retval=bltin_format_string(disp_msg,MAX_LEN_ALLOC, UPDATE_NORMAL,message,
					glob_var_ids,iNumberOfItemIds,pDynVarVals,bDynaVarValChanged);

	add_textMsg(structUIData,disp_msg);	// stevev 26dec07 - common code
	structUIData.userInterfaceDataType = TEXT_MESSAGE;

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
	// stevev 02Jun14 - now set in bltin_format_string
	// structUIData.bDisplayDynamic = false;  //Added by ANOOP 200204
	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	else
	{
	
#ifdef MODIFIED_BY_SS
	//_tstrcpy(disp_msg,mt_String);
	wcsncpy_s(disp_msg, wcslen(mt_String), mt_String, _TRUNCATE);
#endif
		while( true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay )
	{
			bDynaVarValChanged=false;
		disp_msg[0] = '\0';
			int retval= bltin_format_string(disp_msg,MAX_LEN_ALLOC,UPDATE_NORMAL,message,
						glob_var_ids,iNumberOfItemIds,pDynVarVals,bDynaVarValChanged);
		if(true == bDynaVarValChanged )	
		{
			add_textMsg(structUIData,disp_msg);	// stevev 26dec07 - common code		
	    }
		if(false == 
				m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
		{
			bSetAbortFlag=true;
				break;
		}
			LOOP_SLEEP;	
		}//wend till disabled
	}
	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
    structUIData.bDisplayDynamic = false;  //Added by ANOOP 200204

	if(structUIData.textMessage.pchTextMessage)
	{
		delete[] structUIData.textMessage.pchTextMessage;
	    structUIData.textMessage.pchTextMessage = NULL; 
	}

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}
}

int CHart_Builtins::_get_dev_var_value
		(
			tchar *pchDisplayString
			, long *lItemId1
			, int	iNumberOfItemIds
			, long lItemId
		)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA         structUIData;
	tchar                  out_buf[MAX_LEN_ALLOC]={0};
	hCitemBase           * pIB = NULL;
	CValueVarient          vvReturnedDataItem;
	EnumTriad_t            localData;
	int                    valLen;
	wstring                finalstr;
	bool  memAlloc_Enum=false, memAlloc_String=false;
	int                    nCntr=0;
	hCVar                 *pVar=NULL;
//	hCenumList eList;
	bool	bSetAbortFlag=false;
/*<START>Added by ANOOP for dynamci vars %0  */
	CValueVarient 	       pDynVarVals[50];
	clearVarientArray(pDynVarVals);
	bool			       bDynaVarValChanged=false;
	UIntList_t			   uintList;		//Vibhor 200605: added
	UIntList_t			   uValidIndxList; //Vibhor 200605: added

	// memsets are not needed due to ctors in structures.

	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>Added by ANOOP for dynamci vars %0  */
	int rc=m_pDevice->dictionary->get_string_translation(pchDisplayString,pchDisplayString,
																_tstrlen(pchDisplayString)+1);


	int retval=bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,pchDisplayString,
				   lItemId1,iNumberOfItemIds,pDynVarVals,bDynaVarValChanged);



LOGIT(CLOG_LOG,"** out_buf = %s",out_buf);


//	Go to dev obj and get to know the type of the variable as well as the min and max value.	
	
	if (m_pDevice->getItemBySymNumber(lItemId, &pIB) == SUCCESS
		&& pIB->IsVariable()                                     )
	{
		pVar=(hCVar *)pIB;
		// stevev 08feb11 - get all formatting information at once
		int rgt, max ;
		wstring edt,dsp,scn;
		pVar->getEditFormatInfo(max,rgt,&edt);
				
		//Anil Done changes for the Pre Edit aActions from the method 21 December 2005
		vector<itemID_t> actionList;
		varAttrType_t actionType = varAttrPreEditAct;

		//Get all the pre edit actions for this variable
		pVar->getActionList(actionType,actionList);
				
		//Go through each of Pre edit actions and Execute it						
		int iReturnVal = m_pMeth->ExecuteActionsInMethod(actionList);
		if( 0 != iReturnVal)
		{
			return BI_ERROR;
		}					
		structUIData.DDitemId    = lItemId;					
		structUIData.pVar4ItemID = pVar;

		// stevev 22may07 fix the index variable editting in method dialogs
		int locType = pVar->VariableType() ;
		// treat non-item arrays as integers
		if (locType == vT_Index)
		{
			hCitemArray*  pIndexedItem = NULL;
			hCitemBase*   pIB = NULL;
			if(	((hCindex*)pVar)->pIndexed->getArrayPointer(pIB) != SUCCESS || 
				pIB == NULL ||
								(pIB->getIType() != iT_ItemArray && 
								 pIB->getIType() != iT_Array     && 
								 pIB->getIType() != iT_List)       )
			{
				locType = vT_Unsigned;// treat it like an int
			}// else just keep going
			else // is one of three indexed types
			if (pIB->getIType() == iT_ItemArray)
			{// index must display descriptions
				// leave locType as vT_Index
				pIndexedItem = (hCitemArray*)pIB;// a no-op
			}
			else 
			{//iT_Array,iT_List - index shouldn't try to display descriptions
				locType = vT_Unsigned;// treat it like an int
				pIndexedItem = NULL;
			}
		}
		// end stevev 22may07 

		switch(locType)
		{
			case  vT_Enumerated:	
			{
				structUIData.userInterfaceDataType = COMBO;
				structUIData.ComboBox.comboBoxType = COMBO_BOX_TYPE_SINGLE_SELECT;

                hCEnum* pEn = NULL;
				pEn = (hCEnum*)pIB;

				hCenumList eList(pEn->devHndl());
				uValidIndxList.clear();
				if ( pEn->procureList(eList) != SUCCESS )
				{
					return BI_ERROR;
				}
				else
				{
					CValueVarient cvIndexValue;
					//cvIndexValue = pEn->getDispValue();
					
					if( Read(lItemId,cvIndexValue,true) != BI_SUCCESS )
					{	
						return BI_ERROR;
					}
					finalstr = mt_String;

					structUIData.EditBox.nSize =
					valLen = eList.maxDescLen();// stevev 08feb11     pEn->VariableSize();

					unsigned long nCurrentValue = (unsigned long)cvIndexValue;
					nCntr = 0; // yes, it duplicates the original clear

					structUIData.ComboBox.nCurrentIndex = 0xffffffff;// -1 into unsigned 
					for (hCenumList::iterator iT = eList.begin(); iT != eList.end(); ++iT)
					{//iT isa ptr 2 hCenumDesc
						localData = *iT;					// a EnumTriad_t
						unsigned long nPosition =  localData.val; 

						if ( finalstr.empty() )
						{
							finalstr = localData.descS;
						}								
						else
						{
							finalstr += _T(";");
							finalstr += localData.descS;
						}
						//stevev - these are not required to be contiguous
						uValidIndxList.push_back((UINT32)nPosition);
						
						if(nCurrentValue == nPosition)
						{
							structUIData.ComboBox.nCurrentIndex = nCntr;
						}
						nCntr++;
					}// next enumerated value
					
					if (structUIData.ComboBox.nCurrentIndex == 0xffffffff)
					{// we never found a match <current value is not a valid one>
						wchar_t tmp[32];
						memset(tmp,0,sizeof(wchar_t) *32);

//jad debug
LOGIT(CLOG_LOG,"** NoEnumeration = %d",nCurrentValue);


						swprintf(tmp,32,L"0x%02x NoEnumeration",nCurrentValue);
						finalstr += _T(";");
						finalstr += tmp;

						uValidIndxList.push_back(nCurrentValue);
						
						structUIData.ComboBox.nCurrentIndex = nCntr;
						nCntr++;
					}

				}// endelse we got the list of triads					 
				
				if(!finalstr.empty())
				{
					memAlloc_Enum = true;
					structUIData.ComboBox.pchComboElementText = 
														new wchar_t[finalstr.length() + 1];
					

#ifdef MODIFIED_BY_SS
					/*wcsncpy( structUIData.ComboBox.pchComboElementText, finalstr.c_str(),
																			finalstr.length());*/
					wcsncpy_s(structUIData.ComboBox.pchComboElementText, finalstr.length(), finalstr.c_str(), _TRUNCATE);
#endif
					//stevev 23oct09 - copy the trailing NULL too
					structUIData.ComboBox.pchComboElementText[finalstr.length()] = 0;
					structUIData.ComboBox.iNumberOfComboElements = nCntr;
					// done at match...structUIData.ComboBox.nCurrentIndex = nIndex;
				}
				break;
			}	
			case  vT_BitEnumerated:
			{
				structUIData.userInterfaceDataType = COMBO;
				structUIData.ComboBox.comboBoxType = COMBO_BOX_TYPE_MULTI_SELECT;
				
				hCBitEnum* pEn = NULL;
				pEn = (hCBitEnum*)pIB;

				hCenumList eList(pEn->devHndl());
				uValidIndxList.clear();					
				if ( pEn->procureList(eList) != SUCCESS )
				{
					return BI_ERROR;
				}
				else
				{
					CValueVarient cvIndexValue;//WS:EPM 30apr07
					//cvIndexValue = pEn->getRawDispValue();//WS:EPM 30apr07
				
					if( Read(lItemId,cvIndexValue,true) != BI_SUCCESS )
					{	
						return BI_ERROR;
					}
					finalstr = mt_String;

					structUIData.EditBox.nSize = 
					valLen = eList.maxDescLen();// stevev 08feb11     pEn->VariableSize();
					
					unsigned long nCurrentValue = (unsigned long)cvIndexValue;
					nCntr    = 0;

					structUIData.ComboBox.nCurrentIndex = 0xffffffff;// -1 into unsigned 
					for (hCenumList::iterator iT = eList.begin(); iT < eList.end(); iT++)
					{//iT isa ptr 2 hCenumDesc
						localData = *iT;					// a EnumTriad_t
						unsigned long nPosition= localData.val; 

						//WS:EPM 30apr07 - start section
						if( nCntr >= (valLen*8) ) 
						{// stevev-21may07-from::  MAXIMUM_NUMBER_OF_BITS_IN_BITENUM )
							break;//if there is more than 32 bits here;  we have problems.
						}
						//WS:EPM 30apr07 end section
						//do we have a description
						wstring ss= localData.descS;
						if ( nCntr == 0 ) // stevev-21may07-from::   finalstr.empty() )
						{
							finalstr = ss;
						}								
						else
						{	
							finalstr += _T(";");
							finalstr += ss;
						}
						/* the current value probably has several bits set, just send it
						uValidIndxList.push_back((UINT32)nPosition);//stevev - these are not required to be contiguous

						if(nCurrentValue == nPosition)
						{
							structUIData.ComboBox.nCurrentIndex = nCntr;
						}
						*/

						structUIData.ComboBox.m_lBitValues[nCntr] = nPosition;
						//stevev 22may07..changed from::>  localData.val;//WS:EPM 30apr07
						nCntr++;
					}// next desc
					// if no match,
					structUIData.ComboBox.nCurrentIndex = nCurrentValue; //several bits set
				}
				if(!finalstr.empty())
				{
					memAlloc_Enum=true;
					structUIData.ComboBox.pchComboElementText = 
														new wchar_t[finalstr.length() + 1];
#ifdef MODIFIED_BY_SS
					//changed from wcsncpy
					wcsncpy_s(structUIData.ComboBox.pchComboElementText, finalstr.size(),finalstr.c_str(),
																		  _TRUNCATE);
#endif
					structUIData.ComboBox.iNumberOfComboElements=nCntr;
				}
				break;
			}
			/*
			case vT_Password:
			{
				structUIData.userInterfaceDataType= EDIT;	
				structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_PASSWORD;
				if( Read(lItemId,structUIData.EditBox.editBoxValue,true) == BI_SUCCESS )
				{	
					//if (structUIData.EditBox.editBoxValue.vSize > 0)
					//{
					//	memAlloc_String = true;
					//	structUIData.EditBox.pchDefaultValue = new char[ structUIData.EditBox.editBoxValue.vSize +1 ];
					//	strcpy(structUIData.EditBox.pchDefaultValue,
					//		  ((string)structUIData.EditBox.editBoxValue).c_str());
					//	structUIData.EditBox.iDefaultStringLength = nLen_str;
					//}
					//structUIData.EditBox.iMaxStringLength=pVar->VariableSize(); //WS:EPM 30apr07	
					
					structUIData.EditBox.nSize = pVar->VariableSize(); 
				}
				else
				{
					return BI_ERROR;
				}	
				break;				
			}
			*/
			case  vT_Password:
			case  vT_Ascii:					
			case  vT_PackedAscii:
			{
				/*<START>Commented by ANOOP 25MAR2004 No need of any conversions from packed ascii to ascii
				char pch_PackedAscii[1024]={0};
				int nLen_PackedAscii;
				<END>Commented by ANOOP 25MAR2004	*/  	
				//char pch_Ascii[MAX_LEN_ALLOC]={0};				
				
				structUIData.userInterfaceDataType = EDIT;	

				if ( locType == vT_Password )	
					structUIData.EditBox.editBoxType = EDIT_BOX_TYPE_PASSWORD;
				else
					structUIData.EditBox.editBoxType = EDIT_BOX_TYPE_STRING;

				if( Read(lItemId,structUIData.EditBox.editBoxValue,true) == BI_SUCCESS )
				{	/*			
					// we wouldn't be here if we weren't one of these...
					//if( pVar->VariableType() == vT_PackedAscii || pVar->VariableType() == vT_Ascii)
					//{
						if( vvReturnedDataItem.vType == vvReturnedDataItem.isWideString )
						{
							wcstombs( pch_Ascii, vvReturnedDataItem.sWideStringVal.c_str(), sizeof(pch_Ascii) );
						}
						else 
						if( vvReturnedDataItem.vType == vvReturnedDataItem.isString )
						{
							strcpy(pch_Ascii,vvReturnedDataItem.sStringVal.c_str()  );
						}
					//}
					
					if( strlen(pch_Ascii) > 0)
					{
						memAlloc_String=true;
						structUIData.EditBox.pchDefaultValue =new char[strlen(pch_Ascii) +1];
						strcpy(structUIData.EditBox.pchDefaultValue,pch_Ascii); 
						structUIData.EditBox.iDefaultStringLength=strlen(pch_Ascii);
					}
					else
					{
						structUIData.EditBox.pchDefaultValue = NULL; 
						structUIData.EditBox.iDefaultStringLength=0;
					}	
					structUIData.EditBox.iMaxStringLength=pVar->VariableSize();
					*/
					structUIData.EditBox.nSize = max+1;// stevev 08feb11 
				}
				else
				{
					return BI_ERROR;
				}
								
				break;
			}
			case vT_TimeValue:
			{
				wstring	sTmp;

				structUIData.userInterfaceDataType= TIME;	
				structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_TIME;

				if( Read(lItemId,structUIData.EditBox.editBoxValue,true) == BI_SUCCESS )
				{
				/*
				hCTimeValue	*pTime=(hCTimeValue	*)pVar;	
				sTmp = pTime->getDispValueString(); 
				int	nTmplen = sTmp.length();
				if( nTmplen >0 )
				{
					structUIData.datetime.pchHartDate=new tchar[nTmplen +1];
					_tstrcpy(structUIData.datetime.pchHartDate,sTmp.c_str()); 
					structUIData.EditBox.iDefaultStringLength = nTmplen +1;
				}
				else
				{
					structUIData.datetime.pchHartDate =new tchar[8];//MAX_LEN_ALLOC];
					_tstrcpy(structUserInput.datetime.pchHartDate,mt_String); 
				}
				structUIData.EditBox.iMaxStringLength=MAX_LEN_ALLOC -1;
				*/		
					/* stevev 21jul09 - bogus assumption::>
					structUIData.EditBox.nSize = structUIData.EditBox.editBoxValue.vSize; 
					<<:: replace with the following ::>>*/
					structUIData.EditBox.nSize = max+1;
					
				}
				else
				{
					return BI_ERROR;
				}							
			}
			break;

			
			case  vT_Integer:
			case  vT_Unsigned:	
			case  vT_FloatgPt:				
			case  vT_Double:	
			//case  vT_BitString:
			//case  vT_VisibleString:
			{

				//jad debug
				LOGIT(CLOG_LOG, "CHart_Builtins::_get_dev_var_value");







				structUIData.userInterfaceDataType = EDIT;	
				structUIData.EditBox.nSize         = pVar->VariableSize(); 
				if( Read(lItemId,structUIData.EditBox.editBoxValue,true) == BI_SUCCESS )
				{
					if(
					structUIData.EditBox.editBoxValue.vType == CValueVarient::isIntConst ||
					structUIData.EditBox.editBoxValue.vType == CValueVarient::isVeryLong   )
					{
						structUIData.EditBox.editBoxType = EDIT_BOX_TYPE_INTEGER;
						//structUIData.EditBox.iValue=(__int64)vvReturnedDataItem;
						//varient will coerce ints into __int64
						//structUIData.EditBox.editBoxValue = vvReturnedDataItem;

						//wstring tmpStr;
				//((hCNumeric *) pVar)->ReadForEdit(structUIData.EditBox.strEdtFormat);
						structUIData.EditBox.strEdtFormat = edt;
						//structUIData.EditBox.strEdtFormat = tmpStr;

/*<START>Added by ANOOP to validata a list of ranges */ 

/* stevev 28 may 09 - we are going to use the variable's range checking to deal with this.

						hCRangeList retList;
						MinMaxVal tmpMinMaxVal;
						RangeList_t::iterator mmFnd;

						((hCinteger *)pVar)->getMinMaxList(retList);

						for (mmFnd = retList.begin(); mmFnd != retList.end(); mmFnd++)
						{
							tmpMinMaxVal.IntMinMaxVal.iMinval = mmFnd->second.minVal;
							tmpMinMaxVal.IntMinMaxVal.iMaxval = mmFnd->second.maxVal;
							structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
						}
//<END>Added by ANOOP to validate a list of ranges 
						end stevev 29 may09 */

/*<START> Commented by ANOOP to validating a list of ranges
						structUIData.EditBox.iMinValue=INT_MIN;
						structUIData.EditBox. iMaxValue=INT_MAX;
<END> Commented by ANOOP to validating a list of ranges	*/

					}	
					else if (
						structUIData.EditBox.editBoxValue.vType == CValueVarient::isFloatConst)
					{// could be an int w/ float format	
						structUIData.EditBox.editBoxType = EDIT_BOX_TYPE_FLOAT;
						//structUIData.EditBox.fValue=(float)vvReturnedDataItem;
						//structUIData.EditBox.editBoxValue = vvReturnedDataItem;
						//tstring tmpStr;
					//((hCFloat *) pVar)->ReadForEdit(structUIData.EditBox.strEdtFormat);
						structUIData.EditBox.strEdtFormat = edt;
						//structUIData.EditBox.strEdtFormat=tmpStr;



/* stevev 28 may 09 - we are going to use the variable's range checking to deal with this.
						hCRangeList retList;
						MinMaxVal tmpMinMaxVal;
						RangeList_t::iterator mmFnd;

						
						((hCFloat *)pVar)->getMinMaxList(retList);
						for (mmFnd = retList.begin(); mmFnd != retList.end(); mmFnd++)
						{ 
							tmpMinMaxVal.FloatMinMaxVal.fMinval   = mmFnd->second.minVal;
							tmpMinMaxVal.FloatMinMaxVal.fMaxval   = mmFnd->second.maxVal;
							structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
						}
						end stevev 29 may09 */
/*<START> Commented by ANOOP to validating a list of ranges
						structUIData.EditBox.fMinValue=FLT_MIN;
						structUIData.EditBox.fMaxValue=FLT_MAX;
<END> Commented by ANOOP to validating a list of ranges	*/
					}
					else if ( 
						structUIData.EditBox.editBoxValue.vType == CValueVarient::isString  )
					{
						structUIData.EditBox.editBoxType = EDIT_BOX_TYPE_STRING;
						structUIData.EditBox.editBoxValue=				// convert to wide
												(wstring)structUIData.EditBox.editBoxValue;

						//char pchStrval[MAX_LEN_ALLOC]={0};
								
						//strcpy(pchStrval,structUIData.EditBox.editBoxValue.sStringVal.c_str() );
						//if( strlen(pchStrval) > 0)
						//{
						//	memAlloc_String=true;
						//	structUIData.EditBox.pchDefaultValue =new char[strlen(pchStrval) +1];
						//	strcpy(structUIData.EditBox.pchDefaultValue,pchStrval); 
						//	structUIData.EditBox.iDefaultStringLength=strlen(pchStrval);
						//}
						//else
						//{
						//	strcpy(structUIData.EditBox.pchDefaultValue,""); 
						//	structUIData.EditBox.iDefaultStringLength=0;
						//	structUIData.EditBox.iMaxStringLength=pVar->VariableSize();
						//}
						//structUIData.EditBox.iMaxStringLength=pVar->VariableSize();

						//structUserInput.EditBox.pchDefaultValue =new char[MAX_LEN_ALLOC];
						//strcpy(structUserInput.EditBox.pchDefaultValue,""); 
						//structUserInput.EditBox.iDefaultStringLength=MAX_LEN_ALLOC -1;
					}
					else if ( 
						structUIData.EditBox.editBoxValue.vType == CValueVarient::isWideString)
					{
						structUIData.EditBox.editBoxType = EDIT_BOX_TYPE_STRING;
						// leave value as read above
					}
					// else all other types are discarded
				}
				else // Read() returned an error
				{
					return BI_ERROR;
				}
				break;					
			}  
			case vT_HartDate:
			{
				structUIData.userInterfaceDataType = HARTDATE;	
				structUIData.EditBox.editBoxType   = EDIT_BOX_TYPE_DATE;

				if( Read(lItemId,structUIData.EditBox.editBoxValue,true) == BI_SUCCESS )
				{
				/*
				hChartDate	*pDate=(hChartDate	*)pVar;	
				pDate->Read(sTmpdate); 
				int		nTmplen=sTmpdate.length();
				if( nTmplen >0 )
				{
					structUIData.datetime.pchHartDate=new tchar[nTmplen +1];
					_tstrcpy(structUIData.datetime.pchHartDate,sTmpdate.c_str()); 
				}
				else
				{
				//	_tstrcpy(structUIData.datetime.pchHartDate,_T(""));
				//	structUserInput.datetime.pchHartDate =new tchar[MAX_LEN_ALLOC];
					structUIData.datetime.pchHartDate =new tchar[MAX_LEN_ALLOC];
				//	_tstrcpy(structUserInput.datetime.pchHartDate,mt_String);
					_tstrcpy(structUIData.datetime.pchHartDate,mt_String);
				}
				*/	
					structUIData.EditBox.nSize = max+1;// stevev 08feb11
										// was structUIData.EditBox.editBoxValue.vSize; 
				}
				else
				{
					return BI_ERROR;
				}	

				//structUserInput.datetime.pchHartDate =new tchar[MAX_LEN_ALLOC];
				//_tstrcpy(structUserInput.datetime.pchHartDate,mt_String); 
//				structUserInput.datetime.pchHartDate=MAX_LEN_ALLOC -1;
				break;
			}
//PARFIX: 5544, the case for Index type variables was not there at all !!!
// stevev - 22may07 :: indexes may be item arrays, lists or value arrays
			case vT_Index:	// only for item arrays, see filter before switch
			{
				structUIData.userInterfaceDataType = COMBO;
				structUIData.ComboBox.comboBoxType = COMBO_BOX_TYPE_SINGLE_SELECT;

				hCindex * pIndx = NULL;
				pIndx = (hCindex*)pIB;

				hCitemArray* pIndxdArr     = NULL;
				hCgroupItemDescriptor *pGID= NULL;
				hCitemBase*  pIndexedItem  = NULL;// stevev - 22may07
			
				uValidIndxList.clear();
				hCitemBase* pIB = NULL;

				// stevev - 22may07-if(	pIndx->pIndexed->getArrayPointer(pIndxdArr) ==
				//											 SUCCESS && pIndxdArr != NULL)
				if(	pIndx->pIndexed->getArrayPointer(pIB) == SUCCESS && pIB != NULL)
				{
					pIndxdArr = (hCitemArray*)pIB;
					if ( pIndxdArr->getIType() == iT_ItemArray)//iT_Array,iT_List
					{
						pIndxdArr->getAllindexValues(uintList);
						if(uintList.size())
						{
							CValueVarient cvIndexValue;						
							if( Read(lItemId,cvIndexValue,true) != BI_SUCCESS )
							{	
								return BI_ERROR;
							}
							finalstr = mt_String;
							
							structUIData.EditBox.nSize = pIndx->VariableSize();

							unsigned int nCurrentValue = (unsigned int)cvIndexValue;
							nCntr = 0; // yes, it duplicates the original clear

							structUIData.ComboBox.nCurrentIndex = 0xffffffff;
							UIntList_t::iterator it;
							for(it = uintList.begin();  it != uintList.end();  ++it )
							{
								unsigned int uIndxVal = (*it);
								if(SUCCESS == pIndxdArr->getByIndex(uIndxVal,&pGID))
								{
									wstring tmpstr = (wstring)(pGID->getDesc());
									if (tmpstr.empty())								
									{//empty
										wchar_t tmp[64];
										swprintf(tmp,64,L"%d",uIndxVal);
										tmpstr = tmp;
									}
									if ( finalstr.empty() )
									{
										finalstr += tmpstr;
									}								
									else
									{
										finalstr += _T(";");
										finalstr += tmpstr;
									}
									uValidIndxList.push_back(uIndxVal);
									/* * *  WS:EPM 10aug07 * * */
									if( uIndxVal == nCurrentValue )
									{
										structUIData.ComboBox.nCurrentIndex = nCntr;
									}
									nCntr++;
									/* * end -- WS:EPM 10aug07 * * */
								}// else getbyindex failed - skip it silently
								RAZE(pGID);
								// stevev via HOMZ 21feb07 - pGID alloc'd in getbyindex
							}// next index
							if (structUIData.ComboBox.nCurrentIndex == 0xffffffff)
							{// we never found a match <current value is not a valid one>
								wchar_t tmp[32] = {0};
								swprintf(tmp,32, L"0x%02x Nonexistent",nCurrentValue);

LOGIT(CLOG_LOG,"** Nonexistent = %d",nCurrentValue);

								finalstr += _T(";");
								finalstr += tmp;

								uValidIndxList.push_back(nCurrentValue);

								structUIData.ComboBox.nCurrentIndex = nCntr;
								nCntr++;
							}
						}
					}
					else // iT_Array,iT_List  stevev - 22may07
					{// treat it like an integer - should have been done before we got here
						LOGIT(CERR_LOG,
							  "ERROR: internal: have an array/list index at itemarray.\n");
						break;
					}
				}
				else
				{
					return BI_ERROR;
				}
				
				if(!finalstr.empty())
				{
					memAlloc_Enum=true;
					structUIData.ComboBox.pchComboElementText = 
															new wchar_t[finalstr.length() + 1];
#ifdef MODIFIED_BY_SS
					/*wcsncpy(structUIData.ComboBox.pchComboElementText,finalstr.c_str(),
																			 finalstr.size() );*/
					wcsncpy_s(structUIData.ComboBox.pchComboElementText, finalstr.size(), finalstr.c_str(),
						_TRUNCATE);
#endif
					structUIData.ComboBox.pchComboElementText[finalstr.length()] = 0;
					structUIData.ComboBox.iNumberOfComboElements=nCntr;
				}
				break;		
			}

			default:
				LOGIT(CERR_LOG,"Unhandled data type in _get_dev_var_value()\n");
				break;
		}// end of switch case 


		UI_DATA_TYPE h = structUIData.userInterfaceDataType;
		add_textMsg(structUIData,out_buf);	    // stevev 26dec07 - common code
		structUIData.userInterfaceDataType = h;;// sjv 19feb08 - restore held value

		structUIData.bUserAcknowledge=true; 
		/*Vibhor 030304: Start of Code*/
		structUIData.bEnableAbortOnly = false; // just defensive
		/*Vibhor 030304: End of Code*/
		/*Vibhor 040304: Start of Code*/
		structUIData.uDelayTime = 0;// just defensive
		/*Vibhor 040304: End of Code*/
/*Vibhor 040304: Comment: Added the second condition below*/

		if(m_pMeth->GetMethodAbortStatus())// always true,you just set it...&& (structUIData.bEnableAbortOnly == false))
		{
			structUIData.bMethodAbortedSignalToUI =true;
		}
		else
		{
			structUIData.bMethodAbortedSignalToUI =false;// defensive only
		}
//=============================================================================================
		if(false == 
			   m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
		{
			bSetAbortFlag=true;
		}
		else
		{	//Anil Done changes for the post Edit aActions from the method 21 December 2005
			vector<itemID_t> actionList;
			varAttrType_t actionType = varAttrPostEditAct;

			//Get all the pre post actions for this variable
			pVar->getActionList(actionType,actionList);
#ifdef MODIFIED_BY_SS			
			//_tstrcpy(out_buf, mt_String);
			std::copy(mt_String, mt_String + wcslen(mt_String),
				out_buf);
#endif
			while(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay)
			{
				bDynaVarValChanged=false;	
				out_buf[0] = '\0';
				int retval=  bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,
												pchDisplayString,lItemId1, iNumberOfItemIds, 
												pDynVarVals, bDynaVarValChanged);

				if(true == bDynaVarValChanged )	
				{
					add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
				}
//=============================================================================================
				// has to be after the format so bEnableDynamicDisplay is preserved 4 while test
				if(false == 
			   m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
				{
					bSetAbortFlag=true;
					break;
				}
				LOOP_SLEEP;
			}//wend till disabled

	        structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204
			
			// stevev - moved POST edit actions till the edit interface is completely back
			//Go through each of post edit actions and Execute it							
			int iReturnVal = m_pMeth->ExecuteActionsInMethod(actionList);
			if( 0 != iReturnVal)
			{
				return BI_ERROR;
			}				

// Set user entered display value
			switch (structUserInput.userInterfaceDataType)
			{
				case COMBO:
				{
					int nSelect = structUserInput.nComboSelection;// stevev 22may07 -1;	
					switch( pVar->VariableType() )
					{
						case  vT_Enumerated:	
						{
							nSelect -= 1;// index is zero based here stevev 22may07
							hCEnum* pEn = NULL;
							pEn = (hCEnum*)pVar;
							if ( pEn != NULL )
							{
						//	hCenumList eList(pEn->devHndl());
						//	if ( pEn->procureList(eList) != SUCCESS )
						//	{
						//		return BI_ERROR;
						//	}
						//	else
						//	{
						//		if( ((hCEnum*)pVar)->procureList(eList)== SUCCESS && eList.size() > 0 ) 
						//		{
						//				vvReturnedDataItem = (int) eList[nSelect].val;
								// convert selection to enumValue
								vvReturnedDataItem = uValidIndxList.at(nSelect);
						//		}	
								pEn->setDispValue(vvReturnedDataItem);	
							}	
							break;
						}
						case vT_BitEnumerated:
						{
					// stevev 22may07 - bit-enum returns value of all bits set (not an index)
							vvReturnedDataItem = (unsigned int) nSelect;
							((hCBitEnum*)pVar)->setDisplayValue(vvReturnedDataItem);
							/* stevev 22may07 -  was::>   
							hCEnum* pEn = NULL;
							pEn = (hCBitEnum*)pIB;
							hCenumList eList(pEn->devHndl());
							if ( pEn->procureList(eList) != SUCCESS )
							{
								return BI_ERROR;
							}
							else
							{
								if( ((hCBitEnum*)pVar)->procureList(eList)== SUCCESS && eList.size() > 0 ) 
								{
										vvReturnedDataItem = (int) eList[nSelect].val;
								}
								pVar->setDispValue(vvReturnedDataItem);	
							}
							** end was **/
							break;
						}

//PARFIX: 5544, the case for Index type variables was not there at all !!!
						case vT_Index:
						{
							nSelect -= 1;
							hCindex * pIndx = NULL;
							pIndx = (hCindex*)pVar;
							if(pIndx != NULL)
							{
								vvReturnedDataItem = uValidIndxList.at(nSelect);// unmap
								pIndx->setDispValue(vvReturnedDataItem);
							}
						}
						break;

					}//end switch varType
				
					break;	
				}
				case EDIT:
				{
					switch(structUserInput.EditBox.editBoxType) 
					{
						case EDIT_BOX_TYPE_INTEGER:
						{
							//vvReturnedDataItem = (INT64)structUserInput.EditBox.iValue;
							//vvReturnedDataItem = structUserInput.EditBox.editBoxValue;
							// stevev 27feb06
							if ( ! ((hCinteger*)pVar)->
									isInRange(structUserInput.EditBox.editBoxValue) )
							{
						//TODO: now what
							}							

							pVar->setDispValue(structUserInput.EditBox.editBoxValue);	
							break;
						}
						case EDIT_BOX_TYPE_FLOAT:
						{
							//vvReturnedDataItem = (float)structUserInput.EditBox.fValue;
							//vvReturnedDataItem = structUserInput.EditBox.editBoxValue;
							/*m_pDevice->WriteImd (lItemId,&vvReturnedDataItem);*/
							/*if (Write(lItemId,vvReturnedDataItem) != SUCCESS)
							{
								return BI_ERROR;
							}*/
							pVar->setDispValue(structUserInput.EditBox.editBoxValue);	
							break;
						}
						case EDIT_BOX_TYPE_STRING:
						case EDIT_BOX_TYPE_PASSWORD:
						{
							//vvReturnedDataItem = 
							//				(char *)structUserInput.EditBox.pchDefaultValue;
							pVar->setDispValue(structUserInput.EditBox.editBoxValue);	
							break;
						}
						case UNKNOWN_EDIT_BOX_TYPE:
						{
							break;
						}
					}
					break;
				
				}
				case HARTDATE:
				{
					hChartDate	*pDate = (hChartDate*)pVar;	
					//vvReturnedDataItem = (tchar *)structUserInput.datetime.pchHartDate;
					//pDate->setDispValue(vvReturnedDataItem);
					// gcc requires a local
					wstring localStr(structUserInput.EditBox.editBoxValue);
				   pDate->setDisplayValueString(localStr);
					//if(structUserInput.datetime.pchHartDate)
					//{
					//	delete[] structUserInput.datetime.pchHartDate;
					//	if(structUIData.datetime.pchHartDate)
					//	{
					//		delete[]  structUIData.datetime.pchHartDate;
					//	}
					//}
					break;
				}
				case TIME:
				{
					hCTimeValue	*pTime=(hCTimeValue	*)pVar;	
					//wstring tmpStr;
					//tmpStr = (tchar *)structUserInput.datetime.pchHartDate;
					//was vvReturnedDataItem = (tchar *)structUserInput.datetime.pchHartDate;
					//pTime->setDispValue(vvReturnedDataItem);
					// gcc requires a local
					wstring localStr(structUserInput.EditBox.editBoxValue);
				   pTime->setDisplayValueString(localStr);
					/*
					if(structUserInput.datetime.pchHartDate)
					{
						delete[] structUserInput.datetime.pchHartDate;
						structUserInput.datetime.pchHartDate = NULL;
						if(structUIData.datetime.pchHartDate)
						{
							delete[]  structUIData.datetime.pchHartDate;
							structUIData.datetime.pchHartDate = NULL;
						}
					}*/
				}
				break;
			}// end switch	userInterfaceDataType
		}//end else methodDisplay was success
	}//endif getbysymbolnumber success   AND  IsVariable.......else - falls thru to do nothing
	if (memAlloc_Enum)
	{
		delete[] structUIData.ComboBox.pchComboElementText;
	}

	if(structUIData.textMessage.pchTextMessage)	
	{
		delete[] structUIData.textMessage.pchTextMessage;	
	    structUIData.textMessage.pchTextMessage = NULL;
	}
	
	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}
}// end _get_dev_var_value



int CHart_Builtins::_get_local_var_value				
		(
			tchar *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
			, char *pchVariableName
		)
{

	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar out_buf[MAX_LEN_ALLOC]={0};
	char strVar_name[50]={0};
	char   *curr_ptr       =NULL;
	hCitemBase* pIB = NULL;
	CValueVarient vvReturnedDataItem;
	INTER_VARIANT varVal;
	VARIANT_TYPE  varVariantType; // we have to remember this across display execution
	bool	bSetAbortFlag=false;
/*<START>Added by ANOOP for dynamci vars %0  */
	CValueVarient 	       pDynVarVals[50];	
	clearVarientArray(pDynVarVals);
	bool	bDynaVarValChanged=false;

	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>Added by ANOOP for dynamci vars %0  */	

	captureStartTime();
	
	int rc=m_pDevice->dictionary->get_string_translation(pchDisplayString,pchDisplayString,
																_tstrlen(pchDisplayString) +1);
	DEBUGLOG(CLOG_LOG,L"_get_dev_var_value:: prompt>  '%s'\n",pchDisplayString);
	int retval=bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,pchDisplayString,
					plItemIds,iNumberOfItemIds,pDynVarVals,bDynaVarValChanged);
	// WS:EPM 17jul07 checkin - start
	// remove trailing whitespace from the char* (could be in InterpretedVisitor.cpp in visitFunctionExpression(...)) SF:EPM
	string sTrim(pchVariableName);
	int nLastPosition = sTrim.length() - 1;			// get the last position of the array
	// stevev 26oct10 - deal with empty strings and the like...
	while( sTrim.length() > 0 && nLastPosition >= 0 && isspace(sTrim[nLastPosition]) )
	{
		sTrim.erase(nLastPosition);					// remove the whitespace
#ifdef MODIFIED_BY_SS
		//strcpy( pchVariableName, sTrim.c_str() );	// copy the new string to pchVariableName
		std::copy(sTrim.begin(), sTrim.end(),pchVariableName);	// copy the new string to pchVariableName
#endif		
		nLastPosition = sTrim.length() - 1;			// get the new last position of the array
	}
	// WS:EPM 17jul07 checkin - end
//	Get the value of the variable as well as the min and max value.	
	m_pInterpreter->GetVariableValue(pchVariableName,varVal);
	
	add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
	//add_textMsg sets the structUIData.userInterfaceDataType == TEXT_MESSAGE
	
	structUIData.userInterfaceDataType=EDIT;
	varVariantType = varVal.GetVarType();
	
	LOGIT(CLOG_LOG,  "_get_local_var_value::varVariantType= 0x%x\n", varVariantType );

	switch(varVariantType)
	{	  
		case RUL_CHAR:
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_INTEGER;
			//structUIData.EditBox.iValue=(int)varVal;
			structUIData.EditBox.editBoxValue = (char)varVal;
			structUIData.EditBox.nSize        = 1;
/*<START>Added by stevev 27feb06 for validating the list of  ranges */
			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.IntMinMaxVal.iMinval = SCHAR_MIN;
			tmpMinMaxVal.IntMinMaxVal.iMaxval = SCHAR_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
/*<END>Added by stevev for validating the list of  ranges */
		}
		break;
		case RUL_UNSIGNED_CHAR:	
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_INTEGER;
			//structUIData.EditBox.iValue=(int)varVal;
			structUIData.EditBox.editBoxValue = (unsigned char)varVal;
			structUIData.EditBox.nSize = 1;

			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.IntMinMaxVal.iMinval = 0;
			tmpMinMaxVal.IntMinMaxVal.iMaxval = UCHAR_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
		}
		break;
		case RUL_INT:
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_INTEGER;
			//structUIData.EditBox.iValue=(int)varVal;
			structUIData.EditBox.editBoxValue = (int)varVal;
			structUIData.EditBox.nSize = 4;
/*<START>Added by ANOOP for validating the list of  ranges */
			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.IntMinMaxVal.iMinval = INT_MIN;
			tmpMinMaxVal.IntMinMaxVal.iMaxval = INT_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);

/*			structUIData.EditBox.iMinValue=INT_MIN;
			structUIData.EditBox.iMaxValue=INT_MAX;
/*<END>Added by ANOOP for validating the list of  ranges */
		}
		break;
		case RUL_UINT:
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_INTEGER;
			//structUIData.EditBox.iValue=(int)varVal;
			structUIData.EditBox.editBoxValue = (unsigned int)varVal;
			structUIData.EditBox.nSize = 4;

			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.IntMinMaxVal.iMinval = 0;
			tmpMinMaxVal.IntMinMaxVal.iMaxval = UINT_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
		}
		break;
		case RUL_SHORT:	
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_INTEGER;
			//structUIData.EditBox.iValue=(int)varVal;
			structUIData.EditBox.editBoxValue = (short)varVal;
			structUIData.EditBox.nSize = 2;

			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.IntMinMaxVal.iMinval = SHRT_MIN;
			tmpMinMaxVal.IntMinMaxVal.iMaxval = SHRT_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
		}
		break;
		case RUL_USHORT:
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_INTEGER;
			//structUIData.EditBox.iValue=(int)varVal;
			structUIData.EditBox.editBoxValue = (unsigned short)(((unsigned int)varVal) & 0xffff);
			structUIData.EditBox.nSize = 2;

			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.IntMinMaxVal.iMinval = 0;
			tmpMinMaxVal.IntMinMaxVal.iMaxval = USHRT_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
		}
		break;
		case RUL_LONGLONG:
		{
			structUIData.EditBox.editBoxType  =  EDIT_BOX_TYPE_INTEGER;
			//structUIData.EditBox.iValue=(INT64)varVal;
			structUIData.EditBox.editBoxValue = (INT64)varVal;
			structUIData.EditBox.nSize = 8;

			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.IntMinMaxVal.iMinval = _LL_MIN;
			tmpMinMaxVal.IntMinMaxVal.iMaxval = _LL_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
		}
		break;
		case RUL_ULONGLONG:
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_INTEGER;
			//structUIData.EditBox.iValue=(INT64)varVal;
			structUIData.EditBox.editBoxValue = (UINT64)varVal;
			structUIData.EditBox.nSize = 8;

			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.IntMinMaxVal.iMinval = 0;
			tmpMinMaxVal.IntMinMaxVal.iMaxval = ULL_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);
		}
		break;
		case RUL_FLOAT:
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_FLOAT;
			//structUIData.EditBox.fValue=(float)varVal; 
			structUIData.EditBox.editBoxValue = (float)varVal; 
/*<START>Added by ANOOP for validating the list of  ranges */
			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.FloatMinMaxVal.fMinval = -FLT_MAX;  //Code corrected in the code review 
			tmpMinMaxVal.FloatMinMaxVal.fMaxval = FLT_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);

/*			structUIData.EditBox.fMinValue=FLT_MIN;
			structUIData.EditBox.fMaxValue=FLT_MAX;
/*<END>Added by ANOOP for validating the list of  ranges */
		}
		break;
		case RUL_DOUBLE:
		{
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_FLOAT;
			//structUIData.EditBox.fValue=(float)((double)varVal);  // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast> 
																  // stevev - merge 19feb07 - added par 750, editbox needs a double.
			structUIData.EditBox.editBoxValue = (double)varVal;
/*<START>Added by ANOOP for validating the list of  ranges */
			MinMaxVal tmpMinMaxVal;
			tmpMinMaxVal.FloatMinMaxVal.fMinval = -DBL_MAX;  //Code corrected in the code review 
			tmpMinMaxVal.FloatMinMaxVal.fMaxval = DBL_MAX;
			structUIData.EditBox.MinMaxVal.push_back(tmpMinMaxVal);

/*			structUIData.EditBox.fMinValue=FLT_MIN;
			structUIData.EditBox.fMaxValue=FLT_MAX;
/*<END>Added by ANOOP for validating the list of  ranges */
		}
		break;


/******* assumption: the UI will only allow the editing of wide characters, all strings will 
						go to/from wide char to be edited.                        ***************/
		case RUL_CHARPTR:
		case RUL_WIDECHARPTR:
		case RUL_DD_STRING:     // SF:EPM:  ADDED RUL_DD_STRING. This was not being handled !
		case RUL_SAFEARRAY:// only of type (one of three above)
		{
//			structUserInput.EditBox.iDefaultStringLength =
//			structUserInput.EditBox.iMaxStringLength     = MAX_DD_STRING;
//			structUserInput.EditBox.pchDefaultValue = NULL;  // we do wide chars
//			structUserInput.EditBox.pwcDefaultValue = new wchar_t[MAX_DD_STRING+1];
//			memset(structUserInput.EditBox.pwcDefaultValue,0,sizeof(wchar_t)*MAX_DD_STRING);
			/* stevev 28may09 - use hart varient for easier handling....
			structUIData.EditBox.editBoxType=EDIT_BOX_TYPE_STRING;
			structUIData.EditBox.iMaxStringLength=MAX_DD_STRING;
			structUIData.EditBox.iDefaultStringLength = 0;
			varVal.GetStringValue(&structUIData.EditBox.pwcDefaultValue);
			if( structUIData.EditBox.pwcDefaultValue )
			{
				structUIData.EditBox.iDefaultStringLength = wcslen(structUIData.EditBox.pwcDefaultValue);
			}
			*/
			structUIData.EditBox.editBoxType  = EDIT_BOX_TYPE_STRING;
			structUIData.EditBox.editBoxValue = (wstring) varVal.GetStringValue();//converts from narrow
			structUIData.EditBox.nSize = MAX_LEN_ALLOC;// stevev 14apr10 - found while doing 2493

			break;
		}
		default: // and RUL_NULL & RUL_SAFEARRAY & RUL_BOOL &  RUL_BYTE_STRING
			// TAB - 12/10/14 - not a noop - need to abort for this illegal and unhandled case
			LOGIT(CERR_LOG,  "ERROR:Illegal Edit Box Type in method. Type = 0x%x.\n", varVariantType );
			LOGIT(CERR_LOG,  "ERROR: Aborting Method\n" );
			bSetAbortFlag=true;
			break;

	}

//	add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
//I have commented out the previous line because if it is called here it overwrites important 
//     information from the strctIUIData. It is already called above.  This call is redundant.
	structUIData.bUserAcknowledge=true; 
	/*Vibhor 030304: Start of Code*/
	structUIData.bEnableAbortOnly = false; // just defensive
	/*Vibhor 030304: End of Code*/
	/*Vibhor 040304: Start of Code*/
	structUIData.uDelayTime = 0;// just defensive
	/*Vibhor 040304: End of Code*/
/*Vibhor 040304: Comment: Added the second condition below*/

	if(m_pMeth->GetMethodAbortStatus())// always true, you  just set it.. && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
	else
	{
		structUIData.bMethodAbortedSignalToUI =false;// defensive only
	}
	// stevev 02Jun14 - now set in bltin_format_string
	// structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204
//===============================================================================================
	if(false ==m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
	{
		bSetAbortFlag=true;
	}
	else
	{// stevev 30may14 - add dynamic update loop to the get local var value for dynamic in prompt
#ifdef MODIFIED_BY_SS		
	 //_tstrcpy(out_buf, mt_String);
		std::wstring strTemp(mt_String);
		std::copy(strTemp.begin(), strTemp.end(), out_buf);
#endif
		while(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay)
		{
			bDynaVarValChanged=false;
			out_buf[0] = '\0';
			int retval=
			   bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,pchDisplayString,
					plItemIds, iNumberOfItemIds, pDynVarVals, bDynaVarValChanged);

			if(true == bDynaVarValChanged )	
	{
				add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
			}
//=============================================================================================
			if(false == 
		       m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
			{
				bSetAbortFlag=true;
				break;
			}	
			LOOP_SLEEP;
		}// loop till not enabled
		
		logTime();// isa logif start_stop (has newline)
        structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204

		switch(structUserInput.userInterfaceDataType)
		{
			case EDIT:
			{
				switch(structUserInput.EditBox.editBoxType) 
				{
					case EDIT_BOX_TYPE_INTEGER:
					{
						switch(varVariantType)
						{
						case RUL_CHAR:			
						varVal = (char)			   structUserInput.EditBox.editBoxValue;break;
						case RUL_UNSIGNED_CHAR:	
						varVal = (unsigned char)   structUserInput.EditBox.editBoxValue ;break;
						case RUL_INT:			
						varVal = (short int)             structUserInput.EditBox.editBoxValue ;break;
						case RUL_UINT:			
						varVal = (unsigned int)    structUserInput.EditBox.editBoxValue ;break;
						case RUL_SHORT:			
						varVal = (short)           structUserInput.EditBox.editBoxValue ;break;
						case RUL_USHORT:
						{
						unsigned short y(structUserInput.EditBox.editBoxValue);
					//	varVal = (unsigned short)  structUserInput.EditBox.editBoxValue ;break;
						varVal = (wchar_t)y;
						}break;
						case RUL_LONGLONG:		
						varVal = (__int64)         structUserInput.EditBox.editBoxValue ;break;
						case RUL_ULONGLONG:		
						varVal = (unsigned __int64)structUserInput.EditBox.editBoxValue ;break;
						default:				varVal.Clear();
						}
						//varVal = (INT64)structUserInput.EditBox.iValue;
						m_pInterpreter->SetVariableValue(pchVariableName,varVal);
						break;
					}
					case EDIT_BOX_TYPE_FLOAT:
					{
						if ( varVariantType == RUL_FLOAT )
						{
							varVal = (float)structUserInput.EditBox.editBoxValue ;
						}
						else
						if ( varVariantType == RUL_DOUBLE )
						{
							varVal = (double)structUserInput.EditBox.editBoxValue ;
						}
						else
						{
							varVal.Clear();
						}
						//if( varVal.GetVarType() == RUL_FLOAT )//WS:EPM 10aug07
						//{
						//varVal  = (float)structUserInput.EditBox.fValue;
						//}//WS:EPM 10aug07
						//else if( varVal.GetVarType() == RUL_DOUBLE )//WS:EPM 10aug07
						//{
						//	varVal  = (double)structUserInput.EditBox.fValue;//WS:EPM 10aug07
						//}
						m_pInterpreter->SetVariableValue(pchVariableName,varVal);
						break;
					}
					case EDIT_BOX_TYPE_STRING:
					{
					//	varVal = (wchar_t *)structUserInput.EditBox.pwcDefaultValue;
						varVal = (wchar_t *) ((wstring)structUserInput.EditBox.editBoxValue).c_str();
						m_pInterpreter->SetVariableValue(pchVariableName,varVal);
						break;
					}
					case UNKNOWN_EDIT_BOX_TYPE:
					{
						break;
					}
				}
				break;
			}
			break;
		}
	
	}
	
		

	if(structUIData.textMessage.pchTextMessage)
	{
		delete[] structUIData.textMessage.pchTextMessage;
	    structUIData.textMessage.pchTextMessage = NULL;	
	}

//	if(structUIData.EditBox.pchDefaultValue)
//	{
//		delete[] structUIData.EditBox.pchDefaultValue;
//		structUIData.EditBox.pchDefaultValue=NULL;
//	}

//	if(structUIData.EditBox.pwcDefaultValue)
//	{
//		delete[] structUIData.EditBox.pwcDefaultValue;
//		structUIData.EditBox.pwcDefaultValue = NULL;
//	}
	
//	if(structUserInput.EditBox.pchDefaultValue)
//	{
//		delete[] structUserInput.EditBox.pchDefaultValue;
//		structUserInput.EditBox.pchDefaultValue = NULL;
//	}
	
//	if(structUserInput.EditBox.pwcDefaultValue)
//	{
//		delete[] structUserInput.EditBox.pwcDefaultValue;
//		structUserInput.EditBox.pwcDefaultValue = NULL;
//	}
	
	
	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}
}// end _get_local_var_value


int CHart_Builtins::display(tchar *message,long * glob_var_ids,int iNumberOfItemIds)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar disp_msg[MAX_LEN_ALLOC]={0};
	bool	bSetAbortFlag=false;
/*<START>Added by ANOOP for dynamci vars %0  */
	CValueVarient 	pDynVarVals[50];
	clearVarientArray(pDynVarVals);
	bool			bDynaVarValChanged=false;
	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>Added by ANOOP for dynamci vars %0  */	
	int rc=m_pDevice->dictionary->get_string_translation(message,message,_tstrlen(message) +1);

	int retval=bltin_format_string(disp_msg,MAX_LEN_ALLOC,UPDATE_ALL,message,
					glob_var_ids,iNumberOfItemIds,pDynVarVals,bDynaVarValChanged);

	//jad debug
	LOGIT(CLOG_LOG, "CHart_Builtins::display");



	add_textMsg(structUIData,disp_msg);	// stevev 26dec07 - common code
	structUIData.userInterfaceDataType = TEXT_MESSAGE;

	structUIData.bUserAcknowledge=true; 
	/*Vibhor 030304: Start of Code*/
	structUIData.bEnableAbortOnly = false; // just defensive
	/*Vibhor 030304: End of Code*/
	/*Vibhor 040304: Start of Code*/
	structUIData.uDelayTime = 0;// just defensive
	/*Vibhor 040304: End of Code*/
/*Vibhor 040304: Comment: Added the second condition below*/

	if(m_pMeth->GetMethodAbortStatus())// always true, you  just set it..  && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
	// stevev 02Jun14 - now set in bltin_format_string
	// structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204
	if(false == 
		     m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	else
	{
	
#ifdef MODIFIED_BY_SS		
	//_tstrcpy(disp_msg,mt_String);
	std::wstring strTemp(mt_String);
	std::copy(strTemp.begin(), strTemp.end(), disp_msg);
#endif
	while(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay )
	{
			bDynaVarValChanged=false;
		disp_msg[0] = '\0';
			int retval=bltin_format_string(disp_msg,MAX_LEN_ALLOC,UPDATE_ALL,message,
					 glob_var_ids,iNumberOfItemIds,pDynVarVals,bDynaVarValChanged);

		if(true == bDynaVarValChanged )	
		{
			add_textMsg(structUIData,disp_msg);	// stevev 26dec07 - common code
		}	
//=============================================================================================
				// has to be after the format so bEnableDynamicDisplay is preserved 4 while test
		if(false == 
			   m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
		{
			bSetAbortFlag=true;
				break;
		}
			LOOP_SLEEP;			
		}//wend till disabled
	}

	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
    structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204

	if(structUIData.textMessage.pchTextMessage)
	{
		delete[] structUIData.textMessage.pchTextMessage; 
		structUIData.textMessage.pchTextMessage = NULL;
	}
	
	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}
}




int CHart_Builtins::SELECT_FROM_LIST
			(
				tchar *pchDisplayString
				, tchar *pchList
			)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar out_buf[MAX_LEN_ALLOC]={0};
	tchar lst_buf[MAX_LEN_ALLOC]={0};
	char *strList=NULL,*curr_ptr=NULL;
	unsigned int iTimeInSeconds=PUT_MESSAGE_SLEEP_TIME;  //preset Time for the message to be displayed
	int nCntr=0,pos=0,rc;
	bool	bSetAbortFlag=false;
/*<START>14FEB04  Added by ANOOP for dynamic variables %0 */
	CValueVarient 	pDynVarVals[50], pDynListVals[50];
	clearVarientArray(pDynVarVals);
	clearVarientArray(pDynListVals);
	bool			bDynaVarValChanged=false;	
	
	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>14FEB04  Added by ANOOP for dynamic variables %0 */	
	
	rc=m_pDevice->dictionary->get_string_translation(pchDisplayString,pchDisplayString,
																_tstrlen(pchDisplayString) +1);
	int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,pchDisplayString,
									  NULL,0,pDynVarVals,bDynaVarValChanged);
	add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code

//*** CHECK *****
//Each element in the ';' sep'd list may have multi languages AND variable names as PUT_MESSAGE
// stevev 30may14 - yes that is true, the prompt string and all the select strings.
	rc    = m_pDevice->dictionary->get_string_translation(pchList,pchList,_tstrlen(pchList) +1);
	retval= bltin_format_string(lst_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,
									pchList,NULL,0,pDynListVals,bDynaVarValChanged);
	add_optionList(structUIData, lst_buf);
	

	structUIData.bUserAcknowledge=true;
	/*Vibhor 030304: Start of Code*/
	structUIData.bEnableAbortOnly = false; // just defensive
	/*Vibhor 030304: End of Code*/
	/*Vibhor 040304: Start of Code*/
	structUIData.uDelayTime = 0;// just defensive
	/*Vibhor 040304: End of Code*/

	structUIData.userInterfaceDataType=COMBO;
	structUIData.ComboBox.comboBoxType = COMBO_BOX_TYPE_SINGLE_SELECT;//WS:EPM 30apr07

/*Vibhor 040304: Comment: Added the second condition below*/
	if(m_pMeth->GetMethodAbortStatus() )// always true, you  just set it.. && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
	// stevev 02Jun14 - now set in bltin_format_string
	// structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204
	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	else
	{
	while(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay  )
	{
			bDynaVarValChanged=false;
		out_buf[0] = lst_buf[0] = 0;

			retval= bltin_format_string(out_buf,MAX_LEN_ALLOC,UPDATE_NORMAL,pchDisplayString,
										   NULL,0,pDynVarVals,bDynaVarValChanged);		
		if(true == bDynaVarValChanged )	
		{
			add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
		}

			bDynaVarValChanged=false;
			retval= bltin_format_string(lst_buf,MAX_LEN_ALLOC, UPDATE_NORMAL,pchList,
												NULL,0,pDynListVals,bDynaVarValChanged);	
		if(true == bDynaVarValChanged )	
		{
			add_optionList(structUIData, lst_buf);
		}
//=============================================================================================
				// has to be after the format so bEnableDynamicDisplay is preserved 4 while test
			if(false == 
			   m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
		{
			bSetAbortFlag=true;
				break;
		}
			LOOP_SLEEP;	
		}//wend till disabled
	}

	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
  	structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204
	/*<END>14FEB04 Added by ANOOP for dynamic variables  %0	*/

	if(NULL != structUIData.textMessage.pchTextMessage)
	{
		delete[] structUIData.textMessage.pchTextMessage;
		structUIData.textMessage.pchTextMessage = NULL;
	}
	
	//Added by ANOOP 200204
	if(NULL != structUIData.ComboBox.pchComboElementText)
	{
		delete[] structUIData.ComboBox.pchComboElementText;
	}

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		if(structUserInput.userInterfaceDataType == COMBO)
		{//WS:EPM 30apr07 - start section
			if( structUserInput.ComboBox.comboBoxType == COMBO_BOX_TYPE_SINGLE_SELECT )
			{
				return (structUserInput.nComboSelection - 1);
			}
			else if( structUserInput.ComboBox.comboBoxType == COMBO_BOX_TYPE_MULTI_SELECT )
			{
				return structUserInput.nComboSelection;
			}
			else
			{
					return BI_ERROR;// was structUserInput.nComboSelection - 1;
			}
		}//WS:EPM 30apr07 - end section
		else
		{
			return BI_ERROR;
		}
	}

}

int CHart_Builtins::_display_xmtr_status
		(
			long lItemId
			, int  iStatusValue
		)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tstring status_str;
	hCitemBase* pIB = NULL;
	bool	bSetAbortFlag=false;

	//I am being very defensive.  I dont want the uDelayTime to be set to some inifinite number
	// because it is not set in the following code.
	structUIData.bUserAcknowledge=true;
	structUIData.bEnableAbortOnly = false; // just defensive
	structUIData.uDelayTime = 0;// just defensive

	if (m_pDevice->getItemBySymNumber(lItemId, &pIB) == SUCCESS)
	{
		hCVar *pVar=(hCVar *)pIB;
		switch(pVar->VariableType() )
		{
			case vT_Enumerated:
			{
				hCEnum *pEnum=(hCEnum *)pVar;
				pEnum->procureString(iStatusValue, status_str); // input value, get a string
				if(!status_str.empty())
				{
					if(status_str.length() >0)
					{
						add_textMsg(structUIData, (tchar*)status_str.c_str());	// stevev 26dec07 - common code
						structUIData.userInterfaceDataType = TEXT_MESSAGE;
					}	
					else
					{
						//do something?
					}
				}
				break;
			}		
			
			case vT_BitEnumerated:
			{
				hCBitEnum *pBitEnum=(hCBitEnum *)pVar;
				pBitEnum->procureString(iStatusValue, status_str); // input value, get a string
				if(!status_str.empty())
				{
					if(status_str.length() >0)
					{
						add_textMsg(structUIData, (tchar*)status_str.c_str());	// stevev 26dec07 - common code
						structUIData.userInterfaceDataType = TEXT_MESSAGE;
					}	
					else
					{
						//do something?
					}
				}
				break;
			}		
			default:
				//do something?
				break;
		
		}	
/*Vibhor 040304: Comment: Added the second condition below*/

		if(m_pMeth->GetMethodAbortStatus() && (structUIData.bEnableAbortOnly == false))
		{
			structUIData.bMethodAbortedSignalToUI =true;
		}
		structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204
		structUIData.userInterfaceDataType=TEXT_MESSAGE;	//Added by ANOOP 05FEB2004
		if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
		{
			bSetAbortFlag=true;
		}
	
	}

	if(structUIData.textMessage.pchTextMessage)  //Added by ANOOP 19FEB2004 
	{
		delete[] structUIData.textMessage.pchTextMessage;
		structUIData.textMessage.pchTextMessage = NULL;
	}
	
	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}

}
	
int CHart_Builtins::select_from_list
		(
			tchar *pchDisplayString
			, long *lItemId
			, int iNumberOfItemIds
			, tchar *pchList
		)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	tchar out_buf[MAX_LEN_ALLOC]={0};
	tchar lst_buf[MAX_LEN_ALLOC]={0};

	unsigned int iTimeInSeconds=2000;  //preset Time for the message to be displayed
	int nCntr=0,pos=0,rc;
	bool	bSetAbortFlag=false;
/*<START>14FEB04  Added by ANOOP for dynamic variables %0 */
	CValueVarient 	pDynVarVals[50],pDynListVals[50];
	clearVarientArray(pDynVarVals);
	clearVarientArray(pDynListVals);
	bool			bDynaVarValChanged=false;

	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
/*<END>14FEB04  Added by ANOOP for dynamic variables %0 */
	rc=m_pDevice->dictionary->get_string_translation(pchDisplayString,pchDisplayString,
																_tstrlen(pchDisplayString) +1);
	
	int retval= bltin_format_string(out_buf,MAX_LEN_ALLOC, UPDATE_NORMAL,pchDisplayString,
					lItemId,iNumberOfItemIds,pDynVarVals,bDynaVarValChanged);
	add_textMsg(structUIData, out_buf);	// stevev 26dec07 - common code

	// we need to save it because the next call will probably change it
	bool valueWasUpdate = m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay;

//*** CHECK ****
//Each element in the ';' sep'd list may have multi languages AND variable names as PUT_MESSAGE
	rc    = m_pDevice->dictionary->get_string_translation(pchList,pchList,_tstrlen(pchList) +1);
	retval= bltin_format_string(lst_buf,MAX_LEN_ALLOC, UPDATE_NORMAL,pchList,
											NULL,0,pDynListVals,bDynaVarValChanged);
	add_optionList(structUIData, lst_buf);

	structUIData.bUserAcknowledge=true;
	/*Vibhor 030304: Start of Code*/
	structUIData.bEnableAbortOnly = false; // just defensive
	/*Vibhor 030304: End of Code*/
	/*Vibhor 040304: Start of Code*/
	structUIData.uDelayTime = 0;// just defensive
	/*Vibhor 040304: End of Code*/

	if ( valueWasUpdate && ! m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay )
	{
		m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay =
		structUIData.bDisplayDynamic = true;
	}

	structUIData.userInterfaceDataType=COMBO;
	structUIData.ComboBox.comboBoxType = COMBO_BOX_TYPE_SINGLE_SELECT;// WS:EPM 30apr07

/*Vibhor 040304: Comment: Added the second condition below*/

	if(m_pMeth->GetMethodAbortStatus())// always true, you  just set it..  && (structUIData.bEnableAbortOnly == false))
	{
		structUIData.bMethodAbortedSignalToUI =true;
	}
	// stevev 02Jun14 - now set in bltin_format_string 
	// structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204
	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput) )
	{
		bSetAbortFlag=true;
	}
	else
	{
		while(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay)
		{
			bDynaVarValChanged=false;
		out_buf[0] = lst_buf[0] = 0;

			retval= bltin_format_string(out_buf,MAX_LEN_ALLOC,  UPDATE_NORMAL,pchDisplayString,
						  lItemId,iNumberOfItemIds,pDynVarVals,bDynaVarValChanged);
		
		if(true == bDynaVarValChanged )	
		{
			add_textMsg(structUIData,out_buf);	// stevev 26dec07 - common code
		}
			// save it to combine with next
			valueWasUpdate = m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay;

			// now do the selection update
			bDynaVarValChanged=false;
			retval= bltin_format_string(lst_buf,MAX_LEN_ALLOC,  UPDATE_NORMAL,pchList,
										   NULL,0,pDynListVals,bDynaVarValChanged);	
		if(true == bDynaVarValChanged )	
		{
			add_optionList(structUIData, lst_buf);
		}

			if ( valueWasUpdate || m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay )
			{
				m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay =
	    structUIData.bDisplayDynamic = true;
			}
//=============================================================================================
				// has to be after the format so bEnableDynamicDisplay is preserved 4 while test
			if(false == 
			   m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInput))
//=============================================================================================
		{
			bSetAbortFlag=true;
				break;
		}
			LOOP_SLEEP;
		}//wend till disabled
	}

	m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=false;
  	structUIData.bDisplayDynamic = false;	//Added by ANOOP 200204

	/*<END>14FEB04  Added by ANOOP for dyanmaic variables %0 */

	if(NULL != structUIData.textMessage.pchTextMessage)
	{
		delete[] structUIData.textMessage.pchTextMessage;
		structUIData.textMessage.pchTextMessage = NULL;
	}
	
	//Added by ANOOP 200204
	if(NULL != structUIData.ComboBox.pchComboElementText)
	{
		delete[] structUIData.ComboBox.pchComboElementText;
	}

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		if(structUserInput.userInterfaceDataType == COMBO)
		{
			//WS:EPM 30apr07- start section
			if( structUserInput.ComboBox.comboBoxType == COMBO_BOX_TYPE_SINGLE_SELECT )
			{
				return (structUserInput.nComboSelection - 1);
			}
			else if( structUserInput.ComboBox.comboBoxType == COMBO_BOX_TYPE_MULTI_SELECT )//WS:EPM 30apr07
			{
				return structUserInput.nComboSelection;
			}
			else
			{
				return BI_ERROR;
			}//WS:EPM 30apr07 - end section
		}
		else
		{
			return BI_ERROR;
		}
	}

}

/*Arun 190505 Start of code*/

tchar* CHart_Builtins::BUILD_MESSAGE
		(
			tchar* dest
		  , tchar* message
		)
{        
	bool bDynVarValsChanged = true; 

    if( m_pDevice ) 
    { 
        if( m_pDevice->dictionary ) 
        { 
            m_pDevice->dictionary->get_string_translation( message, message, _tstrlen(message)+1 ); 
            bltin_format_string( dest, MAX_LEN_ALLOC,up_DONOT_UPDATE,message,
								 NULL, 0, NULL, bDynVarValsChanged); 
        } 
    } 

    return dest; 
}

/*End of code*/

//Added By Anil September 26 2005 for _MenuDisplay
int CHart_Builtins::_MenuDisplay(long lMenuId, tchar *pchOptionList, long* lselection)
{
	//Fill the structure required for the Menu display
	ACTION_UI_DATA structUIData;
	ACTION_USER_INPUT_DATA structUserInputData;
	structUIData.userInterfaceDataType = MENU;
	structUIData.DDitemId = lMenuId;

//**** CHECK ****
//Each element in the ';' sep'd list may have multi languages but MAY NOT have any variables
	int rc=m_pDevice->dictionary->get_string_translation(pchOptionList,pchOptionList,
																_tstrlen(pchOptionList) +1);
	// stevev 04oct05 - add extra buttons so dialog can tell it's a display menu
	tchar* szTemp = new tchar[ _tstrlen(pchOptionList) + 10];

	
#ifdef MODIFIED_BY_SS		
	//_tstrcpy(szTemp,_T("Abort;"));
	//_tstrcat(szTemp, );
	std::wstring strTemp(_T("Abort;"));
	strTemp.append(pchOptionList);
	std::copy(strTemp.begin(), strTemp.end(), szTemp);

#endif
	
	int c,i;
	for ( c = 0, i = 0; i < (int)_tstrlen(szTemp); i++ ) // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
	{
		if ( (szTemp[i] == _T(';') )   || 
			 (szTemp[i] != _T(';')   && szTemp[i+1] == 0)   )
		{
			c++;
		}
	}
	structUIData.ComboBox.pchComboElementText = szTemp;
	structUIData.ComboBox.iNumberOfComboElements = c;
	// end stevev 04oct05
	bool	bSetAbortFlag=false;
	//As of today this functionality is not complete as Steve has to make changes in the MethodDisplay code
	//Any attempt to call this builtin will crash the SDC

	//jad debug
	LOGIT(CLOG_LOG, "before menudisplay, menuID = %d", structUIData.DDitemId);

	if(false == m_pDevice->m_pMethSupportInterface->MethodDisplay(structUIData,structUserInputData) )
	{
		bSetAbortFlag=true;
	}

	*lselection = structUserInputData.nComboSelection;
	if(szTemp)
	{
		delete[] szTemp;
	}
	// stevev added 4oct07

	if(bSetAbortFlag)
	{
		m_pMeth->process_abort();
		return (METHOD_ABORTED);
	}
	else
	{
		return (BI_SUCCESS);
	}
	//return BI_SUCCESS;

}//End of MenuDisplay

/*************** <END> Methods UI Built-ins	**************************************/
