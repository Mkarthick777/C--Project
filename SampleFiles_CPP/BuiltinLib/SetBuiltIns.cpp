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


/***** Start Scaling ABORT, IGNORE, RETRY builtins *****/
int CHart_Builtins:: _set_comm_status(int iCommStatus, int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byCommAbortMask |= (unsigned char) iCommStatus;
			m_pMeth->m_byCommRetryMask &= ~(unsigned char) iCommStatus;
			break;

		case __RETRY__:
			m_pMeth->m_byCommAbortMask &= ~(unsigned char) iCommStatus;
			m_pMeth->m_byCommRetryMask |= (unsigned char) iCommStatus;
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byCommAbortMask &= ~(unsigned char) iCommStatus;
			m_pMeth->m_byCommRetryMask &= ~(unsigned char) iCommStatus;
			break;
	}

 return BI_SUCCESS;
}/*End _set_comm_status*/

int CHart_Builtins :: _set_device_status (int iDeviceStatus, int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byStatusAbortMask |= (unsigned char) iDeviceStatus;
			m_pMeth->m_byStatusRetryMask &= ~(unsigned char) iDeviceStatus;
			break;

		case __RETRY__:
			m_pMeth->m_byStatusAbortMask &= ~(unsigned char) iDeviceStatus;
			m_pMeth->m_byStatusRetryMask |= (unsigned char) iDeviceStatus;
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byStatusAbortMask &= ~(unsigned char) iDeviceStatus;
			m_pMeth->m_byStatusRetryMask &= ~(unsigned char) iDeviceStatus;
			break;
	}

 return BI_SUCCESS;

}/*End _set_device_status*/

int CHart_Builtins :: _set_resp_code(int iResponseCode, int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byRespAbortMask[BYTENUM(iResponseCode)] |= (unsigned char)(1<<BITNUM(iResponseCode));
			m_pMeth->m_byRespRetryMask[BYTENUM(iResponseCode)] &= ~(unsigned char)(1<<BITNUM(iResponseCode));
			break;

		case __RETRY__:
			m_pMeth->m_byRespAbortMask[BYTENUM(iResponseCode)] &= ~(unsigned char)(1<<BITNUM(iResponseCode));
			m_pMeth->m_byRespRetryMask[BYTENUM(iResponseCode)] |= (unsigned char)(1<<BITNUM(iResponseCode));
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byRespAbortMask[BYTENUM(iResponseCode)] &= ~(unsigned char)(1<<BITNUM(iResponseCode));
			m_pMeth->m_byRespRetryMask[BYTENUM(iResponseCode)] &= ~(unsigned char)(1<<BITNUM(iResponseCode));
			break;
	}

 return BI_SUCCESS;

}/*End _set_device_status*/


int CHart_Builtins :: _set_all_resp_code(int iAbortIgnoreRetry)
{
	int i;

	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			
			m_pMeth->m_byRespAbortMask[0] = 0xFE;
			m_pMeth->m_byRespRetryMask[0] = 0;
			for(i = 1; i< RESP_MASK_LEN; i++)
			{
				m_pMeth->m_byRespAbortMask[i] = 0xFF;
				m_pMeth->m_byRespRetryMask[i] = 0;
			}
			break;

		case __RETRY__:
			m_pMeth->m_byRespAbortMask[0] = 0;
			m_pMeth->m_byRespRetryMask[0] = 0xFE;
			for(i = 1; i< RESP_MASK_LEN; i++)
			{
				m_pMeth->m_byRespAbortMask[i] = 0;
				m_pMeth->m_byRespRetryMask[i] = 0xFF;
			}
			break;

		default:		/* __IGNORE__ */
			for(i = 0; i< RESP_MASK_LEN; i++)
			{
				m_pMeth->m_byRespAbortMask[i] = 0;
				m_pMeth->m_byRespRetryMask[i] = 0;
			}
			break;
	}
	
	return BI_SUCCESS;

}/*End _set_all_resp_code*/

