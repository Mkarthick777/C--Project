/************************************************************************************************
 * Attributes.cpp   DDParser version
 ***********************************************************************************************/
#include "pch.h"
#pragma warning (disable : 4786)

#include "DDlConditional.h" // includes--"Attributes.h"
#ifdef _DEBUG
#include "Dictionary.h"// for dictionary test
#include "LitStringTable.h"
extern CDictionary *pGlobalDict; /*The Global Dictionary object*/
extern LitStringTable *pLitStringTable; /*The Global literal string table object*/
#endif
#include "logging.h"

ddpREF& ddpREF::operator= (const ddpREF& src)
{
	type = src.type;
	
	if(type == VIA_ITEM_ARRAY_REF  ||
	   type == VIA_ARRAY_REF       ||
	   type == VIA_LIST_REF        ||
	   type == CONSTANT_REF        )
	{
		val.index = new ddpExpression(*(src.val.index));
	}
	else
	{
		memcpy(&val, &(src.val),sizeof(val));
	}
	return *this;
}

void ddpREF ::Cleanup()
{ 
		if(type == VIA_ITEM_ARRAY_REF  ||
			/* VIA_COLLECTION_REF || VIA_FILE_REF || VIA_BITENUM_REF || VIA_CHART_REF || 
			   VIA_GRAPH_REF || VIA_SOURCE_REF || VIA_ATTR_REF
			these are supposed to be member names, not expression lists */
		   type == VIA_ARRAY_REF       ||
		   type == VIA_LIST_REF        ||
		   type == CONSTANT_REF        )/* the other 7 via_xxx are made to be constant member or id inparse_base*/
		{
			if(val.index != NULL && val.index->size() > 0)
			{
				if ( val.index->size() > 10000 )
				{
					LOGIT(CLOG_LOG,"ddpREF has an issue while trying to Cleanup().\n");
					RAZE (val.index);
				}
				else
				{

				try
				{
				ddpExpression :: iterator it;// ptr2 element
				for(it = val.index->begin(); it != val.index->end(); it++)
				{
					if (&( *it ))	// PAW &* added 07/04/09
						(*it).Cleanup();	
#ifdef _DEBUG
					else
						LOGIT(CERR_LOG,"ERROR: NULL pointer in the index (expression) list of a via ref.\n");
#endif
				}
				val.index->clear();
				delete val.index;
				}
				catch (...)
				{
					// First-chance exception in <program.exe>: 0xC0000005: Access Violation.

					// this error is coming up when we de-allocate everything in the ParserTest
					// program.  The program seems to work correctly other than this exception.
					// We will pass on fixing the real problem for now and come back to it later.
					// 20dec2007  timj	
#ifdef _DEBUG
					LOGIT(CLOG_LOG,"...caught exception:ddpREF ::Cleanup()\n");
#endif
				}

				val.index = NULL;
				}// end else, decent sized index
			}
		}
		type = 0;
		val.member = 0L;// will cover val.index,val.id both
}

ddpREFERENCE& ddpREFERENCE::operator= (const ddpREFERENCE& src)
{
	Cleanup();// try and prevent memory leaks;
	vector <ddpREF>:: const_iterator iRef;// const added PAW 07/04/09
	for (iRef =  /*(vector <ddpREF>::iterator)PAW 07/04/09*/src.begin(); 
	     iRef != /*(vector <ddpREF>::iterator)PAW 07/04/09*/src.end(); 		 ++iRef)
	{   push_back(*((ddpREF*)&(*iRef)));	// should execute copy constructor	  
	}
	return *this;
}

void ddpREFERENCE::Cleanup()
{
	vector <ddpREF>::iterator iRef;
#ifdef _DEBUG
	int Z = 0;
	for (iRef = begin(); iRef != end(); ++iRef, Z++)
#else
	for (iRef = begin(); iRef != end(); ++iRef)
#endif
	{   
		((ddpREF*)&(*iRef))->Cleanup(); 
	}
	erase(begin(),end()); 
};

