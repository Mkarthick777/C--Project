// DDlDevDesc6.cpp: HART6 implementation of the DDlDevDescription class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)
#include "pch.h"
//sjv06feb06 #include <windows.h>
#include <assert.h>

#include "ddbGeneral.h"

#include "DDlDevDescription.h"

#include <iostream>

#include <iomanip>

//#include<iostream.h>
#include "globaldefs.h"

#include "fetch_item.h"	//??
#include "evl_loc.h"	//??
//#include "Dict.h"
#include "DDlDevDescription.h"
//#include "PrintData.h"
#include "DDl6Items.h"
#include "Eval_Item.h"
#ifdef _DEBUG
#include "Dictionary.h"// for dictionary test
extern CDictionary *pGlobalDict; /*The Global Dictionary object*/
extern LitStringTable *pLitStringTable; /*The Global literal string table object*/

//#define READ_DEBUG

#endif
extern int	ddl_parse_integer_func(unsigned char **chunkp, unsigned long *size, 
								   UINT64 *value);
extern int ddl_parse_tag_func(unsigned char **chunkp, unsigned long *size, unsigned long *tagp,
							  unsigned long *lenp);
#if _MSC_VER < 1400	/* these are defined in VS 2005 */
extern std::ostream& operator<<(std::ostream& os, __int64 i );
extern std::ostream& operator<<(std::ostream& os, unsigned __int64 i );
#endif
#define TABLE_OFFSET_INVALID	-1

#ifndef linux
#define EVAL_FAILED( w, p ) LOGIT(CERR_LOG,L"\n eval_attrs failed for " L#w L": %d\t i = %d\t RetCode = %d\n", p ->id, i, iRetVal)
#else
#define EVAL_FAILED( w, p ) ::logout(CERR_LOG, L"\n eval_attrs failed for " L ## #w L": %d\t i = %d\t RetCode = %d\n" , p ->id, i, iRetVal)
#endif

//Vibhor 010904: Add HART 6 Definitions for Device Directory
FLAT_DEVICE_DIR_6 device_dir_6;
BIN_DEVICE_DIR_6 bin_dev_dir_6;


FLAT_BLOCK_DIR_6  block_dir_6;
BIN_BLOCK_DIR_6   bin_blk_dir_6;


