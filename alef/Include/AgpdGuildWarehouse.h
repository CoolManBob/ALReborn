/*============================================================

	AgpdGuildWarehouse.h
	
============================================================*/


#ifndef _AGPD_GUILD_WAREHOUSE_H_
	#define _AGPD_GUILD_WAREHOUSE_H_


#include "ApBase.h"
#include "ApModule.h"
#include "AgpdGrid.h"
#include "AgpdGuild.h"
#include <set>


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const INT32		AGPDGUILDWAREHOUSE_MAX_GRID_LAYER	= 4;
const INT32		AGPDGUILDWAREHOUSE_MAX_GRID_ROW		= 4;
const INT32		AGPDGUILDWAREHOUSE_MAX_GRID_COLUMN	= 7;


const enum eAGPDGUILDWAREHOUSE_PREV
	{
	AGPDGUILDWAREHOUSE_PREV_ITEM_IN		= 0x00000001,
	AGPDGUILDWAREHOUSE_PREV_ITEM_OUT	= 0x00000002,
	AGPDGUILDWAREHOUSE_PREV_MONEY_IN	= 0x00000004,
	AGPDGUILDWAREHOUSE_PREV_MONEY_OUT	= 0x00000008,
	AGPDGUILDWAREHOUSE_PREV_EXPANSION	= 0x00000010,
	};


const enum eAGPDGUILDWAREHOUSE_STATUS
	{
	AGPDGUILDWAREHOUSE_STATUS_NOT_LOADED = 0,
	AGPDGUILDWAREHOUSE_STATUS_WAITING,
	AGPDGUILDWAREHOUSE_STATUS_LOAD_COMPLETED,
	};


typedef std::set<INT32>				OpenedMember;
typedef OpenedMember::iterator		OpenedMemberIter;




/****************************************************/
/*		The Definition of Guild Attached Data		*/
/****************************************************/
//
class AgpdGuildWarehouse : public ApBase
	{
	public :
		AgpdGuild	*m_pAgpdGuild;
		
		eAGPDGUILDWAREHOUSE_STATUS	m_eStatus;
		
		UINT32		m_ulPrevMaster;
		UINT32		m_ulPrevMember;
		
		INT64			m_llMoney;
		INT32			m_lSlot;

		AgpdGrid		m_WarehouseGrid;

		OpenedMember	*m_pOpenedMember;
	
	public :
		void	Reset()
			{
			m_pAgpdGuild = NULL;
			
			m_eStatus = AGPDGUILDWAREHOUSE_STATUS_NOT_LOADED;
		
			m_ulPrevMaster = AGPDGUILDWAREHOUSE_PREV_ITEM_IN |
							 AGPDGUILDWAREHOUSE_PREV_ITEM_OUT |
							 AGPDGUILDWAREHOUSE_PREV_MONEY_IN |
							 AGPDGUILDWAREHOUSE_PREV_MONEY_OUT |
							 AGPDGUILDWAREHOUSE_PREV_EXPANSION;

			m_ulPrevMember = AGPDGUILDWAREHOUSE_PREV_ITEM_IN |
							 AGPDGUILDWAREHOUSE_PREV_MONEY_IN;

			m_llMoney = 0;
			m_lSlot = 0;
			m_pOpenedMember = NULL;
			}
		
		void	SetMasterPreviledge(eAGPDGUILDWAREHOUSE_PREV ePrev, BOOL bSet = TRUE)
			{
			if (bSet)
				m_ulPrevMaster |= ePrev;
			else
				m_ulPrevMaster &= ~ePrev;
			}
		
		void	SetMemberPreviledge(eAGPDGUILDWAREHOUSE_PREV ePrev, BOOL bSet = TRUE)
			{
			if (bSet)
				m_ulPrevMember |= ePrev;
			else
				m_ulPrevMember &= ~ePrev;
			}
			
		BOOL	GetMasterPreviledge(eAGPDGUILDWAREHOUSE_PREV ePrev)
			{
			return (m_ulPrevMaster & ePrev);
			}				

		BOOL	GetMemberPreviledge(eAGPDGUILDWAREHOUSE_PREV ePrev)
			{
			return (m_ulPrevMember & ePrev);
			}							
	};


#endif
