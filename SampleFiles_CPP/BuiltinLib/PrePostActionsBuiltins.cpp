//#include "stdafx.h"
//#include <windows.h>
//#include <ALPHAOPS.H>
#include "pch.h"
#include "ddbGeneral.h"
#include <limits.h>
#include <float.h>
#include "ddbdevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"
#include "MEE.h"

// from alphaops.h::
#define SINGLE_SIGNAL_NAN_PREFIX 0x7f800000

/* the  NaN Signaling Infinity divided by Infinity is the HART NAN!! stevev 11jul05 */
#define SINGLE_INF_DIVBY_INF_NAN (SINGLE_SIGNAL_NAN_PREFIX | 0x00200000) 
#define HART_NAN      SINGLE_INF_DIVBY_INF_NAN


/*********************************Vibhor 270204:Start of Comment*********************************
Signature of Read() has been modified to pass a bool flag, telling whether the reqd value 
needs to be scaled or not.
All the calls in this file DO NOT REQUIRE scaling so "false" is passed as the third argument
/*********************************Vibhor 270204:End of Comment***********************************/

/***********************************Vibhor 041106: Start of Comment*****************************
After the issue seen with ABB 600T for action - re entrancy and consequent escalation 
Wally took a decision on Nov 02 2006 that scaling functions will not trigger a device read
The user (read caller here) whatever is in the variable at that time (irrespective of data quality and state)
Hence all the *get*() functions below will now call just pVar->getRawDispValue() instead of Read()
Please compare with previous version to see the difference
***********************************Vibhor 041106: End of Comment*****************************/
/********************************* stevev 19 jun09 Note 1 ***********************************
Note 1: In september of '07 the xsetval() functions were repaired to prevent dynamic updates
	from walking on the new scaled value.  In that clean up, the operation to copy the new
	Display value to the Device Value was deleted.
	Scaling-Functions must put their new values into all three data copies: a) Display Value -
	(Just like we have always done), b) Device-Value - (Just like it came from the device) and
	c) Cache-Value - (as if a save_values() was called just on that variable.)
*********************************** end of note ********************************************/
/*******************************stevev 14 august 2014 ****************************************
 * Preventing dynamic updates from overwriting the scaled value is unworkable.  If that is an 
 * issue, the writer needs to have a post read action that will re-scale the value when read
 * in the dynamic command.  The user-written bit is set in methods so the ischanged stays in
 * place and the display value is not overwritten by dynamic reads.  In the action built-ins
 * here (*setval), the new display value is copied to the cache and the device value so the
 * is changed will never be true, defeating the need for user-written bit being set.
 * This was found when a dynamic read-only value had a post read action to scale it.  The 
 * user-written bit was set so the display was never updated after the first read. Unacceptable
 ************************************end note************************************************/


/***** Start Pre and Post actions builtins *****/
INT64 CHart_Builtins::igetvalue()
{
	INT64   iretVal = BI_ERROR;
	hCitemBase* pIB = NULL;

	if(m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVar = (hCVar*)pIB;
		if( true == pVar->IsNumeric() )
		{
			CValueVarient ppReturnedDataItem;
			/* Ideally we should have checked for IsInt, but there are DDs 
		    which assign one numeric type to other */
			ppReturnedDataItem = pVar->getRawDispValue(); //don't check what we got
			iretVal=(INT64)ppReturnedDataItem;
		}// else type error
	}// else parameter error
	return iretVal;// single point return
}

INT64 CHart_Builtins::igetval()
{
	return igetvalue();
}

float CHart_Builtins::fgetval()
{
	float fretVal;
	CValueVarient ppReturnedDataItem;
	hCVar *pVar = NULL;
	hCitemBase* pIB = NULL;

	if(m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS)
	{
		if(NULL != pIB)
	{
			pVar = (hCVar*)pIB;
			if(true == pVar->IsNumeric())
			{/* Ideally we should have checked for IsInt, but there are DDs 
			    which assign one numeric type to other */
				ppReturnedDataItem = pVar->getRawDispValue(); //don't check what we got
		fretVal=(float)ppReturnedDataItem;
		return(fretVal);
				
			}
	
	}
	}	
	// We come here ONLY if we fell through one of the above conditions....
	return BI_ERROR;
}

