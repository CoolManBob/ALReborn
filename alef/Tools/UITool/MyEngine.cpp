// MyEngine.cpp: implementation of the MyEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rtcharse.h"
#include "rpcollis.h"
#include "rpusrdat.h"
#include "rpspline.h"
#include "rtanim.h"
#include "rplodatm.h"
#include "rpmatfx.h"
#include "rpmorph.h"

#include "AcuRpMtexture.h"

#include <skeleton.h>
#include <menu.h>
#include <camera.h>

#include "MyEngine.h"

#include "time.h"

// Image Loader
#include "rtbmp.h"
#include "rtpng.h"
 
#include "MainWindow.h"
#include <win.h>

#include "ApMemoryTracker.h"
#include "AuPackingManager.h"

#include "AuStrTable.h"

//@{ kday 20050120
#include "AgcdEffGlobal.h"
//@} kday

#include "AgpmEventGacha.h"
#include "AgcmEventGacha.h"
#include "AgcmUIEventGacha.h"

MainWindow			g_MainWindow		;
MyEngine			g_MyEngine			;

AgcmResourceLoader	*g_pcsAgcmResourceLoader	;

AgpmTimer			*g_pcsAgpmTimer			;
AgpmGrid			*g_pcsAgpmGrid			;

AgcmUIControl		*g_pcsAgcmUIControl		;
AgcmUIManager2		*g_pcsAgcmUIManager2		;

AgcmUISample		*g_pcsAgcmUISample		;

AgpmFactors			*g_pcsAgpmFactors			;

ApmMap				*g_pcsApmMap				;

ApmObject			*g_pcsApmObject			;
AgcmObject			*g_pcsAgcmObject			;

AgpmCharacter		*g_pcsAgpmCharacter		;
AgcmCharacter		*g_pcsAgcmCharacter		;

AgpmUnion			*g_pcsAgpmUnion			;

AgpmItem			*g_pcsAgpmItem			;
AgcmItem			*g_pcsAgcmItem			;

AgpmItemConvert		*g_pcsAgpmItemConvert	;
AgcmItemConvert		*g_pcsAgcmItemConvert	;

AgpmCombat			*g_pcsAgpmCombat		;

AgcmPrivateTrade	*g_pcsAgcmPrivateTrade	;

AcuObject			g_csAcuObject			;

AgcmConnectManager	*g_pcsAgcmConnectManager	;
AgpmLogin			*g_pcsAgpmLogin;
AgcmLogin			*g_pcsAgcmLogin;

AgcmAuction			*g_pcsAgcmAuction			;
AgcmRecruit			*g_pcsAgcmRecruit			;

//AgcmSystemInfo		*g_pcsAgcmSystemInfo		;

//@{ kday 20050113
//AgcmCamera		*g_pcsCamera				;
AgcmCamera2			*g_pcsCamera2				;
//@} kday
AgcmRender			*g_pcsAgcmRender			;

ApmEventManager		*g_pcsApmEventManager		;
AgcmEventManager	*g_pcsAgcmEventManager	;

AgpmEventNature		*g_pcsAgpmEventNature		;
AgcmEventNature		*g_pcsAgcmEventNature		;

AgcmEventNPCTrade	*g_pcsAgcmEventNPCTrade	;

AgpmRide			*g_pcsAgpmRide;

//AgpmEventMasterySpecialize	*g_pcsAgpmEventMasterySpecialize;
//AgcmEventMasterySpecialize	*g_pcsAgcmEventMasterySpecialize;

AgpmEventItemRepair	*g_pcsAgpmEventItemRepair;
AgcmEventItemRepair	*g_pcsAgcmEventItemRepair;

AgpmEventTeleport	*g_pcsAgpmEventTeleport	;
AgcmEventTeleport	*g_pcsAgcmEventTeleport	;

AgpmEventNPCTrade	*g_pcsAgpmEventNPCTrade;

AgpmEventNPCDialog	*g_pcsAgpmEventNPCDialog;
AgcmEventNPCDialog	*g_pcsAgcmEventNPCDialog;

AgpmEventBank		*g_pcsAgpmEventBank		;
AgcmEventBank		*g_pcsAgcmEventBank		;

AgpmEventItemConvert	*g_pcsAgpmEventItemConvert	;
AgcmEventItemConvert	*g_pcsAgcmEventItemConvert	;

AgpmEventGuild		*g_pcsAgpmEventGuild;
AgcmEventGuild		*g_pcsAgcmEventGuild;

AgpmEventProduct	*g_pcsAgpmEventProduct;
AgcmEventProduct	*g_pcsAgcmEventProduct;

AgpmEventQuest		*g_pcsAgpmEventQuest;
AgcmEventQuest		*g_pcsAgcmEventQuest;

AgpmBuddy			*g_pcsAgpmBuddy;
AgpmChannel			*g_pcsAgpmChannel;

AgcmEventEffect		*g_pcsAgcmEventEffect;
AgcmUVAnimation*	g_pcsAgcmUVAnimation	= NULL;		
AgcmEff2*			g_pcsAgcmEff2			= NULL;
AgcmSound			*g_pcsAgcmSound;

AgpmParty			*g_pcsAgpmParty;
AgcmParty			*g_pcsAgcmParty;

//AgcmUIManager		*g_pcsAgcmUIManager		;

