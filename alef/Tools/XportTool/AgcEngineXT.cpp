#include "AgcEngineXT.h"
#include "AgcdEffGlobal.h"
#include "AgpmBattleGround.h"

#define MAX_OBJECT 10000

AgpmGuild		*	g_pcsAgpmGuild		= NULL;
AgpmSiegeWar	*	g_pcsAgpmSiegeWar	= NULL;

AgcEngineXT::AgcEngineXT() : pAgpmArchlord_( NULL )
{
	pAgcmResourceLoader_	= NULL;
	pApmMap_				= NULL;
	pApmOcTree_				= NULL;
	pAgcmOcTree_			= NULL;
	pAgcmFont_				= NULL;
	pAgpmFactors_			= NULL;
	pAgcmRender_			= NULL;
	pAgcmLODManager_		= NULL;
	pApmObject_				= NULL;
	pAgcmObject_			= NULL;
	pAgpmCharacter_			= NULL;
	pAgcmCharacter_			= NULL;
	pAgpmGrid_				= NULL;
	pAgpmItem_				= NULL;
	pAgcmItem_				= NULL;
	pAgcmSound_				= NULL;
	pAgcmEff2_				= NULL;
	pApmEventManager_		= NULL;
	pAgpmTimer_				= NULL;
	pAgpmUnion_				= NULL;
	pAgpmParty_				= NULL;
	pAgcmParty_				= NULL;
	pAgcmUIControl_			= NULL;
	pAgpmSkill_				= NULL;
	pAgcmSkill_				= NULL;
	pAgpmItemConvert_		= NULL;
	pAgcmItemConvert_		= NULL;
	pAgcmEventEffect_		= NULL;
	pAgcmEventManager_		= NULL;
	pAgcmUVAnimation_		= NULL;
	pAgcmPreLODManager_		= NULL;
	pAgcmCustomizeRender_	= NULL;
	pAgpmBillInfo_			= NULL;
}

// -----------------------------------------------------------------------------
AgcEngineXT::~AgcEngineXT()
{
}

BOOL AgcEngineXT::Initialize()
{
	if(pAgcmResourceLoader_)
	{
		pAgcmResourceLoader_->m_bForceImmediate = TRUE;
	}

	if(pAgcmRender_)
	{
		AgcdEffGlobal::bGetInst().bFlagOff(E_GFLAG_USEPACK);
	}

	if(pAgcmRender_)
	{
		//pAgcmRender_->SetMainFrame(RwCameraGetFrame(m_csRenderWare.GetCamera()));
	}

	if(pApmObject_)
	{
		pApmObject_->SetMaxObject(MAX_OBJECT);
		pApmObject_->SetMaxObjectTemplate(MAX_OBJECT);
	}

	if(pAgpmCharacter_)
	{
		pAgpmCharacter_->SetMaxCharacter(MAX_OBJECT);
		pAgpmCharacter_->SetMaxCharacterTemplate(MAX_OBJECT);
	}

	if(pAgpmItem_)
	{
		pAgpmItem_->SetMaxItem(MAX_OBJECT);
	}

	if(pAgpmSkill_)
	{
		pAgpmSkill_->SetMaxSkill(MAX_OBJECT);
		pAgpmSkill_->SetMaxSkillTemplate(MAX_OBJECT);
	}

	if(pAgcmCharacter_)
	{
		pAgcmCharacter_->SetMaxAnimations(20000);
		pAgcmCharacter_->SetAnimationPath(".\\Character\\Animation\\");
		//pAgcmCharacter_->SetWorld(m_csRenderWare.GetWorld(), m_csRenderWare.GetCamera());
	}

	if(pAgcmObject_)
	{
		pAgcmObject_->SetMaxAnimation(MAX_OBJECT);
		pAgcmObject_->SetAnimationPath(".\\Object\\Animation\\");
		//pAgcmObject_->SetRpWorld(m_csRenderWare.GetWorld());
	}

	if(pAgcmItem_)
	{
		//pAgcmItem_->SetWorld(m_csRenderWare.GetWorld());
		pAgcmItem_->SetMaxItemClump(MAX_OBJECT);
	}

	if(pApmEventManager_)
	{
		pApmEventManager_->SetMaxEvent(MAX_OBJECT);
	}

	if(pAgcmSound_)
	{
		//pAgcmSound_->m_bLoadPackInfo = FALSE;
		pAgcmSound_->SetReadFromPack( FALSE ); 
	}

	return AgcEngine::Initialize();
}

