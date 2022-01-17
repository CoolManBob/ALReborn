#include "AgcmTextBoardMng.h"

#include "AgcmDmgBoard.h"
#include "AgcmHPBar.h"
#include "AgcmIDBoard.h"
#include "AgcmImoticonBoard.h"
#include "AgcmTextBoard.h"

#include "AgcmUIManager2.h"

#pragma warning( disable : 4244 )

//---------------------------- CPostFX -----------------------------
CPostFX::CPostFX() : m_pcsAgcmPostFX(NULL)
{
	m_fStart		= 0;
	m_bCircleFX		= false;
	m_bPostFXOn		= FALSE;
	m_fSpeedScale	= 1.0f;
}

CPostFX::~CPostFX()
{
}

BOOL	CPostFX::Init()
{
	return TRUE;
}
void	CPostFX::Destory()
{
}

void	CPostFX::AddModules( AgcmPostFX* postFX  )
{
	m_pcsAgcmPostFX	= postFX;
}

void	CPostFX::Update()
{
	if( !m_pcsAgcmPostFX )	return;
	if( !m_bCircleFX )		return;

	float fElapsed = float( clock() ) / CLOCKS_PER_SEC - m_fStart;
	if( fElapsed < 0.2f * m_fSpeedScale )
		m_pcsAgcmPostFX->setRadius( fElapsed * 1.5f );
	else
	{
		if( m_bPostFXOn )
		{
			m_pcsAgcmPostFX->setPipeline( m_szPipeBU );
			m_pcsAgcmPostFX->On();
		}
		else
			m_pcsAgcmPostFX->Off(true);

		m_bCircleFX = false;
	}
}
void	CPostFX::PlayEffectFX( const char* szText, float fSpeedScale )
{
	if( !m_bCircleFX )
	{
		if( m_bPostFXOn = m_pcsAgcmPostFX->isOn() )
			m_pcsAgcmPostFX->getPipeline( m_szPipeBU, 1023 );
		else
			m_szPipeBU[0] = '\0';
	}

	string strBuf( m_szPipeBU );
	if( !strBuf.empty() )
		strBuf += "-";
	strBuf += szText;

	m_pcsAgcmPostFX->setPipeline( strBuf.c_str() );
	m_pcsAgcmPostFX->On();

	m_fStart		= float( clock() ) / CLOCKS_PER_SEC;
	m_bCircleFX		= true;
	m_fSpeedScale	= fSpeedScale;
}

AgcmTextBoardMng::AgcmTextBoardMng(void)
:m_pcsAgcmUIManager2(NULL)
{
	SetModuleName( "AgcmTextBoardMng" );

	EnableIdle( TRUE );

	SetEnableMiniHPBar( TRUE );

	
	m_pcsAgpmCharacter   = NULL;	m_pcRender           = NULL;	m_pcCmCharacter        = NULL;
	m_pcFont             = NULL;	m_pcApmMap           = NULL;	m_pcsAgpmFactors       = NULL;
	m_pcsAgpmItem        = NULL;	m_pcsAgcmItem        = NULL;	m_pcsApmObject         = NULL;
	m_pcsAgcmObject      = NULL;	m_pcsApmEventManager = NULL;	m_pcsAgpmEventTeleport = NULL;
	m_pcsAgpmSkill       = NULL;	m_pcsAgcmSkill       = NULL;	m_pcsAgpmGuild         = NULL;
	m_pcsAgpmParty       = NULL;	m_pcsAgpmPvP         = NULL;	m_pcsAgpmBillInfo      = NULL;
	m_pcsAgpmSiegeWar    = NULL;	m_pcsAgcmSiegeWar    = NULL;	m_pcsAgpmBattleGround  = NULL;
	m_pcsAgcmUIControl   = NULL;	m_pcsAgcmSound       = NULL;	m_pcsAgcmUIOption      = NULL;
	m_pcsAgcmCharacter	 = NULL;

	m_pCamera			 = NULL;

	m_bMakeTextBaloon =	m_bDrawNameMine   = m_bDrawNameMonster = m_bDrawNameGuild	= TRUE;
	m_bDrawNameParty  = m_bDrawNameOthers = m_bDrawNameClass   = TRUE;
	m_bDrawHPMP		  = TRUE;
	m_bDrawID		  = TRUE;

	m_LastTick			=	0;
	m_TickDiff			=	0;

	m_pMainCharacterID = NULL;

	ZeroMemory( m_ImgIDETC      , sizeof(RwTexture*) * TBID_TEXTURE_ADDINTIONAL_SIZE );
	ZeroMemory( m_ImgBalloon    , sizeof(RwTexture*) * TB_IMAGE_COUNT				);
	ZeroMemory( m_ImgBalloonTail, sizeof(RwTexture*) * TB_IMAGE_TAIL_COUNT			);
	ZeroMemory( m_ImgIDTag      , sizeof(RwTexture*) * TB_MAX_TAG					);
	ZeroMemory( m_ImgImoticon   , sizeof(RwTexture*) * (IMOTICON_COUNT-530)			);
	ZeroMemory( m_ImgHPBar      , sizeof(RwTexture*) * 3 );

	ZeroMemory( m_vDamageUVW        , sizeof(RwV3d) * 10 );
	ZeroMemory( m_fDamageSizeSmall  , sizeof(FLOAT) * 10 );

	AgcmBaseBoard::SetMng( this );

	m_vecTexCharisma.clear();
}

AgcmTextBoardMng::~AgcmTextBoardMng(void)
{

}

sBoardDataPtr AgcmTextBoardMng::CreateBoard( UINT32 nID , sBoardAttrPtr pAttr )
{
	if( pAttr == NULL ) return NULL;

	AgpdCharacter  *pdCharacter = m_pcCmCharacter->GetAgpmCharacter()->GetCharacter( nID );	
	sBoardDataPtr   pBoardData = NULL;

	return CreateBoard( (ApBase*)pdCharacter , pAttr );
}

sBoardDataPtr AgcmTextBoardMng::CreateBoard( ApBase* pBase , sBoardAttrPtr pAttr )
{
	if( pAttr == NULL ) 
		return NULL;

	switch( pAttr->eType )
	{
	case AGCM_BOARD_ITEMID:
		return _CreateItemID( pBase , pAttr );

	case AGCM_BOARD_IDBOARD:
		return _CreateIDBoard( pBase , pAttr );

	case AGCM_BOARD_TEXT:
		return _CreateText( pBase , pAttr );
	
	case AGCM_BOARD_DAMAGE:
	case AGCM_BOARD_CRITICALDAMAGE:
		return _CreateDmg( pBase , pAttr );

	case AGCM_BOARD_IMOTICON:
	default:
		return _CreateImoticon( pBase , pAttr );
		
	}

	return NULL;
}

sBoardDataPtr AgcmTextBoardMng::GetBoard( ApBase* pBase  )
{
	if( !pBase )
		return NULL;

	sBoardDataPtr		pBoardData	=	NULL;

	switch( pBase->m_eType )
	{
	case APBASE_TYPE_CHARACTER:
		{
			AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( (AgpdCharacter*)pBase );
			if( pcdCharacter && pcdCharacter->m_pvTextboard )
				pBoardData	=	(sBoardDataPtr)pcdCharacter->m_pvTextboard;
		}

		break;

	case APBASE_TYPE_ITEM:
		{
			AgcdItem*	pcdItem		=	m_pcsAgcmItem->GetItemData( (AgpdItem*)pBase );
			if( pcdItem && pcdItem->m_pvTextboard )
				pBoardData	=	(sBoardDataPtr)pcdItem->m_pvTextboard;
		}
		break;
	}

	return pBoardData;
}

sBoardDataPtr AgcmTextBoardMng::GetBoard( UINT32 nID  )
{
	AgpdCharacter  *pdCharacter = m_pcCmCharacter->GetAgpmCharacter()->GetCharacter( nID );	
	
	return GetBoard( static_cast< ApBase* >(pdCharacter) );
}

BOOL AgcmTextBoardMng::OnAddModule()
{
	CPostFX::AddModules( reinterpret_cast<AgcmPostFX*>( GetModule("AgcmPostFX") ) );
	
	//Get Module-------------------------------------------------------------------
	m_pcsAgpmCharacter		= (AgpmCharacter   *)GetModule("AgpmCharacter");
	m_pcRender				= (AgcmRender      *)GetModule("AgcmRender");
	m_pcCmCharacter			= (AgcmCharacter   *)GetModule("AgcmCharacter");
	m_pcFont				= (AgcmFont        *)GetModule("AgcmFont");
	m_pcApmMap				= (ApmMap          *)GetModule("ApmMap");
	m_pcsAgpmFactors		= (AgpmFactors     *)GetModule("AgpmFactors");
	m_pcsAgpmItem			= (AgpmItem        *)GetModule("AgpmItem");
	m_pcsAgcmItem			= (AgcmItem        *)GetModule("AgcmItem"); 
	m_pcsApmObject			= (ApmObject       *)GetModule("ApmObject");
	m_pcsAgcmObject			= (AgcmObject      *)GetModule("AgcmObject");
	m_pcsApmEventManager	= (ApmEventManager *)GetModule("ApmEventManager");
	m_pcsAgpmSkill			= (AgpmSkill       *)GetModule("AgpmSkill");
	m_pcsAgcmSkill			= (AgcmSkill       *)GetModule("AgcmSkill");
	m_pcsAgpmGuild			= (AgpmGuild       *)GetModule("AgpmGuild");
	m_pcsAgpmParty			= (AgpmParty       *)GetModule("AgpmParty");
	m_pcsAgpmPvP			= (AgpmPvP         *)GetModule("AgpmPvP");
	m_pcsAgpmBillInfo		= (AgpmBillInfo    *)GetModule("AgpmBillInfo");
	m_pcsAgpmSiegeWar		= (AgpmSiegeWar    *)GetModule("AgpmSiegeWar");
	m_pcsAgcmSiegeWar		= (AgcmSiegeWar    *)GetModule("AgcmSiegeWar");
	m_pcsAgpmBattleGround	= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pcsAgcmUIControl		= (AgcmUIControl   *)GetModule("AgcmUIControl");
	m_pcsAgcmSound			= (AgcmSound       *)GetModule("AgcmSound");
	m_pcsAgcmCharacter		= (AgcmCharacter   *)GetModule("AgcmCharacter");
	//--------------------------------------------------------------------------------

	//Set Callback--------------------------------------------------------------------
	m_pcRender->SetCallbackPostRender2				( CB_POST_RENDER		  , this );

	m_pcCmCharacter->SetCallbackInitCharacter		( CB_AddChar			  , this );
	//m_pcCmCharacter->SetCallbackRemoveCharacter		( CB_RemoveChar			  , this );
	m_pcCmCharacter->SetCallbackPreRemoveCharacter	( CB_RemoveChar			  , this );
	m_pcCmCharacter->SetCallbackSetSelfCharacter	( CB_SetSelfCharacter     , this );
	m_pcCmCharacter->SetCallbackReleaseSelfCharacter( CB_ReleaseSelfCharacter , this );
	m_pcCmCharacter->SetCallbackDisplayActionResult	( CB_DisplayActionResult  , this );
	m_pcCmCharacter->SetCallbackUpdateCharHP		( CB_HPUpdate			  , this );
	m_pcCmCharacter->SetCallbackUpdateCharMP		( CB_MPUpdate			  , this );
	m_pcCmCharacter->SetCallbackKillCharacter		( CB_KillCharacter		  , this );
	m_pcCmCharacter->SetCallbackDestroyBoard		( CB_DestroyBoard		  , this );

	m_pcsAgcmItem->SetCallbackFieldItem	   ( CB_ItemAdd   , this );
	m_pcsAgcmItem->SetCallbackPreRemoveData( CB_ItemRemove, this );

	m_pcsAgpmCharacter->SetCallbackUpdateActionStatus  ( CB_UpdateActionStatus		      , this );
	m_pcsAgpmCharacter->SetCallbackUpdateCriminalStatus( CB_UpdateIDFirstAttackerIconFlag , this );
	m_pcsAgpmCharacter->SetCallbackUpdateNickName	   ( CB_UpdateNickName				  , this );

	m_pcsAgpmFactors->SetCallbackUpdateFactorMurdererPoint( CB_UpdateIDMurderIconFlag , this );

	m_pcsAgcmSkill->SetCallbackMissCastSkill ( CB_MissCastSkill  , this );
	m_pcsAgcmSkill->SetCallbackBlockCastSkill( CB_BlockCastSkill , this );
	m_pcsAgcmSkill->SetCallbackAddHpBar		 ( CB_SkillAddHPBar  , this );

	m_pcsAgpmGuild->SetCallbackCharData( CB_Guild , this );

	m_pcsAgpmParty->SetCBAddParty	 ( CB_PartyAdd         , this );
	m_pcsAgpmParty->SetCBRemoveParty ( CB_PartyLeave       , this );
	m_pcsAgpmParty->SetCBAddMember	 ( CB_PartyAddMember   , this );
	m_pcsAgpmParty->SetCBRemoveMember( CB_PartyLeaveMember , this );

	m_pcsAgpmPvP->SetCallbackPvPInfo		  ( CB_PvPRefreshChar, this );
	m_pcsAgpmPvP->SetCallbackAddFriend		  ( CB_PvPRefreshChar, this );
	m_pcsAgpmPvP->SetCallbackAddEnemy		  ( CB_PvPRefreshChar, this );
	m_pcsAgpmPvP->SetCallbackRemoveFriend	  ( CB_PvPRefreshChar, this );
	m_pcsAgpmPvP->SetCallbackRemoveEnemy	  ( CB_PvPRefreshChar, this );
	m_pcsAgpmPvP->SetCallbackAddFriendGuild	  ( CB_PvPRefreshGuild, this );
	m_pcsAgpmPvP->SetCallbackAddEnemyGuild	  ( CB_PvPRefreshGuild, this );
	m_pcsAgpmPvP->SetCallbackRemoveFriendGuild( CB_PvPRefreshGuild, this );
	m_pcsAgpmPvP->SetCallbackRemoveEnemyGuild ( CB_PvPRefreshGuild, this );
	m_pcsAgpmPvP->SetCallbackInitFriendGuild  ( CB_PvPInit, this );
	m_pcsAgpmPvP->SetCallbackInitEnemyGuild	  ( CB_PvPInit, this );
	//---------------------------------------------------------------------------------------

	SetWorldCamera( GetCamera() );
	return TRUE;
}

