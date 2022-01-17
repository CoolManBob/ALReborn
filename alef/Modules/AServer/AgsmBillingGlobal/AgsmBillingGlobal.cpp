#include "AgsmBillingGlobal.h"
#ifdef _WEBZEN_BILLING_
#include "WebzenBilling_Kor.h"
#else
#include "WebzenBilling.h"
#endif
CWebzenShop* m_pWZShop = NULL;
CWebzenBilling* m_pWZBilling = NULL;


#ifdef _WEBZEN_BILLING_

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD_x64_KOR.lib") 
	#else
		#pragma comment(lib, "WebzenBilling_x64_KOR.lib") 
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD_KOR.lib") 
	#else
		#pragma comment(lib, "WebzenBilling_KOR.lib") 
	#endif
#endif

#else

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD_x64.lib") 
	#else
		#pragma comment(lib, "WebzenBilling_x64.lib") 
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD.lib") 
	#else
		#pragma comment(lib, "WebzenBilling.lib") 
	#endif
#endif

#endif //_WEBZEN_BILLING_

#ifdef _WEBZEN_BILLING_
const DWORD WZBILLING_GAMECODE = 822;
const DWORD WZBILLING_SALESZONE = 825;
const DWORD WZBILLING_SERVERTYPE = 823; // ¾ÆÅ©·Îµå ±Û·Î¹ú
#else
const DWORD WZBILLING_GAMECODE = 417;
const DWORD WZBILLING_SALESZONE = 428;
const DWORD WZBILLING_SERVERTYPE = 686; // ¾ÆÅ©·Îµå ±Û·Î¹ú
#endif

AgsmBillingGlobal::AgsmBillingGlobal()
{
}

AgsmBillingGlobal::~AgsmBillingGlobal()
{
	if( m_pWZShop )
		delete m_pWZShop;

	if( m_pWZBilling )
		delete m_pWZBilling;
}

BOOL AgsmBillingGlobal::Initialize()
{
	m_AdminGUID.SetCount(10000);
	if (!m_AdminGUID.InitializeObject(sizeof(AgpdBillingGlobal*), m_AdminGUID.GetCount()))
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	//
	m_pWZShop = new CWebzenShop;

	if(!m_pWZShop)
		return FALSE;

	m_pWZShop->SetCallbackOnLog(AgsmBillingGlobal::OnLog);
	m_pWZShop->SetCallbackOnInquireCash(AgsmBillingGlobal::OnInquireCash);
	m_pWZShop->SetCallbackOnBuyProduct(AgsmBillingGlobal::OnBuyProduct);
	//JK_À¥Á¨ºô¸µ
	m_pWZShop->SetCallbackOnNetConnect(AgsmBillingGlobal::OnNetConnectShop);
	m_pWZShop->SetCallbackOnInquireSalesZoneScriptVersion(AgsmBillingGlobal::OnInquireSalesZoneScriptVersion);
	m_pWZShop->SetCallbackOnUpdateVersion(AgsmBillingGlobal::OnUpdateVersion);
	m_pWZShop->SetCallbackOnUseStorage(AgsmBillingGlobal::OnUseStorage);	
	m_pWZShop->SetCallbackOnInquireStorageListPageNoGiftMessage(AgsmBillingGlobal::OnInquireStorageListPageNoGiftMessage);
	m_pWZShop->SetCallbackOnRollbackUseStorage(AgsmBillingGlobal::OnRollbackUseStorage);
	m_pWZShop->Initialize();

	//////////////////////////////////////////////////////////////////////////
	//
	m_pWZBilling = new CWebzenBilling;
	if(!m_pWZBilling)
		return FALSE;

	m_pWZBilling->SetCallbackOnLog(AgsmBillingGlobal::OnLog);
	m_pWZBilling->SetCallbackOnUserStatus(AgsmBillingGlobal::OnUserStatus);
	m_pWZBilling->SetCallbackOnInquirePersonDeduct(AgsmBillingGlobal::OnInquirePersonDeduct);
	m_pWZBilling->SetCallbackOnInquirePCRoomPoint(AgsmBillingGlobal::OnInquirePCRoomPoint);
	m_pWZBilling->Initialize();

	return TRUE;
}