AgcmShadow			*g_pcsAgcmShadow			;
AgcmWater			*g_pcsAgcmWater			;
AgcmLODManager		*g_pcsAgcmLODManager		;
AgcmFont			*g_pcsAgcmFont			;
AgcmLensFlare		*g_pcsAgcmLensFlare		;
AgcmTextBoardMng	*g_pcsAgcmTextBoardMng		;

AgpmSkill			*g_pcsAgpmSkill			;
AgcmSkill			*g_pcsAgcmSkill			;

AgpmEventSkillMaster	*g_pcsAgpmEventSkillMaster	;
AgcmEventSkillMaster	*g_pcsAgcmEventSkillMaster	;

AgpmAreaChatting	*g_pcsAgpmAreaChatting	;
AgcmChatting2		*g_pcsAgcmChatting2		;
AgpmChatting		*g_pcsAgpmChatting		;

AgpmAuctionCategory *g_pcsAgpmAuctionCategory;
AgpmAuction			*g_pcsAgpmAuction		;

AgcmGlyph			*g_pcsAgcmGlyph			;
AgcmTargeting		*g_pcsAgcmTargeting		;

AgpmGuild			*g_pcsAgpmGuild;
AgcmGuild			*g_pcsAgcmGuild;

AgpmProduct			*g_pcsAgpmProduct;
AgcmProduct			*g_pcsAgcmProduct;

AgpmRefinery*			g_pcsAgpmRefinery = NULL;
AgcmRefinery*			g_pcsAgcmRefinery = NULL;
AgpmEventRefinery*		g_pcsAgpmEventRefinery = NULL;
AgcmEventRefinery*		g_pcsAgcmEventRefinery = NULL;

AgpmQuest			*g_pcsAgpmQuest;
AgcmQuest			*g_pcsAgcmQuest;

AgpmRemission		*g_pcsAgpmRemission = NULL;

AgcmUIMain			*g_pcsAgcmUIMain			;
AgcmUICharacter		*g_pcsAgcmUICharacter		;
AgcmUIItem			*g_pcsAgcmUIItem			;
AgcmUICooldown		*g_pcsAgcmUICooldown		;
AgcmUIEventItemRepair	*g_pcsAgcmUIEventItemRepair	;
AgcmUIEventTeleport	*g_pcsAgcmUIEventTeleport	;
AgcmUISkill2		*g_pcsAgcmUISkill2			;
AgpmWorld			*g_pcsAgpmWorld				;
AgcmUILogin			*g_pcsAgcmUILogin			;
AgcmUIParty			*g_pcsAgcmUIParty			;
AgcmUIPartyOption	*g_pcsAgcmUIPartyOption		;
AgcmUIEventNPCDialog*g_pcsAgcmUIEventNPCDialog	;
AgcmUIEventBank		*g_pcsAgcmUIEventBank		;
AgcmUIEventItemConvert	*g_pcsAgcmUIEventItemConvert	;
AgcmUIGuild			*g_pcsAgcmUIGuild;
AgcmUISplitItem		*g_pcsAgcmUISplitItem		;
AgcmUIRefinery*			g_pcsAgcmUIRefinery = NULL;
AgcmUIProduct		*g_pcsAgcmUIProduct;
AgcmUIProductSkill	*g_pcsAgcmUIProductSkill;
//AgcmUIQuest			*g_pcsAgcmUIQuest;
AgcmUIQuest2			*g_pcsAgcmUIQuest2;
AgcmUIBuddy			*g_pcsAgcmUIBuddy;
AgcmUIChannel		*g_pcsAgcmUIChannel;

AgcmUISearch		*g_pcsAgcmUISearch;
AgpmSearch			*g_pcsAgpmSearch;

AgpmPvP				*g_pcsAgpmPvP;
AgcmUIPvP			*g_pcsAgcmUIPvP;

AgcmWorld			*g_pcsAgcmWorld			;

AcuFrameMemory		g_csAcuFrameMemory		;
AgcmUIChatting2		*g_pcsAgcmUIChatting2		;
AgcmUIOption		*g_pcsAgcmUIOption		;

AgcmUIAuction		*g_pcsAgcmUIAuction;
AgcmMinimap			*g_pcsAgcmMinimap			;
AgcmUITips			*g_pcsAgcmUITips		;

ApmOcTree*				g_pcsApmOcTree = NULL;
AgcmOcTree*				g_pcsAgcmOcTree = NULL;

AgcmCustomizeRender		*g_pcsAgcmCustomizeRender;

AgpmEventCharCustomize	*g_pcsAgpmEventCharCustomize;
AgcmEventCharCustomize	*g_pcsAgcmEventCharCustomize;
AgcmUICustomizing		*g_pcsAgcmUICustomizing;
AgcmUIRemission		*g_pcsAgcmUIRemission = NULL;

AgpmBillInfo		*g_pcsAgpmBillInfo;

AgpmSummons			*g_pcsAgpmSummons;

AgpmCashMall		*g_pcsAgpmCashMall		= NULL;
AgcmCashMall		*g_pcsAgcmCashMall		= NULL;
AgcmUICashMall		*g_pcsAgcmUICashMall	= NULL;
AgcmUICashInven		*g_pcsAgcmUICashInven	= NULL;	//	2005.10.14. By SungHoon
AgpmMailBox			*g_pcsAgpmMailBox = NULL;
AgcmUIMailBox		*g_pcsAgcmUIMailBox = NULL;

