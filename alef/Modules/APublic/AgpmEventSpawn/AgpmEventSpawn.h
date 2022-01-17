// AgpmEventSpawn.h: interface for the AgpmEventSpawn class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGPMEVENTSPAWN_H__8B96EF83_D271_47D0_988D_03F7E2513F13__INCLUDED_)
#define AFX_AGPMEVENTSPAWN_H__8B96EF83_D271_47D0_988D_03F7E2513F13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApModule.h"
#include "ApmEventManager.h"
#include "AgpdEventSpawn.h"
#include "AuRandomNumber.h"
#include "AuExcelTxtLib.h"

#include "AgpaSpawnGroup.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmEventSpawnD" )
#else
#pragma comment ( lib , "AgpmEventSpawn" )
#endif
#endif

#define AGPMSPAWN_MAX_CHARACTER_LIST	1024

#define AGPMSPAWN_INI_NAME_START			"SpawnStart"
#define AGPMSPAWN_INI_NAME_END				"SpawnEnd"

#define	AGPMSPAWN_INI_NAME_CTID				"CTID"
#define	AGPMSPAWN_INI_NAME_AI_TID			"AITID"
#define AGPMSPAWN_INI_NAME_NAME				"Name"
#define AGPMSPAWN_INI_NAME_SPAWN_RATE		"SpawnRate"
#define AGPMSPAWN_INI_NAME_MAX_CHAR			"MaxChar"
#define AGPMSPAWN_INI_NAME_GROUPID			"GroupID"
#define AGPMSPAWN_INI_NAME_MIN_CHAR			"MinChar"
#define AGPMSPAWN_INI_NAME_INTERVAL			"Interval"
#define	AGPMSPAWN_INI_NAME_SIEGEWAR_GROUP	"SiegeWarGroup"

/*#define AGPMSPAWN_INI_NAME_ITID				"ITID"
#define AGPMSPAWN_INI_NAME_ITEM_SPAWN_RATE	"ItemSpawnRate"
#define AGPMSPAWN_INI_NAME_ITEM_DROP_RATE	"ItemDropRate"
#define AGPMSPAWN_INI_NAME_ITEM_STATUS		"ItemInitStatus"*/

#define AGPMSPAWN_INI_DROPITEM_TEMPLATE		"DropItemTemplate"

#define AGPMSPAWN_INI_NAME_SUB_START		"SubStart"
#define AGPMSPAWN_INI_NAME_SUB_END			"SubEnd"

#define AGPMSPAWN_INI_NAME_EVENT_START		"EventStart"
#define AGPMSPAWN_INI_NAME_EVENT_END		"EventEnd"

//4 Excel
#define AGPMSPAWN_GROUP_NAME			"GroupName"
#define AGPMSPAWN_SPAWN_NAME			"Name"
#define AGPMSPAWN_MAX_MOB_COUNT			"MaxMobCount"
#define	AGPMSPAWN_SPAWN_TIME			"Spawntime"
#define AGPMSPAWN_MOB_TID				"MobTID"
#define AGPMSPAWN_MOB_LV				"MobLV"
#define AGPMSPAWN_MOB_NAME				"MobName"
#define AGPMSPAWN_RATE					"Rate"
#define AGPMSPAWN_AITID					"MobAITID"
#define	AGPMSPAWN_SIEGEWAR_TYPE			"SiegeWarType"

typedef enum
{
	AGPMSPAWN_DATA_SPAWN		= 0,
	AGPMSPAWN_DATA_SPAWN_GROUP,
} AgpmSpawnData;

typedef enum
{
	AGPMSPAWN_CB_SPAWN = 0,
	AGPMSPAWN_CB_IS_RESPAWN_MONSTER,
	AGPMSPAWN_CB_CUSTOM_SPAWN,
} AgpmSpawnCB;

class AgpmEventSpawn : public ApModule  
{
private:
	INT16				m_nAttachIndexChar;

