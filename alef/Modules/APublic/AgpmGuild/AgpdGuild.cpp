#include "AgpdGuild.h"

//----------------------------- Global -----------------------------
INT32 g_nGuildBattleTime[eGuildBattleTimeModeMax]	= {	5, 10, 30, 60, 90, 120 };

INT32 g_nBattleTotalMan[eGuildBattleTotalModeMax]	= { 5, 10, 15, 20, 25, 50 };
INT32 g_nBattlePrivateMan[eGuildBattlePrivateModeMax]	= { 3, 5, 7, 9 };
//INT32 g_nBattleTotalMan[eGuildBattleTotalModeMax]	= { 1, 2, 3, 4, 5, 6 };
//INT32 g_nBattlePrivateMan[eGuildBattlePrivateModeMax]	= { 1, 2, 3, 4 };

//----------------------------- AgpdGuildBattle -----------------------------
void	AgpdGuildBattle::Init()
{
	m_eType			= eGuildBattleMax;
	m_ulPerson		= 0;

	m_ulAcceptTime	= 0;
	m_ulReadyTime	= 0;
	m_ulStartTime	= 0;
	m_ulDuration	= 0;

	m_ulCurrentTime	= 0;

	m_lMyScore		= 0;
	m_lEnemyScore	= 0;

	m_lMyUpScore	= 0;
	m_lEnemyUpScore	= 0;
	
	m_lMyUpPoint	= 0;
	m_lEnemyUpPoint	= 0;

	m_cResult		= 0;

	m_ulRound		= 0;
	
	memset(m_szEnemyGuildID, 0, sizeof(m_szEnemyGuildID));
	memset(m_szEnemyGuildMasterID, 0, sizeof(m_szEnemyGuildMasterID));
}

void	AgpdGuildBattle::Start( UINT32 lTime )
{
	m_ulCurrentTime		= lTime;
	m_lMyScore			= 0;
	m_lEnemyScore		= 0;
	m_lMyUpScore		= 0;
	m_lEnemyUpScore		= 0;
	m_cResult			= 0;		// 0:None, 1:Win, 2:Draw, 3:Lose
	m_ulRound			= -1;		// 0라운드 유저를 위해 초기값을 -1로
}

void	AgpdGuildBattle::End( UINT32 lTime )
{
	m_ulCurrentTime	= lTime;
	
	//switch( m_eType )
	//{
	//	case eGuildBattlePointMatch:	//많은 점수확보
	//	{
	//	}
	//	case eGuildBattlePKMatch:		//상대방 많이 PK
	//	{
	//	}
	//	case eGuildBattleTotalSurvive:	//모두전멸
	//	{
	//	}
	//	case eGuildBattlePrivateSurvive://다이다이
	//	{
	//	}
	//	case eGuildBattleDeadMatch:		//길마 많이 죽이기
	//	{
	//	}
	//}
}

BOOL	AgpdGuildBattle::Update( UINT32 lTime )
{
	m_ulCurrentTime	= lTime;

	return FALSE;
}

INT32	AgpdGuildBattle::GetGuildPoint()
{
	const UINT32 MINUTE = 60; // 초단위
	const INT32 g_nGBResultPoint[AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW+1]	= { 0, 5, 2, 0, 5, -20 };
	
	UINT32	nGBTimePoint = 1;
	UINT32	ulTotalBattleTime = m_ulCurrentTime - m_ulStartTime; // 초단위
	
	if(ulTotalBattleTime > m_ulDuration)
		ulTotalBattleTime = m_ulDuration;
		
	switch(m_cResult)
	{
		case AGPMGUILD_BATTLE_RESULT_WIN:
		case AGPMGUILD_BATTLE_RESULT_DRAW:
		case AGPMGUILD_BATTLE_RESULT_LOSE:
			{
				if(ulTotalBattleTime < MINUTE*11)
					nGBTimePoint = 1;
				else if(ulTotalBattleTime < MINUTE*31)
					nGBTimePoint = 3;
				else if(ulTotalBattleTime < MINUTE*61)
					nGBTimePoint = 6;
				else if(ulTotalBattleTime < MINUTE*91)
					nGBTimePoint = 9;
				else if(ulTotalBattleTime < MINUTE*121)
					nGBTimePoint = 12;
				else
					nGBTimePoint = 1;
			} break;
		case AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW:
			{
				if(ulTotalBattleTime < MINUTE*20)
					nGBTimePoint = 0;
				else if(ulTotalBattleTime < MINUTE*30)
					nGBTimePoint = 2;
				else if(ulTotalBattleTime < MINUTE*60)
					nGBTimePoint = 3;
				else if(ulTotalBattleTime < MINUTE*90)
					nGBTimePoint = 6;
				else if(ulTotalBattleTime < MINUTE*120)
					nGBTimePoint = 9;
				else
					nGBTimePoint = 1;	
			} break;
		case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
			{
				nGBTimePoint = 1;
			} break;
	}
		
	m_lMyUpPoint = g_nGBResultPoint[m_cResult] * nGBTimePoint;
	
	return m_lMyUpPoint;
}

