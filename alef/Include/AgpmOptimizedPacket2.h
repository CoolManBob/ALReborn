#ifndef __AGPMOPTIMIZIEDPACKET2_H__
#define __AGPMOPTIMIZIEDPACKET2_H__

#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "AgpmItemConvert.h"
#include "AgpmSkill.h"
#include "ApmEventManager.h"
#include "AgpmGuild.h"
#include "AgpmUIStatus.h"
#include "AgpmBillInfo.h"
#include "AgpmTitle.h"

typedef enum	AgpmOptimizedPacket2Operation {
	AGPM_OPTIMIZEDPACKET2_NONE							= 0,
	AGPM_OPTIMIZEDPACKET2_ADD_CHARACTER_VIEW,
	AGPM_OPTIMIZEDPACKET2_ADD_CHARACTER,
	AGPM_OPTIMIZEDPACKET2_RELEASE_MOVE_ACTION,
};

typedef enum	_AgpmOptimizedPacket2NextMoveActionType {
	AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_NONE				= 0,
	AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_ATTACK,
	AGPM_OPTIMIZEDPACKET2_MOVE_TYPE_SKILL,
} AgpmOptimizedPacket2NextMoveActionType;


typedef enum	_AgpmOptimizedPacket2CBID {
	AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_ATTACK			= 0,
	AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_SKILL,
	AGPM_OPTIMIZEDPACKET2_CB_MOVE_ACTION_RELEASE,
} AgpmOptimizedPacket2CBID;


typedef struct	_AgpmOptimizedPacket2BuffedList {
	INT32	lSkillTID;
	INT32	lCasterTID;
	INT8	cChargeLevel;
	UINT32	ulExpiredTime;
} AgpmOptimizedPacket2BuffedList;

const INT32 CashItemPacketIsOk = 99999;		// cash item 소유 개수가 현재 200개(바뀔 가능성 있음)니 99999근처에도 안가겠지.

class AgpmOptimizedPacket2 : public ApModule {
private:
	AgpmFactors			*m_pcsAgpmFactors;
	AgpmCharacter		*m_pcsAgpmCharacter;
	AgpmGrid			*m_pcsAgpmGrid;
	AgpmItem			*m_pcsAgpmItem;
	AgpmItemConvert		*m_pcsAgpmItemConvert;
	AgpmSkill			*m_pcsAgpmSkill;
	ApmEventManager		*m_pcsApmEventManager;
	AgpmGuild			*m_pcsAgpmGuild;
	AgpmUIStatus		*m_pcsAgpmUIStatus;
	AgpmBillInfo		*m_pcsAgpmBillInfo;
	AgpmTitle			*m_pcsAgpmTitle;

	AuPacket			m_csPacket;
	AuPacket			m_csPacketCharView;
	AuPacket			m_csPacketItemView;

	AuPacket			m_csPacketCharMove;
	AuPacket			m_csPacketCharAction;

	ApMemoryPool		m_csOptimizedPacketMemoryPool;

private:
	AgpdCharacter*		ProcessCharacterPacket(PVOID pvPacketChar, DispatchArg *pstCheckArg);
	BOOL				ProcessItemPacket(AgpdCharacter *pcsCharacter, PVOID *ppvPacketItem, DispatchArg *pstCheckArg);
	BOOL				ProcessBuffedSkillPacket(AgpdCharacter *pcsCharacter, CHAR *pszBuffedSkill, UINT16 unDataSize, DispatchArg *pstCheckArg);
	BOOL				ProcessEventPacket(AgpdCharacter *pcsCharacter, CHAR *szEventPacket, UINT16 unEventPacketSize, DispatchArg *pstCheckArg, UINT32 ulNID);

	BOOL				ProcessCharAllItemPacket(CHAR *szCharAllItem, UINT16 unCharAllItemLength, DispatchArg *pstCheckArg, UINT32 ulNID);
	BOOL				ProcessCharAllItemConvertPacket(CHAR *szCharAllItemConvert, UINT16 unCharAllItemConvertLength, DispatchArg *pstCheckArg, UINT32 ulNID);
	BOOL				ProcessCharAllSkillPacket(CHAR *szCharAllSkill, UINT16 unCharAllSkillLength, DispatchArg *pstCheckArg, UINT32 ulNID);

	BOOL				OnReceiveCharView(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL				OnReceiveCharMove(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);
	BOOL				OnReceiveCharAction(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL				AddItemPacketToBuffer(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize, INT32 *plAddBufferSize);
	BOOL				AddItemConvertPacketToBuffer(AgpdItem *pcsItem, CHAR *szBuffer, INT32 lBufferSize, INT32 *plAddBufferSize);

	INT32				MakeBufferCharAllItem(AgpdCharacter *pcsCharacter, CHAR *szItemBuffer, INT32 *plItemBufferSize, CHAR *szItemConvertBuffer, INT32 *plItemConvertBufferSize);
	INT32				MakeBufferCharAllBankItem(AgpdCharacter *pcsCharacter, CHAR *szItemBuffer, INT32 *plItemBufferSize, CHAR *szItemConvertBuffer, INT32 *plItemConvertBufferSize);
	INT32				MakeBufferCharAllCashItem(AgpdCharacter *pcsCharacter, CHAR *szItemBuffer, INT32 *plItemBufferSize, CHAR *szItemConvertBuffer, INT32 *plItemConvertBufferSize, INT32* pIndex);
	INT32				MakeBufferCharAllSkill(AgpdCharacter *pcsCharacter, CHAR *szBuffer);

	UINT16				GetCharBitFlag(AgpdCharacter *pcsCharacter);
	BOOL				SetCharBitFlag(AgpdCharacter *pcsCharacter, UINT16 unBitFlag);

public:
	AgpmOptimizedPacket2();
	virtual ~AgpmOptimizedPacket2();

	BOOL				OnInit();
	BOOL				OnAddModule();
	BOOL				OnDestroy();

	BOOL				SetMaxMemoryPoolCount(INT32 lMaxCount);

	BOOL				OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	PVOID				MakePacketCharView(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bIsNewChar = FALSE);

	PVOID				MakePacketCharMove(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);
	PVOID				MakePacketCharMoveContainAction(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);
	PVOID				MakePacketCharMove(INT32 lCID, INT8 cMoveFlag, INT8 cMoveDirection, AuPOS *pcsDestPos, INT16 *pnPacketLength);

	PVOID				MakePacketReleaseMoveAction(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);

	PVOID				MakePacketCharAction(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsTargetCharacter,
											 INT8 cAttackResult, PVOID pvPacketFactor, INT32 lHP,
											 INT8 cComboInfo, BOOL bForceAttack, UINT32 ulAdditionalEffect, 
											 UINT8 cHitIndex, INT16 *pnPacketLength);
	PVOID				MakePacketCharAction(INT32 lCID, INT32 lTargetID, INT8 cComboInfo, BOOL bForceAttack,
											 UINT32 ulAdditionalEffect, UINT8 cHitIndex, INT16 *pnPacketLength);

	PVOID				MakePacketCharAllItemSkillExceptBankCash(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);
	PVOID				MakePacketCharAllBankItem(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength);
	PVOID				MakePacketCharAllCashItem(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, INT32* plIndexIndex);

	BOOL				SetCallbackMoveActionAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackMoveActionSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL				SetCallbackMoveActionRelease(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGPMOPTIMIZIEDPACKET2_H__