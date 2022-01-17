/*===================================================================

	AgsdBilling.h

===================================================================*/

#ifndef	_AGSD_BILLING_H_
	#define	_AGSD_BILLING_H_


#include "ApBase.h"
#include "AgpmCharacter.h"


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMBILLING_TYPE
	{
	AGSMBILLING_TYPE_NONE = 0,
	AGSMBILLING_TYPE_MONEY,
	AGSMBILLING_TYPE_ITEM,
	};


const enum eAGSMBILLING_RESULT
	{
	AGSMBILLING_RESULT_FAIL = 0,
	AGSMBILLING_RESULT_SUCCESS,
	AGSMBILLING_RESULT_SYSTEM_FAILURE,
	AGSMBILLING_RESULT_NOT_ENOUGH_MONEY,
	};




/************************************************/
/*		The Definition of AgsdBilling class		*/
/************************************************/
//
//	==========	Base	==========
//
class AgsdBilling
	{
	protected:
		eAGSMBILLING_TYPE		m_eType;
		
	public :
		INT32					m_lID;
		AgpdCharacter			*m_pAgpdCharacter;
		INT32					m_lResult;
	
	public :
		AgsdBilling()
			{
			m_eType = AGSMBILLING_TYPE_NONE;
			m_lID = 0;
			m_pAgpdCharacter = NULL;
			m_lResult = AGSMBILLING_RESULT_FAIL;
			}
		
		virtual ~AgsdBilling()
			{
			}
		
		void Release()
			{
			delete this;
			}
			
		eAGSMBILLING_TYPE	Type()
			{
			return m_eType;
			}
		
		virtual BOOL	Decode(char *pszPacket, INT32 lCode) = NULL;
	};


//
//	==========	Money	==========
//
class AgsdBillingMoney : public AgsdBilling, public ApMemory<AgsdBillingMoney, 10000>
	{
	public :
		INT64		m_llMoney;
		INT64		m_llInternalEventMoney;
		INT64		m_llExternalEventMoney;
		INT64		m_llCouponMoney;
	
	public :
		AgsdBillingMoney()
			{
			m_eType = AGSMBILLING_TYPE_MONEY;
			m_llMoney = 0;
			m_llInternalEventMoney = 0;
			m_llExternalEventMoney = 0;
			m_llCouponMoney = 0;
			}
		
		virtual BOOL	Decode(char *pszPacket, INT32 lCode);
	};
//JK_ºô¸µ
class AgsdBillingMoneyGlobal : public AgsdBilling, public ApMemory<AgsdBillingMoneyGlobal, 10000>
{
public :
	INT64		m_lWCoin;
	INT64		m_lPCoin;

public :
	AgsdBillingMoneyGlobal()
	{
		m_eType = AGSMBILLING_TYPE_MONEY;
		m_lWCoin = 0;
		m_lPCoin = 0;
	}

	virtual BOOL	Decode(char *pszPacket, INT32 lCode) 
	{
		return TRUE; 
	};
};

//
//	==========	Item	==========
//
class AgsdBillingItem : public AgsdBilling, public ApMemory<AgsdBillingItem, 10000>
	{
	public :
		INT32		m_lProductID;
		INT64		m_llBuyMoney;
		UINT64		m_ullListSeq;
		INT32		m_lType;//JK_ºô¸µ
		
		UINT64		m_ullOrderNo;
	
	public :
		AgsdBillingItem()
			{
			m_eType = AGSMBILLING_TYPE_ITEM;
			m_lProductID = 0;
			m_llBuyMoney = 0;
			m_ullListSeq = 0;
			m_ullOrderNo = 0;
			m_lType      = 0;//JK_ºô¸µ
			}
		
		void Release()
			{
			delete this;
			}
	
		virtual BOOL	Decode(char *pszPacket, INT32 lCode);
	};


#endif
