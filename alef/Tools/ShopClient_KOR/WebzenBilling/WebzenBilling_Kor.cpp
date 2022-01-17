#include <Winsock2.h>
#include <Windows.h>

#include "WebzenBilling_Kor.h"
#include "LibClientSession.h"
#include <atlstr.h>
#include <atltime.h>

CLibClientSessionShop* g_LibClientSessionShop = NULL;
CLibClientSessionBilling* g_LibClientSessionBilling = NULL;

CWebzenShop::CWebzenShop()
{
	g_LibClientSessionShop = new CLibClientSessionShop;
}

CWebzenShop::~CWebzenShop()
{
	if(g_LibClientSessionShop)
		delete g_LibClientSessionShop;
}

BOOL CWebzenShop::Initialize()
{
	if(!g_LibClientSessionShop)
		return FALSE;

	if(!g_LibClientSessionShop->CreateSession())
		return FALSE;

	return TRUE;
}

eWZConnect CWebzenShop::Connect(char* dest, unsigned short port)
{
	g_LibClientSessionShop->m_bConnect = eConnecting;
	
	CTime tTime = CTime::GetCurrentTime();

	g_LibClientSessionShop->Connect(dest, port);

	/*while( eConnecting == g_LibClientSession->m_bConnect )
	{
		if(tTime + CTimeSpan(0, 0, 0, 10) < CTime::GetCurrentTime())
			return FALSE;

		Sleep(100);
	}

	return (g_LibClientSession->m_bConnect == eConnected);*/
	return eConnecting;
}

BOOL CWebzenShop::InquireCash( BYTE ViewType, DWORD AccountSeq, CHAR* AccountID, DWORD GameCode, BOOL SumOnly )
{
	//return g_LibClientSessionShop->InquireCash(ViewType, AccountSeq, AccountID, GameCode, SumOnly);
	return FALSE;
}
//JK_À¥Á¨ºô¸µ
BOOL CWebzenShop::InquireCash( DWORD GameCode, BYTE ViewType, DWORD AccountSeq, BOOL SumOnly, DWORD MileageSection )
{
	return g_LibClientSessionShop->InquireCash(GameCode, ViewType, AccountSeq, SumOnly, MileageSection);
}

VOID CWebzenShop::SetCallbackOnInquireCash( CFunction_OnInquireCash fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnInquireCash = fnCallback;
}

BOOL CWebzenShop::BuyProduct(DWORD AccountSeq, CHAR* AccountID, DWORD GameCode, DWORD ProductSeq, CHAR* ProductName, DWORD InGamePurchaseSeq, int Class, int Level, CHAR* CharName, int ServerIndex, DWORD SalesZone, double DeductPrice, char DeductType, int MethodType)
{
	USES_CONVERSION;

	WCHAR* wProductName	= A2W( ProductName );
	WCHAR* wCharName	= A2W( CharName );

	//return g_LibClientSessionShop->BuyProduct(AccountSeq, AccountID, GameCode, ProductSeq, wProductName, InGamePurchaseSeq, Class, Level, wCharName, ServerIndex, SalesZone, DeductPrice, DeductType, MethodType);
	return FALSE;
}
//JK_À¥Á¨ºô¸µ
BOOL CWebzenShop::BuyProduct(DWORD GameCode, DWORD AccountSeq, DWORD ProductSeq, DWORD DisplaySeq, DWORD SalesZone, DWORD PriceSeq, int Class, int Level, CHAR* CharName, CHAR* Rank, int ServerIndex)
{
	USES_CONVERSION;

	WCHAR* wRank		= A2W( Rank );
	WCHAR* wCharName	= A2W( CharName );

	return g_LibClientSessionShop->BuyProduct(GameCode, AccountSeq, ProductSeq, DisplaySeq, SalesZone, PriceSeq, Class, Level, wCharName, wRank, ServerIndex);
}
//JK_À¥Á¨ºô¸µ
BOOL CWebzenShop::InquireSalesZoneScriptVersion(DWORD GameCode, DWORD SalesZone)
{
	USES_CONVERSION;

	return g_LibClientSessionShop->InquireSalesZoneScriptVersion(GameCode, SalesZone);

}
//JK_À¥Á¨ºô¸µ
BOOL CWebzenShop::InquireStorageListPageNoGiftMessage(DWORD AccountSeq, DWORD GameCode, DWORD SalesZone, char StorageType,int NowPage, int PageSize)
{
	USES_CONVERSION;

	return g_LibClientSessionShop->InquireStorageListPageNoGiftMessage(AccountSeq, GameCode, SalesZone, StorageType, NowPage, PageSize);
}
//JK_À¥Á¨ºô¸µ
BOOL CWebzenShop::UseStorage(DWORD AccountSeq, DWORD GameCode, DWORD IPAddress, DWORD StorageSeq, DWORD StorageItemSeq, char StorageItemType, int Class, int Level, CHAR* CharName, CHAR* Rank, int ServerIndex)
{
	USES_CONVERSION;

	WCHAR* wRank		= A2W( Rank );
	WCHAR* wCharName	= A2W( CharName );

	return g_LibClientSessionShop->UseStorage(AccountSeq, GameCode, IPAddress, StorageSeq, StorageItemSeq, StorageItemType, Class, Level, wCharName, wRank, ServerIndex);

}

