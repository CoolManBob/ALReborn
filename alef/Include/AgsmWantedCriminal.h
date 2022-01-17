/*====================================================================

	AgsmWantedCriminal.h
	
====================================================================*/


#ifndef _AGSM_WANTEDCRIMINAL_H_
	#define _AGSM_WANTEDCRIMINAL_H_


#include "AgsmCharacter.h"
#include "AgsmPvP.h"
#include "AgsmDeath.h"
#include "AgsmItem.h"
#include "AgpmWantedCriminal.h"
#include "AgsdBuddy.h"					// for rowset... but informal
#include <map>


/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMWANTEDCRIMINAL_CB
	{
	AGSMWANTEDCRIMINAL_CB_SELECT = 0,
	AGSMWANTEDCRIMINAL_CB_INSERT,
	AGSMWANTEDCRIMINAL_CB_UPDATE,
	AGSMWANTEDCRIMINAL_CB_DELETE,
	AGSMWANTEDCRIMINAL_CB_OFFLINE_CHAR_INFO,
	};




/********************************************/
/*		The Definition of Ranking Map		*/
/********************************************/
//
typedef std::multimap<INT64, AgpdWantedCriminal *, std::greater<INT64> > CriminalRank;
typedef pair<INT64, AgpdWantedCriminal *> CriminalRankPair;
typedef CriminalRank::iterator CriminalRankIter;


class ApmMap;

/********************************************************/
/*		The Definition of AgsmWantedCriminal class		*/
/********************************************************/
//
class AgsmWantedCriminal : public AgsModule
	{
	private:
		//	Related modules
		AgsmServerManager	*m_pAgsmServerManager;
		AgsmInterServerLink	*m_pAgsmInterServerLink;
		AgpmCharacter		*m_pAgpmCharacter;
		AgsmCharacter		*m_pAgsmCharacter;
		AgpmPvP				*m_pAgpmPvP;
		AgsmPvP				*m_pAgsmPvP;
		AgsmDeath			*m_pAgsmDeath;
		AgsmItem			*m_pAgsmItem;
		AgpmWantedCriminal	*m_pAgpmWantedCriminal;
		AgpmLog				*m_pAgpmLog;
		ApmMap				*m_pApmMap;

		//	Ranking List
		ApCriticalSection	m_RankingLock;
		CriminalRank		m_Ranking;
		
		//	
		BOOL				m_bReady;

	public:
		AgsmWantedCriminal();
		virtual ~AgsmWantedCriminal();

		//	ApModule inherited
		BOOL	OnAddModule();
		BOOL	OnInit();

		//	Operations
		BOOL	OnConnectRelayServer();
		BOOL	OnSelectResultWantedCriminalList(stBuddyRowset *pRowset, BOOL bEnd = FALSE);
		
		BOOL	OnArrange(AgpdCharacter *pAgpdCharacter, AgpdWantedCriminalParam *pParam);
		BOOL	OnPvPDead(AgpdCharacter *pAgpdCharacterKill, AgpdCharacter *pAgpdCharacterDead);
		BOOL	OnRequestList(AgpdCharacter *pAgpdCharacter, INT32 lPage);
		BOOL	OnRequestMyRank(AgpdCharacter *pAgpdCharacter);
		BOOL	OnSelectOfflineCharacter(TCHAR *pszChar, INT32 lTID, INT32 lLevel);

		BOOL	DeleteWCWithNoEffect(const CHAR* szName);
		INT32	GetRank(const CHAR* szName);

		//	Callbacks
		static BOOL	CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBArrange(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBPvPDead(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCheckCriminal(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBRequestList(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBRequestMyRank(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBUpdateLevel(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBLeaveGame(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBResurrection(PVOID pData, PVOID pClass, PVOID pCustData);
		
		//	Packet Send
		BOOL	SendArrangeResult(AgpdCharacter *pAgpdCharacter, AgpdWantedCriminal *pAgpdWantedCriminal, INT32 lResult);
		BOOL	SendWantedCriminal(AgpdCharacter *pAgpdCharacter, AgpdWantedCriminal *pAgpdWantedCriminal, INT32 lRank, INT32 lPage);
		BOOL	SendMyRank(AgpdCharacter *pAgpdCharacter, INT32 lRank, INT64 llMoney);
		BOOL	SendUpdateKiller(AgpdCharacter *pAgpdCharacter, AgpdCharacter *pAgpdCharacterKiller);
		BOOL	SendUpdateKiller(AgpdCharacter *pAgpdCharacter, TCHAR *pszKiller);
		BOOL	SendUpdateFlag(AgpdCharacter *pAgpdCharacter);
		BOOL	SendNotify(AgpdCharacter *pAgpdCharacter, TCHAR *pszCharID, INT64 llMoney, INT32 lResult);
		
		//	Callback Setting
		BOOL	SetCallbackSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackOffCharInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		
	protected:
		//	Rank
		BOOL	_InsertRank(AgpdWantedCriminal *pAgpdWantedCriminal);
		BOOL	_RemoveRank(AgpdWantedCriminal *pAgpdWantedCriminal);
		INT32	_GetRank(AgpdWantedCriminal *pAgpdWantedCriminal);
		INT32	_GetRank(AgpdCharacter *pAgpdCharacter);
		
		//	Helper
		UINT32	_GetCharacterNID(INT32 lCID);
		UINT32	_GetCharacterNID(AgpdCharacter *pAgpdCharacter);
		
		//	Log
		BOOL	WriteLog(eAGPDLOGTYPE_ETC eType, AgpdCharacter *pAgpdCharacter, TCHAR *pszCharID, INT64 llValue);
	};


#endif