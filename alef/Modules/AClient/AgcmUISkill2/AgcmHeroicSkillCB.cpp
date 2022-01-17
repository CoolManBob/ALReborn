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

BOOL	AgcmHeroicSkill::CBHeroicSkillRollBack( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmHeroicSkill*			pThis				=	static_cast< AgcmHeroicSkill* >(pClass);
	AgpdEventSkillHeroic		stHeroic			=	pThis->GetHeroicSkill( pThis->GetSelectIndex() );
	AgpdSkill*					pcsSkill			=	NULL;
	AgpdSkillTemplate*			pcsSkillTemplate	=	NULL;
	AgpdCharacter*				pcsSelfCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	CHAR						szUIMessage[ MAX_PATH ];

	// 있는 스킬인지 확인
	pcsSkill = pThis->m_pcsAgpmSkill->GetSkillByTID(pcsSelfCharacter, stHeroic.m_lSkillTID );
	if(!pcsSkill)		
		return FALSE;

	pcsSkillTemplate	=	(AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if( !pcsSkillTemplate )
		return FALSE;

	if( pcsSkill && pcsSkill->m_pcsTemplate )
	{
		// 롤백이 불가능
		if( !pThis->m_pcsAgpmEventSkillMaster->CheckHeroicSkillCondition( pcsSelfCharacter , pcsSkillTemplate ) )
			return FALSE;
	}

	// 회귀 오브가 없으면 실패
	if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll(pcsSelfCharacter) && !pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pcsSelfCharacter))
		return FALSE;

	CHAR*		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Init_Confirm_Message" );
	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsSkillTemplate->m_szName );

	INT			nAnswer			=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage );
	// OK 를 누르면 RollBack
	if( nAnswer )
	{
		pThis->SetSelectIndex( -1 );
		return pThis->m_pcsAgcmSkill->SendRequestRollback( pcsSkill->m_lID );
	}

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBHeroicSkillUpgrade( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	CHAR							szUIMessage	[ MAX_PATH ];
	AgcmHeroicSkill*				pThis			=	static_cast< AgcmHeroicSkill* >(pClass);
	AgpdCharacter*					pAgpdCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdSkillTemplate*				pcsSkillTemplate	=	NULL;
	AgpdSkill*						pcsSkill			=	NULL;


	if( pThis->GetSelectIndexUpgrade() == -1 )
		return FALSE;

	AgpdEventSkillHeroic	stHeroic	=	pThis->GetHeroicSkill( pThis->GetSelectIndexUpgrade() );
	pcsSkillTemplate = pThis->m_pcsAgpmSkill->GetSkillTemplate( stHeroic.m_lSkillTID );
	if( !pcsSkillTemplate )
		return FALSE;

	pcsSkill		=	pThis->m_pcsAgpmSkill->GetSkill( pAgpdCharacter , pcsSkillTemplate->m_szName );

	// 업그레이드가 불가능하면 그냥 종료한다
	if( !pThis->IsActiveUpgradeHeroic( stHeroic.m_lSkillTID ) )
		return FALSE;

	CHAR*	szConfirmMsg				=	NULL;
	if( pcsSkill )
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Upgrade_Confirm_Message" );
	else
		szConfirmMsg	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( "Skill_Buy_Confirm_Message" );

	if( !szConfirmMsg )
		return FALSE;

	sprintf_s( szUIMessage , MAX_PATH , szConfirmMsg , pcsSkillTemplate->m_szName );

	if( !pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szUIMessage ) )
		return FALSE;

	pThis->m_pcsEvent	=	pThis->m_pcsAgcmUISkill2->GetApdEvent();

	if(pcsSkill)
	{
		// 이미 배웠다면 업그레이드
		pThis->m_pcsAgcmEventSkillMaster->SendRequestSkillUpgrade(pThis->m_pcsEvent, pcsSkill->m_lID, 1);
	}
	else
	{
		// 처음 배우는 거
		pThis->m_pcsAgcmEventSkillMaster->SendLearnSkill(pThis->m_pcsEvent, pcsSkillTemplate->m_lID);
	}
	
	pThis->SetSelectIndexUpgrade( -1 );

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBSelectHeroicSkillGrid( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if (!pClass)
		return FALSE;

	AgcmHeroicSkill		*pThis				=	static_cast< AgcmHeroicSkill *	>(pClass);
	AcUIGrid			*pGrid				=	static_cast< AcUIGrid*			>(pControl->m_pcsBase);
	AgpdCharacter		*pCharacter			=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	INT32				nCharTID			=	0;
	AgpdSkillTemplate*	pSkillTemplate		=	NULL;


	// Scion이면 캐릭터가 변신해 있는 TID를 얻어온다
	//if( pThis->_IsScion( pCharacter ) )
	//	nCharTID	=	pCharacter->m_pcsCharacterTemplate->GetID();

	//// Scion아니면 선택된 NPC의 TID를 얻어온다
	//else
	nCharTID	=	pCharacter->m_pcsCharacterTemplate->GetID();


	// Grid에 세팅된 Skill TID를 얻어와서 선택해준다
	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;

		pThis->SetSelectIndex( pThis->GetHeroicSkillIndex( nCharTID , nSkillTID ) );
		pSkillTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( nSkillTID );
	}

	//	스킬 롤백이 가능한지 확인후 롤백버튼 활성/비활성 을 해준다
	pThis->m_bHeroicActiveRollbackButton	=	FALSE;
	if( pSkillTemplate )
	{
		// 롤백이 가능하다
		if( pThis->m_pcsAgpmEventSkillMaster->CheckHeroicSkillCondition( pCharacter , pSkillTemplate ) )
			pThis->m_bHeroicActiveRollbackButton	=	TRUE;

		// 롤백 불가능
		else
			pThis->m_bHeroicActiveRollbackButton	=	FALSE;
	}

	// 회귀의 오브가 있는지 확인한다
	if (!pThis->m_pcsAgpmItem->GetSkillRollbackScroll
		(pThis->m_pcsAgcmCharacter->GetSelfCharacter()) &&
		!pThis->m_pcsAgpmItem->GetCashSkillRollbackScroll(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
	{
		pThis->m_bHeroicActiveRollbackButton	=	FALSE;
	}

	// 갱신해준다
	pThis->UnSelectSkillHeroic();	
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_nEventHeroicSelectedItem[ pThis->GetSelectIndex() ] );
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_nEventHeroicSelectedItemOther[ pThis->GetSelectIndex() ] );
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pHeroicActiveRollbackButton );

	return TRUE;	
}

