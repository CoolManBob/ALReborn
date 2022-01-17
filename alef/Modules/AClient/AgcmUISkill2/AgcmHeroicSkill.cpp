#include "AgcmHeroicSkill.h"

#include "AgpmGrid.h"
#include "AgpmSkill.h"
#include "AgcmCharacter.h"
#include "AgcmUIManager2.h"
#include "AgpmEventSkillMaster.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgcmSkill.h"
#include "AgcmEventSkillMaster.h"
#include "AgcmUICharacter.h"
#include "AgcmUIMain.h"
#include "AgcmUIItem.h"
#include "AgcmUISkill2.h"


AgcmHeroicSkill::AgcmHeroicSkill( VOID )
{
	m_nSelectIndex			=	-1;
	m_nSelectIndexUpgrade	=	-1;
	m_nHeroicSkillPoint		=	0;
	m_pcsEvent				=	NULL;
}

AgcmHeroicSkill::~AgcmHeroicSkill( VOID )
{
	
}

BOOL	AgcmHeroicSkill::_InitGrid( VOID )
{
	CHAR			szHeroicUserDataName				[ MAX_PATH ];
	CHAR			szHeroicUserDataNameUpgrade			[ MAX_PATH ];
	CHAR			szHeroicDisplayName					[ MAX_PATH ];
	CHAR			szHeroicDisplayNameUpgrade			[ MAX_PATH ];

	for( INT i = 0 ; i < SKILL_HEROIC_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Init( &m_HeroicSkillGrid[i] , 1 , 1 , 1 );
		m_pcsAgpmGrid->Init( &m_HeroicSkillUpGrid[i] , 1 , 1 , 1 );

		m_HeroicSkillGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
		m_HeroicSkillUpGrid[i].m_lGridType	=	AGPDGRID_ITEM_TYPE_SKILL;
	}


	for( INT i = 0 ; i < SKILL_HEROIC_COUNT ; ++i )
	{
		sprintf_s( szHeroicUserDataName				, MAX_PATH , "SkillHeroicUserData_%d"	, i );
		sprintf_s( szHeroicUserDataNameUpgrade		, MAX_PATH , "SkillUpHeroicUserData_%d"	, i );
		sprintf_s( szHeroicDisplayName				, MAX_PATH , "Skill_Heroic_Display_%d" , i );
		sprintf_s( szHeroicDisplayNameUpgrade		, MAX_PATH , "Skill_Up_Heroic_Display_%d" , i );

		m_pHeroicDisplayUserData[i]	=	
			m_pcsAgcmUIManager2->AddUserData( szHeroicDisplayName , &m_nHeroicDisplayUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pHeroicDisplayUpUserData[i]	=	
			m_pcsAgcmUIManager2->AddUserData( szHeroicDisplayNameUpgrade , &m_nHeroicDisplayUpUserData[i] , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );

		m_pHeroicSkillUserData[i]		=
			m_pcsAgcmUIManager2->AddUserData( szHeroicUserDataName , &m_HeroicSkillGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );

		m_pHeroicSkillUpUserData[i]	=
			m_pcsAgcmUIManager2->AddUserData( szHeroicUserDataNameUpgrade , &m_HeroicSkillUpGrid[i] , sizeof(AgpdGrid) , 1 , AGCDUI_USERDATA_TYPE_GRID );
	}

	// Scion Skill Point
	m_pHeroicSkillPointUserData		= m_pcsAgcmUIManager2->AddUserData( "HeroicSkillpoint" , &m_nHeroicSkillPoint , sizeof(INT32) , 1 , AGCDUI_USERDATA_TYPE_INT32 );
	m_pHeroicActiveUpgradeButton	= m_pcsAgcmUIManager2->AddUserData( "Skill_HeroicActiveUpgradeButton",	&m_bHeroicActiveUpgradeButton,	sizeof(m_bHeroicActiveUpgradeButton),	1,	AGCDUI_USERDATA_TYPE_BOOL);
	m_pHeroicActiveRollbackButton	= m_pcsAgcmUIManager2->AddUserData( "Skill_HeroicActiveRollbackButton",	&m_bHeroicActiveRollbackButton,	sizeof(m_bHeroicActiveRollbackButton),	1,	AGCDUI_USERDATA_TYPE_BOOL);

	return TRUE;
}

BOOL	AgcmHeroicSkill::_InitEvent( VOID )
{

	CHAR	szEventName[ MAX_PATH ];

	// 히로익 스킬 이벤트 등록
	for( INT i = 0 ; i < SKILL_HEROIC_COUNT ; ++i )
	{	
		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_SelectedItem_%d" , i );
		m_nEventHeroicSelectedItem[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicSelectedItem[i] )				break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UnSelectedItem_%d" , i );
		m_nEventHeroicUnSelectedItem[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUnSelectedItem[i] )			break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UpgradeSelectedItem_%d" , i );
		m_nEventHeroicUpgradeSelectedItem[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUpgradeSelectedItem[i] )		break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UpgradeUnSelectedItem_%d" , i );
		m_nEventHeroicUpgradeUnSelectedItem[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUpgradeUnSelectedItem[i] )	break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_ActiveItem_%d" , i );
		m_nEventHeroicActiveItem[i]		=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicActiveItem[i] )				break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_DisableItem_%d" , i );
		m_nEventHeroicDisableItem[i]		=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicDisableItem[i] )				break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UpgradeActiveItem_%d" , i );
		m_nEventHeroicUpgradeActiveItem[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUpgradeActiveItem[i] )		break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UpgradeDisableItem_%d" , i );
		m_nEventHeroicUpgradeDisableItem[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUpgradeDisableItem[i] )		break;	


		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_SelectedItemOther_%d" , i );
		m_nEventHeroicSelectedItemOther[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicSelectedItemOther[i] )				break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UnSelectedItemOther_%d" , i );
		m_nEventHeroicUnSelectedItemOther[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUnSelectedItemOther[i] )			break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UpgradeSelectedItemOther_%d" , i );
		m_nEventHeroicUpgradeSelectedItemOther[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUpgradeSelectedItemOther[i] )		break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UpgradeUnSelectedItemOther_%d" , i );
		m_nEventHeroicUpgradeUnSelectedItemOther[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUpgradeUnSelectedItemOther[i] )	break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_ActiveItemOther_%d" , i );
		m_nEventHeroicActiveItemOther[i]		=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicActiveItemOther[i] )				break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_DisableItemOther_%d" , i );
		m_nEventHeroicDisableItemOther[i]		=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicDisableItemOther[i] )				break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UpgradeActiveItemOther_%d" , i );
		m_nEventHeroicUpgradeActiveItemOther[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUpgradeActiveItemOther[i] )		break;

		sprintf_s( szEventName , MAX_PATH , "Skill_Heroic_UpgradeDisableItemOther_%d" , i );
		m_nEventHeroicUpgradeDisableItemOther[i]	=	m_pcsAgcmUIManager2->AddEvent( szEventName );
		if( !m_nEventHeroicUpgradeDisableItemOther[i] )		break;	
	}

	m_nEventOpenUIHeroic		=	m_pcsAgcmUIManager2->AddEvent( "Skill_OpenUIHeroic" );
	m_nEventCloseUIHeroic		=	m_pcsAgcmUIManager2->AddEvent( "Skill_CloseUIHeroic" );
	m_nEventOpenUIHeroicUp		=	m_pcsAgcmUIManager2->AddEvent( "Skill_OpenUpUIHeroic" );
	m_nEventCloseUIHeroicUp		=	m_pcsAgcmUIManager2->AddEvent( "Skill_CloseUpUIHeroic" );

	m_nEventOpenUIHeroicOther	=	m_pcsAgcmUIManager2->AddEvent( "Skill_OpenUIHeroicOther" );
	m_nEventCloseUIHeroicOther	=	m_pcsAgcmUIManager2->AddEvent( "Skill_CloseUIHeroicOther" );
	m_nEventOpenUIHeroicUpOther	=	m_pcsAgcmUIManager2->AddEvent( "Skill_OpenUpUIHeroicOther" );
	m_nEventCloseUIHeroicUpOther=	m_pcsAgcmUIManager2->AddEvent( "Skill_CloseUpUIHeroicOther" );

	return TRUE;
}

