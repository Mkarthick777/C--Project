//#include "stdafx.h"

//#if _MSC_VER >= 1300  // HOMZ - port to 2003, VS7
//stevev 20feb07-merge- contents moved to ddbGeneral #include "..\DevServices\stdafx.h" 
//#endif
/** NOTE: 29dec11 - from emerson partner: memset of a structure containing a class is BAD.
    all memsets removed from constructors today.  ctor in struct added to handle init.
	Causes crashes in MEE in VS2010.
**/
#include "pch.h"
#include <assert.h>
#include "ddbGeneral.h"	// stevev 20feb07 - merge to get rid of 'stdafx.h'
/* comutil.h uses Bill's TRUE/FALSE that the general erroneously defines on purpose */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

#include "INTER_SAFEARRAY.h"
#include "INTER_VARIANT.h"

int size_ofs[] = SIZE_OFS;

//////////////////////////////////////////////////
// Simple Constructor
/////////////////////////////////////////////////
INTER_SAFEARRAY::INTER_SAFEARRAY()
{
	m_wcharPtr = NULL;
	m_i32mem = 0;
}

//////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////
INTER_SAFEARRAY::INTER_SAFEARRAY( _CHAR* pszValue )
{
	m_wcharPtr = NULL;
	m_i32mem = strlen(pszValue)+1;
	m_data.pvData = new _UCHAR[m_i32mem];
	m_data.varType = RUL_CHAR;
	m_data.cbElements = 1;
	m_data.cDims = 1;
	memcpy( m_data.pvData, pszValue, m_i32mem );
}

INTER_SAFEARRAY::INTER_SAFEARRAY( _UCHAR* pszValue )
{
	m_wcharPtr = NULL;
	m_i32mem = strlen((_CHAR*)pszValue)+1;
	m_data.pvData = new _UCHAR[m_i32mem];
	m_data.varType = RUL_UNSIGNED_CHAR;
	m_data.cbElements = 1;
	m_data.cDims = 1;
	memcpy( m_data.pvData, pszValue, m_i32mem );
}

INTER_SAFEARRAY::INTER_SAFEARRAY( tchar* pszValue )
{
	m_wcharPtr = NULL;
	m_i32mem = _tstrlen(pszValue)+1;
	m_data.cbElements = sizeof(tchar);
	m_data.pvData = new _UCHAR[m_i32mem*m_data.cbElements];
	m_data.varType = RUL_USHORT;
	m_data.cDims = 1;
	memcpy( m_data.pvData, pszValue, m_i32mem*m_data.cbElements );
}


INTER_SAFEARRAY::INTER_SAFEARRAY( _BYTE_STRING* bsValue )
{
	m_wcharPtr = NULL;
	m_i32mem = bsValue->bsLen;	// actual size
	m_data.cbElements = sizeof(_UCHAR);
	m_data.pvData = new _UCHAR[m_i32mem + 1];// store an extra null top be kind
	m_data.varType = RUL_UNSIGNED_CHAR;
	m_data.cDims = 1;
	memcpy( m_data.pvData, bsValue->bs, m_i32mem );
	m_data.pvData[m_i32mem] = 0;// just in case...
}

//////////////////////////////////////////////////
// Copy Constructor
/////////////////////////////////////////////////
INTER_SAFEARRAY::INTER_SAFEARRAY( INTER_SAFEARRAY& safearray)
{
	m_wcharPtr = NULL;
	m_i32mem = 0;
	operator=(safearray);
}

//////////////////////////////////////////////////
// destructor
/////////////////////////////////////////////////
INTER_SAFEARRAY::~INTER_SAFEARRAY()
{
	if( m_wcharPtr )
	{
		delete m_wcharPtr;
		m_wcharPtr = NULL;
	}

	if( m_data.pvData != 0)
	{
		try		//TSRPRASAD 09MAR2004 Fix the memory leaks
		{
			delete []  m_data.pvData;
		m_data.pvData = 0;
		}
		catch(...)
		{
		}
	}
	m_data.vecBounds.clear();
}

