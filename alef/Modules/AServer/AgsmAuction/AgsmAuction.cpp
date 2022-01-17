/*=============================================================================

	AgsmAuction.cpp

=============================================================================*/


#include "AgsmAuction.h"


#define AGSMAUCTION_TRACE				TRACE
#define AUCTION_EXPIRE_TIME				6048000000000L	// 7 days


/****************************************************/
/*		The Implementation of AgsmAuction class		*/
/****************************************************/
//
AgsmAuction::AgsmAuction()
	{
	SetModuleName(_T("AgsmAuction"));
	m_SalesCountLock.Init();
	
	m_nSalesCountReceived = 0;
	m_bReady = FALSE;
	}


AgsmAuction::~AgsmAuction()
	{
	m_SalesCountLock.Destroy();
	}




//	ApModule inherited
//============================================
//
BOOL AgsmAuction::OnAddModule()
	{
	m_pAgpmGrid	= (AgpmGrid *) GetModule(_T("AgpmGrid"));
	m_pApmMap = (ApmMap *) GetModule(_T("ApmMap"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmFactors = (AgpmFactors *) GetModule(_T("AgpmFactors"));
	m_pAgpmItem = (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgpmItemConvert = (AgpmItemConvert *) GetModule(_T("AgpmItemConvert"));
	m_pAgpmAuction = (AgpmAuction *)GetModule(_T("AgpmAuction"));
	m_pAgsmCharacter = (AgsmCharacter *)GetModule(_T("AgsmCharacter"));
	m_pAgsmItem = (AgsmItem *) GetModule(_T("AgsmItem"));
	m_pAgsmItemManager = (AgsmItemManager *) GetModule(_T("AgsmItemManager"));
	m_pAgsmServerManager = (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmAuctionRelay = (AgsmAuctionRelay *) GetModule(_T("AgsmAuctionRelay"));
	m_pAgsmInterServerLink = (AgsmInterServerLink *) GetModule(_T("AgsmInterServerLink"));
	m_pAgpmLog = (AgpmLog *) GetModule(_T("AgpmLog"));
	m_pAgpmConfig = (AgpmConfig *) GetModule(_T("AgpmConfig"));
	m_pAgsmSystemMessage = (AgsmSystemMessage *) GetModule(_T("AgsmSystemMessage"));

	if (!m_pApmMap || !m_pAgpmCharacter || !m_pAgpmFactors || !m_pAgpmItem ||
		!m_pAgpmItemConvert || !m_pAgpmAuction || !m_pAgsmCharacter || !m_pAgsmItem ||
		!m_pAgsmItemManager || !m_pAgsmServerManager || !m_pAgsmInterServerLink || !m_pAgpmConfig || !m_pAgsmSystemMessage
		)
		return FALSE;

	if (!m_pAgsmInterServerLink->SetCallbackConnect(CBRelayConnect, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackSell(CBSell, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackCancel(CBCancel, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackConfirm(CBConfirm, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackBuy(CBBuy, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackSelect2(CBSelect, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackNotify(CBNotify, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackOpenAuction(CBOpenAuction, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackOpenAnywhere(CBOpenAnywhere, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackLogin(CBEnterGame, this))
		return FALSE;

	// result callback setting
	if (m_pAgsmAuctionRelay && !m_pAgsmAuctionRelay->SetCallbackSell(CBSellResult, this))
		return FALSE;

	if (m_pAgsmAuctionRelay && !m_pAgsmAuctionRelay->SetCallbackCancel(CBCancelResult, this))
		return FALSE;

	if (m_pAgsmAuctionRelay && !m_pAgsmAuctionRelay->SetCallbackConfirm(CBConfirmResult, this))
		return FALSE;

	if (m_pAgsmAuctionRelay && !m_pAgsmAuctionRelay->SetCallbackBuy(CBBuyResult, this))
		return FALSE;

	if (m_pAgsmAuctionRelay && !m_pAgsmAuctionRelay->SetCallbackSelect(CBSelectResult, this))
		return FALSE;

	if (m_pAgsmAuctionRelay && !m_pAgsmAuctionRelay->SetCallbackSelectSales(CBSelectSalesResult, this))
		return FALSE;

	if (m_pAgsmAuctionRelay && !m_pAgsmAuctionRelay->SetCallbackSelectSalesCount(CBSelectSalesCountResult, this))
		return FALSE;

	// login server to game server
	if (!m_pAgsmCharacter->SetCallbackSendCharacterNewID(CBLogin, this))
		return FALSE;	
	
	// game server to client
	if (!m_pAgsmCharacter->SetCallbackSendCharacterAllInfo(CBSendCharacterAllInfo, this))
		return FALSE;

	if (!m_pAgpmAuction->SetCallbackRequestAllSales(CBRequestAllSales, this))
		return FALSE;

	if (!m_pAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;

	return TRUE;
	}





//	Request callbacks
//===========================================
//
BOOL AgsmAuction::CBSell(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmAuction			*pThis = (AgsmAuction *) pClass;;
	AgpmAuctionArg		*pAuctionArg = (AgpmAuctionArg *) pData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnSell(pAgpdCharacter, pAuctionArg);
	}


BOOL AgsmAuction::CBCancel(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmAuction			*pThis = (AgsmAuction *) pClass;
	AgpmAuctionArg		*pAuctionArg  = (AgpmAuctionArg	*)pData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnCancel(pAgpdCharacter, pAuctionArg);
	}


BOOL AgsmAuction::CBConfirm(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmAuction			*pThis = (AgsmAuction *) pClass;
	AgpmAuctionArg		*pAuctionArg  = (AgpmAuctionArg	*)pData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnConfirm(pAgpdCharacter, pAuctionArg);
	}


BOOL AgsmAuction::CBBuy(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgsmAuction			*pThis = (AgsmAuction *) pClass;
	AgpmAuctionArg		*pAuctionArg = (AgpmAuctionArg *) pData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	return pThis->OnBuy(pAgpdCharacter, pAuctionArg);
	}


BOOL AgsmAuction::CBNotify(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	TRACE(_T("!!! Error : Notify packet received\n"));

	return TRUE;
	}


BOOL AgsmAuction::CBOpenAuction(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
		
	AgsmAuction		*pThis = (AgsmAuction *) pClass;
	ApdEvent		*pApdEvent = (ApdEvent *) pData;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pCustData;

	return pThis->SendOpenAuction(pAgpdCharacter, pApdEvent);
	}


BOOL AgsmAuction::CBOpenAnywhere(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData || !pCustData)
		return FALSE;
		
	AgsmAuction		*pThis = (AgsmAuction *) pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;
	
	BOOL bAble = pThis->m_pAgpmAuction->IsAbleToOpenAnywhere(pAgpdCharacter);
	
	return pThis->SendOpenAnywhere(pAgpdCharacter, bAble);
	}


BOOL AgsmAuction::CBSelect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if( !pClass || !pData || !pCustData)
		return FALSE;

	AgsmAuction			*pThis = (AgsmAuction *) pClass;
	AgpmAuctionArg		*pAuctionArg = (AgpmAuctionArg *) pData;
	AgpdCharacter		*pAgpdCharacter = (AgpdCharacter *) pCustData;
	
	// check whether character exist
	if (0 == pThis->GetCharacterNID(pAuctionArg->m_lCID))
		return FALSE;

	AgpdAuctionCAD *pAgpdAuctionCAD = pThis->m_pAgpmAuction->GetCAD(pAgpdCharacter);
	if (NULL == pAgpdAuctionCAD)
		return FALSE;
	
	AuAutoLock Lock(pAgpdAuctionCAD->m_Mutex);
	if (!Lock.Result()) return FALSE;
	
	if (0 == pAuctionArg->m_lTotalSales
		&& pThis->GetClockCount() < pAgpdAuctionCAD->m_ulClock + 2000)
		return FALSE;

	return pThis->m_pAgsmAuctionRelay->SendSelect(pAgpdCharacter->m_lID, pAuctionArg->m_lItemTID,
												  pAuctionArg->m_ullDocID, pAuctionArg->m_nFlag
												  );
	}




//	Request processing
//=====================================
//
BOOL AgsmAuction::OnSell(AgpdCharacter *pAgpdCharacter, AgpmAuctionArg *pAuctionArg)
{
	if (!pAgpdCharacter || !pAuctionArg)
		return FALSE;

	if (!m_pAgpmConfig->IsEnableAuction())
	{
		m_pAgsmSystemMessage->SendSystemMessage(pAgpdCharacter, AGPMSYSTEMMESSAGE_CODE_DISABLE_AUCTION);
		return FALSE;
	}

	if (FALSE == m_bReady)
	{
		return SendSellResult(pAgpdCharacter, AGPMAUCTION_EXCPT_NOT_AVAILABLE_SERVER,
							  pAuctionArg->m_lItemID, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney, NULL);
	}

	//JK_거래중금지
	if(pAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		return FALSE;


	if (0 >= pAuctionArg->m_lMoney || pAuctionArg->m_lMoney >= 500000000)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%04d/%02d/%02d char=[%s] price=[%d]\n",
				  st.wYear, st.wMonth, st.wDay, pAgpdCharacter->m_szID, pAuctionArg->m_lMoney);
		AuLogFile_s("LOG\\AgsmAuction_Gheld_Limit_Exceed.txt", strCharBuff);
		return FALSE;
	}

	if (TRUE == CheckActionBlocked(pAgpdCharacter))
		return FALSE;

	// check maximum sales
	AgpdAuctionCAD *pAgpdAuctionCAD = m_pAgpmAuction->GetCAD(pAgpdCharacter);
	if (NULL == pAgpdAuctionCAD)
		return FALSE;

	AuAutoLock Lock(pAgpdAuctionCAD->m_Mutex);
	if (!Lock.Result()) return FALSE;

	if (pAgpdAuctionCAD->GetCount() >= AGPMAUCTION_MAX_REGISTER)
	{
		return SendSellResult(pAgpdCharacter, AGPMAUCTION_EXCPT_EXCEED_REGISTRATION_LIMIT,
							  pAuctionArg->m_lItemID, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney, NULL);
	}

	// item check
	AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAuctionArg->m_lItemID);
	AgsdItem *pAgsdItem = m_pAgsmItem->GetADItem(pAgpdItem);
	if (NULL == pAgpdItem || NULL == pAgsdItem || NULL == pAgpdItem->m_pcsItemTemplate)
		return FALSE;

	// item must be in inventory
	if (AGPDITEM_STATUS_INVENTORY != pAgpdItem->m_eStatus)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%04d/%02d/%02d char=[%s] item=[%I64d], status=[%d] in OnSell()\n",
				  st.wYear, st.wMonth, st.wDay, pAgpdCharacter->m_szID, pAgsdItem->m_ullDBIID, pAgpdItem->m_eStatus);
		AuLogFile_s("LOG\\AgsmAuction_Gheld_Invalid_Status.txt", strCharBuff);

		return SendSellResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INVALID_ITEM,
							  pAuctionArg->m_lItemID, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney, NULL);
	}

	// check register fee
	const INT64 llRegisterFee = INT64(pAuctionArg->m_lMoney * AGPMITEM_REGISTER_FEE_RATIO);
	INT64 llInvenMoney = 0;
	m_pAgpmCharacter->GetMoney(pAgpdCharacter, &llInvenMoney);

	if (llInvenMoney < llRegisterFee)
		return FALSE;

	// 2007.12.06. steeple
	// 실 소유주 체크. 복사 방지.
	if (pAgpdItem->m_ulCID != pAgpdCharacter->m_lID)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%04d/%02d/%02d char=[%s] item=[%I64d], status=[%d] in OnSell(), Not real Owner\n",
				  st.wYear, st.wMonth, st.wDay, pAgpdCharacter->m_szID, pAgsdItem->m_ullDBIID, pAgpdItem->m_eStatus);
		AuLogFile_s("LOG\\AgsmAuction_Gheld_Invalid_Status.txt", strCharBuff);

		return SendSellResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INVALID_ITEM,
							  pAuctionArg->m_lItemID, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney, NULL);
	}

	INT16 nQuantity = 1;
	// if stackable
	if (pAgpdItem->m_pcsItemTemplate->m_bStackable)
	{
		nQuantity = pAgpdItem->m_nCount;

		if (0 >= nQuantity)
		{
			return SendSellResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INVALID_ITEM,
								  pAuctionArg->m_lItemID, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney, NULL);
		}
	}

	// quantity check
	if (pAuctionArg->m_nQuantity != nQuantity)
	{
		return SendSellResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INVALID_ITEM,
							  pAuctionArg->m_lItemID, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney, NULL);
	}

	// bound type check
	if (m_pAgpmItem->GetBoundType(pAgpdItem) != E_AGPMITEM_NOT_BOUND)
	{
		return SendSellResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INVALID_ITEM,
							  pAuctionArg->m_lItemID, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney, NULL);
	}

	// add to sales box
	if (FALSE == m_pAgpmItem->AddItemToSalesBox(pAgpdCharacter, pAgpdItem, -1, -1, -1))
	{
		return SendSellResult(pAgpdCharacter, AGPMAUCTION_EXCPT_EXCEED_REGISTRATION_LIMIT,
							  pAuctionArg->m_lItemID, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney, NULL);
	}

	SetActionBlock(pAgpdCharacter);

	// send request to auction server
	INT16 nItemStatus = AGPDITEM_STATUS_SALESBOX_GRID;
	return  m_pAgsmAuctionRelay->SendSell(pAgpdCharacter->m_lID,
										  pAgsdItem->m_ullDBIID,
										  pAuctionArg->m_lMoney,
										  nQuantity,
										  nItemStatus,
										  pAgpdCharacter->m_szID,
										  pAgpdItem->m_lID);
}


BOOL AgsmAuction::OnCancel(AgpdCharacter *pAgpdCharacter, AgpmAuctionArg *pAuctionArg)
	{
	if (!pAgpdCharacter || !pAuctionArg)
		return FALSE;

	if (FALSE == m_bReady)
		{
		return SendCancelResult(pAgpdCharacter, AGPMAUCTION_EXCPT_NOT_AVAILABLE_SERVER, 0, 0, 0);
		}

	if (TRUE == CheckActionBlocked(pAgpdCharacter))
		return FALSE;
	
	//JK_거래중금지
	if(pAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		return FALSE;

	// get sales
	AgpdAuctionSales *pAgpdAuctionSales = m_pAgpmAuction->GetSales(pAuctionArg->m_lSalesID);
	if (!pAgpdAuctionSales)
		return FALSE;

	AgpdAuctionCAD *pAgpdAuctionCAD = m_pAgpmAuction->GetCAD(pAgpdCharacter);
	if (!pAgpdAuctionCAD) 
		return FALSE;

	//AuAutoLock LockSales(pAgpdAuctionSales->m_Mutex);
	AuAutoLock LockCAD(pAgpdAuctionCAD->m_Mutex);
	if (!LockCAD.Result()) return FALSE;

	// if not on sale
	if (AGPMAUCTION_SALES_STATUS_ONSALE != pAgpdAuctionSales->m_nStatus)
		{
		return SendCancelResult(pAgpdCharacter, AGPMAUCTION_EXCPT_UNKNOWN,
								pAuctionArg->m_lSalesID, 0, 0);
		}
	
	// check inventory
	if (IsFullInventory(pAgpdCharacter))
		{
		return SendCancelResult(pAgpdCharacter, AGPMAUCTION_EXCPT_FULL_INVENTORY,
								pAuctionArg->m_lSalesID, 0, 0);
		}

	AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdAuctionSales->m_lItemID);
	if (!pAgpdItem || pAgpdItem->m_pcsCharacter != pAgpdCharacter)
		{
		SYSTEMTIME st;
		GetLocalTime(&st);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%04d/%02d/%02d char=[%s] docid=[%I64d] in OnCancel()\n",
			st.wYear, st.wMonth, st.wDay, pAgpdCharacter->m_szID, pAgpdAuctionSales->m_ullDocID);
		AuLogFile_s("LOG\\AgsmAuction_Item_Not_Found.txt", strCharBuff);

		return SendCancelResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INVALID_ITEM,
								pAuctionArg->m_lSalesID, 0, 0);
		}

	// salesboard에 없는 놈들은 DB update를 하지 않고 걍 뺀다.
	if (0 == pAgpdAuctionSales->m_ullDocID)
		{
		m_pAgpmAuction->RemoveSalesFromCAD(pAgpdCharacter, pAgpdAuctionSales);
		m_pAgpmItem->RemoveItemFromSalesBox(pAgpdCharacter, pAgpdItem);
		m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem, FALSE);
		
		// remove sales
		INT32 lItemTID = pAgpdItem->m_pcsItemTemplate ? ((AgpdItemTemplate *)pAgpdItem->m_pcsItemTemplate)->m_lID : 0;
		INT16 nQuantity = pAgpdItem->m_pcsItemTemplate->m_bStackable ? pAgpdItem->m_nCount : 1;
		m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
		SendRemoveSales(pAgpdCharacter, pAuctionArg->m_lSalesID);

		return SendCancelResult(pAgpdCharacter, AGPMAUCTION_EXCPT_SUCCESS, pAuctionArg->m_lSalesID, lItemTID, nQuantity);
		}

	SetActionBlock(pAgpdCharacter);
	
	// send packet to relay server
	INT16 nStatusFrom = AGPMAUCTION_SALES_STATUS_ONSALE;
	INT16 nStatusTo = AGPMAUCTION_SALES_STATUS_CANCEL;
	return m_pAgsmAuctionRelay->SendCancel(pAgpdCharacter->m_lID,
										   pAuctionArg->m_lSalesID,
										   pAgpdAuctionSales->m_ullDocID,
										   pAgpdCharacter->m_szID
										   );
	}



