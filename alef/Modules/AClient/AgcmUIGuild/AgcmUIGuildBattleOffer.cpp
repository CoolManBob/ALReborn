#include "AgcmUIGuildBattleOffer.h"
#include "AgcmUIGuild.h"
#include "AgcmGuild.h"

char* g_szGuildBattleTime[eGuildBattleTimeModeMax]	= {	"5", "10", "30", "60", "90", "120" };
char* g_szBattleTotalMan[eGuildBattleTotalModeMax]	= { "5", "10", "15", "20", "25", "50" };
char* g_szBattlePrivateMan[eGuildBattlePrivateModeMax]	= { "3", "5", "7", "9" };
char* g_szBattleTypeArray[eGuildBattleMax]			= { "포인트매치", "넉아웃매치", "치프데스매치", "서바이벌(정예)", "서바이벌(개인)" };

AgcmUIGuildBattleOffer::AgcmUIGuildBattleOffer()
{
	m_nBattleType	= 0;
	m_nBattleTime	= 0;
	m_nBattlePerson	= 0;

	for( int i=0; i<5; ++i )
		memset( m_szBattleTypeArray[i], 0, 128+1 );

	memset( m_szBattleType, 0, 128+1 );
	memset( m_szBattleTime, 0, AGPMGUILD_MAX_GUILD_ID_LENGTH+1 );
	memset( m_szBattlePerson, 0, 128+1 );
}

AgcmUIGuildBattleOffer::~AgcmUIGuildBattleOffer()
{
}

BOOL	AgcmUIGuildBattleOffer::Open( INT32 nType, UINT32 nTime, UINT32 nPerson, INT32 lEvent, BOOL bReceive )
{
	SetString();

	SetBattleType( nType );
	SetBattleTime( nTime, bReceive );
	SetBattlePerson( nPerson, bReceive );

	if( !bReceive )
		m_pcsAgcmUIManager2->ThrowEvent( m_lEventUpdateComboBattlePerson );

	m_pcsAgcmUIManager2->ThrowEvent( lEvent );

	return TRUE;
}

BOOL	AgcmUIGuildBattleOffer::AddModule( AgcmUIManager2* pUIMgr, AgcmUIGuild* pUIGuild )
{
	m_pcsAgcmUIManager2 = pUIMgr;
	m_pcsAgcmUIGuild	= pUIGuild;

	m_pudBattleType = m_pcsAgcmUIManager2->AddUserData( "OP_GuildBattleType", g_szBattleTypeArray, sizeof(CHAR *), eGuildBattleMax, AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pudBattleType )			return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleType", CBGuildBattleType, 0 ) )	return FALSE;

	m_pudBattleTime = m_pcsAgcmUIManager2->AddUserData( "OP_GuildBattleTime", g_szGuildBattleTime, sizeof(CHAR *), eGuildBattleTimeModeMax, AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pudBattleTime )			return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleTime", CBGuildBattleTime, 0 ) )	return FALSE;

	m_pudBattlePerson = m_pcsAgcmUIManager2->AddUserData( "OP_GuildBattleMan", g_szBattleTotalMan, sizeof(CHAR *), eGuildBattleTotalModeMax, AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pudBattlePerson )		return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleMan", CBGuildBattleMan, 0 ) )	return FALSE;

	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_GuildBattleRequest", CBBattleRequest, 0 ) )		return FALSE;
	if( !m_pcsAgcmUIManager2->AddFunction( this, "OP_UpdateComboGuildBattlePerson", CBUpdateComboBattlePerson, 1, "combo control" ) )		return FALSE;
	m_lEventUpdateComboBattlePerson = m_pcsAgcmUIManager2->AddEvent( "UpdateComboGuildBattlePerson" );

	m_lEventBattleRequestEnd = m_pcsAgcmUIManager2->AddEvent("BattleRequestEnd", CBMessageBattleRequestEnd, this )	;

	Init( pUIMgr, "Guild_GBattle" );

	return TRUE;
}

void	AgcmUIGuildBattleOffer::Update()
{
}


BOOL	AgcmUIGuildBattleOffer::ComboClear()
{
	if( m_pudBattleType )		m_pudBattleType->m_lSelectedIndex = 0;
	if( m_pudBattleTime )		m_pudBattleTime->m_lSelectedIndex = 0;
	if( m_pudBattlePerson )		m_pudBattlePerson->m_lSelectedIndex = 0;

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pudBattleType );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pudBattleTime );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pudBattlePerson );

	return TRUE;
}

