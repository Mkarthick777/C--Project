
/*!!!!!   Take care of moderating the contents of this file		!!!!!*/


#ifndef ATTRS_H
#define ATTRS_H

//#include <windows.h>

#include <iostream>
#include "std.h"

#include "DDLDefs.h"

// moved to common as tags_sa.h    #include "DDLTags.h"
#include "Tags_sa.h"

#include <vector>

using namespace std;
#ifdef _DBGMIL
#include "logging.h"
#endif

/*#ifdef __cplusplus
	extern "C" {
#endif /* __cplusplus */

/*
 * Expressions
 */

/*
 * Types of expressions.
 * The permitted type fields are the same as variables types (ddldefs.h)
 */

class Element; /*Forward declaration*/

// temp - try something different...typedef vector	<Element>  Expression;

class ddpExpression : public vector<Element>  
{
public:
	ddpExpression() 
	{ clear();	};
	ddpExpression(const ddpExpression& src){ operator=(src); };
	~ddpExpression();
	ddpExpression& operator=(const ddpExpression& src);
};

#define ATTRIBUTE_INDEX_DEF 1
typedef	unsigned short	OBJECT_INDEX;
typedef	int	SUBINDEX;

/*
 * Defines for REFERENCE_ITEM types
 */


/*Vibhor 260903: Adding following definition for ddpREFERENCE*/
/* stevev 1apr09 - make this a class to see if the stack trashing can be found
struct REF {
	unsigned short  type;

	union {
		ITEM_ID         id;		// image number too  
		Expression *     index;
		unsigned long   member;	// member name id 

	}               val;

	REF (){type = DEFAULT_TYPE_REF;val.index = NULL;val.id = 0;};
	REF (const REF& r);  // copy constructor for vector pushes
	~REF(){ Cleanup(); };
	
	void Cleanup ();
	
};
*  substituted by the following class ***/
class ddpREF 
{
public:
	unsigned short  type;

	union 
	{
		ITEM_ID         id;		// image number too  
		ddpExpression *     index;
		unsigned long   member;	// member name id 

	}val;

public:
	ddpREF ()
	{	type = DEFAULT_TYPE_REF;
		val.index = NULL;
		val.id = 0;
	};
	ddpREF (const ddpREF& r):type(0){val.index=NULL; operator=(r); };  // copy constructor for vector pushes
	~ddpREF()				{ Cleanup(); 	};
	
	void Cleanup ();

	ddpREF& operator= (const ddpREF& src);
	
};


//typedef vector <REF> ddpREFERENCE; /*Though defined as a vector , its just one Reference */
// temp
class ddpREFERENCE : public vector <ddpREF>
{
public:
	ddpREFERENCE() { clear(); };
	~ddpREFERENCE(){Cleanup();};
	ddpREFERENCE&  operator= (const ddpREFERENCE& src);
	ddpREFERENCE   (const ddpREFERENCE& r)	{ operator=(r); };

void Cleanup();		// PAW return parameter added 07/04/09

public:
//	void* First(void) { return ((void*)_First);};removed for debug PAW 07/04/09
// left out, apparently not needed.  10aug10 stevev
};

typedef vector <ddpREFERENCE> REFERENCE_LIST;/*This is actual List of Reference */


/*
 * Binary and Dependency Info
 */

struct BININFO{
	unsigned long   size;
	unsigned char  *chunk;
};

struct DEPBIN{
	unsigned long   bin_size;
	unsigned char  *bin_chunk;
};


#define	DONT_FREE_STRING	0X00
#define	FREE_STRING			0X01
#define ISEMPTYSTRING       0X10

/*Vibhor 230903 : Defining this structure for handling enumerated varible 
references in strings */

struct ddbENUM_REF{
	
