//#include "stdafx.h"
#include "pch.h"
#include "Variable.h"

CVariable::CVariable()
{
}

CVariable::CVariable(
			CToken* pToken)
	:CToken(*pToken)
{
	RUL_TOKEN_TYPE Type = pToken->GetType();
	RUL_TOKEN_SUBTYPE SubType = pToken->GetSubType();

	if (Type == RUL_SIMPLE_VARIABLE)
	{
		switch(SubType)
		{
			case RUL_CHAR_DECL:
					m_Value = (char)' ';
					break;
			case RUL_LONG_LONG_DECL:
					m_Value = (INT64)0;
					break;
					// Walt EPM 08sep08 - added
			case RUL_UNSIGNED_SHORT_INTEGER_DECL:
#ifdef MODIFIED_BY_SS
					//m_Value = (ushort)(0);;
				m_Value = static_cast<_UINT32>(0);
#endif
					break;
			case RUL_SHORT_INTEGER_DECL:
					m_Value = static_cast<short>(0); 
					break;
			case RUL_UNSIGNED_INTEGER_DECL:
					m_Value = static_cast<unsigned int>(0);
					break;
					// Walt EPM 08sep08 - end added
			case RUL_INTEGER_DECL:
			case RUL_LONG_DECL:
					m_Value = (_INT32)0;//a long is not an int
					break;
			case RUL_BOOLEAN_DECL:
					m_Value = (bool)false;
					break;
			case RUL_REAL_DECL:
					m_Value = (float)0.0;
					break;
			case RUL_DOUBLE_DECL:
					m_Value = (double)0.0;//WS:EPM 10aug07
					break;
			case RUL_STRING_DECL:
			case RUL_DD_STRING_DECL: //Added By Anil July 07 2005
					m_Value = (char*)"";
					break;
			case RUL_UNSIGNED_CHAR_DECL:
					m_Value = (unsigned char)' ';//WHS EP June17-2008 have changed this to make sure that it works for all data types
					break;
			

		}
	}
}

CVariable::CVariable(
			const _CHAR* szLexeme,
			RUL_TOKEN_TYPE Type,
			RUL_TOKEN_SUBTYPE SubType) 
	:CToken(szLexeme,Type,SubType,-1)
{
}

CVariable::~CVariable()
{
}

void CVariable::Identify(
		_CHAR* szData)
{
	std::string data(szData);

	// Append opening tag
	data += "<";
	data += m_pszLexeme;
	data += ">";

	if (GetCompoundData())
	{
		// Append compound data
		data += GetCompoundData()->m_szName;
		data += ",";
		data += GetCompoundData()->m_szAttribute;
	}

	// Append closing tag
	data += "</";
	data += m_pszLexeme;
	data += ">";

	// Copy the updated string back to szData
	size_t dataSize = data.size();
	memcpy(szData, data.c_str(), dataSize);
	szData[dataSize] = '\0'; // Ensure null-termination
}

INTER_VARIANT& CVariable::GetValue()
{
	return m_Value;
}

//Anil August 26 2005 For handling DD variable and Expression
void CVariable::SetVarType(RUL_TOKEN_TYPE Type, RUL_TOKEN_SUBTYPE SubType)
{
	if (Type == RUL_SIMPLE_VARIABLE)
	{
		switch (SubType)
		{
		case RUL_CHAR_DECL:
			m_Value = ' ';
			break;
		case RUL_LONG_LONG_DECL:
			m_Value = static_cast<INT64>(0);
			break;
		case RUL_UNSIGNED_SHORT_INTEGER_DECL:
			m_Value = static_cast<_UINT32>(0);
			break;
		case RUL_SHORT_INTEGER_DECL:
			m_Value = static_cast<short>(0);
			break;
		case RUL_UNSIGNED_INTEGER_DECL:
			m_Value = static_cast<unsigned int>(0);
			break;
		case RUL_INTEGER_DECL:
		case RUL_LONG_DECL:
			m_Value = static_cast<_INT32>(0); // a long is not an int
			break;
		case RUL_BOOLEAN_DECL:
			m_Value = false;
			break;
		case RUL_REAL_DECL:
			m_Value = 0.0f;
			break;
		case RUL_DOUBLE_DECL:
			m_Value = 0.0; // WS:EPM 10aug07
			break;
		case RUL_STRING_DECL:
		case RUL_DD_STRING_DECL: // Added By Anil July 07 2005
			m_Value = ""; // Assign an empty string
			break;
		case RUL_UNSIGNED_CHAR_DECL:
			m_Value = static_cast<unsigned char>(' '); // WHS EP June17-2008 have changed this to make sure that it works for all data types
			break;
		}
	}
}