#include "AgcmChatting2.h"
#include "AgcmRender.h"
#include "ApUtil.h"
#include <crtdbg.h>
#include <mbstring.h>

#include "AgcmCamera2.h"
//@{ kday 20050430
#include "AgcuCamMode.h"
//@} kday

#include "AgcdEffGlobal.h"
#include "AgcuCamPathWork.h"

#include "AgcmUIChannel.h"
#include "AuStrTable.h"
#include "AgcmTextBoardMng.h"

#include "AgcmUISiegeWar.h"

#include "ApmMap.h"
#include "AgcmTitle.h"

#define	AGCMCHATTING2_INTERVAL_AREA_MESSAGE		5000
#define	AGCMCHATTING2_INTERVAL_RETRY_CONNECT	30000

UINT32	g_aulDefaultChatColor[AGPDCHATTING_TYPE_MAX]; //= 
//{
//	//0xffffffff,			// AGPDCHATTING_TYPE_NORMAL
//	//0xff99ccff,			// AGPDCHATTING_TYPE_WORD_BALLOON
//
//	//0xffffffff,			// AGPDCHATTING_TYPE_UNION
//	//0xffff9900,			// AGPDCHATTING_TYPE_GUILD
//	//0xff00ec06,			// AGPDCHATTING_TYPE_PARTY
//
//	//0xffb1f895,			// AGPDCHATTING_TYPE_WHISPER
//	//0xffff99cc,			// AGPDCHATTING_TYPE_WHISPER2
//
//	//0xff99ccff,			// AGPDCHATTING_TYPE_WHOLE_WORLD
//
//	//0xffffff00,			// AGPDCHATTING_TYPE_SYSTEM_LEVEL1
//	//0xffffff00,			// AGPDCHATTING_TYPE_SYSTEM_LEVEL2
//	//0xffcc0000,			// AGPDCHATTING_TYPE_SYSTEM_LEVEL3
//
//	//0xff00ff00,			// AGPDCHATTING_TYPE_NOTICE_LEVEL1
//	//0xff00ff00,			// AGPDCHATTING_TYPE_NOTICE_LEVEL2
//
//	//0xffffff66,			// AGPDCHATTING_TYPE_GUILD_NOTICE
//	//0xffffff00,			// AGPDCHATTING_TYPE_GUILD_JOINT
//
//	//0xff00ffdc,			// AGPDCHATTING_TYPE_EMPHASIS
//	//0xffc8ffc8,			// AGPDCHATTING_TYPE_SIEGEWARINFO
//
//	0xffffffff,			// AGPDCHATTING_TYPE_NORMAL
//	0xff99ccff,			// AGPDCHATTING_TYPE_WORD_BALLOON
//
//	0xffffffff,			// AGPDCHATTING_TYPE_UNION
//	0xffffa500,			// AGPDCHATTING_TYPE_GUILD
//	0xff7cfc00,			// AGPDCHATTING_TYPE_PARTY
//
//	0xff00ee90,			// AGPDCHATTING_TYPE_WHISPER
//	0xff00ee90,			// AGPDCHATTING_TYPE_WHISPER2
//
//	0xffffff00,			// AGPDCHATTING_TYPE_WHOLE_WORLD
//
//	0xffffff00,			// AGPDCHATTING_TYPE_SYSTEM_LEVEL1
//	0xffffff00,			// AGPDCHATTING_TYPE_SYSTEM_LEVEL2
//	0xffcc0000,			// AGPDCHATTING_TYPE_SYSTEM_LEVEL3
//
//	0xff00ff00,			// AGPDCHATTING_TYPE_NOTICE_LEVEL1
//	0xff00ff00,			// AGPDCHATTING_TYPE_NOTICE_LEVEL2
//
//	0xffffff66,			// AGPDCHATTING_TYPE_GUILD_NOTICE
//	0xffc71585,			// AGPDCHATTING_TYPE_GUILD_JOINT
//
//	0xff00ffdc,			// AGPDCHATTING_TYPE_EMPHASIS
//	0xffc8ffc8,			// AGPDCHATTING_TYPE_SIEGEWARINFO
//};

CHAR	*g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_MAX][2] =
{
	{NULL, NULL},
};

AgcmChatting2::AgcmChatting2()
{
	SetModuleName("AgcmChatting2");

	EnableIdle(TRUE);

	ZeroMemory(m_aszChatMessage, sizeof(CHAR) * AGCMCHATTING_TYPE_MAX * AGCMCHATTING_MAX_CHAT_LIST * (AGCMCHATTING_MAX_CHAT_MESSAGE_LENGTH + 1));
	ZeroMemory(m_aulChatMessageColor, sizeof(UINT32) * AGCMCHATTING_TYPE_MAX * AGCMCHATTING_MAX_CHAT_LIST);
	ZeroMemory(m_aulChatMessageTime, sizeof(UINT32) * AGCMCHATTING_TYPE_MAX * AGCMCHATTING_MAX_CHAT_LIST);
	ZeroMemory(m_aulChatMessageCount, sizeof(UINT32) * AGCMCHATTING_TYPE_MAX);
	ZeroMemory(m_abChatMessageApplyColorAllText, sizeof(BOOL) * AGCMCHATTING_TYPE_MAX * AGCMCHATTING_MAX_CHAT_LIST);

	for (int i = 0; i < AGCMCHATTING_TYPE_MAX; ++i)
		m_abChatMessageEnable[i]	= TRUE;

	m_nNID	= (-1);

	m_ulLastSendAreaMessageMSec	= 0;

//	m_bIsRetryConnectChatServer	= FALSE;
//	m_ulRetryConnectTimeMSec	= 0;
	m_pcamPathWork				= NULL;

	memset(m_szLastWhisperCharacter, 0, sizeof(m_szLastWhisperCharacter));

	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GREETING][0] = ClientStr().GetStr(STI_SA_GREETING);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GREETING][1] = ClientStr().GetStr(STI_SA_GREETING_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_CELEBRATION][0] = ClientStr().GetStr(STI_SA_CELEBRATION);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_CELEBRATION][1] = ClientStr().GetStr(STI_SA_CELEBRATION_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GRATITUDE][0] = ClientStr().GetStr(STI_SA_THANKS);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GRATITUDE][1] = ClientStr().GetStr(STI_SA_THANKS_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_ENCOURAGEMENT][0] = ClientStr().GetStr(STI_SA_GREATE);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_ENCOURAGEMENT][1] = ClientStr().GetStr(STI_SA_GREATE_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_DISREGARD][0] = ClientStr().GetStr(STI_SA_INSULT);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_DISREGARD][1] = ClientStr().GetStr(STI_SA_INSULT_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_DANCING][0] = ClientStr().GetStr(STI_SA_DANCE);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_DANCING][1] = ClientStr().GetStr(STI_SA_DANCE_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_DOZINESS][0] = ClientStr().GetStr(STI_SA_ZZZ);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_DOZINESS][1] = ClientStr().GetStr(STI_SA_ZZZ_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_STRETCH][0] = ClientStr().GetStr(STI_SA_RISING);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_STRETCH][1] = ClientStr().GetStr(STI_SA_RISING_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_LAUGH][0] = ClientStr().GetStr(STI_SA_LAUGH);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_LAUGH][1] = ClientStr().GetStr(STI_SA_LAUGH_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_WEEPING][0] = ClientStr().GetStr(STI_SA_GLOOM);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_WEEPING][1] = ClientStr().GetStr(STI_SA_GLOOM_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_RAGE][0] = ClientStr().GetStr(STI_SA_ANGER);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_RAGE][1] = ClientStr().GetStr(STI_SA_ANGER_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_POUT][0] = ClientStr().GetStr(STI_SA_SULKY);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_POUT][1] = ClientStr().GetStr(STI_SA_SULKY_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_APOLOGY][0] = ClientStr().GetStr(STI_SA_SORRY);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_APOLOGY][1] = ClientStr().GetStr(STI_SA_SORRY_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_TOAST][0] = ClientStr().GetStr(STI_SA_DRINK);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_TOAST][1] = ClientStr().GetStr(STI_SA_DRINK_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_CHEER][0] = ClientStr().GetStr(STI_SA_CHEERS);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_CHEER][1] = ClientStr().GetStr(STI_SA_CHEERS_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_RUSH][0] = ClientStr().GetStr(STI_SA_CHARGE);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_RUSH][1] = ClientStr().GetStr(STI_SA_CHARGE_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_SIT][0] = ClientStr().GetStr(STI_SA_SITDOWN);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_SIT][1] = ClientStr().GetStr(STI_SA_SITDOWN_E);
	// for GM
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_GREETING][0] = ClientStr().GetStr(STI_GM_GREETING);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_GREETING][1] = ClientStr().GetStr(STI_GM_GREETING_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_CELEBRATION][0] = ClientStr().GetStr(STI_GM_CELEBRATION);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_CELEBRATION][1] = ClientStr().GetStr(STI_GM_CELEBRATION_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_DANCING][0] = ClientStr().GetStr(STI_GM_DANCE);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_DANCING][1] = ClientStr().GetStr(STI_GM_DANCE_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_WEEPING][0] = ClientStr().GetStr(STI_GM_GLOOM);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_WEEPING][1] = ClientStr().GetStr(STI_GM_GLOOM_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_DEEPBOW][0] = ClientStr().GetStr(STI_GM_BOW);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_DEEPBOW][1] = ClientStr().GetStr(STI_GM_BOW_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_HI][0] = ClientStr().GetStr(STI_GM_HI);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_HI][1] = ClientStr().GetStr(STI_GM_HI_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_WAIT][0] = ClientStr().GetStr(STI_GM_WAIT);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_WAIT][1] = ClientStr().GetStr(STI_GM_WAIT_E);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_HAPPY][0] = ClientStr().GetStr(STI_GM_NICEDAY);
	g_aszSocialAnimationCommand[AGPDCHAR_SOCIAL_TYPE_GM_HAPPY][1] = ClientStr().GetStr(STI_GM_NICEDAY_E);


}

