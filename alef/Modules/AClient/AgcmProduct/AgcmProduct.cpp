/*===================================================================

	AgcmProduct.cpp

===================================================================*/

#include "AgcmProduct.h"


/****************************************************/
/*		The Implementation of AgcmProduct class		*/
/****************************************************/
//
AgcmProduct::AgcmProduct()
	{
	SetModuleName("AgcmProduct");
	SetModuleType(APMODULE_TYPE_CLIENT);

	EnableIdle(FALSE);

	m_pAgpmProduct = NULL;
	m_pAgpmCharacter = NULL;
	}

AgcmProduct::~AgcmProduct()
	{
	}


//	ApModule inherited
//========================================
//
BOOL AgcmProduct::OnAddModule()
	{
	m_pAgpmProduct = (AgpmProduct *) GetModule("AgpmProduct");
	m_pAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");

	if (!m_pAgpmProduct || !m_pAgpmCharacter)
		return FALSE;

	return TRUE;
	}

BOOL AgcmProduct::OnInit()
	{
	return TRUE;
	}

BOOL AgcmProduct::OnDestroy()
	{
	return TRUE;
	}

BOOL AgcmProduct::SendPacketCompose(INT32 lCID, INT32 lSkillID, INT32 lComposeID, INT32 lReceipeID)
	{
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMPRODUCT_PACKET_OPERATION_COMPOSE;

	PVOID pvPacket = m_pAgpmProduct->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMPRODUCT_PACKET_TYPE,
														   &cOperation,
														   &lCID,
														   &lSkillID,
														   &lComposeID,
														   &lReceipeID,
														   NULL,
														   NULL,
														   NULL
														   );
	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pAgpmProduct->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}