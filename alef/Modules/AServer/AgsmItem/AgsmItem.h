/******************************************************************************
Module:  AgsmItem.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 07. 29
******************************************************************************/

#if !defined(__AGSMITEM_H__)
#define __AGSMITEM_H__

#include "ApBase.h"
#include "ApmMap.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgpmDropItem2.h"
#include "AgpmSkill.h"
#include "AgpmEventSkillMaster.h"
#include "AgpmParty.h"
#include "AgpmEventBinding.h"
#include "AgpmPvP.h"
#include "AgpmLog.h"
#include "AgpmBillInfo.h"
#include "AgpmSiegeWar.h"
#include "AgpmSummons.h"
#include "AgsmSystemMessage.h"

#include "AsDefine.h"
//#include "AgsmDBStream.h"
#include "AgsEngine.h"
#include "AgsmFactors.h"
#include "AgsmAOIFilter.h"
#include "AgsdItem.h"
#include "AgsmCharacter.h"
#include "AgsmZoning.h"
#include "AgsmParty.h"
#include "AgpmGrid.h"
#include "AuGenerateID.h"
#include "AuPacket.h"
#include "AgpmOptimizedPacket2.h"
#include "AuDatabase2.h"
#include "AgpmArchlord.h"


typedef enum _eAgsmItemCB {
	AGSMITEM_CB_GET_NEW_IID				= 0,
	AGSMITEM_CB_GET_NEW_DBID,

	AGSMITEM_CB_UPDATE_ITEM_DURABILITY,

	AGSMITEM_CB_RECEIVE_ITEM_DATA,

	AGSMITEM_CB_CREATE_NEW_ITEM,			// 아이템 생성 kelovon 20051123

	AGSMITEM_CB_INSERT_ITEM_TO_DB,
	AGSMITEM_CB_UPDATE_ITEM_TO_DB,
	AGSMITEM_CB_DELETE_ITEM_TO_DB,

	AGSMITEM_CB_INSERT_CONVERT_HISTORY_TO_DB,
	AGSMITEM_CB_REMOVE_CONVERT_HISTORY_FROM_DB,

	AGSMITEM_CB_SEND_ITEM,
	AGSMITEM_CB_SEND_ITEM_VIEW,

	AGSMITEM_CB_USE_ITEM_SKILLBOOK,
	AGSMITEM_CB_USE_ITEM_SKILLSCROLL,
	AGSMITEM_CB_USE_ITEM_SKILLROLLBACKSCROLL,
	AGSMITEM_CB_USE_ITEM_REVERSE_ORB,
	AGSMITEM_CB_USE_ITEM_TRANSFORM,
	AGSMITEM_CB_USE_ITEM_LOTTERYBOX,

	AGSMITEM_CB_USE_ITEM_PICKUP_CHECK_QUEST_VALID,

	AGSMITEM_CB_USE_TELEPORT_SCROLL,
	AGSMITEM_CB_RIDE_MOUNT,
	AGSMITEM_CB_RIDE_DISMOUNT,
	
	AGSMITEM_CB_GET_NEW_CASHITEMBUYID,
	AGSMITEM_CB_INSERT_CASHITEMBUYLIST,
	AGSMITEM_CB_UPDATE_CASHITEMBUYLIST,
	AGSMITEM_CB_UPDATE_CASHITEMBUYLIST2,

	AGSMITEM_CB_PRE_CHECK_ENABLE_SKILLSCROLL,

	AGSMITEM_CB_CHECK_RETURN_POSITION,

	AGSMITEM_CB_PICKUP_CHECK_ITEM,

	AGSMITEM_CB_END_BUFFED_SKILL_BY_ITEM,
	
} eAgsmItemCB;

const int	AGSMITEM_AD_ITEM							= 1;
const int	AGSMITEM_REDUCE_DURABILITY					= 1;
const int	AGSMITEM_PROCESS_REMOVE_FIELD_ITEM_INTERVAL	= 10000;
const int	AGSMITEM_MAX_PRESERVE_ITEM_FIELD_TIME		= 30000;	// 일단 30초 한다.

class AgpmGuild;
class AgsmTitle;

class AgsmItem : public AgsModule {
private:
	AgpmGrid		*m_pagpmGrid;

