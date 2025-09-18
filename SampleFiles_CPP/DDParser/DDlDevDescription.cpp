// DDlDevDescription.cpp: implementation of the DDlDevDescription class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)

//sjv06feb06 #include <windows.h>
#include "pch.h"
#include <fstream>
#include <iostream>
#include "ddbGeneral.h"
#include <string>
#include "DDlDevDescription.h"

#include <iostream>

#include <iomanip>

//#include<iostream.h>
#include "globaldefs.h"

#include "fetch_item.h"
#include "evl_loc.h"
//#include "Dict.h"
#include "Table.h"
//#include "PrintData.h"
#include "DDlItems.h"
#include "Eval_Item.h"

#include "Parse_Base.h"/* stevev 10/13/04 - inline critical parameter table parse */
#include "tags_sa.h"

#include "logging.h"


#ifdef _DEBUG
/// #define LOG_READ /* comment out to prevent logging as-we-read */
#endif
//////////////////////////////////////////////////////////////////////
// Global memory allocation
//////////////////////////////////////////////////////////////////////

FLAT_DEVICE_DIR device_dir;
BIN_DEVICE_DIR bin_dev_dir;

extern FLAT_DEVICE_DIR_6 device_dir_6; 
extern FLAT_BLOCK_DIR_6  block_dir_6;

// extern const char* symFilePath;
bool bTokRev6Flag = false;/*Vibhor 090804: This flag will help some functions down under, 
										   decide which DeviceDirectory to use, HART 5 or HART 6*/

DDlBlock *pBlock; /* Global DDlBlock pointer to resolve 'Parameter' references */

CDictionary *pGlobalDict = NULL;
LitStringTable *pLitStringTable = NULL; /*The Global literal string table object*/

char langCode[5];

char *symFilePath = NULL;
//Vibhor 200105: Increasing the size of arrays, was 1500
// stevev 28jun06 Increase to 2K was 1800
// stevev 23oct08 Just be done with it... 16k from 0x800
#define MAX_SOD   0x4000
DOMAIN_FIXED ObjectFixed[MAX_SOD];
BYTE byExtLengths[MAX_SOD]; /*Array to hold the object Extension Lengths*/ //Vibhor 300904: Restored
BYTE *pbyExtensions[MAX_SOD];/*Array of pointers pointing to the Extension parts of objects*/
BYTE *pbyObjectValue[MAX_SOD];/*Array of pointers pointing to the value (data) parts of the objects*/
unsigned uSODLength;

bool isInTokizer = false;

//////////////////////////////////////////////////////////////////////
// Static - global memory allocation
//////////////////////////////////////////////////////////////////////
static bool bGlobalDictAllocated = false;
//bool bDictAllocated = false; /*We will set this flag if we build the dictionary*/

//////////////////////////////////////////////////////////////////////
// Static - prototypes
//////////////////////////////////////////////////////////////////////
int get_item_name(unsigned long item_id, char* item_name);

//////////////////////////////////////////////////////////////////////
// Definitions
//////////////////////////////////////////////////////////////////////
#define DEFAULT_LANGUAGE_CODE  "|en|"
#define SYM_EXTN_LEN  4

#ifndef linux
#define EVAL_FAILED( w, p ) LOGIT(CERR_LOG,L"\n eval_" L#w L"  failed for : %d\t i = %d\t RetCode = %d\n", p ->id, i, iRetVal)
#else
#define EVAL_FAILED( w, p ) ::logout(CERR_LOG, L"\n eval_" L ## #w L"  failed for : %d\t i = %d\t RetCode = %d\n", p ->id, i, iRetVal)
#endif
//  EVAL_FAILED(  ,  );
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



DDlDevDescription::DDlDevDescription()
{
	uSodLen = 0;
	bDevDirAllocated = false;
	bDictAllocated = false;
	bSymAllocated = false;

	fp = NULL;
	pGraphics = NULL;
	// for unitialized read (purify)
	bDevDir6Allocated = false;
	ClearArrays();
}

DDlDevDescription::DDlDevDescription(char* dictPath)
{
	uSodLen = 0;
	bDevDirAllocated = false;
	bDictAllocated = false;
	bSymAllocated = false;

	fp = NULL;
	pGraphics = NULL;
	std::string str(dictPath);
	std::copy(str.begin(),str.end(), dictfilepath);
	bDevDir6Allocated = false;// sjv 11jan06 - try this

	ItemsList.clear();
	ImagesList.clear();
	CriticalParamList.clear();
	ClearArrays();
}



DDlDevDescription::~DDlDevDescription()
{
	if(ItemsList.size() > 0)
	{
		for(DevItemList:: iterator it = ItemsList.begin(); it != ItemsList.end(); it++)
		{
			delete (*it);
		}
		ItemsList.clear();
	}
	if(CriticalParamList.size() > 0)
		CriticalParamList.clear();

	if(/*bDictAllocated == true &&  J.U.*/bGlobalDictAllocated == true) 
	{

		if(bDictAllocated)// was if(pGlobalDict && bDictAllocated)  J.U.
		{
			if (pGlobalDict != NULL) // added J.U.
				delete pGlobalDict;  
			bDictAllocated = false;
			// J.U. moved down      bGlobalDictAllocated = false;
		}  
		pGlobalDict = NULL;
		bGlobalDictAllocated = false;	 // J.U.
		// stevev 14mar08-this memory no longer belongs to us.  someone else must delete it
		// stevev 30jan08-for now, dealloc with dictionary since it'll be alloc'd with the dict
			//if(pLitStringTable)
			//{delete pLitStringTable;pLitStringTable=NULL;}
		pLitStringTable=NULL;
	}

//	CleanGlobals();

	/* stevev 20may13 - remove memory leaks for a bunch of tables */
	FLAT_BLOCK_DIR_6 *pflatBlkDir = &block_dir_6;

	{
		BLK_ITEM_TBL* pFlatItemTbl = &(pflatBlkDir->blk_item_tbl);
		if ( pFlatItemTbl->count && pFlatItemTbl->list )
		{
			delete[] pFlatItemTbl->list;
			pFlatItemTbl->count = 0;
			pFlatItemTbl->list  = NULL;
		}
	}

	{	
		BLK_ITEM_NAME_TBL* pFlatItemTbl = &(pflatBlkDir->blk_item_name_tbl);
		if ( pFlatItemTbl->count && pFlatItemTbl->list )
		{
			delete[] pFlatItemTbl->list;
			pFlatItemTbl->count = 0;
			pFlatItemTbl->list  = NULL;
		}
	}
	
	{
		PARAM_TBL* pFlatItemTbl = &(pflatBlkDir->param_tbl);
		if ( pFlatItemTbl->count && pFlatItemTbl->list )
		{
			delete[] pFlatItemTbl->list;
			pFlatItemTbl->count = 0;
			pFlatItemTbl->list  = NULL;
		}
	}

	{
		REL_TBL* pFlatItemTbl = &(pflatBlkDir->rel_tbl);
		if ( pFlatItemTbl->count && pFlatItemTbl->list )
		{
			delete[] pFlatItemTbl->list;
			pFlatItemTbl->count = 0;
			pFlatItemTbl->list  = NULL;
		}
	}

	{
		UPDATE_TBL* pFlatItemTbl = &(pflatBlkDir->update_tbl);
		if ( pFlatItemTbl->count && pFlatItemTbl->list )
		{
			delete[] pFlatItemTbl->list;
			pFlatItemTbl->count = 0;
			pFlatItemTbl->list  = NULL;
		}
	}

	if (CodingMajor == DDOD_REV_SUPPORTED_EIGHT)
	{							
		COMMAND_TBL_8* pFlatItemTbl = &(pflatBlkDir->command_to_var_tbl);
		if ( pFlatItemTbl->count && pFlatItemTbl->list )
		{
			for (int y = 0; y < pFlatItemTbl->count; y++)
			{
				// read list
				if ( pFlatItemTbl->list[y].rd_count && pFlatItemTbl->list[y].rd_list )
				{	// index list
					for (int z = 0; z < pFlatItemTbl->list[y].rd_count; z++)
					{
						if ( pFlatItemTbl->list[y].rd_list[z].count && 
							 pFlatItemTbl->list[y].rd_list[z].index_list )
						{
							delete[] pFlatItemTbl->list[y].rd_list[z].index_list;
							pFlatItemTbl->list[y].rd_list[z].index_list = NULL;
							pFlatItemTbl->list[y].rd_list[z].count      = 0;
						}
					}
					delete[] pFlatItemTbl->list[y].rd_list;
					pFlatItemTbl->list[y].rd_list = NULL;
					pFlatItemTbl->list[y].rd_count= 0;
				}
				// write list
				if ( pFlatItemTbl->list[y].wr_count && pFlatItemTbl->list[y].wr_list )
				{	// index list
					// write list should use write count - repaired 10jan14 from DD@fluke
					for (int z = 0; z < pFlatItemTbl->list[y].wr_count; z++)
					{
						if ( pFlatItemTbl->list[y].wr_list[z].count && 
							 pFlatItemTbl->list[y].wr_list[z].index_list )
						{
							delete[] pFlatItemTbl->list[y].wr_list[z].index_list;
							pFlatItemTbl->list[y].wr_list[z].index_list = NULL;
							pFlatItemTbl->list[y].wr_list[z].count      = 0;
						}
					}
					delete[] pFlatItemTbl->list[y].wr_list;
					pFlatItemTbl->list[y].wr_list = NULL;
					pFlatItemTbl->list[y].wr_count= 0;
				}
			}
			delete[] pFlatItemTbl->list;
			pFlatItemTbl->count = 0;
			pFlatItemTbl->list  = NULL;
		}
	}
	else
	{
		COMMAND_TBL* pFlatItemTbl = &(pflatBlkDir->command_tbl);
		if ( pFlatItemTbl->count && pFlatItemTbl->list )
		{
			for (int y = 0; y < pFlatItemTbl->count; y++)
			{
				if ( pFlatItemTbl->list[y].count && pFlatItemTbl->list[y].index_list )
				{
					delete[] pFlatItemTbl->list[y].index_list;
					pFlatItemTbl->list[y].index_list = NULL;
					pFlatItemTbl->list[y].count      = 0;
				}
			}
			delete[] pFlatItemTbl->list;
			pFlatItemTbl->count = 0;
			pFlatItemTbl->list  = NULL;
		}
	}

	{
		CRIT_PARAM_TBL* pFlatItemTbl = &(pflatBlkDir->crit_param_tbl);
		if ( pFlatItemTbl->count && pFlatItemTbl->list )
		{
			delete[] pFlatItemTbl->list;
			pFlatItemTbl->count = 0;
			pFlatItemTbl->list  = NULL;
		}
	}

}



bool DDlDevDescription :: LoadDictionary(char *pchLanguageCode)
{
	int iRetVal;
	
	/*Vibhor 180105: Start of Code*/

	/*Modifications for loading E&H dictionary*/
	/* 7aug07 - mods for loading the Siemens dictionary */

	
#ifdef MODIFIED_BY_SS
	//char dictfile[255],draegerfile[255],mmifile[255],EnHfile[255], Siemens[255];
	/*strcpy(dictfile,dictfilepath);
	strcat(dictfile,"standard.dct");
	strcpy(draegerfile,dictfilepath);
	strcat(draegerfile,"draeger.dct");
	strcpy(mmifile,dictfilepath);
	strcat(mmifile,"mmi.dct");
	strcpy(EnHfile,dictfilepath);
	strcat(EnHfile,"endress_hauser.dct");
	strcpy(Siemens,dictfilepath);
	strcat(Siemens,"siemens.dct");*/
	std::string dictfile = std::string(dictfilepath) + "standard.dct";
	std::string draegerfile = std::string(dictfilepath) + "draeger.dct";
	std::string mmifile = std::string(dictfilepath) + "mmi.dct";
	std::string EnHfile = std::string(dictfilepath) + "endress_hauser.dct";
	std::string Siemens = std::string(dictfilepath) + "siemens.dct";

	//char* chDictionaryExtensions[]
	//	= {
	//		draegerfile,
	//		mmifile,
	//		EnHfile,
	//		Siemens,	// end 7aug7 
	//			NULL
	//};
	char* chDictionaryExtensions[] = {
	   &draegerfile[0],
	   &mmifile[0],
	   &EnHfile[0],
	   &Siemens[0], // end 7aug7
	   NULL
	};

#endif 
	/*Assumption: The standard dictionary should be in the same location as the execuatble*/

	
	
	/*Vibhor 180105: End of Code*/

	pGlobalDict      = new CDictionary(pchLanguageCode);
	// this MUST be passed in...(it belongs to the device) pLitStringTable  = new LitStringTable();
	if (pGlobalDict != NULL)	// J.U. routine check
	{							
	iRetVal = pGlobalDict->makedict((char *)dictfile.c_str(), chDictionaryExtensions);

	if(iRetVal != SUCCESS)
	{
		/*Log an error in the error file in the calling routine */
			LOGIT(CERR_LOG|UI_LOG,"\n ERROR: makeDictionary failed.\n");
			// J.U. may be have to delete and NULL pGlobalDict or set bGlobalDictAllocated
			delete pGlobalDict;	// J.U. delete created pGlobalDict 
			pGlobalDict = NULL; // J.U. null pGlobalDict
		return false;
	}

	bDictAllocated = true; /*We have allocated the dictionary , so we own the responsiblty to delete it too!!*/
	bGlobalDictAllocated = true;

	return true;
	}
	else 
	{
		return false;	// J.U. if new pGlobal dictionari is not created
	}




}/*End LoadDictionary */


void DDlDevDescription::ClearArrays()
{
	unsigned i;
	for ( i = 0; i < MAX_SOD ;i++)
	{
		pbyExtensions[i]  = NULL;// assumed random or already deleted
		
		pbyObjectValue[i] = NULL;
	}
}


