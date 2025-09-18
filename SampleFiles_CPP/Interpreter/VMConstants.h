//#include "stdafx.h"
#ifndef INCLUDE_2BD5F546_B1D2_476b_9E80_E3C1836A9526
#define INCLUDE_2BD5F546_B1D2_476b_9E80_E3C1836A9526

/*
*The following are java Byte Codes
*/

#define _BYTE_CODE_ICONSTM1			(_UCHAR)0x02
#define _BYTE_CODE_ICONST0			(_UCHAR)0x03
#define _BYTE_CODE_ICONST1			(_UCHAR)0x04
#define _BYTE_CODE_BIPUSH			(_UCHAR)0x10
#define _BYTE_CODE_ILOAD			(_UCHAR)0x15
#define _BYTE_CODE_ILOAD0			(_UCHAR)0x1A
#define _BYTE_CODE_ISTORE			(_UCHAR)0x36
#define _BYTE_CODE_ISTORE0			(_UCHAR)0x3B
#define _BYTE_CODE_IADD				(_UCHAR)0x60
#define _BYTE_CODE_ISUB				(_UCHAR)0x64
#define _BYTE_CODE_IMUL				(_UCHAR)0x68
#define _BYTE_CODE_IDIV				(_UCHAR)0x6C
#define _BYTE_CODE_IREM				(_UCHAR)0x70

#define _BYTE_CODE_FADD				(_UCHAR)0x62
#define _BYTE_CODE_FSUB				(_UCHAR)0x66
#define _BYTE_CODE_FMUL				(_UCHAR)0x6A
#define _BYTE_CODE_FDIV				(_UCHAR)0x6E
#define _BYTE_CODE_FREM				(_UCHAR)0x72

#define _BYTE_CODE_FSTORE0			(_UCHAR)0x43
#define _BYTE_CODE_FSTORE1			(_UCHAR)0x44
#define _BYTE_CODE_FSTORE2			(_UCHAR)0x45
#define _BYTE_CODE_FSTORE3			(_UCHAR)0x46
#define _BYTE_CODE_FSTORE			(_UCHAR)0x38

#define _BYTE_CODE_FLOAD0			(_UCHAR)0x22
#define _BYTE_CODE_FLOAD1			(_UCHAR)0x23
#define _BYTE_CODE_FLOAD2			(_UCHAR)0x24
#define _BYTE_CODE_FLOAD3			(_UCHAR)0x25
#define _BYTE_CODE_FLOAD			(_UCHAR)0x17

#define _BYTE_CODE_I2F				(_UCHAR)0x86
#define _BYTE_CODE_F2I				(_UCHAR)0x8B

#define _BYTE_CODE_IALOAD			(_UCHAR)0x2E
#define _BYTE_CODE_LALOAD			(_UCHAR)0x2F
#define _BYTE_CODE_FALOAD			(_UCHAR)0x30
#define _BYTE_CODE_DALOAD			(_UCHAR)0x31
#define _BYTE_CODE_AALOAD			(_UCHAR)0x32
#define _BYTE_CODE_BALOAD			(_UCHAR)0x33
#define _BYTE_CODE_CALOAD			(_UCHAR)0x34
#define _BYTE_CODE_SALOAD			(_UCHAR)0x35

#define _BYTE_CODE_IASTORE			(_UCHAR)0x4F
#define _BYTE_CODE_LASTORE			(_UCHAR)0x50
#define _BYTE_CODE_FASTORE			(_UCHAR)0x51
#define _BYTE_CODE_DASTORE			(_UCHAR)0x52
#define _BYTE_CODE_AASTORE			(_UCHAR)0x53
#define _BYTE_CODE_BASTORE			(_UCHAR)0x54
#define _BYTE_CODE_CASTORE			(_UCHAR)0x55
#define _BYTE_CODE_SASTORE			(_UCHAR)0x56

