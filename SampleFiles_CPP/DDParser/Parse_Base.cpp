/*****************************************************************************************

  This file contains the implementation of the methods  for  Core Parsing methods which 
  are called by "attr_parse_......" methods, to parse different types of the attributes
  
******************************************************************************************/

/* from .....Evl_Base.c*/
#pragma warning (disable : 4786)

//sjv06feb06 #include <windows.h>
#include "pch.h"
#include "ddbGeneral.h"

#include <assert.h>

//jad vs2017 ios	pw - apparently apple doesn't malloc...it works
#ifndef APPLE
#include <malloc.h>
#endif

#include <memory.h>		/* K&R only */
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>

#include "std.h"

#include "evl_loc.h"
#include "panic.h"

#include "DDlItems.h"

#include "Parse_Base.h"

#include "Dictionary.h"
#include "LitStringTable.h"
//#include "DDlConditional.h"
#include "logging.h"

//extern char langCode[4];


//extern int ddi_get_string_translation (char *, char *, char *, int);

extern CDictionary *pGlobalDict; /*The Global Dictionary object*/
extern LitStringTable *pLitStringTable; /*The Global literal string table object*/

#define USE_SIGNED_INTEGER_CONSTANTS	1 /* legacy was incorrect and all were Unsigned */

int	ddl_parse_integer_func(unsigned char **chunkp, unsigned long *size, UINT64 *value)
{
	
	//ADDED By Deepak Initialize all vars
	unsigned long        cnt=0;	/* temp ptr to size of binary */
	UINT64               val=0;	/* temp storage for parsed integer */
	unsigned char		*chunk=NULL;	/* temp ptr to binary */
	unsigned char           c=0;		/* temp ptr to binary */
	int				   more_indicator=0;	/* need to parse another byte */
#ifdef MODIFIED_BY_SS	
	//ASSERT_DBG(chunkp && *chunkp && size);
	assert(chunkp && *chunkp && size);
#endif	
	/*
	* Read each character, building the ulong until the high order bit is
	* not set
	*/
	
	val = 0;
	chunk = *chunkp;
	cnt = *size;
	
	do {
		if (cnt == 0) {
			return DDL_INSUFFICIENT_OCTETS;
		}
		
		c = *chunk++;
		more_indicator = c & 0x80;
		c &= 0x7f;
		
		if (val > ( DDL_UINT64_MAX >> 7)) {
			return DDL_LARGE_VALUE;
		}
		
		val <<= 7;
		if (val >   DDL_UINT64_MAX - c) {
			return DDL_LARGE_VALUE;
		}
		
		val |= c;
		--cnt;
	} while (more_indicator);
	
	/*
	* Update the pointer and size, and return the value
	*/
	
	*size = cnt;
	*chunkp = chunk;
	
	if (value) {
		*value = val;
	}
	
	return DDL_SUCCESS;
	
}/*End ddl_parse_integer_func*/




int ddl_parse_tag_func(unsigned char **chunkp, unsigned long *size, unsigned long *tagp,
					   unsigned long *lenp)
{
	//ADDED By Deepak Initialize all vars
	int             lenflag=0;/* indicates implicit/explicit binary length */
	unsigned char	*chunk=NULL;	/* temp ptr to the binary chunk */
	unsigned long   cnt=0;	/* temp ptr to the size of the binary chunk */
	unsigned char           c=0;		/* current value of the char pointed at by chunk */
	int             rc=0;		/* return code */
	unsigned long        tag=0;	/* temp storage of parsed tag */
	unsigned long        length=0;	/* temp storage of length of binary assoc with tag */
	unsigned __int64 LL;
	
	
#ifdef MODIFIED_BY_SS	
	//ASSERT_DBG(chunkp && *chunkp && size);
	assert(chunkp && *chunkp && size);
#endif
	chunk = *chunkp;
	cnt = *size;
	if (cnt == 0) {
		return DDL_INSUFFICIENT_OCTETS;
	}
	
	/*
	* Read the first character, and determine if there is an explicit
	* length specified (bit 7 == 1)
	*/
	
	c = *chunk++;
	cnt--;
	lenflag = c & 0x80;
	tag = c & 0x7f;
	
	/*
	* If the tag from the first character is <= 126, we are through. If
	* the tag is == 127, we need to build the tag ID from the following
	* characters.
	*/
	
	if (tag == 127) {
		DDL_PARSE_INTEGER(&chunk, &cnt, &LL); tag = (ulong) LL;
		tag += 126;
	}
	
	if (tagp) {
		*tagp = tag;
	}
	
	/*
	* If there is an explicit length, get it.
	*/
	
	if (!lenflag) {
		length = 0;
	}
	else {
		DDL_PARSE_INTEGER(&chunk, &cnt, &LL); length = (ulong) LL;
	}
	
	if (length > cnt) {
		return DDL_ENCODING_ERROR;
	}
	
	if (lenp) {
		*lenp = length;
	}
	*size = cnt;
	*chunkp = chunk;
	
	return DDL_SUCCESS;
	
	
}/* End ddl_parse_tag_func */ 



int ddl_parse_float(unsigned char **chunkp, unsigned long *size, float *value)
{
	//ADDED By Deepak initialize all vars
	float           temp=0;	/* temporary storage */
	
	if (*size < (unsigned long) sizeof(float)) {
		return DDL_INSUFFICIENT_OCTETS;
	}

/*LOGIT(CLOG_LOG, "  ddl_parse_float %02x %02x %02x %02x %02x %02x %02x %02x.(size=%d)", 
	(*chunkp)[0], (*chunkp)[1], (*chunkp)[2], (*chunkp)[3], (*chunkp)[4], (*chunkp)[5], (*chunkp)[6], (*chunkp)[7], sizeof(float));// pw uneditable  	
a direct cast of this sequence gives -4896732.0 or such  ON INTEL little endian
a cast of this reversed gives 0.1                        ON INTEL little endian
**/
	/*
	* Move the four bytes into the float, taking care to handle the
	* different byte ordering.
	*/
	/* encoded file records it as big endian (non Intel) */
//jad vs2017 ios	pw - this works, we can clean it up later
//#if (defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN))

	if (isBIGendian)
	{
	/* Use BIG_ENDIAN byte order! */
		(void)memcpy((char *)&temp, (char *)*chunkp, sizeof(float));
	}
	else
	{
	//jad vs2017	pw - this works, we can clean it up later
	//#if (!defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN))

	/* Use LITTLE_ENDIAN byte order! */
	{
		size_t          i;
		unsigned char  *bytep;
		unsigned char  *chunk;	/* local pointer to binary */

		chunk = *chunkp;
			bytep = ((unsigned char *)&temp) + (sizeof(float) - 1);
		for (i = 0; i < sizeof(float); ++i)
			*(bytep--) = *(chunk++);
	}
	//jad vs2017	pw - this works, we can clean it up later
	//#else
	/* Must define one or the other */

	//jad vs2017
	//#error Must define BIG_ENDIAN or LITTLE_ENDIAN, but not both.
	//#endif
	}
	
	*chunkp += sizeof(float);
	*size -= (unsigned long) sizeof(float);
	
	if (value) {
		*value = temp;
	}
	
	return DDL_SUCCESS;
	
	
}/* End ddl_parse_float*/


/* This one might be coupled with the first one*/

int ddl_parse_bitstring(unsigned char **chunkp, unsigned long *size, unsigned long *bitstring)
{
	//ADDED By Deepak initialize all vars
	unsigned char  *chunk=NULL;		/* temp ptr to the binary chunk */
	unsigned long        read_mask=0;	/* mask used to read bits from the binary */
	unsigned long        write_mask=0;	/* mask used to store the bits  */
	unsigned long        value=0;		/* temp storage of parsed bits */
	unsigned long        count=0;		/* local value of binary size */
	int             more_indicator=0;	/* indicators more bits to be read */
	int             last_unused=0;	/* # of bits to ignore in last octet */
	int             bitlimit=0;	/* loop counter */
	unsigned char   c=0;

#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(chunkp && *chunkp && size);
	assert(chunkp && *chunkp && size);
#endif	/*
	* Read the first character
	*/
	
	chunk = *chunkp;
	count = *size;
	value = 0;
	count--;
	c = *chunk++;
	
	/*
	* Special case the situation where there is no bitstring (that is, the
	* bit string is zero).
	*/
	
	if (c == 0x80) {
		*size = count;
		*chunkp = chunk;
		
		if (bitstring) {
			*bitstring = value;
		}
		return DDL_SUCCESS;
	}
	
	/*
	* Pull out the number of bits to ignore in the last octet, and the
	* indicator of whether there are more octets.
	*/
	
	more_indicator = c & 0x10;
	last_unused = (c & 0xE0) >> 5;
	c &= 0xF;
	
	/*
	* Build up the return value.  Note that in each octet, the highest
	* order bit corresponds to the lowest order bit in the returned bit
	* mask.  That is, in the first octet, bit 3 corresponds to bit 0, bit
	* 2 to bit 1, etc.  In the next octet, bit 6 corresponds to bit 4, bit
	* 5 to bit 5, bit 4 to bit 6, ...  In the next octet, bit 6
	* corresponds to bit 11, bit 5 to bit 12, ...
	*/
	
	write_mask = 1;
	read_mask = 0x8;
	bitlimit = 4;
	
	while (more_indicator) {
		
		if (!count) {
			
			return DDL_INSUFFICIENT_OCTETS;
		}
		
		for (; bitlimit; bitlimit--, read_mask >>= 1, write_mask <<= 1) {
			
			if (c & read_mask) {
				
				value |= write_mask;
			}
		}
		
		/*
		* Read the next octet.
		*/
		
		count--;
		c = *chunk++;
		more_indicator = c & 0x80;
		c &= 0x7F;
		read_mask = 0x40;
		bitlimit = 7;
	}
	
	/*
	* In the last octet, some of the bits may be ignored.  The number of
	* bits to ignore was specified in the very first octet, and remembered
	* in "last_unused".
	*/
	
	bitlimit -= last_unused;
	
	if (bitlimit <= 0) {
		
		return DDL_ENCODING_ERROR;
	}
	
	for (; bitlimit; bitlimit--, read_mask >>= 1, write_mask <<= 1) {
		
		if (!write_mask) {
			
			return DDL_LARGE_VALUE;
		}
		if (c & read_mask) {
			
			value |= write_mask;
		}
	}
	
	*size = count;
	*chunkp = chunk;
	
	if (bitstring) {
		
		*bitstring = value;
	}
	
	return DDL_SUCCESS;
	
	
}/* End ddl_parse_bitstring */


int ddl_parse_definition(unsigned char *chunk, unsigned long size, DEFINITION *def)
{
#ifdef MODIFIED_BY_SS	
	//ASSERT_RET(chunk && size, DDL_INVALID_PARAM);
	assert(chunk && size, DDL_INVALID_PARAM);
#endif	
	def->size = (unsigned long) size;

	def->data = new char[size];

	if (!def->data) {
		return DDL_MEMORY_ERROR;
	}
	unsigned length = strlen((char *)chunk) +1;
	if (def->size < length)
	{
		return FAILURE;
	}
#ifdef MODIFIED_BY_SS
	
	size_t arraySize = sizeof(chunk) / sizeof(chunk[0]); // Calculate the size of the array
	std::string str(chunk, chunk + arraySize);
	/*size_t length = str.length();*/
	// (void)strcpy((char *) def->data, (char *) chunk);
	std::copy(str.begin(),str.end(), static_cast<char*>(def->data));
#endif	
	return DDL_SUCCESS;
	
	
}/* End ddl_parse_definition*/



