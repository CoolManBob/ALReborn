#pragma once

#include "ApModule.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"

#include "AgpdCashMall.h"

#define	AGPMCASHMALL_MAX_TAB_NAME			32
#define AGPMCASHMALL_MAX_TAB_COUNT			10
#define	AGPMCASHMALL_MAX_ITEM_DESCRIPTION	256
#define AGPMCASHMALL_MAX_ITEM_BUY_MESSAGE	256
#define AGPMCASHMALL_MAX_ITEM_TID			10

#define	AGPMCASHMALL_STREAM_PRODUCTID		_T("ProductID")
#define	AGPMCASHMALL_STREAM_ITEMTID			_T("ItemTID")
#define	AGPMCASHMALL_STREAM_COUNT			_T("Count")
#define	AGPMCASHMALL_STREAM_PRICE			_T("Price")
#define	AGPMCASHMALL_STREAM_SPECIALFLAG		_T("SpecialFlag")
#define	AGPMCASHMALL_STREAM_DESCRIPTION		_T("Description")
#define	AGPMCASHMALL_STREAM_BUY_MESSAGE		_T("BuyDescription")
#define AGPMCASHMALL_STREAM_PCBANGBUY		_T("PCBangBuy")

#define	AGPMCASHMALL_STREAM_FLAG_NEW		_T("New")
#define	AGPMCASHMALL_STREAM_FLAG_HOT		_T("Hot")
#define	AGPMCASHMALL_STREAM_FLAG_BEST		_T("Best")
#define	AGPMCASHMALL_STREAM_FLAG_EVENT		_T("Event")
#define AGPMCASHMALL_STREAM_FLAG_SALE		_T("Sale")
#define AGPMCASHMALL_STREAM_FLAG_SPC		_T("SPC")
#define AGPMCASHMALL_STREAM_FLAG_TPACK		_T("TPACK")

typedef enum _AgpmCashSpecialFlag {
	AGPMCASH_SPECIALFLAG_NEW		= 0x0001,
	AGPMCASH_SPECIALFLAG_HOT		= 0x0002,
	AGPMCASH_SPECIALFLAG_BEST		= 0x0004,
	AGPMCASH_SPECIALFLAG_EVENT		= 0x0008,
	AGPMCASH_SPECIALFLAG_SALE		= 0x0010,
	AGPMCASH_SPECIALFLAG_SPC		= 0x0020,
	AGPMCASH_SPECIALFLAG_TPACK		= 0x0040,
};

typedef enum _AgpmCashOperation {
	AGPMCASH_OPERATION_REQUEST_MALL_PRODUCT_LIST		= 1,
	AGPMCASH_OPERATION_RESPONSE_MALL_LIST,
	AGPMCASH_OPERATION_RESPONSE_PRODUCT_LIST,
	AGPMCASH_OPERATION_REQUEST_BUY_ITEM,
	AGPMCASH_OPERATION_RESPONSE_BUY_RESULT,
	AGPMCASH_OPERATION_REFRESH_CASH,
	AGPMCASH_OPERATION_CHECK_LIST_VERSION,	
	AGPMCASH_OPERATION_REQUEST_BUY_ITEM_WEBZEN,		//JK_À¥Á¨ºô¸µ
	AGPMCASH_OPERATION_RESPONSE_BUY_RESULT_WEBZEN,		//JK_À¥Á¨ºô¸µ
	AGPMCASH_OPERATION_USESTORAGE_CSREQ,			//JK_À¥Á¨ºô¸µ
	AGPMCASH_OPERATION_USESTORAGE_SCACK,			//JK_À¥Á¨ºô¸µ
	AGPMCASH_OPERATION_INQUIRESTOAGELIST_CSREQ,		//JK_À¥Á¨ºô¸µ
	AGPMCASH_OPERATION_INQUIRESTOAGELIST_SCACK,		//JK_À¥Á¨ºô¸µ

};

typedef enum _AgpmCashBuyResult {
	AGPMCASH_BUY_RESULT_SUCCESS			= 1,
	AGPMCASH_BUY_RESULT_NOT_ENOUGH_CASH,
	AGPMCASH_BUY_RESULT_INVENTORY_FULL,
	AGPMCASH_BUY_RESULT_NEED_NEW_ITEM_LIST,
	AGPMCASH_BUY_RESULT_PCBANG_ONLY,
	AGPMCASH_BUY_RESULT_LIMITED_COUNT,
	AGPMCASH_BUY_RESULT_FAIL,
	AGPMCASH_BUY_RESULT_NOT_T_PCBANG,
	AGPMCASH_BUY_RESULT_NOT_S_PCBANG,
};

