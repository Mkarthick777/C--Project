#include "pch.h"
//#include "stdafx.h"
#include "RuleServiceStatement.h"
#include "SymbolTable.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CRuleServiceStatement::CRuleServiceStatement()
{
	m_pRuleName = 0;
}

CRuleServiceStatement::~CRuleServiceStatement()
{
	DELETE_PTR(m_pRuleName);
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CRuleServiceStatement::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitRuleService(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif

// Rule Service Statement is of the form
//	<RuleEngine> <::> <Invoke> <(> <RuleName> <)>;
_INT32 CRuleServiceStatement::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CToken* pToken=0;
	try
	{
//Munch a <RuleEngine>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			||  (RUL_KEYWORD != pToken->GetType()) 
			|| (RUL_RULE_ENGINE != pToken->GetSubType()))
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);

//Munch a <::>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (RUL_SYMBOL != pToken->GetType())
			|| RUL_SCOPE != pToken->GetSubType())
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGSCOPE);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch a <Invoke>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			||  (RUL_KEYWORD != pToken->GetType()) 
			|| (RUL_INVOKE != pToken->GetSubType()))
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGINVOKE);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch a <(>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (RUL_SYMBOL != pToken->GetType())
			|| RUL_LPAREN != pToken->GetSubType())
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGLPAREN);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

//Munch a <RuleName> -- this is a string
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (RUL_STRING_CONSTANT != pToken->GetSubType()
				&& RUL_STRING_DECL != pToken->GetSubType())
			)
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGRNAME);
			plexAnal->UnGetToken();
			DELETE_PTR(pToken);
		}
		m_pRuleName = pToken;
		pToken = 0;
		
//Munch a <)>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (RUL_SYMBOL != pToken->GetType())
			|| RUL_RPAREN != pToken->GetSubType())
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGRPAREN);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);

		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| (RUL_SEMICOLON != pToken->GetSubType()))
		{
			//ADD_ERROR(C_RS_ERROR_MISSINGSC);
			plexAnal->UnGetToken();
		}
		DELETE_PTR(pToken);
		return PARSE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
		plexAnal->MovePast(
			RUL_SYMBOL,
			RUL_SEMICOLON,
			pSymbolTable);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return PARSE_FAIL;
}

void CRuleServiceStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,szTokenSubstrings[RUL_RULE_ENGINE]);
	strcat(szData,">");
	
	if(m_pRuleName)
		m_pRuleName->Identify(szData);

	strcat(szData,"</");
	strcat(szData,szTokenSubstrings[RUL_RULE_ENGINE]);
	strcat(szData,">");*/
	std::string result(szData);

	result += "<";
	result += szTokenSubstrings[RUL_RULE_ENGINE];
	result += ">";
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string

	if (m_pRuleName)
		m_pRuleName->Identify(szData);

	result += "</";
	result += szTokenSubstrings[RUL_RULE_ENGINE];
	result += ">";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif
}

void CRuleServiceStatement::GetRuleName(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	//strcat(szData,m_pRuleName->GetLexeme());
	std::string lexeme(szData);
	lexeme += m_pRuleName->GetLexeme();

    // Copy the lexeme to szData
    std::copy(lexeme.begin(), lexeme.end(), szData);
    szData[lexeme.size()] = '\0'; // Null-terminate the string
#endif
}

_INT32 CRuleServiceStatement::GetLineNumber()
{
	return m_pRuleName->GetLineNumber();
}
