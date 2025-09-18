#pragma warning (disable : 4786)
#include "pch.h"
//sjv06feb06 #include <windows.h>
#include "ddbGeneral.h"

#include "DDl6Items.h"
#include "DD_Header.h"
#include "ParserEndian.h"
#include "Retn_Code.h"
#include "fetch_item.h"
// extern void panic(char *format,...);
#include "panic.h"
#include <assert.h>

const unsigned char maskSizes[] = { MskSzArr }; //Vibhor 170904 : Added

FLAT_UNION_T DDL6BaseItem::glblFlats;

/*********************************************************************************
	
								BASE  ITEM	METHODS
		
/*********************************************************************************/
int _preFetchItem(BYTE maskSize, BYTE** pObjExtn, /*INT*/ int& rSize, unsigned long &_attrMask)
{
	int retVal = 0;// success
	ITEM_EXTN* pItmExtn = (ITEM_EXTN*)(*pObjExtn);
	_attrMask   = 0L;


	UINT uiExtLength  = pItmExtn->byLength;
	
	(*pObjExtn) += sizeof(ITEM_EXTN); /*Point to the masks skip>len,type,subtype,id<*/

	for (int j = 0; j < maskSize; j++)
	{
		_attrMask = (_attrMask << 8) | (unsigned long)*((*pObjExtn)++);
	}

	rSize = uiExtLength - sizeof(ITEM_EXTN) + EXTEN_LENGTH_SIZE - maskSize; 
	/*If no attributes or the mask value is zero,means the DD is corrupt!! */
	if(rSize <= 0 || _attrMask == 0)
		  return (DDL_ENCODING_ERROR);
	
	return retVal;
}

int DDL6BaseItem::preFetchItem(DDlBaseItem* pBaseItm, BYTE maskSize, BYTE** pObjExtn, /*INT*/ int& rSize)
{
	ITEM_EXTN* pItmExtn = (ITEM_EXTN*)(*pObjExtn);
	bool byRetVal= read_dword(&(pBaseItm->id),&(pItmExtn->byItemId[0]),FORMAT_BIG_ENDIAN);
	if(!byRetVal)
		return (DDL_ENCODING_ERROR);		
	if (pBaseItm->id == 0L)	           // no id for an external object (leave mask 0)
		return (FETCH_EXTERNAL_OBJECT);// no need to go further

	int retVal = _preFetchItem(maskSize, pObjExtn, rSize, attrMask);

	memset((char *)(&glblFlats), 0, sizeof(FLAT_UNION_T));// clear the decks for action
	// note: there are a bunch of pointers in FLAT_UNION_T but no classes. memset OK

	return retVal;
}

/*********************************************************************************
	
								VARIABLE  ITEM	METHODS
		
/*********************************************************************************/


int DDl6Variable ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pVar != NULL);
	assert(pbyObjExtn[1] == VARIABLE_ITYPE);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pVar != NULL);
	ASSERT_DBG(pbyObjExtn[1] == VARIABLE_ITYPE);
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[VARIABLE_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  
		return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pVar->masks.bin_exists =  attrMask & VAR_ATTR_MASKS;
	pVar->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pVar,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pVar->masks.bin_hooked,   attach_var_data);

	return iRetVal;
}

void DDl6Variable ::AllocAttributes( void )
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;
	
	DDlVariable::AllocAttributes(attrMask);// version 5 alloc

	// then version 6 additions

	/*Vibhor 030904: Start of Code*/
	
	if(attrMask & VAR_DEFAULT_VALUE)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarDefaultVal",
										VAR_DEFAULT_VALUE_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	/*Vibhor 030904: End of Code*/

	/* stevev 10may05 */
	
	if(attrMask & VAR_REFRESH_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarRefreshActions",
										VAR_REFRESH_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

		
	if(attrMask & VAR_DEBUG)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarDebugData",
										VAR_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);//  db_debug_info	4 + 1
	
		attrList.push_back(pDDlAttr);
	
	}
//#ifdef XMTR	
	if(attrMask & VAR_POST_RQST_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarPostRequestActions",
										VAR_POST_RQST_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);	
		attrList.push_back(pDDlAttr);	
	}	

	if(attrMask & VAR_POST_USER_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarPostUserChangeActions",
										VAR_POST_USER_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);	
		attrList.push_back(pDDlAttr);	
	}
//#endif

	/* end stevev 10may05 */


	if(attrMask & VAR_TIME_FORMAT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarTimeFormat",
										VAR_TIME_FORMAT_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & VAR_TIME_SCALE)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("VarTimeScale",
										VAR_TIME_SCALE_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	/*Moved TYPE_SIZE attribute in to eval_variable*/

	/*Vibhor 141103 : Moving Handling to the beginning of the attribute list,
	So we will parse Handling in Eval as the first attribute*/		


	return ;

} /*End AllocVarAttributes */


void DDl6Variable ::clear_flat(void)
{
	if(pVar->depbin)
	{
		if(pVar->depbin->db_class)
			delete (pVar->depbin->db_class);
		if(pVar->depbin->db_display)
			delete (pVar->depbin->db_display);
		if(pVar->depbin->db_edit)
			delete (pVar->depbin->db_edit);
		if(pVar->depbin->db_enums)
			delete (pVar->depbin->db_enums);
		if(pVar->depbin->db_handling)
			delete (pVar->depbin->db_handling);
		if(pVar->depbin->db_help)
			delete (pVar->depbin->db_help);
		if(pVar->depbin->db_index_item_array)
			delete (pVar->depbin->db_index_item_array);
		if(pVar->depbin->db_label)
			delete (pVar->depbin->db_label);
		if(pVar->depbin->db_resp_codes)
			delete (pVar->depbin->db_resp_codes);
		if(pVar->depbin->db_type_size)
			delete (pVar->depbin->db_type_size);
		if(pVar->depbin->db_default_value)
			delete (pVar->depbin->db_default_value);
		delete(pVar->depbin);

	}/*Endif pVar->depbin*/

	if(pVar->misc)
	{
		if(pVar->misc->depbin)
		{
			if(pVar->misc->depbin->db_max_val)
				delete (pVar->misc->depbin->db_max_val);
			if(pVar->misc->depbin->db_min_val)
				delete (pVar->misc->depbin->db_min_val);
			//if(pVar->misc->depbin->db_read_time_out)
			//	delete (pVar->misc->depbin->db_read_time_out);
			//if(pVar->misc->depbin->db_write_time_out)
			//	delete (pVar->misc->depbin->db_write_time_out);
			if(pVar->misc->depbin->db_height)
				delete (pVar->misc->depbin->db_height);
			if(pVar->misc->depbin->db_width)
				delete (pVar->misc->depbin->db_width);
			if(pVar->misc->depbin->db_scale)
				delete (pVar->misc->depbin->db_scale);
			if(pVar->misc->depbin->db_unit)
				delete (pVar->misc->depbin->db_unit);
			if(pVar->misc->depbin->db_valid)
				delete (pVar->misc->depbin->db_valid);
			if(pVar->misc->depbin->db_debug_info)
				delete (pVar->misc->depbin->db_debug_info);
			if(pVar->misc->depbin->db_time_format)
				delete (pVar->misc->depbin->db_time_format);	// timj 4jan08
			if(pVar->misc->depbin->db_time_scale)
				delete (pVar->misc->depbin->db_time_scale);		// timj 4jan08
			delete (pVar->misc->depbin);
		}
		delete (pVar->misc);
	}
	if(pVar->actions)
	{
		if(pVar->actions->depbin)
		{
			if(pVar->actions->depbin->db_post_edit_act)
				delete (pVar->actions->depbin->db_post_edit_act);
			if(pVar->actions->depbin->db_post_read_act)
				delete (pVar->actions->depbin->db_post_read_act);
			if(pVar->actions->depbin->db_post_write_act)
				delete (pVar->actions->depbin->db_post_write_act);
			if(pVar->actions->depbin->db_pre_edit_act)
				delete (pVar->actions->depbin->db_pre_edit_act);
			if(pVar->actions->depbin->db_pre_read_act)
				delete (pVar->actions->depbin->db_pre_read_act);
			if(pVar->actions->depbin->db_pre_write_act)
				delete (pVar->actions->depbin->db_pre_write_act);
			if(pVar->actions->depbin->db_refresh_act)
				delete (pVar->actions->depbin->db_refresh_act);
			if(pVar->actions->depbin->db_post_rqst_act)
				delete (pVar->actions->depbin->db_post_rqst_act);
			if(pVar->actions->depbin->db_post_user_act)
				delete (pVar->actions->depbin->db_post_user_act);
			delete (pVar->actions->depbin);
		}
		delete (pVar->actions);
	}
	
	memset((char *)pVar, 0, sizeof(FLAT_VAR));
}

