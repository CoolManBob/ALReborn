//2003.02.26 -Ashulam-

#ifndef __AGCM_EVENT_NPCTRADE_H__
#define __AGCM_EVENT_NPCTRADE_H__

#include "ApBase.h"
#include "AgcModule.h"
#include "ApmObject.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmEventNPCTrade.h"
#include "ApmEventManager.h"
#include "AgpmGrid.h"
#include "AgcmCharacter.h"
#include "AgcmEventManager.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventNPCTradeD" )
#else
#pragma comment ( lib , "AgcmEventNPCTrade" )
#endif
#endif

enum
{
	NPCTRADE_CB_ID_UPDATE_NPCTRADE_GRID = 0,
	NPCTRADE_CB_ID_UPDATE_INVENTORY_GRID,
	NPCTRADE_CB_ID_REQUEST_ITEM_LIST,
	NPCTRADE_CB_ID_OPEN_UI_NPCTRADE,
	NPCTRADE_CB_ID_ARISE_NPCTRADE_EVENT,

//여기부터 UI MSG callback
	NPCTRADE_CB_MSG_INVEN_FULL,
	NPCTRADE_CB_MSG_SELL_ITEM,
	NPCTRADE_CB_MSG_BUY_ITEM,
	NPCTRADE_CB_MSG_NOT_ENOUGH_GHELLD,
	NPCTRADE_CB_MSG_NOT_ENOUGH_SPACE_TO_RETURN_GHELLD,
};

class AgcmEventNPCTrade : public AgcModule  
{
private:
	ApmObject			*m_pcsApmObject;
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmItem			*m_pcsAgpmItem;
	ApmEventManager		*m_pcsApmEventManager;
	AgpmGrid			*m_pcsAgpmGrid;
	AgcmCharacter		*m_pcsAgcmCharacter;
	AgcmEventManager	*m_pcsAgcmEventManager;

	AuPacket			m_csPacket;
	AuPacket			m_csDetailInfo;

public:
	INT32				m_lNPCID;
	ApBaseType			m_eNPCType;

	AgcmEventNPCTrade();
	virtual ~AgcmEventNPCTrade();

	BOOL OnAddModule();
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	void SetNPCInfo( INT32 lNPCID, ApBaseType eNPCType );
	void ResetNPCInfo();

	BOOL ProcessItemList(ApdEvent *pstEvent, INT32 lIID, void *pvDetailInfo);
	BOOL ProcessSellItem( INT32 lResult );
	BOOL ProcessBuyItem( INT32 lResult );

	BOOL SendBuy( INT32 lCID, INT32 lIID, INT32 lItemCount, INT16 nStatus = (-1), INT8 lLayer = (-1), INT8 lRow = (-1), INT8 lColumn = (-1) );
	BOOL SendSell( INT32 lCID, INT32 lIID, INT32 lItemCount );
	BOOL SendRequestItemList( INT32 lCID );

	void* MakeBasePacket( INT32 lOBJID, ApBaseType eType, ApdEventFunction eEventFunction );

	//UI Callback
	BOOL SetCallbackUpdateNPCTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUpdateInventoryGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOpenUINPCTrade(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAriseNPCTradeEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	//UI MSG Callback
	BOOL SetCallbackInvenFull(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackBuyItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSellItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackNotEnoughGhelld(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackNotEnoughSpaceToReturnGhelld(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL	CBRequestNPCItemList(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif