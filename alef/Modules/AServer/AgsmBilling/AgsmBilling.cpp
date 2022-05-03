/*===================================================================

	AgsmBilling.h

===================================================================*/

#include "AuGameEnv.h"
#include "AuTimeStamp.h"
#include "AgsmBilling.h"
#include "AgsmCashMall.h"


#include "AgpmBillInfo.h"//JK_빌링
#include "AgsmBillInfo.h"
#include "AgppBillInfo.h"
#include "AgppCashMall.h"//JK_빌링

#ifdef _AREA_JAPAN_
#pragma message("_AREA_JAPAN_")
#include "AgsmBillingJapan.h"
#elif defined(_AREA_CHINA_)
#pragma message("_AREA_CHINA_")
#pragma message("defined(_AREA_CHINA_)")
#include "AgsmBillingChina.h"
#else
#ifdef _HANGAME_
#include "HannEverBillForSvr.h"
#endif
#ifdef _WEBZEN_BILLING_
#include "AgsmBillingGlobal.h"
#endif
#endif

AgsmBilling* AgsmBilling::m_pInstance = NULL;


// 일본에서 사용하는 주문 번호의 기본형을 만든다.
// AL01_07_08_23_01 과 같은 스트링을 만들어냄(_제외)
void GetOrderSeed( INT32 serverID, char* buffer, int length )
{
	SYSTEMTIME st;
	GetLocalTime( &st );

	_snprintf_s( buffer, length, _TRUNCATE,
				 "AL%02d%02d%02d%02d%02d%02d",
				 serverID%100,	// 일본에서 서버 ID를 2자리만 쓴다고 해서 이렇게 해놨음. 혹 3자리 이상이 나오면 문제가 있어서
				 st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute );
}

/****************************************************/
/*		The Implementation of AgsmBilling class		*/
/****************************************************/
//
BOOL AgsdBillingMoney::Decode(char *pszPacket, INT32 lCode)
	{
	if (NULL == pszPacket || '\0' == *pszPacket)
		return FALSE;
	
	switch (lCode)
		{
		case 99 :
			m_lResult = AGSMBILLING_RESULT_SYSTEM_FAILURE;
			break;
		
		case 0 :
		case 1 :
			if (4 == sscanf(pszPacket, "%I64d|%I64d|%I64d|%I64d|", &m_llMoney,
																   &m_llInternalEventMoney,
																   &m_llExternalEventMoney,
																   &m_llCouponMoney)
				)
				m_lResult = AGSMBILLING_RESULT_SUCCESS;
			break;
	
		default :
			m_lResult = AGSMBILLING_RESULT_FAIL;
			break;
		}
	
	return TRUE;
	}


BOOL AgsdBillingItem::Decode(char *pszPacket, INT32 lCode)
	{
	if (NULL == pszPacket || '\0' == *pszPacket)
		return FALSE;
	
	switch (lCode)
		{
		case 0 :	
			m_lResult = AGSMBILLING_RESULT_SUCCESS;
			// 주문번호.
			m_ullOrderNo = _atoi64(pszPacket);
			break;
		
		case 99 :
			m_lResult = AGSMBILLING_RESULT_SYSTEM_FAILURE;
			break;

		case 4 :
			m_lResult = AGSMBILLING_RESULT_NOT_ENOUGH_MONEY;
			break;
						
		default :		// 다른 응답코드도 있지만 모두 FAIL로 처리한다.
			m_lResult = AGSMBILLING_RESULT_FAIL;
			break;
		}			
	
	return TRUE;
	}




/****************************************************/
/*		The Implementation of AgsmBilling class		*/
/****************************************************/
//
AgsmBilling::AgsmBilling()
	{

	m_pInstance = this;//JK_빌링

	SetModuleName(_T("AgsmBilling"));
	EnableIdle2(TRUE);

	m_pAgsmCharacter			= NULL;
	m_pagpmCharacter			= NULL;//JK_심야샷다운
	m_pAgsmServerManager2		= NULL;
	
	m_pAgpmCashMall				= NULL;
	m_pAgsmCashMall				= NULL;
	m_pAgsmItemManager			= NULL;
	m_pAgpmItem					= NULL;
	m_pAgsmItem					= NULL;

	m_pAgsmBillingChina			= NULL;
	m_pAgsmBillingJapan			= NULL;
	m_pAgsdServerBilling		= NULL;
	m_pAgsmBillingGlobal		= NULL;//JK_빌링
	m_pAgsmBillingWebzen		= NULL;//JK_빌링웹젠

	m_pagpmBillInfo				= NULL;//JK_빌링
	m_pagsmBillInfo				= NULL;
	m_pagpmFactors				= NULL;

	m_pagsmSystemMessage		= NULL; //JK_심야샷다운
	m_pAgpmConfig				= NULL; //JK_심야샷다운


	m_ulLastCheckClockClock	= 0;
	m_ulLastCheckClockForHanGame = 0;
	m_ulLastCheckClockForWebzen = 0;
	m_ulLastCheckClockForMidNightShutDown = 0;//JK_심야샷다운

	InitFuncPtr();
	}

AgsmBilling::~AgsmBilling()
{
#ifdef _AREA_JAPAN_
	if(m_pAgsdServerBilling)
		delete m_pAgsdServerBilling;
	if(m_pAgsmBillingJapan)
		delete m_pAgsmBillingJapan;
#elif defined (_AREA_CHINA_)
	if(m_pAgsmBillingChina)
		delete m_pAgsmBillingChina;
#endif

#ifdef _WEBZEN_BILLING_
	if(m_pAgsmBillingGlobal)
		delete m_pAgsmBillingGlobal;
	if(m_pAgsmBillingWebzen)//JK_빌링웹젠
		delete m_pAgsmBillingWebzen;
#endif
}

// Callback setting method(for result processing)
//==========================================================
//
BOOL AgsmBilling::SetCallbackGetCashMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBILLING_CB_RESULT_GETMONEY, pfCallback, pClass);
}


BOOL AgsmBilling::SetCallbackBuyCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMBILLING_CB_RESULT_BUYITEM, pfCallback, pClass);
}

//	Admin
//======================================================
//
AgsdBilling* AgsmBilling::Get(INT32 lID)
{
	return m_Admin.Get(lID);
}

AgsdBilling* AgsmBilling::Get(CHAR* szAccountID)//JK_빌링
{
	return m_Admin.Get(szAccountID);
}


BOOL AgsmBilling::Add(AgsdBilling *pAgsdBilling)
{
	return m_Admin.Add(pAgsdBilling);
}

BOOL AgsmBilling::Add(AgsdBilling* pAgsdBilling, CHAR* AccountID)//JK_빌링
{
	return m_Admin.Add(pAgsdBilling, AccountID);
}

BOOL AgsmBilling::Remove(INT32 lID)
{

	return m_Admin.Remove(lID);
}

BOOL AgsmBilling::Remove(CHAR* AccountID)//JK_빌링
{

	return m_Admin.Remove(AccountID);
}

void AgsmBilling::InitFuncPtr( void )
{
#ifdef _AREA_CHINA_
	OnAddModulePtr	= &AgsmBilling::OnAddModuleCn;
	OnInitPtr		= &AgsmBilling::OnInitCn;
	OnDisconnectPtr = &AgsmBilling::OnDisconnectCn;

	SendGetCashMoneyPtr		= &AgsmBilling::SendGetCashMoneyCn;
	SendBuyCashItemPtr		= &AgsmBilling::SendBuyCashItemCn;
#elif defined(_AREA_JAPAN_)
	OnAddModulePtr	= &AgsmBilling::OnAddModuleJp;
	OnInitPtr		= &AgsmBilling::OnInitJp;
	OnDisconnectPtr = &AgsmBilling::OnDisconnectJp;

	SendGetCashMoneyPtr		= &AgsmBilling::SendGetCashMoneyJp;
	SendBuyCashItemPtr		= &AgsmBilling::SendBuyCashItemJp;
#else

#ifdef _WEBZEN_BILLING_
	OnAddModulePtr	= &AgsmBilling::OnAddModuleGlobal;
	OnInitPtr		= &AgsmBilling::OnInitGlobal;
	OnDisconnectPtr = NULL; //&AgsmBilling::OnDisconnectKr;

	ConnectBillingServerPtr = &AgsmBilling::ConnectBillingServerGlobal;
	WaitForConnectServerPtr = &AgsmBilling::WaitForConnectServerGlobal;
	BillingSvrDisconnectPtr = NULL; //&AgsmBilling::BillingSvrDisconnectKr;
	SendGetCashMoneyPtr		= &AgsmBilling::SendGetCashMoneyGlobal;
//	SendBuyCashItemPtr		= &AgsmBilling::SendBuyCashItemGlobal;
#else
	//OnAddModulePtr	= &AgsmBilling::OnAddModuleKr;
	//OnInitPtr		= &AgsmBilling::OnInitKr;
	//OnDisconnectPtr = &AgsmBilling::OnDisconnectKr;

	//ConnectBillingServerPtr = &AgsmBilling::ConnectBillingServerKr;
	//SendGetCashMoneyPtr		= &AgsmBilling::SendGetCashMoneyKr;
	//SendBuyCashItemPtr		= &AgsmBilling::SendBuyCashItemKr;
#endif

#endif
}
//	ApModule inherited
//====================================================
#ifdef _AREA_CHINA_
BOOL AgsmBilling::OnIdle2(UINT32 ulClockCount)
{
	if (m_ulLastCheckClockClock + 100000 < ulClockCount)
	{

		if (m_pAgsmBillingChina)
		{
			m_pAgsmBillingChina->CheckConnection();
		}

		m_ulLastCheckClockClock	= ulClockCount;
	}
	return TRUE;
}

