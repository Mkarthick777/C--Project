#ifndef lint
static char SCCSID[] = "@(#)dict.c	40.1  40  07 Jul 1998";
#endif /* lint */

/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */
#include "pch.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "std.h"
#include "dict.h"
//#include "ddi_lib.h"
#include "retn_code.h"
#include "panic.h"
#include <cassert>

/*
 * DDI language
 */

extern int ddi_iso_to_pc (char *, char *, int);
extern int ddi_pc_to_iso (char *, char *, int);
extern int ddi_get_string_translation (char *, char *, char *, int);

extern char langCode[4];


#ifdef SUN
extern int fprintf();
extern int fclose();
#endif


/*
 * Set this and error messages will contain the name of the
 * program (see dicterr() for details).
 */
char *dict_program;

/*
 *	Formats for error messages
 */
static char *errfmt[] = {
	(char*)"string for language '%s' not found in\n\t%s",
	(char*)"duplicate entries found for section %d, offset %d",
	(char*)"non contiguous entries, no entries between offsets %d and %d, sect %d",
	(char*)"invalid start of section %d",
	(char*)"high text marker not highest offset in section %d",
	(char*)"high text marker is missing for section %d",
};

/*
 *	The special text the must be the highest offset for each section
 */
static char high_text[] = "@XX@Highest Offset";

/*
 * Info kept in globals for efficiency.
 */
static int dicterrs;
static FILE *dictfp;
static int dictline;
static char *dictfile;

/*
 * The token buffer.
 */
static char *tokbase;
static char *tokptr;
static char *toklim;

/*
 * Token values (lex() return values).
 */
#define ERROR 0
#define DONE 1
#define NUMBER 2
#define NAME 3
#define TEXT 4
#define COMMA 5

/*
 *	Local structures for reading in the entire dictionary file
 *	to allow for sorting.
 */

typedef struct
{
	unsigned long	section;	/* The entry section number*/
	unsigned long	offset;		/* The entry number within the section */
	unsigned long	value;		/* The value is computed from section and offset*/
	char			*name;		/* The name of the entry*/
	char 			*dict_text;	/* The text for the entry*/
} DICT_ENT;

static DICT_ENT *dict_array;	/* Array of DICT_ENTs */
static int 		dict_limit;		/* Size of dict_array */
static int 		dict_count;		/* Number of entries in dict_array */


extern unsigned long 		num_dict_table_entries;
extern DICT_TABLE_ENTRY 	dict_table[MAX_DICT_TABLE_SIZE];
extern void dict_table_install (ulong ref, char *name, char *str);


/*********************************************************************
 *
 *	Name: dicterr
 *	ShortDesc: print error message
 *
 *	Description:
 *		Print an error message on standard error and returns.
 *		If the global variable "dict_program" is set, it is printed
 *		before the text of the error message.
 *
 *		We use this function instead of the other error printing
 *		functions we have (such as error(), panic(), etc), so that
 *		this file remains as self contained as possible. 
 *
 *	Inputs:
 *		msg - error message to print
 *
 *	Outputs:
 *		None
 *
 *	Returns:
 *		Nothing
 *
 *	Author:
 *		Jon Westbrock
 *
 *********************************************************************/

static void
dicterr(std::string msg)

{

	++dicterrs;
	if (dict_program != (char *) 0)
	{
		fprintf(stderr, "%s: ", dict_program);
	}
	fprintf(stderr, "\"%s\", ", dictfile);
	if (dictline)
	{
		fprintf(stderr, "line %d: ", dictline);
	}
	fprintf(stderr, "%s\n", msg.c_str());
}


/*********************************************************************
 *
 *	Name: inschar
 *	ShortDesc: insert a character in the token buffer
 *
 *	Description:
 *		Insert a character into the token buffer (tokbase, tokptr,
 *		toklim).  The token buffer is always kept null terminated,
 *		and is grown as needed.
 *
 *	Inputs:
 *		c - character to be inserted
 *
 *	Outputs:
 *		None
 *
 *	Returns:
 *		Nothing
 *
 *	Author:
 *		Jon Westbrock
 *
 *********************************************************************/

static void
inschar(int	c)

