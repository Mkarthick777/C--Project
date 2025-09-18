#ifndef lint
static char SCCSID[] = "@(#)ddi_lang.c	40.2  40  07 Jul 1998";
#endif /* lint */

/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */

/*
 *  This file contains all of the functions for DDI language
 */
#include "pch.h"
#include <string.h>
#include <ctype.h>
#include <string>
#include "std.h"
#include "Retn_code.h"
#include "panic.h"
#include <string>
//#include "app_xmal.h"

#define D_CHAR	0X2A	/* default char for unconvertable characters */
						/* currently using asterisk '*' as default   */

#define DEF_LANG_CTRY		"|en|"	/* Of the form "|LL|" or "|LL CC|" */

#define COUNTRY_CODE_MARK 	'|'

static const
unsigned char   iso_to_pc[] = {

/* offset for character look up is 0XA0 */

	0XFF,			/* 0XA0		phantom space */
	0XAD,			/* 0XA1		opening exclamation */
	0X9B,			/* 0XA2		cent sign */
	0X9C,			/* 0XA3		pound sign */
	D_CHAR,			/* 0XA4 */
	0X9D,			/* 0XA5		yen sign */
	D_CHAR,			/* 0XA6 */
	0X15,			/* 0XA7		section sign */
	D_CHAR,			/* 0XA8 */
	D_CHAR,			/* 0XA9 */
	0X0C,			/* 0XAA		female sign */
	0XAE,			/* 0XAB		opening guillemets */
	D_CHAR,			/* 0XAC */
	D_CHAR,			/* 0XAD */
	D_CHAR,			/* 0XAE */
	D_CHAR,			/* 0XAF */

	0XF8,			/* 0XB0		degree */
	0XF1,			/* 0XB1		plus minus sign */
	0XFD,			/* 0XB2		superscript 2 */
	D_CHAR,			/* 0XB3 */
	D_CHAR,			/* 0XB4 */
	0XE6,			/* 0XB5		micro sign / mu */
	0X14,			/* 0XB6		paragraph sign */
	0XFE,			/* 0XB7		box */
	D_CHAR,			/* 0XB8 */
	D_CHAR,			/* 0XB9 */
	0X0B,			/* 0XBA		male sign */
	0XAF,			/* 0XBB		closing guillemets */
	0XAC,			/* 0XBC		1/4 */
	0XAB,			/* 0XBD		1/2 */
	D_CHAR,			/* 0XBE */
	0XA8,			/* 0XBF		opening question mark */

	0X41,			/* 0XC0		A */
	0X41,			/* 0XC1		A */
	0X41,			/* 0XC2		A */
	0X41,			/* 0XC3		A */
	0X8E,			/* 0XC4		A diaeresis */
	0X8F,			/* 0XC5		A ring */
	0X92,			/* 0XC6		AE diphthong */
	0X80,			/* 0XC7		C cedilla */
	0X45,			/* 0XC8		E */
	0X90,			/* 0XC9		E acute */
	0X45,			/* 0XCA		E */
	0X45,			/* 0XCB		E */
	0X49,			/* 0XCC		I */
	0X49,			/* 0XCD		I */
	0X49,			/* 0XCE		I */
	0X49,			/* 0XCF		I */

	0X44,			/* 0XD0		D */
	0XA5,			/* 0XD1		N tilde */
	0X4F,			/* 0XD2		O */
	0X4F,			/* 0XD3		O */
	0X4F,			/* 0XD4		O */
	0X4F,			/* 0XD5		O */
	0X99,			/* 0XD6		O diaeresis */
	0X78,			/* 0XD7		x */
	D_CHAR,			/* 0XD8 */
	0X55,			/* 0XD9		U */
	0X55,			/* 0XDA		U */
	0X55,			/* 0XDB		U */
	0X9A,			/* 0XDC		U diaeresis */
	0X59,			/* 0XDD		Y */
	D_CHAR,			/* 0XDE */
	D_CHAR,			/* 0XDF */

	0X85,			/* 0XE0		a grave */
	0XA0,			/* 0XE1		a acute */
	0X83,			/* 0XE2		a circumflex */
	0X61,			/* 0XE3		a */
	0X61,			/* 0XE4		a */
	0XC8,			/* 0XE5		a ring */
	0X91,			/* 0XE6		ae diphthong */
	0X87,			/* 0XE7		c cedilla */
	0X8A,			/* 0XE8		e grave */
	0X65,			/* 0XE9		e */
	0X88,			/* 0XEA		e circumflex */
	0X89,			/* 0XEB		e diaeresis */
	0X8D,			/* 0XEC		i circumflex */
	0XA1,			/* 0XED		i acute */
	0X8C,			/* 0XEE		i circumflex */
	0X8B,			/* 0XEF		i diaeresis */

	D_CHAR,			/* 0XF0 */
	0XA4,			/* 0XF1		n tilde */
	0X95,			/* 0XF2		o grave */
	0XA2,			/* 0XF3		o acute */
	0X93,			/* 0XF4		o circumflex */
	0X6F,			/* 0XF5		o */
	0X94,			/* 0XF6		o diaeresis */
	0XF6,			/* 0XF7		divide by sign */
	D_CHAR,			/* 0XF8 */
	0X97,			/* 0XF9		u grave */
	0XA3,			/* 0XFA		u acute */
	0X96,			/* 0XFB		u circumflex */
	0X75,			/* 0XFC		u */
	0X79,			/* 0XFD		y */
	D_CHAR,			/* 0XFE */
	0X98			/* 0XFF		y diaeresis */

};

