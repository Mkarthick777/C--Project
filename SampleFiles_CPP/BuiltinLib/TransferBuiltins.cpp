#include "pch.h"

////#include "stdafx.h"
////#include <windows.h>
#include "ddbGeneral.h"
////#include <ALPHAOPS.H>
#include "Char.h"
#include "typedefs.h"
//#include <limits.h>
//#include <float.h>
#include "ddbdevice.h"
//#include "Interpreter.h"
#include "Hart_Builtins.h"
//#include "Delay_Builtin.h"
//#include "MethodInterfaceDefs.h"
//#include "MEE.h"
#include "ddbTransfer.h"
#include "BI_Codes.h"

#pragma warning (disable : 4786) 

// Actual max segment length is 235, it may change to verify functionality
#define MAX_SEG_LEN 230

#define STARTING_MASTER_SYNC  0

int   CHart_Builtins :: openPort(int iportNumber)
{
	RETURNCODE  rc = FAILURE;
	hCcmdDispatcher* pCmdDispatch = m_pDevice->pCmdDispatch;

	hCTransferChannel* pChannel = pCmdDispatch->getPort( (iportNumber & 0xff) );
		// we do not own pChannel memory - do not delete
	if (pChannel == NULL)
	{
		LOGIT(CERR_LOG|UI_LOG,"Memory error: port %d could not be opened\n",iportNumber);
		return BI_ERROR;
	}
	else
	if (pChannel->Session_State != ss_Undefined && pChannel->Session_State != ss_Closed)
	{
		LOGIT(CERR_LOG|UI_LOG,"Error: port %d already opened\n",iportNumber);
		return BI_PORT_IN_USE;
	}
	else
	{
		assert(pChannel->portNumber == iportNumber);

		pChannel->Session_State = ss_Opening;

		pChannel->max_segment_len = MAX_SEG_LEN;
		pChannel->masterSync = pChannel->startMasterSync = (int)STARTING_MASTER_SYNC;
		pChannel->maserSyncRolloverCnt = 0;

		if ( (rc = pChannel->setupCmd111(tf_OpenPort) )  != BI_SUCCESS )
			return rc;

		if ( (rc = send_command( 111 )) != BI_SUCCESS )////////////////////////////////////
		{// exit on command error
			pChannel->Session_State = ss_Closed;
			return rc;
		}	///////////////////////////////////////////////////////////////////////////////
		
		//  fill in port info;
		pChannel->fillPortInfo(111);

		if ( pChannel->respCd != 0 && pChannel->respCd != 8 )
		{
			pChannel->Session_State = ss_Closed;
			rc = BI_ERROR;
			if ( pChannel->respCd == 10 )
			{
				rc = BI_PORT_IN_USE;
			}
			return rc;
		}
		
		if ( pChannel->funcCd != (int)tf_OpenPort )
		{
			pChannel->Session_State = ss_Closed;
			return BI_ERROR;
		}
		
		if (pChannel->max_segment_len > MAX_SEG_LEN )
		{// must be <= master_max_seg_len
			LOGIT(CERR_LOG|UI_LOG,"Device Error: Device segment length too big.\n");
			pChannel->max_segment_len = MAX_SEG_LEN;
		}
		//else leave it lay
		
		if (pChannel->masterSync != STARTING_MASTER_SYNC)
		{
			LOGIT(CERR_LOG|UI_LOG,"Error:Cmd 111 did not use the sent master sync.\n");
			closePort(iportNumber);
			return BI_ERROR;
		}// else, leave it
		pChannel->Session_State = ss_Opened;

		return BI_SUCCESS;
	}
	return BI_ERROR;
}

