/*======================================================================

	AgsmRelay2.h

======================================================================*/

#ifndef _AGSM_RELAY2_H_
	#define _AGSM_RELAY2_H_


#include "AgsEngine.h"
#include "AgpmConfig.h"
#include "AgsmServerManager2.h"
#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgsmCharacter.h"
#include "AgpmItem.h"
#include "AgsmItem.h"
#include "AgsmItemConvert.h"
#include "AgpmSkill.h"
#include "AgsmSkill.h"
#include "AgsmProduct.h"
#include "AgsmEventSkillMaster.h"
#include "AgsmProduct.h"
#include "AgsmUIStatus.h"
#include "AgpmGuild.h"
#include "AgsmGuild.h"
#include "AgsmLog.h"
#include "AgpmQuest.h"
#include "AgsmQuest.h"
#include "AgsmBuddy.h"
#include "AgsmMailBox.h"
#include "AgsmAdmin.h"
#include "AgsmAuctionRelay.h"
#include "AgsmCashMall.h"
#include "AuCircularBuffer.h"
#include "AgsmReturnToLogin.h"
#include "AgsmWantedCriminal.h"
#include "AgsmSiegeWar.h"
#include "AgsmTax.h"
#include "AgsmGuildWarehouse.h"
#include "AgsmArchlord.h"
#include "AgsmServerStatus.h"
#include "AgpmTitle.h"
#include "AgsmTitle.h"

#include "AuDatabase.h"
#include "AgsmDatabasePool.h"
#include "AgsdRelay2.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
#define AGMSRELAY2_LOG_FILENAME1		_T("LOG\\RELAY.log")
#define AGMSRELAY2_LOG_FILENAME_THREAD	_T("LOG\\RELAY_THREAD.log")
#define AGMSRELAY2_LOG_FILENAME_ITEMSEQ	_T("LOG\\RELAY_ITEMSEQ.log")

const INT32		AGSMRELAY2_ROWSET_IND_EOF	= (-1000);

enum eRelayQueryIndex
{
	AGSMRELAY_QUERY_WORLD_CHAMPIONSHIP_REQUEST	= 1000,
	AGSMRELAY_QUERY_WORLD_CHAMPIONSHIP_ENTER	= 1001,
	AGSMRELAY_QUERY_ATTENDANCE					= 1002,
	AGSMRELAY_QUERY_EVENTUSERCHECK				= 1003,
	AGSMRELAY_QUERY_EVENTUSERFLAGUPDATE			= 1004,
	AGSMRELAY_QUERY_CHANGENAME					= 1005,
	AGSMRELAY_QUERY_CERARIUMORB_REQUEST_LIST	= 1006,
	AGSMRELAY_QUERY_CERARIUMORB_REQUEST_CHARACTERNAME	= 1007,
	AGSMRELAY_QUERY_CERARIUMORB_SEALING			= 1008,
	AGSMRELAY_QUERY_CERARIUMORB_SEALING_DATA	= 1009,
	AGSMRELAY_QUERY_CERARIUMORB_SEALING_DATA_ALL	= 1010,
	AGSMRELAY_QUERY_CERARIUMORB_RELEASE_SEAL_REQUEST	= 1011,
	AGSMRELAY_QUERY_CASHITEMTIMEEXTEND			= 1012,
	AGSMRELAY_QUERY_BANKMONEYEDIT				= 1013,
	AGSMRELAY_QUERY_CHARISMAPOINTEDIT			= 1014,
	AGSMRELAY_QUERY_TITLE_ADD					= 1015,
	AGSMRELAY_QUERY_TITLE_USE					= 1016,
	AGSMRELAY_QUERY_TITLE_LIST					= 1017,
	AGSMRELAY_QUERY_TITLE_QUEST_REQUEST			= 1018,
	AGSMRELAY_QUERY_TITLE_QUEST_CHECK			= 1019,
	AGSMRELAY_QUERY_TITLE_QUEST_LIST			= 1020,
	AGSMRELAY_QUERY_TITLE_QUEST_COMPLETE		= 1021,
	AGSMRELAY_QUERY_TITLE_DELETE				= 1022,
	AGSMRELAY_QUERY_EVENT_CREATIONDATE_CHECK	= 1023,
	AGSMRELAY_QUERY_SERVERMOVE					= 1024,//JK_특성화서버

};



