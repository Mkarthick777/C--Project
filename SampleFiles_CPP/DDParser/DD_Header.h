#ifndef DDHEADER_H
#define DDHEADER_H

/* see Tags_sa.h ... this is a copy for some reason...BE CAREFUL */
typedef	unsigned short	short_offset;
typedef	unsigned int	long_offset;
typedef	unsigned short	object_index;


struct DDOD_HEADER{  
	unsigned int		magic_number;
	unsigned int		header_size;
	unsigned int		objects_size;
	unsigned int		data_size;
	BYTE				byManufacturer[3];
	unsigned short		device_type;
	BYTE				device_revision;
	BYTE				dd_revision;
	BYTE				tok_rev_major;/*Vibhor 300904: New binary spec,2 bytes from reserved1 */
	BYTE				tok_rev_minor;/*moved out & defined as these 2 rev nos.*/
	unsigned short		reserved1;
	unsigned int		fileSignature;
	unsigned int		reserved3;
	unsigned int		reserved4;
};
/*
#define MAGIC_NUMBER_OFFSET		0
#define HEADER_SIZE_OFFSET		4
#define OBJECTS_SIZE_OFFSET		8
#define DATA_SIZE_OFFSET		12
#define MANUFACTURER_OFFSET		16
#define DEV_TYPE_OFFSET			19
#define DEV_REV_OFFSET			21
#define DD_REV_OFFSET			22

#define DDOD_HEADER_SIZE 39 */

struct	DD_ODES{
	object_index		index;
	BYTE				ram_rom_flag;
	BYTE				name_length;
	BYTE				access_protection_flag;
	unsigned short		version;

	long_offset			local_address_odes;

	unsigned short		STOD_length;

	long_offset			local_address_stod;

	object_index		sod_first_index;
	unsigned short		sod_length;
	long_offset			local_address_sod;

	object_index		dpod_first_index;
	unsigned short		dpod_length;
	long_offset			local_address_dpod;


	object_index		dvod_first_index;
	unsigned short		dvod_length;
	long_offset			local_address_dvod;
};

#define DD_ODES_INDEX_OFFSET		0
#define	RAM_ROM_FLAG_OFFSET			2
#ifndef OD_DEFS_H
  #define NAME_LENGTH_OFFSET			3
#endif
#define ACCESS_PROT_FLAG_OFFSET		4
#define VERSION_OFFSET				5
#ifndef OD_DEFS_H
  #define LOC_ADDR_ODES_OFFSET		7
#define STOD_LENGTH_OFFSET			11
#define LOC_ADDR_STOD_OFFSET		13
#endif
#define SOD_FIRST_INDX_OFFSET		17
#ifndef OD_DEFS_H
#define SOD_LENGTH_OFFSET			19
#define LOC_ADDR_SOD_OFFSET			21
#endif
#define DPOD_FIRST_INDX_OFFSET		25
#ifndef OD_DEFS_H
#define DPOD_LENGTH_OFFSET			27
#define LOC_ADDR_DPOD_OFFSET		29
#endif
#define DVOD_FIRST_INDX_OFFSET		33
#ifndef OD_DEFS_H
#define DVOD_LENGTH_OFFSET			35
#define LOC_ADDR_DVOD_OFFSET		37
#endif

#define DD_ODES_SIZE  41

struct	DOMAIN_FIXED{
	object_index		index;
	BYTE				byObjectCode;
	WORD				wDomainDataSize;
	BYTE				byReserved1;
	BYTE				byReserved2;
	WORD				wReserved3;
	long_offset			longAddress;
	BYTE				byDomainState;
	BYTE				byUploadState;
	char				chCounter;
} ; 

#define DOM_FIXED_INDX_OFFSET		0
#define	OBJ_CODE_OFFSET				2
#define	DOM_DATA_SIZE_OFFSET		3
#define	LONG_ADDR_OFFSET			9
#define DOM_STATE_OFFSET			13
#ifndef UPLOAD_STATE_OFFSET
#define UPLOAD_STATE_OFFSET			14	
#endif
#ifndef COUNTER_OFFSET
#define	COUNTER_OFFSET				15
#endif

#define DOMAIN_FIXED_SIZE 16

struct	FORMAT_EXTENSION{

	BYTE				byLength;
	BYTE				byFormatObjectCode;
	BYTE				byCodingFormatMajor;
	BYTE				byCodingFormatMinor;
	BYTE				byDDRevision;
	char				pchProfileNumber[2];
	WORD				wNumberOfImports;
	WORD				wNumberOfLikes;
};

