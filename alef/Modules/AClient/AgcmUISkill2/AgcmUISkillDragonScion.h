#ifndef __AGCM_UI_SKILL_DRAGONSCION_H__
#define __AGCM_UI_SKILL_DRAGONSCION_H__

#include "AgcdUIManager2.h"
#include "AgcmUISkillDragonScionDef.h"
#include "AgpmEventSkillMaster.h"
#include "AgcmHeroicSkill.h"

class AgcmCharacter;
class AgcmUIManager2;
class AgcmEventSkillMaster;
class AgpmFactors;
class AgpmCharacter;
class AgpmItem;
class AgcmSkill;
class AgcmUICharacter;
class AgcmUIMain;
class AgcmUIItem;
class AgpmSkill;
class AgcmUISkill2;

class AgcmUISkillDragonScion
{
private :
	eDragonScionSkillUIType								m_eCurrentUI;				//	스킬 창
	eDragonScionSkillUIType								m_eCurrentUpUI;				//	스킬 업그레이드 창

	eDragonScionSkillUIType								m_eLastUI;					//	마지막 스킬 창	
	eDragonScionSkillUIType								m_eLastUpUI;				//	마지막 스킬 업그레이드 창

	// 마지막에 열린창이 뭔지 알려준다( SkillPoint Display에서 쓴다 )
	// 1 = 스킬창 , 2 = 스킬업그레이드창
	INT32												m_nLastOpen;				

	// 스킬창 그리드
	AgpdGrid											m_ScionSkillGrid			[ SKILL_SCION_COUNT		];
	AgpdGrid											m_SlayerSkillGrid			[ SKILL_SLAYER_COUNT	];
	AgpdGrid											m_SummernerSkillGrid		[ SKILL_SUMMENER_COUNT	];
	AgpdGrid											m_ObiterSkillGrid			[ SKILL_OBITER_COUNT	];
	AgpdGrid											m_PassiveSkillGrid			[ SKILL_PASSIVE_COUNT	];
	AgpdGrid											m_ArchlordSkillGrid			[ DRAGONSCION_ARCHLORD_SKILL_COUNT	];

	// 스킬창 UserData
	AgcdUIUserData*										m_pScionSkillUserData		[ SKILL_SCION_COUNT		];
	AgcdUIUserData*										m_pSlayerSkillUserData		[ SKILL_SLAYER_COUNT	];
	AgcdUIUserData*										m_pSummernerSkillUserData	[ SKILL_SUMMENER_COUNT	];
	AgcdUIUserData*										m_pObiterSkillUserData		[ SKILL_OBITER_COUNT	];
	AgcdUIUserData*										m_pPassiveSkillUserData		[ SKILL_PASSIVE_COUNT	];
	AgcdUIUserData*										m_pArchlordSkillUserData;

	AgcdUIUserData*										m_pScionDisplayUserData		[ SKILL_SCION_COUNT		];
	AgcdUIUserData*										m_pSlayerDisplayUserData	[ SKILL_SLAYER_COUNT	];
	AgcdUIUserData*										m_pSummernerDisplayUserData	[ SKILL_SUMMENER_COUNT	];
	AgcdUIUserData*										m_pObiterDisplayUserData	[ SKILL_OBITER_COUNT	];
	AgcdUIUserData*										m_pPassiveDisplayUserData	[ SKILL_PASSIVE_COUNT	];

	INT32												m_nScionDisplayUserData		[ SKILL_SCION_COUNT		];
	INT32												m_nSlayerDisplayUserData	[ SKILL_SLAYER_COUNT	];
	INT32												m_nSummernerDisplayUserData	[ SKILL_SUMMENER_COUNT	];
	INT32												m_nObiterDisplayUserData	[ SKILL_OBITER_COUNT	];
	INT32												m_nPassiveDisplayUserData	[ SKILL_PASSIVE_COUNT	];

