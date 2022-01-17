/*=====================================================================

	AgpdRefinery.h

=====================================================================*/

#ifndef _AGPD_REFINERY_H_
	#define _AGPD_REFINERY_H_

#include "ApBase.h"
#include "AgpdItemTemplate.h"
#include <list>
/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGPMREFINERY_MAX_GRID			10



const enum eAGPMREFINERY_CATEGORY
	{
	AGPMREFINERY_CATEGORY_NONE			= 0,
	AGPMREFINERY_CATEGORY_STONE			= 100,
	AGPMREFINERY_CATEGORY_SKEL			= 200,
	AGPMREFINERY_CATEGORY_KEY			= 300,
	AGPMREFINERY_CATEGORY_ITEM			= 400,
	AGPMREFINERY_CATEGORY_MAX
	};


const enum eAGPMREFINERY_TEMPLATE_EXCEL_COLUMN
	{
	AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_CATEGORY			= 0,
	AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_ITEMTID,
	AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_RESULT_ITEMTID,
	AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_QUANTITY,
	AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_PRICE,
	AGPMREFINERY_TEMPLATE_EXCEL_COLUMN_SUCCESS_PROB,
	};


#define AGPMREFINERY_REFINE_ITEM_MAX_RES					10


const enum eAGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN
{
    AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_MAKEITEM_TOTAL = 0,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_MAKEITEM_ITEMTID,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_MAKEITEM_ITEM_NAME,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_MAKEITEM_ITEM_COUNT,
    AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_BONUS_ITEMTID,
    AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_BOUNS_ITEM_NAME,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_OPTION_MIN,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_OPTION_MAX,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_SOCKET_INCHANT_MIN,
    AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_SOCKET_INCHANT_MAX,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_PRICE,
	AGPMREFINERY_REFINE_ITEM_EXCEL_VALID_TIME,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_TOTAL_RES,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_RES_TID_A,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_RES_NAME_A,
	AGPMREFINERY_REFINE_ITEM_EXCEL_COLUMN_RES_QTY_A,
};

// option by spirit stone
const enum AGPMREFINERY_REFINE_ITEM_OPTION_STONE_EXCEL_COLUMN
	{
	AGPMREFINERY_REFINE_ITEM_OPTION_STONE_EXCEL_COLUMN_ITEM_TID		= 0,
	AGPMREFINERY_REFINE_ITEM_OPTION_STONE_EXCEL_COLUMN_ITEM_NAME,
	AGPMREFINERY_REFINE_ITEM_OPTION_STONE_EXCEL_COLUMN_OPTION_TID,
	AGPMREFINERY_REFINE_ITEM_OPTION_STONE_EXCEL_COLUMN_OPTION_CONT,
	};




/********************************************/
/*		The Definition of RefineTemplate	*/
/********************************************/
//
class AgpdRefineTemplate
	{
	public:
		INT32			m_eCategory;
		INT32			m_lItemTID;
		INT32			m_lResultItemTID;
		INT32			m_lQuantity;
		INT32			m_lPrice;				// 지금은 아니지만 따로 설정하게 되어있다. 정제기 주인이 각 레벨 별로...
		INT32			m_lSuccessProb;
	};


/********************************************/
/*		The Definition of Item Combine		*/
/********************************************/
//
struct AgpdRefineItemRes
{
    INT32	m_lItemTID;
    INT32	m_lQuantity;

    bool operator< (const AgpdRefineItemRes& res) const
    {
        return this->m_lItemTID < res.m_lItemTID;
    }
};

class AgpdRefineItem
	{
	public:
		INT32		m_lID;
		INT32		m_lResultItemTID;
		INT32		m_lOptionGroupID;
		INT32		m_lSocketInchantID;
		INT32		m_lPrice;
		INT32		m_lResourceCount;
		ApSafeArray<AgpdRefineItemRes, AGPMREFINERY_REFINE_ITEM_MAX_RES>	m_Resources;
		
	public:
		AgpdRefineItem()
			{
			Init();
			}
			
		~AgpdRefineItem()		{}
		
		void Init()
			{
			m_lID = 0;
			m_lResultItemTID = 0;
			m_lOptionGroupID = 0;
			m_lSocketInchantID = 0;	
			m_lPrice = 0;
			m_Resources.MemSetAll();
			}
	};


struct AgpdRefineMakeItem
{
    INT32		m_lMakeItemTID;
	INT32		m_IMakeItemCount;
	INT32		m_lValidTime;
    INT32       m_IBonusItemTID;
    INT32		m_lOptionMIN;
    INT32		m_lOptionMAX;
    INT32		m_lSocketInchantMIN;
    INT32		m_lSocketInchantMAX;
};

class AgpdRefineItem2
{
public:
    std::vector<AgpdRefineMakeItem> m_RefineMakeItemVec;
    std::string                     m_RefineKey;
    INT32		                    m_lPrice;

};
#endif
