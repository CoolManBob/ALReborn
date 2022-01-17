/*============================================================================

	AgsmAuction.h

============================================================================*/

#ifndef _AGSM_AUCTION_H_
	#define _AGSM_AUCTION_H_

#include "AuGenerateID.h"
#include "ApBase.h"
#include "AgsEngine.h"
#include "AgpmAuction.h"
#include "ApmMap.h"
#include "AgpmGrid.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmServerManager2.h"
#include "AgsmAuctionRelay.h"
#include "AgsmInterServerLink.h"
#include "AuTimeStamp.h"
#include "unordered_map"
#include "AgpmConfig.h"
#include "AgsmSystemMessage.h"


//using namespace stdext;


typedef pair<INT32, INT32> SalesCountPair;
typedef unordered_map<INT32, INT32>::iterator SalesCountIter;


/************************************************/
/*		The Definition of AgsmAuction class		*/
/************************************************/
//
class AgsmAuction : public AgsModule
	{
	private:
		AgpmGrid			*m_pAgpmGrid;
		ApmMap				*m_pApmMap;
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmFactors			*m_pAgpmFactors;
		AgpmItem			*m_pAgpmItem;
		AgpmItemConvert		*m_pAgpmItemConvert;
		AgpmAuction			*m_pAgpmAuction;
		AgsmCharacter		*m_pAgsmCharacter;
		AgsmItem			*m_pAgsmItem;
		AgsmItemManager		*m_pAgsmItemManager;
		AgsmServerManager	*m_pAgsmServerManager;
		AgsmAuctionRelay	*m_pAgsmAuctionRelay;
		AgsmInterServerLink	*m_pAgsmInterServerLink;
		AgpmLog				*m_pAgpmLog;
		AgpmConfig			*m_pAgpmConfig;
		AgsmSystemMessage	*m_pAgsmSystemMessage;

		AuGenerateID			m_GenerateID;
		unordered_map<INT32, INT32>	m_SalesCount;
		ApCriticalSection		m_SalesCountLock;
		INT16					m_nSalesCountReceived;
		BOOL					m_bReady;

	private:
		void GetExpireTime(char* szDest, char* szTime);
		bool CheckValidDate(char* szTime);
		void StrDateToSystemTime(char* szDate, SYSTEMTIME* stTime);
		
	public:
		AgsmAuction();
		~AgsmAuction();

		//	ApModule inherited
		BOOL	OnAddModule();

		//	Request callbacks
		static BOOL	CBSell(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCancel(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBConfirm(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBBuy(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBNotify(PVOID pData, PVOID pClass, PVOID pCustData);
		
		static BOOL	CBOpenAuction(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOpenAnywhere(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Request processing
		BOOL	OnSell(AgpdCharacter *pAgpdCharacter, AgpmAuctionArg *pAuctonArg);
		BOOL	OnCancel(AgpdCharacter *pAgpdCharacter, AgpmAuctionArg *pAuctonArg);
		BOOL	OnConfirm(AgpdCharacter *pAgpdCharacter, AgpmAuctionArg *pAuctonArg);
		BOOL	OnBuy(AgpdCharacter *pAgpdCharacter, AgpmAuctionArg *pAuctonArg);
		
		//	Result callbacks(After auction-server operaton) 
		static BOOL	CBSellResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCancelResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBConfirmResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBBuyResult(PVOID pData, PVOID pClass, PVOID pCustData);
		
		static BOOL	CBSelectResult(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Result processing
		BOOL	OnSellResult(AgsdAuctionRelaySell *pAgsdAuctionRelay, INT16 nParam);
		BOOL	OnCancelResult(AgsdAuctionRelayCancel *pAgsdAuctionRelay, INT16 nParam);
		BOOL	OnConfirmResult(AgsdAuctionRelayConfirm *pAgsdAuctionRelay, INT16 nParam);
		BOOL	OnBuyResult(AgsdAuctionRelayBuy *pAgsdAuctionRelay, INT16 nParam);

		//	sales count callback
		static BOOL CBRelayConnect(PVOID pData, PVOID pClass, PVOID pCustData);				// game -> relay
		static BOOL CBSelectSalesCountResult(PVOID pData, PVOID pClass, PVOID pCustData);	// from relay
		
		//	sales count processing
		BOOL	OnSelectSalesCountResult(AgsdAuctionRelaySelectSalesCount *pAgsdAuctionRelay, INT16 nParam);
		
		//	sales(of given character) related callback
		static BOOL CBLogin(PVOID pData, PVOID pClass, PVOID pCustData);				// login -> game
		static BOOL CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);	// game -> client
		static BOOL	CBSelectSalesResult(PVOID pData, PVOID pClass, PVOID pCustData);	// game (from relay) -> client
		static BOOL CBRequestAllSales(PVOID pData, PVOID pClass, PVOID pCustData);		// for refresh
		static BOOL CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);		// log out
		static BOOL CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData);			// game -> relay

		//	sales(of given character) related processing
		BOOL	OnSelectSalesResult(AgsdAuctionRelaySelectSales *pAgsdAuctionRelay, INT16 nParam);

		//	Packet send
		//	Sales
		BOOL	SendAddSales(AgpdCharacter *pAgpdCharacter, AgpdAuctionSales *pAgpdAuctionSales, UINT32 ulNID);
		BOOL	SendRemoveSales(AgpdCharacter *pAgpdCharacter, INT32 lSalesID);
		BOOL	SendUpdateSales(AgpdCharacter *pAgpdCharacter, AgpdAuctionSales *pAgpdAuctionSales);
		BOOL	SendAllSales(AgpdCharacter *pAgpdCharacter, UINT32 ulNID);
		//	Result
		BOOL	SendSellResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
							   INT32 lItemID, INT16 nQuantity, INT32 lMoney, TCHAR *pszDate);
		BOOL	SendCancelResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
								INT32 lSalesID, INT32 lItemTID, INT16 nQuantity);
		BOOL	SendConfirmResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
								  INT32 lSalesID, INT32 lItemTID, INT16 nQuantity, INT32 lMoney);
		BOOL	SendBuyResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
							  INT32 lItemTID, INT16 nQuantity, INT32 lMoney);
		//	Notify
		BOOL	SendNotify(AgpdCharacter *pAgpdCharacter, INT32 lException,
							INT32 lItemTID, INT16 nQuantity, INT32 lUnitPrice);
		//	Event
		BOOL	SendOpenAuction(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent);
		BOOL	SendOpenAnywhere(AgpdCharacter *pAgpdCharacter, BOOL bAble);
		//	ETC
		BOOL	SendSelectResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
								INT32 lItemTID, UINT64 ullDocID, INT16 nFlag, INT32 lTotal, PVOID pvPacketRowset);
		BOOL	SendCharacterLogin(AgpdCharacter *pAgpdCharacter, UINT32 ulNID);
		
	protected:
		//	Helper methods
		BOOL	IsFullInventory(AgpdCharacter *pcsAgpdCharacter);
		BOOL	CheckActionBlocked(AgpdCharacter *pAgpdCharacter);
		void	SetActionBlock(AgpdCharacter *pAgpdCharacter, BOOL bSet = TRUE);
		UINT32	GetCharacterNID(INT32 lCID);
		UINT32	GetCharacterNID(AgpdCharacter *pAgpdCharacter);
		AgpdAuctionSales* AddSales(UINT64 ullDocID, UINT64 ullItemSeq, INT32 lItemID, INT32 lUnitPrice,
								   INT16 nQuantity, INT16 nStatus, TCHAR *pszDate, INT32 lItemTID,
								   AgpdCharacter *pAgpdCharacter);
		
		//	Log
		void	WriteLog(eAGPDLOGTYPE_ITEM eType, AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lMoney, TCHAR *pszCharID2, UINT64 ulDocID);
		void	WriteLog(eAGPDLOGTYPE_ITEM eType, AgpdCharacter *pAgpdCharacter, UINT64 ullItemSeq, INT32 lItemTID, INT32 lMoney, INT16 nQuantity, UINT64 ulDocID);
	};

#endif
