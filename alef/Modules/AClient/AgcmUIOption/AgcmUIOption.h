#ifndef			_AGCMUIOPTION_H_
#define			_AGCMUIOPTION_H_

#include "AgcModule.h"
#include "AgcmUIManager2.h"

#include "AgcmTuner.h"
#include "AgcmShadow.h"
#include "AgcmShadow2.h"
#include "AgcmEff2.h"
#include "AgcmSound.h"
#include "AcUIToolTip.h"
#include "AgcmRender.h"
#include "AgcmLogin.h"

#include "AgcmGrass.h"
#include "AgcmLensFlare.h"
#include "AgcmEventNature.h"
#include "AgcmWater.h"
#include "AgcmShadowmap.h"
#include "AgcmCharacter.h"
#include "AgpmCharacter.h"
#include "AgcmGuild.h"
#include "AgpmGuild.h"
#include "AgcmUIParty.h"
#include "AgcmUIStatus.h"
#include "AgcmUITips.h"
#include "AgcmMinimap.h"

#include "AgcmUIOptionDefine.h"
#include "AgcmPostFX.h"

#include <d3d9.h>

class AgcmTextBoardMng;

#define	AGCMOPTION_UI_SKIN_TYPE1_PATH		_T("_TYPE1")
#define AGCMOPTION_MAX_SPEAKER_TYPE			6

class CMyCheckBox
{
public:
	CMyCheckBox();
	~CMyCheckBox();

	BOOL	Refresh();
	void	Update();

	BOOL			m_bState;
	AgcdUIUserData*	m_pUserData;
};

class AgcmUIOption : public AgcModule
{
public:
	AgcmUIManager2 *	m_pcsAgcmUIManager2;

	AgcmTuner*			m_pcsAgcmTuner;
	AgcmShadow*			m_pcsAgcmShadow;
	AgcmShadow2*		m_pcsAgcmShadow2;
	AgcmSound*			m_pcsAgcmSound;
	AgcmEff2*			m_pcsAgcmEff2;
	AgcmTextBoardMng*		m_pcsAgcmTextBoardMng;
	AcUIToolTip*		m_pcsAcUIToolTip;
	AgcmRender*			m_pcsAgcmRender;
	AgcmLogin*			m_pcsAgcmLogin;

	AgcmGrass*			m_pcsAgcmGrass;
	AgcmLensFlare*		m_pcsAgcmLensFlare;
	AgcmEventNature*	m_pcsAgcmEventNature;
	AgcmWater*			m_pcsAgcmWater;
	AgcmShadowmap*		m_pcsAgcmShadowMap;
	AgcmMap*			m_pcsAgcmMap;
	AgcmCharacter*		m_pcsAgcmCharacter;
	AgpmCharacter*		m_pcsAgpmCharacter;
	AgpmGuild*			m_pcsAgpmGuild;
	AgcmGuild*			m_pcsAgcmGuild;
	AgcmUIParty*		m_pcsAgcmUIParty;
	AgcmUIStatus*		m_pcsAgcmUIStatus;
	AgcmUITips*			m_pcsAgcmUITips;
	AgcmMinimap*		m_pcsAgcmMinimap;

	AgcmPostFX*			m_pcsAgcmPostFX;

	CHAR *				m_aszNameView[AGCD_OPTION_MAX_VIEW];
	CHAR *				m_aszNameToggle[AGCD_OPTION_MAX_TOGGLE];
	CHAR *				m_aszNameNature[AGCD_OPTION_MAX_NATURE];
	CHAR *				m_aszNameEffect[AGCD_OPTION_MAX_EFFECT];
	CHAR *				m_aszNameEffect2[AGCD_OPTION_MAX_EFFECT2];

	// 화면 설정
	INT32				m_lEventIDAutoTuningEnable;				// 비활성화
	INT32				m_lEventIDAutoTuningDisable;			// 비활성화

	BOOL				m_bAutoTuning;							// 자동 최적화(비활성화)
	AgcdUIUserData*		m_pstUDAutoTuning;						//

	BOOL				m_bSpeedTuning;							// 게임 속도 최적화
	AgcdUIUserData*		m_pstUDSpeedTuning;