{

	int		off, size;

	/*
	 * Make sure token buffer exists, and can accommodate another
	 * character.  We keep the token buffer null terminated, hence
	 * the strange limit check.
	 */
	if (tokbase == (char *) 0 || tokptr + 1 == toklim)
	{
		off = tokptr - tokbase;
		size = toklim - tokbase;
		if (size == 0)
		{
			size = 32;
			tokbase = (char *) new char[((unsigned)size)];
		}
		else
		{
			char TempToken[10000];
			memcpy(TempToken, tokbase,size);

			size *= 2;

			delete tokbase;			
			tokbase = new char[size];
			memcpy(tokbase, TempToken,size/2);
		}
		if (tokbase == (char *) 0)
		{
			dicterr("memory exhausted");
			exit(-1);
		}
		tokptr = tokbase + off;
		toklim = tokbase + size;
	}

	/*
	 * Insert the character, keeping the buffer null terminated.
	 */
	*tokptr++ = (char) c;
	*tokptr = 0;
}


/*********************************************************************
 *
 *	Name: comment
 *	ShortDesc: reads a C style comment
 *
 *	Description:
 *		Read a C style comment from the current dictionary
 *		file (dictfp). If an EOF is seen prior to the closing
 * 		of the comment, an error is returned.
 *
 *	Inputs:
 *		None		
 *
 *	Outputs:
 *		None
 *
 *	Returns:
 *		DONE - if successful
 *		ERROR - otherwise
 *
 *	Author:
 *		Jon Westbrock
 *
 *********************************************************************/

static int
comment()
{

	int		c;

	/*
	 * Verify we're at the start of a comment. If we don't
	 * find a valid start of comment, return an error.
	 */
	c = getc(dictfp);
	if (c != '*')
	{
		return ERROR;
	}

	/*
	 * Consume the comment.
	 */
	c = getc(dictfp);
	for (;;)
	{
		if (c == EOF)
		{
			return ERROR;
		}

		/*
		 * If it's a *, check for end of comment.
		 */
		if (c == '\n')
		{
			++dictline;
		}
		else if (c == '*')
		{
			c = getc(dictfp);
			if (c == EOF)
			{
				return ERROR;
			}
			if (c == '\n')
			{
				++dictline;
			}
			else if (c == '/')
			{
				break;
			}
			else
			{
				continue;
			}
		}
		/*
		 * Next please.
		 */
		c = getc(dictfp);
	}

	/*
	 * Indicates valid comment consumed.
	 */
	return DONE;
}


/*********************************************************************
 *
 *	Name:  lex
 *	ShortDesc: standard dictionary lexical analyser
 *
 *	Description:
 *		Reads characters from the current dictionary file (dictfp) and
 *		translates them into tokens, which are parsed by parse() (see below).
 *
 *	Inputs:
 *		None
 *
 *	Outputs:
 *		None
 *
 *	Returns:
 *		ERROR  - error in the input
 *		DONE   - end of file
 *		NUMBER - standard dictionary number
 *		NAME   - standard dictionary name
 *		TEXT   - text of a standard dictionary string
 *		COMMA  - the comma character
 *
 *	Author:
 *		Jon Westbrock
 *
 *********************************************************************/

