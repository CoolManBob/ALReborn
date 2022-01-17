/******************************************************************************
Module:  AgsEngine.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 06
******************************************************************************/

#include <WINSOCK2.H>
#include "AgsEngine.h"
#include <stdio.h>
#include <process.h>
#include "AuPacket.h"
#include "ApLockManager.h"

#include "ExceptionHandler.h"

#include "ApMemoryTracker.h"

//extern AgsmDBStream		g_csAgsmDBStream;
extern AuCircularBuffer g_AuCircularBuffer;

AgsDatabaseVender g_eAgsDatabaseVender = AGS_DB_VENDER_ORACLE;
AgsEngine* AgsEngine::m_pInstance = NULL;

AgsEngine::AgsEngine()
{
	m_pInstance = this;

	m_lGameThreadCount		= 8;
	m_lConnectionCount		= 0;
	m_lMaxSendBufferMB		= 0;
	m_lPort					= 0;
	m_nmsecClockInterval	= 200;

	ZeroMemory(m_stRecvPacketStatistics, sizeof(stPacketStatistics) * APPACKET_MAX_PACKET_TYPE);
	ZeroMemory(m_stSendPacketStatistics, sizeof(stPacketStatistics) * APPACKET_MAX_PACKET_TYPE);

	m_ulStartPacketStatisticsTime	= 0;
	m_nServerStatus		= 0;

	_originalExceptionHandler	= NULL;
	m_pcsGameProcess = NULL;
}

AgsEngine::~AgsEngine()
{
//	StopEngine();
	AuExcelTxtLibHelper::GetSingletonPtr()->DestroyInstance();
	
	if(m_pcsGameProcess)
		delete m_pcsGameProcess;

	if (_originalExceptionHandler)
	{
		SetUnhandledExceptionFilter(_originalExceptionHandler);
		_originalExceptionHandler	= NULL;
	}
}

BOOL AgsEngine::StartServer(INT32 lPort, INT32 lThreadCount, INT32 lMaxConnectionCount, char *pstrIPBlockingFileName, char *pstrPCRoomIPFileName, INT32 lMaxSendBufferMB )
{
	//m_AsIOCPServer.Initialize(lThreadCount, lPort, lMaxConnectionCount);
	m_DPClientModule.Initialize();

	m_lGameThreadCount = lThreadCount;
	m_lConnectionCount = lMaxConnectionCount;
	m_lMaxSendBufferMB = lMaxSendBufferMB;
	m_lPort = lPort;

	m_AsIOCPServer.InitLocalIPAddr(lPort);

	m_AsIOCPServer.LoadIPBlockRes( pstrIPBlockingFileName, pstrPCRoomIPFileName );


//	TRACEFILE("AgsEngine : dplay server is started");

	return TRUE;
}

BOOL AgsEngine::StartProcess()
{
	// CreateThread에서 _beginthreadex로 변경 
	// 스레드에서 CRT를 사용할 경우 CreateThread는 해당 Thread의 CRT영역을 초기화 하지 못함
	m_pcsGameProcess = new 	AgsGameProcess((INT16*)&m_nServerStatus, this);
	
	if (!StartIdleThread())
	{
//		TRACEFILE("AgsEngine : Failed!!! StartIdleThread");
		return FALSE;
	}

	if (!StartIdleThread2())
	{
//		TRACEFILE("AgsEngine : Failed!!! StartIdleThread2");
		return FALSE;
	}

	if (!StartIdleThread3())
	{
//		TRACEFILE("AgsEngine : Failed!!! StartIdleThread2");
		return FALSE;
	}

	if (!StartGameThread())
	{
//		TRACEFILE("AgsEngine : Failed!!! StartGameThread");
		return FALSE;
	}

	m_AsIOCPServer.Initialize(m_lGameThreadCount, m_lPort, m_lConnectionCount, m_lMaxSendBufferMB);

//	TRACEFILE("AgsEngine : engine is started");

	return TRUE;
}

BOOL AgsEngine::StartIdleThread()
{
	// Idle 쓰레드 초기화
	static ParamIdleThread paramIdleThread;
	paramIdleThread.pcsGameProcess = m_pcsGameProcess;
	paramIdleThread.nmsecClockInterval = m_nmsecClockInterval;
	
	m_stProtectIdleThread.hThread = (HANDLE)_beginthreadex(NULL, 0, AgsGameProcess::ProtectIdleThread, 
													(PVOID)&paramIdleThread, 0, &m_stProtectIdleThread.ulThreadID);

	if ((!m_stProtectIdleThread.hThread) || (!m_stProtectIdleThread.ulThreadID)) return FALSE;
	SetThreadName(m_stProtectIdleThread.ulThreadID, "IdleThraed1");

	return TRUE;
}

BOOL AgsEngine::StartIdleThread2()
{
	// Idle 쓰레드 초기화
	static ParamIdleThread paramIdleThread2;
	paramIdleThread2.pcsGameProcess = m_pcsGameProcess;
	paramIdleThread2.nmsecClockInterval = m_nmsecClockInterval;
	
	m_stProtectIdleThread2.hThread = (HANDLE)_beginthreadex(NULL, 0, AgsGameProcess::ProtectIdleThread2, 
													(PVOID)&paramIdleThread2, 0, &m_stProtectIdleThread2.ulThreadID);

	if ((!m_stProtectIdleThread2.hThread) || (!m_stProtectIdleThread2.ulThreadID)) return FALSE;
	SetThreadName(m_stProtectIdleThread2.ulThreadID, "IdleThraed2");

	return TRUE;
}