BOOL AgsmBilling::OnAddModuleCn()
{
	m_pAgsmCharacter = (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgsmServerManager2 = (AgsmServerManager2 *) GetModule(_T("AgsmServerManager2"));

	if (NULL == m_pAgsmCharacter || NULL == m_pAgsmServerManager2)
		return FALSE;

	m_pAgsmBillingChina = new AgsmBillingChina;
	if (!m_pAgsmBillingChina)
		return FALSE;

	return TRUE;
}
BOOL AgsmBilling::OnInitCn()
{
	m_Admin.SetCount(10000);

	if ( TRUE == m_Admin.InitializeObject(sizeof(AgsdBilling *), m_Admin.GetCount()))
	{
		if (m_pAgsmBillingChina->Init(this))
			return TRUE;

		m_pAgsmBillingChina->WriteErrorLog();
	}

	GetGameEnv().InitEnvironment();
	return GetGameEnv().IsDebugTest();
}

BOOL AgsmBilling::SendGetCashMoneyCn(AgpdCharacter *pAgpdCharacter)
{
	if (NULL == pAgpdCharacter)
	{
		m_pAgsmBillingChina->SomethingWrong("pAgpdCharacter is NULL");
		return FALSE;
	}

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
	{
		m_pAgsmBillingChina->SomethingWrong("pAgsdCharacter is NULL");
		return FALSE;
	}

	char *pszPacket = 0;

	AgsdBillingMoney *pAgsdBilling = new AgsdBillingMoney;

	// add to map
	pAgsdBilling->m_lID = m_GenerateID.GetID();
	pAgsdBilling->m_pAgpdCharacter = pAgpdCharacter;

	if (FALSE == Add(pAgsdBilling))
	{
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		pAgsdBilling->Release();

		m_pAgsmBillingChina->SomethingWrong("pAgsdBilling is NULL");

		return FALSE;
	}

	BOOL bResult = (0 == m_pAgsmBillingChina->SendAccountAuthen(
		pAgsdCharacter->m_szAccountID,
		GetPlayerIPAddress(pAgsdCharacter->m_dpnidCharacter),
		pAgsdBilling));

	if (FALSE == bResult)
	{
		Remove(pAgsdBilling->m_lID);
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		pAgsdBilling->Release();
	}

	return bResult;
}

BOOL AgsmBilling::SendBuyCashItemCn(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList)
{
	if (NULL == pAgpdCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

	AgsdServer *pAgsdServer = m_pAgsmServerManager2->GetThisServer();
	if (NULL == pAgsdServer)
		return FALSE;

	//char *pszPacket = (char *) m_PacketMemoryPool.Alloc();
	//if (NULL == pszPacket)
	//	return FALSE;

	AgsdBillingItem *pAgsdBilling = new AgsdBillingItem;

	// add to map
	pAgsdBilling->m_lID = m_GenerateID.GetID();
	pAgsdBilling->m_pAgpdCharacter = pAgpdCharacter;
	pAgsdBilling->m_lProductID = lProductID;
	pAgsdBilling->m_llBuyMoney = llMoney;
	pAgsdBilling->m_ullListSeq = sList.m_ullBuyID;

	if (FALSE == Add(pAgsdBilling))
	{
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		pAgsdBilling->Release();
		return FALSE;
	}

	BOOL bResult = (0 == m_pAgsmBillingChina->SendAccountLock(
		pAgsdCharacter->m_szAccountID,
		GetPlayerIPAddress(pAgsdCharacter->m_dpnidCharacter),
		lProductID,
		(int)llMoney,
		pAgsdBilling));

	//m_PacketMemoryPool.Free(pszPacket);

	if (FALSE == bResult)
	{
		Remove(pAgsdBilling->m_lID);
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		pAgsdBilling->Release();
	}

	return bResult;
}

#elif defined(_AREA_JAPAN_)
BOOL AgsmBilling::SendGetCashMoneyJp(AgpdCharacter *pAgpdCharacter)
{

	if (NULL == pAgpdCharacter ||
		NULL == m_pAgsdServerBilling ||
		FALSE == m_pAgsdServerBilling->m_bIsConnected )
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

	int balance = 0;

	const char* szBillNo = m_pAgsmBillingJapan->GetBillingNumber( pAgsdCharacter->m_szAccountID );
	if(szBillNo)
		strncpy( pAgsdCharacter->m_szMemberBillingNum , szBillNo, sizeof(szBillNo) );

	bool result = m_pAgsmBillingJapan->GetBalance( pAgsdCharacter->m_szAccountID, pAgsdCharacter->m_szMemberBillingNum, balance );

	return result ? m_pAgsmCharacter->SetCash( pAgpdCharacter, balance ) : FALSE;

}
void AgsmBilling::SetOrderSeed()
{
	char orderSeed[15] = {0, };
	GetOrderSeed( m_pAgsmServerManager2->GetServerID(), orderSeed, 15 );
	m_pAgsmBillingJapan->SetOrderSeed( orderSeed );
}

BOOL AgsmBilling::OnAddModuleJp()
{
	m_pAgsmCharacter	  = (AgsmCharacter *)	   GetModule(_T("AgsmCharacter"));
	m_pAgsmServerManager2 = (AgsmServerManager2 *) GetModule(_T("AgsmServerManager2"));

	if (NULL == m_pAgsmCharacter || NULL == m_pAgsmServerManager2)
		return FALSE;
	m_pAgsmBillingJapan = new AgsmBillingJapan;
	if ( NULL == m_pAgsmBillingJapan )
		return FALSE;
	return TRUE;
}

BOOL AgsmBilling::OnInitJp()
{
	// AddModule 순서 때문에 여기서 따로 모듈을 가져 온다.
	m_pAgpmCashMall		= (AgpmCashMall*)GetModule("AgpmCashMall");
	m_pAgsmCashMall		= (AgsmCashMall*)GetModule("AgsmCashMall");
	m_pAgsmItemManager	= (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pAgpmItem			= (AgpmItem*)GetModule("AgpmItem");
	m_pAgsmItem			= (AgsmItem*)GetModule("AgsmItem");

	if ( !m_pAgpmCashMall || !m_pAgsmCashMall || !m_pAgsmItemManager || !m_pAgpmItem || !m_pAgsmItem )
		return FALSE;

	m_pAgsdServerBilling = new AgsdServer;
	m_pAgsdServerBilling->m_bIsConnected = TRUE;

	return m_pAgsmBillingJapan->InitBillingModule();
}

BOOL AgsmBilling::SendBuyCashItemJp(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList)
{
	if (NULL == pAgpdCharacter ||
		NULL == m_pAgsdServerBilling ||
		FALSE == m_pAgsdServerBilling->m_bIsConnected)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

	AgpmCashItemInfo* pCashItemInfo = m_pAgpmCashMall->GetCashItem( lProductID );
	if ( NULL == pCashItemInfo )
		return FALSE;

	char itemCode[13] = {0, };
	_snprintf_s( itemCode, 13, _TRUNCATE, "AL%d", pCashItemInfo->m_lProductID);

	AgpdItemTemplate* pItemTemplate = m_pAgpmItem->GetItemTemplate( pCashItemInfo->m_alItemTID[0] );
	if ( NULL == pItemTemplate )
		return FALSE;

	char itemName[128] = {0, };
	if (pCashItemInfo->m_alItemQty[0] == 1)
		strncpy_s(itemName, 128, pItemTemplate->m_szName, _TRUNCATE);
	else
		_snprintf_s(itemName, 128, _TRUNCATE, "%s(%dset)", pItemTemplate->m_szName, pCashItemInfo->m_alItemQty[0]);

	// 일본 아이템 구매 쿼리
	char orderString[21] = {0, };
	if ( false == m_pAgsmBillingJapan->BuyItem( pAgsdCharacter->m_szAccountID,
		pAgsdCharacter->m_szMemberBillingNum,
		itemCode,
		itemName,
		(int)llMoney,
		GetPlayerIPAddress(pAgsdCharacter->m_dpnidCharacter),
		orderString) )
	{
		m_pAgsmCashMall->SendBuyResult(pAgpdCharacter, AGPMCASH_BUY_RESULT_FAIL);
		return FALSE;
	}

	strncpy_s(sList.m_szOrderID, orderString,sizeof(orderString));

	vector<AgpdItem*> apcsItems;
	apcsItems.reserve(AGPMCASHMALL_MAX_ITEM_TID);

	for (INT32 i = 0; i < pCashItemInfo->m_lNumTotalItems; ++i)
	{
		// 아이템을 생성하자
		AgpdItem *pcsItem = m_pAgsmItemManager->CreateItem(pCashItemInfo->m_alItemTID[i], pAgpdCharacter, pCashItemInfo->m_alItemQty[i]);
		if (!pcsItem)
		{
			if ( 0 == i )
			{
				m_pAgsmCashMall->SendBuyResult(pAgpdCharacter, AGPMCASH_BUY_RESULT_FAIL);
				return FALSE;
			}

			continue;
		}

		// insert item into cash inventory
		if (!m_pAgpmItem->AddItemToCashInventory(pAgpdCharacter, pcsItem))
		{
			if ( 0 == i )
			{
				m_pAgpmItem->RemoveItem(pcsItem, TRUE);
				m_pAgsmCashMall->SendBuyResult(pAgpdCharacter, AGPMCASH_BUY_RESULT_FAIL);
				return FALSE;
			}

			continue;
		}

		apcsItems.push_back(pcsItem);
	}

	const UINT16 itemCount = (UINT16)apcsItems.size();
	if (!m_pAgsmCharacter->SubCash(pAgpdCharacter, llMoney))
	{
		for (UINT16 i = 0; i < itemCount; ++i )
			m_pAgpmItem->RemoveItem(apcsItems[i], TRUE);

		m_pAgsmCashMall->SendBuyResult(pAgpdCharacter, AGPMCASH_BUY_RESULT_FAIL);
		return FALSE;
	}

	AgsdItem *pcsAgsdItem = m_pAgsmItem->GetADItem(apcsItems[0]);

	m_pAgsmItem->UpdateCashItemBuyList_Complete2(pcsAgsdItem->m_ullDBIID, sList.m_szOrderID, 1);

	for (UINT16 i = 0; i < itemCount; ++i)
		m_pAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_BUY, pAgpdCharacter->m_lID, apcsItems[i], apcsItems[i]->m_nCount, (INT32)pCashItemInfo->m_llPrice);

	m_pAgsmCashMall->SendBuyResult(pAgpdCharacter, AGPMCASH_BUY_RESULT_SUCCESS);

	return TRUE;
}
#else //_AREA_KOREA_ 외 기타

#ifdef _HANGAME_//JK_빌링
////////////////////////////////////////////////////////////////
// 2008.01.11. steeple
// 한게임 빌링 바꼈다.
// 2007.11.21. steeple
// 여기서 부터 한게임 빌링 작업 시작

#define ARCHLORD_GAME_ID			"K_ARCHLORD"
#define ARHCLORD_TEST_GAME_ID		"K_ARCHTEST"
#define BILLING_ERROR_LOG_FILE_NAME	"LOG\\BillError.log"

eAGSMBILLING_CONNECT_RESULT AgsmBilling::ConnectBillingServerKr()
{
#ifdef _HANGAME_
	GetGameEnv().InitEnvironment();

	if (ConnectHanGameBilling())
		return AGSMBILLING_CONNECT_RESULT_SUCCESS;
	else
		return AGSMBILLING_CONNECT_RESULT_FAIL;
#endif

	return AGSMBILLING_CONNECT_RESULT_SUCCESS;
}

BOOL AgsmBilling::OnAddModuleKr()
{
	m_pAgsmCharacter = (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgsmServerManager2 = (AgsmServerManager2 *) GetModule(_T("AgsmServerManager2"));

	if (NULL == m_pAgsmCharacter || NULL == m_pAgsmServerManager2)
		return FALSE;

	if (FALSE == m_pAgsmServerManager2->SetCallbackAddServer(CBAddServer, this))
		return FALSE;

	return TRUE;
}
BOOL AgsmBilling::OnInitKr()
{
	// AddModule 순서 때문에 여기서 따로 모듈을 가져 온다.
	m_pAgpmCashMall		= (AgpmCashMall*)GetModule("AgpmCashMall");
	m_pAgsmCashMall		= (AgsmCashMall*)GetModule("AgsmCashMall");
	m_pAgsmItemManager	= (AgsmItemManager*)GetModule("AgsmItemManager");
	m_pAgpmItem			= (AgpmItem*)GetModule("AgpmItem");
	m_pAgsmItem			= (AgsmItem*)GetModule("AgsmItem");

	if ( !m_pAgpmCashMall || !m_pAgsmCashMall || !m_pAgsmItemManager || !m_pAgpmItem || !m_pAgsmItem )
		return FALSE;

	m_pAgsdServerBilling = new AgsdServer;
	m_Admin.SetCount(10000);

	if (FALSE == m_Admin.InitializeObject(sizeof(AgsdBilling *), m_Admin.GetCount()))
		return FALSE;

	m_PacketMemoryPool.Initialize(1024, 256, _T("AgsmBilling::PaketMemoryPool"));

	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (NULL == pModuleManager)
		return FALSE;

	return pModuleManager->SetCBBilling(this, DispatchBilling, DisconnectBilling);
}

BOOL AgsmBilling::SendBuyCashItemKr(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList)
{
	return BuyCashItem(pAgpdCharacter, lProductID, pszDesc, llMoney, sList.m_ullBuyID, sList.m_lItemTID );
}
BOOL AgsmBilling::CBAddServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (NULL == pData || NULL == pClass)
		return FALSE;

	AgsmBilling	*pThis = (AgsmBilling *) pClass;
	AgsdServer	*pAgsdServer = (AgsdServer *) pData;

	if (AGSMSERVER_TYPE_BILLING_SERVER == pAgsdServer->m_cType)
	{
		pThis->m_pAgsdServerBilling = pAgsdServer;
	}
	return TRUE;
}

#ifdef _HANGAME_
BOOL AgsmBilling::ConnectHanGameBilling()
{
	char* szGameString;
	if(GetGameEnv().IsAlpha())
		szGameString = ARHCLORD_TEST_GAME_ID;
	else
		szGameString = ARCHLORD_GAME_ID;

	int iResult = HannEverBillInit(szGameString);
	if(iResult != HAN_NEVERBILL_OK)
	{
		CHAR szBuf[255];
		AuTimeStamp::GetCurrentTimeStampString(szBuf, 254);

		char strCharBuff[512] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] HannEverBillInit returned a failure. result:%d\n", szBuf, iResult);
		AuLogFile_s(BILLING_ERROR_LOG_FILE_NAME, strCharBuff);
		return FALSE;
	}

	return TRUE;
}

BOOL AgsmBilling::GetHanCoin(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	RESULT_QUERYCOIN rstQC;
	CLEAR_RST_QC(&rstQC);

	if(HAN_NEVERBILL_OK != HannEverBillQueryCoin(&rstQC, pcsAgsdCharacter->m_szAccountID))
	{
		CHAR szBuf[255];
		AuTimeStamp::GetCurrentTimeStampString(szBuf, 254);

		char strCharBuff[512] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] HannEverBillQueryCoin returned a failure. [Acc:%s] %s\n",
			szBuf, pcsAgsdCharacter->m_szAccountID, HannEverBillErrorString(HannEverBillGetLastError()));
		AuLogFile_s(BILLING_ERROR_LOG_FILE_NAME, strCharBuff);
		return FALSE;
	}
	else
	{
		AgsdBillingMoney csBillingMoney;
		memset(&csBillingMoney, 0, sizeof(csBillingMoney));

		csBillingMoney.m_pAgpdCharacter = pcsCharacter;
		csBillingMoney.m_lResult = AGSMBILLING_RESULT_SUCCESS;

		csBillingMoney.m_llMoney = rstQC.lCoin;
		csBillingMoney.m_llCouponMoney = rstQC.lGiftCoin;
		csBillingMoney.m_llExternalEventMoney = rstQC.lExEvntCoin;
		csBillingMoney.m_llInternalEventMoney = rstQC.lInEvntCoin;

		EnumCallback(AGSMBILLING_CB_RESULT_GETMONEY, &csBillingMoney, csBillingMoney.m_pAgpdCharacter);
	}

	return TRUE;
}
#endif

