
//#include "stdafx.h"
#include "pch.h"
#include "Program.h"
#include "Declarations.h"
#include "StatementList.h"
#include "SymbolTable.h"
#include "GrammarNodeVisitor.h"

//#include "RIDEError.h"
#include "ErrorDefinitions.h"
#include "SynchronisationSet.h"

CProgram::CProgram()
{
	m_pDeclarations = 0;
	m_pStmtList = 0;
}

CProgram::~CProgram()
{
	DELETE_PTR(m_pDeclarations);
	DELETE_PTR(m_pStmtList);
}
#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
//CJK DP GETS INTO HERE REDO from CPARSEr execute
_INT32 CProgram::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	_INT32 nReturn =  pVisitor->visitProgram(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
	if(nReturn == VISIT_ERROR)// emerson april2013
	{
		throw(C_UM_ERROR_INTERNALERR);
	}
	return nReturn;
}
#endif
#endif
_INT32 CProgram::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
	_INT32 i32Ret = 0;
	CToken* pToken=0;
	try
	{
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			||(pToken->GetSubType() != RUL_LBRACK))
		{
			DELETE_PTR(pToken);//clean up memory even on errors
			throw(C_UM_ERROR_INTERNALERR);
		}
		DELETE_PTR(pToken);

		m_pDeclarations	= new CDeclarations;
		i32Ret = m_pDeclarations->CreateParseSubTree(
			plexAnal,
			pSymbolTable,
			pvecErrors);

		m_pStmtList = new CStatementList;
		i32Ret = m_pStmtList->CreateParseSubTree(
			plexAnal,
			pSymbolTable,
			pvecErrors);

		if (PARSE_FAIL == i32Ret)
		{
			if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
				|| !pToken
				||(pToken->GetSubType() != RUL_RBRACK))
			{
				
				DELETE_PTR(pToken);//clean up memory, even on errors
				throw(C_UM_ERROR_INTERNALERR);
				return i32Ret;
			}
			else
			{
#ifdef _DEBUG
				if (plexAnal->m_nSymbolTableScopeIndex != 0)
				{
					LOGIT(CERR_LOG,"Mismatched braces.\n");
				}
#endif
				DELETE_PTR(pToken);
				return PARSE_SUCCESS;			
			}
		}
	}
	catch(CRIDEError* perr)
	{
		//if nobody has bothered to catch the error till now,
		//what else can be done but to just eat it and keep quiet...
		pvecErrors->push_back(perr);
	}
	catch(...)
	{
		throw(C_UM_ERROR_UNKNOWNERROR);
	}
	return i32Ret;
}

void CProgram::Identify(
		_CHAR* szData)
{
	/*strcat(szData,"<Program>");
	m_pDeclarations->Identify(szData);
	m_pStmtList->Identify(szData);
	strcat(szData,"</Program>");*/
#ifdef MODIFIED_BY_SS
	std::string strData(szData);

	// Append "<Program>" to data
	strData.append("<Program>");

	std::copy(strData.begin(), strData.end(), szData);
	szData[strData.size()] = '\0'; // Null-terminate szData
	// Call Identify method on m_pDeclarations passing data
	m_pDeclarations->Identify(szData);

	// Call Identify method on m_pStmtList passing data
	m_pStmtList->Identify(szData);

	// Append "</Program>" to data
	strData.append("</Program>");

	// Copy data to szData
	std::copy(strData.begin(), strData.end(), szData);
	szData[strData.size()] = '\0'; // Null-terminate szData
#endif
}

CDeclarations* CProgram::GetDeclarations()
{
	return m_pDeclarations;
}

CStatementList* CProgram::GetStatementList()
{
	return m_pStmtList;
}

_INT32 CProgram::GetLineNumber()
{
	return m_pStmtList->GetLineNumber();
}
