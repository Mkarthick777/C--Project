#include "pch.h"
#pragma warning (disable : 4786)

//sjv06feb06 #include <windows.h>
#ifndef _WIN32_WCE
#include "std.h"	//removed to stop compiler error win ce PAW 09/04/09
#endif
#include "Eval_item.h"

#include "DDl_Attr_Parse.h"
#include "Retn_Code.h"
// moved to common as tags_sa.h    #include "DDLTags.h"
#include "Tags_sa.h"
#include "DDL6Items.h"

#include <vector>




/*F*/
/***************************************************************************
** FUNCTION NAME: eval_variable()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the AttributeList of the DDl6Variable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6Variable ::eval_attrs( void )
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	DDlAttribute *pAttribute = NULL;

	/*Vibhor 141103: Since HANDLING is not affected by TypeSize attribute & 
	 for some efficiency we need it at the beginning of the list , so parsing
	 it as the first attribute!!!*/

	if(attrMask & VAR_HANDLING)
	{

		pAttribute = (DDlAttribute*)new DDlAttribute("VarHandling",
										VAR_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		AttrChunkPtr = pVar->depbin->db_handling->bin_chunk;
		ulChunkSize = pVar->depbin->db_handling->bin_size;
		rc = parse_attr_bitstring(pAttribute,AttrChunkPtr,ulChunkSize);
		if(rc != SUCCESS)
				return rc;
		attrList.push_back(pAttribute);
	
	}
	else
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("VarHandling",
										VAR_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = READ_HANDLING | WRITE_HANDLING; /*Default Attribute*/

		attrList.push_back(pAttribute);
		
	}
	
	/*Since some attributes become invalid depending upon the "type" attribute
	 of the variable , we will evaluate it first and restructure the attribute
	 list , if required ( by deleting the invalid attributes*/
	
	/*The first attribute is always TYPE_SIZE*/
	//pAttribute = pVar->attrList.begin();

	pAttribute = (DDlAttribute*)new DDlAttribute("VarTypeSize",
										VAR_TYPE_SIZE_ID,
										DDL_ATTR_DATA_TYPE_TYPE_SIZE,
										false);
	
	AttrChunkPtr = pVar->depbin->db_type_size->bin_chunk;
	ulChunkSize = pVar->depbin->db_type_size->bin_size;

	rc = parse_attr_type_size(pAttribute,AttrChunkPtr,ulChunkSize);
	
	if(rc != SUCCESS)
		return rc;

	switch (pAttribute->pVals->typeSize.type){
			case INTEGER:
			case UNSIGNED:
			case FLOATG_PT:
			case DOUBLE_FLOAT:
				attrMask &= INVALID_ARITH_TYPE_SUBATTR_MASK;
				break;

			case ENUMERATED:
			case BIT_ENUMERATED:
				attrMask &= INVALID_ENUM_TYPE_SUBATTR_MASK;
				break;

			case INDEX:
				attrMask &= INVALID_INDEX_TYPE_SUBATTR_MASK;
				break;

//FF			case EUC:
			case ASCII:
			case PACKED_ASCII:
			case PASSWORD:
//FF			case BITSTRING:
				attrMask &= INVALID_STRING_TYPE_SUBATTR_MASK;
				break;

			case HART_DATE_FORMAT:
//FF			case TIME:
//FF			case DATE_AND_TIME:
//FF			case DURATION:
				attrMask &= INVALID_DATE_TIME_TYPE_SUBATTR_MASK;
				break;

			case TIME_VALUE:	// timj 26dec07
				attrMask &= INVALID_TIME_VALUE_TYPE_SUBATTR_MASK;
				break;

			default:	/* should never happen */
				attrMask &= INVALID_VAR_TYPE_SUBATTR_MASK;
				break;
	} /*End Switch pAttribute->pVals->typeSize.type */
	

	/*Now push the TYPE_SIZE attribute onto the Attribute List*/
	attrList.push_back(pAttribute);

	/*Now pass the updated attribute mask to allocate the attributes*/
	AllocAttributes();

	/*Now we have the Attribute list for this variable ready !!!
	 Just iterate through it and parse each individual attribute
	 Note : We have already parsed the first attribute ie. TYPE_SIZE
	 so we'll start from the second attribute*/

	ItemAttrList :: iterator p;

	for(p = (attrList.begin())+1;p != attrList.end();p++)
	{	

			switch((*p)->byAttrID)
			{
			case	VAR_CLASS_ID:
				{
					AttrChunkPtr = pVar->depbin->db_class->bin_chunk;
					ulChunkSize = pVar->depbin->db_class->bin_size;
					rc = parse_attr_bitstring((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
				}
				break;
/*			case	VAR_HANDLING_ID:
				{
					AttrChunkPtr = pVar->depbin->db_handling->bin_chunk;
					ulChunkSize = pVar->depbin->db_handling->bin_size;
					rc = parse_attr_bitstring((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
				}
				break;
*/
			case	VAR_UNIT_ID :
				{	
					AttrChunkPtr = pVar->misc->depbin->db_unit->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_unit->bin_size;
					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
					
				}
				break;

			case	VAR_LABEL_ID:
				{
					AttrChunkPtr = pVar->depbin->db_label->bin_chunk;
					ulChunkSize = pVar->depbin->db_label->bin_size;
					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
	
				}
				break;
			case  	VAR_HELP_ID:
				{
					AttrChunkPtr = pVar->depbin->db_help->bin_chunk;
					ulChunkSize = pVar->depbin->db_help->bin_size;
					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
	
				}
				break;
/* removed 15oct12
			case	VAR_READ_TIME_OUT_ID:
				{	
					AttrChunkPtr = pVar->misc->depbin->db_read_time_out->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_read_time_out->bin_size;
					//here we have to parse a constant integral value from an expression;
					//Need to define an API for this
					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;					
				}
				break;

			case	VAR_WRITE_TIME_OUT_ID:
				{	
					AttrChunkPtr = pVar->misc->depbin->db_write_time_out->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_write_time_out->bin_size;
					//here we have to parse a constant integral value from an expression;
					//Need to define an API for this
					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
***/
			case VAR_HEIGHTSIZE_ID:
				{				
					AttrChunkPtr = pVar->misc->depbin->db_height->bin_chunk;
					ulChunkSize  = pVar->misc->depbin->db_height->bin_size;

					rc = parse_attr_scope_size((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case VAR_WIDTHSIZE_ID:
				{				
					AttrChunkPtr = pVar->misc->depbin->db_width->bin_chunk;
					ulChunkSize  = pVar->misc->depbin->db_width->bin_size;

					rc = parse_attr_scope_size((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	VAR_VALID_ID:
				{	
					AttrChunkPtr = pVar->misc->depbin->db_valid->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_valid->bin_size;
					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
					
				}
				break;
			case	VAR_PRE_READ_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_pre_read_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_pre_read_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_POST_READ_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_post_read_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_post_read_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_PRE_WRITE_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_pre_write_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_pre_write_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_POST_WRITE_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_post_write_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_post_write_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_PRE_EDIT_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_pre_edit_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_pre_edit_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_POST_EDIT_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_post_edit_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_post_edit_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_REFRESH_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_refresh_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_refresh_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
//#ifdef XMTR
			case	VAR_POST_RQST_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_post_rqst_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_post_rqst_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_POST_USER_ACT_ID:
				{	
					AttrChunkPtr = pVar->actions->depbin->db_post_user_act->bin_chunk;
					ulChunkSize = pVar->actions->depbin->db_post_user_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
//#endif /*XMTR*/
/*			case	VAR_RESP_CODES_ID:
				{	
					AttrChunkPtr = pVar->depbin->db_resp_codes->bin_chunk;
					ulChunkSize = pVar->depbin->db_resp_codes->bin_size;
					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break; */
			case	VAR_DISPLAY_ID:
				{	
					AttrChunkPtr = pVar->depbin->db_display->bin_chunk;
					ulChunkSize = pVar->depbin->db_display->bin_size;
					rc = parse_attr_disp_edit_format((*p),AttrChunkPtr,ulChunkSize,DISPLAY_FORMAT_TAG);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_EDIT_ID:
				{	
					AttrChunkPtr = pVar->depbin->db_edit->bin_chunk;
					ulChunkSize = pVar->depbin->db_edit->bin_size;
					rc = parse_attr_disp_edit_format((*p),AttrChunkPtr,ulChunkSize,EDIT_FORMAT_TAG);
					if(rc != SUCCESS)
						return rc; 
				}
				break;
			case	VAR_MIN_VAL_ID:
				{	
					AttrChunkPtr = pVar->misc->depbin->db_min_val->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_min_val->bin_size;
					rc = parse_attr_min_max_list((*p),AttrChunkPtr,ulChunkSize,MIN_VALUE_TAG);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;				
			case	VAR_MAX_VAL_ID:
				{	
					AttrChunkPtr = pVar->misc->depbin->db_max_val->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_max_val->bin_size;
					rc = parse_attr_min_max_list((*p),AttrChunkPtr,ulChunkSize,MAX_VALUE_TAG);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;	
			case	VAR_SCALE_ID:
				{	
					AttrChunkPtr = pVar->misc->depbin->db_scale->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_scale->bin_size;
					rc = parse_attr_scaling_factor((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;  
				}
				break;	
			case	VAR_ENUMS_ID:
				{	
					AttrChunkPtr = pVar->depbin->db_enums->bin_chunk;
					ulChunkSize = pVar->depbin->db_enums->bin_size;
					rc = parse_attr_enum_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;  
					
				}
				break;				
			case	VAR_INDEX_ITEM_ARRAY_ID:
				{	
					AttrChunkPtr = pVar->depbin->db_index_item_array->bin_chunk;
					ulChunkSize = pVar->depbin->db_index_item_array->bin_size;
					rc = parse_attr_array_name((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;  
					
				}
				break;	
			case	VAR_DEFAULT_VALUE_ID:		//Vibhor 030904: Start of Code
				{	
					AttrChunkPtr = pVar->depbin->db_default_value->bin_chunk;
					ulChunkSize = pVar->depbin->db_default_value->bin_size;
					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;  					
				}
				break;	
			case	VAR_DEBUG_ID:		//stevev 11may05
				{	
					AttrChunkPtr = pVar->misc->depbin->db_debug_info->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_debug_info->bin_size;
					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)	// 4
						return rc;  
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;  // here memory leak PAW 09/04/09
				}
				break;	
			/* handled elsewhere - but recognized */
			case VAR_HANDLING_ID: 
			case VAR_TYPE_SIZE_ID: 
				break;

			case VAR_TIME_FORMAT_ID:
				{	
					AttrChunkPtr = pVar->misc->depbin->db_time_format->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_time_format->bin_size;
					rc = parse_attr_disp_edit_format((*p),AttrChunkPtr,ulChunkSize,TIME_FORMAT_TAG);
					if(rc != SUCCESS)
						return rc; 
				}
				break;

			case VAR_TIME_SCALE_ID:
								{	
					AttrChunkPtr = pVar->misc->depbin->db_time_scale->bin_chunk;
					ulChunkSize = pVar->misc->depbin->db_time_scale->bin_size;
					rc = parse_attr_time_scale((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;  
				}
				break;	

			default:
				/*Should Never Reach here!!!!*/
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unknown variable attribute tag:%d\n",((int)(*p)->byAttrID) );
#endif
				break;
			}/*End Switch*/
	}/*End for*/

	/*Vibhor 271003: Though Handling is an optional attribute,
	 but wee need it for display processing & by definition a 
	 variable without a Handling can be both read & written,
	 so we will default this value to READ & WRITE if not already there*/

/*	 if (!(attrMask & VAR_HANDLING))
	 {
		pAttribute = (DDlAttribute*)new DDlAttribute("VarHandling",
										VAR_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ulVal = READ_HANDLING | WRITE_HANDLING; 

		pVar->attrList.push_back(pAttribute);
	 }
*/
	/*Just check if we got the validity attribute from the binary ,
	 if not then allocate it and default it to true & push it on the 
	 attrList */

	 if (!(attrMask & VAR_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("VarValidity",
										VAR_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		attrList.push_back(pAttribute);
	 }

	 // be sure they are set
	 attrMask = attrMask | VAR_CLASS |VAR_TYPE_SIZE| VAR_HANDLING | VAR_VALID;
		
	 ulItemMasks = attrMask;

	return SUCCESS;	

}/*End eval_variable*/


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_command()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the AttributeList of the DDl6Command
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

//int eval_command(FLAT_COMMAND* fcmd, unsigned long ulCmdMask,DDlCommand *pCmd)
int DDl6Command :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();

	ItemAttrList :: iterator p;

	for(p = attrList.begin();p != attrList.end();p++)
	{// ptr2aPtr2a DDlAttribute 
		DDlAttribute *pAt = (*p);
		switch( pAt->byAttrID )
		{
		case	COMMAND_NUMBER_ID:
			{
	
				AttrChunkPtr = pCmd->depbin->db_number->bin_chunk;
				ulChunkSize = pCmd->depbin->db_number->bin_size;

				rc = parse_attr_ulong(pAt,AttrChunkPtr,ulChunkSize);

				if(rc != SUCCESS)
					return rc;
			}
			break;
		case	COMMAND_OPER_ID:
			{
				AttrChunkPtr = pCmd->depbin->db_oper->bin_chunk;
				ulChunkSize = pCmd->depbin->db_number->bin_size;

				rc = parse_attr_ulong(pAt,AttrChunkPtr,ulChunkSize);

				if(rc != SUCCESS)
					return rc;
			}
			break;
		case	COMMAND_TRANS_ID:
			{
				AttrChunkPtr = pCmd->depbin->db_trans->bin_chunk;
				ulChunkSize = pCmd->depbin->db_trans->bin_size;
				rc = parse_attr_transaction_list(pAt,AttrChunkPtr,ulChunkSize);
				if(rc != SUCCESS)
					return rc;
/** temp 
if((*p)->pVals->transList[0][0].post_rqst_rcv_act.size())
{
	TRANSACTION *t = &((*p)->pVals->transList[0][0]);
	LOGIT(CLOG_LOG,"Finished transactions: "<<(*p)->pVals->transList[0][0].post_rqst_rcv_act.size() << " actions."<<endl;
}**/
			}

			break;
		case	COMMAND_RESP_CODES_ID:
			{
				AttrChunkPtr = pCmd->depbin->db_resp_codes->bin_chunk;
				ulChunkSize = pCmd->depbin->db_resp_codes->bin_size;
				rc = parse_attr_resp_code_list(pAt,AttrChunkPtr,ulChunkSize);
				if(rc != SUCCESS)
					return rc;
				
			}
			break;
		case	COMMAND_DEBUG_ID:
			{
				AttrChunkPtr = pCmd->depbin->db_debug_info->bin_chunk;
				ulChunkSize = pCmd->depbin->db_debug_info->bin_size;
				rc = parse_debug_info(pAt,AttrChunkPtr,ulChunkSize);
				if(rc != SUCCESS)
					return rc;
				else					
					strItemName = pAt->pVals->debugInfo->symbol_name;
				
			}
			break;
		default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized command attribute tag:%d\n",(*p)->byAttrID );
#endif
			/*should never reach here*/
			break;
		}
	}/*End for */
		
	 ulItemMasks = attrMask;

	return  SUCCESS;

}/*End eval_command*/

/*F*/
/***************************************************************************
** FUNCTION NAME: eval_menu()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/
#ifdef _DBGMIL
bool flagthisone = false;
MENU_ITEM_LIST* pGlblMIL = NULL;
#endif
//int eval_menu(FLAT_MENU* fmenu, unsigned long ulMenuMask,DDlMenu *pMenu)
int DDl6Menu :: eval_attrs(void)
{
	
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	DDlAttribute *pAttribute = NULL;

	AllocAttributes();

	ItemAttrList :: iterator p;

	for(p = attrList.begin();p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case MENU_LABEL_ID:
				{
					AttrChunkPtr = pMenu->depbin->db_label->bin_chunk;
					ulChunkSize = pMenu->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
#ifdef XX_PARSER_DEBUG
					//p->pVals->strVal
LOGIT(CERR_LOG,"------------ menu label--------------%ls---\n", (*p)->pVals->strVal->str );
#endif
				}
				break;

			case MENU_ITEMS_ID:
				{
					AttrChunkPtr = pMenu->depbin->db_items->bin_chunk;
					ulChunkSize = pMenu->depbin->db_items->bin_size;

					rc = parse_attr_menu_item_list((*p),AttrChunkPtr,ulChunkSize);

					if(rc != SUCCESS)
						return rc;
				}
				break;

			case MENU_HELP_ID:
				{
					AttrChunkPtr = pMenu->depbin->db_help->bin_chunk;
					ulChunkSize = pMenu->depbin->db_help->bin_size;
					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
				}
				break;

			case MENU_VALID_ID:
				{
					AttrChunkPtr = pMenu->depbin->db_valid->bin_chunk;
					ulChunkSize = pMenu->depbin->db_valid->bin_size;
					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);

					if(rc != SUCCESS)
						return rc;
				}
				break;

			case MENU_STYLE_ID:
				{
					AttrChunkPtr = pMenu->depbin->db_style->bin_chunk;
					ulChunkSize = pMenu->depbin->db_style->bin_size;
					rc = parse_attr_menu_style((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
				}
				break;

			case MENU_DEBUG_ID:
				{
					AttrChunkPtr = pMenu->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pMenu->depbin->db_debug_info->bin_size;
					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;

			default:
				/*should never reach here*/
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized menu attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/

	/*See if we didn't get validity, default it to true and push it onto the attribute List*/
	if (!(attrMask & MENU_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("MenuValidity",
										MENU_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		attrList.push_back(pAttribute);
	 }

/*	if (!(attrMask & MENU_STYLE))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("MenuStyle",
										MENU_STYLE_ID,
										DDL_ATTR_DATA_TYPE_INTEGER,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->iVal = 0; //Default Attribute

		attrList.push_back(pAttribute);
	 }
*/
	
	attrMask= attrMask | MENU_VALID ;// |MENU_STYLE;
		
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_menu */

/*F*/
/***************************************************************************
** FUNCTION NAME: eval_edit_display()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/


//int eval_edit_display(FLAT_EDIT_DISPLAY* fedit, unsigned long ulEditDispMask,DDlEditDisplay *pEditDisp)
int DDl6EditDisplay :: eval_attrs(void)
{

	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	DDlAttribute *pAttribute = NULL;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin();p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	EDIT_DISPLAY_LABEL_ID:
				{

					AttrChunkPtr = pEdDisp->depbin->db_label->bin_chunk;
					ulChunkSize  = pEdDisp->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_EDIT_ITEMS_ID:
				{
					AttrChunkPtr = pEdDisp->depbin->db_edit_items->bin_chunk;
					ulChunkSize  = pEdDisp->depbin->db_edit_items->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	EDIT_DISPLAY_DISP_ITEMS_ID:
				{

					AttrChunkPtr = pEdDisp->depbin->db_disp_items->bin_chunk;
					ulChunkSize  = pEdDisp->depbin->db_disp_items->bin_size;
				
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_PRE_EDIT_ACT_ID:
				{
					AttrChunkPtr = pEdDisp->depbin->db_pre_edit_act->bin_chunk;
					ulChunkSize  = pEdDisp->depbin->db_pre_edit_act->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_POST_EDIT_ACT_ID:
				{
					AttrChunkPtr = pEdDisp->depbin->db_post_edit_act->bin_chunk;
					ulChunkSize  = pEdDisp->depbin->db_post_edit_act->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_HELP_ID:
				{

					AttrChunkPtr = pEdDisp->depbin->db_help->bin_chunk;
					ulChunkSize  = pEdDisp->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_VALID_ID:
				{

					AttrChunkPtr = pEdDisp->depbin->db_valid->bin_chunk;
					ulChunkSize  = pEdDisp->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_DEBUG_ID:
				{
					AttrChunkPtr = pEdDisp->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pEdDisp->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized edit_display attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;
		
		}/*End switch*/

	}/*End for*/

	/*See if we didn't get validity, default it to true and push it onto the attribute List*/
	if (!(attrMask & EDIT_DISPLAY_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("EditDisplayValidity",
										EDIT_DISPLAY_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		attrList.push_back(pAttribute);
	 }

	
	 attrMask= attrMask | EDIT_DISPLAY_VALID ;
		
	 ulItemMasks = attrMask;

	return SUCCESS;


}/*End eval_edit_display */


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_method()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

//int eval_method(FLAT_METHOD* fmeth, unsigned long ulMethodMask,DDlMethod *pMeth)
int DDl6Method :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	DDlAttribute *pAttribute = NULL;

	AllocAttributes();

	ItemAttrList :: iterator p;

	int i = 0;
	int sizeoflist = attrList.size ();

	//for(p = pMeth->attrList.begin();p != pMeth->attrList.end();p++)
	p = attrList.begin();
	while (p != attrList.end())
	{
		i++;

		switch((*p)->byAttrID)
		{
			case	METHOD_CLASS_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_class->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_class->bin_size;

					rc = parse_attr_bitstring((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	METHOD_LABEL_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_label->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				 
				}
				break;
			case	METHOD_HELP_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_help->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	METHOD_DEF_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_def->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_def->bin_size;

					rc = parse_attr_definition((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	METHOD_VALID_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_valid->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	METHOD_SCOPE_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_scope->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_scope->bin_size;

					rc = parse_attr_meth_scope((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	METHOD_TYPE_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_type->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_type->bin_size;

					rc = parse_attr_method_type((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	METHOD_PARAMS_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_params->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_params->bin_size;

					rc = parse_attr_param_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	METHOD_DEBUG_ID:
				{
					AttrChunkPtr = pMthd->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pMthd->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized method attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;
		}/*End switch*/

		p++;
	}/*End while */

/*Vibhor 240204: Start of Code*/
/*If a methode has no Help defined for it we'll have default string*/
	
	if (!(attrMask & METHOD_HELP))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("MethodHelp",
										METHOD_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
			
		pAttribute->pVals = new VALUES;

		pAttribute->pVals->strVal = new ddpSTRING;

		pAttribute->pVals->strVal->str = new char[18];
#ifdef MODIFIED_BY_SS
		//strcpy(pAttribute->pVals->strVal->str, ""/*"No Help Available"*/);
		std::copy_n("", 1, pAttribute->pVals->strVal->str);
#else
		strcpy(pAttribute->pVals->strVal->str,""/*"No Help Available"*/);//Removed No Help Available, DDHost Test requires a blank label, POB - 12/2/2013
#endif		
		pAttribute->pVals->strVal->flags = FREE_STRING;

		pAttribute->pVals->strVal->strType = DEV_SPEC_STRING_TAG; //This will ensure cleanup.
		
		attrList.push_back(pAttribute);

		attrMask |= METHOD_HELP ;

	 }

/*Vibhor 240204: End of Code*/

	if (!(attrMask & METHOD_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("MethodValidity",
										METHOD_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		attrList.push_back(pAttribute);

		attrMask |= METHOD_VALID;
	 };
			
	 ulItemMasks = attrMask;

	return SUCCESS;
}/*End eval_method*/


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_refresh()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

//int eval_refresh(FLAT_REFRESH* frfrsh, unsigned long ulRefrshMask,DDlRefresh *pRefrsh)
int DDl6Refresh :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();

	ItemAttrList :: iterator p;

	for(p = attrList.begin();p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	REFRESH_ITEMS_ID:
				{
					AttrChunkPtr = pRfsh->depbin->db_items->bin_chunk;
					ulChunkSize  = pRfsh->depbin->db_items->bin_size;

					rc = parse_attr_refresh_relation((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	REFRESH_DEBUG_ID:
				{
					AttrChunkPtr = pRfsh->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pRfsh->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized refresh attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;
		}/*End switch*/

	}/*End for*/
				
	 ulItemMasks = attrMask;

	return SUCCESS;
}/*End eval_refresh */


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_unit()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

//int eval_unit(FLAT_UNIT* funit, unsigned long ulUnitMask,DDlUnit *pUnit)
int DDl6Unit :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();

	ItemAttrList :: iterator p;

	for(p = attrList.begin();p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	UNIT_ITEMS_ID:
				{
					AttrChunkPtr = pUnit->depbin->db_items->bin_chunk;
					ulChunkSize  = pUnit->depbin->db_items->bin_size;

					rc = parse_attr_unit_relation((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	UNIT_DEBUG_ID:
				{
					AttrChunkPtr = pUnit->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pUnit->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized unit_relation attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;
				
		}/*End switch */

	}/*End for*/
		
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_unit */

/*F*/
/***************************************************************************
** FUNCTION NAME: eval_wao()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

//int eval_wao(FLAT_WAO* fwao, unsigned long ulWaoMask,DDlWao *pWao)
int DDl6Wao :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();

	ItemAttrList :: iterator p;

	for(p = attrList.begin();p != attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	WAO_ITEMS_ID:
				{
					AttrChunkPtr = pWao->depbin->db_items->bin_chunk;
					ulChunkSize  = pWao->depbin->db_items->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}

				break;
			case	WAO_DEBUG_ID:
				{
					AttrChunkPtr = pWao->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pWao->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}

				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized wao attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/

		
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_wao */


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_item_array()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/



//int eval_item_array(FLAT_ITEM_ARRAY* fiarr, unsigned long ulItemArrayMask,DDlItemArray *pIArray)
int DDl6ItemArray :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	DDlAttribute *pAttribute = NULL;
	
	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	ITEM_ARRAY_ELEMENTS_ID:
				{
					AttrChunkPtr = pItmArr->depbin->db_elements->bin_chunk;
					ulChunkSize  = pItmArr->depbin->db_elements->bin_size;

					rc = parse_attr_item_array_element_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			case	ITEM_ARRAY_LABEL_ID:
				{
					AttrChunkPtr = pItmArr->depbin->db_label->bin_chunk;
					ulChunkSize  = pItmArr->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	ITEM_ARRAY_HELP_ID:
				{
					AttrChunkPtr = pItmArr->depbin->db_help->bin_chunk;
					ulChunkSize  = pItmArr->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	ITEM_ARRAY_VALIDITY_ID:
				{
					AttrChunkPtr = pItmArr->depbin->db_valid->bin_chunk;
					ulChunkSize  = pItmArr->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	ITEM_ARRAY_DEBUG_ID:
				{
					AttrChunkPtr = pItmArr->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pItmArr->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized item_array attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;
		}/*End switch*/

	}/*End for */

	/*See if we didn't get validity, default it to true and push it onto the attribute List*/
	if (!(attrMask & ITEM_ARRAY_VALIDITY))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("ItemArrayValidity",
										ITEM_ARRAY_VALIDITY_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		attrList.push_back(pAttribute);
	 }

	
	 attrMask= attrMask | ITEM_ARRAY_VALIDITY ;

		
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_item_array */


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_collection()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/


//int eval_collection(FLAT_COLLECTION* fcoll, unsigned long ulCollectionMask,DDlCollection *pColl)
int DDl6Collection :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	COLLECTION_MEMBERS_ID:	
				{
					AttrChunkPtr = pColl->depbin->db_members->bin_chunk;
					ulChunkSize  = pColl->depbin->db_members->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					

				}
				break;
			case	COLLECTION_LABEL_ID:
				{
					AttrChunkPtr = pColl->depbin->db_label->bin_chunk;
					ulChunkSize  = pColl->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	COLLECTION_HELP_ID:
				{
					AttrChunkPtr = pColl->depbin->db_help->bin_chunk;
					ulChunkSize  = pColl->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	COLLECTION_VALID_ID:/* added 23jan06 stevev - spec change */
				{
					AttrChunkPtr = pColl->depbin->db_valid->bin_chunk;
					ulChunkSize  = pColl->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;				
				}
				break;
			case	COLLECTION_DEBUG_ID:
				{
					AttrChunkPtr = pColl->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pColl->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized collection attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/

	/* default attributes  - stevev 23jan07 */
	DDlAttribute *pAttribute = NULL;
	if(!(attrMask & COLLECTION_VALIDITY))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("CollectionValidity",
										COLLECTION_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = 1;
		attrList.push_back(pAttribute);
	}
			
	 ulItemMasks = attrMask | COLLECTION_VALIDITY;

	return SUCCESS;

}/*End eval_collection */

/*F*/
/***************************************************************************
** FUNCTION NAME: eval_record()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   attrMask Mask of attributes available for this variable
**
**   OUTPUT ...
**     pVar		Pointer to a DDlVariable object which was populated with parsed 
**              attributes
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

//int eval_record(FLAT_RECORD* frec, unsigned long ulRecordMask,DDlRecord *pRec)
int DDl6Record :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;
	
	AllocAttributes();

	ItemAttrList :: iterator p;

	for(p = attrList.begin();p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	RECORD_MEMBERS_ID:
				{
					AttrChunkPtr = pRec->depbin->db_members->bin_chunk;
					ulChunkSize  = pRec->depbin->db_members->bin_size;
					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	RECORD_LABEL_ID:
				{
					AttrChunkPtr = pRec->depbin->db_label->bin_chunk;
					ulChunkSize  = pRec->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	RECORD_HELP_ID:
				{
					AttrChunkPtr = pRec->depbin->db_help->bin_chunk;
					ulChunkSize  = pRec->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
/*Vibhor 311003: Commenting this one*/
/*			case	RECORD_RESP_CODES_ID:
				{
					AttrChunkPtr = pRec->depbin->db_resp_codes->bin_chunk;
					ulChunkSize  = pRec->depbin->db_resp_codes->bin_size;

					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
*/ 
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized record attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for */

			
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_record */



//int eval_block(FLAT_BLOCK* fblock, unsigned long ulBlockMask,DDlBlock *pBlock)
int DDl6Block :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();

	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	BLOCK_CHARACTERISTIC_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_characteristic->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_characteristic->bin_size;
					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_LABEL_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_label->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_HELP_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_help->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_PARAM_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_param->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_param->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_MENU_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_menu->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_menu->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_EDIT_DISP_ID: 
				{
					AttrChunkPtr = pBlk->depbin->db_edit_disp->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_edit_disp->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_METHOD_ID: 
				{
					AttrChunkPtr = pBlk->depbin->db_method->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_method->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_REFRESH_ID: 
				{
					AttrChunkPtr = pBlk->depbin->db_refresh->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_refresh->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_UNIT_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_unit->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_unit->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_WAO_ID: 
				{
					AttrChunkPtr = pBlk->depbin->db_wao->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_wao->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_COLLECT_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_collect->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_collect->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_ITEM_ARRAY_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_item_array->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_item_array->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_PARAM_LIST_ID:
				{
					AttrChunkPtr = pBlk->depbin->db_param_list->bin_chunk;
					ulChunkSize  = pBlk->depbin->db_param_list->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized block attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;


		}/*End switch*/

	}/*End for*/

		
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_block*/

/***************************************************************************
** FUNCTION NAME: eval_array()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/
extern int	ddl_parse_integer_func(unsigned char **chunkp, unsigned long *size, UINT64 *value);
int DDl6Array :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;
	UINT64        tempLong;

	DDlAttribute *pAttribute = NULL;
	
	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	ARRAY_NUM_OF_ELEMENTS_ID:
				{
					AttrChunkPtr = pArr->depbin->db_num_of_elements->bin_chunk;
					ulChunkSize  = pArr->depbin->db_num_of_elements->bin_size;
					
					(*p)->pVals = new VALUES;
					
					DDL_PARSE_INTEGER(&AttrChunkPtr,&ulChunkSize,&tempLong);

					if(DDL_ATTR_DATA_TYPE_INT == (*p)->attrDataType)
					{
						(*p)->pVals->llVal = (INT64)tempLong;
					}
					else /*DDL_ATTR_DATA_TYPE_UNSIGNED_LONG == pAttr->attrDataType*/
					{
						(*p)->pVals->ullVal = tempLong;
					}

	//				rc = parse_attr_int((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			case	ARRAY_LABEL_ID:
				{
					AttrChunkPtr = pArr->depbin->db_label->bin_chunk;
					ulChunkSize  = pArr->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	ARRAY_HELP_ID:
				{
					AttrChunkPtr = pArr->depbin->db_help->bin_chunk;
					ulChunkSize  = pArr->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;

			case	ARRAY_VALID_ID:
				{
					AttrChunkPtr = pArr->depbin->db_valid->bin_chunk;
					ulChunkSize  = pArr->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	ARRAY_TYPE_ID:
				{
					AttrChunkPtr = pArr->depbin->db_type->bin_chunk;
					ulChunkSize  = pArr->depbin->db_type->bin_size;

					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	ARRAY_DEBUG_ID:
				{
					AttrChunkPtr = pArr->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pArr->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;

			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized array attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;
		}/*End switch*/

	}/*End for */

	/*See if we didn't get validity, default it to true and push it onto the attribute List*/
	if (!(attrMask & ARRAY_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("ArrayValidity",
										ARRAY_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		attrList.push_back(pAttribute);
	 }

	
	 attrMask= attrMask | ARRAY_VALID ;


		
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_item_array */


/***************************************************************************
** FUNCTION NAME: eval_chart()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6Chart :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;
	
	DDlAttribute *pAttribute = NULL;
	
	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	CHART_MEMBERS_ID:
				{
					AttrChunkPtr = pChart->depbin->db_members->bin_chunk;
					ulChunkSize  = pChart->depbin->db_members->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			case	CHART_LABEL_ID:
				{
					AttrChunkPtr = pChart->depbin->db_label->bin_chunk;
					ulChunkSize  = pChart->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	CHART_HELP_ID:
				{
					AttrChunkPtr = pChart->depbin->db_help->bin_chunk;
					ulChunkSize  = pChart->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;

			case	CHART_VALID_ID:
				{
					AttrChunkPtr = pChart->depbin->db_valid->bin_chunk;
					ulChunkSize  = pChart->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;

			case	CHART_HEIGHT_ID:
				{
					AttrChunkPtr = pChart->depbin->db_height->bin_chunk;
					ulChunkSize  = pChart->depbin->db_height->bin_size;

					rc = parse_attr_scope_size((*p),AttrChunkPtr,ulChunkSize); //Vibhor 260804: Changed to int
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;

			case	CHART_WIDTH_ID:
				{
					AttrChunkPtr = pChart->depbin->db_width->bin_chunk;
					ulChunkSize  = pChart->depbin->db_width->bin_size;

					rc = parse_attr_scope_size((*p),AttrChunkPtr,ulChunkSize); //Vibhor 260804: Changed to int
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;

			case	CHART_TYPE_ID:
				{
					AttrChunkPtr = pChart->depbin->db_type->bin_chunk;
					ulChunkSize  = pChart->depbin->db_type->bin_size;

					rc = parse_attr_chart_type((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;

			case	CHART_LENGTH_ID:
				{
					AttrChunkPtr = pChart->depbin->db_length->bin_chunk;
					ulChunkSize  = pChart->depbin->db_length->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize); //was parse_attr_int
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;

			case	CHART_CYCLETIME_ID:
				{
					AttrChunkPtr = pChart->depbin->db_cytime->bin_chunk;
					ulChunkSize  = pChart->depbin->db_cytime->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;

			case	CHART_DEBUG_ID:
				{
					AttrChunkPtr = pChart->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pChart->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;

			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized chart attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;
		}/*End switch*/

	}/*End for */

	/*See if we didn't get validity, default it to true and push it onto the attribute List*/
	if (!(attrMask & CHART_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("ChartValidity",
										CHART_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		attrList.push_back(pAttribute);
	 }

	//Added By Anil October 25 2005--starts here
	//To make the Chart type as Strip chart if it is not defined
	if (!(attrMask & CHART_TYPE))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("ChartType",
										CHART_TYPE_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 4; /*Default Attribute Strip chart*/

		attrList.push_back(pAttribute);
	 }
	//Added By Anil October 25 2005 --Ends here

	//Added By Anil January 03 2006 on Chart Lenght

	if (!(attrMask & CHART_LENGTH))
	 {
/* was ( pre 31jan06 ) ::		
		 pAttribute = (DDlAttribute*)new DDlAttribute("ChartLength",
										CHART_LENGTH_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ulVal = 600000; /x*Default Attribute Length of chart*x/
*** now::  */
		pAttribute = (DDlAttribute*)new DDlAttribute("ChartLength",
										CHART_LENGTH_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->pExpr = new ddpExpression;

		Element		exprElem;
		
		exprElem.byElemType   = INTCST_OPCODE;
		exprElem.elem.ulConst = 600000;
		pAttribute->pVals->pExpr->push_back(exprElem);
		//exprElem.clean();
		exprElem.Cleanup();
/* end new 31jan06 */

		attrList.push_back(pAttribute);
	 }

	//Anil 230506: Start of Code for Default value handling
	if(!(attrMask & CHART_HEIGHT))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("ChartHeight",
										CHART_HEIGHT_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = MEDIUM_DISPSIZE; /*Default Attribute for height is MEDIUM*/
		attrList.push_back(pAttribute);
	}


	if(!(attrMask & CHART_WIDTH))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("ChartWidth",
										CHART_WIDTH_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = MEDIUM_DISPSIZE; /*Default Attribute for Width is MEDIUM*/
		attrList.push_back(pAttribute);
	}
	
	if(!(attrMask & CHART_CYCLETIME))//expr
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("ChartCycleTime",
										CHART_CYCLETIME_ID,
										DDL_ATTR_DATA_TYPE_EXPRESSION,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->pExpr = new ddpExpression;

		Element		exprElem;
		
		exprElem.byElemType   = INTCST_OPCODE;
		exprElem.elem.ulConst = 1000;/*Default Attribute for Cycle time is 1 sec*/
		pAttribute->pVals->pExpr->push_back(exprElem);
		//exprElem.clean();
		exprElem.Cleanup();
		attrList.push_back(pAttribute);	
	}

	
	attrMask= attrMask | CHART_VALID | CHART_TYPE | CHART_LENGTH |CHART_HEIGHT |CHART_WIDTH |CHART_CYCLETIME ; //Added by Anil January 03 2006
//Anil 230506: End of Code for Default value handling


		
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_chart */



/***************************************************************************
** FUNCTION NAME: eval_graph()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6Graph :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	DDlAttribute *pAttribute = NULL;
	
	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	GRAPH_MEMBERS_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_members->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_members->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			case	GRAPH_LABEL_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_label->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	GRAPH_HELP_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_help->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;

			case	GRAPH_VALID_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_valid->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;

			case	GRAPH_HEIGHT_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_height->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_height->bin_size;

					rc = parse_attr_scope_size((*p),AttrChunkPtr,ulChunkSize); //Vibhor 260804: Changed to int
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;

			case	GRAPH_WIDTH_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_width->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_width->bin_size;

					rc = parse_attr_scope_size((*p),AttrChunkPtr,ulChunkSize);	//Vibhor 260804: Changed to int
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;

			case	GRAPH_XAXIS_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_x_axis->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_x_axis->bin_size;

					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;

			case	GRAPH_CYCLETIME_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_cytime->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_cytime->bin_size;


					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;

			case	GRAPH_DEBUG_ID:
				{
					AttrChunkPtr = pGraph->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pGraph->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;

			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized graph attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;
		}/*End switch*/

	}/*End for */

	/*See if we didn't get validity, default it to true and push it onto the attribute List*/
	if (!(attrMask & GRAPH_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("GraphValidity",
										GRAPH_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		attrList.push_back(pAttribute);
	 }
//Anil 230506: Start of Code for Default value handling
	if(!(attrMask & GRAPH_HEIGHT))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("GraphHeight",
										GRAPH_HEIGHT_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		pAttribute->pVals = new VALUES;		
		pAttribute->pVals->ullVal = MEDIUM_DISPSIZE; /*Default Attribute for Graph height is MEDIUM*/
		attrList.push_back(pAttribute);
	}


	if(!(attrMask & GRAPH_WIDTH))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("GraphWidth",
										GRAPH_WIDTH_ID,
										DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
										false);
		pAttribute->pVals = new VALUES;		
		pAttribute->pVals->ullVal = MEDIUM_DISPSIZE; /*Default Attribute for Graph is MEDIUM*/
		attrList.push_back(pAttribute);
	}
	
	 attrMask= attrMask | GRAPH_VALID |GRAPH_WIDTH |GRAPH_HEIGHT  ;
//Anil 230506: End of Code for Default value handling
		
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_graph */


/*Vibhor 260804: Start of Code*/

/***************************************************************************
** FUNCTION NAME: eval_axis()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6Axis ::eval_attrs()
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;
	
	DDlAttribute *pAttribute = NULL;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	AXIS_LABEL_ID:
				{
					AttrChunkPtr = pAxis->depbin->db_label->bin_chunk;
					ulChunkSize  = pAxis->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;

			case	AXIS_HELP_ID:
				{
					AttrChunkPtr = pAxis->depbin->db_help->bin_chunk;
					ulChunkSize  = pAxis->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;		
/***** axis no longer has validity - 23jan07 sjv - spec change ***
*			case	AXIS_VALID_ID:
*				{
*					AttrChunkPtr = pAxis->depbin->db_valid->bin_chunk;
*					ulChunkSize  = pAxis->depbin->db_valid->bin_size;
*
*					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
*					if(rc != DDL_SUCCESS) 
*						return rc; 
*				}
*
*				break;
*******************/
			case	AXIS_MINVAL_ID:
				{
					AttrChunkPtr = pAxis->depbin->db_minval->bin_chunk;
					ulChunkSize  = pAxis->depbin->db_minval->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			
			case	AXIS_MAXVAL_ID:
				{
					AttrChunkPtr = pAxis->depbin->db_maxval->bin_chunk;
					ulChunkSize  = pAxis->depbin->db_maxval->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			
			case	AXIS_SCALING_ID:
				{
					AttrChunkPtr = pAxis->depbin->db_scaling->bin_chunk;
					ulChunkSize  = pAxis->depbin->db_scaling->bin_size;

					rc = parse_attr_int((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			
			case	AXIS_CONSTUNIT_ID:
				{
					AttrChunkPtr = pAxis->depbin->db_unit->bin_chunk;
					ulChunkSize  = pAxis->depbin->db_unit->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			
			case	AXIS_DEBUG_ID:
				{
					AttrChunkPtr = pAxis->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pAxis->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}

				break;

			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized axis attribute tag:%d\n", (*p)->byAttrID );
#endif
				break;
		}/*End switch*/

	}/*End for*/


	/*See if we didn't get validity, default it to true and push it onto the attribute List*/
/** spec change: removed
* 	if (!(attrMask & AXIS_VALID))
*	 {
*		 pAttribute = (DDlAttribute*)new DDlAttribute("AxisValidity",
*										AXIS_VALID_ID,
*										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
*										false);			
*		pAttribute->pVals = new VALUES;		
*		pAttribute->pVals->ulVal = 1; //Default Attribute
*		attrList.push_back(pAttribute);
*	 }
**** end spec change ***/
	//Anil 230506: Start of Code for Default value handling
	if(!(attrMask & AXIS_SCALING))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("AxisScaling",
										AXIS_SCALING_ID,
										DDL_ATTR_DATA_TYPE_INT,
										false);
		pAttribute->pVals = new VALUES;		
		pAttribute->pVals->ullVal = LINEAR_SCALE; /*Default Attribute*/
		attrList.push_back(pAttribute);	
	}

	
	 attrMask= attrMask |AXIS_SCALING ;// spec change 23jan07, sjv  | AXIS_VALID 
	 //Anil 230506: End of Code for Default value handling

	ulItemMasks = attrMask;

	return SUCCESS;
}/*End DDl6Axis ::eval_attrs()*/


/***************************************************************************
** FUNCTION NAME: eval_file()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6File :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	FILE_MEMBERS_ID:	
				{
					AttrChunkPtr = pFile->depbin->db_members->bin_chunk;
					ulChunkSize  = pFile->depbin->db_members->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					

				}
				break;
			case	FILE_LABEL_ID:
				{
					AttrChunkPtr = pFile->depbin->db_label->bin_chunk;
					ulChunkSize  = pFile->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	FILE_HELP_ID:
				{
					AttrChunkPtr = pFile->depbin->db_help->bin_chunk;
					ulChunkSize  = pFile->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	FILE_DEBUG_ID:
				{
					AttrChunkPtr = pFile->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pFile->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized file attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/

			
	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_file */


/***************************************************************************
** FUNCTION NAME: eval_waveform()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6Waveform :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	WAVEFORM_LABEL_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_label->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	WAVEFORM_HELP_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_help->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	WAVEFORM_HANDLING_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_handling->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_handling->bin_size;

					rc = parse_attr_bitstring((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;

			case	WAVEFORM_EMPHASIS_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_emphasis->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_emphasis->bin_size;

					rc = parse_attr_int((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	WAVEFORM_LINETYPE_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_linetype->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_linetype->bin_size;

					rc = parse_attr_line_type((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;
			
			case	WAVEFORM_LINECOLOR_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_linecolor->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_linecolor->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;

			case	WAVEFORM_YAXIS_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_y_axis->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_y_axis->bin_size;

					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	WAVEFORM_KEYPTS_X_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_x_keypts->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_x_keypts->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;

			case	WAVEFORM_KEYPTS_Y_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_y_keypts->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_y_keypts->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;

			case	WAVEFORM_TYPE_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_type->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_type->bin_size;

					rc = parse_attr_wavefrm_type((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	WAVEFORM_X_VALUES_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_x_values->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_x_values->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;
			
			case	WAVEFORM_Y_VALUES_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_y_values->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_y_values->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;

			case	WAVEFORM_X_INITIAL_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_x_initial->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_x_initial->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	WAVEFORM_X_INCREMENT_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_x_incr->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_x_incr->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;
			case	WAVEFORM_POINT_COUNT_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_pt_count->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_pt_count->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;
			case	WAVEFORM_INIT_ACTIONS_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_init_acts->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_init_acts->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;
			case	WAVEFORM_RFRSH_ACTIONS_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_rfrsh_acts->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_rfrsh_acts->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;
			case	WAVEFORM_EXIT_ACTIONS_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_exit_acts->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_exit_acts->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;	
			case	WAVEFORM_DEBUG_ID:
				{
					AttrChunkPtr = pWaveFrm->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
			break;
			case	WAVEFORM_VALID_ID:	
				{
					AttrChunkPtr = pWaveFrm->depbin->db_valid->bin_chunk;
					ulChunkSize  = pWaveFrm->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;

			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized waveform attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/
//Anil 230506: Start of Code for Default value handling
	DDlAttribute *pAttribute = NULL;

	if (!(attrMask & WAVEFORM_VALID))	// added 23jan07 - sjv - spec change
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("WaveformValidity",
										WAVEFORM_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
			
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = 1; //Default Attribute
		attrList.push_back(pAttribute);
	}

	if(!(attrMask & WAVEFORM_HANDLING))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("WaveformHandling",
										WAVEFORM_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = READ_HANDLING | WRITE_HANDLING;
		attrList.push_back(pAttribute);
	
	}

	if(!(attrMask & WAVEFORM_EMPHASIS))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("WaveformEmphasis",
										WAVEFORM_EMPHASIS_ID,
										DDL_ATTR_DATA_TYPE_INT,	//Assuming bool will be encoded as int only
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = 1;
		attrList.push_back(pAttribute);
	}
attrMask= attrMask | WAVEFORM_HANDLING |WAVEFORM_EMPHASIS;
//Anil 230506: End of Code for Default value handling

	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_collection */

/*Vibhor 260804: End of Code*/

/*Vibhor 290904: Start of Code*/

/***************************************************************************
** FUNCTION NAME: eval_list()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6List :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	LIST_LABEL_ID:
				{
					AttrChunkPtr = pList->depbin->db_label->bin_chunk;
					ulChunkSize  = pList->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	LIST_HELP_ID:
				{
					AttrChunkPtr = pList->depbin->db_help->bin_chunk;
					ulChunkSize  = pList->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	LIST_VALID_ID:	
				{
					AttrChunkPtr = pList->depbin->db_valid->bin_chunk;
					ulChunkSize  = pList->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	LIST_TYPE_ID:	
				{
					AttrChunkPtr = pList->depbin->db_type->bin_chunk;
					ulChunkSize  = pList->depbin->db_type->bin_size;

					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					

				}
				break;
			case	LIST_COUNT_ID:	
				{
					AttrChunkPtr = pList->depbin->db_count->bin_chunk;
					ulChunkSize  = pList->depbin->db_count->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					

				}
				break;
			case	LIST_CAPACITY_ID:	
				{
					AttrChunkPtr = pList->depbin->db_capacity->bin_chunk;
					ulChunkSize  = pList->depbin->db_capacity->bin_size;

					rc = parse_attr_int((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;	
			case	LIST_DEBUG_ID:	
				{
					AttrChunkPtr = pList->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pList->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;	

			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized list attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/

	DDlAttribute *pAttribute = NULL;

	if (!(attrMask & LIST_VALID))	// added 23jan07 - sjv - spec change
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("ListValidity",
										LIST_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
			
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = 1; //Default Attribute
		attrList.push_back(pAttribute);
	}
			
	 ulItemMasks = attrMask | LIST_VALID;

	return SUCCESS;

}/*End eval_file */


/***************************************************************************
** FUNCTION NAME: eval_source()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6Source :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			
			case	SOURCE_LABEL_ID:
				{
					AttrChunkPtr = pSource->depbin->db_label->bin_chunk;
					ulChunkSize  = pSource->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	SOURCE_HELP_ID:
				{
					AttrChunkPtr = pSource->depbin->db_help->bin_chunk;
					ulChunkSize  = pSource->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	SOURCE_VALID_ID:	
				{
					AttrChunkPtr = pSource->depbin->db_valid->bin_chunk;
					ulChunkSize  = pSource->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	SOURCE_EMPHASIS_ID:	
				{
					AttrChunkPtr = pSource->depbin->db_emphasis->bin_chunk;
					ulChunkSize  = pSource->depbin->db_emphasis->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	SOURCE_LINETYPE_ID:	
				{
					AttrChunkPtr = pSource->depbin->db_linetype->bin_chunk;
					ulChunkSize  = pSource->depbin->db_linetype->bin_size;

					rc = parse_attr_line_type((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	SOURCE_LINECOLOR_ID:	
				{
					AttrChunkPtr = pSource->depbin->db_linecolor->bin_chunk;
					ulChunkSize  = pSource->depbin->db_linecolor->bin_size;

					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	SOURCE_YAXIS_ID:	
				{
					AttrChunkPtr = pSource->depbin->db_y_axis->bin_chunk;
					ulChunkSize  = pSource->depbin->db_y_axis->bin_size;

					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	SOURCE_INIT_ACTIONS_ID:
				{
					AttrChunkPtr = pSource->depbin->db_init_acts->bin_chunk;
					ulChunkSize  = pSource->depbin->db_init_acts->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;
			case	SOURCE_RFRSH_ACTIONS_ID:
				{
					AttrChunkPtr = pSource->depbin->db_rfrsh_acts->bin_chunk;
					ulChunkSize  = pSource->depbin->db_rfrsh_acts->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;
			case	SOURCE_EXIT_ACTIONS_ID:
				{
					AttrChunkPtr = pSource->depbin->db_exit_acts->bin_chunk;
					ulChunkSize  = pSource->depbin->db_exit_acts->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
			break;	
			case	SOURCE_MEMBERS_ID:	
				{
					AttrChunkPtr = pSource->depbin->db_members->bin_chunk;
					ulChunkSize  = pSource->depbin->db_members->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;	

			case	SOURCE_DEBUG_ID:	
				{
					AttrChunkPtr = pSource->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pSource->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized source attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/
	//Anil 230506: Start of Code for Default value handling
	DDlAttribute *pAttribute = NULL;
	if(!(attrMask & SOURCE_VALID))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("SourceValidity",
										SOURCE_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = 1;
		attrList.push_back(pAttribute);
	}
	if(!(attrMask & SOURCE_EMPHASIS))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("SourceEmphasis",
										SOURCE_EMPHASIS_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = 1;
		attrList.push_back(pAttribute);
	}

	attrMask = attrMask |SOURCE_VALID | SOURCE_EMPHASIS ;
	//Anil 230506: End of Code for Default value handling

	 ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_file */


/*Vibhor 290904: End of Code*/

/*stevev 22mar05 */
/***************************************************************************
** FUNCTION NAME: eval_image()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6Image :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	IMAGE_LABEL_ID:
				{
					AttrChunkPtr = pImage->depbin->db_label->bin_chunk;
					ulChunkSize  = pImage->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	IMAGE_HELP_ID:
				{
					AttrChunkPtr = pImage->depbin->db_help->bin_chunk;
					ulChunkSize  = pImage->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	IMAGE_VALID_ID:	
				{
					AttrChunkPtr = pImage->depbin->db_valid->bin_chunk;
					ulChunkSize  = pImage->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	IMAGE_LINK_ID:	
				{
					AttrChunkPtr = pImage->depbin->db_link->bin_chunk;
					ulChunkSize  = pImage->depbin->db_link->bin_size;

					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					

				}
				break;
			case	IMAGE_PATH_ID:	
				{
					AttrChunkPtr = pImage->depbin->db_path->bin_chunk;
					ulChunkSize  = pImage->depbin->db_path->bin_size;

					//rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					rc = parse_attr_int((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					

				}
				break;
			case	IMAGE_DEBUG_ID:	
				{
					AttrChunkPtr = pImage->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pImage->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized image attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/

	DDlAttribute *pAttribute = NULL;
	if(!(attrMask & IMAGE_VALID))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("SourceValidity",
										IMAGE_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = 1;
		attrList.push_back(pAttribute);
	}
	
	 ulItemMasks = attrMask | IMAGE_VALID;

	return SUCCESS;

}/*End eval_image */


/***************************************************************************
** FUNCTION NAME: eval_grid()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   attrMask and populates the variableAttributeList of the DDl item
**
** PARAMETER:
**
**   INPUT ...
**		None
**
**   OUTPUT ...
**		None	
**	
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if available attributes were parsed successfully
**   rc (return code) from various called routines in case of error
**
****************************************************************************/

int DDl6Grid :: eval_attrs(void)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	AllocAttributes();
	
	ItemAttrList :: iterator p;

	for(p = attrList.begin(); p != attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	GRID_LABEL_ID:
				{
					AttrChunkPtr = pGrid->depbin->db_label->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	GRID_HELP_ID:
				{
					AttrChunkPtr = pGrid->depbin->db_help->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	GRID_VALID_ID:	
				{
					AttrChunkPtr = pGrid->depbin->db_valid->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize); 
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;

			case	GRID_HEIGHT_ID:	
				{
					AttrChunkPtr = pGrid->depbin->db_height->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_height->bin_size;

					rc = parse_attr_scope_size((*p),AttrChunkPtr,ulChunkSize); //Vibhor 260804: Changed to int
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	GRID_WIDTH_ID:	
				{
					AttrChunkPtr = pGrid->depbin->db_width->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_width->bin_size;

					rc = parse_attr_scope_size((*p),AttrChunkPtr,ulChunkSize); //Vibhor 260804: Changed to int
					if(rc != DDL_SUCCESS)
						return rc;				
				}
				break;
			case	GRID_ORIENT_ID:	
				{
					AttrChunkPtr = pGrid->depbin->db_orient->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_orient->bin_size;

					rc = parse_attr_orient_size((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	GRID_HANDLING_ID:	
				{
					AttrChunkPtr = pGrid->depbin->db_handling->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_handling->bin_size;

					rc = parse_attr_bitstring((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	GRID_MEMBERS_ID:	
				{
					AttrChunkPtr = pGrid->depbin->db_members->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_members->bin_size;

					rc = parse_gridmembers_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	GRID_DEBUG_ID:	
				{
					AttrChunkPtr = pGrid->depbin->db_debug_info->bin_chunk;
					ulChunkSize  = pGrid->depbin->db_debug_info->bin_size;

					rc = parse_debug_info((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
					else
						strItemName = (*p)->pVals->debugInfo->symbol_name;
				}
				break;
			default:
#ifdef _DEBUG
				LOGIT(CERR_LOG,"Unrecognized grid attribute tag:%d\n",(*p)->byAttrID );
#endif
				break;

		}/*End switch*/

	}/*End for*/

	//Anil 230506: Start of Code for Default value handling
	DDlAttribute *pAttribute = NULL;
	if (!(attrMask & GRID_VALID_ID))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("GridValidity",
								GRID_VALID_ID,
								DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
								false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = 1; /*Default Attribute for Validity is true*/
		attrList.push_back(pAttribute);
	}
	if (!(attrMask & GRID_HEIGHT_ID))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("GridHeight",
								GRID_HEIGHT_ID,
								DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
								false);

		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = MEDIUM_DISPSIZE; /*Default Attribute for height is MEDIUM*/
		attrList.push_back(pAttribute);
	}

	if (!(attrMask & GRID_WIDTH_ID))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("GridWidth",
								GRID_WIDTH_ID,
								DDL_ATTR_DATA_TYPE_SCOPE_SIZE,
								false);

		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = MEDIUM_DISPSIZE; /*Default Attribute for Width is MEDIUM*/
		attrList.push_back(pAttribute);
	}

	if (!(attrMask & GRID_ORIENT_ID))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("GridOrientation",
								GRID_ORIENT_ID,
								DDL_ATTR_DATA_TYPE_INT,
								false);

		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = ORIENT_VERT; /*Default Attribute for Grid Orientation VERTICAL*/
		attrList.push_back(pAttribute);
	}

	if(!(attrMask & GRID_HANDLING))
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("GridHandling",
										GRID_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
		pAttribute->pVals = new VALUES;
		pAttribute->pVals->ullVal = READ_HANDLING | WRITE_HANDLING; /*Default Attribute for Grid handling is read write*/
		attrList.push_back(pAttribute);	
	}
	


	attrMask = attrMask | GRID_VALID_ID | GRID_HEIGHT_ID | GRID_WIDTH_ID | GRID_ORIENT_ID | GRID_HANDLING_ID;

	//Anil 230506: End of Code for Default value handling
	
	ulItemMasks = attrMask;

	return SUCCESS;

}/*End eval_image */

/* stevev 22mar05 - end */