void DDlDevDescription::CleanArrays()
{
	unsigned i;
	for ( i = 0; i < MAX_SOD ;i++)
	{
		if(pbyExtensions[i] != NULL)
		{
			delete[] pbyExtensions[i];
			pbyExtensions[i] = NULL;
		}
		
		if(pbyObjectValue[i] != NULL)
		{
			delete[] pbyObjectValue[i];
			pbyObjectValue[i] = NULL;
		}
	}
}


void DDlDevDescription ::CleanGlobals()
{
	unsigned i;

	CleanArrays();
/*
	if(bDictAllocated == true && bGlobalDictAllocated == true)
	{
		if(pGlobalDict)
			delete pGlobalDict;
		bDictAllocated = false;
	}

*/

//LOGIT(CLOG_LOG,"Clean Pt 002.\n");
	if(bDevDirAllocated == true)
	{
		if(device_dir.string_tbl.count)
		{
			for(i = 0; i < ((unsigned)device_dir.string_tbl.count); i++)
			{
				//delete [] device_dir.string_tbl.list[i].str;
				device_dir.string_tbl.list[i].Cleanup();
			}

			delete[] device_dir.string_tbl.list;
				
			if(device_dir.dict_ref_tbl.count)
				delete [] device_dir.dict_ref_tbl.list;
			device_dir.dict_ref_tbl.count = 0; // stevev 08apr13
			
		}
	}
	if(bDevDir6Allocated == true)
	{
		if(device_dir_6.string_tbl.count)
		{
			for(i = 0; i < ((unsigned)device_dir_6.string_tbl.count); i++)
			{	// tmp sjv 11jan06  delete [] device_dir_6.string_tbl.list[i].str;
				
				device_dir_6.string_tbl.list[i].Cleanup();// tmp sjv 11jan06
				device_dir_6.string_tbl.list[i].str = NULL;// tmp sjv 11jan06
			}

			delete[] device_dir_6.string_tbl.list;
			device_dir_6.string_tbl.list  = NULL;
			device_dir_6.string_tbl.count = 0; // stevev 08apr13
				
			if (device_dir_6.dict_ref_tbl.name==NULL && device_dir_6.dict_ref_tbl.text==NULL)
			{ // this is from an fm6 and is references only
			}
			else
			{
				//stevev 28sep11	if(device_dir_6.dict_ref_tbl.count)
				//this won't work!!!	delete [] device_dir_6.dict_ref_tbl.list;
				for(i = 0; i < ((unsigned)device_dir_6.dict_ref_tbl.count); i++)
				{
					device_dir_6.dict_ref_tbl.name[i].Cleanup();
					device_dir_6.dict_ref_tbl.text[i].Cleanup();
				}
			   delete [] device_dir_6.dict_ref_tbl.name; device_dir_6.dict_ref_tbl.name = NULL;
			   delete [] device_dir_6.dict_ref_tbl.text; device_dir_6.dict_ref_tbl.text = NULL;
			}
			delete [] device_dir_6.dict_ref_tbl.list; device_dir_6.dict_ref_tbl.list = NULL;
			device_dir_6.dict_ref_tbl.count = 0; // stevev 08apr13
		}
		if (device_dir_6.item_tbl.list)
		{
			delete [] device_dir_6.item_tbl.list;  device_dir_6.item_tbl.list = NULL;
			device_dir_6.item_tbl.count = 0;
		}
		if (device_dir_6.cmd_num_id_tbl.list)
		{
		  delete [] device_dir_6.cmd_num_id_tbl.list;  device_dir_6.cmd_num_id_tbl.list = NULL;
		  device_dir_6.cmd_num_id_tbl.count = 0;
		}
		/* blk_tbl, prog_tbl, domain_tbl & local_var_tbl are always empty */
	}

	if ( bSymAllocated == true )
	{
		delete[] symFilePath;
		symFilePath = NULL;  // HOMZ  <<< why not just check if symFilePath is NULL? >>
	}
}


bool DDlDevDescription :: Initialize(char *pchFileName
									 ,char*pchLanguageCode
									 ,CDictionary *pDict
									 ,LitStringTable *pLit
									 )
{
	bool bRetVal;
//	char *tmpDictFilePath;
	BYTE filelen;

	if(NULL == pchFileName )
		return false;
	
//	tmpDictFilePath = strstr(pchFileName,"Release");
//
//	if ( tmpDictFilePath == NULL)
//	{
//		tmpDictFilePath = strstr(pchFileName,"release");
//		if ( tmpDictFilePath == NULL)
//		return false;
//	}
	/*Vibhor 290904: Start of Code*/

	/*
	 With the new tokenizer release, the extension of the binary file is being changed to
	 ".fm6",this is to differentiate with old version of tokenizer, also to allow both
     the versions of DDs to be in the same directory, e.g, for the same source ddl, there
	 will be a ####.fms and ####.fm6
	 This way old hosts will still function normally with the old fms files, whereas the new
	 hosts can use the new fm6 files.
	 Smart hosts should work with both so JIT Parser will take any format...
	 --- 2008 - extended to fm8 ( fm7 was skipped so the tokenizer and the HART spec would 
				have different version numbers )
	*/
#ifdef NEVER__DEBUG  /* code makes no sense except for debugging purposes */
	symFilePath = strstr(pchFileName,".fm8"); // timj 11oct07 new BFF 8 extension
	if(NULL == symFilePath) //Look for the old "fms" extension
	{
		symFilePath = strstr(pchFileName,".fms");
	}
#endif
	/*Vibhor 290904: End of Code*/
/* stevev 20feb07  Note: this requires the strstr() filling in the debug ifdef above to be disabled	*/
	if (symFilePath != NULL)  //// HOMZ - Fix memory leak;  Release resources before allocating new ones...
	{
		delete[] symFilePath;
		symFilePath = NULL;
	}

	//filelen = symFilePath - pchFileName;
	//filelen = pchFileName - symFilePath;
	filelen = strlen(pchFileName) - SYM_EXTN_LEN;
	symFilePath = new char[filelen + SYM_EXTN_LEN + 1];
	
	if (symFilePath != NULL)   // HOMZ - add check for null
	{
#ifdef MODIFIED_BY_SS
		//bSymAllocated = true;
		//memset((char*)symFilePath,0,(filelen + SYM_EXTN_LEN + 1));//ok
		//strncpy(symFilePath,pchFileName,filelen);
		////	symFilePath[filelen] = 0;
		//strcat(symFilePath,".sym");
		//symFilePath[filelen + SYM_EXTN_LEN] ='\0';

					bSymAllocated = true;
			std::string fileName(pchFileName);
			std::string symExtension = ".sym";
			std::copy(fileName.begin(), fileName.begin() + filelen, symFilePath);
			std::copy(symExtension.begin(), symExtension.end(), symFilePath + filelen);
			 symFilePath[filelen + SYM_EXTN_LEN] = '\0';
#endif


	}

// this is done at instantiation
//	strncpy(dictfilepath,pchFileName,tmpDictFilePath - pchFileName + sizeof("Release"));
//	dictfilepath[tmpDictFilePath - pchFileName + sizeof("Release")] = 0;

	/*Checking of file extension ".fms" has to be taken care of by the calling routine*/

	/*Vibhor 201003: */
	/*Now check the Language code for strings & initialize the global Language code
	 appropriately*/
#ifdef MODIFIED_BY_SS
	if(NULL == pchLanguageCode)
	{

		//strcpy(langCode,DEFAULT_LANGUAGE_CODE);
		std::string str(DEFAULT_LANGUAGE_CODE);
		std::copy(str.begin(), str.end(), langCode);
	}
	else
	{
		//strcpy(langCode,pchLanguageCode);
		std::string str(pchLanguageCode);
		std::copy(str.begin(), str.end(), langCode);
	}
#endif
	/*Even before opening the fms file . load the standard dictionary ,
	if that itself fails there's no point in going forward, just log an error & return false*/

	if((NULL == pDict) && (false == bGlobalDictAllocated) && (NULL == pGlobalDict)) //Vibhor 120504: Modified the condition	
	{
		bRetVal = LoadDictionary(langCode);		// Two allocated flags are set

		if(bRetVal != true)
		{
			//cerr<<"\n Standard dictionary loading failed!!!!!\n"<<endl;
			LOGIT(CERR_LOG|UI_LOG,"\n Standard dictionary failed to load.\n");	// two allocated flags are not set
			return false;
		}
	}
	else	// pDict != NULL || bGlobalDictAllocated == true || pGlobDict != NULL
	{
		if(NULL == pGlobalDict)	// this means that pDict != NULL (or bGlobalDictAllocated == true incorrectly)
		{
			pGlobalDict = pDict;
			bDictAllocated = false;
			bGlobalDictAllocated = true;	// J.U.  in order to Null pGlobal Dict 
		}

		// if(NULL == pLitStringTable)
		pLitStringTable = pLit;// this is not ours
	}

	//jad chinese
	pGlobalDict->pchFileName = pchFileName;

	LOGIT(CLOG_LOG, "before fopen");
//#ifdef _WIN32
//
//	fp.open(pchFileName, std::ios::binary); // Open file in binary mode
//
//	if (!fp.is_open()) {
//		// Print error message using LOGIT
//		LOGIT(CLOG_LOG, "- Could not open DD file '%s'\n", pchFileName);
//		LOGIT(CLOG_LOG, "fopen error");
//		return false;
//	}
//	LOGIT(CLOG_LOG, "fopen ok");
//	return true;
//#else
	fopen_s(&fp, pchFileName , "rb");

	if(!fp)
	{
//		printf("Error opening DD file\n");
		LOGIT(CLOG_LOG,"- Could not open DD file '%s'\n",pchFileName);/*sjv 26jan05 - caller logs this*/
		LOGIT(CLOG_LOG, "fopen error");

		return false;
	}
	LOGIT(CLOG_LOG, "fopen ok");

	return true;
//#endif
}


bool DDlDevDescription :: ReadHeader(DDOD_HEADER& hdr, FILE * pF)
{
	int iRetVal;
	BYTE byTempHeader[HEADER_SIZE];

	LOGIT(CLOG_LOG, "before readheader fread");


	iRetVal = fread(byTempHeader, 1, HEADER_SIZE, pF);
	
	LOGIT(CLOG_LOG, "readheader fread ret = %d, header = %d",iRetVal, HEADER_SIZE);



	if (iRetVal != HEADER_SIZE)
	{
		return false;
	}

	hdr.magic_number		= *((unsigned int *) &byTempHeader[MAGIC_NUMBER_OFFSET]);
	hdr.header_size			= *((unsigned int *) &byTempHeader[HEADER_SIZE_OFFSET]);
	hdr.objects_size		= *((unsigned int *) &byTempHeader[OBJECTS_SIZE_OFFSET]);
	hdr.data_size			= *((unsigned int *) &byTempHeader[DATA_SIZE_OFFSET]);
	hdr.byManufacturer[0] = byTempHeader[MANUFACTURER_OFFSET];
	hdr.byManufacturer[1] = byTempHeader[MANUFACTURER_OFFSET+1];
	hdr.byManufacturer[2] = byTempHeader[MANUFACTURER_OFFSET+2];
	hdr.device_type			= *((unsigned short*) &byTempHeader[DEVICE_TYPE_OFFSET]);
	hdr.device_revision   = byTempHeader[DEVICE_REV_OFFSET];
	hdr.dd_revision       = byTempHeader[DD_REV_OFFSET];
	hdr.tok_rev_major     = byTempHeader[TOKENIZER_MAJOR_REV_OFFSET];
	hdr.tok_rev_minor     = byTempHeader[TOKENIZER_MINOR_REV_OFFSET];
	hdr.reserved1			= *((unsigned short*) &byTempHeader[RESERVED1_OFFSET]);
	hdr.fileSignature		= *((unsigned int *)  &byTempHeader[SIGNATURE_OFFSET]);;
	hdr.reserved3;
	hdr.reserved4;

	//jad debug
	LOGIT(CLOG_LOG, "hdr.magic_number = %d", hdr.magic_number);
	LOGIT(CLOG_LOG, "hdr.header_size = %d", hdr.header_size);
	LOGIT(CLOG_LOG, "hdr.objects_size = %d", hdr.objects_size);
	LOGIT(CLOG_LOG, "hdr.data_size = %d", hdr.data_size);
	LOGIT(CLOG_LOG, "hdr.byManufacturer = %d, %d, %d", hdr.byManufacturer[0], hdr.byManufacturer[1], hdr.byManufacturer[2]);
	LOGIT(CLOG_LOG, "hdr.device_type = %d", hdr.device_type);
	LOGIT(CLOG_LOG, "hdr.device_revision = %d", hdr.device_revision);
	LOGIT(CLOG_LOG, "hdr.dd_revision = %d", hdr.dd_revision);
	LOGIT(CLOG_LOG, "hdr.tok_rev_major = %d", hdr.tok_rev_major);
	LOGIT(CLOG_LOG, "hdr.tok_rev_minor = %d", hdr.tok_rev_minor);

	LOGIT(CLOG_LOG, "byTempHeader[0] = %d", byTempHeader[0]);
	LOGIT(CLOG_LOG, "byTempHeader[1] = %d", byTempHeader[1]);
	LOGIT(CLOG_LOG, "byTempHeader[2] = %d", byTempHeader[2]);
	LOGIT(CLOG_LOG, "byTempHeader[3] = %d", byTempHeader[3]);
	LOGIT(CLOG_LOG, "byTempHeader[4] = %d", byTempHeader[4]);
	LOGIT(CLOG_LOG, "byTempHeader[5] = %d", byTempHeader[5]);
	LOGIT(CLOG_LOG, "byTempHeader[6] = %d", byTempHeader[6]);
	LOGIT(CLOG_LOG, "byTempHeader[7] = %d", byTempHeader[7]);
	LOGIT(CLOG_LOG, "byTempHeader[8] = %d", byTempHeader[8]);
	LOGIT(CLOG_LOG, "byTempHeader[9] = %d", byTempHeader[9]);
	

	return true;
}