	union{
		ITEM_ID		iD;  /*valid if ENUMERATION_STRING_TAG*/
		ddpREFERENCE	*ref; /*valid if ENUM_REF_STRING_TAG */ //Vibhor 051103
	}enmVar;
	unsigned long enumValue;
	ddbENUM_REF(){enmVar.ref=NULL;enmVar.iD=0;enumValue=0xffffffff;};
};


class ddpSTRING 
{
public:	
	char            *str;	/* the pointer to the string */
	unsigned short  len;	/* the length of the string */
	unsigned short  flags;	/* memory allocation flags */
	unsigned long	strType; /*Simply store the tag of string type*/

	/*The above 3 guys would be Null in the following cases*/

	ddbENUM_REF		enumStr; /*reference to an enumerated varible*/
	ITEM_ID			varId; /*id of a var of type string to be entered by the user @ runtime*/
	ddpREFERENCE 		varRef; /* reference to a variable of type string
							   if present, this value has to be 
								entered by the user @ runtime!!!*/
public:
	ddpSTRING();
	ddpSTRING& operator=(const ddpSTRING& srcStr );
	ddpSTRING( const ddpSTRING& s) : str(NULL),len(0),flags(0),strType(0),varId(0)
									{  operator=(s);} ;
	~ddpSTRING() { Cleanup(); };

	void Cleanup();
};


/* Masks for all members */

#define MEMBER_DESC_EVALED		0X01
#define MEMBER_HELP_EVALED		0X02
#define MEMBER_LOC_EVALED		0X04
#define MEMBER_REF_EVALED		0X08

/* The masks for ITEM_ARRAY_ELEMENT */

#define IA_DESC_EVALED		MEMBER_DESC_EVALED
#define IA_HELP_EVALED		MEMBER_HELP_EVALED
#define IA_INDEX_EVALED		MEMBER_LOC_EVALED
#define IA_REF_EVALED		MEMBER_REF_EVALED


class ITEM_ARRAY_ELEMENT
{
public:
	unsigned short  evaled;
	unsigned long   index;
	ddpREFERENCE		item;
	ddpSTRING          desc;
	ddpSTRING          help;
	string	        mem_name;/* collections only stevev 15sep05 (for now)*/

public:
	void Cleanup();
	ITEM_ARRAY_ELEMENT(){evaled=0;index=0;};
	ITEM_ARRAY_ELEMENT(const ITEM_ARRAY_ELEMENT& iae) : 
						item(iae.item),desc(iae.desc),help(iae.help)
	{evaled=iae.evaled;index=iae.index;mem_name=iae.mem_name;};
	~ITEM_ARRAY_ELEMENT(){ Cleanup(); };
	ITEM_ARRAY_ELEMENT& operator=(const ITEM_ARRAY_ELEMENT& s)
	{evaled = s.evaled;index= s.index;mem_name= s.mem_name; 
	 item   = s.item;  desc = s.desc; help    = s.help;   return *this;};
};


typedef vector <ITEM_ARRAY_ELEMENT> ITEM_ARRAY_ELEMENT_LIST;


/* The masks for MEMBER */

#define MEM_DESC_EVALED		MEMBER_DESC_EVALED
#define MEM_HELP_EVALED		MEMBER_HELP_EVALED
#define MEM_NAME_EVALED		MEMBER_LOC_EVALED
#define MEM_REF_EVALED		MEMBER_REF_EVALED


struct MEMBER{
	unsigned short  evaled;
	unsigned long   name;
	ddpREFERENCE		item;
	ddpSTRING          desc;
	ddpSTRING          help;
	string	        member_name;

	void Cleanup();
	MEMBER(){evaled=0;name=0;};
	MEMBER(const MEMBER& iae) : item(iae.item),desc(iae.desc),help(iae.help)
	{evaled=iae.evaled;name=iae.name;member_name=iae.member_name;};
	~MEMBER(){ Cleanup(); };
	MEMBER& operator=(const MEMBER& s)
	{Cleanup(); evaled=s.evaled;name=s.name;item=s.item;desc=s.desc;
	 help=s.help;member_name=s.member_name; return *this;};
			
};


