//#pragma once

#ifndef		_ACUICONTROL_H_
#define		_ACUICONTROL_H_

#include "AgcModule.h"
#include "AgcmSound.h"
#include "AgcmFont.h"
#include "AuIniManager.h"
#include "AcUIControlHeader.h"
#include "AgpmGrid.h"

#define	AGCMUICONTROL_KEY_NAME_BUTTON_CLICK_SOUND				"button_click"
#define	AGCMUICONTROL_KEY_NAME_BASE								"base"
#define AGCMUICONTROL_KEY_NAME_TT_BACK							"tt_back"
#define AGCMUICONTROL_KEY_NAME_TT_TLINE							"tt_tline"
#define AGCMUICONTROL_KEY_NAME_TT_BLINE							"tt_bline"
#define AGCMUICONTROL_KEY_NAME_TT_LLINE							"tt_lline"	
#define AGCMUICONTROL_KEY_NAME_TT_RLINE							"tt_rline"
#define AGCMUICONTROL_KEY_NAME_TT_TLCORNER						"tt_tlcorner"	
#define AGCMUICONTROL_KEY_NAME_TT_TRCORNER						"tt_trcorner"
#define AGCMUICONTROL_KEY_NAME_TT_BLCORNER						"tt_blcorner"
#define AGCMUICONTROL_KEY_NAME_TT_BRCORNER						"tt_brcorner"
#define	AGCMUICONTROL_KEY_NAME_COMBO_BACK						"combo_back"
#define AGCMUICONTROL_KEY_NAME_COMBO_BUTTON						"combo_button"
#define AGCMUICONTROL_KEY_NAME_MESSAGE_BACK						"message_back"		//"MBox_Back.png"
#define AGCMUICONTROL_KEY_NAME_MESSAGE_BUTTON0					"message_button0"	//"MBox_ButtonA.png"
#define AGCMUICONTROL_KEY_NAME_MESSAGE_BUTTON1					"message_button1"	//"MBox_ButtonB.png"
#define AGCMUICONTROL_KEY_NAME_MESSAGE_BUTTON2					"message_button2"	//"MBox_ButtonC.png"
#define	AGCMUICONTROL_KEY_NAME_CONVERT_BASE						"convert_base"
#define	AGCMUICONTROL_KEY_NAME_CONVERT_DISABLE					"convert_disable"
#define	AGCMUICONTROL_KEY_NAME_CONVERT_OUTLINE					"convert_outline"
#define	AGCMUICONTROL_KEY_NAME_PVP_FREE							"pvp_free"
#define	AGCMUICONTROL_KEY_NAME_PVP_DANGER						"pvp_danger"
#define AGCMUICONTROL_KEY_NAME_CANCEL_TRANSFORM					"cancel_transform"
#define AGCMUICONTROL_KEY_NAME_CANCEL_EVOLUTION					"cancel_evolution"
#define	AGCMUICONTROL_KEY_NAME_DURABILITY_ZERO					"durability_zero"
#define AGCMUICONTROL_KEY_NAME_CATAPULT_USE						"catapult_use"
#define	AGCMUICONTROL_KEY_NAME_CATAPULT_REPAIR					"catapult_repair"
#define AGCMUICONTROL_KEY_NAME_ATKRESTOWER_USE					"atk_res_tower_use"
#define	AGCMUICONTROL_KEY_NAME_ATKRESTOWER_REPAIR				"atk_res_tower_repair"
#define	AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_HUMAN				"humancastleowner"
#define	AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_ORC					"orccastleowner"
#define	AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_MOONELF				"moonelfcastleowner"
#define AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_DRAGONSCION			"dragonscioncastleowner"
#define	AGCMUICONTROL_KEY_NAME_CASTLE_OWNER_ARCHLORD			"archlordcastleowner"
#define	AGCMUICONTROL_KEY_NAME_ARCHLORD_GUILD_MARK				"archlordguildmark"
#define AGCMUICONTROL_KEY_NAME_GO_MARK							"go_mark"
#define AGCMUICONTROL_KEY_NAME_PET_STATE						"pet_state"
#define AGCMUICONTROL_KEY_NAME_HANGAME_S						"hangame_s_pc"
#define AGCMUICONTROL_KEY_NAME_HANGAME_T						"hangame_t_pack"

#define AGCMUICONTROL_MESSAGE_DIALOG_X							400
#define	AGCMUICONTROL_MESSAGE_DIALOG_Y							273
#define AGCMUICONTROL_MESSAGE_WIDTH								412
#define	AGCMUICONTROL_MESSAGE_HEIGHT							180	

#define AGCMUICONTROL_TEXTURE_LENGTH							64

// ----------------- CUIOpenCloseSystem ------------------
class CUIOpenCloseSystem
{
public:
	CUIOpenCloseSystem( void );
	~CUIOpenCloseSystem( void );

