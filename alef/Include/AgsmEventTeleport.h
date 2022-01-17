// AgsmEventTeleport.h: interface for the AgsmEventTeleport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMEVENTTELEPORT_H__A0E553AB_3B66_401B_9266_F8941142BFAC__INCLUDED_)
#define AFX_AGSMEVENTTELEPORT_H__A0E553AB_3B66_401B_9266_F8941142BFAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgsEngine.h"
#include "AgsmAOIFilter.h"
#include "AgpmSummons.h"
#include "AgpmEventTeleport.h"
#include "AgpmEventBinding.h"
#include "AgsmCharacter.h"
#include "AgsmItem.h"
#include "AgsmDeath.h"
#include "AgsmParty.h"
#include "AgpmLog.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventTeleportD" )
#else
#pragma comment ( lib , "AgsmEventTeleport" )
#endif
#endif

#define	AGSMTELEPORT_PI		3.141592

class AgsmEventTeleport : public AgsModule  
{
private:
	ApmMap *			m_pcsApmMap;
	AgpmCharacter *		m_pcsAgpmCharacter;
	AgpmSummons *		m_pcsAgpmSummons;
	AgsmAOIFilter *		m_pcsAgsmAOIFilter;
	AgpmEventTeleport *	m_pcsAgpmEventTeleport;
	AgpmEventBinding *	m_pcsAgpmEventBinding;
	AgsmCharacter *		m_pcsAgsmCharacter;
	AgsmItem *			m_pcsAgsmItem;
	AgsmDeath *			m_pcsAgsmDeath;
	AgsmParty *			m_pcsAgsmParty;
	AgpmLog *			m_pcsAgpmLog;

public:
	AgsmEventTeleport();
	virtual ~AgsmEventTeleport();

	// Virtual Function ต้
	BOOL			OnAddModule();

	//BOOL			Teleport(ApdEvent *pstEvent, INT32 lCID, INT32 lPosIndex, AuPOS *pstPos);
	//static BOOL		CBTeleport(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBCalcPos(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBReturnTown(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBReceiveTeleportLoading(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBSyncTeleportLoading(PVOID pData, PVOID pClass, PVOID pCustData);
	
	static BOOL		CBTeleportLog(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBBattleGroundTeleportLog(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL			SetStartTeleport(AgpdCharacter *pcsCharacter, AuPOS *pstTargetPos);
	BOOL			EndTeleport(AgpdCharacter *pcsCharacter);
	
	BOOL			WriteTeleportLog(AgpdCharacter *pcsCharacter, CHAR *pszSrc, CHAR *pszDest, INT32 lFee);
	BOOL			WriteBattleGroundTeleportLog(AgpdCharacter *pcsCharacter, CHAR *pszSrc, CHAR *pszDest, INT32 lFee);
};

#endif // !defined(AFX_AGSMEVENTTELEPORT_H__A0E553AB_3B66_401B_9266_F8941142BFAC__INCLUDED_)