BOOL AgsmAuction::OnConfirm(AgpdCharacter *pAgpdCharacter, AgpmAuctionArg *pAuctionArg)
	{
	CHAR szLog[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szLog, "LOG\\AgsmAuctionConfirm-%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);
	
	if (!pAgpdCharacter || !pAuctionArg)
		return FALSE;

	if (FALSE == m_bReady)
		{
		return SendConfirmResult(pAgpdCharacter, AGPMAUCTION_EXCPT_NOT_AVAILABLE_SERVER, 0, 0, 0, 0);
		}

	if (TRUE == CheckActionBlocked(pAgpdCharacter))
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%02d:%02d:%02d Character=[%s] Action Blocked\n",
				  st.wHour, st.wMinute, st.wSecond, pAgpdCharacter->m_szID);
		AuLogFile_s(szLog, strCharBuff);
		return FALSE;
		}

	//JK_거래중금지
	if(pAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		return FALSE;

	
	// get sales
	AgpdAuctionSales *pAgpdAuctionSales = m_pAgpmAuction->GetSales(pAuctionArg->m_lSalesID);
	if (!pAgpdAuctionSales)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%02d:%02d:%02d Character=[%s] Can't Get Sales[%d]\n",
				  st.wHour, st.wMinute, st.wSecond, pAgpdCharacter->m_szID, pAuctionArg->m_lSalesID);
		AuLogFile_s(szLog, strCharBuff);
		return FALSE;
		}
	
	AgpdAuctionCAD *pAgpdAuctionCAD = m_pAgpmAuction->GetCAD(pAgpdCharacter);			
	if (!pAgpdAuctionCAD)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%02d:%02d:%02d Character=[%s] Can't Get CAD\n",
				  st.wHour, st.wMinute, st.wSecond, pAgpdCharacter->m_szID);
		AuLogFile_s(szLog, strCharBuff);
		return FALSE;
		}
	
	//AuAutoLock LockSales(pAgpdAuctionSales->m_Mutex);
	AuAutoLock Lock(m_pAgpmAuction->m_Mutex);
	AuAutoLock LockCAD(pAgpdAuctionCAD->m_Mutex);

	if (!Lock.Result() || !LockCAD.Result()) return FALSE;

	// if not completed
	if (AGPMAUCTION_SALES_STATUS_COMPLETE != pAgpdAuctionSales->m_nStatus)
		{
		return SendConfirmResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INVALID_STATUS,
								 pAuctionArg->m_lSalesID, 0, 0, 0);
		}
	
	if (0 != pAgpdAuctionSales->m_ullItemSeq)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%04d/%02d/%02d char=[%s] item=[%I64d], doc=[%I64d]\n",
			st.wYear, st.wMonth, st.wDay, pAgpdCharacter->m_szID, pAgpdAuctionSales->m_ullItemSeq, pAgpdAuctionSales->m_ullDocID);
		AuLogFile_s("LOG\\AgsmAuction_Sequence_Exist_On_Confirm.txt", strCharBuff);
	
		return SendConfirmResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INVALID_STATUS,
								 pAuctionArg->m_lSalesID, 0, 0, 0);		
		}

	SetActionBlock(pAgpdCharacter);
	
	// send packet	
	INT16 nStatusFrom = AGPMAUCTION_SALES_STATUS_COMPLETE;
	INT16 nStatusTo = AGPMAUCTION_SALES_STATUS_CONFIRM_COMPLETE;
	return m_pAgsmAuctionRelay->SendConfirm(pAgpdCharacter->m_lID, 
										   pAuctionArg->m_lSalesID,
										   pAgpdAuctionSales->m_ullDocID,
										   pAgpdCharacter->m_szID
										   );
	}


