#ifdef _AREA_KOREA_

#ifndef __CLASS_WEBZEN_SHOP_H__
#define __CLASS_WEBZEN_SHOP_H__





#include "CExNPCDialog.h"
#include "CUICashMallTexture.h"
#include "AgcdUIManager2.h"


#pragma warning( disable : 4996 )


#ifdef _DEBUG
	#pragma comment( lib, "WebzenShopScript_d.lib" )
#else
	#pragma comment( lib, "WebzenShopScript.lib" )
#endif


struct stWebzenShopButton
{
	CExNPCDialogButton*									m_pButton;
	int													m_nCategoryId;

	stWebzenShopButton( void )
	{
		m_pButton = NULL;
		m_nCategoryId = 0;
	}
};

struct stWebzenProductView
{
	int													m_nIndex;

	CUICashMallTexture*									m_pImage;

	CUICashMallTexture*									m_pImageEvent;
	CUICashMallTexture*									m_pImageEventBG;

	CUICashMallTexture*									m_pImageHot;
	CUICashMallTexture*									m_pImageHotBG;

	CUICashMallTexture*									m_pImageNew;
	CUICashMallTexture*									m_pImageNewBG;

	AcUIEdit*											m_pEditName;
	AcUIEdit*											m_pEditDesc;
	AcUIEdit*											m_pEditPrice;

	CExNPCDialogButton*									m_pButtonBuy;

	stWebzenProductView( void )
	{
		m_nIndex = 0;
		m_pImage = NULL;

		m_pImageEvent = NULL;
		m_pImageEventBG = NULL;

		m_pImageHot = NULL;
		m_pImageHotBG = NULL;

		m_pImageNew = NULL;
		m_pImageNewBG = NULL;

		m_pEditName = NULL;
		m_pEditDesc = NULL;
		m_pEditPrice = NULL;

		m_pButtonBuy = NULL;
	}
};

struct stWebzenProductPageButton
{
	AcUIButton*											m_pButton;
	int													m_nIndex;
	int													m_nPageNumber;

	stWebzenProductPageButton( void )
	{
		m_pButton = NULL;
		m_nIndex = -1;
		m_nPageNumber = 0;
	}
};

class CWebzenShopClient
{
private :
	BOOL												m_bIsDialogOpen;
	BOOL												m_bIsProductViewCreate;
	int													m_nCurrentCategory;

	int													m_nCurPage;
	int													m_nMaxPage;

	int													m_nEvent_ShopOpen;
	int													m_nEvent_ShopClose;

	int													m_nEvent_BuyInfoOpen;
	int													m_nEvent_BuyInfoClose;

	ContainerMap< int, stWebzenShopButton >				m_mapButtonCategory;
	ContainerMap< int, stWebzenProductView >			m_mapViewProduct;
	ContainerMap< int, stWebzenProductPageButton >		m_mapPageButtons;

	__int64												m_nCash;
	int													m_nBuyInfo_PackageId;
	int													m_nBuyInfo_CategoryId;
	int													m_nBuyInfo_PriceSeq;

public :
	CWebzenShopClient( void );
	virtual ~CWebzenShopClient( void );

public :
	BOOL				OnWebzenShopInitialize			( void );
	BOOL				OnWebzenShopDestroy				( void );

	BOOL				OnWebzenShopShow				( void );
	BOOL				OnWebzenShopClose				( void );

	BOOL				OnShowBuyInformation			( int nCateogryIndex, int nProductIndex );
	BOOL				OnRefreshBuyInformation			( int nCateogryIndex, int nProductIndex );
	BOOL				OnSelectCategory				( int nCategoryIndex );

	BOOL				OnSelectPagePrev				( void );
	BOOL				OnSelectPageNext				( void );
	BOOL				OnSelectPage					( int nPageIndex );

	BOOL				OnRequestBuyProduct				( int nProductViewIndex );

public :	// 서버로 보내는 패킷
	BOOL				OnSendRequestShopVersion		( void );
	BOOL				OnSendRequestBuyProduct			( void );
	BOOL				OnSendRequestCashData			( void );

public :	// 서버로부터 받은 패킷 처리
	BOOL				OnReceiveWebzenShopVersion		( unsigned short nYear, unsigned short nYearID );
	BOOL				OnReceiveCashData				( __int64 nCash );
	BOOL				OnReceiveBuyResult				( int nResultCode );

private :
	BOOL				_RegisterUIEvent				( void );
	BOOL				_RegisterUICallBack				( void );
	BOOL				_RegisterPacketCallBack			( void );

	void*				_GetWebzenShopUIWindow			( void );