//--------------- AgpdGuild ---------------------
AgpdGuildMember* AgpdGuild::GetMember( CHAR* szID )
{
	if( !szID || !szID[0] )		return NULL;
	
	AgpdGuildMember** ppMember = (AgpdGuildMember**)m_pMemberList->GetObject(szID);
	return (ppMember) ? (*ppMember) : NULL;
}

AgpdGuildMember* AgpdGuild::GetMaster()
{
	AgpdGuildMember** ppMember = (AgpdGuildMember**)m_pMemberList->GetObject( m_szMasterID );
	return (ppMember) ? (*ppMember) : NULL;	
}

AgpdGuildMember* AgpdGuild::GetRoundMember( UINT32 ulRound )
{
	//ASSERT( eGuildBattlePrivateSurvive == m_csCurrentBattleInfo.m_eType );

	AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		 ppMember;
		 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )				continue;
		if( !(*ppMember)->m_bBattle )	continue;
		
		if( (*ppMember)->m_ulSequence == ulRound )
			return (*ppMember);
	}

	return NULL;
}

UINT32	AgpdGuild::GetBattleMemberCount()
{
	switch( m_csCurrentBattleInfo.m_eType )
	{
	case eGuildBattlePointMatch:
	case eGuildBattlePKMatch:
	case eGuildBattleDeadMatch:
		return m_pMemberList->GetObjectCount();
	case eGuildBattleTotalSurvive:
	case eGuildBattlePrivateSurvive:
		{
			UINT32	lCount = 0;
			AgpdGuildMember** ppMember = NULL;
			INT32 lIndex = 0;
			for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
				 ppMember;
				 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
			{
				if( !*ppMember )		continue;
				if( (*ppMember)->m_bBattle )
					++lCount;
			}
			return lCount;
		}
	}

	return 0;
}

AgpdGuildMemberBattleInfo* AgpdGuild::GetBattleMemberPack()
{
	UINT32 lCount = GetBattleMemberCount();
	if( !lCount )	return NULL;

	AgpdGuildMemberBattleInfo* pInfo = new AgpdGuildMemberBattleInfo[lCount];
	if( !pInfo )	return NULL;

	UINT32	ulCurrentCount = 0;
	eGuildBattleType eType = m_csCurrentBattleInfo.m_eType;
	AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		 ppMember;
		 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )					continue;
		if( eGuildBattleTotalSurvive == eType || eGuildBattlePrivateSurvive == eType )
			if( !(*ppMember)->m_bBattle )	continue;

		strcpy( pInfo[ulCurrentCount].m_szID, (*ppMember)->m_szID );
		pInfo[ulCurrentCount].m_bBattle		= TRUE;
		pInfo[ulCurrentCount].m_ulScore		= (*ppMember)->m_ulScore;
		pInfo[ulCurrentCount].m_ulKill		= (*ppMember)->m_ulKill;
		pInfo[ulCurrentCount].m_ulDeath		= (*ppMember)->m_ulDeath;
		pInfo[ulCurrentCount].m_ulSequence	= (*ppMember)->m_ulSequence;
		ulCurrentCount++;
	}

	return pInfo;
}

UINT32	AgpdGuild::GetStateMemberCount( AgpmGuildMemberStatus eState )
{
	UINT32	lCount = 0;
	AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		 ppMember;
		 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )		continue;
		if( (*ppMember)->m_cStatus == eState )
			++lCount;
	}
	return lCount;
}

void	AgpdGuild::BattleStart( UINT32 lTime )
{
	m_cStatus = AGPMGUILD_STATUS_BATTLE;

	m_csCurrentBattleInfo.Start( lTime );

	//시작했을때는 Battle의 기본정보를 Reset
	AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		 ppMember;
		 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )		continue;

		(*ppMember)->m_ulScore	= 0;
		(*ppMember)->m_ulKill	= 0;
		(*ppMember)->m_ulDeath	= 0;
	}
}

void	AgpdGuild::BattleEnd( UINT32 lTime )
{
	m_cStatus = AGPMGUILD_STATUS_NONE;

	m_csCurrentBattleInfo.End( lTime );

	/*AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		 ppMember;
		 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )		continue;

		(*ppMember)->m_bBattle		= FALSE;
		(*ppMember)->m_ulSequence	= -1;
	}*/
}