BOOL AgsEngine::StartIdleThread3()
{
	// Idle 쓰레드 초기화
	static ParamIdleThread paramIdleThread3;
	paramIdleThread3.pcsGameProcess = m_pcsGameProcess;
	paramIdleThread3.nmsecClockInterval = m_nmsecClockInterval;
	
	m_stProtectIdleThread3.hThread = (HANDLE)_beginthreadex(NULL, 0, AgsGameProcess::ProtectIdleThread3, 
													(PVOID)&paramIdleThread3, 0, &m_stProtectIdleThread3.ulThreadID);

	if ((!m_stProtectIdleThread3.hThread) || (!m_stProtectIdleThread3.ulThreadID)) return FALSE;
	SetThreadName(m_stProtectIdleThread3.ulThreadID, "IdleThraed3");

	return TRUE;
}

BOOL AgsEngine::StartGameThread()
{
	// Game 쓰레드 초기화
	m_AsIOCPServer.SetDispatchGameModule(m_pcsGameProcess, AgsGameProcess::GameThread, AgsGameProcess::CBDisconnect);
	return TRUE;
}

BOOL AgsEngine::Stop()
{
	// directplay stop
	m_DPClientModule.Stop();

//	m_AsIOCPServer.Destroy();

//	TRACEFILE("AgsEngine : dplay server is stopped");

	return TRUE;
}

BOOL AgsEngine::StopProcess()
{
	// wait engine stop
	///////////////////////////////////////////////////////////////////////////

	// Idle Thread 종료
	m_nServerStatus = GF_SERVER_STOP;
	
	m_SystemCommand.m_eCommand = ASSYSTEM_COMMAND_TERMINATE;

	m_AsIOCPServer.PostStatus((ULONG_PTR)&m_SystemCommand);

	//::TerminateThread(m_stProtectIdleThread.hThread, 1);
	::WaitForSingleObject(m_stProtectIdleThread.hThread, INFINITE);

	//::TerminateThread(m_stProtectIdleThread2.hThread, 1);
	::WaitForSingleObject(m_stProtectIdleThread2.hThread, INFINITE);

	//::TerminateThread(m_stProtectIdleThread3.hThread, 1);
	::WaitForSingleObject(m_stProtectIdleThread3.hThread, INFINITE);

	m_AsIOCPServer.DestroyAllClient();

	// IOCP Worker Thread 종료
	m_AsIOCPServer.Destroy();
	
//	TRACEFILE("AgsEngine : engine is stopped");

	return TRUE;
}

BOOL AgsEngine::StopEngine()
{
	if (!StopProcess())
		return FALSE;

	if (!Stop())
		return FALSE;

	//return StopModule();
	return TRUE;
}

BOOL AgsEngine::WaitStopEngine()
{
	// wait engine stop
	///////////////////////////////////////////////////////////////////////////
	if (!Stop())
		return FALSE;

	//return StopModule();
	return TRUE;
}