double CHart_Builtins::dgetval()
{
	double dretVal;
	CValueVarient ppReturnedDataItem;
	hCVar *pVar = NULL;
	hCitemBase* pIB = NULL;

	if(m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS)
	{
		if(NULL != pIB)
		{
			pVar = (hCVar*)pIB;
			if(true == pVar->IsNumeric())
			{/* Ideally we should have checked for IsInt, but there are DDs 
			    which assign one numeric type to other */
				ppReturnedDataItem = pVar->getRawDispValue(); //don't check what we got
				dretVal=(double)ppReturnedDataItem;
				return(dretVal);				
			}	
		}
	}
	// We come here ONLY if we fell through one of the above conditions....
	return BI_ERROR;
}

INT64 CHart_Builtins::lgetval()
{	
	return (INT64)igetvalue();
}

int CHart_Builtins::isetval(INT64 iValue)
{
	int nRetVal = BI_ERROR;//start in failed state
	hCitemBase* pIB = NULL;
	
	if( m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarDest = (hCVar*)pIB;
		CValueVarient tempValue;
		if(true == pVarDest->IsNumeric())
		{
			tempValue = (INT64)iValue;
			pVarDest->setRawDispValue(tempValue);//Set the raw value.Handles Locals
			pVarDest->ApplyIt();         // see Note 1 at top
			pVarDest->cacheValue();      // display => cache (scaling function)

			/* see 14aug14 not at the top of the file
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                             //                                    dynamic comands
			**/
			// stevev 26sep08 .. a method can't change this state!....    
			//                    pVarDest->markItemState(IDS_CACHED);
			// stevev 26sep08 we have to notify that there has been a change -scaling-
			hCmsgList        msgs;  
			msgs.insertUnique(pVarDest->getID(), mt_Mth, 0);
			pVarDest->notifyUpdate(msgs);
			nRetVal = BI_SUCCESS;//we have successfully executed
		}
	}
	return nRetVal;//single exit point
}

int CHart_Builtins::fsetval(float fValue)
{
	hCitemBase* pIB = NULL;
	
	if(m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarDest = (hCVar*)pIB;
	
		if(true == pVarDest->IsNumeric())
		{/* Ideally we should have checked for IsFloat, but there are DDs 
			which assign one numeric type to other */
			CValueVarient tempValue; tempValue = fValue;			/*Vibhor 270204*/
			pVarDest->setRawDispValue(tempValue);//Set the raw value.Handles Locals
			pVarDest->ApplyIt();                 // see Note 1 at top
			pVarDest->cacheValue();			      // display => cache (scaling function)
			/* see 14aug14 not at the top of the file
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                             //                                    dynamic comands
			**/
			// stevev 26sep08 .. a method can't change this state!....    
			//                    pVarDest->markItemState(IDS_CACHED);
			// stevev 26sep08 we have to notify that there has been a change -scaling-
			hCmsgList        msgs;  
			msgs.insertUnique(pVarDest->getID(), mt_Mth, 0);
			pVarDest->notifyUpdate(msgs);
			return BI_SUCCESS;
		}
	}	
	// We come here ONLY if we fell through one of the above conditions....
	return BI_ERROR;
}

int CHart_Builtins::lsetval(INT64 lValue)
{// stevev 26sep08 - this seems a little ridiculous...
	return isetval(lValue);
}
/*  too much cut n paste....
	int nRetVal = BI_ERROR;//start in failed state
	hCitemBase* pIB = NULL;
	
	if(m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS && pIB != NULL)
	{
		CValueVarient tempValue;
		hCVar *pVarDest = (hCVar*)pIB;
		
		if(true == pVarDest->IsNumeric())
		{
			tempValue = (INT64)lValue;
			pVarDest->setRawDispValue(tempValue);//was setDispValue(tempValue);
			pVarDest->ApplyIt();
			pVarDest->cacheValue();
			pVarDest->markItemState(IDS_CACHED);
			nRetVal = BI_SUCCESS;
		}	
	}
	return nRetVal;//single exit point
}
*/
int CHart_Builtins::dsetval(double dValue)
{
	hCitemBase* pIB = NULL;

	if(m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS && NULL != pIB)
	{
		hCVar *pVarDest = (hCVar*)pIB;
		CValueVarient tempValue;		
		if(true == pVarDest->IsNumeric())
		{/* Ideally we should have checked for IsDouble, but there are DDs 
			which assign one numeric type to other */
			tempValue = (double)dValue;
			pVarDest->setRawDispValue(tempValue);//Set the raw value.Handles Locals
			pVarDest->ApplyIt();         // see Note 1 at top
			pVarDest->cacheValue();      // display => cache (scaling function)
			/* see 14aug14 not at the top of the file
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                             //                                    dynamic comands
			**/
			// stevev 26sep08 .. a method can't change this state!....    
			//                    pVarDest->markItemState(IDS_CACHED);
			// stevev 26sep08 we have to notify that there has been a change			
			hCmsgList        msgs;  
			msgs.insertUnique(pVarDest->getID(), mt_Mth, 0);
			pVarDest->notifyUpdate(msgs);
			return BI_SUCCESS;
		}	
	}	
	// We come here ONLY if we fell through one of the above conditions....
	return BI_ERROR;
}

