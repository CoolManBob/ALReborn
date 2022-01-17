/******************************************************************************
Module:  AgpmParty.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 07
******************************************************************************/

#if !defined(__AGPMPARTY_H__)
#define __AGPMPARTY_H__

#include "ApBase.h"
#include "ApModule.h"
#include "AgpdParty.h"
#include "AgpaParty.h"
#include "AgpmCharacter.h"
#include "ApAdmin.h"
#include "AgpmFactors.h"
#include "AgpmUnion.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgpmPartyD" )
#else
#pragma comment ( lib , "AgpmParty" )
#endif
#endif


const int	AGPMPARTY_MIN_PARTY_MEMBER			= 4;
const int	AGPMPARTY_PROCESS_REMOVE_INTERVAL	= 3000;
const int	AGPMPARTY_PRESERVE_PARTY_DATA		= 5000;

const int	AGPMPARTY_MAX_EFFECT_AREA			= 5000;		// 반경 50m
const int	AGPMPARTY_AVAILABLE_LEVEL_DIFF		= 12;		// 반경 50m

typedef enum _eAgpmPartyPacketOperation {
	AGPMPARTY_PACKET_OPERATION_ADD			= 0,
	AGPMPARTY_PACKET_OPERATION_REMOVE,
	AGPMPARTY_PACKET_OPERATION_UPDATE,
	AGPMPARTY_PACKET_OPERATION_ADD_MEMBER,
	AGPMPARTY_PACKET_OPERATOIN_REMOVE_MEMBER,
	AGPMPARTY_PACKET_OPERATION_UPDATE_EXP_TYPE,
	AGPMPARTY_PACKET_OPERATION_UPDATE_ITEM_DIVISION,	//	Item 분배방식 변경 ID 2005.04.19 By SungHoon
} eAgpmPartyPacketOperation;


typedef enum _eAgpmPartyCBID {
	AGPMPARTY_CB_CREATE						= 0,
	AGPMPARTY_CB_REMOVE,
	AGPMPARTY_CB_REMOVE_ID,
	AGPMPARTY_CB_ADD_MEMBER,
	AGPMPARTY_CB_REMOVE_MEMBER,
	AGPMPARTY_CB_CHANGE_LEADER,
	AGPMPARTY_CB_ADD_PARTY,
	AGPMPARTY_CB_UPDATE_FACTOR,
	AGPMPARTY_CB_SYNC_REMOVE_CHARACTER,
	AGPMPARTY_CB_CHECK_DESTROY_PARTY,
	AGPMPARTY_CB_CHAR_LEVEL_UP,
	AGPMPARTY_CB_RESET_EFFECT_AREA,
	AGPMPARTY_CB_UPDATE_EXP_TYPE,	
	AGPMPARTY_CB_UPDATE_BONUS_STATS,
	AGPMPARTY_CB_UPDATE_PARTY_FACTOR,
	AGPMPARTY_CB_UPDATE_ITEM_DIVISION,		//	Item 분배방식 변경 CallBack ID 2005.04.19 By SungHoon
} eAgpmPartyCBID;



//	AgpmParty class
///////////////////////////////////////////////////////////////////////////////
class AgpmParty : public ApModule {
private:
	AgpmFactors			*m_pagpmFactors;
	AgpmCharacter		*m_pagpmCharacter;
	AgpmUnion			*m_pagpmUnion;

	AgpaParty			m_csPartyAdmin;

	INT16				m_nIndexADCharacter;

	BOOL				ChangeLeader(AgpdParty *pParty, BOOL bIsCalcEffectArea = TRUE);

	AgpdParty*			CreatePartyData();
	BOOL				DestroyPartyData(AgpdParty *pcsParty);

	ApAdmin				m_csAdminPartyRemove;
	UINT32				m_ulPrevRemoveClockCount;

public:
	AuPacket			m_csPacket;
	AuPacket			m_csPacketEffectArea;
	AuPacket			m_csPacketBonusStats;

public:
	AgpmParty();
	~AgpmParty();

