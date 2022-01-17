/*=============================================================

	AgsdAuctionRelay.h

=============================================================*/

#ifndef _AGSD_AUCTION_RELAY_H_
	#define _AGSD_AUCTION_RELAY_H_

#include "ApBase.h"
#include "AgpdLog.h"
#include "AgsdDatabase.h"
#include "AgsdRelay2.h"
#include "AgpmItem.h"
#include "AgpdAuction.h"
#include "AgpmAuction.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGSMAUCTIONRELAY_PARAM
	{
	AGSMAUCTIONRELAY_PARAM_BASE = AGSMDATABASE_PARAM_AUCTION,		// 40
	AGSMAUCTIONRELAY_PARAM_SELL,
	AGSMAUCTIONRELAY_PARAM_CANCEL,
	AGSMAUCTIONRELAY_PARAM_CONFIRM,	
	AGSMAUCTIONRELAY_PARAM_BUY,
	AGSMAUCTIONRELAY_PARAM_SELECT,
	AGSMAUCTIONRELAY_PARAM_CHAR,
	AGSMAUCTIONRELAY_PARAM_SELECT_SALES,
	AGSMAUCTIONRELAY_PARAM_SELECT_SALES_COUNT
	};


enum eAGSMAUCTIONRELAY_CHAR_OP
	{
	AGSMAUCTIONRELAY_CHAR_OP_NONE = 0,
	AGSMAUCTIONRELAY_CHAR_OP_ENTER,
	AGSMAUCTIONRELAY_CHAR_OP_LEAVE
	};




/********************************************/
/*		The Definition of Parameters		*/
/********************************************/
//
//	==========		Base		==========
//
class AgsdAuctionRelay : public AgsdDBParam
	{
	public:
		INT32			m_lCID;				// Character ID
		INT16			m_nCode;			// Result Code

	public:
		AgsdAuctionRelay()
			: m_lCID(0)
			{
			m_nCode = AGPMAUCTION_EXCPT_UNKNOWN;
			m_eOperation = AGSMDATABASE_OPERATION_EXECUTE;
			}
		virtual ~AgsdAuctionRelay()		{}
		virtual void Release() { delete this; }
	};


//
//	==========		Select		==========
//
class AgsdAuctionRelaySelect : public AgsdAuctionRelay, public ApMemory<AgsdAuctionRelaySelect, 30000>
	{
	public:
		//	In
		INT32		m_lItemTID;
		INT16		m_nStatus;
		UINT64		m_ullDocID;
		INT16		m_nFlag;

		//	Out
		PVOID		m_pvPacket;
		
		//	Dummy
		UINT32		ullDocID;
		
	public:
		AgsdAuctionRelaySelect();
		AgsdAuctionRelaySelect(const AgsdAuctionRelaySelect &r);
		AgsdAuctionRelaySelect(const AgsdAuctionRelaySelect *p);
		virtual ~AgsdAuctionRelaySelect()	{}
		virtual void Release()	{ delete this; }

	protected:
		BOOL	SetParamSelect(AuStatement* pStatement);
	};


//
//	==========		Select Sales		==========
//
class AgsdAuctionRelaySelectSales : public AgsdAuctionRelay, public ApMemory<AgsdAuctionRelaySelectSales, 30000>
	{
	public:
		//	In
		CHAR		m_szChar[_MAX_CHARNAME_LENGTH + 1];

		//	Out
		PVOID		m_pvPacket;

	public:
		AgsdAuctionRelaySelectSales();
		AgsdAuctionRelaySelectSales(const AgsdAuctionRelaySelectSales &r);
		AgsdAuctionRelaySelectSales(const AgsdAuctionRelaySelectSales *p);
		virtual ~AgsdAuctionRelaySelectSales()	{}
		virtual void Release()	{ delete this; }

	protected:
		BOOL	SetParamSelect(AuStatement* pStatement);
	};


//
//	==========		Select Item Count		==========
//
class AgsdAuctionRelaySelectSalesCount : public AgsdAuctionRelay
	{
	public:
		//	In

		//	Out
		INT16		m_nTotalPacket;
		PVOID		m_pvPacket;

	public:
		AgsdAuctionRelaySelectSalesCount();
		AgsdAuctionRelaySelectSalesCount(const AgsdAuctionRelaySelectSalesCount &r);
		AgsdAuctionRelaySelectSalesCount(const AgsdAuctionRelaySelectSalesCount *p);
		virtual ~AgsdAuctionRelaySelectSalesCount()	{}
		virtual void Release()	{ delete this; }

	protected:
		BOOL	SetParamSelect(AuStatement *pStatement);
	};


//
//	==========		Sell		==========
//
class AgsdAuctionRelaySell : public AgsdAuctionRelay, public ApMemory<AgsdAuctionRelaySell, 30000>
	{
	public:
		//	In
		UINT64		m_ullItemSeq;
		INT32		m_lPrice;
		INT16		m_nQuantity;
		INT16		m_nItemStatus;
		UINT16		m_nStatusTo;
		CHAR		m_szChar[_MAX_CHARNAME_LENGTH + 1];
		INT32		m_lItemID;
		
		//	Out
		UINT64		m_ullDocID;
		CHAR		m_szDate[_MAX_DATETIME_LENGTH + 1];

		//	Dummy
		CHAR		m_szItemSeq[_I64_STR_LENGTH + 1];
		CHAR		m_szDocID[_I64_STR_LENGTH + 1];		

	public:
		AgsdAuctionRelaySell();
		AgsdAuctionRelaySell(const AgsdAuctionRelaySell &r);
		AgsdAuctionRelaySell(const AgsdAuctionRelaySell *p);
		virtual ~AgsdAuctionRelaySell()	{}
		virtual void Release()	{ delete this; }

		virtual void Dump(CHAR *pszOp);

	protected:
		BOOL	SetParamExecute(AuStatement* pStatement);
	};