//////////////////////////////////////////////////
// Equal Operators
/////////////////////////////////////////////////
INTER_SAFEARRAY& INTER_SAFEARRAY::operator=( INTER_SAFEARRAY& safearray )
{
	if( m_i32mem )
	{
		memset( m_data.pvData, 0, m_i32mem ); 
		int nMinimumDataSize = safearray.m_i32mem;
		if( nMinimumDataSize > m_i32mem )
		{
			nMinimumDataSize = m_i32mem;
		}
		memcpy( m_data.pvData, safearray.m_data.pvData, nMinimumDataSize );
		m_data.cDims = safearray.GetDims(NULL);
	}
	else
	{
		m_data.varType = safearray.m_data.varType;
		m_data.cbElements = (_USHORT)safearray.GetElementSize();
		// use calculation below....m_i32mem = safearray.MemoryAllocated();
		m_data.cDims = safearray.GetDims(NULL);
	
		//now look at the vector bounds.
		m_data.vecBounds.clear();
		int vecBoundsSize = safearray.m_data.vecBounds.size();
		assert(vecBoundsSize == m_data.cDims);
		for( int i=0; i<vecBoundsSize; i++ )
		{
			m_data.vecBounds.push_back(safearray.m_data.vecBounds[i]);
		}

		Allocate();

#ifdef _DEBUG
  #ifndef _CONSOLE  /* not in Varient Test */
		if (m_i32mem != safearray.MemoryAllocated())
		{// log an error
			LOGIT(CERR_LOG|CLOG_LOG,"ERROR: MEE 'safearray equals safearray' size mismatch. (debug only message)\n"
									"       MEE this  always occurs on array declarations.\n");
		}
  #endif
#endif

		if (m_data.pvData != NULL && safearray.m_data.pvData != NULL && m_i32mem>0)
			memcpy( m_data.pvData, safearray.m_data.pvData, m_i32mem );
	
	}

	return *this;
}

INTER_SAFEARRAY& INTER_SAFEARRAY::operator=( char*    pSrc)
{
	if (pSrc == NULL) return *this;					/* throw a failure - parameter empty */
	if( m_i32mem ) // size set and memory allocated
	{
		if (m_data.cDims != 1) return *this;		/* throw a failure - dimensional mismatch*/ 
		memset( m_data.pvData, 0, m_i32mem );
		int nMinimumDataSize = strlen(pSrc);
		if( nMinimumDataSize > m_i32mem )
		{
			nMinimumDataSize = m_i32mem;
		}

		if ( m_data.varType == RUL_CHAR )	// c-string to c-string
		{
			memcpy( m_data.pvData, pSrc, nMinimumDataSize );
			m_data.pvData[m_i32mem-1] = '\0';
		}
		else
		if ( m_data.varType == RUL_UNSIGNED_CHAR)	// c-string to byte-string
		{
			memcpy( m_data.pvData, pSrc, nMinimumDataSize );
			m_data.pvData[m_i32mem-1] = '\0';
		}
		else
		if ( m_data.varType == RUL_USHORT )//c_string to wide-string
		{// convert
			string  srcStr(pSrc);
			wstring dstStr;
			dstStr = AStr2TStr(srcStr);
			if ( (int)(dstStr.size() * sizeof(wchar_t)) < nMinimumDataSize )
			{
				 nMinimumDataSize = dstStr.size() * sizeof(wchar_t);
			}
			memcpy( m_data.pvData, dstStr.c_str(), nMinimumDataSize);

		}
		// else - unsupported string type or  multi dimension array 
	}
	else
	if (m_data.varType == RUL_NULL )
	{// alloc and copy
		int srcLen = strlen(pSrc);

		m_data.varType = RUL_CHAR;
		m_data.cbElements = sizeof(char);
		m_data.cDims      = 1;
		if( m_data.pvData != 0)//double check that we do not leak memory
		{
			delete []  m_data.pvData;
			m_data.pvData = 0;
			m_i32mem = 0;
		}
			
		m_i32mem = srcLen + 1;

		m_data.pvData = new _UCHAR[m_i32mem];
		memset( m_data.pvData,0,m_i32mem);
		memcpy( m_data.pvData, pSrc, m_i32mem );

		//now look at the vector bounds.
		INTER_SAFEARRAYBOUND lVB;
		lVB.cElements = m_i32mem;
		m_data.vecBounds.clear();

		m_data.vecBounds.push_back(lVB);

	}
	else
		return *this; /* throw a failure */
	

	return *this;
}