typedef vector <MEMBER> MEMBER_LIST;

/*
 * Menu
 */

class MENU_ITEM
{
public:
	ddpREFERENCE	    item;
	unsigned short  qual; /*This is a bit string*/
	
	MENU_ITEM(){
	qual=0;
	};
	MENU_ITEM(const MENU_ITEM& mi):item(mi.item),qual(mi.qual){};
	~MENU_ITEM(){Cleanup();};
	void Cleanup();
	MENU_ITEM& operator=(const MENU_ITEM& s){item = s.item;qual = s.qual; return *this;};

#ifdef _DBGMIL
	void dumpItem(void);
#endif
};

#ifdef _DBGMIL
class MENU_ITEM_LIST : public vector <MENU_ITEM> 
{
public:
	void dumpList(void)
	{	LOGIT(COUT_LOG,"-----  Menu Item List at <%p>  ------\n",this);
		MENU_ITEM* pMI;
		MENU_ITEM_LIST::iterator iX;
		int y = 0;
		for ( iX = begin(); iX != end(); ++iX,y++)
		{
			pMI = (MENU_ITEM*)iX;
			LOGIT(COUT_LOG,"MI %2d] <%p> qualifier %d\n",y,pMI,pMI->qual);
			pMI->dumpItem();
		}
		LOGIT(COUT_LOG,"----------------------------------------------------------------\n");
	};
};
#else /* non-debug mode */
typedef vector <MENU_ITEM> MENU_ITEM_LIST;
#endif

/*
 * Response Codes
 */

/* The masks for RESPONSE_CODE */

#define RS_DESC_EVALED		0X01
#define RS_HELP_EVALED		0X02
#define RS_TYPE_EVALED		0X04
#define RS_VAL_EVALED		0X08


class RESPONSE_CODE
{
public:	
	unsigned short  evaled;
	unsigned short	val;
	unsigned short	type;
	ddpSTRING          desc;
	ddpSTRING          help;

	RESPONSE_CODE(){
		evaled=0,val=0,type=0;
	};
	RESPONSE_CODE(const RESPONSE_CODE& rc):desc(rc.desc),help(rc.help)
	{	evaled=rc.evaled;val=rc.val;type=rc.type;};
	~RESPONSE_CODE(){Cleanup();};
	void Cleanup();
	RESPONSE_CODE& operator=(const RESPONSE_CODE& s)
	{	desc=s.desc;help=s.help;evaled=s.evaled;val=s.val;type=s.type;return *this;};
};


typedef  vector <RESPONSE_CODE> RESPONSE_CODE_LIST;

/*
 * Relation Types
 */


class REFRESH_RELATION
{
public:	
		REFERENCE_LIST	 watch_list;
		REFERENCE_LIST	 update_list;

		REFRESH_RELATION(){};
		REFRESH_RELATION(const REFRESH_RELATION& rr){operator=(rr);};
		~REFRESH_RELATION(){Cleanup();};

	REFRESH_RELATION& operator=(const REFRESH_RELATION& rr);

	void Cleanup();		
};


class UNIT_RELATION
{
public:		
		ddpREFERENCE		unit_var;
		REFERENCE_LIST	var_units;

		UNIT_RELATION(){};
		UNIT_RELATION(const UNIT_RELATION& ur){operator=(ur);};
		~UNIT_RELATION(){Cleanup();};

	UNIT_RELATION& operator=(const UNIT_RELATION& ur);

	void Cleanup();
};


/*
 * Definitions
 */

struct DEFINITION{

	unsigned long   size;
	char           *data;
	
//	DEFINITION(){};
//	~DEFINITION(){ if (data != NULL){ delete data;}};

};


struct METHOD_PARAM
{
	int param_type;
	int param_modifiers;
	char* param_name;//struct owns memory-delete[]

	void Clear(){param_type=param_modifiers=0;param_name=NULL;}

