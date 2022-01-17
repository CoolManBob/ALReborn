/******************************************************************************
Module:  AgpmShrine.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 23
******************************************************************************/

#include "AgpmShrine.h"
#include "ApModuleStream.h"

AgpmShrine::AgpmShrine()
{
	SetModuleName("AgpmShrine");

	SetModuleType(APMODULE_TYPE_PUBLIC);

	// setting module data
	SetModuleData(sizeof(AgpdShrine), AGPMSHRINE_DATA_TYPE_SHRINE);
	SetModuleData(sizeof(AgpdShrineTemplate), AGPMSHRINE_DATA_TYPE_TEMPLATE);

	SetPacketType(AGPMSHRINE_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,		1,		// operation
							AUTYPE_INT32,		1,		// shrine id
							AUTYPE_INT32,		1,		// shrine tid
							AUTYPE_INT8,		1,		// m_bIsActive
							AUTYPE_POS,			1,		// m_posShrine
							AUTYPE_PACKET,		1,		// event base packet
							AUTYPE_INT32,		1,		// generator character id
							AUTYPE_END,			0
							);
	m_nIndexADChar		= 0;
	m_nIndexADObject	= 0;
	m_nIndexADSpawn		= 0;

	m_papmObject			= NULL;
	m_pagpmFactors			= NULL;
	m_pagpmCharacter		= NULL;
	m_pagpmItem				= NULL;
	m_pagpmEventNature		= NULL;
	m_papmEventManager		= NULL;
	m_pagpmEventSpawn		= NULL;
}

AgpmShrine::~AgpmShrine()
{
}

