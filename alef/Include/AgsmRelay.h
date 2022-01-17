// AgsmRelay.h: interface for the AgsmRelay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMRELAY_H__DC507B62_BA16_45CE_968E_B75256F10C63__INCLUDED_)
#define AFX_AGSMRELAY_H__DC507B62_BA16_45CE_968E_B75256F10C63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgsEngine.h"
#include "AgsmServerManager2.h"
#include "AgpmFactors.h"

#include "AgpmCharacter.h"
#include "AgsmCharacter.h"

#include "AgpmItem.h"
#include "AgsmItem.h"
#include "AgsmItemConvert.h"

#include "AgpmSkill.h"
#include "AgsmSkill.h"
#include "AgsmEventSkillMaster.h"

#include "AgsmUIStatus.h"

#include "AgsmGuild.h"

#include "AgsmAdmin.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgsmRelayD" )
#else
#pragma comment ( lib , "AgsmRelay" )
#endif
#endif

const INT32 MAX_PARAM_CHARACTER = 10;
const INT32 MAX_PARAM_ITEM		= 10;
const INT32 MAX_PARAM_SKILL		= 10;

typedef enum AgsmRelayPacketOperation
{
	/*
	AGSMRELAY_OPERATION_CHARACTER		= 1,
	AGSMRELAY_OPERATION_ITEM,
	AGSMRELAY_OPERATION_ITEM_CONVERT_HISTORY,
	AGSMRELAY_OPERATION_SKILL,
	AGSMRELAY_OPERATION_LOGIN,
	*/

	AGSMRELAY_OPERATION2_CHARACTER		= 1,
	AGSMRELAY_OPERATION2_ITEM,
	AGSMRELAY_OPERATION2_ITEM_CONVERT_HISTORY,
	AGSMRELAY_OPERATION2_SKILL,
	AGSMRELAY_OPERATION2_LOGIN,
	AGSMRELAY_OPERATION2_UI_STATUS,
	AGSMRELAY_OPERATION2_GUILD_MASTER,
	AGSMRELAY_OPERATION2_GUILD_MEMBER,
	AGSMRELAY_OPERATION2_ACCOUNT_WORLD,
	AGSMRELAY_OPERATION2_SEARCH_CHARACTER,
	AGSMRELAY_OPERATION2_MAX,
} AgsmRelayPacketCategory;

typedef enum AgsmRelayDBOperation
{
	AGSMRELAY_DB_OPERATION_INSERT	= 1,
	AGSMRELAY_DB_OPERATION_UPDATE,
	AGSMRELAY_DB_OPERATION_DELETE,
	AGSMRELAY_DB_OPERATION_SELECT,
	AGSMRELAY_DB_OPERATION_SELECT_RESULT,
	AGSMRELAY_DB_OPERATION_CHAR_LIST_RESULT,
} AgsmRelayPacketDBOperation;

/*
struct AgsdRelayCharacter
{
	CHAR*	pszAccountID;
	CHAR*	pszName;
	INT32	lTemplateID;
	CHAR*	pszTitle;
	AuPOS	stPosition;
	INT8	lCriminalStatus;
	INT32	lMurderPoint;
	INT64	lInventoryMoney;
	INT64	lBankMoney;
	UINT32	ulRemainCriminalTime;
	UINT32	ulRemainMurderTime;
	INT32	lHP;
	INT32	lMP;
	INT32	lSP;
	INT32	lExp;
	INT32	lLevel;

	UINT32	ulNID;
};

struct AgsdRelayItem
{
	CHAR*	pszCharName;
	INT32	lTemplateID;
	INT32	lStackCount;
	INT16	lStatus;
	AuPOS	stPosition;
	INT16	lGridTab;
	INT16	lGridRow;
	INT16	lGridColumn;
	INT32	lNeedLevel;
	UINT64	lDBID;

	UINT32	ulNID;
};

struct AgsdRelayItemConvertHistory
{
	INT8	cType;
	INT32	lAttribute;
	INT32	lValue;
	UINT64	ullDBID;

	UINT32	ulNID;
};

struct AgsdRelaySkill
{
	CHAR*	pszCharName;
	INT32	lMasteryIndex;
	INT32	lTotalSkillPoint;
	CHAR*	pszTreeNode;
	INT8	lInstSpec;
	UINT32	ulRemainInstSepc;
	INT8	lConstSpec;

	UINT32	ulNID;
};
*/

