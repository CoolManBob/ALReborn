#include "AgsmAreaChatting.h"
#include "AuStrTable.h"

#include <process.h>

#include "AgpmBattleGround.h"

#ifdef	__PROFILE__
#include "API_AuFrameProfile.h"
#endif


#define	AGSMAREACHATTING_INTERVAL			10000


UINT32	g_aulAreaChatColor[3] = 
{
	0xffff81f9,			// 종족나팔
	0xffffff00,			// 패왕외침
	0xffff0000			// 천상천하
};



AgsmAreaChatting::AgsmAreaChatting()
{
	SetModuleName("AgsmAreaChatting");
	
	m_pagpmBattleGround		= NULL;
	m_pagsmSystemMessage	= NULL;

	m_lMaxUser	= 0;
	m_lMaxClient	= 0;
}

AgsmAreaChatting::~AgsmAreaChatting()
{
}

BOOL AgsmAreaChatting::OnAddModule()
{
	m_pcsAgpmLog			= (AgpmLog *)				GetModule("AgpmLog");
	m_pcsApmMap				= (ApmMap *)				GetModule("ApmMap");
	m_pcsAgpmFactors		= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pcsAgpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmAreaChatting	= (AgpmAreaChatting *)		GetModule("AgpmAreaChatting");
	m_pcsAgpmAdmin			= (AgpmAdmin *)				GetModule("AgpmAdmin");
	m_pcsAgsmCharacter		= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pcsAgsmItem			= (AgsmItem *)				GetModule("AgsmItem");
	m_pcsAgsmChatting		= (AgsmChatting *)			GetModule("AgsmChatting");
	m_pcsAgsmServerManager2	= (AgsmServerManager2 *)	GetModule("AgsmServerManager2");
	m_pcsAgsmGlobalChatting	= (AgsmGlobalChatting *)	GetModule("AgsmGlobalChatting");
	
	if (!m_pcsAgpmLog ||
		!m_pcsApmMap ||
		!m_pcsAgpmFactors ||
		!m_pcsAgpmCharacter ||
		!m_pcsAgpmAreaChatting ||
		!m_pcsAgpmAdmin ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmItem ||
		!m_pcsAgsmChatting ||
		!m_pcsAgsmServerManager2 ||
		!m_pcsAgsmGlobalChatting)
		return FALSE;

	m_nIndexAttachCharacterData	= m_pcsAgpmCharacter->AttachCharacterData(this, sizeof(AgsdAreaChatting), NULL, NULL);
	if (m_nIndexAttachCharacterData < 0)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackBindingRegionChange(CBRegionChange, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackAddCharacterToMap(CBAddCharacterToMap, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackRemoveCharacterFromMap(CBRemoveCharacterFromMap, this))
		return FALSE;

	if (!m_pcsAgpmAreaChatting->SetCallbackRecvSendMessage(CBRecvChatMessage, this))
		return FALSE;

	if (!m_pcsAgsmChatting->SetCallbackAreaChattingRace(CBRecvRaceChatMessage, this))
		return FALSE;
	if (!m_pcsAgsmChatting->SetCallbackAreaChattingAll(CBRecvAllChatMessage, this))
		return FALSE;
	if (!m_pcsAgsmChatting->SetCallbackArchlordMessage(CBRecvArchlordMessage, this))
		return FALSE;

	if (!m_pcsAgsmGlobalChatting->SetCallbackReceiveBroadCastMessage(CBRecvBroadCastMessage, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmAreaChatting::OnInit()
{
	m_pagpmBattleGround		= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagsmSystemMessage	= (AgsmSystemMessage*)GetModule("AgsmSystemMessage");
	
	if(!m_pagpmBattleGround || !m_pagsmSystemMessage)
		return FALSE;
		
	for (int i = 0; i < AGSMAREACHATTING_MAX_REGIONTEMPLATE; ++i)
	{
		m_acsRegionAdmin[i].InitializeObject(sizeof(UINT32), 3000, NULL, NULL, this);
	}

	for (int i = 0; i < AURACE_TYPE_MAX; ++i)
	{
		m_acsRaceAdmin[i].InitializeObject(sizeof(UINT32), 5000, NULL, NULL, this);
	}

	m_csAllPlayerAdmin.InitializeObject(sizeof(UINT32), 7000, NULL, NULL, this);

	m_bDestroyThread	= FALSE;

	m_csReceivePool.Initialize(sizeof(AgsmChatReceiveBuffer), 2000, _T("AgsmAreaChatting::m_csReceivePool"));

	m_csSendCircularBuffer.Init(sizeof(AgsmChatReceiveBuffer) * 3000);

	m_stSendThread.hThread = (HANDLE)_beginthreadex(NULL, 0, AgsmAreaChatting::SendChatThread, 
													this, 0, &m_stSendThread.ulThreadID);

	if ((!m_stSendThread.hThread) || (!m_stSendThread.ulThreadID)) 
	{
		// Thread 생성 실패
		DebugBreak();
		return FALSE;
	}

	SetThreadName(m_stSendThread.ulThreadID, "SendChatDataThread");

	return TRUE;
}

BOOL AgsmAreaChatting::OnDestroy()
{
	m_bDestroyThread	= TRUE;

	::WaitForSingleObject(m_stSendThread.hThread, INFINITE);

	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result()) return FALSE;

	// Queue에 남아있는 Data를 모두 삭제한다.
	while (!m_stlQueue.empty())
	{
		AgsmChatReceiveBuffer* pQueueData = m_stlQueue.front();
		m_csReceivePool.Free(pQueueData);
		m_stlQueue.pop();

//		delete pQueueData;
	}

	m_csAllPlayerAdmin.RemoveObjectAll(FALSE);

	for (int i = 0; i < AURACE_TYPE_MAX; ++i)
	{
		m_acsRaceAdmin[i].RemoveObjectAll(FALSE);
	}

	for (int i = 0; i < AGSMAREACHATTING_MAX_REGIONTEMPLATE; ++i)
	{
		m_acsRegionAdmin[i].RemoveObjectAll(FALSE);
	}

	return TRUE;
}

BOOL AgsmAreaChatting::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmAreaChatting	*pThis				= (AgsmAreaChatting *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	INT16				nPrevRegion			= *(INT16 *)			pCustData;

	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	if (nPrevRegion >= 0 && nPrevRegion < AGSMAREACHATTING_MAX_REGIONTEMPLATE)
		pThis->m_acsRegionAdmin[nPrevRegion].RemoveObject(pcsAgsdCharacter->m_dpnidCharacter);

	INT16	nRegionIndex	= pThis->m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);

	if (nRegionIndex >= 0 && nRegionIndex < AGSMAREACHATTING_MAX_REGIONTEMPLATE)
		pThis->m_acsRegionAdmin[nRegionIndex].AddObject(&pcsAgsdCharacter->m_dpnidCharacter, pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmAreaChatting::CBAddCharacterToMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmAreaChatting	*pThis				= (AgsmAreaChatting *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	pThis->m_csAllPlayerAdmin.AddObject(&pcsAgsdCharacter->m_dpnidCharacter, pcsAgsdCharacter->m_dpnidCharacter);

	if(pThis->m_pcsAgpmAdmin->IsAdminCharacter(pcsCharacter))
	{
		for(int i = 0; i < 12; i++)
			pThis->m_acsRaceAdmin[i].AddObject(&pcsAgsdCharacter->m_dpnidCharacter, pcsAgsdCharacter->m_dpnidCharacter);
	}
	else
	{
		INT32	lRace	= pThis->m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
		if (lRace >= 0)
			pThis->m_acsRaceAdmin[lRace].AddObject(&pcsAgsdCharacter->m_dpnidCharacter, pcsAgsdCharacter->m_dpnidCharacter);
	}
	
	INT16	nRegionIndex	= pThis->m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);

	if (nRegionIndex >= 0 && nRegionIndex < AGSMAREACHATTING_MAX_REGIONTEMPLATE)
		pThis->m_acsRegionAdmin[nRegionIndex].AddObject(&pcsAgsdCharacter->m_dpnidCharacter, pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmAreaChatting::CBRemoveCharacterFromMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmAreaChatting	*pThis				= (AgsmAreaChatting *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	if (!pThis->m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return TRUE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRemoveCharacterFromMap"));

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_dpnidCharacter == 0)
		return TRUE;

	pThis->m_csAllPlayerAdmin.RemoveObject(pcsAgsdCharacter->m_dpnidCharacter);

	if(pThis->m_pcsAgpmAdmin->IsAdminCharacter(pcsCharacter))
	{
		for(int i = 0; i < 12; i++)
			pThis->m_acsRaceAdmin[i].RemoveObject(pcsAgsdCharacter->m_dpnidCharacter);
	}
	else
	{
		INT32	lRace	= pThis->m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
		if (lRace >= 0)
			pThis->m_acsRaceAdmin[lRace].RemoveObject(pcsAgsdCharacter->m_dpnidCharacter);
	}

	INT16	nRegionIndex	= pThis->m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);

	if (nRegionIndex >= 0 && nRegionIndex < AGSMAREACHATTING_MAX_REGIONTEMPLATE)
		pThis->m_acsRegionAdmin[nRegionIndex].RemoveObject(pcsAgsdCharacter->m_dpnidCharacter);

	return TRUE;
}

BOOL AgsmAreaChatting::CBRecvChatMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmAreaChatting	*pThis				= (AgsmAreaChatting *)	pClass;
	INT32				lCID				= *(INT32 *)			pData;

	PVOID				*ppvBuffer			= (PVOID *)				pCustData;

	//INT8				cIsAdmin			= (INT8)				ppvBuffer[0];
	CHAR				*szMessage			= (CHAR *)				ppvBuffer[1];
	UINT16				unMessageLength		= (UINT16)				ppvBuffer[2];

	if (lCID == AP_INVALID_CID ||
		!szMessage ||
		!szMessage[0] ||
		unMessageLength < 1 ||
		unMessageLength >= 256)
		return FALSE;

	if (!pThis->CheckColorText(szMessage, unMessageLength))
		return FALSE;

	AgpdCharacter	*pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	// Chatting 불가능 캐릭터라면 채팅을 아예 원천봉쇄한다.
	if(pThis->m_pcsAgpmCharacter->IsDisableChattingCharacter(pcsCharacter) == TRUE)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	// Chatting 불가능 지역이라면 채팅을 못하게 막는다. 
	if(!pThis->m_pcsAgpmAdmin->IsAdminCharacter(pcsCharacter))
	{
		if(pThis->m_pcsApmMap->CheckRegionPerculiarity(pcsCharacter->m_nBindingRegionIndex, APMMAP_PECULIARITY_CHATTING) == APMMAP_PECULIARITY_RETURN_DISABLE_USE)
		{
			if( pThis->m_pagsmSystemMessage )
				pThis->m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );

			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}
	}

	if(pThis->m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
	{
		if( pThis->m_pagsmSystemMessage )
			pThis->m_pagsmSystemMessage->SendSystemMessage( pcsCharacter , AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS );

		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
	{
		// 15레벨 이상부터 외치기를 할 수 있다.
		if (pThis->m_pcsAgpmCharacter->GetLevel(pcsCharacter) < 15)
		{
			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}

		UINT32	ulCurrentClockCount	= pThis->GetClockCount();

		AgsdAreaChatting	*pcsAttachData	= pThis->GetAttachCharacterData(pcsCharacter);
		if (pcsAttachData->m_ulLastReceivedTimeMSec + AGSMAREACHATTING_INTERVAL > ulCurrentClockCount)
		{
			pcsCharacter->m_Mutex.Release();
			return FALSE;
		}

		pcsAttachData->m_ulLastReceivedTimeMSec	= ulCurrentClockCount;
	}

	AgsmChatReceiveBuffer	*pstBuffer	= (AgsmChatReceiveBuffer *) pThis->m_csReceivePool.Alloc();
	if (!pstBuffer)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	ZeroMemory(pstBuffer, sizeof(AgsmChatReceiveBuffer));

	pstBuffer->lSendCID		= lCID;
	pstBuffer->lSize		= unMessageLength;
	CopyMemory(pstBuffer->szPacketBuffer, szMessage, unMessageLength);
	pstBuffer->eChatType	= AGSMAREACHAT_TYPE_REGION;

	// 2006.07.12. steeple
	if(pThis->m_pcsAgpmLog->m_bWriteChattingLog)
	{
		pThis->m_pcsAgpmLog->WriteLog_Chatting(0, pcsCharacter->m_szID, szMessage, unMessageLength);
	}

	pcsCharacter->m_Mutex.Release();

	AuAutoLock Lock(pThis->m_Mutex);
	if (Lock.Result())
		pThis->m_stlQueue.push(pstBuffer);

	return TRUE;
}

BOOL AgsmAreaChatting::CBRecvRaceChatMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAreaChatting	*pThis			= (AgsmAreaChatting *)	pClass;
	AgpdChatData		*pstChatData	= (AgpdChatData *)		pData;

	if (pstChatData->lMessageLength > 1024)
		return FALSE;

	if (!pThis->CheckColorText(pstChatData->szMessage, pstChatData->lMessageLength))
		return FALSE;

	AgsmChatReceiveBuffer	*pstBuffer	= (AgsmChatReceiveBuffer *) pThis->m_csReceivePool.Alloc();
	if (!pstBuffer)
		return FALSE;

	ZeroMemory(pstBuffer, sizeof(AgsmChatReceiveBuffer));

	pstBuffer->lSendCID		= pstChatData->pcsSenderBase->m_lID;
	pstBuffer->lSize		= pstChatData->lMessageLength;
	CopyMemory(pstBuffer->szPacketBuffer, pstChatData->szMessage, pstChatData->lMessageLength);
	pstBuffer->eChatType	= AGSMAREACHAT_TYPE_RACE;

	AuAutoLock Lock(pThis->m_Mutex);
	if (Lock.Result())
		pThis->m_stlQueue.push(pstBuffer);

	return TRUE;
}

BOOL AgsmAreaChatting::CBRecvAllChatMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAreaChatting	*pThis			= (AgsmAreaChatting *)	pClass;
	AgpdChatData		*pstChatData	= (AgpdChatData *)		pData;

	if (pstChatData->lMessageLength > 1024)
		return FALSE;

	if (!pThis->CheckColorText(pstChatData->szMessage, pstChatData->lMessageLength))
		return FALSE;

	AgsmChatReceiveBuffer	*pstBuffer	= (AgsmChatReceiveBuffer *) pThis->m_csReceivePool.Alloc();
	if (!pstBuffer)
		return FALSE;

	ZeroMemory(pstBuffer, sizeof(AgsmChatReceiveBuffer));

	pstBuffer->lSendCID		= pstChatData->pcsSenderBase->m_lID;
	pstBuffer->lSize		= pstChatData->lMessageLength;
	CopyMemory(pstBuffer->szPacketBuffer, pstChatData->szMessage, pstChatData->lMessageLength);
	pstBuffer->eChatType	= AGSMAREACHAT_TYPE_ALL;

	AuAutoLock Lock(pThis->m_Mutex);
	if (Lock.Result())
		pThis->m_stlQueue.push(pstBuffer);

	return TRUE;
}

BOOL AgsmAreaChatting::CBRecvBroadCastMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;
	
	AgsmAreaChatting	*pThis				= (AgsmAreaChatting *)	pClass;

	PVOID				*ppvBuffer			= (PVOID *)	pData;

	AgsmGlobalChatType	eChatType			= (AgsmGlobalChatType)	PtrToInt(ppvBuffer[0]);
	CHAR				*pszSenderName		= (CHAR *)	ppvBuffer[1];
	CHAR				*pszServerName		= (CHAR *)	ppvBuffer[2];
	CHAR				*pszChatMessage		= (CHAR *)	ppvBuffer[3];
	UINT16				unChatMessageLength	= (UINT16)	PtrToInt(ppvBuffer[4]);

	if (unChatMessageLength > 1024)
		return FALSE;

	AgsmChatReceiveBuffer	*pstBuffer	= (AgsmChatReceiveBuffer *) pThis->m_csReceivePool.Alloc();
	if (!pstBuffer)
		return FALSE;

	ZeroMemory(pstBuffer, sizeof(AgsmChatReceiveBuffer));

	pstBuffer->lSize		= unChatMessageLength;
	pstBuffer->eChatType	= AGSMAREACHAT_TYPE_GLOBAL;
	CopyMemory(pstBuffer->szSenderName, pszSenderName, sizeof(CHAR) * AGPDCHARACTER_MAX_ID_LENGTH);
	CopyMemory(pstBuffer->szServerName, pszServerName, sizeof(CHAR) * AGSM_MAX_SERVER_NAME);

	CopyMemory(pstBuffer->szPacketBuffer, pszChatMessage, unChatMessageLength);

	if (eChatType == AGSMGLOBALCHAT_TYPE_NOTIFY)
		pstBuffer->bIsNotify	= TRUE;

	// 2006.07.12. steeple
	if(pThis->m_pcsAgpmLog->m_bWriteChattingLog)
		pThis->m_pcsAgpmLog->WriteLog_Chatting(0, pstBuffer->szSenderName, pszChatMessage, unChatMessageLength);

	AuAutoLock Lock(pThis->m_Mutex);
	if (Lock.Result())
		pThis->m_stlQueue.push(pstBuffer);

	return TRUE;
}

BOOL AgsmAreaChatting::SetMaxRegionChar(INT32 lMaxUser)
{
	m_lMaxUser	= lMaxUser;

	return TRUE;
}

BOOL AgsmAreaChatting::SetMaxClient(INT32 lMaxClient)
{
	m_lMaxClient	= lMaxClient;

	return TRUE;
}

UINT WINAPI AgsmAreaChatting::SendChatThread(PVOID pvParam)
{
	if (!pvParam)
		return FALSE;

	AgsmAreaChatting	*pThis	= (AgsmAreaChatting *)	pvParam;

	while (TRUE)
	{
		Sleep(100);

		if (pThis->m_bDestroyThread)
			break;

		if (!pThis->m_stlQueue.empty())
		{
			AuAutoLock Lock(pThis->m_Mutex);
			if (!Lock.Result())
				continue;

			AgsmChatReceiveBuffer	*pcsBuffer = pThis->m_stlQueue.front();
			if (!pcsBuffer)
				continue;

			//////////////////////////////////////////////////////////////////////////
			{
				if (pcsBuffer->eChatType == AGSMAREACHAT_TYPE_GLOBAL)
				{
					pThis->BroadCastMessage(pcsBuffer);
				}
				else
				{
					AgpdCharacter	*pcsCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter(pcsBuffer->lSendCID);
					if (pcsCharacter)
					{
						if (!pThis->m_pcsAgpmCharacter->IsAllBlockStatus(pcsCharacter))
						{
							switch (pcsBuffer->eChatType) {
							case AGSMAREACHAT_TYPE_REGION:
								{
									INT16	nPacketLength	= 0;
									PVOID	pvPacket		= pThis->MakeAreaChatPacket(pcsBuffer, pcsCharacter, &nPacketLength);
									pThis->SendPacketToRegion(pcsCharacter, pvPacket, nPacketLength);
								}
								break;

							case AGSMAREACHAT_TYPE_RACE:
								{
									AgpdItem	*pcsItem	= pThis->m_pcsAgsmItem->GetItemAreaChattingRace(pcsCharacter);
									if (pcsItem)
									{
										INT16	nPacketLength	= 0;
										PVOID	pvPacket		= pThis->MakeAreaChatPacket(pcsBuffer, pcsCharacter, &nPacketLength, pcsItem->m_pcsItemTemplate->m_szName, g_aulAreaChatColor[0]);
										pThis->SendPacketToRace(pcsCharacter, pvPacket, nPacketLength);
									}
								}
								break;

							case AGSMAREACHAT_TYPE_ALL:
								{
									AgpdItem	*pcsItem	= pThis->m_pcsAgsmItem->GetItemAreaChattingAll(pcsCharacter);
									if (pcsItem)
									{
										INT16	nPacketLength	= 0;
										PVOID	pvPacket		= pThis->MakeAreaChatPacket(pcsBuffer, pcsCharacter, &nPacketLength, pcsItem->m_pcsItemTemplate->m_szName, g_aulAreaChatColor[1]);
										pThis->SendPacketToAll(pcsCharacter, pvPacket, nPacketLength);
									}
								}
								break;

							case AGSMAREACHAT_TYPE_ARCHLORD:
								{
									// 아크로드인지 확인한다.
									if(pThis->m_pcsAgpmCharacter->IsArchlord(pcsCharacter->m_szID))
									{
										INT16	nPacketLength	= 0;
										PVOID	pvPacket		= pThis->MakeAreaChatPacket(pcsBuffer, pcsCharacter, &nPacketLength, ServerStr().GetStr(STI_ARCHLORD_ENG_UPPER), g_aulAreaChatColor[1]);
										pThis->SendPacketToAll(pcsCharacter, pvPacket, nPacketLength, TRUE);
									}
								}
								break;
							}
						}
					}
				}

				pThis->m_csReceivePool.Free(pcsBuffer);
				pThis->m_stlQueue.pop();	// 데이터가 존재하면 Queue에서 삭제하고 값을 리턴한다.
			}
		}
	}

	return FALSE;
}

PVOID AgsmAreaChatting::MakeAreaChatPacket(AgsmChatReceiveBuffer *pcsBuffer, AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, CHAR *szHeader, UINT32 ulTextColorRGB)
{
	if (!pcsBuffer || !pcsCharacter || !pnPacketLength)
		return NULL;

	CHAR	szBuffer[1024];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	if (szHeader &&
		szHeader[0])
		sprintf(szBuffer, "[%s] ", szHeader);

	sprintf(szBuffer + strlen(szBuffer), "%s : %s", pcsCharacter->m_szID, (CHAR *) pcsBuffer->szPacketBuffer);

	BOOL	bIsAdminCharacter	= FALSE;

	if (m_pcsAgpmAdmin->IsAdminCharacter(pcsCharacter) &&
		m_pcsAgpmCharacter->IsGM(pcsCharacter))
		bIsAdminCharacter	= TRUE;

	if (strlen(szBuffer) <= 256)
	{
		INT16	nMakePacketLength	= 0;
		PVOID	pvMakePacket		= m_pcsAgpmAreaChatting->MakePacketSendMessage(
																pcsBuffer->lSendCID,
																(INT8) bIsAdminCharacter,
																(INT8) (szHeader && szHeader[0]) ? FALSE : TRUE,
																ulTextColorRGB,
																szBuffer,
																(INT16)strlen(szBuffer),
																&nMakePacketLength);

		if (!pvMakePacket || nMakePacketLength < sizeof(PACKET_HEADER))
			return NULL;

		PVOID pvPacket = m_csSendCircularBuffer.Alloc(sizeof(AgsmChatReceiveBuffer));
		if (!pvPacket)
			return NULL;
		
		memset(pvPacket, 0, sizeof(AgsmChatReceiveBuffer));
		CopyMemory(pvPacket, pvMakePacket, nMakePacketLength);
		(*pnPacketLength) = nMakePacketLength;

		m_pcsAgpmAreaChatting->m_csPacket.FreePacket(pvMakePacket);

		return pvPacket;
	}

	return NULL;
}

PVOID AgsmAreaChatting::MakeAreaChatPacket(BOOL bIsNotify, CHAR *pszSenderName, CHAR *pszServerName, CHAR *pszMessage, UINT16 unMessageLength, INT16 *pnPacketLength, CHAR *szHeader, UINT32 ulTextColorRGB)
{
	if (!pszMessage || !pszMessage[0])
		return NULL;

	CHAR	szBuffer[1024];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	if (szHeader &&
		szHeader[0])
		sprintf(szBuffer, "[%s] ", szHeader);

	sprintf(szBuffer + strlen(szBuffer), "%s-%s : %s", pszServerName, pszSenderName, pszMessage);

	BOOL	bIsAdminCharacter	= bIsNotify;

	if (strlen(szBuffer) <= 256)
	{
		INT16	nMakePacketLength	= 0;
		PVOID	pvMakePacket		= m_pcsAgpmAreaChatting->MakePacketSendMessage(AP_INVALID_CID, (INT8) bIsAdminCharacter, (INT8) FALSE, ulTextColorRGB, szBuffer, (INT16)strlen(szBuffer), &nMakePacketLength);

		if (!pvMakePacket || nMakePacketLength < sizeof(PACKET_HEADER))
			return NULL;

		PVOID pvPacket = m_csSendCircularBuffer.Alloc(sizeof(AgsmChatReceiveBuffer));
		if (!pvPacket)
			return NULL;
		
		memset(pvPacket, 0, sizeof(AgsmChatReceiveBuffer));
		CopyMemory(pvPacket, pvMakePacket, nMakePacketLength);
		(*pnPacketLength) = nMakePacketLength;

		m_pcsAgpmAreaChatting->m_csPacket.FreePacket(pvMakePacket);

		return pvPacket;
	}

	return NULL;
}

BOOL AgsmAreaChatting::SendPacketToRegion(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength)
{
	if (!pcsCharacter || !pvPacket)
		return FALSE;

	if(!pcsCharacter->m_pcsCharacterTemplate)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "SendPacketToRegion() : pcsCharacter->m_pcsCharacterTemplate is null\n");
		AuLogFile_s("LOG\\AgsmAreaChattingError.txt", strCharBuff);
		return FALSE;
	}

	if(m_pagpmBattleGround->IsInBattleGround(pcsCharacter))
		return FALSE;

