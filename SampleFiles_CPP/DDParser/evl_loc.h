/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *	@(#)evl_loc.h	40.2  40  07 Jul 1998
 */

#include "evl_lib.h"

#ifndef EVL_LOC_H
#define EVL_LOC_H


/**
 ** Standard dictionary default string indexes.
 ** If an upcall (for a string) returns "!DDL_SUCESS" or "DDL_DEFAULT_ATTR"
 ** one of these default strings is to be used.
 **/

#define DEFAULT_STD_DICT_STRING         (unsigned long)((400 << 16) + 0)
#define DEFAULT_DEV_SPEC_STRING          (unsigned long)((400 << 16) + 1)
#define DEFAULT_STD_DICT_HELP           (unsigned long)((400 << 16) + 2)
#define DEFAULT_STD_DICT_LABEL          (unsigned long)((400 << 16) + 3)
#define DEFAULT_STD_DICT_DESC           (unsigned long)((400 << 16) + 4)
#define DEFAULT_STD_DICT_DISP_INT       (unsigned long)((400 << 16) + 5)
#define DEFAULT_STD_DICT_DISP_UINT      (unsigned long)((400 << 16) + 6)
#define DEFAULT_STD_DICT_DISP_FLOAT     (unsigned long)((400 << 16) + 7)
#define DEFAULT_STD_DICT_DISP_DOUBLE    (unsigned long)((400 << 16) + 8)
#define DEFAULT_STD_DICT_EDIT_INT       (unsigned long)((400 << 16) + 9)
#define DEFAULT_STD_DICT_EDIT_UINT      (unsigned long)((400 << 16) + 10)
#define DEFAULT_STD_DICT_EDIT_FLOAT     (unsigned long)((400 << 16) + 11)
#define DEFAULT_STD_DICT_EDIT_DOUBLE    (unsigned long)((400 << 16) + 12)

/*
 *	Macro to silence complaints from SABER and CODECENTER
 */

#define NULL_PTR	NULL

/*
 *	Parse tag macro.
 *  This macro can handle parsing simple tags.
 * 	If the Most Significant Bit of CHUNK is set, an explicit length
 *  is stored with the tag ( call ddl_parse_tag_func() ).
 *  if the tag occupies multiple bytes, (ie. **chunk == 127),
 *  ddl_parse_tag_func() must be called.
 */
#ifdef XMTR
#define DDL_PARSE_TAG(C,S,T,L)	\
	{	\
		if ( (**(C) & 0x80) || ( (**(C) & 0x7f) == 127 )){	\
			rc = ddl_parse_tag_func((C), (S), (T), (L));	\
			if (rc != DDL_SUCCESS)	\
				return true;	\
		} else {	\
			(L) ? *(L) = 0 : 0;	\
			(T) ? *(T) = **(C) : 0;	\
			 ++(*(C));	\
			--(*(S));	\
		}	\
	}
#else // tokenizer...
#define DDL_PARSE_TAG(C,S,T,L)	\
	{	\
		if ( (**(C) & 0x80) || ( (**(C) & 0x7f) == 127 )){	\
			rc = ddl_parse_tag_func((C), (S), (T), (L));	\
			if (rc != DDL_SUCCESS)	\
				return rc;	\
		} else {	\
			(L) ? *(L) = 0 : 0;	\
			(T) ? *(T) = **(C) : 0;	\
			 ++(*(C));	\
			--(*(S));	\
		}	\
	}

#endif


typedef int (*EVALDIR_FN) (void*,BININFO*);  
/*
 * Evaluation functions
 */





#endif /* EVL_LOC_H */
