#ifndef __AGCM_UIAUCTION_H__
#define __AGCM_UIAUCTION_H__

#include "AgpmGrid.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgpmAuctionCategory.h"
#include "AgpmAuction.h"
#include "AgcmAuction.h"
#include "AgcmItem.h"
#include "AgcmUIManager2.h"
#include "AgcmUISplitItem.h"
#include "AgcmUICooldown.h"
#include "AgcmChatting2.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmUIAuctionD" )
#else
#pragma comment ( lib , "AgcmUIAuction" )
#endif
#endif

#define	AGCM_UI_AUCTION_MAX_CATEGORY_COUNT							5000
#define AGCM_UI_AUCTION_MAX_BOARD_COUNT								15
#define	AGCM_UI_AUCTION_MAX_CATEGORY_STRING_SIZE					80

#define AGCM_UI_AUCTION_DEPTH_1										1
#define AGCM_UI_AUCTION_DEPTH_2										2
#define AGCM_UI_AUCTION_DEPTH_3										3

#define	AGCMUIAUCTION_CLOSE_UI_DISTANCE								150

typedef enum _AgcmUIAuctonPageDirection
{
	AGCM_UIAUCTION_PAGE_DIRECTION_NONE								= 0,
	AGCM_UIAUCTION_PAGE_DIRECTION_LEFT,
	AGCM_UIAUCTION_PAGE_DIRECTION_RIGHT,
} AgcmUIAuctonPageDirection;

typedef enum _AgcmUIAuction_Display_ID 
{
	AGCM_UIAUCTION_DISPLAY_NONE										= 0,
	
} AgcmUIAuction_Display_ID;

typedef enum AgcmUIAuctionCB
{
	AGCMUIAUCTIONCB_ID_SETFOCUS_ONBOARD								= 0,
	AGCMUIAUCTIONCB_ID_KILLFOCUS_ONBOARD,
};

struct stCategoryItemEntry
{
	int																m_nTID;
	int																m_nLevel;

	stCategoryItemEntry( void )
	{
		m_nTID = -1;
		m_nLevel = -1;
	};

	void operator = ( stCategoryItemEntry& rEntry )
	{
		m_nTID = rEntry.m_nTID;
		m_nLevel = rEntry.m_nLevel;
	};
};

class AgcdUIAuctionBoardUnit
{
public:
	INT64															m_llDocID;			//번호
	INT32															m_lIndex;			//인덱스
	char*															m_pstrItemName;		//판매 아이템 이름
	INT32															m_lPrice;			//가격
	INT32															m_lCount;			//수량
	char*															m_pstrSellerName;	//판매자이름

	AgcdUIAuctionBoardUnit( void )
	{
		m_llDocID			= 0;
		m_lIndex			= 0;
		m_pstrItemName		= NULL;
		m_lPrice			= 0;
		m_lCount			= 0;
		m_pstrSellerName	= NULL;
	}

	~AgcdUIAuctionBoardUnit( void )
	{
		Reset();
	}
	
	void Reset();
};

class AgcdUIAuctionBoardInfo
{
	AgcdUIAuctionBoardUnit											m_csBoard[ AGCM_UI_AUCTION_MAX_BOARD_COUNT ];

public:
	AgcdUIAuctionBoardInfo( void );

public :
	void						clear								( void );
	INT32						SetData								( INT64 llDocID, INT32 lIndex, char *pstrItemName,
																		INT32 lPrice, INT32 lCount, char *pstrSellerName );
	AgcdUIAuctionBoardUnit*		GetData								( INT32 lIndex );
};

class AgcdUICategoryInfoUnit
{
public:
	char*															m_pstrCategoryName;
	INT32															m_lCategoryID;

	AgcdUICategoryInfoUnit( void )
	{
		m_pstrCategoryName = NULL;
		m_lCategoryID = 0;
	}
};

class AgcmUICategoryInfo
{
	AgcdUICategoryInfoUnit											m_csCategory[ AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ];

public:
	AgcmUICategoryInfo( void );
	void						Clear								( void );
	INT32						SetData								( char *pstrName, INT32 lCategory );
	AgcdUICategoryInfoUnit*		GetData								( INT32 lIndex );
};

