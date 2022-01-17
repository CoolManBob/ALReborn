#include "AgcmUIGuildBattleResult.h"
#include "AgcmUIGuild.h"

UINT32 GuildBattleUnit::ulBattleMode = eGuildBattlePrivateSurvive;

//-------------------------- GuildBattleResultInfo ------------------------------
void GuildBattleResultInfo::SetInfo( AgpdGuild* pGuild )
{
	if( !pGuild )	return;

	eResult = (AgpmGuildBattleResult)pGuild->m_csCurrentBattleInfo.m_cResult;
	cTeam[0].SetInfo( pGuild->m_szID, pGuild->m_szMasterID, pGuild->m_csCurrentBattleInfo.m_lMyScore );
	cTeam[1].SetInfo( pGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, pGuild->m_csCurrentBattleInfo.m_lEnemyScore );
}

void	GuildBattleResultInfo::SetMemberInfo( char* szName, UINT32 ulKO, UINT32 ulKill, UINT32 ulDeath, UINT32 ulSequence, BOOL bMy )
{
	cTeam[bMy ? 0 : 1].Update( szName, ulKO, ulKill, ulDeath, ulSequence );
	//cTeam[bMy ? 0 : 1].Sort();
}

void	GuildBattleResultInfo::SetMemberListInfo( INT32 lMyCount, AgpdGuildMemberBattleInfo* pMyInfo, INT32 lEnemyCount, AgpdGuildMemberBattleInfo* pEnemyInfo )
{
	for( int i=0; i<lMyCount; ++i )
		cTeam[0].Update( pMyInfo[i].m_szID, pMyInfo[i].m_ulScore, pMyInfo[i].m_ulKill, pMyInfo[i].m_ulDeath, pMyInfo[i].m_ulSequence );
	
	for( int i=0; i<lEnemyCount; ++i )
		cTeam[1].Update( pEnemyInfo[i].m_szID, pEnemyInfo[i].m_ulScore, pEnemyInfo[i].m_ulKill, pEnemyInfo[i].m_ulDeath, pEnemyInfo[i].m_ulSequence );

	//cTeam[0].Sort();
	//cTeam[1].Sort();
}

BOOL	GuildBattleResultInfo::IsMember( char* szName, BOOL bMy )
{
	if( !szName )	return FALSE;

	GuildResultUnitVec& vecGuildResultUnit = cTeam[bMy ? 0 : 1].vecGuildResultUnit;
	for( GuildResultUnitVecItr Itr = vecGuildResultUnit.begin(); Itr != vecGuildResultUnit.end(); ++Itr )
	{
		if( (*Itr).strName == szName )
			return TRUE;
	}

	return FALSE;
}

BOOL	GuildBattleResultInfo::IsCurRoundMember( char* szName, UINT32 ulRound, BOOL bMy )
{
	if( !szName )	return FALSE;

	GuildResultUnitVec& vecGuildResultUnit = cTeam[bMy ? 0 : 1].vecGuildResultUnit;
	//if( 0 > ulRound || ulRound >= vecGuildResultUnit.size() )	return FALSE;
	for( GuildResultUnitVecItr Itr = vecGuildResultUnit.begin(); Itr != vecGuildResultUnit.end(); ++Itr )
		if( (*Itr).ulSequence == ulRound )
			return (*Itr).strName == szName ? TRUE : FALSE;

	return FALSE;
}

void	GuildBattleResultInfo::GetRoundName( UINT32 ulRound, string& strMyTeam, string& strYourTeam )
{
	for( GuildResultUnitVecItr Itr = cTeam[0].vecGuildResultUnit.begin(); Itr != cTeam[0].vecGuildResultUnit.end(); ++Itr )
	{
		if( (*Itr).ulSequence == ulRound )
		{
			strMyTeam = (*Itr).strName;
			break;
		}
	}

	for( GuildResultUnitVecItr Itr = cTeam[1].vecGuildResultUnit.begin(); Itr != cTeam[1].vecGuildResultUnit.end(); ++Itr )
	{
		if( (*Itr).ulSequence == ulRound )
		{
			strYourTeam = (*Itr).strName;
			break;
		}
	}
}

