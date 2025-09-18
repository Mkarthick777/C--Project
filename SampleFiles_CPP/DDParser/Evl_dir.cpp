/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *	This file contains the functions for evaluating a block or device
 *	directory.
 */
#pragma warning (disable : 4786)
#include "pch.h"
//sjv06feb06 #include <windows.h>
#include "ddbGeneral.h"
#include <fstream>
#include <string>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "std.h"
#include "evl_loc.h"
#include "Parse_Base.h"
#include "panic.h"

// 12dec07 timj
#include "Dictionary.h"
#include "LitStringTable.h"
extern CDictionary *pGlobalDict; /*The Global Dictionary object*/
extern LitStringTable *pLitStringTable; /*The Global literal string table object*/

#include "ParserEndian.h"

#define TABLE_OFFSET_INVALID	-1

#undef max	/* macro takes precedence over function */

#define MAXIMUM_INT   (std::numeric_limits<int>::max())
#define MAXIMUM_USHRT (std::numeric_limits<unsigned short>::max())

//jad chinese
char* externalString(unsigned short newlen, char* rawstring, int rawlen, const ulong stringNumber, FILE *fp);
char* getNewDictString(ulong stringNumber);
char* getNewLitString(ulong stringNumber, FILE *fp);

/*********************************************************************
 *
 *	Name: eval_dir_string_tbl
 *	ShortDesc: evaluate the STRING_TBL
 *
 *	Description:
 *		eval_dir_string_tbl will load the STRING_TBL structure
 *		by parsing the binary in bin
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		string_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_string_tbl(STRING_TBL *string_tbl, BININFO *bin)
{
	//jad chinese
	//had to change name so can use string functions
	//ddpSTRING			*string = NULL;	 	/* temp pointer for the list */
	ddpSTRING			*xstring = NULL;	 	/* temp pointer for the list */
	ddpSTRING			*end_string =NULL;	/* end pointer for the list */
	DDL_UINT    	temp_int;		/* integer value */
	unsigned long	size =0;			/* temp size */
	unsigned char	*root_ptr = NULL;		/* temp pointer */
	int				rc = 0;				/* return code */
#ifdef MODIFIED_BY_SS
	assert(bin && bin->chunk && bin->size);
	//ASSERT_DBG(bin && bin->chunk && bin->size);
#endif
#ifdef DDSTEST
	TEST_FAIL(EVAL_STRING_TBL);
#endif /* DDSTEST */

	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		string_tbl->count = 0;
		string_tbl->list = NULL;
		return DDL_SUCCESS;
	}

	assert(temp_int < MAXIMUM_INT);

	string_tbl->count = (int) temp_int;

	/* malloc the list */
	string_tbl->list = (ddpSTRING *)new ddpSTRING[((size_t)(temp_int))];
	if (string_tbl->list == NULL) {

		string_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}

	/* malloc the root */
/*	string_tbl->root =
		(unsigned char *)new char[((size_t) (bin->size ))];
	if (string_tbl->root == NULL) {

		return DDL_MEMORY_ERROR;
	}

	/* load list with zeros */
/*	(void)memset((char *) string_tbl->list, 0,
			(size_t) (temp_int * sizeof(ddpSTRING))); */

	/* copy the chunk to the root */
/*	(void)memcpy((char *) string_tbl->root, (char *) bin->chunk,
			(size_t) (bin->size * sizeof(unsigned char))); */

	/*
	 * load the list
	 */


	 //jad chinese
	 //ugh, how to get language
	 //how to get dd to load
	//LOGIT(CERR_LOG, "before languageDD string");

	bool bDeviceObject;
	FILE* fp = NULL;
	int ilang = 0;

	ilang = pGlobalDict->GetlanguageCode();

	//LOGIT(CERR_LOG, "languageDD string = %d", ilang);

	if (ilang == 1)
	{
		//check if file exists
		LOGIT(CERR_LOG, "chinese");

		//get dd loaded filename, change extension to .txt
		string sFileName;
		size_t index;
		sFileName = pGlobalDict->pchFileName;
		index = sFileName.find(".fm8");
		sFileName.replace(index, 4, ".txt");

		bDeviceObject = false;

		//want to skip the standard device object
		size_t loc;
		loc = sFileName.find("\\0000f9\\0083\\0101");

		if (loc != string::npos)
			bDeviceObject = true;
		else
		{
//#ifdef MODIFIED_BY_SS
//			//rewritten code 
//			std::ifstream file(sFileName, std::ios::binary); // Open file in binary mode
//
//			if (!file.is_open()) {
//				LOGIT(CERR_LOG, "Error opening %s", sFileName.c_str());
//				// Skip using this file
//				bDeviceObject = false;
//			}
//#endif
			
			fopen_s(&fp, sFileName.c_str(), "rb");

			if (!fp)
			{
				LOGIT(CERR_LOG, "Error opening %s", sFileName.c_str());
				//skip using this file
				bDeviceObject = false;
			}
			

		}
	}

	size = bin->size;
	//root_ptr = string_tbl->root;
	root_ptr = (unsigned char *)bin->chunk;

	//LOGIT(CERR_LOG, "Before string list loop");

	int i = 0;
	for (xstring = string_tbl->list, end_string = xstring + temp_int; xstring < end_string; xstring++) 
	{
		DDL_PARSE_INTEGER(&root_ptr, &size, &temp_int);
		assert(temp_int < MAXIMUM_USHRT );
		xstring->len = (unsigned short) temp_int;		

		//jad chinese
		if (fp == NULL)
		{
			xstring->str = new char[xstring->len + 1];	// this is start of leak |en|Temperature  PAw 08/04/09 
			//strcpy(xstring->str, (char*)root_ptr);
			std::copy(root_ptr, root_ptr + xstring->len, xstring->str);
			xstring->str[xstring->len] = '\0'; // Null-terminate the string
		}
		else
			xstring->str = externalString(xstring->len, (char*)root_ptr, xstring->len, i, fp);

		size -=  (unsigned)temp_int; /* decrement size */
		root_ptr += temp_int;        /* increment the root pointer */

		xstring->flags = DONT_FREE_STRING;
		i++;

		//LOGIT(CERR_LOG, " - %s", xstring->str);
	}

	//LOGIT(CERR_LOG, "After string list loop");

	if (fp)
		fclose(fp);

	return DDL_SUCCESS;
}

