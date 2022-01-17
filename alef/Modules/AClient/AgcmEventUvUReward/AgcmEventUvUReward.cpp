//	AgcmEventUvUReward module
//		- union vs union battle 시 나오는 유골에 관한 보상을 담당하는 모듈이다.
/////////////////////////////////////////////////////////////////////////////

#include "AgcmEventUvUReward.h"

AgcmEventUvUReward::AgcmEventUvUReward()
{
	SetModuleName("AgcmEventUvUReward");
}

AgcmEventUvUReward::~AgcmEventUvUReward()
{
}

BOOL AgcmEventUvUReward::OnAddModule()
{
	m_pcsAgpmEventUvUReward		= (AgpmEventUvUReward *)	GetModule("AgpmEventUvUReward");

	m_pcsAgcmCharacter			= (AgcmCharacter *)			GetModule("AgcmCharacter");
	m_pcsAgcmEventManager		= (AgcmEventManager *)		GetModule("AgcmEventManager");

	if (!m_pcsAgpmEventUvUReward ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmEventManager)
		return FALSE;

	if (!m_pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_UVU_REWARD, CBEvent, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmEventUvUReward::OnInit()
{
	return TRUE;
}

BOOL AgcmEventUvUReward::OnDestroy()
{
	return TRUE;
}

BOOL AgcmEventUvUReward::CBEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventUvUReward	*pThis			= (AgcmEventUvUReward *)	pClass;
	ApdEvent			*pcsEvent		= (ApdEvent *)				pData;
	ApBase				*pcsGenerator	= (ApBase *)				pCustData;

	// pcsGenerator (자기 자신이다) 에게 유골을 보여달라고 한다.
	//
	//
	//
	//
	//
	//
	//

	return TRUE;
}

BOOL AgcmEventUvUReward::RequestReward(ApBase *pcsEventBase, AgpdCharacter *pcsOperator, AgpdItem *pcsItemSkull)
{
	if (!pcsEventBase || !pcsOperator || !pcsItemSkull)
		return FALSE;

	ApdEvent	*pcsEvent = m_pcsApmEventManager->GetEvent(pcsEventBase, APDEVENT_FUNCTION_UVU_REWARD);
	if (!pcsEvent)
		return FALSE;

	// pcsItemSkull 이 올바른 유골인지 먼저 확인한다.
	if (!m_pcsAgpmEventUvUReward->IsValidSkull(pcsOperator, pcsItemSkull))
	{
		// 이건 유골이 아니던가 아님.. 같은 유니온 유골이던가..
		// 암튼 이넘은 잘못된 유골이다.
		//
		//
		//		메시지를 뿌려주던 뭘하던 한다.
		//
		//
	}

	// 유골이 맞는거다.
	// 서버로 정보를 보낸다.

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmEventUvUReward->MakePacket(pcsEvent, pcsOperator->m_lID, pcsItemSkull->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmEventUvUReward->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}