AgcmChatting2::~AgcmChatting2()
{
	OnClearFilter( TRUE );
	OnClearFilter( FALSE );
}

LuaGlue	LG_PushChatMessage( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nCID			= ( INT32 ) pLua->GetNumberArgument( 1 , -1);
	const char *pMessage	= pLua->GetStringArgument( 2 , "No Message" );

	AgpmChatting *pAgpmChatting = ( AgpmChatting * ) g_pEngine->GetModule( "AgpmChatting" );
	AgpmCharacter *pAgpmCharacter = ( AgpmCharacter * ) g_pEngine->GetModule( "AgpmCharacter" );
	if( !pAgpmChatting ) return 0;

	AgpdCharacter	* pCharacter;
	pCharacter	= pAgpmCharacter->GetCharacter( nCID );
	if( pCharacter ) 
		pAgpmChatting->OperationChat( pCharacter , ( char * ) pMessage );

	return 0;
}

LuaGlue	LG_SendChatMessage( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	const char *pMessage	= pLua->GetStringArgument( 1 , "No Message" );

	AgcmChatting2 *pcsAgcmChatting2 = ( AgcmChatting2 * ) g_pEngine->GetModule( "AgcmChatting2" );

	if( pcsAgcmChatting2 )
	{
		pcsAgcmChatting2->SendChatMessage( AGPDCHATTING_TYPE_NORMAL , ( CHAR * ) pMessage , strlen( pMessage ) , NULL );
	}
	return 0;
}

static luaDef MVLuaGlue[] = 
{
	{"PushChatMessage"		,	LG_PushChatMessage		},
	{"SendChatMessage"		,	LG_SendChatMessage		},
	{NULL					,	NULL					},
};

void	AgcmChatting2::OnLuaInitialize( AuLua * pLua )
{
	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
}

