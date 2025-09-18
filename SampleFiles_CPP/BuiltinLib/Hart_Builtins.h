#ifndef HART_BUILTINS_H
#define HART_BUILTINS_H

# define BLTIN_SUCCESS 0
# define BLTIN_FAILURE 1

# define MAX_PROMPT_MSG_PARAMS 10 

#define BYTENUM(x)          ((int)((x)/8))
#define BITNUM(x)           ((x)-(BYTENUM(x)*8))

#define METHOD_ABORTED 0xf3
#define METHOD_ABORTED 0xf3

#define PUT_MESSAGE_SLEEP_TIME	2000

/*Vibhor 180204: Start of Code*/

#define DEFAULT_TRANSACTION_NUMBER -1 /*Vibhor : The default transaction Number is -1 , not 0 !!!*/

/*Vibhor 180204: End of Code*/


//#define _ANP_TEST

#include "Bi_Codes.h"
#include "ddbdefs.h"

#pragma warning (disable : 4786)
#include <map>
#include "../common/ddbGeneral.h"
class INTER_VARIANT;
class FunctionExpression;
//#include "ddbItemBase.h" //Vibhor 081204: Added
class hCVar;// extern

// Anil December 16 2005 deleted codes related to Plot builtins. Please refer the previous version

// stevev 10aug07 - added a bit-enum to indicate what and how dynamics should be handled
//		currently, only DISPLAY updates dynamics, ACKNOWLEDGE updates special (%#) indicated dynamic

/* stevev 23jun11 - for the host test - The up_UPDATE_SPEC_DYN has to be in place for at least
 * acknowledge(), select_from_list(), delay(), get_dev_var_value(), get_local_var_value(),
 * display(),  put_message() in order to be backwards compatible with the current library.
 * This is 100% of the builtins that use the var_ids array.
 * SELECT_FROM_LIST, BUILD_MESSAGE, DELAY(), PUT_MESSAGE, ACKNOWLEDGE do not use var_ids[]
 * These have all been changed to a macro: UPDATE_NORMAL 
 **/

typedef enum updatePermission_e
{
	up_DONOT_UPDATE,	// 0
	up_UPDATE_STD_DYN   = 1,	// eg in DISPLAY builtin, do all %{dyn} && %#ofDyn
	up_UPDATE_SPEC_DYN	= 2		// update special dynamics - all %#ofDyn
}								// next one should be 4
/*typedef*/ updatePermission_t;

#define UPDATE_ALL		(updatePermission_t)(up_UPDATE_STD_DYN | up_UPDATE_SPEC_DYN)
// remove the up_UPDATE_SPEC_DYN to stop support 4 the %[format]n being equal to %[format]{n}
#define UPDATE_NORMAL   (up_UPDATE_STD_DYN)  /* update specials but NOT regualr dynamics */

enum SDC_VAR_TYPE
{
	SDC_VAR_TYPE_UNKNOWN = 0
	, SDC_VAR_TYPE_CHAR
	, SDC_VAR_TYPE_INT
	, SDC_VAR_TYPE_LONG
	, SDC_VAR_TYPE_FLOAT
	, SDC_VAR_TYPE_DOUBLE
};

struct SDCVariant
{
	SDC_VAR_TYPE valueType;
	union
	{
		char	chValue;
		int		iValue;
		long	lValue;
		float	fValue;
		double	dValue;
	};
};

