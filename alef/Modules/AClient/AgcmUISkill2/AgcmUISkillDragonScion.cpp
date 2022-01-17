#include "AgcmUISkillDragonScion.h"
#include "AgcmUISkillDragonScion.inl"

#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgpmEventSKillMaster.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgcmSkill.h"
#include "AgcmEventSkillMaster.h"
#include "AgcmUICharacter.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AgpmSkill.h"
#include "AgcmUISkill2.h"
#include "AgcmHeroicSkill.h"

AgcmUISkillDragonScion::AgcmUISkillDragonScion( VOID )
{
	m_eCurrentUI					= SkillUI_NotOpened;
	m_eCurrentUpUI					= SkillUI_NotOpened;

	m_eLastUI						= SkillUI_NotOpened;
	m_eLastUpUI						= SkillUI_NotOpened;

	m_nEventOpenUIScion				= -1;
	m_nEventOpenUISlayer			= -1;
	m_nEventOpenUIObiter			= -1;
	m_nEventOpenUISummerner			= -1;
	m_nEventOpenUIPassive			= -1;
	m_nEventOpenUIArchlord			= -1;

	m_nEventCloseUIScion			= -1;
	m_nEventCloseUISlayer			= -1;
	m_nEventCloseUIObiter			= -1;
	m_nEventCloseUISummerner		= -1;
	m_nEventCloseUIPassive			= -1;
	m_nEventCloseUIArchlord			= -1;

	m_nScionSkillPoint				= 0;

	m_pcsEvent						=	NULL;
	
	m_pcsAgcmCharacter				=	NULL;
	m_pcsAgcmUIManager2				=	NULL;
	m_pcsAgpmGrid					=	NULL;
	m_pcsAgpmSkill					=	NULL;

	ZeroMemory( m_nArrSelectIndex			, sizeof(m_nArrSelectIndex) );
	ZeroMemory( m_nArrUpgradeSelectIndex	, sizeof(m_nArrUpgradeSelectIndex) );
}

AgcmUISkillDragonScion::~AgcmUISkillDragonScion( void )
{
}

BOOL AgcmUISkillDragonScion::OnAddModule( AgcmUISkill2*	pAgcmUISkill )
{
	m_pcsAgpmGrid				= (AgpmGrid *)				pAgcmUISkill->GetModule("AgpmGrid");
	m_pcsAgpmSkill				= (AgpmSkill *)				pAgcmUISkill->GetModule("AgpmSkill");
	m_pcsAgcmCharacter			= (AgcmCharacter *)			pAgcmUISkill->GetModule("AgcmCharacter");
	m_pcsAgcmUIManager2			= (AgcmUIManager2 *)		pAgcmUISkill->GetModule("AgcmUIManager2");
	m_pcsAgpmEventSkillMaster	= (AgpmEventSkillMaster *)	pAgcmUISkill->GetModule("AgpmEventSkillMaster");
	m_pcsAgpmFactors			= (AgpmFactors *)			pAgcmUISkill->GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter *)			pAgcmUISkill->GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)				pAgcmUISkill->GetModule("AgpmItem");
	m_pcsAgcmSkill				= (AgcmSkill *)				pAgcmUISkill->GetModule("AgcmSkill");
	m_pcsAgcmEventSkillMaster	= (AgcmEventSkillMaster *)	pAgcmUISkill->GetModule("AgcmEventSkillMaster");
	m_pcsAgcmUICharacter		= (AgcmUICharacter *)		pAgcmUISkill->GetModule("AgcmUICharacter");
	m_pcsAgcmUIMain				= (AgcmUIMain *)			pAgcmUISkill->GetModule("AgcmUIMain");
	m_pcsAgcmUIItem				= (AgcmUIItem *)			pAgcmUISkill->GetModule("AgcmUIItem");

	m_pAgcmUISkill2				=	pAgcmUISkill;
	m_pcsEvent					=	m_pAgcmUISkill2->GetApdEvent();

	if (!m_pcsAgpmEventSkillMaster->SetCallbackLearnResult(CBBuyScionSkillResult, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackResponseUpgrade(CBUpgradeScionSkillResult, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackInitSkill( CBUpdateScionSkill , this ))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackUpdateSkillPoint( CBUpdateScionSkill , this))
		return FALSE;

	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorHp( CBUpdateScionSkill, this))
		return FALSE;
	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorMp( CBUpdateScionSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRemoveSkill( CBUpdateScionSkill, this) )
		return FALSE;

	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorSkillPoint( CBUpdateScionSkill , this) )
		return FALSE;

	return TRUE;
}

BOOL AgcmUISkillDragonScion::OnInit( VOID )
{

	for( INT i = 0 ; i < SKILL_SCION_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Init( &m_ScionSkillGrid[i]	, 1 , 1 , 1 );
		m_pcsAgpmGrid->Init( &m_ScionSkillUpGrid[i]	, 1 , 1 , 1 );

		m_ScionSkillGrid[i].m_lGridType		=	AGPDGRID_ITEM_TYPE_SKILL;
		m_ScionSkillUpGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
	}

	for( INT i = 0 ; i < SKILL_SLAYER_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Init( &m_SlayerSkillGrid[i]	, 1 , 1 , 1 );
		m_pcsAgpmGrid->Init( &m_SlayerSkillUpGrid[i]	, 1 , 1 , 1 );

		m_SlayerSkillGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
		m_SlayerSkillUpGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
	}

	for( INT i = 0 ; i < SKILL_SUMMENER_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Init( &m_SummernerSkillGrid[i]	, 1 , 1 , 1 );
		m_pcsAgpmGrid->Init( &m_SummernerSkillUpGrid[i]	, 1 , 1 , 1 );

		m_SummernerSkillGrid[i].m_lGridType		=	AGPDGRID_ITEM_TYPE_SKILL;
		m_SummernerSkillUpGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
	}

	for( INT i = 0 ; i < SKILL_OBITER_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Init( &m_ObiterSkillGrid[i]	, 1 , 1 , 1 );
		m_pcsAgpmGrid->Init( &m_ObiterSkillUpGrid[i] , 1 , 1 , 1 );

		m_ObiterSkillGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
		m_ObiterSkillUpGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
	}

	for( INT i = 0 ; i < SKILL_PASSIVE_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Init( &m_PassiveSkillGrid[i] , 1 , 1 , 1 );
		m_pcsAgpmGrid->Init( &m_PassiveSkillUpGrid[i] , 1 , 1 , 1 );

		m_PassiveSkillGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
		m_PassiveSkillUpGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
	}

	for( INT i = 0 ; i < DRAGONSCION_ARCHLORD_SKILL_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Init( &m_ArchlordSkillGrid[i] , 1 , 1 , 1 );

		m_ArchlordSkillGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
	}

	return TRUE;
}

BOOL AgcmUISkillDragonScion::OnDestroy( VOID )
{
	CloseSkillUI();
	CloseSkillUpUI();

	for( INT i = 0 ; i < SKILL_SCION_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Remove( &m_ScionSkillGrid[i]		);
		m_pcsAgpmGrid->Remove( &m_ScionSkillUpGrid[i]	);
	}

	for( INT i = 0 ; i < SKILL_SLAYER_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Remove( &m_SlayerSkillGrid[i]	);
		m_pcsAgpmGrid->Remove( &m_SlayerSkillUpGrid[i]	);
	}

	for( INT i = 0 ; i < SKILL_SUMMENER_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Remove( &m_SummernerSkillGrid[i]	);
		m_pcsAgpmGrid->Remove( &m_SummernerSkillUpGrid[i]);
	}

	for( INT i = 0 ; i < SKILL_OBITER_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Remove( &m_ObiterSkillGrid[i]	);
		m_pcsAgpmGrid->Remove( &m_ObiterSkillUpGrid[i]	);
	}

	for( INT i = 0 ; i < SKILL_PASSIVE_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Remove( &m_PassiveSkillGrid[i]	);
		m_pcsAgpmGrid->Remove( &m_PassiveSkillUpGrid[i]	);
	}

	return TRUE;
}