/*********************************************************************
 *
 *	Name: eval_dir_domain_tbl
 *	ShortDesc: evaluate the DOMAIN_TBL
 *
 *	Description:
 *		eval_dir_domain_tbl will load the DOMAIN_TBL structure
 *		by parsing the binary in bin
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		domain_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_domain_tbl(DOMAIN_TBL *domain_tbl, BININFO *bin)
{

	DOMAIN_TBL_ELEM		*element =NULL;	 /* temp pointer for the list */
	DOMAIN_TBL_ELEM		*end_element =NULL;/* end pointer for the list */
	DDL_UINT        	temp_int;	 /* integer value */
	int					rc;			 /* return code */
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif
#ifdef DDSTEST
	TEST_FAIL(EVAL_DOMAIN_TBL);
#endif /* DDSTEST */

	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		domain_tbl->count = 0;
		domain_tbl->list = NULL;
		return DDL_SUCCESS;
	}

	domain_tbl->count = (int) temp_int;

	/* malloc the list */
	domain_tbl->list =
		(DOMAIN_TBL_ELEM *)new DOMAIN_TBL_ELEM[((size_t)(temp_int))];

	if (domain_tbl->list == NULL) {

		domain_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}

	/* load list with zeros */
	(void)memset((char *) domain_tbl->list, 0,
			(size_t) temp_int * sizeof(DOMAIN_TBL_ELEM));

	/*
	 * load the list
	 */

	for (element = domain_tbl->list, end_element = element + temp_int;
			element < end_element; element++) {

		/* parse ITEM_ID */

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->item_id = (ITEM_ID) temp_int;

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->dd_ref.object_index = (OBJECT_INDEX) temp_int;
	}

	return DDL_SUCCESS;
}


/*********************************************************************
 *
 *	Name: eval_dir_item_tbl
 *	ShortDesc: evaluate the ITEM_TBL
 *
 *	Description:
 *		eval_dir_item_tbl will load the ITEM_TBL structure
 *		by parsing the binary in bin
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		item_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_item_tbl(ITEM_TBL *item_tbl, BININFO *bin)
{

	ITEM_TBL_ELEM	*element =NULL;	  /* temp pointer for the list */
	ITEM_TBL_ELEM	*end_element = NULL; /* end pointer for the list */
	DDL_UINT       	temp_int;	  /* integer value */
	int				rc;			  /* return code */
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif

#ifdef DDSTEST
	TEST_FAIL(EVAL_ITEM_TBL);
#endif /* DDSTEST */

	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		item_tbl->count = 0;
		item_tbl->list = NULL;
		return DDL_SUCCESS;
	}

	item_tbl->count = (int) temp_int;

	/* malloc the list */
	item_tbl->list =
		(ITEM_TBL_ELEM *)new ITEM_TBL_ELEM[((size_t) (temp_int ))];

	if (item_tbl->list == NULL) {

		item_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}

	/* load list with zeros */
	(void)memset((char *) item_tbl->list, 0,
		(size_t) temp_int * sizeof(ITEM_TBL_ELEM));

	/*
	 * load the list
	 */

	for (element = item_tbl->list,	end_element = element + temp_int;
		 element < end_element; element++) {

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->item_id = (ITEM_ID) temp_int;

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->dd_ref.object_index = (OBJECT_INDEX) temp_int;

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->item_type = (ITEM_TYPE) temp_int;
	}

	return DDL_SUCCESS;
}

/*********************************************************************
 *
 *	Name: eval_dir_prog_tbl
 *	ShortDesc: evaluate the PROG_TBL
 *
 *	Description:
 *		eval_dir_prog_tbl will load the PROG_TBL structure
 *		by parsing the binary in bin
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		prog_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_prog_tbl(PROG_TBL *prog_tbl, BININFO *bin)
{

	PROG_TBL_ELEM		*element = NULL;	 /* temp pointer for the list */
	PROG_TBL_ELEM		*end_element =NULL;/* end pointer for the list */
	DDL_UINT        	temp_int;	 /* integer value */
	int					rc;			 /* return code */

#ifdef MODIFIED_BY_SS
	// ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif 
#ifdef DDSTEST
	TEST_FAIL(EVAL_PROG_TBL);
#endif /* DDSTEST */

	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		prog_tbl->count = 0;
		prog_tbl->list = NULL;
		return DDL_SUCCESS;
	}

	prog_tbl->count = (int) temp_int;

	/* malloc the list */
	prog_tbl->list =
		(PROG_TBL_ELEM *)new PROG_TBL_ELEM[((size_t) (temp_int))];

	if (prog_tbl->list == NULL) {

		prog_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}

	/* load list with zeros */
	(void)memset((char *) prog_tbl->list, 0,
			(size_t) temp_int * sizeof(PROG_TBL_ELEM));

	/*
	 * load the list
	 */

	for (element = prog_tbl->list,	end_element = element + temp_int;
		 element < end_element; element++) {

		/* parse ITEM_ID */

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->item_id = (ITEM_ID) temp_int;

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->dd_ref.object_index = (OBJECT_INDEX) temp_int;
	}

	return DDL_SUCCESS;
}


/*********************************************************************
 *
 *	Name: eval_dir_local_var_tbl
 *	ShortDesc: evaluate the LOCAL_VAR_TBL
 *
 *	Description:
 *		eval_dir_local_var_tbl will load the LOCAL_VAR_TBL structure
 *		by parsing the binary in bin
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		local_var_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_local_var_tbl(LOCAL_VAR_TBL *local_var_tbl, BININFO *bin)
{

	LOCAL_VAR_TBL_ELEM	*element =NULL;	  /* temp pointer for the list */
	LOCAL_VAR_TBL_ELEM	*end_element =NULL; /* end pointer for the list */
	DDL_UINT        	temp_int;	  /* integer value */
	int					rc;			  /* return code */

#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif
	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		local_var_tbl->count = 0;
		local_var_tbl->list = NULL;
		return DDL_SUCCESS;
	}

	local_var_tbl->count = (int) temp_int;

	/* malloc the list */
	local_var_tbl->list =
		(LOCAL_VAR_TBL_ELEM *)new LOCAL_VAR_TBL_ELEM[((size_t)(temp_int))];

	if (local_var_tbl->list == NULL) {

		local_var_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}

	/* load list with zeros */
	(void)memset((char *) local_var_tbl->list, 0,
			(size_t) temp_int * sizeof(LOCAL_VAR_TBL_ELEM));

	/*
	 * load the list
	 */

	for (element = local_var_tbl->list,	end_element = element + temp_int;
		 element < end_element; element++) {

		/* parse ITEM_ID */

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->item_id = (ITEM_ID) temp_int;

		/* parse type */

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->type = (unsigned short) temp_int;

		/* parse size */

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->size = (unsigned short) temp_int;

		/* parse DD reference */

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->dd_ref.object_index = (OBJECT_INDEX) temp_int;

	}

	return DDL_SUCCESS;
}


