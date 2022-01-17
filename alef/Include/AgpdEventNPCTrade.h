#ifndef	__AGPD_EVENT_NPCTRADE_H__
#define __AGPD_EVENT_NPCTRADE_H__

#include "ApBase.h"
#include "LinkedList.h"
#include "AgpdItem.h"

class AgpdEventNPCTradeItemGroupList
{
public:
	CDList<INT32 *>	m_csNPCTradeItemList;

	char			m_strGroupName[80];

	AgpdEventNPCTradeItemGroupList()
	{
		memset( m_strGroupName, 0, sizeof(m_strGroupName) );
	}

	BOOL operator == ( AgpdEventNPCTradeItemGroupList &csGroupName )
	{
		BOOL			bResult;

		bResult = FALSE;

		if( !strcmp( m_strGroupName, csGroupName.m_strGroupName ) )
		{
			bResult = TRUE;
		}

		return bResult;
	}
};

class AgpdEventNPCTradeItemListData
{
public:
	INT32			m_lItemTID;
	INT32			m_lItemCount;
};

class AgpdEventNPCTradeTemplate
{
public:
	INT32			m_lNPCTID;
	float			m_fSellFunc;
	float			m_fBuyFunc;
	float			m_fBuyOtherFunc;

	//그룹 리스트
	CDList<AgpdEventNPCTradeItemListData *>	m_csItemList;

	AgpdEventNPCTradeTemplate()
	{
		m_lNPCTID = 0;
	}
};

class AgpdEventNPCTradeData
{
public:
	INT32			m_lNPCTradeTemplateID;

	AgpdGrid		*m_pcsGrid;

	AgpdGridItem	*m_ppcInitGridData[AGPMITEM_NPCTRADEBOX_LAYER * AGPMITEM_NPCTRADEBOX_ROW * AGPMITEM_NPCTRADEBOX_COLUMN];
	INT32			m_alItemID[AGPMITEM_NPCTRADEBOX_LAYER * AGPMITEM_NPCTRADEBOX_ROW * AGPMITEM_NPCTRADEBOX_COLUMN];
	AgpdItem		*m_ppcItemData[AGPMITEM_NPCTRADEBOX_LAYER * AGPMITEM_NPCTRADEBOX_ROW * AGPMITEM_NPCTRADEBOX_COLUMN];
};


#endif