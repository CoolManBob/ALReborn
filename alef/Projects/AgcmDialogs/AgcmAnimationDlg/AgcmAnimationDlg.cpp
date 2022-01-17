#include "stdafx.h"
#include "../resource.h"
#include "AgcmAnimationDlg.h"
#include "AnimFlagsDlg.h"
#include "CharacterAnimationDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

static AgcmAnimationDlg*			g_pcsAgcmAnimationDlgInst	= NULL;
static CCharacterAnimationDlg*		g_pcsCharacterAnimationDlg	= NULL;

static CHAR* g_aszAnimationTypeName[AGCMCHAR_MAX_ANIM_TYPE] =
{
	"[ Wait ]",
	"[ Walk ]",
	"[ Run ]",
	"[ Attack ]",
	"[ Struck ]",
	"[ Dead ]",	
	"[ Customize Preview ]",
	"[ LeftSide Step ]",
	"[ RightSide Step ]",
	"[ Backward Step ]",
	"[ Pickup ]",
	"[ Eat ]",
	"[ Summon Conveyance ]",
	"[ Embark ]",
	"[ Ride ]",
	"[ Abnormal ]",
};

#define D_CAD_DEFAULT_ANIMATION_TYPE2_NAME					"[ Default ]"
#define D_CAD_WAIT_ANIMATION_TYPE2_NAME						"[ Wait ]"

static CHAR* g_aszBaseAnimationType2Name[AGCMCHAR_AT2_BASE_NUM] =
{
	D_CAD_DEFAULT_ANIMATION_TYPE2_NAME
};

static CHAR* g_aszWarriorAnimationType2Name[AGCMCHAR_AT2_WARRIR_NUM] =
{
	D_CAD_DEFAULT_ANIMATION_TYPE2_NAME,
	"[ OneSlash ]",
	"[ OneBlunt ]",
	"[ TwoSlash ]",
	"[ TwoBlunt ]",
	"[ Polearm ]",
	"[ Scythe ]",
	"[ Default_RideWar ]",
	"[ Weapon_RideWar ]",
	"[ Standard_RideWar ]",
	"[ OneCharonWar ]",
	"[ TwoCharonWar ]",
};	

static CHAR* g_aszArcherAnimationType2Name[AGCMCHAR_AT2_ARCHER_NUM] =
{
	D_CAD_DEFAULT_ANIMATION_TYPE2_NAME,
	"[ Bow ]",
	"[ Crossbow ]",
	"[ Glave ]",
	"[ Default_RideHun ]",
	"[ Weapon_RideHun ]",
	"[ Katariya ]",
	"[ Standard_RideHun ]",
	"[ OneZennonHun ]",
	"[ TwoZennonHun ]",
	"[ OneCharonHun ]",
	"[ TwoCharonHun ]",
};

static CHAR* g_aszWizardAnimationType2Name[AGCMCHAR_AT2_WIZARD_NUM] =
{
	D_CAD_DEFAULT_ANIMATION_TYPE2_NAME,
	"[ Staff ]",
	"[ Trophy ]",
	"[ Hoop ]",
	"[ Default_RideWiz ]",
	"[ Weapon_RideWiz ]",
	"[ Chakram ]",
	"[ Standard_RideWiz ]",
	"[ OneCharonWiz ]",
	"[ TwoCharonWiz ]",
};

static CHAR* g_aszArchlordAnimationType2Name[AGCMCHAR_AT2_ARCHLORD_NUM] =
{
	D_CAD_DEFAULT_ANIMATION_TYPE2_NAME,		
	"[ Default_RideArchlord ]",
	"[ Standard_RideArchlord ]",
};

CHAR** AgcmAnimationDlg::GetAnimTypeName()
{
	return g_aszAnimationTypeName;
}

CHAR** AgcmAnimationDlg::GetAnimType2Name(INT32 lAnimType2)
{
	switch (lAnimType2)
	{
	case AGCMCHAR_AT2_BASE:
	case AGCMCHAR_AT2_BOSS_MONSTER:
		return g_aszBaseAnimationType2Name;
	case AGCMCHAR_AT2_WARRIOR:
		return g_aszWarriorAnimationType2Name;
	case AGCMCHAR_AT2_ARCHER:
		return g_aszArcherAnimationType2Name;
	case AGCMCHAR_AT2_WIZARD:
		return g_aszWizardAnimationType2Name;
	case AGCMCHAR_AT2_ARCHLORD:
		return g_aszArchlordAnimationType2Name;
	}

	return NULL;
}

