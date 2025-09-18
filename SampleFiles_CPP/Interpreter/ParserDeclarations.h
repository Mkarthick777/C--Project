
#if !defined(INCLUDE_EC779043_753E_4BA7_876F_0AA85AECAD71)
#define INCLUDE_EC779043_753E_4BA7_876F_0AA85AECAD71

//#include "stdafx.h"
#include "typedefs.h"
#include "safevar.h"
//#include "EventLoggerExports.h"

#ifdef __PARSEENGINE_EXPORTS

#define __PARSEENGINE_API __declspec(dllexport)
#define EXPIMP_TEMPLATE

#else

#define __PARSEENGINE_API __declspec(dllimport)
#define EXPIMP_TEMPLATE	extern

#endif


#include <vector>
// note 11mar14 - linux needs a real class...not just a name
class CRIDEError{};
using namespace std;
typedef vector<CRIDEError*> ERROR_VEC;

#define LCUR				'{'
#define RCUR				'}'

#define BUFFER_SIZE			1000
#define LBOX				'['\

/* The maximum parameters the functions can take */
#define MAX_NUMBER_OF_FUNCTION_PARAMETERS	10

// Anil December 16 2005 deleted codes related to Plot builtins. Please refer the previous version

enum RUL_TOKEN_TYPE				{	RUL_TYPE_NONE=0, 
									RUL_KEYWORD, 
									RUL_SYMBOL,
									RUL_ARITHMETIC_OPERATOR, 
									RUL_ASSIGNMENT_OPERATOR,
									RUL_RELATIONAL_OPERATOR,
									RUL_LOGICAL_OPERATOR,
									RUL_NUMERIC_CONSTANT, 
									RUL_STR_CONSTANT,
									RUL_CHR_CONSTANT,
									RUL_SIMPLE_VARIABLE, 
									RUL_ARRAY_VARIABLE, 
									RUL_SERVICE,
									RUL_TYPE_ERROR,
									RUL_COMMENT,
									RUL_EOF,
									RUL_DD_ITEM		//Vibhor 140705: Added
								};


enum RUL_TOKEN_SUBTYPE			{	RUL_SUBTYPE_NONE=0,
									//INT Operators
									RUL_UPLUS, 
									RUL_UMINUS, 
									RUL_PLUS, 
									RUL_MINUS, 
									RUL_MUL, 
									RUL_DIV, 
									RUL_MOD,
									RUL_EXP,
									RUL_NOT_EQ,  
									RUL_LT,
									RUL_GT,
									RUL_EQ,  
									RUL_GE, 
									RUL_LE,
									RUL_LOGIC_AND,
									RUL_LOGIC_OR,
									RUL_LOGIC_NOT,
									RUL_ASSIGN,
									RUL_PLUS_ASSIGN,
									RUL_MINUS_ASSIGN,
									RUL_DIV_ASSIGN,
									RUL_MOD_ASSIGN,
									RUL_MUL_ASSIGN,
									RUL_PLUS_PLUS,
									RUL_MINUS_MINUS,
									RUL_PRE_PLUS_PLUS,
									RUL_PRE_MINUS_MINUS,
									RUL_BIT_AND_ASSIGN,
									RUL_BIT_OR_ASSIGN,
									RUL_BIT_XOR_ASSIGN,
									RUL_BIT_RSHIFT_ASSIGN,
									RUL_BIT_LSHIFT_ASSIGN,
									//FLOAT Operators
									RUL_FUPLUS, 
									RUL_FUMINUS, 
									RUL_FPLUS, 
									RUL_FMINUS, 
									RUL_FMUL, 
									RUL_FDIV,
									RUL_FMOD,
#ifdef XMTR
									RUL_FRAND,
#endif
									RUL_FEXP,
									RUL_I2F,
									RUL_F2I,
									RUL_NOT_FEQ,  
									RUL_FLT,
									RUL_FGT,
									RUL_FEQ,  
									RUL_FGE, 
									RUL_FLE,
									//DOUBLE Operators
									RUL_DUPLUS, 
									RUL_DUMINUS, 
									RUL_DPLUS, 
									RUL_DMINUS, 
									RUL_DMUL, 
									RUL_DDIV,
									RUL_DMOD,
									RUL_DEXP,
									RUL_I2D,
									RUL_D2I,
									RUL_F2D,
									RUL_D2F,
									RUL_NOT_DEQ,  
									RUL_DLT,
									RUL_DGT,
									RUL_DEQ,  
									RUL_DGE, 
									RUL_DLE,
									//String Operators
									RUL_SPLUS,
									RUL_SEQ,
									RUL_NOT_SEQ,
									//Keywords
									RUL_IF,
									RUL_ELSE,
									RUL_SWITCH,
									RUL_CASE,
									RUL_DEFAULT,
									RUL_WHILE,
									RUL_FOR,
									RUL_DO,
									RUL_CHAR_DECL,
									RUL_UNSIGNED_INTEGER_DECL,
									RUL_INTEGER_DECL,
									RUL_LONG_LONG_DECL,//WaltSigtermans March 14 2008 Added 8 byte integer
									RUL_LONG_DECL,
									RUL_UNSIGNED_SHORT_INTEGER_DECL,
									RUL_SHORT_INTEGER_DECL,
									RUL_REAL_DECL, 
									RUL_DOUBLE_DECL, 
									RUL_BOOLEAN_DECL, 
									RUL_STRING_DECL,
									RUL_ARRAY_DECL,
									//Added By Anil June 14 2005 --starts here
									RUL_DD_STRING_DECL,
									//Added By Anil June 14 2005 --Ends here
									RUL_UNSIGNED_CHAR_DECL,
									//Symbols
									RUL_LPAREN,
									RUL_RPAREN,
									RUL_LBRACK,
									RUL_RBRACK,
									RUL_LBOX,
									RUL_RBOX,
									RUL_SEMICOLON,
									RUL_COLON,
									RUL_COMMA,
									RUL_DOT,
									RUL_QMARK,
									RUL_SCOPE,
									//Constants
									RUL_CHAR_CONSTANT, 
									RUL_INT_CONSTANT, 
									RUL_REAL_CONSTANT,
									RUL_BOOL_CONSTANT,
									RUL_STRING_CONSTANT,
									//Service SubTypes
									RUL_SERVICE_INVOKE,
									RUL_SERVICE_ATTRIBUTE,
									//Rule Self Invoke
									RUL_RULE_ENGINE,
									RUL_INVOKE,
									//Object manager
									RUL_OM,
									//General
									RUL_DOLLAR,
									RUL_SUBTYPE_ERROR,
									// Jump statements
									RUL_BREAK,
									RUL_CONTINUE,
									RUL_RETURN,
									// Bit wise operators
									RUL_BIT_AND,
									RUL_BIT_OR,
									RUL_BIT_XOR,
									RUL_BIT_NOT,
									RUL_BIT_RSHIFT,
									RUL_BIT_LSHIFT,
									// Function
									RUL_FUNCTION,
									// DD (Global) Identifier
									RUL_DD_SIMPLE,//Anil August 26 2005 For handling DD variable
									RUL_DD_COMPLEX,//Anil August 26 2005 For handling DD variable
									RUL_DD_METHOD//Anil Octobet 5 2005 for handling Method Calling Method
								};