void AgcmUISkillDragonScion::OnAddEvent( void* pUIManager )
{

	while( 1 )
	{
		AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
		if( !pcmUIManager ) break;

		m_nEventOpenUIScion = pcmUIManager->AddEvent( "Skill_OpenUIScion" );
		if( m_nEventOpenUIScion < 0 )			break;
		m_nEventOpenUISlayer = pcmUIManager->AddEvent( "Skill_OpenUISlayer" );
		if( m_nEventOpenUISlayer < 0 )			break;
		m_nEventOpenUIObiter = pcmUIManager->AddEvent( "Skill_OpenUIObiter" );
		if( m_nEventOpenUIObiter < 0 )			break;
		m_nEventOpenUISummerner = pcmUIManager->AddEvent( "Skill_OpenUISummerner" );
		if( m_nEventOpenUISummerner < 0 )		break;
		m_nEventOpenUIPassive = pcmUIManager->AddEvent( "Skill_OpenUIPassive" );
		if( m_nEventOpenUIPassive < 0 )			break;
		m_nEventOpenUIArchlord = pcmUIManager->AddEvent( "Skill_OpenUIArchlord" );
		if( m_nEventOpenUIArchlord < 0 )		break;

		m_nEventCloseUIScion = pcmUIManager->AddEvent( "Skill_CloseUIScion" );
		if( m_nEventCloseUIScion < 0 )			break;
		m_nEventCloseUISlayer = pcmUIManager->AddEvent( "Skill_CloseUISlayer" );
		if( m_nEventCloseUISlayer < 0 )			break;
		m_nEventCloseUIObiter = pcmUIManager->AddEvent( "Skill_CloseUIObiter" );
		if( m_nEventCloseUIObiter < 0 )			break;
		m_nEventCloseUISummerner = pcmUIManager->AddEvent( "Skill_CloseUISummerner" );
		if( m_nEventCloseUISummerner < 0 )		break;
		m_nEventCloseUIPassive = pcmUIManager->AddEvent( "Skill_CloseUIPassive" );
		if( m_nEventCloseUIPassive < 0 )		break;
		m_nEventCloseUIArchlord = pcmUIManager->AddEvent( "Skill_CloseUIArchlord" );
		if( m_nEventCloseUIArchlord < 0 )		break;

		if( 0 > (m_nEventOpenUpUIScion[ EVENT_SCION			] = pcmUIManager->AddEvent( "Skill_OpenUpUIScion"	 ) ) )	break;
		if( 0 > (m_nEventOpenUpUIScion[ EVENT_SLAYER		] = pcmUIManager->AddEvent( "SKill_OpenUpUISlayer"	 ) ) )	break;
		if( 0 > (m_nEventOpenUpUIScion[ EVENT_OBITER		] = pcmUIManager->AddEvent( "Skill_OpenUpUIObiter"	 ) ) )	break;
		if( 0 > (m_nEventOpenUpUIScion[ EVENT_SUMMERNER		] = pcmUIManager->AddEvent( "Skill_OpenUpUISummerner") ) )	break;
		if( 0 > (m_nEventOpenUpUIScion[ EVENT_PASSIVE		] = pcmUIManager->AddEvent( "Skill_OpenUpUIPassive"	 ) ) )	break;
		if( 0 > (m_nEventOpenUpUIScion[ EVENT_ARCHLORD		] = pcmUIManager->AddEvent( "Skill_OpenUpUIArchlord" ) ) )	break;

		if( 0 > (m_nEventCloseUpUIScion[ EVENT_SCION		] = pcmUIManager->AddEvent( "Skill_CloseUpUIScion"		) ) )	break;
		if( 0 > (m_nEventCloseUpUIScion[ EVENT_SLAYER		] = pcmUIManager->AddEvent( "Skill_CloseUpUISlayer"		) ) )	break;
		if( 0 > (m_nEventCloseUpUIScion[ EVENT_OBITER		] = pcmUIManager->AddEvent( "SKill_CloseUpUIObiter"		) )	)	break;
		if( 0 > (m_nEventCloseUpUIScion[ EVENT_SUMMERNER	] = pcmUIManager->AddEvent( "Skill_CloseUpUISummerner"	) ) )	break;
		if( 0 > (m_nEventCloseUpUIScion[ EVENT_PASSIVE		] = pcmUIManager->AddEvent( "Skill_CloseUpUIPassive"	) ) )	break;
		if( 0 > (m_nEventCloseUpUIScion[ EVENT_ARCHLORD		] = pcmUIManager->AddEvent( "Skill_CloseUpUIArchlord"	) )	)	break;


		CHAR		szEventName[ 128 ];
		ZeroMemory( szEventName , 128 );

		// 시온 이벤트 등록
		for( INT i = 0 ; i < SKILL_SCION_COUNT ; ++i )
		{
			sprintf_s( szEventName , 128 , "Skill_Scion_SelectedItem_%d" , i );
			m_nEventScionSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionSelectedItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Scion_UnSelectedItem_%d" , i );
			m_nEventScionUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionUnSelectedItem[i] )			break;

			sprintf_s( szEventName , 128 , "Skill_Scion_UpgradeSelectedItem_%d" , i );
			m_nEventScionUpgradeSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionUpgradeSelectedItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Scion_UpgradeUnSelectedItem_%d" , i );
			m_nEventScionUpgradeUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionUpgradeUnSelectedItem[i] )	break;

			sprintf_s( szEventName , 128 , "Skill_Scion_ActiveItem_%d" , i );
			m_nEventScionActiveItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionActiveItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Scion_DisableItem_%d" , i );
			m_nEventScionDisableItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionDisableItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Scion_UpgradeActiveItem_%d" , i );
			m_nEventScionUpgradeActiveItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionUpgradeActiveItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Scion_UpgradeDisableItem_%d" , i );
			m_nEventScionUpgradeDisableItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionUpgradeDisableItem[i] )		break;
		}

		// 슬레이어 이벤트 등록
		for( INT i = 0 ; i < SKILL_SLAYER_COUNT ; ++i )
		{
			sprintf_s( szEventName , 128 , "Skill_Slayer_SelectedItem_%d" , i );
			m_nEventSlayerSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventScionSelectedItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Slayer_UnSelectedItem_%d" , i );
			m_nEventSlayerUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSlayerUnSelectedItem[i] )			break;

			sprintf_s( szEventName , 128 , "Skill_Slayer_UpgradeSelectedItem_%d" , i );
			m_nEventSlayerUpgradeSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSlayerUpgradeSelectedItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Slayer_UpgradeUnSelectedItem_%d" , i );
			m_nEventSlayerUpgradeUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSlayerUpgradeUnSelectedItem[i] )	break;

			sprintf_s( szEventName , 128 , "Skill_Slayer_ActiveItem_%d" , i );
			m_nEventSlayerActiveItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSlayerActiveItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Slayer_DisableItem_%d" , i );
			m_nEventSlayerDisableItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSlayerDisableItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Slayer_UpgradeActiveItem_%d" , i );
			m_nEventSlayerUpgradeActiveItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSlayerUpgradeActiveItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Slayer_UpgradeDisableItem_%d" , i );
			m_nEventSlayerUpgradeDisableItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSlayerUpgradeDisableItem[i] )		break;
		}

		// 오비터 이벤트 등록
		for( INT i = 0 ; i < SKILL_OBITER_COUNT ; ++i )
		{
			sprintf_s( szEventName , 128 , "Skill_Obiter_SelectedItem_%d" , i );
			m_nEventObiterSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventObiterSelectedItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Obiter_UnSelectedItem_%d" , i );
			m_nEventObiterUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventObiterUnSelectedItem[i] )			break;

			sprintf_s( szEventName , 128 , "Skill_Obiter_UpgradeSelectedItem_%d" , i );
			m_nEventObiterUpgradeSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventObiterUpgradeSelectedItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Obiter_UpgradeUnSelectedItem_%d" , i );
			m_nEventObiterUpgradeUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventObiterUpgradeUnSelectedItem[i] )	break;

			sprintf_s( szEventName , 128 , "Skill_Obiter_ActiveItem_%d" , i );
			m_nEventObiterActiveItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventObiterActiveItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Obiter_DisableItem_%d" , i );
			m_nEventObiterDisableItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventObiterDisableItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Obiter_UpgradeActiveItem_%d" , i );
			m_nEventObiterUpgradeActiveItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventObiterUpgradeActiveItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Obiter_UpgradeDisableItem_%d" , i );
			m_nEventObiterUpgradeDisableItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventObiterUpgradeDisableItem[i] )		break;	
		}

		// 패시브 이벤트 등록
		for( INT i = 0 ; i < SKILL_PASSIVE_COUNT ; ++i )
		{
			sprintf_s( szEventName , 128 , "Skill_Passive_SelectedItem_%d" , i );
			m_nEventPassiveSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventPassiveSelectedItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Passive_UnSelectedItem_%d" , i );
			m_nEventPassiveUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventPassiveUnSelectedItem[i] )			break;

			sprintf_s( szEventName , 128 , "Skill_Passive_UpgradeSelectedItem_%d" , i );
			m_nEventPassiveUpgradeSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventPassiveUpgradeSelectedItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Passive_UpgradeUnSelectedItem_%d" , i );
			m_nEventPassiveUpgradeUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventPassiveUpgradeUnSelectedItem[i] )	break;

			sprintf_s( szEventName , 128 , "Skill_Passive_ActiveItem_%d" , i );
			m_nEventPassiveActiveItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventPassiveActiveItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Passive_DisableItem_%d" , i );
			m_nEventPassiveDisableItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventPassiveDisableItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Passive_UpgradeActiveItem_%d" , i );
			m_nEventPassiveUpgradeActiveItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventPassiveUpgradeActiveItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Passive_UpgradeDisableItem_%d" , i );
			m_nEventPassiveUpgradeDisableItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventPassiveUpgradeDisableItem[i] )		break;	
		}

		// 서머너 이벤트 등록
		for( INT i = 0 ; i < SKILL_SUMMENER_COUNT ; ++i )
		{	
			sprintf_s( szEventName , 128 , "Skill_Summerner_SelectedItem_%d" , i );
			m_nEventSummernerSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSummernerSelectedItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Summerner_UnSelectedItem_%d" , i );
			m_nEventSummernerUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSummernerUnSelectedItem[i] )			break;

			sprintf_s( szEventName , 128 , "Skill_Summerner_UpgradeSelectedItem_%d" , i );
			m_nEventSummernerUpgradeSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSummernerUpgradeSelectedItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Summerner_UpgradeUnSelectedItem_%d" , i );
			m_nEventSummernerUpgradeUnSelectedItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSummernerUpgradeUnSelectedItem[i] )	break;

			sprintf_s( szEventName , 128 , "Skill_Summerner_ActiveItem_%d" , i );
			m_nEventSummernerActiveItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSummernerActiveItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Summerner_DisableItem_%d" , i );
			m_nEventSummernerDisableItem[i]		=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSummernerDisableItem[i] )				break;

			sprintf_s( szEventName , 128 , "Skill_Summerner_UpgradeActiveItem_%d" , i );
			m_nEventSummernerUpgradeActiveItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSummernerUpgradeActiveItem[i] )		break;

			sprintf_s( szEventName , 128 , "Skill_Summerner_UpgradeDisableItem_%d" , i );
			m_nEventSummernerUpgradeDisableItem[i]	=	pcmUIManager->AddEvent( szEventName );
			if( !m_nEventSummernerUpgradeDisableItem[i] )		break;	
		}

		sprintf_s( szEventName , 128 , "SKill_Archlord_SelectedItem" );
		m_nArchlordSkillSelect		=	pcmUIManager->AddEvent( szEventName );
		if( !m_nArchlordSkillSelect )		
			break;

		sprintf_s( szEventName , 128 , "Skill_Archlord_UnSelectedItem" );
		m_nArchlordSkillUnSelect	=	pcmUIManager->AddEvent( szEventName );
		if( !m_nArchlordSkillUnSelect )
			break;

		sprintf_s( szEventName , 128 , "Skill_Archlord_ActiveItem" );
		m_nArchlordSkillActive		=	pcmUIManager->AddEvent( szEventName );
		if( !m_nArchlordSkillActive )
			break;

		sprintf_s( szEventName , 128 , "Skill_Archlord_DisableItem" );
		m_nArchlordSkillDisable		=	pcmUIManager->AddEvent( szEventName );
		if( !m_nArchlordSkillDisable )
			break;

		// 이쪽을 오면 초기화 성공
		return;
	}
	
	MessageBox( NULL , _TEXT( "AgcmUISkillDragonScion::OnAddEvent 에서 실패") , _TEXT( "Error" ) , MB_OK );
}