BOOL	AgcmHeroicSkill::_InitDisplay( VOID )
{
	// 스킬창 Heroic
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_1_1" , SKILL_HEROIC_1_1 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_2_1" , SKILL_HEROIC_2_1 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_2_2" , SKILL_HEROIC_2_2 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_3_1" , SKILL_HEROIC_3_1 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_3_2" , SKILL_HEROIC_3_2 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_3_3" , SKILL_HEROIC_3_3 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_4_1" , SKILL_HEROIC_4_1 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_4_2" , SKILL_HEROIC_4_2 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_4_3" , SKILL_HEROIC_4_3 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillHeroic_5_1" , SKILL_HEROIC_5_1 , CBDisplaySkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;

	// 스킬창 Heroic Upgrade
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_1_1" , SKILL_HEROIC_1_1 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_2_1" , SKILL_HEROIC_2_1 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_2_2" , SKILL_HEROIC_2_2 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_3_1" , SKILL_HEROIC_3_1 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_3_2" , SKILL_HEROIC_3_2 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_3_3" , SKILL_HEROIC_3_3 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_4_1" , SKILL_HEROIC_4_1 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_4_2" , SKILL_HEROIC_4_2 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_4_3" , SKILL_HEROIC_4_3 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;
	if( !m_pcsAgcmUIManager2->AddDisplay( this , "SkillUpHeroic_5_1" , SKILL_HEROIC_5_1 , CBDisplayUpSkillHeroic , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;




	if( !m_pcsAgcmUIManager2->AddDisplay( this , "HeroicSkillpoint" , 0 , CBDisplayHeroicSkillPoint , AGCDUI_USERDATA_TYPE_INT32 ) )
		return FALSE;


	return TRUE;
}

BOOL	AgcmHeroicSkill::_InitFunction( VOID )
{
	m_pcsAgcmUIManager2->AddFunction( this , "Skill_SelectHeroicGrid"			,	CBSelectHeroicSkillGrid , 0 );
	m_pcsAgcmUIManager2->AddFunction( this , "Skill_SelectUpgradeHeroicGrid"	,	CBSelectUpgradeHeroicSkillGrid , 0 );
	m_pcsAgcmUIManager2->AddFunction( this , "Skill_Heroic_Skill_Upgrade"		,	CBHeroicSkillUpgrade	, 0 );
	m_pcsAgcmUIManager2->AddFunction( this , "Skill_Heroic_Skill_RollBack"		,	CBHeroicSkillRollBack	, 0 );
	m_pcsAgcmUIManager2->AddFunction( this , "Skill_Heroic_Open_Tooltip"		,	CBOpenUpgradeHeroicSkillToolTip, 0);

	return TRUE;
}

BOOL	AgcmHeroicSkill::Initialize( AgcmUISkill2*	pAgcmUISkill )
{
	
	if( !pAgcmUISkill )
		return FALSE;

	m_pcsAgcmUISkill2			= pAgcmUISkill;

	m_pcsAgpmGrid				= static_cast< AgpmGrid* >				( pAgcmUISkill->GetModule("AgpmGrid")				);
	m_pcsAgpmSkill				= static_cast< AgpmSkill* >				( pAgcmUISkill->GetModule("AgpmSkill")				);
	m_pcsAgcmCharacter			= static_cast< AgcmCharacter* >			( pAgcmUISkill->GetModule("AgcmCharacter")			);
	m_pcsAgcmUIManager2			= static_cast< AgcmUIManager2* >		( pAgcmUISkill->GetModule("AgcmUIManager2")			);
	m_pcsAgpmEventSkillMaster	= static_cast< AgpmEventSkillMaster* >	( pAgcmUISkill->GetModule("AgpmEventSkillMaster")	);
	m_pcsAgpmFactors			= static_cast< AgpmFactors* >			( pAgcmUISkill->GetModule("AgpmFactors")			);
	m_pcsAgpmCharacter			= static_cast< AgpmCharacter* >			( pAgcmUISkill->GetModule("AgpmCharacter")			);
	m_pcsAgpmItem				= static_cast< AgpmItem* >				( pAgcmUISkill->GetModule("AgpmItem")				);
	m_pcsAgcmSkill				= static_cast< AgcmSkill* >				( pAgcmUISkill->GetModule("AgcmSkill")				);
	m_pcsAgcmEventSkillMaster	= static_cast< AgcmEventSkillMaster* >	( pAgcmUISkill->GetModule("AgcmEventSkillMaster")	);
	m_pcsAgcmUICharacter		= static_cast< AgcmUICharacter* >		( pAgcmUISkill->GetModule("AgcmUICharacter")		);
	m_pcsAgcmUIMain				= static_cast< AgcmUIMain* >			( pAgcmUISkill->GetModule("AgcmUIMain")				);
	m_pcsAgcmUIItem				= static_cast< AgcmUIItem* >			( pAgcmUISkill->GetModule("AgcmUIItem")				);

	_InitGrid();
	_InitDisplay();
	_InitFunction();

	_InitEvent(  );

	if (!m_pcsAgpmEventSkillMaster->SetCallbackLearnResult(CBBuyHeroicSkillResult, this))
		return FALSE;
	if (!m_pcsAgpmEventSkillMaster->SetCallbackResponseUpgrade(CBUpgradeHeroicSkillResult, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackInitSkill( CBUpdateHeroicSkill , this ))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackUpdateSkillPoint( CBUpdateHeroicSkill , this))
		return FALSE;

	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorHp( CBUpdateHeroicSkill, this))
		return FALSE;
	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorMp( CBUpdateHeroicSkill, this))
		return FALSE;
	if (!m_pcsAgpmSkill->SetCallbackRemoveSkill( CBUpdateHeroicSkill, this) )
		return FALSE;

	if (!m_pcsAgpmFactors->SetCallbackUpdateFactorHeroicPoint( CBUpdateFactorHeroicPoint , this) )
		return FALSE;

	m_pcsEvent	=	pAgcmUISkill->GetApdEvent();

	return TRUE;
}

