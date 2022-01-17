#include "AgcmEventGacha.h"
#include "AgcmCharacter.h"
#include "AgcmEventManager.h"
#include "AgcmChatting2.h"

static AgpmEventGacha	*s_pcsAgpmEventGacha = NULL;
static ApdEvent			*s_pcsLastEventForLua	= NULL;

AgcmEventGacha::AgcmEventGacha():m_bGachaWindowOpened( FALSE )
{
	SetModuleName("AgcmEventGacha");
}

AgcmEventGacha::~AgcmEventGacha()
{
}

LuaGlue	LG_Gacha( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	AgcmCharacter * pcsAgcmCharacter = ( AgcmCharacter * ) g_pEngine->GetModule( "AgcmCharacter" );

	INT16 nPacketLength = 0;

	PVOID pvPacket = s_pcsAgpmEventGacha->MakePacketEventGacha( 
		s_pcsLastEventForLua , 
		pcsAgcmCharacter->GetSelfCharacter()->m_lID,
		&nPacketLength );

	if( pvPacket )
	{
		BOOL bResult = s_pcsAgpmEventGacha->SendPacket(pvPacket, nPacketLength);
		s_pcsAgpmEventGacha->m_csPacket.FreePacket(pvPacket);

		g_pEngine->LuaErrorMessage( "Gacha Request" );
	}
	else
	{
		g_pEngine->LuaErrorMessage( "Gacha Request Failed" );
	}

	LUA_RETURN( TRUE )
}

void	AgcmEventGacha::OnLuaInitialize( AuLua * pLua )
{
	/*
	luaDef MVLuaGlue[] = 
	{
		{"Gacha"	,	LG_Gacha	},
		{NULL					,	NULL				},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
	*/
}

BOOL AgcmEventGacha::OnAddModule()
{
	AgcmEventManager *pcsAgcmEventManager = (AgcmEventManager *) GetModule("AgcmEventManager");
	s_pcsAgpmEventGacha = ( AgpmEventGacha * ) GetModule( "AgpmEventGacha" );

	if( !pcsAgcmEventManager || !s_pcsAgpmEventGacha)
		return FALSE;

	if(!pcsAgcmEventManager->RegisterEventCallback(APDEVENT_FUNCTION_GACHA, CBEventRequest, this))
		return FALSE;

	s_pcsAgpmEventGacha->SetCallbackRequestGranted	( CBEventGrant	, this );
	s_pcsAgpmEventGacha->SetCallbackResult			( CBEventResult	, this );

	return TRUE;
}

BOOL AgcmEventGacha::CBEventRequest	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventGacha *pThis = (AgcmEventGacha *) pClass;
	ApdEvent	*pcsEvent = ( ApdEvent * ) pData;
	ApBase *pGenerator = (ApBase *) pCustData;

	AgpmEventGacha	* pcsAgpmEventGacha		= (AgpmEventGacha *) pThis->GetModule("AgpmEventGacha");
	AgpmCharacter	* pcsAgpmCharacter		= (AgpmCharacter *) pThis->GetModule("AgpmCharacter");
	AgcmCharacter	* pcsAgcmCharacter		= (AgcmCharacter *) pThis->GetModule("AgcmCharacter");

	// 이미 떠있으면 막아라.
	if( pThis->IsGachaWindowOpen() ) return FALSE;

	INT32	nGachaType = PtrToInt( pcsEvent->m_pvData );

	if(pcsAgpmCharacter->IsActionBlockCondition( pcsAgcmCharacter->m_pcsSelfCharacter) )
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = pcsAgpmEventGacha->MakePacketEventRequest(pcsEvent, pGenerator->m_lID, nGachaType , &nPacketLength);
	
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = pThis->SendPacket(pvPacket, nPacketLength);
	pcsAgpmEventGacha->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmEventGacha::CBEventGrant	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//Display_System_Message( "Gacha Event Granted" );
	// 여기서 UI를 띄워야함..

	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmEventGacha *pThis = (AgcmEventGacha *) pClass;
	AgpmEventGacha::GachaInfo	*pstGachaInfo = ( AgpmEventGacha::GachaInfo * ) pData ;
	ApBase *pGenerator = (ApBase *) pCustData;

	s_pcsLastEventForLua	= pstGachaInfo->pcsEvent;

	pThis->EnumCallback( CB_OPEN_GACHAUI , pstGachaInfo , pGenerator );

	return TRUE;
}

BOOL AgcmEventGacha::CBEventResult	(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventGacha *pThis = (AgcmEventGacha *) pClass;
	AgpmEventGacha::GachaInfo	*pstGachaInfo = ( AgpmEventGacha::GachaInfo * ) pData ;

	INT32 nTID		= pstGachaInfo->lResult	;
	INT32 eResult	= pstGachaInfo->lTID	;

	ApBase *pGenerator = (ApBase *) pCustData;

	//Display_System_Message( "Result Received" );

	pThis->EnumCallback( CB_GACHA_ROLL , IntToPtr( eResult ) , IntToPtr( nTID ) );

	return TRUE;
}
