#pragma once

#include <AgcEngine.h>

#include "AgpmTimer.h"
#include "AgpmFactors.h"

#include "AgpmGrid.h"
#include "AgcModule.h"
#include "AgcmRender.h"
#include "AgcmFont.h"
#include "AgcmSound.h"
#include "AgcmUIControl.h"
#include "AgcmUIManager2.h"
#include "AgcmNatureEffect.h"

#include "AgcmUISample.h"

#include "ApmMap.h"
#include "AgcmMap.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgcmItem.h"
#include "AgpmItemConvert.h"
#include "AgcmItemConvert.h"
#include "AgcmPrivateTrade.h"
#include "AgcmConnectManager.h"
#include "AgcmLogin.h"
#include "AgcmAuction.h"
#include "AgcmRecruit.h"
#include "AgpmRide.h"
#include "AgcmCamera2.h"
#include "ApmEventManager.h"
#include "AgcmEventManager.h"
#include "AgpmEventNature.h"
#include "AgcmEventNature.h"
#include "AgcmEventEffect.h"
#include "AgcmEventNPCTrade.h"
#include "AgpmEventItemRepair.h"
#include "AgcmEventItemRepair.h"
#include "AgpmEventItemConvert.h"
#include "AgcmEventItemConvert.h"
#include "AgpmEventGuild.h"
#include "AgcmEventGuild.h"
#include "AgpmEventProduct.h"
#include "AgcmEventProduct.h"
#include "AgcmEventSkillMaster.h"
#include "AgpmEventQuest.h"
#include "AgcmEventQuest.h"
#include "AgpmEventNPCTrade.h"
#include "AgpmEventNPCDialog.h"
#include "AgcmEventNPCDialog.h"
#include "AgcmUVAnimation.h"
#include "AgcmEff2.h"
#include "AgcmSound.h"
#include "AgpmParty.h"
#include "AgcmParty.h"
#include "AgcmShadow.h"
#include "AgcmWater.h"
#include "AgcmSkill.h"
#include "AgcmLODManager.h"
#include "AgcmFont.h"
#include "AgpmChatting.h"
#include "AgpmAreaChatting.h"
#include "AgcmChatting2.h"
#include "AgcmUIControl.h"
#include "AgcmLensFlare.h"
#include "AgcmTextBoardMng.h"
#include "AgcmGlyph.h"
#include "AgcmTargeting.h"
#include "AgpmGuild.h"
#include "AgcmGuild.h"
#include "AgpmWorld.h"
#include "AgcmWorld.h"
#include "AgpmAuctionCategory.h"
#include "AgpmAuction.h"
#include "AgpmProduct.h"
#include "AgcmProduct.h"
#include "AgpmRefinery.h"
#include "AgcmRefinery.h"
#include "AgpmEventRefinery.h"
#include "AgcmEventRefinery.h"
#include "AgpmQuest.h"
#include "AgcmQuest.h"
#include "AgpmBuddy.h"
#include "AgpmChannel.h"
#include "AgpmStartupEncryption.h"
#include "AgcmUIMain.h"
#include "AgcmUICharacter.h"
#include "AgcmUIItem.h"
#include "AgcmUICooldown.h"
#include "AgcmUIEventItemRepair.h"
#include "AgcmUIEventTeleport.h"
#include "AgcmUISkill2.h"
#include "AgcmUILogin.h"
#include "AgcmUIEventBank.h"
#include "AgcmUIEventNPCDialog.h"
#include "AgcmUIChatting2.h"
#include "AgcmUIParty.h"
#include "AgcmUIPartyOption.h"
#include "AgcmUIOption.h"
#include "AgcmUIEventItemConvert.h"
#include "AgcmUIGuild.h"
#include "AgcmUISplitItem.h"
#include "AgcmUIRefinery.h"
#include "AgcmUIProduct.h"
#include "AgcmUIProductSkill.h"
#include "AgcmUIQuest2.h"
#include "AgcmUIPvP.h"
#include "AgcmUICustomizing.h"
#include "AgcmUIBuddy.h"
#include "AgcmUIChannel.h"

#include "AgcmUISearch.h"

#include "AgcmAuction.h"
#include "AgcmUIAuction.h"
#include "AgcmUITips.h"

#include "AgcmMinimap.h"
#include "AgpmRemission.h"
#include "AgcmUIRemission.h"
#include "AgpmMailBox.h"
#include "AgcmUIMailBox.h"