int ddl_parse_string(unsigned char **chunkp, unsigned long *size, ddpSTRING *string)
{
	//ADDED By Deepak initialize all  vars
	int             rc=0;				/* return code */
	DEV_STRING_INFO dev_str_info;	/* holds device specific string info */
	unsigned long        tag=0;			/* temporary tag */
	unsigned long        temp=0;		/* temporary storage for casting conversion */
	unsigned long        tag_len=0;	/* length of binary assoc. w/ parsed tag */
	unsigned long   enum_val=0;		/* used for enumeration strings */
	unsigned __int64 LL;
	
	/*Vibhor 201003: Adding following variables for string translation */

	char* pchTranslatedString = NULL;

#ifdef MODIFIED_BY_SS	
	//ASSERT_DBG(chunkp && *chunkp && size);
	assert(chunkp && *chunkp && size);
#endif	
	/*
	* initialize string struct
	*/
	
	if (string != NULL) {
		string->flags = DONT_FREE_STRING;
		string->len = 0;
		string->str = NULL;
		string->strType = DICTIONARY_STRING_TAG; /*By default we will put a string to be of 
												  type DICTIONARY ddpSTRING*/
	}
	

	/*
	* Parse the tag to find out what kind of string it is.
	*/
	
	DDL_PARSE_TAG(chunkp, size, &tag, &tag_len);
	
	switch (tag) 
	{		
	case DEV_SPEC_STRING_TAG:
		
	/*
	* device specific string (ie. string number). Parse the file
	* information and string number.
		*/
		
		if (string) 
		{			
			DDL_PARSE_INTEGER(chunkp, size, &LL);
			dev_str_info.mfg = (unsigned long) LL;
			
			DDL_PARSE_INTEGER(chunkp, size, &LL);
			dev_str_info.dev_type = (unsigned short) LL;
			
			DDL_PARSE_INTEGER(chunkp, size, &LL);
			dev_str_info.rev = (unsigned char) LL;
			
			DDL_PARSE_INTEGER(chunkp, size, &LL);
			dev_str_info.ddrev = (unsigned char) LL;
			
			DDL_PARSE_INTEGER(chunkp, size, &LL);
			dev_str_info.id = (unsigned int) LL;
			
			unsigned long 
			ddKey = (dev_str_info.mfg & 0xff)      << 24;
			ddKey+= (dev_str_info.dev_type & 0xff) << 16;
			ddKey+= (dev_str_info.rev & 0xff)      << 8;
			ddKey+= (dev_str_info.ddrev & 0xff);

			rc = app_func_get_dev_spec_string(&dev_str_info,
				string);
			
			/*
			* If a string was not found, get the default error string.
			*/
			
			if (rc != DDL_SUCCESS) 
			{
				LOGIT(CERR_LOG,L"Device specific string 0x%04x for device DDkey 0x%08x was not aquired.\n",temp,ddKey);
				rc = pGlobalDict->get_dictionary_string(DEFAULT_DEV_SPEC_STRING,
					string);
				if (rc != DDL_SUCCESS) 
				{
					LOGIT(CERR_LOG,L"Default device specific string was not aquired.\n");
					return rc;
				}
			}
		}
		
		/* I donno when do we need to parse the string if we don't want to store it*/
		else 
		{
			DDL_PARSE_INTEGER(chunkp, size, (UINT64 *) NULL_PTR);
			DDL_PARSE_INTEGER(chunkp, size, (UINT64 *) NULL_PTR);
			DDL_PARSE_INTEGER(chunkp, size, (UINT64 *) NULL_PTR);
			DDL_PARSE_INTEGER(chunkp, size, (UINT64 *) NULL_PTR);
			DDL_PARSE_INTEGER(chunkp, size, (UINT64 *) NULL_PTR);
		}

/*	timj 8jan08 - all languages now retained in the string, translation deferred to 
					copy_ddlstring in parserInfc.cpp

		//Translate the string to the desired language as specified by the global langCode
		//Vibhor 020104: If the DD has a Null string ie "" then we will store a string which is
		//nothing but null string ... 
		if(string->len > 0)
		{
			pchTranslatedString = new char[string->len];

			 //This call will translate the string with into the language with which the 
			 //DDlDevdescription Class was initialized
			rc = pGlobalDict->get_string_translation(string->str,pchTranslatedString,string->len);
			if(rc == SUCCESS)
			{//We have a translated string!!
				//string->str is holding a memory from Dictionary, we will allocate new memory to it
				//to store the translated string
				string->str = NULL;
				if(strlen(pchTranslatedString) > 0)
				{
					string->str = new char[strlen(pchTranslatedString)+1];
					strcpy(string->str,pchTranslatedString);
					string->len = strlen(string->str);
					string->flags = FREE_STRING;// stevev 7aug7
				}
				else
				{	
					string->str = new char[2];
					strcpy(string->str,"");
					string->len = 0;
					string->flags = FREE_STRING | ISEMPTYSTRING;// stevev 14nov05 set a flag for empty

				}
			}
			//free the memory allocated to the input & translated string 
			
			delete [] pchTranslatedString; //Vibhor 120504 : changed to delete []
			
			pchTranslatedString = NULL;
		}
		else
 end timj */

		if(string->len == 0)
		{
			string->str = new char[2];
#ifdef MODIFIED_BY_SS			
			//strcpy(string->str,"");
			memset(string->str,0,sizeof(string->str));
			//string->str = "";
#endif
			string->len = 0;
			string->flags = FREE_STRING | ISEMPTYSTRING;// stevev 14nov05 set a flag for empty
		}

		string->strType = tag;
		break;
		
	case VARIABLE_STRING_TAG:
		DDL_PARSE_INTEGER(chunkp, size, &LL);
		string->varId = (ITEM_ID)LL;
		
		string->strType = tag;
		break;
	case VAR_REF_STRING_TAG: /*Undocumented: We have to verify if VAR_REF_STRING_TAG is really encountered*/
		rc = ddl_parse_ref(chunkp,size,&(string->varRef));
		string->strType = tag;
		break;
		
	case ENUMERATION_STRING_TAG:
	case ENUM_REF_STRING_TAG:
		
	/*
	* enumeration/enumeration_reference string. Parse the
	* enumeration ID, and the value within the enumeration.
		*/
		
		if (string) {
			
				if( tag == 	ENUMERATION_STRING_TAG)
				{
				DDL_PARSE_INTEGER(chunkp, size, &LL);
					string->enumStr.enmVar.iD = (ITEM_ID) LL;
				}
				else /*ENUM_REF_STRING_TAG*/
				{
					string->enumStr.enmVar.ref = new ddpREFERENCE;
					rc = ddl_parse_ref(chunkp,size,string->enumStr.enmVar.ref);
				}
				
				string->strType = tag;
			
			DDL_PARSE_INTEGER(chunkp, size, &LL);

			string->enumStr.enumValue = (UINT32) LL;
			
		}
		else {
			if (tag == ENUMERATION_STRING_TAG) {
				DDL_PARSE_INTEGER(chunkp, size, (UINT64 *) NULL_PTR);
			}
			else {
			/*
			rc = ddl_parse_item_id(chunkp, size, (ITEM_ID *) NULL, depinfo,
			var_needed);
			if (rc != DDL_SUCCESS) {
			return rc;
			}
				*/
			}
			DDL_PARSE_INTEGER(chunkp, size, (UINT64 *) NULL_PTR);
		}
		string->strType = tag;
		break;
		
	case DICTIONARY_STRING_TAG:
		
	/*
	* dictionary string. Parse the dictionary string number.
		*/
		
		if (string)
		{			
			DDL_PARSE_INTEGER(chunkp, size, &LL); temp = (ulong) LL;
			// stevev 19dec13 - no label/help available is now blank
			// version 8 dictionary doesn't include blank
			//if ( temp == 0x1900002 || temp == 0x1900003 )
			//{
			//	temp = 0x1260009;
			//}
			if ( temp == 0x1900002 || temp == 0x1900003 )
			{
				string->str = new char[2];
#ifdef MODIFIED_BY_SS
				//strcpy(string->str,"");
				memset(string->str, 0, sizeof(string->str));
#endif
				string->len = 0;
				return DDL_SUCCESS;
			}
			rc = pGlobalDict->get_dictionary_string(temp, string);
			
			/*
			* If a string was not found, get the default error string.
			*/
			
			if (rc != DDL_SUCCESS) 
			{
#ifdef _DEBUG
				LOGIT(CLOG_LOG,"Parser: Dict String Not found: Key=0x%04x\n",temp);
#endif
				rc = pGlobalDict->get_dictionary_string(DEFAULT_STD_DICT_STRING, string);
				if (rc != DDL_SUCCESS) 
				{
					return rc;
				}
			}
		}
		else 
		{
			DDL_PARSE_INTEGER(chunkp, size, (UINT64 *) NULL_PTR);
		}
		string->strType = tag;
		break;
		
	default:
		
		/*
		* Unknown tag. Default this string.
		*/
		
		if (string) 
		{
#ifdef _DEBUG
			LOGIT(CLOG_LOG,"Parser: Dict String Type is Unknown: Tag=0x%04x\n",tag);
#endif
			rc = pGlobalDict->get_dictionary_string(DEFAULT_STD_DICT_STRING, string);
			if (rc != DDL_SUCCESS) 
			{
				return rc;
			}
			
		}
		*chunkp += tag_len;
		*size -= tag_len;
	}
	
	return DDL_SUCCESS;
	
	
}/*End ddl_parse_string*/

// for debugging...


