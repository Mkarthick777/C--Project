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
#include "Mee.h"
#include "messageUI.h"

int	CHart_Builtins:: SEND_COMMAND(
			
			int iCommandNumber
			, int iTransNumber
			, uchar *pchResponseStatus
			, uchar *pchMoreDataStatus
			, uchar *pchMoreDataInfo
			, int  iCmdType
			, bool bMoreDataFlag
			, int& iMoreInfoSize
			)
{
	int iRetVal = BI_SUCCESS; /*Return value of this function*/
	int iSendMethCmdRetCode = SUCCESS; /*Return value from the SendMethCmd for NORMAL_COMMAND*/
	int iSendMethCmdRetCode48 = FAILURE;/*Return value from the SendMethCmd for COMMAND_48*/
	bool bRetry=false; /*Main loop controlling var*/
	bool bRetryAsked=false;/*Flag set from the retry/abort masks for the method execution*/
	bool bMoreStatusAvailable=false;/*Flag to indicate there's more status available*/
	int  iXmtrDataCount=0;/*Byte count for Cmd 48 reply*/

	int iRetryCount = 0; /*Command Retry Counter*/

	int i=0;

	varPtrList_t itemList ;
	
	hCitemBase* pIB = NULL;
	CValueVarient ValueNeeded;
	hCVar *pLocalVar = NULL;

	//int iRc;
	/*Initialize the ResponseStatus array*/

	pchResponseStatus[STATUS_RESPONSE_CODE] = 0;
	pchResponseStatus[STATUS_COMM_STATUS]   = 0;
	pchResponseStatus[STATUS_DEVICE_STATUS] = 0;

	/*Initialize the MoreDataStatus (Cmd 48) array*/
/*Vibhor 120204: Start of Code*/

/*There was copy paste problem!! */

	pchMoreDataStatus[STATUS_RESPONSE_CODE] = 0;
	pchMoreDataStatus[STATUS_COMM_STATUS]   = 0;
	pchMoreDataStatus[STATUS_DEVICE_STATUS] = 0;

/*And this initialization was missing */

	for(i = 0 ; i < MAX_XMTR_STATUS_LEN ; i++)
	{
		pchMoreDataInfo[i] = 0;
	}

/*Vibhor 120204: End of Code*/
	do
	{
		bRetry = false;
		bRetryAsked = false;
		bMoreStatusAvailable = false;
		iRetVal = BI_SUCCESS;

		if(iCmdType == NORMAL_CMD)
		{
			iSendMethCmdRetCode = m_pDevice->sendMethodCmd(iCommandNumber,iTransNumber);

			if(iSendMethCmdRetCode & (/*DEVICE_NO_DISPATCH |*/ HOST_INTERNALERR))//As per Steve anil on 3 November 2005
			{	/*Command_Not_Implemented*/
				pchResponseStatus[STATUS_RESPONSE_CODE] = 0;
				pchResponseStatus[STATUS_COMM_STATUS]   = 0;
				pchResponseStatus[STATUS_DEVICE_STATUS] = 0;
				//Anil added to Fix Par 5573 , command not implemented  is send
				tchar pchString[1024]={0};
#ifdef MODIFIED_BY_SS
				std::wstring wstrTemp = M_METHOD_SEND_ERROR;
				//_tstrcpy(pchString,M_METHOD_SEND_ERROR);
				std::copy(wstrTemp.begin(), wstrTemp.end(), pchString);
#endif
				acknowledge(pchString, NULL, 0);	

			}/*Endif DEVICE_NO_COMMANDCLASS*/
			else
			{
				if(m_pDevice->getItemBySymNumber(DEVICE_RESPONSECODE, &pIB) == SUCCESS)
				{
					pLocalVar = (hCVar*)pIB;

/*Vibhor 270204: Start of Code*/
					ValueNeeded = pLocalVar->getRawDispValue();//was getDispValue();
/*Vibhor 270204: Start of Code*/
					
					pchResponseStatus[STATUS_RESPONSE_CODE] = (char)((int)ValueNeeded);
					
				}/*Endif DEVICE_RESPONSECODE*/

				if(m_pDevice->getItemBySymNumber(DEVICE_COMM_STATUS, &pIB) == SUCCESS)
				{
					pLocalVar = (hCVar*)pIB;
/*Vibhor 270204: Start of Code*/
					ValueNeeded = pLocalVar->getRawDispValue();//was getDispValue();
/*Vibhor 270204: Start of Code*/
					int itemp = (int)ValueNeeded;

					pchResponseStatus[STATUS_COMM_STATUS] = (char)((int)ValueNeeded);
					
				}/*Endif DEVICE_COMM_STATUS*/

				if(m_pDevice->getItemBySymNumber(DEVICE_COMM48_STATUS, &pIB) == SUCCESS)
				{
					pLocalVar = (hCVar*)pIB;

/*Vibhor 270204: Start of Code*/
					ValueNeeded = pLocalVar->getRawDispValue();//was getDispValue();
/*Vibhor 270204: Start of Code*/

					int itemp = (int)ValueNeeded;

					pchResponseStatus[STATUS_DEVICE_STATUS] = (char)((int)ValueNeeded);
					
				}/*Endif DEVICE_COMM48_STATUS*/

			}/* End else*/

		}/*Endif NORMAL_CMD*/
		/*
		 * else pretend we sent the command so the more status command
		 * can be issued
		 */
		else{
				iSendMethCmdRetCode = SUCCESS;

				pchResponseStatus[STATUS_RESPONSE_CODE] = 0;
				pchResponseStatus[STATUS_COMM_STATUS]   = 0;
				pchResponseStatus[STATUS_DEVICE_STATUS] = 0;

				bMoreDataFlag = true;
		}/*End else*/

		if((iSendMethCmdRetCode == SUCCESS))
		{
			
			if(bMoreDataFlag == true){
					if(iCmdType == NORMAL_CMD)
					{
						bMoreStatusAvailable = false;
						iSendMethCmdRetCode48 = SUCCESS;
					}/*Endif iCmdType*/

					if((iCmdType == MORE_STATUS_CMD)
						 || (pchResponseStatus[STATUS_DEVICE_STATUS] & DS_MORESTATUSAVAIL))
					{/*Send Command 48*/

						iSendMethCmdRetCode48 = 
									   m_pDevice->sendMethodCmd(48,DEFAULT_TRANSACTION_NUMBER);
						bMoreStatusAvailable = true;

						if(iSendMethCmdRetCode48 & (DEVICE_NO_DISPATCH | HOST_INTERNALERR))
						{	/*Command_Not_Implemented*/
							pchMoreDataStatus[STATUS_RESPONSE_CODE] = 0;
							pchMoreDataStatus[STATUS_COMM_STATUS]   = 0;
							pchMoreDataStatus[STATUS_DEVICE_STATUS] = 0;

						}/*Endif DEVICE_NO_COMMANDCLASS*/
						else
						{
							if(m_pDevice->getItemBySymNumber(DEVICE_RESPONSECODE, &pIB) == SUCCESS)
							{
								pLocalVar = (hCVar*)pIB;

/*Vibhor 270204: Start of Code*/
								ValueNeeded = pLocalVar->getRawDispValue();//was getDispValue();
/*Vibhor 270204: Start of Code*/
								
								pchMoreDataStatus[STATUS_RESPONSE_CODE] = (char)((int)ValueNeeded);
								
							}/*Endif DEVICE_RESPONSECODE*/

							if(m_pDevice->getItemBySymNumber(DEVICE_COMM_STATUS, &pIB) == SUCCESS)
							{
								pLocalVar = (hCVar*)pIB;

/*Vibhor 270204: Start of Code*/
								ValueNeeded = pLocalVar->getRawDispValue();//was getDispValue();
/*Vibhor 270204: Start of Code*/

								int itemp = (int)ValueNeeded;

								pchMoreDataStatus[STATUS_COMM_STATUS] = (char)((int)ValueNeeded);
								
							}/*Endif DEVICE_COMM_STATUS*/

							if(m_pDevice->getItemBySymNumber(DEVICE_COMM48_STATUS, &pIB) == SUCCESS)
							{
								pLocalVar = (hCVar*)pIB;

/*Vibhor 270204: Start of Code*/
								ValueNeeded = pLocalVar->getRawDispValue();//was getDispValue();
/*Vibhor 270204: Start of Code*/

								int itemp = (int)ValueNeeded;

								pchMoreDataStatus[STATUS_DEVICE_STATUS] = (char)((int)ValueNeeded);
								
							}/*Endif DEVICE_COMM48_STATUS*/

						}/* End else*/
								
					}/*Endif iCmdType == MORE_STATUS_CMD*/

					if(bMoreStatusAvailable == true)
					{
						if(iSendMethCmdRetCode48 == SUCCESS)
						{
							hCcommand *pCommand = NULL; // WS:EPM 24may07
							CCmdList *ptrCmndList = (CCmdList *)m_pDevice->getListPtr(iT_Command);
							// WS:EPM 24may07  hCcommand *pCommand = ptrCmndList->getCmdByNumber(48); 
							if (ptrCmndList ) 
                            { 
								pCommand = ptrCmndList->getCmdByNumber(48); 
                            } // end  WS:EPM 24may07


							if(pCommand)
							{
								
								hCtransaction *pTrans =	pCommand->getTransactionByNumber(DEFAULT_TRANSACTION_NUMBER);
								
								if(pTrans)
								{
/*Vibhor 120204: Start of Code*/
/*Fix for PAR 5326 : Earlier pTrans->GetReplyList() was being used, which only returned
the lits of valid var ptrs in the reply packet, but here we want the whole byte string
of the 	reply */
									hPkt tempPkt;
									tempPkt.clear();
									if(pTrans->generateReplyPkt(&tempPkt) == SUCCESS)
									{
										if(tempPkt.dataCnt > 2)
										{
											for( i= 2;((i < tempPkt.dataCnt) && (i < MAX_XMTR_STATUS_LEN)) ;i++)
											{
												pchMoreDataInfo[i-2] =	tempPkt.theData[i];

											}/*Endfor*/
											iMoreInfoSize  =
											iXmtrDataCount = tempPkt.dataCnt - 2;
										}

										else 
										{
											iXmtrDataCount = 0;
										}

									}/*Endif SUCCESS*/
/*Vibhor 120204: Start of Code*/

								}/*Endif pTrans*/


							}/*Endif pCommand*/

						}/*Endif iSendMethCmdRetCode48 == SUCCESS*/

						if(iSendMethCmdRetCode48 & HOST_NORESPONSE)
						{
							if(m_pMeth->m_byXmtrReturnNodevAbortMask){
								
								iRetVal = BI_ABORT;
							}
							else if(m_pMeth->m_byXmtrReturnNodevRetryMask){

								bRetryAsked = true;
							}
							else
							{ 
								iRetVal = BI_NO_DEVICE;
							}

						}/*Endif HOST_NORESPONSE*/


						else if(iSendMethCmdRetCode48 & HOST_PARITYERR 
												 & HOST_OVERUNERR 
												 & HOST_FRAMINGERR
												 & HOST_RXBUFFOVER 
												 & HOST_CHKSUMERR 
												 & HOST_HARTFRAMEERR)
						{
							if(m_pMeth->m_byXmtrCommAbortMask & pchMoreDataStatus[STATUS_COMM_STATUS]){

								iRetVal = BI_ABORT;
							}
							else if(m_pMeth->m_byXmtrCommRetryMask & pchMoreDataStatus[STATUS_COMM_STATUS]){
								
								bRetryAsked = true;
							}
							else
							{
								iRetVal = BI_COMM_ERR;
							}
	

						}/*Endif HOST_COMM_ERROR*/

						else
						{/*For all other conditions  Request a retry 
						  * and set iSendMethCmdRetCode48 = SUCCESS
						  */
							if (iSendMethCmdRetCode48 != SUCCESS)
							{
								bRetryAsked = true;
								iSendMethCmdRetCode48 = SUCCESS;
							}
							else
							{
								bRetryAsked = false;
							}							
						}
							

					}/*Endif bMoreStatus == true*/
		
				}/*Endif bMoreDataFlag*/

				
		}/*Endif iSendMethCmdRetCode == SUCCESS*/

				if(iSendMethCmdRetCode & HOST_NORESPONSE)
				{

					if(m_pMeth->m_byReturnNodevAbortMask){
						
						iRetVal = BI_ABORT;
					}
					else if(m_pMeth->m_byReturnNodevRetryMask){
						bRetryAsked = true;
					}
					else
					{ 
						iRetVal = BI_NO_DEVICE;
					}
				}/*Endif HOST_NORESPONSE*/


		else if(iSendMethCmdRetCode & HOST_PARITYERR 
									   & HOST_OVERUNERR 
									   & HOST_FRAMINGERR
									   & HOST_RXBUFFOVER 
									   & HOST_CHKSUMERR 
									   & HOST_HARTFRAMEERR)
				{
					if(m_pMeth->m_byCommAbortMask & pchResponseStatus[STATUS_COMM_STATUS]){
							iRetVal = BI_ABORT;
					}
					else if(m_pMeth->m_byCommRetryMask & pchResponseStatus[STATUS_COMM_STATUS]){
						
						bRetryAsked = true;
					}
					else
					{
						iRetVal = BI_COMM_ERR;
					}
				}/*Endif HOST_COMM_ERROR*/

				else
				{/*For all other conditions  Request a retry */
					if (iSendMethCmdRetCode != SUCCESS)
					{					
						bRetryAsked = true;
					}
					else
					{					
						bRetryAsked = false;
					}
				}
		/*If both the commands went fine or only one was requested*/

		if(iSendMethCmdRetCode == SUCCESS && ((bMoreDataFlag == false) || (iSendMethCmdRetCode48 == SUCCESS)))
		{
			if((iRetVal == BI_SUCCESS)
				&&((m_pMeth->m_byRespAbortMask[BYTENUM(pchResponseStatus[STATUS_RESPONSE_CODE])]
				   & (1 << BITNUM(pchResponseStatus[STATUS_RESPONSE_CODE])))
				   ||(pchResponseStatus[STATUS_COMM_STATUS] 
				        & m_pMeth->m_byCommAbortMask)
				   ||(pchResponseStatus[STATUS_DEVICE_STATUS]
					 & m_pMeth->m_byStatusAbortMask))){

				/*<START>Added by ANOOP to acknowledge the user that the current execution of method is aborted due to response code or device status */
				tchar pchString[1024]={0};
#ifdef MODIFIED_BY_SS
				std::wstring wstrTemp = M_METHOD_SEND_ERROR;
				//_tstrcpy(pchString,M_METHOD_SEND_ERROR);
				std::copy(wstrTemp.begin(), wstrTemp.end(), pchString);
#endif
				acknowledge(pchString, NULL, 0);	
				/*<END>Added by ANOOP to acknowledge the user that the current execution of method is aborted due to response code or device status */
				iRetVal = BI_ABORT;
			}

			if(bMoreStatusAvailable == true)
			{
				if((iRetVal == BI_SUCCESS)
					&&((m_pMeth->m_byXmtrRespAbortMask[BYTENUM(pchMoreDataStatus[STATUS_RESPONSE_CODE])]
					   & (1 << BITNUM(pchMoreDataStatus[STATUS_RESPONSE_CODE])))
					   ||(pchMoreDataStatus[STATUS_COMM_STATUS] 
							& m_pMeth->m_byXmtrCommAbortMask)
					   ||(pchMoreDataStatus[STATUS_DEVICE_STATUS]
					     & m_pMeth->m_byXmtrStatusAbortMask))){

					iRetVal = BI_ABORT;
				}

				if(iRetVal == BI_SUCCESS)
				{
					for(i = 0;i < iXmtrDataCount; i++)
					{
						if( pchMoreDataInfo[i] & m_pMeth->m_byXmtrDataAbortMask[i])
						{
							iRetVal = BI_ABORT;
							break;
						}
					}

				}


			}/*Endif bMoreDataAvailable*/

			if((iRetVal == BI_SUCCESS)
				&&((m_pMeth->m_byRespRetryMask[BYTENUM(pchResponseStatus[STATUS_RESPONSE_CODE])]
				   & (1 << BITNUM(pchResponseStatus[STATUS_RESPONSE_CODE])))
				   ||(pchResponseStatus[STATUS_COMM_STATUS] 
				        & m_pMeth->m_byCommRetryMask)
				   ||(pchResponseStatus[STATUS_DEVICE_STATUS]
					 & m_pMeth->m_byStatusRetryMask))){
						
					bRetryAsked = true;
			}

			if(bMoreStatusAvailable == true)
			{

				if((iRetVal == BI_SUCCESS)
					&&((m_pMeth->m_byXmtrRespRetryMask[BYTENUM(pchMoreDataStatus[STATUS_RESPONSE_CODE])]
					   & (1 << BITNUM(pchMoreDataStatus[STATUS_RESPONSE_CODE])))
					   ||(pchMoreDataStatus[STATUS_COMM_STATUS] 
							& m_pMeth->m_byXmtrCommRetryMask)
					   ||(pchMoreDataStatus[STATUS_DEVICE_STATUS]
					     & m_pMeth->m_byXmtrStatusRetryMask))){

					bRetryAsked = true;
				}

				if(iRetVal == BI_SUCCESS)
				{
					for(i = 0; i <iXmtrDataCount; i++)
					{
						if( pchMoreDataInfo[i] & m_pMeth->m_byXmtrDataRetryMask[i])
						{
							bRetryAsked = true;
							break;
						}
					}

				}

			}/*Endif bMoreDataAvailable*/
		
		}/*Endif iSendMethCmdRetCode == SUCCESS && ....*/

		if(bRetryAsked == true)
		{
			if(iRetryCount < m_pMeth->m_iAutoRetryLimit)
			{
				iRetryCount++;
				bRetry = true;
			}
			else
			{
				iRetVal = BI_ABORT;
			}
		}

	}while (bRetry == true);

	return iRetVal;

}/*End SEND_COMMAND*/



