#if !defined(AFX_RUL_SAFEARRAY_H__D92CC21C_0888_4992_9C4F_0FCF39F2C5AC__INCLUDED_)
#define AFX_RUL_SAFEARRAY_H__D92CC21C_0888_4992_9C4F_0FCF39F2C5AC__INCLUDED_

#include "typedefs.h"
#include "SafeVar.h"

extern int size_ofs[];
int inline size_of( VARIANT_TYPE a ) { return (size_ofs[a]); };


class INTER_VARIANT;

class INTER_SAFEARRAY  
{
public:
	// constructors
	INTER_SAFEARRAY();
	INTER_SAFEARRAY( _CHAR* pszValue );
	INTER_SAFEARRAY( _UCHAR* pszValue );
	INTER_SAFEARRAY( wchar_t* pszValue );
	INTER_SAFEARRAY( _BYTE_STRING* bsValue );
	INTER_SAFEARRAY( INTER_SAFEARRAY& safearray);//copy constructor

	virtual ~INTER_SAFEARRAY();
	
	INTER_SAFEARRAY& operator=( INTER_SAFEARRAY& safearray );//equal operator
	INTER_SAFEARRAY& operator=( _CHAR*   pSrc);		// converts if this type is USHORT
	INTER_SAFEARRAY& operator=( _UCHAR*  pSrc);
	INTER_SAFEARRAY& operator=( wchar_t* pSrc);		// converts if this type is CHAR
	INTER_SAFEARRAY& operator=( _BYTE_STRING& src);

	operator _CHAR *(void);						//   assume string - w/conversion (wide 2 narrow)
	operator wchar_t *(void);					//   assume string - w/conversion (narrow 2 wide)

	_INT32 Allocate();
	void makeEmpty(); // makes all elements zero(preserves type & length)

	_INT32 GetElement(
		_INT32 i32Idx,
		INTER_VARIANT* pvar);// pvar must be a passed in varient to be filled

	_USHORT	GetDims(
		vector<_INT32>* pvecDims=0);

	void AddDim(
		INTER_SAFEARRAYBOUND* prgsaBound);
	void AddDim(_INT32 prgsaBound);

	_INT32 MemoryAllocated();

	void SetAllocationParameters(
		VARIANT_TYPE vt,
		_USHORT cDims,
		INTER_SAFEARRAYBOUND* prgsaBound);
	void SetAllocationParameters(
		VARIANT_TYPE vt,
		_USHORT cDims,
		_INT32  prgsaBound);

	_INT32 GetNumberOfElements()	// aka .size()
	{// stevev 31may07 - avoid divide by zero
		if (m_data.cbElements)
			return (m_i32mem/m_data.cbElements);
		else
			return 0;
	}

	_INT32 GetElementSize()
	{
		return (m_data.cbElements);
	}

	_INT32 SetElement(
		_INT32 i32Idx,
		INTER_VARIANT* pvar);
	_INT32 Type()
		{
			return m_data.varType;
		};

	_INT32 XMLize(
		_CHAR* szData);

protected:
	INTER_SAFEARRAY_DATA m_data;
	_INT32				 m_i32mem;
private:
	wchar_t *m_wcharPtr;//this is intended to avoid memory leaks when casting strings between narrow and wide.
	
//public:
//stevev 13feb08	void* getDataPtr(){ return m_data.pvData; };
};

#endif