int   CHart_Builtins :: closePort(int iportNumber)
{
	RETURNCODE  rc = FAILURE;
	hCcmdDispatcher* pCmdDispatch = m_pDevice->pCmdDispatch;

	hCTransferChannel* pChannel = pCmdDispatch->getPort( (iportNumber & 0xff) );
		// we do not own pChannel memory - do not delete
	if (pChannel == NULL)
	{
		LOGIT(CERR_LOG|UI_LOG,"Memory error: port %d could not be opened\n",iportNumber);
		return BI_ERROR;
	}
	else
	if (pChannel->Session_State == ss_Undefined && pChannel->Session_State == ss_Closed)
	{
		LOGIT(CERR_LOG|UI_LOG,"Error: port %d not opened\n",iportNumber);
		return BI_ERROR;
	}
	else//opened,opening,closing
	{
		assert(pChannel->portNumber == iportNumber);

		sessionState_t hldSt = pChannel->Session_State;
		pChannel->Session_State = ss_Closing;

		if ( (rc = pChannel->setupCmd111(tf_Ready2Close) )  != BI_SUCCESS )
		{
			pChannel->Session_State = hldSt;
			return rc;
		}

		if ( (rc = send_command( 111 )) != BI_SUCCESS )////////////////////////////////////
		{// exit on command error
			pChannel->Session_State = hldSt;
			return rc;
		}	///////////////////////////////////////////////////////////////////////////////
		
		//  fill in port info;
		//should be set by the send_command...pChannel->fillPortInfo(111);

	//  reply of 'Port Closed' is success..BI_SUCCESS
		if (pChannel->funcCd == tf_PortClosed)
		{
			pChannel->Session_State = ss_Closed;
			return BI_SUCCESS;
		}
		else
	// if reply function is 'Transfer Data' there is more for the stack to do..BI_PORT_IN_USE
		if(pChannel->funcCd == tf_Transfer)
		{
			pChannel->Session_State = hldSt;
			return BI_PORT_IN_USE;
		}
		else
	// everything else is BI_ERROR
		{
			pChannel->Session_State = hldSt;
			return BI_ERROR;
		}
	}
	return BI_ERROR;//never reached.
}

int   CHart_Builtins :: abortPort(int iportNumber)
{
	RETURNCODE  rc = FAILURE;
	hCcmdDispatcher* pCmdDispatch = m_pDevice->pCmdDispatch;

	hCTransferChannel* pChannel = pCmdDispatch->getPort( (iportNumber & 0xff) );
		// we do not own pChannel memory - do not delete
	if (pChannel == NULL)
	{
		LOGIT(CERR_LOG|UI_LOG,"Memory error: port %d could not be opened\n",iportNumber);
		return BI_ERROR;
	}
	else
	if (pChannel->Session_State == ss_Undefined || pChannel->Session_State == ss_Closed)
	{
		LOGIT(CERR_LOG|UI_LOG,"Error: port %d not opened\n",iportNumber);
		return BI_ERROR;
	}
	else
	{
		assert(pChannel->portNumber == iportNumber);

		sessionState_t hldSt = pChannel->Session_State;
		pChannel->Session_State = ss_Closing;

		if ( (rc = pChannel->setupCmd111(tf_ResetPort) )  != BI_SUCCESS )
		{
			pChannel->Session_State = hldSt;
			return rc;
		}

		if ( (rc = send_command( 111 )) != BI_SUCCESS )////////////////////////////////////
		{// exit on command error
			pChannel->Session_State = hldSt;
			return rc;
		}	///////////////////////////////////////////////////////////////////////////////
		
		//  fill in port info;
		pChannel->fillPortInfo(111);

	//  reply of 'Port Closed' is success..BI_SUCCESS
		if (pChannel->funcCd == tf_PortClosed)
		{			
			pChannel->Session_State = ss_Closed;
			return BI_SUCCESS;
		}
		else
	// if reply function is 'Transfer Data' there is more for the stack to do..BI_PORT_IN_USE
		if(pChannel->funcCd == tf_Transfer)
		{
			pChannel->Session_State = hldSt;
			return BI_PORT_IN_USE;
		}
		else
	// everything else is BI_ERROR
		{
			pChannel->Session_State = hldSt;
			return BI_ERROR;
		}
	}
	return BI_ERROR;//never reached.
}

