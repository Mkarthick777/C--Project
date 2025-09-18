//#include "stdafx.h"
#include "pch.h"
#include "ReturnStatement.h"
#include "SymbolTable.h"
#include "ExpParser.h"
#include "GrammarNodeVisitor.h"
#include "OMServiceExpression.h"
#include "FunctionExpression.h"

#include "ErrorDefinitions.h"
//#include "RIDEError.h"
#include "SynchronisationSet.h"

CReturnStatement::CReturnStatement()
{
	m_pExpression = NULL;//Walt:EPM 16aug07 checkin
}

CReturnStatement::~CReturnStatement()
{
	DELETE_PTR(m_pExpression);//Walt:EPM 16aug07 checkin
}
#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CReturnStatement::Execute(
			CGrammarNodeVisitor*	pVisitor,
			CSymbolTable*			pSymbolTable,
			INTER_VARIANT*		pvar,
			ERROR_VEC*				pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	//Anil Octobet 5 2005 for handling Method Calling Method
	return pVisitor->visitReturnStatement(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);
//	return VISIT_RETURN;
}
#endif
#endif

// Assignment RETURN Statement is of the form
//	<return><expression>
_INT32 CReturnStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors
			)
{
	CToken* pToken=0;
	try
	{
//Munch a <return>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken->IsRETURNStatement())
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);
//Munch a <;>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| pToken->GetSubType() == RUL_SEMICOLON)//Anil Octobet 5 2005 for handling Method Calling Method
		{
			//throw(C_UM_ERROR_INTERNALERR)//Commented by anil
			//this is the case of Void return so no need to parse the futher satement			
			m_pExpression = NULL;//Anil Octobet 5 2005 for handling Method Calling Method
			DELETE_PTR(pToken);
			return PARSE_SUCCESS;

		}
		plexAnal->UnGetToken();
		DELETE_PTR(pToken);// uncommented WS:EPM 17jul07

		//Added Anil Octobet 5 2005 for handling Method Calling Method
		//Return statement may be a Expression So Do Parse those Expression and Push it on to m_pExpression
		CExpParser expParser;
		try
		{
			m_pExpression 
				= expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_ASSIGN);

			if(!m_pExpression)
			{
				throw(C_AP_ERROR_MISSINGEXP);
			}
		}
		catch(CRIDEError* perr)
		{
			pvecErrors->push_back(perr);
			plexAnal->SynchronizeTo(EXPRESSION,pSymbolTable);
		}
		return PARSE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
		plexAnal->SynchronizeTo(EXPRESSION,pSymbolTable);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return PARSE_FAIL;
}

// Assignment BREAK Statement is of the form
//	<break><;>
_INT32 CReturnStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors,
			STATEMENT_TYPE		stmt_type
			)
{
	return PARSE_SUCCESS;
}

void CReturnStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,"BREAK");
	strcat(szData,">");
	strcat(szData,"</");
	strcat(szData,"BREAK");
	strcat(szData,">");*/

	std::string data(szData);

	// Append "<BREAK>" to data
	data.append("<BREAK>");

	// Append "</BREAK>" to data
	data.append("</BREAK>");

	// Copy the content of data to szData
	std::copy(data.begin(), data.end(), szData);
	szData[data.size()] = '\0'; // Null-terminate szData
#endif

}

_INT32 CReturnStatement::GetLineNumber()
{
	return i32LineNumber;
}

//Anil Octobet 5 2005 for handling Method Calling Method
CExpression* CReturnStatement::GetExpression()
{
	return m_pExpression;
}
