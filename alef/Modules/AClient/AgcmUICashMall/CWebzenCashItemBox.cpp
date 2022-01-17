#ifdef _AREA_KOREA_


#include "CWebzenCashItemBox.h"
#include "AgcmUIManager2.h"
#include "AgcmUICashMall.h"
#include "AgppCashMall.h"
#include "AgcmUICashInven.h"



static int g_nCashItemViewCount = 7;

CWebzenCashItemBox::CWebzenCashItemBox( void )
{
	m_nEventCashItemBoxOpen = -1;
	m_nEventCashItemBoxClose = -1;

	m_nCurPage = 1;
	m_nMaxPage = 1;
	m_nTotalItemCount = 0;

	m_pTextItemCount = NULL;
	m_pTextPageCount = NULL;
}

CWebzenCashItemBox::~CWebzenCashItemBox( void )
{
}

BOOL CWebzenCashItemBox::OnCashItemBoxInitialize( void )
{
	if( !_RegisterUIEvent() ) return FALSE;
	if( !_RegisterUICallBack() ) return FALSE;
	if( !_RegisterPacketCallBack() ) return FALSE;

	return TRUE;
}

BOOL CWebzenCashItemBox::OnCashItemBoxDestroy( void )
{
	_DeleteCashItemView();
	return TRUE;
}

BOOL CWebzenCashItemBox::OnCashItemBoxShow( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->ThrowEvent( m_nEventCashItemBoxOpen );

	if( !_CollectDialogControls() ) return FALSE;
	if( !_MakeCashItemView() ) return FALSE;

	// 뷰가 준비되었으니 첫번째 페이지의 목록을 요청한다.
	if( !OnSendRequestCashItemBoxData( 1 ) ) return FALSE;

	return OnRefreshCashItemBox();
}

BOOL CWebzenCashItemBox::OnCashItemBoxHide( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->ThrowEvent( m_nEventCashItemBoxClose );
	return TRUE;
}

BOOL CWebzenCashItemBox::OnSelectPagePrev( void )
{
	return OnSelectPage( m_nCurPage - 1 );
}

BOOL CWebzenCashItemBox::OnSelectPageNext( void )
{
	return OnSelectPage( m_nCurPage + 1 );
}

BOOL CWebzenCashItemBox::OnSelectPage( int nPage )
{
	if( nPage <= 0 || nPage > m_nMaxPage ) return FALSE;
	return OnSendRequestCashItemBoxData( nPage );
}

BOOL CWebzenCashItemBox::OnRefreshCashItemBox( void )
{
	// 데이터 업데이트가 되었으니 아이템 갯수표시 업데이트
	_RefreshCashItemCount();

	// 페이지 정보 업데이트
	_RefreshCashItemPage();

	// 뷰를 업데이트
	return _RefreshCashItemList();
}

BOOL CWebzenCashItemBox::OnClickUseCashItem( int nIndex )
{
	stCashItemBoxEntry* pEntry = m_vecCashItemBox.Get( nIndex );
	if( !pEntry ) return FALSE;

	AgcmUICashInven * cashInven = (AgcmUICashInven*)g_pEngine->GetModule("AgcmUICashInven");

	if( cashInven )
	{
		if( cashInven->IsFull() ) // 캐쉬 인벤이 꽉찬경우 실패
		{
			AgcmUIManager2 * uiManager = (AgcmUIManager2*)g_pEngine->GetModule("AgcmUIManager2");
			if( uiManager )
				uiManager->ActionMessageOKDialog( "칸트라 인벤토리에 남은 공간이 없습니다." );
			return FALSE;
		}
	}

	return OnSendRequestUseCashItem( pEntry->m_nStorageSeq, pEntry->m_nStorageItemSeq );
}

BOOL CWebzenCashItemBox::OnSendRequestCashItemBoxData( int nPage )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	// 유료보관함 목록 요청패킷을 보낸다.
	PACKET_CASHMALL_INQUIRESTORAGELIST_CSREQ Packet( ppdCharacter->m_lID, nPage );
	return g_pEngine->SendPacket( Packet );
}

BOOL CWebzenCashItemBox::OnSendRequestUseCashItem( int nStorageSeq, int nStorageItemSeq )
{
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	PACKET_CASHMALL_USESTORAGE_CSREQ Packet( ppdCharacter->m_lID, nStorageSeq, nStorageItemSeq );
	return g_pEngine->SendPacket( Packet );
}

BOOL CWebzenCashItemBox::OnReceiveCashItemData( void* pData )
{
	PACKET_CASHMALL_INQUIRESTORAGELIST_SCACK* pPacket = ( PACKET_CASHMALL_INQUIRESTORAGELIST_SCACK* )pData;
	if( !pPacket ) return FALSE;
	if( pPacket->m_nResultCode != 0 ) return FALSE;

	// 패킷을 받은 경우 반드시 기존 데이터를 클리어 해야 한다. ㅡ.ㅡ.. 무조건 서버데이터에 의존해야 하므로..
	m_vecCashItemBox.Clear();

	for( int nCount = 0 ; nCount < pPacket->m_nListCount ; nCount++ )
	{
		_AddCashItem( &pPacket->m_pStorageList[ nCount ] );
	}

	m_nCurPage = pPacket->m_nNowPage;
	m_nMaxPage = pPacket->m_nTotalPage;
	m_nTotalItemCount = pPacket->m_nTotalCount;
	
	return OnRefreshCashItemBox();
}