#define PC_TABLE_OFFSET		0XA0
#define ISO_TABLE_OFFSET	0X7F
#define NO_CONVERSION		0X7F



#ifdef LIBS_ONLY
/*********************************************************************
 *
 *	Name: ddi_iso_to_pc
 *	ShortDesc: convert an ISO string to an ASCII string
 *
 *	Description:
 *		ddi_iso_to_pc takes a pointer to an ASCII string
 *		as input and allocates memory for a new string
 *		and then converts the ISO string to an ASCII string,
 *		storing the converted string in the malloced space.
 *
 *	Inputs:
 *		iso:	     a pointer to the ISO string
 *		buffer_size: the size of the inbound buffer
 *
 *	Outputs:
 *		buffer:	the loaded buffer
 *
 *	Returns:
 *		DDS_SUCCESS, DDI_INSUFFICIENT_BUFFER
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

int
ddi_iso_to_pc(char *iso, char *buffer, int buffer_size)
{

	size_t          inc;		/* incrementer */
	size_t          length;		/* length of string */
	unsigned char	*iso_temp;	/* temp unsigned char buffer */

	ASSERT_RET(iso && buffer && buffer_size, DDI_INVALID_PARAM);

	iso_temp = (unsigned char *) iso;
	length = strlen(iso);
	length++;		/* add the NULL character to the count */

	if (length > (size_t) buffer_size) {
		return DDI_INSUFFICIENT_BUFFER;
	}

	for (inc = 0; inc < length; inc++, buffer++, iso_temp++) {
		
		if (*iso_temp < NO_CONVERSION) {
			*buffer = (char) *iso_temp;	/* no conversion */

		} else if (*iso_temp < PC_TABLE_OFFSET) { 
			
			/*
			 * chunk of iso character set which is empty
			 */
			*buffer = 0X20;	/* space character */

		} else {
			*buffer = (char) iso_to_pc[*iso_temp - PC_TABLE_OFFSET];	/* conversion table */
		}
	}

	return DDS_SUCCESS;
}