BOOL AgsmBilling::BuyCashItem(AgpdCharacter* pcsCharacter, INT32 lProductID, CHAR* pszDesc, INT64 llMoney, UINT64 ullListSeq, INT32 lItemTID)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

#ifdef _HANGAME_
	REQUEST_USECOIN reqUC;
	CLEAR_REQ_UC(&reqUC);

	RESULT_USECOIN rstUC;
	CLEAR_RST_UC(&rstUC);

	CHAR szTmp[255] = {0,};

	AgsdBillingItem csBillingItem;
	ZeroMemory(&csBillingItem, sizeof(csBillingItem));
	csBillingItem.m_pAgpdCharacter = pcsCharacter;

	// 2008.02.21. steeple
	// 0 원짜리 아이템이라면 그냥 무사 통과이다. OrderID 에다가는 0 넣었다.
	if((INT32)llMoney == 0)
	{
		csBillingItem.m_lResult = AGSMBILLING_RESULT_SUCCESS;

		csBillingItem.m_ullOrderNo = 0;
		csBillingItem.m_lProductID = lProductID;
		csBillingItem.m_llBuyMoney = llMoney;
		csBillingItem.m_ullListSeq = ullListSeq;

		EnumCallback(AGSMBILLING_CB_RESULT_BUYITEM, &csBillingItem, csBillingItem.m_pAgpdCharacter);
		return TRUE;
	}

	// set params
	reqUC.nPayment = (INT32)llMoney;
	strcpy_s(reqUC.szUserKey, &pcsAgsdCharacter->m_szAccountID[0]);
	strcpy_s(reqUC.szCPID, "archlord");

	sprintf_s(szTmp, "LNITEM%03d", lProductID);
	strcpy_s(reqUC.szItemID, szTmp);

	AgpdItemTemplate* pcsItemTemplate = m_pAgpmItem->GetItemTemplate(lItemTID);
	if(pcsItemTemplate)
		strncpy_s(reqUC.szTitle, _TRUNCATE, pcsItemTemplate->m_szName, nEB_TITLE_LEN);
	else
		strncpy_s(reqUC.szTitle, _TRUNCATE, pszDesc, nEB_TITLE_LEN);

	sprintf_s(szTmp, "%I64d", ullListSeq);
	strcpy_s(reqUC.szMemo, szTmp);

	strcpy_s(reqUC.szRemoteIP, &pcsAgsdCharacter->m_strIPAddress[0]);

	if(HAN_NEVERBILL_OK != HannEverBillUseCoin(&rstUC, &reqUC))
	{
		CHAR szBuf[255];
		AuTimeStamp::GetCurrentTimeStampString(szBuf, 254);

		char strCharBuff[512] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] HannEverBillUseCoin returned a failure. [Acc:%s] %s\n",
			szBuf, pcsAgsdCharacter->m_szAccountID, HannEverBillErrorString(HannEverBillGetLastError()));
		AuLogFile_s(BILLING_ERROR_LOG_FILE_NAME, strCharBuff);

		csBillingItem.m_lResult = AGSMBILLING_RESULT_FAIL;
		EnumCallback(AGSMBILLING_CB_RESULT_BUYITEM, &csBillingItem, csBillingItem.m_pAgpdCharacter);
		return FALSE;
	}
	else
	{
		if(rstUC.nRtnCode == 0)
		{
			// 성공
			csBillingItem.m_lResult = AGSMBILLING_RESULT_SUCCESS;

			csBillingItem.m_ullOrderNo = (UINT64)_atoi64(rstUC.szRtnMsg);
			csBillingItem.m_lProductID = lProductID;
			csBillingItem.m_llBuyMoney = llMoney;
			csBillingItem.m_ullListSeq = ullListSeq;

			EnumCallback(AGSMBILLING_CB_RESULT_BUYITEM, &csBillingItem, csBillingItem.m_pAgpdCharacter);
		}
		else
		{
			switch(rstUC.nRtnCode)
			{
			case 99:
				csBillingItem.m_lResult = AGSMBILLING_RESULT_SYSTEM_FAILURE;
				break;

			case 7:
				csBillingItem.m_lResult = AGSMBILLING_RESULT_NOT_ENOUGH_MONEY;
				break;

			default :		// 다른 응답코드도 있지만 모두 FAIL로 처리한다.
				csBillingItem.m_lResult = AGSMBILLING_RESULT_FAIL;
				break;
			}

			EnumCallback(AGSMBILLING_CB_RESULT_BUYITEM, &csBillingItem, csBillingItem.m_pAgpdCharacter);
		}
	}

	return rstUC.nRtnCode == 0 ? TRUE : FALSE;