try
{
	INT16	nRegionIndex	= m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);

	if (nRegionIndex < 0 || nRegionIndex >= AGSMAREACHATTING_MAX_REGIONTEMPLATE)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "SendPacketToRegion() : nRegionIndex : %d, Character Name : %s (%.0f, %.0f)\n", nRegionIndex, pcsCharacter->m_szID, pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);
		AuLogFile_s("LOG\\AgsmAreaChattingError.txt", strCharBuff);
		return FALSE;
	}
	
	INT32	lIndex	= 0;
	UINT32	*pulNID	= (UINT32 *)m_acsRegionAdmin[nRegionIndex].GetObjectSequence(&lIndex);

	while (pulNID)
	{
		SendPacket(pvPacket, nPacketLength, *pulNID);
		pulNID	= (UINT32 *)m_acsRegionAdmin[nRegionIndex].GetObjectSequence(&lIndex);
	}
}
catch(...)
{
	INT16	nRegionIndex	= m_pcsApmMap->GetRegion(pcsCharacter->m_stPos.x, pcsCharacter->m_stPos.z);
	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "SendPacketToRegion() : catch(...) nRegionIndex = %d\n", nRegionIndex);
	AuLogFile_s("LOG\\AgsmAreaChattingError.txt", strCharBuff);
	return FALSE;
}

	return TRUE;
}

