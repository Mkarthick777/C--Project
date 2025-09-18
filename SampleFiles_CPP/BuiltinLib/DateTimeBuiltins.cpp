#include "pch.h"
#include "ddbdevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"
//#include "MEE.h"
#include "DDBitemBase.h"
#include "ddbdefs.h"
#include <time.h>
#ifdef _WIN32_WCE
#include "time_ce.h"	// PAW function missing 06/05/09
#endif

#ifdef MODIFIED_BY_SS
/// commented by Srilatha
///* no longer required - 25jun07 WS:EPM
//static char daytab[2][13] =  {
//	{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
//	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
//};
//***/
//
///** added by stevev 25jun07 ***/
volatile unsigned long randomKey = 0;// set from outside
//
//// internal conversion functions
//long   __time_t2Htime_t(time_t timeIn); // SDC internal time_t (32 or 64 bit) to user time_t (as used in the method)
//time_t __Htime_t2time_t(long htimeT);	// user time_t (as used in the method) converted to SDC internal time_t (32 or 64 bit)
//
//#if defined( IS_SDC ) || defined ( linux )
//#define make_time_t( a)    __time_t2Htime_t( a )	/* returns Htime_t, a long */
//#define get_time( b)       __Htime_t2time_t( b )	/* returns time_t, a int or int64 */
//#else
//// most of the uses of this should be optimized out by the compiler
//#define make_time_t( a)    ( (long)   (a) )	/* returns Htime_t, a long */
//#define get_time( b)       ( (time_t) (b) )	/* returns time_t, a int or int64 */
//#endif
//
//// internal conversion functions
//// SDC internal time_t (32 or 64 bit) to user time_t (as used in the method)
//long   __time_t2Htime_t(time_t timeIn)
//{// use randomKey to encode our internal time_t to the HART-time_t
//	long val = (long)timeIn; // we cast the (possibly) __int64 to a long, 
//							 // but we will loose resolution (i.e. won't work after year 2038)
//	// temporary algorithm							 
//	val ^= randomKey;
//	return val;
//}
// 
//// user time_t (as used in the method) converted to SDC internal time_t (32 or 64 bit)
//time_t __Htime_t2time_t(long htimeT)
//{// use randomKey to decode the HART-time_t to our internal time_t
//	// temporary algorithm	
//	time_t rVal = htimeT ^ randomKey;
//	return rVal;
//}
///** end added by stevev 25jun07 **/
//
///* Arun 160505 Start of code*/
///****************************Date Time Builtins (eDDL)*******************/
//
////int CHart_Builtins::is_leap_year(int year)// WS 25jun07 - no longer required.
////{
////	return (year%4 == 0 && year%100 != 0) || year%400 == 0;
////}
//
//
////long CHart_Builtins::YearMonthDay_to_Date(int year,int month,int dayOfMonth)// WS 25jun07 - not a builtin.
////{
////	return 0;
////}	
//	
////convert long DATE type to struct tm type
////if there is error in input, this function returns -1. Otherwise, this function returns 0
//int CHart_Builtins::get_tm(struct tm *pstTM, long date)
//{
//	pstTM->tm_wday = 0;
//	pstTM->tm_yday = 0;
//	pstTM->tm_year =  date  &  0x0000FF;
//	pstTM->tm_mon =   ((date & 0x00FF00) >> 8) - 1;
//	pstTM->tm_mday =  (date & 0xFF0000) >> 16;
//	pstTM->tm_hour = 0;
//	pstTM->tm_min = 0;
//	pstTM->tm_sec = 0;
//	pstTM->tm_isdst = -1;	//the C run-time library code compute whether standard time or daylight saving time is in effect.
//
//	//check input limits
//	if (pstTM->tm_mon < 0)
//	{
//		return BI_ERROR;
//	}
//	else if (pstTM->tm_mon > 11)
//	{
//		return BI_ERROR;
//	}
//	if (pstTM->tm_mday < 1)
//	{
//		return BI_ERROR;
//	}
//	else if (pstTM->tm_mday > 31)
//	{
//		return BI_ERROR;
//	}
//
//	(void)mktime(pstTM);
//	return BI_SUCCESS;
//}	
//
////convert unsinged long long DATE_AND_TIME type to struct tm type
////if there is error in input, this function returns -1. Otherwise, this function returns 0
//int CHart_Builtins::get_tm_from_date_and_time(struct tm *pstTM, UINT64 dateTime)
//{
//	pstTM->tm_wday = ((dateTime >> 21) & 0x07);
//	pstTM->tm_yday = 0;
//	pstTM->tm_year =  dateTime  &  0xFF;
//	pstTM->tm_mon =   ((dateTime >> 8) & 0xFF) - 1;
//	pstTM->tm_mday =  ((dateTime >> 16) & 0x1F);
//	pstTM->tm_hour = ((dateTime >> 24) & 0x1F);
//	pstTM->tm_min = ((dateTime >> 32) & 0x3F);
//	pstTM->tm_sec = ((dateTime >> 40) & 0xFFFF) / 1000;
//	//1: daylight saving time is in effect. 0: daylight saving time is not in effect.
//	pstTM->tm_isdst = ((dateTime >> 31) & 0x1);
//
//	//check input limits
//	if (pstTM->tm_mon < 0)
//	{
//		return BI_ERROR;
//	}
//	else if (pstTM->tm_mon > 11)
//	{
//		return BI_ERROR;
//	}
//	if (pstTM->tm_mday < 1)
//	{
//		return BI_ERROR;
//	}
//	else if (pstTM->tm_mday > 31)
//	{
//		return BI_ERROR;
//	}
//
//	(void)mktime(pstTM);
//	return BI_SUCCESS;
//}
//
//	
int CHart_Builtins::Date_to_Year( long hart_date )//WS 25jun07 - converted to HARTdate to year
{
	int nYear = hart_date & 0xFF;
	nYear += 1900;
	return nYear;
}
//
int CHart_Builtins::Date_to_Month( long hart_date )//WS 25jun07 - converted to HARTdate to month
{
	int nMonth = ((hart_date >> 8) & 0xFF) + 1;
	return nMonth;
}
//
int CHart_Builtins::Date_to_DayOfMonth( long hart_date )//WS 25jun07 - converted to HARTdate to DOM
{
	int nDay = (hart_date >> 16) & 0xFF;
	return nDay;
}
//
////long CHart_Builtins::GetCurrentDate()//WS 25jun07 - Not a builtin
////{
//// removed body
////}
//
//long CHart_Builtins::GetCurrentTime()// WS 25jun07 - changed return type
//{
//#ifndef _WIN32_WCE	// function missing PAW 06/05/09
//	time_t		ltime;//Note that time_t is defined as an __int64 in VS2005
//
//	time( &ltime );
//#else
//	time_t_ce ltime;	
//	ltime = time_ce(NULL);
//#endif
//	return make_time_t( ltime );
//}
//
////float CHart_Builtins::GetCurrentDateAndTime()// WS 25jun07 - not a builtin
////{
////	return GetCurrentTime();
////}
//
////float CHart_Builtins::To_Date_and_Time(	int days,int hour,int minute,int second,int millisecond	)// WS 25jun07 - not a builtin
////{
////	return float (days + hour/24.0 + (minute)/(24.0*60.0) + (second)/(24.0 * 60.0 * 60.0)+ (millisecond / 1000)/(24.0 * 60.0 * 60.0));
////}
//
double CHart_Builtins::DiffTime(long time_t1, long time_t0) {
	// Convert long values to std::chrono::time_point objects
	std::chrono::duration<double> diff = std::chrono::time_point<std::chrono::system_clock> (std::chrono::seconds(time_t1)) - 
	std::chrono::time_point<std::chrono::system_clock> (std::chrono::seconds(time_t0));

	

	// Return the difference in seconds as a double
	return diff.count();
}