//-------------------------- AgcmUIGuildBattleResult ------------------------------
AgcmUIGuildBattleResult::AgcmUIGuildBattleResult()
{
	m_ulRound = 0;

	Clear();
}

AgcmUIGuildBattleResult::~AgcmUIGuildBattleResult()
{
	Clear();
}

BOOL	AgcmUIGuildBattleResult::AddModule( AgcmUIManager2* pUIMgr, AgcmUIGuild* pUIGuild )
{
	m_pcsAgcmUIManager2	= pUIMgr;
	m_pcsAgcmUIGuild	= pUIGuild;

	for( int i=0; i<eGuildBattleMax; ++i )
		memset( m_szGuildBattleResultPointArray[i], 0, 256 );

	CHAR* szMessagePoint = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleResult_Point" );
	CHAR* szMessageKillDead = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleResult_Kill/Dead" );
	strcpy( m_szGuildBattleResultPointArray[eGuildBattlePointMatch], szMessagePoint ? szMessagePoint : "포인트" );
	strcpy( m_szGuildBattleResultPointArray[eGuildBattlePKMatch], szMessagePoint ? szMessageKillDead : "KO승/KO패" );
	strcpy( m_szGuildBattleResultPointArray[eGuildBattleDeadMatch], szMessagePoint ? szMessagePoint : "포인트" );
	strcpy( m_szGuildBattleResultPointArray[eGuildBattleTotalSurvive], szMessagePoint ? szMessagePoint : "포인트" );
	strcpy( m_szGuildBattleResultPointArray[eGuildBattlePrivateSurvive], szMessagePoint ? szMessagePoint : "포인트" );
	
	m_pstBattleInfo = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleInfo", &m_lBattleInfo, sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );

	m_pstWinLose[0] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildNameLeft", &m_lWinLose[0], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[1] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildResultNameLeft", &m_lWinLose[1], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[2] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildNameRight", &m_lWinLose[2], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[3] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildResultNameRight", &m_lWinLose[3], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[4] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleLeftTotal", &m_lWinLose[4], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[5] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleRightTotal", &m_lWinLose[5], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[6] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleWin", &m_lWinLose[6], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[7] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleLose", &m_lWinLose[7], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[8] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleWinDetail1", &m_lWinLose[8], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[9] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleWinDetail2", &m_lWinLose[9], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[10] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleLoseDetail1", &m_lWinLose[10], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );
	m_pstWinLose[11] = m_pcsAgcmUIManager2->AddUserData( "DisplayGuildBattleLoseDetail2", &m_lWinLose[11], sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_STRING );

	m_pcsAgcmUIManager2->AddUserData( "GuildBattleResultRound", &m_nResultRound, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32 );

	for( int i=0; i<GUILD_BATTLE_RESULT_MAX; ++i )
		m_lBattleResult[i] = i;
	m_pstBattleResult = m_pcsAgcmUIManager2->AddUserData( "GuildBattleResultList", m_lBattleResult, sizeof(INT32), GUILD_BATTLE_RESULT_MAX, AGCDUI_USERDATA_TYPE_INT32 );
	m_pstBattleResult->m_stUserData.m_lCount = 0;

	AgcdUIDisplay* pDisplay = NULL;
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleInfo", 0, CBDisplayGuildBattleInfo, AGCDUI_USERDATA_TYPE_STRING );

	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildNameLeft", 0, CBDisplayGuildNameLeft, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildResultNameLeft", 0, CBDisplayGuildResultNameLeft, AGCDUI_USERDATA_TYPE_STRING );

	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildNameRight", 0, CBDisplayGuildNameRight, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildResultNameRight", 0, CBDisplayGuildResultNameRight, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleLeftTotal", 0, CBDisplayGuildBattleLeftTotal, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleRightTotal", 0, CBDisplayGuildBattleRightTotal, AGCDUI_USERDATA_TYPE_STRING );

	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleWin", 0, CBDisplayGuildBattleWin, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleLose", 0, CBDisplayGuildBattleLose, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleWinDetail1", 0, CBDisplayGuildBattleWinDetail1, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleWinDetail2", 0, CBDisplayGuildBattleWinDetail2, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleLoseDetail1", 0, CBDisplayGuildBattleLoseDetail1, AGCDUI_USERDATA_TYPE_STRING );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleLoseDetail2", 0, CBDisplayGuildBattleLoseDetail2, AGCDUI_USERDATA_TYPE_STRING );

	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberNameLeft", 0, CBDisplayGuildBattleMemberNameLeft, AGCDUI_USERDATA_TYPE_INT32 );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberPointLeft", 0, CBDisplayGuildBattleMemberPointLeft, AGCDUI_USERDATA_TYPE_INT32 );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberNameRight", 0, CBDisplayGuildBattleMemberNameRight, AGCDUI_USERDATA_TYPE_INT32 );
	pDisplay = m_pcsAgcmUIManager2->AddDisplay( this, "DisplayGuildBattleMemberPointRight", 0, CBDisplayGuildBattleMemberPointRight, AGCDUI_USERDATA_TYPE_INT32 );

	m_pcsAgcmUIManager2->AddBoolean( this, "IsGuildBattleRound", IsBattleRound, AGCDUI_USERDATA_TYPE_NONE );

	Init( pUIMgr, "UI_GuildBattleResult" );

	return TRUE;
}

