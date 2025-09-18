//#include "stdafx.h"
#include "pch.h"
#include "Declarations.h"
#include "Declaration.h"
#include "SymbolTable.h"
#include "ParserBuilder.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CDeclarations::CDeclarations()
{
}

CDeclarations::~CDeclarations()
{
	_INT32 nSize = m_declList.size();
	for(_INT32 i=0;i<nSize;i++)
	{
		delete m_declList[i];
	}
	m_declList.clear();
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CDeclarations::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitDeclarations(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif
//Look-ahead to see if the next token is a decl.
//if so, then call the CDeclaration parser to do the appropriate action.
//else terminate parsing the declarations.
_INT32 CDeclarations::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	CParserBuilder builder;
	CGrammarNode* pDecl = 0;

	try
	{
		while(0!=(pDecl = builder.CreateParser(plexAnal,STMT_DECL)))
		{
			m_declList.push_back((CDeclaration*)pDecl);
			pDecl->CreateParseSubTree(
				plexAnal,
				pSymbolTable,
				pvecErrors);
		}
		return PARSE_SUCCESS;
	}
	catch(CRIDEError* perr)
	{
		pvecErrors->push_back(perr);
		//just skip a few tokens and hope everything becomes alright.
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

void CDeclarations::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<Declarations>");
	strcat(szData,"</Declarations>");*/
	std::string result;

	result += "<Declarations>";
	result += "</Declarations>";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif
}

DECL_LIST* CDeclarations::GetDeclarations()
{
	return &m_declList;
}

_INT32 CDeclarations::GetLineNumber()
{
	return -1;
}
