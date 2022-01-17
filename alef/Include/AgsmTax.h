/*====================================================================

	AgsmTax.h
	
====================================================================*/


#ifndef _AGSM_TAX_H_
	#define _AGSM_TAX_H_


#include "AgpmTax.h"
#include "AgpmSiegeWar.h"
#include "AgpmGuildWarehouse.h"
#include "AgpmArchlord.h"
#include "AgsmCharacter.h"
#include "AgsmPvP.h"
#include "AgsmDeath.h"
#include "AgsmSiegeWar.h"
#include <map>


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMTAX_CB
	{
	AGSMTAX_CB_DBUPDATE = 0,
	};


const UINT32	AGSMTAX_INTERVAL_5MINUTE =		(60 * 1000 * 5);		// 5 minutes




/********************************************/
/*		The Definition of AgsmTax class		*/
/********************************************/
//
class AgsmTax : public AgsModule
	{
	private:
		//	Related modules
		AgpmCharacter		*m_pAgpmCharacter;
		AgpmSiegeWar		*m_pAgpmSiegeWar;
		AgpmGuildWarehouse	*m_pAgpmGuildWarehouse;
		AgpmTax				*m_pAgpmTax;
		AgpmArchlord		*m_pAgpmArchlord;

		AgsmCharacter		*m_pAgsmCharacter;
		AgsmSiegeWar		*m_pAgsmSiegeWar;

		AgpmLog				*m_pAgpmLog;
		
		//	Idle
		UINT32				m_ulPrevClockCount;
		BOOL				m_bLoad;

	public:
		AgsmTax();
		virtual ~AgsmTax();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnIdle(UINT32 ulClockCount);

		//	Operations
		BOOL	OnRegionChange(AgpdCharacter *pAgpdCharacter);
		BOOL	OnModifyRatio(AgpdCharacter *pAgpdCharacter, AgpdTaxParam *pAgpdTaxParam);
		BOOL	OnInquireTax(AgpdCharacter *pAgpdCharacter, AgpdTaxParam *pAgpdTaxParam);
		BOOL	OnAddIncome(TCHAR *pszRegion, INT64 llIncome);
		BOOL	NotifyRatioAllCharactersInRegion(AgpdRegionTax *pAgpdRegionTax);
		BOOL	TransferAll();
		BOOL	Transfer(AgpdTax *pAgpdTax);
		BOOL	OnCastleOwnerChange(AgpdSiegeWar *pAgpdSiegeWar, AgpdGuild *pAgpdGuildPrev);
		BOOL	UpdateDBAll();
		BOOL	UpdateDB(AgpdTax *pAgpdTax);
		
		//	Callbacks
		static BOOL	CBLoadCastle(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBPayTax(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBModifyRatio(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBInquireTax(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCastleOwnerChange(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Packet Send
		BOOL	SendNotifyRegionTax(AgpdCharacter *pAgpdCharacter, AgpdRegionTax *pAgpdRegionTax);
		BOOL	SendResultModifyTaxRatio(AgpdCharacter *pAgpdCharacter, AgpdTax *pAgpdTax, INT32 lResult);
		BOOL	SendResultInquireTax(AgpdCharacter *pAgpdCharacter, AgpdTax *pAgpdTax, INT32 lResult);
		
		//	Callback Setting
		BOOL	SetCallbackDBUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	protected:
		//	Helper
		UINT32	_GetCharacterNID(INT32 lCID);
		UINT32	_GetCharacterNID(AgpdCharacter *pAgpdCharacter);
		
		//	Log
		BOOL	WriteBackupLog(AgpdTax *pAgpdTax);
		BOOL	WriteTransferLog(TCHAR *pszCastle, INT64 llSumOfWeek, INT64 llTotal);
		BOOL	WriteUpdateRatioLog(AgpdCharacter *pAgpdCharacter, TCHAR *pszCastle, TCHAR *pszDesc);
	};


#endif