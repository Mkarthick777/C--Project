// LitStringTable.cpp: implementation of the LitStringTable class.
//
//////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "foundation.h"
#include "LitStringTable.h"
#include "Dictionary.h"

//jad vs2017
#include <stdlib.h>
extern CDictionary *pGlobalDict; /*The Global Dictionary object*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LitStringTable::LitStringTable()
{
}

LitStringTable::~LitStringTable()
{
	// stevev - 28sep11 - this stuff has to go away sometime
	unsigned long I;
	char* actualStr = NULL;
	map<unsigned long, char *, ulong_lt>::iterator pos;
	for (pos = table.begin(); pos != table.end(); pos++)
	{
		I = pos->first;
		actualStr = table[I];
		delete [] actualStr;
		table[I] = NULL;
	}
}

void LitStringTable::install(unsigned long index, char *s)
{
	table.insert( make_pair(index, s) );
}

wstring LitStringTable::get_lit_string(unsigned long index)
{
	if ( table.find(index) != table.end() )
		return UTF82Unicode(table[index]);
	else
		return wstring(_T(""));// otherwise it would make a record.
}


char* LitStringTable::get_lit_char(unsigned long index)
{
	if ( table.find(index) != table.end() )
		return table[index];
	else
		return (char*)"";// otherwise it would make a record.
}
void LitStringTable::dump()
{
	cout << "\nLiteral String Table    Count: " << table.size() << "\n";
	cout << "\nFormat     Lit: [index]  [str]\n\n";

	map<unsigned long, char *, ulong_lt>::iterator pos;
	for (pos = table.begin(); pos != table.end(); pos++)
	{
		wstring litstr = get_lit_string(pos->first);

		cout   << "Lit: \t" << pos->first << "\t" << addlinebreaks(w2c(litstr))
			   << endl;
	}

	cout << "\n\n" ;
}


// build lit string table from binary
//	copy all strings so that they are not de-allocated!
//	all lit string entries from the binary have been used by the DD or an import
// 24apr13 - update to cleaner code that removes a fms memory leak
int LitStringTable::makelit(STRING_TBL *string_tbl, bool isLatin1)
{
	ddpSTRING			*string = NULL;	 	/* temp pointer for the list */
	ddpSTRING			*end_string =NULL;	/* end pointer for the list */
	char *lit ;

	unsigned long index = 0;
	for (string = string_tbl->list, end_string = string + string_tbl->count;
		 string < end_string; string++) 
	{// a list of ddpSTRING
		if (isLatin1)
		{
			int iAllocLength = latin2utf8size(string->str) + 1;
			lit = new char[iAllocLength];
			if (lit == (char *) 0)
			{
				LOGIT(CERR_LOG,L"Memory exhausted.\n");
				exit(-1);
			}
			
			latin2utf8(string->str, lit, iAllocLength);
			delete[] string->str;
			string->str = NULL;// no memory - get rid of this leak 22apr13
		}
		else
		{
			lit = string->str;
			string->str = NULL;// stevev 28sep11 - take ownership of the memory
		}

		install(index++, lit);// lit memory is carried into the table
	}
	
	return 0;	// dicterrs
}