//Added By Anil June 27 2005 --starts here
//this implimentation is because after sme 120 Builtins  Compiler was cribing for the nesting of if else
//Any new built in added should be added at the End and corresponding Enum value is to be used
enum BUILTIN_NAME
{
	BUILTIN_delay							=0
	,	BUILTIN_DELAY						=1
	,	BUILTIN_DELAY_TIME					=2
	,	BUILTIN_BUILD_MESSAGE				=3
	,	BUILTIN_PUT_MESSAGE
	,	BUILTIN_put_message
	,	BUILTIN_ACKNOWLEDGE
	,	BUILTIN_acknowledge
	,	BUILTIN__get_dev_var_value
	,	BUILTIN__get_local_var_value
	,	BUILTIN__display_xmtr_status
	,	BUILTIN_display_response_status
	,	BUILTIN_display
	,	BUILTIN_SELECT_FROM_LIST
	,	BUILTIN_select_from_list
	,	BUILTIN__vassign
	,	BUILTIN__dassign
	,	BUILTIN__fassign
	,	BUILTIN__lassign
	,	BUILTIN__iassign
	,	BUILTIN__fvar_value
	,	BUILTIN__ivar_value
	,	BUILTIN__lvar_value
	,	BUILTIN_svar_value
	,	BUILTIN_sassign
	,	BUILTIN_save_values
	,	BUILTIN_get_more_status
	,	BUILTIN__get_status_code_string
	,	BUILTIN_get_enum_string
	,	BUILTIN__get_dictionary_string
	//Anil 22 December 2005 for dictionary_string built in
	,	BUILTIN__dictionary_string
	,	BUILTIN_resolve_array_ref
	,	BUILTIN_resolve_record_ref
	,	BUILTIN_resolve_param_ref
	,   BUILTIN_resolve_local_ref
	,	BUILTIN_rspcode_string
	,	BUILTIN__set_comm_status
	,	BUILTIN__set_device_status
	,	BUILTIN__set_resp_code
	,	BUILTIN__set_all_resp_code
	,	BUILTIN__set_no_device
	,	BUILTIN_SET_NUMBER_OF_RETRIES
	,	BUILTIN__set_xmtr_comm_status
	,	BUILTIN__set_xmtr_device_status
	,	BUILTIN__set_xmtr_resp_code
	,	BUILTIN__set_xmtr_all_resp_code
	,	BUILTIN__set_xmtr_no_device
	,	BUILTIN__set_xmtr_all_data
	,	BUILTIN__set_xmtr_data
	,	BUILTIN_abort
	,	BUILTIN_process_abort
	,	BUILTIN__add_abort_method
	,	BUILTIN__remove_abort_method
	,	BUILTIN_remove_all_abort
	,	BUILTIN_push_abort_method
	,	BUILTIN_pop_abort_method
	,	BUILTIN_NaN_value
	,	BUILTIN_isetval
	,	BUILTIN_lsetval
	,	BUILTIN_fsetval
	,	BUILTIN_dsetval
	,	BUILTIN_igetvalue
	,	BUILTIN_igetval
	,	BUILTIN_lgetval
	,	BUILTIN_fgetval
	,	BUILTIN_dgetval
	,	BUILTIN_sgetval
	,	BUILTIN_ssetval
	,	BUILTIN_send
	,	BUILTIN_send_command
	,	BUILTIN_send_command_trans
	,	BUILTIN_send_trans
	,	BUILTIN_ext_send_command
	,	BUILTIN_ext_send_command_trans
	,	BUILTIN_tsend_command
	,	BUILTIN_tsend_command_trans
	,	BUILTIN_abs
	,	BUILTIN_acos
	,	BUILTIN_asin
	,	BUILTIN_atan
	,	BUILTIN_cbrt
	,	BUILTIN_ceil
	,	BUILTIN_cos
	,	BUILTIN_cosh
	,	BUILTIN_exp
	,	BUILTIN_floor
	,	BUILTIN_fmod
#ifdef XMTR
	,	BUILTIN_frand
#endif
	,	BUILTIN_log
	,	BUILTIN_log10
	,	BUILTIN_log2
	,	BUILTIN_pow
	,	BUILTIN_round
	,	BUILTIN_sin
	,	BUILTIN_sinh
	,	BUILTIN_sqrt
	,	BUILTIN_tan
	,	BUILTIN_tanh
	,	BUILTIN_trunc
	,	BUILTIN_atof
	,	BUILTIN_atoi
	,	BUILTIN_itoa
	,	BUILTIN_YearMonthDay_to_Date
	,	BUILTIN_Date_to_Year
	,	BUILTIN_Date_to_Month
	,	BUILTIN_Date_to_DayOfMonth
	,	BUILTIN_GetCurrentDate
	,	BUILTIN_GetCurrentTime
	,	BUILTIN_GetCurrentDateAndTime
	,	BUILTIN_To_Date_and_Time
	,	BUILTIN_strstr
	,	BUILTIN_strupr
	,	BUILTIN_strlwr
	,	BUILTIN_strlen
	,	BUILTIN_strcmp
	,	BUILTIN_strtrim
	,	BUILTIN_strmid
	,	BUILTIN_discard_on_exit
	,   BUILTIN__ListInsert				//Vibhor 200905: Added List Builtins
	,	BUILTIN__ListDeleteElementAt
	,   BUILTIN__MenuDisplay//Anil September 26 2005 added MenuDisplay
	,   BUILTIN_remove_all_abort_methods
	,   BUILTIN_DiffTime
	,   BUILTIN_AddTime
	,   BUILTIN_Make_Time
	,   BUILTIN_To_Time
	,   BUILTIN_Date_To_Time
	,   BUILTIN_To_Date
	,   BUILTIN_Time_To_Date
	,BUILTIN_DATE_to_days				// stevev 16jul14 - rest of the time builtins
	,BUILTIN_days_to_DATE				// stevev 16jul14 
	,BUILTIN_From_DATE_AND_TIME_VALUE	// stevev 16jul14 
	,BUILTIN_From_TIME_VALUE			// stevev 16jul14 
	,BUILTIN_TIME_VALUE_to_seconds		// stevev 16jul14 
	,BUILTIN_TIME_VALUE_to_Hour			// stevev 16jul14 
	,BUILTIN_TIME_VALUE_to_Minute		// stevev 16jul14 
	,BUILTIN_TIME_VALUE_to_Second		// stevev 16jul14 
	,BUILTIN_seconds_to_TIME_VALUE		// stevev 16jul14 
	,BUILTIN_DATE_AND_TIME_VALUE_to_string	// stevev 16jul14 
	,BUILTIN_DATE_to_string				// stevev 16jul14 
	,BUILTIN_TIME_VALUE_to_string		// stevev 16jul14 
	,BUILTIN_timet_to_string			// stevev 16jul14 
	,BUILTIN_timet_to_TIME_VALUE		// stevev 16jul14 
	,BUILTIN_To_TIME_VALUE				// stevev 16jul14 
	,   BUILTIN_fpclassify
	,   BUILTIN_nanf
	,   BUILTIN_nan		// stevev 25jun07
	,   BUILTIN_literal_string	// stevev 29jan08
	,   BUILTIN_openTransferPort	// stevev 24nov08 - block transfer
	,	BUILTIN_closeTransferPort	// stevev 24nov08
	,	BUILTIN_abortTransferPort	// stevev 24nov08
	,	BUILTIN_writeItem2Port		// stevev 24nov08
	,	BUILTIN_readItemfromPort	// stevev 24nov08
	,	BUILTIN_getTransferStatus	// stevev 24nov08
	,	BUILTIN__ERROR		// stevev 16jul14 -  debugging
	,	BUILTIN__TRACE		// stevev 16jul14
	,	BUILTIN__WARNING	// stevev 16jul14

};
//Added By Anil June 27 2005 --Ends here