BOOL AgcmTextBoardMng::OnInit()
{
	CPostFX::Init();
	LoadFlagFromFile( "INI\\FlagImoticon.xml" );

	m_LastTick    = 0;

	FLOAT     fWidth    = 256.0f;
	FLOAT     fTemp[11] = { 0 , 21 , 43 , 65 , 89 , 112 , 135 , 156 , 179 , 202 , 227 };

	for( int i = 1 ; i < 10 ; ++i )
	{
		m_vDamageUVW[i].x   = fTemp[ i-1 ] / fWidth;
		m_vDamageUVW[i].y   = fTemp[  i  ] / fWidth;
		m_vDamageUVW[i].z   = fTemp[  i  ] - fTemp[ i - 1 ];
		m_vDamageUVW[i].z  *= 1.2f;
	}

	m_vDamageUVW[0].x    = fTemp[  9 ] / fWidth;
	m_vDamageUVW[0].y    = fTemp[ 10 ] / fWidth;
	m_vDamageUVW[0].z    = fTemp[ 10 ] - fTemp[ 9 ];
	m_vDamageUVW[0].z   *= 1.2f;

	FLOAT	fTemp2[10] = { 19 , 14 , 17 , 18 , 18 , 17 , 18 , 15 , 19 , 17 };

	for( int i = 0 ; i < 10 ; ++i )
	{
		m_fDamageSizeSmall[i]    = fTemp2[ i ];
		m_fDamageSizeSmall[i]   *= 1.2f;
	}

	AgcmResourceLoader*	pLoader		=	static_cast< AgcmResourceLoader* >(GetModule( "AgcmResourceLoader" ));

	m_BattleNameColor[ BATTLEGROUND_HUMAN_NAME_COLOR		] =	ARGB_TO_ABGR(pLoader->GetColor( "BG_HUMAN_NAME" ));
	m_BattleNameColor[ BATTLEGROUND_ORC_NAME_COLOR			] =	ARGB_TO_ABGR(pLoader->GetColor( "BG_ORC_NAME" ));
	m_BattleNameColor[ BATTLEGROUND_MOONELF_NAME_COLOR		] =	ARGB_TO_ABGR(pLoader->GetColor( "BG_MOONELF_NAME" ));
	m_BattleNameColor[ BATTLEGROUND_DRAGONSCION_NAME_COLOR	] =	ARGB_TO_ABGR(pLoader->GetColor( "BG_DRAGONSCION_NAME" ));

	m_pcsAgcmUIManager2 = static_cast<AgcmUIManager2*>(GetModule( "AgcmUIManager2" ));

	AS_REGISTER_TYPE_BEGIN( AgcmTextBoardMng , AgcmTextBoardMng );
	AS_REGISTER_METHOD0( void , ToggleIDDisplay );
	AS_REGISTER_METHOD0( void , ToggleMiniHPBar );
	AS_REGISTER_TYPE_END;

	return TRUE;
}

BOOL AgcmTextBoardMng::OnDestroy()
{
	ClearAllFlag();
	CPostFX::Destory();
	
	for( int i = 0 ; i < TB_IMAGE_COUNT      ; ++i ) RW_TEXTURE_DESTROY( m_ImgBalloon[i] );
	for( int i = 0 ; i < TB_IMAGE_TAIL_COUNT ; ++i ) RW_TEXTURE_DESTROY( m_ImgBalloonTail[i] );
	for( int i = 0 ; i < TB_MAX_TAG          ; ++i ) RW_TEXTURE_DESTROY( m_ImgIDTag[i] );
	for( int i = 0 ; i < IMOTICON_COUNT-530  ; ++i ) RW_TEXTURE_DESTROY( m_ImgImoticon[i] );

	RW_TEXTURE_DESTROY( m_ImgDamage );
	RW_TEXTURE_DESTROY( m_ImgGOMark );

	for( RwTextureVecItr Itr = m_vecTexCharisma.begin(); Itr != m_vecTexCharisma.end(); ++Itr )
		RW_TEXTURE_DESTROY( (*Itr) );

	m_vecTexCharisma.clear();

	std::list< AgcmImoticonBoard* >::iterator   EmoListIter;
	std::list< AgcmDmgBoard*      >::iterator   DmgListIter;


	INT				lIndex			=	0;
	AgpdCharacter*	ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence( &lIndex );
	for( ; ppdCharacter ; ppdCharacter = m_pcsAgpmCharacter->GetCharSequence( &lIndex ) )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
		if( !pcdCharacter || !pcdCharacter->m_pvTextboard )
			continue;

		sBoardDataPtr	pBoardData	=	static_cast< sBoardDataPtr >(pcdCharacter->m_pvTextboard);
		
		SAFE_DELETE( pBoardData->pHPBar );
		SAFE_DELETE( pBoardData->pIDBoard );
		SAFE_DELETE( pBoardData->pTextBoard );

		DmgListIter	=	pBoardData->pDmgBoard.begin();
		EmoListIter	=	pBoardData->pImotioconBoard.begin();

		for( ; DmgListIter != pBoardData->pDmgBoard.end() ; ++DmgListIter )
		{
			SAFE_DELETE( (*DmgListIter) );
		}

		for( ; EmoListIter != pBoardData->pImotioconBoard.end() ; ++EmoListIter )
		{
			SAFE_DELETE( (*EmoListIter) );
		}

		pBoardData->pDmgBoard.clear();
		pBoardData->pImotioconBoard.clear();

		SAFE_DELETE( pBoardData );
		pcdCharacter->m_pvTextboard	=	NULL;
		
	}

	m_pMainCharacterID	= NULL;

	return TRUE;
}

BOOL AgcmTextBoardMng::OnIdle( UINT32 ulClockCount )
{
	PROFILE("AgcmTextBoardMng::OnIdle");
	CPostFX::Update();

	UINT32	ulElapse = ulClockCount - m_LastTick;
	m_TickDiff = ulElapse;

	RwFrame* CameraFrame = RwCameraGetFrame( m_pcRender->GetCamera() );

	LockFrame();
	RwMatrix*  matCamera = RwFrameGetLTM( CameraFrame );
	UnlockFrame();

	Update( ulElapse , matCamera );

	m_LastTick = ulClockCount;

	return TRUE;
}