BOOL AgcmChatting2::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmChatting		= (AgpmChatting *)		GetModule("AgpmChatting");
	m_pcsAgpmGuild			= (AgpmGuild *)			GetModule("AgpmGuild");
	m_pcsAgpmAreaChatting	= (AgpmAreaChatting *)	GetModule("AgpmAreaChatting");

	m_pcsAgcmCharacter		= (AgcmCharacter *)		GetModule("AgcmCharacter");
	m_pcsAgcmTextBoardMng		= (AgcmTextBoardMng *)		GetModule("AgcmTextBoardMng");
	m_pcsAgcmUIManager2		= (AgcmUIManager2 *)	GetModule("AgcmUIManager2");
	m_pcsAgcmFont			= (AgcmFont *)			GetModule("AgcmFont");
	m_pcsAgcmEventEffect	= (AgcmEventEffect *)	GetModule("AgcmEventEffect");
	m_pcsAgcmGuild			= (AgcmGuild *)			GetModule("AgcmGuild");
	m_pcsAgcmConnectManager	= (AgcmConnectManager *)	GetModule("AgcmConnectManager");
	m_pcsApmMap				= (ApmMap*	)			GetModule( "ApmMap" );

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmChatting ||
		!m_pcsAgpmGuild ||
		!m_pcsAgcmCharacter ||
		!m_pcsAgcmUIManager2 ||
		!m_pcsAgcmFont ||
		!m_pcsAgcmGuild ||
		!m_pcsAgpmAreaChatting ||
		!m_pcsAgcmConnectManager)
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackSocialAnimation(CBSocialAnimation, this))
		return FALSE;
	//if (!m_pcsAgpmCharacter->SetCallbackBindingRegionChange(CBChangeRegionIndex, this))
	//	return FALSE;

	if (!m_pcsAgpmChatting->SetCallbackRecvNormalMsg(CBRecvNormalMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvWordBalloonMsg(CBRecvWordBalloonMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvGuildMsg(CBRecvGuildMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvPartyMsg(CBRecvPartyMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvWhisperMsg(CBRecvWhisperMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvWholeWorldMsg(CBRecvWholeWorldMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvSystemLevel1Msg(CBRecvSystemMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvSystemLevel2Msg(CBRecvSystemMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvSystemLevel3Msg(CBRecvSystemMsg, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvEmphasisMsg(CBRecvNormalMsg, this))
		return FALSE;

	if (!m_pcsAgpmChatting->SetCallbackRecvReplyBlockWhisper(CBRecvReplyBlockWhisper, this))
		return FALSE;
	if (!m_pcsAgpmChatting->SetCallbackRecvOfflineWhisperTarget(CBRecvOfflineWhisperTarget, this))
		return FALSE;

	if (!m_pcsAgpmChatting->SetCallbackRecvGuildJointMsg(CBRecvGuildJointMsg, this))
		return FALSE;

	if (!m_pcsAgcmUIManager2->SetCallbackActionMessageChat(CBPutUIEventActionMsg, this))
		return FALSE;

	if (!m_pcsAgpmAreaChatting->SetCallbackRecvSendMessage(CBRecvAreaChatMsg, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmChatting2::OnInit(VOID)
{
	AgcmResourceLoader*	pLoader = (AgcmResourceLoader*)GetModule( "AgcmResourceLoader" );

	g_aulDefaultChatColor[AGPDCHATTING_TYPE_NORMAL]			=	pLoader->GetColor( "NORMAL_CHAT" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_WORD_BALLOON]	=	pLoader->GetColor( "WORD_BALLON" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_UNION]			=	pLoader->GetColor( "UNION_CHAT" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_GUILD]			=	pLoader->GetColor( "GUILD_CHAT" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_PARTY]			=	pLoader->GetColor( "PARTY_CHAT" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_WHISPER]		=	pLoader->GetColor( "WHISPER_SEND" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_WHISPER2]		=	pLoader->GetColor( "WHISPER_RECV" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_WHOLE_WORLD]	=	pLoader->GetColor( "WHOLE_WORLD" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_SYSTEM_LEVEL1]	=	pLoader->GetColor( "SYSTEM_CHAT1" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_SYSTEM_LEVEL2]	=	pLoader->GetColor( "SYSTEM_CHAT2" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_SYSTEM_LEVEL3]	=	pLoader->GetColor( "SYSTEM_CHAT3" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_NOTICE_LEVEL1]	=	pLoader->GetColor( "NOTICE_CHAT1" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_NOTICE_LEVEL2]	=	pLoader->GetColor( "NOTICE_CHAT2" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_GUILD_NOTICE]	=	pLoader->GetColor( "GUILDNOTICE_CHAT" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_GUILD_JOINT]	=	pLoader->GetColor( "GUILDJOINT_CHAT" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_EMPHASIS]		=	pLoader->GetColor( "EMPHASIS_CHAT" );
	g_aulDefaultChatColor[AGPDCHATTING_TYPE_SIEGEWARINFO]	=	pLoader->GetColor( "SIEGEWARINFO_CHAT" );

	return TRUE;
}

BOOL AgcmChatting2::OnIdle(UINT32 ulClockCount)
{
	/*
	if (m_bIsRetryConnectChatServer &&
		ulClockCount >= m_ulRetryConnectTimeMSec)
	{
		m_bIsRetryConnectChatServer	= FALSE;

		if (m_nNID != (-1))
			return TRUE;

		m_pcsAgcmCharacter->m_szGameServerAddress[strlen(m_pcsAgcmCharacter->m_szGameServerAddress) - 1] = '7';

		m_nNID = m_pcsAgcmConnectManager->Connect(m_pcsAgcmCharacter->m_szGameServerAddress, ACDP_SERVER_TYPE_CHATTINGSERVER, this, 
														NULL, NULL, NULL);

		if (m_nNID < 0)
		{
			m_bIsRetryConnectChatServer	= TRUE;
			m_ulRetryConnectTimeMSec	= GetClockCount() + AGCMCHATTING2_INTERVAL_RETRY_CONNECT;
		}
	}
	*/

	if( m_pcamPathWork )
	{
		AgcmCamera2*	pAgcmCamera2 = ( AgcmCamera2* ) GetModule("AgcmCamera2");

		RwMatrix	mat;
		if( !m_pcamPathWork->bOnIdle(mat, AgcdEffGlobal::bGetInst().bGetDiffTimeMS()) )
		{
			pAgcmCamera2->bSetCamFrm(mat);
		}

		if( m_pcamPathWork->GetAccumTime() >= m_pcamPathWork->GetLife() )
		{
			StopCameraWalking();
		}
	}

	return TRUE;
}

VOID AgcmChatting2::ClearChatMessages()
{
	ZeroMemory(m_aszChatMessage, sizeof(CHAR) * AGCMCHATTING_TYPE_MAX * AGCMCHATTING_MAX_CHAT_LIST * (AGCMCHATTING_MAX_CHAT_MESSAGE_LENGTH + 1));
	ZeroMemory(m_aulChatMessageColor, sizeof(UINT32) * AGCMCHATTING_TYPE_MAX * AGCMCHATTING_MAX_CHAT_LIST);
	ZeroMemory(m_aulChatMessageTime, sizeof(UINT32) * AGCMCHATTING_TYPE_MAX * AGCMCHATTING_MAX_CHAT_LIST);
	ZeroMemory(m_aulChatMessageCount, sizeof(UINT32) * AGCMCHATTING_TYPE_MAX);
	ZeroMemory(m_abChatMessageApplyColorAllText, sizeof(BOOL) * AGCMCHATTING_TYPE_MAX * AGCMCHATTING_MAX_CHAT_LIST);

	for (int i = 0; i < AGCMCHATTING_TYPE_MAX; ++i)
		m_abChatMessageEnable[i]	= TRUE;

	m_nNID	= (-1);
	m_ulLastSendAreaMessageMSec	= 0;

	memset(m_szLastWhisperCharacter, 0, sizeof(m_szLastWhisperCharacter));
}

BOOL AgcmChatting2::AddMessage(AgcmChattingType eChatType, CHAR *szMessage, UINT32 ulColor, BOOL bAddForce, BOOL bApplyAllTextColor, UINT32 ulBGColor, BOOL bBold)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX || !szMessage)
		return FALSE;

	if (!bAddForce && !m_abChatMessageEnable[eChatType])
		return TRUE;

	if (eChatType == AGCMCHATTING_TYPE_NOTICE ||
		eChatType == AGCMCHATTING_TYPE_SYSTEM)
		eChatType = AGCMCHATTING_TYPE_SYSTEM;
	else if (eChatType != AGCMCHATTING_TYPE_SIEGEWARINFO)
		eChatType = AGCMCHATTING_TYPE_NORMAL;

	if (m_aulChatMessageCount[eChatType] == AGCMCHATTING_MAX_CHAT_LIST)
	{
		CopyMemory(&m_aszChatMessage[eChatType][0][0], &m_aszChatMessage[eChatType][1][0], sizeof(CHAR) * (AGCMCHATTING_MAX_CHAT_MESSAGE_LENGTH + 1) * (AGCMCHATTING_MAX_CHAT_LIST - 1));
		ZeroMemory(&m_aszChatMessage[eChatType][AGCMCHATTING_MAX_CHAT_LIST - 1][0], sizeof(CHAR) * (AGCMCHATTING_MAX_CHAT_MESSAGE_LENGTH + 1));

		CopyMemory(&m_aulChatMessageColor[eChatType][0], &m_aulChatMessageColor[eChatType][1], sizeof(UINT32) * (AGCMCHATTING_MAX_CHAT_LIST - 1));
		ZeroMemory(&m_aulChatMessageColor[eChatType][AGCMCHATTING_MAX_CHAT_LIST - 1], sizeof(UINT32));

		CopyMemory(&m_abChatMessageApplyColorAllText[eChatType][0], &m_abChatMessageApplyColorAllText[eChatType][1], sizeof(BOOL) * (AGCMCHATTING_MAX_CHAT_LIST - 1));
		ZeroMemory(&m_abChatMessageApplyColorAllText[eChatType][AGCMCHATTING_MAX_CHAT_LIST - 1], sizeof(BOOL));

		CopyMemory(&m_aulChatMessageTime[eChatType][0], &m_aulChatMessageTime[eChatType][1], sizeof(UINT32) * (AGCMCHATTING_MAX_CHAT_LIST - 1));
		ZeroMemory(&m_aulChatMessageTime[eChatType][AGCMCHATTING_MAX_CHAT_LIST - 1], sizeof(UINT32));
	}
	else
	{
		++m_aulChatMessageCount[eChatType];
	}

	INT32	nIndex			= 0;
	INT32	nPixelLength	= 0;
	INT32	nLineIndex		= 0;
	UINT32	ulClockCount	= GetClockCount();

	m_abChatMessageBold[eChatType][m_aulChatMessageCount[eChatType] - 1]	= bBold;
	m_aulChatMessageColor[eChatType][m_aulChatMessageCount[eChatType] - 1]	= ulColor;
	m_aulChatBackgroundColor[eChatType][m_aulChatMessageCount[eChatType] - 1]	= ulBGColor;
	m_abChatMessageApplyColorAllText[eChatType][m_aulChatMessageCount[eChatType] - 1]	= bApplyAllTextColor;
	m_aulChatMessageTime[eChatType][m_aulChatMessageCount[eChatType] - 1]	= ulClockCount;
	strncpy(m_aszChatMessage[eChatType][m_aulChatMessageCount[eChatType] - 1], szMessage, AGCMCHATTING_MAX_CHAT_MESSAGE_LENGTH);

	return TRUE;
}

void AgcmChatting2::TranslateChatMessage(CHAR *szDest, CHAR *szSrc, INT32 lLength)
{
	INT32	lIndex;

	for (lIndex = 0; lIndex < lLength; ++lIndex, ++szDest)
	{
		if (szSrc[lIndex] == '<')
		{
			*szDest = '<';
			++szDest;
		}

		*szDest = szSrc[lIndex];
	}

	*szDest = 0;
}

BOOL AgcmChatting2::AddChatMessage(AgcmChattingType eChatType, AgpdChatData * pstChatData, BOOL bIsSendWhisper, BOOL bTranslateMessage)
{
	if ( eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX || !pstChatData)		return FALSE;

	if ( pstChatData->lMessageLength < 1 )											return FALSE;

	if ( eChatType == AGCMCHATTING_TYPE_NORMAL &&
		(!pstChatData->szSenderName ||!pstChatData->szSenderName[0]) )
	{
		return FALSE;
	}


	CHAR	szBuffer[1024];
	ZeroMemory(szBuffer, sizeof(CHAR) * 1024);

	UINT32	ulBGColor	= 0x00000000;
	UINT32	ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_NORMAL];
	BOOL	bBold = FALSE;
	CHAR *	szMessage = NULL;

	switch (eChatType) 
	{
	case AGCMCHATTING_TYPE_NORMAL:
		sprintf(szBuffer, "%s : ", pstChatData->szSenderName);
		if (bTranslateMessage)
		{
			TranslateChatMessage(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		}
		else
		{
			CopyMemory(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		}
		break;

	case AGCMCHATTING_TYPE_PARTY:
		szMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_PARTY_HEADER);
		if (szMessage)
		{
			sprintf(szBuffer, "%s ", szMessage);
			sprintf(szBuffer + strlen(szBuffer), "%s : ", pstChatData->szSenderName);
		}
		if (bTranslateMessage)
		{
			TranslateChatMessage(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		}
		else
		{
			CopyMemory(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		}
		ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_PARTY];
		break;

	case AGCMCHATTING_TYPE_GUILD:
		szMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_GUILD_HEADER);
		if (szMessage)
		{
			sprintf(szBuffer, "%s", szMessage);

			if( m_pcsAgpmCharacter )
			{
				AgpdCharacter* pMyCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
				if( pMyCharacter )
				{
					AgpmGuild* pGuild = ( AgpmGuild* )GetModule( "AgpmGuild" );
					if( pGuild )
					{
						CHAR* pGuildName = pGuild->GetJoinedGuildID( pMyCharacter );
						if( pGuildName )
						{
							sprintf(szBuffer + strlen(szBuffer), "[%s] ", pGuildName);
						}
					}
				}
			}

			sprintf(szBuffer + strlen(szBuffer), "%s : ", pstChatData->szSenderName);
		}
		if (bTranslateMessage)
		{
			TranslateChatMessage(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		}
		else
		{
			CopyMemory(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		}

		if(pstChatData->eChatType == AGPDCHATTING_TYPE_GUILD_NOTICE)	ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_GUILD_NOTICE];
		else															ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_GUILD];

		break;

	case AGCMCHATTING_TYPE_GUILD_JOINT:
		szMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_GUILD_JOINT_HEADER);
		if (szMessage)
		{
			sprintf(szBuffer, "%s ", szMessage);
			sprintf(szBuffer + strlen(szBuffer), "%s : ", pstChatData->szSenderName);
		}
		CopyMemory(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_GUILD_JOINT];
		break;

	case AGCMCHATTING_TYPE_WHISPER:
		if (bIsSendWhisper)
		{
			sprintf(szBuffer, "%s ", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_WHISPER_HEADER2));
			sprintf(szBuffer + strlen(szBuffer), "%s : ", pstChatData->szTargetName);
			ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_WHISPER2];
		}
		else
		{
			sprintf(szBuffer, "%s ", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_WHISPER_HEADER));
			sprintf(szBuffer + strlen(szBuffer), "%s : ", pstChatData->szSenderName);
			ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_WHISPER];
		}

		if (bTranslateMessage)
			TranslateChatMessage(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		else
			CopyMemory(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		break;

	case AGCMCHATTING_TYPE_NOTICE:
		sprintf(szBuffer, "%s ", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_NOTICE_HEADER));
		if (bTranslateMessage)
			TranslateChatMessage(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		else
			CopyMemory(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_NOTICE_LEVEL1];
		break;

	case AGCMCHATTING_TYPE_SYSTEM:
		if (bTranslateMessage)
			TranslateChatMessage(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		else
			CopyMemory(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_SYSTEM_LEVEL1];
		break;

	case AGCMCHATTING_TYPE_EMPHASIS:
		sprintf(szBuffer, "%s : ", pstChatData->szSenderName);
		if (bTranslateMessage)
			TranslateChatMessage(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		else
			CopyMemory(szBuffer + strlen(szBuffer), pstChatData->szMessage, sizeof(CHAR) * pstChatData->lMessageLength);
		ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_EMPHASIS];
		ulBGColor = 0xff534cb1;
		break;

	case AGCMCHATTING_TYPE_SIEGEWARINFO:
		sprintf(szBuffer, "%s : ", pstChatData->szMessage);
		ulColor	= g_aulDefaultChatColor[AGPDCHATTING_TYPE_NORMAL];
		break;
	}

	AddMessage(eChatType, szBuffer, ulColor, FALSE, FALSE, ulBGColor, bBold);

	return TRUE;
}

BOOL AgcmChatting2::AddSystemMessage(CHAR *szMessage, UINT32 ulColor)
{
	if (!szMessage)
		return FALSE;

	AddMessage(AGCMCHATTING_TYPE_SYSTEM, szMessage, ulColor);


	return TRUE;
}

BOOL AgcmChatting2::AddSystemMessage(CHAR *szMessage)
{
	if (!szMessage)
		return FALSE;

	AddSystemMessage(szMessage, g_aulDefaultChatColor[AGPDCHATTING_TYPE_SYSTEM_LEVEL1]);

	EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
}

BOOL AgcmChatting2::AddAreaChatMessage(CHAR *szMessage, INT32 lMessageLength, BOOL bIsAdminCharacter, BOOL bIsAddHeader, UINT32 ulTextColorRGB)
{
	if (!szMessage || !szMessage[0] || lMessageLength < 1)
		return FALSE;

	CHAR	szBuffer[1100];
	ZeroMemory(szBuffer, sizeof(CHAR) * 1100);

	if (bIsAddHeader)
		sprintf(szBuffer, "%s ", m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_AREA_CHAT_HEADER));
	CopyMemory(szBuffer + strlen(szBuffer), szMessage, sizeof(CHAR) * lMessageLength);

	if (bIsAdminCharacter)
		AddMessage(AGCMCHATTING_TYPE_NORMAL, szBuffer, g_aulDefaultChatColor[AGPDCHATTING_TYPE_NOTICE_LEVEL1]);
	else
		AddMessage(AGCMCHATTING_TYPE_AREA, szBuffer, (ulTextColorRGB > 0) ? ulTextColorRGB : g_aulDefaultChatColor[AGPDCHATTING_TYPE_WORD_BALLOON], FALSE, (ulTextColorRGB > 0) ? TRUE : FALSE);

	EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
}

BOOL AgcmChatting2::AddSiegeWarMessage(CHAR *szMessage)
{
	if (!szMessage)
		return FALSE;

	AddMessage(AGCMCHATTING_TYPE_SIEGEWARINFO, szMessage, g_aulDefaultChatColor[AGPDCHATTING_TYPE_SIEGEWARINFO]);

	EnumCallback(AGCMCHATTING2_CB_INPUT_SIEGEWAR_MESSAGE, NULL, NULL);

	return TRUE;
}

void	AgcmChatting2::StopCameraWalking()
{
	DEF_SAFEDELETE(m_pcamPathWork);
}

BOOL AgcmChatting2::ParseSystemCommand(CHAR *szMessage, INT32 lLength)
{
	if (!szMessage || !szMessage[0] || lLength < 1)
		return FALSE;

	// social animation이 있는지 본다.
	int startIndex = 0;
	int endIndex = 0;
	if( m_pcsAgpmCharacter->IsGM( m_pcsAgcmCharacter->GetSelfCharacter() ) )
	{
		startIndex = AGPDCHAR_SOCIAL_TYPE_GM_GREETING;
		endIndex = AGPDCHAR_SOCIAL_TYPE_GM_HAPPY;
	}
	else if( m_pcsAgpmCharacter->IsPC( m_pcsAgcmCharacter->GetSelfCharacter() ) )
	{
		startIndex = AGPDCHAR_SOCIAL_TYPE_GREETING;
		endIndex = AGPDCHAR_SOCIAL_TYPE_SPECIAL1;
	}
	
	for (int i = startIndex; i <= endIndex; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			if (g_aszSocialAnimationCommand[i][j] &&
				strncmp(szMessage + 1, g_aszSocialAnimationCommand[i][j], strlen(g_aszSocialAnimationCommand[i][j])) == 0)
			{
				if (FALSE == m_pcsAgcmCharacter->SendSocialAnimation(i))
				{
					ApSafeArray<CHAR, 255> buffer;
					buffer.MemSetAll();

					if ( AP_SERVICE_AREA_KOREA == g_eServiceArea )
						sprintf(&buffer[0], "'%s' 애니메이션을 수행 할 수 없습니다.", g_aszSocialAnimationCommand[i][0]);
					else
						sprintf(&buffer[0], "You can not use '%s'", g_aszSocialAnimationCommand[i][0]);
					
					AddSystemMessage(&buffer[0]);
				}

				return FALSE;
			}
		}
	}

#ifdef _DEBUG
	if(!strcmp(szMessage +1,"battlemsg"))
	{
		AgcmUISiegeWar* pcmUISiegeWar = ( AgcmUISiegeWar* )GetModule( "AgcmUISiegeWar" );
		pcmUISiegeWar->OpenSiegeWarInfo();
		pcmUISiegeWar->SetInfoEdit( "배틀그라운드 알림메세지 테스트용입니다. 생계형 아이돌 카라를 찬양하라~~! 하라야 다음에는 콘돔을 끼구하라!", 0xFFFFFFFF );
		return FALSE;
	}
#endif

	if(!strcmp(szMessage +1,"지평선"))
	{
		AgcmRender*	pRender = (AgcmRender*) GetModule("AgcmRender");
		pRender->m_bDrawOcLineDebug = !pRender->m_bDrawOcLineDebug;
		return FALSE;
	}

	else if(!strcmp(szMessage +1,"reloadtitle"))
	{
		AgcmTitle* pcmTitle = GetAgcmTitle();
		if( pcmTitle )
		{
			pcmTitle->OnLoadTitleSetting( "INI\\TitleSetting.xml" );
		}
	}

	else if( !strcmp(szMessage+1, "camera 1") )
	{
		// 1인칭 카메라 모드
		AgcmCamera2*	pAgcmCamera2 = ( AgcmCamera2* ) GetModule("AgcmCamera2");
		if(pAgcmCamera2)
			AgcuCamMode::bGetInst().bChageMode( *pAgcmCamera2, AgcuCamMode::eMode_1stPerson );
	}
	else if( !strcmp(szMessage+1, "camera 3") )
	{
		// 3인칭 카메라 모드
		AgcmCamera2*	pAgcmCamera2 = ( AgcmCamera2* ) GetModule("AgcmCamera2");
		if(pAgcmCamera2)
			AgcuCamMode::bGetInst().bChageMode( *pAgcmCamera2, AgcuCamMode::eMode_Free );
	}
	
	if( szMessage[ 0 ] == '/' || 
		(AP_SERVICE_AREA_JAPAN == g_eServiceArea && szMessage[ 0 ] == -127 && szMessage[ 1 ] == 69 )	// 일본어 전각 슬러시.
		)
	{
		const	INT32	cMaxBuffer = 1023;
		char	szConverted[ cMaxBuffer + 1 ] = "";
		INT32	lConvertedLength = 0;

		if( AP_SERVICE_AREA_JAPAN == g_eServiceArea )
		{
			// 전각 스페이스바 , 전각 슬러시를 동일하게 처리하기 위해
			// 반각으로 변환
			int i = 0;
			if( szMessage[ 0 ] == '/' )
			{
				i++;
				szConverted[ lConvertedLength ++ ] = '/';
			}
			else if( szMessage[ 0 ] == -127 && szMessage[ 1 ] == 69 )
			{
				i += 2;
				szConverted[ lConvertedLength ++ ] = '/';
			}

			BOOL	bFirst = TRUE;
			for(; i < lLength ; i ++ )
			{
				// 혹시나를 위한 프로텍션..
				if( lConvertedLength >= cMaxBuffer ) break;

				if(	bFirst							&&
					szMessage[ i		] == -127	&&
					szMessage[ i + 1	] == 64		)
				{
					bFirst = FALSE;	// 최초에 녀석만 스페이스로 변환..
					// 전각 스페이스
					szConverted[ lConvertedLength ++ ] = ' ';
					i++;	// 한번 더 증가함
				}
				else
				{
					szConverted[ lConvertedLength ++ ] = szMessage[ i ];
				}
			}

			szConverted[ lConvertedLength ++ ] = '\0';

			// 대치시킴..
			szMessage	= szConverted		;
			lLength		= lConvertedLength	;
		}
		
		//// 2005.02.17. steeple
		//if(strcmp(szMessage, AGPMCHAT_COMMAND_GUILD_LEAVE) == 0 ||
		//	strcmp(szMessage, AGPMCHAT_COMMAND_GUILD_LEAVE_ENG) == 0)
		//{
		//	return EnumCallback(AGCMCHATTING2_CB_COMMAND_GUILD_LEAVE, NULL, NULL);
		//}

		CGetArg2	arg;
		arg.SetParam( szMessage , " \t\n");
		
		char	strCommand[ 256 ];
		strncpy( strCommand , arg.GetParam( 0 ) , 256 );

		int nLen = 0;
		while( nLen < ( int ) strlen( strCommand ) )
		{
			strCommand[ nLen ] = toupper( strCommand[ nLen ] );
			nLen ++;
		}

		if( !strcmp( strCommand , "/CW" ) )
		{
			//UINT32	uLoop = AgcmCamera2::AGCMCAMERA_CW_PLAY;
			AgcmCamera2*	pAgcmCamera2 = ( AgcmCamera2* ) GetModule("AgcmCamera2");
			StopCameraWalking();

			// 카메라워킹..
			if( arg.GetArgCount() < 2 )
			{
				// 인자부족..
				// 카메라워킹 스톱시킴..
				// pAgcmCamera2->PlayCameraWalkFile( NULL , AgcmCamera2::AGCMCAMERA_CW_STOP );
				// AddSystemMessage( "인자부족 두번째는 파일이름 을 넣으세요~ ( a.saf 면 'a' 만 입력 )" , 0xffff0000 );

				if( pAgcmCamera2 )
				{
					pAgcmCamera2->bSetIdleType( AgcmCamera2::e_idle_default );
				}
			}
			else
			{
				/*
				if( arg.GetArgCount() >= 3 )
				{
					uLoop = atoi( arg.GetParam( 2 ) );

					if( AgcmCamera2::AGCMCAMERA_CW_STOP == uLoop )
						uLoop = AgcmCamera2::AGCMCAMERA_CW_PLAY;
					if( AgcmCamera2::AGCMCAMERA_CW_LOOP < uLoop )
						uLoop = AgcmCamera2::AGCMCAMERA_CW_LOOP;
				}

				*/
				char strFilename[ 256 ];
				wsprintf( strFilename , "%s.sab" , arg.GetParam( 1 ) );

				m_pcamPathWork = AgcuPathWork::CreateSplinePath( strFilename );

				// 카메라 모드 설정 변경.
				pAgcmCamera2->bSetIdleType( AgcmCamera2::e_idle_login );
			}

			return FALSE;
		}
		else if( !strcmp( strCommand , "/MC" ) )
		{
			if (_CrtCheckMemory())
				AddSystemMessage( "메모리 OK~" , 0xffff0000 );
			else
				AddSystemMessage( "메모리에 문제 있음!!!!!!!!1" , 0xffff0000 );
		}
		else if( !stricmp( strCommand, "/reportclientall"))
		{
			ApModuleManager *pManager = (ApModuleManager *) GetModuleManager();
			if (pManager)
				pManager->ReportAll();
		}
		else if( !stricmp( strCommand, "/reportmodule"))
		{
			if( arg.GetArgCount() > 1)
			{
				ApModuleManager *pManager = (ApModuleManager *) GetModuleManager();
				if (pManager)
					pManager->ReportModule(arg.GetParam(1));
			}
		}
#ifdef _DEBUG
		else if( !stricmp( strCommand, "/flag"))
		{
			if( arg.GetArgCount() > 1)
			{
				AgcmTextBoardMng* pcmTextBoardMng = ( AgcmTextBoardMng* )GetModule( "AgcmTextBoardMng" );
				if( pcmTextBoardMng )
				{
					char* pParam = arg.GetParam( 1 );
					if( pParam && strlen( pParam ) > 0 )
					{						
						if( strcmp( pParam, "reload" ) == 0 )
						{
							pcmTextBoardMng->ReloadFlag( "INI\\FlagImoticon.xml" );
						}
					}
				}
			}
		}
#endif
		else 
		{
			AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )GetModule( "AgcmUIManager2" );
			if( pcmUIManager )
			{
				char* pCmdAddFilter = pcmUIManager->GetUIMessage( "AddTextFilter" );
				if( pCmdAddFilter && strlen( pCmdAddFilter ) > 0 )
				{
					if( !stricmp( strCommand, pCmdAddFilter ) )
					{
						if( arg.GetArgCount() > 1)
						{
							AgcmChatting2* pcmChatting = ( AgcmChatting2* )GetModule( "AgcmChatting2" );
							if( pcmChatting )
							{
								char* pParam = arg.GetParam( 1 );
								if( pParam && strlen( pParam ) > 0 )
								{						
									pcmChatting->OnAddFilter( pParam, TRUE, TRUE );
								}
							}
						}
					}
				}
			}
		}

		if( EnumCallback(AGCMCHATTING2_CB_COMMAND_CALLBACK, strCommand , &arg ) )
		{
			// do nothing.
			// 메시지를 보내지 않음..
			return FALSE;
		}
		else
		{
			// 처리하지 못했기땜에 암것도 안함..
		}
	}
	
	return TRUE;
}

BOOL AgcmChatting2::CBSocialAnimation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmChatting2		*pThis				= (AgcmChatting2 *)		pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	UINT8				ucSocialAnimation	= *(UINT8 *)			pCustData;

	if (pThis->m_pcsAgcmEventEffect)
	{
		pThis->m_pcsAgcmEventEffect->StartSocialAnimation(pcsCharacter, (AgpdCharacterSocialType) ucSocialAnimation);
	}

	return TRUE;
}

/*
BOOL AgcmChatting2::CBChangeRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmChatting2			*pThis				= (AgcmChatting2 *)			pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsCharacter)
		return TRUE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= pThis->m_pcsAgpmAreaChatting->MakePacketChangeRegion(pcsCharacter->m_nBindingRegionIndex, &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= pThis->SendPacket(pvPacket, nPacketLength, ACDP_SEND_CHATTINGSERVER);

	pThis->m_pcsAgpmAreaChatting->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}
*/

BOOL AgcmChatting2::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmChatting2			*pThis				= (AgcmChatting2 *)			pClass;
	AgpdCharacter			*pcsCharacter		= (AgpdCharacter *)			pData;

	pThis->ClearChatMessages();

	/*
	pThis->m_bIsRetryConnectChatServer	= FALSE;

	if (pThis->m_nNID != (-1))
		return TRUE;

	pThis->m_pcsAgcmCharacter->m_szGameServerAddress[strlen(pThis->m_pcsAgcmCharacter->m_szGameServerAddress) - 1] = '7';

	pThis->m_nNID = pThis->m_pcsAgcmConnectManager->Connect(pThis->m_pcsAgcmCharacter->m_szGameServerAddress, ACDP_SERVER_TYPE_CHATTINGSERVER, pThis, 
													NULL, NULL, NULL);

	if (pThis->m_nNID < 0)
	{
		pThis->m_bIsRetryConnectChatServer	= TRUE;
		pThis->m_ulRetryConnectTimeMSec	= pThis->GetClockCount() + AGCMCHATTING2_INTERVAL_RETRY_CONNECT;

		return FALSE;
	}
	*/

	return TRUE;
}

//		SendChatMessage
//	Functions
//		- chatting message를 서버로 보낸다.
//	Arguments
//		- eType			:	AGPDCHATTING_TYPE_NORMAL
//							AGPDCHATTING_TYPE_GUILD
//							AGPDCHATTING_TYPE_PARTY
//							AGPDCHATTING_TYPE_WHISPER
//		- szMessage		:	chat message
//		- lLength		:	message length
//		- szTargetID	:	귓속말인 경우 상대방 아뒤
//	Return value
//		- BOOL : 성공여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgcmChatting2::SendChatMessage(AgpdChattingType eType, CHAR *szMessage, INT32 lLength,
								   CHAR *szTargetID)
{
	if (!szMessage || !strlen(szMessage) || lLength < 1)
		return FALSE;

	AgpdCharacter *pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	PVOID	pvPacket		= NULL;
	INT16	nPacketLength	= 0;

	if (lLength > AGCMCHATTING_MAX_CHAT_STRING_LENGTH)
		lLength	= AGCMCHATTING_MAX_CHAT_STRING_LENGTH;

	if (!ParseSystemCommand(szMessage, lLength))
		return FALSE;

	OnFiltering( szMessage );

	BOOL	bSendResult	= FALSE;
	int		i;

	for (i = 0; i < lLength; ++i)
		if (szMessage[i] != ' ')
			break;

	if ((strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_WORD_BALLOON_LOWER), strlen(ClientStr().GetStr(STI_CHAT_WORD_BALLOON_LOWER))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_WORD_BALLOON_LOWER))] == ' ') ||
		(strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_WORD_BALLOON_UPPER), strlen(ClientStr().GetStr(STI_CHAT_WORD_BALLOON_UPPER))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_WORD_BALLOON_UPPER))] == ' ') ||
		(strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_WORD_BALLOON_LOCALIZE), strlen(ClientStr().GetStr(STI_CHAT_WORD_BALLOON_LOCALIZE))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_WORD_BALLOON_LOCALIZE))] == ' ') ||
		ClientStr().GetStr(STI_CHAT_NORMAL_AREA_CHAT_SPECIAL) &&
		szMessage[i] == ClientStr().GetStr(STI_CHAT_NORMAL_AREA_CHAT_SPECIAL)[0])
	{
		if (m_ulLastSendAreaMessageMSec + AGCMCHATTING2_INTERVAL_AREA_MESSAGE > GetClockCount())
			return TRUE;

		CHAR	szBuffer[1024];
		ZeroMemory(szBuffer, sizeof(szBuffer));

		//sprintf(szBuffer, "[외치기] %s : ", pcsSelfCharacter->m_szID);

		if (szMessage[i] == '/')
		{
			CopyMemory(szBuffer + strlen(szBuffer),
					   szMessage + i + strlen(ClientStr().GetStr(STI_CHAT_WORD_BALLOON_LOWER)) + 1,
					   sizeof(CHAR) * (lLength - (i + strlen(ClientStr().GetStr(STI_CHAT_WORD_BALLOON_LOWER)) + 1)));
		}
		else
		{
			CopyMemory(szBuffer + strlen(szBuffer),
					   szMessage + i + 1,
					   sizeof(CHAR) * (lLength - (i + 1)));
		}

		pvPacket	= m_pcsAgpmAreaChatting->MakePacketSendMessage(pcsSelfCharacter->m_lID, 0, 1, 0, szBuffer, strlen(szBuffer), &nPacketLength); 

		if (!pvPacket || nPacketLength < 1)
			return FALSE;

		bSendResult	= SendPacket(pvPacket, nPacketLength);

		m_pcsAgpmAreaChatting->m_csPacket.FreePacket(pvPacket);

		m_ulLastSendAreaMessageMSec	= GetClockCount();
	}
	//. 2006. 1. 6. Nonstopdj /JOIN, /LEAVE 커맨드 추가
	//. join channel command
	else if((strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_LOWER), strlen(ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_LOWER))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_LOWER))] == ' ') ||
		(strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_UPPER), strlen(ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_UPPER))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_UPPER))] == ' ') ||
		 (strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_CUSTOM), strlen(ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_CUSTOM))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_CUSTOM))] == ' ') ||
		(strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_LOCALIZE), strlen(ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_LOCALIZE))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_JOIN_CHANNEL_LOCALIZE))] == ' '))
	{
		//. todo. join channel 패킷날리기
		AgcmUIChannel* pChannel = (AgcmUIChannel*)GetModule("AgcmUIChannel");
		if(pChannel)
		{
			string strTemp = string(szMessage);
			INT32 nIdx = strTemp.find(" ") + 1;

			pChannel->SendPacketJoinChannel(_T(szMessage + nIdx));
		}
	}
	//. leave channel command
	else if((strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_LOWER), strlen(ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_LOWER))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_LOWER))] == ' ') ||
		(strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_UPPER), strlen(ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_UPPER))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_UPPER))] == ' ') ||
		 (strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_CUSTOM), strlen(ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_CUSTOM))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_CUSTOM))] == ' ') ||
		(strncmp(szMessage + i, ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_LOCALIZE), strlen(ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_LOCALIZE))) == 0 &&
		 szMessage[i + strlen(ClientStr().GetStr(STI_CHAT_LEAVE_CHANNEL_LOCALIZE))] == ' '))
	{
		//. todo. leave channel 패킷날리기
		AgcmUIChannel* pChannel = (AgcmUIChannel*)GetModule("AgcmUIChannel");

		if(pChannel)
		{
			string strTemp = string(szMessage);
			INT32 nIdx = strTemp.find(" ") + 1;

			pChannel->SendPacketLeaveChannel(_T(szMessage + nIdx));
		}
	}
	else
	{
		switch (eType) {
		case AGPDCHATTING_TYPE_NORMAL:
			{
				pvPacket = m_pcsAgpmChatting->MakePacketChatting(pcsSelfCharacter->m_lID,
																AGPDCHATTING_TYPE_NORMAL,
																NULL,
																NULL,
																szMessage,
																lLength,
																&nPacketLength);
			}
			break;

		case AGPDCHATTING_TYPE_PARTY:
			{
				pvPacket = m_pcsAgpmChatting->MakePacketChatting(pcsSelfCharacter->m_lID,
																AGPDCHATTING_TYPE_PARTY,
																NULL,
																NULL,
																szMessage,
																lLength,
																&nPacketLength);
			}
			break;

		case AGPDCHATTING_TYPE_GUILD:
			{
				pvPacket = m_pcsAgpmChatting->MakePacketChatting(pcsSelfCharacter->m_lID,
																AGPDCHATTING_TYPE_GUILD,
																NULL,
																NULL,
																szMessage,
																lLength,
																&nPacketLength);
			}
			break;

		case AGPDCHATTING_TYPE_WHISPER:
			{
				pvPacket = m_pcsAgpmChatting->MakePacketChatting(pcsSelfCharacter->m_lID,
																AGPDCHATTING_TYPE_WHISPER,
																NULL,
																szTargetID,
																szMessage,
																lLength,
																&nPacketLength);
			}
			break;
			
		default:
			return FALSE;
			break;
		}

		if (!pvPacket || nPacketLength < 1)
			return FALSE;

		bSendResult = SendPacket(pvPacket, nPacketLength);

		m_pcsAgpmChatting->m_csPacket.FreePacket(pvPacket);
	}

	return bSendResult;
}

