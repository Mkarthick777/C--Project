/*************************************************************************************************

	This file contains the implementation of the methods  for parsing different Attribute
											Data Types

*************************************************************************************************/

#pragma warning (disable : 4786)

//sjv06feb06 #include <windows.h>
#include "pch.h"
#include "ddbGeneral.h"
#include "Evl_loc.h"
#include "DDl_Attr_Parse.h"
#include "Parse_Base.h"
#include "Panic.h"
#include "DDlConditional.h"
#include <iostream>

#include <iomanip>
#include <cassert>


/*
int	ddl_parse_integer_func(unsigned char **chunkp, unsigned long *size, unsigned long *value)

int ddl_parse_tag_func(unsigned char **chunkp, unsigned long *size, unsigned long *tagp,
					   unsigned long *lenp)
*/



/* Parse an attribute which is an Integer ddpExpression*/

int parse_attr_int(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long * length = NULL;
	unsigned long tag,tagp,len;
//	long temp;
//	unsigned long tempLong;
	unsigned __int64 LL;
	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	/*Parse the Tag to know if we have a conditional or a direct object*/

	DDL_PARSE_TAG(chunkp, length, &tag, &len);
	 
	switch (tag)
			{

				case IF_TAG: /*We have an IF THEN ELSE conditional*/
					{
						pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);	
						
						/*Now Parse the ddpExpression associated with the IF block */
						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						/*otherwise Parse the value of the attribute associated with THEN clause*/
						
						rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
					
						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						

						/*Parse the ELSE portion if there's one*/
						if (*length >0)
						{
							rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/

							pAttr->pCond->byNumberOfSections++;

						}

					}
					break; /*End IF_TAG*/
			
				case SELECT_TAG: /*We have a Switch Case conditional*/
					{
						pAttr->bIsAttributeConditional = true;
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
														pAttr->attrDataType,
														0);

						/*Now Parse the ddpExpression Argument of the SELECT */

						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if(rc != SUCCESS)
							return rc;
						
						/*otherwise Parse all the CASE branches and the DEFAULT */
						while (*length > 0)
						{
							DDL_PARSE_TAG(chunkp,length,&tagp,&len);

							switch(tagp)
							{
								case CASE_TAG:
									{
										/*We are parsing the CASE constants as expression
										just bcoz of the spec. But it should be a constant 
										value , ie. an expression with just a  constant (integer)
										value*/

										rc = ddl_parse_expression(chunkp,length,&tempExpr);

										if(rc != SUCCESS)
											return rc;

/*										ddpExpression :: iterator it;

										it = tempExpr.begin();

										if(it->byElemType != INTCST_OPCODE)
											{
#ifdef _PARSER_DEBUG
				fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_int()!!!");
												return DDL_UNHANDLED_STUFF_FAILURE;
#endif
												
											}
										temp = (long)it->elem.ulConst;
*/
										pAttr->pCond->caseVals.push_back(tempExpr);
										
										/*We have the case constant value 
										Now parse the attributre value from the 
										following chunk	*/

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/

										pAttr->pCond->byNumberOfSections++;
										
										tempExpr.clear();
									}
									break;/*End CASE_TAG*/

								case DEFAULT_TAG:
									{
/*										temp = DEFAULT_TAG_VALUE;
										pAttr->pCond->caseVals.push_back(temp);
*/
										
										pAttr->pCond->byNumberOfSections++;

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/
									}
									break;/*End DEFAULT_TAG*/

								default:
									return DDL_ENCODING_ERROR;

							}/*End Switch tagp*/


						}/*End while*/


					}
					break; /*End SELECT_TAG*/

				case OBJECT_TAG : /*We have a direct object*/
					{

						pAttr->pVals = new VALUES;
					
						DDL_PARSE_INTEGER(chunkp,length,&LL);

						if(DDL_ATTR_DATA_TYPE_INT == pAttr->attrDataType)
						{
							pAttr->pVals->llVal = (INT64)LL;
						}
						else /*DDL_ATTR_DATA_TYPE_UNSIGNED_LONG == pAttr->attrDataType*/
						{
							pAttr->pVals->ullVal = LL;
						}

					}
					break; /*End OBJECT_TAG*/
				default:
					return DDL_ENCODING_ERROR;
					break;
			}/*End switch tag*/
	return SUCCESS;	

}/*End parse_attr_int*/

int parse_attr_ulong(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{

	return parse_attr_int(pAttr,binChunk,size);

}/*End parse_attr_int*/

int parse_attr_float(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{

	int rc;
	unsigned char **chunkp = NULL;
	unsigned long * length = NULL;
	unsigned long tag,tagp,len;
//	long temp;
	float fTemp;
	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	/*Parse the Tag to know if we have a conditional or a direct object*/

	DDL_PARSE_TAG(chunkp, length, &tag, &len);
	
	switch (tag)
			{

				case IF_TAG: /*We have an IF THEN ELSE conditional*/
					{
						pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);	
						
						/*Now Parse the ddpExpression associated with the IF block */
						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						/*otherwise Parse the value of the attribute associated with THEN clause*/
						
						rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
					
						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						

						/*Parse the ELSE portion if there's one*/
						if (*length >0)
						{
							rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/

							pAttr->pCond->byNumberOfSections++;

						}

					}
					break; /*End IF_TAG*/
			
				case SELECT_TAG: /*We have a Switch Case conditional*/
					{
						pAttr->bIsAttributeConditional = true;
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
														pAttr->attrDataType,
														0);

						/*Now Parse the ddpExpression Argument of the SELECT */

						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if(rc != SUCCESS)
							return rc;
						
						/*otherwise Parse all the CASE branches and the DEFAULT */
						while (*length > 0)
						{
							DDL_PARSE_TAG(chunkp,length,&tagp,&len);

							switch(tagp)
							{
								case CASE_TAG:
									{
										/*We are parsing the CASE constants as expression
										just bcoz of the spec. But it should be a constant 
										value , ie. an expression with just a  constant (integer)
										value*/

										rc = ddl_parse_expression(chunkp,length,&tempExpr);

										if(rc != SUCCESS)
											return rc;
/*
										ddpExpression :: iterator it;

										it = tempExpr.begin();

										if(it->byElemType != INTCST_OPCODE)
											{
#ifdef _PARSER_DEBUG
				fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_float!!!");
											return DDL_UNHANDLED_STUFF_FAILURE;
#endif												
											}
										temp = (long)it->elem.ulConst;
*/
										pAttr->pCond->caseVals.push_back(tempExpr);
										
										/*We have the case constant value 
										Now parse the attributre value from the 
										following chunk	*/

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/

										pAttr->pCond->byNumberOfSections++;
										
										tempExpr.clear();
									}
									break;/*End CASE_TAG*/

								case DEFAULT_TAG:
									{
/*
										temp = DEFAULT_TAG_VALUE;
										pAttr->pCond->caseVals.push_back(temp);
*/
										pAttr->pCond->byNumberOfSections++;
										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/
									}
									break;/*End DEFAULT_TAG*/

								default:
									return DDL_ENCODING_ERROR;

							}/*End Switch tagp*/


						}/*End while*/


					}
					break; /*End SELECT_TAG*/

				case OBJECT_TAG : /*We have a direct object*/
					{

						pAttr->pVals = new VALUES;
					
						rc = ddl_parse_float(chunkp,length,&fTemp);

						if(DDL_ATTR_DATA_TYPE_FLOAT == pAttr->attrDataType)
						{
							pAttr->pVals->fVal = fTemp;
						}
						else /*DDL_ATTR_DATA_TYPE_DOUBLE == pAttr->attrDataType*/
						{
							pAttr->pVals->dVal = (double)fTemp;
						}

					}
					break; /*End OBJECT_TAG*/
				default:
					return DDL_ENCODING_ERROR;
					break;
			}/*End switch tag*/
	return SUCCESS;	

}/*End parse_attr_float */

int parse_attr_double(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	return parse_attr_float(pAttr,binChunk,size);
}/*End parse_attr_double*/

int parse_attr_bitstring(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{

	int rc;
	unsigned char **chunkp = NULL;
	unsigned long * length = NULL;
	unsigned long tag,tagp,len;
//	long temp;
	unsigned long tempLong;
	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	/*Parse the Tag to know if we have a conditional or a direct object*/

	DDL_PARSE_TAG(chunkp, length, &tag, &len);
	
	switch (tag)
			{

				case IF_TAG: /*We have an IF THEN ELSE conditional*/
					{
						pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);	
						
						/*Now Parse the ddpExpression associated with the IF block */
						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						/*otherwise Parse the value of the attribute associated with THEN clause*/
						
						rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
					
						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						

						/*Parse the ELSE portion if there's one*/
						if (*length >0)
						{
							rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/

							pAttr->pCond->byNumberOfSections++;

						}

					}
					break; /*End IF_TAG*/
			
				case SELECT_TAG: /*We have a Switch Case conditional*/
					{
						pAttr->bIsAttributeConditional = true;
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
														pAttr->attrDataType,
														0);

						/*Now Parse the ddpExpression Argument of the SELECT */

						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if(rc != SUCCESS)
							return rc;
						
						/*otherwise Parse all the CASE branches and the DEFAULT */
						while (*length > 0)
						{
							DDL_PARSE_TAG(chunkp,length,&tagp,&len);

							switch(tagp)
							{
								case CASE_TAG:
									{
										/*We are parsing the CASE constants as expression
										just bcoz of the spec. But it should be a constant 
										value , ie. an expression with just a  constant (integer)
										value*/

										rc = ddl_parse_expression(chunkp,length,&tempExpr);

										if(rc != SUCCESS)
											return rc;
/*
										ddpExpression :: iterator it;

										it = tempExpr.begin();

										if(it->byElemType != INTCST_OPCODE)
											{
#ifdef _PARSER_DEBUG
				fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_bitstring!!!");
											return DDL_UNHANDLED_STUFF_FAILURE;
#endif											
											}
										temp = (long)it->elem.ulConst;
*/
										pAttr->pCond->caseVals.push_back(tempExpr);
										
										/*We have the case constant value 
										Now parse the attributre value from the 
										following chunk	*/

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/

										pAttr->pCond->byNumberOfSections++;
										
										tempExpr.clear();
									}
									break;/*End CASE_TAG*/

								case DEFAULT_TAG:
									{
/*
										temp = DEFAULT_TAG_VALUE;
										pAttr->pCond->caseVals.push_back(temp);
*/							
										pAttr->pCond->byNumberOfSections++;

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/
									}
									break;/*End DEFAULT_TAG*/

								default:
									return DDL_ENCODING_ERROR;

							}/*End Switch tagp*/


						}/*End while*/


					}
					break; /*End SELECT_TAG*/

				case OBJECT_TAG : /*We have a direct object*/
					{

						pAttr->pVals = new VALUES;
					
						rc = ddl_parse_bitstring(chunkp,length,&tempLong);

						if(rc != SUCCESS)
								return rc;
						/*Since bitstring value is a ulong only, we'll store it as a ulong itself*/
						pAttr->pVals->ullVal = (UINT32)tempLong;
	
					}
					break; /*End OBJECT_TAG*/
				default:
					return DDL_ENCODING_ERROR;
					break;
			}/*End switch tag*/
	return SUCCESS;	


}/*End parse_attr_bitstring*/

int parse_attr_string(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{

	int rc;
	unsigned char **chunkp = NULL;
	unsigned long * length = NULL;
	unsigned long tag,tagp,len;
//	long temp;
	ddpSTRING strTemp;
	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	/*Parse the Tag to know if we have a conditional or a direct object*/

	DDL_PARSE_TAG(chunkp, length, &tag, &len);
	
	
	switch (tag)
	{

		case IF_TAG: /*We have an IF THEN ELSE conditional*/
			{
				pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
				pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
												pAttr->attrDataType,
												1);	
				
				/*Now Parse the ddpExpression associated with the IF block */
				rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

				/*otherwise Parse the value of the attribute associated with THEN clause*/
				
				rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
			
				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

				

				/*Parse the ELSE portion if there's one*/
				if (*length >0)
				{
					rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
					if (rc != SUCCESS)
					return rc; /* Return if not successful*/

					pAttr->pCond->byNumberOfSections++;

				}

			}
			break; /*End IF_TAG*/
	
		case SELECT_TAG: /*We have a Switch Case conditional*/
			{
				pAttr->bIsAttributeConditional = true;
				pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
												pAttr->attrDataType,
												0);

				/*Now Parse the ddpExpression Argument of the SELECT */

				rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

				if(rc != SUCCESS)
					return rc;
				
				/*otherwise Parse all the CASE branches and the DEFAULT */
				while (*length > 0)
				{
					DDL_PARSE_TAG(chunkp,length,&tagp,&len);

					switch(tagp)
					{
						case CASE_TAG:
							{
								/*We are parsing the CASE constants as expression
								just bcoz of the spec. But it should be a constant 
								value , ie. an expression with just a  constant (integer)
								value*/

								rc = ddl_parse_expression(chunkp,length,&tempExpr);

								if(rc != SUCCESS)
									return rc;
/*
								ddpExpression :: iterator it;

								it = tempExpr.begin();

								if(it->byElemType != INTCST_OPCODE)
									{
#ifdef _PARSER_DEBUG
		fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_string()!!!");
									return DDL_UNHANDLED_STUFF_FAILURE;

									}
								temp = (long)it->elem.ulConst;
*/
								pAttr->pCond->caseVals.push_back(tempExpr);
								
								/*We have the case constant value 
								Now parse the attributre value from the 
								following chunk	*/

								rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/

								pAttr->pCond->byNumberOfSections++;
								
								tempExpr.clear();
							}
							break;/*End CASE_TAG*/

						case DEFAULT_TAG:
							{
/*										temp = DEFAULT_TAG_VALUE;
								pAttr->pCond->caseVals.push_back(temp);
*/
		
								pAttr->pCond->byNumberOfSections++;

								rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/
							}
							break;/*End DEFAULT_TAG*/

						default:
							return DDL_ENCODING_ERROR;

					}/*End Switch tagp*/


				}/*End while*/


			}
			break; /*End SELECT_TAG*/

		case OBJECT_TAG : /*We have a direct object*/
			{

				pAttr->pVals = new VALUES;

				pAttr->pVals->strVal = new ddpSTRING;
			
				rc = ddl_parse_string(chunkp,length,pAttr->pVals->strVal);

			}
			break; /*End OBJECT_TAG*/
		default:
			return DDL_ENCODING_ERROR;
			break;
	}/*End switch tag*/
	return SUCCESS;

} /*End parse_attr_string*/


/* I guess this one could be merged with the ulong one */
int parse_attr_item_id(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size); 