/*********************************************************************
 *
 *	Name: eval_dir_cmd_num_id_tbl
 *	ShortDesc: evaluate the CMD_NUM_ID_TBL
 *
 *	Description:
 *		eval_dir_cmd_num_id_tbl will load the CMD_NUM_ID_TBL structure
 *		by parsing the binary in bin
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		cmd_num_id_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_cmd_num_id_tbl(CMD_NUM_ID_TBL *cmd_num_id_tbl, BININFO *bin)
{

	CMD_NUM_ID_TBL_ELEM	*element =NULL ;	  /* temp pointer for the list */
	CMD_NUM_ID_TBL_ELEM	*end_element =NULL; /* end pointer for the list */
	DDL_UINT        	temp_int;	  /* integer value */
	int					rc;			  /* return code */
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif
	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		cmd_num_id_tbl->count = 0;
		cmd_num_id_tbl->list = NULL;
		return DDL_SUCCESS;
	}

	cmd_num_id_tbl->count = (int) temp_int;

	/* malloc the list */
	cmd_num_id_tbl->list = (CMD_NUM_ID_TBL_ELEM *)new CMD_NUM_ID_TBL_ELEM[((size_t) (temp_int))];

	if (cmd_num_id_tbl->list == NULL) {

		cmd_num_id_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}

	/* load list with zeros */
	(void)memset((char *) cmd_num_id_tbl->list, 0,
			(size_t) temp_int * sizeof(CMD_NUM_ID_TBL_ELEM));

	/*
	 * load the list
	 */

	for (element = cmd_num_id_tbl->list,	end_element = element + temp_int;
		 element < end_element; element++) {

		/* command number */

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->number = (unsigned long) temp_int;

		/* parse ITEM_ID */

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->item_id = (ITEM_ID) temp_int;
	}

	return DDL_SUCCESS;
}


/*********************************************************************
 *
 *	Name: eval_dir_dict_ref_tbl8
 *	ShortDesc: evaluate the DICT_REF_TBL for BFF8
 *
 *	Description:
 *		eval_dir_dict_ref_tbl8 will load the DICT_REF_TBL structure
 *		by parsing the binary in bin
 *		BFF 8 adds the variable name and text
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		dict_ref_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_dict_ref_tbl8(DICT_REF_TBL *dict_ref_tbl, BININFO *bin)
{

	UINT32		*element =NULL;	 			/* temp pointer for the list */
	UINT32		*end_element = NULL;			/* end pointer for the list */
	unsigned long	size =0;			/* temp size */
	unsigned char	*root_ptr = NULL;		/* temp pointer */
	DDL_UINT 	temp_int;	 			/* integer value */
	int			rc;						/* return code */
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif 
#ifdef DDSTEST
	TEST_FAIL(EVAL_DICT_REF_TBL);
#endif /* DDSTEST */

	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		dict_ref_tbl->count = 0;
		dict_ref_tbl->list = NULL;
		dict_ref_tbl->name = NULL;
		dict_ref_tbl->text = NULL;
		return DDL_SUCCESS;
	}

	dict_ref_tbl->count = (int) temp_int;

	/* malloc the lists */
	dict_ref_tbl->list = (UINT32 *)new UINT32[((size_t)(temp_int))];
	dict_ref_tbl->name = (ddpSTRING *)new ddpSTRING[((size_t)(temp_int))];
	dict_ref_tbl->text = (ddpSTRING *)new ddpSTRING[((size_t)(temp_int))];

	if (dict_ref_tbl->list == NULL || dict_ref_tbl->name == NULL || dict_ref_tbl->text == NULL) {

		dict_ref_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}


	/* load list with zeros */
	(void)memset((char *) dict_ref_tbl->list, 0,
			(size_t) temp_int * sizeof(UINT32));

	/*
	 * load the list
	 */
#ifdef _DEBUG
	int i_test=200;	// PAW 02/06/09 debugging only
#endif

	//jad chinese
	//ugh, how to get language
	//how to get dd to load