	ApmMap			*m_papmMap;
	AgpmFactors		*m_pagpmFactors;
	AgpmCharacter	*m_pagpmCharacter;
	AgpmItem		*m_pagpmItem;
	AgpmItemConvert	*m_pagpmItemConvert;
	AgpmDropItem2	*m_pagpmDropItem2;
	AgpmSkill		*m_pagpmSkill;
	AgpmEventSkillMaster *m_pagpmEventSkillMaster;
	AgpmParty		*m_pagpmParty;
	ApmEventManager	*m_papmEventManager;
	AgpmEventBinding	*m_pagpmEventBinding;
	AgpmPvP			*m_pagpmPvP;
	AgpmLog			*m_pagpmLog;
	AgpmBillInfo	*m_pagpmBillInfo;
	AgpmSiegeWar	*m_pagpmSiegeWar;
	AgpmSummons		*m_pagpmSummons;
	AgsmSystemMessage	*m_pagsmSystemMessage;

	AgpmOptimizedPacket2	*m_pagpmOptimizedPacket2;

	//AgsmDBStream	*m_pagsmDBStream;
	AgsmFactors		*m_pagsmFactors;
	AgsmAOIFilter	*m_pagsmAOIFilter;
	AgsmServerManager	*m_pAgsmServerManager;
	AgsmCharacter	*m_pagsmCharacter;
	AgsmZoning		*m_pagsmZoning;
	AgsmParty		*m_pagsmParty;
	AgpmArchlord	*m_pagpmArchlord;
	AgsmTitle		*m_pagsmTitle;
	
	INT16			m_nIndexADItem;
	INT16			m_nIndexADChar;

	ApAdmin			m_csAdminFieldItem;
	ApAdmin			m_csAdminDBIDItem;

	//AuGenerateID	m_csGenerateID;
	//AuGenerateID64	m_csGenerateID64;

	MTRand			m_csRandomNumber;

	BOOL			m_bFirst;

	UINT32			m_ulNextProcessRemoveFieldItemMSec;

	LONG			m_lUnaddedItemCount;

	BOOL			m_bCreate500000ItemCountLog;

public:
	AuPacket		m_csPacket;
	AuPacket		m_csPacketData;
	AuPacket		m_csPacketADChar;

	AuPacket		m_csPacketPartyItem;		// 파티멤버가 아이템을 획득했다는 정보를 전송하기 위해	2005.04.21	By SungHoon

	// Item Log 는 간편하게 처리하기 위해서 public
	//AgpmItemLog*		m_pagpmItemLog;

private:
	BOOL			OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	AgpdItem*		CreateItem(INT32 lTID, AgpdCharacter* pcsCharacter, INT32 lCount = 0, BOOL bGenerateDBID = TRUE, INT32 lSkillTID = 0);

public:
	AgsmItem();
	~AgsmItem();

	BOOL OnAddModule();

	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle2(UINT32 ulClockCount);
	void Report(FILE *fp);

	BOOL OnValid(CHAR* szData, INT16 nSize);

	BOOL	SetMaxFieldItem(INT32 lCount);
	BOOL	SetMaxDBIDItem(INT32 lCount);

	BOOL	AddDBIDAdmin(UINT64 ullDBID);
	BOOL	RemoveDBIDAdmin(UINT64 ullDBID);

	UINT64	GetDBIID(INT32 lIID, BOOL bIsSetNewDBID = FALSE);
	UINT64	GetDBIID(AgpdItem *pItem, BOOL bIsSetNewDBID = FALSE);

	INT16	SetDBIID(INT32 lIID, UINT64 ullDBIID);
	INT16	SetDBIID(AgpdItem *pItem, UINT64 ullDBIID);

	AgpdItem*	GetItemByDBID(AgpdCharacter *pcsCharacter, UINT64 ullDBID);

	// GetAttachedModuleData() Wrap Functions
	AgsdItem* GetADItem(PVOID pData);
	AgsdItemADChar* GetADCharacter(PVOID pData);