BOOL AgsmAreaChatting::SendPacketToRace(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength)
{
	if (!pcsCharacter || !pvPacket)
		return FALSE;

	INT32	lRace	= m_pcsAgpmFactors->GetRace(&pcsCharacter->m_csFactor);
	if (lRace >= 0)
	{
		if (m_pcsAgsmItem->UseItemAreaChattingRace(pcsCharacter))
		{
			INT32	lIndex	= 0;
			UINT32	*pulNID	= (UINT32 *) m_acsRaceAdmin[lRace].GetObjectSequence(&lIndex);

			while (pulNID)
			{
				SendPacket(pvPacket, nPacketLength, *pulNID);

				pulNID	= (UINT32 *) m_acsRaceAdmin[lRace].GetObjectSequence(&lIndex);
			}
		}
	}

	return TRUE;
}

BOOL AgsmAreaChatting::SendPacketToAll(AgpdCharacter *pcsCharacter, PVOID pvPacket, INT16 nPacketLength, BOOL bIsArchlordMessage)
{
	if (!pcsCharacter || !pvPacket)
		return FALSE;

	BOOL bUsedSuccess = FALSE;

	if(bIsArchlordMessage == FALSE)
	{
		bUsedSuccess = m_pcsAgsmItem->UseItemAreaChattingAll(pcsCharacter);
	}

	if (bUsedSuccess || bIsArchlordMessage)
	{
		INT32	lIndex	= 0;
		UINT32	*pulNID	= (UINT32 *) m_csAllPlayerAdmin.GetObjectSequence(&lIndex);

		while (pulNID)
		{
			SendPacket(pvPacket, nPacketLength, *pulNID);

			pulNID	= (UINT32 *) m_csAllPlayerAdmin.GetObjectSequence(&lIndex);
		}
	}

	return TRUE;
}