//LOGIT(CERR_LOG, "before languageDD Dictionary");

	bool bDeviceObject = false;
	FILE* dfp = NULL;
	int ilang = 0;

	ilang = pGlobalDict->GetlanguageCode();

	//LOGIT(CERR_LOG, "languageDD string = %d", ilang);

	if (ilang == 1)
	{
		//check if file exists
		LOGIT(CERR_LOG, "chinese");

		//get dd loaded filename, change extension to .txt
		string sFileName;
		size_t index;
		sFileName = pGlobalDict->pchFileName;
		index = sFileName.find(".fm8");
		sFileName.replace(index, 4, "dct.txt");

		bDeviceObject = false;

		LOGIT(CERR_LOG, "dictionary = %s", pGlobalDict->pchFileName);

		//want to skip the standard device object
		size_t loc;
		loc = sFileName.find("\\0000f9\\0083\\0101");

		if (loc != string::npos)
			bDeviceObject = true;
		else
		{
			fopen_s(&dfp, sFileName.c_str(), "rb");

			if (!dfp)
			{
				LOGIT(CERR_LOG, "Error opening %s", sFileName.c_str());
				//skip using this file
				bDeviceObject = false;
			}
			//else
			//	fclose(fp);
		}
	}

	//if ch and txt file exists, do new load
	//else do normal

	//suggested to do same with russian

	//new load
	//open file

	//LOGIT(CERR_LOG, "Before dictionary list loop");

	size = bin->size;
	root_ptr = (unsigned char *)bin->chunk;

	for (unsigned i = 0; i <  dict_ref_tbl->count; i++)
	{
		char *s;

		// value

		DDL_PARSE_INTEGER(&root_ptr, &size, &temp_int);
		dict_ref_tbl->list[i] = (UINT32) temp_int;

		// name
		// stevev 06jan10 - allow string to be freed.  The makedict call immediatly after this
		//		makes a duplicate of both the strings.

		DDL_PARSE_INTEGER(&root_ptr, &size, &temp_int);
		assert(temp_int < MAXIMUM_USHRT );
		dict_ref_tbl->name[i].len = (UINT32) temp_int;

		dict_ref_tbl->name[i].str = new char[(UINT32)temp_int + 1];
		//strcpy(dict_ref_tbl->name[i].str, (char*)root_ptr);
		std::copy(root_ptr, root_ptr + (UINT32)temp_int, dict_ref_tbl->name[i].str);
		dict_ref_tbl->name[i].str[(UINT32)temp_int] = '\0';
		size -= (unsigned)temp_int;             
		root_ptr += temp_int;        
		dict_ref_tbl->name[i].flags = FREE_STRING;// DONT_FREE_STRING;
		s = dict_ref_tbl->name[i].str;

		// text

		DDL_PARSE_INTEGER(&root_ptr, &size, &temp_int);
		assert(temp_int < MAXIMUM_USHRT );
		dict_ref_tbl->text[i].len = (UINT32) temp_int;
#ifdef xxx_DEBUG
		if (i == i_test)	// PAW 02/06/09 debugging only
			i_test = 250;
#endif
		//jad chinese
		if (dfp == NULL)
		{
			dict_ref_tbl->text[i].str = new char[(unsigned)temp_int + 1];
			//strcpy(dict_ref_tbl->text[i].str, (char*)root_ptr);
			std::copy(root_ptr, root_ptr + (unsigned)temp_int, dict_ref_tbl->text[i].str);
			dict_ref_tbl->text[i].str[(unsigned)temp_int] = '\0';
		}
		else
		{
			dict_ref_tbl->text[i].str = externalString(dict_ref_tbl->text[i].len, (char*)root_ptr, dict_ref_tbl->text[i].len, dict_ref_tbl->list[i], dfp);

			//since not sequencial, need to set file pointer back to beginning
			fseek(dfp, 0, SEEK_SET);
		}

		size -= (unsigned)temp_int;             
		root_ptr += temp_int;        
		dict_ref_tbl->text[i].flags = FREE_STRING;//DONT_FREE_STRING;
		s = dict_ref_tbl->text[i].str;

		//fuji
		//LOGIT(CERR_LOG, " i = %d, %s", i,s);

	}

	//LOGIT(CERR_LOG, "dictionary loop end");

	if ((bDeviceObject) && (dfp != NULL))
		fclose(dfp);
	
	return DDL_SUCCESS;
}

//jad chinese
//    externalString    modifies string for language insertion
//
//    newLen        reference to the len storage (returns strlen of the returned string)
//    rawstring     ptr to the read-in string
//    rawlen        the length of the rawstring
//    stringNumber  The dictionary  or LiteralString number
//    isDict        true at dictionary string, false at literal string
//
//    returns       a new’d character string to use in the application.
//                  It may or may not be modified from the rawstring values.
//                  The length of the returned string in newLen
//
//static
char* externalString(unsigned short newlen, char* rawstring, int rawlen, const ulong stringNumber, FILE* fp)
{
	// strings are null terminated in the table
	char* newstr = getNewLitString(stringNumber, fp);

	//LOGIT(CERR_LOG, "%d  newstr = %s", stringNumber, newstr);


	// newstr will be null if there is no string supplied; it will point to the leading "|" otherwise.
	// it is assumed that the new string will be more accurate than the old string and should replace it.

	char* ending = NULL;	//     will be set zero or ptr to found  “ | ”
	int length;

	if (newstr) // we have one to replace this one
	{
		char *chstrt = strstr(rawstring, "|zh|");
		if (chstrt) // there is a string to replace
		{
			//scan from chstrt till a zero or a “ | ” is found.
			*chstrt = 0; // terminate the leading string
			int length = (chstrt - rawstring) + strlen(newstr);
			length += (ending) ? strlen(ending) : 0;
		}
		else
		{
			length = rawlen + strlen(newstr);
			ending = 0;
		}
	}
	else
	{
		length = rawlen;
		ending = 0;
	}
	char* retStr = new char[length + 1];

	//strcpy(retStr, rawstring);
	std::copy(rawstring, rawstring + strlen(rawstring) + 1, retStr);
	retStr[(unsigned)length] = '\0';

#ifdef MODIFIED_BY_SS
	/*if (newstr)
		strcat(retStr, newstr);
	if (ending)
		strcat(retStr, ending);

	newlen = strlen(retStr);*/
	std::string result;
	result.reserve(length); // Reserve memory for efficiency

	// Append rawstring to result
	result.append(rawstring, rawlen);

	// Append newstr if not null
	if (newstr)
		result.append(newstr);

	// Append ending if not null
	if (ending)
		result.append(ending);

	// Update newlen
	newlen = result.length();

	// Reallocate or allocate memory for retStr
	if (newlen + 1 > length) {
		delete[] retStr;
		retStr = new char[newlen + 1];
		length = newlen + 1;
	}

	// Copy result to retStr
	std::copy(result.begin(), result.end(), retStr);
	retStr[newlen] = '\0'; // Ensure null termination

	

#endif 
	return retStr;
}

//jad don't think this is needed
char* getNewDictString(ulong stringNumber)
{
#ifdef MODIFIED_BY_SS
	
	/*
	* char	chNumber[16];
	sprintf(chNumber, "%d", stringNumber);

	LOGIT(CERR_LOG, "Dict String request = ", chNumber);

	int length = 0;
	char* retStr = new char[length + 1];

	return retStr;*/
	// Convert stringNumber to string using std::to_string
	std::string chNumber = std::to_string(stringNumber);
	
	// Log chNumber
	LOGIT(CERR_LOG, "Dict String request = " , chNumber.c_str());

	// Allocate memory for retStr based on the length of chNumber
	int length = chNumber.length();
	char* retStr = new char[length + 1];

	// Copy chNumber to retStr
	std::copy(chNumber.begin(), chNumber.end(), retStr);
	retStr[length] = '\0'; // Ensure null termination

	return retStr;
#endif
}