BOOL AgsEngine::SendPacket( PACKET_HEADER& pPacket, UINT32 ulNID, PACKET_PRIORITY ePriority, INT16 nFlag)
{
	UINT16	nLength = pPacket.unPacketLength + 1; // end guard byte

	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);		// 패킷 앞뒤로 가드바이트를 붙인다.

	if (!pvPacketRaw) return FALSE;

	CopyMemory(pvPacketRaw, &pPacket, nLength);
	
	// set guard byte
	*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
	*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;
	
	((PACKET_HEADER *) pvPacketRaw)->unPacketLength		= nLength;
	((PACKET_HEADER *) pvPacketRaw)->lOwnerID			= 0;
	((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= 0;
	((PACKET_HEADER *) pvPacketRaw)->cType				= pPacket.cType;
	
	return SendPacket(pvPacketRaw, nLength, ulNID, ePriority, nFlag);
}

BOOL AgsEngine::SendPacket(PVOID pvPacket, INT16 nLength, UINT32 ulNID, PACKET_PRIORITY ePriority, INT16 nFlag)
{
	PROFILE("AgsEngine::SendPacket");

	if (!pvPacket || nLength < 1)
	{
		ASSERT(!"잘못된 패킷이예용.. 패킷이 NULL, 또는 길이가 0이예욤"); 
		return FALSE;
	}

	if (m_nServerStatus == GF_SERVER_STOP)
		return TRUE;

	//PacketMonitoring(pvPacket, nLength, TRUE);

	if (ulNID > 1000)
	{
		if (!m_AsIOCPServer.m_csSocketManager.IsValidArrayIndex(ulNID) || !m_AsIOCPServer.m_csSocketManager[ulNID])
			return FALSE;

		EnumAsyncSendResult eResult = m_AsIOCPServer.m_csSocketManager[ulNID]->AsyncSend((CHAR*)pvPacket, nLength, ePriority);
		switch (eResult)
		{
			case ASYNC_SEND_SUCCESS:
				break;
			case ASYNC_SEND_BUFFER_FULL:
				break;

			case ASYNC_SEND_FAIL:
			case ASYNC_SEND_SOCKET_NOT_ACTIVE:
			default:
			{
				DestroyClient(ulNID);
				return FALSE;
			}
		};
	}
	else
	{
		EnumAsyncSendResult eResult = m_DPClientModule.Send((CHAR *) pvPacket, nLength, ulNID, ePriority);
		switch (eResult)
		{
			case ASYNC_SEND_SUCCESS:
			case ASYNC_SEND_BUFFER_FULL:
				{
				} break;

			case ASYNC_SEND_FAIL:
			case ASYNC_SEND_SOCKET_NOT_ACTIVE:
			default:
				{
					m_DPClientModule.Close(ulNID);
					return FALSE;
				} break;
		};
	}

	/*
	if (!m_AsIOCPServer.m_csSocketManager.IsValidArrayIndex(ulNID) || !m_AsIOCPServer.m_csSocketManager[ulNID])
		return FALSE;

//	INT16 nResult = 0;
//	if (nFlag == APMODULE_SENDPACKET_PLAYER)
//	{
		if (!m_AsIOCPServer.m_csSocketManager[ulNID]->AsyncSend((const char*)pvPacket, nLength))
		{
			DestroyClient(ulNID);
			return FALSE;
		}
//	}
//	else if (nFlag == APMODULE_SENDPACKET_ALL_PLAYERS)
//	{
//		m_AsIOCPServer.m_csSocketManager[m_AsIOCPServer.GetDPNIDClients()]->AsyncSend((const char*)pvPacket, nLength);
//	}
//	else if (nFlag == APMODULE_SENDPACKET_ALL_SERVERS)
//	{
//		m_AsIOCPServer.m_csSocketManager[m_AsIOCPServer.GetDPNIDServers()]->AsyncSend((const char*)pvPacket, nLength);
//	}
	*/

	return TRUE;
}

BOOL AgsEngine::SendPacketAllUser(PVOID pvPacket, INT16 nLength, PACKET_PRIORITY ePriority, INT16 nFlag)
{
	for (int i = 1001; i < m_AsIOCPServer.m_csSocketManager.GetMaxConnectionCount(); ++i)
	{
		if (m_AsIOCPServer.m_csSocketManager.IsValidArrayIndex(i) &&
			m_AsIOCPServer.m_csSocketManager[i] &&
			ASSERVER_STATE_ALIVE == m_AsIOCPServer.m_csSocketManager[i]->GetState() &&
			!m_AsIOCPServer.m_csSocketManager[i]->GetPlayerData()->bServer)
			SendPacket(pvPacket, nLength, i, ePriority, nFlag);
	}

	return TRUE;
}

BOOL AgsEngine::SendPacketAllUser(PACKET_HEADER& pPacket, PACKET_PRIORITY ePriority, INT16 nFlag)
{
	for (int i = 1001; i < m_AsIOCPServer.m_csSocketManager.GetMaxConnectionCount(); ++i)
	{
		if (m_AsIOCPServer.m_csSocketManager.IsValidArrayIndex(i) &&
			m_AsIOCPServer.m_csSocketManager[i] &&
			ASSERVER_STATE_ALIVE == m_AsIOCPServer.m_csSocketManager[i]->GetState() &&
			!m_AsIOCPServer.m_csSocketManager[i]->GetPlayerData()->bServer)
			SendPacket(pPacket, i, ePriority, nFlag);
	}

	return TRUE;
}

BOOL AgsEngine::SendPacketSvr(PVOID pvPacket, INT16 nLength, INT16 nServerNID)
{
	//PacketMonitoring(pvPacket, nLength, TRUE);

	return SendPacket(pvPacket, nLength, nServerNID);

	/*
	INT16 nResult = m_DPClientModule.Send((CHAR *) pvPacket, nLength, nServerNID);

	if (nResult < 0)
		return FALSE;

	return TRUE;
	*/
}

INT16 AgsEngine::GetServerStatus()
{
	return m_nServerStatus;
}

BOOL AgsEngine::SetServerStatus(INT16 nServerStatus)
{
	m_nServerStatus = nServerStatus;

	CHAR	szBuffer[128];
	sprintf(szBuffer, "changed server status : %d", nServerStatus);
//	TRACEFILE(szBuffer);

	return TRUE;
}

BOOL AgsEngine::Disconnect(INT32 lCID, UINT32 ulNID)
{
	ApModuleList*		cur_data = m_listModule;
	while(cur_data)
	{
		if(cur_data->pModule)
		{
			if(((ApModule*) cur_data->pModule)->GetModuleType() == APMODULE_TYPE_SERVER)
			{
				((AgsModule *) cur_data->pModule)->OnDisconnect(lCID, ulNID);
			}
		}

		cur_data = cur_data->next;
	}

	if (ApLockManager::Instance()->IsExistData())
		ApLockManager::Instance()->AllUnlock();

	return TRUE;
}

BOOL AgsEngine::Disconnect(CHAR *szAccountName, UINT32 ulNID)
{
	if (!szAccountName || !szAccountName[0])
		return FALSE;

	ApModuleList*		cur_data = m_listModule;
	while(cur_data)
	{
		if(cur_data->pModule)
		{
			if(((ApModule*) cur_data->pModule)->GetModuleType() == APMODULE_TYPE_SERVER)
			{
				((AgsModule *) cur_data->pModule)->OnDisconnect(szAccountName, ulNID);
			}
		}

		cur_data = cur_data->next;
	}

	if (ApLockManager::Instance()->IsExistData())
		ApLockManager::Instance()->AllUnlock();

	return TRUE;
}

BOOL AgsEngine::DisconnectSvr(INT32 lServerID, UINT32 ulNID)
{
	if (lServerID == AP_INVALID_SERVERID)
		return FALSE;

	ApModuleList*		cur_data = m_listModule;
	while(cur_data)
	{
		if(cur_data->pModule)
		{
			if(((ApModule*) cur_data->pModule)->GetModuleType() == APMODULE_TYPE_SERVER)
			{
				((AgsModule *) cur_data->pModule)->OnDisconnectSvr(lServerID, ulNID);
			}
		}

		cur_data = cur_data->next;
	}

	if (ApLockManager::Instance()->IsExistData())
		ApLockManager::Instance()->AllUnlock();

	return TRUE;
}

BOOL AgsEngine::GetLocalIPAddr(CHAR **pszBuffer, INT16 *pnNumAddr)
{
	return m_AsIOCPServer.GetLocalIPAddr(pszBuffer, pnNumAddr);
}

PlayerData* AgsEngine::GetPlayerDataForID(INT32 lNID)
{
	if (lNID > 1000)
		return m_AsIOCPServer.GetPlayerDataForID(lNID);
	else
		return m_DPClientModule.GetPlayerDataForID(lNID);
}

BOOL AgsEngine::SetIDToPlayerContext(INT32 lID, UINT32 dpnid)
{
	if (dpnid > 1000)
		return m_AsIOCPServer.SetIDToPlayerContext(lID, dpnid);
	else
		return m_DPClientModule.SetIDToPlayerContext(lID, dpnid);
}

BOOL AgsEngine::SetServerFlagToPlayerContext(UINT32 dpnid)
{
	if (dpnid > 1000)
		return m_AsIOCPServer.SetServerFlagToPlayerContext(dpnid);
	else
		return m_DPClientModule.SetServerFlagToPlayerContext(dpnid);
}

BOOL AgsEngine::SetCheckValidation(UINT32 dpnid)
{
	if (dpnid > 1000)
		return m_AsIOCPServer.SetCheckValidation(dpnid);

	return TRUE;
}

BOOL AgsEngine::ResetCheckValidation(UINT32 dpnid)
{
	if (dpnid > 1000)
		return m_AsIOCPServer.ResetCheckValidation(dpnid);

	return TRUE;
}

BOOL AgsEngine::SetSelfCID(INT32 lCID)
{
	m_AsIOCPServer.SetSelfCID(lCID);
	m_DPClientModule.m_lSelfCID = lCID;

	return TRUE;
}

BOOL AgsEngine::DestroyClient(INT32 lNID, INT32 lDestroyReason)
{
//	PVOID pPacket = g_AuCircularBuffer.Alloc(50);
//
//	*(INT16*)pPacket = sizeof(INT16) + strlen("DISCONNECT_NORMAL") + 1;
//	CopyMemory((char*)pPacket + sizeof(INT16), "DISCONNECT_NORMAL", strlen("DISCONNECT_NORMAL"));
//
//	return m_AsIOCPServer.DestroyClient(lNID, lDestroyReason, (CHAR*)pPacket);

	return m_AsIOCPServer.DestroyClient(lNID, lDestroyReason, NULL);
}

BOOL AgsEngine::SetAccountNameToPlayerContext(CHAR *szAccountName, UINT32 dpnid)
{
	return m_AsIOCPServer.SetAccountNameToPlayerContext(szAccountName, dpnid);
}

BOOL AgsEngine::GetConnectionInfo(UINT32 dpnid, UINT32 *pulRoundTripLatencyMS, UINT32 *pulThroughputBPS)
{
//	return m_DPModule.GetConnectionInfo(dpnid, pulRoundTripLatencyMS, pulThroughputBPS);
	return TRUE;
}

UINT32 AgsEngine::CreateGroup()
{
//	return ((UINT32) m_DPModule.CreateGroup());
	return 0;
}

BOOL AgsEngine::DestroyGroup(UINT32 ulGroupID)
{
//	return m_DPModule.DestroyGroup((DPNID) ulGroupID);
	return 0;
}

BOOL AgsEngine::AddPlayerToGroup(UINT32 ulGroupID, UINT32 ulAddNID)
{
//	return m_DPModule.AddPlayerToGroup((DPNID) ulGroupID, (DPNID) ulAddNID);
	return FALSE;
}

BOOL AgsEngine::RemovePlayerFromGroup(UINT32 ulGroupID, UINT32 ulRemoveNID)
{
//	return m_DPModule.RemovePlayerFromGroup((DPNID) ulGroupID, (DPNID) ulRemoveNID);
	return FALSE;
}

INT16 AgsEngine::ConnectServer(CHAR *szAddress, BOOL IsSendConnectInfo)
{
	INT32 lIndex = m_DPClientModule.Open(szAddress, IsSendConnectInfo);
	if (lIndex == AP_INVALID_SERVERID) 
		return lIndex;
	
	m_AsIOCPServer.AttachHandle((HANDLE)m_DPClientModule.m_SocketManager[lIndex]->GetHandle(), 
									(ULONG_PTR)m_DPClientModule.m_SocketManager[lIndex]);
	m_DPClientModule.m_SocketManager[lIndex]->SetState(ASSERVER_STATE_ALIVE);
	m_DPClientModule.m_SocketManager[lIndex]->AsyncRecv();
	return lIndex;
}

BOOL AgsEngine::DisconnectServer(INT16 nServerNID)
{
	return m_DPClientModule.Close(nServerNID);
}

BOOL AgsEngine::SetNumInitGameThreads(INT16 nCount)
{
	m_lGameThreadCount = nCount;

	return TRUE;
}

BOOL AgsEngine::SetClockInterval(INT16 nClockInterval_msec)
{
	m_nmsecClockInterval = nClockInterval_msec;

	return TRUE;
}

BOOL AgsEngine::PacketMonitoring(PVOID pvPacket, INT16 nLength, BOOL bIsSendPacket)
{
	if (!pvPacket || nLength < 1)
		return FALSE;

	PPACKET_HEADER pstPacket = (PPACKET_HEADER) pvPacket;

	// pstPacket의 타입과 nLength를 가지고 패킷 통계를 낸다.

	if (pstPacket->cType >= APPACKET_MAX_PACKET_TYPE)
		return FALSE;

	if (m_ulStartPacketStatisticsTime == 0)
		m_ulStartPacketStatisticsTime = GetTickCount();

	if (bIsSendPacket)
	{
		++m_stSendPacketStatistics[pstPacket->cType].ulPacketCount;
		m_stSendPacketStatistics[pstPacket->cType].ullPacketSize += nLength;
	}
	else
	{
		++m_stRecvPacketStatistics[pstPacket->cType].ulPacketCount;
		m_stRecvPacketStatistics[pstPacket->cType].ullPacketSize += nLength;
	}

	return TRUE;
}

BOOL AgsEngine::ResetPacketStatistics()
{
	ZeroMemory(m_stRecvPacketStatistics, sizeof(stPacketStatistics) * APPACKET_MAX_PACKET_TYPE);
	ZeroMemory(m_stSendPacketStatistics, sizeof(stPacketStatistics) * APPACKET_MAX_PACKET_TYPE);

	m_ulStartPacketStatisticsTime = 0;

	return TRUE;
}

pstPacketStatistics AgsEngine::GetRecvPacketStatistic(INT32 lPacketType)
{
	if (lPacketType >= APPACKET_MAX_PACKET_TYPE)
		return NULL;

	if (m_pModuleDisp[lPacketType] == NULL)
		return NULL;

	if (m_stRecvPacketStatistics[lPacketType].ullPacketSize > 0)
	{
		UINT32	ulCurrentClockCount		= GetTickCount();
		UINT32	ulBaseClockCount		= m_stRecvPacketStatistics[lPacketType].ulLastProcessClockCount;
		if (ulBaseClockCount == 0)
			ulBaseClockCount = m_ulStartPacketStatisticsTime;

		m_stRecvPacketStatistics[lPacketType].ulAveragePacketSize
			= (UINT32) (m_stRecvPacketStatistics[lPacketType].ullPacketSize / m_stRecvPacketStatistics[lPacketType].ulPacketCount);

		m_stRecvPacketStatistics[lPacketType].ulCurrentThroughputBPS
			= (UINT32)
			((m_stRecvPacketStatistics[lPacketType].ullPacketSize - m_stRecvPacketStatistics[lPacketType].ullLastProcessPacketSize) * 8
			/
			((ulCurrentClockCount - ulBaseClockCount) / 1000.0));

		m_stRecvPacketStatistics[lPacketType].ulTotalThroughputBPS
			= (UINT32)
			(m_stRecvPacketStatistics[lPacketType].ullPacketSize * 8 / ((ulCurrentClockCount - m_ulStartPacketStatisticsTime) / 1000.0));

		if (m_stRecvPacketStatistics[lPacketType].ulPeakThroughputBPS < m_stRecvPacketStatistics[lPacketType].ulCurrentThroughputBPS)
			m_stRecvPacketStatistics[lPacketType].ulPeakThroughputBPS = m_stRecvPacketStatistics[lPacketType].ulCurrentThroughputBPS;

		m_stRecvPacketStatistics[lPacketType].ulLastProcessClockCount = ulCurrentClockCount;
		m_stRecvPacketStatistics[lPacketType].ullLastProcessPacketSize = m_stRecvPacketStatistics[lPacketType].ullPacketSize;
		m_stRecvPacketStatistics[lPacketType].ulLastProcessClockCount = m_stRecvPacketStatistics[lPacketType].ulPacketCount;
	}

	return m_stRecvPacketStatistics + lPacketType;
}

pstPacketStatistics AgsEngine::GetSendPacketStatistic(INT32 lPacketType)
{
	if (lPacketType >= APPACKET_MAX_PACKET_TYPE)
		return NULL;

	if (m_pModuleDisp[lPacketType] == NULL)
		return NULL;

	if (m_stSendPacketStatistics[lPacketType].ullPacketSize > 0)
	{
		UINT32	ulCurrentClockCount		= GetTickCount();
		UINT32	ulBaseClockCount		= m_stSendPacketStatistics[lPacketType].ulLastProcessClockCount;
		if (ulBaseClockCount == 0)
			ulBaseClockCount = m_ulStartPacketStatisticsTime;

		m_stSendPacketStatistics[lPacketType].ulAveragePacketSize
			= (UINT32) (m_stSendPacketStatistics[lPacketType].ullPacketSize / m_stSendPacketStatistics[lPacketType].ulPacketCount + 0.0);

		m_stSendPacketStatistics[lPacketType].ulCurrentThroughputBPS
			= (UINT32)
			((m_stSendPacketStatistics[lPacketType].ullPacketSize - m_stSendPacketStatistics[lPacketType].ullLastProcessPacketSize) * 8
			/
			((ulCurrentClockCount - ulBaseClockCount) / 1000.0));

		m_stSendPacketStatistics[lPacketType].ulTotalThroughputBPS
			= (UINT32)
			(m_stSendPacketStatistics[lPacketType].ullPacketSize * 8 / ((ulCurrentClockCount - m_ulStartPacketStatisticsTime) / 1000.0));

		if (m_stSendPacketStatistics[lPacketType].ulPeakThroughputBPS < m_stSendPacketStatistics[lPacketType].ulCurrentThroughputBPS)
			m_stSendPacketStatistics[lPacketType].ulPeakThroughputBPS = m_stSendPacketStatistics[lPacketType].ulCurrentThroughputBPS;

		m_stSendPacketStatistics[lPacketType].ulLastProcessClockCount = ulCurrentClockCount;
		m_stSendPacketStatistics[lPacketType].ullLastProcessPacketSize = m_stSendPacketStatistics[lPacketType].ullPacketSize;
		m_stSendPacketStatistics[lPacketType].ulLastProcessClockCount = m_stSendPacketStatistics[lPacketType].ulPacketCount;
	}

	return m_stSendPacketStatistics + lPacketType;
}

UINT32 AgsEngine::GetSendTotalThroughputBPS()
{
	UINT32	ulTotalThroughputBPS	= 0;

	for (int i = 0; i < APPACKET_MAX_PACKET_TYPE; ++i)
	{
		ulTotalThroughputBPS += m_stSendPacketStatistics[i].ulTotalThroughputBPS;
	}

	return ulTotalThroughputBPS;
}

UINT32 AgsEngine::GetRecvTotalThroughputBPS()
{
	UINT32	ulTotalThroughputBPS	= 0;

	for (int i = 0; i < APPACKET_MAX_PACKET_TYPE; ++i)
	{
		ulTotalThroughputBPS += m_stRecvPacketStatistics[i].ulTotalThroughputBPS;
	}

	return ulTotalThroughputBPS;
}

BOOL AgsEngine::SetCBLoginDisconnect(PVOID pClass, AsDispatchCallBack fpDisconnect)
{
	m_AsIOCPServer.m_fpLoginDisconnect = fpDisconnect;
	m_AsIOCPServer.m_pClass = pClass;

	return TRUE;
}

BOOL AgsEngine::SetCBLKServer(PVOID pClass, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect)
{
	m_AsIOCPServer.SetDispatchLKModule(pClass, fpDispatch, fpDisconnect);
	return TRUE;
}

BOOL AgsEngine::SetCBBilling(PVOID pClass, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect)
{
	m_AsIOCPServer.SetDispatchBillingModule(pClass, fpDispatch, fpDisconnect);
	return TRUE;
}

BOOL AgsEngine::SetCBPatch(PVOID pClass, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect)
{
	m_AsIOCPServer.SetDispatchPatchModule(pClass, fpDispatch, fpDisconnect);
	return TRUE;
}

AsTimerEventData* AgsEngine::AddTimer(UINT32 uDelay, INT32 lCID, PVOID pClass, AsTimerCallBack fpTimerCallBack, PVOID pvData)
{
	return m_AsIOCPServer.m_csTimer.AddTimer(uDelay, lCID, pClass, fpTimerCallBack, pvData);
}

BOOL AgsEngine::ActiveSendBuffer(UINT32 ulNID)
{
	return TRUE;// m_AsIOCPServer.m_csSocketManager[ulNID]->ActiveSendBuffer();
}

BOOL AgsEngine::DisableSendBuffer(UINT32 ulNID)
{
	return TRUE; //m_AsIOCPServer.m_csSocketManager[ulNID]->DisableSendBuffer();
}

BOOL AgsEngine::IsPCRoom(UINT32 ulNID)
{
	return m_AsIOCPServer.m_csSocketManager[ulNID]->IsPCRoom();
}

LONG __stdcall AgsEngine::xCrashExceptionHandler(EXCEPTION_POINTERS* pExPtrs)
{
	// now, the XCrashReport comes in.
	return RecordExceptionInfo(pExPtrs, "Archlord Server", NULL, NULL);
}

BOOL AgsEngine::SetCrashReportExceptionHandler()
{
	_originalExceptionHandler = SetUnhandledExceptionFilter(xCrashExceptionHandler);

	return TRUE;
}

BOOL AgsEngine::SetLKServerSocket(UINT32 ulNID)
{
	if (ulNID > 1000)
	{
		if (!m_AsIOCPServer.m_csSocketManager.IsValidArrayIndex(ulNID) || !m_AsIOCPServer.m_csSocketManager[ulNID])
			return FALSE;

		return m_AsIOCPServer.m_csSocketManager[ulNID]->SetSocketType(SOCKET_TYPE_LKSERVER);
	}
	else
	{
		return m_DPClientModule.m_SocketManager[ulNID]->SetSocketType(SOCKET_TYPE_LKSERVER);
	}

	return TRUE;
}

BOOL AgsEngine::SetBillingSocket(UINT32 ulNID)
{

	if (ulNID > 1000)
	{
		if (!m_AsIOCPServer.m_csSocketManager.IsValidArrayIndex(ulNID) || !m_AsIOCPServer.m_csSocketManager[ulNID])
			return FALSE;

		return m_AsIOCPServer.m_csSocketManager[ulNID]->SetSocketType(SOCKET_TYPE_BILLING);
	}
	else
	{
		return m_DPClientModule.m_SocketManager[ulNID]->SetSocketType(SOCKET_TYPE_BILLING);
	}

	return TRUE;
}

AgsModule::AgsModule()
{
	SetModuleType(APMODULE_TYPE_SERVER);
}

AgsModule::~AgsModule()
{
}

/*
BOOL AgsModule::AddModule(PVOID pModuleManager, AsCMDQueue* pqueueResult, AsCMDQueue* pqueueResultSvr)
{
	m_pqueueResult = pqueueResult;
	m_pqueueResultSvr = pqueueResultSvr;

	return ApModule::AddModule(pModuleManager);
}
*/

BOOL AgsModule::GetLocalIPAddr(CHAR **pszBuffer, INT16 *pnNumAddr)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->GetLocalIPAddr(pszBuffer, pnNumAddr);
}