BOOL AgsmAreaChatting::BroadCastMessage(AgsmChatReceiveBuffer *pcsBuffer)
{
	if (!pcsBuffer)
		return FALSE;

	if (!pcsBuffer->bIsNotify)
	{
		CHAR	*szItemName	= NULL;

		AgsdServer2	*pcsThisServer	= m_pcsAgsmServerManager2->GetThisServer();
		if (!pcsThisServer)
			return FALSE;

		if (strcmp(pcsThisServer->m_szGroupName, pcsBuffer->szServerName) == 0)
		{
			AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(pcsBuffer->szSenderName);
			if (!pcsCharacter)
				return FALSE;

			 AgpdItem	*pcsItem	= m_pcsAgsmItem->GetItemAreaChattingGlobal(pcsCharacter);
			 if (pcsItem)
				 szItemName	= pcsItem->m_pcsItemTemplate->m_szName;

			pcsCharacter->m_Mutex.Release();
		}

		INT16	nPacketLength	= 0;
		PVOID	pvPacket		= MakeAreaChatPacket(FALSE, pcsBuffer->szSenderName, pcsBuffer->szServerName, pcsBuffer->szPacketBuffer, pcsBuffer->lSize, &nPacketLength, szItemName, g_aulAreaChatColor[2]);

		if (pvPacket && nPacketLength > sizeof(PACKET_HEADER))
		{
			INT32	lIndex	= 0;
			UINT32	*pulNID	= (UINT32 *) m_csAllPlayerAdmin.GetObjectSequence(&lIndex);

			while (pulNID)
			{
				SendPacket(pvPacket, nPacketLength, *pulNID);

				pulNID	= (UINT32 *) m_csAllPlayerAdmin.GetObjectSequence(&lIndex);
			}
		}
	}
	else
	{
		return m_pcsAgsmChatting->SendMessageAll(AGPDCHATTING_TYPE_WHOLE_WORLD, AP_INVALID_CID, pcsBuffer->szPacketBuffer, pcsBuffer->lSize);
	}

	return TRUE;
}

