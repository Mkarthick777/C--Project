
#pragma warning (disable : 4786)

#include "pch.h"
//sjv06feb06 #include <windows.h>
#ifndef _WIN32_WCE
#include "std.h"	// removed for win ce PAW 23/04/09
#endif
#include "Eval_item.h"

#include "DDl_Attr_Parse.h"
#include "Retn_Code.h"
// moved to common as tags_sa.h    #include "DDLTags.h"
#include "Tags_sa.h"

#include <vector>




/*F*/
/***************************************************************************
** FUNCTION NAME: eval_variable()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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



int eval_variable(FLAT_VAR* fvar, unsigned long ulVarMask,DDlVariable *pVar)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	DDlAttribute *pAttribute = NULL;

	/*Vibhor 141103: Since HANDLING is not affected by TypeSize attribute & 
	 for some efficiency we need it at the beginning of the list , so parsing
	 it as the first attribute!!!*/

	if(ulVarMask & VAR_HANDLING)
	{

		pAttribute = (DDlAttribute*)new DDlAttribute("VarHandling",
										VAR_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
	
		AttrChunkPtr = fvar->depbin->db_handling->bin_chunk;
		ulChunkSize = fvar->depbin->db_handling->bin_size;
		rc = parse_attr_bitstring(pAttribute,AttrChunkPtr,ulChunkSize);
		if(rc != SUCCESS)
		{
			if (pAttribute) delete pAttribute;   //// HOMZ - Free up resources first
				return rc;
		}
		pVar->attrList.push_back(pAttribute);
	
	}
	else
	{
		pAttribute = (DDlAttribute*)new DDlAttribute("VarHandling",
										VAR_HANDLING_ID,
										DDL_ATTR_DATA_TYPE_BITSTRING,
										false);
		if (pAttribute != NULL)  // HOMZ
		{
		pAttribute->pVals = new VALUES;
			if (pAttribute->pVals) // HOMZ
		pAttribute->pVals->ullVal = READ_HANDLING | WRITE_HANDLING; /*Default Attribute*/
		}
		pVar->attrList.push_back(pAttribute);
	}
	pAttribute = NULL; /// HOMZ - Done!!! Set to NULL for sanity check
		
	
	/*Since some attributes become invalid depending upon the "type" attribute
	 of the variable , we will evaluate it first and restructure the attribute
	 list , if required ( by deleting the invalid attributes*/
	
	/*The first attribute is always TYPE_SIZE*/
	//pAttribute = pVar->attrList.begin();

	pAttribute = (DDlAttribute*)new DDlAttribute("VarTypeSize",
										VAR_TYPE_SIZE_ID,
										DDL_ATTR_DATA_TYPE_TYPE_SIZE,
										false);
	
	AttrChunkPtr = fvar->depbin->db_type_size->bin_chunk;
	ulChunkSize = fvar->depbin->db_type_size->bin_size;

	rc = parse_attr_type_size(pAttribute,AttrChunkPtr,ulChunkSize);
	
	if(rc != SUCCESS)
	{
		if (pAttribute) delete pAttribute;  // HOMZ - Free resources before going out of scope
		return rc;
	}

	switch (pAttribute->pVals->typeSize.type){
			case INTEGER:
			case UNSIGNED:
			case FLOATG_PT:
			case DOUBLE_FLOAT:
				ulVarMask &= INVALID_ARITH_TYPE_SUBATTR_MASK;
				break;

			case ENUMERATED:
			case BIT_ENUMERATED:
				ulVarMask &= INVALID_ENUM_TYPE_SUBATTR_MASK;
				break;

			case INDEX:
				ulVarMask &= INVALID_INDEX_TYPE_SUBATTR_MASK;
				break;

//FF			case EUC:
			case ASCII:
			case PACKED_ASCII:
			case PASSWORD:
//FF			case BITSTRING:
				ulVarMask &= INVALID_STRING_TYPE_SUBATTR_MASK;
				break;

			case HART_DATE_FORMAT:
//FF			case TIME:
//FF			case DATE_AND_TIME:
//FF			case DURATION:
				ulVarMask &= INVALID_DATE_TIME_TYPE_SUBATTR_MASK;
				break;

			default:	/* should never happen */
				ulVarMask &= INVALID_VAR_TYPE_SUBATTR_MASK;
				break;
	} /*End Switch pAttribute->pVals->typeSize.type */
	

	/*Now push the TYPE_SIZE attribute onto the Attribute List*/
	pVar->attrList.push_back(pAttribute);

	pAttribute = NULL; /// HOMZ - Done!!! Set to NULL for sanity check

	/*Now pass the updated attribute mask to allocate the attributes*/
	pVar->AllocAttributes(ulVarMask);

	/*Now we have the Attribute list for this variable ready !!!
	 Just iterate through it and parse each individual attribute
	 Note : We have already parsed the first attribute ie. TYPE_SIZE
	 so we'll start from the second attribute*/

	ItemAttrList :: iterator p;

	for(p = (pVar->attrList.begin())+1;p != pVar->attrList.end();p++)
	{	

			switch((*p)->byAttrID)
			{
			case	VAR_CLASS_ID:
				{
					AttrChunkPtr = fvar->depbin->db_class->bin_chunk;
					ulChunkSize = fvar->depbin->db_class->bin_size;
					rc = parse_attr_bitstring((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
				}
				break;
/*			case	VAR_HANDLING_ID:
				{
					AttrChunkPtr = fvar->depbin->db_handling->bin_chunk;
					ulChunkSize = fvar->depbin->db_handling->bin_size;
					rc = parse_attr_bitstring((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
				}
				break;
*/
			case	VAR_UNIT_ID :
				{	
					AttrChunkPtr = fvar->misc->depbin->db_unit->bin_chunk;
					ulChunkSize = fvar->misc->depbin->db_unit->bin_size;
					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
					
				}
				break;

			case	VAR_LABEL_ID:
				{
					AttrChunkPtr = fvar->depbin->db_label->bin_chunk;
					ulChunkSize = fvar->depbin->db_label->bin_size;
					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
	
				}
				break;
			case  	VAR_HELP_ID:
				{
					AttrChunkPtr = fvar->depbin->db_help->bin_chunk;
					ulChunkSize = fvar->depbin->db_help->bin_size;
					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
	
				}
				break;
/* removed 15oct12
			case	VAR_READ_TIME_OUT_ID:
				{	
					AttrChunkPtr = fvar->misc->depbin->db_read_time_out->bin_chunk;
					ulChunkSize = fvar->misc->depbin->db_read_time_out->bin_size;
					//here we have to parse a constant integral value from an expression;
					//Need to define an API for this
					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;					
				}
				break;

			case	VAR_WRITE_TIME_OUT_ID:
				{	
					AttrChunkPtr = fvar->misc->depbin->db_write_time_out->bin_chunk;
					ulChunkSize = fvar->misc->depbin->db_write_time_out->bin_size;
					//here we have to parse a constant integral value from an expression;
					//Need to define an API for this
					rc = parse_attr_expr((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
*****/
			case	VAR_VALID_ID:
				{	
					AttrChunkPtr = fvar->misc->depbin->db_valid->bin_chunk;
					ulChunkSize = fvar->misc->depbin->db_valid->bin_size;
					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;
					
				}
				break;
			case	VAR_PRE_READ_ACT_ID:
				{	
					AttrChunkPtr = fvar->actions->depbin->db_pre_read_act->bin_chunk;
					ulChunkSize = fvar->actions->depbin->db_pre_read_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_POST_READ_ACT_ID:
				{	
					AttrChunkPtr = fvar->actions->depbin->db_post_read_act->bin_chunk;
					ulChunkSize = fvar->actions->depbin->db_post_read_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_PRE_WRITE_ACT_ID:
				{	
					AttrChunkPtr = fvar->actions->depbin->db_pre_write_act->bin_chunk;
					ulChunkSize = fvar->actions->depbin->db_pre_write_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_POST_WRITE_ACT_ID:
				{	
					AttrChunkPtr = fvar->actions->depbin->db_post_write_act->bin_chunk;
					ulChunkSize = fvar->actions->depbin->db_post_write_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_PRE_EDIT_ACT_ID:
				{	
					AttrChunkPtr = fvar->actions->depbin->db_pre_edit_act->bin_chunk;
					ulChunkSize = fvar->actions->depbin->db_pre_edit_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_POST_EDIT_ACT_ID:
				{	
					AttrChunkPtr = fvar->actions->depbin->db_post_edit_act->bin_chunk;
					ulChunkSize = fvar->actions->depbin->db_post_edit_act->bin_size;
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
/*			case	VAR_RESP_CODES_ID:
				{	
					AttrChunkPtr = fvar->depbin->db_resp_codes->bin_chunk;
					ulChunkSize = fvar->depbin->db_resp_codes->bin_size;
					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break; */
			case	VAR_DISPLAY_ID:
				{	
					AttrChunkPtr = fvar->depbin->db_display->bin_chunk;
					ulChunkSize = fvar->depbin->db_display->bin_size;
					rc = parse_attr_disp_edit_format((*p),AttrChunkPtr,ulChunkSize,DISPLAY_FORMAT_TAG);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;
			case	VAR_EDIT_ID:
				{	
					AttrChunkPtr = fvar->depbin->db_edit->bin_chunk;
					ulChunkSize = fvar->depbin->db_edit->bin_size;
					rc = parse_attr_disp_edit_format((*p),AttrChunkPtr,ulChunkSize,EDIT_FORMAT_TAG);
					if(rc != SUCCESS)
						return rc; 
				}
				break;
			case	VAR_MIN_VAL_ID:
				{	
					AttrChunkPtr = fvar->misc->depbin->db_min_val->bin_chunk;
					ulChunkSize = fvar->misc->depbin->db_min_val->bin_size;
					rc = parse_attr_min_max_list((*p),AttrChunkPtr,ulChunkSize,MIN_VALUE_TAG);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;				
			case	VAR_MAX_VAL_ID:
				{	
					AttrChunkPtr = fvar->misc->depbin->db_max_val->bin_chunk;
					ulChunkSize = fvar->misc->depbin->db_max_val->bin_size;
					rc = parse_attr_min_max_list((*p),AttrChunkPtr,ulChunkSize,MAX_VALUE_TAG);
					if(rc != SUCCESS)
						return rc; 
					
				}
				break;	
			case	VAR_SCALE_ID:
				{	
					AttrChunkPtr = fvar->misc->depbin->db_scale->bin_chunk;
					ulChunkSize = fvar->misc->depbin->db_scale->bin_size;
					rc = parse_attr_scaling_factor((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;  
				}
				break;	
			case	VAR_ENUMS_ID:
				{	
					AttrChunkPtr = fvar->depbin->db_enums->bin_chunk;
					ulChunkSize = fvar->depbin->db_enums->bin_size;
					rc = parse_attr_enum_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;  
					
				}
				break;				
			case	VAR_INDEX_ITEM_ARRAY_ID:
				{	
					AttrChunkPtr = fvar->depbin->db_index_item_array->bin_chunk;
					ulChunkSize = fvar->depbin->db_index_item_array->bin_size;
					rc = parse_attr_array_name((*p),AttrChunkPtr,ulChunkSize);
					if(rc != SUCCESS)
						return rc;  
					
				}
				break;			
			default:
				/*Should Never Reach here!!!!*/
				break;
			}/*End Switch*/
	}/*End for*/

	/*Vibhor 271003: Though Handling is an optional attribute,
	 but wee need it for display processing & by definition a 
	 variable without a Handling can be both read & written,
	 so we will default this value to READ & WRITE if not already there*/

/*	 if (!(ulVarMask & VAR_HANDLING))
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

	 if (!(ulVarMask & VAR_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("VarValidity",
										VAR_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
		if (pAttribute != NULL)  // HOMZ
		{
			pAttribute->pVals = new VALUES;
		
			if (pAttribute->pVals != NULL)
				pAttribute->pVals->ullVal = 1; /*Default Attribute*/
		}
		pVar->attrList.push_back(pAttribute);
	 }

	 pVar->ulItemMasks = ulVarMask | VAR_CLASS |VAR_TYPE_SIZE| VAR_HANDLING | VAR_VALID;
	
	return SUCCESS;	

}/*End eval_variable*/


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_command()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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


/*
COMMAND_NUMBER_ID    
COMMAND_OPER_ID      
COMMAND_TRANS_ID     
COMMAND_RESP_CODES_ID

*/


int eval_command(FLAT_COMMAND* fcmd, unsigned long ulCmdMask,DDlCommand *pCmd)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	pCmd->AllocAttributes(ulCmdMask);

	ItemAttrList :: iterator p;

	for(p = pCmd->attrList.begin();p != pCmd->attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
		case	COMMAND_NUMBER_ID:
			{
	
				AttrChunkPtr = fcmd->depbin->db_number->bin_chunk;
				ulChunkSize = fcmd->depbin->db_number->bin_size;

				rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);

				if(rc != SUCCESS)
					return rc;
			}
			break;
		case	COMMAND_OPER_ID:
			{
				AttrChunkPtr = fcmd->depbin->db_oper->bin_chunk;
				ulChunkSize = fcmd->depbin->db_number->bin_size;

				rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);

				if(rc != SUCCESS)
					return rc;
			}
			break;
		case	COMMAND_TRANS_ID:
			{
				AttrChunkPtr = fcmd->depbin->db_trans->bin_chunk;
				ulChunkSize = fcmd->depbin->db_trans->bin_size;
				rc = parse_attr_transaction_list((*p),AttrChunkPtr,ulChunkSize);
				if(rc != SUCCESS)
					return rc;
			}

			break;
		case	COMMAND_RESP_CODES_ID:
			{
				AttrChunkPtr = fcmd->depbin->db_resp_codes->bin_chunk;
				ulChunkSize = fcmd->depbin->db_resp_codes->bin_size;
				rc = parse_attr_resp_code_list((*p),AttrChunkPtr,ulChunkSize);
				if(rc != SUCCESS)
					return rc;
				
			}
			break;
		default:
			/*should never reach here*/
			break;
		}
	}/*End for */

	pCmd->ulItemMasks = ulCmdMask;

return  SUCCESS;

}/*End eval_command*/

/*F*/
/***************************************************************************
** FUNCTION NAME: eval_menu()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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

int eval_menu(FLAT_MENU* fmenu, unsigned long ulMenuMask,DDlMenu *pMenu)
{
	
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	pMenu->AllocAttributes(ulMenuMask);

	ItemAttrList :: iterator p;

	for(p = pMenu->attrList.begin();p != pMenu->attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case MENU_LABEL_ID:
				{
					AttrChunkPtr = fmenu->depbin->db_label->bin_chunk;
					ulChunkSize = fmenu->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);

					if(rc != SUCCESS)
						return rc;
				}
				break;

			case MENU_ITEMS_ID:
				{
					AttrChunkPtr = fmenu->depbin->db_items->bin_chunk;
					ulChunkSize = fmenu->depbin->db_items->bin_size;

					rc = parse_attr_menu_item_list((*p),AttrChunkPtr,ulChunkSize);

					if(rc != SUCCESS)
						return rc;
				}
				break;

			default:
				/*should never reach here*/
				break;

		}/*End switch*/

	}/*End for*/

	pMenu->ulItemMasks = ulMenuMask;

	return SUCCESS;

}/*End eval_menu */

/*F*/
/***************************************************************************
** FUNCTION NAME: eval_edit_display()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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


int eval_edit_display(FLAT_EDIT_DISPLAY* fedit, unsigned long ulEditDispMask,DDlEditDisplay *pEditDisp)
{

	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	pEditDisp->AllocAttributes(ulEditDispMask);
	
	ItemAttrList :: iterator p;

	for(p = pEditDisp->attrList.begin();p != pEditDisp->attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	EDIT_DISPLAY_LABEL_ID:
				{

					AttrChunkPtr = fedit->depbin->db_label->bin_chunk;
					ulChunkSize  = fedit->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_EDIT_ITEMS_ID:
				{
					AttrChunkPtr = fedit->depbin->db_edit_items->bin_chunk;
					ulChunkSize  = fedit->depbin->db_edit_items->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	EDIT_DISPLAY_DISP_ITEMS_ID:
				{

					AttrChunkPtr = fedit->depbin->db_disp_items->bin_chunk;
					ulChunkSize  = fedit->depbin->db_disp_items->bin_size;
				
					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_PRE_EDIT_ACT_ID:
				{
					AttrChunkPtr = fedit->depbin->db_pre_edit_act->bin_chunk;
					ulChunkSize  = fedit->depbin->db_pre_edit_act->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	EDIT_DISPLAY_POST_EDIT_ACT_ID:
				{
					AttrChunkPtr = fedit->depbin->db_post_edit_act->bin_chunk;
					ulChunkSize  = fedit->depbin->db_post_edit_act->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			default:
				break;
		
		}/*End switch*/

	}/*End for*/

	pEditDisp->ulItemMasks = ulEditDispMask;

	return SUCCESS;


}/*End eval_edit_display */


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_method()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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

int eval_method(FLAT_METHOD* fmeth, unsigned long ulMethodMask,DDlMethod *pMeth)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	DDlAttribute *pAttribute = NULL;

	pMeth->AllocAttributes(ulMethodMask);

	ItemAttrList :: iterator p;

	int i = 0;
	int sizeoflist = pMeth->attrList.size ();

	//for(p = pMeth->attrList.begin();p != pMeth->attrList.end();p++)
	p = pMeth->attrList.begin();
	while (p != pMeth->attrList.end())
	{
		i++;

		switch((*p)->byAttrID)
		{
			case	METHOD_CLASS_ID:
				{
					AttrChunkPtr = fmeth->depbin->db_class->bin_chunk;
					ulChunkSize  = fmeth->depbin->db_class->bin_size;

					rc = parse_attr_bitstring((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	METHOD_LABEL_ID:
				{
					AttrChunkPtr = fmeth->depbin->db_label->bin_chunk;
					ulChunkSize  = fmeth->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				 
				}
				break;
			case	METHOD_HELP_ID:
				{
					AttrChunkPtr = fmeth->depbin->db_help->bin_chunk;
					ulChunkSize  = fmeth->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			case	METHOD_DEF_ID:
				{
					AttrChunkPtr = fmeth->depbin->db_def->bin_chunk;
					ulChunkSize  = fmeth->depbin->db_def->bin_size;

					rc = parse_attr_definition((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	METHOD_VALID_ID:
				{
					AttrChunkPtr = fmeth->depbin->db_valid->bin_chunk;
					ulChunkSize  = fmeth->depbin->db_valid->bin_size;

					rc = parse_attr_ulong((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			case	METHOD_SCOPE_ID:
				{
					AttrChunkPtr = fmeth->depbin->db_scope->bin_chunk;
					ulChunkSize  = fmeth->depbin->db_scope->bin_size;

					rc = parse_attr_meth_scope((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			default:
				break;
		}/*End switch*/

		p++;
	}/*End while */

/*Vibhor 240204: Start of Code*/
/*If a methode has no Help defined for it we'll have default string*/
	
	if (!(ulMethodMask & METHOD_HELP))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("MethodHelp",
										METHOD_HELP_ID,
										DDL_ATTR_DATA_TYPE_STRING,
										false);
	
			
		pAttribute->pVals = new VALUES;

		pAttribute->pVals->strVal = new ddpSTRING;

		pAttribute->pVals->strVal->str = new char[18];
#ifdef MODIFIED_BY_SS
		memset( pAttribute->pVals->strVal->str, 0, sizeof(pAttribute->pVals->strVal->str));
		//strcpy(pAttribute->pVals->strVal->str,""/*"No Help Available"*/); //Removed No Help Available, DDHost Test requires a blank label, POB - 12/2/2013
#endif
		pAttribute->pVals->strVal->strType = DEV_SPEC_STRING_TAG; //This will ensure cleanup.
		pAttribute->pVals->strVal->flags   = FREE_STRING;         // stevev 7aug7
		
		pMeth->attrList.push_back(pAttribute);

		// this gets overwritten...pMeth->ulItemMasks = ulMethodMask | METHOD_HELP ;
		ulMethodMask |= METHOD_HELP ; // sent to pMeth later

	 }

/*Vibhor 240204: End of Code*/

	if (!(ulMethodMask & METHOD_VALID))
	 {
		 pAttribute = (DDlAttribute*)new DDlAttribute("MethodValidity",
										METHOD_VALID_ID,
										DDL_ATTR_DATA_TYPE_UNSIGNED_LONG,
										false);
	
			
		pAttribute->pVals = new VALUES;
		
		pAttribute->pVals->ullVal = 1; /*Default Attribute*/

		pMeth->attrList.push_back(pAttribute);
	 }

	
	pMeth->ulItemMasks = ulMethodMask | METHOD_VALID ;

	return SUCCESS;
}/*End eval_method*/


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_refresh()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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

int eval_refresh(FLAT_REFRESH* frfrsh, unsigned long ulRefrshMask,DDlRefresh *pRefrsh)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	pRefrsh->AllocAttributes(ulRefrshMask);

	ItemAttrList :: iterator p;

	for(p = pRefrsh->attrList.begin();p != pRefrsh->attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	REFRESH_ITEMS_ID:
				{
					AttrChunkPtr = frfrsh->depbin->db_items->bin_chunk;
					ulChunkSize  = frfrsh->depbin->db_items->bin_size;

					rc = parse_attr_refresh_relation((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				
				}
				break;
			default:
				break;
		}/*End switch*/

	}/*End for*/

	pRefrsh->ulItemMasks = ulRefrshMask;
		
	return SUCCESS;
}/*End eval_refresh */


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_unit()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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

int eval_unit(FLAT_UNIT* funit, unsigned long ulUnitMask,DDlUnit *pUnit)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	pUnit->AllocAttributes(ulUnitMask);

	ItemAttrList :: iterator p;

	for(p = pUnit->attrList.begin();p != pUnit->attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	UNIT_ITEMS_ID:
				{
					AttrChunkPtr = funit->depbin->db_items->bin_chunk;
					ulChunkSize  = funit->depbin->db_items->bin_size;

					rc = parse_attr_unit_relation((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}
				break;
			default:
				break;
				
		}/*End switch */

	}/*End for*/

	pUnit->ulItemMasks = ulUnitMask;
	return SUCCESS;

}/*End eval_unit */

/*F*/
/***************************************************************************
** FUNCTION NAME: eval_wao()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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

int eval_wao(FLAT_WAO* fwao, unsigned long ulWaoMask,DDlWao *pWao)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	pWao->AllocAttributes(ulWaoMask);

	ItemAttrList :: iterator p;

	for(p = pWao->attrList.begin();p != pWao->attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	WAO_ITEMS_ID:
				{
					AttrChunkPtr = fwao->depbin->db_items->bin_chunk;
					ulChunkSize  = fwao->depbin->db_items->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS)
						return rc;
				}

				break;
			default:
				break;

		}/*End switch*/

	}/*End for*/

	pWao->ulItemMasks = ulWaoMask;

	return SUCCESS;

}/*End eval_wao */


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_item_array()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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



int eval_item_array(FLAT_ITEM_ARRAY* fiarr, unsigned long ulItemArrayMask,DDlItemArray *pIArray)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;
	
	pIArray->AllocAttributes(ulItemArrayMask);
	
	ItemAttrList :: iterator p;

	for(p = pIArray->attrList.begin(); p != pIArray->attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	ITEM_ARRAY_ELEMENTS_ID:
				{
					AttrChunkPtr = fiarr->depbin->db_elements->bin_chunk;
					ulChunkSize  = fiarr->depbin->db_elements->bin_size;

					rc = parse_attr_item_array_element_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc; 
				}

				break;
			case	ITEM_ARRAY_LABEL_ID:
				{
					AttrChunkPtr = fiarr->depbin->db_label->bin_chunk;
					ulChunkSize  = fiarr->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	ITEM_ARRAY_HELP_ID:
				{
					AttrChunkPtr = fiarr->depbin->db_help->bin_chunk;
					ulChunkSize  = fiarr->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			default:
				break;
		}/*End switch*/

	}/*End for */

	pIArray->ulItemMasks = ulItemArrayMask;

	return SUCCESS;

}/*End eval_item_array */


/*F*/
/***************************************************************************
** FUNCTION NAME: eval_collection()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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


int eval_collection(FLAT_COLLECTION* fcoll, unsigned long ulCollectionMask,DDlCollection *pColl)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	pColl->AllocAttributes(ulCollectionMask);
	
	ItemAttrList :: iterator p;

	for(p = pColl->attrList.begin(); p != pColl->attrList.end(); p++)
	{
		switch((*p)->byAttrID)
		{
			case	COLLECTION_MEMBERS_ID:	
				{
					AttrChunkPtr = fcoll->depbin->db_members->bin_chunk;
					ulChunkSize  = fcoll->depbin->db_members->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					

				}
				break;
			case	COLLECTION_LABEL_ID:
				{
					AttrChunkPtr = fcoll->depbin->db_label->bin_chunk;
					ulChunkSize  = fcoll->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	COLLECTION_HELP_ID:
				{
					AttrChunkPtr = fcoll->depbin->db_help->bin_chunk;
					ulChunkSize  = fcoll->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			default:
				break;

		}/*End switch*/

	}/*End for*/

	pColl->ulItemMasks = ulCollectionMask;
	
	return SUCCESS;

}/*End eval_collection */

/*F*/
/***************************************************************************
** FUNCTION NAME: eval_record()
**
** PURPOSE:
**   This function evaluates the attributes of a variable indicated thru 
**   ulVarMask and populates the variableAttributeList of the DDlVariable
**
** PARAMETER:
**
**   INPUT ...
**     fvar		Pointer to a FLAT_VAR structure, which holds the binary 
**              chunks of the attributes
**	   ulVarMask Mask of attributes available for this variable
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

int eval_record(FLAT_RECORD* frec, unsigned long ulRecordMask,DDlRecord *pRec)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;
	
	pRec->AllocAttributes(ulRecordMask);

	ItemAttrList :: iterator p;

	for(p = pRec->attrList.begin();p != pRec->attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	RECORD_MEMBERS_ID:
				{
					AttrChunkPtr = frec->depbin->db_members->bin_chunk;
					ulChunkSize  = frec->depbin->db_members->bin_size;
					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;					
				}
				break;
			case	RECORD_LABEL_ID:
				{
					AttrChunkPtr = frec->depbin->db_label->bin_chunk;
					ulChunkSize  = frec->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	RECORD_HELP_ID:
				{
					AttrChunkPtr = frec->depbin->db_help->bin_chunk;
					ulChunkSize  = frec->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
/*Vibhor 311003: Commenting this one*/
/*			case	RECORD_RESP_CODES_ID:
				{
					AttrChunkPtr = frec->depbin->db_resp_codes->bin_chunk;
					ulChunkSize  = frec->depbin->db_resp_codes->bin_size;

					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
*/ 
				break;
			default:
				break;

		}/*End switch*/

	}/*End for */

	pRec->ulItemMasks = ulRecordMask;
	
	return SUCCESS;

}/*End eval_record */



int eval_block(FLAT_BLOCK* fblock, unsigned long ulBlockMask,DDlBlock *pBlock)
{
	int rc;
	
	unsigned char * AttrChunkPtr = NULL;
	unsigned long ulChunkSize = 0;

	pBlock->AllocAttributes(ulBlockMask);

	ItemAttrList :: iterator p;

	for(p = pBlock->attrList.begin(); p != pBlock->attrList.end();p++)
	{
		switch((*p)->byAttrID)
		{
			case	BLOCK_CHARACTERISTIC_ID:
				{
					AttrChunkPtr = fblock->depbin->db_characteristic->bin_chunk;
					ulChunkSize  = fblock->depbin->db_characteristic->bin_size;
					rc = parse_attr_reference((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_LABEL_ID:
				{
					AttrChunkPtr = fblock->depbin->db_label->bin_chunk;
					ulChunkSize  = fblock->depbin->db_label->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_HELP_ID:
				{
					AttrChunkPtr = fblock->depbin->db_help->bin_chunk;
					ulChunkSize  = fblock->depbin->db_help->bin_size;

					rc = parse_attr_string((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_PARAM_ID:
				{
					AttrChunkPtr = fblock->depbin->db_param->bin_chunk;
					ulChunkSize  = fblock->depbin->db_param->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_MENU_ID:
				{
					AttrChunkPtr = fblock->depbin->db_menu->bin_chunk;
					ulChunkSize  = fblock->depbin->db_menu->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_EDIT_DISP_ID: 
				{
					AttrChunkPtr = fblock->depbin->db_edit_disp->bin_chunk;
					ulChunkSize  = fblock->depbin->db_edit_disp->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_METHOD_ID: 
				{
					AttrChunkPtr = fblock->depbin->db_method->bin_chunk;
					ulChunkSize  = fblock->depbin->db_method->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_REFRESH_ID: 
				{
					AttrChunkPtr = fblock->depbin->db_refresh->bin_chunk;
					ulChunkSize  = fblock->depbin->db_refresh->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_UNIT_ID:
				{
					AttrChunkPtr = fblock->depbin->db_unit->bin_chunk;
					ulChunkSize  = fblock->depbin->db_unit->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_WAO_ID: 
				{
					AttrChunkPtr = fblock->depbin->db_wao->bin_chunk;
					ulChunkSize  = fblock->depbin->db_wao->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_COLLECT_ID:
				{
					AttrChunkPtr = fblock->depbin->db_collect->bin_chunk;
					ulChunkSize  = fblock->depbin->db_collect->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_ITEM_ARRAY_ID:
				{
					AttrChunkPtr = fblock->depbin->db_item_array->bin_chunk;
					ulChunkSize  = fblock->depbin->db_item_array->bin_size;

					rc = parse_attr_reference_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			case	BLOCK_PARAM_LIST_ID:
				{
					AttrChunkPtr = fblock->depbin->db_param_list->bin_chunk;
					ulChunkSize  = fblock->depbin->db_param_list->bin_size;

					rc = parse_attr_member_list((*p),AttrChunkPtr,ulChunkSize);
					if(rc != DDL_SUCCESS) 
						return rc;
				}
				break;
			default:
				break;


		}/*End switch*/

	}/*End for*/

	pBlock->ulItemMasks = ulBlockMask;

	return SUCCESS;

}/*End eval_block*/