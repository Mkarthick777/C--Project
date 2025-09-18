//#include "stdafx.h"
//#include <windows.h>
#include "pch.h"
#include "ddbGeneral.h"
//#include <ALPHAOPS.H>
#include <limits.h>
#include <float.h>
#include "ddbdevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"

bool CHart_Builtins::GetLongArray
				(
					INTER_VARIANT &varValue
					, long *plongArray
					, int  &iArraySize
				)
{
	INTER_SAFEARRAY *sa=NULL;
	if (varValue.GetVarType() == RUL_SAFEARRAY)
	{
		sa = varValue.GetSafeArray();

		int idims = sa->GetDims(NULL);

		if (idims == 1)
		{
			INTER_VARIANT varValue;
			for (int i = 0,index = 0;index < sa->GetNumberOfElements();i+=sa->GetElementSize(),index++)
			{
				sa->GetElement(i, &varValue);
				if (varValue.GetVarType() != RUL_INT)
				{
					iArraySize = 0;
					return false;
				}				
				plongArray[index] = (int)varValue;
			}
			iArraySize = sa->GetNumberOfElements();
			return true;
		}
		else
		{
			iArraySize = 0;
			return false;
		}
	}
	else
	{
		iArraySize = 0;
		return false;
	}
}

bool CHart_Builtins::GetCharArray
				(
					INTER_VARIANT &varValue
					, char *pchArray
					, int  &iArraySize /* stevev 30may07: passed in MAX_LEN, returned actual Len*/
				)
{
	INTER_SAFEARRAY *sa=NULL;
	if (varValue.GetVarType() == RUL_SAFEARRAY && iArraySize > 0 )
	{
		sa = varValue.GetSafeArray();

		int idims = sa->GetDims(NULL);

		if (idims == 1)
		{
			int saLen = sa->GetNumberOfElements();
			if ( saLen > iArraySize )
			{
				saLen = iArraySize;
			}
			INTER_VARIANT varValue;
			for (int i = 0,index = 0; index < saLen;  i+=sa->GetElementSize(),index++)
			{
				sa->GetElement(i, &varValue);
				if (varValue.GetVarType() == RUL_CHAR)
				{
					varValue.GetValue((void *)&pchArray[index],RUL_CHAR);
				}
				else if (varValue.GetVarType() == RUL_UNSIGNED_CHAR)
				{
					varValue.GetValue((void *)&pchArray[index],RUL_UNSIGNED_CHAR);
				}
				else
				{
					iArraySize = 0;
					return false;
				}				
			}

			iArraySize = saLen;
			pchArray[iArraySize] = 0;
			return true;
		}		
	}
	iArraySize = 0;
	return false;	
}

bool CHart_Builtins::SetCharArray
				(
					INTER_VARIANT &varValue
					, char *pchArray
					, int  aSize  /* aSize added to deal with more_data_info[] where zero is not a terminator*/
				)
{
	INTER_SAFEARRAY *sa=NULL;
	if (varValue.GetVarType() == RUL_SAFEARRAY)
	{
		sa = varValue.GetSafeArray();

		int idims = sa->GetDims(NULL);
		int saLen = sa->GetNumberOfElements();
		int chLen = 0;
		if (aSize)
			chLen = min(aSize,MAX_DD_STRING);
		else
		    chLen = strlen(pchArray) +1;// WS:EPM 04jun07 - take null terminator into account
										// handles bug where second string has remnants of the first in it.
		if (idims == 1)
		{
			saLen = min (saLen,chLen);
			INTER_VARIANT variantTemp;
			int iArrayIndex = 0;
			for (int i = 0,index = 0;index < saLen;/*sa->GetNumberOfElements();*/i+=sa->GetElementSize(),index++)
			{
				variantTemp = (char) pchArray[iArrayIndex];
				sa->SetElement(i, &variantTemp);
				iArrayIndex++;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool CHart_Builtins::GetWCharArray
				(
					INTER_VARIANT &varValue
					, tchar *pchArray
					, int  &iArraySize /* stevev 30may07: passed in MAX_LEN, returned actual Len*/
				)
{
	INTER_SAFEARRAY *sa=NULL;
	if (varValue.GetVarType() == RUL_SAFEARRAY && iArraySize > 0 )
	{
		sa = varValue.GetSafeArray();

		int idims = sa->GetDims(NULL);

		if (idims == 1)
		{
			int saLen = sa->GetNumberOfElements();
			if ( saLen > iArraySize )
			{
				saLen = iArraySize;
			}
			INTER_VARIANT varValue;
			uchar  U = 0;
			char   C = 0;
			for (int i = 0,index = 0; index < saLen;  i+=sa->GetElementSize(),index++)
			{
				sa->GetElement(i, &varValue);
				if (varValue.GetVarType() == RUL_CHAR)
				{
					varValue.GetValue(&C,RUL_CHAR);
					pchArray[index]  = C; // narrow to wide conversion
				}
				else 
				if (varValue.GetVarType() == RUL_SHORT)
				{
					varValue.GetValue((void *)&pchArray[index],RUL_SHORT);
				}
				else 
				if (varValue.GetVarType() == RUL_UNSIGNED_CHAR)
				{
					varValue.GetValue(&U,RUL_UNSIGNED_CHAR);
					pchArray[index] = U; // narrow to wide conversion
				}
				else
				{
					iArraySize = 0;
					return false;
				}				
			}

			iArraySize = saLen;
			pchArray[iArraySize] = 0;
			return true;
		}// else silent error???		
	}// else silent error???
	iArraySize = 0;
	return false;	
}

bool CHart_Builtins::SetWCharArray
				(
					INTER_VARIANT &varValue
					, tchar *pchArray
					, int  aSize  /* aSize added to deal with more_data_info[] where zero is not a terminator*/
				)
{
	INTER_SAFEARRAY *sa=NULL;
	if (varValue.GetVarType() == RUL_SAFEARRAY)
	{
		sa = varValue.GetSafeArray();

		int idims = sa->GetDims(NULL);
		int saLen = sa->GetNumberOfElements();
		int chLen = 0;
		if (aSize)
			chLen = min(aSize,MAX_DD_STRING);
		else
		    chLen = _tstrlen(pchArray) +1;// WS:EPM 04jun07 - take null terminator into account
									// handles bug where second string has remnants of the first in it.
		if (idims == 1)
		{
			saLen = min (saLen,chLen);
			INTER_VARIANT variantTemp;
			int iArrayIndex = 0;
			for (int i = 0,index = 0;index < saLen; i+=sa->GetElementSize(),index++)
			{
				variantTemp = pchArray[iArrayIndex];
				sa->SetElement(i, &variantTemp);
				iArrayIndex++;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}