#include "pch.h"

#ifdef _WIN32_WCE /* Include only for Windows CE version */
#pragma warning (disable : 4786)
#pragma warning (disable : 4018)

//sjv06feb06 #include <windows.h>


#include <iostream>
#include <fstream>
#include <iomanip> 

using namespace std;
#include "PrintData.h"


//# define myprintf(...) fprintf(__VA_ARGS__)

void myprintf(FILE *stream,const char* format,...)
{
	
	va_list         ap;

	va_start(ap, format);

	(void)vfprintf(stream,format,ap);

	va_end(ap); 

	return;

}

	
/******************************************************************************************/

void dump_blank(FILE* fout,int i)
{
	while(i)
	{
		fprintf(fout," ");
		i--;
	}
}

void dump_reference(FILE * fout, REFERENCE ref)
{
	
	REFERENCE :: iterator p;
	for(p = ref.begin();p != ref.end();p++)
	{
		if(p > ref.begin())
		{
			/*We are looping !!* Print on the next line now */
			fprintf(fout,"\n\t");
		}/*Endif*/
		switch(p->type)
		{
			case	ITEM_ID_REF:
				fprintf(fout,"\t Ref Type :ITEM_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case 	ITEM_ARRAY_ID_REF:
				fprintf(fout,"\t Ref Type :ITEM_ARRAY_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	COLLECTION_ID_REF:
				fprintf(fout,"\t Ref Type :COLLECTION_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	BLOCK_ID_REF:
				fprintf(fout,"\t Ref Type :BLOCK_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	VARIABLE_ID_REF:
				fprintf(fout,"\t Ref Type :VARIABLE_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	MENU_ID_REF:
				fprintf(fout,"\t Ref Type :MENU_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	EDIT_DISP_ID_REF:
				fprintf(fout,"\t Ref Type :EDIT_DISP_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	METHOD_ID_REF:
				fprintf(fout,"\t Ref Type :METHOD_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	REFRESH_ID_REF:
				fprintf(fout,"\t Ref Type :REFRESH_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	UNIT_ID_REF:
				fprintf(fout,"\t Ref Type :UNIT_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	WAO_ID_REF:
				fprintf(fout,"\t Ref Type :WAO_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	RECORD_ID_REF:
				fprintf(fout,"\t Ref Type :RECORD_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	RESP_CODES_ID_REF:
				fprintf(fout,"\t Ref Type :RESP_CODES_ID_REF");
				fprintf(fout,"\tRef ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			case	VIA_ITEM_ARRAY_REF:
				fprintf(fout,"\t Ref Type :VIA_ITEM_ARRAY_REF \t\t Index:");
				dump_expression(fout,p->val.index);
				break;
			case	VIA_COLLECTION_REF:
				fprintf(fout,"\t Ref Type :VIA_COLLECTION_REF");
				fprintf(fout,"\t Member: %u   (0x%x)",p->val.member,p->val.member);
				break;
			case	VIA_RECORD_REF:	
				fprintf(fout,"\t Ref Type :VIA_RECORD_REF");
				fprintf(fout,"\t Member: %u   (0x%x)",p->val.member,p->val.member);
				break;
			case	VIA_PARAM_REF:
				fprintf(fout,"\t Ref Type :VIA_PARAM_REF");
				fprintf(fout,"\t Member: %u   (0x%x)",p->val.member,p->val.member);
				break;
			case	VIA_PARAM_LIST_REF:
				fprintf(fout,"\t Ref Type :VIA_PARAM_LIST_REF");
				fprintf(fout,"\t Member: %u   (0x%x)",p->val.member,p->val.member);
				break;
			case	VIA_BLOCK_REF:
				fprintf(fout,"\t Ref Type :VIA_BLOCK_REF");
				fprintf(fout,"\t Characteristic Rec. ID: %u   (0x%x)",p->val.id,p->val.id);
				break;
			default:
				break;
		}/*End switch*/
	}/*End for*/
			
}/*End dump_reference*/


void dump_string(FILE * fout, STRING * Str)
{
	switch(Str->strType)
	{
		case 	DEV_SPEC_STRING_TAG:
		case	DICTIONARY_STRING_TAG: 
			fprintf(fout,"\t\"%s\" ",Str->str);
			break;
		case	VARIABLE_STRING_TAG:  
			fprintf(fout,"\t\"Var String ID : %u   (0x%x)\"", Str->varId,Str->varId);
			break;
		case	VAR_REF_STRING_TAG:
			fprintf(fout,"\"");
			dump_reference(fout,Str->varRef);
			fprintf(fout,"\"");
			break;
		case	ENUMERATION_STRING_TAG:
			fprintf(fout,"\t\" Enum String ID : %u   (0x%x) \tValue: %u   (0x%x) \"",
												Str->enumStr.enmVar.iD,
												Str->enumStr.enmVar.iD,
												Str->enumStr.enumValue,
												Str->enumStr.enumValue);
			break;
		case	ENUM_REF_STRING_TAG:
			fprintf(fout,"\" Enum Ref:\t");
			dump_reference(fout,*(Str->enumStr.enmVar.ref));
			fprintf(fout," \tValue: %u   (0x%x)\"",Str->enumStr.enumValue,Str->enumStr.enumValue);

											
			break;

		default:
			break;
		
	}/*End switch*/
	
}/*End dump_string */


void dump_type_size(FILE * fout, TYPE_SIZE typeSize)
{
	switch(typeSize.type)
	{
		case 	INTEGER: 
			fprintf(fout,"\t INTEGER  , size : %u\n", typeSize.size);
			break;
		case	UNSIGNED:
			fprintf(fout,"\t UNSIGNED  , size : %u\n", typeSize.size);
			break;
		case	FLOATG_PT:
			fprintf(fout,"\t FLOAT  , size :%u\n",typeSize.size);
			break;
		case	DOUBLE: 	
			fprintf(fout,"\t DOUBLE  , size :%u\n",typeSize.size);
			break;
		case	ENUMERATED:
			fprintf(fout,"\t ENUMERATED  , size :%u\n",typeSize.size);
			break;
		case	BIT_ENUMERATED:
			fprintf(fout,"\t BIT_ENUMERATED  , size :%u\n",typeSize.size);
			break;
		case	INDEX:
			fprintf(fout,"\t INDEX  , size :%u\n",typeSize.size);
			break;
		case	ASCII:
			fprintf(fout,"\t ASCII  , size :%u\n",typeSize.size);
			break;
		case	PACKED_ASCII:
			fprintf(fout,"\t PACKED_ASCII  , size :%u\n",typeSize.size);
			break;
		case	PASSWORD:
			fprintf(fout,"\t PASSWORD  , size :%u\n",typeSize.size);
			break;
		case	BITSTRING:
			fprintf(fout,"\t BITSTRING  , size :%u\n",typeSize.size);
			break;
		case	HART_DATE_FORMAT:
			fprintf(fout,"\t HART_DATE_FORMAT  , size :%u\n",typeSize.size);
			break;
		case	TIME:
			fprintf(fout,"\t TIME  , size :%u\n",typeSize.size);
			break;
		case	DATE_AND_TIME:
			fprintf(fout,"\t DATE_AND_TIME  , size :%u\n",typeSize.size);
			break;
		case	DURATION:
			fprintf(fout,"\t DURATION  , size :%u\n",typeSize.size);
			break;
		case 	EUC:
			fprintf(fout,"\t EUC  , size :%u\n",typeSize.size);
			break;
		case	OCTETSTRING:
			fprintf(fout,"\t OCTETSTRING  , size :%u\n",typeSize.size);
			break;
		case	VISIBLESTRING:	
			fprintf(fout,"\t VISIBLESTRING  , size :%u\n",typeSize.size);
			break;
		case	TIME_VALUE:		
			fprintf(fout,"\t TIME_VALUE  , size :%u\n",typeSize.size);
			break;
		case	BOOLEAN_T:
			fprintf(fout,"\t BOOLEAN  , size :%u\n",typeSize.size);
			break;
		default:
		/*surprise!!!*/
			break;
	}/*End switch*/

}/*End dump_type_size */


void dump_enumlist(FILE * fout, ENUM_VALUE_LIST *enmList)
{
	ENUM_VALUE_LIST :: iterator p;
	ENUM_VALUE		enmVal;

	fprintf(fout,"\t\t Enum Count: % u\n",enmList->size());

	for(p = enmList->begin(); p != enmList->end(); p++)
	{
		enmVal = *p;
		fprintf(fout,"Val:  0x%x \t",enmVal.val);
		fprintf(fout,"Desc:  \"");
		dump_string(fout,&enmVal.desc);
		fprintf(fout,"\"\n");
		fprintf(fout,"Help:\"");
		dump_string(fout,&enmVal.help);
		fprintf(fout,"\"\n");
		fprintf(fout,"Func_Class:  0x%x \t",enmVal.func_class);
		fprintf(fout,"Action_Method:  %u   (0x%x)\n",enmVal.actions,enmVal.actions);
		fprintf(fout,"Status_Class:  0x%x\t  OutputClass_Count: %u\n",
						enmVal.status.status_class,enmVal.status.oclasses.size());
		if(enmVal.status.oclasses.size() > 0)
		{
			OUTPUT_STATUS_LIST stList = enmVal.status.oclasses;
			OUTPUT_STATUS_LIST :: iterator it;
			for(it = stList.begin();it != stList.end();it++)
			{
				switch(it->kind)
				{
					case OC_DV:
						fprintf(fout,"\"Kind_nWhich : DV%u\t",it->which);
						break;
					case OC_TV:
						fprintf(fout,"\"Kind_nWhich : TV%u\t",it->which);
						break;
					case OC_AO:
						fprintf(fout,"\"Kind_nWhich : AO%u\t",it->which);
						break;
					default:
						fprintf(fout,"\"Kind_nWhich : ALL");
						break;
				}/*End Switch*/

				switch(it->oclass)
				{
					case 00:
						fprintf(fout,"Mode_nReliability: AUTO & GOOD\n");
						break;
					case 01:
						fprintf(fout,"Mode_nReliability: MANUAL & GOOD\n");
						break;
					case 02:
						fprintf(fout,"Mode_nReliability: AUTO & BAD\n");
						break;
					case 03:
						fprintf(fout,"Mode_nReliability: MANUAL & BAD\n");
						break;
					default:
						fprintf(fout,"\n");
						break;
				}/*End switch*/

			}/*Endfor it*/
			
		}/*Endif size*/

	fprintf(fout,"\t\t\t\t------------------------\n");
	}/*End for iterator p*/
	
}/* End dump_enumlist */


void dump_reference_list(FILE * fout, REFERENCE_LIST *refList)
{
	REFERENCE_LIST :: iterator p;
	REFERENCE	ref;
	fprintf(fout,"\t\t List Size: % u\n",refList->size());
	int i = 0;
	for(p = refList->begin();p != refList->end();p++)
	{
		ref = *p;
		fprintf(fout,"%d)",i++);
		dump_reference(fout,ref);
		fprintf(fout,"\t\t\t\t------------------------\n");
	}/*End for*/
}/*End dump_reference_list*/

void dump_data_item_list(FILE *fout, DATA_ITEM_LIST dataItemList)
{
	DATA_ITEM_LIST :: iterator p;
	DATA_ITEM dataItem;
	fprintf(fout,"\t\t Item Count: % u\n",dataItemList.size());
	int i = 0;
	for(p = dataItemList.begin();p != dataItemList.end(); p++)
	{
		fprintf(fout,"%d)",i++);
		dataItem = *p;
		switch(dataItem.type)
		{
			case 	DATA_CONSTANT:
				fprintf(fout,"\tIntConst:  %u (0x%u)",dataItem.data.iconst,dataItem.data.iconst);
				break;
			case	DATA_REFERENCE:
				dump_reference(fout,*(dataItem.data.ref));
				break;
			case	DATA_REF_FLAGS:
				dump_reference(fout,*(dataItem.data.ref));
				fprintf(fout,"\t Flags:   0x%x",dataItem.flags);
				break;
			case	DATA_REF_WIDTH:
				dump_reference(fout,*(dataItem.data.ref));
				fprintf(fout,"\t Width:   %u",dataItem.width);
				break;
			case	DATA_REF_FLAGS_WIDTH:
				dump_reference(fout,*(dataItem.data.ref));
				fprintf(fout,"\t Flags:   0x%x",dataItem.flags);
				fprintf(fout,"\t Width:   %u",dataItem.width);
				break;
			case	DATA_FLOATING:
				fprintf(fout,"\t FltConst:  %f",dataItem.data.fconst); 
			default:
				/*Should not come here!!!*/
				break;
		}/*End switch*/
		/*Go to the next line*/
		fprintf(fout,"\n");
	}/*End for*/

	fprintf(fout,"\t\t\t\t------------------------\n");

}/*End dump_data_item_list*/

void dump_response_code_list(FILE* fout,RESPONSE_CODE_LIST *respList)
{
	RESPONSE_CODE_LIST :: iterator p;
	RESPONSE_CODE	respCode;
	int i =0;
	fprintf(fout,"\t\t List Size: % u\n",respList->size());

	for(p = respList->begin();p != respList->end();p++)
	{
		fprintf(fout,"%d)",i++);
		respCode = *p;
		fprintf(fout,"\tValue:  0x%x",respCode.val);
		fprintf(fout,"\tType:  0x%x\n",respCode.type);
		fprintf(fout,"\tDesc:  \"");
		dump_string(fout,&respCode.desc);
		fprintf(fout,"\"\n");
		if(respCode.evaled & RS_HELP_EVALED)
		{
			fprintf(fout,"\tHelp:  \"");
			dump_string(fout,&respCode.help);
			fprintf(fout,"\"\n");

		}/*End if*/

	}/*End for*/
	fprintf(fout,"\t\t\t\t------------------------\n");

}/*End dump_response_code_list*/



void dump_transaction_list(FILE* fout,TRANSACTION_LIST* transList)
{
	TRANSACTION_LIST :: iterator p;
	TRANSACTION trans;
	int i = 0;
	fprintf(fout,"\t\t List Size: % u\n",transList->size());

	for(p = transList->begin();p != transList->end();p++)
	{
		fprintf(fout,"%d)",i++);
		trans = *p;
		fprintf(fout,"\tNumber:  %u\n",trans.number);
		fprintf(fout,"\tRequest: ");
		dump_data_item_list(fout,trans.request);
		fprintf(fout,"\tReply:  ");
		dump_data_item_list(fout,trans.reply);
		fprintf(fout,"\tResponse Codes:  ");
		dump_response_code_list(fout,&trans.rcodes);
	}/*Endfor p*/

}/*End dump_transaction_list*/


void dump_menu_item_list(FILE* fout,MENU_ITEM_LIST* menuList)
{
	MENU_ITEM_LIST :: iterator p;
	MENU_ITEM menuItem;
	int i = 0;
	fprintf(fout,"\t\t Item Count: % u\n",menuList->size());
	for(p = menuList->begin(); p != menuList->end(); p++)
	{
		fprintf(fout,"%d)",i++);
		menuItem = *p;
		dump_reference(fout,menuItem.item);
		fprintf(fout,"\n\t Qualifier:  0x%x\n",menuItem.qual);

	}/*End for*/

	fprintf(fout,"\t\t\t\t------------------------\n");

}/*End dump_transaction_list*/

void dump_refresh_relation(FILE* fout,REFRESH_RELATION* refReln)
{
	
	fprintf(fout,"\n");
	fprintf(fout,"Watch Items:");
	dump_reference_list(fout,&refReln->watch_list);
	fprintf(fout,"Update Items:");
	dump_reference_list(fout,&refReln->update_list);

}/*End dump_refresh_relation*/


void dump_unit_relation(FILE* fout, UNIT_RELATION* unitReln)
{
	fprintf(fout,"\n");
	fprintf(fout,"Unit Variable:");
	dump_reference(fout,unitReln->unit_var);
	fprintf(fout,"\n");
	fprintf(fout,"Variables with Above Units:");
	dump_reference_list(fout,&unitReln->var_units);
}/*End dump_unit_relation*/


void dump_item_array_element_list(FILE* fout, ITEM_ARRAY_ELEMENT_LIST *itemArray)
{
	ITEM_ARRAY_ELEMENT_LIST	:: iterator p;
	ITEM_ARRAY_ELEMENT element;
	int i = 0;
	fprintf(fout,"\t\t List Count: % u\n",itemArray->size());
	for(p = itemArray->begin();p != itemArray->end();p++)
	{
		fprintf(fout,"%d)",i++);
		element = *p;
		fprintf(fout,"\tIndex:  %u   (0x%x)\n",element.index,element.index);
		fprintf(fout,"\tItem:");
		dump_reference(fout,element.item);
		fprintf(fout,"\n");
		if(element.evaled & IA_DESC_EVALED)
		{
			fprintf(fout,"\tDesc:  \"");
			dump_string(fout,&element.desc);
			fprintf(fout,"\"\n");
		}
		if(element.evaled & IA_HELP_EVALED)
		{
			fprintf(fout,"\tHelp:  \"");
			dump_string(fout,&element.desc);
			fprintf(fout,"\"\n");
		}

	}/*End for*/
	fprintf(fout,"\t\t\t\t------------------------\n");

}/*End dump_item_array*/


void dump_member_list(FILE* fout, MEMBER_LIST *memberList)
{
	MEMBER_LIST	:: iterator p;
	MEMBER member;
	int i = 0;
	fprintf(fout,"\t\t List Count: % u\n",memberList->size());
	for(p = memberList->begin();p != memberList->end();p++)
	{
		fprintf(fout,"%d)",i++);
		member = *p;
		fprintf(fout,"\tName:  %u   (0x%x)\n",member.name,member.name);
		fprintf(fout,"\tItem:");
		dump_reference(fout,member.item);
		fprintf(fout,"\n");
		if(member.evaled & MEM_DESC_EVALED)
		{
			fprintf(fout,"\tDesc:  \"");
			dump_string(fout,&member.desc);
			fprintf(fout,"\"\n");
		}
		if(member.evaled & MEM_HELP_EVALED)
		{
			fprintf(fout,"\tHelp:  \"");
			dump_string(fout,&member.desc);
			fprintf(fout,"\"\n");
		}

	}/*End for*/
	fprintf(fout,"\t\t\t\t------------------------\n");

}/*End dump_member_list*/


void dump_expression(FILE* fout, Expression *pExpr)
{
	Expression :: iterator p;
	Element element;
	int i = 0;
	for(p = pExpr->begin(); p != pExpr->end(); p++)
	{
		element = *p;
		fprintf(fout,"\t <%d>  ",i++);
		switch(element.byElemType)
		{

			case	NOT_OPCODE:				/* 1 */
				fprintf(fout,"NOT");
				break;
			case	NEG_OPCODE:				/* 2 */
				fprintf(fout,"NEG");
				break;
			case	BNEG_OPCODE:			 /* 3 */
				fprintf(fout,"BNEG");
				break;
			case	ADD_OPCODE:				/* 4 */
				fprintf(fout,"ADD");
				break;
			case	SUB_OPCODE:				 /* 5 */
				fprintf(fout,"SUB");
				break;
			case	MUL_OPCODE:				/* 6 */
				fprintf(fout,"MULT");
				break;
			case	DIV_OPCODE:				/* 7 */
				fprintf(fout,"DIV");
				break;
			case	MOD_OPCODE:				/* 8 */
				fprintf(fout,"MOD");
				break;
			case	LSHIFT_OPCODE:			/* 9 */ 
				fprintf(fout,"LEFT_SHIFT");
				break;
			case	RSHIFT_OPCODE:			/* 10 */
				fprintf(fout,"RIGHT_SHIFT");
				break;
			case	AND_OPCODE:				/* 11 */
				fprintf(fout,"AND");
				break;
			case	OR_OPCODE:				/* 12 */
				fprintf(fout,"OR");
				break;
			case	XOR_OPCODE:				/* 13 */
				fprintf(fout,"XOR");
				break;
			case	LAND_OPCODE:			/* 14 */
				fprintf(fout,"LOGICAL_AND");
				break;
			case	LOR_OPCODE:				/* 15 */
				fprintf(fout,"LOGICAL_OR");
				break;
			case	LT_OPCODE:				/* 16 */
				fprintf(fout,"LESSTHAN");
				break;
			case	GT_OPCODE:				/* 17 */
				fprintf(fout,"GREATERTHAN");
				break;
			case	LE_OPCODE:				/* 18 */
				fprintf(fout,"LESSTHAN_OR_EQUALTO");
				break;
			case	GE_OPCODE:				/* 19 */
				fprintf(fout,"GREATERTHAN_OR_EQUALTO");
				break;
			case	EQ_OPCODE:				/* 20 */
				fprintf(fout,"EQUALTO");
				break;
			case	NEQ_OPCODE:				/* 21 */
				fprintf(fout,"NOT_EQUALTO");
				break;
			case	INTCST_OPCODE:			/* 22 */
				fprintf(fout,"INTCONST:\t %u",element.elem.ulConst);
				break;
			case	FPCST_OPCODE:			/* 23 */
				fprintf(fout,"FPCONST:\t %f",element.elem.fConst);
				break;
			case	VARID_OPCODE:			/* 24 */
				fprintf(fout,"VARID:\t %u   (0x%x)",element.elem.varId,element.elem.varId);
				break;
			case	MAXVAL_OPCODE:			/* 25 */
				fprintf(fout,"MAX%u OF %u   (0x%x)",element.elem.minMax.which, element.elem.minMax.variable.id);
				break;
			case	MINVAL_OPCODE:			/* 26 */
				fprintf(fout,"MIN%u OF %u   (0x%x)",element.elem.minMax.which, element.elem.minMax.variable.id);
				break;
			case	VARREF_OPCODE:			/* 27 */
				fprintf(fout,"VARREF:");
				dump_reference(fout,*(element.elem.varRef));
				break;
			case	MAXREF_OPCODE:			/* 28 */
				fprintf(fout,"MAX%u OF \t",element.elem.minMax.which);
				dump_reference(fout,*(element.elem.minMax.variable.ref));
				break;
			case	MINREF_OPCODE:			/* 29 */
				fprintf(fout,"MIN%u OF \t",element.elem.minMax.which);
				dump_reference(fout,*(element.elem.minMax.variable.ref));
				break;
			case	BLOCK_OPCODE:			/* 30 */
			case	BLOCKID_OPCODE:			/* 31 */
			case	BLOCKREF_OPCODE:		/* 32 */
			default:
				break;

		}/*End switch*/

		fprintf(fout,"\n");

	}/*End for */

}/*End dump_expression*/


void dump_min_max_list(FILE* fout, MIN_MAX_LIST *minMaxList)
{
	MIN_MAX_LIST :: iterator p;
	MIN_MAX_VALUE minMax;
	int i = 0;
	fprintf(fout,"\t\t List Size: % u\n",minMaxList->size());

	for(p = minMaxList->begin();p != minMaxList->end(); p++)
	{
		minMax = *p;
		fprintf(fout,"%d)",i++);
		fprintf(fout,"Which:  %u\n",minMax.which);
		fprintf(fout,"Value: \n");
		dump_expression(fout,&minMax.value);
	}/*End for p*/

	fprintf(fout,"\t\t\t\t------------------------\n");
}/*End dump_min_max_list*/



void dump_conditional_attribute(FILE* fout,DDlConditional *pCond, int indent)
{
	int iSectionIndex = 0; /*We will use this to index into isSectionConditionalList */
	int iChildIndex = 0; /*We will use this to index into  listOfChilds*/
	int iValueIndex = 0; /*We will use this to index into Vals vector*/
	
	switch(pCond->condType)
	{
		case DDL_COND_TYPE_IF:
			{
				dump_blank(fout,indent);
				fprintf(fout,"IF\n");
				dump_expression(fout,&pCond->expr);
				dump_blank(fout,(indent+5));
				fprintf(fout,"THEN  {\n");
				if(pCond->isSectionConditionalList[iSectionIndex] == false)
				{
					dump_blank(fout,(indent+7));
					switch(pCond->attrDataType)
					{
						case 	DDL_ATTR_DATA_TYPE_INT:
							fprintf(fout,"	%d   (0x%x)\n",pCond->Vals[iValueIndex].iVal,pCond->Vals[iSectionIndex].iVal);
							break;
						case 	DDL_ATTR_DATA_TYPE_UNSIGNED_LONG:
							fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].ulVal,pCond->Vals[iSectionIndex].ulVal);
							break;
						case	DDL_ATTR_DATA_TYPE_FLOAT:
						case	DDL_ATTR_DATA_TYPE_DOUBLE:
							fprintf(fout,"	%f\n",pCond->Vals[iValueIndex].fVal);
							break;
						case	DDL_ATTR_DATA_TYPE_BITSTRING :
							fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].ulVal,pCond->Vals[iSectionIndex].ulVal);
							break;
						case	DDL_ATTR_DATA_TYPE_STRING:
							dump_string(fout,pCond->Vals[iValueIndex].strVal);
							fprintf(fout,"\n"); /*Go to the next line for the next attribute!!*/
							break;
						case	DDL_ATTR_DATA_TYPE_ITEM_ID:
							fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].id,pCond->Vals[iSectionIndex].id);
							break;
						case	DDL_ATTR_DATA_TYPE_ENUM_LIST:
							dump_enumlist(fout,pCond->Vals[iValueIndex].enmList);
							break;

						case	DDL_ATTR_DATA_TYPE_REFERENCE :
							dump_reference(fout,*(pCond->Vals[iValueIndex].ref));
							fprintf(fout,"\n");
							break;
						case	DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
							dump_reference_list(fout,pCond->Vals[iValueIndex].refList);
							break;
						case	DDL_ATTR_DATA_TYPE_TYPE_SIZE:
							dump_type_size(fout,pCond->Vals[iValueIndex].typeSize);
							break;
						case	DDL_ATTR_DATA_TYPE_TRANSACTION_LIST:
							dump_transaction_list(fout,pCond->Vals[iValueIndex].transList);
							break;
						case	DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
							dump_response_code_list(fout,pCond->Vals[iValueIndex].respCdList);
							break;
						case	DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
							dump_menu_item_list(fout,pCond->Vals[iValueIndex].menuItemsList);
							break;
						case	DDL_ATTR_DATA_TYPE_REFRESH_RELATION:
							dump_refresh_relation(fout,pCond->Vals[iValueIndex].refrshReln);
							break;
						case	DDL_ATTR_DATA_TYPE_UNIT_RELATION:
							dump_unit_relation(fout,pCond->Vals[iValueIndex].unitReln);
							break;
						case	DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
							dump_item_array_element_list(fout,pCond->Vals[iValueIndex].itemArrElmnts);
							break;
						case	DDL_ATTR_DATA_TYPE_MEMBER_LIST:
							dump_member_list(fout,pCond->Vals[iValueIndex].memberList);
							break;
						case	DDL_ATTR_DATA_TYPE_EXPRESSION:
							fprintf(fout,"\n");
							dump_expression(fout,pCond->Vals[iValueIndex].pExpr);
							break;
						case	DDL_ATTR_DATA_TYPE_MIN_MAX:
							dump_min_max_list(fout,pCond->Vals[iValueIndex].minMaxList);
							break;
						default:
					/*Surprise!!!*/
						break;
					}/*End switch*/
					fprintf(fout,"\n");
					dump_blank(fout,(indent+7));
					fprintf(fout,"}\n");

				}/*End if */
				else
				{
					dump_conditional_attribute(fout,pCond->listOfChilds[iChildIndex],(indent+7));
					iChildIndex++;
					
				}/*End else*/
				/*Just check if we have ELSE part of IF, if yes then dump it too*/
				if(pCond->byNumberOfSections == 2)
				{
					iSectionIndex++;
					iValueIndex++;
					dump_blank(fout,(indent+5));
					fprintf(fout,"ELSE  {\n");
					if(pCond->isSectionConditionalList[iSectionIndex] == false)
					{
						/*increment the section index*/
						
						dump_blank(fout,(indent+7));
						switch(pCond->attrDataType)
						{
							case 	DDL_ATTR_DATA_TYPE_INT:
								fprintf(fout,"	%d   (0x%x)\n",pCond->Vals[iValueIndex].iVal,pCond->Vals[iValueIndex].iVal);
								break;
							case 	DDL_ATTR_DATA_TYPE_UNSIGNED_LONG:
								fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].ulVal,pCond->Vals[iValueIndex].ulVal);
								break;
							case	DDL_ATTR_DATA_TYPE_FLOAT:
							case	DDL_ATTR_DATA_TYPE_DOUBLE:
								fprintf(fout,"	%f\n",pCond->Vals[iValueIndex].fVal);
								break;
							case	DDL_ATTR_DATA_TYPE_BITSTRING :
								fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].ulVal,pCond->Vals[iValueIndex].ulVal);
								break;
							case	DDL_ATTR_DATA_TYPE_STRING:
								dump_string(fout,pCond->Vals[iValueIndex].strVal);
								fprintf(fout,"\n"); /*Go to the next line for the next attribute!!*/
								break;
							case	DDL_ATTR_DATA_TYPE_ITEM_ID:
								fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].id,pCond->Vals[iValueIndex].id);
								break;
							case	DDL_ATTR_DATA_TYPE_ENUM_LIST:
								dump_enumlist(fout,pCond->Vals[iValueIndex].enmList);
								break;

							case	DDL_ATTR_DATA_TYPE_REFERENCE :
								dump_reference(fout,*(pCond->Vals[iValueIndex].ref));
								fprintf(fout,"\n");
								break;
							case	DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
								dump_reference_list(fout,pCond->Vals[iValueIndex].refList);
								break;
							case	DDL_ATTR_DATA_TYPE_TYPE_SIZE:
								dump_type_size(fout,pCond->Vals[iValueIndex].typeSize);
								break;
							case	DDL_ATTR_DATA_TYPE_TRANSACTION_LIST:
								dump_transaction_list(fout,pCond->Vals[iValueIndex].transList);
								break;
							case	DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
								dump_response_code_list(fout,pCond->Vals[iValueIndex].respCdList);
								break;
							case	DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
								dump_menu_item_list(fout,pCond->Vals[iValueIndex].menuItemsList);
								break;
							case	DDL_ATTR_DATA_TYPE_REFRESH_RELATION:
								dump_refresh_relation(fout,pCond->Vals[iValueIndex].refrshReln);
								break;
							case	DDL_ATTR_DATA_TYPE_UNIT_RELATION:
								dump_unit_relation(fout,pCond->Vals[iValueIndex].unitReln);
								break;
							case	DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
								dump_item_array_element_list(fout,pCond->Vals[iValueIndex].itemArrElmnts);
								break;
							case	DDL_ATTR_DATA_TYPE_MEMBER_LIST:
								dump_member_list(fout,pCond->Vals[iValueIndex].memberList);
								break;
							case	DDL_ATTR_DATA_TYPE_EXPRESSION:
								fprintf(fout,"\n");
								dump_expression(fout,pCond->Vals[iValueIndex].pExpr);
								break;
							case	DDL_ATTR_DATA_TYPE_MIN_MAX:
								dump_min_max_list(fout,pCond->Vals[iValueIndex].minMaxList);
								break;
							default:
						/*Surprise!!!*/
							break;
						}/*End switch*/
						fprintf(fout,"\n");
						dump_blank(fout,(indent+7));
						fprintf(fout,"}\n");

					}/*End if */
					else
					{
						dump_conditional_attribute(fout,pCond->listOfChilds[iChildIndex],(indent+7));
					}
				}/*End ifpCond->byNumberOfSections == 2*/

			}
			break;
		case DDL_COND_TYPE_SELECT:
			{
				dump_blank(fout,indent);
				fprintf(fout,"SELECT\n");
				dump_expression(fout,&pCond->expr);
				while( iSectionIndex < pCond->byNumberOfSections)
				{
					dump_blank(fout,(indent+5));
					fprintf(fout,"{\n");
					dump_blank(fout,(indent+10));
					if(iSectionIndex == pCond->caseVals.size())
					{
						fprintf(fout,"DEFAULT :\n");
					}
					else
					{
						fprintf(fout,"CASE");
						dump_expression(fout,&pCond->caseVals[iSectionIndex]);
					}
					if(pCond->isSectionConditionalList[iSectionIndex] == false)
					{
						dump_blank(fout,(indent+20));
						switch(pCond->attrDataType)
						{
							case 	DDL_ATTR_DATA_TYPE_INT:
								fprintf(fout,"	%d   (0x%x)\n",pCond->Vals[iValueIndex].iVal,pCond->Vals[iValueIndex].iVal);
								break;
							case 	DDL_ATTR_DATA_TYPE_UNSIGNED_LONG:
								fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].ulVal,pCond->Vals[iValueIndex].ulVal);
								break;
							case	DDL_ATTR_DATA_TYPE_FLOAT:
							case	DDL_ATTR_DATA_TYPE_DOUBLE:
								fprintf(fout,"	%f\n",pCond->Vals[iValueIndex].fVal);
								break;
							case	DDL_ATTR_DATA_TYPE_BITSTRING :
								fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].ulVal,pCond->Vals[iValueIndex].ulVal);
								break;
							case	DDL_ATTR_DATA_TYPE_STRING:
								dump_string(fout,pCond->Vals[iValueIndex].strVal);
								fprintf(fout,"\n"); /*Go to the next line for the next attribute!!*/
								break;
							case	DDL_ATTR_DATA_TYPE_ITEM_ID:
								fprintf(fout,"	%u   (0x%x)\n",pCond->Vals[iValueIndex].id,pCond->Vals[iValueIndex].id);
								break;
							case	DDL_ATTR_DATA_TYPE_ENUM_LIST:
								dump_enumlist(fout,pCond->Vals[iValueIndex].enmList);
								break;

							case	DDL_ATTR_DATA_TYPE_REFERENCE :
								dump_reference(fout,*(pCond->Vals[iValueIndex].ref));
								fprintf(fout,"\n");
								break;
							case	DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
								dump_reference_list(fout,pCond->Vals[iValueIndex].refList);
								break;
							case	DDL_ATTR_DATA_TYPE_TYPE_SIZE:
								dump_type_size(fout,pCond->Vals[iValueIndex].typeSize);
								break;
							case	DDL_ATTR_DATA_TYPE_TRANSACTION_LIST:
								dump_transaction_list(fout,pCond->Vals[iValueIndex].transList);
								break;
							case	DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
								dump_response_code_list(fout,pCond->Vals[iValueIndex].respCdList);
								break;
							case	DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
								dump_menu_item_list(fout,pCond->Vals[iValueIndex].menuItemsList);
								break;
							case	DDL_ATTR_DATA_TYPE_REFRESH_RELATION:
								dump_refresh_relation(fout,pCond->Vals[iValueIndex].refrshReln);
								break;
							case	DDL_ATTR_DATA_TYPE_UNIT_RELATION:
								dump_unit_relation(fout,pCond->Vals[iValueIndex].unitReln);
								break;
							case	DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
								dump_item_array_element_list(fout,pCond->Vals[iValueIndex].itemArrElmnts);
								break;
							case	DDL_ATTR_DATA_TYPE_MEMBER_LIST:
								dump_member_list(fout,pCond->Vals[iValueIndex].memberList);
								break;
							case	DDL_ATTR_DATA_TYPE_EXPRESSION:
								fprintf(fout,"\n");
								dump_expression(fout,pCond->Vals[iValueIndex].pExpr);
								break;
							case	DDL_ATTR_DATA_TYPE_MIN_MAX:
								dump_min_max_list(fout,pCond->Vals[iValueIndex].minMaxList);
								break;
							default:
						/*Surprise!!!*/
							break;
						}/*End switch*/
						fprintf(fout,"\n");
						dump_blank(fout,(indent+5));
						fprintf(fout,"}\n");

					
					}/*Endif*/
					else
					{
						dump_conditional_attribute(fout,pCond->listOfChilds[iChildIndex],(indent+20));
						iChildIndex++;
					}
				iSectionIndex++;
				iValueIndex++;

				}/*End while iSectionIndex*/

			}
			break;		
		default:
			break;

	}/*End switch*/

}/*End dump_conditional_attribute*/


