/******************************************************************************
Module:  AgsmTrade.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 15
******************************************************************************/

#if !defined(__AGSMTRADE_H__)
#define __AGSMTRADE_H__

#include "ApBase.h"
#include "AgsModule.h"
#include "AsDefine.h"
#include "ApAdmin.h"
#include "AgsdTrade.h"
#include "AgpdCharacter.h"

#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgsmCharacter.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmTradeD" )
#else
#pragma comment ( lib , "AgsmTrade" )
#endif
#endif


//	define trade step
///////////////////////////////////////////////////////////////////////////////
const int AGSMTRADE_TRADE_START			= 1;
const int AGSMTRADE_INVENTORY_UPDATE	= 2;
const int AGSMTRADE_TRADE_COMPLETE		= 3;
const int AGSMTRADE_TRADE_CANCEL		= 4;

// attached module data index
const int AGSMTRADE_AD_CHARACTER		= 1;

//	AgsmTrade class
///////////////////////////////////////////////////////////////////////////////
class AgsmTrade : public AgsModule {
private:
	AgpmCharacter*	m_pagpmCharacter;
	AgpmItem*		m_pagpmItem;
	AgsmCharacter*	m_pagsmCharacter;

public:
	AgsmTrade();
	~AgsmTrade();

	BOOL OnValid(CHAR* pszData, INT16 nSize);

	BOOL OnAddModule();

	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	BOOL IsAvailableTrade(AgpdCharacter *pCharacter);

	BOOL StartTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2);
	BOOL UpdateTrade(AgpdCharacter *pCharacter, UINT32 ulIID, INT16 nPosInventory, UINT32 ulMoney);
	BOOL CompleteTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2);
	BOOL CancelTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2);

	BOOL CheckInventoryPos(AgsdTrade *pAgsdTrade, INT16 nPosInventory, INT16 nWidth, INT16 nHeight);
	BOOL AddInventory(AgpdCharacter *pCharacter, AgsdTrade *pAgsdTrade, 
					  UINT32 ulIID, INT16 nPosInventory, INT16 nWidth, INT16 nHeight);

	INT16 ProcessTrade(UINT32 ulType, UINT32 ulAction, CHAR* pszData, INT16 nSize);
	INT16 ProcessConnectDeal(UINT32 ulType, UINT32 ulAction, CHAR* pszData, INT16 nSize);

	static VOID CancelTrade(PVOID pData, PVOID pClass);		// 캐릭터가 움직이면 거래가 취소된다
};

#endif //__AGSMTRADE_H__
