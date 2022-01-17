// ShopClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#include "ShopClient.h"

#pragma comment(lib, "Ws2_32.lib")

bool bInquire = false;
bool bBuyProduct = false;
bool bScriptVersion = false;
bool bStorageList = false;

long BILLING_dwInquireResult = -1;
bool BILLING_bLogin = false;
long BILLING_dwBillingGUID = -1;

//////////////////////////////////////////////////////////////////////////
//
const DWORD GameCode = 822;//417; // 아크로드 글로벌
const DWORD SalesZone = 825;//428; // 아크로드 글로벌 - 게임
const DWORD ServerType = 823; // 아크로드 글로벌

CWebzenBilling* m_pWZBilling = NULL;
CWebzenShop* m_pWZShop = NULL;

void OnInquire(void* Inquire)
{
	stInquire* pInquire = (stInquire*)Inquire;
	printf("OnInquire\n");
	printf("Result = %d\n", pInquire->ResultCode);
	printf("AccountSeq = %d\n", pInquire->AccountSeq);
	printf("AccountID = %s\n", pInquire->AccountID);
	printf("WCoinSum = %d, PCoinSum = %d \n", (int)pInquire->WCoinSum, (int)pInquire->PCoinSum);

	bInquire = true;
}

void OnLog(char* strLog)
{
	printf("%s\n", strLog);
}

void OnBuyProduct(void* BuyProduct)
{
	stBuyProduct* pBuyProduct = (stBuyProduct*)BuyProduct;
	printf("ResultCode = %d\n", pBuyProduct->ResultCode);
	printf("AccountSeq = %d\n", pBuyProduct->AccountSeq);
	printf("AccountID = %s\n", pBuyProduct->AccountID);
	printf("DeductCashSeq = %d\n", pBuyProduct->DeductCashSeq);
	printf("InGamePurchaseSeq = %d\n", pBuyProduct->InGamePurchaseSeq);	

	bBuyProduct = true;
}

void OnInquireSalesZoneScriptVersion(void* SalesZoneScriptVersion)
{
	stSalesZoneScriptVersion* pSalesZoneScriptVersion = (stSalesZoneScriptVersion*)SalesZoneScriptVersion;
	printf("GameCode = %d\n", pSalesZoneScriptVersion->GameCode);	
	printf("SalesZone = %d\n", pSalesZoneScriptVersion->SalesZone);
	printf("Year = %d\n", pSalesZoneScriptVersion->Year);
	printf("YaerIdentity = %d\n", pSalesZoneScriptVersion->YaerIdentity);
	printf("ResultCode = %d\n", pSalesZoneScriptVersion->ResultCode);
	

	bScriptVersion = true;
}

void OnInquireStorageListPageNoGiftMessage(void* StorageListPageNoGiftMessage)
{
	stStorageListPageNoGiftMessage* pStorageListPageNoGiftMessage = (stStorageListPageNoGiftMessage*)StorageListPageNoGiftMessage;

	bStorageList = true;

}

void OnUseStorage(void* UseStorage)
{
	stUseStorage* pUseStorage = (stUseStorage*)UseStorage;

	bStorageList = true;

}

void OnRollbackUseStorage(void* RollbackUseStorage)
{
	stRollbackUseStorage* pRollbackUseStorage = (stRollbackUseStorage*)RollbackUseStorage;

	bStorageList = true;

}

/*void OnInquirePCRoomPoint(PVOID InquirePCRoomPoint)
{
	stInquirePCRoomPoint* pInquirePCRoomPoint = (stInquirePCRoomPoint*)InquirePCRoomPoint;

}*/

void OnUserStatus(PVOID UserStatus)
{
	stUserStatus* pUserStatus = (stUserStatus*)UserStatus;

	pUserStatus->dwAccountGUID;
	pUserStatus->dwBillingGUID;
	pUserStatus->RealEndDate;
	pUserStatus->EndDate;
	pUserStatus->dRestPoint;
	pUserStatus->dRestTime;
	pUserStatus->nDeductType;
	pUserStatus->nAccessCheck;
	pUserStatus->nResultCode;

	if(pUserStatus->nResultCode == 0 && pUserStatus->nAccessCheck == 0 && pUserStatus->nDeductType != 0)
	{
		printf("Success\n");

		BILLING_dwBillingGUID = pUserStatus->dwBillingGUID;
		BILLING_bLogin = true;
	}
}
void OnNetConnect(PVOID NetConnect)
{
	stOnNetConnect* pOnNetConnect = (stOnNetConnect*)NetConnect;
	if(pOnNetConnect->success)
	{
		 m_pWZShop->InquireSalesZoneScriptVersion(GameCode,SalesZone);
	}
}
/*void OnInquireMultiUser(PVOID InquireMultiUser)
{
	stInquireMultiUser* pInquireMultiUser = (stInquireMultiUser*)InquireMultiUser;

}*/

