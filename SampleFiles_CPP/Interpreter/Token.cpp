
//#include "stdafx.h"
#include "pch.h"
#include "Token.h"


CToken::CToken()
{
	m_pszLexeme = NULL;
	m_pszDDItemName = NULL;//Anil August 26 2005 For handling DD variable and Expression
	m_pCompound = NULL;
	m_Type		= RUL_TYPE_NONE;
	m_SubType	= RUL_SUBTYPE_NONE;
	m_nSymbolTableIndex = -1;
	m_i32constant_pool_idx = -1;
	m_i32LineNo = -1;
	m_bIsGlobal = false;  //Vibhor 070705: Added
	m_bIsRoutineToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_bIsReturnToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_nSymbolTableScopeIndex=0; ///SCR26200 /Felix for handling Nested Depth of Symbol

}

CToken::CToken(
			const _CHAR* szLexeme)
{
	_INT32 nLen = strlen(szLexeme) + 1;
	m_pszLexeme = new _CHAR[nLen];
	memset(m_pszLexeme,0,nLen);
	memcpy(m_pszLexeme,szLexeme,nLen );
	m_pszDDItemName = NULL;//Added By Anil August 22 2005
	m_pCompound = NULL;

	m_Type		= RUL_TYPE_NONE;
	m_SubType	= RUL_SUBTYPE_NONE;
	m_nSymbolTableIndex = -1;
	m_i32constant_pool_idx = -1;
	m_i32LineNo = -1;
	m_bIsGlobal = false;  //Vibhor 070705: Added
	m_bIsRoutineToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_bIsReturnToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_nSymbolTableScopeIndex=0; //SCR26200 Felix for handling Nested Depth of Symbol

}

CToken::CToken(
			const _CHAR* szLexeme,
			RUL_TOKEN_TYPE Type,
			RUL_TOKEN_SUBTYPE SubType,
			_INT32	i32LineNo)
{
	_INT32 nLen = strlen(szLexeme) + 1;
	m_pszLexeme = new _CHAR[nLen];
	memset(m_pszLexeme,0,nLen);
	memcpy(m_pszLexeme,szLexeme,nLen );
	m_pszDDItemName = NULL;//Added By Anil August 22 2005
	m_pCompound = NULL;

	m_Type		= Type;
	m_SubType	= SubType;
	m_nSymbolTableIndex = -1;
	m_i32constant_pool_idx = -1;
	m_i32LineNo = i32LineNo;
	m_bIsGlobal = false;  //Vibhor 070705: Added
	m_bIsRoutineToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_bIsReturnToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_nSymbolTableScopeIndex=0; //SCR26200 Felix for handling Nested Depth of Symbol

}

CToken::CToken(
			const _CHAR* szLexeme,
			RUL_TOKEN_TYPE Type,
			RUL_TOKEN_SUBTYPE SubType,
			COMPOUND_DATA& cmpData,
			_INT32	i32LineNo)
{
	_INT32 nLen = strlen(szLexeme) + 1;
	m_pszLexeme = new _CHAR[nLen];
	memset(m_pszLexeme,0,nLen);
	memcpy(m_pszLexeme,szLexeme,nLen );
	m_pszDDItemName = NULL;//Added By Anil August 22 2005

	m_Type		= Type;
	m_SubType	= SubType;
	m_nSymbolTableIndex = -1;
	m_i32constant_pool_idx = -1;
	m_i32LineNo = i32LineNo;
	m_bIsGlobal = false;  //Vibhor 070705: Added

	m_pCompound = new COMPOUND_DATA;
	memset(m_pCompound->m_szName,0,BUFFER_SIZE);// emerson checkin april2013
	memset(m_pCompound->m_szAttribute,0,BUFFER_SIZE);// emerson checkin april2013
#ifdef MODIFIED_BY_SS
	/*strcpy(m_pCompound->m_szName,cmpData.m_szName);
	strcpy(m_pCompound->m_szAttribute,cmpData.m_szAttribute);*/
	std::string szName(cmpData.m_szName);
	std::copy(szName.begin(), szName.end(), m_pCompound->m_szName);
	m_pCompound->m_szName[szName.length()] = '\0';
	szName.assign(cmpData.m_szAttribute);
	std::copy(szName.begin(), szName.end(), m_pCompound->m_szAttribute);
	m_pCompound->m_szAttribute[szName.length()] = '\0';
#endif
	m_bIsReturnToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_bIsRoutineToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_nSymbolTableScopeIndex=0; //SCR26200 Felix for handling Nested Depth of Symbol

}