/*********************************************************************************
	
								COMMAND  ITEM	METHODS
		
/*********************************************************************************/


int DDl6Command ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pCmd != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pCmd != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[COMMAND_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pCmd->masks.bin_exists =  attrMask & COMMAND_ATTR_MASKS;
	pCmd->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pCmd,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pCmd->masks.bin_hooked,   attach_command_data);

	return iRetVal;
}

void DDl6Command :: AllocAttributes( void)
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(attrMask & COMMAND_NUMBER)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdNumber",
										COMMAND_NUMBER_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & COMMAND_OPER)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdOperationType",
										COMMAND_OPER_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & COMMAND_TRANS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdTransaction",
										COMMAND_TRANS_ID,
										DDL_ATTR_DATA_TYPE_TRANSACTION_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & COMMAND_RESP_CODES)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdResponseCodes",
										COMMAND_RESP_CODES_ID,
										DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST,
										false);	
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & COMMAND_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("CmdDebugData",
										COMMAND_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);	
		attrList.push_back(pDDlAttr);	
	}



	return;

} /* End AllocCmdAttributes*/


void DDl6Command :: clear_flat(void)
{
	if(pCmd->depbin)
	{
		if(pCmd->depbin->db_number)
			delete (pCmd->depbin->db_number);
		if(pCmd->depbin->db_oper)
			delete (pCmd->depbin->db_oper);
		if(pCmd->depbin->db_resp_codes)
			delete (pCmd->depbin->db_resp_codes);
		if(pCmd->depbin->db_trans)
			delete (pCmd->depbin->db_trans);
		if(pCmd->depbin->db_debug_info)
			delete (pCmd->depbin->db_debug_info);

		delete (pCmd->depbin);
	}
	memset((char *)pCmd, 0, sizeof(FLAT_COMMAND));
}


/*********************************************************************************
	
								MENU	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Menu ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pMenu != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pMenu != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[MENU_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pMenu->masks.bin_exists =  attrMask & MENU_ATTR_MASKS;
	pMenu->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pMenu,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pMenu->masks.bin_hooked,   attach_menu_data);

	return iRetVal;
}


void DDl6Menu :: AllocAttributes( void)
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(attrMask & MENU_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MenuLabel",
										MENU_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(attrMask & MENU_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MenuItems",
										MENU_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	if(attrMask & MENU_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MenuHelp",
										MENU_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & MENU_VALID)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MenuValidity",
										MENU_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & MENU_STYLE)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MenuStyle",
										MENU_STYLE_ID,
										DDL_ATTR_DATA_TYPE_MENU_STYLE,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & MENU_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("MenuDebugData",
										MENU_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);	
		attrList.push_back(pDDlAttr);	
	}

	return;

}/* End AllocMenuAttributes*/


void DDl6Menu :: clear_flat( void)
{

	if(pMenu->depbin)
	{
		if(pMenu->depbin->db_items)
			delete (pMenu->depbin->db_items);
		if(pMenu->depbin->db_label)
			delete (pMenu->depbin->db_label);
		if(pMenu->depbin->db_help)
			delete (pMenu->depbin->db_help);
		if(pMenu->depbin->db_valid)
			delete (pMenu->depbin->db_valid);
		if(pMenu->depbin->db_style)
			delete (pMenu->depbin->db_style);
		if(pMenu->depbin->db_debug_info)
			delete (pMenu->depbin->db_debug_info);

		delete (pMenu->depbin);
	}

	memset((char *)pMenu, 0, sizeof(FLAT_MENU));

}

/*********************************************************************************
	
								EDIT_DISPLAY	ITEM	METHODS
		
/*********************************************************************************/

int DDl6EditDisplay ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pEdDisp != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pEdDisp != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[EDIT_DISP_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pEdDisp->masks.bin_exists =  attrMask & EDIT_DISP_ATTR_MASKS;
	pEdDisp->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pEdDisp,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pEdDisp->masks.bin_hooked,   attach_edit_disp_data);

	return iRetVal;
}


void DDl6EditDisplay :: AllocAttributes( void)
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;


	if(attrMask & EDIT_DISPLAY_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispLabel",
										EDIT_DISPLAY_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(attrMask & EDIT_DISPLAY_EDIT_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispEditItems",
										EDIT_DISPLAY_EDIT_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & EDIT_DISPLAY_DISP_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispDisplayItems",
										EDIT_DISPLAY_DISP_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & EDIT_DISPLAY_PRE_EDIT_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispPreEditActions",
										EDIT_DISPLAY_PRE_EDIT_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & EDIT_DISPLAY_POST_EDIT_ACT)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispPostEditActions",
										EDIT_DISPLAY_POST_EDIT_ACT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	
	if(attrMask & EDIT_DISPLAY_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispHelp",
										EDIT_DISPLAY_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & EDIT_DISPLAY_VALID)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispValidity",
										EDIT_DISPLAY_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & EDIT_DISPLAY_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("EditDispDebugData",
										EDIT_DISPLAY_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);	
		attrList.push_back(pDDlAttr);	
	}
	

	return;

}/* End AllocEditDispAttributes*/



