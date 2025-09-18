//#include "stdafx.h"
#pragma warning (disable : 4786)
//#include <windows.h>
//#include <ALPHAOPS.H>
#include "pch.h"
#include "ddbGeneral.h"
#include <limits.h>
#include <float.h>
#include "ddbDevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"
#include "MEE.h"
#ifndef USING_FMAPARSER
	#include "LitStringTable.h"
#else
    #include "FMA_Dict.h" // holds both
#endif
#include "retn_code.h"

class Cdictionary;

/*********************************Vibhor 270204:Start of Comment*********************************
Signature of Read() has been modified to pass a bool flag, telling whether the reqd value 
needs to be scaled or not.
All the calls in this file DO NOT REQUIRE scaling so "false" is passed as the third argument
/*********************************Vibhor 270204:End of Comment***********************************/

/*************** <START> Methods UI Built-ins	**************************************/

float CHart_Builtins::_fvar_value
		(
			long lItemId
		)
{
	float fretVal;
	CValueVarient ppReturnedDataItem;

	if( Read(lItemId,ppReturnedDataItem,false) == BI_SUCCESS )
	{
		fretVal=(float)ppReturnedDataItem;
		return(fretVal);
	}
	else
	{
		return BI_ERROR;
	}	

}	
INT64 CHart_Builtins::_ivar_value
		(
			long lItemId
		)
{
	INT64 iretVal=0;
	CValueVarient ppReturnedDataItem;

	if( Read(lItemId,ppReturnedDataItem,false) == BI_SUCCESS )
	{
		iretVal=(INT64)ppReturnedDataItem;
		return(iretVal);
	}
	else
	{
		return BI_ERROR;
	}	

}
	
INT64 CHart_Builtins::_lvar_value
		(
			long lItemId
		)
{
	INT64 lretVal=0;
	hCitemBase* pIB = NULL;
	hCVar *pVar=NULL;
	CValueVarient ppReturnedDataItem;

	if( Read(lItemId,ppReturnedDataItem,false) == BI_SUCCESS )
	{
		lretVal = (INT64)ppReturnedDataItem;
		return(lretVal);
	}
	else
	{
		return BI_ERROR;
	}	

}

int CHart_Builtins::_get_status_code_string
		(
			long lItemId
			, int iStatusCode
			, tchar *pchStatusString
			, int iStatusStringLength  // truncates at this length
		)
{
	wstring status_str;
	hCitemBase* pIB = NULL;

	if (m_pDevice->getItemBySymNumber(lItemId, &pIB) == SUCCESS && pIB != NULL)
	{
		hCVar *pVar=(hCVar *)pIB;

		switch(pVar->VariableType())
		{
			case vT_Enumerated:
			{
				hCEnum *pEnum=(hCEnum *)pVar;
				pEnum->procureString(iStatusCode, status_str); // input value, get a string							
				break;
			}
			case vT_BitEnumerated:
			{
				hCBitEnum *pBitEnum=(hCBitEnum *)pVar;
				pBitEnum->procureString(iStatusCode, status_str); // input value, get a string
				break;					
			}
			default:
				break;
		}
		
		if(!status_str.empty())
		{
#ifdef MODIFIED_BY_SS
			/*_tstrncpy(pchStatusString,status_str.c_str(),(iStatusStringLength-1));*/
			// std::string status_str = "Your status string";

			status_str.copy(pchStatusString, iStatusStringLength - 1);			
			pchStatusString[iStatusStringLength-1] = '\0';
#endif
		}
		else // stevev - 30may07 -- EVERY 'if' must have an 'else' - even if it's just a comment!!!
		{
			pchStatusString[0] = '\0';
		}
		return BI_SUCCESS;
	}
	else
	{
		return BI_ERROR;
	}	
}


int CHart_Builtins::_get_dictionary_string
		(
			long lItemId
			, tchar *pchDictionaryString
			, int iMaxStringLength
		)
{
	tchar *strTmp=NULL;

	wstring str, newstr;
	m_pDevice->dictionary->get_dictionary_string(lItemId,newstr);
	m_pDevice->dictionary->get_string_translation(newstr, str);

	if(!str.empty() && iMaxStringLength > 0)
	{
#ifdef MODIFIED_BY_SS
		//_tstrncpy(pchDictionaryString,str.c_str(),(iMaxStringLength-1));
		// std::string status_str = "Your status string";
		str.copy(pchDictionaryString, iMaxStringLength - 1);
		pchDictionaryString[iMaxStringLength-1] = _T('\0');
#endif
	}
	else
	{
		pchDictionaryString[0] = _T('\0');
	}
	return (BI_SUCCESS);
}

//Anil 22 December 2005 for dictionary_string built in
int CHart_Builtins::_dictionary_string
		(
			long lItemId
			, tchar **pchDictionaryString			
		)
{
	wstring str;
	int r = m_pDevice->dictionary->get_dictionary_string(lItemId,str);
	
	if(!str.empty())
	{
#ifdef MODIFIED_BY_SS
		*pchDictionaryString = new tchar[str.size()+1];
		//_tstrcpy(*pchDictionaryString,str.c_str());
		str.copy(*pchDictionaryString, str.size());
		*pchDictionaryString[str.size()] = _T('\0');
#endif
	}
	if ( r != DDL_SUCCESS )
		return (BI_ERROR);
	else
		return (BI_SUCCESS);
}

