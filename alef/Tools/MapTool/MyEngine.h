// MyEngine.h: interface for the MyEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYENGINE_H__944AAD22_B83C_4977_A376_A382BE50B1BA__INCLUDED_)
#define AFX_MYENGINE_H__944AAD22_B83C_4977_A376_A382BE50B1BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AgcEngine.h>
#include "rpspline.h"

#include "ApmMap.h"
#include "AgcmMap.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"

#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgpmDropItem.h"
#include "AgcmItem.h"

#include "AgpmUnion.h"

#include "AgpmParty.h"

#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgpmFactors.h"

#include "AgcmRender.h"

#include "ApmEventManager.h"
#include "AgpmEventNature.h"
#include "AgcmEventNature.h"
#include "AgpmEventSpawn.h"
#include "AgpmEventTeleport.h"
#include "AgpmEventNPCDialog.h"
#include "AgpmEventNPCTrade.h"
#include "AgcmEventNPCTrade.h"

#include "AgpmAuction.h"

#include "ApmEventManagerDlg.h"
#include "AgcmEventNPCDialogDlg.h"
#include "AgcmEventNPCTradeDlg.h"
#include "AgcmEventAuctionDlg.h"
#include "AgcmEventSpawnDlg.h"
#include "AgcmEventTeleportDlg.h"

//#include "AgpmAI.h"
//#include "AgcmAIDlg.h"

#include "AgpmAI2.h"

//@{ kday 20050113
//#include "AgcmCamera.h"
//@} kday
#include "AgcmShadow.h"
#include "AgcmWater.h"
#include "AgcmFont.h"
#include "AgcmSkill.h"
//#include "AgcmImportClientData.h"
#include "AgcmEventEffect.h"

#include "AgcmUIControl.h"
#include "AgcmShrineDlg.h"
#include "AgcmEventNatureDlg.h"

//#include "AgcmEffect.h"

#include "AgcmBlockingDlg.h"
#include "AgcmLODDlg.h"
#include "AgcmEffectDlg.h"
//#include "AgcmSkillDlg.h"
//#include "AgcmAnimationDlg.h"

#include "AgcmTuner.h"

#include "AgcmWaterDlg.h"
#include "AgcmWaterDlg2.h"

#include "AgcmObjectTypeDlg.h"
#include "AgpmTimer.h"
#include "AgcmUIControl.h"

#include "AgcmShadow2.h"
#include "AgpmPathFind.h"

#include "AgcmOcTree.h"
#include "ApmOcTree.h"

//#include "AgpmEventMasterySpecialize.h"

#include "AcuIMDraw.h"

#include "AgcmGrass.h"

//@{ Jaewon 20040621
#include "AgcmDynamicLightmap.h"
//@} Jaewon

// 마고자 (2004-06-15 오후 5:56:20) : 네똥아저씨 추가.
#include "AgpmEventBank.h"
#include "AgcmEventBank.h"
#include "AgcmEventManager.h"
#include "AgpmItemConvert.h"

#include "AgpmEventItemConvert.h"
#include "AgcmEventItemConvert.h"

#include "AgpmEventGuild.h"
#include "AgcmEventGuild.h"

#include "AgpmEventProduct.h"
#include "AgcmEventProduct.h"

#include "AgcmPostFX.h"
#include "AgcmUVAnimation.h"
#include "AcuRpMatD3DFx.h"

#include "AgpmEventBinding.h"
#include "AgcmEventBindingDlg.h"

#include "AgpmQuest.h"
#include "AgpmEventQuest.h"
#include "AgcmEventQuestDlg2.h"

#include "AgpmEventSkillMaster.h"
#include "AgcmEventSkillMaster.h"
#include "AgcmEventSkillMasterDlg.h"

#include "AgcmEff2.h"

#include "AgpmProduct.h"
#include "AgcmEventProductDlg.h"

//@{ Jaewon 20041229
#include "AgcmAmbOcclMap.h"
//@} Jaewon