void DDl6EditDisplay :: clear_flat( void)
{
	if(pEdDisp->depbin)
	{
		if(pEdDisp->depbin->db_disp_items)
			delete (pEdDisp->depbin->db_disp_items);
		if(pEdDisp->depbin->db_edit_items)
			delete (pEdDisp->depbin->db_edit_items);
		if(pEdDisp->depbin->db_label)
			delete (pEdDisp->depbin->db_label);
		if(pEdDisp->depbin->db_post_edit_act)
			delete (pEdDisp->depbin->db_post_edit_act);
		if(pEdDisp->depbin->db_pre_edit_act)
			delete (pEdDisp->depbin->db_pre_edit_act);
		if(pEdDisp->depbin->db_help)
			delete (pEdDisp->depbin->db_help);
		if(pEdDisp->depbin->db_valid)
			delete (pEdDisp->depbin->db_valid);
		if(pEdDisp->depbin->db_debug_info)
			delete (pEdDisp->depbin->db_debug_info);
		delete (pEdDisp->depbin);
	}
	memset((char *)pEdDisp, 0, sizeof(FLAT_EDIT_DISPLAY));
}

/*********************************************************************************
	
								METHOD		ITEM	METHODS
		
/*********************************************************************************/


int DDl6Method ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pMthd != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pMthd != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[METHOD_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pMthd->masks.bin_exists =  attrMask & METHOD_ATTR_MASKS;
	pMthd->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pMthd,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pMthd->masks.bin_hooked,   attach_method_data);

	return iRetVal;
}


void DDl6Method ::AllocAttributes( void)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(attrMask & METHOD_CLASS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodClass",
										METHOD_CLASS_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		attrList.push_back(pDDlAttr);
	}


	if(attrMask & METHOD_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodLabel",
										METHOD_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & METHOD_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodHelp",
										METHOD_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & METHOD_DEF)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodDefinition",
										METHOD_DEF_ID,
										DDL_ATTR_DATA_TYPE_DEFINITION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(attrMask & METHOD_VALID)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodValidity",
										METHOD_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & METHOD_SCOPE)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodScope",
										METHOD_SCOPE_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & METHOD_TYPE)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodType",
										METHOD_TYPE_ID,
										DDL_ATTR_DATA_TYPE_PARAM,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & METHOD_PARAMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodParameters",
										METHOD_PARAMS_ID,
										DDL_ATTR_DATA_TYPE_PARAM_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & METHOD_DEBUG)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("MethodDebugData",
										METHOD_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}



	return;


}/* End AllocMethodAttributes*/


void DDl6Method :: clear_flat( void)
{
	if(pMthd->depbin)
	{
		if(pMthd->depbin->db_class)
			delete (pMthd->depbin->db_class);
		if(pMthd->depbin->db_def)
			delete (pMthd->depbin->db_def);
		if(pMthd->depbin->db_help)
			delete (pMthd->depbin->db_help);
		if(pMthd->depbin->db_label)
			delete (pMthd->depbin->db_label);
		if(pMthd->depbin->db_scope)
			delete (pMthd->depbin->db_scope);
		if(pMthd->depbin->db_valid)
			delete (pMthd->depbin->db_valid);
		if(pMthd->depbin->db_type)
			delete (pMthd->depbin->db_type);
		if(pMthd->depbin->db_params)
			delete (pMthd->depbin->db_params);
		if(pMthd->depbin->db_debug_info)
			delete (pMthd->depbin->db_debug_info);
		delete (pMthd->depbin);
	}

	memset((char *)pMthd, 0, sizeof(FLAT_METHOD));
}
/*********************************************************************************
	
								REFRESH		ITEM	METHODS
		
/*********************************************************************************/


int DDl6Refresh ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pRfsh != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pRfsh != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[REFRESH_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pRfsh->masks.bin_exists =  attrMask & REFRESH_ATTR_MASKS;
	pRfsh->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pRfsh,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pRfsh->masks.bin_hooked,   attach_refresh_data);

	return iRetVal;
}


void DDl6Refresh ::AllocAttributes(void)
{
	
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(attrMask & REFRESH_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("RefreshItems",
										REFRESH_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_REFRESH_RELATION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	if(attrMask & REFRESH_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("RefreshDebugData",
										REFRESH_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);	
		attrList.push_back(pDDlAttr);	
	}


	return;


}/* End AllocRefreshAttributes*/


void DDl6Refresh :: clear_flat( void)
{	
	if(pRfsh->depbin)
	{
		if(pRfsh->depbin->db_items)
			delete (pRfsh->depbin->db_items);
		if(pRfsh->depbin->db_debug_info)
			delete (pRfsh->depbin->db_debug_info);
		delete (pRfsh->depbin);
	}

	memset((char *)pRfsh, 0, sizeof(FLAT_REFRESH));

}


/*********************************************************************************
	
								UNIT	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Unit ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pUnit != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pUnit != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[UNIT_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pUnit->masks.bin_exists =  attrMask & UNIT_ATTR_MASKS;
	pUnit->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pUnit,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pUnit->masks.bin_hooked,   attach_unit_data);

	return iRetVal;
}


void DDl6Unit ::AllocAttributes(void)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(attrMask & UNIT_ITEMS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("UnitItems",
										UNIT_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_UNIT_RELATION,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & UNIT_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("UnitDebugData",
										UNIT_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);	
		attrList.push_back(pDDlAttr);	
	}


	return;



}/* End AllocUnitAttributes */


void DDl6Unit :: clear_flat( void)
{
	if(pUnit->depbin)
	{
		if(pUnit->depbin->db_items)
			delete (pUnit->depbin->db_items);
		if(pUnit->depbin->db_debug_info)
			delete (pUnit->depbin->db_debug_info);
		delete (pUnit->depbin);
	}

	memset((char *)pUnit, 0, sizeof(FLAT_UNIT));
}
/*********************************************************************************
	
								WAO		ITEM	METHODS
		
/*********************************************************************************/

int DDl6Wao ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pWao != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pWao != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[WAO_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pWao->masks.bin_exists =  attrMask & WAO_ATTR_MASKS;
	pWao->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pWao,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pWao->masks.bin_hooked,   attach_wao_data);

	return iRetVal;
}


void DDl6Wao ::AllocAttributes(void)
{
	
	//Modified by Deepak
	DDlAttribute *pDDlAttr =NULL;

	if(attrMask & WAO_ITEMS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaoItems",
										WAO_ITEMS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);	
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAO_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaoDebugData",
										WAO_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);	
		attrList.push_back(pDDlAttr);	
	}


	return;

}/* End AllocWaoAttributes*/



void DDl6Wao :: clear_flat( void)
{	
	if(pWao->depbin)
	{
		if(pWao->depbin->db_items)
			delete (pWao->depbin->db_items);
		if(pWao->depbin->db_debug_info)
			delete (pWao->depbin->db_debug_info);
		delete (pWao->depbin);
	}

	memset((char *)pWao, 0, sizeof(FLAT_WAO));
}


/*********************************************************************************
	
								ITEM_ARRAY	ITEM	METHODS
		
/*********************************************************************************/

int DDl6ItemArray ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pItmArr != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pItmArr != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[ITEM_ARRAY_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pItmArr->masks.bin_exists =  attrMask & ITEM_ARRAY_ATTR_MASKS;
	pItmArr->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pItmArr,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pItmArr->masks.bin_hooked,   attach_item_array_data);

	return iRetVal;
}


