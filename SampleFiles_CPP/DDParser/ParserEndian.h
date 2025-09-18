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
/*I*/
/***************************************************************************
**
** APPLICATION: HARTSoftwareMux 
** HEADER NAME: Endian.h
**
** PURPOSE:
**   This file contains definitions of functions to read and write values in 
**   different multi-byte data representation (big endian, little endian).
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
#ifndef PARSERENDIAN_H
#define PARSERENDIAN_H

#define FORMAT_BIG_ENDIAN		1
#define FORMAT_LITTLE_ENDIAN	2

bool write_word(void *dest, WORD *source, int format);
bool write_dword(void *dest, DWORD *source, int format);
bool write_float(void *dest, FLOAT *source, int format);
bool read_word(WORD *dest, void *source, int format);
bool read_dword(DWORD *dest, void *source, int format);
bool read_dword_spl(DWORD *dest, void *source, int size, int format);
bool read_float(FLOAT *dest, void *source, int format);

#endif /* ENDIAN_H */