#ifdef _DEBUG
bool DDlDevDescription :: LogHeader()
{
	LOGIT(COUT_LOG,"Header Values  size = 0x%x\n",HEADER_SIZE);
	LOGIT(COUT_LOG," Offset  Field         Value\n");
	LOGIT(COUT_LOG,"%08x Magic Number:  0x%04x\n",MAGIC_NUMBER_OFFSET,header.magic_number);
	LOGIT(COUT_LOG,"%08x  Header Size:  0x%04x\n",HEADER_SIZE_OFFSET, header.header_size);
	LOGIT(COUT_LOG,"%08x Objects Size:  0x%04x\n",OBJECTS_SIZE_OFFSET,header.objects_size);
	LOGIT(COUT_LOG,"%08x    Data Size:  0x%04x\n",DATA_SIZE_OFFSET,header.data_size);
	LOGIT(COUT_LOG,"%08x Manufacturer:  0x%02x%02x%02x\n",MANUFACTURER_OFFSET,
		header.byManufacturer[0],header.byManufacturer[1],header.byManufacturer[2]);
	LOGIT(COUT_LOG,"%08x  Device Type:  0x%04x\n",DEVICE_TYPE_OFFSET,header.device_type);
	LOGIT(COUT_LOG,"%08x Dev Revision:  0x%04x\n",DEVICE_REV_OFFSET,header.device_revision);
	LOGIT(COUT_LOG,"%08x  DD Revision:  0x%04x\n",DD_REV_OFFSET,header.dd_revision);
	LOGIT(COUT_LOG,"%08x    Tok Major:  0x%04x\n",DD_REV_OFFSET + 1,header.tok_rev_major);
	LOGIT(COUT_LOG,"%08x    Tok Minor:  0x%04x\n",DD_REV_OFFSET + 2,header.tok_rev_minor);
	LOGIT(COUT_LOG,"\n\n");

	return true;
}
#endif

bool DDlDevDescription ::ValidateHeader(DDOD_HEADER *pDDODheader)
{

	DWORD dwMagicNumber;
	read_dword(&dwMagicNumber, &(pDDODheader->magic_number), FORMAT_BIG_ENDIAN);
	if (dwMagicNumber != 0x7F3F5F77L)
	{
		LOGIT(CERR_LOG, "dwMagicNumber = %d", dwMagicNumber);
		return false;
	}
	pDDODheader->magic_number = dwMagicNumber;

	DWORD dwHeaderSize;
	read_dword(&dwHeaderSize, &(pDDODheader->header_size ), FORMAT_BIG_ENDIAN);
	if (dwHeaderSize < HEADER_SIZE)
	{
		LOGIT(CERR_LOG, "dwHeaderSize = %d, %d", dwHeaderSize, HEADER_SIZE);
		return false;
	}
	pDDODheader->header_size = dwHeaderSize;

	/* size of the Objects section */
	DWORD dwDDObjectsSectionSize;
	read_dword(&dwDDObjectsSectionSize, &(pDDODheader->objects_size ), FORMAT_BIG_ENDIAN);


#ifdef _PARSER_DEBUG

	cout<<"Objects Section size: "<<dwDDObjectsSectionSize<<"BYTES \n";

#endif

	pDDODheader->objects_size = dwDDObjectsSectionSize;

	/* size of the Objects Data section */
	DWORD dwDDObjectsDataSectionSize;
	read_dword(&dwDDObjectsDataSectionSize, &(pDDODheader->data_size ), FORMAT_BIG_ENDIAN);

#ifdef _PARSER_DEBUG

	cout<<"Objects Data Section size: "<<dwDDObjectsDataSectionSize<<"BYTES \n";

#endif

	pDDODheader->data_size = dwDDObjectsDataSectionSize;

	/*Add code to read the Manufacturer */


	DWORD dwMfg;
	read_dword_spl(&dwMfg,&(pDDODheader->byManufacturer),MFG_ID_SIZE,FORMAT_BIG_ENDIAN);

	devID.ulMfgID = dwMfg;

	LOGIF(LOGP_MISC_CMTS)(COUT_LOG,"Manufacturer: %02x\n",devID.ulMfgID);

	/* Device Type */
	WORD wDeviceType;
	read_word(&wDeviceType, &(pDDODheader->device_type ), FORMAT_BIG_ENDIAN);

	devID.uDevType = wDeviceType;
	
	LOGIF(LOGP_MISC_CMTS)(COUT_LOG,"Device Type: %02x\n",devID.uDevType);

	pDDODheader->device_type = wDeviceType;
	/*Add code to read the DevRev & DDRev too */

	devID.byDevRev = pDDODheader->device_revision;
	devID.byDDRev	 = pDDODheader->dd_revision;


	LOGIF(LOGP_MISC_CMTS)(COUT_LOG,"Device Revision: %02x\n",(int)devID.byDevRev);


	LOGIF(LOGP_MISC_CMTS)(COUT_LOG,"DD Revision: %02x\n",(int)devID.byDDRev);

	/* TODO!!! : It would be a good check to verify the Device ID against the
	  Mfg , Devtype, DevRev & DDRev obtained from the DD Name */

	cout.flush();

	return true;
}

bool DDlDevDescription ::ReadObjectDescription()
{
	int iRetVal;
	BYTE byTmpBuf[DD_ODES_SIZE];

	//LOGIT(CLOG_LOG, "In ReadObjectDescription");

	iRetVal = fread(byTmpBuf, 1, DD_ODES_SIZE, fp);

	if (iRetVal != DD_ODES_SIZE)
	{
		return false;
	}
	
	descriptor.index = *((object_index *) &byTmpBuf[DD_ODES_INDEX_OFFSET]);
	descriptor.ram_rom_flag = byTmpBuf[RAM_ROM_FLAG_OFFSET];
	descriptor.name_length = byTmpBuf[NAME_LENGTH_OFFSET];
	descriptor.access_protection_flag = byTmpBuf[ACCESS_PROT_FLAG_OFFSET];
	descriptor.version = *((unsigned short *) &byTmpBuf[VERSION_OFFSET]);
	descriptor.local_address_odes = *((long_offset *) &byTmpBuf[LOC_ADDR_ODES_OFFSET]);
	descriptor.STOD_length = *((unsigned short *) &byTmpBuf[STOD_LENGTH_OFFSET]);
	descriptor.local_address_stod = *((long_offset *) &byTmpBuf[LOC_ADDR_STOD_OFFSET]);
	descriptor.sod_first_index = *((object_index *) &byTmpBuf[SOD_FIRST_INDX_OFFSET]);
	descriptor.sod_length = *((unsigned short *) &byTmpBuf[SOD_LENGTH_OFFSET]);
	descriptor.local_address_sod = *((long_offset *) &byTmpBuf[LOC_ADDR_SOD_OFFSET]);
	descriptor.dpod_first_index = *((object_index *) &byTmpBuf[DPOD_FIRST_INDX_OFFSET]);
	descriptor.dpod_length = *((unsigned short *) &byTmpBuf[DPOD_LENGTH_OFFSET]);
	descriptor.local_address_dpod = *((long_offset *) &byTmpBuf[LOC_ADDR_DPOD_OFFSET]);
	descriptor.dvod_first_index = *((object_index *) &byTmpBuf[DVOD_FIRST_INDX_OFFSET]);
	descriptor.dvod_length = *((unsigned short *) &byTmpBuf[DVOD_LENGTH_OFFSET]);
	descriptor.local_address_dvod = *((long_offset *) &byTmpBuf[LOC_ADDR_DVOD_OFFSET]);

	//LOGIT(CLOG_LOG, "descriptor.index = %d", descriptor.index);
	//LOGIT(CLOG_LOG, "descriptor.local_address_dvod = %d", descriptor.local_address_dvod);

	//LOGIT(CLOG_LOG, "ReadObjectDescription end");
	return true;
}


#ifdef _DEBUG
bool DDlDevDescription ::LogOD()
{
	LOGIT(COUT_LOG," ODES size = 0x%x\n",DD_ODES_SIZE);
	LOGIT(COUT_LOG," Offset    Field       Value\n");
	LOGIT(COUT_LOG,"%08x    ODES Desc:  0x%04x\n",DD_ODES_INDEX_OFFSET,descriptor.index);
	LOGIT(COUT_LOG,"%08x  RamRom Flag:  0x%04x\n",RAM_ROM_FLAG_OFFSET, descriptor.ram_rom_flag);
	LOGIT(COUT_LOG,"%08x  Name Length:  0x%04x\n",NAME_LENGTH_OFFSET,descriptor.name_length);
	LOGIT(COUT_LOG,"%08x  Access Prot:  0x%04x\n",ACCESS_PROT_FLAG_OFFSET,descriptor.access_protection_flag);
	LOGIT(COUT_LOG,"%08x      Version:  0x%04x\n",VERSION_OFFSET,descriptor.version);
	LOGIT(COUT_LOG,"%08x ODES Address:  0x%04x\n",LOC_ADDR_ODES_OFFSET,descriptor.local_address_odes);
	LOGIT(COUT_LOG,"%08x  STOD Length:  0x%04x\n",STOD_LENGTH_OFFSET,descriptor.STOD_length);
	LOGIT(COUT_LOG,"%08x STOD Address:  0x%04x\n",LOC_ADDR_STOD_OFFSET,descriptor.local_address_stod);
	LOGIT(COUT_LOG,"%08x SOD 1stIndex:  0x%04x\n",SOD_FIRST_INDX_OFFSET,descriptor.sod_first_index);
	LOGIT(COUT_LOG,"%08x   SOD Length:  0x%04x\n",SOD_LENGTH_OFFSET,descriptor.sod_length);
	LOGIT(COUT_LOG,"%08x  SOD Address:  0x%04x\n",LOC_ADDR_SOD_OFFSET,descriptor.local_address_sod);
	LOGIT(COUT_LOG,"%08x DPOD 1st Idx:  0x%04x\n",DPOD_FIRST_INDX_OFFSET,descriptor.dpod_first_index);
	LOGIT(COUT_LOG,"%08x DPOD  Length:  0x%04x\n",DPOD_LENGTH_OFFSET,descriptor.dpod_length);
	LOGIT(COUT_LOG,"%08x DPOD Address:  0x%04x\n",LOC_ADDR_DPOD_OFFSET,descriptor.local_address_dpod);
	LOGIT(COUT_LOG,"%08x DVOD 1st Idx:  0x%04x\n",DVOD_FIRST_INDX_OFFSET,descriptor.dvod_first_index);
	LOGIT(COUT_LOG,"%08x DVOD  Length:  0x%04x\n",DVOD_LENGTH_OFFSET,descriptor.dvod_length);
	LOGIT(COUT_LOG,"%08x DVOD Address:  0x%04x\n",LOC_ADDR_DVOD_OFFSET,descriptor.local_address_dvod);
	LOGIT(COUT_LOG,"\n\n");

	return true;
}
#endif

bool DDlDevDescription ::ValidateObjectDescription(DD_ODES * pDescriptor)
{
	WORD wVersion;
	read_word(&wVersion, &(pDescriptor->version), FORMAT_BIG_ENDIAN);
	pDescriptor->version = wVersion;


	WORD wFirstIndex;
	read_word(&wFirstIndex, &(pDescriptor->sod_first_index), FORMAT_BIG_ENDIAN);
	if (wFirstIndex != 100)
	{
		return false;
	}
	pDescriptor->sod_first_index = wFirstIndex;

	WORD wSODLength;// a ushort
	read_word(&wSODLength, &(pDescriptor->sod_length), FORMAT_BIG_ENDIAN);
	pDescriptor->sod_length = wSODLength;

	if (wSODLength >= MAX_SOD)// ste at x4000 well short of an unsigned short
	{// tell everybody
		LOGIT(CERR_LOG|CLOG_LOG|UI_LOG,"Failure: Too many SOD items in the DD binary.(%d)\n",
			pDescriptor->sod_length);
		return false;
	}

#ifdef _PARSER_DEBUG
	cout<<"Number of SOD Objects: "<<wSODLength<<endl;
#endif

	DWORD wLocalAddressSOD;
	read_dword(&wLocalAddressSOD, &(pDescriptor->local_address_sod), FORMAT_BIG_ENDIAN);
	pDescriptor->local_address_sod = wLocalAddressSOD;


#ifdef _PARSER_DEBUG
	
	cout<<"Local Address SOD: "<<hex<<wLocalAddressSOD<<dec<<endl;
#endif

	return true;

}


