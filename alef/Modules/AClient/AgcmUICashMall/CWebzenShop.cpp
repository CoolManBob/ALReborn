#ifdef _AREA_KOREA_

#include "CWebzenShop.h"
#include "AgcmUIManager2.h"
#include "AgcmItem.h"
#include "AuStrTable.h"
#include "AgppBillInfo.h"
#include "AgpmCashMall.h"
#include "AgppCashMall.h"
#include "AgcmUICashInven.h"
#include "CWebzenShopList.h"


// 한페이지당 출력할 상품 갯수
static int g_nProductViewCountPerPage = 6;


CWebzenShopClient::CWebzenShopClient( void )
{
	m_bIsDialogOpen = FALSE;
	m_bIsProductViewCreate = FALSE;
	m_nCurrentCategory = 0;

	m_nCurPage = 0;
	m_nMaxPage = 0;

	m_nEvent_ShopOpen = -1;
	m_nEvent_ShopClose = -1;

	m_nEvent_BuyInfoOpen = -1;
	m_nEvent_BuyInfoClose = -1;

	m_nCash = 0;
	m_nBuyInfo_PackageId = 0;
	m_nBuyInfo_CategoryId = 0;
	m_nBuyInfo_PriceSeq = 0;
}

CWebzenShopClient::~CWebzenShopClient( void )
{
}

BOOL CWebzenShopClient::OnWebzenShopInitialize( void )
{
	if( !_RegisterUIEvent() ) return FALSE;
	if( !_RegisterUICallBack() ) return FALSE;
	if( !_RegisterPacketCallBack() ) return FALSE;
	return TRUE;
}

BOOL CWebzenShopClient::OnWebzenShopDestroy( void )
{
	_DeleteCategoryButton();
	_DeleteProductView();
	return TRUE;
}

BOOL CWebzenShopClient::OnWebzenShopShow( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	//if( m_bIsDialogOpen ) return OnWebzenShopClose();

	AgcdUI * ui = pcmUIManager->GetUI( "UI_CashMall_Webzen" );

	if( ui && (ui->m_eStatus == AGCDUI_STATUS_OPENED || ui->m_eStatus == AGCDUI_STATUS_OPENING) )
	{
		return OnWebzenShopClose();
	}

	if( !pcmUIManager->ThrowEvent( m_nEvent_ShopOpen ) ) return FALSE;

	// 캐쉬정보 요청
	OnSendRequestCashData();

	_CreateProductView();
	_CollectPageButtons();

	m_bIsDialogOpen = TRUE;

	_RefreshCategoryButtons();
	OnSelectCategory( 0 );

	return TRUE;
}

BOOL CWebzenShopClient::OnWebzenShopClose( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;
	//if( !pcmUIManager->ThrowEvent( m_nEvent_ShopClose ) ) return FALSE;
	pcmUIManager->ThrowEvent( m_nEvent_ShopClose );
	m_bIsDialogOpen = FALSE;
	return TRUE;
}


BOOL CWebzenShopClient::OnShowBuyInformation( int nCateogryIndex, int nProductIndex )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	// 창 가져와서..
	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_CashMall_WebzenBuy" );
	if( !pcdUI || !pcdUI->m_pcsUIWindow ) return FALSE;

	// TopMost 속성을 지정해준다.. 안그러면 샵 UI 밑에 깔려버린다;;
	pcdUI->m_pcsUIWindow->m_Property.bTopmost = TRUE;

	// 창 열고..
	pcmUIManager->ThrowEvent( m_nEvent_BuyInfoOpen );

	// 창 내용 업데이트
	return OnRefreshBuyInformation( nCateogryIndex, nProductIndex );
}