BOOL AgcmChatting2::CBRecvNormalMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmChatting2		*pThis			= (AgcmChatting2 *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	AgpdCharacter* ppdsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter( pstChatData->lSenderID );
	if(!ppdsCharacter)
		return FALSE;

	AgcdCharacter* pstAgcdCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData( ppdsCharacter );
	if(!pstAgcdCharacter)
		return FALSE;

	if (pstChatData->szMessage && pstChatData->szMessage[0] == '/')
		return TRUE;

	if (pstChatData->eChatType == AGPDCHATTING_TYPE_NORMAL)
		pThis->AddChatMessage(AGCMCHATTING_TYPE_NORMAL, pstChatData, FALSE, TRUE);
	else if (pstChatData->eChatType == AGPDCHATTING_TYPE_EMPHASIS)
		pThis->AddChatMessage(AGCMCHATTING_TYPE_EMPHASIS, pstChatData, FALSE, TRUE);

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, pstChatData, NULL);

	if ((pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED) || !(pstAgcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_INIT_COMPLETE))
		return TRUE;

	if (pThis->m_pcsAgcmTextBoardMng)
	{
		char* pChatMessage	= pstChatData->szMessage;

		pChatMessage[ pstChatData->lMessageLength ]	= 0;
		sBoardAttr Attr;
		Attr.eType   = AGCM_BOARD_TEXT;   Attr.String = pChatMessage;  Attr.Color = 0xffffffff;
		Attr.ViewCount = 7000;    Attr.fType   = 0;

		pThis->m_pcsAgcmTextBoardMng->CreateBoard( pstChatData->lSenderID , &Attr );
	}

	return TRUE;
}