void dump_conditional_chunks(FILE* fout,DDlAttribute *pAttr)
{
	int iDirectValIndex = 0;
	int iCondValIndex = 0;
	for(int i = 0; i < pAttr->byNumOfChunks; i++)
	{
		if(pAttr->isChunkConditionalList[i] == true) /*If conditional chunk*/
		{
			dump_conditional_attribute(fout,pAttr->conditionalVals[iCondValIndex],5);
			iCondValIndex++;
		}/*Endif Conditional chunk */
		else
		{/*Direct chunk*/
			switch(pAttr->attrDataType)
			{
				case	DDL_ATTR_DATA_TYPE_ENUM_LIST:
					dump_enumlist(fout,pAttr->directVals[iDirectValIndex].enmList);
					break;
				case	DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
					dump_reference_list(fout,pAttr->directVals[iDirectValIndex].refList);
					break;
				case	DDL_ATTR_DATA_TYPE_TRANSACTION_LIST:
					dump_transaction_list(fout,pAttr->directVals[iDirectValIndex].transList);
					break;
				case	DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
					dump_response_code_list(fout,pAttr->directVals[iDirectValIndex].respCdList);
					break;
				case	DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
					dump_menu_item_list(fout,pAttr->directVals[iDirectValIndex].menuItemsList);
					break;
				case	DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
					dump_item_array_element_list(fout,pAttr->directVals[iDirectValIndex].itemArrElmnts);
					break;	
				case	DDL_ATTR_DATA_TYPE_MEMBER_LIST:
					dump_member_list(fout,pAttr->directVals[iDirectValIndex].memberList);
					break;
				case	DDL_ATTR_DATA_TYPE_MIN_MAX:
					dump_min_max_list(fout,pAttr->directVals[iDirectValIndex].minMaxList);
					break;
				default:
					break;
			
			}/*End switch*/	
			
			iDirectValIndex++;
		
		}/*End else*/

	}/*Endfor i*/
	
}/*End dump_conditional_chunks*/




void dump_attributes(FILE * fout, ItemAttrList list)
{
	
	ItemAttrList :: iterator p;
	DDlAttribute *pAttr = NULL;

	for(p = list.begin(); p!= list.end(); p++)
	{
		pAttr = (*p);
		fprintf(fout,"%s :",pAttr->attrName.c_str());
		if(pAttr->bIsAttributeConditional == false &&
			pAttr->bIsAttributeConditionalList == false) /*We have a direct attribute */
		{
			/*switch to the appropriate data type & simply dump it!!*/
			switch(pAttr->attrDataType)
			{

				case 	DDL_ATTR_DATA_TYPE_INT:
					fprintf(fout,"	%d   (0x%x)\n",pAttr->pVals->iVal,pAttr->pVals->iVal);
					break;
				case 	DDL_ATTR_DATA_TYPE_UNSIGNED_LONG:
					fprintf(fout,"	%u   (0x%x)\n",pAttr->pVals->ulVal,pAttr->pVals->ulVal);
					break;
				case	DDL_ATTR_DATA_TYPE_FLOAT:
				case	DDL_ATTR_DATA_TYPE_DOUBLE:
					fprintf(fout,"	%f\n",pAttr->pVals->fVal);
					break;
				case	DDL_ATTR_DATA_TYPE_BITSTRING :
					fprintf(fout,"	(0x%x)\n",pAttr->pVals->ulVal,pAttr->pVals->ulVal);
					break;
				case	DDL_ATTR_DATA_TYPE_STRING:
					dump_string(fout,pAttr->pVals->strVal);
					fprintf(fout,"\n"); /*Go to the next line for the next attribute!!*/
					break;
				case	DDL_ATTR_DATA_TYPE_ITEM_ID:
					fprintf(fout,"	%u   (0x%x)\n",pAttr->pVals->id,pAttr->pVals->id);
					break;
				case	DDL_ATTR_DATA_TYPE_ENUM_LIST:
					dump_enumlist(fout,pAttr->pVals->enmList);
					break;

				case	DDL_ATTR_DATA_TYPE_REFERENCE :
					dump_reference(fout,*(pAttr->pVals->ref));
					fprintf(fout,"\n");
					break;
				case	DDL_ATTR_DATA_TYPE_REFERENCE_LIST:
					dump_reference_list(fout,pAttr->pVals->refList);
					break;
				case	DDL_ATTR_DATA_TYPE_TYPE_SIZE:
					dump_type_size(fout,pAttr->pVals->typeSize);
					break;
				case	DDL_ATTR_DATA_TYPE_TRANSACTION_LIST:
					dump_transaction_list(fout,pAttr->pVals->transList);
					break;
				case	DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST:
					dump_response_code_list(fout,pAttr->pVals->respCdList);
					break;
				case	DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST:
					dump_menu_item_list(fout,pAttr->pVals->menuItemsList);
					break;
				case	DDL_ATTR_DATA_TYPE_DEFINITION:
					fprintf(fout,"\n");
					int i;
					for(i=0;i < pAttr->pVals->defData.size;i++)
					{
						fprintf(fout,"%c",pAttr->pVals->defData.data[i]);
					}
					fprintf(fout,"\n");
					break;
				case	DDL_ATTR_DATA_TYPE_REFRESH_RELATION:
					dump_refresh_relation(fout,pAttr->pVals->refrshReln);
					break;
				case	DDL_ATTR_DATA_TYPE_UNIT_RELATION:
					dump_unit_relation(fout,pAttr->pVals->unitReln);
					break;
				case	DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST:
					dump_item_array_element_list(fout,pAttr->pVals->itemArrElmnts);
					break;
				case	DDL_ATTR_DATA_TYPE_MEMBER_LIST:
					dump_member_list(fout,pAttr->pVals->memberList);
					break;
				case	DDL_ATTR_DATA_TYPE_EXPRESSION:
					fprintf(fout,"\n");
					dump_expression(fout,pAttr->pVals->pExpr);
					break;
				case	DDL_ATTR_DATA_TYPE_MIN_MAX:
					dump_min_max_list(fout,pAttr->pVals->minMaxList);
					break;
				default:
					/*Surprise!!!*/
					break;
			}/*End switch*/


		}/*Endif */
		else /*dump the conditional attribute*/
		{
			fprintf(fout,"\n");
			if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == false)
			{
				dump_conditional_attribute(fout,pAttr->pCond,5);
			}
			else
			{
				dump_conditional_chunks(fout,pAttr);
			}


		}/*End else*/

	
	}/*End for p*/


}/*End dump_attributes */