typedef enum _AgpmCashCallback {
	AGPMCASH_CB_REQUEST_MALL_PRODUCT_LIST		= 0,
	AGPMCASH_CB_REQUEST_BUY_ITEM,
	AGPMCASH_CB_RESPONSE_BUY_RESULT,
	AGPMCASH_CB_UPDATE_MALL_LIST,
	AGPMCASH_CB_REFRESH_CASH,
	AGPMCASH_CB_CHECK_LIST_VERSION,
	AGPMCASH_CB_DISABLE_USE_FORAWHILE,
	AGPMCASH_CB_REQUEST_BUY_ITEM_WEBZEN,//JK_À¥Á¨ºô¸µ
	AGPMCASH_CB_REQUEST_USESTORAGE,		//JK_À¥Á¨ºô¸µ
	AGPMCASH_CB_RESPONSE_USESTORAGE,		//JK_À¥Á¨ºô¸µ
	AGPMCASH_CB_REQUEST_INQUIRESTORAGELIST, //JK_À¥Á¨ºô¸µ
	AGPMCASH_CB_RESPONSE_INQUIRESTORAGELIST,
};

typedef struct _AgpmCashTabInfo {
	INT32	*m_plTabProductList;
	ApString<AGPMCASHMALL_MAX_TAB_NAME>	m_szTabName;
	INT32	m_lNumItem;

	_AgpmCashTabInfo()
	{
		m_plTabProductList	= NULL;
		m_lNumItem	= 0;
	}

	~_AgpmCashTabInfo()
	{
		if (m_plTabProductList)
		{
			delete [] m_plTabProductList;

			m_plTabProductList	= NULL;
		}
	}
} AgpmCashTabInfo, *pAgpmCashTabInfo;

typedef struct _AgpmCashItemInfo {
	INT32	m_lProductID;
	ApSafeArray<INT32, AGPMCASHMALL_MAX_ITEM_TID>	m_alItemTID;
	ApSafeArray<INT32, AGPMCASHMALL_MAX_ITEM_TID>	m_alItemQty;
	INT32	m_lNumTotalItems;
	INT64	m_llPrice;
	INT32	m_lSpecialFlag;
	ApString<AGPMCASHMALL_MAX_ITEM_DESCRIPTION>	m_szDescription;
	ApString<AGPMCASHMALL_MAX_ITEM_BUY_MESSAGE>	m_szBuyMessage;

	INT32	m_lPCBangBuy;

	AgpdItem	*m_pcsItem;

	_AgpmCashItemInfo()
	{
		m_lProductID		= 0;
		m_alItemTID.MemSetAll();
		m_alItemQty.MemSetAll();
		m_lNumTotalItems	= 0;
		m_llPrice			= 0;
		m_lSpecialFlag		= 0;
		m_lPCBangBuy		= 0;

		m_pcsItem			= NULL;
	}
} AgpmCashItemInfo, *pAgpmCashItemInfo;

class CashMallInfo {
public:
	INT32				m_lNumTab;
	INT32				m_lNumTabListItem;

	pAgpmCashTabInfo	m_pstMallTabInfo;

	INT32				m_lNumMallItemInfo;

	pAgpmCashItemInfo	m_pstMallItemInfo;

	ApString<128>		m_szFileMallTab;
	ApString<128>		m_szFileMallItemList;

	UINT8				m_ucMallListVersion;
	ApMutualEx			m_Mutex;

	ApAdmin				m_csAdminProduct;

public:
	CashMallInfo();
	~CashMallInfo();

	BOOL				SetMaxProduct(INT32 lMax)
	{
		return m_csAdminProduct.InitializeObject(sizeof(CashMallInfo *), lMax);
	};
};

class AgpmCashMall : public ApModule
{
private:
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmGrid			*m_pcsAgpmGrid;
	AgpmItem			*m_pcsAgpmItem;

	CashMallInfo		m_csCashMallInfo;

	// ¾Æ·¡ EncodeString ¹è¿­ 10°³·Î ´Ã·È´Ù. 2007.08.16. steeple
	ApString<4096>		m_strMallEncodeString;
	ApString<16384>		m_strProductListEncodeString[AGPMCASHMALL_MAX_TAB_COUNT];		// 2007.03.28. laki. Â¯±ú´Â 4096³Ñ¾ú´Ù. -_-; ÆÐÅ¶»çÀÌÁîÁ¦ÇÑÀÌ ÀÖÀ¸´Ï 8192¿¡ ¸¸Á·ÇÏµµ·Ï ÇÏÀÚ. ÂÍ...