void	AgcmUIGuildBattleOffer::BattleStringClear()
{
	memset( m_szBattleType, 0, sizeof( m_szBattleType ) );
	memset( m_szBattleTime, 0, sizeof( m_szBattleTime ) );
	memset( m_szBattlePerson, 0, sizeof( m_nBattlePerson ) );
}

void	AgcmUIGuildBattleOffer::SetBattleType( INT32 nType )
{	
	m_nBattleType = nType;
	if( nType != eGuildBattleMax )
		strcpy( m_szBattleType, m_szBattleTypeArray[ m_nBattleType ] );
}

void	AgcmUIGuildBattleOffer::SetBattleTime( UINT32 nType, BOOL bReceive )
{
	if( bReceive )
	{
		m_nBattleTime = 0;	//5분짜리로 그냥 넣어주자..

		for( int i=0; i<eGuildBattleTimeModeMax; ++i )
		{
			if( nType == g_nGuildBattleTime[i] * 60 )
				m_nBattleTime = i;
		}
	}
	else
		m_nBattleTime = nType;

	strcpy( m_szBattleTime, g_szGuildBattleTime[ m_nBattleTime ] );
}

UINT32	AgcmUIGuildBattleOffer::GetBattlePersonSize()
{
	switch( m_nBattleType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
		return 0;
	case eGuildBattleTotalSurvive:
		return g_nBattleTotalMan[m_nBattlePerson];
	case eGuildBattlePrivateSurvive:
		return g_nBattlePrivateMan[m_nBattlePerson];
	}

	return 0;
}

void	AgcmUIGuildBattleOffer::SetBattlePerson( UINT32 nType, BOOL bReceive )
{
	if( bReceive )
	{
		m_nBattlePerson = 0;

		int nMax = eGuildBattleTotalSurvive == m_nBattleType ? eGuildBattleTotalModeMax : eGuildBattlePrivateModeMax;
		INT32* pArray = eGuildBattleTotalSurvive == m_nBattleType ? g_nBattleTotalMan : g_nBattlePrivateMan;
		for( int i=0; i<nMax; ++i )
		{
			if( nType == pArray[i] )
				m_nBattlePerson = i;
		}
	}
	else
	{
		m_nBattlePerson = nType;
	}

	CHAR* szCurPerson = eGuildBattleTotalSurvive == m_nBattleType ? g_szBattleTotalMan[ m_nBattlePerson ] : g_szBattlePrivateMan[ m_nBattlePerson ];
	strcpy( m_szBattlePerson, szCurPerson );
}

BOOL AgcmUIGuildBattleOffer::CBGuildBattleType(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleOffer* pThis = (AgcmUIGuildBattleOffer *) pClass;
	pThis->m_nBattleType = pThis->m_pudBattleType->m_lSelectedIndex;

	//이곳에서 콤보박스들의 상태를 점검한다
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventUpdateComboBattlePerson );

	return TRUE;
}

BOOL AgcmUIGuildBattleOffer::CBGuildBattleTime(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleOffer* pThis = (AgcmUIGuildBattleOffer *) pClass;
	pThis->m_nBattleTime = pThis->m_pudBattleTime->m_lSelectedIndex;
	return TRUE;
}

BOOL AgcmUIGuildBattleOffer::CBGuildBattleMan(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleOffer* pThis = (AgcmUIGuildBattleOffer *) pClass;
	pThis->m_nBattlePerson = pThis->m_pudBattlePerson->m_lSelectedIndex;

	return TRUE;
}