int ddl_parse_expression( unsigned char **chunkp, unsigned long *size, ddpExpression *exprList)
{
	int rc; 
	
	//ADDED By Deepak initialize vars
	unsigned long  length=0,tag=0,len=0;
	unsigned __int64 LL;
	
//	Element		exprElem;
	
#ifdef MODIFIED_BY_SS	
	//ASSERT_DBG(chunkp && *chunkp && size);
	assert(chunkp && *chunkp && size);
#endif	
	
	/*
	* Parse the tag, and make sure it is an EXPRESSION_TAG.
	*/
	
	DDL_PARSE_TAG(chunkp, size, &tag, &length);
	
	if ((DDL_UINT) EXPRESSION_TAG != tag) {
		return DDL_ENCODING_ERROR;
	}
	
	/*
	* Make sure the length makes sense.
	*/
	
	if ((unsigned long) 0 == length || (unsigned long) length > *size) {
		return DDL_ENCODING_ERROR;
	}
	
	
	*size -= length;
	
	
	while (length >0)
	{
		
	/*
	* Parse the tag, and switch on the tag type
		*/
		Element		exprElem;
		
		DDL_PARSE_TAG(chunkp, &length, &tag, &len );// NOTE : len is a placeholder
		assert(tag < MAXIMUM_UCHAR);

		switch(tag)
		{
		case INTCST_OPCODE: /* Integer constant opcode*/
			{
				UINT64	ullTemp;
				
				DDL_PARSE_INTEGER(chunkp, &length, &ullTemp);
				/* note:  The Tokenizer will insert a Unary NEG opcode for negative constants.
				So, a -10 will be encoded as 0x0a, NEG_OPCODE, rest of expression...
				>>>>expression handler: default constant type is int...not unsigned.<<<<<<
				*******/
				exprElem.byElemType = (unsigned char)tag;
				exprElem.elem.ulConst = ullTemp; 				
			}
			break;/*INTCST_OPCODE*/
			
		case FPCST_OPCODE:
			{
				float	fTemp;
				rc = ddl_parse_float(chunkp,&length,&fTemp);
				exprElem.byElemType = (unsigned char)tag;
				exprElem.elem.fConst = fTemp;				
			}
			break;/*FPCST_OPCODE*/
		case STRCST_OPCODE:
			{	
				exprElem.elem.pSTRCNST = new ddpSTRING;
				exprElem.byElemType = (unsigned char)tag;
				rc = ddl_parse_string(chunkp,&length, exprElem.elem.pSTRCNST);				
			}
			break;/*STRCST_OPCODE*/
			
		case BLOCK_OPCODE: 
		case BLOCKID_OPCODE: 
		case BLOCKREF_OPCODE: 
		case SYSTEMENUM_OPCODE:
			{
#ifdef _PARSER_DEBUG
				cerr<<"\n ddpExpression Unusable Opcode encountered!!!";
#endif						/*Log this one!!!*/
			}
			break;

		case VARID_OPCODE:
		case MAXVAL_OPCODE:
		case MINVAL_OPCODE:
		case VARREF_OPCODE:
		case MAXREF_OPCODE:
		case MINREF_OPCODE:
			{
				unsigned long whichTemp = 0;
				ITEM_ID			tempID = 0;
				
				if ((tag != VARID_OPCODE) && (tag != VARREF_OPCODE)) 
				{	/*
					* A min or max value reference.  Parse which
					* value (0, 1, 2, ...),
					*/
					DDL_PARSE_INTEGER(chunkp, &length, &LL); whichTemp = (UINT32) LL;
				}/*Endif VARID_OPCODE......*/
				
				
				switch (tag)
				{
				case VARID_OPCODE:
				case MAXVAL_OPCODE:
				case MINVAL_OPCODE:
				/*
				 * Parse the variable ID.
				 */					
					DDL_PARSE_INTEGER(chunkp, &length, &LL); 
					tempID = (ulong) LL;
					if(tag != VARID_OPCODE)
					{
						exprElem.byElemType = (unsigned char)tag;
						//exprElem.elem.minMax.which = whichTemp;
						//exprElem.elem.minMax.variable.id = tempID;
						exprElem.elem.minMax = new MIN_MAX(whichTemp,tempID);
					}/*End if*/
					else
					{	
						exprElem.byElemType = (unsigned char)tag;
						exprElem.elem.varId = tempID;
											
					}/*End else*/
					
					break;
					
				case VARREF_OPCODE:
					exprElem.byElemType = (unsigned char)tag;
					/* Parse the reference of the variable */
					exprElem.elem.varRef = new ddpREFERENCE;
					rc = ddl_parse_ref(chunkp,&length,exprElem.elem.varRef);
					
					if(rc != SUCCESS)
						return rc;
					break;

				case MAXREF_OPCODE:
				case MINREF_OPCODE:
					exprElem.byElemType = (unsigned char)tag;

//					exprElem.elem.minMax.which = whichTemp; /*Set the which value */
					exprElem.elem.minMax = new MIN_MAX();
					exprElem.elem.minMax->which  = whichTemp;
//					exprElem.elem.minMax.variable.ref = new ddpREFERENCE;
					exprElem.elem.minMax->variable.ref = new ddpREFERENCE;	
					exprElem.elem.minMax->isID = false;
//					rc = ddl_parse_ref(chunkp,&length,exprElem.elem.minMax.variable.ref);
					rc = ddl_parse_ref(chunkp,&length,exprElem.elem.minMax->variable.ref);
					
					if(rc != SUCCESS)
						return rc;
					break;
						
						
				default:	/* this cannot happen */
					//							CRASH_RET(DDL_SERVICE_ERROR);
					/* NOTREACHED */
					break;
				}/*End switch nested*/
				
			}
			break;/*VARID_OPCODE,MAXVAL_OPCODE,MINVAL_OPCODE,VARREF_OPCODE,MAXREF_OPCODE */
			
/* these are now part of the reference by attribute *
		case CNTREF_OPCODE:
		case CAPREF_OPCODE:
		case FSTREF_OPCODE:
		case LSTREF_OPCODE:
			{
				exprElem.byElemType = tag;
				/x* Parse the reference of the variable *x/
				exprElem.elem.varRef = new ddpREFERENCE;
				rc = ddl_parse_ref(chunkp,&length,exprElem.elem.varRef);
			}
			break;
** end 21apr05 stevev */
		default:
			{
				
				/* This has to be an opcode, simply push it on the list */
				exprElem.byElemType =	(BYTE)tag;
				exprElem.elem.byOpCode = (BYTE)tag;
			}
			break;
			
		}/* End (Element type) tag*/
		
		exprList->push_back(exprElem);
		//exprElem.clean();
		exprElem.Cleanup();
		
	}/*End while*/

	return DDL_SUCCESS;
	
}/* End ddl_parse_expression*/

/*

 This guy is called by the root of the conditional tree ie. the pAttr->pCond
 to parse the conditional value against a IF (THEN) / ELSE / or CASE / DEFAULT 
 arguments
 The data type of the attribute being parsed is already there in the pConditional,
 & every conditional object being declared herein this method will point to pConditional
 as the parent!!!
 
*/


/* .... from ddl_cond....*/
int ddl_parse_conditional(DDlConditional *pConditional, unsigned char **chunkp, unsigned long *size)
{
	//ADDED By Deepak initialize vars
	int rc=0;
	unsigned long len=0,lenp=0; /*Length of the data associated with the Tag*/
	unsigned long tag=0,tagp=0;/* The "tag" */
//	unsigned long temp;
	
	unsigned char* leave_pointer=NULL; /* end-of-chunk pointer*/
	bool bNestedConditional = false;

	VALUES tempVal = {0};
	
	//ddpExpression tempExpr;

	DDlConditional *pChild = NULL;
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(chunkp && *chunkp && size);
	assert(chunkp && *chunkp && size);
#endif
	/*
	 * Parse the tag to find out if this is a SELECT statement, an if/else,
	 * or a simple assignment.
	 */

	DDL_PARSE_TAG(chunkp, size, &tag, &len);

	/*
	 * Adjust size of remaining chunk.
	 */

	*size -= len;

	/*
	 * Calculate the return chunk pointer (we may be able to use it it later
	 * for an early exit).
	 */

	leave_pointer = *chunkp + len;

	switch(tag)
	{
	case IF_TAG:
		{
			/* If we reach here it means that there's a nested conditional*/
			bNestedConditional = true;
			pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed

			pConditional->Vals.push_back(tempVal); /* Push a NULL value on the value list*/
			pChild = new DDlConditional(DDL_COND_TYPE_IF,
										pConditional->attrDataType,
										1);

			/* Now Parse the ddpExpression associated with the IF block */

			rc = ddl_parse_expression(chunkp,&len,&(pChild->expr));

			if (rc != SUCCESS)
				return rc; /* Return if not successful*/

			/*otherwise Parse the value of the attribute associated with THEN clause*/
						
			rc = ddl_parse_conditional(pChild,chunkp,&len);
					
			if (rc != SUCCESS)
				return rc; /* Return if not successful*/

				/*Parse the ELSE portion if there's one*/
				if (len >0)
				{
				
							rc = ddl_parse_conditional(pChild,chunkp,&len);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/
							pChild->byNumberOfSections++;
							
				}

		/*This has to be the last statement, in this case*/
			pConditional->listOfChilds.push_back(pChild);/*Push the child on the list*/
		}
		break; /*End case :IF_TAG*/
	
	case SELECT_TAG:
		{

			bNestedConditional = true;
			pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
			/*This guy will come in each case, not in select*/
			pConditional->Vals.push_back(tempVal);	/* Push a NULL value on the value list*/

			pChild = new DDlConditional(DDL_COND_TYPE_SELECT,
										pConditional->attrDataType,
										0);


			/*Now Parse the ddpExpression Argument of the SELECT */

			rc = ddl_parse_expression(chunkp,&len,&(pChild->expr));

				if(rc != SUCCESS)
					return rc;

								
			/*otherwise Parse all the CASE branches and the DEFAULT */
			while (len > 0)
			{
				ddpExpression tempExpr;

				DDL_PARSE_TAG(chunkp,&len,&tagp,&lenp);

				switch(tagp)
				{
					case CASE_TAG:
						{
							/*We are parsing the CASE constants as ddpExpression
							just bcoz of the spec. But it should be a constant 
							value , ie. an expression with just a  constant (integer)
							value*/

							rc = ddl_parse_expression(chunkp,&len,&tempExpr);

							if(rc != SUCCESS)
								return rc;

							pChild->caseVals.push_back(tempExpr);
						
							/*We have the case constant value 
							Now parse the attributre value from the 
							following chunk	*/

							rc = ddl_parse_conditional(pChild,chunkp,&len);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/

							pChild->byNumberOfSections++;
							
							tempExpr.clear();
						}
						break;/*End CASE_TAG*/

					case DEFAULT_TAG:
						{
/*
							temp = DEFAULT_TAG_VALUE;
							pChild->caseVals.push_back(temp);
*/
							
							pChild->byNumberOfSections++;

							rc = ddl_parse_conditional(pChild,chunkp,&len);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/
						}
							break;/*End DEFAULT_TAG*/
					default:
						return DDL_ENCODING_ERROR;

							}/*End Switch tagp*/


						}/*End while*/



		/*This has to be the last statement, in this case*/
			pConditional->listOfChilds.push_back(pChild);/*Push the child on the list*/	
		}
		break;
	
	case OBJECT_TAG: /*We have a direct object, just parse it & return!!*/
		{
			bNestedConditional = false;
			pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_DIRECT);//Vibhor 200105: Changed
			switch(pConditional->attrDataType)
			{
				
			case	DDL_ATTR_DATA_TYPE_INT:
				{
					INT64 iTemp;
						
					DDL_PARSE_INTEGER( chunkp,&len,(UINT64 *)&iTemp);

					tempVal.llVal = iTemp;
					pConditional->Vals.push_back(tempVal);
				}
				break;
			case	DDL_ATTR_DATA_TYPE_UNSIGNED_LONG:
				{
					UINT64 ulTemp;
						
					DDL_PARSE_INTEGER( chunkp,&len,&ulTemp);

					tempVal.ullVal = ulTemp;
					pConditional->Vals.push_back(tempVal);

				}
				break;
			case	DDL_ATTR_DATA_TYPE_FLOAT:
			case	DDL_ATTR_DATA_TYPE_DOUBLE:
				{
					float fTemp;

					rc = ddl_parse_float(chunkp,&len,&fTemp);
					if(DDL_ATTR_DATA_TYPE_FLOAT == pConditional->attrDataType)
						tempVal.fVal = fTemp;
					else
						tempVal.dVal = (double)fTemp;
				
					pConditional->Vals.push_back(tempVal);
				}
				break;
			case	DDL_ATTR_DATA_TYPE_STRING:
				{
					//ddpSTRING strTemp;
					tempVal.strVal = new ddpSTRING;
					rc = ddl_parse_string(chunkp,&len,tempVal.strVal);
					
					pConditional->Vals.push_back(tempVal);
					
				}
				break;
			case	DDL_ATTR_DATA_TYPE_ITEM_ID:
				{
					UINT64 idTemp;
					DDL_PARSE_INTEGER( chunkp,&len,&idTemp);

					tempVal.id = (ITEM_ID)idTemp;
					pConditional->Vals.push_back(tempVal);
				
				}
				break;
			case	DDL_ATTR_DATA_TYPE_BITSTRING:
				{
					unsigned long ulTemp;
					rc = ddl_parse_bitstring(chunkp,&len,&ulTemp);
					tempVal.ullVal = ulTemp;
					pConditional->Vals.push_back(tempVal);

				}
				break;

			case	DDL_ATTR_DATA_TYPE_REFERENCE:
				{
					tempVal.ref = new ddpREFERENCE;

					rc = ddl_parse_ref(chunkp,&len,tempVal.ref);

					if(rc != DDL_SUCCESS)
						return rc;
					pConditional->Vals.push_back(tempVal);
					
				}
				break;//Vibhor 200105: Added

			case	DDL_ATTR_DATA_TYPE_EXPRESSION:
				{
					tempVal.pExpr = new ddpExpression;

					rc = ddl_parse_expression(chunkp,&len,tempVal.pExpr);
					if(rc != DDL_SUCCESS)
						return rc;
					pConditional->Vals.push_back(tempVal);
				}
				break;
			case	DDL_ATTR_DATA_TYPE_MIN_MAX:
				{
/* not conditional
					tempVal.minMaxList = new MIN_MAX_LIST;
					MIN_MAX_VALUE tmpMinMax;
					tmpMinMax.which = 0;
					rc = ddl_parse_expression(chunkp,&len,&tmpMinMax.value);
					if(rc != DDL_SUCCESS)
						return rc;
					tempVal.minMaxList->push_back(tmpMinMax);

					pConditional->Vals.push_back(tempVal);
**********/					
				}
				break;

			case	DDL_ATTR_DATA_TYPE_LINE_TYPE:
				{
					rc = ddl_parse_linetype(chunkp,&len,tempVal.lineType);
					if(rc != DDL_SUCCESS)
						return rc;
					pConditional->Vals.push_back(tempVal);
				}

			/*Following lists & compound data type will be handled 
			  separately*/

/*			case	DDL_ATTR_DATA_TYPE_ITEM_ID_LIST: 
								break;
			case	DDL_ATTR_DATA_TYPE_ENUM_LIST:
				break;
			case	DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
				break;
			case	DDL_ATTR_DATA_TYPE_TRANSACTION_LIST:
				break;
			case	DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
				break;
			case	DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
				break;
			case	DDL_ATTR_DATA_TYPE_OP_REF_TRAIL_LIST:
				break;
			case	DDL_ATTR_DATA_TYPE_DEFINITION:
				break;
			case	DDL_ATTR_DATA_TYPE_REFRESH_RELATION:
				break;
			case	DDL_ATTR_DATA_TYPE_UNIT_RELATION:
				break;
			case	DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
				break;
			case	DDL_ATTR_DATA_TYPE_MEMBER_LIST: 
				break; */

			default:
				/*it should not come here*/

				return DDL_ENCODING_ERROR;
				break;
				
			}/*end switch pConditional->attrDataType*/

		}



	}/*End switch tag*/


	return SUCCESS;
}