#define BUILTIN_SUCCESS		0
#define BUILTIN_ABORT		1

#define NORMAL_CMD		0
#define MORE_STATUS_CMD	1


class hCitemBase;
class hCddbDevice;
class CInterpreter;
class CValueVarient;
class OneMeth;//MEE;
typedef map<string, BUILTIN_NAME> MAPBUILTINNAMETOENUM;

class CHart_Builtins
{
private:
	hCddbDevice *m_pDevice;
	CInterpreter *m_pInterpreter;
	OneMeth *m_pMeth;
	//Added By Anil June 27 2005 --starts here
	MAPBUILTINNAMETOENUM m_MapBuiltinFunNameToEnum;
	MAPBUILTINNAMETOENUM::iterator m_MapBuiltinFunNameToEnumIter;
	void MapBuiltinFunNameToEnum();
	//Added By Anil June 27 2005 --Ends here
	


	long lPre_postItemID;

	int SEND_COMMAND(
			
			int iCommandNumber
			, int iTransNumber
			, uchar *pchResponseStatus
			, uchar *pchMoreDataStatus
			, uchar *pchMoreDataInfo
			, int  iCmdType
			, bool bMoreDataFlag
			, int& iMoreInfoSize
			);

	void PackedASCIIToASCII (
								unsigned char *pbyPackedASCII
								, unsigned short wPackedASCIISize
								, char *pchASCII
							);
	void ASCIIToPackedASCII (
								char *pchASCII
		  					    , unsigned char *pbyPackedASCIIOutput
   						        , unsigned short *pwPackedASCIISize
	   					    );
/*Vibhor 081204: Start of Code*/
	//Adding this function to execute the actions on waveforms. Should only be called from PlotBuiltins
	int ExecuteWaveActions(actionType_t actionType,vector <hCitemBase*> wavePtrList);
/*Vibhor 081204: End of Code*/

/*Arun 170505 Start of code */
	int is_leap_year(int year);
/*End of code*/

public:	
	CHart_Builtins():m_pDevice(NULL),m_pInterpreter(NULL),lPre_postItemID(0),m_pMeth(0),m_AbortInProgress(false)
	{
		MapBuiltinFunNameToEnum();
	}

