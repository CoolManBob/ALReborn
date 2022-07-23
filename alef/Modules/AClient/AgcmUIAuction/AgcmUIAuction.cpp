#include "AgcmUIAuction.h"
#include "ApMemoryTracker.h"
#include "AuStrTable.h"


#define	BUTTON_COOLTIME												2000
#define PROGRESS_COOLTIME											2500
#define AGCMAUCTION_MAX_DOCID										9999999999999


static UINT32 g_ulPrevTick = 0;


AgcmUIAuction::AgcmUIAuction( void )
{
	SetModuleName( "AgcmUIAuction" );

	m_lBoardCurrentTID = 0;
	m_lBoardCurrentStartPage = 0;
	m_lBoardMaxPage = 0;
	m_llBuyDocID = 0;
	m_lOldBoardGridItemTID = 0;
	m_lBuyStackItemCount = 0;
	m_lBoardCurrentRequetedPage = 0;
	m_lPageDirection = AGCM_UIAUCTION_PAGE_DIRECTION_NONE;

	m_llBuyPrice = 0;

	m_pcsUserDataBoardItemTemplateGrid = NULL;
	m_pstUDMainTree = NULL;
	m_pstUDMainTreeDepth = NULL;

	ZeroMemory( m_paCategoryUD, sizeof( AgcdUICategoryInfoUnit* ) * AGCM_UI_AUCTION_MAX_CATEGORY_COUNT );
	m_pstCategoryUD = NULL;

	ZeroMemory( m_paBoardUD, sizeof( AgcdUIAuctionBoardUnit* ) * AGCM_UI_AUCTION_MAX_BOARD_COUNT );
	m_pstBoardUD = NULL;

	for( INT32 lCounter = 0 ; lCounter < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ; ++lCounter )
	{
		m_pacsAgpdItemOnBoard[ lCounter ] = NULL;
	}

	m_lAuctionItemTIDList.MemSetAll();
	m_lAuctionIDListDepth.MemSetAll();
	m_lAuctionCategoryStatus.MemSetAll();

	ZeroMemory( m_strCategorySearchEdit, sizeof( char ) * 80 );
	m_pcsCategorySearchEdit = NULL;
	
	ZeroMemory( m_strCategoryItemName, sizeof( m_strCategoryItemName ) );
	m_pstCateogryNameUD = NULL;
	
	ZeroMemory( m_strPageInfo, sizeof( m_strPageInfo ) );
	m_pstPageInfoUD = NULL;

	for( int nButtonCount = 0 ; nButtonCount < 10 ; ++ nButtonCount )
	{
		m_pstPageButtonStaticUD[ nButtonCount ] = NULL;
		m_aiPageButtonStatic[ nButtonCount ] = 0;
	}

	m_lEventOpenBoard = 0;
	m_lEventCloseBoard = 0;
	m_lBoardBuyMessageBox = 0;
	m_lBoardNotEnoughMoney = 0;

	m_lCategoryEnableItem = 0;
	m_lCategoryDisableItem = 0;

	m_lEnableProgress0 = 0;
	m_lEnableProgress1 = 0;
	m_lEnableProgress2 = 0;

	m_lEnablePage1 = 0;
	m_lEnablePage2 = 0;
	m_lEnablePage3 = 0;
	m_lEnablePage4 = 0;
	m_lEnablePage5 = 0;
	m_lEnablePage6 = 0;
	m_lEnablePage7 = 0;
	m_lEnablePage8 = 0;
	m_lEnablePage9 = 0;
	m_lEnablePage10 = 0;

	m_lEventListButtonOn = 0;
	m_lEventListButtonOff = 0;
	
	m_ullFirstDocID	= AGCMAUCTION_MAX_DOCID;
	m_ullLastDocID = 0;

	m_lCooldownID = AGCMUICOOLDOWN_INVALID_ID;
	m_bNowProgress = FALSE;
}

AgcmUIAuction::~AgcmUIAuction( void )
{
	INT32 lCounter;

	for( lCounter = 0 ; lCounter < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ; ++lCounter )
	{
		if( m_paCategoryUD[ lCounter ] != NULL )
		{
			delete m_paCategoryUD[ lCounter ];
			m_paCategoryUD[ lCounter ] = NULL;
		}
	}

	for( lCounter = 0 ; lCounter < AGCM_UI_AUCTION_MAX_BOARD_COUNT ; ++lCounter )
	{
		if( m_paBoardUD[ lCounter ] != NULL )
		{
			delete m_paBoardUD[ lCounter ];
			m_paBoardUD[ lCounter ] = NULL;
		}
	}
}

BOOL AgcmUIAuction::OnAddModule( void )
{
	m_pcsAgpmGrid				= ( AgpmGrid* )GetModule( "AgpmGrid" );
	m_pcsAgpmFactors			= ( AgpmFactors* )GetModule( "AgpmFactors" );
	m_pcsAgpmCharacter			= ( AgpmCharacter* )GetModule( "AgpmCharacter" );
	m_pcsAgcmCharacter			= ( AgcmCharacter* )GetModule( "AgcmCharacter" );
	m_pcsAgpmItem				= ( AgpmItem* )GetModule( "AgpmItem");
	m_pcsAgpmItemConvert		= ( AgpmItemConvert* )GetModule( "AgpmItemConvert" );
	m_pcsAgpmAuctionCategory	= ( AgpmAuctionCategory* )GetModule( "AgpmAuctionCategory" );
	m_pcsAgpmAuction			= ( AgpmAuction* )GetModule( "AgpmAuction" );
	m_pcsAgcmAuction			= ( AgcmAuction* )GetModule( "AgcmAuction" );
	m_pcsAgcmItem				= ( AgcmItem* )GetModule( "AgcmItem" );
	m_pcsAgcmUIManager2			= ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
	m_pcsAgcmUISplitItem		= ( AgcmUISplitItem* )GetModule( "AgcmUISplitItem" );
	m_pcsAgcmUICooldown			= ( AgcmUICooldown* )GetModule( "AgcmUICooldown" );
	m_pcsAgcmChatting			= ( AgcmChatting2* )GetModule( "AgcmChatting2" );

	if( !m_pcsAgpmGrid || !m_pcsAgpmFactors || !m_pcsAgpmCharacter || !m_pcsAgcmCharacter || !m_pcsAgpmItem || 
		!m_pcsAgpmItemConvert || !m_pcsAgpmAuctionCategory || !m_pcsAgpmAuction || !m_pcsAgcmAuction || !m_pcsAgcmItem || 
		!m_pcsAgcmUIManager2 || !m_pcsAgcmUISplitItem || !m_pcsAgcmChatting )
	{
		return FALSE;
	}

	if( !AddEvent() ) return FALSE;
	if( !AddFunction() ) return FALSE;
	if( !AddDisplay() )	return FALSE;
	if( !AddUserData() ) return FALSE;

	m_lTreeDepth = 0;
	m_lCurrentCategory0ID = 0;
	m_lCurrentCategory1ID = 0;
	m_lCurrentCategory2ID = 0;
	m_bIsAuctionUIOpen = FALSE;

	if( !m_pcsAgcmAuction->SetCallbackSelect(				CBBoardSelect,			this ) ) return FALSE;
	if( !m_pcsAgcmAuction->SetCallbackAuctionUIOpen(		CBOpenAuctionUI,		this ) ) return FALSE;
	if( !m_pcsAgpmAuction->SetCallbackSell(					CBSellUI,				this ) ) return FALSE;
	if( !m_pcsAgpmAuction->SetCallbackCancel(				CBCancelUI,				this ) ) return FALSE;
	if( !m_pcsAgpmAuction->SetCallbackConfirm(				CBConfirmUI,			this ) ) return FALSE;
	if( !m_pcsAgpmAuction->SetCallbackBuy(					CBBuyUI,				this ) ) return FALSE;
	if( !m_pcsAgcmAuction->SetCallbackSalesBox2Update(		CBUpdateUI,				this ) ) return FALSE;
	if(	!m_pcsAgcmCharacter->SetCallbackSelfUpdatePosition(	CBSelfUpdatePosition,	this ) ) return FALSE;

	if( m_pcsAgcmUICooldown )
	{
		m_lCooldownID = m_pcsAgcmUICooldown->RegisterCooldown( CBAfterCooldown, this );
		if( AGCMUICOOLDOWN_INVALID_ID >= m_lCooldownID ) return FALSE;
	}

	return TRUE;
}

BOOL AgcmUIAuction::OnDestroy( void )
{
	m_pcsAgpmGrid->Remove( &m_csBoardItemTemplateGrid );
	return TRUE;
}

BOOL AgcmUIAuction::OnInit( void )
{
	if( m_pcsAgpmGrid )
	{
		m_pcsAgpmGrid->Init( &m_csBoardItemTemplateGrid, 1, 1, 1 );
	}

	m_csBoardItemTemplateGrid.m_lGridType = AGPDGRID_ITEM_TYPE_ITEM;

	if( m_pstBoardUD )
	{
		m_pstBoardUD->m_stUserData.m_lCount = 0;
	}

	// 쌔워줬으니~~~ 화면 갱신!! 냠냠~ ^^;
	if( m_pcsAgcmUIManager2 )
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstBoardUD );
	}

	return TRUE;
}

