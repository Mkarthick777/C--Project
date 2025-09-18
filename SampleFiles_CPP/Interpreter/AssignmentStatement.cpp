#include "pch.h"
//#include "stdafx.h"
#include "AssignmentStatement.h"
#include "SymbolTable.h"
#include "ExpParser.h"
#include "GrammarNodeVisitor.h"
#include "OMServiceExpression.h"
#include "FunctionExpression.h"

#include "ErrorDefinitions.h"
//#include "RIDEError.h"
#include "SynchronisationSet.h"

CAssignmentStatement::CAssignmentStatement()
{
	m_pVariable			= 0;
	m_pExpression		= 0;
	m_pArrayExp			= 0;
	m_pOMExp			= 0;
	m_pComplexDDExp		= 0;//Walt:EPM 16aug07 checkin
	m_bLvalueIsArray	= false;
	m_bLvalueIsComplexDD = false;//Anil August 23 2005 For Handling DD var and Expression

	SetNodeType(NODE_TYPE_ASSIGN);
}

CAssignmentStatement::~CAssignmentStatement()
{
	DELETE_PTR(m_pVariable);
	DELETE_PTR(m_pExpression);
	DELETE_PTR(m_pArrayExp);
	DELETE_PTR(m_pOMExp);
	DELETE_PTR(m_pComplexDDExp);//Walt:EPM 16aug07 checkin
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 CAssignmentStatement::Execute(
			CGrammarNodeVisitor*	pVisitor,
			CSymbolTable*			pSymbolTable,
			INTER_VARIANT*		pvar,
			ERROR_VEC*				pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitAssignment(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif
// Assignment Statement is of the form
//	<Var>		<=> <Expr>;
//	<ArrVar>	<=>	<Expr>
//	<OM>		<=>	<Expr>
_INT32 CAssignmentStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors
			)
{
	CToken* pToken=0;
	try
	{
//Munch a <Var>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken)
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		if(pToken->IsArrayVar())
		{
			plexAnal->UnGetToken();
			CExpParser expParser;
			m_pArrayExp = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_LVALUE);
			m_bLvalueIsArray = true;
			m_pVariable = 0;
			m_pOMExp	= 0;
			m_pComplexDDExp = 0;//Added By Anil August 23 2005

			DELETE_PTR(pToken);
		}
		//Added By Anil August 4 2005 --starts here
		//For Handlin the DD variable and Expressions
		else
		if( pToken->IsDDItem() )
		{
			plexAnal->UnGetToken();
			CExpParser expParser;
			m_pComplexDDExp = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_LVALUE);
			m_bLvalueIsComplexDD = true;
			m_pArrayExp = 0;
			m_pVariable = 0;
			m_pOMExp	= 0;

			DELETE_PTR(pToken);

		}
		//Added By Anil August 4 2005 --Ends here
		else 
		if(pToken->IsVariable())
		{
			m_pVariable = pToken;
			m_pArrayExp = 0;
			m_pOMExp	= 0;
			m_pComplexDDExp = 0;//Anil August 23 2005

			//DELETE_PTR(pToken);
			//todo walter
		}
		else if(pToken->IsOMToken())
		{
			//do something...
			plexAnal->UnGetToken();
			m_pOMExp = new COMServiceExpression;
			m_pOMExp->CreateParseSubTree(plexAnal,pSymbolTable);
			m_pVariable = 0;
			m_pArrayExp = 0;
			m_pComplexDDExp = 0;//Anil August 23 2005

			DELETE_PTR(pToken);
		}
		else
		{
			DELETE_PTR(pToken);
			throw(C_AP_ERROR_LVALUE);
		}

		//DELETE_PTR(pToken);  //todo walter
		pToken = 0;
//Munch a <=> or <*=>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsAssignOp())
		{
			DELETE_PTR(pToken);
			throw(C_AP_ERROR_MISSINGEQ);
		}
		m_AssignType = pToken->GetSubType();

		DELETE_PTR(pToken);

//Munch & Parse the expression.
//we got to give the expression string to the expression parser.
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

		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			 || (RUL_SEMICOLON != pToken->GetSubType()))
		{
			DELETE_PTR(pToken);
			throw(C_AP_ERROR_MISSINGSC);
		}
		DELETE_PTR(pToken);
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

// Assignment Statement is of the form
//	<Var>		<=> <Expr> <)>
//	<ArrVar>	<=>	<Expr> <)>
//	<OM>		<=>	<Expr> <)>
// To handle assignments in FOR loops
_INT32 CAssignmentStatement::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors,
			STATEMENT_TYPE		stmt_type
			)
{
	CToken* pToken=0;
	try
	{
//Munch a <Var>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken)
		{
			DELETE_PTR(pToken);
			throw(C_UM_ERROR_INTERNALERR);
		}
		if(pToken->IsVariable())
		{
			m_pVariable = pToken;
			m_pArrayExp = 0;
			m_pOMExp	= 0;
			m_pComplexDDExp = 0;//Anil August 23 2005
		}
		else if(pToken->IsArrayVar())
		{
			plexAnal->UnGetToken();
			CExpParser expParser;
			m_pArrayExp = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_LVALUE);
			m_bLvalueIsArray = true;
			m_pVariable = 0;
			m_pOMExp	= 0;
			m_pComplexDDExp = 0;//Anil August 23 2005

			DELETE_PTR(pToken);
		}
		//Added By Anil August 4 2005 --starts here
		//Handling DD variable and Expressions
		else if(pToken->IsDDItem())
		{
			plexAnal->UnGetToken();
			CExpParser expParser;
			m_pComplexDDExp = expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_LVALUE);
			m_bLvalueIsArray = true;
			m_pVariable = 0;
			m_pOMExp	= 0;
			m_pArrayExp = 0;

			DELETE_PTR(pToken);
		}
		//Added By Anil August 4 2005 --Ends here
		else if(pToken->IsOMToken())
		{
			//do something...
			plexAnal->UnGetToken();
			m_pOMExp = new COMServiceExpression;
			m_pOMExp->CreateParseSubTree(plexAnal,pSymbolTable);
			m_pVariable = 0;
			m_pArrayExp = 0;

			DELETE_PTR(pToken);
		}
		else
		{
			DELETE_PTR(pToken);
			throw(C_AP_ERROR_LVALUE);
		}

		pToken = 0;