BOOL CWebzenCashItemBox::OnReceiveCashItemUseResult( int nResultCode )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	char* pMsg = NULL;
	switch( nResultCode )
	{
	// 요건 성공
	case 0 :		pMsg = pcmUIManager->GetUIMessage( "WebzenCashItemBox_UseResult_Success" );					break;

	// 요기서부터는 에러
	case -2 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_DBAccessFailed" );					break;		// DB 접근이 안되요
	case -1 :		pMsg = pcmUIManager->GetUIMessage( "WebzenShop_BuyResult_DBError" );						break;		// DB에서 에러났어요
	case 1 :		pMsg = pcmUIManager->GetUIMessage( "WebzenCashItemBox_UseResult_NotUsableItem" );			break;		// 사용할 수 없는 물건이예요
	case 2 :		pMsg = pcmUIManager->GetUIMessage( "WebzenCashItemBox_UseResult_EnableOnlyPCBang" );		break;		// PC방에서만 쓸수 있어요

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

	// 아이템 사용결과를 받았으니 보관함 목록을 다시 요청한다.
	OnSendRequestCashItemBoxData( m_nCurPage );

	// 사용결과 확인 메세지를 띄운다.
	pcmUIManager->ActionMessageOKDialog( pMsg );
	return TRUE;
}

int CWebzenCashItemBox::FindCashItemViewIndex( CExNPCDialogButton* pButton )
{
	int nViewCount = m_vecCashItemView.GetSize();
	for( int nCount = 0 ; nCount < nViewCount ; nCount++ )
	{
		stCashItemBoxView* pView = m_vecCashItemView.Get( nCount );
		if( pView && pView->m_pButtonUse == pButton )
		{
			return nCount;
		}
	}

	return -1;
}

BOOL CWebzenCashItemBox::CallBack_OnClickOpenCashMall( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl )
{
	AgcmUICashMall* pcmUICashMall = ( AgcmUICashMall* )g_pEngine->GetModule( "AgcmUICashMall" );
	if( !pcmUICashMall ) return FALSE;

	CWebzenShopClient* pWebzenShop = pcmUICashMall->GetWebzenShop();
	if( !pWebzenShop ) return FALSE;

	return pWebzenShop->OnWebzenShopShow();
}

BOOL CWebzenCashItemBox::CallBack_OnClickViewPagePrev( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl )
{
	CWebzenCashItemBox* pCashItemBox = ( CWebzenCashItemBox* )pClass;
	if( !pCashItemBox ) return FALSE;
	return pCashItemBox->OnSelectPagePrev();
}

BOOL CWebzenCashItemBox::CallBack_OnClickViewPageNext( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl )
{
	CWebzenCashItemBox* pCashItemBox = ( CWebzenCashItemBox* )pClass;
	if( !pCashItemBox ) return FALSE;
	return pCashItemBox->OnSelectPageNext();
}

BOOL CWebzenCashItemBox::CallBack_OnClickUseCashItem( void* pClass, void* pControl )
{
	CWebzenCashItemBox* pCashItemBox = ( CWebzenCashItemBox* )pClass;
	CExNPCDialogButton* pButton = ( CExNPCDialogButton* )pControl;
	if( !pCashItemBox || !pButton ) return FALSE;

	int nSelectedProductIndex = pCashItemBox->FindCashItemViewIndex( pButton );
	pCashItemBox->OnClickUseCashItem( nSelectedProductIndex );
	return TRUE;
}

BOOL CWebzenCashItemBox::CallBack_OnReceiveCashItemBoxData( void* pData, void* pClass, void* pCustData )
{
	CWebzenCashItemBox* pCashItemBox = ( CWebzenCashItemBox* )pClass;
	if( !pCashItemBox ) return FALSE;

	pCashItemBox->OnReceiveCashItemData( pData );
	return TRUE;
}

BOOL CWebzenCashItemBox::CallBack_OnReceiveCashItemUseResult( void* pData, void* pClass, void* pCustData )
{
	CWebzenCashItemBox* pCashItemBox = ( CWebzenCashItemBox* )pClass;
	if( !pCashItemBox ) return FALSE;

	PACKET_CASHMALL_USESTORAGE_SCACK* pPacket = ( PACKET_CASHMALL_USESTORAGE_SCACK* )pData;
	if( !pPacket ) return FALSE;

	pCashItemBox->OnReceiveCashItemUseResult( pPacket->m_nResultCode );
	return TRUE;
}


#endif