DWORD AgsModule::GetDPNID(PVOID pszData, INT16 nDataSize)
{
	DWORD	dpnid;

	CopyMemory(&dpnid, (CHAR *) pszData + nDataSize - sizeof(DWORD), sizeof(DWORD));

	return dpnid;
}

PlayerData* AgsModule::GetPlayerDataForID(INT32 lNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->GetPlayerDataForID(lNID);
}

BOOL AgsModule::SetIDToPlayerContext(INT32 lID, UINT32 dpnid)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SetIDToPlayerContext(lID, dpnid);
}

BOOL AgsModule::SetAccountNameToPlayerContext(CHAR *szAccountName, UINT32 dpnid)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SetAccountNameToPlayerContext(szAccountName, dpnid);
}

BOOL AgsModule::SetServerFlagToPlayerContext(UINT32 dpnid)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SetServerFlagToPlayerContext(dpnid);
}

BOOL AgsModule::SetCheckValidation(UINT32 dpnid)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SetCheckValidation(dpnid);
}

BOOL AgsModule::ResetCheckValidation(UINT32 dpnid)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->ResetCheckValidation(dpnid);
}

CHAR* AgsModule::GetPlayerIPAddress(UINT32 lNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return NULL;

	if (NULL == pModuleManager->m_AsIOCPServer.m_csSocketManager[lNID]) return NULL;
	return inet_ntoa(pModuleManager->m_AsIOCPServer.m_csSocketManager[lNID]->GetRemoteInfo()->sin_addr);
}