//@{ Jaewon 20050103
#include "AmbOcclMap/AmbOcclMap.h"
//@} Jaewon

#include "AgcmCamera2.h"
#include "AgcmNatureEffect.h"

#include "AgpmEventPointLight.h"
#include "AgcmEventPointLight.h"

#include "AgpmTax.h"
#include "AgcmUITax.h"

#include "AgpmGuildWarehouse.h"
#include "AgcmUIGuildWarehouse.h"

#include "AgpmArchlord.h"

#include "AgpmEventGacha.h"
#include "AgcmEventGachaDlg.h"

class MyEngine : public AgcEngine  
{
public:
	// 모듈들..

	RwCamera *		GetParallelCamera	()	{ return m_pParallelCamera		; }
	BOOL			RenderAtomicOnly	( RpAtomic * pAtomic );
	void			RenderSubCamera		()	;
	void			RenderSubCameraGeometryOnly();
	BOOL			IsRenderSubCamera	()	{ return m_bIsRenderSubCamera	; }

	INT32			GetSubCameraType	()	{ return m_nSubCameraType		; }
	void			SetSubCameraType	( INT32 nType ) { m_nSubCameraType = nType; }
	void			MoveSubCameraToCenter( ApWorldSector * pSectorInput = NULL , BOOL bUseHeight = FALSE , FLOAT fHeight = 0.0f , INT32 nOffset = 0)	;
	

	void			SetRenderObject		( BOOL bUse )	{ m_bRenderObject = bUse; }
	BOOL			IsRenderObject		()	{ return m_bRenderObject		; }

	// Sub Camera Type
	enum
	{
		SC_OFF				,
		SC_MINIMAP			,
		SC_ROUGH			,
		SC_OBJECTCAPTURE	,	// 오브젝트 촬영용.
		SC_MINIMAPX4		,	// 미니맵 4배짜리.
		SC_MAX
	};

//protected:
	BOOL			m_bRenderObject					;
	INT32			m_nSubCameraType				;
	BOOL			m_bIsRenderSubCamera			;
	RwCamera *		m_pParallelCamera				;
	RwRaster *		m_pSubCameraMainCameraSubRaster	;
	RwRaster *		m_pSubCameraMainCameraSubZRaster;

	//@{ Jaewon 20050103
	AmbOcclMap		m_ambOcclMap;
	bool			m_computingAmbOcclMaps;
	float			m_progress;
	//@{ Jaewon20050113
	// flag for preventing the infinite recursion of the illumination callback
	bool			m_inIlluminationCallBack;
	//@} Jaewon
	//@} Jaewon
public:
	MyEngine();
	virtual ~MyEngine();

//	RpLight		*m_pLightDirect2		;

	// Message
	virtual	BOOL		OnAttachPlugins			();	// 플러그인 연결 은 여기서
	virtual	RpWorld *	OnCreateWorld			(); // 월드를 생성해서 리턴해준다.
	virtual	RwCamera *	OnCreateCamera			( RpWorld * pWorld );	// 메인 카메라를 Create해준다.
	virtual	BOOL		OnCreateLight			();	// 광원을 생성해서. 두 변수에 설정해준다. m_pLightAmbient , m_pLightDirect
	virtual	AgcWindow *	OnSetMainWindow			();	// 디폴트 윈도우를 설정해주는 메시지.. 윈도우를 초기화해서 포인터 넘기면 됀다.
	virtual	BOOL		OnRegisterModule		();
	virtual BOOL		OnRegisterModulePreCreateWorld		();
	virtual BOOL		OnInit					(); // RW가 초기화 돼기 전에 호출됀다.
	virtual BOOL		OnAddModule				();
	virtual BOOL		OnCameraResize			( RwRect * pRect );
	virtual	BOOL		OnDestroyCamera			();

	virtual VOID		OnWorldRender			(); // World Render 부분에서 AgcmRender를 사용한다.
	virtual void		OnRender				();	// 렌더링 과정 