	INT32				m_lIndexAttachData;

	BOOL				m_bUseCashMall;

public:
	AuPacket			m_csPacket;

public:
	AgpmCashMall();
	virtual ~AgpmCashMall();

	virtual BOOL	OnAddModule();
	virtual BOOL	OnInit();
	virtual BOOL	OnDestroy();
	virtual BOOL	OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

public:
	CashMallInfo*	GetCashMallInfo();
	BOOL			SetMaxProduct(INT32 lMax);

	pAgpmCashItemInfo	GetCashItem(INT32 lProductID);
	BOOL			AddCashItem(pAgpmCashItemInfo pItemInfo);

	AgpdCashMall*	GetADCharacter(AgpdCharacter *pcsCharacter);

	inline	BOOL	IsEnableUseCashMall() { return m_bUseCashMall; }
	inline  VOID	SetUseCashMall(BOOL bUse) { m_bUseCashMall = bUse; }			


private:
	BOOL	OnOperationRequestMallProductList(INT32 lCID, INT32 lTab);
	BOOL	OnOperationResponseMallList(TCHAR *pszMallList, UINT16 unMallListLength, UINT8 ucMallListVersion);
	BOOL	OnOperationResponseProductList(TCHAR *pszProductList, UINT16 unProductListLength, UINT8 ucMallListVersion, INT32 lTab);
	BOOL	OnOperationRequestBuyItem(INT32 lCID, INT32 lProductID, UINT8 ucMallListVersion);
	BOOL	OnOperationResponseBuyResult(UINT8 cResult);
	BOOL	OnOperationResponseRefreshCash(INT32 lCID);
	BOOL	OnOperationCheckListVersion(INT32 lCID, UINT8 ucMallListVersion);
	//JK_À¥Á¨ºô¸µ
	BOOL	OnOperationRequestBuyItemWebzen(INT32 lCID, INT32 PackageSeq, INT32 DisplaySeq, INT32 PriceSeq);
	//JK_À¥Á¨ºô¸µ
	BOOL	OnOperationRequestUseStorage(INT32 lCID, INT32 StorageSeq, INT32 StorageItemSeq);
	BOOL	OnOperationRequestStorageList(INT32 ICID, INT32 NowPage);



public:
	BOOL	StreamReadCashMallTab(TCHAR *pszFile, BOOL bDecryption);
	BOOL	StreamReadCashMallItemList(TCHAR *pszFile, BOOL bDecryption);

	BOOL	RefreshMallList();

	BOOL	SetCallbackRequestMallProductList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRequestBuyItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackResponseBuyResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackUpdateMallList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackRefreshCash(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackCheckListVersion(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackDisableForaWhile(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//JK_À¥Á¨ºô¸µ
	BOOL	SetCallbackRequestBuyItemWebzen(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	//JK_À¥Á¨ºô¸µ
	BOOL	SetCallbackRequestUseStorage(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackResponseUseStorage(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL    SetCallbackRequestInquireStorageList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL    SetCallbackResponseInquireStorageList(ApModuleDefaultCallBack pfCallback, PVOID pClass);



	BOOL	EncodeTabList();
	BOOL	DecodeTabList(TCHAR *pszMallList, UINT16 unMallListLength);

	BOOL	EncodeProductList();
	BOOL	DecodeProductList(TCHAR *pszProductList, UINT16 unProductListLength, INT32 lTab);

public:
	PVOID	MakePacketRequestMallInfo(INT32 lCID, INT16 *pnPacketLength, INT32 lTab);
	PVOID	MakePacketMallTabInfo(INT16 *pnPacketLength);
	PVOID	MakePacketMallProductInfo(INT16 *pnPacketLength, INT32 lTab);

	PVOID	MakePacketBuyProduct(INT32 lProductID, INT32 lCID, INT16 *pnPacketLength);
	PVOID	MakePacketBuyResult(INT8 lResult, INT16 *pnPacketLength);

	PVOID	MakePacketRefreshCash(INT32 lCID, INT16 *pnPacketLength);
	PVOID	MakePacketCheckListVersion(INT32 lCID, INT16 *pnPacketLength);
};