class AgcmUIAuctionProgress
{
public:
	INT32															m_lCID;
	INT32															m_lTID;
	UINT64															m_ullDOCID;
	INT16															m_nFlag;
	INT32															m_lCustom;

	AgcmUIAuctionProgress( INT32 lCID, INT32 lTID, UINT64 ullDOCID, INT16 nFlag, INT32 lCustom = 0 )
		: m_lCID( lCID ), m_lTID( lTID ), m_ullDOCID( ullDOCID ), m_nFlag( nFlag ), m_lCustom( lCustom )
	{
	}

private:
	AgcmUIAuctionProgress( void )
	{
	}
};

class AgcmUIAuction : public AgcModule
{
private:
	AgpmGrid*														m_pcsAgpmGrid;
	AgpmFactors*													m_pcsAgpmFactors;
	AgpmCharacter*													m_pcsAgpmCharacter;
	AgcmCharacter*													m_pcsAgcmCharacter;
	AgpmItem*														m_pcsAgpmItem;
	AgpmItemConvert*												m_pcsAgpmItemConvert;
	AgpmAuctionCategory*											m_pcsAgpmAuctionCategory;
	AgpmAuction*													m_pcsAgpmAuction;
	AgcmAuction*													m_pcsAgcmAuction;
	AgcmItem*														m_pcsAgcmItem;
	AgcmUIManager2*													m_pcsAgcmUIManager2;
	AgcmUISplitItem*												m_pcsAgcmUISplitItem;
	AgcmUICooldown*													m_pcsAgcmUICooldown;
	AgcmChatting2*													m_pcsAgcmChatting;
	AgpdItem*														m_pacsAgpdItemOnBoard[ AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ];

	char															m_strCategorySearchEdit[ 80 ];

	INT64															m_llBuyDocID;
	INT32															m_lBuyStackItemCount;
	INT64															m_llBuyPrice;
	INT32															m_lOldBoardGridItemTID;

	//현재 Board정보, TID, Page정보
	INT32															m_lBoardCurrentTID;
	INT32															m_lBoardCurrentStartPage;
	INT32															m_lBoardCurrentRequetedPage;
	INT32															m_lBoardMaxPage;
	INT32															m_lPageDirection;

	AgcdUIUserData*													m_pcsUserDataBoardItemTemplateGrid;
	AgpdGrid														m_csBoardItemTemplateGrid;

	//현재 트리 깊이
	INT32															m_lTreeDepth;
	INT32															m_lCurrentCategory0ID;
	INT32															m_lCurrentCategory1ID;
	INT32															m_lCurrentCategory2ID;
	BOOL															m_bIsAuctionUIOpen;

	//Tree
	ApSafeArray< INT32, AGCM_UI_AUCTION_MAX_CATEGORY_COUNT > 		m_lAuctionItemTIDList;
	ApSafeArray< INT32, AGCM_UI_AUCTION_MAX_CATEGORY_COUNT > 		m_lAuctionIDListDepth;
	ApSafeArray< INT32, AGCM_UI_AUCTION_MAX_CATEGORY_COUNT > 		m_lAuctionCategoryStatus;

	//UserData
	AgcdUIUserData*													m_pstUDMainTree;
	AgcdUIUserData*													m_pstUDMainTreeDepth;

	AgcdUICategoryInfoUnit*											m_paCategoryUD[ AGCM_UI_AUCTION_MAX_CATEGORY_COUNT ];
	AgcdUIUserData*													m_pstCategoryUD;

	AgcdUIAuctionBoardUnit*											m_paBoardUD[ AGCM_UI_AUCTION_MAX_BOARD_COUNT ];
	AgcdUIUserData*													m_pstBoardUD;

	char															m_strCategoryItemName[ 255 ];
	AgcdUIUserData*													m_pstCateogryNameUD;

	char															m_strPageInfo[ 255 ];
	AgcdUIUserData*													m_pstPageInfoUD;

	INT32															m_aiPageButtonStatic[ 10 ];
	AgcdUIUserData*													m_pstPageButtonStaticUD[ 10 ];
	AgcdUIUserData*													m_pcsCategorySearchEdit;

	//Event ID List
	INT32															m_lEventOpenBoard;
	INT32															m_lEventCloseBoard;
	INT32															m_lBoardBuyMessageBox;
	INT32															m_lBoardNotEnoughMoney;