BOOL CWebzenShopClient::OnRefreshBuyInformation( int nCateogryIndex, int nProductIndex )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	std::string packageName = "";
	int price = 0;
	int categoryId = 0;
	int packageId = 0;
	int priceId = 0;

	bool findData = false;

	stWebzenShopButton * categoryBtn = m_mapButtonCategory.GetByIndex( nCateogryIndex );

	if( categoryBtn )
	{
		Webzen::Category const * category = Webzen::Shop::GetCategory( categoryBtn->m_nCategoryId );

		if( category 
			&& (int)( category->packages.size() ) > nProductIndex )
		{
			Webzen::Package const & package = category->packages[nProductIndex];

			packageName = package.name;
			price = package.price;

			categoryId = category->id;
			packageId = package.id;
			priceId = package.priceId;

			findData = true;
		}
	}

	if( !findData )
		return FALSE;


	// 창 가져와서..
	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_CashMall_WebzenBuy" );
	if( !pcdUI || !pcdUI->m_pcsUIWindow ) return FALSE;

	// 구매할 아이템 이름
	AgcdUIControl* pcdUIControl_EditBuyItemName = pcmUIManager->GetControl( pcdUI, "Edit_BuyProductDetail" );
	if( !pcdUIControl_EditBuyItemName ) return FALSE;

	AcUIEdit* pEditBuyItemName = ( AcUIEdit* )pcdUIControl_EditBuyItemName->m_pcsBase;
	if( pEditBuyItemName )
	{
		pEditBuyItemName->SetText( const_cast<char*>(packageName.c_str()) );
	}

	// 구매할 아이템의 가격
	AgcdUIControl* pcdUIControl_EditBuyItemPrice = pcmUIManager->GetControl( pcdUI, "Edit_ProductPriceDetail" );
	if( !pcdUIControl_EditBuyItemPrice ) return FALSE;

	AcUIEdit* pEditBuyItemPrice = ( AcUIEdit* )pcdUIControl_EditBuyItemPrice->m_pcsBase;
	if( pEditBuyItemPrice )
	{
		char strBuffer[ 64 ] = { 0, };
		sprintf_s( strBuffer, sizeof( char ) * 64, "%d", price );

		pEditBuyItemPrice->SetText( strBuffer );
	}

	// 구매에 사용할 캐쉬의 종류
	AgcdUIControl* pcdUIControl_EditBuyCashType = pcmUIManager->GetControl( pcdUI, "Edit_YourCashTypeDetail" );
	if( !pcdUIControl_EditBuyCashType ) return FALSE;

	AcUIEdit* pEditBuyCashType = ( AcUIEdit* )pcdUIControl_EditBuyCashType->m_pcsBase;
	if( pEditBuyCashType )
	{
		char strBuffer[ 64 ] = { 0, };
		sprintf_s( strBuffer, sizeof( char ) * 64, "%d", price );

		pEditBuyCashType->SetText( strBuffer );
	}

	// 구매하고 난 후의 예상 잔액
	AgcdUIControl* pcdUIControl_EditRemainCash = pcmUIManager->GetControl( pcdUI, "Edit_RemainYourCash" );
	if( !pcdUIControl_EditRemainCash ) return FALSE;

	AcUIEdit* pEditRemainCash = ( AcUIEdit* )pcdUIControl_EditRemainCash->m_pcsBase;
	if( pEditRemainCash )
	{
		// 내 캐쉬 잔액... 지금은 없으니 일단 0
		__int64 nRemainCash = m_nCash - ( __int64 )price;

		char strBuffer[ 64 ] = { 0, };
		sprintf_s( strBuffer, sizeof( char ) * 64, "%I64d", nRemainCash >= 0 ? nRemainCash : 0 );

		pEditRemainCash->SetText( strBuffer );

		// 구매가 가능한 경우 구입버튼을 활성화시킨다.
		AgcdUIControl* pcdUIControl_BtnDoBuyProduct = pcmUIManager->GetControl( pcdUI, "Btn_DoBuyProduct" );
		if( !pcdUIControl_BtnDoBuyProduct ) return FALSE;

		AcUIButton* pButtonBuy = ( AcUIButton* )pcdUIControl_BtnDoBuyProduct->m_pcsBase;
		if( pButtonBuy )
		{
			pButtonBuy->SetButtonEnable( nRemainCash >= 0 ? TRUE : FALSE );
		}
	}

	// 구매하고자 하는 아이템의 정보를 저장해둔다.
	_SaveProductBuyInfo( categoryId, packageId, priceId );
	return TRUE;
}

BOOL CWebzenShopClient::OnSelectCategory( int nCategoryIndex )
{
	m_nCurrentCategory = nCategoryIndex;

	int nProductCount = 0;
	// 현재 카테고리에 지정된 상품갯수를 가져오고...
	stWebzenShopButton * categoryBtn = m_mapButtonCategory.GetByIndex( m_nCurrentCategory );
	if( categoryBtn )
	{
		Webzen::Category const * category = Webzen::Shop::GetCategory( categoryBtn->m_nCategoryId );
		if( category )
		{
			nProductCount = (int)( category->packages.size() );
		}
	}

	m_nMaxPage = nProductCount / g_nProductViewCountPerPage;

	if( nProductCount % g_nProductViewCountPerPage > 0 )
		m_nMaxPage += 1;

	return OnSelectPage( 1 );
}

BOOL CWebzenShopClient::OnSelectPagePrev( void )
{
	return OnSelectPage( m_nCurPage - 1 );
}