BOOL AgpmShrine::OnAddModule()
{
	m_papmObject		= (ApmObject *)			GetModule("ApmObject");
	m_pagpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pagpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_papmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pagpmEventNature	= (AgpmEventNature *)	GetModule("AgpmEventNature");
	m_pagpmEventSpawn	= (AgpmEventSpawn *)	GetModule("AgpmEventSpawn");

	if (!m_papmObject || !m_pagpmFactors || !m_pagpmCharacter || !m_pagpmItem || !m_papmEventManager || !m_pagpmEventNature || !m_pagpmEventSpawn)
		return FALSE;

	m_nIndexADChar = m_pagpmCharacter->AttachCharacterData(this, sizeof(AgpdShrineADChar), ConAgpdShrineADChar, DesAgpdShrineADChar);
	if (m_nIndexADChar < 0)
		return FALSE;

	m_nIndexADSpawn = m_pagpmEventSpawn->AttachSpawnData(this, sizeof(AgpdShrineADSpawn), ConAgpdShrineADSpawn, DesAgpdShrineADSpawn);
	if (m_nIndexADSpawn < 0)
		return FALSE;

	m_nIndexADObject = m_papmObject->AttachObjectData(this, sizeof(AgpdShrineADObject), ConAgpdShrineADObject, DesAgpdShrineADObject);
	if (m_nIndexADObject < 0)
		return FALSE;

	if (!m_pagpmItem->SetCallbackChangeItemOwner(CBChangeItemOwner, this))
		return FALSE;

	if (!AddStreamCallback(AGPMSHRINE_DATA_TYPE_TEMPLATE, TemplateReadCB, TemplateWriteCB, this))
		return FALSE;

	if (!m_papmObject->AddStreamCallback(APMOBJECT_DATA_OBJECT, ObjectReadCB, ObjectWriteCB, this))
		return FALSE;

	if (!m_papmEventManager->RegisterEvent(APDEVENT_FUNCTION_SHRINE, NULL, NULL, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmShrine::OnInit()
{
	BOOL	bRetVal;

	// initialize data size (item, item template data)
	bRetVal = m_csShrineAdmin.InitializeObject(sizeof(AgpdShrine *), m_csShrineAdmin.GetCount());
	bRetVal &= m_csTemplateAdmin.InitializeObject(sizeof(AgpdShrineTemplate *), m_csTemplateAdmin.GetCount());

	if (bRetVal == FALSE)
	{
		// initialize failed... error return
		OutputDebugString("AgpmShrine::OnInit() Error (1) !!!\n");
		return FALSE;
	}

	return bRetVal;
}

BOOL AgpmShrine::OnDestroy()
{
	return TRUE;
}

BOOL AgpmShrine::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgpmShrine::CheckDay()
{
	// 현재 게임내 시간을 얻어온다.
	// 날씨도 본다.

	// 게임내 시간이 07:00 ~ 19:00
	// 날씨는 맑음

	UINT32 ulHour = m_pagpmEventNature->GetHour(m_pagpmEventNature->GetGameTime());

	if (ulHour < 07 && ulHour > 19)
		return FALSE;

	return TRUE;
}

BOOL AgpmShrine::ActiveShrine(AgpdShrine *pcsShrine, UINT32 ulClockCount)
{
	if (!pcsShrine)
		return FALSE;

	switch (pcsShrine->m_pcsTemplate->m_eReActiveCondition) {
	case AGPMSHRINE_ACTIVE_DAY_ONLY:
		{
			pcsShrine->m_bIsActive = TRUE;

			if (pcsShrine->m_pcsTemplate->m_bIsRandomPosition)
				pcsShrine->m_posShrine = pcsShrine->m_pcsTemplate->m_posShrine[m_csRandom.randInt(pcsShrine->m_pcsTemplate->m_nNumShrinePos - 1)];
			else
				pcsShrine->m_posShrine = pcsShrine->m_pcsTemplate->m_posShrine[0];

			pcsShrine->m_ulLastActiveTimeMSec	= ulClockCount;

			EnumCallback(AGPMSHRINE_CB_ACTIVE_SHRINE, pcsShrine, NULL);
		}
		break;

	case AGPMSHRINE_ACTIVE_SPAWN:
		{
			pcsShrine->m_bIsActive = TRUE;

			if (pcsShrine->m_pcsTemplate->m_bIsRandomPosition)
				pcsShrine->m_posShrine = pcsShrine->m_pcsTemplate->m_posShrine[m_csRandom.randInt(pcsShrine->m_pcsTemplate->m_nNumShrinePos - 1)];
			else
				pcsShrine->m_posShrine = pcsShrine->m_pcsTemplate->m_posShrine[0];

			pcsShrine->m_ulLastActiveTimeMSec	= ulClockCount;

			INT32	lActiveTime = 
				pcsShrine->m_pcsTemplate->m_ulMinActiveTimeHR + 
				m_csRandom.randInt(pcsShrine->m_pcsTemplate->m_ulMaxActiveTimeHR - pcsShrine->m_pcsTemplate->m_ulMinActiveTimeHR);

			pcsShrine->m_ulLastActiveDurationMSec = 
				lActiveTime * 60 * 60 * 1000;

			EnumCallback(AGPMSHRINE_CB_ACTIVE_SHRINE, pcsShrine, NULL);
		}
		break;
	}

	return TRUE;
}

BOOL AgpmShrine::InactiveShrine(AgpdShrine *pcsShrine, UINT32 ulClockCount)
{
	if (!pcsShrine)
		return FALSE;

	if (pcsShrine->m_pcsTemplate->m_eActiveCondition == AGPMSHRINE_ACTIVE_ALWAYS)
		return FALSE;

	pcsShrine->m_bIsActive = FALSE;

	pcsShrine->m_ulLastInactiveTimeMSec = ulClockCount;

	EnumCallback(AGPMSHRINE_CB_ACTIVE_SHRINE, pcsShrine, NULL);

	return TRUE;
}

AgpdShrine* AgpmShrine::AddShrine(INT32 lShrineID, INT32 lTID)
{
	if (lShrineID == AP_INVALID_SHRINEID || lTID == AP_INVALID_SHRINEID)
		return NULL;

	AgpdShrine *pcsShrine = CreateShrine();
	if (!pcsShrine)
		return NULL;

	pcsShrine->m_lID = lShrineID;
	pcsShrine->m_pcsTemplate = GetShrineTemplate(lTID);

	if (!pcsShrine->m_pcsTemplate)
	{
		DestroyShrine(pcsShrine);
		return NULL;
	}

	return m_csShrineAdmin.AddShrine(pcsShrine);
}

AgpdShrine* AgpmShrine::AddShrine(AgpdShrine *pcsShrine)
{
	return m_csShrineAdmin.AddShrine(pcsShrine);
}

BOOL AgpmShrine::RemoveShrine(INT32 lShrineID)
{
	return m_csShrineAdmin.RemoveShrine(lShrineID);
}

AgpdShrine* AgpmShrine::GetShrine(INT32 lShrineID)
{
	return m_csShrineAdmin.GetShrine(lShrineID);
}

AgpdShrine* AgpmShrine::GetShrineSequence(INT32 *plIndex)
{
	return (AgpdShrine *) m_csShrineAdmin.GetObjectSequence(plIndex);
}

AgpdShrineTemplate* AgpmShrine::AddShrineTemplate(INT32 lTID)
{
	if (lTID == AP_INVALID_SHRINEID)
		return NULL;

	AgpdShrineTemplate *pcsTemplate = CreateTemplate();
	if (!pcsTemplate)
		return NULL;

	pcsTemplate->m_lID = lTID;

	return AddShrineTemplate(pcsTemplate);
}

AgpdShrineTemplate* AgpmShrine::AddShrineTemplate(AgpdShrineTemplate *pcsShrine)
{
	return m_csTemplateAdmin.AddTemplate(pcsShrine);
}

BOOL AgpmShrine::RemoveShrineTemplate(INT32 lTID)
{
	return m_csTemplateAdmin.RemoveTemplate(lTID);
}

AgpdShrineTemplate* AgpmShrine::GetShrineTemplate(INT32 lTID)
{
	return m_csTemplateAdmin.GetTemplate(lTID);
}

AgpdShrineTemplate* AgpmShrine::GetShrineTemplate(CHAR *szName)
{
	return m_csTemplateAdmin.GetTemplate(szName);
}

AgpdShrineTemplate* AgpmShrine::GetShrineSequenceTemplate(INT32 *plIndex)
{
	AgpdShrineTemplate **ppcsShrineTemplate = (AgpdShrineTemplate **) m_csTemplateAdmin.GetObjectSequence(plIndex);
	if (!ppcsShrineTemplate)
		return NULL;

	return *ppcsShrineTemplate;
}

BOOL AgpmShrine::SetMaxShrine(INT32 lCount)
{
	return m_csShrineAdmin.SetCount(lCount);
}

BOOL AgpmShrine::SetMaxShrineTemplate(INT32 lCount)
{
	return m_csTemplateAdmin.SetCount(lCount);
}

AgpdShrineADChar* AgpmShrine::GetADCharacter(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter || !m_pagpmCharacter)
		return FALSE;

	return (AgpdShrineADChar *) m_pagpmCharacter->GetAttachedModuleData(m_nIndexADChar, (PVOID) pcsCharacter);
}

AgpdShrineADObject* AgpmShrine::GetADObject(ApdObject *pcsObject)
{
	if (!pcsObject || !m_papmObject)
		return FALSE;

	return (AgpdShrineADObject *) m_papmObject->GetAttachedModuleData(m_nIndexADObject, (PVOID) pcsObject);
}

AgpdShrineADSpawn* AgpmShrine::GetADSpawn(PVOID pvData)
{
	if (!pvData || !m_pagpmEventSpawn)
		return FALSE;

	return (AgpdShrineADSpawn *) m_pagpmEventSpawn->GetAttachedModuleData(m_nIndexADSpawn, pvData);
}

BOOL AgpmShrine::ConAgpdShrineADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmShrine			*pThis			= (AgpmShrine *)	pClass;
	AgpdShrineADChar	*pAttachedData	= pThis->GetADCharacter((AgpdCharacter *) pData);

	// 데이타 초기화
	pAttachedData->m_bIsGuardian		= FALSE;
	pAttachedData->m_lShrineID			= AP_INVALID_SHRINEID;
	pAttachedData->m_lGuardianLevel		= 0;

	ZeroMemory(pAttachedData->m_lKillGuardianNum, sizeof(INT32) * AGPMSHRINE_MAX_SHRINE * AGPMSHRINE_MAX_LEVEL);
	ZeroMemory(pAttachedData->m_ulKillGuardianTime, sizeof(UINT32) * AGPMSHRINE_MAX_SHRINE * AGPMSHRINE_MAX_LEVEL * AGPMSHRINE_MAX_KILL_GUARDIAN);
	ZeroMemory(pAttachedData->m_bUseShrineNum, sizeof(BOOL)  * AGPMSHRINE_MAX_SHRINE * AGPMSHRINE_MAX_LEVEL);
	ZeroMemory(pAttachedData->m_ulUseShrineTimeMSec, sizeof(UINT32)  * AGPMSHRINE_MAX_SHRINE * AGPMSHRINE_MAX_LEVEL);

	return TRUE;
}

BOOL AgpmShrine::DesAgpdShrineADChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgpmShrine			*pThis			= (AgpmShrine *)	pClass;
	AgpdShrineADChar	*pAttachedData	= pThis->GetADCharacter((AgpdCharacter *) pData);

	return TRUE;
}

