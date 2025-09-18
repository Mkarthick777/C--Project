/*
 *  Copyright 1998 - HART Communication Foundation
 *  All rights reserved.
 */

/*
 *	@(#)dict.h	40.1	 40  07 Jul 1998
 */


#ifndef DICT_H
#define DICT_H

/*
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Name of program for dict.c.
 */

#define		MAX_DICT_TABLE_SIZE		5000

typedef struct {
	unsigned long	 ref;
	unsigned short	 len;
	char			*str;
} DICT_TABLE_ENTRY;


extern char	*dict_program;
//int	makedict P((char *, char *, void (*func)(ulong , char *,char *)));
/*Vibhor 141003: changing the definition of the second parameter*/

int makedict(char *, char **, void (*func)(unsigned long , char *,char *));

/*
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DICT_H */