void DDl6ItemArray :: AllocAttributes(void)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & ITEM_ARRAY_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("ItemArrayLabel",
										ITEM_ARRAY_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & ITEM_ARRAY_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("ItemArrayHelp",
										ITEM_ARRAY_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}


	if(attrMask & ITEM_ARRAY_ELEMENTS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("ItemArrayElements",
										ITEM_ARRAY_ELEMENTS_ID,
										DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	
	if(attrMask & ITEM_ARRAY_VALIDITY)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("ItemArrayValidity",
										ITEM_ARRAY_VALIDITY_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	
	if(attrMask & ITEM_ARRAY_DEBUG)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("ItemArrayDebugData",
										ITEM_ARRAY_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}
	


	return;



}/* End AllocItemArrayAttributes */


void DDl6ItemArray :: clear_flat( void)
{	
	if(pItmArr->depbin)
	{
		if(pItmArr->depbin->db_elements)
			delete (pItmArr->depbin->db_elements);
		if(pItmArr->depbin->db_help)
			delete (pItmArr->depbin->db_help);
		if(pItmArr->depbin->db_label)
			delete (pItmArr->depbin->db_label);
		if(pItmArr->depbin->db_valid)
			delete (pItmArr->depbin->db_valid);
		if(pItmArr->depbin->db_debug_info)
			delete (pItmArr->depbin->db_debug_info);
		delete (pItmArr->depbin);
	}

	memset((char *)pItmArr, 0, sizeof(FLAT_ITEM_ARRAY));
}

/*********************************************************************************
	
								COLLECTION	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Collection ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pColl != NULL);
	assert(pbyObjExtn[1] == byItemType);
#else
	ASSERT_DBG(pbyObjExtn != NULL && pColl != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[COLLECTION_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pColl->masks.bin_exists =  attrMask & COLLECTION_ATTR_MASKS;
	pColl->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pColl,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pColl->masks.bin_hooked,   attach_collection_data);

	return iRetVal;
}


void DDl6Collection ::AllocAttributes(void)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & COLLECTION_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionLabel",
										COLLECTION_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
		
	}

	if(attrMask & COLLECTION_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionHelp",
										COLLECTION_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
		
	}

	if(attrMask & COLLECTION_VALIDITY)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionValidity",
										COLLECTION_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & COLLECTION_MEMBERS)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionMembers",
										COLLECTION_MEMBERS_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & COLLECTION_DEBUG)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionDebugData",
										COLLECTION_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

		

	return;

}/*End AllocCollectionAttributes*/



void DDl6Collection :: clear_flat( void)
{	
	if(pColl->depbin)
	{
		if(pColl->depbin->db_help)
			delete (pColl->depbin->db_help);
		if(pColl->depbin->db_label)
			delete (pColl->depbin->db_label);
		if(pColl->depbin->db_valid)
			delete (pColl->depbin->db_valid);
		if(pColl->depbin->db_members)
			delete (pColl->depbin->db_members);
		if(pColl->depbin->db_debug_info)
			delete (pColl->depbin->db_debug_info);
		delete (pColl->depbin);
	}

	memset((char *)pColl, 0, sizeof(FLAT_COLLECTION));
}

/*********************************************************************************
	
								RECORD	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Record ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pRec != NULL);
	assert(pbyObjExtn[1] == byItemType);
//#else
//	ASSERT_DBG(pbyObjExtn != NULL && pRec != NULL);
//	ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[RECORD_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pRec->masks.bin_exists =  attrMask & RECORD_ATTR_MASKS;
	pRec->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pRec,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pRec->masks.bin_hooked,   attach_record_data);

	return iRetVal;
}

void DDl6Record ::AllocAttributes(void)
{

	//Modified by Deepak
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & RECORD_LABEL)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionLabel",
										RECORD_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	
	if(attrMask & RECORD_HELP)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("CollectionHelp",
										RECORD_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
		attrList.push_back(pDDlAttr);
	
	}

	if(attrMask & RECORD_MEMBERS)
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
	
/*	if(attrMask & RECORD_RESP_CODES)
	{

		pDDlAttr = (DDlAttribute*)new DDlAttribute("RecordRespCodes",
										RECORD_RESP_CODES_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
	
		attrList.push_back(pDDlAttr);
	
	} */


	return;

}/*End AllocRecordAttributes */


void DDl6Record :: clear_flat( void)
{	
	if(pRec->depbin)
	{
		if(pRec->depbin->db_help)
			delete (pRec->depbin->db_help);
		if(pRec->depbin->db_label)
			delete (pRec->depbin->db_label);
		if(pRec->depbin->db_members)
			delete (pRec->depbin->db_members);
		if(pRec->depbin->db_resp_codes)
			delete (pRec->depbin->db_resp_codes);
		delete (pRec->depbin);
	}

	memset((char *)pRec, 0, sizeof(FLAT_RECORD));
}


/*********************************************************************************
	
								BLOCK	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Block ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pBlk != NULL);
	assert(pbyObjExtn[1] == byItemType);
//#else
	/*ASSERT_DBG(pbyObjExtn != NULL && pBlk != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[BLOCK_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pBlk->masks.bin_exists =  attrMask & BLOCK_ATTR_MASKS;
	pBlk->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pBlk,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pBlk->masks.bin_hooked,   attach_block_data);

	return iRetVal;
}

void DDl6Block :: AllocAttributes()
{
	//Modified by Deepak
	DDlAttribute *pDDlAttr=NULL;

/*	if(attrMask & BLOCK_CHARACTERISTIC)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockCharacteristic",
										BLOCK_CHARACTERISTIC_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
		
		attrList.push_back(pDDlAttr);
	}
*/

/*	if(attrMask & BLOCK_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockLabel",
										BLOCK_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		
		attrList.push_back(pDDlAttr);

	}
*/

/*	if(attrMask & BLOCK_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockHelp",
										BLOCK_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		
		attrList.push_back(pDDlAttr);

	}
*/
	
	if(attrMask & BLOCK_PARAM)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockParams",
										BLOCK_PARAM_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

/*	if(attrMask & BLOCK_MENU)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockMenus",
										BLOCK_MENU_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(attrMask & BLOCK_EDIT_DISP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockEditDisplays",
										BLOCK_EDIT_DISP_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(attrMask & BLOCK_METHOD)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockMethods",
										BLOCK_METHOD_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(attrMask & BLOCK_REFRESH)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockRefreshRelations",
										BLOCK_REFRESH_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(attrMask & BLOCK_UNIT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockUnitRelations",
										BLOCK_UNIT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(attrMask & BLOCK_WAO)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockWAORelations",
										BLOCK_WAO_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(attrMask & BLOCK_COLLECT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockCollections",
										BLOCK_COLLECT_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(attrMask & BLOCK_ITEM_ARRAY)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("BlockItemArrays",
										BLOCK_ITEM_ARRAY_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		
		attrList.push_back(pDDlAttr);

	}

	if(attrMask & BLOCK_PARAM_LIST)
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


void DDl6Block :: clear_flat( void)
{	
	if(pBlk->depbin)
	{
		if(pBlk->depbin->db_characteristic)
			delete(pBlk->depbin->db_characteristic);
		if(pBlk->depbin->db_collect)
			delete(pBlk->depbin->db_collect);
		if(pBlk->depbin->db_edit_disp)
			delete(pBlk->depbin->db_edit_disp);
		if(pBlk->depbin->db_help)
			delete(pBlk->depbin->db_help);
		if(pBlk->depbin->db_item_array)
			delete(pBlk->depbin->db_item_array);
		if(pBlk->depbin->db_label)
			delete(pBlk->depbin->db_label);
		if(pBlk->depbin->db_menu)
			delete(pBlk->depbin->db_menu);
		if(pBlk->depbin->db_method)
			delete(pBlk->depbin->db_method);
		if(pBlk->depbin->db_param)
			delete(pBlk->depbin->db_param);
		if(pBlk->depbin->db_param_list)
			delete(pBlk->depbin->db_param_list);
		if(pBlk->depbin->db_refresh)
			delete(pBlk->depbin->db_refresh);
		if(pBlk->depbin->db_unit)
			delete(pBlk->depbin->db_unit);
		if(pBlk->depbin->db_wao)
			delete(pBlk->depbin->db_wao);

		delete (pBlk->depbin);
	}
	memset((char *)pBlk, 0, sizeof(FLAT_BLOCK));
}


/*********************************************************************************
	
								ARRAY	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Array ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{

#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pArr != NULL);
	assert(pbyObjExtn[1] == byItemType);
	//#else
	/*ASSERT_DBG(pbyObjExtn != NULL && pArr != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[ARRAY_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pArr->masks.bin_exists =  attrMask & ARRAY_ATTR_MASKS;
	pArr->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pArr,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pArr->masks.bin_hooked,   attach_array_data);

	return iRetVal;
}/*End DDl6Array ::fetch_item*/


void DDl6Array :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & ARRAY_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ArrayLabel",
										ARRAY_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & ARRAY_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ArrayHelp",
										ARRAY_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & ARRAY_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ArrayValidity",
										ARRAY_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & ARRAY_TYPE)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ArrayType",
										ARRAY_TYPE_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & ARRAY_NUM_OF_ELEMENTS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ArrayLength",
										ARRAY_NUM_OF_ELEMENTS_ID,
										DDL_ATTR_DATA_TYPE_INT,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & ARRAY_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ArrayDebugData",
										ARRAY_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}

	return;

}/* End DDl6Array :: AllocAttributes */