void AgcmTextBoardMng::PostRender()
{

	RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	RwRenderStateSet( rwRENDERSTATETEXTUREFILTER	, (void *)rwFILTERNEAREST );
	RwRenderStateSet( rwRENDERSTATETEXTUREADDRESS	, (void *)rwTEXTUREADDRESSCLAMP );
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE		, (void *)TRUE  );

	/////////////////////////////////////////////////////////////////////////////////////
	// Character Sequence Update	-	Start
	/////////////////////////////////////////////////////////////////////////////////////
	INT		lIndex	=	0;
	AgpdCharacter*	pdSequenceCharacter	=	m_pcsAgpmCharacter->GetCharSequence( &lIndex );

	for( ; pdSequenceCharacter ; pdSequenceCharacter	=	m_pcsAgpmCharacter->GetCharSequence( &lIndex ) )
	{
		if( !pdSequenceCharacter->m_bNPCDisplayForNameBoard )
			continue;

		AgcdCharacter*	cdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( pdSequenceCharacter );
		if( !cdCharacter || !cdCharacter->m_pvTextboard )
			continue;

		sBoardDataPtr	pBoardData	=	static_cast< sBoardDataPtr >(cdCharacter->m_pvTextboard);
		if( !pBoardData )
			continue;

		if( pBoardData->pHPBar )	
			pBoardData->pHPBar->Render();

		if( pBoardData->pTextBoard )
			pBoardData->pTextBoard->Render();

		if( pBoardData->pIDBoard )
			pBoardData->pIDBoard->Render();

		std::list< AgcmDmgBoard*      >::iterator   DmgListIter;
		std::list< AgcmImoticonBoard* >::iterator   EmoListIter;

		DmgListIter     = pBoardData->pDmgBoard.begin();
		EmoListIter     = pBoardData->pImotioconBoard.begin();

		for( ; DmgListIter != pBoardData->pDmgBoard.end() ; ++DmgListIter )
		{
			(*DmgListIter)->Render();
		}

		for( ; EmoListIter != pBoardData->pImotioconBoard.end() ; ++EmoListIter )
		{
			(*EmoListIter)->Render();
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	// Character Sequence Update	-	End
	/////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////
	// Item Sequence Update	-	Start
	/////////////////////////////////////////////////////////////////////////////////////
	lIndex	=	0;
	AgpdItem*	pItemSeq	=	m_pcsAgpmItem->GetItemSequence( &lIndex );
	for( ; pItemSeq ; pItemSeq	=	m_pcsAgpmItem->GetItemSequence( &lIndex ) )
	{
		AgcdItem*	pcdItem	=	m_pcsAgcmItem->GetItemData( pItemSeq );
		if( !pcdItem || !pcdItem->m_pvTextboard )
			continue;

		sBoardDataPtr	pBoardData	=	static_cast< sBoardDataPtr >(pcdItem->m_pvTextboard);
		if( !pBoardData )
			continue;

		if( pBoardData->pHPBar )	
			pBoardData->pHPBar->Render();

		if( pBoardData->pTextBoard )
			pBoardData->pTextBoard->Render();

		if( pBoardData->pIDBoard )
			pBoardData->pIDBoard->Render();

		std::list< AgcmDmgBoard*      >::iterator   DmgListIter;
		std::list< AgcmImoticonBoard* >::iterator   EmoListIter;

		DmgListIter     = pBoardData->pDmgBoard.begin();
		EmoListIter     = pBoardData->pImotioconBoard.begin();

		for( ; DmgListIter != pBoardData->pDmgBoard.end() ; ++DmgListIter )
		{
			(*DmgListIter)->Render();
		}

		for( ; EmoListIter != pBoardData->pImotioconBoard.end() ; ++EmoListIter )
		{
			(*EmoListIter)->Render();
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	// Item Sequence Update	-	End
	/////////////////////////////////////////////////////////////////////////////////////

	//std::list< AgcmDmgBoard*      >::iterator   DmgListIter;
	//std::list< AgcmImoticonBoard* >::iterator   EmoListIter;

	//BoardDataListIter   Iter = m_BoardDataList.begin();

	//for( ; Iter != m_BoardDataList.end() ; ++Iter )
	//{
	//	ApBase* pBase = (*Iter)->pBase;
	//	if( pBase && pBase->m_eType == APBASE_TYPE_CHARACTER )
	//	{
	//		AgpdCharacter* ppdCharacter = ( AgpdCharacter* )pBase;
	//		if( !ppdCharacter->m_bNPCDisplayForNameBoard )
	//			continue;
	//	}

	//	if( (*Iter)->pHPBar     )   (*Iter)->pHPBar->Render();
	//	if( (*Iter)->pTextBoard )   (*Iter)->pTextBoard->Render();
	//	if( (*Iter)->pIDBoard   )   (*Iter)->pIDBoard->Render();

	//	DmgListIter     = (*Iter)->pDmgBoard.begin();
	//	EmoListIter     = (*Iter)->pImotioconBoard.begin();

	//	for( ; DmgListIter != (*Iter)->pDmgBoard.end() ; ++DmgListIter )
	//	{
	//		(*DmgListIter)->Render();
	//	}

	//	for( ; EmoListIter != (*Iter)->pImotioconBoard.end() ; ++EmoListIter )
	//	{
	//		(*EmoListIter)->Render();
	//	}
	//}
}

void AgcmTextBoardMng::Update( INT32 TickDiff , RwMatrix* CamMat )
{
	BoardDataListIter   Iter   = m_BoardDataList.begin();

	INT	lIndex	=	0;

	AgpdCharacter*	ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence( &lIndex );
	for( ; ppdCharacter ; ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence(&lIndex) )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
		if( pcdCharacter && pcdCharacter->m_pvTextboard )
			_Update( (sBoardDataPtr)pcdCharacter->m_pvTextboard , TickDiff , (ApBase*)pcdCharacter );

	}

	lIndex	=	0;

	AgpdItem*		ppdItem			=	m_pcsAgpmItem->GetItemSequence( &lIndex );
	for( ; ppdItem ; ppdItem = m_pcsAgpmItem->GetItemSequence(&lIndex) )
	{
		AgcdItem*		pcdItem		=	m_pcsAgcmItem->GetItemData( ppdItem );
		if( pcdItem && pcdItem->m_pvTextboard )
			_Update( (sBoardDataPtr)pcdItem->m_pvTextboard , TickDiff , (ApBase*)pcdItem );
	}

	m_TickDiff    = TickDiff;

}

VOID	AgcmTextBoardMng::_Update( sBoardDataPtr	pBoardData , INT32 TickDiff , ApBase*	pBase )
{
	std::list< AgcmDmgBoard*      >::iterator   DmgListIter;
	std::list< AgcmImoticonBoard* >::iterator   EmoListIter;
	
	if( !pBoardData )
		return;

	// ID Update
	if( pBoardData->pIDBoard )
		pBoardData->pIDBoard->Update( TickDiff , NULL );

	// HPBar Update
	if( pBoardData->pHPBar )
		pBoardData->pHPBar->Update( TickDiff , NULL );

	// Text board Update
	if( pBoardData->pTextBoard )
	{
		if( pBoardData->pIDBoard && pBoardData != GetMainCharacter() )
			pBoardData->pTextBoard->SetIDBoard( pBoardData->pIDBoard );

		else
			pBoardData->pTextBoard->SetIDBoard( GetMainCharacter()->pHPBar );

		if( !pBoardData->pTextBoard->Update( TickDiff , NULL ) )
			SAFE_DELETE( pBoardData->pTextBoard );
	}

	// Damage Update
	DmgListIter   = pBoardData->pDmgBoard.begin();
	EmoListIter   = pBoardData->pImotioconBoard.begin();

	// Damage
	for( ; DmgListIter != pBoardData->pDmgBoard.end() ; )
	{
		if( (*DmgListIter)->Update( TickDiff , NULL ) == FALSE )
		{
			SAFE_DELETE( (*DmgListIter) );

			pBoardData->pDmgBoard.erase( DmgListIter++ );
		}
		else     
			++DmgListIter;
	}

	// Imo Update
	for ( ; EmoListIter != pBoardData->pImotioconBoard.end() ; )
	{
		if( (*EmoListIter)->Update( TickDiff , NULL ) == FALSE )
		{
			SAFE_DELETE( (*EmoListIter) );
			pBoardData->pImotioconBoard.erase( EmoListIter++ );
		}
		else   
			++EmoListIter;
	}


	//for( ; Iter != m_BoardDataList.end() ; ++Iter )
	//{
	//	// IDBoard Update
	//	if( (*Iter)->pIDBoard	)	(*Iter)->pIDBoard->Update( TickDiff , CamMat );


	//	// HPBar Update
	//	if( (*Iter)->pHPBar		)	(*Iter)->pHPBar->Update( TickDiff, CamMat );

	//	// TextBoard Update
	//	if( (*Iter)->pTextBoard )
	//	{
	//		if( (*Iter)->pIDBoard && (*Iter) != GetMainCharacter() )	(*Iter)->pTextBoard->SetIDBoard( (*Iter)->pIDBoard );
	//		else 														(*Iter)->pTextBoard->SetIDBoard( GetMainCharacter()->pHPBar );

	//		if( (*Iter)->pTextBoard->Update( TickDiff , CamMat ) == FALSE )	
	//		{
	//			SAFE_DELETE( (*Iter)->pTextBoard );	
	//		}
	//	}

	//	DmgListIter   = (*Iter)->pDmgBoard.begin();
	//	EmoListIter   = (*Iter)->pImotioconBoard.begin();

	//	// Damage
	//	for( ; DmgListIter != (*Iter)->pDmgBoard.end() ; )
	//	{
	//		if( (*DmgListIter)->Update( TickDiff , CamMat ) == FALSE )
	//		{
	//			SAFE_DELETE( (*DmgListIter) );

	//			(*Iter)->pDmgBoard.erase( DmgListIter++ );
	//		}
	//		else     ++DmgListIter;
	//	}

	//	// Imo Update
	//	for ( ; EmoListIter != (*Iter)->pImotioconBoard.end() ; )
	//	{
	//		if( (*EmoListIter)->Update( TickDiff , CamMat ) == FALSE )
	//		{
	//			SAFE_DELETE( (*EmoListIter) );
	//			(*Iter)->pImotioconBoard.erase( EmoListIter++ );
	//		}
	//		else    ++EmoListIter;
	//	}
	//}

	m_TickDiff    = TickDiff;

}

sBoardDataPtr AgcmTextBoardMng::_CreateImoticon( ApBase* pBase , sBoardAttrPtr pAttr )
{
	AgpdCharacter* pcsAgpdCharacter = static_cast< AgpdCharacter* >(pBase);
	if( !pcsAgpdCharacter )		return FALSE;

	AgcdCharacter* pcsAgcdCharacter = m_pcCmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( !pcsAgcdCharacter )		return FALSE;

	AgcmImoticonBoard*	pImoBoard		= NULL;

	INT32               nType			= pAttr->eType - IMOTICON_MISS;

	sBoardDataPtr		pBoardData		= GetBoard( pBase );

	if( pBoardData == NULL )
	{
		pBoardData						=	new sBoardData;
		

		pBoardData->pBase				=	pBase;
		//m_BoardDataList.push_back( pBoardData );
		pcsAgcdCharacter->m_pvTextboard	=	(PVOID)pBoardData;
	}

	pImoBoard				= new AgcmImoticonBoard;

	pImoBoard->SetBoardType( pAttr->eType );
	pImoBoard->SetAniCount( 0 );
	pImoBoard->SetAniEndCount( pAttr->ViewCount );
	pImoBoard->SetAniType( nType );
	pImoBoard->SetHeight( (FLOAT)pcsAgcdCharacter->GetHeight() + TB_IMOTICON_HEIGHT_OFFSET );
	pImoBoard->SetImageID( nType );
	pImoBoard->SetCharacter( pcsAgpdCharacter );
	pImoBoard->SetClump( pcsAgcdCharacter->m_pClump );
	pImoBoard->SetAniParam( 0 , (INT)(pImoBoard->GetAniEndCount() * 0.5f) );
	pImoBoard->SetAniParam( 1 , 0 );


	switch( pAttr->eType )		
	{
	case IMOTICON_MISS:
		// 미스 사운드 추가
		// added miss sound by Bischoff. 2012.04.03
	case IMOTICON_EVADE:
	case IMOTICON_DODGE:
		m_pcsAgcmUIManager2->ThrowEvent(m_pcsAgcmUIManager2->m_nEventMissSound);
		pImoBoard->SetAniVal( 0 , 16.0f );
		pImoBoard->SetAniVal( 1 , 8.0f  );
		pImoBoard->SetOffset( -32.0f , -32.0f );
		break;

	case IMOTICON_REFLEX:
		m_pcsAgcmUIManager2->ThrowEvent(m_pcsAgcmUIManager2->m_nEventBlockSound);
		pImoBoard->SetAniVal( 0 , 16.0f );
		pImoBoard->SetAniVal( 1 , 8.0f  );
		pImoBoard->SetOffset( -32.0f , -32.0f );
		break;

	case IMOTICON_CRITICAL:
		// 크리티컬 사운드 추가
		// added critical sound by Bischoff. 2012.04.03
		m_pcsAgcmUIManager2->ThrowEvent(m_pcsAgcmUIManager2->m_nEventCriticalSound);
	case IMOTICON_ATTACK:
	case IMOTICON_CHARISMA1:
	case IMOTICON_CHARISMA2:
	case IMOTICON_CHARISMA3:
		pImoBoard->SetOffset( -32.0f , -52.0f );
		break;

	case IMOTICON_BLOCK:
		// 블럭 사운드 추가
		// added block sound by Bischoff. 2012.04.03
		m_pcsAgcmUIManager2->ThrowEvent(m_pcsAgcmUIManager2->m_nEventBlockSound);
		pImoBoard->SetAniVal( 0 , 0.0f );
		pImoBoard->SetAniVal( 1 , 0.5f );
		pImoBoard->SetAniVal( 2 , 2.0f );
		pImoBoard->SetAniVal( 3 , 0.0f );
		pImoBoard->SetOffset( -32.0f , -32.0f );
		break;
	}

	pBoardData->pImotioconBoard.push_back( pImoBoard );


	return pBoardData;
}



sBoardDataPtr AgcmTextBoardMng::_CreateIDBoard( ApBase* pBase , sBoardAttrPtr pAttr )
{

	// check-----------------------------------------------------------------------------
	AgpdCharacter* pdCharacter  = static_cast< AgpdCharacter* >( pBase );
	if( pdCharacter == NULL )     return NULL;

	AgcdCharacter *cdCharacter = m_pcCmCharacter->GetCharacterData(pdCharacter);
	if( cdCharacter == NULL )	  return NULL;

	if( !cdCharacter->m_pClump ) {
		TRACE( "AgcmTextBoardMng::MakeIDBoard - %s 클럼프가 없다", pdCharacter->m_szID );
		return FALSE;
	}

	if ( m_pcCmCharacter->IsNonPickingTID(pdCharacter->m_lTID1)	)	return FALSE;

	INT len = (INT)pAttr->String.size();
	if( !len )				return FALSE;

	AgpdGuildADChar* pADG = m_pcsAgpmGuild->GetADCharacter( pdCharacter );
	AgpdPartyADChar* pADP = m_pcsAgpmParty->GetADCharacter( pdCharacter );
	if ( !pADG || !pADP )	return FALSE;

	//-----------------------------------------------------------------------------------

	DWORD			Color		= 0;
	AgcmIDBoard*	pIDBoard	= new AgcmIDBoard;
	pIDBoard->SetBoardType( AGCM_BOARD_IDBOARD );

	sBoardDataPtr pBoardData	= GetBoard( pBase );

	if( pBoardData )
	{
		SAFE_DELETE( pBoardData->pIDBoard );
		pBoardData->pIDBoard  = pIDBoard;
	}

	else
	{
		pBoardData				= new sBoardData;

		pBoardData->pIDBoard    = pIDBoard;
		pBoardData->pBase       = pBase;

		//m_BoardDataList.push_back( pBoardData );
		cdCharacter->m_pvTextboard	=	(PVOID)pBoardData;
	}

	pIDBoard->SetHeight( (FLOAT)cdCharacter->GetHeight() );
	pIDBoard->SetDepth( cdCharacter->GetDepth() );
	pIDBoard->SetPosition( &pdCharacter->m_stPos );
	pIDBoard->SetClump( cdCharacter->m_pClump );
	pIDBoard->SetEnabled( TRUE );
	pIDBoard->SetGuild( FALSE );
	pIDBoard->SetEnableGuild( 0 );
	pIDBoard->SetParty( FALSE );
	pIDBoard->SetEnableParty( 0 );
	
	if( m_pcsAgpmCharacter->IsMonster( pdCharacter ) )
	{
		INT32	lLevel1 = 0;
		INT32	lLevel2 = 0;


		pIDBoard->SetIDType( TB_MONSTER );

		if(!m_bDrawNameMonster)	pIDBoard->SetEnabled( FALSE );

		// ARGB 순서를 ABGR로 바꾸자.. renderware로 그리는 것은 BGR순서다..
		char a = (pdCharacter->m_pcsCharacterTemplate->m_ulIDColor & 0xff000000) >> 24;
		char r = (pdCharacter->m_pcsCharacterTemplate->m_ulIDColor & 0x00ff0000) >> 16;
		char g = (pdCharacter->m_pcsCharacterTemplate->m_ulIDColor & 0x0000ff00) >> 8;
		char b = (pdCharacter->m_pcsCharacterTemplate->m_ulIDColor & 0x000000ff);

		Color = ARGB32_TO_DWORD(a,r,g,b);
	}
	else if(m_pcsAgpmCharacter->IsNPC( pdCharacter ))
	{
		pIDBoard->SetIDType( TB_NPC );
		Color		= 0xff0099ff;
	}
	else 
	{
		pIDBoard->SetIDType( TB_OTHERS );
		Color		= 0xff0099ff;
		if( !m_bDrawNameOthers ) pIDBoard->SetEnabled( FALSE );
	}

	if(m_pcsAgpmCharacter->IsPC(pdCharacter))
	{
		Color = cdCharacter->m_dwCharIDColor ? cdCharacter->m_dwCharIDColor : 0xFFFFFFFF;
	}
	
	pIDBoard->SetApBase( pBase );
	pIDBoard->SetText( pAttr->String );
	pIDBoard->SetColor( Color );
	pIDBoard->SetFontType( pAttr->fType );
	pIDBoard->ReCalulateOffsetX();

	char TmpStr[ TB_ID_MAX_CHAR + 2 ];
	ZeroMemory( TmpStr , TB_ID_MAX_CHAR + 2 );

	for( INT i = 0 ; i < (INT)pAttr->String.size() ; ++i )
	{
		TmpStr[ i ] = '*' ;
	}
	pIDBoard->SetBattleText( TmpStr );
	pIDBoard->ReCalulateOffsetX();

	pIDBoard->SetCharismaStep( m_pcsAgpmCharacter->GetCharismaStep( pdCharacter ) - 1 );

	char* szCharismaTitle = m_pcsAgpmCharacter->GetCharismaTitle( pdCharacter );
	if( szCharismaTitle )
	{
		pIDBoard->SetCharismaText( szCharismaTitle );
		pIDBoard->SetColorCharisma( 0xffffffff );
		pIDBoard->ReCalulateOffsetX();
	}

	pIDBoard->SetNickNameText( pdCharacter->m_szNickName );
	pIDBoard->SetColorNickName( 0xffffffff );
	pIDBoard->ReCalulateOffsetX();

	int TextExtent = m_pcFont->GetTextExtent( pAttr->fType , (char*)pAttr->String.c_str() , len );
	int	TextHeight = m_pcFont->m_iYL[ pAttr->fType ];

	// offset은 공간 활용 funcs에서 구해준다. (아직 구현 x)
	RwV2d   Offset;
	Offset.x      = -(FLOAT)TextExtent / 2 ;
	Offset.y      = -(FLOAT)TextHeight;

	pIDBoard->SetOffset( Offset );

	if (strlen(pADG->m_szGuildID))
	{
#ifndef USE_MFC
		DWORD dwGuildColor;

		try
		{
			dwGuildColor = GetGuildIDColor(pADG->m_szGuildID, pdCharacter->m_szID);
		}
		catch( ... )
		{
			dwGuildColor = 0xffffffff;
		}
		EnableGuild(pBase, pADG->m_szGuildID, dwGuildColor);
#endif
	}

	if (pADP->lPID)  EnableParty(pBase);

	pIDBoard->SetAlpha( 255 );
	pIDBoard->SetVisible( TRUE );

	m_pcRender->AddUpdateInfotoClump( pIDBoard->GetClump() ,this,CB_UpdateIDBoard,NULL,pBoardData,NULL);
	

	return pBoardData;
}

RwV2d AgcmTextBoardMng::GetTextureSize( RwTexture* pTexture )
{
	RwV2d   vTex   = { (RwReal)RwRasterGetWidth ( RwTextureGetRaster(pTexture) ) , 
		(RwReal)RwRasterGetHeight( RwTextureGetRaster(pTexture) )   };

	return vTex;
}

sBoardDataPtr AgcmTextBoardMng::EnableHPBar( ApBase* pBase , BOOL bRemove , INT32 nRemoveCount )
{
	if( pBase->m_eType != APBASE_TYPE_CHARACTER	)
		return NULL;

	BOOL         bFind   = FALSE;
	AgcmHPBar*   pHPBar  = NULL;
	
	sBoardDataPtr		pBoardData		=	NULL;
	AgpdCharacter*		pdCharacter		=	static_cast< AgpdCharacter* >(pBase );
	AgcdCharacter*		cdCharacter		=	m_pcCmCharacter->GetCharacterData( pdCharacter );
	if( !pdCharacter || !cdCharacter )    
		return NULL;


	if( GetMainCharacter() && GetMainCharacter()->pBase	==	pBase )
	{
		pBoardData		=	GetMainCharacter();
	}
	else
	{
		pBoardData		=	GetBoard( pBase );
	}

	if( !pBoardData )
		return NULL;

	if( pBoardData->pHPBar == NULL )
	{
		if( pdCharacter == NULL )                        
			return NULL;

		if( !m_pcsAgpmCharacter->IsMonster(pdCharacter) )  
			return NULL;

		pBoardData->pHPBar	  =	new AgcmHPBar;
		pBoardData->pHPBar->SetBoardType( AGCM_BOARD_HPBAR );
		
		pBoardData->pHPBar->SetDraw( FALSE );
		pBoardData->pHPBar->SetApBase( pBase );
		pBoardData->pHPBar->SetPosition( &pdCharacter->m_stPos );

		pBoardData->pHPBar->SetRemove( bRemove );
		pBoardData->pHPBar->SetRemoveCount( nRemoveCount );
		pBoardData->pHPBar->SetClump( cdCharacter->m_pClump );
		pBoardData->pHPBar->SetHeight( (FLOAT)cdCharacter->GetHeight() );

		pBoardData->pHPBar->GetOffset().x    = -60.0f;
		pBoardData->pHPBar->GetOffset().y    = -TB_HP_OFFSET;

		pBoardData->pHPBar->SetDecreaseHP( FALSE );
		pBoardData->pHPBar->SetPerResultHP( GetPointPercent( pdCharacter , 0 ) );
		pBoardData->pHPBar->SetVisible( TRUE );
	}

	if( pBoardData->pIDBoard &&  m_bDrawNameMonster == FALSE  )
	{
		if( pBoardData->pIDBoard->GetApBase() == pBase &&
			pBoardData->pIDBoard->GetIDType() & TB_MONSTER )
		{
			pBoardData->pIDBoard->SetEnabled( TRUE );
		}
	}

	return  pBoardData;

}

void AgcmTextBoardMng::DisableHPBar( ApBase* pBase )
{

	sBoardDataPtr		pBoardData	=	GetBoard( pBase );

	if( GetMainCharacter() && GetMainCharacter()->pBase == pBase ) return;

	if( pBoardData )
	{
		SAFE_DELETE( pBoardData->pHPBar );
	}

	if( !m_bDrawNameMonster || !m_bDrawNameOthers )
	{
		// monster name draw가 false일때 mouse over되서 그려졌던 id disable
		if( pBoardData && pBoardData->pIDBoard )
		{
			if( ( pBoardData->pIDBoard->GetIDType() & TB_MONSTER ) && !m_bDrawNameMonster )			pBoardData->pIDBoard->SetEnabled( FALSE );
			else if( ( pBoardData->pIDBoard->GetIDType() & TB_OTHERS ) && !m_bDrawNameOthers )		pBoardData->pIDBoard->SetEnabled( FALSE );
		}
	}
}

void AgcmTextBoardMng::DisableHPBar( RpClump* pClump )
{
	AgcmHPBar*    pHPBar = NULL;

	if( GetMainCharacter() && GetMainCharacter()->pHPBar )
	{
		if( GetMainCharacter()->pHPBar->GetClump() == pClump )	
			return;
	}

	INT					lIndex			=	0;
	AgpdCharacter*		ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence( &lIndex );
	for( ; ppdCharacter ; ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence(&lIndex) )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
		if( !pcdCharacter || !pcdCharacter->m_pvTextboard )
			continue;

		pHPBar  = ((sBoardData*)pcdCharacter->m_pvTextboard)->pHPBar;

		if( pHPBar && pHPBar->GetClump() == pClump )
		{
			SAFE_DELETE( ((sBoardData*)pcdCharacter->m_pvTextboard)->pHPBar );		
			break;
		}
	}

	if( !m_bDrawNameMonster || !m_bDrawNameOthers )
	{
		// monster name draw가 false일때 mouse over되서 그려졌던 id disable
		AgcmIDBoard*		pIDBoard		=	NULL;
		lIndex			=	0;
		ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence( &lIndex );

		for( ; ppdCharacter ; ppdCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex) )
		{
			AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
			if( !pcdCharacter || !pcdCharacter->m_pvTextboard )
				continue;

			pIDBoard   = ((sBoardData*)pcdCharacter->m_pvTextboard)->pIDBoard;

			if( pIDBoard && pIDBoard->GetClump() == pClump )
			{
				if( ( pIDBoard->GetIDType() & TB_MONSTER ) && !m_bDrawNameMonster )
				{
					pIDBoard->SetEnabled( FALSE );
					break;
				}
				else if( ( pIDBoard->GetIDType() & TB_OTHERS ) && !m_bDrawNameOthers )
				{
					pIDBoard->SetEnabled( FALSE );
					break;
				}
			}
		}
	}

}

