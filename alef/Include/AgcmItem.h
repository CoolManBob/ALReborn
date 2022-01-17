#ifndef __AGCMITEM_H__
#define __AGCMITEM_H__

#include "AgcmConnectManager.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rpusrdat.h"

#include "AgcModule.h"
#include "ApBase.h"
#include "ApModule.h"

#include "AgpmCharacter.h"
#include "AgpdCharacter.h"
#include "AgcmCharacter.h"

#include "AgpmItem.h"
#include "AgpmSkill.h"
#include "AgpmItemConvert.h"
#include "AgcdItem.h"
#include "AgcaItemClump.h"
#include "AgcmLODManager.h"
#include "AgcmUIControl.h"
#include "AgpmGrid.h"
#include "AgcmRide.h"
#include "AgcmUICooldown.h"

#include "AgcmMap.h"

#include "AuRandomNumber.h"
#include "AgcmResourceLoader.h"

#include "AcuObject.h"

#include "AgcmShadow2.h"

#include "AgpmSystemMessage.h"
#include "AgcmItemEvolutionTable.h"
#include "AgcmItemEffectFinder.h"

#define AGCMITEM_NODE_ID_RIGHT_HAND					101
#define AGCMITEM_NODE_ID_LEFT_HAND					102
#define AGCMITEM_MAX_PATH_CLUMP						128
#define AGCMITEM_MAX_PATH_NAME_BUFFER				256
#define AGCMITEM_MAX_PATH_VALUE_BUFFER				1024
#define AGCMITEM_INI_NAME_BASE_CLUMP				"BASE_DFF"
#define AGCMITEM_INI_NAME_SECOND_CLUMP				"SECOND_DFF"
#define AGCMITEM_INI_NAME_FIELD_CLUMP				"FIELD_DFF"
#define AGCMITEM_INI_NAME_PICK_CLUMP				"PICK_DFF"
#define	AGCMITEM_INI_NAME_TEXTURE					"TEXTURE"
#define	AGCMITEM_INI_NAME_DURABILITY_ZERO_TEXTURE	"TEXTURE_DURA_ZERO"
#define	AGCMITEM_INI_NAME_DURABILITY_5UNDER_TEXTURE	"TEXTURE_DURA_5UNDER"
#define	AGCMITEM_INI_NAME_SMALL_TEXTURE				"SMALLTEXTURE"
#define AGCMITEM_INI_NAME_ATOMIC_COUNT				"ATOMIC_COUNT"
#define AGCMITEM_INI_NAME_TRANSFORM					"TRANSFORM"
#define AGCMITEM_INI_NAME_TRANSFORMDATA				"TRANSFORM_DATA"
#define AGCMITEM_INI_NAME_FIELD_TRANSFORM			"FIELD_TRANSFORM"
#define AGCMITEM_INI_NAME_LOD_LEVEL					"LOD_LEVEL"
#define AGCMITEM_INI_NAME_LOD_DISTANCE				"LOD_DISTANCE"
#define AGCMITEM_INI_NAME_LOD_HAS_BILLBOARD_NUM		"LOD_HAS_BILLBOARD_NUM"
#define AGCMITEM_INI_NAME_LOD_BILLBOARD_INFO		"LOD_BILLBOARD_INFO"
#define AGCMITEM_INI_NAME_EQUIP_TWOHANDS			"EQUIP_TWOHANDS"
#define AGCMITEM_INI_NAME_WITH_HAIR					"WITH_HAIR"
#define AGCMITEM_INI_NAME_WITH_FACE					"WITH_FACE"
#define AGCMITEM_INI_NAME_PRE_LIGHT					"PRE_LIGHT"
#define AGCMITEM_INI_NAME_OBJECT_TYPE				"OBJECT_TYPE"
#define AGCMITEM_INI_NAME_BOUNDING_SPHERE			"BSPHERE"
#define AGCMITEM_INI_NAME_LABEL						"LABEL"
#define AGCMITEM_USR_DAT_GENDER_ID_NAME				"GenderID"

#define AGCMITEM_INI_NAME_OCTREE_DATA				"OCTREE_DATA"

#define AGCMITEM_INI_NAME_DNF_1						"DID_NOT_FINISH_KOREA"
#define AGCMITEM_INI_NAME_DNF_2						"DID_NOT_FINISH_CHINA"
#define AGCMITEM_INI_NAME_DNF_3						"DID_NOT_FINISH_WESTERN"
#define AGCMITEM_INI_NAME_DNF_4						"DID_NOT_FINISH_JAPAN"

