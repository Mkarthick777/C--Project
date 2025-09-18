#ifndef lint
static char SCCSID[] = "@(#)app_pnic.c	40.2  40  07 Jul 1998";
#endif /* lint */
/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */
#include "pch.h"
#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(SVR4) || defined(linux)
#include	<stdarg.h>
#else
#include    <varargs.h>
#endif

#include	<stdlib.h>
#include	<stdio.h>
#include    "panic.h"

#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(SVR4)
/***********************************************************************
 *
 * Name: panic
 *
 * ShortDesc: Printf on error and quit.
 *
 * Descripton:
 *		This routine displays the error message and quits the program.
 *
 * Inputs:
 *		va_alist -- variable argument list.
 *
 * Returns:
 *		void.
 *
 **********************************************************************/
/* VARARGS */
void
panic(char *format, ...)
{
	va_list         ap;

	//ADDED By Deepak 
	if(NULL==format)
		return;

	va_start(ap, format);

	(void) vprintf(format, ap);

	va_end(ap);

	exit(1);
}

#else
/***********************************************************************
 *
 * Name: panic
 *
 * ShortDesc: Printf on error and quit.
 *
 * Descripton:
 *		This routine displays the error message and quits the program.
 *
 * Inputs:
 *		va_alist -- variable argument list.
 *
 * Returns:
 *		void.
 *
 **********************************************************************/
/* VARARGS */
#ifndef linux
void
panic(va_alist)
va_dcl
{
	va_list         ap;
	
	//ADDED By Deepak
	char           *format=NULL;

	va_start(ap);

	format = va_arg(ap, char *);

	(void) vprintf(format, ap);

	va_end(ap);

	exit(1);
}
#else // is linux
// #define panic(...) __panic(-1, __VA_ARGS__ )

void
__panic(int dummy, ...)
{
	va_list         ap;

	char           *format=NULL;

	va_start(ap, dummy);

	format = va_arg(ap, char *);

	(void) vprintf(format, ap);

	va_end(ap);

	exit(1);
}
#endif


#endif
