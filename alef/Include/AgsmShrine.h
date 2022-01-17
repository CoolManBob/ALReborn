/******************************************************************************
Module:  AgsmShrine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 24
******************************************************************************/

#if !defined(__AGSMSHRINE_H__)
#define __AGSMSHRINE_H__

#include "ApBase.h"
#include "AgsEngine.h"

#include "ApmObject.h"
#include "ApmMap.h"

#include "AgpmCharacter.h"
#include "AgpmSkill.h"
#include "AgpmShrine.h"

#include "AgsmAOIFilter.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"
#include "AgsmCharacter.h"
#include "AgsmSkill.h"
#include "AgsmSkillManager.h"

#include "AgsdShrine.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmShrineD" )
#else
#pragma comment ( lib , "AgsmShrine" )
#endif
#endif

class AgsmShrine : public AgsModule {
private:
	ApmObject*			m_papmObject;
	ApmMap*				m_papmMap;

	AgpmFactors*		m_pagpmFactors;
	AgpmCharacter*		m_pagpmCharacter;
	AgpmSkill*			m_pagpmSkill;
	AgpmShrine*			m_pagpmShrine;

	AgsmAOIFilter*		m_pagsmAOIFilter;
	AgsmServerManager*	m_pAgsmServerManager;
	AgsmCharacter*		m_pagsmCharacter;
	AgsmSkill*			m_pagsmSkill;
	AgsmSkillManager*	m_pagsmSkillManager;

	AgpdFactor			m_csUnionFactor[AGPMCHAR_MAX_UNION];

	INT32				m_lShrineGenerateID;

	INT16				m_nIndexADSector;

public:
	AgsmShrine();
	~AgsmShrine();

	BOOL			OnAddModule();

	BOOL			OnInit();
	BOOL			OnDestroy();
	BOOL			OnIdle2(UINT32 ulClockCount);

	BOOL			CreateShrineEvent(AgpdShrine *pcsShrine);

	static BOOL		CBGenerateShrineEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBActiveShrine(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBInactiveShrine(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBReCalcResultFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBAddShrineEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBConAgsdADSector(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBDesAgsdADSector(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBAddChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBMoveChar(PVOID pData, PVOID pClass, PVOID pCustData);

	AgsdShrineADSector*	GetADSector(PVOID pvSector);

	static BOOL		CBSpawnCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData);
};

#endif //__AGSMSHRINE_H__
