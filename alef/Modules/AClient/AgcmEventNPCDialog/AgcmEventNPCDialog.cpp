#include "AgcmEventNPCDialog.h"
#include "AgcmEventManager.h"
#include "AgpmEventNPCDialog.h"
#include "AgpdCharacter.h"
#include "AgcmCharacter.h"


AgcmEventNPCDialog::AgcmEventNPCDialog( void )
{
	SetModuleName("AgcmEventNPCDialog");
}

AgcmEventNPCDialog::~AgcmEventNPCDialog( void )
{
}

BOOL AgcmEventNPCDialog::OnAddModule( void )
{
	AgcmEventManager* pcmEventManager = ( AgcmEventManager* )GetModule( "AgcmEventManager" );
	if( !pcmEventManager ) return FALSE;
	if( !pcmEventManager->RegisterEventCallback( APDEVENT_FUNCTION_NPCDAILOG, CBRequestNPCDialog, this ) ) return FALSE;

	AgpmEventNPCDialog* ppmEventNPCDialog = ( AgpmEventNPCDialog* )GetModule( "AgpmEventNPCDialog" );
	if( !ppmEventNPCDialog ) return FALSE;
	if( !ppmEventNPCDialog->SetCallbackGrant( CBReceiveGrant, this ) ) return FALSE;

	m_ExNPC.SetModule( this );
	m_ExNPC.OnRegisterCallBack();
	return TRUE;
}

BOOL AgcmEventNPCDialog::SetCallbackAriseNPCDialogEvent( ApModuleDefaultCallBack pfCallback, void* pClass )
{
	return SetCallback( 0, pfCallback, pClass );
}

BOOL AgcmEventNPCDialog::CBRequestNPCDialog( void* pData, void* pClass, void* pCustData )
{
	if( !pData || !pClass || !pCustData ) return FALSE;

	ApdEvent* ppdEvent = ( ApdEvent* )pData;
	AgcmEventNPCDialog* pThis = ( AgcmEventNPCDialog* )pClass;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )pThis->GetModule( "AgpmCharacter" );
	AgpmEventNPCDialog* ppmEventNPCDialog = ( AgpmEventNPCDialog* )pThis->GetModule( "AgpmEventNPCDialog" );
	if( !pcmCharacter || !ppmCharacter || !ppmEventNPCDialog ) return FALSE;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return FALSE;

	BOOL bResult = ppmCharacter->IsActionBlockCondition( ppdCharacter );
	if( bResult ) return FALSE;

	INT16 nPacketLength	= 0;
	void* pPacket = ppmEventNPCDialog->MakePacketEventRequest( ppdEvent, ppdCharacter->m_lID, &nPacketLength );
	if( !pPacket || nPacketLength < 1 )	return FALSE;

	bResult	= pThis->SendPacket( pPacket, nPacketLength );
	ppmEventNPCDialog->m_csPacket.FreePacket( pPacket );

	return bResult;
}

BOOL AgcmEventNPCDialog::CBReceiveGrant( void* pData, void* pClass, void* pCustData )
{
	if( !pData || !pClass || !pCustData ) return FALSE;

	AgcmEventNPCDialog* pThis = ( AgcmEventNPCDialog* )pClass;
	AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pCustData;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );
	if( !pcmCharacter || pcmCharacter->GetSelfCharacter() != ppdCharacter ) return FALSE;

	pThis->EnumCallback( 0, pData, ppdCharacter );
	return TRUE;
}