enum AgcmItemCallbackPoint
{
	AGCMITEM_CB_ID_INIT_ITEM = 0,		// 사용하지 않는다. (281103, BOB)
	AGCMITEM_CB_ID_EQUIP_ITEM,
	AGCMITEM_CB_ID_EQUIP_DEFAULT_ARMOUR,
	AGCMITEM_CB_ID_UNEQUIP_ITEM,		// Unequip 하기 전에 불린다. (040205, BOB)
	AGCMITEM_CB_ID_DETACHED_ITEM,		// Unequip하고 clump에 atomic이 detach되고 기본갑옷clump가 attach된 다음에 불린다. (040205, BOB)
	AGCMITEM_CB_ID_LOAD_CLUMP,
	AGCMITEM_CB_ID_FIELD_ITEM,
	AGCMITEM_CB_ID_PRE_REMOVE_DATA,
	AGCMITEM_CB_ID_SETUP_ITEM,
	AGCMITEM_CB_ID_READ_TEMPLATE,		// 2004.02.11. 김태희. 다시넣음
	AGCMITEM_CB_ID_CHECK_USE_ITEM,
	AGCMITEM_CB_ID_CONFIRM_USE_SKILL_BOOK,
	AGCMITEM_CB_ID_USE_MAP_ITEM,		// 맵사용할때 호출.
	AGCMITEM_CB_ID_NUM
};

enum eAgcmItemFlags
{
	E_AGCM_ITEM_FLAGS_NONE			= 0x0000,
	E_AGCM_ITEM_FLAGS_EXPORT		= 0x0001
};

extern char* g_aszWeaponTypeName[AGPMITEM_EQUIP_WEAPON_TYPE_NUM];
extern char* g_aszWeaponKindName[AGPMITEM_PART_V_BODY];

class AgcmItem : public AgcModule
{
private:
	AgcaItemClump		m_csClump;
	RpWorld*			m_pWorld;

	ApmMap*				m_pcsApmMap;
	AgpmFactors*		m_pcsAgpmFactors;
	AgpmItem*			m_pcsAgpmItem;
	AgpmSkill*			m_pcsAgpmSkill;
	AgpmItemConvert*	m_pcsAgpmItemConvert;
	AgcmCharacter*		m_pcsAgcmCharacter;
	AgpmCharacter*		m_pcsAgpmCharacter;
	AgcmRender*			m_pcsAgcmRender;
	AgcmLODManager*		m_pcsAgcmLODManager;
	AgcmShadow*			m_pcsAgcmShadow;
	AgcmUIControl*		m_pcsAgcmUIControl;
	AgpmGrid*			m_pcsAgpmGrid;
	AgcmMap*			m_pcsAgcmMap;
	AgcmShadow2*		m_pcsAgcmShadow2;
	AgcmRide*			m_pcsAgcmRide;
	AgcmUICooldown		*m_pAgcmUICooldown;

	AgpmSystemMessage*	m_pcsAgpmSystemMessage;

	INT16				m_nMaxItemClump;

	INT16				m_nItemAttachIndex;
	INT16				m_nItemTemplateAttachIndex;
	CHAR				m_szClumpPath[AGCMITEM_MAX_PATH_CLUMP];

	BOOL				m_bUseTexDict;
	CHAR				m_szTexDict[AGCMITEM_MAX_PATH_NAME_BUFFER];
	RwTexDictionary *	m_pstTexDict;

	MTRand				m_csRandom;

	INT32				m_lLoaderLoadID;
	INT32				m_lLoaderSetID;

	UINT32				m_ulNextAutoPickupItemTimeMSec;

	UINT32				m_ulModuleFlags;

	INT32				m_lCooldownID;

	AgcmItemEvolutionTable	m_ItemEvolutionTable;

public:
	static AgcmItem*	m_pcsThis;

	CHAR				m_szTexturePath[AGCMITEM_MAX_PATH_NAME_BUFFER];
	CHAR				m_szIconTexturePath[AGCMITEM_MAX_PATH_NAME_BUFFER];
	AgcmResourceLoader*	m_pcsAgcmResourceLoader;

public:
	AgcmItem();
	virtual ~AgcmItem()		{				}