int parse_attr_enum_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{

	int rc;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;
	unsigned long tag,tagp,length,len,len1;
	

//	DDlConditional *tempPtrToCond = NULL;

	ENUM_VALUE_LIST *tempPtrToEnumList = NULL;

	ENUM_VALUE tmpEnm;

	VALUES		tempVal;
		

	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	lengthp = &size;

	/*The first tag should be a ENUMERATOR_SEQLIST_TAG if not then return error*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(ENUMERATOR_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;

	*lengthp -= length;
	

	while(length >0)
	{
//		DDlConditional *tempPtrToCond = NULL;

		/*Parse the Tag to know if we have a conditional or a direct object*/

		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;
		
		switch (tag)
				{

					case IF_TAG: /*We have an IF THEN ELSE conditional*/
						{
							DDlConditional *tempPtrToCond = NULL;

							if((pAttr->byNumOfChunks == 0) && length == 0)
							{/*We have a conditional in single chunk*/
							
								pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
								pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
															pAttr->attrDataType,
															1);
								tempPtrToCond = pAttr->pCond;
								pAttr->byNumOfChunks++;
							}
							else
							{/*We have a multichunk list which has conditionals*/
								tempPtrToCond = new DDlConditional(DDL_COND_TYPE_IF,
																	pAttr->attrDataType,
																	1);
								
								pAttr->bIsAttributeConditionalList = true;
								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
								pAttr->byNumOfChunks++;
							}


								/*Now Parse the ddpExpression associated with the IF block */
								rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							/*otherwise Parse the value of the attribute associated with THEN clause*/
							
							rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,ENUMERATOR_SEQLIST_TAG);
						
							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							

							/*Parse the ELSE portion if there's one*/
							if (len >0)
							{
								rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,ENUMERATOR_SEQLIST_TAG);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/

								tempPtrToCond->byNumberOfSections++;

							}

							if(pAttr->bIsAttributeConditionalList == true)
							{/*then we have to push this conditional value on conditionalVals*/

								pAttr->conditionalVals.push_back(tempPtrToCond);
							}
						
						}
						break; /*End IF_TAG*/
				
					case SELECT_TAG: /*We have a Switch Case conditional*/
						{
							DDlConditional *tempPtrToCond = NULL;

							if((pAttr->byNumOfChunks == 0) && length == 0)
							{/*We have a conditional in single chunk*/
							
								pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
								pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
															pAttr->attrDataType,
															0);
								tempPtrToCond = pAttr->pCond;
								pAttr->byNumOfChunks++;
							}
							else
							{/*We have a multichunk list which has conditionals*/
								tempPtrToCond = new DDlConditional(DDL_COND_TYPE_SELECT,
																	pAttr->attrDataType,
																	0);
								
								pAttr->bIsAttributeConditionalList = true;
								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
								pAttr->byNumOfChunks++;
							}

							/*Now Parse the ddpExpression Argument of the SELECT */

							rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

							if(rc != SUCCESS)
								return rc;
							
							/*otherwise Parse all the CASE branches and the DEFAULT */
							while (len > 0)
							{
								DDL_PARSE_TAG(chunkp,&len,&tagp,&len1);

								switch(tagp)
								{
									case CASE_TAG:
										{
											/*We are parsing the CASE constants as expression
											just bcoz of the spec. But it should be a constant 
											value , ie. an expression with just a  constant (integer)
											value*/

											rc = ddl_parse_expression(chunkp,&len,&tempExpr);

											if(rc != SUCCESS)
												return rc;
/*
											ddpExpression :: iterator it;

											it = tempExpr.begin();

											if(it->byElemType != INTCST_OPCODE)
												{
	#ifdef _PARSER_DEBUG
					fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_enum_list!!!");
													return DDL_UNHANDLED_STUFF_FAILURE;
	#endif												
												}
											temp = (long)it->elem.ulConst;
*/
											tempPtrToCond->caseVals.push_back(tempExpr);
											
											/*We have the case constant value 
											Now parse the attributre value from the 
											following chunk	*/

											rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
																			&len,ENUMERATOR_SEQLIST_TAG);
											if (rc != SUCCESS)
											return rc; /* Return if not successful*/

											tempPtrToCond->byNumberOfSections++;
											
											tempExpr.clear();
										}
										break;/*End CASE_TAG*/

									case DEFAULT_TAG:
										{
/*
											temp = DEFAULT_TAG_VALUE;
											pAttr->pCond->caseVals.push_back(temp);
*/											
											tempPtrToCond->byNumberOfSections++;

											rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
																			&len,ENUMERATOR_SEQLIST_TAG);
											if (rc != SUCCESS)
											return rc; /* Return if not successful*/
										}
										break;/*End DEFAULT_TAG*/

									default:
										return DDL_ENCODING_ERROR;

								}/*End Switch tagp*/


							}/*End while*/

							if(pAttr->bIsAttributeConditionalList == true)
							{/*then we have to push this conditional value on conditionalVals*/

								pAttr->conditionalVals.push_back(tempPtrToCond);
							}


						}
						break; /*End SELECT_TAG*/

				case OBJECT_TAG : /*We have a direct object*/
					{
						if((pAttr->byNumOfChunks == 0) && length == 0)
						{ /*We have a direct list in a single chunk*/
							pAttr->pVals = new VALUES;

							pAttr->pVals->enmList = new ENUM_VALUE_LIST;

							rc = ddl_parse_enums(chunkp,&len,pAttr->pVals->enmList);

							if(rc != DDL_SUCCESS)

							return rc;

							pAttr->byNumOfChunks++;
							break;

						}
						else /*We are having a  possible combination of direct & conditional chunks*/
						{ /*Spl case of all chunks having direct Values , we'll handle after looping */

							tempVal.enmList = new ENUM_VALUE_LIST;

							rc = ddl_parse_enums(chunkp,&len,tempVal.enmList);
							
							if(rc != DDL_SUCCESS)
								return rc;

							pAttr->directVals.push_back(tempVal);

							pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);//Vibhor 200105: Changed
	
//							pAttr->bIsAttributeConditionalList = true;

									
							pAttr->byNumOfChunks++;
										
							//tempVal.enmList->clear();

				
							/*Just set the conditional flag every time we come here irrespective of 
							 whether or not its set earlier, If we have chunks of non conditionals,
							we will reset this flag later*/
							pAttr->bIsAttributeConditional = true;
										
						}/*Parse the enumerations*/
						break;
					}
					 /*End OBJECT_TAG*/
				default:
					return DDL_ENCODING_ERROR;
					break;
			}/*End switch tag*/

				
	}/*End while length > 0*/
	
	if(pAttr->pVals == NULL 
		&& pAttr->bIsAttributeConditionalList == false 
		&& (pAttr->conditionalVals.size() == 0)
		&& (pAttr->directVals.size() > 0)) /*The last one is a double check*/
	{
		/*We have a Direct list in more than one chunk!!!!
		 So we will copy the same to pAttr->Vals->enmList*/
		pAttr->pVals = new VALUES;
		pAttr->pVals->enmList = new ENUM_VALUE_LIST;

		ValueList::iterator ittVALS;
		//for (int i = 0; i < pAttr->directVals.size(); i++)// a ValueList  aka vector  <VALUES>    
		for ( ittVALS = pAttr->directVals.begin();ittVALS != pAttr->directVals.end();++ittVALS)
		{//a ptr 2a VALUES
			//tempPtrToEnumList = pAttr->directVals.at(i).enmList;
// 			tempPtrToEnumList = ((VALUES*) ittVALS)->enmList; // a ENUM_VALUE_LIST* PAW see below 03/03/09
			tempPtrToEnumList = ((VALUES*) &(*ittVALS))->enmList; // a ENUM_VALUE_LIST* see above PAW
			if ( !tempPtrToEnumList) continue; // skip nulls
			ENUM_VALUE_LIST::iterator ittENMVAL; // a ENUM_VALUE*
			ENUM_VALUE*  peV = NULL;
			//for (int j =0; j < tempPtrToEnumList->size(); j++)
			for (ittENMVAL = tempPtrToEnumList->begin(); 
							ittENMVAL != tempPtrToEnumList->end(); ++ittENMVAL)
			{// a ptr 2a ENUM_VALUE
				//tmpEnm = tempPtrToEnumList->at(j);
				//pAttr->pVals->enmList->push_back(tmpEnm);
//				peV = (ENUM_VALUE*) ittENMVAL;//PAW see below 03/03/09
				peV = (ENUM_VALUE*) &(*ittENMVAL);//PAW
				pAttr->pVals->enmList->push_back(*(peV));
				peV->Cleanup();
			}/*Endfor j*/
					
			/*Just clear this list, its not required any more*/
			tempPtrToEnumList->clear();
			//tempPtrToEnumList->erase(tempPtrToEnumList->begin(),tempPtrToEnumList->end());
			delete tempPtrToEnumList;
//			((VALUES*) ittVALS)->enmList = NULL;//PAW see below 03/03/09
			((VALUES*) &(*ittVALS))->enmList = NULL;//PAW see above
		}/*Endfor i*/
				
		/*Now clear the directVals list too*/

		pAttr->directVals.clear();

		/*Reset the bIsAttributeConditional flag*/
		pAttr->bIsAttributeConditional = false;
	
	}/*Endif*/

	/*Vibhor 180105: Start of Code*/

	/*If due to some combination both Conditional & ConditionalList Flags are set, 
	 Reset the bIsAttributeConditional */
	if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == true)
		pAttr->bIsAttributeConditional = false;

	/*Vibhor 180105: End of Code*/

	return SUCCESS;

}/*End parse_attr_enum_list*/

int parse_attr_reference_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;
	unsigned long tag,tagp,length,len,len1;
//	long temp;

	DDlConditional *tempPtrToCond = NULL;

	REFERENCE_LIST *tempPtrToReferenceList = NULL;

	ddpREFERENCE tmpRef;

	VALUES		tempVal;
	
	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	lengthp = &size;

	/*The first tag should be a REFERENCE_SEQLIST_TAG if not then return error*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(REFERENCE_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;

	*lengthp -= length;
	
	
	while(length >0)
	{
		/*Parse the Tag to know if we have a conditional or a direct object*/

		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch (tag)
				{

					case IF_TAG: /*We have an IF THEN ELSE conditional*/
						{
							if((pAttr->byNumOfChunks == 0) && length == 0)
							{/*We have a conditional in single chunk*/
							
								pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
								pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
															pAttr->attrDataType,
															1);
								tempPtrToCond = pAttr->pCond;
								pAttr->byNumOfChunks++;
							}
							else
							{/*We have a multichunk list which has conditionals*/
								tempPtrToCond = new DDlConditional(DDL_COND_TYPE_IF,
																	pAttr->attrDataType,
																	1);
								
								pAttr->bIsAttributeConditionalList = true;
								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
								pAttr->byNumOfChunks++;
							}		
							
							/*Now Parse the ddpExpression associated with the IF block */
							rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							/*otherwise Parse the value of the attribute associated with THEN clause*/
							
							rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,REFERENCE_SEQLIST_TAG);
						
							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							

							/*Parse the ELSE portion if there's one*/
							if (len >0)
							{
								rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,REFERENCE_SEQLIST_TAG);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/

								tempPtrToCond->byNumberOfSections++;

							}

							if(pAttr->bIsAttributeConditionalList == true)
							{/*then we have to push this conditional value on conditionalVals*/

								pAttr->conditionalVals.push_back(tempPtrToCond);
							}

						}
						break; /*End IF_TAG*/
				
					case SELECT_TAG: /*We have a Switch Case conditional*/
						{
							if((pAttr->byNumOfChunks == 0) && length == 0)
							{/*We have a conditional in single chunk*/
							
								pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
								pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
															pAttr->attrDataType,
															0);
								tempPtrToCond = pAttr->pCond;
								pAttr->byNumOfChunks++;
							}
							else
							{/*We have a multichunk list which has conditionals*/
								tempPtrToCond = new DDlConditional(DDL_COND_TYPE_SELECT,
																	pAttr->attrDataType,
																	0);
								
								pAttr->bIsAttributeConditionalList = true;
								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
								pAttr->byNumOfChunks++;
							}

							/*Now Parse the ddpExpression Argument of the SELECT */

							rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

							if(rc != SUCCESS)
								return rc;
							
							/*otherwise Parse all the CASE branches and the DEFAULT */
							while (len > 0)
							{
								DDL_PARSE_TAG(chunkp,&len,&tagp,&len1);

								switch(tagp)
								{
									case CASE_TAG:
										{
											/*We are parsing the CASE constants as expression
											just bcoz of the spec. But it should be a constant 
											value , ie. an expression with just a  constant (integer)
											value*/

											rc = ddl_parse_expression(chunkp,&len,&tempExpr);

											if(rc != SUCCESS)
												return rc;
/*
											ddpExpression :: iterator it;

											it = tempExpr.begin();

											if(it->byElemType != INTCST_OPCODE)
												{
	#ifdef _PARSER_DEBUG
					fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_reference_list()!!!");
													return DDL_UNHANDLED_STUFF_FAILURE;
	#endif									
												}
											temp = (long)it->elem.ulConst;
*/											
											tempPtrToCond->caseVals.push_back(tempExpr);
											
											/*We have the case constant value 
											Now parse the attributre value from the 
											following chunk	*/

											rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
																			&len,REFERENCE_SEQLIST_TAG);
											if (rc != SUCCESS)
											return rc; /* Return if not successful*/

											tempPtrToCond->byNumberOfSections++;
											
											tempExpr.clear();
										}
										break;/*End CASE_TAG*/

									case DEFAULT_TAG:
										{
/*
											temp = DEFAULT_TAG_VALUE;
											pAttr->pCond->caseVals.push_back(temp);
*/
										
											pAttr->pCond->byNumberOfSections++;

											rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
																			&len,REFERENCE_SEQLIST_TAG);
											if (rc != SUCCESS)
											return rc; /* Return if not successful*/
										}
										break;/*End DEFAULT_TAG*/

									default:
										return DDL_ENCODING_ERROR;

								}/*End Switch tagp*/


							}/*End while*/

							if(pAttr->bIsAttributeConditionalList == true)
							{/*then we have to push this conditional value on conditionalVals*/

								pAttr->conditionalVals.push_back(tempPtrToCond);
							}

						}
						break; /*End SELECT_TAG*/

					case OBJECT_TAG : /*We have a direct object*/
						{

							if((pAttr->byNumOfChunks == 0) && length == 0)
							{ /*We have a direct list in a single chunk*/
								pAttr->pVals = new VALUES;

								pAttr->pVals->refList = new REFERENCE_LIST;

								rc = ddl_parse_reflist(chunkp,&len,pAttr->pVals->refList);

								if(rc != DDL_SUCCESS)

								return rc;

								pAttr->byNumOfChunks++;
								break;

							}
							else /*We are having a  possible combination of direct & conditional chunks*/
							{ /*Spl case of all chunks having direct Values , we'll handle after looping */

								tempVal.refList = new REFERENCE_LIST;

								rc = ddl_parse_reflist(chunkp,&len,tempVal.refList);
								
								if(rc != DDL_SUCCESS)
									return rc;

								pAttr->directVals.push_back(tempVal);

								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);//Vibhor 200105: Changed
		
										
								pAttr->byNumOfChunks++;
											
											
								/*Just set the conditional flag every time we come here irrespective of 
								 whether or not its set earlier, If we have chunks of non conditionals,
								we will reset this flag later*/
								pAttr->bIsAttributeConditional = true;
										
							}
							break;
							
						}
						 /*End OBJECT_TAG*/
					default:
						return DDL_ENCODING_ERROR;
						break;
				}/*End switch tag*/
	}/*End while*/
	
	if(pAttr->pVals == NULL 
		&& pAttr->bIsAttributeConditionalList == false 
		&& (pAttr->conditionalVals.size() == 0)
		&& (pAttr->directVals.size() > 0)) /*The last one is a double check*/
	{
		/*We have a Direct list in more than one chunk!!!!
		 So we will copy the same to pAttr->Vals->enmList*/
		pAttr->pVals = new VALUES;
		pAttr->pVals->refList = new REFERENCE_LIST;
		for (unsigned i = 0; i < pAttr->directVals.size(); i++)
		{
			tempPtrToReferenceList = pAttr->directVals.at(i).refList;
			for (unsigned j =0; j < tempPtrToReferenceList->size(); j++)
			{
				tmpRef = tempPtrToReferenceList->at(j);
				pAttr->pVals->refList->push_back(tmpRef);

			}/*Endfor j*/
					
			/*Just clear this list, its not required any more*/
			tempPtrToReferenceList->clear();
		}/*Endfor i*/
				
		/*Now clear the directVals list too*/

		pAttr->directVals.clear();

		/*Reset the bIsAttributeConditional flag*/
		pAttr->bIsAttributeConditional = false;
	
	}/*Endif*/

	/*Vibhor 180105: Start of Code*/

	/*If due to some combination both Conditional & ConditionalList Flags are set, 
	 Reset the bIsAttributeConditional */
	if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == true)
		pAttr->bIsAttributeConditional = false;

	/*Vibhor 180105: End of Code*/

	return SUCCESS;


}/*End parse_attr_reference_list*/

