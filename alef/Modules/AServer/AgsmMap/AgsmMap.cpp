/******************************************************************************
Module:  AgsmMap.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 06
******************************************************************************/

#include "ApBase.h"
#include "AgsmMap.h"
#include "MagDebug.h"

AgsmMap::AgsmMap()
{
	SetModuleName("AgsmMap");

	SetModuleType(APMODULE_TYPE_SERVER);
}

AgsmMap::~AgsmMap()
{
}

BOOL AgsmMap::OnAddModule()
{
	m_papmMap				= ( ApmMap				*	) GetModule( "ApmMap"				);
	m_pAgsmServerManager	= ( AgsmServerManager	*	) GetModule( "AgsmServerManager2"	);
	m_pagsmAOIFilter		= ( AgsmAOIFilter		*	) GetModule( "AgsmAOIFilter"		);

	ASSERT( NULL != m_papmMap				);
	ASSERT( NULL != m_pAgsmServerManager	);
	ASSERT( NULL != m_pagsmAOIFilter		);

	if (!m_papmMap || !m_pAgsmServerManager || !m_pagsmAOIFilter)
		return FALSE;

	m_nIndexADMap = m_papmMap->AttachSectorData(this, sizeof(AgsdControlServer), ConAgsdControlServer, NULL);

	if (!m_pagsmAOIFilter->SetCallbackCheckControlServer(CallbackCheckControlServer, this))
		return FALSE;

	if (!m_pAgsmServerManager->SetCallbackAddServer(CallbackAddServer, this))
		return FALSE;

	m_lSelfSID = AP_INVALID_SERVERID;

	return TRUE;
}

BOOL AgsmMap::ConAgsdControlServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmMap *pThis = (AgsmMap *) pClass;
	AgsdControlServer *pcsControlServer = pThis->GetADMap(pData);

	if (!pcsControlServer)
		return FALSE;

	pcsControlServer->m_lServerID = AP_INVALID_SERVERID;
	pcsControlServer->m_bIsZoning = FALSE;

	return TRUE;
}

INT32 AgsmMap::IsNeedZoning(AuPOS posDest)
{
	ApWorldSector *pSector = (ApWorldSector *) m_papmMap->GetSector(posDest);

	return IsNeedZoning(pSector);
}

INT32 AgsmMap::IsNeedZoning(ApWorldSector *pSector)
{
	if (!pSector)
		return FALSE;

	AgsdControlServer *pControlServer = GetADMap(pSector);
	if (!pControlServer)
		return FALSE;

	if (pControlServer->m_lServerID != GetThisServer())
		return TRUE;

	return pControlServer->m_bIsZoning;
}

INT32 AgsmMap::GetControlServer(ApWorldSector *pSector)
{
	if (!pSector)
		return AP_INVALID_SERVERID;

	AgsdControlServer *pControlServer = GetADMap(pSector);
	if (!pControlServer)
		return NULL;

	if (pControlServer->m_lServerID == AP_INVALID_SERVERID)
	{
		if (m_lSelfSID == AP_INVALID_SERVERID)
		{
			AgsdServer *pcsServer = m_pAgsmServerManager->GetThisServer();
			if (!pcsServer)
				return AP_INVALID_SERVERID;

			m_lSelfSID = pcsServer->m_lServerID;
		}

		AuPOS	posDest;
		posDest.x = pSector->GetXStart() + 1;
		posDest.z = pSector->GetZStart() + 1;

		AgsdServer	*pServer = m_pAgsmServerManager->GetControlServer(posDest);

		if (!pServer)
			return AP_INVALID_SERVERID;

		pControlServer->m_lServerID = pServer->m_lServerID;
	}

	return pControlServer->m_lServerID;
}

INT32 AgsmMap::GetThisServer()
{
	if (m_lSelfSID == AP_INVALID_SERVERID)
	{
		AgsdServer *pcsServer = m_pAgsmServerManager->GetThisServer();
		if (!pcsServer)
			return AP_INVALID_SERVERID;

		m_lSelfSID = pcsServer->m_lServerID;
	}

	return m_lSelfSID;
}

BOOL AgsmMap::CallbackCheckControlServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmMap			*pThis = (AgsmMap *) pClass;

	ApWorldSector	*pSector = (ApWorldSector *) pData;
	BOOL			*pbIsThis = (BOOL *) pCustData;

	INT32 lControlServerID = pThis->GetControlServer(pSector);

	*pbIsThis = FALSE;

	if (lControlServerID == pThis->GetThisServer())
		*pbIsThis = TRUE;

	return TRUE;
}

AgsdControlServer*	AgsmMap::GetADMap(PVOID pData)
{
	if (!pData)
		return NULL;

	return (AgsdControlServer *) m_papmMap->GetAttachedModuleData(m_nIndexADMap, pData);
}