struct AgsdRelayCharacter2
{
	CHAR*	pszName;
	CHAR*	pszTitle;
	CHAR*	pszPosition;
	INT8	lCriminalStatus;
	INT32	lMurderPoint;
	INT64	lInventoryMoney;
	UINT32	ulRemainCriminalTime;
	UINT32	ulRemainMurderTime;
	INT32	lHP;
	INT32	lMP;
	INT32	lSP;
	INT32	lExp;
	INT32	lLevel;
	INT32	lSkillPoint;

	UINT32	ulNID;
};

struct AgsdRelayItem2
{
	UINT64	lDBID;
	CHAR*	pszCharName;
	INT32	lTemplateID;
	INT32	lStackCount;
	INT16	lStatus;
	CHAR*	pszPosition;
	INT32	lNeedLevel;
	INT32	lDurability;
	PVOID*	pDeleteReasonString;
	INT16	nDeleteReasonLength;

	UINT32	ulNID;
};

struct AgsdRelayItemConvertHistory2
{
	UINT64	ullDBID;
	PVOID	*pConvertHistoryString;
	INT16	nStringLength;

	UINT32	ulNID;
};

struct AgsdRelaySkill2
{
	CHAR*	pszCharName;
//	INT32	lMasteryIndex;
//	INT32	lTotalSkillPoint;
	CHAR*	pszTreeNode;
	CHAR*	pszSpecialize;

	UINT32	ulNID;
};

struct AgsdRelayUIStatus2
{
	CHAR*	pszCharName;
	CHAR*	pszQBeltString;
	INT16	nStringLength;

	UINT32	ulNID;
};

// 2004.07.01. steeple
struct AgsdRelayGuildMaster2
{
	CHAR* m_szGuildID;
	CHAR* m_szMasterID;
	
	INT32 m_lGuildTID;
	INT32 m_lGuildRank;
	INT32 m_lCreationDate;
	INT32 m_lMaxMemberCount;
	INT32 m_lUnionID;
	
	CHAR* m_szPassword;

	UINT32 m_ulNID;
};

// 2004.07.01. steeple
struct AgsdRelayGuildMember2
{
	CHAR* m_szMemberID;
	CHAR* m_szGuildID;
	INT32 m_lMemberRank;
	INT32 m_lJoinDate;
	INT32 m_lLevel;
	INT32 m_lTID;

	UINT32 m_ulNID;
};

struct AgsdRelayAccountWorld
{
	CHAR	*m_szAccountID;
	INT64	m_llBankMoney;

	UINT32	m_ulNID;
};

// 2004.08.16. steeple
struct AgsdRelaySearchCharacter
{
	CHAR* m_szAdminName;
	
	CHAR* m_szCharName;
	CHAR* m_szAccName;
	
	INT32 m_lLevel;
	INT32 m_lRace;
	INT32 m_lClass;
	INT32 m_lStatus;

	CHAR* m_szCreationDate;

	UINT32	m_ulNID;
};


class AgsmRelay : public AgsModule
{
private:	// Module 
	AgsmServerManager*	m_pAgsmServerManager;

	AgpmCharacter*		m_pAgpmCharacter;
	AgsmCharacter*		m_pAgsmCharacter;

	AgpmItem*			m_pAgpmItem;
	AgsmItem*			m_pAgsmItem;
	AgsmItemConvert*	m_pAgsmItemConvert;
	
	AgpmSkill*			m_pAgpmSkill;
	AgsmSkill*			m_pAgsmSkill;
	AgsmEventSkillMaster*	m_pAgsmEventSkillMaster;

	AgpmFactors*		m_pAgpmFactors;

	AgsmUIStatus*		m_pAgsmUIStatus;

	AgsmGuild* m_pAgsmGuild;