static const
unsigned char   pc_to_iso[] = {

	D_CHAR,			/* 0X7F	 */

	0XC7,			/* 0X80		C cedilla	 */
	0XFC,			/* 0X81		u umlaut	 */
	0XE9,			/* 0X82		e acute	 */
	0XE2,			/* 0X83		a circumflex	 */
	0XE4,			/* 0X84		a umlaut	 */
	0XE0,			/* 0X85		a grave	 */
	0XE5,			/* 0X86		a ring	 */
	0XE7,			/* 0X87		c cedilla	 */
	0XEA,			/* 0X88		e circumflex	 */
	0XEB,			/* 0X89		e umlaut	 */
	0XE8,			/* 0X8A		e grave	 */
	0XEF,			/* 0X8B		i umlaut	 */
	0XEE,			/* 0X8C		i circumflex	 */
	0XEC,			/* 0X8D		i grave	 */
	0XC4,			/* 0X8E		A umlaut	 */
	0XC5,			/* 0X8F		A ring	 */

	0XC9,			/* 0X90		E acute	 */
	0XE6,			/* 0X91		ae	 */
	0XC6,			/* 0X92		AE	 */
	0XF4,			/* 0X93		o circumflex	 */
	0XF6,			/* 0X94		o umlaut	 */
	0XF2,			/* 0X95		o grave	 */
	0XFB,			/* 0X96		u circumflex	 */
	0XF9,			/* 0X97		u grave	 */
	0XFF,			/* 0X98		y umlaut	 */
	0XD6,			/* 0X99		O umlaut	 */
	0XDC,			/* 0X9A		U umlaut	 */
	0XA2,			/* 0X9B		cent sign	 */
	0XA3,			/* 0X9C		pound sign (British currency)	 */
	0XA5,			/* 0X9D		yen sign	 */
	D_CHAR,			/* 0X9E */
	D_CHAR,			/* 0X9F */

	0XE1,			/* 0XA0		a acute	 */
	0XED,			/* 0XA1		i acute	 */
	0XF3,			/* 0XA2		o acute	 */
	0XFA,			/* 0XA3		u acute	 */
	0XF1,			/* 0XA4		n tilde	 */
	0XD1,			/* 0XA5		N tilde	 */
	0XAA,			/* 0XA6		a macron (superscripted, underscored a)	 */
	0XBA,			/* 0XA7		o macron (superscripted, underscored o)	 */
	0XBF,			/* 0XA8		inverted question mark	 */
	D_CHAR,			/* 0XA9 */
	0XAC,			/* 0XAA		not sign	 */
	0XBD,			/* 0XAB		1/2	 */
	0XBC,			/* 0XAC		1/4	 */
	0XA1,			/* 0XAD		inverted exclamation mark	 */
	0XAB,			/* 0XAE		left angle quotation mark	 */
	0XBB,			/* 0XAF		right angle quotation mark	 */

	D_CHAR,			/* 0XB0 */
	0X02,			/* 0XB1		medium block */
	D_CHAR,			/* 0XB2 */
	0X19,			/* 0XB3 	single vertical */
	0X16,			/* 0XB4 	single right juncture */
	D_CHAR,			/* 0XB5 */
	D_CHAR,			/* 0XB6 */
	D_CHAR,			/* 0XB7 */
	D_CHAR,			/* 0XB8 */
	D_CHAR,			/* 0XB9 */
	D_CHAR,			/* 0XBA */
	D_CHAR,			/* 0XBB */
	D_CHAR,			/* 0XBC */
	D_CHAR,			/* 0XBD */
	D_CHAR,			/* 0XBE */
	0X0C,			/* 0XBF 	single upper right */

	0X0E,			/* 0XC0 	single lower left */
	0X17,			/* 0XC1 	single lower juncture */
	0X18,			/* 0XC2		single upper juncture */
	0X15,			/* 0XC3 	single left juncture */
	0X12,			/* 0XC4 	single horizontal */
	0X0F,			/* 0XC5 	single intersection */
	D_CHAR,			/* 0XC6 */
	D_CHAR,			/* 0XC7 */
	D_CHAR,			/* 0XC8 */
	D_CHAR,			/* 0XC9 */
	D_CHAR,			/* 0XCA */
	D_CHAR,			/* 0XCB */
	D_CHAR,			/* 0XCC */
	D_CHAR,			/* 0XCD */
	D_CHAR,			/* 0XCE */
	D_CHAR,			/* 0XCF */

	D_CHAR,			/* 0XD0 */
	D_CHAR,			/* 0XD1 */
	D_CHAR,			/* 0XD2 */
	D_CHAR,			/* 0XD3 */
	D_CHAR,			/* 0XD4 */
	D_CHAR,			/* 0XD5 */
	D_CHAR,			/* 0XD6 */
	D_CHAR,			/* 0XD7 */
	D_CHAR,			/* 0XD8 */
	0X0B,			/* 0XD9		single lower right */
	0X0D,			/* 0XDA 	single upper left */
	D_CHAR,			/* 0XDB */
	D_CHAR,			/* 0XDC */
	D_CHAR,			/* 0XDD */
	D_CHAR,			/* 0XDE */
	D_CHAR,			/* 0XDF */

	D_CHAR,			/* 0XE0 */
	D_CHAR,			/* 0XE1 */
	D_CHAR,			/* 0XE2 */
	D_CHAR,			/* 0XE3 */
	D_CHAR,			/* 0XE4 */
	D_CHAR,			/* 0XE5 */
	D_CHAR,			/* 0XE6 */
	D_CHAR,			/* 0XE7 */
	D_CHAR,			/* 0XE8 */
	D_CHAR,			/* 0XE9 */
	D_CHAR,			/* 0XEA */
	D_CHAR,			/* 0XEB */
	D_CHAR,			/* 0XEC */
	D_CHAR,			/* 0XED */
	D_CHAR,			/* 0XEE */
	D_CHAR,			/* 0XEF */

	D_CHAR,			/* 0XF0 */
	0X08,			/* 0XF1		plus/minus */
	0X1B,			/* 0XF2		greater/equal sign */
	0X1A,			/* 0XF3 	less/equal sign */
	D_CHAR,			/* 0XF4 */
	D_CHAR,			/* 0XF5 */
	0XF8,			/* 0XF6 	divide by sign */
	D_CHAR,			/* 0XF7 */
	0X07,			/* 0XF8 	degree */
	D_CHAR,			/* 0XF9 */
	D_CHAR,			/* 0XFA */
	D_CHAR,			/* 0XFB */
	D_CHAR,			/* 0XFC */
	0XB2,			/* 0XFD 	superscript 2 */
	0X1F,			/* 0XFE 	box */
	0XA0			/* 0XFF 	phantom space */
};