bool DDlDevDescription	:: LoadDeviceDirectory_6()
{
	int iRetVal;
	OBJECT_INDEX objectIndex;
	bool bDevDirLoadedFlag = false;
	//LOGIT(CLOG_LOG, "in LoadDeviceDirectory_6");

	for (int i = 0; i < descriptor.sod_length;i++)
	{
		iRetVal =0;
		objectIndex = ObjectFixed[i].index;

		switch (pbyExtensions[i][1])
		{
 			case DEVICE_DIR_TYPE:
			{	// no classes in FLAT_DEVICE_DIR_6 nor BIN_DEVICE_DIR_6; memset ok
				(void)memset((char *) &device_dir_6, 0, sizeof(FLAT_DEVICE_DIR_6));
				(void)memset((char *) &bin_dev_dir_6, 0, sizeof(BIN_DEVICE_DIR_6));

				FLAT_DEVICE_DIR_6 *flatDevDir = &device_dir_6;
				BIN_DEVICE_DIR_6 *binDevDir   = &bin_dev_dir_6;
				BININFO *binTablePtr;
				DATAPART_SEGMENT_6 *dirExtnOffset;
				DWORD dwOffset;
				DWORD dwTblLength;
				bool bRet;

#ifdef _PARSER_DEBUG
				cout<< i+1 <<" Device Directory Object: \n";
#endif
				DEVICE_DIR_EXT_6 *pDevDirExt;
				BYTE *pbyPointer = (BYTE *)pbyExtensions[i];

				pDevDirExt = new DEVICE_DIR_EXT_6;
				memset(pDevDirExt,0,sizeof(DEVICE_DIR_EXT_6));// ok


 				pDevDirExt->byLength	          = pbyPointer[DEV_DIR_LENGTH_6_OFFSET]; // Not sure abt this guy
				pDevDirExt->byDeviceDirObjectCode = pbyPointer[DEV_DIR_OBJ_CODE_6_OFFSET];
				pDevDirExt->byFormatCode	      = pbyPointer[DEV_DIR_FORMAT_CODE_6_OFFSET];
#ifdef READ_DEBUG
				cout<<"---------- Device Directory -----------------"<<endl;
				cout<<"    Length:  0x"<<hex<<(int)pDevDirExt->byLength<<dec<<endl;
				cout<<"ObjectCode:  0x"<<hex<<(int)pDevDirExt->byDeviceDirObjectCode<<dec<<endl;
				cout<<"FormatCode:  0x"<<hex<<(int)pDevDirExt->byFormatCode<<dec<<endl;
#endif
if (CodingMajor > 7)
{
#ifndef _WIN32_WCE	// PAW aligment issues 29/05/09

	//LOGIT(CLOG_LOG, "CodingMajor > 7");



				pDevDirExt->ItemTable.offset = *((long_offset *) &pbyPointer[ITEM_TBL_8_OFFSET]);
				pDevDirExt->ItemTable.uSize = *((DWORD *) &pbyPointer[ITEM_TBL_8_OFFSET + SEG_SIZE_OFFSET_8 ]);
				pDevDirExt->StringTable.offset = *((long_offset *) &pbyPointer[STRNG_TBL_8_OFFSET]);
				pDevDirExt->StringTable.uSize = *((DWORD *) &pbyPointer[STRNG_TBL_8_OFFSET + SEG_SIZE_OFFSET_8]);
				pDevDirExt->DictReferenceTable.offset = *((long_offset *) &pbyPointer[DICT_REF_TBL_8_OFFSET]);
				pDevDirExt->DictReferenceTable.uSize = *((DWORD *) &pbyPointer[DICT_REF_TBL_8_OFFSET + SEG_SIZE_OFFSET_8]);
				pDevDirExt->CommandTable.offset = *((long_offset *) &pbyPointer[CMD_TBL_8_OFFSET]);
				pDevDirExt->CommandTable.uSize = *((DWORD *) &pbyPointer[CMD_TBL_8_OFFSET + SEG_SIZE_OFFSET_8]);
				pDevDirExt->ImageTable.offset = *((long_offset *) &pbyPointer[IMG_TBL_8_OFFSET]);
				pDevDirExt->ImageTable.uSize = *((DWORD *) &pbyPointer[IMG_TBL_8_OFFSET + SEG_SIZE_OFFSET_8]);
#else
				pDevDirExt->ItemTable.offset = aligndata(&pbyPointer[ITEM_TBL_8_OFFSET]);
				pDevDirExt->ItemTable.uSize = aligndata(&pbyPointer[ITEM_TBL_8_OFFSET + SEG_SIZE_OFFSET_8 ]);
				pDevDirExt->StringTable.offset = aligndata(&pbyPointer[STRNG_TBL_8_OFFSET]);
				pDevDirExt->StringTable.uSize = aligndata(&pbyPointer[STRNG_TBL_8_OFFSET + SEG_SIZE_OFFSET_8]);
				pDevDirExt->DictReferenceTable.offset = aligndata(&pbyPointer[DICT_REF_TBL_8_OFFSET]);
				pDevDirExt->DictReferenceTable.uSize = aligndata(&pbyPointer[DICT_REF_TBL_8_OFFSET + SEG_SIZE_OFFSET_8]);
				pDevDirExt->CommandTable.offset = aligndata(&pbyPointer[CMD_TBL_8_OFFSET]);
				pDevDirExt->CommandTable.uSize = aligndata(&pbyPointer[CMD_TBL_8_OFFSET + SEG_SIZE_OFFSET_8]);
				pDevDirExt->ImageTable.offset = aligndata(&pbyPointer[IMG_TBL_8_OFFSET]);
				pDevDirExt->ImageTable.uSize = aligndata(&pbyPointer[IMG_TBL_8_OFFSET + SEG_SIZE_OFFSET_8]);
#endif
				/*Do Some Validations*/
				if(pDevDirExt->byLength < DEVICE_DIR_LENGTH_8) 
				{
					delete pDevDirExt;
					return false; /* INVALID_EXTN_LENGTH*/
				}
}
else // CodingMajor is 6 (or 7)
{
#ifndef _WIN32_WCE	// PAW aligment issues 29/05/09

	//LOGIT(CLOG_LOG, "CodingMajor is 6 (or 7)");


				pDevDirExt->BlockNameTable.offset = *((long_offset *) &pbyPointer[BLK_NAME_TBL_6_OFFSET]);
				pDevDirExt->BlockNameTable.uSize = *((DWORD *) &pbyPointer[BLK_NAME_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->ItemTable.offset = *((long_offset *) &pbyPointer[ITEM_TBL_6_OFFSET]);
				pDevDirExt->ItemTable.uSize = *((DWORD *) &pbyPointer[ITEM_TBL_6_OFFSET + SEG_SIZE_OFFSET_6 ]);
				pDevDirExt->ProgramTable.offset = *((long_offset *) &pbyPointer[PROG_TBL_6_OFFSET]);
				pDevDirExt->ProgramTable.uSize = *((DWORD *) &pbyPointer[PROG_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->DomainTable.offset = *((long_offset *) &pbyPointer[DOM_TBL_6_OFFSET]);
				pDevDirExt->DomainTable.uSize = *((DWORD *) &pbyPointer[DOM_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->StringTable.offset = *((long_offset *) &pbyPointer[STRNG_TBL_6_OFFSET]);
				pDevDirExt->StringTable.uSize = *((DWORD *) &pbyPointer[STRNG_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->DictReferenceTable.offset = *((long_offset *) &pbyPointer[DICT_REF_TBL_6_OFFSET]);
				pDevDirExt->DictReferenceTable.uSize = *((DWORD *) &pbyPointer[DICT_REF_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->LocalVariableTable.offset = *((long_offset *) &pbyPointer[LOC_VAR_TBL_6_OFFSET]);
				pDevDirExt->LocalVariableTable.uSize = *((DWORD *) &pbyPointer[LOC_VAR_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->CommandTable.offset = *((long_offset *) &pbyPointer[CMD_TBL_6_OFFSET]);
				pDevDirExt->CommandTable.uSize = *((DWORD *) &pbyPointer[CMD_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->ImageTable.offset = *((long_offset *) &pbyPointer[IMG_TBL_6_OFFSET]);
				pDevDirExt->ImageTable.uSize = *((DWORD *) &pbyPointer[IMG_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
#else
				pDevDirExt->BlockNameTable.offset = aligndata(&pbyPointer[BLK_NAME_TBL_6_OFFSET]);
				pDevDirExt->BlockNameTable.uSize = aligndata(&pbyPointer[BLK_NAME_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->ItemTable.offset = aligndata(&pbyPointer[ITEM_TBL_6_OFFSET]);
				pDevDirExt->ItemTable.uSize = aligndata(&pbyPointer[ITEM_TBL_6_OFFSET + SEG_SIZE_OFFSET_6 ]);
				pDevDirExt->ProgramTable.offset = aligndata(&pbyPointer[PROG_TBL_6_OFFSET]);
				pDevDirExt->ProgramTable.uSize = aligndata(&pbyPointer[PROG_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->DomainTable.offset = aligndata(&pbyPointer[DOM_TBL_6_OFFSET]);
				pDevDirExt->DomainTable.uSize = aligndata(&pbyPointer[DOM_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->StringTable.offset = aligndata(&pbyPointer[STRNG_TBL_6_OFFSET]);
				pDevDirExt->StringTable.uSize = aligndata(&pbyPointer[STRNG_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->DictReferenceTable.offset = aligndata(&pbyPointer[DICT_REF_TBL_6_OFFSET]);
				pDevDirExt->DictReferenceTable.uSize = aligndata(&pbyPointer[DICT_REF_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->LocalVariableTable.offset = aligndata(&pbyPointer[LOC_VAR_TBL_6_OFFSET]);
				pDevDirExt->LocalVariableTable.uSize = aligndata(&pbyPointer[LOC_VAR_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->CommandTable.offset = aligndata(&pbyPointer[CMD_TBL_6_OFFSET]);
				pDevDirExt->CommandTable.uSize = aligndata(&pbyPointer[CMD_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
				pDevDirExt->ImageTable.offset = aligndata(&pbyPointer[IMG_TBL_6_OFFSET]);
				pDevDirExt->ImageTable.uSize = aligndata(&pbyPointer[IMG_TBL_6_OFFSET + SEG_SIZE_OFFSET_6]);
#endif
				/*Do Some Validations*/
				if(pDevDirExt->byLength < DEVICE_DIR_LENGTH_6) //Vibhor 280904: Changed
				{
					delete pDevDirExt;
					return false; /* INVALID_EXTN_LENGTH*/
				}
}
#ifdef READ_DEBUG
if (CodingMajor <= 7)
{
	cout<<"BlockNameTable"<<endl;
	cout<<"offset:  0x"<<hex<<setw(4)<<setfill('0')<<pDevDirExt->BlockNameTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<setw(4)<<setfill('0')<<pDevDirExt->BlockNameTable.uSize <<dec<<endl<<endl;
}
	cout<<"ItemTable"<<endl;
	cout<<"offset:  0x"<<hex<<pDevDirExt->ItemTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pDevDirExt->ItemTable.uSize <<dec<<endl<<endl;
if (CodingMajor <= 7)
{
	cout<<"ProgramTable"<<endl;
	cout<<"offset:  0x"<<hex<<pDevDirExt->ProgramTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pDevDirExt->ProgramTable.uSize <<dec<<endl<<endl;
	cout<<"DomainTable"<<endl;
	cout<<"offset:  0x"<<hex<<pDevDirExt->DomainTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pDevDirExt->DomainTable.uSize <<dec<<endl<<endl;
}
	cout<<"StringTable"<<endl;
	cout<<"offset:  0x"<<hex<<pDevDirExt->StringTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pDevDirExt->StringTable.uSize <<dec<<endl<<endl;
	cout<<"DictReferenceTable"<<endl;
	cout<<"offset:  0x"<<hex<<pDevDirExt->DictReferenceTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pDevDirExt->DictReferenceTable.uSize <<dec<<endl<<endl;
if (CodingMajor <= 7)
{
	cout<<"LocalVariableTable"<<endl;
	cout<<"offset:  0x"<<hex<<pDevDirExt->LocalVariableTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pDevDirExt->LocalVariableTable.uSize <<dec<<endl<<endl;
}
	cout<<"CommandTable"<<endl;
	cout<<"offset:  0x"<<hex<<pDevDirExt->CommandTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pDevDirExt->CommandTable.uSize <<dec<<endl<<endl;
	cout<<"ImageTable"<<endl;
	cout<<"offset:  0x"<<hex<<pDevDirExt->ImageTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pDevDirExt->ImageTable.uSize <<dec<<endl<<endl;
	
	cout<<"---------------------------------------------"<<endl;
#endif

				if(pDevDirExt->byDeviceDirObjectCode != DEVICE_DIR_TYPE)
				{
					delete pDevDirExt;
					return false; /* DIR_TYPE_MISMATCH*/
				}

				if(!(binDevDir->bin_exists))
				{
					binDevDir->bin_exists = 0L;
					//LOGIT(CLOG_LOG, "if(!(binDevDir->bin_exists))");
					if(pDevDirExt->BlockNameTable.uSize)
						    binDevDir->bin_exists |= (1<<BLK_TBL_ID);
					if(pDevDirExt->ItemTable.uSize)
						    binDevDir->bin_exists |= (1<<ITEM_TBL_ID);
					if(pDevDirExt->ProgramTable.uSize)
						    binDevDir->bin_exists |= (1<<PROG_TBL_ID);
					if(pDevDirExt->DomainTable.uSize)
						    binDevDir->bin_exists |= (1<<DOMAIN_TBL_ID);
					if(pDevDirExt->StringTable.uSize)
						    binDevDir->bin_exists |= (1<<STRING_TBL_ID);
					if(pDevDirExt->DictReferenceTable.uSize)
						    binDevDir->bin_exists |= (1<<DICT_REF_TBL_ID);
					if(pDevDirExt->LocalVariableTable.uSize)
						    binDevDir->bin_exists |= (1<<LOCAL_VAR_TBL_ID);
					if(pDevDirExt->CommandTable.uSize)
						    binDevDir->bin_exists |= (1<<CMD_NUM_ID_TBL_ID);
					if(pDevDirExt->ImageTable.uSize)
							binDevDir->bin_exists |= (1<<IMAGE_TBL_ID);
				}

				/* start stevev added  01nov05 */
				/* handles the too long heap reference */

				if ( ObjectFixed[i].wDomainDataSize == 0xffff && pbyObjectValue[i] == NULL)
				{	
					int sizeTotal = 0;
					dirExtnOffset = &(pDevDirExt->StringTable);
					if (CodingMajor <= 7)
					{
						bRet = read_dword(&dwOffset,(void*)&(pDevDirExt->BlockNameTable.offset),FORMAT_BIG_ENDIAN);
						bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->BlockNameTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)
						{	sizeTotal += dwTblLength; }

						bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->ProgramTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)
						{	sizeTotal += dwTblLength; }
						bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->DomainTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)
						{	sizeTotal += dwTblLength; }

						bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->LocalVariableTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)
						{	sizeTotal += dwTblLength; }
					}
					else
					{
						bRet = read_dword(&dwOffset,(void*)&(pDevDirExt->ItemTable.offset),FORMAT_BIG_ENDIAN);

						//LOGIT(CLOG_LOG, "bRet = %d", bRet);


					}
					bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->ItemTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)
					{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->StringTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)
					{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->DictReferenceTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)
					{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->CommandTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)
					{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pDevDirExt->ImageTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)
					{	sizeTotal += dwTblLength; }
				
				
				// seek to heap
					long offset = ObjectFixed[i].longAddress
									+ header.header_size + header.objects_size;

					iRetVal = fseek(fp, (long)offset, 0);

					if (iRetVal < 0)
					{
						pbyObjectValue[i] = NULL;

						//LOGIT(CLOG_LOG, "pbyObjectValue[i] = NULL");

					}
					else
					{	// alloc size bytes to pbyObjectValue[i]
						pbyObjectValue[i] = new BYTE[sizeTotal];
						// read in size

						iRetVal = fread((BYTE *)pbyObjectValue[i], 1
												, sizeTotal, fp);
						if (iRetVal != sizeTotal)
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
                                    //delete[] pbyObjectValue;//via DD@F
									CleanArrays();
									LOGIT(CERR_LOG,L" Count mismatch without EOF and without a file error.\n");
								}
							}
							return false ;
						}
					}
#ifdef XXX
					//fp.seekg(offset, std::ios::beg);

					//if (!fp) {
					//	std::cerr << "Error seeking to position." << std::endl;
					//	fp.close();
					//	return false;
					//}
					//else
					//{
					//	// alloc size bytes to pbyObjectValue[i]
					//	pbyObjectValue[i] = new BYTE[sizeTotal];
					//	// read in size
					//	// Read 'sizeTotal' bytes into pbyObjectValue[i]
					//	fp.read(reinterpret_cast<char*>(pbyObjectValue[i]), sizeTotal);

					//	// Check if read operation was successful
					//	if (fp.gcount() != static_cast<std::streamsize>(sizeTotal)) {
					//		if (fp.eof()) {
					//			LOGIT(CERR_LOG, L"End of File reached unexpectedly.\n");
					//		}
					//		else if (fp.fail()) {
					//			LOGIT(CERR_LOG, "File read failure.\n");
					//		}
					//		else {
					//			CleanArrays();
					//			LOGIT(CERR_LOG, L" Count mismatch without EOF and without a file error.\n");
					//		}

					//		fp.close();
					//		return false;
					//	}

					//}
#endif

				}
				/* end added 01nov05 */
				unsigned short uTag = 0;
				unsigned long  ulTableMaskBit = 0L;
				binTablePtr = (BININFO*) 0L;
				unsigned long ulReqMask = DEVICE_TBL_MASKS;
				long lOffset = 0;

				while ((ulReqMask) && (uTag < MAX_DEVICE_TBL_ID_HCF_6)) 
				{
					/*
					* Check for request mask bit corresponding to the tag value.
					* Skip to next tag value if not requested.
					*/
					lOffset = 0;
 
					if (!((ulReqMask) & (1L << uTag))) 
					{
						uTag++;
						continue;
					}
					/*
					* Point to appropriate values for the table type
					*/

					switch (uTag++) 
					{
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
	bRet = read_dword(&dwOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);
	if(bRet == false)
	{
		delete pDevDirExt;
		return false;
	}

	bRet = read_dword(&dwTblLength,(void*)&dirExtnOffset->uSize,FORMAT_BIG_ENDIAN);
	if(bRet == false)
	{
		delete pDevDirExt;
		return false;
	}
#ifdef READ_DEBUG
	cout<<"item table"<<endl;
	cout<<"offset:  0x"<<hex<<setw(4)<<setfill('0')<<dwOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<setw(4)<<setfill('0')<<dwTblLength<<dec<<endl<<endl;
#endif
	/*
	 * Attach the table if non-zero length, else go
	 * to the next table
	 */

	if (dwTblLength) 
	{
		if(NULL != pbyObjectValue[i])
		{
			binTablePtr->chunk = pbyObjectValue[i] + dwOffset;
			binTablePtr->size = dwTblLength;
		}
		else
		{
			binTablePtr->size = dwTblLength;
			binTablePtr->chunk = new BYTE[dwTblLength];

			lOffset = ObjectFixed[i].longAddress
					  + header.header_size 
					  + header.objects_size
					  + dwTblLength;
//#ifndef _WIN32			
			iRetVal = fseek(fp, (long)lOffset, SEEK_SET);
			if(iRetVal < 0)
			{
				delete pDevDirExt;
				delete [] binTablePtr->chunk;
				binTablePtr->chunk = NULL;
				return false;
			}
			iRetVal = fread((BYTE *)binTablePtr->chunk, 1, dwTblLength, fp);
//#else
//			fp.seekg(lOffset, std::ios::beg);
//
//			if (!fp) {
//				delete pDevDirExt;
//				delete[] binTablePtr->chunk;
//				binTablePtr->chunk = NULL;
//				return false;
//			}
//
//			// Read 'dwTblLength' bytes into 'chunk'
//			fp.read(reinterpret_cast<char*>(binTablePtr->chunk), dwTblLength);
//
//			// Check if read operation was successful
//			if (fp.gcount() != static_cast<std::streamsize>(dwTblLength)) {
//				//LOGIT("Error reading file.");
//				fp.close();
//				return false;
//			}
//#endif
		}			
		binDevDir->bin_hooked |= ulTableMaskBit;
	}
	// no table available (0 length)
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
						ulTableMaskBit = STRING_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->StringTable);
						binTablePtr = &(binDevDir->string_tbl);
						ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
						if (!(binTablePtr->chunk)) 
						{
							bRet = read_dword(&dwOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);
							if(bRet == false)
							{
								delete pDevDirExt;
								return false;
							}

							bRet = read_dword(&dwTblLength,(void*)&dirExtnOffset->uSize,FORMAT_BIG_ENDIAN);
							if(bRet == false)
							{
								delete pDevDirExt;
								return false;
							}
#ifdef READ_DEBUG

	cout<<"string table"<<endl;
	cout<<"offset:  0x"<<hex<<setw(4)<<setfill('0')<<dwOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<setw(4)<<setfill('0')<<dwTblLength<<dec<<endl<<endl;
#endif

							/*
							 * Attach the table if non-zero length, else go
							 * to the next table
							 */

							if (dwTblLength) 
							{
								if(NULL != pbyObjectValue[i])
								{
									binTablePtr->chunk = pbyObjectValue[i] + dwOffset;
									binTablePtr->size = dwTblLength;
								}
								else
								{
									binTablePtr->size = dwTblLength;
									binTablePtr->chunk = new BYTE[dwTblLength];

									lOffset = ObjectFixed[i].longAddress
											  + header.header_size 
											  + header.objects_size
											  + dwTblLength;
//#ifndef _WIN32							
									iRetVal = fseek(fp, (long)lOffset, SEEK_SET);
									if(iRetVal < 0)
									{
										delete pDevDirExt;
										delete [] binTablePtr->chunk;
										binTablePtr->chunk = NULL;
										return false;
									}
									iRetVal = fread((BYTE *)binTablePtr->chunk, 1, dwTblLength, fp);
//#else
//									fp.seekg(lOffset, std::ios::beg);
//
//									if (!fp) {
//										delete pDevDirExt;
//										delete[] binTablePtr->chunk;
//										binTablePtr->chunk = NULL;
//										return false;
//									}
//
//									// Read 'dwTblLength' bytes into 'chunk'
//									fp.read(reinterpret_cast<char*>((BYTE*)binTablePtr->chunk), dwTblLength);
//
//									// Check if read operation was successful
//									if (fp.gcount() != static_cast<std::streamsize>(dwTblLength)) {
//										delete pDevDirExt;
//										delete[] binTablePtr->chunk;
//										binTablePtr->chunk = NULL;
//										return false;
//										
//									}
//#endif


								}
									
								binDevDir->bin_hooked |= ulTableMaskBit;
							}
					
						}
						break;

					case DICT_REF_TBL_ID:	/* Dictionary Reference Table */
						ulTableMaskBit = DICT_REF_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->DictReferenceTable);
						binTablePtr = &(binDevDir->dict_ref_tbl);
						ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
						if (!(binTablePtr->chunk)) {

						bRet = read_dword(&dwOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);

						if(bRet == false)
						{
							delete pDevDirExt;
							return false;
						}

						bRet = read_dword(&dwTblLength,(void*)&dirExtnOffset->uSize,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pDevDirExt;
							return false;
						}
#ifdef READ_DEBUG
	cout<<"dictionary table"<<endl;
	cout<<"offset:  0x"<<hex<<setw(4)<<setfill('0')<<dwOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<setw(4)<<setfill('0')<<dwTblLength<<dec<<endl<<endl;
#endif

						/*
						 * Attach the table if non-zero length, else go
						 * to the next table
						 */

						if (dwTblLength) {
								if(NULL != pbyObjectValue[i])
								{
									binTablePtr->chunk = pbyObjectValue[i] + dwOffset;
									binTablePtr->size = dwTblLength;
								}
								else
								{
									binTablePtr->size = dwTblLength;
									binTablePtr->chunk = new BYTE[dwTblLength];

									lOffset = ObjectFixed[i].longAddress
											  + header.header_size 
											  + header.objects_size
											  + dwTblLength;
//#ifndef _WIN32								
									iRetVal = fseek(fp, (long)lOffset, SEEK_SET);
									if(iRetVal < 0)
									{
										delete pDevDirExt;
										delete [] binTablePtr->chunk;
										binTablePtr->chunk = NULL;
										return false;
									}
									iRetVal = fread((BYTE *)binTablePtr->chunk, 1
											, dwTblLength, fp);
//#else
//									fp.seekg(lOffset, std::ios::beg);
//
//									if (!fp) {
//										delete pDevDirExt;
//										delete[] binTablePtr->chunk;
//										binTablePtr->chunk = NULL;
//										return false;
//									}
//
//									// Read 'dwTblLength' bytes into 'chunk'
//									fp.read(reinterpret_cast<char*>((BYTE*)binTablePtr->chunk), dwTblLength);
//
//									// Check if read operation was successful
//									if (fp.gcount() != static_cast<std::streamsize>(dwTblLength)) {
//										delete pDevDirExt;
//										delete[] binTablePtr->chunk;
//										binTablePtr->chunk = NULL;
//										return false;
//
//									}
//
//
//#endif

								}
								binDevDir->bin_hooked |= ulTableMaskBit;
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
dirExtnOffset  = &(pDevDirExt->CommandTable);
binTablePtr    = &(binDevDir->cmd_num_id_tbl); 

ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
if (!(binTablePtr->chunk)) 
{
	bRet = read_dword(&dwOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);
	if(bRet == false)
	{
		delete pDevDirExt;
		return false;
	}

	bRet = read_dword(&dwTblLength,(void*)&dirExtnOffset->uSize,FORMAT_BIG_ENDIAN);
	if(bRet == false)
	{
		delete pDevDirExt;
		return false;
	}
#ifdef READ_DEBUG
	cout<<"command number"<<endl;
	cout<<"offset:  0x"<<hex<<setw(4)<<setfill('0')<<dwOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<setw(4)<<setfill('0')<<dwTblLength<<dec<<endl<<endl;
#endif
	/*
	 * Attach the table if non-zero length, else go
	 * to the next table
	 */

	if (dwTblLength) 
	{
		if(NULL != pbyObjectValue[i])
		{
			binTablePtr->chunk = pbyObjectValue[i] + dwOffset;
			binTablePtr->size  = dwTblLength;
		}
		else
		{
			binTablePtr->chunk = new BYTE[dwTblLength];
			binTablePtr->size  = dwTblLength;

			lOffset = ObjectFixed[i].longAddress
					  + header.header_size 
					  + header.objects_size
					  + dwTblLength;
//#ifndef _WIN32			
			iRetVal = fseek(fp, (long)lOffset, SEEK_SET);
			if(iRetVal < 0)
			{
				delete pDevDirExt;
				delete [] binTablePtr->chunk;
				binTablePtr->chunk = NULL;
				return false;
			}
			iRetVal = fread((BYTE *)binTablePtr->chunk, 1, dwTblLength, fp);
//#else
//			fp.seekg(lOffset, std::ios::beg);
//
//			if (!fp) {
//				delete pDevDirExt;
//				delete[] binTablePtr->chunk;
//				binTablePtr->chunk = NULL;
//				return false;
//			}
//
//			// Read 'dwTblLength' bytes into 'chunk'
//			fp.read(reinterpret_cast<char*>((BYTE*)binTablePtr->chunk), dwTblLength);
//
//			// Check if read operation was successful
//			if (fp.gcount() != static_cast<std::streamsize>(dwTblLength)) {
//				//std::cerr << "Error reading file." << std::endl;
//				fp.close();
//				return false;
//			}
//
//#endif
		}			
		binDevDir->bin_hooked |= ulTableMaskBit;
	}
	// no table available (0 length)
}
					}
						break;

					case IMAGE_TBL_ID:

						ulTableMaskBit = IMAGE_TBL_MASK;
						dirExtnOffset = &(pDevDirExt->ImageTable);
						binTablePtr = &(binDevDir->image_tbl);
						ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
						if (!(binTablePtr->chunk)) 
						{
							bRet = read_dword(&dwOffset,(void*)&dirExtnOffset->offset,FORMAT_BIG_ENDIAN);

							if(bRet == false)
							{
								delete pDevDirExt;
								return false;
							}

							bRet = read_dword(&dwTblLength,(void*)&dirExtnOffset->uSize,FORMAT_BIG_ENDIAN);
							if(bRet == false)
							{
								delete pDevDirExt;
								return false;
							}
#ifdef READ_DEBUG
	cout<<"image table"<<endl;
	cout<<"offset:  0x"<<hex<<setw(4)<<setfill('0')<<dwOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<setw(4)<<setfill('0')<<dwTblLength<<dec<<endl<<endl;
#endif

							/*
							 * Attach the table if non-zero length, else go
							 * to the next table
							 */
							if (dwTblLength) 
							{
								if(NULL != pbyObjectValue[i])
								{
									binTablePtr->chunk = pbyObjectValue[i] + dwOffset;
									binTablePtr->size = dwTblLength;
								}
								else
								{
									binTablePtr->size = dwTblLength;
									binTablePtr->chunk = new BYTE[dwTblLength];

									lOffset = ObjectFixed[i].longAddress
											  + header.header_size 
											  + header.objects_size
											  + dwTblLength;
//#ifndef _WIN32									
									iRetVal = fseek(fp, (long)lOffset, SEEK_SET);
									if(iRetVal < 0)
									{
										delete pDevDirExt;
										delete [] binTablePtr->chunk;
										binTablePtr->chunk = NULL;
										return false;
									}
									iRetVal = fread((BYTE *)binTablePtr->chunk, 1
											, dwTblLength, fp);

//#else
//									fp.seekg(lOffset, std::ios::beg);
//
//									if (!fp) {
//										//std::cerr << "Error seeking to position." << std::endl;
//										fp.close();
//										return false;
//									}
//
//									// Read 'dwTblLength' bytes into 'chunk'
//									fp.read(reinterpret_cast<char*>((BYTE*)binTablePtr->chunk), dwTblLength);
//
//									// Check if read operation was successful
//									if (fp.gcount() != static_cast<std::streamsize>(dwTblLength)) {
//										//std::cerr << "Error reading file." << std::endl;
//										fp.close();
//										return false;
//									}
//
//
//#endif
								}
							
								binDevDir->bin_hooked |= ulTableMaskBit;
							}
					
						}
						break;

					default:	/* goes here for reserved or undefined table IDs */
						break;
					}
				}/* end while */
#ifdef READ_DEBUG					
	cout<<"---------End Device Directory ---------------"<<endl;
#endif

/*We have Fetched the Device dir binary chunks, now Evaluate the device directories*/

				// timj 9oct07  was eval_dir_device_tables_6()
#define DIR_TABLES_OF_INTEREST  ITEM_TBL_MASK|STRING_TBL_MASK | DICT_REF_TBL_MASK |CMD_NUM_ID_TBL_MASK|IMAGE_TBL_MASK 
				if (CodingMajor <= DDOD_REV_SUPPORTED_SIX)
					iRetVal = eval_dir_device_tables_6(flatDevDir, binDevDir,DIR_TABLES_OF_INTEREST);
				else
					iRetVal = eval_dir_device_tables_8(flatDevDir, binDevDir,DIR_TABLES_OF_INTEREST);

					/*DEVICE_TBL_MASKS );*/
				if(iRetVal != SUCCESS)
				{
					delete pDevDirExt;
					return false;
				}
				else
				if (flatDevDir->image_tbl.count > 0)// we have graphics
				{
					long lOffset, lPos, lSize, lLoc, lILen;
					int  iRetVal;
					lOffset =   header.header_size 
							  + header.objects_size
							  + header.data_size;
					lPos = ftell(fp);// save for others (may not be needed)
					fseek(fp, 0, SEEK_END);
					lSize = ftell(fp);// total file size

					iRetVal = fseek(fp, (long)lOffset, SEEK_SET);// first graphic byte
					lLoc    = ftell(fp);   
					lILen   = lSize - lLoc;// graphic size
					// debug
					if ( lLoc != lOffset )
					{// seek did not work
						LOGIT(CERR_LOG,L"Seek error brkpt.\n");
					}
					if ( lILen < 4 )
					{// some type of file error - or no graphics
						LOGIT(CERR_LOG,L"No Image data.(%d bytes for %d images)\n",
															lILen,flatDevDir->image_tbl.count);
					// stevev 11dec08, let it continue...	return DDL_SUCCESS;
					}
					// end debug
					pGraphics = new BYTE[lILen];// allocate memory
					iRetVal = fread(pGraphics, 1, lILen, fp);// read ALL the raw images
					if(iRetVal < 0)
					{
                        delete [] pGraphics;//via DD@F
						return false;
					}
					else
					{
						iRetVal = fseek(fp, (long)lPos, SEEK_SET);// back were we started (in case somebody cares)
						// insert raw pointers into the image table
						IMAGE_TBL_ELEM* pImgTblElem = flatDevDir->image_tbl.list;
						IMG_ITEM *      pImgItm;
						// for each image
						for ( unsigned x = 0; x < flatDevDir->image_tbl.count; x++, pImgTblElem++)
						{
							pImgItm = pImgTblElem->img_list;
						//	for each language
							for ( unsigned y = 0; y < pImgTblElem->num_langs; y++, pImgItm++)
							{// set the pointer to the raw graphic
								pImgItm->p2Graphik = pGraphics + min((int)(pImgItm->img_file.offset),lILen);
#ifdef _PARSER_DEBUG	/* * * * * * * */
								if ( (pImgItm->p2Graphik + pImgItm->img_file.uSize) > (pGraphics + lILen) )
								{
									cout<< "ERROR: IMAGE SIZE overrun.\n";
									cerr<< "ERROR: IMAGE SIZE overrun by " << 
										((pGraphics + lILen) - (pImgItm->p2Graphik + pImgItm->img_file.uSize) )
										<< " bytes."<<endl;
								}
#endif					/* * * * * * * */
							}
						}
					}
				}
				// else - no-op
				//TODO : See memory, if allocated to binTablePtr->chunk is freed properly.
				bDevDirLoadedFlag = true; /*Device Directory Loaded Successfully*/
				delete pDevDirExt;
			}
			break;
			case BLOCK_DIR_TYPE:
			{	// no classes in FLAT_BLOCK_DIR_6 nor BIN_BLOCK_DIR_6; memset ok
				(void)memset((char *) &block_dir_6,   0, sizeof(FLAT_BLOCK_DIR_6));
				(void)memset((char *) &bin_blk_dir_6, 0, sizeof(BIN_BLOCK_DIR_6));
   
				FLAT_BLOCK_DIR_6 *pflatBlkDir = &block_dir_6;
				BIN_BLOCK_DIR_6  *pbinBlkDir  = &bin_blk_dir_6;
			//	BININFO *binTablePtr;
			//	DATAPART_SEGMENT_6 *pBlkExtnOffset;
			//	DWORD dwOffset;
				DWORD dwTblLength;
				bool bRet = false;

#ifdef _PARSER_DEBUG
				cout<< i+1 <<" Block Directory Object: \n";
#endif
				BLOCK_DIR_EXT_6 *pBlkDirExt;
				BYTE *pbyPointer = (BYTE *)pbyExtensions[i];
					
				pBlkDirExt = new BLOCK_DIR_EXT_6;

				pBlkDirExt->byLength	         = pbyPointer[BLK_DIR_LENGTH_6_OFFSET];
				pBlkDirExt->byBlockDirObjectCode = pbyPointer[BLK_DIR_OBJ_CODE_6_OFFSET];
				pBlkDirExt->byFormatCode	     = pbyPointer[BLK_DIR_FORMAT_CODE_6_OFFSET];

#ifdef READ_DEBUG
				cout<<"----------- Block Directory -----------------"<<endl;
				cout<<"    Length:  0x"<<hex<<(int)pBlkDirExt->byLength<<dec<<endl;
				cout<<"ObjectCode:  0x"<<hex<<(int)pBlkDirExt->byBlockDirObjectCode<<dec<<endl;
				cout<<"FormatCode:  0x"<<hex<<(int)pBlkDirExt->byFormatCode<<dec<<endl;
#endif
#ifndef _WIN32_WCE	// PAW 02/05/09 causes alignment error
#define RD_DATAPARTSEG(d,s) \
		pBlkDirExt-> d .offset = *((long_offset *) &pbyPointer[ (s) ]);\
		pBlkDirExt-> d .uSize  = *((DWORD *) &pbyPointer[ (s) + SEG_SIZE_OFFSET_6])
#else
#define RD_DATAPARTSEG(d,s) \
		pBlkDirExt-> d .offset = aligndata(&pbyPointer[ (s) ]);\
		pBlkDirExt-> d .uSize  = aligndata(&pbyPointer[ (s) + SEG_SIZE_OFFSET_6])
#endif
				RD_DATAPARTSEG(BlockItemTable,				BLK_ITEM_TBL_6_OFFSET);
				RD_DATAPARTSEG(BlockItemNameTable,			BLK_ITEMNAME_TBL_6_OFFSET);
				RD_DATAPARTSEG(ParameterTable,				BLK_PARAM_TBL_6_OFFSET);
#ifdef READ_DEBUG
	cout<<"BlockItemTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->BlockItemTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->BlockItemTable.uSize <<dec<<endl<<endl;
	cout<<"BlockItemNameTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->BlockItemNameTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->BlockItemNameTable.uSize <<dec<<endl<<endl;
	cout<<"ParameterTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterTable.uSize <<dec<<endl<<endl;
#endif
				if (CodingMajor < DDOD_REV_SUPPORTED_EIGHT)
				{
					RD_DATAPARTSEG(ParameterMemberTable,		BLK_PARAMEMBER_TBL_6_OFFSET);
					RD_DATAPARTSEG(ParameterMemberNameTable,	BLK_PARAMEMBERNAME_TBL_6_OFFSET);
					RD_DATAPARTSEG(ParameterElementTable,		BLK_ELEMENT_TBL_6_OFFSET);
					RD_DATAPARTSEG(ParameterListTable,			BLK_PARAMLIST_TBL_6_OFFSET);
					RD_DATAPARTSEG(ParameterListMemberTable,	BLK_PARAMLISTMEMBER_TBL_6_OFFSET);
					RD_DATAPARTSEG(ParameterListMemberNameTable,BLK_PARAMLISTMEMBERNAME_TBL_6_OFFSET);
					RD_DATAPARTSEG(CharectersiticsMemberTable,	BLK_CHARMEMBER_TBL_6_OFFSET);
					RD_DATAPARTSEG(CharectersiticsMemberNameTable,BLK_CHARMEMBERNAME_TBL_6_OFFSET);

					RD_DATAPARTSEG(RelationTable,				BLK_RELATION_TBL_6_OFFSET);
					RD_DATAPARTSEG(UpdateTable,					BLK_UPDATE_TBL_6_OFFSET);
					RD_DATAPARTSEG(ParameterCommandTable,		BLK_PARAM2COMMAND_TBL_6_OFFSET);
					RD_DATAPARTSEG(CriticalParameterTable,		BLK_CRITICALPARAM_TBL_6_OFFSET);
					/*Do Some Validations*/
					if(pBlkDirExt->byLength < BLK_DIR_LENGTH_6)//was BLK_DIR_LEN_HCF)
					{
						delete pBlkDirExt;
						return false; /* INVALID_EXTN_LENGTH*/
					}
#ifdef READ_DEBUG
	cout<<"ParameterMemberTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterMemberTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterMemberTable.uSize <<dec<<endl<<endl;
	cout<<"ParameterMemberNameTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterMemberNameTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterMemberNameTable.uSize <<dec<<endl<<endl;
	cout<<"ParameterElementTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterElementTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterElementTable.uSize <<dec<<endl<<endl;
	cout<<"ParameterListTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterListTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterListTable.uSize <<dec<<endl<<endl;
	cout<<"ParameterListMemberTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterListMemberTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterListMemberTable.uSize <<dec<<endl<<endl;
	cout<<"ParameterListMemberNameTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterListMemberNameTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterListMemberNameTable.uSize <<dec<<endl<<endl;
	cout<<"CharectersiticsMemberTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->CharectersiticsMemberTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->CharectersiticsMemberTable.uSize <<dec<<endl<<endl;
	cout<<"CharectersiticsMemberNameTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->CharectersiticsMemberNameTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->CharectersiticsMemberNameTable.uSize <<dec<<endl<<endl;
	cout<<"RelationTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->RelationTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->RelationTable.uSize <<dec<<endl<<endl;
	cout<<"UpdateTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->UpdateTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->UpdateTable.uSize <<dec<<endl<<endl;
	cout<<"ParameterCommandTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterCommandTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterCommandTable.uSize <<dec<<endl<<endl;
	cout<<"CriticalParameterTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->CriticalParameterTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->CriticalParameterTable.uSize <<dec<<endl<<endl;
#endif
				}
				else// eight and above
				{
					RD_DATAPARTSEG(RelationTable,				BLK_RELATION_TBL_8_OFFSET);
					RD_DATAPARTSEG(UpdateTable,					BLK_UPDATE_TBL_8_OFFSET);
					RD_DATAPARTSEG(ParameterCommandTable,		BLK_PARAM2COMMAND_TBL_8_OFFSET);
					RD_DATAPARTSEG(CriticalParameterTable,		BLK_CRITICALPARAM_TBL_8_OFFSET);
					/*Do Some Validations*/
					if(pBlkDirExt->byLength < BLK_DIR_LENGTH_8)
					{
						delete pBlkDirExt;
						return false; /* INVALID_EXTN_LENGTH*/
					}
#ifdef READ_DEBUG
	cout<<"RelationTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->RelationTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->RelationTable.uSize <<dec<<endl<<endl;
	cout<<"UpdateTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->UpdateTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->UpdateTable.uSize <<dec<<endl<<endl;
	cout<<"ParameterCommandTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->ParameterCommandTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->ParameterCommandTable.uSize <<dec<<endl<<endl;
	cout<<"CriticalParameterTable"<<endl;
	cout<<"offset:  0x"<<hex<<pBlkDirExt->CriticalParameterTable.offset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<pBlkDirExt->CriticalParameterTable.uSize <<dec<<endl<<endl;
#endif
				}


				if(pBlkDirExt->byBlockDirObjectCode != BLOCK_DIR_TYPE)
				{
					delete pBlkDirExt;
					return false; /* DIR_TYPE_MISMATCH*/
				}

				if( pbinBlkDir->bin_exists == 0 )
				{
					if(pBlkDirExt->BlockItemTable.uSize)
						    pbinBlkDir->bin_exists |= (1<<BLK_ITEM_TBL_ID);					
					if(pBlkDirExt->BlockItemNameTable.uSize)
						    pbinBlkDir->bin_exists |= (1<<BLK_ITEM_NAME_TBL_ID);
					if(pBlkDirExt->ParameterTable.uSize)
						    pbinBlkDir->bin_exists |= (1<<PARAM_TBL_ID);
					if (CodingMajor < DDOD_REV_SUPPORTED_EIGHT)
					{
						if(pBlkDirExt->ParameterMemberTable.uSize)
								pbinBlkDir->bin_exists |= (1<<PARAM_MEM_TBL_ID);
						if(pBlkDirExt->ParameterMemberNameTable.uSize)
								pbinBlkDir->bin_exists |= (1<<PARAM_MEM_NAME_TBL_ID);
						if(pBlkDirExt->ParameterElementTable.uSize)
								pbinBlkDir->bin_exists |= (1<<PARAM_ELEM_TBL_ID);
						if(pBlkDirExt->ParameterListTable.uSize)
								pbinBlkDir->bin_exists |= (1<<PARAM_LIST_TBL_ID);
						if(pBlkDirExt->ParameterListMemberTable.uSize)
								pbinBlkDir->bin_exists |= (1<<PARAM_LIST_MEM_TBL_ID);
						if(pBlkDirExt->ParameterListMemberNameTable.uSize)
								pbinBlkDir->bin_exists |= (1<<PARAM_LIST_MEM_NAME_TBL_ID);
						if(pBlkDirExt->CharectersiticsMemberTable.uSize)
								pbinBlkDir->bin_exists |= (1<<CHAR_MEM_TBL_ID);
						if(pBlkDirExt->CharectersiticsMemberNameTable.uSize)
								pbinBlkDir->bin_exists |= (1<<CHAR_MEM_NAME_TBL_ID);
					}// else they ain't there
					if(pBlkDirExt->RelationTable.uSize)
						    pbinBlkDir->bin_exists |= (1<<REL_TBL_ID);
					if(pBlkDirExt->UpdateTable.uSize)
						    pbinBlkDir->bin_exists |= (1<<UPDATE_TBL_ID);
					if(pBlkDirExt->ParameterCommandTable.uSize)
						    pbinBlkDir->bin_exists |= (1<<COMMAND_TBL_ID);
					if(pBlkDirExt->CriticalParameterTable.uSize)
						    pbinBlkDir->bin_exists |= (1<<CRIT_PARAM_TBL_ID);
				}

				/* handle the too long heap reference */
				if ( ObjectFixed[i].wDomainDataSize == 0xffff && pbyObjectValue[i] == NULL)
				{	
					int sizeTotal = 0;
					
					bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->BlockItemTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)	{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->BlockItemNameTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)	{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->ParameterTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)	{	sizeTotal += dwTblLength; }
					if (CodingMajor < DDOD_REV_SUPPORTED_EIGHT)
					{
						bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->ParameterMemberTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)	{	sizeTotal += dwTblLength; }
						bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->ParameterMemberNameTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)	{	sizeTotal += dwTblLength; }
						bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->ParameterElementTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)	{	sizeTotal += dwTblLength; }
						bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->ParameterListTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)	{	sizeTotal += dwTblLength; }
						bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->ParameterListMemberTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)	{	sizeTotal += dwTblLength; }
						bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->ParameterListMemberNameTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)	{	sizeTotal += dwTblLength; }
						bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->CharectersiticsMemberTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)	{	sizeTotal += dwTblLength; }
						bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->CharectersiticsMemberNameTable.uSize),FORMAT_BIG_ENDIAN);
						if(bRet != false)	{	sizeTotal += dwTblLength; }
					}// else the tables are not there
					bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->RelationTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)	{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->UpdateTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)	{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->ParameterCommandTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)	{	sizeTotal += dwTblLength; }
					bRet = read_dword(&dwTblLength,(void*)&(pBlkDirExt->CriticalParameterTable.uSize),FORMAT_BIG_ENDIAN);
					if(bRet != false)	{	sizeTotal += dwTblLength; }


				// seek to heap
					long offset = ObjectFixed[i].longAddress
									+ header.header_size + header.objects_size;

					iRetVal = fseek(fp, (long)offset, 0);
					if (iRetVal < 0)
					{
						pbyObjectValue[i] = NULL;
					}
					else
					{// alloc size bytes to pbyObjectValue[i]
						pbyObjectValue[i] = new BYTE[sizeTotal];
						// read in size

						iRetVal = fread((BYTE *)pbyObjectValue[i], 1
												, sizeTotal, fp);
						if (iRetVal != sizeTotal)
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
                                    //delete[] pbyObjectValue;//via DD@F
									CleanArrays();
									LOGIT(CERR_LOG,L" Count mismatch without EOF and without a file error.\n");
								}
							}
							return false ;
						}
					}
				}
				
				unsigned short uTag = 0;
				unsigned long  ulTableMaskBit = 0L;
				//binTablePtr = (BININFO*) 0L;
				unsigned long ulReqMask = BLOCK_TBL_MASKS_HCF;
				DATAPART_SEGMENT_6 *tblExtnOffset;

				unsigned long   lOffset = 0, lSize = 0;
				unsigned long   size, tag;
				unsigned char  *chunk;
				UINT64          LL;
	
				if(pbyObjectValue[i] == NULL)
				{
					delete pBlkDirExt;
					return false;
				}

				while ((ulReqMask) && (uTag < MAX_BLOCK_TBL_ID_HCF)) 
				{  /*Check for request mask bit corresponding to the tag value.
					* Skip to next tag value if not requested.
					*/
					lOffset = 0;
					size    = 0;
 
					if (!((ulReqMask) & (1L << uTag))) 
					{
						uTag++;
						continue;
					}
					/*
					* Process Tables
					*/

					switch (uTag++) 
					{
					case BLK_ITEM_TBL_ID:
					{
						ulTableMaskBit = BLK_ITEM_TBL_MASK;
						//binTablePtr = &(binDevDir->item_tbl); 
						tblExtnOffset = &(pBlkDirExt->BlockItemTable);

						ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
						
						bRet = read_dword(&lOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}

						bRet = read_dword(&lSize,(void*)&tblExtnOffset->uSize,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}
#ifdef READ_DEBUG
	cout<<"block item table"<<endl;
	cout<<"offset:  0x"<<hex<<lOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<lSize<<dec<<endl<<endl;
#endif	

						if (lSize)
						{
							int rc; // for parse integer func
							BLK_ITEM_TBL_ELEM* pItmTblElem, *pEndItemTblElem;
							chunk = pbyObjectValue[i] + lOffset;
							size  = lSize;
							unsigned long cnt = 0;
							UINT64        temp_int;

							BLK_ITEM_TBL* pFlatItemTbl = &(pflatBlkDir->blk_item_tbl);
							
							DDL_PARSE_INTEGER(&chunk, &size, &LL); cnt = (ulong)LL;
							pFlatItemTbl->count = cnt;
							pFlatItemTbl->list  = new BLK_ITEM_TBL_ELEM[(size_t)cnt];

							if (pFlatItemTbl->list == NULL) 
							{
								delete pBlkDirExt;
								pFlatItemTbl->count = 0;
								return false;// out-of-memory error
							}
							// clear the table
							memset((char *) pFlatItemTbl->list,0,cnt * sizeof(BLK_ITEM_TBL_ELEM));//ok

							// load the list
							//
							for (pItmTblElem     = pFlatItemTbl->list,	
								 pEndItemTblElem = pItmTblElem + cnt;
								 pItmTblElem < pEndItemTblElem;      pItmTblElem++) 
							{
								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								pItmTblElem->blk_item_id = (ITEM_ID) temp_int;

								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								pItmTblElem->blk_item_name_tbl_offset = (int)temp_int;
							}

						}// else no table
						else
						{
							pflatBlkDir->blk_item_tbl.count = 0;
							pflatBlkDir->blk_item_tbl.list  = NULL;
						}
					}
					break;	
					case BLK_ITEM_NAME_TBL_ID:
					{
						ulTableMaskBit = BLK_ITEM_NAME_TBL_MASK;
						//binTablePtr = &(binDevDir->item_tbl); 
						tblExtnOffset = &(pBlkDirExt->BlockItemNameTable);

						ulReqMask &= ~ulTableMaskBit;	/* clear request mask bit */
						
						bRet = read_dword(&lOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}

						bRet = read_dword(&lSize,(void*)&tblExtnOffset->uSize,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}	
#ifdef READ_DEBUG
	cout<<"block item name table"<<endl;
	cout<<"offset:  0x"<<hex<<lOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<lSize<<dec<<endl<<endl;
#endif
						if (lSize)
						{
							int rc; // for parse integer func
							BLK_ITEM_NAME_TBL_ELEM* pItmTblElem;
							chunk = pbyObjectValue[i] + lOffset;
							size  = lSize;
							unsigned long item = 0;
							UINT64 cnt = 0, temp_int = 0;

							BLK_ITEM_NAME_TBL* pFlatItemTbl = &(pflatBlkDir->blk_item_name_tbl);
							
							DDL_PARSE_INTEGER(&chunk, &size, &cnt);
							pFlatItemTbl->count = (int)cnt;
							pFlatItemTbl->list  = new BLK_ITEM_NAME_TBL_ELEM[(size_t)cnt];

							if (pFlatItemTbl->list == NULL) 
							{
								delete pBlkDirExt;
								pFlatItemTbl->count = 0;
								return false;// out-of-memory error
							}
							// clear the table
							memset((char *) pFlatItemTbl->list,0,
										((size_t)cnt) * sizeof(BLK_ITEM_NAME_TBL_ELEM));//ok
							// load the list
							//
							pItmTblElem = (pFlatItemTbl->list) -1;//less first increment
							item        = -1;					  // ditto
							while (size)
							{// tagged ints...tag,implicit,value - most are optional
								DDL_PARSE_TAG(&chunk,&size, &tag,(ulong *) NULL_PTR);
								switch (tag)
								{
								case BINT_BLK_ITEM_NAME_TAG: // req'd
									{
										pItmTblElem += 1;
										item++;
										// if (item == cnt) return DDL_ENCODING_ERROR;

										// clear the optional data elements
										pItmTblElem->param_tbl_offset      = TABLE_OFFSET_INVALID;
										pItmTblElem->param_list_tbl_offset = TABLE_OFFSET_INVALID;
										pItmTblElem->rel_tbl_offset        = TABLE_OFFSET_INVALID;
										pItmTblElem->read_cmd_tbl_offset   = TABLE_OFFSET_INVALID;
										pItmTblElem->read_cmd_count        = 0;
										pItmTblElem->write_cmd_tbl_offset  = TABLE_OFFSET_INVALID;
										pItmTblElem->write_cmd_count       = 0;

										DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
										pItmTblElem->blk_item_name = (ITEM_ID) temp_int;
									}
									break;
								case BINT_ITEM_TBL_OFFSET_TAG:  // req'd
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->item_tbl_offset = (int)temp_int;
									break;
								case BINT_PARAM_TBL_OFFSET_TAG:
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->param_tbl_offset = (int)temp_int;
									break;
								case BINT_PARAM_LIST_TBL_OFFSET_TAG:
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->param_list_tbl_offset = (int)temp_int;
									break;
								case BINT_REL_TBL_OFFSET_TAG:
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->rel_tbl_offset = (int)temp_int;
									break;
								case BINT_READ_CMD_TBL_OFFSET_TAG:
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->read_cmd_tbl_offset = (int)temp_int;
									break;
								case BINT_READ_CMD_TBL_COUNT_TAG:
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->read_cmd_count = (int)temp_int;
									break;
								case BINT_WRITE_CMD_TBL_OFFSET_TAG:
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->write_cmd_tbl_offset = (int)temp_int;
									break;
								case BINT_WRITE_CMD_TBL_COUNT_TAG:
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->write_cmd_count = (int)temp_int;
									break;
								default:
									delete pBlkDirExt;
									return false;// DDL_ENCODING_ERROR
									break;
								}// end switch on tag for optional table elements								
							}// wend more size to parse
						}// else no table
						else
						{
							pflatBlkDir->blk_item_name_tbl.count = 0;
							pflatBlkDir->blk_item_name_tbl.list  = NULL;
						}
					}
					break;	
					case PARAM_TBL_ID:
					{
						ulTableMaskBit = PARAM_TBL_MASK;
						tblExtnOffset  = &(pBlkDirExt->ParameterTable);
						ulReqMask     &= ~ulTableMaskBit;	/* clear request mask bit */
						
						bRet = read_dword(&lOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}

						bRet = read_dword(&lSize,(void*)&tblExtnOffset->uSize,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}	
#ifdef READ_DEBUG
	cout<<"parameter table"<<endl;
	cout<<"offset:  0x"<<hex<<lOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<lSize<<dec<<endl<<endl;
#endif
						if (lSize)
						{
							int rc; // for parse integer func
							PARAM_TBL_ELEM* pItmTblElem;
							chunk = pbyObjectValue[i] + lOffset;
							size  = lSize;
							unsigned long item = 0;
							UINT64  cnt = 0, temp_int = 0 ;

							PARAM_TBL* pFlatItemTbl = &(pflatBlkDir->param_tbl);
							
							DDL_PARSE_INTEGER(&chunk, &size, &cnt);
							pFlatItemTbl->count = (int)cnt;
							pFlatItemTbl->list  = new PARAM_TBL_ELEM[(size_t)cnt];

							if (pFlatItemTbl->list == NULL) 
							{
								delete pBlkDirExt;
								pFlatItemTbl->count = 0;
								return false;// out-of-memory error
							}
							// clear the table
							memset((char *) pFlatItemTbl->list,0, 
												((size_t)cnt) * sizeof(PARAM_TBL_ELEM));//ok
							// load the list
							//
							pItmTblElem = (pFlatItemTbl->list) -1;//less first increment
							item        = -1;					  // ditto
							while (size)
							{// tagged ints...tag,implicit,value - most are optional
								DDL_PARSE_TAG(&chunk,&size, &tag,(ulong *) NULL_PTR);
								switch (tag)
								{
								case PT_BLK_ITEM_NAME_TBL_OFFSET_TAG: // req'd
									{
										pItmTblElem += 1;
										item++;
										// if (item > cnt) return DDL_ENCODING_ERROR;

										// clear the optional data elements
										pItmTblElem->blk_item_name_tbl_offset  = 
										pItmTblElem->param_mem_tbl_offset      = 
										pItmTblElem->param_elem_tbl_offset     = 
										pItmTblElem->array_elem_item_tbl_offset= 
											TABLE_OFFSET_INVALID;									

										DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
										pItmTblElem->blk_item_name_tbl_offset = (int)temp_int;
									}
									break;
								case PT_PARAM_MEM_TBL_OFFSET_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->param_mem_tbl_offset = (int)temp_int;
									break;
								case PT_PARAM_MEM_COUNT_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->param_mem_count = (int)temp_int;
									break;
								case PT_PARAM_ELEM_TBL_OFFSET_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->param_elem_tbl_offset = (int)temp_int;
									break;	
								case PT_PARAM_ELEM_COUNT_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->param_elem_count = (int)temp_int;
									break;	
								case PT_PARAM_ELEM_MAX_COUNT_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->array_elem_count = (int)temp_int;
									break;	
								case PT_ARRAY_ELEM__ITEM_TBL_OFFSET_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->array_elem_item_tbl_offset = (int)temp_int;
									break;	
								case PT_ARRAY_ELEM_TYPE_OR_VAR_TYPE_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->array_elem_type_or_var_type = (int)temp_int;
									break;	
								case PT_ARRAY_ELEM_SIZE_OR_VAR_SIZE_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->array_elem_size_or_var_size = (int)temp_int;
									break;	
								case PT_ARRAY_ELEM_CLASS_VAR_CLASS_TAG:  
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->array_elem_class_or_var_class = (ulong)temp_int;
									break;							
								default:
									delete pBlkDirExt;
									return false;// DDL_ENCODING_ERROR
									break;
								}// end switch on tag for optional table elements								
							}// wend more size to parse
						}// else no table
						else
						{
							pflatBlkDir->param_tbl.count = 0;
							pflatBlkDir->param_tbl.list  = NULL;
						}

					}
					break;				
								case PARAM_MEM_TBL_ID:
								{
									ulTableMaskBit = PARAM_MEM_TBL_MASK;
									tblExtnOffset  = &(pBlkDirExt->ParameterMemberTable);
									ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
								}
								break;			
								case PARAM_MEM_NAME_TBL_ID:
								{
									ulTableMaskBit = PARAM_MEM_NAME_TBL_MASK;
									tblExtnOffset  = &(pBlkDirExt->ParameterMemberNameTable);
									ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
								}
								break;		
								case PARAM_ELEM_TBL_ID:
								{
									ulTableMaskBit = PARAM_ELEM_TBL_MASK;
									tblExtnOffset  = &(pBlkDirExt->ParameterElementTable);
									ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
								}
								break;			
								case PARAM_LIST_TBL_ID:
								{
									ulTableMaskBit = PARAM_LIST_TBL_MASK;
									tblExtnOffset  = &(pBlkDirExt->ParameterListTable);
									ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
								}
								break;			
								case PARAM_LIST_MEM_TBL_ID:
								{
									ulTableMaskBit = PARAM_LIST_MEM_TBL_MASK;
									tblExtnOffset  = &(pBlkDirExt->ParameterListMemberTable);
									ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
								}
								break;		
								case PARAM_LIST_MEM_NAME_TBL_ID:
								{
									ulTableMaskBit = PARAM_LIST_MEM_NAME_TBL_MASK;
									tblExtnOffset  = &(pBlkDirExt->ParameterListMemberNameTable);
									ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
								}
								break;	
								case CHAR_MEM_TBL_ID:
								{
									ulTableMaskBit = CHAR_MEM_TBL_MASK;
									tblExtnOffset  = &(pBlkDirExt->CharectersiticsMemberTable);
									ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
								}
								break;				
								case CHAR_MEM_NAME_TBL_ID:
								{
									ulTableMaskBit = CHAR_MEM_NAME_TBL_MASK;
									tblExtnOffset  = &(pBlkDirExt->CharectersiticsMemberNameTable);
									ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
								}
								break;		
					case REL_TBL_ID:
					{
						ulTableMaskBit = REL_TBL_MASK;
						tblExtnOffset  = &(pBlkDirExt->RelationTable);
						ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */

						bRet = read_dword(&lOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}

						bRet = read_dword(&lSize,(void*)&tblExtnOffset->uSize,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}	
#ifdef READ_DEBUG
	cout<<"relation table"<<endl;
	cout<<"offset:  0x"<<hex<<lOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<lSize<<dec<<endl<<endl;
#endif

						if (lSize)
						{
							int rc; // for parse integer func
							REL_TBL_ELEM* pItmTblElem, *pEndItemTblElem;
							chunk = pbyObjectValue[i] + lOffset;
							size  = lSize;
							unsigned long item = 0;
							UINT64    cnt = 0, temp_int = 0;

							REL_TBL* pFlatItemTbl = &(pflatBlkDir->rel_tbl);
							
							DDL_PARSE_INTEGER(&chunk, &size, &cnt);
							if (cnt == 0) {
								// if cnt is zero, then 
								//   don't allocate the block, since it will not be freed later
								// initialize all varibles from this case, and break out early.
								pFlatItemTbl->count = 0;
								pFlatItemTbl->list = NULL;
								break;// out of switch
							} 

							pFlatItemTbl->count = (int)cnt;
							pFlatItemTbl->list  = new REL_TBL_ELEM[(size_t)cnt];

							if (pFlatItemTbl->list == NULL) 
							{
								delete pBlkDirExt;
								pFlatItemTbl->count = 0;
								return false;// out-of-memory error
							}
							// clear the table
							memset((char *) pFlatItemTbl->list,0, 
													((size_t)cnt) * sizeof(REL_TBL_ELEM));//ok
							// load the list
							//
							// all are required, load 'em in sequence
							
							for (pItmTblElem     = pFlatItemTbl->list,	
								 pEndItemTblElem = pItmTblElem + cnt;
								 pItmTblElem < pEndItemTblElem;      pItmTblElem++) 
							{
								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								if (temp_int == UNUSED_OFFSET)
									pItmTblElem->wao_item_tbl_offset = TABLE_OFFSET_INVALID;
								else 
									pItmTblElem->wao_item_tbl_offset = (int)temp_int;

								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								if (temp_int == UNUSED_OFFSET) 
									pItmTblElem->unit_item_tbl_offset = TABLE_OFFSET_INVALID;
								else 
									pItmTblElem->unit_item_tbl_offset = (int)temp_int;

								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								if (temp_int == UNUSED_OFFSET) 
									pItmTblElem->update_tbl_offset = TABLE_OFFSET_INVALID;
								else 
									pItmTblElem->update_tbl_offset = (int) temp_int;

								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								pItmTblElem->update_count = (int) temp_int;

								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								pItmTblElem->unit_count = (int) temp_int;
							}// next element
						}// else no table
						else
						{
							pflatBlkDir->rel_tbl.count = 0;
							pflatBlkDir->rel_tbl.list  = NULL;
						}
					}
					break;					
					case UPDATE_TBL_ID:
					{
						ulTableMaskBit = UPDATE_TBL_MASK;
						tblExtnOffset  = &(pBlkDirExt->UpdateTable);
						ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */

						bRet = read_dword(&lOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}
						bRet = read_dword(&lSize,(void*)&tblExtnOffset->uSize,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}	
#ifdef READ_DEBUG
	cout<<"update table"<<endl;
	cout<<"offset:  0x"<<hex<<lOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<lSize<<dec<<endl<<endl;
#endif	

						if (lSize)
						{
							int rc; // for parse integer func
							UPDATE_TBL_ELEM* pItmTblElem, *pEndItemTblElem;
							chunk = pbyObjectValue[i] + lOffset;
							size  = lSize;
							unsigned long item = 0;
							UINT64   cnt = 0, temp_int = 0;

							UPDATE_TBL* pFlatItemTbl = &(pflatBlkDir->update_tbl);
							
							DDL_PARSE_INTEGER(&chunk, &size, &cnt);
							if (cnt == 0) {
								// if cnt is zero, then 
								//    don't allocate the block, since it will not be freed later
								// initialize all varibles from this case, and break out early.
								pFlatItemTbl->count = 0;
								pFlatItemTbl->list = NULL;
								break;// out of switch
							}

							pFlatItemTbl->count = (int)cnt;
							pFlatItemTbl->list  = new struct UPDATE_TBL_ELEM[(size_t)cnt];

							if (pFlatItemTbl->list == NULL) 
							{
								delete pBlkDirExt;
								pFlatItemTbl->count = 0;
								return false;// out-of-memory error
							}
							// clear the table
							memset((char *) pFlatItemTbl->list,0, 
												((size_t)cnt) * sizeof(UPDATE_TBL_ELEM));//ok
							// load the list
							//
							// all are required, load 'em in sequence
							
							for (pItmTblElem     = pFlatItemTbl->list,	
								 pEndItemTblElem = pItmTblElem + cnt;
								 pItmTblElem < pEndItemTblElem;      pItmTblElem++) 
							{
								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								pItmTblElem->desc_it_offset = (int)temp_int;

								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								pItmTblElem->op_it_offset = (int)temp_int;

								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								pItmTblElem->op_subindex = (int) temp_int;
							}// next element
						}// else no table
						else
						{
							pflatBlkDir->update_tbl.count = 0;
							pflatBlkDir->update_tbl.list  = NULL;
						}
					}
					break;				
					case COMMAND_TBL_ID	:
					{
						ulTableMaskBit = COMMAND_TBL_MASK;
						tblExtnOffset  = &(pBlkDirExt->ParameterCommandTable);
						ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */
						//command_tbl

						bRet = read_dword(&lOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}
						bRet = read_dword(&lSize,(void*)&tblExtnOffset->uSize,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}	
#ifdef READ_DEBUG
	cout<<"command to parameter table"<<endl;
	cout<<"offset:  0x"<<hex<<lOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<lSize<<dec<<endl<<endl;
#endif	

						if (lSize)
						{
							if (CodingMajor == DDOD_REV_SUPPORTED_SIX)
							{							
								int rc; // for parse integer func
								COMMAND_TBL_ELEM* pItmTblElem, *pEndItemTblElem;
								COMMAND_INDEX*    pCmdIndex,   *pEndCmdIndex;
								chunk = pbyObjectValue[i] + lOffset;
								size  = lSize;
								unsigned long item = 0;
								UINT64  cnt = 0, temp_int = 0;

								COMMAND_TBL* pFlatItemTbl = &(pflatBlkDir->command_tbl);
								
								DDL_PARSE_INTEGER(&chunk, &size, &cnt);
								pFlatItemTbl->count = (int)cnt;
								pFlatItemTbl->list  = new struct COMMAND_TBL_ELEM[(size_t)cnt];

								if (pFlatItemTbl->list == NULL) 
								{
									delete pBlkDirExt;
									pFlatItemTbl->count = 0;
									return false;// out-of-memory error
								}
								// clear the table
								memset((char *) pFlatItemTbl->list,0, 
												((size_t)cnt) * sizeof(COMMAND_TBL_ELEM));//ok
								// load the list
								//
								// all are required, load 'em in sequence
								
								for (pItmTblElem     = pFlatItemTbl->list,	
									 pEndItemTblElem = pItmTblElem + cnt;
									 pItmTblElem < pEndItemTblElem;      pItmTblElem++) 
								{
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->subindex = (unsigned short) temp_int;

									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->number = (unsigned long) temp_int;

									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->transaction = (unsigned long) temp_int;

									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->weight = (unsigned short) temp_int;

									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->count = (int) temp_int;

									assert( temp_int < DDL_INT32_MAX );
									//
									// if there are index elements, parse them
									//
									if (temp_int != 0) 
									{	// make the list 
										pItmTblElem->index_list = 
										(COMMAND_INDEX*)new BYTE[((int)temp_int * sizeof(COMMAND_INDEX))];

										if (pItmTblElem->index_list == NULL) 
										{
											pItmTblElem->count = 0;
											delete pBlkDirExt;
											return false;// out-of-memory error
										}
										//
										// load the list of indexes
										//
										for (pCmdIndex    = pItmTblElem->index_list,  
											 pEndCmdIndex = pCmdIndex + temp_int;
											 pCmdIndex < pEndCmdIndex;				pCmdIndex++) 
										{
											DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
											pCmdIndex->id= (ITEM_ID) temp_int;

											DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
											pCmdIndex->value = (unsigned long) temp_int;

										}// next index
									}
									else 
									{

										pItmTblElem->index_list = NULL;
									}
								}// next element
							}
							else
							if (CodingMajor == DDOD_REV_SUPPORTED_EIGHT)
							{							
								int rc; // for parse integer func
								COMMAND_TBL_8* pFlatItemTbl = &(pflatBlkDir->command_to_var_tbl);

								PTOC_TBL_8_ELEM     *pItmTblElem, *pEndItemTblElem;
								COMMAND_TBL_8_ELEM	*pTOCTblElem, *pEndTOCTblElem;
								COMMAND_INDEX		*pCmdIndex,   *pEndCmdIndex;
								chunk = pbyObjectValue[i] + lOffset;
								size  = lSize;
								unsigned long item = 0;
								UINT64  cnt = 0, temp_int = 0;


								DDL_PARSE_INTEGER(&chunk, &size, &cnt);
								if (cnt == 0) {
									// if cnt is zero, then don't allocate the block, 
									//   since it will not be freed later
									// initialize all varibles from this case, and break out early.
									pFlatItemTbl->count = 0;
									pFlatItemTbl->list = NULL;
									LOGIT(CERR_LOG,"Error reading PTOC table, entry count is zero.\n");
									break;// out of switch
								} 
#ifdef READ_DEBUG
	cout<<"\n          Data load"<<endl;
	cout<<"ptoc entries: 0x"<<hex<<setw(4)<<setfill('0')<<cnt<<dec<<endl;
#endif	
								pFlatItemTbl->count = (int)cnt;
								pFlatItemTbl->list  = new struct PTOC_TBL_8_ELEM[(size_t)cnt];

								if (pFlatItemTbl->list == NULL) 
								{
									delete pBlkDirExt;
									pFlatItemTbl->count = 0;
									return false;// out-of-memory error
								}
								// clear the table
								memset((char *) pFlatItemTbl->list,0, 
												((size_t)cnt) * sizeof(PTOC_TBL_8_ELEM));//ok
								// load the list
								//
								// all are required, load 'em in sequence
								int z = 0;
								for (pItmTblElem     = pFlatItemTbl->list,	
									 pEndItemTblElem = pItmTblElem + cnt;
									 pItmTblElem < pEndItemTblElem;      pItmTblElem++, z++) 
								{
									if ( size <= 0 )
									{
										LOGIT(CERR_LOG,"Error: PTOC decode ended with %d size "
											"having decoded %d of %d entries.\n",size, z,cnt);
										return true;// temporary true
									}
									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->item_id = (unsigned long) temp_int;
#ifdef READ_DEBUG
	cout<<setw(3)<<z<<"   ItemID: 0x"<<hex<<setw(4)<<setfill('0')<<temp_int<<dec<<endl;
#endif	


									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->rd_count = (int) temp_int;
#ifdef READ_DEBUG
	cout<<"    ReadCmds: 0x"<<hex<<setw(4)<<setfill('0')<<temp_int<<dec<<endl;
#endif	

									if (temp_int != 0)
									{// there are read commands							
										pItmTblElem->rd_list = 
												new struct COMMAND_TBL_8_ELEM[(size_t)temp_int];
							
										if(pItmTblElem->rd_list)
										{
											ulong  w = 0;
											UINT64 tmp_int;
											for(pTOCTblElem = pItmTblElem->rd_list,  
											 pEndTOCTblElem = pTOCTblElem + temp_int;
											 pTOCTblElem < pEndTOCTblElem;	  	   pTOCTblElem++,w++)
											{												 
												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->subindex = (unsigned short) tmp_int;
#ifdef READ_DEBUG
	cout<<"          "<<setw(3)<<w<<"   Subindex: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	

												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->number = (unsigned long) tmp_int;
#ifdef READ_DEBUG
	cout<<"               CmdNumber: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	

												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->transaction =(unsigned long) tmp_int;
#ifdef READ_DEBUG
	cout<<"             Transaction: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	

												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->weight = (unsigned short) tmp_int;
#ifdef READ_DEBUG
	cout<<"                  Weight: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	

												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->count = (int) tmp_int;
#ifdef READ_DEBUG
	cout<<"                IndexCnt: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	
									//
									// if there are index elements, parse them
									//
									if (tmp_int != 0) 
									{	// make the list 
										UINT64 tmpint;
										pTOCTblElem->index_list = 
														new struct COMMAND_INDEX[(size_t)tmp_int];

										if (pTOCTblElem->index_list == NULL) 
										{
											pTOCTblElem->count = 0;
											delete pBlkDirExt;
											return false;// out-of-memory error
										}
										//
										// load the list of indexes
										//
										int y = 0;
										for (pCmdIndex    = pTOCTblElem->index_list,  
											 pEndCmdIndex = pCmdIndex + tmp_int;
											 pCmdIndex < pEndCmdIndex;				pCmdIndex++,y++) 
										{
											DDL_PARSE_INTEGER(&chunk, &size, &tmpint);
											pCmdIndex->id= (ITEM_ID) tmpint;

											DDL_PARSE_INTEGER(&chunk, &size, &tmpint);
											pCmdIndex->value = (unsigned long) tmpint;
											
#ifdef READ_DEBUG
	cout<<"                "<<setw(2)<<y<<" Index's ID:  0x"<<hex<<setw(4)<<setfill('0')<<pCmdIndex->id<<dec;
	cout<<"    w/ Value: "<<tmpint<<endl;
#endif	

										}// next index
									}
									else 
									{
										pTOCTblElem->index_list = NULL;
									}

											}
										}
										else
										{
											delete pBlkDirExt;
											pFlatItemTbl->count = 0;
											return false;// out-of-memory error
										}
									}
									else
									{
										pItmTblElem->rd_list = NULL;
									}

									DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
									pItmTblElem->wr_count = (int) temp_int;
#ifdef READ_DEBUG
	cout<<"   WriteCmds: 0x"<<hex<<setw(4)<<setfill('0')<<temp_int<<dec<<endl;
#endif	
									if (temp_int != 0)
									{// there are write commands
										pItmTblElem->wr_list = 
												new struct COMMAND_TBL_8_ELEM[(size_t)temp_int];
										if(pItmTblElem->wr_list)
										{
											ulong  w = 0;
											UINT64 tmp_int;
											for(pTOCTblElem = pItmTblElem->wr_list,  
											 pEndTOCTblElem = pTOCTblElem + temp_int;
											 pTOCTblElem < pEndTOCTblElem;	  	   pTOCTblElem++,w++)
											{												 
												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->subindex = (unsigned short) tmp_int;
#ifdef READ_DEBUG
	cout<<"          "<<setw(3)<<w<<"   Subindex: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	

												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->number = (unsigned long) tmp_int;
#ifdef READ_DEBUG
	cout<<"               CmdNumber: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	

												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->transaction =(unsigned long) tmp_int;
#ifdef READ_DEBUG
	cout<<"             Transaction: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	

												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->weight = (unsigned short) tmp_int;
#ifdef READ_DEBUG
	cout<<"                  Weight: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	

												DDL_PARSE_INTEGER(&chunk, &size, &tmp_int);
												pTOCTblElem->count = (int) tmp_int;
#ifdef READ_DEBUG
	cout<<"                IndexCnt: 0x"<<hex<<setw(4)<<setfill('0')<<tmp_int<<dec<<endl;
#endif	
									//
									// if there are index elements, parse them
									//
									if (tmp_int) 
									{	// make the list 
										UINT64 tmpint;
										pTOCTblElem->index_list = 
														new struct COMMAND_INDEX[(size_t)tmp_int];

										if (pTOCTblElem->index_list == NULL) 
										{
											pTOCTblElem->count = 0;
											delete pBlkDirExt;
											return false;// out-of-memory error
										}
										//
										// load the list of indexes
										//
										int y = 0;
										for (pCmdIndex    = pTOCTblElem->index_list,  
											 pEndCmdIndex = pCmdIndex + tmp_int;
											 pCmdIndex < pEndCmdIndex;				pCmdIndex++,y++) 
										{
											DDL_PARSE_INTEGER(&chunk, &size, &tmpint);
											pCmdIndex->id= (ITEM_ID) tmpint;

											DDL_PARSE_INTEGER(&chunk, &size, &tmpint);
											pCmdIndex->value = (unsigned long) tmpint;
											
#ifdef READ_DEBUG
	cout<<"                "<<setw(2)<<y<<" Index's ID:  0x"<<hex<<setw(4)<<setfill('0')<<pCmdIndex->id<<dec;
	cout<<"    w/ Value: "<<tmpint<<endl;
#endif	

										}// next index
									}
									else 
									{
										pTOCTblElem->index_list = NULL;
									}

											}
										}
										else
										{
											delete pBlkDirExt;
											pFlatItemTbl->count = 0;
											return false;// out-of-memory error
										}
									}
									else
									{
										pItmTblElem->wr_list = NULL;
									}
								}// next item element
							}
							else
							{// binary file format not supported								
								delete pBlkDirExt;
								return false;
							}


						}// else no table
						else
						{
							pflatBlkDir->command_tbl.count = 0;
							pflatBlkDir->command_tbl.list  = NULL;
						}
					}
					break;			
					case CRIT_PARAM_TBL_ID:
					{
						ulTableMaskBit = CRIT_PARAM_TBL_MASK;
						tblExtnOffset  = &(pBlkDirExt->CriticalParameterTable);
						ulReqMask     &= ~ulTableMaskBit;/* clear request mask bit */

						bRet = read_dword(&lOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}
						bRet = read_dword(&lSize,(void*)&tblExtnOffset->uSize,FORMAT_BIG_ENDIAN);
						if(bRet == false)
						{
							delete pBlkDirExt;
							return false;
						}		
#ifdef READ_DEBUG
	cout<<"offset:  0x"<<hex<<lOffset<<dec<<endl;
	cout<<"  size:  0x"<<hex<<lSize<<dec<<endl<<endl;
#endif

						if (lSize)
						{
							int rc; // for parse integer func

							chunk = pbyObjectValue[i] + lOffset;
							size  = lSize;
							unsigned long item = 0;
							UINT64  cnt = 0, temp_int = 0;

							CRIT_PARAM_TBL* pFlatItemTbl = &(pflatBlkDir->crit_param_tbl);
							
							DDL_PARSE_INTEGER(&chunk, &size, &cnt);
							if (cnt == 0) {
								// if cnt is zero, then don't allocate the block, 
								//   since it will not be freed later
								// initialize all varibles from this case, and break out early.
								pFlatItemTbl->count = 0;
								pFlatItemTbl->list = NULL;
								LOGIT(CERR_LOG,"Warning there are no critical prameters in this DD.\n");
								break;
							} 
							pFlatItemTbl->count = (int)cnt;
							pFlatItemTbl->list  = new ITEM_ID[(size_t)cnt];
							int y;	// PAW 03/03/09
							for ( /*int*/ y = 0; y < cnt && size > 0; y++)
							{
								DDL_PARSE_INTEGER(&chunk, &size, &temp_int);
								pFlatItemTbl->list[y] = (ITEM_ID) temp_int;
								//CriticalParamList.push_back(temp_int);
							}
							if ( size != 0 || y < cnt )
							{								
								LOGIT(CERR_LOG,L"\n eval_crit_table failed!!!! size=%d cnt=%d\n", size, y);
							}
						}// no size : no table
					}
					break;	
					default:
						break;// do nothing with it
					}// endswitch - block directory tables
				}// wend - next block directory table

					/* we are only interested in a couple of tables */
			//		WORD wOffset, wTblLength;
			//		DATAPART_SEGMENT_6 *tblExtnOffset;

			//		unsigned long   size;
			//		unsigned char  *chunk;
			//		DDL_UINT    	temp_int, numeric;

					/* critical parameter table */

			//		tblExtnOffset = &(pBlkDirExt->CriticalParameterTable);

			//		bRet = read_word(&wOffset,(void*)&tblExtnOffset->offset,FORMAT_BIG_ENDIAN);
			//		if(bRet == false)
			//		{
			//			delete pBlkDirExt;
			//			return false;
			//		}

			//		bRet = read_word(&wTblLength,(void*)&tblExtnOffset->uSize,FORMAT_BIG_ENDIAN);
			//		if(bRet == false)
			//		{
			//			delete pBlkDirExt;
			//			return false;
			//		}						

			//		if (wTblLength) 
			//		{
			//			int rc; // for parse integer func
			//			chunk = pbyObjectValue[i] + wOffset;
			//			size = wTblLength;
						/* tbl == (encoded_int) count: count instances of (encoded_int)itemID */
			//			CriticalParamList.clear();
						/* eval_cpt */
						
			//			DDL_PARSE_INTEGER(&chunk, &size, &temp_int);/* parse count */

			//			if ( temp_int != 0 )
			//			{
			//				for ( int y = 0; y < temp_int && size > 0; y++)
			//				{
			//					DDL_PARSE_INTEGER(&chunk, &size, &numeric);/* parse count */
			//					CriticalParamList.push_back(numeric);
			//				}
			//				if ( size != 0 || y < temp_int )
			//				{								
			//					cerr<<"\n eval_crit_table failed!!!! size="<< size<<" cnt="<<y<<endl;
			//				}
			//			}
			//		}
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
/* we're looking for the block directory too...
		if(bDevDirLoadedFlag == true)
			break;
***/

	}/*End for*/

//	bDevDirAllocated = true;//Commented by anil
	bDevDir6Allocated = true;//Bug Fix Anil january 4 2006 -- When Std was fms and device was fm6, simulator was failing


	//LOGIT(CLOG_LOG, "End LoadDeviceDirectory");


	return true;
}/*End LoadDeviceDirectory*/

#ifdef _DEBUG 
int i;
#define USE_I  i
#else
#define USE_I int i
#endif

bool DDlDevDescription ::GetItems6()
{
	/*Here we will loop through the SOD Object Extensions*/

	BYTE byItemType;
	BYTE byItemSubType;
	unsigned long ulItemID;
	unsigned long ulItemMask;
	int iRetVal;
	OBJECT_INDEX objectIndex;


	for (USE_I = 0; i < descriptor.sod_length;i++)
	{
		byItemType    = 0;
		byItemSubType = 0;
		ulItemID      = 0L;
		ulItemMask    = 0L;
		iRetVal       = 0;
		objectIndex   = ObjectFixed[i].index;



		if (byExtLengths[i] == 0)
		{
			cout<< i <<" has no data \n";
			continue;
		}

		if (pbyExtensions[i] == NULL)
		{			
			cout<< i <<" has no data \n";
			continue;
		}


		switch (pbyExtensions[i][1])
		{
			case VARIABLE_ITYPE:
				{
				//LOGIT(CLOG_LOG, "Var start");


					DDl6Variable *pVar = new DDl6Variable();
					if ( pVar == NULL)
						return false;

					iRetVal = pVar->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
				    if(iRetVal == FETCH_EXTERNAL_OBJECT)
						/*TODO see if wee have to delete pXwhatever here to return memory*/
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					/* We will set the masks after evaluating the "type" attribute */

					iRetVal = pVar->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Variable , pVar );
						return false;
					}

					/*Push the parsed Item on the list*/
DevItemList::iterator iY;
for (iY = ItemsList.begin(); iY != ItemsList.end(); ++iY)
{
	if ( (*iY)->id == pVar->id )
	{
		LOGIT(CERR_LOG,L"ERROR: Duplicate item ids 0x%04x\n",pVar->id);
	}

}

//LOGIT(CLOG_LOG, "pVar->id = %d", pVar->id);



					ItemsList.push_back(pVar);
					pVar->clear_flat();


					//LOGIT(CLOG_LOG, "Var end");
					break;
				}


			case COMMAND_ITYPE://Command
				{
					DDl6Command *pCmd = new DDl6Command();
					if ( pCmd == NULL)
						return false;

				    iRetVal = pCmd->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					iRetVal = pCmd->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Command , pCmd );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pCmd);

					pCmd->clear_flat();

					break;
				}
			case MENU_ITYPE:// Menu
				{
					DDl6Menu *pMenu = new DDl6Menu();
					if ( pMenu == NULL)
						return false;

				    iRetVal = pMenu->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					iRetVal = pMenu->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Menu , pMenu );
						if ( isInTokizer )
						printf("\n eval_attrs failed for Menu: 0x%04x\t i = %d\t RetCode = %d\n",
																		(unsigned)pMenu->id, i, iRetVal);
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pMenu);

					pMenu->clear_flat();

					break;
				}
			case EDIT_DISP_ITYPE:// Edit Display
				{
					DDl6EditDisplay *pEditDisp = new DDl6EditDisplay();
					if ( pEditDisp == NULL)
						return false;

				    iRetVal = pEditDisp->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pEditDisp->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( EditDisplay , pEditDisp );
						return false;
					}


					/*Push the parsed Item on the list*/

					ItemsList.push_back(pEditDisp);

					pEditDisp->clear_flat();

					break;
				}
			case METHOD_ITYPE://Method
				{
					DDl6Method *pMethod = new DDl6Method();
					if ( pMethod == NULL)
						return false;

				    iRetVal = pMethod->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pMethod->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Method , pMethod );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pMethod);

					pMethod->clear_flat();

					break;
				}
			case REFRESH_ITYPE:// Refresh Relation
				{
					DDl6Refresh* pRefresh = new DDl6Refresh();
					if ( pRefresh == NULL)
						return false;

				    iRetVal = pRefresh->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pRefresh->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Refresh, pRefresh );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pRefresh);

					pRefresh->clear_flat();

					break;
				}
			case UNIT_ITYPE:// Unit Relation
				{
					DDl6Unit* pUnit = new DDl6Unit();
					if ( pUnit == NULL)
						return false;

				    iRetVal = pUnit->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pUnit->eval_attrs();
					if(iRetVal != SUCCESS)
					{						
						EVAL_FAILED( Unit, pUnit );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pUnit);

					pUnit->clear_flat();

					break;
				}
			case WAO_ITYPE:// WAO Relation
				{
					DDl6Wao* pWao = new DDl6Wao();
					if ( pWao == NULL)
						return false;

				    iRetVal = pWao->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pWao->eval_attrs();
					if(iRetVal != SUCCESS)
					{					
						EVAL_FAILED( Wao, pWao );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pWao);

					pWao->clear_flat();

					break;
				}
			case ITEM_ARRAY_ITYPE:// Item Array
				{
					DDl6ItemArray* pItemArray = new DDl6ItemArray();
					if ( pItemArray == NULL)
						return false;
					pItemArray->byItemSubType = ((ITEM_EXTN*)pbyExtensions[i])->bySubType;

				    iRetVal = pItemArray->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pItemArray->eval_attrs();
					if(iRetVal != SUCCESS)
					{					
						EVAL_FAILED( ItemArray, pItemArray );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pItemArray);
					pItemArray->clear_flat();

					break;
				}
			case COLLECTION_ITYPE:// Collection
				{
					DDl6Collection* pCollection = new DDl6Collection();
					if ( pCollection == NULL)
						return false;
					pCollection->byItemSubType = ((ITEM_EXTN*)pbyExtensions[i])->bySubType;

				    iRetVal = pCollection->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pCollection->eval_attrs();
					if(iRetVal != SUCCESS)
					{					
						EVAL_FAILED( Collection, pCollection );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pCollection);

					pCollection->clear_flat();

					break;
				}
			case RECORD_ITYPE://Record
				{
					DDl6Record* pRecord = new DDl6Record();
					if ( pRecord == NULL)
						return false;
					pRecord->byItemSubType = ((ITEM_EXTN*)pbyExtensions[i])->bySubType;

				    iRetVal = pRecord->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pRecord->eval_attrs();
					if(iRetVal != SUCCESS)
					{					
						EVAL_FAILED( Record, pRecord );
						return false;
					}

					/*Vibhor 311003: For implementing the Demunging Solution We will store this
					 as a Collection item*/

					pRecord->byItemType = COLLECTION_ITYPE;

					pRecord->strItemName = "Collection";


					/*Push the parsed Item on the list*/

					ItemsList.push_back(pRecord);

					pRecord->clear_flat();

					break;
				}
			case ARRAY_ITYPE://Array
				{
					DDl6Array* pArray = new DDl6Array();
					if ( pArray == NULL)
						return false;
					pArray->byItemSubType = ((ITEM_EXTN*)pbyExtensions[i])->bySubType;

				    iRetVal = pArray->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pArray->eval_attrs();
					if(iRetVal != SUCCESS)
					{					
						EVAL_FAILED( Array, pArray );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pArray);

					pArray->clear_flat();
				}
				break;

			case VAR_LIST_ITYPE:// Variable List
//#ifdef _PARSER_DEBUG
					LOGIT(CERR_LOG,L"\n############################VARRIABLE LIST ITEM!!!#################################\n");			
//#endif
					break;
			case RESERVED_ITYPE1:
					LOGIT(CERR_LOG,L"\n###################################RESERVED ITEM 0#################################\n");			
					break;
			case RESERVED_ITYPE2:
					LOGIT(CERR_LOG,L"\n###################################RESERVED ITEM 11################################\n");			
					break;
			case PROGRAM_ITYPE:
					LOGIT(CERR_LOG,L"\n####################################PROGRAM ITEM ##################################\n");			
					break;

			case RESP_CODES_ITYPE:// Response Code
//#ifdef _PARSER_DEBUG
					LOGIT(CERR_LOG,L"\n#################################RESP CODE ITEM!!!#################################\n");			
//#endif
					break;

			case BLOCK_ITYPE://Block
				{
					DDl6Block* pBlock = new DDl6Block();
					if ( pBlock == NULL)
						return false;
					pBlock->byItemSubType = ((ITEM_EXTN*)pbyExtensions[i])->bySubType;

				    iRetVal = pBlock->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pBlock->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						LOGIT(CERR_LOG,L"\n eval_attrs failed for BLOCK!!!!");
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pBlock);

					pBlock->clear_flat();
				}
				break;
				
			case DOMAIN_ITYPE  :
					LOGIT(CERR_LOG,L"\n#####################################DOMAIN ITEM 0#################################\n");			
					break;
			case MEMBER_ITYPE  :
					LOGIT(CERR_LOG,L"\n###################################MEMBER ITEM ####################################\n");			
					break;
			case FILE_ITYPE	:
				{
					DDl6File *pFile = new DDl6File();
					if ( pFile == NULL)
						return false;

				    iRetVal = pFile->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pFile->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( File, pFile );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pFile);

					pFile->clear_flat();

				}
				break;
					break;
			case CHART_ITYPE:
				{
					DDl6Chart *pChart = new DDl6Chart();
					if ( pChart == NULL)
						return false;

				    iRetVal = pChart->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pChart->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Chart , pChart );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pChart);

					pChart->clear_flat();

				}
				break;

			case GRAPH_ITYPE:
				{
					DDl6Graph *pGraph = new DDl6Graph();
					if ( pGraph == NULL)
						return false;

				    iRetVal = pGraph->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pGraph->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Graph , pGraph );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pGraph);

					pGraph->clear_flat();

				}
				break;

			case AXIS_ITYPE	:
				{
					DDl6Axis *pAxis = new DDl6Axis();
					if ( pAxis == NULL)
						return false;

				    iRetVal = pAxis->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pAxis->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Axis , pAxis );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pAxis);

					pAxis->clear_flat();

				}
				break;

			case WAVEFORM_ITYPE:
				{
					DDl6Waveform *pWvForm = new DDl6Waveform();
					if ( pWvForm == NULL)
						return false;

				    iRetVal = pWvForm->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					
					iRetVal = pWvForm->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Waveform , pWvForm );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pWvForm);

					pWvForm->clear_flat();
				}
				break;
			case SOURCE_ITYPE:
				{
					DDl6Source *pSrc = new DDl6Source();
					if ( pSrc == NULL)
						return false;

				    iRetVal = pSrc->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					iRetVal = pSrc->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Source , pSrc );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pSrc);

					pSrc->clear_flat();
				}
				break;

			case LIST_ITYPE	:
				{
					DDl6List *pLst = new DDl6List();
					if ( pLst == NULL)
						return false;

				    iRetVal = pLst->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					 
					iRetVal = pLst->eval_attrs();
					if(iRetVal != SUCCESS)
					{						
						EVAL_FAILED( List , pLst );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pLst);

					pLst->clear_flat();
				}
				break;

			case GRID_ITYPE	:
				{
					DDl6Grid *pGrd = new DDl6Grid();
					if ( pGrd == NULL)
						return false;

				    iRetVal = pGrd->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					 
					iRetVal = pGrd->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Grid , pGrd );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pGrd);

					pGrd->clear_flat();
				}
				break;

			case IMAGE_ITYPE	:
				{
					DDl6Image *pImg = new DDl6Image();
					if ( pImg == NULL)
						return false;

				    iRetVal = pImg->fetch_item(pbyExtensions[i],ObjectFixed[i].index);
					if(iRetVal == FETCH_EXTERNAL_OBJECT)
						break; /*Don't quit if an external object was tried as a base object*/
					else
					if(iRetVal != SUCCESS)
						return false;
					// else all is well, continue

					 
					iRetVal = pImg->eval_attrs();
					if(iRetVal != SUCCESS)
					{
						EVAL_FAILED( Image , pImg );
						return false;
					}

					/*Push the parsed Item on the list*/

					ItemsList.push_back(pImg);

					pImg->clear_flat();
				}
				break;

			default:

