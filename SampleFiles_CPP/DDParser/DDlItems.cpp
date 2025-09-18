
#pragma warning (disable : 4786)

//sjv06feb06 #include <windows.h>
#include "pch.h"
#ifndef _WIN32_WCE
#include "std.h"	//removed to stop compiler error win ce PAW 09/04/09
#endif
#include "DDlItems.h"


/*********************************************************************************
	
								VARIABLE  ITEM	METHODS
		
/*********************************************************************************/




void DDlVariable ::AllocAttributes( unsigned long ulVarMask)
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;
	
	
/*	if(ulVarMask & VAR_TYPE_SIZE)
	{ */
	
		/*Moved TYPE_SIZE attribute in to eval_variable*/
/*		pDDlAttr = (DDlAttribute*)new DDlAttribute("attrVarTypeSize",
										VAR_TYPE_SIZE_ID,
										DDL_ATTR_DATA_TYPE_TYPE_SIZE,
										false);
	
		attrList.push_back(pDDlAttr); */
	
/*	} */

/*	if(ulVarMask & VAR_CLASS)
	{ */
	/*Type is a mandatory attribute and should be allocated anyway*/
		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarClass",
										VAR_CLASS_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
/*	} */

/*Vibhor 141103 : Moving Handling to the beginning of the attribute list,
 So we will parse Handling in Eval as the first attribute*/		
/*	if(ulVarMask & VAR_HANDLING)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarHandling",
										VAR_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
*/

	if(ulVarMask & VAR_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarLabel",
										VAR_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


		

	if(ulVarMask & VAR_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarHelp",
										VAR_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulVarMask & VAR_DISPLAY)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarDisplayFormat",
										VAR_DISPLAY_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulVarMask & VAR_EDIT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarEditFormat",
										VAR_EDIT_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulVarMask & VAR_ENUMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarEnums",
										VAR_ENUMS_ID,
										DDL_ATTR_DATA_TYPE_ENUM_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


		
	if(ulVarMask & VAR_UNIT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarConstantUnit",
										VAR_UNIT_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

/* removed 15oct12
	if(ulVarMask & VAR_READ_TIME_OUT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarReadTimeOut",
										VAR_READ_TIME_OUT_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulVarMask & VAR_WRITE_TIME_OUT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarWriteTimeOut",
										VAR_WRITE_TIME_OUT_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
****/
		
/*	if(ulVarMask & VAR_RESP_CODES)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarResponseCodes",
										VAR_RESP_CODES_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}  */

	if(ulVarMask & VAR_MIN_VAL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarMinVal",
										VAR_MIN_VAL_ID,
										DDL_ATTR_DATA_TYPE_MIN_MAX,/*This needs to be taken care of*/
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	

	if(ulVarMask & VAR_MAX_VAL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarMaxVal",
										VAR_MAX_VAL_ID,
										DDL_ATTR_DATA_TYPE_MIN_MAX,/*This needs to be taken care of*/
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulVarMask & VAR_SCALE)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarScalingFactor",
										VAR_SCALE_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	
	if(ulVarMask & VAR_INDEX_ITEM_ARRAY)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarIndexItemArrayName",
										VAR_INDEX_ITEM_ARRAY_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulVarMask & VAR_PRE_READ_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarPreReadActions",
										VAR_PRE_READ_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulVarMask & VAR_POST_READ_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarPostReadActions",
										VAR_POST_READ_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulVarMask & VAR_PRE_WRITE_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarPreWriteActions",
										VAR_PRE_WRITE_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulVarMask & VAR_POST_WRITE_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarPostWriteActions",
										VAR_POST_WRITE_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulVarMask & VAR_PRE_EDIT_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarPreEditActions",
										VAR_PRE_EDIT_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(ulVarMask & VAR_POST_EDIT_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarPostEditActions",
										VAR_POST_EDIT_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*After Defining all move this guy at the end*/
	/*We will allocate Validity if mask contains it;
	If not, then we will default it after parsing all other attributes in
	eval_variable*/

	if(ulVarMask & VAR_VALID) 
	{  

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarValidity",
										VAR_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
	/*	pDDlAttr->pVals->ulVal = 0x01L; /*Default Attribute*/

		attrList.push_back(pDDlAttr);
	
	} 


	return ;

} /*End AllocVarAttributes */


/*********************************************************************************
	
								COMMAND  ITEM	METHODS
		
/*********************************************************************************/


void DDlCommand :: AllocAttributes( unsigned long ulCmdMask)
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(ulCmdMask & COMMAND_NUMBER)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdNumber",
										COMMAND_NUMBER_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulCmdMask & COMMAND_OPER)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdOperationType",
										COMMAND_OPER_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulCmdMask & COMMAND_TRANS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdTransaction",
										COMMAND_TRANS_ID,
										DDL_ATTR_DATA_TYPE_TRANSACTION_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulCmdMask & COMMAND_RESP_CODES)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdResponseCodes",
										COMMAND_RESP_CODES_ID,
										DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	return;

} /* End AllocCmdAttributes*/