INT32 AgcmAnimationDlg::GetAnimType2Index(CString strCmp)
{
	if( strCmp == D_CAD_DEFAULT_ANIMATION_TYPE2_NAME )						return AGCMCHAR_AT2_COMMON_DEFAULT;

	for( int i = 0; i < AGCMCHAR_AT2_WARRIR_NUM; ++i )
		if( strCmp == g_aszWarriorAnimationType2Name[i] )
			return i;

	for( int i = 0; i < AGCMCHAR_AT2_ARCHER_NUM; ++i )
		if( strCmp == g_aszArcherAnimationType2Name[i] )
			return i;

	for( int i = 0; i < AGCMCHAR_AT2_WIZARD_NUM; ++i )
		if( strCmp == g_aszWizardAnimationType2Name[i] )
			return i;

	return -1;

	//else if( strCmp == D_CAD_ONE_SLASH_ANIMATION_TYPE2_NAME )				return AGCMCHAR_AT2_WARRIR_ONE_HAND_SLASH;
	//else if( strCmp == D_CAD_ONE_BLUNT_ANIMATION_TYPE2_NAME )				return AGCMCHAR_AT2_WARRIR_ONE_HAND_BLUNT;
	//else if( strCmp == D_CAD_TWO_SLASH_ANIMATION_TYPE2_NAME )				return AGCMCHAR_AT2_WARRIR_TWO_HAND_SLASH;
	//else if( strCmp == D_CAD_TWO_BLUNT_ANIMATION_TYPE2_NAME )				return AGCMCHAR_AT2_WARRIR_TWO_HAND_BLUNT;
	//else if( strCmp == D_CAD_POLEARM_ANIMATION_TYPE2_NAME )					return AGCMCHAR_AT2_WARRIR_ONE_HAND_POLEARM;
	//else if( strCmp == D_CAD_SCYTHE_ANIMATION_TYPE2_NAME )					return AGCMCHAR_AT2_WARRIR_ONE_HAND_SCYTHE;
	//else if( strCmp == D_CAD_DEFAULT_RIDE_WAR_ANIMATION_TYPE2_NAME )		return AGCMCHAR_AT2_WARRIR_DEFAULT_RIDE;
	//else if( strCmp == D_CAD_WEAPON_RIDE_WAR_ANIMATION_TYPE2_NAME )			return AGCMCHAR_AT2_WARRIR_WEAPON_RIDE;
	//else if( strCmp == D_CAD_STANDARD_RIDE_WAR_ANIMATION_TYPE2_NAME )		return AGCMCHAR_AT2_WARRIR_STANDARD_RIDE;
	//else if( strCmp == D_CAD_ONE_CHARON_WAR_ANIMATION_TYPE2_NAME )			return AGCMCHAR_AT2_WARRIR_ONE_HAND_CHARON;
	//else if( strCmp == D_CAD_ONE_CHARON_WAR_ANIMATION_TYPE2_NAME )			return AGCMCHAR_AT2_WARRIR_ONE_HAND_CHARON;
	//
	//else if( strCmp == D_CAD_DEFAULT_RIDE_HUN_ANIMATION_TYPE2_NAME )		return AGCMCHAR_AT2_ARCHER_DEFAULT_RIDE;
	//else if( strCmp == D_CAD_WEAPON_RIDE_HUN_ANIMATION_TYPE2_NAME )			return AGCMCHAR_AT2_ARCHER_WEAPON_RIDE;	
	//else if( strCmp == D_CAD_BOW_ANIMATION_TYPE2_NAME )						return AGCMCHAR_AT2_ARCHER_TWO_HAND_BOW;
	//else if( strCmp == D_CAD_CROSSBOW_ANIMATION_TYPE2_NAME )				return AGCMCHAR_AT2_ARCHER_TWO_HAND_CROSSBOW;
	//else if( strCmp == D_CAD_STANDARD_RIDE_HUN_ANIMATION_TYPE2_NAME )		return AGCMCHAR_AT2_ARCHER_STANDARD_RIDE;
	//else if( strCmp == D_CAD_KATARIYA_ANIMATION_TYPE2_NAME )				return AGCMCHAR_AT2_ARCHER_TWO_HAND_KATARIYA;
	//else if( strCmp == D_CAD_ONE_ZENNON_HUN_ANIMATION_TYPE2_NAME )			return AGCMCHAR_AT2_ARCHER_ONE_HAND_ZENNON;

	//else if( strCmp == D_CAD_STAFF_ANIMATION_TYPE2_NAME )					return AGCMCHAR_AT2_WIZARD_STAFF;
	//else if( strCmp == D_CAD_TROPHY_ANIMATION_TYPE2_NAME )					return AGCMCHAR_AT2_WIZARD_TROPHY;
	//else if( strCmp == D_CAD_DEFAULT_RIDE_WIZ_ANIMATION_TYPE2_NAME )		return AGCMCHAR_AT2_WIZARD_DEFAULT_RIDE;
	//else if( strCmp == D_CAD_WEAPON_RIDE_WIZ_ANIMATION_TYPE2_NAME )			return AGCMCHAR_AT2_WIZARD_WEAPON_RIDE;
	//else if( strCmp == D_CAD_CHAKRAM_ANIMATION_TYPE2_NAME )					return AGCMCHAR_AT2_WIZARD_TWO_HAND_CHAKRAM;
	//else if( strCmp == D_CAD_STANDARD_RIDE_WIZ_ANIMATION_TYPE2_NAME )		return AGCMCHAR_AT2_WIZARD_STANDARD_RIDE;
	//else if( strCmp == D_CAD_CHARON_WIZ_ANIMATION_TYPE2_NAME )				return AGCMCHAR_AT2_WIZARD_CHARON;

	//else if( strCmp == D_CAD_DEFAULT_RIDE_ARCHLORD_ANIMATION_TYPE2_NAME )	return AGCMCHAR_AT2_ARCHLORD_DEFAULT_RIDE;
	//else if( strCmp == D_CAD_STANDARD_RIDE_ARCHLORD_ANIMATION_TYPE2_NAME )	return AGCMCHAR_AT2_ARCHLORD_STANDARD_RIDE;

	//return -1;
}