BOOL AgsmBillingGlobal::OnIdle()
{
	eWZConnect eStatusShop = m_pWZShop->GetStatus();
	if(eStatusShop == eNotConnect)
		ConnectShop();

	eWZConnect eStatusBilling = m_pWZBilling->GetStatus();
	if(eStatusBilling == eNotConnect)
		ConnectBilling();

	return TRUE;
}

BOOL AgsmBillingGlobal::ConnectShop()
{
	AuXmlDocument pDoc;
	if(!pDoc.LoadFile("WebzenBilling.xml"))
		return FALSE;

	AuXmlNode* pFirstNode = pDoc.FirstChild("WebzenBilling");
	if(!pFirstNode)
		return FALSE;

	AuXmlElement* pShop = pFirstNode->FirstChildElement("Shop");
	if(!pShop)
		return FALSE;

	AuXmlElement* pNodeIP	= pShop->FirstChildElement("IPAddress");
	AuXmlElement* pNodePort	= pShop->FirstChildElement("Port");

	if(!pNodeIP || !pNodePort)
		return FALSE;

	CHAR* pIPAddress	= (CHAR*)pNodeIP->GetText();
	CHAR* pPort			= (CHAR*)pNodePort->GetText();

	if(!pIPAddress || !pPort)
		return FALSE;

	m_pWZShop->Connect(pIPAddress, atoi(pPort));

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "ConnectShop : pIPAddress = %s, pPort = %s", pIPAddress, pPort);
		OnLog(strLog);
	}

	return TRUE;
}

BOOL AgsmBillingGlobal::ConnectBilling()
{
	AuXmlDocument pDoc;
	if(!pDoc.LoadFile("WebzenBilling.xml"))
		return FALSE;

	AuXmlNode* pFirstNode = pDoc.FirstChild("WebzenBilling");
	if(!pFirstNode)
		return FALSE;

	AuXmlElement* pBilling = pFirstNode->FirstChildElement("Billing");
	if(!pBilling)
		return FALSE;

	AuXmlElement* pNodeIP	= pBilling->FirstChildElement("IPAddress");
	AuXmlElement* pNodePort	= pBilling->FirstChildElement("Port");

	if(!pNodeIP || !pNodePort)
		return FALSE;

	CHAR* pIPAddress	= (CHAR*)pNodeIP->GetText();
	CHAR* pPort			= (CHAR*)pNodePort->GetText();

	if(!pIPAddress || !pPort)
		return FALSE;

	m_pWZBilling->Connect(pIPAddress, atoi(pPort));

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "ConnectBilling : pIPAddress = %s, pPort = %s", pIPAddress, pPort);
		OnLog(strLog);
	}

	return TRUE;
}

//JK_À¥Á¨ºô¸µ
void AgsmBillingGlobal::OnNetConnectShop(PVOID pOnNetConnect)
{
	stOnNetConnect* pNetConnect = (stOnNetConnect*)pOnNetConnect;
	if(pNetConnect->success)
	{
		m_pWZShop->InquireSalesZoneScriptVersion(WZBILLING_GAMECODE, WZBILLING_SALESZONE);
	}

}

void AgsmBillingGlobal::OnLog( char* strLog )
{
	printf("%s\n", strLog);
	AuLogFile_s("LOG\\WZBilling.txt", strLog);
}
//JK_À¥Á¨ºô¸µ
void AgsmBillingGlobal::OnInquireSalesZoneScriptVersion( PVOID pInquire )
{
	stSalesZoneScriptVersion* pSalesZoneScriptVersion = (stSalesZoneScriptVersion*)pInquire;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnInquireSalesZoneScriptVersion : GameCode : %d , Result = %d, Year = %d, YearIndentity = %d", 
			pSalesZoneScriptVersion->SalesZone, pSalesZoneScriptVersion->ResultCode, pSalesZoneScriptVersion->Year, pSalesZoneScriptVersion->YaerIdentity);
		OnLog(strLog);
	}

	if(pSalesZoneScriptVersion->ResultCode != 0 || pSalesZoneScriptVersion->SalesZone != WZBILLING_SALESZONE)
	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "ERROR!! OnInquireSalesZoneScriptVersion SalesZone : %d , Result : %d", 
			pSalesZoneScriptVersion->SalesZone, pSalesZoneScriptVersion->ResultCode);
		OnLog(strLog);
	}


	AgsmBilling::GetInstance()->SetSalesZoneScriptVersion(pSalesZoneScriptVersion->Year, pSalesZoneScriptVersion->YaerIdentity);

}

