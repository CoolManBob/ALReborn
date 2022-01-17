/*****************************************************************************
* File : AgcmPreLODManager.h
*
* Desc :
*
* 111103
*****************************************************************************/

#ifndef __AGCMPRELODMANAGER_H__
#define __AGCMPRELODMANAGER_H__

#include "apmodule.h"
#include "AgcdPreLOD.h"

#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgcmLODManager.h"
#include "AgcmPreLODAdmin.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmPreLODManagerD" )
#else
#pragma comment ( lib , "AgcmPreLODManager" )
#endif
#endif

#define AGCM_PRELOD_STREAM_TYPE_PRELOD_DFF_NAME			"PRELOD_DFF_NAME"
#define AGCM_PRELOD_STREAM_TYPE_PRELOD_USEATOMICINDEX	"PRELOD_HAS_ATOMICINDEX"
#define AGCM_PRELOD_STREAM_TYPE_PRELOD_IS_BILLBOARD		"PRELOD_IS_BILLBOARD"
#define AGCM_PRELOD_STREAM_TYPE_PRELOD_LEVEL			"PRELOD_LEVEL"
#define AGCM_PRELOD_STREAM_TYPE_PRELOD_TRANSFORM_GEOM	"PRELOD_TRANSFORM_GEOMETRY"

#define AGCM_PRELOD_DUMMY_OBJECT_NAME					"DUMMY.DFF"

typedef enum
{
	AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_CHARACTER = 0,
	AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_ITEM,
	AGCM_PRELODMANAGER_STREAM_TYPE_PRELOD_OBJECT,
	AGCM_PRELODMANAGER_STREAM_TYPE_NUM
} eAgcmPreLODManagerStreamType;

class AgcmPreLODManager : public ApModule
{
public:
	AgcmPreLODManager();
	virtual ~AgcmPreLODManager();
	
	// stream
	// character
	BOOL			CharPreLODStreamRead(CHAR *szFile);
	BOOL			CharPreLODStreamWrite(CHAR *szFile);
	// item
	BOOL			ItemPreLODStreamRead(CHAR *szFile);
	BOOL			ItemPreLODStreamWrite(CHAR *szFile);
	// object
	BOOL			ObjectPreLODStreamRead(CHAR *szFile);
	BOOL			ObjectPreLODStreamWrite(CHAR *szFile);

	AgcdPreLODData	*GetPreLODData(AgcdPreLOD *pstLOD, INT32 lIndex, BOOL bAdd = TRUE);

	AgcdPreLOD		*GetPreLOD(ApBase *pBaseTemplate);
	AgcdPreLOD		*GetCharacterPreLOD(AgpdCharacterTemplate *pcsAgpdCharacterTemplate);
	AgcdPreLOD		*GetItemPreLOD(AgpdItemTemplate *pcsAgpdItemTemplate);
	AgcdPreLOD		*GetObjectPreLOD(ApdObjectTemplate *pcsApdObjectTemplate);

	AgpdCharacterTemplate	*GetCharacterTemplate(AgcdPreLOD *pcsAgcdPreLOD);
	AgpdItemTemplate		*GetItemTemplate(AgcdPreLOD *pcsAgcdPreLOD);
	ApdObjectTemplate		*GetObjectTemplate(AgcdPreLOD *pcsAgcdPreLOD);	

//	VOID			SetDummyClump(RpClump *pstClump) {m_pstDummyClump = pstClump;}

protected:
	// 필수 함수들....
	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();

//	BOOL	SetLOD(ApdObjectTemplate *pstApdObjectTemplate, CHAR *szDataPath);
	BOOL	SetLOD(RpClump *pstClump, AgcdPreLODData *pcsData, CHAR *szDataPath, INT32 lPartID = 0);

	// callback	
	static BOOL PreLODStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL PreLODStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream);
	static BOOL PreLODConstructCharCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL PreLODConstructItemCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL PreLODConstructObjtCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL PreLODDestructCharCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL PreLODDestructItemCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL PreLODDestructObjtCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL ObjectLoadClumpCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CharacterLoadClumpCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CharacterLoadDefaultArmourClumpCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL ItemLoadClumpCB(PVOID pData, PVOID pClass, PVOID pCustData);

public:
	AgcmPreLODAdmin	m_csPreLODAdmin;

protected:
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgcmCharacter	*m_pcsAgcmCharacter;
	AgpmItem		*m_pcsAgpmItem;
	AgcmItem		*m_pcsAgcmItem;
	ApmObject		*m_pcsApmObject;
	AgcmObject		*m_pcsAgcmObject;
	AgcmLODManager	*m_pcsAgcmLODManager;

	INT16			m_nCharPreLODAttachIndex;
	INT16			m_nItemPreLODAttachIndex;
	INT16			m_nObjectPreLODAttachIndex;

//	RpClump			*m_pstDummyClump;
};

#endif // __AGCMPRELODMANAGER_H__