BOOL AgcmUIAuction::AddEvent( void )
{
	m_lEventOpenBoard = m_pcsAgcmUIManager2->AddEvent(		"AC_OpenEvent" );
	if( m_lEventOpenBoard < 0 )	return FALSE;

	m_lEventCloseBoard = m_pcsAgcmUIManager2->AddEvent(		"AC_CloseEvent");
	if( m_lEventCloseBoard < 0 ) return FALSE;

	m_lBoardBuyMessageBox = m_pcsAgcmUIManager2->AddEvent(	"AC_BuyMessageBox", CBReturnConfirmBuyItemFromAuction, this );
	if( m_lBoardBuyMessageBox < 0 )	return FALSE;

	m_lBoardNotEnoughMoney = m_pcsAgcmUIManager2->AddEvent(	"AC_NotEnoughMoney");
	if( m_lBoardNotEnoughMoney < 0 ) return FALSE;

	m_lCategoryEnableItem = m_pcsAgcmUIManager2->AddEvent(	"AC_EnableCategoryItem" );
	if( m_lCategoryEnableItem < 0 )	return FALSE;

	m_lCategoryDisableItem = m_pcsAgcmUIManager2->AddEvent( "AC_DisableCategoryItem" );
	if( m_lCategoryDisableItem < 0 ) return FALSE;

	m_lEnableProgress0 = m_pcsAgcmUIManager2->AddEvent(		"AC_EP0" );
	if( m_lEnableProgress0 < 0 ) return FALSE;

	m_lEnableProgress1 = m_pcsAgcmUIManager2->AddEvent(		"AC_EP1" );
	if( m_lEnableProgress1 < 0 ) return FALSE;

	m_lEnableProgress2 = m_pcsAgcmUIManager2->AddEvent(		"AC_EP2" );
	if( m_lEnableProgress2 < 0 ) return FALSE;

	m_lEnablePage1 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage1" );
	if( m_lEnablePage1 < 0 ) return FALSE;

	m_lEnablePage2 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage2" );
	if( m_lEnablePage2 < 0 ) return FALSE;

	m_lEnablePage3 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage3" );
	if( m_lEnablePage3 < 0 ) return FALSE;

	m_lEnablePage4 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage4" );
	if( m_lEnablePage4 < 0 ) return FALSE;

	m_lEnablePage5 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage5" );
	if( m_lEnablePage5 < 0 ) return FALSE;

	m_lEnablePage6 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage6" );
	if( m_lEnablePage6 < 0 ) return FALSE;

	m_lEnablePage7 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage7" );
	if( m_lEnablePage7 < 0 ) return FALSE;

	m_lEnablePage8 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage8" );
	if( m_lEnablePage8 < 0 ) return FALSE;

	m_lEnablePage9 = m_pcsAgcmUIManager2->AddEvent( 		"AC_EPage9" );
	if( m_lEnablePage9 < 0 ) return FALSE;

	m_lEnablePage10 = m_pcsAgcmUIManager2->AddEvent(		"AC_EPage10" );
	if( m_lEnablePage10 < 0 ) return FALSE;

	m_lEventListButtonOn = m_pcsAgcmUIManager2->AddEvent(	"AC_ListButtonOn" );
	if( m_lEventListButtonOn < 0 ) return FALSE;

	m_lEventListButtonOff = m_pcsAgcmUIManager2->AddEvent(	"AC_ListButtonOff" );
	if( m_lEventListButtonOff < 0 ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAuction::AddFunction( void )
{
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_OpenUI",			CBOpenUI,				1 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_SetFocusOnBoard",	CBSetFocusOnACBoard,	1 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_KillFocusOnBoard", CBKillFocusOnACBoard,	1 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_SelectCategory",	CBSelectCategory,		1 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_BackToParent",		CBBackToParentButton,	1 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_SearchTID",		CBSearchCategoryButton, 1 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_BuyItem",			CBBuyItemFromAuction,	0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_PrevPage",			CBPrevPage,				0 ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "AC_NextPage",			CBNextPage,				0 ) ) return FALSE;
	if(	!m_pcsAgcmUIManager2->AddFunction( this, "AC_OpenAnywhere",		CBOpenAnywhere,			0 ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAuction::AddDisplay( void )
{
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_Tree_Display",			0, CBDisplayAuctionTree,		AGCDUI_USERDATA_TYPE_INT32 ) )	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_Category",				0, CBDisplayCategory,			AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_BoardCategoryName",	0, CBDisplayBoardCategoryName,	AGCDUI_USERDATA_TYPE_STRING ) )	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageIngfo",			0, CBPageInfo,					AGCDUI_USERDATA_TYPE_STRING ) )	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_1",			0, CBPageInfo1, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_2",			0, CBPageInfo2, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_3",			0, CBPageInfo3, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_4",			0, CBPageInfo4, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_5",			0, CBPageInfo5, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_6", 			0, CBPageInfo6, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_7", 			0, CBPageInfo7, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_8", 			0, CBPageInfo8, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_9", 			0, CBPageInfo9, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_PageInfo_10",			0, CBPageInfo10, 				AGCDUI_USERDATA_TYPE_INT32 ) ) 	return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_BoardDoc",				0, CBDisplayDoc, 				AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_BoardItemName",		0, CBDisplayItemName,			AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_BoardPrice",			0, CBDisplayPrice,				AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_BoardQuantity",		0, CBDisplayQuantity,			AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this, "Auction_BoardSellerName",		0, CBDisplaySellerName,			AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAuction::AddUserData( void )
{
	m_pstUDMainTree						= m_pcsAgcmUIManager2->AddUserData( "AuctionTreeData",				&m_lAuctionItemTIDList[ 0 ],	sizeof( INT32 ),					AGCM_UI_AUCTION_MAX_CATEGORY_COUNT, AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstUDMainTree ) return FALSE;

	m_pstUDMainTreeDepth				= m_pcsAgcmUIManager2->AddUserData( "AuctionTreeDepthData",			&m_lAuctionIDListDepth[ 0 ],	sizeof( INT32 ),					AGCM_UI_AUCTION_MAX_CATEGORY_COUNT, AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstUDMainTreeDepth )	return FALSE;

	m_pstCategoryUD						= m_pcsAgcmUIManager2->AddUserData( "AuctionCategory",				m_paCategoryUD,					sizeof( AgcdUICategoryInfoUnit* ),	AGCM_UI_AUCTION_MAX_CATEGORY_COUNT, AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pstCategoryUD ) return FALSE;

	m_pstCateogryNameUD					= m_pcsAgcmUIManager2->AddUserData( "Auction_BoardCategoryName",	m_strCategoryItemName,			sizeof( char ),						255,								AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pstCateogryNameUD ) return FALSE;

	m_pcsUserDataBoardItemTemplateGrid	= m_pcsAgcmUIManager2->AddUserData( "AC_BoardItemTemplateGrid",		&m_csBoardItemTemplateGrid,		sizeof( AgpdGrid ),					1,									AGCDUI_USERDATA_TYPE_GRID );
	if( !m_pcsUserDataBoardItemTemplateGrid ) return FALSE;

	m_pstPageInfoUD						= m_pcsAgcmUIManager2->AddUserData( "Auction_BoardPageInfo",		m_strPageInfo,					sizeof( char ),						255,								AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pstPageInfoUD ) return FALSE;

	m_pstBoardUD						= m_pcsAgcmUIManager2->AddUserData( "AuctionBoard",					m_paBoardUD,					sizeof( AgcdUIAuctionBoardUnit* ),	AGCM_UI_AUCTION_MAX_BOARD_COUNT,	AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pstBoardUD )	return FALSE;

	m_pstPageButtonStaticUD[ 0 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_1",				&m_aiPageButtonStatic[ 0 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 0 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 1 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_2",				&m_aiPageButtonStatic[ 1 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 1 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 2 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_3",				&m_aiPageButtonStatic[ 2 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 2 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 3 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_4",				&m_aiPageButtonStatic[ 3 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 3 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 4 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_5",				&m_aiPageButtonStatic[ 4 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 4 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 5 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_6",				&m_aiPageButtonStatic[ 5 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 5 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 6 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_7",				&m_aiPageButtonStatic[ 6 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 6 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 7 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_8",				&m_aiPageButtonStatic[ 7 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 7 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 8 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_9",				&m_aiPageButtonStatic[ 8 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 8 ] )	return FALSE;

	m_pstPageButtonStaticUD[ 9 ]		= m_pcsAgcmUIManager2->AddUserData( "AC_PageInfoUD_10",				&m_aiPageButtonStatic[ 9 ],		sizeof( INT32 ),					1,									AGCDUI_USERDATA_TYPE_INT32 );
	if( !m_pstPageButtonStaticUD[ 9 ] )	return FALSE;

	m_pcsCategorySearchEdit				= m_pcsAgcmUIManager2->AddUserData( "AC_CategorySearchEdit",		m_strCategorySearchEdit,		sizeof( m_strCategorySearchEdit ),	1,									AGCDUI_USERDATA_TYPE_STRING );
	if ( !m_pcsCategorySearchEdit )		return FALSE;

	return TRUE;
}

BOOL AgcmUIAuction::CBBuyItemFromAuction( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass || !pcsSourceControl ) return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	
	// 진행상황 창이 떠 있으면 그냥 나간다. 2008.01.08. steeple
	if( pThis->m_bNowProgress )	return TRUE;
	if( !pThis->m_paBoardUD[ pcsSourceControl->m_lUserDataIndex ] )	return FALSE;

	// JNY 2005.6.1
	// Bug Tracking System 51, 125번 문제에 의해 자기자신이 올린 아이템은 구입할수 없다.
	AgpdCharacter* pAgpdCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pAgpdCharacter ) return FALSE;

	if( !_tcscmp( pThis->m_paBoardUD[ pcsSourceControl->m_lUserDataIndex ]->m_pstrSellerName, pAgpdCharacter->m_szID ) )
	{
		SystemMessage.ProcessSystemMessage( ClientStr().GetStr( STI_CANT_BUY_MINE ) );
		return FALSE;
	}

	//우선 1개를 산다고 세팅한다.
	pThis->m_lBuyStackItemCount = 1;
	pThis->m_llBuyDocID = pThis->m_paBoardUD[ pcsSourceControl->m_lUserDataIndex ]->m_llDocID;
	pThis->m_llBuyPrice	= ( INT64 )pThis->m_paBoardUD[ pcsSourceControl->m_lUserDataIndex ]->m_lPrice;

	//1개 이상있다면?
	if( pThis->m_paBoardUD[ pcsSourceControl->m_lUserDataIndex ]->m_lCount > 1 )
	{
		// 몽창 산다.
		pThis->m_lBuyStackItemCount = pThis->m_paBoardUD[ pcsSourceControl->m_lUserDataIndex ]->m_lCount;
	}

	if( ( INT64 )pThis->m_llBuyPrice > pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_llMoney )
	{
		// 돈이 모자라욤
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lBoardNotEnoughMoney );
	}
	else
	{
		// 샀어욤
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lBoardBuyMessageBox );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBBoardSelect( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( pClass && pData && pCustData )
	{
		//대략 Auction UI 콜백 쌔워주삼~
		AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
		stRowset* pstRowSet	= ( stRowset* )pData;
		AgcdUIAuctionBoardUnit* pcsAuctionBoardUnit = 0;
		AgpmAuctionArg* pstArg = ( AgpmAuctionArg* )pCustData;
		
		pThis->m_lBoardMaxPage = ( ( pstArg->m_lTotalSales - 1 ) / AGCM_UI_AUCTION_MAX_BOARD_COUNT ) + 1;

		BOOL bStackableItem = FALSE;
		char* pstrData = 0;
		INT32 lItemTID;
		INT32 lCounter;

		if( pstRowSet->m_ulRows == 0 )
		{
			// no result set in this range but has more than 1 page.
			// several buy operation from other client may cause this
			if( pThis->m_lBoardMaxPage > 0)
			{
				// if clicked prev. page, we try first page
				if( AGCM_UIAUCTION_PAGE_DIRECTION_LEFT == pThis->m_lPageDirection )
				{
					pThis->m_lPageDirection = AGCM_UIAUCTION_PAGE_DIRECTION_NONE;
					pThis->m_lBoardCurrentRequetedPage = 1;
					pThis->SetCooldown( pThis->m_pcsAgcmCharacter->m_lSelfCID, pThis->m_lBoardCurrentTID, AGCMAUCTION_MAX_DOCID, 0, 1 );
					pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage1 );
					return TRUE;
				}
				else if( AGCM_UIAUCTION_PAGE_DIRECTION_RIGHT == pThis->m_lPageDirection )
				{
					pThis->m_lBoardCurrentRequetedPage = pThis->m_lBoardMaxPage;
				}
			}

			// 첫 페이지가 아닌데, 내용이 없는 경우 안내 메시지 출력
			if( pThis->m_lBoardCurrentRequetedPage != 1 )
			{
				pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( ClientStr().GetStr( STI_NO_SALES_CONTENTS ) );
			}

		}

		//아이템 인스턴스를 초기화한다.
		for( lCounter = 0 ; lCounter < AGCM_UI_AUCTION_MAX_BOARD_COUNT ; ++lCounter )
		{
			if( pThis->m_pacsAgpdItemOnBoard[ lCounter ] )
			{
				pThis->m_pcsAgpmItem->RemoveItem( pThis->m_pacsAgpdItemOnBoard[ lCounter ], TRUE, TRUE );
				pThis->m_pacsAgpdItemOnBoard[ lCounter ] = NULL;
			}
		}

		for( lCounter = 0 ; lCounter < AGCM_UI_AUCTION_MAX_BOARD_COUNT ; ++lCounter )
		{
			if( pThis->m_paBoardUD[ lCounter ] )
			{
				delete pThis->m_paBoardUD[ lCounter ];
				pThis->m_paBoardUD[ lCounter ] = NULL;
			}
		}

		if( pstRowSet->Get( 0, AGPMAUCTION_SELECT_COLUMN_DOCID ) )
		{
			pThis->m_ullFirstDocID = _atoi64( pstRowSet->Get( 0, AGPMAUCTION_SELECT_COLUMN_DOCID ) );
		}

		if( pstRowSet->Get( pstRowSet->m_ulRows - 1, AGPMAUCTION_SELECT_COLUMN_DOCID ) )
		{
			pThis->m_ullLastDocID = _atoi64( pstRowSet->Get( pstRowSet->m_ulRows - 1, AGPMAUCTION_SELECT_COLUMN_DOCID ) );
		}
		
		for( int lRow = 0 ; lRow < ( int )pstRowSet->m_ulRows ; ++lRow )
		{
			lItemTID = 0;

			pcsAuctionBoardUnit = new AgcdUIAuctionBoardUnit;
			pcsAuctionBoardUnit->m_lIndex = ( pThis->m_lBoardCurrentRequetedPage - 1 ) * AGCM_UI_AUCTION_MAX_BOARD_COUNT + lRow + 1;

			pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_DOCID );
			if( pstrData )
			{
				pcsAuctionBoardUnit->m_llDocID = _atoi64( pstrData );
			}

			pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_PRICE );
			if( pstrData )
			{
				pcsAuctionBoardUnit->m_lPrice = atoi( pstrData );
			}

			pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_ITEMTID );
			if( pstrData )
			{
				AgpdItemTemplate* pcsItemTemplate;

				pcsAuctionBoardUnit->m_pstrItemName = new char[ 255 ];
				memset( pcsAuctionBoardUnit->m_pstrItemName, 0, sizeof( char ) * 255 );

				lItemTID = atoi( pstrData );
				pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate( lItemTID );

				if( pcsItemTemplate )
				{
					strcat( pcsAuctionBoardUnit->m_pstrItemName, pcsItemTemplate->m_szName.c_str() );
					bStackableItem = pcsItemTemplate->m_bStackable;
				}
			}

			pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_STACKCOUNT );
			if( pstrData )
			{
				if( bStackableItem )
				{
					pcsAuctionBoardUnit->m_lCount = atoi( pstrData );
				}
				//스택 불가능한 아이템(장비)등은 무조건 1로 보여준다.
				else
				{
					pcsAuctionBoardUnit->m_lCount = 1;
				}
			}

			pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_SELLER );
			if( pstrData )
			{
				pcsAuctionBoardUnit->m_pstrSellerName = new char[ 255 ];
				memset( pcsAuctionBoardUnit->m_pstrSellerName, 0, sizeof( char ) * 255 );
				strcat( pcsAuctionBoardUnit->m_pstrSellerName, pstrData );
			}

			//아이템의 개조히스토리
			pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_CONVHIST );

			//실제 아이템 인스턴스를 만들어본다.
			if( pstrData )
			{
				AgpdItemConvertADItem* pcsItemConvertAD;
				pThis->m_pacsAgpdItemOnBoard[ lRow ] = pThis->m_pcsAgpmItem->AddItem( 0, lItemTID, 1, FALSE );

				if( pThis->m_pacsAgpdItemOnBoard[ lRow ] )
				{
					pcsItemConvertAD = pThis->m_pcsAgpmItemConvert->GetADItem( pThis->m_pacsAgpdItemOnBoard[ lRow ] );
					pThis->m_pcsAgpmItemConvert->DecodeConvertHistory( pcsItemConvertAD, pstrData, strlen( pstrData ), TRUE, pThis->m_pacsAgpdItemOnBoard[ lRow ] );

					//아이템의 옵션 히스토리
					pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_OPTION );
					if( pstrData )
					{
						INT32 ucOptionTID[ AGPDITEM_OPTION_MAX_NUM ];
						ZeroMemory( ucOptionTID, sizeof( ucOptionTID ) );

						int	retval = sscanf( pstrData, "%d,%d,%d,%d,%d", &ucOptionTID[ 0 ],	&ucOptionTID[ 1 ], &ucOptionTID[ 2 ], &ucOptionTID[ 3 ], &ucOptionTID[ 4 ] );
						if( retval > 0 && retval != EOF )
						{
							for( int nItemOptionCount = 0 ; nItemOptionCount < AGPDITEM_OPTION_MAX_NUM ; ++nItemOptionCount )
							{
								if( ucOptionTID[ nItemOptionCount ] == 0 ) break;
								pThis->m_pcsAgpmItem->AddItemOption( pThis->m_pacsAgpdItemOnBoard[ lRow ], ucOptionTID[ nItemOptionCount ], FALSE );
							}
						}
					}
					
					pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_SKILLPLUS );
					if( pstrData )
					{
						INT32 ulSkillPlusTID[ AGPMITEM_MAX_SKILL_PLUS_EFFECT ];
						ZeroMemory( ulSkillPlusTID, sizeof( ulSkillPlusTID ) );

						int retval = sscanf( pstrData, "%d,%d,%d", &ulSkillPlusTID[ 0 ], &ulSkillPlusTID[ 1 ], &ulSkillPlusTID[ 2 ] );
						if( retval > 0 && retval != EOF )
						{
							for( int nSkillPlusCount = 0 ; nSkillPlusCount < AGPMITEM_MAX_SKILL_PLUS_EFFECT ; ++nSkillPlusCount )
							{
								if( ulSkillPlusTID[ nSkillPlusCount ] == 0 ) break;
								pThis->m_pcsAgpmItem->AddItemSkillPlus( pThis->m_pacsAgpdItemOnBoard[ lRow ], ulSkillPlusTID[ nSkillPlusCount ] );
							}
						}
					}

					pThis->m_pcsAgpmItemConvert->CalcPhysicalConvertFactor( pThis->m_pacsAgpdItemOnBoard[ lRow ] );
				}
			}

			pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_MAX_DURABILITY );
			if( pstrData )
			{
				pThis->m_pcsAgpmFactors->SetValue( &pThis->m_pacsAgpdItemOnBoard[ lRow ]->m_csFactor, atoi( pstrData ),
										AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY );
			}

			pstrData = pstRowSet->Get( lRow, AGPMAUCTION_SELECT_COLUMN_DURABILITY );
			if( pstrData )
			{
				pThis->m_pcsAgpmFactors->SetValue( &pThis->m_pacsAgpdItemOnBoard[ lRow ]->m_csFactor, atoi( pstrData ),
										AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY );
			}

			if( pThis->m_pacsAgpdItemOnBoard[ lRow ] )
			{
				AgpdItem* pcsItem = pThis->m_pacsAgpdItemOnBoard[ lRow ];
				if( pcsItem )
				{
					// Physical Convert
					if( pThis->m_pcsAgpmItemConvert->GetNumPhysicalConvert( pcsItem ) > 0 )
					{
						sprintf( pcsAuctionBoardUnit->m_pstrItemName + strlen( pcsAuctionBoardUnit->m_pstrItemName ), " + %d",  pThis->m_pcsAgpmItemConvert->GetNumPhysicalConvert( pcsItem ) );
					}

					// Socket Convert
					if( pThis->m_pcsAgpmItemConvert->GetNumSocket( pcsItem ) > 0 )
					{
						sprintf( pcsAuctionBoardUnit->m_pstrItemName + strlen( pcsAuctionBoardUnit->m_pstrItemName ), " [%d/%d]",  pThis->m_pcsAgpmItemConvert->GetNumConvertedSocket( pcsItem ),  pThis->m_pcsAgpmItemConvert->GetNumSocket( pcsItem ) );
					}

					// durability
					INT32 lMaxItemDurability = 0;
					INT32 lMaxDurability = 0;
					INT32 lDurability = 0;

					pThis->m_pcsAgpmFactors->GetValue( &( ( AgpdItemTemplate* ) pcsItem->m_pcsItemTemplate )->m_csFactor, &lMaxItemDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY );
					pThis->m_pcsAgpmFactors->GetValue( &pcsItem->m_csFactor, &lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY );
					pThis->m_pcsAgpmFactors->GetValue( &pcsItem->m_csFactor, &lMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY );

					if( lMaxDurability == ( -1000 ) && lMaxItemDurability > 0 )
					{
						lMaxDurability = lMaxItemDurability;
					}
					else if( lMaxDurability > lMaxItemDurability )
					{
						lMaxDurability = lMaxItemDurability;
					}

					if( lDurability > lMaxDurability || lDurability == ( -1000 ) )
					{
						lDurability = lMaxDurability;
					}
					else if( lDurability < 0 )
					{
						lDurability = 0;
					}

					pThis->m_pcsAgpmFactors->SetValue( &pcsItem->m_csFactor, lDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_DURABILITY );
					pThis->m_pcsAgpmFactors->SetValue( &pcsItem->m_csFactor, lMaxDurability, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_MAX_DURABILITY );
				}
			}

			pThis->m_paBoardUD[ lRow ] = pcsAuctionBoardUnit;
		}

		for( lCounter = 0 ; lCounter < 10 ; ++lCounter )
		{
			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstPageButtonStaticUD[ lCounter ] );
		}

		pThis->m_pstBoardUD->m_stUserData.m_lCount = pstRowSet->m_ulRows;

		//쌔워줬으니~~~ 화면 갱신!! 냠냠~ ^^;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstBoardUD );
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstPageInfoUD );
		return TRUE;
	}
	
	return FALSE;
}

