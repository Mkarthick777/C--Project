
/**********************************************************************************************

							COPYRIGHT (c) 2000
							  HONEYWELL INC.,
						    ALL RIGHTS RESERVED

		This software is a copyrighted work and/or information protected 
		as a trade secret. Legal rights of Honeywell Inc. in this software
		is distinct from ownership of any medium in which the software is 
		embodied. Copyright or trade secret notices included must be
		reproduced in any copies authorized by Honeywell Inc.

		The information in this software is subject to change without notice 
		and should not be considered as a commitment by Honeywell Inc.


------------------------------------------------------------------------------------

	File Name				:	typedefs.h

	Project Title			:	PMS

	Version					:	

	Development Platform	:	Windows NT

	Development	Tool		:	Visual C++ Version 6.0

	Author(s)				:	Jayaprakash Meruva

	Purpose					:	

	Date of Creation		:	06/11/2000

	-------------------------------------------------------------------------------------

	Modification History	::

	Author(s)				:

	Date of Modification	:

	Description of the modification: 		:	

********************************************************************************************/


#ifndef __TYPEDEFS
#define __TYPEDEFS

#if defined  _WIN32 || defined ANDROID


//#include <stdio.h>

typedef		char				_CHAR	;			// -128 to 127			(8 bits)
typedef 	unsigned char		_UCHAR	;			//	0 to 255		    (8 bits)
typedef 	short				_SHORT	;			// -32,768 to 32,767	(16 bits)
typedef 	unsigned short 		_USHORT	;			// 0 to 65,535			(16 bits)

typedef 	_UCHAR				_UINT8	;			// 0 to 255 (8 bits)
typedef 	_USHORT				_UINT16	;			// 0 to 65,535 (16 bits)

#ifndef linux
typedef 	unsigned  long int	_UINT32	;			// 0 to 4,294,967,295 (32 bits)
typedef		unsigned __int64	_UINT64 ;
typedef		__int64				_INT64  ;
typedef 	long int			_INT32	;			// -2,147,483,648 to 2,147,483,647 (32 bits)
#else

//jad arm64
typedef		unsigned long long			_UINT64;
typedef		long long					_INT64;


typedef 	__UINT32_TYPE__     _UINT32	;			// 0 to 4,294,967,295 (32 bits)
//typedef		__UINT64_TYPE__ 	_UINT64 ;
//typedef		__INT64_TYPE__  	_INT64  ;
typedef 	__INT32_TYPE__		_INT32	;			// -2,147,483,648 to 2,147,483,647 (32 bits)
#endif

typedef 	_UCHAR				_BYTE	;			// 8 bits 
typedef 	_USHORT				_WORD	;			// 16 bits
typedef 	_UINT32				_DWORD	;			// 32 bits 

typedef 	_CHAR				_INT8	;			// -128 to 127 (8 bits)
typedef 	_SHORT				_INT16	;			// -32,768 to 32,767 (16 bits)

typedef 	float				_FLOAT	;			// 3.4E +/- 38 (7 digits)		(32 bits)
typedef 	double				_DOUBLE	;			// 1.7E +/- 308 (15 digits)		(64 bits)
													// WS - 9apr07 - VS2005 checkin 
typedef 	long double			_L_DOUBLE;			// 1.2E +/- 4932 (19 digits)	(80 bits)
typedef 	bool				_BOOL	;			// boolean 
typedef		void *				_LPVOID;


typedef struct byteString_s
{
	_UCHAR* bs;
	_UINT32  bsLen;
	//struct byteString_s() {bs = 0;bsLen = 0;};
	//struct byteString_s(_UINT32 Len) {if (Len){bs = new _UCHAR[Len];bsLen=Len;}\
	//	else{bs = 0;bsLen = 0;}    };
	//~byteString_s() {if ( bs ) { delete[] bs;bsLen = 0;} };
} _BYTE_STRING;

#define BYTE_STRING( name, size )  _BYTE_STRING name;name.bs = new _UCHAR[ size ];name.bsLen= size
#define BYTE_STR( name )           _BYTE_STRING name;name.bs = (_UCHAR*)0;name.bsLen= 0
#define DEL_BYTE_STR( nm )         { if (nm.bs){ delete [] nm.bs; nm.bs = 0;} nm.bsLen = 0;} 

//#define		TRUE 1				
//#define		FALSE 0
#define		_HANDLE	HANDLE
#ifndef MAX_DD_STRING
  #define MAX_DD_STRING	1024	/*stevev 20may07 - not everywhere yet */
#endif
/// define the equivalent types for the other flat forms

// #elif defined  _LINUX


// #elif defined  _SUNOS
#else

#pragma message "Nothing is Typedef'ed in typedefs.h"

#endif 

#endif // __TYPEDEFS