sBoardDataPtr AgcmTextBoardMng::EnableGuild( ApBase* pBase , const char* szString , DWORD dwGuildColor )
{
	int   len    = strlen( szString );
	char  TmpString[ TB_ID_MAX_CHAR + 10 ];

	ZeroMemory( TmpString , len+10 );

	sBoardDataPtr  pBoardData   = GetBoard( pBase );
	
	if( pBoardData && pBoardData->pHPBar && pBoardData->pHPBar->GetBoardType() == AGCM_BOARD_HPMPBAR )
	{
		if( m_bDrawNameGuild ) m_pMainCharacterID->pHPBar->SetEnableGuild( 1 );

		m_pMainCharacterID->pHPBar->SetGuild( TRUE );
		m_pMainCharacterID->pHPBar->SetColorGuild( dwGuildColor );

		ZeroMemory( TmpString , len+10 );
		sprintf_s( TmpString , sizeof(TmpString), "[%s]" , szString );

		m_pMainCharacterID->pHPBar->SetGuildText( TmpString );
		m_pMainCharacterID->pHPBar->ReCalulateOffsetX();

		INT TextExtent  = m_pcFont->GetTextExtent( m_pMainCharacterID->pHPBar->GetFontType() , TmpString , len+2 );
		INT TextHeight  = m_pcFont->m_iYL[ m_pMainCharacterID->pHPBar->GetFontType() ];

		m_pMainCharacterID->pHPBar->GetOffsetGuild().x = -(FLOAT)TextExtent / 2 - m_pMainCharacterID->pHPBar->GetOffset().x;
		m_pMainCharacterID->pHPBar->GetOffsetGuild().y = -(FLOAT)( TextHeight + 20.0f );
	}

	else if( pBoardData && pBoardData->pIDBoard )
	{
		if( m_bDrawNameGuild ) pBoardData->pIDBoard->SetEnableGuild( 1 );
		
		pBoardData->pIDBoard->SetGuild( TRUE );
		
		if( pBoardData->pIDBoard->GetColor() == 0xFFFFFFFF || pBoardData->pIDBoard->GetColor() == 0 )
		{
			pBoardData->pIDBoard->SetColorGuild( dwGuildColor );
		}
		else
		{
			pBoardData->pIDBoard->SetColorGuild( pBoardData->pIDBoard->GetColor() );
		}

		sprintf_s( TmpString , sizeof(TmpString), "[%s]" , szString );

		pBoardData->pIDBoard->SetGuildText( TmpString );
		pBoardData->pIDBoard->ReCalulateOffsetX();

		INT TextExtent = m_pcFont->GetTextExtent( pBoardData->pIDBoard->GetFontType() , TmpString , len+2 );
		INT TextHeight = m_pcFont->m_iYL[ pBoardData->pIDBoard->GetFontType() ];

		pBoardData->pIDBoard->GetOffsetGuild().x = -(FLOAT)TextExtent / 2 - pBoardData->pIDBoard->GetOffset().x;
		pBoardData->pIDBoard->GetOffsetGuild().y = -(FLOAT)( TextHeight + 20.0f );
	}

	return pBoardData;
}

void AgcmTextBoardMng::DisableGuild( ApBase* pBase )
{
	sBoardDataPtr pBoardData  = GetBoard( pBase );

	if( pBoardData && pBoardData->pHPBar && pBoardData->pHPBar->GetBoardType() == AGCM_BOARD_HPMPBAR )
	{
		pBoardData->pHPBar->SetEnableGuild( 0 );
		pBoardData->pHPBar->SetGuild( FALSE );
		pBoardData->pHPBar->SetGuildText( "" );
		pBoardData->pHPBar->ReCalulateOffsetX();
	}

	else if( pBoardData && pBoardData->pIDBoard )
	{
		pBoardData->pIDBoard->SetEnableGuild( 0 );
		pBoardData->pIDBoard->SetGuild( FALSE );
		pBoardData->pIDBoard->SetGuildText( "" );
		pBoardData->pIDBoard->ReCalulateOffsetX();
	}
}

sBoardDataPtr AgcmTextBoardMng::EnableParty( ApBase* pBase )
{
	sBoardDataPtr  pBoardData  = GetBoard( pBase );

	if( pBoardData && pBoardData->pHPBar && pBoardData == GetMainCharacter() )
	{
		if( m_bDrawNameParty )	pBoardData->pHPBar->SetEnableParty( 1 );
		pBoardData->pHPBar->SetParty( TRUE );
	}

	else if( pBoardData && pBoardData->pIDBoard )
	{
		if( m_bDrawNameParty ) pBoardData->pIDBoard->SetEnableParty( 1 );
		pBoardData->pIDBoard->SetParty( TRUE );
	}

	return pBoardData;
}

void AgcmTextBoardMng::DisableParty( ApBase* pBase )
{
	sBoardDataPtr pBoardData   = GetBoard( pBase );
	
	if( pBoardData && pBoardData->pHPBar && pBoardData == GetMainCharacter() )
	{
		pBoardData->pHPBar->SetEnableParty( 0 );
		pBoardData->pHPBar->SetParty( FALSE );
	}


	else if( pBoardData && pBoardData->pIDBoard )
	{
		pBoardData->pIDBoard->SetEnableParty( 0 );
		pBoardData->pIDBoard->SetParty( FALSE );
	}
}

void AgcmTextBoardMng::DisableAllParty()
{
	//BoardDataListIter    Iter   = m_BoardDataList.begin();

	//for( ; Iter != m_BoardDataList.end() ; ++Iter )
	//{
	//	if( (*Iter)->pIDBoard )
	//	{
	//		(*Iter)->pIDBoard->SetEnableParty( 0 );
	//		(*Iter)->pIDBoard->SetParty( FALSE );
	//	}
	//	else if( GetMainCharacter() == (*Iter) )
	//	{
	//		(*Iter)->pHPBar->SetEnableParty( 0 );
	//		(*Iter)->pHPBar->SetParty( FALSE );
	//	}
	//}

	INT				lIndex			=	0;
	AgpdCharacter*	ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence(&lIndex );
	for( ; ppdCharacter ; ppdCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex) )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
		if( !pcdCharacter || !pcdCharacter->m_pvTextboard )
			continue;

		sBoardDataPtr	pBoardData	=	static_cast< sBoardDataPtr >(pcdCharacter->m_pvTextboard);
		
		if( pBoardData && pBoardData->pIDBoard )
		{
			pBoardData->pIDBoard->SetEnableParty( 0 );
			pBoardData->pIDBoard->SetParty( FALSE );
		}
		else if( GetMainCharacter() == pBoardData )
		{
			pBoardData->pHPBar->SetEnableParty( 0 );
			pBoardData->pHPBar->SetParty( FALSE );
		}
	}

}

sBoardDataPtr AgcmTextBoardMng::EnableIDFlag( ApBase* pBase , DWORD dwFlag )
{
	sBoardDataPtr pBoardData   = GetBoard( pBase );
	
	if( pBoardData && pBoardData->pIDBoard )
	{
		pBoardData->pIDBoard->SetFlag( (pBoardData->pIDBoard->GetFlag() | dwFlag) );
	}

	else if( pBoardData && pBoardData->pHPBar )
	{
		pBoardData->pHPBar->SetFlag( (pBoardData->pHPBar->GetFlag() | dwFlag) );
	}

	return pBoardData;
}