#endif

	return TRUE;
}
BOOL AgsmBilling::SendGetCashMoneyKr(AgpdCharacter *pAgpdCharacter)
{
#ifdef USE_HANGAME_BILLING
	return GetHanCoin(pAgpdCharacter);
#endif

	if (NULL == pAgpdCharacter ||
		NULL == m_pAgsdServerBilling ||
		FALSE == m_pAgsdServerBilling->m_bIsConnected )
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

	char *pszPacket = (char *) m_PacketMemoryPool.Alloc();
	if (NULL == pszPacket)
		return FALSE;

	AgsdBillingMoney *pAgsdBilling = new AgsdBillingMoney;

	// add to map
	pAgsdBilling->m_lID = m_GenerateID.GetID();
	pAgsdBilling->m_pAgpdCharacter = pAgpdCharacter;

	if (FALSE == Add(pAgsdBilling))
	{
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		pAgsdBilling->Release();

		return FALSE;
	}

	char szFormat[128] = {0, };
	sprintf(szFormat, "|%d|%s\n", pAgsdBilling->m_lID, pAgsdCharacter->m_szAccountID);

	INT32 lLength = (INT32)strlen(szFormat);
	sprintf(pszPacket, "60%05d%s", lLength, szFormat);

	BOOL bResult = SendPacket(pszPacket, (INT16)strlen(pszPacket), m_pAgsdServerBilling->m_dpnidServer);

	m_PacketMemoryPool.Free(pszPacket);

	if (FALSE == bResult)
	{
		Remove(pAgsdBilling->m_lID);
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		pAgsdBilling->Release();
	}

	return bResult;
}

#endif //_HAN_GAME_..//JK_빌링

#ifdef _WEBZEN_BILLING_

BOOL AgsmBilling::OnAddModuleGlobal()
{
	return TRUE;
}

eAGSMBILLING_CONNECT_RESULT AgsmBilling::ConnectBillingServerGlobal()
{
	return AGSMBILLING_CONNECT_RESULT_SUCCESS;
}

BOOL AgsmBilling::WaitForConnectServerGlobal()
{
	return TRUE;
}


BOOL AgsmBilling::OnInitGlobal()
{
	m_pAgsmServerManager2	= (AgsmServerManager2*)GetModule("AgsmServerManager2");
	m_pagpmBillInfo			= (AgpmBillInfo*)GetModule("AgpmBillInfo");
	m_pagsmBillInfo			= (AgsmBillInfo*)GetModule("AgsmBillInfo");
	m_pAgsmCharacter		= (AgsmCharacter*)GetModule("AgsmCharacter");
	m_pagpmFactors			= (AgpmFactors*)GetModule("AgpmFactors");
	m_pAgpmCashMall			= (AgpmCashMall*)GetModule("AgpmCashMall");
	m_pAgpmItem				= (AgpmItem*)GetModule("AgpmItem");
	m_pAgsmItem				= (AgsmItem*)GetModule("AgsmItem");
	m_pagsmSkill			= (AgsmSkill*)GetModule("AgsmSkill");
	m_pAgsmItemManager		= (AgsmItemManager*)GetModule("AgsmItemManager"); //JK_웹젠빌링
	m_pagpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");//JK)심야샷다운
	m_pagsmSystemMessage	= (AgsmSystemMessage *) GetModule("AgsmSystemMessage");//JK_심야샷다운
	m_pAgpmConfig			= (AgpmConfig*)GetModule("AgpmConfig");//JK_심야샷다운

	if ( !m_pAgsmServerManager2 || !m_pagpmBillInfo || !m_pAgsmCharacter || !m_pagpmFactors || !m_pAgpmCashMall || !m_pAgpmItem || !m_pAgsmItemManager || !m_pagpmCharacter || !m_pagsmSystemMessage || !m_pAgpmConfig)
		return FALSE;
	//JK_웹젠빌링
	if (!m_pagpmBillInfo->SetCallbackUpdateSalesZoneScriptVersionInfo(CBSendToClientSalesZoneScriptVersion, this))
		return FALSE;


	m_Admin.SetCount(10000);

	if (!m_Admin.InitializeObject(sizeof(AgsdBilling *), m_Admin.GetCount()))
		return FALSE;
/*
	m_pAgsdServerBilling = new AgsdServer;
	if(!m_pAgsdServerBilling)
		return FALSE;

	m_pAgsdServerBilling->m_bIsConnected = FALSE;
*/
	m_pAgsmBillingGlobal = new AgsmBillingGlobal;
	if(!m_pAgsmBillingGlobal)
		return FALSE;

	m_pAgsmBillingGlobal->Initialize();

	//JK_빌링웹젠
	//m_pAgsmBillingWebzen = new AgsmBillingWebzen();
	//if(!m_pAgsmBillingWebzen)
	//	return FALSE;


	return TRUE;
}

BOOL AgsmBilling::OnIdle2(UINT32 ulClockCount)
{
	if(m_ulLastCheckClockClock + 60*1000 < ulClockCount)
	{
		if(m_pAgsmBillingGlobal)//if(g_eServiceArea == AP_SERVICE_AREA_GLOBAL && m_pAgsmBillingGlobal)
		{
			m_pAgsmBillingGlobal->OnIdle();
		}

		m_ulLastCheckClockClock = ulClockCount;
	}
	//JK_PC방과금상태확인
	if(m_ulLastCheckClockForWebzen + 60*1000*5 < ulClockCount)
	{
		if(m_pAgsmBillingGlobal)
		{
			m_pAgsmBillingGlobal->CheckPCRoomPointAllUser();
		}

		m_ulLastCheckClockForWebzen = ulClockCount;
	}
	//JK_심야샷다운
	if(m_ulLastCheckClockForMidNightShutDown + 60 * 1000 < ulClockCount)
	{
		INT32 nLimitUnderAge = m_pAgpmConfig->GetLimitUnderAge();
		if(m_pAgsmServerManager2->GetThisServerType() == AGSMSERVER_TYPE_GAME_SERVER && nLimitUnderAge != 0)
		{
			INT32 nLeftMin = m_pAgsmServerManager2->CheckMidnightShutDownTime();
			switch(nLeftMin)
			{
			case 0:
				{
					if(!m_pAgsmServerManager2->GetThisServer()->m_bMidNightShutDown)
					{
						KickOffUnderAge();
						m_pAgsmServerManager2->GetThisServer()->m_bMidNightShutDown = TRUE;
					}
				}
				break;
			case 120:
			case 30:
			case 5:
			case 1:
				{
					//메세지를 보내리라..Min을 넣어서..
					SendMidNightShutDownMessageAll(nLeftMin);
					m_pAgsmServerManager2->GetThisServer()->m_bMidNightShutDown = FALSE;
				}
				break;
			default:
				break;
			}

		}
		m_ulLastCheckClockForMidNightShutDown = ulClockCount;

	}

	return TRUE;
}
   