	~CHart_Builtins()
	{
	}


		//prashant 05/04/04
	bool m_AbortInProgress;

	//operations
	bool Initialise(hCddbDevice *pDevice,CInterpreter *pInterpreter,OneMeth/*MEE*/ *pMeth)
	{
		if ((pDevice == NULL) || (pInterpreter == NULL) || (pMeth == NULL))
		{
			return false;
		}
		m_pDevice = pDevice;
		m_pInterpreter = pInterpreter;
		m_pMeth = pMeth;

		return true;
	}
	
	bool Initialise(hCddbDevice *pDevice,CInterpreter *pInterpreter, long lItemId)
	{
		if ((pDevice == NULL) || (pInterpreter == NULL))
		{
			return false;
		}
		m_pDevice = pDevice;
		m_pInterpreter = pInterpreter;

		lPre_postItemID = lItemId;

		return true;
	}

	bool InvokeFunction(
					char *pchFunctionName
					, int iNumberOfParameters
					, INTER_VARIANT *pVarParameters
					, INTER_VARIANT *pVarReturnValue
					, int	*pBuiltinReturnCode
					, FunctionExpression* pFuncExp = 0
					);

	/* Methods to convert a Variant array to a normal array */
	bool GetLongArray
					(
						INTER_VARIANT &varValue
						, long *plongArray
						, int  &iArraySize
					);

	bool GetCharArray
					(
						INTER_VARIANT &varValue
						, char *pchArray
						, int  &iArraySize
					);

	bool SetCharArray
					(
						INTER_VARIANT &varValue
						, char *pchArray
						, int  aSize = 0 // stevev 30may07-0 is use-string length, non zero, use aSize
					);

	bool GetWCharArray
					(
						INTER_VARIANT &varValue
						, tchar *pchArray
						, int  &iArraySize
					);