	BOOL					OnInit();
	BOOL					OnAddModule();
	BOOL					OnIdle(UINT32 ulClockCount);	
	BOOL					OnDestroy();
	void					OnLuaInitialize( AuLua * pLua );


	AgpdItem				*GetItem(AgcdItem *pstItem);
	AgcdItem				*GetItemData(AgpdItem *pstItem);
	PVOID					GetTemplate(AgcdItemTemplate *pstTemplate);	
	AgcdItemTemplate		*GetTemplateData(AgpdItemTemplate *pstTemplate);


	AgcmCharacter			*GetAgcmCharacter()		{	return m_pcsAgcmCharacter;	}
	AgpmItem				*GetAgpmItem()			{	return m_pcsAgpmItem;		}

	CHAR*					GetWeaponTypeName(INT32 lIndex);
	VOID					AddFlags(UINT32 ulAdd)	{	m_ulModuleFlags |= ulAdd;	}

	void					SetMaxItemClump(INT16 nCount)		{	m_nMaxItemClump = nCount;	}
	void					SetWorld(RpWorld  *pWorld)			{	m_pWorld = pWorld;			}
	void					SetClumpPath(CHAR *szPath)			{
		strncpy(m_szClumpPath, szPath, AGCMITEM_MAX_PATH_CLUMP);
		m_szClumpPath[AGCMITEM_MAX_PATH_CLUMP - 1] = '\0';
	}
	void					SetTexDictFile(CHAR *szTexDict)		{
		m_bUseTexDict = TRUE;
		strncpy(m_szTexDict, szTexDict, AGCMITEM_MAX_PATH_NAME_BUFFER);
		m_szTexDict[AGCMITEM_MAX_PATH_NAME_BUFFER - 1] = '\0';
	}
	void					SetTexturePath(CHAR *szTexturePath)	{
		strncpy(m_szTexturePath, szTexturePath, AGCMITEM_MAX_PATH_NAME_BUFFER);
		m_szTexturePath[AGCMITEM_MAX_PATH_NAME_BUFFER - 1] = '\0';
	}
	void					SetIconTexturePath(CHAR *szTexturePath)	{
		strncpy(m_szIconTexturePath, szTexturePath, AGCMITEM_MAX_PATH_NAME_BUFFER);
		m_szIconTexturePath[AGCMITEM_MAX_PATH_NAME_BUFFER - 1] = '\0';
	}

	BOOL					MakeItemClump(AgpdItem *pcsAgpdItem);
	BOOL					ReleaseItemData(AgpdItem *pcsAgpdItem);
	BOOL					RemoveClumpData(AgpdItem *pcsAgpdItem, AgcdItem *pcsAgcdItem);

	BOOL					LoadAllTemplateData();
	BOOL					LoadTemplateClump(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate, BOOL bArmour);
	BOOL					LoadTemplateTexture(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate);
	BOOL					LoadTemplateData(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate, BOOL bCheckArmour = TRUE);
	BOOL					ReleaseTemplateClump(AgcdItemTemplate *pstAgcdItemTemplate, BOOL bArmour);
	BOOL					ReleaseTemplateTexture(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate);
	BOOL					ReleaseTemplateData(AgpdItemTemplate *pcsAgpdItemTemplate, AgcdItemTemplate *pstAgcdItemTemplate, BOOL bCheckArmour = TRUE);

	BOOL					DeleteTemplatePreData(AgcdItemTemplate *pstAgcdItemTemplate);

	RpClump					*GetClumpAdminData(CHAR *szFile, BOOL bMake = TRUE);
	BOOL					RemoveClumpAdminData(CHAR *szName);

	RwTexture				*GetTexture(CHAR *szFile);
	BOOL					SetGridItemAttachedTexture( AgpdItem* pcsAgpdItem );
	BOOL					SetGridItemTemplateAttachedTexture( AgpdItemTemplate* pcsAgpdItemTemplate, AgpdGridItem *pcsGridItem );

	BOOL					SetGridItemAttachedSmallTexture( AgpdItemTemplate* pcsAgpdItemTemplate, AgpdGridItem *pcsGridItem );

	// util
	RpClump					*CopyClump(RpClump *pstClump);
	RpAtomic				*CopyAtomic(RpAtomic *pstAtomic);
	AgpdItem				*FindCharacterDefaultItem(INT32 lCID, INT32 lTID);