long CHart_Builtins::AddTime(long time_t1, long lseconds)
{// modified to decode/encode 25jun07 - stevev
	std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::from_time_t(static_cast<time_t>(time_t1));

	// Create a duration of lseconds
	std::chrono::seconds duration(lseconds);

	// Add the duration to the time point
	std::chrono::time_point<std::chrono::system_clock> result_time = t1 + duration;

	// Convert the result back to time_t and return
	return std::chrono::system_clock::to_time_t(result_time);
}

long CHart_Builtins::Make_Time(int year, int month, int dayOfMonth, int hour, int minute, int second, int isDST) {
	// Create a tm structure with the provided values
	std::tm StDateNTime = {};
	StDateNTime.tm_year = year - 1900;
	StDateNTime.tm_mon = month - 1;
	StDateNTime.tm_mday = dayOfMonth;
	StDateNTime.tm_hour = hour;
	StDateNTime.tm_min = minute;
	StDateNTime.tm_sec = second;
	StDateNTime.tm_isdst = isDST;

	// Convert the tm structure to a time_t value
	std::time_t time_t_value = std::mktime(&StDateNTime);

	// Return the time_t value
	return static_cast<long>(time_t_value);
}

long CHart_Builtins::To_Time(long date, int hour, int minute, int second, int isDST) {
	// Extract year, month, and dayOfMonth from the date value
	int year = date & 0xFF;
	int month = (date >> 8) & 0xFF;
	int dayOfMonth = (date >> 16) & 0xFF;

	// Create a tm structure with the extracted values
	std::tm StDateNTime = {};
	StDateNTime.tm_year = year - 1900; // Years since 1900
	StDateNTime.tm_mon = month - 1;     // Months since January (0-based)
	StDateNTime.tm_mday = dayOfMonth;
	StDateNTime.tm_hour = hour;
	StDateNTime.tm_min = minute;
	StDateNTime.tm_sec = second;
	StDateNTime.tm_isdst = isDST;

	// Convert the tm structure to a time_t value
	std::time_t time_t_value = std::mktime(&StDateNTime);

	// Return the time_t value
	return static_cast<long>(time_t_value);
}
long CHart_Builtins::Date_To_Time(long date) {
	// Extract year, month, and dayOfMonth from the date value
	int year = date & 0xFF;
	int month = (date >> 8) & 0xFF;
	int dayOfMonth = (date >> 16) & 0xFF;

	// Create a tm structure with the extracted values
	std::tm StDateNTime = {};
	StDateNTime.tm_year = year - 1900; // Years since 1900
	StDateNTime.tm_mon = month - 1;     // Months since January (0-based)
	StDateNTime.tm_mday = dayOfMonth;
	StDateNTime.tm_hour = 0;
	StDateNTime.tm_min = 0;
	StDateNTime.tm_sec = 0;
	StDateNTime.tm_isdst = false;

	// Convert the tm structure to a time_t value
	std::time_t time_t_value = std::mktime(&StDateNTime);

	// Return the time_t value
	return static_cast<long>(time_t_value);
}