int parse_attr_type_size(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	
//	unsigned char  *leave_pointer;	/* chunk ptr for early exit */
	int             rc;
	unsigned char **chunkp = NULL;
	unsigned long * length = NULL;
	unsigned long	tag,len;	/* parsed tag */
//	unsigned long   typesize;		/* size of tag item (2-byte int) */
	unsigned __int64 LL;
	TYPE_SIZE      vartype;

	assert(binChunk && size);

	chunkp  = &binChunk;
	length  = &size;

	/*Parse the tag to know the type of the variable, as per the spec this can't be a conditional*/
	DDL_PARSE_TAG(chunkp,length,&tag,&len);
	
	vartype.type = (unsigned short)tag;

	switch (tag)
	{
	case BOOLEAN_T:
		vartype.size = (unsigned short)BOOLEAN_SIZE;
		break;
	case INTEGER:
	case UNSIGNED:
	
	/* The length of an item is encoded
	 * with a first byte of zero, and then the following byte(s) encode the
	 * length.  If the first byte of the chunk is non-zero, then there is
	 * no length encoded, and the default value of 1 is returned.
	 */
		if(len > *length)
		{
			return DDL_ENCODING_ERROR;
		}
		
		*length -= len;

		if((0 == len) || **chunkp)
		{
			vartype.size =(unsigned short) DEFAULT_SIZE;
		}
		else /*Parse the length*/
		{
		/*
		 * Skip the zero, and parse the size.
		 */

			(*chunkp)++;
			(len)--;

		DDL_PARSE_INTEGER(chunkp, &len, &LL);

		vartype.size = (unsigned short)LL;
		}
		break;
	case FLOATG_PT:
		if(len > *length)
		{
			return DDL_ENCODING_ERROR;
		}
		*length -= len;

		vartype.size = (unsigned short)FLOAT_SIZE;

		break;
	case DOUBLE_FLOAT: 
		if(len > *length)
		{
			return DDL_ENCODING_ERROR;
		}
		*length -= len;
		vartype.size = (unsigned short)DOUBLE_SIZE;
		break;
	case ENUMERATED:
	case BIT_ENUMERATED:
	case INDEX:
		if(len > *length)
		{
			return DDL_ENCODING_ERROR;
		}
		*length -= len;

		if((0 == len) || **chunkp)
		{
			vartype.size = (unsigned short)DEFAULT_SIZE;
		}
		else /*Parse the length*/
		{
		/*
		 * Skip the zero, and parse the size.
		 */

			(*chunkp)++;
			(len)--;

		DDL_PARSE_INTEGER(chunkp, &len, &LL);

		vartype.size = (unsigned short)LL;
		}
		break;

	case ASCII:			/*These four cases don't have explicit length tag for size*/
	case PACKED_ASCII:
	case PASSWORD:
	case BITSTRING:

		DDL_PARSE_INTEGER(chunkp,length,&LL);
		vartype.size = (unsigned short)LL;
		break;
	case HART_DATE_FORMAT:
		vartype.size = (unsigned short) DATE_SIZE;
		break;
//FF	case TIME:
//FF		vartype.size = (unsigned short) TIME_SIZE;
//FF		break;
	case TIME_VALUE:
		vartype.size = (unsigned short) TIME_VALUE_SIZE;
		break;
//FF	case DATE_AND_TIME:
//FF		vartype.size = (unsigned short) DATE_AND_TIME_SIZE;
//FF		break;
//FF	case DURATION:		/* DURATION */
//FF		vartype.size = (unsigned short) DURATION_SIZE;
//FF		break;	
//FF	case EUC:
		
//FF		DDL_PARSE_INTEGER(chunkp,length,&typesize);
//FF		vartype.size = (unsigned short)typesize;

//FF		break;
//FF	case OCTETSTRING:	//TODO: These are undefined at this time
//FF	case VISIBLESTRING:
//	case BOOLEAN_T:
	default:
		return DDL_ENCODING_ERROR;
		break;

	}/*End Switch tag*/

  pAttr->pVals = new VALUES;
// PAW start debugging
//  pAttr->pVals->typeSize = vartype;// PAW 20/03/09 revised to stop corruption
	pAttr->pVals->typeSize.size = (unsigned short)vartype.size;
	pAttr->pVals->typeSize.type = (unsigned short)vartype.type;
// PAW 20/03/09 end

  return SUCCESS;

} /*End parse_attr_type_size*/




int parse_attr_transaction_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{

	int rc;
	unsigned long tag,tag1,len,len1,len2;//,tmpInt;

	unsigned  char** chunkp = NULL;

	unsigned long *length = NULL;
	unsigned __int64 LL;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	TRANSACTION tmpTrans;

	pAttr->pVals = new VALUES;

	pAttr->pVals->transList = new TRANSACTION_LIST;

	while(*length > 0)
	{
		//26aug10   was::> (void)memset((char*)&tmpTrans,0,sizeof(TRANSACTION));
		TRANSACTION tmpTrans;// 26aug10  try to stop the 
							//  _SCL_SECURE_VALIDATE(this->_Has_container());
							// assertion.

		/*Parse the tag it should be TRANSACTION_TAG*/

		DDL_PARSE_TAG(chunkp,length,&tag,&len);

		if(tag != TRANSACTION_TAG)
			return DDL_ENCODING_ERROR;

		*length -= len;

		/*Parse the transaction number*/	

		DDL_PARSE_INTEGER(chunkp,&len,&LL);
		
		tmpTrans.number = (UINT32)LL;

		/*
		 * The rest of the TRANSACTION definition consists of a series of
		 * REQUEST, REPLY, and RESPONSE_CODE entries.  Parse the tag to
		 * determine which of the entries each one is, and then parse the entry.
		 */
		 while(len > 0)
		 {
			DDL_PARSE_TAG(chunkp,&len,&tag,&len1);

			switch(tag)
			{
				case	TRANS_REQ_TAG:
					rc = ddl_parse_dataitems(chunkp,&len,&tmpTrans.request);
					if(rc != DDL_SUCCESS)
						return rc;
					break;

				case	TRANS_REPLY_TAG:
					rc = ddl_parse_dataitems(chunkp,&len,&tmpTrans.reply);
					if(rc != DDL_SUCCESS)
						return rc;
					break;
				case	TRANS_RESP_CODES_TAG:
					
					DDL_PARSE_TAG(chunkp,&len,&tag1,&len1);

					if(tag1 != RSPCODES_SEQLIST_TAG)
						return DDL_ENCODING_ERROR;

					len -= len1;

					/*Parse the tag to ascertain whether its a IF / SELECT or a DIRECT object*/
					/*Its an assumption that we won't encounter conditional here*/


					DDL_PARSE_TAG(chunkp,&len1,&tag1,&len2);
					
					switch(tag1)
					{
						case IF_TAG:
						case SELECT_TAG:
#ifdef _PARSER_DEBUG

						LOGIT(CERR_LOG,"\nConditional encountered in parsing DATA_ITEM_LIST!!!!\n");

#endif /* _PARSER_DEBUG */
						return DDL_UNHANDLED_STUFF_FAILURE;
						break;

						case OBJECT_TAG:

							rc = ddl_parse_respcodes(chunkp,&len1,&tmpTrans.rcodes);
							if(rc != DDL_SUCCESS)
							return rc;
							break;
						default:
							return DDL_ENCODING_ERROR;
							break;
					}/*End switch*/
					break;

				case	TRANS_POSTRCV_ACTION_TAG:

					DDL_PARSE_TAG(chunkp,&len,&tag1,&len1);

					if(tag1 != REFERENCE_SEQLIST_TAG)
						return DDL_ENCODING_ERROR;

					len -= len1;

					/*Parse the tag to ascertain whether its a IF / SELECT or a DIRECT object*/
					/*Its an assumption that we won't encounter conditional here*/


					DDL_PARSE_TAG(chunkp,&len1,&tag1,&len2);
					
					switch(tag1)
					{
						case IF_TAG:
						case SELECT_TAG:
#ifdef _PARSER_DEBUG

						LOGIT(CERR_LOG,"\nConditional encountered in parsing POS RQST ACTION LIST!!!!\n");

#endif /* _PARSER_DEBUG */
						return DDL_UNHANDLED_STUFF_FAILURE;
						break;

						case OBJECT_TAG:

							rc = ddl_parse_reflist(chunkp,&len2,&tmpTrans.post_rqst_rcv_act);

							if(rc != DDL_SUCCESS)
								return rc;

							break;
						default:
							return DDL_ENCODING_ERROR;
							break;
					}/*End switch*/
					break;


				default:
					return DDL_ENCODING_ERROR;
			}/*switch tag*/

		 }/*End while len > 0*/

	pAttr->pVals->transList->push_back(tmpTrans);
	}/*End while*/

	return SUCCESS;

}/*End parse_attr_transaction_list*/




int parse_attr_resp_code_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{

	int rc;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;
	unsigned long tag,tagp,length,len,len1;
//	long temp;

	DDlConditional *tempPtrToCond = NULL;

	RESPONSE_CODE_LIST *tempPtrToRespCodeList = NULL;

	RESPONSE_CODE tmpRespCode;

	VALUES		tempVal;

	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	lengthp = &size;

	/*The first tag should be a RSPCODES_SEQLIST_TAG if not then return error*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(RSPCODES_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;

	*lengthp -= length;// never used, apparently for debugging
	
	
	while(length > 0)
	{	
		/*Parse the Tag to know if we have a conditional or a direct object*/

		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch (tag)
		{
		case IF_TAG: /*We have an IF THEN ELSE conditional*/
			{
				if((pAttr->byNumOfChunks == 0) && length == 0)
				{/*We have a conditional in single chunk*/
				
					pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
					pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
												pAttr->attrDataType,
												1);
					tempPtrToCond = pAttr->pCond;
					pAttr->byNumOfChunks++;
				}
				else
				{/*We have a multichunk list which has conditionals*/
					tempPtrToCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);
					
					pAttr->bIsAttributeConditionalList = true;
					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
					pAttr->byNumOfChunks++;
				}			
				
				/*Now Parse the ddpExpression associated with the IF block */
				rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

				/*otherwise Parse the value of the attribute associated with THEN clause*/
				
				rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,RSPCODES_SEQLIST_TAG);
			
				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

				

				/*Parse the ELSE portion if there's one*/
				if (len >0)
				{
					rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,RSPCODES_SEQLIST_TAG);
					if (rc != SUCCESS)
					return rc; /* Return if not successful*/

					tempPtrToCond->byNumberOfSections++;

				}

				if(pAttr->bIsAttributeConditionalList == true)
				{/*then we have to push this conditional value on conditionalVals*/

					pAttr->conditionalVals.push_back(tempPtrToCond);
				}

			}
			break; /*End IF_TAG*/
	
		case SELECT_TAG: /*We have a Switch Case conditional*/
			{
				if((pAttr->byNumOfChunks == 0) && length == 0)
				{/*We have a conditional in single chunk*/
				
					pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
					pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
												pAttr->attrDataType,
												0);
					tempPtrToCond = pAttr->pCond;
					pAttr->byNumOfChunks++;
				}
				else
				{/*We have a multichunk list which has conditionals*/
					tempPtrToCond = new DDlConditional(DDL_COND_TYPE_SELECT,
														pAttr->attrDataType,
														0);
					
					pAttr->bIsAttributeConditionalList = true;
					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
					pAttr->byNumOfChunks++;
				}
				
				/*Now Parse the ddpExpression Argument of the SELECT */

				rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

				if(rc != SUCCESS)
					return rc;
				
				/*otherwise Parse all the CASE branches and the DEFAULT */
				while (len > 0)
				{
//sjv 18apr06								DDL_PARSE_TAG(chunkp,&length,&tagp,&len1);
					DDL_PARSE_TAG(chunkp,&len,&tagp,&len1);
//sjv 29jun06- using len in the parses within this while loop has nothing to do with Implicit
//			   it is equivelent to doing a 'len -= len1;' here, but doing it a little at a time
					switch(tagp)
					{
					case CASE_TAG:
						{
							/*We are parsing the CASE constants as expression
							just bcoz of the spec. But it should be a constant 
							value , ie. an expression with just a  constant (integer)
							value*/

							rc = ddl_parse_expression(chunkp,&len,&tempExpr);

							if(rc != SUCCESS)
								return rc;
/*
							ddpExpression :: iterator it;

							it = tempExpr.begin();

							if(it->byElemType != INTCST_OPCODE)
								{
#ifdef _PARSER_DEBUG
	fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_resp_code_list()!!!");
								return DDL_UNHANDLED_STUFF_FAILURE;
#endif												
								}
							temp = (long)it->elem.ulConst;
*/
							tempPtrToCond->caseVals.push_back(tempExpr);
							
							/*We have the case constant value 
							Now parse the attributre value from the 
							following chunk	*/

							rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
															&len,RSPCODES_SEQLIST_TAG);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/

							tempPtrToCond->byNumberOfSections++;
							
							tempExpr.clear();
						}
						break;/*End CASE_TAG*/

					case DEFAULT_TAG:
						{
/*
							temp = DEFAULT_TAG_VALUE;
							pAttr->pCond->caseVals.push_back(temp);
*/
							
							tempPtrToCond->byNumberOfSections++;

							rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
															&len,RSPCODES_SEQLIST_TAG);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/
						}
						break;/*End DEFAULT_TAG*/

					default:
						return DDL_ENCODING_ERROR;

					}/*End Switch tagp*/


				}/*End while*/

				if(pAttr->bIsAttributeConditionalList == true)
				{/*then we have to push this conditional value on conditionalVals*/

					pAttr->conditionalVals.push_back(tempPtrToCond);
				}


			}
			break; /*End SELECT_TAG*/

		case OBJECT_TAG : /*We have a direct object*/
			{
				if((pAttr->byNumOfChunks == 0) && length == 0)
				{ /*We have a direct list in a single chunk*/
					pAttr->pVals = new VALUES;

					pAttr->pVals->respCdList = new RESPONSE_CODE_LIST;

					rc = ddl_parse_respcodes(chunkp,&len,pAttr->pVals->respCdList);

					if(rc != DDL_SUCCESS)

					return rc;

					pAttr->byNumOfChunks++;
					break;

				}
				else /*We are having a  possible combination of direct & conditional chunks*/
				{ /*Spl case of all chunks having direct Values , we'll handle after looping */

					tempVal.respCdList = new RESPONSE_CODE_LIST;

					rc = ddl_parse_respcodes(chunkp,&len,tempVal.respCdList);
					
					if(rc != DDL_SUCCESS)
						return rc;

					pAttr->directVals.push_back(tempVal);

					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);//Vibhor 200105: Changed

							
					pAttr->byNumOfChunks++;
								
								
					/*Just set the conditional flag every time we come here irrespective of 
					 whether or not its set earlier, If we have chunks of non conditionals,
					we will reset this flag later*/
					pAttr->bIsAttributeConditional = true;
							
				}
				break; 
			
			}
			/*End OBJECT_TAG*/
		default:
			return DDL_ENCODING_ERROR;
			break;
		}/*End switch tag*/
				
	}/*End while*/

	if(pAttr->pVals == NULL 
		&& pAttr->bIsAttributeConditionalList == false 
		&& (pAttr->conditionalVals.size() == 0)
		&& (pAttr->directVals.size() > 0) ) /*The last one is a double check*/
	{
		/*We have a Direct list in more than one chunk!!!!
		 So we will copy the same to pAttr->Vals->enmList*/
		pAttr->pVals = new VALUES;
		pAttr->pVals->respCdList = new RESPONSE_CODE_LIST;

		for (unsigned i = 0; i < pAttr->directVals.size(); i++)
		{
			tempPtrToRespCodeList = pAttr->directVals.at(i).respCdList;
			for (unsigned j =0; j < tempPtrToRespCodeList->size(); j++)
			{
				tmpRespCode = tempPtrToRespCodeList->at(j);
				pAttr->pVals->respCdList->push_back(tmpRespCode);

			}/*Endfor j*/
					
			/*Just clear this list, its not required any more*/
			tempPtrToRespCodeList->clear();
		}/*Endfor i*/
				
		/*Now clear the directVals list too*/

		pAttr->directVals.clear();

		/*Reset the bIsAttributeConditional flag*/
		pAttr->bIsAttributeConditional = false;
	
	}/*Endif*/

	/*Vibhor 180105: Start of Code*/

	/*If due to some combination both Conditional & ConditionalList Flags are set, 
	 Reset the bIsAttributeConditional */
	if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == true)
		pAttr->bIsAttributeConditional = false;

	/*Vibhor 180105: End of Code*/

	return SUCCESS;

}/*End parse_attr_resp_code_list*/

