//sjv06feb06 #include <windows.h>

#pragma warning (disable : 4786)
#include "pch.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

#include "ddbGeneral.h"

#ifdef _WIN32_WCE
  #include <windows.h>	// added for FindFirstFile function PAW 23/04/09
  #include <atlbase.h>
  #define B_SEARCH	binary_search
#else
  // removed to stop compiler error win ce PAW 09/04/09
  #ifndef linux
    #include <io.h>
  #else
    #include "DDFileOps.h"
  #endif
  #define B_SEARCH	bsearch
#endif
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "retn_code.h"

//ifstream dictfp;

#include "Dictionary.h"
#include "logging.h"

#include "foundation.h"
#include "evl_loc.h"

bool v_and_v = false;// true when doing v&v on dictionary strings
 
#if 0 // stevev 11feb10 - disallowed on check-in but left here for reference
// PAW start 02/06/09 move from within the class as new and malloc are tramping all over them ! */
	struct DICT_TABLE_ENTRY{
		unsigned long	 ref;
		char			*name;		// timj 13dec07
		unsigned short	 len;
		char			*str;
		bool			used;		// timj 14jan08
		};

	DICT_TABLE_ENTRY 	dict_table[MAX_DICT_TABLE_SIZE]; /*This array holds the dictionary*/
	
	char *pchDictData;
	int iIndex;

	unsigned long 		num_dict_table_entries;
// end PAW 02/06/09
#endif // 0

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

//timj 17jan08 was static char *high_text = "@XX@Highest Offset";
static char *high_text = (char*)"|en|@XX@Highest Offset";

	struct DICT_ENT{
		unsigned long	section;	/* The entry section number*/
		unsigned long	offset;		/* The entry number within the section */
		unsigned long	value;		/* The value is computed from section and offset*/
		char			*name;		/* The name of the entry*/
		char 			*dict_text;	/* The text for the entry*/
	};

	static int dicterrs;
	static int dictline;
	//MODIFIED: Deepak Initialized to NULL
	static char* dictfile =NULL;

	/*
	 * The token buffer.
	 */
	//MODIFIED: Deepak Initialized to NULL
	static char* tokbase =NULL;
	static char* tokptr=NULL;
	static char* toklim=NULL;

	static DICT_ENT* dict_array=NULL;	/* Array of DICT_ENTs */
	static int dict_limit=0;		/* Size of dict_array */
	static int dict_count=0;		/* Number of entries in dict_array */

char* makeDumpStr(wchar_t* pStr)
{
	char* retStr =  NULL;// will be alloc's via malloc

	string wrkStr, ascStr;
	char  locStr[125] = { 0 };

	wchar_t * pChar = pStr;

	if ( pStr == NULL || *pStr == 0 )
	{
		retStr = (char*)malloc(1);
		*retStr = 0;
		return retStr;
	}// else continue

	int y = wcslen(pStr);
	for ( int j = 0; j < min(12,y); j++)
	{
		sprintf_s(locStr," 0x%02x", pStr[j]);
		wrkStr = wrkStr + locStr;
		ascStr = ascStr + (char)(pStr[j] & 0xff);
		*locStr = 0;
	}//wend

	retStr = (char*)malloc(wrkStr.size() + 1 + 2 + ascStr.size());
	strncpy_s(retStr, wrkStr.size() + 1, wrkStr.c_str(),_TRUNCATE );
	strcat_s(retStr, sizeof(retStr), "  ");
	strcat_s(retStr, sizeof(retStr), ascStr.c_str());
	return retStr;
}
CDictionary::CDictionary(const char *pchLangCode)
{
	int i;
	//MODIFIED: Deepak Added if condition
	if(pchLangCode)
	{
		wstring ws = AStr2TStr( string(pchLangCode) );
#ifdef MODIFIED_BY_SS
		wcscpy_s(languageCode, ws.size() + 1, ws.c_str());
		//std::wcscpy(languageCode,ws.c_str());
	}
	else
	{
		//std::wcscpy(languageCode,DEF__LANG__CTRY);
		wcscpy_s(languageCode, sizeof(DEF__LANG__CTRY) + 1, DEF__LANG__CTRY);
	}
#endif
	for (i=0;i < MAX_DICT_TABLE_SIZE ; i++)
	{
		dict_table[i].str = NULL;
		//MODIFIED by Deepak, Added two member initialization
		dict_table[i].ref =0;
		dict_table[i].len=0;
		dict_table[i].name = NULL;	// J.U. this is checked in the destructor and have to be initialized
		dict_table[i].used = false;	// J.U.
	}
	//MODIFIED by Deepak, Added two member initialization
	pchDictData =NULL;
	iIndex =0;
		
	num_dict_table_entries = 0;	
}

CDictionary::CDictionary(const CDictionary &dict)
{
	for(unsigned i =0;i < dict.num_dict_table_entries; i++)
	{
		dict_table[i].ref = dict.dict_table[i].ref;
		dict_table[i].len = dict.dict_table[i].len;
		// timj 18jan08 - unknown reason
		// dict_table[i].str = new char[(dict.dict_table[i].len+1)];
		// strcpy(dict_table[i].str, dict.dict_table[i].str);
		dict_table[i].str = strdup(dict.dict_table[i].str);		// J.U. was dict_table[i].str = dict.dict_table[i].str; Assignment of char *
		dict_table[i].name = strdup(dict.dict_table[i].name);	// J.U. Initialize the name
		dict_table[i].used = dict.dict_table[i].used;			// J.U. Initialize the used
	}
	num_dict_table_entries = dict.num_dict_table_entries;

	//MODIFIED: Deepak Added if condition
	//MODIFIED: again Carolyn H - 10oct06 - looking in the wrong place
	// was:: if(languageCode)
	if(dict.languageCode)
#ifdef MODIFIED_BY_SS
		//std::wcscpy(languageCode,dict.languageCode);
		wcscpy_s(languageCode, sizeof(dict.languageCode) + 1, dict.languageCode);
	else
		//std::wcscpy(languageCode,DEF__LANG__CTRY);
		wcscpy_s(languageCode, sizeof(languageCode) / sizeof(languageCode[0]), DEF__LANG__CTRY);
#endif
	//MODIFIED by Deepak, Added two member initialization
	pchDictData =NULL;
	iIndex =0;
}