void AgpdGuild::BattleMemberInit()
{
	AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		ppMember;
		ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )		continue;

		(*ppMember)->m_bBattle		= FALSE;
		(*ppMember)->m_ulSequence	= -1;
	}
}

BOOL	AgpdGuild::BattleUpdate( UINT32 lTime )
{
	return m_csCurrentBattleInfo.Update( lTime );
}

BOOL	AgpdGuild::BattleResult( AgpdGuild* pEnemy )
{
	if( !pEnemy )		return FALSE;

	switch( m_csCurrentBattleInfo.m_eType )
	{
	case eGuildBattlePointMatch:
		if( m_csCurrentBattleInfo.m_lMyScore > pEnemy->m_csCurrentBattleInfo.m_lMyScore )
			BattleResultSetting( this, pEnemy );
		else if( m_csCurrentBattleInfo.m_lMyScore < pEnemy->m_csCurrentBattleInfo.m_lMyScore )
			BattleResultSetting( pEnemy, this );
		else
			BattleResultSetting( this, pEnemy, TRUE );
		break;
	case eGuildBattlePKMatch:
		if( GetKillAll() > pEnemy->GetKillAll() )
			BattleResultSetting( this, pEnemy );
		else if( GetKillAll() < pEnemy->GetKillAll() )
			BattleResultSetting( pEnemy, this );
		else
			BattleResultSetting( this, pEnemy, TRUE );
		break;
	case eGuildBattleTotalSurvive:
		if( m_csCurrentBattleInfo.m_lMyScore > pEnemy->m_csCurrentBattleInfo.m_lMyScore )
			BattleResultSetting( this, pEnemy );
		else if( m_csCurrentBattleInfo.m_lMyScore < pEnemy->m_csCurrentBattleInfo.m_lMyScore )
			BattleResultSetting( pEnemy, this );
		else
			BattleResultSetting( this, pEnemy, TRUE );
		break;
	case eGuildBattlePrivateSurvive:
		if( GetKillAll() > pEnemy->GetKillAll() )
			BattleResultSetting( this, pEnemy );
		else if( GetKillAll() < pEnemy->GetKillAll() )
			BattleResultSetting( pEnemy, this );
		else	//비기는 일은 없을텐뎅.. ㅎㅎ
			BattleResultSetting( this, pEnemy, TRUE );
		break;
	case eGuildBattleDeadMatch:
		{
			AgpdGuildMember* pMaster1 = GetMaster();
			AgpdGuildMember* pMaster2 = pEnemy->GetMaster();
			if( !pMaster1 || !pMaster2 )	return FALSE;

			if( pMaster1->m_ulDeath < pMaster2->m_ulDeath )
				BattleResultSetting( this, pEnemy );
			else if( pMaster1->m_ulDeath > pMaster2->m_ulDeath )
				BattleResultSetting( pEnemy, this );
			else
				BattleResultSetting( this, pEnemy, TRUE );
		}
		break;
	}

	return TRUE;
}

void	AgpdGuild::BattleResultSetting( AgpdGuild* pWinner, AgpdGuild* pLoser, BOOL bDraw )
{
	if( bDraw )
	{
		pWinner->m_csCurrentBattleInfo.m_cResult	= AGPMGUILD_BATTLE_RESULT_DRAW;
		pLoser->m_csCurrentBattleInfo.m_cResult		= AGPMGUILD_BATTLE_RESULT_DRAW;
		pWinner->m_lDraw++;
		pLoser->m_lDraw++;
	}
	else
	{
		pWinner->m_csCurrentBattleInfo.m_cResult	= AGPMGUILD_BATTLE_RESULT_WIN;
		pLoser->m_csCurrentBattleInfo.m_cResult		= AGPMGUILD_BATTLE_RESULT_LOSE;
		pWinner->m_lWin++;
		pLoser->m_lLose++;
	}
}

void	AgpdGuild::SetBattle( CHAR* szID, BOOL bBattle )
{
	AgpdGuildMember* pMember = GetMember( szID );
	if( pMember )
		pMember->m_bBattle = bBattle;
}

BOOL	AgpdGuild::IsBattle( CHAR* szID )
{
	AgpdGuildMember* pMember = GetMember( szID );
	if( !pMember )	return FALSE;
	
	return IsBattle(pMember);
}

BOOL	AgpdGuild::IsBattle( AgpdGuildMember* pMember )
{
	if(!pMember) return FALSE;
	
	switch( m_csCurrentBattleInfo.m_eType )
	{
		case eGuildBattlePointMatch:
		case eGuildBattlePKMatch:
		case eGuildBattleDeadMatch:
			return TRUE;
		case eGuildBattleTotalSurvive:
		case eGuildBattlePrivateSurvive:
			return pMember->m_bBattle;
	}

	return FALSE;
}