BOOL AgsmAuction::OnBuy(AgpdCharacter *pAgpdCharacter, AgpmAuctionArg *pAuctionArg)
	{
	if (!pAgpdCharacter || !pAuctionArg)
		return FALSE;

	if (FALSE == m_bReady)
		{
		return SendBuyResult(pAgpdCharacter, AGPMAUCTION_EXCPT_NOT_AVAILABLE_SERVER, 0, 0, 0);
		}

	// check inventory
	if (IsFullInventory(pAgpdCharacter))
		{
		AGSMAUCTION_TRACE(_T("!!! Warning : [%s]inventory full in OnBuy()\n"), pAgpdCharacter->m_szID);
		return SendBuyResult(pAgpdCharacter, AGPMAUCTION_EXCPT_FULL_INVENTORY, 0, 0, 0);
		}

	if(pAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_TRADE)
		return FALSE;

	// 구매 제한 체크(구매 결과가 돌아올때까지 다음 구매 불가)
	if (TRUE == CheckActionBlocked(pAgpdCharacter))
		return FALSE;

	// check money
	INT64 llMoney = 0;
	if(!m_pAgpmCharacter->GetMoney(pAgpdCharacter, &llMoney))
		return FALSE;

	// 현재갖은 Money보다 아이템 값이 더 크면 안된다,
	/*if(pAuctionArg->m_lMoney > llMoney)
	{
		return SendBuyResult(pAgpdCharacter, AGPMAUCTION_EXCPT_INSUFFICIENT_MONEY, 0, 0, 0);	
	}*/		

	SetActionBlock(pAgpdCharacter);
	
	return m_pAgsmAuctionRelay->SendBuy(pAgpdCharacter->m_lID,
										pAuctionArg->m_ullDocID,
										pAgpdCharacter->m_szID,
										llMoney
										);
	}




//	Result callbacks
//===============================================================
//
//	pData : AgsdAuctionRelay, pCustData : eAGSMAUCTIONRELAY_PARAM 
//
BOOL AgsmAuction::CBSellResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmAuction *pThis = (AgsmAuction *) pClass;
	AgsdAuctionRelaySell *pAgsdAuctionRelay = (AgsdAuctionRelaySell *) pData;
	INT16 nParam = (INT16) pCustData;

	return pThis->OnSellResult(pAgsdAuctionRelay, nParam);
	}


BOOL AgsmAuction::CBCancelResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmAuction *pThis = (AgsmAuction *) pClass;
	AgsdAuctionRelayCancel *pAgsdAuctionRelay = (AgsdAuctionRelayCancel *) pData;
	INT16 nParam = (INT16) pCustData;

	return pThis->OnCancelResult(pAgsdAuctionRelay, nParam);
	}


BOOL AgsmAuction::CBConfirmResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmAuction *pThis = (AgsmAuction *) pClass;
	AgsdAuctionRelayConfirm *pAgsdAuctionRelay = (AgsdAuctionRelayConfirm *) pData;
	INT16 nParam = (INT16) pCustData;

	return pThis->OnConfirmResult(pAgsdAuctionRelay, nParam);
	}


BOOL AgsmAuction::CBBuyResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmAuction *pThis = (AgsmAuction *) pClass;
	AgsdAuctionRelayBuy *pAgsdAuctionRelay = (AgsdAuctionRelayBuy *) pData;
	INT16 nParam = (INT16) pCustData;

	return pThis->OnBuyResult(pAgsdAuctionRelay, nParam);
	}


BOOL AgsmAuction::CBSelectResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmAuction *pThis = (AgsmAuction *) pClass;
	AgsdAuctionRelaySelect *pAgsdAuctionRelay = (AgsdAuctionRelaySelect *) pData;
	INT16 nParam = (INT16) pCustData;

	AgpdCharacter *pAgpdCharacter = pThis->m_pAgpmCharacter->GetCharacter(pAgsdAuctionRelay->m_lCID);
	
	INT32 lTotal = 0;
	SalesCountIter Iter = pThis->m_SalesCount.find(pAgsdAuctionRelay->m_lItemTID);
	if (Iter != pThis->m_SalesCount.end())
		lTotal = Iter->second;
	
	return pThis->SendSelectResult(pAgpdCharacter,
								   pAgsdAuctionRelay->m_nCode,
								   pAgsdAuctionRelay->m_lItemTID,
								   pAgsdAuctionRelay->m_ullDocID,
								   pAgsdAuctionRelay->m_nFlag,
								   lTotal,
								   pAgsdAuctionRelay->m_pvPacket
								   );
	}