void AgcmUISkillDragonScion::OnAddFunction( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	pcmUIManager->AddFunction( this, "Skill_UIScion_GetBaseScion"		,	CallBackGetBaseControlScion, 0		);
	pcmUIManager->AddFunction( this, "Skill_UIScion_GetBaseSlayer"		,	CallBackGetBaseControlSlayer, 0		);
	pcmUIManager->AddFunction( this, "Skill_UIScion_GetBaseObiter"		,	CallBackGetBaseControlObiter, 0		);
	pcmUIManager->AddFunction( this, "Skill_UIScion_GetBaseSummerner"	,	CallBackGetBaseControlSummerner, 0	);
	pcmUIManager->AddFunction( this, "Skill_UIScion_GetBasePassive"		,	CallBackGetBaseControlPassive, 0	);
	pcmUIManager->AddFunction( this, "Skill_UIScion_GetBaseHeroic"		,	CallBackGetBaseControlHeroic, 0	);
	pcmUIManager->AddFunction( this, "Skill_UIScion_Archlord"			,	CallBackArchlord, 0					);

	pcmUIManager->AddFunction( this, "Skill_UIScion_Upgrade",		CBOpenScionUpgrade, 0 );
	pcmUIManager->AddFunction( this, "Skill_UISlayer_Upgrade",		CBOpenSlayerUpgrade, 0 );
	pcmUIManager->AddFunction( this, "Skill_UIObiter_Upgrade",		CBOpenObiterUpgrade, 0 );
	pcmUIManager->AddFunction( this, "Skill_UIPassive_Upgrade",		CBOpenPassiveUpgrade, 0 );
	pcmUIManager->AddFunction( this, "Skill_UISummerner_Upgrade",	CBOpenSummernerUpgrade, 0 );
	pcmUIManager->AddFunction( this, "Skill_UIHeroic_Upgrade"	,	CBOpenHeroicUpgrade, 0 );

	pcmUIManager->AddFunction( this, "Skill_UIScion_BtnScroll",			CallBackUIScroll, 0 );

	pcmUIManager->AddFunction( this , "SKill_UIScion_Close" ,			CBCloseScionSkillUI		, 0 );
	pcmUIManager->AddFunction( this , "Skill_UIScionUP_Close" ,			CBCloseScionUPSkillUI	, 0 );

	pcmUIManager->AddFunction( this , "Skill_SelectScionGrid"			, CBSelectScionSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectBuyScionGrid"		, CBSelectBuyScionSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectUpgradeScionGrid"	, CBSelectUpgradeScionSkillGrid , 0 );

	pcmUIManager->AddFunction( this , "Skill_SelectSlayerGrid"			, CBSelectSlayerSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectBuySlayerGrid"		, CBSelectBuySlayerSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectUpgradeSlayerGrid"	, CBSelectUpgradeSlayerSkillGrid , 0 );

	pcmUIManager->AddFunction( this , "Skill_SelectObiterGrid"			, CBSelectObiterSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectBuyObiterGrid"		, CBSelectBuyObiterSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectUpgradeObiterGrid"	, CBSelectUpgradeObiterSkillGrid , 0 );

	pcmUIManager->AddFunction( this , "Skill_SelectPassiveGrid"			, CBSelectPassiveSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectBuyPassiveGrid"		, CBSelectBuyPassiveSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectUpgradePassiveGrid"	, CBSelectUpgradePassiveSkillGrid , 0 );

	pcmUIManager->AddFunction( this , "Skill_SelectSummernerGrid"			, CBSelectSummernerSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectBuySummernerGrid"		, CBSelectBuySummernerSkillGrid , 0 );
	pcmUIManager->AddFunction( this , "Skill_SelectUpgradeSummernerGrid"	, CBSelectUpgradeSummernerSkillGrid , 0 );

	pcmUIManager->AddFunction( this , "Skill_SelectScionArchlordGrid"			, CBSelectArchlordSkillGrid , 0 );

	pcmUIManager->AddFunction( this , "Skill_Open_Scion_ToolTip"	,	CBOpenScionSkillToolTip		, 0 );
	pcmUIManager->AddFunction( this , "Skill_Close_Scion_ToolTip"	,	CBCloseScionSkillToolTip	, 0	);

	pcmUIManager->AddFunction( this , "Skill_Open_UpgradeScion_ToolTip"	,	CBOpenUpgradeScionSkillToolTip , 0 );
	pcmUIManager->AddFunction( this , "Skill_Close_UpgradeScion_ToolTip" ,	CBCloseUpgradeScionSkillToolTip, 0 );

	pcmUIManager->AddFunction( this , "Skill_Open_BuyScion_ToolTip"		,	CBOpenBuyScionSkillToolTip		, 0 );
	pcmUIManager->AddFunction( this , "Skill_Close_BuyScion_ToolTip"	,	CBCloseBuyScionSkillToolTip		, 0 );

	pcmUIManager->AddFunction( this , "Skill_Scion_Skill_Upgrade"		,	CBScionSkillUpgrade , 0 );
	pcmUIManager->AddFunction( this , "Skill_Slayer_Skill_Upgrade"		,	CBSlayerSkillUpgrade , 0 );
	pcmUIManager->AddFunction( this , "Skill_Obiter_Skill_Upgrade"		,	CBObiterSkillUpgrade , 0 );
	pcmUIManager->AddFunction( this , "Skill_Passive_Skill_Upgrade"		,	CBPassiveSkillUpgrade , 0 );
	pcmUIManager->AddFunction( this , "Skill_Summerner_Skill_Upgrade"	,	CBSummernerSkillUpgrade , 0 );

	pcmUIManager->AddFunction( this , "Skill_Scion_Skill_RollBack"		,	CBScionSkillRollBack	, 0 );
	pcmUIManager->AddFunction( this , "Skill_Slayer_Skill_RollBack"		,	CBSlayerSkillRollBack	, 0 );
	pcmUIManager->AddFunction( this , "Skill_Obiter_Skill_RollBack"		,	CBObiterSkillRollBack	, 0 );
	pcmUIManager->AddFunction( this , "Skill_Passive_Skill_RollBack"	,	CBPassiveSkillRollBack	, 0 );
	pcmUIManager->AddFunction( this , "Skill_Summerner_Skill_RollBack"	,	CBSummernerSkillRollBack , 0 );

	return;
}

void AgcmUISkillDragonScion::OnAddDisplay( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return;

	// 스킬창 Scion
	if( !pcmUIManager->AddDisplay( this , "SkillScion_0"   , SKILL_SCION_0	 , CBDisplaySkillScion0	  , AGCDUI_USERDATA_TYPE_INT32  ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_1_1" , SKILL_SCION_1_1 , CBDisplaySkillScion1_1 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_1_2" , SKILL_SCION_1_2 , CBDisplaySkillScion1_2 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_1_3" , SKILL_SCION_1_3 , CBDisplaySkillScion1_3 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_2_1" , SKILL_SCION_2_1 , CBDisplaySkillScion2_1 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_2_2" , SKILL_SCION_2_2 , CBDisplaySkillScion2_2 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_2_3" , SKILL_SCION_2_3 , CBDisplaySkillScion2_3 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_3_1" , SKILL_SCION_3_1 , CBDisplaySkillScion3_1 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_3_2" , SKILL_SCION_3_2 , CBDisplaySkillScion3_2 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_4_1" , SKILL_SCION_4_1 , CBDisplaySkillScion4_1 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_4_2" , SKILL_SCION_4_2 , CBDisplaySkillScion4_2 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_4_3" , SKILL_SCION_4_3 , CBDisplaySkillScion4_3 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_5_1" , SKILL_SCION_5_1 , CBDisplaySkillScion5_1 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_5_2" , SKILL_SCION_5_2 , CBDisplaySkillScion5_2 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_5_3" , SKILL_SCION_5_3 , CBDisplaySkillScion5_3 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_6_1" , SKILL_SCION_6_1 , CBDisplaySkillScion6_1 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_6_2" , SKILL_SCION_6_2 , CBDisplaySkillScion6_2 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillScion_6_3" , SKILL_SCION_6_3 , CBDisplaySkillScion6_3 , AGCDUI_USERDATA_TYPE_INT32	) )
		return;
	
	// 스킬창 Obiter
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_1_1" , SKILL_OBITER_1_1 , CBDisplaySkillObiter1_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_1_2" , SKILL_OBITER_1_2 , CBDisplaySkillObiter1_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_1_3" , SKILL_OBITER_1_3 , CBDisplaySkillObiter1_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_1_4" , SKILL_OBITER_1_4 , CBDisplaySkillObiter1_4 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_1_5" , SKILL_OBITER_1_5 , CBDisplaySkillObiter1_5 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_2_1" , SKILL_OBITER_2_1 , CBDisplaySkillObiter2_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_2_2" , SKILL_OBITER_2_2 , CBDisplaySkillObiter2_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_2_3" , SKILL_OBITER_2_3 , CBDisplaySkillObiter2_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_3_1" , SKILL_OBITER_3_1 , CBDisplaySkillObiter3_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_3_2" , SKILL_OBITER_3_2 , CBDisplaySkillObiter3_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_3_3" , SKILL_OBITER_3_3 , CBDisplaySkillObiter3_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillObiter_3_4" , SKILL_OBITER_3_4 , CBDisplaySkillObiter3_4 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;

	// 스킬창 Slayer
	if( !pcmUIManager->AddDisplay( this , "SkillSlayer_1_1" , SKILL_SLAYER_1_1 , CBDisplaySkillSlayer1_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSlayer_1_2" , SKILL_SLAYER_1_2 , CBDisplaySkillSlayer1_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSlayer_1_3" , SKILL_SLAYER_1_3 , CBDisplaySkillSlayer1_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SKillSlayer_1_4" , SKILL_SLAYER_1_4 , CBDisplaySkillSlayer1_4 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSlayer_2_1" , SKILL_SLAYER_2_1 , CBDisplaySkillSlayer2_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSlayer_2_2" , SKILL_SLAYER_2_2 , CBDisplaySkillSlayer2_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SKillSlayer_2_3" , SKILL_SLAYER_2_3 , CBDisplaySkillSlayer2_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSlayer_2_4" , SKILL_SLAYER_2_4 , CBDisplaySkillSlayer2_4 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSlayer_3_1" , SKILL_SLAYER_3_1 , CBDisplaySkillSlayer3_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSlayer_3_2" , SKILL_SLAYER_3_2 , CBDisplaySkillSlayer3_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SKillSlayer_3_3" , SKILL_SLAYER_3_3 , CBDisplaySkillSlayer3_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;

	// 스킬창 Summerner
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_1_1" , SKILL_SUMMENER_1_1 , CBDisplaySkillSummener1_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_1_2" , SKILL_SUMMENER_1_2 , CBDisplaySkillSummener1_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_1_3" , SKILL_SUMMENER_1_3 , CBDisplaySkillSummener1_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SKillSummener_1_4" , SKILL_SUMMENER_1_4 , CBDisplaySkillSummener1_4 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_1_5" , SKILL_SUMMENER_1_5 , CBDisplaySkillSummener1_5 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_2_1" , SKILL_SUMMENER_2_1 , CBDisplaySkillSummener2_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_2_2" , SKILL_SUMMENER_2_2 , CBDisplaySkillSummener2_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_2_3" , SKILL_SUMMENER_2_3 , CBDisplaySkillSummener2_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_2_4" , SKILL_SUMMENER_2_4 , CBDisplaySkillSummener2_4 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_3_1" , SKILL_SUMMENER_3_1 , CBDisplaySkillSummener3_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SKillSummener_3_2" , SKILL_SUMMENER_3_2 , CBDisplaySkillSummener3_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillSummener_3_3" , SKILL_SUMMENER_3_3 , CBDisplaySkillSummener3_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;

	// 스킬창 Passive
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_1_1" , SKILL_PASSIVE_1_1 , CBDisplaySkillPassive1_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_1_2" , SKILL_PASSIVE_1_2 , CBDisplaySkillPassive1_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_1_3" , SKILL_PASSIVE_1_3 , CBDisplaySkillPassive1_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_2_1" , SKILL_PASSIVE_2_1 , CBDisplaySkillPassive2_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_2_2" , SKILL_PASSIVE_2_2 , CBDisplaySkillPassive2_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_2_3" , SKILL_PASSIVE_2_3 , CBDisplaySkillPassive2_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_3_1" , SKILL_PASSIVE_3_1 , CBDisplaySkillPassive3_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_3_2" , SKILL_PASSIVE_3_2 , CBDisplaySkillPassive3_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_3_3" , SKILL_PASSIVE_3_3 , CBDisplaySkillPassive3_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_4_1" , SKILL_PASSIVE_4_1 , CBDisplaySkillPassive4_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_4_2" , SKILL_PASSIVE_4_2 , CBDisplaySkillPassive4_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_4_3" , SKILL_PASSIVE_4_3 , CBDisplaySkillPassive4_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_5_1" , SKILL_PASSIVE_5_1 , CBDisplaySkillPassive5_1 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_5_2" , SKILL_PASSIVE_5_2 , CBDisplaySkillPassive5_2 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;
	if( !pcmUIManager->AddDisplay( this , "SkillPassive_5_3" , SKILL_PASSIVE_5_3 , CBDisplaySkillPassive5_3 , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;

	if( !pcmUIManager->AddDisplay( this , "ScionSkillPoint" , 0 , CBDisplayScionSkillPoint , AGCDUI_USERDATA_TYPE_INT32 ) )
		return;

	return;
}

VOID AgcmUISkillDragonScion::OnAddUserData( VOID* pUIManager )
{
	AgcmUIManager2*	pcmUIManager	=	static_cast< AgcmUIManager2* >(pUIManager);
	if( !pcmUIManager )		return;

	CHAR		szName	[ 100 ];
	CHAR		szName2	[ 100 ];
	CHAR		szDisplayName[ 100 ];
	CHAR		szDisplayName2[ 100 ];
	ZeroMemory( szName  , 100 );
	ZeroMemory( szName2 , 100 );
	ZeroMemory( szDisplayName , 100 );
	ZeroMemory( szDisplayName2 , 100 );
	
	for( INT i = 0 ; i < SKILL_SCION_COUNT ; ++i )
	{
		sprintf_s( szName  , 100 , "SkillScionUserData_%d"		, i );
		sprintf_s( szName2 , 100 , "SkillUpScionUserData_%d"	, i );

		sprintf_s( szDisplayName	, 100 , "Skill_Scion_Display_%d" , i );
		sprintf_s( szDisplayName2	, 100 , "Skill_Up_Scion_Display_%d" , i);

		m_pScionDisplayUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName , &m_nScionDisplayUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pScionDisplayUpUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName2 , &m_nScionDisplayUpUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pScionSkillUserData[i]	=	
			pcmUIManager->AddUserData( szName , &m_ScionSkillGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );

		m_pScionSkillUpUserData[i]	=
			pcmUIManager->AddUserData( szName2 , &m_ScionSkillUpGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );

	}

	for ( INT i = 0 ; i < SKILL_SLAYER_COUNT ;++i )
	{
		sprintf_s( szName	, 100 , "SkillSlayerUserData_%d"	, i );
		sprintf_s( szName2	, 100 ,	"SkillUpSlayerUserData_%d"	, i );

		sprintf_s( szDisplayName	, 100 , "Skill_Slayer_Display_%d" , i );
		sprintf_s( szDisplayName2	, 100 , "Skill_Up_Slayer_Display_%d" , i );

		m_pSlayerDisplayUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName , &m_nSlayerDisplayUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pSlayerDisplayUpUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName2 , &m_nSlayerDisplayUpUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pSlayerSkillUserData[i]		=
			pcmUIManager->AddUserData( szName , &m_SlayerSkillGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );

		m_pSlayerSkillUpUserData[i]	=
			pcmUIManager->AddUserData( szName2 , &m_SlayerSkillUpGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );
	}

	for( INT i = 0 ; i < SKILL_SUMMENER_COUNT ; ++i )
	{
		sprintf_s( szName	, 100 , "SkillSummernerUserData_%d"		, i );
		sprintf_s( szName2	, 100 , "SkillUpSummernerUserData_%d"	, i );

		sprintf_s( szDisplayName	, 100 , "Skill_Summerner_Display_%d" , i );
		sprintf_s( szDisplayName2	, 100 , "Skill_Up_Summerner_Display_%d" , i );

		m_pSummernerDisplayUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName , &m_nSummernerDisplayUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pSummernerDisplayUpUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName2 , &m_nSummernerDisplayUpUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pSummernerSkillUserData[i]	=
			pcmUIManager->AddUserData( szName , &m_SummernerSkillGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );
		
		m_pSummernerSkillUpUserData[i]	=
			pcmUIManager->AddUserData( szName2 , &m_SummernerSkillUpGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );
	}

	for( INT i = 0 ; i < SKILL_OBITER_COUNT ; ++i )
	{
		sprintf_s( szName	, 100 , "SkillObiterUserData_%d"	, i );
		sprintf_s( szName2	, 100 , "SkillUpObiterUserData_%d"	, i );

		sprintf_s( szDisplayName	, 100 , "Skill_Obiter_Display_%d" , i );
		sprintf_s( szDisplayName2	, 100 , "Skill_Up_Obiter_Display_%d" , i );

		m_pObiterDisplayUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName , &m_nObiterDisplayUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pObiterDisplayUpUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName2 , &m_nObiterDisplayUpUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pObiterSkillUserData[i]		=
			pcmUIManager->AddUserData( szName , &m_ObiterSkillGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );
		m_pObiterSkillUpUserData[i]	=
			pcmUIManager->AddUserData( szName2 , &m_ObiterSkillUpGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );
	}

	for( INT i = 0 ; i < SKILL_PASSIVE_COUNT ; ++i )
	{
		sprintf_s( szName	, 100 , "SkillPassiveUserData_%d"	, i );
		sprintf_s( szName2	, 100 , "SkillUpPassiveUserData_%d"	, i );

		sprintf_s( szDisplayName	, 100 , "Skill_Passive_Display_%d" , i );
		sprintf_s( szDisplayName2	, 100 , "Skill_Up_Passive_Display_%d" , i );

		m_pPassiveDisplayUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName , &m_nPassiveDisplayUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pPassiveDisplayUpUserData[i]	=	
			pcmUIManager->AddUserData( szDisplayName2 , &m_nPassiveDisplayUpUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pPassiveSkillUserData[i]		=
			pcmUIManager->AddUserData( szName , &m_PassiveSkillGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );

		m_pPassiveSkillUpUserData[i]	=
			pcmUIManager->AddUserData( szName2 , &m_PassiveSkillUpGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );
	}

	// Archlord Skill Grid
	m_pArchlordSkillUserData	= m_pcsAgcmUIManager2->AddUserData("ScionArchlordSkillListGrid", m_ArchlordSkillGrid, sizeof(AgpdGrid), 0, AGCDUI_USERDATA_TYPE_GRID);

	// Scion Skill Point
	m_pScionSkillPointUserData	= pcmUIManager->AddUserData( "ScionSkillpoint" , &m_nScionSkillPoint , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

	// Scion Rollback Button
	m_pScionActiveRollbackButtonUserData	= m_pcsAgcmUIManager2->AddUserData( "Skill_ScionActiveRollbackButton",	&m_bScionActiveRollbackButton,	sizeof(m_bScionActiveRollbackButton),	1,	AGCDUI_USERDATA_TYPE_BOOL);

}

VOID AgcmUISkillDragonScion::OnAddBoolean( VOID* pUIManager )
{
	AgcmUIManager2*	pcmUIManager	=	static_cast< AgcmUIManager2* >(pUIManager);
	if( !pcmUIManager )		return;

}


BOOL AgcmUISkillDragonScion::OnOpenUI( eDragonScionSkillUIType eOpenType )
{
	if( !GetUIManager() ) return FALSE;

	INT32 nEvent = -1;
	switch( eOpenType )
	{
	case SkillUI_Scion :		nEvent = m_nEventOpenUIScion;					break;
	case SkillUI_Slayer :		nEvent = m_nEventOpenUISlayer;					break;
	case SkillUI_Obiter :		nEvent = m_nEventOpenUIObiter;					break;
	case SkillUI_Summerner :	nEvent = m_nEventOpenUISummerner;				break;
	case SkillUI_Passive :		nEvent = m_nEventOpenUIPassive;					break;
	case SkillUI_Archlord :		nEvent = m_nEventOpenUIArchlord;				break;
	case SkillUI_Heroic:		nEvent = m_pAgcmUISkill2->m_HeroicSkill.GetHeroicOpenEvent();	break;
	default :					return FALSE;						
	}

	_MoveSkillWindow( eOpenType );

	m_eCurrentUI	= eOpenType;
	m_nLastOpen		=	1;
	m_nArrSelectIndex[ eOpenType ]		=	-1;

	if( eOpenType == SkillUI_Heroic )
		m_pAgcmUISkill2->m_HeroicSkill.SetSelectIndex( -1 );

	LoadSkill( eOpenType );

	GetUIManager()->ThrowEvent(GetUIManager()->m_nEventToggleUIOpen);

	return GetUIManager()->ThrowEvent( nEvent );
}

VOID AgcmUISkillDragonScion::UnSelectSkillUI( eDragonScionSkillUIType eType )
{
	switch( eType )
	{
	case SkillUI_Scion:			_UnSelectSkillScion();					break;
	case SkillUI_Slayer:		_UnSelectSkillSlayer();					break;
	case SkillUI_Obiter:		_UnSelectSkillObiter();					break;
	case SkillUI_Summerner:		_UnSelectSkillSummerner();				break;
	case SkillUI_Passive:		_UnSelectSkillPassive();				break;
	case SkillUI_Archlord:		_UnSelectSkillArchlord();				break;
	case SkillUI_Heroic:		m_pAgcmUISkill2->m_HeroicSkill.UnSelectSkillHeroic();	break;
	}
}

VOID AgcmUISkillDragonScion::UnSelectUpgradeSkillUI( eDragonScionSkillUIType eType )
{

	INT32		nSkillCount		=	0;
	INT32		*pEventSkill	=	NULL;

	switch( eType )
	{
	case SkillUI_Scion:			pEventSkill	=	m_nEventScionUpgradeUnSelectedItem;		nSkillCount = SKILL_SCION_COUNT;	break;
	case SkillUI_Slayer:		pEventSkill =	m_nEventSlayerUpgradeUnSelectedItem;	nSkillCount = SKILL_SLAYER_COUNT;	break;
	case SkillUI_Obiter:		pEventSkill =	m_nEventObiterUpgradeUnSelectedItem;	nSkillCount = SKILL_OBITER_COUNT;	break;
	case SkillUI_Summerner:		pEventSkill =	m_nEventSummernerUpgradeUnSelectedItem;	nSkillCount = SKILL_SUMMENER_COUNT;	break;
	case SkillUI_Passive:		pEventSkill =	m_nEventPassiveUpgradeUnSelectedItem;	nSkillCount = SKILL_PASSIVE_COUNT;	break;
	case SkillUI_Heroic:		m_pAgcmUISkill2->m_HeroicSkill.UnSelectSkillHeroicUpgrade();				return;
	}
	
 	for( INT i = 0 ; i < nSkillCount ; ++i )
	{
		m_pcsAgcmUIManager2->ThrowEvent( pEventSkill[i] );
	}
}

VOID AgcmUISkillDragonScion::_UnSelectSkillScion( VOID )
{
	for( INT i = 0 ; i < SKILL_SCION_COUNT ; ++i )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventScionUnSelectedItem[i] );
	}
}

VOID AgcmUISkillDragonScion::_UnSelectSkillSlayer( VOID )
{
	for( INT i = 0 ; i < SKILL_SLAYER_COUNT ; ++i )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSlayerUnSelectedItem[i] );
	}
}

VOID AgcmUISkillDragonScion::_UnSelectSkillObiter( VOID )
{
	for( INT i = 0 ; i < SKILL_OBITER_COUNT ; ++i )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventObiterUnSelectedItem[i] );
	}
}

VOID AgcmUISkillDragonScion::_UnSelectSkillSummerner( VOID )
{
	for( INT i = 0 ; i < SKILL_SUMMENER_COUNT ; ++i )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSummernerUnSelectedItem[i] );
	}
}

VOID AgcmUISkillDragonScion::_UnSelectSkillPassive( VOID )
{
	for( INT i = 0 ; i < SKILL_PASSIVE_COUNT ; ++i )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventPassiveUnSelectedItem[i] );
	}
}