	// 스킬 업그레이드창 그리드
	AgpdGrid											m_ScionSkillUpGrid			[ SKILL_SCION_COUNT		];
	AgpdGrid											m_SlayerSkillUpGrid			[ SKILL_SLAYER_COUNT	];
	AgpdGrid											m_SummernerSkillUpGrid		[ SKILL_SUMMENER_COUNT	];
	AgpdGrid											m_ObiterSkillUpGrid			[ SKILL_OBITER_COUNT	];
	AgpdGrid											m_PassiveSkillUpGrid		[ SKILL_PASSIVE_COUNT	];

	// 스킬 업그레이드창 UserData
	AgcdUIUserData*										m_pScionSkillUpUserData		[ SKILL_SCION_COUNT		];
	AgcdUIUserData*										m_pSlayerSkillUpUserData	[ SKILL_SLAYER_COUNT	];
	AgcdUIUserData*										m_pSummernerSkillUpUserData	[ SKILL_SUMMENER_COUNT	];
	AgcdUIUserData*										m_pObiterSkillUpUserData	[ SKILL_OBITER_COUNT	];
	AgcdUIUserData*										m_pPassiveSkillUpUserData	[ SKILL_PASSIVE_COUNT	];

	AgcdUIUserData*										m_pScionDisplayUpUserData	[ SKILL_SCION_COUNT		];
	AgcdUIUserData*										m_pSlayerDisplayUpUserData	[ SKILL_SLAYER_COUNT	];
	AgcdUIUserData*										m_pSummernerDisplayUpUserData[ SKILL_SUMMENER_COUNT	];
	AgcdUIUserData*										m_pObiterDisplayUpUserData	[ SKILL_OBITER_COUNT	];
	AgcdUIUserData*										m_pPassiveDisplayUpUserData	[ SKILL_PASSIVE_COUNT	];

	INT32												m_nScionDisplayUpUserData	[ SKILL_SCION_COUNT		];
	INT32												m_nSlayerDisplayUpUserData	[ SKILL_SLAYER_COUNT	];
	INT32												m_nSummernerDisplayUpUserData[ SKILL_SUMMENER_COUNT	];
	INT32												m_nObiterDisplayUpUserData	[ SKILL_OBITER_COUNT	];
	INT32												m_nPassiveDisplayUpUserData	[ SKILL_PASSIVE_COUNT	];

	//---------------------------------------------------------------------------------------------------------------------
	// --------------Skill 상태변경 이벤트---------------------------------------------------------------------------------
	INT32												m_nEventScionSelectedItem				[ SKILL_SCION_COUNT		];
	INT32												m_nEventSlayerSelectedItem				[ SKILL_SLAYER_COUNT	];
	INT32												m_nEventObiterSelectedItem				[ SKILL_OBITER_COUNT	];
	INT32												m_nEventPassiveSelectedItem				[ SKILL_PASSIVE_COUNT	];
	INT32												m_nEventSummernerSelectedItem			[ SKILL_SUMMENER_COUNT	];

	INT32												m_nEventScionUnSelectedItem				[ SKILL_SCION_COUNT		];
	INT32												m_nEventSlayerUnSelectedItem			[ SKILL_SLAYER_COUNT	];
	INT32												m_nEventObiterUnSelectedItem			[ SKILL_OBITER_COUNT	];
	INT32												m_nEventPassiveUnSelectedItem			[ SKILL_PASSIVE_COUNT	];
	INT32												m_nEventSummernerUnSelectedItem			[ SKILL_SUMMENER_COUNT	];

	INT32												m_nEventScionUpgradeSelectedItem		[ SKILL_SCION_COUNT		];
	INT32												m_nEventSlayerUpgradeSelectedItem		[ SKILL_SLAYER_COUNT	];
	INT32												m_nEventObiterUpgradeSelectedItem		[ SKILL_OBITER_COUNT	];
	INT32												m_nEventPassiveUpgradeSelectedItem		[ SKILL_PASSIVE_COUNT	];
	INT32												m_nEventSummernerUpgradeSelectedItem	[ SKILL_SUMMENER_COUNT	];

