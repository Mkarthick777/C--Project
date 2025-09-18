#ifndef FUNCTIONEXPRESSION_H
#define FUNCTIONEXPRESSION_H

#include "Expression.h"
#include "VMConstants.h"

#include <vector>
using namespace std;

class FunctionExpression : public CExpression  
{
public:
	FunctionExpression();
	virtual ~FunctionExpression();

//	Identify self
	virtual void Identify(
		_CHAR* szData);

//	Allow Visitors to do different operations on the node.
	//virtual _INT32 Execute(
	//	CGrammarNodeVisitor* pVisitor, 
	//	CSymbolTable* pSymbolTable,
	//	INTER_VARIANT* pvar=0,
	//	ERROR_VEC*				pvecErrors=0,
	//	RUL_TOKEN_SUBTYPE	AssignType = RUL_ASSIGN);//Anil August 26 2005 to Fix a[exp1] += exp2

//	Create as much of the parse tree as possible.
	virtual _INT32 CreateParseSubTree(
		CLexicalAnalyzer* plexAnal, 
		CSymbolTable* pSymbolTable,
		ERROR_VEC*				pvecErrors=0
		);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	_UCHAR*	GetFunctionName()
	{
		return m_pchFunctionName;
	}

	_INT32	GetParameterCount()
	{
		return m_i32ParameterCount;
	}

	CExpression* GetExpParameter(int iParameterIndex)
	{
		return m_pExpression[iParameterIndex];
	}

	RUL_TOKEN_TYPE	GetParameterType(int iParameterIndex)
	{
		return m_pTokenType[iParameterIndex];
	}

	CToken* GetConstantParameter(int iParameterIndex)
	{
		return m_pConstantTokens[iParameterIndex];
	}
protected:
	_UCHAR*			m_pchFunctionName;
	_INT32			m_i32ParameterCount;

	CExpression*	m_pExpression[MAX_NUMBER_OF_FUNCTION_PARAMETERS];
	RUL_TOKEN_TYPE	m_pTokenType[MAX_NUMBER_OF_FUNCTION_PARAMETERS];
	CToken*			m_pConstantTokens[MAX_NUMBER_OF_FUNCTION_PARAMETERS];

	_INT32 GetFunctionDetails(
							  CToken *pToken
							  , Function_Signatures *pFunc
							  );

	
};

#endif /* FUNCTIONEXPRESSION_H */
