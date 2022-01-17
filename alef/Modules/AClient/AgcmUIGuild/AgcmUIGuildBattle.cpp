// AgcmUIGuildBattle.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 04. 19.

#include "AgcmUIGuild.h"
#include "AuStrTable.h"

#define AGCMUIGUILD_DRAW_BATTLE_INFO_INTERVAL			100		// 0.5 초


// 2005.04.18. steeple
BOOL AgcmUIGuild::OpenBattleRequestConfirmUI(CHAR* szEnemyGuildID, INT32 lType, UINT32 ulDuration, UINT32 ulPerson)
{
	if( !szEnemyGuildID )	return FALSE;
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return FALSE;
	if( !strlen(m_pcsAgcmGuild->GetSelfGuildID()) )		return FALSE;
	if( !m_pcsAgpmGuild->IsMaster( m_pcsAgcmGuild->GetSelfGuildID(), pcsCharacter->m_szID ) )
		return TRUE;

	if( !strcmp( m_pcsAgcmGuild->GetSelfGuildID(), szEnemyGuildID ) )
	{
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventGuildBattleFailRequestSelfGuild);
		return TRUE;
	}

	if( !m_cUIGuildBattleOffer.IsPossiblePerson( lType, ulPerson ) )
		return TRUE;

	// 멤버변수에 저장을 하고
	strncpy( m_szBattleEnemyGuildID, szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH );
	GetUIGuildBattleOffer().Open( lType, ulDuration, ulPerson, m_lEventOpenBattleRequestConfirmUI );

	return TRUE;
}

// 2005.04.18. steeple
// 정말 배틀 취소 신청할 건지 물어본다.
BOOL AgcmUIGuild::OpenBattleCancelRequestConfirmUI()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;

	CHAR* szJoinedGuildID = m_pcsAgcmGuild->GetSelfGuildID();
	if(!szJoinedGuildID || strlen(szJoinedGuildID) == 0)
		return FALSE;

	BOOL bUIOpen = FALSE;

	AgpdGuild* pcsGuild = m_pcsAgpmGuild->GetGuildLock(szJoinedGuildID);
	if(pcsGuild)
	{
		// 마스터 일때만 한다.
		if(m_pcsAgpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
		{
			if(m_pcsAgpmGuild->IsBattleReadyStatus(pcsGuild) &&						// 배틀 준비 상태
				strlen(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID) > 0)
			{
				bUIOpen = TRUE;
				strncpy(m_szBattleEnemyGuildID, pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
			}
		}

		pcsGuild->m_Mutex.Release();
	}

	if(bUIOpen)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenBattleCancelRequestConfirmUI);

	return TRUE;
}

// 2005.04.18. steeple
// 서버에서 배틀 취소 신청을 받았을 때 어케할건지 물어본다.
BOOL AgcmUIGuild::OpenBattleCancelAcceptConfirmUI()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return FALSE;

	BOOL bUIOpen = FALSE;
	AgpdGuild* pcsGuild = m_pcsAgcmGuild->GetSelfGuildLock();
	if( pcsGuild )
	{
		// 마스터 일때만 한다.
		if( m_pcsAgpmGuild->IsMaster( pcsGuild, pcsCharacter->m_szID ) )
		{
			if( m_pcsAgpmGuild->IsBattleReadyStatus(pcsGuild) && strlen(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID ) > 0)
			{
				bUIOpen = TRUE;
				strncpy(m_szBattleEnemyGuildID, pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);
			}
		}

		pcsGuild->m_Mutex.Release();
	}

	if( bUIOpen )
		m_pcsAgcmUIManager2->ThrowEvent( m_lEventOpenBattleCancelAcceptConfirmUI );

	return TRUE;
}

// 2005.04.18. steeple
// 정말 GG 칠건지 다시 한번 물어본다.
BOOL AgcmUIGuild::OpenBattleWithdrawConfirmUI()
{
	AgpdCharacter* pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )								return FALSE;
		
	CHAR* szSelfGuildID = m_pcsAgcmGuild->GetSelfGuildID();
	if( !szSelfGuildID && !szSelfGuildID[0] )		return FALSE;
		
	// 마스터가 아니라면 무시
	if( !m_pcsAgpmGuild->IsMaster( szSelfGuildID, pcsCharacter->m_szID ) )
		return TRUE;

	m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenBattleWithdrawConfirmUI);

	return TRUE;
}

// 2005.04.19. steeple
// 시간 업뎃을 받을때마다 불린다.
BOOL AgcmUIGuild::UpdateBattleTime(AgpdGuild* pcsGuild)
{
	return TRUE;		//아무짓을 안해도 되는구나 ^^
}