	bool SetWCharArray
					(
						INTER_VARIANT &varValue
						, tchar *pchArray
						, int  aSize = 0 // stevev 30may07-0 is use-string length, non zero, use aSize
					);
/* <START>	List of built ins............ <START>	*/

/***** Start Delay builtins *****/
int delay
		(
			int iTimeInSeconds
			, tchar *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
		);

int DELAY
		(
			int iTimeInSeconds
			, tchar *pchDisplayString
		);

int DELAY_TIME
		(
			int iTimeInSeconds
		);
/* End Delay builtins */


/***** Start Abort builtins *****/
int abort();

int process_abort();

int _add_abort_method(long lMethodId);

int _remove_abort_method(long lMethodId);

int remove_all_abort();

/*Arun 190505 Start of code */
int push_abort_method(long lMethodId);

int pop_abort_method();

/*End of code*/

/* End Abort builtins */

/***** Start Display Message, Value, and Menu builtins *****/

/*Arun 190505 Start of code*/

tchar* BUILD_MESSAGE
		(
			tchar* dest
			, tchar* message
		);

/*End of code*/

int PUT_MESSAGE
		(
			tchar *pchDisplayString
		);

int put_message
		(
			tchar *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
		);

int ACKNOWLEDGE
		(
			tchar *pchDisplayString
		);

int acknowledge
		(
			tchar *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
		);

int _get_dev_var_value
		(
			tchar *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
			, long lItemId
		);

int _get_local_var_value				
		(
			tchar *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
			, char *pchVariableName
		);

int _display_xmtr_status
		(
			long lItemId
			, int  iStatusValue
		);

int display_response_status
		(
			long lCommandNumber
			, int  iStatusValue
		);

int display
		(
			tchar *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
		);

int SELECT_FROM_LIST
		(
			tchar *pchDisplayString
			, tchar *pchList
		);
	
int select_from_list
		(
			tchar *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
			, tchar *pchList
		);

/* End Abort Display Message, Value, and Menu builtins */

/***** Start Variable Access Builtins (Non-scaling) builtins *****/
/* Assign one item id to another */
int _vassign
		(
			long lItemId1
			, long lItemId
		);

int _dassign
		(
			long lItemId
			, double dValue
		);

int _fassign
		(
			long lItemId
			, float fValue
		);

int _iassign
		(
			long lItemId
			, INT64 iValue
		);

int _lassign
		(
			long lItemId
			,INT64 lValue
		);

float _fvar_value
		(
			long lItemId
		);
	
virtual INT64 _ivar_value
		(
			long lItemId
		);
	
virtual INT64 _lvar_value
		(
			long lItemId
		);

int save_values();
	
//Added By Anil July 01 2005 --starts here
//Discards any DD Variable changes made in a method when the method exits. 
int discard_on_exit();

//Added By Anil July 01 2005 --Ends here

	
//Added By Anil June 20 2005 --starts here

//svar_value will return the value of the specified DD VARIABLE. 
//The variable must be valid and of type ASCII or PACKED_ASCII. 
//The size of the assigned string must be at least as big as the size of the source variable plus 1 (for NULL termination).
//char[] svar_value(DD_ITEM & device_var)
char* svar_value(long lItemId);

//sassign will assign the specified value to the DD VARIABLE.  
//The variable must be valid, and must reference a variable of type ASCII or PACKED_ASCII. If the new value is too small, the string variable will be padded with spaces. 
//If the new value is too large, the string will be truncated to fit.
//int sassign(DD_ITEM & device_var, DD_STRING new_value)
int sassign(long lItemId, char* new_value);


//Added By Anil June 20 2005 --Ends here

	
/* End Variable Access Builtins (Non-scaling) builtins */


unsigned long  resolve_group_reference(unsigned long dwItemId, unsigned long dwIndex, itemType_t typeCheck);



/***** Start Communications builtins *****/
int get_more_status
		(
			  uchar *pchResponseCode
			, uchar *pchMoreStatusCode
			, int& moreInfoSize
		);
	
int _get_status_code_string
		(
			long lItemId
			, int iStatusCode
			, tchar *pchStatusString
			, int iStatusStringLength
		);

/* End Communications builtins */

/***** Start Name to ID Translation builtins *****/

/*Arun 190505 Start of code*/

int get_enum_string
		(
			long lItemId
			, int variable_value
			, tchar* status_string
			, int status_string_length
		);

/*End of code*/

int _get_dictionary_string
		(
			long lItemId
			, tchar *pchDictionaryString
			, int iMaxStringLength
		);

//Anil 22 December 2005 for dictionary_string built in
int _dictionary_string
		(
			long lItemId
			, tchar **pchDictionaryString			
		);
// stevev 29jan08
int literal_string
		(
			long lItemId
			, tchar **pchDictionaryString			
		);

long resolve_array_ref
		(
			long lItemId
			, int iIndex
		);

long resolve_record_ref
		(
			long lItemId
			, int iIndex
		);

long resolve_param_ref
		(
			long lItemId
		);

int rspcode_string
		(
			int iCommandNumber
			, int iResponseCode
			, tchar *pchResponseCodeString
			, int iResponseCodeStringLength
		);

/* End Name to ID Translation builtins */

/***** Start Scaling ABORT, IGNORE, RETRY builtins *****/
int _set_comm_status
		(
			int iCommStatus
			, int iAbortIgnoreRetry
		);

int _set_device_status
		(
			int iDeviceStatus
			, int iAbortIgnoreRetry
		);

int _set_resp_code
		(
			int iResponseCode
			, int iAbortIgnoreRetry
		);

int _set_all_resp_code
		(
			int iAbortIgnoreRetry
		);

int _set_no_device
		(
			int iAbortIgnoreRetry
		);

int SET_NUMBER_OF_RETRIES
		(
			int iNumberOfRetries
		);

/* End Scaling ABORT, IGNORE, RETRY builtins */

/***** Start XMTR ABORT, IGNORE, RETRY builtins *****/
int _set_xmtr_comm_status
		(
			int iCommStatus
			, int iAbortIgnoreRetry
		);

int _set_xmtr_device_status
		(
			int iDeviceStatus
			, int iAbortIgnoreRetry
		);

int _set_xmtr_resp_code
		(
			int iResponseCode
			, int iAbortIgnoreRetry
		);

int _set_xmtr_all_resp_code
		(
			int iAbortIgnoreRetry
		);

int _set_xmtr_no_device
		(
			int iAbortIgnoreRetry
		);

int _set_xmtr_all_data
		(
			int iAbortIgnoreRetry
		);

int _set_xmtr_data
		(
			int iByteCode
			, int iBitMask
			, int iAbortIgnoreRetry

		);

/* End XMTR ABORT, IGNORE, RETRY builtins */


/***** Start Floating point builtins *****/

//float NaN_value();
unsigned int NaN_value();

/* End Floating point builtins */

/***** Start Pre and Post actions builtins *****/
virtual int isetval(INT64 iValue);

int fsetval(float fValue);

virtual int lsetval(INT64 lValue);

int dsetval(double dValue);

virtual INT64 igetvalue();

virtual INT64 igetval();

float fgetval();

virtual INT64 lgetval();

double dgetval();

/*Arun 190505 Start of code*/

int sgetval
		(
			char* string
			, int length
		);

char* ssetval
		(
			const char* value
		);

/*End of code*/

/* End Pre and Post actions builtins */

/***** Start Command dispatching builtins *****/
int send
		(
			int iCommandNumber
			, uchar *pchResponseStatus
		);

int send_command
		(
			int iCommandNumber
		);

int send_command_trans
		(
			int iCommandNumber
			, int iTransNumber
		);

int send_trans
		(
			int iCommandNumber
			, int iTransNumber
			, uchar *pchResponseStatus
		);

int ext_send_command
		(
			int iCommandNumber
			, uchar *pchResponseStatus
			, uchar *pchMoreDataStatus
			, uchar *pchMoreDataInfo
			, int& moreInfoSize
		);

int ext_send_command_trans
		(
			int iCommandNumber
			, int iTransNumber
			, uchar *pchResponseStatus
			, uchar *pchMoreDataStatus
			, uchar *pchMoreDataInfo
			, int&  moreInfoSize
		);

int tsend_command
		(
			int iCommandNumber
		);

int tsend_command_trans
		(
			int iCommandNumber
			, int iTransNumber
		);

/* End Command dispatching builtins */
/* <END>  List of built ins................  <END> */


/*	int process_prompt(char *, ULONG *, ULONG *, long *);
	long delayfor (long , char *, ULONG *, ULONG *, long );		*/

