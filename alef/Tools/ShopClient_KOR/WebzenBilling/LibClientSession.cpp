#include "LibClientSession.h"
#include "stdio.h"

CLibClientSessionShop::CLibClientSessionShop(void)
{
	WZSHOP_OnInquireCash	= NULL;
	WZSHOP_OnBuyProduct		= NULL;
	WZSHOP_OnLog			= NULL;

	//JK_웹젠빌링
	WZSHOP_OnInquireSalesZoneScriptVersion = NULL; 
	WZSHOP_OnUpdateVersion  = NULL;
	WZSHOP_OnInquireStorageListPageNoGiftMessage = NULL;
	WZSHOP_OnUseStorage = NULL;
	WZSHOP_OnRollbackUseStorage = NULL;

	WZSHOP_OnNetConnect = NULL;

	m_bConnect = eNotConnect;
	m_nLastError = 0;
}

CLibClientSessionShop::~CLibClientSessionShop(void)
{
}

void CLibClientSessionShop::OnNetConnect(bool success, DWORD error)
{
	m_nLastError = error;
	m_bConnect = ( success ) ? eConnected : eNotConnect;

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] %d, %d\n", __FUNCTION__, success, error);
	WriteLog(szLog);

	stOnNetConnect pOnNetConnect;
	pOnNetConnect.success = success;
	pOnNetConnect.error   = error;

	if(WZSHOP_OnNetConnect)
		WZSHOP_OnNetConnect(&pOnNetConnect);
}

void CLibClientSessionShop::OnNetSend( int size )
{
	//AfxMessageBox("OnSend");
}

void CLibClientSessionShop::OnNetClose( DWORD error )
{
	m_nLastError = error;
	m_bConnect = eNotConnect;

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] %d\n", __FUNCTION__, error);
	WriteLog(szLog);
}

void CLibClientSessionShop::OnGiftProduct(DWORD SenderSeq, DWORD ReceiverSeq, DWORD DeductCashSeq, long ResultCode) 
{
	/*CString strMsg;

	strMsg.Format("상품 선물 결과\r\n보낸사람: %d, 받는사람: %d, 결과: %d, 웹 상품 차감에 대한 차감 번호: %d\r\n", 
		SenderSeq, ReceiverSeq, ResultCode, DeductCashSeq);

	WriteLog(strMsg.GetBuffer(0));*/
}
#ifdef WEBZEN_UNION_BILLING
void CLibClientSessionShop::OnInquireCash(DWORD AccountSeq, 
						   double CashSum, 
						   double PointSum, 
						   double MileageSum, 
						   int DetailCount, 
						   ShopProtocol::STCashDetail Detail[], 
						   long ResultCode)
{
	stInquire pInquire;
	memset( &pInquire, 0, sizeof(stInquire));

	pInquire.ResultCode	= ResultCode;
	pInquire.AccountSeq = AccountSeq;
	/*
	strcpy_s(pInquire.AccountID, sizeof(pInquire.AccountID), AccountID);

	for(int i = 0 ; i < nCashInfoCount ;i++)
	{
		if(Detail[i].Type == 'C')
		{
			switch(Detail[i].CashTypeCode)
			{
			case 508: // WCash
				{
					pInquire.WCoinSum += Detail[i].Value;
				} break;
			case 509: // PP Card Only
				{
					pInquire.PCoinSum += Detail[i].Value;
				} break;
			}
		}
		else if(Detail[i].Type == 'P')
		{
			// nop
		}
	}

	//pInquire.WCoinSum -= pInquire.PCoinSum;
*/
	pInquire.WCoinSum = CashSum;

	if( WZSHOP_OnInquireCash )
		WZSHOP_OnInquireCash(&pInquire);
}

void CLibClientSessionShop::OnBuyProduct(DWORD AccountSeq, 
						  long ResultCode, 
						  long LeftProductCount)
{
	stBuyProduct_Union pBuyProduct;
	pBuyProduct.ResultCode		 = ResultCode;
	pBuyProduct.AccountSeq		 = AccountSeq;
	pBuyProduct.LeftProductCount = LeftProductCount;

	if( WZSHOP_OnBuyProduct )
		WZSHOP_OnBuyProduct(&pBuyProduct);
}