#define FMT_EXTN_LENGTH_OFFSET	0
#define FMT_OBJ_CODE_OFFSET		1
#define CODING_FMT_MAJ_OFFSET	2
#define CODING_FMT_MIN_OFFSET	3
#define DDREV_OFFSET			4
#define PROFILE_NO_OFFSET		5
#define NO_O_IMPORTS_OFFSET		7
#define NO_O_LIKES_OFFSET		9

//#define FORMAT_EXTENSION_SIZE  15

struct DATAPART_SEGMENT{

	short_offset		offset; 
	WORD				wSize; 
};


#define SEG_DATA_OFFSET			0
#define SEG_SIZE_OFFSET			2
#define DATAPART_SEGMENT_SIZE	4

struct DEVICE_DIR_EXT{ 

	BYTE				byLength;
	BYTE				byDeviceDirObjectCode;
	BYTE				byFormatCode;
	DATAPART_SEGMENT	BlockNameTable;
	DATAPART_SEGMENT	ItemTable;
	DATAPART_SEGMENT	ProgramTable;
	DATAPART_SEGMENT	DomainTable;
	DATAPART_SEGMENT	StringTable;
	DATAPART_SEGMENT	DictReferenceTable;
	DATAPART_SEGMENT	LocalVariableTable;
	DATAPART_SEGMENT	CommandTable;
};

#define DEV_DIR_LENGTH_OFFSET		0
#define DEV_DIR_OBJ_CODE_OFFSET		1
#define DEV_DIR_FORMAT_CODE_OFFSET	2
#define BLK_NAME_TBL_OFFSET			3
#define ITEM_TBL_OFFSET				7
#define PROG_TBL_OFFSET				11
#define DOM_TBL_OFFSET				15
#define STRNG_TBL_OFFSET			19
#define DICT_REF_TBL_OFFSET			23
#define LOC_VAR_TBL_OFFSET			27
#define CMD_TBL_OFFSET				31

#define DEVICE_DIR_EXT_SIZE 35
#define DEVICE_DIR_LENGTH			(DEVICE_DIR_EXT_SIZE - 1)



/* stevev 10/13/04 - added back in */
struct BLOCK_DIR_EXT{
	BYTE				byLength;
	BYTE				byBlockDirObjectCode;
	BYTE				byFormatCode;
	DATAPART_SEGMENT	BlockItemTable;
	DATAPART_SEGMENT	BlockItemNameTable;
	DATAPART_SEGMENT	ParameterTable;
	DATAPART_SEGMENT	ParameterMemberTable;
	DATAPART_SEGMENT	ParameterMemberNameTable;
	DATAPART_SEGMENT	ParameterElementTable;
	DATAPART_SEGMENT	ParameterListTable;
	DATAPART_SEGMENT	ParameterListMemberTable;
	DATAPART_SEGMENT	ParameterListMemberNameTable;
	DATAPART_SEGMENT	CharectersiticsMemberTable;
	DATAPART_SEGMENT	CharectersiticsMemberNameTable;
	DATAPART_SEGMENT	RelationTable;
	DATAPART_SEGMENT	UpdateTable;
	DATAPART_SEGMENT	ParameterCommandTable;
	DATAPART_SEGMENT	CriticalParameterTable;
} ;

#define BLK_DIR_LENGTH_OFFSET				0
#define BLK_DIR_OBJ_CODE_OFFSET				1
#define BLK_DIR_FORMAT_CODE_OFFSET			2
#define BLK_ITEM_TBL_OFFSET					3
#define BLK_ITEMNAME_TBL_OFFSET				7
#define BLK_PARAM_TBL_OFFSET				11
#define BLK_PARAMEMBER_TBL_OFFSET			15
#define BLK_PARAMEMBERNAME_TBL_OFFSET		19
#define BLK_ELEMENT_TBL_OFFSET				23
#define BLK_PARAMLIST_TBL_OFFSET			27
#define BLK_PARAMLISTMEMBER_TBL_OFFSET		31
#define BLK_PARAMLISTMEMBERNAME_TBL_OFFSET	35
#define BLK_CHARMEMBER_TBL_OFFSET			39
#define BLK_CHARMEMBERNAME_TBL_OFFSET		43
#define BLK_RELATION_TBL_OFFSET				47
#define BLK_UPDATE_TBL_OFFSET				51
#define BLK_PARAM2COMMAND_TBL_OFFSET		55
#define BLK_CRITICALPARAM_TBL_OFFSET		59


