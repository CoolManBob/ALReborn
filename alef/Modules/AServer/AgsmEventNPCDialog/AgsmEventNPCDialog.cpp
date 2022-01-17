#include "AgsmEventNPCDialog.h"
#include "AgsmNpcManager.h"

AgsmEventNPCDialog::AgsmEventNPCDialog()
{
	SetModuleName("AgsmEventNPCDialog");

	m_pcsAgsmNpcManager = NULL;
}

AgsmEventNPCDialog::~AgsmEventNPCDialog()
{
}

BOOL AgsmEventNPCDialog::OnAddModule()
{
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmEventNPCDialog		= (AgpmEventNPCDialog *)	GetModule("AgpmEventNPCDialog");
	m_pcsAgsmCharacter			= (AgsmCharacter *)			GetModule("AgsmCharacter");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmEventNPCDialog ||
		!m_pcsAgsmCharacter)
		return FALSE;

	if (!m_pcsAgpmEventNPCDialog->SetCallbackRequest(CBNPCDialogEventRequest, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmEventNPCDialog::OnInit()
{
	m_pcsAgsmNpcManager = (AgsmNpcManager*)GetModule("AgsmNpcManager");
	if(!m_pcsAgsmNpcManager)
		return FALSE;

	return TRUE;
}

BOOL AgsmEventNPCDialog::CBNPCDialogEventRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmEventNPCDialog		*pThis				= (AgsmEventNPCDialog *)	pClass;
	ApdEvent				*pcsEvent			= (ApdEvent *)				pData;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pCustData;


	AgpdCharacter* pcsNpc = (AgpdCharacter*)pcsEvent->m_pcsSource;
	if(pcsNpc)
	{
		if(pThis->m_pcsAgsmNpcManager->GetFileName(pcsNpc->m_nNPCID))
		{
			pThis->m_pcsAgsmNpcManager->OnEventRequest(pcsCharacter, pcsNpc);

			return TRUE;
		}
	}

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmEventNPCDialog->MakePacketEventGrant(pcsEvent, pcsCharacter->m_lID, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter));

	pThis->m_pcsAgpmEventNPCDialog->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}