	static BOOL ConAgsdItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgsdItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL ConAgsdItemADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL DesAgsdItemADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackInit(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackNewItemToClient(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackEquip(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackUnEquip(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveForNearCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackAddInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveInventory(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackUpdateTradeGrid(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackUpdateSalesBoxGrid(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackAddField(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveField(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackChangeOwner(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackAddItemToQuest(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackUpdateBank(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveBank(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackUpdateQuest(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackAddChar(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackCharLevelUp(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateRegionIndex(PVOID pData, PVOID pClass, PVOID pCustData);

	/*
	static BOOL CallbackSendSectorInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackSendSectorRemoveInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	*/

	static BOOL CallbackSendCellInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackSendCellRemoveInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackSyncAddChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackSendRemoveChar(PVOID pData, PVOID pClass, PVOID pCustData);

	/*
	static BOOL CallbackAddSector(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveSector(PVOID pData, PVOID pClass, PVOID pCustData);
	*/

	static BOOL CallbackAddCell(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveCell(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackZoningStart(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackZoningPassControl(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackRemoveItemOnly(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CallbackReCalcFactor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CallbackUpdateFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CallbackConnectDB(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CallbackDisconnectDB(PVOID pData, PVOID pClass, PVOID pCustData);
	//static BOOL CallbackDBOperationResult(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackSendCharacterAllInfo(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackSendCharacterNewID(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackSendCharacterAllServerInfo(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackUseItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackUnuseItem(PVOID pData, PVOID pClass, PVOID pCustData);		//	2005.11.30. By SungHoon
	static BOOL CallbackUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);

	//static BOOL CallbackDeleteItem(PVOID pData, PVOID pClass, PVOID pCustData);

//	static BOOL CallbackAskReallyConvertItem(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL CallbackConvertItem(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL CallbackAddConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL CallbackRemoveConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL CallbackUpdateConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL CallbackSendConvertHistory(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackCheckPickupItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackPickupItemMoney(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackPickupItemResult(PVOID pData, PVOID pClass, PVOID pCustData);

//	static BOOL CallbackUpdateEgoExp(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL CallbackUpdateEgoLevel(PVOID pData, PVOID pClass, PVOID pCustData);

//	static BOOL CallbackPutSoulIntoCube(PVOID pData, PVOID pClass, PVOID pCustData);
//	static BOOL CallbackUseSoulCube(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackUpdateStackCount(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL ProcessIdleRemoveFieldItem(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData);

	//static BOOL CallbackSendAllDBData(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackUpdateAllToDB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackUseReturnScroll(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackRestoreTransform(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackAdjustTransformFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackUpdateReverseOrbReuseTime(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackUpdateTransformReuseTime(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CallbackInitTransformReuseTime(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CallbackUpdateItemStatusFlag(PVOID pData, PVOID pClass, PVOID pCustData);

//	static BOOL CallbackApplyBonusFactor(PVOID pData, PVOID pClass, PVOID pCustData);

	//static BOOL CBBackupItemBasicData(PVOID pData, PVOID pClass, PVOID pCustData);
	//static BOOL CBBackupItemAddConvertHistoryData(PVOID pData, PVOID pClass, PVOID pCustData);
	//static BOOL CBBackupItemRemoveConvertHistoryData(PVOID pData, PVOID pClass, PVOID pCustData);

	// Cash Item 관련 함수, 20051125, kelovon
	static BOOL CallbackAddCashInventory(PVOID pData, PVOID pClass, PVOID pCustData);

	// 캐시 아이템 관련, 20051201, kelovon
	static BOOL CBUseCashItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUnUseCashItem(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBConvertAsDrop(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUpdateCooldown(PVOID pData, PVOID pClass, PVOID pCustData);

	// Relay Server Operation
	// Callback 함수
	BOOL SetCallbackItemInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackItemUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackItemDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackItemConvertHistoryInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
//	BOOL SetCallbackItemConvertHistoryDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackCashItemBuyListInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCashItemBuyListUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCashItemBuyListUpdate2(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackPickupCheckItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackEndBuffedSkillByItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// 
	BOOL SendRelayUpdate(AgpdItem* pItem);
	BOOL SendRelayInsert(AgpdItem* pItem);
	BOOL SendRelayDelete(AgpdItem* pItem);
/*
	static BOOL CBInsertToRelay(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBUpdateToRelay(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDeleteToRelay(PVOID pData, PVOID pClass, PVOID pCustData);
*/
	//BOOL InitServer(UINT32 ulStartValue, UINT32 ulServerFlag = 0, INT16 nSizeServerFlag = 0);
	//BOOL InitItemDBIDServer(UINT64 ullStartValue, UINT64 ullServerFlag = 0, INT16 nSizeServerFlag = 0);
	//INT32 GenerateIID();
	//INT64 GenerateDBID();

	BOOL LoadItem(AgpdCharacter *pcsCharacter);
	BOOL SendItem(AgpdCharacter *pcsCharacter);
	
	BOOL SendNewItem(AgpdItem *pItem);
	BOOL SendNewItemToClient(AgpdItem *pItem);

	BOOL SendPacketItem(AgpdItem *pcsItem, UINT32 ulNID = DPNID_ALL_PLAYERS_GROUP, BOOL bGroupNID = FALSE, INT16 nFlag = APMODULE_SENDPACKET_PLAYER );
	BOOL SendPacketItemView(AgpdItem *pcsItem, UINT32 ulNID = DPNID_ALL_PLAYERS_GROUP, BOOL bGroupNID = FALSE, UINT32 ulSelfNID = 0, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	BOOL SendPacketItemAll(AgpdCharacter *pcsCharacter, UINT32 ulNID = DPNID_ALL_PLAYERS_GROUP, BOOL bGroupNID = FALSE, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	BOOL SendPacketItemAllNewID(AgpdCharacter *pcsCharacter, UINT32 ulNID = DPNID_ALL_PLAYERS_GROUP);
	BOOL SendPacketItemEquip(AgpdCharacter *pcsCharacter, UINT32 ulNID = DPNID_ALL_PLAYERS_GROUP, BOOL bGroupNID = FALSE, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	BOOL SendPacketItemRemove(INT32 lIID, UINT32 ulNID = DPNID_ALL_PLAYERS_GROUP, BOOL bGroupNID = FALSE, INT16 nFlag = APMODULE_SENDPACKET_PLAYER);
	BOOL SendPacketItemRemoveNear(INT32 lIID, AuPOS stPos);

	BOOL SendPacketItemServerData(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin = TRUE);
	BOOL SendPacketItemADCharacter(AgpdCharacter *pcsCharacter, UINT32 ulNID);

	BOOL SendPacketItemStackCount(AgpdItem *pcsItem, UINT32 ulNID);
	BOOL SendPacketItemStackCount(AgpdItem *pcsItem, INT32 lStackCount, UINT32 ulNID);

	BOOL SendPacketItemFactor(AgpdItem *pcsItem, PVOID pvPacketFactor, PVOID pvPacketFactorPercent, UINT32 ulNID);
	BOOL SendPacketItemFactor(AgpdItem *pcsItem, UINT32 ulNID);

	BOOL SendPacketPickupItemResult(INT8 cResult, INT32 lIID, INT32 lTID, INT32 lItemCount, UINT32 ulNID);

//	BOOL SendPacketConvertHistory(AgpdItem *pcsItem, UINT32 dpnid);
//	BOOL SendPacketAddConvertHistory(AgpdItem *pcsItem, UINT32 dpnid);
//	BOOL SendPacketRemoveConvertHistory(AgpdItem *pcsItem, INT32 lIndex, UINT32 dpnid);

	BOOL SendPacketUpdateReturnScrollStatus(AgpdCharacter *pcsCharacter, BOOL bIsEnable);
	BOOL SendPacketUseReturnScrollResult(AgpdCharacter *pcsCharacter, BOOL bIsSuccess);

	BOOL SendPacketUseItemByTID(AgpdCharacter *pcsItemOwner, INT32 lTID, AgpdCharacter *pcsTargetCharacter, UINT32 ulUseInterval = 0);
	BOOL SendPacketUseItemByTID(AgpdCharacter *pcsItemOwner, INT32 lTID, INT32 lTargetID, UINT32 ulUseInterval = 0);
	BOOL SendPacketUseItemFailByTID(AgpdCharacter *pcsItemOwner, INT32 lTID, AgpdCharacter *pcsTargetCharacter);
	BOOL SendPacketUseItemSuccess(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem);


	BOOL SendPacketNewItemID(AgpdItem *pcsItem, UINT32 ulNID);

	BOOL SendPacketUpdateReverseOrbReuseTime(AgpdCharacter *pcsCharacter, UINT32 ulNID);
	BOOL SendPacketUpdateTransformReuseTime(AgpdCharacter *pcsCharacter, UINT32 ulNID);
	BOOL SendPacketInitTransformReuseTime(AgpdCharacter *pcsCharacter, UINT32 ulNID);

	BOOL SendPacketUpdateItemStatusFlag(AgpdItem *pcsItem, UINT32 ulNID);

	BOOL SendPacketUnuseItem(AgpdItem *pcsItem);			//	2005.11.30. By SungHoon
	BOOL SendPacketUnuseItemFailed(AgpdItem *pcsItem);		//	2005.11.30. By SungHoon

	BOOL SendPacketPauseItem(AgpdItem* pcsItem);			// 2006.01.08. steeple

	BOOL SendPacketUpdateItemUseTime(AgpdItem *pcsItem);	// 20051202, kelovon
	BOOL SendPacketUpdateItemStaminaRemainTime(AgpdItem* pcsItem);	// 2008.06.09. steeple

	BOOL SendPacketUpdateCooldown(AgpdCharacter* pcsCharacter, AgpdItemCooldownBase stCooldownBase);
	BOOL SendPacketAllCooldown(AgpdCharacter* pcsCharacter);

protected:
	PVOID MakePacketItemForPartyMember(AgpdItem *pcsItem, INT32 lCID, INT16 *pnPacketLength);							//	2005.04.21 By SungHoon
	BOOL  SendItemToPartyMember(AgpdCharacter *pAgpdCharacter, AgpdItem *pcsItem);							//	2005.04.22 By SungHoon
public:
//	PVOID MakePacketItem(AgpdItem *pcsItem, INT16 *pnPacketLength);
//	PVOID MakePacketItemView(AgpdItem *pcsItem, INT16 *pnPacketLength, BOOL IsStaticPacket = FALSE);
	PVOID MakePacketItemRemove(AgpdItem *pcsItem, INT16 *pnPacketLength);
	PVOID MakePacketItemUpdateStatus(AgpdItem *pcsItem, INT16 *pnPacketLength);

	PVOID MakePacketItemBank(AgpdItem *pcsItem, INT16 *pnPacketLength);
	PVOID MakePacketItemQuest(AgpdItem *pcsItem, INT16 *pnPacketLength);

	PVOID MakePacketItemServerData(AgpdItem *pcsItem, INT16 *pnPacketLength, BOOL bLogin = TRUE);
	PVOID MakePacketItemADCharacter(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	PVOID	MakePacketAgpdDBData(AgpdItem *pcsItem, INT16 *pnPacketLength);
	PVOID	MakePacketAgsdDBData(AgpdItem *pcsItem, INT16 *pnPacketLength);

	PVOID	MakePacketUpdateReturnScrollStatus(AgpdCharacter *pcsCharacter, BOOL bIsEnable, INT16 *pnPacketLength);
	PVOID	MakePacketUseReturnScrollResultFailed(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	PVOID	MakePacketUseItemResult(AgpdCharacter *pcsCharacter, AgpmItemUseResult eResult, INT16 *pnPacketLength);

	//BOOL	SendPacketBasicDBData(AgpdItem *pcsItem, DPNID dpnid, BOOL bGroupNID = FALSE);

	//BOOL	BackupBasicItemData(AgpdItem *pcsItem);

	static BOOL CallbackSendCharEquipItem(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL SyncAddChar(AgpdCharacter *pcsCharacter);

	/*
	BOOL				SetCallbackDBStreamInsert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamDelete(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamSelect(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackDBStreamUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	*/

	BOOL				SetCallbackGetNewIID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackGetNewDBID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackGetNewCashItemBuyID(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackCreateNewItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);	// 아이템 생성 kelovon 20051123

	BOOL				SetCallbackUpdateItemDurability(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackSendItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackSendItemView(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackUseItemSkillBook(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUseItemSkillScroll(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUseItemSkillRollbackScroll(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUseItemReverseOrb(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUseItemTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackUseItemLotteryBox(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackPickupQuestValidCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackUseTeleportScroll(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackCheckReturnPosition(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackRideMount(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackRideDisMount(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCallbackPreCheckEnableSkillScroll(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	/*
	// get DB Query String.
	BOOL				GetInsertQuery( AgpdItem *pcsItem, char *pstrQuery );
	BOOL				GetSelectQuery( char *szCharName, char *pstrQuery );
	BOOL				GetUpdateQuery( AgpdItem *pcsItem, char *pstrQuery );
	BOOL				GetDeleteQueryByOwner( char *szCharName, char *pstrQuery );
	BOOL				GetDeleteQueryByDBID( AgpdItem *pcsItem, char *pstrQuery );

	BOOL				GetInsertHistoryDB(AgpdItem *pcsItem, char *pstrQuery, INT32 lHistoryIndex);
	BOOL				GetSelectHistoryDB(AgpdItem *pcsItem, char *pstrQuery, INT32 lHistoryIndex);
	BOOL				GetDeleteHistoryDB(AgpdItem *pcsItem, char *pstrQuery, INT32 lHistoryIndex);
	BOOL				GetDeleteHistoryAllDB(AgpdItem *pcsItem, char *pstrQuery);
	BOOL				GetUpdateOwnerQuery( AgpdItem *pcsItem, char *pstrQuery );

	// get DB Query String 2.
	BOOL				GetInsertQuery2( AgpdItem *pcsItem, char *pstrQuery );
	BOOL				GetSelectQuery2( char *szCharName, char *pstrQuery );
	BOOL				GetUpdateQuery2( AgpdItem *pcsItem, char *pstrQuery );
	BOOL				GetDeleteQueryByOwner2( char *szCharName, char *pstrQuery );
	BOOL				GetDeleteQueryByDBID2( AgpdItem *pcsItem, char *pstrQuery );

	BOOL				GetInsertHistoryDB2(AgpdItem *pcsItem, char *pstrQuery, INT32 lHistoryIndex);
	BOOL				GetSelectHistoryDB2(AgpdItem *pcsItem, char *pstrQuery);
	BOOL				GetDeleteHistoryDB2(AgpdItem *pcsItem, char *pstrQuery, INT32 lHistoryIndex);
	BOOL				GetDeleteHistoryAllDB2(AgpdItem *pcsItem, char *pstrQuery);
	BOOL				GetUpdateOwnerQuery2( AgpdItem *pcsItem, char *pstrQuery );
	*/

	// get DB Query String 3.
	/*
	BOOL				GetInsertQuery3( AgpdItem *pcsItem, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetSelectEquipQuery3( char *szCharName, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetSelectNotEquipQuery3( char *szCharName, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetSelectBankQuery3( char *szAccountID, char *pstrWorldDBName, char *pstrQuery, INT32 lQueryLength );
	BOOL				GetDeleteQueryByOwner3( char *szCharName, char *pstrQuery, INT32 lQueryLength );

	BOOL				GetInsertQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				SetParamInsertQuery4( COLEDB *pcOLEDB, AgpdItem *pcsItem );
	BOOL				GetSelectEquipQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				GetSelectNotEquipQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				GetSelectBankQuery4( char *pstrQuery, INT32 lQueryLength );
	BOOL				GetDeleteQueryByOwner4( char *pstrQuery, INT32 lQueryLength );
	*/

	BOOL				SetParamInsertQuery5(AuDatabase2 *pDatabase, AgpdItem *pcsItem);

	/*
	// db operation
	BOOL				StreamInsertDB(AgpdItem *pcsItem, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamSelectDB(CHAR *szCharName, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamUpdateDB(AgpdItem *pcsItem, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamDeleteDB(AgpdItem *pcsItem, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamDeleteDBByOwner(CHAR *szCharName, ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				StreamUpdateOwnerDB(AgpdItem *pcsItem, ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				StreamInsertHistoryDB(AgpdItem *pcsItem, INT32 lHistoryIndex, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamSelectHistoryDB(AgpdItem *pcsItem, INT32 lHistoryIndex, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamDeleteHistoryDB(AgpdItem *pcsItem, INT32 lHistoryIndex, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				StreamDeleteHistoryAllDB(AgpdItem *pcsItem, ApModuleDefaultCallBack pfCallback, PVOID pClass);
	*/

	// Get Query Result;
	//BOOL				GetSelectConvertHistoryResult(COLEDB *pcOLEDB, stAgpmItemConvertHistory *pstConvertHistory);
	//BOOL				GetSelectConvertHistoryResult2(COLEDB *pcOLEDB, stAgpmItemConvertHistory *pstConvertHistory, AgpdItem *pcsItem);

	/*
	BOOL				InsertItemDB(AgpdItem *pcsItem);
	BOOL				SaveItemDB(AgpdItem *pcsItem);
	BOOL				DeleteItemDB(AgpdItem *pcsItem);
	BOOL				DeleteItemByOwner(char *szCharName);

	BOOL				SaveItemOwner(AgpdItem *pcsItem);

	BOOL				AddItemConvertHistory(AgpdItem *pcsItem, INT32 lHistoryIndex);
	BOOL				SaveItemConvertHistory(AgpdItem *pcsItem);
	BOOL				DeleteAllConvertHistory(AgpdItem *pcsItem);

	static BOOL			CBSaveCharacterDB(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL			CBStreamDB(PVOID pData, PVOID pClass, PVOID pCustData);
	*/

	BOOL				AddItemToUseList(AgpdItem *pcsItem);
	BOOL				RemoveItemFromUseList(AgpdItem *pcsItem, BOOL bOnlyRemoveList = FALSE);

	BOOL				UseItem(AgpdItem *pcsItem, BOOL bIsFirstUse = FALSE, AgpdCharacter *pcsTargetChar = NULL);
	BOOL				CheckUsable(AgpdItem* pcsItem, BOOL bIsFirstUse = FALSE, AgpdCharacter *pcsTargetChar = NULL);

	INT32				UpdateAllInUseLimitTimeItem(AgpdCharacter *pcsCharacter, AgpdGrid m_csGrid, INT32 lElapsedTickCount, UINT32 lCurrentTimeStamp);
	BOOL				UpdateAllInUseLimitTimeItemTime(AgpdCharacter *pcsCharacter, INT32 lElapsedTickCount, UINT32 lCurrentTimeStamp);
	BOOL				UnuseItem(AgpdItem *pcsItem, BOOL bEndEffect = FALSE);			//	2005.11.30. By SungHoon
	BOOL				UnuseAvatarItem(AgpdItem* pcsItem);
	BOOL				PauseItem(AgpdItem* pcsItem);			//	2006.01.08. steeple

	BOOL				UseItemPotion(AgpdItem *pcsItem, BOOL bIsFirstUse = FALSE);
	BOOL				UseItemTeleportScroll(AgpdItem *pcsItem);
	BOOL				UseItemTransform(AgpdItem *pcsItem);
	BOOL				UseItemSkillBook(AgpdItem *pcsItem);
	BOOL				UseItemSkillScroll(AgpdItem *pcsItem, AgpdCharacter *pcsTargetChar, BOOL bIsWriteLog = TRUE);
	BOOL				UseItemSkillRollbackScroll(AgpdItem *pcsItem);
	BOOL				UseItemReverseOrb(AgpdItem *pcsItem);
	BOOL				UseItemRecallParty(AgpdItem *pcsItem);
	BOOL				UseItemLotteryBox(AgpdItem *pcsItem, BOOL bKeyCheck = TRUE);	// LotteryBox 2005.9.2 kelovon
	BOOL				UseItemLotteryKey(AgpdItem *pcsItem);	// LotteryKey 20051122 kelovon

	BOOL				UseItemAreaChattingRace(AgpdCharacter *pcsCharacter);
	BOOL				UseItemAreaChattingAll(AgpdCharacter *pcsCharacter);
	BOOL				UseItemAreaChattingGlobal(AgpdCharacter *pcsCharacter);
	BOOL				UseItemAddBankSlot(AgpdItem *pcsItem);
	BOOL				UseItemEffect(AgpdItem *pcsItem);
	BOOL				UseItemChatting(AgpdItem *pcsItem);
	BOOL				UseItemSkillInitialize(AgpdItem *pcsItem);
	BOOL				UseItemAvatar(AgpdItem* pcsItem);
	BOOL				UseItemPrivateTradeOption(AgpdItem* pcsItem);

	AgpdItem*			GetItemAreaChattingRace(AgpdCharacter *pcsCharacter);
	AgpdItem*			GetItemAreaChattingAll(AgpdCharacter *pcsCharacter);
	AgpdItem*			GetItemAreaChattingGlobal(AgpdCharacter *pcsCharacter);

	BOOL				UpdateItemDurability(INT32 lIID, INT32 lDurability, INT32 lNewMaxDurability = 0);
	BOOL				UpdateItemDurability(AgpdItem *pcsItem, INT32 lDurability, INT32 lNewMaxDurability = 0);

	BOOL				ReduceDurability(AgpdItem *pcsItem);
	BOOL				ReduceArmourDurability(AgpdCharacter *pcsCharacter);

	BOOL				CheckItemDurability(AgpdItem *pcsItem);
	FLOAT				GetReduceTypeValue(AgpdItem *pcsItem);

	BOOL				ProcessItemSkillPlus(AgpdItem* pcsItem, AgpdCharacter* pcsCharacter);		// 2007.02.05. steeple

	BOOL				AdjustRecalcFactor(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, BOOL bIsAdd, BOOL bCheckDragonScionWeapon = FALSE, BOOL bForced = FALSE);

	BOOL				AddItemToDB(AgpdItem *pcsItem);

	BOOL				AddItemToPartyMember(ApBase *pcsFirstLooter, ApBase *pcsDropCharacter, AgpdItem *pcsItem);		//	2005.04.22 By SungHoon

//	BOOL				MakeStringItemConvertHistory(AgpdItem *pcsItem, CHAR *szStringBuffer, INT32 lStringBufferSize);
//	BOOL				ParseStringItemConvertHistory(AgpdItem *pcsItem, CHAR *szStringBuffer);
	
	//////////////////////////////////////////////////////////////////////////
	// Log 관련 - 2004.05.04.steeple
	BOOL				WriteItemLog(eAGPDLOGTYPE_ITEM eType, INT32 lCID, AgpdItem* pAgpdItem, INT32 lCount,
									INT32 lGheld = 0, CHAR *pszTargetChar = NULL);
	BOOL				WritePickupLog(INT32 lCID, AgpdItem* pcsItem, INT32 lCount = 1);
	BOOL				WriteDropLog(INT32 lCID, AgpdItem* pcsItem, INT32 lCount = 1);
	BOOL				WriteUseLog(INT32 lCID, AgpdItem* pcsItem);
	BOOL				WriteUseStartLog(INT32 lCID, AgpdItem* pcsItem, INT32 lCount = 1);

	BOOL				EncodingOption(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferLength);
	BOOL				DecodingOption(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferLength);

	BOOL				EncodingSkillPlus(AgpdItem* pcsItem, CHAR* szBuffer, INT32 lBufferLength);
	BOOL				DecodingSkillPlus(AgpdItem* pcsItem, CHAR* szBuffer, INT32 lBufferLength);

	BOOL				EnterGameWorld(AgpdCharacter* pcsCharacter);
	BOOL				UpdateAllInUseCashItemTime(AgpdCharacter *pcsCharacter, INT32 lElapsedTickCount, UINT32 lCurrentTimeStamp);
	BOOL				UseAllEnableCashItem(AgpdCharacter* pcsCharacter, BOOL bRideCheck = FALSE, eAgsdItemPauseReason eReason = AGSDITEM_PAUSE_REASON_NONE, BOOL bIncludePause = FALSE);
	BOOL				UnUseAllCashItem(AgpdCharacter* pcsCharacter, BOOL bRideCheck = FALSE, eAgsdItemPauseReason eReason = AGSDITEM_PAUSE_REASON_NONE);
	BOOL				UsePausedCashItem(AgpdCharacter* pcsCharacter, eAgsdItemPauseReason eReason);
	
	UINT64				WriteCashItemBuyList(AgpdCharacter *pcsCharacter, stCashItemBuyList *pstList);
	UINT64				WriteCashItemBuyList(stCashItemBuyList *pstList, CHAR *pszAccName, CHAR *pszCharName);
	BOOL				UpdateCashItemBuyList_Complete(UINT64 ullItemSeq, UINT64 ullOrderNo, UINT64 ullBuyID);
	BOOL				UpdateCashItemBuyList_Complete2(UINT64 ullItemSeq, CHAR *pszOrderID, UINT64 ullBuyID);
	BOOL				UpdateCashItemBuyList_Other(INT8 cStatus, UINT64 ullItemSeq);

	BOOL				CalcItemOptionSkillData(AgpdItem* pcsItem, BOOL bAdd = TRUE, INT32 lPrevLevel = 0);
	BOOL				UpdateItemOptionSkillData(AgpdCharacter* pcsCharacter, BOOL bFirst = FALSE, INT32 lPrevLevel = 0);
	static BOOL			CBAddItemOption(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL				ProcessLimitedLevelItemOnLevelUp(AgpdCharacter* pcsCharacter);

	BOOL				EncodingCooldown(AgpdCharacter* pcsCharacter, CHAR* szBuffer, INT32 lBufferLength);
	BOOL				DecodingCooldown(AgpdCharacter* pcsCharacter, CHAR* szBuffer, INT32 lBufferLength);

	BOOL				CheckStatminaPet(AgpdItem* pcsItem);

	BOOL				IsCharacterUsingTimeLimitItem(AgpdCharacter *pcsCharacter);
};

#endif //__AGSMITEM_H__
