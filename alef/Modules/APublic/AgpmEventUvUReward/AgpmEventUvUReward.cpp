//	AgpmEventUvUReward.cpp
//		- Union vs Union 의 전투에서 얻는 보상과 관련된 모듈
///////////////////////////////////////////////////////////////

#include "AgpmEventUvUReward.h"

AgpmEventUvUReward::AgpmEventUvUReward()
{
	SetModuleName("AgpmEventUvUReward");

	SetPacketType(AGPMEVENT_UVUREWARD_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_PACKET,			1,			// event base packet
							AUTYPE_INT32,			1,			// character id (operator)
							AUTYPE_INT32,			1,			// item(skull) id
							AUTYPE_END,				0
							);
}

AgpmEventUvUReward::~AgpmEventUvUReward()
{
}

BOOL AgpmEventUvUReward::OnAddModule()
{
	m_pcsAgpmFactors			= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)			GetModule("AgpmItem");

	m_pcsApmEventManager		= (ApmEventManager *)	GetModule("ApmEventManager");

	if (!m_pcsAgpmFactors || !m_pcsAgpmCharacter || !m_pcsAgpmItem || !m_pcsApmEventManager)
		return FALSE;

	if (!m_pcsApmEventManager->RegisterEvent(APDEVENT_FUNCTION_UVU_REWARD, ConAgpdUvUReward, DesAgpdUvUReward, NULL, CBStreamWriteEvent, CBStreamReadEvent, this))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventUvUReward::OnInit()
{
	return TRUE;
}

BOOL AgpmEventUvUReward::OnDestroy()
{
	return TRUE;
}

BOOL AgpmEventUvUReward::CBStreamWriteEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pstEvent	= (ApdEvent *)				pData;
	AgpmEventUvUReward	*pThis		= (AgpmEventUvUReward *)	pClass;
	ApModuleStream		*pStream	= (ApModuleStream *)		pCustData;

	if (!pstEvent->m_pvData)
		return FALSE;

	AgpdUvUReward		*pcsUvUReward	= (AgpdUvUReward *) pstEvent->m_pvData;

	if (!pStream->WriteValue(AGPMEVENT_UVU_REWARD_STREAM_START, 0))
		return FALSE;

	if (!pStream->WriteValue(AGPMEVENT_UVU_REWARD_STREAM_TYPE, (INT32) pcsUvUReward->m_eRewardType))
		return FALSE;

	if (!pStream->WriteValue(AGPMEVENT_UVU_REWARD_STREAM_END, 0))
		return FALSE;

	return TRUE;
}

BOOL AgpmEventUvUReward::CBStreamReadEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pstEvent	= (ApdEvent *)				pData;
	AgpmEventUvUReward	*pThis		= (AgpmEventUvUReward *)	pClass;
	ApModuleStream		*pStream	= (ApModuleStream *)		pCustData;

	if (!pstEvent->m_pvData)
		return FALSE;

	AgpdUvUReward		*pcsUvUReward	= (AgpdUvUReward *)	pstEvent->m_pvData;

	if (!pStream->ReadNextValue())
		return FALSE;

	const CHAR	*szName = pStream->GetValueName();
	if (strncmp(szName, AGPMEVENT_UVU_REWARD_STREAM_START, strlen(AGPMEVENT_UVU_REWARD_STREAM_START)) != 0)
		return FALSE;

	while (pStream->ReadNextValue())
	{
		szName = pStream->GetValueName();

		if (strncmp(szName, AGPMEVENT_UVU_REWARD_STREAM_TYPE, strlen(AGPMEVENT_UVU_REWARD_STREAM_TYPE)) == 0)
		{
			INT32	lRewardType	= 0;
			pStream->GetValue(&lRewardType);

			pcsUvUReward->m_eRewardType = (eAgpmEventUvURewardType) lRewardType;
		}
		else if (strncmp(szName, AGPMEVENT_UVU_REWARD_STREAM_END, strlen(AGPMEVENT_UVU_REWARD_STREAM_END)) == 0)
		{
			break;
		}
	}

	return TRUE;
}

BOOL AgpmEventUvUReward::ConAgpdUvUReward(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventUvUReward	*pThis		= (AgpmEventUvUReward *)	pClass;
	ApdEvent			*pstEvent	= (ApdEvent *)				pData;

	pstEvent->m_pvData	= GlobalAlloc(GMEM_FIXED, sizeof(AgpdUvUReward));
	if (!pstEvent->m_pvData)
		return FALSE;

	ZeroMemory(pstEvent->m_pvData, sizeof(AgpdUvUReward));

	((AgpdUvUReward *) pstEvent->m_pvData)->m_eRewardType = AGPMEVENT_UVU_REWARD_NOT_SET;

	return TRUE;
}