int parse_attr_menu_item_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;
	unsigned long tag,tagp,length,len,len1;
//	long temp;

	DDlConditional *tempPtrToCond = NULL;

	MENU_ITEM_LIST *tempPtrToMenuItemList = NULL;

	MENU_ITEM tmpMenuItem;

	VALUES		tempVal;

	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	lengthp = &size;

	/*The first tag should be a MENU_ITEM_SEQLIST_TAG if not then return error*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(MENU_ITEM_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;

	*lengthp -= length;// never used, apparently for debugging

	/*Parse the Tag to know if we have a conditional or a direct object*/

	while(length >0)
	{
		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch (tag)
		{
		case IF_TAG: /*We have an IF THEN ELSE conditional*/
			{
				if((pAttr->byNumOfChunks == 0) && length == 0)
				{/*We have a conditional in single chunk*/
				
					pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
					pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
												pAttr->attrDataType,
												1);
					tempPtrToCond = pAttr->pCond;
					pAttr->byNumOfChunks++;
				}
				else
				{/*We have a multichunk list which has conditionals*/
					tempPtrToCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);
					
					pAttr->bIsAttributeConditionalList = true;
					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
					pAttr->byNumOfChunks++;
				}	

				/*Now Parse the ddpExpression associated with the IF block */
				rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

				/*otherwise Parse the value of the attribute associated with THEN clause*/
				
				rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,MENU_ITEM_SEQLIST_TAG);
			
				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

				

				/*Parse the ELSE portion if there's one*/
				if (len >0)
				{
					rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,MENU_ITEM_SEQLIST_TAG);
					if (rc != SUCCESS)
						return rc; /* Return if not successful*/

					tempPtrToCond->byNumberOfSections++;

				}

				if(pAttr->bIsAttributeConditionalList == true)
				{/*then we have to push this conditional value on conditionalVals*/

					pAttr->conditionalVals.push_back(tempPtrToCond);
				}

			}
			break; /*End IF_TAG*/
	
		case SELECT_TAG: /*We have a Switch Case conditional*/
			{				
				if((pAttr->byNumOfChunks == 0) && length == 0)
				{/*We have a conditional in single chunk*/
				
					pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
					pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
												pAttr->attrDataType,
												0);
					tempPtrToCond = pAttr->pCond;
					pAttr->byNumOfChunks++;
				}
				else
				{/*We have a multichunk list which has conditionals*/
					tempPtrToCond = new DDlConditional(DDL_COND_TYPE_SELECT,pAttr->attrDataType,0);
					
					pAttr->bIsAttributeConditionalList = true;
					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
					pAttr->byNumOfChunks++;
				}

				/*Now Parse the ddpExpression Argument of the SELECT */

				rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

				if(rc != SUCCESS)
					return rc;
				
				/*otherwise Parse all the CASE branches and the DEFAULT */
				while (len > 0)
				{
//sjv 18apr06								DDL_PARSE_TAG(chunkp,&length,&tagp,&len1);
					DDL_PARSE_TAG(chunkp,&len,&tagp,&len1);
//sjv 18apr06-comment 29jun06 
//			 - using len in the parses within this while loop has nothing to do with Implicit
//			   it is equivelent to doing a 'len -= len1;' here ( and using len1 there ), 
//			   but doing the subtraction a little at a time
					switch(tagp)
					{
					case CASE_TAG:
						{
							/*We are parsing the CASE constants as expression
							just bcoz of the spec. But it should be a constant 
							value , ie. an expression with just a  constant (integer)
							value*/

							rc = ddl_parse_expression(chunkp,&len,&tempExpr);// Implicit tag, do not use len1

							if(rc != SUCCESS)
								return rc;
/*
							ddpExpression :: iterator it;

							it = tempExpr.begin();

							if(it->byElemType != INTCST_OPCODE)
								{
#ifdef _PARSER_DEBUG
	fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_menu_item_list() !!!");
									return DDL_UNHANDLED_STUFF_FAILURE;
#endif												
								}
							temp = (long)it->elem.ulConst;
*/
							tempPtrToCond->caseVals.push_back(tempExpr);
							
							/*We have the case constant value 
							Now parse the attributre value from the 
							following chunk	*/

							rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
											&len,MENU_ITEM_SEQLIST_TAG);// Implicit tag, do not use len1
							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							tempPtrToCond->byNumberOfSections++;
							
							tempExpr.clear();
						}
						break;/*End CASE_TAG*/

					case DEFAULT_TAG:
						{
/*
							temp = DEFAULT_TAG_VALUE;
							pAttr->pCond->caseVals.push_back(temp);
*/									
							
							tempPtrToCond->byNumberOfSections++;

							rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
											&len,MENU_ITEM_SEQLIST_TAG);// Implicit tag, do not use len1
							if (rc != SUCCESS)
								return rc; /* Return if not successful*/
						}
						break;/*End DEFAULT_TAG*/

					default:
						return DDL_ENCODING_ERROR;

					}/*End Switch tagp*/


				}/*End while*/

				if(pAttr->bIsAttributeConditionalList == true)
				{/*then we have to push this conditional value on conditionalVals*/

					pAttr->conditionalVals.push_back(tempPtrToCond);
				}

			}
			break; /*End SELECT_TAG*/

		case OBJECT_TAG : /*We have a direct object*/
			{

				if((pAttr->byNumOfChunks == 0) && length == 0)
				{ /*We have a direct list in a single chunk*/
					pAttr->pVals = new VALUES;

					pAttr->pVals->menuItemsList = new MENU_ITEM_LIST;

					rc = ddl_parse_menuitems(chunkp,&len,pAttr->pVals->menuItemsList);

					if(rc != DDL_SUCCESS)
						return rc;

					pAttr->byNumOfChunks++;
					break;

				}
				else /*We are having a  possible combination of direct & conditional chunks*/
				{ /*Spl case of all chunks having direct Values , we'll handle after looping */

					tempVal.menuItemsList = new MENU_ITEM_LIST;

					rc = ddl_parse_menuitems(chunkp,&len,tempVal.menuItemsList);
					
					if(rc != DDL_SUCCESS)
						return rc;

					pAttr->directVals.push_back(tempVal);

					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);//Vibhor 200105: Changed

							
					pAttr->byNumOfChunks++;
								
								
					/*Just set the conditional flag every time we come here irrespective of 
					 whether or not its set earlier, If we have chunks of non conditionals,
					we will reset this flag later*/
					pAttr->bIsAttributeConditional = true;
							
				}
				break;
				
			}
			 /*End OBJECT_TAG*/
		default:
			return DDL_ENCODING_ERROR;
			break;
		}/*End switch tag*/
	}/*End while length > 0 */

	if(pAttr->pVals == NULL 
		&& pAttr->bIsAttributeConditionalList == false 
		&& (pAttr->conditionalVals.size() == 0)
		&& (pAttr->directVals.size() > 0)) /*The last one is a double check*/
	{
		/*We have a Direct list in more than one chunk!!!!
		 So we will copy the same to pAttr->Vals->enmList*/
		pAttr->pVals = new VALUES;
		pAttr->pVals->menuItemsList = new MENU_ITEM_LIST;
		for (unsigned i = 0; i < pAttr->directVals.size(); i++)
		{
			tempPtrToMenuItemList = pAttr->directVals.at(i).menuItemsList;
			for (unsigned j =0; j < tempPtrToMenuItemList->size(); j++)
			{
				tmpMenuItem = tempPtrToMenuItemList->at(j);
				pAttr->pVals->menuItemsList->push_back(tmpMenuItem);
				tmpMenuItem.Cleanup();

			}/*Endfor j*/
					
			/*Just clear this list, its not required any more*/
			tempPtrToMenuItemList->clear();

//stevev - merge - 20feb07 - make it always #if _MSC_VER >= 1300  // HOMZ - port to 2003, VS7 - Memory Leak fix
			delete tempPtrToMenuItemList;
//#endif
		}/*Endfor i*/
				
		/*Now clear the directVals list too*/

		pAttr->directVals.clear();

		/*Reset the bIsAttributeConditional flag*/
		pAttr->bIsAttributeConditional = false;
	}/*Endif*/

	/*Vibhor 180105: Start of Code*/

	/*If due to some combination both Conditional & ConditionalList Flags are set, 
	 Reset the bIsAttributeConditional */
	if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == true)
		pAttr->bIsAttributeConditional = false;

	/*Vibhor 180105: End of Code*/


	return SUCCESS;


}/*End parse_attr_menu_item_list*/



int parse_attr_definition(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	pAttr->pVals = new VALUES;
	(void)memset((char*)&pAttr->pVals->defData,0,sizeof(DEFINITION));//ok

	return ddl_parse_definition(binChunk,size,&pAttr->pVals->defData);
}
 
int parse_attr_refresh_relation(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;
	unsigned long tag,length,len;

	assert(binChunk && size);

	chunkp = &binChunk;
	lengthp = &size;


	 /*A refresh relation consists of a list of watch items and a list of update items
	 both are reference lists*/

	 


	/*The first tag should be a REFERENCE_SEQLIST_TAG if not then return error*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(REFERENCE_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;

	*lengthp -= length; /*The remaining length should contain the update list!!*/

	pAttr->pVals = new VALUES;

	pAttr->pVals->refrshReln = new REFRESH_RELATION;


	/*ASSUMPTION: Since we are parsing a reference list as a subattribute 
	we r not expecting conditionals */

	/*Parse the watch list*/
	while (length > 0)
	{

		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch(tag)
		{
			case	IF_TAG:
			case	SELECT_TAG:
				/*Log this one!!!!*/
#ifdef _PARSER_DEBUG

				LOGIT(CERR_LOG,"\n Conditional encountered in REFRESH RELATION subattribute!!!!\n");

#endif /*_PARSER_DEBUG*/
				return DDL_UNHANDLED_STUFF_FAILURE;

				break;
			case	OBJECT_TAG:
				{
					
					rc = ddl_parse_reflist(chunkp,&len,&pAttr->pVals->refrshReln->watch_list);
					if(rc != DDL_SUCCESS)
						return rc;

				}
				break;

			default:
				return DDL_ENCODING_ERROR;
				break;
		}/*End switch*/
	
	}/*End while*/	

	/*Parse the update list*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(REFERENCE_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;
	
	
	while(length > 0)
	{

		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch(tag)
		{
			case	IF_TAG:
			case	SELECT_TAG:
				/*Log this one!!!!*/
#ifdef _PARSER_DEBUG

				LOGIT(CERR_LOG,"\n Conditional encountered in REFRESH RELATION subattribute!!!!\n");

#endif /*_PARSER_DEBUG*/
				return DDL_UNHANDLED_STUFF_FAILURE;
				break;
			case	OBJECT_TAG:
				{
					
					rc = ddl_parse_reflist(chunkp,&len,&pAttr->pVals->refrshReln->update_list);
					if(rc != DDL_SUCCESS)
						return rc;

				}
				break;
			default:
				return DDL_ENCODING_ERROR;
				break;
		}/*End switch*/
	
	}/*End while */

	return SUCCESS;

}

int parse_attr_unit_relation(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;
	unsigned long tag,length,len;

	assert(binChunk && size);

	chunkp = &binChunk;
	lengthp = &size;

	/*A unit relation consists of a reference to a unit variable & a list of 
	 references to vars which have that varible as a unit */
	
	/*Since we can't handle conditionals as subattribute we r expecting direct objects*/

	pAttr->pVals = new VALUES;

	pAttr->pVals->unitReln = new UNIT_RELATION;

	/*Parse the unit var*/

	DDL_PARSE_TAG(chunkp,lengthp,&tag,&length);

	*lengthp -= length;
	
	switch(tag)
		{
			case	IF_TAG:
			case	SELECT_TAG:
			/*Log this one!!!!*/
	#ifdef _PARSER_DEBUG

				LOGIT(CERR_LOG,"\n Conditional encountered in UNIT RELATION subattribute!!!!\n");

	#endif /*_PARSER_DEBUG*/
				return DDL_UNHANDLED_STUFF_FAILURE;
				break;
				
			case	OBJECT_TAG:

				rc = ddl_parse_ref(chunkp,&length,&pAttr->pVals->unitReln->unit_var);
				if(rc != DDL_SUCCESS)
					return rc;
				break;
			default:
				return DDL_ENCODING_ERROR;
				break;
		}/*End switch*/

	
	/*Now parse the var list*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(REFERENCE_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;
	
	*lengthp -= length;	
	
	while(length >0)
	{

		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch(tag)
		{
			case	IF_TAG:
			case	SELECT_TAG:
				/*Log this one!!!!*/
#ifdef _PARSER_DEBUG

				LOGIT(CERR_LOG,"\n Conditional encountered in UNIT RELATION subattribute!!!!\n");

#endif /*_PARSER_DEBUG*/
				return DDL_UNHANDLED_STUFF_FAILURE;
				break;
			case	OBJECT_TAG:
				{
					
					rc = ddl_parse_reflist(chunkp,&len,&pAttr->pVals->unitReln->var_units);
					if(rc != DDL_SUCCESS)
						return rc;

				}
				break;
			default:
				return DDL_ENCODING_ERROR;
				break;
		}/*End switch*/
	
	}/*End while*/

	return SUCCESS;

}

int parse_attr_item_array_element_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;
	unsigned long tag,tagp,length,len,len1;
//	long temp;