// 2005.04.19. steeple
// Idle 에서 불린다.
BOOL AgcmUIGuild::ProcessDrawBattleInfo(UINT32 ulClockCount)
{
	AgpdGuild* pcsGuild = m_pcsAgcmGuild->GetSelfGuildLock();
	if( !pcsGuild )		return FALSE;

	// 배틀중일 때만 그려주면 된다.
	// 종족전중이면 안그려준다.
	if( m_pcsAgpmGuild->IsBattleStatus( pcsGuild ) )
	{
		if ( m_pcsAgpmPvP->IsInRaceBattle() )
			DrawBattleInfoInit(pcsGuild);
		else
			DrawBattleInfoTime(pcsGuild);
	}

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.19. steeple
// 화면에 메시지를 뿌려준다~ eType 으로 잘 구분해서 뿌린다.
BOOL AgcmUIGuild::DrawBattleInfo(AgpdGuild* pcsGuild, AgcmUIGuildDrawBattleInfoType eType)
{
	switch( eType )
	{
		case AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_INIT:
			DrawBattleInfoInit(pcsGuild);
			break;
		case AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_ACCEPT:
			DrawBattleInfoAccept(pcsGuild);
			break;
		case AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_START:
			DrawBattleInfoStart(pcsGuild);
			break;
		case AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_TIME:
			if (m_pcsAgpmPvP->IsInRaceBattle())
				DrawBattleInfoInit(pcsGuild);
			else
				DrawBattleInfoTime(pcsGuild);
			break;
		case AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_SCORE:
			if (m_pcsAgpmPvP->IsInRaceBattle())
				DrawBattleInfoInit(pcsGuild);
			else
				DrawBattleInfoScore(pcsGuild);
			break;
		case AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_RESULT:
			DrawBattleInfoResult(pcsGuild);
			break;
	}

	return TRUE;
}

// 2005.04.19. steeple
// 화면에 뿌린거 다 지운다.
BOOL AgcmUIGuild::DrawBattleInfoInit(AgpdGuild* pcsGuild)
{
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectText(_T(""), 0, 0, FALSE, 0, 0.0f, 0, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectAboveText(_T(""), 0, 0, FALSE, 0, 0.0f);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectSubText(_T(""), 0, 0, FALSE, 0, 0.0f);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow2.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow3.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow4.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow5.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow6.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow7.SetEffectSubText(_T(""), 0, 0, 0, FALSE, 0, 0.0f, 0, TRUE);
	
	return TRUE;
}

// 2005.04.19. steeple
// Accept 됨을 뿌린다.
BOOL AgcmUIGuild::DrawBattleInfoAccept(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lFont	= 3;
	INT32 lOffsetY = INT32(m_pcsAgcmUIManager2->m_csEffectWindow.h * 0.19f);
	INT32 lDuration	= 5000;

	FLOAT fScale = 1.4f;
	UINT32 uColor = 0xFFFFFF00;

	CHAR szMessage[255];
	memset(szMessage, 0, sizeof(szMessage));

	sprintf(szMessage, ClientStr().GetStr(STI_BATTLE_WITH), pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID);

	//CHAR* szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_READY_INFO);
	//if(szUIMessage)
	//	sprintf(szMessage, szUIMessage, pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID);

	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectText(szMessage, lFont, lOffsetY, FALSE, uColor, fScale, lDuration);

	return TRUE;
}

// 2005.04.19. steeple
// Start 됨을 뿌린다.
BOOL AgcmUIGuild::DrawBattleInfoStart(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lFont	= 3;
	INT32 lOffsetY = INT32(m_pcsAgcmUIManager2->m_csEffectWindow.h * 0.19f);
	INT32 lDuration	= 5000;

	FLOAT fScale = 1.4f;
	UINT32 uColor = 0xFFFF0000;

	CHAR szMessage[255];
	memset(szMessage, 0, sizeof(szMessage));

	sprintf(szMessage, ClientStr().GetStr(STI_BATTLE_START));

	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectText(szMessage, lFont, lOffsetY, FALSE, uColor, fScale, lDuration);

	return TRUE;
}

// 2005.04.19. steeple
// Time 업뎃~
// 이 함수는 Idle 에서도 불린다. 0.5 초에 한번씩
BOOL AgcmUIGuild::DrawBattleInfoTime(AgpdGuild* pcsGuild)
{
	if( !pcsGuild )		return FALSE;

	INT32 lFont	= 3;
	INT32 lOffsetX = 5;
	INT32 lOffsetY = 230;
	INT32 lDuration	= 0x7FFFFFFF;		// 이거 시간은 무한대루 준다. No-Fade

	FLOAT fScale = 1.05f;

	UINT32 uColor = D3DCOLOR_RGBA( 255, 255, 255, 0 );

	CHAR szMessage[255], szMessage2[255];
	memset(szMessage, 0, sizeof(szMessage));
	memset(szMessage2, 0, sizeof(szMessage2));

	UINT32 ulCurrentClockCount = GetClockCount();

	if(m_pcsAgpmGuild->IsBattleReadyStatus(pcsGuild))
	{
		INT32 lRemainTime = pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime - (pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime - pcsGuild->m_csCurrentBattleInfo.m_ulAcceptTime);
		lRemainTime = max( 0, lRemainTime );

		// 아직 시작하기 1분이상 남아있다면
		if( lRemainTime >= 60 )
		{
			INT32 lRemainTimeMin = lRemainTime / 60;
			INT32 lRemainTimeSec = lRemainTime % 60;
			sprintf(szMessage, "vs %s", pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID );
			sprintf(szMessage2, "%s %d:%02d %s", ClientStr().GetStr(STI_BATTLE_BEGIN), lRemainTimeMin, lRemainTimeSec, ClientStr().GetStr(STI_PRE));
		}
		else
		{	// 밀리초 단위도 살짝 보여준다.
			UINT32 ulRemainTimeMSec = ( 10 - ( ( ulCurrentClockCount % 1000 ) / 100) ) % 10;

			sprintf(szMessage, "vs %s", pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID );
			sprintf(szMessage2, "%s %02d.%u %s", ClientStr().GetStr(STI_BATTLE_BEGIN), lRemainTime, ulRemainTimeMSec, ClientStr().GetStr(STI_PRE));
		}
	}
	else if(m_pcsAgpmGuild->IsBattleIngStatus(pcsGuild))
	{
		//일정 높이 위로 타입을 넣어줘야 겠군. 
		if( strlen( GetUIGuildBattleOffer().GetBattleTypeString() ) > 0 )
		{
			m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow7.SetEffectSubText( GetUIGuildBattleOffer().GetBattleTypeString(), lFont, lOffsetX, lOffsetY - 30, FALSE, uColor, fScale, lDuration, TRUE, 0 );
		}

		INT32 lRemainTime = pcsGuild->m_csCurrentBattleInfo.m_ulDuration - ( pcsGuild->m_csCurrentBattleInfo.m_ulCurrentTime - pcsGuild->m_csCurrentBattleInfo.m_ulStartTime);
		lRemainTime = max( 0, lRemainTime );

		// 배틀중일때 끝나기 1분이상 남아있다면
		if( lRemainTime >= 60)
		{
			INT32 lRemainTimeMin = lRemainTime / 60;
			INT32 lRemainTimeSec = lRemainTime % 60;
			sprintf(szMessage, "vs%s[%s] %d:%02d", pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, lRemainTimeMin, lRemainTimeSec );
		}
		else
		{
			UINT32 ulRemainTimeMSec = ( 10 - ( ( ulCurrentClockCount % 1000 ) / 100 ) ) % 10;
			sprintf(szMessage, "vs%s[%s] %d:%02d", pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID, pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildMasterID, lRemainTime, ulRemainTimeMSec );
		}
	}

	if( strlen(szMessage) > 0 )
		m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow2.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor, fScale, lDuration, TRUE, 0);
											
	if( strlen(szMessage2) > 0 )
		m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow3.SetEffectSubText(szMessage2, lFont, lOffsetX, lOffsetY - 30, FALSE, uColor, fScale, lDuration, TRUE, 0);

	return TRUE;
}

// 2005.04.19. steeple
// 점수를 받는다~
BOOL AgcmUIGuild::DrawBattleInfoScore(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	if(!m_pcsAgpmGuild->IsBattleIngStatus(pcsGuild))
		return FALSE;

	INT32 lFont	= 3;
	INT32 lOffsetX = 5;
	INT32 lOffsetY = 280;
	INT32 lDuration	= 0x7FFFFFFF;		// 이거 시간은 무한대루 준다. No-Fade

	FLOAT fScale = 1.05f;

	UINT32 uColor = 0xFFFFFF00;

	CHAR szMessage[255];
	memset(szMessage, 0, sizeof(szMessage));

	//sprintf(szMessage, "%d : %d", pcsGuild->m_csCurrentBattleInfo.m_lMyScore, pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);
	//m_pcsAgcmFont->FontDrawStart(0);
	//m_pcsAgcmFont->DrawTextIM2D(95, 45, szMessage, 0, 230);
	//m_pcsAgcmFont->FontDrawEnd();
	
	uColor = D3DCOLOR_RGBA( 28, 65, 255, 0 );
	sprintf(szMessage, "%s:%d", ClientStr().GetStr(STI_OUR_TEAM), pcsGuild->m_csCurrentBattleInfo.m_lMyScore);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow3.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor, fScale, lDuration, TRUE, 0);

	lOffsetY += 45;
	uColor = D3DCOLOR_RGBA( 255, 6, 6, 0 );
	sprintf(szMessage, "%s:%d", ClientStr().GetStr(STI_ENEMY), pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow4.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor, fScale, lDuration, TRUE, 0);
										

	lOffsetY -= 45;

	if(pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore > 0)
	{
		CHAR szTmp[32];
		sprintf(szTmp, "%d", pcsGuild->m_csCurrentBattleInfo.m_lMyScore);

		INT32 nSize = 100 + strlen(szTmp) * 10;
		lOffsetX	+= nSize;
		lOffsetY	+= 5;
		lDuration	= 8000;
		fScale		= 1.0f;
		uColor		= D3DCOLOR_RGBA( 56, 206, 255, 0 );

		sprintf(szMessage, "+%d", pcsGuild->m_csCurrentBattleInfo.m_lMyUpScore);
		m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow5.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor, fScale, lDuration, TRUE, 0);

		lOffsetX -= nSize;
	}

	if(pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore > 0)
	{
		CHAR szTmp[32];
		sprintf(szTmp, "%d", pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);

		lOffsetX	+= 100 + strlen(szTmp) * 10;
		lOffsetY	+= 50;
		lDuration	= 8000;
		fScale		= 1.0f;
		uColor		= D3DCOLOR_RGBA( 255, 6, 6, 0 );

		sprintf(szMessage, "+%d", pcsGuild->m_csCurrentBattleInfo.m_lEnemyUpScore);
		m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow6.SetEffectSubText(szMessage, lFont, lOffsetX, lOffsetY, FALSE, uColor, fScale, lDuration, TRUE, 0);
	}

	return TRUE;
}