BOOL AgcEngineXT::Destroy()
{
	if(AuPackingManager::GetSingletonPtr())
		delete AuPackingManager::GetSingletonPtr();

	return AgcEngine::Destroy();
}

BOOL AgcEngineXT::OnRegisterModule()
{
	REGISTER_MODULE(pAgpmConfig_		, AgpmConfig			)
	REGISTER_MODULE(pAgcmResourceLoader_, AgcmResourceLoader);
	REGISTER_MODULE(pApmMap_, ApmMap);
	REGISTER_MODULE(pApmOcTree_, ApmOcTree);
	REGISTER_MODULE(pAgcmOcTree_, AgcmOcTree);
	REGISTER_MODULE(pAgcmFont_, AgcmFont);
	REGISTER_MODULE(pAgpmFactors_, AgpmFactors);
	REGISTER_MODULE_SINGLETON(AgcmMap);
	REGISTER_MODULE(pAgcmRender_, AgcmRender);
	REGISTER_MODULE(pAgcmLODManager_, AgcmLODManager);
	REGISTER_MODULE(pApmObject_, ApmObject);
	REGISTER_MODULE(pAgcmObject_, AgcmObject);
	REGISTER_MODULE(pAgpmCharacter_, AgpmCharacter);

	REGISTER_MODULE(pAgpmArchlord_, AgpmArchlord);
	REGISTER_MODULE(pAgcmCharacter_, AgcmCharacter);
	REGISTER_MODULE(pAgpmUnion_, AgpmUnion);
	REGISTER_MODULE(pAgpmParty_, AgpmParty);
	REGISTER_MODULE(pAgpmGrid_, AgpmGrid);
	REGISTER_MODULE(pAgpmBillInfo_, AgpmBillInfo);
	REGISTER_MODULE(pAgpmItem_, AgpmItem);
	REGISTER_MODULE(pApmEventManager_, ApmEventManager);
	REGISTER_MODULE(pAgpmEventSkillMaster_,AgpmEventSkillMaster);
	REGISTER_MODULE(pAgpmSkill_, AgpmSkill);
	REGISTER_MODULE(pAgpmSummons_, AgpmSummons);

	REGISTER_MODULE(pAgpmGuild_ , AgpmGuild );
	REGISTER_MODULE(pAgpmBattleGround, AgpmBattleGround);
	REGISTER_MODULE(pAgpmPvP_ , AgpmPvP );

	REGISTER_MODULE(pAgcmItem_, AgcmItem);
	REGISTER_MODULE(pAgcmSound_, AgcmSound);
	REGISTER_MODULE(pAgcmUVAnimation_, AgcmUVAnimation);
	REGISTER_MODULE(pAgcmEff2_, AgcmEff2);

	REGISTER_MODULE(pAgpmTimer_, AgpmTimer);
	REGISTER_MODULE(pAgcmParty_, AgcmParty);
	REGISTER_MODULE(pAgcmUIControl_, AgcmUIControl);
	REGISTER_MODULE(pAgcmSkill_, AgcmSkill);
	REGISTER_MODULE(pAgpmItemConvert_, AgpmItemConvert);
	REGISTER_MODULE(pAgcmItemConvert_, AgcmItemConvert);
	REGISTER_MODULE(pAgcmEventEffect_, AgcmEventEffect);
	REGISTER_MODULE(pAgcmEventManager_, AgcmEventManager);
	
	REGISTER_MODULE(pAgcmCustomizeRender_, AgcmCustomizeRender);
	REGISTER_MODULE(pAgcmPreLODManager_, AgcmPreLODManager);

	return TRUE;
}