char* getNewLitString(ulong stringNumber, FILE* fp)
{
	//read file starting from beginning until desired line
	//read line and return string (whole string including |zh|

	//arg, or should the file be read into ram array in one operation, then use string number as index into array?
	char	chLine[1024];
	
	bool	bStringFound = false;
	string  strLine;
	string	strNumber;
	string	strChars;
	string	strAddedLine;
	int		iPara;
#ifdef MODIFIED_BY_SS
	//char	chNumber[20];
	/*sprintf(chNumber, "%d",stringNumber);*/
	//LOGIT(CERR_LOG, "String request = %s", chNumber);
	std::string chNumber = std::to_string(stringNumber);

	// Log chNumber
	LOGIT(CERR_LOG, "String request = %s", chNumber.c_str());

#endif 

	size_t numloc;
	
	//loop
	while (!bStringFound)
	{
		if (fgets(chLine, sizeof(chLine), fp) != NULL)
			strLine = chLine;
		else
		{
			LOGIT(CERR_LOG, "fgets error");

			//char* retStr = new char[1];
			//retStr[0] = 0;	//set to null
			//return retStr;
			return NULL;
		}

		if (stringNumber == 0)
		{
			string strZero = strLine.substr(3,strLine.length() - 3);
			strLine = strZero;
		}

		//want to only search in the string number field
		iPara = strLine.find('\'');
		strNumber = strLine.substr(0, iPara);
//		strNumber = strLine.substr(0,6);

		LOGIT(CERR_LOG, "iPara, strNumber = %d, %s", iPara, strNumber.c_str());


		numloc = strNumber.find(chNumber);

		if (numloc != string::npos)
			bStringFound = true;
	}

	LOGIT(CERR_LOG, "String number Found");

	//start with |zh| and go to '.
	//determine if last chars are '.crlf
	bool bStringEnd = false;
	size_t loc;

	LOGIT(CERR_LOG, "line = %s", strLine.c_str());
	
	while(!bStringEnd)
	{
		//loc = strLine.find("'ã€‚\r\n");
		loc = strLine.find("'。");

		if (loc != string::npos)
		{
			LOGIT(CERR_LOG, "end found");
			bStringEnd = true;
		}	
		else
		{
			// need to read another line and add
			LOGIT(CERR_LOG, "end not found");
			fgets(chLine, sizeof(chLine), fp);
			strAddedLine = chLine;
			strLine += strAddedLine;
		}
	}

	int length = strLine.length();
	//strChars = strLine.substr(7,length-6-7);
	iPara++;
	strChars = strLine.substr(iPara, length - 6 - iPara);
#ifdef MODIFIED_BY_SS
	char* retStr = new char[length + 1];
	//sprintf(retStr,"%s",strChars.c_str());
	std::copy(strChars.begin(), strChars.end(), retStr);
#endif
	return retStr;
}

/*********************************************************************
 *
 *	Name: eval_dir_dict_ref_tbl
 *	ShortDesc: evaluate the DICT_REF_TBL
 *
 *	Description:
 *		eval_dir_dict_ref_tbl will load the DICT_REF_TBL structure
 *		by parsing the binary in bin
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		dict_ref_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_dict_ref_tbl(DICT_REF_TBL *dict_ref_tbl, BININFO *bin)
{

	UINT32		*element =NULL;	 			/* temp pointer for the list */
	UINT32		*end_element = NULL;			/* end pointer for the list */
	DDL_UINT 	temp_int;	 			/* integer value */
	int			rc;						/* return code */
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif

#ifdef DDSTEST
	TEST_FAIL(EVAL_DICT_REF_TBL);
#endif /* DDSTEST */

	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		dict_ref_tbl->count = 0;
		dict_ref_tbl->list = NULL;
		return DDL_SUCCESS;
	}

	dict_ref_tbl->count = (int) temp_int;

	/* malloc the list */
	dict_ref_tbl->list = (UINT32 *)new UINT32[((size_t)(temp_int))];

	if (dict_ref_tbl->list == NULL) {

		dict_ref_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}

	/* load list with zeros */
	(void)memset((char *) dict_ref_tbl->list, 0,
			(size_t) temp_int * sizeof(UINT32));

	/*
	 * load the list
	 */

	for (element = dict_ref_tbl->list, end_element = element + temp_int;
		 element < end_element; element++) {

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		*element = (UINT32) temp_int;
	}


	return DDL_SUCCESS;
}


/*********************************************************************
 *
 *	Name: eval_dir_command_tbl
 *	ShortDesc: evaluate the COMMAND_TBL
 *
 *	Description:
 *		eval_dir_command_tbl will load the COMMAND_TBL structure
 *		by parsing the binary in bin
 *
 *	Inputs:
 *		bin: a pointer to the binary to parse
 *
 *	Outputs:
 *		command_tbl: a pointer to the loaded table
 *
 *	Returns: DDL_SUCCESS, DDL_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
eval_dir_command_tbl(COMMAND_TBL *command_tbl,BININFO *bin)
{

	COMMAND_TBL_ELEM	*element = NULL;			/* temp pointer for the list of commands*/
	COMMAND_TBL_ELEM	*end_element =NULL;		/* end pointer for the list of commands */
	COMMAND_INDEX		*index =NULL;				/* temp pointer for the list of indexes */
	COMMAND_INDEX		*end_index =NULL;			/* end pointer for the list of indexes */
	DDL_UINT 			temp_int; 		    /* integer value */
	int					rc;				    /* return code */

#ifdef MODIFIED_BY_SS
	// ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif 
	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		command_tbl->count = 0;
		command_tbl->list = NULL;
		return DDL_SUCCESS;
	}

	command_tbl->count = (int) temp_int;

	/* malloc the list */
	command_tbl->list =
		(COMMAND_TBL_ELEM *)new COMMAND_TBL_ELEM[((size_t)(temp_int))];

	if (command_tbl->list == NULL) {

		command_tbl->count = 0;
		return DDL_MEMORY_ERROR;
	}

	/* load list with zeros */
	(void)memset((char *) command_tbl->list, 0,
			(size_t) temp_int * sizeof(COMMAND_TBL_ELEM));

	/*
	 * load the list
	 */

	for (element = command_tbl->list, end_element = element + temp_int;
		 element < end_element; element++) {

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->subindex = (unsigned short) temp_int;

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->number = (unsigned long) temp_int;

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->transaction = (unsigned long) temp_int;

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->weight = (unsigned short) temp_int;

		DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
		element->count = (int) temp_int;

		/*
		 * if there are index elements, parse them
		 */

		if (temp_int) {

				/* malloc the list */
			element->index_list =
				(COMMAND_INDEX *)new COMMAND_INDEX[((size_t)(temp_int))];

			if (element->index_list == NULL) {

				element->count = 0;
				return DDL_MEMORY_ERROR;
			}

			/*
			 * load the list of indexes
			 */

			for (index = element->index_list, end_index = index + temp_int;
					 index < end_index; index++) {

				DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
				index->id= (ITEM_ID) temp_int;

				DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
				index->value = (unsigned long) temp_int;

			}
		}
		else {

			element->index_list = NULL;
		}
	}

	return DDL_SUCCESS;
}

