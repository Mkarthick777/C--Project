//#include "stdafx.h"

//#if _MSC_VER >= 1300  // HOMZ - port to 2003, VS7
//stevev 20feb07-merge- contents moved to ddbGeneral #include "..\DevServices\stdafx.h"   // HOMZ
//#endif
#include "pch.h"
#include "ddbGeneral.h"	// stevev 20feb07 - merge to get rid of 'stdafx.h'
/* comutil.h uses Bill's TRUE/FALSE that the general erroneously defines on purpose */
#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0

#include "IFExpression.h"
#include "GrammarNodeVisitor.h"
#include "ExpParser.h"
#include "ErrorDefinitions.h"
#if _MSC_VER < 1400	/* stevev 23feb10 - 2005 mods, these are not needed */
//#include <comdef.h>
//#include "OMServiceExpression.h"
//#include "SymbolTable.h"
#endif

IFExpression::IFExpression()
{
	m_pIfExpression = NULL;
	m_pTrueExpression = NULL;
	m_pFalseExpression = NULL;
}

IFExpression::~IFExpression()
{
	if(m_pIfExpression)
	{
		delete m_pIfExpression;
		m_pIfExpression = NULL;
	}
	if(m_pTrueExpression)
	{
		delete m_pTrueExpression;
		m_pTrueExpression = NULL;
	}
	if(m_pFalseExpression)
	{
		delete m_pFalseExpression;
		m_pFalseExpression = NULL;
	}
}
#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 IFExpression::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitIFExpression(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif
_INT32 IFExpression::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CExpParser expParser;
	CToken* pToken=0;
	try
	{
		m_pIfExpression = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_ASSIGN,NULL,false);
		if(!m_pIfExpression)
		{
			throw(C_IF_ERROR_MISSINGEXP);
		}

//Munch a <?> 
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !((pToken->GetType() == RUL_SYMBOL) && (pToken->GetSubType() == RUL_QMARK))
			)
		{
			DELETE_PTR(pToken);
			throw(C_RS_ERROR_MISSINGSC);
		}
		
		m_pTrueExpression = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_ASSIGN,NULL,false);
		if(!m_pTrueExpression)
		{
			DELETE_PTR(pToken);
			throw(C_IF_ERROR_MISSINGEXP);
		}
		DELETE_PTR(pToken);

//Munch a <:> 
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !((pToken->GetType() == RUL_SYMBOL) && (pToken->GetSubType() == RUL_COLON))
			)
		{
			DELETE_PTR(pToken);
			throw(C_RS_ERROR_MISSINGSC);
		}
		
		m_pFalseExpression = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_ASSIGN,NULL,false);//Vibhor 110205: Changed from EXPR_FOR
		if(!m_pFalseExpression)
		{
			DELETE_PTR(pToken);
			throw(C_IF_ERROR_MISSINGEXP);
		}

//Munch a <;> 
/*		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsEOS())
		{
			throw(C_RS_ERROR_MISSINGSC);
		}*/
		
		DELETE_PTR(pToken);

		return PARSE_SUCCESS;
	}
	catch (...)
	{
		return PARSE_FAIL;
	}

	return 0;
}

void IFExpression::Identify(
		_CHAR* szData)
{
}


_INT32 IFExpression::GetLineNumber()
{
	return -1;
}