void AgcmTextBoardMng::DisableIDFlag( ApBase* pBase , DWORD dwFlag )
{
	sBoardDataPtr  pBoardData  = GetBoard( pBase );
	if( pBoardData && pBoardData->pIDBoard )
	{
		pBoardData->pIDBoard->SetFlag( (pBoardData->pIDBoard->GetFlag() & ~dwFlag) );
	}
	
	else if( pBoardData && pBoardData->pHPBar )
	{
		pBoardData->pHPBar->SetFlag( (pBoardData->pHPBar->GetFlag() & ~dwFlag ) );
	}
}

sBoardDataPtr AgcmTextBoardMng::_CreateHPBar( ApBase* pBase , BOOL bRemove , INT32 nRemoveCount )
{
	AgpdCharacter*    pdCharacter   = (AgpdCharacter*)pBase;

	if( pdCharacter == NULL )                          
		return NULL;

	if( m_pcsAgpmCharacter->IsMonster(pdCharacter) )   
		return NULL;

	AgcdCharacter* cdCharacter      = m_pcCmCharacter->GetCharacterData( pdCharacter );
	if( cdCharacter == NULL )                          
		return NULL;

	
	AgcmHPBar*	pHPBar	=	new AgcmHPBar;

	pHPBar->SetBoardType( AGCM_BOARD_HPBAR );
	pHPBar->SetDraw( FALSE );
	pHPBar->SetApBase( pBase );
	pHPBar->SetPosition( &pdCharacter->m_stPos );
	pHPBar->SetRemove( bRemove );
	pHPBar->SetRemoveCount( nRemoveCount );
	pHPBar->SetClump( cdCharacter->m_pClump );
	pHPBar->SetHeight( (FLOAT)cdCharacter->GetHeight() );
	pHPBar->SetDecreaseHP( FALSE );
	pHPBar->SetPerResultHP( GetPointPercent( pdCharacter , 0 ) );

	sBoardDataPtr pBoardData = GetBoard( pBase );

	if( pBoardData )
	{
		SAFE_DELETE( pBoardData->pHPBar );
		pBoardData->pHPBar = pHPBar;
	}

	else
	{
		pBoardData			= new sBoardData;

		pBoardData->pBase	= pBase;
		pBoardData->pHPBar	= pHPBar;

		cdCharacter->m_pvTextboard	=	(PVOID)pBoardData;
	}
		
	return pBoardData;
}

sBoardDataPtr AgcmTextBoardMng::_CreateDmg( ApBase* pBase , sBoardAttrPtr pAttr )
{
	AgpdCharacter*  pcsAgpdCharacter = static_cast< AgpdCharacter* >( pBase );
	if( pcsAgpdCharacter == NULL )		
		return NULL;
	
	AgcdCharacter*  pcsAgcdCharacter = m_pcCmCharacter->GetCharacterData(pcsAgpdCharacter);
	if( !pcsAgcdCharacter )				
		return NULL;

	if( !pcsAgcdCharacter->m_pClump )	
		return NULL;

	
	AgcmDmgBoard*	pDmgBoard	=	new AgcmDmgBoard;

	pDmgBoard->SetBoardType( pAttr->eType );
	pDmgBoard->SetHeight( pcsAgcdCharacter->GetHeight() );
	pDmgBoard->SetCharacter( pcsAgpdCharacter );
	pDmgBoard->SetAniCount( 0 );
	pDmgBoard->SetClump( pcsAgcdCharacter->m_pClump );

	BOOL bMainCharacter = FALSE;
	if( GetMainCharacter() && GetMainCharacter()->pBase	==	pBase )
	{
		bMainCharacter		=	TRUE;
	}

	int temp	= pAttr->nDamage;
	int i		= 0;
	int	xoffset;
	char Dmg[ TB_DAMAGE_MAX_NUM ];
	ZeroMemory( Dmg, TB_DAMAGE_MAX_NUM );

	if( pAttr->eType == AGCM_BOARD_DAMAGE ) 
	{
		pDmgBoard->SetAniEndCount( 800 );
		pDmgBoard->GetOffset().y        = -(FLOAT)55.0f;
		xoffset = bMainCharacter ? -17 : -29;

		while( temp > 0 && i < TB_DAMAGE_MAX_NUM )
		{
			Dmg[i++] = temp % 10;
			if(bMainCharacter)
			{
				xoffset += ( int ) (m_fDamageSizeSmall[temp%10] * 0.5f);
			}
			else
			{
				xoffset += ( int ) (m_vDamageUVW[temp%10].z * 0.5f);
			}

			temp /= 10;
		}
	}

	else
	{
		pDmgBoard->SetAniEndCount( 700 );
		xoffset = -29;
		pDmgBoard->GetOffset().y        = -(FLOAT)105.0f;

		while(temp > 0 && i<TB_DAMAGE_MAX_NUM)
		{
			Dmg[i++] = temp % 10;
			xoffset += ( int ) (m_vDamageUVW[temp%10].z * 0.5f);

			temp /= 10;
		}
	}
	
	pDmgBoard->SetNumCount( i );
	pDmgBoard->GetColor().red		=  (unsigned char)((pAttr->Color & 0x00ff0000) >> 16);
	pDmgBoard->GetColor().green	    =  (unsigned char)((pAttr->Color & 0x0000ff00) >> 8);
	pDmgBoard->GetColor().blue	    =  (unsigned char)( pAttr->Color & 0x000000ff);
	pDmgBoard->GetOffset().x        =  (FLOAT)xoffset;
	
	sBoardDataPtr   pBoardData      = GetBoard( pBase );

	if( pBoardData )
	{
		pBoardData->pDmgBoard.push_back( pDmgBoard );
	}

	else
	{
		pBoardData            = new sBoardData;
		pBoardData->pBase     = pBase;

		pBoardData->pDmgBoard.push_back( pDmgBoard );
		pcsAgcdCharacter->m_pvTextboard	=	(PVOID)pBoardData;
	}

	pDmgBoard->SetVisible( TRUE );
	pDmgBoard->SetAlpha( 255 );
	pDmgBoard->SetDamage( Dmg );

	return pBoardData;
}

sBoardDataPtr AgcmTextBoardMng::_CreateText( ApBase* pBase , sBoardAttrPtr pAttr )
{
	if( !m_bMakeTextBaloon )			
		return NULL;

	AgpdCharacter *pcsAgpdCharacter  = static_cast< AgpdCharacter* >( pBase );
	if( !pcsAgpdCharacter )				
		return NULL;

	AgcdCharacter *pcsAgcdCharacter = m_pcCmCharacter->GetCharacterData(pcsAgpdCharacter);
	if( !pcsAgcdCharacter )				
		return NULL;

	if( !pcsAgcdCharacter->m_pClump )	
		return NULL;

	if( m_pcCmCharacter->IsNonPickingTID( pcsAgpdCharacter->m_lTID1 ) )		return NULL;

	sBoardDataPtr pBoardData = GetBoard( pBase );
	if( !pBoardData )
	{
		pBoardData			= new sBoardData;
		pBoardData->pBase	= pBase;
		//m_BoardDataList.push_back( pBoardData );

		pcsAgcdCharacter->m_pvTextboard	=	(PVOID)pBoardData;
	}

	if( pBoardData && pBoardData->pIDBoard )
	{
		pBoardData->pIDBoard->SetEnabled( FALSE );

		if( pBoardData->pIDBoard->GetEnableGuild() == 1 )   pBoardData->pIDBoard->SetEnableGuild( 2 );
		if( pBoardData->pIDBoard->GetEnableParty() == 1 )	pBoardData->pIDBoard->SetEnableParty( 2 );

	}
	else
	{
		if( m_pMainCharacterID->pBase == (ApBase*)pcsAgpdCharacter )
		{
			m_pMainCharacterID->pHPBar->SetEnabled( FALSE );

			if( m_pMainCharacterID->pHPBar->GetEnableGuild() == 1 )
				m_pMainCharacterID->pHPBar->SetEnableGuild( 2 );

			if( m_pMainCharacterID->pHPBar->GetEnableParty() == 1 )
				m_pMainCharacterID->pHPBar->SetEnableParty( 2 );
		}
	}
	
	// 기존에 없다면 만든다
	if( pBoardData && pBoardData->pTextBoard == NULL )
	{
		pBoardData->pTextBoard	= new AgcmTextBoard;
		pBoardData->pTextBoard->SetBoardType( AGCM_BOARD_TEXT );
	}
	
	pBoardData->pTextBoard->SetTextColor( pAttr->Color );
	pBoardData->pTextBoard->SetHeight   ( (FLOAT)pcsAgcdCharacter->GetHeight() );
	pBoardData->pTextBoard->SetLifeCount( pAttr->ViewCount );
	pBoardData->pTextBoard->SetCharacter( pcsAgpdCharacter );
	pBoardData->pTextBoard->SetFontType ( pAttr->fType );
	pBoardData->pTextBoard->SetLineCount(   0   );
	
	char  TmpStr [ (TB_TEXT_MAX_CHAR + 2) * 3 ];
	INT   len      = 0;
	INT   len2     = 0;
	INT	  maxwidth = 0;

	strcpy_s( TmpStr , (TB_TEXT_MAX_CHAR+2)*3 , pAttr->String.c_str() );
	len   = (INT)strlen( TmpStr );

	pBoardData->pTextBoard->InitText();

	for( int i=0; i<len; )
	{
		while(i+len2 < len)
		{
			if( m_pcFont->IsLeadByte( *(TmpStr + i + len2) ) ) // 한글이라면
			{
				if( len2 + 2 > TB_TEXT_MAX_CHAR )
					break;
				len2 += 2;
			}
			else 
			{
				if( len2 + 1 > TB_TEXT_MAX_CHAR )
					break;
				++len2;
			}
		}
		TmpStr[ len2 ] = '\0';
		pBoardData->pTextBoard->SetText( pBoardData->pTextBoard->GetLineCount() , TmpStr+i , len2 );
		pBoardData->pTextBoard->SetLineCount( pBoardData->pTextBoard->GetLineCount() +1 );
		
		if( pBoardData->pTextBoard->GetLineCount() >= TB_TEXT_MAX_CHARLINE)    break;

		int textextent = m_pcFont->GetTextExtent( pAttr->fType , TmpStr+i , len2 );
		if(textextent > maxwidth) maxwidth = textextent;

		i += len2;
		len2 = 0;
	}
	pBoardData->pTextBoard->SetBoardW( maxwidth + 16 );
	pBoardData->pTextBoard->SetBoardH( pBoardData->pTextBoard->GetLineCount() * m_pcFont->m_iYL[pAttr->fType] + 10 );
	
	// offset은 공간 활용 funcs에서 구해준다. (아직 구현 x)
	pBoardData->pTextBoard->GetOffset().x   = -(FLOAT)(pBoardData->pTextBoard->GetBoardW() >> 1);
	pBoardData->pTextBoard->GetOffset().y   = -(FLOAT)(pBoardData->pTextBoard->GetBoardH() >> 1) - TB_TEXT_HEIGHT_OFFSET;
	pBoardData->pTextBoard->SetTailType( rand() % 3 );

	pBoardData->pTextBoard->SetClump( pcsAgcdCharacter->m_pClump );
	pBoardData->pTextBoard->SetDraw( TRUE );

	return pBoardData;
}


sBoardDataPtr AgcmTextBoardMng::_CreateItemID( ApBase* pBase , sBoardAttrPtr pAttr )
{

	// check--------------------------------------------------
	AgpdItem* pdItem = static_cast< AgpdItem* >(pBase);
	if( pdItem == NULL )		
		return NULL;

	AgcdItem* cdItem = m_pcsAgcmItem->GetItemData(pdItem);
	if( cdItem == NULL )		
		return NULL;

	if(!cdItem->m_pstClump) 
	{
		TRACE("AgcmTextBoardMng::MakeItemID 클럼프가 없다");
		return NULL;
	}

	INT len = (INT)(pAttr->String).size();
	if( !len )			
		return NULL;
	//-----------------------------------------------------------

	AgcmIDBoard*     pIDBoard = NULL;

	int TextExtent = m_pcFont->GetTextExtent( pAttr->fType , (char*)pAttr->String.c_str() , len );
	int	TextHeight = m_pcFont->m_iYL[ pAttr->fType ];

	// offset은 공간 활용 funcs에서 구해준다. (아직 구현 x)
	RwV2d   Offset;
	Offset.x      = -(FLOAT)TextExtent / 2;
	Offset.y      = -(FLOAT)TextHeight;

	pIDBoard = new AgcmIDBoard;
	pIDBoard->SetBoardType( AGCM_BOARD_ITEMID );

	//set
	pIDBoard->SetHeight( 60.0f );
	pIDBoard->SetClump( cdItem->m_pstClump );
	pIDBoard->SetGuild( FALSE );
	pIDBoard->SetParty( FALSE );
	pIDBoard->SetEnableGuild( 0 );
	pIDBoard->SetEnableParty( 0 );
	pIDBoard->SetEnabled( TRUE );
	pIDBoard->SetPosition( &pdItem->m_posItem );
	pIDBoard->SetApBase( pBase );
	pIDBoard->SetText( pAttr->String );
	pIDBoard->SetColor( pAttr->Color );
	pIDBoard->SetFontType( pAttr->fType );
	pIDBoard->SetOffset( Offset );
	pIDBoard->SetVisible( TRUE );
	pIDBoard->SetDepth( 0.0f );
	pIDBoard->ReCalulateOffsetX();

	sBoardDataPtr pBoardData = new sBoardData;

	pBoardData->pIDBoard         = pIDBoard;
	pBoardData->pBase	         = pBase;

	//m_BoardDataList.push_back( pBoardData );
	cdItem->m_pvTextboard	=	(PVOID)pBoardData;


	return pBoardData;
}