/* This routine if for parsing the conditionals  in case the attribute is some sort of list
  eg. ENUM_LIST or REFERENCE_LIST etc... 
  
  Each such list is preceded by a list tag which is passed as an argument in tagExpected
	
	*/

/*Vibhor 200105: Start of Code Modifications*/

/*This function if severely modified to handle the "chunks in sections of conditionals"*/

int ddl_parse_conditional_list(DDlConditional *pConditional, unsigned char **chunkp, unsigned long *size, unsigned long tagExpected)
{
//ADDED By Deepak initialize vars
	int rc=0;
	unsigned long len=0,len1=0,lenp=0; /*Length of the data associated with the Tag*/
	unsigned long tag=0,tagp=0;/* The "tag" */
//	unsigned long temp;
	
	unsigned char* leave_pointer=NULL; /* end-of-chunk pointer*/
	bool bNestedConditional = false;
	
	bool bCondSectInitialized = false; //Vibhor 200105: Added

	VALUES tempVal = {0};
	
//	ddpExpression tempExpr;

	DDlConditional *pChild = NULL; /*This guy goes on ChildList of the conditional
								     or the ConditionalList of the Chunk*/

	DDlSectionChunks *pSecChunks = NULL;//Vibhor 200105: Added//This guy is initialized only once in a single call
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(chunkp && *chunkp && size);
	assert(chunkp && *chunkp && size);
#endif
	
	DDL_PARSE_TAG(chunkp, size, &tag, &len);

	if (tag != tagExpected) {
		return DDL_ENCODING_ERROR;
	}
	
	
	*size -= len;
	
	
	

	while (len > 0) 
	{

		/*
		 * Parse the tag to find out if this is a SELECT statement, an if/else,
		 * or a simple assignment.
		 */



		DDL_PARSE_TAG(chunkp, &len, &tag, &len1);

		/*
		 * Adjust size of remaining chunk.
		 */

		len -= len1;

		/*
		 * Calculate the return chunk pointer (we may be able to use it it later
		 * for an early exit).
		 */

		leave_pointer = *chunkp + len;
		
		/*See if we have something left over in our binary chunk, if yes, we have
		a multichunklist and so lets initialize our DDlSectionChunks ptr*/
		if(len > 0 && NULL == pSecChunks)
		{
			pSecChunks = new DDlSectionChunks(pConditional->attrDataType);
		}


		switch(tag)
		{
		case IF_TAG:
			{
				
				if(NULL == pSecChunks && len == 0) //double check
				{
					/* If we reach here it means that there's a pure nested conditional*/
					pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_CONDNL);
					pConditional->Vals.push_back(tempVal); /* Push a NULL value on the value list*/

				}
				else if(NULL != pSecChunks)
				{
					/* We have a possible multichunkList for this section*/
					if(bCondSectInitialized == false)
					{
						pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_CHUNKS);
						pConditional->Vals.push_back(tempVal); /* Push a NULL value on the value list*/
						bCondSectInitialized = true;
					}
					pSecChunks->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);
					pSecChunks->byNumOfChunks++;
				}

				
				pChild = new DDlConditional(DDL_COND_TYPE_IF,
											pConditional->attrDataType,
											1);

				/* Now Parse the ddpExpression associated with the IF block */

				rc = ddl_parse_expression(chunkp,&len1,&(pChild->expr));

				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

				/*otherwise Parse the value of the attribute associated with THEN clause*/
							
				rc = ddl_parse_conditional_list(pChild,chunkp,&len1,tagExpected);
						
				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

					/*Parse the ELSE portion if there's one*/
					if (len1 >0)
					{
					
								rc = ddl_parse_conditional_list(pChild,chunkp,&len1,tagExpected);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/
								pChild->byNumberOfSections++;
								
					}

			/*We got one IF..ELSE block, now push it either on Conditionals Child List
			 or on SectionChunk's Conditional List, as the case may be*/
				if(NULL == pSecChunks)
				{//this guy belongs to the conditional
					pConditional->listOfChilds.push_back(pChild);/*Push the child on the list*/
				}
				else
				{//this guy is part of the chunk list
					pSecChunks->conditionalVals.push_back(pChild);
				}
			}
			break; /*End case :IF_TAG*/
		
		case SELECT_TAG:
			{

				if(NULL == pSecChunks && len == 0) //double check
				{
					pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_CONDNL);
					/*This guy will come in each case, not in select*/
					pConditional->Vals.push_back(tempVal);	/* Push a NULL value on the value list*/
				}
				else if(NULL != pSecChunks)
				{
					if(bCondSectInitialized == false)
					{
						pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_CHUNKS);
						pConditional->Vals.push_back(tempVal); /* Push a NULL value on the value list*/
						bCondSectInitialized = true;
					}
					pSecChunks->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);
					pSecChunks->byNumOfChunks++;
				}

				pChild = new DDlConditional(DDL_COND_TYPE_SELECT,
										pConditional->attrDataType,
										0);


				/*Now Parse the ddpExpression Argument of the SELECT */

				rc = ddl_parse_expression(chunkp,&len1,&(pChild->expr));

					if(rc != SUCCESS)
						return rc;
							
				/*otherwise Parse all the CASE branches and the DEFAULT */
				while (len1 > 0)
				{
					ddpExpression tempExpr;

					DDL_PARSE_TAG(chunkp,&len1,&tagp,&lenp);

					switch(tagp)
					{
						case CASE_TAG:
							{
								/*We are parsing the CASE constants as expression
								just bcoz of the spec. But it should be a constant 
								value , ie. an expression with just a  constant (integer)
								value*/

								rc = ddl_parse_expression(chunkp,&len1,&tempExpr);

								if(rc != SUCCESS)
									return rc;

								pChild->caseVals.push_back(tempExpr);
							
								/*We have the case constant value 
								Now parse the attributre value from the 
								following chunk	*/

								rc = ddl_parse_conditional_list(pChild,chunkp,&len1,tagExpected);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/

								pChild->byNumberOfSections++;
								
								tempExpr.clear();
							}
							break;/*End CASE_TAG*/

						case DEFAULT_TAG:
							{
/*
								temp = DEFAULT_TAG_VALUE;
								pChild->caseVals.push_back(temp);
*/
								
								pChild->byNumberOfSections++;

								rc = ddl_parse_conditional_list(pChild,chunkp,&len1,tagExpected);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/
							}
								break;/*End DEFAULT_TAG*/
						default:
							return DDL_ENCODING_ERROR;

								}/*End Switch tagp*/


							}/*End while*/



				/*We got one SELECT block, now push it either on Conditionals Child List
				or on SectionChunk's Conditional List, as the case may be*/
				if(NULL == pSecChunks)
				{//this guy belongs to the conditional
					pConditional->listOfChilds.push_back(pChild);/*Push the child on the list*/
				}
				else
				{//this guy is part of the chunk list
					pSecChunks->conditionalVals.push_back(pChild);
				}
			}
			break;
		
		case OBJECT_TAG: /*We have a direct object, just parse it & return!!*/
			{
				if(NULL == pSecChunks && len == 0) //double check
				{
					pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_DIRECT);
					/*We have SINGLE a chunk which has the desired "list" of values!!
					  switch on attDataType and parse the appropriate value */
				}
				else if(NULL != pSecChunks)
				{ //We have a possible combination of directs and conditionals
					if(bCondSectInitialized == false)
					{
						pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_CHUNKS);
						pConditional->Vals.push_back(tempVal); /* Push a NULL value on the value list*/
						bCondSectInitialized = true;
					}
					pSecChunks->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);
					pSecChunks->byNumOfChunks++;
				}
				
				VALUES tmpVal= {0};

				switch(pConditional->attrDataType)
				{
				case	DDL_ATTR_DATA_TYPE_ENUM_LIST:
					{
						tmpVal.enmList = new ENUM_VALUE_LIST;
											
						if (tmpVal.enmList != NULL) // HOMZ
						{
						rc = ddl_parse_enums(chunkp,&len1,tmpVal.enmList);
						}
						else
						{
							rc = DDL_MEMORY_ERROR;
						}

						if(rc != DDL_SUCCESS)
							return rc;
					}

					break;
				case	DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
					{
						
						tmpVal.refList = new REFERENCE_LIST;

						rc = ddl_parse_reflist(chunkp,&len1,tmpVal.refList);

						if(rc != DDL_SUCCESS)
							return rc;
						

					}
					break;
				case	DDL_ATTR_DATA_TYPE_TRANSACTION_LIST:
					{

					}
					break;
				case	DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
					{
						
						tmpVal.respCdList = new RESPONSE_CODE_LIST;

						rc = ddl_parse_respcodes(chunkp,&len1,tmpVal.respCdList);

						if(rc != DDL_SUCCESS)
							return rc;
						
					}

					break;
				case	DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
					{
						/*!!!! Ideally the control should not come here!!!! */
#ifdef XX_PARSER_DEBUG
					cerr<<"\n Conditional MENU_ITEM_LIST found!!!";
#endif						/*Log this one!!!*/

						 
						tmpVal.menuItemsList = new MENU_ITEM_LIST;

						rc = ddl_parse_menuitems(chunkp,&len1,tmpVal.menuItemsList);
						if(rc != DDL_SUCCESS)
							return rc;
						
					}
					break;
/*				case	DDL_ATTR_DATA_TYPE_REFRESH_RELATION:
					break;
				case	DDL_ATTR_DATA_TYPE_UNIT_RELATION:
					break; */
				case	DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
					{
						 
						tmpVal.itemArrElmnts = new ITEM_ARRAY_ELEMENT_LIST;

						rc = ddl_parse_itemarray(chunkp,&len1,tmpVal.itemArrElmnts);

						if(rc != DDL_SUCCESS)
							return rc;
						
					}
					break;
				case	DDL_ATTR_DATA_TYPE_MEMBER_LIST: 
					{
						 
						tmpVal.memberList = new MEMBER_LIST;

						rc = ddl_parse_members(chunkp,&len1,tmpVal.memberList);
						if(rc != DDL_SUCCESS)
							return rc;
						
					}
					break; 

				case	DDL_ATTR_DATA_TYPE_GRID_SET:
					{
#ifdef XX_PARSER_DEBUG
					cerr<<"\n Conditional GRID_TYPE found!!!";
#endif						/*Log this one!!!*/

						 
						tmpVal.gridMemList = new GRID_SET_LIST;

						rc = ddl_parse_gridMembers(chunkp,&len1,tmpVal.gridMemList);
						if(rc != DDL_SUCCESS)
							return rc;
						
					}
					break;

				default:
					/*it should not come here*/

					return DDL_ENCODING_ERROR;
					break;
					
				}/*end switch pConditional->attrDataType*/
				
				if(NULL == pSecChunks)
				{//this guy belongs to the conditional
					pConditional->Vals.push_back(tmpVal);/*Push the value on the direct vals list*/
				}
				else
				{//this guy is part of the chunk list
					pSecChunks->directVals.push_back(tmpVal);
				}



			}
			break;



		}/*End switch tag*/
	}/*End While*/

	/*See if we had a multichunk conditional section, just push it onto the conditional*/
	if(NULL != pSecChunks)
	{
		/*We may have parsed a direct list in multiple chunks !!
		 If that's the case just empty it out from the pSecChunk to the 
		 pConditional*/
		unsigned i,j = 0;
		if(pSecChunks->conditionalVals.size() == 0)
		{
			switch(pConditional->attrDataType)
			{
				case	DDL_ATTR_DATA_TYPE_ENUM_LIST:
					{
						
						tempVal.enmList = new ENUM_VALUE_LIST;

						ENUM_VALUE_LIST *pTmpEnmList = NULL;
						ENUM_VALUE tmpEnm;

						for(i = 0;i < pSecChunks->directVals.size(); i++)
						{
							pTmpEnmList = pSecChunks->directVals.at(i).enmList;
							for(j = 0; j < pTmpEnmList->size(); j++)
							{
								tmpEnm = pTmpEnmList->at(j);
								tempVal.enmList->push_back(tmpEnm);
							}
						}
						
					}

					break;
				case	DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
					{
						
						tempVal.refList = new REFERENCE_LIST;
						REFERENCE_LIST *pTmpRefList = NULL;
						ddpREFERENCE tmpRef;

						for(i = 0;i < pSecChunks->directVals.size(); i++)
						{
							pTmpRefList = pSecChunks->directVals.at(i).refList;
							for(j = 0; j < pTmpRefList->size(); j++)
							{
								tmpRef = pTmpRefList->at(j);
								tempVal.refList->push_back(tmpRef);
							}
						}


					}
					break;
				case	DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
					{
						
						tempVal.respCdList = new RESPONSE_CODE_LIST;
						RESPONSE_CODE_LIST *pTmpRspCdList = NULL;
						RESPONSE_CODE tmpRsp;

						for(i = 0;i < pSecChunks->directVals.size(); i++)
						{
							pTmpRspCdList = pSecChunks->directVals.at(i).respCdList;
							for(j = 0; j < pTmpRspCdList->size(); j++)
							{
								tmpRsp = pTmpRspCdList->at(j);
								tempVal.respCdList->push_back(tmpRsp);
							}
						}
						
					}
					break;
				case	DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
					{

						tempVal.menuItemsList = new MENU_ITEM_LIST;
						MENU_ITEM_LIST *pTmpMenuItmLst = NULL;
						MENU_ITEM tmpMnItm;
						
						for(i = 0;i < pSecChunks->directVals.size(); i++)
						{
							pTmpMenuItmLst = pSecChunks->directVals.at(i).menuItemsList;
							for(j = 0; j < pTmpMenuItmLst->size(); j++)
							{
								tmpMnItm = pTmpMenuItmLst->at(j);
								tempVal.menuItemsList->push_back(tmpMnItm);
							}
						}

					}
					break;

				case	DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
					{
						 
						tempVal.itemArrElmnts = new ITEM_ARRAY_ELEMENT_LIST;
						ITEM_ARRAY_ELEMENT_LIST *pTmpIAElmLst = NULL;
						ITEM_ARRAY_ELEMENT tmpElm;
						
						for(i = 0;i < pSecChunks->directVals.size(); i++)
						{
							pTmpIAElmLst = pSecChunks->directVals.at(i).itemArrElmnts;
							for(j = 0; j < pTmpIAElmLst->size(); j++)
							{
								tmpElm = pTmpIAElmLst->at(j);
								tempVal.itemArrElmnts->push_back(tmpElm);
							}
						}

						
					}
					break;
				case	DDL_ATTR_DATA_TYPE_MEMBER_LIST: 
					{
						 
						tempVal.memberList = new MEMBER_LIST;
						MEMBER_LIST *pTmpMemList = NULL;
						MEMBER tmpMmbr;
						
						for(i = 0;i < pSecChunks->directVals.size(); i++)
						{
							pTmpMemList = pSecChunks->directVals.at(i).memberList;
							for(j = 0; j < pTmpMemList->size(); j++)
							{
								tmpMmbr = pTmpMemList->at(j);
								tempVal.memberList->push_back(tmpMmbr);
							}
						}

						
					}
					break; 
				/* stevev 25mar05 */
				case	DDL_ATTR_DATA_TYPE_GRID_SET:
					{

						tempVal.gridMemList = new GRID_SET_LIST;
						GRID_SET_LIST *pTmpGridSetLst = NULL;
						GRID_SET tmpGridSet;
						
						for(i = 0;i < pSecChunks->directVals.size(); i++)
						{
							pTmpGridSetLst = pSecChunks->directVals.at(i).gridMemList;
							for(j = 0; j < pTmpGridSetLst->size(); j++)
							{
								tmpGridSet = pTmpGridSetLst->at(j);
								tempVal.gridMemList->push_back(tmpGridSet);
								tmpGridSet.Cleanup();

							}
						}
					}
					break;
				/* end stevev 25mar05 */
				default:
					/*it should not come here*/

					return DDL_ENCODING_ERROR;
					break;
					
			}/*end switch pConditional->attrDataType*/

			pConditional->isSectionConditionalList.pop_back();
			pConditional->isSectionConditionalList.push_back(DDL_SECT_TYPE_DIRECT);
			pConditional->Vals.pop_back();
			pConditional->Vals.push_back(tempVal);

			/*We don't need the pSecChunk any more, so delete it*/

			pSecChunks->directVals.erase(pSecChunks->directVals.begin(),pSecChunks->directVals.end());
			pSecChunks->directVals.clear();
			pSecChunks->isChunkConditionalList.clear();
			delete pSecChunks;
				
		}
		else
		{
			pConditional->listOfChunks.push_back(pSecChunks);
		}
	}/*Endif NULL != pSecChunks*/


	return SUCCESS;

}/*End ddl_parse_conditional_list*/