INT32 AgcmAnimationDlg::GetAnimTypeIndex(CString strCmp)
{
	for (INT32 lIndex = 0; lIndex < AGCMCHAR_MAX_ANIM_TYPE; ++lIndex)
		if (!strcmp( g_aszAnimationTypeName[lIndex], strCmp ))
			return lIndex;

	return -1;
}

AgcmAnimationDlg* AgcmAnimationDlg::GetInstance()
{
	return g_pcsAgcmAnimationDlgInst;
}

AgcmAnimationDlg::AgcmAnimationDlg()
{
	SetModuleName("AgcmAnimationDlg");

	g_pcsAgcmAnimationDlgInst	= this;

	m_pcsAgcmCharacter			= NULL;
	m_pcsAgcmItem				= NULL;
	m_pcsAgcmEventEffect		= NULL;
}

AgcmAnimationDlg::~AgcmAnimationDlg()
{
}

BOOL AgcmAnimationDlg::OpenAnimFlags( AgcdAnimationFlag *pstFlags, ACA_AttachedData *pcsACA )
{
	if (!pstFlags)
		return FALSE;

	CAnimFlagsDlg csFlags(pstFlags, pcsACA);
	csFlags.DoModal();

	return TRUE;
}

BOOL AgcmAnimationDlg::OnInit()
{
	return TRUE;
}

BOOL AgcmAnimationDlg::OnAddModule()
{
	m_pcsAgcmCharacter		= (AgcmCharacter *)(GetModule("AgcmCharacter"));
	m_pcsAgcmItem			= (AgcmItem *)(GetModule("AgcmItem"));
	m_pcsAgcmEventEffect	= (AgcmEventEffect *)(GetModule("AgcmEventEffect"));

	return TRUE;
}

BOOL AgcmAnimationDlg::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgcmAnimationDlg::OnDestroy()
{
	return TRUE;
}

BOOL AgcmAnimationDlg::OpenCharAnim(AgcdCharacter *pcsAgcdCharacter, AgcdCharacterTemplate *pcsAgcdCharacterTemplate)
{
	if( g_pcsCharacterAnimationDlg )		return TRUE;

	g_pcsCharacterAnimationDlg	= new CCharacterAnimationDlg( pcsAgcdCharacterTemplate );
	if ( !g_pcsCharacterAnimationDlg )		return FALSE;

	m_pcdCharacter = pcsAgcdCharacter;
	g_pcsCharacterAnimationDlg->Create(pcsAgcdCharacterTemplate);
	g_pcsCharacterAnimationDlg->ShowWindow(SW_SHOW);

	return TRUE;
}

