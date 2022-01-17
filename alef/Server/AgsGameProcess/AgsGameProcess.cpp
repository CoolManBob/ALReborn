/******************************************************************************
Module:  AgsClientProcess.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 22
******************************************************************************/

#include "ApPacket.h"

#include "AgsGameProcess.h"
#include "AsDP8.h"

/*
#define _WIN32_WINNT	0x0500
#define	WINVER			0x0500
*/

//	constructor
///////////////////////////////////////////////////////////////////////////////
AgsGameProcess::AgsGameProcess(INT16 *pnServerStatus, INT16 nQueueIndex, AsDP8 *pDPModule, 
							   AsCMDQueue *pqueueRecv, AsCMDQueue *pqueueRecvSvr, ApModuleManager *pmmModuleManager)
{
	m_pnServerStatus = pnServerStatus;

	m_pDPModule = pDPModule;

	m_nQueueIndex = nQueueIndex;
	m_pqueueRecv = pqueueRecv;
	m_pqueueRecvSvr = pqueueRecvSvr;
	m_pmmModuleManager = pmmModuleManager;

	/*
	if (nQueueIndex == 0)
		m_pqueueNewClient->Initialize(AGSCP_CMD_QUEUE_SIZE);

	m_pqueueGameProc[m_nQueueIndex]->Initialize(AGSCP_CMD_QUEUE_SIZE);
	*/

	m_pqueueRecv->Initialize(AGSGP_RECV_CMD_QUEUE_SIZE, sizeof(stCOMMAND));

	m_nNumClient = 0;

	/*
	// 요건 ManageProcess 에서 일괄적으로 처리해준다. 여기선 따로 할 필요 없다.
	m_hClockTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	LARGE_INTEGER li = { 0 };
	SetWaitableTimer(m_hClockTimer, &li, AGSGP_PROCESS_CYCYE_TIME, UpdateAPCRoutine, NULL, FALSE);
	*/
}

//	destructor
///////////////////////////////////////////////////////////////////////////////
AgsGameProcess::~AgsGameProcess()
{
	CloseHandle(m_hClockTimer);
}

//		Process
//	Functions
//		- receive queue에 들어오는 데이타를 처리한다.
//	Arguments
//		- none
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsGameProcess::Process()
{
	INT16	nProcessCommand = 0;

	UINT32	ulThreadID = GetCurrentThreadId();

	printf("AgsGameProcess : (thread_id : %d) enter thread main loop\n", ulThreadID);

	for ( ; ; )
	{
		nProcessCommand += ProcessServer();
		nProcessCommand += ProcessClient();

		if (!nProcessCommand)
			Sleep(50);

		nProcessCommand = 0;
	}

	return TRUE;
}

BOOL AgsGameProcess::AddNewServer(DPNID dpnidServer)
{
	return TRUE;
}

BOOL AgsGameProcess::RemoveServer(DPNID dpnidServer)
{
	return TRUE;
}

