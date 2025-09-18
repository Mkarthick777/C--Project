/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *	@(#)std.h	40.1  40  07 Jul 1998
 */

#ifndef STD_H
#define STD_H

#include "ddbGeneral.h"
/*
 *	Allow "prototyping" for reference, but use K&R syntax for code
 */
/*
#ifdef __cplusplus
	extern "C" {
#endif */

#if (defined(CODECENTER) || defined(_MSC_VER) || defined(MSDOS) || defined(SVR4) || defined(linux))
#define USE_PROTOS
#endif

#ifdef USE_PROTOS
#define P(x)	x
#else
#define P(x)	()		/* Make argument prototypes disappear */
#endif

/*
 * Define platform specific parameters
 */

#if (defined(SUN) || defined(SVR4))
#define	BIG_ENDIAN	1	/* Uses BIG_ENDIAN byte order */
#undef LITTLE_ENDIAN
#endif				/* SUN or SVR4*/


/*
 *	For use with qsort() and bsearch()
 */
/*
typedef int (*CMP_FN_PTR) P((const void *, const void *));
*/

#if (defined(MSDOS) || defined(WIN32) || defined(_MSC_VER) || defined(__ANDROID__))
#define	LITTLE_ENDIAN	1	/* Uses LITTLE_ENDIAN byte order */
#undef BIG_ENDIAN

//jad vs2017 ios	pw - this seems to work, we'll leave it
#define LITTLE_ENDIAN_IOS


#endif /* MSDOS or WIN32 or _MSC_VER */

typedef enum { FMS_FALSE, FMS_TRUE } fms_boolean;


/*
 *  All purpose fatal_msg() and warn_msg() messages.
 */

#ifdef INTERP
void fatal_msg(char* fmt, ...);
void warn_msg(char* fmt, ...);

/*
 *  One global to tell us if we are the fileserver.
 *  If non-zero, it contains the database file name.
 */

extern char* fileserver;


/*
 *	Assert macros
 */

#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

#undef VERIFY
#undef assert

#define massert(x) assert(x);

#ifndef strcmpi
#  define strcmpi(a,b) strcasecmp(a,b)
#endif

#endif /* INTERP */

#define REQUIRE(cond) \
		/*lint -save -e506*/ \
		extern int __require[(cond) ? 1 : -1] \
		/*lint -restore*/

#define ASSERT_ALL(cond) \
		if(!(cond)) panic("%s(%d) - Condition not true\n", __FILE__, __LINE__)

#define CRASH_ALL() \
		panic("%s(%d) - Crash\n", __FILE__, __LINE__) /* NOTREACHED */

#define insist(x) { if (!(x)) (*(int*) 1) = 0; }

#define ASSERT_RET(cond,param) \
		if(!(cond)) panic("%s(%d) - Condition not true: passed value = %d\n", \
			__FILE__, __LINE__, (param))

#define CRASH_RET(param) \
		panic("%s(%d) - Crash: passed value = %d\n", __FILE__, __LINE__, (param))

#define ASSERT_DBG(cond)	ASSERT_ALL(cond)

#define CRASH_DBG() CRASH_ALL()


#ifdef DEBUG

#if (defined WIN32 || defined __WIN32__)
__declspec(dllimport) void __stdcall DebugBreak( void);
#endif /* WIN32 or __WIN32__ */

#ifdef INTERP

#  define VERIFY(x) (x)->verify()
#  define HAS_VERIFY void verify(void)
#  define VIRTUAL_VERIFY virtual void verify(void)
#  define VIRTUAL_VERIFY_NULL virtual void verify(void) = 0
#  define assert(x) { if (!(x)) _asm { int 3 } }
#  define assume(x) x
/*
 * void tracef(char*, ...);
 */

#endif /* INTERP */

#else

#ifdef INTERP

#  define VERIFY(x)
#  define HAS_VERIFY
#  define VIRTUAL_VERIFY
#  define VIRTUAL_VERIFY_NULL
#  define assert(x)
#  define assume(x)
/*
 *	Apparently unused function which breaks the compile
 * __inline void tracef(char* fmt, ...) {}
 */

#endif /* INTERP */

#endif		/* DEBUG */


/*
 *	Standard defines
 */

//#define FALSE 0
//#define TRUE 1

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

/*
 *	For DDSTEST on the PC, if standard out exists (and it should), give
 *	standard error the same definition, so that messages and errors go
 *	to the same file in the proper (i.e., chronological) order.
 */

#if ((defined(_WINDOWS) || defined(_MSC_VER)) && defined(DDSTEST))
#ifdef stdout
#ifdef stderr
#undef stderr
#endif /* stderr */
#define stderr stdout
#endif /* stdout */
#endif /* (defined(_WINDOWS) || defined(_MSC_VER)) && defined(DDSTEST) */

/*
 *	Standard typedefs
 */

typedef unsigned char	uchar;
typedef unsigned short	ushort;
// use the one in ddbGeneral.h...typedef unsigned long	ulong;

/////////////////////////////////////  HOMZ 
/////////////////////////////////////  error C2371: redefinition
///typedef char INT8;
//////////////////////////////////////////////////////////////////
//typedef char           INT8;PAW error C2371: redefinition 03/03/09
typedef unsigned char UINT8;
typedef short           INT16;
typedef unsigned short UINT16;

#ifndef _BASETSD_H_
  #define _BASETSD_H_
  typedef long INT32;
  typedef unsigned long UINT32;
  #ifdef linux
    typedef long long           INT64;
    typedef unsigned long long UINT64;
  #else
    typedef __int64           INT64;
    typedef unsigned __int64 UINT64;
  #endif
#endif /*_BASETSD_H_*/

typedef ulong DDITEM;

struct BLOCK;

/*
#ifdef __cplusplus
	}
#endif /* __cplusplus */

#endif				/* STD_H */
