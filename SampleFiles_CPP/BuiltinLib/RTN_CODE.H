

/**
 *		Copyright 1995 - HART Communication Foundation
 *		All rights reserved.
 */



/*
 *	@(#)rtn_def.h	40.1  40  07 Jul 1998
 *
 *	rtn_def.h is used to generate rtn_code.h and as an include
 *	file to dds_err.c
 */



/********************************************************************
 * rtn_code.h is a generated from rtn_def.h!!
 *     Edit rtn_def.h to make changes!!!
 ********************************************************************/
#  ifndef RTN_CODE_H
#  define RTN_CODE_H

//#  include <std>

#  define RETURN_CODE int

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

extern char *dds_error_string (int, char *, int);

#  ifdef __cplusplus
}
#  endif /* __cplusplus */






















/* DDS error codes */
#  define  	SUCCESS 	 						(0)  
#  define  	FAILURE 	 						(1)  
#  define  	DDS_SUCCESS 	 					(0)  
#  define  	DDS_OUT_OF_MEMORY 	 				(1400  + 2)  
#  define  	VARIABLE_VALUE_NEEDED 	 			(1400  + 3)  
#  define  	DDS_WRONG_DDOD_REVISION 	 		(1400  + 4)  
#  define  	DDS_WRONG_TOK_TYPE 	 				(1400  + 5)  

#  define  	DDS_ERROR_END 	 					(1400  + 99)  

/* EVAL error codes */
#  define  	DDL_SUCCESS 	 					(0)  
#  define  	DDL_MEMORY_ERROR 	 				(1500  + 1)  
#  define  	DDL_INSUFFICIENT_OCTETS 	 		(1500  + 2)  
#  define  	DDL_SHORT_BUFFER 	 				(1500  + 3)  
#  define  	DDL_ENCODING_ERROR 	 				(1500  + 4)  
#  define  	DDL_LARGE_VALUE 	 				(1500  + 5)  
#  define  	DDL_DIVIDE_BY_ZERO 	 				(1500  + 6)  
#  define  	DDL_BAD_VALUE_TYPE 	 				(1500  + 7)  
#  define  	DDL_SERVICE_ERROR 	 				(1500  + 8)  
#  define  	DDL_FILE_ERROR 	 					(1500  + 9)  
#  define  	DDL_BAD_FILE_TYPE 	 				(1500  + 10)  
#  define  	DDL_FILE_NOT_FOUND 	 				(1500  + 11)  
#  define  	DDL_OUT_OF_DATA 	 				(1500  + 12)  
#  define  	DDL_DEFAULT_ATTR 	 				(1500  + 13)  
#  define  	DDL_NULL_POINTER 	 				(1500  + 14)  
#  define  	DDL_INCORRECT_FILE_FORMAT 	 		(1500  + 15)  
#  define  	DDL_INVALID_PARAM 	 				(1500  + 16)  
#  define  	DDL_CHECK_RETURN_LIST 	 			(1500  + 17)  
#  define  	DDL_DEV_SPEC_STRING_NOT_FOUND 	 		(1500  + 18)  
#  define  	DDL_DICT_STRING_NOT_FOUND 	 		(1500  + 19)  
#  define  	DDL_READ_VAR_VALUE_FAILED 	 		(1500  + 20)  
#  define  	DDL_BINARY_REQUIRED 	 			(1500  + 21)  
#  define  	DDL_VAR_TYPE_NEEDED 	 			(1500  + 22)  
#  define  	DDL_EXPR_STACK_OVERFLOW 	 		(1500  + 23)  
#  define  	DDL_INVALID_TYPE_SUBATTR 	 		(1500  + 24)  
#  define  	DDL_BAD_FETCH_TYPE 	 				(1500  + 25)  
#  define  	DDL_RESOLVE_FETCH_FAIL 	 			(1500  + 26)  
#  define  	DDL_BLOCK_TABLES_NOT_LOADED 	 	(1500  + 27)  

#  define  	DDL_ERROR_END 	 					(1500  + 99)  