//JK_심야샷다운
void AgsmBilling::KickOffUnderAge()
{

	INT32 lIndex = 0;
	AgpdCharacter* pcsCharacter = NULL;
	INT32 nLimitUnderAge = m_pAgpmConfig->GetLimitUnderAge();
	while (pcsCharacter = m_pagpmCharacter->GetCharSequence(&lIndex))
	{
		if (!m_pagpmCharacter->IsPC(pcsCharacter) || m_pAgsmCharacter->GetCharDPNID(pcsCharacter) == 0) continue;
		if ( m_pagpmBillInfo->GetAccountAge(pcsCharacter) < nLimitUnderAge)
		{
			UINT32 ulNID = m_pAgsmCharacter->GetCharDPNID(pcsCharacter);
			INT32 lCID = pcsCharacter->m_lID;

			// 무언가 메세지를 던지면 더 좋겠군,...
			m_pagsmSystemMessage->SendSystemMessageWithType(ulNID, AGPMSYSTEMMESSAGE_PACKET_MODAL_MESSAGE, AGPMSYSTEMMESSAGE_CODE_MIDNIGHTSHUTDOWN);//12시가 되어서 문닫았소..
			


		//	pcsCharacter->m_Mutex.Release();

			if(ulNID != 0)
				DestroyClient(ulNID);
			else if(lCID != 0)
				m_pagpmCharacter->RemoveCharacter(lCID);
			
		}
	}

}

//JK_심야샷다운
void AgsmBilling::SendMidNightShutDownMessageAll(INT32 nLeftMin)
{

	INT32 lIndex = 0;
	AgpdCharacter* pcsCharacter = NULL;
	while (pcsCharacter = m_pagpmCharacter->GetCharSequence(&lIndex))
	{
		if (!m_pagpmCharacter->IsPC(pcsCharacter) || m_pAgsmCharacter->GetCharDPNID(pcsCharacter) == 0) continue;

		UINT32 ulNID = m_pAgsmCharacter->GetCharDPNID(pcsCharacter);
		INT32 lCID = pcsCharacter->m_lID;

		// 무언가 메세지를 던지면 더 좋겠군,...
		m_pagsmSystemMessage->SendSystemMessage(ulNID, AGPMSYSTEMMESSAGE_CODE_MIDNIGHTSHUTDOWN_LEFT_MIN,nLeftMin);//12시가 되어서 문닫았소..

	}

}


//JK_웹젠빌링
void AgsmBilling::SetSalesZoneScriptVersion(int Year, int YearIndentity)
{
	m_nYear = Year;
	m_nYearIndentity = YearIndentity;
}
//JK_웹젠빌링
void AgsmBilling::UpdateScriptVersionAllUser(int Year, int YearIdentity)
{
	SetSalesZoneScriptVersion(Year, YearIdentity);

	PACKET_BILLINGINFO_VERSIONINFO_SCACK pPacket(Year, YearIdentity);
	SendPacketAllUser(pPacket);

}
//JK_웹젠빌링
BOOL AgsmBilling::SendToClientSalesZoneScriptVersion(AgpdCharacter *pAgpdCharacter)
{
	if (NULL == pAgpdCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;
	
	PACKET_BILLINGINFO_VERSIONINFO_SCACK pPacket( m_nYear, m_nYearIndentity);
	SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter));

	return TRUE;
}
//JK_웹젠빌링
BOOL AgsmBilling::CBSendToClientSalesZoneScriptVersion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass )
		return FALSE;

	AgsmBilling	*pThis			= (AgsmBilling *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	return pThis->SendToClientSalesZoneScriptVersion(pcsCharacter);
	
}
BOOL AgsmBilling::SendGetCashMoneyGlobal(AgpdCharacter *pAgpdCharacter)
{
	if (NULL == pAgpdCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

#ifdef _WEBZEN_BILLING_
	DWORD dwAccountGUID = m_pagpmBillInfo->GetGUIDGlobal(pAgpdCharacter);
	BOOL bResult = m_pAgsmBillingGlobal->InquireCash( dwAccountGUID );
#else

	AgsdBillingMoneyGlobal *pAgsdBilling = new AgsdBillingMoneyGlobal;
	if(!pAgsdBilling)
		return FALSE;

	// add to map
	pAgsdBilling->m_lID = m_GenerateID.GetID();
	pAgsdBilling->m_pAgpdCharacter = pAgpdCharacter;

	if (FALSE == Add(pAgsdBilling, pAgsdCharacter->m_szAccountID))
	{
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		Remove(pAgsdCharacter->m_szAccountID);
		pAgsdBilling->Release();

		return FALSE;
	}

	BOOL bResult = m_pAgsmBillingGlobal->InquireCash( pAgsdCharacter->m_szAccountID );

	if (FALSE == bResult)
	{
		Remove(pAgsdCharacter->m_szAccountID);
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		pAgsdBilling->Release();
	}
#endif
	return bResult;
}

BOOL AgsmBilling::SendBuyCashItemGlobal(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, UINT64 ullListSeq, INT32 lItemTID, INT32 lType)
{
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

	AgsdServer *pAgsdServer = m_pAgsmServerManager2->GetThisServer();
	if (NULL == pAgsdServer)
		return FALSE;

	AgpmCashItemInfo* pCashItemInfo = m_pAgpmCashMall->GetCashItem( lProductID );
	if ( NULL == pCashItemInfo )
		return FALSE;

	AgpdItemTemplate* pItemTemplate = m_pAgpmItem->GetItemTemplate( pCashItemInfo->m_alItemTID[0] );
	if ( NULL == pItemTemplate )
		return FALSE;

	INT32 Class = m_pagpmFactors->GetClass(&pAgpdCharacter->m_csFactor);
	INT32 Level = m_pagpmFactors->GetLevel(&pAgpdCharacter->m_csFactor);
	//	int MethodType = (lType == AGPMCASHMALL_TYPE_WCOIN_PPCARD) ? 1 : 0;

	CHAR szItemName[50] = {0, };
	strncpy_s(szItemName, sizeof(szItemName), pItemTemplate->m_szName, _TRUNCATE);

	AgsdBillingItem *pAgsdBilling = new AgsdBillingItem;
	pAgsdBilling->m_lID = m_GenerateID.GetID();
	pAgsdBilling->m_pAgpdCharacter = pAgpdCharacter;
	pAgsdBilling->m_lProductID = lProductID;
	pAgsdBilling->m_llBuyMoney = llMoney;
	pAgsdBilling->m_ullListSeq = ullListSeq;
	pAgsdBilling->m_lType      = lType;

	if (FALSE == Add(pAgsdBilling, pAgsdCharacter->m_szAccountID))
	{
		m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
		Remove(pAgsdCharacter->m_szAccountID);
		pAgsdBilling->Release();

		return FALSE;
	}

	return m_pAgsmBillingGlobal->BuyProduct(pAgsdCharacter->m_szAccountID, lProductID, szItemName, ullListSeq, Class, Level, pAgpdCharacter->m_szID, pAgsdServer->m_lServerID, llMoney, lType );

}
//JK_웹젠빌링
BOOL AgsmBilling::SendBuyCashItemWebzen(AgpdCharacter *pAgpdCharacter, INT32 PackageSeq, INT32 DisplaySeq, INT32 PriceSeq)
{
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

	AgsdServer *pAgsdServer = m_pAgsmServerManager2->GetThisServer();
	if (NULL == pAgsdServer)
		return FALSE;

	INT32 Class = m_pagpmFactors->GetClass(&pAgpdCharacter->m_csFactor);
	INT32 Level = m_pagpmFactors->GetLevel(&pAgpdCharacter->m_csFactor);

	DWORD dwAccountGUID = m_pagpmBillInfo->GetGUIDGlobal(pAgpdCharacter);
	if(!dwAccountGUID) 
	{
		return FALSE;
	}

	return m_pAgsmBillingGlobal->BuyProduct(dwAccountGUID, PackageSeq, DisplaySeq, PriceSeq, Class, Level, pAgpdCharacter->m_szID,"", pAgsdServer->m_lServerID );

}

//JK_웹젠빌링
BOOL AgsmBilling::SendUseStorageToBilling(AgpdCharacter *pAgpdCharacter, INT32 StorageSeq, INT32 StorageItemSeq)
{
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

	AgsdServer *pAgsdServer = m_pAgsmServerManager2->GetThisServer();
	if (NULL == pAgsdServer)
		return FALSE;

	INT32 Class = m_pagpmFactors->GetClass(&pAgpdCharacter->m_csFactor);
	INT32 Level = m_pagpmFactors->GetLevel(&pAgpdCharacter->m_csFactor);

	DWORD dwAccountGUID = m_pagpmBillInfo->GetGUIDGlobal(pAgpdCharacter);
	if(!dwAccountGUID) 
	{
		return FALSE;
	}

	return m_pAgsmBillingGlobal->UseStorage(dwAccountGUID, &pAgsdCharacter->m_strIPAddress[0],StorageSeq, StorageItemSeq, Class, Level, pAgpdCharacter->m_szID,"", pAgsdServer->m_lServerID );

}
//JK_웹젠빌링
BOOL AgsmBilling::SendInquireStorageListToBilling(AgpdCharacter *pAgpdCharacter, INT32 NowPage)
{
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return FALSE;

	AgsdServer *pAgsdServer = m_pAgsmServerManager2->GetThisServer();
	if (NULL == pAgsdServer)
		return FALSE;

	DWORD dwAccountGUID = m_pagpmBillInfo->GetGUIDGlobal(pAgpdCharacter);
	if(!dwAccountGUID) 
	{
		return FALSE;
	}

	if (NowPage < 1) return FALSE;

	return m_pAgsmBillingGlobal->InquireStorageList(dwAccountGUID, NowPage);

}

BOOL AgsmBilling::OnInquireCash(long ReturnCode, CHAR* AccountID, double WCoin, double PCoin )
{
	AgsdBilling *pAgsdBilling = Get(AccountID);
	if (!pAgsdBilling)
		return FALSE;

	if(pAgsdBilling->Type() != AGSMBILLING_TYPE_MONEY)
		return FALSE;

	if(ReturnCode != 0)
		return FALSE;

	AgpdCharacter* pcsCharacter = pAgsdBilling->m_pAgpdCharacter;
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;

	m_pagpmBillInfo->SetCashGlobal(pcsCharacter, WCoin, PCoin);

	m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
	Remove(pagsdCharacter->m_szAccountID);
	pAgsdBilling->Release();

	CashInfoGlobal pCash;
	m_pagpmBillInfo->GetCashGlobal(pcsCharacter, pCash.m_WCoin, pCash.m_PCoin);

	PACKET_BILLINGINFO_CASHINFO pPacket(pcsCharacter->m_lID, pCash.m_WCoin, pCash.m_PCoin);
	SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

	return TRUE;
}
//JK_웹젠빌링
BOOL AgsmBilling::OnInquireCash(long ReturnCode, DWORD AccountGUID, double WCoin )
{
	AgpdCharacter* pcsCharacter = m_pAgsmBillingGlobal->GetCharacter(AccountGUID);
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pagsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);
	if(!pagsdCharacter)
		return FALSE;
/*
	AgsdBilling *pAgsdBilling = Get(pagsdCharacter->m_szAccountID);
	if (!pAgsdBilling)
		return FALSE;
	
	if(pAgsdBilling->Type() != AGSMBILLING_TYPE_MONEY)
		return FALSE;

	m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
	Remove(pagsdCharacter->m_szAccountID);
	pAgsdBilling->Release();
*/
	if(ReturnCode != 0)
		return FALSE;

	m_pagpmBillInfo->SetCashGlobal(pcsCharacter, WCoin, 0);

	CashInfoGlobal pCash;
	m_pagpmBillInfo->GetCashGlobal(pcsCharacter, pCash.m_WCoin, pCash.m_PCoin);

	PACKET_BILLINGINFO_CASHINFO pPacket(pcsCharacter->m_lID, pCash.m_WCoin, pCash.m_PCoin);
	SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

	return TRUE;
}