long CHart_Builtins::To_Date(int year, int month, int dayOfMonth) {
	// Perform range checking for year
	if (year >= 1900) {
		year = year - 1900;
	}
	if (year > 255) {
		year = 255;
	}

	// Perform range checking for month
	if (month > 0) { // Assuming January == 1, not January == 0
		month--;
	}
	if (month > 11) { // No months are greater than December
		month = 11;
	}

	// Perform range checking for dayOfMonth
	if (dayOfMonth > 31) { // No day of months > 31
		dayOfMonth = 31; // Don't bother checking for February 31... This is already an error condition
	}

	// Calculate the HART date
	long hart_date = year;
	hart_date |= (static_cast<long>(month) << 8);
	hart_date |= (static_cast<long>(dayOfMonth) << 16);

	return hart_date;
}
time_t get_time(long input) {
	// Some implementation to convert 'input' to a time_t value
	// For example:
	return static_cast<time_t>(input);
}
long CHart_Builtins::Time_To_Date(long time_t1) {
	// Decode time_t value
	time_t tempTime = get_time(time_t1);

	// Convert time_t to tm structure
	struct tm StDateNTime;
#ifndef _WIN32_WCE
	gmtime_s(&StDateNTime, &tempTime); // Use gmtime_s for safer operation
#else
	struct tm tmbuf;
	gmtime_r_ce(&tempTime, &tmbuf, FALSE /*non local*/); // Adjust for your platform
	StDateNTime = tmbuf; // Copy the result to StDateNTime
#endif

	// Calculate HART date
	long hart_date = StDateNTime.tm_year;
	hart_date |= (StDateNTime.tm_mon << 8);
	hart_date |= (StDateNTime.tm_mday << 16);

	return hart_date;
}

