
#ifndef DDLCONDITIONAL_H
#define DDLCONDITIONAL_H


//#include <windows.h>

#ifndef MAX_PATH
#define MAX_PATH  260   /* to match windef.h...was:: 512 22mar06 sjv */
                        /* 512 causes a crash in the Parser - memory */
#endif
#include "ddbGeneral.h"

//#include <stdio.h>
#include <string>

#include <vector>

using namespace std;

#include "Attributes.h"


/*This file contains the DDlConditional class which serves as the 
base class for all the attributes of all the DD Items , each attribute 
is instantiated as a specific object of this class */





typedef enum {
	
	DDL_ATTR_DATA_TYPE_UNDEFINED = 0
	, DDL_ATTR_DATA_TYPE_INT
	, DDL_ATTR_DATA_TYPE_UNSIGNED_LONG
	, DDL_ATTR_DATA_TYPE_FLOAT
	, DDL_ATTR_DATA_TYPE_DOUBLE
	, DDL_ATTR_DATA_TYPE_BITSTRING /* for this data type we'll parse a ulong value only*/
	, DDL_ATTR_DATA_TYPE_STRING
	, DDL_ATTR_DATA_TYPE_ITEM_ID
	, DDL_ATTR_DATA_TYPE_ENUM_LIST
	, DDL_ATTR_DATA_TYPE_REFERENCE /*Just a single reference*/
	, DDL_ATTR_DATA_TYPE_REFERENCE_LIST /*List of references*/
	, DDL_ATTR_DATA_TYPE_TYPE_SIZE
	, DDL_ATTR_DATA_TYPE_TRANSACTION_LIST
	, DDL_ATTR_DATA_TYPE_RESPONSE_CODE_LIST
	, DDL_ATTR_DATA_TYPE_MENU_ITEM_LIST
	, DDL_ATTR_DATA_TYPE_DEFINITION
	, DDL_ATTR_DATA_TYPE_REFRESH_RELATION
	, DDL_ATTR_DATA_TYPE_UNIT_RELATION
	, DDL_ATTR_DATA_TYPE_ITEM_ARRAY_ELEMENT_LIST
	, DDL_ATTR_DATA_TYPE_MEMBER_LIST
	, DDL_ATTR_DATA_TYPE_EXPRESSION
	, DDL_ATTR_DATA_TYPE_MIN_MAX
	, DDL_ATTR_DATA_TYPE_LINE_TYPE	/*DDl6 :: Waveform,Source*/
	, DDL_ATTR_DATA_TYPE_WAVEFORM_TYPE //All below are just indicators for spl parse fns
	, DDL_ATTR_DATA_TYPE_CHART_TYPE //But all these will be stored as ulVal in VALUES 
	, DDL_ATTR_DATA_TYPE_MENU_STYLE
	, DDL_ATTR_DATA_TYPE_SCOPE_SIZE
	, DDL_ATTR_DATA_TYPE_GRID_SET	/* stevev 25mar05 */
	, DDL_ATTR_DATA_TYPE_DEBUG_DATA /* stevev 10may05 */
	, DDL_ATTR_DATA_TYPE_PARAM      /* stevev 10may05 */
	, DDL_ATTR_DATA_TYPE_PARAM_LIST
	

/* I think we need a data type to hold the references to DD_ITEM also*/

} DDL_ATTR_DATA_TYPE;


typedef enum {

	DDL_COND_TYPE_UNDEFINED = 0
	, DDL_COND_TYPE_IF
	, DDL_COND_TYPE_SELECT
	, DDL_COND_TYPE_DIRECT

} DDL_COND_TYPE;	

/*Vibhor 190105: Start of Code*/
//moved to attributes.h because it is needed there and this includes that
//typedef enum {
//	DDL_SECT_TYPE_DIRECT = 0  /*Direct*/
//	, DDL_SECT_TYPE_CONDNL   /*Condiional*/
//	, DDL_SECT_TYPE_CHUNKS    /*Possible Combination of Direct & Conditional*/
//} DDL_COND_SECTION_TYPE;
//
/*Vibhor 190105: End of Code*/


class DDlConditional; /*Forward refernce*/
class DDlSectionChunks; /*Forward reference*/ //Vibhor 190105: Added

//typedef vector	<Element>  Expression; /*May need to take this definition to Attributes.h*/
//typedef vector  <long>	   CaseConstList;
typedef vector <ddpExpression> CaseConstList;
typedef vector  <DDL_COND_SECTION_TYPE>     SectionCondList; //Vibhor 190105: Changed , was vector <BOOL>
typedef vector  <DDlConditional*> ConditionalList;
typedef vector  <DDlSectionChunks*> SectionChunkList; //Vibhor 190105: Added for "Chunky" conditional sections


/*The value of the attribute*/

/*Which element is valid depends on the DDlConditional :: byAttrDataType */

union VALUES{
	
