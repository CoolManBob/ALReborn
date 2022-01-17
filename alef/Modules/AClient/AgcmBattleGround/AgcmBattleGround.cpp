#include "AgcmBattleGround.h"
#include "AgppBattleGround.h"
#include "AgpmBattleGround.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgcmChatting2.h"
#include "AgcmUISiegeWar.h"
#include "AuStrTable.h"

AgcmBattleGround::AgcmBattleGround()
{
	SetModuleName( "AgcmBattleGround" );
	SetModuleType( APMODULE_TYPE_CLIENT );

	SetPacketType(AGPM_BATTLEGROUND_PACKET_TYPE);

	EnableIdle( FALSE );

	m_pcsAgpmBattleGround	= NULL;
	m_pcsAgpmCharacter		= NULL;
	m_pcsAgcmCharacter		= NULL;
	m_pcsAgcmChatting		= NULL;
	m_pcsAgcmUIManager2		= NULL;
	m_pcsAgcmUISiegeWar		= NULL;
}

AgcmBattleGround::~AgcmBattleGround()
{
}

BOOL	AgcmBattleGround::OnAddModule()
{
	m_pcsAgpmBattleGround	= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pcsAgpmCharacter		= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgcmCharacter		= (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgpmFactors		= (AgpmFactors*)GetModule("AgpmFactors");
	m_pcsAgcmChatting		= (AgcmChatting2*)GetModule("AgcmChatting2");
	m_pcsAgcmUIManager2		= (AgcmUIManager2*)GetModule("AgcmUIManager2");
	m_pcsAgcmUISiegeWar		= (AgcmUISiegeWar*)GetModule("AgcmUISiegeWar");

	if( !m_pcsAgpmBattleGround	||
		!m_pcsAgpmCharacter		||
		!m_pcsAgcmCharacter		||
		!m_pcsAgpmFactors		||
		!m_pcsAgcmChatting		||	
		!m_pcsAgcmUIManager2	||
		!m_pcsAgcmUISiegeWar )
		return FALSE;

	//public module callback setting
	//m_pcsAgpmBattleGround->SetCallbackSystemMessage( CBSystemMessage, this );

	return TRUE;
}

BOOL	AgcmBattleGround::OnInit()
{
	return TRUE;
}

BOOL	AgcmBattleGround::OnDestroy()
{
	return TRUE;
}

BOOL	AgcmBattleGround::OnIdle( UINT32 ulClockCount )
{
	//m_pcsAgcmUISiegeWar->SetRemainTime( startClock, ulClockCount );
	return TRUE;
}

BOOL AgcmBattleGround::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	PACKET_BATTLEGROUND* pPacket = (PACKET_BATTLEGROUND*)pvPacket;

	switch(pPacket->pcOperation)
	{
	case AGPM_BATTLEGROUND_PACKET_NOTICE:
		{
			BattleGroundNotice(pPacket);
		}
		break;
	}

	return TRUE;
}

void AgcmBattleGround::BattleGroundNotice(PACKET_BATTLEGROUND* pvPacket)
{
	PACKET_BATTLEGROUND_NOTICE* pPacketBattleGroundNotice = (PACKET_BATTLEGROUND_NOTICE*)pvPacket;

	switch( pPacketBattleGroundNotice->lNoticeType )
	{

	case 4:
		if( pPacketBattleGroundNotice->bEvent )
		{
			EndEventBattleGround();
		}
		else
		{
			EndBattleGround();
		}

		m_pcsAgcmUISiegeWar->CloseSiegeWarInfo();
		break;

	case 5:
		if( pPacketBattleGroundNotice->strNotice )
			_EpicZoneEffectText( pPacketBattleGroundNotice->strNotice , pPacketBattleGroundNotice->dwColor );

		break;

	case 2:
		if( pPacketBattleGroundNotice->bEvent )
		{
			StartEventBattleGround();
		}
		else
		{
			StartBattleGround();
		}

	default:
		CHAR szMessage[MAX_NOTICE_LEN];
		memset( szMessage, 0, sizeof(szMessage) );
		memcpy( szMessage, pPacketBattleGroundNotice->strNotice, MAX_NOTICE_LEN );

		m_pcsAgcmUISiegeWar->OpenSiegeWarInfo();
		m_pcsAgcmUISiegeWar->SetInfoEdit( szMessage, pPacketBattleGroundNotice->dwColor );
		break;
	}
}

BOOL	AgcmBattleGround::IsBattleGournd( AgpdCharacter* pChar )
{
	return pChar ? m_pcsAgpmBattleGround->IsInBattleGround( pChar ) : FALSE;
}

BOOL	AgcmBattleGround::IsMyBattleGround()
{
	return IsBattleGournd( (AgpdCharacter*)m_pcsAgcmCharacter->GetSelfCharacter() );
}

void	AgcmBattleGround::StartBattleGround()
{
	_BattleGroundEffectText( ClientStr().GetStr(STI_START_BATTLEGROUND) );
}

void	AgcmBattleGround::EndBattleGround()
{
	_BattleGroundEffectText( ClientStr().GetStr(STI_END_BATTLEGROUND) );
}

void	AgcmBattleGround::StartEventBattleGround()
{
	_BattleGroundEffectText( ClientStr().GetStr(STI_START_EVENT_BATTLEGROUND) );
}

void	AgcmBattleGround::EndEventBattleGround()
{
	_BattleGroundEffectText( ClientStr().GetStr(STI_END_EVENT_BATTLEGROUND) );
}

void	AgcmBattleGround::_BattleGroundEffectText( const char* szMessage )
{
	INT32 lOffsetY = INT32( m_pcsAgcmUIManager2->m_csEffectWindow.h * 0.19f );
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectText( (char*)szMessage, 3, lOffsetY, FALSE, 0xff00ff00, 1.4f, 1000 * 15 );
}

void	AgcmBattleGround::_EpicZoneEffectText( const char* szMessage , DWORD dwColor )
{
	INT32 lOffsetY = INT32( m_pcsAgcmUIManager2->m_csEffectWindow.h * 0.19f );
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectText( (char*)szMessage, 3, lOffsetY, FALSE, dwColor , 1.1f, 1000 * 15 );
}

BOOL	AgcmBattleGround::IsOtherRace( AgpdCharacter* pOther )
{
	if( !pOther )		return FALSE;

	AgpdCharacter* pMy = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pMy )			return FALSE;
	if( pOther == pMy )	return FALSE;
	if( !m_pcsAgpmCharacter->IsPC( pOther ) )					return FALSE;
	if( !IsBattleGournd( pMy ) || !IsBattleGournd( pOther ) )	return FALSE;

	INT32 nOther = m_pcsAgpmFactors->GetRace( &pOther->m_csFactor );
	INT32 nMy = m_pcsAgpmFactors->GetRace( &pMy->m_csFactor );

	return nOther != nMy ? TRUE : FALSE;
}