	CExNPCDialogButton*	_CreateCategoryButton			( int categoryId );
	void				_DeleteCategoryButton			( void );

	BOOL				_CollectPageButtons				( void );

	BOOL				_CreateProductView				( void );
	
	CUICashMallTexture*	_CreateProductViewImage			( int nIndex );

	CUICashMallTexture*	_CreateProductViewImageEvent	( int nIndex );
	CUICashMallTexture*	_CreateProductViewImageEventBG	( int nIndex );

	CUICashMallTexture*	_CreateProductViewImageHot		( int nIndex );
	CUICashMallTexture*	_CreateProductViewImageHotBG	( int nIndex );

	CUICashMallTexture*	_CreateProductViewImageNew		( int nIndex );
	CUICashMallTexture*	_CreateProductViewImageNewBG	( int nIndex );

	AcUIEdit*			_CreateProductViewName			( int nIndex );
	AcUIEdit*			_CreateProductViewDesc			( int nIndex );
	AcUIEdit*			_CreateProductViewPrice			( int nIndex );
	
	CExNPCDialogButton*	_CreateProductViewButtonBuy		( int nIndex );

	BOOL				_DeleteProductView				( void );

	CExNPCDialogButton*	_GetCategoryButton				( int categoryId );
	CUICashMallTexture*	_GetProductImage				( int nIndex );

	CUICashMallTexture*	_GetProductImageEvent			( int nIndex );
	CUICashMallTexture*	_GetProductImageEventBG			( int nIndex );

	CUICashMallTexture*	_GetProductImageHot				( int nIndex );
	CUICashMallTexture*	_GetProductImageHotBG			( int nIndex );

	CUICashMallTexture*	_GetProductImageNew				( int nIndex );
	CUICashMallTexture*	_GetProductImageNewBG			( int nIndex );

	AcUIEdit*			_GetProductEditName				( int nIndex );
	AcUIEdit*			_GetProductEditDesc				( int nIndex );
	AcUIEdit*			_GetProductEditPrice			( int nIndex );

	CExNPCDialogButton*	_GetProductButtonBuy			( int nIndex );

public :
	int					FindCategoryIndex				( void* pClickedButton );
	int					FindProductIndex				( void* pClickedButton );
	BOOL				IsEnableBuyCashItem				( int productIdx );
	RwTexture*			GetItemIconTexture				( int nTID );

private :
	BOOL				_RefreshCategoryButtons			( void );
	
	BOOL				_RefreshProductList				( void );
	BOOL				_RefreshProductInfo				( int categoryId, int startIdx, int endIdx );

	BOOL				_RefreshProductImage			( int nIndex, int nItemTID );
	BOOL				_RefreshProductName				( int nIndex, char* pName, BOOL bIsEvent, BOOL bIsHot, BOOL bIsNew );
	BOOL				_RefreshProductDesc				( int nIndex, char* pDesc );
	BOOL				_RefreshProductPrice			( int nIndex, int nPrice );

	BOOL				_RefreshProductBuyButton		( int nIndex, BOOL bIsEnableBuy );

	BOOL				_RefreshPageCount				( void );
	BOOL				_RefreshPageButtons				( void );
	BOOL				_RefreshPageButton				( AcUIButton* pButton, int nPosX, int nPosY, int nPageNumber, BOOL bIsCurPage, BOOL bIsShow );

	BOOL				_RefreshCashData				( void );

	int					_CalcStartPageNumber			( void );
	int					_CalcPageButtonPosX				( int nIndex, int nMaxIndex );

	void				_SaveProductBuyInfo				( int categoryId, int packageId, int priceId );

public :	// 모듈 콜백
	static BOOL			CallBack_OnSetSelfCharacter		( void* pData, void* pClass, void* pCustData );

public :	// UI 콜백
	static BOOL			CallBack_OnSelectCategory		( void* pClass, void* pControl );
	static BOOL			CallBack_OnRequestBuyProduct	( void* pClass, void* pControl );
	static BOOL			CallBack_OnConfirmBuyProduct	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl );
	static BOOL			CallBack_OnClickPageButton		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl );
	static BOOL			CallBack_OnClickPagePrev		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl );
	static BOOL			CallBack_OnClickPageNext		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl );
	static BOOL			CallBack_OnClickRefreshCash		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pcdControl );

public :	// 패킷 콜백
	static BOOL			CallBack_OnReceiveShopVersion	( void* pData, void* pClass, void* pCustData );
	static BOOL			CallBack_OnReceiveCashData		( void* pData, void* pClass, void* pCustData );
	static BOOL			CallBack_OnReceiveBuyResult		( void* pData, void* pClass, void* pCustData );
};



#endif

#endif