int CHart_Builtins	:: _set_no_device(int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byReturnNodevAbortMask |= (unsigned char) true;;
			m_pMeth->m_byReturnNodevRetryMask &= ~(unsigned char) true;;
			break;

		case __RETRY__:
			m_pMeth->m_byReturnNodevAbortMask &= ~(unsigned char) true;
			m_pMeth->m_byReturnNodevRetryMask |= (unsigned char) true;
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byReturnNodevAbortMask &= ~(unsigned char) true;
			m_pMeth->m_byReturnNodevRetryMask &= ~(unsigned char) true;
			break;
	}

 return BI_SUCCESS;


}/*End _set_no_device*/


int CHart_Builtins :: SET_NUMBER_OF_RETRIES(int iNumberOfRetries )
{
	
	m_pMeth->m_iAutoRetryLimit = iNumberOfRetries;

	return BI_SUCCESS;

}/*End SET_NUMBER_OF_RETRIES*/



/* End Scaling ABORT, IGNORE, RETRY builtins */




/***** Start XMTR ABORT, IGNORE, RETRY builtins *****/
int CHart_Builtins :: _set_xmtr_comm_status(int iCommStatus, int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byXmtrCommAbortMask |= (unsigned char) iCommStatus;
			m_pMeth->m_byXmtrCommRetryMask &= ~(unsigned char) iCommStatus;
			break;

		case __RETRY__:
			m_pMeth->m_byXmtrCommAbortMask &= ~(unsigned char) iCommStatus;
			m_pMeth->m_byXmtrCommRetryMask |= (unsigned char) iCommStatus;
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byXmtrCommAbortMask &= ~(unsigned char) iCommStatus;
			m_pMeth->m_byXmtrCommRetryMask &= ~(unsigned char) iCommStatus;
			break;
	}

 return BI_SUCCESS;
	

}

int CHart_Builtins :: _set_xmtr_device_status(int iDeviceStatus, int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byXmtrStatusAbortMask |= (unsigned char) iDeviceStatus;
			m_pMeth->m_byXmtrStatusRetryMask &= ~(unsigned char) iDeviceStatus;
			break;

		case __RETRY__:
			m_pMeth->m_byXmtrStatusAbortMask &= ~(unsigned char) iDeviceStatus;
			m_pMeth->m_byXmtrStatusRetryMask |= (unsigned char) iDeviceStatus;
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byXmtrStatusAbortMask &= ~(unsigned char) iDeviceStatus;
			m_pMeth->m_byXmtrStatusRetryMask &= ~(unsigned char) iDeviceStatus;
			break;
	}

 return BI_SUCCESS;

}/*End _set_xmtr_device_status*/

int CHart_Builtins :: _set_xmtr_resp_code(int iResponseCode, int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byXmtrRespAbortMask[BYTENUM(iResponseCode)] |= (unsigned char)(1<<BITNUM(iResponseCode));
			m_pMeth->m_byXmtrRespRetryMask[BYTENUM(iResponseCode)] &= ~(unsigned char)(1<<BITNUM(iResponseCode));
			break;

		case __RETRY__:
			m_pMeth->m_byXmtrRespAbortMask[BYTENUM(iResponseCode)] &= ~(unsigned char)(1<<BITNUM(iResponseCode));
			m_pMeth->m_byXmtrRespRetryMask[BYTENUM(iResponseCode)] |= (unsigned char)(1<<BITNUM(iResponseCode));
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byXmtrRespAbortMask[BYTENUM(iResponseCode)] &= ~(unsigned char)(1<<BITNUM(iResponseCode));
			m_pMeth->m_byXmtrRespRetryMask[BYTENUM(iResponseCode)] &= ~(unsigned char)(1<<BITNUM(iResponseCode));
			break;
	}

 return BI_SUCCESS;


}/*End _set_xmtr_resp_code*/