BOOL AgsModule::GetConnectionInfo(UINT32 dpnid, UINT32 *pulRoundTripLatencyMS, UINT32 *pulThroughputBPS)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->GetConnectionInfo(dpnid, pulRoundTripLatencyMS, pulThroughputBPS);
}

UINT32 AgsModule::CreateGroup()
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->CreateGroup();
}

BOOL AgsModule::DestroyGroup(UINT32 ulGroupID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->DestroyGroup(ulGroupID);
}

BOOL AgsModule::AddPlayerToGroup(UINT32 ulGroupID, UINT32 ulAddNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->AddPlayerToGroup(ulGroupID, ulAddNID);
}

BOOL AgsModule::RemovePlayerFromGroup(UINT32 ulGroupID, UINT32 ulRemoveNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->RemovePlayerFromGroup(ulGroupID, ulRemoveNID);
}

/*
UINT32 AgsModule::GetClockCount()
{
	return GetTickCount();
}
*/

INT16 AgsModule::GetServerStatus()
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->GetServerStatus();
}

BOOL AgsModule::SetServerStatus(INT16 nServerStatus)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SetServerStatus(nServerStatus);
}

BOOL AgsModule::SendPacketSvr(PVOID pvPacket, INT16 nLength, INT16 nServerNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SendPacketSvr(pvPacket, nLength, nServerNID);
}