BOOL AgcmUIAuction::CBOpenAuctionUI( PVOID pData, PVOID pClass, PVOID pCustData )
{

	if( pClass )
	{
		AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;

		AgcdUI*		pUI		=	pThis->m_pcsAgcmUIManager2->GetUI( "UI_PrivateTrade" );

		if( pUI->m_pcsUIWindow->m_bOpened )
		{
			char* pMsg = pThis->m_pcsAgcmUIManager2->GetUIMessage( "CannotAuctionInPrivateTrade" );
			if( pMsg && strlen( pMsg ) > 0 )
			{
				pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( pMsg );
			}

			return FALSE;
		}

		pThis->m_stUIOpenPos = pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_stPos;
		pThis->m_bIsUIOpen = TRUE;
		pThis->m_bNowProgress = FALSE;	// 매번 열 때 초기화 해준다. 2008.01.08. steeple

		return pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventOpenBoard );
	}

	return FALSE;
}

BOOL AgcmUIAuction::CBSellUI( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData )	return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgpmAuctionArg* pAuctionArg = ( AgpmAuctionArg* )pData;

	CHAR szMsg[ 255 ] = {0, };
	if( AGPMAUCTION_EXCPT_SUCCESS == pAuctionArg->m_lResult )
	{
		AgpdItem* pAgpdItem = pThis->m_pcsAgpmItem->GetItem( pAuctionArg->m_lItemID );
		if( !pAgpdItem ) return FALSE;
			
		if( AP_SERVICE_AREA_WESTERN == g_eServiceArea )
		{
			sprintf( szMsg, ClientStr().GetStr( STI_ITEM_REGISTERED ), pAuctionArg->m_nQuantity, pAgpdItem->m_pcsItemTemplate->m_szName, pAuctionArg->m_lMoney );
		}
		else
		{
			sprintf( szMsg, ClientStr().GetStr( STI_ITEM_REGISTERED ), pAgpdItem->m_pcsItemTemplate->m_szName, pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney );
		}

		SystemMessage.ProcessSystemMessage( szMsg );
	}
	else if( AGPMAUCTION_EXCPT_EXCEED_REGISTRATION_LIMIT == pAuctionArg->m_lResult )
	{
		// 최대 등록 개수 초과
		sprintf( szMsg, ClientStr().GetStr( STI_CANT_REGISTER ) );
		SystemMessage.ProcessSystemMessage( szMsg );
	}
	else if( AGPMAUCTION_EXCPT_NO_TICKET == pAuctionArg->m_lResult )
	{
		sprintf( szMsg, ClientStr().GetStr( STI_CANT_REGISTER ) );
		SystemMessage.ProcessSystemMessage( szMsg );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBCancelUI( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData ) return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgpmAuctionArg* pAuctionArg = ( AgpmAuctionArg* )pData;

	ApSafeArray< CHAR, 255 > strMessage;
	strMessage.MemSetAll();

	if( AGPMAUCTION_EXCPT_SUCCESS == pAuctionArg->m_lResult )
	{
		AgpdItemTemplate* pAgpdItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate( pAuctionArg->m_lItemTID );
		if( !pAgpdItemTemplate ) return FALSE;
			
		if( AP_SERVICE_AREA_WESTERN == g_eServiceArea )
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_REGISTER_CANCEL ), pAuctionArg->m_nQuantity, pAgpdItemTemplate->m_szName );
		}
		else
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_REGISTER_CANCEL ), pAgpdItemTemplate->m_szName, pAuctionArg->m_nQuantity );
		}

		SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
	}
	else if( AGPMAUCTION_EXCPT_FULL_INVENTORY == pAuctionArg->m_lResult )
	{
		sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_CANT_CANCEL ) );
		SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBSelfUpdatePosition( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pData || !pClass )	return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgpdCharacter* pcsCharacter	= ( AgpdCharacter* )pData;
	if( !pThis->m_bIsUIOpen ) return TRUE;

	FLOAT fDistance = AUPOS_DISTANCE_XZ( pcsCharacter->m_stPos, pThis->m_stUIOpenPos );
	if( ( INT32 ) fDistance < AGCMUIAUCTION_CLOSE_UI_DISTANCE )	return TRUE;

	pThis->m_bIsUIOpen = FALSE;
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventCloseBoard );
	return TRUE;
}