bool DDlDevDescription ::ReadSOD()
{
	int iRetVal;
	BYTE byTmpBuf[DOMAIN_FIXED_SIZE];

	//LOGIT(CLOG_LOG, "in ReadSOD");


	for (int i = 0; i < descriptor.sod_length;i++)
	{
		unsigned      uVal = 0;
		BYTE		  byLen = 0;
		iRetVal = fread(byTmpBuf, 1,DOMAIN_FIXED_SIZE, fp);
		if (iRetVal != DOMAIN_FIXED_SIZE)
		{
			return false;
		}
			
		ObjectFixed[i].index = *((object_index *) &byTmpBuf[DOM_FIXED_INDX_OFFSET]);
		ObjectFixed[i].byObjectCode = byTmpBuf[OBJ_CODE_OFFSET];
		ObjectFixed[i].wDomainDataSize = *((WORD *) &byTmpBuf[DOM_DATA_SIZE_OFFSET]);
		ObjectFixed[i].longAddress = *((long_offset *) &byTmpBuf[LONG_ADDR_OFFSET]);
		ObjectFixed[i].byDomainState = byTmpBuf[DOM_STATE_OFFSET];
		ObjectFixed[i].byUploadState = byTmpBuf[UPLOAD_STATE_OFFSET];
		ObjectFixed[i].chCounter = (char) byTmpBuf[COUNTER_OFFSET];

		ValidateFixed(&ObjectFixed[i]);


/*Vibhor 310804: Start of Code*/
/*
		iRetVal = fread((BYTE *)&byExtLengths[i], 1
									, sizeof(BYTE), fp);
	In new Binary File format, the Extension Length is a 
	multibyte INTEGER instead of unsigned8, so commenting 
	out this one. Unsigned8 would still be parsed as a spl
	case of new format.
*/	
		//Read the integer and its length !!
		iRetVal = fread((BYTE *)&byExtLengths[i], 1 , sizeof(BYTE), fp);

		//LOGIT(CLOG_LOG, "iRetVal1 = %d", iRetVal);
//		iRetVal = ReadIntegerValueFromFile(fp,uVal,byLen); 
		
	/*	if(DDL_SUCCESS != iRetVal)
		{
			cerr <<"Reading a (multibyte) integer from file failed !!!" << endl;
			return false;
		}
	*/
//		byExtLengths[i] = uVal;
		long lSeekLcn = -1;//-byLen;
		if (byExtLengths[i] > 0)
		{
			pbyExtensions[i] = new BYTE[byExtLengths[i]+1];
			iRetVal = fseek(fp,lSeekLcn,SEEK_CUR);
			//LOGIT(CLOG_LOG, "iRetVal2 = %d", iRetVal);
			iRetVal = fread((BYTE *)pbyExtensions[i],1,
										byExtLengths[i] +1 , fp);
			//LOGIT(CLOG_LOG, "iRetVal3 = %d", iRetVal);
#ifdef _PARSER_DEBUG
		cout<<"Objcet: "<<i<<"\tLength: "<<(int)byExtLengths[i]<<"\tCode: "<<(int)(pbyExtensions[i])[1]<<endl;;
#endif
		}//Vibhor 310804: End of Code (change)
		else
		{

#ifdef _PARSER_DEBUG
			cout<<"Object "<<i<<"has no data \n";
#endif

			//LOGIT(CLOG_LOG, "object has no data");

		}


	}

	//LOGIT(CLOG_LOG, "exit ReadSOD");

	return true;
}

#ifdef x_DEBUG
bool DDlDevDescription ::LogSOD()
{
	
	LOGIT(COUT_LOG," SOD with 0x%x items\n",descriptor.sod_length);
	LOGIT(COUT_LOG," Offset    Field       Value\n");
	for (int i = 0; i < descriptor.sod_length;i++)
	{
		LOGIT(COUT_LOG,"           SOD # 0x%04x\n",i);	
		LOGIT(COUT_LOG,"%08x Object Index:  0x%04x\n",DOM_FIXED_INDX_OFFSET,ObjectFixed[i].index);
		LOGIT(COUT_LOG,"%08x Object  Code:  0x%04x\n",OBJ_CODE_OFFSET,      ObjectFixed[i].byObjectCode);	
		LOGIT(COUT_LOG,"%08x    Max Bytes:  0x%04x\n",DOM_DATA_SIZE_OFFSET, ObjectFixed[i].wDomainDataSize);	
		LOGIT(COUT_LOG,"%08x Local Addres:  0x%04x\n",LONG_ADDR_OFFSET,     ObjectFixed[i].longAddress);	
		LOGIT(COUT_LOG,"%08x Domain State:  0x%04x\n",DOM_STATE_OFFSET,     ObjectFixed[i].byDomainState);	
		LOGIT(COUT_LOG,"%08x Upload State:  0x%04x\n",UPLOAD_STATE_OFFSET,  ObjectFixed[i].byUploadState);	
		LOGIT(COUT_LOG,"%08x      Counter:  0x%04x\n",COUNTER_OFFSET,       ObjectFixed[i].chCounter);
		
		LOGIT(COUT_LOG,"        Extension Length:  0x%04x\n",byExtLengths[i]);
		if (byExtLengths[i] > 0)
		{
			;// for (int k = 0, k < byExtLengths[i]; k++){ output pbyExtensions[i]}
		}
	}
	return true;
}
#endif

bool DDlDevDescription ::ValidateFixed(DOMAIN_FIXED *pDomainFixed)
{

	WORD index;
	read_word(&index, &(pDomainFixed->index), FORMAT_BIG_ENDIAN);
	pDomainFixed->index = index;

	WORD wDomainDataSize;
	read_word(&wDomainDataSize, &(pDomainFixed->wDomainDataSize), FORMAT_BIG_ENDIAN);
	pDomainFixed->wDomainDataSize = wDomainDataSize;

	DWORD longAddress;
	read_dword(&longAddress, &(pDomainFixed->longAddress), FORMAT_BIG_ENDIAN);
	pDomainFixed->longAddress = longAddress;

	return true;
}

int _preFetchItem(BYTE maskSize, BYTE** pObjExtn, int& rSize, unsigned long &_attrMask);

//unsigned int DDlDevDescription ::GetSize(unsigned int domainsize)
unsigned int DDlDevDescription ::GetSize(int i)
{
	unsigned int size = ObjectFixed[i].wDomainDataSize;

	if (size < 0xffff)
		return size;

	size = 0;
		
	BYTE* pLocExtn = pbyExtensions[i];
	int rSize;
	unsigned long attrMask;
	
	_preFetchItem(4 /* mask size always 4 */, &pLocExtn, rSize, attrMask);
	
	
	unsigned char  *obj_attr_ptr = pLocExtn;			/* pointer to attributes in object extension */
	unsigned char	extn_attr_length = rSize;	/*length of Extn data in external obj*/
	unsigned long   local_req_mask = attrMask;			/* request mask for base or external* objects */
	

	while ((obj_attr_ptr < (pLocExtn + extn_attr_length)) && local_req_mask) 
	{	
		unsigned short  curr_attr_RI;	/* RI for current attribute */
		unsigned short  curr_attr_tag;	/* tag for current attribute */
		unsigned long   curr_attr_length;	/* data length for current attribute */
	
		parse_attribute_id(&obj_attr_ptr, &curr_attr_RI, &curr_attr_tag, &curr_attr_length);
				
		unsigned long attr_mask_bit = (unsigned long) (1L << curr_attr_tag);

		local_req_mask &= ~attr_mask_bit;	// clear the bit in local mask
		
		// increment ptr
		if (curr_attr_RI == RI_LOCAL)
		{
			size += curr_attr_length;
			// consume the trailing offset integer
			unsigned long attr_offset = 0L;
			do {
				if (attr_offset & MAX_LENGTH_MASK) { 
					return (FETCH_ATTR_LENGTH_OVERFLOW);
				}
				attr_offset = (attr_offset << LENGTH_SHIFT) |	      
					(unsigned long) (LENGTH_MASK & *obj_attr_ptr);
			} while (LENGTH_ENCODE_MASK & *obj_attr_ptr++);
		}
		else
		if (curr_attr_RI == RI_IMMEDIATE)
		{// increment past the trailing attribute
			obj_attr_ptr += curr_attr_length;
		}
		else
		{// unknown RI type - we are out of sync
			LOGIT(CERR_LOG,"ERROR: reading DD. GetSize() read an unknown RI type.\n");
			return size;
		}

	}
	
//	if (error)
//		return 0;
//	else
		return size;

	
}

bool DDlDevDescription ::ReadObjectValues()
{
	int iRetVal;

	//LOGIT(CLOG_LOG, "in ReadObjectValues");
	//LOGIT(CLOG_LOG, "descriptor.sod_length = %d", descriptor.sod_length);

	for (int i = 0; i < descriptor.sod_length;i++)
	{
		if(!(ObjectFixed[i].wDomainDataSize))
		{
			pbyObjectValue[i] = NULL;
			continue;
		}
		if (ObjectFixed[i].longAddress == 0xffffffff)
		{
			pbyObjectValue[i] = NULL;
			continue;
		}
		if (byExtLengths[i] == 0)
		{
			pbyObjectValue[i] = NULL;
			continue;
		}
#ifdef LOG_READ
		LOGIT(COUT_LOG,"            ROV # 0x%04x\n",i);
		LOGIT(COUT_LOG,"        Object Index':  0x%04x\n", ObjectFixed[i].index);	
		LOGIT(COUT_LOG,"           Max Bytes':  0x%04x\n", ObjectFixed[i].wDomainDataSize);	
		LOGIT(COUT_LOG,"        Local Addres':  0x%04x\n", ObjectFixed[i].longAddress);	
#endif
		long offset = ObjectFixed[i].longAddress
						+ header.header_size + header.objects_size;

		iRetVal = fseek(fp, (long)offset, 0);
		if (iRetVal < 0)
		{
			pbyObjectValue[i] = NULL;
			continue;
		}
		/*Vibhor 010904: Start of Code*/


		if( ObjectFixed[i].wDomainDataSize >= 0xffff && i < descriptor.sod_first_index)
		{	
			/*It should not come here for any object other than Device & Block Directory*/
			pbyObjectValue[i] = NULL;

			// find the correct size

		#ifdef _PARSER_DEBUG

			if(pbyExtensions[i][1] == DEVICE_DIR_TYPE)
			{
				cout<< "Device Directory has max-bytes(wDomainDataSize) > 0xffff"<<endl;
			}
			else if(pbyExtensions[i][1] == BLOCK_DIR_TYPE)
			{
				cout<< "Block Directory has max-bytes(wDomainDataSize) > 0xffff"<<endl;
			}
			else
			{
				cerr<<"Object"<< i+1 <<" has max-bytes(wDomainDataSize) > 0xffff !!"<<endl;
			}
			
		#endif

		}
		else
		{
			//size = GetSize(ObjectFixed[i].wDomainDataSize);	
			int size = GetSize(i);	


			pbyObjectValue[i] = new BYTE[size];
/* stevev 15apr11 - use the calculated size for all
			iRetVal = fread((BYTE *)pbyObjectValue[i], 1
									, ObjectFixed[i].wDomainDataSize, fp);
			if (iRetVal != ObjectFixed[i].wDomainDataSize)
**/
			iRetVal = fread((BYTE *)pbyObjectValue[i], 1, size, fp);
			if (iRetVal != size)
			{
				if ( feof(fp) )
				{
					LOGIT(CERR_LOG,L"End of File reached unexpectedly.\n");
				}
				else
				{
					if ( ferror(fp) )
					{
									//perror( "File read failure" );	 PAW 09/04/09 see below
									//fprintf(stderr, "File read failure");// PAW 09/04/09 see below
									LOGIT(CERR_LOG,"File read failure.\n"); // stevev 12aug10
					}
					else
					{
						LOGIT(CERR_LOG,L" Count mismatch without EOF and without a file error.\n");
					}
				}
				return false ;
			}
		}
	}


	//LOGIT(CLOG_LOG, "i = %d", i);
	//LOGIT(CLOG_LOG, "exit ReadObjectValues");
	return true;
}
/*Vibhor 010904: Start of Code*/
//Adding here for better maintaining the sequence for readability
bool DDlDevDescription ::ReadFormatObject()
{
	int iRetVal = 0;
	FORMAT_EXTENSION FmtExt;
	bool bFmtDone = false;
#ifdef _WIN32_WCE
	WORD word_value;		// PAW 29/05/09
	BYTE byte_value;		// PAW 29/05/09
#endif

	//LOGIT(CLOG_LOG, "in ReadFormatObject");


	for (int i = 0; i < descriptor.sod_length;i++)
	{
		iRetVal =0;
		switch (pbyExtensions[i][1])
		{
			case FORMAT_OBJECT_TYPE:
			{
				#ifdef _PARSER_DEBUG
									
									cout<< i+1 <<" Format Object: \n";
				#endif
				BYTE    *pbyPointer = (BYTE *)pbyExtensions[i];

				FmtExt.byLength				= pbyPointer[FMT_EXTN_LENGTH_OFFSET];
				FmtExt.byFormatObjectCode	= pbyPointer[FMT_OBJ_CODE_OFFSET];
				FmtExt.byCodingFormatMajor	= pbyPointer[CODING_FMT_MAJ_OFFSET];
				FmtExt.byCodingFormatMinor	= pbyPointer[CODING_FMT_MIN_OFFSET];
				FmtExt.byDDRevision			= pbyPointer[DDREV_OFFSET];
				FmtExt.pchProfileNumber[0]	= pbyPointer[PROFILE_NO_OFFSET];
				FmtExt.pchProfileNumber[1]	= pbyPointer[PROFILE_NO_OFFSET+1];
#ifndef _WIN32_WCE	// PAW causes datatype alignment error 29/05/09
				FmtExt.wNumberOfImports		= *((WORD *) &pbyPointer[NO_O_IMPORTS_OFFSET ]);
				FmtExt.wNumberOfLikes		= *((WORD *) &pbyPointer[NO_O_LIKES_OFFSET]);
#else
				word_value = 0;
				byte_value = *(&pbyPointer[NO_O_IMPORTS_OFFSET ]);
				word_value += byte_value;			// LSB
				byte_value = *(&pbyPointer[NO_O_IMPORTS_OFFSET+1 ]);
				word_value += (byte_value << 8);	// MSB;
				FmtExt.wNumberOfImports = word_value;

				word_value = 0;
				byte_value = *(&pbyPointer[NO_O_LIKES_OFFSET ]);
				word_value += byte_value;			// LSB
				byte_value = *(&pbyPointer[NO_O_LIKES_OFFSET+1 ]);
				word_value += (byte_value << 8);	// MSB;
				FmtExt.wNumberOfLikes = word_value;
#endif

				if (FmtExt.byFormatObjectCode == 128)
				{
					CodingMajor = FmtExt.byCodingFormatMajor;
					CodingMinor = FmtExt.byCodingFormatMinor & 0x7f;//DDOD_REV_MINOR_HCF | (HCF_TOK_TYPE << CODING_FMT_MINOR_SIZE * 7)
					TokenizerType = FmtExt.byCodingFormatMinor >> 7;
					DDRevision  = FmtExt.byDDRevision;
					if ( false == read_word(&ImpCnt,(void*)&FmtExt.wNumberOfImports,FORMAT_BIG_ENDIAN) )
					{
						return false;
					}
					if ( false == read_word(&LikCnt,(void*)&FmtExt.wNumberOfLikes,FORMAT_BIG_ENDIAN) )
					{
						return false;
					}
				}
				else
				{
					return false;
				}
				bFmtDone = true;
			}/*End Case*/
				break;
		}/*End Switch*/
		if(bFmtDone)
			break; //Break from the loop if we got the format object

	}/*Endfor*/

	//LOGIT(CLOG_LOG, "exit ReadFormatObject");
	return true;


}/*End of ReadFormatObject()*/

