/*************************************************************************************************
 *
 * INTER_VARIENT.cpp
 * 
 *************************************************************************************************
 * The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2002-2008, HART Communication Foundation, All Rights Reserved 
 *************************************************************************************************
 *
 * Description:
 *		The self contained varient used in the Method Execution Engine
 *
 * Component History: 
 *	
 */

#include "pch.h"
#include <stdio.h>
#include <assert.h>
#include "INTER_VARIANT.h"
#include <math.h>
#include "ParserDeclarations.h"
#include "INTER_SAFEARRAY.h"
#include <ctype.h>

//jad vs2017
#ifdef ANDROID
#include <cmath>
#include <cstdlib>
#endif

//jad vs2017
#ifdef APPLE
#include <float.h>
#endif


/** 
*	stevev feb08 - rework entire class 
*  all strings are WIDE unicode chars: RUL_DD_STRING is NOT char but wchar_t now
*  none of the 5 string types may be cast to a const eg: int k = (int)DD_STRING-type is k = 0;
*  operator= NEVER changes the lvalue's type UNLESS the lvalue's type is RUL_NULL 
*  The string-type operator= SAFEARRAY operation will only work if the safearray type matches
*		the string type.  eg it is a safearray of char and the lvalue is a charptr
*	Use GetString() if you want a string conversion.
*  This feb workover HAS NOT TOUCHED the operator methods (eg <=, == etc)
*  This feb workover HAS NOT TOUCHED the safearray class AT ALL!
*********************************************************************
*  WHS EP reworked / refactored the whole thing june of 2008.
*  Indications are that safearray was not changed in this workover.
*  All operators are reportedly functional for all types. 
**/

char vtype_strings[VAR_TYPE_STRING_CNT][VAR_TYPE_STRING_MXLEN] = { VARIANT_TYPE_STRINGS };


struct INT_RANK{ int rank; bool is_unsigned; };
#define isSigned( a )    ( ! a.is_unsigned )
#define isUNSigned( b )  ( a.is_unsigned )

// these are SCANF formats
#ifdef linux
    #define ULLXFMT "%Lx"
    #define ULLOFMT "%Lo"
    #define ULLDFMT "%Ld"
    #define ULLUFMT "%Lu"
#else // >>> Not linux
    #define ULLXFMT "%I64x"
    #define ULLOFMT "%I64o"
    #define ULLDFMT "%I64d"
    #define ULLUFMT "%I64u"
#endif
#if defined(linux) && ! defined(__INT64DEFFED__)
    #define __int64 long long
// else it's a natural __int64
    #define __INT64DEFFED__ 1
#endif

/* promotes the two types according to C rules, returns type of result */
static VARIANT_TYPE promote( INTER_VARIANT& inOne, INTER_VARIANT& outOne, 
							 INTER_VARIANT& inTwo, INTER_VARIANT& outTwo) 
{
	VARIANT_TYPE  retType = RUL_NULL;
	INTER_VARIANT local;
	outOne.Clear(); outOne = inOne;
	outTwo.Clear(); outTwo = inTwo;
	if ( (! inOne.isNumeric()) || (! inTwo.isNumeric()) )
	{	return retType; // an error 
	}

	INT_RANK oneRank, twoRank, oneCnvt={0,false},twoCnvt= {0,false};
	
/*	First, if the corresponding real type of either operand is long double, the other
operand is converted, without change of type domain, to a type whose
corresponding real type is long double.   */
	/*
	--- We don't support long double at this time 
	*/ 
/*  Otherwise, if the corresponding real type of either operand is double, the other
operand is converted, to a double. */
	if ( outOne.GetVarType() == RUL_DOUBLE && outTwo.GetVarType() != RUL_DOUBLE )
	{	local.Clear(); local = (double)outTwo;  outTwo.Clear(); outTwo = local;
		retType = RUL_DOUBLE;
	}
	else if ( outOne.GetVarType() != RUL_DOUBLE && outTwo.GetVarType() == RUL_DOUBLE )
	{	local.Clear(); local = (double)outOne;  outOne.Clear(); outOne = local;
		retType = RUL_DOUBLE;
	}
	else if ( outOne.GetVarType() == RUL_DOUBLE && outTwo.GetVarType() == RUL_DOUBLE )
	{	local.Clear(); local = (double)outOne;  outOne.Clear(); outOne = local;
		retType = RUL_DOUBLE;
	}

/*  Otherwise, if the corresponding real type of either operand is float, the other
operand is converted to a float.  */
	else if ( outOne.GetVarType() == RUL_FLOAT && outTwo.GetVarType() != RUL_FLOAT )
	{	local.Clear(); local = (float)outTwo;  outTwo.Clear(); outTwo = local;
		retType = RUL_FLOAT;
	}
	else if ( outOne.GetVarType() != RUL_FLOAT && outTwo.GetVarType() == RUL_FLOAT )
	{	local.Clear(); local = (float)outOne;  outOne.Clear(); outOne = local;
		retType = RUL_FLOAT;
	}
	else if ( outOne.GetVarType() == RUL_FLOAT && outTwo.GetVarType() == RUL_FLOAT )
	{	local.Clear(); local = outOne;  outOne.Clear(); outOne = local;
		retType = RUL_FLOAT;
	}

/*  Otherwise, the integer promotions are performed on both operands. Then the
following rules are applied to the promoted operands:   */
	if ( retType == RUL_NULL ) // not a float type...
	{
		switch (outOne.GetVarType())
		{
		case	RUL_BOOL:			oneRank.is_unsigned = false; oneRank.rank = 1;  break;
		case	RUL_CHAR:			oneRank.is_unsigned = false; oneRank.rank = 2;  break;
		case	RUL_UNSIGNED_CHAR:	oneRank.is_unsigned =  true; oneRank.rank = 2;  break;
		case	RUL_SHORT:			oneRank.is_unsigned = false; oneRank.rank = 3;  break;
		case	RUL_USHORT:			oneRank.is_unsigned =  true; oneRank.rank = 3;  break;
		case	RUL_INT:			oneRank.is_unsigned = false; oneRank.rank = 4;  break;
		case	RUL_UINT:			oneRank.is_unsigned =  true; oneRank.rank = 4;  break;
		case	RUL_LONGLONG:		oneRank.is_unsigned = false; oneRank.rank = 5;  break;
		case	RUL_ULONGLONG:		oneRank.is_unsigned =  true; oneRank.rank = 5;  break;
		//case	RUL_FLOAT:
		//case	RUL_DOUBLE:
		default:	return RUL_NULL; /* error */ break;
		}// end switch
		switch (outTwo.GetVarType())
		{
		case	RUL_BOOL:			twoRank.is_unsigned = false; twoRank.rank = 1;  break;
		case	RUL_CHAR:			twoRank.is_unsigned = false; twoRank.rank = 2;  break;
		case	RUL_UNSIGNED_CHAR:	twoRank.is_unsigned =  true; twoRank.rank = 2;  break;
		case	RUL_SHORT:			twoRank.is_unsigned = false; twoRank.rank = 3;  break;
		case	RUL_USHORT:			twoRank.is_unsigned =  true; twoRank.rank = 3;  break;
		case	RUL_INT:			twoRank.is_unsigned = false; twoRank.rank = 4;  break;
		case	RUL_UINT:			twoRank.is_unsigned =  true; twoRank.rank = 4;  break;
		case	RUL_LONGLONG:		twoRank.is_unsigned = false; twoRank.rank = 5;  break;
		case	RUL_ULONGLONG:		twoRank.is_unsigned =  true; twoRank.rank = 5;  break;
		//case	RUL_FLOAT:
		//case	RUL_DOUBLE:
		default:	return RUL_NULL; /* error */ break;	
		}// end switch

	/* If both operands have the same type, then no further conversion is needed. */
		if ( outOne.GetVarType() == outTwo.GetVarType() )
		{	return outTwo.GetVarType();// done
		}

	/* Otherwise, if both operands have signed integer types or both have unsigned
	integer types, the operand with the type of lesser integer conversion rank is
	converted to the type of the operand with greater rank. */
		if ( (( oneRank.is_unsigned) && ( twoRank.is_unsigned)) || 
			 ((!oneRank.is_unsigned) && (!twoRank.is_unsigned))  )
		{// lower to higher
			oneCnvt,twoCnvt;
			if (oneRank.rank > twoRank.rank)
			{
				twoCnvt = oneRank;// other stays empty
			}
			else
			{
				oneCnvt = twoRank;// other stays empty
			}
		}
		else // one is signed, the other is unsigned
	/* Otherwise, if the operand that has unsigned integer type has rank greater or
	equal to the rank of the type of the other operand, then the operand with
	signed integer type is converted to the type of the operand with unsigned
	integer type.*/
		if ( oneRank.is_unsigned && oneRank.rank >= twoRank.rank )
		{// two converted to one's type
			twoCnvt = oneRank;// other stays empty
		}
		else
		if ( twoRank.is_unsigned && twoRank.rank >= oneRank.rank )
		{// one converted to two's type
			oneCnvt = twoRank;// other stays empty
		}
		else
	/* Otherwise, if the type of the operand with signed integer type can represent
	all of the values of the type of the operand with unsigned integer type, then
	the operand with unsigned integer type is converted to the type of the
	operand with signed integer type.  */
		if ( (!oneRank.is_unsigned) && oneRank.rank > twoRank.rank)
		{//two converted to one's type
			twoCnvt = oneRank;// other stays empty
		}
		else
		if ( (!twoRank.is_unsigned) && twoRank.rank > oneRank.rank )
		{// one converted to two's type
			oneCnvt = twoRank;// other stays empty
		}
		else
	/* Otherwise, both operands are converted to the unsigned integer type
	corresponding to the type of the operand with signed integer type.	*/
		if ( oneRank.is_unsigned )// two is SIGNED
		{//both to twoRank.rank and unsigned
			twoCnvt = oneCnvt   = twoRank;// other stays empty
			twoCnvt.is_unsigned = true;
			oneCnvt.is_unsigned = true;
		}
		else // one is SIGNED
		{//both to oneRank.rank and unsigned
			twoCnvt = oneCnvt   = oneRank;// other stays empty
			twoCnvt.is_unsigned = true;
			oneCnvt.is_unsigned = true;
		}

		// do the conversion(s)
		if (oneCnvt.rank > 0 )
		{// convert oneOut to oneCnvt type
			switch (oneCnvt.rank)
			{
			case 1:	// bool
				{	local.Clear(); local = (bool)outOne;  
					outOne.Clear(); outOne = local;
					retType = RUL_BOOL;
				}
				break;
			case 2:	// char
				{
					if (oneCnvt.is_unsigned)
					{	local.Clear();  local = (unsigned char)outOne;  
						outOne.Clear(); outOne = local;
						retType = RUL_UNSIGNED_CHAR;
					}
					else//signed
					{	local.Clear();  local = (char)outOne;  
						outOne.Clear(); outOne = local;
						retType = RUL_CHAR;
					}
				}
				break;
			case 3:	// short
				{
					if (oneCnvt.is_unsigned)
					{	local.Clear();  local = (wchar_t)outOne;  
						outOne.Clear(); outOne = local;
						retType = RUL_USHORT;
					}
					else//signed
					{	local.Clear();  local = (short)outOne;  
						outOne.Clear(); outOne = local;
						retType = RUL_SHORT;
					}
				}
				break;
			case 4:	// int
				{
					if (oneCnvt.is_unsigned)
					{	local.Clear();  local = (unsigned int)outOne;  
						outOne.Clear(); outOne = local;
						retType = RUL_UINT;
					}
					else//signed
					{
#ifdef MODIFIED_BY_SS
						//local.Clear();  local = (INT32)outOne;  
						local.Clear();  local = (short)outOne;
						outOne.Clear(); outOne = local;
						retType = RUL_INT;
#endif
					}
				}
				break;
			case 5:	// long long
				{
					if (oneCnvt.is_unsigned)
					{	local.Clear();  local = (unsigned __int64)outOne;  
						outOne.Clear(); outOne = local;
						retType = RUL_ULONGLONG;
					}
					else//signed
					{	local.Clear();  local = (__int64)outOne;  
						outOne.Clear(); outOne = local;
						retType = RUL_LONGLONG;
					}
				}
				break;
			default:
				outOne.Clear();// error
				retType = RUL_NULL;
				break;
			}// endswitch
		}// else no conversion on one

		if (twoCnvt.rank > 0 )
		{// convert twoOut to twoCnvt type
			switch (twoCnvt.rank)
			{
			case 1:	// bool
				{	local.Clear(); local = (bool)outTwo;  
					outTwo.Clear(); outTwo = local;
					retType = RUL_BOOL;
				}
				break;
			case 2:	// char
				{
					if (twoCnvt.is_unsigned)
					{	local.Clear();  local = (unsigned char)outTwo;  
						outTwo.Clear(); outTwo = local;
						retType = RUL_UNSIGNED_CHAR;
					}
					else//signed
					{	local.Clear();  local = (char)outTwo;  
						outTwo.Clear(); outTwo = local;
						retType = RUL_CHAR;
					}
				}
				break;
			case 3:	// short
				{
					if (twoCnvt.is_unsigned)
					{	local.Clear();  local = (wchar_t)outTwo;  
						outTwo.Clear(); outTwo = local;
						retType = RUL_USHORT;
					}
					else//signed
					{	local.Clear();  local = (short)outTwo;  
						outTwo.Clear(); outTwo = local;
						retType = RUL_SHORT;
					}
				}
				break;
			case 4:	// int
				{
					if (twoCnvt.is_unsigned)
					{	local.Clear();  local = (unsigned int)outTwo;  
						outTwo.Clear(); outTwo = local;
						retType = RUL_UINT;
					}
					else//signed
					{	local.Clear();  local = (short)outTwo;  
						outTwo.Clear(); outTwo = local;
						retType = RUL_INT;
					}
				}
				break;
			case 5:	// long long
				{
					if (twoCnvt.is_unsigned)
					{	local.Clear();  local = (unsigned __int64)outTwo;  
						outTwo.Clear(); outTwo = local;
						retType = RUL_ULONGLONG;
					}
					else//signed
					{	local.Clear();  local = (__int64)outTwo;  
						outTwo.Clear(); outTwo = local;
						retType = RUL_LONGLONG;
					}
				}
				break;
			default:
				outTwo.Clear();// error
				retType = RUL_NULL;
				break;
			}// endswitch
		}// else no conversion on two

	}//else let the float types stay
	return retType;
}


