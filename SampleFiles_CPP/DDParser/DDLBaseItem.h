

#ifndef DDLBASEITEM_H
#define DDLBASEITEM_H



#include <string>
#include "pvfc.h"


#include "Attributes.h"

#include "DDlConditional.h"

// this is us??? #include "DDlBaseItem.h"

using namespace std;

typedef vector<DDlAttribute *>   ItemAttrList;

/* This class basically contains the */
class DDlBaseItem
{

public:

	ITEM_ID		id;		/* Unique Id of the item*/
	BYTE		byItemType;		/* Type of the item*/
	BYTE		byItemSubType;	/* Sub Type (if applicable of the item */
	
	string		strItemName; /*This guy might go....*/ 
	
	unsigned long ulItemMasks; /*The mask of the attributes item is having */

	
	ItemAttrList	attrList;

	unsigned long getItemId(){ return id;};
	BYTE		  getItemType() { return byItemType;};
	BYTE		  getItemSubType(){ return byItemSubType;}; 
								/*This one is applicable for ItemArrays and collections only*/
	
	// pure virtual function -over-ride in classes
	virtual void AllocAttributes(unsigned long attrMask) PVFC( "DDlBaseItem_AA" ); 

	DDlBaseItem()
	{ id = 0; byItemType = byItemSubType = RESERVED_ITYPE1;
	  ulItemMasks = 0xFFFFFFFF; attrList.clear();strItemName.erase();attrList.clear();};

	virtual 
	~DDlBaseItem(){ 
		if(attrList.size() > 0)
		{	for(ItemAttrList:: iterator it = attrList.begin(); it != attrList.end(); it++)
			{	delete(*it);
			}
			attrList.clear();
		} };
};

/******************************************************************************/
// stevev 07Jan05 - image support - used to get the info out of the flat_device_dir
//					before it is cleared
struct IMAGEFRAME_S
{
	unsigned int   ifs_size;		// in bytes
	unsigned char* ifs_pRawFrame;	// pointer to the raw image
	unsigned int   ifs_offset;		// for reference
	char           ifs_language[6]; // only the first 3 are used
};

typedef vector<IMAGEFRAME_S>    BIframeList_t;
typedef BIframeList_t::iterator BIframeList_it;

typedef vector<BIframeList_t>   BIimageList_t;
typedef BIimageList_t::iterator BIimageList_it;

#endif /*DDLBASEITEM_H*/