/* FETCH error codes */
#  define  	FETCH_INVALID_DEVICE_HANDLE 	 	(1600  + 0)  
#  define  	FETCH_DEVICE_NOT_FOUND 	 			(1600  + 1)  
#  define  	FETCH_INVALID_DEV_TYPE_HANDLE 	 	(1600  + 2)  
#  define  	FETCH_DEV_TYPE_NOT_FOUND 	 		(1600  + 3)  
#  define  	FETCH_INVALID_DD_HANDLE_TYPE 	 	(1600  + 4)  
#  define  	FETCH_TABLES_NOT_FOUND 	 			(1600  + 5)  
#  define  	FETCH_ITEM_NOT_FOUND 	 			(1600  + 6)  
#  define  	FETCH_DIRECTORY_NOT_FOUND 	 		(1600  + 7)  
#  define  	FETCH_INSUFFICIENT_SCRATCHPAD 	 	(1600  + 10)  
#  define  	FETCH_NULL_POINTER 	 				(1600  + 11)  
#  define  	FETCH_ITEM_TYPE_MISMATCH 	 		(1600  + 12)  
#  define  	FETCH_INVALID_ATTRIBUTE 	 		(1600  + 13)  
#  define  	FETCH_INVALID_RI 	 				(1600  + 14)  
#  define  	FETCH_INVALID_ITEM_TYPE 	 		(1600  + 15)  
#  define  	FETCH_EMPTY_ITEM_MASK 	 			(1600  + 16)  
#  define  	FETCH_ATTRIBUTE_NO_MASK_BIT 	 	(1600  + 17)  
#  define  	FETCH_ATTRIBUTE_NOT_FOUND 	 		(1600  + 18)  
#  define  	FETCH_ATTR_LENGTH_OVERFLOW 	 		(1600  + 19)  
#  define  	FETCH_ATTR_ZERO_LENGTH 	 			(1600  + 20)  
#  define  	FETCH_OBJECT_NOT_FOUND 	 			(1600  + 21)  
#  define  	FETCH_DATA_NOT_FOUND 	 			(1600  + 22)  
#  define  	FETCH_DATA_OUT_OF_RANGE 	 		(1600  + 23)  
#  define  	FETCH_INVALID_DIR_TYPE 	 			(1600  + 24)  
#  define  	FETCH_INVALID_TABLE 	 			(1600  + 25)  
#  define  	FETCH_INVALID_EXTN_LEN 	 			(1600  + 26)  
#  define  	FETCH_INVALID_PARAM 	 			(1600  + 27)  
#  define  	FETCH_INVALID_ITEM_ID 	 			(1600  + 28)  
#  define  	FETCH_INVALID_ATTR_LENGTH 	 		(1600  + 29)  
#  define  	FETCH_BAD_DD_DEVICE_LOAD 	 		(1600  + 30)  
#  define  	FETCH_DIR_TYPE_MISMATCH 	 		(1600  + 31)  
#  define  	FETCH_NO_OBJ_EXTN 	 				(1600  + 32)  
#  define  	FETCH_ERROR_END 	 				(1600  + 99)  