VOID AgcmUISkillDragonScion::_UnSelectSkillArchlord( VOID )
{

}

VOID AgcmUISkillDragonScion::LoadSkill( eDragonScionSkillUIType eType )
{
	switch(	eType )
	{
	case SkillUI_Scion:			_LoadScionSkill();					break;
	case SkillUI_Slayer:		_LoadSlayerSkill();					break;
	case SkillUI_Obiter:		_LoadObiterSkill();					break;
	case SkillUI_Summerner:		_LoadSummernerSkill();				break;
	case SkillUI_Passive:		_LoadPassiveSkill();				break;
	case SkillUI_Archlord:		_LoadArchlordSkill();				break;
	case SkillUI_Heroic:		m_pAgcmUISkill2->m_HeroicSkill.LoadHeroicSkill();	break;
	default:														return;
	}
}

VOID AgcmUISkillDragonScion::LoadBuySkill( eDragonScionSkillUIType eType )
{
	switch(	eType )
	{
	case SkillUI_Scion:			_LoadScionSkillBuy();				break;
	case SkillUI_Slayer:		_LoadSlayerSkillBuy();				break;
	case SkillUI_Obiter:		_LoadObiterSkillBuy();				break;
	case SkillUI_Summerner:		_LoadSummernerSkillBuy();			break;
	case SkillUI_Passive:		_LoadPassiveSkillBuy();				break;
	case SkillUI_Heroic:		m_pAgcmUISkill2->m_HeroicSkill.LoadHeroicSkillBuy();	break;
	default:														return;
	}
}

BOOL AgcmUISkillDragonScion::OnCloseUI( VOID )
{
	if( !GetUIManager() ) return FALSE;

	INT32 nEvent = -1;
	switch( m_eCurrentUI )
	{
	case SkillUI_Scion :		nEvent = m_nEventCloseUIScion;					break;
	case SkillUI_Slayer :		nEvent = m_nEventCloseUISlayer;					break;
	case SkillUI_Obiter :		nEvent = m_nEventCloseUIObiter;					break;
	case SkillUI_Summerner :	nEvent = m_nEventCloseUISummerner;				break;
	case SkillUI_Passive :		nEvent = m_nEventCloseUIPassive;				break;
	case SkillUI_Archlord :		nEvent = m_nEventCloseUIArchlord;				break;
	case SkillUI_Heroic:		nEvent = m_pAgcmUISkill2->m_HeroicSkill.GetHeroicCloseEvent();	break;
	default :					return FALSE;						
	}

	m_eLastUI	=	m_eCurrentUI;	
	m_eCurrentUI = SkillUI_NotOpened;
	return GetUIManager()->ThrowEvent( nEvent );
}

BOOL AgcmUISkillDragonScion::OnOpenUpUI( eDragonScionSkillUIType eOpenType )
{
	if( !GetUIManager() ) return FALSE;

	INT32 nEvent = -1;
	if( eOpenType	!= SkillUI_NotOpened )		
	{
		nEvent		=	m_nEventOpenUpUIScion[ eOpenType - 1 ];
	}
	else	
	{
		return FALSE;
	}
	
	_MoveSkillUpgradeWindow( eOpenType );

	m_eCurrentUpUI							=	eOpenType;
	m_nLastOpen								=	2;
	m_nArrUpgradeSelectIndex[ eOpenType ]	=	-1;
	if( eOpenType == SkillUI_Heroic )
	{
		nEvent		=	m_pAgcmUISkill2->m_HeroicSkill.GetHeroicUpOpenEvent();
		m_pAgcmUISkill2->m_HeroicSkill.SetSelectIndexUpgrade( -1 );
	}

	LoadBuySkill( eOpenType );

	return GetUIManager()->ThrowEvent( nEvent );
}

BOOL AgcmUISkillDragonScion::OnCloseUpUI( VOID )
{
	if( !GetUIManager() )	return FALSE;

	INT32	nEvent	=	-1;
	if( m_eCurrentUpUI	!= SkillUI_NotOpened )
	{
		nEvent		=	m_nEventCloseUpUIScion[ m_eCurrentUpUI - 1 ];
	}

	if( m_eCurrentUpUI	==	SkillUI_Heroic )
	{
		nEvent		=	m_pAgcmUISkill2->m_HeroicSkill.GetHeroicUpCloseEvent();
	}

	m_eLastUpUI		=	m_eCurrentUpUI;
	m_eCurrentUpUI	=	SkillUI_NotOpened;
	return GetUIManager()->ThrowEvent( nEvent );
}