BOOL CWebzenShopClient::OnSelectPageNext( void )
{
	return OnSelectPage( m_nCurPage + 1 );
}

BOOL CWebzenShopClient::OnSelectPage( int nPageIndex )
{
	// 비정상적인 페이지 인덱스면 패스
	if( nPageIndex < 1 || nPageIndex > m_nMaxPage ) return FALSE;

	m_nCurPage = nPageIndex;
	_RefreshPageCount();
	_RefreshProductList();
	return TRUE;
}

BOOL CWebzenShopClient::OnSendRequestShopVersion( void )
{
	if( !g_pEngine ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	AgpmBillInfo* ppmBillInfo = ( AgpmBillInfo* )g_pEngine->GetModule( "AgpmBillInfo" );
	if( !pcmCharacter || !ppmBillInfo ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	PACKET_BILLINGINFO_VERSIONINFO_CSREQ Packet( ppdSelfCharacter->m_lID );
	if( !g_pEngine->SendPacket( Packet ) ) return FALSE;

	return TRUE;
}

BOOL CWebzenShopClient::OnRequestBuyProduct( int nProductViewIndex )
{
	// 우선 현재 구매가능한 상태인지를 검사한다.
	if( !IsEnableBuyCashItem(nProductViewIndex) ) return FALSE;

	// 인덱스 범위 벗어나면 패스
	if( nProductViewIndex < 0 || nProductViewIndex >= g_nProductViewCountPerPage ) return FALSE;

	// 현재 페이지 번호와 조합하여 실제 순서인덱스를 가져온다.
	nProductViewIndex = ( ( m_nCurPage - 1 ) * g_nProductViewCountPerPage ) + nProductViewIndex;

	// 구매확인창을 띄운다.
	return OnShowBuyInformation( m_nCurrentCategory, nProductViewIndex );
}

BOOL CWebzenShopClient::OnSendRequestBuyProduct( void )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	PACKET_CASHMALL_BUY_CSREQ Packet( ppdCharacter->m_lID, m_nBuyInfo_PackageId, m_nBuyInfo_CategoryId, m_nBuyInfo_PriceSeq );
	if( !g_pEngine->SendPacket( Packet ) ) return FALSE;
	return TRUE;
}

BOOL CWebzenShopClient::OnSendRequestCashData( void )
{
	// 서버로 캐쉬정보 다시 달라고 패킷을 보낸다.. 기존에 사용하던 캐쉬데이터 요청 패킷을 그대로 사용한다.
	AgcmCashMall* pcmCashMall = ( AgcmCashMall* )g_pEngine->GetModule( "AgcmCashMall" );
	if( !pcmCashMall ) return FALSE;

	pcmCashMall->SendPacketRefreshCash();
	return TRUE;
}

BOOL CWebzenShopClient::OnReceiveWebzenShopVersion( unsigned short nYear, unsigned short nYearID )
{
	// 현재 내가 PC방 유저인지 아닌지의 정보를 라이브러리쪽에 넘겨줘야 한다.
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	AgpmBillInfo* ppmBillInfo = ( AgpmBillInfo* )g_pEngine->GetModule( "AgpmBillInfo" );
	if( !pcmCharacter || !ppmBillInfo ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	if( Webzen::Script::SetVersion( nYear, nYearID ) )
	{
		Webzen::Shop::SetShopVersion( nYear, nYearID );
		Webzen::ProductDic::Init( nYear, nYearID );
	}

	return TRUE;
}

BOOL CWebzenShopClient::OnReceiveCashData( __int64 nCash )
{
	// 별거 없다 그냥 저장..
	m_nCash = nCash;

	// 캐쉬정보 처리가 끝났으면 UI 를 업데이트 한다.
	return _RefreshCashData();
}

BOOL CWebzenShopClient::OnReceiveBuyResult( int nResultCode )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	char* pMsg = NULL;
	switch( nResultCode )
	{
	// 요건 성공
	case 0 :
		{
			pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_Success" );

			// 구매가 완료되었으니 유료보관함의 목록이 변경되었을터.. 유료보관함의 목록을 갱신해 달라고 서버로 요청한다.
			AgcmUICashInven* pcmUICashInven = ( AgcmUICashInven* )g_pEngine->GetModule( "AgcmUICashInven" );
			if( pcmUICashInven )
			{
				CWebzenCashItemBox* pCashItemBox = pcmUICashInven->GetCashItemBox();
				if( pCashItemBox )
				{
					pCashItemBox->OnSendRequestCashItemBoxData( 1 );
				}
			}
		}
		break;

	// 요기서부터는 에러
	case -2 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_DBAccessFailed" );				break;		// DB 접근이 안되요
	case -1 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_DBError" );					break;		// DB에서 에러났어요
	case 1 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_NotEnoughCash" );				break;		// 돈없어요
	case 2 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_PermissionDenied" );			break;		// 넌 구매할 권한이 없어요
	case 3 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_NoRemainProduct" );			break;		// 재고가 없어요 다팔렸음다
	case 4 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_SalesOutofDate" );				break;		// 이 상품은 판매기간이 지났어요
	case 5 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_SalesOutofDate" );				break;		// 이 상품은 이제 안팔아요
	case 6 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_ShopIsNotOpened" );			break;		// 지금 장사 안해요
	case 7 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_CannotBuyEventItem" );			break;		// 이벤트 상품은 살 수 없어요
	case 8 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_CannotBuyMoreEventItem" );		break;		// 이벤트 상품이라 더 이상 살 수 없어요
	case 9 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_FailedByTooFast" );			break;		// 마지막 구매시간으로부터 지금까지 쿨타임이 아직 안지났어요 좀있다 사요

	// 미정의 코드
	default :
		{
			pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_UnKnownError" );
			if( pMsg && strlen( pMsg ) > 0 )
			{
				char strBuffer[ 256 ] = { 0, };
				sprintf_s( strBuffer, sizeof( char ) * 256, pMsg, nResultCode );
				pcmUIManager->ActionMessageOKDialog( pMsg );
			}

			return TRUE;
		}
		break;
	}

	pcmUIManager->ActionMessageOKDialog( pMsg );
	return TRUE;
}

int CWebzenShopClient::FindCategoryIndex( void* pClickedButton )
{
	if( !pClickedButton ) return -1;

	int nCategoryButtonCount = m_mapButtonCategory.GetSize();
	for( int nCount = 0 ; nCount < nCategoryButtonCount ; nCount++ )
	{
		stWebzenShopButton* pBtn = m_mapButtonCategory.GetByIndex( nCount );
		if( pBtn && pBtn->m_pButton && pBtn->m_pButton == pClickedButton)
		{
			//return pBtn->m_nCategoryIndex;
			return nCount;
		}
	}

	return -1;
}

int CWebzenShopClient::FindProductIndex( void* pClickedButton )
{
	if( !pClickedButton ) return -1;

	int nProductViewCount = m_mapViewProduct.GetSize();
	for( int nCount = 0 ; nCount < nProductViewCount ; nCount++ )
	{
		stWebzenProductView* pView = m_mapViewProduct.GetByIndex( nCount );
		if( pView && pView->m_pButtonBuy == pClickedButton )
		{
			return pView->m_nIndex;
		}
	}

	return -1;
}

BOOL CWebzenShopClient::IsEnableBuyCashItem( int productIdx )
{
	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !ppmItem || !pcmCharacter || !pcmUIManager ) return FALSE;

	// 일단 내 캐릭터는 있어야 한다.
	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	// 내 캐릭터가 죽어 있으면 불가능하다.
	if( ppdSelfCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
	{
		// 죽어있는 상태에서는 아이템 구매 못한다고 알려준다.
		char* pErrorMsg = pcmUIManager->GetUIMessage( "NotCommitOnDead" );
		pcmUIManager->ActionMessageOKDialog( pErrorMsg );
		return FALSE;
	}

	// 캐쉬아이템을 지정갯수 이상 보유한 경우 더이상 구매할 수 없다.
	AgpdGrid* ppdCashItemGrid = ppmItem->GetCashInventoryGrid( ppdSelfCharacter );
	if( !ppdCashItemGrid ) return FALSE;

	if( ppdCashItemGrid->m_lItemCount >= ppdCashItemGrid->m_lGridCount )
	{
		char* pErrorMsg = ClientStr().GetStr( STI_CASHITEM_MAX_COUNT );
		pcmUIManager->ActionMessageOKDialog( pErrorMsg );
		return FALSE;
	}

	BOOL isPCBang = FALSE;

	AgpmBillInfo * billinfo = ( AgpmBillInfo * )g_pEngine->GetModule( "AgpmBillInfo" );

	if( billinfo )
	{
		isPCBang = billinfo->IsPCBang( pcmCharacter->GetSelfCharacter() );
	}

	stWebzenShopButton * categoryBtn = m_mapButtonCategory.GetByIndex( m_nCurrentCategory );

	if( categoryBtn )
	{
		Webzen::Category const * category = Webzen::Shop::GetCategory( categoryBtn->m_nCategoryId );

		if( !isPCBang && category->IsPCRoomCategory() )
		{
			pcmUIManager->ActionMessageOKDialog( "웹젠 PC방에서 구입해 주시기 바랍니다." );
			return FALSE;
		}
	}	

	// 그 외에는 살수 있다.
	return TRUE;
}

RwTexture* CWebzenShopClient::GetItemIconTexture( int nTID )
{
	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgcmItem* pcmItem = ( AgcmItem* )g_pEngine->GetModule( "AgcmItem" );
	if( !ppmItem || !pcmItem ) return NULL;

	AgpdItemTemplate* ppdItemTemplate = ppmItem->GetItemTemplate( nTID );
	if( !ppdItemTemplate ) return NULL;

	AgcdItemTemplate* pcdItemTemplate = pcmItem->GetTemplateData( ppdItemTemplate );
	if( !pcdItemTemplate ) return NULL;

	if( !pcdItemTemplate->m_pTexture )
	{
		if( !pcmItem->LoadTemplateData( ppdItemTemplate, pcdItemTemplate ) ) return NULL;
	}

	return pcdItemTemplate->m_pTexture;
}

BOOL CWebzenShopClient::CallBack_OnSetSelfCharacter( void* pData, void* pClass, void* pCustData )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop ) return FALSE;

	// 내 캐릭터가 지정된 시점에 샵 데이터를 요청한다.
	pShop->OnSendRequestShopVersion();
	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnSelectCategory( void* pClass, void* pControl )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	CExNPCDialogButton* pButton = ( CExNPCDialogButton* )pControl;
	if( !pShop || !pButton ) return FALSE;

	int nCategoryIndex = pShop->FindCategoryIndex( pButton );
	pShop->OnSelectCategory( nCategoryIndex );

	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnRequestBuyProduct( void* pClass, void* pControl )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	CExNPCDialogButton* pButton = ( CExNPCDialogButton* )pControl;
	if( !pShop || !pButton ) return FALSE;

	int nSelectedProductIndex = pShop->FindProductIndex( pButton );
	pShop->OnRequestBuyProduct( nSelectedProductIndex );
	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnConfirmBuyProduct( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop ) return FALSE;

	pShop->OnSendRequestBuyProduct();
	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnClickPageButton( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop || !pcdControl || !pcdControl->m_pcsBase ) return FALSE;

	AcUIButton* pButton = ( AcUIButton* )pcdControl->m_pcsBase;
	if( pButton->m_szStaticString && strlen( pButton->m_szStaticString ) > 0 )
	{
		int nPageNumber = atoi( pButton->m_szStaticString );
		pShop->OnSelectPage( nPageNumber );
	}

	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnClickPagePrev( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop ) return FALSE;

	pShop->OnSelectPagePrev();
	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnClickPageNext( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop ) return FALSE;

	pShop->OnSelectPageNext();
	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnClickRefreshCash( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop ) return FALSE;

	pShop->OnSendRequestCashData();
	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnReceiveShopVersion( void* pData, void* pClass, void* pCustData )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop ) return FALSE;

	PACKET_BILLINGINFO_VERSIONINFO_SCACK* pPacket = ( PACKET_BILLINGINFO_VERSIONINFO_SCACK* )pData;
	if( !pPacket ) return FALSE;

	// 패킷으로부터 Year, YearID 정보 추출
	unsigned short nYear = pPacket->m_nYear;
	unsigned short nYearID = pPacket->m_nYearIdentity;

	pShop->OnReceiveWebzenShopVersion( nYear, nYearID );
	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnReceiveCashData( void* pData, void* pClass, void* pCustData )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop ) return FALSE;

	AgpmBillInfo* ppmBillInfo = ( AgpmBillInfo* )g_pEngine->GetModule( "AgpmBillInfo" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmBillInfo || !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	AgpdBillInfo* pBillInfo = ppmBillInfo->GetADCharacter( ppdCharacter );
	if( !pBillInfo ) return FALSE;

	pShop->OnReceiveCashData( pBillInfo->m_CashInfoGlobal.m_WCoin );
	return TRUE;
}

BOOL CWebzenShopClient::CallBack_OnReceiveBuyResult( void* pData, void* pClass, void* pCustData )
{
	CWebzenShopClient* pShop = ( CWebzenShopClient* )pClass;
	if( !pShop ) return FALSE;

	PACKET_CASHMALL_BUY_SCACK* pPacket = ( PACKET_CASHMALL_BUY_SCACK* )pData;
	if( !pPacket ) return FALSE;

	pShop->OnReceiveBuyResult( pPacket->m_nResultCode );
	return TRUE;
}


#endif