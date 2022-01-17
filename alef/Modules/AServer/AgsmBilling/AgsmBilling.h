/*===================================================================

	AgsmBilling.h

===================================================================*/

#ifndef	_AGSM_BILLING_H_
	#define	_AGSM_BILLING_H_

#include "AgsEngine.h"
#include "AgsmServerManager2.h"
#include "AgsmCharacter.h"
#include "AgsdBilling.h"
#include "AgsaBilling.h"
#include "AgsdItem.h"
#include "AgsmSystemMessage.h"//JK_½É¾ß¼¦´Ù¿î
#ifdef _WEBZEN_BILLING_
#include "WebzenBilling_kor.h"
#endif
/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMBILLING_CB
	{
	AGSMBILLING_CB_RESULT_GETMONEY = 0,
	AGSMBILLING_CB_RESULT_BUYITEM,
	};


const enum eAGSMBILLING_CONNECT_RESULT
	{
	AGSMBILLING_CONNECT_RESULT_SUCCESS = 0,
	AGSMBILLING_CONNECT_RESULT_NOT_EXIST_SERVER,
	AGSMBILLING_CONNECT_RESULT_FAIL,
	AGSMBILLING_CONNECT_RESULT_NEED_WAIT,
	};




/************************************************/
/*		The Definition of AgsmBilling class		*/
/************************************************/
//
class AgpmItem;
class AgsmItem;
class AgsmItemManager;
class AgpmCashMall;
class AgsmCashMall;
class AgsmBillingJapan;
class AgsmBillingChina;
class AgsmBillingGlobal;//JK_ºô¸µ
class AgsmBillingWebzen;//JK_ºô¸µÀ¥Á¨
class CCallBackFunc;
class AgpmBillInfo;//JK_ºô¸µ
class AgsmBillInfo;
class AgpmFactors;
class AgsmSkill;

// ÇÑ±¹°ú ºÏ¹Ì´Â ºô¸µ ¸ðµâÀÌ ¿ÏÀüÈ÷ °°´Ù.