BOOL AgcmUISkillDragonScion::OpenSkillUI( eDragonScionSkillUIType eType )
{

	OnCloseUI( );
	return OnOpenUI(  eType );
}

BOOL AgcmUISkillDragonScion::OpenSkillUpUI( eDragonScionSkillUIType eType )
{
	OnCloseUpUI();
	return OnOpenUpUI( eType );
}

BOOL AgcmUISkillDragonScion::CloseSkillUI( VOID )
{
	OnCloseUI();
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CloseSkillUpUI( VOID )
{
	OnCloseUpUI();
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CallBackGetBaseControlScion( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUI( SkillUI_Scion );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CallBackGetBaseControlSlayer( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUI( SkillUI_Slayer );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CallBackGetBaseControlObiter( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUI( SkillUI_Obiter );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CallBackGetBaseControlSummerner( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUI( SkillUI_Summerner );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CallBackGetBaseControlPassive( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUI( SkillUI_Passive );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CallBackGetBaseControlHeroic( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUI( SkillUI_Heroic );

	return TRUE;
}


BOOL AgcmUISkillDragonScion::CallBackArchlord( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUI( SkillUI_Archlord );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBOpenScionUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUpUI( SkillUI_Scion );
	
	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBOpenSlayerUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUpUI( SkillUI_Slayer );
	if( !pThis || !pData1 ) return FALSE;

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBOpenObiterUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUpUI( SkillUI_Obiter );
	if( !pThis || !pData1 ) return FALSE;

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBOpenSummernerUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUpUI( SkillUI_Summerner );
	if( !pThis || !pData1 ) return FALSE;

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBOpenPassiveUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUpUI( SkillUI_Passive );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CBOpenHeroicUpgrade( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	pThis->OpenSkillUpUI( SkillUI_Heroic );

	return TRUE;
}

BOOL AgcmUISkillDragonScion::CallBackUIScroll( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUISkillDragonScion* pThis	=	static_cast< AgcmUISkillDragonScion* >(pClass);
	if( !pThis || !pControl ) return FALSE;

	AcUIScroll* pScroll = ( AcUIScroll* )pControl->m_pcsBase;
	AcUIList* pList = ( AcUIList* )pControl->m_pcsBase;
	if( !pScroll || !pList ) return FALSE;

	//AcUIList*	pBtnBase	=	static_cast< AcUIList* >( pThis->GetCurrentBaseControl() );
	AcUIList* pBtnBase		=	NULL;
	if( !pBtnBase ) return FALSE;

	// Step1. 스크롤바의 이동범위와 현재 포지션을 가져온다.
	INT32 nScrollMin = pScroll->m_lMinPosition;
	INT32 nScrollMax = pScroll->m_lMaxPosition;
	float fScrollCurrent = pScroll->GetScrollValue();

	// Step2. View 영역의 세로 크기와 BG 컨트롤의 세로크기를 가져온다.
	INT32 nViewHeight = pList->h;
	//INT32 nControlHeight = pList->m_lListItemHeight;
	INT32 nControlHeight = pList->w;

	// Step3. 스크롤바의 위치비율에 맞춰 BG 컨트롤의 위치를 조정한다.
	INT32 nNewControlPosY = ( INT32 )( ( float )( nControlHeight - nViewHeight ) * fScrollCurrent );
	//pBtnBase->MoveWindow( 0, nNewControlPosY, pBtnBase->w, pBtnBase->h );

	return TRUE;
}

VOID AgcmUISkillDragonScion::_LoadScionSkill( VOID )
{
	AgpdCharacter*		pAgpdCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SCION );
	if(!pVector)
	{
		for( INT i = 0 ; i < SKILL_SCION_COUNT ; ++i )
		{
			m_pcsAgpmGrid->Reset( &m_ScionSkillGrid[i] );
		}
	}

	else
	{
		INT i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i < TOTAL_SCIONSKILL_COUNT )
		{
			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

			if(!pcsSkillTemplate)
			{
				++iter; ++i;
				continue;
			}

			m_pcsAgpmGrid->Reset( &m_ScionSkillGrid[i] );

			// 일단 그리드 텍스쳐는 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);

			// 스킬을 배웠으면 잘 나오고 아니면 그레이로 나온다.
			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(pAgpdCharacter, pcsSkillTemplate->m_szName);
			if(pcsSkill && pcsSkill->m_pcsTemplate)
			{
				// Active 해준다.
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventScionActiveItem[i]);

				if (m_pcsAgpmGrid->Add(& m_ScionSkillGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1 ) )
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pScionSkillUserData[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pScionDisplayUserData[i]);
				}
			}
			else
			{
				// Disable 해준다.
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventScionDisableItem[i]);

				if (m_pcsAgpmGrid->Add( &m_ScionSkillGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1 ))
				{	
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pScionSkillUserData[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pScionDisplayUserData[i]);
				}
			}

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for(int i = 0; i < SKILL_SCION_COUNT; ++i)
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventScionUnSelectedItem[i]);

	if( m_nArrSelectIndex[SkillUI_Scion] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventScionSelectedItem[ m_nArrSelectIndex[SkillUI_Scion] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
}

VOID AgcmUISkillDragonScion::_LoadSlayerSkill( VOID )
{
	AgpdCharacter*		pAgpdCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SLAYER );
	if(!pVector)
	{
		for( INT i = 0 ; i < SKILL_SLAYER_COUNT ; ++i )
		{
			m_pcsAgpmGrid->Reset( &m_SlayerSkillGrid[i] );
		}
	}

	else
	{
		INT i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i < TOTAL_SCIONSKILL_COUNT )
		{
			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

			m_pcsAgpmGrid->Reset( &m_SlayerSkillGrid[i] );

			if(!pcsSkillTemplate)
			{
				++iter; ++i;
				continue;
			}

			// 일단 그리드 텍스쳐는 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);

			// 스킬을 배웠으면 잘 나오고 아니면 그레이로 나온다.
			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(pAgpdCharacter, pcsSkillTemplate->m_szName);
			if(pcsSkill && pcsSkill->m_pcsTemplate && eClassType == AUCHARCLASS_TYPE_KNIGHT )
			{
				// Active 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventSlayerActiveItem[i] );

				if (m_pcsAgpmGrid->Add(& m_SlayerSkillGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1 ) )
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pSlayerSkillUserData[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pSlayerDisplayUserData[i]);
				}
			}
			else
			{
				// Disable 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventSlayerDisableItem[i] );

				if (m_pcsAgpmGrid->Add( &m_SlayerSkillGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1 ))
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pSlayerSkillUserData[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pSlayerDisplayUserData[i]);
				}
			}

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for(int i = 0; i < SKILL_SLAYER_COUNT; ++i)
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSlayerUnSelectedItem[i] );

	if( m_nArrSelectIndex[SkillUI_Slayer] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSlayerSelectedItem[ m_nArrSelectIndex[SkillUI_Slayer] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
}

VOID AgcmUISkillDragonScion::_LoadObiterSkill( VOID )
{
	AgpdCharacter*		pAgpdCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_OBITER );
	if(!pVector)
	{
		for( INT i = 0 ; i < SKILL_OBITER_COUNT ; ++i )
		{
			m_pcsAgpmGrid->Reset( &m_ObiterSkillGrid[i] );
		}
	}

	else
	{
		INT i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i < TOTAL_SCIONSKILL_COUNT )
		{
			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

			if(!pcsSkillTemplate)
			{
				++iter; ++i;
				continue;
			}

			m_pcsAgpmGrid->Reset( &m_ObiterSkillGrid[i] );

			// 일단 그리드 텍스쳐는 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);

			// 스킬을 배웠으면 잘 나오고 아니면 그레이로 나온다.
			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(pAgpdCharacter, pcsSkillTemplate->m_szName);
			if(pcsSkill && pcsSkill->m_pcsTemplate && eClassType == AUCHARCLASS_TYPE_RANGER )
			{
				// Active 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventObiterActiveItem[i] );

				if (m_pcsAgpmGrid->Add(& m_ObiterSkillGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1 ) )
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pObiterSkillUserData[i]	);
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pObiterDisplayUserData[i]	);
				}
			}
			else
			{
				// Disable 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventObiterDisableItem[i]);

				if (m_pcsAgpmGrid->Add( &m_ObiterSkillGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1 ))
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pObiterSkillUserData[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pObiterDisplayUserData[i]);
				}
			}

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for(int i = 0; i < SKILL_OBITER_COUNT; ++i)
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventObiterUnSelectedItem[i] );

	if( m_nArrSelectIndex[SkillUI_Obiter] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventObiterSelectedItem[ m_nArrSelectIndex[SkillUI_Obiter] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
}

VOID AgcmUISkillDragonScion::_LoadSummernerSkill( VOID )
{
	AgpdCharacter*		pAgpdCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SUMMERNER );
	if(!pVector)
	{
		for( INT i = 0 ; i < SKILL_SUMMENER_COUNT ; ++i )
		{
			m_pcsAgpmGrid->Reset( &m_SummernerSkillGrid[i] );
		}
	}

	else
	{
		INT i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i < TOTAL_SCIONSKILL_COUNT )
		{
			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

			if(!pcsSkillTemplate)
			{
				++iter; ++i;
				continue;
			}

			m_pcsAgpmGrid->Reset( &m_SummernerSkillGrid[i] );

			// 일단 그리드 텍스쳐는 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);

			// 스킬을 배웠으면 잘 나오고 아니면 그레이로 나온다.
			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(pAgpdCharacter, pcsSkillTemplate->m_szName);
			if(pcsSkill && pcsSkill->m_pcsTemplate && eClassType == AUCHARCLASS_TYPE_MAGE )
			{
				// Active 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventSummernerActiveItem[i] );

				if (m_pcsAgpmGrid->Add(& m_SummernerSkillGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1 ) )
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pSummernerSkillUserData[i]	);
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pSummernerDisplayUserData[i]	);
				}
			}
			else
			{
				// Disable 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventSummernerDisableItem[i] );

				if (m_pcsAgpmGrid->Add( &m_SummernerSkillGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1 ))
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pSummernerSkillUserData[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pSummernerDisplayUserData[i]);
				}
			}

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for(int i = 0; i < SKILL_SUMMENER_COUNT; ++i)
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSummernerUnSelectedItem[i] );

	if( m_nArrSelectIndex[SkillUI_Summerner] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSummernerSelectedItem[ m_nArrSelectIndex[SkillUI_Summerner] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
}

VOID AgcmUISkillDragonScion::_LoadPassiveSkill( VOID )
{
	AgpdCharacter*		pAgpdCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( AGPMEVENT_HIGHLEVEL_PASSIVE_SCION );
	if(!pVector)
	{
		for( INT i = 0 ; i < SKILL_PASSIVE_COUNT ; ++i )
		{
			m_pcsAgpmGrid->Reset( &m_PassiveSkillGrid[i] );
		}
	}

	else
	{
		INT i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i < TOTAL_SCIONSKILL_COUNT )
		{
			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

			if(!pcsSkillTemplate)
			{
				++iter; ++i;
				continue;
			}

			m_pcsAgpmGrid->Reset( &m_PassiveSkillGrid[i] );

			// 일단 그리드 텍스쳐는 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);

			// 스킬을 배웠으면 잘 나오고 아니면 그레이로 나온다.
			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(pAgpdCharacter, pcsSkillTemplate->m_szName);
			if(pcsSkill && pcsSkill->m_pcsTemplate)
			{
				// Active 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventPassiveActiveItem[i] );

				if (m_pcsAgpmGrid->Add(& m_PassiveSkillGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1 ) )
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pPassiveSkillUserData[i]	);
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pPassiveDisplayUserData[i]	);
				}
			}
			else
			{
				// Disable 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventPassiveDisableItem[i] );

				if (m_pcsAgpmGrid->Add( &m_PassiveSkillGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1 ))
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pPassiveSkillUserData[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pPassiveDisplayUserData[i]);
				}
			}

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for(int i = 0; i < SKILL_PASSIVE_COUNT; ++i)
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventPassiveUnSelectedItem[i] );

	if( m_nArrSelectIndex[SkillUI_Passive] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventPassiveSelectedItem[ m_nArrSelectIndex[SkillUI_Passive] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
}

VOID AgcmUISkillDragonScion::_LoadArchlordSkill( VOID )
{
	m_nArchlordSkillSelectIndex	=	-1;

	INT32	lAddedItem	= 0;

	AgpdCharacter		*pAgpdCharacter	=	m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pAgpdCharacter )
		return;

	ArchlordSkillList&	vcArchlordSkill =	m_pcsAgpmEventSkillMaster->GetArchlordSkillList();
	ArchlordSkillIter	iter			=	vcArchlordSkill.begin();

	int i = 0;
	while(iter != vcArchlordSkill.end() && i < DRAGONSCION_ARCHLORD_SKILL_COUNT )
	{
		m_pcsAgpmGrid->Reset(&m_ArchlordSkillGrid[i]);

		AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(*iter);
		if(!pcsSkillTemplate)
		{
			++iter; ++i;
			continue;
		}

		// 아크로드이면 잘 나오고 아니면 그레이로 나온다.
		AgpdSkill* pcsSkill	= m_pcsAgpmSkill->GetSkill(pAgpdCharacter, pcsSkillTemplate->m_szName);
		if (pcsSkill && pcsSkill->m_pcsTemplate)
		{
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkill);

			if (m_pcsAgpmGrid->Add(&m_ArchlordSkillGrid[lAddedItem], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1))
				++lAddedItem;
		}
		else
		{
			if(m_pcsAgpmCharacter->IsArchlord(pAgpdCharacter->m_szID))
				m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);
			else
				m_pcsAgcmSkill->SetGridSkillAttachedUnableTexture(pcsSkillTemplate);

			if (m_pcsAgpmGrid->Add(&m_ArchlordSkillGrid[lAddedItem], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1))
				++lAddedItem;
		}

		++iter;	++i;
	}

	m_pArchlordSkillUserData->m_stUserData.m_lCount	=	lAddedItem;

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pArchlordSkillUserData );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );

	m_pcsAgcmUIManager2->RefreshUserData( m_pArchlordSkillUserData );

	for (i = 0; i < lAddedItem; ++i)
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nArchlordSkillUnSelect , i );
		m_pcsAgcmUIManager2->ThrowEvent( m_nArchlordSkillActive	  , i );
	}
}