CDictionary ::~CDictionary()
{
	//unsigned long dwStartTime = ddbGetTickCount();
	for(unsigned i = 0; i < num_dict_table_entries; i++)
	{
		if(dict_table[i].str != NULL)
		{
			delete [] dict_table[i].str;
			dict_table[i].str =NULL;
		}
		/* stevev 26apr10 - memory leak removal */		
		if(dict_table[i].name != NULL)
		{
			delete [] dict_table[i].name;
			dict_table[i].name =NULL;
		}
	}
	num_dict_table_entries = 0;
	//unsigned long dwEndTime = ddbGetTickCount();
	//
	//cout << "Dictionary Cleanup time = " << dwEndTime - dwStartTime << endl;

}
// done to stop the assertion where alloc is used to get 'em and delete is used to lose 'em
char* CDictionary::strdup(const char* instr)
{
	if (instr == NULL ) return NULL;
	int y = strlen(instr);

	char* rV = new char[y+1];
	if ( rV != NULL )
		strncpy_s(rV, y + 1,instr,_TRUNCATE);
	return rV;
}

void CDictionary ::dicterr(char * msg)
{
	++dicterrs;
		
	if (dictfile)
	{
		LOGIT(CERR_LOG,L"\"%s\"",dictfile);
	}

	if (dictline)
	{
		LOGIT(CERR_LOG,L" line %d:",dictline);
	}

	//MODIFIED by Deepak added if
	if(msg)
		LOGIT(CERR_LOG,L"%s\n",msg);
	else
		LOGIT(CERR_LOG,L"\n");
}


void CDictionary ::inschar(int c)
{
	int		off=0, size=0;

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

			//ADDED 'if' by Deepak
			if(tokbase)
				delete [] tokbase;

			//ADDED by Deepak
			tokbase =NULL;

			tokbase = new char[size];
			memcpy(tokbase, TempToken,size/2);
		}
		if (tokbase == (char *) 0)
		{
			dicterr((char*)"memory exhausted");
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


int CDictionary ::comment()
{
	int		c;

	/*
	 * Verify we're at the start of a comment. If we don't
	 * find a valid start of comment, return an error.
	 */
	c = pchDictData[iIndex++];
	if (c != '*')
	{
		return TOK_ERROR;
	}

	/*
	 * Consume the comment.
	 */
	c = pchDictData[iIndex++];
	for (;;)
	{
		if (c == EOF)
		{
			return TOK_ERROR;
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
			c = pchDictData[iIndex++];
			if (c == EOF)
			{
				return TOK_ERROR;
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
		c = pchDictData[iIndex++];
	}

	/*
	 * Indicates valid comment consumed.
	 */
	return TOK_DONE;
}


int CDictionary ::lex()
{
	int			c;
	static int	nextchar = -1;

	/*
	 * Get next character.
	 */
	if (nextchar < 0)
	{
		c = pchDictData[iIndex++];
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
				return TOK_DONE;

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
				c = pchDictData[iIndex++];
				break;
	
			/*
			 * Eat a comment.
			 */

			case '/':
				if (comment() != TOK_DONE)
				{
					return TOK_ERROR;
				}
				c = pchDictData[iIndex++];
				break;
	
			case ',':
				return TOK_COMMA;
	
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
					c = pchDictData[iIndex++];
				} while (isdigit(c));
	
				/*
				 * Read a comma.
				 */

				if (c != ',')
				{
					return TOK_ERROR;
				}
	
				/*
				 * Read another digit sequence.
				 */

				do
				{
					inschar(c);
					c = pchDictData[iIndex++];
				} while (isdigit(c));
	
				/*
				 * Verify it ends correctly.
				 */

				if (c != ']')
				{
					return TOK_ERROR;
				}
				inschar(c);
	
				/*
				 * Return it.
				 */

				return TOK_NUMBER;
	
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
					c = pchDictData[iIndex++];
				} while (isalnum(c) || c == '_');
	
				/*
				 * Save the lookahead character.
				 */

				nextchar = c;
	
				/*
				 * Return it.
				 */

				return TOK_NAME;
	
			case '\"':
				/*
				 * Clear the token buffer.
				 */

				tokptr = tokbase;
	
				/*
				 * If string does not start with a country code,
				 * add the default country code (English).
				 */

				c = pchDictData[iIndex++];
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

				for (;; c = pchDictData[iIndex++])
				{
					switch (c)
					{
						/*
						 * Unterminated string.
						 */

						case EOF:
						case '\n':
							return TOK_ERROR;
		
						/*
						 * Anything can be escaped with a backslash,
						 * this will usually be \".
						 */

						case '\\':
							inschar(c);
							c = pchDictData[iIndex++];
							if (c == EOF)
							{
								return TOK_ERROR;
							}
							inschar(c);
							break;
		
						/*
						 * We've come to the end of the string. Read ahead
						 * to see if there is string immediately following
						 * this one, and if there is, read that one also.
						 */

						case '\"':
							c = pchDictData[iIndex++];
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
										c = pchDictData[iIndex++];
										continue;
			
									/*
									 * Eat a comment.
									 */

									case '/':
										if (comment() != TOK_DONE)
										{
											return TOK_ERROR;
										}
										c = pchDictData[iIndex++];
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
							return TOK_TEXT;
		
						/*
						 * Ordinary character, add it to the token buffer.
						 */

						default:
							inschar(c);
							break;
					}// endswitch on string c
				}// next string char
				/*NOTREACHED*/

			/*
			 * Invalid input.
			 */
			default:
				return TOK_ERROR;
		}// endswitch on c
	}// next
}