int CHart_Builtins :: _set_xmtr_all_resp_code(int iAbortIgnoreRetry)
{
	int i;

	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			
			m_pMeth->m_byXmtrRespAbortMask[0] = 0xFE;
			m_pMeth->m_byXmtrRespRetryMask[0] = 0;
			for(i = 1; i< RESP_MASK_LEN; i++)
			{
				m_pMeth->m_byXmtrRespAbortMask[i] = 0xFF;
				m_pMeth->m_byXmtrRespRetryMask[i] = 0;
			}
			break;

		case __RETRY__:
			m_pMeth->m_byXmtrRespAbortMask[0] = 0;
			m_pMeth->m_byXmtrRespRetryMask[0] = 0xFE;
			for(i = 1; i< RESP_MASK_LEN; i++)
			{
				m_pMeth->m_byXmtrRespAbortMask[i] = 0;
				m_pMeth->m_byXmtrRespRetryMask[i] = 0xFF;
			}
			break;

		default:		/* __IGNORE__ */
			for(i = 0; i< RESP_MASK_LEN; i++)
			{
				m_pMeth->m_byXmtrRespAbortMask[i] = 0;
				m_pMeth->m_byXmtrRespRetryMask[i] = 0;
			}
			break;
	}
	
	return BI_SUCCESS;

}/*End _set_xmtr_all_resp_code*/

int CHart_Builtins :: _set_xmtr_no_device(int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byXmtrReturnNodevAbortMask |= (unsigned char) true;;
			m_pMeth->m_byXmtrReturnNodevRetryMask &= ~(unsigned char) true;;
			break;

		case __RETRY__:
			m_pMeth->m_byXmtrReturnNodevAbortMask &= ~(unsigned char) true;
			m_pMeth->m_byXmtrReturnNodevRetryMask |= (unsigned char) true;
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byXmtrReturnNodevAbortMask &= ~(unsigned char) true;
			m_pMeth->m_byXmtrReturnNodevRetryMask &= ~(unsigned char) true;
			break;
	}

 return BI_SUCCESS;

}/*End _set_xmtr_no_device*/

int CHart_Builtins :: _set_xmtr_all_data(int iAbortIgnoreRetry)
{
	int i;
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			
			for(i = 0; i< DATA_MASK_LEN; i++)
			{
				m_pMeth->m_byXmtrDataAbortMask[i] = 0xFF;
				m_pMeth->m_byXmtrDataRetryMask[i] = 0;
			}
			break;

		case __RETRY__:
			
			for(i = 0; i< DATA_MASK_LEN; i++)
			{
				m_pMeth->m_byXmtrDataAbortMask[i] = 0;
				m_pMeth->m_byXmtrDataRetryMask[i] = 0xFF;
			}
			break;

		default:		/* __IGNORE__ */
			
			for(i = 0; i< DATA_MASK_LEN; i++)
			{
				m_pMeth->m_byXmtrDataAbortMask[i] = 0;
				m_pMeth->m_byXmtrDataRetryMask[i] = 0;
			}
			break;
	}
	
	return BI_SUCCESS;

}/*End _set_xmtr_all_data*/


int CHart_Builtins :: _set_xmtr_data(int iByteCode, int iBitMask, int iAbortIgnoreRetry)
{
	switch(iAbortIgnoreRetry)
	{
		case __ABORT__:
			m_pMeth->m_byXmtrDataAbortMask[BYTENUM(iByteCode)] |= (unsigned char)(1<<BITNUM(iBitMask));
			m_pMeth->m_byXmtrDataRetryMask[BYTENUM(iByteCode)] &= ~(unsigned char)(1<<BITNUM(iBitMask));
			break;

		case __RETRY__:
			m_pMeth->m_byXmtrDataAbortMask[BYTENUM(iByteCode)] &= ~(unsigned char)(1<<BITNUM(iBitMask));
			m_pMeth->m_byXmtrDataRetryMask[BYTENUM(iByteCode)] |= (unsigned char)(1<<BITNUM(iBitMask));
			break;

		default:		/* __IGNORE__ */
			m_pMeth->m_byXmtrDataAbortMask[BYTENUM(iByteCode)] &= ~(unsigned char)(1<<BITNUM(iBitMask));
			m_pMeth->m_byXmtrDataRetryMask[BYTENUM(iByteCode)] &= ~(unsigned char)(1<<BITNUM(iBitMask));
			break;
	}

 return BI_SUCCESS;
	
}/*End _set_xmtr_data*/

