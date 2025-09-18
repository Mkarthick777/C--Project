
/*
 * generated from DDLItems.h 7/21/04
 * 
 */

#ifndef DDL6ITEMS_H
#define DDL6ITEMS_H

#include <vector>
#include "pvfc.h"

#include "DDLItems.h"
/*This file contains the specific definitions of the DDL HART6 items */
/*All the DDL item classes are derived from the DDLBaseItem class    */
#include "flats.h"

typedef union flats6
{
	FLAT_VAR			fVar;
	FLAT_COMMAND		fCmd;
	FLAT_MENU			fMenu;
	FLAT_EDIT_DISPLAY	fEditDisp;
	FLAT_METHOD			fMethod;
	FLAT_ITEM_ARRAY		fIArr;
	FLAT_COLLECTION		fColl;
	FLAT_REFRESH		fRefresh;
	FLAT_UNIT			fUnit;
	FLAT_WAO			fWao;
	FLAT_RECORD			fRec;
	FLAT_BLOCK			fBlock;
	FLAT_ARRAY			fArr;
	//FLAT_PROGRAM		fProg;
	//FLAT_VAR_LIST		fVarList;
	//FLAT_RESP_CODE	fRespCd;
	//FLAT_DOMAIN		fDomain;
	FLAT_FILE			fFile;
	FLAT_CHART			fChart;
	FLAT_GRAPH			fGraph;
	FLAT_AXIS			fAxis;
	FLAT_WAVEFORM		fWaveFrm;
	FLAT_SOURCE			fSource;
	FLAT_LIST			fList;
	FLAT_GRID           fGrid;
	FLAT_IMAGE			fImage;
	FLAT_BLOB			fBlob;
}
FLAT_UNION_T;

class DDL6BaseItem
{
protected:
	static FLAT_UNION_T glblFlats;// we only do one item at a time

   int preFetchItem(DDlBaseItem* pBaseItm, BYTE maskSize, BYTE** pObjExtn, /*INT*/int& rSize);

public:	
	unsigned long attrMask;

	DDL6BaseItem(){attrMask=0L;};
	virtual int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex) RPVFC( "DDL6BaseItem_A",0 );// pure virtual func
	virtual int  eval_attrs(void) RPVFC( "DDL6BaseItem_B",0 );
	virtual void clear_flat(void) PVFC( "DDL6BaseItem_C" );
};



class DDl6Variable : public DDlVariable, public DDL6BaseItem		/*Item Type == 1*/
{
	FLAT_VAR* pVar;

public:
	DDl6Variable() 
	{ byItemType=VARIABLE_ITYPE; strItemName = "Variable";  pVar = &(glblFlats.fVar);};	          
	virtual ~DDl6Variable(){	};