//					printf("Error : Invalid Object Type  : Type Code = % d\n",pbyExtensions[i][1]);
//#ifdef _PARSER_DEBUG
				if(	(pbyExtensions[i][1] == MEMBER_ITYPE )  ||
					(pbyExtensions[i][1] == VAR_LIST_ITYPE )  ||
					(pbyExtensions[i][1] == RESP_CODES_ITYPE )
				  )
				{
					LOGIT(CERR_LOG,L"Error : Invalid Object Type  : Type Code = %d\n",pbyExtensions[i][1]);
				}
#ifdef _DEBUG
				else
				{// normal to have 3 of these...
					if      (pbyExtensions[i][1] == FORMAT_OBJECT_TYPE)
						//cerr<<"Error : Unused Object Type  : Type Code = Format Object"<<endl;
						LOGIF(LOGP_MISC_CMTS)(CERR_LOG,"Error : Unused Object Type  : Type Code = Format Object(debug only msg)\n");
					else if (pbyExtensions[i][1] == DEVICE_DIR_TYPE)
						//cerr<<"Error : Unused Object Type  : Type Code = Device Directory Object"<<endl;
						LOGIF(LOGP_MISC_CMTS)(CERR_LOG,"Error : Unused Object Type  : Type Code = Device Directory Object(debug only msg)\n");
					else if (pbyExtensions[i][1] == BLOCK_DIR_TYPE)
						//cerr<<"Error : Unused Object Type  : Type Code = Block Directory Object"<<endl;
						LOGIF(LOGP_MISC_CMTS)(CERR_LOG,"Error : Unused Object Type  : Type Code = Block Directory Object(debug only msg)\n");
					else
						//cerr<<"Error : Unknown Object Type  : Type Code = "<<pbyExtensions[i][1]<<endl;
						LOGIT(CERR_LOG,"Error : Unknown Object Type  : Type Code = %d (debug only msg)\n",pbyExtensions[i][1]);
				}
