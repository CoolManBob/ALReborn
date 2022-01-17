#ifndef __AGCD_UI_ITEM_H__
#define __AGCD_UI_ITEM_H__

class AgcdUIItem
{
public:
	BOOL		m_bNotifyDurabilityUnder5Percent;
	BOOL		m_bNotifyDurabilityZero;
};

class AgcdUIItemSalesBox2Data
{
public:
	INT32		m_lItemID;
	INT32		m_lPrice;
	char		m_strDate[255];

	AgcdUIItemSalesBox2Data()
	{
		m_lItemID = 0;
		m_lPrice = 0;
		memset( m_strDate, 0, sizeof(m_strDate) );
	}
};

#endif