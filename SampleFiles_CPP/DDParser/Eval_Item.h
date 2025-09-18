/********************************************************************************************

	This file contains the declarations of the functions to evaluate individual Items


********************************************************************************************/

#ifndef EVALITEM_H
#define EVALITEM_H


#include "DDlItems.h"
#include "Flats.h"

int eval_variable(FLAT_VAR* fvar, unsigned long ulVarMask,DDlVariable *pVar);

int eval_command(FLAT_COMMAND* fcmd, unsigned long ulCmdMask,DDlCommand *pCmd);

int eval_menu(FLAT_MENU* fmenu, unsigned long ulMenuMask,DDlMenu *pMenu);

int eval_edit_display(FLAT_EDIT_DISPLAY* fedit, unsigned long ulEditDispMask,DDlEditDisplay *pEditDisp);

int eval_method(FLAT_METHOD* fmeth, unsigned long ulMethodMask,DDlMethod *pMeth);

int eval_refresh(FLAT_REFRESH* frfrsh, unsigned long ulRefrshMask,DDlRefresh *pRefrsh);

int eval_unit(FLAT_UNIT* funit, unsigned long ulUnitMask,DDlUnit *pUnit);

int eval_wao(FLAT_WAO* fwao, unsigned long ulWaoMask,DDlWao *pWao);

int eval_item_array(FLAT_ITEM_ARRAY* fiarr, unsigned long ulItemArrayMask,DDlItemArray *pIArray);;

int eval_collection(FLAT_COLLECTION* fcoll, unsigned long ulCollectionMask,DDlCollection *pColl);

int eval_record(FLAT_RECORD* frec, unsigned long ulRecordMask,DDlRecord *pRec);

int eval_block(FLAT_BLOCK* fblock, unsigned long ulBlockMask,DDlBlock *pBlock);



#endif /*EVALITEM_H*/



