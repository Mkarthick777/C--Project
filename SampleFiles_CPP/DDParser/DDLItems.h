

#ifndef DDLITEMS_H
#define DDLITEMS_H

#include <vector>

#include "Attributes.h"

#include "DDlConditional.h"

#include "DDlBaseItem.h"

/*This file contains the specific definitions of the DDL items */
/*All the DDL item classes are derived from the DDLBaseItem class*/

class DDlVariable : public DDlBaseItem		/*Item Type == 1*/
{

public:
	
	DDlVariable(){
		//ADDED by Deepak
//		variableAttrList.clear();
	};
	
	virtual ~DDlVariable()
	{
		
	};

	//void AllocVarAttributes( unsigned long ulVarMask);
	void AllocAttributes( unsigned long attrMask);


};


class DDlCommand : public DDlBaseItem		/*Item Type == 2*/
{
	
public:

	DDlCommand(){
		//ADDED by Deepak
//		commandAttrList.clear();
	};
	
	virtual ~DDlCommand()
	{
//		if(commandAttrList.size() > 0)
//		{
//			//COMMENT: Why being deleted in destructor ?
//			for(ItemAttrList:: iterator it = commandAttrList.begin(); it != commandAttrList.end(); it++)
//			{
//				delete(*it);
//			}
//			commandAttrList.clear();
//		}
	};
	
//	void AllocCmdAttributes( unsigned long ulCmdMask);
	void AllocAttributes( unsigned long attrMask);

};


class DDlMenu : public DDlBaseItem			/*Item Type == 3*/
{

public:
	
	DDlMenu(){
		//ADDED by Deepak
//		menuAttrList.clear();
	};
	
	virtual ~DDlMenu();
/*	moved implementation to ddlitems.cpp  23mar09  {
		if(menuAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = menuAttrList.begin(); it != menuAttrList.end(); it++)
			{
				delete(*it);
			}
			menuAttrList.clear();
		}
*  /
#ifdef _DBGMIL
if (id == 0x3eb)
{
	MENU_ITEM_LIST* pMil = NULL;
	ItemAttrList :: iterator p;

	for(p = attrList.begin();p != attrList.end();p++)
	{
		if((*p)->byAttrID == MENU_ITEMS_ID)
		{
			pMil = (*p)->pVals->menuItemsList;
			break;
		}
	}// next attr	
	pMil->dumpList();
}
#endif
	};
**/

//	void AllocMenuAttributes( unsigned long ulMenuMask);
	void AllocAttributes( unsigned long attrMask);

};


class DDlEditDisplay : public DDlBaseItem		/*Item Type == 4*/
{

public:

	DDlEditDisplay(){
		//ADDED by Deepak
//		editDisplayAttrList.clear();
	};
	
	virtual ~DDlEditDisplay()
	{
/*		if(editDisplayAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = editDisplayAttrList.begin(); it != editDisplayAttrList.end(); it++)
			{
				delete(*it);
			}
			editDisplayAttrList.clear();
		}
*/
	};
	
//	void AllocEditDispAttributes( unsigned long ulEditDispMask);
	void AllocAttributes( unsigned long attrMask);

};


class DDlMethod : public DDlBaseItem			/*Item Type == 5*/
{

public:

	DDlMethod(){
		//ADDED by Deepak
//		methodAttrList.clear();
	};
	
	virtual ~DDlMethod()
	{
/*		if(methodAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = methodAttrList.begin(); it != methodAttrList.end(); it++)
			{
				delete(*it);
			}
			methodAttrList.clear();
		}
*/
	};
	
//	void AllocMethodAttributes( unsigned long ulMethodMask);
	void AllocAttributes( unsigned long attrMask);

};


class DDlRefresh : public DDlBaseItem			/*Item Type == 6*/
{

public:

	DDlRefresh(){
		//ADDED by Deepak
//		refreshAttrList.clear();
	};
	