void CDictionary ::parse()
{
	//Modified by Deepak initialize all vars
	int				tok=0, rc=0;
	char			*last=NULL, *name=NULL;
	unsigned long	section=0, offset=0;
	char			*strbuf=NULL;
	DICT_ENT		*curr_dict=NULL, *end_dict=NULL;

	/*
	 * Initialize.
	 */
	tok = lex();
	curr_dict = dict_array + dict_count;
	end_dict = dict_array + dict_limit;

	while (tok != TOK_DONE)
	{
		/*
		 * Verify the token is a number.
		 */	
		if (tok != TOK_NUMBER)
		{
			break;
			dicterr((char*)"syntax error");
			do
			{ 
				tok = lex();
			} while (tok != TOK_DONE && tok != TOK_NUMBER);
			continue;
		}

		/*
		 * Extract the section number and offset from the token.
		 */

		section = strtoul(tokbase + 1, &last, 10);
		offset = strtoul(last + 1, (char **) 0, 10);
		int ref = section * 65536 + offset;

		/*
		 * Get next token and verify it's a string name.
		 */

		tok = lex();
		if (tok != TOK_NAME)
		{	
			dicterr((char*)"syntax error");
			do
			{
				tok = lex();
			} while (tok != TOK_DONE && tok != TOK_NUMBER);
			continue;
		}

		/*
		 * Save the name.
		 */

		name = (char *) new char[((unsigned)(strlen(tokbase) + 1))];
		
		if (name == (char *) 0)
		{
			dicterr((char*)"memory exhausted");
			exit(-1);
		}
		//ADDED By Deepak 'if'
		if(tokbase)
		strcpy_s(name, sizeof(((unsigned)(strlen(tokbase) + 1))),tokbase);

		/*
		 * Get the next token and verify it's a string.
		 */

		tok = lex();
		if (tok != TOK_TEXT)
		{
			dicterr((char*)"syntax error");
			do
			{
				tok = lex();
			} while (tok != TOK_DONE && tok != TOK_NUMBER);
			continue;
		}

		/*
		 *	Move the string of the desired language to the top of the buffer.
		 *	This is done entirely within the buffer.
		 */
		/*Vibhor 201003: Changed second parameter to langCode from "|en|" */


#ifdef CTRY_FILTER
		//
		// timj 9jan08 - no longer filtering on language on input. all languages
		// are retained.  translation to current language happens in
		// dictionary::get_dictionary_string(index, wstring &)
		//
		rc = get_string_translation(tokbase, tokbase, strlen(tokbase) + 1);
#endif

		/*
		 *	Save the desired language.  Make sure that the string ends with
		 *	an end-of-string character.
		 */

		if (rc != DDS_SUCCESS)
		{
			tokbase[0] = '\0';
		}

		// timj 9jan08 - fm6 strings are latin1, convert to utf8 before storage:
		//				beginning with fm8, dictionary is read from the binary file
		//				therefore, this code is only invoked on the SDC side to parse
		//				dictionaries for fm6 binaries.  We are gauranteed that this
		//				parse() only reads latin1 encoded .dct files.
		int iAllocLength = latin2utf8size(tokbase) + 1;
		strbuf = new char[iAllocLength];
		if (strbuf == (char *) 0)
		{
			dicterr((char*)"memory exhausted");
			exit(-1);
		}
		if(tokbase)
		{
			latin2utf8(tokbase, strbuf, iAllocLength); // timj 18jan08 was::>  strcpy (strbuf, tokbase);
		}
		// end timj

		/*
		 *	Save this entry into dict_array, expanding it if necessary.
		 */

		if (curr_dict == end_dict)
		{
			dict_count = dict_limit;
			dict_limit *= 2;
	//realloc used with new
			dict_array = (DICT_ENT *)realloc ((char *)dict_array,
							(unsigned)(dict_limit * sizeof (*dict_array)));
			if (!dict_array)
			{
				dicterr((char*)"memory exhausted");
				exit(-1);
			}
			curr_dict = dict_array + dict_count;
			end_dict = dict_array + dict_limit;
		}
		curr_dict->section = section;
		curr_dict->offset = offset;
		curr_dict->value = section * 65536 + offset;
		curr_dict->name = name;	// memory leak here?? 07/04/09 PAW
		curr_dict->dict_text = strbuf;
		curr_dict++;

		/*
		 *	Get the next token
		 */

		tok = lex();
	}// wend tok done
	dict_count = curr_dict - dict_array;
}


int CDictionary::compdict(const void *dict_ent1,const void *dict_ent2)
{
	return (int)(((DICT_ENT*)dict_ent1)->value - ((DICT_ENT*)dict_ent2)->value);
}

void CDictionary ::dict_table_install(unsigned long ref,char* name,char*str)
{
	dict_table[num_dict_table_entries].ref = ref;
	dict_table[num_dict_table_entries].len = (unsigned short) strlen(str);
	dict_table[num_dict_table_entries].str = str;
	dict_table[num_dict_table_entries].name = name;
	dict_table[num_dict_table_entries].used = true;	// timj 14jan08
#ifdef DIRECT_DICT_REFERENCE
	nameMap[name] = num_dict_table_entries;
//	if(name)
//	delete[] (name);
//	(name)=NULL;
#endif

	num_dict_table_entries++;
	// stevev 20aug07 - the computer knows it has an issue, it needs to tell about it.
	if ( num_dict_table_entries >= MAX_DICT_TABLE_SIZE )
	{
		LOGIT(CERR_LOG|UI_LOG,"ERROR: Dictionaries too big.\n");
		num_dict_table_entries--;// force a write-over to prevent writing into un-alloc'd memory
	}
}

int CDictionary::dict_compare(const void *ptr_a,const void *ptr_b)
{
	DICT_TABLE_ENTRY *dict_a, *dict_b;

	dict_a = (DICT_TABLE_ENTRY *) ptr_a;
	dict_b = (DICT_TABLE_ENTRY *) ptr_b;

	return ((int) ((long) dict_a->ref - (long) dict_b->ref));
}

static bool isInVector(unsigned int x, vector<unsigned int>& list)
{//aka: return (list.find(x) != list.end());//sjv
	for (unsigned i = 0; i < list.size(); i++)
	{
		if (list[i] == x)
			return true;
	}

	return false;
}

