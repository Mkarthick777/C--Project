//#include "stdafx.h"
#include "pch.h"
#include "ExpParser.h"
#include "LexicalAnalyzer.h"
#include "SymbolTable.h"
#include "Token.h"
#include "PrimaryExpression.h"
#include "CompoundExpression.h"
#include "IFExpression.h"

#include "ErrorDefinitions.h"

#ifdef _FULL_RULE_ENGINE

#include "ArrayExpression.h"
#include "ComplexDDExpression.h"//Anil August 26 2005
#include "OMServiceExpression.h"
#include "FunctionExpression.h"
//#include "RIDEError.h"

#endif

PRECEDENCE_TABLE CExpParser::s_PrecedenceTable;

//	E := E || E
//	E := E && E
//	E := E <> E
//	E := E == E
//	E := E <  E
//	E := E >  E
//	E := E <= E
//	E := E >= E
//	E := E +  E
//	E := E -  E
//	E := E *  E
//	E := E /  E
//	E := E %  E
//	E := E ** E
//	E := +E
//	E := -E
//	E := !E
//	E := (E)
//	E := id
//	E := Constants
void CExpParser::InitializePrecedenceTable()
{
	s_PrecedenceTable[RUL_DOLLAR].StackTop			= -1;
	s_PrecedenceTable[RUL_DOLLAR].Incoming			= -1;
	s_PrecedenceTable[RUL_DOLLAR].n					= 0;
							
// =, +=, -=, *=, /=, %=, &=, ^=, |=, <<=, >>=
	s_PrecedenceTable[RUL_ASSIGN].StackTop			= 12;
	s_PrecedenceTable[RUL_ASSIGN].Incoming			= 11;
	s_PrecedenceTable[RUL_ASSIGN].n					= 2;

	s_PrecedenceTable[RUL_PLUS_ASSIGN].StackTop		= 12;
	s_PrecedenceTable[RUL_PLUS_ASSIGN].Incoming		= 11;
	s_PrecedenceTable[RUL_PLUS_ASSIGN].n			= 2;

	s_PrecedenceTable[RUL_MINUS_ASSIGN].StackTop	= 12;
	s_PrecedenceTable[RUL_MINUS_ASSIGN].Incoming	= 11;
	s_PrecedenceTable[RUL_MINUS_ASSIGN].n			= 2;

	s_PrecedenceTable[RUL_DIV_ASSIGN].StackTop		= 12;
	s_PrecedenceTable[RUL_DIV_ASSIGN].Incoming		= 11;
	s_PrecedenceTable[RUL_DIV_ASSIGN].n				= 2;

	s_PrecedenceTable[RUL_MOD_ASSIGN].StackTop		= 12;
	s_PrecedenceTable[RUL_MOD_ASSIGN].Incoming		= 11;
	s_PrecedenceTable[RUL_MOD_ASSIGN].n				= 2;

	s_PrecedenceTable[RUL_MUL_ASSIGN].StackTop		= 12;
	s_PrecedenceTable[RUL_MUL_ASSIGN].Incoming		= 11;
	s_PrecedenceTable[RUL_MUL_ASSIGN].n				= 2;

	s_PrecedenceTable[RUL_BIT_AND_ASSIGN].StackTop	= 12;
	s_PrecedenceTable[RUL_BIT_AND_ASSIGN].Incoming	= 11;
	s_PrecedenceTable[RUL_BIT_AND_ASSIGN].n			= 2;

	s_PrecedenceTable[RUL_BIT_OR_ASSIGN].StackTop	= 12;
	s_PrecedenceTable[RUL_BIT_OR_ASSIGN].Incoming	= 11;
	s_PrecedenceTable[RUL_BIT_OR_ASSIGN].n			= 2;

	s_PrecedenceTable[RUL_BIT_XOR_ASSIGN].StackTop	= 12;
	s_PrecedenceTable[RUL_BIT_XOR_ASSIGN].Incoming	= 11;
	s_PrecedenceTable[RUL_BIT_XOR_ASSIGN].n			= 2;

	s_PrecedenceTable[RUL_BIT_RSHIFT_ASSIGN].StackTop	= 12;
	s_PrecedenceTable[RUL_BIT_RSHIFT_ASSIGN].Incoming	= 11;
	s_PrecedenceTable[RUL_BIT_RSHIFT_ASSIGN].n			= 2;

	s_PrecedenceTable[RUL_BIT_LSHIFT_ASSIGN].StackTop	= 12;
	s_PrecedenceTable[RUL_BIT_LSHIFT_ASSIGN].Incoming	= 11;
	s_PrecedenceTable[RUL_BIT_LSHIFT_ASSIGN].n			= 2;

/*	s_PrecedenceTable[RUL_EXP].StackTop				= 13;
	s_PrecedenceTable[RUL_EXP].Incoming				= 14;
	s_PrecedenceTable[RUL_EXP].n					= 2;*/

// ?, :
	s_PrecedenceTable[RUL_QMARK].StackTop		= 14;
	s_PrecedenceTable[RUL_QMARK].Incoming		= 13;
	s_PrecedenceTable[RUL_QMARK].n				= 0;

	s_PrecedenceTable[RUL_COLON].StackTop		= 14;
	s_PrecedenceTable[RUL_COLON].Incoming		= 13;
	s_PrecedenceTable[RUL_COLON].n				= 0;


// ||	
	s_PrecedenceTable[RUL_LOGIC_OR].StackTop		= 16;
	s_PrecedenceTable[RUL_LOGIC_OR].Incoming		= 15;
	s_PrecedenceTable[RUL_LOGIC_OR].n				= 2;


// &&
	s_PrecedenceTable[RUL_LOGIC_AND].StackTop		= 18;
	s_PrecedenceTable[RUL_LOGIC_AND].Incoming		= 17;
	s_PrecedenceTable[RUL_LOGIC_AND].n				= 2;


// |
	s_PrecedenceTable[RUL_BIT_OR].StackTop			= 20;
	s_PrecedenceTable[RUL_BIT_OR].Incoming			= 19;
	s_PrecedenceTable[RUL_BIT_OR].n					= 2;


// ^
	s_PrecedenceTable[RUL_BIT_XOR].StackTop			= 22;
	s_PrecedenceTable[RUL_BIT_XOR].Incoming			= 21;
	s_PrecedenceTable[RUL_BIT_XOR].n				= 2;


// &
	s_PrecedenceTable[RUL_BIT_AND].StackTop			= 24;
	s_PrecedenceTable[RUL_BIT_AND].Incoming			= 23;
	s_PrecedenceTable[RUL_BIT_AND].n				= 2;


// ==, !=
	s_PrecedenceTable[RUL_NOT_EQ].StackTop			= 26;
	s_PrecedenceTable[RUL_NOT_EQ].Incoming			= 25;
	s_PrecedenceTable[RUL_NOT_EQ].n					= 2;

	s_PrecedenceTable[RUL_EQ].StackTop				= 26;
	s_PrecedenceTable[RUL_EQ].Incoming				= 25;
	s_PrecedenceTable[RUL_EQ].n						= 2;


// >, <, >=, <=
	s_PrecedenceTable[RUL_LT].StackTop				= 28;
	s_PrecedenceTable[RUL_LT].Incoming				= 27;
	s_PrecedenceTable[RUL_LT].n						= 2;

	s_PrecedenceTable[RUL_GT].StackTop				= 28;
	s_PrecedenceTable[RUL_GT].Incoming				= 27;
	s_PrecedenceTable[RUL_GT].n						= 2;

	s_PrecedenceTable[RUL_GE ].StackTop				= 28;
	s_PrecedenceTable[RUL_GE ].Incoming				= 27;
	s_PrecedenceTable[RUL_GE ].n					= 2;

	s_PrecedenceTable[RUL_LE].StackTop				= 28;
	s_PrecedenceTable[RUL_LE].Incoming				= 27;
	s_PrecedenceTable[RUL_LE].n						= 2;


// >>, <<
	s_PrecedenceTable[RUL_BIT_RSHIFT].StackTop		= 30;
	s_PrecedenceTable[RUL_BIT_RSHIFT].Incoming		= 29;
	s_PrecedenceTable[RUL_BIT_RSHIFT].n				= 2;

	s_PrecedenceTable[RUL_BIT_LSHIFT].StackTop		= 30;
	s_PrecedenceTable[RUL_BIT_LSHIFT].Incoming		= 29;
	s_PrecedenceTable[RUL_BIT_LSHIFT].n				= 2;


// +, -
	s_PrecedenceTable[RUL_PLUS].StackTop			= 32;
	s_PrecedenceTable[RUL_PLUS].Incoming			= 31;
	s_PrecedenceTable[RUL_PLUS].n					= 2;

	s_PrecedenceTable[RUL_MINUS].StackTop			= 32;
	s_PrecedenceTable[RUL_MINUS].Incoming			= 31;
	s_PrecedenceTable[RUL_MINUS].n					= 2;

	
// *, /, %
	s_PrecedenceTable[RUL_MUL].StackTop				= 34;
	s_PrecedenceTable[RUL_MUL].Incoming				= 33;
	s_PrecedenceTable[RUL_MUL].n					= 2;

	s_PrecedenceTable[RUL_DIV].StackTop				= 34;
	s_PrecedenceTable[RUL_DIV].Incoming				= 33;
	s_PrecedenceTable[RUL_DIV].n					= 2;

	s_PrecedenceTable[RUL_MOD].StackTop				= 34;
	s_PrecedenceTable[RUL_MOD].Incoming				= 33;
	s_PrecedenceTable[RUL_MOD].n					= 2;

// +, -, ++, --, !, ~
	s_PrecedenceTable[RUL_UPLUS].StackTop			= 35;
	s_PrecedenceTable[RUL_UPLUS].Incoming			= 36;
	s_PrecedenceTable[RUL_UPLUS].n					= 1;

	s_PrecedenceTable[RUL_UMINUS].StackTop			= 35;
	s_PrecedenceTable[RUL_UMINUS].Incoming			= 36;
	s_PrecedenceTable[RUL_UMINUS].n					= 1;

	s_PrecedenceTable[RUL_PLUS_PLUS].StackTop		= 35;
	s_PrecedenceTable[RUL_PLUS_PLUS].Incoming		= 36;
	s_PrecedenceTable[RUL_PLUS_PLUS].n				= 1;

	s_PrecedenceTable[RUL_MINUS_MINUS].StackTop		= 35;
	s_PrecedenceTable[RUL_MINUS_MINUS].Incoming		= 36;
	s_PrecedenceTable[RUL_MINUS_MINUS].n			= 1;

	s_PrecedenceTable[RUL_PRE_PLUS_PLUS].StackTop	= 35;
	s_PrecedenceTable[RUL_PRE_PLUS_PLUS].Incoming	= 36;
	s_PrecedenceTable[RUL_PRE_PLUS_PLUS].n			= 1;

	s_PrecedenceTable[RUL_PRE_MINUS_MINUS].StackTop	= 35;
	s_PrecedenceTable[RUL_PRE_MINUS_MINUS].Incoming	= 36;
	s_PrecedenceTable[RUL_PRE_MINUS_MINUS].n		= 1;

	s_PrecedenceTable[RUL_LOGIC_NOT].StackTop		= 35;
	s_PrecedenceTable[RUL_LOGIC_NOT].Incoming		= 36;
	s_PrecedenceTable[RUL_LOGIC_NOT].n				= 1;

	s_PrecedenceTable[RUL_BIT_NOT].StackTop			= 35;
	s_PrecedenceTable[RUL_BIT_NOT].Incoming			= 36;
	s_PrecedenceTable[RUL_BIT_NOT].n				= 1;


// Parenthesis and Brackets
	s_PrecedenceTable[RUL_LPAREN].StackTop			= 0;
	s_PrecedenceTable[RUL_LPAREN].Incoming			= 37;
	s_PrecedenceTable[RUL_LPAREN].n					= 0;
	
	s_PrecedenceTable[RUL_RPAREN].StackTop			= 38;
	s_PrecedenceTable[RUL_RPAREN].Incoming			= 0;
	s_PrecedenceTable[RUL_RPAREN].n					= 1;
	s_PrecedenceTable[RUL_RPAREN].t					= 1;

	s_PrecedenceTable[RUL_LBOX].StackTop			= 0;
	s_PrecedenceTable[RUL_LBOX].Incoming			= 37;
	s_PrecedenceTable[RUL_LBOX].n					= 0;
	
	s_PrecedenceTable[RUL_RBOX].StackTop			= 38;
	s_PrecedenceTable[RUL_RBOX].Incoming			= 0;
	s_PrecedenceTable[RUL_RBOX].n					= 1;
	s_PrecedenceTable[RUL_RBOX].t					= 1;


// Declarations
	s_PrecedenceTable[RUL_CHAR_DECL].StackTop		= 40;
	s_PrecedenceTable[RUL_CHAR_DECL].Incoming		= 39;
	s_PrecedenceTable[RUL_CHAR_DECL].n				= 0;
	// ** Walt EPM 08sep08
	s_PrecedenceTable[RUL_UNSIGNED_INTEGER_DECL].StackTop	= 40;
	s_PrecedenceTable[RUL_UNSIGNED_INTEGER_DECL].Incoming	= 39;
	s_PrecedenceTable[RUL_UNSIGNED_INTEGER_DECL].n			= 0;
	// ** end Walt EPM 08sep08
	s_PrecedenceTable[RUL_INTEGER_DECL].StackTop	= 40;
	s_PrecedenceTable[RUL_INTEGER_DECL].Incoming	= 39;
	s_PrecedenceTable[RUL_INTEGER_DECL].n			= 0;

	s_PrecedenceTable[RUL_LONG_DECL].StackTop		= 40;
	s_PrecedenceTable[RUL_LONG_DECL].Incoming		= 39;
	s_PrecedenceTable[RUL_LONG_DECL].n				= 0;

	s_PrecedenceTable[RUL_LONG_LONG_DECL].StackTop		= 40;
	s_PrecedenceTable[RUL_LONG_LONG_DECL].Incoming		= 39;
	s_PrecedenceTable[RUL_LONG_LONG_DECL].n				= 0;
	// ** Walt EPM 08sep08
	s_PrecedenceTable[RUL_UNSIGNED_SHORT_INTEGER_DECL].StackTop	= 40;
	s_PrecedenceTable[RUL_UNSIGNED_SHORT_INTEGER_DECL].Incoming	= 39;
	s_PrecedenceTable[RUL_UNSIGNED_SHORT_INTEGER_DECL].n			= 0;
	// ** end Walt EPM 08sep08
	s_PrecedenceTable[RUL_SHORT_INTEGER_DECL].StackTop	= 40;
	s_PrecedenceTable[RUL_SHORT_INTEGER_DECL].Incoming	= 39;
	s_PrecedenceTable[RUL_SHORT_INTEGER_DECL].n			= 0;

	s_PrecedenceTable[RUL_REAL_DECL].StackTop		= 40;
	s_PrecedenceTable[RUL_REAL_DECL].Incoming		= 39;
	s_PrecedenceTable[RUL_REAL_DECL].n				= 0;

	s_PrecedenceTable[RUL_DOUBLE_DECL].StackTop		= 40;
	s_PrecedenceTable[RUL_DOUBLE_DECL].Incoming		= 39;
	s_PrecedenceTable[RUL_DOUBLE_DECL].n			= 0;

	s_PrecedenceTable[RUL_BOOLEAN_DECL].StackTop	= 40;
	s_PrecedenceTable[RUL_BOOLEAN_DECL].Incoming	= 39;
	s_PrecedenceTable[RUL_BOOLEAN_DECL].n			= 0;

	s_PrecedenceTable[RUL_STRING_DECL].StackTop		= 40;
	s_PrecedenceTable[RUL_STRING_DECL].Incoming		= 39;
	s_PrecedenceTable[RUL_STRING_DECL].n			= 0;

	s_PrecedenceTable[RUL_CHAR_CONSTANT].StackTop	= 40;
	s_PrecedenceTable[RUL_CHAR_CONSTANT].Incoming	= 39;
	s_PrecedenceTable[RUL_CHAR_CONSTANT].n			= 0;

	s_PrecedenceTable[RUL_INT_CONSTANT].StackTop	= 40;
	s_PrecedenceTable[RUL_INT_CONSTANT].Incoming	= 39;
	s_PrecedenceTable[RUL_INT_CONSTANT].n			= 0;

	s_PrecedenceTable[RUL_BOOL_CONSTANT].StackTop	= 40;
	s_PrecedenceTable[RUL_BOOL_CONSTANT].Incoming	= 39;
	s_PrecedenceTable[RUL_BOOL_CONSTANT].n			= 0;

	s_PrecedenceTable[RUL_REAL_CONSTANT].StackTop	= 40;
	s_PrecedenceTable[RUL_REAL_CONSTANT].Incoming	= 39;
	s_PrecedenceTable[RUL_REAL_CONSTANT].n			= 0;

	s_PrecedenceTable[RUL_STRING_CONSTANT].StackTop	= 40;
	s_PrecedenceTable[RUL_STRING_CONSTANT].Incoming	= 39;
	s_PrecedenceTable[RUL_STRING_CONSTANT].n		= 0;

	s_PrecedenceTable[RUL_CHAR_CONSTANT].StackTop	= 40;
	s_PrecedenceTable[RUL_CHAR_CONSTANT].Incoming	= 39;
	s_PrecedenceTable[RUL_CHAR_CONSTANT].n			= 0;

	s_PrecedenceTable[RUL_SERVICE_ATTRIBUTE].StackTop		= 40;
	s_PrecedenceTable[RUL_SERVICE_ATTRIBUTE].Incoming		= 39;
	s_PrecedenceTable[RUL_SERVICE_ATTRIBUTE].n				= 0;

	//Added By Anil June 16 2005 --starts here
	s_PrecedenceTable[RUL_DD_STRING_DECL].StackTop		= 40;
	s_PrecedenceTable[RUL_DD_STRING_DECL].Incoming		= 39;
	s_PrecedenceTable[RUL_DD_STRING_DECL].n				= 0;
	//Added By Anil June 16 2005 --Ends here

	//Anil August 26 2005 For handling DD variable and Expression
	s_PrecedenceTable[RUL_DD_SIMPLE].StackTop		= 40;
	s_PrecedenceTable[RUL_DD_SIMPLE].Incoming		= 39;
	s_PrecedenceTable[RUL_DD_SIMPLE].n				= 0;

	s_PrecedenceTable[RUL_DD_COMPLEX].StackTop		= 40;
	s_PrecedenceTable[RUL_DD_COMPLEX].Incoming		= 39;
	s_PrecedenceTable[RUL_DD_COMPLEX].n				= 0;

	s_PrecedenceTable[RUL_DD_METHOD].StackTop		= 40;
	s_PrecedenceTable[RUL_DD_METHOD].Incoming		= 39;
	s_PrecedenceTable[RUL_DD_METHOD].n				= 0;

		//Added By Anil June 16 2005 --starts here
	s_PrecedenceTable[RUL_UNSIGNED_CHAR_DECL].StackTop		= 40;
	s_PrecedenceTable[RUL_UNSIGNED_CHAR_DECL].Incoming		= 39;
	s_PrecedenceTable[RUL_UNSIGNED_CHAR_DECL].n				= 0;
	//Added By Anil June 16 2005 --Ends here


}