BOOL AgcmUIAuction::CBConfirmUI( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData )	return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgpmAuctionArg* pAuctionArg = ( AgpmAuctionArg* )pData;

	ApSafeArray< CHAR, 255 > strMessage;
	strMessage.MemSetAll();

	if( AGPMAUCTION_EXCPT_SUCCESS == pAuctionArg->m_lResult )
	{
		AgpdItemTemplate* pAgpdItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate( pAuctionArg->m_lItemTID );
		if( !pAgpdItemTemplate ) return FALSE;
			
		if( AP_SERVICE_AREA_WESTERN == g_eServiceArea )
		{
			sprintf(&strMessage[ 0 ], ClientStr().GetStr( STI_SELL_CONFIRM ), pAuctionArg->m_nQuantity, pAgpdItemTemplate->m_szName, pAuctionArg->m_lMoney );
		}
		else
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_SELL_CONFIRM ), pAgpdItemTemplate->m_szName,	pAuctionArg->m_nQuantity, pAuctionArg->m_lMoney );
		}

		SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBBuyUI(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pClass || !pData || !pCustData ) return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgpmAuctionArg* pAuctionArg = ( AgpmAuctionArg* )pData;
	AgpdCharacter* pAgpdCharacter = ( AgpdCharacter* )pCustData;

	ApSafeArray< CHAR, 255 > strMessage;
	strMessage.MemSetAll();

	if( AGPMAUCTION_EXCPT_SUCCESS == pAuctionArg->m_lResult )
	{
		AgpdItemTemplate* pcsAgpdItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate( pAuctionArg->m_lItemTID );
		if( !pcsAgpdItemTemplate ) return FALSE;

		if( AP_SERVICE_AREA_WESTERN == g_eServiceArea )
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_BUY_COMPLETE ), pAuctionArg->m_nQuantity, pcsAgpdItemTemplate->m_szName, pAuctionArg->m_lMoney );
		}
		else
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_BUY_COMPLETE ), pcsAgpdItemTemplate->m_szName, pAuctionArg->m_nQuantity,	pAuctionArg->m_lMoney );
		}

		SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
	}
	else if( AGPMAUCTION_EXCPT_INSUFFICIENT_QTY == pAuctionArg->m_lResult )
	{
		sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_COUNT_RANGE ) );
		SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
	}
	else if( AGPMAUCTION_EXCPT_FULL_INVENTORY == pAuctionArg->m_lResult )
	{
		sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_NO_INVENTORY ) );
		SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
	}
	else if( AGPMAUCTION_EXCPT_NOT_EXIST == pAuctionArg->m_lResult )
	{
		sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_ALREADY_SELL ) );
		SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBUpdateUI( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData ) return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;	
	INT32 lSalesID = ( INT32 )( ( PVOID* )pData )[ 0 ];
	INT16 nStatus = ( INT16 )( ( PVOID* )pData )[ 1 ];
	AgpdCharacter* pAgpdCharacter = ( AgpdCharacter* )pCustData;

	ApSafeArray< CHAR, 255 > strMessage;
	strMessage.MemSetAll();
	
	AgpdAuctionSales* pAgpdAuctionSales = pThis->m_pcsAgpmAuction->GetSales( lSalesID );
	if( !pAgpdAuctionSales ) return FALSE;

	if( AGPMAUCTION_SALES_STATUS_COMPLETE == nStatus )
	{
		AgpdItemTemplate* pAgpdItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate( pAgpdAuctionSales->m_lItemTID );
		if( !pAgpdAuctionSales ) return FALSE;

		if( g_eServiceArea == AP_SERVICE_AREA_WESTERN )
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_SELL_COMPLETED ), pAgpdAuctionSales->m_nQuantity, pAgpdItemTemplate->m_szName, pAgpdAuctionSales->m_lPrice );
		}
		else
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_SELL_COMPLETED ), pAgpdItemTemplate->m_szName, pAgpdAuctionSales->m_nQuantity, pAgpdAuctionSales->m_lPrice );
		}

		SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
	}
	
	return TRUE;
}

