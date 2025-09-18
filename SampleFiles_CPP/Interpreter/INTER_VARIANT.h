/*************************************************************************************************
 *
 * INTER_VARIANT.h
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
 *		feb08 reworked by stevev
 *
 * #include "INTER_VARIANT.h"
 */

#if !defined(AFX_INTER_VARIANT_H__C80BD8E3_AFE7_46BD_BF40_586F8EFDE642__INCLUDED_)
#define AFX_INTER_VARIANT_H__C80BD8E3_AFE7_46BD_BF40_586F8EFDE642__INCLUDED_


#ifdef INC_DEBUG
#pragma message("In INTER_VARIANT.h") 
#endif

#include "typedefs.h"
#include "ParserDeclarations.h"
#include "SafeVar.h"
#include "varient.h" // stevev 14aug07 - to get conversion                                       *


#ifdef INC_DEBUG
#pragma message("    Finished Includes::INTER_VARIANT.h") 
#endif



class INTER_SAFEARRAY;

class INTER_VARIANT
{
public:
	//constructors 
	INTER_VARIANT();
	INTER_VARIANT( const INTER_VARIANT& variant );
	INTER_VARIANT( bool bIsNumber, const _CHAR* szNumber );
	INTER_VARIANT( bool bIsNumber, const wchar_t* szNumber );
	INTER_VARIANT( void* pmem, VARIANT_TYPE vt );

	~INTER_VARIANT();
	
//Type Casts
	operator bool(void);
	operator char(void); //Vibhor 110205: Added
	// no cast to string allowed at this time!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	operator char *(void);						//   assume string
//	operator wchar_t *(void);						//   assume string
//	operator _BYTE_STRING(void); // stevev 1feb08 - embedded nulls, length is part of it
	operator unsigned char(void); // stevev 1feb08
	// 1feb08  stop using...cast to char as required for string  
	operator int(void);
	operator unsigned int(void); // stevev 1feb08
	operator short(void);   // stevev 1feb08
	operator wchar_t(void); // stevev 20dec07 added - unsigned short
	operator __int64(void); // stevev 1feb08
	operator unsigned __int64(void); // stevev 1feb08
	operator float(void);
	operator long(void);
	operator double(void);//Vibhor 110205: Added

//Assignment Operators
	const INTER_VARIANT& operator=(bool b);
	const INTER_VARIANT& operator=(_CHAR c);				// assume wide string
	const INTER_VARIANT& operator=(_SHORT  s);  // stevev 1feb08
	const INTER_VARIANT& operator=(int n);   // from 'int' - WIN32 and ANDROID have 32 bit ints
	const INTER_VARIANT& operator=(_INT64  ll); // stevev 1feb08
//jad attempted arm64 fix
	//jad arm64 debug
	//const INTER_VARIANT& operator=(__int64 ll);
	
	const INTER_VARIANT& operator=(_UCHAR  uc); // stevev 1feb08
	const INTER_VARIANT& operator=(wchar_t  t); // aka _USHORT
	const INTER_VARIANT& operator=(unsigned int un); // stevev 1feb08
	const INTER_VARIANT& operator=(_UINT64 ull);// stevev 1feb08

	//jad arm64 debug
	//jad attempted arm64 fix
	//const INTER_VARIANT& operator=(unsigned __int64 ull);