	METHOD_PARAM(){Clear();};
	METHOD_PARAM(const METHOD_PARAM& s):param_type(0),param_modifiers(0),param_name(NULL)
																		{operator=(s);};
	~METHOD_PARAM(){ if (param_name)delete[] param_name; Clear();};
	/*METHOD_PARAM& operator=(const METHOD_PARAM& s){param_type=s.param_type;
		param_modifiers=s.param_modifiers;
		if (s.param_name) {param_name=new char[strlen(s.param_name)+1];
		strncpy(param_name,s.param_name,strlen(s.param_name)+1);}
		else param_name = NULL;    return *this;};*/
	// added by SS
	METHOD_PARAM& operator=(const METHOD_PARAM& s) {
		// Assign param_type and param_modifiers directly
		param_type = s.param_type;
		param_modifiers = s.param_modifiers;

		// If s.param_name is not NULL
		if (s.param_name) {
			// If param_name is already allocated, delete it to avoid memory leak
			if (param_name) {
				delete[] param_name;
			}

			// Use std::string for dynamic memory allocation and string handling
			param_name = new char[strlen(s.param_name) + 1];
			std::string sParamName = s.param_name;
			std::copy(sParamName.begin(), sParamName.end(), param_name);
			param_name[sParamName.size()] = '\0'; // Ensure null-termination
		}
		else {
			// If s.param_name is NULL, set param_name to NULL
			param_name = nullptr;
		}

		return *this;
	}
};

typedef vector <METHOD_PARAM> METHOD_PARAM_LIST;

/*
 * Variable Types
 */

struct TYPE_SIZE{
	unsigned short  type;
	unsigned short  size;
};


/*
 * Enumerations
 */

struct OUTPUT_STATUS{
	unsigned short  kind;
	unsigned short  which;
	unsigned short  oclass;

	OUTPUT_STATUS(){ kind = 0;which = 0; oclass = 0;};
//	~OUTPUT_STATUS(){};
	
};



typedef vector<OUTPUT_STATUS> OUTPUT_STATUS_LIST;

struct BIT_ENUM_STATUS{
	unsigned long   status_class;
	OUTPUT_STATUS_LIST oclasses;
	
	BIT_ENUM_STATUS(){ status_class = 0; };
	BIT_ENUM_STATUS(const BIT_ENUM_STATUS& bes):oclasses(bes.oclasses)
									{status_class = bes.status_class;};
	~BIT_ENUM_STATUS()
	{	oclasses.clear();	};
	BIT_ENUM_STATUS& operator=(const BIT_ENUM_STATUS& s)
	{	status_class=s.status_class;	oclasses=s.oclasses;  return *this; };
};


/* The masks for ENUM_VALUE */

#define ENUM_ACTIONS_EVALED		0X01
#define ENUM_CLASS_EVALED		0X02
#define ENUM_DESC_EVALED		0X04
#define ENUM_HELP_EVALED		0X08
#define ENUM_STATUS_EVALED		0X10
#define ENUM_VAL_EVALED			0X20


class ENUM_VALUE
{
public:
	unsigned short  evaled;
	unsigned long   val;
	ddpSTRING          desc;
	ddpSTRING          help;
	unsigned long   func_class;	/* functional class */
	BIT_ENUM_STATUS status;
	ITEM_ID         actions;

	ENUM_VALUE(){ func_class = 0; actions = 0;evaled=0,val=0,func_class=0;};
	ENUM_VALUE(const ENUM_VALUE& ev):desc(ev.desc),help(ev.help),status(ev.status)
	{
		evaled = ev.evaled;				val = ev.val;
		func_class = ev.func_class;		actions = ev.actions;
	};
	void Cleanup();
	~ENUM_VALUE(){Cleanup();};
	ENUM_VALUE& operator=(const ENUM_VALUE& s )
	{	desc = s.desc;		help       = s.help;		status = s.status;	  evaled  = s.evaled;	
		val  = s.val;		func_class = s.func_class;	actions = s.actions;  return *this;
	};
};