BOOL AgcmUIGuildBattleOffer::CBBattleRequest(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	//각각의 셋팅된 정보들을 넣어주자고요 ^^
	AgcmUIGuildBattleOffer* pThis	= (AgcmUIGuildBattleOffer*)pClass;
	AgcmUIGuild* pUIGuild	= pThis->m_pcsAgcmUIGuild;
	AgcmGuild* pGuild = pUIGuild->m_pcsAgcmGuild;
	AgpmGuild* pPublicGuild = pUIGuild->m_pcsAgpmGuild;

	if( !pThis || !pUIGuild )	return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pUIGuild->m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )			return FALSE;

	char* szMyGuildID = pGuild->GetSelfGuildID();

	// 길드에 가입중이 아니면 길드 가입 신청을 한다.
	if( !szMyGuildID )
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pUIGuild->m_lEventGuildBattleTimeUIClose );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pUIGuild->m_lEventConfirmAllowJoin );
		return TRUE;
	}

	// 마스터가 아니면 나간다.
	if( !pPublicGuild->IsMaster( szMyGuildID, pcsCharacter->m_szID ) )
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pUIGuild->m_lEventGuildBattleTimeUIClose );
		return TRUE;
	}

	AgpdGuild* pGuildData = pPublicGuild->GetGuildLock( szMyGuildID );
	if( pGuildData )
	{
		BOOL bState = pPublicGuild->IsBattleStatus( pGuildData );
		pGuildData->m_Mutex.Release();

		if( bState )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pUIGuild->m_lEventGuildBattleTimeUIClose );
			SystemMessage.ProcessSystemMessage( "길드전중에 길드전 신청을 할수 없습니다" );
			return TRUE;
		}
	}

	if( !strcmp( szMyGuildID, pUIGuild->m_szBattleEnemyGuildID ) )
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pUIGuild->m_lEventGuildBattleTimeUIClose );
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pUIGuild->m_lEventGuildBattleFailRequestSelfGuild );
		return TRUE;
	}

	switch( pThis->GetBattleType() )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
		pThis->SendRequest();
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_pcsAgcmUIGuild->m_lEventGuildBattleTimeUIClose );
		break;
	case eGuildBattleTotalSurvive:
	case eGuildBattlePrivateSurvive:
		{
			pThis->m_pcsAgcmUIGuild->GetUIGuildBattleMember().SetOpenType( AgcmUIGuildBattleMember::eOpenRequest );

			AgpdGuild* pGuild = pPublicGuild->GetGuildLock( szMyGuildID );
			if( pGuild )
			{
				//접속되어 있는 맴버와 경기할수 있는 맴버의 수를 확인해야 하는뎅..
				INT32	lIndex = 0;
				INT32	lJoinCount = 0;
				AgpdGuildMember** ppcsMember = NULL;
				for( ppcsMember = (AgpdGuildMember**)pGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
					 ppcsMember = (AgpdGuildMember**)pGuild->m_pMemberList->GetObjectSequence(&lIndex) )
				{
					if( !*ppcsMember )		break;
					if(  AGPMGUILD_MEMBER_STATUS_ONLINE == (*ppcsMember)->m_cStatus )
						lJoinCount++;
				}
				
				if( lJoinCount < (INT32)pThis->GetBattlePersonSize() )
				{
					char* szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleOffer_NotEnough" );
					SystemMessage.ProcessSystemMessage( szMessage ? szMessage : "접속한 길드원이 적습니다" );
				}
				else
				{
					pThis->m_pcsAgcmUIGuild->GetUIGuildBattleMember().Open( pGuild );
					pThis->Close();
				}
				
				pGuild->m_Mutex.Release();
			}
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUIGuildBattleOffer::CBUpdateComboBattlePerson(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if( !pClass || !pData1 )	return FALSE;

	AgcmUIGuildBattleOffer* pThis = (AgcmUIGuildBattleOffer*)pClass;
	AgcdUIControl* pcsComboControl = (AgcdUIControl *)	pData1;

	if( AcUIBase::TYPE_COMBO != pcsComboControl->m_lType )
		return FALSE;

	AcUICombo* pCombo = (AcUICombo*)pcsComboControl->m_pcsBase;
	if( !pCombo )	return FALSE;

	pCombo->ClearAllString();
	if( eGuildBattleTotalSurvive == pThis->m_nBattleType )
	{
		pThis->m_pudBattlePerson->m_stUserData.m_pvData	= g_szBattleTotalMan;
		pThis->m_pudBattlePerson->m_stUserData.m_lCount	= eGuildBattleTotalModeMax;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pudBattlePerson );
	}
	else if( eGuildBattlePrivateSurvive == pThis->m_nBattleType )
	{
		pThis->m_pudBattlePerson->m_stUserData.m_pvData	= g_szBattlePrivateMan;
		pThis->m_pudBattlePerson->m_stUserData.m_lCount	= eGuildBattlePrivateModeMax;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pudBattlePerson );
	}

	return TRUE;
}