long CHart_Builtins::GetTickCount() {
	// Define the type aliases for clarity
	using Clock = std::chrono::steady_clock;
	using Duration = std::chrono::milliseconds;

	// Get the current time point
	auto now = Clock::now();

	// Get the duration since some arbitrary reference point
	auto duration = std::chrono::duration_cast<Duration>(now.time_since_epoch());

	// Return the count of milliseconds
	return static_cast<long>(duration.count());
}
int CHart_Builtins::get_tm(struct tm* timeinfo, long date) {
	timeinfo->tm_sec = 0;
	timeinfo->tm_min = 0;
	timeinfo->tm_hour = 0;
	timeinfo->tm_mday = date >> 16;
	timeinfo->tm_mon = ((date >> 8) & 0xFF) - 1;
	timeinfo->tm_year = (date & 0xFF) + 100;

	return 0; // Return 0 for success, -1 for error
}

long CHart_Builtins::DATE_to_days(long date1, long date0) {
	struct tm StDateNTime;
	if (get_tm(&StDateNTime, date1) == BI_ERROR)
		return BI_ERROR;

	time_t time1 = mktime(&StDateNTime);

	struct tm StDateNTime0;
	if (get_tm(&StDateNTime0, date0) == BI_ERROR)
		return BI_ERROR;

	time_t time0 = mktime(&StDateNTime0);

	double dDiffTime = difftime(time1, time0); // in seconds

	if (date1 != date0 && dDiffTime == 0)
		return BI_ERROR;
	else
		return static_cast<long>(dDiffTime / (24 * 60 * 60)); // in days
}

long CHart_Builtins::days_to_DATE(long days, long date0) {
	struct tm StDateNTime;
	if (get_tm(&StDateNTime, date0) == BI_ERROR)
		return BI_ERROR;

	StDateNTime.tm_mday += days;

	time_t time0 = mktime(&StDateNTime);

	struct tm StDateNTime0;
#ifdef _MSC_VER // Check if compiling with MSVC
	localtime_s(&StDateNTime0, &time0); // Use localtime_s for safer operation
#else
	localtime_r(&time0, &StDateNTime0); // Use localtime_r for other platforms
#endif

	long futureDate = ((StDateNTime0.tm_mday << 16) & 0xFFFF0000) | (((StDateNTime0.tm_mon + 1) << 8) & 0xFF00) | ((StDateNTime0.tm_year - 100) & 0xFF);
	return futureDate;
}









long CHart_Builtins::From_DATE_AND_TIME_VALUE(long date, unsigned long time_value) {
	struct tm timeinfo;
	if (get_tm(&timeinfo, date) == BI_ERROR)
		return BI_ERROR;

	timeinfo.tm_sec = time_value / 32000;

	time_t returnedVal;
	returnedVal = mktime(&timeinfo);
	return returnedVal;
}

long CHart_Builtins::From_TIME_VALUE(unsigned long time_value) {
	time_t returnedVal;
	time(&returnedVal);
	returnedVal += (time_value / 32000);
	return returnedVal;
}

unsigned long CHart_Builtins::seconds_to_TIME_VALUE(double seconds) {
	return static_cast<unsigned long>(seconds * 32000);
}

double CHart_Builtins::TIME_VALUE_to_seconds(unsigned long time_value) {
	return static_cast<double>(time_value) / 32000;
}

int CHart_Builtins::TIME_VALUE_to_Hour(unsigned long time_value) {
	return static_cast<int>((static_cast<float>(time_value) / (60 * 60 * 32000)));
}

int CHart_Builtins::TIME_VALUE_to_Minute(unsigned long time_value) {
	return static_cast<int>((static_cast<float>(time_value) / (60 * 32000)));
}

int CHart_Builtins::TIME_VALUE_to_Second(unsigned long time_value) {
	return static_cast<int>((static_cast<float>(time_value) / 32000));
}

int CHart_Builtins::DATE_AND_TIME_VALUE_to_string(wchar_t* output_str, wchar_t* format, long date, unsigned long time_value) {
	struct tm StDateNTime;
	if (get_tm(&StDateNTime, date) == BI_ERROR)
		return BI_ERROR;

	StDateNTime.tm_sec = time_value / 32000;

	// update tm in standard format for display
	mktime(&StDateNTime);

	return wcsftime(output_str, MAX_DD_STRING, format, &StDateNTime);
}

int CHart_Builtins::DATE_to_string(wchar_t* output_str, wchar_t* format, long date) {
	struct tm StDateNTime;
	if (get_tm(&StDateNTime, date) == BI_ERROR)
		return BI_ERROR;

	return wcsftime(output_str, MAX_DD_STRING, format, &StDateNTime);
}