BOOL AgcmUIAuction::GetCategoryText( INT32 lDepth, INT32 lCategory )
{
	for( int lCounter = 0  ; lCounter < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ; lCounter++ )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_lCategoryEnableItem, lCounter );
	}

	if( lDepth == 0 )
	{
		//Depth는 최상위이다.
		INT32 lIndex = 0;
		INT32 lCounter = 0;

		ApAdmin* pcsCategory1Admin = m_pcsAgpmAuctionCategory->GetCategory1();
		AgpdAuctionCategory1Info** ppcsAuctionCategory1Info = ( AgpdAuctionCategory1Info** )pcsCategory1Admin->GetObjectSequence( &lIndex );

		for( lCounter = 0 ; ppcsAuctionCategory1Info ; ++lCounter )
		{
			AgcdUICategoryInfoUnit* pcsData = pcsData = new AgcdUICategoryInfoUnit;
			if( pcsData )
			{
				pcsData->m_pstrCategoryName = ( *ppcsAuctionCategory1Info )->m_strName;
				pcsData->m_lCategoryID = ( *ppcsAuctionCategory1Info )->m_lCategoryID;

				m_paCategoryUD[ lCounter ] = pcsData;
			}

			ppcsAuctionCategory1Info = ( AgpdAuctionCategory1Info** )pcsCategory1Admin->GetObjectSequence( &lIndex );
		}

		//몇개인지 세팅하고~
		m_pstCategoryUD->m_stUserData.m_lCount = lCounter;
		m_lCurrentCategory0ID = lCategory;
	}
	else if( lDepth == 1 )
	{
		ApAdmin* pcsCategory1Admin = m_pcsAgpmAuctionCategory->GetCategory1();
		ApAdmin* pcsCategory2Admin = m_pcsAgpmAuctionCategory->GetCategory2();

		AgpdAuctionCategory1Info** ppcsAuctionCategory1Info = ( AgpdAuctionCategory1Info** )pcsCategory1Admin->GetObject( lCategory );
		if( ppcsAuctionCategory1Info && *ppcsAuctionCategory1Info )
		{
			INT32 lChildCount = ( *ppcsAuctionCategory1Info )->m_ChildCount;
			INT32 lCategory2Count = 0;

			for( int nCategoryCount = 0 ; nCategoryCount < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ; ++nCategoryCount )
			{
				if( m_paCategoryUD[ nCategoryCount ] )
				{
					delete m_paCategoryUD[ nCategoryCount ];
					m_paCategoryUD[ nCategoryCount ] = NULL;
				}
			}

			ZeroMemory( m_paCategoryUD, sizeof( AgcdUICategoryInfoUnit* ) * AGCM_UI_AUCTION_MAX_CATEGORY_COUNT );
			for( int lCounter = 0 ; lCounter < lChildCount ; ++lCounter )
			{
				AgpdAuctionCategory2Info** ppcsAuctionCategory2Info = ( AgpdAuctionCategory2Info** )pcsCategory2Admin->GetObject( ( *ppcsAuctionCategory1Info )->m_plChildID[ lCounter ] );
				if( ppcsAuctionCategory2Info && *ppcsAuctionCategory2Info )
				{
					AgcdUICategoryInfoUnit* pcsData = new AgcdUICategoryInfoUnit;
					if( pcsData )
					{
						pcsData->m_lCategoryID = ( *ppcsAuctionCategory2Info )->m_lCategoryID;
						pcsData->m_pstrCategoryName = ( *ppcsAuctionCategory2Info )->m_strName;

						m_paCategoryUD[ lCounter ] = pcsData;
						++lCategory2Count;
					}
				}
			}

			//몇개인지 세팅하고~
			m_pstCategoryUD->m_stUserData.m_lCount = lCategory2Count;
		}
	}
	else if( lDepth == 2 )
	{
		ApAdmin* pcsCategory2Admin = m_pcsAgpmAuctionCategory->GetCategory2();
		AgpdAuctionCategory2Info** ppcsAuctionCategory2Info = ( AgpdAuctionCategory2Info** )pcsCategory2Admin->GetObject( lCategory );;

		if( ppcsAuctionCategory2Info && *ppcsAuctionCategory2Info )
		{
			INT32 lChildCount = ( *ppcsAuctionCategory2Info )->m_ChildCount;
			INT32 lCategory3Count = 0;

			for( int lCounter = 0 ; lCounter < lChildCount ; ++lCounter )
			{
				INT32 lItemTID = ( *ppcsAuctionCategory2Info )->m_plChildID[ lCounter ];

				AgpdItemTemplate* pcsAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate( lItemTID );
				if( pcsAgpdItemTemplate )
				{
					AgcdUICategoryInfoUnit* pcsData = new AgcdUICategoryInfoUnit;
					if( pcsData )
					{
						pcsData->m_pstrCategoryName = (char*)pcsAgpdItemTemplate->m_szName.c_str();
						pcsData->m_lCategoryID = pcsAgpdItemTemplate->m_lID;

						m_paCategoryUD[ lCounter ] = pcsData;
						++lCategory3Count;
					}
				}
			}

			//몇개인지 세팅하고~
			m_pstCategoryUD->m_stUserData.m_lCount = lCategory3Count;
		}
	}

	switch( m_lTreeDepth )
	{
	case 0 :		m_pcsAgcmUIManager2->ThrowEvent( m_lEnableProgress0 );		break;
	case 1 :		m_pcsAgcmUIManager2->ThrowEvent( m_lEnableProgress1 );		break;
	case 2 :		m_pcsAgcmUIManager2->ThrowEvent( m_lEnableProgress2 );		break;
	}

	return TRUE;
}