FLOAT AgcmTextBoardMng::GetPointPercent( AgpdCharacter* pdCharacter, INT32 nMode )
{
	AgpdCharacter* pTarget		=	pdCharacter;

	AgpdFactor* pResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor( &pTarget->m_csFactor, AGPD_FACTORS_TYPE_RESULT );
	if( !pResult )
		pResult	= &pTarget->m_csFactor;

	AgpdFactorCharPointType  	   ePoint;
	AgpdFactorCharPointMaxType     ePointMax;

	switch( nMode )
	{
	case 0:
		ePoint		= AGPD_FACTORS_CHARPOINT_TYPE_HP;
		ePointMax	= AGPD_FACTORS_CHARPOINTMAX_TYPE_HP;
		break;
	case 1:
		ePoint		= AGPD_FACTORS_CHARPOINT_TYPE_MP;
		ePointMax	= AGPD_FACTORS_CHARPOINTMAX_TYPE_MP;
		break;
	}

	INT32 nCur = 0;
	INT32 nMax = 0;

	m_pcsAgpmFactors->GetValue( pResult, &nCur, AGPD_FACTORS_TYPE_CHAR_POINT, ePoint );
	m_pcsAgpmFactors->GetValue( pResult, &nMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, ePointMax );

	nCur = nCur < 0 ? 0 : nCur;
	nMax = nMax < 0 ? 0 : nMax;

	nCur = min( nCur, nMax );
	return nMax <= 0 ? 0.f : (float)nCur / (float)nMax;
}

DWORD AgcmTextBoardMng::GetGuildIDColor( const char* szMemberID )
{
	ApAutoLockCharacter Lock( m_pcsAgpmCharacter , (char*)szMemberID );

	AgpdCharacter*  pcsCharacter   = Lock.GetCharacterLock();
	if( pcsCharacter == NULL )  return 0xff81fab2;

	char* szGuildID                = m_pcsAgpmGuild->GetJoinedGuildID( pcsCharacter );
	if( szGuildID    == NULL )  return 0xff81fab2;

	return GetGuildIDColor( szGuildID , szMemberID );
}

DWORD AgcmTextBoardMng::GetGuildIDColor( const char* szGuildID , const char* szMemberID , DWORD dwDefaultColor /* = 0xFF81FAB2 */ )
{
	if( !szGuildID || !szGuildID[0] ) return dwDefaultColor;

	DWORD dwColor         = dwDefaultColor;
	AgpdGuild* pcsGuild   = m_pcsAgpmGuild->GetGuildLock( (char*)szGuildID );

	if( pcsGuild == NULL )
	{
		AgpdGuildADChar* pcsGuildADChar   = m_pcsAgpmGuild->GetADCharacter( m_pcCmCharacter->GetSelfCharacter() );
		if( pcsGuildADChar && m_pcsAgpmGuild->IsJointGuild( pcsGuildADChar->m_szGuildID , (char*)szGuildID ) )
		{
			dwColor     = 0xff009900;
		}

		return dwColor;
	}
	
	AgpdGuildMember* pcsMember = m_pcsAgpmGuild->GetMember( pcsGuild , (char*)szMemberID );
	
	if( pcsMember )
	{
		switch( pcsMember->m_lRank )
		{
		case AGPMGUILD_MEMBER_RANK_MASTER:
		case AGPMGUILD_MEMBER_RANK_NORMAL:        
			dwColor = dwDefaultColor;  
			break;

		case AGPMGUILD_MEMBER_RANK_JOIN_REQUEST:  
		case AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST: 
			dwColor = 0xffa0a0a0;    
			break;
		}
	}

	pcsGuild->m_Mutex.Release();

	return dwColor;
}			

void	AgcmTextBoardMng::DrawGuildImage( AgcmIDBoard* pIDBoard, RwReal fPosX, RwTexture* pTexture, DWORD dwColor, float fScale, int nJoinMax, int nJoinIndex, BOOL bIsJoinSiegeWar )
{
	if( !pIDBoard|| !pTexture )	return;

	RwV2d	vOffset;
	RwV2d	vImageSize = GetTextureSize( pTexture );

	//vOffset.x = ( fPosX > pIDBoard->GetOffset().x + pIDBoard->GetScreenPos().x ) ? pIDBoard->GetOffset().x : pIDBoard->GetOffset().x + pIDBoard->GetOffsetGuild().x;
	//vOffset.x -= ( vImageSize.x + 4.f );

	vOffset.x = pIDBoard->GetOffsetXLeft() - ( vImageSize.x + 4.f );
	if( bIsJoinSiegeWar )	vOffset.x -= ( vImageSize.x + 4.f );

	//INT32		nCharismaIndex		=	0;
	//INT32		nCharismaYOffset	=	0;

	////CarismaPoint
	//if( pIDBoard->GetCharismaStep() >= 0 )
	//{
	//	nCharismaIndex		=	pIDBoard->GetCharismaStep();
	//	nCharismaYOffset	=	GetTextureSize( m_vecTexCharisma[nCharismaIndex] ).y;
	//}

	vOffset.y = pIDBoard->GetOffset().y - ( vImageSize.y * .5f )/* - nCharismaYOffset*/;

	if( nJoinMax )		CalculateJointGuildMarkPositionOffset( &vOffset, vImageSize.x * fScale, nJoinMax, nJoinIndex );

	g_pEngine->DrawIm2DInWorld( pTexture, &pIDBoard->GetCameraPos(), &pIDBoard->GetScreenPos(),pIDBoard->GetRecipz() , &vOffset, vImageSize.x * fScale, vImageSize.y * fScale, 0.0f, 0.0f, 1.0f, 1.0f, dwColor );
}

void	AgcmTextBoardMng::CalculateJointGuildMarkPositionOffset(RwV2d* pOffset, float fSize, int TotalMarkCount, int CurrentMarkIndex)
{
	//. Join Guild마크에 따른 offset 위치 조정
	if(TotalMarkCount >= 3)
	{
		switch(CurrentMarkIndex)
		{
		case 1:
			{
				pOffset->x = pOffset->x + fSize * 0.5f;
			}
			break;
		case 2:
			{
				pOffset->y = pOffset->y + fSize;
			}
			break;
		case 3:
			{
				pOffset->y = pOffset->y + fSize;
				pOffset->x = pOffset->x + fSize;
			}
			break;
		}
	}
	else if(TotalMarkCount >= 2)
	{
		switch(CurrentMarkIndex)
		{
		case 2:
			{
				pOffset->y = pOffset->y + fSize;
				pOffset->x = pOffset->x + fSize;
			}
			break;
		}
	}
}


void AgcmTextBoardMng::ChangeIDColor(AgpdCharacter* pCharacter,DWORD color)
{
	
	DWORD				dwGuildColor	=	GetGuildIDColor(pCharacter->m_szID);
	INT					lIndex			=	0;
	AgpdCharacter*		ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence( &lIndex );
	AgcmIDBoard*		pIDBoard		=	NULL;
	for( ; ppdCharacter ; ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence( &lIndex ) )
	{
		if( ppdCharacter == pCharacter )
		{
			AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
			if( pcdCharacter && pcdCharacter->m_pvTextboard )
			{
				pIDBoard	=	((sBoardDataPtr)pcdCharacter->m_pvTextboard)->pIDBoard;
				if( pIDBoard && pIDBoard->GetApBase() == (ApBase*)pCharacter )
				{
					pIDBoard->SetColor( color );

					if( color != 0xFFFFFFFF && color != 0 )
						pIDBoard->SetColorGuild( color );

					else
						pIDBoard->SetColorGuild( dwGuildColor );

					break;
				}
			}
		}
	}

}

DWORD AgcmTextBoardMng::GetTargetIDColor( AgpdCharacter* pcsCharacter )
{
	if( pcsCharacter == NULL )   return 0xffffffff;

	DWORD dwColor = 0xffffffff;
	AgpdCharacter*  pcsSelfCharacter = m_pcCmCharacter->GetSelfCharacter();

	// 타켓이 자신이다
	if( pcsSelfCharacter == pcsCharacter )
	{
		if( m_pcsAgpmPvP->IsCombatPvPMode( pcsCharacter ) )
		{
			//dwColor   = 0xFF2AE180;
		}
	}

	// 타켓이 다른 캐릭
	else if(pcsSelfCharacter && !m_pcsAgpmCharacter->IsNPC(pcsCharacter) &&
			(m_pcsAgpmCharacter->IsStatusSummoner(pcsCharacter) || !m_pcsAgpmCharacter->IsMonster(pcsCharacter)))
	{

		//적일 경우
		AgcmGuild* pcsAgcmGuild = (AgcmGuild*)GetModule("AgcmGuild");
		if( m_pcsAgpmPvP->IsEnemyCharacter( pcsSelfCharacter , pcsCharacter , FALSE ) ||
			pcsAgcmGuild->IsGuildBattleMember( pcsSelfCharacter , pcsCharacter) )
		{
			dwColor   = 0xFF0000FF;   // Red
		}

		// 같은편
		else if( m_pcsAgpmPvP->IsFriendCharacter(pcsSelfCharacter , pcsCharacter))
		{
			CHAR* szSelfGuildID   = m_pcsAgpmGuild->GetJoinedGuildID( pcsSelfCharacter );
			CHAR* szTargetGuildId = m_pcsAgpmGuild->GetJoinedGuildID( pcsCharacter );
		
			if( szSelfGuildID   && _tcslen( szSelfGuildID   ) > 0 &&
				szTargetGuildId && _tcslen( szTargetGuildId )  )
			{
				// 둘다 길드원인데 길드는 다르다
				if( _tcscmp( szSelfGuildID , szTargetGuildId ) != 0 )
				{
					if( m_pcsAgpmGuild->IsJointGuild( szSelfGuildID , szTargetGuildId ))
					{
						dwColor   = 0xFF009900;  // 
					}

					else if( m_pcsAgpmGuild->IsHostileGuild(szSelfGuildID , szTargetGuildId) )
					{
						dwColor   = 0xFF0000FF;  // Red
					}

					else
					{
						// 자신과 제3자가 CombatArea에 있을때 제 3자는 노란색
						if( m_pcsAgpmPvP->IsCombatPvPMode(pcsSelfCharacter) 
							&& m_pcsAgpmPvP->IsCombatPvPMode(pcsCharacter) )
						{
							dwColor   = 0xFF41E6FF;   // 노랑
						}

						// 제 3자가 전투중일때는 색을 바꾼다
						if( m_pcsAgpmPvP->IsNowPvP(pcsCharacter) )
						{
							dwColor   = 0xFFFFFFFF;
						}
					}
				}

				else
				{
					// 같은 길드일 때는 랭크에 따라 색이 바뀐다
					dwColor  = GetGuildIDColor( szTargetGuildId , pcsCharacter->m_szID , 0xFF2AE180 );
				}
			}

			else
			{
				// 자신과 제3자가 CombatArea 에 있을 때 제3자는 노란색이다
				if( m_pcsAgpmPvP->IsCombatPvPMode(pcsSelfCharacter) && m_pcsAgpmPvP->IsCombatPvPMode(pcsCharacter) )
					dwColor   = 0xFF41E6FF;

				// 제3자가 전투중일때는 색을 바꾼다
				if( m_pcsAgpmPvP->IsNowPvP( pcsCharacter ) )
					dwColor   = 0xFFFFFFFF;
			}

		}
	}

	return dwColor;
}