// 2005.04.19. steeple
// 배틀 결과가 날라왔당~
BOOL AgcmUIGuild::DrawBattleInfoResult(AgpdGuild* pcsGuild)
{
	if(!pcsGuild)
		return FALSE;

	INT32 lFont	= 3;
	INT32 lOffsetY = INT32(m_pcsAgcmUIManager2->m_csEffectWindow.h * 0.19f);
	INT32 lDuration	= 20000;

	FLOAT fScale = 1.7f;
	UINT32 ulColor = 0xFFFFFFFF;

	CHAR szMessage[255];
	memset(szMessage, 0, sizeof(szMessage));

	CHAR* szUIMessage = NULL;

	switch(pcsGuild->m_csCurrentBattleInfo.m_cResult)
	{
		case AGPMGUILD_BATTLE_RESULT_WIN:
		case AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW:
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_WIN);
			strcpy( szMessage, szUIMessage ? szUIMessage : "Win!" );
			ulColor = 0xFF00FF00;
			break;

		case AGPMGUILD_BATTLE_RESULT_DRAW:
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_DRAW);
			strcpy( szMessage, szUIMessage ? szUIMessage : "Draw!" );
			ulColor = 0xFFFFFFFF;
			break;

		case AGPMGUILD_BATTLE_RESULT_LOSE:
		case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_LOSE);
			strcpy( szMessage, szUIMessage ? szUIMessage : "Lose!" );
			ulColor = 0xFF66FFFF;
			break;
	}

	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectText(szMessage, lFont, lOffsetY, FALSE, ulColor, fScale, lDuration, TRUE, TRUE);

	// 위에다가 점수 찍고
	fScale = 1.0f;
	sprintf(szMessage, "%d : %d", pcsGuild->m_csCurrentBattleInfo.m_lMyScore, pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectAboveText(szMessage, lFont, lOffsetY - 40, FALSE, ulColor, fScale);

	memset(szMessage, 0, sizeof(szMessage));
	// GG 로 끝난거면 아래에다가 이유를 찍어준다.
	if(pcsGuild->m_csCurrentBattleInfo.m_cResult == AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW)
	{
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_WIN_BY_GG);
		strcpy(szMessage, szUIMessage ? szUIMessage : ClientStr().GetStr(STI_ENEMY_DECLARE_DEFEAT));
		ulColor = D3DCOLOR_RGBA( 28, 65, 255, 0 );
	}
	else if(pcsGuild->m_csCurrentBattleInfo.m_cResult == AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW)
	{
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_LOSE_BY_GG);
		strcpy( szMessage, szUIMessage ? szUIMessage : ClientStr().GetStr(STI_DECLARE_DEFEAT) );
		ulColor = D3DCOLOR_RGBA( 255, 6, 6, 0 );
	}

	if(strlen(szMessage) > 0)
	{
		fScale = 0.7f;
		m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectSubText(szMessage, lFont, lOffsetY + 90, FALSE, ulColor, fScale);
	}

	return TRUE;
}