/*Vibhor 010904: End of Code*/

bool DDlDevDescription	:: LoadDeviceDirectory()
{
	int iRetVal;
	OBJECT_INDEX objectIndex;
	bool bDevDirLoadedFlag = false;
 
	for (int i = 0; i < descriptor.sod_length;i++)
	{
		iRetVal =0;
		objectIndex = ObjectFixed[i].index;

		switch (pbyExtensions[i][1])
		{
 			case DEVICE_DIR_TYPE:
				{//neither FLAT_DEVICE_DIR nor BIN_DEVICE_DIR have classes; memset ok
					(void)memset((char *) &device_dir, 0, sizeof(FLAT_DEVICE_DIR));
					(void)memset((char *) &bin_dev_dir, 0, sizeof(BIN_DEVICE_DIR));

					FLAT_DEVICE_DIR *flatDevDir = &device_dir;
					BIN_DEVICE_DIR *binDevDir = &bin_dev_dir;
					BININFO *binTablePtr;
					DATAPART_SEGMENT *dirExtnOffset;
					WORD wOffset;
					WORD wTblLength;
					bool bRet;

#ifdef _PARSER_DEBUG
					cout<< i+1 <<" Device Directory Object: \n";
#endif
					DEVICE_DIR_EXT *pDevDirExt;
					BYTE *pbyPointer = (BYTE *)pbyExtensions[i];
//					pDevDirExt = (DEVICE_DIR_EXT *)(pbyPointer);
					
					pDevDirExt = new DEVICE_DIR_EXT;

					pDevDirExt->byLength	= pbyPointer[DEV_DIR_LENGTH_OFFSET];
					pDevDirExt->byDeviceDirObjectCode = pbyPointer[DEV_DIR_OBJ_CODE_OFFSET];
					pDevDirExt->byFormatCode	 = pbyPointer[DEV_DIR_FORMAT_CODE_OFFSET];

					pDevDirExt->BlockNameTable.offset = *((short_offset *) &pbyPointer[BLK_NAME_TBL_OFFSET]);
					pDevDirExt->BlockNameTable.wSize = *((WORD *) &pbyPointer[BLK_NAME_TBL_OFFSET + SEG_SIZE_OFFSET]);
					pDevDirExt->ItemTable.offset = *((short_offset *) &pbyPointer[ITEM_TBL_OFFSET]);
					pDevDirExt->ItemTable.wSize = *((WORD *) &pbyPointer[ITEM_TBL_OFFSET + SEG_SIZE_OFFSET ]);
					pDevDirExt->ProgramTable.offset = *((short_offset *) &pbyPointer[PROG_TBL_OFFSET]);
					pDevDirExt->ProgramTable.wSize = *((WORD *) &pbyPointer[PROG_TBL_OFFSET + SEG_SIZE_OFFSET]);
					pDevDirExt->DomainTable.offset = *((short_offset *) &pbyPointer[DOM_TBL_OFFSET]);
					pDevDirExt->DomainTable.wSize = *((WORD *) &pbyPointer[DOM_TBL_OFFSET + SEG_SIZE_OFFSET]);
					pDevDirExt->StringTable.offset = *((short_offset *) &pbyPointer[STRNG_TBL_OFFSET]);
					pDevDirExt->StringTable.wSize = *((WORD *) &pbyPointer[STRNG_TBL_OFFSET + SEG_SIZE_OFFSET]);
					pDevDirExt->DictReferenceTable.offset = *((short_offset *) &pbyPointer[DICT_REF_TBL_OFFSET]);
					pDevDirExt->DictReferenceTable.wSize = *((WORD *) &pbyPointer[DICT_REF_TBL_OFFSET + SEG_SIZE_OFFSET]);
					pDevDirExt->LocalVariableTable.offset = *((short_offset *) &pbyPointer[LOC_VAR_TBL_OFFSET]);
					pDevDirExt->LocalVariableTable.wSize = *((WORD *) &pbyPointer[LOC_VAR_TBL_OFFSET + SEG_SIZE_OFFSET]);
					pDevDirExt->CommandTable.offset = *((short_offset *) &pbyPointer[CMD_TBL_OFFSET]);
					pDevDirExt->CommandTable.wSize = *((WORD *) &pbyPointer[CMD_TBL_OFFSET + SEG_SIZE_OFFSET]);

					/*Do Some Validations*/
					if(pDevDirExt->byLength < DEVICE_DIR_LENGTH) //Vibhor 280904: Changed
					{
						delete pDevDirExt;
						return false; /* INVALID_EXTN_LENGTH*/
					}

					if(pDevDirExt->byDeviceDirObjectCode != DEVICE_DIR_TYPE)
					{
						delete pDevDirExt;
						return false; /* DIR_TYPE_MISMATCH*/
					}

					if(!(binDevDir->bin_exists))
					{
						binDevDir->bin_exists = 0L;

						//if(pDevDirExt->BlockNameTable.wSize)
						//    	binDevDir->bin_exists |= (1<<BLK_TBL_ID);
						if(pDevDirExt->ItemTable.wSize)
						    	binDevDir->bin_exists |= (1<<ITEM_TBL_ID);
						//if(pDevDirExt->ProgramTable.wSize)
						//    	binDevDir->bin_exists |= (1<<PROG_TBL_ID);
						//if(pDevDirExt->DomainTable.wSize)
						//    	binDevDir->bin_exists |= (1<<DOMAIN_TBL_ID);
						if(pDevDirExt->StringTable.wSize)
						    	binDevDir->bin_exists |= (1<<STRING_TBL_ID);
						if(pDevDirExt->DictReferenceTable.wSize)
						    	binDevDir->bin_exists |= (1<<DICT_REF_TBL_ID);
						//if(pDevDirExt->LocalVariableTable.wSize)
						//    	binDevDir->bin_exists |= (1<<LOCAL_VAR_TBL_ID);
						if(pDevDirExt->CommandTable.wSize)
						    	binDevDir->bin_exists |= (1<<CMD_NUM_ID_TBL_ID);
					}

					unsigned short uTag = 0;
					unsigned long  ulTableMaskBit = 0L;
					binTablePtr = (BININFO*) 0L;
					unsigned long ulReqMask = DEVICE_TBL_MASKS;

					while ((ulReqMask) && (uTag < MAX_DEVICE_TBL_ID_HCF)) {

					/*
					* Check for request mask bit corresponding to the tag value.
					* Skip to next tag value if not requested.
					*/

					if (!((ulReqMask) & (1L << uTag))) {
					uTag++;
					continue;
					}

					/*
					* Point to appropriate values for the table type
					*/

					switch (uTag++) {

					case BLK_TBL_ID:	/* Block Table */
			/*			ulTableMaskBit = BLK_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->BlockNameTable);
						binTablePtr =&(binDevDir->blk_tbl); */
						break;

					case ITEM_TBL_ID:	/* Item Table */
					{
ulTableMaskBit = ITEM_TBL_MASK;
dirExtnOffset = &(pDevDirExt->ItemTable);
binTablePtr = &(binDevDir->item_tbl); 

ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
if (!(binTablePtr->chunk)) 
{
	bRet = read_word(&wOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);
	if(bRet == false)
	{
		delete pDevDirExt;
		return false;
	}

	bRet = read_word(&wTblLength,(void*)&dirExtnOffset->wSize,FORMAT_BIG_ENDIAN);
	if(bRet == false)
	{
		delete pDevDirExt;
		return false;
	}
	/*
	 * Attach the table if non-zero length, else go
	 * to the next table
	 */
	if (wTblLength) {
		binTablePtr->chunk = pbyObjectValue[i] + wOffset;
		binTablePtr->size = wTblLength;
		binDevDir->bin_hooked |= ulTableMaskBit;
	}
}
					}
					break;

					case PROG_TBL_ID:	/* Program Table */
			/*			ulTableMaskBit = PROG_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->ProgramTable);
						binTablePtr = &(binDevDir->prog_tbl); */
						break;

					case DOMAIN_TBL_ID:	/* Domain Table */
			/*			ulTableMaskBit = DOMAIN_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->DomainTable);
						binTablePtr = &(binDevDir->domain_tbl); */
						break;

					case STRING_TBL_ID:	/* String Table */
					{
						ulTableMaskBit = STRING_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->StringTable);
						binTablePtr = &(binDevDir->string_tbl);

						ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
						if (!(binTablePtr->chunk)) 
						{
							bRet = read_word(&wOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);

							if(bRet == false)
							{
								delete pDevDirExt;
								return false;
							}

							bRet = read_word(&wTblLength,(void*)&dirExtnOffset->wSize,FORMAT_BIG_ENDIAN);
							if(bRet == false)
							{
								delete pDevDirExt;
								return false;
							}

							/*
							 * Attach the table if non-zero length, else go
							 * to the next table
							 */

							if (wTblLength) {
								binTablePtr->chunk = pbyObjectValue[i] + wOffset;
								binTablePtr->size = wTblLength;
								binDevDir->bin_hooked |= ulTableMaskBit;
							}					
						}
					}
					break;

					case DICT_REF_TBL_ID:	/* Dictionary Reference Table */
					{
						ulTableMaskBit = DICT_REF_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->DictReferenceTable);
						binTablePtr = &(binDevDir->dict_ref_tbl);
						ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
						if (!(binTablePtr->chunk)) 
						{
							bRet = read_word(&wOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);
							if(bRet == false)
							{
								delete pDevDirExt;
								return false;
							}

							bRet = read_word(&wTblLength,(void*)&dirExtnOffset->wSize,FORMAT_BIG_ENDIAN);
							if(bRet == false)
							{
								delete pDevDirExt;
								return false;
							}
							/*
							 * Attach the table if non-zero length, else go
							 * to the next table
							 */
							if (wTblLength) 
							{
								binTablePtr->chunk = pbyObjectValue[i] + wOffset;
								binTablePtr->size = wTblLength;
								binDevDir->bin_hooked |= ulTableMaskBit;
							}
						}
					}
					break;

					case LOCAL_VAR_TBL_ID:	/* Dictionary Reference Table */
				/*		ulTableMaskBit = LOCAL_VAR_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->LocalVariableTable);
						binTablePtr = &(binDevDir->local_var_tbl); */
						break;

					case CMD_NUM_ID_TBL_ID:	/* Command Number to Item ID Table */
					{
ulTableMaskBit = CMD_NUM_ID_TBL_MASK;
dirExtnOffset = &(pDevDirExt->CommandTable);
binTablePtr = &(binDevDir->cmd_num_id_tbl); 

ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
if (!(binTablePtr->chunk)) 
{
	bRet = read_word(&wOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);
	if(bRet == false)
	{
		delete pDevDirExt;
		return false;
	}

	bRet = read_word(&wTblLength,(void*)&dirExtnOffset->wSize,FORMAT_BIG_ENDIAN);
	if(bRet == false)
	{
		delete pDevDirExt;
		return false;
	}
	/*
	 * Attach the table if non-zero length, else go
	 * to the next table
	 */
	if (wTblLength) 
	{
		binTablePtr->chunk = pbyObjectValue[i] + wOffset;
		binTablePtr->size = wTblLength;
		binDevDir->bin_hooked |= ulTableMaskBit;
	}
}
					}
					break;

					default:	/* goes here for reserved or undefined table IDs */
						break;
				}

				/*
				 * Attach the binary for the table if it was requested and if
				 * it has not already been attached and if it not zero length.
				 */

//				ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
//				if (!(binTablePtr->chunk)) {

//				bRet = read_word(&wOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);

//				if(bRet == false)
//					return false;

//				bRet = read_word(&wTblLength,(void*)&dirExtnOffset->wSize,FORMAT_BIG_ENDIAN);
//				if(bRet == false)
//					return false;

				/*
				 * Attach the table if non-zero length, else go
				 * to the next table
				 */

//				if (wTblLength) {
//					binTablePtr->chunk = pbyObjectValue[i] + wOffset;
//					binTablePtr->size = wTblLength;
//					binDevDir->bin_hooked |= ulTableMaskBit;
//				}
			

//		}
	}			/* end while */

	/*We have Fetched the Device dir binary chunks, now Evaluate the device directories*/

				iRetVal = eval_dir_device_tables(flatDevDir, binDevDir,STRING_TBL_MASK | DICT_REF_TBL_MASK );
					/*DEVICE_TBL_MASKS );*/
				if(iRetVal != SUCCESS)
				{
					delete pDevDirExt;
						return false;
				}

					bDevDirLoadedFlag = true; /*Device Directory Loaded Successfully*/
					delete pDevDirExt;
					break;
				}
			case BLOCK_DIR_TYPE:
				{
//					printf("%d Block Directory Object: \n", i+1);
#ifdef _PARSER_DEBUG
					cout<< i+1 <<" Block Directory Object: \n";
#endif
/*					BLOCK_DIR_EXT *pBlkDir;
					BYTE *pbyPointer = (BYTE *)pbyExtensions[i];
					pBlkDir = (BLOCK_DIR_EXT *)(pbyPointer);
*/

#ifdef _PARSER_DEBUG
					cout<< i+1 <<" Block Directory Object: \n";
#endif
					BLOCK_DIR_EXT *pBlkDirExt;
					BYTE *pbyPointer = (BYTE *)pbyExtensions[i];
					
					pBlkDirExt = new BLOCK_DIR_EXT;

					pBlkDirExt->byLength	= pbyPointer[BLK_DIR_LENGTH_OFFSET];
					pBlkDirExt->byBlockDirObjectCode = pbyPointer[BLK_DIR_OBJ_CODE_OFFSET];
					pBlkDirExt->byFormatCode	 = pbyPointer[BLK_DIR_FORMAT_CODE_OFFSET];

#define RD_DATAPARTSEG(d,s) \
		pBlkDirExt-> d .offset = *((short_offset *) &pbyPointer[ (s) ]);\
		pBlkDirExt-> d .wSize  = *((WORD *) &pbyPointer[ (s) + SEG_SIZE_OFFSET])

					RD_DATAPARTSEG(BlockItemTable,				BLK_ITEM_TBL_OFFSET);
					RD_DATAPARTSEG(BlockItemNameTable,			BLK_ITEMNAME_TBL_OFFSET);
					RD_DATAPARTSEG(ParameterTable,				BLK_PARAM_TBL_OFFSET);
					RD_DATAPARTSEG(ParameterMemberTable,		BLK_PARAMEMBER_TBL_OFFSET);
					RD_DATAPARTSEG(ParameterMemberNameTable,	BLK_PARAMEMBERNAME_TBL_OFFSET);
					RD_DATAPARTSEG(ParameterElementTable,		BLK_ELEMENT_TBL_OFFSET);
					RD_DATAPARTSEG(ParameterListTable,			BLK_PARAMLIST_TBL_OFFSET);
					RD_DATAPARTSEG(ParameterListMemberTable,	BLK_PARAMLISTMEMBER_TBL_OFFSET);
					RD_DATAPARTSEG(ParameterListMemberNameTable,BLK_PARAMLISTMEMBERNAME_TBL_OFFSET);
					RD_DATAPARTSEG(CharectersiticsMemberTable,	BLK_CHARMEMBER_TBL_OFFSET);
					RD_DATAPARTSEG(CharectersiticsMemberNameTable,BLK_CHARMEMBERNAME_TBL_OFFSET);
					RD_DATAPARTSEG(RelationTable,				BLK_RELATION_TBL_OFFSET);
					RD_DATAPARTSEG(UpdateTable,					BLK_UPDATE_TBL_OFFSET);
					RD_DATAPARTSEG(ParameterCommandTable,		BLK_PARAM2COMMAND_TBL_OFFSET);
					RD_DATAPARTSEG(CriticalParameterTable,		BLK_CRITICALPARAM_TBL_OFFSET);

					/*Do Some Validations*/
					if(pBlkDirExt->byLength < BLK_DIR_LEN_HCF)
					{
						delete pBlkDirExt;
						return false; /* INVALID_EXTN_LENGTH*/
					}

					if(pBlkDirExt->byBlockDirObjectCode != BLOCK_DIR_TYPE)
					{
						delete pBlkDirExt;
						return false; /* DIR_TYPE_MISMATCH*/
					}
					/* we are only interested in a couple of tables */
					WORD wOffset, wTblLength;
					DATAPART_SEGMENT *tblExtnOffset;

					unsigned long   size;
					unsigned char  *chunk;
					bool            bRet;
					DDL_UINT    	temp_int, numeric;

					/* critical parameter table */

					tblExtnOffset = &(pBlkDirExt->CriticalParameterTable);

					bRet = read_word(&wOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
					if(bRet == false)
					{
						delete pBlkDirExt;
						return false;
					}

					bRet = read_word(&wTblLength,(void*)&tblExtnOffset->wSize,FORMAT_BIG_ENDIAN);
					if(bRet == false)
					{
						delete pBlkDirExt;
						return false;
					}						

					if (wTblLength) 
					{
						int rc; // for parse integer func
						chunk = pbyObjectValue[i] + wOffset;
						size = wTblLength;
						/* tbl == (encoded_int) count: count instances of (encoded_int)itemID */
						CriticalParamList.clear();
						/* eval_cpt */
						
						DDL_PARSE_INTEGER(&chunk, &size, &temp_int);/* parse count */

						if ( temp_int != 0 )
						{
							int y;// PAW 03/03/09
							for ( /*int*/ y = 0; y < temp_int && size > 0; y++)
							{
								DDL_PARSE_INTEGER(&chunk, &size, &numeric);/* parse count */
								CriticalParamList.push_back((unsigned long)numeric);
							}
							if ( size != 0 || y < temp_int )
							{								
								LOGIT(CERR_LOG,L"\n eval_crit_table failed!!!! size=%d cnt=%d\n", size,y);
							}
						}
					}
					/* end critical parameter table */
					/* we'll do the command table some other time */
					delete pBlkDirExt;

					break;
				}
			default:
				/*These cases are handled in GetItems()*/
				break;

		}/*End switch*/

		/*Just quit this loop once we are done with the loading of Device Directory*/

//stevev 10/13/04		
//		if(bDevDirLoadedFlag == true)
//			break;

	}/*End for*/

	bDevDirAllocated = true;
	return true;
}/*End LoadDeviceDirectory*/