VOID	AgcmHeroicSkill::Destroy( VOID )
{

	for( INT i = 0 ; i < SKILL_HEROIC_COUNT ; ++i )
	{
		m_pcsAgcmUIManager2->DestroyUserData( m_pHeroicDisplayUserData[i] );
		m_pcsAgcmUIManager2->DestroyUserData( m_pHeroicDisplayUpUserData[i] );
		m_pcsAgcmUIManager2->DestroyUserData( m_pHeroicSkillUserData[i] );
		m_pcsAgcmUIManager2->DestroyUserData( m_pHeroicSkillUpUserData[i] );
	}

	for ( INT i = 0 ; i < SKILL_HEROIC_COUNT ; ++i )
	{
		m_pcsAgpmGrid->Remove( &m_HeroicSkillGrid[i]	);
		m_pcsAgpmGrid->Remove( &m_HeroicSkillUpGrid[i]	);
	}
}

BOOL	AgcmHeroicSkill::IsActiveUpgradeHeroic( INT32 lSkillTID )
{

	AgpdSkillTemplate*	pcsSkillTemplate	=	m_pcsAgpmSkill->GetSkillTemplate( lSkillTID );
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdCharacter*		pcsSelfCharacter	=	m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsSelfCharacter )
		return FALSE;

	AgpdSkill*					pcsAgpdSkill			=	m_pcsAgpmSkill->GetSkill( pcsSelfCharacter , pcsSkillTemplate->m_szName );

	if( !pcsAgpdSkill )
	{
		if( m_pcsAgpmEventSkillMaster->CheckLearnSkill( pcsSelfCharacter , pcsSkillTemplate ) != AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS )
			return FALSE;
	}
	else
	{
		if( m_pcsAgpmEventSkillMaster->CheckUpgradeSkill( pcsSelfCharacter , pcsAgpdSkill ) != AGPMEVENT_SKILL_LEARN_RESULT_SUCCESS )
			return FALSE;
	}

	return TRUE;
}

