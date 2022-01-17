#pragma once

#include "ApModule.h"
#include "AuPacket.h"
#include "AgpdCharacter.h"
#include "ApmEventManager.h"

enum AgpmArchlordPacketOperation
{
	AGPMARCHLORD_OPERATION_SET_ARCHLORD = 0,
	AGPMARCHLORD_OPERATION_CANCEL_ARCHLORD,
	AGPMARCHLORD_OPERATION_SET_GUARD,
	AGPMARCHLORD_OPERATION_CANCEL_GUARD,
	AGPMARCHLORD_OPERATION_GUARD_INFO,
	AGPMARCHLORD_OPERATION_STEP,
	AGPMARCHLORD_OPERATION_MESSAGE_ID,
	AGPMARCHLORD_OPERATION_EVENT_REQUEST,
	AGPMARCHLORD_OPERATION_EVENT_GRANT,
};

enum AgpmArchlordCBID
{
	AGPMARCHLORD_CB_OPERATION_SET_ARCHLORD = 0,	// Packet operation용 callback
	AGPMARCHLORD_CB_OPERATION_CANCEL_ARCHLORD,	// Packet operation용 callback
	AGPMARCHLORD_CB_OPERATION_SET_GUARD,
	AGPMARCHLORD_CB_OPERATION_CANCEL_GUARD,
	AGPMARCHLORD_CB_OPERATION_GUARD_INFO,
	AGPMARCHLORD_CB_OPERATION_CURRENT_STEP,
	AGPMARCHLORD_CB_OPERATION_MESSAGE_ID,
	AGPMARCHLORD_CB_SET_ARCHLORD,
	AGPMARCHLORD_CB_CANCEL_ARCHLORD,
	AGPMARCHLORD_CB_EVENT_REQUEST,
	AGPMARCHLORD_CB_EVENT_GRANT,
};

enum AgpmArchlordMessageId
{
	AGPMARCHLORD_MESSAGE_NOT_ENOUGH_INVENTORY,		// 인벤토리 부족
	AGPMARCHLORD_MESSAGE_DONT_SET_GUARD,			// 호위대로 설정되지 못함
	AGPMARCHLORD_MESSAGE_DONT_CANCEL_GUARD,			// 호위대에서 해제되지 못함
	AGPMARCHLORD_MESSAGE_NEED_ARCHON_ITEM,			// 아콘 아이템이 필요함
	AGPMARCHLORD_MESSAGE_GUILD_MASTER_ITEM,			// 길드 마스터만이 소유 할 수 있는 아이템
};

enum AgpmArchlordStep
{
	AGPMARCHLORD_STEP_NONE,		// 
	AGPMARCHLORD_STEP_DUNGEON,	// 비밀의 던젼 시작 --> 렌스피어로 들어갈 대기 시간으로 변경. 2009. 09. 29 박경도
	AGPMARCHLORD_STEP_SIEGEWAR,	// 아크로드 공성전 시작
	AGPMARCHLORD_STEP_ARCHLORD,	// 아크로드 혹은 디카인과 대전
	AGPMARCHLORD_STEP_END		// 아크로드 시스템 종료
};

const int AGPMARCHLORD_DUNGEON_NAME = 40;
const int AGPMARCHLORD_MAX_USE_RANGE = 1600;

struct AgpdArchlord
{
	ApString<AGPMARCHLORD_DUNGEON_NAME> m_szName;
//	UINT64
};

//class AgpmSiegeWar;
class AgpmCharacter;

class AgpmArchlord : public ApModule 
{
public:
//	AgpmSiegeWar	*m_pcsAgpmSiegeWar;
	AgpmCharacter	*m_pcsAgpmCharacter;
	ApmEventManager	*m_pcsApmEventManager;
	AgpmArchlordStep m_eCurrentStep;
public:
	ApString<AGPDCHARACTER_MAX_ID_LENGTH> m_szArchlord;

	AuPacket		m_csPacket;

public:
	AgpmArchlord();
	virtual ~AgpmArchlord();

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle();
	virtual BOOL OnDestroy();

	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL OnOperationEventRequest(ApdEvent *pApdEvent, AgpdCharacter *pcsCharacter);
	BOOL OnOperationEventGrant(ApdEvent *pApdEvent, AgpdCharacter *pcsCharacter);

	BOOL SetCallbackOperationSetArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOperationCancelArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOperationSetGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOperationCancelGuard(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOperationGuardInfo(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOperationCurrentStep(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackOperationMessageId(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetCallbackSetArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackCancelArchlord(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	
	BOOL SetCallbackEventRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL SetArchlord(CHAR *szID);
	BOOL IsArchlord(CHAR *szID);
	BOOL CancelArchlord(CHAR *szID);

	AgpmArchlordStep GetCurrentStep();
	void SetCurrentStep(AgpmArchlordStep eStep);

	PVOID MakePacketSetArchlord(INT16 *pnPacketLength, char* szArchlordID);
	PVOID MakePacketCancelArchlord(INT16 *pnPacketLength, char* szArchlordID);
	PVOID MakePacketGuardInfo(INT16 *pnPacketLength, INT32 lCurrentGuard, INT32 lMaxGuard);
	PVOID MakePacketSetGuard(INT16 *pnPacketLength, char* szGuardID, CHAR cResult);
	PVOID MakePacketCancelGuard(INT16 *pnPacketLength, char* szGuardID, CHAR cResult);
	PVOID MakePacketCurrentStep(INT16 *pnPacketLength, INT32 lStep);
	PVOID MakepacketMessageID(INT16 *pnPacketLength, AgpmArchlordMessageId eMessageID);
	PVOID MakePacketEventRequest(INT16 *pnPacketLength, ApdEvent *pApdEvent, INT32 lCID);
	PVOID MakePacketEventGrant(INT16 *pnPacketLength, ApdEvent *pApdEvent, INT32 lCID);

	static BOOL CBIsArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventAction(PVOID pData, PVOID pClass, PVOID pCustData);
};