/************************************************/
/*		The Definition of Relay Sender Thread	*/
/************************************************/
//
class Relay2Sender : public zzThread
	{
	protected:
		CHAR				m_szName[20];
		AuCircularBuffer	m_csBuffer;
		AgsModule			*m_pModule;
		AgsdServer			*m_pServer;
		BOOL				m_bStop;

		BOOL				m_bInit;
		ApMutualEx			m_csLock;
		deque<PVOID>		m_QueueItem;
		deque<INT16>		m_QueueLength;

	protected:
		//	zzThread inherited
		BOOL		OnCreate();
		void		OnTerminate();
		DWORD		Do();

		//	Internal
		PVOID	Get(INT16 &nLength);
		void	Remove(PVOID pv);
		BOOL	Send(BOOL bAll = FALSE);

	public:
		Relay2Sender();
		virtual ~Relay2Sender();

		//	Stop
		void Stop();

		//	Set(Init)
		BOOL Set(INT32 lBufferSize, AgsModule* pModule, AgsdServer* pServer, CHAR *pszName = NULL);

		//	Push(to send)
		BOOL Push(PVOID pvPacket, INT16 nPacketLength);

		BOOL SendAll();
	};


//#define _RELAYDEBUG
class AgsmEventSystem;
class AgsmItemManager;

