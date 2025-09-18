#ifndef INTERPRETER_H
#define INTERPRETER_H

#pragma warning (disable : 4786)//Anil August 26 2005
#include "parser.h"
#include "DDBitems.h" // stevev added 10aug07

typedef enum methErrors_e
{
	me_NoError,
	me_Aborted
}
/*typedef*/ methErrors_t;

class MEE; //Vibhor 010705: Added

class CInterpreter
{
private:
	/* Flag to indicate if source code is assigned or not */
	bool	m_bInitialized;

	/* Status of the interpreter */
	INTERPRETER_STATUS	m_intStatus;

	/* Pointer to the source as passed by the Client */
	char	*m_pchSourceCode;

	/* The pointer to the parser object */
	CParser  *m_pParser;

	MEE		 *m_pMEE; //Vibhor 010705 Added : Required for access to "Global" (DD) Data

public:
	CInterpreter();
	~CInterpreter();

	INTERPRETER_STATUS ExecuteCode
							(
#ifdef STANDALONE_PARSER
								CBuiltInLib  *pBuiltInLibParam
#else
								CHart_Builtins *pBuiltInLibParam
#endif
								, char *pchSource
								, char *pchSourceName
								, char *pchCodeData
								, char *pchSymbolDump
								, MEE  *pMEE			//Vibhor 010705: Added
							);

	bool GetVariableValue
				(
					char *pchVariableName
					, INTER_VARIANT &varValue
					, hCVar**     ppDevObjVar = NULL
				);

	bool SetVariableValue
				(
					char *pchVariableName
					, INTER_VARIANT &varValue
				);
//Anil Octobet 5 2005 for handling Method Calling Method
	INTERPRETER_STATUS ExecuteCode
							(
#ifdef STANDALONE_PARSER
								CBuiltInLib  *pBuiltInLibParam
#else
								CHart_Builtins *pBuiltInLibParam
#endif
								, char *pchSource
								, char *pchSourceName
								, char *pchCodeData
								, char *pchSymbolDump
								, MEE  *pMEE
								,METHOD_ARG_INFO_VECTOR* vectMethArg
								,vector<INTER_VARIANT>* vectInterVar
								);

				
};

#endif /*INTERPRETER_H*/
