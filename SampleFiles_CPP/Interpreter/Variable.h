#if !defined(INCLUDE_408977D6_A002_4EAE_A2CC_058502289978)
#define INCLUDE_408977D6_A002_4EAE_A2CC_058502289978

#include "Token.h"
#include "INTER_VARIANT.h"

class CVariable : public CToken
{

public:
	CVariable();
	CVariable(
		CToken* pToken);

	CVariable(
		const _CHAR* szLexeme,
		RUL_TOKEN_TYPE Type,
		RUL_TOKEN_SUBTYPE SubType);

	virtual ~CVariable();

	//	Identify self
	virtual void Identify(
		_CHAR* szData);

	INTER_VARIANT& GetValue();
	void SetVarType(RUL_TOKEN_TYPE Type, RUL_TOKEN_SUBTYPE SubType);//Added By Anil August 5 2005 For handling DD variable and Expression
	/*Vibhor 070705: Start of Code*/
	//Adding following overloaded fns for setting the initial
	//values of Global (DD) variables .
#ifdef MODIFIED_BY_SS
	template<typename T>
	void SetValue(T value)
	{
		m_Value = value;
	}
	/*void SetValue(int iValue)
	{
		m_Value = (_INT32)iValue;
	};

	void SetValue(float fValue)
	{
		m_Value = fValue;
	};

	void SetValue(double dValue)
	{
		m_Value = dValue;
	};

	void SetValue(char* pszValue)
	{
		m_Value = pszValue;
	};

	void SetValue(bool bValue)
	{
		m_Value = bValue;
	};

	void SetValue(unsigned short bValue)
	{
		m_Value = bValue;
	};
	void SetValue(std::string sValue)
	{
		m_Value = sValue;
	};*/
#endif
/*Vibhor 070705: End of Code*/

protected:
	INTER_VARIANT		m_Value;

};

#endif