	AgsmAdmin* m_pAgsmAdmin;

private:
	// 패킷 관련 클래스
	AuPacket m_csPacket;
	/*
	AuPacket m_csCharPacket;
	AuPacket m_csItemPacket;
	AuPacket m_csItemConvertHistoryPacket;
	AuPacket m_csSkillPacket;
	*/

	AuPacket m_csCharPacket2;
	AuPacket m_csItemPacket2;
	AuPacket m_csItemConvertHistoryPacket2;
	AuPacket m_csSkillPacket2;
	AuPacket m_csUIStatusPacket2;
	AuPacket m_csGuildMasterPacket2;
	AuPacket m_csGuildMemberPacket2;
	AuPacket m_csAccountWorldPacket;
	AuPacket m_csSearchCharacterPacket;

	// 패킷 초기화 함수들
	void InitPacket();
	void InitCharPacket();
	void InitItemPacket();
	void InitItemConvertHistoryPacket();
	void InitSkillPacket();
	void InitUIStatusPacket();
	void InitGuildMasterPacket();
	void InitGuildMemberPacket();
	void InitAccountWorldPacket();
	void InitSearchCharacterPacket();

	// Send를 위한 make packet
	PVOID MakeRelayPacket(BOOL bPacket, INT16* pnPacketLength, INT8 cOperation, INT8 cStatus, INT8 cDBOperation,
							PVOID pvPacketChar, PVOID pvPacketItem, PVOID pvPacketItemConvertHistory, PVOID pvPacketSkill, PVOID pvPacketUIStatus,
							PVOID pvPacketGuildMaster, PVOID pvPacketGuildMember, PVOID pvPacketAccountWorld,
							PVOID pvPacketSearchCharacter);

public:
	AgsmRelay();
	virtual ~AgsmRelay();

	virtual BOOL OnAddModule();
	virtual BOOL OnDestroy();
	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// Operation
	/*
	BOOL OperationCharacter(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationItem(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationItemConvertHistory(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationSkill(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	*/

