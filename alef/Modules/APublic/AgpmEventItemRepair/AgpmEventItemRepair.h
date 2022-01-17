//	AgpmEventItemRepair module header
//		- item 수리(내구도 수리)를 담당한다.
/////////////////////////////////////////////////////////////////////////

#ifndef	__AGPMEVENTITEMREPAIR_H__
#define	__AGPMEVENTITEMREPAIR_H__

#include "AgpmFactors.h"
#include "AgpmCharacter.h"
#include "AgpmItem.h"
#include "ApmEventManager.h"
#include "AuPacket.h"

// define packet operation
enum EnumAgpmEventItemRepairOperation 
{
	AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_REQ					= 0,	// client에서 server로 아템 수리 요청한다.
	AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_ACK,
	AGPMEVENT_ITEMREPAIR_OPERATION_REPAIR_RESULT,
	AGPMEVENT_ITEMREPAIR_OPERATION_EVENT_REQ,
	AGPMEVENT_ITEMREPAIR_OPERATION_GRANT,
};

enum EnumAgpmEventItemRepairResultCode
{
	// request에 대한 결과
	AGPMEVENT_ITEMREPAIR_RESULTCODE_NONE				= 0,
	AGPMEVENT_ITEMREPAIR_RESULTCODE_SUCCESS,						// 성공
	AGPMEVENT_ITEMREPAIR_RESULTCODE_NOT_ENOUGH_MONEY,				// 돈이 충분치 않다.
	AGPMEVENT_ITEMREPAIR_RESULTCODE_ALREADY_FULL,					// 이미 내구력이 풀이다.
	AGPMEVENT_ITEMREPAIR_RESULTCODE_REPAIR_FAIL,					// 고치기 실패했다.
	AGPMEVENT_ITEMREPAIR_RESLUTCODE_CANNOT_REPAIR_ITEM,				// 고칠 수 없는 아템이다.
};

enum EnumAgpmEventItemRepairCB
{
	AGPMEVENT_ITEMREPAIR_CB_REPAIR_REQ					= 0,
	AGPMEVENT_ITEMREPAIR_CB_REPAIR_ACK,
	AGPMEVENT_ITEMREPAIR_CB_REPAIR_RESULT,
	AGPMEVENT_ITEMREPAIR_CB_EVENT_REQ,
	AGPMEVENT_ITEMREPAIR_CB_GRANT
};

enum EnumGridCount
{
	ITEM_REPAIR_GRID_LAYER	= 1,
	ITEM_REPAIR_GRID_ROW	= 5,
	ITEM_REPAIR_GRID_COLUMN	= 4,
	ITEM_REPAIR_GRID_MAX	= (ITEM_REPAIR_GRID_LAYER * ITEM_REPAIR_GRID_ROW * ITEM_REPAIR_GRID_COLUMN),
};

const int	AGPMEVENT_ITEMREPAIR_DURABILITY_FOR_NEED_REPAIR	= 20;
const int	AGPMEVENT_ITEMREPAIR_MAX_USE_RANGE = 1600;

struct AgpdItemRepair
{
	INT8	cOperation;
	PVOID	pvPacketEventBase;
	INT32	lCID;
	INT64	llRepairCost;
	ApSafeArray<INT32, ITEM_REPAIR_GRID_MAX>	ItemIDs;
	EnumAgpmEventItemRepairResultCode eResultCode;

	AgpdItemRepair()
	{
		cOperation			= -1;
		pvPacketEventBase	= NULL;
		lCID				= -1;
		llRepairCost		= -1;
		eResultCode			= AGPMEVENT_ITEMREPAIR_RESULTCODE_NONE;
		ItemIDs.MemSetAll();
	}
};

class	AgpmEventItemRepair : public ApModule {
private:
	AgpmFactors*			m_pcsAgpmFactors;
	AgpmCharacter*			m_pcsAgpmCharacter;
	AgpmItem*				m_pcsAgpmItem;

	ApmEventManager*		m_pcsApmEventManager;

public:
	AuPacket				m_csPacket;
	AuPacket				m_csPacketEventData;

public:
	AgpmEventItemRepair();
	~AgpmEventItemRepair();

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnDestroy();
	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	// make packet functions
	/////////////////////////////////////////////////////////
	PVOID MakePacketItemRepair(ApdEvent *pcsEvent, EnumAgpmEventItemRepairOperation eOperation, INT32 lCID, 
								INT32* ItemIDs, INT64 lRepairCost, EnumAgpmEventItemRepairResultCode eResultCode, 
								INT16 *pnPacketLength);

	BOOL SetCallbackRepairReq(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRepairAck(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRepairResult(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackEventReq(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	BOOL OnOperationEventReq(ApdEvent *pApdEvent, AgpdCharacter *pAgpdCharacter);

	INT64 GetItemRepairPrice(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter, INT64 *plTax = NULL);

	static BOOL CBActionItemRepair(PVOID pData, PVOID pClass, PVOID pCustData);

	// ApmEventManager Embedded Packet
	static BOOL CBEmbeddedMakeEventPacket(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEmbeddedReceiveEventPacket(PVOID pData, PVOID pClass, PVOID pCustData);

	PVOID MakePacketEventData(ApdEvent *pcsEvent);
	BOOL ParseEventDataPacket(ApdEvent *pcsEvent, PVOID pvPacketCustomData);

	PVOID MakePacketEventRequest(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID MakePacketEventGrant(ApdEvent *pApdEvent, INT32 lCID, INT16 *pnPacketLength);
};

#endif	//__AGPMEVENTITEMREPAIR_H__