BOOL AgcmChatting2::CBRecvWordBalloonMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmChatting2		*pThis			= (AgcmChatting2 *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	// 그려주던 뭘하던 한다. ㅡ.ㅡ....(안날라온다. --ㅋ )
	//pThis->m_pcsAgcmTextBoard->MakeTextBoard(pstChatData->lSenderID,pstChatData->szMessage,0,0xffffffff,50000);

	return TRUE;
}

BOOL AgcmChatting2::CBRecvGuildMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmChatting2		*pThis			= (AgcmChatting2 *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	CHAR* szSelfGuildID = pThis->m_pcsAgcmGuild->GetSelfGuildID();
	if(!szSelfGuildID || strlen(szSelfGuildID) == 0)
		return TRUE;

	// 2005.04.08. steeple
	// 이유가 명확하게 밝혀지진 않았지만 자기 길드가 아닌 메시지가 보이는 경우가 생기는 듯. (게시판에서 발견)
	// 고로 자기 길드 멤버인지 함 확인한 후 아니라면 GG 친다.

	BOOL bSelfGuildOperation = FALSE;
	AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(szSelfGuildID);
	if(pcsGuild)
	{
		// 메시지 보낸 놈이 자기 길드 멤버인지 확인
		if(pThis->m_pcsAgpmGuild->GetMember(pcsGuild, pstChatData->szSenderName))
			bSelfGuildOperation = TRUE;

		pcsGuild->m_Mutex.Release();
	}

	// 자기길드가 아니면 나간다.
	if(!bSelfGuildOperation)
		return TRUE;

	// 그려주던 뭘하던 한다. ㅡ.ㅡ....
	pThis->AddChatMessage(AGCMCHATTING_TYPE_GUILD, pstChatData, FALSE, TRUE);

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, pstChatData, NULL);

	return TRUE;
}