	static BOOL			ConAgpdPartyADChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			DesAgpdPartyADChar(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdParty*			AddParty(INT32 lPID);
	BOOL				RemoveParty(INT32 lPID, BOOL bIsLocked = FALSE);
	AgpdParty*			GetParty(INT32 lPID);
	AgpdParty*			GetPartyLock(INT32 lPID);

	AgpdParty*			GetParty(AgpdCharacter *pcsCharacter);
	AgpdParty*			GetPartyLock(AgpdCharacter *pcsCharacter);

	INT32				GetNumPartyMember(INT32 lPID);
	INT32				GetNumPartyMember(AgpdParty *pcsParty);

	INT32				GetPartyHighestLevel(INT32 lPID);
	INT32				GetPartyHighestLevel(AgpdParty *pcsParty);

	INT32				GetPartyLowerLevel(INT32 lPID);				//	2005.05.27 By SungHoon
	INT32				GetPartyLowerLevel(AgpdParty *pcsParty);	//	2005.05.27 By SungHoon

	INT32				GetPartyHighestUnionRank(INT32 lPID);
	INT32				GetPartyHighestUnionRank(AgpdParty *pcsParty);

	INT32				GetPartyAvrMemberLevel(AgpdParty *pcsParty);

	INT32				GetPartyTotalMemberLevel(INT32 lPID);
	INT32				GetPartyTotalMemberLevel(AgpdParty *pcsParty);

	BOOL				ArrangeHighestLevel(AgpdParty *pcsParty);
	BOOL				ArrangeLowerLevel(AgpdParty *pcsParty);
	BOOL				ArrangeLevel(AgpdParty* pcsParty);

	BOOL				ArrangeHighestUnionRank(AgpdParty *pcsParty);

	INT32				GetLeaderCID( AgpdCharacter	*pcsAgpdCharacter );
	INT32				GetLeaderCID( AgpdParty *pcsAgpdParty );		// 2005.05.02 By SungHoon

	BOOL				AddMember(INT32 lPID, INT32 lCID);
	BOOL				AddMember(AgpdParty* pParty, INT32 lCID);
	BOOL				AddMember(AgpdParty* pParty, AgpdCharacter* pcsCharacter, BOOL bIsCalcEffectArea = TRUE);

	BOOL				RemoveMember(INT32 lPID, INT32 lCID, BOOL *pbDestroyParty);
	BOOL				RemoveMember(AgpdParty* pParty, INT32 lCID, BOOL *pbDestroyParty, BOOL bIsCalcEffectArea = TRUE);

//	2005.05.02	Start By SungHoon
	BOOL				DelegationLeader(AgpdParty *pcsParty, INT32 lOperatorID, INT32 lTargetID);
//	2005.05.02	Finish By SungHoon

	AgpdParty*			CreateParty(INT32 lPID, INT32 lPartyLeaderID, INT32 lMemberID);
	AgpdParty*			CreateParty(INT32 lPID, AgpdCharacter *pcsPartyLeader, AgpdCharacter *pcsMember);
	AgpdParty*			CreateParty(INT32 lPID, INT32 lLeaderID, INT32 lMemberID, INT16 nMaxMember);
	AgpdParty*			CreateParty(INT32 lPID, AgpdCharacter *pcsPartyLeader, AgpdCharacter *pcsMember, INT16 nMaxMember);

	BOOL				DestroyParty(INT32 lPID);
	BOOL				DestroyParty(AgpdParty *pcsParty);

//	2005.04.15	Start By SungHoon
//	아이템 순차적 획득에 필요한 변수
	INT32				GetNextGetItemMember(INT32 lPID, BOOL bTemp = FALSE);
	INT32				GetNextGetItemMember(AgpdParty *pcsParty, BOOL bTemp = FALSE);
//	2005.04.15	Finish By SungHoon
	VOID				ReSetCurrentGetItemMember(AgpdParty *pcsParty, INT32 lCID);
	VOID				ReSetFinishGetItemMember(AgpdParty *pcsParty, INT32 lCID);


	BOOL				SetMaxParty(INT32 nCount);
	BOOL				SetMaxPartyRemove(INT32 lCount);

	BOOL				IsMember(AgpdParty* pParty, INT32 lCID);

	BOOL				IsFull(AgpdParty *pcsParty);
	BOOL				IsNeedDestroy(AgpdParty *pcsParty);

	// GetAttachedModuleData() Wrap Functions
	AgpdPartyADChar*	GetADCharacter(PVOID pData);

	// module register
	BOOL				OnAddModule();

	BOOL				OnInit();
	BOOL				OnDestroy();

	BOOL				OnIdle2(UINT32 ulClockCount);

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL				OnOperationUpdateEffectArea(AgpdParty *pcsParty, PVOID pvPacketEffectArea);
	BOOL				OnOperationUpdateBonusStats(AgpdParty *pcsParty, PVOID pvPacketBonusStats);

	// register callback
	BOOL				SetCBRemoveParty(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBRemoveID(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBChangeLeader(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBAddMember(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBRemoveMember(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBSyncRemoveCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBCreateParty(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBResetEffectArea(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCBAddParty(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBUpdateFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCBCheckDestroyParty(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCBCharLevelUp(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCBUpdateExpType(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBUpdateBonusStats(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCBUpdatePartyFactor(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL				SetCBUpdateItemDivision(ApModuleDefaultCallBack pfCallback, PVOID pClass);	//	2005.04.19	By SungHoon

	INT16				AttachPartyData(PVOID pClass, INT32 nDataSize, ApModuleDefaultCallBack pfConstructor, ApModuleDefaultCallBack pfDestructor);

	PVOID				MakePacketParty(AgpdParty *pcsParty, INT16 *pnPacketLength);

	PVOID				MakePacketAddMember(AgpdParty *pcsParty, INT32 lMemberID, INT16 *pnPacketLength);
	PVOID				MakePacketRemoveMember(INT32 lPartyID, INT32 lMemberID, INT16 *pnPacketLength);
	PVOID				MakePacketRemoveParty(INT32 lPartyID, INT16 *pnPacketLength);

	PVOID				MakePacketSyncMemberHP(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);
	PVOID				MakePacketSyncMemberHPMax(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	PVOID				MakePacketEffectArea(AgpdParty *pcsParty, BOOL *pbOldList, INT16 *pnPacketLength);
//	PVOID				MakePacketCalcExpType(AgpdParty *pcsParty, INT16 *pnPacketLength);		//	2005.05.17 By SungHoon
//	2005.04.19	Start By SungHoon
	PVOID				MakePacketItemDivision(AgpdParty *pcsParty, INT32 lMemberCID, INT16 *pnPacketLength);
	PVOID				MakePacketItemDivision(INT32 lPartyID, INT32 lMemberCID, AgpmPartyOptionDivisionItem eDivisionItem, INT16 *pnPacketLength);
//	2005.04.19	Finish By SungHoon

//	2005.05.17	Start By SungHoon
	PVOID				MakePacketExpDivision(AgpdParty *pcsParty, INT32 lMemberCID, INT16 *pnPacketLength);
	PVOID				MakePacketExpDivision(INT32 lPartyID, INT32 lMemberCID, AgpmPartyCalcExpType eCalExpType, INT16 *pnPacketLength);
//	2005.05.17	Finish By SungHoon

	PVOID				MakePacketBonusStatus(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	static BOOL			CBRemoveChar(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBCheckValidNormalAttack(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL			CBCharLevelUp(PVOID pData, PVOID pClass, PVOID pCustData);

	//BOOL				CalcPartyFactor(AgpdCharacter *pcsLeader, AgpdCharacter *pcsMember, AgpdFactor *pcsFactorPoint, AgpdFactor *pcsFactorPercent);
	BOOL				CalcPartyFactor(AgpdCharacter *pcsLeader, AgpdCharacter *pcsMember, AgpdFactor *pcsFactorPoint);

	// party remove pool 관련 함수들
	BOOL				ProcessRemove(UINT32 ulClockCount);
	BOOL				AddRemoveParty(AgpdParty *pcsParty);

	BOOL				SetEffectArea(AgpdParty *pcsParty);

	AgpdParty*			GetPartySequence(INT32 *pnIndex);
};

#endif //__AGPMPARTY_H__