AgcmUIHelp			*g_pcsAgcmUIHelp = NULL;

AgpmReturnToLogin	*g_pcsAgpmReturnToLogin	= NULL;
AgcmReturnToLogin	*g_pcsAgcmReturnToLogin	= NULL;

AgpmWantedCriminal		*g_pcsAgpmWantedCriminal = NULL;
AgcmUIWantedCriminal	*g_pcsAgcmUIWantedCriminal = NULL;

AgcmNatureEffect		*g_pcsAgcmNatureEffect;	//	2005.07.18. By SungHoon

AgpmStartupEncryption*	g_pcsAgpmStartupEncryption;
AgcmStartupEncryption*	g_pcsAgcmStartupEncryption;


AgpmSiegeWar			*g_pcsAgpmSiegeWar;
AgpmEventSiegeWarNPC	*g_pcsAgpmEventSiegeWarNPC;
AgcmSiegeWar			*g_pcsAgcmSiegeWar;
AgcmEventSiegeWarNPC	*g_pcsAgcmEventSiegeWarNPC;
AgcmUISiegeWar			*g_pcsAgcmUISiegeWar;

//@{ 2006/06/27 burumal
AgcmUIHotkey			*g_pcsAgcmUIHotkey;
//@}

AgcmUISocialAction		*g_pcsAgcmUISocialAction;



AgpmTax					*g_pcsAgpmTax;
AgcmUITax				*g_pcsAgcmUITax;


AgpmGuildWarehouse		*g_pcsAgpmGuildWarehouse;
AgcmUIGuildWarehouse	*g_pcsAgcmUIGuildWarehouse;

//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
//AgpmGamble				*g_pcsAgpmGamble;
//AgcmUIGamble			*g_pcsAgcmUIGamble;

AgpmArchlord			*g_pcsAgpmArchlord;

AgpmEventGacha			*g_pcsAgpmEventGacha		= NULL;
AgcmEventGacha			*g_pcsAgcmEventGacha		= NULL;
AgcmUIEventGacha		*g_pcsAgcmUIEventGacha		= NULL;

AgpmBattleGround		*g_pcsAgpmBattleGround = NULL;