BOOL AgcmUIAuction::SetCategoryText( void )
{
	ApAdmin* pcsCategory1Admin = m_pcsAgpmAuctionCategory->GetCategory1();
	ApAdmin* pcsCategory2Admin = m_pcsAgpmAuctionCategory->GetCategory2();

	AgpdAuctionCategory2Info	**ppcsAuctionCategory2Info;
	AgpdItemTemplate			*pcsAgpdItemTemplate;

	INT32 lArraryCounter = 0;
	INT32 lIndex = 0;

	AgpdAuctionCategory1Info** ppcsAuctionCategory1Info = ( AgpdAuctionCategory1Info** )pcsCategory1Admin->GetObjectSequence( &lIndex );

	//Depth1의 정보들을 얻는다.
	for( ; ppcsAuctionCategory1Info ; )
	{
		if( ppcsAuctionCategory1Info && ( *ppcsAuctionCategory1Info ) )
		{
			m_lAuctionCategoryStatus[ lArraryCounter ] = AGCM_UI_AUCTION_DEPTH_1;
			m_lAuctionIDListDepth[ lArraryCounter ] = AGCM_UI_AUCTION_DEPTH_1 - 1;
			m_lAuctionItemTIDList[ lArraryCounter ] = ( *ppcsAuctionCategory1Info )->m_lCategoryID;
			lArraryCounter++;

			//Depth2의 정보들을 얻는다.
			INT32 lChildCount = ( *ppcsAuctionCategory1Info )->m_ChildCount;
			for( int lCounter = 0 ; lCounter < lChildCount ; lCounter++ )
			{
				ppcsAuctionCategory2Info = ( AgpdAuctionCategory2Info** )pcsCategory2Admin->GetObject( ( *ppcsAuctionCategory1Info )->m_plChildID[ lCounter ] );
				if( ppcsAuctionCategory2Info )
				{
					m_lAuctionCategoryStatus[ lArraryCounter ] = AGCM_UI_AUCTION_DEPTH_2;
					m_lAuctionIDListDepth[ lArraryCounter ] = AGCM_UI_AUCTION_DEPTH_2 - 1;
					m_lAuctionItemTIDList[ lArraryCounter ] = ( *ppcsAuctionCategory2Info )->m_lCategoryID;
					lArraryCounter++;

					//Depth3의 정보들을 얻는다.
					int nIndexStart = lArraryCounter;
					INT32 lChildCount2 = ( *ppcsAuctionCategory2Info )->m_ChildCount;
					for( int lCounter2 = 0 ; lCounter2 < lChildCount2 ; lCounter2++ )
					{
						INT32 lItemTID = ( *ppcsAuctionCategory2Info )->m_plChildID[ lCounter2 ];
						pcsAgpdItemTemplate = m_pcsAgpmItem->GetItemTemplate( lItemTID );

						if( pcsAgpdItemTemplate )
						{
							m_lAuctionCategoryStatus[ lArraryCounter ] = AGCM_UI_AUCTION_DEPTH_3;
							m_lAuctionIDListDepth[ lArraryCounter ] = AGCM_UI_AUCTION_DEPTH_3 - 1;
							m_lAuctionItemTIDList[ lArraryCounter ] = pcsAgpdItemTemplate->m_lID;
							lArraryCounter++;
						}
					}

					int nIndexEnd = lArraryCounter;
					_SortCategoryItemByLevel( nIndexStart, nIndexEnd );
				}
			}
		}

		ppcsAuctionCategory1Info = ( AgpdAuctionCategory1Info** )pcsCategory1Admin->GetObjectSequence( &lIndex );
	}

	//유저데이터 카운트세팅.
	m_pstUDMainTree->m_stUserData.m_lCount = lArraryCounter;
	for( int nUserDataCount = 0 ; nUserDataCount < m_pstUDMainTree->m_stUserData.m_lCount ; ++nUserDataCount )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_lEventListButtonOff, nUserDataCount );
	}
	
	return TRUE;
}

AgpdItem* AgcmUIAuction::GetItemInstanceOnBoard( INT32 lIndex )
{
	if( 0 <= lIndex && lIndex < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT )
	{
		return m_pacsAgpdItemOnBoard[ lIndex ];
	}

	return NULL;
}

BOOL AgcmUIAuction::SetCallbackSetFocus( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMUIAUCTIONCB_ID_SETFOCUS_ONBOARD, pfCallback, pClass );
}

BOOL AgcmUIAuction::SetCallbackKillFocus( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMUIAUCTIONCB_ID_KILLFOCUS_ONBOARD, pfCallback, pClass );
}

BOOL AgcmUIAuction::CBOpenUI( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgcdUIControl* pcsControl = ( AgcdUIControl* ) pData1;

	if( pThis )
	{
		BOOL bCloseAllUIResult;

		//옥션을 열기전에 관계없는 모든 UI를 싸악~ 지운다.
		pThis->m_lTreeDepth = 0;
		pThis->m_pcsAgcmUIManager2->CloseAllUIExceptMainUI( &bCloseAllUIResult );
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstUDMainTree );
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstBoardUD );

		//Page Info
		for( int lCounter = 0 ; lCounter < 10 ; ++lCounter )
		{
			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstPageButtonStaticUD[ lCounter ] );
		}

		pThis->m_bIsAuctionUIOpen = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIAuction::CBOpenAnywhere( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;
	
	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;	
	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	
	if( pThis->m_pcsAgpmAuction->IsAbleToOpenAnywhere( pcsCharacter ) )
	{
		pThis->m_pcsAgcmAuction->SendOpenAnywhere( pcsCharacter->m_lID );
	}
	
	return TRUE;
}

