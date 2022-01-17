#pragma once

#include "AgcmHeroicSkillDef.h"
#include "AgcdUIManager2.h"
#include "AgpmEventSkillMaster.h"

class AgpmGrid;
class AgpmSkill;			
class AgcmCharacter;
class AgcmUIManager2;
class AgpmEventSkillMaster;
class AgpmFactors;
class AgpmCharacter;
class AgpmItem;
class AgcmSkill;
class AgcmEventSkillMaster;
class AgcmUICharacter;
class AgcmUIMain;
class AgcmUIItem;
class AgcmUISkill2;

class AgcmHeroicSkill
{
public:	//	--	Public Constructor , Destructor
	AgcmHeroicSkill			( VOID );
	virtual ~AgcmHeroicSkill( VOID );

public: //	--	Public Function

	//	This skill is set by Race
	BOOL						Initialize						( AgcmUISkill2*	pAgcmUISkill );		
	VOID						Destroy							( VOID );

	//////////////////////////////////////////////////////////////////////////////////////
	//		-------------------		Static Function	Start Line	------------------		//
	// Initializes the selected Grid
	static BOOL					CBHeroicSkillRollBack			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl );

	// Upgrade the Selected Grid
	static BOOL					CBHeroicSkillUpgrade			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl );

	// When selecting Grid Processing
	static BOOL					CBSelectHeroicSkillGrid			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectUpgradeHeroicSkillGrid	( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase* pTarget, AgcdUIControl* pControl );

	// Displays the remaining amount of the Heroic Skill
	static BOOL					CBDisplayHeroicSkillPoint		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Grid points to the current Display
	static BOOL					CBDisplaySkillHeroic			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Upgrade Grid points to the current Display
	static BOOL					CBDisplayUpSkillHeroic			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Update
	static BOOL					CBBuyHeroicSkillResult			( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL					CBUpgradeHeroicSkillResult		( PVOID pData , PVOID pClass , PVOID pCustData );
	static BOOL					CBUpdateHeroicSkill				( PVOID pData , PVOID pClass , PVOID pCustData );

	// Factor Update
	static BOOL					CBUpdateFactorHeroicPoint		( PVOID pData , PVOID pClass , PVOID pCustData );

	// Tooltip
	static BOOL					CBOpenUpgradeHeroicSkillToolTip	( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pData3 , PVOID pData4 , PVOID pData5 , ApBase* pTarget, AgcdUIControl* pControl );

	//		-------------------		Static Function End Line	------------------		//
	//////////////////////////////////////////////////////////////////////////////////////

	// Upgrade can check the status
	BOOL						IsActiveUpgradeHeroic		( INT32	lSkillTID	);

	AgpdEventSkillHeroic		GetHeroicSkill				( INT32 nIndex );
	INT32						GetHeroicSkillPoint			( INT32 nIndex , BOOL bUpgrade = FALSE );

	VOID						LoadHeroicSkillBuy			( VOID );
	VOID						LoadHeroicSkill				( VOID );
	VOID						UnSelectSkillHeroic			( VOID );
	VOID						UnSelectSkillHeroicUpgrade	( VOID );

	// Set
	VOID						SetSelectIndex				( INT32 nIndex )	{	m_nSelectIndex			=	nIndex;	}
	VOID						SetSelectIndexUpgrade		( INT32 nIndex )	{	m_nSelectIndexUpgrade	=	nIndex;	}

	// Get
	// Scion Character Use
	INT32						GetHeroicOpenEvent			( VOID )	{	return m_nEventOpenUIHeroic;	}
	INT32						GetHeroicCloseEvent			( VOID )	{	return m_nEventCloseUIHeroic;	}
	INT32						GetHeroicUpOpenEvent		( VOID )	{	return m_nEventOpenUIHeroicUp;	}
	INT32						GetHeroicUpCloseEvent		( VOID )	{	return m_nEventCloseUIHeroicUp;	}

	// Other Character Use
	INT32						GetHeroicOpenEventOther		( VOID )	{	return m_nEventOpenUIHeroicOther;		}
	INT32						GetHeroicCloseEventOther	( VOID )	{	return m_nEventCloseUIHeroicOther;		}
	INT32						GetHeroicUpOpenEventOther	( VOID )	{	return m_nEventOpenUIHeroicUpOther;		}
	INT32						GetHeroicUpCloseEventOther	( VOID )	{	return m_nEventCloseUIHeroicUpOther;	}


	INT32						GetSelectIndex				( VOID )	{	return m_nSelectIndex;			}
	INT32						GetSelectIndexUpgrade		( VOID )	{	return m_nSelectIndexUpgrade;	}

	INT32						GetHeroicSkillIndex			( INT nCharTID , INT32 nSkillTID );

private:	//	-- Private Function
	BOOL						_InitGrid					( VOID );
	BOOL						_InitEvent					( VOID );
	BOOL						_InitDisplay				( VOID );
	BOOL						_InitFunction				( VOID );

	BOOL						_IsScion					( AgpdCharacter* pcsCharacter );
	

private:	//	-- Private Variable
	AgpdGrid											m_HeroicSkillGrid						[ SKILL_HEROIC_COUNT	];
	AgcdUIUserData*										m_pHeroicSkillUserData					[ SKILL_HEROIC_COUNT	];
	AgcdUIUserData*										m_pHeroicDisplayUserData				[ SKILL_HEROIC_COUNT	];
	INT32												m_nHeroicDisplayUserData				[ SKILL_HEROIC_COUNT	];
	AgpdGrid											m_HeroicSkillUpGrid						[ SKILL_HEROIC_COUNT	];
	AgcdUIUserData*										m_pHeroicSkillUpUserData				[ SKILL_HEROIC_COUNT	];
	AgcdUIUserData*										m_pHeroicDisplayUpUserData				[ SKILL_HEROIC_COUNT	];
	INT32												m_nHeroicDisplayUpUserData				[ SKILL_HEROIC_COUNT	];

	INT32												m_nEventHeroicSelectedItem				[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUnSelectedItem			[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUpgradeSelectedItem		[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUpgradeUnSelectedItem		[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicActiveItem				[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUpgradeActiveItem			[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicDisableItem				[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUpgradeDisableItem		[ SKILL_HEROIC_COUNT	];

	INT32												m_nEventHeroicSelectedItemOther			[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUnSelectedItemOther		[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUpgradeSelectedItemOther	[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUpgradeUnSelectedItemOther[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicActiveItemOther			[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUpgradeActiveItemOther	[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicDisableItemOther			[ SKILL_HEROIC_COUNT	];
	INT32												m_nEventHeroicUpgradeDisableItemOther	[ SKILL_HEROIC_COUNT	];


	INT32												m_nEventOpenUIHeroic;
	INT32												m_nEventOpenUIHeroicUp;
	INT32												m_nEventCloseUIHeroic;
	INT32												m_nEventCloseUIHeroicUp;

	INT32												m_nEventOpenUIHeroicOther;
	INT32												m_nEventOpenUIHeroicUpOther;
	INT32												m_nEventCloseUIHeroicOther;
	INT32												m_nEventCloseUIHeroicUpOther;


	INT32												m_nSelectIndex;
	INT32												m_nSelectIndexUpgrade;

	INT32												m_nHeroicSkillPoint;
	AgcdUIUserData*										m_pHeroicSkillPointUserData;
	AgcdUIUserData*										m_pHeroicActiveUpgradeButton;
	AgcdUIUserData*										m_pHeroicActiveRollbackButton;

	BOOL												m_bHeroicActiveUpgradeButton;
	BOOL												m_bHeroicActiveRollbackButton;

	AgpmGrid*											m_pcsAgpmGrid;				
	AgpmSkill*											m_pcsAgpmSkill;				
	AgcmCharacter*										m_pcsAgcmCharacter;			
	AgcmUIManager2*										m_pcsAgcmUIManager2;			
	AgpmEventSkillMaster*								m_pcsAgpmEventSkillMaster;	
	AgpmFactors*										m_pcsAgpmFactors;			
	AgpmCharacter*										m_pcsAgpmCharacter;			
	AgpmItem*											m_pcsAgpmItem;				
	AgcmSkill*											m_pcsAgcmSkill;				
	AgcmEventSkillMaster*								m_pcsAgcmEventSkillMaster;	
	AgcmUICharacter*									m_pcsAgcmUICharacter;		
	AgcmUIMain*											m_pcsAgcmUIMain;				
	AgcmUIItem*											m_pcsAgcmUIItem;		
	ApdEvent*											m_pcsEvent;
	AgcmUISkill2*										m_pcsAgcmUISkill2;
};