static int
lex()
{

	int			c;
	static int	nextchar = -1;

	/*
	 * Get next character.
	 */
	if (nextchar < 0)
	{
		c = getc(dictfp);
	}
	else
	{
		c = nextchar;
		nextchar = -1;
	}

	for (;;)
	{
		switch (c)
		{
			case EOF:
				return DONE;

			/*
			 * Eat whitespace.
			 */

			case '\n':
				++dictline;
				/* FALL THROUGH */
	
			case ' ':
			case '\b':
			case '\f':
			case '\r':
			case '\t':
			case '\v':
				c = getc(dictfp);
				break;
	
			/*
			 * Eat a comment.
			 */

			case '/':
				if (comment() != DONE)
				{
					return ERROR;
				}
				c = getc(dictfp);
				break;
	
			case ',':
				return COMMA;
	
			/*
			 * Read a number.
			 */

			case '[':
				/*
				 * Clear the token buffer.
				 */
				tokptr = tokbase;
	
				/*
				 * Read a digit sequence.
				 */
				do
				{
					inschar(c);
					c = getc(dictfp);
				} while (isdigit(c));
	
				/*
				 * Read a comma.
				 */

				if (c != ',')
				{
					return ERROR;
				}
	
				/*
				 * Read another digit sequence.
				 */

				do
				{
					inschar(c);
					c = getc(dictfp);
				} while (isdigit(c));
	
				/*
				 * Verify it ends correctly.
				 */

				if (c != ']')
				{
					return ERROR;
				}
				inschar(c);
	
				/*
				 * Return it.
				 */

				return NUMBER;
	
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
			case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
			case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
			case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
			case 'Y': case 'Z': case '_': case 'a': case 'b': case 'c':
			case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':
			case 'j': case 'k': case 'l': case 'm': case 'n': case 'o':
			case 'p': case 'q': case 'r': case 's': case 't': case 'u':
			case 'v': case 'w': case 'x': case 'y': case 'z':

				/*
				 * Clear the token buffer.
				 */

				tokptr = tokbase;
	
				/*
				 * Read a sequence of alphanumerics and underscores.
				 */

				do
				{
					inschar(c);
					c = getc(dictfp);
				} while (isalnum(c) || c == '_');
	
				/*
				 * Save the lookahead character.
				 */

				nextchar = c;
	
				/*
				 * Return it.
				 */

				return NAME;
	
			case '\"':
				/*
				 * Clear the token buffer.
				 */

				tokptr = tokbase;
	
				/*
				 * If string does not start with a country code,
				 * add the default country code (English).
				 */

				c = getc(dictfp);
				if (c != '|')
				{
					inschar('|');
					inschar('e');
					inschar('n');
					inschar('|');
				}
	
				/*
				 * Read the characters of the string.
				 */

				for (;; c = getc(dictfp))
				{
					switch (c)
					{
						/*
						 * Unterminated string.
						 */

						case EOF:
						case '\n':
							return ERROR;
		
						/*
						 * Anything can be escaped with a backslash,
						 * this will usually be \".
						 */

						case '\\':
							inschar(c);
							c = getc(dictfp);
							if (c == EOF)
							{
								return ERROR;
							}
							inschar(c);
							break;
		
						/*
						 * We've come to the end of the string. Read ahead
						 * to see if there is string immediately following
						 * this one, and if there is, read that one also.
						 */

						case '\"':
							c = getc(dictfp);
							for (;;)
							{
								switch (c)
								{
									/*
									 * Eat whitespace.
									 */

									case '\n':
										dictline++;
										/* FALL THROUGH */
			
									case ',':
									case ' ':
									case '\b':
									case '\f':
									case '\r':
									case '\t':
										c = getc(dictfp);
										continue;
			
									/*
									 * Eat a comment.
									 */

									case '/':
										if (comment() != DONE)
										{
											return ERROR;
										}
										c = getc(dictfp);
										continue;
			
									/*
									 * We've seen something other than
									 * a comment or whitespace.
									 */

									default:
										break;
								}
								break;
							}
		
							/*
							 * If what we've seen is the start
							 * of another string, read it also.
							 */

							if (c == '\"')
								break;
		
							/*
							 * We've seen something other than the start
							 * of a string.  Save the lookahead character.
							 */

							nextchar = c;
							return TEXT;
		
						/*
						 * Ordinary character, add it to the token buffer.
						 */

						default:
							inschar(c);
							break;
					}
				}
				/*NOTREACHED*/

			/*
			 * Invalid input.
			 */
			default:
				return ERROR;
		}
	}
}


/*********************************************************************
 *
 *	Name: parse
 *	ShortDesc: standard dictionary parser
 *
 *	Description:
 *		Parses the current dictionary file (dictfp). Calls lex()
 *		to get tokens. When an error is detected, it resynchronizes
 *		(by looking for the beginning of a valid dictionary entry)
 *		and continues parsing from that point. This allows multiple
 *		errors in a dictionary file to be detected at once.
 *
 *	Inputs:
 *		None
 *
 *	Outputs:
 *		None
 *
 *	Returns:
 *		Nothing
 *
 *	Author:
 *		Jon Westbrock
 *
 *********************************************************************/