//	DDlConditional *tempPtrToCond = NULL;

	ITEM_ARRAY_ELEMENT_LIST *tempPtrToElementList = NULL;

	ITEM_ARRAY_ELEMENT tmpElement;

	VALUES		tempVal;

	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	lengthp = &size;

	/*The first tag should be a MENU_ITEM_SEQLIST_TAG if not then return error*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(ELEMENT_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;

	*lengthp -= length;

	while(length > 0)
	{
		DDlConditional *tempPtrToCond = NULL;

		/*Parse the Tag to know if we have a conditional or a direct object*/

		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch (tag)
				{

					case IF_TAG: /*We have an IF THEN ELSE conditional*/
						{
							if((pAttr->byNumOfChunks == 0) && length == 0)
							{/*We have a conditional in single chunk*/
							
								pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
								pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
															pAttr->attrDataType,
															1);
								tempPtrToCond = pAttr->pCond;
								pAttr->byNumOfChunks++;
							}
							else
							{/*We have a multichunk list which has conditionals*/
								tempPtrToCond = new DDlConditional(DDL_COND_TYPE_IF,
																	pAttr->attrDataType,
																	1);
								
								pAttr->bIsAttributeConditionalList = true;
								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
								pAttr->byNumOfChunks++;
							}
						
							/*Now Parse the ddpExpression associated with the IF block */
							rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							/*otherwise Parse the value of the attribute associated with THEN clause*/
							
							rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,ELEMENT_SEQLIST_TAG);
						
							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							

							/*Parse the ELSE portion if there's one*/
							if (len >0)
							{
								rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,ELEMENT_SEQLIST_TAG);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/

								tempPtrToCond->byNumberOfSections++;

							}

							if(pAttr->bIsAttributeConditionalList == true)
							{/*then we have to push this conditional value on conditionalVals*/

								pAttr->conditionalVals.push_back(tempPtrToCond);
							}
						

						}
						break; /*End IF_TAG*/
				
					case SELECT_TAG: /*We have a Switch Case conditional*/
						{

							if((pAttr->byNumOfChunks == 0) && length == 0)
							{/*We have a conditional in single chunk*/
							
								pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
								pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
															pAttr->attrDataType,
															0);
								tempPtrToCond = pAttr->pCond;
								pAttr->byNumOfChunks++;
							}
							else
							{/*We have a multichunk list which has conditionals*/
								tempPtrToCond = new DDlConditional(DDL_COND_TYPE_SELECT,
																	pAttr->attrDataType,
																	0);
								
								pAttr->bIsAttributeConditionalList = true;
								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
								pAttr->byNumOfChunks++;
							}
							
							/*Now Parse the ddpExpression Argument of the SELECT */

							rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

							if(rc != SUCCESS)
								return rc;
							
							/*otherwise Parse all the CASE branches and the DEFAULT */
							while (len> 0)
							{
								DDL_PARSE_TAG(chunkp,&len,&tagp,&len1);

								switch(tagp)
								{
									case CASE_TAG:
										{
											/*We are parsing the CASE constants as expression
											just bcoz of the spec. But it should be a constant 
											value , ie. an expression with just a  constant (integer)
											value*/

											rc = ddl_parse_expression(chunkp,&len,&tempExpr);

											if(rc != SUCCESS)
												return rc;
/*
											ddpExpression :: iterator it;

											it = tempExpr.begin();

											if(it->byElemType != INTCST_OPCODE)
												{
	#ifdef _PARSER_DEBUG
					fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_item_array_element_list()!!!");\
												return DDL_UNHANDLED_STUFF_FAILURE;
	#endif												
												}
											temp = (long)it->elem.ulConst;
*/
											pAttr->pCond->caseVals.push_back(tempExpr);
											
											/*We have the case constant value 
											Now parse the attributre value from the 
											following chunk	*/

											rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
																			&len,ELEMENT_SEQLIST_TAG);
											if (rc != SUCCESS)
											return rc; /* Return if not successful*/

											tempPtrToCond->byNumberOfSections++;
											
											tempExpr.clear();
										}
										break;/*End CASE_TAG*/

									case DEFAULT_TAG:
										{
/*
											temp = DEFAULT_TAG_VALUE;
											pAttr->pCond->caseVals.push_back(temp);
*/											
											tempPtrToCond->byNumberOfSections++;

											rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
																			&len,ELEMENT_SEQLIST_TAG);
											if (rc != SUCCESS)
											return rc; /* Return if not successful*/
										}
										break;/*End DEFAULT_TAG*/

									default:
										return DDL_ENCODING_ERROR;

								}/*End Switch tagp*/


							}/*End while*/

							if(pAttr->bIsAttributeConditionalList == true)
							{/*then we have to push this conditional value on conditionalVals*/

								pAttr->conditionalVals.push_back(tempPtrToCond);
							}

						}
						break; /*End SELECT_TAG*/

					case OBJECT_TAG : /*We have a direct object*/
						{

							if((pAttr->byNumOfChunks == 0) && length == 0)
							{ /*We have a direct list in a single chunk*/
								pAttr->pVals = new VALUES;

								pAttr->pVals->itemArrElmnts = new ITEM_ARRAY_ELEMENT_LIST;

								rc = ddl_parse_itemarray(chunkp,&len,pAttr->pVals->itemArrElmnts);

								if(rc != DDL_SUCCESS)

								return rc;

								pAttr->byNumOfChunks++;
								break;

							}
							else /*We are having a  possible combination of direct & conditional chunks*/
							{ /*Spl case of all chunks having direct Values , we'll handle after looping */

								tempVal.itemArrElmnts = new ITEM_ARRAY_ELEMENT_LIST;

								rc = ddl_parse_itemarray(chunkp,&len,tempVal.itemArrElmnts);
								
								if(rc != DDL_SUCCESS)
									return rc;

								pAttr->directVals.push_back(tempVal);

								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);
		
	//							pAttr->bIsAttributeConditionalList = true;

										
								pAttr->byNumOfChunks++;
											
								//tempVal.enmList->clear();

					
								/*Just set the conditional flag every time we come here irrespective of 
								 whether or not its set earlier, If we have chunks of non conditionals,
								we will reset this flag later*/
								pAttr->bIsAttributeConditional = true;
										
							}
							break;
						}
						/*End OBJECT_TAG*/
					default:
						return DDL_ENCODING_ERROR;
						break;
				}/*End switch tag*/
	
	}/*End while length >0 */

	if(pAttr->pVals == NULL 
		&& pAttr->bIsAttributeConditionalList == false 
		&& (pAttr->conditionalVals.size() == 0)
		&& (pAttr->directVals.size() > 0)) /*The last one is a double check*/
	{
		/*We have a Direct list in more than one chunk!!!!
		 So we will copy the same to pAttr->Vals->enmList*/
		pAttr->pVals = new VALUES;
		pAttr->pVals->itemArrElmnts = new ITEM_ARRAY_ELEMENT_LIST;
		for (unsigned i = 0; i < pAttr->directVals.size(); i++)
		{
			tempPtrToElementList = pAttr->directVals.at(i).itemArrElmnts;
			for (unsigned j =0; j < tempPtrToElementList->size(); j++)
			{
				tmpElement = tempPtrToElementList->at(j);
				pAttr->pVals->itemArrElmnts->push_back(tmpElement);

			}/*Endfor j*/
					
			/*Just clear this list, its not required any more*/
			tempPtrToElementList->clear();
		}/*Endfor i*/
				
		/*Now clear the directVals list too*/

		pAttr->directVals.clear();

		/*Reset the bIsAttributeConditional flag*/
		pAttr->bIsAttributeConditional = false;
	
	}/*Endif*/

	/*Vibhor 180105: Start of Code*/

	/*If due to some combination both Conditional & ConditionalList Flags are set, 
	 Reset the bIsAttributeConditional */
	if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == true)
		pAttr->bIsAttributeConditional = false;

	/*Vibhor 180105: End of Code*/


	return SUCCESS;

}/*End parse_attr_item_array_element_list*/

int parse_attr_member_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{

	int rc;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;
	unsigned long tag,tagp,length,len,len1;
//	long temp;


	DDlConditional *tempPtrToCond = NULL;

	MEMBER_LIST *tempPtrToMemberList = NULL;

	MEMBER tmpMember;

	VALUES		tempVal;

	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	lengthp = &size;

	/*The first tag should be a MENU_ITEM_SEQLIST_TAG if not then return error*/

	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(MEMBER_SEQLIST_TAG != tag)
		return DDL_ENCODING_ERROR;

	*lengthp -= length;

	
	while(length > 0)
	{
	
		/*Parse the Tag to know if we have a conditional or a direct object*/

		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch (tag)
				{

					case IF_TAG: /*We have an IF THEN ELSE conditional*/
						{
							if((pAttr->byNumOfChunks == 0) && length == 0)
							{/*We have a conditional in single chunk*/
							
								pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
								pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
															pAttr->attrDataType,
															1);
								tempPtrToCond = pAttr->pCond;
								pAttr->byNumOfChunks++;
							}
							else
							{/*We have a multichunk list which has conditionals*/
								tempPtrToCond = new DDlConditional(DDL_COND_TYPE_IF,
																	pAttr->attrDataType,
																	1);
								
								pAttr->bIsAttributeConditionalList = true;
								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
								pAttr->byNumOfChunks++;
							}	
							
							/*Now Parse the ddpExpression associated with the IF block */
							rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							/*otherwise Parse the value of the attribute associated with THEN clause*/
							
							rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,MEMBER_SEQLIST_TAG);
						
							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							

							/*Parse the ELSE portion if there's one*/
							if (len >0)
							{
								rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,MEMBER_SEQLIST_TAG);
								if (rc != SUCCESS)
								return rc; /* Return if not successful*/

								tempPtrToCond->byNumberOfSections++;

							}

							if(pAttr->bIsAttributeConditionalList == true)
							{/*then we have to push this conditional value on conditionalVals*/

								pAttr->conditionalVals.push_back(tempPtrToCond);
							}

						}
						break; /*End IF_TAG*/
				
					case SELECT_TAG: /*We have a Switch Case conditional*/
						{
							
							if((pAttr->byNumOfChunks == 0) && length == 0)
							{/*We have a conditional in single chunk*/
							
								pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
								pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
															pAttr->attrDataType,
															0);
								tempPtrToCond = pAttr->pCond;
								pAttr->byNumOfChunks++;
							}
							else
							{/*We have a multichunk list which has conditionals*/
								tempPtrToCond = new DDlConditional(DDL_COND_TYPE_SELECT,
																	pAttr->attrDataType,
																	0);
								
								pAttr->bIsAttributeConditionalList = true;
								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
								pAttr->byNumOfChunks++;
							}
							/*Now Parse the ddpExpression Argument of the SELECT */

							rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

							if(rc != SUCCESS)
								return rc;
							
							/*otherwise Parse all the CASE branches and the DEFAULT */
							while (len > 0)
							{
								DDL_PARSE_TAG(chunkp,&len,&tagp,&len1);

								switch(tagp)
								{
									case CASE_TAG:
										{
											/*We are parsing the CASE constants as expression
											just bcoz of the spec. But it should be a constant 
											value , ie. an expression with just a  constant (integer)
											value*/

											rc = ddl_parse_expression(chunkp,&len,&tempExpr);

											if(rc != SUCCESS)
												return rc;
/*
											ddpExpression :: iterator it;

											it = tempExpr.begin();

											if(it->byElemType != INTCST_OPCODE)
												{
	#ifdef _PARSER_DEBUG
					fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_member_list()!!!");
													return DDL_UNHANDLED_STUFF_FAILURE;
	#endif												
												}
											temp = (long)it->elem.ulConst;
*/
											tempPtrToCond->caseVals.push_back(tempExpr);
											
											/*We have the case constant value 
											Now parse the attributre value from the 
											following chunk	*/

											rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
																			&len,MEMBER_SEQLIST_TAG);
											if (rc != SUCCESS)
											return rc; /* Return if not successful*/

											tempPtrToCond->byNumberOfSections++;
											
											tempExpr.clear();
										}
										break;/*End CASE_TAG*/

									case DEFAULT_TAG:
										{
/*
											temp = DEFAULT_TAG_VALUE;
											pAttr->pCond->caseVals.push_back(temp);
*/
											
											tempPtrToCond->byNumberOfSections++;

											rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,
																			&len,MEMBER_SEQLIST_TAG);
											if (rc != SUCCESS)
											return rc; /* Return if not successful*/
										}
										break;/*End DEFAULT_TAG*/

									default:
										return DDL_ENCODING_ERROR;

								}/*End Switch tagp*/


							}/*End while*/

							if(pAttr->bIsAttributeConditionalList == true)
							{/*then we have to push this conditional value on conditionalVals*/

								pAttr->conditionalVals.push_back(tempPtrToCond);
							}


						}
						break; /*End SELECT_TAG*/

					case OBJECT_TAG : /*We have a direct object*/
						{

							if((pAttr->byNumOfChunks == 0) && length == 0)
							{ /*We have a direct list in a single chunk*/
								pAttr->pVals = new VALUES;

								pAttr->pVals->memberList = new MEMBER_LIST;

								//ADDED By Deepak
								(pAttr->pVals->memberList)->clear();
								//END

								rc = ddl_parse_members(chunkp,&len,pAttr->pVals->memberList);

								if(rc != DDL_SUCCESS)

								return rc;

								pAttr->byNumOfChunks++;
								break;

							}
							else /*We are having a  possible combination of direct & conditional chunks*/
							{ /*Spl case of all chunks having direct Values , we'll handle after looping */

								tempVal.memberList = new MEMBER_LIST;

								rc = ddl_parse_members(chunkp,&len,tempVal.memberList);
								
								if(rc != DDL_SUCCESS)
									return rc;

								pAttr->directVals.push_back(tempVal);

								pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);//Vibhor 200105: Changed
		
										
								pAttr->byNumOfChunks++;
											
											
								/*Just set the conditional flag every time we come here irrespective of 
								 whether or not its set earlier, If we have chunks of non conditionals,
								we will reset this flag later*/
								pAttr->bIsAttributeConditional = true;
										
							}
							break;
							
						}
						/*End OBJECT_TAG*/
					default:
						return DDL_ENCODING_ERROR;
						break;
				}/*End switch tag*/

	}/*End while*/


	if(pAttr->pVals == NULL 
		&& pAttr->bIsAttributeConditionalList == false 
		&& (pAttr->conditionalVals.size() == 0)
		&& (pAttr->directVals.size() > 0)) /*The last one is a double check*/
	{
		/*We have a Direct list in more than one chunk!!!!
		 So we will copy the same to pAttr->Vals->enmList*/
		pAttr->pVals = new VALUES;
		pAttr->pVals->memberList = new MEMBER_LIST;
		for (unsigned i = 0; i < pAttr->directVals.size(); i++)
		{
			tempPtrToMemberList = pAttr->directVals.at(i).memberList;
			for (unsigned j =0; j < tempPtrToMemberList->size(); j++)
			{
				tmpMember = tempPtrToMemberList->at(j);
				pAttr->pVals->memberList->push_back(tmpMember);

			}/*Endfor j*/
					
			/*Just clear this list, its not required any more*/
			tempPtrToMemberList->clear();
		}/*Endfor i*/
				
		/*Now clear the directVals list too*/

		pAttr->directVals.clear();

		/*Reset the bIsAttributeConditional flag*/
		pAttr->bIsAttributeConditional = false;
	
	}/*Endif*/

	/*Vibhor 180105: Start of Code*/

	/*If due to some combination both Conditional & ConditionalList Flags are set, 
	 Reset the bIsAttributeConditional */
	if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == true)
		pAttr->bIsAttributeConditional = false;

	/*Vibhor 180105: End of Code*/

	return SUCCESS;

}


/* This method will actually call the parse_attr_string() to parse 
the display or edit format of a variable 

The last argument distinguishes whether we are parsing 
display format: DISPLAY_FORMAT_TAG or
edit format: EDIT_FORMAT_TAG */

int parse_attr_disp_edit_format(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size, unsigned short tagExpected)
{

	int rc;
	unsigned long tag, len;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(tag != tagExpected)
		return DDL_ENCODING_ERROR;

	/*Now its just the string*/

	return parse_attr_string(pAttr,binChunk,size);

}/* End parse_attr_disp_edit_format */


/*This  method parses the scaling factor of a variable , which is defined as a expression*/

