#include "AgsmEventCharCustomize.h"

AgsmEventCharCustomize::AgsmEventCharCustomize()
{
	SetModuleName("AgsmEventCharCustomize");
}

AgsmEventCharCustomize::~AgsmEventCharCustomize()
{
}

BOOL AgsmEventCharCustomize::OnAddModule()
{
	m_pcsAgpmCharacter			= (AgpmCharacter *)				GetModule("AgpmCharacter");
	m_pcsAgpmEventCharCustomize	= (AgpmEventCharCustomize *)	GetModule("AgpmEventCharCustomize");
	m_pcsAgsmCharacter			= (AgsmCharacter *)				GetModule("AgsmCharacter");

	m_pcsAgpmLog				= (AgpmLog *)					GetModule("AgpmLog");
	
	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventCharCustomize ||
		!m_pcsAgsmCharacter)
		return FALSE;

	if (!m_pcsAgpmEventCharCustomize->SetCallbackGrant(CBEventGrant, this))
		return FALSE;

	if (!m_pcsAgpmEventCharCustomize->SetCallbackAfterUpdate(CBAfterUpdate, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventCharCustomize::CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventCharCustomize	*pThis	= (AgsmEventCharCustomize *)	pClass;
	AgpdCharacter *pcsCharacter	= (AgpdCharacter *)	pData;
	ApdEvent	*pcsEvent	= (ApdEvent *)	pCustData;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventCharCustomize->MakePacketResponseEvent(pcsCharacter->m_lID, pcsEvent, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	pThis->m_pcsAgpmEventCharCustomize->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmEventCharCustomize->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmEventCharCustomize::CBAfterUpdate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventCharCustomize	*pThis	= (AgsmEventCharCustomize *)	pClass;
	AgpdCharacter *pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer = (PVOID *) pCustData;

	if (pThis->m_pcsAgpmLog)
	{
		INT32 lFace = PtrToInt(ppvBuffer[0]);
		INT32 lHair = PtrToInt(ppvBuffer[1]);
		INT32 lItemTid = PtrToInt(ppvBuffer[2]);
		INT32 lCost = PtrToInt(ppvBuffer[3]);
	
		CHAR szDesc[AGPDLOG_MAX_DESCRIPTION+1] = {0, };
		_stprintf(szDesc, _T("Face=%d, Hair=%d"), lFace, lHair);
	
		AgsdCharacter* pAgsdCharacter = pThis->m_pcsAgsmCharacter->GetADCharacter(pcsCharacter);
		if(!pAgsdCharacter)
			return FALSE;
		
		INT32 lLevel = pThis->m_pcsAgpmCharacter->GetLevel(pcsCharacter);
		INT64 llExp = pThis->m_pcsAgpmCharacter->GetExp(pcsCharacter);
		
		pThis->m_pcsAgpmLog->WriteLog_CharCustomize(0,
													&pAgsdCharacter->m_strIPAddress[0],
													pAgsdCharacter->m_szAccountID,
													pAgsdCharacter->m_szServerName,
													pcsCharacter->m_szID,
													((AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate)->m_lID,
													lLevel,
													llExp,
													pcsCharacter->m_llMoney,
													pcsCharacter->m_llBankMoney,
													szDesc,
													lCost
													);
	}
	
	return TRUE;
}