#include "MainWindow.h"

#include "AgcmCashMall.h"
#include "AgcmUICashMall.h"
#include "AgcmUICashInven.h"

#include "AgpmWantedCriminal.h"
#include "AgcmUIWantedCriminal.h"

#include "AgcmUIHelp.h"

#include "AgcmStartupEncryption.h"

#include "AgcmUISiegeWar.h"


#include "AgpmSearch.h"

#include "AgcmUIHotkey.h"
#include "AgcmUISocialAction.h"

#include "AgpmTax.h"
#include "AgcmUITax.h"

#include "AgpmGuildWarehouse.h"
#include "AgcmUIGuildWarehouse.h"

//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//#include "AgpmGamble.h"
//#include "AgcmUIGamble.h"

#include "AgpmArchlord.h"
#include "AgpmBattleGround.h"

#define GET_CONTROL_STYLE(control)	(((MyEngine *) g_pEngine)->GetControlData(control))
#define SAVE_CONTROL(control)		(((MyEngine *) g_pEngine)->SaveControlData(control))
#define LOAD_CONTROL(control)		(((MyEngine *) g_pEngine)->LoadControlData(control))

#define UIT_SOUND_PATH		"Sound\\UI\\"

extern AgcmResourceLoader	*g_pcsAgcmResourceLoader	;

extern MainWindow			g_MainWindow		;

extern AgpmTimer			*g_pcsAgpmTimer		;
extern AgpmGrid				*g_pcsAgpmGrid		;

extern AgcmUIControl		*g_pcsAgcmUIControl	;
extern AgcmUIManager2		*g_pcsAgcmUIManager2	;

extern AgcmUISample			*g_pcsAgcmUISample	;

extern AgpmFactors			*g_pcsAgpmFactors		;
extern AgpmCharacter		*g_pcsAgpmCharacter	;
extern AgcmCharacter		*g_pcsAgcmCharacter	;
extern AgpmUnion			*g_pcsAgpmUnion		;
extern AgpmItem				*g_pcsAgpmItem		;
extern AgcmItem				*g_pcsAgcmItem		;
extern AgpmItemConvert		*g_pcsAgpmItemConvert	;
extern AgcmItemConvert		*g_pcsAgcmItemConvert	;
extern AgcmPrivateTrade		*g_pcsAgcmPrivateTrade;
extern AgcmAuction			*g_pcsAgcmAuction		;
extern AgcmRecruit			*g_pcsAgcmRecruit		;
extern AgcmCamera2			*g_pcsCamera2			;
extern ApmEventManager		*g_pcsApmEventManager	;
extern AgpmEventNature		*g_pcsAgpmEventNature	;
extern AgcmEventNature		*g_pcsAgcmEventNature	;
extern AgpmEventItemRepair	*g_pcsAgpmEventItemRepair;
extern AgcmEventItemRepair	*g_pcsAgcmEventItemRepair;
extern AgpmEventTeleport	*g_pcsAgpmEventTeleport;
extern AgcmEventTeleport	*g_pcsAgcmEventTeleport;
extern AgpmEventNPCTrade	*g_pcsAgpmEventNPCTrade;
extern AgpmEventNPCDialog	*g_pcsAgpmEventNPCDialog;
extern AgcmEventNPCDialog	*g_pcsAgcmEventNPCDialog;
extern AgpmEventBank		*g_pcsAgpmEventBank		;
extern AgcmEventBank		*g_pcsAgcmEventBank		;
extern AgpmEventItemConvert	*g_pcsAgpmEventItemConvert;
extern AgcmEventItemConvert	*g_pcsAgcmEventItemConvert;
extern AgpmEventGuild		*g_pcsAgpmEventGuild;
extern AgcmEventGuild		*g_pcsAgcmEventGuild;
extern ApmObject			*g_pcsApmObject		;
extern AgcmObject			*g_pcsAgcmObject		;
extern AgcmMap				*g_pcsAgcmMap			;
extern AgcmEventEffect		*g_pcsAgcmEventEffect	;
extern AgcmEventNPCTrade	*g_pcsAgcmEventNPCTrade;
extern AgcmSound			*g_pcsAgcmSound		;
extern AgpmParty			*g_pcsAgpmParty		;
extern AgcmParty			*g_pcsAgcmParty		;
extern AgcmShadow			*g_pcsAgcmShadow		;
extern AgcmWater			*g_pcsAgcmWater		;
extern AgpmSkill			*g_pcsAgpmSkill		;
extern AgcmSkill			*g_pcsAgcmSkill		;
extern AgpmEventSkillMaster	*g_pcsAgpmEventSkillMaster	;
extern AgcmEventSkillMaster	*g_pcsAgcmEventSkillMaster	;
extern AgcmLODManager		*g_pcsAgcmLODManager	;
extern AgcmFont				*g_pcsAgcmFont		;
extern AgpmAreaChatting		*g_pcsAgpmAreaChatting	;
extern AgcmChatting2		*g_pcsAgcmChatting2	;
extern AgpmChatting			*g_pcsAgpmChatting	;
extern AgcmUIControl		m_cdAgcmUIControl	;
extern AgcmLensFlare		*g_pcsAgcmLensFlare	;
extern AgcmTextBoardMng		*g_pcsAgcmTextBoardMng	;
extern AgcmRender			*g_pcsAgcmRender		;
extern AgcmLogin			*g_pcsAgcmLogin		;
extern AgcmGlyph			*g_pcsAgcmGlyph		;
extern AgcmTargeting		*g_pcsAgcmTargeting	;
extern AgpmGuild			*g_pcsAgpmGuild;
extern AgcmGuild			*g_pcsAgcmGuild;
extern AgpmProduct			*g_pcsAgpmProduct;
extern AgcmProduct			*g_pcsAgcmProduct;
extern AgpmRefinery			*g_pcsAgpmRefinery;
extern AgcmRefinery			*g_pcsAgcmRefinery;
extern AgpmEventRefinery	*g_pcsAgpmEventRefinery;
extern AgcmEventRefinery	*g_pcsAgcmEventRefinery;
extern AgpmWorld			*g_pcsAgpmWorld;
extern AgcmWorld			*g_pcsAgcmWorld;
extern AgpmAuctionCategory	*g_pcsAgcmAuctionCategory;
extern AgpmAuction			*g_pcsAgpmAuction;
extern AgpmChannel			*g_pcsAgpmChannel;
extern AgpmGuildWarehouse	*g_pcsAgpmGuildWarehouse;
//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//extern AgpmGamble			*g_pcsAgpmGamble;