void CDictionary::dumpdict(bool fm6)
{
	unsigned i;
	int used = 0;

	// timj 17jan08  dontprintme is a temporary fix
	// dont print or count the STANDARD dict strings when we are doing
	// V&V
	// they do not exist in the dictionary in fm6 files
	// they are now included in the dictionary in fm8 files
	// the dontprintme list is taken from evl_loc.h

	vector<unsigned int> dontprintme;

	if (v_and_v)
	{
		dontprintme.push_back(DEFAULT_STD_DICT_STRING);
		dontprintme.push_back(DEFAULT_DEV_SPEC_STRING);
		dontprintme.push_back(DEFAULT_STD_DICT_HELP);
		dontprintme.push_back(DEFAULT_STD_DICT_LABEL);
		dontprintme.push_back(DEFAULT_STD_DICT_DESC);
		dontprintme.push_back(DEFAULT_STD_DICT_DISP_INT);
		dontprintme.push_back(DEFAULT_STD_DICT_DISP_UINT);
		dontprintme.push_back(DEFAULT_STD_DICT_DISP_FLOAT);
		dontprintme.push_back(DEFAULT_STD_DICT_DISP_DOUBLE);
		dontprintme.push_back(DEFAULT_STD_DICT_EDIT_INT);
		dontprintme.push_back(DEFAULT_STD_DICT_EDIT_UINT);
		dontprintme.push_back(DEFAULT_STD_DICT_EDIT_FLOAT);
		dontprintme.push_back(DEFAULT_STD_DICT_EDIT_DOUBLE);
	}

	for (i = 0; i < num_dict_table_entries; i++)
		if (!isInVector(dict_table[i].ref, dontprintme) && dict_table[i].used)
			used++;

	cout << "\nDictionary    Count: " << used << "\n";
	cout << "\nFormat     Dict: [ref]  [name]  [len]  [str]\n\n";

	unsigned long n = 0;
	bool sorted = true;		
	for (i = 0; i < num_dict_table_entries; i++)
	{
		// dictionary should be sorted by reference number.
		// n should be monotonically increasing
		if (dict_table[i].ref <= n)
		{
			sorted = false;
			cout << "* ";
		}

		if (!isInVector(dict_table[i].ref, dontprintme) && dict_table[i].used)
		{

			char *p = dict_table[i].str;
			int plen = strlen(p);

			
			wstring w = UTF82Unicode(p);
			p = w2c(w);
			plen = w.size();
			

			cout << "Dict:\t" << dict_table[i].ref << "\t" 
				 << dict_table[i].name << "\t"
				 <<plen << "\t" 
				 << addlinebreaks(p)
				 << endl;

			free(p);
		}
	}

	if (sorted)
		cout << "\n\n" ;
	else
		cout << "Error:  the dictionary is not sorted.\n\n" ;
}

#ifdef _WIN32_WCE
// PAW bsearch function added 06/05/09
void* CDictionary::binary_search(const void *key, const char/*void*/ *base,size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	size_t odd_mask, bytes;
	const char *center, *high, *low;
	int comp;

	odd_mask = ((size ^ (size - 1)) >> 1) + 1;
	low = base;
	bytes = nmemb == 0 ? size : size + 1;
	center = low + nmemb * size;
	comp = 0;
	while (bytes != size) 
	{
		if (comp > 0) 
		{
			low = center;
		}
		else	 
		{
			high = center;
		}
		bytes = high - low;
		center = low + ((bytes & odd_mask ? bytes - size : bytes) >> 1);

		comp = compar(key, center);
		if (comp == 0) 
		{
			return (void *)center;
		}

	}
	return NULL;
}
#endif // _WIN32_WCE

// build dictionary directly from binary
//	copy all strings so that they are not de-allocated!
//	all dict entries from the binary have been used by the DD or an import
//	12dec07 timj

int CDictionary ::makedict(DICT_REF_TBL *dict_ref_tbl)
{
	unsigned i;
	DEBUGLOG(CLOG_LOG,"DICTIONARY::: makedict from table at %p\n",this);

	if (dict_ref_tbl->name==NULL && dict_ref_tbl->text==NULL)
	{
		// timj  14jan08
		// name and text are not present. therefore dictionary just came in
		// from an fm6 binary.  also, dictionary has already been installed
		// from .dct file with name and text.  
		 
		// Each entry with a ref in dict_ref_tbl will be marked as used,
		// others are marked as unused
		
		/* we don't do this because the global dictionary is used for multiple devices
		// first mark all entries as being unused
		for (i = 0; i < num_dict_table_entries; i++)
		{
			dict_table[i].used = false;
		}
		**/

		// then mark ref's in dict_ref_tbl as being used
		for (i = 0; i < dict_ref_tbl->count; i++)
		{
			DICT_TABLE_ENTRY *found_ptr=NULL;
			DICT_TABLE_ENTRY key;
			key.ref = dict_ref_tbl->list[i];	// this ref was used in the DD

// changed bsearch to b_search PAW 09/04/09
//... note stevev 11feb10 - no reason given so I left original for non-CE
			found_ptr = (DICT_TABLE_ENTRY *) B_SEARCH((char *) &key,
				(char *) dict_table, (unsigned) num_dict_table_entries,
				sizeof(DICT_TABLE_ENTRY), dict_compare);

			if (found_ptr)
				found_ptr->used = true;			// so mark dict entry as being used
#ifdef _DEBUG
			else
				LOGIT(CERR_LOG," Dictionary reference was not found in the Dictionary.(%d)\n",
								key.ref);
#endif
		}
	}
	else
	{
		// timj 14jan08
		// dictionary just came in from binary (fm8) or from .dct file (fm6)
		// build the dictionary, marking all entries as used
		for (i = 0; i < dict_ref_tbl->count; i++)
		{
			dict_table_install(dict_ref_tbl->list[i], // number
				strdup(dict_ref_tbl->name[i].str),	  // name
				strdup(dict_ref_tbl->text[i].str)	  // actual dict string
				);
			// stevev - 28sep11 - mark as deletable to try and stop mem leak
			dict_ref_tbl->name[i].flags = FREE_STRING;
			dict_ref_tbl->text[i].flags = FREE_STRING;
		}

	// sort the dictionary entries
	(void)qsort(dict_table, num_dict_table_entries, sizeof(DICT_TABLE_ENTRY), dict_compare);

#ifdef DIRECT_DICT_REFERENCE
	for (i = 0; i < num_dict_table_entries; i++)
	{
		nameMap[name] = i;
	}
#endif
	}
	return 0;	// dicterrs
}
// end timj

