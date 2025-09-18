/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *	@(#)table.h	40.1  40  07 Jul 1998
 *  modified for sdc
 */

#ifndef TABLE_H
#define TABLE_H

#ifdef INC_DEBUG
#pragma message("In table.h") 
#endif

#include "std.h"
#include "Flats.h"
#include "DD_Header.h"
//#include "cm_lib.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::table.h") 
#endif

typedef UINT32	FILE_OFFSET;

union DD_REFERENCE{
	OBJECT_INDEX	object_index;
	FILE_OFFSET		file_offset;
};


/*
 *	Abbreviations Used:
 *
 *	blk - block
 *	char - characteristic
 *	dict - dictionary
 *	dir - directory
 *	elem - element
 *	id - identifier
 *	mem - member
 *	param - parameter
 *	prog - program
 *	rec - record
 *	ref - reference
 *	rel - relation
 *	tbl - table
 *
 */


/*
 *	Block Directory Definitions
 */
struct BLK_ITEM_TBL_ELEM{
	ITEM_ID		blk_item_id;
	int			blk_item_name_tbl_offset;
};

struct BLK_ITEM_TBL{
	int					 count;
	BLK_ITEM_TBL_ELEM	*list;
} ;


/* Block Item Name Table */

struct BLK_ITEM_NAME_TBL_ELEM{
	ITEM_ID		blk_item_name;
	int			item_tbl_offset;
	int			param_tbl_offset;
	int			param_list_tbl_offset;
	int			rel_tbl_offset;
	int			read_cmd_tbl_offset;
	int			read_cmd_count;                     
	int			write_cmd_tbl_offset;
	int			write_cmd_count;

};

struct BLK_ITEM_NAME_TBL{
	int						 count;
	BLK_ITEM_NAME_TBL_ELEM	*list;
};


/* Parameter Table */

struct PARAM_TBL_ELEM{
	int		blk_item_name_tbl_offset;
	int		param_mem_tbl_offset;
	int		param_mem_count;
	int		param_elem_tbl_offset;
	int		param_elem_count;
	int		array_elem_item_tbl_offset;
	int		array_elem_count;
	int		array_elem_type_or_var_type;
	int		array_elem_size_or_var_size;
	ulong	array_elem_class_or_var_class;
};

struct PARAM_TBL{
	int				 count;
	PARAM_TBL_ELEM	*list;
};


/* Parameter Member Table */

struct PARAM_MEM_TBL_ELEM{
	int		item_tbl_offset;
	int		param_mem_type;
	int		param_mem_size;
	ulong	param_mem_class;
	int		rel_tbl_offset;
};

struct PARAM_MEM_TBL{
	int					 count;
	PARAM_MEM_TBL_ELEM	*list;
};

/* Parameter Element Table */

struct PARAM_ELEM_TBL_ELEM{
	SUBINDEX	param_elem_subindex;
	int			rel_tbl_offset;
};

struct PARAM_ELEM_TBL{
	int					 count;
	PARAM_ELEM_TBL_ELEM	*list;
};


/* Parameter Member Name Table */

struct PARAM_MEM_NAME_TBL_ELEM{
	ITEM_ID		param_mem_name;
	int			param_mem_offset;
};

struct PARAM_MEM_NAME_TBL{
	int							 count;
	PARAM_MEM_NAME_TBL_ELEM		*list;
};


/* Parameter List Table */

struct PARAM_LIST_TBL_ELEM{
	int		blk_item_name_tbl_offset;
	int		param_list_mem_tbl_offset;
	int		param_list_mem_count;
};

struct PARAM_LIST_TBL{
	int						 count;
	PARAM_LIST_TBL_ELEM		*list;
};


/* Parameter List Member Table */

struct PARAM_LIST_MEM_TBL_ELEM{
	int		blk_item_name_tbl_offset;
};

struct PARAM_LIST_MEM_TBL{
	int							 count;
	PARAM_LIST_MEM_TBL_ELEM		*list;
};


/* Parameter List Member Name Table */

struct PARAM_LIST_MEM_NAME_TBL_ELEM{
	ITEM_ID		param_list_mem_name;
	int			param_list_mem_tbl_offset;
};

struct PARAM_LIST_MEM_NAME_TBL{
	int								 count;
	PARAM_LIST_MEM_NAME_TBL_ELEM	*list;
};


/* Characteristic Member Table */

struct CHAR_MEM_TBL_ELEM{
	int		item_tbl_offset;
	int		char_mem_type;
	int		char_mem_size;
    ulong   char_mem_class;
	int		rel_tbl_offset;
};

struct CHAR_MEM_TBL{
	int					 count;
	CHAR_MEM_TBL_ELEM	*list;
};


/* Characteristic Member Name Table */

struct CHAR_MEM_NAME_TBL_ELEM{
	ITEM_ID		char_mem_name;
	int			char_mem_offset;
};

struct CHAR_MEM_NAME_TBL{
	int						 count;
	CHAR_MEM_NAME_TBL_ELEM	*list;
};


/* Relation Table */

struct REL_TBL_ELEM{
	int		wao_item_tbl_offset;
	int		unit_item_tbl_offset;
	int		update_tbl_offset;
	int		update_count;
	int		unit_count;// added 27feb08
};

struct REL_TBL{
	int				 count;
	REL_TBL_ELEM	*list;
};

/* Update Table */

struct UPDATE_TBL_ELEM{
	int				op_it_offset ;
	SUBINDEX		op_subindex ;
	int				desc_it_offset ;
};

struct UPDATE_TBL{
	int				 count;
	UPDATE_TBL_ELEM	*list;
};



/* Command Table Structures */

struct COMMAND_INDEX{
	ITEM_ID		id;				
	ulong		value;
};


struct COMMAND_TBL_ELEM{
	ulong				number;
	ulong				transaction;
	SUBINDEX			subindex;
	ushort				weight;
	int					count;
	COMMAND_INDEX		*index_list;
};

struct COMMAND_TBL{
	int					count;
	COMMAND_TBL_ELEM	*list;
};

// stevev 04mar08 - for edd 8.1
struct COMMAND_TBL_8_ELEM
{
	SUBINDEX			subindex;
	ulong				number;
	ulong				transaction;
	ushort				weight;
	int					count;
	COMMAND_INDEX		*index_list;
};

struct PTOC_TBL_8_ELEM
{
	ITEM_ID				item_id;
	int					rd_count;
	COMMAND_TBL_8_ELEM*	rd_list;
	int					wr_count;
	COMMAND_TBL_8_ELEM*	wr_list;
};

struct COMMAND_TBL_8{
	int				count;
	PTOC_TBL_8_ELEM	*list;
};
// end stevev 04mar08 ---------

struct CRIT_PARAM_TBL{
	int	     count;
	ITEM_ID	 *list;
} ;


/* Block Directory Binary */

struct BIN_BLOCK_DIR{
	unsigned long	bin_exists;
	unsigned long	bin_hooked;

	BININFO		blk_item_tbl;
	BININFO		blk_item_name_tbl;
	BININFO		param_tbl;
	BININFO		param_mem_tbl;
	BININFO		param_mem_name_tbl;
	BININFO		param_elem_tbl;
	BININFO		param_list_tbl;
	BININFO		param_list_mem_tbl;
	BININFO		param_list_mem_name_tbl;
	BININFO		char_mem_tbl;
	BININFO		char_mem_name_tbl;
	BININFO		rel_tbl;
	BININFO		update_tbl;

	BININFO		command_tbl;
	BININFO		crit_param_tbl;

};


/* Block Directory Flat */

struct FLAT_BLOCK_DIR{
	unsigned long				attr_avail;
	BLK_ITEM_TBL 				blk_item_tbl;
	BLK_ITEM_NAME_TBL			blk_item_name_tbl;
	PARAM_TBL     				param_tbl;
	PARAM_MEM_TBL 				param_mem_tbl;
	PARAM_MEM_NAME_TBL			param_mem_name_tbl;
	PARAM_ELEM_TBL				param_elem_tbl;
	PARAM_LIST_TBL				param_list_tbl;
	PARAM_LIST_MEM_TBL 			param_list_mem_tbl;
	PARAM_LIST_MEM_NAME_TBL		param_list_mem_name_tbl;
	CHAR_MEM_TBL				char_mem_tbl;
	CHAR_MEM_NAME_TBL			char_mem_name_tbl;
	REL_TBL 					rel_tbl;
	UPDATE_TBL					update_tbl;

	COMMAND_TBL					command_tbl;
    CRIT_PARAM_TBL              crit_param_tbl;

};


/*
 *	Device Tables
 */


/* Block Table */

struct BLK_TBL_ELEM{
	ITEM_ID				blk_id;
	int					item_tbl_offset;
	int					char_rec_item_tbl_offset;
	int					char_rec_bint_offset;
	DD_REFERENCE		blk_dir_dd_ref;
	int					usage;
	FLAT_BLOCK_DIR		flat_block_dir;
};

struct BLK_TBL{
	int				 count;
	BLK_TBL_ELEM	*list;
};


/* Item Table */

struct ITEM_TBL_ELEM{
	ITEM_ID         item_id;
	DD_REFERENCE    dd_ref;
	ITEM_TYPE       item_type;
};

struct ITEM_TBL{
	unsigned		 count;
	ITEM_TBL_ELEM	*list;
};


/* Program Table */

struct PROG_TBL_ELEM{
	ITEM_ID         item_id;
	DD_REFERENCE    dd_ref;
};

struct PROG_TBL{
	int				 count;
	PROG_TBL_ELEM	*list;
};


/* Domain Table */

struct DOMAIN_TBL_ELEM{
	ITEM_ID         item_id;
	DD_REFERENCE    dd_ref;
} ;

struct DOMAIN_TBL{
	int					 count;
	DOMAIN_TBL_ELEM		*list;
};


/* String Table */

struct STRING_TBL{
	unsigned char	*root;    /* memory chunk pointer */
	int				 count;
	ddpSTRING          *list;
};


/* Dictionary Reference Table */

struct DICT_REF_TBL{
	unsigned	 count;
	UINT32		*list;	// list of dict keys

// timj added 9oct07
	ddpSTRING		*name;// list of names
	ddpSTRING		*text;// list of dict strings
};


/* Local variable table */

struct LOCAL_VAR_TBL_ELEM{
	ITEM_ID			item_id;
	DD_REFERENCE	dd_ref;
	unsigned short  type;
	unsigned short  size;
};


struct LOCAL_VAR_TBL{
	int				count;
	LOCAL_VAR_TBL_ELEM	*list;
};


/* Command number to item id conversion table */

struct CMD_NUM_ID_TBL_ELEM{
	ulong		number;
	ITEM_ID		item_id;
};


struct CMD_NUM_ID_TBL{
	unsigned			count;
	CMD_NUM_ID_TBL_ELEM	*list;
};



/* Device Directory Binary */

struct BIN_DEVICE_DIR{
	unsigned long	bin_exists;
	unsigned long	bin_hooked;
	BININFO		blk_tbl;
	BININFO		item_tbl;
	BININFO		prog_tbl;
	BININFO		domain_tbl;
	BININFO		string_tbl;
	BININFO		dict_ref_tbl;
	BININFO		local_var_tbl;
	BININFO		cmd_num_id_tbl;
};


/* Device Directory Flat */

struct FLAT_DEVICE_DIR{
	unsigned long		attr_avail;
	BLK_TBL				blk_tbl;
	ITEM_TBL			item_tbl;
	PROG_TBL			prog_tbl;
	DOMAIN_TBL			domain_tbl;
	STRING_TBL			string_tbl;
	DICT_REF_TBL		dict_ref_tbl;
	LOCAL_VAR_TBL		local_var_tbl;
	CMD_NUM_ID_TBL		cmd_num_id_tbl;
} ;

#define DEFAULT_OFFSET      (-1)

/*Vibhor 010904: Start of Code*/
/*Adding new definition Device Directory for HART 6*/

struct IMG_ITEM{
	BYTE 				lang_code[6]; /*Which language uses this image*/
	DATAPART_SEGMENT_6	img_file;  /*Copy of image file*/
	BYTE*               p2Graphik; /* pointer to the binary, raw image*/
};


struct IMAGE_TBL_ELEM{
		unsigned num_langs; /*Number of Languages*/
		IMG_ITEM *img_list;
};

struct IMAGE_TBL{
	unsigned        count;
	IMAGE_TBL_ELEM  *list;
};


struct BIN_DEVICE_DIR_6{
	unsigned long	bin_exists;
	unsigned long	bin_hooked;
	BININFO		blk_tbl;
	BININFO		item_tbl;
	BININFO		prog_tbl;
	BININFO		domain_tbl;
	BININFO		string_tbl;
	BININFO		dict_ref_tbl;
	BININFO		local_var_tbl;
	BININFO		cmd_num_id_tbl;
	BININFO		image_tbl;
};


/* Device Directory Flat */

struct FLAT_DEVICE_DIR_6{
	unsigned long		attr_avail;
	BLK_TBL				blk_tbl;
	ITEM_TBL			item_tbl;
	PROG_TBL			prog_tbl;
	DOMAIN_TBL			domain_tbl;
	STRING_TBL			string_tbl;
	DICT_REF_TBL		dict_ref_tbl;
	LOCAL_VAR_TBL		local_var_tbl;
	CMD_NUM_ID_TBL		cmd_num_id_tbl;
	IMAGE_TBL			image_tbl; // New table added in eDDL
} ;


/*Vibhor 010904: End of Code*/

// stevev 22feb07 --- added

/* Block Directory Binary */

struct BIN_BLOCK_DIR_6{
	unsigned long	bin_exists;
	unsigned long	bin_hooked;

	BININFO		blk_item_tbl;
	BININFO		blk_item_name_tbl;
	BININFO		param_tbl;
	BININFO		param_mem_tbl;
	BININFO		param_mem_name_tbl;
	BININFO		param_elem_tbl;
	BININFO		param_list_tbl;
	BININFO		param_list_mem_tbl;
	BININFO		param_list_mem_name_tbl;
	BININFO		char_mem_tbl;
	BININFO		char_mem_name_tbl;
	BININFO		rel_tbl;
	BININFO		update_tbl;

	BININFO		command_tbl;
	BININFO		crit_param_tbl;

};


/* Block Directory Flat */

struct FLAT_BLOCK_DIR_6{
	unsigned long				attr_avail;
	BLK_ITEM_TBL 				blk_item_tbl;
	BLK_ITEM_NAME_TBL			blk_item_name_tbl;
	PARAM_TBL     				param_tbl;
	PARAM_MEM_TBL 				param_mem_tbl;
	PARAM_MEM_NAME_TBL			param_mem_name_tbl;
	PARAM_ELEM_TBL				param_elem_tbl;
	PARAM_LIST_TBL				param_list_tbl;
	PARAM_LIST_MEM_TBL 			param_list_mem_tbl;
	PARAM_LIST_MEM_NAME_TBL		param_list_mem_name_tbl;
	CHAR_MEM_TBL				char_mem_tbl;
	CHAR_MEM_NAME_TBL			char_mem_name_tbl;
	REL_TBL 					rel_tbl;
	UPDATE_TBL					update_tbl;

	COMMAND_TBL					command_tbl;
	COMMAND_TBL_8				command_to_var_tbl;
    CRIT_PARAM_TBL              crit_param_tbl;
};

// end stevev 22feb08 

#endif	/* TABLE_H */