void AgcmUIGuildBattleResult::OpenResult()
{
	if( IsOpen() )
		Close();
	else
	{
		RefreshUserData();
		if( m_pcsAgcmUIGuild && m_pcsAgcmUIGuild->m_pcsAgcmGuild )
		{
			AgpdGuild* pGuild = m_pcsAgcmUIGuild->m_pcsAgcmGuild->GetSelfGuildLock();
			if( m_pcsAgcmUIGuild->m_pcsAgpmGuild && m_pcsAgcmUIGuild->m_pcsAgpmGuild->IsBattleStatus( pGuild ) )
				Open();

			if( pGuild )
				pGuild->m_Mutex.Release();

		}
	}
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;
	strcpy( szDisplay, pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().GetBattleTypeString() );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildNameLeft(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;
	sprintf( szDisplay, "%s[%s]", pThis->cTeam[0].strGuildName.c_str(), pThis->cTeam[0].strMasterName.c_str() );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildResultNameLeft(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;

	CHAR* szType = pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().GetBattleTypeString();
	if( szType )
		strcpy( szDisplay, szType );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildNameRight(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;
	sprintf( szDisplay, "%s[%s]", pThis->cTeam[1].strGuildName.c_str(), pThis->cTeam[1].strMasterName.c_str() );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildResultNameRight(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;

	CHAR* szType = pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().GetBattleTypeString();
	if( szType )
		strcpy( szDisplay, szType );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleMemberNameLeft( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass || !pData )	return FALSE;

	AgcmUIGuildBattleResult* pThis	= (AgcmUIGuildBattleResult*)pClass;
	INT32 lIndex = *((INT32*)pData);

	const GuildResultUnitVec& vecGuildResultUnit = pThis->cTeam[0].vecGuildResultUnit;
	if( (INT32)vecGuildResultUnit.size() <= lIndex )
		return TRUE;

	strcpy( szDisplay, vecGuildResultUnit[lIndex].strName.c_str() );

	return TRUE;
}

void AgcmUIGuildBattleResult::GetResultPrivateString( UINT32 ulRound, CHAR* szText, BOOL bMy )
{
	CHAR* szWin = m_pcsAgcmUIManager2->GetUIMessage( "Guild_Msg_Battle_Result_Win" );
	CHAR* szDraw = m_pcsAgcmUIManager2->GetUIMessage( "Guild_Msg_Battle_Result_Draw" );
	CHAR* szLose = m_pcsAgcmUIManager2->GetUIMessage( "Guild_Msg_Battle_Result_Lose" );
	CHAR* szIng = m_pcsAgcmUIManager2->GetUIMessage( "Guild_Msg_Battle_Result_Ing" );
	CHAR* szNotPlay = m_pcsAgcmUIManager2->GetUIMessage( "Guild_Msg_Battle_Result_NotPlay" );

	if( !m_bResult && m_ulRound == ulRound )
		sprintf( szText, szIng ? szIng : "진행중" );
	else if( !m_bResult && m_ulRound < ulRound )
	{
		sprintf( szText, m_bResult ? ( szDraw ? szDraw : "무승부" ) : ( szNotPlay ? szNotPlay : "대기중" ) );
	}
	else
	{
		GuildResultUnitVec& vecGuildResultUnit = cTeam[ bMy ? 0 : 1 ].vecGuildResultUnit;
		if( vecGuildResultUnit.size() > ulRound )
			sprintf( szText, vecGuildResultUnit[ulRound].ulKO ? ( szWin ? szWin : "승" ) : ( szLose ? szLose : "패" ) );
		else
			sprintf( szText, "" );
	}
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleMemberPointLeft( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass || !pData )	return FALSE;

	AgcmUIGuildBattleResult* pThis	= (AgcmUIGuildBattleResult*)pClass;
	INT32 lIndex = *((INT32*)pData);

	const GuildResultUnitVec& vecGuildResultUnit = pThis->cTeam[0].vecGuildResultUnit;
	if( (INT32)vecGuildResultUnit.size() <= lIndex )
		return TRUE;

	eGuildBattleType eBattleType = (eGuildBattleType)pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().GetBattleType();
	//Type별로 출력해주는것이 달라야 한다
	if( eGuildBattlePKMatch  == eBattleType )
		sprintf( szDisplay, "%d / %d", vecGuildResultUnit[lIndex].ulKill, vecGuildResultUnit[lIndex].ulDeath );
	else if( eGuildBattlePrivateSurvive== eBattleType )
	{
		CHAR szText[16];
		pThis->GetResultPrivateString( lIndex, szText, TRUE );
		sprintf( szDisplay, "%s", szText );
	}
	else
		sprintf( szDisplay, "%d", vecGuildResultUnit[lIndex].ulKO );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleMemberNameRight( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass || !pData )	return FALSE;

	AgcmUIGuildBattleResult* pThis	= (AgcmUIGuildBattleResult*)pClass;
	INT32 lIndex = *((INT32*)pData);

	const GuildResultUnitVec& vecGuildResultUnit = pThis->cTeam[1].vecGuildResultUnit;
	if( (INT32)vecGuildResultUnit.size() <= lIndex )
		return TRUE;

	strcpy( szDisplay, vecGuildResultUnit[lIndex].strName.c_str() );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleMemberPointRight( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass || !pData )	return FALSE;

	AgcmUIGuildBattleResult* pThis	= (AgcmUIGuildBattleResult*)pClass;
	INT32 lIndex = *((INT32*)pData);

	const GuildResultUnitVec& vecGuildResultUnit = pThis->cTeam[1].vecGuildResultUnit;
	if( (INT32)vecGuildResultUnit.size() <= lIndex )
		return TRUE;

	eGuildBattleType eBattleType = (eGuildBattleType)pThis->m_pcsAgcmUIGuild->GetUIGuildBattleOffer().GetBattleType();
	//Type별로 출력해주는것이 달라야 한다
	if( eGuildBattlePKMatch  == eBattleType )
		sprintf( szDisplay, "%d / %d", vecGuildResultUnit[lIndex].ulKill, vecGuildResultUnit[lIndex].ulDeath );
	else if( eGuildBattlePrivateSurvive == eBattleType )
	{
		CHAR szText[16];
		pThis->GetResultPrivateString( lIndex, szText, FALSE );
		sprintf( szDisplay, "%s", szText );
	}
	else
		sprintf( szDisplay, "%d", vecGuildResultUnit[lIndex].ulKO );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleLeftTotal(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass )		return FALSE;

	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;
	/*if( AGPMGUILD_BATTLE_RESULT_NONE == pThis->eResult )
	{
		UINT32	ultotal = 0;
		for( GuildResultUnitVecItr Itr = pThis->cTeam[0].vecGuildResultUnit.begin(); Itr != pThis->cTeam[0].vecGuildResultUnit.end(); ++Itr )
			ultotal += (*Itr).ulKO;
		sprintf( szDisplay, "Total : %d", ultotal );
	}
	else
		sprintf( szDisplay, "Total : %d", pThis->cTeam[0].ulTotal );*/

	UINT32	ultotal = 0;
	for( GuildResultUnitVecItr Itr = pThis->cTeam[0].vecGuildResultUnit.begin(); Itr != pThis->cTeam[0].vecGuildResultUnit.end(); ++Itr )
		ultotal += (*Itr).ulKO;
	sprintf( szDisplay, "Total : %d", ultotal );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleRightTotal(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass )		return FALSE;

	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;
	//if( AGPMGUILD_BATTLE_RESULT_NONE == pThis->eResult )
	//{
	//	UINT32	ultotal = 0;
	//	for( GuildResultUnitVecItr Itr = pThis->cTeam[1].vecGuildResultUnit.begin(); Itr != pThis->cTeam[1].vecGuildResultUnit.end(); ++Itr )
	//		ultotal += (*Itr).ulKO;
	//	sprintf( szDisplay, "Total : %d", ultotal );
	//}
	//else
	//	sprintf( szDisplay, "Total : %d", pThis->cTeam[1].ulTotal );

	UINT32	ultotal = 0;
	for( GuildResultUnitVecItr Itr = pThis->cTeam[1].vecGuildResultUnit.begin(); Itr != pThis->cTeam[1].vecGuildResultUnit.end(); ++Itr )
		ultotal += (*Itr).ulKO;
	sprintf( szDisplay, "Total : %d", ultotal );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleWin(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass )		return FALSE;

	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;

	strcpy( szDisplay, " " );

	UINT32 ulPoint = 0;
	switch( pThis->eResult )
	{
	case AGPMGUILD_BATTLE_RESULT_WIN:
	case AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW:
		sprintf( szDisplay, "%s Victory!! %d GP", pThis->cTeam[0].strGuildName.c_str(), pThis->GetGPPoint( TRUE ) );
		break;
	case AGPMGUILD_BATTLE_RESULT_DRAW:
		sprintf( szDisplay, "%s Draw!! %d GP", pThis->cTeam[0].strGuildName.c_str(), pThis->GetGPPoint( TRUE ) );
		break;
	case AGPMGUILD_BATTLE_RESULT_LOSE:
	case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
		sprintf( szDisplay, "%s Victory!! %d GP", pThis->cTeam[1].strGuildName.c_str(), pThis->GetGPPoint( FALSE ) );
		break;
	}

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleLose(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if( !pClass )		return FALSE;

	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;

	strcpy( szDisplay, " " );

	UINT32 ulPoint = 0;
	switch( pThis->eResult )
	{
	case AGPMGUILD_BATTLE_RESULT_WIN:
	case AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW:
		sprintf( szDisplay, "%s Lose!! %d GP", pThis->cTeam[1].strGuildName.c_str(), pThis->GetGPPoint( FALSE ) );
		break;
	case AGPMGUILD_BATTLE_RESULT_DRAW:
		sprintf( szDisplay, "%s Draw!! %d GP", pThis->cTeam[1].strGuildName.c_str(), pThis->GetGPPoint( FALSE ) );
		break;
	case AGPMGUILD_BATTLE_RESULT_LOSE:
	case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
		sprintf( szDisplay, "%s Lose!! %d GP", pThis->cTeam[0].strGuildName.c_str(), pThis->GetGPPoint( TRUE ) );
		break;
	}

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleWinDetail1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	//if( !pClass )		return FALSE;

	//AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;

	//strcpy( szDisplay, " " );

	//UINT32 ulPoint = 0;
	//switch( pThis->eResult )
	//{
	//case AGPMGUILD_BATTLE_RESULT_WIN:
	//case AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW:
	//	sprintf( szDisplay, "%s %s %d Guild Point", pThis->cTeam[0].strGuildName.c_str(), "승", pThis->GetGPPoint( TRUE ) );
	//	break;
	//case AGPMGUILD_BATTLE_RESULT_DRAW:
	//	sprintf( szDisplay, "%s %s %d Guild Point", pThis->cTeam[0].strGuildName.c_str(), "무승부", pThis->GetGPPoint( TRUE ) );
	//	break;
	//case AGPMGUILD_BATTLE_RESULT_LOSE:
	//case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
	//	sprintf( szDisplay, "%s %s %d Guild Point", pThis->cTeam[1].strGuildName.c_str(), "승", pThis->GetGPPoint( FALSE ) );
	//	break;
	//}

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleWinDetail2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	//if( !pClass )		return FALSE;

	//AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;

	//strcpy( szDisplay, " " );
	//if( AGPMGUILD_BATTLE_RESULT_NONE == pThis->eResult )		return TRUE;

	//BOOL bMy = TRUE;
	//switch( pThis->eResult )
	//{
	//case AGPMGUILD_BATTLE_RESULT_LOSE:
	//case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
	//	bMy = FALSE;
	//	break;
	//}

	//sprintf( szDisplay, "%s * %s = %d * %d = %dGP",  pThis->GetResultPointString(), pThis->GetTimePointString(), pThis->GetResultPoint( bMy ), pThis->GetTimePoint(), pThis->GetGPPoint( bMy ) );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleLoseDetail1(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	//if( !pClass )		return FALSE;

	//AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;

	//strcpy( szDisplay, " " );

	//UINT32 ulPoint = 0;
	//switch( pThis->eResult )
	//{
	//case AGPMGUILD_BATTLE_RESULT_WIN:
	//case AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW:
	//	sprintf( szDisplay, "%s %s %d Guild Point", pThis->cTeam[1].strGuildName.c_str(), "패", pThis->GetGPPoint( FALSE ) );
	//	break;
	//case AGPMGUILD_BATTLE_RESULT_DRAW:
	//	sprintf( szDisplay, "%s %s %d Guild Point", pThis->cTeam[1].strGuildName.c_str(), "무승부", pThis->GetGPPoint( FALSE ) );
	//	break;
	//case AGPMGUILD_BATTLE_RESULT_LOSE:
	//case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
	//	sprintf( szDisplay, "%s %s %d Guild Point", pThis->cTeam[0].strGuildName.c_str(), "패", pThis->GetGPPoint( TRUE ) );
	//	break;
	//}

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::CBDisplayGuildBattleLoseDetail2(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	//if( !pClass )		return FALSE;

	//AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*)pClass;

	//strcpy( szDisplay, " " );

	//if( AGPMGUILD_BATTLE_RESULT_NONE == pThis->eResult )		return TRUE;

	//BOOL bMy = FALSE;
	//switch( pThis->eResult )
	//{
	//case AGPMGUILD_BATTLE_RESULT_LOSE:
	//case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
	//	bMy = TRUE;
	//	break;
	//}

	//sprintf( szDisplay, "%s * %s = %d * %d = %dGP",  pThis->GetResultPointString(), pThis->GetTimePointString(), pThis->GetResultPoint( bMy ), pThis->GetTimePoint(), pThis->GetGPPoint( bMy ) );

	return TRUE;
}

BOOL AgcmUIGuildBattleResult::IsBattleRound(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuildBattleResult* pThis = (AgcmUIGuildBattleResult*) pClass;
	return pThis->m_ulRound > (UINT32)pcsSourceControl->m_lUserDataIndex;
}

void AgcmUIGuildBattleResult::RefreshUserData()
{
	if( !m_pcsAgcmUIManager2 )	return;

	m_pstBattleResult->m_stUserData.m_lCount = cTeam[0].vecGuildResultUnit.size() > cTeam[1].vecGuildResultUnit.size() ? 
												cTeam[0].vecGuildResultUnit.size() : cTeam[1].vecGuildResultUnit.size();
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstBattleResult );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstBattleInfo );
	for( int i=0; i<12; ++i )
		m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstWinLose[i] );
}

UINT32	AgcmUIGuildBattleResult::GetGPPoint( BOOL bMy )
{
	return cTeam[ bMy ? 0 : 1 ].lTotal;
}

CHAR*	AgcmUIGuildBattleResult::GetResultPointString()
{
	CHAR* szMessage = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleResult_VictoryPoint" );
	return szMessage ? szMessage : "승리포인트";
}

CHAR*	AgcmUIGuildBattleResult::GetTimePointString()
{
	CHAR* szMessage = m_pcsAgcmUIManager2->GetUIMessage( "GuildBattleResult_TimePoint" );
	return szMessage ? szMessage : "시간포인트";
}