BOOL AgcmChatting2::CBRecvPartyMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmChatting2		*pThis			= (AgcmChatting2 *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	// 그려주던 뭘하던 한다. ㅡ.ㅡ....
	pThis->AddChatMessage(AGCMCHATTING_TYPE_PARTY, pstChatData, FALSE, TRUE);

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, pstChatData, NULL);

	return TRUE;
}

// 2004.04.06. steeple - 자기가 보낸 것도 받게 된다. 그에 대한 처리 추가.
BOOL AgcmChatting2::CBRecvWhisperMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmChatting2		*pThis			= (AgcmChatting2 *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	// 그려주던 뭘하던 한다. ㅡ.ㅡ....
	//AgpdCharacter* ppdsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter( pstChatData->lSenderID );

	AgpdCharacter* pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	BOOL	bIsSendMessage	= FALSE;
	if (pstChatData->lSenderID == pcsSelfCharacter->m_lID)
	{
		if (!stricmp(pstChatData->szSenderName, pstChatData->szTargetName))
		{
			static BOOL bSelfWhisperSend = true;
			if (bSelfWhisperSend)
				bIsSendMessage = TRUE;;

			bSelfWhisperSend = !bSelfWhisperSend;
		}
		else
			bIsSendMessage = TRUE;
	}

	if (!bIsSendMessage)
		pThis->SetLastWhisperCharacter(pstChatData->szSenderName);

	pThis->AddChatMessage(AGCMCHATTING_TYPE_WHISPER, pstChatData, bIsSendMessage, TRUE);

	/*
	// 남이 보낸 것을 받았다.
	if(strncmp(pcsSelfCharacter->m_szID, pstChatData->szSenderName, AGPACHARACTER_MAX_ID_STRING) != 0)
		pThis->m_clChatWindow.PutMessage( AGPDCHATTING_TYPE_WHISPER, pstChatData->szSenderName, pstChatData );
	else
	{	// 자기가 보낸 것을 받았다.
		if(!pstChatData->szTargetID[0])
			return FALSE;

		// 새롭게 메시지 문자열을 만든다.
		CHAR* szOldMessage = pstChatData->szMessage;
		INT32 lOldMessageLength = pstChatData->lMessageLength;

		INT16 nTargetNameLength = strlen(pstChatData->szTargetID[0]);
		INT32 lNewMessageLength = lOldMessageLength + nTargetNameLength + 3;	// "(", ")" 하고 널문자. 널문자는 없어도 되지만, 그냥 넣었음.
		CHAR* szNewMessage = new CHAR[lNewMessageLength];

		memset(szNewMessage, 0, lNewMessageLength);
		strcpy(szNewMessage, "(");
		strncat(szNewMessage, pstChatData->szTargetID[0], nTargetNameLength);
		strcat(szNewMessage, ")");
		memcpy(szNewMessage + strlen(szNewMessage), pstChatData->szMessage, pstChatData->lMessageLength);

		// 새로 만든 문자열 대입
		pstChatData->szMessage = szNewMessage;
		pstChatData->lMessageLength = lNewMessageLength;

		pThis->m_clChatWindow.PutMessage( AGPDCHATTING_TYPE_WHISPER2, pstChatData->szSenderName, pstChatData);

		// 원래 대로 복귀
		pstChatData->szMessage = szOldMessage;
		pstChatData->lMessageLength = lOldMessageLength;

		delete [] szNewMessage;
	}
	*/

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, pstChatData, NULL);

	return TRUE;
}