#endif
//#endif
					break;

		}/*End switch (pbyExtensions[i][1])*/
#ifdef DICT_REF_DEBUG
		if (! pGlobalDict->dict_ok())
		{
			cerr<<"Error."<<endl;
		}
#endif

	}/*End for*/


	
return true;
}



bool DDlDevDescription ::GetImages6()
{//AimageList_t ImagesList;
	if (device_dir_6.image_tbl.count <= 0 )
	{
		return true;
	}// else do the work

	if (ImagesList.size() > 0)
	{
		// clear the list
	}

	IMAGEFRAME_S imgFrm;
	BIframeList_t imageL;

	IMAGE_TBL_ELEM* pITE = device_dir_6.image_tbl.list;// [0]
	IMG_ITEM *      pII;

	for ( unsigned x = 0; x < device_dir_6.image_tbl.count; x++, pITE++)
	{
		pII = pITE->img_list;
		if (pITE->num_langs <= 0)
		{
			LOGIT(CERR_LOG,L"ERROR: Image with no languages.\n");
			return false;
		}
		for ( unsigned y = 0; y < pITE->num_langs; y++, pII++)
		{
			memcpy( (void*)&(imgFrm.ifs_language[0]),
				          (void*)&(pII->lang_code     ),CNTRYCDSTRLEN);
			imgFrm.ifs_size      = pII->img_file.uSize;
			imgFrm.ifs_pRawFrame = pII->p2Graphik;
			imgFrm.ifs_offset    = pII->img_file.offset;

			imageL.push_back(imgFrm);
		}// next lang/frame
		if ( imageL.size() > 0 )
		{
			ImagesList.push_back(imageL);
			imageL.clear();
		}
		// stevev 28sep11 - delete the table memory now that we got what we needed
		delete [] pITE->img_list;//pII no longer points here
		pITE->img_list = NULL;
	}// next image
	// stevev 28sep11 - delete the table memory now that we got what we needed
	delete [] device_dir_6.image_tbl.list; device_dir_6.image_tbl.list = NULL;
	device_dir_6.image_tbl.count = 0;
	return true;
}

#ifdef _WIN32_WCE
unsigned int DDlDevDescription::aligndata(unsigned char * char_ptr)
{
	unsigned char char_byte;	
	unsigned int return_int;

	return_int = 0;
	char_byte = *(char_ptr++);
	return_int += char_byte;				// LSB
	char_byte = *(char_ptr++);
	return_int += char_byte << 8;
	char_byte = *(char_ptr++);
	return_int += char_byte << 16;
	char_byte = *(char_ptr++);
	return_int += char_byte<<24;			// MSB
	return (return_int);
}
#endif