bool DDlDevDescription ::LoadBlockItem()
{
	int iRetVal;
	BYTE byItemType;
	BYTE byItemSubType;
	unsigned long ulItemID;
	unsigned long ulItemMask;

	OBJECT_INDEX objectIndex;

	FLAT_BLOCK			*fblock;

	fblock = &fBlock;

	memset((char *)fblock, 0, sizeof(FLAT_BLOCK));//ok

	/*Since the BLOCK ITEM is found at the rear end of the Object list,
	 we will scan the object list from the back to get to it faster,
	 This is */

		byItemType = 0;
		byItemSubType = 0;
		ulItemID = 0L;
		ulItemMask = 0L;
		iRetVal =0;


	for (int i = descriptor.sod_length - 1; i >= 0;i--)
	{
		objectIndex = ObjectFixed[i].index;

		if(pbyExtensions[i][1] == BLOCK_ITYPE)
		{
//#ifdef _PARSER_DEBUG
//			myprintf(fout,"\n*******************************Block Item*******************************\n");
//			myprintf(fout,"%d Block Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif /*_PARSER_DEBUG*/
			iRetVal = fetch_item(pbyExtensions[i],objectIndex,BLOCK_ITYPE,
														&ulItemMask,(void*)fblock);


			pBlock = new DDlBlock();

			pBlock->id = fblock->id;
			pBlock->byItemType = BLOCK_ITYPE;

			pBlock->strItemName = "Block";

			/* We will set the masks after evaluating the "type" attribute */

			iRetVal = eval_block(fblock,ulItemMask,pBlock);

			if(iRetVal != SUCCESS)
			{
				if(iRetVal == FETCH_EXTERNAL_OBJECT)
					break;
				LOGIT(CERR_LOG,L"\n eval_block failed!!!!\n");
				return false;
			}

			ItemsList.push_back(pBlock);

			if(fblock->depbin)
			{
				if(fblock->depbin->db_characteristic)
					delete(fblock->depbin->db_characteristic);
				if(fblock->depbin->db_collect)
					delete(fblock->depbin->db_collect);
				if(fblock->depbin->db_edit_disp)
					delete(fblock->depbin->db_edit_disp);
				if(fblock->depbin->db_help)
					delete(fblock->depbin->db_help);
				if(fblock->depbin->db_item_array)
					delete(fblock->depbin->db_item_array);
				if(fblock->depbin->db_label)
					delete(fblock->depbin->db_label);
				if(fblock->depbin->db_menu)
					delete(fblock->depbin->db_menu);
				if(fblock->depbin->db_method)
					delete(fblock->depbin->db_method);
				if(fblock->depbin->db_param)
					delete(fblock->depbin->db_param);
				if(fblock->depbin->db_param_list)
					delete(fblock->depbin->db_param_list);
				if(fblock->depbin->db_refresh)
					delete(fblock->depbin->db_refresh);
				if(fblock->depbin->db_unit)
					delete(fblock->depbin->db_unit);
				if(fblock->depbin->db_wao)
					delete(fblock->depbin->db_wao);

				delete (fblock->depbin);
			}
			memset((char *)fblock, 0, sizeof(FLAT_BLOCK));//ok

			/*We are done Just break out of the loop*/
			break;

		}/*Endif*/


	}/*End for*/

	return true;

}/**/



bool DDlDevDescription ::GetItems()
{
	/*Here we will loop through the SOD Object Extensions*/

	BYTE byItemType;
	BYTE byItemSubType;
	unsigned long ulItemID;
	unsigned long ulItemMask;
	int iRetVal;
	FLAT_VAR			*fv;
	FLAT_COMMAND		*fcmd;
	FLAT_MENU			*fmenu;
	FLAT_EDIT_DISPLAY	*fedisp;
	FLAT_METHOD			*fmeth;
	FLAT_ITEM_ARRAY		*fiarr;
	FLAT_COLLECTION		*fcoll;
	FLAT_REFRESH		*frfrsh;
	FLAT_UNIT			*funit;
	FLAT_WAO			*fwao;
	FLAT_RECORD			*frec;
//	FLAT_BLOCK			*fblock;
	//RETURN_LIST errors;
	OBJECT_INDEX objectIndex;
	//(void)memset((char*)&errors,0,sizeof(RETURN_LIST));

	fv = &fVar;
	fcmd = &fCmd;
	fmenu = &fMenu;
	fedisp = &fEditDisp;
	fmeth = &fMethod;
	fiarr = &fIArr;
	fcoll = &fColl;
	frfrsh = &fRefresh;
	funit = &fUnit;
	fwao = &fWao;
	frec = &fRec;
//	fblock = &fBlock;


	memset((char *)fv, 0, sizeof(FLAT_VAR));
	memset((char *)fcmd, 0, sizeof(FLAT_COMMAND));
	memset((char *)fmenu, 0, sizeof(FLAT_MENU));
	memset((char *)fedisp, 0, sizeof(FLAT_EDIT_DISPLAY));
	memset((char *)fmeth, 0, sizeof(FLAT_METHOD));
	memset((char *)frfrsh, 0, sizeof(FLAT_REFRESH));
	memset((char *)funit, 0, sizeof(FLAT_UNIT));
	memset((char *)fwao, 0, sizeof(FLAT_WAO));
	memset((char *)fiarr, 0, sizeof(FLAT_ITEM_ARRAY));
	memset((char *)fcoll, 0, sizeof(FLAT_COLLECTION));
//	memset((char *)fblock, 0, sizeof(FLAT_BLOCK));
	memset((char *)frec, 0, sizeof(FLAT_RECORD));

// stevev moved to a higher level
//	/*Load the device directory*/
//
//	iRetVal = LoadDeviceDirectory();
//	if(iRetVal != true) /*Quit if it fails!!*/
//		return false;

	/*Since we need the HART Block for some ref resolutions we'll load it
	 first, out of turn!!!!*/

	//iRetVal = LoadBlockItem();
	bool bR = LoadBlockItem();
	if(bR != true) /*Quit if it fails!!*/
		return false;



	for (int i = 0; i < descriptor.sod_length;i++)
	{
		byItemType = 0;
		byItemSubType = 0;
		ulItemID = 0L;
		ulItemMask = 0L;
		iRetVal =0;
		objectIndex = ObjectFixed[i].index;



		if (byExtLengths[i] == 0)
		{
//			printf("%d has no data \n", i);
//#ifdef _PARSER_DEBUG
			cout<< i <<" has no data \n";
//#endif
			continue;
		}

		if (pbyExtensions[i] == NULL)
		{
//			printf("%d has no data \n", i);
//#ifdef _PARSER_DEBUG
			
			cout<< i <<" has no data \n";
//#endif
			continue;
		}


		switch (pbyExtensions[i][1])
		{
			case VARIABLE_ITYPE:
				{
//					memset((char *)fv, 0, sizeof(FLAT_VAR));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Variable Item*******************************\n");

//					printf("%d Variable Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Variable Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
				    iRetVal = fetch_item(pbyExtensions[i],objectIndex,VARIABLE_ITYPE,
																	&ulItemMask,(void*)fv);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}

					DDlVariable *pVar = new DDlVariable();

					pVar->id = fv->id;
					pVar->byItemType = VARIABLE_ITYPE;

					pVar->strItemName = "Variable";

					/* We will set the masks after evaluating the "type" attribute */

					iRetVal = eval_variable(fv,ulItemMask,pVar);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
	//					EVAL_FAILED( variable , pVar );
LOGIT(1, L"\n eval_" L"variable" L"  failed for : %d\t i = %d\t RetCode = %d\n", pVar ->id, i, iRetVal);
						//getchar();
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					/*Push the parsed variable on the ItemList*/

					ItemsList.push_back(pVar);

					/*		dump_var(fout,fv);

					if(!(fv->class_attr))
					{
						iNumberOfNoClassVars++;
						myprintf(ferr,"%ul\n",fv->id);
					} */

					if(fv->depbin)
					{
						if(fv->depbin->db_class)
							delete (fv->depbin->db_class);
						if(fv->depbin->db_display)
							delete (fv->depbin->db_display);
						if(fv->depbin->db_edit)
							delete (fv->depbin->db_edit);
						if(fv->depbin->db_enums)
							delete (fv->depbin->db_enums);
						if(fv->depbin->db_handling)
							delete (fv->depbin->db_handling);
						if(fv->depbin->db_help)
							delete (fv->depbin->db_help);
						if(fv->depbin->db_index_item_array)
							delete (fv->depbin->db_index_item_array);
						if(fv->depbin->db_label)
							delete (fv->depbin->db_label);
						if(fv->depbin->db_resp_codes)
							delete (fv->depbin->db_resp_codes);
						if(fv->depbin->db_type_size)
							delete (fv->depbin->db_type_size);
						delete(fv->depbin);

					}/*Endif fv->depbin*/

					if(fv->misc)
					{
						if(fv->misc->depbin)
						{
							if(fv->misc->depbin->db_max_val)
								delete (fv->misc->depbin->db_max_val);
							if(fv->misc->depbin->db_min_val)
								delete (fv->misc->depbin->db_min_val);
							//if(fv->misc->depbin->db_read_time_out)
							//	delete (fv->misc->depbin->db_read_time_out);
							//if(fv->misc->depbin->db_write_time_out)
							//	delete (fv->misc->depbin->db_write_time_out);
							if(fv->misc->depbin->db_height)
								delete (fv->misc->depbin->db_height);
							if(fv->misc->depbin->db_width)
								delete (fv->misc->depbin->db_width);
							if(fv->misc->depbin->db_scale)
								delete (fv->misc->depbin->db_scale);
							if(fv->misc->depbin->db_unit)
								delete (fv->misc->depbin->db_unit);
							if(fv->misc->depbin->db_valid)
								delete (fv->misc->depbin->db_valid);
							delete (fv->misc->depbin);
						}
						delete (fv->misc);
					}
					if(fv->actions)
					{
						if(fv->actions->depbin)
						{
							if(fv->actions->depbin->db_post_edit_act)
								delete (fv->actions->depbin->db_post_edit_act);
							if(fv->actions->depbin->db_post_read_act)
								delete (fv->actions->depbin->db_post_read_act);
							if(fv->actions->depbin->db_post_write_act)
								delete (fv->actions->depbin->db_post_write_act);
							if(fv->actions->depbin->db_pre_edit_act)
								delete (fv->actions->depbin->db_pre_edit_act);
							if(fv->actions->depbin->db_pre_read_act)
								delete (fv->actions->depbin->db_pre_read_act);
							if(fv->actions->depbin->db_pre_write_act)
								delete (fv->actions->depbin->db_pre_write_act);
							if(fv->actions->depbin->db_refresh_act)
								delete (fv->actions->depbin->db_refresh_act);
#ifdef XMTR
							if(fv->actions->depbin->db_post_rqst_act)
								delete (fv->actions->depbin->db_post_rqst_act);
							if(fv->actions->depbin->db_post_user_act)
								delete (fv->actions->depbin->db_post_user_act);
#endif
							delete (fv->actions->depbin);
						}
						delete (fv->actions);
					}

					memset((char *)fv, 0, sizeof(FLAT_VAR));

					break;
				}


			case COMMAND_ITYPE://Command
				{
//					memset((char *)fcmd, 0, sizeof(FLAT_COMMAND));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Command Item*******************************\n");
//					printf("%d Command Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Command Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
				    iRetVal = fetch_item(pbyExtensions[i],objectIndex,COMMAND_ITYPE,
																&ulItemMask,(void*)fcmd);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}
//					iRetVal = eval_item(fcmd, ulItemMask, &errors,COMMAND_ITYPE);

					DDlCommand *pCmd = new DDlCommand();

					pCmd->id = fcmd->id;

					pCmd->byItemType = COMMAND_ITYPE;

					pCmd->strItemName = "Command";

					iRetVal = eval_command(fcmd,ulItemMask,pCmd);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( command ,pCmd);
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					/*Push the parsed Command on the ItemList*/

					ItemsList.push_back(pCmd);

					if(fcmd->depbin)
					{
						if(fcmd->depbin->db_number)
							delete (fcmd->depbin->db_number);
						if(fcmd->depbin->db_oper)
							delete (fcmd->depbin->db_oper);
						if(fcmd->depbin->db_resp_codes)
							delete (fcmd->depbin->db_resp_codes);
						if(fcmd->depbin->db_trans)
							delete (fcmd->depbin->db_trans);

						delete (fcmd->depbin);
					}

					memset((char *)fcmd, 0, sizeof(FLAT_COMMAND));

					break;
				}
			case MENU_ITYPE:// Menu
				{
//					memset((char *)fmenu, 0, sizeof(FLAT_MENU));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Menu Item*******************************\n");
//					printf("%d Menu Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Menu Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
					iRetVal = fetch_item(pbyExtensions[i],objectIndex,MENU_ITYPE,
																&ulItemMask,(void*)fmenu);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}
//					iRetVal = eval_item(fmenu, ulItemMask, &errors,MENU_ITYPE);

					DDlMenu *pMenu = new DDlMenu();

					pMenu->id = fmenu->id;

					pMenu->byItemType = MENU_ITYPE;

					pMenu->strItemName = "Menu";

					iRetVal = eval_menu(fmenu,ulItemMask,pMenu);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( menu , pMenu );
//#endif /*_PARSER_DEBUG*/

						return false;
					}


					/*Push the parsed Menu on the ItemList*/

					ItemsList.push_back(pMenu);


					if(fmenu->depbin)
					{
						if(fmenu->depbin->db_items)
							delete (fmenu->depbin->db_items);
						if(fmenu->depbin->db_label)
							delete (fmenu->depbin->db_label);

						delete (fmenu->depbin);
					}

					memset((char *)fmenu, 0, sizeof(FLAT_MENU));