int CDictionary ::makedict(char *file,char **addnl_file_array)
{
	//ADDED By Deepak initialized the variables
	int iDictDataSize = 0;
	DICT_ENT	*curr_dict=NULL, *end_dict=NULL, *next_dict=NULL;
	char		*errbuf=NULL;
	bool        isOptional = false;// stevev 28aug07 - make addnl optional
	bool        failure    = false;// stevev 28aug07 - used to skip on optional file failure

//	ASSERT_DBG(addnl_file_array);
	DEBUGLOG(CLOG_LOG,"DICTIONARY::: makedict '%s' at %p\n",file,this);
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
	dict_limit = MAX_DICT_TABLE_SIZE;
	dict_count = 0;
	/* DEEPAK : Since we need to use realloc we shall use malloc and free
	dict_array = (DICT_ENT *)new DICT_ENT[((unsigned)(dict_limit))];
	*/
	dict_array = (DICT_ENT *)malloc(sizeof(DICT_ENT)* dict_limit);

	if (!dict_array) {
		dicterr((char*)"memory exhausted");
		exit(-1);
	}

	dictfile   = file;
	isOptional = false;

	iAddnlFileIndex = 0;

	while (dictfile)
	{	/*
		 * Open the file.
		 */
		//dictfp = fopen(dictfile, "r");
#ifndef _WIN32_WCE	
	ifstream dictfp;
    #ifndef linux
		_finddata_t fileData;
		long tempHandle;
		iDictDataSize = 0;
		if(-1L != (tempHandle = _findfirst(dictfile, &fileData)))
		{
			iDictDataSize = fileData.size;
			_findclose(tempHandle);
		}
		else
		{
			if ( isOptional )
				LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Cannot find optional dictionary '%s'\n",dictfile);
			else
			{
				LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Cannot find required dictionary '%s'\n",dictfile);
				return -1; /*Since SUCCESS is defined as 0 we can't return as false*/
			}
			failure = true;
		}
    #else
		iDictDataSize = theFiles.getFileSize(dictfile);
		if(-1L == iDictDataSize)
		{
			iDictDataSize = 0;// that's what they're expecting
			if ( isOptional )
				LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Cannot find optional dictionary '%s'\n",dictfile);
			else
			{
				LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Cannot find required dictionary '%s'\n",dictfile);
				return -1; /*Since SUCCESS is defined as 0 we can't return as false*/
			}
			failure = true;
		}
    #endif

#else
		ifstream dictfp;
//		LPCTSTR lpFileName;
		WIN32_FIND_DATA FindFileData;

		HANDLE tempHandle;
		iDictDataSize = 0;

		USES_CONVERSION;					// PO for unicode conversion
		CComBSTR str(dictfile);				// PO for unicode conversion
		LPTSTR szTemp = OLE2T(str.m_str);	// PO for unicode conversion

		tempHandle = ::FindFirstFile(szTemp, &FindFileData);
		if(tempHandle!= INVALID_HANDLE_VALUE)
		{
			iDictDataSize = FindFileData.nFileSizeLow;
			FindClose(tempHandle);
		}
		else
		{
			if ( isOptional )
				LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Cannot find optional dictionary '%s'\n",dictfile);
			else
			{
				LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Cannot find required dictionary '%s'\n",dictfile);
				return -1; /*Since SUCCESS is defined as 0 we can't return as false*/
			}
			failure = true;
		}

#endif
		if ( ! failure )
		{
			dictfp.open(dictfile,ios::in );//| ios :: binary);
				
			if (!dictfp.is_open())
			{
				if ( isOptional )
					LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Cannot open optional dictionary '%s'\n",dictfile);
				else
				{
					LOGIT(CERR_LOG|CLOG_LOG,"ERROR: Cannot open required dictionary '%s'\n",dictfile);
					return -1; /*Since SUCCESS is defined as 0 we can't return as false*/
				}
				failure = true;
			}
		}

		if ( ! failure )
		{
			pchDictData = new char[iDictDataSize + 1];

			//ADDED by Deepak
			if(pchDictData)
				memset((char*)pchDictData,0,(iDictDataSize + 1));		

			dictfp.read(pchDictData,iDictDataSize);
			
			/*Terminate the buffer with NULL character*/
			pchDictData[iDictDataSize] = '\0';
			/*
			 * Read it.
			 */
			iIndex = 0;
			parse();		
			/*
			 *	Sort the entries from the files
			 */
			//ADDED by Deepak
			if(dict_array)
				(void)qsort((char *)dict_array, dict_count, sizeof(*dict_array), compdict);

			//(void) fclose(dictfp);

			dictfp.clear();
			
			dictfp.close();
		
			delete [] pchDictData;
			//ADDED By Deepak
			pchDictData=NULL;
		}// endif not failure

		if (dictfile == addnl_file_array[iAddnlFileIndex])
		{
			dictfile = NULL;
		}
		else
		{
			dictfile = addnl_file_array[iAddnlFileIndex++];
		}
		isOptional = true;
		failure    = false;
	}// wend dictfile not null

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
			//ADDED By Deepak 'if'
			if(curr_dict->dict_text)
			{
			if (strcmp (curr_dict->dict_text, high_text))
			{
				errbuf = (char*)new char[((unsigned)(strlen(errfmt[5]) + 20))];
				if (errbuf == (char *) 0)
				{
					dicterr((char*)"memory exhausted");
					exit(-1);
				}
				sprintf_s(errbuf,sizeof(((unsigned)(strlen(errfmt[5]) + 20))), errfmt[5], curr_dict->section);
				dicterr (errbuf);

					//ADDED By Deepak 'if'
					if(errbuf)
						delete [] errbuf;

					//ADDED By Deepak
					errbuf=NULL;
			}
			}//if dict_text

			/*
			 *	This is a "hidden" entry, so don't pass it to the
			 *	user function.
			 */
			if(curr_dict->name)
				delete [] (curr_dict->name);
			curr_dict->name =NULL;

			//delete (curr_dict->name);
			if(curr_dict->dict_text)
				delete [] curr_dict->dict_text;
			curr_dict->dict_text =NULL;
			continue;
		}

		/*
		 *	Each entry must be unique.
		 */

		next_dict = curr_dict + 1;
		if (curr_dict->value == next_dict->value)
		{
			errbuf = (char*)new char[((unsigned)(strlen(errfmt[1]) + 20))];
			if (errbuf == (char *) 0)
			{
				dicterr((char*)"memory exhausted");
				exit(-1);
			}
			(void)sprintf_s(errbuf, sizeof(((unsigned)(strlen(errfmt[1]) + 20))),
				errfmt[1], curr_dict->section,
						curr_dict->offset);
			dicterr (errbuf);
			//ADDED By Deepak 'if'
			if(errbuf)
				delete [] errbuf;
			errbuf =NULL;
			curr_dict++;
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

			if(curr_dict->name)
				delete [] curr_dict->name;
			curr_dict->name =NULL;

			if (curr_dict->section == next_dict->section)
			{
				errbuf = (char*)new char[((unsigned)(strlen(errfmt[4]) + 20))];
				if (errbuf == (char *) 0)
				{
					dicterr((char*)"memory exhausted");
					exit(-1);
				}
				sprintf_s(errbuf, ((unsigned)(strlen(errfmt[4]) + 20)), errfmt[4], curr_dict->section);
				dicterr (errbuf);
				//ADDED By Deepak 'if'
				if(errbuf)
					delete[] errbuf;
				errbuf=NULL;
			}
			else if (next_dict->offset != 0)
			{

				/*
				 *	... the first offset in the next section must
				 *	be zero.
				 */

				errbuf = (char*)new char[((unsigned)(strlen(errfmt[3]) + 20))];
				if (errbuf == (char *) 0)
				{
					dicterr((char*)"memory exhausted");
					exit(-1);
				}
				(void)sprintf_s(errbuf, ((unsigned)(strlen(errfmt[3]) + 20)) ,errfmt[3], next_dict->section);
				dicterr (errbuf);
				//ADDED By Deepak 'if'
				if(errbuf)
					delete[] errbuf;
				errbuf =NULL;
			}

			/*
			 *	This is a "hidden" entry, so don't pass it to the
			 *	user function.
			 */

			if(curr_dict->dict_text)
				delete[] curr_dict->dict_text;
			curr_dict->dict_text =NULL;

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
				errbuf = (char*)new char[((unsigned)(strlen(errfmt[5]) + 20))];
				if (errbuf == (char *) 0)
				{
					dicterr((char*)"memory exhausted");
					exit(-1);
				}
				(void)sprintf_s(errbuf, ((unsigned)(strlen(errfmt[5]) + 20)), errfmt[5], curr_dict->section);
				dicterr (errbuf);
				//ADDED By Deepak 'if'
				if(errbuf)
					delete[] errbuf;
				errbuf =NULL;
			}
			else if (curr_dict->offset != next_dict->offset - 1)
			{
				/*
				 *	The offset of the next entry must be one larger
				 *	than this one.
				 */

				errbuf = (char*)new char[((unsigned)(strlen(errfmt[2]) + 40))];
				if (errbuf == (char *) 0)
				{
					dicterr((char*)"memory exhausted");
					exit(-1);
				}
				(void)sprintf_s(errbuf, ((unsigned)(strlen(errfmt[2]) + 20)), errfmt[2], curr_dict->offset,
							next_dict->offset, curr_dict->section);
				dicterr (errbuf);
				//ADDED By Deepak 'if'
				if(errbuf)
					delete[] errbuf;
				errbuf =NULL;
			}

			/*
			 *	Call the install function.
			 */

			dict_table_install(curr_dict->value, curr_dict->name, curr_dict->dict_text);