int CHart_Builtins::save_values()
{
//	bSaveValuesCalled = true; 	
	m_pMeth->save_values(); 
	return BI_SUCCESS;
}

//Added By Anil July 01 2005 --starts here
int CHart_Builtins::discard_on_exit()
{
	m_pMeth->discard_on_exit(); 
	return BI_SUCCESS;
}

//Added By Anil July 01 2005 --Ends here

/* the floating point processor/library does not support a HART NAN - copde it into an int */
UINT CHart_Builtins::NaN_value()
{
	/* WRONG stevev 11jul05  return(SINGLE_QUIET_NAN_VALUE);  */
	return (HART_NAN);
}
	
/*Arun 190505 Start of code*/

int CHart_Builtins::sgetval(char* stringvalue, int length)
{
	CValueVarient ppReturnedDataItem;

	hCVar *pVar = NULL;
	hCitemBase* pIB = NULL;
	if (stringvalue == NULL || length < 1 )
	{
		return BI_ERROR;
	}

	if(m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS)
	{
		if(NULL != pIB)
		{
			pVar = (hCVar*)pIB;
			ppReturnedDataItem = pVar->getRawDispValue(); //don't check the rc or what we got
			
			string stemp;
			stemp = ppReturnedDataItem.sStringVal; 
			/* incorrect treatment- stevev 30may07
			if(stemp.length() <= length)
				return BI_ERROR;
			else
				strcpy(stringvalue,stemp.c_str());
			*/
#ifdef MODIFIED_BY_SS
			//strncpy(stringvalue,stemp.c_str(),(length-1));
			std::copy(stemp.begin(), stemp.end(), stringvalue);
#endif 
			stringvalue[length-1] = '\0';
				
			return BI_SUCCESS;	
		}
		//else fall thru to error return
	}
	// else fall thru to error return
		
	return BI_ERROR;

}

// stevev 27dec07 - only wide strings are available from the methods
//					BUILTIN_ssetval in BuiltinInvoke converts to string
//					only normal ascii may be set to a dd variable 
char* CHart_Builtins::ssetval(const char* value)
{
	CValueVarient tempValue;
	hCitemBase* pIB = NULL;
		
	string S(value);	
	
	if(m_pDevice->getItemBySymNumber(lPre_postItemID,&pIB) == SUCCESS && pIB != NULL)
	{
		hCVar *pVarDest = (hCVar*)pIB;

		if(pVarDest->VariableType() == vT_Ascii || pVarDest->VariableType() ==  vT_PackedAscii
			|| pVarDest->VariableType() == vT_Password /*|| pVarDest->VariableType() == vT_BitString*/)
		{
			tempValue = S;// to a short string
			pVarDest->setRawDispValue(tempValue);//Set the raw value.Handles Locals
			pVarDest->ApplyIt();         // see Note 1 at top
			pVarDest->cacheValue();      // display => cache (scaling function)
			/* see 14aug14 not at the top of the file
			pVarDest->setWriteStatus(1); // stevev 26sep08 - these were getting overwritten by 
			                             //                                    dynamic comands
			**/
			// stevev 26sep08 .. a method can't change this state!....    
			//                    pVarDest->markItemState(IDS_CACHED);	
			// stevev 26sep08 we have to notify that there has been a change -scaling-		
			hCmsgList        msgs;  
			msgs.insertUnique(pVarDest->getID(), mt_Mth, 0);
			pVarDest->notifyUpdate(msgs);
		}
	}
	return (char*) value;
}

/*End of code*/	

/* End Pre and Post actions builtins */