BOOL AgpmEventUvUReward::DesAgpdUvUReward(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgpmEventUvUReward	*pThis		= (AgpmEventUvUReward *)	pClass;
	ApdEvent			*pstEvent	= (ApdEvent *)				pData;

	if (pstEvent->m_pvData)
		GlobalFree(pstEvent->m_pvData);

	return TRUE;
}

BOOL AgpmEventUvUReward::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, BOOL bReceivedFromServer)
{
	if (!pvPacket || nSize < 1)
		return FALSE;

	PVOID	pvEventBasePacket	= NULL;
	INT32	lCID				= AP_INVALID_CID;
	INT32	lIID				= AP_INVALID_IID;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&pvEventBasePacket,
						&lCID,
						&lIID);

	if (!pvEventBasePacket || lCID == AP_INVALID_CID || lIID == AP_INVALID_IID)
		return FALSE;

	ApdEvent	*pcsEvent		= m_pcsApmEventManager->GetEventFromBasePacket(pvEventBasePacket);
	if (!pcsEvent || !pcsEvent->m_pvData)
		return FALSE;

	AgpdUvUReward	*pcsUvUReward	= (AgpdUvUReward *) pcsEvent->m_pvData;

	AgpdCharacter	*pcsCharacter	= m_pcsAgpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	AgpdItem		*pcsItemSkull	= m_pcsAgpmItem->GetItem(lIID);
	if (!pcsItemSkull)
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	if (!IsValidSkull(pcsCharacter, pcsItemSkull))
	{
		pcsCharacter->m_Mutex.Release();
		return FALSE;
	}

	switch (pcsUvUReward->m_eRewardType) {
	case AGPMEVENT_UVU_REWARD_TYPE_EXP:
		{
			EnumCallback(AGPMEVENT_UVU_REWARD_CB_EXP, pcsCharacter, pcsItemSkull);
		}
		break;

	case AGPMEVENT_UVU_REWARD_TYPE_UNION_RANK:
		{
			EnumCallback(AGPMEVENT_UVU_REWARD_CB_UNION_RANK, pcsCharacter, pcsItemSkull);
		}
		break;
	}

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

PVOID AgpmEventUvUReward::MakePacket(ApdEvent *pcsEvent, INT32 lCID, INT32 lIID, INT16 *pnPacketLength)
{
	if (!pcsEvent || lCID == AP_INVALID_CID || lIID == AP_INVALID_IID)
		return NULL;

	PVOID	pvPacketEvent	= m_pcsApmEventManager->MakeBasePacket(pcsEvent);
	if (!pvPacketEvent)
		return NULL;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMEVENT_UVUREWARD_PACKET_TYPE,
											 pvPacketEvent,
											 &lCID,
											 &lIID);

	m_csPacket.FreePacket(pvPacketEvent);

	return pvPacket;
}

BOOL AgpmEventUvUReward::IsValidSkull(AgpdCharacter *pcsCharacter, AgpdItem *pcsSkull)
{
	if (!pcsCharacter || !pcsSkull)
		return FALSE;

	// 먼저 pcsSkull 이 진짜 skull 아템인지 알아본다.
	if (((AgpdItemTemplate *) pcsSkull->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_OTHER ||
		((AgpdItemTemplateOther *) pcsSkull->m_pcsItemTemplate)->m_eOtherItemType != AGPMITEM_OTHER_TYPE_SKULL)
		return FALSE;

	// pcsCharacter와 pcsSkull을 떨군 캐릭터와 적대 유니온 관계인지 살펴본다.
	//		(같은 유니온 유골인경우 이건 사용할 수 없다)
	INT32	lUnionSkull	= m_pcsAgpmCharacter->GetUnion(m_pcsAgpmFactors->GetRace(&pcsSkull->m_csFactor));
	INT32	lUnionChar	= m_pcsAgpmCharacter->GetUnion(pcsCharacter);

	if (lUnionSkull != lUnionChar)
		return FALSE;

	return TRUE;
}

BOOL AgpmEventUvUReward::SetCallbackRewardExp(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_UVU_REWARD_CB_EXP, pfCallback, pClass);
}

BOOL AgpmEventUvUReward::SetCallbackRewardUnionRank(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPMEVENT_UVU_REWARD_CB_UNION_RANK, pfCallback, pClass);
}