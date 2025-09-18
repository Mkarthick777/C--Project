/**********************************************************************************************
FILE:


**********************************************************************************************/


/**********************************************************************************************
** Library Header files
*********************************************************************************************/
#include "pch.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

//jad vs2017 ios	pw - apparently apple doesn't malloc...it works
#ifndef APPLE
#include <malloc.h>
#endif

#ifdef _DEBUG
 #include <assert.h> 
#else /* release */
//// #ifndef assert
////  #define assert(f)
////  #warning "Release defined 'assert' in ddbConditional.h"
//// #endif
#endif

#include <assert.h>
/**********************************************************************************************
** User Header files
*********************************************************************************************/
#pragma warning (disable : 4786)
#include "ddbdevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "RTN_CODE.h"
#include "DDBitemBase.h"
#include "ddldefs.h"
#include "DDBitems.h"
/*
#include "ddbitemlists.h"
#include "ddbdevice.h"
#include "ddbdevicemgr.h"	*/

#include <ctype.h>  // to get toupper
/**********************************************************************************************
** Defines
*********************************************************************************************/

//in dictionary.h   #define COUNTRY_CODE_MARK 	'|'
//in dictionary.h   #define DEF_LANG_CTRY		"|en|"	/* Of the form "|LL|" or "|LL CC|" */
#define ASSERT_ALL(cond) \
		if(!(cond)) panic("%s(%d) - Condition not true\n", __FILE__, __LINE__)

/**********************************************************************************************
** Global Functions
*********************************************************************************************/
int process_prompt(char *, long *);
//(void) display_message(message, param_ids, member_ids,id_count);
long delayfor (long , char *, ULONG *, ULONG *, long );


//int bltin_format_string (char *, int , char *,long *);

/***********************************************************************************
Description:The function process_prompt checks whether the syntax is right like any mising [ or }
  
*************************************************************************************/

#if 0 // we use the dictionary's version now..stevev 10aug13
/*********************************************************************
 *
 *	Name: ddi_get_string_translation
 *	ShortDesc: return the requested translation of the string
 *
 *	Description:
 *		ddi_get_string_translation will take the given string, which is
 *		composed of phrases, and using the language/country code, extract 
 *		and copy the correct string into the output buffer
 *
 *	Inputs:
 *		string:			a pointer to the string to search for the
 *						correct translation
 *		lang_cntry:		a pointer to the language/country code string
 *		outbuf_size:	the size of the output buffer, in characters
 *
 *	Outputs:
 *		outbuf:			a pointer to the output buffer with the correct
 *						translation loaded
 *
 *	Returns:
 *		DDS_SUCCESS, DDI_INSUFFICIENT_BUFFER
 *
 *	Author: Vern Reinhardt
 *
 **********************************************************************/

int
ddi_get_string_translation(tchar *string, tchar *lang_cntry, tchar *outbuf,
		int outbuf_size)
{
	tchar	*ci=NULL;			/* input character pointer */
	tchar	*co=NULL;			/* output character pointer */
	tchar	*first_phrp=NULL;		/* first phrase pointer */
	tchar	*lang_cntry_phrp=NULL;	/* language + country phrase pointer */
	tchar	*lang_only_phrp=NULL;	/* language-only phrase pointer */
	tchar	lang_only[5]={0};	/* language-only pulled from language/country code */
	tchar	*new_def_phrp=NULL;	/* new-style default phrase pointer */
	tchar	*old_def_phrp=NULL;	/* old-style language-only phrase pointer */
	tchar	*out_phrp=NULL;		/* output phrase pointer */
	int		code_length=0;	/* length of language/country code, in characters */


	/*
	 *	If the input string is a null string (which is legal), we can
	 *	skip all the string processing and return the output string,
	 *	which we set to a null string.
	 */
	if (string[0] == _T('\0') ) 
	{
		outbuf[0] = _T('\0');
		return(DDS_SUCCESS);
	}

	/*
	 *	If the input country_code is full-size (i.e., seven characters),
	 *	extract the language code from the language/country code.
	 *	Otherwise, make the language-only code a null string.
	 */
	if (_tstrlen(lang_cntry) == 7)
	{
		(void)_tstrncpy(lang_only, lang_cntry, (size_t)3);
		lang_only[3] = COUNTRY_CODE_MARK;
		lang_only[4] = _T('\0');
	} 
	else 
	{
		lang_only[0] = _T('\0');
	}

	/*
	 *	Check to see if the input string begins with a COUNTRY_CODE_MARK.
	 *	If it does not, set the first-phrase pointer, then enter the loop.
	 */
	if (string[0] != COUNTRY_CODE_MARK)
	{
		first_phrp = outbuf;
	}

	/*
	 *	The Loop:
	 *		On a character-by-character basis, check for any of the
	 *	possible language or language/country codes, or escape sequences.
	 *	Look for the specified language/country code in the input string
	 *	in this order:
	 *
	 *		- the complete language/country code
	 *		- the language-only code (new style)
	 *		- the language-only code (old style)
	 *
	 *	If one of the language/country codes matches, and the corresponding
	 *	phrase pointer is not yet set, save the address of that phrase.  In
	 *	any case that a substring in the form of a language/country code is
	 *	found, even if it's not one we're looking for, insert an end-of-string
	 *	character in the output buffer, then move the input string pointer
	 *	beyond the language/country code.  If no language/country code is
	 *	found, look for escape sequences.  Do this this until the input
	 *	string's end-of-string is encountered.
	 */

	for (co = outbuf, ci = string; *ci; ci++)
	{

	/*
	 *	Look for the complete language/country code.
	 */
		if ((ci[0] == COUNTRY_CODE_MARK) && tisalpha(ci[1]) &&
				tisalpha(ci[2]) && (ci[3] == _T(' ')) && tisalpha(ci[4])
				&& tisalpha(ci[5]) && (ci[6] == COUNTRY_CODE_MARK)) 
		{
			code_length = 7;

			if ((lang_cntry_phrp == 0) &&
					(_tstrncmp(ci, lang_cntry, code_length) == 0))
			{
				lang_cntry_phrp = co + 1;
			}

			if ((new_def_phrp == 0) &&
					(_tstrncmp(ci, DEF__LANG__CTRY, code_length) == 0))
			{
				new_def_phrp = co + 1;
			}

			if (first_phrp == 0)
			{
				first_phrp = co + 1;
			}

			*co++ = _T('\0');
			ci += (code_length - 1);

	/*
	 *	Look for the language-only code (new style).
	 */
		}
		else if ((ci[0] == COUNTRY_CODE_MARK) && tisalpha(ci[1]) &&
				tisalpha(ci[2]) && (ci[3] == COUNTRY_CODE_MARK))
		{
			code_length = 4;

			if ((lang_cntry_phrp == 0) &&
					(_tstrncmp(ci, lang_cntry, code_length) == 0)) {
				lang_cntry_phrp = co + 1;
			}

			if ((lang_only_phrp == 0) && (lang_only[0] != _T('\0')) &&
					(_tstrncmp(ci, lang_only, code_length) == 0)) {
				lang_only_phrp = co + 1;
			}

			if ((new_def_phrp == 0) &&
					(_tstrncmp(ci, DEF__LANG__CTRY, code_length) == 0)) {
				new_def_phrp = co + 1;
			}

			if (first_phrp == 0) {
				first_phrp = co + 1;
			}

			*co++ = _T('\0');
			ci += (code_length - 1);

	/**	Look for the language-only code (old style); default only. */

		}
		else if ((ci[0] == COUNTRY_CODE_MARK) && tisdigit(ci[1]) &&
				tisdigit(ci[2]) && tisdigit(ci[3]))
		{
			code_length = 4;

			if ((old_def_phrp == 0) &&
					(_tstrncmp(ci, _T("|001"), code_length) == 0)) {
				old_def_phrp = co + 1;
			}

			if (first_phrp == 0) {
				first_phrp = co + 1;
			}

			*co++ = _T('\0');
			ci += (code_length - 1);

	/*
	 *	If the escape sequence character (\) is encountered, convert
	 *	the following character as required.  These are the escape
	 *	sequences required by the DDL Spec.
	 */

		}
		else if (*ci == _T('\\'))
		{

			switch (*(ci + 1)) {

				case _T('a'):
					*co++ = _T('\a');
					ci++;
					break;

				case _T('f'):
					*co++ = _T('\f');
					ci++;
					break;

				case _T('n'):
					*co++ = _T('\n');
					ci++;
					break;

				case _T('r'):
					*co++ = _T('\r');
					ci++;
					break;

				case _T('t'):
					*co++ = _T('\t');
					ci++;
					break;

				case _T('v'):
					*co++ = _T('\v');
					ci++;
					break;

				default:
					/* VMKP commented on 030404 */
					*co++ = *ci;
					//*co++ = *(ci + 1);
					//ci++;
					break;
			}

	/**	This is the 'normal' case; this character has no special
	 *	significance, so just copy it to the output pointer. */
		}
		else
		{
			*co++ = *ci;
		}
	}

	/**	Tack an end-of-string character onto the final phrase. */
	*co++ = _T('\0');

	/**	We may have found a phrase to output.  Copy the highest priority
	 *	phrase into the holding buffer.  Priority is determined in this 
	 *	order, depending upon which string pointers have been assigned 
	 *	non-null values:
	 *
	 *		- the phrase specified by the complete language/country code,
	 *		- the phrase specified by just the language in the
	 *		  language/country code,
	 *		- the phrase specified by the new-style default
	 *		  language/country code,
	 *		- the phrase specified by the old-style default
	 *		  language/country code,
	 *		- the first phrase encountered in the input string.
	 */

	if (lang_cntry_phrp)
	{
		out_phrp = lang_cntry_phrp;
	}
	else if (lang_only_phrp)
	{
		out_phrp = lang_only_phrp;
	}
	else if (new_def_phrp)
	{
		out_phrp = new_def_phrp;
	}
	else if (old_def_phrp)
	{
		out_phrp = old_def_phrp;
	}
	else
	{
		out_phrp = first_phrp;
	}

	/**	Check the length of the output buffer.  If the phrase to be output
	 *	is longer than the output buffer, return an error code.  Otherwise,
	 *	copy the phrase in the holding buffer into the output buffer.  	 */
	if ((size_t) outbuf_size < _tstrlen(out_phrp))
	{
		outbuf[0] = _T('\0');
		return DDI_INSUFFICIENT_BUFFER;
	}
	else
	{
		(void)_tstrcpy(outbuf, out_phrp);
	}

	return DDS_SUCCESS;
}
#endif // 0 ---------------------------------------------------------------
/**********************************************************************
 *
 *  Name: find_lang_string
 *  ShortDesc: Finds the current language string in a text string
 *
 *  Description:
 *      Locates the part of a text string associated with the
 *      language/country code "|en|".  A pointer to the located string
 *      is returned.  This pointer is a pointer into the original string,
 *      i.e., no data is ever moved.  In order to return a null terminated
 *      string, a null byte may be written into the middle of the original
 *      string.  Therefore, the original string is probably not useful,
 *      except when referenced via the the pointer return by this function.
 *
 *		This function calls "ddi_get_string_translation".
 *
 *  Inputs:
 *		text - The text to be examined.
 *
 *  Outputs:
 *		None.
 *
 *  Returns:
 *		A pointer to the requested language string within the provided
 *		text.
 *
 *	NOTE: This routine could be replaced in its entirety by a call to 
 *	ddi_get_string_translation, but that would entail changing dozens
 *	of calls to find_lang_string.
 *
 *  Author:
 *		Vern Reinhardt
 *
 *********************************************************************  /

tchar *
find_lang_string (tchar *text)
{
	int			rc;

	//
	//	If the provided text string has a valid phrase, return it.
	//	Otherwise, return a null string.
	//
	rc = m_pDevice->dictionary->get_string_translation(text,text,_tstrlen(text) + 1);
	// using a unique function for this seems ludicrous
	//rc = ddi_get_string_translation(text, _T("|en|"), text, _tstrlen(text) + 1);
	if (rc != DDS_SUCCESS)
	{
		text[0] = _T( '\0' );
	}

	return (text);
}
***************************************************************************/