#define _BYTE_CODE_ALOAD			(_UCHAR)0x19
#define _BYTE_CODE_ALOAD0			(_UCHAR)0x2A
#define _BYTE_CODE_ASTORE			(_UCHAR)0x3A
#define _BYTE_CODE_ASTORE0			(_UCHAR)0x4B

#define _BYTE_CODE_IFEQ				(_UCHAR)0x99
#define _BYTE_CODE_IFNE				(_UCHAR)0x9A
#define _BYTE_CODE_IFLT				(_UCHAR)0x9B
#define _BYTE_CODE_IFGE				(_UCHAR)0x9C
#define _BYTE_CODE_IFGT				(_UCHAR)0x9D
#define _BYTE_CODE_IFLE				(_UCHAR)0x9E
#define _BYTE_CODE_GOTO				(_UCHAR)0xA7
#define _BYTE_CODE_IFICMPEQ			(_UCHAR)0x9F
#define _BYTE_CODE_IFICMPNE			(_UCHAR)0xA0
#define _BYTE_CODE_IFICMPLT			(_UCHAR)0xA1
#define _BYTE_CODE_IFICMPGE			(_UCHAR)0xA2
#define _BYTE_CODE_IFICMPGT			(_UCHAR)0xA3
#define _BYTE_CODE_IFICMPLE			(_UCHAR)0xA4

#define _BYTE_CODE_FCMPG			(_UCHAR)0x95
#define _BYTE_CODE_FCMPL			(_UCHAR)0x96

#define _BYTE_CODE_MULTANEWARRAY	(_UCHAR)0xC5
#define _BYTE_CODE_LDC				(_UCHAR)0x12

/*
* The following byte codes are our extensions to java bytye codes
*/

//Floating point--extend the java byte codes
#define _BYTE_CODE_FPUSH			(_UCHAR)0xD0

//String byte codes
#define _BYTE_CODE_SADD				(_UCHAR)0xD1
#define _BYTE_CODE_SSTORE0			(_UCHAR)0xD2
#define _BYTE_CODE_SSTORE1			(_UCHAR)0xD3
#define _BYTE_CODE_SSTORE2			(_UCHAR)0xD4
#define _BYTE_CODE_SSTORE3			(_UCHAR)0xD5
#define _BYTE_CODE_SSTORE			(_UCHAR)0xD6

#define _BYTE_CODE_SLOAD0			(_UCHAR)0xD7
#define _BYTE_CODE_SLOAD1			(_UCHAR)0xD8
#define _BYTE_CODE_SLOAD2			(_UCHAR)0xD9
#define _BYTE_CODE_SLOAD3			(_UCHAR)0xDA
#define _BYTE_CODE_SLOAD			(_UCHAR)0xDB

#define _BYTE_CODE_STRING_CREATE	(_UCHAR)0xDC
#define _BYTE_CODE_IFSCMPNE			(_UCHAR)0xE2

//Rule self invocation byte codes
#define _BYTE_CODE_RUL_SELF			(_UCHAR)0xDD

//OM invocation byte codes
#define _BYTE_CODE_LDOS				(_UCHAR)0xDE
#define _BYTE_CODE_LDOI				(_UCHAR)0xDF
#define _BYTE_CODE_LDOF				(_UCHAR)0xE0
#define _BYTE_CODE_LDOB				(_UCHAR)0xE1

#define _BYTE_CODE_SOS				(_UCHAR)0xE3
#define _BYTE_CODE_SOI				(_UCHAR)0xE4
#define _BYTE_CODE_SOF				(_UCHAR)0xE5
#define _BYTE_CODE_SOB				(_UCHAR)0xE6

//Integer Push
#define _BYTE_CODE_IPUSH			(_UCHAR)0xE7

//double variable --extend the java byte codes
#define _BYTE_CODE_DPUSH			(_UCHAR)0xE8


/*
* The following are general vm related #defines
*/

//Initial part of class file
//Magic Number
#define _MAGIC_1					(_UCHAR)0x00
#define _MAGIC_2					(_UCHAR)0xAB
#define _MAGIC_3					(_UCHAR)0xCD
#define _MAGIC_4					(_UCHAR)0xEF
									