#else
void CLibClientSessionShop::OnInquireCash(DWORD AccountSeq, CHAR* AccountID, double CashSum, double PointSum, ShopProtocol::STCashDetail_GB Detail[], long nCashInfoCount ,long ResultCode)
{
	stInquire pInquire;
	memset( &pInquire, 0, sizeof(stInquire));

	pInquire.ResultCode	= ResultCode;
	pInquire.AccountSeq = AccountSeq;
	strcpy_s(pInquire.AccountID, sizeof(pInquire.AccountID), AccountID);
	
	for(int i = 0 ; i < nCashInfoCount ;i++)
	{
		if(Detail[i].Type == 'C')
		{
			switch(Detail[i].CashTypeCode)
			{
				case 508: // WCash
					{
						pInquire.WCoinSum += Detail[i].Value;
					} break;
				case 509: // PP Card Only
					{
						pInquire.PCoinSum += Detail[i].Value;
					} break;
			}
		}
		else if(Detail[i].Type == 'P')
		{
			// nop
		}
	}

	//pInquire.WCoinSum -= pInquire.PCoinSum;

	if( WZSHOP_OnInquireCash )
		WZSHOP_OnInquireCash(&pInquire);
}

void CLibClientSessionShop::OnBuyProduct(DWORD AccountSeq, CHAR* AccountID, DWORD DeductCashSeq, DWORD InGamePurchaseSeq, long ResultCode) 
{	
	stBuyProduct pBuyProduct;
	pBuyProduct.ResultCode		= ResultCode;
	pBuyProduct.AccountSeq		= AccountSeq;
	strcpy_s(pBuyProduct.AccountID, sizeof(pBuyProduct.AccountID), AccountID);
	pBuyProduct.DeductCashSeq	= DeductCashSeq;
	pBuyProduct.InGamePurchaseSeq = InGamePurchaseSeq;

	if( WZSHOP_OnBuyProduct )
		WZSHOP_OnBuyProduct(&pBuyProduct);
}
#endif //WEBZEN_UNION_BILLING

/////////////////////////////////////////////////////////////////
void CLibClientSessionShop::OnInquireSalesZoneScriptVersion(unsigned short SalesZone, 
											 unsigned short Year, 
											 unsigned short YearIdentity, 
											 long ResultCode)
{
	stSalesZoneScriptVersion pSalesZoneScriptVersion;
	pSalesZoneScriptVersion.SalesZone    = SalesZone;
	pSalesZoneScriptVersion.Year	     = Year;
	pSalesZoneScriptVersion.YaerIdentity = YearIdentity;
	pSalesZoneScriptVersion.ResultCode   = ResultCode;

	if (WZSHOP_OnInquireSalesZoneScriptVersion)
		WZSHOP_OnInquireSalesZoneScriptVersion(&pSalesZoneScriptVersion);

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] SalesZone : %d, Year : %d, YearIdentity : %d, ResultCode : %d\n", 
							__FUNCTION__, SalesZone, Year,YearIdentity, ResultCode );
	WriteLog(szLog);
}

void CLibClientSessionShop::OnInquireBannerZoneScriptVersion(unsigned short BannerZone, 
											  unsigned short Year, 
											  unsigned short YearIdentity, 
											  long ResultCode)
{

}

void CLibClientSessionShop::OnGiftCash(DWORD SenderSeq, 
						DWORD ReceiverSeq, 
						long ResultCode, 
						double GiftSendLimit)
{

}

void CLibClientSessionShop::OnInquireBuyGiftPossibility(DWORD AccountID, 
								 long ResultCode, 
								 long ItemBuy, 
								 long Present, 
								 double MyPresentCash, 
								 double MyPresentSendLimit)
{

}
void CLibClientSessionShop::OnInquireEventProductList(DWORD AccountSeq, 
							   long ProductDisplaySeq, 
							   int PackagesCount, 
							   long Packages[])
{

}

void CLibClientSessionShop::OnInquireProductLeftCount(long PackageSeq, long LeftCount)
{

}