AgpdEventSkillHeroic	AgcmHeroicSkill::GetHeroicSkill( INT32 nIndex )
{
	AgpdCharacter*			pAgpdCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pAgpdCharacter )
		return AgpdEventSkillHeroic();

	HeroicSkillVector*		pVector			= m_pcsAgpmEventSkillMaster->GetHeroicSkillVector( pAgpdCharacter->m_pcsCharacterTemplate->GetID() );
	if(!pVector)
		return AgpdEventSkillHeroic();

	INT i = 0;
	HeroicSkillIter iter = pVector->begin();
	while( iter != pVector->end() )
	{
		AgpdEventSkillHeroic	stHeroic			= *iter;
		AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHeroic.m_lSkillTID);

		if( i	==	nIndex )
		{
			return stHeroic;
		}

		++iter;		++i;
	}
	return AgpdEventSkillHeroic();
}

INT32	AgcmHeroicSkill::GetHeroicSkillPoint( INT32 nIndex , BOOL bUpgrade /*= FALSE*/ )
{
	INT32						nModifiedSkillLevel		=	0;
	INT32						nSkillLevel				=	0;

	AgpdCharacter*				pAgpdCharacter			=	m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdEventSkillHeroic		stHeroicSkill			=	GetHeroicSkill( nIndex );
	AgpdSkillTemplate*			pcsSkillTemplate		=	m_pcsAgpmSkill->GetSkillTemplate( stHeroicSkill.m_lSkillTID );
	if( !pcsSkillTemplate )		
		return 0;

	// 일반 스킬창
	AgpdSkill*					pcsAgpdSkill			=	m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	if( pcsAgpdSkill )
		nSkillLevel		=	m_pcsAgpmSkill->GetSkillLevel( pcsAgpdSkill );

	if( bUpgrade )
	{
		// 업그레이드 스킬창
		if( !_IsScion( pAgpdCharacter ) )
		{
			if(	pAgpdCharacter->m_pcsCharacterTemplate->m_lID != m_pcsAgcmUISkill2->GetSelectCharacterTID()	)
			{
				nSkillLevel	=	0;	
			}
		}
	}
	
	return nSkillLevel;
}