CExpParser::CExpParser():m_Dollar("$",RUL_SYMBOL,RUL_DOLLAR,-1)
{
	CToken* pToken = new CToken("$",RUL_SYMBOL,RUL_DOLLAR,-1);
	m_Terminals.push(pToken);
}

CExpParser::~CExpParser()
{
	EmptyStacks(true,false);
}

CExpression* CExpParser::ParseExpression(
		CLexicalAnalyzer* plexAnal, 
		CSymbolTable* pSymbolTable,
		STMT_EXPR_TYPE expr,
		ERROR_VEC*	pvecErrors,
		bool bLookForQMark)
{
	CToken* pToken=0;
	CToken* pNextToken=0;
	static bool bProcessingQMark = false;

	if (bLookForQMark && !bProcessingQMark)
	{
		if (plexAnal->ScanLineForToken(RUL_SYMBOL, RUL_QMARK, &pToken))
		{
			DELETE_PTR(pToken);
			if (
			plexAnal->ScanLineForToken(RUL_SYMBOL, RUL_COLON, &pToken)
							)
			{
				DELETE_PTR(pToken);
				IFExpression *pIfExpression = new IFExpression;
				bProcessingQMark = true;
				pIfExpression->CreateParseSubTree(plexAnal, pSymbolTable, pvecErrors);
				bProcessingQMark = false;
				return pIfExpression;
			}
		}
	}

	_INT32	i32BrackCount=0;
	bool	bIsShiftOver = false;
	int		nLastTokenState = LEX_FAIL;  // ** Walt EPM 08sep08
	
	try
	{
		if((expr==EXPR_IF) || (expr==EXPR_WHILE) || (expr==EXPR_FOR))
		{
			i32BrackCount++; //we count the lparen now;
		}
		while( (!bIsShiftOver || (m_Terminals.size() > 1))
			&& (LEX_FAIL != (nLastTokenState=plexAnal->GetNextToken(&pNextToken,pSymbolTable)))// ** Walt EPM 08sep08
			//&& (LEX_FAIL != plexAnal->GetNextToken(&pNextToken,pSymbolTable))
			&& pNextToken
			)
		{
			if(pNextToken->GetSubType() == RUL_LPAREN)
			{
				i32BrackCount++;
			}
			else if(pNextToken->GetSubType() == RUL_RPAREN)
			{
				i32BrackCount--;
			}
/*			if (pNextToken->GetSubType() == RUL_COLON)
			{
				while (!m_Terminals.empty())
					m_Terminals.pop();
				break;
			}*/

			if(0==(pToken = GetActualToken(
					plexAnal,
					pSymbolTable,
					expr,
					i32BrackCount,
					&pNextToken,
					bIsShiftOver)))
			{
				DELETE_PTR(pNextToken);
				continue;
			}
	// OK. we are in the loop only if
	//a.	the expr is an assign and the Token is not a ;
	//b.	the expr is an if and the the paren count is non-zero
	//c.	the expr is an while and the the paren count is non-zero
#ifdef _DEBUG
			CToken* pTerminalTop = m_Terminals.top();
#endif
			if((pToken->GetSubType() == RUL_DOLLAR)
				&& (m_Terminals.top()->GetSubType() == RUL_DOLLAR))
			{
				break;
			}

			/* VMKP added on 030404 */
			/* Fixed the problem in Low Pressure and High Pressure
			  issue in 0x3e Manufacturer DD */
			if((pToken->GetSubType() == RUL_LPAREN)
//				&& (m_NonTerminals.top()->GetSubType() == RUL_LPAREN)
				&& (m_NonTerminals.size() > 0)
				&& ( m_Terminals.size() < 2)
				&& 	i32BrackCount == 2)
			{
				break;
			}
			/* VMKP added on 030404 */
#ifdef _DEBUG
			pTerminalTop = m_Terminals.top();
#endif
			if( (IncomingTokenPrecedence(pToken->GetSubType()) >= StackTopTokenPrecedence((m_Terminals.top())->GetSubType()))			)
			{
				m_Terminals.push(pToken);
			}
			else// if(IncomingTokenPrecedence(pToken->GetSubType()) < StackTopTokenPrecedence((m_Terminals.top())->GetSubType()))
			{
				RUL_TOKEN_SUBTYPE SubType=RUL_SUBTYPE_NONE;
				
				Reduce(SubType,pSymbolTable,plexAnal);//Anil August 26 2005 For handling DD variable and Expression
#ifdef _DEBUG
				pTerminalTop = m_Terminals.top();
#endif
				while(IncomingTokenPrecedence(SubType) <= StackTopTokenPrecedence((m_Terminals.top())->GetSubType()))
				{
					if(m_Terminals.top()->GetSubType() == RUL_DOLLAR)
					{
						break;
					}
					SubType = m_Terminals.top()->GetSubType();
					delete m_Terminals.top();
					m_Terminals.pop();
				}

				if(pNextToken->GetSubType() == RUL_RPAREN)
				{
					i32BrackCount++;
				}
				plexAnal->UnGetToken();
				DELETE_PTR(pNextToken);
			}
			pNextToken = 0;
		}

		DELETE_PTR(pNextToken);
		plexAnal->UnGetToken();
		if( nLastTokenState == LEX_FAIL )    // ** Walt EPM 08sep08
		{
			throw (_UINT32)C_EP_ERROR_LEXERROR;
		}

		if(m_Terminals.size() > 1)
		{
			throw (_UINT32)C_EP_ERROR_MISSINGSC;
		}

		if((expr!=EXPR_IF) && (expr!=EXPR_WHILE) && (expr!=EXPR_FOR))
		{
			if(i32BrackCount)
			{
				throw (_UINT32)C_EP_ERROR_MISSINGPAREN;
			}
		}

/*	Walt:EPM when m_NonTerminals[0] == 0  then the call m_NonTerminals.top() will blow up;
		if(m_NonTerminals.top())
		{
			if((m_NonTerminals.size() > 1) && (expr != EXPR_FOR) )
				if (expr != EXPR_ASSIGN)
					throw (_UINT32)C_EP_ERROR_MISSINGOP;

			return m_NonTerminals.top();
		}
	Replace with below::>
*/		 
		if( m_NonTerminals.size() > 0 )
		{
			if(m_NonTerminals.top())
			{
				if((m_NonTerminals.size() > 1) && (expr != EXPR_FOR) )
				{
					if ((expr != EXPR_ASSIGN) && (expr != EXPR_WHILE))// emerson checkin april2013
					{
						throw (_UINT32)C_EP_ERROR_MISSINGOP;
					}
				}
 
				return m_NonTerminals.top();
			}
		}
		else
		{
			throw (_UINT32)C_EP_ERROR_MISSINGPAREN;
		}
	}
	catch(_UINT32 error)
	{
		if( error == C_EP_ERROR_LEXERROR )// ** Walt EPM 08sep08
		{
		}
		else
		{
		//clean up the terminal stack and the non-terminal stack
		plexAnal->UnGetToken();
		EmptyStacks(true,true);
		DELETE_PTR(pNextToken);
		}
		throw(error);
	}
	catch(...)
	{
		//clean up the terminal stack and the non-terminal stack
		EmptyStacks(true,true);
		DELETE_PTR(pNextToken);
		throw(C_EP_ERROR_UNKNOWN);
	}
	return 0;
}