static void
parse()
{

	int				tok, rc;
	char			*last, *name;
	unsigned long	section, offset;
	char			*strbuf;
	DICT_ENT		*curr_dict, *end_dict;

	/*
	 * Initialize.
	 */
	tok = lex();
	curr_dict = dict_array + dict_count;
	end_dict = dict_array + dict_limit;

	while (tok != DONE)
	{
		/*
		 * Verify the token is a number.
		 */

		if (tok != NUMBER)
		{
			dicterr("syntax error");
			do
			{ 
				tok = lex();
			} while (tok != DONE && tok != NUMBER);
			continue;
		}

		/*
		 * Extract the section number and offset from the token.
		 */

		section = strtoul(tokbase + 1, &last, 10);
		offset = strtoul(last + 1, (char **) 0, 10);

		/*
		 * Get next token and verify it's a string name.
		 */

		tok = lex();
		if (tok != NAME)
		{
			dicterr("syntax error");
			do
			{
				tok = lex();
			} while (tok != DONE && tok != NUMBER);
			continue;
		}

		/*
		 * Save the name.
		 */

		name = (char *) new char[((unsigned)(strlen(tokbase) + 1))];
		if (name == (char *) 0)
		{
			dicterr("memory exhausted");
			exit(-1);
		}
#ifdef MODIFIED_BY_SS
		std::string stokbase = tokbase;
		std::copy(stokbase.begin(), stokbase.end(), name);

		//strcpy(name, tokbase);
#endif

		

		/*
		 * Get the next token and verify it's a string.
		 */

		tok = lex();
		if (tok != TEXT)
		{
			dicterr("syntax error");
			do
			{
				tok = lex();
			} while (tok != DONE && tok != NUMBER);
			continue;
		}

		/*
		 *	Move the string of the desired language to the top of the buffer.
		 *	This is done entirely within the buffer.
		 */
		/*Vibhor 201003: Changed second parameter to langCode from "|en|" */

		rc = ddi_get_string_translation(tokbase, langCode, tokbase, strlen(tokbase) + 1);

		/*
		 *	Save the desired language.  Make sure that the string ends with
		 *	an end-of-string character.
		 */

		if (rc != DDS_SUCCESS)
		{
			tokbase[0] = '\0';
		}

		strbuf = (char *) new char[((unsigned)(strlen(tokbase) + 1))];
		if (strbuf == (char *) 0)
		{
			dicterr("memory exhausted");
			exit(-1);
		}
#ifdef MODIFIED_BY_SS
	    stokbase = tokbase;
		std::copy(stokbase.begin(), stokbase.end(), strbuf);

		//strcpy (strbuf, tokbase);
#endif
		/*
		 *	Save this entry into dict_array, expanding it if necessary.
		 */

		if (curr_dict == end_dict)
		{
			dict_count = dict_limit;
			dict_limit *= 2;
			dict_array = (DICT_ENT *)realloc ((char *)dict_array,
							(unsigned)(dict_limit * sizeof (*dict_array)));
			if (!dict_array)
			{
				dicterr("memory exhausted");
				exit(-1);
			}
			curr_dict = dict_array + dict_count;
			end_dict = dict_array + dict_limit;
		}
		curr_dict->section = section;
		curr_dict->offset = offset;
		curr_dict->value = section * 65536 + offset;
		curr_dict->name = name;
		curr_dict->dict_text = strbuf;
		curr_dict++;

		/*
		 *	Get the next token
		 */

		tok = lex();
	}
	dict_count = curr_dict - dict_array;
}


/*********************************************************************
 *
 *	Name: compdict
 *	ShortDesc: Compare function for qsort of DICT_ENTs
 *
 *	Description
 *		Compares two DICT_ENT entries, and returns less than zero,
 *		equal zero, or greater than zero depending on whether the
 *		first entry is smaller, equal, or greater than the second.
 *
 *	Inputs:
 *		dict_ent1, dict_ent2 - pointers to the two DICT_ENT entries
 *
 *	Outputs:
 *		None
 *
 *	Returns:
 *		< 0 - *dict_ent1 < *dict_ent2
 *		  0 - *dict_ent1 == *dict_ent2
 *		> 0 - *dict_ent1 > *dict_ent2
 *
 *	Author:
 *		Bruce Davis
 *********************************************************************/

static int
compdict (const void *dict_ent1,const void *dict_ent2)

{


	return (int)(((DICT_ENT*)dict_ent1)->value - ((DICT_ENT*)dict_ent2)->value);
}


/*********************************************************************
 *
 *	Name: makedict
 *	ShortDesc: build a dictionary table
 *
 *	Description:
 *		Reads a dictionary formatted file and creates a user defined
 *		dictionary table. The caller must supply an install function
 *		which is called every time a valid dictionary entry is read.
 *		This allows the caller to create as large or small a table
 *		as needed, and allows the complete control over the structure
 *		of the table.
 *
 *		The install function takes three arguments:
 *			unsigned long value - string number
 *			char *name 			- string name
 *			char *text			- text of string
 *
 *		The name and the text arguments are malloc'd, and if the
 *	 	caller does not store them, they should be free'd. If a text
 *		string does not contain the requested language (global variable
 *		"language"), the text argument may be 0. This situation is
 *		flagged as an error, so exiting if the dictionary is not
 *		correctly parsed avoids this situation.
 *
 *      If the add_file input is not null, then the specified file is
 *      also read and correctly formatted information is added to the
 *      table.
 *
 *	Inputs:
 *		file     - file name of dictionary file
 *		add_file - name of additional dictionary file
 *		func     - install function
 *
 *	Outputs:
 *		None
 *
 *	Returns:
 *		Number of error detected.
 *
 *	Author:
 *		Jon Westbrock
 *
 *  Modified:
 *		Del Fredricks
 *
 *********************************************************************/

