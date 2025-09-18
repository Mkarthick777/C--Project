#if !defined(INCLUDE_0BA3845D_FA49_4E6E_B020_994D41858D98)
#define INCLUDE_0BA3845D_FA49_4E6E_B020_994D41858D98
#pragma warning (disable : 4786)//Anil August 26 2005
#include "ParserDeclarations.h"
#include "Program.h"
#include "SymbolTable.h"
#include "ExpParser.h"
#include "InterpretedVisitor.h"
#include "TypeCheckVisitor.h"
//#include "Hart_Builtins.h"

#include <vector>
using namespace std;

#ifdef STANDALONE_PARSER

class CBuiltInLib;

#else

class CHart_Builtins;

#endif

class MEE; //Vibhor 010705: Added

typedef vector<char> BYTE_CODE_ARRAY;

enum INTERPRETER_STATUS
{
	INTERPRETER_STATUS_INVALID
	, INTERPRETER_STATUS_OK
	, INTERPRETER_STATUS_PARSE_ERROR
	, INTERPRETER_STATUS_EXECUTION_ERROR
	, INTERPRETER_STATUS_UNKNOWN_ERROR
};



class CParser  
{
private:
#ifdef STANDALONE_PARSER

CBuiltInLib  *pBuiltInLib;

#else

CHart_Builtins *pBuiltInLib;

#endif
bool m_bIsRoutine;//Anil Octobet 5 2005 for handling Method Calling Method

public:
	CParser();
	virtual ~CParser();

#ifdef STANDALONE_PARSER
	bool Initialize(CBuiltInLib *pBuiltInLib);
#else
	bool Initialize(CHart_Builtins *pBuiltInLib,MEE *pMEE); //Vibhor 010705: Modified
#endif

	_INT32 BuildParseTree(
		_UCHAR* pszSource,
		_UCHAR*	pszRuleName,
		_CHAR* szData,
		_CHAR* szSymbolTable);

	_INT32 Execute(
		_UCHAR* pszSource,
		_UCHAR*	pszRuleName,
		_CHAR* szData,
		_CHAR* szSymbolTable);

	_INT32 Compile(
		_UCHAR* pszSource,
		_UCHAR*	pszRuleName,
		_UCHAR** ppszByteCode,
		_INT32* pi32ByteCodeSize,
		_INT32	i32Console);

	bool GetVariableValue
				(
					char *pchVariableName
					, INTER_VARIANT &varValue
				);

	bool SetVariableValue
				(
					char *pchVariableName
					, INTER_VARIANT &varValue
				);

	//Anil Octobet 5 2005 for handling Method Calling Method
	//Added an overloaded function to handle the methods calling methods
	_INT32 Execute(
		_UCHAR* pszSource,
		_UCHAR*	pszRuleName,
		_CHAR* szData,
		_CHAR* szSymbolTable,
		METHOD_ARG_INFO_VECTOR* vectMethArg,
		vector<INTER_VARIANT>* vectInterVar);


	void SetIsRoutineFlag(bool bisRoutine);
	bool GetIsRoutineFlag();

protected:
	CLexicalAnalyzer	lexAnal;
	CSymbolTable		SymbolTable;
	CProgram			pgm;
	CInterpretedVisitor interpretor;
	CTypeCheckVisitor	typeChecker;
	MEE					*m_pMEE; //Vibhor 010705: Added
};

#endif