/*********************************************************************
 *
 *	Name: dir_mask_man()
 *	ShortDesc: dir_mask_man() checks the state of the masks and calls eval() if necessary
 *
 *	Description:
 *		dir_mask_man() handles all mask switch cases for the directory structures
 *
 *	Inputs:
 *		attr_mask:		the mask which corresponds to the attribute being evaluated
 *		bin_exists:		the mask which indicates which binaries are available
 *		bin_hooked:		the mask which indicates which binaries are hooked to the
 *						bin structure
 *		attr_avail:		
 *		(*eval):   		the eval function to call if the attribute needs evaluating
 *		bin:			the binary to evaluate
 *
 *	Outputs:
 *		masks:			masks are modified according to the action taken by dir_mask_man()
 *		attribute:		attribute is loaded if the eval() was called
 *
 *	Returns:
 *		DDL_SUCCESS, DDL_NULL_POINTER, DDL_DEFAULT_ATTRIBUTE, returns from (*eval)()
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

static int
dir_mask_man(unsigned long attr_mask, UINT32 bin_exists, UINT32 bin_hooked,
	unsigned long *attr_avail, EVALDIR_FN eval, void *attribute, BININFO *bin)
{

	int             rc;	/* return code */

#ifdef DDSTEST
	TEST_FAIL(MASK_MAN_DIR);
#endif /* DDSTEST */

	/*
	 * No binary exists
	 */

	if (!(attr_mask & bin_exists)) {

		/*
		 * This is a DDOD error, by definition all directory tables
         * must have binary available
		 */

		rc = DDL_BINARY_REQUIRED;
	}

	/*
	 * No binary hooked
	 */

	else if (!(attr_mask & bin_hooked)) {

		/*
		 * If value is already available
		 */

		if (attr_mask & *attr_avail) {

			rc = DDL_SUCCESS;
		}

		/*
		 * error, binary should be hooked up
		 */

		else {

			rc = DDL_BINARY_REQUIRED;
		}
	}

	else {

		/*
		 * check masks for evaluating
		 */

		if (!(attr_mask & *attr_avail)) {

			rc = (*eval) (attribute, bin);

			if (rc == DDL_SUCCESS) {

				*attr_avail |= attr_mask;
			}
		}

		/*
		 * evaluation is not necessary
		 */

		else {

			rc = DDL_SUCCESS;
		}
	}

	return rc;
}