BOOL AgpmShrine::ConAgpdShrineADObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmShrine			*pThis			= (AgpmShrine *)	pClass;
	AgpdShrineADObject	*pAttachedData	= pThis->GetADObject((ApdObject *) pData);

	// 데이타 초기화
	ZeroMemory(pAttachedData->m_szShrineName, sizeof(CHAR) * AGPMSHRINE_MAX_NAME);
	pAttachedData->m_lShrineID			= AP_INVALID_SHRINEID;

	return TRUE;
}

BOOL AgpmShrine::DesAgpdShrineADObject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgpmShrine::ConAgpdShrineADSpawn(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmShrine			*pThis			= (AgpmShrine *)	pClass;
	AgpdShrineADSpawn	*pAttachedData	= pThis->GetADSpawn(pData);

	pAttachedData->m_lShrineID			= 0;
	pAttachedData->m_lShrineLevel		= 0;

	ZeroMemory(pAttachedData->m_szShrineName, sizeof(CHAR) * AGPMSHRINE_MAX_NAME);

	return TRUE;
}

BOOL AgpmShrine::DesAgpdShrineADSpawn(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

AgpdShrine* AgpmShrine::CreateShrine()
{
	AgpdShrine *pcsShrine = (AgpdShrine *) CreateModuleData(AGPMSHRINE_DATA_TYPE_SHRINE);

	if (pcsShrine)
	{
		// 초기화
		pcsShrine->m_lID					= AP_INVALID_SHRINEID;
		pcsShrine->m_eType					= APBASE_TYPE_SHRINE;

		pcsShrine->m_Mutex.Init((PVOID) pcsShrine);

		pcsShrine->m_bIsActive				= FALSE;

		pcsShrine->m_ulLastActiveTimeMSec		= 0;
		pcsShrine->m_ulLastInactiveTimeMSec		= 0;
		pcsShrine->m_ulLastActiveDurationMSec	= 0;

		pcsShrine->m_pcsBase					= NULL;

		pcsShrine->m_pcsTemplate				= NULL;

		ZeroMemory(pcsShrine->m_lSkillID, sizeof(INT32) * AGPMSHRINE_MAX_LEVEL);

		m_pagpmFactors->InitFactor(&pcsShrine->m_csSigilFactor);
		pcsShrine->m_lSigilOwnerUnion			= 0;
		ZeroMemory(&pcsShrine->m_csSigilOwnerBase, sizeof(ApBase));
	}

	return pcsShrine;
}

BOOL AgpmShrine::DestroyShrine(AgpdShrine *pcsShrine)
{
	if (!pcsShrine)
		return FALSE;

	m_pagpmFactors->DestroyFactor(&pcsShrine->m_csSigilFactor);

	pcsShrine->m_Mutex.Destroy();

	return DestroyModuleData(pcsShrine, AGPMSHRINE_DATA_TYPE_SHRINE);
}

AgpdShrineTemplate* AgpmShrine::CreateTemplate()
{
	AgpdShrineTemplate *pcsTemplate = (AgpdShrineTemplate *) CreateModuleData(AGPMSHRINE_DATA_TYPE_TEMPLATE);

	if (pcsTemplate)
	{
		// 초기화
		pcsTemplate->m_lID					= AP_INVALID_SHRINEID;
		pcsTemplate->m_eType				= APBASE_TYPE_SHRINE_TEMPLATE;

		pcsTemplate->m_Mutex.Init((PVOID) pcsTemplate);

		ZeroMemory(pcsTemplate->m_szShrineName, sizeof(CHAR) * AGPMSHRINE_MAX_NAME);

		ZeroMemory(pcsTemplate->m_szSkillName, sizeof(CHAR) * AGPMSHRINE_MAX_LEVEL * (AGPMSKILL_MAX_SKILL_NAME + 1));

		pcsTemplate->m_bIsRandomPosition	= FALSE;
		ZeroMemory(pcsTemplate->m_posShrine, sizeof(AuPOS) * AGPMSHRINE_MAX_POSITION);
		pcsTemplate->m_nNumShrinePos		= 0;

		pcsTemplate->m_eUseCondition		= AGPMSHRINE_USE_ONCE;

		pcsTemplate->m_ulUseIntervalTimeHR	= 0;

		pcsTemplate->m_eActiveCondition		= AGPMSHRINE_ACTIVE_ALWAYS;

		pcsTemplate->m_ulMinActiveTimeHR		= 0;
		pcsTemplate->m_ulMaxActiveTimeHR		= 0;

		pcsTemplate->m_eReActiveCondition	= AGPMSHRINE_REACTIVE_NONE;
		pcsTemplate->m_ulReActiveTimeHR		= 0;

		ZeroMemory(pcsTemplate->m_szGuardianSpawnName, sizeof(CHAR) * AGPMSHRINE_MAX_LEVEL * AGPDSPAWN_SPAWN_NAME_LENGTH);

		m_pagpmFactors->InitFactor(&pcsTemplate->m_csSigilAttrFactor);
		m_pagpmFactors->InitFactor(&pcsTemplate->m_csMinValueFactor);
		m_pagpmFactors->InitFactor(&pcsTemplate->m_csMaxValueFactor);
	}

	return pcsTemplate;
}

BOOL AgpmShrine::DestroyTemplate(AgpdShrineTemplate *pcsTemplate)
{
	if (!pcsTemplate)
		return FALSE;

	m_pagpmFactors->DestroyFactor(&pcsTemplate->m_csSigilAttrFactor);
	m_pagpmFactors->DestroyFactor(&pcsTemplate->m_csMinValueFactor);
	m_pagpmFactors->DestroyFactor(&pcsTemplate->m_csMaxValueFactor);

	pcsTemplate->m_Mutex.Release();

	return DestroyModuleData(pcsTemplate, AGPMSHRINE_DATA_TYPE_TEMPLATE);
}

BOOL AgpmShrine::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	INT8		cOperation		= (-1);
	INT32		lShrineID		= AP_INVALID_SHRINEID;
	INT32		lShrineTID		= AP_INVALID_SHRINEID;
	INT8		bIsActive		= (-1);
	AuPOS		posShrine;
	PVOID		pvBasePacket	= NULL;
	INT32		lGeneratorID	= AP_INVALID_CID;

	posShrine.x					= (-0);

	m_csPacket.GetField(TRUE, pvPacket, nSize,
							&cOperation,
							&lShrineID,
							&lShrineTID,
							&bIsActive,
							&posShrine,
							&pvBasePacket,
							&lGeneratorID);

	switch (cOperation) {
	case AGPMSHRINE_OPERATION_ADD_SHRINE:
		{
			if (lShrineID == AP_INVALID_SHRINEID || lShrineTID == AP_INVALID_SHRINEID)
				return FALSE;

			AgpdShrine *pcsShrine = AddShrine(lShrineID, lShrineTID);
			if (!pcsShrine)
				return FALSE;

			pcsShrine->m_Mutex.WLock();

			pcsShrine->m_bIsActive	= (BOOL) bIsActive;
			pcsShrine->m_posShrine	= posShrine;

			pcsShrine->m_Mutex.Release();
		}
		break;

	case AGPMSHRINE_OPERATION_UPDATE_SHRINE:
		{
			if (lShrineID == AP_INVALID_SHRINEID)
				return FALSE;

			AgpdShrine *pcsShrine = GetShrine(lShrineID);
			if (!pcsShrine)
				return FALSE;

			pcsShrine->m_Mutex.WLock();

			if (bIsActive != (-1))
				pcsShrine->m_bIsActive = bIsActive;

			if (posShrine.x != (-0))
				pcsShrine->m_posShrine = posShrine;

			pcsShrine->m_Mutex.Release();
		}
		break;

	case AGPMSHRINE_OPERATION_GENERATE_SHRINE_EVENT:
		{
			if (!pvBasePacket || lGeneratorID == AP_INVALID_CID)
				return FALSE;

			ApdEvent *pcsEvent = m_papmEventManager->GetEventFromBasePacket(pvBasePacket);

			AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(lGeneratorID);
			if (!pcsCharacter)
			{
				return FALSE;
			}

			EnumCallback(AGPMSHRINE_CB_GENERATE_SHRINE_EVENT, pcsEvent, pcsCharacter);

			pcsCharacter->m_Mutex.Release();
		}
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

PVOID AgpmShrine::MakePacketAddShrine(AgpdShrine *pcsShrine, INT16 *pnPacketLength)
{
	if (!pcsShrine || !pcsShrine->m_pcsTemplate || !pnPacketLength)
		return NULL;

	INT8	cOperation = AGPMSHRINE_OPERATION_ADD_SHRINE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSHRINE_PACKET_TYPE,
											&cOperation,
											&pcsShrine->m_lID,
											&pcsShrine->m_pcsTemplate->m_lID,
											&pcsShrine->m_bIsActive,
											&pcsShrine->m_posShrine,
											NULL,
											NULL
											);

	return pvPacket;
}

PVOID AgpmShrine::MakePacketRemoveShrine(AgpdShrine *pcsShrine, INT16 *pnPacketLength)
{
	if (!pcsShrine || !pnPacketLength)
		return NULL;

	INT8	cOperation = AGPMSHRINE_OPERATION_REMOVE_SHRINE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSHRINE_PACKET_TYPE,
											&cOperation,
											&pcsShrine->m_lID,
											NULL,
											NULL,
											NULL,
											NULL,
											NULL
											);

	return pvPacket;
}