	AgpaSpawnGroup		m_csGroups;
	ApmEventManager *	m_pcsApmEventManager;
	AgpmCharacter *		m_pcsAgpmCharacter;

	INT32				m_lMaxGroupID;

	AuExcelTxtLib		m_csAuExcelTxtLib;

public:
	AgpmEventSpawn();
	virtual ~AgpmEventSpawn();

	// Virtual Function 들
	BOOL			OnAddModule();
	BOOL			OnInit();
	BOOL			OnDestroy();

	// Callback 등록과 Data Attach API
	INT16			AttachSpawnData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);
	INT16			AttachSpawnGroupData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	// Group 관리
	VOID				SetMaxGroup(INT32 lMaxGroup);

	AgpdSpawnGroup *	CreateSpawnGroup();
	AgpdSpawnGroup *	AddSpawnGroup(CHAR *szName, BOOL bGenID = TRUE);
	AgpdSpawnGroup *	AddSpawnGroup(AgpdSpawnGroup *pstGroup, BOOL bGenID = TRUE);
	AgpdSpawnGroup *	GetGroup(INT32 lSGID);
	AgpdSpawnGroup *	GetGroup(char *pstrGroupName);
	BOOL				RemoveGroup(AgpdSpawnGroup *pstGroup);
	BOOL				RemoveGroup(INT32 lSGID);

	BOOL				ChangeGroupName(AgpdSpawnGroup *pstGroup, CHAR *szName);

	AgpdSpawnGroup *	GetGroupSequence(INT32 *plIndex);

	BOOL				AddSpawnToGroup(AgpdSpawnGroup *pstGroup, ApdEvent *pstEvent);
	BOOL				RemoveSpawnFromGroup(ApdEvent *pstEvent);

	AgpdSpawnADChar *	GetCharacterData(AgpdCharacter *pcsCharacter);

	BOOL				StreamWriteGroup(CHAR *szFile);
	BOOL				StreamReadGroup(CHAR *szFile);

	INT32					AddSpawnChar(AgpdSpawn *pstSpawn, INT32 lCTID, INT32 lAITID, INT32 lSpawnRate);
	BOOL					RemoveSpawnChar(AgpdSpawn *pstSpawn, INT32 lIndex);

//	INT32					AddSpawnItem(AgpdSpawnConfigChar *pstSpawnChar, INT32 lITID, INT32 lSpawnRate, INT32 lDropRate, AgpdSpawnItemStatus eStatus);
//	AgpdSpawnItem *			GetSpawnItem(AgpdSpawnConfigChar *pstSpawnChar, INT32 lIndex);
//	BOOL					RemoveSpawnItem(AgpdSpawnConfigChar *pstSpawnChar, INT32 lIndex);

	AgpdSpawn*				GetSpawnByMonsterTID(INT32 lTID, INT32* plSpawnIndex);
	AgpdSpawn* FindSpawnData(ApdEvent* pcsEvent, CHAR* strName);

	//static BOOL		CBUpdateIdle(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBStreamWrite(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBStreamRead(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBStreamWriteGroup(PVOID pData, ApModule *pClass, ApModuleStream *pstStream);
	static BOOL		CBStreamReadGroup(PVOID pData, ApModule *pClass, ApModuleStream *pstStream);

	static BOOL		CBUpdateCharStatus(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBEventConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBEventDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL LoadSpawnInfoFromExcel( char *pstrFileName);

	BOOL SetCallbackSpawn(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackIsRespawnMonster(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL RemoveMonsterFromSpawnList(AgpdCharacter *pcsCharacter);

	BOOL AddRespawnTime(AgpdCharacter *pcsCharacter);

	INT32	GetMaxGroupCount();
	INT32	GetGroupCount();
};

#endif // !defined(AFX_AGPMEVENTSPAWN_H__8B96EF83_D271_47D0_988D_03F7E2513F13__INCLUDED_)