INTER_VARIANT::INTER_VARIANT()
{
	memset(&val,0, sizeof(val));
	varType = RUL_NULL;
}

INTER_VARIANT::INTER_VARIANT(const INTER_VARIANT& variant)
{
	memset(&val,0, sizeof(__VAL));
	varType = variant.varType;

	switch(varType)
	{
	case RUL_NULL:
		break;
	case RUL_UNSIGNED_CHAR:
		val.ucValue = variant.val.ucValue;
		break;
	case RUL_CHAR:
		val.cValue = variant.val.cValue;
		break;
	case RUL_INT:
		val.nValue = variant.val.nValue;
		break;
	case RUL_SHORT:
		val.sValue = variant.val.sValue;
		break;
	case RUL_UINT:
		val.unValue = variant.val.unValue;
		break;
	case RUL_USHORT:
		val.usValue = variant.val.usValue;
		break;
	case RUL_LONGLONG:
		val.lValue = variant.val.lValue;
		break;
	case RUL_ULONGLONG:
		val.ulValue = variant.val.ulValue;
		break;
	case RUL_BOOL:
		val.bValue = variant.val.bValue;
		break;
	case RUL_FLOAT:
		val.fValue = variant.val.fValue;
		break;
	case RUL_DOUBLE:
		val.dValue = variant.val.dValue;
		break;

	case RUL_CHARPTR:
		{
			if ( variant.val.pszValue == NULL ) /* throw error */ return;
			_INT32  nLen = strlen(variant.val.pzcVal) + 1;
			val.pzcVal = new char[nLen];
			memset(val.pzcVal,0,nLen);
			memcpy(val.pzcVal,variant.val.pzcVal,nLen);
		}
		break;

	case RUL_WIDECHARPTR:/* aka _USHORTPTR */
	case RUL_DD_STRING:
		{
			if ( variant.val.pszValue == NULL ) /* throw error */ return;
			_INT32  nLen = wcslen(variant.val.pszValue) + 1;
			val.pszValue = new wchar_t[nLen];
			memset(val.pszValue,0,(nLen*sizeof(wchar_t)));
			memcpy(val.pszValue,variant.val.pszValue,(nLen*sizeof(wchar_t)));
		}
		break;
	case RUL_SAFEARRAY:
		{
			if ( variant.val.prgsa == NULL ) /* throw error */ return;
			val.prgsa = new INTER_SAFEARRAY(*(variant.val.prgsa));
		}
		break;
		
	case RUL_BYTE_STRING:
		{
			if (variant.val.bString.bs == NULL ) /* throw error */ return;
			val.bString.bs = new _UCHAR[variant.val.bString.bsLen];
			memcpy(val.bString.bs,variant.val.bString.bs,variant.val.bString.bsLen);
			val.bString.bsLen = variant.val.bString.bsLen;
		}
		break;
	}
}

// all instances where used, isNumber is true... I don't know what it's for.
INTER_VARIANT::INTER_VARIANT(bool bIsNumber,const wchar_t* szNumber)
{
	memset(&val,0, sizeof(__VAL));
	varType = RUL_NULL;
	if(bIsNumber && szNumber != NULL)
	{	
		wstring lngStr(szNumber);
		string  shtStr;
		// convert to a short string, we don't care about letters, numbers are the same
		shtStr = TStr2AStr(lngStr);

		if ( narrowStr2number(val, varType, shtStr.c_str()) != SUCCESS )
		{// throw error
			memset(&val,0, sizeof(__VAL));
			varType = RUL_NULL;
		}
		// else - return what we have
	}
	else
	{// NaN
		// leave clear and NULL
	}
}

// all instances where used, isNumber is true...I don't know what it's for.
INTER_VARIANT::INTER_VARIANT(bool bIsNumber,const _CHAR* szNumber)
{
	memset(&val,0, sizeof(val));
	varType = RUL_NULL;
	if(bIsNumber && szNumber != NULL)
	{	
		if ( narrowStr2number(val, varType, szNumber) != SUCCESS )
		{// throw error
			memset(&val,0, sizeof(val));
			varType = RUL_NULL;
		}
		// else - return what we have
	}
	else
	{// NaN
		// leave clear and NULL
	}
	/* old code deleted 01feb08 - see earlier version to see content of replaced code */
}

INTER_VARIANT::INTER_VARIANT(void* pmem,VARIANT_TYPE vt)
{
	SetValue(pmem, vt);
}







_INT32 INTER_VARIANT::XMLize(_CHAR* szData)
{
	int slen = MAX_DD_STRING*sizeof(wchar_t);
	_CHAR str[MAX_DD_STRING*sizeof(wchar_t)]; 
	memset(str,0,slen);

	switch(varType)
	{		
	case RUL_NULL:			sprintf_s(str,sizeof(str),"%d,",0);					break;
	case RUL_BOOL:			sprintf_s(str, sizeof(str), "%d,",  val.bValue? 1:0);	break;
	case RUL_CHAR:			sprintf_s(str, sizeof(str), "%c,",  val.cValue);		break;
	case RUL_UNSIGNED_CHAR:	sprintf_s(str, sizeof(str), "%u,",  (unsigned int)val.ucValue);break;
	case RUL_SHORT:			sprintf_s(str, sizeof(str), "%hd,",  val.sValue);		break;
	case RUL_USHORT:		sprintf_s(str, sizeof(str), "%hu,",  val.usValue);		break;
	case RUL_INT:			sprintf_s(str, sizeof(str), "%d,",   val.nValue);		break;
	case RUL_UINT:			sprintf_s(str, sizeof(str), "%u,",   val.unValue);		break;
	case RUL_LONGLONG:		sprintf_s(str, sizeof(str), _LL_FMT ",",val.lValue);		break;
	case RUL_ULONGLONG:		sprintf_s(str, sizeof(str), ULL_FMT ",",val.ulValue);		break;

	case RUL_FLOAT:			sprintf_s(str, sizeof(str), "%.5f,", val.fValue);		break;
	case RUL_DOUBLE:		sprintf_s(str, sizeof(str), "%.12g,",val.dValue);		break;

	case RUL_CHARPTR:		sprintf_s(str, sizeof(str), "%s,",val.pzcVal);			break;
	case RUL_WIDECHARPTR:	sprintf_s(str, sizeof(str), "%S,",val.pszValue);		break;
	case RUL_DD_STRING:		sprintf_s(str, sizeof(str), "%S,",val.pszValue);		break;
	case RUL_BYTE_STRING:
		{
			_CHAR num[4];
			sprintf_s(str,sizeof(str),"%d:",val.bString.bsLen);
			std::string strTemp;
			for (int j = 0; j < (int)val.bString.bsLen; j++)
			{
				sprintf_s(num,sizeof(num),"%02x ",val.bString.bs[j]);
				strTemp.append(num);
			}
			strTemp.append(";");
			std::copy(strTemp.begin(), strTemp.end(), str);
			//strcat(str,";");
		}
		break;
	case RUL_SAFEARRAY:		(val.prgsa)->XMLize(szData);			break;
	default:
		break;
	}
	std::string strDataTemp(szData);
	strDataTemp.append(str);
	std::copy(strDataTemp.begin(), strDataTemp.end(), szData);
	return 0;
}

void INTER_VARIANT::SetValue(void* pmem,VARIANT_TYPE vt)
{
	Clear(); 
	memset(&val,0, sizeof(val));
	varType = vt;
	if ( pmem == NULL ) /* throw an error */return;
	int L = 0;
	switch(vt)
	{
	case RUL_NULL:
		//throw error
		break;
	case RUL_BOOL:
		memcpy(&(val.bValue),pmem,sizeof(val.bValue));
		break;
	case RUL_UNSIGNED_CHAR:
		memcpy(&(val.ucValue),pmem,sizeof(val.ucValue));
		break;
	case RUL_CHAR:
		memcpy(&(val.cValue),pmem,sizeof(val.cValue));
		break;
	case RUL_INT:
		memcpy(&(val.nValue),pmem,sizeof(val.nValue));
		break;
	case RUL_SHORT:
		memcpy(&(val.sValue),pmem,sizeof(val.sValue));
		break;
	case RUL_UINT:
		memcpy(&(val.unValue),pmem,sizeof(val.unValue));
		break;		
	case RUL_USHORT:
		memcpy(&(val.usValue),pmem,sizeof(val.usValue));
		break;		
	case RUL_LONGLONG:
		memcpy(&(val.lValue),pmem,sizeof(val.lValue));
		break;
	case RUL_ULONGLONG:
		memcpy(&(val.ulValue),pmem,sizeof(val.ulValue));
		break;
	case RUL_FLOAT:
		memcpy(&(val.fValue),pmem,sizeof(val.fValue));
		break;
	case RUL_DOUBLE:
		memcpy(&(val.dValue),pmem,sizeof(val.dValue));
		break;
	case RUL_WIDECHARPTR:
	case RUL_DD_STRING:
		L = wcslen((wchar_t*)pmem);
		// stevev 24sep10 - this doesn't handle empty strings ("")
		//if ( L > 0 && L <= MAX_DD_STRING)
		if ( L <= MAX_DD_STRING)
		{// including zero length - the empty string
			val.pszValue = new wchar_t[L+1];
#ifdef MODIFIED_BY_SS
			//wcscpy(val.pszValue,(wchar_t*)pmem);
			std::wstring srcStr(reinterpret_cast<const wchar_t*>(pmem)); // Assuming pmem is a null-terminated wide character string
			size_t srcLength = srcStr.length();
			std::copy(srcStr.begin(), srcStr.end(), val.pszValue);
			val.pszValue[srcLength] = L'\0'; // Null-terminate the destin

#endif
		}
		else
		{// just use what we can
			val.pszValue = new wchar_t[MAX_DD_STRING+1];
			wcsncpy_s( val.pszValue,  MAX_DD_STRING, (wchar_t*)pmem,_TRUNCATE);
			val.pszValue[MAX_DD_STRING] = 0;
		}
		break;
	case RUL_CHARPTR:
		L = strlen((char*)pmem);
		if ( L > 0 && L <= MAX_DD_STRING)
		{
			val.pzcVal = new char[L+1];

			
#ifdef MODIFIED_BY_SS
			//strcpy(val.pzcVal,(char*)pmem);
			std::string srcStr(reinterpret_cast<const char*>(pmem)); // Assuming pmem is a null-terminated wide character string
			size_t srcLength = srcStr.length();
			std::copy(srcStr.begin(), srcStr.end(), val.pszValue);
			val.pszValue[srcLength] = L'\0'; // Null-terminate the destin

#endif

		}// else leave it empty
		break;
	case RUL_SAFEARRAY:
		val.prgsa = new INTER_SAFEARRAY( *((INTER_SAFEARRAY*)pmem));
		break;
	case RUL_BYTE_STRING:
		L = ((_BYTE_STRING*)pmem)->bsLen;
		if ( L > 0 && L <= MAX_DD_STRING)// arbitrary max
		{
			val.bString.bs = new _UCHAR[L];
			memcpy(val.bString.bs,(_UCHAR*)pmem,L);
		}// else leave it empty
		break;
	default:
		varType = RUL_NULL;
		break;
	}
}

// apparently equal to the cast...(vt)INTER_VARIANT
void INTER_VARIANT::GetValue(void* pmem,VARIANT_TYPE vt)
{
	__VAL v;
	if ( isNumeric() )//else, string conversion is not supported
	{
		switch(vt)
		{
		case RUL_BOOL:
			v.bValue = (bool)(*this);
			memcpy(pmem,&(v.bValue),sizeof(bool));
			break;
		case RUL_CHAR:
			v.cValue = (char)(*this);
			memcpy(pmem,&(v.cValue),sizeof(char));
			break;
		case RUL_UNSIGNED_CHAR:
			v.ucValue = (unsigned char)(*this);
			memcpy(pmem,&v.ucValue,sizeof(unsigned char));
			break;
		case RUL_SHORT:
			v.sValue = (short)(*this);
			memcpy(pmem,&(v.sValue),sizeof(short));
			break;
		case RUL_USHORT:
			v.usValue = (wchar_t)(*this);
			memcpy(pmem,&(v.usValue),sizeof(wchar_t));
			break;
		case RUL_INT:
			v.nValue = (int)(*this);
			memcpy(pmem,&(v.nValue),sizeof(int));
			break;
		case RUL_UINT:
			v.unValue = (unsigned int)(*this);
			memcpy(pmem,&(v.unValue),sizeof(unsigned int));
			break;
		case RUL_LONGLONG:
			v.lValue = (__int64)(*this);
			memcpy(pmem,&(v.lValue),sizeof(__int64));
			break;
		case RUL_ULONGLONG:
			v.ulValue = (unsigned __int64)(*this);
			memcpy(pmem,&(v.ulValue),sizeof(unsigned __int64));
			break;
		case RUL_FLOAT:
			v.fValue = (float)*this;
			memcpy(pmem,&(v.fValue),sizeof(float));
			break;
		case RUL_DOUBLE:
			v.dValue = (double)*this;
			memcpy(pmem,&(v.dValue),sizeof(double));
			break;
		default:
			/* unsupported conversion error */
			break;
		}
	}
	else
	{// NON numeric
		INTER_VARIANT localVar;
		localVar = vt;// set the type
		localVar = (*this);// converts self to desired type via operator equal
		switch(vt)
		{
		case RUL_CHARPTR:
			{
				char* pchar = localVar.GetValue().pzcVal;
				memcpy(pmem,pchar,strlen(pchar)+1);
			}
			break;
		case RUL_WIDECHARPTR:	
		case RUL_DD_STRING:		
			{
				wchar_t* pchar = localVar.GetValue().pszValue;
				memcpy(pmem,pchar,wcslen(pchar)+1);
			}
			break;
		case RUL_BYTE_STRING:
			{
				unsigned char* pchar = localVar.GetValue().bString.bs;
				memcpy(pmem,pchar,localVar.GetValue().bString.bsLen);
			}
			break;
		case RUL_SAFEARRAY:	
			{
				INTER_SAFEARRAY *pchar = localVar.GetSafeArray();
				if (pchar)
				//	memcpy(pmem,pchar->getDataPtr(),pchar->MemoryAllocated());
					memcpy(pmem,(char*)pchar, pchar->MemoryAllocated());
			}
			break;
			
		case RUL_NULL:
		default:	/* throw an error this can't happen */
			break;
		}
	}// end else numeric or not
}
/**** commented out code removed from here on 07feb08 - see earlier version for contents ****/