	BOOL					UpdateItem(AgpdItem *pcsAgpdItem);
	BOOL					AddClumpToWorld(RpClump *pstClump, BOOL bBillboard = FALSE);
	BOOL					RemoveClumpFromWorld(RpClump *pstClump);

	BOOL					ParseValueFieldTransform(AgcdItemTemplate *pItem, ApModuleStream *pcsStream);

	BOOL					SetEquipAnimType( AgpdCharacter* ppdCharacter, AgcdCharacter* pcdCharacter );
	//BOOL					SetUnEquipAnimType(AgpdCharacter *pstAgpdCharacter, AgcdCharacter *pstAgcdCharacter, AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip);
	int						GetAnimDefaultRide( int lAnimType2 ) const;
	int						GetAnimStandardRide( int lAnimType2 ) const;
	int						GetAnimWeaponRide( int lAnimType2 ) const;

	// Texture Dictionary 관련 함수들
	BOOL					DumpTexDict();

	RpAtomic*				AttachNodeIndex(RpClump *pstBaseClump, RpHAnimHierarchy *pstBaseHierarchy, RpHAnimHierarchy *pstAttachHierarchy, RpAtomic *pstSrcAtomic, RwInt32 lNodeID, RwInt32 lPartID, RwMatrix *pstTransform = NULL, RpHAnimHierarchy **ppstAttachedAtomicHierarchy = NULL, enumRenderType eForedRenderType = R_DEFAULT);	
	BOOL					DetachNodeIndex(RpClump *pstBaseClump, RpHAnimHierarchy *pstBaseHierarchy, RpAtomic *pstDetachAtomic, RwInt32 lNodeID, RwInt32 lNextNodeID = 0);

	BOOL					DropItem(AgpdItem *pcsItem);
	BOOL					PickupItem(AgpdItem *pcsItem, INT32 lLayer = -1, INT32 lRow = -1, INT32 lColumn = -1);
	BOOL					AutoPickupItem();

	// item을 사용한다. (물약, 스킬북 등등의 경우다.)
	BOOL					UseItem(INT32 lIID, AgpdCharacter *pcsCharacter = NULL);
	BOOL					UseItem(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter = NULL);

	BOOL					UseItemSkillBook(AgpdItem *pcsItem, BOOL bIsNeedConfirm);

	BOOL					UseItemHPPotionTest();

	BOOL					CheckReUseInterval(AgpdItem *pcsItem);

	BOOL					SplitItem(INT32 lIID, INT32 lSplitStackCount, AgpdItemStatus eTargetStatus, INT16 nLayer = -1, INT16 nRow = -1, INT16 nColumn = -1);
	BOOL					SplitItem(INT32 lIID, INT32 lSplitStackCount, AuPOS *pstDestPos);

	BOOL					ReleaseEquipItems(AgpdCharacter	*pcsAgpdCharacter);
	BOOL					EquipItem(AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckDefaultArmour = TRUE, BOOL bViewHelmet = FALSE );
	BOOL					EquipDefaultArmour(RpClump *pstCharClump, RpClump *pstArmorClump, RpHAnimHierarchy *pstBaseHierarchy, INT16 nPartID = 0, AttachedAtomics *pcsAttachedAtomics = NULL);
	BOOL					UnEquipItem(AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem, BOOL bViewHelmet = FALSE );
	BOOL					RefreshEquipItems(AgpdCharacter *pcsAgpdCharacter);

	RpAtomic				*AttachItem(RpAtomic *atomic, AgcdItemTemplateEquipSet *pstAgcdItemTemplateEquipSet, BOOL bCheckPartID = TRUE);
	RpAtomic				*AttachCharonToSummoner( RpAtomic *atomic, AgcdItemTemplateEquipSet *pstAgcdItemTemplateEquipSet );
	RpAtomic				*DetachItem(RpAtomic *atomic, AgcdItemTemplateEquipSet *pstAgcdItemTemplateEquipSet);	
	BOOL					AttachItem(INT32 lCharacterTemplateID, AgcdCharacter *pstAgcdCharacter, AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip, AgcdItem *pstAgcdItem, AgcdItemTemplate *pstAgcdItemTemplate, AuCharClassType eClassType = AUCHARCLASS_TYPE_NONE );
	BOOL					DetachItem(AgcdCharacter *pstAgcdCharacter, AgpdItemTemplateEquip *pcsAgpdItemTemplateEquip, AgcdItem *pstAgcdItem, AgcdItemTemplate *pstAgcdItemTemplate = NULL, AuCharClassType eClassType = AUCHARCLASS_TYPE_NONE );