	// Video 관련 HW에 따른 Option
	BOOL				m_bWaterHigh;
	BOOL				m_bBloom1, m_bBloom2;
	BOOL				m_bAutoExposure;

	// Video (Main) Setting
	AgcdUIOptionView	m_eCharView;
	AgcdUIUserData*		m_pstUDCharView;

	AgcdUIOptionView	m_eMapView;
	AgcdUIUserData*		m_pstUDMapView;

	AgcdUIOptionToggle	m_eWideView;
	AgcdUIUserData*		m_pstUDWideView;

	AgcdUIOptionNature	m_eNature;
	AgcdUIUserData*		m_pstUDNature;

	AgcdUIOptionEffect	m_eShadow;
	AgcdUIUserData*		m_pstUDShadow;

	AgcdUIOptionEffect	m_eEffect;
	AgcdUIUserData*		m_pstUDEffect;

	AgcdUIOptionEffect	m_eWater;
	AgcdUIUserData*		m_pstUDWater;

	AgcdUIOptionEffect2	m_eBloom;
	AgcdUIUserData*		m_pstUDBloom;

	AgcdUIOptionToggle	m_eAutoExposure;
	AgcdUIUserData*		m_pstUDAutoExposure;

	AgcdUIOptionToggle	m_eMatEffect;
	AgcdUIUserData*		m_pstUDMatEffect;

	AgcdUIOptionToggle	m_eAntiAliasing;
	AgcdUIUserData*		m_pstUDAntiAliasing;

	AgcdUIOptionToggle	m_eImpact;
	AgcdUIUserData*		m_pstUDImpact;

	AgcdUIOptionToggle	m_eCriminal;
	AgcdUIOptionToggle	m_eMurderer;
	AgcdUIOptionToggle	m_eWanted;
	UINT32				m_iScreenBrightMax, m_iScreenBrightCurrent;
	AgcdUIUserData		*m_pstUDScreenBrightMax, *m_pstUDScreenBrightCurrent;

	//. 2005. 9. 20 Nonstopdj
	//. contrast, brightness 값 추가.
	//. 저장되는 곳은 ./ini/COption.ini
	UINT32				m_iScreenContrastMax, m_iScreenContrastCurrent;
	UINT32				m_iScreenBrightnessMax, m_iScreenBrightnessCurrent;

	UINT32				m_iBloomMax, m_iBloomCurrent;
	AgcdUIUserData		*m_pstUDBloomMax, *m_pstUDBloomCurrent;

	// Video (Nature) Setting
	INT32				m_lEventUpdateWater;
	INT32				m_lEventUpdateBloom;
	INT32				m_lEventUpdateAutoExposure;

	AgcdUIOptionNature	m_eNaturalObj;
	AgcdUIUserData*		m_pstUDNaturalObj;

	AgcdUIOptionToggle	m_eLensFlare;
	AgcdUIUserData*		m_pstUDLensFlare;

	AgcdUIOptionToggle	m_eFog;
	AgcdUIUserData*		m_pstUDFog;

	AgcdUIOptionToggle	m_eSky;
	AgcdUIUserData*		m_pstUDSky;
	
	// Video (Shadow) Setting
	AgcdUIOptionEffect	m_eShadowChar;
	AgcdUIUserData*		m_pstUDShadowChar;

	AgcdUIOptionToggle	m_eShadowSelf;
	AgcdUIUserData*		m_pstUDShadowSelf;

	AgcdUIOptionToggle	m_eShadowObject;
	AgcdUIUserData*		m_pstUDShadowObject;

	// Video (Effect) Setting
	AgcdUIOptionEffect	m_eEffectField;
	AgcdUIUserData*		m_pstUDEffectField;

	AgcdUIOptionEffect	m_eEffectCombat;
	AgcdUIUserData*		m_pstUDEffectCombat;

	AgcdUIOptionToggle	m_eEffectUI;
	AgcdUIUserData*		m_pstUDEffectUI;

	AgcdUIOptionToggle	m_eEffectSkill;
	AgcdUIUserData*		m_pstUDEffectSkill;

	// 사운드 설정
	INT32				m_lEventIDBGMEnable, m_lEventIDBGMDisable, m_lEventIDESEnable, m_lEventIDESDisable;

