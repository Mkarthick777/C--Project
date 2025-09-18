
//#include "stdafx.h"
#include "pch.h"
#include "SymbolTable.h"
#include "stdio.h"
#include "VMConstants.h"

CSymbolTable::CSymbolTable()
{
//Vibhor 080705: Adding following calls for cleaning the things; defensive
	m_symbol_table.clear();	
	m_constant_pool_table.clear();
	m_nCurrentScope=0;			//SCR26200 Felix
}

CSymbolTable::~CSymbolTable()
{
	_INT32 i, nSize = m_symbol_table.size();// WS - 9apr07 - VS2005 checkin
	for( i=0;i<nSize;i++)
	{
		delete m_symbol_table[i];
	}
	m_symbol_table.clear();

	nSize = m_constant_pool_table.size();
	for(i=0;i<nSize;i++)
	{
		//Commeneted below by TSRPRASAD 09MAR2004 Fix the memory leaks
		//delete[] m_constant_pool_table[i]->pBytes;
		//m_constant_pool_table[i]->pBytes = 0;
		delete m_constant_pool_table[i];	//TSRPRASAD 09MAR2004 Fix the memory leaks

	}
	m_constant_pool_table.clear();
	m_nCurrentScope=0;				//SCR26200 Felix

}

_INT32 CSymbolTable::Insert(CToken& token)
{
	_INT32 nIdx = GetIndex(token.GetLexeme());
	if(-1 == nIdx)
	{
		CVariable* pNewToken = new CVariable(&token);
		pNewToken->SetSymbolTableIndex(m_symbol_table.size());
		m_symbol_table.push_back(pNewToken);

		return m_symbol_table.size()-1;
	}
	return nIdx;
}

_INT32 CSymbolTable::Insert(CToken& token,_INT32 m_ScopeIndex)
{
	_INT32 nIdx = -1;
	if( (nIdx = Find(token.GetLexeme(),m_ScopeIndex)) == 0 )
	{
		CVariable* pNewToken = new CVariable(&token);
		pNewToken->SetSymbolTableIndex(m_symbol_table.size());
		pNewToken->SetSymbolTableScopeIndex(m_ScopeIndex);
		m_symbol_table.push_back(pNewToken);

		return m_symbol_table.size()-1;
	}
	return nIdx;// return what Find found
}

_INT32 CSymbolTable::InsertConstant(CToken& token)
{
	CONSTANT_POOL_UTF8* pconst_entry=0;
	pconst_entry = new CONSTANT_POOL_UTF8;
	m_constant_pool_table.push_back(pconst_entry);
	pconst_entry->tag = CONSTANT_Utf8;

	_INT16 i16Count	 = strlen(token.GetLexeme());
	pconst_entry->length = _MSB_INT16(i16Count);
	pconst_entry->length <<= 8;
	pconst_entry->length |= _LSB_INT16(i16Count);
	pconst_entry->pBytes = new _UCHAR[pconst_entry->length + 1];
	memset(pconst_entry->pBytes,0,pconst_entry->length + 1);
	memcpy(pconst_entry->pBytes,token.GetLexeme(),pconst_entry->length);

	return m_constant_pool_table.size() -1;
}

//The OID AID combination is 9 bytes ==> 5 OID + 4 AID
_INT32 CSymbolTable::InsertOMConstant(_UCHAR* pchOID_AID,_UCHAR uchType)
{
	CONSTANT_POOL_UTF8* pconst_entry=0;
	pconst_entry = new CONSTANT_POOL_UTF8;
	m_constant_pool_table.push_back(pconst_entry);
	pconst_entry->tag = uchType;

	pconst_entry->length = 9;
	pconst_entry->pBytes = new _UCHAR[pconst_entry->length + 1];
	memcpy(pconst_entry->pBytes,pchOID_AID,pconst_entry->length);

	return m_constant_pool_table.size() -1;
}

_INT32 CSymbolTable::Delete(const _CHAR* pszTokenName)
{
	_INT32 index=0;
	if(-1 != (index = GetIndex(pszTokenName)))
	{
		m_symbol_table.erase(m_symbol_table.begin() + index);
		return index;
	}
	return -1;
}

CVariable* CSymbolTable::Find(const _CHAR* pszTokenName)
{
	_INT32 nSize = m_symbol_table.size();
	CVariable* pToken = 0;
	CVariable* pToken2 = 0;
	for(_INT32 i=0;i<nSize;i++)
	{
		pToken = 0;
		pToken = m_symbol_table[i];
		if (m_nCurrentScope==0)										//SCR26200 Felix
		{
			if(!strcmp(pToken->GetLexeme(),pszTokenName))
			return pToken;
		}
		else
		{
			//Return values from method calling methods.  Arguments to methods calling methods.  Device Variables.
			if( pToken->m_bIsReturnToken || pToken->m_bIsRoutineToken || pToken->m_bIsGlobal )
			{
				if(!strcmp(pToken->GetLexeme(),pszTokenName))
				{
					return pToken;
				}
			}
			else
			{
				if((!strcmp(pToken->GetLexeme(),pszTokenName))&&(pToken->GetSymbolTableScopeIndex() < m_nCurrentScope))
				{
					if( pToken2 )
					{
						if( pToken2->GetSymbolTableScopeIndex() < pToken->GetSymbolTableScopeIndex() )
						{
							pToken2 = pToken;
						}
						// else pToken2 is the closest, leave it be
					}
					else
					{
						pToken2 = pToken;
					}
				}
				if((!strcmp(pToken->GetLexeme(),pszTokenName))&&(pToken->GetSymbolTableScopeIndex() == m_nCurrentScope))
				{
					return pToken;
				}
			}
		}
	}

	return pToken2;
}