VOID AgcmUISkillDragonScion::_LoadScionSkillBuy( VOID )
{
	AgpdCharacter*		pAgpdCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();

	if( !pAgpdCharacter || !pAgpdCharacter->m_pcsCharacterTemplate)
		return;

	HighLevelSkillVector*	pVector		= m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SCION );
	if(!pVector)
	{
		for(int i = 0; i < SKILL_SCION_COUNT; ++i)
			m_pcsAgpmGrid->Reset( &m_ScionSkillUpGrid[i] );
	}
	else
	{
		int i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i <  TOTAL_SCIONSKILL_COUNT )
		{
			m_pcsAgpmGrid->Reset(&m_ScionSkillUpGrid[i]);

			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );
			if(!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
			{
				++iter; ++i;
				continue;
			}

			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkillByTID( pAgpdCharacter, pcsSkillTemplate->m_lID );
			if(pcsSkill)
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = pcsSkill->m_lID;
			else
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = 0;

			// 일단 그리드는 제대로 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);
			switch( IsActiveUpgradeItem(pcsSkillTemplate->m_lID ) )
			{
				// 배울수 있는 스킬이다
			case SKILL_STATUS_SUCCESS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventScionUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_ScionSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;

				// 배웠지만 다른클래스 스킬이라 그레이로 표시한다
			case SKILL_STATUS_OTHER_CLASS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventScionUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_ScionSkillUpGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1 , 1 );
				break;

				// 조건이 안 맞아 배울수 없는 스킬이다
			case SKILL_STATUS_FAIL:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventScionUpgradeDisableItem[i]);
				m_pcsAgpmGrid->Add(&m_ScionSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;
			}

			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pScionSkillUpUserData[i]	);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pScionDisplayUpUserData[i]);
			
			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for( INT i = 0 ; i < SKILL_SCION_COUNT ; ++i )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventScionUpgradeUnSelectedItem[i] );

	if( m_nArrUpgradeSelectIndex[SkillUI_Scion] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventScionUpgradeSelectedItem[ m_nArrUpgradeSelectIndex[SkillUI_Scion] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
	m_pAgcmUISkill2->RefreshInitButton();
}

VOID AgcmUISkillDragonScion::_LoadSlayerSkillBuy( VOID )
{
	AgpdCharacter*		pAgpdCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();

	if( !pAgpdCharacter || !pAgpdCharacter->m_pcsCharacterTemplate)
		return;

	//INT32 lSelectedCharTID = m_pcsAgpmEventSkillMaster->GetCharTID(m_eRaceType, m_eClassType);

	HighLevelSkillVector*	pVector		= m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SLAYER );
	if(!pVector)
	{
		for(int i = 0; i < SKILL_SLAYER_COUNT; ++i)
			m_pcsAgpmGrid->Reset( &m_SlayerSkillUpGrid[i] );
	}
	else
	{
		int i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i <  TOTAL_SCIONSKILL_COUNT )
		{
			m_pcsAgpmGrid->Reset(&m_SlayerSkillUpGrid[i]);

			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );
			if(!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
			{
				++iter; ++i;
				continue;
			}

			if( eClassType	!= AUCHARCLASS_TYPE_KNIGHT )
			{
				++iter;
				continue;
			}

			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkillByTID( pAgpdCharacter, pcsSkillTemplate->m_lID );
			if(pcsSkill)
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = pcsSkill->m_lID;
			else
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = 0;

			// 일단 그리드는 제대로 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);
			switch( IsActiveUpgradeItem(pcsSkillTemplate->m_lID ) )
			{
				// 배울수 있는 스킬이다
			case SKILL_STATUS_SUCCESS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventSlayerUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_SlayerSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;

				// 배웠지만 다른클래스 스킬이라 그레이로 표시한다
			case SKILL_STATUS_OTHER_CLASS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventSlayerUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_SlayerSkillUpGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1 , 1 );
				break;

				// 조건이 안 맞아 배울수 없는 스킬이다
			case SKILL_STATUS_FAIL:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventSlayerUpgradeDisableItem[i]);
				m_pcsAgpmGrid->Add(&m_SlayerSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;
			}

			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pSlayerSkillUpUserData[i]	);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pSlayerDisplayUpUserData[i]);
			
			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for( INT i = 0 ; i < SKILL_SLAYER_COUNT ; ++i )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSlayerUpgradeUnSelectedItem[i] );

	if( m_nArrUpgradeSelectIndex[SkillUI_Slayer] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSlayerUpgradeSelectedItem[ m_nArrUpgradeSelectIndex[SkillUI_Slayer] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
	m_pAgcmUISkill2->RefreshInitButton();
}

VOID AgcmUISkillDragonScion::_LoadObiterSkillBuy( VOID )
{
	AgpdCharacter*		pAgpdCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();

	if( !pAgpdCharacter || !pAgpdCharacter->m_pcsCharacterTemplate)
		return;

	//INT32 lSelectedCharTID = m_pcsAgpmEventSkillMaster->GetCharTID(m_eRaceType, m_eClassType);

	HighLevelSkillVector*	pVector		= m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_OBITER );
	if(!pVector)
	{
		for(int i = 0; i < SKILL_OBITER_COUNT; ++i)
			m_pcsAgpmGrid->Reset( &m_ObiterSkillUpGrid[i] );
	}
	else
	{
		int i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i <  TOTAL_SCIONSKILL_COUNT )
		{
			m_pcsAgpmGrid->Reset(&m_ObiterSkillUpGrid[i]);

			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );
			if(!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
			{
				++iter; ++i;
				continue;
			}

			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkillByTID( pAgpdCharacter, pcsSkillTemplate->m_lID );
			if(pcsSkill)
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = pcsSkill->m_lID;
			else
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = 0;

			// 일단 그리드는 제대로 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);
			switch( IsActiveUpgradeItem(pcsSkillTemplate->m_lID ) )
			{
				// 배울수 있는 스킬이다
			case SKILL_STATUS_SUCCESS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventObiterUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_ObiterSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;

				// 배웠지만 다른클래스 스킬이라 그레이로 표시한다
			case SKILL_STATUS_OTHER_CLASS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventObiterUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_ObiterSkillUpGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1 , 1 );
				break;

				// 조건이 안 맞아 배울수 없는 스킬이다
			case SKILL_STATUS_FAIL:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventObiterUpgradeDisableItem[i]);
				m_pcsAgpmGrid->Add(&m_ObiterSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;
			}

			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pObiterSkillUpUserData[i]	);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pObiterDisplayUpUserData[i]);
			

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for( INT i = 0 ; i < SKILL_OBITER_COUNT ; ++i )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventObiterUpgradeUnSelectedItem[i] );

	if( m_nArrUpgradeSelectIndex[SkillUI_Obiter] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventObiterUpgradeSelectedItem[ m_nArrUpgradeSelectIndex[SkillUI_Obiter] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
	m_pAgcmUISkill2->RefreshInitButton();
}

VOID AgcmUISkillDragonScion::_LoadSummernerSkillBuy( VOID )
{
	AgpdCharacter*		pAgpdCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();

	if( !pAgpdCharacter || !pAgpdCharacter->m_pcsCharacterTemplate)
		return;

	HighLevelSkillVector*	pVector		= m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SUMMERNER );
	if(!pVector)
	{
		for(int i = 0; i < SKILL_SUMMENER_COUNT; ++i)
			m_pcsAgpmGrid->Reset( &m_SummernerSkillUpGrid[i] );
	}
	else
	{
		int i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i <  TOTAL_SCIONSKILL_COUNT )
		{
			m_pcsAgpmGrid->Reset(&m_SummernerSkillUpGrid[i]);

			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );
			if(!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
			{
				++iter; ++i;
				continue;
			}

			if( eClassType	!= AUCHARCLASS_TYPE_MAGE )
			{
				++iter;
				continue;
			}

			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkillByTID( pAgpdCharacter, pcsSkillTemplate->m_lID );
			if(pcsSkill)
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = pcsSkill->m_lID;
			else
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = 0;

			// 일단 그리드는 제대로 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);
			switch( IsActiveUpgradeItem(pcsSkillTemplate->m_lID ) )
			{
				// 배울수 있는 스킬이다
			case SKILL_STATUS_SUCCESS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventSummernerUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_SummernerSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;

				// 배웠지만 다른클래스 스킬이라 그레이로 표시한다
			case SKILL_STATUS_OTHER_CLASS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventSummernerUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_SummernerSkillUpGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1 , 1 );
				break;

				// 조건이 안 맞아 배울수 없는 스킬이다
			case SKILL_STATUS_FAIL:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventSummernerUpgradeDisableItem[i]);
				m_pcsAgpmGrid->Add(&m_SummernerSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;
			}

			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pSummernerSkillUpUserData[i]	);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pSummernerDisplayUpUserData[i]);
			

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for( INT i = 0 ; i < SKILL_SUMMENER_COUNT ; ++i )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSummernerUpgradeUnSelectedItem[i] );

	if( m_nArrUpgradeSelectIndex[SkillUI_Summerner] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventSummernerUpgradeSelectedItem[ m_nArrUpgradeSelectIndex[SkillUI_Summerner] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
	m_pAgcmUISkill2->RefreshInitButton();
}

VOID AgcmUISkillDragonScion::_LoadPassiveSkillBuy( VOID )
{
	AgpdCharacter*		pAgpdCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();

	if( !pAgpdCharacter || !pAgpdCharacter->m_pcsCharacterTemplate)
		return;

	HighLevelSkillVector*	pVector		= m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( AGPMEVENT_HIGHLEVEL_PASSIVE_SCION );
	if(!pVector)
	{
		for(int i = 0; i < SKILL_PASSIVE_COUNT; ++i)
			m_pcsAgpmGrid->Reset( &m_PassiveSkillUpGrid[i] );
	}
	else
	{
		int i = 0;
		HighLevelSkillIter iter = pVector->begin();
		while(iter != pVector->end() && i <  TOTAL_SCIONSKILL_COUNT )
		{
			m_pcsAgpmGrid->Reset(&m_PassiveSkillUpGrid[i]);

			AgpdEventSkillHighLevel stHighLevel			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );
			if(!pcsSkillTemplate || !pcsSkillTemplate->m_pcsGridItem)
			{
				++iter; ++i;
				continue;
			}

			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkillByTID( pAgpdCharacter, pcsSkillTemplate->m_lID );
			if(pcsSkill)
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = pcsSkill->m_lID;
			else
				pcsSkillTemplate->m_pcsGridItem->m_lItemID = 0;

			// 일단 그리드는 제대로 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);
			switch( IsActiveUpgradeItem(pcsSkillTemplate->m_lID ) )
			{
				// 배울수 있는 스킬이다
			case SKILL_STATUS_SUCCESS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventPassiveUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_PassiveSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;

				// 배웠지만 다른클래스 스킬이라 그레이로 표시한다
			case SKILL_STATUS_OTHER_CLASS:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventPassiveUpgradeActiveItem[i]);
				m_pcsAgpmGrid->Add(&m_PassiveSkillUpGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1 , 1 );
				break;

				// 조건이 안 맞아 배울수 없는 스킬이다
			case SKILL_STATUS_FAIL:
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventPassiveUpgradeDisableItem[i]);
				m_pcsAgpmGrid->Add(&m_PassiveSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
				break;
			}

			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pPassiveSkillUpUserData[i]	);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pPassiveDisplayUpUserData[i]);
			

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for( INT i = 0 ; i < SKILL_PASSIVE_COUNT ; ++i )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventPassiveUpgradeUnSelectedItem[i] );

	if( m_nArrUpgradeSelectIndex[SkillUI_Passive] != -1 )
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventPassiveUpgradeSelectedItem[ m_nArrUpgradeSelectIndex[SkillUI_Passive] ] );

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pScionSkillPointUserData );
	m_pAgcmUISkill2->RefreshInitButton();
}