	BOOL				m_bBackgroundSound;
	AgcdUIUserData*		m_pstUDBackgroundSound;
	UINT32				m_iBackgroundSoundMax, m_iBackgroundSoundCurrent;
	AgcdUIUserData		*m_pstUDBackgroundSoundMax, *m_pstUDBackgroundSoundCurrent;
	
	BOOL				m_bEffectSound;
	AgcdUIUserData*		m_pstUDEffectSound;
	UINT32				m_iEffectSoundMax, m_iEffectSoundCurrent;
	AgcdUIUserData		*m_pstUDEffectSoundMax, *m_pstUDEffectSoundCurrent;
	
	BOOL				m_bEnvSound;
	AgcdUIUserData*		m_pstUDEnvSound;
	UINT32				m_iEnvSoundMax, m_iEnvSoundCurrent;
	AgcdUIUserData		*m_pstUDEnvSoundMax, *m_pstUDEnvSoundCurrent;

	eSoundSpeakerMode	m_eSpeakerType;
	CHAR *				m_aszSpeakerType[AGCMOPTION_MAX_SPEAKER_TYPE];
	AgcdUIUserData*		m_pstUDSpeakerType;

	INT32				m_l3DProvider;
	AgcdUIUserData*		m_pstUD3DProvider;

	BOOL				m_bUseEAX;
	AgcdUIUserData*		m_pstUDUseEAX;

	BOOL				m_bViewNameMine, m_bViewNameMonster, m_bViewNameGuild, m_bViewNameParty, m_bViewNameOthers;
	BOOL				m_bRefuseTrade, m_bRefusePartyIn, m_bRefuseGuildIn, m_bRefuseGuildBattle, m_bRefuseBattle;

	BOOL				m_bRefuseGuildRelation;
	BOOL				m_bRefuseBuddy;

	BOOL				m_bVisiblePartyBuffIcon;
	BOOL				m_bVisibleViewHelmet;
	BOOL				m_bVisibleViewItem;

	BOOL				m_bIsAutoPickItem;

	BOOL				m_bMurderer;
	BOOL				m_bWanted;
	BOOL				m_bCriminal;

	AgcdUIUserData		*m_pstUDViewNameMine, *m_pstUDViewNameMonster, *m_pstUDViewNameGuild, *m_pstUDViewNameParty, *m_pstUDViewNameOthers;
	BOOL				m_bViewHelpBaloonTip, m_bViewHelpBaloonChat, m_bViewHelpUclie;
	AgcdUIUserData		*m_pstUDViewHelpBaloonTip, *m_pstUDViewHelpBaloonChat, *m_pstUDViewHelpUclie;
	BOOL				m_bViewSelfBars;
	AgcdUIUserData		*m_pstUDViewSelfBars;
	AgcdUIUserData		*m_pstUDRefuseTrade, *m_pstUDRefusePartyIn, *m_pstUDRefuseGuildIn, *m_pstUDRefuseGuildBattle, *m_pstUDRefuseBattle;

	BOOL				m_bDisableSkillEffect;
	AgcdUIUserData*		m_pstUDDisableSkillEffect;

	AgcdUIUserData*		m_pstUDRefuseGuildRelation;
	AgcdUIUserData*		m_pstUDRefuseBuddy;
	AgcdUIUserData		*m_pstUDVisiblePartyBuffIcon;
	AgcdUIUserData		*m_pstUDVisibleViewHelmet;
	AgcdUIUserData		*m_pstUDVisibleViewItem;

	AgcdUIUserData*		m_pstUDVisibleCriminal;
	AgcdUIUserData*		m_pstUDVisibleWanted;
	AgcdUIUserData*		m_pstUDVisibleMurderer;
		
	// Patch Backup용
	UINT32				m_iScreenWidth;
	UINT32				m_iScreenHeight;
	UINT32				m_iScreenDepth;

	AgcdUIOptionTexture	m_eTQCharacter;
	AgcdUIOptionTexture	m_eTQObject;
	AgcdUIOptionTexture	m_eTQEffect;
	AgcdUIOptionTexture	m_eTQWorld;

	CHAR				m_strSound[64];
	CHAR				m_strSpeaker[64];

	BOOL				m_bEnableGamma;