	void AllocAttributes( void );
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6Command : public DDlCommand, public DDL6BaseItem			/*Item Type == 2*/
{	
	FLAT_COMMAND* pCmd;

public:
	DDl6Command()
	{byItemType=COMMAND_ITYPE; strItemName = "Command";  pCmd = &(glblFlats.fCmd);};
	
	virtual ~DDl6Command(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6Menu : public DDlMenu, public DDL6BaseItem				/*Item Type == 3*/
{
	FLAT_MENU* pMenu;

public:	
	DDl6Menu()
	{byItemType=MENU_ITYPE; strItemName = "Menu";  pMenu = &(glblFlats.fMenu);};
	
	virtual ~DDl6Menu(){};

	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6EditDisplay : public DDlEditDisplay, public DDL6BaseItem	/*Item Type == 4*/
{
	FLAT_EDIT_DISPLAY* pEdDisp;

public:
	DDl6EditDisplay()
	{byItemType=EDIT_DISP_ITYPE; strItemName = "Edit Display";  pEdDisp = &(glblFlats.fEditDisp);};
	
	virtual ~DDl6EditDisplay(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6Method : public DDlMethod, public DDL6BaseItem			/*Item Type == 5*/
{
	FLAT_METHOD* pMthd;

public:
	DDl6Method()
	{byItemType=METHOD_ITYPE; strItemName = "Method";  pMthd = &(glblFlats.fMethod);};
	
	virtual ~DDl6Method(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6Refresh : public DDlRefresh, public DDL6BaseItem			/*Item Type == 6*/
{
	FLAT_REFRESH* pRfsh;

public:
	DDl6Refresh()
	{byItemType=REFRESH_ITYPE; strItemName = "Refresh Relation";  pRfsh = &(glblFlats.fRefresh);};
	
	virtual ~DDl6Refresh(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);

};

class DDl6Unit : public DDlUnit, public DDL6BaseItem				/*Item Type == 7*/
{
	FLAT_UNIT* pUnit;

public:
	DDl6Unit()
	{byItemType=UNIT_ITYPE; strItemName = "Unit Relation";  pUnit = &(glblFlats.fUnit);};
	
	virtual ~DDl6Unit(){};
	
	void AllocAttributes( void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6Wao : public DDlWao, public DDL6BaseItem					/*Item Type == 8*/
{
	FLAT_WAO* pWao;

public:
	DDl6Wao()
	{byItemType=WAO_ITYPE; strItemName = "WAO Relation";  pWao = &(glblFlats.fWao);};
	
	virtual ~DDl6Wao(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};	


class DDl6ItemArray : public DDlItemArray, public DDL6BaseItem			/*Item Type == 9*/
{
	FLAT_ITEM_ARRAY* pItmArr;

public:
	DDl6ItemArray()
	{byItemType=ITEM_ARRAY_ITYPE; strItemName = "Item Array";  pItmArr = &(glblFlats.fIArr);};
	
	virtual ~DDl6ItemArray(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6Collection : public DDlCollection, public DDL6BaseItem		/*Item Type == 10*/
{
	FLAT_COLLECTION* pColl;

public:
	DDl6Collection()
	{byItemType=COLLECTION_ITYPE; strItemName = "Collection";  pColl = &(glblFlats.fColl);};
	
	virtual ~DDl6Collection(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6Block : public DDlBlock, public DDL6BaseItem					/*Item Type == 12*/
{
	FLAT_BLOCK* pBlk;

public:
	DDl6Block()
	{byItemType=BLOCK_ITYPE; strItemName = "Block";  pBlk = &(glblFlats.fBlock);};
	
	virtual ~DDl6Block(){};
	
	void AllocAttributes( void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

/* UNUSED: PROGRAM_ITYPE */

class DDl6Record : public DDlRecord, public DDL6BaseItem				/*Item Type == 14*/
{
	FLAT_RECORD* pRec;

public:
	DDl6Record()
	{byItemType=RECORD_ITYPE; strItemName = "Record";  pRec = &(glblFlats.fRec);};
	
	virtual ~DDl6Record(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


/* * * * * DDL 6 Additions * * * * * */
class DDl6Array : public DDlBaseItem, public DDL6BaseItem			/*Item Type == 15*/
{
	FLAT_ARRAY* pArr;
	void AllocAttributes(unsigned long attrMask){};// to make base item happy

public:
	DDl6Array()
	{byItemType=ARRAY_ITYPE; strItemName = "Array";  pArr = &(glblFlats.fArr);};
	
	virtual ~DDl6Array(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

/* UNUSED:	VAR_LIST_ITYPE,
			RESP_CODES_ITYPE,
			DOMAIN_ITYPE,
			MEMBER_ITYPE    * */

class DDl6File :     public DDlBaseItem, public DDL6BaseItem		/*Item Type == 20*/
{
	FLAT_FILE* pFile;
	void AllocAttributes(unsigned long attrMask){};// to make base item happy

public:
	DDl6File()
	{byItemType=FILE_ITYPE; strItemName = "File";  pFile = &(glblFlats.fFile);};
	
	virtual ~DDl6File(){};
	
	void AllocAttributes(void);
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

class DDl6Chart :    public DDlBaseItem, public DDL6BaseItem		/*Item Type == 21*/
{
	FLAT_CHART*	pChart;
public:
	DDl6Chart()
	{byItemType=CHART_ITYPE; strItemName = "Chart";  pChart = &(glblFlats.fChart);};
	
	virtual ~DDl6Chart(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

class DDl6Graph :    public DDlBaseItem, public DDL6BaseItem		/*Item Type == 22*/
{
	FLAT_GRAPH*	pGraph;
public:
	DDl6Graph()
	{byItemType=GRAPH_ITYPE; strItemName = "Graph";  pGraph = &(glblFlats.fGraph);};
	
	virtual ~DDl6Graph(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

class DDl6Axis :     public DDlBaseItem, public DDL6BaseItem		/*Item Type == 23*/
{	
	FLAT_AXIS*	pAxis;
public:
	DDl6Axis()
	{byItemType=AXIS_ITYPE; strItemName = "Axis";  pAxis = &(glblFlats.fAxis);};
	
	virtual ~DDl6Axis(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

class DDl6Waveform : public DDlBaseItem, public DDL6BaseItem		/*Item Type == 24*/
{
	FLAT_WAVEFORM* pWaveFrm;
public:
	DDl6Waveform()
	{byItemType=WAVEFORM_ITYPE; strItemName = "Waveform";  pWaveFrm = &(glblFlats.fWaveFrm);};
	
	virtual ~DDl6Waveform(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

class DDl6Source :   public DDlBaseItem, public DDL6BaseItem		/*Item Type == 25*/
{
	FLAT_SOURCE* pSource;
public:
	DDl6Source()
	{byItemType=SOURCE_ITYPE; strItemName = "Source";  pSource = &(glblFlats.fSource);};
	
	virtual ~DDl6Source(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

class DDl6List :     public DDlBaseItem, public DDL6BaseItem		/*Item Type == 26*/
{
	FLAT_LIST* pList;
public:
	DDl6List()
	{byItemType=LIST_ITYPE; strItemName = "List";  pList = &(glblFlats.fList);};
	
	virtual ~DDl6List(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};


class DDl6Grid :     public DDlBaseItem, public DDL6BaseItem		/*Item Type == 27*/
{
	FLAT_GRID* pGrid;
public:
	DDl6Grid()
	{byItemType=GRID_ITYPE; strItemName = "Grid";  pGrid = &(glblFlats.fGrid);};
	
	virtual ~DDl6Grid(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

class DDl6Image :     public DDlBaseItem, public DDL6BaseItem		/*Item Type == 28*/
{
	FLAT_IMAGE* pImage;
public:
	DDl6Image()
	{byItemType=IMAGE_ITYPE; strItemName = "Image";  pImage = &(glblFlats.fImage);};
	
	virtual ~DDl6Image(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

class DDl6Blob :     public DDlBaseItem, public DDL6BaseItem		/*Item Type == 29*/
{
	FLAT_BLOB* pBlob;
public:
	DDl6Blob(){ byItemType=BLOB_ITYPE; strItemName = "Blob";  pBlob = &(glblFlats.fBlob);};
	
	virtual ~DDl6Blob(){};
	
	void AllocAttributes(void);
	void AllocAttributes(unsigned long attrMask){AllocAttributes();};// make base item happy
	int  fetch_item(BYTE* pbyObjExtn, OBJECT_INDEX objIndex);
	int  eval_attrs(void);
	void clear_flat(void);
};

#endif	/*DDLITEMS_H*/