void DDl6Array :: clear_flat( void)
{	
	if(pArr->depbin)
	{
		if(pArr->depbin->db_num_of_elements)
			delete (pArr->depbin->db_num_of_elements);
		if(pArr->depbin->db_help)
			delete (pArr->depbin->db_help);
		if(pArr->depbin->db_label)
			delete (pArr->depbin->db_label);
		if(pArr->depbin->db_valid)
			delete (pArr->depbin->db_valid);
		if(pArr->depbin->db_type)
			delete (pArr->depbin->db_type);
		if(pArr->depbin->db_debug_info)
			delete (pArr->depbin->db_debug_info);
		delete (pArr->depbin);
	}

	memset((char *)pArr, 0, sizeof(FLAT_ARRAY));
}/*End DDl6Array :: clear_flat*/




/*********************************************************************************
	
								CHART	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Chart ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pChart != NULL);
	assert(pbyObjExtn[1] == byItemType);
	//#else
	//ASSERT_DBG(pbyObjExtn != NULL && pChart != NULL);
	//ASSERT_DBG(pbyObjExtn[1] == byItemType);
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[CHART_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pChart->masks.bin_exists =  attrMask & CHART_ATTR_MASKS;
	pChart->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pChart,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pChart->masks.bin_hooked,   attach_chart_data);

	return iRetVal;
}/*End DDl6Chart ::fetch_item*/


void DDl6Chart :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & CHART_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartLabel",
										CHART_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & CHART_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartHelp",
										CHART_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & CHART_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartValidity",
										CHART_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & CHART_HEIGHT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartHeight",
										CHART_HEIGHT_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & CHART_WIDTH)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartWidth",
										CHART_WIDTH_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		attrList.push_back(pDDlAttr);	
	}	

	if(attrMask & CHART_TYPE)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartType",
										CHART_TYPE_ID,
										DDL_ATTR_DATA_TYPE_CHART_TYPE,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & CHART_LENGTH)//expr
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartLength",
										CHART_LENGTH_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}
	
	
		

	if(attrMask & CHART_CYCLETIME)//expr
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartCycleTime",
										CHART_CYCLETIME_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & CHART_MEMBERS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartMembers",
										CHART_MEMBERS_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & CHART_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ChartDebugData",
										CHART_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}

	return;

}/* End DDl6Chart :: AllocAttributes */


void DDl6Chart :: clear_flat( void)
{	
	if(pChart->depbin)
	{
		if(pChart->depbin->db_label)
			delete (pChart->depbin->db_label);
		if(pChart->depbin->db_help)
			delete (pChart->depbin->db_help);
		if(pChart->depbin->db_valid)
			delete (pChart->depbin->db_valid);
		if(pChart->depbin->db_height)
			delete (pChart->depbin->db_height);
		if(pChart->depbin->db_width)
			delete (pChart->depbin->db_width);
		if(pChart->depbin->db_type)
			delete (pChart->depbin->db_type);
		if(pChart->depbin->db_length)
			delete (pChart->depbin->db_length);
		if(pChart->depbin->db_cytime)
			delete (pChart->depbin->db_cytime);
		if(pChart->depbin->db_members)
			delete (pChart->depbin->db_members);
		if(pChart->depbin->db_debug_info)
			delete (pChart->depbin->db_debug_info);
		delete (pChart->depbin);
	}

	memset((char *)pChart, 0, sizeof(FLAT_CHART));
}/*End DDl6Chart :: clear_flat*/




/*********************************************************************************
	
								GRAPH	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Graph ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pGraph != NULL);
	assert(pbyObjExtn[1] == byItemType);
	//#else
	/*ASSERT_DBG(pbyObjExtn != NULL && pGraph != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif
	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[GRAPH_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pGraph->masks.bin_exists =  attrMask & GRAPH_ATTR_MASKS;
	pGraph->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pGraph,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pGraph->masks.bin_hooked,   attach_graph_data);

	return iRetVal;
}/*End DDl6Graph ::fetch_item*/


void DDl6Graph :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & GRAPH_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphLabel",
										GRAPH_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRAPH_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphHelp",
										GRAPH_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRAPH_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphValidity",
										GRAPH_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRAPH_HEIGHT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphHeight",
										GRAPH_HEIGHT_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & GRAPH_WIDTH)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphWidth",
										GRAPH_WIDTH_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		attrList.push_back(pDDlAttr);	
	}	

	if(attrMask & GRAPH_XAXIS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphXaxis",
										GRAPH_XAXIS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & GRAPH_MEMBERS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphMembers",
										GRAPH_MEMBERS_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}	

	if(attrMask & GRAPH_CYCLETIME)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphCycleTime",
										GRAPH_CYCLETIME_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRAPH_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GraphDebugData",
										GRAPH_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}

	return;

}/* End DDl6Graph :: AllocAttributes */


