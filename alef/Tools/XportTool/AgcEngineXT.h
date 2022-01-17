#pragma once

#include "AcuFrameMemory.h"
#include "AgcmUIControl.h"
#include "ApBase.h"
#include "ApModule.h"
#include "ApModuleManager.h"
#include "AgcmMap.h"
#include "AgpmFactors.h"
#include "AgpmGrid.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgcmItem.h" 
#include "AgpmUnion.h"
#include "AgpmSkill.h"
#include "AgpmParty.h"
#include "AgcmParty.h"
#include "AgcmSkill.h"
#include "AgpmEventSkillMaster.h"
#include "AgcmEff2.h"
#include "ApmEventManager.h"
#include "AgpmTimer.h"
#include "AgcmEventEffect.h"
#include "AgcmRender.h"
#include "AgcmSound.h"
#include "AgcmLODManager.h"
#include "AgcmFont.h"
#include "AgcmUVAnimation.h"
#include "AgcmPreLODManager.h"
#include "AgcmCustomizeRender.h"
#include "AgcmResourceLoader.h"
#include "AgcmLensFlare.h"
#include "AgcmTextBoardMng.h"

#include "AgcmEventManager.h"
#include "AgcmItemConvert.h"
#include "AgpmBillInfo.h"
#include "AgcmExportResource.h"
#include "AgpmArchlord.h"

#include "AgpmPVP.h"
#include "AgpmSummons.h"
#include "AgpmGuild.h"

class AgpmBattleGround;
class AgcEngineXT : public AgcEngine
{
public:
	AgcEngineXT();
	virtual	~AgcEngineXT();

	BOOL Initialize();
	BOOL Destroy();
	BOOL OnRegisterModule();

	AgpmConfig*			pAgpmConfig_;
	AgcmResourceLoader*	pAgcmResourceLoader_;
	ApmMap*				pApmMap_;
	ApmOcTree*			pApmOcTree_;
	AgcmOcTree*			pAgcmOcTree_;
	AgcmFont*			pAgcmFont_;
	AgpmFactors*		pAgpmFactors_;
	AgcmRender*			pAgcmRender_;
	AgcmLODManager*		pAgcmLODManager_;
	ApmObject*			pApmObject_;
	AgcmObject*			pAgcmObject_;
	AgpmCharacter*		pAgpmCharacter_;
	AgcmCharacter*		pAgcmCharacter_;
	AgpmGrid*			pAgpmGrid_;
	AgpmItem*			pAgpmItem_;
	AgcmItem*			pAgcmItem_;
	AgcmSound*			pAgcmSound_;
	AgcmEff2*			pAgcmEff2_;
	ApmEventManager*	pApmEventManager_;
	AgpmTimer*			pAgpmTimer_;
	AgpmUnion*			pAgpmUnion_;
	AgpmParty*			pAgpmParty_;
	AgcmParty*			pAgcmParty_;
	AgcmUIControl*		pAgcmUIControl_;
	AgpmEventSkillMaster* pAgpmEventSkillMaster_;
	AgpmSkill*			pAgpmSkill_;
	AgcmSkill*			pAgcmSkill_;
	AgpmItemConvert*	pAgpmItemConvert_;
	AgcmItemConvert*	pAgcmItemConvert_;
	AgcmEventEffect*	pAgcmEventEffect_;
	AgcmEventManager*	pAgcmEventManager_;
	AgcmUVAnimation*	pAgcmUVAnimation_;
	AgcmPreLODManager*	pAgcmPreLODManager_;
	AgcmCustomizeRender*	pAgcmCustomizeRender_;
	AgpmBillInfo*		pAgpmBillInfo_;
	AgpmArchlord*		pAgpmArchlord_;
	AgpmPvP*			pAgpmPvP_;
	AgpmBattleGround*	pAgpmBattleGround;
	AgpmSummons*		pAgpmSummons_;
	AgpmGuild*			pAgpmGuild_;
	AgpmSiegeWar*		pAgpmSiegeWar_;
};

extern ApServiceArea	g_eSelectedServiceArea	;
extern eExportTarget	g_eExportTarget			;
