

/*!!!!!   Take care of moderating the contents of this file		!!!!!*/
#pragma warning (disable : 4786)
#include "pch.h"
#include "ddbGeneral.h"

#include    "Fetch_item.h"
#include <cassert>

//Vibhor 200105: Increasing the size of arrays, was 1500
extern DOMAIN_FIXED ObjectFixed[];//[1800];
extern BYTE byExtLengths[];//[1800]; /*Array to hold the object Extension Lengths*/ //Vibhor 300904: Restored
extern BYTE *pbyExtensions[];//[1800];/*Array of pointers pointing to the Extension parts of objects*/
extern BYTE *pbyObjectValue[];//[1800];/*Array of pointers pointing to the value (data) parts of the objects*/
extern unsigned uSODLength;
extern bool
 bTokRev6Flag; //Vibhor 270904: Added
/*Globals*/



//#include	"fch_lib.h"

/* #defines */




/*********************************************************************
 *
 *	Name:	count_bits
 *
 *	ShortDesc: 	Calculates number of bits set
 *
 *	Description:
 *		Sums the number of bits set in an unsigned long field.
 *
 *	Inputs:
 *		bfield -	unsigned long value
 *
 *	Outputs:
 *
 *	Returns:
 *		bit_cnt -	total number of bits set
 *
 *	Author:
 *		David Bradsher
 *
 *********************************************************************/

 unsigned short
count_bits(unsigned long bfield)
{
	unsigned short  bit_cnt;

	for (bit_cnt = 0; bfield != 0; bfield >>= 1) {
		if (bfield & 1L) {
			bit_cnt++;
		}
	}
	return (bit_cnt);
}


/*********************************************************************
 *
 *	Name: parse_attribute_id
 *
 *	ShortDesc: Get values from Attribute Identifier
 *
 *	Description:
 *		Parse the Attribute Identifier portion of the item attribute
 *		into the RI, tag and length values
 *
 *	Inputs:
 *		attr_ptr -		pointer to first byte of Attribute ID
 *
 *	Outputs:
 *		attr_ptr -		pointer to first byte after Attribute ID
 *		attr_RI -		pointer to value of attribute reference
 *						indicator
 *		attr_tag -		pointer to value of attribute tag
 *		attr_length -	pointer to value of attribute data length
 *
 *	Returns:
 *		SUCCESS
 *		FETCH_INVALID_PARAM
 *		FETCH_ATTR_LENGTH_OVERFLOW
 *		FETCH_ATTR_ZERO_LENGTH
 *
 *	Author:
 *		David Bradsher
 *
 *********************************************************************/

 int
parse_attribute_id(unsigned char **attr_ptr, unsigned short *attr_RI,
	unsigned short *attr_tag, unsigned long *attr_len)
{

	unsigned char  *local_attr_ptr;	/* points to current attribute in
									 * object extension */
	unsigned short  temp;			/* used to unpack attribute ID field */
	unsigned long   calc_length;	/* used to unpack attribute length */

	/*
	 * Check for valid parameters
	 */
#ifdef MODIFIED_BY_SS
	//ASSERT_RET(attr_ptr && attr_RI && attr_tag && attr_len,
		//FETCH_INVALID_PARAM);
	assert(attr_ptr && attr_RI && attr_tag && attr_len,
		FETCH_INVALID_PARAM);
#endif
	/*
	 * Extract the RI and tag from the Attribute Identifier (octet #1).
	 * Check the validity of the input parameters before using any of the
	 * values or references.
	 */

	local_attr_ptr = *attr_ptr;
#ifdef MODIFIED_BY_SS
	//ASSERT_RET(local_attr_ptr,	FETCH_INVALID_PARAM);
	assert(local_attr_ptr, FETCH_INVALID_PARAM);
#endif 
	/*
	 * The attribute RI is in the MS 2 bits of the Attribute Identifier
	 * field while the attribute tag is in the LS 6 bits.
	 */

	temp = (unsigned short) *local_attr_ptr++;
	*attr_tag = temp & ATTR_TAG_MASK;
	*attr_RI = (temp >> REF_INDICATOR_SHIFT) & REF_INDICATOR_MASK;

	/*
	 * If bit 8 of the length byte is set, the length is encoded into the
	 * LS 7 bits of this byte and subsequent bytes.  Bit 8 of the last byte
	 * of the encoded sequence is 0.  The upper 7 bits of the calculated
	 * length are monitored during unpacking and a length overflow error is
	 * returned if these are nonzero.
	 */

	calc_length = 0L;
	do {
		if (calc_length & MAX_LENGTH_MASK) {
			return (FETCH_ATTR_LENGTH_OVERFLOW);
		}
		calc_length = (calc_length << LENGTH_SHIFT) |
			(unsigned long) (LENGTH_MASK & *local_attr_ptr);
	} while (LENGTH_ENCODE_MASK & *local_attr_ptr++);

	*attr_len = calc_length;
	if (!calc_length) {
		return (FETCH_ATTR_ZERO_LENGTH);
	}

	*attr_ptr = local_attr_ptr;
	return (SUCCESS);
}




/*********************************************************************
 *
 *	Name:	attach_var_data
 *
 *	ShortDesc:	Attach binary (attribute) to flat structure
 *
 *	Description: Posts the pointer for an item attribute to
 *				 its corresponding DEPBIN pointer in the flat
 *				 structure.
 *
 *	Inputs:
 *		attr_data_ptr - pointer to attribute data in the
 *						scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *	Returns:
 *		SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *	Author:
 *		David Bradsher
 *
 *********************************************************************/