	INT32												m_nEventScionUpgradeUnSelectedItem		[ SKILL_SCION_COUNT		];
	INT32												m_nEventSlayerUpgradeUnSelectedItem		[ SKILL_SLAYER_COUNT	];
	INT32												m_nEventObiterUpgradeUnSelectedItem		[ SKILL_OBITER_COUNT	];
	INT32												m_nEventPassiveUpgradeUnSelectedItem	[ SKILL_PASSIVE_COUNT	];
	INT32												m_nEventSummernerUpgradeUnSelectedItem	[ SKILL_SUMMENER_COUNT	];

	INT32												m_nEventScionActiveItem					[ SKILL_SCION_COUNT		];
	INT32												m_nEventSlayerActiveItem				[ SKILL_SLAYER_COUNT	];
	INT32												m_nEventObiterActiveItem				[ SKILL_OBITER_COUNT	];
	INT32												m_nEventPassiveActiveItem				[ SKILL_PASSIVE_COUNT	];
	INT32												m_nEventSummernerActiveItem				[ SKILL_SUMMENER_COUNT	];

	INT32												m_nEventScionUpgradeActiveItem			[ SKILL_SCION_COUNT		];
	INT32												m_nEventSlayerUpgradeActiveItem			[ SKILL_SLAYER_COUNT	];
	INT32												m_nEventObiterUpgradeActiveItem			[ SKILL_OBITER_COUNT	];
	INT32												m_nEventPassiveUpgradeActiveItem		[ SKILL_PASSIVE_COUNT	];
	INT32												m_nEventSummernerUpgradeActiveItem		[ SKILL_SUMMENER_COUNT	];

	INT32												m_nEventScionDisableItem				[ SKILL_SCION_COUNT		];
	INT32												m_nEventSlayerDisableItem				[ SKILL_SLAYER_COUNT	];
	INT32												m_nEventObiterDisableItem				[ SKILL_OBITER_COUNT	];
	INT32												m_nEventPassiveDisableItem				[ SKILL_PASSIVE_COUNT	];
	INT32												m_nEventSummernerDisableItem			[ SKILL_SUMMENER_COUNT	];

	INT32												m_nEventScionUpgradeDisableItem			[ SKILL_SCION_COUNT		];
	INT32												m_nEventSlayerUpgradeDisableItem		[ SKILL_SLAYER_COUNT	];
	INT32												m_nEventObiterUpgradeDisableItem		[ SKILL_OBITER_COUNT	];
	INT32												m_nEventPassiveUpgradeDisableItem		[ SKILL_PASSIVE_COUNT	];
	INT32												m_nEventSummernerUpgradeDisableItem		[ SKILL_SUMMENER_COUNT	];
	//--------------------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------

	INT32												m_nEventOpenUIScion;
	INT32												m_nEventOpenUISlayer;
	INT32												m_nEventOpenUIObiter;
	INT32												m_nEventOpenUISummerner;
	INT32												m_nEventOpenUIPassive;
	INT32												m_nEventOpenUIArchlord;

	INT32												m_nEventCloseUIScion;
	INT32												m_nEventCloseUISlayer;
	INT32												m_nEventCloseUIObiter;
	INT32												m_nEventCloseUISummerner;
	INT32												m_nEventCloseUIPassive;
	INT32												m_nEventCloseUIArchlord;

	INT32												m_nEventOpenUpUIScion	[ EVENT_COUNT ];
	INT32												m_nEventCloseUpUIScion	[ EVENT_COUNT ];

	INT32												m_nArrSelectIndex[ SkillUI_Count	];
	INT32												m_nArrUpgradeSelectIndex[ SkillUI_Count ];

	INT32												m_nArchlordSkillSelectIndex;

	INT32												m_nArchlordSkillSelect;
	INT32												m_nArchlordSkillUnSelect;