int parse_attr_scaling_factor(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	
	int rc;
	unsigned long tag, tagp, len,len1;

	unsigned  char** chunkp = NULL;

	unsigned long *length = NULL;

//	long temp;


	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	/*Parse the tag it should be either SCALING_FACTOR_TAG */


	DDL_PARSE_TAG(chunkp,length,&tag,&len);
	
	if(SCALING_FACTOR_TAG != tag)
		return DDL_ENCODING_ERROR;
	
	/*First parse the tag to ascertain whether its a IF / SELECT or a DIRECT tag*/

	DDL_PARSE_TAG(chunkp,length,&tag,&len);

	/*We won't loop here since it's not a list*/	
	switch(tag)
		{
			case IF_TAG:	/*We have an IF THEN ELSE conditional*/
				{
					pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
					pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);	
						
					/*Now Parse the ddpExpression associated with the IF block */
					rc = ddl_parse_expression(chunkp,&len,&(pAttr->pCond->expr));

					if (rc != SUCCESS)
						return rc; /* Return if not successful*/

					/*otherwise Parse the value of the attribute associated with THEN clause*/
						
					rc = ddl_parse_conditional(pAttr->pCond,chunkp,&len);
					
					if (rc != SUCCESS)
						return rc; /* Return if not successful*/

					/*Parse the ELSE portion if there's one*/
					if (*length >0)
					{
						rc = ddl_parse_conditional(pAttr->pCond,chunkp,&len);
						if (rc != SUCCESS)
						return rc; /* Return if not successful*/
						pAttr->pCond->byNumberOfSections++;
					}

				}
				break; /*End IF_TAG*/
			case SELECT_TAG:
				{
					pAttr->bIsAttributeConditional = true;
					pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
														pAttr->attrDataType,
														0);

					/*Now Parse the ddpExpression Argument of the SELECT */

					rc = ddl_parse_expression(chunkp,&len,&(pAttr->pCond->expr));

					if(rc != SUCCESS)
						return rc;
							
					/*otherwise Parse all the CASE branches and the DEFAULT */
					while (len > 0)
					{
						DDL_PARSE_TAG(chunkp,&len,&tagp,&len1);
						switch(tagp)
						{
							case CASE_TAG:
								{
									/*We are parsing the CASE constants as expression
									just bcoz of the spec. But it should be a constant 
									value , ie. an expression with just a  constant (integer)
									value*/
									rc = ddl_parse_expression(chunkp,&len,&tempExpr);
									if(rc != SUCCESS)
										return rc;
/*
									ddpExpression :: iterator it;
									it = tempExpr.begin();

									if(it->byElemType != INTCST_OPCODE)
									{
#ifdef _PARSER_DEBUG
				fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_min_max_list()!!!");
											return DDL_UNHANDLED_STUFF_FAILURE;
#endif											
									}
									temp = (long)it->elem.ulConst;
*/
									pAttr->pCond->caseVals.push_back(tempExpr);
											
									/*We have the case constant value 
									Now parse the attributre value from the 
									following chunk	*/

									rc = ddl_parse_conditional(pAttr->pCond,chunkp,&len);
									if (rc != SUCCESS)
										return rc; /* Return if not successful*/

									pAttr->pCond->byNumberOfSections++;
											
									tempExpr.clear();
								}
								break;/*End CASE_TAG*/

							case DEFAULT_TAG:
								{
/*
									temp = DEFAULT_TAG_VALUE;
									pAttr->pCond->caseVals.push_back(temp);
*/								
									pAttr->pCond->byNumberOfSections++;

									rc = ddl_parse_conditional(pAttr->pCond,chunkp,&len);
									if (rc != SUCCESS)
										return rc; /* Return if not successful*/
								}
								break;/*End DEFAULT_TAG*/
							default:
								return DDL_ENCODING_ERROR;
						}/*End Switch tagp*/


					}/*End while*/

				}/*End SELECT_TAG*/
			break;
			case OBJECT_TAG:
				{
					pAttr->pVals = new VALUES;
					
					pAttr->pVals->pExpr = new ddpExpression;

					ddl_parse_expression(&binChunk,&size,pAttr->pVals->pExpr);

					if(rc != DDL_SUCCESS)
						return rc;

				}
				break;
			default:
				return DDL_ENCODING_ERROR;
				break;
		}/*End switch*/
	
	return SUCCESS;

}/*End parse_attr_scaling_factor*/



bool set_min_max_index(DDlConditional* pCond, unsigned long ulIndex)
{
	int iSectionIndex;

	int iChildIndex = 0;;
	int iValueIndex = 0;

	bool bRetVal;

	if(NULL == pCond)
		return false;
	
	for(iSectionIndex = 0; iSectionIndex < pCond->byNumberOfSections ; iSectionIndex++)
	{
		if(pCond->isSectionConditionalList.at(iSectionIndex) == DDL_SECT_TYPE_DIRECT)//Vibhor 200105: Changed
		{
			pCond->Vals.at(iValueIndex).minMaxList->at(0).which = ulIndex;
		}
		else
		{
			bRetVal = set_min_max_index(pCond->listOfChilds.at(iChildIndex),ulIndex);
			if(bRetVal != true)
				return false;
			iChildIndex++;
		}
		iValueIndex++;
	}
	
	return true;
	

}/*End set_min_max_index*/



/*This method is called to parse the min & max values ie the ranges of a variable if specified */

/*
The last argument distinguishes whether we are parsing 

  min value :MIN_VALUE_TAG
  max value :MAX_VALUE_TAG
 */




/* stevev 24aug06 - the Attribute is actually a list of MIN_MAX 
 *	 but the MIN_MAX is a which AND a Conditional that resolves to an expression.
 *	 ie the List is Non-Conditional but the Value IS conditional.
 */
int parse_attr_min_max_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size, unsigned short tagExpected)
{
	
	int rc;
	unsigned long tag, /*unused tagp,*/ len;/*unused ,len1; */

	unsigned  char** chunkp = NULL;

	unsigned long length;
	unsigned __int64 LL;

	DDlConditional *tempPtrToCond = NULL;

	MIN_MAX_LIST *tempPtrToMinMaxList = NULL;

	MIN_MAX_VALUE tmpMinMax;

// unused	VALUES		tempVal;

// unused	bool bRetVal;

	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	
	length = size;

	

	/*Parse the tag it should be either MIN_VALUE_TAG or MAX_VALUE_TAG*/

	DDL_PARSE_TAG(chunkp,&length,&tag,&len);

	if(tag != tagExpected)
		return DDL_ENCODING_ERROR;

	
	while(length > 0)
	{										// assume pointer was transfered in the push_back
		(void)memset((char*)&tmpMinMax,0,sizeof(MIN_MAX_VALUE));
	
		/*Parse the index of the min / max value*/
		DDL_PARSE_INTEGER(chunkp,&length,&LL); tmpMinMax.which = (UINT32)LL;

		/*parse the Min - max value expression*/

		tmpMinMax.pCond = new DDlConditional(DDL_COND_TYPE_UNDEFINED,
											 DDL_ATTR_DATA_TYPE_EXPRESSION,
											 1);// number of sections
		rc = ddl_parse_conditional(tmpMinMax.pCond,chunkp,&length);
#if 0 
////////////////////////////////////
		/*First parse the tag to ascertain whether its a IF / SELECT or a DIRECT tag*/
		
		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;
		
		switch(tag)
		{
			case IF_TAG:	/*We have an IF THEN ELSE conditional*/
				{
					if((pAttr->byNumOfChunks == 0) && length == 0)
					{/*We have a conditional in single chunk & 
						incidentally its just one min or max value ie. which = 0 
						which is anyway set by default*/
							
						pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
													pAttr->attrDataType,
													1);
						tempPtrToCond = pAttr->pCond;
						pAttr->byNumOfChunks++;

					}
					else
					{/*We have a multichunk list which has conditionals*/
						tempPtrToCond = new DDlConditional(DDL_COND_TYPE_IF,
													pAttr->attrDataType,
													1);
								
						pAttr->bIsAttributeConditionalList = true;
						pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
						pAttr->byNumOfChunks++;

						

					}
						
					/*Now Parse the Expression associated with the IF block */
					rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

					if (rc != SUCCESS)
						return rc; /* Return if not successful*/

					/*otherwise Parse the value of the attribute associated with THEN clause*/
						
					rc = ddl_parse_conditional(tempPtrToCond,chunkp,&len);
					
					if (rc != SUCCESS)
						return rc; /* Return if not successful*/

					/*Parse the ELSE portion if there's one*/
					if (len >0)
					{
						rc = ddl_parse_conditional(tempPtrToCond,chunkp,&len);
						if (rc != SUCCESS)
							return rc; /* Return if not successful*/
						tempPtrToCond->byNumberOfSections++;
					}
					/*Since this conditional is for one value of MIN or MAX ,
					we just need to propagate the index to all the branches of
					the conditional */

					
					bRetVal = set_min_max_index(tempPtrToCond,tmpMinMax.which);
					if(bRetVal != true)
						return false;
					
					if(pAttr->bIsAttributeConditionalList == true)
					{/*then we have to push this conditional value on conditionalVals*/

						pAttr->conditionalVals.push_back(tempPtrToCond);

					}

				}
				break; /*End IF_TAG*/
			case SELECT_TAG:
				{
					if((pAttr->byNumOfChunks == 0) && length == 0)
					{/*We have a conditional in single chunk*/
					
						pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
													pAttr->attrDataType,
													0);
						tempPtrToCond = pAttr->pCond;
					pAttr->byNumOfChunks++;

					}
					else
					{/*We have a multichunk list which has conditionals*/
						tempPtrToCond = new DDlConditional(DDL_COND_TYPE_SELECT,
															pAttr->attrDataType,
															0);
						
						pAttr->bIsAttributeConditionalList = true;
						pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);//Vibhor 200105: Changed
						pAttr->byNumOfChunks++;

				
					}

					/*Now Parse the Expression Argument of the SELECT */

					rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));

					if(rc != SUCCESS)
						return rc;
							
					/*otherwise Parse all the CASE branches and the DEFAULT */
					while (len > 0)
					{
						DDL_PARSE_TAG(chunkp,&len,&tagp,&len1);
						switch(tagp)
						{
							case CASE_TAG:
								{
									/*We are parsing the CASE constants as expression
									just bcoz of the spec. But it should be a constant 
									value , ie. an expression with just a  constant (integer)
									value*/
									rc = ddl_parse_expression(chunkp,&len,&tempExpr);
									if(rc != SUCCESS)
										return rc;
/*
									Expression :: iterator it;
									it = tempExpr.begin();

									if(it->byElemType != INTCST_OPCODE)
									{
#ifdef _PARSER_DEBUG
				fprintf(ferr,"\n Expression encountered in case tag in parse_attr_min_max_list()!!!");
											return DDL_UNHANDLED_STUFF_FAILURE;
#endif												
									}
									temp = (long)it->elem.ulConst;
*/
									tempPtrToCond->caseVals.push_back(tempExpr);
											
									/*We have the case constant value 
									Now parse the attributre value from the 
									following chunk	*/

									rc = ddl_parse_conditional(tempPtrToCond,chunkp,&len);
									if (rc != SUCCESS)
										return rc; /* Return if not successful*/

									tempPtrToCond->byNumberOfSections++;
											
									tempExpr.clear();
								}
								break;/*End CASE_TAG*/

							case DEFAULT_TAG:
								{
/*									
									temp = DEFAULT_TAG_VALUE;
									pAttr->pCond->caseVals.push_back(temp);
*/									
								
									tempPtrToCond->byNumberOfSections++;

									rc = ddl_parse_conditional(tempPtrToCond,chunkp,&len);
									if (rc != SUCCESS)
										return rc; /* Return if not successful*/
								}
								break;/*End DEFAULT_TAG*/
							default:
								return DDL_ENCODING_ERROR;
						}/*End Switch tagp*/

					}/*End while*/
					
					/*Since this conditional is for one value of MIN or MAX ,
					we just need to propagate the index to all the branches of
					the conditional */
					
					bRetVal = set_min_max_index(tempPtrToCond,tmpMinMax.which);
						if(bRetVal != true)
							return false;
					
					if(pAttr->bIsAttributeConditionalList == true)
					{/*then we have to push this conditional value on conditionalVals*/

						pAttr->conditionalVals.push_back(tempPtrToCond);	

					}

				}/*End SELECT_TAG*/
			break;
			case OBJECT_TAG:
				{
					if((pAttr->byNumOfChunks == 0) && length == 0)
					{ /*We have a direct list in a single chunk*/
						pAttr->pVals = new VALUES;

						pAttr->pVals->minMaxList = new MIN_MAX_LIST;

						rc = ddl_parse_expression(chunkp,&len,&tmpMinMax.value);

						if(rc != DDL_SUCCESS)
							return rc;

						pAttr->pVals->minMaxList->push_back(tmpMinMax);	
						pAttr->byNumOfChunks++;
						break;
					}
					else /*We are having a  possible combination of direct & conditional chunks*/
					{ /*Spl case of all chunks having direct Values , we'll handle after looping */

						tempVal.minMaxList = new MIN_MAX_LIST;
						
						rc = ddl_parse_expression(chunkp,&len,&tmpMinMax.value);
					
						if(rc != DDL_SUCCESS)
							return rc;

						tempVal.minMaxList->push_back(tmpMinMax);	

						pAttr->directVals.push_back(tempVal);

						pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);//Vibhor 200105: Changed
		
						pAttr->byNumOfChunks++;
											
								
						/*Just set the conditional flag every time we come here irrespective of 
						whether or not its set earlier, If we have chunks of non conditionals,
						we will reset this flag later*/
						pAttr->bIsAttributeConditional = true;
					
					}
						break;										
				
					}				
				
			default:
				return DDL_ENCODING_ERROR;
				break;
		}/*End switch*/
#endif ///////////////////////////////////////////////////////////////////
		// the list itself is not conditional
		if (pAttr->pVals == NULL )
		{
			pAttr->pVals = new VALUES;
			pAttr->pVals->minMaxList = NULL;
		}
		if (pAttr->pVals->minMaxList == NULL)
		{
			pAttr->pVals->minMaxList = new MIN_MAX_LIST;
		}
		pAttr->pVals->minMaxList->push_back(tmpMinMax);

	}/*End while*/

	if(pAttr->pVals == NULL 
		&& pAttr->bIsAttributeConditionalList == false 
		&& (pAttr->conditionalVals.size() == 0)
		&& (pAttr->directVals.size() > 0)) /*The last one is a double check*/
	{
		/*We have a Direct list in more than one chunk!!!!
		 So we will copy the same to pAttr->Vals->enmList*/
		pAttr->pVals = new VALUES;
		pAttr->pVals->minMaxList = new MIN_MAX_LIST;
		for (unsigned i = 0; i < pAttr->directVals.size(); i++)
		{
			tempPtrToMinMaxList = pAttr->directVals.at(i).minMaxList;
			for (unsigned j =0; j < tempPtrToMinMaxList->size(); j++)
			{
				tmpMinMax = tempPtrToMinMaxList->at(j);
				pAttr->pVals->minMaxList->push_back(tmpMinMax);

			}/*Endfor j*/
					
			/*Just clear this list, its not required any more*/
			tempPtrToMinMaxList->clear();
		}/*Endfor i*/
				
		/*Now clear the directVals list too*/

		pAttr->directVals.clear();

		/*Reset the bIsAttributeConditional flag*/
		pAttr->bIsAttributeConditional = false;
	
	}/*Endif*/

	/*Vibhor 180105: Start of Code*/

	/*If due to some combination both Conditional & ConditionalList Flags are set, 
	 Reset the bIsAttributeConditional */
	if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == true)
		pAttr->bIsAttributeConditional = false;

	/*Vibhor 180105: End of Code*/

	return SUCCESS;

}/* End parse_attr_min_max_list*/




