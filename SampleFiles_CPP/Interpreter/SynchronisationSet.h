



#ifndef _SYNC_SET_H_
#define _SYNC_SET_H_




//#include "stdafx.h"
#include "ParserDeclarations.h"
//#pragma warning(disable:4251)

enum PRODUCTION
{
	EXPRESSION=0,
	DECLARATION=1,
	ASSIGNMENT=2,
	SELECTION=3,
	ITERATION=4,
	COMPOUND_STMT=5,
	STMT_LIST=6
};
#define PRODUCTION_COUNT	7


struct FOLLOW_ELEMENT
{
	FOLLOW_ELEMENT(	RUL_TOKEN_TYPE	rhsType,
							   RUL_TOKEN_SUBTYPE	rhsSubType);
	RUL_TOKEN_TYPE		Type;
	RUL_TOKEN_SUBTYPE	SubType;
};

typedef vector<FOLLOW_ELEMENT>	FOLLOW_VECTOR;

struct FOLLOWS
{
	PRODUCTION			production;
	FOLLOW_VECTOR	set;
};

class CToken;
class FOLLOW_SET
{
public:
	FOLLOW_SET();
	bool	IsPresent(
		PRODUCTION production,
		RUL_TOKEN_TYPE Type,
		RUL_TOKEN_SUBTYPE SubType);

protected:
	FOLLOWS		follows[PRODUCTION_COUNT];
};

extern FOLLOW_SET g_follow_set;

#endif //_SYNC_SET_H_