#define BLK_DIR_EXT_SIZE 63
/* stevev 10/13/04*/


/*Vibhor 2905 : Start of Code*/

/*Structures defining the Extensions & Masks fields */

/*NOTE: While reading the respective masks, chk the ItemId first!!
  if an object doesnot have any ItemId (External Objects) then there
  are no masks for the same
  Adjust the Item's attribute offset accordingly */


/*Generally the Items have mask field of size 1 Octet*/

struct ITEM_EXTN{

	BYTE				byLength;
	BYTE				byItemType;
	BYTE				bySubType;
	BYTE				byItemId[4];
} ;

#define ITEM_EXTN_SIZE 7

/*Vibhor 300804: Start of Code*/
/************************************
HART 6.0 DEFINITIONS
*************************************/

struct DATAPART_SEGMENT_6{

	long_offset			offset; 
//	WORD				wSize ;// just till next Tokenizer iteration
	unsigned			uSize; 
};


#define SEG_DATA_OFFSET_6			0
#define SEG_SIZE_OFFSET_6			4
#define DATAPART_SEGMENT_6_SIZE		8 

struct DEVICE_DIR_EXT_6{ 

	BYTE				byLength;
	BYTE				byDeviceDirObjectCode;
	BYTE				byFormatCode;
	DATAPART_SEGMENT_6	BlockNameTable;
	DATAPART_SEGMENT_6	ItemTable;
	DATAPART_SEGMENT_6	ProgramTable;
	DATAPART_SEGMENT_6	DomainTable;
	DATAPART_SEGMENT_6	StringTable;
	DATAPART_SEGMENT_6	DictReferenceTable;
	DATAPART_SEGMENT_6	LocalVariableTable;
	DATAPART_SEGMENT_6	CommandTable;
	DATAPART_SEGMENT_6	ImageTable; 
};


#define DEV_DIR_LENGTH_6_OFFSET			0
#define DEV_DIR_OBJ_CODE_6_OFFSET		1
#define DEV_DIR_FORMAT_CODE_6_OFFSET	2
#define BLK_NAME_TBL_6_OFFSET			3
#define ITEM_TBL_6_OFFSET				11	/* previous + DATAPART_SEGMENT_6_SIZE */
#define PROG_TBL_6_OFFSET				19
#define DOM_TBL_6_OFFSET				27
#define STRNG_TBL_6_OFFSET				35
#define DICT_REF_TBL_6_OFFSET			43
#define LOC_VAR_TBL_6_OFFSET			51
#define CMD_TBL_6_OFFSET				59
#define IMG_TBL_6_OFFSET				67

#define DEVICE_DIR_EXT_6_SIZE  75	
#define DEVICE_DIR_LENGTH_6		(DEVICE_DIR_EXT_6_SIZE - 1)



/* stevev 22feb08 - convert to new hart 6/7 sizes */
struct BLOCK_DIR_EXT_6{
	BYTE				byLength;
	BYTE				byBlockDirObjectCode;
	BYTE				byFormatCode;
	DATAPART_SEGMENT_6	BlockItemTable;
	DATAPART_SEGMENT_6	BlockItemNameTable;
	DATAPART_SEGMENT_6	ParameterTable;
	DATAPART_SEGMENT_6	ParameterMemberTable;
	DATAPART_SEGMENT_6	ParameterMemberNameTable;
	DATAPART_SEGMENT_6	ParameterElementTable;
	DATAPART_SEGMENT_6	ParameterListTable;
	DATAPART_SEGMENT_6	ParameterListMemberTable;
	DATAPART_SEGMENT_6	ParameterListMemberNameTable;
	DATAPART_SEGMENT_6	CharectersiticsMemberTable;
	DATAPART_SEGMENT_6	CharectersiticsMemberNameTable;
	DATAPART_SEGMENT_6	RelationTable;
	DATAPART_SEGMENT_6	UpdateTable;
	DATAPART_SEGMENT_6	ParameterCommandTable;
	DATAPART_SEGMENT_6	CriticalParameterTable;
} ;