void OnInquirePersonDeduct(PVOID InquirePersonDeduct)
{
	stInquirePersonDeduct* pInquirePersonDeduct = (stInquirePersonDeduct*)InquirePersonDeduct;

	//pInquirePersonDeduct->AccountGUID;
	//pInquirePersonDeduct->GameCode;
	
	BILLING_dwInquireResult = pInquirePersonDeduct->ResultCode;
	

	if(BILLING_dwInquireResult == 1 && BILLING_bLogin == false)
	{
		m_pWZBilling->UserLogin(71618, inet_addr("127.0.0.1"), 0, GameCode, ServerType);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//////////////////////////////////////////////////////////////////////////////////////
	//
	m_pWZBilling = new CWebzenBilling;
	m_pWZBilling->SetCallbackOnLog(OnLog);
	//m_pWZBilling->SetCallbackOnInquirePCRoomPoint(OnInquirePCRoomPoint);
	m_pWZBilling->SetCallbackOnUserStatus(OnUserStatus);
	//m_pWZBilling->SetCallbackOnInquireMultiUser(OnInquireMultiUser);
	m_pWZBilling->SetCallbackOnInquirePersonDeduct(OnInquirePersonDeduct);
	m_pWZBilling->Initialize();

	while(true)
	{
		Sleep(2000);

		if(m_pWZBilling->GetStatus() == eConnecting)
			continue;

		if(m_pWZBilling->GetStatus() == eNotConnect)
		{
			//m_pWZBilling->Connect("218.234.76.11", 45610); // 테스트 Billing 서버
			m_pWZBilling->Connect("218.234.76.11", 45628); // 테스트 Billing 서버
			continue;
		}

		//m_pWZBilling->UserLogin(802557, inet_addr("127.0.0.1"), 39562, GameCode, ServerType);

		break;

//		if(BILLING_bLogin == false)
//		{
//			m_pWZBilling->InquirePersonDeduct(71618, GameCode);
//		}
//		else
//		{

//		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//
	m_pWZShop = new CWebzenShop;
	m_pWZShop->SetCallbackOnLog(OnLog);
	m_pWZShop->SetCallbackOnInquireCash(OnInquire);
	m_pWZShop->SetCallbackOnBuyProduct(OnBuyProduct);
	m_pWZShop->SetCallbackOnInquireSalesZoneScriptVersion(OnInquireSalesZoneScriptVersion);
	m_pWZShop->SetCallbackOnInquireStorageListPageNoGiftMessage(OnInquireStorageListPageNoGiftMessage);
	m_pWZShop->SetCallbackOnUseStorage(OnUseStorage);
	m_pWZShop->SetCallbackOnRollbackUseStorage(OnRollbackUseStorage);
	m_pWZShop->SetCallbackOnNetConnect(OnNetConnect);

	m_pWZShop->Initialize();

	while(TRUE)
	{
		Sleep(100);

		if(m_pWZShop->GetStatus() == eConnecting)
			continue;
		else if(m_pWZShop->GetStatus() == eNotConnect)
		{
			//m_pWZShop->Connect("218.234.76.11", 45611); // 테스트 샾서버
			m_pWZShop->Connect("218.234.76.11", 45627); // 테스트 샾서버
			//m_pWZShop->Connect("172.18.1.131", 45611); // 리얼 샾서버
		}
		else
			break;		
	}

	m_pWZShop->InquireSalesZoneScriptVersion(GameCode,SalesZone);

	m_pWZShop->InquireStorageListPageNoGiftMessage(802557,GameCode,SalesZone,'S',1,7);

	//m_pWZShop->InquireCash(1, 0, "ehtest01", GameCode, false);
	m_pWZShop->InquireCash( GameCode, 0, 802557, true, 0 );

	m_pWZShop->UseStorage(802557,GameCode,888888,7,7,'A',2,30,"Fuck","Low",1);

	m_pWZShop->RollbackUseStorage(802557,GameCode, 7,7);


	while(!bInquire)
	{
		Sleep(100);
	}

	DWORD InGamePurchaseSeq = 12340000;
	BOOL IsPPCardCash = TRUE;
	double DeductPrice = 10;
		
	//m_pWZShop->BuyProduct(GameCode, 802557, DWORD ProductSeq, DWORD DisplaySeq, SalesZone, DWORD PriceSeq, 1, 1,"Bill" , "Low", 1)

	while(!bBuyProduct)
	{
		Sleep(100);
	}

	bInquire = FALSE;
	m_pWZShop->InquireCash(0, 0, "123456789012345678901", GameCode, false);
	
	while(!bInquire)
	{
		Sleep(100);
	}

	return 0;
}