/**********************************************************************
 *
 *  Name: bltin_format_string
 *  ShortDesc: Format a string into the specified buffer
 *
 *  Include:
 *		ddsbltin.h
 *
 *  Description:
 *		Bltin_format_string takes the supplied prompt string, strips out the
 *		current language version, and parses the resulting prompt.  This
 *		parsing means putting the taking the printf-like prompt format, and
 *		inserting param values or labels in the appropriate places.
 *
 *		The type of variable is being checked against the assumed type of 
 *		variable based on the %[] string.  For instance:  if the user specifies
 *		a printf-like string of "%s" but is using a variable of type FLOATG_PT,
 *		an error will be returned.
 *
 *		Types DATE_AND_TIME, TIME, TIME_VALUE, and DURATION all use a default
 * 		printf format.  The user's format is always ignored.
 *
 *  Inputs:
 *		max_length - The length of the output buffer.
 *		passed_prompt - The prompt to strip and parse.
 *		param_ids - The array of param IDs used to parse the prompt.
 *		subindices - The array of param subindices used to parse
 *					the prompt.
 *		env_info - The ENV_INFO structure containing information
 *						about the current request.
 *
 *  Outputs:
 *		out_buf - The buffer to put the formatted string.
 *
 *  Returns:
 *		Any of the error returns specified in builtins.h &/| DDParser/Retn_Code.h
 *
 *  Author:
 *		Bruce Davis
 *   Re-written 10aug07 - stevev
 *
 *********************************************************************/
#if 1
//                in & out        in & out             in           in
int CopyToOutbuf(tchar*& dest,int &availChars,  tchar* source, int srcLen = -1)//-1 is copy till '\0'
{
	int r = BLTIN_SUCCESS;

	if ( dest == NULL || source == NULL )
	{
		SDCLOG(CERR_LOG,"Method Format Error: CopyToOutbuf: Empty Parameter!\n");
		return BLTIN_INVALID_PROMPT;
	}

	if (srcLen < 0)		
	{
		srcLen = _tstrlen (source);
/*
LOGIT(CLOG_LOG,"** CopyToOutbuf source = %s",source);
LOGIT(CLOG_LOG,"** CopyToOutbuf source[0] = %d",source[0]);
LOGIT(CLOG_LOG,"** CopyToOutbuf source[1] = %d",source[1]);
LOGIT(CLOG_LOG,"** CopyToOutbuf source[2] = %d",source[2]);
LOGIT(CLOG_LOG,"** CopyToOutbuf source[3] = %d",source[3]);
LOGIT(CLOG_LOG,"** CopyToOutbuf source[4] = %d",source[4]);
LOGIT(CLOG_LOG,"** CopyToOutbuf source[5] = %d",source[5]);
LOGIT(CLOG_LOG,"** CopyToOutbuf source[6] = %d",source[6]);
LOGIT(CLOG_LOG,"** CopyToOutbuf source[7] = %d",source[7]);

LOGIT(CLOG_LOG,"** CopyToOutbuf srcLen = %d", srcLen);
*/

	}
	if (srcLen > availChars) 
	{// buffer full
		r = BLTIN_BUFFER_TOO_SMALL;
#ifdef _UNICODE
		SDCLOG(CERR_LOG,"Method Format Error: Buffer too small for %S.\n",source);
#else
		SDCLOG(CERR_LOG,"Method Format Error: Buffer too small for %s.\n",source);
#endif
	}
	else
	{//	leave r = SUCCESS-copy over terminating null
#ifdef MODIFIED_BY_SS
		//(void)_tstrncpy(dest, source, srcLen);
		std::wstring strTemp(source);
		std::copy(strTemp.begin(), strTemp.end(), dest);
#endif
		// Null-terminate the destination string
		dest[srcLen] = _T('\0');

		// Update dest pointer and availChars count
		dest += srcLen;
		availChars -= srcLen;
		
		/*dest += (srcLen);
		availChars -= srcLen;*/
#endif 
		
	}

	return r;
}

/*  doFormat rolls the format string into the the value
		basically:  sprintf(retStr,formatStr,vValue);  with some bells and whistles
    pV isn't used at this time (I think we may have to later)   

	returns true on there was an error,  false at SUCCESS

	// stevev - 02jun14 - 
*/
bool CHart_Builtins::
     doFormat(tchar* formatStr, hCVar* pV,CValueVarient& vValue,tchar* retStr,int rsLen)
{
	if (retStr == NULL)
	{
		return true;
	}
	if ( formatStr == NULL || formatStr[0] == _T('\0'))
	{
		wstring t = (wstring)vValue;
		if ( (int)t.size() < rsLen )
		{
#ifdef MODIFIED_BY_SS
			//_tstrcpy(retStr,t.c_str());
			std::wstring strTemp(t.c_str());
			std::copy(strTemp.begin(), strTemp.end(), retStr);

#endif 
		}
		else
		{
#ifdef MODIFIED_BY_SS
			//_tstrncpy(retStr,t.c_str(), rsLen - 1);
			std::copy(t.begin(), t.end(), retStr);
#endif
			retStr[rsLen-1] = _T('\0');
		}
		return false; // works good
	}
	// else we have some unique formatting, we'll have to deal with it

	tchar* pch    =  _tstrchar( formatStr, _T('%') );
	tchar theChar = _T('\0');
	if ( pch != NULL ) // real formatting
	{//	no spaces allowed in formatting, so get the last char
		theChar = *pch;// start with the '%'
		while ( pch )
		{
			pch++;
			if (*pch == _T('\0') || *pch == _T(' ') || *pch == _T('\t') || *pch == _T('\n'))
				break;
			//else
			theChar = *pch;// we want the last char
		}
	}// else - leave cahr at null to get to default

	switch (theChar)
	{
	case 'c':
	case 'C':
#ifdef MODIFIED_BY_SS
	{
		// Convert char value to a wide string
		char charValue = static_cast<char>(vValue);
		std::wstring wideValue(1, charValue);
		//std::wstring wideValue(1, vValue);

		// Format the wide string
		std::wstringstream ss;
		ss << formatStr << wideValue;
		std::wstring result = ss.str();

		// Copy the result to retStr, ensuring the buffer is not overflowed
		wcsncpy_s(retStr, rsLen, result.c_str(), _TRUNCATE);
		retStr[rsLen - 1] = L'\0'; // Ensure null termination
		//WIDE_SWPRINTF(retStr,rsLen,formatStr,(char)vValue);
	}
#endif		
		break;
	case 'd':
	case 'i':
#ifdef MODIFIED_BY_SS
	{
		// Convert char value to a wide string
		std::wstring wideValue = std::to_wstring(static_cast<long long>(vValue));

		// Format the wide string
		std::wstringstream ss;
		ss << formatStr << wideValue;
		std::wstring result = ss.str();

		// Copy the result to retStr, ensuring the buffer is not overflowed
		wcsncpy_s(retStr, rsLen, result.c_str(), _TRUNCATE);
		retStr[rsLen - 1] = L'\0'; // Ensure null termination
		//WIDE_SWPRINTF(retStr,rsLen,formatStr,(char)vValue);
	}
#endif
		break;
	case 'o':
	case 'u':
	case 'x':
	case 'X':
	{
#ifdef MODIFIED_BY_SS
		//WIDE_SWPRINTF(retStr,rsLen,formatStr,(unsigned int)vValue);
		std::wstring wideValue = std::to_wstring(static_cast<unsigned int>(vValue));

		// Format the wide string
		std::wstringstream ss;
		ss << formatStr << wideValue;
		std::wstring result = ss.str();

		// Copy the result to retStr, ensuring the buffer is not overflowed
		wcsncpy_s(retStr, rsLen, result.c_str(), _TRUNCATE);
		retStr[rsLen - 1] = L'\0'; // Ensure null termination
#endif
	}
		break;
	case 'e':
	case 'E':
	case 'f':
	case 'g':
	case 'G':
		if (vValue.vIsDouble)
		{
	
#ifdef MODIFIED_BY_SS
			double d = (double)vValue;
			//WIDE_SWPRINTF(retStr,rsLen,formatStr,(unsigned int)vValue);
			std::wstring wideValue = std::to_wstring(static_cast<double>(vValue));

			// Format the wide string
			std::wstringstream ss;
			ss << formatStr << wideValue;
			std::wstring result = ss.str();

			// Copy the result to retStr, ensuring the buffer is not overflowed
			wcsncpy_s(retStr, rsLen, result.c_str(), _TRUNCATE);
			retStr[rsLen - 1] = L'\0'; // Ensure null termination
#endif
		}
		else
		{
			//float y = (float)vValue;
			//WIDE_SWPRINTF(retStr,rsLen,formatStr,y);
#ifdef MODIFIED_BY_SS
			float d = (float)vValue;
			//WIDE_SWPRINTF(retStr,rsLen,formatStr,(unsigned int)vValue);
			std::wstring wideValue = std::to_wstring(static_cast<float>(vValue));

			// Format the wide string
			std::wstringstream ss;
			ss << formatStr << wideValue;
			std::wstring result = ss.str();

			// Copy the result to retStr, ensuring the buffer is not overflowed
			wcsncpy_s(retStr, rsLen, result.c_str(), _TRUNCATE);
			retStr[rsLen - 1] = L'\0'; // Ensure null termination
#endif
		}
		break;
	case 's':
	case 'S':
	{
		//	WIDE_SWPRINTF(retStr,rsLen,formatStr, ((wstring)vValue).c_str() );
			//jad does this work?
#ifdef MODIFIED_BY_SS

		//WIDE_SWPRINTF(retStr,rsLen,formatStr,(unsigned int)vValue);
		std::wstring wideValue = ((wstring)vValue).c_str();

		// Format the wide string
		std::wstringstream ss;
		ss << formatStr << wideValue;
		std::wstring result = ss.str();

		// Copy the result to retStr, ensuring the buffer is not overflowed
		wcsncpy_s(retStr, rsLen, result.c_str(), _TRUNCATE);
		retStr[rsLen - 1] = L'\0'; // Ensure null termination
#endif
	}
		break;
	case '%':
	default:
	{
#ifdef MODIFIED_BY_SS
		//WIDE_SWPRINTF(retStr,rsLen,formatStr);
		//wcsncpy(retStr, formatStr.c_str(), rsLen);
		std::wstringstream ss;
		ss << formatStr;
		std::wstring result = ss.str();

		// Copy the result to retStr, ensuring the buffer is not overflowed
		wcsncpy_s(retStr, rsLen, result.c_str(), _TRUNCATE);
		retStr[rsLen - 1] = L'\0'; // Ensure null termination
#endif
	}
		break;
	}
	return false; // no error
}
							 



