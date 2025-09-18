#include "pch.h"
#include "ddbdevice.h"
#include "Interpreter.h"
#include "Hart_Builtins.h"
#include "Delay_Builtin.h"
#include "MethodInterfaceDefs.h"
#include "MEE.h"
#include "DDBitemBase.h"
#include "ddbdefs.h"
#include <cmath>

//jad vs2017
//#include <math.h>

//jad vs2017
#ifdef ANDROID
#include <cstdlib>
#endif

/* Arun 120505 Start of code*/
/*Math support builtins (EDDL)*/

double CHart_Builtins:: abs (double x)
{
	// // stevev changed 18oct07 from::> return ::abs(x);
	return ::fabs(x);
}

double CHart_Builtins:: acos (double x)
{
	return ::acos(x);
}

double CHart_Builtins:: asin (double x)
{
	return ::asin(x);
}

double CHart_Builtins:: atan (double x)
{
	return ::atan(x);
}

double CHart_Builtins:: cbrt (double x)
{
	return (x >= 0)?::pow(x, 1.0/3.0):-::pow(-x, 1.0/3.0);
}

double CHart_Builtins:: ceil (double x)
{
	return ::ceil(x);
}

double CHart_Builtins:: cos (double x)
{
	return ::cos(x);
}

double CHart_Builtins:: cosh (double x)
{
	return ::cosh(x);
}

double CHart_Builtins:: exp (double x)
{
	return ::exp(x);
}

double CHart_Builtins:: floor (double x)
{
	return ::floor(x);
}

double CHart_Builtins:: fmod (double x, double y)
{
	return ::fmod(x,y);
}
#ifdef XMTR
double CHart_Builtins:: frand ()
{ 
	return ( ::rand()/((double)RAND_MAX) );
}
#endif
double CHart_Builtins:: log (double x)
{
	return ::log(x);
}

double CHart_Builtins:: log10 (double x)
{
	return ::log10(x);
}

double CHart_Builtins:: log2 (double x)
{
// HOMZ - port to 2003, VS7 >> error C2668: 'log' : ambiguous call to overloaded function
	return (::log(x) / ::log((double)2));
//was 	return (::log(x) / ::log(2));
}

double CHart_Builtins:: pow (double x, double y)
{
	return ::pow(x,y);
}

double CHart_Builtins:: round (double x)
{
	return int(x+0.5F);
}

double CHart_Builtins:: sin (double x)
{
	return ::sin(x);
}

double CHart_Builtins:: sinh (double x)
{
	return ::sinh(x);
}

double CHart_Builtins:: sqrt (double x)
{
	return ::sqrt(x);
}

double CHart_Builtins:: tan (double x)
{
	return ::tan(x);
}

double CHart_Builtins:: tanh (double x)
{
	return ::tanh(x);
}

double CHart_Builtins:: trunc (double x)
{
	return int(x);
}

double CHart_Builtins:: atof (char* string)
{
	char* p = string;
	if ( *p++ == '|')
	{
		while(p < (string + strlen(string)) )
		{
			if ( *p++ == '|' )
			{
				string = p;
				break;
			}
		}
	}
	return ::atof(string);
}

int CHart_Builtins:: atoi (char* string)
{
	char* p = string;
	if ( *p++ == '|')
	{
		while(p < (string + strlen(string)) )
		{
			if ( *p++ == '|' )
			{
				string = p;
				break;
			}
		}
	}
	return ::atoi(string);
}
#ifdef MODIFIED_BY_SS
//wchar_t* CHart_Builtins:: itoa (int value, wchar_t* string,int radix)
//{
//	return ::int2wide(value, string ); // radix has to be 10..., radix);
//                             // otherwise we get to write a itow function
//}

wchar_t* CHart_Builtins::itoa(int value, wchar_t* string, int radix)
{
	// Convert integer value to wide-character string
	std::wstring temp = std::to_wstring(value);

	// Copy the contents of the temporary string to the provided buffer
	if (string != nullptr)
	{
		size_t length = temp.length();
		//wcsncpy(string, temp.c_str(), length);
		std::copy(temp.begin(), temp.end(), string);
		string[length] = L'\0';  // Null-terminate the string
	}

	return string;
}
#endif
/* End of code*/