	BOOL				m_bDisableOtherPlayerSkillEffect;
	AgcdUIUserData*		m_pstUDDisableOtherPlayerSkillEffect;

	BOOL				m_bSave;			// 내부적으로 사용
	BOOL				m_bSaveLoad;		// 외부에서 세팅
	BOOL				m_bAutoPatchInitialized;

	BOOL				m_bWindowed;

	UINT32				m_ulUISkinType;

	AgcmOptionStartup	m_eStartupMode;

	INT32	m_nNearRough;
	INT32	m_nNearDetail;
	INT32	m_nNormalRough;
	INT32	m_nNormalDetail;
	INT32	m_nFarRough;
	INT32	m_nFarDetail;
	INT32	m_nAllRough;
	INT32	m_nAllDetail;

public:
	AgcmUIOption();
	~AgcmUIOption();

public:
	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	AgcdUIUserData*	AddOptionUserData(AgcdUIOptionType eType, CHAR *szName, AgcUICallBack fnCallback);

	BOOL	LoadFromINI( char* szFile);
	BOOL	SaveToINI( char* szFileName);

	void	InitGameOption();
	void	UpdateGameOption();

	void	UpdateAutoTuning(BOOL	bRefresh);
	void	UpdateSpeedTuning(BOOL	bRefresh);
	void	UpdateDisableOtherPlayerSkillEffect( BOOL bRefresh );
	void	UpdateCharView(BOOL	bRefresh);
	void	UpdateMapView(BOOL	bRefresh);
	void	UpdateWideView(BOOL	bRefresh);
	void	UpdateNature(BOOL	bRefresh);
	void	UpdateShadow(BOOL	bRefresh);
	void	UpdateEffect(BOOL	bRefresh);
	void	UpdateWater(BOOL	bRefresh);
	void	UpdateBloom(BOOL	bRefresh);
	void	UpdateAutoExposure(BOOL	bRefresh);
	void	UpdateMatEffect(BOOL	bRefresh);
	void	UpdateAntiAliasing(BOOL	bRefresh);
	void	UpdateImpact(BOOL	bRefresh);
	void	UpdateBloomVal(BOOL	bRefresh);
	void	UpdateBrightVal(BOOL	bRefresh);

	void	UpdateNaturalObj(BOOL	bRefresh);
	void	UpdateLensFlare(BOOL	bRefresh);
	void	UpdateFog(BOOL	bRefresh);
	void	UpdateSky(BOOL	bRefresh);

	void	UpdateShadowChar(BOOL	bRefresh);
	void	UpdateShadowSelf(BOOL	bRefresh);
	void	UpdateShadowObject(BOOL	bRefresh);

	void	UpdateEffectField	(BOOL	bRefresh);
	void	UpdateEffectCombat	(BOOL	bRefresh);
	void	UpdateEffectUI		(BOOL	bRefresh);
	void	UpdateEffectSkill	(BOOL	bRefresh);
	
	void	UpdateBGMSoundEnable(BOOL	bRefresh);
	void	UpdateBGMSoundVal(BOOL	bRefresh);
	void	UpdateEffSoundEnable(BOOL	bRefresh);
	void	UpdateEffSoundVal(BOOL	bRefresh);
	void	UpdateEnvSoundEnable(BOOL	bRefresh);
	void	UpdateEnvSoundVal(BOOL	bRefresh);
	void	UpdateUseEAX(BOOL	bRefresh);
	void	UpdateSpeakerType(BOOL	bRefresh);
	void	Update3DProvider(BOOL	bRefresh);
	
	void	UpdateViewNameMine(BOOL	bRefresh);
	void	UpdateViewNameMonster(BOOL	bRefresh);
	void	UpdateViewNameGuild(BOOL	bRefresh);
	void	UpdateViewNameParty(BOOL	bRefresh);
	void	UpdateViewNameOthers(BOOL	bRefresh);
	void	UpdateViewHelpBaloonTip(BOOL	bRefresh);
	void	UpdateViewHelpBaloonChat(BOOL	bRefresh);
	void	UpdateViewHelpUclie(BOOL	bRefresh);
	void	UpdateViewSelfBars(BOOL	bRefresh);
	void	UpdatePartyBuffIcon(BOOL	bRefresh);
	void	UpdateViewHelmet(BOOL	bRefresh);
	void	UpdateViewItem(BOOL	bRefresh);