ddpExpression::~ddpExpression()
{
	vector <Element>::iterator iElem;
	for (iElem = begin(); iElem != end(); ++iElem)
	{   
		((Element*)&(*iElem))->Cleanup(); 
	}
	clear(); 
}

ddpExpression& ddpExpression::operator=(const ddpExpression& src)
{
	vector <Element>::const_iterator iElem;// const added PAW 07/04/09
	for (iElem =  /*(vector <Element>::iterator) PAW 07/04/09*/src.begin(); 
	     iElem != /*(vector <Element>::iterator) PAW 07/04/09*/src.end();   ++iElem)
	{   
		push_back(*((Element*)&(*iElem)));	// should execute copy constructor	  
	}
	return *this;
}

ddpSTRING::ddpSTRING()
{ 
	str = NULL; 
	len=flags=0; 
	enumStr.enmVar.ref = NULL;
	enumStr.enumValue = 0L;
	strType = DEV_SPEC_STRING_TAG;
	varId=0;
	varRef.Cleanup();// unneeded since we have a constructor now
}

//ddpSTRING& ddpSTRING::operator=(const ddpSTRING& srcStr )
//{
//	Cleanup(); // try and prevent memory leaks
//
//	len     = srcStr.len;
//	flags   = srcStr.flags;
//	strType = srcStr.strType;
//	varId   = srcStr.varId;
//
//	switch(strType)
//	{
//	case DEV_SPEC_STRING_TAG:	//0
//	case DICTIONARY_STRING_TAG: //3 this holds the looked up version 
//		if(srcStr.str != NULL)
//		{
//			int l = strlen(srcStr.str)+1;
//			str = new char[l];
//			strcpy(str,srcStr.str);
//			flags = FREE_STRING;
//		}
//		else
//			str = NULL;
//		break;
//	case VAR_REF_STRING_TAG:	//4	isa ddpREFERENCE, a vector <ddpREF>	
//		varRef = srcStr.varRef;
//		break;
//	
//	case ENUM_REF_STRING_TAG:	//5
//		enumStr.enumValue  = srcStr.enumStr.enumValue;
//		if (srcStr.enumStr.enmVar.ref != NULL)
//			enumStr.enmVar.ref = new ddpREFERENCE(*srcStr.enumStr.enmVar.ref);
//		else
//			enumStr.enmVar.ref = NULL;
//		break;
//	case ENUMERATION_STRING_TAG:  //2
//		enumStr.enumValue = srcStr.enumStr.enumValue;
//		enumStr.enmVar.iD = srcStr.enumStr.enmVar.iD;		
//		break;
//	case VARIABLE_STRING_TAG:		//1 varId
//	default:
//		// we already cleaned up
//		break;
//	}
//	return *this;
//}
ddpSTRING& ddpSTRING::operator=(const ddpSTRING& srcStr) {
	Cleanup(); // Try to prevent memory leaks

	len = srcStr.len;
	flags = srcStr.flags;
	strType = srcStr.strType;
	varId = srcStr.varId;

	switch (strType) {
	case DEV_SPEC_STRING_TAG:       // 0
	case DICTIONARY_STRING_TAG: {   // 3
		if (srcStr.str != nullptr) {
			str = new char[srcStr.len + 1];
			std::copy(srcStr.str, srcStr.str + srcStr.len + 1, str);
			flags = FREE_STRING;
		}
		else {
			str = nullptr;
		}
		break;
	}
	case VAR_REF_STRING_TAG:        // 4
		varRef = srcStr.varRef;
		break;
	case ENUM_REF_STRING_TAG: {     // 5
		enumStr.enumValue = srcStr.enumStr.enumValue;
		if (srcStr.enumStr.enmVar.ref != nullptr) {
			enumStr.enmVar.ref = new ddpREFERENCE(*srcStr.enumStr.enmVar.ref);
		}
		else {
			enumStr.enmVar.ref = nullptr;
		}
		break;
	}
	case ENUMERATION_STRING_TAG: {  // 2
		enumStr.enumValue = srcStr.enumStr.enumValue;
		enumStr.enmVar.iD = srcStr.enumStr.enmVar.iD;
		break;
	}
	case VARIABLE_STRING_TAG:       // 1 varId
	default:
		// No need for additional cleanup
		break;
	}

	return *this;
}
void ddpSTRING::Cleanup()
{
	switch(strType)
	{
		case DEV_SPEC_STRING_TAG:	//0
		case DICTIONARY_STRING_TAG: //3
			if(str != NULL && flags == FREE_STRING)
			{
#ifdef DICT_REF_DEBUG
				if ( pGlobalDict->isDictPtr(str) )
					LOGIT(CERR_LOG,"ERROR: dictionary pointer with FREE_STRING.\n");
				else
#endif
				delete [] str;
			}
			break;

		case VAR_REF_STRING_TAG:			
			varRef.Cleanup();
			break;
		
		case ENUM_REF_STRING_TAG:
			if (enumStr.enmVar.ref != NULL)
				delete (enumStr.enmVar.ref);// it has a destructor
			break;
		case ENUMERATION_STRING_TAG:    //2
			break;
		case VARIABLE_STRING_TAG:		//1 varId
		default:
			break;
	}
//this may cause a leak but the ref is shown as freed
//	varRef.clear();

	enumStr.enumValue  = 0xffffffff;
	enumStr.enmVar.ref = NULL;	// clears id and all...	

	str = NULL; 
	len=flags=0; 
	strType = DEV_SPEC_STRING_TAG;
	varId=0;
}


