/*============================================================================

	AgcmAuction.cpp

============================================================================*/


#include "AgcmAuction.h"


/****************************************************/
/*		The Implementation of AgcmAuction class		*/
/****************************************************/
//
AgcmAuction::AgcmAuction()
	{
	SetModuleName(_T("AgcmAuction"));
	
	m_pApmMap = NULL;
	m_pApmEventManager = NULL;
	m_pAgpmCharacter = NULL;
	m_pAgpmFactors = NULL;
	m_pAgpmItem = NULL;
	m_pAgpmAuction = NULL;
	m_pAgcmConnectManager = NULL;
	m_pAgcmEventManager = NULL;
	m_pAgcmCharacter = NULL;	
	}


AgcmAuction::~AgcmAuction()
	{
	}



//	ApModule inherited
//==============================================
//
BOOL AgcmAuction::OnAddModule()
	{
	m_pApmMap = (ApmMap *) GetModule("ApmMap");
	m_pApmEventManager = (ApmEventManager *) GetModule("ApmEventManager" );
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pAgpmFactors = (AgpmFactors *) GetModule("AgpmFactors");
	m_pAgpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pAgpmAuction = (AgpmAuction *) GetModule("AgpmAuction" );
	m_pAgcmConnectManager = (AgcmConnectManager	*) GetModule( "AgcmConnectManager" );
	m_pAgcmEventManager = (AgcmEventManager *) GetModule( "AgcmEventManager" );
	m_pAgcmCharacter = (AgcmCharacter *) GetModule( "AgcmCharacter" );

	if (!m_pApmMap || !m_pApmEventManager || !m_pAgpmCharacter ||
		!m_pAgpmFactors || !m_pAgpmItem || !m_pAgpmAuction ||
		!m_pAgcmConnectManager || !m_pAgcmEventManager || !m_pAgcmCharacter
		)
		return FALSE;

	//Add Sales
	if (!m_pAgpmAuction->SetCallbackAddSales(CBAdd, this))
		return FALSE;

	//Remove Sales
	if (!m_pAgpmAuction->SetCallbackRemoveSales(CBRemove, this))
		return FALSE;

	//Update Sales
	if (!m_pAgpmAuction->SetCallbackUpdateSales(CBUpdate, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackSelect2(CBSelect, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackOpenAuction(CBAuctionOpen, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackOpenAnywhere(CBOpenAnywhere, this))
		return FALSE;

	if (!m_pAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_AUCTION, CBRequestOpenAuction, this))
		return FALSE;

	return TRUE;
	}



//	Callback setting
//==============================================
//
BOOL AgcmAuction::SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGCMAUCTION_CB_SELECT, pfCallback, pClass);
	}


BOOL AgcmAuction::SetCallbackAuctionUIOpen(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGCMAUCTION_CB_AUCTION_UI_OPEN, pfCallback, pClass);
	}


BOOL AgcmAuction::SetCallbackSalesBox2Add(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGCMAUCTION_CB_SALESBOX2_ADD, pfCallback, pClass);
	}


BOOL AgcmAuction::SetCallbackSalesBox2Remove(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGCMAUCTION_CB_SALESBOX2_REMOVE, pfCallback, pClass);
	}


BOOL AgcmAuction::SetCallbackSalesBox2Update(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGCMAUCTION_CB_SALESBOX2_UPDATE, pfCallback, pClass);
	}


//	Event callback
//========================================
//
BOOL AgcmAuction::CBRequestOpenAuction(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
		
	AgcmAuction		*pThis = (AgcmAuction *) pClass;
	ApdEvent		*pApdEvent = (ApdEvent *) pData;

	if (pThis->m_pAgpmCharacter->IsActionBlockCondition(pThis->m_pAgcmCharacter->GetSelfCharacter()))
		return FALSE;

	if (pApdEvent)
		{
		//서버로 거래 게시판 이벤트를 가진 오브젝트의 정보를 보낸다. 
		pThis->SendOpenAuction(pThis->m_pAgcmCharacter->m_pcsSelfCharacter->m_lID, pApdEvent );
		}

	return TRUE;
	}




