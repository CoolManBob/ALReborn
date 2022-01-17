/*=====================================================================

	AgsdProduct.h

=====================================================================*/


#ifndef _AGSD_PRODUCT_H_
	#define _AGSD_PRODUCT_H_


#include "ApBase.h"
#include "AgpmSkill.h"
#include "AgpmProduct.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGSMPRODUCT_MAX_RESULT_ITEM			10


const enum eAGSMPRODUCT_ITEM_SET_EXCEL_COLUMN
	{
	AGSMPRODUCT_ITEM_SET_EXCEL_COLUMN_SET_ID = 0,					// set id
	AGSMPRODUCT_ITEM_SET_EXCEL_COLUMN_ITEM_TID,					// item tid
	AGSMPRODUCT_ITEM_SET_EXCEL_COLUMN_PROB,						// probability
	AGSMPRODUCT_ITEM_SET_EXCEL_COLUMN_SKILL_LEVEL,				// skill level
	};




/************************************************/
/*		The Definition of Product Item Set		*/
/************************************************/
//
//	==========	Result Item Element	==========
//
struct AgsdProductItemElement
	{
	INT32		m_lItemTID;		//		item TID
	INT32		m_lRange;		//		range(0 ~ 100)
	};


typedef vector<AgsdProductItemElement>	ProductResultItems;
typedef ProductResultItems::iterator	ProductResultItemsIter;


//
//	========== Result Item Set	==========
//
class AgsdProductItemSet
	{
	public:
		INT32		m_lID;
		ApSafeArray<ProductResultItems, AGPMPRODUCT_MAX_LEVEL>	m_LevelSet;			// 0 based
	};




/************************************************************************/
/*		The Definition of Product Result Character Attached Data		*/
/************************************************************************/
//
class AgsdProductResultCAD : public ApBase
	{
	public:
		AgpdSkill*		m_pAgpdSkill;	// skill
		ApBase*			m_pTarget;		// target
		INT32			m_eResult;		// result(eAGPMPRODUCT_RESULT).
		UINT32			m_ulClock;		// casted clock
		INT32			m_lItemTID;		// present item TID
		INT32			m_lItemQty;		// present item quantity
		INT32			m_lExtra;

	public:
		void Init()
			{
			m_pAgpdSkill = NULL;
			m_pTarget = NULL;
			m_eResult = AGPMPRODUCT_RESULT_FAIL;
			m_ulClock = 0;
			m_lItemTID = 0;
			m_lItemQty = 0;
			m_lExtra = 0;
			}
	};


#endif