BOOL AgsmBilling::OnBuyProduct(long ResultCode, CHAR* AccounID, DWORD DeductCashSeq)
{
	AgsdBilling *pAgsdBilling = Get(AccounID);
	if (NULL == pAgsdBilling)
		return FALSE;

	AgsdBillingItem* pcsBillingItem = (AgsdBillingItem*)pAgsdBilling;
	pcsBillingItem->m_ullOrderNo = DeductCashSeq;

	AgpdCharacter* pcsCharacter = pAgsdBilling->m_pAgpdCharacter;

	eAGSMBILLING_RESULT lResult;

	switch(ResultCode)
	{
	case 0:
		{
			if(DeductCashSeq != -1)
				lResult = AGSMBILLING_RESULT_SUCCESS;
			else
				lResult = AGSMBILLING_RESULT_SYSTEM_FAILURE;
		} break;
	case 1:
		{
			lResult = AGSMBILLING_RESULT_NOT_ENOUGH_MONEY;				
		} break;
	default:
		{
			lResult = AGSMBILLING_RESULT_SYSTEM_FAILURE;
		} break;
	}

	pAgsdBilling->m_lResult = (INT32)lResult;

	EnumCallback(AGSMBILLING_CB_RESULT_BUYITEM, pAgsdBilling, pAgsdBilling->m_pAgpdCharacter);

	m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
	Remove(AccounID);
	pAgsdBilling->Release();

	if(lResult == AGSMBILLING_RESULT_SUCCESS) // success
		SendGetCashMoney(pcsCharacter);

	return TRUE;
}
//JK_웹젠빌링
BOOL AgsmBilling::OnBuyProductWebzen(DWORD AccountGUID, DWORD ResultCode, DWORD LeftProductCount)
{
	AgpdCharacter* pcsCharacter = m_pAgsmBillingGlobal->GetCharacter(AccountGUID);
	if(!pcsCharacter)
		return FALSE;
	
	PACKET_CASHMALL_BUY_SCACK pPacket(ResultCode, LeftProductCount);
	SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

	//구매 성공했다면 다시 캐쉬정보를 요구하자.
	if(ResultCode == 0)
		m_pAgsmBillingGlobal->InquireCash(AccountGUID);

	return TRUE;
}
//JK_웹젠빌링
BOOL AgsmBilling::OnUseStorage(DWORD AccountGUID, DWORD ResultCode, CHAR* InGameProductID, BYTE PropertyCount, DWORD ProductSeq, DWORD StorageSeq, DWORD StorageItemSeq, STItemProperty* PropertyList)
{
	AgpdCharacter* pcsCharacter = m_pAgsmBillingGlobal->GetCharacter(AccountGUID);
	if(!pcsCharacter)
	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnUseStorage ERROR--CAN NOT FIND AgpdCharacter : AccountSeq = %d,InGameProductID = %s, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d", 
			AccountGUID, ResultCode, InGameProductID,ProductSeq,StorageSeq, StorageItemSeq);
		m_pAgsmBillingGlobal->OnLog(strLog);

		m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

		PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
		SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

		return FALSE;
	}
	int nFinalItemCount = 1;

	if(ResultCode == 0)
	{
		for(int i = 0; i < PropertyCount; i++)
		{
			if( PropertyList[i].PropertySeq == 87 )//일단 소모성아이템 수량만 적용
			{
				nFinalItemCount = PropertyList[i].Value;
			}
		}

		if( !m_pAgsmBillingGlobal->IsNumberFromItemID(InGameProductID, strlen(InGameProductID)) )
		{
			CHAR strLog[256] = { 0, };
			sprintf_s(strLog, sizeof(strLog), "OnUseStorage ERROR--NOT NUMBER InGameProductID : AccountSeq = %d,InGameProductID = %s, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d", 
				AccountGUID, ResultCode, InGameProductID,ProductSeq,StorageSeq, StorageItemSeq);
			m_pAgsmBillingGlobal->OnLog(strLog);

			m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);
	
			PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
			SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

			return FALSE;
		}
		
		int nItemID = atoi( InGameProductID );



		if (!m_pAgpmItem->IsAnyEmptyCashInventory(pcsCharacter))
		{
			m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

//			PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
//			SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

			return FALSE;

		}


		AgpdItemTemplate* pItemTemplate = m_pAgpmItem->GetItemTemplate(nItemID);
		if (!pItemTemplate)
		{
			return FALSE;
		}

		AgpdItem* pcsItem = NULL;
		if (pItemTemplate->m_bStackable && pItemTemplate->m_lMaxStackableCount >= nFinalItemCount) //스택 가능 하고 최대 스택 갯수 미만이면...갯수만큼 생성.
		{
			pcsItem = m_pAgsmItemManager->CreateItem(pItemTemplate->m_lID, pcsCharacter, nFinalItemCount);

			if (!pcsItem)
			{
				CHAR strLog[256] = { 0, };
				sprintf_s(strLog, sizeof(strLog), "OnUseStorage ERROR--CAN NOT CreateItem!!!: AccountSeq = %d,InGameProductID = %s, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d", 
					AccountGUID, ResultCode, InGameProductID,ProductSeq,StorageSeq, StorageItemSeq);
				m_pAgsmBillingGlobal->OnLog(strLog);


				m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

				PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
				SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

				return FALSE;
			}

			// insert item into cash inventory
			if (!m_pAgpmItem->AddItemToCashInventory(pcsCharacter, pcsItem))
			{
				CHAR strLog[256] = { 0, };
				sprintf_s(strLog, sizeof(strLog), "OnUseStorage ERROR--CAN NOT AddItemToCashInventory : AccountSeq = %d,InGameProductID = %s, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d", 
					AccountGUID, ResultCode, InGameProductID,ProductSeq,StorageSeq, StorageItemSeq);
				m_pAgsmBillingGlobal->OnLog(strLog);

				m_pAgpmItem->RemoveItem(pcsItem, TRUE);

				m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

				PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
				SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

				return FALSE;
			}
		}
		else
		{
			//쑤셔넣을 공간이 있는지 확인.
			AgpdGrid *pcsGrid = m_pAgpmItem->GetCashInventoryGrid(pcsCharacter);
			if (nFinalItemCount > pcsGrid->m_lGridCount - pcsGrid->m_lItemCount)
			{
				m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

				return FALSE;
			}

			for (int i = 0; i < nFinalItemCount ; i++)
			{
				pcsItem = m_pAgsmItemManager->CreateItem(pItemTemplate->m_lID, pcsCharacter, 1);

				if (!pcsItem)
				{
					CHAR strLog[256] = { 0, };
					sprintf_s(strLog, sizeof(strLog), "OnUseStorage ERROR--CAN NOT CreateItem!!!: AccountSeq = %d,InGameProductID = %s, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d", 
						AccountGUID, ResultCode, InGameProductID,ProductSeq,StorageSeq, StorageItemSeq);
					m_pAgsmBillingGlobal->OnLog(strLog);


					m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

					PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
					SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

					return FALSE;
				}

				// insert item into cash inventory
				if (!m_pAgpmItem->AddItemToCashInventory(pcsCharacter, pcsItem))
				{
					CHAR strLog[256] = { 0, };
					sprintf_s(strLog, sizeof(strLog), "OnUseStorage ERROR--CAN NOT AddItemToCashInventory : AccountSeq = %d,InGameProductID = %s, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d", 
						AccountGUID, ResultCode, InGameProductID,ProductSeq,StorageSeq, StorageItemSeq);
					m_pAgsmBillingGlobal->OnLog(strLog);

					m_pAgpmItem->RemoveItem(pcsItem, TRUE);

					m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

					PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
					SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

					return FALSE;
				}
			}

			
		}


		/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
		// 아이템을 생성하자
		AgpdItem *pcsItem = m_pAgsmItemManager->CreateItem(nItemID, pcsCharacter, nFinalItemCount);
		if (!pcsItem)
		{
			CHAR strLog[256] = { 0, };
			sprintf_s(strLog, sizeof(strLog), "OnUseStorage ERROR--CAN NOT CreateItem!!!: AccountSeq = %d,InGameProductID = %s, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d", 
				AccountGUID, ResultCode, InGameProductID,ProductSeq,StorageSeq, StorageItemSeq);
			m_pAgsmBillingGlobal->OnLog(strLog);

			
			m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

			PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
			SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

			return FALSE;
		}
		

		// insert item into cash inventory
		if (!m_pAgpmItem->AddItemToCashInventory(pcsCharacter, pcsItem))
		{
			CHAR strLog[256] = { 0, };
			sprintf_s(strLog, sizeof(strLog), "OnUseStorage ERROR--CAN NOT AddItemToCashInventory : AccountSeq = %d,InGameProductID = %s, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d", 
				AccountGUID, ResultCode, InGameProductID,ProductSeq,StorageSeq, StorageItemSeq);
			m_pAgsmBillingGlobal->OnLog(strLog);

			m_pAgpmItem->RemoveItem(pcsItem, TRUE);
			
			m_pAgsmBillingGlobal->RollbackUseStorage(AccountGUID, StorageSeq, StorageItemSeq);

			PACKET_CASHMALL_USESTORAGE_SCACK pPacket(-1);
			SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

			return FALSE;
		}