//VM Version Number					
#define _VER_MAJ_1					(_UCHAR)0x00
#define _VER_MAJ_2					(_UCHAR)0x00
#define _VER_MIN_1					(_UCHAR)0x00
#define _VER_MIN_2					(_UCHAR)0x01
									
//Security Info						
#define _SECURITY_1					(_UCHAR)0x00
#define _SECURITY_2					(_UCHAR)0x00
#define _SECURITY_3					(_UCHAR)0x00
#define _SECURITY_4					(_UCHAR)0x00
									
//Stack Size						
#define _STCK_1						(_UCHAR)0x00
#define _STCK_2						(_UCHAR)0x02

//Byte Extraction Macros
#define _MSB_INT16(x)	((_UCHAR)(x>>8))
#define _LSB_INT16(x)	((_UCHAR)(x&0x00FF))

#define _MSB_0INT32(x)	(_UCHAR)((x & 0xFF000000)>>24)
#define _MSB_1INT32(x)	(_UCHAR)((x & 0x00FF0000)>>16)
#define _MSB_2INT32(x)	(_UCHAR)((x & 0x0000FF00)>>8)
#define _MSB_3INT32(x)	(_UCHAR)((x & 0x000000FF))

#define _MSB_0INT64(x)	(_UCHAR)((x & 0x00000000FF000000)>>24)
#define _MSB_1INT64(x)	(_UCHAR)((x & 0x0000000000FF0000)>>16)
#define _MSB_2INT64(x)	(_UCHAR)((x & 0x000000000000FF00)>>8)
#define _MSB_3INT64(x)	(_UCHAR)((x & 0x00000000000000FF))

//General constants
//Null
#define _NULL_BYTE		(_UCHAR)0x00
//number fo bytes used to hold code length in class file
#define _CODE_LEN_SIZE	4
#define _MIN_CHAR	-128
#define _MAX_CHAR	127
//jvm has a byte code specifically for this
#define _MINUS_ONE	-1

/* #define BYTE	255   this is not a BYTE...*/
#define MAX_BYTE  0xFF  /* WS.EPM - 21may07 */
#define KB		0x1000


//byte codes for array type...
//Array Type  atype  
//T_BOOLEAN		4  
//T_CHAR		5  
//T_FLOAT		6  
//T_DOUBLE		7  
//T_BYTE		8  
//T_SHORT		9  
//T_INT			10  
//T_LONG		11  

enum VM_TYPES
{
	T_BOOLEAN=		4 ,
	T_CHAR	=		5 ,
	T_FLOAT	=		6 ,
	T_DOUBLE=		7 ,
	T_BYTE	=		8 ,
	T_SHORT	=		9 ,
	T_INT	=		10,
	T_LONG	=		11
};

extern _CHAR _BYTECODE_ARR_TYPE[];



#define CONSTANT_Utf8					(_UCHAR)0x01  
#define CONSTANT_Integer				(_UCHAR)0x03
#define CONSTANT_Float					(_UCHAR)0x04
#define CONSTANT_Long					(_UCHAR)0x05 
#define CONSTANT_Double					(_UCHAR)0x06 
#define CONSTANT_Class					(_UCHAR)0x07 
#define CONSTANT_String					(_UCHAR)0x08
#define CONSTANT_Fieldref				(_UCHAR)0x09
#define CONSTANT_Methodref				(_UCHAR)0x0A 
#define CONSTANT_InterfaceMethodref		(_UCHAR)0x0B  
#define CONSTANT_NameAndType			(_UCHAR)0x0C 


#define CONSTANT_OM_MASK				(_UCHAR)0x80

#define CONSTANT_OM_STRING				(_UCHAR)0x8D
#define CONSTANT_OM_INT					(_UCHAR)0x8E
#define CONSTANT_OM_BYTE				(_UCHAR)0x8F
#define CONSTANT_OM_FLOAT				(_UCHAR)0x90

#endif