void ITEM_ARRAY_ELEMENT::Cleanup()
{
	item.Cleanup();
	item.clear();
	desc.Cleanup();
	help.Cleanup();
}


void MEMBER::Cleanup()
{ 
	item.Cleanup();
	item.clear();
	desc.Cleanup();
	help.Cleanup();
}


void MENU_ITEM::Cleanup()
{
	item.Cleanup();
	item.clear();
	qual = 0;
}


void RESPONSE_CODE::Cleanup()
{
	desc.Cleanup();
	help.Cleanup();
	evaled=val=type=0;
}


void REFRESH_RELATION::Cleanup()
{
	REFERENCE_LIST :: iterator it;

	for(it = watch_list.begin(); it != watch_list.end(); it++)
	{
		(*it).Cleanup();
		(*it).clear();
	}
	watch_list.clear();

	for(it = update_list.begin(); it != update_list.end(); it++)
	{
		(*it).Cleanup();
		(*it).clear();
	}

	update_list.clear();
}

REFRESH_RELATION& REFRESH_RELATION::operator=(const REFRESH_RELATION& rr)
{
	REFERENCE_LIST :: const_iterator it;
	ddpREFERENCE  localRef;
	// be sure we are not doing a +=
	Cleanup();

	for(it = /*(REFERENCE_LIST :: const_iterator)*/rr.watch_list.begin(); 
	    it !=/*(REFERENCE_LIST :: const_iterator)*/rr.watch_list.end();   it++)
	{// don't copy pointers, make a copy
		localRef = *((ddpREFERENCE*)&(*it));
		watch_list.push_back(localRef);
		localRef.Cleanup();
		localRef.clear();
	}

	for(it = /*(REFERENCE_LIST :: const_iterator)*/rr.update_list.begin(); 
	    it !=/*(REFERENCE_LIST :: const_iterator)*/rr.update_list.end();     it++)
	{
		localRef = *((ddpREFERENCE*)&(*it));
		update_list.push_back(localRef);
		localRef.Cleanup();
		localRef.clear();
	}
	return *this;
}

void UNIT_RELATION::Cleanup()
{
	REFERENCE_LIST :: iterator it;

	unit_var.Cleanup();
	unit_var.clear(); 

	for(it = /*(REFERENCE_LIST :: iterator)*/var_units.begin(); 
	    it !=/*(REFERENCE_LIST :: iterator)*/var_units.end(); it++)
	{
		(*it).Cleanup();
		(*it).clear();
	}

	var_units.clear();
}