INT32 AgcmUISkillDragonScion::_GetScionSkillPoint( INT32 nIndex )
{
	INT32						nModifiedSkillLevel		=	0;
	INT32						nSkillLevel				=	0;

	AgpdCharacter*				pAgpdCharacter			=	m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdEventSkillHighLevel		stHighLevelSkill		=	_GetScionSkill( nIndex );
	AgpdSkillTemplate*			pcsSkillTemplate		=	m_pcsAgpmSkill->GetSkillTemplate( stHighLevelSkill.m_lSkillTID );
	if( !pcsSkillTemplate )		return 0;

	AgpdSkill*					pcsAgpdSkill			=	m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	if( pcsAgpdSkill )
	{
		nSkillLevel		=	m_pcsAgpmSkill->GetSkillLevel( pcsAgpdSkill );

		// 열린창이 SKILL 창이라면 추가 스킬포인트가 있는지 확인
		if( m_nLastOpen == 1 )
		{
			if(	m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate))
			{
				nModifiedSkillLevel = m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pAgpdCharacter);
				if(pcsAgpdSkill->m_pcsTemplate)
					nModifiedSkillLevel += m_pcsAgpmItem->GefEffectedSkillPlusLevel(pAgpdCharacter, pcsAgpdSkill->m_pcsTemplate->m_lID);
			}
		}

	}

	return (nModifiedSkillLevel+nSkillLevel);
}

INT32 AgcmUISkillDragonScion::_GetObiterSkillPoint( INT32 nIndex )
{
	INT32						nModifiedSkillLevel		=	0;
	INT32						nSkillLevel				=	0;

	AgpdCharacter*				pAgpdCharacter			=	m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdEventSkillHighLevel		stHighLevelSkill		=	_GetObiterSkill( nIndex );
	AgpdSkillTemplate*			pcsSkillTemplate		=	m_pcsAgpmSkill->GetSkillTemplate( stHighLevelSkill.m_lSkillTID );
	if( !pcsSkillTemplate )		return 0;

	AgpdSkill*					pcsAgpdSkill			=	m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	if( pcsAgpdSkill )
	{
		nSkillLevel		=	m_pcsAgpmSkill->GetSkillLevel( pcsAgpdSkill );

		// 열린창이 SKILL 창이라면 추가 스킬포인트가 있는지 확인
		if( m_nLastOpen == 1 )
		{
			if(	m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate))
			{
				nModifiedSkillLevel = m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pAgpdCharacter);
				if(pcsAgpdSkill->m_pcsTemplate)
					nModifiedSkillLevel += m_pcsAgpmItem->GefEffectedSkillPlusLevel(pAgpdCharacter, pcsAgpdSkill->m_pcsTemplate->m_lID);
			}
		}

	}

	return (nModifiedSkillLevel+nSkillLevel);
}

INT32 AgcmUISkillDragonScion::_GetSlayerSkillPoint( INT32 nIndex )
{
	INT32						nModifiedSkillLevel		=	0;
	INT32						nSkillLevel				=	0;

	AgpdCharacter*				pAgpdCharacter			=	m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdEventSkillHighLevel		stHighLevelSkill		=	_GetSlayerSkill( nIndex );
	AgpdSkillTemplate*			pcsSkillTemplate		=	m_pcsAgpmSkill->GetSkillTemplate( stHighLevelSkill.m_lSkillTID );
	if( !pcsSkillTemplate )		return 0;

	AgpdSkill*					pcsAgpdSkill			=	m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	if( pcsAgpdSkill )
	{
		nSkillLevel		=	m_pcsAgpmSkill->GetSkillLevel( pcsAgpdSkill );

		// 열린창이 SKILL 창이라면 추가 스킬포인트가 있는지 확인
		if( m_nLastOpen == 1 )
		{
			if(	m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate))
			{
				nModifiedSkillLevel = m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pAgpdCharacter);
				if(pcsAgpdSkill->m_pcsTemplate)
					nModifiedSkillLevel += m_pcsAgpmItem->GefEffectedSkillPlusLevel(pAgpdCharacter, pcsAgpdSkill->m_pcsTemplate->m_lID);
			}
		}

	}

	return (nModifiedSkillLevel+nSkillLevel);
}

INT32 AgcmUISkillDragonScion::_GetPassiveSkillPoint( INT32 nIndex )
{
	INT32						nModifiedSkillLevel		=	0;
	INT32						nSkillLevel				=	0;

	AgpdCharacter*				pAgpdCharacter			=	m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdEventSkillHighLevel		stHighLevelSkill		=	_GetPassiveSkill( nIndex );
	AgpdSkillTemplate*			pcsSkillTemplate		=	m_pcsAgpmSkill->GetSkillTemplate( stHighLevelSkill.m_lSkillTID );
	if( !pcsSkillTemplate )		return 0;

	AgpdSkill*					pcsAgpdSkill			=	m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	if( pcsAgpdSkill )
	{
		nSkillLevel		=	m_pcsAgpmSkill->GetSkillLevel( pcsAgpdSkill );

		// 열린창이 SKILL 창이라면 추가 스킬포인트가 있는지 확인
		if( m_nLastOpen == 1 )
		{
			if(	m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate))
			{
				nModifiedSkillLevel = m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pAgpdCharacter);
				if(pcsAgpdSkill->m_pcsTemplate)
					nModifiedSkillLevel += m_pcsAgpmItem->GefEffectedSkillPlusLevel(pAgpdCharacter, pcsAgpdSkill->m_pcsTemplate->m_lID);
			}
		}

	}

	return (nModifiedSkillLevel+nSkillLevel);
}

INT32 AgcmUISkillDragonScion::_GetSummernerSkillPoint( INT32 nIndex )
{
	INT32						nModifiedSkillLevel		=	0;
	INT32						nSkillLevel				=	0;

	AgpdCharacter*				pAgpdCharacter			=	m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdEventSkillHighLevel		stHighLevelSkill		=	_GetSummernerSkill( nIndex );
	AgpdSkillTemplate*			pcsSkillTemplate		=	m_pcsAgpmSkill->GetSkillTemplate( stHighLevelSkill.m_lSkillTID );
	if( !pcsSkillTemplate )		return 0;

	AgpdSkill*					pcsAgpdSkill			=	m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	if( pcsAgpdSkill )
	{
		nSkillLevel		=	m_pcsAgpmSkill->GetSkillLevel( pcsAgpdSkill );

		// 열린창이 SKILL 창이라면 추가 스킬포인트가 있는지 확인
		if( m_nLastOpen == 1 )
		{
			if(	m_pcsAgpmEventSkillMaster->IsUpgradable((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsProductSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate) &&
				!m_pcsAgpmSkill->IsArchlordSkill((AgpdSkillTemplate*)pcsAgpdSkill->m_pcsTemplate))
			{
				nModifiedSkillLevel = m_pcsAgpmSkill->GetModifiedSkillLevel((ApBase*)pAgpdCharacter);
				if(pcsAgpdSkill->m_pcsTemplate)
					nModifiedSkillLevel += m_pcsAgpmItem->GefEffectedSkillPlusLevel(pAgpdCharacter, pcsAgpdSkill->m_pcsTemplate->m_lID);
			}
		}

	}

	return (nModifiedSkillLevel+nSkillLevel);
}


AgpdEventSkillHighLevel	AgcmUISkillDragonScion::_GetScionSkill( INT32 nIndex )
{
	AgpdCharacter*			pAgpdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SCION );
	if(!pVector)
		return AgpdEventSkillHighLevel();

	INT i = 0;
	HighLevelSkillIter iter = pVector->begin();
	while(iter != pVector->end() )
	{
		AgpdEventSkillHighLevel stHighLevel			= *iter;
		AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
		AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

		if( i	==	nIndex )
		{
			return stHighLevel;
		}

		++iter;		++i;
	}
	return AgpdEventSkillHighLevel();
}

AgpdEventSkillHighLevel	AgcmUISkillDragonScion::_GetSlayerSkill( INT32 nIndex )
{
	AgpdCharacter*			pAgpdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SLAYER );
	if(!pVector)
		return AgpdEventSkillHighLevel();

	INT i = 0;
	HighLevelSkillIter iter = pVector->begin();
	while(iter != pVector->end() )
	{
		AgpdEventSkillHighLevel stHighLevel			= *iter;
		AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
		AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

		if( i	==	nIndex )
		{
			return stHighLevel;
		}

		++iter;		++i;
	}
	return AgpdEventSkillHighLevel();
}

AgpdEventSkillHighLevel	AgcmUISkillDragonScion::_GetObiterSkill( INT32 nIndex )
{
	AgpdCharacter*			pAgpdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_OBITER );
	if(!pVector)
		return AgpdEventSkillHighLevel();

	INT i = 0;
	HighLevelSkillIter iter = pVector->begin();
	while(iter != pVector->end() )
	{
		AgpdEventSkillHighLevel stHighLevel			= *iter;
		AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
		AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

		if( i	==	nIndex )
		{
			return stHighLevel;
		}

		++iter;		++i;
	}
	return AgpdEventSkillHighLevel();
}

AgpdEventSkillHighLevel	AgcmUISkillDragonScion::_GetSummernerSkill( INT32 nIndex )
{
	AgpdCharacter*			pAgpdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( DRAGONSCION_SUMMERNER );
	if(!pVector)
		return AgpdEventSkillHighLevel();

	INT i = 0;
	HighLevelSkillIter iter = pVector->begin();
	while(iter != pVector->end())
	{
		AgpdEventSkillHighLevel stHighLevel			= *iter;
		AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
		AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

		if( i	==	nIndex )
		{
			return stHighLevel;
		}

		++iter;		++i;
	}
	return AgpdEventSkillHighLevel();
}

AgpdEventSkillHighLevel AgcmUISkillDragonScion::_GetPassiveSkill( INT32 nIndex )
{
	AgpdCharacter*			pAgpdCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	HighLevelSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHighLevelSkillVector( AGPMEVENT_HIGHLEVEL_PASSIVE_SCION );
	if(!pVector)
		return AgpdEventSkillHighLevel();

	INT i = 0;
	HighLevelSkillIter iter = pVector->begin();
	while(iter != pVector->end())
	{
		AgpdEventSkillHighLevel stHighLevel			= *iter;
		AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHighLevel.m_lSkillTID);
		AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHighLevel.m_lCharTID );

		if( i	==	nIndex )
		{
			return stHighLevel;
		}

		++iter;		++i;
	}
	return AgpdEventSkillHighLevel();
}