#ifndef _WIN32_WCE
			// timj 10jan08 - do not delete name now because we are storing and using it!
			// stevev 11feb10 - tim had this commented out, I'll leave it that way for now
#else
			if(curr_dict->name) // PAW was commented out 07/04/09
				delete[] (curr_dict->name);
#endif
			(curr_dict->name)=NULL;
			
		}
	}
		
	/*
	 * Tidy up.
	 */
	if (tokbase)
	{
		delete [] tokbase;
		tokbase = NULL;
		tokptr = NULL;
		toklim = NULL;
	}
	//COMMENT ??? why typecated to char*
	//delete ((char *)dict_array);
	free(dict_array);

	//ADDED By Deepak
	dict_array=NULL;

	/*
	 * Return number of errors seen.
	 */
	return dicterrs;
}

#ifdef DIRECT_DICT_REFERENCE
int CDictionary ::get_dictionary_string(string        index, string& str)
{
	DICT_TABLE_ENTRY *found_ptr=NULL;
	nameLookup_t::iterator fndIt;

	fndIt = nameMap.find(index);
	if (fndIt == nameMap.end() )
	{
#ifdef _PARSER_DEBUG
		
		cout<<"\n get_dictionary_string: Dictionary string, index '"<<index<<"' not found\n";
			
#endif /*_PARSER_DEBUG*/

		return DDL_DICT_STRING_NOT_FOUND;
	}
	else
	{
		str = dict_table[fndIt->second].str;
		return DDL_SUCCESS;
	}
}
#endif
int CDictionary ::get_dictionary_string(unsigned long index , ddpSTRING *str)
{
	//ADDED By Deepak , initializing all vars
	DICT_TABLE_ENTRY *found_ptr=NULL;
	DICT_TABLE_ENTRY key;

	key.ref = index;

	/*
	 * Perform a binary search on the standard dictionary table to find the
	 * entry we're looking for.
	 */

	found_ptr = (DICT_TABLE_ENTRY *) B_SEARCH((char *) &key,
		(char *) dict_table, (unsigned) num_dict_table_entries,
		sizeof(DICT_TABLE_ENTRY), dict_compare);

	if (found_ptr == NULL) {


#ifdef _PARSER_DEBUG
		
		cout<<"\n get_dictionary_string: Dictionary string, index "<<index<<"not found\n";
			
#endif /*_PARSER_DEBUG*/

		return DDL_DICT_STRING_NOT_FOUND;
	}
	else {

		/*
		 * Retrieve the information
		 */
#ifdef DICT_REF_DEBUG
		if(dict_table[0x498].str == found_ptr->str)
		{
			clog<<"got target string"<<endl;
		}
#endif
		str->flags = DONT_FREE_STRING;
		str->len = found_ptr->len;
		str->str = found_ptr->str;
		return DDL_SUCCESS;
	}
}


int CDictionary::get_dictionary_string(unsigned long index,  wstring& str)
{
	//ADDED By Deepak , initializing all vars
	DICT_TABLE_ENTRY *found_ptr=NULL;
	DICT_TABLE_ENTRY key;

	key.ref = index;

	/*
	 * Perform a binary search on the standard dictionary table to find the
	 * entry we're looking for.
	 */

	found_ptr = (DICT_TABLE_ENTRY *) B_SEARCH((char *) &key,
		(char *) dict_table, (unsigned) num_dict_table_entries,
		sizeof(DICT_TABLE_ENTRY), dict_compare);

	if (found_ptr == NULL) {


#ifdef _PARSER_DEBUG
		
		cout<<"\n get_dictionary_string:Dictionary string, index "<<index<<"not found\n";
			
#endif /*_PARSER_DEBUG*/

		return DDL_DICT_STRING_NOT_FOUND;
	}
	else {

		/*
		 * Retrieve the information
		 */

		// timj 08jan08
		str = UTF82Unicode(found_ptr->str);
		return DDS_SUCCESS;
	}

}