BOOL AgcmUIAuction::CBDisplayAuctionTree( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if( pClass )
	{
		AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
		CHAR strDisplayData[ 128 ] = { 0, };

		INT32 lDepth = pThis->m_lAuctionCategoryStatus[ pcsSourceControl->m_lUserDataIndex ];
		INT32 lID = pThis->m_lAuctionItemTIDList[ pcsSourceControl->m_lUserDataIndex ];
		if( lDepth == 0 || lID == 0 ) return FALSE;

		if( lDepth == AGCM_UI_AUCTION_DEPTH_1 )
		{
			ApAdmin* pcsCategory1Admin = pThis->m_pcsAgpmAuctionCategory->GetCategory1();
			AgpdAuctionCategory1Info** ppcsAuctionCategory1Info = ( AgpdAuctionCategory1Info** )pcsCategory1Admin->GetObject( lID );

			if( ppcsAuctionCategory1Info && ( *ppcsAuctionCategory1Info ) )
			{
				strcpy_s( strDisplayData, sizeof( CHAR ) * 128, ( *ppcsAuctionCategory1Info )->m_strName );
            }			
		}
		else if( lDepth == AGCM_UI_AUCTION_DEPTH_2 )
		{
			ApAdmin* pcsCategory2Admin = pThis->m_pcsAgpmAuctionCategory->GetCategory2();
			AgpdAuctionCategory2Info** ppcsAuctionCategory2Info = ( AgpdAuctionCategory2Info** )pcsCategory2Admin->GetObject( lID );

			if( ppcsAuctionCategory2Info && ( *ppcsAuctionCategory2Info ) )
			{
				strcpy_s( strDisplayData, sizeof( CHAR ) * 128, ( *ppcsAuctionCategory2Info )->m_strName );
            }			
		}
		else if( lDepth == AGCM_UI_AUCTION_DEPTH_3 )
		{
			AgpdItemTemplate* pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate( lID );
            if( pcsItemTemplate )
			{
				INT32 nLevel = 0;
				pThis->m_pcsAgpmFactors->GetValue( &pcsItemTemplate->m_csRestrictFactor, &nLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );

				sprintf_s( strDisplayData, sizeof( CHAR ) * 128, "%s%d %s", ClientStr().GetStr( STI_LEVEL ), nLevel == 0 ? 1 : nLevel, pcsItemTemplate->m_szName );
			}
		}

		sprintf( szDisplay, "%s", strDisplayData );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBDisplayCategory( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING )	return FALSE;

	AgcdUICategoryInfoUnit** ppcsCategoryInfo = ( AgcdUICategoryInfoUnit** )pData;
	if( !ppcsCategoryInfo || !( *ppcsCategoryInfo ) ) return FALSE;

	sprintf( szDisplay, "%s", ( *ppcsCategoryInfo )->m_pstrCategoryName );
	return TRUE;
}

BOOL AgcmUIAuction::CBDisplayBoardCategoryName( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING )	return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	if( pThis->m_lBoardCurrentTID )
	{
		AgpdItemTemplate* pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate( pThis->m_lBoardCurrentTID );
		if( pcsItemTemplate )
		{
			sprintf( szDisplay, "%s", pcsItemTemplate->m_szName );
		}
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBDisplayDoc( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING )	return FALSE;

	AgcdUIAuctionBoardUnit** ppcsBoardInfo = ( AgcdUIAuctionBoardUnit** )pData;
	if( ( *ppcsBoardInfo ) )
	{
		// 2006.05.31. steeple
		// VS2005 에서 lld 로 찍으면 이상한 값이 찍힌다. -_-; 고로 d 로 변경.
		//sprintf(szDisplay, "%lld", (*ppcsBoardInfo)->m_lIndex );
		sprintf( szDisplay, "%d", ( *ppcsBoardInfo )->m_lIndex );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBDisplayItemName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING )	return FALSE;

	AgcdUIAuctionBoardUnit** ppcsBoardInfo = ( AgcdUIAuctionBoardUnit** )pData;
	if( ( *ppcsBoardInfo ) )
	{
		sprintf( szDisplay, "%s", ( *ppcsBoardInfo )->m_pstrItemName );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBDisplayPrice( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING )	return FALSE;

	AgcdUIAuctionBoardUnit** ppcsBoardInfo = ( AgcdUIAuctionBoardUnit** )pData;

	if( ( *ppcsBoardInfo ) )
	{
		sprintf( szDisplay, "%d", ( *ppcsBoardInfo )->m_lPrice );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBDisplayQuantity( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING )	return FALSE;

	AgcdUIAuctionBoardUnit** ppcsBoardInfo = ( AgcdUIAuctionBoardUnit** )pData;
	if( ( *ppcsBoardInfo ) )
	{
		sprintf( szDisplay, "%d", ( *ppcsBoardInfo )->m_lCount );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBDisplaySellerName( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pData || !pClass || eType != AGCDUI_USERDATA_TYPE_STRING )	return FALSE;

	AgcdUIAuctionBoardUnit** ppcsBoardInfo = ( AgcdUIAuctionBoardUnit** )pData;
	if( ( *ppcsBoardInfo ) )
	{
		sprintf( szDisplay, "%s", ( *ppcsBoardInfo )->m_pstrSellerName );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBReturnConfirmBuyItemFromAuction( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	if( !pClass ) return FALSE;
	if( lTrueCancel )
	{
		AgcmUIAuction*		pThis			= ( AgcmUIAuction* )pClass;
		AgpdCharacter*		pAgpdCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();

		// 구입 키를 누르면 다시한번 금액을 체크한다
		if( pAgpdCharacter->m_llMoney < pThis->m_llBuyPrice )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lBoardNotEnoughMoney );
			return FALSE;
		}

		//구입신호를 보낸다.
		INT32 lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;

		pThis->m_pcsAgcmAuction->SendBuy( lCID, pThis->m_llBuyDocID, pThis->m_lBuyStackItemCount, pThis->m_llBuyPrice , pThis->m_lBoardCurrentTID);
		pThis->SetCooldown( lCID, pThis->m_lBoardCurrentTID, pThis->m_ullFirstDocID + 1, 0 );
	}

	return TRUE;	
}

BOOL AgcmUIAuction::CBSetFocusOnACBoard( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgpdItem* pcsAgpdItem = pThis->GetItemInstanceOnBoard( pcsSourceControl->m_lUserDataIndex );

	return pThis->EnumCallback( AGCMUIAUCTIONCB_ID_SETFOCUS_ONBOARD, ( void* )pcsAgpdItem, NULL );
}

BOOL AgcmUIAuction::CBKillFocusOnACBoard( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( !pClass ) return FALSE;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgpdItem* pcsAgpdItem = pThis->GetItemInstanceOnBoard( pcsSourceControl->m_lUserDataIndex );

	return pThis->EnumCallback( AGCMUIAUCTIONCB_ID_KILLFOCUS_ONBOARD, ( void* )pcsAgpdItem, NULL );
}

BOOL AgcmUIAuction::CBSelectCategory( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( pClass && pcsSourceControl )
	{
		AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;

		// 진행상황 창이 떠 있으면 그냥 나간다. 2008.01.08. steeple
		if( pThis->m_bNowProgress )	return TRUE;
	
		pThis->m_ullFirstDocID = AGCMAUCTION_MAX_DOCID;
		pThis->m_ullLastDocID = 0;
		pThis->m_lBoardCurrentStartPage	= 0;

		return pThis->SelectCategory( pcsSourceControl->m_lUserDataIndex );
	}

	return FALSE;
}

BOOL AgcmUIAuction::SelectCategory( INT32 lIndex )
{
	if( m_lAuctionIDListDepth[ lIndex ] == ( AGCM_UI_AUCTION_DEPTH_3 - 1 ) )
	{
		INT32 lCID = m_pcsAgcmCharacter->m_lSelfCID;
		INT32 lItemTID = m_lAuctionItemTIDList[ lIndex ];
		m_lBoardCurrentRequetedPage = 1;
		m_lBoardCurrentTID = lItemTID;
		m_lPageDirection = AGCM_UIAUCTION_PAGE_DIRECTION_NONE;

		//그리드세팅
		AgpdItemTemplate* pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate( lItemTID );
		AgpdItemTemplate* pcsOldItemTemplate = m_pcsAgpmItem->GetItemTemplate( m_lOldBoardGridItemTID );

		if( pcsOldItemTemplate )
		{
			//먼저 있던 그리드용 아이템 텍스쳐는 지운다.
			m_pcsAgcmItem->ReleaseGridItemAttachedTexture( pcsOldItemTemplate );
		}

		if( pcsItemTemplate )
		{
			m_pcsAgpmGrid->Reset( &m_csBoardItemTemplateGrid );

			if( m_pcsAgcmItem->SetGridItemAttachedTexture( pcsItemTemplate ) )
			{
				m_pcsAgpmGrid->Add( &m_csBoardItemTemplateGrid, 0, 0, 0, pcsItemTemplate->m_pcsGridItem, 1, 1 );
			}

			m_lOldBoardGridItemTID = lItemTID;
		}

		m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstCateogryNameUD );
		m_pcsAgcmUIManager2->SetUserDataRefresh( m_pcsUserDataBoardItemTemplateGrid );

		SetCooldown( lCID, m_lBoardCurrentTID, AGCMAUCTION_MAX_DOCID, 0 );

		m_pcsAgcmUIManager2->ThrowEvent( m_lEnablePage1 );

		for( int nUserDataCount = 0; nUserDataCount < m_pstUDMainTree->m_stUserData.m_lCount; ++nUserDataCount )
		{
			m_pcsAgcmUIManager2->ThrowEvent( m_lEventListButtonOff, nUserDataCount );
		}
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBBackToParentButton( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if( pClass )
	{
		AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;

		if( pThis->m_lTreeDepth > 0 )
		{
			if( pThis->m_lTreeDepth == 1 )
			{
				--pThis->m_lTreeDepth;

				pThis->m_lCurrentCategory1ID = 0;
				pThis->GetCategoryText( pThis->m_lTreeDepth, 0 );
				pThis->m_pstCategoryUD->m_lStartIndex = 0;
				pThis->m_pstCategoryUD->m_bUpdateList = TRUE;
				pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstCategoryUD );
			}
			else if( pThis->m_lTreeDepth == 2 )
			{
				--pThis->m_lTreeDepth;

				pThis->m_lCurrentCategory2ID = 0;
				pThis->GetCategoryText( pThis->m_lTreeDepth, pThis->m_lCurrentCategory0ID );
				pThis->m_pstCategoryUD->m_lStartIndex = 0;
				pThis->m_pstCategoryUD->m_bUpdateList = TRUE;
				pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstCategoryUD );
			}
		}
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBSearchCategoryButton( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	UINT32 ulCurrTick = GetTickCount();
	if( ulCurrTick - g_ulPrevTick < BUTTON_COOLTIME ) return TRUE;

	g_ulPrevTick = ulCurrTick;

	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	AgcdUIControl* pcsControl = ( AgcdUIControl* )pData1;
	AgcdUIControl* pcsTreeControl = ( AgcdUIControl* )pData2;

	if( pcsControl->m_lType != AcUIBase::TYPE_EDIT || !pcsControl->m_pcsBase ) return FALSE;

	ApSafeArray< CHAR, 255 > strMessage;
	strMessage.MemSetAll();

	char* pstrItemName = ( char* )( ( AcUIEdit* )( pcsControl->m_pcsBase ) )->GetText();
	if( pstrItemName )
	{
		AgpdItemTemplate* pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate( pstrItemName );
		if( !pcsItemTemplate )
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_WRONG_WORD ) );
			return SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
		}

		pThis->m_lTreeDepth = 2;
		pThis->m_lCurrentCategory0ID = pcsItemTemplate->m_lFirstCategory;
		pThis->m_lCurrentCategory1ID = pcsItemTemplate->m_lSecondCategory;
		pThis->m_lCurrentCategory2ID = 0;

		pThis->GetCategoryText( 2, pcsItemTemplate->m_lSecondCategory );
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstCategoryUD );

		( ( AcUIEdit* )( pcsControl->m_pcsBase ) )->SetText( "" );

		BOOL bFind = FALSE;
		for( int nCategoryCount = 0; nCategoryCount < AGCM_UI_AUCTION_MAX_CATEGORY_COUNT; ++nCategoryCount )
		{
			if( pThis->m_lAuctionItemTIDList[ nCategoryCount ] == pcsItemTemplate->m_lID )
			{
				( ( AcUITree* )pcsTreeControl->m_pcsBase )->OpenItem( nCategoryCount );
				pThis->SelectCategory( nCategoryCount );
				bFind = TRUE;
				break;
			}
		}

		if( !bFind )
		{
			sprintf( &strMessage[ 0 ], ClientStr().GetStr( STI_WRONG_CATEGORY ) );
			return SystemMessage.ProcessSystemMessage( &strMessage[ 0 ] );
		}

		return TRUE;
	}
	
	return FALSE;
}

BOOL AgcmUIAuction::CBPrevPage( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	UINT32 ulCurrTick = GetTickCount();
	if( ulCurrTick - g_ulPrevTick < BUTTON_COOLTIME ) return TRUE;

	g_ulPrevTick = ulCurrTick;
	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;

	if( pThis )
	{
		// 진행상황 창이 떠 있으면 그냥 나간다. 2008.01.08. steeple
		if( pThis->m_bNowProgress )	return TRUE;
		if( pThis->m_lBoardCurrentRequetedPage > 1 )
		{
			INT32 lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;
			INT32 lItemTID = pThis->m_lBoardCurrentTID;

			pThis->m_lBoardCurrentRequetedPage -= 1;
			if( pThis->m_lBoardCurrentRequetedPage - pThis->m_lBoardCurrentStartPage < 1 )
			{
				pThis->m_lBoardCurrentStartPage	= pThis->m_lBoardCurrentRequetedPage - 1;
			}
	
			pThis->m_lPageDirection = AGCM_UIAUCTION_PAGE_DIRECTION_LEFT;
			if( 1 == pThis->m_lBoardCurrentRequetedPage )
			{
				pThis->SetCooldown( lCID, lItemTID, AGCMAUCTION_MAX_DOCID, 0 );
			}
			else
			{
				pThis->SetCooldown( lCID, lItemTID, pThis->m_ullFirstDocID, 1 );
			}

			for( int lCounter = 0; lCounter < 10; ++lCounter )
			{
				pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstPageButtonStaticUD[ lCounter ] );
			}

			switch( pThis->m_lBoardCurrentRequetedPage - pThis->m_lBoardCurrentStartPage - 1 )
			{
			case 0 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage1 );		break;
			case 1 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage2 );		break;
			case 2 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage3 );		break;
			case 3 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage4 );		break;
			case 4 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage5 );		break;
			case 5 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage6 );		break;
			case 6 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage7 );		break;
			case 7 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage8 );		break;
			case 8 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage9 );		break;
			case 9 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage10 );		break;
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIAuction::CBNextPage( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	UINT32 ulCurrTick = GetTickCount();
	if( ulCurrTick - g_ulPrevTick < BUTTON_COOLTIME ) return TRUE;

	g_ulPrevTick = ulCurrTick;
	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	
	if( pThis )
	{
		// 진행상황 창이 떠 있으면 그냥 나간다. 2008.01.08. steeple
		if( pThis->m_bNowProgress ) return TRUE;

		if( pThis->m_lBoardCurrentRequetedPage < pThis->m_lBoardMaxPage )
		{
			INT32 lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;
			INT32 lItemTID = pThis->m_lBoardCurrentTID;

			pThis->m_lBoardCurrentRequetedPage += 1;
			if( pThis->m_lBoardCurrentRequetedPage - pThis->m_lBoardCurrentStartPage > 10 )
			{
				pThis->m_lBoardCurrentStartPage	= pThis->m_lBoardCurrentRequetedPage - 10;
			}

			pThis->m_lPageDirection = AGCM_UIAUCTION_PAGE_DIRECTION_RIGHT;
			pThis->SetCooldown( lCID, lItemTID, pThis->m_ullLastDocID, 0 );

			for( int lCounter = 0 ; lCounter < 10 ; ++lCounter )
			{
				pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pstPageButtonStaticUD[ lCounter ] );
			}

			switch( pThis->m_lBoardCurrentRequetedPage - pThis->m_lBoardCurrentStartPage - 1 )
			{
			case 0 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage1 );	break;
			case 1 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage2 );	break;
			case 2 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage3 );	break;
			case 3 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage4 );	break;
			case 4 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage5 );	break;
			case 5 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage6 );	break;
			case 6 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage7 );	break;
			case 7 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage8 );	break;
			case 8 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage9 );	break;
			case 9 :	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEnablePage10 );	break;
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIAuction::CBPageInfo( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;

	if( pThis->m_lBoardCurrentStartPage == 0 && pThis->m_lBoardMaxPage == 0 )
	{
		sprintf( szDisplay, "" );
	}
	else
	{
		sprintf( szDisplay, "%d/%d", pThis->m_lBoardCurrentRequetedPage, pThis->m_lBoardMaxPage );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo1( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 1;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo2( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 2;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo3(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 3;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo4(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 4;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo5(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 5;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo6(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 6;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo7(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 7;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo8(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 8;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo9(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 9;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

BOOL AgcmUIAuction::CBPageInfo10(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIAuction* pAuction = ( AgcmUIAuction* )pClass;
	INT32 nPageNumber = 10;

	if( pAuction->m_lBoardCurrentStartPage + nPageNumber <= pAuction->m_lBoardMaxPage )
	{
		sprintf( szDisplay, "%d", pAuction->m_lBoardCurrentStartPage + nPageNumber );
	}
	else
	{
		sprintf( szDisplay, "" );
	}

	return TRUE;
}

// 2008.01.08. steeple
// Cooldown 기능을 추가하였다.
BOOL AgcmUIAuction::SetCooldown( INT32 lCID, INT32 lTID, UINT64 ullDOCID, INT16 nFlag, INT32 lCustom )
{
	if( m_bNowProgress ) return FALSE;

	m_bNowProgress = TRUE;
	AgcmUIAuctionProgress* pcsAuctionProgress = new AgcmUIAuctionProgress( lCID, lTID, ullDOCID, nFlag, lCustom );
	return m_pcsAgcmUICooldown->StartCooldown( m_lCooldownID, PROGRESS_COOLTIME, 0, 1, pcsAuctionProgress );
}

BOOL AgcmUIAuction::CBAfterCooldown( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pCustData ) return FALSE;
		
	AgcmUIAuction* pThis = ( AgcmUIAuction* )pClass;
	INT32 lCooldownID = ( INT32 )pData;
	AgcmUIAuctionProgress* pcsAuctionProgress = static_cast< AgcmUIAuctionProgress* >( pCustData );
	
	pThis->m_bNowProgress = FALSE;
	if( pThis->m_lCooldownID != lCooldownID ) return FALSE;

	pThis->m_pcsAgcmAuction->SendSelect( pcsAuctionProgress->m_lCID, pcsAuctionProgress->m_lTID, pcsAuctionProgress->m_ullDOCID, pcsAuctionProgress->m_nFlag, pcsAuctionProgress->m_lCustom );
	delete pcsAuctionProgress;
	pcsAuctionProgress = NULL;

	return TRUE;
}

INT32 AgcmUIAuction::_GetAuctionQuantity( INT32 nID )
{
	INT32 nFindCount = 0;

	return nFindCount;
}

void AgcmUIAuction::_SortCategoryItemByLevel( int nIndexStart, int nIndexEnd )
{
	m_vecSortCategoryItem.Clear();

	if( nIndexStart < 0 || nIndexEnd < 0 ) return;
	if( nIndexStart >= AGCM_UI_AUCTION_MAX_CATEGORY_COUNT || nIndexEnd >= AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ) return;

	// 일단 정렬대상을 수집해서..
	for( int nCount = nIndexStart ; nCount < nIndexEnd ; nCount++ )
	{
		stCategoryItemEntry NewEntry;

		NewEntry.m_nTID = m_lAuctionItemTIDList[ nCount ];
		NewEntry.m_nLevel = _GetItemLevelByTID( NewEntry.m_nTID );

		//AgpdItemTemplate* ppdItemTemplate = m_pcsAgpmItem->GetItemTemplate( NewEntry.m_nTID );

		//char strDebug[ 256 ] = { 0, };
		//sprintf( strDebug, "Add CategoryItem, Name = %s, TID = %d, Level = %d\n", ppdItemTemplate->m_szName, NewEntry.m_nTID, NewEntry.m_nLevel );
		//OutputDebugString( strDebug );

		m_vecSortCategoryItem.Add( NewEntry );
	}

	// 레벨값 기준으로 정렬
	int nItemCount = m_vecSortCategoryItem.GetSize();
	for( int nCount1 = 0 ; nCount1 < nItemCount - 1 ; nCount1++ )
	{
		for( int nCount2 = nCount1 + 1 ; nCount2 < nItemCount ; nCount2++ )
		{
			stCategoryItemEntry* pEntry1 = m_vecSortCategoryItem.Get( nCount1 );
			stCategoryItemEntry* pEntry2 = m_vecSortCategoryItem.Get( nCount2 );

			if( pEntry1 && pEntry2 )
			{
				if( pEntry1->m_nLevel > pEntry2->m_nLevel )
				{
					//AgpdItemTemplate* ppdItemTemplate1 = m_pcsAgpmItem->GetItemTemplate( pEntry1->m_nTID );
					//AgpdItemTemplate* ppdItemTemplate2 = m_pcsAgpmItem->GetItemTemplate( pEntry2->m_nTID );

					//char strDebug[ 256 ] = { 0, };
					//sprintf( strDebug, "Swap CategoryItem, [ Name = %s, Level = %d ] <-> [ Name = %s, Level = %d ]\n",
					//	ppdItemTemplate1->m_szName, pEntry1->m_nLevel, ppdItemTemplate2->m_szName, pEntry2->m_nLevel );
					//OutputDebugString( strDebug );

					stCategoryItemEntry stTemp = *pEntry1;
					*pEntry1 = *pEntry2;
					*pEntry2 = stTemp;
				}
			}
		}
	}

	// 정렬된거대로 다시 적어주기
	for( int nCount = nIndexStart ; nCount < nIndexEnd ; nCount++ )
	{
		stCategoryItemEntry* pEntry = m_vecSortCategoryItem.Get( nCount - nIndexStart );
		if( pEntry )
		{
			//AgpdItemTemplate* ppdItemTemplate = m_pcsAgpmItem->GetItemTemplate( pEntry->m_nTID );

			//char strDebug[ 256 ] = { 0, };
			//sprintf( strDebug, "Result Sort, Index = %d, Name = %s, TID = %d, Level = %d\n", nCount - nIndexStart, ppdItemTemplate->m_szName, pEntry->m_nTID, pEntry->m_nLevel );
			//OutputDebugString( strDebug );

			m_lAuctionItemTIDList[ nCount ] = pEntry->m_nTID;
		}
	}
}

INT32 AgcmUIAuction::_GetItemLevelByTID( INT32 nTID )
{
	AgpmItem* ppmItem = ( AgpmItem* )GetModule( "AgpmItem" );
	AgpmFactors* ppmFactor = ( AgpmFactors* )GetModule( "AgpmFactors" );
	if( !ppmItem || !ppmFactor ) return -1;

	AgpdItemTemplate* ppdItemTemplate = ppmItem->GetItemTemplate( nTID );
	if( !ppdItemTemplate ) return -1;

	INT32 nLevel = 0;
	ppmFactor->GetValue( &ppdItemTemplate->m_csRestrictFactor, &nLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL );
	return nLevel;
}