//Munch a <=> or <*=>
		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			|| !pToken->IsAssignOp())
		{
			DELETE_PTR(pToken);
			throw(C_AP_ERROR_MISSINGEQ);
		}
		m_AssignType = pToken->GetSubType();

		DELETE_PTR(pToken);

//Munch & Parse the expression.
//we got to give the expression string to the expression parser.
		CExpParser expParser;
		try
		{
			if (stmt_type == STMT_ASSIGNMENT_FOR)
			{
				m_pExpression 
					= expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_WHILE);
			}
			else
			{
				m_pExpression 
					= expParser.ParseExpression(plexAnal,pSymbolTable,EXPR_ASSIGN);
			}

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

		if((LEX_FAIL == plexAnal->GetNextToken(&pToken,pSymbolTable)) 
			|| !pToken
			 || (RUL_SEMICOLON != pToken->GetSubType()))
		{
			if (
				(RUL_RBRACK != pToken->GetSubType()) && 
				(stmt_type != STMT_ASSIGNMENT_FOR)
				)
			{
				DELETE_PTR(pToken);
				throw(C_AP_ERROR_MISSINGSC);
			}
		}
		DELETE_PTR(pToken);
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

void CAssignmentStatement::Identify(
		_CHAR* szData)
{
#ifdef MODIFIED_BY_SS
	/*strcat(szData,"<");
	strcat(szData,szTokenSubstrings[RUL_ASSIGN]);
	strcat(szData,">");
	if(m_pVariable)
		m_pVariable->Identify(szData);
	else if(m_pArrayExp)
		m_pArrayExp->Identify(szData);
	else if(m_pOMExp)
		m_pOMExp->Identify(szData);
	m_pExpression->Identify(szData);
	strcat(szData,"</");
	strcat(szData,szTokenSubstrings[RUL_ASSIGN]);
	strcat(szData,">");*/
	std::string result(szData); // Convert szData to std::string

	result += "<";
	result += szTokenSubstrings[RUL_ASSIGN];
	result += ">";

	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0';

	if (m_pVariable)
		m_pVariable->Identify((szData));
	else if (m_pArrayExp)
		m_pArrayExp->Identify(szData);
	else if (m_pOMExp)
		m_pOMExp->Identify(szData);

	m_pExpression->Identify(szData);

	result += "</";
	result += szTokenSubstrings[RUL_ASSIGN];
	result += ">";

	// Copy the result back to szData
	std::copy(result.begin(), result.end(), szData);
	szData[result.size()] = '\0'; // Null-terminate the string
#endif
}

CToken* CAssignmentStatement::GetVariable()
{
	return m_pVariable;
}

CExpression* CAssignmentStatement::GetExpression()
{
	return m_pExpression;
}

CExpression* CAssignmentStatement::GetArrayExp()
{
	return m_pArrayExp;
}

//Added By Anil August 23 2005 --starts here
//Handling DD variable and Expressions
CExpression* CAssignmentStatement::GetComplexDDExp()
{
	return m_pComplexDDExp;
}
//Added By Anil August 23 2005 --Ends here

CExpression* CAssignmentStatement::GetOMExpression()
{
	return m_pOMExp;
}

_INT32 CAssignmentStatement::GetLineNumber()
{
	_INT32 i32LineNumber = 0;
	if(m_pArrayExp)
	{
		i32LineNumber = m_pArrayExp->GetLineNumber();
	}
	else if(m_pOMExp)
	{
		i32LineNumber = m_pOMExp->GetLineNumber();
	}
	else if(m_pExpression)
	{
		i32LineNumber = m_pExpression->GetLineNumber();
	}

	return i32LineNumber;
}
