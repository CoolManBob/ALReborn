// AgcmEffectConditionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "agcmeffectdlg.h"
#include "AgcmEffectConditionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgcmEffectConditionDlg dialog


AgcmEffectConditionDlg::AgcmEffectConditionDlg(AgcdUseEffectSet *pstEventEffect, AgcdUseEffectSetData *pstData, CWnd* pParent /*=NULL*/)
	: CDialog(AgcmEffectConditionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AgcmEffectConditionDlg)
/*	m_bCharAnimAttack		= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK							? TRUE : FALSE;
	m_bCharAnimDead			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD								? TRUE : FALSE;
	m_bCharAnimRun			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN								? TRUE : FALSE;
	m_bCharAnimStruck		= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK							? TRUE : FALSE;
	m_bCharAnimWait			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT								? TRUE : FALSE;
	m_bCharAnimWalk			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK								? TRUE : FALSE;
	m_bCharLogOn			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LOGON								? TRUE : FALSE;
	m_bCommonInit			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT									? TRUE : FALSE;
	m_bItemEquip			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP									? TRUE : FALSE;
	m_bObjectWork			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK									? TRUE : FALSE;
	m_bSkillTarget			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET									? TRUE : FALSE;
	m_bTargetStruck			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK							? TRUE : FALSE;
	m_bLevelUP				= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LEVELUP								? TRUE : FALSE;
	m_bSkillTargetStruck	= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET_STRUCK							? TRUE : FALSE;
	m_bSSAir				= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_AIR					? TRUE : FALSE;
	m_bSSAirStruck			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_AIR_STRUCK				? TRUE : FALSE;
	m_bSSEarth				= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_EARTH					? TRUE : FALSE;
	m_bSSEarthStruck		= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_EARTH_STRUCK			? TRUE : FALSE;
	m_bSSFire				= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_FIRE					? TRUE : FALSE;
	m_bSSFireStruck			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_FIRE_STRUCK			? TRUE : FALSE;
	m_bSSIce				= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_ICE					? TRUE : FALSE;
	m_bSSIceStruck			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_ICE_STRUCK				? TRUE : FALSE;
	m_bSSLightening			= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_LIGHTENING				? TRUE : FALSE;
	m_bSSLighteningStruck	= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_LIGHTENING_STRUCK		? TRUE : FALSE;
	m_bSSMagic				= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_MAGIC					? TRUE : FALSE;
	m_bSSMagicStruck		= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_MAGIC_STRUCK			? TRUE : FALSE;
	m_bSSPoison				= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_POISON					? TRUE : FALSE;
	m_bSSPoisonStruck		= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_POISON_STRUCK			? TRUE : FALSE;
	m_bSSWater				= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_WATER					? TRUE : FALSE;
	m_bSSWaterStruck		= pstData->m_ulConditionFlags & AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_WATER_STRUCK			? TRUE : FALSE;*/
	//}}AFX_DATA_INIT

	m_pstParentData			= pstEventEffect;
	m_pstData				= pstData;
}


void AgcmEffectConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AgcmEffectConditionDlg)
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_ATTACK, m_bCharAnimAttack);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_DEAD, m_bCharAnimDead);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_RUN, m_bCharAnimRun);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_STRUCK, m_bCharAnimStruck);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_WAIT, m_bCharAnimWait);
	DDX_Check(pDX, IDC_CHECK_CHAR_ANIM_WALK, m_bCharAnimWalk);
	DDX_Check(pDX, IDC_CHECK_CHARACTER_LOGON, m_bCharLogOn);
	DDX_Check(pDX, IDC_CHECK_COMMON_INIT, m_bCommonInit);
	DDX_Check(pDX, IDC_CHECK_ITEM_EQUIP, m_bItemEquip);
	DDX_Check(pDX, IDC_CHECK_OBJECT_WORK, m_bObjectWork);
	DDX_Check(pDX, IDC_CHECK_SKILL_TARGET, m_bSkillTarget);
	DDX_Check(pDX, IDC_CHECK_CHAR_TARGET_STRUCK, m_bTargetStruck);
	DDX_Check(pDX, IDC_CHECK_CHARACTER_LEVEL_UP, m_bLevelUP);
	DDX_Check(pDX, IDC_CHECK_SKILL_TARGET_TARGET, m_bSkillTargetStruck);
	DDX_Check(pDX, IDC_CHECK_SS_AIR, m_bSSAir);
	DDX_Check(pDX, IDC_CHECK_SS_AIR_TARGET, m_bSSAirStruck);
	DDX_Check(pDX, IDC_CHECK_SS_EARTH, m_bSSEarth);
	DDX_Check(pDX, IDC_CHECK_SS_EARTH_TARGET, m_bSSEarthStruck);
	DDX_Check(pDX, IDC_CHECK_SS_FIRE, m_bSSFire);
	DDX_Check(pDX, IDC_CHECK_SS_FIRE_TARGET, m_bSSFireStruck);
	DDX_Check(pDX, IDC_CHECK_SS_ICE, m_bSSIce);
	DDX_Check(pDX, IDC_CHECK_SS_ICE_TARGET, m_bSSIceStruck);
	DDX_Check(pDX, IDC_CHECK_SS_LIGHTENING, m_bSSLightening);
	DDX_Check(pDX, IDC_CHECK_SS_LIGHTENING_TARGET, m_bSSLighteningStruck);
	DDX_Check(pDX, IDC_CHECK_SS_MAGIC, m_bSSMagic);
	DDX_Check(pDX, IDC_CHECK_SS_MAGIC_TARGET, m_bSSMagicStruck);
	DDX_Check(pDX, IDC_CHECK_SS_POISON, m_bSSPoison);
	DDX_Check(pDX, IDC_CHECK_SS_POISON_TARGET, m_bSSPoisonStruck);
	DDX_Check(pDX, IDC_CHECK_SS_WATER, m_bSSWater);
	DDX_Check(pDX, IDC_CHECK_SS_WATER_TARGET, m_bSSWaterStruck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AgcmEffectConditionDlg, CDialog)
	//{{AFX_MSG_MAP(AgcmEffectConditionDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AgcmEffectConditionDlg message handlers

void AgcmEffectConditionDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

/*	if(m_bCharAnimAttack)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_ATTACK;
	}

	if(m_bCharAnimDead)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD;
	}
	else
	{	m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_DEAD;
	}

	if(m_bCharAnimRun)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_RUN;
	}

	if(m_bCharAnimStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_STRUCK;
	}

	if(m_bTargetStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_TARGET_STRUCK;
	}

	if(m_bCharAnimWait)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WAIT;
	}

	if(m_bCharAnimWalk)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_ANIM_WALK;
	}

	if(m_bCharLogOn)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LOGON;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LOGON;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LOGON;
	}

	if(m_bLevelUP)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LEVELUP;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LEVELUP;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_CHARACTER_OP_LEVELUP;
	}

	if(m_bCommonInit)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_COMMON_OP_INIT;
	}

	if(m_bItemEquip)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_OP_EQUIP;
	}

	if(m_bObjectWork)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_OBJECT_OP_WORK;
	}

	if(m_bSkillTarget)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET;
	}

	if(m_bSkillTargetStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_SKILL_OP_TARGET_STRUCK;
	}

	if(m_bSSAir)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_AIR;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_AIR;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_AIR;
	}

	if(m_bSSAirStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_AIR_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_AIR_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_AIR_STRUCK;
	}

	if(m_bSSEarth)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_EARTH;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_EARTH;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_EARTH;
	}

	if(m_bSSEarthStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_EARTH_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_EARTH_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_EARTH_STRUCK;
	}

	if(m_bSSFire)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_FIRE;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_FIRE;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_FIRE;
	}

	if(m_bSSFireStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_FIRE_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_FIRE_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_FIRE_STRUCK;
	}

	if(m_bSSIce)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_ICE;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_ICE;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_ICE;
	}

	if(m_bSSIceStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_ICE_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_ICE_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_ICE_STRUCK;
	}

	if(m_bSSLightening)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_LIGHTENING;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_LIGHTENING;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_LIGHTENING;
	}

	if(m_bSSLighteningStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_LIGHTENING_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_LIGHTENING_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_LIGHTENING_STRUCK;
	}

	if(m_bSSMagic)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_MAGIC;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_MAGIC;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_MAGIC;
	}

	if(m_bSSMagicStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_MAGIC_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_MAGIC_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_MAGIC_STRUCK;
	}

	if(m_bSSPoison)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_POISON;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_POISON;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_POISON;
	}

	if(m_bSSPoisonStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_POISON_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_POISON_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_POISON_STRUCK;
	}

	if(m_bSSWater)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_WATER;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_WATER;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_WATER;
	}

	if(m_bSSWaterStruck)
	{
		m_pstParentData->m_ulConditionFlags		|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_WATER_STRUCK;
		m_pstData->m_ulConditionFlags			|= AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_WATER_STRUCK;
	}
	else
	{
		m_pstData->m_ulConditionFlags			&= ~AGCDEVENTEFFECT_CONDITION_FLAG_ITEM_CONVERT_SPIRIT_STONE_WATER_STRUCK;
	}
*/
	CDialog::OnOK();
}