#define INTERNAL_BUFFER_SIZE  (1024*3)  /* long for wide char sets */

int CHart_Builtins::											//Modified by ANOOP 17FEB2004
bltin_format_string (tchar *out_buf, int max_length,    updatePermission_t updateLevel,
					 tchar *passed_prompt,			   long *glob_var_ids, int nCnt_Ids,
					 CValueVarient *pDynamicVarValues, bool & bDyanmicVarvalChanged)  
{
	int				retCode = BLTIN_SUCCESS;
	int				remainingOut = max_length,  copy_length = 0,    paramIdx    = -1;
	bool			isSpecialDyn = false;// tells the one we are working with is a special %n
	tchar			*prompt_buf  = NULL ,		*prompt		= NULL, *curr_ptr	= NULL;
	tchar			*out_ptr     = NULL;

	tchar			temp_buf[INTERNAL_BUFFER_SIZE];// bigger to handle multibyte chars
	tchar			curr_format[20],	curr_param[256] = {0};
	tchar			prompt_char = 0;
	
	hCitemBase		*pIB = NULL;
	hCVar           *pVar= NULL;
	CValueVarient	ReturnedDataItem;
	INTER_VARIANT	VariantVarVal;

	bool            updateDynamic =false;//tells if we are in the update state(==enabledynamic)
	wstring			curr_label, tempValue;	
	int				nDynVarCntr = (pDynamicVarValues)?0:-1;// add to prevent filling an empty

	bDyanmicVarvalChanged = false;
/*
LOGIT(CLOG_LOG,"** out_buf,passed_prompt = %s, %s",out_buf,passed_prompt);

LOGIT(CLOG_LOG,"** remainingOut = %d",remainingOut);

LOGIT(CLOG_LOG,"** passed_prompt = %s",passed_prompt);
LOGIT(CLOG_LOG,"** passed_prompt[0] = %d",passed_prompt[0]);
LOGIT(CLOG_LOG,"** passed_prompt[1] = %d",passed_prompt[1]);
LOGIT(CLOG_LOG,"** passed_prompt[2] = %d",passed_prompt[2]);
LOGIT(CLOG_LOG,"** passed_prompt[3] = %d",passed_prompt[3]);
LOGIT(CLOG_LOG,"** passed_prompt[4] = %d",passed_prompt[4]);
LOGIT(CLOG_LOG,"** passed_prompt[5] = %d",passed_prompt[5]);
LOGIT(CLOG_LOG,"** passed_prompt[6] = %d",passed_prompt[6]);
LOGIT(CLOG_LOG,"** passed_prompt[7] = %d",passed_prompt[7]);
LOGIT(CLOG_LOG,"** passed_prompt[8] = %d",passed_prompt[8]);
LOGIT(CLOG_LOG,"** passed_prompt[9] = %d",passed_prompt[9]);
LOGIT(CLOG_LOG,"** passed_prompt[10] = %d",passed_prompt[10]);
*/
	//#update_dynamics - passed in (depends on builtin function at this time)
	//#find_lang_string()	
		//Find the appropriate country code string in the prompt string.
	prompt_buf = (tchar *) malloc((_tstrlen(passed_prompt)+1) * sizeof(tchar));

	tchar			*hold_buf	= prompt_buf; //to be sure we free what we alloc (prompt_buf can move)
	LOGIT(CLOG_LOG,"bltin_format_string: prompt_buf newly allocated '%p'\n",prompt_buf);

	//jad dvc
	Indentity_t  idDevice;
	idDevice = m_pDevice->getIdentity();

	if (prompt_buf == NULL)
	{
		*out_buf = _T( '\0' );
		return BLTIN_NO_MEMORY;	//#exit if null returned - nothing to work with -	
	}
#ifdef MODIFIED_BY_SS	
	//(void)_tstrcpy (prompt_buf, passed_prompt);
	size_t len = std::wcslen(prompt);

	// Copy passed_prompt to prompt_buf
	std::copy(passed_prompt, passed_prompt + len + 1, prompt_buf);
#endif 
	// 
	// 
	//prompt = find_lang_string (prompt_buf);// copies the string to prompt_buf from internal loc
	// use the dictionary version... no need for 5 versions of this
	// stevev 13mar14 - change the parameters to match find_lang_string
	prompt = prompt_buf;
//LOGIT(CLOG_LOG,"bltin_format_string: getting string translation\n");
	int rc = 
	 m_pDevice->dictionary->get_string_translation(prompt_buf,prompt,_tstrlen(prompt_buf) + 1);
	if ( rc != SUCCESS )
	//if (prompt == NULL)
	{
//LOGIT(CLOG_LOG,"bltin_format_string: string translation returned an error\n");
//sjv 30jun14		free (prompt_buf);
		free(hold_buf);
		*out_buf = _T( '\0' );
		return BLTIN_NO_LANGUAGE_STRING;	//#exit if null returned - nothing to work with -
	}
//LOGIT(CLOG_LOG,"bltin_format_string: post string translation return\n");

	out_ptr = out_buf;

	//jad dvc
	bool bParameterMatch;

	//#for - ever	
	while ( retCode == BLTIN_SUCCESS )// stop on error code
	{
	//#	clear vars as required for the loop	 strings					
		temp_buf[0]   = _T('\0');
		isSpecialDyn  = false;
		updateDynamic = false;
		paramIdx      = -1;
		curr_format[0]=
		curr_param[0] = _T('\0'); // to make Walt happy..

	//#		move short circuit inside loop so we get the language string reguardless
		curr_ptr = _tstrchar(prompt, _T('%'));
	//#	if no more '%'	
		if (curr_ptr == NULL)
		{
	//#		if  !  Copy2Output()	--- no more %, copy the rest to output and leave
			//strcat(out_buf,prompt);
			retCode = CopyToOutbuf(out_ptr,remainingOut,  prompt) ;
			// retCode determines success or failure, we're leaving reguardless
			break;
		}// else we have a '%' - so process it												
	//#
	//#	if ! Copy2Output(  current location, to the '%' char )	
	//	assert("*curr_ptr == '%'");
		copy_length = curr_ptr - prompt;// inherently copies upto but not including '%'

//LOGIT(CLOG_LOG,"** copy_length = %d",copy_length);

		if( copy_length > 0 )
		{
			if(retCode =CopyToOutbuf(out_ptr,remainingOut,prompt,copy_length))//testing retcode
			{// probable full buffer
				break;// error exit
			}	
		}
		prompt = curr_ptr + 1;
		prompt_char = *prompt;	
		
		/*
		 *	Handle the formatting.  The formatting consists of:
		 *
		 *	%[format]{param ID array index} for a device param
		 *	%[format]{method-local param name} for a local paramparam
		 *	%[format]{complex-Var-reference} for a attributes and the like
		 *  %[format]n  when n is 0 to 9 id index (update dynamic if allowed)
		 *
		 *	The format is optional, and consists of standard scanf
		 *	format string, with the addition of 'L', which specifies
		 *	the label of the param and 'U' which is the Units 
		 * stevev 23feb11                                   and ',D' for isDynamic
		 */
	//# acquire the formatting

		if (prompt_char == _T('['))	//#	if nextChar == '[' // whitespace not allowed	
		{	/**	Capture the format string.	 */

			prompt++; // skip the '['
	//#		get following ']'		
			curr_ptr = _tstrchar (prompt, _T(']'));
			if (curr_ptr == NULL || curr_ptr == prompt)
			{											
	//#		exit if not found		
				retCode = BLTIN_INVALID_PROMPT;
				SDCLOG(CERR_LOG,"Method Format Error: format's closing ']' not found.\n");
				break; // exit 
			}
			copy_length = curr_ptr - prompt;											
	//#		curr_format  =  "%"  + string between '[' & ']'			
			*curr_format = _T('%');
#ifdef MODIFIED_BY_SS
			//(void)_tstrncpy (&(curr_format[1]), prompt, copy_lengthint len = _tcslen(prompt);

// Copy prompt to curr_format starting from the second character
			std::copy(prompt, prompt + copy_length, curr_format + 1);

#endif
			curr_format[copy_length+1] = _T('\0');
			prompt = curr_ptr + 1;
			prompt_char = *prompt;
		} 
		else
		{										
	//#	else curr_format == MT string		
			curr_format[0] = _T('\0');
		}
		
	//#
	//#	if nextChar == '{'  // whitespace not allowed			
		if (prompt_char == _T('{'))
		{/**	Get the param string. */			
			prompt++;		// skip the '{'
	//#		get following '}'		
			curr_ptr = _tstrchar (prompt, _T('}'));
			if (curr_ptr == NULL)
			{											
	//#		exit if not found				
				retCode = BLTIN_INVALID_PROMPT;
				SDCLOG(CERR_LOG,"Method Format Error: param's closing '}' not found.\n");
				break; // leave the loop & return
			}
			copy_length = curr_ptr - prompt;								
	//#		curr_param  =  string between '{' & '}'	
	// 
#ifdef MODIFIED_BY_SS
			//(void)_tstrncpy(curr_param, prompt, copy_length);
			//int len = _tcslen(prompt);
			size_t len = std::wcslen(prompt);
			// Copy up to copy_length characters from prompt to curr_param
			std::copy(prompt, prompt + copy_length, curr_param);

			// Ensure null-termination
			if (copy_length < len) {
				curr_param[copy_length] = _T('\0');
			}


#endif	
			

			curr_param[copy_length] = _T('\0');
			prompt = curr_ptr + 1;
			prompt_char = *prompt;
		} 												
	//#	else														
	//#	if nextChar == numeric  // whitspace not allowed - single digit only
		else if (isdigit (prompt_char))
		{	/**	Special case of param string... %X is same as %{X}
			 	where X is a single digit number.		 */				
	//#		curr_param = one digit + '\0'		
			curr_param[0] = prompt_char;
			curr_param[1] = 0;
			prompt++;
			prompt_char = *prompt;
					
			isSpecialDyn = true;// this is a special that is eligible for dynamic updates
			// it will ONLY be updated if it is Dynamic and up_UPDATE_SPEC_DYN has been set
		}											
	//#	else														
	//#	if got no format string	
		else if(*curr_format == _T('\0'))
		{													
	//#		copy '%' to out_buf   - sometimes a percent is just a percent					
			retCode = CopyToOutbuf(out_ptr,remainingOut,(tchar*)  _T("%"), 1) ;
			continue;// will exit if there was an error above...
		}
		else 
	//#	else		// '%' [ stuff ] nothing
		{
			retCode = BLTIN_INVALID_PROMPT;
			SDCLOG(CERR_LOG,"Method Format Error:No parameter after format."
							"(whitespace is not allowed)\n");
	//#		error - exit	
			break;
		}
	//#
	//#	// we have captured 'em both. Now the parameter MUST resolve to an hCVar*  
	//# //		                                            OR a method-local-variable		
	//#
	//#	if curr_par == isdigit

		//jad moved to here
#ifdef linux    //can't get it to work thru the char.h macros
		paramIdx = wcstol(curr_param, 0, 0);
#else
		paramIdx = ttoi(curr_param);
#endif
		//#		do range check - exit on error	
		if (glob_var_ids == NULL || paramIdx > (nCnt_Ids - 1))//zero based glob_var_ids array
		{
			retCode = BLTIN_VAR_NOT_FOUND;
			SDCLOG(CERR_LOG, "Method Format Error:Param Index not found in Var-ID array.\n");
			//#		error - exit	
			break;
		}// else - OK, so process it
//#		lookup itemid in idarray
		long itemID = *(glob_var_ids + paramIdx);
		//jad end

		if (isdigit(curr_param[0]))
		{
			//jad moved from here
/*
#ifdef linux    //can't get it to work thru the char.h macros
			paramIdx = wcstol(curr_param ,0 ,0 );
#else
			paramIdx = ttoi( curr_param );
#endif
			
	//#		do range check - exit on error	
			if (glob_var_ids == NULL || paramIdx > (nCnt_Ids-1))//zero based glob_var_ids array
			{				
				retCode = BLTIN_VAR_NOT_FOUND;
			   SDCLOG(CERR_LOG,"Method Format Error:Param Index not found in Var-ID array.\n");
	//#		error - exit	
				break;
			}// else - OK, so process it
	//#		lookup itemid in idarray
			long itemID = *(glob_var_ids + paramIdx);
			*/
			//jad end

	//#		VarPtr = getItemBySymbolNumber()	/* if device param */
			if (m_pDevice->getItemBySymNumber(itemID, &pIB) == SUCCESS && pIB != NULL
	//#		verify it's a var - exit on error	
				&& pIB->IsVariable()  )
			{
				pVar = (hCVar*)pIB;
				if ( m_pDevice->whatCompatability() == dm_Standard )// NOT lenient mode
				{
					if ( ! pVar->IsValid() )
					{
						retCode = BLTIN_CANNOT_READ_VARIABLE;
						SDCLOG(CERR_LOG,"Method Format Error:Trying to display an "
																		"Invalid Variable\n");
						break;// error exit
					}// else it's OK, continue
				}// else - lenient, we don't care
			}
			else // unfound item
			{
				if (passed_prompt == NULL) passed_prompt = (tchar*)_T("");
				retCode = BLTIN_VAR_NOT_FOUND;
				SDCLOG(CERR_LOG,L"Method Format Error:ID array # %d was not found in the DD\n"
					            L"             Prompt:'%s'\n",paramIdx,passed_prompt);
	//#		error - exit	
				break;
			}
		}
	//#	else    // not a digit, it's a (possibly complex) variable reference	
		else
		{
			//jad dvc
			bParameterMatch = false;

			//jad debug
			//force variable 0x4084
			//only for DD 13/09/0107.fm8
			if (idDevice.wManufacturer == 0x13)
			{
				if (idDevice.wDeviceType == 0x09)
				{
					if (idDevice.cDeviceRev == 0x01)
					{
						if (itemID == 0x4046)
						{
							//itemID = 0x4084;
							bParameterMatch = true;
						}
					}
				}
			}
			//check for DD 13/0a/0107.fm8
			if (idDevice.wManufacturer == 0x13)
			{
				if (idDevice.wDeviceType == 0x0a)
				{
					if (idDevice.cDeviceRev == 0x01)
					{
						if (itemID == 0x4046)
						{
							//itemID = 0x4084;
							bParameterMatch = true;
						}
					}
				}
			}

			//check for 13/1309/0301.fm8
			if (idDevice.wManufacturer == 0x13)
			{
				if (idDevice.wDeviceType == 0x1309)
				{
					if (idDevice.cDeviceRev == 0x03)
					{
						if (itemID == 0x4046)
						{
							//itemID = 0x4084;
							bParameterMatch = true;
						}
					}
				}
			}

			//check for 13/130a/0301.fm8
			if (idDevice.wManufacturer == 0x13)
			{
				if (idDevice.wDeviceType == 0x130a)
				{
					if (idDevice.cDeviceRev == 0x03)
					{
						if (itemID == 0x4046)
						{
							//itemID = 0x4084;
							bParameterMatch = true;
						}
					}
				}
			}

			paramIdx = -1;
	//#		interp-- GetVarPtr	
			VariantVarVal.Clear();

			//jad dvc moved
			//pVar = NULL;
			wstring wS(curr_param);
			string   S;
			S = TStr2AStr(wS);

			//jad dvc
			//get the real variable from the DD not the passed variable
			// the passed variable is wrong, it is a constant!!
			if ((S.compare("qa") != 0) || !bParameterMatch)
			{
				pVar = NULL;
				retCode = m_pInterpreter->GetVariableValue((char*)S.c_str(), VariantVarVal, &pVar);
			}
			else
			{
				//get the var from the itemid
				if (m_pDevice->getItemBySymNumber(itemID, &pIB) == SUCCESS && pIB != NULL
					//#		verify it's a var - exit on error	
					&& pIB->IsVariable())
				{
					pVar = (hCVar*)pIB;
					retCode = 1;
				}
				else
					retCode = 0;
			}
			//jad end


			if (! retCode)// false on error - weird but...
			{
				SDCLOG(CERR_LOG,"Method Format Error:Failed to find Variable Value.'%s'\n",
																		  (char *)curr_param);
	//#		error - exit	
				break;
			}
	//#		if it's a DD-var, hCVar* has value	
	//#		else will use INTER_VARIENT to hold the method-local variable's value	
		}


	//# # # # # # # # # # we have the format string and the variable/value # # # # # # # # #

	//#	if  curr_format '%L' || '%U' 
	//#			value_string <<= label or unit	
		if ( curr_format[0] != 0 &&
			( (!_tstrcmp (curr_format, _T("%L")) ) || 
			  (!_tstrcmp (curr_format, _T("%U")) )   ))
		{	/*	Print the label of the param. or flag dynamic*/
			if ( pVar != NULL )
			{	
	//#		if a DD-variable
				if ( !_tstrcmp (curr_format, _T("%L")) )
				{
					pVar->Label(curr_label); 

//jad 2.3
					//(void)WIDE_SWPRINTF (temp_buf, INTERNAL_BUFFER_SIZE, _T("%s"), (tchar *)curr_label.c_str());
					
//jad 2018.12.17					
					//(void)_tstrncpy(temp_buf, curr_label.c_str(), _tstrlen (curr_label.c_str()));

					//jad debug
					//LOGIT(CLOG_LOG, "_tstrncpy'%S'", temp_buf);

					(void)_tsprintf(temp_buf, INTERNAL_BUFFER_SIZE, _T("%S"), curr_label.c_str());

					//LOGIT(CLOG_LOG, "_tstrncpy2'%S'", temp_buf);



				}
				else // must be U
				{
					pVar->getUnitString(curr_label); 
//jad 2.3
//					(void)WIDE_SWPRINTF(temp_buf,INTERNAL_BUFFER_SIZE, _T("%s"), curr_label.c_str());
#ifdef MODIFIED_BY_SS
					size_t length = std::wcslen(curr_label.c_str());
					(void)wcsncpy_s(temp_buf, length, curr_label.c_str(), _TRUNCATE);
					//(void)_tstrncpy(temp_buf, curr_label.c_str(), _tstrlen (curr_label.c_str()));
#endif
				}
			}
			else // %L & %U are only valid on DD variables
			{
				SDCLOG(CERR_LOG,
					"Method Format Error:%%[L],%%[U] & %%[D] are only valid on DD variables\n");
				break;// nothing to display
			}
		}
		else 
	//#	else     // not L or U  format
		{			
			if ( curr_format[0] != 0)
			{
				if ( !_tstrcmp (curr_format, _T("%D")) )// check for a D alone
				{
					if (pVar->IsDynamic())
					{
						updateDynamic = true; 
					}
					else
					{
						SDCLOG(CERR_LOG,
								 "Method Format Error:%[D] is only valid on CLASS DYNAMIC\n");
						// fall thru to try and display something (we have a variable)
					}
					curr_format[0] = '\0';// no need to do anymore formatting
				}
				else// check for a ,D in the format
				{
					//curr_ptr = _tstrchar (prompt, _T(','));//comma illegal except 4 ',D'
					//if (curr_ptr != NULL && curr_ptr != prompt)
					curr_ptr = _tstrchar (curr_format, _T(','));//comma illegal except 4 ',D'
					if (curr_ptr != NULL && curr_ptr != curr_format)
					{//		found
						*curr_ptr++ = 0;// get rid of comma and whatever follows
						if ( *curr_ptr == _T('D') )// if it was ',D'
						{
							updateDynamic = true; 
						}
						else// otherwise an error - no whitspace allowed
						{
							SDCLOG(CERR_LOG,
							 "Method Format Error:comma is only valid for ',D' formatting\n");
							// fall thru to try and display something
						}
					}
					// else - at not found, do nothing.
				}
			}// endif we have a format so check for a D


			/* modified 30may14 - stevev - 
			 * up_UPDATE_SPEC_DYN is only used for display - where any and all dynamics are updated
			 * up_UPDATE_STD_DYN  is normal - only update if dynamic and has a 'D' format
			 * up_DONOT_UPDATE    is for a couple of builtins that NEVER update 
			 *                    - these will have a null pDynamicVarValues
			 */
			if ( pVar != NULL )// we have a DD variable
			{// do the value
				// isSpecialDyn usage is not defined at this time.
	//#			if  var->isDynamic && ( isSpecialDyn || update_Dynamic )	
				if ( pVar->IsDynamic() // we only update dynamic variables, ever (wap 10aug07)
				     &&
					 pDynamicVarValues != NULL  // we were passed in (if not then NEVER update)
					 &&
					 (  updateDynamic	|| // set via the 'D' option above
						//OR - if it is an 'auto update' built-in function (DISPLAY or display)
						( (updateLevel & up_UPDATE_SPEC_DYN) == up_UPDATE_SPEC_DYN ) 
				     )  
				   )
				{// we are GO to update dynamics
					m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay = 
					updateDynamic = true;
//#				get command,  send it the first time
					hCcommandDescriptor rdCmd = pVar->getRdCmd();
					if ( rdCmd.cmdNumber > -1 )
					{	// get cmd ptr
						/* stevev 20Jul05 this needs the entire descriptor 
						 ** including indexes ***
						 for now we'll just give the right transaction????????? */
LOGIT(CLOG_LOG,"bltin_format_string::Sending Cmd %d Trans %d w/%d index Values to fill 0x%04x\n",
		rdCmd.cmdNumber,rdCmd.transNumb,rdCmd.idxList.size(), pVar->getID());
						m_pDevice->sendMethodCmd(rdCmd.cmdNumber,
												 rdCmd.transNumb,
												 &(rdCmd.idxList));//stevev 30nov11 add indexes
						/* end stevev 20Jul05 */
LOGIT(CLOG_LOG,"bltin_format_string::Back from send command\n");
					}// else - just get the current display value.
					else
					{
		DEBUGLOG(CLOG_LOG,"bltin_format_string: 0x%04x has no read command.\n",pVar->getID());
					}
				}
				else
				{
		DEBUGLOG(CLOG_LOG,"bltin_format_string: Set UpdateDynamic FALSE @ 0x%04x.\n",pVar->getID());
		//jad debug
					//m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay = 
					//updateDynamic = false;
				}// endif dynamic update - send command

//#				getDispValue to CurrValueVarient (dynamic & non-dynamic the same)
				ReturnedDataItem = pVar->getDispValue();// we won't use it in getDisplayString 
//#				put value into dynamic array  &&  set ischanged
				if ( updateDynamic )
				{
#ifdef _DEBUG
					CValueVarient tmpVarVal=pDynamicVarValues[nDynVarCntr];
#endif					
					if( ! (pDynamicVarValues[nDynVarCntr] == ReturnedDataItem) )
					{
						bDyanmicVarvalChanged=true;
						pDynamicVarValues[nDynVarCntr] = ReturnedDataItem;
				}				
					nDynVarCntr++;
				}// else there is a reason this is not supposed to be updated		
			}//endif we have a DD variable 
			else   // not a DD-variable;  MUST be a method-local variable so do INTER_VARIENT
			{	
				if ( updateDynamic )// we have a 'D' format
				{
					SDCLOG(CERR_LOG,
			   "Method Format Error:'D' formatting is illegal for method-local variables.\n");
				}
	//#			convert intervarient to CurrValueVarient
				if ( inter2hcVARIANT(ReturnedDataItem,VariantVarVal) )// true on error
				{
					SDCLOG(CERR_LOG,"Method Format Error: "
											   "Method-Local to Varient conversion failed.\n");
					break;
				}
	//#			if curr_format == NULL - ie isEmpty()	
				if (curr_format[0] == _T('\0'))
				{
	//#				copy default format string into curr_format	
					//_tstrcpy(curr_format,vtype_strings[VariantVarVal.GetVarType()]);
					string  s(vtype_strings[VariantVarVal.GetVarType()]);
					wstring ws; ws = AStr2TStr(s);
#ifdef MODIFIED_BY_SS
					//_tstrcpy(curr_format,ws.c_str());
					wcsncpy_s(curr_format, ws.size(), ws.c_str(), _TRUNCATE);
#endif
				}
	//#			//else -- we'll use the curr_format and currvalvarient to get the string later
			}// end else parameter must be a method-local

	//#		if  curr_format == NULL && VarPtr != NULL	
			if ( curr_format[0] == '\0' && pVar != NULL)
			{// we need to handle the formatting exceptions first
				wstring tmp;

	//#			tmp = getDisplayString()	
				switch(pVar->VariableType())
				{
				case vT_Enumerated:
					{
						if ( ((hCEnum*)pVar)->procureString ((unsigned)ReturnedDataItem, tmp) )
						{
							tmp = L"";
						}
					}
					break;
				case vT_BitEnumerated:
					{
						if (((hCBitEnum*)pVar)->procureString ((unsigned)ReturnedDataItem,tmp))
						{
							tmp = L"";
						}
					}
					break;
				//AOEP35746: VT_PASSWORD DETECTION TO HIDE THE PASSWORD WS_08sep10
				case vT_Password:
					{
						wstring strPassword = pVar->getDispValueString();
						int nSize = strPassword.length();
						wchar_t tempString[100]={0};
						for(int i = 0; i < nSize; i++)
						{
							tempString[i] = '*';
						}
						tmp = tempString;
					}
					break;	
				//END AOEP35746
				default: // all other types...
					// vT_Index does a string substitution...
					tmp = pVar->getDispValueString();	//varient unused here
					break;
				}// endswitch vartype
	//#				value_string <<= tmp	
#ifdef MODIFIED_BY_SS				
				//_tstrcpy(temp_buf,tmp.c_str() );
				wcsncpy_s(temp_buf, tmp.size(), tmp.c_str(), _TRUNCATE);
#endif

			}
			else// - all the rest...
			{	// have a user_format with or without a pVar 
				// OR no  user_format without pVar (curr_format has default formatting)
				if ( doFormat(curr_format,pVar,ReturnedDataItem,temp_buf,INTERNAL_BUFFER_SIZE))
				{// was an error
					
#ifdef MODIFIED_BY_SS				
//_tstrcpy(temp_buf,_T("<Formatting Error>") );
					std::wstring tmp(_T("<Formatting Error>"));
					wcsncpy_s(temp_buf, tmp.size(), tmp.c_str(), _TRUNCATE);
#endif
				}
	//#			switch on varient data type													
	//#				value_string from CurrValueVarient using curr_format
			}
		}// end-else  (not L or U or D)									
	//#	
	//#	Copy2Output(  value_string  )
		if ( retCode = CopyToOutbuf(out_ptr,remainingOut,  temp_buf, -1))//testing retcode
		{// probable full buffer
			break;// error exit
		}	
	//#	Loop to the forever
	}// loop for ever

LOGIT(CLOG_LOG,"bltin_format_string: post while loop\n");
LOGIT(CLOG_LOG,"bltin_format_string: prompt_buf about to be deleted '%p'\n",prompt_buf);


	if ( prompt_buf )
// sjv 30jun14		free (prompt_buf);
		free(hold_buf);
	prompt_buf = NULL;

LOGIT(CLOG_LOG,"bltin_format_string: final exit\n");
LOGIT(CLOG_LOG,"bltin_format_string: retCode = %d\n", retCode);

	return retCode;
}// end of bltin_format_string()


