#if !defined(AFX_SAFEVAR_H__D92CC21C_0888_4992_9C4F_0FCF39F2C5AC__INCLUDED_)
#define AFX_SAFEVAR_H__D92CC21C_0888_4992_9C4F_0FCF39F2C5AC__INCLUDED_

//#include "stdafx.h"
#include "typedefs.h"
#include "char.h"

#include <vector>
#include <string>
using namespace std;

class INTER_SAFEARRAY;


enum VARIANT_TYPE
{
	  RUL_NULL=0,
	  /************numeric types********/
	  RUL_BOOL,
	  RUL_CHAR, 
	  RUL_UNSIGNED_CHAR,
	  RUL_INT,
	  RUL_UINT,
	  RUL_SHORT,
	  RUL_USHORT, /* 16 bit char */
	  RUL_LONGLONG,
	  RUL_ULONGLONG,
	  RUL_FLOAT,
	  RUL_DOUBLE,
	  /**************** string types ****/
	  RUL_CHARPTR,
	  RUL_WIDECHARPTR,/* aka _USHORTPTR */
	  RUL_BYTE_STRING,
	  RUL_DD_STRING,
	  RUL_SAFEARRAY
};
// these must match the above list order
#define SIZE_OFS  { 0, sizeof(bool), sizeof(_CHAR),sizeof(_UCHAR),sizeof(_INT32),\
	sizeof(_UINT32), sizeof(_INT16),sizeof(_UINT16),sizeof(_INT64),sizeof(_UINT64),\
	sizeof(float),sizeof(double),sizeof(_CHAR*),sizeof(wchar_t*),\
	sizeof(_BYTE_STRING),sizeof(wchar_t*),sizeof(INTER_SAFEARRAY *) }

/* helper code:::
   switch ()
   {
   case	RUL_BOOL:
   case	RUL_CHAR:
   case	RUL_UNSIGNED_CHAR:
   case	RUL_INT:
   case	RUL_UINT:
   case	RUL_SHORT:
   case	RUL_USHORT:
   case	RUL_LONGLONG:
   case	RUL_ULONGLONG:
   case	RUL_FLOAT:
   case	RUL_DOUBLE:
   default:	
   }// end switch
****************************/			 
// NOTE: the 'B' is a coded flag for how to handle the data type
//   RUL_SAFEARRAY is being handled like a char array!!!!!!!!!!!
#define VARIANT_TYPE_STRINGS  {"<NULL>"},{"B"},{"%s"},{"%s"},{"%d"},\
						{"%u"},{"%d"},{"%u"},{"%I64d"},{"%I64u"},\
						{"%f"},{"%lf"},{"%s"},{"%ws"},{"%s"},{"%s"},{"<out-of-bounds>"}

//#define VARIANT_TYPE_STRINGS  "<NULL>","%d","%u","B",\
//						"%f","%lf","%s","I",%s","%u","<out-of-bounds>"
#define VAR_TYPE_STRING_CNT   17
#define VAR_TYPE_STRING_MXLEN 16

extern char vtype_strings[VAR_TYPE_STRING_CNT][VAR_TYPE_STRING_MXLEN] ;

struct  INTER_SAFEARRAYBOUND
{
	_UINT32 cElements;
};

typedef vector<INTER_SAFEARRAYBOUND> VECBOUNDS;

union __VAL
{
	bool			bValue;
	_CHAR			cValue;
	_UCHAR			ucValue;
	_INT16			sValue;
	_UINT16			usValue;
	_INT32			nValue;
	_UINT32			unValue;
	_INT64			lValue;
	_UINT64			ulValue;
	_FLOAT			fValue;
	_DOUBLE			dValue;

	wchar_t			*pszValue;	// both wide and dd_string
	_CHAR 			*pzcVal;
	INTER_SAFEARRAY *prgsa;
	_BYTE_STRING	 bString;// int bsLen; and _UCHAR* bs;	
};

struct INTER_SAFEARRAY_DATA
{   /*
    Note: Please make sure that the new member added is initialized in ctor. 
	ctor added by emerson due to 2010 iterator issue(crash on clear())
	*/
    INTER_SAFEARRAY_DATA(): cDims(0),
							fFeatures(0),
							cbElements(0),
							pvData(NULL),
							varType(RUL_NULL)
         {};

	_USHORT					cDims;		// Count of dimensions in this array. 
	_USHORT					fFeatures;	// Flags used by the SafeArray routines.
	_USHORT					cbElements;	// Size of an element of the array. 
	                                    //         Does not include size of pointed-to //data.
	_UCHAR *				pvData;		// Void pointer to the data. 
	VARIANT_TYPE			varType;
	VECBOUNDS				vecBounds;  // Vector can handle its own initialization.
};

#endif