extern AgcmUIMain			*g_pcsAgcmUIMain		;
extern AgcmUICharacter		*g_pcsAgcmUICharacter	;
extern AgcmUIItem			*g_pcsAgcmUIItem		;
extern AgcmUICooldown		*g_pcsAgcmUICooldown	;
extern AgcmUIEventItemRepair	*g_pcsAgcmUIEventItemRepair	;
extern AgcmUIEventTeleport	*g_pcsAgcmUIEventTeleport	;
extern AgcmUISkill2			*g_pcsAgcmUISkill2		;
extern AgcmUILogin			*g_pcsAgcmUILogin		;
extern AgcmUIChatting2		*g_pcsAgcmUIChatting2		;
extern AgcmUIParty			*g_pcsAgcmUIParty		;
extern AgcmUIPartyOption			*g_pcsAgcmUIPartyOption		;
extern AgcmUIOption			*g_pcsAgcmUIOption	;
extern AgcmUIEventNPCDialog	*g_pcsAgcmUIEventNPCDialog	;
extern AgcmUIEventBank		*g_pcsAgcmUIEventBank	;
extern AgcmUIEventItemConvert	*g_pcsAgcmUIEventItemConvert	;
extern AgcmUIGuild			*g_pcsAgcmUIGuild;
extern AgcmUISplitItem		*g_pcsAgcmUISplitItem;
extern AgcmUIRefinery		*g_pcsAgcmUIRefinery;
extern AgcmUIProduct		*g_pcsAgcmUIProduct;
extern AgcmUIProductSkill	*g_pcsAgcmUIProductSkill;

extern AgpmPvP				*g_pcsAgpmPvP;
extern AgcmUIPvP			*g_pcsAgcmUIPvP;

extern ApmOcTree			*g_pcsApmOcTree;
extern AgcmOcTree			*g_pcsAgcmOcTree;

extern AgcmAuction			*g_pcsAgcmAuction;
extern AgcmUIAuction		*g_pcsAgcmUIAuction;
extern AgcmMinimap			*g_pcsAgcmMinimap		;
extern AgcmUITips			*g_pcsAgcmUITips;

