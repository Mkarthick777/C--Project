#ifndef GLOBALDEFS_H
#define GLOBALDEFS_H

#include "DD_Header.h"

//Vibhor 200105: Increasing the size of arrays, was 1500
extern DOMAIN_FIXED ObjectFixed[];//[1800];
extern BYTE byExtLengths[];//[1800]; /*Array to hold the object Extension Lengths*/ //Vibhor 300904: Restored
extern BYTE *pbyExtensions[];//[1800];/*Array of pointers pointing to the Extension parts of objects*/
extern BYTE *pbyObjectValue[];//[1800];/*Array of pointers pointing to the value (data) parts of the objects*/
extern unsigned uSODLength;



#endif /*GLOBALDEFS_H*/