typedef vector <ENUM_VALUE> ENUM_VALUE_LIST;


/*
 * Data Fields
 */

class DATA_ITEM
{
public:
	union {
		unsigned short  iconst;
		ddpREFERENCE*      ref;
		float           fconst;
	}               data;
	unsigned short  type;
	unsigned short  flags;
	// stevev 18jun09 ...this is NO LONGER WIDTH... unsigned short  width;
	UINT64 mask;
	
	//DATA_ITEM(){ data.ref = NULL;data.fconst=0.0;type=0;flags=0;width=0;};
	DATA_ITEM(){ data.ref = NULL;data.fconst=0.0;type=0;flags=0;mask=0;};
	//DATA_ITEM(const DATA_ITEM& di):type(0),flags(0),width(0)
	DATA_ITEM(const DATA_ITEM& di):type(0),flags(0),mask(0)
														{data.ref=NULL; operator=(di);};
	void Cleanup();
	DATA_ITEM& operator=(const DATA_ITEM& di);
	~DATA_ITEM(){Cleanup();};
};


//typedef vector<DATA_ITEM> DATA_ITEM_LIST;
class  DATA_ITEM_LIST : public vector<DATA_ITEM> 
{
public:
	DATA_ITEM_LIST(){ clear(); };
	DATA_ITEM_LIST(const DATA_ITEM_LIST& src){operator=(src);};
	~DATA_ITEM_LIST(){ Cleanup();};
	DATA_ITEM_LIST& operator=(const DATA_ITEM_LIST& src);

	void Cleanup();	// PAw return parameter added 07/04/09
};


/*
 * Transactions
 */

typedef vector<ITEM_ID> ITEM_ID_LIST;

class TRANSACTION
{
public:
	unsigned long   number;
	DATA_ITEM_LIST  request;
	DATA_ITEM_LIST  reply;
	RESPONSE_CODE_LIST rcodes;
	REFERENCE_LIST	post_rqst_rcv_act;
	
	TRANSACTION():number(0)
					{request.clear();reply.clear();rcodes.clear();post_rqst_rcv_act.clear();};
	TRANSACTION(const TRANSACTION& src):number(0xffffffff){ operator=(src);};
	TRANSACTION& operator=(const TRANSACTION& s)
	{	number = s.number;		request = s.request;	
		reply  = s.reply;	     rcodes = s.rcodes;	
		post_rqst_rcv_act   =   s.post_rqst_rcv_act;
		return *this;};
	~TRANSACTION(){Cleanup();};

	void Cleanup();
};


typedef vector <TRANSACTION> TRANSACTION_LIST;

class MIN_MAX
{
public:
	unsigned long	which; /*which min/max value*/
	bool            isID;  // stevev 21aug07 - we gotta know which of the union is valid! 
	union
	{
		ITEM_ID		  id; /*ID of the variable in case of MIN / MAX -ID*/
		ddpREFERENCE*   ref;/*Reference of the vaiable in case of MIN / MAX  - REF*/
	}
	variable;

public:
	void clear() {which = 0; variable.ref = NULL; isID = false;};

	MIN_MAX(){clear();};
	MIN_MAX(const MIN_MAX& s):which(0),isID(false) {variable.ref=NULL;operator=(s);};
	~MIN_MAX(){Cleanup();};
	MIN_MAX& operator=(const MIN_MAX& s);

	void Cleanup();
	// special constructor for ids
	MIN_MAX(unsigned long w, unsigned long symID){which=w;isID=true;variable.id=symID;};
};



/*Vibhor 280903 : Moving this definition from DDLConditional.h */

class Element
{
	
public:

	//BYTE  byElemType ; /*1-21 =>OpCode ; 22 =>Int 23=>float 24-29 => ref_id */
	unsigned char byElemType;
	union {
	//	BYTE			byOpCode;
		unsigned char	byOpCode;
		UINT64          ulConst; // from ulong 18jun08 stevev
		float			fConst;
		ddpSTRING*         pSTRCNST;
		ITEM_ID			varId;  /* for VAR_ID*/
		ddpREFERENCE*		varRef; /* fpr VAR_REF*/
		MIN_MAX*		minMax; /* for MIN/MAX - ID & REF*/// made ptr stevev 21aug07 
	}	elem;

	//Element(){byElemType = 0;  elem.pSTRCNST = NULL; elem.minMax.clear();};// it's a union sjv
	Element(){byElemType = 0;  elem.ulConst = 0; };// minmax 2 ptr 21aug07 sjv
	Element( const Element& cconst):byElemType(0){elem.ulConst=0;operator=(cconst); };
	~Element() { Cleanup(); };
	Element&   operator=(const Element& src);

	//void clean(){ byElemType = 0;/* elem = {0}; */ elem.minMax.clear();};
	//void clean(){ byElemType = 0; elem.minMax->clear();};
	
	void Cleanup();
};

/* stevev 24aug06 - the Attribute is actually a list of MIN_MAX 
 *	 but the MIN_MAX is a which AND a Conditional that resolves to an expression.
 *	 ie the List is Non-Conditional but the Value IS conditional.

struct MIN_MAX_VALUE{
	unsigned long which; //which min-max
	Expression	  value; //expression containing the min / max value 

	MIN_MAX_VALUE(){};
	void Cleanup();

};
*/

/* moved from conditional.h because this file is included in conditional.h (we need it here)*/
typedef enum {
	DDL_SECT_TYPE_DIRECT = 0  /*Direct*/
	, DDL_SECT_TYPE_CONDNL   /*Condiional*/
	, DDL_SECT_TYPE_CHUNKS    /*Possible Combination of Direct & Conditional*/
} DDL_COND_SECTION_TYPE;

typedef vector  <DDL_COND_SECTION_TYPE>     SectionCondList;
class DDlConditional;

union VALUES;
typedef vector  <VALUES>   ValueList;


struct MIN_MAX_VALUE{
	unsigned long which; //which min-max
	//Expression	  value; //expression containing the min / max value 
//	bool isMinMaxConditional;
	DDlConditional	*pCond;	//DDL_ATTR_DATA_TYPE_EXPRESSION
//	VALUES			*pVals;
//	ValueList		 directVals;
//	SectionCondList	 isChunkConditionalList;

	MIN_MAX_VALUE(){pCond=NULL; /*pVals = NULL;*/ which = 0;};
	void Cleanup();

};

typedef vector <MIN_MAX_VALUE> MIN_MAX_LIST;



/*Vibhor 230903: Adding these definitions for using in 
parsing of TYPE_SIZE attribute of a var*/

#define DEFAULT_SIZE		1
#define BOOLEAN_SIZE		1
#define FLOAT_SIZE			4
#define DURATION_SIZE		6
#define TIME_SIZE			6
#define TIME_VALUE_SIZE		4		// timj 4jan08
#define DATE_AND_TIME_SIZE	7
#define DOUBLE_SIZE			8

#define DATE_SIZE			3
/*
 * Mask used to peek at the next tag in the chunk
 */
#define TAG_MASK 0x7f

#define HART_ARRAYNAME_INDICATOR	0	/* used only for type INDEX */
#define HART_SUBATTR_INDICATOR		1	/* list of subattributes exists */


/*Vibhor 270804: Start of Code*/

struct LINE_TYPE{

	unsigned short type;
	int			   qual; /*1. valid only if type == DATA
						   2. if qual = Null ==> DATA, 
							  if qual > 0 ==> DATA#*/

};
/*End LINE_TYPE*/

/*Vibhor 270804: End of Code*/

/* stevev 25mar05 - adding items */


/*
 * Grid Member Fields
 */

struct GRID_SET 
{
	ddpSTRING         desc;
	REFERENCE_LIST values;
	