//					dump_menu(fout,fmenu);
					break;
				}
			case EDIT_DISP_ITYPE:// Edit Display
				{
//					memset((char *)fedisp, 0, sizeof(FLAT_EDIT_DISPLAY));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Edit Display Item*******************************\n");
//					printf("%d Edit Display Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Edit Display Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
					iRetVal = fetch_item(pbyExtensions[i],objectIndex,EDIT_DISP_ITYPE,
																	&ulItemMask,(void*)fedisp);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}

					DDlEditDisplay* pEditDisp = new DDlEditDisplay();

					pEditDisp->id = fedisp->id;

					pEditDisp->byItemType = EDIT_DISP_ITYPE;

					pEditDisp->strItemName = "Edit Display";

					iRetVal = eval_edit_display(fedisp,ulItemMask,pEditDisp);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( edit_display , pEditDisp );
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					/*Push the parsed Edit Display on the list*/

					ItemsList.push_back(pEditDisp);

//					dump_edit_display(fout,fedisp);


					if(fedisp->depbin)
					{
						if(fedisp->depbin->db_disp_items)
							delete (fedisp->depbin->db_disp_items);
						if(fedisp->depbin->db_edit_items)
							delete (fedisp->depbin->db_edit_items);
						if(fedisp->depbin->db_label)
							delete (fedisp->depbin->db_label);
						if(fedisp->depbin->db_post_edit_act)
							delete (fedisp->depbin->db_post_edit_act);
						if(fedisp->depbin->db_pre_edit_act)
							delete (fedisp->depbin->db_pre_edit_act);
						delete (fedisp->depbin);
					}

					memset((char *)fedisp, 0, sizeof(FLAT_EDIT_DISPLAY));

					break;
				}
			case METHOD_ITYPE://Method
				{
//					memset((char *)fmeth, 0, sizeof(FLAT_METHOD));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Method Item*******************************\n");
//					printf("%d Method Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Method Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
					iRetVal = fetch_item(pbyExtensions[i],objectIndex,METHOD_ITYPE,
																	&ulItemMask,(void*)fmeth);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}
					/*if(i == 141)
						break;*/

					DDlMethod * pMeth = new DDlMethod();

					pMeth->id = fmeth->id;

					pMeth->byItemType =	METHOD_ITYPE;

					pMeth->strItemName = "Method";

/*					if (i == 142)
					{
						DDlAttribute **p;
						p = (DDlAttribute **)new (DDlAttribute *);

					}*/

					iRetVal = eval_method(fmeth,ulItemMask,pMeth);



					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( method , pMeth );
//#endif /*_PARSER_DEBUG*/
						return false;
					}
					ItemsList.push_back(pMeth);


					if(fmeth->depbin)
					{
						if(fmeth->depbin->db_class)
							delete (fmeth->depbin->db_class);
						if(fmeth->depbin->db_def)
							delete (fmeth->depbin->db_def);
						if(fmeth->depbin->db_help)
							delete (fmeth->depbin->db_help);
						if(fmeth->depbin->db_label)
							delete (fmeth->depbin->db_label);
						if(fmeth->depbin->db_scope)
							delete (fmeth->depbin->db_scope);
						if(fmeth->depbin->db_valid)
							delete (fmeth->depbin->db_valid);
						delete (fmeth->depbin);
					}

					memset((char *)fmeth, 0, sizeof(FLAT_METHOD));

//					dump_method(fout,fmeth);
					break;
				}
			case REFRESH_ITYPE:// Refresh Relation
				{
//					memset((char *)frfrsh, 0, sizeof(FLAT_REFRESH));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Refresh Item*******************************\n");
//					printf("%d Refresh Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Refresh Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
					iRetVal = fetch_item(pbyExtensions[i],objectIndex,REFRESH_ITYPE,
																	&ulItemMask,(void*)frfrsh);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}


					DDlRefresh* pRefresh = new DDlRefresh();

					pRefresh->id = frfrsh->id;

					pRefresh->byItemType = REFRESH_ITYPE;

					pRefresh->strItemName = "Refresh Relation";

					iRetVal = eval_refresh(frfrsh,ulItemMask,pRefresh);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( refresh , pRefresh );
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					ItemsList.push_back(pRefresh);


					if(frfrsh->depbin)
					{
						if(frfrsh->depbin->db_items)
							delete (frfrsh->depbin->db_items);
						delete (frfrsh->depbin);
					}

					memset((char *)frfrsh, 0, sizeof(FLAT_REFRESH));

//					dump_refresh(fout,frfrsh);
					break;
				}
			case UNIT_ITYPE:// Unit Relation
				{
//					memset((char *)funit, 0, sizeof(FLAT_UNIT));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Unit Item*******************************\n");
//					printf("%d Unit Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Unit Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
				    iRetVal = fetch_item(pbyExtensions[i],objectIndex,UNIT_ITYPE,
																&ulItemMask,(void*)funit);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}

					DDlUnit* pUnit = new DDlUnit();

					pUnit->id = funit->id;

					pUnit->byItemType = UNIT_ITYPE;

					pUnit->strItemName = "Unit Relation";

					iRetVal = eval_unit(funit,ulItemMask,pUnit);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( unit , pUnit );
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					ItemsList.push_back(pUnit);


					if(funit->depbin)
					{
						if(funit->depbin->db_items)
							delete (funit->depbin->db_items);
						delete (funit->depbin);
					}

					memset((char *)funit, 0, sizeof(FLAT_UNIT));

//					dump_unit(fout,funit);
					break;
				}
			case WAO_ITYPE:// WAO Relation
				{
//					memset((char *)fwao, 0, sizeof(FLAT_WAO));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Wao Item*******************************\n");
//					printf("%d Wao Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Wao Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
				    iRetVal = fetch_item(pbyExtensions[i],objectIndex,WAO_ITYPE,
																&ulItemMask,(void*)fwao);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}

					DDlWao* pWao = new DDlWao();

					pWao->id = fwao->id;

					pWao->byItemType = WAO_ITYPE;

					pWao->strItemName = "WAO Relation";

					iRetVal = eval_wao(fwao,ulItemMask,pWao);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( wao , pWao );
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					ItemsList.push_back(pWao);


					if(fwao->depbin)
					{
						if(fwao->depbin->db_items)
							delete (fwao->depbin->db_items);
						delete (fwao->depbin);
					}

					memset((char *)fwao, 0, sizeof(FLAT_WAO));

//					dump_wao(fout,fwao);
					break;
				}
			case ITEM_ARRAY_ITYPE:// Item Array
				{
//					memset((char *)fiarr, 0, sizeof(FLAT_ITEM_ARRAY));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Item Array Item*******************************\n");
//					printf("%d Item Array Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Item Array Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
				    iRetVal = fetch_item(pbyExtensions[i],objectIndex,ITEM_ARRAY_ITYPE,
																	&ulItemMask,(void*)fiarr);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}

					DDlItemArray* pItemArray = new DDlItemArray();

					pItemArray->id = fiarr->id;

					pItemArray->byItemType = ITEM_ARRAY_ITYPE;

					pItemArray->byItemSubType = ((ITEM_EXTN*)pbyExtensions[i])->bySubType;

					pItemArray->strItemName = "Item Array";


					iRetVal = eval_item_array(fiarr,ulItemMask,pItemArray);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( item_array , pItemArray );
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					ItemsList.push_back(pItemArray);


					if(fiarr->depbin)
					{
						if(fiarr->depbin->db_elements)
							delete (fiarr->depbin->db_elements);
						if(fiarr->depbin->db_help)
							delete (fiarr->depbin->db_help);
						if(fiarr->depbin->db_label)
							delete (fiarr->depbin->db_label);
						delete (fiarr->depbin);
					}

					memset((char *)fiarr, 0, sizeof(FLAT_ITEM_ARRAY));

//					dump_item_array(fout,fiarr);
					break;
				}
			case COLLECTION_ITYPE:// Collection
				{
//					memset((char *)fcoll, 0, sizeof(FLAT_COLLECTION));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Collection Item*******************************\n");
//					printf("%d Collection Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Collection Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
				    iRetVal = fetch_item(pbyExtensions[i],objectIndex,COLLECTION_ITYPE,
																	&ulItemMask,(void*)fcoll);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}

					DDlCollection* pCollection = new DDlCollection();

					pCollection->id = fcoll->id;

					pCollection->byItemType = COLLECTION_ITYPE;

					pCollection->byItemSubType = ((ITEM_EXTN*)pbyExtensions[i])->bySubType;

					pCollection->strItemName = "Collection";


					iRetVal = eval_collection(fcoll,ulItemMask,pCollection);


					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( collection , pCollection );
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					ItemsList.push_back(pCollection);

					if(fcoll->depbin)
					{
						if(fcoll->depbin->db_help)
							delete (fcoll->depbin->db_help);
						if(fcoll->depbin->db_label)
							delete (fcoll->depbin->db_label);
						if(fcoll->depbin->db_valid)
							delete (fcoll->depbin->db_valid);// added 22jan07
						if(fcoll->depbin->db_members)
							delete (fcoll->depbin->db_members);// saw was missing 22jan07
						if(fcoll->depbin->db_debug_info)
							delete (fcoll->depbin->db_debug_info);
						delete (fcoll->depbin);
					}

					memset((char *)fcoll, 0, sizeof(FLAT_COLLECTION));

