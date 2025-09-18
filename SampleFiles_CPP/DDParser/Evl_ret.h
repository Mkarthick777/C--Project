/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *	@(#)evl_ret.h	40.1  40  07 Jul 1998
 */

#ifndef EVL_RET_H
#define EVL_RET_H

#include "Attributes.h"

#ifdef __cplusplus
	extern "C" {
#endif /* __cplusplus */

/*
*	The following structures define the information returned from DD Services
*/

#define RETURN_LIST_SIZE	32	/** Max number of elements which can be **/
								/** returned in a RETURN_INFO list **/

typedef struct {
	unsigned long   bad_attr;	/* Bit indicating which attribute is bad */
	int             rc;			/* Return code to be passed back */
	OP_REF          var_needed;	/* Used to store op_ref if unable to get */
								/* value from parameter value service. */
}               RETURN_INFO;


typedef struct {
	unsigned short  count;		/** number of items in the RETURN_INFO list **/
	RETURN_INFO     list[RETURN_LIST_SIZE];	 /** list of return information **/
}               RETURN_LIST;

#ifdef __cplusplus
	}
#endif /* __cplusplus */

#endif	/* EVL_RET_H */
