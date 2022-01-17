/*============================================================

	AgpdWantedCriminal.h
	
============================================================*/


#ifndef _AGPD_WANTEDCRIMINAL_H_
	#define _AGPD_WANTEDCRIMINAL_H_

#include "ApBase.h"
#include "ApModule.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"


/****************************************************/
/*		The Definition of WantedCriminal struct		*/
/****************************************************/
//
//
struct AgpdWantedCriminal
	{
	public :
		ApCriticalSection	m_Mutex;
		TCHAR		m_szCharID[AGPDCHARACTER_NAME_LENGTH + 1];
		INT64		m_llMoney;		// bounty
		INT32		m_lLevel;
		INT32		m_lClass;
		BOOL		m_bOnline;

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
			ZeroMemory(m_szCharID, sizeof(m_szCharID));
			m_llMoney = 0;
			m_lLevel = 0;
			m_lClass = 0;
			m_bOnline = FALSE;
			}
			
		AgpdWantedCriminal& operator = (const AgpdWantedCriminal& cpy)
			{
			_tcscpy(m_szCharID, cpy.m_szCharID);
			m_llMoney = cpy.m_llMoney;
			m_lLevel = cpy.m_lLevel;
			m_lClass = cpy.m_lClass;
			m_bOnline = cpy.m_bOnline;
			
			return *this;
			}
	};


//
//	==========	Character attached data	==========
//
struct AgpdWantedCriminalCAD : public ApBase
	{
	public:
		TCHAR		m_szKiller[AGPDCHARACTER_NAME_LENGTH + 1];
	
	public:
		void	Set(TCHAR *pszKiller)
			{
			_tcsncpy(m_szKiller, pszKiller, AGPDCHARACTER_NAME_LENGTH);
			}
		
		void	Reset()
			{
			ZeroMemory(m_szKiller, sizeof(m_szKiller));
			}
	};




/********************************************/
/*		The Definition of Base Bounty		*/
/********************************************/
//
struct AgpdWantedCriminalBaseBounty
	{
	INT32		m_lMaxLevel;
	INT64		m_llMoney;
	};




/****************************************************/
/*		The Definition of Callback Parameter		*/
/****************************************************/
//
struct AgpdWantedCriminalParam
	{
	AgpdWantedCriminal	*m_pAgpdWantedCriminal;
	INT32				m_lResult;
	INT32				m_lRank;
	INT32				m_lPage;
	
	AgpdWantedCriminalParam()
		{
		m_pAgpdWantedCriminal = NULL;
		m_lResult = 0;
		m_lRank = 0;
		m_lPage = 0;
		}
	};


#endif