BOOL AgcmUIGuild::DrawBattleRount(UINT32 ulRount)
{
	INT32 lFont	= 3;
	INT32 lOffsetY = INT32(m_pcsAgcmUIManager2->m_csEffectWindow.h * 0.19f);
	INT32 lDuration	= 10000;

	FLOAT fScale = 1.4f;
	UINT32 uColor = 0xFFFFFFFF;

	CHAR szMessage[255];
	memset(szMessage, 0, sizeof(szMessage));

	string strMyTeam, strYourTeam;
	GetUIGuildBattleResult().GetRoundName( ulRount - 1/*이전 Round여야 한다*/, strMyTeam, strYourTeam );

	char szText[512];
	sprintf( szText, "%dRound[%s VS %s]", ulRount, strMyTeam.c_str(), strYourTeam.c_str() );
	m_pcsAgcmUIManager2->m_csEffectGuildBattleWindow1.SetEffectText(szText, lFont, lOffsetY, FALSE, uColor, fScale, lDuration);

	return TRUE;
}

// 2005.04.18. steeple
// 배틀 신청 패킷 보낼지 말지 결과가 온다~
BOOL AgcmUIGuild::CBEventReturnBattleRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(lTrueCancel == (INT32)TRUE)
		pThis->GetUIGuildBattleOffer().SendReject();

	return TRUE;
}

// 2005.04.18. steeple
// 배틀 신청 받아서 수락했는 지 안했는 지 결과가 온다.
BOOL AgcmUIGuild::CBEventReturnBattleAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	if(lTrueCancel == (INT32)TRUE)
	{
		AgcmUIGuildBattleOffer& cUIGuildOffer = pThis->GetUIGuildBattleOffer();
		INT32 lType = cUIGuildOffer.GetBattleType();
		if( eGuildBattleTotalSurvive == lType || eGuildBattlePrivateSurvive == lType )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventOpenGuildMemberUI );

			pThis->GetUIGuildBattleMember().SetOpenType( AgcmUIGuildBattleMember::eOpenAccept );
			AgpdGuild* pGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
			if( pGuild )
			{
				pThis->GetUIGuildBattleMember().Open( pGuild );
				pGuild->m_Mutex.Release();
			}
		}
		else
			pThis->GetUIGuildBattleOffer().SendAccept();
	}
	else
	{
		pThis->GetUIGuildBattleOffer().SendReject();

		CHAR* szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_REJECT);
		if(szUIMessage)
			SystemMessage.ProcessSystemMessage(szUIMessage);
	}

	return TRUE;
}

// 2005.04.18. steeple
// 배틀 취소를 할지 말지 물어본 결과가 온다.
BOOL AgcmUIGuild::CBEventReturnBattleCancelRequest(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(lTrueCancel == (INT32)TRUE)
	{
		AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
		if( pcsGuild )
		{
			pThis->GetUIGuildBattleOffer().SendCancelRequest();
			pcsGuild->m_Mutex.Release();
		}
	}

	return TRUE;
}