AgsdAreaChatting* AgsmAreaChatting::GetAttachCharacterData(AgpdCharacter *pcsCharacter)
{
	if (m_pcsAgpmCharacter)
		return (AgsdAreaChatting *) m_pcsAgpmCharacter->GetAttachedModuleData(m_nIndexAttachCharacterData, (PVOID) pcsCharacter);

	return NULL;
}

BOOL AgsmAreaChatting::CheckColorText(CHAR *szMessage, UINT16 unMessageLength)
{
	if (!szMessage || unMessageLength > 1024)
		return FALSE;

	CHAR	szBuffer[1024 + 1];
	ZeroMemory(szBuffer, sizeof(szBuffer));
	CopyMemory(szBuffer, szMessage, sizeof(CHAR) * unMessageLength);

	if (strstr(szBuffer, "<C") ||
		strstr(szBuffer, "<H") ||
		strstr(szBuffer, "<F"))
		return FALSE;

	return TRUE;
}

// 2006.09.29. steeple
BOOL AgsmAreaChatting::CBRecvArchlordMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAreaChatting	*pThis			= (AgsmAreaChatting *)	pClass;
	AgpdChatData		*pstChatData	= (AgpdChatData *)		pData;

	if (pstChatData->lMessageLength > 1024)
		return FALSE;

	AgsmChatReceiveBuffer	*pstBuffer	= (AgsmChatReceiveBuffer *) pThis->m_csReceivePool.Alloc();
	if (!pstBuffer)
		return FALSE;

	ZeroMemory(pstBuffer, sizeof(AgsmChatReceiveBuffer));

	pstBuffer->lSendCID		= pstChatData->pcsSenderBase->m_lID;
	pstBuffer->lSize		= pstChatData->lMessageLength;
	CopyMemory(pstBuffer->szPacketBuffer, pstChatData->szMessage, pstChatData->lMessageLength);
	pstBuffer->eChatType	= AGSMAREACHAT_TYPE_ARCHLORD;

	AuAutoLock Lock(pThis->m_Mutex);
	if (Lock.Result())
		pThis->m_stlQueue.push(pstBuffer);

	return TRUE;
}