/* DDI error codes */
#  define  	DDI_INVALID_FLAT_FORMAT 	 		(1700  + 1)  
#  define  	DDI_INVALID_FLAT_MASKS 	 			(1700  + 2)  
#  define  	DDI_INVALID_TYPE 	 				(1700  + 3)  
#  define  	DDI_INVALID_ITEM_TYPE 	 			(1700  + 4)  
#  define  	DDI_INVALID_PARAM 	 				(1700  + 5)  
#  define  	DDI_MEMORY_ERROR 	 				(1700  + 6)  
#  define  	DDI_TAB_BAD_NAME 	 				(1700  + 7)  
#  define  	DDI_TAB_BAD_DDID 	 				(1700  + 8)  
#  define  	DDI_TAB_BAD_OP_INDEX 	 			(1700  + 9)  
#  define  	DDI_TAB_NO_DEVICE 	 				(1700  + 10)  
#  define  	DDI_TAB_NO_BLOCK 	 				(1700  + 11)  
#  define  	DDI_TAB_BAD_PARAM_OFFSET 	 		(1700  + 12)  
#  define  	DDI_TAB_BAD_PARAM_LIST_OFFSET 	 	(1700  + 13)  
#  define  	DDI_TAB_BAD_CHAR_OFFSET 	 		(1700  + 14)  
#  define  	DDI_TAB_BAD_PARAM_MEMBER 	 		(1700  + 15)  
#  define  	DDI_TAB_BAD_PARAM_LIST_MEMBER 	 	(1700  + 16)  
#  define  	DDI_TAB_BAD_SUBINDEX 	 			(1700  + 17)  
#  define  	DDI_TAB_BAD_PARAM_TYPE 	 			(1700  + 18)  
#  define  	DDI_INVALID_BLOCK_HANDLE 	 		(1700  + 19)  
#  define  	DDI_BLOCK_NOT_FOUND 	 			(1700  + 20)  
#  define  	DDI_INVALID_DEVICE_HANDLE 	 		(1700  + 21)  
#  define  	DDI_DEVICE_NOT_FOUND 	 			(1700  + 22)  
#  define  	DDI_INVALID_DEV_TYPE_HANDLE 	 	(1700  + 23)  
#  define  	DDI_DEV_TYPE_NOT_FOUND 	 			(1700  + 24)  
#  define  	DDI_INVALID_DD_HANDLE 	 			(1700  + 25)  
#  define  	DDI_INVALID_DD_HANDLE_TYPE 	 		(1700  + 26)  
#  define  	DDI_TAB_BAD 	 					(1700  + 27)  
#  define  	DDI_LEGAL_ENUM_VAR_VALUE 	 		(1700  + 28)  
#  define  	DDI_ILLEGAL_ENUM_VAR_VALUE 	 		(1700  + 29)  
#  define  	DDI_DD_BLOCK_REF_NOT_FOUND 	 		(1700  + 30)  
#  define  	DDI_DEVICE_TABLES_NOT_FOUND 	 	(1700  + 31)  
#  define  	DDI_BLOCK_TABLES_NOT_FOUND 	 		(1700  + 32)  
#  define  	DDI_INVALID_BLOCK_TAG 	 			(1700  + 33)  
#  define  	DDI_INVALID_REQUEST_TYPE 	 		(1700  + 34)  
#  define  	DDI_TAB_NO_UNIT 	 				(1700  + 35)  
#  define  	DDI_TAB_NO_WAO 	 					(1700  + 36)  
#  define  	DDI_TAB_NO_UPDATE 	 				(1700  + 37)  
#  define  	DDI_INVALID_DEV_SPEC_STRING 	 	(1700  + 38)  
#  define  	DDI_INSUFFICIENT_BUFFER 	 		(1700  + 39)  
#  define  	DDI_BAD_DD_BLOCK_LOAD 	 			(1700  + 40)  
#  define  	DDI_BAD_DD_DEVICE_LOAD 	 			(1700  + 41)  
#  define  	DDI_NO_COMMANDS_FOR_PARAM 	 		(1700  + 42)  
#  define  	DDI_NO_READ_COMMANDS_FOR_PARAM 	 	(1700  + 43)  
#  define  	DDI_NO_WRITE_COMMANDS_FOR_PARAM 	 (1700  + 44)  
#  define  	DDI_COMMAND_NOT_FOUND 	           (1700  + 45)  
#  define  	DDI_TAB_BAD_PARAM_COUNT 	 		(1700  + 46)  
#  define  	DDI_ERROR_END 	 					(1700  + 99)  

#  define  	BLTIN_SUCCESS 	 					0  
#  define  	BLTIN_NO_MEMORY 	 				(1800 +1)  
#  define  	BLTIN_VAR_NOT_FOUND 	 			(1800 +2)  
#  define  	BLTIN_BAD_ID 	 					(1800 +3)  
#  define  	BLTIN_NO_DATA_TO_SEND 	 			(1800 +4)  
#  define  	BLTIN_WRONG_DATA_TYPE 	 			(1800 +5)  
#  define  	BLTIN_NO_RESP_CODES 	 			(1800 +6)  
#  define  	BLTIN_BAD_METHOD_ID 	 			(1800 +7)  
#  define  	BLTIN_BUFFER_TOO_SMALL 	 			(1800 +8)  
#  define  	BLTIN_CANNOT_READ_VARIABLE 	 		(1800 +9)  
#  define  	BLTIN_INVALID_PROMPT 	 			(1800 +10)  
#  define  	BLTIN_NO_LANGUAGE_STRING 	 		(1800 +11)  
#  define  	BLTIN_DDS_ERROR 	 				(1800 +12)  
#  define  	BLTIN_FAIL_RESPONSE_CODE 	 		(1800 +13)  
#  define  	BLTIN_FAIL_COMM_ERROR 	 			(1800 +14)  
#  define  	BLTIN_NOT_IMPLEMENTED 	 			(1800 +15)  
#  define   BLTIN_BAD_ITEM_TYPE 	 			(1800 +16)  
#  define   BLTIN_VALUE_NOT_SET 	 			(1800 +17)  
#  define   BLTIN_BAD_POINTER 	 				(1800 +18)  
#  define  	BLTN_ERROR_END 	 					(1800 +99)  

