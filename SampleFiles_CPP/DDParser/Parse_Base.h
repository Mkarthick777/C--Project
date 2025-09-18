/*****************************************************************************************

	This file contains the declarations of the methods  for  Core Parsing methods which 
	are called by "attr_parse_......" methods, to parse different types of the attributes

******************************************************************************************/

/*..... from eval_base.c*/



#ifndef PARSEBASE_H
#define PARSEBASE_H

#include "DDlConditional.h"

#include "DDlItems.h"

extern DDlBlock *pBlock;

extern FILE * ferr;

/* Parse an integer valuse from a given binary chunk*/

int	ddl_parse_integer_func(unsigned char **chunkp, unsigned long *size, UINT64 *value);


int ddl_parse_tag_func(unsigned char **chunkp, unsigned long *size, unsigned long *tagp,
	unsigned long *lenp);


int ddl_parse_float(unsigned char **chunkp, unsigned long *size, float *value);


/* This one might be coupled with the first one*/

int ddl_parse_bitstring(unsigned char **chunkp, unsigned long *size, unsigned long *bitstring);


int ddl_parse_definition(unsigned char *chunk, unsigned long size, DEFINITION *def);


int ddl_parse_string(unsigned char **chunk, unsigned long *size, ddpSTRING *string);


int ddl_parse_expression( unsigned char **chunkp, unsigned long *size, ddpExpression *exprList);

/*This function parses a single enumeration from a given binary chunk*/
int ddl_parse_one_enum(unsigned char**chunkp,unsigned long *size,ENUM_VALUE *enmVal);

/* This function parses a list of enumerations from a given binary chunk,
  by calling ddl_parse_one_enum for each enumeration*/
int ddl_parse_enums(unsigned char **chunkp,unsigned long *size,ENUM_VALUE_LIST *pEnmList);

int ddl_parse_conditional(DDlConditional *pConditional, unsigned char **chunkp, unsigned long *size);

int ddl_parse_conditional_list(DDlConditional *pConditional, unsigned char **chunkp, unsigned long *size, unsigned long tagExpected);

int ddl_parse_ref(unsigned char **chunkp,unsigned long *size, ddpREFERENCE *ref);

int ddl_parse_reflist(unsigned char **chunkp,unsigned long *size,REFERENCE_LIST *reflist);

int ddl_parse_menuitems(unsigned char **chunkp,unsigned long *size,MENU_ITEM_LIST *pMenuItems);

unsigned long ddl_mask_width(unsigned long mask);

int ddl_parse_data_item(unsigned char **chunkp, unsigned long *size, DATA_ITEM* dataItem);

int ddl_parse_dataitems(unsigned char **chunkp, unsigned long *size, DATA_ITEM_LIST* dataItemList);

int ddl_parse_respcodes(unsigned char **chunkp, unsigned long *size, RESPONSE_CODE_LIST* respCodeList);

int ddl_parse_itemarray(unsigned char **chunkp, unsigned long *size, ITEM_ARRAY_ELEMENT_LIST* itemArray);

int ddl_parse_members(unsigned char **chunkp, unsigned long *size, MEMBER_LIST* memberList);

int ddl_parse_gridMembers(unsigned char **chunkp,unsigned long *size, GRID_SET_LIST *pGridSets);

/* stevev 15sep05 - needed elsewhere to get member name */
int parse_ascii_string(string& retStr, unsigned char** binChunk, unsigned long& size);

/*Whenever a via_param reference is encoumtered, this method is called to resolve the
  parmater name into a record or a variable ID from the DDlBlock
  Returns the resolved var / record ID if successful a "0" otherwise */
unsigned long ddl_resolve_param_reference(unsigned long paramName,unsigned short *usType);

int ddl_parse_linetype(unsigned char **chunkp, unsigned long *size, LINE_TYPE & lnType);



#endif /*PARSEBASE_H*/
	