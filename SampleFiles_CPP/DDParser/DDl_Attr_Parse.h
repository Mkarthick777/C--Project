
/*****************************************************************************************

	This file contains the declarations for parsing different Attribute Data Types

******************************************************************************************/


#ifndef DDLATTRPARSE_H
#define	DDLATTRPARSE_H


#include "DDlConditional.h"

/*

	  DDL_ATTR_DATA_TYPE_INT
	, DDL_ATTR_DATA_TYPE_UNSIGNED_LONG
	, DDL_ATTR_DATA_TYPE_FLOAT
	, DDL_ATTR_DATA_TYPE_DOUBLE
	, DDL_ATTR_DATA_TYPE_STRING
	, DDL_ATTR_DATA_TYPE_ITEM_ID
	, DDL_ATTR_DATA_TYPE_ITEM_ID_LIST
	, DDL_ATTR_DATA_TYPE_ENUM_LIST
	, DDL_ATTR_DATA_TYPE_REFERENCE_LIST // Just review this
	, DDL_ATTR_DATA_TYPE_TYPE_SIZE
	, DDL_ATTR_DATA_TYPE_TRANSACTION_LIST
	, DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST
	, DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST
	, DDL_ATTR_DATA_TYPE_OP_REF_TRAIL_LIST
	, DDL_ATTR_DATA_TYPE_DEFINITION
	, DDL_ATTR_DATA_TYPE_REFRESH_RELATION
	, DDL_ATTR_DATA_TYPE_UNIT_RELATION
	, DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST
	, DDL_ATTR_DATA_TYPE_MEMBER_LIST
	, DDL_ATTR_DATA_TYPE_LINE_TYPE

*/
/* Defined in DDl_Attr_Parse.cpp*/

extern FILE * ferr;

int parse_attr_bitstring(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_int(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_ulong(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_float(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_double(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_string(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

/* I guess this one could be merged with the ulong one */
int parse_attr_item_id(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_enum_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_reference_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_type_size(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_transaction_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_resp_code_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_menu_item_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_definition(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 
 
int parse_attr_refresh_relation(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_unit_relation(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_item_array_element_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_member_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_disp_edit_format(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size, unsigned short tagExpected);

int parse_attr_scaling_factor(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

bool set_min_max_index(DDlConditional* pCond, unsigned long ulIndex);

int parse_attr_min_max_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size, unsigned short tagExpected);

int parse_attr_expr(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 

int parse_attr_reference(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_array_name(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_meth_scope(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

/*Vibhor 270804: Start of Code*/
/*This subroutine if for parsing the line-type attribute for Waveform and Source*/
int parse_attr_line_type(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);
/*Vibhor 270804: End of Code*/
/*Vibhor 270804: Start of Code*/
int parse_attr_wavefrm_type(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_chart_type(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_menu_style(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

int parse_attr_scope_size(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);

/*Vibhor 270804: End of Code*/

/* stevev 22mar05 */
int parse_attr_orient_size(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);
int parse_gridmembers_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);
/* stevev 22mar05 end */

/* stevev 06may05 */
int parse_debug_info (DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);
/* stevev 06may05 end */


/* stevev 10may05 */
int parse_attr_param(METHOD_PARAM* pParam, unsigned char** binChunk, unsigned long& size);//helper
int parse_attr_method_type(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);
int parse_attr_param_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);
/* stevev 10may05 end */

// timj 4jan08
int parse_attr_time_scale(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size);
// timj 4jan08  end





#endif /* DDLATTRPARSE_H*/