class AgsmBilling : public AgsModule
	{
	friend CCallBackFunc;

	private :
		static AgsmBilling* m_pInstance;//JK_ºô¸µ

		// Related modules
		AgsmCharacter		*m_pAgsmCharacter;
		AgpmCharacter*		m_pagpmCharacter;//JK_½É¾ß¼¦´Ù¿î
		AgsmServerManager2	*m_pAgsmServerManager2;
		AgpmCashMall		*m_pAgpmCashMall;
		AgsmCashMall		*m_pAgsmCashMall;
		AgsmItemManager		*m_pAgsmItemManager;
		AgpmItem			*m_pAgpmItem;
		AgsmItem			*m_pAgsmItem;
		AgsmSkill			*m_pagsmSkill;//JK_ºô¸µ
		AgsmSystemMessage	*m_pagsmSystemMessage;//JK_½É¾ß¼¦´Ù¿î



		// Billing server
		AgsmBillingChina	*m_pAgsmBillingChina;
		AgsmBillingJapan	*m_pAgsmBillingJapan;
		AgsdServer			*m_pAgsdServerBilling;
		AgsmBillingGlobal	*m_pAgsmBillingGlobal;//JK_ºô¸µ
		AgsmBillingWebzen	*m_pAgsmBillingWebzen;//JK_ºô¸µÀ¥Á¨
		
		ApMemoryPool		m_PacketMemoryPool;
		AuGenerateID		m_GenerateID;
		AgsaBilling			m_Admin;

		AgpmBillInfo*		m_pagpmBillInfo;//JK_ºô¸µ
		AgsmBillInfo*		m_pagsmBillInfo;
		AgpmFactors*		m_pagpmFactors;
		AgpmConfig*			m_pAgpmConfig;	//JK_½É¾ß¼¦´Ù¿î


		UINT32				m_ulLastCheckClockClock;
		UINT32				m_ulLastCheckClockForHanGame;
		UINT32			    m_ulLastCheckClockForWebzen;
		UINT32				m_ulLastCheckClockForMidNightShutDown; //JK_½É¾ß¼¦´Ù¿î


		//JK_À¥Á¨ºô¸µ
		int					m_nYear;			// À¥Á¨ ÅëÇÕ ºô¸µ¿¡¼­ »ç¿ëÇÏ´Â ShopScript ¹öÀü
		int					m_nYearIndentity;	// À¥Á¨ ÅëÇÕ ºô¸µ¿¡¼­ »ç¿ëÇÏ´Â ShopScript ¹öÀü
		
	public :
		AgsmBilling();
		virtual ~AgsmBilling();

		static AgsmBilling* GetInstance()//JKºô¸µ
		{
			return m_pInstance;
		};

		//	ApModule inherited
		BOOL	OnAddModule() { return (this->*OnAddModulePtr)(); }
		BOOL	OnAddModuleCn( void );
		BOOL	OnAddModuleJp( void );
		BOOL	OnAddModuleGlobal();//JK_ºô¸µ

		BOOL	OnInit( void ) { return (this->*OnInitPtr)(); }
		BOOL	OnInitCn( void );
		BOOL	OnInitJp( void );
#ifdef _AREA_CHINA_
		BOOL	OnIdle2(UINT32 ulClockCount);
#endif
#ifdef _WEBZEN_BILLING_
		BOOL	OnInitGlobal();
		BOOL	OnIdle2(UINT32 ulClockCount);
#endif
		//	Admin
		AgsdBilling*	Get(INT32 lID);
		AgsdBilling*	Get(CHAR* szAccountID);//JK_ºô¸µ
		BOOL			Add(AgsdBilling *pAgsdBilling);
		BOOL			Add(AgsdBilling* pAgsdBilling, CHAR* AccountID);//JK_ºô¸µ
		BOOL			Remove(INT32 lID);
		BOOL			Remove(CHAR* AccountID);//JK_ºô¸µ

		void	SetOrderSeed();
		
		//	Socket callback point
		static BOOL	DispatchBilling(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
		static BOOL DisconnectBilling(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
	
	private :
		//	Packet processing
		BOOL	OnReceive(PVOID pvPacket, UINT32 ulNID);
		void	InitFuncPtr( void );

		// Function Pointers
		BOOL	(AgsmBilling::*OnAddModulePtr)( void );
		BOOL	(AgsmBilling::*OnInitPtr)( void );
		BOOL	(AgsmBilling::*WaitForConnectServerPtr)( void );
		void	(AgsmBilling::*BillingSvrDisconnectPtr)( void );
		BOOL	(AgsmBilling::*OnDisconnectPtr)( void );
		BOOL	(AgsmBilling::*SendGetCashMoneyPtr)(AgpdCharacter*);
		BOOL	(AgsmBilling::*SendBuyCashItemPtr)(AgpdCharacter*, INT32, CHAR*, INT64, stCashItemBuyList&);
		eAGSMBILLING_CONNECT_RESULT	(AgsmBilling::*ConnectBillingServerPtr)(void);
		
	public :	
		//	Connection
		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServer();

		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServerCn();
		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServerJp();
		eAGSMBILLING_CONNECT_RESULT ConnectBillingServerGlobal();//JK_ºô¸µ

		BOOL		WaitForConnectServerGlobal();//JK_ºô¸µ

		static BOOL	CheckConnectBillingServer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

		BOOL		CheckBillingServer();				// operation check
		
		BOOL		OnDisconnect() { return (this->*OnDisconnectPtr)(); }
		BOOL		OnDisconnectKr();
		BOOL		OnDisconnectJp();
		BOOL		OnDisconnectCn();
		
		//	Billing methods
		BOOL	SendGetCashMoney(AgpdCharacter *pAgpdCharacter){ return (this->*SendGetCashMoneyPtr)(pAgpdCharacter); }
		BOOL	SendGetCashMoneyCn(AgpdCharacter* pAgpdCharacter);
		BOOL	SendGetCashMoneyJp(AgpdCharacter* pAgpdCharacter);
		BOOL	SendGetCashMoneyGlobal(AgpdCharacter* pAgpdCharacter);//JK_ºô¸µ


		BOOL	SendBuyCashItem(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemKr(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemCn(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemJp(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemGlobal(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, UINT64 ullListSeq, INT32 lItemTID, INT32 lType);//JK_ºô¸µ
		//JK_½É¾ß¼¦´Ù¿î
		void	KickOffUnderAge();
		//JK_½É¾ß¼¦´Ù¿î
		void	SendMidNightShutDownMessageAll(INT32 nLeftMin);




#ifdef _WEBZEN_BILLING_//JK_ºô¸µ
		BOOL	OnInquireCash(long ReturnCode, CHAR* AccountID, double WCoin, double PCoin );
		//JK_À¥Á¨ºô¸µ
		BOOL	OnInquireCash(long ReturnCode, DWORD AccountGUID, double WCoin );
		BOOL	OnBuyProduct(long ResultCode, CHAR* AccounID, DWORD DeductCashSeq);
		BOOL	OnInquirePersonDeduct(DWORD AccountGUID, DWORD ResultCode);
		BOOL	OnUserStatus(DWORD AccountGUID, DWORD BillingGUID, DWORD RealEndDate, DOUBLE RestTime, INT32 DeductType);
		//JK_À¥Á¨ºô¸µ : À¥Á¨ÅëÇÕºô¸µ
		BOOL	OnUserStatusWebzen(DWORD AccountGUID, DWORD BillingGUID, DWORD RealEndDate, DOUBLE RestTime, INT32 DeductType);
		//JK_À¥Á¨ºô¸µ
		BOOL	OnBuyProductWebzen(DWORD dwAccountGUID, DWORD dwResultCode, DWORD wdLeftProductCount);
		//JK_À¥Á¨ºô¸µ
		BOOL	OnUseStorage(DWORD AccountGUID, DWORD ResultCode, CHAR* InGameProductID, BYTE PropertyCount, DWORD ProductSeq, DWORD StorageSeq, DWORD StorageItemSeq, STItemProperty* pPropertyList);
		//JK_À¥Á¨ºô¸µ
		BOOL	OnInquireStorageListPageGiftMessage(DWORD AccountGUID, DWORD ResultCode, CHAR StorageType, int NowPage, int TotalPage, int TotalCount, int ListCount, STStorageNoGiftMessage* StorageList);



		void	CheckIn(AgpdCharacter *pAgpdCharacter);
		void	CheckOut(AgpdCharacter *pAgpdCharacter);
		//JK_PC¹æ°ú±Ý»óÅÂÈ®ÀÎ
		void	ReloginWebzen(DWORD dwAccountGUID);


		void	SetSalesZoneScriptVersion(int Year, int YearIndentity);
		//JK_À¥Á¨ºô¸µ
		void	UpdateScriptVersionAllUser(int Year, int YearIdentity);
		BOOL    SendToClientSalesZoneScriptVersion(AgpdCharacter *pAgpdCharacter);
		//JK_À¥Á¨ºô¸µ
		BOOL	SendUseStorageToBilling(AgpdCharacter *pAgpdCharacter, INT32 StorageSeq, INT32 StorageItemSeq);
		//JK_À¥Á¨ºô¸µ
		BOOL	SendBuyCashItemWebzen(AgpdCharacter *pAgpdCharacter, INT32 PackageSeq, INT32 DisplaySeq, INT32 PriceSeq);
		//JK_À¥Á¨ºô¸µ
		BOOL	SendInquireStorageListToBilling(AgpdCharacter *pAgpdCharacter, INT32 NowPage);



		//JK_À¥Á¨ºô¸µ
		static BOOL     CBSendToClientSalesZoneScriptVersion(PVOID pData, PVOID pClass, PVOID pCustData);

#endif
		//	Callback setting method(for result processing)		
		BOOL	SetCallbackGetCashMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBuyCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		// 2008.01.11. steeple
		// ÇÑ°ÔÀÓ ºô¸µ ¹Ù²¼´Ù. ¾Æ ÃÊ ¿Õ Â¥Áõ
#ifdef _AREA_KOREA_
		static BOOL	CBAddServer(PVOID pData, PVOID pClass, PVOID pCustData);// { return (*CBAddServerPtr)(pData, pClass, pCustData); }

		BOOL	OnInitKr( void );
		BOOL	OnAddModuleKr( void );
		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServerKr();
		BOOL	SendGetCashMoneyKr(AgpdCharacter* pAgpdCharacter);
#ifdef _HANGAME_
		BOOL	ConnectHanGameBilling();
		BOOL	GetHanCoin(AgpdCharacter* pcsCharacter);
#endif
		BOOL	BuyCashItem(AgpdCharacter* pcsCharacter, INT32 lProductID, CHAR* pszDesc, INT64 llMoney, UINT64 ullListSeq, INT32 lItemTID);
#endif
	};


#endif