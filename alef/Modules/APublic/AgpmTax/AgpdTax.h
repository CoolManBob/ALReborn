/*============================================================

	AgpdTax.h
	
============================================================*/


#ifndef _AGPD_TAX_H_
	#define _AGPD_TAX_H_

#include "ApBase.h"
#include "ApModule.h"
#include "AgpdCharacter.h"
#include "AgpdSiegeWar.h"


/********************************************/
/*		The Definition of Tax struct		*/
/********************************************/
//
const INT32	AGPDTAX_REGION_MAX	= 4;


//
//	==========	Region Tax	==========
//
struct AgpdRegionTax
	{
	public :
		ApCriticalSection	m_Mutex;
		
		TCHAR		m_szRegionName[32+1];
		INT64		m_llIncome;
		INT32		m_lRatio;
		
	public :
		void	Init()
			{
			m_Mutex.Init();
			Reset();
			}
		
		void	Destroy()
			{
			m_Mutex.Destroy();
			}
		
		void	Reset()
			{
			ZeroMemory(m_szRegionName, sizeof(m_szRegionName));
			m_llIncome = 0;
			m_lRatio = 0;
			}
	};


//
//	==========	Tax (4 castle)	==========
//
struct AgpdTax
	{
	public :
		ApCriticalSection	m_Mutex;

		TCHAR		m_szCastle[AGPMSIEGEWAR_MAX_CASTLE_NAME + 1];
		ApSafeArray<AgpdRegionTax *, AGPDTAX_REGION_MAX>	m_RegionTaxes;
		INT32												m_lRegionTaxes;
		INT64		m_llTotalIncome;
		UINT32		m_ulLatestTransferDate;
		UINT32		m_ulLatestModifyDate;

	public:
		void	Init()
			{
			m_Mutex.Init();
			Reset();
			}
		
		void	Destroy()
			{
			m_Mutex.Destroy();
			}
		
		void	Reset()
			{
			ZeroMemory(m_szCastle, sizeof(m_szCastle));
			m_RegionTaxes.MemSetAll();
			m_lRegionTaxes = 0;
			m_llTotalIncome = 0;
			m_ulLatestTransferDate = 0;
			m_ulLatestModifyDate = 0;
			}
		
		BOOL	Add(AgpdRegionTax *pAgpdRegionTax)
			{
			if (!pAgpdRegionTax)
				return FALSE;
				
			if (0 >= _tcslen(pAgpdRegionTax->m_szRegionName))
				return FALSE;
			
			AuAutoLock Lock(m_Mutex);
			if (!Lock.Result()) return FALSE;

			for (INT32 i = 0; i < m_lRegionTaxes; i++)
				{
				if (m_RegionTaxes[i] == pAgpdRegionTax)
					return TRUE;
				
				if (!m_RegionTaxes[i] &&
					0 == _tcsnicmp(m_RegionTaxes[i]->m_szRegionName, pAgpdRegionTax->m_szRegionName, 32))
					return TRUE;
				}
			
			if (m_lRegionTaxes >= AGPDTAX_REGION_MAX)
				return FALSE;
			
			m_RegionTaxes[m_lRegionTaxes++] = pAgpdRegionTax;
			
			return TRUE;
			}

		// 2007.09.10. steeple
		INT64	GetRecentIncome()
		{
			INT64 llRecent = 0;
			for(INT32 i = 0; i < m_lRegionTaxes; ++i)
			{
				if(m_RegionTaxes[i])
					llRecent += m_RegionTaxes[i]->m_llIncome;
			}

			return llRecent;
		}
	};


//
//	==========	Callback Param	==========
//
struct AgpdTaxParam
	{
	AgpdTax			*m_pAgpdTax;
	BOOL			m_bCastle;
	INT32			m_lResult;
	
	AgpdTaxParam()
		{
		m_pAgpdTax = NULL;
		m_bCastle = TRUE;
		m_lResult = -1;
		}
	};


#endif
