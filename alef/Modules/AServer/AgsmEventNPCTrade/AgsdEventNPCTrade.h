//2003.02.26 -Ashulam-

#ifndef __AGSD_EVENT_NPCTRADE_H__
#define __AGSD_EVENT_NPCTRADE_H__

#include "AgpmItem.h"

class AgsdEventNPCTradeAttachData
{
public:
	AgpdGrid			m_csItemGrid;

	INT32				m_lTimeStamp;

	/* ¿Ã∫Œ∫– º’ ∫¡æﬂµ . (Parn)
	AgsdEventNPCTradeAttachData()
	{
		m_csItemGrid.Init( AGPMITEM_NPCTRADEBOX_LAYER, AGPMITEM_NPCTRADEBOX_ROW, AGPMITEM_NPCTRADEBOX_COLUMN );
	}
	~AgsdEventNPCTradeAttachData()
	{
		m_csItemGrid.Remove();
	}
	*/
};

#endif