BOOL	AgcmTextBoardMng::SetTexture( INT32 nType, INT32 nIndex, RwTexture* pTexture )
{
	if( !pTexture )	return FALSE;

	RwTextureSetAddressing( pTexture , rwTEXTUREADDRESSCLAMP );

	switch( nType )
	{
	case 0:	if( nIndex < TB_IMAGE_COUNT )		m_ImgBalloon[nIndex] = pTexture;		break;
	case 1:	if( nIndex < TB_IMAGE_TAIL_COUNT )	m_ImgBalloonTail[nIndex] = pTexture;	break;
	case 2:										m_ImgDamage = pTexture;					break;
	case 3:	if( nIndex < IMOTICON_COUNT-530 )	m_ImgImoticon[nIndex] = pTexture;	    break;
	case 4:	if( nIndex < 3 )					m_ImgHPBar[nIndex] = pTexture;			break;
	case 5:	if( nIndex < TB_MAX_TAG )			m_ImgIDTag[nIndex] = pTexture;			break;
	case 6:	if( nIndex < TBID_TEXTURE_ADDINTIONAL_SIZE ) m_ImgIDETC[nIndex] = pTexture;	break;
	case 7:										m_ImgGOMark = pTexture;					break;
	case 8:	   m_vecTexCharisma.push_back( pTexture );									break;	//무조건 순차적이야해.. 
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::UpdateNickName( ApBase* pBase )
{
	
	AgpdCharacter*	    pcsCharacter = static_cast< AgpdCharacter * >(pBase);
	if (!pcsCharacter)
		return FALSE;

	sBoardDataPtr pMy   = GetMainCharacter();

	if( pMy && ( pMy->pBase == pBase ) )
	{
		pMy->pHPBar->SetNickNameText( pcsCharacter->m_szNickName );
		pMy->pHPBar->ReCalulateOffsetX();
		return TRUE;
	}

	BoardDataListIter  Iter = m_BoardDataList.begin();

	for( ; Iter != m_BoardDataList.end() ; ++Iter )
	{
		if( (*Iter)->pBase == pBase )
		{
			(*Iter)->pIDBoard->SetNickNameText( pcsCharacter->m_szNickName );
			(*Iter)->pIDBoard->ReCalulateOffsetX();
			return TRUE;
		}
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::ChangeGuildIDColor( AgpdCharacter* pdCharacter , const char* szOtherGuildID )
{
	AgpdCharacter* pcsCharacter   = pdCharacter;
	
	// pcsCharacter 가 자기가 아니면 안해두 된다.
	AgpdCharacter* pcsSelfCharacter = m_pcCmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter )					
		return FALSE;

	if( pcsSelfCharacter != pcsCharacter )	
		return TRUE;

	if( !strlen(szOtherGuildID) || strlen(szOtherGuildID) > AGPMGUILD_MAX_GUILD_ID_LENGTH)	
		return TRUE;

	CHAR* szSelfGuildID = m_pcsAgpmGuild->GetJoinedGuildID( pcsCharacter );

	BOOL bOtherCharacter = FALSE;
	BOOL bEnemy          = FALSE;
	BOOL bEnemyGuild     = FALSE;
	BOOL bJointGuild     = FALSE;

	DWORD dwColor = 0xFFFFFFFF;

	if( m_pcsAgpmPvP->IsEnemyGuild(pcsCharacter, (char*)szOtherGuildID) )
	{
		if( szSelfGuildID && szSelfGuildID[0] )
		{
			//AgcmGuild* pcsAgcmGuild = (AgcmGuild*)pThis->GetModule("AgcmGuild");
			//if( pcsAgcmGuild && pcsAgcmGuild->IsGuildBattle( pcsCharacter ) )
			{
				dwColor = 0xFF0000FF;	// 빨강
				bEnemy = TRUE;
				bEnemyGuild = TRUE;
			}
		}
	}
	else
	{
		if( szSelfGuildID && _tcslen(szSelfGuildID) > 0)
		{
			// 연대/적대 길드인지 확인해준다.
			if( m_pcsAgpmGuild->IsJointGuild(szSelfGuildID, (char*)szOtherGuildID))
			{
				dwColor = 0xFF009900;
				bJointGuild = TRUE;
			}
			else if( m_pcsAgpmGuild->IsHostileGuild(szSelfGuildID, (char*)szOtherGuildID))
			{
				dwColor = 0xFF0000FF;	// 빨강
				bEnemy = TRUE;
			}
		}
	}

	BOOL bIsSelfGuild = FALSE;
	if(m_pcsAgpmPvP->IsFriendGuild(pcsCharacter, (char*)szOtherGuildID))
	{
		// 같은편 임을 나타내는 색을 준다.
		dwColor = 0xFF2AE180;	// 녹색
	}
	else
	{
		// 같은편이 아닐 때는 자기 길드인지 한번 더 확인
		if( szSelfGuildID && strlen(szSelfGuildID) > 0 && !strcmp(szSelfGuildID, szOtherGuildID) )
		{
			dwColor = 0xFF2AE180;	// 녹색	By SungHoon
			bIsSelfGuild = TRUE;
		}
		else if( !bEnemy )
			bOtherCharacter = TRUE;
	}

	CHAR    szTmpGuildID[AGPMGUILD_MAX_GUILD_ID_LENGTH + 1 + 4];	// [ ] 를 추가하려고
	memset( szTmpGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1 + 4) );
	strcpy( szTmpGuildID, "[" );
	strcat( szTmpGuildID, szOtherGuildID );
	strcat( szTmpGuildID, "]" );
	
	BoardDataListIter  Iter     = m_BoardDataList.begin();
	AgcmIDBoard*       pIDBoard = NULL;

	INT					lIndex			=	0;
	AgpdCharacter*		ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence(&lIndex);

	//for( ; Iter != m_BoardDataList.end() ; ++Iter )
	for( ; ppdCharacter ; ppdCharacter = m_pcsAgpmCharacter->GetCharSequence(&lIndex) )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );

		if( !pcdCharacter || !pcdCharacter->m_pvTextboard )
			continue;

		pIDBoard   = ((sBoardDataPtr)pcdCharacter->m_pvTextboard)->pIDBoard;
		if( pIDBoard == NULL ) 
			continue;

		if( strlen( pIDBoard->GetGuildText() ) > 0 && !strcmp( pIDBoard->GetGuildText() , szTmpGuildID ) )
		{
			AgcmGuild* pcsAgcmGuild   = static_cast< AgcmGuild* >( GetModule("AgcmGuild") );
			if( bEnemyGuild && !pcsAgcmGuild->IsGuildBattleMember( pcsCharacter , (AgpdCharacter*)pIDBoard->GetApBase() ) )
			{
			}

			else
			{
				pIDBoard->SetColor( dwColor );
				pIDBoard->SetColorGuild( dwColor != 0xFFFFFFFF ? dwColor : 0xFF81FAB2 );

				if( pIDBoard->GetApBase() && pIDBoard->GetApBase()->m_eType == APBASE_TYPE_CHARACTER )
				{
					if( bIsSelfGuild )
						pIDBoard->SetColorGuild( GetGuildIDColor(szOtherGuildID , ((AgpdCharacter*)pIDBoard->GetApBase())->m_szID , pIDBoard->GetColorGuild() ) );

					if( bOtherCharacter && GetAgpmPvP()->IsNowPvP( (AgpdCharacter*)pIDBoard->GetApBase() ) )
						dwColor = 0xFFFFFFFF;

					AgcdCharacter* pcsAgcdCharacter = GetAgcmCharacter()->GetCharacterData((AgpdCharacter*)pIDBoard->GetApBase() );
					if( pcsAgcdCharacter )	pcsAgcdCharacter->m_dwCharIDColor = dwColor;
				}
			}
		}
	}

	return TRUE;
}

void AgcmTextBoardMng::SetHeight( ApBase* pBase )
{
	if( !pBase )	
		return;

	AgpdCharacter *pdAgpdCharacter = (AgpdCharacter *)pBase;
	AgcdCharacter *pdAgcdCharacter = m_pcCmCharacter->GetCharacterData( pdAgpdCharacter );

	FLOAT	fHeight	= pdAgcdCharacter->GetHeight();
	FLOAT	fDepth	= pdAgcdCharacter->GetDepth();

	sBoardDataPtr  pBoardData = GetBoard( pBase );
	if( !pBoardData )
		return;

	if( pBoardData->pIDBoard )
	{
		pBoardData->pIDBoard->SetHeight( fHeight );
		pBoardData->pIDBoard->SetDepth ( fDepth  );
	}
	
	else if( pBoardData->pHPBar )
	{
		pBoardData->pHPBar->SetHeight( fHeight	);
		pBoardData->pHPBar->SetDepth ( fDepth	);
	}

}

void AgcmTextBoardMng::UpdateIDBoardHPBar( AgcmHPBar* pHPBar )
{
	if( pHPBar == NULL ) return;

	pHPBar->SetDrawHPMP( FALSE );

	AgpdCharacter *	pdCharacter = (AgpdCharacter *) pHPBar->GetApBase();
	if (!pdCharacter)		return;

	pHPBar->SetDrawHPMP( TRUE );
	
	Trans3DTo2D( &pHPBar->GetCamPosHPMP() , &pHPBar->GetScrPosHPMP() , pHPBar->GetRecipzHPMPPtr() ,
				 (const RwV3d*)pHPBar->GetPosition() ,  0.f , pHPBar->GetClump(), 
				 pHPBar->GetDepth() );

	if ( pdCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD )
	{
		if( pHPBar->GetPerResultHP() != 0.0f )
		{
			pHPBar->SetDecreaseHP( TRUE );
			pHPBar->SetPerAfterImageHP( pHPBar->GetPerResultHP() );
			pHPBar->SetSpeedAfterImageHP( AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_SPEED );
			pHPBar->SetPerResultHP( 0.0f );	
		}
	}

	// HP
	if( pHPBar->GetDecreaseHP() )
	{
		pHPBar->SetPerAfterImageHP  ( pHPBar->GetPerAfterImageHP() - pHPBar->GetSpeedAfterImageHP() * m_TickDiff );
		pHPBar->SetSpeedAfterImageHP( pHPBar->GetSpeedAfterImageHP() * (1.0f + AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_ACCELERATION * m_TickDiff) );

		if( pHPBar->GetPerResultHP() >= pHPBar->GetPerAfterImageHP() )
			pHPBar->SetDecreaseHP( FALSE );
	}

	// MP
	if( pHPBar->GetDecreaseMP() )
	{
		pHPBar->SetPerAfterImageMP( pHPBar->GetPerAfterImageMP() - pHPBar->GetSpeedAfterImageMP() * m_TickDiff );
		pHPBar->SetSpeedAfterImageMP( pHPBar->GetSpeedAfterImageMP() * (1.0f + AGCM_TEXT_BOARD_AFTERIMAGE_DECREASE_ACCELERATION * m_TickDiff ) );

		if( pHPBar->GetPerResultMP() >= pHPBar->GetPerAfterImageMP() )
			pHPBar->SetDecreaseMP( FALSE );
	}
}

BOOL AgcmTextBoardMng::DisplayUpdateFactor(AgpdCharacter *pcsCharacter, AgpdFactor *pcsFactor)
{
	if( !m_pcsAgpmFactors )					return FALSE;
	if( !pcsCharacter || !pcsFactor )		return FALSE;

	INT32	nDamage			=	0;
	INT32	nTotalDamage	=	0;
	INT32	nColor			=	0;

	if( m_pMainCharacterID && m_pMainCharacterID->pBase == (ApBase*)pcsCharacter )
		nColor = 0xffbbb5f3;

	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL, nTotalDamage, nDamage )				)	nColor = 0xffffffff;

	nDamage		= 1;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC, nTotalDamage, nDamage )			)	nColor = 0xff9057b2;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_WATER, nTotalDamage, nDamage )			)	nColor = 0xff3286cd;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_FIRE, nTotalDamage, nDamage )			)	nColor = 0xffda4d0a;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_EARTH, nTotalDamage, nDamage )			)	nColor = 0xffa06636;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_AIR, nTotalDamage, nDamage )			)	nColor = 0xffa2d089;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_POISON, nTotalDamage, nDamage )		)	nColor = 0xff2dd23c;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_LIGHTENING, nTotalDamage, nDamage )	)	nColor = 0xffe2ba00;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_ICE, nTotalDamage, nDamage	)			)	nColor = 0xff89d0cd;
	if( GetFactorDamage( pcsFactor, AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC, nTotalDamage, nDamage)				)	nColor = 0xffffffff;
	
	sBoardAttr Attr;
	Attr.eType   = AGCM_BOARD_DAMAGE;
	Attr.Color   = nColor;
	Attr.nDamage  = nTotalDamage;

	CreateBoard( pcsCharacter , &Attr );

	return TRUE;
}