//	Result processing
//==================================================
//
BOOL AgsmAuction::OnSellResult(AgsdAuctionRelaySell *pAgsdAuctionRelay, INT16 nParam)
{
	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(pAgsdAuctionRelay->m_lCID);
	if (!pAgpdCharacter)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuction::OnSellResult(), pAgpdCharacter is NULL. lCID:%d\n", pAgsdAuctionRelay->m_lCID);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		return FALSE;
		}

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result())
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuction::OnSellResult(), Lock failed\n");
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		return FALSE;
		}

	AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgsdAuctionRelay->m_lItemID);
	if (!pAgpdItem)
	{
		SetActionBlock(pAgpdCharacter, FALSE);

		SYSTEMTIME st;
		GetLocalTime(&st);

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%04d/%02d/%02d char=[%s] in SellResult()\n",
			st.wYear, st.wMonth, st.wDay, pAgpdCharacter->m_szID);
		AuLogFile_s("LOG\\AgsmAuction_Item_Not_Found.txt", strCharBuff);
		return FALSE;
	}

	if (AGPMAUCTION_EXCPT_SUCCESS != pAgsdAuctionRelay->m_nCode)
	{
		AGSMAUCTION_TRACE(_T("!!! Error: Relay returns error[%d] in OnSellResult()\n"), pAgsdAuctionRelay->m_nCode);

		// restore item
		m_pAgpmItem->RemoveItemFromSalesBox(pAgpdCharacter, pAgpdItem);
		m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem);

		// release action block
		SetActionBlock(pAgpdCharacter, FALSE);

		// exception mo onazi packet
		return SendSellResult(pAgpdCharacter, pAgsdAuctionRelay->m_nCode,
							  pAgpdItem->m_lID, pAgsdAuctionRelay->m_nQuantity, pAgsdAuctionRelay->m_lPrice, NULL);
	}

	// substract register fee
	const INT64 llRegisterFee = INT64(pAgsdAuctionRelay->m_lPrice * AGPMITEM_REGISTER_FEE_RATIO);
	m_pAgpmCharacter->SubMoney(pAgpdCharacter, llRegisterFee);


	// create module data and add to CAD
	AgpdAuctionSales *pAgpdAuctionSales = AddSales(pAgsdAuctionRelay->m_ullDocID,
												   pAgsdAuctionRelay->m_ullItemSeq,
												   pAgpdItem->m_lID,
												   pAgsdAuctionRelay->m_lPrice,
												   pAgsdAuctionRelay->m_nQuantity,
												   pAgsdAuctionRelay->m_nStatusTo,
												   pAgsdAuctionRelay->m_szDate,
												   pAgpdItem->m_pcsItemTemplate->m_lID,
												   pAgpdCharacter);
	if (!pAgpdAuctionSales)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuction::OnSellResult(), !pAgpdAuctionSales. DocID:%I64d, ItemSeq:%I64d\n",
									pAgsdAuctionRelay->m_ullDocID, pAgsdAuctionRelay->m_ullItemSeq);
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		return FALSE;
		}

	// send sales to client
	SendAddSales(pAgpdCharacter, pAgpdAuctionSales, GetCharacterNID(pAgpdCharacter));

	// release action block
	SetActionBlock(pAgpdCharacter, FALSE);

	// increase number of sales of item TID
	INT32 lItemTID = pAgpdItem->m_pcsItemTemplate->m_lID;
	m_SalesCountLock.Lock();

	SalesCountIter Iter = m_SalesCount.find(lItemTID);
	if (Iter == m_SalesCount.end())
		m_SalesCount.insert(SalesCountPair(lItemTID, 1));
	else
		Iter->second++;
	
	m_SalesCountLock.Unlock();

	WriteLog(AGPDLOGTYPE_ITEM_BOARD_SELL, pAgpdCharacter, pAgpdItem, pAgsdAuctionRelay->m_lPrice,
			 NULL, pAgsdAuctionRelay->m_ullDocID);

	// send result to client
	return SendSellResult(pAgpdCharacter, pAgsdAuctionRelay->m_nCode,
						  pAgpdItem->m_lID, pAgsdAuctionRelay->m_nQuantity,
						  pAgsdAuctionRelay->m_lPrice, pAgsdAuctionRelay->m_szDate);
}


BOOL AgsmAuction::OnCancelResult(AgsdAuctionRelayCancel *pAgsdAuctionRelay, INT16 nParam)
{
	if (!pAgsdAuctionRelay)
		return FALSE;

	INT32 lSalesID = pAgsdAuctionRelay->m_lSalesID;
	INT32 lItemTID = 0;
	INT16 nQuantity = 0;
	INT32 lPrice = 0;

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(pAgsdAuctionRelay->m_lCID);
	if (!pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;

	if (AGPMAUCTION_EXCPT_SUCCESS != pAgsdAuctionRelay->m_nCode)
	{
		SetActionBlock(pAgpdCharacter, FALSE);
		return SendCancelResult(pAgpdCharacter, pAgsdAuctionRelay->m_nCode, lSalesID, lItemTID, nQuantity);
	}

	// remove sales(from CAD, from Admin)
	AgpdAuctionSales *pAgpdAuctionSales = m_pAgpmAuction->GetSales(lSalesID);
	AgpdAuctionCAD *pAgpdAuctionCAD = m_pAgpmAuction->GetCAD(pAgpdCharacter);

	if (NULL == pAgpdAuctionSales)
	{
		SetActionBlock(pAgpdCharacter, FALSE);
		return FALSE;
	}

	m_pAgpmAuction->RemoveSalesFromCAD(pAgpdCharacter, pAgpdAuctionSales);

	// get item
	AgpdItem *pAgpdItem = m_pAgpmItem->GetItem(pAgpdAuctionSales->m_lItemID);
	if (NULL == pAgpdItem || NULL == pAgpdItem->m_pcsItemTemplate)
	{
		SetActionBlock(pAgpdCharacter, FALSE);
		SYSTEMTIME st;
		GetLocalTime(&st);
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%04d/%02d/%02d char=[%s] item=[%I64d] doc=[%I64d] in CancelResult()\n",
				  st.wYear, st.wMonth, st.wDay, pAgpdCharacter->m_szID, pAgpdAuctionSales->m_ullItemSeq, pAgpdAuctionSales->m_ullDocID);
		AuLogFile_s("LOG\\AgsmAuction_Item_Not_Found.txt", strCharBuff);
		return FALSE;	
	}

	// to inventory
	lItemTID = pAgpdItem->m_pcsItemTemplate->m_lID;
	if (pAgpdItem->m_pcsItemTemplate->m_bStackable)
		nQuantity = pAgpdItem->m_nCount;
	else
		nQuantity = 1;

	lPrice = pAgpdAuctionSales->m_lPrice;

	m_pAgpmItem->RemoveItemFromSalesBox(pAgpdCharacter, pAgpdItem);
	m_pAgpmItem->AddItemToInventory(pAgpdCharacter, pAgpdItem);

	// 2008년 2월 28일 정오 이후에 등록한 아이템만 수수료를 환불한다.
	if (CheckValidDate(pAgpdAuctionSales->m_szDate))
	{
		const INT64 llRegisterFee = INT64(pAgpdAuctionSales->m_lPrice * AGPMITEM_REGISTER_FEE_RATIO);
		m_pAgpmCharacter->AddMoney(pAgpdCharacter, llRegisterFee);
	}

	// remove sales
	m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
	SendRemoveSales(pAgpdCharacter, lSalesID);

	// release action block
	SetActionBlock(pAgpdCharacter, FALSE);

	// decrease number of sales of item TID
	m_SalesCountLock.Lock();
	SalesCountIter Iter = m_SalesCount.find(lItemTID);
	if (Iter != m_SalesCount.end())
	{
		Iter->second--;
		if (0 > Iter->second)
			Iter->second = 0;
	}
	m_SalesCountLock.Unlock();


	WriteLog(AGPDLOGTYPE_ITEM_BOARD_CANCEL, pAgpdCharacter, pAgpdItem, lPrice, NULL, pAgsdAuctionRelay->m_ullDocID);

	return SendCancelResult(pAgpdCharacter, pAgsdAuctionRelay->m_nCode, lSalesID, lItemTID, nQuantity);
}


BOOL AgsmAuction::OnConfirmResult(AgsdAuctionRelayConfirm *pAgsdAuctionRelay, INT16 nParam)
	{
	CHAR szLog[MAX_PATH];
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(szLog, "LOG\\AgsmAuctionConfirmResult-%04d%02d%02d.log", st.wYear, st.wMonth, st.wDay);

	if (!pAgsdAuctionRelay)
		return FALSE;

	INT32 lSalesID = pAgsdAuctionRelay->m_lSalesID;
	INT32 lItemTID = 0;
	INT32 lIncome = 0;
	INT16 nQuantity = 0;

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(pAgsdAuctionRelay->m_lCID);
	if (!pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;

	if (AGPMAUCTION_EXCPT_SUCCESS != pAgsdAuctionRelay->m_nCode)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%02d:%02d:%02d Character=[%s] Exception[%d] Occurred\n",
				  st.wHour, st.wMinute, st.wSecond, pAgpdCharacter->m_szID, pAgsdAuctionRelay->m_nCode);
		AuLogFile_s(szLog, strCharBuff);
		SetActionBlock(pAgpdCharacter, FALSE);
		return SendConfirmResult(pAgpdCharacter, pAgsdAuctionRelay->m_nCode, lSalesID, lItemTID, nQuantity, lIncome);
		}

	// remove sales(from CAD, from Admin)
	AgpdAuctionSales *pAgpdAuctionSales = m_pAgpmAuction->GetSales(lSalesID);
	AgpdAuctionCAD *pAgpdAuctionCAD = m_pAgpmAuction->GetCAD(pAgpdCharacter);

	if (NULL == pAgpdAuctionSales)
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%02d:%02d:%02d Character=[%s] Can't Get Sales[%d]\n",
				  st.wHour, st.wMinute, st.wSecond, pAgpdCharacter->m_szID, lSalesID);
		AuLogFile_s(szLog, strCharBuff);
		SetActionBlock(pAgpdCharacter, FALSE);
		return FALSE;
		}
	
	m_pAgpmAuction->RemoveSalesFromCAD(pAgpdCharacter, pAgpdAuctionSales);

	// give money
	lItemTID = pAgpdAuctionSales->m_lItemTID;
	nQuantity = pAgpdAuctionSales->m_nQuantity;
	lIncome = pAgpdAuctionSales->m_lPrice;
	m_pAgpmCharacter->AddMoney(pAgpdCharacter, lIncome);

	// destroy sales item
	m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
	SendRemoveSales(pAgpdCharacter, lSalesID);
	
	SetActionBlock(pAgpdCharacter, FALSE);

	WriteLog(AGPDLOGTYPE_ITEM_BOARD_CONFIRM, pAgpdCharacter, 0, lItemTID, lIncome, nQuantity, pAgsdAuctionRelay->m_ullDocID);

	// result
	return SendConfirmResult(pAgpdCharacter, pAgsdAuctionRelay->m_nCode, lSalesID, lItemTID, nQuantity, lIncome);
	}