BOOL AgcmAnimationDlg::CloseCharAnim()
{	
	if (g_pcsCharacterAnimationDlg)
	{
		g_pcsCharacterAnimationDlg->ShowWindow(SW_HIDE);
		g_pcsCharacterAnimationDlg->DestroyWindow();

		delete g_pcsCharacterAnimationDlg;
		g_pcsCharacterAnimationDlg	= NULL;
	}

	return TRUE;
}

BOOL AgcmAnimationDlg::SetCallbackCharStartAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_START_ANIMATION, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::SetCallbackCharGetAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_GET_ANIMATION, pfCallback, pClass);
}

VOID AgcmAnimationDlg::SetFindCharAnimSoundPath(CHAR *szPath)
{
	strcpy(m_szFindCharAnimSoundPath, szPath);
}

VOID AgcmAnimationDlg::SetFindCharAnimPath(CHAR *szPath1, CHAR *szPath2)
{
	strcpy(m_szFindCharAnimPath1, szPath1);
	strcpy(m_szFindCharAnimPath2, szPath2 ? szPath2 : "" );
}

VOID AgcmAnimationDlg::SetCharAnimPath(CHAR *szPath)
{
	strcpy(m_szCharAnimPath, szPath);
}

BOOL AgcmAnimationDlg::IsOpenCharAnim()
{
	return g_pcsCharacterAnimationDlg ? TRUE: FALSE;
}

BOOL AgcmAnimationDlg::SetCallbackCloseCharAnimDlg(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_CLOSE_CHAR_ANIM_DLG, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::SetCallbackReadRtAnim(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_READ_RT_ANIM, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::SetCallbackAddAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_ADD_ANIMATION, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::SetCallbackAddAnimData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_ADD_ANIM_DATA, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::SetCallbackRemoveAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_REMOVE_ANIMATION, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::SetCallbackRemoveAllAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_REMOVE_ALL_ANIMATION, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::SetCallbackRemoveAnimData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_REMOVE_ANIM_DATA, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::SetCallbackStartAnimation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMANIMATIONDLG_CB_ID_START_ANIM, pfCallback, pClass);
}

BOOL AgcmAnimationDlg::ReadRtAnim(AAD_ReadRtAnimParams *pcsParams)
{
	return EnumCallback(AGCMANIMATIONDLG_CB_ID_READ_RT_ANIM, (PVOID)(pcsParams), NULL);
}

BOOL AgcmAnimationDlg::AddAnimation(AAD_AddAnimationParams *pcsParams)
{
	return EnumCallback(AGCMANIMATIONDLG_CB_ID_ADD_ANIMATION, (PVOID)(pcsParams), NULL);
}

BOOL AgcmAnimationDlg::AddAnimData(AAD_AddAnimDataParams *pcsParams)
{
	return EnumCallback(AGCMANIMATIONDLG_CB_ID_ADD_ANIM_DATA, (PVOID)(pcsParams), NULL);
}

BOOL AgcmAnimationDlg::RemoveAnimData(AAD_RemoveAnimDataParams	*pcsParams)
{
	return EnumCallback(AGCMANIMATIONDLG_CB_ID_REMOVE_ANIM_DATA, (PVOID)(pcsParams), NULL);
}

BOOL AgcmAnimationDlg::RemoveAnimation(AAD_RemoveAnimationParams *pcsParams)
{
	return EnumCallback(AGCMANIMATIONDLG_CB_ID_REMOVE_ANIMATION, (PVOID)(pcsParams), NULL);
}

BOOL AgcmAnimationDlg::RemoveAllAnimation(AAD_RemoveAllAnimationParams *pcsParams)
{
	return EnumCallback(AGCMANIMATIONDLG_CB_ID_REMOVE_ALL_ANIMATION, (PVOID)(pcsParams), NULL);
}

BOOL AgcmAnimationDlg::StartAnimation(AAD_StartAnimationParams *pcsParams)
{
	return EnumCallback(AGCMANIMATIONDLG_CB_ID_START_ANIM, (PVOID)(pcsParams), NULL);
}
