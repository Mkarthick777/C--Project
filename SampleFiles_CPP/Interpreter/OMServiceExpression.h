#if !defined(AFX_OMSERVICESTATEMENT_H__1BD21975_CEC7_403E_96DF_14750A098E24__INCLUDED_)
#define AFX_OMSERVICESTATEMENT_H__1BD21975_CEC7_403E_96DF_14750A098E24__INCLUDED_

#include "Expression.h"
#include "VMConstants.h"

#include <vector>
using namespace std;

struct ATTRIBUTE
{
	_UCHAR uchAttribName[MAX_BYTE];
};

typedef vector<ATTRIBUTE> ATTRIBUTE_LIST;

class COMServiceExpression : public CExpression  
{
public:
	COMServiceExpression();
	virtual ~COMServiceExpression();

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
		ERROR_VEC*				pvecErrors=0);

//This returns the last line in which this node has a presence...
	virtual _INT32 GetLineNumber();

	_UCHAR*	GetObjectName();
	_INT32	GetAttibuteCount();

	_UCHAR*	GetAttributeName(
		_INT32 i32Idx);

	_INT32	GetConstantPoolIdx();
protected:

	_INT32 MakeConstantPoolEntry(
		CLexicalAnalyzer* plexAnal, 
		CSymbolTable* pSymbolTable);

	_UCHAR*			m_pucObjectName;
	ATTRIBUTE_LIST	m_attribList;
	_INT32			m_i32constant_pool_idx;

	
};

#endif