/*********************************************************************
 *
 *	Name: ddi_pc_to_iso
 *	ShortDesc: convert an ASCII string to an ISO string
 *
 *	Description:
 *		ddi_pc_to_iso takes a pointer to an ASCII string as input and
 *		allocates memory for a new string and then converts the ASCII string
 *		to an ISO string, storing the converted string in the malloced space.
 *
 *	Inputs:
 *		pc:		     a pointer to the ASCII string
 *      buffer_size: the length of the inbound buffer
 *
 *	Outputs:
 *		buffer: the loaded buffer
 *
 *	Returns:
 *		DDS_SUCCESS, DDI_MEMORY_ERROR
 *
 *	Author: Chris Gustafson
 *
 **********************************************************************/

int
ddi_pc_to_iso(char *pc, char *buffer, int buffer_size)
{

	size_t          inc;	    /* incrementer */
	size_t          length;	    /* length of string */
	unsigned char	*pc_temp;	/* temp unsigned char buffer */

	ASSERT_RET(pc && buffer && buffer_size, DDI_INVALID_PARAM);

	pc_temp = (unsigned char *) pc;
	length = strlen(pc);
	length++;		/* add the NULL character to the count */

	if (length > (size_t) buffer_size) {
		return DDI_INSUFFICIENT_BUFFER;
	}

	for (inc = 0; inc < length; inc++, buffer++, pc_temp++) {

		if (*pc_temp < NO_CONVERSION) {
			*buffer = (char) *pc_temp;	/* no conversion */

		} else {
			*buffer = (char) pc_to_iso[*pc_temp - ISO_TABLE_OFFSET];	/* conversion table */
		}
	}

	return DDS_SUCCESS;
}
#endif /* LIBS_ONLY */


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
ddi_get_string_translation(char *string, char *lang_cntry, char *outbuf,
		int outbuf_size)
{

	char	*ci;			/* input character pointer */
	char	*co;			/* output character pointer */
	char	*first_phrp;		/* first phrase pointer */
	char	*lang_cntry_phrp;	/* language + country phrase pointer */
	char	*lang_only_phrp;	/* language-only phrase pointer */
	std::string lang_only;
	//char	lang_only[5];	/* language-only pulled from language/country code */
	char	*new_def_phrp;	/* new-style default phrase pointer */
	char	*old_def_phrp;	/* old-style language-only phrase pointer */
	char	*out_phrp;		/* output phrase pointer */
	int		code_length;	/* length of language/country code, in characters */

	/*
	 *	Make all the phrase pointers null pointers.
	 */
	first_phrp = 0;
	lang_cntry_phrp = 0;
	lang_only_phrp = 0;
	new_def_phrp = 0;
	old_def_phrp = 0;
	out_phrp = 0;

	/*
	 *	If the input string is a null string (which is legal), we can
	 *	skip all the string processing and return the output string,
	 *	which we set to a null string.
	 */
	if (string[0] == '\0') {
		outbuf[0] = '\0';
		return(DDS_SUCCESS);
	}

	/*
	 *	If the input country_code is full-size (i.e., seven characters),
	 *	extract the language code from the language/country code.
	 *	Otherwise, make the language-only code a null string.
	 */
	//if (strlen(lang_cntry) == 7) {
	//	(void)strncpy(lang_only, lang_cntry, (size_t)3);
	//	lang_only[3] = COUNTRY_CODE_MARK;
	//	lang_only[4] = '\0';
	//} else {
	//	lang_only[0] = '\0';
	//}
	// modified by SS
	if (strlen(lang_cntry) == 7) {
		// Extract the language-only code
		lang_only = std::string(lang_cntry, 3) + COUNTRY_CODE_MARK;
	}
	else {
		// If lang_cntry doesn't have exactly 7 characters, set lang_only to an empty string
		lang_only.clear();
	}

	/*
	 *	Check to see if the input string begins with a COUNTRY_CODE_MARK.
	 *	If it does not, set the first-phrase pointer, then enter the loop.
	 */
	if (string[0] != COUNTRY_CODE_MARK) {
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

	for (co = outbuf, ci = string; *ci; ci++) {

	/*
	 *	Look for the complete language/country code.
	 */
		if ((ci[0] == COUNTRY_CODE_MARK) && isalpha(ci[1]) &&
				isalpha(ci[2]) && (ci[3] == ' ') && isalpha(ci[4])
				&& isalpha(ci[5]) && (ci[6] == COUNTRY_CODE_MARK)) {
			code_length = 7;

			/*if ((lang_cntry_phrp == 0) &&
					(string.Compare(ci, lang_cntry, code_length) == 0)) {
				lang_cntry_phrp = co + 1;
			}

			if ((new_def_phrp == 0) &&
					(strncmp(ci, DEF_LANG_CTRY, code_length) == 0)) {
				new_def_phrp = co + 1;
			}*/
			//modified by SS

			std::string code(ci, code_length);
			// Compare the language/country code with lang_cntry
			if ((lang_cntry_phrp == nullptr) && (code.compare(0, code_length, lang_cntry) == 0)) {
				lang_cntry_phrp = co + 1;
			}
			// Compare the language/country code with DEF_LANG_CTRY
			if ((new_def_phrp == nullptr) && (code.compare(0, code_length, DEF_LANG_CTRY) == 0)) {
				new_def_phrp = co + 1;
			}

			if (first_phrp == 0) {
				first_phrp = co + 1;
			}

			*co++ = '\0';
			ci += (code_length - 1);

	/*
	 *	Look for the language-only code (new style).
	 */
		} else if ((ci[0] == COUNTRY_CODE_MARK) && isalpha(ci[1]) &&
				isalpha(ci[2]) && (ci[3] == COUNTRY_CODE_MARK)) {
			code_length = 4;
			std::string code(ci, code_length);
			/*if ((lang_cntry_phrp == 0) &&
					(strncmp(ci, lang_cntry, code_length) == 0)) {
				lang_cntry_phrp = co + 1;
			}

			if ((lang_only_phrp == 0) && (lang_only[0] != '\0') &&
					(strncmp(ci, lang_only, code_length) == 0)) {
				lang_only_phrp = co + 1;
			}

			if ((new_def_phrp == 0) &&
					(strncmp(ci, DEF_LANG_CTRY, code_length) == 0)) {
				new_def_phrp = co + 1;
			}*/

			if ((lang_cntry_phrp == 0) &&
				(code.compare(0, code_length,lang_cntry ) == 0)) {
				lang_cntry_phrp = co + 1;
			}

			if ((lang_only_phrp == 0) && (lang_only[0] != '\0') &&
				(code.compare(0, code_length, lang_only ) == 0)) {
				lang_only_phrp = co + 1;
			}

			if ((new_def_phrp == 0) &&
				(code.compare(0, code_length, DEF_LANG_CTRY ) == 0)) {
				new_def_phrp = co + 1;
			}



			if (first_phrp == 0) {
				first_phrp = co + 1;
			}

			*co++ = '\0';
			ci += (code_length - 1);

	/*
	 *	Look for the language-only code (old style); default only.
	 */
		} else if ((ci[0] == COUNTRY_CODE_MARK) && isdigit(ci[1]) &&
				isdigit(ci[2]) && isdigit(ci[3])) {
			code_length = 4;
			std::string code(ci, code_length);

			if ((old_def_phrp == 0) &&
					(code.compare(0, code_length, "|001" ) == 0)) {
				old_def_phrp = co + 1;
			}

			if (first_phrp == 0) {
				first_phrp = co + 1;
			}

			*co++ = '\0';
			ci += (code_length - 1);

	/*
	 *	If the escape sequence character (\) is encountered, convert
	 *	the following character as required.  These are the escape
	 *	sequences required by the DDL Spec.
	 */

		} else if (*ci == '\\') {

			switch (*(ci + 1)) {

				case 'a':
					*co++ = '\a';
					ci++;
					break;

				case 'f':
					*co++ = '\f';
					ci++;
					break;

				case 'n':
					*co++ = '\n';
					ci++;
					break;

				case 'r':
					*co++ = '\r';
					ci++;
					break;

				case 't':
					*co++ = '\t';
					ci++;
					break;

				case 'v':
					*co++ = '\v';
					ci++;
					break;

				default:
					*co++ = *(ci + 1);
					ci++;
					break;
			}

	/*
	 *	This is the 'normal' case; this character has no special
	 *	significance, so just copy it to the output pointer.
	 */
		} else {
			*co++ = *ci;
		}
	}

	/*
	 *	Tack an end-of-string character onto the final phrase.
	 */
	*co++ = '\0';

	/*
	 *	We may have found a phrase to output.  Copy the highest priority
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

	if (lang_cntry_phrp) {
		out_phrp = lang_cntry_phrp;
	} else if (lang_only_phrp) {
		out_phrp = lang_only_phrp;
	} else if (new_def_phrp) {
		out_phrp = new_def_phrp;
	} else if (old_def_phrp) {
		out_phrp = old_def_phrp;
	} else {
		out_phrp = first_phrp;
	}

	/*
	 *	Check the length of the output buffer.  If the phrase to be output
	 *	is longer than the output buffer, return an error code.  Otherwise,
	 *	copy the phrase in the holding buffer into the output buffer.  
	 */
	if ((size_t) outbuf_size < strlen(out_phrp)) {
		outbuf[0] = '\0';
		return DDI_INSUFFICIENT_BUFFER;
	} else {
		//(void)strcpy(outbuf, out_phrp);
		std::copy_n(out_phrp, strlen(out_phrp) + 1, outbuf);
	}

	return DDS_SUCCESS;
}