BOOL AgsmAuction::OnBuyResult(AgsdAuctionRelayBuy *pAgsdAuctionRelay, INT16 nParam)
	{
	UINT32 ulNIDBuyer = 0;
	UINT32 ulNIDSeller = 0;
	
	AgpdCharacter *pAgpdCharacterBuyer = m_pAgpmCharacter->GetCharacter(pAgsdAuctionRelay->m_szBuyer);
	if (NULL == pAgpdCharacterBuyer)
		return FALSE;

	AuAutoLock LockBuyer(pAgpdCharacterBuyer->m_Mutex);
	if (!LockBuyer.Result()) return FALSE;

	if (AGPMAUCTION_EXCPT_SUCCESS != pAgsdAuctionRelay->m_nCode)
		{
		SetActionBlock(pAgpdCharacterBuyer, FALSE);
		return SendBuyResult(pAgpdCharacterBuyer, pAgsdAuctionRelay->m_nCode, 0, 0, 0);
		}

	AgpdItem *pAgpdItem = NULL;
	INT32 lMoney = 0;
	AgpdCharacter *pAgpdCharacterSeller = m_pAgpmCharacter->GetCharacterLock(pAgsdAuctionRelay->m_szSeller);

	if (NULL != pAgpdCharacterSeller)
		{
		//pAgpdCharacterSeller->m_Mutex.WLock();
		
		// find item from seller
		pAgpdItem = m_pAgsmItem->GetItemByDBID(pAgpdCharacterSeller, pAgsdAuctionRelay->m_ullItemSeq);
		if (NULL == pAgpdItem)
			{
			SetActionBlock(pAgpdCharacterBuyer, FALSE);
			pAgpdCharacterSeller->m_Mutex.Release();
			return FALSE;
			}

		// check item status
		if (AGPDITEM_STATUS_SALESBOX_GRID != pAgpdItem->m_eStatus
			|| pAgpdItem->m_nCount != pAgsdAuctionRelay->m_nQuantity
			)
			{
			}

		// remove from seller and add to buyer
		m_pAgsmItem->SendPacketItemRemove(pAgpdItem->m_lID, GetCharacterNID(pAgpdCharacterSeller));
		m_pAgpmItem->AddItemToInventory(pAgpdCharacterBuyer, pAgpdItem);
		
		// update sales
		AgpdAuctionSales *pAgpdAuctionSales =
					m_pAgpmAuction->FindSalesFromCAD(pAgpdCharacterSeller, pAgsdAuctionRelay->m_ullDocID);
		if (!pAgpdAuctionSales)
			{
			SetActionBlock(pAgpdCharacterBuyer, FALSE);
			pAgpdCharacterSeller->m_Mutex.Release();
			return FALSE;
			}
		
		//pAgpdAuctionSales->m_Mutex.WLock();
		m_pAgpmAuction->m_Mutex.WLock();
		pAgpdAuctionSales->m_nStatus = AGPMAUCTION_SALES_STATUS_COMPLETE;
		pAgpdAuctionSales->m_ullItemSeq = 0;
		pAgpdAuctionSales->m_lItemID = 0;
		m_pAgpmAuction->m_Mutex.Release();
		//pAgpdAuctionSales->m_Mutex.Release();

		SendUpdateSales(pAgpdCharacterSeller, pAgpdAuctionSales);
		
		pAgpdCharacterSeller->m_Mutex.Release();
		}
	else
		{
		// create item of given itemseq
		pAgpdItem = m_pAgsmItemManager->CreateItem(pAgsdAuctionRelay->m_lItemTID, pAgsdAuctionRelay->m_nQuantity,
										NULL, AGPDITEM_STATUS_NOTSETTING, pAgsdAuctionRelay->m_szConvHistory,
										pAgsdAuctionRelay->m_lDurability, pAgsdAuctionRelay->m_lMaxDurability,
										0 /* flag */, pAgsdAuctionRelay->m_szOption, pAgsdAuctionRelay->m_szSkillPlus,
										pAgsdAuctionRelay->m_lInUse, pAgsdAuctionRelay->m_lUseCount,
										pAgsdAuctionRelay->m_lRemainTime, pAgsdAuctionRelay->m_lExpireDate, pAgsdAuctionRelay->m_llStaminaRemainTime,
										pAgsdAuctionRelay->m_ullItemSeq, pAgpdCharacterBuyer);
		if (NULL == pAgpdItem)
			{
			SetActionBlock(pAgpdCharacterBuyer, FALSE);
			return FALSE;
			}
		
		// add item to buyer's inventory
		m_pAgpmItem->AddItemToInventory(pAgpdCharacterBuyer, pAgpdItem, FALSE);
		
		// send packet to buyer
		m_pAgsmItem->SendPacketItem(pAgpdItem, GetCharacterNID(pAgpdCharacterBuyer));
		}

	// pay
	if (0 >= pAgsdAuctionRelay->m_lPrice)
		pAgsdAuctionRelay->m_lPrice = abs(pAgsdAuctionRelay->m_lPrice);
	lMoney = pAgsdAuctionRelay->m_lPrice;
	m_pAgpmCharacter->SubMoney(pAgpdCharacterBuyer, lMoney);

	// send result to buyer
	SendBuyResult(pAgpdCharacterBuyer , pAgsdAuctionRelay->m_nCode,
				pAgsdAuctionRelay->m_lItemTID, pAgsdAuctionRelay->m_nQuantity, lMoney);


	WriteLog(AGPDLOGTYPE_ITEM_BOARD_BUY, pAgpdCharacterBuyer, pAgpdItem, lMoney, pAgsdAuctionRelay->m_szSeller, pAgsdAuctionRelay->m_ullDocID);

	SetActionBlock(pAgpdCharacterBuyer, FALSE);

	// decrease number of sales of item TID
	if (AGPMAUCTION_EXCPT_SUCCESS == pAgsdAuctionRelay->m_nCode)
		{
		m_SalesCountLock.Lock();
		SalesCountIter Iter = m_SalesCount.find(pAgsdAuctionRelay->m_lItemTID);
		if (Iter != m_SalesCount.end())
			{
			Iter->second--;
			if (0 > Iter->second)
				Iter->second = 0;
			}
		m_SalesCountLock.Unlock();
		}
	
	return TRUE;
	}




//	sales count callback
//=====================================================
//
BOOL AgsmAuction::CBRelayConnect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if(!pData || !pClass)
		return TRUE;

	AgsdServer *pAgsdServer = (AgsdServer *) pData;
	AgsmAuction *pThis = (AgsmAuction *) pClass;

	AgsdServer *pRelayServer = pThis->m_pAgsmServerManager->GetRelayServer();
	if (!pRelayServer)
		return FALSE;

	if (pAgsdServer == pRelayServer)
		{
		pThis->m_bReady = FALSE;
		pThis->m_nSalesCountReceived = 0;
		pThis->m_pAgsmAuctionRelay->SendSelectSalesCount();
		}

	return TRUE;
	}


BOOL AgsmAuction::CBSelectSalesCountResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmAuction *pThis = (AgsmAuction *) pClass;
	AgsdAuctionRelaySelectSalesCount *pAgsdAuctionRelay = (AgsdAuctionRelaySelectSalesCount *) pData;
	INT16 nParam = (INT16) pCustData;

	return pThis->OnSelectSalesCountResult(pAgsdAuctionRelay, nParam);
	}




//	sales count processing
//====================================================
//
BOOL AgsmAuction::OnSelectSalesCountResult(AgsdAuctionRelaySelectSalesCount *pAgsdAuctionRelay, INT16 nParam)
	{
	stRowset Rowset;
	if (!m_pAgpmAuction->ParseRowsetPacket(pAgsdAuctionRelay->m_pvPacket, &Rowset))
		return FALSE;

	INT32	lItemTID = 0;
	LONG	lCount = 0;

	printf("\n[AgsmAuction] Sales count received(%d/%d).\n", m_nSalesCountReceived+1, pAgsdAuctionRelay->m_nTotalPacket);

	m_SalesCountLock.Lock();

	// remove all
	if (0 == m_nSalesCountReceived)
		m_SalesCount.clear();

	for (UINT32 ul = 0; ul < Rowset.m_ulRows; ++ul)
		{
		UINT32 ulCol = 0;
		TCHAR *psz = NULL;
		
		if (NULL == (psz = Rowset.Get(ul, ulCol++)))		// item tid
			continue;

		lItemTID = _ttoi(psz);
		if (0 == lItemTID)
			continue;

		if (NULL == (psz = Rowset.Get(ul, ulCol++)))		// count
			continue;

		lCount = _ttoi(psz);

		m_SalesCount.insert(SalesCountPair(lItemTID, lCount));
		}

	m_nSalesCountReceived++;
	if (m_nSalesCountReceived >= pAgsdAuctionRelay->m_nTotalPacket)
		{
		printf("\n[AgsmAuction] Sales count receive complete. Total no. of item=(%d).\n", m_SalesCount.size());
		m_bReady = TRUE;
		}

	m_SalesCountLock.Unlock();

	return TRUE;
	}