	GRID_SET(){ };
	GRID_SET(const GRID_SET& srcGS);
	~GRID_SET(){ Cleanup(); };

	void Cleanup();
};


typedef vector<GRID_SET> GRID_SET_LIST;

/* end stevev 25mar05 */

/* stevev 06may05 ---- debug info */
typedef struct MEMBER_DEBUG_s
{
	string	symbol_name;
	unsigned long flags;
	unsigned long member_value;

	MEMBER_DEBUG_s(){flags=0;member_value=0;};
	~MEMBER_DEBUG_s(){symbol_name.erase();};
}
MEMBER_DEBUG_T;
typedef vector<MEMBER_DEBUG_T> MEMBER_DEBUG_LIST;

typedef struct ATTR_DEBUG_INFO_s
{
	unsigned long	attr_tag;
	unsigned long	attr_lineNo;
	ddpSTRING			attr_filename;
	MEMBER_DEBUG_LIST	attr_member_list;/* empty if not a member type */

	ATTR_DEBUG_INFO_s(){attr_tag=attr_lineNo=0;};
	void Cleanup();
}
ATTR_DEBUG_INFO_T;
typedef vector<ATTR_DEBUG_INFO_T*> ATTR_DEBUG_LIST;

struct ITEM_DEBUG_INFO
{
	string			symbol_name;
	ddpSTRING			file_name;
	unsigned long	lineNo;
	unsigned long	flags;
	ATTR_DEBUG_LIST attr_list;
	
	ITEM_DEBUG_INFO(){ lineNo=0;flags=0;};
	void Cleanup()   /* delete the memory in attr list of ptrs */;
};
/* end  stevev 06may05 */
/*
#ifdef __cplusplus
	 }
#endif /* __cplusplus */

#ifndef TOK

  #ifndef linux
    #pragma message("TOK not defined")
  #endif // linux

#else  /*ifdef TOK*/
//#ifdef __cplusplus
//	extern "C" {
//#endif /* __cplusplus */

		/*
 * Expressions
 */

/*
 * Types of expressions.
 * The permitted type fields are the same as variables types (ddldefs.h)
 */

typedef struct {
	unsigned short  type;	/* valid types are defined in ddldefs.h (ie. INTEGER) */
	unsigned short  size;
	union {
		float           f;
		double          d;
		// no longer    unsigned long   u;
		// big enough   long            i;
		unsigned __int64 u;	// 30aug11 - to make compatible with tokenizer
		__int64          i;

	}               val;
}               EXPR;


/*
 *  RANGE_DATA_LIST
 */

typedef struct {
	unsigned short  count;
	unsigned short  limit;
	EXPR           *list;
}               RANGE_DATA_LIST;


typedef struct {
	ITEM_TYPE       type;
	ITEM_ID         id;
	unsigned long   element;
}               RESOLVE_INFO;

typedef struct {

	ITEM_ID         id;
	SUBINDEX        subindex;
	ITEM_TYPE       type;
}               OP_REF;

typedef struct {
	ITEM_ID         op_id;
	SUBINDEX        op_subindex;
	ITEM_TYPE       op_type;
	ITEM_ID         desc_id;
	ITEM_TYPE       desc_type;
	unsigned short  trail_count;
	unsigned short  trail_limit;
	RESOLVE_INFO   *trail;
}               OP_REF_TRAIL;


typedef struct {
	unsigned short  count;
	unsigned short  limit;
	OP_REF_TRAIL   *list;
}               OP_REF_TRAIL_LIST;


typedef struct atrs_string{
	char            *str;	/* the pointer to the string */
	unsigned short  len;	/* the length of the string */
	unsigned short  flags;	/* memory allocation flags */
}               STRING;


//#ifdef __cplusplus
//	 }
//#endif /* __cplusplus */

#endif // TOK

//////#pragma message("outa Attributes.h")
#endif	/* ATTRS_H */
