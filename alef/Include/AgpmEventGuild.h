// AgpmEventGuild.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 13.

#ifndef _AGPMEVENTGUILD_H_
#define _AGPMEVENTGUILD_H_

#include "AgpmCharacter.h"

#include "ApmEventManager.h"

#include "AuPacket.h"

//@{ Jaewon 20041118
#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgpmEventGuildD.lib")
#else
	#pragma comment(lib, "AgpmEventGuild.lib")
#endif
#endif
//@} Jaewon


#define AGPMEVENT_GUILD_MAX_USE_RANGE				800

typedef enum _eAgpmEventGuildPacketType
{
	AGPMEVENT_GUILD_PACKET_REQUEST = 0,
	AGPMEVENT_GUILD_PACKET_GRANT,
	AGPMEVENT_GUILD_PACKET_REQUEST_WAREHOUSE,
	AGPMEVENT_GUILD_PACKET_GRANT_WAREHOUSE,
	AGPMEVENT_GUILD_PACKET_REQUEST_WORLD_CHAMPIONSHIP,
	AGPMEVENT_GUILD_PACKET_GRANT_WORLD_CHAMPIONSHIP,
	AGPMEVENT_GUILD_PACKET_MAX,
} eAgpmEventGuildPacketType;

typedef enum _eAgpmEventGuildCallback
{
	AGPMEVENT_GUILD_CB_REQUEST = 0,
	AGPMEVENT_GUILD_CB_GRANT,
	AGPMEVENT_GUILD_CB_REQUEST_WAREHOUSE,
	AGPMEVENT_GUILD_CB_GRANT_WAREHOUSE,	
	AGPMEVENT_GUILD_CB_REQUEST_WORLD_CHAMPIONSHIP,
	AGPMEVENT_GUILD_CB_GRANT_WORLD_CHAMPIONSHIP,
	AGPMEVENT_GUILD_CB_MAX,
} eAgpmEventGuildCallback;

class AgpmEventGuild : public ApModule
{
private:
	AgpmCharacter* m_pcsAgpmCharacter;
	
	ApmEventManager* m_pcsApmEventManager;
	
public:
	AuPacket m_csPacket;
	
public:
	AgpmEventGuild();
	virtual ~AgpmEventGuild();

	BOOL OnAddModule();

	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	BOOL OnOperationRequest(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);
	BOOL OnOperationGrant(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);
	BOOL OnOperationRequestWarehouse(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);
	BOOL OnOperationGrantWarehouse(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);
	BOOL OnOperationRequestWorldChampionship(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);
	BOOL OnOperationGrantWorldChampionship(ApdEvent* pcsEvent, AgpdCharacter* pcsCharacter);

	PVOID MakePacketEventRequest(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID MakePacketEventGrant(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID MakePacketEventRequestWarehouse(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID MakePacketEventGrantWarehouse(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID MakePacketEventRequestWorldChampionship(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);
	PVOID MakePacketEventGrantWorldChampionship(ApdEvent *pcsEvent, INT32 lCID, INT16 *pnPacketLength);

	BOOL SetCallbackRequest(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRequestWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGrantWarehouse(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRequestWorldChampionship(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackGrantWorldChampionship(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		
	static BOOL CBActionGuild(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBActionGuildWarehouse(PVOID pData, PVOID pClass, PVOID pCustData);	
};

#endif //_AGPMEVENTGUILD_H_