	/*	Functions that use the device object */
//	int Read(long item_id,ITEM_TYPE item_type);
/*Vibhor 270204: Start of Code */
/*Since Read is being called by UI builtins as well as Non UI builtins,
  We need to differntiate between which ones need scaled value and which one not!
  bScalingReqd = true will call getDispValue  (UI Builtins)
  bScalingReqd = false will call getRawDispValue (Non UI Builtins)
*/
	int Read(long  itemID,   CValueVarient& ppReturnedDataItem,bool bScalingReqd);
/*Vibhor 270204: End of Code */
	int Write(long  itemID,   CValueVarient ppReturnedDataItem);
/* stevev -10aug07 - replaced	int bltin_format_string (char *out_buf, int max_length, 
								char *passed_prompt,long *glob_var_ids,int nCnt_Ids,
								CValueVarient *,bool &); //Modified by ANOOP 17FEB2004 ***/
	int 												
	bltin_format_string (tchar *out_buf, int max_length,    updatePermission_t updateLevel,
						 tchar *passed_prompt,			   long *glob_var_ids, int nCnt_Ids,
						 CValueVarient *pDynamicVarValues, bool & bDyanmicVarvalChanged) ;

/*Arun 110505 Start of code*/

/****************Math support builtins (EDDL)*********************/

double abs
		(
			double x
		);

double acos
		(	
			double x
		);

double asin
		(
			double x
		);

double atan
		(
			double x
		);

double cbrt
		(
			double x
		);

double ceil
		(
			double x
		);
double cos
		(	
			double x
		);

double cosh
		(	
			double x
		);

double exp
		(
			double x
		);

double floor
		(
			double x
		);

double fmod
		(
			double x,
			double y
		);
#ifdef XMTR
double frand();
#endif
double log
		(
			double x
		);

double log10
		(
			double x
		);

double log2
		(
			double x
		);

double pow
		(	
			double x,
			double y
		);

double round
		(
			double x
		);

double sin
		(	
			double x
		);

double sinh
		(
			double x
		);

double sqrt
		(
			double x
		);

double tan
		(
			double x
		);

double tanh
		(
			double x
		);

double trunc
		(
			double x
		);

double atof
		(
			char* string
		);

int atoi
		(
			char* string
		);

wchar_t* itoa
		(
			int value,
			wchar_t* string,
			int radix
		);

/*****************************End of Math Builtins (eDDL)************************/

/*End of code*/


/* Arun 160505 Start of code */

/****************************Date Time Builtins (eDDL) ***************************/

//long YearMonthDay_to_Date - WS:EPM-Not a builtin 25jun07
//		(
//			int year,
//			int month,
//			int dayOfMonth
//		);

int Date_to_Year
		(
			long days
		);

int Date_to_Month
		(
			long days
		);

int Date_to_DayOfMonth
		(
			long days
		);

//long GetCurrentDate();- WS:EPM-Not a builtin 25jun07

long GetCurrentTime();  // WS:EPM-return type to match spec 25jun07

//float GetCurrentDateAndTime();- WS:EPM-Not a builtin 25jun07

//float To_Date_and_Time- WS:EPM-Not a builtin 25jun07
//		(
//			int days,
//			int hour,
//			int minute,
//			int second,
//			int millisecond
//		);
			
/****************************End of Date Time Builtins (eDDL) ********************/
//Added By Anil June 17 2005 --starts here
/****************************Start of DD_STRING  Builtins  (eDDL) ********************/

#define MAX_DD_STRING	1024	/*stevev 20may07 - not everywhere yet */

//Find a substring.
wchar_t* STRSTR(wchar_t*  string_var,wchar_t* substring_to_find);

//Convert a string to upper case.
//The strupr function converts, in place, each lowercase letter in string_var to uppercase.
wchar_t* STRUPR(wchar_t* string_var);

//Convert a string to lower case.
//The STRLWR function converts, in place, each uppercase letter in string_var to lowercase.
wchar_t* STRLWR(wchar_t* string_var);

//This function returns the number of characters in a string variable.
int STRLEN(wchar_t* string_var);

//This function compares two strings lexicographically, and returns the relation as described below:
int STRCMP(wchar_t* string_var1, wchar_t* string_var2);

//Remove any leading and trailing space characters from a string and return the new string.
wchar_t* STRTRIM(wchar_t* string_var);

//Get a substring from a string specifying starting position and length.
wchar_t* STRMID(wchar_t* string_var,int start, int len);

/*Vibhor 200905: Start of Code*/

int _ListInsert(long lListId, int iIndex, long lItemId); //Insert an item in the list

int _ListDeleteElementAt(long lListId, int iIndex); //delete the item at the given index

/*Vibhor 200905: End of Code*/

//Anil September 26 2005 added MenuDisplay
int _MenuDisplay(long lMenuId, tchar *pchOptionList, long* lselection);

double DiffTime(long time_t1,long time_t0);

long AddTime(long time_t, long lseconds);//WS:EPM - second param type to match spec 25jun07
#ifdef MODIFIED_BY_SS
long Make_Time(int year,int month , int dayofmonth, int hour, int minute, int second , int isDST);

long To_Time(long date, int hour, int minute, int second, int isDST);

long Date_To_Time (long date);

long To_Date(int Year, int month, int DayOfMonth);

long Time_To_Date(long time_t);
#endif

// added stevev 16jul14 -----------------------
long			From_DATE_AND_TIME_VALUE(long, unsigned long);
long			From_TIME_VALUE(unsigned long);
long			DATE_to_days(long, long);
long			days_to_DATE(long, long);
unsigned long	seconds_to_TIME_VALUE(double);
double			TIME_VALUE_to_seconds(unsigned long);
int				TIME_VALUE_to_Hour(unsigned long);
int				TIME_VALUE_to_Minute(unsigned long);
int				TIME_VALUE_to_Second(unsigned long);
int				DATE_AND_TIME_VALUE_to_string(wchar_t*, wchar_t*, long, unsigned long);
int				DATE_to_string(wchar_t*,wchar_t*,long);
int				TIME_VALUE_to_string(wchar_t*,wchar_t*, unsigned long);
int				timet_to_string(wchar_t*, wchar_t*, long);
unsigned long	timet_to_TIME_VALUE(long);
unsigned long	To_TIME_VALUE(int, int, int);
// support functions
int get_tm(struct tm *pstTM, long date);
int get_tm_from_date_and_time(struct tm *pstTM, UINT64 dateTime);

int _fpclassifyf(float x);// becomes divided up to _fpclassifyf & _fpclassifyd
int _fpclassifyd(double x);// becomes divided up to _fpclassifyf & _fpclassifyd

float nanf(char* nan_string);

int   openPort(int iportNumber);	// stevev 24nov08 - block transfer

int   closePort(int iportNumber);	// stevev 24nov08 - block transfer

int   abortPort(int iportNumber);	// stevev 24nov08 - block transfer

int   write2Port(int iportNumber, unsigned itemNumber);		// stevev 24nov08 - block transfer

int   readFromPort(int iportNumber, unsigned itemNumber);	// stevev 24nov08 - block transfer

int   getTransferStatus(int iportNumber,int iDirection,long pLongItemIds[],int iNumberOfItemIds);

private:
	//An utility Function to get the Language Code
	void GetLanguageCode(char* szString, char* szLanguageCode, bool* bLangCodepresent);
	void GetLanguageCode(wchar_t* szString, wchar_t* szLanguageCode, bool* bLangCodepresent);

/* stevev 30may07 insert common code routines */
	bool GetStringParam(tchar* retString, int retStringLen,INTER_VARIANT *pParamArray, int paramNumber);
	bool GetByteStringParam(uchar* retString, int retStringLen,INTER_VARIANT *pParamArray, int paramNumber);
	bool GetCharStringParam(char* retString, int retStringLen,INTER_VARIANT *pParamArray, int paramNumber);

