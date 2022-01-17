#ifdef _AREA_KOREA_




#include "CWebzenCashItemBox.h"
#include "AgcmUIManager2.h"
#include "AgcmUICashMall.h"
#include "WebzenBilling_Kor.h"
#include "ProductDic.h"



static int g_nCashItemViewCount = 7;

BOOL CWebzenCashItemBox::_RegisterUIEvent( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	m_nEventCashItemBoxOpen = pcmUIManager->AddEvent( "UIEvent_WebzenCashItemBox_OnShow" );
	if( m_nEventCashItemBoxOpen < 0 ) return FALSE;

	m_nEventCashItemBoxClose = pcmUIManager->AddEvent( "UIEvent_WebzenCashItemBox_OnHide" );
	if( m_nEventCashItemBoxClose < 0 ) return FALSE;

	return TRUE;
}

BOOL CWebzenCashItemBox::_RegisterUICallBack( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	if( !pcmUIManager->AddFunction( this, "UICallBack_WebzenCashItemBox_OnClickOpenCashMall", CallBack_OnClickOpenCashMall, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UICallBack_WebzenCashItemBox_OnClickViewPagePrev", CallBack_OnClickViewPagePrev, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "UICallBack_WebzenCashItemBox_OnClickViewPageNext", CallBack_OnClickViewPageNext, 0 ) ) return FALSE;

	return TRUE;
}

BOOL CWebzenCashItemBox::_RegisterPacketCallBack( void )
{
	AgpmCashMall* ppmCashMall = ( AgpmCashMall* )g_pEngine->GetModule( "AgpmCashMall" );
	if( !ppmCashMall ) return FALSE;

	if( !ppmCashMall->SetCallbackResponseInquireStorageList( CallBack_OnReceiveCashItemBoxData, this ) ) return FALSE;
	if( !ppmCashMall->SetCallbackResponseUseStorage( CallBack_OnReceiveCashItemUseResult, this ) ) return FALSE;

	return TRUE;
}

BOOL CWebzenCashItemBox::_AddCashItem( void* pData )
{
	STStorageNoGiftMessage* pItemData = ( STStorageNoGiftMessage* )pData;
	if( !pItemData ) return FALSE;

	// 기존에 갖고 있던 데이터를 클리어한 후에 Add 되므로 무조건 Add 한다.
	stCashItemBoxEntry NewEntry;

	// StorageSeq, StorageItemSeq .. 아이템 사용시 이걸 서버에 보내게 된다.
	NewEntry.m_nStorageSeq = pItemData->Seq;
	NewEntry.m_nStorageItemSeq = pItemData->ItemSeq;

	// 이 아이템의 상품번호.. 이걸 기준으로 View 에 필요한 데이터를 찾는다.
	NewEntry.m_nPID = pItemData->ProductSeq;

	Webzen::Product const * product = Webzen::ProductDic::GetProduct( NewEntry.m_nPID );

	if( product )
	{
		// View 를 위해 필요한 데이터
		NewEntry.m_nTID = product->itemId;
		NewEntry.m_nStackCount = _GetItemStackCount( NewEntry.m_nTID );
		NewEntry.m_strName = product->name;

		m_vecCashItemBox.Add( NewEntry );
		return TRUE;
	}	
	
	return FALSE;
}

void* CWebzenCashItemBox::_GetCashItemBoxUIWindow( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_CashItemBox" );
	if( !pcdUI ) return FALSE;

	return pcdUI->m_pcsUIWindow;
}

BOOL CWebzenCashItemBox::_CollectDialogControls( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_CashItemBox" );
	if( !pcdUI ) return FALSE;

	AgcdUIControl* pcdControl_ItemCount = pcmUIManager->GetControl( pcdUI, "Text_CashItemCount" );
	if( pcdControl_ItemCount && pcdControl_ItemCount->m_pcsBase )
	{
		m_pTextItemCount = ( AcUIEdit* )pcdControl_ItemCount->m_pcsBase;
	}

	AgcdUIControl* pcdControl_PageCount = pcmUIManager->GetControl( pcdUI, "Text_CurrentPageNumber" );
	if( pcdControl_PageCount && pcdControl_PageCount->m_pcsBase )
	{
		m_pTextPageCount = ( AcUIEdit* )pcdControl_PageCount->m_pcsBase;
	}

	return TRUE;
}

BOOL CWebzenCashItemBox::_MakeCashItemView( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )_GetCashItemBoxUIWindow();
	if( !pUIWindow ) return FALSE;

	// View 의 갯수는 7개
	for( int nCount = 0 ; nCount < g_nCashItemViewCount ; nCount++ )
	{
		stCashItemBoxView* pView = m_vecCashItemView.Get( nCount );
		if( !pView )
		{
			stCashItemBoxView NewView;
			NewView.m_nIndex = nCount;

			// 아이템 아이콘 이미지
			NewView.m_pImage = _CreateCashItemViewImage( nCount );
			if( NewView.m_pImage )
			{
				pUIWindow->AddChild( NewView.m_pImage );
			}

			// 아이템 스택 카운트
			// 아이템 StackCount 출력기능 사용안하기로 함.. 샵 스크립트에서 제공하는 상품정보에는 갯수.. 라는값이 존재하지 않아 찍을수가 없음
			//NewView.m_pStackCount = _CreateCashItemViewStackCount( nCount );
			//if( NewView.m_pStackCount && NewView.m_pImage )
			//{
			//	NewView.m_pImage->AddChild( NewView.m_pStackCount );
			//}

			// 아이템 이름
			NewView.m_pTextName = _CreateCashItemViewName( nCount );
			if( NewView.m_pTextName )
			{
				pUIWindow->AddChild( NewView.m_pTextName );
			}

			// 아이템 사용요청 버튼
			NewView.m_pButtonUse = _CreateCashItemViewUseButton( nCount );
			if( NewView.m_pButtonUse )
			{
				pUIWindow->AddChild( NewView.m_pButtonUse );
			}

			m_vecCashItemView.Add( NewView );
		}
	}

	return TRUE;
}

CUICashMallTexture* CWebzenCashItemBox::_CreateCashItemViewImage( int nIndex )
{
	CUICashMallTexture* pImage = new CUICashMallTexture;
	if( !pImage ) return NULL;

	int nWidth = 50;
	int nHeight = 50;
	int nOffSet = 6;
	int nPosX = 7;
	int nPosY = 59 + ( ( nHeight + nOffSet ) * nIndex );

	pImage->MoveWindow( nPosX, nPosY, nWidth, nHeight );
	pImage->ShowWindow( FALSE );
	return pImage;
}

AcUIBase* CWebzenCashItemBox::_CreateCashItemViewStackCount( int nIndex )
{
	AcUIBase* pBase = new AcUIBase;
	if( !pBase ) return NULL;

	int nWidth = 25;
	int nHeight = 25;
	int nOffSet = 0;
	int nPosX = 25;
	int nPosY = 25;

	pBase->MoveWindow( nPosX, nPosY, nWidth, nHeight );
	pBase->SetStaticStringExt( "", 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_LEFT, ACUIBASE_VALIGN_CENTER );
	pBase->ShowWindow( FALSE );
	return pBase;
}

AcUIBase* CWebzenCashItemBox::_CreateCashItemViewName( int nIndex )
{
	AcUIBase* pBase = new AcUIBase;
	if( !pBase ) return NULL;

	int nWidth = 180;
	int nHeight = 48;
	int nOffSet = 8;
	int nPosX = 64;
	int nPosY = 60 + ( ( nHeight + nOffSet ) * nIndex );

	pBase->MoveWindow( nPosX, nPosY, nWidth, nHeight );
	pBase->SetStaticStringExt( "", 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_LEFT, ACUIBASE_VALIGN_CENTER );
	pBase->ShowWindow( FALSE );
	return pBase;
}

CExNPCDialogButton* CWebzenCashItemBox::_CreateCashItemViewUseButton( int nIndex )
{
	CExNPCDialogButton* pButton = new CExNPCDialogButton;
	if( !pButton ) return NULL;

	pButton->AddButtonImage( "Common_Button_OK_Cancel_A.png", ACUIBUTTON_MODE_NORMAL );
	pButton->AddButtonImage( "Common_Button_OK_Cancel_B.png", ACUIBUTTON_MODE_ONMOUSE );
	pButton->AddButtonImage( "Common_Button_OK_Cancel_C.png", ACUIBUTTON_MODE_CLICK );
	pButton->AddButtonImage( "Common_Button_OK_Cancel_D.png", ACUIBUTTON_MODE_DISABLE );

	int nWidth = 39;
	int nHeight = 39;
	int nOffSet = 17;
	int nPosX = 258;
	int nPosY = 64 + ( ( nHeight + nOffSet ) * nIndex );

	pButton->MoveWindow( nPosX, nPosY, nWidth, nHeight );
	pButton->m_lButtonDownStringOffsetY = 1;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( pcmUIManager )
	{
		char* pBtnText = pcmUIManager->GetUIMessage( "CashInven_ItemUse" );
		if( pBtnText && strlen( pBtnText ) > 0 )
		{
			pButton->SetStaticStringExt( pBtnText, 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
		}
	}

	pButton->OnRegisterCallBack( this, CallBack_OnClickUseCashItem );
	pButton->ShowWindow( FALSE );
	return pButton;
}

BOOL CWebzenCashItemBox::_DeleteCashItemView( void )
{
	AgcUIWindow* pUIWindow = ( AgcUIWindow* )_GetCashItemBoxUIWindow();
	if( !pUIWindow ) return FALSE;

	int nViewCount = m_vecCashItemView.GetSize();
	for( int nCount = 0 ; nCount < nViewCount ; nCount++ )
	{
		stCashItemBoxView* pView = m_vecCashItemView.Get( nCount );
		if( pView )
		{
			if( pView->m_pImage )
			{
				pUIWindow->DeleteChild( pView->m_pImage );
				delete pView->m_pImage;
				pView->m_pImage = NULL;
			}

			if( pView->m_pStackCount )
			{
				pUIWindow->DeleteChild( pView->m_pStackCount );
				delete pView->m_pStackCount;
				pView->m_pStackCount = NULL;
			}

			if( pView->m_pTextName )
			{
				pUIWindow->DeleteChild( pView->m_pTextName );
				delete pView->m_pTextName;
				pView->m_pTextName = NULL;
			}

			if( pView->m_pButtonUse )
			{
				pUIWindow->DeleteChild( pView->m_pButtonUse );
				delete pView->m_pButtonUse;
				pView->m_pButtonUse = NULL;
			}
		}
	}

	m_vecCashItemView.Clear();
	return TRUE;
}

stCashItemBoxView* CWebzenCashItemBox::_GetCashItemView( int nIndex )
{
	return m_vecCashItemView.Get( nIndex );
}

CUICashMallTexture* CWebzenCashItemBox::_GetCashItemViewImage( int nIndex )
{
	stCashItemBoxView* pView = _GetCashItemView( nIndex );
	if( !pView ) return NULL;
	return pView->m_pImage;
}

AcUIBase* CWebzenCashItemBox::_GetCashItemViewStackCount( int nIndex )
{
	stCashItemBoxView* pView = _GetCashItemView( nIndex );
	if( !pView ) return NULL;
	return pView->m_pStackCount;
}

AcUIBase* CWebzenCashItemBox::_GetCashItemViewName( int nIndex )
{
	stCashItemBoxView* pView = _GetCashItemView( nIndex );
	if( !pView ) return NULL;
	return pView->m_pTextName;
}

CExNPCDialogButton* CWebzenCashItemBox::_GetCashItemViewButton( int nIndex )
{
	stCashItemBoxView* pView = _GetCashItemView( nIndex );
	if( !pView ) return NULL;
	return pView->m_pButtonUse;
}

BOOL CWebzenCashItemBox::_RefreshCashItemCount( void )
{
	if( !m_pTextItemCount ) return FALSE;

	char strBuffer[ 1024 ] = { 0, };
	sprintf_s( strBuffer, sizeof(strBuffer), "보관중인 아이템 개수 : %d", m_nTotalItemCount );
	
	m_pTextItemCount->SetStaticStringExt( strBuffer, 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	return TRUE;
}

BOOL CWebzenCashItemBox::_RefreshCashItemPage( void )
{
	if( !m_pTextPageCount ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	char* pPageText= pcmUIManager->GetUIMessage( "StaticText_Page" );

	char strBuffer[ 32 ] = { 0, };
	sprintf_s( strBuffer, sizeof( char ) * 32, "%d%s", m_nCurPage, pPageText && strlen( pPageText ) > 0 ? pPageText : "Page" );

	m_pTextPageCount->SetStaticStringExt( strBuffer, 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_CENTER, ACUIBASE_VALIGN_CENTER );
	return TRUE;
}

BOOL CWebzenCashItemBox::_RefreshCashItemList( void )
{
	for( int nCount = 0 ; nCount < g_nCashItemViewCount ; nCount++ )
	{
		stCashItemBoxEntry* pEntry = m_vecCashItemBox.Get( nCount );
		if( !pEntry || pEntry->m_nPID < 0 )
		{
			_RefreshCashItemView( nCount, FALSE, 0, 0, "NoName" );
		}
		else
		{
			_RefreshCashItemView( nCount, TRUE, pEntry->m_nTID, pEntry->m_nStackCount, ( char* )pEntry->m_strName.c_str() );
		}
	}

	return TRUE;
}

BOOL CWebzenCashItemBox::_RefreshCashItemView( int nIndex, BOOL bIsShow, int nTID, int nStackCount, char* pName )
{
	_RefreshCashItemIcon( nIndex, bIsShow, nTID );

	// 아이템 StackCount 출력기능 사용안하기로 함.. 샵 스크립트에서 제공하는 상품정보에는 갯수.. 라는값이 존재하지 않아 찍을수가 없음
	//_RefreshCashItemStackCount( nIndex, bIsShow, nStackCount );

	_RefreshCashItemName( nIndex, bIsShow, pName );
	_RefreshCashItemUseButton( nIndex, bIsShow );
	return TRUE;
}

BOOL CWebzenCashItemBox::_RefreshCashItemIcon( int nIndex, BOOL bIsShow, int nTID )
{
	CUICashMallTexture* pImage = _GetCashItemViewImage( nIndex );
	if( !pImage ) return FALSE;

	CWebzenShopClient* pShop = _GetWebzenShop();
	if( !pShop ) return FALSE;

	RwTexture* pTexture = pShop->GetItemIconTexture( nTID );

	pImage->SetItemTID( nTID );
	pImage->SetTexture( pTexture );
	pImage->ShowWindow( bIsShow );
	return TRUE;
}

BOOL CWebzenCashItemBox::_RefreshCashItemStackCount( int nIndex, BOOL bIsShow, int nStackCount )
{
	AcUIBase* pText = _GetCashItemViewStackCount( nIndex );
	if( !pText ) return FALSE;
	
	CWebzenShopClient* pShop = _GetWebzenShop();
	if( !pShop ) return FALSE;

	char strBuffer[ 16 ] = { 0, };
	sprintf_s( strBuffer, sizeof( char ) * 16, "%d", nStackCount );

	pText->SetStaticStringExt( nStackCount > 0 ? strBuffer : "", 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_RIGHT, ACUIBASE_VALIGN_BOTTOM );
	pText->ShowWindow( bIsShow );
	return TRUE;
}

BOOL CWebzenCashItemBox::_RefreshCashItemName( int nIndex, BOOL bIsShow, char* pName )
{
	AcUIBase* pText = _GetCashItemViewName( nIndex );
	if( !pText ) return FALSE;
	
	CWebzenShopClient* pShop = _GetWebzenShop();
	if( !pShop ) return FALSE;

	pText->SetStaticStringExt( pName && strlen( pName ) > 0 ? pName : "", 1.0, 0, 0xFFFFFFFF, true, ACUIBASE_HALIGN_LEFT, ACUIBASE_VALIGN_CENTER );
	pText->ShowWindow( bIsShow );
	return TRUE;
}

BOOL CWebzenCashItemBox::_RefreshCashItemUseButton( int nIndex, BOOL bIsShow )
{
	CExNPCDialogButton* pButton = _GetCashItemViewButton( nIndex );
	if( !pButton ) return FALSE;

	pButton->ShowWindow( bIsShow );
	return TRUE;
}

CWebzenShopClient* CWebzenCashItemBox::_GetWebzenShop( void )
{
	AgcmUICashMall* pcmUICashMall = ( AgcmUICashMall* )g_pEngine->GetModule( "AgcmUICashMall" );
	if( !pcmUICashMall ) return NULL;
	return pcmUICashMall->GetWebzenShop();
}

int CWebzenCashItemBox::_GetItemStackCount( int nTID )
{
	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	if( !ppmItem ) return 0;

	AgpdItemTemplate* ppdItemTemplate = ppmItem->GetItemTemplate( nTID );
	if( !ppdItemTemplate ) return 0;

	return ppdItemTemplate->m_lMaxStackableCount;
}




#endif