INT16 AgsModule::ConnectServer(CHAR *szAddress, BOOL IsSendConnectInfo)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->ConnectServer(szAddress, IsSendConnectInfo);
}

BOOL AgsModule::DisconnectServer(INT16 nServerNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->DisconnectServer(nServerNID);
}

//		DestroyClient
//	Functions
//		- Client와의 접속을 끊는다.
//	Arguments
//		- dpnid : 끊을 클라이언트 DPNID
//	Return value
//		- BOOL : 성공 여부
///////////////////////////////////////////////////////////////////////////////
BOOL AgsModule::DestroyClient(INT32 lNID, INT32 lDestroyReason)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->DestroyClient(lNID, lDestroyReason);
}

BOOL AgsModule::ResetPacketStatistics()
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->ResetPacketStatistics();
}

pstPacketStatistics AgsModule::GetRecvPacketStatistic(INT32 lPacketType)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->GetRecvPacketStatistic(lPacketType);
}

pstPacketStatistics AgsModule::GetSendPacketStatistic(INT32 lPacketType)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->GetSendPacketStatistic(lPacketType);
}

UINT32 AgsModule::GetSendTotalThroughputBPS()
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->GetSendTotalThroughputBPS();
}

UINT32 AgsModule::GetRecvTotalThroughputBPS()
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->GetRecvTotalThroughputBPS();
}