PVOID AgpmShrine::MakePacketUpdateActive(AgpdShrine *pcsShrine, INT16 *pnPacketLength)
{
	if (!pcsShrine || !pnPacketLength)
		return NULL;

	INT8	cOperation = AGPMSHRINE_OPERATION_UPDATE_SHRINE;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSHRINE_PACKET_TYPE,
											&cOperation,
											&pcsShrine->m_lID,
											NULL,
											&pcsShrine->m_bIsActive,
											(pcsShrine->m_pcsTemplate->m_bIsRandomPosition) ? &pcsShrine->m_posShrine : NULL,
											NULL,
											NULL
											);

	return pvPacket;
}

PVOID AgpmShrine::MakePacketGenerateShrineEvent(ApdEvent *pcsEvent, ApBase *pcsGenerator, INT16 *pnPacketLength)
{
	if (!pcsEvent || !pcsGenerator || !pnPacketLength)
		return NULL;

	PVOID	pvPacketBase = m_papmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketBase)
		return FALSE;

	INT8	cOperation = AGPMSHRINE_OPERATION_GENERATE_SHRINE_EVENT;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMSHRINE_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											NULL,
											pvPacketBase,
											&pcsGenerator->m_lID
											);

	m_csPacket.FreePacket(pvPacketBase);

	return pvPacket;
}

BOOL AgpmShrine::SetCallbackActiveShrine(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSHRINE_CB_ACTIVE_SHRINE, pfCallback, pClass);
}

BOOL AgpmShrine::SetCallbackInactiveShrine(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSHRINE_CB_INACTIVE_SHRINE, pfCallback, pClass);
}

BOOL AgpmShrine::SetCallbackGenerateShrineEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSHRINE_CB_GENERATE_SHRINE_EVENT, pfCallback, pClass);
}

BOOL AgpmShrine::SetCallbackGenerateTownHallEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSHRINE_CB_GENERATE_TOWNHALL_EVENT, pfCallback, pClass);
}

BOOL AgpmShrine::SetCallbackChangeSigilOwner(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSHRINE_CB_CHANGE_SIGIL_OWNER, pfCallback, pClass);
}

BOOL AgpmShrine::SetCallbackAddShrineEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMSHRINE_CB_ADD_SHRINE_EVENT, pfCallback, pClass);
}