INTER_SAFEARRAY& INTER_SAFEARRAY::operator=( wchar_t* pSrc)
{
	if (pSrc == NULL) return *this;					/* throw a failure - parameter empty */
	int nMinimumDataSize = wcslen(pSrc) * sizeof(wchar_t);

	if( m_i32mem ) // size set and memory allocated
	{
		if (m_data.cDims != 1) return *this;		/* throw a failure - dimensional mismatch*/ 
		memset( m_data.pvData, 0, m_i32mem );
		if( nMinimumDataSize > m_i32mem )
		{
			nMinimumDataSize = m_i32mem;
		}

		if ( m_data.varType == RUL_USHORT )	// wide-string to wide-string
		{
			memcpy( m_data.pvData, pSrc, nMinimumDataSize );
			m_data.pvData[m_i32mem-2] =  '\0';m_data.pvData[m_i32mem-1] =  '\0';
		}
		else
		if ( m_data.varType == RUL_UNSIGNED_CHAR)	// wide-string to byte-string
		{
			// convert
			wstring srcStr(pSrc);
			 string dstStr;
			dstStr = TStr2AStr(srcStr);
			if ( (int)(dstStr.size() * sizeof(char)) < nMinimumDataSize )
			{
				 nMinimumDataSize = dstStr.size() * sizeof(char);
			}
			memcpy( m_data.pvData, dstStr.c_str(), nMinimumDataSize);
			m_data.pvData[m_i32mem-1] =  '\0';
		}
		else
		if ( m_data.varType == RUL_CHAR )// wide-string to c_string
		{// convert
			wstring srcStr(pSrc);
			 string dstStr;
			dstStr = TStr2AStr(srcStr);
			if ( (int)(dstStr.size() * sizeof(char)) < nMinimumDataSize )
			{
				 nMinimumDataSize = dstStr.size() * sizeof(char);
			}
			memcpy( m_data.pvData, dstStr.c_str(), nMinimumDataSize);
			m_data.pvData[m_i32mem-1] =  '\0';
		}
		// else - unsupported string type or  multi dimension array 
	}
	else
	if (m_data.varType == RUL_NULL )
	{// alloc and copy
		int srcLen = nMinimumDataSize;// number of bytes wo/null

		m_data.varType    = RUL_USHORT;
		m_data.cbElements = sizeof(wchar_t);
		m_data.cDims      = 1;
		if( m_data.pvData != 0)//double check that we do not leak memory
		{
			delete []  m_data.pvData;
			m_data.pvData = 0;
			m_i32mem = 0;
		}
			
		m_i32mem = srcLen + sizeof(wchar_t);

		m_data.pvData = new _UCHAR[m_i32mem];
		memset( m_data.pvData,0,m_i32mem);
		memcpy( m_data.pvData, pSrc, srcLen );

		//now look at the vector bounds.
		INTER_SAFEARRAYBOUND lVB;
		lVB.cElements = m_i32mem;
		m_data.vecBounds.clear();

		m_data.vecBounds.push_back(lVB);
	}
	else
		return *this; /* throw a failure */
	
	return *this;
}

