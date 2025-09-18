// HCMDDParser.h: interface for the HCMDDParser class.
//
//////////////////////////////////////////////////////////////////////

#ifndef DDLDEVDESCRIPTION_H
#define DDLDEVDESCRIPTION_H

//#include <stdio.h>
#include <vector>
#include "DD_Header.h"
#include "DDLDefs.h"
// moved to common as tags_sa.h    #include "DDLTags.h"
#include "Tags_sa.h"
#include "ParserEndian.h"
#include "Flats.h"
#include "DDlBaseItem.h"
#include "Dictionary.h"
#include "LitStringTable.h"
#include "Table.h" 
#include <iostream>
#include <fstream>
using namespace std;


#define MFG_ID_SIZE		3
	
typedef vector <DDlBaseItem *> DevItemList;	
typedef vector <unsigned long> ItemIDList;	

struct DEV_ID{

	unsigned long 		ulMfgID;
	unsigned short		uDevType;
	unsigned char				byDevRev;
	unsigned char				byDDRev;

	DEV_ID():ulMfgID(0),uDevType(0),byDevRev(0),byDDRev(0){};
	~DEV_ID(){};

};

extern 
bool isInTokizer;// defaults to false.

/*
extern unsigned long 		num_dict_table_entries;
extern DICT_TABLE_ENTRY 	dict_table[MAX_DICT_TABLE_SIZE];
extern void dict_table_install (unsigned long ref, char *name, char *str); */


class DDlDevDescription
{

private:
	

//#ifndef _WIN32
	FILE *fp;
//#else
//	std::ifstream fp;
//#endif
	DDOD_HEADER header;
	DD_ODES descriptor;
	char dictfilepath[MAX_PATH];
	unsigned uSodLen;
	bool bDevDirAllocated;
	bool bDevDir6Allocated; //Vibhor 080904: Added
	bool bDictAllocated;
	bool bSymAllocated;

	BYTE CodingMajor; /*Major Revision Number of the Tokenizer*/
	BYTE CodingMinor; /*Minor Revision Number of the Tokenizer*/
	BYTE TokenizerType;
	BYTE DDRevision;  /*DD Revision*/
	WORD ImpCnt;	  /*Number of Imports in the DD*/	
	WORD LikCnt;	  /*Number of Likes in the DD*/		

	/*The Flat structures are for the "Fetch"ing part of the binary chunks */

	FLAT_VAR			fVar;
	FLAT_COMMAND		fCmd;
	FLAT_MENU			fMenu;
	FLAT_EDIT_DISPLAY	fEditDisp;
	FLAT_METHOD			fMethod;
	FLAT_ITEM_ARRAY		fIArr;
	FLAT_COLLECTION		fColl;
	FLAT_REFRESH		fRefresh;
	FLAT_UNIT			fUnit;
	FLAT_WAO			fWao;
	FLAT_RECORD			fRec;
	FLAT_BLOCK			fBlock;

	BYTE*               pGraphics;// raw image blob
	
	bool ValidateHeader( DDOD_HEADER* pDDODheader );/*Check the Magic no */


	bool ReadObjectDescription();/*Parse the DDOD-ODES*/
	bool ValidateObjectDescription(DD_ODES* pDescriptor);/*Check the First Index etc.*/



/*	Each object in SOD has a fixed part & an extention part, 
	The fixed part is common to all : DOMAIN_FIXED
	Followed by the fixed part is the varible Extension part, the first
	byte of which contains the length of the Extension part, so read the stuff 
	& store it in  the Object Extention array
	Fixed parts are of no better use than to tell us the Object Index, Code & Size of the
	data part . Code is most imp 7 meaningful to us  */
	

	bool ReadSOD(); /* Read the SOD objects : Fixed + Extension */
	bool ValidateFixed( DOMAIN_FIXED *pDomainFixed);

	unsigned int GetSize(int i);	
	bool ReadObjectValues(); /*Read the Data part of the object if there's one
							   and store the same in the global ObjectValue array*/
	
	/*Now we know the Item Types, and their associated data Lengths
	  so we need to have an API to evaluate the attributes */
	bool GetItems(); /*This method loops through the Object lists & calls Fetch
					   & Eval APIs to evaluate the Attributes of the items*/
	bool GetItems6();
	bool GetImages6();

	bool LoadDictionary(char *pchLanguageCode);/*Loads the Standard dictionary & Dictionary Extensions */

	bool LoadDeviceDirectory();/*This method loads a global device directory, containing,
								Standard Dictionary & Device Specific String Tables
								for the resolution of references to the same in various
								DD Items
								Note: If need be we can also load the Block Directory
								in the same method call, with some additions*/

	bool LoadBlockItem();/*To resolve certain references like viaParam ref, viaBlock ref
						  we need to have the HART Block evaluated before any of the 
						  DD items, so this method is called by GetItems after 
						  LoadDeviceDirectory, but before evaluating any 'regular'
						  HART DD item*/

	void CleanGlobals(); // CleanArrays and several others
	void CleanArrays(); /* To delete & clear the pbyExtensions & pbyObjectValues arrays  */
	void ClearArrays(); /* To initialize pbyExtensions & pbyObjectValues to NULL */

//	void UnLoadDictionary();/*Unloading & Cleaning up the Dictionaries*/

	bool ResolveItemName();

	/*Vibhor 310804: Start of Code*/
	int ReadIntegerValueFromFile(FILE *fileP,
								UINT64 & uValue,
								BYTE & byLength);/*This function will read a multibyte integer
												   from  the binary file and return the value
											       and length(in bytes of the integer*/

	bool ReadFormatObject();/*Read the Format Object and Fill in the version 
							  numbers of the tokenizer */

	
	bool LoadDeviceDirectory_6();/*This is same as the LoadDeviceDirectory(),
								   except for the fact that it will load the new Image Table,
								   if present.*/

								
	/*Vibhor 310804: End of Code*/
	
public:
	
	DEV_ID  devID;

	DevItemList   ItemsList ; /*This is the list of all the Items defined in the 
							    DD file for the particular device*/
	BIimageList_t ImagesList;	/* a list of frame lists: frames are lang/size/ptr */
	ItemIDList    CriticalParamList;

	DDlDevDescription();
	DDlDevDescription(char* dictPath);
	~DDlDevDescription();
	/*NOTE : The following API does not get the Manufacturer */
	/*Opens and loads the specified DD into memory*/
	bool Initialize(char *pchFileName,char*pchLanguageCode,CDictionary *pDict = NULL,LitStringTable *pLit = NULL);

	bool LoadDeviceDescription(bool isInTok);/*This is the exposed method to load the device descriptions*/
	
	void getTokVer(unsigned& major, unsigned& minor){major=CodingMajor;minor =CodingMinor;};
#ifdef _WIN32_WCE	
	unsigned int DDlDevDescription::aligndata(unsigned char * char_ptr);	// PAW overcome aligment issues 29/05/09
#endif
#ifdef _DEBUG // 00.01.11
	bool LogHeader();
	bool LogOD();
	bool LogSOD();
#endif	
	// stevev 14mar08-make static and public so we can verify the file by only reading the header*/
	static bool ReadHeader(DDOD_HEADER& hdr, FILE * pF);/*Parse the DD Header Section*/
};

#endif // #ifndef HCMDDPARSER_H