BOOL AgcmUIGuildBattleOffer::CBIsBattlePerson(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	int nType = ((AgcmUIGuildBattleOffer*)pClass)->m_pudBattlePerson->m_lSelectedIndex;
	switch( nType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
	case eGuildBattleMax:
		return FALSE;
	case eGuildBattleTotalSurvive:
	case eGuildBattlePrivateSurvive:
		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIGuildBattleOffer::CBMessageBattleRequestEnd(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	AgcmUIGuildBattleOffer* pOffer = (AgcmUIGuildBattleOffer*)pClass;
	if( !pOffer )	return TRUE;

	if( lTrueCancel )
	{
		//종료를 날려준다..
		pOffer->SendCancelRequest();
		pOffer->Clear();
	}
	else
	{

	}

	return TRUE;
}

void	AgcmUIGuildBattleOffer::Clear()
{
	ComboClear();
	SetBattleType( 0 );
	SetBattleTime( 0 );
	SetBattlePerson( 0 );
	if( m_pcsAgcmUIGuild )
		memset( m_pcsAgcmUIGuild->m_szBattleEnemyGuildID, 0, AGPMGUILD_MAX_GUILD_ID_LENGTH+1 );
}

BOOL	AgcmUIGuildBattleOffer::IsGuildBattle()
{
	if( m_pcsAgcmUIGuild->m_szBattleEnemyGuildID && strlen( m_pcsAgcmUIGuild->m_szBattleEnemyGuildID ) )
		return TRUE;

	return FALSE;
}

BOOL AgcmUIGuildBattleOffer::IsPossiblePerson( INT32 lType, UINT32 ulPerson )
{
	switch( lType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
		return TRUE;
	case eGuildBattleTotalSurvive:
		{
			for( int i=0; i<eGuildBattleTotalModeMax; ++i )
				if( g_nBattleTotalMan[i] == ulPerson )
					return TRUE;
			return FALSE;
		}
	case eGuildBattlePrivateSurvive:
		{
			for( int i=0; i<eGuildBattlePrivateModeMax; ++i )
				if( g_nBattlePrivateMan[i] == ulPerson )
					return TRUE;
			return FALSE;
		}
	}

	return FALSE;
}

BOOL	AgcmUIGuildBattleOffer::SendRequest()
{
	return m_pcsAgcmUIGuild->m_pcsAgcmGuild->SendBattleRequest( m_pcsAgcmUIGuild->m_szBattleEnemyGuildID, 
		GetBattleType(), GetBattleTimeSize(), GetBattlePersonSize() );
}

BOOL	AgcmUIGuildBattleOffer::SendAccept()
{
	return m_pcsAgcmUIGuild->m_pcsAgcmGuild->SendBattleAccept( m_pcsAgcmUIGuild->m_szBattleEnemyGuildID, 
		GetBattleType(), GetBattleTimeSize(), GetBattlePersonSize() );
}

BOOL	AgcmUIGuildBattleOffer::SendReject()
{
	return m_pcsAgcmUIGuild->m_pcsAgcmGuild->SendBattleReject( m_pcsAgcmUIGuild->m_szBattleEnemyGuildID );
}

BOOL	AgcmUIGuildBattleOffer::SendCancelRequest()
{
	return m_pcsAgcmUIGuild->m_pcsAgcmGuild->SendBattleCancelRequest( m_pcsAgcmUIGuild->m_szBattleEnemyGuildID );
}

void	AgcmUIGuildBattleOffer::SetString()
{
	char* szMessage = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleMode_Point" );
	strcpy( m_szBattleTypeArray[eGuildBattlePointMatch], szMessage ? szMessage : "포인트매치" );

	szMessage = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleMode_KnockoutPoint" );
	strcpy( m_szBattleTypeArray[eGuildBattlePKMatch], szMessage ? szMessage : "넉아웃매치" );

	szMessage = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleMode_ChiefDeath" );
	strcpy( m_szBattleTypeArray[eGuildBattleDeadMatch], szMessage ? szMessage : "치프데스매치" );

	szMessage = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleMode_TotalSurvival" );
	strcpy( m_szBattleTypeArray[eGuildBattleTotalSurvive], szMessage ? szMessage : "서바이벌(정예)" );

	szMessage = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleMode_PrivateSurvival" );
	strcpy( m_szBattleTypeArray[eGuildBattlePrivateSurvive], szMessage ? szMessage : "서바이벌(개인)" );

	for( int i=0; i<5; ++i )
		g_szBattleTypeArray[i] = m_szBattleTypeArray[i];

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pudBattleType );
}