/*Vibhor 200105: End of Code Modifications*/


int ddl_parse_one_enum(unsigned char**chunkp,unsigned long *size,ENUM_VALUE *enmVal)
{
	//ADDED By Deepak initialize vars
	int rc=0;
	unsigned long tag=0, len=0;
	unsigned long kind_o_class=0,which=0;
	unsigned __int64 LL;
//	OUTPUT_STATUS tmpStatus;

	while(*size >0)
	{
		

		DDL_PARSE_TAG(chunkp,size, &tag, &len);

		switch(tag)
		{
		case ENUM_VALUE_TAG:
			{
				/*Parse the value of the enumeration*/

				DDL_PARSE_INTEGER(chunkp,size,&LL); enmVal->val = (UINT32)LL;

				enmVal->evaled |= ENUM_VAL_EVALED;

			}
			break;
		case ENUM_STATUS_TAG:
			{
				/*
				* Parse the status class information for the enumeration.
				*/

				*size -=len;

				/*
				 * Parse the class, and the encoding of the kind and
				 * output class (in one byte).
				 */

				DDL_PARSE_INTEGER(chunkp,&len,&LL); enmVal->status.status_class = (UINT32) LL;

				while(len > 0)
				{
					OUTPUT_STATUS tmpStatus;
					
					DDL_PARSE_INTEGER(chunkp, &len, &LL); kind_o_class = (UINT32)LL;

					if (kind_o_class == OC_NORMAL) {
						
						/*
						 * 	There should be nothing more to parse
						 */
						/*I donno if this thing needs to be pushed onto the list,
						 because we are actualyy parsing it & its actually OC_NORMAL
						*/

						if(len>0)
							len = 0;
						
						break;	/* get out of this while loop */
					}/*endif kind_o_class*/
					else
					{

						/*
						 *	In a HART binary, KIND and OUTPUT_CLASS are
						 *	combined in the same binary byte.  KIND is 
						 *	stored in bits 0-2 and OUTPUT_CLASS is stored 
						 *	in bits 3-7 of "kind_oclass".
						 */
							
							
						
							//tmpStatus = (OUTPUT_STATUS*)&enmVal->status.oclasses.list[enmVal->status.oclasses.count++];

							tmpStatus.kind
								= (kind_o_class & 0x07);
							
			/*Vibhor 071003: Since OUTPUT_CLASS is actually stored in bits 3 & 4 only
			I'm stripping off the rest unused bits, See Spec 504
			This further helps in checking the valid combinations of the output classes
			against status.oclass as 
				AUTO & GOOD 	(0x00)
				MANUAL & GOOD	(0x01)
				AUTO & BAD      (0x10)
				MANUAL & BAD    (0x11)
			*/
							/* stevev 12/7/04 marginal uses another bit*/ 
							tmpStatus.oclass
								= (kind_o_class >> 03) & 0x07; 
												

						/*
						 *	There are 4 types of KIND.
						 *		DV = dynamic variable.
						 *		TV = transmitter variable.
						 *		AO = analog output
						 *		ALL = all outputs
						 *
						 *	DV, TV and AO are followed by the integer "which" which
						 *	indicates the variable (or output) the output_class is 
						 *	associated with.
						 */

						if (tmpStatus.kind == OC_DV || tmpStatus.kind == OC_TV ||
								tmpStatus.kind == OC_AO) {
							DDL_PARSE_INTEGER(chunkp, &len, &LL); which = (UINT32)LL;
							tmpStatus.which = (unsigned short) which;
							enmVal->status.oclasses.push_back(tmpStatus);
							
																				
						}

										
					}/*End Else*/

				}/*End while len > 0*/

				enmVal->evaled |= ENUM_STATUS_EVALED;

			}
			break;
		case ENUM_ACTIONS_TAG:
			{
				/*This is an ID of a Method.... though coded as a reference
				  ie.  the reference type has to be a method ID*/
				/*so skipping the first byte ... the REFERENCE_TAG 
				 and parsing the Method ID*/
				DDL_PARSE_TAG(chunkp,size, &which,&len);

				if(METHOD_ID_REF != which)
				{
					/*log an error!!!*/
					/*myprintf(fout,"Invalid reference type in Enum Actions!!!!");*/
				}

				/*Parse the method ID!!*/

				DDL_PARSE_INTEGER(chunkp,size,&LL); enmVal->actions = (UINT32)LL;
				enmVal->evaled |= ENUM_ACTIONS_EVALED;
			}
			break;
		case ENUM_DESC_TAG:
			{
				rc = ddl_parse_string(chunkp,size,&enmVal->desc);
				if(rc != DDL_SUCCESS)
					return rc;
				enmVal->evaled |= ENUM_DESC_EVALED;

			}
			break;
		case ENUM_HELP_TAG:
			{
				rc = ddl_parse_string(chunkp,size,&enmVal->help);
				if(rc != DDL_SUCCESS)
					return rc;
				enmVal->evaled |= ENUM_HELP_EVALED;

			}
			break;
		case ENUM_CLASS_TAG:
			{
				rc = ddl_parse_bitstring(chunkp,size,&enmVal->func_class);
				if(rc != DDL_SUCCESS)
					return rc;

				enmVal->evaled |= ENUM_CLASS_EVALED;
			}
			break;
		default:
			return DDL_ENCODING_ERROR;
			break;



		}/*End switch tag*/


	}/*End while *size >0 */
/*** these are unspeced string additions  stevev 01feb12
	if(!(enmVal->evaled & ENUM_DESC_EVALED))
	{
		enmVal->desc.str = new char[18];
		strcpy(enmVal->desc.str,"No Desc Available");
		enmVal->desc.strType = DEV_SPEC_STRING_TAG;
		enmVal->desc.flags   = FREE_STRING;
		enmVal->desc.len = strlen(enmVal->desc.str);
		enmVal->evaled |= ENUM_DESC_EVALED;
	}

	if(!(enmVal->evaled & ENUM_HELP_EVALED))
	{
		enmVal->help.str = new char[18];
		strcpy(enmVal->help.str,"No Help Available");
		enmVal->help.strType = DEV_SPEC_STRING_TAG;
		enmVal->help.flags   = FREE_STRING;
		enmVal->help.len = strlen(enmVal->help.str);
		enmVal->evaled |= ENUM_HELP_EVALED;
	}
******/
	return DDL_SUCCESS;
}



