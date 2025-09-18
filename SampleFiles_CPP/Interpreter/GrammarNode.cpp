
//#include "stdafx.h"
#include "pch.h"
#include "GrammarNode.h"
#include "SymbolTable.h"

CGrammarNode::CGrammarNode()
{
	SetNodeType(NODE_TYPE_INVALID);
	m_CurrentScopeIndex=0;
}

CGrammarNode::~CGrammarNode()
{

}


_INT32 CGrammarNode::Execute(
			CGrammarNodeVisitor*	pVisitor,
			CSymbolTable*			pSymbolTable,
			INTER_VARIANT*		pvar,
			ERROR_VEC*				pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return 0;
}

_INT32 CGrammarNode::CreateParseSubTree(
			CLexicalAnalyzer*	plexAnal, 
			CSymbolTable*		pSymbolTable,
			ERROR_VEC*			pvecErrors)
{
	return 0;
}

void CGrammarNode::Identify(
			_CHAR* szData)
{
}

_INT32 CGrammarNode::GetLineNumber()
{
	return 0;
}
//SCR26200 Felix
void CGrammarNode::SetScopeIndex(_INT32 nSymTblScpIdx)
{
	m_CurrentScopeIndex = nSymTblScpIdx;
}

_INT32 CGrammarNode::GetScopeIndex()	//SCR26200 Felix
{
	return m_CurrentScopeIndex;
}
