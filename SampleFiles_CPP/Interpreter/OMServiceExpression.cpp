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

#include "OMServiceExpression.h"
#include "GrammarNodeVisitor.h"
#if _MSC_VER < 1400	/* stevev 23feb10 - 2005 mods, these are not needed */
//#include "FunctionExpression.h"
//#include "SymbolTable.h"
#endif

COMServiceExpression::COMServiceExpression()
{
	m_pucObjectName		=	0;
	m_i32constant_pool_idx = -1;
}

COMServiceExpression::~COMServiceExpression()
{
	if(m_pucObjectName)
	{
		delete m_pucObjectName;
		m_pucObjectName = 0;
	}
	m_i32constant_pool_idx = -1;
}

#ifdef MODIFIED_BY_SS
#ifdef _FULL_RULE_ENGINE
_INT32 COMServiceExpression::Execute(
			CGrammarNodeVisitor* pVisitor,
			CSymbolTable* pSymbolTable,
			INTER_VARIANT* pvar,
			ERROR_VEC*	pvecErrors,
			RUL_TOKEN_SUBTYPE	AssignType)//Anil August 26 2005 to Fix a[exp1] += exp2
{
	return pVisitor->visitOMExpression(
		this,
		pSymbolTable,
		pvar,
		pvecErrors,
		AssignType);//Anil August 26 2005 to Fix a[exp1] += exp2
}
#endif
#endif

//	OM Service Expression is of the form
//	OM Service Expression	=>	<ObjectManager> <::> <Point_Address> <;>|<Operator>
//	Point_Address			=>	<Object_Name><Attribute_List>
//	Attribute_List			=>	<.><Attribute><Attribute_List> | <.><Attribute>
//	Attribute				=>	<Identifier>

_INT32 COMServiceExpression::CreateParseSubTree(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable,
			ERROR_VEC*	pvecErrors)
{
/*	CToken* pToken=0;
//Munch a <ObjectManager>
	if((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)) 
		&& pToken
		&&  (RUL_KEYWORD == pToken->GetType()) 
		&& (RUL_OM == pToken->GetSubType()))
	{
	}
	DELETE_PTR(pToken);

//Munch a <::>
	if((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)) 
		&& pToken
		&& (RUL_SYMBOL == pToken->GetType())
		&& RUL_SCOPE == pToken->GetSubType())
	{
		// let's see what to do here...
	}
	else
	{
		// throw error
	}
	DELETE_PTR(pToken);

//Munch a <Object_Name>
	if((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)) 
		&& pToken)
	{
		_UCHAR* ptemp = (_UCHAR*)pToken->GetLexeme();
		m_pucObjectName = new _UCHAR[strlen((const _CHAR*)ptemp)+1];
		strcpy((_CHAR*)m_pucObjectName,(const _CHAR*)ptemp);
	}
	DELETE_PTR(pToken);

//Munch a <Attribute_List>
	while((LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable))
			&& pToken
			&& (RUL_DOT == pToken->GetSubType()))
	{
		DELETE_PTR(pToken);
		if(LEX_FAIL != plexAnal->GetNextToken(&pToken,pSymbolTable)
			&& pToken
			)
		{
			ATTRIBUTE attrib;
			strcpy((_CHAR*)attrib.uchAttribName,pToken->GetLexeme());
			m_attribList.push_back(attrib);
			DELETE_PTR(pToken);
		}
	}

	if(!MakeConstantPoolEntry(plexAnal,pSymbolTable))
	{
		m_i32constant_pool_idx = -1;
	}

	delete pToken;
	pToken = 0;
	plexAnal->UnGetToken();
*/
	return 0;
}

void COMServiceExpression::Identify(
		_CHAR* szData)
{
/*	if(m_pucObjectName)
	{
		strcat(szData,"<");
		strcat(szData,(const _CHAR*)m_pucObjectName);
		strcat(szData,">");

		for(_INT32 i=0; i< m_attribList.size(); i++)
		{
			strcat(szData,(const _CHAR*)m_attribList[i].uchAttribName);
		}

		strcat(szData,"</");
		strcat(szData,(const _CHAR*)m_pucObjectName);
		strcat(szData,">");
	}
*/
}

