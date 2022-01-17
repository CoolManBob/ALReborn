/*===================================================================

	AgsmEventRefinery.cpp

===================================================================*/

#include "AgsmEventRefinery.h"


/************************************************************/
/*		The Implementation of AgsmEventRefinery class		*/
/************************************************************/
//
AgsmEventRefinery::AgsmEventRefinery()
	{
	SetModuleName("AgsmEventRefinery");

	m_pAgpmEventRefinery = NULL;
	m_pAgsmCharacter = NULL;
	}

AgsmEventRefinery::~AgsmEventRefinery()
	{
	}


//	ApModule inherited
//===============================================
//
BOOL AgsmEventRefinery::OnAddModule()
	{
	m_pAgpmEventRefinery= (AgpmEventRefinery *) GetModule("AgpmEventRefinery");
	m_pAgsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");

	if (!m_pAgpmEventRefinery || !m_pAgsmCharacter)
		return FALSE;

	if (!m_pAgpmEventRefinery->SetCallbackRequest(CBEventRequest, this))
		return FALSE;
	
	return TRUE;
	}

BOOL AgsmEventRefinery::CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventRefinery *pThis = (AgsmEventRefinery *)pClass;
	ApdEvent *pApdEvent = (ApdEvent *) pData;
	AgpdCharacter *pAgpdCharacter = (AgpdCharacter *) pCustData;

	INT16 nPacketLength = 0;
	PVOID pvPacket = pThis->m_pAgpmEventRefinery->MakePacketEventGrant(pApdEvent, pAgpdCharacter->m_lID, &nPacketLength);

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pAgsmCharacter->GetCharDPNID(pAgpdCharacter));
	pThis->m_pAgpmEventRefinery->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}