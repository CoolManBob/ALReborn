/*====================================================================

	AgsmEventNPCTrade.h
	
====================================================================*/


#ifndef __AGSM_EVENT_NPCTRADE_H__
	#define __AGSM_EVENT_NPCTRADE_H__


#include "AuExcelTxtLib.h"
#include "LinkedList.h"
#include "ApBase.h"
#include "AgsEngine.h"
#include "AgpmCharacter.h"
#include "AgsmCharacter.h"
#include "AgpmItem.h"
#include "AgpmGuild.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsdEventNPCTrade.h"
#include "AgpmEventNPCTrade.h"
#include "ApmEventManager.h"
#include "AgpmGrid.h"

#include "AgpmSystemMessage.h"
#include "AgpmLog.h"

enum eCantBuy
{
	YOU_CAN_BUY = 0,
	CANT_BUY_GENERAL,
	CANT_BUY_NOT_GUILDMASTER,
};


class AgsmEventNPCTrade : public AgsModule  
	{
	private:
		//	Modules
		AgpmCharacter		*m_pcsAgpmCharacter;
		AgpmFactors			*m_pcsAgpmFactors;
		AgpmItem			*m_pcsAgpmItem;
		AgpmItemConvert		*m_pcsAgpmItemConvert;
		AgpmGrid			*m_pcsAgpmGrid;
		AgpmGuild			*m_pcsAgpmGuild;
		AgpmEventNPCTrade	*m_pcsAgpmEventNPCTrade;
		ApmEventManager		*m_pcsApmEventManager;

		AgsmCharacter		*m_pcsAgsmCharacter;
		AgsmItem			*m_pcsAgsmItem;
		AgsmItemManager		*m_pcsAgsmItemManager;

		AgpmSystemMessage	*m_pcsAgpmSystemMessage;
		AgpmLog				*m_pcsAgpmLog;

		AuPacket			m_csPacket;
		AuPacket			m_csDetailInfo;

	public:
		AgsmEventNPCTrade();
		virtual ~AgsmEventNPCTrade();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		//	Operation
		BOOL	ProcessTradeRequest(ApdEvent *pstEvent, AgpdCharacter *pcsAgpdCharacter);
		BOOL	ProcessSellItem(ApdEvent *pstEvent, AgpdCharacter *pcsAgpdCharacter, INT32 lIID, PVOID pvDetailInfo);
		BOOL	ProcessBuyItem(ApdEvent *pstEvent, AgpdCharacter *pcsAgpdCharacter, INT32 lIID, PVOID pvDetailInfo, INT8 cLayer, INT8 cRow, INT8 cColumn);
		BOOL	ProcessItemList(ApdEvent *pstEvent, AgpdCharacter *pcsAgpdCharacter);
		
		BOOL	ProcessOpenAnywhere(AgpdCharacter *pcsAgpdCharacter, CHAR *pszNPCName);
		BOOL	ProcessOpenAnywhere_Sundry(AgpdCharacter *pcsAgpdCharacter);

		//	Event Callback
		static BOOL	CBActionNPCTrade(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Packet
		BOOL	SendBuyResult(AgpdCharacter *pcsAgpdCharacter, INT32 lResult);
		BOOL	SendSellResult(AgpdCharacter *pcsAgpdCharacter, INT32 lResult);
		BOOL	SendItemInfo(AgpdCharacter *pcsAgpdCharacter, ApdEvent *pstEvent, INT32 lIID, INT16 nLayer, INT16 nRow, INT16 nColumn, AgpdGridItem *pcsGridItem);
		BOOL	SendItemInfoEnded(AgpdCharacter *pcsAgpdCharacter);

		BOOL	BuyNormalItem(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lItemCount, INT16 nStatus, INT8 cLayer, INT8 cRow, INT8 cColumn);
		BOOL	SetEventNPCTradeItem();

		//	Log
		BOOL	WriteNPCTradeLog(AgpdCharacter* pcsCharacter, AgpdItem* pcsItem, INT32 lItemCount, INT32 lItemPrice, INT32 lNPCTID, BOOL bSell);

	protected :
		//	Validation
		BOOL	CheckVaildTradeItem(AgpdCharacter *pcsAgpdCharacter, ApdEvent *pcsEvent, INT32 lIID, AgpdGridItem *pcsGridItem, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn);
		BOOL	CheckUsingCashItem(AgpdCharacter* pcsAgpdCharacter);
		
		BOOL AgsmEventNPCTrade::IsNpcHandleItem(AgpdEventNPCTradeData *pcsAgpdEventNPCTradeData, INT32 lTID, float* fBuyFunc);
		eCantBuy CheckBuyable(AgpdCharacter* pcsAgpdCharacter, AgpdItemTemplate* pcsItemTemplate);
	};


#endif