int parse_attr_expr(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long * length = NULL;
	unsigned long tag,tagp,len;
//	long temp;

	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	/*Parse the Tag to know if we have a conditional or a direct object*/

	DDL_PARSE_TAG(chunkp, length, &tag, &len);
	
	switch (tag)
			{

				case IF_TAG: /*We have an IF THEN ELSE conditional*/
					{
						pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);	
						
						/*Now Parse the Expression associated with the IF block */
						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						/*otherwise Parse the value of the attribute associated with THEN clause*/
						
						rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
					
						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						

						/*Parse the ELSE portion if there's one*/
						if (*length >0)
						{
							rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/

							pAttr->pCond->byNumberOfSections++;

						}

					}
					break; /*End IF_TAG*/
			
				case SELECT_TAG: /*We have a Switch Case conditional*/
					{
						pAttr->bIsAttributeConditional = true;
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
														pAttr->attrDataType,
														0);

						/*Now Parse the Expression Argument of the SELECT */

						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if(rc != SUCCESS)
							return rc;
						
						/*otherwise Parse all the CASE branches and the DEFAULT */
						while (*length > 0)
						{
							DDL_PARSE_TAG(chunkp,length,&tagp,&len);

							switch(tagp)
							{
								case CASE_TAG:
									{
										/*We are parsing the CASE constants as expression
										just bcoz of the spec. But it should be a constant 
										value , ie. an expression with just a  constant (integer)
										value*/

										rc = ddl_parse_expression(chunkp,length,&tempExpr);

										if(rc != SUCCESS)
											return rc;
/*
										Expression :: iterator it;

										it = tempExpr.begin();

										if(it->byElemType != INTCST_OPCODE)
											{
#ifdef _PARSER_DEBUG
				fprintf(ferr,"\n ddpExpression encountered in case tag parse_attr_expr()!!!");
											return DDL_UNHANDLED_STUFF_FAILURE;
#endif												
											}
										temp = (long)it->elem.ulConst;
*/
										pAttr->pCond->caseVals.push_back(tempExpr);
										
										/*We have the case constant value 
										Now parse the attributre value from the 
										following chunk	*/

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/

										pAttr->pCond->byNumberOfSections++;
										
										tempExpr.clear();
									}
									break;/*End CASE_TAG*/

								case DEFAULT_TAG:
									{
/*
										temp = DEFAULT_TAG_VALUE;
										pAttr->pCond->caseVals.push_back(temp);
*/										
										pAttr->pCond->byNumberOfSections++;

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/
									}
									break;/*End DEFAULT_TAG*/

								default:
									return DDL_ENCODING_ERROR;

							}/*End Switch tagp*/


						}/*End while*/


					}
					break; /*End SELECT_TAG*/

				case OBJECT_TAG : /*We have a direct object*/
					{

						pAttr->pVals = new VALUES;

						pAttr->pVals->pExpr = new ddpExpression;
					
						rc = ddl_parse_expression(chunkp,length,pAttr->pVals->pExpr);
						
						if(rc != DDL_SUCCESS)
							return rc;
				
					}
					break; /*End OBJECT_TAG*/
				default:
					return DDL_ENCODING_ERROR;
					break;
			}/*End switch tag*/
	return SUCCESS;


}/*End parse_attr_expr*/


int parse_attr_reference(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long * length = NULL;
	unsigned long tag,tagp,len;
//	long temp;

	ddpExpression tempExpr;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	/*Parse the Tag to know if we have a conditional or a direct object*/

	DDL_PARSE_TAG(chunkp, length, &tag, &len);
	
	switch (tag)
			{

				case IF_TAG: /*We have an IF THEN ELSE conditional*/
					{
						pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);	
						
						/*Now Parse the ddpExpression associated with the IF block */
						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						/*otherwise Parse the value of the attribute associated with THEN clause*/
						
						rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
					
						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						

						/*Parse the ELSE portion if there's one*/
						if (*length >0)
						{
							rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/

							pAttr->pCond->byNumberOfSections++;

						}

					}
					break; /*End IF_TAG*/
			
				case SELECT_TAG: /*We have a Switch Case conditional*/
					{
						pAttr->bIsAttributeConditional = true;
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
														pAttr->attrDataType,
														0);

						/*Now Parse the ddpExpression Argument of the SELECT */

						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if(rc != SUCCESS)
							return rc;
						
						/*otherwise Parse all the CASE branches and the DEFAULT */
						while (*length > 0)
						{
							DDL_PARSE_TAG(chunkp,length,&tagp,&len);

							switch(tagp)
							{
								case CASE_TAG:
									{
										/*We are parsing the CASE constants as expression
										just bcoz of the spec. But it should be a constant 
										value , ie. an expression with just a  constant (integer)
										value*/

										rc = ddl_parse_expression(chunkp,length,&tempExpr);

										if(rc != SUCCESS)
											return rc;
/*
										ddpExpression :: iterator it;

										it = tempExpr.begin();

										if(it->byElemType != INTCST_OPCODE)
											{
#ifdef _PARSER_DEBUG
				fprintf(ferr,"\n ddpExpression encountered in case tag in parse_attr_reference()!!!");
											return DDL_UNHANDLED_STUFF_FAILURE;
#endif												
											}
										temp = (long)it->elem.ulConst;
*/
										pAttr->pCond->caseVals.push_back(tempExpr);
										
										/*We have the case constant value 
										Now parse the attributre value from the 
										following chunk	*/

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/

										pAttr->pCond->byNumberOfSections++;
										
										tempExpr.clear();
									}
									break;/*End CASE_TAG*/

								case DEFAULT_TAG:
									{
/*										
										temp = DEFAULT_TAG_VALUE;
										pAttr->pCond->caseVals.push_back(temp);
*/										
										pAttr->pCond->byNumberOfSections++;

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/
									}
									break;/*End DEFAULT_TAG*/

								default:
									return DDL_ENCODING_ERROR;

							}/*End Switch tagp*/


						}/*End while*/


					}
					break; /*End SELECT_TAG*/

				case OBJECT_TAG : /*We have a direct object*/
					{

						pAttr->pVals = new VALUES;

						pAttr->pVals->ref = new ddpREFERENCE;
					
						rc = ddl_parse_ref(chunkp,length,pAttr->pVals->ref);
						
						if(rc != DDL_SUCCESS)
							return rc;
				
					}
					break; /*End OBJECT_TAG*/
				default:
					return DDL_ENCODING_ERROR;
					break;
			}/*End switch tag*/
	return SUCCESS;

}/*End parse_attr_reference*/


/*This method calls ddl_parse_reference to parse the item array name to which a variable indexing*/

int parse_attr_array_name(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc ; /*return code*/
	unsigned long tag, len ; /*tag and length of the binary, we'll just neglect the len*/

	/*
	 *  The first byte of a chunk will be a ITEM_ARRAYNAME_TAG
	 *  Strip off the ITEM_ARRAYNAME_TAG.
	 */

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(ITEM_ARRAYNAME_TAG != tag)
		return DDL_ENCODING_ERROR;

	pAttr->pVals = new VALUES;

	pAttr->pVals->ref = new ddpREFERENCE;

	/*Call ddl_parse_ref to parse the array name reference*/

	return ddl_parse_ref(&binChunk,&size,pAttr->pVals->ref);

}/*End parse_attr_reference*/


/*The Method->Scope attribute is encoded as a single byte integer , 
 which is to be interpreted as a bitstring , so we will just parse the intereger */

int parse_attr_meth_scope(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = 0 ;
	pAttr->pVals = new VALUES;

	/*Simply parse the integer & return */
	DDL_PARSE_INTEGER(&binChunk,&size,&pAttr->pVals->ullVal);

	return rc;

}/*End parse_attr_meth_scope*/

/*Vibhor 270804: Start of Code*/
int parse_attr_line_type(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc;
	unsigned char **chunkp = NULL;
	unsigned long * length = NULL;
	unsigned long tag,tagp,len;

	ddpExpression tempExpr;

	LINE_TYPE tmpLineType;

	assert(binChunk && size);

	chunkp = &binChunk;
	length = &size;

	/*Parse the Tag to know if we have a conditional or a direct object*/

	DDL_PARSE_TAG(chunkp, length, &tag, &len);
	
	switch (tag)
			{

				case IF_TAG: /*We have an IF THEN ELSE conditional*/
					{
						pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_IF,
														pAttr->attrDataType,
														1);	
						
						/*Now Parse the ddpExpression associated with the IF block */
						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						/*otherwise Parse the value of the attribute associated with THEN clause*/
						
						rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
					
						if (rc != SUCCESS)
							return rc; /* Return if not successful*/

						/*Parse the ELSE portion if there's one*/
						if (*length >0)
						{
							rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
							if (rc != SUCCESS)
							return rc; /* Return if not successful*/

							pAttr->pCond->byNumberOfSections++;

						}

					}
					break; /*End IF_TAG*/
			
				case SELECT_TAG: /*We have a Switch Case conditional*/
					{
						pAttr->bIsAttributeConditional = true;
						pAttr->pCond = new DDlConditional(DDL_COND_TYPE_SELECT,
														pAttr->attrDataType,
														0);

						/*Now Parse the ddpExpression Argument of the SELECT */

						rc = ddl_parse_expression(chunkp,length,&(pAttr->pCond->expr));

						if(rc != SUCCESS)
							return rc;
						
						/*otherwise Parse all the CASE branches and the DEFAULT */
						while (*length > 0)
						{
							DDL_PARSE_TAG(chunkp,length,&tagp,&len);

							switch(tagp)
							{
								case CASE_TAG:
									{
										/*We are parsing the CASE constants as expression
										just bcoz of the spec. But it should be a constant 
										value , ie. an expression with just a  constant (integer)
										value*/

										rc = ddl_parse_expression(chunkp,length,&tempExpr);

										if(rc != SUCCESS)
											return rc;
 
 										pAttr->pCond->caseVals.push_back(tempExpr);
										
										/*We have the case constant value 
										Now parse the attributre value from the 
										following chunk	*/

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/

										pAttr->pCond->byNumberOfSections++;
										
										tempExpr.clear();
									}
									break;/*End CASE_TAG*/

								case DEFAULT_TAG:
									{
 								
										pAttr->pCond->byNumberOfSections++;

										rc = ddl_parse_conditional(pAttr->pCond,chunkp,length);
										if (rc != SUCCESS)
										return rc; /* Return if not successful*/
									}
									break;/*End DEFAULT_TAG*/

								default:
									return DDL_ENCODING_ERROR;

							}/*End Switch tagp*/


						}/*End while*/


					}
					break; /*End SELECT_TAG*/

				case OBJECT_TAG : /*We have a direct object*/
					{

						rc = ddl_parse_linetype(chunkp,length,tmpLineType);
						
						if(rc != DDL_SUCCESS)
							return rc;

						pAttr->pVals = new VALUES;

						pAttr->pVals->lineType = tmpLineType;
				
					}
					break; /*End OBJECT_TAG*/
				default:
					return DDL_ENCODING_ERROR;
					break;
			}/*End switch tag*/
	return SUCCESS;

}/*End parse_attr_line_type*/

/*Vibhor 270804: End of Code*/

/*Vibhor 280904: Start of Code*/

int parse_attr_wavefrm_type(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = SUCCESS;
	unsigned long tag, len;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(tag != WAVE_TYPE_TAG)
		return DDL_ENCODING_ERROR;

	/*Now its just an integer */
	pAttr->pVals = new VALUES;

	/*Simply parse the integer & return */
	DDL_PARSE_INTEGER(&binChunk,&size,&pAttr->pVals->ullVal);

	return rc;

}/*End parse_attr_wavefrm_type*/

int parse_attr_chart_type(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = SUCCESS;
	unsigned long tag, len;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(tag != CHART_TYPE_TAG)
		return DDL_ENCODING_ERROR;

	/*Now its just an integer */
	pAttr->pVals = new VALUES;

	/*Simply parse the integer & return */
	DDL_PARSE_INTEGER(&binChunk,&size,&pAttr->pVals->ullVal);

	return rc;


}/*End parse_attr_chart_type*/

int parse_attr_menu_style(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = SUCCESS;
	unsigned long tag, len;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(tag != MENU_STYLE_TAG)
		return DDL_ENCODING_ERROR;

	/*Now its just an integer */
	pAttr->pVals = new VALUES;

	/*Simply parse the integer & return */
	DDL_PARSE_INTEGER(&binChunk,&size,&pAttr->pVals->ullVal);

//rc = parse_attr_int(pAttr,binChunk,size);

	return rc;

}/*End parse_attr_menu_style*/


/* stevev 06may05 */
int parse_ascii_string(string& retStr, unsigned char** binChunk, unsigned long& size)
{
	int rc = SUCCESS;
	unsigned long tag, totLen = 0, len;
	unsigned __int64 LL;

	DDL_PARSE_TAG(binChunk,&size,&tag,&totLen);// implicit, no len

	if(tag != NAME_STR_TAG)
		return DDL_ENCODING_ERROR;


	DDL_PARSE_INTEGER(binChunk,&size,&LL); len = (UINT32)LL;
	if(len == 0 || len > 0x7ff)// aribitrary maximum len
		return DDL_LARGE_VALUE;

	char *rawStr = new char[len+1];// +1 just in case this is start of memory leak "Ametek_model_cod" PAW 08/04/09
	memcpy(rawStr,*binChunk,len);
	size      -= len;
	*binChunk += len;
	if(strlen(rawStr) > len)
		return DDL_SHORT_BUFFER;

	retStr = rawStr;
	delete[] rawStr;
	return rc;
}

int parse_member_debug_info (MEMBER_DEBUG_T& mem, unsigned char* binChunk, unsigned long& size)
{
	int rc = SUCCESS;
	unsigned long tag, len = 0;
	string   wrkStr;
	unsigned __int64 LL;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);// explict, len will be returned
	if(tag != DEBUG_ATTR_MEMBER_TAG)
		return DDL_ENCODING_ERROR;

	//symbol name
	if ( len > 2 ) 
	{	
		size -= len; // assume all will be successful
		rc = parse_ascii_string(mem.symbol_name, &binChunk,len);
	}
	else
	{
		size = 0;
		rc = DDL_INSUFFICIENT_SIZE;
	}
	// flags
	if ( rc == SUCCESS ) 
			rc = ddl_parse_bitstring(&binChunk,&len,&(mem.flags));
	//value
	if ( rc == SUCCESS ) 
			DDL_PARSE_INTEGER(&binChunk,&size,&LL); mem.member_value = (UINT32)LL;
	return rc;
}


int parse_attr_debug_info (ATTR_DEBUG_INFO_T* pADI, unsigned char& binChunk, unsigned long& size)
{
	int rc = SUCCESS;
	unsigned long tag, len;
	unsigned __int64 LL;
	string   wrkStr;
	MEMBER_DEBUG_T wrkMember;
	unsigned char* bc = & binChunk;

	DDL_PARSE_TAG(&bc,&size,&tag,&len);// explicit - has a length 

	if(tag != DEBUG_ATTR_INFO_TAG)
		return DDL_ENCODING_ERROR;

	// attr-tag-number
	if ( len > 2 ) 
	{	
		size -= len; // assume all will be successful
		DDL_PARSE_INTEGER(&bc,&len,&LL); pADI->attr_tag = (UINT32)LL;
	}
	else
	{
		size = 0;
		rc = DDL_INSUFFICIENT_SIZE;
	}

	// lineno
	if ( rc == SUCCESS && len > 0 ) 
			DDL_PARSE_INTEGER(&bc,&len,&LL); pADI->attr_lineNo = (UINT32)LL;
	// filename
	if ( rc == SUCCESS && len > 2 ) 
	{				
		rc = ddl_parse_string(&bc,&len,&(pADI->attr_filename));
	}
	// members
	while (len > 2 && rc == SUCCESS)
	{
		rc = parse_member_debug_info(wrkMember,&binChunk,len);
		if ( rc == SUCCESS )
		{
			pADI->attr_member_list.push_back(wrkMember);
		}
		wrkMember.symbol_name.erase();
		wrkMember.flags = wrkMember.member_value = 0L;
	}

	return rc;
}