//	sales(of given character) related callback
//===================================================
//
BOOL AgsmAuction::CBLogin(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmAuction		*pThis			= (AgsmAuction *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	// notify character login to game server
	return pThis->SendCharacterLogin(pAgpdCharacter, ulNID);
	}


BOOL AgsmAuction::CBSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmAuction		*pThis			= (AgsmAuction *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= *(UINT32 *)		pCustData;

	return pThis->SendAllSales(pAgpdCharacter, ulNID);
	}


BOOL AgsmAuction::CBSelectSalesResult(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;
	
	AgsmAuction *pThis = (AgsmAuction *) pClass;
	AgsdAuctionRelaySelectSales *pAgsdAuctionRelay = (AgsdAuctionRelaySelectSales *) pData;
	INT16 nParam = (INT16) pCustData;

	return pThis->OnSelectSalesResult(pAgsdAuctionRelay, nParam);
	}


BOOL AgsmAuction::CBRequestAllSales(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmAuction		*pThis			= (AgsmAuction *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulNID			= pThis->GetCharacterNID(pAgpdCharacter);
	
	return pThis->SendAllSales(pAgpdCharacter, ulNID);
	}


BOOL AgsmAuction::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmAuction		*pThis			= (AgsmAuction *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacter"));

	// remove all sales
	// sales item (will/already) removed in AgpmItem module, so we don't care
	pThis->m_pAgpmAuction->RemoveAllSales(pAgpdCharacter, &(pThis->m_GenerateID));
				
	return TRUE;
	}


BOOL AgsmAuction::CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass)
		return FALSE;

	AgsmAuction		*pThis			= (AgsmAuction *)	pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	
	if (!pThis->m_pAgpmCharacter->IsPC(pAgpdCharacter))
		return TRUE;

	// send to relay
	return pThis->m_pAgsmAuctionRelay->SendSelectSales(pAgpdCharacter->m_lID, pAgpdCharacter->m_szID);
	}




//	sales(of given character) related processing
//=====================================================
//
BOOL AgsmAuction::OnSelectSalesResult(AgsdAuctionRelaySelectSales *pAgsdAuctionRelay, INT16 nParam)
	{
	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacter(pAgsdAuctionRelay->m_szChar);
	if (!pAgpdCharacter)
		return FALSE;

	AuAutoLock Lock(pAgpdCharacter->m_Mutex);
	if (!Lock.Result()) return FALSE;

	AgpdAuctionCAD *pAgpdAuctionCAD = m_pAgpmAuction->GetCAD(pAgpdCharacter);
	if (NULL == pAgpdAuctionCAD)
		return FALSE;
	
	pAgpdAuctionCAD->m_ulClock =GetClockCount();

	stRowset Rowset;
	if (!m_pAgpmAuction->ParseRowsetPacket(pAgsdAuctionRelay->m_pvPacket, &Rowset))
		return FALSE;
	
	for (UINT32 ul = 0; ul < Rowset.m_ulRows; ++ul)
		{
		// create sales
		AgpdAuctionSales *pAgpdAuctionSales = m_pAgpmAuction->CreateSales();
		if (!pAgpdAuctionSales)
			return FALSE;

		UINT32 ulCol = 0;
		TCHAR *psz = NULL;

		// salesboard
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))		// doc id
			pAgpdAuctionSales->m_ullDocID = _ttoi64(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))		// itemseq
			pAgpdAuctionSales->m_ullItemSeq = _ttoi64(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))		// price
			pAgpdAuctionSales->m_lPrice = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))		// total quantity
			pAgpdAuctionSales->m_nQuantity = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))		// reg date
			_tcscpy(pAgpdAuctionSales->m_szDate, psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))		// itemtid
			pAgpdAuctionSales->m_lItemTID = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))		// status
			pAgpdAuctionSales->m_nStatus = _ttoi(psz);
		
		// charitem
		UINT64	ullItemSeq = 0;
		INT32	lItemTID = 0;
		INT16	nStackCount = 0;
		TCHAR	*pszPosition = NULL;
		TCHAR	*pszConvert = NULL;
		INT32	lDurability = 0;
		INT32	lMaxDurability = 0;
		INT32	lFlag = 0;
		TCHAR	*pszOption = NULL;
		TCHAR	*pszSkillPlus = NULL;
		INT32	lInUse = 0;
		INT32	lUseCount = 0;
		INT32	lRemainTime = 0;
		INT32	lExpireDate = 0;
		INT64	llStaminaRemainTime = 0;

		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// item seq
			ullItemSeq = _ttoi64(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// item tid
			lItemTID = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// stack count
			nStackCount = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// position
			pszPosition = psz;
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// conv hist
			pszConvert = psz;
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// durability
			lDurability = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// max durability
			lMaxDurability = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// flag
			lFlag = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// option
			pszOption = psz;
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// skill plus
			pszSkillPlus = psz;
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// in use
			lInUse = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// use count
			lUseCount = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// remain time
			lRemainTime = _ttoi(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// expire date
			lExpireDate = AuTimeStamp::ConvertOracleTimeToTimeStamp(psz);
		if (NULL != (psz = Rowset.Get(ul, ulCol++)))	// remain stamina time
			llStaminaRemainTime = _ttoi64(psz);

		AgpdItem *pAgpdItem = NULL;
		
		if (0 != pAgpdAuctionSales->m_ullDocID)
			{
			if (0 == ullItemSeq)
				{
				// no item. already selled(?)
				if (AGPMAUCTION_SALES_STATUS_COMPLETE != pAgpdAuctionSales->m_nStatus)
					{
					m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
					ASSERT(FALSE);
					}
				
				pAgpdAuctionSales->m_lItemID = 0;	// item id
				pAgpdAuctionSales->m_ullItemSeq = 0;
				}
			else
				{
				// create item
				pAgpdItem = m_pAgsmItemManager->CreateItem(lItemTID, nStackCount, pszPosition, AGPDITEM_STATUS_NOTSETTING,
											pszConvert, lDurability, lMaxDurability, lFlag, pszOption, pszSkillPlus,
											lInUse, lUseCount, lRemainTime, lExpireDate, llStaminaRemainTime,
											ullItemSeq, pAgpdCharacter);
				if (NULL == pAgpdItem)
					{
					m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
					continue;
					}
				
				// add to sales box
				m_pAgpmItem->AddItemToSalesBox(pAgpdCharacter, pAgpdItem, pAgpdItem->m_anGridPos[0], pAgpdItem->m_anGridPos[1], pAgpdItem->m_anGridPos[2]);
				m_pAgsmItem->SendPacketItem(pAgpdItem, GetCharacterNID(pAgpdCharacter));
				
				// set sales
				pAgpdAuctionSales->m_lItemID = pAgpdItem->m_lID;		// item id
				}
			
			pAgpdAuctionSales->m_lID = m_GenerateID.GetID();
			}
		else
			{
			 if (0 == ullItemSeq)
				{
				m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
				continue;
				}
				
			// SALESBOARD에는 없고 CHARITEM에 status=8로 있다. 취소할 수 있게 해주자.
			// create item
			pAgpdItem = m_pAgsmItemManager->CreateItem(lItemTID, nStackCount, pszPosition, AGPDITEM_STATUS_NOTSETTING,
							pszConvert, lDurability, lMaxDurability, lFlag, pszOption, pszSkillPlus,
							lInUse, lUseCount, lRemainTime, lExpireDate, llStaminaRemainTime,
							ullItemSeq, pAgpdCharacter);
			if (NULL == pAgpdItem)
					{
					m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
					continue;
					}

			// add to sales box
			m_pAgpmItem->AddItemToSalesBox(pAgpdCharacter, pAgpdItem, pAgpdItem->m_anGridPos[0], pAgpdItem->m_anGridPos[1], pAgpdItem->m_anGridPos[2]);
			m_pAgsmItem->SendPacketItem(pAgpdItem, GetCharacterNID(pAgpdCharacter));
			
			// add dummy sales info.
			pAgpdAuctionSales->m_ullDocID = 0;		// doc id (invalid)
			pAgpdAuctionSales->m_ullItemSeq = 0;	// item seq
			pAgpdAuctionSales->m_lPrice = 0;		// price
			pAgpdAuctionSales->m_nQuantity = pAgpdItem->m_pcsItemTemplate->m_bStackable ? pAgpdItem->m_nCount : 1;
			_tcscpy(pAgpdAuctionSales->m_szDate, _T(""));
			pAgpdAuctionSales->m_nStatus = AGPMAUCTION_SALES_STATUS_ONSALE;
			pAgpdAuctionSales->m_lItemID = pAgpdItem->m_lID;		// item id
			pAgpdAuctionSales->m_lID = m_GenerateID.GetID();
			}

		// add to admin
		if (!m_pAgpmAuction->AddSalesToCAD(pAgpdCharacter, pAgpdAuctionSales))
			{
			INT32 lID = pAgpdAuctionSales->m_lID;
			m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
			m_GenerateID.AddRemoveID(lID);
			return FALSE;		
			}
		}

	return SendAllSales(pAgpdCharacter, GetCharacterNID(pAgpdCharacter));
	}




