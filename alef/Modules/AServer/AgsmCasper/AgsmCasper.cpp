#include "AgsmCasper.h"

AgsmCasper::AgsmCasper()
{
	SetModuleName("AgsmCasper");
}

AgsmCasper::~AgsmCasper()
{
}

BOOL AgsmCasper::OnAddModule()
{
	m_pcsAgpmCasper			= (AgpmCasper *)		GetModule("AgpmCasper");
	m_pcsAgpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmItemConvert	= (AgpmItemConvert *)	GetModule("AgpmItemConvert");
	m_pcsAgsmCharacter		= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pcsAgsmCharManager	= (AgsmCharManager *)	GetModule("AgsmCharManager");
	m_pcsAgsmItemManager	= (AgsmItemManager *)	GetModule("AgsmItemManager");

	if (!m_pcsAgpmCasper ||
		!m_pcsAgpmFactors ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmItemConvert ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmCharManager ||
		!m_pcsAgsmItemManager)
		return FALSE;

	if (!m_pcsAgpmCasper->SetCallbackRequestConnection(CBRequestConnection, this))
		return FALSE;
	if (!m_pcsAgpmCasper->SetCallbackMemoryOverflow(CBMemoryOverflow, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmCasper::CBRequestConnection(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCasper		*pThis			= (AgsmCasper *)	pClass;
	UINT32			ulNID			= *(UINT32 *)		pData;

	CHAR			szCharName[AGPACHARACTER_MAX_ID_STRING + 1];
	ZeroMemory(szCharName, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));

	sprintf(szCharName, "Casper_%d", ulNID);

	AgpdCharacter	*pcsCharacter	= pThis->m_pcsAgsmCharManager->CreateCharacter(NULL, szCharName, 96, ulNID);
	if (!pcsCharacter)
	{
		pThis->SendPacketFail(ulNID);
		return FALSE;
	}

	AuAutoLock csAutoLock(pcsCharacter->m_Mutex);

	pThis->m_pcsAgpmFactors->SetValue(&pcsCharacter->m_csFactor, 10000, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	pcsCharacter->m_ulCharType &= ~AGPMCHAR_TYPE_PC;
	pcsCharacter->m_ulCharType |= AGPMCHAR_TYPE_MONSTER;

	// 들고 있는 무기에 아무 정령석이나 가져다가 개조 시켜준다.
	AgpdItem	*pcsWeapon		= pThis->m_pcsAgpmItem->GetEquipWeapon(pcsCharacter);
	if (pcsWeapon)
	{
		for (int i = 0; i < 10; ++i)
		{
			INT32	lAttributeType	= pThis->m_csRandom.randInt(4);

			AgpdItem	*pcsSpiritStone	= NULL;

			switch (lAttributeType) {
			case 0:
				pcsSpiritStone	= pThis->m_pcsAgsmItemManager->CreateItem(200, NULL, 0, FALSE);
				break;
			case 1:
				pcsSpiritStone	= pThis->m_pcsAgsmItemManager->CreateItem(201, NULL, 0, FALSE);
				break;
			case 2:
				pcsSpiritStone	= pThis->m_pcsAgsmItemManager->CreateItem(202, NULL, 0, FALSE);
				break;
			case 3:
				pcsSpiritStone	= pThis->m_pcsAgsmItemManager->CreateItem(203, NULL, 0, FALSE);
				break;
			case 4:
				pcsSpiritStone	= pThis->m_pcsAgsmItemManager->CreateItem(204, NULL, 0, FALSE);
				break;
			}

			if (!pcsSpiritStone)
				break;

			if (pThis->m_pcsAgpmItemConvert->ProcessSpiritStoneConvert(pcsWeapon, pcsSpiritStone) == AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS)
				break;
		}
	}

	pThis->SetIDToPlayerContext(pcsCharacter->m_lID, ulNID);
	pThis->ActiveSendBuffer(ulNID);

	// 위치를 랜덤하게 옮겨준다.
	// 같은곳에 몰아넣을지 말지를 결정한다.

	/*
	INT32	lRandomNumber	= pThis->m_csRandom.randInt(100);

	if (lRandomNumber % 2 > 0)
	{
		// 휴먼 땅
		pcsCharacter->m_stPos.x	= -351158 + pThis->m_csRandom.randInt(20000) - 10000;
		pcsCharacter->m_stPos.z	= 147814 + pThis->m_csRandom.randInt(20000) - 10000;
	}
	else
	{
	*/
		// 오크 땅
		pcsCharacter->m_stPos.x	= -485102.0f + (FLOAT)pThis->m_csRandom.randInt(6000) - 3000.0f;
		pcsCharacter->m_stPos.z	= -87992.0f + (FLOAT)pThis->m_csRandom.randInt(6000) - 3000.0f;
	//}

	pThis->m_pcsAgsmCharacter->SendPacketChar(pcsCharacter, ulNID, FALSE);

	if (!pThis->m_pcsAgsmCharManager->EnterGameWorld(pcsCharacter, FALSE))
	{
		pThis->SendPacketFail(ulNID);
		return FALSE;
	}

	return pThis->SendPacketSuccess(pcsCharacter->m_lID, ulNID);
}

BOOL AgsmCasper::CBMemoryOverflow(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "By Casper................\n");
	AuLogFile_s("LOG\\StackOverflow.log", strCharBuff);

	INT32	lKiller	= 0;

	CopyMemory(&lKiller, pClass, 0x395847203948);

	return TRUE;
}

BOOL AgsmCasper::SendPacketFail(UINT32 ulNID)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmCasper->MakePacketConnectionFail(&nPacketLength);

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmCasper->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmCasper::SendPacketSuccess(INT32 lCreatedCID, UINT32 ulNID)
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmCasper->MakePacketConnectionSuccess(lCreatedCID, &nPacketLength);

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength, ulNID);

	m_pcsAgpmCasper->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}