enum STATEMENT_TYPE				{
									STMT_DECL=0,
									STMT_ASSIGNMENT,
									STMT_SELECTION,
									STMT_ELSE,
									STMT_ITERATION,
									STMT_COMPOUND,
									STMT_OPTIONAL,
									STMT_EXPRESSION,
									STMT_SERVICE,
									STMT_RUL_INVOKE,
									STMT_asic,
									STMT_ASSIGNMENT_FOR,
									STMT_BREAK,
									STMT_CONTINUE,
									STMT_RETURN,
									STMT_CASE
								};


enum STMT_EXPR_TYPE				{
									EXPR_NONE=0,
									EXPR_ASSIGN,
									EXPR_IF,
									EXPR_WHILE,
									EXPR_FOR,
									EXPR_LVALUE,
									EXPR_CASE
								};

extern _CHAR*	szTokenStrings[];
extern _CHAR*	szTokenSubstrings[];

struct DFA_State
{
	const _CHAR*		szWord;
	RUL_TOKEN_TYPE		Type;
	RUL_TOKEN_SUBTYPE	SubType;
};

const DFA_State State[]	= {	
							{"//",RUL_COMMENT,RUL_SUBTYPE_NONE},

							{"+=",RUL_ASSIGNMENT_OPERATOR,RUL_PLUS_ASSIGN},
							{"-=",RUL_ASSIGNMENT_OPERATOR,RUL_MINUS_ASSIGN},
							{"/=",RUL_ASSIGNMENT_OPERATOR,RUL_DIV_ASSIGN},
							{"%=",RUL_ASSIGNMENT_OPERATOR,RUL_MOD_ASSIGN},
							{"*=",RUL_ASSIGNMENT_OPERATOR,RUL_MUL_ASSIGN},
							{"!=",RUL_RELATIONAL_OPERATOR,RUL_NOT_EQ},

							{"&=",RUL_ASSIGNMENT_OPERATOR,RUL_BIT_AND_ASSIGN},
							{"|=",RUL_ASSIGNMENT_OPERATOR,RUL_BIT_OR_ASSIGN},
							{"^=",RUL_ASSIGNMENT_OPERATOR,RUL_BIT_XOR_ASSIGN},
							{">>=",RUL_ASSIGNMENT_OPERATOR,RUL_BIT_RSHIFT_ASSIGN},
							{"<<=",RUL_ASSIGNMENT_OPERATOR,RUL_BIT_LSHIFT_ASSIGN},

							{"++",RUL_ARITHMETIC_OPERATOR,RUL_PLUS_PLUS},
							{"--",RUL_ARITHMETIC_OPERATOR,RUL_MINUS_MINUS},

							{"+",RUL_ARITHMETIC_OPERATOR,RUL_PLUS},
							{"-",RUL_ARITHMETIC_OPERATOR,RUL_MINUS},
							{"*",RUL_ARITHMETIC_OPERATOR,RUL_MUL},
							{"/",RUL_ARITHMETIC_OPERATOR,RUL_DIV},
							{"%",RUL_ARITHMETIC_OPERATOR,RUL_MOD},

							{"&&",RUL_LOGICAL_OPERATOR,RUL_LOGIC_AND},
							{"||",RUL_LOGICAL_OPERATOR,RUL_LOGIC_OR},
							{"!",RUL_LOGICAL_OPERATOR,RUL_LOGIC_NOT},

							{"&",RUL_ARITHMETIC_OPERATOR,RUL_BIT_AND},
							{"|",RUL_ARITHMETIC_OPERATOR,RUL_BIT_OR},
							{"^",RUL_ARITHMETIC_OPERATOR,RUL_BIT_XOR},
							{"~",RUL_ARITHMETIC_OPERATOR,RUL_BIT_NOT},
							{">>",RUL_ARITHMETIC_OPERATOR,RUL_BIT_RSHIFT},
							{"<<",RUL_ARITHMETIC_OPERATOR,RUL_BIT_LSHIFT},
//							{"!",RUL_ARITHMETIC_OPERATOR,RUL_BIT_NOT},

							{"<=",RUL_RELATIONAL_OPERATOR,RUL_LE},
							{">=",RUL_RELATIONAL_OPERATOR,RUL_GE},
//							{"**",RUL_ARITHMETIC_OPERATOR,RUL_EXP},
//							{"<>",RUL_RELATIONAL_OPERATOR,RUL_NOT_EQ},
							{"<",RUL_RELATIONAL_OPERATOR,RUL_LT},
							{">",RUL_RELATIONAL_OPERATOR,RUL_GT},
							{"==",RUL_RELATIONAL_OPERATOR,RUL_EQ},

							{"=",RUL_ASSIGNMENT_OPERATOR,RUL_ASSIGN},
						
							{"{",RUL_SYMBOL,RUL_LBRACK},
							{"}",RUL_SYMBOL,RUL_RBRACK},
							{"(",RUL_SYMBOL,RUL_LPAREN},
							{")",RUL_SYMBOL,RUL_RPAREN},
							{"[",RUL_SYMBOL,RUL_LBOX},
							{"]",RUL_SYMBOL,RUL_RBOX},
							{";",RUL_SYMBOL,RUL_SEMICOLON},
							{":",RUL_SYMBOL,RUL_COLON},
							{",",RUL_SYMBOL,RUL_COMMA},
							{".",RUL_SYMBOL,RUL_DOT},
							{"?",RUL_SYMBOL,RUL_QMARK},
//							{"::",RUL_SYMBOL,RUL_SCOPE},
							
							{"if",RUL_KEYWORD,RUL_IF},
							{"else",RUL_KEYWORD,RUL_ELSE},
							{"switch",RUL_KEYWORD,RUL_SWITCH},
							{"case",RUL_KEYWORD,RUL_CASE},
							{"default",RUL_KEYWORD,RUL_DEFAULT},
							{"while",RUL_KEYWORD,RUL_WHILE},
							{"for",RUL_KEYWORD,RUL_FOR},
							{"do",RUL_KEYWORD,RUL_DO},
							{"unsigned int",RUL_KEYWORD,RUL_UNSIGNED_INTEGER_DECL},
							{"int",RUL_KEYWORD,RUL_INTEGER_DECL},
							{"unsigned long long",RUL_KEYWORD,RUL_LONG_LONG_DECL},
							{"long long",RUL_KEYWORD,RUL_LONG_LONG_DECL},
							{"unsigned long",RUL_KEYWORD,RUL_UNSIGNED_INTEGER_DECL},
							{"long",RUL_KEYWORD,RUL_LONG_DECL},
							{"unsigned short",RUL_KEYWORD,RUL_UNSIGNED_SHORT_INTEGER_DECL},
							{"short",RUL_KEYWORD,RUL_SHORT_INTEGER_DECL},
							{"float",RUL_KEYWORD,RUL_REAL_DECL},
							{"double",RUL_KEYWORD,RUL_DOUBLE_DECL},
							{"char",RUL_KEYWORD,RUL_CHAR_DECL},
							{"break",RUL_KEYWORD,RUL_BREAK},
							{"continue",RUL_KEYWORD,RUL_CONTINUE},
							{"return",RUL_KEYWORD,RUL_RETURN},
							//Added By Anil June 14 2005 --starts here
							{"DD_STRING",RUL_KEYWORD,RUL_DD_STRING_DECL},
							//Added By Anil June 14 2005 --Ends here

							{"unsigned char",RUL_KEYWORD,RUL_UNSIGNED_CHAR_DECL},

//							{"bool",RUL_KEYWORD,RUL_BOOLEAN_DECL},
//							{"true",RUL_KEYWORD,RUL_BOOL_CONSTANT},
//							{"false",RUL_KEYWORD,RUL_BOOL_CONSTANT},
//							{"string",RUL_KEYWORD,RUL_STRING_DECL},
//							{"Invoke",RUL_KEYWORD,RUL_INVOKE},
//							{"RuleEngine",RUL_KEYWORD,RUL_RULE_ENGINE}//,
//							{"ObjectManager",RUL_KEYWORD,RUL_OM}
/* Start of Function signatures */
							{"DELAY_TIME",RUL_KEYWORD,RUL_FUNCTION},
							{"DELAY",RUL_KEYWORD,RUL_FUNCTION},
							{"delay",RUL_KEYWORD,RUL_FUNCTION},

							{"process_abort",RUL_KEYWORD,RUL_FUNCTION},
							{"_add_abort_method",RUL_KEYWORD,RUL_FUNCTION},
							{"_remove_abort_method",RUL_KEYWORD,RUL_FUNCTION},
							{"remove_all_abort",RUL_KEYWORD,RUL_FUNCTION},
							{"abort",RUL_KEYWORD,RUL_FUNCTION},
							/*Arun 190505 Start of code*/
							{"_push_abort_method",RUL_KEYWORD,RUL_FUNCTION},/*stevev4waltS - 11oct07*/
							{"pop_abort_method",RUL_KEYWORD,RUL_FUNCTION},

							/*End of code*/
							{"remove_all_abort_methods",RUL_KEYWORD,RUL_FUNCTION},							

							/*Arun 200505 Start of code*/
							{"BUILD_MESSAGE",RUL_KEYWORD,RUL_FUNCTION},
							/*End of code*/

							{"PUT_MESSAGE",RUL_KEYWORD,RUL_FUNCTION},
							{"put_message",RUL_KEYWORD,RUL_FUNCTION},
							{"ACKNOWLEDGE",RUL_KEYWORD,RUL_FUNCTION},
							{"acknowledge",RUL_KEYWORD,RUL_FUNCTION},
							{"_get_dev_var_value",RUL_KEYWORD,RUL_FUNCTION},
							{"_get_local_var_value",RUL_KEYWORD,RUL_FUNCTION},
							{"_display_xmtr_status",RUL_KEYWORD,RUL_FUNCTION},
							{"display_response_status",RUL_KEYWORD,RUL_FUNCTION},
							{"display",RUL_KEYWORD,RUL_FUNCTION},
							{"SELECT_FROM_LIST",RUL_KEYWORD,RUL_FUNCTION},
							{"select_from_list",RUL_KEYWORD,RUL_FUNCTION},

							{"_vassign",RUL_KEYWORD,RUL_FUNCTION},
							{"_dassign",RUL_KEYWORD,RUL_FUNCTION},
							{"_fassign",RUL_KEYWORD,RUL_FUNCTION},
							{"_iassign",RUL_KEYWORD,RUL_FUNCTION},
							{"_lassign",RUL_KEYWORD,RUL_FUNCTION},
							{"_fvar_value",RUL_KEYWORD,RUL_FUNCTION},
							{"_ivar_value",RUL_KEYWORD,RUL_FUNCTION},
							{"_lvar_value",RUL_KEYWORD,RUL_FUNCTION},
							{"save_values",RUL_KEYWORD,RUL_FUNCTION},
							//Added By Anil July 01 2005 --starts here
							{"discard_on_exit",RUL_KEYWORD,RUL_FUNCTION},
							//Added By Anil July 01 2005 --Ends here
							//Added By Anil June 20 2005 --starts here
							{"svar_value",RUL_KEYWORD,RUL_FUNCTION},
							{"sassign",RUL_KEYWORD,RUL_FUNCTION},
							//Added By Anil June 20 2005 --Ends here


							{"get_more_status",RUL_KEYWORD,RUL_FUNCTION},
							{"_get_status_code_string",RUL_KEYWORD,RUL_FUNCTION},
							/*Arun 200505 Start of code*/
							// stevev - made it '_get_enum..' from 'get_enum..' 25jul07-sjv
							{"_get_enum_string",RUL_KEYWORD,RUL_FUNCTION},
							/*End of code*/

							{"_get_dictionary_string",RUL_KEYWORD,RUL_FUNCTION},
							//Anil 22 December 2005 for dictionary_string built in
							{"_dictionary_string",RUL_KEYWORD,RUL_FUNCTION},
							//stevev 29jan08
							{"literal_string",RUL_KEYWORD,RUL_FUNCTION},
							{"resolve_param_ref",RUL_KEYWORD,RUL_FUNCTION},
							{"resolve_array_ref",RUL_KEYWORD,RUL_FUNCTION},
							{"resolve_record_ref",RUL_KEYWORD,RUL_FUNCTION},
							{"resolve_local_ref",RUL_KEYWORD,RUL_FUNCTION},
							{"rspcode_string",RUL_KEYWORD,RUL_FUNCTION},

							{"_set_comm_status",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_device_status",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_resp_code",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_all_resp_code",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_no_device",RUL_KEYWORD,RUL_FUNCTION},
							{"SET_NUMBER_OF_RETRIES",RUL_KEYWORD,RUL_FUNCTION},

							{"_set_xmtr_comm_status",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_xmtr_device_status",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_xmtr_resp_code",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_xmtr_all_resp_code",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_xmtr_no_device",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_xmtr_all_data",RUL_KEYWORD,RUL_FUNCTION},
							{"_set_xmtr_data",RUL_KEYWORD,RUL_FUNCTION},

							{"NaN_value",RUL_KEYWORD,RUL_FUNCTION},
							{"fpclassify",RUL_KEYWORD,RUL_FUNCTION},
							{"nanf",RUL_KEYWORD,RUL_FUNCTION},
						{"nan",RUL_KEYWORD,RUL_FUNCTION},/*stevev added 25jun07*/

							

							{"isetval",RUL_KEYWORD,RUL_FUNCTION},
							{"fsetval",RUL_KEYWORD,RUL_FUNCTION},
							{"lsetval",RUL_KEYWORD,RUL_FUNCTION},
							{"dsetval",RUL_KEYWORD,RUL_FUNCTION},
							{"igetvalue",RUL_KEYWORD,RUL_FUNCTION},
							{"igetval",RUL_KEYWORD,RUL_FUNCTION},
							{"fgetval",RUL_KEYWORD,RUL_FUNCTION},
							{"lgetval",RUL_KEYWORD,RUL_FUNCTION},
							{"dgetval",RUL_KEYWORD,RUL_FUNCTION},
							/*Arun 200505 Start of code*/
							{"sgetval",RUL_KEYWORD,RUL_FUNCTION},
							{"ssetval",RUL_KEYWORD,RUL_FUNCTION},
							/*End of code*/

							{"send_command_trans",RUL_KEYWORD,RUL_FUNCTION},
							{"send_command",RUL_KEYWORD,RUL_FUNCTION},
							{"ext_send_command_trans",RUL_KEYWORD,RUL_FUNCTION},
							{"ext_send_command",RUL_KEYWORD,RUL_FUNCTION},
							{"tsend_command_trans",RUL_KEYWORD,RUL_FUNCTION},
							{"tsend_command",RUL_KEYWORD,RUL_FUNCTION},
							{"send_trans",RUL_KEYWORD,RUL_FUNCTION},
							{"send",RUL_KEYWORD,RUL_FUNCTION},

							{"process_abort",RUL_KEYWORD,RUL_FUNCTION},
							
							/*Arun 110505 Start of code*/
/***********************Math Builtins (eDDL)**********************/
							{"abs",RUL_KEYWORD,RUL_FUNCTION},
							{"acos",RUL_KEYWORD,RUL_FUNCTION},
							{"asin",RUL_KEYWORD,RUL_FUNCTION},
							{"atan",RUL_KEYWORD,RUL_FUNCTION},
							{"cbrt",RUL_KEYWORD,RUL_FUNCTION},
							{"ceil",RUL_KEYWORD,RUL_FUNCTION},
							{"cos",RUL_KEYWORD,RUL_FUNCTION},
							{"cosh",RUL_KEYWORD,RUL_FUNCTION},
							{"exp",RUL_KEYWORD,RUL_FUNCTION},
							{"floor",RUL_KEYWORD,RUL_FUNCTION},
							{"fmod",RUL_KEYWORD,RUL_FUNCTION},
#ifdef XMTR
							{"frand",RUL_KEYWORD,RUL_FUNCTION},
#endif
							{"log",RUL_KEYWORD,RUL_FUNCTION},
							{"log10",RUL_KEYWORD,RUL_FUNCTION},
							{"log2",RUL_KEYWORD,RUL_FUNCTION},
							{"pow",RUL_KEYWORD,RUL_FUNCTION},
							{"round",RUL_KEYWORD,RUL_FUNCTION},
							{"sin",RUL_KEYWORD,RUL_FUNCTION},
							{"sinh",RUL_KEYWORD,RUL_FUNCTION},
							{"sqrt",RUL_KEYWORD,RUL_FUNCTION},
							{"tan",RUL_KEYWORD,RUL_FUNCTION},	/* inserted 14feb07 stevev */
							{"tanh",RUL_KEYWORD,RUL_FUNCTION},
							{"trunc",RUL_KEYWORD,RUL_FUNCTION},
							{"atof",RUL_KEYWORD,RUL_FUNCTION},
							{"atoi",RUL_KEYWORD,RUL_FUNCTION},
							{"itoa",RUL_KEYWORD,RUL_FUNCTION},
/**********************End of Math Builtins (eDDL)***************/

/*End of code*/

/*Arun 160505 Start of code */
/***********************Date Time Builtins (eDDL)*******************/
//							{"YearMonthDay_to_Date",RUL_KEYWORD,RUL_FUNCTION},-WS:EPM Not a builtin-25jun07
							{"Date_to_Year",RUL_KEYWORD,RUL_FUNCTION},
							{"Date_to_Month",RUL_KEYWORD,RUL_FUNCTION},
							{"Date_to_DayOfMonth",RUL_KEYWORD,RUL_FUNCTION},
//							{"GetCurrentDate",RUL_KEYWORD,RUL_FUNCTION},-WS:EPM Not a builtin-25jun07
							{"GetCurrentTime",RUL_KEYWORD,RUL_FUNCTION},
//							{"GetCurrentDateAndTime",RUL_KEYWORD,RUL_FUNCTION},-WS:EPM Not a builtin-25jun07
//							{"To_Date_and_Time",RUL_KEYWORD,RUL_FUNCTION},-WS:EPM Not a builtin-25jun07

							{"DiffTime",RUL_KEYWORD,RUL_FUNCTION},
							{"AddTime",RUL_KEYWORD,RUL_FUNCTION},
							{"Make_Time",RUL_KEYWORD,RUL_FUNCTION},
							{"To_Time",RUL_KEYWORD,RUL_FUNCTION},
							{"Date_To_Time",RUL_KEYWORD,RUL_FUNCTION},
							{"To_Date",RUL_KEYWORD,RUL_FUNCTION},
							{"Time_To_Date",RUL_KEYWORD,RUL_FUNCTION},


/**********************End of Date Time Builtins (eDDL)************/

							
//Added By Anil June 17 2005 --starts here
/****************************Start of DD_STRING  Builtins  (eDDL) ********************/

							{"strstr",RUL_KEYWORD,RUL_FUNCTION},//Anil changed to lower case 29th November 2005
							{"strupr",RUL_KEYWORD,RUL_FUNCTION},//Anil changed to lower case 29th November 2005
							{"strlwr",RUL_KEYWORD,RUL_FUNCTION},//Anil changed to lower case 29th November 2005
							{"strlen",RUL_KEYWORD,RUL_FUNCTION},//Anil changed to lower case 29th November 2005
							{"strcmp",RUL_KEYWORD,RUL_FUNCTION},//Anil changed to lower case 29th November 2005
							{"strtrim",RUL_KEYWORD,RUL_FUNCTION},//Anil changed to lower case 29th November 2005
							{"strmid",RUL_KEYWORD,RUL_FUNCTION},//Anil changed to lower case 29th November 2005

/****************************End of DD_STRING  Builtins (eDDL) ********************/
//Added By Anil June 17 2005 --Ends here
/* End of code */

/*Vibhor 200905: Start of Code*/
/****************************Start List Manipulation Builtins (eDDL) ********************/
							
							{"_ListInsert",RUL_KEYWORD,RUL_FUNCTION},
							{"_ListDeleteElementAt",RUL_KEYWORD,RUL_FUNCTION},


/****************************End List Manipulation Builtins (eDDL) ********************/
/*Vibhor 200905: End of Code*/
//Anil September 26 2005 added MenuDisplay
							{"_MenuDisplay",RUL_KEYWORD,RUL_FUNCTION},
// stevev 18feb09 - add transfer functions
							{"openTransferPort",RUL_KEYWORD,RUL_FUNCTION}
							,{"closeTransferPort",RUL_KEYWORD,RUL_FUNCTION}
							,{"abortTransferPort",RUL_KEYWORD,RUL_FUNCTION}
							,{"_writeItemToDevice",RUL_KEYWORD,RUL_FUNCTION}
							,{"_readItemFromDevice",RUL_KEYWORD,RUL_FUNCTION}
							,{"get_transfer_status",RUL_KEYWORD,RUL_FUNCTION}


/* End of Function signatures */
						};

const DFA_State OM_Service[]	= {	{"Service",RUL_SERVICE,RUL_SERVICE_ATTRIBUTE}
								 };

struct Function_Signatures
{
	const _CHAR*		szWord;
	RUL_TOKEN_TYPE		functionReturnType;
	_UINT32				iNumberOfParameters;
	RUL_TOKEN_TYPE		piParameterType[MAX_NUMBER_OF_FUNCTION_PARAMETERS];	
};

const Function_Signatures functionsDefs[]=
{
	{"delay", RUL_SIMPLE_VARIABLE, 3, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"DELAY", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT}
	, {"DELAY_TIME", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}

	, {"abort", RUL_SIMPLE_VARIABLE, 0}
	, {"process_abort", RUL_SIMPLE_VARIABLE, 0}
	, {"_add_abort_method", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"_remove_abort_method", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"remove_all_abort", RUL_SIMPLE_VARIABLE, 0}
	/*Arun 190505 Start of code */
	, {"_push_abort_method", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}/*stevev4waltS - 11oct07*/
	, {"pop_abort_method", RUL_SIMPLE_VARIABLE, 0}
	/*End of code */
	/*Arun 200505 Start of code*/
	, {"BUILD_MESSAGE", RUL_STR_CONSTANT, 1, RUL_STR_CONSTANT } // modified 30may07 per WS:EPM
	/*End of code*/

	, {"remove_all_abort_methods", RUL_SIMPLE_VARIABLE, 0}


	, {"PUT_MESSAGE", RUL_SIMPLE_VARIABLE, 1, RUL_STR_CONSTANT}
	, {"put_message", RUL_SIMPLE_VARIABLE, 2, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"ACKNOWLEDGE", RUL_SIMPLE_VARIABLE, 1, RUL_STR_CONSTANT}
	, {"acknowledge", RUL_SIMPLE_VARIABLE, 2, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_get_dev_var_value", RUL_SIMPLE_VARIABLE, 3, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_get_local_var_value", RUL_SIMPLE_VARIABLE, 3, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT}
	, {"_display_xmtr_status", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"display_response_status", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"display", RUL_SIMPLE_VARIABLE, 2, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"SELECT_FROM_LIST", RUL_SIMPLE_VARIABLE, 2, RUL_STR_CONSTANT, RUL_STR_CONSTANT}
	, {"select_from_list", RUL_SIMPLE_VARIABLE, 3, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT,RUL_STR_CONSTANT}

	, {"_vassign", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_dassign", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_fassign", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_iassign", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_lassign", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_fvar_value", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"_ivar_value", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"_lvar_value", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"save_values", RUL_SIMPLE_VARIABLE, 0}
	//Added By Anil July 01 2005 --starts here
	, {"discard_on_exit", RUL_SIMPLE_VARIABLE, 0}
	//Added By Anil July 01 2005 --Ends here
	//Added By Anil June 20 2005 --starts here
	, {"svar_value", RUL_STR_CONSTANT, 1, RUL_SIMPLE_VARIABLE}
	, {"sassign", RUL_SIMPLE_VARIABLE, 2, RUL_SIMPLE_VARIABLE,RUL_STR_CONSTANT}	

	//Added By Anil June 20 2005 --Ends here

	, {"get_more_status", RUL_SIMPLE_VARIABLE, 2, RUL_STR_CONSTANT, RUL_STR_CONSTANT}
	, {"_get_status_code_string", RUL_SIMPLE_VARIABLE, 4, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT}
	/*Arun 200505 Start of code*/
	//WHS 2007 get_enum_string takes 3 arguments not 4.
	// stevev - made it '_get_enum..' from 'get_enum..' 25jul07-sjv
	, {"_get_enum_string", RUL_SIMPLE_VARIABLE, 3, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT}
	/*End of code*/


	, {"_get_dictionary_string", RUL_SIMPLE_VARIABLE, 3, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT}
	//Anil 22 December 2005 for dictionary_string built in
	, {"_dictionary_string", RUL_STR_CONSTANT, 1, RUL_NUMERIC_CONSTANT}
	//stevev 29dec08
	, {"literal_string", RUL_STR_CONSTANT, 1, RUL_NUMERIC_CONSTANT}
	, {"resolve_param_ref", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"resolve_array_ref", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"resolve_record_ref", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"resolve_local_ref", RUL_SIMPLE_VARIABLE, 1, RUL_DD_ITEM}/* stevev 19feb09 - try DD_Item */
	, {"rspcode_string", RUL_SIMPLE_VARIABLE, 4, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT}

	, {"_set_comm_status", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_set_device_status", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_set_resp_code", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_set_all_resp_code", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"_set_no_device", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"SET_NUMBER_OF_RETRIES", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}

	, {"_set_xmtr_comm_status", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_set_xmtr_device_status", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_set_xmtr_resp_code", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"_set_xmtr_all_resp_code", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"_set_xmtr_no_device", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"_set_xmtr_all_data", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"_set_xmtr_data", RUL_SIMPLE_VARIABLE, 3, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}

	, {"NaN_value", RUL_SIMPLE_VARIABLE, 0}
	, {"fpclassify", RUL_SIMPLE_VARIABLE, 1, RUL_SIMPLE_VARIABLE}
	, {"nanf", RUL_SIMPLE_VARIABLE, 1, RUL_STR_CONSTANT}
	, {"nan",  RUL_SIMPLE_VARIABLE, 1, RUL_STR_CONSTANT} /*stevev added 25jun07*/


	, {"isetval", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"fsetval", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"lsetval", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"dsetval", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"igetvalue", RUL_SIMPLE_VARIABLE, 0}
	, {"igetval", RUL_SIMPLE_VARIABLE, 0}
	, {"fgetval", RUL_SIMPLE_VARIABLE, 0}
	, {"lgetval", RUL_SIMPLE_VARIABLE, 0}
	, {"dgetval", RUL_SIMPLE_VARIABLE, 0}
	/*Arun 200505 Start of code*/
	, {"sgetval", RUL_SIMPLE_VARIABLE, 2,RUL_STR_CONSTANT,RUL_NUMERIC_CONSTANT}
	, {"ssetval", RUL_SIMPLE_VARIABLE, 1,RUL_STR_CONSTANT}
	/*End of code*/


	, {"ext_send_command_trans", RUL_SIMPLE_VARIABLE, 5, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT, RUL_STR_CONSTANT, RUL_STR_CONSTANT}
	, {"ext_send_command", RUL_SIMPLE_VARIABLE, 4, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT, RUL_STR_CONSTANT, RUL_STR_CONSTANT}
	, {"tsend_command_trans", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"tsend_command", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"send_command_trans", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"send_command", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"send_trans", RUL_SIMPLE_VARIABLE, 3, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT}
	, {"send", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT}

/*Arun 110505 Start of code*/
/*********************Math Builtins (eDDL) ***********************/
	, {"abs", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"acos", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"asin", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"atan", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"cbrt", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"ceil", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"cos", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"cosh", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"exp", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"floor", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"fmod", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"frand", RUL_SIMPLE_VARIABLE, 0, RUL_NUMERIC_CONSTANT}
	, {"log", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"log10", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"log2", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"pow", RUL_SIMPLE_VARIABLE, 2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	, {"round", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"sin", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"sinh", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"sqrt", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"tan", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"tanh", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"trunc", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"atof", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"atoi", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"itoa", RUL_SIMPLE_VARIABLE, 3, RUL_NUMERIC_CONSTANT, RUL_STR_CONSTANT, RUL_NUMERIC_CONSTANT}
/*********************End of Math Builtins (eDDL)******************/
/* End of code*/

/* Arun 160505 Start of code*/
/*********************Date Time Builtins(eDDL)********************/
//	, {"YearMonthDay_to_Date", RUL_SIMPLE_VARIABLE, 3, RUL_NUMERIC_CONSTANT}/* not in spec */-WS:EPM removed-25jun07
	, {"Date_to_Year", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"Date_to_Month", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
	, {"Date_to_DayOfMonth", RUL_SIMPLE_VARIABLE, 1, RUL_NUMERIC_CONSTANT}
//	, {"GetCurrentDate", RUL_SIMPLE_VARIABLE, 0}							/* not in spec */-WS:EPM removed-25jun07
	, {"GetCurrentTime", RUL_SIMPLE_VARIABLE, 0}
//	, {"GetCurrentDateAndTime", RUL_SIMPLE_VARIABLE,0}						/* not in spec */-WS:EPM removed-25jun07
//	, {"To_Date_and_Time", RUL_SIMPLE_VARIABLE, 5, RUL_NUMERIC_CONSTANT}	/* not in spec */-WS:EPM removed-25jun07
	//Anil added these Time related function 28t November
	, {"DiffTime", RUL_SIMPLE_VARIABLE, 2, RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE}
	, {"AddTime", RUL_SIMPLE_VARIABLE, 2, RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE}
	, {"Make_Time", RUL_SIMPLE_VARIABLE, 7, RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE}
	, {"To_Time", RUL_SIMPLE_VARIABLE,5, RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE}
	, {"Date_To_Time", RUL_SIMPLE_VARIABLE, 1, RUL_SIMPLE_VARIABLE}
	, {"To_Date", RUL_SIMPLE_VARIABLE, 3, RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE}	
	, {"Time_To_Date", RUL_SIMPLE_VARIABLE, 1, RUL_SIMPLE_VARIABLE}




/*********************End of Date Time Builtins (eDDL) **************/

//Added By Anil June 17 2005 --starts here
/****************************Start of DD_STRING  Builtins  (eDDL) ********************/

	, {"strstr",	RUL_STR_CONSTANT,		2, RUL_STR_CONSTANT,RUL_STR_CONSTANT}//Anil changed to lower case 29th November 2005
	, {"strupr",	RUL_STR_CONSTANT,		1, RUL_STR_CONSTANT}//Anil changed to lower case 29th November 2005
	, {"strlwr",	RUL_STR_CONSTANT,		1, RUL_STR_CONSTANT}//Anil changed to lower case 29th November 2005
	, {"strlen",	RUL_SIMPLE_VARIABLE,	1, RUL_STR_CONSTANT}//Anil changed to lower case 29th November 2005
	, {"strcmp",	RUL_SIMPLE_VARIABLE,	2, RUL_STR_CONSTANT,RUL_STR_CONSTANT}//Anil changed to lower case 29th November 2005
	, {"strtrim",	RUL_STR_CONSTANT,		1, RUL_STR_CONSTANT}//Anil changed to lower case 29th November 2005
	, {"strmid",	RUL_STR_CONSTANT,		3, RUL_STR_CONSTANT,RUL_SIMPLE_VARIABLE,RUL_SIMPLE_VARIABLE}//Anil changed to lower case 29th November 2005
	

/****************************End of DD_STRING  Builtins (eDDL) ********************/
//Added By Anil June 17 2005 --Ends here

/*End of code*/

/*Vibhor 200905: Start of Code*/
/****************************Start List Manipulation Builtins (eDDL) ********************/

	, {"_ListInsert",			RUL_SIMPLE_VARIABLE,	3, RUL_NUMERIC_CONSTANT,RUL_NUMERIC_CONSTANT,RUL_NUMERIC_CONSTANT}
	, {"_ListDeleteElementAt",  RUL_SIMPLE_VARIABLE,	2, RUL_NUMERIC_CONSTANT,RUL_NUMERIC_CONSTANT}

/****************************End List Manipulation Builtins (eDDL) ********************/

/*Vibhor 200905: End of Code*/
//Anil September 26 2005 added Menudisplay
	,{"_MenuDisplay",RUL_SIMPLE_VARIABLE,3,RUL_NUMERIC_CONSTANT,RUL_STR_CONSTANT,RUL_STR_CONSTANT}
/* stevev 18feb09 - add transfer builtins */
	,{"openTransferPort"	,RUL_SIMPLE_VARIABLE,1, RUL_NUMERIC_CONSTANT}
	,{"closeTransferPort"	,RUL_SIMPLE_VARIABLE,1, RUL_NUMERIC_CONSTANT}
	,{"abortTransferPort"	,RUL_SIMPLE_VARIABLE,1, RUL_NUMERIC_CONSTANT}
	,{"_writeItemToDevice"	,RUL_SIMPLE_VARIABLE,2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	,{"_readItemFromDevice"	,RUL_SIMPLE_VARIABLE,2, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
	,{"get_transfer_status"	,RUL_SIMPLE_VARIABLE,3, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT, RUL_NUMERIC_CONSTANT}
/* end stevev transfer builtins */
};
enum DD_ITEM_TYPE
{
	DD_ITEM_VAR = 0,
	DD_ITEM_NONVAR = 1,
	DD_ITEM_METHOD = 2

};

enum DD_METH_AGR_PASSED_TYPE
{
	 DD_METH_AGR_PASSED_UNKNOWN			= 0
	,DD_METH_AGR_PASSED_BYVALUE			= 1
	,DD_METH_AGR_PASSED_BYREFERENCE		= 2
};

//This calss is used when Method is calling other method
//Arg info are filled in this class 
class METHOD_ARG_INFO
{
protected:		// changed from private: WS:EPM 17jul07
	char*							m_pchCalledArgName;
	char*							m_pchCallerArgName;	
	RUL_TOKEN_TYPE					m_Type;
	RUL_TOKEN_SUBTYPE				m_SubType;
	
public:
	bool m_IsReturnVar ;
	enum DD_METH_AGR_PASSED_TYPE    ePassedType;
	long unsigned int				ulDDItemId;
	
	METHOD_ARG_INFO()
	{
		m_pchCalledArgName =  NULL;
		m_pchCallerArgName = NULL;
		ePassedType   =  DD_METH_AGR_PASSED_UNKNOWN;
		m_Type = RUL_TYPE_NONE;
		m_SubType = RUL_SUBTYPE_NONE;
		ulDDItemId = 0;
		m_IsReturnVar = false;
	}

	METHOD_ARG_INFO( const METHOD_ARG_INFO& methodArguments )
	{ 	operator=(methodArguments);
	}

	virtual ~METHOD_ARG_INFO()
	{
		if(m_pchCalledArgName)
		{
			delete[] m_pchCalledArgName;
			m_pchCalledArgName = NULL;
		}
		if(m_pchCallerArgName)
		{
			delete[] m_pchCallerArgName;
			m_pchCallerArgName = NULL;
		}
	}
	METHOD_ARG_INFO& operator =( const METHOD_ARG_INFO& methodArguments )
	{
		m_Type = methodArguments.m_Type;
		m_SubType = methodArguments.m_SubType;
		m_pchCalledArgName = NULL;
		m_pchCallerArgName = NULL;
		if( methodArguments.m_pchCallerArgName )
		{
			SetCallerArgName(methodArguments.m_pchCallerArgName);
		}
		if( methodArguments.m_pchCalledArgName )
		{
			SetCalledArgName(methodArguments.m_pchCalledArgName);
		}
		m_IsReturnVar = methodArguments.m_IsReturnVar;
		ePassedType = methodArguments.ePassedType;
		ulDDItemId = methodArguments.ulDDItemId;
		return *this;
	}
public:
	void SetCalledArgName(const char* pCalledArgName)
	{
		if(m_pchCalledArgName)
		{
			delete[] m_pchCalledArgName;
			m_pchCalledArgName = NULL;
		}
#ifdef MODIFIED_BY_SS
		////Assgn the vlues
		//m_pchCalledArgName = new char[ strlen(pCalledArgName) + 1];
		//memset(m_pchCalledArgName,0,strlen(pCalledArgName) + 1);
		//strcpy(m_pchCalledArgName,pCalledArgName);

		size_t length = strlen(pCalledArgName);
		m_pchCalledArgName = new char[length + 1];
		std::copy(pCalledArgName, pCalledArgName + length, m_pchCalledArgName);
		m_pchCalledArgName[length] = '\0';
#endif
	}
	void SetCallerArgName(const char* pCallerArgName)
	{
		//Fille the Caller Arg name also
		
		if(m_pchCallerArgName)
		{
			delete[] m_pchCallerArgName;
			m_pchCallerArgName = NULL;
		}
#ifdef MODIFIED_BY_SS
		/*m_pchCallerArgName = new char[ strlen(pCallerArgName) + 1];
		memset(m_pchCallerArgName,0,strlen(pCallerArgName) + 1);
		strcpy(m_pchCallerArgName,pCallerArgName)*/
		size_t length = strlen(pCallerArgName);
		m_pchCallerArgName = new char[length + 1];
		std::copy(pCallerArgName, pCallerArgName + length, m_pchCallerArgName);
		m_pchCallerArgName[length] = '\0';
#endif

	}
	void SetType(RUL_TOKEN_TYPE eType)
	{
		m_Type = eType;
	}
	void SetSubType(RUL_TOKEN_SUBTYPE eSubType)
	{
		m_SubType = eSubType;
	}

	RUL_TOKEN_TYPE GetType()
	{
		return m_Type;
	}
	RUL_TOKEN_SUBTYPE GetSubType()
	{
		return  m_SubType;
	}

	const char*	GetCalledArgName()
	{
		return m_pchCalledArgName;

	}

	const char*	GetCallerArgName()
	{
		return m_pchCallerArgName;

	}

};
typedef vector<METHOD_ARG_INFO> METHOD_ARG_INFO_VECTOR;

#define EAT_SPACE				while(isSpace(pszSource,i,Type,SubType,pszBuffer));

#define DELETE_PTR(x)		\
	if(x)					\
	{						\
		delete x;			\
		x = 0;				\
	}						


#define DELETE_ARR(x)		\
	if(x)					\
	{						\
		delete[] x;			\
		x = 0;				\
	}	
					
#endif