//	Packet send
//========================================
//
BOOL AgsmAuction::SendAddSales(AgpdCharacter *pAgpdCharacter, AgpdAuctionSales *pAgpdAuctionSales, UINT32 ulNID)
	{
	if (!pAgpdCharacter || !pAgpdAuctionSales)
		return FALSE;

	char szTimeResult[32] = {0, };
	GetExpireTime(szTimeResult, pAgpdAuctionSales->m_szDate);

	INT8 cOperation = AGPMAUCTION_OPERATION_ADD_SALES;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketSales.MakePacket(FALSE, &nPacketLength, 0,
															&pAgpdAuctionSales->m_lID,
															&pAgpdAuctionSales->m_ullDocID,
															0,		// don't send item seq.
															&pAgpdAuctionSales->m_lItemID,
															&pAgpdAuctionSales->m_lPrice,
															&pAgpdAuctionSales->m_nQuantity,
															&pAgpdAuctionSales->m_nStatus,
															szTimeResult,
															&pAgpdAuctionSales->m_lItemTID
															);

	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketEmb,
														NULL
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, ulNID);
	}


BOOL AgsmAuction::SendRemoveSales(AgpdCharacter *pAgpdCharacter, INT32 lSalesID)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_REMOVE_SALES;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketSales.MakePacket(FALSE, &nPacketLength, 0,
															&lSalesID,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL
															);

	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketEmb,
														NULL
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendUpdateSales(AgpdCharacter *pAgpdCharacter, AgpdAuctionSales *pAgpdAuctionSales)
	{
	if (!pAgpdCharacter || !pAgpdAuctionSales)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_UPDATE_SALES;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketSales.MakePacket(FALSE, &nPacketLength, 0,
															&pAgpdAuctionSales->m_lID,
															NULL,
															NULL,
															NULL,
															NULL,
															NULL,
															&pAgpdAuctionSales->m_nStatus,
															NULL,
															NULL
															);

	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														&cOperation,
														&pAgpdCharacter->m_lID,
														pvPacketEmb,
														NULL
														);
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendAllSales(AgpdCharacter *pAgpdCharacter, UINT32 ulNID)
	{
	AgpdAuctionCAD *pAgpdAuctionCAD = m_pAgpmAuction->GetCAD(pAgpdCharacter);
	if (!pAgpdAuctionCAD)
		return FALSE;
	
	// 다 읽지 못하거나 클라이언트가 받을 준비가 안됐는데 보내는 경우를 막기위해
	// CBSendCharacterAllInfo, CBSelectSalesResult 둘 중에 나중에 온경우에만 보내준다.
	if (FALSE == pAgpdAuctionCAD->m_bCheckResult)
		{
		pAgpdAuctionCAD->m_bCheckResult = TRUE;
		return FALSE;
		}
	
	AuAutoLock Lock(pAgpdAuctionCAD->m_Mutex);
	if (!Lock.Result()) return FALSE;

	AgpdAuctionSales *pAgpdAuctionSales = NULL;
	BOOL bResult = TRUE;
	for (INT16 i=0; i<pAgpdAuctionCAD->GetCount(); ++i)
		{
		pAgpdAuctionSales = m_pAgpmAuction->GetSales(pAgpdAuctionCAD->m_Sales[i]);
		if (!SendAddSales(pAgpdCharacter, pAgpdAuctionSales, ulNID))
			bResult = FALSE;
		}
	
	return bResult;
	}


BOOL AgsmAuction::SendSellResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
								 INT32 lItemID, INT16 nQuantity, INT32 lMoney, TCHAR *pszDate)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_SELL;
	INT16 nPacketLength = 0;

	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketSell.MakePacket(FALSE, &nPacketLength, 0,
															  &lItemID,
															  &nQuantity,
															  &lMoney,
															  pszDate
															  );
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;
	
	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   pvPacketEmb,
														   &lException
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendCancelResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
								   INT32 lSalesID, INT32 lItemTID, INT16 nQuantity)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_CANCEL;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketCancel.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
																	&lSalesID,
																	&lItemTID,
																	&nQuantity
																	);

	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   pvPacketEmb,
														   &lException
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendConfirmResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
									INT32 lSalesID, INT32 lItemTID, INT16 nQuantity, INT32 lMoney)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_CONFIRM;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketConfirm.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
																	 &lSalesID,
																	 &lItemTID,
																	 &nQuantity,
																	 &lMoney
																	 );	
	
	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   pvPacketEmb,
														   &lException
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendBuyResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
								INT32 lItemTID, INT16 nQuantity, INT32 lMoney)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_BUY;
	INT16 nPacketLength = 0;	
	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketBuy.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
																 NULL,
																 &nQuantity,
																 &lItemTID,
																 &lMoney
																 );
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;

	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   pvPacketEmb,
														   &lException
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendNotify(AgpdCharacter *pAgpdCharacter, INT32 lException, INT32 lItemTID, INT16 nQuantity, INT32 lPrice)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_NOTIFY;
	INT16 nPacketLength = 0;	
	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketNotify.MakePacket(FALSE, &nPacketLength, 0,
																	&lItemTID,
																	&nQuantity,
																	&lPrice
																	);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;

	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   pvPacketEmb,
														   &lException
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendOpenAuction(AgpdCharacter *pAgpdCharacter, ApdEvent *pApdEvent)
	{
	if (!pAgpdCharacter || !pApdEvent)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_EVENT_GRANT;
	INT16 nPacketLength = 0;	
	
	PVOID pvPacketEvent = m_pAgpmAuction->MakeEventPacket(pApdEvent);

	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   pvPacketEvent,
														   NULL
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendOpenAnywhere(AgpdCharacter *pAgpdCharacter, BOOL bAble)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT32 lResult = bAble ? 1 : 0;

	INT8 cOperation = AGPMAUCTION_OPERATION_OPEN_ANYWHERE;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   NULL,
														   &lResult
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendSelectResult(AgpdCharacter *pAgpdCharacter, INT32 lException,
									INT32 lItemTID, UINT64 ullDocID, INT16 nFlag, INT32 lTotal, PVOID pvPacketRowset)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_SELECT2;
	INT16 nPacketLength = 0;
	PVOID pvPacketEmb = m_pAgpmAuction->m_csPacketSelect2.MakePacket(FALSE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
																	&lItemTID,			// Item TID
																	&ullDocID,
																	&nFlag,
																	&lTotal,
																	pvPacketRowset		// Result Rowset
																	);
	if (!pvPacketEmb || nPacketLength < 1)
		return FALSE;

	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   pvPacketEmb,
														   &lException
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, GetCharacterNID(pAgpdCharacter));
	}


BOOL AgsmAuction::SendCharacterLogin(AgpdCharacter *pAgpdCharacter, UINT32 ulNID)
	{
	if (!pAgpdCharacter)
		return FALSE;

	INT8 cOperation = AGPMAUCTION_OPERATION_LOGIN;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmAuction->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMAUCTION_PACKET_TYPE,
														   &cOperation,
														   &pAgpdCharacter->m_lID,
														   NULL,
														   NULL
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;	

	return SendPacket(pvPacket, nPacketLength, ulNID);
	}




//	Helper methods
//==========================================================
//
BOOL AgsmAuction::CheckActionBlocked(AgpdCharacter *pAgpdCharacter)
	{
	if (NULL == pAgpdCharacter)
		return TRUE;
	
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return TRUE;
	
	if (TRUE == pAgsdCharacter->m_bIsAuctionBlock)
		return TRUE;
	
	return FALSE;
	}
 
 
void AgsmAuction::SetActionBlock(AgpdCharacter *pAgpdCharacter, BOOL bSet)
	{
	if (NULL == pAgpdCharacter)
		return;
	
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return;
	
	pAgsdCharacter->m_bIsAuctionBlock = bSet;
	return;
	}


BOOL AgsmAuction::IsFullInventory(AgpdCharacter *pAgpdCharacter)
	{
	AgpdItemADChar* pAgpdItemADChar = m_pAgpmItem->GetADCharacter(pAgpdCharacter);
	if (!pAgpdItemADChar || m_pAgpmGrid->IsFullGrid(&pAgpdItemADChar->m_csInventoryGrid))
		return TRUE;
	return FALSE;
	}


UINT32 AgsmAuction::GetCharacterNID(INT32 lCID)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(lCID);
	}


UINT32 AgsmAuction::GetCharacterNID(AgpdCharacter *pAgpdCharacter)
	{
	ASSERT(NULL != m_pAgsmCharacter);
	return m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter);
	}