void CLibClientSessionShop::OnInquireStorageList(DWORD AccountID, 
						  long ListCount, 
						  long ResultCode, 
						  ShopProtocol::STStorage StorageList[])
{

}
void CLibClientSessionShop::OnInquireStorageListPage(DWORD AccountID, 
							  int	ResultCode, 
							  char  StorageType,
							  int	NowPage,
							  int   TotalPage,
							  int   TotalCount,
							  int   ListCount, 
							  ShopProtocol::STStorage StorageList[])
{

}
void CLibClientSessionShop::OnInquireStorageListPageNoGiftMessage(DWORD AccountID, 
										   int   ResultCode, 
										   char  StorageType,
										   int   NowPage,
										   int   TotalPage,
										   int   TotalCount,
										   int   ListCount, 
										   ShopProtocol::STStorageNoGiftMessage StorageList[])
{
	stStorageListPageNoGiftMessage pStorageListPageNoGiftMessage;
	pStorageListPageNoGiftMessage.AccountID = AccountID;
	pStorageListPageNoGiftMessage.ResultCode = ResultCode;
	pStorageListPageNoGiftMessage.StorageType = StorageType;
	pStorageListPageNoGiftMessage.NowPage = NowPage;
	pStorageListPageNoGiftMessage.TotalPage = TotalPage;
	pStorageListPageNoGiftMessage.TotalCount = TotalCount;
	pStorageListPageNoGiftMessage.ListCount = ListCount;
	memcpy(pStorageListPageNoGiftMessage.StorageList,StorageList,sizeof(ShopProtocol::STStorageNoGiftMessage) * ListCount);

	if (WZSHOP_OnInquireStorageListPageNoGiftMessage)
		WZSHOP_OnInquireStorageListPageNoGiftMessage(&pStorageListPageNoGiftMessage);



	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] AccountID : %d, ResultCode : %d, StorageType : %c, NowPage : %d, TotalPage : %d, TotalCount : %d, ListCount : %d, 	\n", 
		__FUNCTION__, AccountID, ResultCode, StorageType,NowPage, TotalPage, TotalCount,ListCount  );
	WriteLog(szLog);

}
void CLibClientSessionShop::OnUseStorage(DWORD AccountSeq, 
				  long ResultCode, 
				  char InGameProductID[MAX_TYPENAME_LENGTH], 
				  BYTE PropertyCount, 
				  long ProductSeq, 
				  long StorageSeq, 
				  long StorageItemSeq, 
				  ShopProtocol::STItemProperty PropertyList[])
{

	stUseStorage pUseStorage;
	pUseStorage.AccountSeq = AccountSeq;
	pUseStorage.ResultCode = ResultCode;
	strcpy_s(pUseStorage.InGameProductID, sizeof(pUseStorage.InGameProductID), InGameProductID);
	pUseStorage.PropertyCount = PropertyCount;
	pUseStorage.ProductSeq = ProductSeq;
	pUseStorage.StorageSeq = StorageSeq;
	pUseStorage.StorageItemSeq = StorageItemSeq;
	memcpy(pUseStorage.PropertyList,PropertyList,sizeof(ShopProtocol::STItemProperty) * PropertyCount);

	if(WZSHOP_OnUseStorage)
		WZSHOP_OnUseStorage(&pUseStorage);


	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] AccountSeq : %d, ResultCode : %d, InGameProductID : %s, PropertyCount : %d, ProductSeq : %d, StorageSeq : %d, StorageItemSeq : %d, 	\n", 
		__FUNCTION__, AccountSeq, ResultCode, InGameProductID, PropertyCount, ProductSeq, StorageSeq,StorageItemSeq  );
	WriteLog(szLog);
}

void CLibClientSessionShop::OnRollbackUseStorage(DWORD AccountSeq, long ResultCode)
{
	stRollbackUseStorage pRollbackUseStorage;
	pRollbackUseStorage.AccountSeq = AccountSeq;
	pRollbackUseStorage.ResultCode = ResultCode;

	if(WZSHOP_OnRollbackUseStorage)
		WZSHOP_OnRollbackUseStorage(&pRollbackUseStorage);

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] AccountSeq : %d, ResultCode : %d  	\n", 
		__FUNCTION__, AccountSeq, ResultCode   );
	WriteLog(szLog);

}

void CLibClientSessionShop::OnThrowStorage(DWORD AccountSeq, long ResultCode)
{

}
void CLibClientSessionShop::OnMileageDeduct(DWORD AccountSeq, long ResultCode)
{

}

void CLibClientSessionShop::OnMileageSave(DWORD AccountSeq, long ResultCode)
{

}

void CLibClientSessionShop::OnMileageLiveSaveUp(DWORD AccountSeq, long ResultCode)
{

}

void CLibClientSessionShop::OnItemSerialUpdate(DWORD AccountSeq, long ResultCode)
{

}

void CLibClientSessionShop::OnUpdateVersion(long GameCode, 
							 unsigned short SalesZone, 
							 unsigned short year, 
							 unsigned short yearIdentity)
{
	stSalesZoneScriptVersion pSalesZoneScriptVersion;
	pSalesZoneScriptVersion.SalesZone    = SalesZone;
	pSalesZoneScriptVersion.Year	     = year;
	pSalesZoneScriptVersion.YaerIdentity = yearIdentity;
	pSalesZoneScriptVersion.GameCode   = GameCode;

	if (WZSHOP_OnUpdateVersion)
		WZSHOP_OnUpdateVersion(&pSalesZoneScriptVersion);

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] SalesZone : %d, Year : %d, YearIdentity : %d, GameCode : %d\n", 
		__FUNCTION__, SalesZone, year,yearIdentity, GameCode );
	WriteLog(szLog);

}