BOOL	AgcmHeroicSkill::CBSelectUpgradeHeroicSkillGrid( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass )
		return FALSE;

	AgcmHeroicSkill*	pThis		=	static_cast< AgcmHeroicSkill*	>(pClass);
	AcUIGrid*			pGrid		=	static_cast< AcUIGrid*			>(pControl->m_pcsBase);
	AgpdCharacter*		pCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	INT32				nCharTID	=	0;

	if( pThis->_IsScion( pCharacter ) )
	{
		nCharTID	=	pCharacter->m_pcsCharacterTemplate->GetID();
	}

	else
	{
		nCharTID	=	pThis->m_pcsAgcmUISkill2->GetSelectCharacterTID();
	}


	if( pGrid && pGrid->m_pAgpdGrid && pGrid->m_pAgpdGrid->m_ppcGridData )
	{
		INT32		nSkillTID		=	pGrid->m_pAgpdGrid->m_ppcGridData[0]->m_lItemTID;
		pThis->SetSelectIndexUpgrade( pThis->GetHeroicSkillIndex( nCharTID , nSkillTID ) );

		if( pThis->IsActiveUpgradeHeroic( nSkillTID ) )
		{
			pThis->m_bHeroicActiveUpgradeButton	=	TRUE;
		}
		else
		{
			pThis->m_bHeroicActiveUpgradeButton	=	FALSE;
		}

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pHeroicActiveUpgradeButton );

	}

	pThis->UnSelectSkillHeroicUpgrade();
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_nEventHeroicUpgradeSelectedItem[ pThis->GetSelectIndexUpgrade() ] );
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_nEventHeroicUpgradeSelectedItemOther[ pThis->GetSelectIndexUpgrade() ] );

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBDisplayHeroicSkillPoint( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{

	AgcmHeroicSkill*		pThis		=	static_cast< AgcmHeroicSkill* >(pClass);

	if( !pThis )	
		return FALSE;

	AgpdCharacter*			pCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	INT32					nSkillPoint		=	pThis->m_pcsAgpmCharacter->GetHeroicPoint( pCharacter );

	sprintf( szDisplay , "%d" , nSkillPoint );

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBDisplaySkillHeroic( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pClass || !pData | !szDisplay )
		return FALSE;

	AgcmHeroicSkill*	pThis			= static_cast< AgcmHeroicSkill* >(pClass);
	if(!pThis )
		return FALSE;

	sprintf( szDisplay , "%d" , pThis->GetHeroicSkillPoint( lID ) );

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBDisplayUpSkillHeroic( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue )
{
	if( !pClass || !pData | !szDisplay )
		return FALSE;

	AgcmHeroicSkill*	pThis			= static_cast< AgcmHeroicSkill* >(pClass);
	if(!pThis )
		return FALSE;

	sprintf( szDisplay , "%d" , pThis->GetHeroicSkillPoint( lID , TRUE ) );

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBBuyHeroicSkillResult( PVOID pData , PVOID pClass , PVOID pCustData )
{
	if( !pClass )
		return FALSE;

	AgcmHeroicSkill*	pThis		=	static_cast< AgcmHeroicSkill* >(pClass );

	pThis->LoadHeroicSkill();
	pThis->LoadHeroicSkillBuy();

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBUpgradeHeroicSkillResult( PVOID pData , PVOID pClass , PVOID pCustData )
{

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmHeroicSkill			*pThis			= static_cast< AgcmHeroicSkill *	>(pClass);
	PVOID					*ppvBuffer		= static_cast< PVOID *				>(pCustData);

	AgpmEventSkillUpgradeResult	eResult		= (AgpmEventSkillUpgradeResult) (INT32) ppvBuffer[2];
	INT32						lSkillID	= (INT32) ppvBuffer[1];

	switch (eResult) 
	{
	case AGPMEVENT_SKILL_UPGRADE_RESULT_SUCCESS:
		{
			pThis->LoadHeroicSkill();
			pThis->LoadHeroicSkillBuy();
		}
		break;
	}

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBUpdateHeroicSkill( PVOID pData , PVOID pClass , PVOID pCustData )
{
	AgcmHeroicSkill	*pThis		=	static_cast< AgcmHeroicSkill* >(pClass);

	pThis->LoadHeroicSkill();
	pThis->LoadHeroicSkillBuy();

	return TRUE;
}

BOOL	AgcmHeroicSkill::CBUpdateFactorHeroicPoint( PVOID pData , PVOID pClass , PVOID pCustData )
{
	if (!pData || !pClass)
		return FALSE;

	AgcmHeroicSkill			*pThis			=	static_cast< AgcmHeroicSkill *	>(pClass);
	AgpdFactor				*pcsFactor		=	static_cast< AgpdFactor *			>(pData);
	AgpdCharacter			*pcsCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	INT32					lOwnerID			= 0;

	if( !pcsCharacter )
		return TRUE;

	pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &lOwnerID, AGPD_FACTORS_TYPE_OWNER, AGPD_FACTORS_OWNER_TYPE_ID);

	if (lOwnerID == 0)
		return FALSE;

	if ( pcsCharacter->m_lID == lOwnerID)
	{
		pThis->m_pcsAgpmFactors->GetValue(pcsFactor, &pThis->m_nHeroicSkillPoint, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DIRT, AGPD_FACTORS_DIRT_TYPE_HEROIC_POINT);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pHeroicSkillPointUserData);
	}


	return TRUE;
}

BOOL AgcmHeroicSkill::CBOpenUpgradeHeroicSkillToolTip( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	if( !pClass )	
		return FALSE;

	AgcmUISkill2_TooltipType	eToolTipType;
	AgcmHeroicSkill*			pThis			=	static_cast< AgcmHeroicSkill* >(pClass);
	AcUIGrid*					pGrid			=	static_cast< AcUIGrid* >(pControl->m_pcsBase);
	AgpdCharacter*				pSelfCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	AgpdSkill*					pAgpdSkill		=	NULL;
	AgpdSkillTemplate*			pSkillTemplate	=	NULL;
	INT							nSkillLevel		=	0;
	
	if( !pGrid->m_pToolTipAgpdGridItem || pGrid->m_pToolTipAgpdGridItem->m_eType != AGPDGRID_ITEM_TYPE_SKILL || !pSelfCharacter )
		return FALSE;

	pSkillTemplate	=	pThis->m_pcsAgpmSkill->GetSkillTemplate( pGrid->m_pToolTipAgpdGridItem->m_lItemTID );
	if( !pSkillTemplate )
		return FALSE;

	pAgpdSkill		=	pThis->m_pcsAgpmSkill->GetSkill( pSelfCharacter , pSkillTemplate->m_szName );
	if( pAgpdSkill )
		nSkillLevel		=	pThis->m_pcsAgpmSkill->GetSkillLevel( pAgpdSkill );

	if( nSkillLevel )
		eToolTipType	=	AGCMUI_SKILL2_TOOLTIP_TYPE_UPGRADE;
	else
		eToolTipType	=	AGCMUI_SKILL2_TOOLTIP_TYPE_BUY;

	pThis->m_pcsAgcmUISkill2->OpenSkillToolTip( pControl , eToolTipType );
	return TRUE;
}