AgpdAuctionSales* AgsmAuction::AddSales(UINT64 ullDocID, UINT64 ullItemSeq, INT32 lItemID, INT32 lPrice,
										INT16 nQuantity, INT16 nStatus, TCHAR *pszDate, INT32 lItemTID,
										AgpdCharacter *pAgpdCharacter)
	{
	AgpdAuctionSales *pAgpdAuctionSales = m_pAgpmAuction->CreateSales();
	if (pAgpdAuctionSales)
		{
		//pAgpdAuctionSales->m_lID = lItemID;
		pAgpdAuctionSales->m_lID = m_GenerateID.GetID();
		
		pAgpdAuctionSales->m_ullDocID = ullDocID;		
		pAgpdAuctionSales->m_ullItemSeq = ullItemSeq;
		pAgpdAuctionSales->m_lItemID = lItemID;
		pAgpdAuctionSales->m_lPrice = lPrice;
		pAgpdAuctionSales->m_nQuantity = nQuantity;
		pAgpdAuctionSales->m_nStatus = nStatus;
		pAgpdAuctionSales->m_lItemTID = lItemTID;
	
		if (pszDate)
			{
			_tcsncpy(pAgpdAuctionSales->m_szDate, pszDate, 32);
			pAgpdAuctionSales->m_szDate[32] = _T('\0');
			}
			
		if (!m_pAgpmAuction->AddSalesToCAD(pAgpdCharacter, pAgpdAuctionSales))
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuction::AddSales(), !m_pAgpmAuction->AddSalesToCAD\n");
			AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
			m_GenerateID.AddRemoveID(pAgpdAuctionSales->m_lID);
			m_pAgpmAuction->DestroySales(pAgpdAuctionSales);
			return NULL;
			}
		}
	else
		{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmAuction::AddSales(), pAgpdAuctionSales is NULL\n");
		AuLogFile_s(RELAY_ERROR_LOG, strCharBuff);
		}

	return pAgpdAuctionSales;
	}




//	Log
//====================================
//
void AgsmAuction::WriteLog(eAGPDLOGTYPE_ITEM eType, AgpdCharacter *pAgpdCharacter, AgpdItem *pAgpdItem, INT32 lMoney, TCHAR *pszCharID2, UINT64 ulDocID)
	{
	if (!m_pAgpmLog || !pAgpdCharacter || !pAgpdItem)
		return;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	AgsdItem *pAgsdItem = m_pAgsmItem->GetADItem(pAgpdItem);
	if (!pAgsdCharacter || !pAgsdItem)
		return;

	AgpdItemConvertADItem *pAgpdItemConvertADItem = m_pAgpmItemConvert->GetADItem(pAgpdItem);

	TCHAR szConvert[AGPDLOG_MAX_ITEM_CONVERT_STRING+1];
	ZeroMemory(szConvert, sizeof(szConvert));
	m_pAgpmItemConvert->EncodeConvertHistory(pAgpdItemConvertADItem, szConvert, AGPDLOG_MAX_ITEM_CONVERT_STRING);

	CHAR szOption[AGPDLOG_MAX_ITEM_FULL_OPTION + 1];
	ZeroMemory(szOption, sizeof(szOption));

	TCHAR szNormalOption[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szNormalOption, sizeof(szNormalOption));
	m_pAgsmItem->EncodingOption(pAgpdItem, szNormalOption, AGPDLOG_MAX_ITEM_OPTION);

	//##########################
	CHAR szSkillPlus[AGPDLOG_MAX_ITEM_OPTION+1];
	ZeroMemory(szSkillPlus, sizeof(szSkillPlus));
	m_pAgsmItem->EncodingSkillPlus(pAgpdItem, szSkillPlus, AGPDLOG_MAX_ITEM_OPTION);	

	sprintf(szOption, "%s::%s", szNormalOption, szSkillPlus);

	CHAR szPosition[33];
	ZeroMemory(szPosition, sizeof(szPosition));
	m_pAgsmCharacter->EncodingPosition(&pAgpdCharacter->m_stPos, szPosition, 32);
	
	TCHAR szDoc[AGPACHARACTER_MAX_ID_STRING+1];
	_i64toa(ulDocID, szDoc, 10);
	
	BOOL bCashItem = FALSE;
	
	m_pAgpmLog->WriteLog_Item(bCashItem,
							eType,
							0,
							&pAgsdCharacter->m_strIPAddress[0],
							pAgsdCharacter->m_szAccountID,
							pAgsdCharacter->m_szServerName,
							pAgpdCharacter->m_szID,
							pAgpdCharacter->m_pcsCharacterTemplate->m_lID,
							m_pAgpmCharacter->GetLevel(pAgpdCharacter),
							m_pAgpmCharacter->GetExp(pAgpdCharacter),
							pAgpdCharacter->m_llMoney,
							pAgpdCharacter->m_llBankMoney,
							pAgsdItem->m_ullDBIID,
							pAgpdItem->m_pcsItemTemplate->m_lID,
							pAgpdItem->m_pcsItemTemplate->m_bStackable ? pAgpdItem->m_nCount : 1,
							szConvert,
							szOption,
							lMoney,
							pszCharID2 ? pszCharID2 : szDoc,
							0,
							0,
							0,
							0,
							0,
							m_pAgpmItem->GetItemDurabilityCurrent(pAgpdItem),
							m_pAgpmItem->GetItemDurabilityMax(pAgpdItem),
							szPosition
							);
	}


void AgsmAuction::WriteLog(eAGPDLOGTYPE_ITEM eType, AgpdCharacter *pAgpdCharacter, UINT64 ullItemSeq, INT32 lItemTID, INT32 lMoney, INT16 nQuantity, UINT64 ulDocID)
	{
	if (!m_pAgpmLog)
		return;
	
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return;
	
	TCHAR szDoc[AGPACHARACTER_MAX_ID_STRING+1];
	_i64toa(ulDocID, szDoc, 10);

	CHAR szPosition[33];
	ZeroMemory(szPosition, sizeof(szPosition));
	m_pAgsmCharacter->EncodingPosition(&pAgpdCharacter->m_stPos, szPosition, 32);
	
	BOOL bCashItem = FALSE;
	
	m_pAgpmLog->WriteLog_Item(bCashItem,
							eType,
							0,
							&pAgsdCharacter->m_strIPAddress[0],
							pAgsdCharacter->m_szAccountID,
							pAgsdCharacter->m_szServerName,
							pAgpdCharacter->m_szID,
							((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID,
							m_pAgpmCharacter->GetLevel(pAgpdCharacter),
							m_pAgpmCharacter->GetExp(pAgpdCharacter),
							pAgpdCharacter->m_llMoney,
							pAgpdCharacter->m_llBankMoney,
							ullItemSeq,
							lItemTID,
							nQuantity,
							NULL,
							NULL,
							lMoney,
							szDoc,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							szPosition
							);
	}

void AgsmAuction::StrDateToSystemTime(char* szTimeString, SYSTEMTIME* stTime)
{
	char szTmp[32] = {0, };

	memcpy(szTmp, szTimeString, 4);
	szTmp[4] = '\0';
	stTime->wYear = (WORD)atoi(szTmp);

	memcpy(szTmp, &szTimeString[5], 2);
	szTmp[2] = '\0';
	stTime->wMonth = (WORD)atoi(szTmp);

	memcpy(szTmp, &szTimeString[8], 2);
	szTmp[2] = '\0';
	stTime->wDay = (WORD)atoi(szTmp);

	memcpy(szTmp, &szTimeString[11], 2);
	szTmp[2] = '\0';
	stTime->wHour = (WORD)atoi(szTmp);

	memcpy(szTmp, &szTimeString[14], 2);
	szTmp[2] = '\0';
	stTime->wMinute = (WORD)atoi(szTmp);

	memcpy(szTmp, &szTimeString[17], 2);
	szTmp[2] = '\0';
	stTime->wSecond = (WORD)atoi(szTmp);
}

void AgsmAuction::GetExpireTime(char* szDest, char* szTimeString)
{
	SYSTEMTIME	st = {0, };
	int expired = 1;

	if(szTimeString && strlen(szTimeString) == 19)
	{
		// create system time
		StrDateToSystemTime(szTimeString, &st);

		// change system time to file tiem
		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);

		// convert file time to ULARGE_INTEGER to calculate.
		ULARGE_INTEGER ultime;
		ultime.LowPart = ft.dwLowDateTime;
		ultime.HighPart = ft.dwHighDateTime;

		ultime.QuadPart += AUCTION_EXPIRE_TIME;

		ft.dwHighDateTime = ultime.HighPart;
		ft.dwLowDateTime = ultime.LowPart;

		// convert file time to system time
		ZeroMemory(&st, sizeof(SYSTEMTIME));
		FileTimeToSystemTime(&ft, &st);

		// expired?
		SYSTEMTIME stNow;
		GetLocalTime(&stNow);

		FILETIME ftNow;
		SystemTimeToFileTime(&stNow, &ftNow);

		expired = CompareFileTime(&ft, &ftNow) > 0 ? 0 : 1;
	}

	sprintf(szDest,
			"%04d-%02d-%02d %02d:%02d:%02d/%d",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, expired);
}

// 2008 2월 27일 정오 이후 날자인지 판단한다.
// 거래소
bool AgsmAuction::CheckValidDate(char* szTime)
{
	SYSTEMTIME st = {0, };
	StrDateToSystemTime(szTime, &st);

	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);

	// 2008-02-27, 12:00:00
	const FILETIME judgementDay = {1194811392, 29915448};

	return (-1 == CompareFileTime(&judgementDay, &ft));
}