	//int					iVal;	 
	float				fVal;
	double				dVal;
	//unsigned long		ulVal;
	INT64               llVal;
	UINT64              ullVal;
	ITEM_ID				id;
	ddpREFERENCE*			ref; /*Just a reference!*/
	REFERENCE_LIST*		refList; /* Pointer to a vector of REFERENCEs!!!*/
	ddpSTRING*				strVal;
	ENUM_VALUE_LIST*	enmList;
	TYPE_SIZE			  typeSize;
	TRANSACTION_LIST*   transList; /*This needs to be optimized after */
	RESPONSE_CODE_LIST* respCdList;
	MENU_ITEM_LIST*		menuItemsList;
	DEFINITION			defData; /*Method Definition Data*/
	REFRESH_RELATION*	refrshReln;/* Refresh Relation*/
	UNIT_RELATION*		unitReln;
	ITEM_ARRAY_ELEMENT_LIST* itemArrElmnts;
	MEMBER_LIST*		memberList;
	ddpExpression*			pExpr;
	MIN_MAX_LIST*		minMaxList; /*Pointer to a vector of MIN_MAX_VALUES*/
	LINE_TYPE			lineType; //Vibhor 270804: added
	GRID_SET_LIST*      gridMemList;
	ITEM_DEBUG_INFO*	debugInfo; // stevev 06may05 
	METHOD_PARAM*		methodType;// stevev 10may05
	METHOD_PARAM_LIST*  paramList; // stevev 10may05

	void Cleanup(DDL_ATTR_DATA_TYPE dataType);
};

typedef vector  <VALUES>   ValueList;

class DDlConditional
{

public :
	
	DDL_COND_TYPE		condType;/* IF, SWITCH, DIRECT*/

	DDL_ATTR_DATA_TYPE	attrDataType; /*Ulong, String, ENUM , etc. */
	
	ddpExpression			expr;/*The Expression as a Postfix list of elements */
	
	BYTE				byNumberOfSections;/* The number of Branches the Conditional is having*/

	CaseConstList		caseVals; /*This is a list of expression*/  /* # define DEFAULT Tag to some value*/

	SectionCondList		isSectionConditionalList; /* Vibhor 190105:Direct, Conditional(Nested) , Chunky (Combination)*/

	ValueList			Vals; /* List of Value Structure for each section*/

	ConditionalList		listOfChilds; /* Pointer to child if any ......*/

	SectionChunkList	listOfChunks;/*Vibhor 190105 Added: If a section has chunks, it goes here*/

	DDlConditional(DDL_COND_TYPE   conditionalType =  DDL_COND_TYPE_UNDEFINED,
				   DDL_ATTR_DATA_TYPE  attributeDataType =	DDL_ATTR_DATA_TYPE_UNDEFINED,
				   BYTE	byNumSections = 1 /*Since we have a conditional it means atleast one section*/
				   
				  )

	{
		condType  = conditionalType;
		byNumberOfSections = byNumSections; /*Assuming the attribute is direct by default*/
		attrDataType = attributeDataType; /* this has to be set by the parent!!!!*/
	
	};

	~DDlConditional();
	
};



/*Each and every attribute is has to be derived from the following class*/
/*attrName and ID , and Data Type would be valid for the Top Level Attribute only,
 so initialize them to NULL , by default*/

/* If the Attribute is a conditional then the pointer pCond is made to point 
   to a valid DDlConditional object , but the attrName, byAttrID, */


class DDlAttribute
{


public:
	
	string				attrName; /*Just for Debugging purposes; Name of the attribute or SubAttribute*/
	BYTE				byAttrID; /*Unique Id of the Attribute, # defined in DDlDefs.h as per Bin file spec*/
	DDL_ATTR_DATA_TYPE	attrDataType; /*Ulong, String, ENUM , etc. */

	VALUES				*pVals; /* Pointer to union containig all possible values*/
	bool				bIsAttributeConditional; /* true / false */
	DDlConditional		*pCond; /*== NULL for a DIRECT object, else points to the next 
								conditional branch*/

	/*Vibhor 221003: Adding following to support Conditional Lists of Lists ,
					 basically  list attributes in different chunks. The chunks
					 may be any combination of Direct / IF / SELECT chunks*/

	bool				bIsAttributeConditionalList; /*true if we have a multi-chunk conditional List*/

	SectionCondList		isChunkConditionalList;/* Vibhor 190105: Only possible values here are Direct and Conditional*/

	ValueList			directVals;

	ConditionalList		conditionalVals;

	BYTE				byNumOfChunks;

	DDlAttribute(	string strName = "" , 
					BYTE byAttributeID = DEFAULT_ATTR_ID,
					DDL_ATTR_DATA_TYPE atttributeDataType = DDL_ATTR_DATA_TYPE_UNDEFINED,
					bool bIsConditional = false
	)
	{
		attrName = strName;
		byAttrID = byAttributeID;
		attrDataType =  atttributeDataType;
		pVals			= (VALUES*)NULL;
		bIsAttributeConditional = bIsConditional;
		pCond	= (DDlConditional*)NULL;
		bIsAttributeConditionalList = false;
		byNumOfChunks		   = 0;
	};

	~DDlAttribute();

};

/*Vibhor 190105: Start of Code*/
/*Adding following class to handle conditional chunk lists inside conditionals (sections, like "Then" part of IF or "ELSE's" body or "CASE's" body*/

class DDlSectionChunks
{
public:

	DDL_ATTR_DATA_TYPE	attrDataType; /*Ulong, String, ENUM , etc. */

	SectionCondList		isChunkConditionalList;

	ValueList			directVals;

	ConditionalList		conditionalVals;

	BYTE				byNumOfChunks;


	DDlSectionChunks(DDL_ATTR_DATA_TYPE atttributeDataType = DDL_ATTR_DATA_TYPE_UNDEFINED)
	{
		attrDataType = atttributeDataType;
		byNumOfChunks		   = 0;
	};

	~DDlSectionChunks();
};

/*Vibhor 190105: End of Code*/

#endif /*DDLCONDITIONAL_H*/