	INT32												m_nArchlordSkillActive;
	INT32												m_nArchlordSkillDisable;

	INT32												m_nScionSkillPoint;
	AgcdUIUserData*										m_pScionSkillPointUserData;

	BOOL												m_bScionActiveRollbackButton;
	AgcdUIUserData*										m_pScionActiveRollbackButtonUserData;

	ApdEvent*											m_pcsEvent;

	AgcmUISkill2*										m_pAgcmUISkill2;

	AgcmUIManager2*										m_pcsAgcmUIManager2;
	AgcmCharacter*										m_pcsAgcmCharacter;
	AgpmGrid*											m_pcsAgpmGrid;
	AgpmSkill*											m_pcsAgpmSkill;
	AgpmEventSkillMaster*								m_pcsAgpmEventSkillMaster;
	AgpmFactors*										m_pcsAgpmFactors;
	AgpmCharacter*										m_pcsAgpmCharacter;
	AgpmItem*											m_pcsAgpmItem;
	AgcmSkill*											m_pcsAgcmSkill;
	AgcmEventSkillMaster*								m_pcsAgcmEventSkillMaster;
	AgcmUICharacter*									m_pcsAgcmUICharacter;
	AgcmUIMain*											m_pcsAgcmUIMain;
	AgcmUIItem*											m_pcsAgcmUIItem;

public :
	AgcmUISkillDragonScion	( VOID );
	~AgcmUISkillDragonScion	( VOID );

	virtual BOOL				OnAddModule						( AgcmUISkill2* pAgcmUISkill );
	virtual BOOL				OnInit							( VOID );
	virtual	BOOL				OnDestroy						( VOID );

	VOID						OnAddEvent						( VOID* pUIManager );
	VOID						OnAddFunction					( VOID* pUIManager );
	VOID						OnAddDisplay					( VOID* pUIManager );
	VOID						OnAddUserData					( VOID* pUIManager );
	VOID						OnAddBoolean					( VOID* pUIManager );
	
	BOOL						OnOpenUI						( eDragonScionSkillUIType eOpenType );
	BOOL						OnOpenUpUI						( eDragonScionSkillUIType eOpenType );

	BOOL						OnCloseUI						( VOID );
	BOOL						OnCloseUpUI						( VOID );

	BOOL						OpenSkillUI						( eDragonScionSkillUIType eType );
	BOOL						OpenSkillUpUI					( eDragonScionSkillUIType eType );

	BOOL						CloseSkillUI					( VOID );
	BOOL						CloseSkillUpUI					( VOID );

	BOOL						IsCurScionWindowOpen			( VOID );

	inline	VOID*				GetCurrentBaseControl			( VOID );
	inline	AgcmUIManager2*		GetUIManager					( VOID );
	inline	AgcmCharacter*		GetAgcmCharacter				( VOID );
	inline	AgpmSkill*			GetAgpmSkill					( VOID );
	inline	AgpmGrid*			GetAgpmGrid						( VOID );
	inline	AgpmCharacter*		GetAgpmCharacter				( VOID );

	static BOOL					CallBackGetBaseControlScion		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CallBackGetBaseControlSlayer	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CallBackGetBaseControlObiter	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CallBackGetBaseControlSummerner	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CallBackGetBaseControlPassive	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CallBackGetBaseControlHeroic	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CallBackArchlord				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBOpenScionUpgrade				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBOpenSlayerUpgrade				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBOpenObiterUpgrade				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBOpenSummernerUpgrade			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBOpenPassiveUpgrade			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBOpenHeroicUpgrade				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CallBackUIScroll				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBCloseScionSkillUI				( VOID* pClass , VOID* pData1 , VOID* pData2 , VOID* pData3 , VOID* pData4 , VOID* pData5 , ApBase* pTarget , AgcdUIControl* pControl );
	static BOOL					CBCloseScionUPSkillUI			( VOID* pClass , VOID* pData1 , VOID* pData2 , VOID* pData3 , VOID* pData4 , VOID* pData5 , ApBase* pTarget , AgcdUIControl* pControl );