/*********************************************************************************
	
								MENU	ITEM	METHODS
		
/*********************************************************************************/

DDlMenu :: ~DDlMenu()
{
#ifdef _DBGMIL
	if (id == 0x3eb)
	{
		MENU_ITEM_LIST* pMil = NULL;
		ItemAttrList :: iterator p;

		for(p = attrList.begin();p != attrList.end();p++)
		{
			if((*p)->byAttrID == MENU_ITEMS_ID)
			{
				pMil = (*p)->pVals->menuItemsList;
				break;
			}
		}// next attr	
		LOGIT(COUT_LOG,"\n---- MenuDestructor ------");
		pMil->dumpList();
	}
#endif
};


void DDlMenu :: AllocAttributes( unsigned long ulMenuMask)
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(ulMenuMask & MENU_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MenuLabel",
										MENU_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(ulMenuMask & MENU_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MenuItems",
										MENU_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	return;

}/* End AllocMenuAttributes*/




/*********************************************************************************
	
								EDIT_DISPLAY	ITEM	METHODS
		
/*********************************************************************************/




void DDlEditDisplay :: AllocAttributes( unsigned long ulEditDispMask)
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;


	if(ulEditDispMask & EDIT_DISPLAY_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispLabel",
										EDIT_DISPLAY_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(ulEditDispMask & EDIT_DISPLAY_EDIT_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispEditItems",
										EDIT_DISPLAY_EDIT_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulEditDispMask & EDIT_DISPLAY_DISP_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispDisplayItems",
										EDIT_DISPLAY_DISP_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulEditDispMask & EDIT_DISPLAY_PRE_EDIT_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispPreEditActions",
										EDIT_DISPLAY_PRE_EDIT_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulEditDispMask & EDIT_DISPLAY_POST_EDIT_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispPostEditActions",
										EDIT_DISPLAY_POST_EDIT_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	
	
	return;

}/* End AllocEditDispAttributes*/




/*********************************************************************************
	
								METHOD		ITEM	METHODS
		
/*********************************************************************************/




void DDlMethod ::AllocAttributes( unsigned  long ulMethodMask)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(ulMethodMask & METHOD_CLASS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodClass",
										METHOD_CLASS_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		attrList.push_back(pDDlAttr);
	}


	if(ulMethodMask & METHOD_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodLabel",
										METHOD_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulMethodMask & METHOD_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodHelp",
										METHOD_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulMethodMask & METHOD_DEF)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodDefinition",
										METHOD_DEF_ID,
										DDL_ATTR_DATA_TYPE_DEFINITION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(ulMethodMask & METHOD_VALID)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodValidity",
										METHOD_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulMethodMask & METHOD_SCOPE)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodScope",
										METHOD_SCOPE_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	return;


}/* End AllocMethodAttributes*/


/*********************************************************************************
	
								REFRESH		ITEM	METHODS
		
/*********************************************************************************/