	INT32															m_lCategoryEnableItem;
	INT32															m_lCategoryDisableItem;

	//Progress Status
	INT32															m_lEnableProgress0;
	INT32															m_lEnableProgress1;
	INT32															m_lEnableProgress2;

	//Page Status
	INT32															m_lEnablePage1;
	INT32															m_lEnablePage2;
	INT32															m_lEnablePage3;
	INT32															m_lEnablePage4;
	INT32															m_lEnablePage5;
	INT32															m_lEnablePage6;
	INT32															m_lEnablePage7;
	INT32															m_lEnablePage8;
	INT32															m_lEnablePage9;
	INT32															m_lEnablePage10;

	INT32															m_lEventListButtonOn;
	INT32															m_lEventListButtonOff;
	
	UINT64															m_ullFirstDocID;	//현재 Page 첫 DocID
	UINT64															m_ullLastDocID;		//현재 Page 마지막 DocID

	BOOL															m_bIsUIOpen;
	AuPOS															m_stUIOpenPos;

	INT32															m_lCooldownID;
	BOOL															m_bNowProgress;

	ContainerVector< stCategoryItemEntry >							m_vecSortCategoryItem;

public:
	AgcmUIAuction( void );
	~AgcmUIAuction( void );

	BOOL						OnAddModule							( void );
	BOOL						OnDestroy							( void );
	BOOL						OnInit								( void );

	BOOL						AddEvent							( void );
	BOOL						AddFunction							( void );
	BOOL						AddDisplay							( void );
	BOOL						AddUserData							( void );

	BOOL						GetCategoryText						( INT32 lDepth, INT32 lCategory );
	BOOL						SetCategoryText						( void );
	BOOL						SelectCategory						( INT32 lIndex );

	AgpdItem*					GetItemInstanceOnBoard				( INT32 lIndex );

	BOOL 						SetCallbackSetFocus					( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL 						SetCallbackKillFocus				( ApModuleDefaultCallBack pfCallback, PVOID pClass );
	BOOL 						SetCallbackSelect					( ApModuleDefaultCallBack pfCallback, PVOID pClass );

	//Functions 1. 기본연산.
	static BOOL					CBOpenUI							( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBOpenAnywhere						( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );

	//Functions 2. 카테고리쪽.
	static BOOL 				CBDisplayAuctionTree				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBDisplayCategory					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBSelectCategory					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL					CBBackToParentButton				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBSearchCategoryButton				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );

	//Functions 3. 보드
	static BOOL 				CBDisplayBoardCategoryName			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBDisplayDoc						( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBDisplayItemName					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBDisplayPrice						( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBDisplayQuantity					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBDisplaySellerName					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBBuyItemFromAuction				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );

	static BOOL 				CBBoardSelect						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBOpenAuctionUI						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBSellUI							( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBCancelUI							( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBConfirmUI							( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBBuyUI								( PVOID pData, PVOID pClass, PVOID pCustData );	
	static BOOL 				CBUpdateUI							( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBReturnConfirmBuyItemFromAuction	( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );

	static BOOL 				CBSetFocusOnACBoard					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBKillFocusOnACBoard				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL					CBSelfUpdatePosition				( PVOID pData, PVOID pClass, PVOID pCustData );

	//Functions 4. 페이지처리
	static BOOL 				CBPrevPage							( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBNextPage							( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBPageInfo							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo1							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo2							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo3							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo4							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo5							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo6							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo7							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo8							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo9							( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL 				CBPageInfo10						( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	//	Cooldown Callback
	BOOL						SetCooldown							( INT32 lCID, INT32 lTID, UINT64 ullDOCID, INT16 nFlag, INT32 lCustom = 0 );
	static BOOL					CBAfterCooldown						( PVOID pData, PVOID pClass, PVOID pCustData );

private :
	// nID 에 해당하는 종류의 아이템이 거래소에 몇개나 등록되어 있는가?
	INT32						_GetAuctionQuantity					( INT32 nID );

	// 특정 카테고리 내의 목록을 레벨순으로 정렬
	void						_SortCategoryItemByLevel			( int nIndexStart, int nIndexEnd );

	// 특정 TID 로 해당 아이템의 기본 레벨을 가져옴
	INT32						_GetItemLevelByTID					( INT32 nTID );
};


#endif