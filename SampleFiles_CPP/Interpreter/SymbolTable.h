
#if !defined(AFX_SYMBOLTABLE_H__685A3744_0D8C_40F9_9508_8F0345F78F36__INCLUDED_)
#define AFX_SYMBOLTABLE_H__685A3744_0D8C_40F9_9508_8F0345F78F36__INCLUDED_


#include <vector>
#include "Token.h"
#include "Variable.h"

using namespace std;
typedef vector<CVariable*> SYMBOL_TABLE;

struct CONSTANT_POOL_UTF8
{
	_UINT8		tag;
	_UINT16		length;
	_UINT8*		pBytes;	// this is of length bytes

	CONSTANT_POOL_UTF8()
	{
		tag = 0;
		length = 0;
		pBytes = 0;
	}
	~CONSTANT_POOL_UTF8()
	{
		tag = 0;
		length = 0;
		if(pBytes)
		{
			delete[] pBytes;
			pBytes = 0;
		}
	}
};
typedef vector<CONSTANT_POOL_UTF8*>  CONSTANT_POOL_TABLE;


class CSymbolTable  
{
public:
	CSymbolTable();
	virtual ~CSymbolTable();

	CVariable*	GetAt(
		_INT32 nIdx);

	CVariable*	GetConstantAt(
		_INT32 nIdx);

	_INT32	Insert(
		CToken& token);

	_INT32	Insert(
		CToken& token,_INT32 m_ScopeIndex);

	_INT32	Delete(
		const _CHAR* pszTokenName);

	CVariable*	Find(
		const _CHAR* pszTokenName);

	_INT32	TraceDump(
		_CHAR* szDumpFile);

	_INT32	GetCount()
		{
			return ((_INT32)m_symbol_table.size());// WS - 9apr07 - VS2005 checkin
		};

	_INT32	InsertConstant(
		CToken& token);

	_INT32	InsertOMConstant(
		_UCHAR* pchOID_AID,
		_UCHAR uchType);

	CONSTANT_POOL_TABLE	m_constant_pool_table;

	_INT32 m_nCurrentScope;			//SCR26200 Felix

	_INT32	GetIndex(
		const _CHAR* pszTokenName);  //Vibhor 010705: Made Public
	
	_INT32 GetIndex(
		const _CHAR* pszTokenName, _INT32 m_nSymbolTableScopeIndex); //SCR26200 Felix
	
	// stevev 25apr13  CVariable* Find(
	_INT32  Find(
		const _CHAR* pszTokenName, _INT32 m_nSymbolTableScopeIndex); //SCR26200 Felix

	void 
		Remove(_INT32 m_ScopeIndex); // 24apr13 stevev

	_INT32	GetSymbTableSize();  //Anil Octobet 5 2005 for handling Method Calling Method



protected:

	SYMBOL_TABLE			m_symbol_table;
};

#endif