BOOL AgsmMap::CallbackAddServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmMap		*pThis		= (AgsmMap *)		pClass;
	AgsdServer2	*pcsServer	= (AgsdServer2 *)	pData;

	// 이 서버가 관리하는 맵의 테두리섹터(조닝 완충지역)를 찾아 m_bIsZoning 값을 TRUE로 세팅한다.
	// 조닝처리시 아주 유용하게 쓰인다.

	for (int i = 0; i < pcsServer->m_nControlArea; i++)
	{
		for (float j = pcsServer->m_csControlArea[i].m_stControlArea.posLeftTop.x + 1; 
		 	       j < pcsServer->m_csControlArea[i].m_stControlArea.posRightBottom.x; 
				   j += MAP_SECTOR_WIDTH)
		{
			ApWorldSector *pSector = pThis->m_papmMap->GetSector(j, pcsServer->m_csControlArea[i].m_stControlArea.posLeftTop.z + 1);
			if (!pSector)
				continue;

			AgsdControlServer *pcsControlServer = pThis->GetADMap(pSector);

			pcsControlServer->m_bIsZoning = TRUE;
		}
		for (float j = pcsServer->m_csControlArea[i].m_stControlArea.posLeftTop.x + 1; 
			       j < pcsServer->m_csControlArea[i].m_stControlArea.posRightBottom.x; 
				   j += MAP_SECTOR_WIDTH)
		{
			ApWorldSector *pSector = pThis->m_papmMap->GetSector(j, pcsServer->m_csControlArea[i].m_stControlArea.posRightBottom.z - 1);
			if (!pSector)
				continue;

			AgsdControlServer *pcsControlServer = pThis->GetADMap(pSector);

			pcsControlServer->m_bIsZoning = TRUE;
		}
		for (float j = pcsServer->m_csControlArea[i].m_stControlArea.posLeftTop.z + 1; 
			       j < pcsServer->m_csControlArea[i].m_stControlArea.posRightBottom.z; 
			  	   j += MAP_SECTOR_HEIGHT)
		{
			ApWorldSector *pSector = pThis->m_papmMap->GetSector(pcsServer->m_csControlArea[i].m_stControlArea.posLeftTop.x + 1, j);
			if (!pSector)
				continue;

			AgsdControlServer *pcsControlServer = pThis->GetADMap(pSector);

			pcsControlServer->m_bIsZoning = TRUE;
		}
		for (float j = pcsServer->m_csControlArea[i].m_stControlArea.posLeftTop.z + 1; 
			       j < pcsServer->m_csControlArea[i].m_stControlArea.posRightBottom.z; 
				   j += MAP_SECTOR_HEIGHT)
		{
			ApWorldSector *pSector = pThis->m_papmMap->GetSector(pcsServer->m_csControlArea[i].m_stControlArea.posRightBottom.x - 1, j);
			if (!pSector)
				continue;

			AgsdControlServer *pcsControlServer = pThis->GetADMap(pSector);

			pcsControlServer->m_bIsZoning = TRUE;
		}
	}

	return TRUE;
}

/*
BOOL AgsmMap::CallbackAddServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmMap		*pThis		= (AgsmMap *)		pClass;
	AgsdServer	*pcsServer	= (AgsdServer *)	pData;

	// 이 서버가 관리하는 맵의 테두리섹터(조닝 완충지역)를 찾아 m_bIsZoning 값을 TRUE로 세팅한다.
	// 조닝처리시 아주 유용하게 쓰인다.

	AgsdServerTemplate *pcsTemplate = (AgsdServerTemplate *) pcsServer->m_pcsTemplate;

	for (int i = 0; i < pcsTemplate->m_nControlArea; i++)
	{
		for (float j = pcsTemplate->m_csControlArea[i].m_stControlArea.posLeftTop.x + 1; 
		 	       j < pcsTemplate->m_csControlArea[i].m_stControlArea.posRightBottom.x; 
				   j += MAP_SECTOR_WIDTH)
		{
			ApWorldSector *pSector = pThis->m_papmMap->GetSector(j, pcsTemplate->m_csControlArea[i].m_stControlArea.posLeftTop.z + 1);
			if (!pSector)
				continue;

			AgsdControlServer *pcsControlServer = pThis->GetADMap(pSector);

			pcsControlServer->m_bIsZoning = TRUE;
		}
		for (      j = pcsTemplate->m_csControlArea[i].m_stControlArea.posLeftTop.x + 1; 
			       j < pcsTemplate->m_csControlArea[i].m_stControlArea.posRightBottom.x; 
				   j += MAP_SECTOR_WIDTH)
		{
			ApWorldSector *pSector = pThis->m_papmMap->GetSector(j, pcsTemplate->m_csControlArea[i].m_stControlArea.posRightBottom.z - 1);
			if (!pSector)
				continue;

			AgsdControlServer *pcsControlServer = pThis->GetADMap(pSector);

			pcsControlServer->m_bIsZoning = TRUE;
		}
		for (      j = pcsTemplate->m_csControlArea[i].m_stControlArea.posLeftTop.z + 1; 
			       j < pcsTemplate->m_csControlArea[i].m_stControlArea.posRightBottom.z; 
			  	   j += MAP_SECTOR_HEIGHT)
		{
			ApWorldSector *pSector = pThis->m_papmMap->GetSector(pcsTemplate->m_csControlArea[i].m_stControlArea.posLeftTop.x + 1, j);
			if (!pSector)
				continue;

			AgsdControlServer *pcsControlServer = pThis->GetADMap(pSector);

			pcsControlServer->m_bIsZoning = TRUE;
		}
		for (      j = pcsTemplate->m_csControlArea[i].m_stControlArea.posLeftTop.z + 1; 
			       j < pcsTemplate->m_csControlArea[i].m_stControlArea.posRightBottom.z; 
				   j += MAP_SECTOR_HEIGHT)
		{
			ApWorldSector *pSector = pThis->m_papmMap->GetSector(pcsTemplate->m_csControlArea[i].m_stControlArea.posRightBottom.x - 1, j);
			if (!pSector)
				continue;

			AgsdControlServer *pcsControlServer = pThis->GetADMap(pSector);

			pcsControlServer->m_bIsZoning = TRUE;
		}
	}

	return TRUE;
}
*/