int ddl_parse_enums(unsigned char **chunkp,unsigned long *size,ENUM_VALUE_LIST *pEnmList)
{
	int rc=0;
	unsigned long len=0, tag=0; /*length & tag of the parsed binary*/
	
	//ENUM_VALUE tempEnm; /*Enum value to be parsed*/

		
	while(*size > 0)
	{

		ENUM_VALUE tempEnm; //was (void)memset((char *) &tempEnm, 0, sizeof(ENUM_VALUE));
		/*
		 * Parse the tag, and make sure it is an ENUMERATOR_TAG
		 */

		DDL_PARSE_TAG(chunkp, size, &tag, &len);

		if (tag != ENUMERATOR_TAG) {
			return DDL_ENCODING_ERROR;
		}

		*size -= len;

		rc = ddl_parse_one_enum(chunkp,&len,&tempEnm);

		if(rc != DDL_SUCCESS)
			return rc;

		pEnmList->push_back(tempEnm);

		tempEnm.Cleanup();

#ifdef DICT_REF_DEBUG
		if (! pGlobalDict->dict_ok())
		{
			cerr<<"Error."<<endl;
		}
#endif//DICT_REF_DEBUG

	
	}/*End while *size > 0*/


	return DDL_SUCCESS;

}

int ddl_parse_ref(unsigned char **chunkp,unsigned long *size, ddpREFERENCE *ref)
{
	int rc=0; /*return code*/
	
	unsigned long tag=0,len=0;/*tag & length of the parse binary*/
//	ddpREF			tmpRef;/* temporary reference structure*/
	bool		parse = true;
	unsigned __int64 LL;

//	ddpExpression tempExpr; /*temporary expression to parse the item_array index*/
#ifdef MODIFIED_BY_SS	
	//ASSERT_DBG(chunkp && *chunkp && size);
	assert(chunkp && *chunkp && size);
#endif

	while(parse)
	{
		/*Parse the tag to find out the type of reference*/
		ddpREF			tmpRef;// was (void)memset((char*)&tmpRef,0,sizeof(ddpREF));

		DDL_PARSE_TAG(chunkp, size, &tag,&len);
		tmpRef.type = (unsigned short)tag;

		switch(tag)
		{
			case ITEM_ID_REF:
			case ITEM_ARRAY_ID_REF:
			case COLLECTION_ID_REF:
			case BLOCK_ID_REF:
			case VARIABLE_ID_REF:
			case MENU_ID_REF:
			case EDIT_DISP_ID_REF:
			case METHOD_ID_REF:
			case REFRESH_ID_REF:
			case UNIT_ID_REF:
			case WAO_ID_REF:
			case RECORD_ID_REF:
			case ARRAY_ID_REF:
			/*case VAR_LIST_ID_REF	
			  case PROGRAM_ID_REF	
			  case DOMAIN_ID_REF
			*/
			case RESP_CODES_ID_REF:
			case FILE_ID_REF:		
			case CHART_ID_REF:
			case GRAPH_ID_REF:
			case AXIS_ID_REF:
			case WAVEFORM_ID_REF:
			case SOURCE_ID_REF:
			case LIST_ID_REF:
/* stevev 23mar05 - these are new items */
			case IMAGE_ID_REF:
			case GRID_ID_REF:
/* end stevev 23mar05 */
/* stevev 06feb13 - these are new items */				
			case BLOB_ID_REF:
			case TEMPLATE_ID_REF:
			case PLUGIN_ID_REF:
/* end stevev  06feb13*/

				{
					if(tag == RECORD_ID_REF)
					{
						tmpRef.type = COLLECTION_ID_REF;
					}
					// else leave it tag

					/*Parse the ID*/
					DDL_PARSE_INTEGER(chunkp,size,&LL); tmpRef.val.id = (ITEM_ID)LL;
					if(ref->size()> 0)
					{	
						/*It comes here if we have a via.... reference in the 
						 parent*/
						if(tag == ITEM_ID_REF)
						{
							if(ref->at(ref->size()-1).type == VIA_COLLECTION_REF
							|| ref->at(ref->size()-1).type == VIA_RECORD_REF)
							{
								tmpRef.type = COLLECTION_ID_REF;
							}
							else 
							if(ref->at(ref->size()-1).type == VIA_ITEM_ARRAY_REF)
							{
								tmpRef.type = ITEM_ARRAY_ID_REF;
							}
						/*	else if(ref->at(ref->size()-1).type == VIA_RECORD_REF)
							{
								tmpRef.type = RECORD_ID_REF;
							} */
							else
							if ((ref->at(ref->size()-1).type) == VIA_FILE_REF)
							{
								tmpRef.type = FILE_ID_REF;
							} 
							else
							if ((ref->at(ref->size()-1).type) == VIA_LIST_REF)
							{
								tmpRef.type = LIST_ID_REF;
							} 	
							else
							if ((ref->at(ref->size()-1).type) == VIA_BITENUM_REF)
							{
								tmpRef.type = VARIABLE_ID_REF;
							} 	// else - leave as is
						}/*Endif tag*/

					}/*Endif ref->size()> 0*/

					ref->push_back(tmpRef);			
					
					parse = false;
				}
				break;

//			case IMAGE_ID_REF:/* if it walks like a duck ...  */ 
//							//handle as a expression  130904//do not handle as expression 7jan05
//				{
//					// get the integer
//					DDL_PARSE_INTEGER(chunkp,size,&(tmpRef.val.id));
//					// insert it as an expression in the reference
//					Expression* pE   =  new Expression;
//					Element		exprElem;
//					exprElem.byElemType   = INTCST_OPCODE;
//					exprElem.elem.ulConst = tmpRef.val.id; 
//
//					pE->push_back(exprElem);
//					tmpRef.val.index = pE;	// replaces the id
//					// clear the reference id
//					// this will null the pointer...tmpRef.val.id = 0;
//					// save it					
//					ref->push_back(tmpRef);			
//					
//					parse = false;
//				}
//				break;
			case VIA_ARRAY_REF:
			case VIA_ITEM_ARRAY_REF:
			case VIA_LIST_REF:
				{
					tmpRef.val.index = new ddpExpression;
					/*Parse the expression which follows, for a constant integer*/
					
					rc = ddl_parse_expression(chunkp,size,tmpRef.val.index);
					/*tmpRef.val.index = tempExpr.at(0).elem.ulConst; */
					
					ref->push_back(tmpRef);
				}
				break;

			case VIA_VAR_LIST_REF:
#ifdef _PARSER_DEBUG
				cerr<<"\n viaVarList reference encountered!!!\n";
#endif					/*Log this one!!!!!*/

			case VIA_COLLECTION_REF:
			case VIA_RECORD_REF:
			case VIA_FILE_REF:
			case VIA_BITENUM_REF:
				/* stevev 06may05 */				
			case VIA_CHART_REF:
			case VIA_GRAPH_REF:
			case VIA_SOURCE_REF:
			case VIA_ATTR_REF:
				/* end 06may06 */				
			/*case VIA_BLOB_REF: 06feb13 we may have to handle this later */
				{
					if ( tag == VIA_RECORD_REF)
						tmpRef.type = VIA_COLLECTION_REF;// translate munge

					/*Parse the membername of the collection / record / varlist*/
					DDL_PARSE_INTEGER(chunkp,size,&LL); tmpRef.val.member = (UINT32)LL; //For VIA_BITENUM its the BitMask

					ref->push_back(tmpRef);
				}
				break;
			case VIA_PARAM_REF:
			case VIA_PARAM_LIST_REF:
				{
					unsigned long ulRefID;/*ID of the item referred by the member*/
					unsigned short usRefType; /*Type of the item*/
					unsigned long ulParamName;
			
					/*Parse the name of param / param list*/

					DDL_PARSE_INTEGER(chunkp,size,&LL); tmpRef.val.member = (UINT32)LL;
					
					ulParamName = tmpRef.val.member;
					/*Now resolve the parameter reference from the HART block*/
					
					ulRefID = ddl_resolve_param_reference(ulParamName,&usRefType);

					if(ulRefID == 0)
					{/*It should not come here */

#ifdef _PARSER_DEBUG
				cerr<<"\n Failed to resolve viaParam Reference!!\n";
				/*Log this one!!!!!*/
#endif				
						tmpRef.type = (unsigned short)tag;

						ref->push_back(tmpRef);

						parse = false;

						break;
					}
					/*Else we have a resolved value & type */					

					tmpRef.type = usRefType;

					tmpRef.val.id = ulRefID;

					ref->push_back(tmpRef);

					parse = false;

				}
				break;
			case VIA_BLOCK_REF:
				{
					tmpRef.type = (unsigned short)tag;
					/*Parse the name of the characteristic record*/

					DDL_PARSE_INTEGER(chunkp,size,&LL); tmpRef.val.id = (ITEM_ID)LL;

#ifdef _PARSER_DEBUG
				cerr<<"\n viaBlock reference encounetered!!!\n";
#endif					/*Log this one!!!!!*/

					ref->push_back(tmpRef);

					parse = false;

				}
				break;
/* stevev 23mar05: try...*/
			case ROWBREAK_REF:
			case SEPARATOR_REF:	
				{
					tmpRef.val.index = NULL; //Just defensive- val is a UNION, clear before setting!!!
					tmpRef.val.id = tmpRef.val.member = 0;
					ref->push_back(tmpRef);

					parse = false;
				}
				break;
			//case IMAGE_REF:
			case CONSTANT_REF:
				{
					tmpRef.val.index = new ddpExpression;
//DDL_PARSE_TAG(chunkp, size, &tag, &length);
					/*Parse the expression which follows, for a constant  */					
					rc = ddl_parse_expression(chunkp,size,tmpRef.val.index);
					
					ref->push_back(tmpRef);

					parse = false;
				}
				break;

			default:
				return DDL_ENCODING_ERROR;
				break;
												
		}/*End switch tag*/


	}/*End while parse*/

	return DDL_SUCCESS;
		
}/*End ddl_parse_ref*/