void DDl6Graph :: clear_flat( void)
{	
	if(pGraph->depbin)
	{
		if(pGraph->depbin->db_label)
			delete (pGraph->depbin->db_label);
		if(pGraph->depbin->db_help)
			delete (pGraph->depbin->db_help);
		if(pGraph->depbin->db_valid)
			delete (pGraph->depbin->db_valid);
		if(pGraph->depbin->db_height)
			delete (pGraph->depbin->db_height);
		if(pGraph->depbin->db_width)
			delete (pGraph->depbin->db_width);
		if(pGraph->depbin->db_x_axis)
			delete (pGraph->depbin->db_x_axis);
		if(pGraph->depbin->db_members)
			delete (pGraph->depbin->db_members);
		if(pGraph->depbin->db_cytime)
			delete (pGraph->depbin->db_cytime);
		if(pGraph->depbin->db_debug_info)
			delete (pGraph->depbin->db_debug_info);
		delete (pGraph->depbin);
	}

	memset((char *)pGraph, 0, sizeof(FLAT_GRAPH));
}/*End DDl6Graph :: clear_flat*/


//Vibhor 260804: Start of Code

/*********************************************************************************
	
								AXIS	ITEM	METHODS
		
/*********************************************************************************/


int DDl6Axis ::fetch_item(BYTE *pbyObjExtn,OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pAxis != NULL);
	assert(pbyObjExtn[1] == byItemType);
	/*ASSERT_DBG(pbyObjExtn != NULL && pAxis != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif
	

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[AXIS_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pAxis->masks.bin_exists =  attrMask & AXIS_ATTR_MASKS;
	
	//pAxis->masks.bin_exists =  attrMask & AXIS_ATTR_MASKS;
	pAxis->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pAxis,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pAxis->masks.bin_hooked,   attach_axis_data);

	return iRetVal;


}/*End DDl6Axis ::fetch_item*/


void DDl6Axis :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & AXIS_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("AxisLabel",
										AXIS_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & AXIS_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("AxisHelp",
										AXIS_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & AXIS_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("AxisValidity",
										AXIS_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & AXIS_MINVAL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("AxisMinVal",
										AXIS_MINVAL_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & AXIS_MAXVAL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("AxisMaxVal",
										AXIS_MAXVAL_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}	

	if(attrMask & AXIS_SCALING)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("AxisScaling",
										AXIS_SCALING_ID,
										DDL_ATTR_DATA_TYPE_INT,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & AXIS_CONSTUNIT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("AxisConstantUnit",
										AXIS_CONSTUNIT_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & AXIS_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("AxisDebugData",
										AXIS_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}

	return;

}/* End DDl6Axis :: AllocAttributes */


void DDl6Axis :: clear_flat( void)
{	
	if(pAxis->depbin)
	{
		if(pAxis->depbin->db_label)
			delete (pAxis->depbin->db_label);
		if(pAxis->depbin->db_help)
			delete (pAxis->depbin->db_help);
		if(pAxis->depbin->db_valid)
			delete (pAxis->depbin->db_valid);
		if(pAxis->depbin->db_minval)
			delete (pAxis->depbin->db_minval);
		if(pAxis->depbin->db_maxval)
			delete (pAxis->depbin->db_maxval);
		if(pAxis->depbin->db_scaling)
			delete (pAxis->depbin->db_scaling);
		if(pAxis->depbin->db_unit)
			delete (pAxis->depbin->db_unit);
		if(pAxis->depbin->db_debug_info)
			delete (pAxis->depbin->db_debug_info);
		delete (pAxis->depbin);
	}

	memset((char *)pAxis, 0, sizeof(FLAT_AXIS));
}/*End DDl6Axis :: clear_flat*/


/*********************************************************************************
	
								FILE	ITEM	METHODS
		
/*********************************************************************************/

int DDl6File ::fetch_item(BYTE *pbyObjExtn,OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pFile != NULL);
	assert(pbyObjExtn[1] == byItemType);
	/*ASSERT_DBG(pbyObjExtn != NULL && pFile != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif
	

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[FILE_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pFile->masks.bin_exists =  attrMask & FILE_ATTR_MASKS;
	
	//pAxis->masks.bin_exists =  attrMask & AXIS_ATTR_MASKS;
	pFile->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pFile,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pFile->masks.bin_hooked,   attach_file_data);

	return iRetVal;


}/*End DDl6File ::fetch_item*/


void DDl6File :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;
	
	if(attrMask & FILE_MEMBERS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("FileMembers",
										FILE_MEMBERS_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	

	if(attrMask & FILE_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("FileLabel",
										FILE_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & FILE_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("FileHelp",
										FILE_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & FILE_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("FileDebugData",
										FILE_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}

	

	return;

}/* End DDl6File :: AllocAttributes */


void DDl6File :: clear_flat( void)
{	
	if(pFile->depbin)
	{
		if(pFile->depbin->db_members)
			delete (pFile->depbin->db_members);
		if(pFile->depbin->db_label)
			delete (pFile->depbin->db_label);
		if(pFile->depbin->db_help)
			delete (pFile->depbin->db_help);
		if(pFile->depbin->db_debug_info)
			delete (pFile->depbin->db_debug_info);
		delete (pFile->depbin);
	}

	memset((char *)pFile, 0, sizeof(FLAT_FILE));
}/*End DDl6File :: clear_flat*/


/*********************************************************************************
	
								WAVEFORM	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Waveform ::fetch_item(BYTE *pbyObjExtn,OBJECT_INDEX objIndex)
{

#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pWaveFrm != NULL);
	assert(pbyObjExtn[1] == byItemType);
	/*ASSERT_DBG(pbyObjExtn != NULL && pWaveFrm != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif
	

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this,maskSizes[WAVEFORM_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pWaveFrm->masks.bin_exists =  attrMask & WAVEFORM_ATTR_MASKS;
	
	//pAxis->masks.bin_exists =  attrMask & AXIS_ATTR_MASKS;
	pWaveFrm->id = id;
	iRetVal  = get_item_attr(objIndex, attrMask, pWaveFrm,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pWaveFrm->masks.bin_hooked,   attach_waveform_data);

	return iRetVal;


}/*End DDl6File ::fetch_item*/

 
void DDl6Waveform :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;
	
    if(attrMask & WAVEFORM_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformLabel",
										WAVEFORM_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformHelp",
										WAVEFORM_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}
	
	if(attrMask & WAVEFORM_HANDLING)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformHandling",
										WAVEFORM_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_EMPHASIS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformEmphasis",
										WAVEFORM_EMPHASIS_ID,
										DDL_ATTR_DATA_TYPE_INT,	//Assuming bool will be encoded as int only
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_LINETYPE)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformLineType",
										WAVEFORM_LINETYPE_ID,
										DDL_ATTR_DATA_TYPE_LINE_TYPE,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_LINECOLOR)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformLineColor",
										WAVEFORM_LINECOLOR_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}
	
	if(attrMask & WAVEFORM_YAXIS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformYAxis",
										WAVEFORM_YAXIS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_KEYPTS_X)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformKeyPointsX",
										WAVEFORM_KEYPTS_X_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_KEYPTS_Y)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformKeyPointsY",
										WAVEFORM_KEYPTS_Y_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_TYPE)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformType",
										WAVEFORM_TYPE_ID,
										DDL_ATTR_DATA_TYPE_WAVEFORM_TYPE,
										false);
		attrList.push_back(pDDlAttr);	
	}
	
	if(attrMask & WAVEFORM_X_VALUES)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformXVals",
										WAVEFORM_X_VALUES_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_Y_VALUES)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformYVals",
										WAVEFORM_Y_VALUES_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_X_INITIAL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformXInitial",
										WAVEFORM_X_INITIAL_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_X_INCREMENT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformXIncr",
										WAVEFORM_X_INCREMENT_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}
	
	if(attrMask & WAVEFORM_POINT_COUNT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformPtCnt",
										WAVEFORM_POINT_COUNT_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_INIT_ACTIONS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformInitActions",
										WAVEFORM_INIT_ACTIONS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_RFRSH_ACTIONS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformRfrshActions",
										WAVEFORM_RFRSH_ACTIONS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_EXIT_ACTIONS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformExitActions",
										WAVEFORM_EXIT_ACTIONS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformDebugData",
										WAVEFORM_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & WAVEFORM_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("WaveformValidity",
										WAVEFORM_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	return;

}/* End DDl6File :: AllocAttributes */


void DDl6Waveform :: clear_flat( void)
{	
	if(pWaveFrm->depbin)
	{
		if(pWaveFrm->depbin->db_label)
			delete (pWaveFrm->depbin->db_label);
		if(pWaveFrm->depbin->db_help)
			delete (pWaveFrm->depbin->db_help);
		if(pWaveFrm->depbin->db_handling)
			delete (pWaveFrm->depbin->db_handling);
		if(pWaveFrm->depbin->db_emphasis)
			delete (pWaveFrm->depbin->db_emphasis);
		if(pWaveFrm->depbin->db_linetype)
			delete (pWaveFrm->depbin->db_linetype);
		if(pWaveFrm->depbin->db_linecolor)
			delete (pWaveFrm->depbin->db_linecolor);
		if(pWaveFrm->depbin->db_y_axis)
			delete (pWaveFrm->depbin->db_y_axis);
		if(pWaveFrm->depbin->db_x_keypts)
			delete (pWaveFrm->depbin->db_x_keypts);
		if(pWaveFrm->depbin->db_y_keypts)
			delete (pWaveFrm->depbin->db_y_keypts);
		if(pWaveFrm->depbin->db_type)
			delete (pWaveFrm->depbin->db_type);
		if(pWaveFrm->depbin->db_x_values)
			delete (pWaveFrm->depbin->db_x_values);
		if(pWaveFrm->depbin->db_y_values)
			delete (pWaveFrm->depbin->db_y_values);
		if(pWaveFrm->depbin->db_x_initial)
			delete (pWaveFrm->depbin->db_x_initial);
		if(pWaveFrm->depbin->db_x_incr)
			delete (pWaveFrm->depbin->db_x_incr);
		if(pWaveFrm->depbin->db_pt_count)
			delete (pWaveFrm->depbin->db_pt_count);
		if(pWaveFrm->depbin->db_init_acts)
			delete (pWaveFrm->depbin->db_init_acts);
		if(pWaveFrm->depbin->db_rfrsh_acts)
			delete (pWaveFrm->depbin->db_rfrsh_acts);
		if(pWaveFrm->depbin->db_exit_acts)
			delete (pWaveFrm->depbin->db_exit_acts);
		if(pWaveFrm->depbin->db_debug_info)
			delete (pWaveFrm->depbin->db_debug_info);
		if(pWaveFrm->depbin->db_valid)
			delete (pWaveFrm->depbin->db_valid);
		
		delete (pWaveFrm->depbin);
	}

	memset((char *)pWaveFrm, 0, sizeof(FLAT_WAVEFORM));
}/*End DDl6File :: clear_flat*/
 

/*********************************************************************************
	
								LIST	ITEM	METHODS
		
/*********************************************************************************/

int DDl6List ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{

#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pList != NULL);
	assert(pbyObjExtn[1] == byItemType);
	/*ASSERT_DBG(pbyObjExtn != NULL && pList != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif
	

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[LIST_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pList->masks.bin_exists =  attrMask & LIST_ATTR_MASKS;
	pList->id = id;

	iRetVal  = get_item_attr(objIndex, attrMask, pList,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pList->masks.bin_hooked,   attach_list_data);

	return iRetVal;
}/*End DDl6List ::fetch_item*/


void DDl6List :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & LIST_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ListLabel",
										LIST_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & LIST_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ListHelp",
										LIST_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & LIST_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ListValidity",
										LIST_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & LIST_TYPE)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ListType",
										LIST_TYPE_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & LIST_COUNT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ListCount",
										LIST_COUNT_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & LIST_CAPACITY)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ListCapacity",
										LIST_CAPACITY_ID,
										DDL_ATTR_DATA_TYPE_INT,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & LIST_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ListDebugData",
										LIST_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}


	return;

}/* End DDl6List :: AllocAttributes */