// 2005.04.18. steeple
// 배틀 취소 요청을 받을지 안 받을지 물어본 것의 결과가 온다.
BOOL AgcmUIGuild::CBEventReturnBattleCancelAccept(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if(pcsGuild)
	{
		if(lTrueCancel == (INT32)TRUE)
		{
			pThis->m_pcsAgcmGuild->SendBattleCancelAccept(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID);
		}
		else
		{
			pThis->m_pcsAgcmGuild->SendBattleCancelReject(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID);

			CHAR* szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_CANCEL_REJECT);
			if( szUIMessage )
				SystemMessage.ProcessSystemMessage(szUIMessage);
		}

		pcsGuild->m_Mutex.Release();
	}

	return TRUE;
}

// 2005.04.18. steeple
// GG 칠 것인지 안 칠것인지 물어본다.
BOOL AgcmUIGuild::CBEventReturnBattleWithdraw(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(lTrueCancel == (INT32)TRUE)
	{
		AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
		if(pcsGuild)
		{
			pThis->m_pcsAgcmGuild->SendBattleWithdraw(pcsGuild->m_csCurrentBattleInfo.m_szEnemyGuildID);
			pcsGuild->m_Mutex.Release();
		}
	}

	return TRUE;
}

// 2005.04.18. steeple
// 다이얼로그 뜰때 EnemyGuildID 를 써준다.
BOOL AgcmUIGuild::CBDisplayDialogEnemyGuildID(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_STRING || !szDisplay)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	sprintf(szDisplay, "%s", pThis->m_szBattleEnemyGuildID);
	
	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayDialogBattleType(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_STRING || !szDisplay)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	sprintf( szDisplay, "%s", pThis->GetUIGuildBattleOffer().GetBattleTypeString() );
	
	return TRUE;
}

// 2005.04.18. steeple
// 다이얼로그 뜰때  Duration를 써준다.
BOOL AgcmUIGuild::CBDisplayDialogDuration(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_STRING || !szDisplay)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	sprintf( szDisplay, "%s", pThis->GetUIGuildBattleOffer().GetBattleTimeString() );
	
	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayDialogBattlePerson(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_STRING || !szDisplay)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32 lType = pThis->GetUIGuildBattleOffer().GetBattleType();
	if( eGuildBattleTotalSurvive == lType || eGuildBattlePrivateSurvive == lType )
		sprintf( szDisplay, "%s", pThis->GetUIGuildBattleOffer().GetBattlePersonString() );
	
	return TRUE;
}

// 2005.04.19. steeple
// 배틀 신청이 들어왔다~
BOOL AgcmUIGuild::CBReceiveBattleRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID))
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	// 마스터일때만 한다.
	if(!pThis->m_pcsAgpmGuild->IsMaster(szGuildID, pcsSelfCharacter->m_szID))
		return FALSE;

	// 멤버 변수에 세팅해주고
	strncpy( pThis->m_szBattleEnemyGuildID, (CHAR*)ppvBuffer[1], AGPMGUILD_MAX_GUILD_ID_LENGTH );
	pThis->GetUIGuildBattleOffer().Open( *((INT32*)ppvBuffer[2]), *((UINT32*)ppvBuffer[3]), *((UINT32*)ppvBuffer[4]), 
										pThis->m_lEventOpenBattleAcceptConfirmUI, TRUE );

	return TRUE;
}

// 2005.04.19. steeple
// 배틀이 승락되었다. READY 상태로 돌입!!! 모든 멤버가 다 받게 된다.
BOOL AgcmUIGuild::CBReceiveBattleAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	AgpdGuildBattle* pstGuildBattle = (AgpdGuildBattle*)pCustData;

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID))
		return FALSE;

	// 길드정보에다가 세팅한다.
	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(szGuildID);
	if( !pcsGuild )		return FALSE;

	// 일단 있는 거 비우고
	pThis->m_pcsAgpmGuild->InitCurrentBattleInfo( pcsGuild );

	pcsGuild->m_cStatus = AGPMGUILD_STATUS_BATTLE_READY;
	memcpy( &pcsGuild->m_csCurrentBattleInfo, pstGuildBattle, sizeof( pcsGuild->m_csCurrentBattleInfo ) );

	//내 길드정도와 상대방 길드정보 셋팅
	pThis->GetUIGuildBattleResult().m_bResult = FALSE;
	pThis->GetUIGuildBattleResult().m_ulRound = 0;
	pThis->GetUIGuildBattleResult().Clear();
	pThis->GetUIGuildBattleResult().SetInfo( pcsGuild );

	pThis->GetUIGuildBattleOffer().SetBattleType( pstGuildBattle->m_eType );
	pThis->GetUIGuildBattleOffer().SetBattleTime( pstGuildBattle->m_ulDuration, TRUE );
	pThis->GetUIGuildBattleOffer().SetBattlePerson( pstGuildBattle->m_ulPerson, TRUE );

	// 시간 업뎃~
	pThis->UpdateBattleTime(pcsGuild);

	// 화면에 뿌려준다.
	pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_ACCEPT);

	CHAR* szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_ACCEPT);
	if(szUIMessage)
	{
		CHAR szMsg[255];
		sprintf(szMsg, szUIMessage, pcsGuild->m_csCurrentBattleInfo.m_ulReadyTime / 60);

		SystemMessage.ProcessSystemMessage(szMsg);
	}

	pcsGuild->m_Mutex.Release();
	return TRUE;
}