UNIT_RELATION& UNIT_RELATION::operator=(const UNIT_RELATION& ur)
{
	REFERENCE_LIST :: const_iterator it;
	ddpREFERENCE  localRef;
	// be sure we are NOT doing a +=
	Cleanup();

	unit_var = ur.unit_var;

	for(it = /*(REFERENCE_LIST :: const_iterator)*/ur.var_units.begin(); 
	    it !=/*(REFERENCE_LIST :: const_iterator)*/ur.var_units.end();   ++it)
	{// don't copy pointers, make a copy
		localRef = *((ddpREFERENCE*)&(*it));
		var_units.push_back(localRef);
		localRef.Cleanup();
		localRef.clear();
	}
	return *this;
}


void ENUM_VALUE::Cleanup()
{
	desc.Cleanup();
	help.Cleanup();

}


void DATA_ITEM::Cleanup()
{
	if((type == DATA_REFERENCE)|| (type == DATA_REF_FLAGS)
		|| (type == DATA_REF_WIDTH)|| (type == DATA_REF_FLAGS_WIDTH))
	{
		if(data.ref != NULL)
		{
			data.ref->Cleanup();
			delete data.ref;
			data.ref = NULL;
		}
	}
}

DATA_ITEM& DATA_ITEM::operator=(const DATA_ITEM& di)
{
	type = di.type;
	flags= di.flags;
	//width= di.width;
	mask = di.mask;

	if( type == DATA_REFERENCE || type == DATA_REF_FLAGS       ||
		type == DATA_REF_WIDTH || type == DATA_REF_FLAGS_WIDTH  )
	{
		if(di.data.ref != NULL)
		{
			data.ref = new ddpREFERENCE(* di.data.ref);
		}
		else
		{
			data.ref = NULL;
		}
	}
	else
	if (type == DATA_FLOATING)
	{
		data.fconst = di.data.fconst;
	}
	else // DATA_CONSTANT
	{
		data.iconst = di.data.iconst;
	}
	return *this;
}


DATA_ITEM_LIST& DATA_ITEM_LIST::operator=(const DATA_ITEM_LIST& src)
{
	vector<DATA_ITEM>::iterator iDi;
	DATA_ITEM_LIST* pDI = (DATA_ITEM_LIST*) &src;
	for ( iDi = pDI->begin(); iDi != pDI->end(); ++iDi)
	{
		push_back(*((DATA_ITEM*)&(*iDi)));// should execute a copy constructor
	}
	return *this;
}

void DATA_ITEM_LIST::Cleanup()	// return parameter added 07/04/09 PAW
{
	vector<DATA_ITEM>::iterator iDi;
	for ( iDi = begin(); iDi != end(); ++iDi)
	{
		((DATA_ITEM*)&(*iDi))->Cleanup();
	}
	clear();
}

void TRANSACTION::Cleanup()
{ 
	DATA_ITEM_LIST:: iterator it;

	RESPONSE_CODE_LIST :: iterator it1;
	
	for(it = request.begin(); it != request.end(); it++)
	{
			(*it).Cleanup();
	}
	
	request.clear();

	for(it = reply.begin(); it != reply.end(); it++)
	{
			(*it).Cleanup();
	}
	
	reply.clear();

	for(it1 = rcodes.begin(); it1 != rcodes.end(); it1++)
	{
			(*it1).Cleanup();
	}
	
	rcodes.clear();

	
}


MIN_MAX& MIN_MAX::operator=(const MIN_MAX& s)
{
	Cleanup();// try to prevent memory leaks
	which = s.which;	isID = s.isID;
	if (isID) {variable.id = s.variable.id;}
	else
	{variable.ref = new ddpREFERENCE(* s.variable.ref);}
	return *this;
}

void MIN_MAX::Cleanup()
{
	if( (!isID) && (variable.ref != NULL) )
	{// ddpREFERENCE has a destructor now....
		delete variable.ref;
		variable.ref = NULL;
	}
	which = 0; isID = false;
}