//JK_À¥Á¨ºô¸µ
void AgsmBillingGlobal::OnUpdateVersion( PVOID pInquire )
{
	stSalesZoneScriptVersion* pSalesZoneScriptVersion = (stSalesZoneScriptVersion*)pInquire;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnUpdateVersion : GameCode : %d ,SalesZoneCode : %d,  Result = %d, Year = %d, YearIndentity = %d", 
			pSalesZoneScriptVersion->GameCode, pSalesZoneScriptVersion->SalesZone, pSalesZoneScriptVersion->ResultCode, pSalesZoneScriptVersion->Year, pSalesZoneScriptVersion->YaerIdentity);
		OnLog(strLog);
	}

	if(pSalesZoneScriptVersion->GameCode != WZBILLING_GAMECODE || pSalesZoneScriptVersion->SalesZone != WZBILLING_SALESZONE)
	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "ERROR!! OnUpdateVersion Wrong GameCode(%d != %d) or SalesZone(%d != %d)", 
			WZBILLING_GAMECODE, pSalesZoneScriptVersion->GameCode,WZBILLING_SALESZONE, pSalesZoneScriptVersion->SalesZone);
		OnLog(strLog);
		return;
	}

	AgsmBilling::GetInstance()->UpdateScriptVersionAllUser(pSalesZoneScriptVersion->Year, pSalesZoneScriptVersion->YaerIdentity);

}


BOOL AgsmBillingGlobal::InquireCash( CHAR* szAccountID )
{
	return m_pWZShop->InquireCash(1, 0, szAccountID, WZBILLING_GAMECODE, false); // archlord gamecode = 417
}
//JK_À¥Á¨ºô¸µ
BOOL AgsmBillingGlobal::InquireCash( DWORD dwAccountGUID )
{
	return m_pWZShop->InquireCash(WZBILLING_GAMECODE, 0, dwAccountGUID, true, 0 ); // archlord gamecode = 417
}

void AgsmBillingGlobal::OnInquireCash( PVOID pInquire )
{
	stInquire* pInquireResult = (stInquire*)pInquire;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnInquire : Result = %d, AccountSeq = %d, AccountID = %s, WCoinSum = %d, PCoinSum = %d", 
			pInquireResult->ResultCode, pInquireResult->AccountSeq, pInquireResult->AccountID, (int)pInquireResult->WCoinSum, (int)pInquireResult->PCoinSum);
		OnLog(strLog);
	}
#ifdef _WEBZEN_BILLING_
	AgsmBilling::GetInstance()->OnInquireCash(pInquireResult->ResultCode, pInquireResult->AccountSeq, pInquireResult->WCoinSum);
#else
	AgsmBilling::GetInstance()->OnInquireCash(pInquireResult->ResultCode, pInquireResult->AccountID, pInquireResult->WCoinSum, pInquireResult->PCoinSum);
#endif
}

BOOL AgsmBillingGlobal::BuyProduct(CHAR* AccountID, INT32 ProductID, CHAR* ProductName, UINT64 ListSeq, INT32 Class, INT32 Level, CHAR* szCharName, INT32 ServerIndex, double Price, INT32 lType )
{
	return m_pWZShop->BuyProduct(0, AccountID, WZBILLING_GAMECODE, ProductID, ProductName, ListSeq, Class, Level, szCharName, ServerIndex, WZBILLING_SALESZONE, Price, 'C', lType);
}
//JK_À¥Á¨ºô¸µ
BOOL AgsmBillingGlobal::BuyProduct(DWORD dwAccountSeq, INT32 ProductSeq, INT32 DisplaySeq, INT32 PriceSeq, INT32 Class, INT32 Level, CHAR* szCharName, CHAR* szRank,  INT32 ServerIndex)
{
	return m_pWZShop->BuyProduct(WZBILLING_GAMECODE, dwAccountSeq, ProductSeq, DisplaySeq, WZBILLING_SALESZONE, PriceSeq, Class, Level, szCharName, szRank, ServerIndex);
}

void AgsmBillingGlobal::OnBuyProduct(PVOID pBuyProduct)
{
#ifdef _WEBZEN_BILLING_
	stBuyProduct_Union* pstBuyProduct = (stBuyProduct_Union*)pBuyProduct;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnBuyProduct : AccountSeq = %d,ResultCode = %d, LeftProductCount : %d", 
			pstBuyProduct->AccountSeq, pstBuyProduct->ResultCode, pstBuyProduct->LeftProductCount);
		OnLog(strLog);
	}

	AgsmBilling::GetInstance()->OnBuyProductWebzen(pstBuyProduct->AccountSeq, pstBuyProduct->ResultCode, pstBuyProduct->LeftProductCount);