// 2005.04.19. steeple
// 배틀 거절 패킷을 받았다. 이 패킷이 오는 경우는 없고, 시스템 메시지만 온다. 그곳에서 처리해준다.
BOOL AgcmUIGuild::CBReceiveBattleReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//if(!pData || !pClass || !pCustData)
	//	return FALSE;

	//CHAR* szGuildID = (CHAR*)pData;
	//AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	//PVOID* ppvBuffer = (PVOID*)pvBuffer;

	//// 자기 길드일 때만 처리해준다.
	//if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID))
	//	return FALSE;

	//AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	//if(!pcsSelfCharacter)
	//	return FALSE;

	//// 마스터일때만 한다.
	//if(!pThis->m_pcsAgpmGuild->IsMaster(szGuildID, pcsSelfCharacter->m_szID))
	//	return FALSE;

	//// 멤버 변수를 비워주고
	//// System Message 밀어넣어준다.
	//CHAR* szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_REJECT_BY_OTHER);
	//if(szUIMessage && ppvBuffer[1])
	//{
	//	CHAR szMsg[255] = {0};
	//	sprintf(szMsg, szUIMessage, (CHAR*)ppvBuffer[1]);
	//	SystemMessage.ProcessSystemMessage(szMsg);
	//}

	return TRUE;
}

// 2005.04.19. steeple
// 배틀 취소 요청을 받았다.
BOOL AgcmUIGuild::CBReceiveBattleCancelRequest(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID))
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	// 마스터가 아니라면 나간다.
	if(!pThis->m_pcsAgpmGuild->IsMaster(pcsGuild, pcsSelfCharacter->m_szID))
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	// 적길드가 아니라면 나간다. 엄한 패킷이 온 경우임.
	if(!pThis->m_pcsAgpmGuild->IsEnemyGuild(pcsGuild, (CHAR*)ppvBuffer[1]))
	{
		pcsGuild->m_Mutex.Release();
		return FALSE;
	}

	pcsGuild->m_Mutex.Release();

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenBattleCancelAcceptConfirmUI);

	return TRUE;
}

// 2005.04.19. steeple
// 배틀 캔슬
BOOL AgcmUIGuild::CBReceiveBattleCancelAccept(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	CHAR* szGuildID = (CHAR*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	//AgpdGuildBattle* pstGuildBattle = (AgpdGuildBattle*)pCustData;

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID))
		return FALSE;

	// 길드정보에다가 세팅한다.
	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(szGuildID);
	if(!pcsGuild)
		return FALSE;

	pcsGuild->m_cStatus = AGPMGUILD_STATUS_NONE;	// 강제루 세팅!!!

	// 일단 있는 거 비우고
	pThis->m_pcsAgpmGuild->InitCurrentBattleInfo(pcsGuild);

	// 화면 비운다.
	pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_INIT);

	// 시스템 메시지 뿌려준다.
	CHAR* szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_CANCEL_ACCEPT);
	if(szUIMessage)
		SystemMessage.ProcessSystemMessage(szUIMessage);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

// 2005.04.19. steeple
BOOL AgcmUIGuild::CBReceiveBattleCancelReject(PVOID pData, PVOID pClass, PVOID pCustData)
{
	// 이 패킷을 받지는 않는다. 서버에서 그냥 시스템 메시지만 보내고 만다.
	return TRUE;
}

// 2005.04.19. steeple
// 배틀 시작을 받았다. 후후훗~
// pcsGuild 는 Lock 되서 넘어온다.
BOOL AgcmUIGuild::CBReceiveBattleStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	OutputDebugString( "BattleStart\n" );

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(pcsGuild->m_szID))
		return FALSE;

	pcsGuild->m_cStatus	= AGPMGUILD_STATUS_BATTLE;		// 강제루 세팅!!!
	pcsGuild->m_csCurrentBattleInfo.m_ulRound	= 0;							//아씨 안해도 될꺼는 같은뎅.. @@

	// 시간 업뎃~
	pThis->GetUIGuildBattleResult().SetInfo(pcsGuild);
	pThis->UpdateBattleTime(pcsGuild);

	// 화면에 뿌려준다.
	pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_START);
	pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_TIME);
	pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_SCORE);

	// 시스템 메시지도
	SystemMessage.ProcessSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_START));

	// 화면에서 우리편 길드는 녹색으로 뿌려야 한다.
	pThis->m_pcsAgpmPvP->EnumCallback(AGPMPVP_CB_ADD_FRIEND_GUILD, pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsGuild->m_szID);

	return TRUE;
}

// 2005.04.19. steeple
// 주기적으로 받는 업뎃 시간~
// pcsGuild 는 Lock 되서 넘어온다.
BOOL AgcmUIGuild::CBReceiveBattleUpdateTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(pcsGuild->m_szID))
		return FALSE;

	// 시간 업뎃~
	pThis->UpdateBattleTime(pcsGuild);

	// 화면에 뿌려준다.
	pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_TIME);

	return TRUE;
}