/*********************************************************************
 *
 *	Name: eval_dir_device_tables
 *	ShortDesc: evaluate (ie. create) the device tables
 *
 *	Description:
 *		eval_dir_device_tables will load the desired device tables into the 
 *		FLAT_DEVICE_DIR structure. The user must specify which device tables
 *		are desired by setting the appropriate bits in the "mask" parameter.
 *
 *	Inputs:
 *		mask:		bit mask specifying the desired device tables.
 *
 *	Outputs:
 *		device_dir:	pointer to a FLAT_DEVICE_DIR structure. Will contain
 *					the desired device tables.
 *      device_bin: pointer to a BIN_DEVICE_DIR structure. Stores the 
 *					binary device information created by the tokenizer.
 *
 *	Returns:
 *		DDS_SUCCESS,
 *		return codes from other DDS functions.
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

int
eval_dir_device_tables(FLAT_DEVICE_DIR *device_dir, BIN_DEVICE_DIR *device_bin,
	unsigned long mask)
{

	int  rc = DDS_SUCCESS;	/* return code */
	
	if (mask & BLK_TBL_MASK) {

/*		rc = dir_mask_man((unsigned long) BLK_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (int (*) ()) eval_dir_blk_tbl,
			(void *) &device_dir->blk_tbl, &device_bin->blk_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}*/
	}
	if (mask & DICT_REF_TBL_MASK) {

		rc = dir_mask_man((unsigned long) DICT_REF_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_dict_ref_tbl,
			(void *) &device_dir->dict_ref_tbl, &device_bin->dict_ref_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
/*	if (mask & DOMAIN_TBL_MASK) {

		rc = dir_mask_man((unsigned long) DOMAIN_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_domain_tbl,
			(void *) &device_dir->domain_tbl, &device_bin->domain_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
*/
	if (mask & ITEM_TBL_MASK) {

		rc = dir_mask_man((unsigned long) ITEM_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_item_tbl,
			(void *) &device_dir->item_tbl, &device_bin->item_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
/*	if (mask & PROG_TBL_MASK) {

		rc = dir_mask_man((unsigned long) PROG_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_prog_tbl,
			(void *) &device_dir->prog_tbl, &device_bin->prog_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
*/
	if (mask & STRING_TBL_MASK) {

		rc = dir_mask_man((unsigned long) STRING_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_string_tbl,
			(void *) &device_dir->string_tbl, &device_bin->string_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
			
		if (pLitStringTable) // stevev 24apr08 - for fm <8..stevev 22apr13 moved to v5 from v6
		// copy the parsed string table into the global lit string table
		pLitStringTable->makelit(&(device_dir->string_tbl), true);
	}

/*	if (mask & LOCAL_VAR_TBL_MASK) {

		rc = dir_mask_man((unsigned long) LOCAL_VAR_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_local_var_tbl,
			(void *) &device_dir->local_var_tbl, &device_bin->local_var_tbl);
		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
*/
	if (mask & CMD_NUM_ID_TBL_MASK) {

		rc = dir_mask_man((unsigned long) CMD_NUM_ID_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_cmd_num_id_tbl,
			(void *) &device_dir->cmd_num_id_tbl, &device_bin->cmd_num_id_tbl);
		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}

	
	return rc;
}


/*********************************************************************
 *
 *	Name: eval_clean_device_dir
 *	ShortDesc: Free the device tables structure
 *
 *	Description:
 *		eval_clean_device_dir will check the attr_avail flags
 *      in the FLAT_DEVICE_DIR structure to see which device tables
 *		exist and free them. Everything in the structure is then
 *		set to zero.
 *
 *	Inputs:
 *		device_dir: pointer to the FLAT_DEVICE_DIR to be cleaned.
 *
 *	Outputs:
 *		device_dir: pointer to the empty FLAT_DEVICE_DIR structure.
 *
 *	Returns:
 *		void
 *
 *	Author:
 *		Chris Gustafson
 *
 *********************************************************************/
#ifndef linux
#define DELTYPE (void*)
#else // is liux
#define DELTYPE
#endif

void
eval_clean_device_dir(FLAT_DEVICE_DIR *device_dir)
{
	ulong           temp_attr_avail;

	if (device_dir == NULL) {
		return;
	}

	
	temp_attr_avail = device_dir->attr_avail;

	/*
	 * Free attribute structures
	 */

	if ((temp_attr_avail & BLK_TBL_MASK) && 
		(device_dir->blk_tbl.list)) {
		delete(DELTYPE device_dir->blk_tbl.list);
	}

	if ((temp_attr_avail & DICT_REF_TBL_MASK) && 
		(device_dir->dict_ref_tbl.list)) {
		delete(DELTYPE device_dir->dict_ref_tbl.list);
	}

	if ((temp_attr_avail & DOMAIN_TBL_MASK) && 
		(device_dir->domain_tbl.list)) {
		delete(DELTYPE device_dir->domain_tbl.list);
	}

	if ((temp_attr_avail & ITEM_TBL_MASK) && 
		(device_dir->item_tbl.list)) {
		delete(DELTYPE device_dir->item_tbl.list);
	}

	if ((temp_attr_avail & PROG_TBL_MASK) && 
		(device_dir->prog_tbl.list)) {
		delete(DELTYPE device_dir->prog_tbl.list);
	}

	if ((temp_attr_avail & STRING_TBL_MASK) && 
		(device_dir->string_tbl.list)) {

		delete(DELTYPE device_dir->string_tbl.root);
		delete(DELTYPE device_dir->string_tbl.list);
	}

	if ((temp_attr_avail & LOCAL_VAR_TBL_MASK) && 
		(device_dir->local_var_tbl.list)) {

		delete(DELTYPE device_dir->local_var_tbl.list);
	}

	if ((temp_attr_avail & CMD_NUM_ID_TBL_MASK) && 
		(device_dir->cmd_num_id_tbl.list)) {

		delete(DELTYPE device_dir->cmd_num_id_tbl.list);
	}

	(void)memset((char *) device_dir, 0, sizeof(FLAT_DEVICE_DIR));

	
}


/*Vibhor 020904: Start of Code*/
/* Adding a new function to evaluate the image table
   Adding a new function to evaluate the HART 6 Device directory*/

static int
eval_dir_image_tbl(IMAGE_TBL *image_table, BININFO *bin)
{
 	IMAGE_TBL_ELEM			*element = NULL;	 	/* temp pointer for the list */
	IMAGE_TBL_ELEM			*end_element =NULL;	/* end pointer for the list */
	IMG_ITEM				*item = NULL;		/*temp ptr to img item list*/
	IMG_ITEM				*end_item = NULL;	/*end ptr to img item list*/
	DDL_UINT    				temp_int = 0;		/* integer value */
	unsigned long				size =0;			/* temp size */
	unsigned char				*root_ptr = NULL;		/* temp pointer */
	int							rc = 0;				/* return code */

	unsigned long			img_list_size = 0; /*temp size of actual img list*/
	unsigned char			*base_ptr = NULL;  /*base ptr of img list chunk*/

#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(bin && bin->chunk && bin->size);
	assert(bin && bin->chunk && bin->size);
#endif

	/* parse count */
	DDL_PARSE_INTEGER(&bin->chunk, &bin->size, &temp_int);
	assert(temp_int < MAXIMUM_INT );

	/*
	 * if count is zero
	 */

	if(!temp_int) {

		image_table->count = 0;
		image_table->list = NULL;
		return DDL_SUCCESS;
	}
	//else store the count

	image_table->count = (int)temp_int;

	//Allocate the list

	image_table->list = (IMAGE_TBL_ELEM *)new IMAGE_TBL_ELEM[((size_t)(temp_int))];

	if (image_table->list == NULL) {

		image_table->count = 0;
		return DDL_MEMORY_ERROR;
	}

	size = bin->size;
	root_ptr = (unsigned char *)bin->chunk;

	/*Loop through the chunks and form the table*/

	for(element = image_table->list, end_element = image_table->list + temp_int;
		element < end_element && size > 0; element++)
	{
		//for each element , parse the number of languages
		DDL_PARSE_INTEGER(&root_ptr, &size, &temp_int);
		assert(temp_int < MAXIMUM_INT );

		//TODO : Handle a graceful exit here as this may result in leaks.
		/*this number must be a non zero*/
		if(!temp_int) {
			return DDL_ENCODING_ERROR;
		}
		//For now assume: Its a non-zer0 number
		element->num_langs =  (int)temp_int;	
		
		//Allocate the number or IMAGE_ITEMs on the list for each lang.
		
		element->img_list = (IMG_ITEM*) new IMG_ITEM[((size_t)(temp_int))];
		if (image_table->list == NULL) {

			image_table->count = 0;
			return DDL_MEMORY_ERROR;
		}

		// Parse the image list.
		IMG_ITEM* pImgItm;
		// for each language
		for ( unsigned y = 0; y < element->num_langs && size > 0; y++)
		{
			pImgItm = &(element->img_list[y]);
			// get byte string (6)  (define is same as used in tokenizer)
#ifdef MODIFIED_BY_SS
			//strncpy((char*)(pImgItm->lang_code), (char*)root_ptr, CNTRYCDSTRLEN);
			size_t length = std::strlen((char*)root_ptr);
			if (length >= CNTRYCDSTRLEN) {
				length = CNTRYCDSTRLEN - 1; // -1 to ensure null-termination
			}

			// Copy the data using std::copy
			std::copy(root_ptr, root_ptr + length, pImgItm->lang_code);

			// Null-terminate the string manually, assuming there's space left for the null character
			pImgItm->lang_code[length] = '\0';

#endif 
			size     -= CNTRYCDSTRLEN;        /* decrement size */
			root_ptr += CNTRYCDSTRLEN;        /* increment the root pointer */

			// get datapart segment
			//		get 	offset	Long_Offset,
			
			DDL_PARSE_INTEGER(&root_ptr, &size, &temp_int);

			pImgItm->img_file.offset =  (long_offset)temp_int;
			
			//		get		size	Unsigned32				
			DDL_PARSE_INTEGER(&root_ptr, &size, &temp_int);

			pImgItm->img_file.uSize  =  (unsigned)temp_int;
			// stevev 11dec08 - missing images are getting through
			// stevev 27jul12 - trust the tokenizer to filter images that are too big
			/***
			if (pImgItm->img_file.uSize > LARGEST_IMAGE_SIZE)
			{// we have a bogus entry
				pImgItm->lang_code[0] = '\0';
				pImgItm->img_file.offset = 0;
				pImgItm->img_file.uSize  = 0;
				pImgItm->p2Graphik       = NULL;
			}
			***/
		}// next language

	}/*Endfor -  next image */

	return DDL_SUCCESS;

}/*End eval_dir_image_tbl*/
				






int
eval_dir_device_tables_6(FLAT_DEVICE_DIR_6 *device_dir, BIN_DEVICE_DIR_6 *device_bin,
	unsigned long mask)
{

	int  rc = DDS_SUCCESS;	/* return code */

 
	if (mask & BLK_TBL_MASK) {

/*		rc = dir_mask_man((unsigned long) BLK_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (int (*) ()) eval_dir_blk_tbl,
			(void *) &device_dir->blk_tbl, &device_bin->blk_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}*/
	}
	if (mask & DICT_REF_TBL_MASK) {

		rc = dir_mask_man((unsigned long) DICT_REF_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_dict_ref_tbl,
			(void *) &device_dir->dict_ref_tbl, &device_bin->dict_ref_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
		
		// copy the parsed dictionary into the global dictionary
		pGlobalDict->makedict(&(device_dir->dict_ref_tbl));
	}

/*	if (mask & DOMAIN_TBL_MASK) {

		rc = dir_mask_man((unsigned long) DOMAIN_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_domain_tbl,
			(void *) &device_dir->domain_tbl, &device_bin->domain_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
*/
	if (mask & ITEM_TBL_MASK) {

		rc = dir_mask_man((unsigned long) ITEM_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_item_tbl,
			(void *) &device_dir->item_tbl, &device_bin->item_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}

/*	if (mask & PROG_TBL_MASK) {

		rc = dir_mask_man((unsigned long) PROG_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_prog_tbl,
			(void *) &device_dir->prog_tbl, &device_bin->prog_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
*/
	if (mask & STRING_TBL_MASK) {

		rc = dir_mask_man((unsigned long) STRING_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_string_tbl,
			(void *) &device_dir->string_tbl, &device_bin->string_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	
		if (pLitStringTable) // stevev 24apr08 - for fm <8
		// copy the parsed string table into the global lit string table
		pLitStringTable->makelit(&(device_dir->string_tbl), true);
	}

/*	if (mask & LOCAL_VAR_TBL_MASK) {

		rc = dir_mask_man((unsigned long) LOCAL_VAR_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_local_var_tbl,
			(void *) &device_dir->local_var_tbl, &device_bin->local_var_tbl);
		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
*/
	if (mask & CMD_NUM_ID_TBL_MASK) {

		rc = dir_mask_man((unsigned long) CMD_NUM_ID_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_cmd_num_id_tbl,
			(void *) &device_dir->cmd_num_id_tbl, &device_bin->cmd_num_id_tbl);
		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}

	if (mask & IMAGE_TBL_MASK) {

		rc = dir_mask_man((unsigned long) IMAGE_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_image_tbl,
			(void *) &device_dir->image_tbl, &device_bin->image_tbl);
		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}

	
	return rc;
}

/*Vibhor 020904: End of Code*/


// timj added 9oct07
int
eval_dir_device_tables_8(FLAT_DEVICE_DIR_6 *device_dir, BIN_DEVICE_DIR_6 *device_bin,
	unsigned long mask)
{

	int  rc = DDS_SUCCESS;	/* return code */

 
	if (mask & BLK_TBL_MASK) {

/*		rc = dir_mask_man((unsigned long) BLK_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (int (*) ()) eval_dir_blk_tbl,
			(void *) &device_dir->blk_tbl, &device_bin->blk_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}*/
	}
	if (mask & DICT_REF_TBL_MASK) {

		rc = dir_mask_man((unsigned long) DICT_REF_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_dict_ref_tbl8,
			(void *) &device_dir->dict_ref_tbl, &device_bin->dict_ref_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}

		// copy the parsed dictionary into the global dictionary
		pGlobalDict->makedict(&(device_dir->dict_ref_tbl));

	}
	if (mask & DOMAIN_TBL_MASK) {

		rc = dir_mask_man((unsigned long) DOMAIN_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_domain_tbl,
			(void *) &device_dir->domain_tbl, &device_bin->domain_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
	if (mask & ITEM_TBL_MASK) {

		rc = dir_mask_man((unsigned long) ITEM_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_item_tbl,
			(void *) &device_dir->item_tbl, &device_bin->item_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
	if (mask & PROG_TBL_MASK) {

		rc = dir_mask_man((unsigned long) PROG_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_prog_tbl,
			(void *) &device_dir->prog_tbl, &device_bin->prog_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}
	if (mask & STRING_TBL_MASK) {

		rc = dir_mask_man((unsigned long) STRING_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_string_tbl,
			(void *) &device_dir->string_tbl, &device_bin->string_tbl);

		if (rc != DDL_SUCCESS) {

			return rc;
		}
		
		if (pLitStringTable) // stevev 24apr08 - for fm <8
		// copy the parsed string table into the global lit string table
		pLitStringTable->makelit(&(device_dir->string_tbl), false);
	}

	if (mask & LOCAL_VAR_TBL_MASK) {

		rc = dir_mask_man((unsigned long) LOCAL_VAR_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_local_var_tbl,
			(void *) &device_dir->local_var_tbl, &device_bin->local_var_tbl);
		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}

	if (mask & CMD_NUM_ID_TBL_MASK) {

		rc = dir_mask_man((unsigned long) CMD_NUM_ID_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_cmd_num_id_tbl,
			(void *) &device_dir->cmd_num_id_tbl, &device_bin->cmd_num_id_tbl);
		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}

	if (mask & IMAGE_TBL_MASK) {

		rc = dir_mask_man((unsigned long) IMAGE_TBL_MASK,
			device_bin->bin_exists, device_bin->bin_hooked,
			&device_dir->attr_avail, (EVALDIR_FN) eval_dir_image_tbl,
			(void *) &device_dir->image_tbl, &device_bin->image_tbl);
		if (rc != DDL_SUCCESS) {

			return rc;
		}
	}

	
	return rc;
}

// end of timj addition
