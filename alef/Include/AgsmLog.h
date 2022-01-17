// AgsmLog.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 04. 26.

//
// 2005.03.15. steeple
// 리뉴얼 단행!!! 속도 향상과, 확장성을 고려하여 작업
//
//

#ifndef _AGSMLOG_H_
#define _AGSMLOG_H_

#include "AgpmLog.h"
#include "AgsEngine.h"
//#include "AgsmServerManager.h"
#include "AgsmServerManager2.h"

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgsmLogD.lib")
#else
	#pragma comment(lib, "AgsmLog.lib")
#endif
#endif

typedef enum _eAgsmLogCB
{
	AGSMLOG_CB_WRITE_LOG = 0,
} eAgsmLogCB;

class AgsmLog : public AgsModule
{
private:
	AgpmLog* m_pagpmLog;
	AgsmServerManager* m_pAgsmServerManager;

	AuPacket m_csPacket;
	//AuPacket m_csLoginPacket;
	//AuPacket m_csCombatPacket;
	//AuPacket m_csSkillPacket;
	//AuPacket m_csItemPacket;
	//AuPacket m_csLevelUpPacket;

protected:
	BOOL IsLoginServer();
	BOOL IsGameServer();
	BOOL IsRelayServer();
	UINT32 GetRelayServerNID();

public:
	AgsmLog();
	virtual ~AgsmLog();

	BOOL OnAddModule();

	//////////////////////////////////////////////////////////////////////////
	// Packet Receive
	BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	//////////////////////////////////////////////////////////////////////////
	// Process
	BOOL WriteLog(PVOID pvLog, INT16 nSize);

	//////////////////////////////////////////////////////////////////////////
	// Packet Send
	BOOL SendLogPacket(INT8 cOperation, PVOID pvLog, INT16 nSize, UINT32 ulNID);

	//////////////////////////////////////////////////////////////////////////
	// Packet Make
	PVOID MakeLogPacket(INT16* pnPacketLength, INT8 cOperation, PVOID pvLog, INT16 nSize);

	//////////////////////////////////////////////////////////////////////////
	// Callback Function - AgpmLog
	static BOOL CBWriteLog(PVOID pData, PVOID pClass, PVOID pCustData);

	//////////////////////////////////////////////////////////////////////////
	// Callback Registration 
	BOOL SetCallbackWriteLog(ApModuleDefaultCallBack pfCallback, PVOID pClass);




	//BOOL OnReceiveLogin(AgpdLog* pcsAgpdLog, PVOID pvLoginPacket);
	//BOOL OnReceiveCombat(AgpdLog* pcsAgpdLog, PVOID pvCombatPacket);
	//BOOL OnReceiveSkill(AgpdLog* pcsAgpdLog, PVOID pvSkillPacket);
	//BOOL OnReceiveItem(AgpdLog* pcsAgpdLog, PVOID pvItemPacket);
	//BOOL OnReceiveLevelUp(AgpdLog* pcsAgpdLog, PVOID pvLevelPacket);

	//PVOID MakeLoginPacket(INT16* pnPacketLength, AgpdLog_Loginout* pcsAgpdLog);
	//PVOID MakeCombatPacket(INT16* pnPacketLength, AgpdLog_Combat* pcsAgpdLog);
	//PVOID MakeSkillPacket(INT16* pnPacketLength, AgpdLog_SkillPoint* pcsAgpdLog);
	//PVOID MakeSkillPacket(INT16* pnPacketLength, AgpdLog_SkillUse* pcsAgpdLog);
	//PVOID MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemPickUse* pcsAgpdLog);
	//PVOID MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemTrade* pcsAgpdLog);
	//PVOID MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemConvert* pcsAgpdLog);
	//PVOID MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemRepair* pcsAgpdLog);
	//PVOID MakeLevelUpPacket(INT16* pnPacketLength, AgpdLog_LevelUp* pcsAgpdLog);
	//PVOID MakeItemPacket(INT16* pnPacketLength, AgpdLog_ItemOwnerChange* pcsAgpdLog);

};

#endif // _AGSMLOG_H_