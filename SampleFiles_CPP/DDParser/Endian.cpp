/*C*/
/***************************************************************************/
/*                                                                         */
/*                  HONEYWELL CONFIDENTIAL & PROPRIETARY                   */
/*                                                                         */
/*    THIS  WORK   CONTAINS  VALUABLE   CONFIDENTIAL  AND  PROPRIETARY     */
/*    INFORMATION:   DISCLOSURE;   USE  OR  REPRODUCTION   OUTSIDE  OF     */
/*    HONEYWELL INTERNATIONAL INC IS PROHIBITED  EXCEPT AS AUTHORIZED      */
/*    IN  WRITING.  THIS UNPUBLISHED WORK IS PROTECTED  BY THE LAWS OF     */
/*    THE  UNITED  STATES  AND  OTHER  COUNTRIES.   IN  THE  EVENT  OF     */
/*    PUBLICATION   THE  FOLLOWING  NOTICE  SHALL  APPLY:                  */
/*    "COPYRIGHT 2002 HONEYWELL INTERNATIONAL ALL RIGHTS RESERVED."        */
/*                                                                         */
/***************************************************************************/
/*M*/
/***************************************************************************
**
** APPLICATION: HARTSoftwareMux 
**      MODULE: Endian.cpp
**
** PURPOSE:
**   This module provides functions to read and write values in different
**   multi-byte data representation (big endian, little endian).
**
** REVISION HISTORY:
**
** REVISION HISTORY:
**  Rev  Date      Programmer    Comment
**  ---- --------- ------------- ------------------------------------------
**  0.0 16-Jul-02  TSR Prasad  initial release
**
** Date...: 16-Jul-2002  
** Author.: TSR Prasad
** Comment: made from file 
**
****************************************************************************/

/**************************************************************************/
#include "pch.h"
/* --- Standard Includes ------------------------------------------------- */
//sjv06feb06 #include <windows.h>
#include "ddbGeneral.h"

/* --- Module Includes ------------------------------------------------- */
#include "ParserEndian.h"


/* ----------------------------------------------------------------------- */
/*F*/
/***************************************************************************
** FUNCTION NAME: write_word()
**
** PURPOSE:
**   This function stores a value of type WORD in "big endian" or
**   or "little endian" data representaion.
**
** PARAMETER:
**
**   INPUT ...
**     source
**     format
**
**   OUTPUT ...
**     dest
**
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   true if format is valid
**   false if format unknown
**
****************************************************************************/
bool write_word(void *dest, WORD *source, int format)
{
	//ADDED By Deepak Initializing the variables
	int i=0;
	BYTE *dest_ptr = (BYTE *) dest;
	WORD tmp=0;

	//ADDED By Deepak 'if'
	if(source)
	tmp = *source;

	switch (format)
	{
		case FORMAT_BIG_ENDIAN:
			dest_ptr += sizeof(tmp);
			for (i = 0; i < sizeof(tmp); i++)
			  *(--dest_ptr) = (BYTE)(tmp >> (i * 8));
			break;

		case FORMAT_LITTLE_ENDIAN:
			for (i = 0; i < sizeof(tmp); i++)
				*dest_ptr++ = (BYTE)(tmp >> (i * 8));
		break;

		default:
			return false;
	}

	return true;
}/* --- end of function write_word() --- */


/*F*/
/***************************************************************************
** FUNCTION NAME: write_dword()
**
** PURPOSE:
**   This function stores a value of type DWORD in "big endian" or
**   or "little endian" data representaion.
**
** PARAMETER:
**
**   INPUT ...
**     source
**     format
**
**   OUTPUT ...
**     dest
**
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   true if format is valid
**   false if format unknown
**
****************************************************************************/
bool write_dword(void *dest, DWORD *source, int format)
{
	//ADDED By Deepak Initializing the variables
	int i=0;
	BYTE *dest_ptr = (BYTE *)dest;
	DWORD tmp=0;

	//ADDED By Deepak 'if'
	if(source)
	tmp = *source;

	switch (format)
	{
		case FORMAT_BIG_ENDIAN:
			dest_ptr += sizeof(tmp);
			for (i = 0; i < sizeof(tmp); i++)
				*(--dest_ptr) = (BYTE)(tmp >> (i * 8));
			break;

	case FORMAT_LITTLE_ENDIAN:
			for (i = 0; i < sizeof(tmp); i++)
				*dest_ptr++ = (BYTE)(tmp >> (i * 8));
			break;

	default:
			return false;
	}

	return true;
}/* --- end of function write_dword() --- */

/*F*/
/***************************************************************************
** FUNCTION NAME: write_float()
**
** PURPOSE:
**   This function stores a value of type FLOAT in "big endian" or
**   or "little endian" data representaion.
**
** PARAMETER:
**
**   INPUT ...
**     source
**     format
**
**   OUTPUT ...
**     dest
**
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   true if format is valid
**   false if format unknown
**
****************************************************************************/
bool write_float(void *dest, FLOAT *source, int format)
{
	return(write_dword(dest, (DWORD*)source, format));
}/* --- end of function write_float() --- */