//SCR26200 Felix
//This searches for exact matches for inserting
// stevev 25apr13...we need the index, not the pointer....CVariable* CSymbolTable::Find(const _CHAR* pszTokenName, _INT32 
_INT32  CSymbolTable::Find(const _CHAR* pszTokenName, _INT32 m_nSymbolTableScopeIndex)
{
	_INT32 nSize = m_symbol_table.size();
	CVariable* pToken = 0;
	for(_INT32 i=0;i<nSize;i++)
	{
		pToken = 0;
		pToken = m_symbol_table[i];
		if( (!strcmp(pToken->GetLexeme(),pszTokenName))  &&
			(pToken->GetSymbolTableScopeIndex() == m_nSymbolTableScopeIndex)  )
		{
			// stevev 25apr13  return pToken;
			return i;
		}
	}

	return 0;
}

CVariable* CSymbolTable::GetAt(_INT32 nIdx)
{
	if(((_UINT32)nIdx<m_symbol_table.size()) && (nIdx>=0))
		return m_symbol_table[nIdx];
	return 0;
}

/*
CVariable* CSymbolTable::GetConstantAt(_INT32 nIdx)
{
	if((nIdx<m_constant_pool_table.size()) && (nIdx>=0))
		return m_constant_pool_table[nIdx];
	return 0;
}
*/
_INT32 CSymbolTable::GetIndex(const _CHAR* pszTokenName)
{
	_INT32 nSize = m_symbol_table.size();
	for(_INT32 i=0;i<nSize;i++)
	{
		CToken* pToken = m_symbol_table[i];
		if(!strcmp(pToken->GetLexeme(),pszTokenName))
			return i;
	}

	return -1;
}

//SCR26200 Felix
_INT32 CSymbolTable::GetIndex(const _CHAR* pszTokenName, _INT32 m_nSymbolTableScopeIndex)
{
	_INT32 nSize = m_symbol_table.size();
	for(_INT32 i=0;i<nSize;i++)
	{
		CToken* pToken = m_symbol_table[i];
		if((!strcmp(pToken->GetLexeme(),pszTokenName))&&(pToken->GetSymbolTableScopeIndex() == m_nSymbolTableScopeIndex))
		{
			return i;
	}
	}
	
	for(_INT32 i=0;i<nSize;i++)
	{
		CToken* pToken = m_symbol_table[i];
		if(!strcmp(pToken->GetLexeme(),pszTokenName))
		{
			return i;
		}
	}
	return -1;
}

_INT32 CSymbolTable::TraceDump(_CHAR* szDumpFile)
{
	_INT32 nSize = m_symbol_table.size();
	INTER_VARIANT var;

	//strcat(szDumpFile,"<");
	//strcat(szDumpFile,"SymbolTable");
	//strcat(szDumpFile,">");
	std::string result(szDumpFile);

	result += "<SymbolTable>";

	// Copy the result back to szDumpFile
	std::copy(result.begin(), result.end(), szDumpFile);
	szDumpFile[result.size()] = '\0'; // Null-terminate the string

	for(_INT32 i=0;i<nSize;i++)
	{
		/*strcat(szDumpFile,"<");
		strcat(szDumpFile,m_symbol_table[i]->GetLexeme());
		strcat(szDumpFile,">");
		m_symbol_table[i]->GetValue().XMLize(szDumpFile);
		strcat(szDumpFile,"</");
		strcat(szDumpFile,m_symbol_table[i]->GetLexeme());
		strcat(szDumpFile,">");*/

		result += "<";
		result += m_symbol_table[i]->GetLexeme();
		result += ">";
		std::copy(result.begin(), result.end(), szDumpFile);
		szDumpFile[result.size()] = '\0'; // Null-terminate the string
		m_symbol_table[i]->GetValue().XMLize(szDumpFile);
		result = szDumpFile;
		result += "</";
		result += m_symbol_table[i]->GetLexeme();
		result += ">";

		// Copy the result back to szDumpFile
		std::copy(result.begin(), result.end(), szDumpFile);
		szDumpFile[result.size()] = '\0'; // Null-terminate the string
	}
	result = szDumpFile;
	/*strcat(szDumpFile,"</");
	strcat(szDumpFile,"SymbolTable");
	strcat(szDumpFile,">");*/
	result += "</SymbolTable>";
	std::copy(result.begin(), result.end(), szDumpFile);
	szDumpFile[result.size()] = '\0'; // Null-terminate the string

	return 1;

}


//Anil Octobet 5 2005 for handling Method Calling Method
_INT32 CSymbolTable::GetSymbTableSize()
{
	return m_symbol_table.size();
}