INTER_SAFEARRAY *INTER_VARIANT::GetSafeArray()
{
	if (varType == RUL_SAFEARRAY)
	{
		return val.prgsa;
	}
	else
	{
		return NULL;
	}
}


_INT32 INTER_VARIANT::VariantSize(VARIANT_TYPE vt)
{
	_INT32 r = 0;
	switch(vt)
	{	
	case RUL_NULL:			r = 0;					break;
	case RUL_BOOL:			r = sizeof(_BOOL);		break;
	case RUL_CHAR:			r = sizeof(_CHAR);		break;
	case RUL_UNSIGNED_CHAR:	r = sizeof(_UCHAR);		break;
	case RUL_SHORT:			r = sizeof(_INT16);		break;
	case RUL_USHORT:		r = sizeof(_UINT16);	break;
	case RUL_INT:			r = sizeof(_INT32);		break;
	case RUL_UINT:			r = sizeof(_UINT32);	break;
	case RUL_LONGLONG:		r = sizeof(_INT64);		break;
	case RUL_ULONGLONG:		r = sizeof(_UINT64);	break;
	case RUL_FLOAT:			r = sizeof(_FLOAT);		break;
	case RUL_DOUBLE:		r = sizeof(_DOUBLE);	break;
		
	case RUL_CHARPTR:
	case RUL_WIDECHARPTR:
	case RUL_DD_STRING:
							r = sizeof(_CHAR*);		break;
	case RUL_BYTE_STRING:	r = sizeof(_BYTE_STRING);break;
	case RUL_SAFEARRAY:		r = sizeof(INTER_SAFEARRAY*); break;
	default:
		r = 0;
		break;
	}// end switch

	return r;
}

//Added By Anil June 17 2005 --starts here
//This Function Has to be used Only to get the string value
// stevev - 11feb08 - will get a narrow-character version of a string
//		note that that VARIANT_TYPE makes no sense here and will be discarded
//			return will be char string, source will be valType of self
void INTER_VARIANT::GetStringValue(char** pmem,VARIANT_TYPE vt)
{
	if ( pmem == 0 || *pmem == 0 )
	{
		LOGIT( CERR_LOG, "GetStringValue:char got null ptrr.\n" );
		LOGIT(CERR_LOG, "GetStringValue:VARIANT_TYPE vt = %d.\n",vt);
		return;
	}
	wstring wStr;
	string  Str;
	
	if ( varType == RUL_SAFEARRAY )
	{
		INTER_VARIANT lIV;
		int cnt = 0;

		INTER_SAFEARRAY *pSA = GetSafeArray();
		if ( pSA && (cnt = pSA->GetNumberOfElements()) > 0 )
		{
			if ( pSA->Type() == RUL_CHARPTR  || pSA->Type() == RUL_CHAR)// emerson checkin april2013
			{
				for ( int i = 0; i < cnt; i++)
				{
					pSA->GetElement(i,&lIV);
					Str += ((char) lIV);
					lIV.Clear();
				}
			}
			else
			if (pSA->Type() == RUL_WIDECHARPTR || pSA->Type() == RUL_DD_STRING)
			{
				for ( int i = 0; i < cnt; i++)
				{
					pSA->GetElement(i,&lIV);
					wStr += ((wchar_t) lIV);
					lIV.Clear();
				}
				Str  = TStr2AStr(wStr);
			}
			else
			if (pSA->Type() == RUL_SAFEARRAY)
			{
				;// multi-dimensional strings are not supported
			}
			//else - numeric conversion not supported
		}
	}
	else
	if ( varType == RUL_CHARPTR  )
	{
		if(val.pzcVal)
		{
			Str = val.pzcVal;
		}
	}
	else
	if ( varType == RUL_WIDECHARPTR || varType == RUL_DD_STRING )
	{
		if(val.pszValue)
		{
		wStr = val.pszValue;
		Str  = TStr2AStr(wStr);
		}
	}
	else
	if ( varType == RUL_FLOAT )
	{
#ifdef MODIFIED_BY_SS
		//sprintf(*pmem,"%.8E",val.fValue);
		std::snprintf(*pmem,20, "%.8E", val.fValue);
#endif
		return;
	}
	if ( varType == RUL_LONGLONG )
	{
#ifdef MODIFIED_BY_SS
		//sprintf(*pmem,"0x%llx",val.ulValue);
		std::snprintf(*pmem, 20, "0x%llx", val.ulValue);
#endif
		
		return;
	}

	*pmem = new _CHAR[Str.size()+1]; // ws 12mar08 return empty string instead od null pointer
	memset(*pmem,0,Str.size()+1);
	if ( Str.size() > 0 )
	{
		memcpy(*pmem,Str.c_str(),Str.size()+1);
		LOGIT(CLOG_LOG,"getStrValue for char: '%s'\n",*pmem);// pwink 01
	}
}
// stevev - 11feb08 - will get a wide-character version of a string
//		note that that VARIANT_TYPE makes no sense here and will be discarded
//			return will be wide-char string; source will be valType of self
void INTER_VARIANT::GetStringValue(wchar_t** pTmem,VARIANT_TYPE vt)
{
	if ( pTmem == NULL ) return;
	wstring wStr;
	 string  Str;
	
	if ( varType == RUL_SAFEARRAY )
	{
		INTER_VARIANT lIV;
		int cnt = 0;

		INTER_SAFEARRAY *pSA = GetSafeArray();
		if ( pSA && (cnt = pSA->GetNumberOfElements()) > 0 )
		{
			if ( pSA->Type() == RUL_CHAR )
			{
				for ( int i = 0; i < cnt; i++)
				{
					pSA->GetElement(i,&lIV);
					Str += ((char) lIV);
					lIV.Clear();
				}
				wStr = AStr2TStr(Str);
			}
			else
			if ( pSA->Type() == RUL_UNSIGNED_CHAR )
			{
				for ( int i = 0; i < cnt; i++)
				{
					pSA->GetElement(i,&lIV);
					Str += ((char) lIV);
					lIV.Clear();
				}
				wStr = AStr2TStr(Str);
			}
			else
			if (pSA->Type() == RUL_USHORT)
			{
				for ( int i = 0; i < cnt; i++)
				{
					pSA->GetElement(i,&lIV);
					wStr += ((wchar_t) lIV);
					lIV.Clear();
				}
			}
			else
			if (pSA->Type() == RUL_SAFEARRAY)
			{
				;// multi-dimensional strings are not supported
			}
			//else - numeric conversion not supported
		}
	}
	
	if ( varType == RUL_CHARPTR  )
	{
		Str  = val.pzcVal;
		wStr = AStr2TStr(Str);
	}

	if ( varType == RUL_WIDECHARPTR || varType == RUL_DD_STRING )
	{
		wStr = val.pszValue;
#ifdef _DEBUG
		Str = TStr2AStr(wStr);
#endif
	}

	if ( wStr.size() > 0 )
	{
		*pTmem = new wchar_t[wStr.size()+1]; 
		memset(*pTmem,0,(wStr.size()+1)*sizeof(wchar_t));// stevev 24sep10 - we were missing the ending zero
		//memcpy(*pTmem,wStr.c_str(),((wStr.size()+1)*sizeof(wchar_t)));
		wcsncpy_s(*pTmem,  wStr.size(), wStr.c_str(), _TRUNCATE);// stevev - we don't need to copy the rest of it

		//LOGIT(CLOG_LOG,L"getStrValue for wchar: '%s'\n",*pTmem);// pwink 01
	}
}

// stevev - 28may09  - return a wstring for a more modern handling scenario
wstring INTER_VARIANT::GetStringValue(void)
{
	wchar_t* pwcLoc = NULL;
	GetStringValue(&pwcLoc);

	wstring retVal(pwcLoc);
	delete[] pwcLoc;

	return retVal;
}



// stevev 05jun07 - used to detect if promotion is possible
//moved isNumeric() to .h file (it wouldn't link in TOK)


INTER_VARIANT::~INTER_VARIANT()
{
	Clear();
}

void INTER_VARIANT::Clear()
{
	switch(varType)
	{
	case RUL_NULL:
		break;
	case RUL_BOOL:
	case RUL_CHAR:
	case RUL_UNSIGNED_CHAR:
	case RUL_INT:
	case RUL_SHORT:
	case RUL_UINT:
	case RUL_USHORT:
	case RUL_LONGLONG:
	case RUL_ULONGLONG:
	case RUL_FLOAT:
	case RUL_DOUBLE:
		varType = RUL_NULL;
		memset(&val,0, sizeof(val));
		break;

	case RUL_CHARPTR:
		varType = RUL_NULL;
		if(val.pzcVal)
			delete[] val.pzcVal;
		memset(&val,0, sizeof(val));
		break;
	case RUL_WIDECHARPTR:
	case RUL_DD_STRING:	
		varType = RUL_NULL;
		if(val.pszValue)
			delete[] val.pszValue;
		memset(&val,0, sizeof(val));
		break;
	case RUL_BYTE_STRING:
		varType = RUL_NULL;
		if(val.bString.bs)
			delete[] val.bString.bs;
		memset(&val,0, sizeof(val));
		break;

	case RUL_SAFEARRAY:
		varType = RUL_NULL;
		//Anil 250407 I am wondering why this is here from such a long time.
		//This memory gets allocated during the Declaration List execution(ie char sztemp[100];). 
		//This should not be deleted afterwards.<were does it go?...sjv 01jun07??>
		//Hence Commenting
		DELETE_PTR(val.prgsa);// uncommented WS:EPM 17jul07 checkin
		memset(&val,0, sizeof(val));// make sure its alllll zero
		break;
	}
}

/***********************************************************************************************
 * start of operator= functions
 *
 * operator= ONLY changes the varType if the varType was RUL_NULL (untyped) otherwise
 *	the incoming value is cast to the lvalue's varType
 *
 * string types cannot be set operator= to numeric types NOR vice versa
 *
 *************************/
// a helper - duplicate code extracted to a macro
#define CLEAR_DATA( typ ) \
{  VARIANT_TYPE vt=varType; \
	if (vt != RUL_SAFEARRAY) { Clear(); varType=( (vt==RUL_NULL) ? ( typ ) : vt); }\
	else if (val.prgsa) val.prgsa->makeEmpty(); /*else leave it null*/  }

/*** the following code is used inside all of the numeric data type's operator= functions ***/
#define NUMERIC_EQUALS( x ) \
switch (varType )	{\
	case RUL_BOOL:			val.bValue = ((x != 0 )?true:false);	break;\
	case RUL_CHAR:			val.cValue = ( char )      x;			break;\
	case RUL_UNSIGNED_CHAR:	val.ucValue = (unsigned char)  x;		break;\
	case RUL_SHORT:			val.sValue  = ( short )    x;			break;\
	case RUL_USHORT:		val.usValue = (wchar_t) x;		break;\
	case RUL_INT:			val.nValue  = ( int )      x;			break;\
	case RUL_UINT:			val.unValue = ( unsigned int ) x;		break;\
	case RUL_LONGLONG:		val.lValue  = ( __int64 )  x;			break;\
	case RUL_ULONGLONG:		val.ulValue = ( unsigned __int64) x;	break;\
	case RUL_FLOAT:			val.fValue  = ( float )    x;			break;\
	case RUL_DOUBLE:		val.dValue  = (double)     x;			break;\
	case RUL_CHARPTR:		case RUL_WIDECHARPTR:	case RUL_BYTE_STRING: \
	case RUL_DD_STRING:		case RUL_SAFEARRAY:/* NUMERIC to STRING conversion attempt*/break;\
	case RUL_NULL:	default:  /* throw an error - shouldn't be possible to be here */   break;\
	}/* end switch */

/*
 ***********************************************************************************************/


const INTER_VARIANT& INTER_VARIANT::operator=(bool n)
{
	CLEAR_DATA( RUL_BOOL );
	int x = 0;  if (n) x = 1;
	
	switch (varType )	
	{
	case RUL_BOOL:			val.bValue = n;							break;
	case RUL_CHAR:			val.cValue = ( char )      x;			break;
	case RUL_UNSIGNED_CHAR:	val.ucValue = (unsigned char)  x;		break;
	case RUL_SHORT:			val.sValue  = ( short )    x;			break;
	case RUL_USHORT:		val.usValue = (wchar_t) x;		break;
	case RUL_INT:			val.nValue  = ( int )      x;			break;
	case RUL_UINT:			val.unValue = ( unsigned int ) x;		break;
	case RUL_LONGLONG:		val.lValue  = ( __int64 )  x;			break;
	case RUL_ULONGLONG:		val.ulValue = ( unsigned __int64) x;	break;
	case RUL_FLOAT:			val.fValue  = ( float )    x;			break;
	case RUL_DOUBLE:		val.dValue  = (double)     x;			break;

	case RUL_CHARPTR:		
	case RUL_WIDECHARPTR:	
	case RUL_BYTE_STRING: 
	case RUL_DD_STRING:		
	case RUL_SAFEARRAY:
		// * NUMERIC to STRING conversion attempt * //
		break;
	case RUL_NULL:	
	default:  
		// * throw an error - shouldn't be possible to be here * //   
		break;
	}// * end switch * //

	return *this;
}


