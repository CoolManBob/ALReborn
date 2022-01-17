#pragma once

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmUsedCharDataPoolD" )
#else
#pragma comment ( lib , "AgsmUsedCharDataPool" )
#endif
#endif

#include "AgpmCharacter.h"
#include "AgpmEventSpawn.h"
#include "AgpmProduct.h"
#include "AgsmFactors.h"
#include "AgsmCharacter.h"
#include "AgsmSkill.h"
#include "AgsmSummons.h"

#include "AgsdUsedPoolList.h"

//#define	_USE_CHARDATAPOOL_	1

class AgsmUsedCharDataPool : public AgsModule {
private:
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmEventSpawn	*m_pcsAgpmEventSpawn;
	AgpmProduct		*m_pcsAgpmProduct;
	AgsmCharacter	*m_pcsAgsmCharacter;
	AgsmCharManager	*m_pcsAgsmCharManager;
	AgsmFactors		*m_pcsAgsmFactors;
	AgsmSkill		*m_pcsAgsmSkill;
	AgsmSummons		*m_pcsAgsmSummons;

	INT32			m_lIndexAttachData;

	AgpdCharacter	**m_ppTIDDataList;
	AgpdCharacter	*m_pRemoveDataListHead;
	AgpdCharacter	*m_pRemoveDataListTail;

	ApMutualEx		m_Mutex;

public:
	AgsmUsedCharDataPool();
	virtual ~AgsmUsedCharDataPool();

	BOOL	OnAddModule();

public:
	AgsdUsedPoolList	*GetCharacterAttachedData(AgpdCharacter *pcsCharacter);

	BOOL	PushUsedData(AgpdCharacter *pcsCharacter);
	AgpdCharacter	*PopUsedTemplateData(INT32 lTID);
	AgpdCharacter	*PopUsedData();

	BOOL	RemoveWorld(AgpdCharacter *pcsCharacter);
	BOOL	AddWorld(AgpdCharacter *pcsCharacter);

	static BOOL	CBReadCharacterTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBRemoveCharacterFromMap(PVOID pData, PVOID pClass, PVOID pCustData);

private:
	BOOL	PushRemoveList(AgpdCharacter *pcsCharacter);
	AgpdCharacter	*PopRemoveList();
	BOOL	DeleteFromRemoveList(AgpdCharacter *pcsCharacter);

	BOOL	PushTIDList(AgpdCharacter *pcsCharacter);
	AgpdCharacter	*PopTIDList(INT32 lTID);
	BOOL	DeleteFromTIDList(AgpdCharacter *pcsCharacter);
};