int CHart_Builtins::timet_to_string(wchar_t* timet_str, wchar_t* format, long timet_value) {
	time_t time_t_val = get_time(timet_value);

	// Get the time in tm struct
	struct tm StDateNTime;
#ifdef _MSC_VER // Check if compiling with MSVC
	gmtime_s(&StDateNTime, &time_t_val); // Use gmtime_s for safer operation
#else
	gmtime_r(&time_t_val, &StDateNTime); // Use gmtime_r for other platforms
#endif

	// Format the time string
	int returnVal = wcsftime(timet_str, MAX_DD_STRING, format, &StDateNTime);

	return returnVal;
}

unsigned long CHart_Builtins::timet_to_TIME_VALUE(long timet_value) {
    // Convert time_t structure to tm structure in local time zone
    time_t inputTime = get_time(timet_value);
    struct tm StDateNTime;
    #ifdef _MSC_VER // Check if compiling with MSVC
        localtime_s(&StDateNTime, &inputTime); // Use localtime_s for safer operation
    #else
        localtime_r(&inputTime, &StDateNTime); // Use localtime_r for other platforms
    #endif

    // Output is the number of 1/32 ms since midnight
    return 32000 * ((StDateNTime.tm_hour * 60 + StDateNTime.tm_min) * 60 + StDateNTime.tm_sec);
}

unsigned long CHart_Builtins::To_TIME_VALUE(int hours, int minutes, int seconds) {
	return static_cast<unsigned long>(32000 * (60 * (60 * hours + minutes) + seconds));
}

int CHart_Builtins::TIME_VALUE_to_string(wchar_t* time_value_str, wchar_t* format, unsigned long time_value) {
	// Initialize tm structure
	struct tm StDateNTime = {};
	StDateNTime.tm_year = 72; // Jan. 1, 1972
	StDateNTime.tm_mon = 0;
	StDateNTime.tm_mday = 1;
	StDateNTime.tm_sec = time_value / 32000;
	StDateNTime.tm_isdst = -1; // Daylight saving time is unknown

	// Normalize tm structure
	mktime(&StDateNTime);

	// Format the time string
	return wcsftime(time_value_str, MAX_DD_STRING, format, &StDateNTime);
}






// long CHart_Builtins::Make_Time(int year, int month, int dayOfMonth, int hour, int minute, int second, int isDST)
//{
//	struct tm StDateNTime;	
//	
//	//do some range checking
//	if( year >= 1900 )
//	{
//		year = year - 1900;
//	}
//	if( year > 255 )
//	{
//		year = 255;
//	}
//
//	if( month > 0 )//assume that the users pass in Jan == 1 not Jan == 0
//	{
//		month--;
//	}
//	if( month > 11 )//No months are greater than Dec.
//	{
//		month = 11;
//	}
//
//	if( dayOfMonth > 31 )//No day of months > 31
//	{
//		dayOfMonth = 31; //Dont bother checking for Feb 31... This is already an error condition.
//	}
//	
//	StDateNTime.tm_wday = 0;
//	StDateNTime.tm_yday = 0;
//	StDateNTime.tm_year = year;
//	StDateNTime.tm_mon = month;
//	StDateNTime.tm_mday = dayOfMonth;
//	StDateNTime.tm_hour = hour;
//	StDateNTime.tm_min = minute;
//	StDateNTime.tm_sec = second;
//	StDateNTime.tm_isdst = isDST;
//#ifndef _WIN32_WCE	// function missing PAW 06/05/09	
//	return make_time_t( mktime( &StDateNTime) );// encode stevev 25jun07
//#else
//	return make_time_t( mktime_ce( &StDateNTime) );// encode stevev 25jun07
//#endif
//	
//
//}