INTER_SAFEARRAY& INTER_SAFEARRAY::operator=( _BYTE_STRING& src)
{	
	if( m_i32mem ) // size set and memory allocated
	{
		if (m_data.cDims != 1) return *this;		/* throw a failure - dimensional mismatch*/ 
		memset( m_data.pvData, 0, m_i32mem );
		int nMinimumDataSize = src.bsLen;
		if( nMinimumDataSize > m_i32mem )
		{
			nMinimumDataSize = m_i32mem;
		}
		int i;

		if ( m_data.varType == RUL_BOOL )
		{
			bool*  pB = (bool*)m_data.pvData;
			bool*  pF = (bool*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( src.bs[i] ) ? true : false;
			}
		}
		else
		if ( m_data.varType == RUL_CHAR )
		{
			_CHAR*  pB = (_CHAR*)m_data.pvData;
			_CHAR*  pF = (_CHAR*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _CHAR )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_INT )
		{
			_INT32*  pB = (_INT32*)m_data.pvData;
			_INT32*  pF = (_INT32*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _INT32 )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_UINT )
		{
			_UINT32*  pB = (_UINT32*)m_data.pvData;
			_UINT32*  pF = (_UINT32*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _UINT32 )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_SHORT )
		{
			_INT16*  pB = (_INT16*)m_data.pvData;
			_INT16*  pF = (_INT16*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _INT16 )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_USHORT )
		{
			_UINT16*  pB = (_UINT16*)m_data.pvData;
			_UINT16*  pF = (_UINT16*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _UINT16 )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_LONGLONG )
		{
			_INT64*  pB = (_INT64*)m_data.pvData;
			_INT64*  pF = (_INT64*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _INT64 )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_ULONGLONG )
		{
			_UINT64*  pB = (_UINT64*)m_data.pvData;
			_UINT64*  pF = (_UINT64*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _UINT64 )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_FLOAT )
		{
			_FLOAT*  pB = (_FLOAT*)m_data.pvData;
			_FLOAT*  pF = (_FLOAT*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _FLOAT )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_DOUBLE )
		{
			_DOUBLE*  pB = (_DOUBLE*)m_data.pvData;
			_DOUBLE*  pF = (_DOUBLE*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _DOUBLE )src.bs[i];
			}
		}
		else
		if ( m_data.varType == RUL_UNSIGNED_CHAR  )
		{
			_UCHAR*  pB = (_UCHAR*)m_data.pvData;
			_UCHAR*  pF = (_UCHAR*)(m_data.pvData+nMinimumDataSize);
			for(i = 0 ; pB < pF; i++, pB++)
			{	*pB = ( _UCHAR )src.bs[i];
			}
		}
		// else - unsupported string type or  multi dimension array 
	}
	else
	if (m_data.varType == RUL_NULL )
	{// alloc and copy
		int srcLen = src.bsLen;

		m_data.varType    = RUL_UNSIGNED_CHAR;
		m_data.cbElements = sizeof(_UCHAR);
		m_data.cDims      = 1;
		if( m_data.pvData != 0)//double check that we do not leak memory
		{
			delete []  m_data.pvData;
			m_data.pvData = 0;
			m_i32mem = 0;
		}
			
		m_i32mem = srcLen + sizeof(_UCHAR);

		m_data.pvData = new _UCHAR[m_i32mem];
		memset( m_data.pvData,0,m_i32mem);
		memcpy( m_data.pvData, &src, srcLen );

		//now look at the vector bounds.
		INTER_SAFEARRAYBOUND lVB;
		lVB.cElements = srcLen;
		m_data.vecBounds.clear();

		m_data.vecBounds.push_back(lVB);
	}
	else
		/* we only support operator= to same type or type null */
		return *this; /* throw a failure */
	
	return *this;
}


INTER_SAFEARRAY::operator _CHAR *(void)
{//   assume string - w/conversion (wide 2 narrow)
	char* pChar = NULL;
assert ( 1 == 0 );
	return pChar;
}
INTER_SAFEARRAY::operator wchar_t *(void)
{//   assume string - w/conversion (narrow 2 wide)
	if( m_wcharPtr )
	{
		delete m_wcharPtr;
		m_wcharPtr = NULL;
	}

	if( (m_data.varType == RUL_CHAR) || (m_data.varType == RUL_UNSIGNED_CHAR) )
	{
		wstring str = AStr2TStr( (char *)m_data.pvData );
		 m_wcharPtr = new wchar_t[wcslen(str.c_str())+1];
		//_tstrcpy( m_wcharPtr, str.c_str() );
		// std::wstring strTemp(t.c_str());
		 std::copy(str.begin(), str.end(), m_wcharPtr);
	}
	else if( m_data.varType == RUL_WIDECHARPTR )
	{
		return (wchar_t *)m_data.pvData;
	}
	return m_wcharPtr;
}



