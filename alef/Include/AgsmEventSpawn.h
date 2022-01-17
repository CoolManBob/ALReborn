// AgsmEventSpawn.h: interface for the AgsmEventSpawn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMEVENTSPAWN_H__4F44CC1B_2A51_417D_8D8D_BDD46ED454D2__INCLUDED_)
#define AFX_AGSMEVENTSPAWN_H__4F44CC1B_2A51_417D_8D8D_BDD46ED454D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgsEngine.h"
#include "AgsmCharManager.h"
#include "ApmEventManager.h"
#include "AgpmEventSpawn.h"
#include "AgpmItem.h"
#include "AgpmAI2.h"
#include "AgpmDropItem2.h"
//#include "AgpmDropItem.h"
#include "AgsmItemManager.h"
//#include "AgsmDropItem.h"
#include "AgsmDeath.h"
#include "AgsmUsedCharDataPool.h"
#include "AgpmSiegeWar.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmEventSpawnD" )
#else
#pragma comment ( lib , "AgsmEventSpawn" )
#endif
#endif

typedef enum
{
	AGSMSPAWN_CB_SPAWN		= 0,
	AGSMSPAWN_CB_SPAWN_USED_DATA,
	AGSMSPAWN_CB_INIT_REUSED_CHARACTER,
} AgsmSpawnCallbackPoint;

typedef enum
{
	AGSMSPAWN_ADDITIONAL_TYPE_SIEGEWAR		= 0,
	AGSMSPAWN_ADDITIONAL_TYPE_SECRET_DUNGEON,
} AgsmSapwnAdditionalType;

struct AdditionalSpawnData
{
	AgsmSapwnAdditionalType m_eType;

	PVOID m_pClass;
	ApModuleDefaultCallBack m_pfCallback;

	INT32 m_lData1;
	INT32 m_lData2;

	AdditionalSpawnData() : m_eType(AGSMSPAWN_ADDITIONAL_TYPE_SIEGEWAR),
							m_pClass(0),
							m_pfCallback(0),
							m_lData1(0),
							m_lData2(0)
	{
	}
};

class AgsmBattleGround;

class AgsmEventSpawn : public AgsModule  
{
private:
	ApmEventManager *	m_pcsApmEventManager;
	AgpmCharacter *		m_pcsAgpmCharacter;
	AgpmEventSpawn *	m_pcsAgpmEventSpawn;
	AgpmItem *			m_pcsAgpmItem;
//	AgpmDropItem		*m_pcsAgpmDropItem;
	AgpmAI2				*m_pcsAgpmAI2;
	AgpmDropItem2		*m_pcsAgpmDropItem2;
	AgsmAOIFilter		*m_pcsAgsmAOIFilter;
	AgsmCharacter *		m_pcsAgsmCharacter;
	AgsmCharManager *	m_pcsAgsmCharManager;
	AgsmItemManager *	m_pcsAgsmItemManager;
//	AgsmDropItem		*m_pcsAgsmDropItem;
	AgsmDeath *			m_pcsAgsmDeath;
	AgsmUsedCharDataPool *	m_pcsAgsmUsedCharDataPool;
	AgsmFactors*		m_pcsAgsmFactors;
	AgpmConfig*			m_pcsAgpmConfig;
	AgpmSiegeWar*		m_pcsAgpmSiegeWar;
	AgsmBattleGround*	m_pcsAgsmBattleGround;

	MTRand				m_csRandomNumber;

	//INT32				m_alCID[AGPMSPAWN_MAX_CHARACTER_LIST];
	ApSafeArray<INT32, AGPMSPAWN_MAX_CHARACTER_LIST>	m_alCID;

	INT32				m_lLastSpawnGroupIndex;

	ApMutualEx			m_IdleMutex;
	std::vector<AdditionalSpawnData> m_vcAdditionalSpawn;

public:
	AgsmEventSpawn();
	virtual ~AgsmEventSpawn();

	// Virtual Function 들
	BOOL			OnAddModule();
	BOOL			OnInit();
	BOOL			OnDestroy();
	BOOL			OnIdle3(UINT32 ulClockCount);	// 2007.03.15. steeple. Spawn 은 Idle3 에서 돈다.

	// Spawn Functions
	BOOL			ProcessSpawnGroup(AgpdSpawnGroup *pstGroup, UINT32 ulClockCount);
	BOOL			ProcessSpawn(ApdEvent *pstEvent, UINT32 ulClockCount);
	AgpdCharacter*	SpawnCharacter(ApdEvent *pstEvent, INT32 lCTID, INT32 lAI2TID, /*AgpdSpawnItem *pstSpawnItem,*/ ApdEventAttachData *pstEvents = NULL);
	BOOL			SpawnCharacter(INT32 lTID, INT32 lAI2ID, AuPOS stPos, AgpdSpawn* pstSpawn, AgpdCharacter* pcsBoss = NULL, BOOL bAdmin = FALSE);

	BOOL			SetCallbackSpawn(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackSpawnUsedData(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackInitReusedCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	static BOOL		CBUpdateCharStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBSpawn(PVOID pData, PVOID pClass, PVOID pCustData);		// 2005.07.21. steeple

	BOOL			IsActiveSpawn(ApdEvent *pcsEvent);

	BOOL			RemoveSpawnCharacter(ApdEvent *pcsEvent);

	BOOL			SetGiftBoxStatus(AgpdCharacter *pcsCharacter);
	BOOL			ReadEventEffectTxt(CHAR* szFileName, BOOL bDecryption = FALSE);

	// 다른데서 Spawn 하고 싶을 때 이걸 사용한다.
	BOOL			InsertAdditionalSpawn(AgsmSapwnAdditionalType eType, PVOID pClass, ApModuleDefaultCallBack pfCallback,
											INT32 lData1, INT32 lData2);
};

#endif // !defined(AFX_AGSMEVENTSPAWN_H__4F44CC1B_2A51_417D_8D8D_BDD46ED454D2__INCLUDED_)