// 2005.04.19. steeple
// 점수가 날때마다 날아오는 점수 패킷~
// pcsGuild 는 Lock 되서 넘어온다.
BOOL AgcmUIGuild::CBReceiveBattleUpdateScore(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(pcsGuild->m_szID))
		return FALSE;

	// 화면에 뿌려준다.
	pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_SCORE);

	return TRUE;
}

// 2005.04.19. steeple
// 결과가 날아온다.
// pcsGuild 는 Lock 되서 넘어온다.
BOOL AgcmUIGuild::CBReceiveBattleResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( !pData || !pClass )	return FALSE;
		
	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// 자기 길드일 때만 처리해준다.
	//if( !pThis->m_pcsAgcmGuild->IsSelfGuildOperation( pcsGuild->m_szID ) )
	//	return FALSE;

	// 자기 길드의 Status 를 바꿔준다.
	pcsGuild->m_cStatus = AGPMGUILD_STATUS_NONE;
	pThis->GetUIGuildBattleResult().m_bResult = TRUE;
	pThis->GetUIGuildBattleResult().SetInfo( pcsGuild );
	pThis->GetUIGuildBattleResult().RefreshUserData();
	pThis->GetUIGuildBattleResult().Open();

	pThis->m_cUIGuildBattleOffer.Clear();

	// 화면에 뿌려준다.
	// 다른 건 다 지우고,
	pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_INIT);
	//pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_RESULT);

	//// System Message 찍어준다.
	CHAR szSystemMessage[255];
	memset(szSystemMessage, 0, sizeof(szSystemMessage));
	CHAR* szUIMessage = NULL;
	UINT32 ulColor = 0xFFFFFFFF;
	switch(pcsGuild->m_csCurrentBattleInfo.m_cResult)
	{
		case AGPMGUILD_BATTLE_RESULT_WIN:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_WIN2);
			if(szUIMessage)
				sprintf(szSystemMessage, szUIMessage, pcsGuild->m_csCurrentBattleInfo.m_lMyScore, pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);
			ulColor = 0xFF66FFFF;
			break;

		case AGPMGUILD_BATTLE_RESULT_DRAW:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_DRAW2);
			if(szUIMessage)
				sprintf(szSystemMessage, szUIMessage, pcsGuild->m_csCurrentBattleInfo.m_lMyScore, pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);
			ulColor = 0xFF00FF00;
			break;

		case AGPMGUILD_BATTLE_RESULT_LOSE:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_LOSE2);
			if(szUIMessage)
				sprintf(szSystemMessage, szUIMessage, pcsGuild->m_csCurrentBattleInfo.m_lMyScore, pcsGuild->m_csCurrentBattleInfo.m_lEnemyScore);
			ulColor = 0xFFFF00CC;
			break;

		case AGPMGUILD_BATTLE_RESULT_WIN_BY_WITHDRAW:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_WIN_BY_GG2);
			if(szUIMessage)
				strcpy(szSystemMessage, szUIMessage);
			ulColor = 0xFF66FFFF;
			break;

		case AGPMGUILD_BATTLE_RESULT_LOSE_BY_WITHDRAW:
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_BATTLE_LOSE_BY_GG2);
			if(szUIMessage)
				strcpy(szSystemMessage, szUIMessage);
			ulColor = 0xFFFF00CC;
			break;
	}

	if( strlen(szSystemMessage) > 0 )
		SystemMessage.ProcessSystemMessage( szSystemMessage );

	return TRUE;
}

BOOL AgcmUIGuild::CBReceiveBattleMember(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID* ppArray = (PVOID*)pData;
	AgcmUIGuild* pThis	= (AgcmUIGuild*)pClass;

	//어느쪽 길드인지 확인해야 하는뎅.
	BOOL bMy = FALSE;
	CHAR* szGuildID = (CHAR*)ppArray[0];
	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( pcsGuild )
	{
		if( !strcmp( szGuildID, pcsGuild->m_szID ) )
			bMy = TRUE;

		pcsGuild->m_Mutex.Release();
	}
	
	pThis->m_cUIGuildBattleResult.SetMemberInfo( (CHAR*)ppArray[1], *(INT32*)ppArray[2], *(INT32*)ppArray[3], *(INT32*)ppArray[4], -1, bMy );
	pThis->m_cUIGuildBattleResult.RefreshUserData();
	return TRUE;
}

BOOL AgcmUIGuild::CBReceiveBattleMemberList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID*	ppArray = (PVOID*)pData;
	AgcmUIGuild* pThis	= (AgcmUIGuild*)pClass;

	INT32 lMyCount		= *(INT32*)ppArray[0];
	AgpdGuildMemberBattleInfo* pMyMemberInfo = (AgpdGuildMemberBattleInfo*)ppArray[1];
	INT32 lEnemyCount	= *(INT32*)ppArray[2];
	AgpdGuildMemberBattleInfo* pEnemyMemberInfo = (AgpdGuildMemberBattleInfo*)ppArray[3];

	pThis->m_cUIGuildBattleResult.SetMemberListInfo( lMyCount, pMyMemberInfo, lEnemyCount, pEnemyMemberInfo );
	pThis->m_cUIGuildBattleResult.RefreshUserData();
	return TRUE;
}

