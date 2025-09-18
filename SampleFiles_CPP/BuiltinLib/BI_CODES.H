/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *
 *		@(#)bi_codes.h	40.2  40  07 Jul 1998
 *
 *		This file defines the return codes used by built-in functions.
 *
 */				 

#ifndef	BI_CODES_FILES
#define BI_CODES_FILES

#define BI_SUCCESS		 0				/* task succeeded in intended task	  */
#define BI_ERROR		-1				/* error occured in task			  */
#define BI_ABORT		-2				/* user aborted task				  */
#define BI_NO_DEVICE	-3				/* no device found on comm request	  */
#define BI_COMM_ERR		-4				/* communications error				  */
#define BI_CONTINUE		-5				/* continue */
#define BI_RETRY		-6				/* retry */
#define BI_PORT_IN_USE  -7				/* block transfer port */

#define N_BI_CODES		 8

#define	ALLBITS				0xFF			/* sets all bits in the byte */

#define STATUS_SIZE				3		/* size of status array				  */
#define STATUS_RESPONSE_CODE	0	 							   
#define STATUS_COMM_STATUS		1
#define STATUS_DEVICE_STATUS	2

#define	RESP_MASK_LEN			16		/* size of response code masks		*/
#define	DATA_MASK_LEN			25		/* size of data masks				*/
#define	MAX_XMTR_STATUS_LEN		DATA_MASK_LEN

#define RESPONSE_BUFFER_LENGTH	40		/* size of buffer string to place resp*/
#define	BI_DISP_STR_LEN			126		/* size of # lines X # char/line in a response code display */

/*device status codes used in defaults*/
#define	DEVICE_MALFUNCTION		0x80
#define	CMD_NOT_IMPLIMENTED		0x40
#define	ACCESS_RESTRICTED		0x10

/*
 * Send Command Error handling
 */

#define __IGNORE__	0
#define __ABORT__	1
#define __RETRY__	2

#endif