//	Operation callback
//========================================
//
BOOL AgcmAuction::CBSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgcmAuction			*pThis = (AgcmAuction *) pClass;
	AgpmAuctionArg		*pAuctionArg = (AgpmAuctionArg*) pData;
	stRowset			stRowData;

	if (pThis->m_pAgpmAuction->ParseRowsetPacket(pAuctionArg->m_pvRowsetPacket, &stRowData))
		{
		return pThis->EnumCallback(AGCMAUCTION_CB_SELECT, &stRowData, pAuctionArg);
		}

	return FALSE;
	}


BOOL AgcmAuction::CBAdd(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmAuction			*pThis = (AgcmAuction *) pClass;
	AgpdAuctionSales	*pAgpdAuctionSales = (AgpdAuctionSales *) pData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->EnumCallback(AGCMAUCTION_CB_SALESBOX2_ADD, pAgpdAuctionSales, pAgpdCharacter);
	}


BOOL AgcmAuction::CBRemove(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmAuction			*pThis = (AgcmAuction *) pClass;
	AgpdAuctionSales	*pAgpdAuctionSales = (AgpdAuctionSales *) pData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->EnumCallback(AGCMAUCTION_CB_SALESBOX2_REMOVE, pAgpdAuctionSales, pAgpdCharacter);
	}


BOOL AgcmAuction::CBUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmAuction		*pThis = (AgcmAuction *) pClass;
	
	return pThis->EnumCallback(AGCMAUCTION_CB_SALESBOX2_UPDATE, pData, pCustData);
	}


BOOL AgcmAuction::CBAuctionOpen(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgcmAuction			*pThis = (AgcmAuction *) pClass;

	pThis->EnumCallback(AGCMAUCTION_CB_AUCTION_UI_OPEN, NULL, NULL);

	return TRUE;
	}


BOOL AgcmAuction::CBOpenAnywhere(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmAuction		*pThis = (AgcmAuction *) pClass;
	INT32			lResult	= *((INT32 *) pCustData);

	if (0 != lResult)
		pThis->EnumCallback(AGCMAUCTION_CB_AUCTION_UI_OPEN, NULL, NULL);

	return TRUE;
	}




//	Packet
//==========================================
//
BOOL AgcmAuction::SendSelect(INT32 lCID, INT32 lItemTID, UINT64 ullDocID, INT16 nFlag, INT32 lCustom)
	{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMAUCTION_OPERATION_SELECT2;
	INT16	nPacketLength = 0;

	PVOID	pvSelectPacket = m_pAgpmAuction->m_csPacketSelect2.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lItemTID,		//ItemTID
													&ullDocID,
													&nFlag,
													&lCustom,
													NULL			//아이템 정보 세부(Rowset).
													);

	PVOID	pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&cOperation,		//lOperation
													&lCID,				//lCID
													pvSelectPacket,		//SelectPacket
													NULL				//Result
												);

	if (pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength);
	
		m_pAgpmAuction->m_csPacketSelect2.FreePacket(pvSelectPacket);
		m_pAgpmAuction->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgcmAuction::SendSell(INT32 lCID, INT32 lItemID, INT16 nQuantity, INT32 lPrice)
	{
	BOOL	bResult = FALSE;
	INT8	lOperation = AGPMAUCTION_OPERATION_SELL;
	INT16	nPacketLength = 0;
	
	PVOID	pvSellPacket = m_pAgpmAuction->m_csPacketSell.MakePacket(FALSE,&nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lItemID,			//ItemID
													&nQuantity,			//lQuantity
													&lPrice,			//lPrice
													NULL				//Date
													);

	PVOID	pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,		//lOperation
													&lCID,				//lCID
													pvSellPacket,		//SellPacket
													NULL				//Result
													);

	if (pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength);

		m_pAgpmAuction->m_csPacketSell.FreePacket(pvSellPacket);
		m_pAgpmAuction->m_csPacket.FreePacket(pvPacket);
		}
	
	return bResult;
	}