VOID	AgcmHeroicSkill::LoadHeroicSkill( VOID )
{
	AgpdCharacter*		pAgpdCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pAgpdCharacter || !pAgpdCharacter->m_pcsCharacterTemplate)
		return;

	HeroicSkillVector* pVector = m_pcsAgpmEventSkillMaster->GetHeroicSkillVector( pAgpdCharacter->m_pcsCharacterTemplate->GetID() );
	if( !pVector )
	{
		for( INT i = 0 ; i < SKILL_HEROIC_COUNT ; ++i )
		{
			m_pcsAgpmGrid->Reset( &m_HeroicSkillGrid[i] );

			m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicDisableItem[i] );
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pHeroicSkillUserData[i]);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pHeroicDisplayUserData[i]);
		}
	}

	else
	{
		INT i = 0;
		HeroicSkillIter iter = pVector->begin();
		while( iter != pVector->end()  )
		{
			AgpdEventSkillHeroic	stHeroic			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHeroic.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHeroic.m_lCharTID );

			if( !pcsSkillTemplate )
			{
				++iter; ++i;
				continue;
			}

			m_pcsAgpmGrid->Reset( &m_HeroicSkillGrid[i] );

			// 일단 그리드 텍스쳐는 그려준다.
			m_pcsAgcmSkill->SetGridSkillAttachedTexture(pcsSkillTemplate);

			// 스킬을 배웠으면 잘 나오고 아니면 그레이로 나온다.
			AgpdSkill* pcsSkill = m_pcsAgpmSkill->GetSkill(pAgpdCharacter, pcsSkillTemplate->m_szName);
			if(pcsSkill && pcsSkill->m_pcsTemplate)
			{
				// Active 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicActiveItem[i] );
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicActiveItemOther[i] );

				if (m_pcsAgpmGrid->Add(& m_HeroicSkillGrid[i], 0, 0, 0, pcsSkill->m_pcsGridItem, 1, 1 ) )
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pHeroicSkillUserData[i]	);
					m_pcsAgcmUIManager2->SetUserDataRefresh( m_pHeroicDisplayUserData[i]	);
				}
			}
			else
			{
				// Disable 해준다.
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicDisableItem[i] );
				m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicDisableItemOther[i] );

				if (m_pcsAgpmGrid->Add( &m_HeroicSkillGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1 ))
				{
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pHeroicSkillUserData[i]);
					m_pcsAgcmUIManager2->SetUserDataRefresh(m_pHeroicDisplayUserData[i]);
				}
			}

			++iter; ++i;
		}
	}

	// 모조리 UnSelect 해준다.
	for(int i = 0; i < SKILL_HEROIC_COUNT; ++i)
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicUnSelectedItem[i] );
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicUnSelectedItemOther[i] );
	}

	if( m_nSelectIndex != -1 )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicSelectedItem[ m_nSelectIndex ] );
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicSelectedItemOther[ m_nSelectIndex ] );
	}

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pHeroicSkillPointUserData );
}