// 2003.11.18. steeple
BOOL AgcmChatting2::CBRecvWholeWorldMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdChatData * pstChatData = (AgpdChatData *)pData;
	AgcmChatting2* pThis = (AgcmChatting2*)pClass;

	if(!pstChatData || !pClass)
		return FALSE;

	// Character 는 얻지 않는다.
	pThis->AddChatMessage(AGCMCHATTING_TYPE_NOTICE, pstChatData, FALSE, TRUE);

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, pstChatData, NULL);

	return TRUE;
}

// 2003.12.8. steeple
BOOL AgcmChatting2::CBRecvSystemMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdChatData * pstChatData = (AgpdChatData *)pData;
	AgcmChatting2* pThis = (AgcmChatting2*)pClass;

	if(!pstChatData || !pClass)
		return FALSE;

	// Character 는 얻지 않는다.
	pThis->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, pstChatData, FALSE, TRUE);

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, pstChatData, NULL);

	return TRUE;
}

// 2006.08.16. steeple
// 연대 채팅 메시지가 날아왔다.
BOOL AgcmChatting2::CBRecvGuildJointMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmChatting2		*pThis			= (AgcmChatting2 *)		pClass;
	AgpdChatData *		pstChatData		= (AgpdChatData *)		pData;

	pThis->AddChatMessage(AGCMCHATTING_TYPE_GUILD_JOINT, pstChatData);

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, pstChatData, NULL);

	return TRUE;
}