*/

		PACKET_CASHMALL_USESTORAGE_SCACK pPacket(ResultCode);
		SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

	}
	else
	{
		PACKET_CASHMALL_USESTORAGE_SCACK pPacket(ResultCode);
		SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

		return FALSE;
	}

	return TRUE;
}

//JK_웹젠빌링
BOOL AgsmBilling::OnInquireStorageListPageGiftMessage(DWORD AccountGUID, DWORD ResultCode, CHAR StorageType, int NowPage, int TotalPage, int TotalCount, int ListCount, STStorageNoGiftMessage* StorageList)
{
	AgpdCharacter* pcsCharacter = m_pAgsmBillingGlobal->GetCharacter(AccountGUID);
	if(!pcsCharacter)
		return FALSE;
	
	if(ResultCode == 0)
	{
		PACKET_CASHMALL_INQUIRESTORAGELIST_SCACK pPacket(ResultCode, NowPage, TotalPage, ListCount, TotalCount, StorageList);
		return SendPacketUser(pPacket, m_pAgsmCharacter->GetCharDPNID(pcsCharacter));

	}
	else
		return FALSE;


}

BOOL AgsmBilling::OnInquirePersonDeduct(DWORD AccountGUID, DWORD ResultCode)
{
	if(ResultCode == 1)
	{
		AgpdCharacter* pcsCharacter = m_pAgsmBillingGlobal->GetCharacter(AccountGUID);
		if(!pcsCharacter)
			return FALSE;

		AgpdBillInfo* pcsAttachBillInfo = m_pagpmBillInfo->GetADCharacter(pcsCharacter);
		if(!pcsAttachBillInfo)
			return FALSE;

		if(!(pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_WEBZEN_GPREMIUM))
		{
			AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pcsCharacter);
			if (NULL == pAgsdCharacter)
				return FALSE;

			m_pAgsmBillingGlobal->Login(AccountGUID, &pAgsdCharacter->m_strIPAddress[0]);
		}
	}

	return TRUE;
}

BOOL AgsmBilling::OnUserStatus(DWORD AccountGUID, DWORD BillingGUID, DWORD RealEndDate, DOUBLE RestTime, INT32 DeductType)
{
	if(DeductType == 0)
		return FALSE;

	AgpdCharacter* pcsCharacter = m_pAgsmBillingGlobal->GetCharacter(AccountGUID);
	if(!pcsCharacter)
		return FALSE;

	AgpdBillInfo* pcsAttachBillInfo = m_pagpmBillInfo->GetADCharacter(pcsCharacter);
	if(!pcsAttachBillInfo)
		return FALSE;

	switch(DeductType)
	{
	case 1: //개인정액
		{
			pcsAttachBillInfo->m_ulPCRoomType |= AGPDPCROOMTYPE_WEBZEN_GPREMIUM;
			pcsAttachBillInfo->m_ulRemainCouponPlayTime = RealEndDate;
		} break;
	default:
		return FALSE;
	}

	m_pAgsmBillingGlobal->SetBillingGUID(AccountGUID, BillingGUID);

	// 버프 스킬들은 모두다 없앤다.
	m_pagsmSkill->EndAllBuffedSkillExceptDebuff(pcsCharacter, TRUE);

	m_pagpmBillInfo->UpdateIsPCRoom(pcsCharacter, TRUE);
	m_pagsmBillInfo->SendCharacterBillingInfo(pcsCharacter);	

	// 캐시 인벤토리에 사용 중인 아이템이 있으면, idle 설정함
	BOOL bRet = m_pAgpmItem->IsCharacterUsingCashItem(pcsCharacter);
	if (bRet)
	{
		// 자동으로 사용할 애들이 있으면 사용해준다.
		m_pAgsmItem->UseAllEnableCashItem(pcsCharacter,
			pcsCharacter->m_bRidable,
			pcsCharacter->m_bRidable ? AGSDITEM_PAUSE_REASON_RIDE : AGSDITEM_PAUSE_REASON_NONE);

		m_pAgsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, GetClockCount());
		m_pAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
	}

	m_pagsmSkill->RecastSaveSkill(pcsCharacter);

	return TRUE;
}
//JK_웹젠빌링 : 웹젠통합빌링
BOOL AgsmBilling::OnUserStatusWebzen(DWORD AccountGUID, DWORD BillingGUID, DWORD RealEndDate, DOUBLE RestTime, INT32 DeductType)
{

	AgpdCharacter* pcsCharacter = m_pAgsmBillingGlobal->GetCharacter(AccountGUID);
	if(!pcsCharacter)
		return FALSE;

	AgpdBillInfo* pcsAttachBillInfo = m_pagpmBillInfo->GetADCharacter(pcsCharacter);
	if(!pcsAttachBillInfo)
		return FALSE;
	
	BOOL bIsPCRoom = FALSE;

	//일단 TPack Type을 사용 한다..
	switch(DeductType)
	{
	case 0:// 사용가능 상품 없음
		{
			pcsAttachBillInfo->m_ulPCRoomType &= ~AGPDPCROOMTYPE_HANGAME_TPACK;
			pcsAttachBillInfo->m_ulRemainCouponPlayTime = 0;
			BillingGUID = 0;
			bIsPCRoom = FALSE;
		}
		break;
	case 2: //PC방정액
	case 3: //PC방정량
	case 6: //PC방 컬러요금제
		{
			pcsAttachBillInfo->m_ulPCRoomType |= AGPDPCROOMTYPE_HANGAME_TPACK;
			pcsAttachBillInfo->m_ulRemainCouponPlayTime = RealEndDate;
			bIsPCRoom = TRUE;
		} break;
	default:
		return FALSE;
	}

	//Deduct 정보가 바뀐경우 BillingGUID가 바뀐다...
	if(m_pAgsmBillingGlobal->GetBillingGUID(AccountGUID) != BillingGUID)
	{
		m_pAgsmBillingGlobal->SetBillingGUID(AccountGUID, BillingGUID);
		m_pagpmBillInfo->UpdateIsPCRoom(pcsCharacter, bIsPCRoom);
		m_pagsmBillInfo->SendCharacterBillingInfo(pcsCharacter);	

	}
/*
	if(bIsPCRoom)
	{

		// 버프 스킬들은 모두다 없앤다.
		m_pagsmSkill->EndAllBuffedSkillExceptDebuff(pcsCharacter, TRUE);

		// 캐시 인벤토리에 사용 중인 아이템이 있으면, idle 설정함
		BOOL bRet = m_pAgpmItem->IsCharacterUsingCashItem(pcsCharacter);
		if (bRet)
		{
			// 자동으로 사용할 애들이 있으면 사용해준다.
			m_pAgsmItem->UseAllEnableCashItem(pcsCharacter,
				pcsCharacter->m_bRidable,
				pcsCharacter->m_bRidable ? AGSDITEM_PAUSE_REASON_RIDE : AGSDITEM_PAUSE_REASON_NONE);

			m_pAgsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, GetClockCount());
			m_pAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_ITEM, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
		}

		m_pagsmSkill->RecastSaveSkill(pcsCharacter);
	}
*/
	return TRUE;
}