#ifdef MODIFIED_BY_SS // commented by srilatha
//#else /*** original subroutine ****/
//int CHart_Builtins::												//Modified by ANOOP 17FEB2004
//bltin_format_string (char *out_buf, int max_length, char *passed_prompt,long *glob_var_ids,
//					 int nCnt_Ids,CValueVarient *pDynamicVarValues,bool & bDyanmicVarvalChanged)  
//{
//
//	char			*out_ptr=NULL, *out_endp=NULL,*tmp_ptr=NULL;
//	char 			*prompt = NULL, *curr_ptr = NULL, *prompt_buf = NULL;
//	int				copy_length, par_index = -1, rs,*u = 0;
//	char			prompt_char;
//	char			curr_format[20], curr_par[100];//, curr_label[100];
//	char			temp_buf[1024];/* t2_buf[200], *temp_ptr, *str_ptr;*///Anil modified September 12 2005
////	char			*byte_ptr;
//	char			*use_format;
////	char 			temp_char;
//	hCitemBase		*pIB = NULL;
//	long			itemID;
//	CValueVarient	ReturnedDataItem;
//	INTER_VARIANT	varVal;
//	long			param_value_type;
//	int				param_value_val_i;
//	float			param_value_val_f;
//	double			param_value_val_d;
//	char			param_value_val_c;//Vibhor 150305: Added
//	string			curr_label,tmpFormat;
//	int				nCntr=0;
//	EnumTriad_t		localData;
//	int				nDynVarCntr=0;		//Added by ANOOP 17FEB2004
///*Vibhor 240105: Start of Code*/
//	/*The Global m_pMethSupportInterface->bEnableDynamicDisplay flag is causing the non-dynamic
//	 vars also to be treated as dynamics, in case the prompt string has both, this in-turn is leading to
//	 1. Dispatch of xtra commands (unnecessary)
//	 2. Invalid treatment of Enumerated and Index vars as just ints !!
//	So I am adding this local boolean flag which would tell whether the current var
//	under consideration needs dynamic handling or not
//	*/
//	bool			bIsDynamic = false;
//
///*Vibhor 240105: End of Code*/
//
//
//	if (nCnt_Ids == 0)
//	{
//		/**	If there are no more occurrences of "%", copy the rest of the string. **/		
//		tmp_ptr=strchr(passed_prompt,'{');
//		if ( NULL == tmp_ptr)
//		{
//			strcpy(out_buf, passed_prompt);
//			return 0;
//		}
//		
//	}
//	
//	/*	Get the output buffer and the prompt string */
//	out_endp = out_buf + max_length;
//
//	prompt_buf =NULL;
//
//	
//
//	//Find the appropriate country code string in the prompt string.
//	prompt_buf = (char *) malloc(strlen(passed_prompt)+1);
//	
//
//	if (prompt_buf == NULL)
//	{
//		*out_buf = 0;
//		rs = 0;
//		return (rs);
//	}
//	
//	(void)strcpy (prompt_buf, passed_prompt);
//	prompt = find_lang_string (prompt_buf);
//
//	if (prompt == NULL)
//	{
//		free (prompt_buf);
//		*out_buf = 0;
//		rs = 0;
//		return (rs);
//	}
//
//	out_ptr = out_buf;
//
//
//
//	
//	for (;;)
//	{
//		temp_buf[0] = '\0'; // WS:EPM 23may07 - empty buf of any garbage from the last time through
//
//		if ( (nCntr >= nCnt_Ids) && (nCnt_Ids != 0)) 
//		{
////			curr_ptr++;
///*Vibhor 240105: Start Code modifications*/
///*There could be a case where the same array index is being referred 
//more than once in the prompt like Current Values:\nM1 = %0 %{1}\nLRV = %{2} %{1}
//Following modifications are to handle this*/
//			if(NULL == strchr(prompt, '%') && NULL != prompt)// NULL != curr_ptr)
//			{
//				copy_length = strlen (prompt);//was curr_ptr
//				(void)strcpy (out_ptr, prompt);//was curr_ptr
//				out_ptr += copy_length;
//				break;
//			}
//			else if (NULL == prompt) //was curr_ptr
//			{
//				break; // it should never come here, just defensive
//			}
//			//else we still have something to process on the prompt...
///*Vibhor 240105: End Code modifications*/			
//		}
//		
//	
//
//		use_format = (char *)0;
//
//		/**	Copy the prompt string up to the first occurrence of a "%".	 **/
//
//		curr_ptr = strchr(prompt, '%');
//
//		/**	If there are no more occurrences of "%", copy the rest of the string. **/
//
//		if (curr_ptr == NULL)
//		{
//			if(NULL != prompt)
//			{
//				strcat(out_buf,prompt);
//			}
//			break;
//		}
//		/**	Copy the prompt up to the next "%".	 */
//
//		assert("*curr_ptr == '%'");
//		copy_length = curr_ptr - prompt;
//		if (out_ptr + copy_length >= out_endp)
//		{
////			rs = METH_INTERNAL_ERROR;
////			goto err_rtn;
//		}
//		(void)strncpy (out_ptr, prompt, copy_length);
//		out_ptr += copy_length;
//		prompt = curr_ptr + 1;
//		prompt_char = *prompt;
//
//		/*
//		 *	Handle the formatting.  The formatting consists of:
//		 *
//		 *	%[format]{param ID array index} for a device param
//		 *	%[format]{local param name} for a local param
//		 *
//		 *	The format is optional, and consists of standard scanf
//		 *	format string, with the addition of 'L', which specifies
//		 *	the label of the param.
//		 */
//
//		if (prompt_char == '[')
//		{
//
//			/**	Get the format string.	 */
//
//			prompt++;
//			curr_ptr = strchr (prompt, ']');
//			if (curr_ptr == NULL || curr_ptr == prompt)
//			{
//				rs = BLTIN_INVALID_PROMPT;
////				goto err_rtn;
//			}
//			copy_length = curr_ptr - prompt;
//			*curr_format = '%';
//			(void)strncpy (&curr_format[1], prompt, copy_length);
//			curr_format[copy_length+1] = 0;
//			prompt = curr_ptr + 1;
//			prompt_char = *prompt;
//		} 
//		else
//		{
//			*curr_format = 0;
//		}
//
//		if (prompt_char == '{')
//		{
//
//			/**	Get the param string.  Should leading or following blanks be removed?	 */
//			
//			prompt++;
//			curr_ptr = strchr (prompt, '}');
//			if (curr_ptr == NULL)
//			{
//				rs = BLTIN_INVALID_PROMPT;
////				goto err_rtn;
//			}
//			copy_length = curr_ptr - prompt;
//			(void)strncpy (curr_par, prompt, copy_length);
//			curr_par[copy_length] = 0;
//			prompt = curr_ptr + 1;
//			prompt_char = *prompt;
//			bIsDynamic = false ; //Vibhor 240105: Added
//		} 
//		else if (isdigit (prompt_char))
//		{
//			/**	Special case of param string... %X is same as %{X}
//			 	where X is a single digit number.		 */
//
//			curr_par[0] = prompt_char;
//			curr_par[1] = 0;
//			prompt++;
//			m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay=true;			//Modified by ANOOP 17FEB2004
//			bIsDynamic = true ; //Vibhor 240105: Added
//			
//		}
//		//For the PAR 5558
//		//Anil 16 November 2005
//		else if(0 == *curr_format)
//		{		
//			(void)strcpy (out_ptr, "%");
//			out_ptr ++;//= copy_length;			
//			continue;
//		}
//		else 
//		{
//			rs = BLTIN_INVALID_PROMPT;
//			bIsDynamic = false ; //Vibhor 240105: Added
////			goto err_rtn;
//		}
//
///*<START>12/12/2003 Modified by Anoop to fix the PAR when both local vars and device vars are coming */
//		par_index=-1;
///*<END>12/12/2003 Modified by Anoop to fix the PAR when both local vars and device vars are coming */
//		if (isdigit(curr_par[0]))
//		{
//				/*	The reference is to a device param.Code for integer index. */
//				nCntr++;
//				par_index = atoi (curr_par);
//				
//		}
//
///*<START>12/12/2003 Modified by Anoop to fix the PAR when both local vars and device vars are coming */
//		if( NULL != glob_var_ids && par_index != -1 )
//		{
//			if(NULL != glob_var_ids + par_index)
//			{
//				itemID=*(glob_var_ids + par_index);
//			}
//			
//		}
///*<END>12/12/2003 Modified by Anoop to fix the PAR when both local vars and device vars are coming */
//
//		/*	Process the format string into out_buf.	 */
//
//		if (!strcmp (curr_format, "%L"))
//		{
//			/*	Print the label of the param. */
//		
//			if (isdigit(curr_par[0]))
//			{  /* if device param */
//
//				if (m_pDevice->getItemBySymNumber(itemID, &pIB) == SUCCESS)
//				{
//					if ( pIB->IsVariable() )
//					{
//						hCVar *pVar=(hCVar *)pIB;
//						pVar->Label(curr_label); 
//					}
//				}
//
////				str_ptr = find_lang_string (curr_label);
//				(void)sprintf (temp_buf, "%s", curr_label.c_str());	//Vibhor 271204: BugFix
//			}
//			else
//			{
//				string strTemp;
//				strTemp = curr_par;
//				if (m_pDevice->getItemBySymName(strTemp, &pIB) == SUCCESS)
//				{
//					if ( pIB->IsVariable() )
//					{
//						hCVar *pVar=(hCVar *)pIB;
//						pVar->Label(curr_label); 
//					}
//				}
//
//
//				(void)sprintf (temp_buf, "%s", curr_label.c_str());
//
//			}
//		}
//		//Anil 20 December 2005 To accomadate the %u format to get the unit
//		else if (!strcmp (curr_format, "%U"))
//		{
//			if (isdigit(curr_par[0]))
//			{ 
//
//				if (m_pDevice->getItemBySymNumber(itemID, &pIB) == SUCCESS)
//				{
//					if ( pIB->IsVariable() )
//					{
//						hCVar *pVar=(hCVar *)pIB;
//						pVar->getUnitString(curr_label); 
//					}
//				}
//
//				(void)sprintf (temp_buf, "%s", curr_label.c_str());	//Vibhor 271204: BugFix
//			}
//			else
//			{
//				string strTemp;
//				strTemp = curr_par;
//				if (m_pDevice->getItemBySymName(strTemp, &pIB) == SUCCESS)
//				{
//					if ( pIB->IsVariable() )
//					{
//						hCVar *pVar=(hCVar *)pIB;
//						pVar->getUnitString(curr_label); 
//					}
//				}
//
//
//				(void)sprintf (temp_buf, "%s", curr_label.c_str());
//
//			}
//		}
//		else
//		{
//			/**	The param value for the device variable has to be fetched from the device object.	 */
//
//			if (isdigit(curr_par[0]))
//			{
//				/* if device param */
//				if (m_pDevice->getItemBySymNumber(itemID, &pIB) == SUCCESS)
//				{
//					if ( pIB->IsVariable() )
//					{
///*<START>Added by ANOOP for dynamic variables %0 to be refreshed continously 17FEB2004 */					
//						if(true == m_pDevice->m_pMethSupportInterface->bEnableDynamicDisplay 
//							&& bIsDynamic //Vibhor 240105: Modified	
//							&& ((hCVar*)pIB)->IsDynamic()
//						   ) //Vibhor 280605: Added another filter
//						{
///*Vibhor 180204: Start of Code*/
//							hCVar *pVar = (hCVar*)pIB;
//							if(NULL != pVar)
//							{
//								hCcommandDescriptor rdCmd = pVar->getRdCmd();
//								if ( rdCmd.cmdNumber < 0xFF && rdCmd.cmdNumber > -1 )
//								{	// get cmd ptr
//									
///* stevev 20Jul05 this needs the entire descriptor ** including indexes ***
//		RETURNCODE 	rc = m_pDevice->sendMethodCmd(rdCmd.cmdNumber,DEFAULT_TRANSACTION_NUMBER);
//				  for now we'll just give the right transaction????????? */
//									RETURNCODE 	rc = m_pDevice->sendMethodCmd(rdCmd.cmdNumber,rdCmd.transNumb);
///* end stevev 20Jul05 */
//									if(SUCCESS == rc)
//									{
//										ReturnedDataItem = pVar->getDispValue();
//									}
//										
//								}
///*Vibhor 190204: Start of Code*/
//								else
//								{//just get the display value!!! the var may be a write only !! 
//									ReturnedDataItem = pVar->getDispValue();
//								}
//							
///*Vibhor 190204: End of Code*/
//							//	RETURNCODE rc = m_pDevice->ReadImd(pIB, ReturnedDataItem);
//							
//							CValueVarient tmpVarVal=pDynamicVarValues[nDynVarCntr];
//							
//							if( ReturnedDataItem.vType == CValueVarient::isIntConst )
//							{
//								param_value_type=2;
//								param_value_val_i=ReturnedDataItem.vValue.iIntConst;
//								if(tmpVarVal.vValue.iIntConst!=ReturnedDataItem.vValue.iIntConst)
//								{
//									bDyanmicVarvalChanged=true;
//									pDynamicVarValues[nDynVarCntr++]=ReturnedDataItem;
//								}
//						
//							}
//							else if( ReturnedDataItem.vType == CValueVarient::isFloatConst )
//							{
//								param_value_type=5;
//								param_value_val_d=ReturnedDataItem.vValue.fFloatConst;
//								if(tmpVarVal.vValue.fFloatConst != ReturnedDataItem.vValue.fFloatConst)
//								{
//									bDyanmicVarvalChanged=true;
//									pDynamicVarValues[nDynVarCntr++]=ReturnedDataItem;
//								}
//							}
//							else if( ReturnedDataItem.vType == CValueVarient::isString )
//							{
//								param_value_type=6;
//								tmpFormat=ReturnedDataItem.sStringVal.c_str();
//								string szPrevVal=tmpVarVal.sStringVal.c_str();
//								if(szPrevVal != tmpFormat)
//								{
//									bDyanmicVarvalChanged=true;
//									pDynamicVarValues[nDynVarCntr++]=ReturnedDataItem;
//								}
//								
//
//							}
//							else if( ReturnedDataItem.vType == CValueVarient::invalid )
//							{
//								param_value_type=6;
//								tmpFormat="*INVALID*";  
//							}
//							}//endif pVar != NULL
//						}	
///*<END>Added by ANOOP for dynamic variables %0 to be refreshed continously 17FEB2004 */											
//						else
//						{
//						hCVar * pLocalVar = (hCVar*)pIB;
//
//						switch(pLocalVar->VariableType())
//						{
//							case vT_Index:	
//							{
//								hCindex* pIndex = NULL;
//								pIndex = (hCindex*)pLocalVar;
//								ReturnedDataItem=pIndex->getDispValueString();	
//								param_value_type=6;
//								tmpFormat=ReturnedDataItem.sStringVal.c_str();
//								break;
//							}
//							case vT_Enumerated:
//							{
//									
//								unsigned int nVal= pLocalVar->getDispValue();
//								hCEnum* pEn = NULL;
//								pEn = (hCEnum*)pIB;
//								hCenumList eList(pEn->devHndl());
//
//								param_value_type=6;
//								if (pEn->procureString (nVal, tmpFormat) != SUCCESS)
//								{
//									tmpFormat = "";
//								}							
//								break;
//							}	
//
//							case vT_BitEnumerated:
//							{
//								unsigned int nVal=pLocalVar->getDispValue();
//								hCBitEnum* pEn = NULL;
//								pEn = (hCBitEnum*)pIB;
//								hCenumList eList(pEn->devHndl());
//
//								param_value_type=6;
//								if (pEn->procureString (nVal, tmpFormat) != SUCCESS)
//								{
//									tmpFormat = "";
//								}							
//								break;
//							}
//									
//							default:
//							{
//								ReturnedDataItem=pLocalVar->getDispValue();
//								if( ReturnedDataItem.vType == CValueVarient::isIntConst )
//								{
//									param_value_type=2;
//									param_value_val_i=ReturnedDataItem.vValue.iIntConst;
//								}
//								else if( ReturnedDataItem.vType == CValueVarient::isFloatConst )
//								{
//									param_value_type=5;
//									param_value_val_d=ReturnedDataItem.vValue.fFloatConst;
//								}
//								else if( ReturnedDataItem.vType == CValueVarient::isString )
//								{
//									param_value_type=6;
//									tmpFormat=ReturnedDataItem.sStringVal.c_str();  
//		//							param_value_val_d=ReturnedDataItem.vValue.
//								}
//								else if( ReturnedDataItem.vType == CValueVarient::invalid )
//								{
//									param_value_type=6;
//									tmpFormat="*INVALID*";  
//		//							param_value_val_d=ReturnedDataItem.vValue.
//								}
//												
//							}
//							break;
//
//						}//end switch pLocalVar->VariableType()
//
//						}//end else
//									
//					}//endif  pIB->IsVariable()
//				}//endif m_pDevice->getItemBySymNumber(itemID, &pIB) == SUCCESS
//				else
//				{
//						param_value_type=6;
//						tmpFormat = "<error>";
//				
//				}
//
//				/**	If no format specified, get format specified in the DDL. */		 
//
//				if (*curr_format == 0)
//				{
//					use_format = (char*)NULL;
//				}
//				else
//				{
//					use_format = curr_format;
//				}
//
//			}
//			else
//			{  
//
//				/** If no format is specified, set the use_format to NULL.
//				 * Remember, local variables do not have a default format
//				 * string like device parameters.			 */
//				if (*curr_format == 0)
//				{
//					use_format = (char*)NULL;
//				}
//				else
//				{
//					use_format = curr_format;
//				}	
//				
//	
//				/** local param - get value from the interpreter symbol table  */
//	/* To be deleted   Needs to return to prasad the value based on the param */
//				//For the PAR 5558
//				//Anil 16 November 2005
//				varVal.Clear();
//				rs=m_pInterpreter->GetVariableValue((char *)curr_par,varVal);
///*				if (rs != BI_SUCCESS)
//				{
//					goto err_rtn;
//				}
//*/
////Added By Anil July 06 2005 --start multiple code modifications
//				bool bGlobalVariable = false;
//				hCitemBase		*pIBTemp = NULL;
//				if(rs == false)
//				{					
//					string strcurr_par = (char *)curr_par;					
//					if(m_pDevice->getItemBySymName(strcurr_par,&pIBTemp) == SUCCESS)
//					{
//						if(pIBTemp != NULL)
//						{
//							bGlobalVariable = true;
//						}
//						else
//						{
//							return false;
//						}
//					}
//					else
//					{
//						return false;
//					}
//				}			
//
//				if(bGlobalVariable == false)
//				{
//					switch(varVal.GetVarType())
//					{
//	/*					case RUL_NULL:
//							break;          */ 
//	/*Vibhor 150305: Start of Code*/
//	//	Uncommented !!!!! case RUL_CHAR
//						case RUL_UNSIGNED_CHAR: /* added 31may07 stevev */
//						case RUL_CHAR:
//							param_value_type  = varVal.GetVarType();
//							param_value_val_c = (char)varVal;
//							break;	
//	/*Vibhor 150305: End of Code*/					
//						case RUL_INT:
//							param_value_type=varVal.GetVarType();
//							param_value_val_i =varVal;
//							break;
//						case RUL_FLOAT:
//							param_value_type=varVal.GetVarType();
//							param_value_val_f =varVal;
//							break;
//						case RUL_DOUBLE:
//							param_value_type=varVal.GetVarType();
//							param_value_val_d = varVal;
//							break;
//							
//						case RUL_CHARPTR:
//						case RUL_DD_STRING:
//							{
//								char *szTemp = NULL;
//								char szLangCode[5] = {0};
//								bool bLangCodePresent = false;
//								param_value_type=varVal.GetVarType();
//								if(varVal.GetVarType() == RUL_DD_STRING)
//								{
//									varVal.GetStringValue(&szTemp,RUL_DD_STRING);
//
//								}
//								if(varVal.GetVarType() == RUL_CHARPTR)
//								{
//									varVal.GetStringValue(&szTemp,RUL_CHARPTR);
//								}
//								GetLanguageCode(szTemp,szLangCode,&bLangCodePresent);	
//								tmpFormat = szTemp;
//								if(szTemp)
//								{
//									delete[] szTemp;
//									szTemp = NULL;
//								}						
//							}
//						break;						
//							
//						//Anil added this case on 4th October 2005
//						case RUL_SAFEARRAY:
//							{		
//								char pchString[1024]={0};
//								int pSize = 1024;// ws:EPM, 22jun07 - initialize locals...
//								GetCharArray(varVal,pchString,pSize);						
//								char szLangCode[5] = {0};
//								bool bLangCodePresent = false;
//								param_value_type=varVal.GetVarType();
//								GetLanguageCode(pchString,szLangCode,&bLangCodePresent);	
//								tmpFormat = pchString;
//							}
//						break;
//						case RUL_NULL:
//						case RUL_BOOL:
//						default:
//							param_value_type = varVal.GetVarType();
//							break;
//
//					}//end of switch(varVal.varType)
//				}//End of bGlobalVariable == false
//				
//				if(bGlobalVariable == true)
//				{					
//					if(pIBTemp->IsVariable() == false)
//					{
//						return false;
//
//					}	
//					hCVar * pLocalVar = (hCVar*)pIBTemp;				
//
//					switch(pLocalVar->VariableType())
//					{		
//						case vT_Index:	
//						{
//							hCindex* pIndex = NULL;
//							pIndex = (hCindex*)pLocalVar;
//							ReturnedDataItem=pIndex->getDispValueString();	
//							param_value_type=6;
//							tmpFormat=ReturnedDataItem.sStringVal.c_str();
//							break;
//						}
//						case vT_Enumerated:
//						{
//								
//							unsigned int nVal= pLocalVar->getDispValue();
//							hCEnum* pEn = NULL;
//							pEn = (hCEnum*)pIBTemp;
//							hCenumList eList(pEn->devHndl());
//
//							param_value_type=6;
//							if (pEn->procureString (nVal, tmpFormat) != SUCCESS)
//							{
//								tmpFormat = "";
//							}							
//							break;
//						}	
//
//						case vT_BitEnumerated:
//						{
//							unsigned int nVal=pLocalVar->getDispValue();
//							hCBitEnum* pEn = NULL;
//							pEn = (hCBitEnum*)pIBTemp;
//							hCenumList eList(pEn->devHndl());
//
//							param_value_type=6;
//							if (pEn->procureString (nVal, tmpFormat) != SUCCESS)
//							{
//								tmpFormat = "";
//							}
//							break;
//						}
//								
//						default:
//						{
//							ReturnedDataItem=pLocalVar->getDispValue();
//							if( ReturnedDataItem.vType == CValueVarient::isIntConst )
//							{
//								param_value_type=2;
//								param_value_val_i=ReturnedDataItem.vValue.iIntConst;
//							}
//							else if( ReturnedDataItem.vType == CValueVarient::isFloatConst )
//							{
//								param_value_type=5;
//								param_value_val_d=ReturnedDataItem.vValue.fFloatConst;
//							}
//							else if( ReturnedDataItem.vType == CValueVarient::isString )
//							{
//								param_value_type=6;
//								tmpFormat=ReturnedDataItem.sStringVal.c_str();  
//							}
//							else if( ReturnedDataItem.vType == CValueVarient::invalid )
//							{
//								param_value_type=6;
//								tmpFormat="*INVALID*";  		
//							}
//		
//						}
//						break;
//		
//					}//end switch pLocalVar->VariableType()
//				}
////Added By Anil July 06 2005 --end multiple code modifications
//
//			}
//
//			/**	Format the value into out_buf.  Each type has a default
//			 *	format to use if none is specified.
//			 **  First check the validity of the format string.		 */
//
//			switch (param_value_type) 
//			{
//			case RUL_UNSIGNED_CHAR: /* added 31may07 - stevev */
//				{		   //overlap with DDLDefs for Var types
//					if (use_format == 0 )//spec does not restrict..stevev 01jun07..was>|| !strstr(use_format,"c")) 
//					{
//						// stevev 27feb06 - we don't want ascii!!use_format = "%c";
//						use_format = "%u";
//					}
//					(void)sprintf (temp_buf, use_format, ((unsigned char)param_value_val_c));
//				}
//				break;
///*Vibhor 150305: Start of Code*/		
//			case RUL_CHAR: //This will work since RUL_CHAR is 1 and doesn't 
//				{		   //overlap with DDLDefs for Var types
//					if (use_format == 0 )//spec does not restrict..stevev 01jun07..was>|| !strstr(use_format,"c")) //Sntosh G:5dec06 - Wrong format results in Zero value after sprintf
//					{
//						// stevev 27feb06 - we don't want ascii!!use_format = "%c";
//						use_format = "%d";
//					}
//					(void)sprintf (temp_buf, use_format, param_value_val_c);
//				}
//				break;
///*Vibhor 150305: End of Code*/
//			case RUL_INT:	// we are using inter-varient types, while equal, not accurate.. case INTEGER:
//				if (use_format == 0)//spec does not restrict..stevev 01jun07..was> || !strstr(use_format,"d")) //Sntosh G:5dec06 - Wrong format results in Zero value after sprintf
//				{
//					use_format = "%d";
//				}
//				(void)sprintf (temp_buf, use_format, param_value_val_i);
//				break;
//			case RUL_FLOAT:	// we are using inter-varient types, while equal, not accurate.. case FLOATG_PT: 			
//				if (use_format == 0)//spec does not restrict..stevev 01jun07..was> || !strstr(use_format,"f")) //Sntosh G:5dec06 - Wrong format results in Zero value after sprintf
//				{
//					use_format = "%f";
//				}
//				(void)sprintf (temp_buf, use_format,param_value_val_f);
//				break;
//			case RUL_DOUBLE:// we are using inter-varient types, while equal, not accurate.. case DOUBLE_FLOAT:
//				if (use_format == 0 )//spec does not restrict..stevev 01jun07..was>|| !strstr(use_format,"f")) //Sntosh G:5dec06 - Wrong format results in Zero value after sprintf
//				{
//					use_format = "%lf";
//				}
//				(void)sprintf (temp_buf, use_format,param_value_val_d);
//				break;
//			case RUL_CHARPTR:
//			case RUL_DD_STRING:	//Added By Anil July 06 2005
//			case RUL_SAFEARRAY://Added By Anil October 4 2005
//				if (use_format == 0 )//spec does not restrict..stevev 01jun07..was>|| !strstr(use_format,"s")) //Sntosh G:5dec06 - Wrong format results in Zero value after sprintf
//				{
//					use_format = "%s";
//				}
//				(void)sprintf (temp_buf, use_format,tmpFormat.c_str());
//				break;
//			
///*				case UNSIGNED:
//				if (use_format == 0) {
//					use_format = "%u";
//				}
//				(void)sprintf (temp_buf, use_format, param_value_val_u);
//				break;*/
//			case RUL_NULL:// stevev 31may07 - stevev - always handle the default!!
//			case RUL_BOOL:
//			default:
//				(void)sprintf (temp_buf, "ERROR:unknown_type");
//				break;				
//			}
//		}
//
//		copy_length = strlen (temp_buf);
//		if (out_ptr + copy_length >= out_endp) {
////			rs = METH_INTERNAL_ERROR;
////			goto err_rtn;
//		}
//		(void)strcpy (out_ptr, temp_buf);
//		out_ptr += copy_length;
//
//		bIsDynamic = false ; //Vibhor 240105: Added
//	}
//
//	/**	Return. */
//	free (prompt_buf);
//	return (BI_SUCCESS);
//
///*	err_rtn:
//	*out_buf = 0;
//	free (prompt_buf);
//
//	return (rs); 
//*/
//}
//#endif /* end of original bltin_format_string() subroutine **************************************/
//
//
//void CHart_Builtins::PackedASCIIToASCII (
//
//						 unsigned char *pbyPackedASCII
//						 , unsigned short wPackedASCIISize
//						 , char *pchASCII
//						 )
//
//{
//
//	unsigned short wSize = wPackedASCIISize;
//
//	int iLoopVar,iASCIICount = 0, iStreamIndex = 0, iASCIIIndex = 0;
//
//
//
//	unsigned char byTemp;
//
//	for (iLoopVar = 0; iLoopVar < wSize; )
//
//	{
//
//		switch(iASCIICount)
//
//		{
//
//			case 0:
//
//				{
//
//					/* The first byte....*/
//
//					byTemp = pbyPackedASCII[iLoopVar];
//
//
//
//					/* Get the six byte Packed ACSCII value */
//
//					byTemp = byTemp & 0xfc;
//
//					byTemp = byTemp >> 2;
//
//
//
//
//
//					/* Now convert it to normal ASCII */
//
//					/* Set bit 6 as the complement of bit 5 */
//
//					unsigned char byTest = byTemp & 0x20;
//
//					if(byTest)
//
//					{
//
//						byTemp = byTemp & 0xbf;
//
//					}
//
//					else
//
//					{
//
//						byTemp = byTemp | 0x40;
//
//					}
//
//
//
//					/* Now store this ascii value */
//
//					pchASCII[iASCIIIndex] = byTemp;
//
//
//
//					iASCIIIndex++;
//
//					iASCIICount++;
//
//					break;
//
//				}
//
//			case 1:
//
//				{
//
//					/* The second unsigned char....*/
//
//					byTemp = pbyPackedASCII[iLoopVar];
//
//					byTemp = byTemp << 6;
//
//
//
//					byTemp = byTemp | ((pbyPackedASCII[iLoopVar + 1] & 0xf0) >> 2);
//
//
//
//					/* Get the six unsigned char Packed ACSCII value */
//
//					byTemp = byTemp & 0xfc;
//
//					byTemp = byTemp >> 2;
//
//
//
//
//
//					/* Now convert it to normal ASCII */
//
//					/* Set bit 6 as the complement of bit 5 */
//
//					if ((byTemp & 0x20))
//
//					{
//
//						byTemp = byTemp & 0xbf;
//
//					}
//
//					else
//
//					{
//
//						byTemp = byTemp | 0x40;
//
//					}
//
//
//
//					/* Now store this ascii value */
//
//					pchASCII[iASCIIIndex] = byTemp;
//
//
//
//					iASCIIIndex++;
//
//					iASCIICount++;
//
//					iLoopVar ++;
//
//					break;
//
//				}
//
//			case 2:
//
//				{
//
//					/* The third unsigned char....*/
//
//					byTemp = pbyPackedASCII[iLoopVar];
//
//					byTemp = byTemp << 4;
//
//
//
//					byTemp = byTemp | ((pbyPackedASCII[iLoopVar + 1] & 0xc0) >> 4);
//
//
//
//					/* Get the six unsigned char Packed ACSCII value */
//
//					byTemp = byTemp & 0xfc;
//
//					byTemp = byTemp >> 2;
//
//
//
//
//
//					/* Now convert it to normal ASCII */
//
//					/* Set bit 6 as the complement of bit 5 */
//
//					if((byTemp & 0x20))
//
//					{
//
//						byTemp = byTemp & 0xbf;
//
//					}
//
//					else
//
//					{
//
//						byTemp = byTemp | 0x40;
//
//					}
//
//
//
//					/* Now store this ascii value */
//
//					pchASCII[iASCIIIndex] = byTemp;
//
//
//
//					iASCIIIndex++;
//
//					iASCIICount++;
//
//					iLoopVar++;
//
//					break;
//
//				}
//
//			case 3:
//
//				{
//
//					/* The fourth unsigned char....*/
//
//					byTemp = pbyPackedASCII[iLoopVar];
//
//					byTemp = byTemp << 2;
//
//
//
//					/* Get the six unsigned char Packed ASCII value */
//
//					byTemp = byTemp & 0xfc;
//
//					byTemp = byTemp >> 2;
//
//
//
//
//
//					/* Now convert it to normal ASCII */
//
//					/* Set bit 6 as the complement of bit 5 */
//
//					if((byTemp & 0x20))
//
//					{
//
//						byTemp = byTemp & 0xbf;
//
//					}
//
//					else
//
//					{
//
//						byTemp = byTemp | 0x40;
//
//					}
//
//
//
//					/* Now store this ascii value */
//
//					pchASCII[iASCIIIndex] = byTemp;
//
//
//
//					iASCIIIndex++;
//
//					iASCIICount = 0;
//
//					iLoopVar++;
//
//					break;
//
//				}
//
//
//
//		}
//
//
//
//	}
//
//	pchASCII[iASCIIIndex] = 0;
//
//
//
//	return;
//
//}/* End of Function: PackedASCIIToASCII() */
//
//
//
///*F*/
//
///***************************************************************************
//
//** FUNCTION NAME: ASCIIToPackedASCII
//
//**
//
//** PURPOSE		: Takes a char pointer to a common ASCII string,
//
//**	and converts it to packed ASCII
//
//**
//
//** PARAMETER	:
//
//**				- Char Pointer to to a common ASCII string
//
//**	- unsigned char pointer to buffer where the converted packed ASCII string
//
//**	will be copied
//
//**	- The size of the packed ASCII string in bytes
//
//**
//
//** RETURN VALUE	: None
//
//****************************************************************************/
//
//void CHart_Builtins::ASCIIToPackedASCII (
//
//						 char *pchASCII
//
//						 , unsigned char *pbyPackedASCIIOutput
//
//						 , unsigned short *pwPackedASCIISize
//
//						 )
//
//{
//
//	if ( (pchASCII == NULL) || (pbyPackedASCIIOutput == NULL) )
//
//	{
//
//		return;
//
//	}
//
//
//
////	_strupr(pchASCII);
//
//    while (*pchASCII)
//    {
//        *pchASCII = toupper((unsigned char) *pchASCII);
//        pchASCII++;
//    }
//
//        
//
//
//
//	int iLoopVar, iASCIICount = 0, iStreamIndex = 0;
//
//	unsigned char pbyPackedASCII[4];
//
//	unsigned char byTemp;
//
//	DWORD dwPackedASCIITemp, *pdwTemp = NULL;
//
//	for (iLoopVar = 0; iLoopVar < (int)strlen(pchASCII);iLoopVar++)  // warning C4018: '>=' : signed/unsigned mismatch <HOMZ: added cast>
//	{
//
//		byTemp = pchASCII[iLoopVar];
//
//
//
//		/* Truncate the BIT 7 & 6 and pack the unsigned char */
//
//		byTemp = byTemp & 0x3F;
//
//		byTemp = byTemp << 2;
//
//
//
//		switch(iASCIICount)
//
//		{
//
//			case 0:
//
//				{
//
//					/* Initialize the unsigned char stream */
//
//					pdwTemp = (DWORD *)pbyPackedASCII;
//
//					*pdwTemp = 0;
//
//
//
//					/* This is the first unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 24;
//
//
//
//					*pdwTemp = dwPackedASCIITemp;
//
//
//
//					iASCIICount++;
//
//					continue;
//
//				}
//
//			case 1:
//
//				{
//
//					/* This is the second unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 18;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					iASCIICount++;
//
//					continue;
//
//				}
//
//			case 2:
//
//				{
//
//					/* This is the third unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 12;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					iASCIICount++;
//
//					continue;
//
//				}
//
//			case 3:
//
//				{
//
//					/* This is the fourth & last unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 6;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					/* Now lets copy our unsigned char steam */
//
//					pbyPackedASCIIOutput [0 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0xff000000) >> 24);
//
//					pbyPackedASCIIOutput [1 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0x00ff0000) >> 16);
//
//					pbyPackedASCIIOutput [2 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0x0000ff00) >> 8);
//
//
//
//					iStreamIndex += 3;
//
//
//
//					iASCIICount = 0;
//
//					continue;
//
//				}
//
//
//
//		}
//
//
//
//	}
//
//
//
//	if (iASCIICount != 0)
//
//	{
//
//		byTemp = ' ';
//
//
//
//		/* Truncate the BIT 7 & 6 and pack the unsigned char */
//
//		byTemp = byTemp & 0x3F;
//
//		byTemp = byTemp << 2;
//
//
//
//		switch(iASCIICount)
//
//		{
//
//			case 1:
//
//				{
//
//					/* This is the second unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 18;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					/* This is the third unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 12;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					/* This is the fourth & last unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 6;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					/* Now lets copy our unsigned char steam */
//
//					pbyPackedASCIIOutput [0 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0xff000000) >> 24);
//
//					pbyPackedASCIIOutput [1 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0x00ff0000) >> 16);
//
//					pbyPackedASCIIOutput [2 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0x0000ff00) >> 8);
//
//
//
//					iStreamIndex += 3;
//
//
//
//					iASCIICount = 0;
//
//					break;
//
//				}
//
//			case 2:
//
//				{
//
//					/* This is the third unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 12;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					/* This is the fourth & last unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 6;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					/* Now lets copy our unsigned char steam */
//
//					pbyPackedASCIIOutput [0 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0xff000000) >> 24);
//
//					pbyPackedASCIIOutput [1 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0x00ff0000) >> 16);
//
//					pbyPackedASCIIOutput [2 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0x0000ff00) >> 8);
//
//
//
//					iStreamIndex += 3;
//
//
//
//					iASCIICount = 0;
//
//					break;
//
//				}
//
//			case 3:
//
//					/* This is the fourth & last unsigned char */
//
//					dwPackedASCIITemp = byTemp;
//
//
//
//					dwPackedASCIITemp = dwPackedASCIITemp << 6;
//
//
//
//					*pdwTemp = *pdwTemp | dwPackedASCIITemp;
//
//
//
//					/* Now lets copy our unsigned char steam */
//
//					pbyPackedASCIIOutput [0 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0xff000000) >> 24);
//
//					pbyPackedASCIIOutput [1 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0x00ff0000) >> 16);
//
//					pbyPackedASCIIOutput [2 + iStreamIndex] = (unsigned char) ((*pdwTemp & 0x0000ff00) >> 8);
//
//
//
//					iStreamIndex += 3;
//
//
//
//					iASCIICount = 0;
//
//					break;
//
//		}
//
//	}
//
//
//
//	*pwPackedASCIISize = iStreamIndex;
//
//
//
//	return;


//}/* End of Function: ASCIIToPackedASCII () */
#endif