	bool SetStringParam(FunctionExpression* pFuncExp, INTER_VARIANT *pParamArray, 
												    int paramNumber, tchar* paramString);
	// ByteString is NOT required to be null terminated and is L bytes long
	bool SetByteStringParam(FunctionExpression* pFuncExp, INTER_VARIANT *pParamArray, 
																 int paramNumber, _BYTE_STRING& bsS);
	// CharString is used to null terminator or L length, which ever comes first
	bool SetCharStringParam(FunctionExpression* pFuncExp, INTER_VARIANT *pParamArray, 
													 int paramNumber, char* paramString, int L);
	bool SetByteArrParam(FunctionExpression* pFuncExp, INTER_VARIANT *pParamArray, 
													 int paramNumber, char* paramString, int L);
	virtual bool 
		OutputParameterValue( FunctionExpression* pFuncExp, int nParamNumber, 
															 INTER_VARIANT &NewVarValue, int L);
/* stevev 30may07 - end */


/****************************End of DD_STRING  Builtins (eDDL) **********************************/
//Added By Anil June 17 2005 --Ends here
/* End of code */

	// added stevev 14aug07
	bool doFormat(tchar* formatStr, hCVar* pV,CValueVarient& vValue,tchar* retStr,int rsLen);

};

#endif /*HART_BUILTINS_H*/