// conceptually this is write to stream...some other process needs to deal with transport
int   CHart_Builtins :: write2Port(int iportNumber, unsigned itemNumber)
{
	RETURNCODE  rc = FAILURE;
	hCcmdDispatcher* pCmdDispatch = m_pDevice->pCmdDispatch;

	hCTransferChannel* pChannel = pCmdDispatch->getPort( (iportNumber & 0xff) );
		// we do not own pChannel memory - do not delete

	if (pChannel == NULL)
	{
		LOGIT(CERR_LOG|UI_LOG,"Memory error: port %d could not be written\n",iportNumber);
		return BI_ERROR;
	}
	else
	if (pChannel->Session_State != ss_Opened)
	{
		LOGIT(CERR_LOG|UI_LOG,"Error: port %d not writable (not opened).\n",iportNumber);
		return BI_ERROR;
	}
	else// state must be ts_Opened
	{// we have to put the data into the stream buffers
//		if (pChannel->itemBeingTransfered == 0 )
//			pChannel->itemBeingTransfered = itemNumber;

		if ( pChannel->fillPortBuffer(itemNumber) != SUCCESS )
			return BI_ERROR;
		// else
		// we have to send the first cmd 112, if that does all the data, return BI_SUCCESS
		// otherwise, turn on the 112 sender and return BI_CONTINUE
	}

	return BI_SUCCESS;
}

// conceptually this is read from stream...some other process needs to deal with transport
int   CHart_Builtins :: readFromPort(int iportNumber, unsigned itemNumber)
{
	RETURNCODE  rc = FAILURE;
	hCcmdDispatcher* pCmdDispatch = m_pDevice->pCmdDispatch;

	hCTransferChannel* pChannel = pCmdDispatch->getPort( (iportNumber & 0xff) );
		// we do not own pChannel memory - do not delete
	if (pChannel == NULL)
	{
		LOGIT(CERR_LOG|UI_LOG,"Memory error: port %d could not be read\n",iportNumber);
		return BI_ERROR;
	}
	else
	if (pChannel->Session_State != ss_Opened)
	{
		LOGIT(CERR_LOG|UI_LOG,"Error: port %d not readable (not opened).\n",iportNumber);
		return BI_ERROR;
	}
	else// state must be ts_Opened
	{
		pChannel->downloadData();

//		if (pChannel->itemBeingTransfered == 0 )
//			pChannel->itemBeingTransfered = itemNumber;
		// we have to get the data from the stream buffers to the device-object item
		if ( pChannel->emtyPortBuffer(itemNumber) != SUCCESS )
			return BI_ERROR;
	}
	return BI_SUCCESS;
}

int   CHart_Builtins :: getTransferStatus(int iportNumber,int iDirection,long pLongItemIds[],   
																		int iNumberOfItemIds)
{// full-duplex ports are not currently supported
	// we assume that the transport layer fills the port info on each transaction
	// we assume that any sending of command 111 also fills the port info
	// we only report the port info from here
	RETURNCODE  rc = FAILURE;
	hCcmdDispatcher* pCmdDispatch = m_pDevice->pCmdDispatch;

	hCTransferChannel* pChannel = pCmdDispatch->getPort( (iportNumber & 0xff) );
		// we do not own pChannel memory - do not delete
	if (pChannel == NULL)
	{
		LOGIT(CERR_LOG|UI_LOG,"Memory error: port %d could not be opened\n",iportNumber);
		return BI_ERROR;
	}
	else
	if (pChannel->Session_State == ss_Undefined || pChannel->Session_State == ss_Closed)
	{
		LOGIT(CERR_LOG|UI_LOG,"Error: port %d not opened\n",iportNumber);
		return BI_ERROR;
	}
	else
	{
		assert(pChannel->portNumber == iportNumber);

		if (iNumberOfItemIds>0)		pLongItemIds[0] = pChannel->respCd;
		if (iNumberOfItemIds>1)		pLongItemIds[1] = pChannel->funcCd;
		if (iNumberOfItemIds>2)		pLongItemIds[2] = pChannel->masterSync;
		if (iNumberOfItemIds>3)		pLongItemIds[3] = pChannel->slave_Sync;

		return BI_SUCCESS;
	}
	return -1;
}
