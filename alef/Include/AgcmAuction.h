/*============================================================================

	AgcmAuction.h

============================================================================*/

#ifndef _AGCM_AUCTION_H_
	#define _AGCM_AUCTION_H_


#include "ApBase.h"
#include "AgcModule.h"
#include "AgpmAuction.h"
#include "ApBase.h"
#include "ApmMap.h"
#include "ApmEventManager.h"
#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpmItem.h"
#include "AgpmAuction.h"
#include "AgcmConnectManager.h"
#include "AgcmEventManager.h"
#include "AgcmCharacter.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum AGCMAUCTION_CB
	{
	AGCMAUCTION_CB_NONE = 0,
	AGCMAUCTION_CB_SELECT,
	AGCMAUCTION_CB_AUCTION_UI_OPEN,
	AGCMAUCTION_CB_SALESBOX2_ADD,
	AGCMAUCTION_CB_SALESBOX2_REMOVE,
	AGCMAUCTION_CB_SALESBOX2_UPDATE,
	};




/************************************************/
/*		The Definition of AgcmAuction class		*/
/************************************************/
//
class AgcmAuction : public AgcModule
	{
	private:
		ApmMap				*m_pApmMap;
		ApmEventManager		*m_pApmEventManager;
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmFactors			*m_pAgpmFactors;
		AgpmItem			*m_pAgpmItem;
		AgpmAuction			*m_pAgpmAuction;
		AgcmConnectManager	*m_pAgcmConnectManager;
		AgcmEventManager	*m_pAgcmEventManager;
		AgcmCharacter		*m_pAgcmCharacter;

	public:
		AgcmAuction();
		virtual ~AgcmAuction();

		//	ApModule inherited
		BOOL	OnAddModule();

		//	Callback setting
		BOOL	SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackAuctionUIOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSalesBox2Add(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSalesBox2Remove(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSalesBox2Update(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		//	Event callback
		static BOOL		CBRequestOpenAuction(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Module Callback
		static BOOL		CBAdd(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBRemove(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBUpdate(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL		CBSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBAuctionOpen(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL		CBOpenAnywhere(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Packet
		BOOL	SendSelect(INT32 lCID, INT32 lItemTID, UINT64 ullDocID, INT16 nFlag, INT32 lCustom = 0);
		BOOL	SendSell(INT32 lCID, INT32 lItemID, INT16 nQuantity, INT32 lPrice);
		BOOL	SendCancel(INT32 lCID, INT32 lSalesID);
		BOOL	SendConfirm(INT32 lCID, INT32 lSalesID);
		BOOL	SendBuy(INT32 lCID, UINT64 ullDocID, INT16 nQuantity , INT32 nPrice , INT32 nItemTID );
		BOOL	SendOpenAuction(INT32 lCID, ApdEvent *pApdEvent);
		BOOL	SendOpenAnywhere(INT32 lCID);
	};


#endif
