// AgcmQuest.cpp: implementation of the AgcmQuest class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmQuest.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmQuest::AgcmQuest()
{
	SetModuleName("AgcmQuest");
	m_pcsAgpmQuest = NULL;
}

AgcmQuest::~AgcmQuest()
{

}

BOOL AgcmQuest::OnInit()
{
	return TRUE;
}

BOOL AgcmQuest::OnAddModule()
{
	m_pcsAgpmQuest = (AgpmQuest*)GetModule("AgpmQuest");

	if (!m_pcsAgpmQuest) return FALSE;

	return TRUE;
}

BOOL AgcmQuest::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgcmQuest::OnDestroy()
{
	return TRUE;
}

BOOL AgcmQuest::SendPacketRequireQuest(INT32 lQuestTID, ApdEvent* pEvent, INT32 lCID)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketRequireQuest(&nPacketLength, pEvent, lCID, lQuestTID);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult =  SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bResult;	
}

BOOL AgcmQuest::SendPacketCompleteQuest(INT32 lQuestTID, ApdEvent* pEvent, INT32 lCID)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketQuestComplete(&nPacketLength, pEvent, lCID, lQuestTID);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmQuest::SendPacketQuestCancel(INT32 lQuestTID, INT32 lCID)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketQuestCancel(&nPacketLength, lCID, lQuestTID);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmQuest::SendPacketCheckPoint(INT32 lQuestTID, ApdEvent* pEvent, INT32 lCID, INT32 lCheckPointIndex)
{
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmQuest->MakePacketRequireCheckPoint(&nPacketLength, lCID, pEvent, lQuestTID, lCheckPointIndex);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmQuest->m_csPacket.FreePacket(pvPacket);

	return bResult;
}