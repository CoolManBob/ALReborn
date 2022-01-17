#include "AgcmUISystemMessage.h"
#include "AgpdSystemMessage.h"
#include "AuStrTable.h"

AgcmUISystemMessage::AgcmUISystemMessage()
{
	SetModuleName("AgcmUISystemMessage");
}

AgcmUISystemMessage::~AgcmUISystemMessage()
{
}

BOOL AgcmUISystemMessage::OnAddModule()
{
	m_pcsAgcmUIManager2			= (AgcmUIManager2	*)GetModule("AgcmUIManager2");
	m_pcsAgcmChatting			= (AgcmChatting2	*)GetModule("AgcmChatting2");

	if	( !m_pcsAgcmUIManager2 || !m_pcsAgcmChatting)		return FALSE;
	if	( !SystemMessage.SetCallbackSystemMessage( CBSystemMessage , this ) )			return FALSE;


	return TRUE;
}

BOOL AgcmUISystemMessage::OnInit()
{
	return TRUE;
}

BOOL AgcmUISystemMessage::OnDestroy()
{
	return TRUE;
}

VOID AgcmUISystemMessage::GetStringAndColor( IN AgpdSystemMessage* pstSysMessage , OUT CHAR* szMessage , OUT DWORD& dwColor )
{
	CHAR* szUIMessage		=	NULL;

	switch(pstSysMessage->m_lCode)
	{
	case AGPMSYSTEMMESSAGE_CODE_LOTTERY_ITEM_TO_POT_ITEM:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_CONVERION_DONE);
		if(!szUIMessage)
			break;

		if (g_eServiceArea == AP_SERVICE_AREA_WESTERN)
			sprintf(szMessage, szUIMessage, pstSysMessage->m_alData[0], pstSysMessage->m_aszData[1], pstSysMessage->m_aszData[0]);
		else
			sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0], pstSysMessage->m_aszData[1], pstSysMessage->m_alData[0]);

		break;

	case AGPMSYSTEMMESSAGE_CODE_LOTTERY_KEY_NOT_USABLE:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERY_KEY_NOT_USABLE);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage);
		break;

	case AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_GOLD_KEY:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_NEEDS_GOLD_KEY);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage);
		break;

	case AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_SILVER_KEY:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_NEEDS_SILVER_KEY);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage);
		break;

	case AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_BRONZE_KEY:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_NEEDS_BRONZE_KEY);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage);
		break;

	case AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_PLATINUM_KEY:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_LOTTERYBOX_NEEDS_PLATINUM_KEY);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage);
		break;

	case AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_STOP_USING_ITEM:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CASH_ITEM_STOP_USING_ITEM);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0]);
		break;

	case AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_END_USING_ITEM:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CASH_ITEM_END_USING_ITEM);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0]);
		break;

	case AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_CANNOT_UNUSE_ITEM:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CASH_ITEM_CANNOT_UNUSE_ITEM);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage);
		break;

	case AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_SAME_KIND_ALREADY_IN_USE:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CASH_ITEM_SAME_KIND_ALREADY_IN_USE);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage);
		break;

	case AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_PAUSE:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CASH_ITEM_PAUSE_ITEM);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0]);
		break;

	case AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_CANNOT_UNUSE_STATUS:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CASH_ITEM_CANNOT_UNUSE_STATUS);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage);
		break;

	case AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_MAX_COUNT:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_TAME_FAILURE_MAX_COUNT);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_INVALID_TARGET:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_TAME_FAILURE_INVALID_TARGET);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_ALREADY_TAMING:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_TAME_FAILURE_ALREADY_TAMING);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_MISS:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_TAME_FAILURE_MISS);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_TRANSPARENT_FAILURE_FOR_COMBAT:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_TRANSPARENT_FAILURE_FOR_COMBAT);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_FIXED_FAILURE_MAX_COUNT:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_FIXED_FAILURE_MAX_COUNT);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_SUMMONS_FAILURE_MAX_COUNT:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_SUMMONS_FAILURE_MAX_COUNT);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_MUTATION_FAILURE_INVALID_TARGET:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_MUTATION_FAILURE_INVALID_TARGET);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_ITEM_CANNOT_BUY:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ITEM_CANNOT_BUY);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CANNOT_USE_BY_STATUS);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_REUSE_INTERVAL:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CANNOT_USE_BY_REUSE_INTERVAL);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_JUMP_ITEM_USED:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_JUMP_ITEM_USED);
		if(!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0]);
		break;

	case AGPMSYSTEMMESSAGE_CODE_CANNOT_RIDE_WHILE_COMBAT:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CANNOT_RIDE_WHILE_COMBAT);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_CANNOT_ATTACKABLE_TARGET:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CANNOT_ATTACKABLE_TARGET);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_CANNOT_INITIALIZE_SKILL :
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_SKILL_INITIALIZE_NO_OWN_SKILL);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_ITEM_FOR_GUILDMASTER:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ITEM_FOR_GUILDMASTER);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_BANK_SLOT_IS_MAX:
		szUIMessage = ClientStr().GetStr(STI_BANK_SLOT_IS_MAX);
		if(!szUIMessage)
			break;
		_tcsncpy_s(szMessage, 255, szUIMessage, _tcslen(szUIMessage));
		dwColor = 0xFFCC0000;
		break;

	case AGPMSYSTEMMESSAGE_CODE_BANK_ADD_SLOT_SUCCESS:
		szUIMessage = ClientStr().GetStr(STI_BANK_ADD_SLOT_SUCCESS);
		if(!szUIMessage)
			break;
		_tcsncpy_s(szMessage, 255, szUIMessage, _tcslen(szUIMessage));
		break;

	case AGPMSYSTEMMESSAGE_CODE_BANK_MONEY_FIRST:
		szUIMessage = ClientStr().GetStr(STI_BANK_MONEY_FIRST);
		if(!szUIMessage)
			break;
		_tcsncpy_s(szMessage, 255, szUIMessage, _tcslen(szUIMessage));
		break;

	case AGPMSYSTEMMESSAGE_CODE_PARTY_PORTAL_ERROR_DEAD_MEMBER			:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_PARTY_PORTAL_ERROR_DEAD_MEMBER);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0]);
		break;
	case AGPMSYSTEMMESSAGE_CODE_PARTY_PORTAL_ERROR_IN_SECRET_DUNGEON	:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_PARTY_PORTAL_ERROR_IN_SECRET_DUNGEON);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0]);
		break;
	case AGPMSYSTEMMESSAGE_CODE_PARTY_PORTAL_ERROR_IN_SIEGE_WAR			:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_PARTY_PORTAL_ERROR_IN_SIEGE_WAR);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0]);
		break;
	case AGPMSYSTEMMESSAGE_CODE_PARTY_PORTAL_ERROR_LACK_OF_LEVEL		:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_PARTY_PORTAL_ERROR_LACK_OF_LEVEL);
		if (!szUIMessage)
			break;
		sprintf(szMessage, szUIMessage, pstSysMessage->m_aszData[0]);
		break;

	case AGPMSYSTEMMESSAGE_CODE_GO_COMMAND_LACK_OF_LEVEL:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GO_COMMAND_LACK_OF_LEVEL);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);	
		break;

	case AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_WANGBOK_SCROLL:
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CANNOT_USE_WANGBOK_SCROLL);
		if(!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);				
		break;

	case AGPMSYSTEMMESSAGE_CODE_INVEN_MONEY_FULL:
		szUIMessage = ClientStr().GetStr(STI_HAVE_NOT_MORE_GELD);
		if (szUIMessage)
		{
			strncpy(szMessage, szUIMessage, 255);
			dwColor = 0xFFCC0000;
		}
		break;

	case AGPMSYSTEMMESSAGE_CODE_SUMMONS_FAILURE_LEVEL_LIMIT_REGION:
		szUIMessage = ClientStr().GetStr(STI_SUMMONS_FAILURE_LEVEL_LIMIT_REGION);
		if (szUIMessage)
		{
			strncpy(szMessage, szUIMessage, 255);
			dwColor = 0xFFCC0000;
		}
		break;

	case AGPMSYSTEMMESSAGE_CODE_PET_IS_HUNGRY:
		szUIMessage = ClientStr().GetStr(STI_PET_HUNGRY_SYSTEM_MESSAGE);
		if (!szUIMessage)
			break;
		strncpy(szMessage, szUIMessage, 255);
		break;

	case AGPMSYSTEMMESSAGE_CODE_GENERAL_STRING:
		sprintf(szMessage, "%s", pstSysMessage->m_aszData[0]);
		break;

	case AGPMSYSTEMMESSAGE_CODE_DEBUG_STRING:
		sprintf(szMessage, "%s", pstSysMessage->m_aszData[0]);
		break;

	case AGPMSYSTEMMESSAGE_CODE_DISABLE_AUCTION:
		szUIMessage = ClientStr().GetStr(STI_DISABLE_AUCTION);
		if( !szUIMessage )
			break;
		strncpy( szMessage, szUIMessage , 255 );
		break;
	case AGPMSYSTEMMESSAGE_CODE_NOT_ENOUGH_INVENTORY:
		szUIMessage = ClientStr().GetStr(STI_NOT_ENOUGH_INVENTORY);
		if( !szUIMessage )
			break;
		strncpy( szMessage, szUIMessage , 255 );
		break;

	case AGPMSYSTEMMESSAGE_CODE_DISABLE_EQUIP_ITEM_THIS_REGION:
		szUIMessage = ClientStr().GetStr( STI_DISABLEEQUIPITEM_THISREGION );
		if( !szUIMessage )
			break;

		sprintf_s( szMessage , 255 , szUIMessage , pstSysMessage->m_aszData[0] );
		break;

	case AGPMSYSTEMMESSAGE_CODE_DISABLE_USE_THIS_REGION:
		szUIMessage	= ClientStr().GetStr( STI_DISABLEUSE_THISREGION );
		if( szUIMessage )
			sprintf_s( szMessage , 255 , szUIMessage );
		break;

	// 셧다운제 시행으로 인하여 %d분 후, 만 16세 미만의 고객은 접속이 종료됩니다. 자정(24시)이 되기전 미리 안전한 곳에서 접속을 종료 시켜주시기 바랍니다. 
	case AGPMSYSTEMMESSAGE_CODE_MIDNIGHTSHUTDOWN_LEFT_MIN :
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage("MidnightShutdown_Left_Min");

		if( szUIMessage )
			sprintf_s( szMessage , 255 , szUIMessage, pstSysMessage->m_alData[0] );
		break;

	// 셧다운제 시행으로 인하여 만 16세 미만의 고객은 접속이 제한 됩니다.(12AM~6AM)
	case AGPMSYSTEMMESSAGE_CODE_MIDNIGHTSHUTDOWN :
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage("MidnightShutdown");

		if( szUIMessage )
			sprintf_s( szMessage , 255 , szUIMessage );
		break;
	}
}