//					dump_collection(fout,fcoll);
					break;
				}
			case RECORD_ITYPE://Record
				{
//					memset((char *)frec, 0, sizeof(FLAT_RECORD));
//#ifdef _PARSER_DEBUG
//					myprintf(fout,"\n*******************************Record Item*******************************\n");
//					printf("%d Record Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//					myprintf(fout,"%d Record Object: \t Index : %u \n", i+1,ObjectFixed[i].index);
//#endif
				    iRetVal = fetch_item(pbyExtensions[i],objectIndex,RECORD_ITYPE,
																&ulItemMask,(void*)frec);
					if(iRetVal != SUCCESS)
					{
						if(iRetVal == FETCH_EXTERNAL_OBJECT)
							break; /*Don't quit if an external object was tried as a base object*/
						return false;
					}

					DDlRecord* pRecord = new DDlRecord();

					pRecord->id = frec->id;

					pRecord->byItemType = RECORD_ITYPE;

					pRecord->byItemSubType = ((ITEM_EXTN*)pbyExtensions[i])->bySubType;

					pRecord->strItemName = "Record";

					iRetVal = eval_record(frec,ulItemMask,pRecord);

					if(iRetVal != SUCCESS)
					{
//#ifdef _PARSER_DEBUG
						EVAL_FAILED( record , pRecord );
//#endif /*_PARSER_DEBUG*/
						return false;
					}

					/*Vibhor 311003: For implementing the Demunging Solution We will store this
					 as a Collection item*/

					pRecord->byItemType = COLLECTION_ITYPE;

					pRecord->strItemName = "Collection";

					ItemsList.push_back(pRecord);


					if(frec->depbin)
					{
						if(frec->depbin->db_help)
							delete (frec->depbin->db_help);
						if(frec->depbin->db_label)
							delete (frec->depbin->db_label);
						if(frec->depbin->db_members)
							delete (frec->depbin->db_members);
						if(frec->depbin->db_resp_codes)
							delete (frec->depbin->db_resp_codes);
						delete (frec->depbin);
					}

					memset((char *)frec, 0, sizeof(FLAT_RECORD));


//					dump_record(fout,frec);
					break;
				}
			case ARRAY_ITYPE://Array
//#ifdef _PARSER_DEBUG
					LOGIT(CERR_LOG,L"\n#################################ARRAY ITEM!!!#################################\n");			
//#endif
					break;
			case VAR_LIST_ITYPE:// Variable List
//#ifdef _PARSER_DEBUG
					LOGIT(CERR_LOG,L"\n#################################VARRIABLE LIST ITEM!!!#################################\n");			
//#endif
					break;

			case RESP_CODES_ITYPE:// Response Code
//#ifdef _PARSER_DEBUG
					LOGIT(CERR_LOG,L"\n#################################RESP CODE ITEM!!!#################################\n");			
//#endif
					break;

			default:

//					printf("Error : Invalid Object Type  : Type Code = % d\n",pbyExtensions[i][1]);
//#ifdef _PARSER_DEBUG
				if(	(   pbyExtensions[i][1] == ARRAY_ITYPE )
					|| (pbyExtensions[i][1] == VAR_LIST_ITYPE )
					|| (pbyExtensions[i][1] == RESP_CODES_ITYPE )
				  )
				{
					cout<<"Error : Invalid Object Type  : Type Code = "<<pbyExtensions[i][1]<<endl;
				}
//#endif
					break;

		}/*End switch (pbyExtensions[i][1])*/

	}/*End for*/


	
return true;
}/*End GetItems()*/




bool DDlDevDescription::ResolveItemName()
{
	vector <DDlBaseItem *>:: iterator p;
	DDlBaseItem *pBaseItem;
	char itemName[80];

	//Added by Deepak to prevent read overrun
	memset(itemName,0,80);

	for(p = ItemsList.begin(); p != ItemsList.end();p++)
	{
		pBaseItem = *p;
// was
//		get_item_name(pBaseItem->id,itemName);
//		pBaseItem->strItemName = itemName;
		if ( pBaseItem->strItemName.empty() )
		{
			get_item_name(pBaseItem->id,itemName);
			pBaseItem->strItemName = itemName;
		}
	}

return true;

}



bool DDlDevDescription ::LoadDeviceDescription(bool isInTok)
{
	bool bRet;

	isInTokizer = isInTok;

	LOGIT(CLOG_LOG, "before ReadHeader");

	bRet = ReadHeader(header,fp);
	if(!bRet)
		return false;
#ifdef LOG_READ
	LogHeader();
#endif

	//jad debug
	LOGIT(CLOG_LOG, "header.magic_number = %d", header.magic_number);
	LOGIT(CLOG_LOG, "header.header_size = %d", header.header_size);
	LOGIT(CLOG_LOG, "header.objects_size = %d", header.objects_size);
	LOGIT(CLOG_LOG, "header.data_size = %d", header.data_size);
	LOGIT(CLOG_LOG, "header.byManufacturer = %d, %d, %d", header.byManufacturer[0], header.byManufacturer[1], header.byManufacturer[2]);
	LOGIT(CLOG_LOG, "header.device_type = %d", header.device_type);
	LOGIT(CLOG_LOG, "header.device_revision = %d", header.device_revision);
	LOGIT(CLOG_LOG, "header.dd_revision = %d", header.dd_revision);
	LOGIT(CLOG_LOG, "header.tok_rev_major = %d", header.tok_rev_major);
	LOGIT(CLOG_LOG, "header.tok_rev_minor = %d", header.tok_rev_minor);

	LOGIT(CLOG_LOG, "before ValidateHeader");
	bRet = ValidateHeader(&header);
	if(!bRet)
		return false;
	LOGIT(CLOG_LOG, "before ReadObjectDescription");
	bRet = ReadObjectDescription();
	if(!bRet)
		return false;
#ifdef LOG_READ
	LogOD();
#endif
	LOGIT(CLOG_LOG, "before ValidateObjectDescription");
	bRet = ValidateObjectDescription(&descriptor);
	if(!bRet)
		return false;
	uSODLength = descriptor.sod_length;

	uSodLen = uSODLength;
	LOGIT(CLOG_LOG, "uSodLen = %d", uSodLen);
	bRet= ReadSOD();
	if(!bRet)
		return false;
#ifdef x_DEBUG // LOG_READ
	LogSOD();
#endif

	LOGIT(CLOG_LOG, "before ReadObjectValues");
	bRet= ReadObjectValues();
	if(!bRet)
		return false;
#ifdef LOG_READ
	//LogOV();
#endif
	
	/*Vibhor 010904: Start of Multiple Code Changes*/
	
	/*Read the Format Object and determine the Tokenizer Revision numbers*/
	LOGIT(CLOG_LOG, "before ReadFormatObject");

	bRet = ReadFormatObject();
	
	if(!bRet)
		return false;
	/*NOTE: We may need to read the binary file in LoadDeviceDirectory_6(), 
			so holding it till done, whereas in HART 5 LoadDeviceDirectory
			we don't need it, so we close the before loading the directory*/
	
	LOGIT(CLOG_LOG, "CodingMajor = %d", CodingMajor);


	if( 
#ifndef _DEBUG
		 CodingMajor == DDOD_REV_SUPPORTED_EIGHT
#else
		(CodingMajor == DDOD_REV_SUPPORTED_EIGHT && CodingMinor <= DDOD_REV_MINOR_HCF )
#endif
		 || CodingMajor == DDOD_REV_SUPPORTED_SIX
	  )
	{
		bRet = LoadDeviceDirectory_6();
		if(!bRet)
			return false; /*Quit if it fails*/
		/*Close the fms file as its no more needed*/
		if(fp)
		{
			fclose(fp);
			fp = NULL;
		}

		bTokRev6Flag = true;
		LOGIT(CLOG_LOG, "before GetItems6");
		bRet = GetItems6();
		if(!bRet)
			return false;
		LOGIT(CLOG_LOG, "before GetImages6");
		bRet = GetImages6();
		if(!bRet)
			return false;
		
	}
	else if( CodingMajor == DDOD_REV_SUPPORTED_FIVE )
	{
		/*Close the fms file as its no more needed*/
	
		if(fp)
		{
			fclose(fp);
			fp = NULL;
		}

		bRet = LoadDeviceDirectory();
		if(bRet != true) /*Quit if it fails!!*/
			return false;

		bTokRev6Flag = false;// Just be safe.

		bRet = GetItems();
		if(!bRet)
			return false;
	}
	else 
	{
#ifdef _DEBUG
		LOGIT(CERR_LOG|UI_LOG,"ERROR: Tokenizer revision %d.%d is NOT supported. (only %d, %d and %d.%d)\n",
				(int)CodingMajor,(int)CodingMinor,DDOD_REV_SUPPORTED_FIVE,DDOD_REV_SUPPORTED_SIX,
													DDOD_REV_SUPPORTED_EIGHT,DDOD_REV_MINOR_HCF);
#else
		LOGIT(CERR_LOG|UI_LOG,"ERROR: Tokenizer revision %d is NOT supported. (only %d, %d and %d)\n",
					(int)CodingMajor,DDOD_REV_SUPPORTED_EIGHT,DDOD_REV_SUPPORTED_SIX,DDOD_REV_SUPPORTED_FIVE);
#endif
		return false;
	}

/*Vibhor 010904: End of Multiple Code Changes*/

	ResolveItemName();
	
	CleanGlobals();
	return true;

}


int get_item_name(unsigned long item_id, char* item_name)
{
	//FILE *fsym;
	char buffer[200];
	char *iname;
	char *token,*last;
	unsigned long id;
#ifdef MODIFIED_BY_SS
	//size_t arraySize = sizeof(chunk) / sizeof(chunk[0]); // Calculate the size of the array
	std::string str("no_symbol_name");
	std::copy(str.begin(), str.end(), static_cast<char*>(item_name));

	std::ifstream fsym((const char *)symFilePath);
    if (!fsym.is_open())
        return -1; // file open error

    std::string line;
    while (std::getline(fsym, line))
    {
        std::string iname;
        std::string token;
        std::size_t pos = 0;
        while ((pos = line.find_first_of(" \t", pos)) != std::string::npos)
        {
            std::size_t nextPos = line.find_first_not_of(" \t", pos);
            token = line.substr(pos, nextPos - pos);
            pos = nextPos;

            if (!iname.empty())
                iname += ' '; // add space separator

            if (std::isdigit(token[0]))
            {
                unsigned long id = std::stoul(token);
                if (id == item_id)
                {
                    std::copy(iname.begin(), iname.end(), item_name);
                    item_name[iname.size()] = '\0'; // add null terminator
                    fsym.close();
                    return 0; // item found
                }
            }
            else
            {
                iname += token;
            }
        }
    }

    fsym.close();
    return 1; // item not found

#endif	


	//fsym =fopen(symFilePath,"r");
	//if(fsym)
	//{
	//	while(fgets(buffer,200,fsym))
	//	{
	//		token= strtok(buffer," \t");
	//		do
	//		{
	//			token = strtok((char*)NULL, " \t");//extract the item name
	//			iname = token;
	//			strcat(iname,"\0");
	//			token = strtok((char*)NULL, " \t");//item_type... not interested
	//			/* This redundant code is for the case when the "type" field in .sym file is blank */
	//			id = strtoul(token,&last,10);
	//			if(id == item_id)
	//			{
	//				//item_name = iname;
	//				strcpy(item_name,iname);
	//				last = strtok((char*)NULL," ");
	//				//last = strtok((char*)NULL," ");
	//				strcat(last,"\0");
	//				fclose(fsym);

	//				return 0;
	//			}
	//			token = strtok((char*)NULL, "\t");
	//			id = strtoul(token,&last,10);
	//			if(id == item_id)
	//			{
	//				//item_name = iname;
	//				strcpy(item_name,iname);
	//				last = strtok(token," ");
	//				last = strtok((char*)NULL," ");
	//				strcat(last,"\0");
	//				fclose(fsym);
	//				return 0;
	//			}
	//		}while(strtok((char*)NULL, "\t"));
	//	}
	//	fclose(fsym);
	//	return 1;//item not found

	//}
	//else
	//	return -1;//file open error

}

/*Vibhor 310804: Start of Code*/

int DDlDevDescription ::ReadIntegerValueFromFile(FILE *fileP, UINT64 &uValue,BYTE &byLength)
{
	BYTE *byTemp = NULL;
	
	int iRetVal = 0;
	BYTE byLen = 1; /*Minimum default length*/
 	UINT64       uVal=0;	/* temp storage for parsed integer */ 
	int		    iMoreIndicator=0;	/* need to parse another byte */
	
	
	byTemp	= new BYTE[2];

	//Read the first byte
	iRetVal = fread(byTemp,1,sizeof(BYTE),fileP);
	
	uVal = *byTemp & 0x7f;
	if(*byTemp & 0x80)
	{
			/*
			* Read each byte, building the ulong until the high order bit is
			* not set 
			*/
		do{
			*byTemp = 0;
			//read the next byte
			iRetVal = fread(byTemp,1,sizeof(BYTE),fileP);
		 	if(sizeof(BYTE) != iRetVal)
			{
				return DDL_ENCODING_ERROR;
			}
			
			iMoreIndicator = *byTemp & 0x80; 

			*byTemp &= 0x7f;

			if (uVal > ( DDL_UINT64_MAX >> 7)) {
				return DDL_LARGE_VALUE;
			}
		
			uVal <<= 7;
			if (uVal >  DDL_UINT64_MAX - *byTemp) {
				return DDL_LARGE_VALUE;
			}
			
			uVal |= *byTemp;
			byLen++; //increment the length of the integer
		
		}while(iMoreIndicator);
		
		uValue = uVal;

	}
	else
	{
		uValue = *byTemp;
	}
	
	byLength = byLen;

	delete byTemp;
	byTemp = NULL;
	return DDL_SUCCESS;

}/*End of ReadIntegerFromFile()*/

/*Vibhor 310804: End of Code*/