int CHart_Builtins::send(int cmd_number,uchar *cmd_status)
{
	uchar more_status[STATUS_SIZE];
	uchar more_status_data[MAX_XMTR_STATUS_LEN];
	int  info_size = 0;
	return SEND_COMMAND(cmd_number,DEFAULT_TRANSACTION_NUMBER,cmd_status,more_status,more_status_data,
																				NORMAL_CMD,false,info_size);

}




int CHart_Builtins::send_command(int cmd_number)
{
	uchar cmd_status[STATUS_SIZE];
	uchar more_status[STATUS_SIZE];
	uchar more_status_data[MAX_XMTR_STATUS_LEN];
	int  info_size;
	
	return SEND_COMMAND(cmd_number,DEFAULT_TRANSACTION_NUMBER,cmd_status,more_status,more_status_data,
																					NORMAL_CMD,true,info_size);
	

}


int CHart_Builtins::send_command_trans(int cmd_number,int iTransNumber)
{
	uchar cmd_status[STATUS_SIZE];
	uchar more_status[STATUS_SIZE];
	uchar more_status_data[MAX_XMTR_STATUS_LEN];
	int  info_size;
	
	//return SEND_COMMAND(cmd_number,iTransNumber,cmd_status,more_status,more_status_data,NORMAL_CMD,true);
	return SEND_COMMAND(cmd_number,iTransNumber,cmd_status,more_status,more_status_data,
																   NORMAL_CMD,false,info_size);
}