BOOL AgcmChatting2::CBPutUIEventActionMsg(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmChatting2*	pThis = (AgcmChatting2*)pClass	;
	CHAR*			pMessage = (CHAR*)pData			;
	DWORD*			plColor	 = (DWORD*)pCustData	;

	if ( NULL == pThis ) return FALSE;

	SystemMessage.ProcessSystemMessage(pMessage, *plColor);

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
}

BOOL AgcmChatting2::SetCallbackInputMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, pfCallback, pClass);
}

BOOL AgcmChatting2::SetCallbackInputSiegeWarMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHATTING2_CB_INPUT_SIEGEWAR_MESSAGE, pfCallback, pClass);
}

BOOL AgcmChatting2::SetCallbackCommandProcess	(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHATTING2_CB_COMMAND_CALLBACK , pfCallback, pClass);
}

BOOL AgcmChatting2::SetCallbackCommandGuildLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMCHATTING2_CB_COMMAND_GUILD_LEAVE, pfCallback, pClass);
}

CHAR* AgcmChatting2::GetChatMessage(AgcmChattingType eChatType, INT32 lIndex)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX ||
		lIndex < 0 || lIndex >= AGCMCHATTING_MAX_CHAT_LIST)
		return NULL;

	return m_aszChatMessage[eChatType][lIndex];
}

UINT32 AgcmChatting2::GetChatMessageColor(AgcmChattingType eChatType, INT32 lIndex)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX ||
		lIndex < 0 || lIndex >= AGCMCHATTING_MAX_CHAT_LIST)
		return 0xFFFFFFFF;

	return m_aulChatMessageColor[eChatType][lIndex];
}

UINT32 AgcmChatting2::GetChatBackgroundColor(AgcmChattingType eChatType, INT32 lIndex)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX ||
		lIndex < 0 || lIndex >= AGCMCHATTING_MAX_CHAT_LIST)
		return 0xFFFFFFFF;

	return m_aulChatBackgroundColor[eChatType][lIndex];
}

BOOL AgcmChatting2::GetChatMessageBold(AgcmChattingType eChatType, INT32 lIndex)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX ||
		lIndex < 0 || lIndex >= AGCMCHATTING_MAX_CHAT_LIST)
		return 0xFFFFFFFF;

	return m_abChatMessageBold[eChatType][lIndex];
}

BOOL AgcmChatting2::GetChatMessageApplyColorAllText(AgcmChattingType eChatType, INT32 lIndex)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX ||
		lIndex < 0 || lIndex >= AGCMCHATTING_MAX_CHAT_LIST)
		return 0xFFFFFFFF;

	return m_abChatMessageApplyColorAllText[eChatType][lIndex];
}

UINT32 AgcmChatting2::GetChatMessageTime(AgcmChattingType eChatType, INT32 lIndex)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX ||
		lIndex < 0 || lIndex >= AGCMCHATTING_MAX_CHAT_LIST)
		return 0xFFFFFFFF;

	return m_aulChatMessageTime[eChatType][lIndex];
}

UINT32 AgcmChatting2::GetChatMessageCount(AgcmChattingType eChatType)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX)
		return 0;

	return m_aulChatMessageCount[eChatType];
}

BOOL AgcmChatting2::ActiveChatMode(AgcmChattingType eChatType)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX)
		return FALSE;

	m_abChatMessageEnable[eChatType]	= TRUE;

	if (eChatType == AGCMCHATTING_TYPE_WHISPER)
		SendReleaseBlockWhisper();

	return TRUE;
}

BOOL AgcmChatting2::DisableChatMode(AgcmChattingType eChatType)
{
	if (eChatType < 0 || eChatType >= AGCMCHATTING_TYPE_MAX)
		return FALSE;

	m_abChatMessageEnable[eChatType]	= FALSE;

	if (eChatType == AGCMCHATTING_TYPE_WHISPER)
		SendSetBlockWhisper();

	return TRUE;
}

BOOL AgcmChatting2::SendSetBlockWhisper()
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmChatting->MakePacketSetBlockWhisper(m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmChatting->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmChatting2::SendReleaseBlockWhisper()
{
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmChatting->MakePacketReleaseBlockWhisper(m_pcsAgcmCharacter->GetSelfCID(), &nPacketLength);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult		= SendPacket(pvPacket, nPacketLength);

	m_pcsAgpmChatting->m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgcmChatting2::CBRecvReplyBlockWhisper(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmChatting2	*pThis			= (AgcmChatting2 *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	CHAR			*szTargetID		= (CHAR *)			pCustData;

	if (!szTargetID[0])
		return FALSE;

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	if (!pcsSelfCharacter || pcsSelfCharacter != pcsCharacter)
		return FALSE;

	CHAR	szMessageBuffer[128];
	ZeroMemory(szMessageBuffer, sizeof(CHAR) * 128);

	//	print block whisper message to szMessageBuffer.
	sprintf(szMessageBuffer, "<%s> ""%s""%s", pcsSelfCharacter->m_szID,
											  szTargetID,
											  pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_BLOCK_WHISPER_MSG));

	pThis->AddMessage(AGCMCHATTING_TYPE_WHISPER, szMessageBuffer, g_aulDefaultChatColor[AGPDCHATTING_TYPE_WHISPER], TRUE);

	BOOL	bIsNormalMessage	= TRUE;

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, &bIsNormalMessage);

	return TRUE;
}

BOOL AgcmChatting2::CBRecvOfflineWhisperTarget(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmChatting2	*pThis			= (AgcmChatting2 *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	CHAR			*szTargetID		= (CHAR *)			pCustData;

	if (!szTargetID[0])
		return FALSE;

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	if (!pcsSelfCharacter || pcsSelfCharacter != pcsCharacter)
		return FALSE;

	CHAR	szMessageBuffer[128];
	ZeroMemory(szMessageBuffer, sizeof(CHAR) * 128);

	//	print block whisper message to szMessageBuffer.
	sprintf(szMessageBuffer, "<%s> ""%s""%s", pcsSelfCharacter->m_szID,
											  szTargetID,
											  pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CHAT_OFFLINE_WHISPER_TARGET_MSG));

	pThis->AddMessage(AGCMCHATTING_TYPE_WHISPER, szMessageBuffer, g_aulDefaultChatColor[AGPDCHATTING_TYPE_WHISPER], TRUE);

	BOOL	bIsNormalMessage	= TRUE;

	pThis->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, &bIsNormalMessage);

	return TRUE;
}

BOOL AgcmChatting2::CBRecvAreaChatMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pCustData)
		return FALSE;

	AgcmChatting2	*pThis		= (AgcmChatting2 *)	pClass;
	PVOID			*ppvBuffer	= (PVOID *)			pCustData;

	INT8				cIsAdmin			= (INT8)				ppvBuffer[0];
	CHAR				*szMessage			= (CHAR *)				ppvBuffer[1];
	UINT16				unMessageLength		= (UINT16)				ppvBuffer[2];
	INT8				cIsAddHeader		= (INT8)				ppvBuffer[3];
	UINT32				ulTextColorRGB		= (UINT32)				ppvBuffer[4];

	if (!szMessage ||
		!szMessage[0] ||
		unMessageLength < 1 ||
		unMessageLength >= 1024)
		return FALSE;

	return pThis->AddAreaChatMessage(szMessage, unMessageLength, (BOOL) cIsAdmin, (BOOL) cIsAddHeader, ulTextColorRGB);
}

void AgcmChatting2::SetLastWhisperCharacter(CHAR *szID)
{
	if (szID)
        strcpy(m_szLastWhisperCharacter, szID);
	else
		m_szLastWhisperCharacter[0] = 0;
}

CHAR *AgcmChatting2::GetLastWhisperCharacter()
{
	return m_szLastWhisperCharacter;
}

BOOL AgcmChatting2::CheckChatRegion( VOID )
{
	if( !m_pcsApmMap )
		return TRUE;

	AgpdCharacter*		pCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	INT					nResult		= m_pcsApmMap->CheckRegionPerculiarity( pCharacter->m_nBindingRegionIndex , APMMAP_PECULIARITY_CHATTING );

	if( nResult == APMMAP_PECULIARITY_RETURN_DISABLE_USE )
		return FALSE;

	return TRUE;
}