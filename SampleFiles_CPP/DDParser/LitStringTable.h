// LitStringTable.h: interface for the LitStringTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(LITSTRINGTABLE_H__INCLUDED_)
#define LITSTRINGTABLE_H__INCLUDED_

#include "DDlConditional.h"// includes--"Attributes.h"
#include "table.h"

#include <string>
#include <map>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class LitStringTable  
{
	//char languageCode[10]; 	/* Of the form "|LL|" or "|LL CC|" */

	struct ulong_lt
	{
	  bool operator()(unsigned long u1, unsigned long u2) const
	  {
		return u1 < u2;
	  }
	};

	map<unsigned long, char *, ulong_lt> table;

public:
	LitStringTable();
	virtual ~LitStringTable();

	
	void install(unsigned long index, char *s); 

	wstring get_lit_string(unsigned long index);
	char*   get_lit_char(unsigned long index);

	void dump();

	int makelit(STRING_TBL *string_tbl, bool isLatin1);

};

#endif // !defined(LITSTRINGTABLE_H__INCLUDED_)