// stevev 29jan08
int CHart_Builtins::literal_string
		(
			long lItemId
			, tchar **pchLiteralString			
		)
{
	wstring str,ostr;
	if ( ! m_pDevice->literalStringTbl )
		return (BI_ERROR);
	str = m_pDevice->literalStringTbl->get_lit_string(lItemId);
	
	if(!str.empty())
	{// stevev 21sep10-literal string function must return a single language
		int rc = m_pDevice->dictionary->get_string_translation(str,ostr);
		if ( rc == DDS_SUCCESS )
		{
			*pchLiteralString = new tchar[ostr.size()+1];
			ostr.copy(*pchLiteralString, ostr.size());
			*pchLiteralString[ostr.size()] = _T('\0');
			//_tstrcpy(*pchLiteralString,ostr.c_str());
			return (BI_SUCCESS);
		}
		else
		{
			return (BI_ERROR);
		}
	}
	else
		return (BI_ERROR);
}

/*Arun 190505 Start of code*/

int CHart_Builtins::get_enum_string
		(
			long lItemId
			, int iStatusCode
			, tchar *pchStatusString
			, int iStatusStringLength
		)
{
	int iReturnValue;
	iReturnValue =  _get_status_code_string(lItemId,iStatusCode,pchStatusString,iStatusStringLength);
	return iReturnValue;
}

/*End of code*/