	const INTER_VARIANT& operator=(_CHAR* psz);					   // assume string
	// 1feb08  stop using...cast to char as required for string  or use _BYTE_STRING
	// const INTER_VARIANT& operator=(_UCHAR* psz);// stevev 27dec07  // assume string	
	const INTER_VARIANT& operator=(wchar_t* t);
	const INTER_VARIANT& operator=(_FLOAT f);
	const INTER_VARIANT& operator=(_DOUBLE d);
	const INTER_VARIANT& operator=(long d);
#ifdef MODIFIED_BY_SS
	const INTER_VARIANT& operator=(_UINT32 d);
	//const INTER_VARIANT& operator=(char c);
	
#endif
	const INTER_VARIANT& operator=(VARIANT_TYPE type);
	const INTER_VARIANT& operator=(const INTER_VARIANT& var);
	const INTER_VARIANT& operator=( INTER_SAFEARRAY *sa );
	const INTER_VARIANT& operator=(const CValueVarient& src);
	//const INTER_VARIANT& operator=(INTER_VARIANT&);
	const INTER_VARIANT& operator=(_BYTE_STRING& bs);// stevev 1feb08

//Arithmetic, Logical and Relational Operators
	const INTER_VARIANT operator+(const INTER_VARIANT&);
	const INTER_VARIANT operator-(const INTER_VARIANT&);
	const INTER_VARIANT operator*(const INTER_VARIANT&);
	const INTER_VARIANT operator/(const INTER_VARIANT&);
	const INTER_VARIANT operator%(const INTER_VARIANT&);
	const INTER_VARIANT operator&(const INTER_VARIANT&);
	const INTER_VARIANT operator|(const INTER_VARIANT&);
	const INTER_VARIANT operator^(const INTER_VARIANT&);
	const INTER_VARIANT operator~();
	const INTER_VARIANT operator>>(const INTER_VARIANT&);
	const INTER_VARIANT operator<<(const INTER_VARIANT&);
	const INTER_VARIANT operator!=(const INTER_VARIANT&);
	const INTER_VARIANT operator<(const INTER_VARIANT&);
	const INTER_VARIANT operator>(const INTER_VARIANT&);
	const INTER_VARIANT operator==(const INTER_VARIANT&);
	const INTER_VARIANT operator>=(const INTER_VARIANT&);
	const INTER_VARIANT operator<=(const INTER_VARIANT&);
	const INTER_VARIANT operator&&(const INTER_VARIANT&);
	const INTER_VARIANT operator||(const INTER_VARIANT&);
	const INTER_VARIANT operator!();

	INTER_SAFEARRAY *GetSafeArray();


	_INT32	XMLize(
		_CHAR* szData);

	void Clear();// releases memory, sets to RUL_NULL

	void SetValue(
		void* pmem,
		VARIANT_TYPE vt);

	void GetValue(
		void* pmem,
		VARIANT_TYPE vt);

	static _INT32 VariantSize(
		VARIANT_TYPE vt);

	void GetStringValue(
		char** pmem,
		VARIANT_TYPE vt = RUL_CHARPTR);
	void GetStringValue(
		wchar_t** pmem,
		VARIANT_TYPE vt = RUL_WIDECHARPTR);

	wstring GetStringValue(void);



	// stevev 05jun07 - used to detect if promotion is possible
	inline  // wouldn't link from the .cpp??? don't know why
	bool isNumeric(void)
	{	switch(GetVarType()) {	case RUL_BOOL:	case RUL_CHAR:	case RUL_UNSIGNED_CHAR:
		case RUL_SHORT:		case RUL_USHORT:	case RUL_INT:	case RUL_UINT:
		case RUL_LONGLONG:	case RUL_ULONGLONG:	case RUL_FLOAT:	case RUL_DOUBLE:
			return true;		break;
		case RUL_CHARPTR:	case RUL_WIDECHARPTR:	case RUL_DD_STRING:	case RUL_BYTE_STRING:
		case RUL_SAFEARRAY:
			return false;		break;
		case RUL_NULL:		default:
		/* throw an error */
		return false;		break;	}
	return false;
	};

	VARIANT_TYPE	GetVarType(){ return varType; };

protected:
	__VAL val;
	VARIANT_TYPE	varType;
	string charout;// for conversion return value from wide to standard

	int narrowStr2number(__VAL& retVal, VARIANT_TYPE& retType, const _CHAR* pStr);
	static
	void StripLangCode(wstring& szString, wchar_t* szLangCode=NULL, bool* bLangCodePresent=NULL);
	static
	void StripLangCode(wchar_t* szString, wchar_t* szLangCode=NULL, bool* bLangCodePresent=NULL);


public:
	const __VAL& GetValue()
	{
		return val;
	};
};

bool inter2hcVARIANT(CValueVarient& destvarient, INTER_VARIANT& srcvariant);
enum VARIANT_TYPE CvarientType2VARIENTtype(const CValueVarient& srcVar);


#endif