#else
	stBuyProduct* pstBuyProduct = (stBuyProduct*)pBuyProduct;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnBuyProduct : ResultCode = %d, AccountSeq = %d, AccountID = %s, DeductCashSeq = %I64d, InGamePurchaseSeq = %I64d", 
			pstBuyProduct->ResultCode, pstBuyProduct->AccountSeq, pstBuyProduct->AccountID, pstBuyProduct->DeductCashSeq, pstBuyProduct->InGamePurchaseSeq);
		OnLog(strLog);
	}

	AgsmBilling::GetInstance()->OnBuyProduct(pstBuyProduct->ResultCode, pstBuyProduct->AccountID, pstBuyProduct->DeductCashSeq);
#endif
}
//JK_À¥Á¨ºô¸µ
BOOL AgsmBillingGlobal::UseStorage(DWORD AccountSeq, CHAR* szIP, DWORD StorageSeq, DWORD StorageItemSeq, INT32 Class, INT32 Level, CHAR* CharName, CHAR* Rank, INT32 ServerIndex)
{
	return m_pWZShop->UseStorage(AccountSeq, WZBILLING_GAMECODE, inet_addr(szIP), StorageSeq, StorageItemSeq, 'P',Class, Level, CharName, Rank,ServerIndex);
}

void AgsmBillingGlobal::OnUseStorage( PVOID pUseStorage)
{
	stUseStorage* pstUseStorage = (stUseStorage*)pUseStorage;
	
	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnUseStorage : AccountSeq = %d,ResultCode = %d, InGameProductID : %s, PropertyCount : %d, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d, ", 
			pstUseStorage->AccountSeq, pstUseStorage->ResultCode, pstUseStorage->InGameProductID, pstUseStorage->PropertyCount, pstUseStorage->ProductSeq, pstUseStorage->StorageSeq, pstUseStorage->StorageItemSeq);
		OnLog(strLog);
	}
	
	AgsmBilling::GetInstance()->OnUseStorage(pstUseStorage->AccountSeq, pstUseStorage->ResultCode, pstUseStorage->InGameProductID, pstUseStorage->PropertyCount, pstUseStorage->ProductSeq, pstUseStorage->StorageSeq, pstUseStorage->StorageItemSeq, pstUseStorage->PropertyList);


}
//JK_À¥Á¨ºô¸µ
BOOL AgsmBillingGlobal::InquireStorageList(DWORD AccountSeq, INT32 NowPage)
{
	return m_pWZShop->InquireStorageListPageNoGiftMessage(AccountSeq, WZBILLING_GAMECODE, WZBILLING_SALESZONE, 'A', NowPage,STORAGELISTCOUNT_PER_PAGE);
}
//JK_À¥Á¨ºô¸µ
void AgsmBillingGlobal::OnInquireStorageListPageNoGiftMessage( PVOID pStorageList )
{
	stStorageListPageNoGiftMessage* pstStorageList = (stStorageListPageNoGiftMessage*)pStorageList;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnInquireStorageListPageBoGiftMessage : AccountSeq = %d,ResultCode = %d, StorageType : %c, NowPage : %d, TotalPage: %d, TotalCount : %d, ListCount : %d, ", 
			pstStorageList->AccountID, pstStorageList->ResultCode, pstStorageList->StorageType, pstStorageList->NowPage, pstStorageList->TotalPage, pstStorageList->TotalCount, pstStorageList->ListCount);
		OnLog(strLog);
	}

	AgsmBilling::GetInstance()->OnInquireStorageListPageGiftMessage(pstStorageList->AccountID, pstStorageList->ResultCode, pstStorageList->StorageType, pstStorageList->NowPage, pstStorageList->TotalPage, pstStorageList->TotalCount, pstStorageList->ListCount, pstStorageList->StorageList);
	
}
//JK_À¥Á¨ºô¸µ 
BOOL AgsmBillingGlobal::RollbackUseStorage(DWORD AccountSeq, DWORD StorageSeq, DWORD StorageItemSeq)
{
	if( ! m_pWZShop->RollbackUseStorage(AccountSeq, WZBILLING_GAMECODE, StorageSeq, StorageItemSeq))
	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "RollbackUseStorage Function FAIL!!: AccountSeq = %d,StorageSeq = %d, StorageItemSeq = %d ", 
			AccountSeq, StorageSeq, StorageItemSeq);
		OnLog(strLog);

		return FALSE;
	}
	return TRUE;
}