//This consists of the following steps
//a.	Pop the Terminals Stack
//b.	Look at the terminal and pop the necessary NonTerminals and terminals
//		out of the NT stack
//c.	Create a CExpression object with the appropriate NonTerminals
//		and push it into the NonTerminals stack
#ifdef MODIFIED_BY_SS
//commented out by srilatha...because some of the variables are undefined.
void CExpParser::Reduce(
		RUL_TOKEN_SUBTYPE& SubType,
		CSymbolTable* pSymbolTable,
		CLexicalAnalyzer* plexAnal//Anil August 26 2005 For handling DD variable and Expression
		)
{
	CExpression* pExpression=0;
	CToken* pToken = m_Terminals.top();
	_INT32 nNonTerminals = s_PrecedenceTable[pToken->GetSubType()].n;
	return;
//	try
//	{
//		SubType = pToken->GetSubType();
//		m_Terminals.pop();
//
//#ifdef _FULL_RULE_ENGINE
//		if(pToken->IsVariable() || pToken->IsNumeric() || pToken->IsConstant())
//		{
//			pExpression = new CPrimaryExpression(pToken);
//		}
//#else
//		if(pToken->IsVariable())
//		{
//			if(g_PointDatabase->IsHWPoint(pToken->GetLexeme()) == DCAP_HW_POINT)
//			{
//				pExpression = new CPrimaryExpression(pToken);
//				//pSymbolTable->Insert(*pToken);
//
//				_INT32 i32Idx = pSymbolTable->Insert(*pToken);
//				if( i32Idx >= 0 )
//                {
//					pToken->SetSymbolTableIndex(i32Idx);
//					pToken->SetSubType(pSymbolTable->GetAt(i32Idx)->GetSubType());
//				}
//				else	// emerson checkin april2013
//                {
//                    LOGIT(CERR_LOG,"Unable to insert variable into symbol table\n");
//                }
//                
//                
//			}
//			else
//			{
//				_CHAR *pszEq;
//				CLexicalAnalyzer LexAnalyzer;
//				CExpParser expParser;
//				
//				// have to do the error checking
//				g_PointDatabase->GetPointEq(pToken->GetLexeme(), &pszEq);
//				LexAnalyzer.Load((_UCHAR*)pszEq);
//				pExpression = expParser.ParseExpression(&LexAnalyzer, pSymbolTable, EXPR_NONE);
//			}
//
//		}
//		else if(pToken->IsNumeric() || pToken->IsConstant())
//		{
//			pExpression = new CPrimaryExpression(pToken);
//		}
//#endif
//		else if(pToken->IsArrayVar())
//		{
//			pExpression = new CArrayExpression(pToken);
//		}
//		//Added By Anil August 4 2005 --starts here
//		//If it is DD item then Form the Expression as ComplexDDExpression
//		else if(pToken->IsDDItem())
//		{
//			pExpression = new CComplexDDExpression(pToken);
//			InsertDDExpr(pToken->GetLexeme(),pExpression,pSymbolTable,plexAnal,pToken);//Anil Octobet 5 2005 for handling Method Calling Method
//		}
//		//Added By Anil August 4 2005 --Ends here
//		else	//operator
//		{
//			CExpression* pNTExpression1=0;
//			CExpression* pNTExpression2=0;
//			switch(nNonTerminals)
//			{
//			case 2:
//				pNTExpression1 = m_NonTerminals.top();
//				m_NonTerminals.pop();
//				pNTExpression2 = m_NonTerminals.top();
//				m_NonTerminals.pop();
//
//				pExpression = new CCompoundExpression(pNTExpression2,pNTExpression1,pToken->GetSubType());
//				break;
//			case 1:
//				if(RUL_RBOX != pToken->GetSubType())
//				{
//					pNTExpression1 = m_NonTerminals.top();
//					m_NonTerminals.pop();
//
//					pExpression = new CCompoundExpression(pNTExpression1,0,pToken->GetSubType());
//				}
//
//#ifdef _FULL_RULE_ENGINE
//				else
//				{
//					pNTExpression1 = m_NonTerminals.top();	//array dim expr
//					m_NonTerminals.pop();
//
//					pNTExpression2 = m_NonTerminals.top();	//array identifier
//					m_NonTerminals.pop();
//
//					((CArrayExpression*)pNTExpression2)->AddDimensionExpr(pNTExpression1);
//					pExpression = pNTExpression2;
//				}
//#endif
//				break;
//			case 0:
//				pExpression = 0;
//				break;
//			}
//
//			DELETE_PTR(pToken);
//			if(s_PrecedenceTable[m_Terminals.top()->GetSubType()].t)	
//			{	//for the moment this is '(' or '['
//				pToken = m_Terminals.top();
//				SubType = pToken->GetSubType();
//				m_Terminals.pop();
//				DELETE_PTR(pToken);
//			}
//		}
//
//		if(pExpression)
//		{
//			m_NonTerminals.push(pExpression);
//		}
//	}
//	catch(...)
//	{
//		DELETE_PTR(pExpression);
//		throw;
//	}
}
#endif
_INT32 CExpParser::StackTopTokenPrecedence(
			RUL_TOKEN_SUBTYPE SubType)
{
	PRECEDENCE_TABLE::iterator myIt;
	myIt = s_PrecedenceTable.find(SubType);
	if(myIt == s_PrecedenceTable.end())
		throw (_UINT32)C_EP_ERROR_ILLEGALOP;

	return s_PrecedenceTable[SubType].StackTop;
}

