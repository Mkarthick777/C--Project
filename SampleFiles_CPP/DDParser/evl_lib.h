/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *
 *	@(#)evl_lib.h	40.1  40  07 Jul 1998
 */

#ifndef EVL_LIB_H
#define EVL_LIB_H

#include <stddef.h>
#include <limits.h>

#include "Retn_Code.h"		/** All of these .h files are needed by the eval library **/
// moved to common as tags_sa.h    #include "DDLTags.h"
#include "Tags_sa.h"		 
#include "DDLDefs.h"		/** user only needs to include evl_lib.h **/
#include "Flats.h"
#include "Table.h"


/*
 *	Structure used to pass info to the device specific string service
 */
struct DEV_STRING_INFO{
	unsigned long   id;
	unsigned long	mfg;
	unsigned short  dev_type;
	unsigned char   rev;
	unsigned char   ddrev;
};



extern int eval_dir_device_tables P(( FLAT_DEVICE_DIR *, BIN_DEVICE_DIR *, unsigned long));

extern int app_func_get_dev_spec_string P((DEV_STRING_INFO *, ddpSTRING *));

/*Vibhor 020904: Start of Code*/
//Adding new prototype for HART 6 Device Directory

extern int eval_dir_device_tables_6 P(( FLAT_DEVICE_DIR_6 *, BIN_DEVICE_DIR_6 *, unsigned long));

/*Vibhor 020904: End of Code*/

// timj added 9oct07
extern int eval_dir_device_tables_8 P(( FLAT_DEVICE_DIR_6 *, BIN_DEVICE_DIR_6 *, unsigned long));




#endif /* EVL_LIB_H */