BOOL AgcmUISystemMessage::CBSystemMessage( PVOID pData, PVOID pClass, PVOID pCustData )
{

	AgpdSystemMessage*		pstSysMessage	=	static_cast< AgpdSystemMessage*		>( pData	);
	AgcmUISystemMessage*	pThis			=	static_cast< AgcmUISystemMessage*	>( pClass	);	

	ASSERT( pstSysMessage );
	ASSERT( pThis		  );

	if( pstSysMessage	==	NULL	)		return FALSE;
	if( pThis			==	NULL	)		return FALSE;

	
	DWORD		dwColor			= 0XFFFFFF00;		// ARGB (UI쪽에 뿌리는 건 ARGB)
	
	CHAR		szMessage[ 255 ];
	ZeroMemory( szMessage , sizeof(szMessage) );

	// lCode값이 -1이 아니면.. 해당 String Table에서 문자열을 받아온다
	if( pstSysMessage->m_lCode != -1 )
	{
		pThis->GetStringAndColor( pstSysMessage , szMessage , dwColor );
	}

	else
	{
		sprintf_s( szMessage , sizeof(szMessage), "%s" , pstSysMessage->m_aszData[0] );
		dwColor	=	pstSysMessage->m_dwColor;
	}


	if(strlen(szMessage) > 0)	
	{

		switch( pstSysMessage->m_lOperation )
		{
		case 0:		pThis->AddSystemMessage(szMessage, dwColor);		break;
		case 1:		pThis->AddModalMessage( szMessage );				break;
		case 2:		pThis->AddModallessMessage( szMessage );			break;
		}

	}

	return TRUE;
}

// System Message 를 밀어넣는다.
BOOL AgcmUISystemMessage::AddSystemMessage(CHAR* szMsg, DWORD dwColor)
{
	if( szMsg	==	NULL )		return FALSE;

	if( dwColor != 0 )	m_pcsAgcmChatting->AddSystemMessage( szMsg , dwColor	);
	else				m_pcsAgcmChatting->AddSystemMessage( szMsg				);

	m_pcsAgcmChatting->EnumCallback( AGCMCHATTING2_CB_INPUT_MESSAGE , NULL , NULL );	
	
	return TRUE;
}

// Modal 메세지 박스 상자를 띄운다
BOOL AgcmUISystemMessage::AddModalMessage( CHAR* szMsg )
{
	if( szMsg	==	NULL)		return FALSE;

	m_pcsAgcmUIManager2->ActionMessageOKDialog( szMsg );	

	return TRUE;
}

// Modalless 메세지 박스 상자를 띄운다
BOOL AgcmUISystemMessage::AddModallessMessage( CHAR* szMsg )
{
	if( szMsg	=	NULL )		return FALSE;

	m_pcsAgcmUIManager2->ActionMessageOKDialog2( szMsg );	

	return TRUE;
}