/************************************************/
/*		The Definition of AgsmRelay2 class		*/
/************************************************/
//
class AgsmRelay2 : public AgsModule
	{
	protected:
		//	Related Modules
		AgpmConfig				*m_pAgpmConfig;
		
		AgsmServerManager		*m_pAgsmServerManager;
		AgsmServerManager2		*m_pAgsmServerManager2;
		AgpmCharacter			*m_pAgpmCharacter;
		AgsmCharacter			*m_pAgsmCharacter;
		AgpmItem				*m_pAgpmItem;
		AgsmItem				*m_pAgsmItem;
		AgsmItemConvert			*m_pAgsmItemConvert;
		AgpmSkill				*m_pAgpmSkill;
		AgsmSkill				*m_pAgsmSkill;
		AgsmProduct				*m_pAgsmProduct;			
		AgsmEventSkillMaster	*m_pAgsmEventSkillMaster;
		AgpmFactors				*m_pAgpmFactors;
		AgsmUIStatus			*m_pAgsmUIStatus;
		AgpmGuild				*m_pAgpmGuild;
		AgsmGuild				*m_pAgsmGuild;
		AgsmInterServerLink		*m_pAgsmInterServerLink;
		AgsmLog					*m_pAgsmLog;
		AgsmQuest				*m_pAgsmQuest;
		AgsmAuctionRelay		*m_pAgsmAuctionRelay;
		AgpmAdmin				*m_pAgpmAdmin;
		AgsmAdmin				*m_pAgsmAdmin;
		AgsmDeath				*m_pAgsmDeath;
		AgsmBuddy				*m_pAgsmBuddy;
		AgpmMailBox				*m_pAgpmMailBox;
		AgsmMailBox				*m_pAgsmMailBox;
		AgsmCashMall			*m_pAgsmCashMall;
		AgsmReturnToLogin		*m_pAgsmReturnToLogin;
		AgsmWantedCriminal		*m_pAgsmWantedCriminal;
		AgsmSiegeWar			*m_pAgsmSiegeWar;
		AgsmTax					*m_pAgsmTax;
		AgsmGuildWarehouse		*m_pAgsmGuildWarehouse;
		AgsmArchlord			*m_pAgsmArchlord;
		AgsmEventSystem			*m_pagsmEventSystem;

		AgsmDatabasePool		*m_pAgsmDatabasePool;
		AgsmServerStatus		*m_pAgsmServerStatus;

		AgpmTitle				*m_pAgpmTitle;
		AgsmTitle				*m_pAgsmTitle;

		//	Sender
		Relay2Sender			*m_pSender;
		Relay2Sender			*m_pSenderLog;

		//	Packets
		AuPacket m_csPacket;
		AuPacket m_csPacketCharacter;
		AuPacket m_csPacketItem;
		AuPacket m_csPacketItemConvert;
		AuPacket m_csPacketSkill;
		AuPacket m_csPacketUIStatus;
		AuPacket m_csPacketCustom;
		AuPacket m_csPacketGuildMaster;
		AuPacket m_csPacketGuildMember;
		AuPacket m_csPacketAccountWorld;
		AuPacket m_csPacketQuest;
		AuPacket m_csPacketQuestFlag;
		AuPacket m_csPacketConcurrentUser;
		AuPacket m_csPacketAdmin;
		AuPacket m_csPacketBuddy;
		AuPacket m_csPacketMail;
		AuPacket m_csPacketMailItem;
		AuPacket m_csPacketRowset;
		AuPacket m_csPacketCashItemBuyList;
		AuPacket m_csPacketWantedCriminal;
		AuPacket m_csPacketCastle;
		AuPacket m_csPacketSiege;
		AuPacket m_csPacketSiegeApplication;
		AuPacket m_csPacketSiegeObject;
		AuPacket m_csPacketTax;
		AuPacket m_csPacketGuildItem;
		AuPacket m_csPacketArchlord;
		AuPacket m_csPacketLordGuard;
				
	protected:
		//	Packet Initialization
		void	InitPacket();
		void	InitPacketCharacter();
		void	InitPacketItem();
		void	InitPacketItemConvert();
		void	InitPacketSkill();
		void	InitPacketUIStatus();
		void	InitPacketCustom();
		void	InitPacketGuildMaster();
		void	InitPacketGuildMember();
		void	InitPacketAccountWorld();
		void	InitPacketQuest();
		void	InitPacketConcurrentUser();
		void	InitPacketAdmin();
		void	InitPacketBuddy();
		void	InitPacketMail();
		void	InitPacketCashItemBuyList();
		void	InitPacketWantedCriminal();
		void	InitPacketSiegeWar();
		void	InitPacketGuildItem();
		void	InitPacketArchlord();
		void	InitPacketLordGuard();

		//	Make Packet (Final)
		PVOID	MakeRelayPacket(BOOL bPacket, INT16* pnPacketLength, INT16 nParam, INT8 cStatus, PVOID pvPacketEmb);

		// Make rowset packet
		PVOID	MakeRowsetPacket(AuRowset *pRowset);
		PVOID	MakeRowsetPacket2(AuRowset *pRowset, INT32 lStep, INT32 lRowsPerStep);

		//	Make and Send Packet
		BOOL	MakeAndSendRelayPacket(PVOID pvPacketEmb, INT16 nParam, UINT32 ulNID);
		BOOL	MakeAndSendRelayPacket(PVOID pvPacketEmb, INT16 nParam);

		//	Packet Processing
		BOOL	OnParamCharacter(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamItem(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamItemConvert(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamSkill(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamUIStatus(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamCustom(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamGuildMaster(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamGuildMember(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamGuildItem(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamAccountWorld(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamQuest(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamQuestFlag(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamConcurrentUser(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamPing(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamAdmin(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamBuddy(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamMail(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamMailItem(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamCashItemBuyList(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamWantedCriminal(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamNotifySaveAll(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamCastle(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamSiege(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamSiegeApplication(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamSiegeObject(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamTax(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamArchlord(INT16 nParam, PVOID pvPacket, UINT32 ulNID);
		BOOL	OnParamLordGuard(INT16 nParam, PVOID pvPacket, UINT32 ulNID);

	public:
		AgsmRelay2();
		virtual ~AgsmRelay2();

		//	Module inherited
		virtual BOOL OnAddModule();
		virtual BOOL OnInit();
		virtual BOOL OnDestroy();
		virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

		//	Callback setting method for Relay Server		
		BOOL		EnableBufferingSend();
		BOOL		SetCallbackOperation();

		//	Connect callback
		static BOOL CBServerConnect(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Operation callback (relay-side after receive callback)
		static BOOL	CBOperation(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationCustom(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationGuildMasterCheck(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationGuildMember(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationGuildItem(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationLog(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationPing(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationAdmin(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationBuddy(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationMail(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationMailItem(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationWantedCriminal(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationCastle(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationSiege(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationSiegeApplication(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationSiegeObject(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationLordGuard(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Finish operation callback (relay-side after execution callback)
		static BOOL	CBFinishOperation(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFailOperation(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationGuildMasterCheck(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationGuildMember(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationGuildItem(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBFinishOperationPing(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBFinishOperationAdmin(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBFinishOperationBuddy(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBFinishOperationMail(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationMailItem(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationWantedCriminal(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationCastle(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationSiege(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationSiegeApplication(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationSiegeObject(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBFinishOperationLordGuard(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Select result (relay-side select query result processing)
		BOOL	OnSelectResult(AuRowset *pRowset, AgsdDBParam *pAgsdRelay);
		BOOL	OnSelectResultGuildMaster(AuRowset *pRowset, AgsdDBParam *pAgsdRelay, BOOL bCheck = FALSE);
		BOOL	OnSelectResultGuildMember(AuRowset *pRowset, AgsdDBParam *pAgsdRelay);
		BOOL	OnSelectResultGuildItem(AuRowset *pRowset, AgsdDBParam *pAgsdRelay);
		BOOL	OnSelectResultPing(AuRowset *pRowset, AgsdDBParam *pAgsdRelay);
		BOOL	OnSelectResultAdmin(AuRowset *pRowset, AgsdDBParam *pAgsdRelay);
		BOOL	OnSelectResultBuddy(AuRowset *pRowset, AgsdDBParam *pAgsdRelay);
		BOOL	OnSelectResultMail(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);
		BOOL	OnSelectResultWantedCriminal(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);
		BOOL	OnSelectResultOffCharInfo(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);
		BOOL	OnInsertResultMail(AgsdDBParam *pAgsdRelay);
		BOOL	OnSelectResultMailItem(AgsdDBParam *pAgsdRelay2);
		BOOL	OnSelectResultCastle(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);
		BOOL	OnSelectResultSiege(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);
		BOOL	OnSelectResultSiegeApplication(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);
		BOOL	OnSelectResultSiegeObject(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);
		BOOL	OnSelectResultArchlord(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);
		BOOL	OnSelectResultLordGuard(AuRowset *pRowset, AgsdDBParam *pAgsdRelay2);

		//	Callback Setting (relay side operation callback setting)
		BOOL	SetCallbackCustom(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackReverseOrb(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackTransform(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackCharacterGameIn(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackCharacterGameOut(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackItemConvert(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSkillSave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackUIStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGuildMaster(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGuildMasterCheck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGuildMember(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGuildBattleHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGuildRename(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGuildRelation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGuildWarehouseMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackGuildItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackAccountWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackQuest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackQuestFlag(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackConcurrentUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackPing(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBuddy(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackMail(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackMailItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackCashItemBuyList(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackCashItemBuyList2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackWantedCriminal(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackCastle(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSiege(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSiegeApplication(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackSiegeObject(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackTax(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackLordGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		//	Callback (Game server side callbacks)
		static BOOL CBCustom(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCharacterInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCharacterUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCharacterDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBUseReverserOrb(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBTransform(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCharacterGameIn(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCharacterGameOut(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBNotifySaveAll(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBItemInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBItemUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBItemDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBItemConvertUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSkillInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSkillUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSkillDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBUIStatusUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMasterInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMasterUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMasterDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMasterSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMasterCheck(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMemberInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMemberUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMemberDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildMemberSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildBattleHistroyInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildRename(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGuildJointSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildJointInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildJointUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildJointDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildHostileInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBGuildHostileDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGuildWarehouseMoneyUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGuildItemSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGuildItemInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBGuildItemDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAccountWorldUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBCurrentQuest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBQuestFlag(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBConcurrentUser(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBPingSend(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBBufferingSend(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBBuddyInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBBuddyRemove(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBBuddyRemove2(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBBuddyOptions(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBBuddyOptions2(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBBuddySelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBMailInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBMailUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBMailDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBMailSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBMailItemSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCashItemBuyListUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCashItemBuyListUpdate2(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCashItemBuyListInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBWantedCriminalSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBWantedCriminalUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBWantedCriminalInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBWantedCriminalDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBWantedCriminalOffCharInfo(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCastleSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBCastleUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeApplicationSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeApplicationUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeApplicationInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeObjectSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeObjectUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeObjectInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBSiegeObjectDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBTaxUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBArchlordSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBArchlordUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBArchlordInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBLordGuardSelect(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBLordGuardInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBLordGuardDelete(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBSearchAccount(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSearchAccountBank(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminUpdateCharacter2(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminUpdateBankMoney(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminUpdateCharismaPoint(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminItemInsert(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminItemDelete(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminItemConvertUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminItemConvertUpdate2(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminItemUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBAdminSkillUpdate(PVOID pData, PVOID pClass, PVOID pCustData);

		//	Operation result callback (game-side after execution callback)
		static BOOL	CBOperationResultGuildMaster(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultGuildMasterCheck(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultGuildMember(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultGuildItem(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultGuildRelation(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultPing(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultAdmin(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultBuddy(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultMail(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultMailItem(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultWantedCriminal(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultCastle(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultSiege(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultSiegeApplication(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultSiegeObject(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL	CBOperationResultLordGuard(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL	CBOperationResultWorldChampionshipResult(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultAttendance(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultEventUser(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultChangeName(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultCerariumOrb(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationResultAddTitleSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultAddTitleFail(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultUseTitleSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultUseTitleFail(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultListTitleSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultListTitleFail(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationResultTitleQuestRequestSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultTitleQuestRequestFail(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationResultTitleQuestCheckSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultTitleQuestCheckFail(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationResultTitleQuestCompleteSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultTitleQuestCompleteFail(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationResultTitleQuestListSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultTitleQuestListFail(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationResultTitleDeleteSuccess(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBOperationResultTitleDeleteFail(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationResultCashItemTimeExtend(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationCharacterCreationDateCheckSuccess(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBOperationResultServerMove(PVOID pData, PVOID pClass, PVOID pCustData);//JK_특성화서버

		BOOL ParseBuddyRowsetPacket(PVOID pvPacket, stBuddyRowset* pstRowset);

		//	Request
		BOOL	SendCharacterUpdate(AgpdCharacter* pAgpdCharacter);
		BOOL	SendUseReverseOrb(AgpdCharacter *pcsCharacter);
		BOOL	SendTransform(AgpdCharacter *pAgpdCharacter);
		BOOL	SendCharacterGameInOut(AgpdCharacter *pAgpdCharacter, BOOL bIn);
		BOOL	SendNotifySaveAll(CHAR *pszName, UINT32 ulNID);
		BOOL	ReceiveNotifySaveAll(CHAR *pszName);
		BOOL	SendItemInsert(AgpdItem* pAgpdItem, UINT64 ullDBID);
		BOOL	SendItemUpdate(AgpdItem* pAgpdItem, UINT64 ullDBID);
		BOOL	SendItemDelete(UINT64 ullDBID, CHAR *szDeleteReason, BOOL bCash = FALSE);
		BOOL	SendGuildItem(AgsdRelay2GuildItem *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendItemConvertUpdate(AgpdItem *pAgpdItem);
		BOOL	SendSkillUpdate(ApBase *pcsBase);
		BOOL	SendUIStatusUpdate(AgpdCharacter *pcsCharacter);
		BOOL	SendGuildMasterInsert(AgpdGuild *pcsGuild);
		BOOL	SendGuildMasterUpdate(AgpdGuild *pcsGuild);
		BOOL	SendGuildMasterDelete(CHAR *pszGuildID);
		BOOL	SendGuildMasterSelect();
		BOOL	SendGuildMasterCheck(AgpdGuild* pcsGuild);
		BOOL	SendGuildMemberInsert(CHAR *pszGuildID, AgpdGuildMember *pcsMember);
		BOOL	SendGuildMemberUpdate(CHAR *pszGuildID, AgpdGuildMember *pcsMember);
		BOOL	SendGuildMemberDelete(CHAR *pszGuildID, CHAR *pszCharID);
		BOOL	SendGuildMemberSelect();
		BOOL	SendGuildMemberLoadingComplete(AgsdDBParam *pAgsdRelay2);
		BOOL	SendGuildBattleHistroyInsert(AgpdGuild* pcsGuild);
		BOOL	SendGuildRename(CHAR *pszOldGuildID, CHAR *pszNewGuildID);
		BOOL	SendGuildJointSelect(CHAR* szGuildID, CHAR* szJointGuildID, UINT32 ulDate, INT8 cRelation, UINT32 ulNID = 0);
		BOOL	SendGuildJointInsert(CHAR* szGuildID, CHAR* szJointGuildID, UINT32 ulDate, INT8 cRelation);
		BOOL	SendGuildJointUpdate(CHAR* szGuildID, CHAR* szJointGuildID, INT8 cRelation);
		BOOL	SendGuildJointDelete(CHAR* szGuildID, CHAR* szJointGuildID);
		BOOL	SendGuildHostileInsert(CHAR* szGuildID, CHAR* szHostileGuildID, UINT32 ulDate);
		BOOL	SendGuildHostileDelete(CHAR* szGuildID, CHAR* szHostileGuildID);
		BOOL	SendGuildWarehouseMoneyUpdate(AgpdGuildWarehouse *pAgpdGuildWarehouse);
		BOOL	SendAccountWorldUpdate(CHAR *pszAccountID, CHAR *pszServerName, INT64 llBankMoney, INT8 cBankSize);
		BOOL	SendBankMoneyEdit(CHAR *pszAccountID, CHAR *pszServerName, INT64 llBankMoney);
		BOOL	SendCharismaPointEdit(CHAR *pszCharID, INT32 lCharismaPoint);
		BOOL	SendQuest(AgsdRelay2Quest *pQuest);
		BOOL	SendQuestFlag(AgsdRelay2QuestFlag *pQuestFlag);
		BOOL	SendConcurrentUser(CHAR *pszServer, CHAR *pszTID, INT32 lCurrent, INT32 lMax);
		BOOL	SendPing(DWORD dwSendTickCount);
		BOOL	SendCustom(INT16 nOperation, CHAR* pszQuery, UINT32 ulCols, PVOID pBuffer,
						   INT16 nBufferSize, PVOID pOffsets, INT16 nOffsetsSize);
		BOOL	SendCustom(INT16 nOperation, INT32 lQueryIndex, UINT32 ulCols, PVOID pBuffer,
						   INT16 nBufferSize, PVOID pOffsets, INT16 nOffsetsSize);
		BOOL	SendBuddy(AgsdRelay2Buddy *pBuddy, PVOID pvRowsetPacket, UINT32 ulNID = 0);
		BOOL	SendMail(AgsdRelay2Mail *pAgsdRelay2Mail, UINT32 ulNID = 0);
		BOOL	SendMailContent(AgsdRelay2Mail *pAgsdRelay2Mail, UINT32 ulNID = 0);
		BOOL	SendMailItem(AgsdRelay2MailItem *pAgsdRelay2MailItem, UINT32 ulNID = 0);
		BOOL	SendCashItemBuyListInsert(AgsdRelay2CashItemBuyList *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendCashItemBuyListUpdate(AgsdRelay2CashItemBuyList *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendCashItemBuyListUpdate2(AgsdRelay2CashItemBuyList *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendWantedCriminal(AgsdRelay2WantedCriminal *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendCastle(AgsdRelay2Castle *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendSiege(AgsdRelay2Siege *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendSiegeApplication(AgsdRelay2SiegeApplication *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendSiegeObject(AgsdRelay2SiegeObject *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendTax(AgsdRelay2Tax *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendArchlord(AgsdRelay2Archlord *pAgsdRelay2, UINT32 ulNID = 0);
		BOOL	SendLordGuard(AgsdRelay2LordGuard *pAgsdRelay2, UINT32 ulNID = 0);

		BOOL	PushToSendQueue(PVOID pvPacket, INT16 nPacketLength);
		BOOL	SendAll();
		BOOL	OnParamSkillSave(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamWorldChampionship(PACKET_HEADER* pvPacket, UINT32 ulNID);
		
		BOOL	OnParamAttendance(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamItemGiveEvent(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamChangeName(PACKET_HEADER* pvPacket, UINT32 ulNID);

		BOOL	OnParamCashItemTimeExtend(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamCerariumOrb(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamBankMoneyEdit(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamCharismaPointEdit(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamTitle(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamTitleQuest(PACKET_AGSP_TITLE_QUEST* pvPacket, UINT32 ulNID);
		BOOL	OnParamCharacterCreationDate(PACKET_HEADER* pvPacket, UINT32 ulNID);
		BOOL	OnParamServerMove(PACKET_HEADER* pvPacket, UINT32 ulNID);//JK_특성화서버

	protected:
		BOOL	SendSelectResultGuildMaster(UINT32 ulNID, AgsdRelay2GuildMaster *pcsGuildMaster, BOOL bCheck);
		BOOL	SendSelectResultGuildMember(UINT32 ulNID, AgsdRelay2GuildMember *pcsGuildMember);
		BOOL	SendSelectResultPing(UINT32 ulNID, DWORD dwSendTickCount);
		BOOL	SendSelectResultAdmin(UINT32 ulNID, AgsdRelay2Admin* pcsAdmin);
		BOOL	SendCustom(AgsdDBParamCustom *pCustom, INT16 nParam, INT32 ulNID);

		BOOL	OperationLogStatic(AgsdDBParam *pAgsdRelay2, INT16 nParam);
		
		// Extra
		PVOID	MakeItemPacket(AgpdItem *pAgpdItem, UINT64 ullDBID, INT16 nOperation, INT16 *pnPacketLength, CHAR *pszGuildID = NULL,
								PVOID pvExtra = NULL, INT16 *pnExtraLength = NULL, stAgpdAdminItemOperation* pstItemOperation = NULL);

  #ifdef _AGSMRELAY2_TEST
	public:
		BOOL	TestSendCharacterUpdate();
		BOOL	TestSendItemInsert(UINT64 ullID = 0);
		BOOL	TestSendItemUpdate();
		BOOL	TestSendItemDelete(UINT64 ullID = 0);
		BOOL	TestSendItemConvertUpdate();
		BOOL	TestSendSkillUpdate();
		BOOL	TestSendUIStatusUpdate();
		BOOL	TestSendGuildMasterInsert();
		BOOL	TestSendGuildMasterUpdate();
		BOOL	TestSendGuildMasterDelete();
		BOOL	TestSendGuildMasterSelect();
		BOOL	TestSendGuildMemberInsert();
		BOOL	TestSendGuildMemberUpdate();
		BOOL	TestSendGuildMemberDelete();
		BOOL	TestSendGuildMemberSelect();
		BOOL	TestSendAccountWorldUpdate();
		BOOL	TestLog(INT16 n);
  #endif
	};

#endif