	virtual ~DDlRefresh()
	{
/*		if(refreshAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = refreshAttrList.begin(); it != refreshAttrList.end(); it++)
			{
				delete(*it);
			}
			refreshAttrList.clear();
		}
*/
	};
	
//	void AllocRefreshAttributes( unsigned long ulRefreshMask);
	void AllocAttributes( unsigned long attrMask);

};

class DDlUnit : public DDlBaseItem				/*Item Type == 7*/
{

public:

	DDlUnit(){
		//ADDED by Deepak
//		unitAttrList.clear();
	};
	
	virtual ~DDlUnit()
	{
/*		if(unitAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = unitAttrList.begin(); it != unitAttrList.end(); it++)
			{
				delete(*it);
			}
			unitAttrList.clear();
		}
*/
	};
	
//	void AllocUnitAttributes( unsigned long ulUnitMask);
	void AllocAttributes( unsigned long attrMask);

};


class DDlWao : public DDlBaseItem				/*Item Type == 8*/
{

public:

	DDlWao(){
		//ADDED by Deepak
//		waoAttrList.clear();
	};
	
	virtual ~DDlWao()
	{
/*		if(waoAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = waoAttrList.begin(); it != waoAttrList.end(); it++)
			{
				delete(*it);
			}
			waoAttrList.clear();
		}
*/
	};
	
//	void AllocWaoAttributes( unsigned long ulWaoMask);
	void AllocAttributes( unsigned long attrMask);

};	


class DDlItemArray : public DDlBaseItem			/*Item Type == 9*/
{

public:

	DDlItemArray(){
		//ADDED by Deepak
//		iArrAttrList.clear();
	};
	
	virtual ~DDlItemArray()
	{
/*		if(iArrAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = iArrAttrList.begin(); it != iArrAttrList.end(); it++)
			{
				delete(*it);
			}
			iArrAttrList.clear();
		}
*/
	};
	
//	void AllocItemArrayAttributes( unsigned long ulItemArrayMask);
	void AllocAttributes( unsigned long attrMask);

};


class DDlCollection : public DDlBaseItem		/*Item Type == 10*/
{

public:

	DDlCollection(){
		//ADDED by Deepak
//		collectionAttrList.clear();
	};
	
	virtual ~DDlCollection()
	{
/*		if(collectionAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = collectionAttrList.begin(); it != collectionAttrList.end(); it++)
			{
				delete(*it);
			}
			collectionAttrList.clear();
		}
*/
	};
	
//	void AllocCollectionAttributes( unsigned long ulCollectionMask);
	void AllocAttributes( unsigned long attrMask);

};


class DDlBlock : public DDlBaseItem				/*Item Type == 12*/
{

public:

	DDlBlock(){
		//ADDED by Deepak
//		blockAttrList.clear();
	};
	
	virtual ~DDlBlock()
	{
/*		if(blockAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = blockAttrList.begin(); it != blockAttrList.end(); it++)
			{
				delete(*it);
			}
			blockAttrList.clear();
		}
*/	};
	
//	void AllocBlockAttributes( unsigned long ulBlockMask);
	void AllocAttributes( unsigned long attrMask);

};


class DDlRecord : public DDlBaseItem			/*Item Type == 14*/
{

public:

	DDlRecord(){
		//ADDED by Deepak
//		recordAttrList.clear();
	};
	
	virtual ~DDlRecord()
	{
/*		if(recordAttrList.size() > 0)
		{
			//COMMENT: Why being deleted in destructor ?
			for(ItemAttrList:: iterator it = recordAttrList.begin(); it != recordAttrList.end(); it++)
			{
				delete(*it);
			}
			recordAttrList.clear();
		}
*/	};
	
//	void AllocRecordAttributes( unsigned long ulRecordMask);
	void AllocAttributes( unsigned long attrMask);
};


#endif	/*DDLITEMS_H*/