eDragonScionSKill_Status AgcmUISkillDragonScion::IsActiveUpgradeItem( INT32 lSkillTID )
{
	AgpdCharacter*		pAgpdCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();

	if(lSkillTID <= 0)
		return SKILL_STATUS_FAIL;

	AgpdSkillTemplate* pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(lSkillTID);
	if(!pcsSkillTemplate)
		return SKILL_STATUS_FAIL;

	if(!m_pcsAgpmEventSkillMaster->IsUpgradable(pcsSkillTemplate))
		return SKILL_STATUS_FAIL;

	// 스킬이 있는 지 확인한다.
	AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(pAgpdCharacter, pcsSkillTemplate->m_szName);
	if(!pcsSkill)
	{
		INT32 lBuyCost = m_pcsAgpmEventSkillMaster->GetBuyCost(pcsSkillTemplate, pAgpdCharacter);
		if(lBuyCost < 0)
			return SKILL_STATUS_FAIL;

		INT64 llMoney = 0;
		m_pcsAgpmCharacter->GetMoney(pAgpdCharacter, &llMoney);

		if(llMoney < (INT64)lBuyCost)
			return SKILL_STATUS_FAIL;

		AgpmEventSkillLearnResult eCheckResult = m_pcsAgpmEventSkillMaster->CheckLearnSkill(pAgpdCharacter, pcsSkillTemplate , _GetCharacterTID( m_eCurrentUpUI ) );
		if (eCheckResult != AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS)
			return SKILL_STATUS_FAIL;
	}
	else
	{		
		AgpdEventSkillHighLevel	stHighLevel		=	_GetSkill( m_eCurrentUpUI , lSkillTID );

		AgpmEventSkillUpgradeResult eCheckResult = m_pcsAgpmEventSkillMaster->CheckUpgradeSkill( pAgpdCharacter , pcsSkill );
		if( eCheckResult != AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS )
			return SKILL_STATUS_FAIL;

		// 다른종족 스킬이다
		if( pAgpdCharacter->m_lTID1 != stHighLevel.m_lCharTID	)
			return SKILL_STATUS_OTHER_CLASS;

		INT32 lUpgradeCost = m_pcsAgpmEventSkillMaster->GetUpgradeCost(pcsSkill, pAgpdCharacter);
		if(lUpgradeCost < 0)
			return SKILL_STATUS_FAIL;

		INT64 llMoney = 0;
		m_pcsAgpmCharacter->GetMoney(pAgpdCharacter, &llMoney);

		if(llMoney < (INT64)lUpgradeCost)
			return SKILL_STATUS_FAIL;

		INT32 lUpgradeCostSP = m_pcsAgpmEventSkillMaster->GetUpgradeCostSP(pcsSkill);
		if(lUpgradeCostSP < 0)
			return SKILL_STATUS_FAIL;

		INT32 lCharacterSP = m_pcsAgpmCharacter->GetSkillPoint(pAgpdCharacter);

		if(lCharacterSP < lUpgradeCostSP)
			return SKILL_STATUS_FAIL;

		INT32 lSkillLevel = m_pcsAgpmSkill->GetSkillLevel(pcsSkill);
		if(lSkillLevel < 1)
			return SKILL_STATUS_FAIL;

		INT32 lCharLevel = m_pcsAgpmCharacter->GetLevel(pAgpdCharacter);

		if(lCharLevel < ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1])
			return SKILL_STATUS_FAIL;
	}

	return SKILL_STATUS_SUCCESS;
}

BOOL		AgcmUISkillDragonScion::CheckRollbackSkill( INT	lSkillTID )
{
	if( !lSkillTID )
		return FALSE;

	AgpdCharacter*		pcsCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdSkillTemplate*	pcsSkillTemplate	=	m_pcsAgpmSkill->GetSkillTemplate( lSkillTID );
	AgpdSkill*			pcsSkill			=	NULL;

	if( !pcsSkillTemplate )
		return FALSE;

	pcsSkill	=	m_pcsAgpmSkill->GetSkill( pcsCharacter , pcsSkillTemplate->m_szName );

	// 변신을 안한 상태에서만 사용 가능
	if( m_pcsAgcmCharacter->GetClassTypeByTID( m_pcsAgcmCharacter->GetSelfCharacter()->m_pcsCharacterTemplate->m_lID) 
		!=	AUCHARCLASS_TYPE_SCION)
		return FALSE;

	// Condition 체크
	if( !m_pcsAgpmEventSkillMaster->CheckTargetSkillForRollback( pcsSkill ) )
		return FALSE;

	// 회귀의 오브가 있는지 체크
	if (!m_pcsAgpmItem->GetSkillRollbackScroll(m_pcsAgcmCharacter->GetSelfCharacter()) &&
		!m_pcsAgpmItem->GetCashSkillRollbackScroll(m_pcsAgcmCharacter->GetSelfCharacter()))
		return FALSE;

	return TRUE;
}


INT32		AgcmUISkillDragonScion::_GetSkillIndex( eDragonScionSkillUIType	eScionType , INT32 nTID	)
{
	AgpdEventSkillHighLevel (AgcmUISkillDragonScion::*GetSkillFunc		)(INT32);

	INT32	nSkillCount			=	0;
	switch( eScionType )
	{
	case SkillUI_Scion:			GetSkillFunc		=	&AgcmUISkillDragonScion::_GetScionSkill;	 nSkillCount = SKILL_SCION_COUNT;		break;
	case SkillUI_Slayer:		GetSkillFunc		=	&AgcmUISkillDragonScion::_GetSlayerSkill;	 nSkillCount = SKILL_SLAYER_COUNT;		break;
	case SkillUI_Obiter:		GetSkillFunc		=	&AgcmUISkillDragonScion::_GetObiterSkill;	 nSkillCount = SKILL_OBITER_COUNT;		break;
	case SkillUI_Summerner:		GetSkillFunc		=	&AgcmUISkillDragonScion::_GetSummernerSkill; nSkillCount = SKILL_SUMMENER_COUNT;	break;
	case SkillUI_Passive:		GetSkillFunc		=	&AgcmUISkillDragonScion::_GetPassiveSkill;	 nSkillCount = SKILL_PASSIVE_COUNT;		break;
	}
	


	for( INT i = 0 ; i < nSkillCount ; ++i )
	{
		AgpdEventSkillHighLevel	stHighLevel	=	(this->*GetSkillFunc)(i);

			if( stHighLevel.m_lSkillTID		== nTID )
				return i;
	}
	
	return -1;
}

AgpdEventSkillHighLevel	AgcmUISkillDragonScion::_GetSkill( eDragonScionSkillUIType eScionType , INT32 nTID )
{
	AgpdEventSkillHighLevel (AgcmUISkillDragonScion::*GetSkillFunc)(INT32);


	INT32	nSkillCount			=	0;
	switch( eScionType )
	{
	case SkillUI_Scion:			GetSkillFunc	=	&AgcmUISkillDragonScion::_GetScionSkill;	 nSkillCount = SKILL_SCION_COUNT;		break;
	case SkillUI_Slayer:		GetSkillFunc	=	&AgcmUISkillDragonScion::_GetSlayerSkill;	 nSkillCount = SKILL_SLAYER_COUNT;		break;
	case SkillUI_Obiter:		GetSkillFunc	=	&AgcmUISkillDragonScion::_GetObiterSkill;	 nSkillCount = SKILL_OBITER_COUNT;		break;
	case SkillUI_Summerner:		GetSkillFunc	=	&AgcmUISkillDragonScion::_GetSummernerSkill; nSkillCount = SKILL_SUMMENER_COUNT;	break;
	case SkillUI_Passive:		GetSkillFunc	=	&AgcmUISkillDragonScion::_GetPassiveSkill;	 nSkillCount = SKILL_PASSIVE_COUNT;		break;
	}

	for( INT i = 0 ; i < nSkillCount ; ++i )
	{
		AgpdEventSkillHighLevel	stHighLevel	=	(this->*GetSkillFunc)(i);

		if( stHighLevel.m_lSkillTID		== nTID )
			return stHighLevel;
	}

	return AgpdEventSkillHighLevel();
}

BOOL AgcmUISkillDragonScion::IsCurScionWindowOpen( VOID )
{

	AgcdUI*		pAgcdUI	=	NULL;

	pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_SCION		);	
	if( pAgcdUI->m_eStatus	==	AGCDUI_STATUS_OPENED )
		return TRUE;

	pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_OBITER		);
	if( pAgcdUI->m_eStatus	==	AGCDUI_STATUS_OPENED )
		return TRUE;

	pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_SLAYER		);
	if( pAgcdUI->m_eStatus	==	AGCDUI_STATUS_OPENED )
		return TRUE;

	pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_PASSIVE		);
	if( pAgcdUI->m_eStatus	==	AGCDUI_STATUS_OPENED )
		return TRUE;

	pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_SUMMERNER	);
	if( pAgcdUI->m_eStatus	==	AGCDUI_STATUS_OPENED )
		return TRUE;

	pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_ARCHLORD	);
	if( pAgcdUI->m_eStatus	==	AGCDUI_STATUS_OPENED )
		return TRUE;

	pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_HEROIC_SCION);
	if( pAgcdUI->m_eStatus	==	AGCDUI_STATUS_OPENED )
		return TRUE;

	return FALSE;
}

BOOL AgcmUISkillDragonScion::_MoveSkillWindow( eDragonScionSkillUIType eType )
{
	if( m_eLastUI	== SkillUI_NotOpened )		return FALSE;

	AgcdUIMode		eUIMode		=	m_pcsAgcmUIManager2->GetUIMode();
	AgcdUI*			pSrcUI		=	_GetUI( m_eLastUI	, FALSE );
	AgcdUI*			pDesUI		=	_GetUI( eType			, FALSE );


	pDesUI->m_pcsUIWindow->x	=	pSrcUI->m_pcsUIWindow->x;
	pDesUI->m_pcsUIWindow->y	=	pSrcUI->m_pcsUIWindow->y;

	return TRUE;
}

BOOL AgcmUISkillDragonScion::_MoveSkillUpgradeWindow( eDragonScionSkillUIType eType )
{
	if( m_eLastUpUI	== SkillUI_NotOpened )		return FALSE;

	AgcdUIMode		eUIMode		=	m_pcsAgcmUIManager2->GetUIMode();
	AgcdUI*			pSrcUI		=	_GetUI( m_eLastUpUI	, TRUE );
	AgcdUI*			pDesUI		=	_GetUI( eType			, TRUE );

	pDesUI->m_pcsUIWindow->x	=	pSrcUI->m_pcsUIWindow->x;
	pDesUI->m_pcsUIWindow->y	=	pSrcUI->m_pcsUIWindow->y;

	return TRUE;
}

AgcdUI* AgcmUISkillDragonScion::_GetUI( eDragonScionSkillUIType eType , BOOL bUpgradeUI /* = FALSE */ )
{
	AgcdUI*			pAgcdUI		=	NULL;

	if( bUpgradeUI	== FALSE )
	{
		switch( eType )
		{
		case SkillUI_Scion:			pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_SCION		);		break;
		case SkillUI_Obiter:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_OBITER		);		break;
		case SkillUI_Slayer:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_SLAYER		);		break;
		case SkillUI_Passive:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_PASSIVE		);		break;
		case SkillUI_Summerner:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_SUMMERNER	);		break;
		case SkillUI_Archlord:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_ARCHLORD	);		break;
		case SkillUI_Heroic:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_HEROIC_SCION);		break;
		}
	}

	else
	{
		switch( eType )
		{
		case SkillUI_Scion:			pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_UP_SCION		);		break;
		case SkillUI_Obiter:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_UP_OBITER		);		break;
		case SkillUI_Slayer:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_UP_SLAYER		);		break;
		case SkillUI_Passive:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_UP_PASSIVE		);		break;
		case SkillUI_Summerner:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_UP_SUMMERNER	);		break;
		case SkillUI_Heroic:		pAgcdUI		=	m_pcsAgcmUIManager2->GetUI( UI_NAME_UP_HEROIC_SCION );		break;
		}
	}

	return pAgcdUI;
}

INT32		AgcmUISkillDragonScion::_GetCharacterTID( eDragonScionSkillUIType eScionType )
{
	switch( eScionType )
	{
	case SkillUI_Scion:			return DRAGONSCION_SCION;
	case SkillUI_Slayer:		return DRAGONSCION_SLAYER;
	case SkillUI_Obiter:		return DRAGONSCION_OBITER;
	case SkillUI_Summerner:		return DRAGONSCION_SUMMERNER;
	case SkillUI_Passive:		return AGPMEVENT_HIGHLEVEL_PASSIVE_SCION;
	default:					return 0;
	}

}