void CLibClientSessionShop::OnUpdateBannerVersion(long GameCode, 
								   unsigned short BannerZone, 
								   unsigned short year, 
								   unsigned short yearIdentity)
{

}

void CLibClientSessionShop::OnInquireInGamePointValue(long ResultCode,
									   long PointCount,
									   ShopProtocol::STPointDetail PointList[])
{

}
////////////////////////////////////////////////////////////////////////////////
void CLibClientSessionShop::WriteLog(char* szMsg)
{
	if( WZSHOP_OnLog )
		WZSHOP_OnLog(szMsg);
}


//////////////////////////////////////////////////////////////////////////
// BILLING
CLibClientSessionBilling::CLibClientSessionBilling(void)
{
	WZBILLING_OnLog					= NULL;
	WZBILLING_OnInquirePCRoomPoint	= NULL;
	WZBILLING_OnInquireMultiUser	= NULL;
	WZBILLING_OnUserStatus			= NULL;
	WZBILLING_OnInquirePersonDeduct	= NULL;

	m_bConnect = eNotConnect;
	m_nLastError = 0;
}

CLibClientSessionBilling::~CLibClientSessionBilling(void)
{
}

void CLibClientSessionBilling::OnNetConnect(bool success, DWORD error)
{
	m_nLastError = error;
	m_bConnect = ( success ) ? eConnected : eNotConnect;

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] %d, %d\n", __FUNCTION__, success, error);
	WriteLog(szLog);
}

void CLibClientSessionBilling::OnNetSend( int size )
{
	//AfxMessageBox("OnSend");
}

void CLibClientSessionBilling::OnNetClose( DWORD error )
{
	m_nLastError = error;
	m_bConnect = eNotConnect;

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] %d\n", __FUNCTION__, error);
	WriteLog(szLog);
}

void CLibClientSessionBilling::WriteLog(char* szMsg)
{
	if( WZBILLING_OnLog )
		WZBILLING_OnLog(szMsg);
}

void CLibClientSessionBilling::OnInquirePCRoomPoint( long AccountID, long RoomGUID, long GameCode, long ResultCode )
{
	stInquirePCRoomPoint pInquirePCRoomPoint;
	pInquirePCRoomPoint.AccountGUID = AccountID;
	pInquirePCRoomPoint.RoomGUID = RoomGUID;
	pInquirePCRoomPoint.GameCode = GameCode;
	pInquirePCRoomPoint.ResultCode = ResultCode;

	if( WZBILLING_OnInquirePCRoomPoint )
		WZBILLING_OnInquirePCRoomPoint(&pInquirePCRoomPoint);
}

void CLibClientSessionBilling::OnInquireMultiUser( long AccountID, long RoomGUID, long Result )
{
	stInquireMultiUser pInquireMultiUser;
	pInquireMultiUser.AccountID = AccountID;
	pInquireMultiUser.RoomGUID = RoomGUID;
	pInquireMultiUser.Result = Result;
	
	if( WZBILLING_OnInquireMultiUser )
		WZBILLING_OnInquireMultiUser(&pInquireMultiUser);
}

void CLibClientSessionBilling::OnUserStatus( DWORD dwAccountGUID, long dwBillingGUID, DWORD RealEndDate, DWORD EndDate, double dRestPoint, double dRestTime, short nDeductType, short nAccessCheck, short nResultCode )
{
	stUserStatus pstUserStatus;
	pstUserStatus.dwAccountGUID = dwAccountGUID;
	pstUserStatus.dwBillingGUID = dwBillingGUID;
	pstUserStatus.RealEndDate = RealEndDate;
	pstUserStatus.EndDate = EndDate;
	pstUserStatus.dRestPoint = dRestPoint;
	pstUserStatus.dRestTime = dRestTime;
	pstUserStatus.nDeductType = nDeductType;
	pstUserStatus.nAccessCheck = nAccessCheck;
	pstUserStatus.nResultCode = nResultCode;

	if( WZBILLING_OnUserStatus )
		WZBILLING_OnUserStatus(&pstUserStatus);
}

void CLibClientSessionBilling::OnInquirePersonDeduct( long AccountGUID, long GameCode, long ResultCode )
{
	stInquirePersonDeduct pstInquirePersonDeduct;
	pstInquirePersonDeduct.AccountGUID = AccountGUID;
	pstInquirePersonDeduct.GameCode = GameCode;
	pstInquirePersonDeduct.ResultCode = ResultCode;

	if( WZBILLING_OnInquirePersonDeduct )
		WZBILLING_OnInquirePersonDeduct(&pstInquirePersonDeduct);
}

void CLibClientSessionBilling::OnCheckLoginUser(long AccountID, long BillingGUID)
{

}