VOID	AgcmHeroicSkill::LoadHeroicSkillBuy( VOID )
{
	AgpdCharacter*		pAgpdCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();
	HeroicSkillVector*	pVector				=	NULL;
	if( !pAgpdCharacter || !pAgpdCharacter->m_pcsCharacterTemplate)
		return;

	if( _IsScion( pAgpdCharacter ) )
	{
		pVector		= m_pcsAgpmEventSkillMaster->GetHeroicSkillVector( pAgpdCharacter->m_pcsCharacterTemplate->GetID() );
	}

	else
	{
		pVector		= m_pcsAgpmEventSkillMaster->GetHeroicSkillVector( m_pcsAgcmUISkill2->GetSelectCharacterTID() );
	}

	if(!pVector)
	{
		for(int i = 0; i < SKILL_HEROIC_COUNT; ++i)
		{
			m_pcsAgpmGrid->Reset( &m_HeroicSkillUpGrid[i] );
			m_pcsAgcmUIManager2->ThrowEvent(m_nEventHeroicUpgradeDisableItem[i]);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pHeroicSkillUpUserData[i]	);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pHeroicDisplayUpUserData[i]);
		}
	}
	else
	{
		int i = 0;
		HeroicSkillIter iter = pVector->begin();
		while( iter != pVector->end() )
		{
			m_pcsAgpmGrid->Reset(&m_HeroicSkillUpGrid[i]);

			AgpdEventSkillHeroic	stHeroic			= *iter;
			AgpdSkillTemplate*		pcsSkillTemplate	= m_pcsAgpmSkill->GetSkillTemplate(stHeroic.m_lSkillTID);
			AuCharClassType			eClassType			= m_pcsAgcmCharacter->GetClassTypeByTID( stHeroic.m_lCharTID );
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
			if( IsActiveUpgradeHeroic(pcsSkillTemplate->m_lID ) )
			{
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventHeroicUpgradeActiveItem[i]);
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventHeroicUpgradeActiveItemOther[i]);
				m_pcsAgpmGrid->Add(&m_HeroicSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);
			}

			// 조건이 안 맞아 배울수 없는 스킬이다
			else
			{
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventHeroicUpgradeDisableItem[i]);
				m_pcsAgcmUIManager2->ThrowEvent(m_nEventHeroicUpgradeDisableItemOther[i]);
				m_pcsAgpmGrid->Add(&m_HeroicSkillUpGrid[i], 0, 0, 0, pcsSkillTemplate->m_pcsGridItem, 1, 1);

			}

			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pHeroicSkillUpUserData[i]	);
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pHeroicDisplayUpUserData[i]);

			++iter;	++i;
		}
	}

	// 모조리 UnSelect 해준다.
	UnSelectSkillHeroicUpgrade();

	if( m_nSelectIndex != -1 )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicUpgradeSelectedItem[ m_nSelectIndex ] );
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicUpgradeSelectedItemOther[ m_nSelectIndex ] );
	}

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pHeroicSkillPointUserData );
	//m_pAgcmUISkill2->RefreshInitButton();
}

VOID	AgcmHeroicSkill::UnSelectSkillHeroic( VOID )
{
	for( INT i = 0 ; i < SKILL_HEROIC_COUNT ; ++i )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicUnSelectedItem[i] );
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicUnSelectedItemOther[i] );
	}
}

VOID	AgcmHeroicSkill::UnSelectSkillHeroicUpgrade( VOID )
{
	for( INT i = 0 ; i < SKILL_HEROIC_COUNT ; ++i )
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicUpgradeUnSelectedItem[i] );
		m_pcsAgcmUIManager2->ThrowEvent( m_nEventHeroicUpgradeUnSelectedItemOther[i] );
	}
}

INT32	AgcmHeroicSkill::GetHeroicSkillIndex( INT32 nCharTID , INT32 nSkillTID )
{
	AgpdCharacter*	pCharacter		=	m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pCharacter )
		return 0;

	HeroicSkillVector*	pVector	= m_pcsAgpmEventSkillMaster->GetHeroicSkillVector( nCharTID );
	if( !pVector )
		return 0;

	HeroicSkillIter		Iter	=	pVector->begin();	

	for( INT i = 0 ; Iter != pVector->end() ; ++Iter , ++i )
	{
		if( Iter->m_lSkillTID	==	nSkillTID )
			return i;
	}

	return 0;
}

BOOL	AgcmHeroicSkill::_IsScion( AgpdCharacter* pcsCharacter )
{
	INT nTID		=	pcsCharacter->m_pcsCharacterTemplate->GetID();

	if( m_pcsAgcmUISkill2->GetSelectCharacterTID()	!=	DRAGONSCION_SCION )
		return FALSE;
	
	switch( nTID )
	{
	case DRAGONSCION_SCION:
	case DRAGONSCION_OBITER:
	case DRAGONSCION_SLAYER:
	case DRAGONSCION_SUMMERNER:
		return TRUE;
	}

	return FALSE;
}