int CHart_Builtins::send_trans(int cmd_number,int iTransNumber,uchar *pchResponseStatus)
{
	uchar more_status[STATUS_SIZE];
	uchar more_status_data[MAX_XMTR_STATUS_LEN];
	int  info_size;
	
	//return SEND_COMMAND(cmd_number,iTransNumber,pchResponseStatus,more_status,more_status_data,NORMAL_CMD,false);
	return SEND_COMMAND(cmd_number,iTransNumber,pchResponseStatus,more_status,more_status_data,
		                                                             NORMAL_CMD,true,info_size);

}

int CHart_Builtins::ext_send_command(int cmd_number	, uchar *pchResponseStatus, 
								uchar *pchMoreDataStatus, uchar *pchMoreDataInfo, int& moreInfoSize)
{

	return SEND_COMMAND(cmd_number,DEFAULT_TRANSACTION_NUMBER,pchResponseStatus,pchMoreDataStatus,
		                                            pchMoreDataInfo,NORMAL_CMD,true,moreInfoSize);
	
}

int CHart_Builtins::ext_send_command_trans
		(
			int iCommandNumber
			, int iTransNumber
			, uchar *pchResponseStatus
			, uchar *pchMoreDataStatus
			, uchar *pchMoreDataInfo
			, int&  moreInfoSize
		)
{

	return SEND_COMMAND(iCommandNumber,DEFAULT_TRANSACTION_NUMBER,pchResponseStatus,
							pchMoreDataStatus,pchMoreDataInfo,NORMAL_CMD,true,moreInfoSize);
	
}

