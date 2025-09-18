
/***** Start Delay builtins *****/
int delay
		(
			int iTimeInSeconds
			, char *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
		);

int DELAY
		(
			int iTimeInSeconds
			, char *pchDisplayString
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

char* BUILD_MESSAGE
		(
			char* dest
			, char* message
		);

/*End of code*/

int PUT_MESSAGE
		(
			char *pchDisplayString
		);

int put_message
		(
			char *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
		);

int ACKNOWLEDGE
		(
			char *pchDisplayString
		);

int acknowledge
		(
			char *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
		);

int _get_dev_var_value
		(
			char *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
			, long lItemId
		);

int _get_local_var_value				
		(
			char *pchDisplayString
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
			char *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
		);

int SELECT_FROM_LIST
		(
			char *pchDisplayString
			, char *pchList
		);
	
int select_from_list
		(
			char *pchDisplayString
			, long *plItemIds
			, int	iNumberOfItemIds
			, char *pchList
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
			, int iValue
		);

int _lassign
		(
			long lItemId
			, long lValue
		);

float _fvar_value
		(
			long lItemId
		);
	
int _ivar_value
		(
			long lItemId
		);
	
long _lvar_value
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

/***** Start Communications builtins *****/
int get_more_status
		(
			char *pchResponseCode
			, char *pchMoreStatusCode
		);
	
int _get_status_code_string
		(
			long lItemId
			, int iStatusCode
			, char *pchStatusString
			, int iStatusStringLength
		);

/* End Communications builtins */

/***** Start Name to ID Translation builtins *****/

/*Arun 190505 Start of code*/

int get_enum_string
		(
			long lItemId
			, int variable_value,
			, char* status_string
			, int status_string_length
		);

/*End of code*/

int _get_dictionary_string
		(
			long lItemId
			, char *pchDictionaryString
			, int iMaxStringLength
		);

//Anil 22 December 2005 for dictionary_string built in
int _dictionary_string
		(
			long lItemId
			, char *pchDictionaryString		
		);

//stevev 29jan08
int literal_string
		(
			long lItemId
			, char *pchDictionaryString		
		);


long resolve_array_ref
		(
			long lItemId
			, int iIndex
		);

long resolve_record_ref
		(
			long lItemId
			, int iMember
		);

long resolve_param_ref
		(
			long lItemId
		);

int rspcode_string
		(
			int iCommandNumber
			, int iResponseCode
			, char *pchResponseCodeString
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

float NaN_value();
double nan(	char* string ); /* stevev - added 25jun07 */
float nanf(	char* string ); /* stevev - added 25jun07 */
int fpclassifyf( float fValue );/* stevev - added 25jun07 */
int fpclassifyd( double dValue );/* stevev - added 08mar14 */

/* End Floating point builtins */

/***** Start Pre and Post actions builtins *****/
int isetval(int iValue);

int fsetval(float fValue);

int lsetval(long lValue);

int dsetval(double dValue);

int igetvalue();

int igetval();

float fgetval();

long lgetval();

double dgetval();

/*Arun 190505 Start of code*/

int sgetval
		(
			char* string
			, int length
		);

char* ssetval
		(
			char* value
		);

/*End of code*/

/* End Pre and Post actions builtins */

/***** Start Command dispatching builtins *****/
int send
		(
			int iCommandNumber
			, char *pchResponseStatus
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
			, char *pchResponseStatus
		);

int ext_send_command
		(
			int iCommandNumber
			, char *pchResponseStatus
			, char *pchMoreDataStatus
			, char *pchMoreDataInfo
		);

int ext_send_command_trans
		(
			int iCommandNumber
			, int iTransNumber
			, char *pchResponseStatus
			, char *pchMoreDataStatus
			, char *pchMoreDataInfo
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

// Anil December 16 2005 deleted codes related to Plot builtins. Please refer the previous version

/*Arun 110505 Start of code*/

/*Math support builtins (EDDL)*/

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
double frand		();
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

char* itoa
		(
			int value,
			char* string,
			int radix
		);

/*****************************End of Math Builtins (eDDL)************************/

/*End of code*/


/* Arun 160505 Start of code */

/****************************Date Time Builtins (eDDL) ***************************/

//long YearMonthDay_to_Date - Not a Builtin WS:EPM-25jun07
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

//long GetCurrentDate(); - Not a Builtin WS:EPM-25jun07

long GetCurrentTime();//- change return value to spec'd WS:EPM-25jun07

//float GetCurrentDateAndTime();- Not a Builtin WS:EPM-25jun07

//float To_Date_and_Time - Not a Builtin WS:EPM-25jun07
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

//Find a substring.
char* STRSTR(char*  string_var,char* substring_to_find);

//Convert a string to upper case.
//The strupr function converts, in place, each lowercase letter in string_var to uppercase.
char* STRUPR(char* string_var);

//Convert a string to lower case.
//The STRLWR function converts, in place, each uppercase letter in string_var to lowercase.
char* STRLWR(char* string_var);

//This function returns the number of characters in a string variable.
int STRLEN(char* string_var);

//This function compares two strings lexicographically, and returns the relation as described below:
int STRCMP(char* string_var1, char* string_var2);

//Remove any leading and trailing space characters from a string and return the new string.
char* STRTRIM(char* string_var);

//Get a substring from a string specifying starting position and length.
char* STRMID(char* string_var,int start, int len);


/****************************End of DD_STRING  Builtins (eDDL) ********************/
//Added By Anil June 17 2005 --Ends here
/* End of code */

/*Vibhor 200905: Start of Code*/

//The tokenizer would preprocess the names to Ids

int _ListInsert(long lListId, int iIndex, long lItemId);

int _ListDeleteElementAt(long lListId, int iIndex);

/*Vibhor 200905: End of Code*/
//Added By Anil September 26 2005
int _MenuDisplay(long lMenuId, char *pchOptionList, long* lselection);