/*F*/
/***************************************************************************
** FUNCTION NAME: read_word()
**
** PURPOSE:
**   This function reads a value of type WORD in "big endian" or
**   or "little endian" data representaion.
**
** PARAMETER:
**
**   INPUT ...
**     source
**     format
**
**   OUTPUT ...
**     dest
**
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   true if format is valid
**   false if format unknown
**
****************************************************************************/
bool read_word(WORD *dest, void *source, int format)
{
	//ADDED By Deepak initializing the variables
	int i=0;
	BYTE *source_ptr = (BYTE *) source;
	WORD tmp=0;

	tmp = 0;

	switch (format)
	{
		case FORMAT_BIG_ENDIAN:
			for (i = 0; i < sizeof(tmp); i++)
			{
				tmp <<= 8;
				tmp |= *source_ptr++;
			}
		break;

		case FORMAT_LITTLE_ENDIAN:
			source_ptr += sizeof(tmp);
			for (i = 0; i < sizeof(tmp); i++)
			{
				tmp <<= 8;
				tmp |= *(--source_ptr);
			}
		break;

		default:
			return false;
	}

	*dest = tmp;

	return true;
}/* --- end of function read_word() --- */

/*F*/
/***************************************************************************
** FUNCTION NAME: read_dword()
**
** PURPOSE:
**   This function reads a value of type DWORD in "big endian" or
**   or "little endian" data representaion.
**
** PARAMETER:
**
**   INPUT ...
**     source
**     format
**
**   OUTPUT ...
**     dest
**
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   true if format is valid
**   false if format unknown
**
****************************************************************************/
bool read_dword(DWORD *dest, void *source, int format)
{
	//ADDED By Deepak initializing the variables
	int i=0;
	BYTE *source_ptr =  (BYTE *)source;
	DWORD tmp;

	tmp = 0;

	switch (format)
	{
		case FORMAT_BIG_ENDIAN:

			//LOGIT(CERR_LOG, "FORMAT_BIG_ENDIAN");

			//LOGIT(CERR_LOG, "sizeof(tmp) = %d", sizeof(tmp));
			
//jad ios debug
//still needed!!
//			for (i = 0; i < sizeof(tmp); i++)
			for (i = 0; i < 4; i++)
			{
				tmp <<= 8;
				tmp |= *source_ptr++;

				//LOGIT(CERR_LOG, "tmp = %d", tmp);

			}
		break;

		case FORMAT_LITTLE_ENDIAN:
			source_ptr += sizeof(tmp);
			for (i = 0; i < sizeof(tmp); i++)
			{
				tmp <<= 8;
				tmp |= *(--source_ptr);
			}
		break;

		default:
			return false;
	}

	*dest = tmp;

	return true;
}/* --- end of function read_dword() --- */

/*F*/
/***************************************************************************
** FUNCTION NAME: read_dword_spl()
**
** PURPOSE:
**   This function reads a value of type DWORD in "big endian" or
**   or "little endian" data representaion from a chunk whose size is passed,
**	 as an argument. This routine helps in parsing a MultiByte integer value!!!
**
** PARAMETER:
**
**   INPUT ...
**     source
**     format
**	   size
**
**   OUTPUT ...
**     dest
**
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   true if format is valid
**   false if format unknown
**
****************************************************************************/



bool read_dword_spl(DWORD *dest, void *source, int size, int format)
{
	//ADDED By Deepak initializing the variables
	int i=0;
	BYTE *source_ptr =  (BYTE *)source;
	DWORD tmp;

	tmp = 0;

	switch (format)
	{
		case FORMAT_BIG_ENDIAN:
			for (i = 0; i < size; i++)
			{
				tmp <<= 8;
				tmp |= *source_ptr++;
			}
		break;

		case FORMAT_LITTLE_ENDIAN:
			source_ptr += size;
			for (i = 0; i < size; i++)
			{
				tmp <<= 8;
				tmp |= *(--source_ptr);
			}
		break;

		default:
			return false;
	}

	*dest = tmp;

	return true;
}/* --- end of function read_dword() --- */

/*F*/
/***************************************************************************
** FUNCTION NAME: read_float()
**
** PURPOSE:
**   This function reads a value of type FLOAT in "big endian" or
**   or "little endian" data representaion.
**
** PARAMETER:
**
**   INPUT ...
**     source
**     format
**
**   OUTPUT ...
**     dest
**
**   MODIFY ...
**     ---
**
** RETURN VALUE:
**   true if format is valid
**   false if format unknown
**
****************************************************************************/
bool read_float(FLOAT *dest, void *source, int format)
{
	return(read_dword((DWORD*)dest, source, format));
}/* --- end of function read_float() --- */

/* ------------------------ End Of File --------------------------------- */