	BOOL OperationCharacter2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationItem2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationItemConvertHistory2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationSkill2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationUIStatus2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationGuildMaster2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationGuildMember2(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationGuildMasterSelectResult2(AgsdRelayGuildMaster2* pcsGuildMaster);
	BOOL OperationGuildMemberSelectResult2(AgsdRelayGuildMember2* pcsGuildMember);
	BOOL OperationAccountWorld(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationSearchCharacter(INT32 lDBOperation, PVOID pvPacket, UINT32 ulNID);
	BOOL OperationSearchCharacterSelectResult(AgsdRelaySearchCharacter* pcsSearchCharacter);

	// Relay Server로 보내는 캐릭터 패킷 함수들
	/*
	BOOL SendCharacterInsert(AgpdCharacter* pAgpdCharacter);
	BOOL SendCharacterUpdate(AgpdCharacter* pAgpdCharacter);
	BOOL SendCharacterDelete(AgpdCharacter* pAgpdCharacter);
	*/

	BOOL SendCharacterUpdate2(AgpdCharacter* pAgpdCharacter);

	// Relay Server로 보내는 아이템 패킷 함수들
	/*
	BOOL SendItemInsert(AgpdItem* pAgpdItem, UINT64 lDBID);
	BOOL SendItemUpdate(AgpdItem* pAgpdItem, UINT64 lDBID);
	BOOL SendItemDelete(UINT64 lDBID);
	*/

	BOOL SendItemInsert2(AgpdItem* pAgpdItem, UINT64 lDBID);
	BOOL SendItemUpdate2(AgpdItem* pAgpdItem, UINT64 lDBID);
	BOOL SendItemDelete2(UINT64 lDBID, CHAR *szDeleteReason);

	/*
	BOOL SendItemConvertHistoryInsert(AgpdItem *pAgpdItem, INT32 lIndex);
	BOOL SendItemConvertHistoryDelete(AgpdItem *pAgpdItem, INT32 lIndex);
	*/

	BOOL SendItemConvertHistoryUpdate2(AgpdItem *pAgpdItem);

	// Relay Server로 보내는 스킬 패킷 함수들
	/*
	BOOL SendSkillInsert(ApBase *pcsBase, INT32 lMasteryIndex);
	BOOL SendSkillUpdate(ApBase *pcsBase, INT32 lMasteryIndex);
	BOOL SendSkillDelete(ApBase *pcsBase, INT32 lMasteryIndex);
	*/

//	BOOL SendSkillUpdate2(ApBase *pcsBase, INT32 lMasteryIndex);
	BOOL SendSkillUpdate2(ApBase *pcsBase);

	BOOL SendUIStatusUpdate2(AgpdCharacter *pcsCharacter);

	// Guild 관련
	BOOL SendGuildMasterInsert2(AgpdGuild* pcsGuild);
	BOOL SendGuildMasterUpdate2(AgpdGuild* pcsGuild);
	BOOL SendGuildMasterDelete2(CHAR* szGuildID);
	BOOL SendGuildMasterSelect2();
	BOOL SendGuildMasterSelectResult2(UINT32 lNID, CHAR* szGuildID, INT32 lGuildTID, CHAR* szMasterID, INT32 lMaxMemberCount,
															INT32 lCreationDate, INT32 lUnionID, INT32 lGuildRank, CHAR* szPassword);
	BOOL SendGuildMasterSelectResult2(UINT32 lNID, AgsdRelayGuildMaster2* pcsGuildMaster);

	BOOL SendGuildMemberInsert2(CHAR* szGuildID, AgpdGuildMember* pcsMember);
	BOOL SendGuildMemberUpdate2(CHAR* szGuildID, AgpdGuildMember* pcsMember);
	BOOL SendGuildMemberDelete2(CHAR* szGuildID, CHAR* szCharID);
	BOOL SendGuildMemberSelect2();
	BOOL SendGuildMemberSelectResult2(UINT32 lNID, CHAR* szMemberID, CHAR* szGuildID, INT32 lRank, INT32 lJoinDate, INT32 lTID, INT32 lLevel);
	BOOL SendGuildMemberSelectResult2(UINT32 lNID, AgsdRelayGuildMember2* pcsGuildMember);

	BOOL SendAccountWorldUpdate(CHAR *szAccountID, INT64 llBankMoney);

	BOOL SendSearchCharacterSelect(CHAR* szAdminName, CHAR* szAccName, CHAR* szCharName);
	BOOL SendSearchCharacterSelectResult(UINT32 ulNID, AgsdRelaySearchCharacter* pcsSearchCharacter);

	// Select 결과를 Send 하기위한 함수들
	/*
	BOOL SendResultCharacter(AgsdRelayCharacter *pAgsdRelayCharacter);
	BOOL SendResultItem(AgsdRelayItem *pAgsdRelayItem);
	BOOL SendResultSkill(AgsdRelaySkill *pAgsdRelaySkill);
	*/
	
	// 콜백 함수 등록
	/*
	BOOL SetCallbackCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackItemConvertHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	*/

	BOOL SetCallbackCharacter2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackItem2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackItemConvertHistory2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSkill2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackUIStatus2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildMaster2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGuildMember2(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAccountWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackSearchCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// AgsmCharacter에 등록될 Callback Functions
	static BOOL CBCharacterInsert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCharacterUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCharacterDelete(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBAccountWorldUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// AgsmItem에 등록될 Callback Functions
	static BOOL CBItemInsert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItemUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItemDelete(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBItemConvertHistoryInsert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBItemConvertHistoryDelete(PVOID pData, PVOID pClass, PVOID pCustData);

	// AgsmSkill에 등록될 Callback Functions
	static BOOL CBSkillInsert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSkillUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBSkillDelete(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBUIStatusUpdate(PVOID pData, PVOID pClass, PVOID pCustData);

	// Guild 관련
	static BOOL CBGuildMasterInsert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMasterUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMasterDelete(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMasterSelect(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBGuildMemberInsert(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMemberUpdate(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMemberDelete(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBGuildMemberSelect(PVOID pData, PVOID pClass, PVOID pCustData);

	// Admin 관련
	static BOOL CBSearchCharacterByAccountSelect(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif // !defined(AFX_AGSMRELAY_H__DC507B62_BA16_45CE_968E_B75256F10C63__INCLUDED_)