void DDlRefresh ::AllocAttributes(unsigned long ulRefreshMask)
{
	
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(ulRefreshMask & REFRESH_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("RefreshItems",
										REFRESH_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_REFRESH_RELATION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	return;


}/* End AllocRefreshAttributes*/


/*********************************************************************************
	
								UNIT	ITEM	METHODS
		
/*********************************************************************************/


void DDlUnit ::AllocAttributes(unsigned long ulUnitMask)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(ulUnitMask & UNIT_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("UnitItems",
										UNIT_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_UNIT_RELATION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	return;



}/* End AllocUnitAttributes */



/*********************************************************************************
	
								WAO		ITEM	METHODS
		
/*********************************************************************************/


void DDlWao ::AllocAttributes(unsigned long ulWaoMask)
{
	
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(ulWaoMask & WAO_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaoItems",
										WAO_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	return;

}/* End AllocWaoAttributes*/




/*********************************************************************************
	
								ITEM_ARRAY	ITEM	METHODS
		
/*********************************************************************************/


void DDlItemArray :: AllocAttributes(unsigned long ulItemArrayMask)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr=NULL;

	if(ulItemArrayMask & ITEM_ARRAY_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("ItemArrayLabel",
										ITEM_ARRAY_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulItemArrayMask & ITEM_ARRAY_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("ItemArrayHelp",
										ITEM_ARRAY_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(ulItemArrayMask & ITEM_ARRAY_ELEMENTS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("ItemArrayElements",
										ITEM_ARRAY_ELEMENTS_ID,
										DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	return;



}/* End AllocItemArrayAttributes */


/*********************************************************************************
	
								COLLECTION	ITEM	METHODS
		
/*********************************************************************************/

void DDlCollection ::AllocAttributes(unsigned long ulCollectionMask)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr=NULL;

	if(ulCollectionMask & COLLECTION_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionLabel",
										COLLECTION_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
		
	}

	if(ulCollectionMask & COLLECTION_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionHelp",
										COLLECTION_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
		
	}

	if(ulCollectionMask & COLLECTION_MEMBERS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionMembers",
										COLLECTION_MEMBERS_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	
	
	return;

}/*End AllocCollectionAttributes*/



/*********************************************************************************
	
								BLOCK	ITEM	METHODS
		
/*********************************************************************************/

void DDlBlock :: AllocAttributes(unsigned long ulBlockMask)
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr=NULL;

/*	if(ulBlockMask & BLOCK_CHARACTERISTIC)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockCharacteristic",
										BLOCK_CHARACTERISTIC_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
		
		attrList.push_back(pDDlAttr);
	}
*/

/*	if(ulBlockMask & BLOCK_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockLabel",
										BLOCK_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		
		attrList.push_back(pDDlAttr);

	}
*/

/*	if(ulBlockMask & BLOCK_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockHelp",
										BLOCK_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		
		attrList.push_back(pDDlAttr);

	}
*/
	
	if(ulBlockMask & BLOCK_PARAM)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockParams",
										BLOCK_PARAM_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

/*	if(ulBlockMask & BLOCK_MENU)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockMenus",
										BLOCK_MENU_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(ulBlockMask & BLOCK_EDIT_DISP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockEditDisplays",
										BLOCK_EDIT_DISP_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(ulBlockMask & BLOCK_METHOD)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockMethods",
										BLOCK_METHOD_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(ulBlockMask & BLOCK_REFRESH)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockRefreshRelations",
										BLOCK_REFRESH_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(ulBlockMask & BLOCK_UNIT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockUnitRelations",
										BLOCK_UNIT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(ulBlockMask & BLOCK_WAO)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockWAORelations",
										BLOCK_WAO_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(ulBlockMask & BLOCK_COLLECT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockCollections",
										BLOCK_COLLECT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(ulBlockMask & BLOCK_ITEM_ARRAY)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockItemArrays",
										BLOCK_ITEM_ARRAY_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(ulBlockMask & BLOCK_PARAM_LIST)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockItemArrays",
										BLOCK_PARAM_LIST_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}
*/

	return ;

}/*End AllocBlockAttributes*/



/*********************************************************************************
	
								RECORD	ITEM	METHODS
		
/*********************************************************************************/

void DDlRecord ::AllocAttributes(unsigned long ulRecordMask)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr=NULL;

	if(ulRecordMask & RECORD_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionLabel",
										RECORD_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(ulRecordMask & RECORD_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionHelp",
										RECORD_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(ulRecordMask & RECORD_MEMBERS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionMembers",
										RECORD_MEMBERS_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	/*Vibhor 311003: With the solution for implementing Demunging , We'll be storing
	 Record as Collection Items , since both are same except for Response Codes,
	 We won't parse the RECORD_RESP_CODES at all.*/
	
/*	if(ulRecordMask & RECORD_RESP_CODES)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("RecordRespCodes",
										RECORD_RESP_CODES_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
	
		attrList.push_back(pDDlAttr);
	
	} */

	return;

}/*End AllocRecordAttributes */