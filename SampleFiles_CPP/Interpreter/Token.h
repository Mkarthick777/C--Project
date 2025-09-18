
#if !defined(INCLUDE_44485AC8_5910_418A_A61C_1B05F786EEF1)
#define INCLUDE_44485AC8_5910_418A_A61C_1B05F786EEF1

#include "ParserDeclarations.h"

struct COMPOUND_DATA
{
	_CHAR	m_szName[BUFFER_SIZE];
	_CHAR	m_szAttribute[BUFFER_SIZE];
};

class CToken  
{
public:
	CToken();
	CToken(
		const _CHAR* szLexeme);

	CToken(
		const _CHAR* szLexeme,
		RUL_TOKEN_TYPE Type,
		RUL_TOKEN_SUBTYPE SubType,
		_INT32 i32LineNo);

	CToken(
		const CToken& token);

	CToken(
		const _CHAR* szLexeme,
		RUL_TOKEN_TYPE Type,
		RUL_TOKEN_SUBTYPE SubType,
		COMPOUND_DATA& cmpData,
		_INT32 i32LineNo);

	virtual ~CToken();

	bool				IsAssignOp();
	bool				IsOperator();
	bool				IsNumeric();
	bool				IsVariable();
	bool				IsDeclaration();
	bool				IsSelection();
	bool				IsIFStatement();
	bool				IsWHILEStatement();
	bool				IsDOStatement();
	bool				IsFORStatement();
	bool				IsELSEStatement();
	bool				IsIteration();
	bool				IsCompound();
	bool				IsConstant();
	bool				IsEOS();
	bool				IsService();
	bool				IsArrayVar();
	bool				IsOMToken();
	bool				IsFunctionToken();
	bool				IsBREAKStatement();
	bool				IsCONTINUEStatement();
	bool				IsRETURNStatement();
	bool				IsDDItem(); //Vibhor 140705: Added to return if the token is a DD ITEM
	RUL_TOKEN_TYPE		GetType()
		{
			return m_Type;
		};

	RUL_TOKEN_SUBTYPE	GetSubType()
		{
			return m_SubType;
		};

	void SetSubType(
		RUL_TOKEN_SUBTYPE SubType);

	bool IsSymbol();

	_INT32 GetSymbolTableIndex()
	{
		return m_nSymbolTableIndex;
	};

	void SetSymbolTableIndex(
		_INT32 nSymTblIdx)
	{
		m_nSymbolTableIndex = nSymTblIdx;
	};

	_INT32 GetConstantIndex()
	{
		return m_i32constant_pool_idx;
	};

	void SetConstantIndex(
		_INT32 i32constant_pool_idx)
	{
		m_i32constant_pool_idx = i32constant_pool_idx;
	};

	//SCR26200 Felix
	void SetSymbolTableScopeIndex(
		_INT32 nSymTblScpIdx)
	{
		m_nSymbolTableScopeIndex = nSymTblScpIdx;
	};

	_INT32 GetSymbolTableScopeIndex()	//SCR26200 Felix
	{
		return m_nSymbolTableScopeIndex;
	};

	CToken&	operator=(const CToken& token);
	bool	operator==(const CToken& token);
	
	const _CHAR*	GetLexeme();
	COMPOUND_DATA*	GetCompoundData();
	const _CHAR*	GetDDItemName();//Anil August 26 2005 For handling DD variable and Expression

//	Identify self
	virtual void Identify(
		_CHAR* szData);

	virtual _INT32	GetLineNumber();

protected:
	RUL_TOKEN_TYPE		m_Type;
	RUL_TOKEN_SUBTYPE	m_SubType;
	_CHAR*				m_pszLexeme;
	_INT32				m_nSymbolTableIndex;
	_INT32				m_i32constant_pool_idx;
	COMPOUND_DATA*		m_pCompound;
	_INT32				m_i32LineNo;
	_CHAR*				m_pszDDItemName;//Anil August 26 2005 For handling DD variable and Expression



public:									//Vibhor 060705: Added
	bool				m_bIsGlobal;	//whether the symbol is a DD item (not a method local)
	
	bool				m_bIsRoutineToken;//Anil Octobet 5 2005 for handling Method Calling Method
										  //To know whthet is routine ie, if this parameter has come when one method calls other
										  //So that need not  Execute the declaration list

	bool				m_bIsReturnToken; //Anil Octobet 5 2005 for handling Method Calling Method
										  //Just to make sure the the variable is reuturn toke
	void SetType(RUL_TOKEN_TYPE Type);	//Vibhor 140705: Added: CAUTION !!!! Have great discretion in using this call!!!
	void SetLexeme(const _CHAR* szLexeme); //Anil August 26 2005 For handling DD variable and Expression
	void SetDDItemName(const _CHAR* szComplexDDExpre);//Anil August 26 2005 For handling DD variable and Expression

	_INT32				m_nSymbolTableScopeIndex; //SCR26200 Felix for handling Nested Depth of Symbol


};

#endif 