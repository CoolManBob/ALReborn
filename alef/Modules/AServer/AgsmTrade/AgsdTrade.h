/******************************************************************************
Module:  AgsdTrade.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 15
******************************************************************************/

#if !defined(__AGSDTRADE_H__)
#define __AGSDTRADE_H__

#include "ApBase.h"
#include "AsDefine.h"

class AgsdTrade {
public:
	INT16	nNumItem;
	UINT32	ulTradeInventory[AGSMTRADE_MAX_INVENTORY_COLUMN][AGSMTRADE_MAX_INVENTORY_ROW];
//	UINT32	ulItem[AGSMTRADE_MAX_ITEM];
	ApSafeArray<UINT32, AGSMTRADE_MAX_ITEM>		ulItem;

	UINT32	ulMoney;
};

#endif //__AGSDTRADE_H__