BOOL	AgpdGuild::IsOnBattle( CHAR* szID )
{
	AgpdGuildMember* pMember = GetMember( szID );
	if( !pMember )	return FALSE;

	return IsOnBattle(pMember);
}

BOOL	AgpdGuild::IsOnBattle( AgpdGuildMember* pMember )
{
	if(!pMember) return FALSE;
	
	switch( m_csCurrentBattleInfo.m_eType )
	{
		case eGuildBattlePointMatch:
		case eGuildBattlePKMatch:
		case eGuildBattleDeadMatch:
			return TRUE;
		case eGuildBattleTotalSurvive:
			return pMember->m_bBattle;
		case eGuildBattlePrivateSurvive:
			//return ( pMember == GetCurRoundMember() );
			return (pMember->m_ulSequence == m_csCurrentBattleInfo.m_ulRound);
	}

	return FALSE;
}


void	AgpdGuild::UpdateScore( CHAR* szID, UINT32 lScore )
{
	AgpdGuildMember* pMember = GetMember( szID );
	if( pMember && IsBattle( pMember ) )
		pMember->m_ulScore += lScore;
}

void	AgpdGuild::UpdateKill( CHAR* szID )
{
	AgpdGuildMember* pMember = GetMember( szID );
	if( pMember && IsBattle( pMember ) )
		++pMember->m_ulKill;
}

void	AgpdGuild::UpdateDeath( CHAR* szID )
{
	AgpdGuildMember* pMember = GetMember( szID );
	if( pMember && IsBattle( pMember ) )
		++pMember->m_ulDeath;
}

UINT32	AgpdGuild::GetScoreAll()
{
	UINT32 ulScore = 0;
	AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		 ppMember;
		 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )				continue;
		if( !IsBattle( *ppMember ) )	continue;

		ulScore += (*ppMember)->m_ulScore;
	}

	return ulScore;
}

UINT32	AgpdGuild::GetKillAll()
{
	UINT32 ulKill = 0;
	AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		 ppMember;
		 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )				continue;
		if( !IsBattle( *ppMember ) )	continue;

		ulKill += (*ppMember)->m_ulKill;
	}

	return ulKill;
}

UINT32	AgpdGuild::GetDeathAll()
{
	UINT32 ulDeath = 0;
	AgpdGuildMember** ppMember = NULL;
	INT32 lIndex = 0;
	for( ppMember = (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex);
		 ppMember;
		 ppMember= (AgpdGuildMember**)m_pMemberList->GetObjectSequence(&lIndex) )
	{
		if( !*ppMember )				continue;
		if( !IsBattle( *ppMember ) )	continue;

		ulDeath += (*ppMember)->m_ulDeath;
	}

	return ulDeath;
}

BOOL AgpdGuild::IsPrivateSurvivePlayer( AgpdGuild* pEnemyGuild, CHAR* szPlay1, CHAR* szPlay2 )
{
	if( !pEnemyGuild || !szPlay1 || !szPlay2 )							return FALSE;
	if( eGuildBattlePrivateSurvive != m_csCurrentBattleInfo.m_eType )	return TRUE;	//항상성공이징..

	AgpdGuildMember* pMember = GetCurRoundMember();
	AgpdGuildMember* pEnemyMember = pEnemyGuild->GetCurRoundMember();
	if( !pMember || !pEnemyMember )				return FALSE;

	if( !strcmp( pMember->m_szID, szPlay1 ) && !strcmp( pEnemyMember->m_szID, szPlay2 ) )
		return TRUE;

	if( !strcmp( pMember->m_szID, szPlay2 ) && !strcmp( pEnemyMember->m_szID, szPlay1 ) )
		return TRUE;

	return  FALSE;
}

VOID AgpdGuild::SetMemberRankChange( CHAR* szMemberID , INT32 nRank )
{
	AgpdGuildMember*	pMember	=	GetMember( szMemberID );		
	if( !pMember )		return;

	// 길드원 랭크 변경
	pMember->m_lRank	=	nRank;


	// 길드마스터와 길드부마스터는 AgpdGuild에서도 변경해줘야 한다
	switch( nRank )
	{
	case AGPMGUILD_MEMBER_RANK_MASTER:		SetMasterID		( szMemberID ); SetSubMasterID( "" );	break;
	case AGPMGUILD_MEMBER_RANK_SUBMASTER:	SetSubMasterID	( szMemberID );							break;
	case AGPMGUILD_MEMBER_RANK_NORMAL:		
		if( strcmp( m_szSubMasterID , szMemberID ) == 0 )	SetSubMasterID( "" );
		break;
	}

}