int CHart_Builtins::tsend_command
		(
			int iCommandNumber
		)
{

	uchar cmd_status[STATUS_SIZE];
	uchar more_status[STATUS_SIZE];
	uchar more_status_data[MAX_XMTR_STATUS_LEN];
	int  info_size;
	
	return SEND_COMMAND(iCommandNumber,DEFAULT_TRANSACTION_NUMBER,cmd_status,more_status,
													more_status_data,NORMAL_CMD,false,info_size);
	

}

int CHart_Builtins::tsend_command_trans
		(
			int iCommandNumber
			, int iTransNumber
		)
{
	uchar cmd_status[STATUS_SIZE];
	uchar more_status[STATUS_SIZE];
	uchar more_status_data[MAX_XMTR_STATUS_LEN];
	int  info_size;
	
	return SEND_COMMAND(iCommandNumber,iTransNumber,cmd_status,more_status,more_status_data,
																	NORMAL_CMD,false,info_size);
}


int CHart_Builtins::get_more_status(uchar *more_data_status,uchar *more_data_info, int& moreInfoSize)
{
	
	uchar cmd_status[STATUS_SIZE];

	return SEND_COMMAND(0,DEFAULT_TRANSACTION_NUMBER,cmd_status,more_data_status,more_data_info,
																MORE_STATUS_CMD,true,moreInfoSize);
	
}