Element&   Element::operator=(const Element& src)
{
	Cleanup();// try to prevent memory leaks
	byElemType = src.byElemType;

	switch(byElemType)// stevev 8au807- then modify those that need to be
	{
	case VARREF_OPCODE:
		if(src.elem.varRef != NULL)
		{			
			elem.varRef = new ddpREFERENCE(*(src.elem.varRef));
		}
		break;
	case MAXREF_OPCODE:
	case MINREF_OPCODE:
		if(src.elem.minMax != NULL)
		{			
			elem.minMax = new MIN_MAX(*(src.elem.minMax));
		}
		break;
	case STRCST_OPCODE:
		if(src.elem.pSTRCNST != NULL)
		{
			elem.pSTRCNST = new ddpSTRING(*(src.elem.pSTRCNST));
		}
		break;
	default:
		memcpy(&elem, &(src.elem),sizeof(elem));// stevev 8au807-do the rest
		break;
	}
	return *this;
}

void Element::Cleanup()
{
	switch(byElemType)
	{
		case VARREF_OPCODE:
			if(elem.varRef != NULL)
			{// we gave ddpREFERENCE a destructor
				delete elem.varRef;
				elem.varRef = NULL;
			}
			break;
		case MAXREF_OPCODE:
		case MINREF_OPCODE:
			if(elem.minMax != NULL)
			{
				delete elem.minMax;// does cleanup()
				elem.minMax =  NULL;
			}
		case STRCST_OPCODE:
			{
				elem.pSTRCNST->Cleanup();
				delete elem.pSTRCNST;
				elem.pSTRCNST = NULL;
			}
		default:
			break;
	}
	byElemType = 0;
}


void MIN_MAX_VALUE::Cleanup()
{
//	ValueList :: iterator it;
//	if ( pVals != NULL )
//		pVals->Cleanup(DDL_ATTR_DATA_TYPE_EXPRESSION);
	if ( pCond != NULL )
		delete pCond;
	pCond = NULL;
		
//	for(it = directVals.begin(); it != directVals.end(); ++it)
//	{
//		(*it).Cleanup(DDL_ATTR_DATA_TYPE_MIN_MAX);
//	}
//	directVals.clear();
}

void GRID_SET::Cleanup()
{	
	REFERENCE_LIST :: iterator it;// ptr 2a reference
	ddpREFERENCE* pReference = NULL;
	ddpREFERENCE :: iterator it1;// ptr 2a ref
	ddpREF* pRef = NULL;

	for(it = values.begin(); it != values.end(); it++)
	{
		pReference = (ddpREFERENCE*) &(*it);// PAW was just it 07/04/09
		for(it1 = pReference->begin(); it1 != pReference->end(); it1++)
		{
			pRef = (ddpREF*)&(*it1);	// PAW was just it1 07/04/09
			pRef->Cleanup();// we want to cleanup the actual, not a copy			
		}
		pReference->clear();
	}
	values.clear();

	desc.Cleanup();
}

GRID_SET::GRID_SET(const GRID_SET& srcGS) : desc(srcGS.desc), values(srcGS.values)
{
}

void ATTR_DEBUG_INFO_T::Cleanup()
{
	attr_member_list.clear();
	attr_filename.Cleanup();
}


void ITEM_DEBUG_INFO :: Cleanup() 
{
	ATTR_DEBUG_LIST :: iterator it;


	for(it = attr_list.begin(); it != attr_list.end(); it++)
	{// ptr2aptr2a ATTR_DEBUG_INFO_T
		if (*it != NULL)
		{
			(*it)->Cleanup();
			delete (*it);
			(*it) = NULL;
		}
	}
	attr_list.clear();
	file_name.Cleanup();		
}