#  define  	METH_METHOD_COLLISION 	 			(1900 +1)  
#  define  	METH_INTERNAL_ERROR 	 			(1900 +2)  
#  define  	METH_TOO_MANY_METHODS 	 			(1900 +3)  
#  define  	METH_METHOD_ABORT 	 				(1900 +4)  
#  define  	METH_METHOD_RETRY 	 				(1900 +5)  
#  define   METH_PARSE_ERROR 	 				(1900 +6)  
#  define  	METH_ERROR_END 	 					(1900 +99)  

/* Parameter Cache Errors */
#  define   PC_SUCCESS 	 						0  
#  define   PC_ERROR_START 	 					(2000 +1)  
#  define   PC_METHOD_COLLISION 	 			(2000 +1)  
#  define   PC_NO_READ_OVER_CHANGED_PARAM 	 	(2000 +2)  
#  define   PC_INTERNAL_ERROR 	 				(2000 +3)  
#  define   PC_WRONG_DATA_TYPE 	 				(2000 +4)  
#  define   PC_NO_MEMORY 	 					(2000 +5)  
#  define   PC_VALUE_NOT_SET 	 				(2000 +6)  
#  define   PC_ENTRY_BUSY 	 					(2000 +7)  
#  define   PC_PARAM_NOT_FOUND 	 				(2000 +8)  
#  define   PC_BUFFER_TOO_SMALL 	 			(2000 +9)  
#  define   PC_BAD_STATUS_INFO 	 				(2000 +10)  
#  define   PC_BAD_REQ_TYPE 	    				(2000 +11)  
#  define   PC_BAD_BLOCK_HANDLE 	 			(2000 +12)  
#  define   PC_NO_DEV_VALUE_WRITE 	 			(2000 +13)  
#  define   PC_NO_ACTION_VALUE 	 				(2000 +14)  
#  define   PC_PARAM_CANNOT_BE_WRITTEN 	 		(2000 +15)  
#  define   PC_PARAM_CANNOT_BE_READ 	 		(2000 +16)  
#  define   PC_BAD_POINTER 	 					(2000 +17)  
#  define   PC_BAD_METHOD_TAG 	 				(2000 +18)  
#  define   PC_BAD_PARAM_REQUEST 	 			(2000 +19)  
#  define   PC_NO_DATA_TO_SEND 	 				(2000 +20)  
#  define   PC_ERROR_END 	 					(2000 +99)  

/* layer 7 errors */
#  define   L7_ERROR_START 	                  (2100 +1)  
#  define   L7_INVALID_VAR_TYPE 	             (2100 +2)  
#  define   L7_INVALID_DD_ITEM_TYPE 	         (2100 +3)  
#  define   L7_ALREADY_ACTIVE 	               (2100 +4)   /* NOT USED?? */
#  define   L7_INVALID_FUNCTION_PTR 	         (2100 +5)  
#  define   L7_NO_RESPONSE 	                  (2100 +6)  
#  define   L7_LOCAL_COMM_ERROR 	             (2100 +7)  
#  define   L7_REMOTE_COMM_ERROR 	            (2100 +8)  
#  define   L7_DEVICE_BUSY 	                  (2100 +9)  
#  define   L7_CMD_NOT_IMPLEMENTED 	          (2100 +10)  
#  define   L7_DEVICE_ERROR 	                 (2100 +11)  
#  define   L7_INVALID_RESPONSE_CODE 	        (2100 +12)  
#  define   L7_UNKNOWN_DEVICE 	               (2100 +13)  
#  define   L7_NO_VAR_MATCH 	                 (2100 +14)   /*NOT USED??*/
#  define   L7_NO_READ_COMMAND 	              (2100 +15)  
#  define   L7_NO_WRITE_COMMAND 	             (2100 +16)  
#  define   L7_NO_COMMAND 	                   (2100 +17)  
#  define   L7_INVALID_REQ_DATA_FIELD 	       (2100 +18)  
#  define   L7_INVALID_REPLY_DATA_FIELD 	     (2100 +19)  
#  define   L7_INVALID_NUM_OF_REQ_ITEMS 	     (2100 +20)  
#  define   L7_INVALID_NUM_OF_REPLY_ITEMS 	   (2100 +21)  
#  define   L7_INVALID_VARIABLE_LENGTH 	      (2100 +22)  
#  define   L7_INVALID_RESP_CODE_COUNT 	      (2100 +23)  
#  define   L7_INVALID_COMM_STAT_COUNT 	      (2100 +24)  
#  define   L7_INVALID_DEVICE_STAT_COUNT 	    (2100 +25)  
#  define   L7_DDI_NO_MATCH 	                (2100 +26)  
#  define   L7_INVALID_RESPONSE_CMD 	         (2100 +27)  
#  define   L7_POLL_ADDR_ALREADY_USED 	       (2100 +28)  
#  define   L7_WARNING_DATA_EXPECTED 	        (2100 +29)  
#  define   L7_TRANSMITTER_FAULT 	            (2100 +30)  
#  define   L7_INVALID_TRANSACTION_NUMBER 	   (2100 +31)  
#  define   L7_INVALID_DEVICE_HANDLE 	   		(2100 +31)  
#  define   L7_INVALID_BLOCK_HANDLE 	   		(2100 +32)  
 