	virtual	void		OnDestroyLight			();
	virtual BOOL		OnRenderPreCameraUpdate	();
	
	virtual	void		OnTerminate				();
	virtual	VOID		OnEndIdle				();
};

extern MyEngine			g_MyEngine			;

extern	ApmMap				*	g_pcsApmMap					;	// 맵 퍼블릭 모듈..

extern	AgpmCharacter		*	g_pcsAgpmCharacter			;
extern	AgcmCharacter		*	g_pcsAgcmCharacter			;

extern	AgpmItem			*	g_pcsAgpmItem				;
extern	AgpmItemConvert		*	g_pcsAgpmItemConvert		;
extern	AgpmDropItem		*	g_pcsAgpmDropItem			;
extern	AgcmItem			*	g_pcsAgcmItem				;

extern	AgpmUnion			*	g_pcsAgpmUnion				;

extern	AgpmParty			*	g_pcsAgpmParty				;

extern	ApmObject			*	g_pcsApmObject				;
extern	AgcmObject			*	g_pcsAgcmObject				;
extern	AgpmFactors			*	g_pcsAgpmFactors			;

extern	AgcmRender			*	g_pcsAgcmRender				;	// Render Module

extern	ApmEventManager		*	g_pcsApmEventManager		;
extern	AgpmEventNature		*	g_pcsAgpmEventNature		;
extern	AgcmEventNature		*	g_pcsAgcmEventNature		;
extern	AgpmEventSpawn		*	g_pcsAgpmEventSpawn			;
extern	AgpmEventNPCDialog	*	g_pcsAgpmEventNPCDialog		;
extern	AgpmEventNPCTrade	*	g_pcsAgpmEventNPCTrade		;
extern	AgpmAuction			*	g_pcsAgpmEventAuction		;

//extern	AgpmAI				*	g_pcsAgpmAI					;
extern	AgpmAI2				*	g_pcsAgpmAI2				;

//extern	AgcmCamera			*	g_pcsAgcmCamera				;

extern	AgcmShadow			*	g_pcsAgcmShadow				;
extern	AgcmShadow2			*	g_pcsAgcmShadow2			;

extern	AgcmWater			*	g_pcsAgcmWater				;
extern	AgcmFont			*	g_pcsAgcmFont				;

extern	AgpmShrine			*	g_pcsAgpmShrine				;

extern	ApmOcTree			*	g_pcsApmOcTree				;
extern	AgcmOcTree			*	g_pcsAgcmOcTree				;

extern	AgcmTuner			*	g_pcsAgcmTuner				;

extern	AgcmGrass			*	g_pcsAgcmGrass				;

extern	AgpmGrid			*	g_pcsAgpmGrid				;
extern	AgcmLODManager		*	g_pcsAgcmLODManager			;
extern	AgcmSound			*	g_pcsAgcmSound				;
//extern	AgcmEffect			*	g_pcsAgcmEffect				;
extern	AgcmEff2			*	g_pcsAgcmEff2				;

extern	ApmEventManager		*	g_pcsApmEventManager		;
extern	AgpmSkill			*	g_pcsAgpmSkill				;
extern	AgcmSkill			*	g_pcsAgcmSkill				;
extern	AgpmEventTeleport	*	g_pcsAgpmEventTeleport		;
//AgcmImportClientData*	g_pcsAgcmImportClientData	;
extern	AgpmEventNPCDialog	*	g_pcsAgpmEventNPCDialog		;
extern	AgpmEventNPCTrade	*	g_pcsAgpmEventNPCTrade		;
extern	AgcmEventNPCTrade	*	g_pcsAgcmEventNPCTrade		;
extern	AgcmEventEffect		*	g_pcsAgcmEventEffect		;

//AgcmObjectTypeDlg		g_pcsAgcmObjectTypeDlg		;
extern	AgpmTimer			*	g_pcsAgpmTimer				;
extern	AgcmUIControl		*	g_pcsAgcmUIControl			;
extern	AgcmPreLODManager	*	g_pcsAgcmPreLODManager		;
extern	AgcmResourceLoader	*	g_pcsAgcmResourceLoader		;