BOOL AgsModule::ActiveSendBuffer(UINT32 ulNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->ActiveSendBuffer(ulNID);
}

BOOL AgsModule::DisableSendBuffer(UINT32 ulNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->DisableSendBuffer(ulNID);
}

BOOL AgsModule::IsPCRoom(UINT32 ulNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->IsPCRoom(ulNID);
}

AsTimerEventData* AgsModule::AddTimer(UINT32 uDelay, INT32 lCID, PVOID pClass, AsTimerCallBack fpTimerCallBack, PVOID pvData)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->AddTimer(uDelay, lCID, pClass, fpTimerCallBack, pvData);
}

BOOL AgsModule::SetLKServerSocket(UINT32 ulNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SetLKServerSocket(ulNID);
}

BOOL AgsModule::SetBillingSocket(UINT32 ulNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SetBillingSocket(ulNID);
}

BOOL AgsModule::SendPacketUser(PACKET_HEADER& pPacket, UINT32 ulNID)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SendPacket(pPacket, ulNID);
}

BOOL AgsModule::SendPacketAllUser(PVOID pvPacket, INT16 nLength, PACKET_PRIORITY ePriority, INT16 nFlag)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SendPacketAllUser(pvPacket, nLength, ePriority, nFlag);
}

BOOL AgsModule::SendPacketAllUser(PACKET_HEADER& pPacket, PACKET_PRIORITY ePriority, INT16 nFlag)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return FALSE;

	return pModuleManager->SendPacketAllUser(pPacket, ePriority, nFlag);
}