#  define   L7_ERROR_END 	                    (2100 +99)  

/* network management errors */
#  define   NM_ERROR_START 	                  (2200 +1)  
#  define   HC_UNKNOWN_DEVICE 	               (2200 +2)  
#  define   HC_MSG_NOT_SENT 	                 (2200 +3)  
#  define   HC_NO_RESPONSE 	                  (2200 +4)  
#  define   HC_COMM_ERROR 	                   (2200 +5)  
#  define   HC_REMOTE_COMM_ERROR 	            (2200 +6)  
#  define   HC_INVALID_CMD 	                  (2200 +7)  
#  define   HC_DEVICE_ERROR 	                 (2200 +8)  
#  define   HC_INVALID_PARAM 	                (2200 +9)  
#  define   HC_NO_MEM 	                       (2200 +10)  
#  define   HC_ACTIVE  	                      (2200 +11)  
#  define   HC_INACTIVE 	                     (2200 +12)  
#  define   HC_NO_PROCESS 	                   (2200 +13)  
#  define   HC_CLEANUP 	                      (2200 +14)  
#  define   HC_EXIST 	                        (2200 +15)  
#  define   HC_UART_FAILURE 	                 (2200 +16)  
#  define   HC_CMD_NOT_IMPLEMENTED 	          (2200 +17)  
#  define   HC_DEVICE_BUSY 	                  (2200 +18)  
#  define   HC_RE_IDENT 	                     (2200 +19)  
#  define   HC_UPDATE_KNOWN_DEVICE 	          (2200 +20)  
#  define   HC_HANDLE_UNAVAILABLE 	           (2200 +21)  
#  define   NM_UNKNOWN_VARIABLE 	             (2200 +22)  
#  define   NM_UNKNOWN_TABLE 	                (2200 +23)  
#  define   HC_DEVICE_DISCONNECTED 	          (2200 +24)  
#  define   NM_ERROR_END 	                    (2200 +99)  
 
/* Comm Stack errors */
#  define   CS_ERROR_START 	                  (2300 +1)  
#  define   PS_SUCCESS 	                      0  
#  define   PS_INVALID_BLOCK_HANDLE 	         (2300 +2)  
#  define   PS_INVALID_DEVICE_HANDLE 	        (2300 +3)  


#  define   PS_INVALID_HART_TYPE 	            (2300 +2)  

#  define   CS_ERROR_END 	                    (2300 +99)  