void AgsmBillingGlobal::OnRollbackUseStorage( PVOID pRollbackUseStorage )
{
	stRollbackUseStorage* pstRollbackUseStorage = (stRollbackUseStorage*)pRollbackUseStorage;
	
	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnRollbackUseStorage : AccountSeq = %d,ResultCode = %d ", 
			pstRollbackUseStorage->AccountSeq, pstRollbackUseStorage->ResultCode);
		OnLog(strLog);
	}
	
}

void AgsmBillingGlobal::OnUserStatus( PVOID UserStatus )
{
	stUserStatus* pUserStatus = (stUserStatus*)UserStatus;

	/*pUserStatus->dwAccountGUID;
	pUserStatus->dwBillingGUID;
	pUserStatus->RealEndDate;
	pUserStatus->EndDate;
	pUserStatus->dRestPoint;
	pUserStatus->dRestTime;
	pUserStatus->nDeductType;
	pUserStatus->nAccessCheck;
	pUserStatus->nResultCode;*/

	if(pUserStatus->nResultCode == 0 && pUserStatus->dwBillingGUID > 0 )
	{
		{
			CTime tEnd = CTime(pUserStatus->EndDate);
			std::string str = tEnd.Format("%Y-%m-%d %H:%M:%S");

			CHAR strLog[256] = { 0, };
			sprintf_s(strLog, sizeof(strLog), "OnUserStatus : nResultCode = %d, nDeductType = %d, dwAccountGUID = %d, dwBillingGUID = %d, nDeductType = %d, EndDate = %s", 
				pUserStatus->nResultCode, pUserStatus->nDeductType,
				pUserStatus->dwAccountGUID, pUserStatus->dwBillingGUID, pUserStatus->nDeductType, str.c_str());
			OnLog(strLog);
		}
#ifdef _WEBZEN_BILLING_
		AgsmBilling::GetInstance()->OnUserStatusWebzen(pUserStatus->dwAccountGUID, pUserStatus->dwBillingGUID, pUserStatus->EndDate, pUserStatus->dRestTime, pUserStatus->nDeductType);
#else
		AgsmBilling::GetInstance()->OnUserStatus(pUserStatus->dwAccountGUID, pUserStatus->dwBillingGUID, pUserStatus->EndDate, pUserStatus->dRestTime, pUserStatus->nDeductType);
#endif //_WEBZEN_BILLING_
	}

}

void AgsmBillingGlobal::OnInquirePersonDeduct( PVOID InquirePersonDeduct )
{
	stInquirePersonDeduct* pInquirePersonDeduct = (stInquirePersonDeduct*)InquirePersonDeduct;

	AgsmBilling::GetInstance()->OnInquirePersonDeduct(pInquirePersonDeduct->AccountGUID, pInquirePersonDeduct->ResultCode);
}

BOOL AgsmBillingGlobal::InquirePersonDeduct( DWORD AccountGUID )
{
	if( m_pWZBilling )
		m_pWZBilling->InquirePersonDeduct(AccountGUID, WZBILLING_GAMECODE);

	return TRUE;
}
//JK_PC¹æ°ú±Ý»óÅÂÈ®ÀÎ
void AgsmBillingGlobal::OnInquirePCRoomPoint( PVOID pInquirePCRoomPoint )
{
	stInquirePCRoomPoint* pstInquirePCRoomPoint = (stInquirePCRoomPoint*)pInquirePCRoomPoint;

	if(pstInquirePCRoomPoint->ResultCode == -2)
	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "Error__OnInquirePCRoomPoint : AccountGUID = %d, PCRoomGUID = %d, GAMECODE + %d. ResultCode = %d ", 
			pstInquirePCRoomPoint->AccountGUID, pstInquirePCRoomPoint->RoomGUID, pstInquirePCRoomPoint->GameCode, pstInquirePCRoomPoint->ResultCode);
		OnLog(strLog);
		return;
	}

	if(pstInquirePCRoomPoint->ResultCode == 1 && pstInquirePCRoomPoint->RoomGUID) //PC¹æÀÌ°í °ú±ÝÀÌ º¸À¯ »óÅÂ¶ó¸é...
	{

		AgsmBilling::GetInstance()->ReloginWebzen(pstInquirePCRoomPoint->AccountGUID);
		
	}

}