//		ProcessServer
//	Functions
//		- command queue로부터 데이타를 가져와 처리를 한다.
//			1. 새로 접속한 서버 처리를 한다.
//			2. 접속 종료된 서버 처리를 한다.
//			3. 그외의 데이타는 모듈메니저를 통해 모듈로 넘긴다.
//	Arguments
//		- none
//	Return value
//		- INT16 : 처리한 command 갯수
///////////////////////////////////////////////////////////////////////////////
INT16 AgsGameProcess::ProcessServer()
{
	for (int i = 0; i < 5; i++)
	{
		stCOMMANDSVR	stCommandSvr;

		if (!m_pqueueRecvSvr->PopCommand(&stCommandSvr))
			return i;

		printf("AgsGameProcess : (thread_id : %d) receive server data from dplay\n", GetCurrentThreadId());

		switch (stCommandSvr.unType) {
		case ASDP_CK_CREATE_PLAYER:

			AddNewServer(((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->dpnidPlayer);

			/*
			// 새로운 서버가 접속했다는 내부적으로만 사용되는 패킷을 만들어 큐에 넣는다.
			PACKET_INTERNAL_NEW_SERVER	packetNewServer;

			packetNewServer.cType = PACKET_TYPE_INTERNAL;
			packetNewServer.cAction = PACKET_ACTION_INTERNAL_NEW_SERVER;
			packetNewServer.dpnidServer = ((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->dpnidPlayer;
			*/

			break;

		case ASDP_CK_DELETE_PLAYER:

			RemoveServer(((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->dpnidPlayer);

			if (((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->lID != AP_INVALID_SERVERID)
				m_pmmModuleManager->DisconnectSvr(((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->lID);

			break;

		case ASDP_CK_RECEIVE_DATA:

			m_pmmModuleManager->Dispatch(stCommandSvr.szData, stCommandSvr.unDataSize, stCommandSvr.dpnid);
			break;

		default:
			break;
		};
	}

	return i;
}

BOOL AgsGameProcess::AddNewClient(DPNID dpnidPlayer)
{
	if (*m_pnServerStatus != GF_SERVER_START)
	{
		m_pDPModule->DestroyClient(dpnidPlayer);
	}

	//MessageBox(NULL, "add new client", "Game Process", 1);

	return TRUE;
}

BOOL AgsGameProcess::RemoveClient(DPNID dpnidPlayer)
{
	return TRUE;
}

//		ProcessClient
//	Functions
//		- command queue로부터 데이타를 가져와 처리를 한다.
//			1. 새로 들어온 캐릭터 처리를 한다.
//			2. 접속 종료된 캐릭터 처리를 한다.
//			3. 그외의 데이타는 모듈메니저를 통해 모듈로 넘긴다.
//	Arguments
//		- none
//	Return value
//		- INT16 : 처리한 command 갯수
///////////////////////////////////////////////////////////////////////////////
INT16 AgsGameProcess::ProcessClient()
{
	for (int i = 0; i < 5; i++)
	{
		stCOMMAND	stCommand;

		//while (!m_pqueueRecv->PopCommand(&stCommand))
		//{
		//	Sleep(50);
		//}

		if (!m_pqueueRecv->PopCommand(&stCommand))
			return i;

		printf("AgsGameProcess : (thread_id : %d) receive client data from dplay\n", GetCurrentThreadId());

		switch (stCommand.unType) {
		case ASDP_CK_CREATE_PLAYER:

			AddNewClient(((AS_APP_PLAYER_INFO *) stCommand.pvCharacter)->dpnidPlayer);

			// 필요하다면 새로운 클라이언트가 접속했다는 내부적으로만 사용되는 패킷을 만들어 큐에 넣는다.

			break;

		case ASDP_CK_DELETE_PLAYER:

			RemoveClient(((AS_APP_PLAYER_INFO *) stCommand.pvCharacter)->dpnidPlayer);

			if (((AS_APP_PLAYER_INFO *) stCommand.pvCharacter)->lID != AP_INVALID_CID)
				m_pmmModuleManager->Disconnect(((AS_APP_PLAYER_INFO *) stCommand.pvCharacter)->lID);
			
			break;

		case ASDP_CK_RECEIVE_DATA:

			//MessageBox(NULL, "Receive Data From DP. Process Dispatch()", "AgsGameProcess", 1);
			//printf("AgsGameProcess : (thread_id : %d) receive data from dplay\n", GetCurrentThreadId());

			printf("AgsGameProcess : (thread_id : %d) dispatch() received data\n", GetCurrentThreadId());
			//MessageBox(NULL, "received data from DP.", "AgsGameProcess", 1);
			m_pmmModuleManager->Dispatch(stCommand.szData, stCommand.unDataSize, stCommand.dpnid);
			break;

		default:
			break;
		};
	}

	return i;
}

/*
//		UpdateAPCRoutine
//	Functions
//		- //process clock 마다 모듈의 Updates() 함수를 호출해준다.
//		- character의 각 상태들을 점검해 상태를 업데이트 한다.
//	Arguments
//		- pvArgToCompletionRoutine
//		- dwTimerLowValue
//		- dwTimerHighValue
//	Return value
//		- VOID
///////////////////////////////////////////////////////////////////////////////
VOID APIENTRY AgsGameProcess::UpdateAPCRoutine(PVOID pvArgToCompletionRoutine, 
											   DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	//m_pmmModuleManager->Updates();

	return;
}
*/