extern AgcmUISearch			*g_pcsAgcmUISearch;
extern AgpmSearch			*g_pcsAgpmSearch;


extern AgcmCustomizeRender	*g_pcsAgcmCustomizeRender;

extern AgpmEventCharCustomize	*g_pcsAgpmEventCharCustomize;
extern AgcmEventCharCustomize	*g_pcsAgcmEventCharCustomize;
extern AgcmUICustomizing		*g_pcsAgcmUICustomizing;

extern AgpmBillInfo			*g_pcsAgpmBillInfo;
extern AgcmNatureEffect		*g_pcsAgcmNatureEffect;	//	2005.07.18. By SungHoon

extern AgpmCashMall			*g_pcsAgpmCashMall;
extern AgcmCashMall			*g_pcsAgcmCashMall;
extern AgcmUICashMall		*g_pcsAgcmUICashMall;
extern AgcmUICashInven		*g_pcsAgcmUICashInven;
extern AgcmUIBuddy			*g_pcsAgcmUIBuddy;
extern AgcmUIChannel		*g_pcsAgcmUIChannel;

extern AgpmReturnToLogin	*g_pcsAgpmReturnToLogin;
extern AgcmReturnToLogin	*g_pcsAgcmReturnToLogin;

extern AgpmWantedCriminal	*g_pcsAgpmWantedCriminal;
extern AgcmUIWantedCriminal	*g_pcsAgcmUIWantedCriminal;

extern AgcmUIHelp			*g_pcsAgcmUIHelp;

extern AgpmStartupEncryption* g_pcsAgpmStartupEncryption;
extern AgcmStartupEncryption* g_pcsAgcmStartupEncryption;

extern AgpmSiegeWar			*g_pcsAgpmSiegeWar;
extern AgpmEventSiegeWarNPC	*g_pcsAgpmEventSiegeWarNPC;
extern AgcmSiegeWar			*g_pcsAgcmSiegeWar;
extern AgcmEventSiegeWarNPC	*g_pcsAgcmEventSiegeWarNPC;
extern AgcmUISiegeWar		*g_pcsAgcmUISiegeWar;
extern AgcmUIGuildWarehouse *g_pcsAgcmUIGuildWarehouse;
//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//extern AgcmUIGamble			*g_pcsAgcmUIGamble;
extern AgcmUIHotkey			*g_pcsAgcmUIHotkey;
extern AgcmUISocialAction	*g_pcsAgcmUISocialAction;

extern AgpmTax				*g_pcsAgpmTax;
extern AgcmUITax			*g_pcsAgcmUITax;

extern AgpmBattleGround		*g_pcsAgpmBattleGround;

typedef struct
{
	AgcWindowProperty	m_stWinProperty;
	AgcdUIBaseProperty	m_stBaseProperty;
	INT32				m_lListItemVisibleRow;
} UITControlData;

class MyEngine : public AgcEngine  
{
private:
	INT16				m_nControlADIndex		;
public:
	MyEngine();
	virtual ~MyEngine();

	// Message
	BOOL				OnInit					();
	BOOL				OnAttachPlugins			();	// 플러그인 연결 은 여기서
	RpWorld *			OnCreateWorld			(); // 월드를 생성해서 리턴해준다.
	RwCamera *			OnCreateCamera			( RpWorld * pWorld );	// 메인 카메라를 Create해준다.
	BOOL				OnCreateLight			();	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
	AgcWindow *			OnSetMainWindow			();	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.

	BOOL				OnRegisterModule		();

	VOID				OnWorldRender			(); // World Render 부분에서 AgcmRender를 사용한다.

	VOID				OnTerminate				();

	VOID				OnEndIdle				();

	UITControlData *	GetControlData			(AgcdUIControl *pcsControl)
	{ return (UITControlData *) g_pcsAgcmUIManager2->GetAttachedModuleData(m_nControlADIndex, pcsControl);	}

	VOID				SaveControlData			(AgcdUIControl *pcsControl);
	VOID				LoadControlData			(AgcdUIControl *pcsControl);
	
	virtual BOOL				OnCameraResize			( RwRect * pRect );
	
	virtual BOOL		OnPostInitialize		();
};

extern MyEngine			g_MyEngine;
