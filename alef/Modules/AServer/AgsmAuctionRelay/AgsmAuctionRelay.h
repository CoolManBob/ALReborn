/*=============================================================

	AgsmAuctionRelay.h

=============================================================*/

#ifndef _AGSM_AUCTION_RELAY_H_
	#define _AGSM_AUCTION_RELAY_H_


#include "AgsEngine.h"
#include "AgpmCharacter.h"
#include "AgsmCharacter.h"
#include "AgpmAuction.h"
#include "AgsmServerManager2.h"
#include "AgsmInterServerLink.h"
#include "AuDatabase.h"
#include "AgsmDatabasePool.h"
#include "AgsdAuctionRelay.h"
#include "AgsaAuctionRelay.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGSMAUCTIONRELAY_CB
	{
	AGSMAUCTIONRELAY_CB_NONE = 0,
	AGSMAUCTIONRELAY_CB_SELL,
	AGSMAUCTIONRELAY_CB_CANCEL,
	AGSMAUCTIONRELAY_CB_CONFIRM,
	AGSMAUCTIONRELAY_CB_BUY,
	AGSMAUCTIONRELAY_CB_SELECT,
	AGSMAUCTIONRELAY_CB_SELECT_SALES,
	AGSMAUCTIONRELAY_CB_BUFFERING_SEND,
	AGSMAUCTIONRELAY_CB_SELECT_SALES_COUNT,
	};


/********************************************************/
/*		The Definition of AgsmAuctionRelay class		*/
/********************************************************/
//
class AgsmAuctionRelay : public AgsModule
	{
	private:
		//	Modules
		AgpmCharacter			*m_pAgpmCharacter;
		AgsmCharacter			*m_pAgsmCharacter;
		AgsmServerManager		*m_pAgsmServerManager;
		AgsmInterServerLink		*m_pAgsmInterServerLink;
		AgsmDatabasePool		*m_pAgsmDatabasePool;

		//	Admin
		AgsaAuctionRelay		m_csAdmin;

		//	Packets
		AuPacket		m_csPacket;
		AuPacket		m_csPacketCancel;
		AuPacket		m_csPacketSell;
		AuPacket		m_csPacketConfirm;
		AuPacket		m_csPacketBuy;
		AuPacket		m_csPacketSelect;
		AuPacket		m_csPacketSelectSales;
		AuPacket		m_csPacketSelectSalesCount;

		// ### TO AgpmAuction
		AuPacket		m_csPacketRowset;

	protected:
		//	Packet Initialization
		void	InitPacket();
		void	InitPacketSell();
		void	InitPacketCancel();
		void	InitPacketConfirm();
		void	InitPacketBuy();
		void	InitPacketSelect();

		//	Packet Processing
		BOOL	OnParamSell(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamCancel(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamConfirm(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamBuy(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamSelect(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamSelectSales(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamSelectSalesCount(INT16 nParam, PVOID pvPacket, UINT32 ulNID);

		//	Packet Send(Internal)
		BOOL	SendSell(AgsdAuctionRelaySell *pAgsdAuctionRelay, UINT32 ulNID);
		BOOL	SendCancel(AgsdAuctionRelayCancel *pAgsdAuctionRelay, UINT32 ulNID);
		BOOL	SendConfirm(AgsdAuctionRelayConfirm *pAgsdAuctionRelay, UINT32 ulNID);		
		BOOL	SendBuy(AgsdAuctionRelayBuy *pAgsdAuctionRelay, UINT32 ulNID);
		BOOL	SendSelect(AgsdAuctionRelaySelect *pAgsdAuctionRelay, UINT32 ulNID);
		BOOL	SendSelectSales(AgsdAuctionRelaySelectSales *pAgsdAuctionRelay, UINT32 ulNID);
		BOOL	SendSelectSalesCount(AgsdAuctionRelaySelectSalesCount *pAgsdAuctionRelay, UINT32 ulNID);

		BOOL	MakeAndSendPacket(PVOID pvPacketEmb, INT16 nParam, UINT32 ulNID);

		//	Callback
		static	BOOL CBOperation(PVOID pData, PVOID pClass, PVOID pCustData);		// After Receive
		static	BOOL CBSellResult(PVOID pData, PVOID pClass, PVOID pCustData);		// after DB operation
		static	BOOL CBCancelResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static	BOOL CBConfirmResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static	BOOL CBBuyResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static	BOOL CBSelectResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static	BOOL CBSelectSalesResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static	BOOL CBSelectSalesCountResult(PVOID pData, PVOID pClass, PVOID pCustData);

	public:
		AgsmAuctionRelay();
		virtual ~AgsmAuctionRelay();

		//	AgsModule inherited
		virtual BOOL OnAddModule();
		virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
		
		//	Packet Send(External)
		BOOL	SendSell(INT32 lCID, UINT64 ullItemSeq, INT32 lPrice, INT16 nQuantity,
						 INT16 nItemStatus, CHAR *pszChar, INT32 lItemTID);
		BOOL	SendCancel(INT32 lCID, INT32 lSalesID, UINT64 ullDocID, CHAR *pszChar);
		BOOL	SendConfirm(INT32 lCID, INT32 lSalesID, UINT64 ullDocID, CHAR *pszChar);
		BOOL	SendBuy(INT32 lCID, UINT64 llDocID, CHAR *pszChar, INT64 llMoney);
		BOOL	SendSelect(INT32 lCID, INT32 lItemTID, UINT64 ullDocID, INT16 nFlag);
		BOOL	SendSelectSales(INT32 lCID, CHAR *pszChar);
		BOOL	SendSelectSalesCount();

		//	Callback Setting
		BOOL	SetCallbackSell(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackCancel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackConfirm(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBuy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSelectSales(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSelectSalesCount(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBufferingSend(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		//	Auction-side callbacks setting method
		BOOL	SetCallbackForAuctionServer();

	protected:
		// #####################
		BOOL	MakeRowsetPakcet(AuRowset *pRowset, PVOID *ppvPakcet);
		BOOL	MakeRowsetPakcet2(AuRowset *pRowset, PVOID *ppvPacket, INT32 lStep, INT32 lRowsPerStep);
	};



#endif