#pragma warning (disable : 4786)


#include "pch.h"
#include "DDlConditional.h"

void VALUES::Cleanup(DDL_ATTR_DATA_TYPE dataType)
{
	switch(dataType)
	{
		case DDL_ATTR_DATA_TYPE_STRING:
			strVal->Cleanup();
			delete strVal;
			strVal = NULL;
			break;
		case DDL_ATTR_DATA_TYPE_ENUM_LIST:
			if(enmList != NULL)
			{
				ENUM_VALUE_LIST:: iterator it;
				for(it = enmList->begin(); it != enmList->end(); it++)
				{
					(*it).Cleanup();
				}
				//enmList->clear();
				enmList->erase(enmList->begin(),enmList->end());
				delete enmList;
				enmList = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_REFERENCE:
			if(ref != NULL)
			{
				ddpREFERENCE :: iterator it;
				for(it = ref->begin(); it != ref->end(); it++)
				{
					(*it).Cleanup();
				}
				ref->clear();
				delete ref;
				ref = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
			if(refList != NULL)
			{
				REFERENCE_LIST :: iterator it;
				ddpREFERENCE :: iterator it1;

				for(it = refList->begin(); it != refList->end(); it++)
				{
					for(it1 = (*it).begin(); it1 != (*it).end(); it1++)
					{
						(*it1).Cleanup();
						
					}
					(*it).clear();
				}

				refList->clear();
				delete refList;
				refList = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_TRANSACTION_LIST:
			if(transList != NULL)
			{
				TRANSACTION_LIST :: iterator it;
				
				for(it = transList->begin(); it != transList->end(); it++)
				{
					(*it).Cleanup();
				}
				transList->clear();
				delete transList;
				transList = NULL;
			}
			break;
			
		case DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
			if(respCdList != NULL)
			{
				RESPONSE_CODE_LIST :: iterator it;
				
				for(it = respCdList->begin(); it != respCdList->end(); it++)
				{
					(*it).Cleanup();
				}
				respCdList->clear();
				delete respCdList;
				respCdList = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
			if(menuItemsList != NULL)
			{
				MENU_ITEM_LIST :: iterator it;
#ifdef _DEBUG
				int Z = 0;
				for(it = menuItemsList->begin(); it != menuItemsList->end(); it++,Z++)
#else
				for(it = menuItemsList->begin(); it != menuItemsList->end(); it++)
#endif
				{
					(*it).Cleanup();
				}

				menuItemsList->clear();
				delete menuItemsList;
				menuItemsList = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_DEFINITION:
			if(defData.data != NULL)
			{
				delete [] defData.data;
			}
			defData.data = NULL;
			break;
		case DDL_ATTR_DATA_TYPE_REFRESH_RELATION:
			if(refrshReln != NULL)
			{
				refrshReln->Cleanup();
				delete refrshReln;
				refrshReln = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_UNIT_RELATION:
			if(unitReln != NULL)
			{
				unitReln->Cleanup();
				delete  unitReln;
				unitReln = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
			if(itemArrElmnts != NULL)
			{
				ITEM_ARRAY_ELEMENT_LIST :: iterator it;
				
				for(it = itemArrElmnts->begin(); it != itemArrElmnts->end(); it++)
				{
					(*it).Cleanup();
				}
				itemArrElmnts->clear();
				delete itemArrElmnts;
				itemArrElmnts = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_MEMBER_LIST:
			if(memberList != NULL)
			{
				MEMBER_LIST :: iterator it;
				
				for(it = memberList->begin(); it != memberList->end(); it++)
				{
					(*it).Cleanup();
				}
				memberList->clear();
				delete memberList;
				memberList = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_EXPRESSION:
			if(pExpr != NULL)
			{
				ddpExpression :: iterator it;
				
				for(it = pExpr->begin(); it != pExpr->end(); it++)
				{
					(*it).Cleanup();
				}
				pExpr->clear();
				delete pExpr;
				pExpr = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_MIN_MAX:
			if(minMaxList != NULL)
			{
				MIN_MAX_LIST :: iterator it;
				
				for(it = minMaxList->begin(); it != minMaxList->end(); it++)
				{
					(*it).Cleanup();
				}
				minMaxList->clear();
				delete minMaxList;
				minMaxList = NULL;
			}
			break;
		case DDL_ATTR_DATA_TYPE_GRID_SET:
			{
				if (gridMemList != NULL)
				{
					GRID_SET_LIST :: iterator it;
					
					for(it = gridMemList->begin(); it != gridMemList->end(); it++)
					{
						(*it).Cleanup();
					}
					gridMemList->clear();
					delete gridMemList;
					gridMemList = NULL;
				}
			}
			break;
		case DDL_ATTR_DATA_TYPE_DEBUG_DATA:
			{
				if (debugInfo != NULL)
				{
					debugInfo->Cleanup();
					delete debugInfo;
					debugInfo = NULL;
				}
			}
			break;
		case DDL_ATTR_DATA_TYPE_PARAM     :
			{
				if (methodType != NULL && methodType->param_name != NULL)
				{
					delete[] methodType->param_name;
					methodType->param_name = NULL; // stevev 21aug07 prevent trying to delete it again elsewhere
				}
				//Anil 260706: Memory leak Fix
				if (methodType != NULL)
				{
					methodType->Clear();
					delete methodType;
					methodType = NULL;
				}
			}
			break;
		case DDL_ATTR_DATA_TYPE_PARAM_LIST:
			{
				if (paramList != NULL)
				{
					METHOD_PARAM_LIST :: iterator it;
					
					for(it = paramList->begin(); it != paramList->end(); it++)
					{
						if (it->param_name != NULL)
						{
							delete[] it->param_name;
							it->param_name = NULL; // stevev 21aug07 prevent trying to delete it again elsewhere
						}
					}
					paramList->clear();
					delete paramList;
					paramList = NULL;
				}
			}
			break;

		default:
			break;
				
	}/*End switch*/

	
}

// DDlConditional 

DDlConditional::~DDlConditional()
{
	unsigned i;
	ddpExpression :: iterator it;
	for(it= expr.begin(); it!= expr.end(); it++)
	{
		(*it).Cleanup();
	}
	expr.clear();
	
	CaseConstList:: iterator it1;

	for(it1= caseVals.begin(); it1!= caseVals.end(); it1++)
	{
		for(it= (*it1).begin(); it!= (*it1).end(); it++)
		{
			(*it).Cleanup();
		}
		(*it1).clear();
	}
	caseVals.clear();
	
	ValueList :: iterator it2;

	for(i = 0,it2 = Vals.begin();i < isSectionConditionalList.size() && it2 != Vals.end();i++ ,it2++)
	{
		if(isSectionConditionalList.at(i) == DDL_SECT_TYPE_DIRECT)//Vibhor 200105: Changed
		{
			(*it2).Cleanup(attrDataType);
		}
	}
	Vals.clear();

	isSectionConditionalList.clear();
	
	ConditionalList:: iterator it3;

	for (it3 = listOfChilds.begin();it3 != listOfChilds.end(); it3++)
	{
		delete (*it3);

	}
	listOfChilds.clear();
	/*Vibhor 200105: Start of Code*/
	SectionChunkList :: iterator it4;

	for(it4 = listOfChunks.begin();it4 != listOfChunks.end();it4++)
	{
		delete (*it4);
	}
	listOfChunks.clear();
	/*Vibhor 200105: End of Code*/

}



	// DDlAttribute //


DDlAttribute::~DDlAttribute()
{
	if((bIsAttributeConditional == false) && (bIsAttributeConditionalList == false))
	{/*Direct Attribute*/
		if(pVals != NULL)
		{
			pVals->Cleanup(attrDataType);
			delete pVals;
		
		}/*Endif pVals*/

	}/*Endif bIsAttributeConditional*/
	else if((bIsAttributeConditional == true) && (bIsAttributeConditionalList == false))
	{/*Conditional Attribute */

		if(pCond != NULL)
		{
			delete pCond;
		}
	}
	else if((bIsAttributeConditional == false) && (bIsAttributeConditionalList == true))
	{/*Conditional Chunk(list) Attribute*/
		
		if(isChunkConditionalList.size() >0)
			isChunkConditionalList.clear();
		
		ValueList :: iterator it;
		for(it = directVals.begin(); it != directVals.end(); it++)
		{
			(*it).Cleanup(attrDataType);
		}
		directVals.clear();
		
		ConditionalList:: iterator it1;
		
		for ( it1 = conditionalVals.begin(); it1 != conditionalVals.end(); it1++)
		{
			delete (*it1);

		}
		conditionalVals.clear();
		
	}


}
/*Vibhor 200105: Start of Code*/

DDlSectionChunks::~DDlSectionChunks()
{
	if(isChunkConditionalList.size() >0)
			isChunkConditionalList.clear();
		
	ValueList :: iterator it;
	for(it = directVals.begin(); it != directVals.end(); it++)
	{
		(*it).Cleanup(attrDataType);
	}
	directVals.clear();
	
	ConditionalList:: iterator it1;
	
	for ( it1 = conditionalVals.begin(); it1 != conditionalVals.end(); it1++)
	{
		delete (*it1);

	}
	conditionalVals.clear();
}

/*Vibhor 200105: End of Code*/