BOOL AgsmBillingGlobal::AddUser(AgpdCharacter* pcsCharacter, DWORD AccountGUID)
{
	AgpdBillingGlobal* pBillingUser = new AgpdBillingGlobal;
	if(!pBillingUser)
		return FALSE;

	pBillingUser->pcsCharacter = pcsCharacter;
	pBillingUser->AccountGUID = AccountGUID;

	m_AdminGUID.AddObject(&pBillingUser, AccountGUID);

	return TRUE;
}

BOOL AgsmBillingGlobal::RemoveUser(DWORD AccountGUID)
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return FALSE;

	m_AdminGUID.RemoveObject(AccountGUID);

	delete (*pBillingUser);

	return TRUE;
}

AgpdCharacter* AgsmBillingGlobal::GetCharacter( DWORD AccountGUID )
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return NULL;

	return (*pBillingUser)->pcsCharacter;
}

BOOL AgsmBillingGlobal::SetBillingGUID(DWORD AccountGUID, DWORD BillingGUID)
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return FALSE;

	(*pBillingUser)->dwBillingGUID = BillingGUID;

	return TRUE;
}
//JK_À¥Á¨ºô¸µ
DWORD AgsmBillingGlobal::GetBillingGUID(DWORD AccountGUID)
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return FALSE;

	return (*pBillingUser)->dwBillingGUID;
}

BOOL AgsmBillingGlobal::Login( DWORD AccountGUID, CHAR* szIP )
{
	if( m_pWZBilling )
		m_pWZBilling->UserLogin(AccountGUID, inet_addr(szIP), 0, WZBILLING_GAMECODE, WZBILLING_SERVERTYPE);

	return TRUE;
}
//JK_À¥Á¨ºô¸µ
BOOL AgsmBillingGlobal::LoginWebzen( DWORD AccountGUID, CHAR* szIP, DWORD PCRoomGUID)
{
	if( m_pWZBilling )
		m_pWZBilling->UserLogin(AccountGUID, inet_addr(szIP), PCRoomGUID, WZBILLING_GAMECODE, WZBILLING_SERVERTYPE);

	return TRUE;
}

void AgsmBillingGlobal::LogOut( DWORD AccountGUID )
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return;

	DWORD dwBillingGUID = (*pBillingUser)->dwBillingGUID;

	if( m_pWZBilling && dwBillingGUID > 0)
		m_pWZBilling->UserLogout(dwBillingGUID);
}
//JK_À¥Á¨ºô¸µ : ¼ýÀÚÀÇ ¹®ÀÚ¿­ÀÎ°¡?
BOOL AgsmBillingGlobal::IsNumberFromItemID( char* pData, int nSize )
{
	for(int i = 0 ; i < nSize; i++)
	{
		if(isdigit(pData[i]) == false)
			return FALSE;
	}
	return TRUE;
}
//JK_PC¹æ°ú±Ý»óÅÂÈ®ÀÎ
BOOL AgsmBillingGlobal::SetPCRoomGUID(DWORD AccountGUID, DWORD PCRoomGUID)
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return FALSE;

	(*pBillingUser)->dwPCRoomGUID = PCRoomGUID;

	return TRUE;
}
//JK_À¥Á¨ºô¸µ
DWORD AgsmBillingGlobal::GetPCRoomGUID(DWORD AccountGUID)
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return FALSE;

	return (*pBillingUser)->dwPCRoomGUID;
}

//JK_À¥Á¨ºô¸µ
void AgsmBillingGlobal::CheckPCRoomPointAllUser()
{
	INT32 lIndex = 0;
	AgpdBillingGlobal** ppBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObjectSequence(&lIndex);
	while(ppBillingUser && *ppBillingUser)
	{
		if ( (*ppBillingUser)->dwPCRoomGUID && !(*ppBillingUser)->dwBillingGUID )
		{
			if( m_pWZBilling )
				m_pWZBilling->InquirePCRoomPoint( (*ppBillingUser)->AccountGUID, (*ppBillingUser)->dwPCRoomGUID, WZBILLING_GAMECODE);

		}

		ppBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObjectSequence(&lIndex);
	}
}