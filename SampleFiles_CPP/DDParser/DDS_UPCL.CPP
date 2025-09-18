#ifndef lint
static char SCCSID[] = "@(#)dds_upcl.c	40.2  40  07 Jul 1998";
#endif /* lint */
/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *  This file contains all functions pertaining to upcalls.
 */

#include "pch.h"
//#include    <varargs.h>
#pragma warning (disable : 4786)
#include "pch.h"
//sjv06feb06 #include	<windows.h>
#include "ddbGeneral.h"

//#include    <stdio.h>
#include <iostream>

#include <iomanip>
#include	<stdlib.h>
#include    <assert.h>
#include    <ctype.h>
#include    <string.h>
#ifdef SUN
#include    <memory.h>		/* K&R only */
#endif



#include    "std.h"

#include "DDLDefs.h"
#include "DDlConditional.h"// includes--"Attributes.h"
#include "Retn_Code.h"
#include "Evl_lib.h"
//#include	"Dict.h"
//#include "PrintData.h"

//#include    "cm_lib.h"
//#include    "tst_cmn.h"
//#include    "tst_dds.h"
//#include	"pc_loc.h"
#include "logging.h"

#include "LitStringTable.h"

extern LitStringTable *pLitStringTable;


extern FLAT_DEVICE_DIR device_dir;

extern FLAT_DEVICE_DIR_6 device_dir_6;

extern bool bTokRev6Flag;

//#ifdef _PARSER_DEBUG

//extern FILE *fout;

//\#endif /*_PARSER_DEBUG*/

/*********************************************************************
 *
 *  Name: dict_compare
 *
 *  ShortDesc:  Compare the reference numbers in the DICT_TABLE_ENTRY
 *				struct passed.  This routine is used by the bsearch
 *				library function called in app_func_get_dict_string upcall.
 *
 *  Inputs:
 *		ptr_a - pointer to void.
 *		ptr_b - pointer to void.
 *
 *  Returns:
 *		< 0, > 0, or = 0 -- comparison result;
 *
 *  Author: Dave Raskin
 *
 **********************************************************************/
/*
static int
dict_compare(void const *ptr_a, void const *ptr_b)
{
	DICT_TABLE_ENTRY *dict_a, *dict_b;

	dict_a = (DICT_TABLE_ENTRY *) ptr_a;
	dict_b = (DICT_TABLE_ENTRY *) ptr_b;

	return ((int) ((long) dict_a->ref - (long) dict_b->ref));
}
*/

/***********************************************************************
 *
 * Name: dict_table_install()
 *
 * ShortDesc: Put new entry in dict_table.
 *
 * Description:
 *		This routine is called from parse() function, and puts new entry
 *		into the standard dictionary table.
 *
 * Inputs:
 *		ref -- standard dictionary reference number.
 *		name -- char pointer to the name of the item (not needed for dict_table).
 *		str -- char pointer to the standard dictionary string.
 *
 * Returns:
 *		void
 *
 * Author:
 *		Dave Raskin
 *
 **********************************************************************/
/*1
void
dict_table_install(unsigned long ref, char *name, char *str)
{

	dict_table[num_dict_table_entries].ref = ref;
	dict_table[num_dict_table_entries].len = (unsigned short) strlen(str);
	dict_table[num_dict_table_entries].str = str;

//	free(name);
	delete name;
	num_dict_table_entries++;

}

*/
/*---------------------------------------------------------------------
 ------------------------------ UPCALLS -------------------------------
 ---------------------------------------------------------------------*/

/*
 * pc_get_param_value upcall is in pc_lib.c
 */

/*********************************************************************
 *
 *  Name: app_func_get_dict_string
 *
 *  ShortDesc:  Upcall to retrieve a standard text dictionary string.
 *
 *  Description:  Returns a standard text dictionary string which has been 
 *				  specified by a standard text dictionary index.
 *
 *  Inputs:
 *      env_info - pointer to the env_info structure
 *		index - reference into the application's standard text dictionary,
 *
 *  Outputs:
 *		str - pointer to a ddpSTRING structure to be filled by the application.
 *
 *  Returns:
 *		DDL_DICT_STRING_NOT_FOUND;
 *		DDL_SUCCESS;
 *
 *  Author: Dave Raskin
 *
 **********************************************************************/
/* ARGSUSED */
/*1
int
app_func_get_dict_string(DDL_UINT index, ddpSTRING *str)
{

	DICT_TABLE_ENTRY *found_ptr;
	DICT_TABLE_ENTRY key;

	key.ref = index;

	/*
	 * Perform a binary search on the standard dictionary table to find the
	 * entry we're looking for.
	 */

/*2	found_ptr = (DICT_TABLE_ENTRY *) bsearch((char *) &key,
		(char *) dict_table, (unsigned) num_dict_table_entries,
		sizeof(DICT_TABLE_ENTRY), dict_compare);

	if (found_ptr == NULL) {

/*		printf("app_func_get_dict_string: Dictionary string, index %ld, not found\n",
				index); */
/*3
#ifdef _PARSER_DEBUG
		
		LOGIT(COUT_LOG,"\napp_func_get_dict_string: Dictionary string, index "<<index<<"not found\n";

#endif /*_PARSER_DEBUG*/

/*4		return DDL_DICT_STRING_NOT_FOUND;
	}
	else {

		/*
		 * Retrieve the information
		 */