int ddl_parse_reflist(unsigned char **chunkp,unsigned long *size,REFERENCE_LIST *refList)
{
	//ADDED By Deepak initialize vars
	int rc=0;
//	unsigned long tag, len;/*tag & length of the parse binary*/
	
//	ddpREFERENCE	tempRef; /*Reference to be parsed*/

	while(*size > 0)
	{
		ddpREFERENCE	tempRef;
		rc = ddl_parse_ref(chunkp,size,&tempRef);
		if(rc != DDL_SUCCESS)
				return rc;
		refList->push_back(tempRef);

		tempRef.clear();/*clean it for the next iteration*/
//		tempRef.erase(tempRef.begin(),tempRef.end());
	}/*End while */

	return DDL_SUCCESS;

}/*End ddl_parse_reflist*/


int ddl_parse_gridMembers(unsigned char **chunkp,unsigned long *size, GRID_SET_LIST *pGridSets)
{ 
	int           rc     =0;
	unsigned long tmpQual=0;
	unsigned long tag,  len;
	unsigned long tag1, len1;

	GRID_SET tmpGridSet;

	while(*size > 0)// a sequence of grid members
	{
		/* verify the tag */
		DDL_PARSE_TAG(chunkp,size,&tag,&len);
		
		if(tag != GRID_ELEMENT_TAG)
			return DDL_ENCODING_ERROR;

		/* assume we will parse the whole thing */
		*size -= len;

		if (len > 0 )// grid element contents
		{
			/*Parse the String*/
			rc = ddl_parse_string(chunkp,&len,&tmpGridSet.desc);
			if(rc != DDL_SUCCESS)
				return rc;
			
			DDL_PARSE_TAG(chunkp,&len,&tag1,&len1);
		
			if(tag1 != GRID_MEMBERS_TAG)
				return DDL_ENCODING_ERROR;

			rc = ddl_parse_reflist(chunkp,&len1,&tmpGridSet.values);
			if(rc != DDL_SUCCESS)
				return rc;
		}
#ifdef _PARSER_DEBUG
		else
		{
			cerr<<"\n parsing grid with an empty (len=0) member\n";
		}
#endif	
	
		pGridSets->push_back(tmpGridSet);
		// trashes the string ptrs:::tmpGridSet.Cleanup();
		REFERENCE_LIST :: iterator it;

		ddpREFERENCE :: iterator it1;

		for(it = tmpGridSet.values.begin(); it != tmpGridSet.values.end(); it++)
		{
			for(it1 = (*it).begin(); it1 != (*it).end(); it1++)
			{
				(*it1).Cleanup();
				
			}
			(*it).clear();
		}
		tmpGridSet.values.clear();



	}/* wend members are left to process */
	return DDL_SUCCESS;

}/*End ddl_parse_gridMembers*/


int ddl_parse_menuitems(unsigned char **chunkp,unsigned long *size,MENU_ITEM_LIST *pMenuItems)
{
	//ADDED By Deepak initialize vars
	int rc=0;
#ifdef _DEBUG
int cnt = 0;
#endif
	unsigned long tmpQual=0;

//	MENU_ITEM tmpMenuItem;

	while(*size > 0)
	{
		/*Parse the Item Reference*/

		MENU_ITEM tmpMenuItem; // was(void)memset((char*)&tmpMenuItem,0,sizeof(MENU_ITEM));

		rc = ddl_parse_ref(chunkp,size,&tmpMenuItem.item);
		if(rc != DDL_SUCCESS)
			return rc;
#ifdef XX_PARSER_DEBUG
		else
		{//    .val.id/member/expr
			int z = tmpMenuItem.item[0].type;
			cerr<<"\n MenuItem Ref ** Type:"<< z <<"\n";
			if ( ( z>= 0 && z <= 2) || ( z >= 11 && z <= 31))
			{
			cerr<<  "                   ID:"<<tmpMenuItem.item[0].val.id <<"\n";
			}
			else if ((z >= 3 && z <= 10) ||(z >= 35 && z <= 37))
			{
			cerr<<  "                  via:"<<"????"<<"\n";
			}
			else if (z == 32)
			{
			cerr<<  "                Image:"<<tmpMenuItem.item[0].val.id <<"\n";
			}
			else if (z == 33)
			{
			cerr<<  "                  SEP:\n";
			}
			else if (z == 34)
			{
			cerr<<  "                Const:\n";
			}
			else
			{
			cerr<<  "            UNKNOWN TYPE:\n";
			}
		}
#endif					/*Log this one!!!!!*/

		/*Parse the Item Qualifier*/

		rc = ddl_parse_bitstring(chunkp,size,&tmpQual);

		if(rc != DDL_SUCCESS)
			return rc;

		tmpMenuItem.qual = (unsigned short)tmpQual;

		pMenuItems->push_back(tmpMenuItem);

#ifdef _DEBUG
cnt++;
#endif
		tmpMenuItem.Cleanup();

	}/*End while *size > 0*/

	return DDL_SUCCESS;

}/*End ddl_parse_menuitems*/


unsigned long ddl_mask_width(unsigned long mask)
{
	//ADDED By Deepak initialize vars
	unsigned long        count=0;
	unsigned long        bitmask=0;

	for (bitmask = 1; bitmask; bitmask <<= 1) {
		if (mask & bitmask) {
			break;
		}
	}

	for (count = 0; bitmask; bitmask <<= 1, count++) {
		if (!(mask & bitmask)) {
			break;
		}
	}

	return count;
}



int ddl_parse_data_item(unsigned char **chunkp, unsigned long *size, DATA_ITEM* dataItem)
{
	//ADDED By Deepak initialize vars
	int rc=0;
	unsigned long tag=0, len=0;
	unsigned long tmp=0;
	unsigned __int64 LL;
	
#ifdef MODIFIED_BY_SS	
	//ASSERT_DBG(chunkp && *chunkp && *size);
	assert(chunkp && *chunkp && *size);
#endif
	/*Parse the tag to find what type of data item is this*/

	DDL_PARSE_TAG(chunkp,size,&tag,&len);
	assert(tag < DDL_UINT16_MAX);

	switch(tag)
	{
		case	DATA_CONSTANT:
			/*Its an Integer constant!! Just parse it*/
			DDL_PARSE_INTEGER(chunkp,size,&LL);
			assert(LL < DDL_UINT16_MAX);

			dataItem->data.iconst = (unsigned short)LL;
			dataItem->type        = (unsigned short)tag;
			dataItem->flags       = 0;
			//dataItem->width       = 0;
			dataItem->mask        = 0;
			break;

		case	DATA_REFERENCE:

			/*Its a variable reference!!*/
			dataItem->data.ref = new ddpREFERENCE;
			rc = ddl_parse_ref(chunkp,size,dataItem->data.ref);
			if(rc != DDL_SUCCESS)
				return rc;
			dataItem->type = (unsigned short)tag;
			dataItem->flags = 0;
			//dataItem->width = 0;
			dataItem->mask = 0;
			break;
			
		case	DATA_REF_FLAGS:
			/*
			 * A data reference with INFO or INDEX flag bits
			 */
			dataItem->data.ref = new ddpREFERENCE;
			rc = ddl_parse_ref(chunkp,size,dataItem->data.ref);
			if(rc != DDL_SUCCESS)
				return rc;

			/*Now parse the flags*/
			rc = ddl_parse_bitstring(chunkp,size,&tmp);
			if(rc != DDL_SUCCESS)
				return rc;
			dataItem->type  = (unsigned short)tag;
			dataItem->flags = (unsigned short)tmp;
			//dataItem->width = 0;
			dataItem->mask = 0;
			break;

		case	DATA_REF_WIDTH:
			/*
			 * A data reference with a data mask.
			 */
			dataItem->data.ref = new ddpREFERENCE;
			rc = ddl_parse_ref(chunkp,size,dataItem->data.ref);
			if(rc != DDL_SUCCESS)
				return rc;
			
			dataItem->type = (unsigned short)tag;

			/*Parse the data mask*/

			DDL_PARSE_INTEGER(chunkp,size,&LL);

// stevev - 18jun09 - not decoding properly (decoder assumes 8 bit masks)
//			dataItem->width = (unsigned short)ddl_mask_width((UINT32)LL);
// encoding this is ludicrous anyway
			dataItem->mask = LL;			
			break;

		case	DATA_REF_FLAGS_WIDTH:
			/*
			 * A data reference INFO or INDEX flag bits and a data
			 * mask
			 */
			dataItem->data.ref = new ddpREFERENCE;
			rc = ddl_parse_ref(chunkp,size,dataItem->data.ref);
			if(rc != DDL_SUCCESS)
				return rc;

			/*Parse the data mask*/

			DDL_PARSE_INTEGER(chunkp,size,&LL);

// stevev - 18jun09 - not decoding properly (decoder assumes 8 bit masks)
//			dataItem->width = (unsigned short)ddl_mask_width((UINT32)LL);
// encoding this is ludicrous anyway
			dataItem->mask = LL;

			/*Parse the flags*/

			rc = ddl_parse_bitstring(chunkp,size,&tmp);
			if(rc != DDL_SUCCESS)
				return rc;
			
			dataItem->flags = (unsigned short)tmp;			
			dataItem->type  = (unsigned short)tag;
			break;

		case	DATA_FLOATING:
			/*A floating point data item*/

			rc = ddl_parse_float(chunkp,size,&dataItem->data.fconst);

			if(rc != SUCCESS)
				return rc;

			dataItem->type = (unsigned short)tag;
			break;

		default:
			return DDL_ENCODING_ERROR;
			break;
			
	}/*End switch*/

	return DDL_SUCCESS;
		
}/*End ddl_parse_data_item*/



/*With the current design we are not in a position to store a "Conditional - subattribute"
so the following method just parses DIRECT object case of the DATA_ITEM_LIST*/

