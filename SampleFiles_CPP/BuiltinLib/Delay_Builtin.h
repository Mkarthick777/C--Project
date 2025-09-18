
#ifndef _DELAY_BUILTIN_H
#define _DELAY_BUILTIN_H


#define MAX_PROMPT_MSG_PARAMS 10

typedef unsigned short USHORT;
typedef unsigned long ULONG;


typedef struct {
    ULONG     id;
    int         po;
    char	*name;	
    int    subindex;
    int    subsubindex;
    int         type;
} P_REF;

/**********************************************************************************************
** Global Functions
*********************************************************************************************/
#ifndef DELAY_BUILTIN

extern int process_prompt(char *, ULONG *, ULONG *, long *);
// use the dictionary version  
// stevev 09aug13  extern int ddi_get_string_translation(tchar *string, tchar *lang_cntry, tchar *outbuf,int outbuf_size);
//(void) display_message(message, param_ids, member_ids,id_count);
//extern long delayfor (long , char *, ULONG *, ULONG *, long );

#endif

#endif //_DELAY_BUILTIN_H