CToken::CToken(
			const CToken& token)
{
	_INT32 nLen = 0;
	m_Type = token.m_Type;
	m_SubType = token.m_SubType;
	nLen = strlen(token.m_pszLexeme);
	m_pszLexeme = new _CHAR[nLen + 1];
	memset(m_pszLexeme,0,nLen+1);
	memcpy(m_pszLexeme,token.m_pszLexeme,nLen);
	m_pszDDItemName = NULL;
	//Anil August 26 2005 For handling DD variable and Expression
	if(token.m_pszDDItemName != NULL)
	{
		nLen = strlen(token.m_pszDDItemName);
		m_pszDDItemName = new _CHAR[nLen + 1];
		memset(m_pszDDItemName,0,nLen+1);
		memcpy(m_pszDDItemName,token.m_pszDDItemName,nLen);
	}

	m_nSymbolTableIndex = token.m_nSymbolTableIndex;
	m_i32constant_pool_idx = token.m_i32constant_pool_idx;
	m_i32LineNo = token.m_i32LineNo;
	m_bIsGlobal = token.m_bIsGlobal;  //Vibhor 070705: Added
	m_bIsRoutineToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	m_bIsReturnToken = false;//Anil Octobet 5 2005 for handling Method Calling Method
	
	m_nSymbolTableScopeIndex=0; //SCR26200 Felix for handling Nested Depth of Symbol

	m_pCompound = NULL;
	if(token.m_pCompound)
	{
		m_pCompound = new COMPOUND_DATA;
		memset(m_pCompound->m_szName,0,BUFFER_SIZE);// emerson checkin april2013
		memset(m_pCompound->m_szAttribute,0,BUFFER_SIZE);// emerson checkin april2013

#ifdef MODIFIED_BY_SS
	/*	strcpy(m_pCompound->m_szName,token.m_pCompound->m_szName);
		strcpy(m_pCompound->m_szAttribute,token.m_pCompound->m_szAttribute);*/
		std::string szName(token.m_pCompound->m_szName);
		std::copy(szName.begin(), szName.end(), m_pCompound->m_szName);
		m_pCompound->m_szName[szName.length()] = '\0';
		szName.assign(token.m_pCompound->m_szAttribute);
		std::copy(szName.begin(), szName.end(), m_pCompound->m_szAttribute);
		m_pCompound->m_szAttribute[szName.length()] = '\0';
#endif
	}

}

CToken::~CToken()
{
	DELETE_ARR( m_pszLexeme );
	DELETE_ARR(m_pszDDItemName);
	DELETE_PTR(m_pCompound);
	m_nSymbolTableIndex = -1;
	m_i32constant_pool_idx = -1;

}


bool CToken::IsOperator()
{
	if((m_Type == RUL_ARITHMETIC_OPERATOR) 
		|| (m_Type == RUL_LOGICAL_OPERATOR)
		|| (m_Type == RUL_ASSIGNMENT_OPERATOR)
		|| (m_Type == RUL_RELATIONAL_OPERATOR)
		)
	{
		return true;
	}
	return false;
}

CToken& CToken::operator=(const CToken& token)
{
	_INT32 nLen = 0;
	m_Type = token.m_Type;
	m_SubType = token.m_SubType;

	//set lexeme
	DELETE_ARR( m_pszLexeme );
	if(token.m_pszLexeme)
	{
		nLen = strlen(token.m_pszLexeme);
	}

	m_pszLexeme = new _CHAR[nLen + 1];
	memset(m_pszLexeme,0,nLen+1);
	memcpy(m_pszLexeme,token.m_pszLexeme,nLen);
	m_i32LineNo = token.m_i32LineNo;

	//set compound
	DELETE_PTR(m_pCompound);
	if(token.m_pCompound)
	{
		m_pCompound = new COMPOUND_DATA;
		memset(m_pCompound->m_szName,0,BUFFER_SIZE);// emerson checkin april2013
		memset(m_pCompound->m_szAttribute,0,BUFFER_SIZE);// emerson checkin april2013
#ifdef MODIFIED_BY_SS
		//strcpy(m_pCompound->m_szName, token.m_pCompound->m_szName);
		std::string szName(token.m_pCompound->m_szName);
		std::copy(szName.begin(), szName.end(), m_pCompound->m_szName);

		//strcpy(m_pCompound->m_szAttribute,token.m_pCompound->m_szAttribute);
		szName.assign(token.m_pCompound->m_szAttribute);
		std::copy(szName.begin(), szName.end(), m_pCompound->m_szAttribute);

		
#endif
	}
	return *this;
}