	void						Init							( AgcmUIManager2* pUIMgr, CHAR* szUIName );

	BOOL						IsOpen							( void );
	BOOL						Open							( void );
	BOOL						Close							( void );

private:
	AgcmUIManager2*												m_pcsAgcmUIManager2;
	CHAR														m_szUIName[ MAX_PATH ];
};

// ----------------- AgcdUICustomTexture ------------------
struct AgcdUICustomTexture
{
	AgcdUICustomTexture( void )
	{
		memset( szName, 0, sizeof( szTextureName ) );
		memset( szTextureName, 0, sizeof( szTextureName ) );
	}

	CHAR														szName[ AGCMUICONTROL_TEXTURE_LENGTH ];
	CHAR														szTextureName[ AGCMUICONTROL_TEXTURE_LENGTH ];
};

// ----------------- AgcmUIControl ------------------
class AgcmUIOption;
class AgcmUIControl : public AgcModule
{
private:
	typedef vector< AgcdUICustomTexture >						CustomTextureVec;
	typedef CustomTextureVec::iterator							CustomTextureItr;

public:
	AgcmUIControl( void );
	virtual ~AgcmUIControl( void );

	virtual BOOL				OnAddModule						( void );
	virtual BOOL				OnInit							( void );
	virtual BOOL				OnDestroy						( void );
	virtual void				OnClose							( void );
	
	BOOL						SetMessageDialogPosition		( INT32 lPosX, INT32 lPosY );
	VOID						SetButtonName					( CHAR *szOK, CHAR *szCancel );
	CHAR*						GetCustomTexture				( CHAR *szName );

	BOOL						UIIniRead						( CHAR* szName, BOOL bDecryption );
	RwTexture*					GetGridItemTexture				( AgpdGridItem* pstGridItem );
	RwTexture*					GetGridItemTexture				( AgpdGrid* pstGrid, INT32 lLayer, INT32 lRow, INT32 lColumn );
	AgpdGridItem*				GetGridItemPointer				( AgpdGrid* pstGrid, INT32 lLayer, INT32 lRow, INT32 lColumn );
	RwTexture**					GetAttachGridItemTextureData	( AgpdGridItem *pcsGridItem );

	static BOOL					ItemGridTextureConstructor		( PVOID pData, PVOID pClass, PVOID pCustData ) { return TRUE; }
	static BOOL					ItemGridTextureDestructor		( PVOID pData, PVOID pClass, PVOID pCustData ) { return TRUE; }

private :
	void						_LoadTextureFromINI				( AuIniManagerA* pINIManager );
	void						_LoadSoundFromINI				( AuIniManagerA* pINIManager );

public:
	AgpmGrid*													m_pcsAgpmGrid;

	RwTexture*													m_pToolTipTexture[ACUITOOLTIP_TTTEXTURE_NUM];
	RwTexture*													m_pBaseTexture;			// AcUIBase 가 쓰는 기본 Texture 
	RwTexture*													m_pComboBackTexture;	// Combo Back 이미지 

	RwTexture*													m_pConvertBase;
	RwTexture*													m_pConvertDisable;
	RwTexture*													m_pConvertOutline;

	RwTexture*													m_pPvPFree;
	RwTexture*													m_pPvPDanger;

	RwTexture*													m_pCancelTransform;
	RwTexture*													m_pCancelEvolution;

	RwTexture*													m_pDurabilityZero;

	RwTexture*													m_pCatapultUse;
	RwTexture*													m_pCatapultRepair;

	RwTexture*													m_pAtkResTowerUse;
	RwTexture*													m_pAtkResTowerRepair;

	RwTexture*													m_pCastleOwnerHuman;
	RwTexture*													m_pCastleOwnerOrc;
	RwTexture*													m_pCastleOwnerMoonelf;
	RwTexture*													m_pCastleOwnerDragonScion;
	RwTexture*													m_pCastleOwnerArchlord;

	RwTexture*													m_pArchlordGuildMark;
	RwTexture*													m_pGoMark;
	RwTexture*													m_pPetState;

	RwTexture*													m_pTextureHangameS;
	RwTexture*													m_pTextureHangameT;

	INT32														m_lItemGridTextureADDataIndex;
		
	char														m_szButtonClickSound[ AGCMSOUND_SOUND_FILENAME_LENGTH ];	// 버튼 클릭 사운드 파일명 

	AcUIWaitingDialog											m_clWaitingDialog;
	AcUIMessageDialog											m_clMessageDialog;
	AcUIOKCancelDialog											m_clOkCancelDialog;
	AcUIEditOKDialog											m_clEditOKDialog;
	
	CustomTextureVec											m_vCustomTexture;
};

#endif