// SetCallbackPostRender
BOOL	__AgcmRender_PostRenderCallback (PVOID pData, PVOID pClass, PVOID pCustData)
{
	if( g_pEngine && g_pEngine->m_pCurrentFullUIModule )
		g_MainWindow.WindowRender();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyEngine::MyEngine()
{
	g_ClientStrEncrypt = false;

	m_szImagePath = ".\\Texture\\;.\\Texture\\UI\\";
}

MyEngine::~MyEngine()
{

}

BOOL		MyEngine::OnAttachPlugins	()	// 플러그인 연결 은 여기서
{
    /* 
     * Attach world plug-in...
     */
    if (!RpWorldPluginAttach())
    {
        return FALSE;
    }

	if (!RpMatFXPluginAttach())
	{
		return FALSE;
	}

	if (!RpHAnimPluginAttach())
	{
		return FALSE;
	}

	if (!RpSkinPluginAttach())
	{
		return FALSE;
	}

    if (!RpCollisionPluginAttach() )
    {
        return FALSE;
    }

	if (!RpSplinePluginAttach() )
	{
		return FALSE;
	}

	if(!RpMorphPluginAttach())
	{
		return FALSE;
	}

	if( !RpMTexturePluginAttach() )
	{
		return FALSE;
	}

	if( !RpUserDataPluginAttach() )
	{
		return FALSE;
	}

	if( !RpLODAtomicPluginAttach() )
	{
		return FALSE;
	}

	if (!RtAnimInitialize())
	{
		return FALSE;
	}

	return TRUE;
}

RpMaterial *Material(RpMaterial *material, void *data)
{
	return material;
}

RpWorld *	MyEngine::OnCreateWorld		() // 월드를 생성해서 리턴해준다.
{
	RpWorld *pWorld;

	//RwImageSetPath( "./Texture/" );
	RwImageSetPath(".\\Texture\\Etc\\;.\\Texture\\World\\;.\\Texture\\Character\\;.\\Texture\\Object\\;.\\Texture\\Object\\;.\\Texture\\Effect\\;.\\Texture\\UI\\;.\\Texture\\SKILL\\;.\\Texture\\Item\\;.\\Texture\\UI\\BASE\\;.\\Texture\\NotPacked\\");

	RwTextureSetMipmapping(TRUE);
	RwTextureSetAutoMipmapping(TRUE);

	pWorld = AGCMMAP_THIS->GetWorld();
	/*
	RwBBox bbox;
	bbox.inf.x=bbox.inf.y=bbox.inf.z=-100000;
	bbox.sup.x=bbox.sup.y=bbox.sup.z=-100000;
	pWorld = RpWorldCreate(&bbox);
	*/

	return pWorld;
}

RwCamera *	MyEngine::OnCreateCamera	( RpWorld * pWorld )	// 메인 카메라를 Create해준다.
{
	m_fFarClipPlane = 50000.0f;
	m_fNearClipPlane = 100.0f;

	return CreateCamera( pWorld );
}

BOOL		MyEngine::OnCreateLight		()	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
{
	ASSERT( m_pWorld );
	//return AgcEngine::OnCreateLight();
	m_pLightAmbient = RpLightCreate(rpLIGHTAMBIENT);
	
	float	ambiantvalue		= 0.3f;//0.4f;
	float	directionalvalue	= 0.7f;

    if (m_pLightAmbient)
    {
		RwRGBAReal color = {ambiantvalue, ambiantvalue, ambiantvalue, 1.0f};
		RpLightSetColor(m_pLightAmbient, &color);
		RpWorldAddLight(m_pWorld, m_pLightAmbient);
    }

    m_pLightDirect = RpLightCreate(rpLIGHTDIRECTIONAL);

    if (m_pLightDirect)
    {
        RwFrame *f = RwFrameCreate();
        RpLightSetFrame( m_pLightDirect , f);

		RwV3d v = { 0.0f , 0.0f , 0.0f };
		RwFrameTranslate	( f , & v ,	rwCOMBINEREPLACE );

		RwMatrix	* pMatrix	= RwFrameGetLTM	( f			);
		RwV3d		* pAt		= RwMatrixGetAt		( pMatrix	);
		pAt->x	= 0.556f;
		pAt->y	= -0.658f;
		pAt->z	= 0.056f;

		RwRGBAReal color = {(240.0f/255.0f), (202.0f/255.0f), (116.0f/255.0f), 1.0f};
		RpLightSetColor(m_pLightDirect, &color);

        RpWorld * pWorld = RpWorldAddLight(m_pWorld, m_pLightDirect);
    }

	return TRUE;
}

AgcWindow *	MyEngine::OnSetMainWindow	()	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.
{
	return &g_MainWindow;
}

BOOL MyEngine::OnRegisterModule()
{
	g_MyEngine.SetMaxConnection(5, 10240);		// Client Socket 갯수 설정

	REGISTER_MODULE( g_pcsAgcmResourceLoader, AgcmResourceLoader	)

	REGISTER_MODULE( g_pcsApmMap, ApmMap				)
	REGISTER_MODULE( g_pcsApmOcTree			, ApmOcTree				)
	if (g_pcsApmOcTree)	g_pcsApmOcTree->m_bOcTreeEnable = FALSE;			// 다른 곳의 onAddModule에서 참고할수 있다..
	REGISTER_MODULE( g_pcsAgpmTimer, 	AgpmTimer		)
	REGISTER_MODULE( g_pcsAgpmFactors, AgpmFactors		)
	REGISTER_MODULE( g_pcsApmObject, ApmObject			)
	REGISTER_MODULE( g_pcsAgpmCharacter, AgpmCharacter		)
	REGISTER_MODULE( g_pcsAgpmBillInfo, AgpmBillInfo	)
	REGISTER_MODULE( g_pcsAgpmUnion, AgpmUnion			)
	REGISTER_MODULE( g_pcsAgpmReturnToLogin, AgpmReturnToLogin	)
	REGISTER_MODULE( g_pcsAgpmLogin, AgpmLogin			)
	REGISTER_MODULE( g_pcsAgpmGrid, AgpmGrid			)
	REGISTER_MODULE( g_pcsAgpmItem, AgpmItem			)
	REGISTER_MODULE( g_pcsAgpmParty, AgpmParty			)
	REGISTER_MODULE( g_pcsAgpmSummons, AgpmSummons		)
	REGISTER_MODULE( g_pcsApmEventManager, ApmEventManager	)
	REGISTER_MODULE( g_pcsAgpmSkill, AgpmSkill			)
	REGISTER_MODULE( g_pcsAgpmItemConvert, AgpmItemConvert	)
	REGISTER_MODULE( g_pcsAgpmArchlord, AgpmArchlord	)

	REGISTER_MODULE( g_pcsAgcmOcTree		, AgcmOcTree			)
	REGISTER_MODULE( g_pcsAgcmRender, AgcmRender			)

	REGISTER_MODULE( g_pcsAgcmLODManager, AgcmLODManager		)
	REGISTER_MODULE( g_pcsAgcmFont, AgcmFont			)
	REGISTER_MODULE( g_pcsAgcmSound, AgcmSound			)
	REGISTER_MODULE_SINGLETON( AgcmMap )
	REGISTER_MODULE( g_pcsAgcmObject, AgcmObject			)
	//REGISTER_MODULE( g_pcsAgcmWater, AgcmWater			)
	//REGISTER_MODULE( g_pcsAgcmShadow, AgcmShadow			)
	REGISTER_MODULE( g_pcsAgcmConnectManager, AgcmConnectManager	)
	REGISTER_MODULE( g_pcsAgcmCharacter, AgcmCharacter		)
	
	REGISTER_MODULE( g_pcsCamera2, AgcmCamera2				)
	REGISTER_MODULE( g_pcsAgcmLogin, AgcmLogin			)
	REGISTER_MODULE( g_pcsAgcmUIControl, AgcmUIControl		)
	REGISTER_MODULE( g_pcsAgcmItem, AgcmItem			)
	REGISTER_MODULE( g_pcsAgcmItemConvert, AgcmItemConvert	)
	REGISTER_MODULE( g_pcsAgcmRecruit, AgcmRecruit		)
	REGISTER_MODULE( g_pcsAgcmPrivateTrade, AgcmPrivateTrade	)
	REGISTER_MODULE( g_pcsAgcmParty, AgcmParty			)
	REGISTER_MODULE( g_pcsAgcmNatureEffect	, AgcmNatureEffect		)		// 2005.07.18. By SungHoon
	REGISTER_MODULE( g_pcsAgcmEventManager, AgcmEventManager	)
	REGISTER_MODULE( g_pcsAgpmEventNature, AgpmEventNature	)
	REGISTER_MODULE( g_pcsAgpmGuild, AgpmGuild	)
	REGISTER_MODULE( g_pcsAgpmSiegeWar, AgpmSiegeWar )
	REGISTER_MODULE( g_pcsAgpmEventTeleport, AgpmEventTeleport	)
	REGISTER_MODULE( g_pcsAgcmEventTeleport, AgcmEventTeleport	)
	REGISTER_MODULE( g_pcsAgpmEventNPCDialog, AgpmEventNPCDialog)
	REGISTER_MODULE( g_pcsAgcmEventNPCDialog, AgcmEventNPCDialog)
	REGISTER_MODULE( g_pcsAgpmEventNPCTrade, AgpmEventNPCTrade)
	REGISTER_MODULE( g_pcsAgpmEventBank, AgpmEventBank	)
	REGISTER_MODULE( g_pcsAgcmEventBank, AgcmEventBank	)
	REGISTER_MODULE( g_pcsAgpmEventItemConvert, AgpmEventItemConvert	)
	REGISTER_MODULE( g_pcsAgcmEventItemConvert, AgcmEventItemConvert	)
	REGISTER_MODULE( g_pcsAgpmEventGuild, AgpmEventGuild	)
	REGISTER_MODULE( g_pcsAgcmEventGuild, AgcmEventGuild		)
	REGISTER_MODULE( g_pcsAgpmProduct, AgpmProduct		)
	REGISTER_MODULE( g_pcsAgcmProduct, AgcmProduct		)
	REGISTER_MODULE( g_pcsAgpmEventProduct, AgpmEventProduct)
	REGISTER_MODULE( g_pcsAgcmEventProduct, AgcmEventProduct)
	REGISTER_MODULE( g_pcsAgpmEventQuest, AgpmEventQuest )
	REGISTER_MODULE( g_pcsAgcmEventQuest, AgcmEventQuest )

	REGISTER_MODULE( g_pcsAgcmUIManager2, AgcmUIManager2		)
	
	REGISTER_MODULE( g_pcsAgcmSkill, AgcmSkill			)

	REGISTER_MODULE( g_pcsAgpmQuest, AgpmQuest )
	REGISTER_MODULE( g_pcsAgcmQuest, AgcmQuest )
	REGISTER_MODULE( g_pcsAgpmBuddy, AgpmBuddy )
	REGISTER_MODULE( g_pcsAgpmChannel, AgpmChannel )
	
	REGISTER_MODULE( g_pcsAgpmRide, AgpmRide)

	REGISTER_MODULE( g_pcsAgpmEventSkillMaster, AgpmEventSkillMaster	)
	REGISTER_MODULE( g_pcsAgcmEventSkillMaster, AgcmEventSkillMaster	)
	
	REGISTER_MODULE( g_pcsAgpmCombat, AgpmCombat			)

	REGISTER_MODULE( g_pcsAgpmBattleGround, AgpmBattleGround )
	REGISTER_MODULE( g_pcsAgpmPvP, AgpmPvP	)

	REGISTER_MODULE( g_pcsAgcmTextBoardMng, AgcmTextBoardMng		)
	REGISTER_MODULE( g_pcsAgcmEventNPCTrade, AgcmEventNPCTrade	)
	REGISTER_MODULE( g_pcsAgcmUVAnimation, AgcmUVAnimation	)
	REGISTER_MODULE( g_pcsAgcmEff2, AgcmEff2		)
	REGISTER_MODULE( g_pcsAgcmEventEffect, AgcmEventEffect	)
	REGISTER_MODULE( g_pcsAgcmEventNature, AgcmEventNature	)
	REGISTER_MODULE( g_pcsAgcmLensFlare, AgcmLensFlare		)
	REGISTER_MODULE( g_pcsAgpmChatting, AgpmChatting		)

	REGISTER_MODULE( g_pcsAgcmGlyph, AgcmGlyph			)

	REGISTER_MODULE( g_pcsAgpmEventItemRepair, AgpmEventItemRepair	)
	REGISTER_MODULE( g_pcsAgcmEventItemRepair, AgcmEventItemRepair	)

	REGISTER_MODULE( g_pcsAgpmAreaChatting	, AgpmAreaChatting		)

	

	REGISTER_MODULE( g_pcsAgpmStartupEncryption, AgpmStartupEncryption	)

	REGISTER_MODULE( g_pcsAgcmGuild, AgcmGuild	)

	REGISTER_MODULE( g_pcsAgcmChatting2, AgcmChatting2		)

	REGISTER_MODULE( g_pcsAgpmRefinery		, AgpmRefinery			)
	REGISTER_MODULE( g_pcsAgcmRefinery		, AgcmRefinery			)
	REGISTER_MODULE( g_pcsAgpmEventRefinery	, AgpmEventRefinery		)
	REGISTER_MODULE( g_pcsAgcmEventRefinery	, AgcmEventRefinery		)

	REGISTER_MODULE( g_pcsAgpmAuctionCategory, AgpmAuctionCategory		)
	REGISTER_MODULE( g_pcsAgpmAuction, AgpmAuction			)
	REGISTER_MODULE( g_pcsAgcmAuction, AgcmAuction		)

	REGISTER_MODULE( g_pcsAgpmCashMall, AgpmCashMall)
	REGISTER_MODULE( g_pcsAgcmCashMall, AgcmCashMall)

	REGISTER_MODULE( g_pcsAgpmEventSiegeWarNPC, AgpmEventSiegeWarNPC )
	REGISTER_MODULE( g_pcsAgcmSiegeWar, AgcmSiegeWar )
	REGISTER_MODULE( g_pcsAgcmEventSiegeWarNPC, AgcmEventSiegeWarNPC )

	REGISTER_MODULE( g_pcsAgcmUISample, AgcmUISample		)
	REGISTER_MODULE( g_pcsAgcmUIMain, AgcmUIMain			)
	REGISTER_MODULE( g_pcsAgcmMinimap, AgcmMinimap		)
	REGISTER_MODULE( g_pcsAgcmUICharacter, AgcmUICharacter	)
	REGISTER_MODULE( g_pcsAgcmUISplitItem, AgcmUISplitItem)
	REGISTER_MODULE( g_pcsAgcmUIAuction, AgcmUIAuction		)
	REGISTER_MODULE( g_pcsAgcmUIItem, AgcmUIItem			)
	REGISTER_MODULE( g_pcsAgcmUICooldown, AgcmUICooldown	)		
	REGISTER_MODULE( g_pcsAgcmUIEventItemRepair, AgcmUIEventItemRepair	)
	REGISTER_MODULE( g_pcsAgcmUIEventTeleport, AgcmUIEventTeleport	)
	REGISTER_MODULE( g_pcsAgcmUICashInven, AgcmUICashInven )
	REGISTER_MODULE( g_pcsAgcmUISkill2, AgcmUISkill2		)
	REGISTER_MODULE( g_pcsAgcmUIRefinery	, AgcmUIRefinery		)
	REGISTER_MODULE( g_pcsAgcmUIProduct, AgcmUIProduct	)
	REGISTER_MODULE( g_pcsAgcmUIProductSkill, AgcmUIProductSkill	)
	REGISTER_MODULE( g_pcsAgcmUIQuest2, AgcmUIQuest2	)

	REGISTER_MODULE( g_pcsAgpmWorld, AgpmWorld		)
	REGISTER_MODULE( g_pcsAgcmWorld, AgcmWorld			)

	REGISTER_MODULE( g_pcsAgcmReturnToLogin, AgcmReturnToLogin)
	
	REGISTER_MODULE( g_pcsAgpmGuildWarehouse, AgpmGuildWarehouse)
	//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
	//REGISTER_MODULE( g_pcsAgpmGamble, AgpmGamble)
	REGISTER_MODULE( g_pcsAgpmEventGacha, AgpmEventGacha	);
	REGISTER_MODULE( g_pcsAgcmEventGacha, AgcmEventGacha	);

	REGISTER_MODULE( g_pcsAgpmEventCharCustomize, AgpmEventCharCustomize )
	REGISTER_MODULE( g_pcsAgcmUIGuild, AgcmUIGuild	)
	REGISTER_MODULE( g_pcsAgcmUILogin, AgcmUILogin		)
	REGISTER_MODULE( g_pcsAgcmUIChatting2, AgcmUIChatting2		)
	REGISTER_MODULE( g_pcsAgcmUIParty, AgcmUIParty		)
	REGISTER_MODULE( g_pcsAgcmUIPartyOption, AgcmUIPartyOption	)
	REGISTER_MODULE( g_pcsAgcmUIOption, AgcmUIOption		)
	REGISTER_MODULE( g_pcsAgcmUIEventNPCDialog, AgcmUIEventNPCDialog)
	REGISTER_MODULE( g_pcsAgcmUIEventBank, AgcmUIEventBank	)
	REGISTER_MODULE( g_pcsAgcmUIEventItemConvert, AgcmUIEventItemConvert	)
	REGISTER_MODULE( g_pcsAgcmUIBuddy, AgcmUIBuddy )
	REGISTER_MODULE( g_pcsAgcmUIChannel, AgcmUIChannel )

	REGISTER_MODULE( g_pcsAgcmUIPvP, AgcmUIPvP	)

	REGISTER_MODULE( g_pcsAgcmTargeting, AgcmTargeting		)

	REGISTER_MODULE( g_pcsAgcmCustomizeRender, AgcmCustomizeRender )

	REGISTER_MODULE( g_pcsAgcmEventCharCustomize, AgcmEventCharCustomize )
	REGISTER_MODULE( g_pcsAgcmUICustomizing, AgcmUICustomizing )
	REGISTER_MODULE( g_pcsAgcmUITips, AgcmUITips )
	REGISTER_MODULE( g_pcsAgpmRemission, AgpmRemission )
	REGISTER_MODULE( g_pcsAgcmUIRemission, AgcmUIRemission )
	REGISTER_MODULE( g_pcsAgcmUICashMall, AgcmUICashMall)
	REGISTER_MODULE( g_pcsAgpmMailBox, AgpmMailBox)
	REGISTER_MODULE( g_pcsAgcmUIMailBox, AgcmUIMailBox)
	REGISTER_MODULE( g_pcsAgpmWantedCriminal, AgpmWantedCriminal )
	REGISTER_MODULE( g_pcsAgcmUIWantedCriminal, AgcmUIWantedCriminal )

	REGISTER_MODULE( g_pcsAgcmUIHelp, AgcmUIHelp)

	REGISTER_MODULE( g_pcsAgcmUISiegeWar, AgcmUISiegeWar )
	REGISTER_MODULE( g_pcsAgcmUIGuildWarehouse, AgcmUIGuildWarehouse)
	//중국만 쓰던 모듈이나 이제 중국도 안쓴다. tj@081013
	//REGISTER_MODULE( g_pcsAgcmUIGamble, AgcmUIGamble)
	REGISTER_MODULE( g_pcsAgcmUIEventGacha, AgcmUIEventGacha);

	REGISTER_MODULE( g_pcsAgpmSearch, AgpmSearch )
	REGISTER_MODULE( g_pcsAgcmUISearch, AgcmUISearch )

	REGISTER_MODULE( g_pcsAgcmStartupEncryption, AgcmStartupEncryption	)

	REGISTER_MODULE( g_pcsAgcmUIHotkey, AgcmUIHotkey )

	REGISTER_MODULE( g_pcsAgcmUISocialAction, AgcmUISocialAction )
	
	REGISTER_MODULE(g_pcsAgpmTax, AgpmTax);
	REGISTER_MODULE(g_pcsAgcmUITax, AgcmUITax);

	g_pcsAgcmLogin->SetMainWindow(&g_MainWindow);

	g_pcsAgcmRender->SetMainFrame(RwCameraGetFrame(m_pCamera));		// modify by gemani
	
	g_pcsAgcmRender->SetWorldCamera		( m_pWorld, m_pCamera);
	g_pcsAgcmRender->SetLight				( m_pLightAmbient, m_pLightDirect );

	g_pcsApmMap->		SetLoadingMode		( TRUE , FALSE	);
	g_pcsApmMap->		SetAutoLoadData		( TRUE			);
	AGCMMAP_THIS->	SetAutoLoadRange	( 10 , 10 , 5 );

	if( !g_pcsApmMap->Init( NULL ) )
		return FALSE;

	AuPackingManager::MakeInstance();

	char	strClientMapDataDirectory[ 1024 ];
	GetCurrentDirectory( 1024 , strClientMapDataDirectory );
	AGCMMAP_THIS->		SetDataPath				( strClientMapDataDirectory );

	AGCMMAP_THIS->InitMapClientModule(m_pWorld);

	g_pcsApmObject->		SetMaxObjectTemplate	( 1000							);
	g_pcsApmObject->		SetMaxObject			( 3000							);

	g_pcsAgcmObject->		SetClumpPath			( ".\\Object\\"					);
	g_pcsAgcmObject->		SetAnimationPath		( ".\\Object\\Animation\\"		);
	g_pcsAgcmObject->		SetRpWorld				( m_pWorld						);
	g_pcsAgcmObject->		SetMaxAnimation			( 1000							);

	g_pcsAgpmCharacter->	SetMaxCharacterTemplate	( 1000							);
	g_pcsAgpmCharacter->	SetMaxCharacter			( 1000							);

	g_pcsAgcmCharacter->	SetWorld				( m_pWorld, m_pCamera			);
	g_pcsAgcmCharacter->	SetMaxAnimations		( 1000							);

	g_pcsAgcmCharacter->	SetClumpPath			( ".\\Character\\"				);
	g_pcsAgcmCharacter->	SetAnimationPath		( ".\\Character\\Animation\\"	);
	g_pcsAgcmCharacter->	SetSendMoveDelay		( 1000							);

	g_pcsAgpmItem->		SetMaxItem				( 1000							);
	g_pcsAgpmItem->		SetMaxItemTemplate		( 20000							);

	g_pcsAgcmItem->		SetClumpPath			( ".\\Character\\"				);
	g_pcsAgcmItem->		SetWorld				( m_pWorld						);
	g_pcsAgcmItem->		SetMaxItemClump			( 1000							);

	AGCMMAP_THIS->SetCamera( m_pCamera );

	AGCMMAP_THIS->		SetTexDict				( ".\\Texture\\Tiles.txd"		);
	g_pcsAgcmObject->		SetTexDictFile			( ".\\Texture\\Object.txd"		);
	g_pcsAgcmCharacter->	SetTexDictFile			( ".\\Texture\\Character.txd"	);
	g_pcsAgcmItem->		SetTexDictFile			( ".\\Texture\\Item.txd"		);

	g_pcsAgpmSkill->		SetMaxSkill				( 1000							);
	g_pcsAgpmSkill->		SetMaxSkillTemplate		( 3000							);
	g_pcsAgpmSkill->		SetMaxSkillSpecializeTemplate	( 10					);
	g_pcsAgpmSkill->		SetMaxSkillTooltipTemplate ( 1000						);

	g_pcsAgcmResourceLoader->SetTexDictPath(".\\Texture\\Object\\");


	g_pcsAgpmWorld->SetMaxWorld(20);

	g_pcsAgpmQuest->SetMaxTemplateCount(200);
	g_pcsAgpmQuest->SetMaxGroupCount(200);

	if (g_pcsAgcmEff2)
	{
		g_pcsAgcmEff2->bSetGlobalVariable(NULL, NULL, m_pWorld, m_pCamera);
		AgcdEffGlobal::bGetInst().bFlagOff( E_GFLAG_USEPACK );
	}

	g_pcsAgpmEventNature->AddDefaultSkySet();
	g_pcsAgcmEventNature->Init( m_pWorld , m_pLightAmbient , m_pLightDirect , m_pCamera );
	g_pcsAgpmEventNature->SetSpeedRate( 10 );

	g_pcsAgpmEventTeleport->SetMaxTeleportPoint(300);
	g_pcsAgpmEventTeleport->SetMaxTeleportGroup(300);

	g_pcsAgpmParty->SetMaxParty(1);

	// Guild
	g_pcsAgpmGuild->SetMaxGuild(5);

	// Product
	g_pcsAgpmProduct->SetMaxSkillFactor(10);
	g_pcsAgpmProduct->SetMaxComposeTemplate(100);
	//g_pcsAgpmRefinery->SetMaxRefineTemplate(100);

	//if( g_pcsAgcmShadow )
	//{
	//	g_pcsAgcmShadow->AddWorldAmbientLight(m_pLightAmbient);
	//	g_pcsAgcmShadow->AddWorldDirectLight(m_pLightDirect);
	//	g_pcsAgcmShadow->AddWorldCamera(m_pCamera);
	//	g_pcsAgcmShadow->AddRpWorld(m_pWorld);
	//}

	g_pcsAgcmLensFlare->SetMyWorldCamera(m_pCamera);
	g_pcsAgcmLensFlare->SetMyWorld(m_pWorld);

	g_pcsAgcmTextBoardMng->SetWorldCamera(m_pCamera);

	g_pcsAgcmFont->SetWorldCamera(m_pCamera);
	
	g_pcsAgcmGlyph->SetWorldCamera(m_pCamera);

	g_pcsAgcmTargeting->SetRpWorld(m_pWorld);
	g_pcsAgcmTargeting->SetRwCamera(m_pCamera);
	g_pcsAgcmTargeting->SetShowCursor(TRUE);

	g_pcsAgcmUIManager2->SetTexturePath(".\\Texture\\UI\\");
	g_pcsAgcmUIManager2->SetMainWindow			(&g_MainWindow);
	g_pcsAgcmUIManager2->m_bUseAbsoluteOnly = TRUE;
	g_pcsAgcmUIManager2->SetSoundPath(UIT_SOUND_PATH);
	g_pcsAgcmUIManager2->SetMaxUIMessage(1024);

	if (g_pcsAgcmUITips)
	{
		g_pcsAgcmUITips->SetTexturePath("Texture\\UI\\Base\\");
	}

	m_nControlADIndex = g_pcsAgcmUIManager2->AttachControlData(this, sizeof(UITControlData), NULL, NULL);
	if (m_nControlADIndex < 0)
		return FALSE;

	return TRUE;
}

BOOL MyEngine::OnPostInitialize()
{
	return TRUE;
}

BOOL MyEngine::OnInit()
{
	g_csAcuFrameMemory.Create();
	g_csAcuFrameMemory.InitManager(10000000);				// 10MB 할당

	return AgcEngine::OnInit();
}

VOID MyEngine::OnWorldRender()
{
	g_pcsAgcmRender->Render();
}

BOOL MyEngine::OnCameraResize( RwRect * pRect )
{
	g_pcsAgcmUIManager2->ResizeManagerWindow(pRect->x, pRect->y, pRect->w, pRect->h);

	return AgcEngine::OnCameraResize( pRect );
}

VOID MyEngine::OnTerminate()
{
	g_csAcuFrameMemory.Release();

#ifdef _AP_MEMORY_TRACKER_
	ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);
#endif

	::PostQuitMessage(1);

	AgcEngine::OnTerminate();
}