/* Connection Manager Errors */
#  define   CM_SUCCESS 	 						0  
#  define   CM_NO_MEMORY 	 					(2400 +0)  
#  define   CM_BLOCK_NOT_FOUND 	 				(2400 +1)  
#  define   CM_DEVICE_NOT_FOUND 	 			(2400 +2)  
#  define   CM_DEVICE_TYPE_NOT_FOUND 	 		(2400 +3)  
#  define   CM_BAD_BLOCK_OPEN 	 				(2400 +4)  
#  define   CM_BAD_BLOCK_CLOSE 	 				(2400 +5)  
#  define   CM_BAD_BLOCK_HANDLE 	 			(2400 +6)  
#  define   CM_BAD_PARAM_OFFSET 	 			(2400 +7)  
#  define   CM_BAD_INDEX 	 					(2400 +8)  
#  define   CM_BAD_OPEN 	 					(2400 +9)  
#  define   CM_NOT_DDOD_FILE 	 				(2400 +10)  
#  define   CM_FILE_ERROR 	 					(2400 +11)  
#  define   CM_NO_DEVICE 	 					(2400 +12)  
#  define   CM_NO_BLOCK 	 					(2400 +13)  
#  define   CM_BAD_BLOCK_OBJECT 	 			(2400 +14)  
#  define   CM_NO_FUNCTION 	 					(2400 +15)  
#  define   CM_BAD_SYMBOL 	 					(2400 +16)  
#  define   CM_BAD_DEVICE_HANDLE 	 			(2400 +17)  
#  define   CM_NO_COMM 	 						(2400 +18)  
#  define   CM_BAD_ROD_OPEN 	 				(2400 +19)  
#  define   CM_BLOCK_TAG_NOT_FOUND 	 			(2400 +20)  
#  define   CM_INVALID_ROD_HANDLE 	 			(2400 +21)  
#  define   CM_ILLEGAL_PUT 	 					(2400 +22)  
#  define   CM_NO_ROD_MEMORY 	 				(2400 +23)  
#  define   CM_OBJECT_NOT_IN_HEAP 	 			(2400 +24)  
#  define   CM_NO_OBJECT 	 					(2400 +25)  
#  define   CM_NO_VALUE 	 					(2400 +26)  
#  define   CM_BAD_DD_BLK_LOAD 	 				(2400 +27)  
#  define   CM_BAD_DD_DEV_LOAD 	 				(2400 +28)  
#  define   CM_NO_DD_BLK_REF 	 				(2400 +29)  
#  define   CM_FILE_NOT_FOUND 	 				(2400 +30)  
#  define   CM_BAD_DD_FLAT_LOAD 	 			(2400 +31)  
#  define   CM_BAD_DDOD_LOAD 	 				(2400 +32)  
#  define   CM_BAD_DD_HANDLE 	 				(2400 +33)  
#  define   CM_BAD_BLOCK_LOAD 	 				(2400 +34)  
#  define   CM_ILLEGAL_WRITE 	 				(2400 +35)  
#  define   CM_VALUE_NOT_IN_HEAP 	 			(2400 +36)  
#  define   CM_NO_DATA_TYPE 	 				(2400 +37)  
#  define   CM_BAD_OBJECT 	 					(2400 +38)  
#  define   CM_BAD_SUBINDEX 	 				(2400 +39)  
#  define   CM_BAD_OPROD_OPEN 	 				(2400 +40)  
#  define   CM_BAD_FILE_OPEN 	 				(2400 +41)  
#  define   CM_BAD_FILE_CLOSE 	 				(2400 +42)  
#  define   CM_COMM_ERR 	 					(2400 +43)  
#  define   CM_RESPONSE_CODE 	 				(2400 +44)  
#  define   CM_BAD_COMM_TYPE 	 				(2400 +45)  
#  define   CM_FF_PARAM_LOAD_FAILED 	 		(2400 +46)  
#  define   CM_FF_PARAM_WRITE_FAILED 	 		(2400 +47)  
#  define   CM_FF_PARAM_READ_FAILED 	 		(2400 +48)  
#  define   CM_BAD_POINTER 	 					(2400 +49)  
#  define   CM_INTERNAL_ERROR 	 				(2400 +50)  
#  define   CM_BAD_NETWORK_HANDLE 	 			(2400 +51)  
#  define   CM_BAD_DEVICE_TYPE_HANDLE 	 		(2400 +52)  
#  define   CM_BAD_DD_PATH 	 					(2400 +53)  
#  define   CM_BAD_CM_MODE 	 					(2400 +54)  
#  define   CM_ERROR_END 	 					(2400 +99)  