bool CToken::operator==(const CToken& token)
{
	if(strcmp(token.m_pszLexeme, m_pszLexeme))
	{
		return false;
	}
	if((token.m_Type != m_Type) || (token.m_SubType != m_SubType))
	{
		return false;
	}
	if(token.m_pCompound && m_pCompound)
	{
		return !(strcmp(token.m_pCompound->m_szAttribute, m_pCompound->m_szAttribute) 
			&& strcmp(token.m_pCompound->m_szName, m_pCompound->m_szName))? true:false;
	}
	else
	{
		if(token.m_pCompound == m_pCompound == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;

}

bool CToken::IsNumeric()
{
	return (m_Type == RUL_NUMERIC_CONSTANT)? true:false;
}

bool CToken::IsConstant()
{
	switch(m_SubType)
	{
	case RUL_REAL_CONSTANT:
	case RUL_BOOL_CONSTANT:
	case RUL_CHAR_CONSTANT:
	case RUL_INT_CONSTANT:
	case RUL_STRING_CONSTANT:
		return true;
	}
	return false;
}

bool CToken::IsVariable()
{
	return ((m_Type == RUL_SIMPLE_VARIABLE)//||(m_Type == RUL_OBJECT_VARIABLE)
			|| (m_SubType == RUL_SERVICE_ATTRIBUTE))? true:false;
}

bool CToken::IsAssignOp()
{
	return (m_Type == RUL_ASSIGNMENT_OPERATOR)? true:false;
}

bool CToken::IsWHILEStatement()
{
	if(RUL_KEYWORD == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_WHILE:
			return true;
		}
	}
	return false;
}

bool CToken::IsDOStatement()
{
	if(RUL_KEYWORD == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_DO:
			return true;
		}
	}
	return false;
}

bool CToken::IsFORStatement()
{
	if(RUL_KEYWORD == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_FOR:
			return true;
		}
	}
	return false;
}

bool CToken::IsDeclaration()
{
	if(RUL_KEYWORD == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_UNSIGNED_INTEGER_DECL:
		case RUL_INTEGER_DECL:
		case RUL_UNSIGNED_SHORT_INTEGER_DECL:
		case RUL_SHORT_INTEGER_DECL:
		case RUL_LONG_DECL:
		case RUL_LONG_LONG_DECL:
		case RUL_REAL_DECL: 
		case RUL_DOUBLE_DECL: 
		case RUL_BOOLEAN_DECL:
		case RUL_CHAR_DECL:
		case RUL_STRING_DECL:
		//Added By Anil June 14 2005 --starts here
		case RUL_DD_STRING_DECL:
		//Added By Anil June 14 2005 --Ends here
		case RUL_UNSIGNED_CHAR_DECL:
			return true;

		}
	}
	return false;

}

bool CToken::IsSelection()
{
	if(RUL_KEYWORD == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_IF:
		case RUL_ELSE:
		case RUL_SWITCH:
		case RUL_CASE:
		case RUL_DEFAULT:
			return true;
		}
	}
	return false;
}

bool CToken::IsIFStatement()
{
	if(RUL_KEYWORD == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_IF:
			return true;
		}
	}
	return false;
}

bool CToken::IsELSEStatement()
{
	if(RUL_KEYWORD == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_ELSE:
			return true;
		}
	}
	return false;
}

bool CToken::IsIteration()
{
	if(RUL_KEYWORD == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_WHILE:
		case RUL_FOR:
		case RUL_DO:
			return true;
		}
	}
	return false;
}