void dump_items(FILE* fout, DDlDevDescription *pDevDesc)
{
	vector <DDlBaseItem *>:: iterator p;
	DDlBaseItem *pBaseItem;

	for(p = pDevDesc->ItemsList.begin(); p != pDevDesc->ItemsList.end();p++)
	{
		pBaseItem = *p;
		switch(pBaseItem->byItemType)
		{

			case	VARIABLE_ITYPE:
				fprintf (fout,"\n\n*********************************Variable Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlVariable*)pBaseItem)->variableAttrList);
				break;
			case	COMMAND_ITYPE:
				fprintf (fout,"\n\n*********************************Command Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlCommand*)pBaseItem)->commandAttrList);
				break;
			case	MENU_ITYPE: 
				fprintf (fout,"\n\n*********************************Menu Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlMenu*)pBaseItem)->menuAttrList);
				break;
			case	EDIT_DISP_ITYPE:  
				fprintf (fout,"\n\n*********************************Edit Display Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlEditDisplay*)pBaseItem)->editDisplayAttrList);
				break;
			case	METHOD_ITYPE:     
				fprintf (fout,"\n\n*********************************Method Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlMethod*)pBaseItem)->methodAttrList);
				break;
			case	REFRESH_ITYPE:    
				fprintf (fout,"\n\n*********************************Refresh Relation Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlRefresh*)pBaseItem)->refreshAttrList);
				break;
			case	UNIT_ITYPE:       
				fprintf (fout,"\n\n*********************************Unit Relation Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlUnit*)pBaseItem)->unitAttrList);
				break;
			case	WAO_ITYPE: 	
				fprintf (fout,"\n\n*********************************WAO Relation Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlWao*)pBaseItem)->waoAttrList);
				break;
			case	ITEM_ARRAY_ITYPE: 
				fprintf (fout,"\n\n*********************************Item Array Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				fprintf	(fout,"Item Sub Type:	%u\n",pBaseItem->byItemSubType);
				dump_attributes(fout,((DDlItemArray*)pBaseItem)->iArrAttrList);
				break;
			case	COLLECTION_ITYPE:
				fprintf (fout,"\n\n*********************************Collection Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				fprintf	(fout,"Item Sub Type:	%u\n",pBaseItem->byItemSubType);
				dump_attributes(fout,((DDlCollection*)pBaseItem)->collectionAttrList);
				break;
			case	BLOCK_ITYPE:      
				fprintf (fout,"\n\n*********************************Block Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				dump_attributes(fout,((DDlBlock*)pBaseItem)->blockAttrList);
				break;
			case	RECORD_ITYPE: 
				fprintf (fout,"\n\n*********************************Record Item*********************************\n");
				fprintf (fout,"Item ID:   %u   (0x%x)\n",pBaseItem->id,pBaseItem->id);
				fprintf (fout,"Item Masks:   %u   (0x%x)\n",pBaseItem->ulItemMasks,pBaseItem->ulItemMasks);
				fprintf	(fout,"Item Sub Type:	%u\n",pBaseItem->byItemSubType);
				dump_attributes(fout,((DDlRecord*)pBaseItem)->recordAttrList);
				break;
			default:
				break;
			
		}/*End switch*/

	}/*End for p*/

}/*End dump_items*/
#endif  /* _WIN32_WCE */