	BOOL					AddAttachedAtomic(AttachedAtomics *pcsAttachedAtomics, RpAtomic *pstAtomic);
	BOOL					RemoveAllAttachedAtomics(AttachedAtomics *pcsAttachedAtomics);
	INT32					GetNumAttachedAtomics(AttachedAtomics *pcsAttachedAtomics);

	// Callback
	static RpAtomic			*AttachItemCheckPartIDCB(RpAtomic *atomic, PVOID pData);
	static RpAtomic			*AttachItemCB(RpAtomic *atomic, PVOID pData);
	static RpAtomic			*AttachCharonToSummonerCB(RpAtomic *atomic, PVOID pData);
	static RpAtomic			*DetachItemCB(RpAtomic *atomic, PVOID pData);

	static BOOL				ItemConstructorCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemDestructorCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemTemplateConstructorCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemTemplateDestructorCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				ItemStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL				ItemStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL				ItemTemplateStreamReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL				ItemTemplateStreamWriteCB(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);

	static BOOL				ItemInitCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemAddCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemRemoveCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemEquipCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemUnequipCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemFieldCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemInventoryCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				ItemUseCB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				ItemUseByTIDCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				ItemUseSuccessCB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				CBUnuseItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBPauseItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL				InitCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData);	
	static BOOL				PreRemoveCharacterCB(PVOID pData, PVOID pClass, PVOID pCustData);	

	static BOOL				CBUpdateFactorDurability(PVOID pData, PVOID pClass, PVOID pCustData);	
	static BOOL				CBUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					SetCallbackInitItem				(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackEquipItem			(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackEquipDefaultArmour	(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUnequipItem			(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackDetachedItem			(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackLoadClump			(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackFieldItem			(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackPreRemoveData		(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackSetupItem			(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackReadTemplate			(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackCheckUseItem			(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackConfirmUseSkillBook	(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL					SetCallbackUseMapItem			(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// 통신 관련
	BOOL					SendEquipInfo(INT32 lIID, INT32 lCID);
	BOOL					SendInventoryInfo(INT32 lIID, INT32 lCID, INT16 nInvIdx = -1, INT16 nInvRow = -1, INT16 nInvCol = -1);
	BOOL					SendSubInventoryInfo(INT32 lIID, INT32 lCID, INT16 nInvIdx = -1, INT16 nInvRow = -1, INT16 nInvCol = -1);
	BOOL					SendBankInfo(INT32 lIID, INT32 lCID, INT16 lLayer = -1, INT16 lRow = -1, INT16 lColumn = -1);
	BOOL					SendQuestInfo(INT32 lIID, INT32 lCID, INT16 lLayer = -1, INT16 lRow = -1, INT16 lColumn = -1);
	BOOL					SendFieldInfo(INT32 lIID, INT32 lCID, AuPOS *ppcsPos);
	BOOL					SendPickupItem(INT32 lIID, INT32 lCID);
	BOOL					SendBuyBankSlot(INT32 lCID);
	BOOL					SendRequestConvert(INT32 lIID, INT32 lCID, INT32 lSpiritStoneID);
	BOOL					SendDropMoneyToField(INT32 lCID, INT32 lMoneyCount );
	BOOL					SendRequestDestroyItem(INT32 lCID, INT32 lIID);
	BOOL					SendUseReturnScroll();
	BOOL					SendCancelReturnScroll();
	BOOL					SendSplitItem(INT32 lIID, INT32 lSplitStackCount, AgpdItemStatus eTargetStatus, INT16 nLayer = -1, INT16 nRow = -1, INT16 nColumn = -1);
	BOOL					SendSplitItem(INT32 lIID, INT32 lSplitStackCount, AuPOS *pstDestPos);

	BOOL					SendAutoPickItem(INT32 llID, INT32 lCID, INT16 *lSwitch);

	BOOL					UnuseItem(AgpdItem *pcsItem);

	// UI 관련
	BOOL					BeStackableItem( INT32 lIID );		// Stackable Item 인지 알려준다
	INT32					GetItemCount( INT32 lIID );			// Stackable Item 의 Count를 알려준다

	RwTexture*				GetItemTexture( INT32 lItemID );	// Template에 있는 2D Image Texture 를 가져온다. 

	BOOL					UpdateInventoryForUI( INT32 lItemID, INT16 nLayer, INT16 nRow, INT16 nColumn );

	static	BOOL			CBUpdateInventory( PVOID pData, PVOID pClass, PVOID pCustData );		// Inventory Item 목록이 업데이트 
	static	BOOL			CBUpdateBank( PVOID pData, PVOID pClass, PVOID pCustData );				// Bank Item 목록이 업데이트
	static	BOOL			CBUpdateMoney( PVOID pData, PVOID pClass, PVOID pCustData );			// Inventory 내의 Money가 업데이트
	static  BOOL			CBUpdateBankMoney( PVOID pData, PVOID pClass, PVOID pCustData );		// Bank 내의 Money가 업데이트
	static	BOOL			CBUpdateEquipment( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL			CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)		;	// Self Character가 Setting되었음

	static  BOOL			CBActionPickupItem( PVOID pData, PVOID pClass, PVOID pCustData )		;

	static	BOOL			CBTransformAppear( PVOID pData, PVOID pClass, PVOID pCustData )			;
	static	BOOL			CBRestoreTransformAppear( PVOID pData, PVOID pClass, PVOID pCustData )			;

	static	BOOL			CBEvolutionAppear( PVOID pData, PVOID pClass, PVOID pCustData )			;
	static	BOOL			CBRestoreEvolution( PVOID pData, PVOID pClass, PVOID pCustData )			;

	static	BOOL			CBAfterCooldown(PVOID pData, PVOID pClass, PVOID pCustData);
	static	BOOL			CBAddItemExtraData(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					SetFieldItem(AgpdItem *pcsAgpdItem);
	BOOL					SetInventoryItem(AgpdItem *pcsAgpdItem);

	BOOL					ReleaseGridItemAttachedTexture( AgpdItemTemplate* pcsAgpdItemTemplate );
	BOOL					SetGridItemAttachedTexture( AgpdItemTemplate* pcsAgpdItemTemplate );

	// fOffset은 0.0f에서 2.0f 사이의 범위다.
	BOOL					SetPreLightForAllItem(FLOAT fOffset);

	RpAtomic				*CreateAtomic(CHAR *szDFF);
	static RpAtomic			*UtilSetAtomicCB(RpAtomic *atomic, void *data);
	RpAtomic				*UtilCopyAtomic(RpAtomic *pstAtomic);

	// Tool
	RpClump					*CreateArmourPartClump(RpClump *pstArmourClump, INT32 lPartID);
	static RpAtomic			*CreateArmourPartClumpCB(RpAtomic *atomic, void *data);

	static BOOL				CBLoadTemplate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBMakeItemClump(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL				CBSetItem(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL					IsItemValid(AgpdItem *pcsItem);
	BOOL					IsItemValid(AgpdItem *pcsItem, AgcdItem *pstItem);

	BOOL					IsItemReady(AgpdItem *pcsItem);
	BOOL					IsItemReady(AgpdItem *pcsItem, AgcdItem *pstItem);

	VOID					SetItemStatus(AgpdItem *pcsItem, INT32 lStatus);
	VOID					SetItemStatus(AgcdItem *pstItem, INT32 lStatus);
	VOID					ResetItemStatus(AgpdItem *pcsItem, INT32 lStatus);
	VOID					ResetItemStatus(AgcdItem *pstItem, INT32 lStatus);

	VOID					ViewHelmet( AgcdCharacter* pstCharacter, BOOL bViewHelmet );
	VOID					SetAgcdRide( AgcmRide* pAgcmRide );

	VOID					CancelCoolDown();

	BOOL					ReceiveUnuseItem( AgpdItem *pcsItem );			//	2005.11.30. By SungHoon
	BOOL					ReceiveUseItem( AgpdItem *pcsItem );			//	2005.11.30. By SungHoon
	BOOL					ReceivePauseItem(AgpdItem* pcsItem);			//	2006.01.08. steeple

	BOOL					OnEquipItemNecklace( AgpdCharacter* ppdCharacter, AgpdItem* ppdItem, AgcdItem* pcdItem, AuCharClassType eClassType = AUCHARCLASS_TYPE_NONE );
	BOOL					OnUnEquipItemNecklace( AgpdCharacter* ppdCharacter, AgpdItem* ppdItem, AgcdItem* pcdItem, AuCharClassType eClassType = AUCHARCLASS_TYPE_NONE );
	BOOL					OnAttachCharonToSummoner( AgpdCharacter* ppdCharacter, AgcdCharacter* pcdCharacter,
														AgpdItemTemplateEquip* ppdItemTemplateEquip,
														AgcdItemTemplate* pcdItemTemplate, AgcdItem* pcdItem );

	AgpdItem*				GetCurrentEquipWeapon( void* pCharacter );
	AgpdItem*				GetCurrentEquipWeapon( void* pCharacter, INT32 nHand );

	BOOL					OnAttachWeapon( void* pCharacter );
	BOOL					OnDetachWeapon( void* pCharacter );

	BOOL					OnAttachPart( VOID* pCharacter , AgpmItemPart ePartType );
	BOOL					OnDetachPart( VOID* pCharacter , AgpmItemPart ePartType	);

	BOOL					OnUpdateHairAndFace( AgpdItemTemplate* ppdItemTemplate, AgcdCharacter* pcdCharacter, BOOL bIsEquip );
	BOOL					OnUpdateViewHelmet( AgcdCharacter* pcdCharacter );

	BOOL					IsTeleportScroll( AgpdItem* ppdItem );
	BOOL					OpenProgressBarWaiting( AgpdItem* ppdItem, INT32 nWaitDuration = 0 );
	BOOL					OpenProgressBarWaitingReturn( INT32 nWaitDuration = 0 );
	BOOL					IsNowWaitForUseItem( void ) { return m_bIsNowWaitingForUseItem; }
	BOOL					IsNoneDurabilityItem( AgpdItem* ppdItem );

	static BOOL				CBAfterCoolDownWaitUseItem( void* pData, void* pClass, void* pCustomData );
	static BOOL				CBAfterCoolDownWaitUseItemReturn( void* pData, void* pClass, void* pCustomData );

private :
	BOOL					m_bIsNowWaitingForUseItem;
	INT32					m_nCoolDownWaitUseItemID;
	INT32					m_nCoolDownWaitUseItemReturnID;
	INT32					m_nWaitingItemID;
	AgcmItemEffectFinder	m_ItemEffectFinder;

	AgpdItemTemplateEquip*	_GetDragonScionItemAgpdTemplate( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter, AuCharClassType eClassType );
	AgpdItemTemplateEquip*	_GetDragonScionItemAgpdTemplate( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType );
	AgcdItemTemplate*		_GetDragonScionItemAgcdTemplate( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter, AuCharClassType eClassType );
	AgcdItemTemplate*		_GetDragonScionItemAgcdTemplate( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType );
	AgcdItem*				_GetDragonScionItemAgcdItem( AgpdItemTemplate* ppdItemTemplate, AgpdCharacter* ppdCharacter, AuCharClassType eClassType );
	BOOL					_IsMatrixValid( RwMatrix* pTM );

	RpAtomic*				_AttachItemArmor( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart );
	RpAtomic*				_AttachItemWeapon( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart );
	RpAtomic*				_AttachItemShield( RpAtomic* pAtomic, AgcdItemTemplateEquipSet* pEquipSet, INT32 nPart );

	BOOL					_EquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bCheckDefaultArmour = TRUE, BOOL bViewHelmet = FALSE );
	BOOL					_UnEquipItem( AgpdCharacter *pstAgpdCharacter, AgpdItem *pcsAgpdItem, BOOL bEquipDefaultItem, BOOL bViewHelmet = FALSE );
	BOOL					_SetEquipAnimType( AgcdCharacter* pcdCharacter, INT32 nWeaponType );
	BOOL					_IsWearAvatarItem( AgpdCharacter* ppdCharacter, BOOL bIsCheckFace, BOOL bIsCheckHair );

	BOOL					IsDisableUnEquipItem( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter = NULL );
};

RpAtomic*					CallBack_FindAtomicArms( RpAtomic*, void* pData );


#endif // __AGCMITEM_H__
