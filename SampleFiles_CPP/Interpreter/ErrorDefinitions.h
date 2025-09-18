#if !defined(INCLUDE_256D73A7_64E0_40a0_829D_D3354EF44DCC)
#define INCLUDE_256D73A7_64E0_40a0_829D_D3354EF44DCC

#define UNUSED_LOCAL(x)					\
	x;						

#define THROW_ERROR(ErrNo)				\
		throw new CRIDEError(			\
			plexAnal->GetRuleName(),	\
			ErrNo,						\
			plexAnal->GetLineNumber()	\
			);

#define THROW_ERROR1(ErrNo,Node)				\
		throw new CRIDEError(			\
			m_pszRuleName,				\
			ErrNo,						\
			Node->GetLineNumber()		\
			);

#define THROW_LEXICAL_ERROR(ErrNo)		\
		throw new CRIDEError(				\
			(_CHAR*)m_szRuleName,		\
			ErrNo,						\
			m_i32LineNo);					
		
#define BEGIN_RETHROW_ERROR			\
	catch(CRIDEError& err)			\
	{								\
		UNUSED_LOCAL(err);		


#define END_RETHROW_ERROR			\
		throw;						\
	}

#define BEGIN_RETHROW_ALL			\
	catch(...)						\
	{								

#define END_RETHROW_ALL				\
		THROW_LEXICAL_ERROR(COMPILE_ERROR_UNKNOWNERROR);	\
	}

#define ADD_ERROR(ErrNo)						\
	{											\
		CRIDEError* perr = new CRIDEError(					\
			plexAnal->GetRuleName(),			\
			ErrNo,								\
			plexAnal->GetLineNumber()			\
			);									\
		pvecErrors->push_back(perr);			\
	}

#define ADD_ERROR1(ErrNo,Node)					\
	{											\
		CRIDEError* perr = new CRIDEError(		\
			m_pszRuleName,						\
			ErrNo,								\
			Node->GetLineNumber()				\
			);									\
		pvecErrors->push_back(perr);			\
	}

//Parser Errors...
/*
*	Totally 4 bytes for errors
*	MSB 1 byte for module within the Parser.
*	LSB 3 bytes for errors within the module
*/

#define LEX_SUCCESS			1
#define LEX_FAIL			0

#define PARSE_SUCCESS		1
#define PARSE_FAIL			0

#define TOKEN_SUCCESS		1
#define TOKEN_FAILURE		0

#define TYPE_SUCCESS		1
#define TYPE_FAILURE		0

#define RET_SUCCESS(X)		((X) == TOKEN_SUCCESS)


#define		UNKNOWN_MODULE			0x00
#define		LEXICAL_ANALYZER		0x01
#define		ASSIGNMENT_PARSER		0x02

// Reserve 0xFxxxxxxx for Runtime errors
//Unknown Module
#define	C_UM_ERROR_UNKNOWNERROR		0x00000001
#define	C_UM_ERROR_LOWMEMORY		0x00000002
#define	C_UM_ERROR_INTERNALERR		0x00000003

//CLexicalAnalyzer
#define	C_LEX_ERROR_IDLONG			0x01000001
#define	C_LEX_ERROR_ILLEGALCHAR		0x01000002
#define	C_LEX_ERROR_ILLEGALITEM		0x01000003

//CAssignmentParser
#define	C_AP_ERROR_LVALUE			0x02000001
#define	C_AP_ERROR_MISSINGEQ		0x02000002
#define C_AP_ERROR_MISSINGEXP		0x02000003
#define	C_AP_ERROR_MISSINGSC		0x02000004

//CExpParser
#define	C_EP_ERROR_ILLEGALOP		0x03000001
#define	C_EP_ERROR_MISSINGOP		0x03000002
#define	C_EP_ERROR_MISSINGPAREN		0x03000003
#define	C_EP_ERROR_MISSINGSC		0x03000004
#define	C_EP_ERROR_UNKNOWN			0x03000005
#define	C_EP_ERROR_LEXERROR			0x03000006 /* Walt EPM 08sep08 */

//CParser
#define	C_CP_ERROR_HASERRORS		0x04000001

//CSelectionStatement
#define C_IF_ERROR_MISSINGLP		0x05000001
#define C_IF_ERROR_MISSINGEXP		0x05000002
#define C_IF_ERROR_MISSINGRP		0x05000003
#define C_IF_ERROR_MISSINGSTMT		0x05000004

//CIterationStatement
#define C_WHILE_ERROR_MISSINGLP		0x06000001
#define C_WHILE_ERROR_MISSINGEXP	0x06000002
#define C_WHILE_ERROR_MISSINGRP		0x06000003
#define C_WHILE_ERROR_MISSINGSTMT	0x06000004

//CCompoundStatement
#define C_CS_ERROR_MISSINGRBRACK	0x07000001

//CELSEStatement
#define C_ES_ERROR_MISSINGSTMT		0x08000001

//CRuleServiceStatement
#define C_RS_ERROR_MISSINGSCOPE		0x09000001
#define C_RS_ERROR_MISSINGINVOKE	0x09000002
#define C_RS_ERROR_MISSINGLPAREN	0x09000003
#define C_RS_ERROR_MISSINGRNAME		0x09000004
#define C_RS_ERROR_MISSINGRPAREN	0x09000005
#define C_RS_ERROR_MISSINGSC		0x09000006

//CDeclarations
#define C_DECL_ERROR_EXPRESSION		0x0A000001
#define C_DECL_ERROR_LBOX			0x0A000002
#define C_DECL_ERROR_RBOX			0x0A000003
#define C_DECL_ERROR_NUM			0x0A000004
#define C_DECL_ERROR_IDMISSING		0x0A000005
#define C_DECL_ERROR_COMMAMISSING	0x0A000006
#define C_DECL_ERROR_UNKNOWN		0x0A000007

//CTypeCheckVisitor
#define C_TC_ERROR_DIM_MISMATCH		0x0B000000
#define C_TC_ERROR_TYP_MISMATCH		0x0B000001
#define C_TC_ERROR_TYP_NOTIMPL		0x0B000002

#endif