//
//	==========		Cancel		==========
//
class AgsdAuctionRelayCancel : public AgsdAuctionRelay, public ApMemory<AgsdAuctionRelayCancel, 10000>
	{
	public:
		INT32		m_lSalesID;

		//	In
		UINT64		m_ullDocID;
		CHAR		m_szChar[_MAX_CHARNAME_LENGTH + 1];
		INT16		m_nStatusFrom;
		INT16		m_nStatusTo;

		//	Dummy
		CHAR		m_szDocID[_I64_STR_LENGTH + 1];
			
	public:
		AgsdAuctionRelayCancel();
		AgsdAuctionRelayCancel(const AgsdAuctionRelayCancel &r);
		AgsdAuctionRelayCancel(const AgsdAuctionRelayCancel *p);
		virtual ~AgsdAuctionRelayCancel()	{}
		virtual void Release()	{ delete this; }

		virtual void Dump(CHAR *pszOp);

	protected:
		BOOL	SetParamExecute(AuStatement* pStatement);
	};


//
//	==========		Confirm		==========
//
class AgsdAuctionRelayConfirm : public AgsdAuctionRelay, public ApMemory<AgsdAuctionRelayConfirm, 10000>
	{
	public:
		INT32		m_lSalesID;
		
		//	In
		UINT64		m_ullDocID;
		CHAR		m_szChar[_MAX_CHARNAME_LENGTH + 1];
		INT16		m_nStatusFrom;
		INT16		m_nStatusTo;
		
		//	Dummy
		CHAR		m_szDocID[_I64_STR_LENGTH + 1];
		
			
	public:
		AgsdAuctionRelayConfirm();
		AgsdAuctionRelayConfirm(const AgsdAuctionRelayConfirm &r);
		AgsdAuctionRelayConfirm(const AgsdAuctionRelayConfirm *p);
		virtual ~AgsdAuctionRelayConfirm()	{}
		virtual void Release()	{ delete this; }

		virtual void Dump(CHAR *pszOp);

	protected:
		BOOL	SetParamExecute(AuStatement* pStatement);
	};



//
//	==========		Buy		==========
//
class AgsdAuctionRelayBuy : public AgsdAuctionRelay, public ApMemory<AgsdAuctionRelayBuy, 30000>
	{
	public:
		//	In
		UINT64		m_ullDocID;
		CHAR		m_szBuyer[_MAX_CHARNAME_LENGTH + 1];
		INT64		m_llMoney;
		INT16		m_nStatusFrom;
		INT16		m_nStatusTo;

		//	Out
		CHAR		m_szSeller[_MAX_CHARNAME_LENGTH + 1];				// seller id
		INT32		m_lPrice;											// price
		UINT64		m_ullItemSeq;										// item seq.
		INT32		m_lItemTID;											// item tid
		INT16		m_nQuantity;										// remained quantity
		INT8		m_cNeedLevel;										// needlevel
		CHAR		m_szOwnTime[_MAX_DATETIME_LENGTH + 1];				// owntime
		CHAR		m_szConvHistory[_MAX_CONVERT_HISTORY_LENGTH + 1];	// convhist
		INT32		m_lDurability;										// durability
		INT32		m_lMaxDurability;									// max durability
		CHAR		m_szOption[_MAX_OPTION_LENGTH + 1];					// option
		CHAR		m_szSkillPlus[_MAX_OPTION_LENGTH + 1];				// skill plus
		INT32		m_lInUse;											// in use
		INT32		m_lUseCount;										// use count
		INT64		m_lRemainTime;										// remain time
		INT32		m_lExpireDate;										// expire date
		INT64		m_llStaminaRemainTime;								// remain stamina time

		//	Dummy
		CHAR		m_szDocID[_I64_STR_LENGTH + 1];
		CHAR		m_szMoney[_I64_STR_LENGTH + 1];
		CHAR		m_szItemSeq[_I64_STR_LENGTH + 1];
		CHAR		m_szExpireDate[_MAX_DATETIME_LENGTH + 1];			// expire date

	public:
		AgsdAuctionRelayBuy();
		AgsdAuctionRelayBuy(const AgsdAuctionRelayBuy &r);
		AgsdAuctionRelayBuy(const AgsdAuctionRelayBuy *p);
		virtual ~AgsdAuctionRelayBuy()	{}
		virtual void Release()	{ delete this; }

		virtual void Dump(CHAR *pszOp);

	protected:
		BOOL	SetParamExecute(AuStatement* pStatement);
	};


//
//	==========		Character		==========
//
//		!!! DB에는 사용되지 않는다.
//
class AgsdAuctionRelayCharacter
	{
	public:
		//CHAR		m_szChar[_MAX_CHARNAME_LENGTH + 1];
		UINT32		m_lServerID;

	public:
		AgsdAuctionRelayCharacter();
		virtual ~AgsdAuctionRelayCharacter();
	};


#endif