	// Scion Display
	static BOOL					CBDisplaySkillScion0			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion1_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion1_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion1_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion2_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion2_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion2_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion3_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion3_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion4_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion4_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion4_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion5_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion5_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion5_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion6_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion6_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillScion6_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Obiter Display
	static BOOL					CBDisplaySkillObiter1_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter1_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter1_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter1_4			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter1_5			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter2_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter2_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter2_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter3_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter3_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter3_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillObiter3_4			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Slayer Display
	static BOOL					CBDisplaySkillSlayer1_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer1_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer1_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer1_4			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer2_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer2_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer2_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer2_4			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer3_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer3_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSlayer3_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Summerner Display
	static BOOL					CBDisplaySkillSummener1_1		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener1_2		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener1_3		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener1_4		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener1_5		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener2_1		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener2_2		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener2_3		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener2_4		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener3_1		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener3_2		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillSummener3_3		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Passive Display
	static BOOL					CBDisplaySkillPassive1_1		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive1_2		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive1_3		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive2_1		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive2_2		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive2_3		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive3_1		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive3_2		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive3_3		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive4_1		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive4_2		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive4_3		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive5_1		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive5_2		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillPassive5_3		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Heroic Display
	static BOOL					CBDisplaySkillHeroic1_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic2_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic2_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic3_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic3_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic3_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic4_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic4_2			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic4_3			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );
	static BOOL					CBDisplaySkillHeroic5_1			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	// Scion의 남은 스킬포인트
	static BOOL					CBDisplayScionSkillPoint		( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue );

	static BOOL					CBScionSkillUpgrade				( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSlayerSkillUpgrade			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBObiterSkillUpgrade			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBPassiveSkillUpgrade			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSummernerSkillUpgrade			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBScionSkillRollBack			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSlayerSkillRollBack			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBObiterSkillRollBack			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBPassiveSkillRollBack			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSummernerSkillRollBack		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBSelectScionSkillGrid			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectBuyScionSkillGrid		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectUpgradeScionSkillGrid	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBSelectSlayerSkillGrid			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectBuySlayerSkillGrid		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectUpgradeSlayerSkillGrid	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBSelectObiterSkillGrid			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectBuyObiterSkillGrid		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectUpgradeObiterSkillGrid	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBSelectPassiveSkillGrid		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectBuyPassiveSkillGrid		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectUpgradePassiveSkillGrid	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBSelectSummernerSkillGrid		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectBuySummernerSkillGrid	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectUpgradeSummernerSkillGrid( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBSelectHeroicSkillGrid			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectBuyHeroicSkillGrid		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBSelectUpgradeHeroicSkillGrid	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBSelectArchlordSkillGrid		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBOpenScionSkillToolTip			( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBCloseScionSkillToolTip		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBOpenUpgradeScionSkillToolTip	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBCloseUpgradeScionSkillToolTip	( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBOpenBuyScionSkillToolTip		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );
	static BOOL					CBCloseBuyScionSkillToolTip		( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl );

	static BOOL					CBBuyScionSkillResult			(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL					CBUpgradeScionSkillResult		(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL					CBUpdateScionSkill				(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL					CBUpdateScionCost				(PVOID pData, PVOID pClass, PVOID pCustData);
	
	VOID						LoadSkill						( eDragonScionSkillUIType		eType );				//	해당 Type UI의 스킬 정보를 읽어온다
	VOID						LoadBuySkill					( eDragonScionSkillUIType		eType );				//	해당 Type UI의 스킬업그레이드 정보를 읽어온다
	VOID						UnSelectSkillUI					( eDragonScionSkillUIType		eType );				//	해당 Type UI의 선택된 스킬 선택해제
	VOID						UnSelectUpgradeSkillUI			( eDragonScionSkillUIType		eType );				//  해당 Type UI의 선택된 스킬 선택해제

	// 해당 스킬이 업그레이드가 가능한지 확인
	eDragonScionSKill_Status	IsActiveUpgradeItem				( INT32 lSkillTID	);
	BOOL						CheckRollbackSkill				( INT32 lSkillTID	);

private:
	// 스킬 창의 스킬정보를 얻어온다
	VOID						_LoadScionSkill					( VOID );
	VOID						_LoadSlayerSkill				( VOID );
	VOID						_LoadObiterSkill				( VOID );
	VOID						_LoadSummernerSkill				( VOID );
	VOID						_LoadPassiveSkill				( VOID );
	VOID						_LoadArchlordSkill				( VOID );

	// 스킬 업그레이드 정보를 얻어온다
	VOID						_LoadScionSkillBuy				( VOID );
	VOID						_LoadSlayerSkillBuy				( VOID );
	VOID						_LoadObiterSkillBuy				( VOID );
	VOID						_LoadSummernerSkillBuy			( VOID );
	VOID						_LoadPassiveSkillBuy			( VOID );

	// 선택된 스킬창 선택 해제
	VOID						_UnSelectSkillScion				( VOID );
	VOID						_UnSelectSkillSlayer			( VOID );
	VOID						_UnSelectSkillObiter			( VOID );
	VOID						_UnSelectSkillSummerner			( VOID );
	VOID						_UnSelectSkillPassive			( VOID );
	VOID						_UnSelectSkillArchlord			( VOID );

	//////////////////////////////////////////////////////////////////////////////
	// 시온 종족의 직업별로 인덱스번호를 이용해 스킬 포인트를 반환
	// 일반 스킬창이 열릴경우 추가되는 스킬포인트를 합산해서 반환
	INT32						_GetScionSkillPoint				( INT32 nIndex );
	INT32						_GetSlayerSkillPoint			( INT32 nIndex );
	INT32						_GetObiterSkillPoint			( INT32 nIndex );
	INT32						_GetSummernerSkillPoint			( INT32 nIndex );
	INT32						_GetPassiveSkillPoint			( INT32	nIndex );
	//////////////////////////////////////////////////////////////////////////////

	// 시온 종족의 스킬 정보 반환
	AgpdEventSkillHighLevel		_GetScionSkill					( INT32 nIndex );
	AgpdEventSkillHighLevel		_GetSlayerSkill					( INT32 nIndex );
	AgpdEventSkillHighLevel		_GetObiterSkill					( INT32 nIndex );
	AgpdEventSkillHighLevel		_GetSummernerSkill				( INT32 nIndex );
	AgpdEventSkillHighLevel		_GetPassiveSkill				( INT32 nindex );

	// 해당 타입의 SKILL TID를 이용해 HIghLevel 스킬 반환
	AgpdEventSkillHighLevel		_GetSkill						( eDragonScionSkillUIType	eScionType , INT32 nTID );
	
	INT32						_GetCharacterTID				( eDragonScionSkillUIType	eScionType );

	// 해당 타입의 SKILL TID를 이용해 Index를 반환
	INT32						_GetSkillIndex					( eDragonScionSkillUIType	eScionType , INT32 nTID	);

	// 각 타입의 UI의 윈도우가 나오는 위치를 바로 전 UI로 맞춘다
	BOOL						_MoveSkillWindow				( eDragonScionSkillUIType		eType );
	BOOL						_MoveSkillUpgradeWindow			( eDragonScionSkillUIType		eType );

	// 해당 타입 UI의 포인터를 반환( 두번째 인자에 TRUE를 주면.. UpgradeUI 포인터를 반환 )
	AgcdUI*						_GetUI							( eDragonScionSkillUIType		eType	,	BOOL bUpgradeUI	= FALSE );
};

#endif