void DDl6List :: clear_flat( void)
{	
	if(pList->depbin)
	{
		if(pList->depbin->db_label)
			delete (pList->depbin->db_label);
		if(pList->depbin->db_help)
			delete (pList->depbin->db_help);
		if(pList->depbin->db_valid)
			delete (pList->depbin->db_valid);
		if(pList->depbin->db_type)
			delete (pList->depbin->db_type);
		if(pList->depbin->db_count)
			delete (pList->depbin->db_count);
		if(pList->depbin->db_capacity)
			delete (pList->depbin->db_capacity);
		if(pList->depbin->db_debug_info)
			delete (pList->depbin->db_debug_info);
		delete (pList->depbin);

	}

	memset((char *)pList, 0, sizeof(FLAT_LIST));
}/*End DDl6Array :: clear_flat*/

/*********************************************************************************
	
								SOURCE	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Source ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{

#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pSource != NULL);
	assert(pbyObjExtn[1] == byItemType);
	/*ASSERT_DBG(pbyObjExtn != NULL && pSource != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif
	

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[SOURCE_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pSource->masks.bin_exists =  attrMask & SOURCE_ATTR_MASKS;
	pSource->id = id;

	iRetVal  = get_item_attr(objIndex, attrMask, pSource,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pSource->masks.bin_hooked,   attach_source_data);

	return iRetVal;
}/*End DDl6Array ::fetch_item*/


void DDl6Source :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & SOURCE_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceLabel",
										SOURCE_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceHelp",
										SOURCE_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceValidity",
										SOURCE_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_EMPHASIS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceEmphasis",
										SOURCE_EMPHASIS_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_LINETYPE)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceLineType",
										SOURCE_LINETYPE_ID,
										DDL_ATTR_DATA_TYPE_LINE_TYPE,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & SOURCE_LINECOLOR)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceLineColor",
										SOURCE_LINECOLOR_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & SOURCE_YAXIS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceYAxis",
										SOURCE_YAXIS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_INIT_ACTIONS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceInitActions",
										SOURCE_INIT_ACTIONS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_RFRSH_ACTIONS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceRfrshActions",
										SOURCE_RFRSH_ACTIONS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_EXIT_ACTIONS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceExitActions",
										SOURCE_EXIT_ACTIONS_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_MEMBERS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceMembers",
										SOURCE_MEMBERS_ID,
										DDL_ATTR_DATA_TYPE_MEMBER_LIST,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & SOURCE_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("SourceDebugData",
										SOURCE_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}



	return;

}/* End DDl6Array :: AllocAttributes */