void AgsmBilling::CheckIn(AgpdCharacter *pAgpdCharacter)
{
	if(!pAgpdCharacter)
		return;

	DWORD	AccountGUID = m_pagpmBillInfo->GetGUIDGlobal(pAgpdCharacter);


	m_pAgsmBillingGlobal->RemoveUser(AccountGUID);
	m_pAgsmBillingGlobal->AddUser(pAgpdCharacter, AccountGUID);

#ifdef _WEBZEN_BILLING_
	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return;

	DWORD PCRoomGUID = m_pagpmBillInfo->GetPCRoomGUIDGlobal(pAgpdCharacter);
	DWORD ClientCnt  = m_pagpmBillInfo->GetClientCntGlobal(pAgpdCharacter);

	if(PCRoomGUID && ClientCnt <= 1)
	{
		m_pAgsmBillingGlobal->SetPCRoomGUID(AccountGUID, PCRoomGUID); //JK_PC방과금상태확인 
		m_pAgsmBillingGlobal->LoginWebzen(AccountGUID, &pAgsdCharacter->m_strIPAddress[0],PCRoomGUID );
	}
#else
	m_pAgsmBillingGlobal->InquirePersonDeduct(AccountGUID);
#endif
}

void AgsmBilling::CheckOut(AgpdCharacter *pAgpdCharacter)
{
	if(!pAgpdCharacter)
		return;

	DWORD	AccountGUID = m_pagpmBillInfo->GetGUIDGlobal(pAgpdCharacter);

	m_pAgsmBillingGlobal->LogOut(AccountGUID);

	m_pAgsmBillingGlobal->RemoveUser(AccountGUID);
}
//JK_PC방과금상태확인
void AgsmBilling::ReloginWebzen(DWORD dwAccountGUID)
{
	AgpdCharacter* pAgpdCharacter = m_pAgsmBillingGlobal->GetCharacter(dwAccountGUID);

	if(!pAgpdCharacter)
		return;

	DWORD	AccountGUID = m_pagpmBillInfo->GetGUIDGlobal(pAgpdCharacter);

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter)
		return;

	DWORD PCRoomGUID = m_pagpmBillInfo->GetPCRoomGUIDGlobal(pAgpdCharacter);
	DWORD ClientCnt  = m_pagpmBillInfo->GetClientCntGlobal(pAgpdCharacter);


	if(PCRoomGUID && ClientCnt <= 1)
	{
		m_pAgsmBillingGlobal->SetPCRoomGUID(AccountGUID, PCRoomGUID); //JK_PC방과금상태확인 
		m_pAgsmBillingGlobal->LoginWebzen(AccountGUID, &pAgsdCharacter->m_strIPAddress[0],PCRoomGUID );
	}

}

#endif //_WEBZEN_BILLING_
#endif



//	Socket callback point
//======================================================
//
BOOL AgsmBilling::DispatchBilling(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	if (NULL == pvPacket || NULL == pvParam || NULL == pvSocket)
		return FALSE;

	AgsmBilling		*pThis = (AgsmBilling *) pvParam;
	AsServerSocket	*pSocket = (AsServerSocket *) pvSocket;

	return pThis->OnReceive(pvPacket, pSocket->GetIndex());
}

BOOL AgsmBilling::DisconnectBilling(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	if (NULL == pvParam || NULL == pvSocket)
		return FALSE;

	AgsmBilling		*pThis = (AgsmBilling *) pvParam;
	AsServerSocket	*pSocket = (AsServerSocket *) pvSocket;

	return pThis->OnDisconnect();
}

//	Packet processing
//===========================================================
//
BOOL AgsmBilling::OnReceive(PVOID pvPacket, UINT32 ulNID)
	{
	if (NULL == pvPacket || 0 == ulNID)
		return FALSE;

	char *pszPacket = (char *) pvPacket;
	
	char sz[6];
	// code
	ZeroMemory(sz, sizeof(sz));
	char *psz = sz;
	for (INT32 i = 0; i < 2; ++i)
		*psz++ = *pszPacket++;
	
	INT32 lCode = atoi(sz);

	// length
	ZeroMemory(sz, sizeof(sz));
	psz = sz;
	for (INT32 i = 0; i < 5; ++i)
		*psz++ = *pszPacket++;
		
	INT32 lLength = atoi(sz);

	// skip '|'
	pszPacket++;

	char szKey[21];
	ZeroMemory(szKey, sizeof(szKey));
	char *pszKey = szKey;
	while ('|' != *pszPacket)
		*pszKey++ = *pszPacket++;

	INT32 lKey = atoi(szKey);

	// find AgsdBilling
	AgsdBilling *pAgsdBilling = Get(lKey);
	if (NULL == pAgsdBilling)
		{
		return FALSE;
		}

	// skip '|'
	pszPacket++;
	
	char szFormat[256];
	ZeroMemory(szFormat, sizeof(szFormat));
	strncpy(szFormat, pszPacket, lLength);

	if (FALSE == pAgsdBilling->Decode(szFormat, lCode))
		return FALSE;
		
	if (AGSMBILLING_TYPE_MONEY == pAgsdBilling->Type())
		EnumCallback(AGSMBILLING_CB_RESULT_GETMONEY, pAgsdBilling, pAgsdBilling->m_pAgpdCharacter);
	else if (AGSMBILLING_TYPE_ITEM == pAgsdBilling->Type())
		EnumCallback(AGSMBILLING_CB_RESULT_BUYITEM, pAgsdBilling, pAgsdBilling->m_pAgpdCharacter);
	
	m_GenerateID.AddRemoveID(pAgsdBilling->m_lID);
	Remove(pAgsdBilling->m_lID);
	pAgsdBilling->Release();
	
	return TRUE;
	}

//	Connection
//==========================================================
//

eAGSMBILLING_CONNECT_RESULT AgsmBilling::ConnectBillingServer()
{
	//JK_빌링
	return (this->*ConnectBillingServerPtr)();
/*
#ifdef _AREA_KOREA_
	return ConnectBillingServerKr();
#else
	return AGSMBILLING_CONNECT_RESULT_SUCCESS;
#endif
*/
}

BOOL AgsmBilling::CheckConnectBillingServer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	if (NULL == pClass)
		return FALSE;

	AgsmBilling *pThis = (AgsmBilling *) pClass;

	if (NULL == pThis->m_pAgsdServerBilling)
		return FALSE;

	if (pThis->m_pAgsdServerBilling->m_bIsConnected == FALSE)
	{
		eAGSMBILLING_CONNECT_RESULT eResult = pThis->ConnectBillingServer();
		if (AGSMBILLING_CONNECT_RESULT_SUCCESS == eResult ||
			AGSMBILLING_CONNECT_RESULT_NEED_WAIT == eResult)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

BOOL AgsmBilling::CheckBillingServer()
{
	return TRUE;
}
//
BOOL AgsmBilling::OnDisconnectKr()
{
	if (NULL != m_pAgsdServerBilling)
	{
		m_pAgsdServerBilling->m_bIsConnected = FALSE;
		if (!AgsModule::AddTimer(3000, m_pAgsdServerBilling->m_lServerID, this, CheckConnectBillingServer, m_pAgsdServerBilling))
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmBilling::OnDisconnectJp()
{
	return TRUE;	
}

BOOL AgsmBilling::OnDisconnectCn()
{
	return TRUE;	
}


//	Billing methods
//=========================================================================
//
//	!!! UNICODE기반으로 소스가 바뀔 경우 빌링서버의 프로토콜 포맷이 바뀌지
//	않으면 multi-byte 코드로 수정해서 보내야 한다.
//


BOOL AgsmBilling::SendBuyCashItem(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList)
{ 
	return (this->*SendBuyCashItemPtr)(pAgpdCharacter, lProductID, pszDesc, llMoney, sList);
}