bool CToken::IsCompound()
{
	if(RUL_SYMBOL == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_LBRACK:
		case RUL_COLON:// For handling case
			return true;
		}
	}
	return false;
}

bool CToken::IsService()
{
	if(RUL_SERVICE == m_Type)
	{
		switch(m_SubType)
		{
		case RUL_SERVICE_INVOKE:
			return true;
		}
	}
	return false;
}

const _CHAR* CToken::GetLexeme()
{
	return m_pszLexeme;
}

bool CToken::IsEOS()
{
	if(m_SubType == RUL_SEMICOLON)
		return true;
	return false;
}

void CToken::SetSubType(
				RUL_TOKEN_SUBTYPE SubType)
{
	m_SubType = SubType;
}

bool CToken::IsSymbol()
{
	if(m_Type == RUL_SYMBOL)
		return true;
	return false;
}

COMPOUND_DATA* CToken::GetCompoundData()
{
	return m_pCompound;
}

void CToken::Identify(_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,m_pszLexeme);
	strcat(szData,">");

	if(GetCompoundData())
	{
		strcat(szData,GetCompoundData()->m_szName);
		strcat(szData,",");
		strcat(szData,GetCompoundData()->m_szAttribute);
	} 

	strcat(szData,"</");
	strcat(szData,m_pszLexeme);
	strcat(szData,">");*/
	std::string result(szData);

	result += "<";
	result += m_pszLexeme;
	result += ">";

	if (GetCompoundData())
	{
		result += GetCompoundData()->m_szName;
		result += ",";
		result += GetCompoundData()->m_szAttribute;
	}

	result += "</";
	result += m_pszLexeme;
	result += ">";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string




#endif 
}

bool CToken::IsArrayVar()
{
	if(m_Type == RUL_ARRAY_VARIABLE)
		return true;
	return false;
}

bool CToken::IsOMToken()
{
	if((m_Type == RUL_KEYWORD)
		&& (m_SubType == RUL_OM))
		return true;
	return false;
}

bool CToken::IsFunctionToken()
{
	if((m_Type == RUL_KEYWORD)
		&& (m_SubType == RUL_FUNCTION))
		return true;
	return false;
}

bool CToken::IsBREAKStatement()
{
	if((m_Type == RUL_KEYWORD)
		&& (m_SubType == RUL_BREAK))
		return true;
	return false;
}

bool CToken::IsCONTINUEStatement()
{
	if((m_Type == RUL_KEYWORD)
		&& (m_SubType == RUL_CONTINUE))
		return true;
	return false;
}

bool CToken::IsRETURNStatement()
{
	if((m_Type == RUL_KEYWORD)
		&& (m_SubType == RUL_RETURN))
		return true;
	return false;
}

_INT32 CToken::GetLineNumber()
{
	return m_i32LineNo;
}

/*Vibhor 140705: Start of Code*/

bool CToken::IsDDItem()
{
	return ((m_Type == RUL_DD_ITEM) ? true:false);
}

void CToken::SetType(RUL_TOKEN_TYPE type)
{
	m_Type = type;
}

/*Vibhor 140705: End of Code*/


//Added By Anil July 28 2005 --starts here
//Added for the Global Symbol takel Though it is not that Clan Solution
void CToken::SetLexeme(const _CHAR* szLexeme)
{
	DELETE_ARR( m_pszLexeme );
	
	_INT32 nLen = strlen(szLexeme) + 1;
	m_pszLexeme = new _CHAR[nLen];
	memset(m_pszLexeme,0,nLen);
	memcpy(m_pszLexeme,szLexeme,nLen );

	return;
}


void CToken::SetDDItemName(const _CHAR* szComplexDDExpre)
{
	_INT32 nLen = strlen(szComplexDDExpre) + 1;
	
	DELETE_ARR( m_pszDDItemName );
	m_pszDDItemName = new _CHAR[nLen];
	memset(m_pszDDItemName,0,nLen);
	memcpy(m_pszDDItemName,szComplexDDExpre,nLen );

	return;
}



const _CHAR* CToken::GetDDItemName()
{
		return m_pszDDItemName;
}