extern	AgpmPathFind		*		g_pcsAgpmPathFind				;
//extern	AgpmEventMasterySpecialize*	g_pcsAgpmEventMasterySpecialize	;

extern	AgpmEventBank		*	g_pcsAgpmEventBank			;
extern	AgcmEventBank		*	g_pcsAgcmEventBank			;
extern	AgcmEventManager	*	g_pcsAgcmEventManager		;

extern	AgpmEventGuild		*	g_pcsAgpmEventGuild		;
extern	AgcmEventGuild		*	g_pcsAgcmEventGuild		;

extern	AgpmEventProduct		*	g_pcsAgpmEventProduct		;
extern	AgcmEventProduct		*	g_pcsAgcmEventProduct		;

extern	AgpmQuest				*	g_pcsAgpmQuest				;
extern	AgpmEventQuest			*	g_pcsAgpmEventQuest			;

extern	AgpmEventSkillMaster	*	g_pcsAgpmEventSkillMaster	;
extern	AgcmEventSkillMaster	*	g_pcsAgcmEventSkillMaster	;

//@{ Jaewon 20041229
extern AgcmAmbOcclMap		*	g_pcsAgcmAmbOcclMap		;
//@} Jaewon

extern	ApmEventManagerDlg	*	g_pcsApmEventManagerDlg		;
extern	AgcmEventSpawnDlg	*	g_pcsAgcmEventSpawnDlg		;
extern	AgcmEventNatureDlg	*	g_pcsAgcmEventNatureDlg		;
extern	AgcmEventNPCDialogDlg*	g_pcsAgcmEventNPCDialogDlg	;
extern	AgcmEventNPCTradeDlg*	g_pcsAgcmEventNPCTradeDlg	;
extern	AgcmEventAuctionDlg *	g_pcsAgcmEventAuctionDlg	;
//extern	AgcmAIDlg			*	g_pcsAgcmAIDlg				;
extern	AgcmShrineDlg		*	g_pcsAgcmShrineDlg			;
extern	AgcmEventTeleportDlg*	g_pcsAgcmEventTeleportDlg	;
extern	AgcmBlockingDlg		*	g_pcsAgcmBlockingDlg		;
extern	AgcmLODDlg			*	g_pcsAgcmLODDlg				;
extern	AgcmEffectDlg		*	g_pcsAgcmEffectDlg			;
extern	AgcmFactorsDlg		*	g_pcsAgcmFactorsDlg			;
//extern	AgcmAnimationDlg	*	g_pcsAgcmAnimationDlg		;
//extern	AgcmSkillDlg		*	g_pcsAgcmSkillDlg			;
extern	AgcmEventQuestDlg		*	g_pcsAgcmEventQuestDlg		;
extern	AgcmEventSkillMasterDlg	*	g_pcsAgcmEventSkillMasterDlg	;
extern	AgcmWaterDlg		*	g_pcsAgcmWaterDlg			;
extern	AgcmWaterDlg2		*	g_pcsAgcmWaterDlg2			;

extern	AgpmProduct				*	g_pcsAgpmProduct			;
extern	AgcmEventProductDlg		*	g_pcsAgcmEventProductDlg	;
extern	AgcmCamera2				*	g_pcsAgcmCamera2			;
extern  AgcmNatureEffect		*	g_pcsAgcmNatureEffect		;

extern  AgpmEventPointLight		*	g_pcsAgpmEventPointLight	;
extern  AgcmEventPointLight		*	g_pcsAgcmEventPointLight	;

extern  AgpmEventGacha		*	g_pcsAgpmEventGacha		;
extern  AgcmEventGachaDlg	*	g_pcsAgcmEventGachaDlg	;



#endif // !defined(AFX_MYENGINE_H__944AAD22_B83C_4977_A376_A382BE50B1BA__INCLUDED_)