BOOL AgpmShrine::StreamWriteTemplate(CHAR *szFile)
{
	if (!szFile)
		return FALSE;;

	ApModuleStream			csStream;
	INT32					lIndex = 0;
	CHAR					szTID[32];
	AgpdShrineTemplate		*pcsTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// 등록된 모든 shrine Template에 대해서...
	for (pcsTemplate = GetShrineSequenceTemplate(&lIndex); pcsTemplate; pcsTemplate = GetShrineSequenceTemplate(&lIndex))
	{
		sprintf(szTID, "%d", pcsTemplate->m_lID);

		// TID로 Section을 설정하고
		csStream.SetSection(szTID);

		// Stream Enumerate 한다.
		if (!csStream.EnumWriteCallback(AGPMSHRINE_DATA_TYPE_TEMPLATE, pcsTemplate, this))
		{
			OutputDebugString("AgpmShrine::StreamWriteTemplate() Error (1) !!!\n");
			return FALSE;
		}
	}

	csStream.Write(szFile);

	return TRUE;
}

BOOL AgpmShrine::StreamReadTemplate(CHAR *szFile)
{
	if (!szFile)
		return FALSE;

	ApModuleStream		csStream;
	UINT16				nNumKeys;
	INT32				i;
	INT32				lTID;
	AgpdShrineTemplate	*pcsTemplate;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);

	// szFile을 읽는다.
	csStream.Open(szFile);

	nNumKeys = csStream.GetNumSections();

	// 각 Section에 대해서...
	for (i = 0; i < nNumKeys; ++i)
	{
		// Section Name은 TID 이다.
		lTID = atoi(csStream.ReadSectionName(i));

		// Template을 추가한다.
		pcsTemplate = AddShrineTemplate(lTID);
		if (!pcsTemplate)
		{
			OutputDebugString("AgpmShrine::StreamReadTemplate() Error (1) !!!\n");
			return FALSE;
		}

		// Stream Enumerate를 한다. (ApModuleStream에서 Ini File을 읽으면서 적당한 Module에 Callback해준다.
		if (!csStream.EnumReadCallback(AGPMSHRINE_DATA_TYPE_TEMPLATE, pcsTemplate, this))
		{
			OutputDebugString("AgpmShrine::StreamReadTemplate() Error (2) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmShrine::TemplateWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpmShrine				*pThis			= (AgpmShrine *)			pClass;
	AgpdShrineTemplate		*pcsTemplate	= (AgpdShrineTemplate *)	pData;

	// Shrine Template의 값들을 Write한다.
	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_NAME, pcsTemplate->m_szShrineName))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (1) !!!\n");
		return FALSE;
	}

	for (int i = 0; i < AGPMSHRINE_MAX_LEVEL; ++i)
	{
		if (strlen(pcsTemplate->m_szSkillName[i]) > 0)
		{
			if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_SKILL_NAME, pcsTemplate->m_szSkillName[i]))
			{
				OutputDebugString("AgpmShrine::TemplateWriteCB() Error (2) !!!\n");
				return FALSE;
			}
		}
		else
			break;
	}

	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_IS_RANDOM_POS, pcsTemplate->m_bIsRandomPosition))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (3) !!!\n");
		return FALSE;
	}

	for (int i = 0; i < pcsTemplate->m_nNumShrinePos; ++i)
	{
		CHAR	szBuffer[64];
		INT32	lStrlen	= sprintf(szBuffer, "%f,%f,%f", pcsTemplate->m_posShrine[i].x,
											 pcsTemplate->m_posShrine[i].y,
											 pcsTemplate->m_posShrine[i].z);

		ASSERT(lStrlen < 64);

		if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_SHRINE_POS, szBuffer))
		{
			OutputDebugString("AgpmShrine::TemplateWriteCB() Error (4) !!!\n");
			return FALSE;
		}
	}

	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_USE_CONDITION, pcsTemplate->m_eUseCondition))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (5) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_USE_INTERVAL_TIME, (INT32) pcsTemplate->m_ulUseIntervalTimeHR))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (6) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_ACTIVE_CONDITION, pcsTemplate->m_eActiveCondition))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (7) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_MIN_ACTIVE_DURATION, (INT32) pcsTemplate->m_ulMinActiveTimeHR))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (8) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_MAX_ACTIVE_DURATION, (INT32) pcsTemplate->m_ulMaxActiveTimeHR))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (9) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_REACTIVE_CONDITION, pcsTemplate->m_eReActiveCondition))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (10) !!!\n");
		return FALSE;
	}

	if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_REACTIVE_TIME, (INT32) pcsTemplate->m_ulReActiveTimeHR))
	{
		OutputDebugString("AgpmShrine::TemplateWriteCB() Error (11) !!!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL AgpmShrine::TemplateReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	const CHAR				*szValueName;
	AgpmShrine				*pThis			= (AgpmShrine *)			pClass;

	AgpdShrineTemplate		*pcsTemplate	= (AgpdShrineTemplate *)	pData;

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_NAME, strlen(AGPMSHRINE_INI_NAME_NAME)))
		{
			pStream->GetValue(pcsTemplate->m_szShrineName, AGPMSHRINE_MAX_NAME);
			pThis->m_csTemplateAdmin.AddTemplateStringKey(pcsTemplate->m_lID, pcsTemplate->m_szShrineName);
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_SKILL_NAME, strlen(AGPMSHRINE_INI_NAME_SKILL_NAME)))
		{
			int i = 0;
			for (i = 0; i < AGPMSHRINE_MAX_LEVEL; ++i)
			{
				if (strlen(pcsTemplate->m_szSkillName[i]) < 1)
				{
					pStream->GetValue(pcsTemplate->m_szSkillName[i], AGPMSKILL_MAX_SKILL_NAME);
					break;
				}
			}

			if (i == AGPMSHRINE_MAX_LEVEL)
				return FALSE;
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_IS_RANDOM_POS, strlen(AGPMSHRINE_INI_NAME_IS_RANDOM_POS)))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_bIsRandomPosition);
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_SHRINE_POS, strlen(AGPMSHRINE_INI_NAME_SHRINE_POS)))
		{
			if (pcsTemplate->m_nNumShrinePos == AGPMSHRINE_MAX_POSITION)
				return FALSE;

			CHAR	szBuffer[128];
			ZeroMemory(szBuffer, sizeof(CHAR) * 128);

			pStream->GetValue(szBuffer, 128);

			// value format : xxx.xxx,xxx.xxx,xxx.xxx
			sscanf(szBuffer, "%f,%f,%f", 
					&pcsTemplate->m_posShrine[pcsTemplate->m_nNumShrinePos].x, 
					&pcsTemplate->m_posShrine[pcsTemplate->m_nNumShrinePos].y, 
					&pcsTemplate->m_posShrine[pcsTemplate->m_nNumShrinePos].z);

			++pcsTemplate->m_nNumShrinePos;
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_USE_CONDITION, strlen(AGPMSHRINE_INI_NAME_USE_CONDITION)))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_eUseCondition);
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_USE_INTERVAL_TIME, strlen(AGPMSHRINE_INI_NAME_USE_INTERVAL_TIME)))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_ulUseIntervalTimeHR);
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_ACTIVE_CONDITION, strlen(AGPMSHRINE_INI_NAME_ACTIVE_CONDITION)))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_eActiveCondition);
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_MIN_ACTIVE_DURATION, strlen(AGPMSHRINE_INI_NAME_MIN_ACTIVE_DURATION)))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_ulMinActiveTimeHR);
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_MAX_ACTIVE_DURATION, strlen(AGPMSHRINE_INI_NAME_MAX_ACTIVE_DURATION)))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_ulMaxActiveTimeHR);
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_REACTIVE_CONDITION, strlen(AGPMSHRINE_INI_NAME_REACTIVE_CONDITION)))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_eReActiveCondition);
		}
		else if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_REACTIVE_TIME, strlen(AGPMSHRINE_INI_NAME_REACTIVE_TIME)))
		{
			pStream->GetValue((INT32 *) &pcsTemplate->m_ulReActiveTimeHR);
		}
	}

	return TRUE;
}