	void	UpdateCriminal	(BOOL	bRefresh);
	void	UpdateWanted	(BOOL	bRefresh);
	void	UpdateMurderer	(BOOL	bRefresh);

	VOID	RefreshRefuseControl(INT32 lOptionFlag);
	VOID	RefreshRefuseControl();

	void	EnableSaveLoad(BOOL bSaveLoad = TRUE)			{	m_bSaveLoad = bSaveLoad;		}
	void	SetStartMode(AgcmOptionStartup eStartupMode)	{	m_eStartupMode = eStartupMode;	}

	// 항목별로 Texture 품질을 return한다.
	AgcdUIOptionTexture	GetTextureQualityCharacter()	{ return m_eTQCharacter;	}
	AgcdUIOptionTexture	GetTextureQualityObject()		{ return m_eTQObject;		}
	AgcdUIOptionTexture	GetTextureQualityEffect()		{ return m_eTQEffect;		}
	AgcdUIOptionTexture	GetTextureQualityWorld()		{ return m_eTQWorld;		}

	// 내 캐릭터의 스킬이펙트를 끌 것인가?
	BOOL	IsDisableMySkillEffect( void )				{ return m_bDisableSkillEffect; }

	// 다른 플레이어의 스킬이펙트를 끌 것인가?
	BOOL	IsDisableOtherSkillEffect( void )			{ return m_bDisableOtherPlayerSkillEffect; }

	void	AddSystemMessageForSkillEffect( BOOL bIsOFF );
	void	AddSystemMessageForOtherSkillEffect( BOOL bIsOFF );

	static BOOL		CBLoginProcessEnd( PVOID pData, PVOID pClass, PVOID pCustData )		;	// Login 과정이 끝났다

	static BOOL		CBAutoTuning(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSpeedTuning(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBCharView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBMapView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBWideView(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBNature(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBShadow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBWater(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBBloom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBAutoExposure(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBMatEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBAntiAliasing(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBImpact(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBBloomVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBNaturalObj(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBLensFlare(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBFog(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSky(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBCriminal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBWanted(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBMurderer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBShadowChar(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBShadowSelf(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBShadowObject(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBEffectField	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectCombat	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectUI		(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectSkill	(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	static BOOL		CBBrightVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBBackgroundSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBBackgroundSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEffectSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEnvSoundEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBEnvSoundVal(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBUseEAX(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBSpeakerType(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CB3DProvider(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBViewNameMine(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewNameMonster(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewNameGuild(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewNameParty(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewNameOthers(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewHelpBaloonTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewHelpBaloonChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewHelpUclie(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBViewSelfBars(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBRefuseTrade(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefusePartyIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseGuildIn(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseGuildBattle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseBattle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseGuildRelation(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBRefuseBuddy(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBDisableSkillEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL		CBVisiblePartyBuffIcon(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBVisibleViewHelmet(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL		CBVisibleViewItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	static BOOL		CBSettingCharacterOK( PVOID pData, PVOID pClass, PVOID pCustData );	
	static BOOL		CBUpdateOptionFlag( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL		CBSystemMessage( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL		CBDisplay3DProvider(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	static BOOL		CBDisableOtherPlayerSkillEffect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);	

protected:
	VOID			SendOptionFlag(INT32 lOptionFlag, BOOL bClick);
	VOID			SendOptionFlag();

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//. 2005. 09. 20 Nonstopdj
	//. script 엔진에 등록될 gamma correction method
	void			SetScreenContrast( int nContrast )
	{
		m_iScreenContrastCurrent = nContrast;
		UpdateBrightVal(true);	//. 적용
	}

	void			SetScreenBrightness( int nBrightness )
	{
		m_iScreenBrightnessCurrent = nBrightness;
		UpdateBrightVal(true);	//. 적용
	}

	void			SetScreenGamma( int nGamma )
	{
		m_iScreenBrightCurrent = nGamma;
		UpdateBrightVal(true);	//. 적용
	}
};

#endif       //_AGCMUIOPTION_H_	