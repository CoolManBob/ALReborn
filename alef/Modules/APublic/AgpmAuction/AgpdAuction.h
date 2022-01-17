/*============================================================

	AgpdAuction.h
	
============================================================*/


#ifndef _AGPD_AUCTION_H_
	#define _AGPD_AUCTION_H_


#include "ApBase.h"
#include "ApModule.h"
#include "AgpdItem.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum AGPMAUCTION_SALES_STATUS
	{
	AGPMAUCTION_SALES_STATUS_ONSALE			= 0,
	AGPMAUCTION_SALES_STATUS_COMPLETE,
	AGPMAUCTION_SALES_STATUS_CANCEL,
	AGPMAUCTION_SALES_STATUS_CONFIRM_COMPLETE,
	};


const INT32 AGPMAUCTION_MAX_REGISTER =	(AGPMITEM_SALES_LAYER * AGPMITEM_SALES_ROW * AGPMITEM_SALES_COLUMN);



	
/************************************************/
/*		The Definition of AgpdAuction class		*/
/************************************************/
//
//	==========	Auction sales element	==========
//
class AgpdAuctionSales : public ApBaseExLock
	{
	public:
		UINT64		m_ullDocID;
		UINT64		m_ullItemSeq;
		INT32		m_lItemID;
		INT32		m_lPrice;
		INT16		m_nQuantity;
		INT16		m_nStatus;
		CHAR		m_szDate[33];
		INT32		m_lItemTID;		// 거래 완료된 아이템(다른사람에게 넘어간)의 TID.
	};


//
//	==========	Character attached data	==========
//	
class AgpdAuctionCAD : public ApBase
	{
	public:
		UINT32		m_ulClock;
		INT16		m_nCount;
		ApSafeArray<INT32, AGPMAUCTION_MAX_REGISTER>	m_Sales;
		
		BOOL		m_bCheckResult;
	
	public:
		AgpdAuctionCAD()
			{
			m_ulClock = 0;
			m_nCount = 0;
			m_Sales.MemSetAll();
			
			m_bCheckResult = FALSE;
			}
			
		~AgpdAuctionCAD()
			{
			m_nCount = 0;
			m_Sales.MemSetAll();
			}
		
		INT16	GetCount()
			{
			return m_nCount;
			}
		
		BOOL	Add(INT32 lID)
			{
			BOOL bResult = FALSE;
			for (INT32 l = 0; l < AGPMAUCTION_MAX_REGISTER; l++)
				{
				if (0 == m_Sales[l])
					{
					m_Sales[l] = lID;
					m_nCount++;
					bResult = TRUE;
					break;
					}
				}

			return bResult;
			}
			
		BOOL	Remove(INT32 lID)
			{
			BOOL bResult = FALSE;
			for (INT32 l=0; l < AGPMAUCTION_MAX_REGISTER; l++)
				{
				if (lID == m_Sales[l])
					{
					for (INT32 m = l; m < AGPMAUCTION_MAX_REGISTER - 1; m++)
						{
						m_Sales[m] = m_Sales[m + 1];
						}

					m_Sales[AGPMAUCTION_MAX_REGISTER - 1] = 0;

//					m_Sales[l] = 0;
					m_nCount--;
					bResult = TRUE;
					break;
					}
				}

			return bResult;
			}
		
		INT32	GetIndex(INT32 lID)
			{
			INT32 lIndex = -1;
			for (INT32 l=0; l < AGPMAUCTION_MAX_REGISTER; l++)
				{
				if (0 != m_Sales[l] && lID == m_Sales[l])
					{
					return lIndex = l;
					break;
					}
				}
			return lIndex;
			}
	};


//
//	==========	Event data	==========
//
class AgpdEventAuction
	{
	public:
		BOOL				m_bHaveAuctionEvent;
	};




/****************************************************/
/*		The Definition of Callback Arg. class		*/
/****************************************************/
//
class AgpmAuctionArg
	{
	public:
		INT8		m_cOperation;
		INT32		m_lCID;
		INT32		m_lResult;
		
		INT32		m_lItemID;
		INT32		m_lItemTID;
		INT16		m_nQuantity;
		INT32		m_lMoney;
		INT32		m_lSalesID;
		CHAR		m_szDate[33];
		UINT64		m_ullDocID;
		INT16		m_nFlag;
		INT32		m_lTotalSales;
		PVOID		m_pvRowsetPacket;

	public:
		AgpmAuctionArg()
			{
			Init();
			}
			
		void Init()
			{
			m_cOperation;
			m_lCID;
			m_lResult;
		
			m_lItemID = 0;
			m_lItemTID = 0;
			m_nQuantity = 0;
			m_lMoney = 0;
			m_lSalesID = 0;
			ZeroMemory(m_szDate, sizeof(m_szDate));
			m_ullDocID = 0;
			m_lTotalSales = 0;
			m_nFlag = 0;
			m_pvRowsetPacket = NULL;
			}
	};

	
#endif