VOID	MyEngine::OnEndIdle()
{
	g_csAcuFrameMemory.Clear();
}

VOID MyEngine::SaveControlData(AgcdUIControl *pcsControl)
{
	UITControlData *pstControlData = GetControlData(pcsControl);

	pstControlData->m_stWinProperty		=	pcsControl->m_pcsBase->m_Property	;
	pstControlData->m_stBaseProperty	=	pcsControl->m_pcsBase->m_clProperty	;

	switch (pcsControl->m_lType)
	{
	case AcUIBase::TYPE_TREE:
	case AcUIBase::TYPE_LIST:
		{
			pstControlData->m_lListItemVisibleRow = ((AcUIList *) pcsControl->m_pcsBase)->m_lVisibleListItemRow;
			break;
		}
	}
}

VOID MyEngine::LoadControlData(AgcdUIControl *pcsControl)
{
	UITControlData *pstControlData = GetControlData(pcsControl);

	pcsControl->m_pcsBase->m_Property	=	pstControlData->m_stWinProperty		;
	pcsControl->m_pcsBase->m_clProperty	=	pstControlData->m_stBaseProperty	;

	switch (pcsControl->m_lType)
	{
	case AcUIBase::TYPE_TREE:
	case AcUIBase::TYPE_LIST:
		{
//			((AcUIList *) pcsControl->m_pcsBase)->SetListItemWindowVisibleRow(pstControlData->m_lListItemVisibleRow);
			break;
		}
	}
}