void INTER_SAFEARRAY::SetAllocationParameters(VARIANT_TYPE vt,_USHORT cDims,
																INTER_SAFEARRAYBOUND* prgsaBound)
{
	m_data.cDims = cDims;
	m_data.cbElements = (_USHORT)INTER_VARIANT::VariantSize(vt);
	m_data.varType = vt;
	m_data.vecBounds.push_back(prgsaBound[0]);
}
void INTER_SAFEARRAY::SetAllocationParameters(VARIANT_TYPE vt,_USHORT cDims,_INT32 prgsaBound)
{
	INTER_SAFEARRAYBOUND local[1] = { prgsaBound };
	SetAllocationParameters(vt,cDims, local);
}

void INTER_SAFEARRAY::AddDim(INTER_SAFEARRAYBOUND* prgsaBound)
{
	m_data.cDims++;
	m_data.vecBounds.push_back(prgsaBound[0]);
	//throw error if prgsaBound[0].cElements <=0;
}

void INTER_SAFEARRAY::AddDim(_INT32 prgsaBound)
{
	INTER_SAFEARRAYBOUND local[1] = { prgsaBound };
	AddDim(local);
}

_USHORT INTER_SAFEARRAY::GetDims(vector<_INT32>* pvecDims)
{
	if(pvecDims)
	{
		_INT32 i32Size = m_data.vecBounds.size();
		for(_INT32 i=0; i<i32Size; i++)
		{
			pvecDims->push_back(m_data.vecBounds[i].cElements);
		}
	}
	return m_data.cDims;
}


_INT32	INTER_SAFEARRAY::MemoryAllocated()
{
	return m_i32mem;
}


_INT32	INTER_SAFEARRAY::Allocate()
{
	_INT32 i32Size = m_data.vecBounds.size();
	if( (m_data.cDims > 0) && (i32Size > 0) )
	{
		//Walt:EPM-24aug07
		if( m_data.pvData != 0)//double check that we do not leak memory
		{
			delete []  m_data.pvData;
			m_data.pvData = 0;
		}

		_INT32 i32mem = m_data.cbElements;
		for(_INT32 i=0; i<i32Size; i++)
		{
			i32mem *= m_data.vecBounds[i].cElements;
		}

		m_data.pvData = new _UCHAR[i32mem];
		memset(m_data.pvData,0,i32mem);
		m_i32mem = i32mem;
	}
	return 1;
}

void INTER_SAFEARRAY::makeEmpty()
{
	_INT32 i32Size = m_data.vecBounds.size();
	if( (m_data.cDims > 0) && (i32Size > 0) )
	{
		_INT32 i32mem=1;
		for(_INT32 i=0; i<i32Size; i++)
		{
			i32mem *= m_data.vecBounds[i].cElements;
		}
		i32mem *= m_data.cbElements;
		assert(m_i32mem == i32mem);
		memset(m_data.pvData,0,i32mem);
	}
}


_INT32	INTER_SAFEARRAY::GetElement(_INT32 i32Idx,INTER_VARIANT* pvar)
{// set return pvar value from array data                       
	pvar->SetValue(((_UCHAR*)(m_data.pvData))+i32Idx,m_data.varType);
	return 0;
}

_INT32	INTER_SAFEARRAY::SetElement(_INT32 i32Idx,INTER_VARIANT* pvar)
{// get pvar value into array data
	pvar->GetValue(((_UCHAR*)(m_data.pvData))+i32Idx,m_data.varType);
	return 0;
}

_INT32 INTER_SAFEARRAY::XMLize(_CHAR* szData)
{
	INTER_VARIANT temp;
	for(_INT32 i=0; i<m_i32mem; i+=m_data.cbElements)
	{
		temp.SetValue(((_UCHAR*)(m_data.pvData)) + i,m_data.varType);
		temp.XMLize(szData);
	}
	return 0;
}