BOOL AgpmShrine::ObjectWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	AgpmShrine	*pThis			= (AgpmShrine *)	pClass;
	ApdObject	*pcsObject		= (ApdObject *)		pData;

	AgpdShrineADObject	*pcsShrineADObject = pThis->GetADObject(pcsObject);

	if (pcsShrineADObject->m_szShrineName && strlen(pcsShrineADObject->m_szShrineName))
	{
		AgpdShrine	*pcsShrine = pThis->GetShrine(pcsShrineADObject->m_lShrineID);

		if (!pStream->WriteValue(AGPMSHRINE_INI_NAME_SHRINE_NAME, pcsShrine->m_pcsTemplate->m_szShrineName))
		{
			OutputDebugString("AgpmShrine::TemplateWriteCB() Error (1) !!!\n");
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AgpmShrine::ObjectReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	if (!pData || !pClass || !pStream)
		return FALSE;

	const CHAR				*szValueName;
	AgpmShrine				*pThis			= (AgpmShrine *)			pClass;

	ApdObject				*pcsObject		= (ApdObject *)				pData;

	AgpdShrineADObject		*pcsShrineADObject = pThis->GetADObject(pcsObject);

	// 다음 Value가 없을때까지 각 항목에 맞는 값을 읽는다.
	while (pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		if (!strncmp(szValueName, AGPMSHRINE_INI_NAME_SHRINE_NAME, strlen(AGPMSHRINE_INI_NAME_SHRINE_NAME)))
		{
			pStream->GetValue(pcsShrineADObject->m_szShrineName, AGPMSHRINE_MAX_NAME);

			AgpdShrineTemplate *pcsTemplate = pThis->GetShrineTemplate(pcsShrineADObject->m_szShrineName);
			if (pcsTemplate)
			{
				AgpdShrine *pcsShrine = pThis->AddShrine(pcsTemplate->m_lID, pcsTemplate->m_lID);
				if (pcsShrine)
				{
					pcsShrineADObject->m_lShrineID = pcsShrine->m_lID;
					pcsShrine->m_pcsBase = (ApBase *) pcsObject;
				}
			}
		}
	}

	return TRUE;
}

BOOL AgpmShrine::IsGuardian(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdShrineADChar *pcsAttachData = GetADCharacter(pcsCharacter);
	if (!pcsAttachData) return FALSE;

	return pcsAttachData->m_bIsGuardian;
}

//		GetSigilFactor
//	Functions
//		- 이 사원에 소속된 sigil이 있는 경우 그 sigil 아템의 속성을 pcsFactor에 세팅한다.
//			(sigil 아템을 생성해서 pcsFactor에 넣어 넘겨준다고 생각하면 된다)
//	Arguments
//		- pcsShrine	: 사원 포인터
//		- pcsFactor	: sigil 속성을 넘겨받을 factor pointer
//	Return value
//		- BOOL : 성공 여부 (sigil을 가지고 있지 않는 경우나 factor 세팅이 실패한 경우 FALSE 리턴)
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmShrine::GetSigilFactor(AgpdShrine *pcsShrine, AgpdFactor *pcsFactor)
{
	if (!pcsShrine || !pcsShrine->m_pcsTemplate || !pcsFactor)
		return FALSE;

	AgpdShrineTemplate	*pcsTemplate = pcsShrine->m_pcsTemplate;

	if (!pcsTemplate->m_bIsSigil)
		return FALSE;

	// m_csSigilAttrFactor, m_csMinValueFactor, m_csMaxValueFactor를 참조해 sigil factor를 생성한다.
	// 생성된 factor는 m_csSigilFactor에 저장한다.

	m_pagpmFactors->InitFactor(&pcsShrine->m_csSigilFactor);

	// 현재 sigil은 status, magic defense 속성만 갖는다.

	// status를 살펴본다.
	AgpdFactorCharStatus *pcsSigilCharStatus = 
		(AgpdFactorCharStatus *) m_pagpmFactors->GetFactor(&pcsShrine->m_csSigilFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	AgpdFactorCharStatus *pcsCharStatusProb = 
		(AgpdFactorCharStatus *) m_pagpmFactors->GetFactor(&pcsTemplate->m_csSigilAttrFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	AgpdFactorCharStatus *pcsCharStatusMinValue = 
		(AgpdFactorCharStatus *) m_pagpmFactors->GetFactor(&pcsTemplate->m_csMinValueFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);
	AgpdFactorCharStatus *pcsCharStatusMaxValue = 
		(AgpdFactorCharStatus *) m_pagpmFactors->GetFactor(&pcsTemplate->m_csMaxValueFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	if (pcsSigilCharStatus && pcsCharStatusProb && pcsCharStatusMaxValue)
	{
		for (int i = 0; i < AGPD_FACTORS_CHARSTATUS_MAX_TYPE; ++i)
		{
			if (pcsCharStatusProb->lValue[i] != 0)
			{
				INT32	lRandomNumber = m_csRandom.randInt(99) + 1;
				if (lRandomNumber <= pcsCharStatusProb->lValue[i])
				{
					INT32	lMinValue	= 0;
					if (pcsCharStatusMinValue)
						lMinValue = pcsCharStatusMinValue->lValue[i];

					INT32	lDiffer	= pcsCharStatusMaxValue->lValue[i] - lMinValue;
					if (lDiffer >= 0)
					{
						INT32	lRandomValue = lMinValue + m_csRandom.randInt(lDiffer);

						pcsSigilCharStatus->lValue[i] = lRandomValue;
					}
				}
			}
		}
	}
	
	// magic defense를 살펴본다.
	AgpdFactorDefense *pcsSigilDefense = 
		(AgpdFactorDefense *) m_pagpmFactors->GetFactor(&pcsShrine->m_csSigilFactor, AGPD_FACTORS_TYPE_DEFENSE);

	AgpdFactorDefense *pcsDefenseProb = 
		(AgpdFactorDefense *) m_pagpmFactors->GetFactor(&pcsTemplate->m_csSigilAttrFactor, AGPD_FACTORS_TYPE_DEFENSE);

	AgpdFactorDefense *pcsDefenseMinValue = 
		(AgpdFactorDefense *) m_pagpmFactors->GetFactor(&pcsTemplate->m_csMinValueFactor, AGPD_FACTORS_TYPE_DEFENSE);
	AgpdFactorDefense *pcsDefenseMaxValue = 
		(AgpdFactorDefense *) m_pagpmFactors->GetFactor(&pcsTemplate->m_csMaxValueFactor, AGPD_FACTORS_TYPE_DEFENSE);

	if (pcsSigilDefense && pcsDefenseProb && pcsDefenseMaxValue)
	{
		for (int i = 0; i < AGPD_FACTORS_ATTRIBUTE_MAX_TYPE; ++i)
		{
			if (pcsDefenseProb->csValue[AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT].lValue[i] != 0)
			{
				INT32	lRandomNumber = m_csRandom.randInt(99) + 1;
				if (lRandomNumber <= pcsDefenseProb->csValue[AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT].lValue[i])
				{
					INT32	lMinValue	= 0;
					if (pcsDefenseMinValue)
						lMinValue = pcsDefenseMinValue->csValue[AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT].lValue[i];

					INT32	lDiffer	= pcsDefenseMaxValue->csValue[AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT].lValue[i] - lMinValue;
					if (lDiffer >= 0)
					{
						INT32	lRandomValue = lMinValue + m_csRandom.randInt(lDiffer);

						pcsSigilDefense->csValue[AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT].lValue[i] = lRandomValue;
					}
				}
			}
		}
	}

	// 위에서 만든 factor를 pcsFactor에 복사한다.
	return m_pagpmFactors->CopyFactor(pcsFactor, &pcsShrine->m_csSigilFactor, FALSE);
}

BOOL AgpmShrine::ChangeSigilOwner(AgpdShrine *pcsShrine, ApBase *pcsBase)
{
	if (!pcsShrine || !pcsBase)
		return FALSE;

	ApBase	csPrevOwner	= pcsShrine->m_csSigilOwnerBase;

	pcsShrine->m_csSigilOwnerBase.m_eType	= pcsBase->m_eType;
	pcsShrine->m_csSigilOwnerBase.m_lID		= pcsBase->m_lID;

	EnumCallback(AGPMSHRINE_CB_CHANGE_SIGIL_OWNER, pcsShrine, &csPrevOwner);

	return TRUE;
}

BOOL AgpmShrine::CBChangeItemOwner(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgpmShrine		*pThis		= (AgpmShrine *)	pClass;
	AgpmItem		*pcsItem	= (AgpmItem *)		pData;
	INT32			lPrevOwner	= *(INT32 *)		pCustData;

	return TRUE;
}

//		AddKillGuardian
//	Functions
//		- pcsKiller가 pcsGuardian을 ulKillTime에 죽였다.
//			1. pcsGuardian이 어느 사원의 어느 레벨에 속하는 넘인지 구한다.
//			2. pcsKiller의 AgpdShrineADChar에서 m_lKillGuardianNum, m_ulKillGuardianTime를 업데이트한다.
//	Arguments
//		- pcsKiller		: 가디언을 죽인넘
//		- pcsGuardian	: 죽은 가디언
//		- ulKillTime	: 가디언을 죽인 시간
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmShrine::AddKillGuardian(AgpdCharacter *pcsKiller, AgpdCharacter *pcsGuardian, UINT32 ulKillTime)
{
	if (!pcsKiller || !pcsGuardian || ulKillTime == 0)
		return FALSE;

	if (!IsGuardian(pcsGuardian))
		return FALSE;

	AgpdShrineADChar	*pcsKillerADCharacter = GetADCharacter(pcsKiller);
	if (!pcsKillerADCharacter)
		return FALSE;

	AgpdShrineADChar	*pcsGuardianADCharacter = GetADCharacter(pcsGuardian);
	if (!pcsGuardianADCharacter)
		return FALSE;

	// pcsGuardian을 보고 이넘이 어느 사원의 어느 레벨에 속한 넘인지 가져와야 한다.

	INT32	lShrineID		= pcsGuardianADCharacter->m_lShrineID;
	INT32	lGuardianLevel	= pcsGuardianADCharacter->m_lGuardianLevel;

	// 죽인 시간을 입력한다. 배열이 꽉찬경우 앞으로 한칸씩 shift시키고 맨 뒤에 추가한다.
	if (pcsKillerADCharacter->m_lKillGuardianNum[lShrineID][lGuardianLevel] >= AGPMSHRINE_MAX_KILL_GUARDIAN)
	{
		CopyMemory(pcsKillerADCharacter->m_ulKillGuardianTime[lShrineID][lGuardianLevel],
				   pcsKillerADCharacter->m_ulKillGuardianTime[lShrineID][lGuardianLevel] + 1,
				   sizeof(UINT32) * (AGPMSHRINE_MAX_KILL_GUARDIAN - 1));

		pcsKillerADCharacter->m_ulKillGuardianTime[lShrineID][lGuardianLevel][AGPMSHRINE_MAX_KILL_GUARDIAN - 1] = ulKillTime;
	}
	else
		pcsKillerADCharacter->m_ulKillGuardianTime[lShrineID][lGuardianLevel][pcsKillerADCharacter->m_lKillGuardianNum[lShrineID][lGuardianLevel]] = ulKillTime;

	// 죽인넘 갯수를 하나 증가시킨다.
	++pcsKillerADCharacter->m_lKillGuardianNum[lShrineID][lGuardianLevel];

	return TRUE;
}

//		ResetKillGuardian
//	Functions
//		- pcsKiller의 가디언 죽인 정보 중 pcsShrine의 lGuardianLevel 값들을 초기화한다.
//	Arguments
//		- pcsCharacter	: 초기화 할 캐릭터
//		- pcsShrine		: 초기화 할 사원
//		- lGuardianLevel : 사원의 레벨중 어느건지...
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmShrine::ResetKillGuardian(AgpdCharacter *pcsCharacter, AgpdShrine *pcsShrine, INT32 lGuardianLevel)
{
	if (!pcsCharacter || !pcsShrine || lGuardianLevel < 0 || lGuardianLevel >= AGPMSHRINE_MAX_LEVEL)
		return FALSE;

	AgpdShrineADChar	*pcsADCharacter = GetADCharacter(pcsCharacter);
	if (!pcsADCharacter)
		return FALSE;

	pcsADCharacter->m_lKillGuardianNum[pcsShrine->m_lID][lGuardianLevel] = 0;
	ZeroMemory(pcsADCharacter->m_ulKillGuardianTime[pcsShrine->m_lID][lGuardianLevel], sizeof(UINT32) * AGPMSHRINE_MAX_KILL_GUARDIAN);

	return TRUE;
}

//		IsKillGuardianConditionTrue
//	Functions
//		- pcsKiller가 pcsShrine의 lGuardianLevel 에 속한 가디언을 ulKillTimeLimit 안에 lKillCount 만큼 죽였는지 검사한다.
//	Arguments
//		- pcsKiller	: 검사할 캐릭터
//		- pcsShrine	: 사원
//		- lGuardianLevel : 사원의 레벨중 어느건지...
//		- lKillCount : 몇마리 죽여야 하는지
//		- ulKillTimeLimit : 얼마 시간안에..
//	Return value
//		- BOOL : 
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmShrine::IsKillGuardianConditionTrue(AgpdCharacter *pcsKiller, AgpdShrine *pcsShrine, INT32 lGuardianLevel, 
											 INT32 lKillCount, UINT32 ulKillTimeLimit)
{
	if (!pcsKiller || !pcsShrine || lGuardianLevel < 0 || lGuardianLevel > AGPMSHRINE_MAX_LEVEL || lKillCount < 1 || ulKillTimeLimit == 0)
		return FALSE;

	AgpdShrineADChar	*pcsADCharacter = GetADCharacter(pcsKiller);
	if (!pcsADCharacter)
		return FALSE;

	// 먼저 몇마리 죽였는지 본다.
	if (pcsADCharacter->m_lKillGuardianNum[pcsShrine->m_lID][lGuardianLevel] < lKillCount)
		return FALSE;

	// 위에서 몇마리 죽였는지 봤으니 이제 죽인 시간을 검사해본다.
	INT32	lKillGuardian = pcsADCharacter->m_lKillGuardianNum[pcsShrine->m_lID][lGuardianLevel];
	if (lKillGuardian > AGPMSHRINE_MAX_KILL_GUARDIAN)
		lKillGuardian = AGPMSHRINE_MAX_KILL_GUARDIAN;

	INT32	lKillInterval = 
		pcsADCharacter->m_ulKillGuardianTime[pcsShrine->m_lID][lGuardianLevel][lKillGuardian - 1] -
		pcsADCharacter->m_ulKillGuardianTime[pcsShrine->m_lID][lGuardianLevel][lKillGuardian - lKillCount];

	if (lKillInterval > (INT32)ulKillTimeLimit)
		return FALSE;

	return TRUE;
}

BOOL AgpmShrine::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent		*pstEvent	= (ApdEvent *)			pData;
	AgpmShrine		*pThis		= (AgpmShrine *)		pClass;
	ApModuleStream	*pStream	= (ApModuleStream *)	pCustData;

	AgpdShrine		*pcsShrine	= (AgpdShrine *) pstEvent->m_pvData;
	if (!pcsShrine)
		return FALSE;

	pStream->WriteValue(AGPMSHRINE_INI_NAME_EVENT_SHRINE_NAME, pcsShrine->m_pcsTemplate->m_szShrineName);

	return TRUE;
}

BOOL AgpmShrine::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent		*pstEvent	= (ApdEvent *)			pData;
	AgpmShrine		*pThis		= (AgpmShrine *)		pClass;
	ApModuleStream	*pStream	= (ApModuleStream *)	pCustData;

	if (!pStream->ReadNextValue())
		return FALSE;

	const CHAR	*szName = pStream->GetValueName();
	if (strncmp(szName, AGPMSHRINE_INI_NAME_EVENT_SHRINE_NAME, strlen(AGPMSHRINE_INI_NAME_EVENT_SHRINE_NAME)) != 0)
		return FALSE;

	CHAR	szShrineName[128];
	ZeroMemory(szShrineName, sizeof(CHAR) * 128);

	if (!pStream->GetValue(szShrineName, 128))
		return FALSE;

	AgpdShrineTemplate	*pcsTemplate = pThis->GetShrineTemplate(szShrineName);
	if (!pcsTemplate)
		return FALSE;

	return pThis->EnumCallback(AGPMSHRINE_CB_ADD_SHRINE_EVENT, pstEvent, pcsTemplate);
}

BOOL AgpmShrine::CBSpawnRead(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgpmShrine *				pThis = (AgpmShrine *) pClass;
	AgpdSpawn *					pstSpawn = (AgpdSpawn *) pData;
	AgpdShrineADSpawn *			pstShrineADSpawn = pThis->GetADSpawn(pstSpawn);

	while (pStream->ReadNextValue())
	{
		if (!strcmp(pStream->GetValueName(), AGPMSHRINE_INI_NAME_SPAWN_SHRINE_NAME))
		{
			pStream->GetValue(pstShrineADSpawn->m_szShrineName, AGPMSHRINE_MAX_NAME);
		}
		else if (!strcmp(pStream->GetValueName(), AGPMSHRINE_INI_NAME_SPAWN_GUARDIAN_LEVEL))
		{
			pStream->GetValue((INT32 *) &pstShrineADSpawn->m_lShrineLevel);
		}
		else if (!strcmp(pStream->GetValueName(), AGPMSHRINE_INI_NAME_END))
		{
			pStream->ReadNextValue();
			return TRUE;
		}
	}

	return TRUE;
}
