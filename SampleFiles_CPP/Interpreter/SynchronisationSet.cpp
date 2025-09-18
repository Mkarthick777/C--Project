
//#include "stdafx.h"
#include "pch.h"
#include "SynchronisationSet.h"
#include "Token.h"

FOLLOW_SET g_follow_set;

FOLLOW_ELEMENT::FOLLOW_ELEMENT(	RUL_TOKEN_TYPE	rhsType,
							   RUL_TOKEN_SUBTYPE	rhsSubType)
{
	Type = rhsType;
	SubType = rhsSubType;
}

FOLLOW_SET::FOLLOW_SET()
{
	follows[EXPRESSION].production = EXPRESSION;
	follows[EXPRESSION].set.push_back(FOLLOW_ELEMENT(RUL_SYMBOL,RUL_SEMICOLON));
	follows[EXPRESSION].set.push_back(FOLLOW_ELEMENT(RUL_SYMBOL,RUL_RPAREN));
	follows[EXPRESSION].set.push_back(FOLLOW_ELEMENT(RUL_SYMBOL,RUL_LBRACK));
	follows[EXPRESSION].set.push_back(FOLLOW_ELEMENT(RUL_SYMBOL,RUL_RBRACK));

	follows[DECLARATION].production = DECLARATION;
	follows[DECLARATION].set.push_back(FOLLOW_ELEMENT(RUL_SYMBOL,RUL_COMMA));
	follows[DECLARATION].set.push_back(FOLLOW_ELEMENT(RUL_SYMBOL,RUL_SEMICOLON));
}

bool FOLLOW_SET::IsPresent(
		PRODUCTION production,
		RUL_TOKEN_TYPE Type,
		RUL_TOKEN_SUBTYPE SubType)
{
	for(_INT32 i=0; i<PRODUCTION_COUNT;i++)
	{
		if(follows[i].production == production)
		{
			_INT32 i32SetSize = follows[i].set.size();
			for(_INT32 j=0;	j<i32SetSize;	j++)
			{
				if((follows[i].set[j].Type == Type)
					&& (follows[i].set[j].SubType == SubType))
				return true;
			}
		}
	}
	return false;
}