int
attach_var_data(unsigned char *attr_data_ptr, unsigned long data_len,
 void *flats, unsigned short tag)
{

	FLAT_VAR       *var_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointers */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	var_flat = (FLAT_VAR *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer arrays.
	 * These must be reserved on the scratchpad before the DEPBIN
	 * structures for each attribute can be created. Return if there is not
	 * enough scratchpad memory to reserve the array.  For the Variables
	 * flat structure only, the sub-structures var_flat->misc and
	 * var_flat->actions must already exist.
	 */

	switch (tag) {

	case VAR_UNIT_ID:
/* removed 15oct12
	case VAR_READ_TIME_OUT_ID:
	case VAR_WRITE_TIME_OUT_ID:
	***/
	case VAR_WIDTHSIZE_ID:
	case VAR_HEIGHTSIZE_ID:

	case VAR_VALID_ID:
	case VAR_MIN_VAL_ID:
	case VAR_MAX_VAL_ID:
	case VAR_SCALE_ID:
	case VAR_DEBUG_ID:
	case VAR_TIME_FORMAT_ID:									// timj 4jan07 added
	case VAR_TIME_SCALE_ID:									// timj 4jan07 added
	//	ASSERT_RET(var_flat->misc, FETCH_NULL_POINTER);
		if(!var_flat->misc)
		{
			var_flat->misc =
					(FLAT_VAR_MISC*)new FLAT_VAR_MISC;

			/*
			 * Force the DEPBIN pointer array to all 0's
			 */

			(void)memset((char *) var_flat->misc, 0, sizeof(FLAT_VAR_MISC));
		}

		if (!var_flat->misc->depbin) {
			
			var_flat->misc->depbin =
					(VAR_MISC_DEPBIN*)new VAR_MISC_DEPBIN;

			/*
			 * Force the DEPBIN pointer array to all 0's
			 */

			(void)memset((char *) var_flat->misc->depbin, 0, sizeof(VAR_MISC_DEPBIN));
		}
		break;

	case VAR_PRE_READ_ACT_ID:
	case VAR_POST_READ_ACT_ID:
	case VAR_PRE_WRITE_ACT_ID:
	case VAR_POST_WRITE_ACT_ID:
	case VAR_PRE_EDIT_ACT_ID:
	case VAR_POST_EDIT_ACT_ID:
	case VAR_REFRESH_ACT_ID:
//#ifdef XMTR												// timj
	case VAR_POST_RQST_ACT_ID:
	case VAR_POST_USER_ACT_ID:
//#endif

	//	ASSERT_RET(var_flat->actions, FETCH_NULL_POINTER);
		if(!var_flat->actions)
		{
			var_flat->actions =
					(FLAT_VAR_ACTIONS*)new FLAT_VAR_ACTIONS;

			/*
			 * Force the actions pointer array to all 0's
			 */

			(void)memset((char *) var_flat->actions, 0, sizeof(FLAT_VAR_ACTIONS));
		}

		if (!var_flat->actions->depbin) {
			
			var_flat->actions->depbin =
				(VAR_ACTIONS_DEPBIN*)new VAR_ACTIONS_DEPBIN;		
		
			/*
			 * Force the DEPBIN pointer array to all 0's
			 */

			(void)memset((char *) var_flat->actions->depbin, 0, sizeof(VAR_ACTIONS_DEPBIN));
		}
		break;

	default:
		if (!(var_flat->depbin)) {

			var_flat->depbin =
				(VAR_DEPBIN *) new VAR_DEPBIN;
			
 			(void)memset((char *) var_flat->depbin, 0, sizeof(VAR_DEPBIN));
		}

	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case VAR_CLASS_ID:
		depbin_ptr = &var_flat->depbin->db_class;
		break;
	case VAR_HANDLING_ID:
		depbin_ptr = &var_flat->depbin->db_handling;
		break;
	case VAR_UNIT_ID:
		depbin_ptr = &var_flat->misc->depbin->db_unit;
		break;
	case VAR_LABEL_ID:
		depbin_ptr = &var_flat->depbin->db_label;
		break;
	case VAR_HELP_ID:
		depbin_ptr = &var_flat->depbin->db_help;
		break;
/*  removed 15oct12
	case VAR_READ_TIME_OUT_ID:
		depbin_ptr = &var_flat->misc->depbin->db_read_time_out;
		break;
	case VAR_WRITE_TIME_OUT_ID:
		depbin_ptr = &var_flat->misc->depbin->db_write_time_out;
		break;
***/
	case VAR_WIDTHSIZE_ID:
		depbin_ptr = &var_flat->misc->depbin->db_height;
		break;
	case VAR_HEIGHTSIZE_ID:
		depbin_ptr = &var_flat->misc->depbin->db_width;
		break;
	case VAR_VALID_ID:
		depbin_ptr = &var_flat->misc->depbin->db_valid;
		break;
	case VAR_PRE_READ_ACT_ID:
		depbin_ptr = &var_flat->actions->depbin->db_pre_read_act;
		break;
	case VAR_POST_READ_ACT_ID:
		depbin_ptr = &var_flat->actions->depbin->db_post_read_act;
		break;
	case VAR_PRE_WRITE_ACT_ID:
		depbin_ptr = &var_flat->actions->depbin->db_pre_write_act;
		break;
	case VAR_POST_WRITE_ACT_ID:
		depbin_ptr = &var_flat->actions->depbin->db_post_write_act;
		break;
	case VAR_PRE_EDIT_ACT_ID:
		depbin_ptr = &var_flat->actions->depbin->db_pre_edit_act;
		break;
	case VAR_POST_EDIT_ACT_ID:
		depbin_ptr = &var_flat->actions->depbin->db_post_edit_act;
		break;
	case VAR_REFRESH_ACT_ID:
		depbin_ptr = &var_flat->actions->depbin->db_refresh_act;
		break;
	case VAR_RESP_CODES_ID:
		depbin_ptr = &var_flat->depbin->db_resp_codes;
		break;
	case VAR_TYPE_SIZE_ID:
		depbin_ptr = &var_flat->depbin->db_type_size;
		break;
	case VAR_DISPLAY_ID:
		depbin_ptr = &var_flat->depbin->db_display;
		break;
	case VAR_EDIT_ID:
		depbin_ptr = &var_flat->depbin->db_edit;
		break;
	case VAR_MIN_VAL_ID:
		depbin_ptr = &var_flat->misc->depbin->db_min_val;
		break;
	case VAR_MAX_VAL_ID:
		depbin_ptr = &var_flat->misc->depbin->db_max_val;
		break;
	case VAR_SCALE_ID:
		depbin_ptr = &var_flat->misc->depbin->db_scale;
		break;
	case VAR_ENUMS_ID:
		depbin_ptr = &var_flat->depbin->db_enums;
		break;
	case VAR_INDEX_ITEM_ARRAY_ID:
		depbin_ptr = &var_flat->depbin->db_index_item_array;
		break;
	case VAR_DEFAULT_VALUE_ID:							//Vibhor 280904: Added
		depbin_ptr = &var_flat->depbin->db_default_value;
		break;
//#ifdef XMTR
	case VAR_POST_RQST_ACT_ID:			//stevev 21feb05: Added  // timj 26dec07 no longer required
		depbin_ptr = &var_flat->actions->depbin->db_post_rqst_act;
		break;
	case VAR_POST_USER_ACT_ID:							//stevev 21feb05: Added
		depbin_ptr = &var_flat->actions->depbin->db_post_user_act;
		break;
//#endif
	case VAR_DEBUG_ID:									// stevev 06may05: addedAdded
		depbin_ptr = &var_flat->misc->depbin->db_debug_info;
		break;
	case VAR_TIME_FORMAT_ID:									// timj 26dec07 added
		depbin_ptr = &var_flat->misc->depbin->db_time_format;
		break;
	case VAR_TIME_SCALE_ID:										// timj 26dec07 added
		depbin_ptr = &var_flat->misc->depbin->db_time_scale;
		break;
	default:
		if (tag >= MAX_VAR_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;

	}			/* end switch */

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		/*Put a check if malloc fails, return if yes!!*/
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));
		
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the .bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	var_flat->masks.bin_hooked |= (1L << tag);

	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:	attach_block_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_block_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_BLOCK     *block_flat;
	DEPBIN        **depbin_ptr;
//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	block_flat = (FLAT_BLOCK *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!block_flat->depbin) {
		
		block_flat->depbin =
			(BLOCK_DEPBIN *)new BLOCK_DEPBIN; 

		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) block_flat->depbin, 0, sizeof(BLOCK_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case BLOCK_CHARACTERISTIC_ID:
		depbin_ptr = &block_flat->depbin->db_characteristic;
		break;
	case BLOCK_LABEL_ID:
		depbin_ptr = &block_flat->depbin->db_label;
		break;
	case BLOCK_HELP_ID:
		depbin_ptr = &block_flat->depbin->db_help;
		break;
	case BLOCK_PARAM_ID:
		depbin_ptr = &block_flat->depbin->db_param;
		break;
	case BLOCK_MENU_ID:
		depbin_ptr = &block_flat->depbin->db_menu;
		break;
	case BLOCK_EDIT_DISP_ID:
		depbin_ptr = &block_flat->depbin->db_edit_disp;
		break;
	case BLOCK_METHOD_ID:
		depbin_ptr = &block_flat->depbin->db_method;
		break;
	case BLOCK_UNIT_ID:
		depbin_ptr = &block_flat->depbin->db_unit;
		break;
	case BLOCK_REFRESH_ID:
		depbin_ptr = &block_flat->depbin->db_refresh;
		break;
	case BLOCK_WAO_ID:
		depbin_ptr = &block_flat->depbin->db_wao;
		break;
	case BLOCK_COLLECT_ID:
		depbin_ptr = &block_flat->depbin->db_collect;
		break;
	case BLOCK_ITEM_ARRAY_ID:
		depbin_ptr = &block_flat->depbin->db_item_array;
		break;
	case BLOCK_PARAM_LIST_ID:
		depbin_ptr = &block_flat->depbin->db_param_list;
		break;
	default:
		return (FETCH_INVALID_ATTRIBUTE);

	}			/* end switch */

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		/*Put a check if malloc fails, return if yes!!*/
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));
		
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	block_flat->masks.bin_hooked |= (1L << tag);

	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_menu_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_menu_data(unsigned char *attr_data_ptr, unsigned long data_len,
	 void *flats, unsigned short tag)
{

	FLAT_MENU      *menu_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	menu_flat = (FLAT_MENU *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!menu_flat->depbin) {
		
		menu_flat->depbin =
			(MENU_DEPBIN *)new MENU_DEPBIN;
		

		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) menu_flat->depbin, 0, sizeof(MENU_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case MENU_LABEL_ID:
		depbin_ptr = &menu_flat->depbin->db_label;
		break;
	case MENU_ITEMS_ID:
		depbin_ptr = &menu_flat->depbin->db_items;
		break;

	case MENU_HELP_ID:
		depbin_ptr = &menu_flat->depbin->db_help;
		break;
	case MENU_VALID_ID:
		depbin_ptr = &menu_flat->depbin->db_valid;
		break;
	case MENU_STYLE_ID:
		depbin_ptr = &menu_flat->depbin->db_style;
		break;
	case MENU_DEBUG_ID:
		depbin_ptr = &menu_flat->depbin->db_debug_info;
		break;
	default:
		if ( tag >= MAX_MENU_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		/*
		 * Reserve the DEPBIN structure on the scratchpad and assign
		 * the pointer to the DEPBIN pointer array field.
		 */
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		/*Put a check if malloc fails, return if yes!!*/
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));
		
		
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	menu_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_edit_disp_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_edit_disp_data(unsigned char *attr_data_ptr, unsigned long data_len,
	 void *flats, unsigned short tag)
{

	FLAT_EDIT_DISPLAY *edit_disp_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	edit_disp_flat = (FLAT_EDIT_DISPLAY *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!edit_disp_flat->depbin) {
		
		edit_disp_flat->depbin =
			(EDIT_DISPLAY_DEPBIN *)new EDIT_DISPLAY_DEPBIN;

		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) edit_disp_flat->depbin, 0, sizeof(EDIT_DISPLAY_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case EDIT_DISPLAY_LABEL_ID:
		depbin_ptr = &edit_disp_flat->depbin->db_label;
		break;
	case EDIT_DISPLAY_EDIT_ITEMS_ID:
		depbin_ptr = &edit_disp_flat->depbin->db_edit_items;
		break;
	case EDIT_DISPLAY_DISP_ITEMS_ID:
		depbin_ptr = &edit_disp_flat->depbin->db_disp_items;
		break;
	case EDIT_DISPLAY_PRE_EDIT_ACT_ID:
		depbin_ptr = &edit_disp_flat->depbin->db_pre_edit_act;
		break;
	case EDIT_DISPLAY_POST_EDIT_ACT_ID:
		depbin_ptr = &edit_disp_flat->depbin->db_post_edit_act;
		break;

	case EDIT_DISPLAY_HELP_ID:
		depbin_ptr = &edit_disp_flat->depbin->db_help;
		break;
	case EDIT_DISPLAY_VALID_ID:
		depbin_ptr = &edit_disp_flat->depbin->db_valid;
		break;
	case EDIT_DISPLAY_DEBUG_ID:
		depbin_ptr = &edit_disp_flat->depbin->db_debug_info;
		break;
	default:
		if ( tag >= MAX_EDIT_DISPLAY_ID )
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;// try to be forward compatible

	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));
		
	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	edit_disp_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_method_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_method_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_METHOD    *method_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	method_flat = (FLAT_METHOD *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!method_flat->depbin) {
		
		method_flat->depbin =
			(METHOD_DEPBIN *)new METHOD_DEPBIN;
				/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) method_flat->depbin, 0, sizeof(METHOD_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case METHOD_CLASS_ID:
		depbin_ptr = &method_flat->depbin->db_class;
		break;
	case METHOD_LABEL_ID:
		depbin_ptr = &method_flat->depbin->db_label;
		break;
	case METHOD_HELP_ID:
		depbin_ptr = &method_flat->depbin->db_help;
		break;
	case METHOD_DEF_ID:
		depbin_ptr = &method_flat->depbin->db_def;
		break;
	case METHOD_VALID_ID:
		depbin_ptr = &method_flat->depbin->db_valid;
		break;
	case METHOD_SCOPE_ID:
		depbin_ptr = &method_flat->depbin->db_scope;
		break;
	case METHOD_TYPE_ID:
		depbin_ptr = &method_flat->depbin->db_type;
		break;
	case METHOD_PARAMS_ID:
		depbin_ptr = &method_flat->depbin->db_params;
		break;
	case METHOD_DEBUG_ID:
		depbin_ptr = &method_flat->depbin->db_debug_info;
		break;
	default:
		if ( tag >= MAX_METHOD_ID )
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;//eat it to be forward compatible

	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	method_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_refresh_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_refresh_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_REFRESH   *refresh_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	refresh_flat = (FLAT_REFRESH *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!refresh_flat->depbin) {
	
		refresh_flat->depbin =
			(REFRESH_DEPBIN *) new REFRESH_DEPBIN;
	
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) refresh_flat->depbin, 0, sizeof(REFRESH_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case REFRESH_ITEMS_ID:
		depbin_ptr = &refresh_flat->depbin->db_items;		
		break;
	case REFRESH_DEBUG_ID:
		depbin_ptr = &refresh_flat->depbin->db_debug_info;
		break;
	default:
		if ( tag >=  MAX_REFRESH_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;//eat it to be forward compatible

	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	refresh_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_unit_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_unit_data(unsigned char *attr_data_ptr, unsigned long data_len,
		void *flats, unsigned short tag)
{

	FLAT_UNIT      *unit_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	unit_flat = (FLAT_UNIT *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!unit_flat->depbin) {

		unit_flat->depbin =
			(UNIT_DEPBIN *)new UNIT_DEPBIN;
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) unit_flat->depbin, 0, sizeof(UNIT_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case UNIT_ITEMS_ID:
		depbin_ptr = &unit_flat->depbin->db_items;
		break;
	case UNIT_DEBUG_ID:
		depbin_ptr = &unit_flat->depbin->db_debug_info;
		break;
	default:
		if ( tag >=  MAX_UNIT_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;//eat it to be forward compatible

	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {
	
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	unit_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_wao_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_wao_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_WAO       *wao_flat;
	DEPBIN        **depbin_ptr;
//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	wao_flat = (FLAT_WAO *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!wao_flat->depbin) {

		wao_flat->depbin =
			(WAO_DEPBIN *) new WAO_DEPBIN;
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) wao_flat->depbin, 0, sizeof(WAO_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case WAO_ITEMS_ID:
		depbin_ptr = &wao_flat->depbin->db_items;
		break;
	case WAO_DEBUG_ID:
		depbin_ptr = &wao_flat->depbin->db_debug_info;
		break;
	default:
		return (FETCH_INVALID_ATTRIBUTE);
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	wao_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_item_array_data
 *
 *  ShortDesc:	Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_item_array_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_ITEM_ARRAY *item_array_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	item_array_flat = (FLAT_ITEM_ARRAY *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!item_array_flat->depbin) {

		item_array_flat->depbin =
			(ITEM_ARRAY_DEPBIN *)new ITEM_ARRAY_DEPBIN;
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) item_array_flat->depbin, 0, sizeof(ITEM_ARRAY_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case ITEM_ARRAY_ELEMENTS_ID:
		depbin_ptr = &item_array_flat->depbin->db_elements;
		break;
	case ITEM_ARRAY_LABEL_ID:
		depbin_ptr = &item_array_flat->depbin->db_label;
		break;
	case ITEM_ARRAY_HELP_ID:
		depbin_ptr = &item_array_flat->depbin->db_help;
		break;
	case ITEM_ARRAY_VALIDITY_ID:
		depbin_ptr = &item_array_flat->depbin->db_valid;
		break;
	case ITEM_ARRAY_DEBUG_ID:
		depbin_ptr = &item_array_flat->depbin->db_debug_info;
		break;
	default:
		if ( tag >=  MAX_ITEM_ARRAY_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;//eat it to be forward compatible
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {
	
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	item_array_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_collection_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_collection_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_COLLECTION *collection_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	collection_flat = (FLAT_COLLECTION *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!collection_flat->depbin) {

		collection_flat->depbin =
			(COLLECTION_DEPBIN *)new COLLECTION_DEPBIN;
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) collection_flat->depbin, 0, sizeof(COLLECTION_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case COLLECTION_MEMBERS_ID:
		depbin_ptr = &collection_flat->depbin->db_members;
		break;
	case COLLECTION_LABEL_ID:
		depbin_ptr = &collection_flat->depbin->db_label;
		break;
	case COLLECTION_HELP_ID:
		depbin_ptr = &collection_flat->depbin->db_help;
		break;
	case COLLECTION_VALID_ID:
		depbin_ptr = &collection_flat->depbin->db_valid;
		break;
	case COLLECTION_DEBUG_ID:
		depbin_ptr = &collection_flat->depbin->db_debug_info;
		break;
	default:
		if ( tag >=  MAX_COLLECTION_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;//eat it to be forward compatible
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	collection_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_program_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_program_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_PROGRAM   *program_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	program_flat = (FLAT_PROGRAM *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!program_flat->depbin) {

		program_flat->depbin =
			(PROGRAM_DEPBIN *)new PROGRAM_DEPBIN;
		
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) program_flat->depbin, 0, sizeof(PROGRAM_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case PROGRAM_ARGS_ID:
		depbin_ptr = &program_flat->depbin->db_args;
		break;
	case PROGRAM_RESP_CODES_ID:
		depbin_ptr = &program_flat->depbin->db_resp_codes;
		break;
	default:
		return (FETCH_INVALID_ATTRIBUTE);
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {
		
		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	program_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_record_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_record_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_RECORD    *record_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	record_flat = (FLAT_RECORD *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!record_flat->depbin) {

		record_flat->depbin =
			(RECORD_DEPBIN *) new RECORD_DEPBIN;

		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) record_flat->depbin, 0, sizeof(RECORD_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case RECORD_MEMBERS_ID:
		depbin_ptr = &record_flat->depbin->db_members;
		break;
	case RECORD_LABEL_ID:
		depbin_ptr = &record_flat->depbin->db_label;
		break;
	case RECORD_HELP_ID:
		depbin_ptr = &record_flat->depbin->db_help;
		break;
	case RECORD_RESP_CODES_ID:
		depbin_ptr = &record_flat->depbin->db_resp_codes;
		break;
	default:
		return (FETCH_INVALID_ATTRIBUTE);
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	record_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_array_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_array_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_ARRAY     *array_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	array_flat = (FLAT_ARRAY *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!array_flat->depbin) {

		array_flat->depbin =
			(ARRAY_DEPBIN *) new ARRAY_DEPBIN;
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) array_flat->depbin, 0, sizeof(ARRAY_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case ARRAY_LABEL_ID:
		depbin_ptr = &array_flat->depbin->db_label;
		break;
	case ARRAY_HELP_ID:
		depbin_ptr = &array_flat->depbin->db_help;
		break;
	case ARRAY_VALID_ID:							//Vibhor 280904: Added	
		depbin_ptr = &array_flat->depbin->db_valid;
		break;
	case ARRAY_TYPE_ID:
		depbin_ptr = &array_flat->depbin->db_type;
		break;
	case ARRAY_NUM_OF_ELEMENTS_ID:
		depbin_ptr = &array_flat->depbin->db_num_of_elements;
		break;
	case ARRAY_DEBUG_ID:
		depbin_ptr = &array_flat->depbin->db_debug_info;
		break;
	
//	case ARRAY_RESP_CODES_ID:
//		depbin_ptr = &array_flat->depbin->db_resp_codes;
//		break;
		
	default:
		if ( tag >=  MAX_ARRAY_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;//eat it to be forward compatible
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	array_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_var_list_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_var_list_data(unsigned char *attr_data_ptr, unsigned long data_len,
		void *flats, unsigned short tag)
{

	FLAT_VAR_LIST  *var_list_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	var_list_flat = (FLAT_VAR_LIST *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!var_list_flat->depbin) {

		var_list_flat->depbin = 
			(VAR_LIST_DEPBIN *)new VAR_LIST_DEPBIN;
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) var_list_flat->depbin, 0, sizeof(VAR_LIST_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case VAR_LIST_MEMBERS_ID:
		depbin_ptr = &var_list_flat->depbin->db_members;
		break;
	case VAR_LIST_LABEL_ID:
		depbin_ptr = &var_list_flat->depbin->db_label;
		break;
	case VAR_LIST_HELP_ID:
		depbin_ptr = &var_list_flat->depbin->db_help;
		break;
	case VAR_LIST_RESP_CODES_ID:
		depbin_ptr = &var_list_flat->depbin->db_resp_codes;
		break;
	default:
		return (FETCH_INVALID_ATTRIBUTE);
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	var_list_flat->masks.bin_hooked |= (1L << tag);
	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_resp_codes_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      David Bradsher
 *
 *********************************************************************/

 int
attach_resp_codes_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_RESP_CODE *resp_codes_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	resp_codes_flat = (FLAT_RESP_CODE *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!resp_codes_flat->depbin) {

		/*
		 * Reserve the DEPBIN pointer array on the scratchpad and
		 * assign the pointer to the flat structure.
		 */

		resp_codes_flat->depbin =
			(RESP_CODE_DEPBIN *) new RESP_CODE_DEPBIN;
		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) resp_codes_flat->depbin, 0, sizeof(RESP_CODE_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case RESP_CODE_MEMBER_ID:
		depbin_ptr = &resp_codes_flat->depbin->db_member;
		break;
	default:
		return (FETCH_INVALID_ATTRIBUTE);
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	resp_codes_flat->masks.bin_hooked |= (1L << tag);

	return (SUCCESS);
}


/*********************************************************************
 *
 *  Name:   attach_command_data
 *
 *  ShortDesc:  Attach binary (attribute) to flat structure
 *
 *  Description: Posts the pointer for an item attribute to
 *               its corresponding DEPBIN pointer in the flat
 *               structure.
 *
 *  Inputs:
 *      attr_data_ptr - pointer to attribute data in the
 *                      scratchpad
 *		data_len -		length of the attribute data
 *      tag - 			the attribute type
 *
 *  Outputs:
 *      flats -			pointer to union of flat structures for the
 *                  	items with DEPBIN pointer attached
 *
 *  Returns:
 *      SUCCESS
 *		FETCH_INVALID_ATTRIBUTE
 *
 *  Author:
 *      Chris Gustafson
 *
 *********************************************************************/

 int
attach_command_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag)
{

	FLAT_COMMAND *command_flat;
	DEPBIN        **depbin_ptr;

//	int             rcode;

	depbin_ptr = (DEPBIN **) 0L;	/* Initialize the DEPBIN pointer */

	/*
	 * Assign the appropriate flat structure pointer
	 */

	command_flat = (FLAT_COMMAND *) flats;

	/*
	 * Check the flat structure for existence of the DEPBIN pointer array.
	 * This must be reserved on the scratchpad before the DEPBIN structures
	 * for each attribute can be created. Return if there is not enough
	 * scratchpad memory to reserve the array.
	 */

	if (!command_flat->depbin) {

		command_flat->depbin =
			(COMMAND_DEPBIN *) new COMMAND_DEPBIN;

		/*
		 * Force the DEPBIN pointer array to all 0's
		 */

		(void)memset((char *) command_flat->depbin, 0, sizeof(COMMAND_DEPBIN));
	}

	/*
	 * Select the type of attribute and attach the address in the scratchpad
	 * to the corresponding DEPBIN pointer in the flat structure.  If the
	 * DEPBIN structure pointer is null, reserve the space for it on the
	 * scratchpad and set the pointer in the DEPBIN array.
	 */

	switch (tag) {

	case COMMAND_NUMBER_ID:
		depbin_ptr = &command_flat->depbin->db_number;
		break;
	case COMMAND_OPER_ID:
		depbin_ptr = &command_flat->depbin->db_oper;
		break;
	case COMMAND_TRANS_ID:
		depbin_ptr = &command_flat->depbin->db_trans;
		break;
	case COMMAND_RESP_CODES_ID:
		depbin_ptr = &command_flat->depbin->db_resp_codes;
		break;

	case COMMAND_DEBUG_ID:
		depbin_ptr = &command_flat->depbin->db_debug_info;
		break;

	default:
		if ( tag >=  MAX_COMMAND_ID)
			return (FETCH_INVALID_ATTRIBUTE);
		else
			return SUCCESS;//eat it to be forward compatible
	}

	/*
	 * Attach the data and the data length to the DEPBIN structure. It the
	 * structure does not yet exist, reserve it on the scratchpad first.
	 */

	if (!(*depbin_ptr)) {

		*depbin_ptr = (DEPBIN*)new DEPBIN;
		(void*)memset((char*)*depbin_ptr, 0, sizeof(DEPBIN));

	}
	(*depbin_ptr)->bin_chunk = attr_data_ptr;
	(*depbin_ptr)->bin_size = data_len;

	/*
	 * Set the bin_hooked bit in the flat structure for the appropriate
	 * attribute
	 */

	command_flat->masks.bin_hooked |= (1L << tag);

	return (SUCCESS);
}


/*********************************************************************
 *
 *	Name: 	get_item_attr
 *
 *	ShortDesc: 	Get attributes for Domain item
 *
 *	Description:
 *		Determines the data type for each requested attribute in the
 *		object extension and passes the pointer to the data to the
 *		attribute attachment routine.
 *
 *  Inputs:
 *      rhandle -       handle for the ROD containing the object
 *      obj -           pointer to the object structure, returned by
 *						get_rod_object(), containing pointers to the
 *						object extension and local data area
 *      req_mask -		attributes requested for the Item, indicated
 *						by individual bits set for each attribute
 *						(attributes depend on the Item type).  A bit
 *						for a requested attribute is reset after the
 *						binary has been attached.
 *		obj_item_mask -	the item mask field for the current object
 *						extension or, in the case of an externally
 *						referenced object, the item mask of the object
 *						extension which references it.
 *		extn_attr_length  -	length (in octets) of the attribute data.
 *		obj_ext_ptr -	pointer to the first attribute in the object
 *						extension
 *		itype -			the type of the requested item
 *		attach_attr_fn -	a pointer to the appropriate attribute
 *						attach functions for the item type
 *		item_bin_hooked -	the field in the item's flat structure
 *						whose bits indicate which attributes have
 *						already been attached.
 *
 *  Outputs:
 *      scrpad -        pointer to a structure whose members point
 *                      to a scratchpad memory area containing
 *                      the object requested and any external objects
 *                      and data referenced by the object.
 *		req_mask -     	remaining attributes requested for the Item. As
 *                      an attribute is attached, the bit in the mask
 *                      corresponding to the attribute is reset.
 *      flats -    		pointer to the Item attributes in the
 *						scratchpad memory (contents of structure
 *						depend on the Item type)
 *
 *	Returns:
 *		SUCCESS
 *		FETCH_INVALID_PARAM
 *		FETCH_INVALID_RI
 *		FETCH_ATTRIBUTE_NO_MASK_BIT
 *		FETCH_INSUFFICIENT_SCRATCHPAD
 *
 *	Author:
 *		David Bradsher

    NOTES: Vibhor 290904: In the new tokenizer (HART 6), attr_offset is being encoded & read as
	a multibyte encoded variable. Since JIT parser has to parse both HART 5 and HART 6 DDs,
	there is substantial code change below in CASE RI_LOCAL with 2 different branches 
	for HART 5 and HART 6 respectively.
 *
 *********************************************************************/

 int
get_item_attr(OBJECT_INDEX obj_index,unsigned long obj_item_mask, void *flats,
	unsigned char extn_attr_length, unsigned char *obj_ext_ptr,
	ITEM_TYPE itype, unsigned long item_bin_hooked, ATTR_FN attach_attr_fn)
{

	unsigned char  *local_data_ptr; 
	unsigned char  *obj_attr_ptr;	/* pointer to attributes in object
									 * extension */
	unsigned char  *extern_obj_attr_ptr; /*pointer to attributes in external oject extn */
	unsigned char	extern_extn_attr_length; /*length of Extn data in external obj*/
	unsigned short  curr_attr_RI;	/* RI for current attribute */
	unsigned short  curr_attr_tag;	/* tag for current attribute */
	unsigned long   curr_attr_length;	/* data length for current attribute */
	unsigned long   local_req_mask;	/* request mask for base or external
									 * objects */
	unsigned long   attr_mask_bit;	/* bit in item mask corresponding to
									 * current attribute */
	unsigned long   extern_attr_mask; 	/* used for recursive call for
										 * External All objects */
	unsigned short  extern_obj_index; 	/* attribute data field for
										 * object index of External object */
	unsigned long  attr_offset; 	/* attribute data field for offset of //Vibhor 270904: Changed to long, see commments below
									 * data in local data area */
	int             rcode;
/*	BYTE byMaskSize=0; */

	/*
	 * Check the validity of the pointer parameters
	 */
#ifdef MODIFIED_BY_SS
	//ASSERT_RET(obj_ext_ptr, FETCH_INVALID_PARAM);
	assert(obj_ext_ptr, FETCH_INVALID_PARAM);
#endif

/*
	 * Point to the first attribute in the object extension and begin
	 * extracting the attribute data
	 */

	local_req_mask = obj_item_mask;
	obj_attr_ptr = obj_ext_ptr;

	while ((obj_attr_ptr < (obj_ext_ptr + extn_attr_length)) &&
		local_req_mask) {

		/*
		 * Retrieve the Attribute Identifier information from the
		 * leading attribute bytes.  The object extension pointer will
		 * point to the first byte after the Attribute ID, which will
		 * be Immediate data or will reference Local or External data.
		 */

		rcode = parse_attribute_id(&obj_attr_ptr, &curr_attr_RI,
			&curr_attr_tag, &curr_attr_length);

		if (rcode != SUCCESS) {
			return (rcode);
		}

		/*
		 * Confirm that the current attribute being is matched by a set
		 * bit in the Item Mask.  The mask or the attribute tag is
		 * incorrect if there is not a match.
		 */

		attr_mask_bit = (unsigned long) (1L << curr_attr_tag);
		if (!(obj_item_mask & attr_mask_bit)) {
			return (FETCH_ATTRIBUTE_NO_MASK_BIT);
		}

		/*
		 * Use the Tag field from the Attribute Identifier to determine
		 * if the attribute was requested or not (not applicable to
		 * External All data type).  For all data types except External
		 * All, skip to the next attribute in the extension if not
		 * requested.  Also, skip forward if the attribute has already
		 * been attached.
		 */

		switch (curr_attr_RI) {

		case RI_IMMEDIATE:
			if (local_req_mask & attr_mask_bit) {
				if (!(item_bin_hooked & attr_mask_bit)) {

					rcode = (*attach_attr_fn) (obj_attr_ptr, curr_attr_length,
						 flats, curr_attr_tag);

					if (rcode != SUCCESS) {
						return rcode;
					}
					else {
						local_req_mask &= ~attr_mask_bit;	/* clear bit */
					}
				}
				else {
					local_req_mask &= ~attr_mask_bit;	/* clear bit */
				}
			}
			obj_attr_ptr += curr_attr_length;

			/*
			 * Check for invalid length that would advance the
			 * object extension pointer past the end of the object
			 * extension
			 */

			if (obj_attr_ptr > (obj_ext_ptr + extn_attr_length)) {
				return (FETCH_INVALID_ATTR_LENGTH);
			}
			break;

		case RI_LOCAL:
/*This is reached in 2 cases:
			1- You access an attribute data thru an external object 
			   ie. you come here as a consequence of a recursive call to get_item_attr 
			   thru RI 2 or 3
			2- Some base object has the RI as 1 directly
*/

			if (local_req_mask & attr_mask_bit) {
				if (!(item_bin_hooked & attr_mask_bit)) {

/*Vibhor 270904: Start of Code*/

					/* attr_offset: used to be a ushort , now a ulong.
					 In new tokenizer this is being encoded as a variable length integer
					 so we'll parse this stuff accordingly.
					*/
					if(false == bTokRev6Flag) // HART 5
					{
						attr_offset =
							(((unsigned short) obj_attr_ptr[0]) << 8) |
							obj_attr_ptr[1];
						
						obj_attr_ptr += LOCAL_REF_SIZE; //Increment the object attribute pointer

					}
					else					//HART 6
					{
						/* Read encoded int */
						
						attr_offset = 0L;
						do {
							if (attr_offset & MAX_LENGTH_MASK) { 
								return (FETCH_ATTR_LENGTH_OVERFLOW);
							}
							attr_offset = (attr_offset << LENGTH_SHIFT) |	      
								(unsigned long) (LENGTH_MASK & *obj_attr_ptr);
						} while (LENGTH_ENCODE_MASK & *obj_attr_ptr++);

						/* end  Read encoded int */

					}
						
/*Vibhor 270904: End of Code*/
						unsigned int i;
						
						for(i = 0; i < uSODLength ; i++)
						{
							if(ObjectFixed[i].index == obj_index)
								break;
						}
						
						if((ObjectFixed[i].wDomainDataSize < (curr_attr_length + attr_offset)) &&
							(ObjectFixed[i].wDomainDataSize != 0xffff))		// allow long attrs through #2500
							return (int)false; /*Data out of range*/
						
						local_data_ptr = (unsigned char*)pbyObjectValue[i] + attr_offset;
							
						rcode = (*attach_attr_fn) (local_data_ptr, curr_attr_length,
						 flats, curr_attr_tag);

						if (rcode != SUCCESS)
							return rcode;

				}//endif item_bin_hooked...
				else {
					local_req_mask &= ~attr_mask_bit;	/* clear bit */
					if(false == bTokRev6Flag) // HART 5  //For HART 6 its incremented automatically
					{
						obj_attr_ptr += LOCAL_REF_SIZE;
					}
					else
					{
						//read and discard the encoded integer, mainly to advance the object attribute pointer
						attr_offset = 0L;
							do {
								if (attr_offset & MAX_LENGTH_MASK) { 
									return (FETCH_ATTR_LENGTH_OVERFLOW);
								}
								attr_offset = (attr_offset << LENGTH_SHIFT) |	      
									(unsigned long) (LENGTH_MASK & *obj_attr_ptr);
							} while (LENGTH_ENCODE_MASK & *obj_attr_ptr++);
					}
				
				}
			}//endif local_req_mask...
			else
			{
				if(false == bTokRev6Flag) // HART 5  //For HART 6 its incremented automatically
				{
					obj_attr_ptr += LOCAL_REF_SIZE;
				}
				else
				{
					//read and discard the encoded integer, mainly to advance the object attribute pointer
					attr_offset = 0L;
						do {
							if (attr_offset & MAX_LENGTH_MASK) { 
								return (FETCH_ATTR_LENGTH_OVERFLOW);
							}
							attr_offset = (attr_offset << LENGTH_SHIFT) |	      
								(unsigned long) (LENGTH_MASK & *obj_attr_ptr);
						} while (LENGTH_ENCODE_MASK & *obj_attr_ptr++);
				}
			}

			break;

		case RI_EXTERNAL_SINGLE:
			if (local_req_mask & attr_mask_bit) {
				if (!(item_bin_hooked & attr_mask_bit)) {

						/*
						 * Set a request mask with a single attribute
						 */

						extern_attr_mask = attr_mask_bit;

						extern_obj_index =
						(((unsigned short) obj_attr_ptr[0]) << 8) |
						obj_attr_ptr[1];
						
						unsigned int i;
						/*Locate the external object */
						for(i = 0; i < uSODLength ; i++)
						{
							if(ObjectFixed[i].index == extern_obj_index)
								break;
						}
						/*Get extension length & the attribute offset*/
						
/*						switch(((ITEM_EXTN*)pbyExtensions[i])->byItemType)
						{
						case VARIABLE_ITYPE:
							byMaskSize = VAR_ATTR_MASK_SIZE;
							break;
						case BLOCK_ITYPE:
							byMaskSize = VAR_ATTR_MASK_SIZE;
							break;
						default:
							byMaskSize = MIN_ATTR_MASK_SIZE;

						} 
*/ /* Not required as the external object won't have any ID or a mask*/

						extern_extn_attr_length = byExtLengths[i] 
										- sizeof(ITEM_EXTN) + EXTEN_LENGTH_SIZE ;
										//-byMaskSize;
										
						extern_obj_attr_ptr = (unsigned char*)pbyExtensions[i] 
													+ sizeof(ITEM_EXTN);
										//			+ byMaskSize;

													

						/*Now make a recursive call to get the get_item_attr
						  to get the attribute for this object*/

						rcode = get_item_attr(extern_obj_index,obj_item_mask, flats,
									extern_extn_attr_length, extern_obj_attr_ptr,
								itype, item_bin_hooked, attach_attr_fn);

						if(rcode != SUCCESS)
							return(rcode);


			
			

				}
				local_req_mask &= ~attr_mask_bit;
			}
			obj_attr_ptr += EXTERNAL_REF_SIZE;
			break;

		case RI_EXTERNAL_ALL:
			
			extern_attr_mask = attr_mask_bit;

			extern_obj_index =
				(((unsigned short) obj_attr_ptr[0]) << 8) |
					obj_attr_ptr[1];
				
			unsigned int i;
			/*Locate the external object */
			for(i = 0; i < uSODLength ; i++)
			{
				if(ObjectFixed[i].index == extern_obj_index)
					break;
			}
			/*Get extension length & the attribute offset*/
/*			switch(((ITEM_EXTN*)pbyExtensions[i])->byItemType)
				{
				case VARIABLE_ITYPE:
					byMaskSize = VAR_ATTR_MASK_SIZE;
					break;
				case BLOCK_ITYPE:
					byMaskSize = VAR_ATTR_MASK_SIZE;
					break;
				default:
					byMaskSize = MIN_ATTR_MASK_SIZE;
				}
*/ /* Not required as the external object won't have any ID or a mask*/
				extern_extn_attr_length = byExtLengths[i] 
								- sizeof(ITEM_EXTN) + EXTEN_LENGTH_SIZE ;
							
				extern_obj_attr_ptr = (unsigned char*)pbyExtensions[i] 
													+ sizeof(ITEM_EXTN);
											
			/*Now make a recursive call to get the get_item_attr
			  to get the attribute for this object*/
			rcode = get_item_attr(extern_obj_index,obj_item_mask, flats,
						extern_extn_attr_length, extern_obj_attr_ptr,
					itype, item_bin_hooked, attach_attr_fn);


			/*
			 * Ignore FETCH_ATTRIBUTE_NOT_FOUND errors in this
			 * context since the request mask may point to
			 * attributes not contained in the external object.
			 */

			if ((rcode != SUCCESS) &&
				(rcode != FETCH_ATTRIBUTE_NOT_FOUND)) 
				return rcode;
			/*
			 * Reduce attribute count by number of
			 * attributes attached from External object
			 */

			local_req_mask &= ~extern_attr_mask;	/* clear attached bits */
		

			obj_attr_ptr += EXTERNAL_REF_SIZE;
			break;

		default:
			return (FETCH_INVALID_RI);
		}		/* end switch */

	}			/* end while */

	return (SUCCESS);


}


/*Vibhor 05082003*/

/***************************************************************************
** FUNCTION NAME: fetch_item 
**
** PURPOSE:
**   This function provides a generic interface to "fetch" the binaries of an
**   object and attach them to the supplied flat str for subsequent evaluation 
**   eval_item function
**
** PARAMETER:
**
**   INPUT ...
**     pbyObjExtn : a pointer to the Extension part of the object
**     item_type  : the type of the DDL item
**     flats	  : a pointer to the flat structure provided by the caller 
**
**   OUTPUT ...
**	   flats      : attached  with the binaries of the object
**
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   SUCCESS if Successful,
**	 Return codes from get_item_attr function   
**
****************************************************************************/


int fetch_item( BYTE *pbyObjExtn, OBJECT_INDEX objIndex, ITEM_TYPE itemType,unsigned long *pulItemMask, void *flats)
{

	BYTE *pbyItemExtn;
	BYTE byMaskSize;
//	BYTE byItemType;
//	BYTE byItemSubType;
	unsigned long ulItemId;
	unsigned long ulItemMask;
	//BYTE bySizeOfExtnMasks;
	bool byRetVal;
	int iRetVal;
	BYTE *pbyAttribOffset;
	BYTE *pbyLocalAttribOffset;
	BYTE byAttrsLength;
	BYTE byExtLength;
	
	pbyItemExtn =pbyObjExtn;
	switch(itemType)
	{
	case VARIABLE_ITYPE:
		
		byMaskSize = VAR_ATTR_MASK_SIZE;
		break;
	
	case BLOCK_ITYPE:
		byMaskSize = BLOCK_ATTR_MASK_SIZE;
		break;

	default:
		byMaskSize = MIN_ATTR_MASK_SIZE;
		break;

	}/*End switch item_type*/
	byExtLength = ((ITEM_EXTN*)pbyItemExtn)->byLength;
	
	 byRetVal= read_dword(&ulItemId, &(((ITEM_EXTN*)pbyItemExtn)->byItemId[0]),FORMAT_BIG_ENDIAN);
	 if(!byRetVal)
		  return (DDL_ENCODING_ERROR);
	 if (0L == ulItemId) {/* No mask for External objects */
		 ulItemMask = 0L; /*Vibhor: This thing needs to be taken care of!!!*/
		/*I think we need to break/continue from here because there would
		  be no attributes!!!*/
	 /*pbyAttribOffset = pbyItemExtn +  sizeof(ITEM_EXTN);*/
	/*Need to return from this*/			
		 return FETCH_EXTERNAL_OBJECT;
	}
	else {
			pbyItemExtn += sizeof(ITEM_EXTN); /*Point to the masks */
			ulItemMask = 0L;
			for (int j = 0; j < byMaskSize; j++)
				{
				    ulItemMask = (ulItemMask << 8) |
						(unsigned long)*(pbyItemExtn++);
				}
		pbyAttribOffset = pbyItemExtn ;/*+  bySizeOfExtnMasks; */
		byAttrsLength = byExtLength - sizeof(ITEM_EXTN) + EXTEN_LENGTH_SIZE -byMaskSize; 
		}
	
	/*If the mask value is zero,means the DD is corrupt!! so return false & quit*/
	if(!ulItemMask) 
	{
		return DDL_ENCODING_ERROR;
	} 
	

	pbyLocalAttribOffset = pbyAttribOffset;
	
	*pulItemMask = ulItemMask;
	switch (itemType) {

	case VARIABLE_ITYPE:
				((FLAT_VAR *) flats)->masks.bin_exists =
							ulItemMask & VAR_ATTR_MASKS;
				((FLAT_VAR *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							VARIABLE_ITYPE,((FLAT_VAR*)flats)->masks.bin_hooked, attach_var_data);
			
		break;

	case MENU_ITYPE:
				((FLAT_MENU *) flats)->masks.bin_exists =
							ulItemMask & MENU_ATTR_MASKS;/* sjv note: this was VAR_ATTR_MASKS */
				((FLAT_MENU *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							MENU_ITYPE,((FLAT_MENU*)flats)->masks.bin_hooked, attach_menu_data);	
		break;

	case EDIT_DISP_ITYPE:
				((FLAT_EDIT_DISPLAY *) flats)->masks.bin_exists =
							ulItemMask & EDIT_DISP_ATTR_MASKS;
				((FLAT_EDIT_DISPLAY *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							EDIT_DISP_ITYPE,((FLAT_EDIT_DISPLAY*)flats)->masks.bin_hooked, attach_edit_disp_data);
		break;

	case METHOD_ITYPE:
				((FLAT_METHOD *) flats)->masks.bin_exists =
							ulItemMask & METHOD_ATTR_MASKS;
				((FLAT_METHOD *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							METHOD_ITYPE,((FLAT_METHOD*)flats)->masks.bin_hooked, attach_method_data);
		break;

	case REFRESH_ITYPE:
				((FLAT_REFRESH *) flats)->masks.bin_exists =
							ulItemMask & REFRESH_ATTR_MASKS;
				((FLAT_REFRESH *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							REFRESH_ITYPE,((FLAT_REFRESH*)flats)->masks.bin_hooked, attach_refresh_data);
		break;

	case UNIT_ITYPE:
				((FLAT_UNIT *) flats)->masks.bin_exists =
							ulItemMask & UNIT_ATTR_MASKS;
				((FLAT_UNIT *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							UNIT_ITYPE,((FLAT_UNIT*)flats)->masks.bin_hooked, attach_unit_data);
		break;

	case WAO_ITYPE:
				((FLAT_WAO *) flats)->masks.bin_exists =
							ulItemMask & WAO_ATTR_MASKS;
				((FLAT_WAO *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							WAO_ITYPE,((FLAT_WAO*)flats)->masks.bin_hooked, attach_wao_data);

		break;

	case ITEM_ARRAY_ITYPE:
				((FLAT_ITEM_ARRAY *) flats)->masks.bin_exists =
							ulItemMask & ITEM_ARRAY_ATTR_MASKS;
				((FLAT_ITEM_ARRAY *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							ITEM_ARRAY_ITYPE,((FLAT_ITEM_ARRAY*)flats)->masks.bin_hooked, attach_item_array_data);
		break;

	case COLLECTION_ITYPE:
				((FLAT_COLLECTION *) flats)->masks.bin_exists =
							ulItemMask & COLLECTION_ATTR_MASKS;
				((FLAT_ITEM_ARRAY *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							COLLECTION_ITYPE,((FLAT_COLLECTION*)flats)->masks.bin_hooked, attach_collection_data);
		break;

	case BLOCK_ITYPE:
				((FLAT_BLOCK *) flats)->masks.bin_exists =
							ulItemMask & BLOCK_ATTR_MASKS;
				((FLAT_BLOCK *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							BLOCK_ITYPE,((FLAT_BLOCK*)flats)->masks.bin_hooked, attach_block_data);

		break;



	case RECORD_ITYPE:
				((FLAT_RECORD *) flats)->masks.bin_exists =
							ulItemMask & RECORD_ATTR_MASKS;
				((FLAT_RECORD *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							RECORD_ITYPE,((FLAT_RECORD*)flats)->masks.bin_hooked, attach_record_data);

		break;

	case COMMAND_ITYPE:
				((FLAT_COMMAND *) flats)->masks.bin_exists =
							ulItemMask & COMMAND_ATTR_MASKS;
				((FLAT_COMMAND *) flats)->id = ulItemId;
				iRetVal =  get_item_attr(objIndex,ulItemMask, flats,
								byAttrsLength, pbyLocalAttribOffset,
							COMMAND_ITYPE,((FLAT_COMMAND*)flats)->masks.bin_hooked, attach_command_data);
		break;


	case PROGRAM_ITYPE:

	case ARRAY_ITYPE:

	case VAR_LIST_ITYPE:

	case RESP_CODES_ITYPE:

	case DOMAIN_ITYPE:
		
	default:
		return FETCH_INVALID_ITEM_TYPE;
	}			/* end switch */

return iRetVal;

}
