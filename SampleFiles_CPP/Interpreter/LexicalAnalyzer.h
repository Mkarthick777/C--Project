
#if !defined(INCLUDE_AAED80B3_5159_421A_BE00_CC1F06FAFF21)
#define INCLUDE_AAED80B3_5159_421A_BE00_CC1F06FAFF21

#include "ParserDeclarations.h"

#include "SynchronisationSet.h"
#include "Token.h"
//#include "RIDEGeneral.h"

enum PRODUCTION;
class CSymbolTable;
class MEE; //Vibhor 010705: Added

class CLexicalAnalyzer  
{
public:
	CLexicalAnalyzer();
	virtual ~CLexicalAnalyzer();

	_INT32	LookAheadToken(
		CToken**	ppToken);

	/* Returns TRUE if the token type passed is found */
	_BOOL	ScanLineForToken(
							RUL_TOKEN_TYPE tokenType
							, RUL_TOKEN_SUBTYPE tokenSubType
							, CToken** ppToken
							);

	_INT32	GetNextToken(
		CToken**		ppToken,
		CSymbolTable*	pSymbolTable);

	_INT32	GetNextVarToken(
		CToken**		ppToken,
		CSymbolTable*	pSymbolTable,
		RUL_TOKEN_SUBTYPE SubType);

	_INT32	UnGetToken();

	_INT32	GetLineNumber();

	_INT32	Load(
		_UCHAR*			pszRule,
		_UCHAR*			pszRuleName,
		ERROR_VEC*		pvecErrors=0);

	_CHAR*	GetRuleName();

	_INT32	MoveTo(
		RUL_TOKEN_TYPE		Type, 
		RUL_TOKEN_SUBTYPE	SubType,
		CSymbolTable*		pSymbolTable);

	_INT32	MovePast(
		RUL_TOKEN_TYPE		Type, 
		RUL_TOKEN_SUBTYPE	SubType,
		CSymbolTable*		pSymbolTable);

	_INT32	SynchronizeTo(
		PRODUCTION			production,
		CSymbolTable*		pSymbolTable);

	bool IsEndOfSource()
	{
		// warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
		return (m_i32CurrentPos >= (_INT32)strlen((char*)m_pszSource)) ? true:false;
	}

protected:
	_INT32	SaveState(
		_INT32			nCurState);

	_INT32	nextToken(
		CToken**		ppToken,
		_INT32&			i32NewPos,
		_CHAR*			pszBuffer
		,bool           isLookAhead	= false	// stevev 25apr13
		);

	_INT32 nextAnyToken(
		CToken**	ppToken,
		_INT32&		i32NewPos,
		_CHAR*		pszBuffer);

//	Checks for Tokens...
	inline bool isSpace(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos, 
		RUL_TOKEN_TYPE&		Type, 
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer);

	inline bool isTerminal(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos, 
		RUL_TOKEN_TYPE&		Type, 
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer);

	inline bool isIdentifier(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos,
		RUL_TOKEN_TYPE&		Type,
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer);

	inline bool isNumber(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos, 
		RUL_TOKEN_TYPE&		Type,
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer);

	inline bool isString(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos,
		RUL_TOKEN_TYPE&		Type,
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer);

	inline bool isChar(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos,
		RUL_TOKEN_TYPE&		Type,
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer);

	inline bool MatchGrammarTerminals(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos, 
		RUL_TOKEN_TYPE&		Type, 
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer);

	inline bool MatchOMService(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos, 
		RUL_TOKEN_TYPE&		Type, 
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer);

	inline bool isObject(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos,
		RUL_TOKEN_TYPE&		Type,
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer,
		COMPOUND_DATA&		cmpData);

	inline bool isService(
		const _CHAR*		pszSource,
		_INT32&				i32CurPos,
		RUL_TOKEN_TYPE&		Type,
		RUL_TOKEN_SUBTYPE&	SubType,
		_CHAR*				pszBuffer,
		COMPOUND_DATA&		cmpData);

//	Tokenizers...
	_INT32 Tokenize(
		_INT32				i32CurState, 
		RUL_TOKEN_TYPE		Type, 
		RUL_TOKEN_SUBTYPE	SubType,
		CToken**			ppToken,
		_CHAR*				pszBuffer);

	_INT32 Tokenize(
		_INT32				i32CurState, 
		RUL_TOKEN_TYPE		Type, 
		RUL_TOKEN_SUBTYPE	SubType,
		CToken**			ppToken,
		_CHAR*				pszBuffer,
		COMPOUND_DATA&		cmpData);

	_INT32 TokenizeWithoutSave(
		_INT32				i32CurState, 
		RUL_TOKEN_TYPE		Type, 
		RUL_TOKEN_SUBTYPE	SubType,
		CToken**			ppToken,
		_CHAR*				pszBuffer
		);

	_BOOL GetRulString(
					RUL_TOKEN_TYPE tokenType
					, RUL_TOKEN_SUBTYPE tokenSubType
					, _CHAR*		pszRulString
					);
	
//	Data Members...
	_INT32			m_i32CurrentPos;
	_INT32			m_i32PrevPos;
	_INT32			m_i32LAPosition;
	_INT32			m_i32LineNo;
	_INT32			m_i32CurLineNo;
	_INT32			m_i32PrevLineNo;

	_UCHAR*			m_pszSource;
//	_UCHAR			m_szRuleName[RULENAMELEN];

	CToken			m_PrevToken;
	CToken			m_CurToken;
	ERROR_VEC*		m_pvecErr;
/*Vibhor 010705: Start of Code*/	
// MEE member pointer : Path to Global (DD) Data 
	MEE				*m_pMEE; 

public:

	bool InitMeeInterface(MEE *pMEE)
	{
		if(pMEE)
		{
			m_pMEE = pMEE;
			return true;
		}
		return false;

	};
	//Anil August 26 2005 For handling DD variable and Expression
	//This is called When array has Expression within its left and right box
	//this will be formed as Char buffer and Loaded to Lexical Analyzer
	MEE* GetMEEInterface()
	{
		if(m_pMEE)
			return m_pMEE;
		else
			return NULL;
	}
	//Anil August 26 2005 For handling DD variable and Expression
	//Anil changed the function prototype Octobet 5 2005 for handling Method Calling Method
	bool GetComplexDotExp(int iPosOfDot, char** szDotExpression,DD_ITEM_TYPE DDitemType);//Added By Anil July 19 2005
	bool IsEndofComDotOp(char ch,long int iPos);

/*Vibhor 010705: End of Code*/	

	_INT32  m_nPrevSymbolTableScopeIndex;//stevev 25apr13
	_INT32	m_nSymbolTableScopeIndex; //SCR26200 Felix
	_INT32	m_nLastSymbolTableScopeIndex; //SCR26200 Felix for handling Nested Depth of Symbol
	
	//SCR26200 Felix
	//This will return the nested depth of the symbols
	_INT32 GetSymbolTableScopeIndex();
	void SetSymbolTableScopeIndex(
	_INT32 nSymTblScpIdx);


};

#endif