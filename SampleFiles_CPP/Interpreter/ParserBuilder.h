
#if !defined(AFX_PARSERBUILDER_H__0E7B5DED_1503_4C07_92CA_13E1564F27AB__INCLUDED_)
#define AFX_PARSERBUILDER_H__0E7B5DED_1503_4C07_92CA_13E1564F27AB__INCLUDED_


#include "ParserDeclarations.h"


class CGrammarNode;
class CToken;
class CLexicalAnalyzer;
enum STATEMENT_TYPE;

class CParserBuilder  
{
public:
	CParserBuilder();
	virtual ~CParserBuilder();

	CGrammarNode*	CreateParser(
		CLexicalAnalyzer* plexAnal,
		STATEMENT_TYPE stmt_type);
};

#endif