int CHart_Builtins::_iassign(long item_id,INT64 new_value)
{
	hCitemBase* pIB = NULL;
	
	if(m_pDevice->getItemBySymNumber(item_id,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarDest = (hCVar*)pIB;
	
		if(true == pVarDest->IsNumeric())
		{/* Ideally we should have checked for IsFloat, but there are DDs 
			which assign one numeric type to other */
			CValueVarient tempValue; tempValue = new_value;	
			pVarDest->setRawDispValue(tempValue);			//Set the raw value.Handles Locals
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                            //                                    dynamic comands
			return BI_SUCCESS;
		}
	}	
	// We come here ONLY if we fell through one of the above conditions....
	return BI_ERROR;	
}

int CHart_Builtins::_lassign(long item_id,INT64 new_value)
{
	int nRetVal = BI_ERROR;
	hCitemBase* pIB = NULL;

	if(m_pDevice->getItemBySymNumber(item_id,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarDest = (hCVar*)pIB;
	
		if(true == pVarDest->IsNumeric())
		{
			CValueVarient tempValue; tempValue = new_value;
			pVarDest->setRawDispValue(tempValue);			//Set the raw value.Handles Locals
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                            //                                    dynamic comands
			nRetVal = BI_SUCCESS;//mark as having completed successfully
		}
	}
	return nRetVal;//single exit point
}


int CHart_Builtins::_fassign(long item_id,float new_value)
{
	hCitemBase* pIB = NULL;
	
	if(m_pDevice->getItemBySymNumber(item_id,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarDest = (hCVar*)pIB;
	
		if(true == pVarDest->IsNumeric())
		{/* Ideally we should have checked for IsFloat, but there are DDs 
			which assign one numeric type to other */
			CValueVarient tempValue; tempValue = new_value;
			pVarDest->setRawDispValue(tempValue);			//Set the raw value.Handles Locals
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                            //                                    dynamic comands
			return BI_SUCCESS;
		}
	}	
	// We come here ONLY if we fell through one of the above conditions....
	return BI_ERROR;
}

int CHart_Builtins::_dassign(long item_id,double new_value)
{
	hCitemBase* pIB = NULL;	
	
	if(m_pDevice->getItemBySymNumber(item_id,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarDest = (hCVar*)pIB;

		if(true == pVarDest->IsNumeric())
		{/* Ideally we should have checked for IsDouble, but there are DDs 
			which assign one numeric type to other */
			CValueVarient tempValue; tempValue = new_value;
			pVarDest->setRawDispValue(tempValue);			//Set the raw value.Handles Locals
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                            //                                    dynamic comands
			return BI_SUCCESS;
		}
	}
	// We come here ONLY if we fell through one of the above conditions....
	return BI_ERROR;	
}

int CHart_Builtins::_vassign(long item_id_dest,long item_id_source)
{
	CValueVarient Var_src;
	hCitemBase* pIB = NULL;
	hCVar *pVarDest=NULL;

	if(m_pDevice->getItemBySymNumber(item_id_source,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarSrc = (hCVar*)pIB;
		Var_src = pVarSrc->getRawDispValue();//was getDispValue();		
		pIB = NULL;
		if(m_pDevice->getItemBySymNumber(item_id_dest,&pIB) == SUCCESS && NULL != pIB)
		{
			pVarDest = (hCVar*)pIB;
			if ( (pVarDest->VariableType() == pVarSrc->VariableType()) &&
				 (pVarDest->VariableSize() >= pVarSrc->VariableSize())    )
			{
				pVarDest->setRawDispValue(Var_src);			//Set the raw value.Handles Locals	
				pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
					                        //                                    dynamic comands					
				return BI_SUCCESS;
			}		
		}	
	}
	// We come here ONLY if we fell through one of the above conditions....
	return BI_ERROR;	
}	

int CHart_Builtins::Read(long item_id,   CValueVarient & ppReturnedDataItem,bool bScalingReqd)
{
	hCitemBase* pIB = NULL;
	hCVar *pVar=NULL;
	hCcommand *pC=NULL;
	CCmdList *pCmdList = NULL;
				
	if (m_pDevice->getItemBySymNumber(item_id, &pIB) == SUCCESS)
	{
		if(NULL != pIB)
		{
			pVar=(hCVar *)pIB;
			
			INSTANCE_DATA_STATE_T ids = pVar->getDataState();
			DATA_QUALITY_T         dq = pVar->getDataQuality();
			
			if ( dq == DA_NOT_VALID ||  dq == DA_STALEUNK
					|| ids == IDS_UNINITIALIZED || ids == IDS_INVALID
					||(ids == IDS_STALE && dq== DA_STALEUNK) )	
			{
				
				hCcommandDescriptor rdc = pVar->getRdCmd();
//				localIdxUse =  rdc;
				if ( rdc.cmdNumber < 0xFFFF /*0xFF Commented by Anil October 25 2005 and Changed to 0xFFFF PAR 5539 */  
					&& rdc.cmdNumber > -1 )
				{						
					pCmdList = (CCmdList*) m_pDevice->getListPtr(iT_Command);//<hCcommand*> 				
					
					pC = pCmdList->getCmdByNumber(rdc.cmdNumber);
					/* stevev 16apr07 the above is easier to maintain...
					// get cmd ptr
					for (CCmdList::iterator iCT = pCmdList->begin(); 
						 iCT < pCmdList->end();                 iCT++ )
					{// iCT is ptr 2 ptr to hCcommand
						if ( rdc.cmdNumber == (*iCT)->getCmdNumber() )
						{
							pC = (hCcommand*)(*iCT);
							break; // out of for loop
						}
					}// next command
					....***/
					if ( pC != NULL )// command supported
					{
/*Vibhor 220204: Start of Code*/
					// was	int retVal=m_pDevice->sendMethodCmd( rdc.cmdNumber,0);
						// stevev 30nov11 - added index list
						int retVal=m_pDevice->sendMethodCmd( rdc.cmdNumber,
												DEFAULT_TRANSACTION_NUMBER,&(rdc.idxList));
/*Vibhor 220204: End of Code*/
						if (retVal != BI_SUCCESS )
						{
							//call abort method
							return BI_ERROR; // Vibhor 220204
						}
					}
				}// if there's no read Command for the Var, just fall through...
			}	
/*Vibhor 270204: Start of Code*/
			if(bScalingReqd)
			{
			ppReturnedDataItem=pVar->getDispValue(); /* VMKP removed repetative code from two 
													 places above and put it in common place 
													on 200204 */
			}
			else
			{//Scaling not required !!
				ppReturnedDataItem=pVar->getRawDispValue();

//jad debug
//LOGIT(CERR_LOG,"SetStoragePath copy = %s, %s",ppReturnedDataItem->);

			}
/*Vibhor 270204: End of Code*/		
		}	
		return BI_SUCCESS;
	}
	else
	{
		return BI_ERROR;
		
	}

}



int CHart_Builtins::Write(long  item_id,   CValueVarient ppReturnedDataItem)
{
	hCitemBase* pIB = NULL;
	hCVar *pVar=NULL;
	hCcommand *pC=NULL;
	CCmdList *pCmdList = NULL;
				
	if (m_pDevice->getItemBySymNumber(item_id, &pIB) == SUCCESS)
	{
		if(NULL != pIB)
		{
			pVar=(hCVar *)pIB;

			INSTANCE_DATA_STATE_T ids = pVar->getDataState();
			DATA_QUALITY_T         dq = pVar->getDataQuality();		

			if ( ids != IDS_CACHED )	
			{
				/* stevev 01oct08 -- the assumption here, that the command will be successful, has no basis.
				   we will leave the command dispatcher to decide if the value becomes cached
				pVar->markItemState(IDS_CACHED);
				   note that this may be an issue for write commands that don't 'echo-back' their values.
				   If this occurs, we will need to determine a better solution
				***/
				
				hCcommandDescriptor rdc = pVar->getWrCmd();
				if ( rdc.cmdNumber < 0xFFFF /* stevev 16apr07 caught in walk thru 0xFF*/ && rdc.cmdNumber > -1 )
				{
					pCmdList = (CCmdList*) m_pDevice->getListPtr(iT_Command);//<hCcommand*> 

					pC = pCmdList->getCmdByNumber(rdc.cmdNumber);
					/* stevev 16apr07 the above is easier to maintain...				
					// get cmd ptr
					for (CCmdList::iterator iCT = pCmdList->begin(); 
						 iCT < pCmdList->end();                iCT++ )
					{// iCT is ptr 2 ptr to hCcommand
						if ( rdc.cmdNumber == (*iCT)->getCmdNumber() )
						{
							pC = (hCcommand*)(*iCT);
							break; // out of for loop
						}
					}// next command
					.../***/
					if ( pC != NULL )// command supported
					{// stevev 30nov11 - change transaction to default, add index list
						int retVal=m_pDevice->sendMethodCmd( rdc.cmdNumber,
													DEFAULT_TRANSACTION_NUMBER,&(rdc.idxList));
						if (retVal != BI_SUCCESS )
						{
							//call abort method
						}
						
					}
				}
				
			}	
			(void)pVar->setDispValue(ppReturnedDataItem);
		}	
		return BI_SUCCESS;
	}
	else
	{
		return BI_ERROR;
	}

}

//Vibhor 200905: Deleted the commented code


int CHart_Builtins::rspcode_string
		(
			int iCommandNumber
			, int iResponseCode
			, tchar *pchResponseCodeString
			, int iResponseCodeStringLength
		)
{
	varPtrList_t itemList;
	int nHandle = 0;		// Used for optimisation, 0 means resolve it otherwise use the existing resolved list.
	int retVal = BI_SUCCESS;

	responseCodeList_t respCodeList;

	//hCrespCode	rspCode(m_pDevice->devHndl());
	hCrespCode		*pRspCode = NULL;

	hCRespCodeList	*tmprespCodeList;// treat as a vector of <hCrespCode>

	tmprespCodeList = (hCRespCodeList *) &respCodeList;
	pchResponseCodeString[0] = _T('\0');


	CCmdList *ptrCmndList = (CCmdList *)m_pDevice->getListPtr(iT_Command);
	if (ptrCmndList)
	{
		hCcommand *pCommand = ptrCmndList->getCmdByNumber(iCommandNumber);
		if (pCommand)
		{
			pCommand->getRespCodes(*tmprespCodeList);
			for (hCRespCodeList::iterator iT = respCodeList.begin(); iT != respCodeList.end(); iT++)
			{
				//rspCode=(*iT);
				pRspCode = (hCrespCode*)&(*iT);
				
				//if( rspCode.getVal() == iResponseCode)
				if( pRspCode->getVal() == iResponseCode)
				{
					//wstring strtmp=rspCode.getDescStr().s; 
					wstring strtmp=pRspCode->getDescStr(); 
#ifdef MODIFIED_BY_SS
					//_tstrncpy(pchResponseCodeString,strtmp.c_str(),(iResponseCodeStringLength-1));
					strtmp.copy(pchResponseCodeString, iResponseCodeStringLength - 1);
#endif
					pchResponseCodeString[iResponseCodeStringLength-1] = _T('\0');
					break;
				}// else keep looking	
			}// next
			if (pchResponseCodeString[0] == _T('\0'))// not found
			{
				retVal = BI_ERROR;// bad response code value
			}
		}
		else
		{
			retVal = BI_ERROR;// bad command number
		}
	}
	else
	{
		retVal = BI_ERROR;// no commands in device - we really should bail out now :}
	}

	return retVal; 

}

int CHart_Builtins::display_response_status
		(
			long lCommandNumber
			, int  iStatusValue
		)
{
	ACTION_USER_INPUT_DATA structUserInput;
	ACTION_UI_DATA structUIData;
	varPtrList_t itemList;
	int nHandle = 0;		// Used for optimisation, 0 means resolve it otherwise use the existing resolved list.
	wstring str_desc;
	bool	bSetAbortFlag=false;

	responseCodeList_t respCodeList;

	//hCrespCode	rspCode(m_pDevice->devHndl());
	hCrespCode*     pRspCode = NULL;

	hCRespCodeList	*tmprespCodeList;// treat as a vector of <hCrespCode>

	tmprespCodeList = (hCRespCodeList *) &respCodeList;


	CCmdList *ptrCmndList = (CCmdList *)m_pDevice->getListPtr(iT_Command);
	if (ptrCmndList)
	{
		hCcommand *pCommand = ptrCmndList->getCmdByNumber(lCommandNumber);
		if (pCommand)
		{
			pCommand->getRespCodes(*tmprespCodeList);
			for (hCRespCodeList::iterator iT = respCodeList.begin(); iT != respCodeList.end(); iT++)
			{
				//rspCode=(*iT);
				pRspCode = (hCrespCode*)&(*iT);
				
				//if( rspCode.getVal() == iStatusValue)
				if( pRspCode->getVal() == iStatusValue)
				{
					int str_len;
					//str_desc=rspCode.getDescStr(); 
					str_desc=pRspCode->getDescStr();
					
					str_len=str_desc.length(); 
					structUIData.userInterfaceDataType=TEXT_MESSAGE;
					if(str_len>0)
					{
					structUIData.textMessage.pchTextMessage = new tchar[str_len + 1];
					structUIData.textMessage.iTextMessageLength = str_len;
#ifdef MODIFIED_BY_SS
					//_tstrcpy(structUIData.textMessage.pchTextMessage ,str_desc.c_str() );
					str_desc.copy(structUIData.textMessage.pchTextMessage, str_len );
#endif
					}
					
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
	
				}
	
			}
		}
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
}


//Added By Anil June 20 2005 --starts here

char* CHart_Builtins::svar_value(long lItemId)
{
	
	CValueVarient ppReturnedDataItem;

	if( Read(lItemId,ppReturnedDataItem,false) == BI_SUCCESS )
	{		
		string strTemp =ppReturnedDataItem;
		int nLen = strTemp.length();
		char* szReturnValue;
		nLen = nLen+1;
		
		szReturnValue = new _CHAR[nLen]; 
		memset(szReturnValue,0,sizeof(_CHAR) * nLen);
		memcpy(szReturnValue,strTemp.c_str(),nLen);
		return szReturnValue;
	
	}
	else
	{
		return NULL;
	}	

	
}

int CHart_Builtins::sassign(long lItemId, char* new_value)
{
	hCitemBase* pIB = NULL;

	string S(new_value);// narrow string	

	if(m_pDevice->getItemBySymNumber(lItemId,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarDest = (hCVar*)pIB;
		CValueVarient tempValue; tempValue = S;

		int varType = vT_unused;
		varType = pVarDest->VariableType();

		if(varType == vT_Ascii || varType == vT_PackedAscii || varType == vT_Password)
		{
			pVarDest->setRawDispValue(tempValue);			//Set the raw value.Handles Locals
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                            //                                    dynamic comands
			return BI_SUCCESS;
		}
	}
	return BI_ERROR;
}

/*Vibhor 200905: Start of Code*/


int CHart_Builtins::_ListInsert(long lListId, int iIndex, long lItemId)
{
	hCitemBase *pIB = NULL;
	int iRetCode = SUCCESS;
	
	if((0 == lListId) || (-1 == iIndex) || (0 == lItemId))
		return BI_ERROR;
	/*Check if List exists & is Valid*/
	if((SUCCESS == m_pDevice->getItemBySymNumber(lListId,&pIB))
		&& (NULL != pIB)
		&& (pIB->getIType() == iT_List)
		&& (pIB->IsValidTest())
		)
	{
		hClist *pList = (hClist*)pIB;

		pIB = NULL;

		/*Now check if item to be inserted exists and is Valid*/
		
		if((SUCCESS == m_pDevice->getItemBySymNumber(lItemId,&pIB))
			&& (NULL != pIB)
			&& (pIB->IsValidTest())
			)
		{
			/*Rest of the sanity checking is done in the hClist::insert itself
			  So just call and return accordingly*/
			iRetCode = pList->insert(pIB,iIndex);

			if(SUCCESS != iRetCode)
				return BI_ERROR;
		}
		else
			return BI_ERROR;
	}
	else
		return BI_ERROR;

	return BI_SUCCESS;
	
}/*End _ListInsert()*/


int CHart_Builtins::_ListDeleteElementAt(long lListId, int iIndex)
{
	hCitemBase *pIB = NULL;
	int iRetCode = SUCCESS;

	if((0 == lListId)  || (-1 == iIndex))
		return BI_ERROR;
	/*Check is List exists & is Valid*/
	if((SUCCESS == m_pDevice->getItemBySymNumber(lListId,&pIB))
		&& (NULL != pIB)
		&& (pIB->getIType() == iT_List)
		&& (pIB->IsValidTest())
		)
	{
		hClist *pList = (hClist*)pIB;
		/*Just calll the hClist::Remove() and return accordingly*/

		iRetCode = pList->remove(iIndex);

		if(SUCCESS != iRetCode)
				return BI_ERROR;
	}
	else
		return BI_ERROR;

	return BI_SUCCESS;

}/*End _ListDeleteElementAt()*/



/*Vibhor 200905: End of Code*/

//Added By Anil June 27 2005 --starts here
void CHart_Builtins::MapBuiltinFunNameToEnum()
{
	
	m_MapBuiltinFunNameToEnum["delay"]									= BUILTIN_delay;

	m_MapBuiltinFunNameToEnum["DELAY"]									= BUILTIN_DELAY;

	m_MapBuiltinFunNameToEnum["DELAY_TIME"]								= BUILTIN_DELAY_TIME;

	m_MapBuiltinFunNameToEnum["BUILD_MESSAGE"]							= BUILTIN_BUILD_MESSAGE;

	m_MapBuiltinFunNameToEnum["PUT_MESSAGE"]							= BUILTIN_PUT_MESSAGE;

	m_MapBuiltinFunNameToEnum["put_message"]							= BUILTIN_put_message;

	m_MapBuiltinFunNameToEnum["ACKNOWLEDGE"]							= BUILTIN_ACKNOWLEDGE;

	m_MapBuiltinFunNameToEnum["acknowledge"]							= BUILTIN_acknowledge;

	m_MapBuiltinFunNameToEnum["_get_dev_var_value"]						= BUILTIN__get_dev_var_value;

	m_MapBuiltinFunNameToEnum["_get_local_var_value"]					= BUILTIN__get_local_var_value;

	m_MapBuiltinFunNameToEnum["_display_xmtr_status"]					= BUILTIN__display_xmtr_status;

	m_MapBuiltinFunNameToEnum["display_response_status"]				= BUILTIN_display_response_status;

	m_MapBuiltinFunNameToEnum["display"]								= BUILTIN_display;

	m_MapBuiltinFunNameToEnum["SELECT_FROM_LIST"]						= BUILTIN_SELECT_FROM_LIST;

	m_MapBuiltinFunNameToEnum["select_from_list"]						= BUILTIN_select_from_list;

	m_MapBuiltinFunNameToEnum["_vassign"]								= BUILTIN__vassign;

	m_MapBuiltinFunNameToEnum["_dassign"]								= BUILTIN__dassign;

	m_MapBuiltinFunNameToEnum["_fassign"]								= BUILTIN__fassign;

	m_MapBuiltinFunNameToEnum["_lassign"]								= BUILTIN__lassign;

	m_MapBuiltinFunNameToEnum["_iassign"]								= BUILTIN__iassign;

	m_MapBuiltinFunNameToEnum["_fvar_value"]							= BUILTIN__fvar_value;

	m_MapBuiltinFunNameToEnum["_ivar_value"]							= BUILTIN__ivar_value;

	m_MapBuiltinFunNameToEnum["_lvar_value"]							= BUILTIN__lvar_value;

	m_MapBuiltinFunNameToEnum["svar_value"]								= BUILTIN_svar_value;

	m_MapBuiltinFunNameToEnum["sassign"]								= BUILTIN_sassign;

	m_MapBuiltinFunNameToEnum["save_values"]							= BUILTIN_save_values;	

	m_MapBuiltinFunNameToEnum["get_more_status"]						= BUILTIN_get_more_status;

	m_MapBuiltinFunNameToEnum["_get_status_code_string"]				= BUILTIN__get_status_code_string;

	// made it into _get.. 25jul07 -stevev- methods.h added METHODID() and the underbar
	m_MapBuiltinFunNameToEnum["_get_enum_string"]						= BUILTIN_get_enum_string;

	m_MapBuiltinFunNameToEnum["_get_dictionary_string"]					= BUILTIN__get_dictionary_string;

	//Anil 22 December 2005 for dictionary_string built in
	m_MapBuiltinFunNameToEnum["_dictionary_string"]					    = BUILTIN__dictionary_string;

	m_MapBuiltinFunNameToEnum["resolve_array_ref"]						= BUILTIN_resolve_array_ref;

	m_MapBuiltinFunNameToEnum["resolve_record_ref"]						= BUILTIN_resolve_record_ref;

	m_MapBuiltinFunNameToEnum["resolve_param_ref"]						= BUILTIN_resolve_param_ref;

	m_MapBuiltinFunNameToEnum["resolve_local_ref"]						= BUILTIN_resolve_local_ref;

	m_MapBuiltinFunNameToEnum["rspcode_string"]							= BUILTIN_rspcode_string;

	m_MapBuiltinFunNameToEnum["_set_comm_status"]						= BUILTIN__set_comm_status;

	m_MapBuiltinFunNameToEnum["_set_device_status"]						= BUILTIN__set_device_status;

	m_MapBuiltinFunNameToEnum["_set_resp_code"]							= BUILTIN__set_resp_code;

	m_MapBuiltinFunNameToEnum["_set_all_resp_code"]						= BUILTIN__set_all_resp_code;

	m_MapBuiltinFunNameToEnum["_set_no_device"]							= BUILTIN__set_no_device;

	m_MapBuiltinFunNameToEnum["SET_NUMBER_OF_RETRIES"]					= BUILTIN_SET_NUMBER_OF_RETRIES;

	m_MapBuiltinFunNameToEnum["_set_xmtr_comm_status"]					= BUILTIN__set_xmtr_comm_status;

	m_MapBuiltinFunNameToEnum["_set_xmtr_device_status"]				= BUILTIN__set_xmtr_device_status;

	m_MapBuiltinFunNameToEnum["_set_xmtr_resp_code"]					= BUILTIN__set_xmtr_resp_code;

	m_MapBuiltinFunNameToEnum["_set_xmtr_all_resp_code"]				= BUILTIN__set_xmtr_all_resp_code;

	m_MapBuiltinFunNameToEnum["_set_xmtr_no_device"]					= BUILTIN__set_xmtr_no_device;

	m_MapBuiltinFunNameToEnum["_set_xmtr_all_data"]						= BUILTIN__set_xmtr_all_data;

	m_MapBuiltinFunNameToEnum["_set_xmtr_data"]							= BUILTIN__set_xmtr_data;

	m_MapBuiltinFunNameToEnum["abort"]									= BUILTIN_abort;
																		
	m_MapBuiltinFunNameToEnum["process_abort"]							= BUILTIN_process_abort;
																		
	m_MapBuiltinFunNameToEnum["_add_abort_method"]						= BUILTIN__add_abort_method;
																		
	m_MapBuiltinFunNameToEnum["_remove_abort_method"]					= BUILTIN__remove_abort_method;
																		
	m_MapBuiltinFunNameToEnum["remove_all_abort"]						= BUILTIN_remove_all_abort;
																		
	m_MapBuiltinFunNameToEnum["_push_abort_method"]						= BUILTIN_push_abort_method;/*stevev4waltS 11oct07 - match methods.h*/
																		
	m_MapBuiltinFunNameToEnum["pop_abort_method"]						= BUILTIN_pop_abort_method;
																		
	m_MapBuiltinFunNameToEnum["NaN_value"]								= BUILTIN_NaN_value;
																	
	m_MapBuiltinFunNameToEnum["isetval"]								= BUILTIN_isetval;
																	
	m_MapBuiltinFunNameToEnum["lsetval"]								= BUILTIN_lsetval;
																	
	m_MapBuiltinFunNameToEnum["fsetval"]								= BUILTIN_fsetval;
																	
	m_MapBuiltinFunNameToEnum["dsetval"]								= BUILTIN_dsetval;
																	
	m_MapBuiltinFunNameToEnum["igetvalue"]								= BUILTIN_igetvalue;
																	
	m_MapBuiltinFunNameToEnum["igetval"]								= BUILTIN_igetval;
																	
	m_MapBuiltinFunNameToEnum["lgetval"]								= BUILTIN_lgetval;
																	
	m_MapBuiltinFunNameToEnum["fgetval"]								= BUILTIN_fgetval;
																	
	m_MapBuiltinFunNameToEnum["dgetval"]								= BUILTIN_dgetval;
																	
	m_MapBuiltinFunNameToEnum["sgetval"]								= BUILTIN_sgetval;
																	
	m_MapBuiltinFunNameToEnum["ssetval"]								= BUILTIN_ssetval;
																	
	m_MapBuiltinFunNameToEnum["send"]									= BUILTIN_send;
																	
	m_MapBuiltinFunNameToEnum["send_command"]							= BUILTIN_send_command;
																	
	m_MapBuiltinFunNameToEnum["send_command_trans"]						= BUILTIN_send_command_trans;
																	
	m_MapBuiltinFunNameToEnum["send_trans"]								= BUILTIN_send_trans;
																	
	m_MapBuiltinFunNameToEnum["ext_send_command"]						= BUILTIN_ext_send_command;
																	
	m_MapBuiltinFunNameToEnum["ext_send_command_trans"]					= BUILTIN_ext_send_command_trans;
																	
	m_MapBuiltinFunNameToEnum["tsend_command"]							= BUILTIN_tsend_command;
																	
	m_MapBuiltinFunNameToEnum["tsend_command_trans"]					= BUILTIN_tsend_command_trans;
																		
	m_MapBuiltinFunNameToEnum["abs"]									= BUILTIN_abs;
																		
	m_MapBuiltinFunNameToEnum["acos"]									= BUILTIN_acos;
																		
	m_MapBuiltinFunNameToEnum["asin"]									= BUILTIN_asin;
																		
	m_MapBuiltinFunNameToEnum["atan"]									= BUILTIN_atan;
																		
	m_MapBuiltinFunNameToEnum["cbrt"]									= BUILTIN_cbrt;
																		
	m_MapBuiltinFunNameToEnum["ceil"]									= BUILTIN_ceil;
																		
	m_MapBuiltinFunNameToEnum["cos"]									= BUILTIN_cos;
																		
	m_MapBuiltinFunNameToEnum["cosh"]									= BUILTIN_cosh;
																		
	m_MapBuiltinFunNameToEnum["exp"]									= BUILTIN_exp;
																		
	m_MapBuiltinFunNameToEnum["floor"]									= BUILTIN_floor;
																		
	m_MapBuiltinFunNameToEnum["fmod"]									= BUILTIN_fmod;
#ifdef XMTR								
	m_MapBuiltinFunNameToEnum["frand"]									= BUILTIN_frand;
#endif
																		
	m_MapBuiltinFunNameToEnum["log"]									= BUILTIN_log;
																		
	m_MapBuiltinFunNameToEnum["log10"]									= BUILTIN_log10;
																		
	m_MapBuiltinFunNameToEnum["log2"]									= BUILTIN_log2;
																		
	m_MapBuiltinFunNameToEnum["pow"]									= BUILTIN_pow;
																		
	m_MapBuiltinFunNameToEnum["round"]									= BUILTIN_round;
																		
	m_MapBuiltinFunNameToEnum["sin"]									= BUILTIN_sin;
																		
	m_MapBuiltinFunNameToEnum["sinh"]									= BUILTIN_sinh;
																		
	m_MapBuiltinFunNameToEnum["sqrt"]									= BUILTIN_sqrt;
																		
	m_MapBuiltinFunNameToEnum["tan"]									= BUILTIN_tan;
																		
	m_MapBuiltinFunNameToEnum["tanh"]									= BUILTIN_tanh;
																		
	m_MapBuiltinFunNameToEnum["trunc"]									= BUILTIN_trunc;
																		
	m_MapBuiltinFunNameToEnum["atof"]									= BUILTIN_atof;
																		
	m_MapBuiltinFunNameToEnum["atoi"]									= BUILTIN_atoi;
																		
	m_MapBuiltinFunNameToEnum["itoa"]									= BUILTIN_itoa;
																		
//	m_MapBuiltinFunNameToEnum["YearMonthDay_to_Date"]					= BUILTIN_YearMonthDay_to_Date;//WS:EPM Not a builtin 25jun07
																		
	m_MapBuiltinFunNameToEnum["Date_to_Year"]							= BUILTIN_Date_to_Year;
																
	m_MapBuiltinFunNameToEnum["Date_to_Month"]							= BUILTIN_Date_to_Month;
																
	m_MapBuiltinFunNameToEnum["Date_to_DayOfMonth"]						= BUILTIN_Date_to_DayOfMonth;
																
//	m_MapBuiltinFunNameToEnum["GetCurrentDate"]							= BUILTIN_GetCurrentDate;//WS:EPM Not a builtin 25jun07
																
	m_MapBuiltinFunNameToEnum["GetCurrentTime"]							= BUILTIN_GetCurrentTime;
																
//	m_MapBuiltinFunNameToEnum["GetCurrentDateAndTime"]					= BUILTIN_GetCurrentDateAndTime;//WS:EPM Not a builtin 25jun07
																
//	m_MapBuiltinFunNameToEnum["To_Date_and_Time"]						= BUILTIN_To_Date_and_Time;//WS:EPM Not a builtin 25jun07
																		
	m_MapBuiltinFunNameToEnum["strstr"]									= BUILTIN_strstr;
																		
	m_MapBuiltinFunNameToEnum["strupr"]									= BUILTIN_strupr;
																		
	m_MapBuiltinFunNameToEnum["strlwr"]									= BUILTIN_strlwr;
																		
	m_MapBuiltinFunNameToEnum["strlen"]									= BUILTIN_strlen;
																		
	m_MapBuiltinFunNameToEnum["strcmp"]									= BUILTIN_strcmp;
																		
	m_MapBuiltinFunNameToEnum["strtrim"]								= BUILTIN_strtrim;
																		
	m_MapBuiltinFunNameToEnum["strmid"]									= BUILTIN_strmid;

	//Added By Anil July 01 2005 --starts here
	m_MapBuiltinFunNameToEnum["discard_on_exit"]						= BUILTIN_discard_on_exit;
	//Added By Anil July 01 2005 --Ends here

//Vibhor 200905: Added
	m_MapBuiltinFunNameToEnum["_ListInsert"]							= BUILTIN__ListInsert;			//Vibhor 130705: Added

	m_MapBuiltinFunNameToEnum["_ListDeleteElementAt"]					= BUILTIN__ListDeleteElementAt;	//Vibhor 130705: Added

	m_MapBuiltinFunNameToEnum["_MenuDisplay"]							= BUILTIN__MenuDisplay;	//Anil September 26 2005 added MenuDisplay

	m_MapBuiltinFunNameToEnum["remove_all_abort_methods"]				= BUILTIN_remove_all_abort_methods ;

	m_MapBuiltinFunNameToEnum["DiffTime"]								= BUILTIN_DiffTime  ;

	m_MapBuiltinFunNameToEnum["AddTime"]								= BUILTIN_AddTime  ;

	m_MapBuiltinFunNameToEnum["Make_Time"]								= BUILTIN_Make_Time  ;

	m_MapBuiltinFunNameToEnum["To_Time"]								= BUILTIN_To_Time  ;

	m_MapBuiltinFunNameToEnum["Date_To_Time"]							= BUILTIN_Date_To_Time  ;

	m_MapBuiltinFunNameToEnum["To_Date"]								= BUILTIN_To_Date  ;

	m_MapBuiltinFunNameToEnum["Time_To_Date"]							= BUILTIN_Time_To_Date  ;

	// added 16jul14------------------------------------------------------------------------------
	m_MapBuiltinFunNameToEnum["DATE_to_days"]                           = BUILTIN_DATE_to_days  ;

	m_MapBuiltinFunNameToEnum["days_to_DATE"]							= BUILTIN_days_to_DATE  ;

	m_MapBuiltinFunNameToEnum["From_DATE_AND_TIME_VALUE"]				= BUILTIN_From_DATE_AND_TIME_VALUE  ;

	m_MapBuiltinFunNameToEnum["From_TIME_VALUE"]						= BUILTIN_From_TIME_VALUE  ;

	m_MapBuiltinFunNameToEnum["TIME_VALUE_to_seconds"]					= BUILTIN_TIME_VALUE_to_seconds  ;

	m_MapBuiltinFunNameToEnum["TIME_VALUE_to_Hour"]						= BUILTIN_TIME_VALUE_to_Hour  ;

	m_MapBuiltinFunNameToEnum["TIME_VALUE_to_Minute"]					= BUILTIN_TIME_VALUE_to_Minute  ;

	m_MapBuiltinFunNameToEnum["TIME_VALUE_to_Second"]					= BUILTIN_TIME_VALUE_to_Second  ;

	m_MapBuiltinFunNameToEnum["seconds_to_TIME_VALUE"]					= BUILTIN_seconds_to_TIME_VALUE  ;

	m_MapBuiltinFunNameToEnum["DATE_AND_TIME_VALUE_to_string"]			= BUILTIN_DATE_AND_TIME_VALUE_to_string  ;

	m_MapBuiltinFunNameToEnum["DATE_to_string"]							= BUILTIN_DATE_to_string  ;

	m_MapBuiltinFunNameToEnum["TIME_VALUE_to_string"]					= BUILTIN_TIME_VALUE_to_string  ;

	m_MapBuiltinFunNameToEnum["timet_to_string"]						= BUILTIN_timet_to_string  ;

	m_MapBuiltinFunNameToEnum["timet_To_TIME_VALUE"]					= BUILTIN_timet_to_TIME_VALUE  ;

	m_MapBuiltinFunNameToEnum["To_TIME_VALUE"]							= BUILTIN_To_TIME_VALUE  ;
// end 16jul14 addition

	m_MapBuiltinFunNameToEnum["fpclassify"]								= BUILTIN_fpclassify  ;

	m_MapBuiltinFunNameToEnum["nanf"]									= BUILTIN_nanf  ;

	m_MapBuiltinFunNameToEnum["nan"]									= BUILTIN_nan  ;// stevev - added 25jun07
	//stevev 29jan08 to look up method's literal string
	m_MapBuiltinFunNameToEnum["literal_string"]							= BUILTIN_literal_string;
	// stevev 24nov08 - add block transfer functions
	m_MapBuiltinFunNameToEnum["openTransferPort"]						= BUILTIN_openTransferPort  ;

	m_MapBuiltinFunNameToEnum["closeTransferPort"]						= BUILTIN_closeTransferPort  ;

	m_MapBuiltinFunNameToEnum["abortTransferPort"]						= BUILTIN_abortTransferPort  ;

	m_MapBuiltinFunNameToEnum["_writeItemToDevice"]						= BUILTIN_writeItem2Port  ;

	m_MapBuiltinFunNameToEnum["_readItemFromDevice"]					= BUILTIN_readItemfromPort  ;

	m_MapBuiltinFunNameToEnum["get_transfer_status"]					= BUILTIN_getTransferStatus  ;
	// end transfer functions
// added 16jul14 - stevev ------------------------------------------------------------------------------
	m_MapBuiltinFunNameToEnum["_ERROR"]									= BUILTIN__ERROR  ;

	m_MapBuiltinFunNameToEnum["_TRACE"]									= BUILTIN__TRACE  ;

	m_MapBuiltinFunNameToEnum["_WARNING"]								= BUILTIN__WARNING  ;

}
//Added By Anil June 27 2005 --Ends here