/*Vibhor 141003: Modifying the definition to load upto "4" additional dictionaries
				which are specified as add_file1 to add_file4*/

int
makedict(char *file, char **addnl_file_array, void (*func) (ulong , char *,char *))
{

	DICT_ENT	*curr_dict, *end_dict, *next_dict;
	char		*errbuf;
#ifdef MODIFIED_BY_SS
	assert(addnl_file_array);
	//ASSERT_DBG(addnl_file_array);
#endif
	int iAddnlFileIndex;
	/*
	 * Initialize the globals.
	 */
	dicterrs = 0;
	dictline = 1;

	/*
	 *	Create the DICT_ENT pointer array.
	 */

	//dict_limit = 200; 
	/*Vibhor 141003: Increasing the dict limit to 500 as we are supporting upto 
					 4 additional dictionaries 	*/
	dict_limit = 500;
	dict_count = 0;
	dict_array = (DICT_ENT *)new DICT_ENT[((unsigned)(dict_limit))];
	if (!dict_array) {
		dicterr("memory exhausted");
		exit(-1);
	}

	dictfile = file;

	iAddnlFileIndex = 0;

	while (dictfile)
	{

		/*
		 * Open the file.
		 */
#ifdef MODIFIED_BY_SS

		FILE* dictfp;
		if (fopen_s(&dictfp, dictfile, "r") != 0 || !dictfp)
		/*dictfp = fopen(dictfile, "r");
		if (!dictfp)
		{
			if (dict_program != (char*)0)
			{
				fprintf(stderr, "%s: ", dict_program);
			}
			fprintf(stderr, "can't open dictionary \"%s\"\n", dictfile);
			exit(-1);
		}*/

#endif	
		/*
		 * Read it.
		 */
		parse();
	
		/*
		 *	Sort the entries from the files
		 */

		(void)qsort((char *)dict_array, dict_count, sizeof(*dict_array), compdict);

		(void) fclose(dictfp);

		if (dictfile == addnl_file_array[iAddnlFileIndex])
		{
			dictfile = NULL;
		}
		else
		{
			dictfile = addnl_file_array[iAddnlFileIndex++];
		}
	}

	/*
	 *	Step through the sorted entries, verifying them and passing
	 *	them to the use supplied function.  Verification includes
	 *	that the offsets are continguous within a section, that each
	 *	section starts with offset zero, and that each section ends
	 *	with a special entry with text matching high_text.
	 *
	 *	Dictline is set to zero to signal dicterr() not to print
	 *	and line numbers.
	 */

	dictline = 0;
	curr_dict = dict_array;
	end_dict = dict_array + dict_count - 1;
	for (; curr_dict <= end_dict; curr_dict++)
	{
		/*
		 *	If this is the last entry, it must be a high text entry.
		 */

		if (curr_dict == end_dict)
		{
			if (strcmp (curr_dict->dict_text, high_text))
			{
				
#ifdef MODIFIED_BY_SS
				/*errbuf = (char*)new char[((unsigned)(strlen(errfmt[5]) + 20))];
				if (errbuf == (char*)0)
				{
					dicterr("memory exhausted");
					exit(-1);
				}*/
				std::ostringstream oss;
				oss << errfmt[5] << curr_dict->section;
				std::string errbuf = oss.str();
				dicterr(errbuf);
				//delete (errbuf);
				//(void)sprintf(errbuf, errfmt[5], curr_dict->section);
				// dicterr(errbuf);
				// delete (errbuf);

#endif

			}

			/*
			 *	This is a "hidden" entry, so don't pass it to the
			 *	user function.
			 */
			delete (curr_dict->name);
			delete (curr_dict->dict_text);
			continue;
		}

		/*
		 *	Each entry must be unique.
		 */

		next_dict = curr_dict + 1;
		if (curr_dict->value == next_dict->value)
		{
			
#ifdef MODIFIED_BY_SS
			//errbuf = (char*)new char[((unsigned)(strlen(errfmt[1]) + 20))];
			//if (errbuf == (char*)0)
			//{
			//	dicterr("memory exhausted");
			//	exit(-1);
			//}
			std::ostringstream oss;
			oss << errfmt[1] << curr_dict->section;
			std::string errbuf = oss.str();
			dicterr(errbuf);

			/*(void)sprintf(errbuf, errfmt[1], curr_dict->section, 
						curr_dict->offset);
			dicterr (errbuf);
			delete (errbuf);*/
			curr_dict++;
#endif
			continue;
		}
 
		/*
		 *	If this entry is the special high text string ...
		 */

		if (curr_dict->dict_text && strcmp (curr_dict->dict_text, high_text) == 0)
		{
			/*
			 *	... then the next section must be different than
			 *	this section, and ...
			 */

			delete (curr_dict->name);
			if (curr_dict->section == next_dict->section)
			{
				

#ifdef MODIFIED_BY_SS
		/*		errbuf = (char*)new char[((unsigned)(strlen(errfmt[4]) + 20))];
				if (errbuf == (char*)0)
				{
					dicterr("memory exhausted");
					exit(-1);
				}*/
				std::ostringstream oss;
				oss << errfmt[4] << curr_dict->section;
				std::string errbuf = oss.str();
				dicterr(errbuf);

				/*(void)sprintf(errbuf, errfmt[4], curr_dict->section);
				dicterr (errbuf);
				delete (errbuf);*/
				
#endif
				
			}
			else if (next_dict->offset != 0)
			{

				/*
				 *	... the first offset in the next section must
				 *	be zero.
				 */

			

#ifdef MODIFIED_BY_SS
				 /*	errbuf = (char*)new char[((unsigned)(strlen(errfmt[3]) + 20))];
				 if (errbuf == (char *) 0)
				 {
					 dicterr("memory exhausted");
					 exit(-1);
				 }*/
				std::ostringstream oss;
				oss << errfmt[3] << next_dict->section;
				std::string errbuf = oss.str();
				dicterr(errbuf);

				/*(void)sprintf(errbuf, errfmt[3], next_dict->section);
				dicterr (errbuf);
				delete (errbuf);*/

#endif
				
			}

			/*
			 *	This is a "hidden" entry, so don't pass it to the
			 *	user function.
			 */

			delete (curr_dict->dict_text);
			continue;

		}
		else
		{
			/*
			 *	This is a "regular" entry.  It must not be the
			 *	last entry in a section.
			 */

			if (curr_dict->section != next_dict->section)
			{
#ifdef MODIFIED_BY_SS
				/*		errbuf = (char*)new char[((unsigned)(strlen(errfmt[4]) + 20))];
						if (errbuf == (char*)0)
						{
							dicterr("memory exhausted");
							exit(-1);
						}*/
				std::ostringstream oss;
				oss << errfmt[5] << curr_dict->section;
				std::string errbuf = oss.str();
				dicterr(errbuf);
				
				//(void)sprintf(errbuf, errfmt[5], curr_dict->section);
				//dicterr (errbuf);
				//delete (errbuf)
#endif
			}
			else if (curr_dict->offset != next_dict->offset - 1)
			{
				/*
				 *	The offset of the next entry must be one larger
				 *	than this one.
				 */
#ifdef MODIFIED_BY_SS
				 /*		errbuf = (char*)new char[((unsigned)(strlen(errfmt[2]) + 40))];
						if (errbuf == (char *) 0)
						{
							dicterr("memory exhausted");
							exit(-1);
						}*/
					
				std::ostringstream oss;
				oss << errfmt[5] << curr_dict->offset << next_dict->offset << curr_dict->section;
				std::string errbuf = oss.str();
				dicterr(errbuf);
				
				/*(void)sprintf(errbuf, errfmt[2], curr_dict->offset, 
							next_dict->offset, curr_dict->section);
				dicterr (errbuf);
				delete (errbuf);*/
#endif
			}

			/*
			 *	Call the install function.
			 */

			(*func) (curr_dict->value, curr_dict->name, curr_dict->dict_text);
		}
	}
		
	/*
	 * Tidy up.
	 */
	if (tokbase)
	{
		delete (tokbase);
		tokbase = NULL;
		tokptr = NULL;
		toklim = NULL;
	}
	delete ((char *)dict_array);

	/*
	 * Return number of errors seen.
	 */
	return dicterrs;
}