// temporary - call wide string version till this func is removed,
int CDictionary ::get_string_translation(char *string,char *toutbuf,int outbuf_size)
{

//LOGIT(CLOG_LOG,"CDictionary ::get_string_translation AAA\n");


	// allocate temporary space for wide strings and copy input
	int n = (int)strlen(string);
	wchar_t *inbuf = new wchar_t[n+1];
	memset(inbuf, 0, sizeof(inbuf) );

	int i=0;
	for (i=0; i < n; i++)
	{
		inbuf[i] = string[i];
	}

	inbuf[i] = 0;

	wchar_t *outbuf = new wchar_t[outbuf_size+ 3];
	memset(outbuf, 0, sizeof(outbuf) );

	int result = get_string_translation(inbuf, outbuf, outbuf_size);

	//wcstombs(toutbuf, outbuf, outbuf_size );
	std::string multi_byte_str = wideCharToMultiByte(outbuf);
	//wcstombs( var_name, wide_var_name, MAX_DD_STRING );
	// Copy the multibyte string to the char array
	std::copy(multi_byte_str.begin(), multi_byte_str.end(), toutbuf);

	// free temp bufs
	delete [] inbuf;
	delete [] outbuf;

	return result;
}

// timj 08jan08
int CDictionary ::get_string_translation(wstring &instr, wstring &outstr)
{

//LOGIT(CLOG_LOG,"CDictionary ::get_string_translation BBB\n");


	int n = (int)instr.size();
	wchar_t *in = (wchar_t *)instr.c_str();
	wchar_t *out = new wchar_t[n+1];
	memset( out, 0, sizeof(wchar_t)*(n+1) );

	int result = get_string_translation(in, out, n+1);

	if (result == DDL_SUCCESS)
	{
		wstring temp(out);
		outstr = temp;
	}

	// Free allocated memory
	delete[] out;

	return result;

}