void DDl6Source :: clear_flat( void)
{	
	if(pSource->depbin)
	{
		if(pSource->depbin->db_label)
			delete (pSource->depbin->db_label);
		if(pSource->depbin->db_help)
			delete (pSource->depbin->db_help);
		if(pSource->depbin->db_valid)
			delete (pSource->depbin->db_valid);
		if(pSource->depbin->db_emphasis)
			delete (pSource->depbin->db_emphasis);
		if(pSource->depbin->db_linetype)
			delete (pSource->depbin->db_linetype);
		if(pSource->depbin->db_linecolor)
			delete (pSource->depbin->db_linecolor);
		if(pSource->depbin->db_y_axis)
			delete (pSource->depbin->db_y_axis);
		if(pSource->depbin->db_init_acts)
			delete (pSource->depbin->db_init_acts);
		if(pSource->depbin->db_rfrsh_acts)
			delete (pSource->depbin->db_rfrsh_acts);
		if(pSource->depbin->db_exit_acts)
			delete (pSource->depbin->db_exit_acts);
		if(pSource->depbin->db_members)
			delete (pSource->depbin->db_members);
		if(pSource->depbin->db_debug_info)
			delete (pSource->depbin->db_debug_info);
		delete (pSource->depbin);

	}

	memset((char *)pSource, 0, sizeof(FLAT_SOURCE));
}/*End DDl6Array :: clear_flat*/



/*********************************************************************************
	
								IMAGE	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Image ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{

#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pImage != NULL);
	assert(pbyObjExtn[1] == byItemType);
	/*ASSERT_DBG(pbyObjExtn != NULL && pImage != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif

	

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[IMAGE_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pImage->masks.bin_exists =  attrMask & IMAGE_ATTR_MASKS;
	pImage->id = id;

	iRetVal  = get_item_attr(objIndex, attrMask, pImage,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pImage->masks.bin_hooked,   attach_image_data);

	return iRetVal;
}/*End DDl6Image ::fetch_item*/


void DDl6Image :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & IMAGE_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ImageLabel",
										IMAGE_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & IMAGE_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ImageHelp",
										IMAGE_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & IMAGE_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ImageValidity",
										IMAGE_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & IMAGE_LINK)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ImageLink",
										IMAGE_LINK_ID,
										DDL_ATTR_DATA_TYPE_REFERENCE,/****************/
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & IMAGE_PATH)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ImagePath",
										IMAGE_PATH_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,/*** verify ****/
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & IMAGE_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("ImageDebugData",
										IMAGE_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}

	return;

}/* End DDl6Image :: AllocAttributes */


void DDl6Image :: clear_flat( void)
{	
	if(pImage->depbin)
	{
		if(pImage->depbin->db_label)
			delete (pImage->depbin->db_label);
		if(pImage->depbin->db_help)
			delete (pImage->depbin->db_help);
		if(pImage->depbin->db_valid)
			delete (pImage->depbin->db_valid);
		if(pImage->depbin->db_link)
			delete (pImage->depbin->db_link);
		if(pImage->depbin->db_path)
			delete (pImage->depbin->db_path);
		if(pImage->depbin->db_debug_info)
			delete (pImage->depbin->db_debug_info);
		delete (pImage->depbin);

	}

	memset((char *)pImage, 0, sizeof(FLAT_IMAGE));
}/*End DDl6Image :: clear_flat*/


/*********************************************************************************
	
								GRID	ITEM	METHODS
		
/*********************************************************************************/

int DDl6Grid ::fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex)
{
#ifdef MODIFIED_BY_SS
	assert(pbyObjExtn != NULL && pGrid != NULL);
	assert(pbyObjExtn[1] == byItemType);
	/*ASSERT_DBG(pbyObjExtn != NULL && pGrid != NULL);
	ASSERT_DBG(pbyObjExtn[1] == byItemType);*/
#endif
	

	BYTE* pbyLocalAttrOffset;
	BYTE* pbyItemExtn = pbyObjExtn;// internal iterator
	INT   iAttrLength,  
		  iRetVal     = preFetchItem(this, maskSizes[GRID_ITYPE], &pbyItemExtn, iAttrLength);

	if (iRetVal)  return iRetVal;

	pbyLocalAttrOffset = pbyItemExtn;
	
	pGrid->masks.bin_exists =  attrMask & GRID_ATTR_MASKS;
	pGrid->id = id;

	iRetVal  = get_item_attr(objIndex, attrMask, pGrid,
					iAttrLength,       pbyLocalAttrOffset,
					byItemType,        pGrid->masks.bin_hooked,   attach_grid_data);

	return iRetVal;
}/*End DDl6Grid ::fetch_item*/


void DDl6Grid :: AllocAttributes(void)
{
	DDlAttribute *pDDlAttr=NULL;

	if(attrMask & GRID_LABEL)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridLabel",
										GRID_LABEL_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRID_HELP)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridHelp",
										GRID_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRID_VALID)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridValidity",
										GRID_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRID_HEIGHT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridHeight",
										GRID_HEIGHT_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRID_WIDTH)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridWidth",
										GRID_WIDTH_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRID_ORIENT)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridOrientation",
										GRID_ORIENT_ID,
										DDL_ATTR_DATA_TYPE_INT,
										false);
		attrList.push_back(pDDlAttr);	
	}

	if(attrMask & GRID_HANDLING)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridHandling",
										GRID_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & GRID_MEMBERS)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridPath",
										GRID_MEMBERS_ID,
										DDL_ATTR_DATA_TYPE_GRID_SET,
										false);
		attrList.push_back(pDDlAttr);	
	}


	if(attrMask & GRID_DEBUG)
	{
		pDDlAttr = (DDlAttribute*)new DDlAttribute("GridDebugData",
										GRID_DEBUG_ID,
										DDL_ATTR_DATA_TYPE_DEBUG_DATA,
										false);
		attrList.push_back(pDDlAttr);	
	}

	return;

}/* End DDl6Grid :: AllocAttributes */


void DDl6Grid :: clear_flat( void)
{	
	if(pGrid->depbin)
	{
		if(pGrid->depbin->db_label)
			delete (pGrid->depbin->db_label);
		if(pGrid->depbin->db_help)
			delete (pGrid->depbin->db_help);
		if(pGrid->depbin->db_valid)
			delete (pGrid->depbin->db_valid);
		if(pGrid->depbin->db_height)
			delete (pGrid->depbin->db_height);
		if(pGrid->depbin->db_width)
			delete (pGrid->depbin->db_width);
		if(pGrid->depbin->db_orient)
			delete (pGrid->depbin->db_orient);
		if(pGrid->depbin->db_handling)
			delete (pGrid->depbin->db_handling);
		if(pGrid->depbin->db_members)
			delete (pGrid->depbin->db_members);
		if(pGrid->depbin->db_debug_info)
			delete (pGrid->depbin->db_debug_info);
		delete (pGrid->depbin);

	}

	memset((char *)pGrid, 0, sizeof(FLAT_GRID));
}/*End DDl6Grid :: clear_flat*/