#define BLK_DIR_LENGTH_6_OFFSET				  0
#define BLK_DIR_OBJ_CODE_6_OFFSET			  1
#define BLK_DIR_FORMAT_CODE_6_OFFSET		  2
#define BLK_ITEM_TBL_6_OFFSET				  3
#define BLK_ITEMNAME_TBL_6_OFFSET			 11
#define BLK_PARAM_TBL_6_OFFSET				 19
#define BLK_PARAMEMBER_TBL_6_OFFSET			 27
#define BLK_PARAMEMBERNAME_TBL_6_OFFSET		 35
#define BLK_ELEMENT_TBL_6_OFFSET			 43
#define BLK_PARAMLIST_TBL_6_OFFSET			 51
#define BLK_PARAMLISTMEMBER_TBL_6_OFFSET	 59
#define BLK_PARAMLISTMEMBERNAME_TBL_6_OFFSET 67
#define BLK_CHARMEMBER_TBL_6_OFFSET			 75
#define BLK_CHARMEMBERNAME_TBL_6_OFFSET		 83
#define BLK_RELATION_TBL_6_OFFSET			 91
#define BLK_UPDATE_TBL_6_OFFSET				 99
#define BLK_PARAM2COMMAND_TBL_6_OFFSET		107
#define BLK_CRITICALPARAM_TBL_6_OFFSET		115


#define BLK_DIR_EXT_6_SIZE 123	
#define BLK_DIR_LENGTH_6		(BLK_DIR_EXT_6_SIZE - 1)
/* stevev 22feb08*/



/************************************
HART 8.0 DEFINITIONS
*************************************/

// no change from EDDF 6
// struct DATAPART_SEGMENT_6 
#define DATAPART_SEGMENT_8		DATAPART_SEGMENT_6
#define SEG_DATA_OFFSET_8		SEG_DATA_OFFSET_6
#define SEG_SIZE_OFFSET_8		SEG_SIZE_OFFSET_6
#define DATAPART_SEGMENT_8_SIZE	DATAPART_SEGMENT_6_SIZE

struct DEVICE_DIR_EXT_8{ 

	BYTE				byLength;
	BYTE				byDeviceDirObjectCode;
	BYTE				byFormatCode;
	DATAPART_SEGMENT_8	ItemTable;
	DATAPART_SEGMENT_8	StringTable;
	DATAPART_SEGMENT_8	DictReferenceTable;
	DATAPART_SEGMENT_8	CommandTable;
	DATAPART_SEGMENT_8	ImageTable; 
};


#define DEV_DIR_LENGTH_8_OFFSET			0
#define DEV_DIR_OBJ_CODE_8_OFFSET		1
#define DEV_DIR_FORMAT_CODE_8_OFFSET	2
#define ITEM_TBL_8_OFFSET				3	/* previous + DATAPART_SEGMENT_8_SIZE */
#define STRNG_TBL_8_OFFSET				11
#define DICT_REF_TBL_8_OFFSET			19
#define CMD_TBL_8_OFFSET				27
#define IMG_TBL_8_OFFSET				35

#define DEVICE_DIR_EXT_8_SIZE  43	
#define DEVICE_DIR_LENGTH_8		(DEVICE_DIR_EXT_8_SIZE - 1)



/* stevev 22feb08 - convert to new hart 6/7 sizes */
struct BLOCK_DIR_EXT_8{
	BYTE				byLength;
	BYTE				byBlockDirObjectCode;
	BYTE				byFormatCode;
	DATAPART_SEGMENT_8	BlockItemTable;
	DATAPART_SEGMENT_8	BlockItemNameTable;
	DATAPART_SEGMENT_8	ParameterTable;
	DATAPART_SEGMENT_8	RelationTable;
	DATAPART_SEGMENT_8	UpdateTable;
	DATAPART_SEGMENT_8	ParameterCommandTable;
	DATAPART_SEGMENT_8	CriticalParameterTable;
} ;

#define BLK_DIR_LENGTH_8_OFFSET				  0
#define BLK_DIR_OBJ_CODE_8_OFFSET			  1
#define BLK_DIR_FORMAT_CODE_8_OFFSET		  2
#define BLK_ITEM_TBL_8_OFFSET				  3
#define BLK_ITEMNAME_TBL_8_OFFSET			 11
#define BLK_PARAM_TBL_8_OFFSET				 19
#define BLK_RELATION_TBL_8_OFFSET			 27
#define BLK_UPDATE_TBL_8_OFFSET				 35
#define BLK_PARAM2COMMAND_TBL_8_OFFSET		 43
#define BLK_CRITICALPARAM_TBL_8_OFFSET		 51


#define BLK_DIR_EXT_8_SIZE   59	
#define BLK_DIR_LENGTH_8		(BLK_DIR_EXT_8_SIZE - 1)
/* stevev 22feb08*/


#endif /*DDHEADER_H*/
