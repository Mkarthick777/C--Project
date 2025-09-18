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

DWORD CHart_Builtins :: resolve_group_reference(DWORD dwItemId, DWORD dwIndex, itemType_t typeCheck)
{
	int  rc = SUCCESS;
    hCitemBase* pItm = NULL;
    hCgroupItemDescriptor*  pGID = NULL;
			
	DWORD dwRetVal = 0;
	itemID_t iId = 0;
	/* stevev 25jan07 - expand to handle all group item types */
	bool paramsOK = (typeCheck == iT_ItemArray  || // sjv 25jan07
					 typeCheck == iT_Collection || 
					 typeCheck == iT_File       || 
					 typeCheck == iT_Array      || 
					 typeCheck == iT_List        );
	
    rc = m_pDevice->getItemBySymNumber(dwItemId, &pItm);
    if ( rc == SUCCESS && pItm != NULL &&  paramsOK )
    {
/* stevev 25jan07 replaced the following */
/*Vibhor 120204: Start of Code*/
//				if(typeCheck == iT_Collection)
//				{
//					rc = ((hCcollection *)pItm)->getByIndex(dwIndex, &pGID );
//				}
//				else if(typeCheck == iT_ItemArray)
//				{
//					rc = ((hCitemArray *)pItm)->getByIndex(dwIndex, &pGID );
//				}
/*Vibhor 120204: End of Code*/	
		/* stevev 18feb08 - additional types with unique acess techniques */
		if (pItm->getIType() == iT_ItemArray  || 
			pItm->getIType() == iT_Collection || 
			pItm->getIType() == iT_File        )
		{
/* stevev 25jan07 end of remove, replaced with the following */
			rc = ((hCgrpItmBasedClass *)pItm)->getByIndex(dwIndex, &pGID );			
/* stevev 25jan07: end of replacement **/
			
            if ( rc == SUCCESS && pGID != NULL )
            {
                    hCreference localRef(pGID->getRef());
                    rc = localRef.resolveID(iId);
                    if ( rc != SUCCESS)
                    {
						iId = 0;
                    }
					/*<START>Fixing the leaks Added by ANOOP	09APR2004*/
					delete pGID;
					pGID=NULL;
				
					/*<END>Fixing the leaks Added by ANOOP 09APR2004*/
            }
		}
		else
		if (pItm->getIType() == iT_Array)
		{
			hCarray* pL = (hCarray*)pItm;
			if ( pL->isInGroup(dwIndex) )
			{
				hCitemBase* pI = pL->operator [](dwIndex);
				iId = pI->getID();
			}
			// else bad index - fall thru to error return
		}
		else
		if (pItm->getIType() == iT_List)
		{
			hClist* pL = (hClist*)pItm;
			if ( pL->isInGroup(dwIndex) )
			{
				hCitemBase* pI = pL->operator [](dwIndex);
				iId = pI->getID();
			}
			// else bad index - fall thru to error return
		}
		// else fall through to error handling
    }
	RAZE(pGID);// stevev via HOMZ 21feb07 - maybe alloc'd in getbyindex w/error
	return (DWORD)iId;
}

/*Vibhor 011103: Since these builtins don't have a return code the resolved value is 
 returned as such , which is 0 in case of resolution is unsuccessful; The invoke 
 function should check the return value of such builtins*/

long CHart_Builtins :: resolve_array_ref(long lItemId, int iIndex)
{
	long lRetVal ;
	
	DWORD dwItemid = lItemId, dwIndex = iIndex;
	lRetVal = (DWORD) resolve_group_reference(dwItemid, dwIndex, iT_ItemArray);
    	  
	return lRetVal;
	

}/*End resolve_array_ref*/

long CHart_Builtins :: resolve_record_ref(long lItemId, int iIndex)
{

	long lRetVal ;
	
	DWORD dwItemid = lItemId, dwIndex = iIndex;
	lRetVal = (DWORD) resolve_group_reference(dwItemid, dwIndex, iT_Collection);
    	  
	return lRetVal;
}/*End resolve_record_ref*/

long CHart_Builtins :: resolve_param_ref(long lItemId)
{
	
	if ( lItemId < 0xC0009F00 || lItemId > 0xC0009f62)
        {
                /* error handling */
                return 0;
        }
        else
        {
				long lValue = 0;
				DWORD dwItemValue, dwTemp;
				dwItemValue = (DWORD)lItemId;
				dwTemp = dwItemValue - 0x3fff9fef;
                return (long)dwTemp;
        }

}/*End resolve_param_ref*/
