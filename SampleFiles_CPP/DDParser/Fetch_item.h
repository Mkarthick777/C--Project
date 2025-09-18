

#ifndef FETCHITEM_H
#define FETCHITEM_H

//sjv06feb06 #include	<windows.h>

#include	<limits.h>
//#include    <stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include    "panic.h"
#include	"retn_code.h"
// moved to common as tags_sa.h    #include "DDLTags.h"
#include    "Tags_sa.h"
#include	"DDLDefs.h"
#include	"Flats.h"
#include	"DD_Header.h"
#include    "ParserEndian.h"
#include    "DDlConditional.h"// stevev 24aug06


/*Prototypes for Fetch_item.cpp*/




#define	MAX_LENGTH_MASK		~(ULONG_MAX >> LENGTH_SHIFT)
#define	LENGTH_ENCODE_MASK	~LENGTH_MASK

typedef int (*ATTR_FN)(unsigned char *attr_data_ptr, unsigned long data_len,
		void *flats, unsigned short tag);


unsigned short count_bits(unsigned long bfield);

int parse_attribute_id(unsigned char **attr_ptr, unsigned short *attr_RI,
	unsigned short *attr_tag, unsigned long *attr_len);


int attach_var_data(unsigned char *attr_data_ptr, unsigned long data_len,
 void *flats, unsigned short tag);

int attach_block_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);


int attach_menu_data(unsigned char *attr_data_ptr, unsigned long data_len,
	 void *flats, unsigned short tag);

int attach_edit_disp_data(unsigned char *attr_data_ptr, unsigned long data_len,
	 void *flats, unsigned short tag);

int attach_method_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_refresh_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_unit_data(unsigned char *attr_data_ptr, unsigned long data_len,
		void *flats, unsigned short tag);

int attach_wao_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_item_array_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_collection_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_program_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_record_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_array_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_var_list_data(unsigned char *attr_data_ptr, unsigned long data_len,
		void *flats, unsigned short tag);

int attach_resp_codes_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_command_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_domain_data(unsigned char *attr_data_ptr, unsigned long data_len,
		void *flats, unsigned short tag);

int get_item_attr(OBJECT_INDEX obj_index, unsigned long obj_item_mask, void *flats,
	unsigned char extn_attr_length, unsigned char *obj_ext_ptr,
	ITEM_TYPE itype, unsigned long item_bin_hooked, ATTR_FN attach_attr_fn);

/*Added by Vibhor : 05082003 : The following fn provides a generic interface to get 
  the binaries of an object and attach them to the supplied the flat str. */

int fetch_item( BYTE *pbyObjExtn,OBJECT_INDEX obj_index, ITEM_TYPE itemType,
			     unsigned long *pulItemMask, void *flats);

/*Prototypes for DDL6FetchNattach.cpp*/
int attach_chart_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_graph_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

/*Vibhor 260804: Start of Code */

int attach_axis_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_file_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_waveform_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

/*Vibhor 260804: End of Code */

/*Vibhor 290904: Start of code*/

int attach_list_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

int attach_source_data(unsigned char *attr_data_ptr, unsigned long data_len,
	void *flats, unsigned short tag);

/*Vibhor 290904: End of code*/

/*stevev 22mar05 - add image */
int attach_image_data(unsigned char *attr_data_ptr, unsigned long data_len,
					  void *flats, unsigned short tag);
int attach_grid_data (unsigned char *attr_data_ptr, unsigned long data_len,
					  void *flats, unsigned short tag);
/* end stevev 22mar05 */

#endif /*FETCHITEM_H*/