BOOL AgcmAuction::SendCancel(INT32 lCID, INT32 lSalesID)
	{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMAUCTION_OPERATION_CANCEL;
	INT16	nPacketLength = 0;
	
	PVOID pvCancelPacket = m_pAgpmAuction->m_csPacketCancel.MakePacket(FALSE,&nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lSalesID,			// sales id
													NULL,				// item tid
													NULL				// quantity
													);

	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&cOperation,		//lOperation
													&lCID,				//lCID
													pvCancelPacket,		//CancelPacket
													NULL				//Result
													);

	if (pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength);

		m_pAgpmAuction->m_csPacketCancel.FreePacket(pvCancelPacket);
		m_pAgpmAuction->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgcmAuction::SendConfirm(INT32 lCID, INT32 lSalesID)
	{
	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMAUCTION_OPERATION_CONFIRM;
	INT16	nPacketLength = 0;

	PVOID pvConfirmPacket = m_pAgpmAuction->m_csPacketConfirm.MakePacket(FALSE,&nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lSalesID,		// sales id
													NULL,			// item tid
													NULL,			// quantity
													NULL			// income
													);

	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&cOperation,		//lOperation
													&lCID,				//lCID
													pvConfirmPacket,	//ConfirmPacket
													NULL				//Result
													);

	if (pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength);

		m_pAgpmAuction->m_csPacketConfirm.FreePacket(pvConfirmPacket);
		m_pAgpmAuction->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgcmAuction::SendBuy(INT32 lCID, UINT64 ullDocID, INT16 nQuantity, INT32 nPrice , INT32 nItemTID)
	{
	BOOL	bResult			= FALSE;
	INT8	cOperation		= AGPMAUCTION_OPERATION_BUY;
	INT16	nPacketLength	= 0;

	PVOID pvBuyPacket = m_pAgpmAuction->m_csPacketBuy.MakePacket(FALSE,&nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&ullDocID,			// doc id
													&nQuantity,			// quantity
													&nItemTID,			// item tid
													&nPrice				// price
													);

	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&cOperation,		//lOperation
													&lCID,				//lCID
													pvBuyPacket,		//BuyPacket
													NULL				//Result
													);

	if (pvPacket)
		{
		bResult = SendPacket( pvPacket, nPacketLength );

		m_pAgpmAuction->m_csPacketBuy.FreePacket(pvBuyPacket);
		m_pAgpmAuction->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgcmAuction::SendOpenAuction(INT32 lCID, ApdEvent *pApdEvent)
	{
	BOOL	bResult = FALSE;
	INT8	lOperation = AGPMAUCTION_OPERATION_EVENT_REQUEST;
	INT16	nPacketLength = 0;

	PVOID	pvPacketBase = m_pAgpmAuction->MakeEventPacket(pApdEvent);
	if (!pvPacketBase)
		return bResult;

	PVOID	pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,		//lOperation
													&lCID,				//lCID
													pvPacketBase,		//
													NULL				//Result
													);

	if (pvPacket)
		{
		bResult = SendPacket( pvPacket, nPacketLength );
		
		m_pAgpmAuction->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgcmAuction::SendOpenAnywhere(INT32 lCID)
	{
	BOOL	bResult = FALSE;
	INT8	lOperation = AGPMAUCTION_OPERATION_OPEN_ANYWHERE;
	INT16	nPacketLength = 0;
	PVOID	pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
													&lOperation,		//lOperation
													&lCID,				//lCID
													NULL,				//
													NULL				//Result
													);

	if (pvPacket)
		{
		bResult = SendPacket( pvPacket, nPacketLength );
		
		m_pAgpmAuction->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}