// timj 07jan08 - modified to work with wide chars
int CDictionary ::get_string_translation(wchar_t *instring, wchar_t *outbuf, int outbuf_size)
{
//LOGIT(CLOG_LOG,"DDParser:CDictionary ::get_string_translation\n");

//LOGIT(CLOG_LOG,"CDictionary ::get_string_translation CCC\n");


	//ADDED By Deepak , initializing all vars
	wchar_t 	*ci=NULL;			/* input character pointer */
	wchar_t 	*co=NULL;			/* output character pointer */
	wchar_t 	*first_phrp=NULL;		/* first phrase pointer */
	wchar_t 	*lang_cntry_phrp=NULL;	/* language + country phrase pointer */
	wchar_t 	*lang_only_phrp=NULL;	/* language-only phrase pointer */
	wchar_t 	lang_only[10];//[5];	/* language-only pulled from language/country code */
	wchar_t 	*new_def_phrp=NULL;	/* new-style default phrase pointer */
	wchar_t 	*old_def_phrp=NULL;	/* old-style language-only phrase pointer */
	wchar_t 	*out_phrp=NULL;		/* output phrase pointer */
	int		code_length=0;	/* length of language/country code, in characters */
	int     n = 0;          /* working value ofstring length */
	
	wchar_t  lang_cntry[10];
	// stevev 2sept08 - outbuf is never guaranteed to hold the entire in buf string!


	/*
	 *	If the input string is a null string (which is legal), we can
	 *	skip all the string processing and return the output string,
	 *	which we set to a null string.
	 */
	if (instring == NULL || instring[0] == 0)
	{
		outbuf[0] = 0;
		return(DDS_SUCCESS);
	}

	int inl = wcslen(instring);

	//LOGIT(CERR_LOG,"testing instring %d long\n", inl);
	wchar_t* pBuf = new wchar_t[inl + 3];
	wchar_t* ptrPBuf = &pBuf[0];
	memset(pBuf, '\0', inl + 3);
if (pBuf == NULL)
{
	LOGIT(CERR_LOG,"Out of memory in dictionary\n");
	return -1;
}

	pBuf[0] = pBuf[1] = 0;

	//ADDED  By Deepak
	if(languageCode)
		//wcscpy(lang_cntry,languageCode);
		(void)wcsncpy_s(lang_cntry, languageCode, (size_t)10);
	else
		wcscpy_s(lang_cntry,DEF__LANG__CTRY);

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
	 *	If the input country_code is full-size (i.e., seven characters),
	 *	extract the language code from the language/country code.
	 *	Otherwise, make the language-only code a null string.
	 */
	if (std::wcslen(lang_cntry) == 7) 
	{
		(void)wcsncpy_s(lang_only, lang_cntry, (size_t)3);
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
	if (instring[0] != COUNTRY_CODE_MARK)
	{
		// stevev - this default string but points to an empty return buf :first_phrp = outbuf;
		first_phrp = instring; // stevev - changed 29nov11 from above
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

	for (co = pBuf, ci = instring; *ci; ci++)
	{
		n = wcslen(ci);
		//LOGIT(CLOG_LOG, "value of n = %d\n", n);
	/*
	 *	Look for the complete language/country code.
	 */
		if (  n>=7 &&   (ci[0] == COUNTRY_CODE_MARK) && iswalpha(ci[1]) &&
				iswalpha(ci[2]) && (ci[3] == _T(' ')) && iswalpha(ci[4])
				&& iswalpha(ci[5]) && (ci[6] == COUNTRY_CODE_MARK)) 
		{
			code_length = 7;

			if ((lang_cntry_phrp == 0) &&
					(wcsncmp(ci, lang_cntry, code_length) == 0)) 
			{
				lang_cntry_phrp = co + 1;
			}

			if ((new_def_phrp == 0) &&
					(wcsncmp(ci, DEF__LANG__CTRY, code_length) == 0)) 
			{
				new_def_phrp = co + 1;
			}

			if (first_phrp == 0)
			{
				first_phrp = co + 1;
			}

			*co++ = _T('\0');
			ci += (code_length - 1);
		} 

		/*
	 	*	Look for the language-only code (new style).
	 	*/
		else if ( n>=4 && (ci[0] == COUNTRY_CODE_MARK) && iswalpha(ci[1]) &&
				iswalpha(ci[2]) && (ci[3] == COUNTRY_CODE_MARK)) 
		{
			code_length = 4;

			if ((lang_cntry_phrp == 0) &&
					(wcsncmp(ci, lang_cntry, code_length) == 0)) {
				lang_cntry_phrp = co + 1;
			}

			if ((lang_only_phrp == 0) && (lang_only[0] != _T('\0')) &&
					(wcsncmp(ci, lang_only, code_length) == 0)) {
				lang_only_phrp = co + 1;
			}

			if ((new_def_phrp == 0) &&
					(wcsncmp(ci, DEF__LANG__CTRY, code_length) == 0)) {
				new_def_phrp = co + 1;
			}

			if (first_phrp == 0)
			{
				first_phrp = co + 1;
			}

			*co++ = _T('\0');
			ci += (code_length - 1);

	/*
	 *	Look for the language-only code (old style); default only.
	 */
		} else if ( n >= 4 && (ci[0] == COUNTRY_CODE_MARK) && iswdigit(ci[1]) &&
				iswdigit(ci[2]) && iswdigit(ci[3])) 
		{
			code_length = 4;

			if ((old_def_phrp == 0) &&
					(wcsncmp(ci, L"|001", code_length) == 0)) 
			{
				old_def_phrp = co + 1;
			}

			if (first_phrp == 0)
			{
				first_phrp = co + 1;
			}

			*co++ = _T('\0');
			ci += (code_length - 1);

	/*
	 *	If the escape sequence character (\) is encountered, convert
	 *	the following character as required.  These are the escape
	 *	sequences required by the DDL Spec.
	 */

		} else if (*ci == _T('\\')) 
		{

//LOGIT(CLOG_LOG,"** escape sequence character");

			switch (*(ci + 1)) 
			{

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
					*co++ = *(ci + 1);
					ci++;
					break;
			}

	/*
	 *	This is the 'normal' case; this character has no special
	 *	significance, so just copy it to the output pointer.
	 */
		} 
		else 
		{
			*co++ = *ci;
		}
	}// next wchar_t

	/*
	 *	Tack an end-of-string character onto the final phrase.
	 */
	*co++ = _T('\0');

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
/*
LOGIT(CLOG_LOG,"** lang_cntry_phrp = %d",lang_cntry_phrp);
LOGIT(CLOG_LOG,"** lang_only_phrp = %d",lang_only_phrp);
LOGIT(CLOG_LOG,"** new_def_phrp = %d",new_def_phrp);
LOGIT(CLOG_LOG,"** old_def_phrp = %d",old_def_phrp);
LOGIT(CLOG_LOG,"** first_phrp = %d",first_phrp);
*/
/*
if (lang_cntry_phrp) 
{
LOGIT(CLOG_LOG,"** lang_cntry_phrp = %s",lang_cntry_phrp);
LOGIT(CLOG_LOG,"** lang_cntry_phrp[0] = %d",lang_cntry_phrp[0]);
LOGIT(CLOG_LOG,"** lang_cntry_phrp[1] = %d",lang_cntry_phrp[1]);
LOGIT(CLOG_LOG,"** lang_cntry_phrp[2] = %d",lang_cntry_phrp[2]);
LOGIT(CLOG_LOG,"** lang_cntry_phrp[3] = %d",lang_cntry_phrp[3]);
LOGIT(CLOG_LOG,"** lang_cntry_phrp[4] = %d",lang_cntry_phrp[4]);
LOGIT(CLOG_LOG,"** lang_cntry_phrp[5] = %d",lang_cntry_phrp[5]);
LOGIT(CLOG_LOG,"** lang_cntry_phrp[6] = %d",lang_cntry_phrp[6]);
}
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
if (out_phrp) 
{
LOGIT(CLOG_LOG,"** out_phrp = %s",out_phrp);
LOGIT(CLOG_LOG,"** out_phrp[0] = %d",out_phrp[0]);
LOGIT(CLOG_LOG,"** out_phrp[1] = %d",out_phrp[1]);
LOGIT(CLOG_LOG,"** out_phrp[2] = %d",out_phrp[2]);
LOGIT(CLOG_LOG,"** out_phrp[3] = %d",out_phrp[3]);
LOGIT(CLOG_LOG,"** out_phrp[4] = %d",out_phrp[4]);
LOGIT(CLOG_LOG,"** out_phrp[5] = %d",out_phrp[5]);
LOGIT(CLOG_LOG,"** out_phrp[6] = %d",out_phrp[6]);
}
*/
	/*
	 *	Check the length of the output buffer.  If the phrase to be output
	 *	is longer than the output buffer, return an error code.  Otherwise,
	 *	copy the phrase in the holding buffer into the output buffer.  
	 */
	if ((size_t) outbuf_size < wcslen(out_phrp)) 
	{// do the best we can
		wcsncpy_s(outbuf, outbuf_size - 1, out_phrp,_TRUNCATE );
		outbuf[outbuf_size-1] = _T('\0');
		delete[] pBuf;
		pBuf = NULL;
LOGIT(CLOG_LOG,"   Dict. insufficient buffer.\n");
		return DDI_INSUFFICIENT_BUFFER;
	} 
	else 
	{
#ifdef MODIFIED_BY_SS
		//n = wcslen(out_phrp);
		//(void)wcscpy(outbuf, out_phrp);
		wcscpy_s(outbuf, outbuf_size, out_phrp);


		delete[] pBuf;
		pBuf = NULL;
#endif
	}
	// this needs to be relooked... currently commented out-- srilatha
	//delete[] pBuf;

/*
LOGIT(CLOG_LOG,"** outbuf = %s",outbuf);
LOGIT(CLOG_LOG,"** outbuf[0] = %d",outbuf[0]);
LOGIT(CLOG_LOG,"** outbuf[1] = %d",outbuf[1]);
LOGIT(CLOG_LOG,"** outbuf[2] = %d",outbuf[2]);
LOGIT(CLOG_LOG,"** outbuf[3] = %d",outbuf[3]);
LOGIT(CLOG_LOG,"** outbuf[4] = %d",outbuf[4]);
LOGIT(CLOG_LOG,"** outbuf[5] = %d",outbuf[5]);
LOGIT(CLOG_LOG,"** outbuf[6] = %d",outbuf[6]);
LOGIT(CLOG_LOG,"** outbuf[7] = %d",outbuf[7]);
*/
	return DDS_SUCCESS;
}

#ifdef DICT_REF_DEBUG
// this is for pre-UTF8 debugging
bool CDictionary::dict_ok()// false on problem
{
	bool ret = true;// default- all OK
	for (unsigned i = 0; i < num_dict_table_entries; i++)
	{
		if ( dict_table[i].str && dict_table[i].str[0] & 0x80)
		{
			dict_table[i].str[0] = 0x7e;//~
			ret = false;
			break;// out of for loop
		}
	}
	return ret;
}
bool CDictionary::isDictPtr(char* t)
{
	bool ret = false;// default- all OK
	for (unsigned i = 0; i < num_dict_table_entries; i++)
	{
		if ( dict_table[i].str == t )
		{
			ret = true;
			break;// out of for loop
		}
	}
	return ret;
}
#endif  // DICT_REF_DEBUG