_INT32	CExpParser::IncomingTokenPrecedence(
			RUL_TOKEN_SUBTYPE SubType)
{
	PRECEDENCE_TABLE::iterator myIt;
	myIt = s_PrecedenceTable.find(SubType);
	if(myIt == s_PrecedenceTable.end())
		throw (_UINT32)C_EP_ERROR_ILLEGALOP;

	return s_PrecedenceTable[SubType].Incoming;
}

void	CExpParser::EmptyStacks(
				bool IsTerminal, 
				bool IsNonTerminal)
{
	if(IsTerminal)
	{
		CToken* pToken=0;
		while(!m_Terminals.empty())
		{
			pToken = m_Terminals.top();
			delete pToken;
			pToken = 0;

			m_Terminals.pop();
		}
	}
	if(IsNonTerminal)
	{
		CExpression* pExp=0;
		while(!m_NonTerminals.empty())
		{
			pExp = m_NonTerminals.top();
			delete pExp;
			pExp = 0;

			m_NonTerminals.pop();
		}
	}
}

CToken*	CExpParser::GetActualToken(
		CLexicalAnalyzer*	plexAnal,
		CSymbolTable*		pSymbolTable,
		STMT_EXPR_TYPE		expr,
		_INT32				i32BrackCount,
		CToken**			ppNextToken,
		bool&			bIsShiftOver)
{
	CToken* pToken = 0;
	if(		((expr==EXPR_LVALUE) && (*ppNextToken)->IsAssignOp ())
		||	((expr==EXPR_ASSIGN) && (*ppNextToken)->IsEOS())
		||	((expr==EXPR_IF)	 && !i32BrackCount)
		||	((expr==EXPR_WHILE)	 && !i32BrackCount)
		||	((expr==EXPR_WHILE)	 && ((*ppNextToken)->GetSubType() == RUL_COMMA))
		||	((expr==EXPR_ASSIGN) && ((*ppNextToken)->GetSubType() == RUL_QMARK))
		||	((expr==EXPR_ASSIGN) && ((*ppNextToken)->GetSubType() == RUL_COLON))
		||	((expr==EXPR_FOR)	 && !i32BrackCount)
		||	((expr==EXPR_FOR)	 && ((*ppNextToken)->GetSubType() == RUL_SEMICOLON))
		||  ((expr==EXPR_CASE) && ((*ppNextToken)->GetSubType() == RUL_COLON))
		)
	{
		bIsShiftOver = true;
		pToken = &m_Dollar;
	}
	else
	{

#ifdef _FULL_RULE_ENGINE

//If the expression contains an OM expression, then
//1.	Parse it using the COMServiceExpression
//2.	Push it into NT stack(ie we are by-passing the reduxion to Expression).
		if((*ppNextToken)->IsOMToken())
		{
			DELETE_PTR(*ppNextToken);
			plexAnal->UnGetToken();

			COMServiceExpression* pExpression=0;
			pExpression = new COMServiceExpression;
			pExpression->CreateParseSubTree(plexAnal,pSymbolTable);
		
			m_NonTerminals.push(pExpression);

		}
		else
		if((*ppNextToken)->IsFunctionToken())
		{
			DELETE_PTR(*ppNextToken);
			plexAnal->UnGetToken();

			FunctionExpression* pExpression=0;
			pExpression = new FunctionExpression;
			pExpression->CreateParseSubTree(plexAnal,pSymbolTable);
		
			m_NonTerminals.push(pExpression);

		}
		else
		{
			pToken = *ppNextToken;
		}
#else
		pToken = *ppNextToken;
#endif
	}

	return pToken;
}