BOOL AgcmUIGuild::CBReceiveBattleRound(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PVOID*	ppArray = (PVOID*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR* szGuildID	= (CHAR*)ppArray[0];
	UINT32 ulRound	= *(UINT32*)ppArray[1];

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(szGuildID))
		return FALSE;

	AgpdGuild* pGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( pGuild )
	{
		pGuild->m_csCurrentBattleInfo.m_ulRound = ulRound;
		pGuild->m_Mutex.Release();
	}

	pThis->GetUIGuildBattleResult().m_ulRound = ulRound;
	//pThis->DrawBattleRount( ulRound );
	pThis->GetUIGuildBattleResult().RefreshUserData();
	pThis->GetUIGuildBattleResult().Open();

	return TRUE;
}

// 2005.04.19. steeple
// 현재 배틀 정보가 날아온다. 보통 READY 나 배틀중일 때 클라이언트가 접속을 하게 되면 이 패킷을 받는다.
// pcsGuild 는 Lock 되서 넘어온다.
BOOL AgcmUIGuild::CBReceiveBattleInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdGuild* pcsGuild = (AgpdGuild*)pData;
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// 자기 길드일 때만 처리해준다.
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildOperation(pcsGuild->m_szID))
		return FALSE;

	pThis->UpdateBattleTime(pcsGuild);

	pThis->GetUIGuildBattleOffer().SetBattleType( pcsGuild->m_csCurrentBattleInfo.m_eType );
	pThis->GetUIGuildBattleOffer().SetBattleTime( pcsGuild->m_csCurrentBattleInfo.m_ulDuration, TRUE );
	pThis->GetUIGuildBattleOffer().SetBattlePerson( pcsGuild->m_csCurrentBattleInfo.m_ulPerson, TRUE );
	pThis->GetUIGuildBattleResult().SetInfo( pcsGuild );

	// 길드의 현재 상태와, 배틀 상태를 분석해서 뿌린다.
	if(pThis->m_pcsAgpmGuild->IsBattleReadyStatus(pcsGuild))
	{
		pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_TIME);
	}
	else if(pThis->m_pcsAgpmGuild->IsBattleIngStatus(pcsGuild))
	{
		pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_TIME);
		pThis->DrawBattleInfo(pcsGuild, AGCMUIGUILD_DRAW_BATTLE_INFO_TYPE_UPDATE_SCORE);

		// 화면에서 우리편 길드는 녹색으로 뿌려야 한다.
		pThis->m_pcsAgpmPvP->EnumCallback(AGPMPVP_CB_ADD_FRIEND_GUILD, pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pcsGuild->m_szID);
	}

	// 전적 다시 그려준다.
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstRecordUD);

	return TRUE;
}

// 2005.04.19. steeple
// 메인 창의 Reocrd 를 그려준다.
BOOL AgcmUIGuild::CBDisplayRecord(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if(pcsGuild)
	{
		CHAR szBuffer[300];
		memset(szBuffer, 0, sizeof(CHAR) * 300);

		// 2005.04.20. steeple
		// 멤버 보여주는 건 한줄로 줄이고, 그 아래 줄에 전적을 보여주게 바꿨다.
//		sprintf(szBuffer, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_GUILD_MEMBER_INFO),
//									pThis->m_pcsAgpmGuild->GetOnlineMemberCount(pcsGuild), pThis->m_pcsAgpmGuild->GetMemberCount(pcsGuild),
//									pcsGuild->m_lWin, pcsGuild->m_lDraw, pcsGuild->m_lLose);

		//	2005.07.06 By SungHoon
		//	길드 정보 보여주기 수정		임의로 넣었음 나중 UIMessage로 빼야함
		wsprintf(szBuffer, ClientStr().GetStr(STI_GUILD_INFO),
			pcsGuild->m_szID, pcsGuild->m_szMasterID, pThis->m_pcsAgpmGuild->GetMemberCount(pcsGuild), pcsGuild->m_lMaxMemberCount, 
			pThis->m_pcsAgpmGuild->GetOnlineMemberCount(pcsGuild) );

		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);

		pcsGuild->m_Mutex.Release();
	}
	else
	{
		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText("");
	}

	return TRUE;
}

/*
	2005.07.15 by SungHoon
	길드전에 필요한 정보를 초기화하기 위한 Event를 호출한다. 실제 UI Open 은 다음번 Event의 CallBack에서 한다.
*/
BOOL AgcmUIGuild::CBGuildBattleTimeOpenUIClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass ) return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(pThis->m_lSelectedGuildIndex < 0 || pThis->m_lSelectedGuildIndex >= pThis->m_lVisibleGuildCount)
		return FALSE;

	AgcdGuildList* pcsAgcdGuildList = pThis->m_parrAgcdGuildList[pThis->m_lSelectedGuildIndex];
	if( !pcsAgcdGuildList ) return FALSE;

	strncpy( pThis->m_szBattleEnemyGuildID, pcsAgcdGuildList->m_szID, AGPMGUILD_MAX_GUILD_ID_LENGTH );

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if( pcsCharacter )
	{
		AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
		if( pcsGuild )
		{
			if( pThis->m_pcsAgpmGuild->IsMaster( pcsGuild, pcsCharacter->m_szID ) )
			{
				pThis->GetUIGuildBattleOffer().ComboClear();

				if( pThis->m_pcsAgpmGuild->IsBattleStatus( pcsGuild ) )
					SystemMessage.ProcessSystemMessage( "이미 길드전을 신청중입니다" );
				else
					pThis->GetUIGuildBattleOffer().Open( 0, 0, 0, pThis->m_lEventGuildBattleTimeUIOpen );
			}

			pcsGuild->m_Mutex.Release();
		}
		else
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventConfirmJoingRequest );
	}

	return TRUE;
}