/* FF Device Simulator Errors */
#  define   FFDS_SUCCESS 	 					0  
#  define   FFDS_MEMORY_ERR 	 				(2500 +0)  
#  define   FFDS_FILE_OPEN_ERR 	 			(2500 +1)  
#  define   FFDS_FILE_READ_ERR 	 			(2500 +2)  
#  define   FFDS_FILE_CLOSE_ERR 	 			(2500 +3)  
#  define   FFDS_MULT_BLK_ERR 	 				(2500 +4)  
#  define   FFDS_MULT_DEV_ERR 	 				(2500 +5)  
#  define   FFDS_BLK_CLASS_ERR 	 			(2500 +6)  
#  define   FFDS_PHYS_BLK_ERR 	 				(2500 +7)  
#  define   FFDS_OD_CONFLICT_ERR 	 			(2500 +7)  
#  define   FFDS_SYM_FILE_ERR 	 				(2500 +8)  
#  define   FFDS_SYMBOL_ERR 	 				(2500 +9)  
#  define   FFDS_DEV_HANDLE_ERR 	 			(2500 +10)  
#  define   FFDS_ROD_ERR 	 					(2500 +11)  
#  define   FFDS_COMM_ERR 	 					(2500 +12)  
#  define   FFDS_FIND_BLK_ERR 	 				(2500 +13)  
#  define   FFDS_CFG_FILE_ID_ERR 	 			(2500 +14)  
#  define   FFDS_PUT_INT_ERR 	 				(2500 +15)  
#  define   FFDS_OBJECT_ERR 	 				(2500 +16)  
#  define   FFDS_ERROR_END 	 				(2500 +99)  


/* HART Device Simulator Errors */
#  define   HRTDS_SUCCESS 	 					0  
#  define   HRTDS_MEMORY_ERR 	 				(2600 +0)  
#  define   HRTDS_FILE_OPEN_ERR 	 			(2600 +1)  
#  define   HRTDS_FILE_READ_ERR 	 			(2600 +2)  
#  define   HRTDS_FILE_CLOSE_ERR 	 			(2600 +3)  
#  define   HRTDS_UNKNOWN_VAR_TYPE 	 			(2600 +4)  
#  define   HRTDS_INVALID_CFG_FILE_ID 	 		(2600 +5)  
#  define   HRTDS_DEVICE_NOT_FOUND 	 			(2600 +6)  
#  define   HRTDS_NETWORK_NOT_FOUND 	 		(2600 +7)  
#  define   HRTDS_TOO_MANY_NETWORKS 	 		(2600 +8)  
#  define   HRTDS_TOO_MANY_DEVICES 	 			(2600 +9)  
#  define   HRTDS_DEVICE_DISCONNECTED 	 		(2600 +10)  
#  define   HRTDS_INVALID_NUM_OF_ITEMS 	 		(2600 +11)  
#  define   HRTDS_VAR_NOT_FOUND 	 			(2600 +12)  
#  define   HRTDS_ERROR_END 	 				(2600 +99)  


/* RCSIM return types. */
#  define   RCSIM_SUCCESS 	  	 				0  
#  define   RCSIM_MATCH 	  	 				(2700 +0)  
#  define   RCSIM_NOMATCH 	  	 				(2700 +5)  
#  define   RCSIM_END_OF_LIST 	 				(2700 +6)  
#  define   RCSIM_NO_ITEMS 	 					(2700 +7)  
#  define   RCSIM_ERR_CLASS_FIND 	  	 		(2700 +10)  
#  define   RCSIM_ERR_NETWORK_FIND 	  	 	(2700 +11)  
#  define   RCSIM_ERR_STATION_FIND 	  	 	(2700 +12)  
#  define   RCSIM_ERR_MEMORY 	  	 			(2700 +20)  
#  define   RCSIM_ERR_NET_ID 	  	 			(2700 +21)  
#  define   RCSIM_ERR_CLASS 	  	 			(2700 +22)  
#  define   RCSIM_ERR_VALUE_TYPE 	  	 		(2700 +23)  
#  define   RCSIM_ERR_NET_FILE_OPEN 	  	 	(2700 +24)  
#  define   RCSIM_ERR_NET_FILE_READ 	  	 	(2700 +25)  
#  define   RCSIM_ERR_NET_FILE_CLOSE 	  	 	(2700 +26)  
#  define   RCSIM_ERR_SIM_FILE_OPEN 	  	 	(2700 +27)  
#  define   RCSIM_ERR_SIM_FILE_READ 	  	 	(2700 +28)  
#  define   RCSIM_ERR_SIM_FILE_CLOSE 	  	 	(2700 +29)  
#  define   RCSIM_ERROR_END 	  	 			(2700 +99)  




#  endif		/* RTN_CODE_H */

