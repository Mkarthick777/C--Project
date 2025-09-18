/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *	@(#)panic.h	40.1  40  07 Jul 1998
 */
#ifndef _PANIC_H
#define _PANIC_H

#include "std.h"

#ifdef linux
#define panic(...) __panic(-1, __VA_ARGS__ )
extern void __panic P((int dummy,...));
#else
	/*lint +fva variable number of arguments */
	#if !defined(NO_P4_DOS)
	#if defined(MSDOS) || defined(CODECENTER) || defined(_MSC_VER) || defined(SVR4)

	extern void panic P((char *,...));

	#else
	extern void panic P((const char *,...));

	#endif /* defined(MSDOS) || defined(CODECENTER) || defined(_MSC_VER) || defined(SVR4) */
	#endif /* !defined(NO_P4_DOS) */
	/*lint -fva end of varying arguments */
#endif

#endif /* _PANIC_H */