BOOL AgcmTextBoardMng::GetFactorDamage( AgpdFactor* pFactor, AgpdFactorCharPointType eFactorType, INT32& nTotalDamage, INT32& nPreDamage )
{
	INT32	nDamage = 0;
	m_pcsAgpmFactors->GetValue( pFactor, &nDamage, AGPD_FACTORS_TYPE_CHAR_POINT, eFactorType );
	if( nDamage < 0 )
	{
		nTotalDamage += ( nDamage *= -1);

		if( nDamage > nPreDamage )
		{
			nPreDamage = nDamage;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgcmTextBoardMng::DestroyMainBoard()
{	
	std::list< AgcmDmgBoard*      >::iterator     DmgListIter;
	std::list< AgcmImoticonBoard* >::iterator     EmoListIter;

	if( m_pMainCharacterID )
	{
		DestroyBoard( m_pMainCharacterID );
		m_pMainCharacterID		= NULL;
	}

	return TRUE;
}

BOOL AgcmTextBoardMng::SetMainCharacter( sBoardDataPtr pBoardData )
{
	
	DestroyMainBoard();

	m_pMainCharacterID				=  pBoardData;
	m_pMainCharacterID->pHPBar		=  new AgcmHPBar;
	m_pMainCharacterID->pHPBar->SetBoardType( AGCM_BOARD_HPMPBAR );
	m_pMainCharacterID->pHPBar->SetData( pBoardData->pIDBoard );

	SAFE_DELETE( pBoardData->pIDBoard );

	pBoardData->pIDBoard	= NULL;

	return TRUE;
}


BOOL AgcmTextBoardMng::DestroyBoard( ApBase* pPtr )
{
	std::list< AgcmDmgBoard*      >::iterator    DmgListIter;
	std::list< AgcmImoticonBoard* >::iterator	 EmoListIter;
	
	if( GetMainCharacter() && pPtr	== GetMainCharacter()->pBase )
	{
		m_pMainCharacterID	=	NULL;
	}

	if( !pPtr )
		return FALSE;

	sBoardDataPtr			pBoardData	=	NULL;

	if( pPtr->m_eType == APBASE_TYPE_CHARACTER )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( (AgpdCharacter*)pPtr );
		if( !pcdCharacter || !pcdCharacter->m_pvTextboard )
			return FALSE;

		pBoardData	=	static_cast< sBoardDataPtr >(pcdCharacter->m_pvTextboard);
	}

	else if( pPtr->m_eType == APBASE_TYPE_ITEM )
	{
		AgcdItem*		pcdItem			=	m_pcsAgcmItem->GetItemData( (AgpdItem*)pPtr );
		if( !pcdItem || !pcdItem->m_pvTextboard )
			return FALSE;

		pBoardData	=	static_cast< sBoardDataPtr >(pcdItem->m_pvTextboard);
	}

	if( !pBoardData )
		return FALSE;

	SAFE_DELETE( pBoardData->pHPBar );		
	SAFE_DELETE( pBoardData->pIDBoard );			
	SAFE_DELETE( pBoardData->pTextBoard );		

	DmgListIter   = pBoardData->pDmgBoard.begin();
	EmoListIter   = pBoardData->pImotioconBoard.begin();

	for( ; DmgListIter != pBoardData->pDmgBoard.end() ; ++DmgListIter )
	{
		SAFE_DELETE( (*DmgListIter) );
		(*DmgListIter)	= NULL;
	}

	for( ; EmoListIter != pBoardData->pImotioconBoard.end() ; ++EmoListIter )
	{
		SAFE_DELETE( (*EmoListIter) );
		(*EmoListIter)	= NULL;
	}

	SAFE_DELETE( pBoardData );
	if( pPtr->m_eType == APBASE_TYPE_CHARACTER )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( (AgpdCharacter*)pPtr );
		if( !pcdCharacter || !pcdCharacter->m_pvTextboard )
			return FALSE;

		pcdCharacter->m_pvTextboard	=	NULL;
	}

	else if( pPtr->m_eType == APBASE_TYPE_ITEM )
	{
		AgcdItem*		pcdItem			=	m_pcsAgcmItem->GetItemData( (AgpdItem*)pPtr );
		if( !pcdItem || !pcdItem->m_pvTextboard )
			return FALSE;

		pcdItem->m_pvTextboard	=	NULL;
	}



	return TRUE;
}

AgcmBaseBoard* AgcmTextBoardMng::DestroyBoard( AgcmBaseBoard* pTextBoard )
{
	return pTextBoard;
}

BOOL AgcmTextBoardMng::DestroyBoard( sBoardDataPtr  pBoardData )
{
	return DestroyBoard( pBoardData->pBase );
}

BOOL AgcmTextBoardMng::StreamReadTemplate( CHAR *szFile, BOOL bDecryption )
{
	if( !szFile )		return FALSE;

	ApModuleStream	csStream;
	const CHAR		*szValueName = NULL;
	CHAR			szValue[256];
	CHAR			szFileName[256];
	INT32			lIndex;

	csStream.SetMode( APMODULE_STREAM_MODE_NAME_OVERWRITE );

	VERIFY(csStream.Open(szFile, 0, bDecryption) && "TextBoard INI파일을 읽지 못했습니다.");

	for( int i=0; i<9; ++i )
	{
		if( !csStream.ReadSectionName( i ) )
			continue;

		csStream.SetValueID( -1 );
		while( csStream.ReadNextValue() )
		{
			szValueName = csStream.GetValueName();

			if( !strcmp( szValueName, TB_INI_NAME_FILE_NAME ) )
			{
				csStream.GetValue( szValue, 256 );
				sscanf( szValue, "%d:%s", &lIndex, szFileName );
				SetTexture( i, lIndex, RwTextureRead( szFileName, NULL ) );
			}
		}
	}

	return TRUE;
}

void AgcmTextBoardMng::SetAllIDEnable( TEXTBOARD_IDTYPE eIDType , BOOL bVal )
{

	INT32			lIndex				=	0;
	AgpdCharacter*	ppdCharacter		=	m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	for( ; ppdCharacter ; ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence(&lIndex) )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
		if( pcdCharacter && pcdCharacter->m_pvTextboard )
		{
			sBoardDataPtr		pBoardData	=	static_cast< sBoardDataPtr >(pcdCharacter->m_pvTextboard );
			if( pBoardData && pBoardData->pIDBoard && (eIDType & pBoardData->pIDBoard->GetIDType()) )
			{
				pBoardData->pIDBoard->SetEnabled( bVal );
			}
		}
	}

	if( m_pMainCharacterID )
		m_pMainCharacterID->pHPBar->SetEnabled( bVal );

}

void AgcmTextBoardMng::SetAllGuildEnable( BOOL bVal )
{
	INT32			lIndex				=	0;
	AgpdCharacter*	ppdCharacter		=	m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	for( ; ppdCharacter ; ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence(&lIndex) )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
		if( pcdCharacter && pcdCharacter->m_pvTextboard )
		{
			sBoardDataPtr		pBoardData	=	static_cast< sBoardDataPtr >(pcdCharacter->m_pvTextboard );
			if( pBoardData && pBoardData->pIDBoard )
			{
				pBoardData->pIDBoard->SetEnableGuild( (INT)bVal );
			}
		}
	}

	if( m_pMainCharacterID )
		m_pMainCharacterID->pHPBar->SetEnableGuild( (INT)bVal );
}

void AgcmTextBoardMng::SetAllPartyEnable( BOOL bVal )
{
	INT32			lIndex				=	0;
	AgpdCharacter*	ppdCharacter		=	m_pcsAgpmCharacter->GetCharSequence(&lIndex);
	for( ; ppdCharacter ; ppdCharacter	=	m_pcsAgpmCharacter->GetCharSequence(&lIndex) )
	{
		AgcdCharacter*	pcdCharacter	=	m_pcsAgcmCharacter->GetCharacterData( ppdCharacter );
		if( pcdCharacter && pcdCharacter->m_pvTextboard )
		{
			sBoardDataPtr		pBoardData	=	static_cast< sBoardDataPtr >(pcdCharacter->m_pvTextboard );
			if( pBoardData && pBoardData->pIDBoard )
			{
				pBoardData->pIDBoard->SetEnableParty( (INT)bVal );
			}
		}
	}

	if( m_pMainCharacterID )
		m_pMainCharacterID->pHPBar->SetEnableParty( (INT)bVal );

}

BOOL AgcmTextBoardMng::PvPRefreshSiege( CHAR *pszGuildName )
{
	if (!pszGuildName || !pszGuildName[0])
		return FALSE;

	CB_PvPRefreshGuild(m_pcCmCharacter->GetSelfCharacter(), this, pszGuildName);

	return TRUE;
}

void AgcmTextBoardMng::SetWorldCamera( RwCamera* pCamera )
{ 
	m_pCamera = pCamera; 
	AgcmBaseBoard::SetCamera( pCamera ); 
}

void AgcmTextBoardMng::Trans3DTo2D( RwV3d* pCameraPos , RwV3d* pScreenPos, FLOAT* pRecip, const RwV3d* pWorldPos, const FLOAT fHeight /* = 0.f */, const RpClump* pClump /* = NULL */, const FLOAT fDepth /* = 0.f */ )
{	
	RwV3d wNewPos = *pWorldPos;
	wNewPos.y += fHeight;

	if( pClump )		//Clump가 존재할때만.. ZDepth를 이용
	{
		RwV3d* pClampAt = RwMatrixGetAt( RwFrameGetLTM( RpClumpGetFrame( pClump ) ) );
		wNewPos.x += ( pClampAt->x * fDepth );
		wNewPos.z += ( pClampAt->z * fDepth );
	}

	GetWorldPosToScreenPos( &wNewPos, pCameraPos, pScreenPos, pRecip );
}

FLOAT AgcmTextBoardMng::GetWorldPosToScreenPos(RwV3d* pWorldPos , RwV3d* pCameraPos, RwV3d* pScreenPos , FLOAT* recip_z)
{
	RwMatrix *viewMatrix = RwCameraGetViewMatrix(m_pCamera);
	RwV3dTransformPoint(pCameraPos, pWorldPos, viewMatrix);

	float recipZ = 1.0f/pCameraPos->z;

	pScreenPos->x = pCameraPos->x * recipZ * RwRasterGetWidth ( RwCameraGetRaster( m_pCamera )) + m_pCamera->viewOffset.x;
	pScreenPos->y = pCameraPos->y * recipZ * RwRasterGetHeight( RwCameraGetRaster( m_pCamera )) + m_pCamera->viewOffset.y;
	pScreenPos->z = m_pCamera->zScale * recipZ + m_pCamera->zShift;

	*recip_z = recipZ;

	return 1.0f;		// 0보다 작으면 그리지 말라!! 카메라 뒤
}	

RwTexture* AgcmTextBoardMng::GetCastleTexture( UINT64 uiState )
{
	if( uiState & AGPDCHAR_SPECIAL_STATUS_HUMAN_CASTLE_OWNER )
		return m_pcsAgcmSiegeWar->GetCastleOwnerTextureTexture( AGPMSIEGEWAR_HUMAN_CASTLE_INDEX );
	else if( uiState & AGPDCHAR_SPECIAL_STATUS_ORC_CASTLE_OWNER )
		return m_pcsAgcmSiegeWar->GetCastleOwnerTextureTexture( AGPMSIEGEWAR_ORC_CASTLE_INDEX );
	else if( uiState & AGPDCHAR_SPECIAL_STATUS_MOONELF_CASTLE_OWNER)
		return m_pcsAgcmSiegeWar->GetCastleOwnerTextureTexture( AGPMSIEGEWAR_MOONELF_CASTLE_INDEX );
	else if( uiState & AGPDCHAR_SPECIAL_STATUS_DRAGONSCION_CASTLE_OWNER)
		return m_pcsAgcmSiegeWar->GetCastleOwnerTextureTexture( AGPMSIEGEWAR_DRAGONSCION_CASTLE_INDEX );
	else if( uiState & AGPDCHAR_SPECIAL_STATUS_ARCHLORD_CASTLE_OWNER)
		return m_pcsAgcmSiegeWar->GetCastleOwnerTextureTexture( AGPMSIEGEWAR_ARCHLORD_CASTLE_INDEX );
	else
		return NULL;
}

RwTexture* AgcmTextBoardMng::GetStateTexture( AgpdCharacter* pdCharacter )
{
	if( !pdCharacter )		return NULL;

	AgpdSiegeWarMonsterType	eMonsterType = m_pcsAgpmSiegeWar->GetSiegeWarMonsterType( pdCharacter );
	if( AGPD_SIEGE_MONSTER_CATAPULT  == eMonsterType || AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER == eMonsterType  )
	{
		// 사용중인지, 수리중인지 그려준다.
		if (eMonsterType == AGPD_SIEGE_MONSTER_CATAPULT)
		{
			if( m_pcsAgpmCharacter->IsUseCharacter( pdCharacter ) )
				return m_pcsAgcmSiegeWar->GetCatapultUseTexture();
			else if( m_pcsAgpmCharacter->IsRepairCharacter( pdCharacter ) )
				return m_pcsAgcmSiegeWar->GetCatapultRepairTexture();
		}
		else if( eMonsterType == AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER )
		{
			if ( m_pcsAgpmCharacter->IsUseCharacter( pdCharacter ) )
				return m_pcsAgcmSiegeWar->GetAtkResTowerUseTexture();
			else if( m_pcsAgpmCharacter->IsRepairCharacter( pdCharacter ) )
				return m_pcsAgcmSiegeWar->GetAtkResTowerRepairTexture();
		}
	}

	return NULL;
}

void AgcmTextBoardMng::RenderTexture( RwTexture* pTexture, RwV3d& vCamera, RwV3d& vScreen, float fRecipz /* = 1.f */, RwV2d* pOffset /* = NULL */ )
{
	if( !pTexture )		return;

	RwV2d vImageSize	= GetTextureSize( pTexture );
	g_pEngine->DrawIm2DInWorld( pTexture, &vCamera, &vScreen, fRecipz, pOffset, vImageSize.x, vImageSize.y );
}

VOID AgcmTextBoardMng::RenderText( char* szText, RwV3d& vPos, RwFrame* pFrame, float fDepth )
{
	if( !szText )	return;

	RwV3d vNewPos = vPos;
	if( pFrame )		//Clump가 존재할때만.. ZDepth를 이용
	{
		RwV3d* pAt = RwMatrixGetAt( RwFrameGetLTM( pFrame ) );
		vNewPos.x += ( pAt->x * fDepth );
		vNewPos.z += ( pAt->z * fDepth );
	}

	RwV3d vCameraPos, vScreenPos;
	FLOAT fRecip;

	GetWorldPosToScreenPos( &vNewPos, &vCameraPos, &vScreenPos, &fRecip );

	m_pcFont->DrawTextIMMode2DWorld( vScreenPos.x, vScreenPos.y, vScreenPos.z, fRecip, szText, 0, 255, 0xffffffff );
}

BOOL	AgcmTextBoardMng::CheckPeculiarity( AgpdCharacter* pCharacter , OUT INT& nResult )
{
	if( !pCharacter )
		return FALSE;

	nResult	=	GetApmMap()->CheckRegionPerculiarity( pCharacter->m_nBindingRegionIndex , APMMAP_PECULIARITY_SHOWNAME );

	return TRUE;
}

BOOL AgcmTextBoardMng::IsRender( AgpdCharacter* pdCharacter )
{
	if( !pdCharacter )		return FALSE;

	if( APBASE_TYPE_CHARACTER == pdCharacter->m_eType )
	{
		if( GetAgpmCharacter()->IsStatusTransparent( pdCharacter ) )
		{
			AgcdCharacter* pcsAgcdCharacter = GetAgcmCharacter()->GetCharacterData( pdCharacter );
			if( (INT8)AGCMCHAR_TRANSPARENT_FULL == pcsAgcdCharacter->m_cTransparentType )
				return FALSE;
		}

		AgpdSiegeWarMonsterType	eMonsterType	= GetAgpmSiegeWar()->GetSiegeWarMonsterType( pdCharacter );
		if (eMonsterType > AGPD_SIEGE_MONSTER_TYPE_NONE && 
			eMonsterType < AGPD_SIEGE_MONSTER_TYPE_MAX &&
			eMonsterType != AGPD_SIEGE_MONSTER_CATAPULT && 
			eMonsterType != AGPD_SIEGE_MONSTER_ATTACK_RESURRECTION_TOWER &&
			eMonsterType != AGPD_SIEGE_MONSTER_THRONE )
			return FALSE;
	}

	return TRUE;
}