int parse_debug_info (DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{/* this the item level debug info */
	int rc = SUCCESS;
	unsigned long tag, len;
	unsigned __int64 LL=0;	// PAW prevents uninitialised variable error 03/03/09
	string   wrkStr;
	ATTR_DEBUG_INFO_T wrkAttr;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);// explicit

	if(tag != DEBUG_INFO_TAG)
		return DDL_ENCODING_ERROR;

	pAttr->pVals = new VALUES;
	pAttr->pVals->debugInfo = new  ITEM_DEBUG_INFO;

	/* symbol name */
	if ( len > 2 ) 
	{	
		size -= len; // assume all will be successful
		rc =	parse_ascii_string(pAttr->pVals->debugInfo->symbol_name, &binChunk, len);// PAW memory leak started here 09/04/09
	}
	else
	{
		size = 0;
		rc = DDL_INSUFFICIENT_SIZE;
	}


	// file name
	if ( rc == SUCCESS && len > 2 ) 
	{				
		rc = ddl_parse_string(&binChunk,&len,&(pAttr->pVals->debugInfo->file_name));
	}
	// line number

	if ( rc == SUCCESS && len > 0 ) 
			DDL_PARSE_INTEGER(&binChunk,&len,&LL); 	pAttr->pVals->debugInfo->lineNo = (UINT32)LL;

	// flags
	if ( rc == SUCCESS && len > 0 ) 
			rc = ddl_parse_bitstring(&binChunk,&len,&(pAttr->pVals->debugInfo->flags));

	// if more len
	ATTR_DEBUG_INFO_T* pADI;
	while (len > 2 && rc == SUCCESS)
	{
	// attributes
		pADI = new ATTR_DEBUG_INFO_T;

		rc = parse_attr_debug_info (pADI, *binChunk, len);
		if ( rc == SUCCESS )
		{
			pAttr->pVals->debugInfo->attr_list.push_back(pADI);
		}
	}
	return rc;

}/*End parse_attr_debug_info*/
/* end  stevev 06may05 */

int parse_attr_scope_size(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = SUCCESS;
	unsigned long tag, len;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(tag != SCOPE_SIZE_TAG)
		return DDL_ENCODING_ERROR;

	/*Now its just an integer */
	pAttr->pVals = new VALUES;

	/*Simply parse the integer & return */
	DDL_PARSE_INTEGER(&binChunk,&size,&pAttr->pVals->ullVal);

	return rc;

}/*End parse_attr_scope_size*/



int parse_attr_orient_size(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = SUCCESS;
	unsigned long tag, len;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(tag != GRID_ORIENT_TAG)
		return DDL_ENCODING_ERROR;

	/*Now its just an integer */
	pAttr->pVals = new VALUES;

	/*Simply parse the integer & return */
	DDL_PARSE_INTEGER(&binChunk,&size,&pAttr->pVals->ullVal);

	return rc;

}/*End parse_attr_orient_size*/



int parse_gridmembers_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = SUCCESS;
	unsigned char **chunkp = NULL;
	unsigned long *lengthp = NULL;

	unsigned long tag,  len,  length;
	unsigned long tag1, len1;
	
	DDlConditional *tempPtrToCond  = NULL;
	GRID_SET_LIST *tp_2gridSetList = NULL;

	GRID_SET   tmpGridSet;
	ddpExpression tempExpr;
	VALUES	   tempVal;


	assert(binChunk && size);
	
	chunkp  = &binChunk;
	lengthp = &size;

	/*The first tag should be a GRID_SEQLIST_TAG if not then return error*/
	DDL_PARSE_TAG(chunkp, lengthp, &tag, &length);

	if(tag != GRID_SEQLIST_TAG)
		return DDL_ENCODING_ERROR;

	*lengthp -= length;// get finished length (for return value?)

	/*Parse the Tag to know if we have a conditional or a direct object*/

	while(length >0)
	{
		DDL_PARSE_TAG(chunkp,&length,&tag,&len);

		length -= len;

		switch (tag)
		{
		case IF_TAG: /*We have an IF THEN ELSE conditional*/
			{
				tempPtrToCond  = new DDlConditional(DDL_COND_TYPE_IF, pAttr->attrDataType, 1);

				if((pAttr->byNumOfChunks == 0) && length == 0)
				{/*We have a conditional in single chunk*/
					pAttr->pCond   = tempPtrToCond;
					pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
				}
				else
				{/*We have a multichunk list which has conditionals*/				
					pAttr->bIsAttributeConditionalList = true;
					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);
				}	
				pAttr->byNumOfChunks++;

				/*Now Parse the ddpExpression associated with the IF block */
				rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));
				if (rc != SUCCESS)
					return rc; /* Return if not successful*/

				/*otherwise Parse the value of the attribute associated with THEN clause*/				
				rc = ddl_parse_conditional_list(tempPtrToCond, chunkp, &len, GRID_SEQLIST_TAG);			
				if (rc != SUCCESS)
					return rc; /* Return if not successful*/
				

				/*Parse the ELSE portion if there's one*/
				if (len >0)
				{
					rc = ddl_parse_conditional_list(tempPtrToCond,chunkp,&len,GRID_SEQLIST_TAG);
					if (rc != SUCCESS)
						return rc; /* Return if not successful*/

					tempPtrToCond->byNumberOfSections++;
				}

				if(pAttr->bIsAttributeConditionalList == true)
				{/*then we have to push this conditional value on conditionalVals*/
					pAttr->conditionalVals.push_back(tempPtrToCond);
				}
			}
			break; /*End IF_TAG*/
				
		case SELECT_TAG: /*We have a Switch Case conditional*/
			{				
				tempPtrToCond = new DDlConditional(DDL_COND_TYPE_SELECT, pAttr->attrDataType, 0);

				if((pAttr->byNumOfChunks == 0) && length == 0)
				{/*We have a conditional in single chunk*/
					pAttr->pCond = tempPtrToCond;				
					pAttr->bIsAttributeConditional = true; /*This guy is a conditional*/
				}
				else
				{/*We have a multichunk list which has conditionals*/					
					pAttr->bIsAttributeConditionalList = true;
					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_CONDNL);
				}
				pAttr->byNumOfChunks++;

				/*Now Parse the ddpExpression Argument of the SELECT */
				rc = ddl_parse_expression(chunkp,&len,&(tempPtrToCond->expr));
				if(rc != SUCCESS)
					return rc;
				
				/*otherwise Parse all the CASE branches and the DEFAULT */
				while (len > 0)
				{
					DDL_PARSE_TAG(chunkp,&length,&tag1,&len1);

					switch(tag1)
					{
					case CASE_TAG:
						{/* DD CASE's are expressions (variables or calc'd values)*/
							rc = ddl_parse_expression(chunkp, &len, &tempExpr);
							if(rc != SUCCESS)
								return rc;

							tempPtrToCond->caseVals.push_back(tempExpr);
							
							/*We have the case value 
							  Now parse the attributre value from the following chunk	*/
							rc = ddl_parse_conditional_list(tempPtrToCond, chunkp,
															       &len, GRID_SEQLIST_TAG);
							if (rc != SUCCESS)
								return rc; /* Return if not successful*/

							tempPtrToCond->byNumberOfSections++;							
							tempExpr.clear();
						}
						break;/*End CASE_TAG*/

					case DEFAULT_TAG:
						{
							tempPtrToCond->byNumberOfSections++;

							rc = ddl_parse_conditional_list(tempPtrToCond, chunkp,
															       &len, GRID_SEQLIST_TAG);
							if (rc != SUCCESS)
								return rc; /* Return if not successful*/
						}
						break;/*End DEFAULT_TAG*/

					default:
						return DDL_ENCODING_ERROR;

					}/*End Switch tagp*/

				}/* wend len */

				if(pAttr->bIsAttributeConditionalList == true)
				{/*then we have to push this conditional value on conditionalVals*/
					pAttr->conditionalVals.push_back(tempPtrToCond);
				}
			}
			break; /*End SELECT_TAG*/

		case OBJECT_TAG : /*We have a direct object*/
			{
				if((pAttr->byNumOfChunks == 0) && length == 0)
				{ /*We have a direct list in a single chunk*/
					pAttr->pVals = new VALUES;

					pAttr->pVals->gridMemList = new GRID_SET_LIST;

					rc = ddl_parse_gridMembers(chunkp, &len, pAttr->pVals->gridMemList);
					if(rc != DDL_SUCCESS)
						return rc;

				}
				else /*We are having a  possible combination of direct & conditional chunks*/
				{ /*Spl case of all chunks having direct Values , we'll handle after looping */

					tempVal.gridMemList = new GRID_SET_LIST;

					rc = ddl_parse_gridMembers(chunkp, &len, tempVal.gridMemList);					
					if(rc != DDL_SUCCESS)
						return rc;

					pAttr->directVals.push_back(tempVal);

					pAttr->isChunkConditionalList.push_back(DDL_SECT_TYPE_DIRECT);								
								
					/*Just set the conditional flag every time we come here irrespective of 
					 whether or not its set earlier, If we have chunks of non conditionals,
					we will reset this flag later*/
					pAttr->bIsAttributeConditional = true;
							
				}
				pAttr->byNumOfChunks++;							
			}
			break;/*End OBJECT_TAG*/
						 
		default:
			return DDL_ENCODING_ERROR;
			break;
		}/*End switch tag*/
	}/*wend length > 0 */

	if(    pAttr->pVals == NULL 
		&& pAttr->bIsAttributeConditionalList == false 
		&& (pAttr->conditionalVals.size() == 0)
		&& (pAttr->directVals.size() > 0) ) /*The last one is a double check*/
	{
		/*We have a Direct list in more than one chunk!!!!
		 So we will copy the same to pAttr->Vals->enmList*/
		pAttr->pVals = new VALUES;
		pAttr->pVals->gridMemList = new GRID_SET_LIST;

		GRID_SET_LIST::iterator itGridSet; // a ptr 2a GRID_SET
		GRID_SET*                pGridSet =  NULL;

		for (unsigned i = 0; i < pAttr->directVals.size(); i++)// vector of VALUES 
		{
			tp_2gridSetList = pAttr->directVals.at(i).gridMemList;
			// was:for (int j =0; j < tp_2gridSetList->size(); j++)
			for (itGridSet  = tp_2gridSetList->begin(); 
				 itGridSet != tp_2gridSetList->end();   ++itGridSet)// stevev 06aug07
			{
				//tmpGridSet = tp_2gridSetList->at(j);
				//pAttr->pVals->gridMemList->push_back(tmpGridSet);
//					 pGridSet = (GRID_SET*)itGridSet;	see below
 					 pGridSet = (GRID_SET*)&(*itGridSet);// PAW changed from above 03/03/09
				pAttr->pVals->gridMemList->push_back( *pGridSet );
				pGridSet->Cleanup();

			}/*next j*/
					
			/*Just clear this list, it's not required any more*/
			tp_2gridSetList->clear();
			delete pAttr->directVals.at(i).gridMemList;// may as well recover the memory while we're here
			pAttr->directVals.at(i).gridMemList = NULL;
		}/*next i*/
				
		/*Now clear the directVals list too*/

		pAttr->directVals.clear();

		/*Reset the bIsAttributeConditional flag*/
		pAttr->bIsAttributeConditional = false;
	
	}/*Endif*/

	/*If due to some combination both Conditional & ConditionalList Flags are set, 
	 Reset the bIsAttributeConditional */
	if(pAttr->bIsAttributeConditional == true && pAttr->bIsAttributeConditionalList == true)
	{
		pAttr->bIsAttributeConditional = false;
	}

	return SUCCESS;

}/*End parse_gridmembers_list*/


/*Vibhor 280904: End of Code*/

/* stevev 10may05 */
//  helper function for the other two 
int parse_attr_param(METHOD_PARAM* pParam, unsigned char** binChunk, unsigned long& size)
{
	int rc = SUCCESS;
	unsigned long tag, len;
	unsigned __int64 LL;
	string   sName;
	assert(pParam != NULL);

	DDL_PARSE_TAG(binChunk,&size,&tag,&len);

	if(tag != PARAMETER_TAG)
		return DDL_ENCODING_ERROR;
	size -= len;// assume we will handle the whole thing
	
	//pAttr->pVals = new VALUES;
	// encoded integer var type
	
	DDL_PARSE_INTEGER(binChunk,&len,&LL); pParam->param_type = (INT32)LL;
	// type modifiers  bitstring
	rc = ddl_parse_bitstring(binChunk,&len,(ulong*)&(pParam->param_modifiers) );
	if(rc != SUCCESS)
			return rc;
	//int (string& retStr, unsigned char* binChunk, unsigned long& size)
	if ( len == 0 )
	{
		LOGIT(CERR_LOG,"Param parsed with no name.\n");
		pParam->param_name = NULL;
		return rc;
	}
	else
	{
		rc = parse_ascii_string(sName,binChunk,len);
		if(rc != SUCCESS)
				return rc;
		pParam->param_name =  new char[sName.length()+1];
#ifdef MODIFIED_BY_SS
			std::copy_n(sName.c_str(), sName.length(), pParam->param_name);
#else
		strcpy(pParam->param_name, sName.c_str());
#endif
		
	}
	// exit
	return rc;
} /* end parse_attr_param */

int parse_attr_method_type(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	assert(pAttr != NULL);

	pAttr->pVals             = new VALUES;
	pAttr->pVals->methodType = new METHOD_PARAM;

	return (parse_attr_param(pAttr->pVals->methodType, &binChunk, size));
} /* end parse_attr_method_type*/

int parse_attr_param_list(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = SUCCESS;
	unsigned long tag, len;
	METHOD_PARAM  wrkParam;

	assert(pAttr != NULL);

	// explicit tag PARAMETER_SEQLIST_TAG
	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(tag != PARAMETER_SEQLIST_TAG || len == 0)
		return DDL_ENCODING_ERROR;

	size -= len;// assume we will handle the whole thing	
	pAttr->pVals            = new VALUES;
	pAttr->pVals->paramList = new METHOD_PARAM_LIST;

	while (len)
	{
		rc = parse_attr_param(&wrkParam, &binChunk, len);
		if ( rc != SUCCESS)
			return rc;
		//else
		pAttr->pVals->paramList->push_back(wrkParam);
		wrkParam.Clear();
	}
	// exit
	return rc;
}/* end parse_attr_paramlist */

/* stevev 10may05 - end */

int parse_attr_time_scale(DDlAttribute *pAttr, unsigned char* binChunk, unsigned long size)
{
	int rc = SUCCESS;
	unsigned long tag, len;

	DDL_PARSE_TAG(&binChunk,&size,&tag,&len);

	if(tag != TIME_SCALE_TAG)
		return DDL_ENCODING_ERROR;

	/*Now its just an integer */
	pAttr->pVals = new VALUES;

	/*Simply parse the integer & return */
	DDL_PARSE_INTEGER(&binChunk,&size,&pAttr->pVals->ullVal);

	return rc;


}/*End parse_attr_time_scale*/