_UCHAR* COMServiceExpression::GetObjectName()
{
	return m_pucObjectName;
}

_UCHAR* COMServiceExpression::GetAttributeName(
			_INT32 i32Idx)
{
	return m_attribList[i32Idx].uchAttribName;
}

_INT32 COMServiceExpression::GetAttibuteCount()
{
	return m_attribList.size();
}

//#include "RuleOMItfExports.h"
//#include "RuleObjMgrInterface.h"
//#include "OMObject.h"

_INT32 OIDConvertIntToChar(
			_INT32 i32OID, 
			_CHAR* chOID)
{
	union unionOID
	{
		_CHAR				chOID[5];
		struct strOID
		{
			_INT32			nOID;
			_UCHAR			chOID;
		}sOID;
	};
	unionOID uOID;
	uOID.sOID.nOID	=	i32OID;
	uOID.sOID.chOID	=	0;
	memcpy(chOID,uOID.chOID,5);

	return 0;
}

_INT32 COMServiceExpression::MakeConstantPoolEntry(
			CLexicalAnalyzer* plexAnal, 
			CSymbolTable* pSymbolTable)
{
/*
//	Get the Object Name from the COMServiceExpression object
//	Get the Object Id from the object name (use dbs handler)
	_UCHAR	chOIDAID[9];
	_INT32	i32Count = GetAttibuteCount();
	_UCHAR	chType=0;

	if(0==m_pucObjectName ||m_attribList.size()<=0)
	{
		return 0;
	}

	if(i32Count <1)
	{
		return 0;
	}
	else if(i32Count==1)
	{
		GetOIDAID(m_pucObjectName,
					m_attribList[i32Count-1].uchAttribName,
					chOIDAID,
					&chType);
	}
	else
	{
//	root.a1.a2.a3.... an-1. an
//	1.	Get root object_name(=m_pucObjectName).
//	2.	Get OID
//	3.	while(i32Count-2>=0)
//		{
//	4.		newObjectName = m_attribList[i32Count-2].uchAttribName;
//	5.		newAttribName = m_attribList[i32Count-1].uchAttribName;
//	6.		CObject* obj = GetObject(OID);
//	7.		obj->GetAttribute(newObjectName,var);
//	8.		OID = var.intVal;
//	9.		i32Count--;
//		}
//	10.	obj->GetAttribute(newAttribName,var);
//	11.	AID = var.intVal;
//	12.	TID = GetType();

		
//		GetOIDAID(m_attribList[i32Count-2].uchAttribName,
//					m_attribList[i32Count-1].uchAttribName,
//					chOIDAID,
//					&chType);

		_UCHAR	chOID[5];
		_UCHAR	chObjName[255];
		memset(chObjName,0,255);
		_UCHAR	chAttribName[255];
		memset(chAttribName,0,255);

		CRuleObjMgrInterface om;

		GetOID(m_pucObjectName,chOID);
		
		_INT32 i32OID=0;
		_variant_t var;
		COMObject obj;
		DCAP_VALUETYPE enValueType;

		while(i32Count-2>=0)
		{
			var.Clear();
			strcpy((_CHAR*)chObjName,(const _CHAR*)m_attribList[i32Count-2].uchAttribName);
			strcpy((_CHAR*)chAttribName,(const _CHAR*)m_attribList[i32Count-1].uchAttribName);
		

			om.GetObject(chOID,&obj);
			obj.GetAttribute((_CHAR*)chAttribName,enValueType,var);

			i32OID = var.lVal;
			OIDConvertIntToChar(i32OID,(_CHAR*)chOID);

			i32Count--;
		}

		obj.GetAttribute((_CHAR*)chAttribName,enValueType,var);
		_INT32 i32AID = var.intVal;
//		_INT32 i32TID = GetType();

	}
//	Put the object id in the constant pool table.
	m_i32constant_pool_idx = pSymbolTable->InsertOMConstant(chOIDAID,chType);
*/
	return 1;
}

_INT32 COMServiceExpression::GetConstantPoolIdx()
{
	return m_i32constant_pool_idx;
}

_INT32 COMServiceExpression::GetLineNumber()
{
	return -1;
}
