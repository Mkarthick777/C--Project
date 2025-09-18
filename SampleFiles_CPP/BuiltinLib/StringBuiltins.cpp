//Start Of file
//Added By Anil June 17 2005 --starts here
#include "pch.h"
#include "ddbdevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"
#include "MEE.h"
#include "DDBitemBase.h"
#include "ddbdefs.h"
#include "string.h"
#include <cmath>

#include <ctype.h>  //4  toupper/lower

/****************************Start of DD_STRING  Builtins  (eDDL) ********************/

wchar_t* CHart_Builtins::STRSTR(wchar_t*  string_var,wchar_t* substring_to_find)
{
	wchar_t* szRetVal = NULL;
	if( string_var && substring_to_find )
	{
		szRetVal = wcsstr( string_var, substring_to_find );
	}
	return szRetVal;
}



wchar_t* CHart_Builtins::STRUPR(wchar_t* string_var)
{
	wchar_t* szRetVal = NULL;
	/* stevev 10mar14 modified to be portable
	if( string_var )
	{
		szRetVal = _wcsupr(string_var);
	}
	return szRetVal;
	***/
	while (string_var)// apparently only works for ascii
	{
	    *string_var = toupper((unsigned char)*string_var);
	    string_var++;
	}
    return string_var;
}


wchar_t* CHart_Builtins::STRLWR(wchar_t* string_var)
{	
	wchar_t* szRetVal = NULL;
	/* stevev 10mar14 modified to be portable	
	if( string_var )
	{
		szRetVal = _wcslwr(string_var);
	}
	return szRetVal;
	***/
	while (string_var)// apparently only works for ascii
	{
	    *string_var = tolower((unsigned char)*string_var);
	    string_var++;
	}
    return string_var;
}


int CHart_Builtins::STRLEN(wchar_t* string_var)
{
	int nRetVal = 0;
	if( string_var )
	{
		nRetVal = wcslen(string_var);
	}
	return nRetVal;
}

int CHart_Builtins::STRCMP(wchar_t* string_var1, wchar_t* string_var2)
{
	int nRetVal = -1;
	if( string_var1 == NULL && string_var2 == NULL )//beware of being passed NULL pointers
	{
		nRetVal = 0;//if they are both NULL (empty) then say they match.
	}
	else if( string_var1 == NULL || string_var2 == NULL )//beware of being passed NULL pointers
	{
		nRetVal = -1;
	}
	else
	{
		nRetVal = wcscmp(string_var1,string_var2);
    }
	return nRetVal;
}


wchar_t* CHart_Builtins::STRTRIM(wchar_t* string_var)
{
	if( string_var ) //check for NULL pointer
	{
		int istrLen = wcslen(string_var);
		int iNoOfspace = 0;
		int icount;// PAW see below 03/03/09
		for(/*int*/ icount = 0; icount < (istrLen - iNoOfspace); icount++)
		{
			if( (string_var[icount] == _T(' ')) || (string_var[icount] == _T('\t')) || (string_var[icount] == _T('\r'))|| (string_var[icount] == _T('\n')) )
			{				
				for(int iRef= icount ;  iRef < (istrLen - iNoOfspace); iRef++)
				{
					string_var[iRef] = string_var[iRef+1];	
				}
				iNoOfspace++;
				icount--;
			}
			else
			{
					break;
			}
		}
		istrLen = wcslen(string_var);
		for(icount = istrLen; icount > 0; icount--)
		{
			if( (string_var[icount-1] == _T(' ')) || (string_var[icount-1] == _T('\t')) || (string_var[icount-1] == _T('\r'))|| (string_var[icount-1] == _T('\n')) )
			{
				string_var[icount-1]=_T('\0');
			}
			else
			{
					break;
			}
		}
	}
	return string_var;

}

//Get a substring from a string specifying starting position and length.
wchar_t* CHart_Builtins::STRMID(wchar_t* string_var,int start, int len)
{
	wchar_t* szReturnValue = NULL;
	if( string_var )
	{
		int nSourceStringLength = wcslen(string_var);

		if(  (start < nSourceStringLength) && (len <= nSourceStringLength) && 
		    ((start +len) <= nSourceStringLength))  	
		{
			szReturnValue = new wchar_t[nSourceStringLength+1];//this is NOT a memory leak, it will be freed by the calling function.
			memset(szReturnValue,0,sizeof(wchar_t)*nSourceStringLength+1);

			szReturnValue[0]=0;
			for(int nIndex=0; nIndex<len; nIndex++)
	{
				szReturnValue[nIndex] = string_var[start+nIndex];
				szReturnValue[nIndex+1] = 0;
			}
		}
	}
	return szReturnValue;
}


//Added By Anil June 17 2005 --Ends here
/****************************End of DD_STRING  Builtins (eDDL) ********************/

//End Of File