//double CHart_Builtins::DiffTime(long time_t1,long time_t0)
//{// modified to decode 25jun07 - stevev
//	time_t _t1 = get_time(time_t1);
//	time_t _t0 = get_time(time_t0);
//	return difftime( _t1, _t0 );
//}
//// WS 25jun07-changed 2nd param type- it is cast from numeric in Invoke
//long CHart_Builtins::AddTime(long time_t1, long lseconds)
//{// modified to decode/encode 25jun07 - stevev
//	time_t _t1 = get_time(time_t1);
//	return make_time_t(_t1 + lseconds);
//}
//
//long CHart_Builtins::Make_Time( int year, int month, int dayOfMonth, int hour, int minute, int second, int isDST )
//{
//	struct tm StDateNTime;	
//	
//	//do some range checking
//	if( year >= 1900 )
//	{
//		year = year - 1900;
//	}
//	if( year > 255 )
//	{
//		year = 255;
//	}
//
//	if( month > 0 )//assume that the users pass in Jan == 1 not Jan == 0
//	{
//		month--;
//	}
//	if( month > 11 )//No months are greater than Dec.
//	{
//		month = 11;
//	}
//
//	if( dayOfMonth > 31 )//No day of months > 31
//	{
//		dayOfMonth = 31; //Dont bother checking for Feb 31... This is already an error condition.
//	}
//	
//	StDateNTime.tm_wday = 0;
//	StDateNTime.tm_yday = 0;
//	StDateNTime.tm_year = year;
//	StDateNTime.tm_mon = month;
//	StDateNTime.tm_mday = dayOfMonth;
//	StDateNTime.tm_hour = hour;
//	StDateNTime.tm_min = minute;
//	StDateNTime.tm_sec = second;
//	StDateNTime.tm_isdst = isDST;
//#ifndef _WIN32_WCE	// function missing PAW 06/05/09	
//	return make_time_t( mktime( &StDateNTime) );// encode stevev 25jun07
//#else
//	return make_time_t( mktime_ce( &StDateNTime) );// encode stevev 25jun07
//#endif
//	
//
//}
//
//long CHart_Builtins::To_Time(long date, int hour, int minute, int second, int isDST)
//{
//	struct tm StDateNTime;	
//	
//	StDateNTime.tm_wday = 0;
//	StDateNTime.tm_yday = 0;
//	StDateNTime.tm_year =  date  &  0xFF;// WS 25jun07 fixed via normalization
//	StDateNTime.tm_mon =   (date >> 8) &  0xFF;// WS 25jun07 fixed via normalization
//	StDateNTime.tm_mday =  (date >> 16)  &  0xFF;// WS 25jun07 fixed via normalization
//	StDateNTime.tm_hour = hour;
//	StDateNTime.tm_min = minute;
//	StDateNTime.tm_sec = second;
//	StDateNTime.tm_isdst = isDST;
//
//#ifndef _WIN32_WCE	// function missing PAW 06/05/09	
//	return make_time_t( mktime( &StDateNTime) );// encode stevev 25jun07
//#else
//	return make_time_t( mktime_ce( &StDateNTime) );// encode stevev 25jun07
//#endif
//	
//
//}
//
//long CHart_Builtins::Date_To_Time (long date)
//{
//	struct tm StDateNTime;	
//	
//	StDateNTime.tm_wday = 0;
//	StDateNTime.tm_yday = 0;
//	StDateNTime.tm_year =  date  &  0xFF;// WS 25jun07 fixed via normalization
//	StDateNTime.tm_mon =   (date >> 8) &  0xFF;// WS 25jun07 fixed via normalization
//	StDateNTime.tm_mday =  (date >> 16)  &  0xFF;// WS 25jun07 fixed via normalization
//	StDateNTime.tm_hour = 0;
//	StDateNTime.tm_min = 0;
//	StDateNTime.tm_sec = 0;
//	StDateNTime.tm_isdst = false;//As per Wally's comment
//
//#ifndef _WIN32_WCE	// function missing PAW 06/05/09	
//	return make_time_t( mktime( &StDateNTime) );// encode stevev 25jun07
//#else
//	return make_time_t( mktime_ce( &StDateNTime) );// encode stevev 25jun07
//#endif
//
//
//}
//
//
//long CHart_Builtins::To_Date(int year, int month, int dayOfMonth)// WS 25jun07 - return a HART DATE
//{
//	long hart_date = 0;
//	//do some range checking
//	if( year >= 1900 )
//	{
//		year = year - 1900;
//	}
//	if( year > 255 )
//	{
//		year = 255;
//	}
//
//	if( month > 0 )//assume that the users pass in Jan == 1 not Jan == 0
//	{
//		month--;
//	}
//	if( month > 11 )//No months are greater than Dec.
//	{
//		month = 11;
//	}
//
//	if( dayOfMonth > 31 )//No day of months > 31
//	{
//		dayOfMonth = 31; //Dont bother checking for Feb 31... This is already an error condition.
//	}
//
//	hart_date = year;
//	hart_date |= (month << 8);
//	hart_date |= (dayOfMonth << 16);
//
//	return hart_date;
//}
//
//long CHart_Builtins::Time_To_Date(long time_t1)// WS 25jun07 - return a HART DATE
//{// stevev 25jun07 - decode time_t coming in
//	time_t tempTime = get_time(time_t1);  // copy our data from a long into a time_t (which is defined as an __int64 in VS2005)
//#ifndef _WIN32_WCE		// PAW function missing 06/05/09
//	struct tm *StDateNTime = gmtime( &tempTime );// WS - 9apr07 - 2005 checkin
//#else
//	struct tm *tmbuf;
//	struct tm *StDateNTime = gmtime_r_ce( &tempTime, tmbuf, FALSE /*non local*/ );
//#endif
//
//	long hart_date = StDateNTime->tm_year;
//	hart_date |= (StDateNTime->tm_mon << 8);
//	hart_date |= (StDateNTime->tm_mday << 16);
//
//	return hart_date;
//}
//
////================= DATE & TIME functions added 16jul14 stevev from emerson =======================================================
//// This function assumes the inputs are DATE types in 4 bytes.
//// | days of month ([1,31] in 2 bytes) | month ([1,12] in 1 byte) | year ([0, 255] in 1 byte) |
//long CHart_Builtins::DATE_to_days( long date1, long date0 )
//{
//	struct tm StDateNTime;
//	if (get_tm(&StDateNTime, date1) == BI_ERROR)
//	{
//		return BI_ERROR;
//	}
//
//	time_t time1 = mktime( &StDateNTime);
//
//	struct tm StDateNTime0;	
//	if (get_tm(&StDateNTime0, date0) == BI_ERROR)
//	{
//		return BI_ERROR;
//	}
//
//	time_t time0 = mktime( &StDateNTime0);
//
//	double dDiffTime = difftime(time1, time0);	//in seconds
//
//	if ((date1 != date0) && (dDiffTime == 0))
//	{
//		return (BI_ERROR);
//	}
//	else
//	{
//		long lDiffTime = (long)(dDiffTime/(24*60*60));	//in days
//		return(lDiffTime);
//	}
//}
//
//// This function assumes the second input is DATE types in 4 bytes.
//// | days of month ([1,31] in 2 bytes) | month ([1,12] in 1 byte) | year ([0, 255] in 1 byte) |
//long CHart_Builtins::days_to_DATE( long days, long date0 )
//{
//	struct tm StDateNTime;	
//	if (get_tm(&StDateNTime, date0) == BI_ERROR)
//	{
//		return BI_ERROR;
//	}
//	StDateNTime.tm_mday += days;
//
//	time_t time0 = mktime( &StDateNTime);
//
//	//convert time_t structure back to tm structure in local time zone
//	struct tm *StDateNTime0;
//	StDateNTime0 = gmtime(&time0);
//
//	long futureDate = ((StDateNTime0->tm_mday << 16) & 0xFFFF0000) | (((StDateNTime0->tm_mon + 1) << 8) & 0xFF00) | (StDateNTime0->tm_year & 0xFF);
//	return futureDate;
//}
//
////This builtin creates a time_t value from the DATE and TIME_VALUE
//long CHart_Builtins::From_DATE_AND_TIME_VALUE( long date, unsigned long time_value )
//{
//	struct tm timeinfo;
//	if (get_tm(&timeinfo, date) == BI_ERROR)
//	{
//		return BI_ERROR;
//	}
//	// Fill tm structure  
//	long sec = time_value / 32000;
//	timeinfo.tm_sec = sec;
//
//	// call mktime to create time_t type  
//	time_t returnedVal;
//	returnedVal = mktime (&timeinfo);
//	return make_time_t( returnedVal );
//}
//
////This builtin creates a time_t from the TIME_VALUE
//long CHart_Builtins::From_TIME_VALUE( unsigned long time_value )
//{
//	time_t returnedVal;
//
//	//get current time
//	time( &returnedVal );
//	returnedVal += (time_value / 32000);
//	return make_time_t( returnedVal );
//}
//
//unsigned long CHart_Builtins::seconds_to_TIME_VALUE(double seconds)
//{
//	return ((unsigned long)(seconds * 32000));
//}
//double CHart_Builtins::TIME_VALUE_to_seconds(unsigned long time_value)
//{
//	return ((double)time_value / 32000);
//}
//
//int CHart_Builtins::TIME_VALUE_to_Hour(unsigned long time_value)
//{
//	int returnVal;
//
//	returnVal = (int) ((float)time_value / (60 * 60 * 32000));
//	return (returnVal);
//}
//int CHart_Builtins::TIME_VALUE_to_Minute(unsigned long time_value)
//{
//	int returnVal;
//
//	returnVal = (int) ((float)time_value / (60 * 32000));
//	return (returnVal);
//}
//int CHart_Builtins::TIME_VALUE_to_Second(unsigned long time_value)
//{
//	int returnVal;
//
//	returnVal = (int) ((float)time_value / 32000);
//	return (returnVal);
//}
//
//int CHart_Builtins::DATE_AND_TIME_VALUE_to_string(wchar_t* output_str, wchar_t* format, long date, unsigned long time_value)
//{
//	struct tm StDateNTime;	
//	if (get_tm(&StDateNTime, date) == BI_ERROR)
//	{
//		return BI_ERROR;
//	}
//	
//	StDateNTime.tm_sec = (time_value)/32000;
//
//	//update tm in stantdard format for display
//	(void)mktime(&StDateNTime);
//	int returnVal = wcsftime(output_str, MAX_DD_STRING, format, &StDateNTime);
//
//	return (returnVal);
//}
//
//int CHart_Builtins::DATE_to_string(wchar_t* output_str, wchar_t* format, long date)
//{
//	struct tm StDateNTime;	
//	if (get_tm(&StDateNTime, date) == BI_ERROR)
//	{
//		return BI_ERROR;
//	}
//
//	int returnVal = wcsftime(output_str, MAX_DD_STRING, format, &StDateNTime);
//
//	return (returnVal);
//}
//
//int CHart_Builtins::TIME_VALUE_to_string(wchar_t* time_value_str, wchar_t* format, unsigned long time_value)
//{
//	struct tm StDateNTime;	
//	
//	StDateNTime.tm_wday = 0;
//	StDateNTime.tm_yday = 0;
//	StDateNTime.tm_year =  72;	//Jan. 1, 1972
//	StDateNTime.tm_mon =   0;
//	StDateNTime.tm_mday =  1;
//	StDateNTime.tm_hour = 0;
//	StDateNTime.tm_min = 0;
//	StDateNTime.tm_sec = (time_value)/32000;
//	StDateNTime.tm_isdst = -1;	//daylight saving time is unknown
//
//	//update tm in stantdard format for display
//	(void)mktime(&StDateNTime);
//	int returnVal = wcsftime(time_value_str, MAX_DD_STRING, format, &StDateNTime);
//
//	return (returnVal);
//}
//int CHart_Builtins::timet_to_string(wchar_t* timet_str, wchar_t* format, long timet_value)
//{
//	time_t time_t_val = get_time(timet_value);
//
//	//get the time in tm struct
//	struct tm *StDateNTime = gmtime(&time_t_val);
//	int returnVal = wcsftime(timet_str, MAX_DD_STRING, format, StDateNTime);
//
//	return (returnVal);
//}
//
//// The Builtin timet_to_TIME_VALUE converts the time of day part of a time_t to a TIME_VALUE(4).
//unsigned long CHart_Builtins::timet_to_TIME_VALUE(long timet_value)
//{
//	//convert time_t structure to tm structure in local time zone
//	time_t inputTime = get_time(timet_value);
//	struct tm *StDateNTime;
//	StDateNTime = gmtime(&inputTime);
//
//	//output is number of 1/32 ms since midnight
//	unsigned long time_value = 32000 * ((StDateNTime->tm_hour * 60 + StDateNTime->tm_min) * 60 + StDateNTime->tm_sec);
//
//	return (time_value);
//}
//
//unsigned long CHart_Builtins::To_TIME_VALUE(int hours, int minutes, int seconds)
//{
//	unsigned long time_value = (unsigned long)32000 * (60 * (60 * hours + minutes) + seconds);
//
//	return (time_value);
//}
///***************************End of Date Time Builtins (eDDL)*************/
//
///* End of code 
#endif