#ifdef __PROFILE__
void AgsModule::SetProfileDelay(DWORD dwDelay)
{
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	if (!pModuleManager)
		return;

	return pModuleManager->m_AsIOCPServer.SetAcceptableDelay(dwDelay);
}
#endif

/*
//		Send
//	Functions
//		- 데이타를 stCOMMAND 구조체에 넣어 m_pqueueResult에 넣는다.
//	Arguments
//		- dpnid : 데이타 전송할 DPNID
//		- pvCharacter : 데이타를 보내는 character pointer
//		- pszData : 보낼 데이타 포인터
//		- nDataSize : 데이타 크기
//		- unType : 보낼 데이타 타입
//			AGSNP_CMDTYPE_SEND_CLIENT  : send client
//			AGSNP_CMDTYPE_SEND_CLIENT_WHOLE : send whole client
//	Return value
//		- queue에 넣은 결과
///////////////////////////////////////////////////////////////////////////////
BOOL AgsModule::Send(DPNID dpnid, PVOID pvCharacter, PVOID pszData, INT16 nDataSize, UINT16 unType)
{
	stCOMMAND	stCommand;

	stCommand.dpnid = dpnid;
	stCommand.pvCharacter = pvCharacter;
	stCommand.unDataSize = nDataSize;
	strncpy(stCommand.szData, (char *) pszData, nDataSize);
	stCommand.unType = unType;

	return m_pqueueResult->PushCommand(&stCommand);
}

//		SendServer
//	Functions
//		- 데이타를 stCOMMAND 구조체에 넣어 m_pqueueResult에 넣는다.
//	Arguments
//		- dpnid : 데이타 전송할 DPNID
//		- pvCharacter : 데이타를 보내는 character pointer
//		- pszData : 보낼 데이타 포인터
//		- nDataSize : 데이타 크기
//		- unType : 보낼 데이타 타입
//			AGSNP_CMDTYPE_SEND_SERVER  : send server
//			AGSNP_CMDTYPE_SEND_SERVERS : send server group
//	Return value
//		- queue에 넣은 결과
///////////////////////////////////////////////////////////////////////////////
BOOL AgsModule::SendServer(DPNID dpnid, PVOID pvServer, PVOID pszData, INT16 nDataSize, UINT16 unType)
{
	stCOMMANDSVR	stCommandSvr;

	stCommandSvr.dpnid = dpnid;
	stCommandSvr.pvServer = pvServer;
	stCommandSvr.unDataSize = nDataSize;
	strncpy(stCommandSvr.szData, (char *) pszData, nDataSize);
	stCommandSvr.unType = unType;

	return m_pqueueResultSvr->PushCommand(&stCommandSvr);
}
*/