//JK_À¥Á¨ºô¸µ
BOOL CWebzenShop::RollbackUseStorage(DWORD AccountSeq, DWORD GameCode, DWORD StorageSeq, DWORD StorageItemSeq)
{
	USES_CONVERSION;

	return g_LibClientSessionShop->RollbackUseStorage(AccountSeq, GameCode,  StorageSeq, StorageItemSeq);

}

VOID CWebzenShop::SetCallbackOnBuyProduct( CFunction_OnBuyProduct fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnBuyProduct = fnCallback;
}

VOID CWebzenShop::SetCallbackOnLog( CFunction_OnLog fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnLog = fnCallback;
}
//JK_À¥Á¨ºô¸µ
VOID CWebzenShop::SetCallbackOnInquireSalesZoneScriptVersion( CFunction_OnInquireSalesZoneScriptVersion fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnInquireSalesZoneScriptVersion = fnCallback;
}

VOID CWebzenShop::SetCallbackOnUpdateVersion( CFunction_OnUpdateVersion fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnUpdateVersion = fnCallback;
}

VOID CWebzenShop::SetCallbackOnInquireStorageListPageNoGiftMessage( CFunction_OnInquireStorageListPageNoGiftMessage fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnInquireStorageListPageNoGiftMessage = fnCallback;
}

VOID CWebzenShop::SetCallbackOnUseStorage( CFunction_OnUseStorage fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnUseStorage = fnCallback;
}

VOID CWebzenShop::SetCallbackOnRollbackUseStorage( CFunction_OnUseStorage fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnRollbackUseStorage = fnCallback;
}


VOID CWebzenShop::SetCallbackOnNetConnect( CFunction_OnNetConnect fnCallback )
{
	g_LibClientSessionShop->WZSHOP_OnNetConnect = fnCallback;
}

eWZConnect CWebzenShop::GetStatus()
{
	return g_LibClientSessionShop->m_bConnect;
}

//////////////////////////////////////////////////////////////////////////
// Billing
CWebzenBilling::CWebzenBilling()
{
	g_LibClientSessionBilling = new CLibClientSessionBilling;
}

CWebzenBilling::~CWebzenBilling()
{
	if(g_LibClientSessionBilling)
		delete g_LibClientSessionBilling;
}

BOOL CWebzenBilling::Initialize()
{
	if(!g_LibClientSessionBilling)
		return FALSE;

	if(!g_LibClientSessionBilling->CreateSession())
		return FALSE;

	return TRUE;
}

eWZConnect CWebzenBilling::Connect(char* dest, unsigned short port)
{
	g_LibClientSessionBilling->m_bConnect = eConnecting;

	g_LibClientSessionBilling->Connect(dest, port);

	return eConnecting;
}

VOID CWebzenBilling::SetCallbackOnLog( CFunction_OnLog fnCallback )
{
	g_LibClientSessionBilling->WZBILLING_OnLog = fnCallback;
}

VOID CWebzenBilling::SetCallbackOnInquirePCRoomPoint(CFunction_OnInquirePCRoomPoint fnCallback)
{
	g_LibClientSessionBilling->WZBILLING_OnInquirePCRoomPoint = fnCallback;
}

VOID CWebzenBilling::SetCallbackOnInquireMultiUser(CFunction_OnInquireMultiUser fnCallback)
{
	g_LibClientSessionBilling->WZBILLING_OnInquireMultiUser = fnCallback;
}

VOID CWebzenBilling::SetCallbackOnUserStatus( CFunction_OnUserStatus fnCallback )
{
	g_LibClientSessionBilling->WZBILLING_OnUserStatus = fnCallback;
}

VOID CWebzenBilling::SetCallbackOnInquirePersonDeduct( CFunction_OnInquirePersonDeduct fnCallback )
{
	g_LibClientSessionBilling->WZBILLING_OnInquirePersonDeduct = fnCallback;
}

eWZConnect CWebzenBilling::GetStatus()
{
	return g_LibClientSessionBilling->m_bConnect;
}

BOOL CWebzenBilling::UserLogin( DWORD dwAccountGUID, DWORD dwIPAddress, DWORD dwRoomGUID, DWORD dwGameCode, DWORD dwServerType )
{
	return g_LibClientSessionBilling->UserLogin(dwAccountGUID, dwIPAddress, dwRoomGUID, dwGameCode, dwServerType);
}

BOOL CWebzenBilling::UserLogout( long dwBillingGUID )
{
	return g_LibClientSessionBilling->UserLogout(dwBillingGUID);
}

BOOL CWebzenBilling::InquireUser( long dwBillingGUID )
{
	return g_LibClientSessionBilling->InquireUser(dwBillingGUID);
}

BOOL CWebzenBilling::InquireMultiUser( long AccountGUID, long RoomGUID )
{
	return g_LibClientSessionBilling->InquireMultiUser(AccountGUID, RoomGUID);
}

BOOL CWebzenBilling::InquirePCRoomPoint( long AccountGUID, long RoomGUID, long GameCode )
{
	return g_LibClientSessionBilling->InquirePCRoomPoint(AccountGUID, RoomGUID, GameCode);
}

BOOL CWebzenBilling::InquirePersonDeduct( long AccountGUID, long GameCode )
{
	return g_LibClientSessionBilling->InquirePersonDeduct(AccountGUID, GameCode);
}