/*5		str->flags = DONT_FREE_STRING;
		str->len = found_ptr->len;
		str->str = found_ptr->str;
		return DDL_SUCCESS;
	}
}


/*********************************************************************
 *
 *  Name: app_func_get_dev_spec_string
 *
 *  ShortDesc:  Upcall to retrieve a device specific string.
 *
 *  Description:
 *		Returns device specific string which has been specified as a
 *		device-specific string reference.  The device specific string
 *		table to use is specified by the block_handle passed in. 
 *		There is one device specific string table per device type.
 *
 *  Inputs:
 *		env_info - contains block_handle which is used to access device
 *					specific string table.
 *		dev_str_info - information about the string.
 *
 *  Outputs:
 *		str - pointer to a ddpSTRING structure to be filled by the application.
 *
 *  Returns:
 *		DDL_DEV_SPEC_STRING_NOT_FOUND;
 *		DDL_SUCCESS;
 *
 *  Author: Dave Raskin
 *
 **********************************************************************/
int
app_func_get_dev_spec_string(DEV_STRING_INFO *dev_str_info,
		ddpSTRING *str)
{

/*	int             rs; */
	STRING_TBL		*stringTbl;
if ( pLitStringTable != NULL )
{
}
	/*
	 * Find the pointer to the device specific table corresponding to block handle
	 */
	/*Vibhor 090804: We have two possibilities:
	  1. Device Directory == HART 5 Dev Dir
	  2. Device Directory == HART 6 Dev Dir
	  Since the design is such that only one of them would be allocated,
	*/
	if(true == bTokRev6Flag)
	{
		stringTbl = &(device_dir_6.string_tbl); //HART 6
	}
	else
	{
	
		stringTbl = &(device_dir.string_tbl); //HART 5
	}

	if(!stringTbl)
	{
		LOGIT(CERR_LOG,"\napp_func_get_dev_spec_string: Device specific string table not found\n");
		return DDL_DEV_SPEC_STRING_NOT_FOUND;
	}

	if (dev_str_info->id >= (unsigned long) stringTbl->count) {

		/*
		 * If string not found
		 */

/*		fprintf(fout,"app_func_get_dev_spec_string: Device specific string of ID %d not found\n",
			dev_str_info->id); */

#ifdef _PARSER_DEBUG
		
		LOGIT(COUT_LOG,"\napp_func_get_dev_spec_string: Device specific string of ID : %d not found\n",dev_str_info->id );

#endif /*_PARSER_DEBUG*/

		return DDL_DEV_SPEC_STRING_NOT_FOUND;
	}
	else {

		/*
		 * Retrieve the device specific string information.
		 */
		if ( str->str == NULL )// the memory has been transfered
		{
			if(!pLitStringTable)
			{
				LOGIT(CERR_LOG,
				"\napp_func_get_dev_spec_string: Literal string table not found\n");
				return DDL_DEV_SPEC_STRING_NOT_FOUND;
			}// else get the string
			
			str->flags = DONT_FREE_STRING;
			str->str   = pLitStringTable->get_lit_char(dev_str_info->id);
			str->len   = (unsigned short) strlen(str->str);
		}
		else
		{
			str->flags = DONT_FREE_STRING;
			str->str = stringTbl->list[dev_str_info->id].str;
			str->len = (unsigned short) strlen(str->str);
		}
		return DDL_SUCCESS;
	}
}


/***********************************************************************
 *
 * Name: fetch_hart_item
 *
 * ShortDesc: This is a stub; fill in the fetch functions
 *
 * Returns:
 *		SUCCESS
 *
 **********************************************************************/
/* ARGSUSED */
//int
//fetch_hart_item(DEVICE_HANDLE device_handle, ITEM_ID item_id,
//	unsigned long request_mask, void *item, ITEM_TYPE item_type)
//{

//	return SUCCESS;
//}


/***********************************************************************
 *
 * Name: free_flat_item_depbin()
 *
 * ShortDesc: This is a stub; fill in the fetch functions
 *
 * Returns:
 *		void
 *
 **********************************************************************/
/* ARGSUSED */
/*void
free_flat_item_depbin(void *flat_item, ITEM_TYPE item_type)
{

	return;
}
*/
/**********************************************************************
 *
 *  Name: app func_get_param_value
 *  ShortDesc: Return the param value from the parameter cache
 *
 *  Include:
 *		pc_loc.h
 *
 *  Description:
 *		app func_get_param_value is a wrapper around
 *		pc get_param_value.  It is needed because DDS
 *		requires that this service be called
 *		app_func_get_param_value.
 *
 *  Inputs:
 *
 *		env_info - environment information
 *		pc_p_ref - P_REF specifies po or ID and subindex of 
 *						requested param.
 *
 *  Outputs:
 *		param_value - EVAL_VAR_VALUE structure to return information
 *
 *  Returns:
 *		PC_SUCCESS, PC_NO_READ_OVER_CHANGED_PARAM, PC_NO_MEMORY, 
 *		BLTIN_VALUE_NOT_SET, PC_PARAM_NOT_FOUND, PC_INTERNAL_ERROR,
 *		PC_METHOD_COLLISION and errors from cr_param_read.
 *
 *	Also:
 *
 *  Author:
 *		Conrad Beaulieu
 *
 *********************************************************************/
/*
int
app_func_get_param_value(OP_REF *op_ref, EVAL_VAR_VALUE *param_value)
{

/*	P_REF		pc_p_ref;
	int			rs;

	if (op_ref == (OP_REF *) NULL) {
		return (PC_BAD_POINTER);
	} */

	/*
	 * The conversion to pc_p_ref MUST be removed and the parameter
	 * op_ref changed to pc_p_ref if and when DDS is changed.
	 */

	/*pc_p_ref.id			= op_ref->id;
	pc_p_ref.subindex	= op_ref->subindex;
	pc_p_ref.type		= PC_ITEM_ID_REF;

	rs = pc_get_param_value (env_info, &pc_p_ref, param_value);
	return (rs); */
/*	return SUCCESS;
}*/