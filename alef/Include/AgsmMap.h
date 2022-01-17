/******************************************************************************
Module:  AgsmMap.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 23
******************************************************************************/

#if !defined(__AGSMMAP_H__)
#define __AGSMMAP_H__

#include "ApBase.h"
#include "ApmMap.h"

#include "AsDefine.h"
#include "AgsEngine.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"
#include "AgsmAOIFilter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmMapD" )
#else
#pragma comment ( lib , "AgsmMap" )
#endif
#endif

class AgsdControlServer {
public:
	INT32				m_lServerID;
	BOOL				m_bIsZoning;	// 이 섹터가 조닝을 해야하는 섹터인지의 여부.
};

class AgsmMap : public AgsModule {
private:
	ApmMap*				m_papmMap;
	AgsmServerManager*	m_pAgsmServerManager;
	AgsmAOIFilter*		m_pagsmAOIFilter;

	INT32				m_lSelfSID;		// this server id

	INT16				m_nIndexADMap;

public:
	AgsmMap();
	~AgsmMap();

	BOOL				OnAddModule();

	static BOOL			ConAgsdControlServer(PVOID pData, PVOID pClass, PVOID pCustData);

	AgsdControlServer*	GetADMap(PVOID pData);

	INT32				IsNeedZoning(AuPOS posDest);
	INT32				IsNeedZoning(ApWorldSector *pSector);

	INT32				GetControlServer(ApWorldSector *pSector);
	INT32				GetThisServer();

	static BOOL			CallbackCheckControlServer(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CallbackAddServer(PVOID pData, PVOID pClass, PVOID pCustData);
};

/*
#include "ApBase.h"
#include "../../APublic/ApmMap/ApmMap.h"

#include "AgsZoning.h"

#include "AgpmCharacter.h"



class AgsmMap : public AgsModule {
private:
	ApmMap*			m_pagpmMap;
	AgpmCharacter*	m_pagpmCharacter;
	AgsmCharacter*	m_pagsmCharacter;
	AgsmAOIFilter*	m_pagsmAOIFilter;
	AgsmServer*		m_pagsmServer;

private:
	AgsZoning	m_csZoning;

public:
	AgsmMap();
	~AgsmMap();

	BOOL OnValid(CHAR* pszData, INT16 nSize);

	BOOL OnAddModule();

	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	BOOL CalcDistance(pstAPADMINOBJECTCXT pstObjectCxt, 
					  AuPOS stOriginPos, AuPOS stDestPos, AuPOS* stResultPos);
	BOOL CalcMove(pstAPADMINOBJECTCXT pstObjectCxt, 
					  AuPOS stOriginPos, AuPOS stDestPos);

	static INT16 ProcessMap(UINT32 ulType, UINT32 ulAction, CHAR* pszData, INT16 nSize, PVOID pClass);
	//static INT16 ProcessZoning(UINT32 ulType, UINT32 ulAction, CHAR* pszData, INT16 nSize, PVOID pClass);

	INT16 SendMoveResult(AgpdCharacter *pCharacter, AuPOS stDestPos);
};
*/

#endif //__AGSMMAP_H__