int ddl_parse_dataitems(unsigned char **chunkp, unsigned long *size, DATA_ITEM_LIST* dataItemList)
{
	//ADDED By Deepak initialize vars
	int rc=0;
	unsigned long len=0,len1=0,tag=0; /*length & tag of the parsed binary*/
	unsigned char* leave_pointer=NULL; /*for safe & proper exit*/

//	DATA_ITEM tmpDataItem;

	/*Parse the tag , it should be DATA_ITEMS_SEQLIST_TAG*/

	DDL_PARSE_TAG(chunkp,size,&tag,&len);

	if(DATA_ITEMS_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;
	
	*size -= len;
	
	leave_pointer = *chunkp + len;
	
	while(len > 0)
	{
		/*Now parse the tag to know whether its an IF / SELECT or a DIRECT tag*/
	
		DDL_PARSE_TAG(chunkp,&len,&tag,&len1)
		
		len -= len1;
				
		switch(tag)
		{
			case IF_TAG:
			case SELECT_TAG:
				/*Not handled!!!!*/
	#ifdef _PARSER_DEBUG
				cerr<<"\nConditional encountered in parsing DATA_ITEM_LIST!!!!\n";

	#endif /* _PARSER_DEBUG */
				return DDL_UNHANDLED_STUFF_FAILURE;
				break;
			case OBJECT_TAG:
				while(len1 > 0)
				{
					DATA_ITEM tmpDataItem; //was (void)memset((char*)&tmpDataItem,0,sizeof(DATA_ITEM));

					rc = ddl_parse_data_item(chunkp,&len1,&tmpDataItem);

					if(rc != DDL_SUCCESS)
					return rc;

				dataItemList->push_back(tmpDataItem);
				}/*End while*/

				break;
			default:
				return DDL_ENCODING_ERROR;
		}/*End switch tag*/

	}

	/*This is a safe exit ....*/
	*chunkp = leave_pointer;

	return DDL_SUCCESS;

}/*End ddl_parse_dataitems*/




int ddl_parse_respcodes(unsigned char **chunkp, unsigned long *size, RESPONSE_CODE_LIST* respCodeList)
{
	//ADDED By Deepak initialize vars
	int rc=0;
	unsigned long tag=0, len=0, tmp=0;
	unsigned __int64 LL;

//	RESPONSE_CODE tmpRespCode;

	/* this is crashing the system while trying to convert to wide char 
	ASSERT_DBG(chunkp && *chunkp && *size);
	*/
	if (!chunkp || !( *chunkp ) || !( *size ))
	{
		LOGIT(CLOG_LOG|CERR_LOG,"ddl_parse_respcodes has a bad parameter!\n");
	}

	while(*size > 0)
	{
		/*Parse the tag it should be RESPONSE_CODE_TAG*/

		DDL_PARSE_TAG(chunkp,size,&tag,&len);

		if(RESPONSE_CODE_TAG !=tag)
			return DDL_ENCODING_ERROR;

		*size -= len;

		RESPONSE_CODE tmpRespCode; // was (void)memset((char*)&tmpRespCode,0,sizeof(RESPONSE_CODE));

		/*Now parse the value , type, description & help of the response code*/

		DDL_PARSE_INTEGER(chunkp,&len,&LL);

		tmpRespCode.val = (unsigned short)LL;

		DDL_PARSE_INTEGER(chunkp,&len,&LL);

		tmpRespCode.type = (unsigned short)LL;

		rc = ddl_parse_string(chunkp,&len,&tmpRespCode.desc);

		if(rc != DDL_SUCCESS)
			return rc;

		tmpRespCode.evaled |= RS_TYPE_EVALED | RS_VAL_EVALED | RS_DESC_EVALED;
		
		if(len > 0) /*If help is there...*/
		{	/*Parse it!!*/
			
			rc = ddl_parse_string(chunkp,&len,&tmpRespCode.help);

			if(rc != DDL_SUCCESS)
			return rc;

			tmpRespCode.evaled |= RS_HELP_EVALED;
		
		}/*Endif len*/

		/*push this value on to the list*/
	
		respCodeList->push_back(tmpRespCode);
		
	}/*End while *size > 0*/

return DDL_SUCCESS;

}/*End ddl_parse_respcodes*/




int ddl_parse_itemarray(unsigned char **chunkp, unsigned long *size, ITEM_ARRAY_ELEMENT_LIST* itemArray)
{
	//ADDED By Deepak initialize vars
	int rc=0;
	unsigned long tag=0, len=0, tmp=0;
	unsigned __int64 LL;

//	ITEM_ARRAY_ELEMENT tmpElement;
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(chunkp && *chunkp && *size);
	assert(chunkp && *chunkp && *size);
#endif
	while (*size > 0)
	{
		/*Parse the tag , it should be ITEM_ARRAY_ELEMENT_TAG*/

		DDL_PARSE_TAG(chunkp,size,&tag,&len);

		if(ITEM_ARRAY_ELEMENT_TAG != tag)
			return DDL_ENCODING_ERROR;

		*size -= len;

		ITEM_ARRAY_ELEMENT tmpElement; //was (void)memset((char*)&tmpElement,0,sizeof(ITEM_ARRAY_ELEMENT));

		/* Parse the index*/
		DDL_PARSE_INTEGER(chunkp,&len,&LL);

		tmpElement.index = (unsigned short)LL;

		/*Parse the item*/

		rc = ddl_parse_ref(chunkp,&len,&tmpElement.item);
		if(rc != DDL_SUCCESS)
			return rc;
		
		tmpElement.evaled |= IA_INDEX_EVALED | IA_REF_EVALED;

		if(len >0) /*If description & help is there*/
		{ 
			/*Parse description*/
			rc = ddl_parse_string(chunkp,&len,&tmpElement.desc);
			if(rc != DDL_SUCCESS)
			return rc;
			
			tmpElement.evaled |= IA_DESC_EVALED;
			if(len > 0) 
			{
				rc = ddl_parse_string(chunkp,&len,&tmpElement.help);
				if(rc != DDL_SUCCESS)
				return rc;
				tmpElement.evaled |= IA_HELP_EVALED;
			}/*Endif len nested*/
			
		}/*Endif len*/

		itemArray->push_back(tmpElement);

	}/*End while *size > 0*/

	return DDL_SUCCESS;

}/*End ddl_parse_itemarray*/



int ddl_parse_members(unsigned char **chunkp, unsigned long *size, MEMBER_LIST* memberList)
{
	//ADDED By Deepak initialize vars
	int rc=0;
	unsigned long tag=0, len=0;//, tmp=0;
	unsigned __int64 LL;

//	MEMBER tmpMember;

	//ADDED By Deepak 
	if(NULL== memberList)
		return DDL_ENCODING_ERROR;
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(chunkp && *chunkp && *size);
	assert(chunkp && *chunkp && *size);
#endif
	while (*size > 0)
	{
		/*Parse the tag , it should be MEMBER_TAG*/

		DDL_PARSE_TAG(chunkp,size,&tag,&len);

		if(MEMBER_TAG != tag)
			return DDL_ENCODING_ERROR;

		*size -= len;

		MEMBER tmpMember; // was (void)memset((char*)&tmpMember,0,sizeof(MEMBER));

		/* Parse the name*/
		DDL_PARSE_INTEGER(chunkp,&len,&LL);

		tmpMember.name = (UINT32)LL;
		/*Parse the item*/

		rc = ddl_parse_ref(chunkp,&len,&tmpMember.item);
		if(rc != DDL_SUCCESS)
			return rc;
		
		tmpMember.evaled |= MEM_NAME_EVALED | MEM_REF_EVALED;

		if(len >0) /*If description & help is there*/
		{ 
			if ( **chunkp == 57 )
			{// member name - do before optional desc&help
				
				rc = parse_ascii_string(tmpMember.member_name, chunkp,len);
				if(rc != DDL_SUCCESS)
					return rc;				
			}
			if(len >0) /*If description & help is there*/
			{ 
				/*Parse description*/
				rc = ddl_parse_string(chunkp,&len,&tmpMember.desc);
				if(rc != DDL_SUCCESS)
				return rc;
				
				tmpMember.evaled |= MEM_DESC_EVALED;
				if(len > 0) 
				{
					rc = ddl_parse_string(chunkp,&len,&tmpMember.help);
					if(rc != DDL_SUCCESS)
					return rc;
					tmpMember.evaled |= MEM_HELP_EVALED;
				}/*Endif len nested*/
			}//endif - len after member name
			
		}/*Endif len*/

		memberList->push_back(tmpMember);

	}/*End while *size > 0*/

	return DDL_SUCCESS;


}/*End ddl_parse_members*/


unsigned long ddl_resolve_param_reference(unsigned long paramName, unsigned short *usType)
{
	
	unsigned long ulResolvedID = 0;

	ItemAttrList :: iterator p;
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(pBlock);
	assert(pBlock);
#endif
	if (pBlock->attrList.size() == 0)
		return ulResolvedID;
	
	DDlAttribute *pAttr;

	for(p = pBlock->attrList.begin(); p != pBlock->attrList.end();p++)
	{
		if((*p)->byAttrID == BLOCK_PARAM_ID)
		{
			pAttr = *p;
			/*Now iterate thru the memberlist & find the value*/
			
			/*Just return if Member List is empty*/
	
			if(pAttr->pVals->memberList->size() == 0)
				break;
			
			MEMBER_LIST :: iterator it;

			MEMBER member;

			for(it = pAttr->pVals->memberList->begin(); it != pAttr->pVals->memberList->end(); it++)
			{
				member = *it;
				if(member.name == paramName)
				{
					/*Assumption: This reference should be a direct reference
					of type ID, coz here we can't handle a via Reference */
					for(unsigned i = 0; i < member.item.size();i++)
					{
						switch(member.item[i].type)
						{
							case ITEM_ID_REF:
							case ITEM_ARRAY_ID_REF:
							case COLLECTION_ID_REF:
							case BLOCK_ID_REF:
							case VARIABLE_ID_REF:
							case MENU_ID_REF:
							case EDIT_DISP_ID_REF:
							case METHOD_ID_REF:
							case REFRESH_ID_REF:
							case UNIT_ID_REF:
							case WAO_ID_REF:
							case RECORD_ID_REF:
								{
									ulResolvedID = member.item[i].val.id;
									*usType = member.item[i].type;
								}
								break;
							default:
								/*We can't handle others here
								 & ideally it shouldn't come here either!!!!*/
								/* Log this one!!!!*/
								break;

						}/*End switch*/
						
						break; /*for i*/
					}/*End for i*/
				/*We are done !!! Just break out of this loop & return*/
				
					break; /*for it*/

				}/*Endif member.name*/
					
			}/*End for it*/
			
		break; /*for iterator p*/
		}/*End if (*p)->byAttrID*/


	}/*End for p*/
	
	return ulResolvedID;
}/*End ddl_resolve_param_reference*/

/*Vibhor 270804: Start of Code*/

int ddl_parse_linetype(unsigned char **chunkp, unsigned long *size, LINE_TYPE & lnType)
{
	int rc=0;
	unsigned long tag=0, len=0;
	unsigned long tmp=0;
	unsigned __int64 LL;
#ifdef MODIFIED_BY_SS
	//ASSERT_DBG(chunkp && *chunkp && *size);
	assert(chunkp && *chunkp && *size);
#endif
	/*Parse the tag to find what type of line type is this*/

	DDL_PARSE_TAG(chunkp,size,&tag,&len);
	assert(tag < DDL_UINT16_MAX);

	switch(tag)
	{
	
		case	DATA_LINETYPE:
				{
					DDL_PARSE_INTEGER(chunkp,size,&LL);
					lnType.qual = (UINT32)LL;
				}
				break;
		case	LOWLOW_LINETYPE:				
		case	LOW_LINETYPE:		
		case	HIGH_LINETYPE:		
		case	HIGHHIGH_LINETYPE:	
		case	TRANSPARENT_LINETYPE:
				lnType.qual = 0;
				break;

		default:
			return DDL_ENCODING_ERROR;
			break;

	}/*End switch*/

	lnType.type = (unsigned short)tag;

	return DDL_SUCCESS;

}/*End ddl_parse_linetype*/

/*Vibhor 270804: End of Code*/
