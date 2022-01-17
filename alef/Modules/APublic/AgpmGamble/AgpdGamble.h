/*============================================================

	AgpdGamble.h
	
============================================================*/


#ifndef _AGPD_GAMBLE_H_
	#define _AGPD_GAMBLE_H_

#include "ApBase.h"
#include "ApModule.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"


/********************************************/
/*		The Definition of Gamble struct		*/
/********************************************/
//
const INT32			AGPDGAMBLE_MAX_RANK		= 3;
typedef vector<AgpdItemTemplate *>		GambleItemList;
typedef GambleItemList::iterator		GambleItemListIter;



//
//	==========	Gamble	==========
//
struct AgpdGamble
	{
	public :
		INT32				m_lTID;
		AgpdItemTemplate	*m_pItemTemplate;
		CHAR				m_szName[AGPMITEM_MAX_ITEM_NAME + 1];
		INT32				m_lBaseCost;
		ApSafeArray<INT32, AGPDGAMBLE_MAX_RANK>	m_alRank;
		
	public :
		AgpdGamble()
			{
			m_lTID = 0;
			m_pItemTemplate = NULL;
			ZeroMemory(m_szName, sizeof(m_szName));
			m_lBaseCost = 0;
			m_alRank.MemSetAll();
			}
	};




//
//	==========	Gamble Item List	==========
//
class AgpdGambleItemList
	{
	public :
		ApSafeArray<GambleItemList, AGPDGAMBLE_MAX_RANK>		m_apItemTemplateByRank;
	
	public :
		AgpdGambleItemList()	{}
		~AgpdGambleItemList()	{}
	
		BOOL	Add(AgpdItemTemplate *pAgpdItemTemplate, INT32 lRank)
			{
			if (NULL == pAgpdItemTemplate || 0 > lRank || lRank >= AGPDGAMBLE_MAX_RANK)
				return FALSE;
			
			m_apItemTemplateByRank[lRank].push_back(pAgpdItemTemplate);
			return TRUE;
			}
		
		INT32	Count(INT32 lRank)
			{
			if (0 > lRank || lRank >= AGPDGAMBLE_MAX_RANK)
				return 0;
			
			return (INT32) m_apItemTemplateByRank[lRank].size();
			}
	};




//
//	==========	Callback Parameter	==========
//
struct AgpdGambleParam
	{
	public :
		INT32		m_lTID;
		INT32		m_lResult;
	
	public :
		AgpdGambleParam()
			{
			m_lTID		= 0;
			m_lResult	= 0;
			}
	};


#endif