const INTER_VARIANT& INTER_VARIANT::operator=(_CHAR n)
{
	CLEAR_DATA( RUL_CHAR );

	NUMERIC_EQUALS( n );

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(_UCHAR n)
{
	CLEAR_DATA( RUL_UNSIGNED_CHAR );

	NUMERIC_EQUALS( n );

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(wchar_t n)// aka _USHORT
{
	CLEAR_DATA( RUL_USHORT );

	NUMERIC_EQUALS( n );

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(_SHORT n)
{
	CLEAR_DATA( RUL_SHORT );

	NUMERIC_EQUALS( n );

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(int n)
{
	CLEAR_DATA( RUL_INT );

	NUMERIC_EQUALS( n );

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(unsigned int n)
{
	CLEAR_DATA( RUL_UINT );

	NUMERIC_EQUALS( n );

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(_INT64 n)
{
	CLEAR_DATA( RUL_LONGLONG );

	NUMERIC_EQUALS( n );

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(_UINT64 n)
{
	CLEAR_DATA( RUL_ULONGLONG );

	// * bill has not implemented a cast from uint64 to float or double...* //
	switch (varType )	
	{
	case RUL_BOOL:			val.bValue = ((n != 0 )?true:false);	break;
	case RUL_CHAR:			val.cValue = ( char )      n;			break;
	case RUL_UNSIGNED_CHAR:	val.ucValue = (unsigned char)  n;		break;
	case RUL_SHORT:			val.sValue  = ( short )    n;			break;
	case RUL_USHORT:		val.usValue = (wchar_t) n;		break;
	case RUL_INT:			val.nValue  = ( int )      n;			break;
	case RUL_UINT:			val.unValue = ( unsigned int ) n;		break;
	case RUL_LONGLONG:		val.lValue  = ( __int64 )  n;			break;
	case RUL_ULONGLONG:		val.ulValue = ( unsigned __int64) n;	break;
	case RUL_FLOAT:			val.fValue  = ( float ) (( __int64 )n);	break;
	case RUL_DOUBLE:		val.dValue  = (double)  (( __int64 )n);	break;

	case RUL_CHARPTR:		
	case RUL_WIDECHARPTR:	
	case RUL_BYTE_STRING: 
	case RUL_DD_STRING:		
	case RUL_SAFEARRAY:
		// * NUMERIC to STRING conversion attempt* //
		break;
	case RUL_NULL:	
	default:  
		// * throw an error - shouldn't be possible to be here * //   
		break;
	}// * end switch * //

	return *this;
}
//jad attempted arm64 fix
//jad arm64 debug
/*
const INTER_VARIANT& INTER_VARIANT::operator=(__int64 n)
{
	CLEAR_DATA(RUL_LONGLONG);

	NUMERIC_EQUALS(n);

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(unsigned __int64 n)
{
	CLEAR_DATA(RUL_ULONGLONG);

	// * bill has not implemented a cast from uint64 to float or double...* //
	switch (varType)
	{
	case RUL_BOOL:			val.bValue = ((n != 0) ? true : false);	break;
	case RUL_CHAR:			val.cValue = (char)n;			break;
	case RUL_UNSIGNED_CHAR:	val.ucValue = (unsigned char)n;		break;
	case RUL_SHORT:			val.sValue = (short)n;			break;
	case RUL_USHORT:		val.usValue = (wchar_t)n;		break;
	case RUL_INT:			val.nValue = (int)n;			break;
	case RUL_UINT:			val.unValue = (unsigned int)n;		break;
	case RUL_LONGLONG:		val.lValue = (__int64)n;			break;
	case RUL_ULONGLONG:		val.ulValue = (unsigned __int64)n;	break;
	case RUL_FLOAT:			val.fValue = (float)((__int64)n);	break;
	case RUL_DOUBLE:		val.dValue = (double)((__int64)n);	break;

	case RUL_CHARPTR:
	case RUL_WIDECHARPTR:
	case RUL_BYTE_STRING:
	case RUL_DD_STRING:
	case RUL_SAFEARRAY:
		// * NUMERIC to STRING conversion attempt* //
		break;
	case RUL_NULL:
	default:
		// * throw an error - shouldn't be possible to be here * //   
		break;
	}// * end switch * //

	return *this;
}
*/

const INTER_VARIANT& INTER_VARIANT::operator=(_FLOAT n)
{
	CLEAR_DATA( RUL_FLOAT );

	NUMERIC_EQUALS( n );

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(long n)
{
	CLEAR_DATA(RUL_INT);

	NUMERIC_EQUALS(n);

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(unsigned long n)
{
	CLEAR_DATA(RUL_UINT);

	NUMERIC_EQUALS(n);

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=(_DOUBLE n)
{
	CLEAR_DATA( RUL_DOUBLE );

	NUMERIC_EQUALS( n );

	return *this;
}

/********* strings ********** strings ************* strings ********** strings *****************/
/* stevev 26dec07
	If we are wide bodied (wide or dd-string) then the user meant to convert.
	If we are NULL (just generated) or normal char type then just put it in.
   We treat this as a char string but put it into the (overloaded) pszValue
*/

const INTER_VARIANT& INTER_VARIANT::operator=(_CHAR* psz)
{	//Added By Anil June 16 2005 --starts here
	//This was required as this may get called even if it is RUL_DD_STRING: 
	//This has to be implimented Evey Where
//stevev 13feb08.this occurs on empty string for some reason...	assert(psz != NULL);
//       change to:
	char mtStr[] = {0};
	if (psz == NULL)
	{
		psz = &(mtStr[0]);
	}

	int inLen    = strlen(psz);
	int storeLen = inLen + 1;
	
	CLEAR_DATA( RUL_CHARPTR );
	
	if ( varType == RUL_CHARPTR )
	{
		val.pzcVal = new char[storeLen]; 
		memcpy(val.pzcVal,psz,storeLen);
		val.pzcVal[inLen] = 0;
	}
	else
	if ( varType == RUL_WIDECHARPTR || varType == RUL_DD_STRING)
	{// narrow to wide conversion (destination never changes in operator=)
		wstring tmpstr;
		string  tz(psz);		
		tmpstr = AStr2TStr(tz);

		val.pszValue = new tchar[tmpstr.size() +1]; 
#ifdef MODIFIED_BY_SS
		//_tstrcpy(val.pszValue, tmpstr.c_str());		
		std::copy(tmpstr.begin(), tmpstr.end(), val.pszValue);
#endif
		
		val.pszValue[tmpstr.size()] = 0;
	}
	else
	if ( varType == RUL_BYTE_STRING )
	{// converted to unsigned
		val.bString.bs    = new uchar[storeLen]; 
		val.bString.bsLen = storeLen;
		memset(val.bString.bs,0,storeLen);
		memcpy(val.bString.bs,psz,storeLen);
	}
	else 
	if( varType == RUL_SAFEARRAY )
	{
		if ( val.prgsa == NULL )
			val.prgsa = new INTER_SAFEARRAY(psz);
		else
			*(val.prgsa) = psz;
	}
	// else throw an error - its a string to numeric conversion attempt!!!!!!!!!!!!!!!

	return *this;
}

/****** code commented * 1feb08 * was removed 07feb08 - see earlier versions for content ********/

const INTER_VARIANT& INTER_VARIANT::operator=(wchar_t* psz)
{
	if( psz )
	{
	int inLen    = wcslen(psz);
	int storeLen = inLen + 1;
	
	CLEAR_DATA( RUL_WIDECHARPTR );
	
	if ( varType == RUL_CHARPTR )
	{// wide to narrow conversion
		wstring tmpstr(psz);
		string  tz;		
		tz = TStr2AStr(tmpstr);

		val.pzcVal = new char[tz.size() +1]; 
		strncpy_s(val.pzcVal, (tz.size() + 1),tz.c_str(),_TRUNCATE);
		val.pzcVal[tz.size()] = 0;
	}
	else
	if ( varType == RUL_WIDECHARPTR || varType == RUL_DD_STRING)
	{// wide to copy of wide
		val.pszValue = new wchar_t[storeLen]; 
		memcpy(val.pszValue,psz,storeLen * sizeof(wchar_t));
		val.pszValue[inLen] = 0;
	}
	else
	if ( varType == RUL_BYTE_STRING )
	{// converted to unsigned bytes
		storeLen = inLen * sizeof(wchar_t);
		val.bString.bs    = new uchar[storeLen]; 
		val.bString.bsLen = storeLen;
		memcpy(val.bString.bs,(unsigned char*)psz,storeLen);
	}
	else 
	if( varType == RUL_SAFEARRAY )
	{
		if ( val.prgsa == NULL )
			val.prgsa = new INTER_SAFEARRAY(psz);
		else
			*(val.prgsa) = psz;
	}
	// else throw an error - its a string to numeric conversion attempt!!!!!!!!!!!!!!!
	}
	else
	{
		CLEAR_DATA( RUL_WIDECHARPTR );
	}

	return *this;
}


const INTER_VARIANT& INTER_VARIANT::operator=(const INTER_VARIANT& var)
{	
	const INTER_VARIANT* ptV = &var;
	INTER_VARIANT* pV = (INTER_VARIANT*)ptV;

	CLEAR_DATA( var.varType );// only changes type is its currently RUL_NULL
	
	int z; unsigned int w;

	switch(varType)
	{	
	case RUL_BOOL:		
		if ( pV->isNumeric() )			operator=( (bool)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_CHAR:		
		if ( pV->isNumeric() )			operator=( (char)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_UNSIGNED_CHAR:		
		if ( pV->isNumeric() )			operator=( (unsigned char)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_SHORT:		
		if ( pV->isNumeric() )			operator=( (short)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_USHORT:	
		if ( pV->isNumeric() )			operator=( (wchar_t)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_INT:	
		z = (int)*pV;
		if ( pV->isNumeric() )			operator=( z );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_UINT:
		w = (unsigned int)*pV;
		if ( pV->isNumeric() )			operator=( w );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_LONGLONG:
		if ( pV->isNumeric() )			operator=( (__int64)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_ULONGLONG:
		if ( pV->isNumeric() )			operator=( (unsigned __int64)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_FLOAT:
		if ( pV->isNumeric() )			operator=( (float)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_DOUBLE:
		if ( pV->isNumeric() )			operator=( (double)*pV );
		// else - string to numeric conversion attempt - returns clear
		break;
	case RUL_CHARPTR:		//	-|
	case RUL_WIDECHARPTR:	//	 |
	case RUL_DD_STRING:		//	 |---------- LVALUE type
	case RUL_BYTE_STRING:	//	 |
	case RUL_SAFEARRAY:		//	-|
		if (var.varType == RUL_CHARPTR)   // rvalue type
		{
			operator=((char*)(pV->GetValue().pzcVal));  // converts to all 5
		}
		else
		if (var.varType == RUL_WIDECHARPTR || var.varType == RUL_DD_STRING) // rvalue type
		{
			if( pV->GetValue().pszValue )
			{
				operator=((wchar_t*)(pV->GetValue().pszValue));// converts to all 5
			}
		}
		else
		if (var.varType == RUL_SAFEARRAY)  // rvalue type
		{
			operator=((INTER_SAFEARRAY*)(pV->GetValue().prgsa));//NO conversion! 
		}
		else
		if (var.varType == RUL_BYTE_STRING)// rvalue type
		{// linux needs a local
		    _BYTE_STRING localBS = (_BYTE_STRING)pV->GetValue().bString;
			operator=(localBS);// converts to all 5
		}
		//else numeric to string conversion attempt - returns clear
		break;
	case RUL_NULL:
	default:
		/* throw an error this can't happen */
		break;

/**** commented-out code removed 13feb08.  See earlier version for content ****/

	}//end switch
	return *this;
}


const INTER_VARIANT& INTER_VARIANT::operator=(VARIANT_TYPE type)
{
	Clear();
	varType = type;

	return *this;
}

const INTER_VARIANT& INTER_VARIANT::operator=( INTER_SAFEARRAY *sa )
{	
	CLEAR_DATA( RUL_SAFEARRAY );
	int aryTyp = sa->Type();
	int arySiz = sa->GetNumberOfElements();
	int i;
	int elemSz = sa->GetElementSize();
	INTER_VARIANT localIV;

	switch (varType)
	{
	case RUL_BOOL:
	case RUL_CHAR:
	case RUL_UNSIGNED_CHAR:
	case RUL_SHORT:
	case RUL_USHORT:
	case RUL_INT:
	case RUL_UINT:
	case RUL_LONGLONG:
	case RUL_ULONGLONG:
	case RUL_FLOAT:
	case RUL_DOUBLE:
		/* error --- attempt to set a numeric from an array */
		break;//return cleared

	/*  safearray to string is currently only allowed for matching types 
		eg array of wide chars to widecharptr  **/

	case RUL_CHARPTR:
		if ( aryTyp == RUL_CHAR )
		{
			val.pzcVal = new char[arySiz+1];
			for (i = 0; i < arySiz; i++)
			{
				sa->GetElement(i,&localIV);
				val.pzcVal[i] = (char)localIV;
				localIV.Clear();
			}
			val.pzcVal[arySiz] = 0;
		}
		// else conversion desired... not supported at this time
		break;
	case RUL_WIDECHARPTR:
	case RUL_DD_STRING:
		if ( aryTyp == RUL_USHORT )
		{
			val.pszValue = new wchar_t[arySiz+1];
			int offset = 0;
			for (i = 0; i < arySiz; offset+=elemSz, i++)
			{
				sa->GetElement(offset,&localIV);
				val.pszValue[i] = (wchar_t)localIV;
				localIV.Clear();
			}
			val.pszValue[arySiz] = 0;
		}
		// else conversion desired... not supported at this time
		break;
	case RUL_BYTE_STRING:
		if ( aryTyp == RUL_UNSIGNED_CHAR )
		{
			val.bString.bsLen = arySiz;
			val.bString.bs    = new unsigned char[arySiz];
			for (i = 0; i < arySiz; i++)
			{
				sa->GetElement(i,&localIV);
				val.bString.bs[i] = (unsigned char)localIV;
				localIV.Clear();
			}
			val.bString.bs[arySiz] = 0;
		}
		// else conversion desired... not supported at this time
		break;

	case RUL_SAFEARRAY:	
		if ( val.prgsa == NULL )
			val.prgsa = new INTER_SAFEARRAY(*sa);//allocate new safe array
		else
			*(val.prgsa) = *sa;
		break;
	case RUL_NULL:
	default:
		/* throw an error - this is impossible to get into */
		break;
	}

	return *this;
}

/* a bytestring has an internal length since it may contain embedded nulls */
const INTER_VARIANT& INTER_VARIANT::operator=(_BYTE_STRING& f)
{
	int inLen    = f.bsLen;
	int storeLen = inLen + 1;
	
	CLEAR_DATA( RUL_BYTE_STRING );
	
	if ( varType == RUL_CHARPTR )
	{// byte to string conversion
		if ( f.bs != NULL )
		{
			val.pzcVal = new char[storeLen]; 
			memcpy(val.pzcVal,(char*)f.bs,storeLen);// copy past first null just-in-case
			val.pzcVal[inLen] = 0;					// force string termination if not-a-string
		}
		else
		{
			val.pzcVal = new char; 
			val.pzcVal = NULL;
		}
	}
	else
	if ( varType == RUL_WIDECHARPTR || varType == RUL_DD_STRING)
	{// byte to wide
		val.pszValue = new wchar_t[storeLen]; 
		for (int y = 0; y < inLen; val.pszValue[y]=f.bs[y],y++);// one wide-char per char
		val.pszValue[inLen] = 0;// force string termination if not-a-string
	}
	else
	if ( varType == RUL_BYTE_STRING )
	{
		val.bString.bs    = new uchar[inLen]; 
		val.bString.bsLen = inLen;
		memcpy(val.bString.bs,(unsigned char*)f.bs,inLen);
	}
	else 
	if( varType == RUL_SAFEARRAY )
	{
		if ( val.prgsa == NULL )
			val.prgsa = new INTER_SAFEARRAY(&f);
		else
			*(val.prgsa) = f;
	}
	// else throw an error - its a string to numeric conversion attempt!!!!!!!!!!!!!!!

	return *this;
}




/**** casts ******** casts ******** casts ******** casts ******** casts ******** casts **********/

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator bool() 
{
	bool retVal = 0;
	switch (varType)
	{	
	case RUL_BOOL:
		retVal = val.bValue;
		break;
	case RUL_CHAR:
		retVal = (bool)( val.cValue != 0 );
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (bool)(val.ucValue != 0 );
		break;
	case RUL_SHORT:
		retVal = (bool)(val.sValue!=0);
		break;
	case RUL_USHORT:
		retVal = (bool)(val.usValue!=0);
		break;
	case RUL_INT:
		retVal = (bool)(val.nValue!=0);
		break;
	case RUL_UINT:
		retVal = (bool)(val.unValue!=0);
		break;
	case RUL_LONGLONG:
		retVal = (bool) (val.lValue!=0);
		break;
	case RUL_ULONGLONG:
		retVal = (bool) (val.ulValue!=0);
		break;
	case RUL_FLOAT:
		retVal =  (bool) (val.fValue!=0.0);
		break;
	case RUL_DOUBLE:
		retVal =  (bool) (val.dValue!=0.0);
		break;
	default:
		retVal = (bool)0; 
		break;
	}// end switch

	return retVal;
}

    
//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator int()
{
	int retVal = 0;
	switch(varType)
	{	
	case RUL_BOOL:
		if( val.bValue )
		{
			retVal = (int)1;
		}
		break;
	case RUL_CHAR:
		retVal = (int)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (int)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (int)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (int)val.usValue;
		break;
	case RUL_INT:
		retVal = (int)val.nValue;
		break;
	case RUL_UINT:
		retVal = (int)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (int) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (int) val.ulValue;
		break;
	case RUL_FLOAT:
		retVal =  (int) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (int) val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (int)0; 
		break;
	}// end switch
	return retVal;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator unsigned int(void)
{
	unsigned int retVal = 0;
	switch(varType)
	{	
	case RUL_BOOL:
		if( val.bValue )
		{
			retVal = (unsigned int)1;
		}
		break;
	case RUL_CHAR:
		retVal = (unsigned int)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (unsigned int)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (unsigned int)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (unsigned int)val.usValue;
		break;
	case RUL_INT:
		retVal = (unsigned int)val.nValue;
		break;
	case RUL_UINT:
		retVal = (unsigned int)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (unsigned int) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (unsigned int) val.ulValue;
		break;
	case RUL_FLOAT:
		retVal =  (unsigned int) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (unsigned int) val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (unsigned int)0; 
		break;
	}// end switch
	return retVal;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator float()
{
	float retVal = 0.0;
	switch(varType)
	{	
	case RUL_BOOL:
		if (val.bValue )
		{
			retVal = 1.0;
		} 
		break;
	case RUL_CHAR:
		retVal = (float)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (float)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (float)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (float)val.usValue;
		break;
	case RUL_INT:
		retVal = (float)val.nValue;
		break;
	case RUL_UINT:
		retVal = (float)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (float) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (float) ((__int64)val.ulValue);
		break;
	case RUL_FLOAT:
		retVal =  (float) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (float)val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (float)0.0; 
		break;
	}// end switch
	return retVal;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator char()
{
	char retVal = 0;
	switch(varType)
	{	
	case RUL_BOOL:
		if( val.bValue )
		{
			retVal = (char)1;
		}
		break;
	case RUL_CHAR:
		retVal = (char)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (char)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (char)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (char)val.usValue;
		break;
	case RUL_INT:
		retVal = (char)val.nValue;
		break;
	case RUL_UINT:
		retVal = (char)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (char) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (char) val.ulValue;
		break;
	case RUL_FLOAT:
		retVal =  (char) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (char) val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (char)0; 
		break;
	}// end switch
	return retVal;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator unsigned char(void)
{
	unsigned char retVal = 0;
	switch(varType)
	{	
	case RUL_BOOL:
		if( val.bValue )
		{
			retVal = (unsigned char)1;
		}
		break;
	case RUL_CHAR:
		retVal = (unsigned char)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (unsigned char)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (unsigned char)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (unsigned char)val.usValue;
		break;
	case RUL_INT:
		retVal = (unsigned char)val.nValue;
		break;
	case RUL_UINT:
		retVal = (unsigned char)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (unsigned char) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (unsigned char) val.ulValue;
		break;
	case RUL_FLOAT:
		retVal =  (unsigned char) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (unsigned char) val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (unsigned char)0; 
		break;
	}// end switch
	return retVal;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator double()
{
	double retVal = 0.0;
	switch(varType)
	{	
	case RUL_BOOL:
		if (val.bValue )
		{
			retVal = 1.0;
		} 
		break;
	case RUL_CHAR:
		retVal = (double)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (double)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (double)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (double)val.usValue;
		break;
	case RUL_INT:
		retVal = (double)val.nValue;
		break;
	case RUL_UINT:
		retVal = (double)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (double) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (double) ((__int64)val.ulValue);
		break;
	case RUL_FLOAT:
		retVal =  (double) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (double)0.0; 
		break;
	}// end switch
	return retVal;
}



//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator short(void)
{
	short retVal = 0;
	switch(varType)
	{	
	case RUL_BOOL:
		if( val.bValue )
		{
			retVal = (short)1;
		}
		break;
	case RUL_CHAR:
		retVal = (short)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (short)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (short)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (short)val.usValue;
		break;
	case RUL_INT:
		retVal = (short)val.nValue;
		break;
	case RUL_UINT:
		retVal = (short)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (short) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (short) val.ulValue;
		break;
	case RUL_FLOAT:
		retVal =  (short) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (short) val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (short)0; 
		break;
	}// end switch
	return retVal;

}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator wchar_t(void)	// aka unsigned short() 
{
	unsigned short retVal = 0;
	switch(varType)
	{
	case RUL_BOOL:
		if( val.bValue )
		{
			retVal = (wchar_t)1;
		}
		break;	
	case RUL_CHAR:
		retVal = (wchar_t)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (wchar_t)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (wchar_t)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (wchar_t)val.usValue;
		break;
	case RUL_INT:
		retVal = (wchar_t)val.nValue;
		break;
	case RUL_UINT:
		retVal = (wchar_t)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (wchar_t) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (wchar_t) val.ulValue;
		break;
	case RUL_FLOAT:
		retVal =  (wchar_t) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (wchar_t) val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (wchar_t)0; 
		break;
	}// end switch
	return retVal;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator __int64(void)
{
	__int64 retVal = 0;
	switch(varType)
	{
	case RUL_BOOL:
		if( val.bValue )
		{
			retVal = (__int64)1;
		}
		break;	
	case RUL_CHAR:
		retVal = (__int64)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (__int64)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (__int64)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (__int64)val.usValue;
		break;
	case RUL_INT:
		retVal = (__int64)val.nValue;
		break;
	case RUL_UINT:
		retVal = (__int64)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (__int64) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (__int64) val.ulValue;
		break;
	case RUL_FLOAT:
		retVal =  (__int64) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (__int64) val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (__int64)0; 
		break;
	}// end switch
	return retVal;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
INTER_VARIANT::operator unsigned __int64(void)
{
	unsigned __int64 retVal = 0;
	switch(varType)
	{
	case RUL_BOOL:
		if( val.bValue )
		{
			retVal = (unsigned __int64)1;
		}
		break;	
	case RUL_CHAR:
		retVal = (unsigned __int64)val.cValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retVal = (unsigned __int64)val.ucValue;
		break;
	case RUL_SHORT:
		retVal = (unsigned __int64)val.sValue;
		break;
	case RUL_USHORT:
		retVal = (unsigned __int64)val.usValue;
		break;
	case RUL_INT:
		retVal = (unsigned __int64)val.nValue;
		break;
	case RUL_UINT:
		retVal = (unsigned __int64)val.unValue;
		break;
	case RUL_LONGLONG:
		retVal = (unsigned __int64) val.lValue;
		break;
	case RUL_ULONGLONG:
		retVal = (unsigned __int64) val.ulValue;
		break;
	case RUL_FLOAT:
		retVal =  (unsigned __int64) val.fValue;
		break;
	case RUL_DOUBLE:
		retVal =  (unsigned __int64) val.dValue;
		break;

	/* for now, all strings will NOT cast to a const */
	default:
		retVal = (unsigned __int64)0; 
		break;
	}// end switch
	return retVal;
}

/********** casting strings is not currently supported ******************************************
INTER_VARIANT::operator char *() 
{
	//Vibhor 240204: Donno if this is also required
	// stevev changed 20dec07  return val.pszValue;
	wstring tc(val.pszValue);
	charout = TStr2AStr(tc);
	strcpy((char*)val.pszValue,charout.c_str());
	return ((char*)val.pszValue);
}

INTER_VARIANT::operator wchar_t*() 
{
	//Vibhor 240204: Donno if this is also required
	return val.pszValue;
}

INTER_VARIANT::operator _BYTE_STRING(void)
{
}
************************** end of string casting ************************************************/






/** operations ******** operations ******** operations ******** operations ******** operations **/




//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator+(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
	{
		retValue = (double)*this + (double)temp;
	}
	else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
	{
		retValue = (float)*this + (float)temp;
	}
	else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
	{
		retValue = (unsigned __int64)*this + (unsigned __int64)temp;
	}
	else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
	{
		retValue = (__int64)*this + (__int64)temp;
	}
	else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
	{
		retValue = (unsigned int)*this + (unsigned int)temp;
	}
	else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
	{
		retValue = (int)*this + (int)temp;
	}
	else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
	{
		retValue = (wchar_t)*this + (wchar_t)temp;
	}
	else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
	{
		retValue = (short)*this + (short)temp;
	}// end switch
	else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
	{
		retValue = (unsigned char)*this + (unsigned char)temp;
	}
	else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
	{// not both - we can't do this...
		retValue = (char)*this + (char)temp;
	}
	else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
	{
		retValue = (bool)*this & (bool)temp;
	}
	else if(  
		(((varType == RUL_DD_STRING)||(varType == RUL_WIDECHARPTR)) && var.varType == RUL_CHARPTR)
		||
		(((var.varType == RUL_DD_STRING)||(var.varType == RUL_WIDECHARPTR)) && varType == RUL_CHARPTR)
		)
	{// narrow to wide conversion
		INTER_VARIANT widend, wideAlready;
		bool widendFirst = true;
		widend.varType = RUL_DD_STRING;
		if (var.varType == RUL_CHARPTR)
		{
			widend      =  var;  // convert to DD_STRING
			wideAlready = (*this);
			widendFirst = false;
		}
		else
		{
			widend      =  (*this);  // convert to DD_STRING
			wideAlready = var;
			widendFirst = true;
		}
		retValue.varType = RUL_DD_STRING;
		StripLangCode(widend.val.pszValue);
		StripLangCode(wideAlready.val.pszValue);

		_INT32 nLen1 = _tstrlen(widend.val.pszValue);
		_INT32 nLen2 = _tstrlen(wideAlready.val.pszValue)+1;
		retValue.val.pszValue = new tchar[nLen1+nLen2+1];
		memset(retValue.val.pszValue,0,sizeof(tchar)*(nLen1+nLen2+1));

		if (widendFirst)
		{
#ifdef MODIFIED_BY_SS
			//wcscpy(retValue.val.pszValue, widend.val.pszValue);
			//wcscat(retValue.val.pszValue, wideAlready.val.pszValue);
			std::wstring retValueStr(retValue.val.pszValue);
			std::wstring widendStr(widend.val.pszValue);
			std::wstring wideAlreadyStr(wideAlready.val.pszValue);

			// Concatenate wide character strings
			std::wstring concatenatedStr = retValueStr + widendStr + wideAlreadyStr;

			// Copy concatenated string back to retValue.val.pszValue
			wcsncpy_s(retValue.val.pszValue, concatenatedStr.length(), concatenatedStr.c_str(),_TRUNCATE);
#endif			
		}
		else
		{
			
#ifdef MODIFIED_BY_SS
			//wcscpy(retValue.val.pszValue, wideAlready.val.pszValue);
			//wcscat(retValue.val.pszValue, widend.val.pszValue);
			std::wstring retValueStr(retValue.val.pszValue);
			std::wstring widendStr(widend.val.pszValue);
			std::wstring wideAlreadyStr(wideAlready.val.pszValue);

			// Concatenate wide character strings
			std::wstring concatenatedStr = retValueStr  + wideAlreadyStr + widendStr;

			// Copy concatenated string back to retValue.val.pszValue
			wcsncpy_s(retValue.val.pszValue, concatenatedStr.length(), concatenatedStr.c_str(), _TRUNCATE);
#endif	

		}
				
	}
	//Added By Stevev 20dec07 --starts here
	else if( 
			((varType == RUL_DD_STRING) || (varType == RUL_WIDECHARPTR)) 
			&&
			((var.varType == RUL_WIDECHARPTR)||(var.varType == RUL_DD_STRING)) 
		   )
	{
		retValue.varType = RUL_DD_STRING;

		StripLangCode(val.pszValue);
		_INT32 nLen1 = _tstrlen(val.pszValue);

		StripLangCode(var.val.pszValue);
		_INT32 nLen2 = _tstrlen(var.val.pszValue)+1;

		retValue.val.pszValue = new tchar[nLen1+nLen2+1];
		memset(retValue.val.pszValue,0,sizeof(tchar)*(nLen1+nLen2+1));
#ifdef MODIFIED_BY_SS
		/*wcscpy( retValue.val.pszValue, val.pszValue );			
		wcscat( retValue.val.pszValue, var.val.pszValue );	*/
		std::wstring retValueVal(retValue.val.pszValue);
		std::wstring valStr(val.pszValue);
		retValueVal += valStr;
		std::copy(retValueVal.begin(), retValueVal.end(), retValue.val.pszValue);

#endif 

	}
	else if ((varType == RUL_CHARPTR)  &&(var.varType == RUL_CHARPTR))
	{
		retValue.varType = RUL_CHARPTR;
		_INT32 nLen1 = strlen(val.pzcVal);
		//Remove the Language Code if it is Present:This is Required as Both temp and var has Language Code
		//I am not Sure Whether I need to be support this???????????????????
		if( (var.val.pzcVal[0] == '|') &&  (var.val.pzcVal[3] == '|' ) )
		{
			int count, itemp = strlen(var.val.pzcVal);// WS - 9apr07 - 2005 checkin
			for(count = 4; count <itemp ;count++)// WS - 9apr07 - 2005 checkin
			{
				var.val.pzcVal[count-4] = var.val.pzcVal[count];
			}
			var.val.pzcVal[count-4] = '\0';
		
		}
			//Added By Anil June 16 2005 --Ends here
			_INT32 nLen2 = strlen(var.val.pzcVal)+1;
		retValue.val.pzcVal = new _CHAR[nLen1+nLen2+1];
		memset(retValue.val.pzcVal,0,nLen1+nLen2+1);

		memcpy(retValue.val.pzcVal,val.pszValue, nLen1*sizeof(_CHAR));
		memcpy(retValue.val.pzcVal+nLen1,var.val.pzcVal,nLen2*sizeof(_CHAR));
		retValue.varType = varType;
	}
	else if ((varType == RUL_BYTE_STRING)  &&(var.varType == RUL_BYTE_STRING))
	{
		retValue.varType = RUL_BYTE_STRING;
		retValue.val.bString.bsLen = val.bString.bsLen+var.val.bString.bsLen;
		retValue.val.bString.bs = new _UCHAR[retValue.val.bString.bsLen];
		memset(retValue.val.bString.bs, 0,   retValue.val.bString.bsLen);

		memcpy(retValue.val.bString.bs, val.bString.bs, val.bString.bsLen);
		memcpy(retValue.val.bString.bs+val.bString.bsLen, var.val.bString.bs, 
				   var.val.bString.bsLen);
		retValue.varType = varType;
	}
	// Walt EPM - 05sep08 - add
	else if ((varType == RUL_DD_STRING)  &&(var.varType == RUL_SAFEARRAY))
	{
		retValue.varType = RUL_DD_STRING;

		_INT32 nLen1 = _tstrlen((wchar_t *)*var.val.prgsa);

		StripLangCode(val.pszValue);
		_INT32 nLen2 = _tstrlen(val.pszValue)+1;

		retValue.val.pszValue = new tchar[nLen1+nLen2+1];
		memset(retValue.val.pszValue,0,sizeof(tchar)*(nLen1+nLen2+1));

		
#ifdef MODIFIED_BY_SS
		/*wcscpy( retValue.val.pszValue, val.pszValue );			
		wcscat( retValue.val.pszValue, (wchar_t *)*var.val.prgsa );	*/
		std::wstring retValueVal(retValue.val.pszValue);
		std::wstring valStr((wchar_t*)*var.val.prgsa);
		retValueVal += valStr;
		std::copy(retValueVal.begin(), retValueVal.end(), retValue.val.pszValue);

#endif 

	}
	// Walt EPM - 05sep08 - end
	return retValue;
	
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator-(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			retValue = (double)*this - (double)temp;
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			retValue = (float)*this - (float)temp;
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (unsigned __int64)*this - (unsigned __int64)temp;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (__int64)*this - (__int64)temp;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (unsigned int)*this - (unsigned int)temp;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (int)*this - (int)temp;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (wchar_t)*this - (wchar_t)temp;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (short)*this - (short)temp;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (unsigned char)*this - (unsigned char)temp;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (char)*this - (char)temp;
		}
		else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
		{
			retValue = (bool)*this - (bool)temp;
		}
	}

	return retValue;	
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator*(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			retValue = (double)*this * (double)temp;
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			retValue = (float)*this * (float)temp;
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (unsigned __int64)*this * (unsigned __int64)temp;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (__int64)*this * (__int64)temp;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (unsigned int)*this * (unsigned int)temp;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (int)*this * (int)temp;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (wchar_t)*this * (wchar_t)temp;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (short)*this * (short)temp;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (unsigned char)*this * (unsigned char)temp;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (char)*this * (char)temp;
		}
		else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
		{
			retValue = (bool)*this - (bool)temp;
		}
	}

	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator/(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			double dDenominator = (double)temp;
			if( dDenominator != 0.0 )
			{
				retValue = (double)*this / dDenominator;
			}
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			float fDenominator = (float)temp;
			if( fDenominator != 0.0 )
			{
				retValue = (float)*this / fDenominator;
			}
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			unsigned __int64 ullDenominator = (unsigned __int64)temp;
			if( ullDenominator != 0 )
			{
				retValue = (unsigned __int64)*this / ullDenominator;
			}
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			__int64 llDenominator = (__int64)temp;
			if( llDenominator != 0 )
			{
				retValue = (__int64)*this / llDenominator;
			}
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			unsigned int nDenominator = (unsigned int)temp;
			if( nDenominator != 0 )
			{
				retValue = (unsigned int)*this / nDenominator;
			}
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			int nDenominator = (int)temp;
			if( nDenominator != 0 )
			{
				retValue = (int)*this / nDenominator;
			}
		
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			int usDenominator = (wchar_t)temp;
			if( usDenominator != 0 )
			{
				retValue = (wchar_t)*this / usDenominator;
			}
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			int sDenominator = (short)temp;
			if( sDenominator != 0 )
			{
				retValue = (short)*this / sDenominator;
			}
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			int ucDenominator = (unsigned char)temp;
			if( ucDenominator != 0 )
			{
				retValue = (unsigned char)*this / ucDenominator;
			}
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			int cDenominator = (char)temp;
			if( cDenominator != 0 )
			{
				retValue = (char)*this / cDenominator;
			}
		}
	}

	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator%(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;
	
	
	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			unsigned __int64 ullDenominator = (unsigned __int64)temp;
			if( ullDenominator != 0 )
			{
				retValue = (unsigned __int64)*this % ullDenominator;
			}
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			__int64 llDenominator = (__int64)temp;
			if( llDenominator != 0 )
			{
				retValue = (__int64)*this % llDenominator;
			}
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			unsigned int unDenominator = (unsigned int)temp;
			if( unDenominator != 0 )
			{
				retValue = (unsigned int)*this % unDenominator;
			}
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			int nDenominator = (int)temp;
			if( nDenominator != 0 )
			{
				retValue = (int)*this % nDenominator;
			}
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			wchar_t usDenominator = (wchar_t)temp;
			if( usDenominator != 0 )
			{
				retValue = (wchar_t)*this % usDenominator;
			}
	
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			short sDenominator = (short)temp;
			if( sDenominator != 0 )
			{
				retValue = (short)*this % sDenominator;
			}
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			unsigned char ucDenominator = (unsigned char)temp;
			if( ucDenominator != 0 )
			{
				retValue = (unsigned char)*this % ucDenominator;
			}
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			char cDenominator = (char)temp;
			if( cDenominator != 0 )
			{
				retValue = (char)*this % cDenominator;
			}
		}
	}
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator&(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;	
	
	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (unsigned __int64)*this & (unsigned __int64)temp;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (__int64)*this & (__int64)temp;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (unsigned int)*this & (unsigned int)temp;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (int)*this & (int)temp;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (wchar_t)*this & (wchar_t)temp;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (short)*this & (short)temp;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (unsigned char)*this & (unsigned char)temp;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (char)*this & (char)temp;
		}
		else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
		{
			retValue = (bool)*this & (bool)temp;
		}
	}
	return retValue;	
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator|(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (unsigned __int64)*this | (unsigned __int64)temp;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (__int64)*this | (__int64)temp;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (unsigned int)*this | (unsigned int)temp;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (int)*this | (int)temp;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (wchar_t)*this | (wchar_t)temp;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (short)*this | (short)temp;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (unsigned char)*this | (unsigned char)temp;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (char)*this | (char)temp;
		}
		else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
		{
			retValue = (bool)*this | (bool)temp;
		}
	}
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator^(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;	
	
	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (unsigned __int64)*this ^ (unsigned __int64)temp;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (__int64)*this ^ (__int64)temp;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (unsigned int)*this ^ (unsigned int)temp;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (int)*this ^ (int)temp;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (wchar_t)*this ^ (wchar_t)temp;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (short)*this ^ (short)temp;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (unsigned char)*this ^ (unsigned char)temp;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (char)*this ^ (char)temp;
		}
	}
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator~()
{
	INTER_VARIANT retValue;
		
	switch(varType)
	{
	case RUL_ULONGLONG:
		retValue = ~val.ulValue;
		break;
	case RUL_LONGLONG:
		retValue = ~val.lValue;
		break;
	case RUL_UINT:
		retValue = (unsigned int)~val.unValue;
		break;
	case RUL_INT:
		retValue = (int)~val.nValue;
		break;
	case RUL_USHORT:
		retValue = ~val.usValue;
		break;
	case RUL_SHORT:
		retValue = ~val.sValue;
		break;
	case RUL_UNSIGNED_CHAR:
		retValue = ~val.ucValue;
		break;
	case RUL_CHAR:
		retValue = ~val.cValue;
		break;
	case RUL_BOOL:
		retValue = !val.bValue;
		break;
	}

	return retValue;	
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator>>(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_ULONGLONG))
		{
			retValue = (unsigned __int64)*this >> (int)temp;
		}
		else if( (varType == RUL_LONGLONG) )
		{
			retValue = (__int64)*this >> (int)temp;
		}
		else if( (varType == RUL_UINT) )
		{
			retValue = (unsigned int)*this >> (int)temp;
		}
		else if( (varType == RUL_INT) )
		{
			retValue = (int)*this >> (int)temp;
		}
		else if( (varType == RUL_USHORT) )
		{
			retValue = (wchar_t)*this >> (int)temp;
		}
		else if( (varType == RUL_SHORT) )
		{
			retValue = (short)*this >> (int)temp;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (unsigned char)*this >> (int)temp;
		}
		else if( (varType == RUL_CHAR) )
		{
			retValue = (char)*this >> (int)temp;
		}
	}
	return retValue;	
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator<<(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_ULONGLONG) )
		{
			retValue = (unsigned __int64)*this << (int)temp;
		}
		else if( (varType == RUL_LONGLONG) )
		{
			retValue = (__int64)*this << (int)temp;
		}
		else if( (varType == RUL_UINT) )
		{
			retValue = (unsigned int)*this << (int)temp;
		}
		else if( (varType == RUL_INT) )
		{
			retValue = (int)*this << (int)temp;
		}
#ifdef MODIFIED_BY_SS
		/*else if( (varType == RUL_USHORT) )
		{
			retValue = static_cast<unsigned short>(*this) << static_cast<int>(temp);
		}*/
#endif
		else if( (varType == RUL_SHORT) )
		{
			retValue = (short)*this << (short int)temp;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = static_cast<unsigned char>(*this) << static_cast<int>(temp);
		}
		else if( (varType == RUL_CHAR) )
		{
			retValue = static_cast<_CHAR>(*this) << static_cast<int>(temp);
		}
	}
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator!=(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;
	
	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			retValue = (bool)((double)*this != (double)temp)?true:false;
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			retValue = (bool)((float)*this != (float)temp)?true:false;
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (bool)((unsigned __int64)*this != (unsigned __int64)temp)?true:false;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (bool)((__int64)*this != (__int64)temp)?true:false;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (bool)((unsigned int)*this != (unsigned int)temp)?true:false;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (bool)((int)*this != (int)temp)?true:false;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (bool)((wchar_t)*this != (wchar_t)temp)?true:false;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (bool)((short)*this != (short)temp)?true:false;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (bool)((unsigned char)*this != (unsigned char)temp)?true:false;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (bool)((char)*this != (char)temp)?true:false;
		}
		else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
		{
			retValue = (bool)((bool)*this != (bool)temp)?true:false;
		}
	}
	// Walt EPM - 05sep08 - add
	else //string compares
	{
		wstring str1;
		wstring str2;
		switch( varType )
		{
			case RUL_CHARPTR:
				str1 = AStr2TStr(val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str1 = val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str1 = (wchar_t *)*val.prgsa;
				break;
		}
		switch( var.varType )
		{
			case RUL_CHARPTR:
				str2 = AStr2TStr(var.val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str2 = var.val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str2 = (wchar_t *)*var.val.prgsa;
				break;
		}
		if( wcscmp(str1.c_str(), str2.c_str()) == 0 )
		{
			retValue = (bool)false;
		}
		else
		{
			retValue = (bool)true;
		}
	}
	// Walt EPM - 05sep08 - end
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator==(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )	
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			retValue = (bool)((double)*this == (double)temp)?true:false;
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			retValue = (bool)((float)*this == (float)temp)?true:false;
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (bool)((unsigned __int64)*this == (unsigned __int64)temp)?true:false;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (bool)((__int64)*this == (__int64)temp)?true:false;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (bool)((unsigned int)*this == (unsigned int)temp)?true:false;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (bool)((int)*this == (int)temp)?true:false;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (bool)((wchar_t)*this == (wchar_t)temp)?true:false;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (bool)((short)*this == (short)temp)?true:false;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (bool)((unsigned char)*this == (unsigned char)temp)?true:false;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (bool)((char)*this == (char)temp)?true:false;
		}
		else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
		{
			retValue = (bool)((bool)*this == (bool)temp)?true:false;
		}
	}
	else//todo string compares
	{
		// Walt EPM - 05sep08 - add
		wstring str1;
		wstring str2;
		switch( varType )
		{
			case RUL_CHARPTR:
				str1 = AStr2TStr(val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str1 = val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str1 = (wchar_t *)*val.prgsa;
				break;
		}
		switch( var.varType )
		{
			case RUL_CHARPTR:
				str2 = AStr2TStr(var.val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str2 = var.val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str2 = (wchar_t *)*var.val.prgsa;
				break;
		}
		if( wcscmp(str1.c_str(), str2.c_str())== 0 )
		{
			retValue = (bool)true;
		}
		else
		{
			retValue = (bool)false;
		}
		// Walt EPM - 05sep08 - end
	}
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator<(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			retValue = (bool)((double)*this < (double)temp)?true:false;
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			retValue = (bool)((float)*this < (float)temp)?true:false;
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (bool)((unsigned __int64)*this < (unsigned __int64)temp)?true:false;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (bool)((__int64)*this < (__int64)temp)?true:false;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (bool)((unsigned int)*this < (unsigned int)temp)?true:false;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (bool)((int)*this < (int)temp)?true:false;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (bool)((wchar_t)*this < (wchar_t)temp)?true:false;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (bool)((short)*this < (short)temp)?true:false;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (bool)((unsigned char)*this < (unsigned char)temp)?true:false;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (bool)((char)*this < (char)temp)?true:false;
		}
		else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
		{
			retValue = (bool)((bool)*this < (bool)temp)?true:false;
		}
	}
	// Walt EPM - 05sep08 -  add
	else //string compares
	{
		wstring str1;
		wstring str2;
		switch( varType )
		{
			case RUL_CHARPTR:
				str1 = AStr2TStr(val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str1 = val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str1 = (wchar_t *)*val.prgsa;
				break;
		}
		switch( var.varType )
		{
			case RUL_CHARPTR:
				str2 = AStr2TStr(var.val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str2 = var.val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str2 = (wchar_t *)*var.val.prgsa;
				break;
		}
		if( wcscmp(str1.c_str(), str2.c_str()) < 0 )
		{
			retValue = (bool)true;
		}
		else
		{
			retValue = (bool)false;
		}
	}
	// Walt EPM - 05sep08 - end
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator>(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			retValue = (bool)((double)*this > (double)temp)?true:false;
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			retValue = (bool)((float)*this > (float)temp)?true:false;
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (bool)((unsigned __int64)*this > (unsigned __int64)temp)?true:false;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (bool)((__int64)*this > (__int64)temp)?true:false;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (bool)((unsigned int)*this > (unsigned int)temp)?true:false;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (bool)((int)*this > (int)temp)?true:false;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (bool)((wchar_t)*this > (wchar_t)temp)?true:false;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (bool)((short)*this > (short)temp)?true:false;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (bool)((unsigned char)*this > (unsigned char)temp)?true:false;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (bool)((char)*this > (char)temp)?true:false;
		}
		else if( (varType == RUL_BOOL) || (var.varType == RUL_BOOL) )
		{
			retValue = (bool)((bool)*this > (bool)temp)?true:false;
		}
	}
	// Walt EPM - 05sep08 - add
	else //string compares
	{
		wstring str1;
		wstring str2;
		switch( varType )
		{
			case RUL_CHARPTR:
				str1 = AStr2TStr(val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str1 = val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str1 = (wchar_t *)*val.prgsa;
				break;
		}
		switch( var.varType )
		{
			case RUL_CHARPTR:
				str2 = AStr2TStr(var.val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str2 = var.val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str2 = (wchar_t *)*var.val.prgsa;
				break;
		}
		if( wcscmp(str1.c_str(), str2.c_str()) > 0 )
		{
			retValue = (bool)true;
		}
		else
		{
			retValue = (bool)false;
		}
	}
	// Walt EPM - 05sep08 - end
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator<=(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			retValue = (bool)((double)*this <= (double)temp)?true:false;
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			retValue = (bool)((float)*this <= (float)temp)?true:false;
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (bool)((unsigned __int64)*this <= (unsigned __int64)temp)?true:false;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (bool)((__int64)*this <= (__int64)temp)?true:false;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (bool)((unsigned int)*this <= (unsigned int)temp)?true:false;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (bool)((int)*this <= (int)temp)?true:false;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (bool)((wchar_t)*this <= (wchar_t)temp)?true:false;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (bool)((short)*this <= (short)temp)?true:false;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (bool)((unsigned char)*this <= (unsigned char)temp)?true:false;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (bool)((char)*this <= (char)temp)?true:false;
		}
	}
	// Walt EPM - 05sep08 - add
	else //string compares
	{
		wstring str1;
		wstring str2;
		switch( varType )
		{
			case RUL_CHARPTR:
				str1 = AStr2TStr(val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str1 = val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str1 = (wchar_t *)*val.prgsa;
				break;
		}
		switch( var.varType )
		{
			case RUL_CHARPTR:
				str2 = AStr2TStr(var.val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str2 = var.val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str2 = (wchar_t *)*var.val.prgsa;
				break;
		}
		if( wcscmp(str1.c_str(), str2.c_str()) <= 0 )
		{
			retValue = (bool)true;
		}
		else
		{
			retValue = (bool)false;
		}
	}
	// Walt EPM - 05sep08 - end
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator>=(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;

	if( isNumeric() && temp.isNumeric() )
	{
		if( (varType == RUL_DOUBLE) || (var.varType == RUL_DOUBLE) )
		{
			retValue = (bool)((double)*this >= (double)temp)?true:false;
		}
		else if( (varType == RUL_FLOAT) || (var.varType == RUL_FLOAT) )
		{
			retValue = (bool)((float)*this >= (float)temp)?true:false;
		}
		else if( (varType == RUL_ULONGLONG) || (var.varType == RUL_ULONGLONG) )
		{
			retValue = (bool)((unsigned __int64)*this >= (unsigned __int64)temp)?true:false;
		}
		else if( (varType == RUL_LONGLONG) || (var.varType == RUL_LONGLONG) )
		{
			retValue = (bool)((__int64)*this >= (__int64)temp)?true:false;
		}
		else if( (varType == RUL_UINT) || (var.varType == RUL_UINT) )
		{
			retValue = (bool)((unsigned int)*this >= (unsigned int)temp)?true:false;
		}
		else if( (varType == RUL_INT) || (var.varType == RUL_INT) )
		{
			retValue = (bool)((int)*this >= (int)temp)?true:false;
		}
		else if( (varType == RUL_USHORT) || (var.varType == RUL_USHORT) )
		{
			retValue = (bool)((wchar_t)*this >= (wchar_t)temp)?true:false;
		}
		else if( (varType == RUL_SHORT) || (var.varType == RUL_SHORT) )
		{
			retValue = (bool)((short)*this >= (short)temp)?true:false;
		}
		else if( (varType == RUL_UNSIGNED_CHAR) || (var.varType == RUL_UNSIGNED_CHAR) )
		{
			retValue = (bool)((unsigned char)*this >= (unsigned char)temp)?true:false;
		}
		else if( (varType == RUL_CHAR) || (var.varType == RUL_CHAR) )
		{
			retValue = (bool)((char)*this >= (char)temp)?true:false;
		}
	}
	// Walt EPM - 05sep08 - add
	else //string compares
	{
		wstring str1;
		wstring str2;
		switch( varType )
		{
			case RUL_CHARPTR:
				str1 = AStr2TStr(val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str1 = val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str1 = (wchar_t *)*val.prgsa;
				break;
		}
		switch( var.varType )
		{
			case RUL_CHARPTR:
				str2 = AStr2TStr(var.val.pzcVal);
				break;
			case RUL_WIDECHARPTR:
			case RUL_DD_STRING:
				str2 = var.val.pszValue;
				break;
			case RUL_SAFEARRAY:
				str2 = (wchar_t *)*var.val.prgsa;
				break;
		}
		if( wcscmp(str1.c_str(), str2.c_str()) >= 0 )
		{
			retValue = (bool)true;
		}
		else
		{
			retValue = (bool)false;
		}
	}
	// Walt EPM - 05sep08 - end
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator&&(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;
	
	if( isNumeric() && temp.isNumeric() )
	{
		retValue = (bool)*this && (bool)temp;
	}
	return retValue;
	
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator||(const INTER_VARIANT& var)
{
	INTER_VARIANT retValue;
	INTER_VARIANT temp = var;
	
	if( isNumeric() && temp.isNumeric() )
	{
		retValue = (bool)*this || (bool)temp;
	}
	
	return retValue;
}

//WHS EP June17-2008 have changed this to make sure that it works for all data types
const INTER_VARIANT INTER_VARIANT::operator!()
{
	INTER_VARIANT retValue;
	
	if( isNumeric())
	{
		retValue = !((bool)*this);
	}
	
	return retValue;
}







const INTER_VARIANT& INTER_VARIANT::operator=(const CValueVarient& src)
{	
	CLEAR_DATA( CvarientType2VARIENTtype(src) );
	
	CValueVarient locCV = src;// we have a non const copy that will self destruct on exit

	if ( locCV.isNumeric() )		//	rvalue type
	{
		switch(varType) // our type (lvalue type)
		{
		case RUL_BOOL:			val.bValue = (bool)locCV;			break;
		case RUL_CHAR:			val.cValue = (char)locCV;			break;
		case RUL_UNSIGNED_CHAR:	val.ucValue= (unsigned char)locCV;	break;
		case RUL_SHORT:			val.sValue = (short)locCV;			break;
		case RUL_USHORT:		val.usValue= (unsigned short int)locCV;	break;
		case RUL_INT:			val.nValue = (int)locCV;			break;
		case RUL_UINT:			val.unValue= (unsigned int)locCV;	break;
		case RUL_LONGLONG:		val.lValue = (__int64)locCV;		break;
		case RUL_ULONGLONG:		val.ulValue= (unsigned __int64)locCV;break;
		case RUL_FLOAT:			val.fValue = (float)locCV;			break;
		case RUL_DOUBLE:		val.dValue = (double)locCV;			break;
		default:
			// error: attempted numeric to string conversion
			break;
		}// end numeric switch
	}
	else
	{// NOT numeric
		switch(varType) // lvalue
		{
		case RUL_CHARPTR:	
			{			
				string locStr = (string)locCV;
				val.pzcVal    = new char[locStr.size()+1]; 
				memcpy(val.pzcVal,locStr.c_str(),(locStr.size()+1) );
			}
			break;
		case RUL_WIDECHARPTR:
		case RUL_DD_STRING:		
			{			
				wstring locStr = (wstring)locCV;
				val.pszValue   = new tchar[locStr.size()+1]; 
				memcpy(val.pszValue,locStr.c_str(),(locStr.size()+1)*sizeof(tchar));
			}
			break;

		case RUL_SAFEARRAY:
			{
				if (locCV.vType == CValueVarient::isString )
				{
					if ( val.prgsa == NULL )
						val.prgsa = new INTER_SAFEARRAY( (char*) locCV.sStringVal.c_str() );
					else
						*(val.prgsa) = (char*) locCV.sStringVal.c_str();
				}
				else
				if (locCV.vType == CValueVarient::isWideString )
				{
					if ( val.prgsa == NULL )
						val.prgsa = new INTER_SAFEARRAY( (wchar_t*) locCV.sWideStringVal.c_str());
					else
						*(val.prgsa) = (wchar_t*) locCV.sWideStringVal.c_str();
				}
				// else -  error - not numeric and not string....
			}
			break;
		case RUL_BYTE_STRING:	
			break;//  string to BYTE array NOT supported!!!!
		case RUL_NULL:
		default:	/* throw an error this is a non-numeric r value to numeric lvalue*/
			break;
		}// end switch
	}
	return *this;
}


// stevev 14aug07 convert inter-variant to a hcVarient
// returns true on error
bool inter2hcVARIANT(CValueVarient& destvarient, INTER_VARIANT& srcvariant)
{
	__VAL lV = srcvariant.GetValue();
	destvarient.clear();
	destvarient.vIsValid = true;// until proven otherwise

	switch(srcvariant.GetVarType())
	{
	case RUL_BOOL:
		destvarient.vValue.bIsTrue = lV.bValue;				//bool			bValue;
		destvarient.vType          = CValueVarient::isBool;
		break;
	case RUL_CHAR:
		destvarient.vValue.iIntConst = lV.cValue;				//_CHAR			cValue;
		destvarient.vType            = CValueVarient::isIntConst;
		destvarient.vSize            = sizeof(_CHAR);
		destvarient.vIsUnsigned      = false;
		break;
	case RUL_UNSIGNED_CHAR:
		destvarient.vValue.iIntConst = lV.ucValue;				//_CHAR			cValue;
		destvarient.vType            = CValueVarient::isIntConst;
		destvarient.vSize            = sizeof(_CHAR);
		destvarient.vIsUnsigned      = true;
		break;
	case RUL_INT:
		destvarient.vValue.iIntConst = lV.nValue;				//_INT32			nValue;
		destvarient.vType            = CValueVarient::isIntConst;
		destvarient.vSize            = sizeof(_INT32);
		destvarient.vIsUnsigned      = false;
		break;
	case RUL_UINT:
		destvarient.vValue.iIntConst = lV.unValue;
		destvarient.vType            = CValueVarient::isIntConst;
		destvarient.vSize            = sizeof(_UINT32);
		destvarient.vIsUnsigned      = true;
		break;		
	case RUL_SHORT:
		destvarient.vValue.iIntConst = lV.sValue;
		destvarient.vType            = CValueVarient::isIntConst;
		destvarient.vSize            = sizeof(_INT16);
		destvarient.vIsUnsigned      = false;
		break;
	case RUL_USHORT:
		destvarient.vValue.iIntConst = lV.usValue;
		destvarient.vType            = CValueVarient::isIntConst;
		destvarient.vSize            = sizeof(_UINT16);
		destvarient.vIsUnsigned      = true;
		break;
	case RUL_LONGLONG:
		destvarient.vValue.longlongVal = lV.lValue;
		destvarient.vType            = CValueVarient::isVeryLong;
		destvarient.vSize            = sizeof(_INT64);
		destvarient.vIsUnsigned      = false;
		break;
	case RUL_ULONGLONG:
		destvarient.vValue.longlongVal = lV.ulValue;
		destvarient.vType            = CValueVarient::isVeryLong;
		destvarient.vSize            = sizeof(_UINT64);
		destvarient.vIsUnsigned      = true;
		break;

	case RUL_FLOAT:
		destvarient.vValue.fFloatConst = lV.fValue;				//_FLOAT			fValue;
		destvarient.vType            = CValueVarient::isFloatConst;
		destvarient.vSize            = sizeof(_FLOAT);
		destvarient.vIsUnsigned      = false;
		destvarient.vIsDouble        = false;
		break;
	case RUL_DOUBLE:
		destvarient.vValue.fFloatConst = lV.dValue;				//_DOUBLE			dValue;
		destvarient.vType            = CValueVarient::isFloatConst;
		destvarient.vSize            = sizeof(_DOUBLE);
		destvarient.vIsUnsigned      = false;
		destvarient.vIsDouble        = true;
		break;

	case RUL_CHARPTR:
		if( lV.pzcVal )
		{
			destvarient.sStringVal		 = lV.pzcVal;				
		}
		else
		{
			destvarient.sStringVal = "";
		}
		destvarient.vType            = CValueVarient::isString;
		destvarient.vSize            = destvarient.sStringVal.size();
		break;

	case RUL_WIDECHARPTR:
	case RUL_DD_STRING:
		if( lV.pszValue )
		{
			destvarient.sWideStringVal   = lV.pszValue;					
		}
		else
		{
			destvarient.sWideStringVal   = _T("");					
		}
		destvarient.vType            = CValueVarient::isWideString;
		destvarient.vSize            = destvarient.sWideStringVal.size();
		break;

	case RUL_SAFEARRAY:
	{// others treat SAFEARRAY like a char array, Do that if it is a char array
		INTER_SAFEARRAY *sa = srcvariant.GetSafeArray();
		int idims      = sa->GetDims(NULL);
		int elemSz     = sa->GetElementSize();

		if (idims == 1)
		{
			int saLen = sa->GetNumberOfElements();
			if ( saLen > MAX_DD_STRING )
			{
				saLen = MAX_DD_STRING;
			}
			INTER_VARIANT elemValue;
			if (sa->Type() == RUL_CHAR || sa->Type() == RUL_UNSIGNED_CHAR)
			{
				char pchString[MAX_DD_STRING]={0};
				int i=0;
				int index=0;
				for (i = 0,index = 0; index < saLen;  i+=elemSz,index++)
				{
					sa->GetElement(i, &elemValue);
					pchString[index] = (char)elemValue;
					elemValue.Clear();
				}// next element
				pchString[index] = 0;

				destvarient.sStringVal       = pchString;
				destvarient.vType            = CValueVarient::isString;
				destvarient.vSize            = destvarient.sStringVal.size();
			}
			else
			if (sa->Type() == RUL_USHORT || sa->Type() == RUL_SHORT)
			{
				wchar_t pchString[MAX_DD_STRING]={0};
				int i=0;
				int index=0;
				for (i = 0,index = 0; index < saLen;  i+=elemSz,index++)
				{
					sa->GetElement(i, &elemValue);
					pchString[index] = (wchar_t)elemValue;
					elemValue.Clear();
				}// next element
				pchString[index] = 0;

				destvarient.sWideStringVal   = pchString;
				destvarient.vType            = CValueVarient::isWideString;
				destvarient.vSize            = destvarient.sWideStringVal.size();
			}
			// else arrays of other stuff can't convert to a (w)string
		}		
		else //  idims != 1 --- can't handle
		{
			destvarient.vIsValid = false;
			return true;			// unknown/illegal variant type
		}
	}
	break;

	case RUL_BYTE_STRING:
		break;

	case RUL_NULL:
	default:
		destvarient.vIsValid = false;
		return true;			// unknown/illegal variant type
		break;
	}
	return false;// no error

}


int INTER_VARIANT::narrowStr2number(__VAL& retVal, VARIANT_TYPE& retType,const _CHAR* pStr)
{
	
	_INT32 nLen  = 0;
	_INT32 nVal  = 0;
	_INT64 lVal  = 0;
	_UINT64 ulVal= 0;
	_INT32 i     = 0;
	_DOUBLE fVal = 0;
	_CHAR* pEnd = NULL;
	bool bIsFloat = false;
	bool bIsHex   = false, bIsOctal = false, bIsNeg = false;
	int iBase = 10;

	if (pStr == NULL || (nLen = strlen(pStr)) == 0 )
	{
		return -1; // failure
	}

	/************************* reworked by stevev 10oct05 *********************************/
	if (nLen >= 2)
	{
		bIsNeg = (pStr[0] == '-');
		if (pStr[0] == '0')
		{// has to be octal || hex
			if ((pStr[1] == 'x') || (pStr[1] == 'X'))
			{
				bIsHex = true;
				iBase = 16;
#ifdef MODIFIED_BY_SS
				//sscanf(pStr,ULLXFMT,&ulVal);
				std::istringstream iss(pStr);
				iss >> ulVal;
#endif
			}
			else
			if ( pStr[1] >= '0' && pStr[1] < '8' )
			{// octal
				bIsOctal = true;
				iBase = 8;
				
#ifdef MODIFIED_BY_SS
				//sscanf(pStr,ULLOFMT,&ulVal);
				std::istringstream iss(pStr);
				iss >> ulVal;
#endif
			}
			else
			{// float or decimal - actually an error....
				for(i=1;i<nLen;i++)// we'll try to recover via float
				{
					if (pStr[i] == '.' || pStr[i] == 'E' || pStr[i] == 'e')
					{
						bIsFloat = true;
						break;
					}
				}
				if ( ! bIsFloat )
				{// if all still false then its decimal eg 0999 - actually an error
					// throw error
					return -2;
				}
			}//end else			
		}
		else
		{// starts with a non-zero [1-9+\-]
		    // Walt EPM - 17oct08- make '.025' work as well as '0.025'
			for(i=0;i<nLen;i++)// we'll try to recover via float/
			{
				if (pStr[i] == '.' || pStr[i] == 'E' || pStr[i] == 'e')
				{
					bIsFloat = true;
					break;
				}
			}
		}// endelse a decimal/float
	}
	// else: length == 1, can't be octal or hex or float, must be decimal...process as such

	if ( bIsFloat )
	{
		//retVal.fValue = (float)atof(pStr);
		retVal.dValue = strtod(pStr,&pEnd);
		if ( (retVal.dValue <=  FLT_MAX   && retVal.dValue >= FLT_MIN)     ||
			 (retVal.dValue > (- FLT_MAX) && retVal.dValue < (- FLT_MIN) ) )
		{
			fVal = retVal.dValue;
			retVal.fValue = (float)fVal;
			retType = RUL_FLOAT;
		}
		else
		{
			retType = RUL_DOUBLE;
		}
	}
	else
	{// decimal is the only option left
	 //		we just gotta figure out how big
		if ( (! bIsHex ) && (! bIsOctal ) )
		{// we haven't scanned it yet, do it now
			iBase = 10;
			if ( bIsNeg )
			{
				//sscanf(pStr,ULLDFMT,&lVal);
				std::istringstream iss(pStr);
				iss >> lVal;
			}
			else// go unsigned until proven otherwise
			{
				std::istringstream iss(pStr);
				iss >> ulVal;
				//sscanf(pStr,ULLUFMT,&ulVal);
			}
		}

		if (bIsNeg)
		{
				retVal.lValue = lVal;
				retType       = RUL_LONGLONG;
		}
		else
		{// we are non-negative
			if (ulVal <= _LL_MAX)//WHS EP June17-2008 dont constrain constants to short/char/int - default to natural size.
			{
				retVal.lValue = (__int64)ulVal;
				retType       = RUL_LONGLONG;
			}
			else
			{
				retVal.ulValue = ulVal;
				retType        = RUL_ULONGLONG;
			}
		}
	}
	
	return 0; // SUCCESS
}

enum VARIANT_TYPE CvarientType2VARIENTtype(const CValueVarient& srcVar)
{
	enum VARIANT_TYPE retVal = RUL_NULL;

	switch (srcVar.vType)
	{
	case CValueVarient::isBool:
		{	
			retVal = RUL_BOOL;
		}
		break;
	case CValueVarient::isIntConst:
		{
			if (srcVar.vSize == 1 )
			{
				if (srcVar.vIsUnsigned)
					retVal = RUL_UNSIGNED_CHAR;
				else
					retVal = RUL_CHAR;
			}
			else
			if (srcVar.vSize == 2 )
			{
				if (srcVar.vIsUnsigned)
					retVal = RUL_USHORT;
				else
					retVal = RUL_SHORT;
			}
			else // must be three or four
			{
				if (srcVar.vIsUnsigned)
					retVal = RUL_UINT;
				else
					retVal = RUL_INT;
			}
		}
		break;
	case CValueVarient::isFloatConst:
		{
			if (srcVar.vIsDouble)
				retVal = RUL_DOUBLE;
			else
				retVal = RUL_FLOAT;
		}
		break;
	case CValueVarient::isVeryLong:
		{
			if (srcVar.vIsUnsigned)
				retVal = RUL_ULONGLONG;
			else
				retVal = RUL_LONGLONG;
		}
		break;
	case CValueVarient::isString:
		{
			retVal = RUL_CHARPTR;
		}
		break;
	case CValueVarient::isWideString:
		{
			retVal = RUL_WIDECHARPTR;
		}
		break;

	case CValueVarient::isOpcode:
	case CValueVarient::isDepIndex:
	case CValueVarient::isSymID:
	case CValueVarient::invalid:
	default:
		retVal = RUL_NULL;
		break;
	// UN-SUPPORTED:  RUL_DD_STRING, RUL_BYTE_STRING, RUL_SAFEARRAY
	}// end switch src type

	return retVal;
}

/*static*/
void INTER_VARIANT::StripLangCode(wstring& szString, wchar_t* szLangCode, bool* bLangCodePresent)
{
	if (bLangCodePresent)
	{
		*bLangCodePresent =false;
	}
	if( szString.length() > 3 )// emerson checkin april2013
	{
		if( (szString[0] == _T('|')) &&  (szString[3] == _T('|')) )
		{
			if (bLangCodePresent)
			{
				*bLangCodePresent = true;
			}
			if (szLangCode)
			{
#ifdef MODIFIED_BY_SS
				//wcsncpy(szLangCode, szString.substr(1,2).c_str(),2);
				wcsncpy_s(szLangCode, 2, szString.substr(1, 2).c_str(),_TRUNCATE);
#endif
				szLangCode[2] = 0;
			}
			szString = szString.substr(4);
		}
	}
	return;
}
/*static*/
void INTER_VARIANT::StripLangCode(wchar_t* szString, wchar_t* szLangCode, bool* bLangCodePresent)
{
	if ( szString )
	{
		int y = wcslen(szString);
		wstring lstr(szString);
		StripLangCode(lstr,szLangCode,bLangCodePresent);
		wcsncpy_s(szString, y, lstr.c_str(),_TRUNCATE);
		szString[lstr.size()] = 0;
	}
	return;
}