#ifdef _DBGMIL
void MENU_ITEM::dumpItem(void)
{
	Expression* pEx;
	Expression::iterator pEt;
	Element*    pElm;
	ddpREFERENCE::iterator iR;
	ddpREF* pPR;
	int z = 0, a = 0;
	for (iR = item.begin(); iR != item.end(); ++ iR, z++)
	{
		pEx = NULL;
		pPR = (ddpREF*) iR;
		LOGIT(COUT_LOG,"       ddpREF %2d] <%p> type: %02x    ",z,pPR,pPR->type);
		switch(pPR->type)
		{
			case ITEM_ID_REF:
			case ITEM_ARRAY_ID_REF:
			case COLLECTION_ID_REF:
			case BLOCK_ID_REF:
			case VARIABLE_ID_REF:
			case MENU_ID_REF:
			case EDIT_DISP_ID_REF:
			case METHOD_ID_REF:
			case REFRESH_ID_REF:
			case UNIT_ID_REF:
			case WAO_ID_REF:
			case RECORD_ID_REF:
			case ARRAY_ID_REF:
			case RESP_CODES_ID_REF:
			case FILE_ID_REF:		
			case CHART_ID_REF:
			case GRAPH_ID_REF:
			case AXIS_ID_REF:
			case WAVEFORM_ID_REF:
			case SOURCE_ID_REF:
			case LIST_ID_REF:
			case IMAGE_ID_REF:
			case GRID_ID_REF:
			{
				LOGIT(COUT_LOG,"ident 0x%04x\n",pPR->val.id);
			}
			break;
		case VIA_ARRAY_REF:
		case VIA_ITEM_ARRAY_REF:
		case VIA_LIST_REF:
			{
				pEx = pPR->val.index;
			}
			break;

		case VIA_VAR_LIST_REF:
		case VIA_COLLECTION_REF:
		case VIA_RECORD_REF:
		case VIA_FILE_REF:
		case VIA_BITENUM_REF:			
		case VIA_CHART_REF:
		case VIA_GRAPH_REF:
		case VIA_SOURCE_REF:
		case VIA_ATTR_REF:
			{
				LOGIT(COUT_LOG,"membr 0x%04x\n",pPR->val.member);
			}
			break;
		case CONSTANT_REF:
			{
				pEx = pPR->val.index;
			}
			break;

		default:
			return ;//DDL_ENCODING_ERROR;
			break;
											
		}/*End switch tag*/
		if (pEx != NULL)
		{
			LOGIT(COUT_LOG,"\nExpression <%p> with %d elements\n",pEx,pEx->size());
			a = 0;
			for ( pEt = pEx->begin(); pEt != pEx->end(); ++pEt, a++ )
			{
				pElm = (Element*)pEt;
				if ((int)pElm > 0x3000000)
				{
					LOGIT(COUT_LOG,"               Expr %02d]  <%p> POINTER OUT OF RANGE.  ",a,pElm);
					return;
				}
				LOGIT(COUT_LOG,"               Expr %02d]  <%p>   Element Type: %d  ",a,pElm,pElm->byElemType);
				switch (pElm->byElemType) 
				{
				case INTCST_OPCODE: /* Integer constant opcode*/
					{		
						LOGIT(COUT_LOG,"ulConst:  %u",pElm->elem.ulConst);
					}
					break;/*INTCST_OPCODE*/			
				case FPCST_OPCODE:
					{	
						LOGIT(COUT_LOG,"fConst:   %f",pElm->elem.fConst);							
					}
					break;/*FPCST_OPCODE*/
				case STRCST_OPCODE:
					{	
						LOGIT(COUT_LOG,"sConst:  *not Expanded*"/*,pElm->elem.pSTRCNST*/);		
					}
					break;/*STRCST_OPCODE*/			
				case BLOCK_OPCODE: 
				case BLOCKID_OPCODE: 
				case BLOCKREF_OPCODE: 
				case SYSTEMENUM_OPCODE:
					{
						LOGIT(COUT_LOG," Unusable Opcode");
					}
					break;
				case VARID_OPCODE:
				case MAXVAL_OPCODE:
				case MINVAL_OPCODE:
				case VARREF_OPCODE:
				case MAXREF_OPCODE:
				case MINREF_OPCODE:
					{
						LOGIT(COUT_LOG," Var/min/max op code");				
					}
					break;/*VARID_OPCODE,MAXVAL_OPCODE,MINVAL_OPCODE,VARREF_OPCODE,MAXREF_OPCODE */
				default:
					{
						LOGIT(COUT_LOG," Unknown (default)");
					}
					break;
					
				}/* End (Element type) tag*/
				LOGIT(COUT_LOG,"\n");
			}// next element of expression
		}//endif we have an expression
	}// next reference entry
}
#endif