//Anil August 26 2005 For handling DD variable and Expression
//This Function Inserts any Expression within  [], in to the main DDcomplex Expression
#ifdef MODIFIED_BY_SS
//bool CExpParser::InsertDDExpr(const char* pszComplexDDExpre,CExpression* DDArrayExpression,CSymbolTable* pSymbolTable,CLexicalAnalyzer* plexAnal,CToken* pToken)
//{
//	
//	CExpParser expParser;	
//	
//	if( (RUL_DD_ITEM == pToken->GetType()) && ( RUL_DD_METHOD == pToken->GetSubType() ))
//	{
//		// ** Walt EPM 08sep08
//		int nSizeOfBuffer = strlen(pszComplexDDExpre)+1;
//		char* pchDecSource = new char[nSizeOfBuffer];   //Allocate enough memory for us to work with.
//		memset(pchDecSource,0,nSizeOfBuffer);	//make sure to clear it	
//		// ** end  Walt EPM 08sep08
//		int iLeftPeranthis = 0;
//		bool bValidMethodCall = false;
//		long int i = strlen(pToken->GetDDItemName());
//
//		for(; i < (long)strlen(pszComplexDDExpre); i++) // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
//
//		{
//			if(pszComplexDDExpre[i] == '(')
//			{
//				bValidMethodCall = true;
//				iLeftPeranthis =1;
//				i++;
//				break;
//			}
//		}
//		if( bValidMethodCall ==  false)
//		{
//			return false;
//		}		
//
//		int iNoOfchar = 0;
//		long int lstlen = strlen(pszComplexDDExpre);
//		for(; i< (lstlen) ; i++)
//		{
//			if( (' ' != pszComplexDDExpre[i]))				
//			{
//				pchDecSource[iNoOfchar] = pszComplexDDExpre[i];// ** Walt EPM 08sep08
//				iNoOfchar++ ;
//				pchDecSource[iNoOfchar] = 0;// ** Walt EPM 08sep08
//			}			
//			if(')' == pszComplexDDExpre[i])
//			{
//				iLeftPeranthis--;
//
//			}
//			if('(' == pszComplexDDExpre[i])
//			{
//				iLeftPeranthis++;
//
//			}
//			if(((pszComplexDDExpre[i] == ',') || (0 == iLeftPeranthis) ) && (iNoOfchar>1))
//			{
//				//do insert here
//				/*** Walt EPM 08sep08				
//				int istartPosOfPassedItem = 0 ;
//				iNoOfchar--;//Because ; or ) is included
//				int iNoOfSpaces = 0;
//				for(int x = i-1; ;x--)
//				{
//					if(' ' == pszComplexDDExpre[x])
//					{
//						iNoOfSpaces++;
//
//					}
//					else
//					{
//						break;
//					}
//
//				}
//				istartPosOfPassedItem = i - iNoOfchar - iNoOfSpaces ;
//				
//				int iCount = iNoOfchar + 1 + 1;
//				char* pchDecSource = new char[ iCount ];// +1 for Null Char +1 for ; -1 for as it had counted ]
//				memset(pchDecSource,0,iCount);				
//				strncpy(pchDecSource,(const char*)&pszComplexDDExpre[istartPosOfPassedItem],iNoOfchar);
//				strcat(pchDecSource, ";");
//				pchDecSource[iCount - 1] = '\0';
//				*** end Walt EPM 08sep08 */
//				
//				pchDecSource[iNoOfchar-1]=';';//last character may be a ")" or a ";" // Walt EPM 08sep08
//				
//				//Form this as new Lexical, and load it in to it
//				CLexicalAnalyzer	clexAnalTemp;
//				clexAnalTemp.InitMeeInterface(plexAnal->GetMEEInterface());
//				clexAnalTemp.Load((_UCHAR*)pchDecSource,(unsigned char*)"test",0);
//				//Call Parse Expression to get the Expression class
//				CExpression* pExpression = expParser.ParseExpression(&clexAnalTemp,pSymbolTable,EXPR_ASSIGN);
//				//Insert this exp in to the main DDComplex expression class
//				if( pExpression )
//				{
//				((CComplexDDExpression*)DDArrayExpression)->AddDimensionExpr(pExpression);
//				/* Walt EPM 08sep08 - moved below
//				if(pchDecSource)
//				{
//					delete[] pchDecSource;
//					pchDecSource = NULL;
//				}
//				*** end  Walt EPM 08sep08 - moved */
//				iNoOfchar = 0;
//			}
//
//			}
//			if(0 == iLeftPeranthis)
//			{
//				break;
//			}
//		}//end of for loop
//		// ** Walt EPM 08sep08 moved to here
//		if(pchDecSource)
//		{
//			delete[] pchDecSource;
//			pchDecSource = NULL;
//		}
//
//	}
//	else
//	{
//		
//	for(long int i = 0; i < (long)strlen(pszComplexDDExpre); i++) 	// warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
//	{
//		if(pszComplexDDExpre[i] == '[')
//		{
//			i++;
//			int iPos = i;
//			int iLeftBrackCount = 1;
//			int iCount = 0;
//			while( (iLeftBrackCount!=0) && (i < (int)strlen(pszComplexDDExpre)) )  // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
//			{
//				if(pszComplexDDExpre[i] == '[')
//					iLeftBrackCount++;
//				if(pszComplexDDExpre[i] == ']')
//					iLeftBrackCount--;
//				i++;
//				iCount++;				
//			}
//			i--; // stevev - 9-22-10 - it is missing the second(and subsequent)'[' in array[2][3]
//			//Get the Expression within the []
//			if(iCount>0)
//			{
//				char* pchDecSource = new char[iCount- 1 + 1 + 1];// +1 for Null Char +1 for ; -1 for as it had counted ]
//				memset(pchDecSource,0,iCount + 1);				
//				strncpy(pchDecSource,(const char*)&pszComplexDDExpre[iPos],iCount-1);
//				strcat(pchDecSource, ";");
//				pchDecSource[iCount] = '\0';
//				//Form this as new Lexical, and load it in to it
//				CLexicalAnalyzer	clexAnalTemp;
//				clexAnalTemp.InitMeeInterface(plexAnal->GetMEEInterface());
//				clexAnalTemp.Load((_UCHAR*)pchDecSource,(unsigned char*)"test",0);
//				//Call Parse Expression to get the Expression class
//				CExpression* pExpression = expParser.ParseExpression(&clexAnalTemp,pSymbolTable,EXPR_ASSIGN);
//				//Insert this exp in to the main DDComplex expression class
//				((CComplexDDExpression*)DDArrayExpression)->AddDimensionExpr(pExpression);
//				if(pchDecSource)
//				{
//					delete[] pchDecSource;
//					pchDecSource = NULL;
//				}
//				
//			}
//
//			}
//		}
//	}
//
//	return true;
//
//	
//
//}
#endif