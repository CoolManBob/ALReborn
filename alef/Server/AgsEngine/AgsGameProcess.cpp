/******************************************************************************
Module:  AgsClientProcess.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 22
******************************************************************************/
#include "ApBase.h"
#include "ApPacket.h"

#include "AgsGameProcess.h"
#include "AgsEngine.h"
#include <memory>
#include <process.h>
#include "ApLockManager.h"

#include "ApFilterFunction.h"

#ifdef __PROFILE__
#include "API_AuFrameProfile.h"
#endif

/*
#define _WIN32_WINNT	0x0500
#define	WINVER			0x0500
*/

//	constructor
///////////////////////////////////////////////////////////////////////////////
AgsGameProcess::AgsGameProcess(INT16 *pnServerStatus, ApModuleManager *pmmModuleManager)
{
	m_pnServerStatus	= pnServerStatus;
	m_pmmModuleManager	= pmmModuleManager;
}

//	destructor
///////////////////////////////////////////////////////////////////////////////
AgsGameProcess::~AgsGameProcess()
{

}

// Idle Thread가 사라지면 다시 재시작 시키기 위해 
UINT WINAPI AgsGameProcess::ProtectIdleThread(PVOID pvParam)
{
	stTHREADOBJECT		stIdleThread;
	INT32				lReturnCode = 0;

	ParamIdleThread		*pstIdleThread	= (ParamIdleThread *)	pvParam;

	while (TRUE)
	{
#ifdef __PROFILE__
		__try
		{
#endif
			stIdleThread.hThread = (HANDLE)_beginthreadex(NULL, 0, AgsGameProcess::IdleThread, 
															pvParam, 0, &stIdleThread.ulThreadID);

			if ((!stIdleThread.hThread) || (!stIdleThread.ulThreadID)) 
			{
				// Idle Thread 생성 실패
				DebugBreak();
				return FALSE;
			}

			SetThreadName(stIdleThread.ulThreadID, "ProtectIdleThread1");

			switch (::WaitForSingleObject(stIdleThread.hThread, INFINITE))
			{
			case WAIT_OBJECT_0:		lReturnCode = 1; break;	// 쓰레드가 종료되었다.
			case WAIT_ABANDONED:	lReturnCode = 2; break;	// 포기된 Mutex
			case WAIT_TIMEOUT:		lReturnCode = 3; break; // Timeout에 의한 리턴
			default:				lReturnCode = -1;		//
			}

			if (*pstIdleThread->pcsGameProcess->m_pnServerStatus == GF_SERVER_STOP)
				break;

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "ProtectIdleThread ReturnCode : %d", lReturnCode);
			AuLogFile_s("LOG\\ProtectIdleThread.log", strCharBuff);

#ifdef __PROFILE__
		}
		__except(FrameProfile_ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
		{
		}
#endif
	}
	
	return FALSE;
}

// Idle Thread가 사라지면 다시 재시작 시키기 위해 
UINT WINAPI AgsGameProcess::ProtectIdleThread2(PVOID pvParam)
{
	stTHREADOBJECT		stIdleThread;
	INT32				lReturnCode = 0;

	ParamIdleThread		*pstIdleThread	= (ParamIdleThread *)	pvParam;

	while (TRUE)
	{
#ifdef __PROFILE__
		__try
		{
#endif
			stIdleThread.hThread = (HANDLE)_beginthreadex(NULL, 0, AgsGameProcess::IdleThread2, 
															pvParam, 0, &stIdleThread.ulThreadID);

			if ((!stIdleThread.hThread) || (!stIdleThread.ulThreadID)) 
			{
				// Idle Thread 생성 실패
				DebugBreak();
				return FALSE;
			}

			SetThreadName(stIdleThread.ulThreadID, "ProtectIdleThread2");

			switch (::WaitForSingleObject(stIdleThread.hThread, INFINITE))
			{
			case WAIT_OBJECT_0:		lReturnCode = 1; break;	// 쓰레드가 종료되었다.
			case WAIT_ABANDONED:	lReturnCode = 2; break;	// 포기된 Mutex
			case WAIT_TIMEOUT:		lReturnCode = 3; break; // Timeout에 의한 리턴
			default:				lReturnCode = -1;		//
			}

			if (*pstIdleThread->pcsGameProcess->m_pnServerStatus == GF_SERVER_STOP)
				break;

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "ProtectIdleThread2 ReturnCode : %d", lReturnCode);
			AuLogFile_s("LOG\\ProtectIdleThread2.log", strCharBuff);

#ifdef __PROFILE__
		}
		__except(FrameProfile_ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
		{
		}
#endif
	}

	return FALSE;
}

// Idle Thread가 사라지면 다시 재시작 시키기 위해 
UINT WINAPI AgsGameProcess::ProtectIdleThread3(PVOID pvParam)
{
	stTHREADOBJECT		stIdleThread;
	INT32				lReturnCode = 0;

	ParamIdleThread		*pstIdleThread	= (ParamIdleThread *)	pvParam;

	while (TRUE)
	{
#ifdef __PROFILE__
		__try
		{
#endif
			stIdleThread.hThread = (HANDLE)_beginthreadex(NULL, 0, AgsGameProcess::IdleThread3, 
															pvParam, 0, &stIdleThread.ulThreadID);

			if ((!stIdleThread.hThread) || (!stIdleThread.ulThreadID)) 
			{
				// Idle Thread 생성 실패
				DebugBreak();
				return FALSE;
			}

			SetThreadName(stIdleThread.ulThreadID, "ProtectIdleThread3");

			switch (::WaitForSingleObject(stIdleThread.hThread, INFINITE))
			{
			case WAIT_OBJECT_0:		lReturnCode = 1; break;	// 쓰레드가 종료되었다.
			case WAIT_ABANDONED:	lReturnCode = 2; break;	// 포기된 Mutex
			case WAIT_TIMEOUT:		lReturnCode = 3; break; // Timeout에 의한 리턴
			default:				lReturnCode = -1;		//
			}

			if (*pstIdleThread->pcsGameProcess->m_pnServerStatus == GF_SERVER_STOP)
				break;

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "ProtectIdleThread2 ReturnCode : %d", lReturnCode);
			AuLogFile_s("LOG\\ProtectIdleThread3.log", strCharBuff);

#ifdef __PROFILE__
		}
		__except(FrameProfile_ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
		{
		}
#endif
	}

	return FALSE;
}

UINT WINAPI AgsGameProcess::IdleThread(PVOID pvParam)
{
	ParamIdleThread *pParam = (ParamIdleThread*)pvParam;

#ifdef __PROFILE__
	DWORD dwTickCount = 0;
	FrameProfile_SetEnable( TRUE );
	FrameProfile_SetThreadName( "IdleThread" );
#endif

#ifdef __PROFILE__
	__try
	{
#endif
		while (TRUE)
		{
#ifdef __PROFILE__
			dwTickCount = timeGetTime();
			FrameProfile_Init();
#endif
			pParam->pcsGameProcess->m_pmmModuleManager->Idle(pParam->pcsGameProcess->m_pmmModuleManager->GetClockCount());
#ifdef __PROFILE__
			if (dwTickCount + ((AgsEngine *) pParam->pcsGameProcess->m_pmmModuleManager)->m_AsIOCPServer.GetAcceptableDelay() < timeGetTime())
			{
				FrameProfile_WriteFile();
			}
#endif
			Sleep(pParam->nmsecClockInterval);
			
			if (ApLockManager::Instance()->IsExistData())
				ApLockManager::Instance()->AllUnlock();

			if (*pParam->pcsGameProcess->m_pnServerStatus == GF_SERVER_STOP)
				break;
		}
#ifdef __PROFILE__
	}
	__except(FrameProfile_ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
	}
#endif

	return FALSE;
}

UINT WINAPI AgsGameProcess::IdleThread2(PVOID pvParam)
{
	ParamIdleThread *pParam = (ParamIdleThread*)pvParam;

#ifdef __PROFILE__
	DWORD dwTickCount = 0;
	FrameProfile_SetEnable( TRUE );
	FrameProfile_SetThreadName( "IdleThread2" );
#endif

#ifdef __PROFILE__
	__try
	{
#endif
		while (TRUE)
		{
#ifdef __PROFILE__
			dwTickCount = timeGetTime();
			FrameProfile_Init();
#endif
			pParam->pcsGameProcess->m_pmmModuleManager->Idle2(pParam->pcsGameProcess->m_pmmModuleManager->GetClockCount());
#ifdef __PROFILE__
			if (dwTickCount + ((AgsEngine *) pParam->pcsGameProcess->m_pmmModuleManager)->m_AsIOCPServer.GetAcceptableDelay() < timeGetTime())
			{
				FrameProfile_WriteFile();
			}
#endif
			Sleep(pParam->nmsecClockInterval);
			
			if (ApLockManager::Instance()->IsExistData())
				ApLockManager::Instance()->AllUnlock();

			if (*pParam->pcsGameProcess->m_pnServerStatus == GF_SERVER_STOP)
				break;
		}
#ifdef __PROFILE__
	}
	__except(FrameProfile_ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
	}
#endif

	return FALSE;
}

UINT WINAPI AgsGameProcess::IdleThread3(PVOID pvParam)
{
	ParamIdleThread *pParam = (ParamIdleThread*)pvParam;

#ifdef __PROFILE__
	DWORD dwTickCount = 0;
	FrameProfile_SetEnable( TRUE );
	FrameProfile_SetThreadName( "IdleThread3" );
#endif

#ifdef __PROFILE__
	__try
	{
#endif
		while (TRUE)
		{
#ifdef __PROFILE__
			dwTickCount = timeGetTime();
			FrameProfile_Init();
#endif
			pParam->pcsGameProcess->m_pmmModuleManager->Idle3(pParam->pcsGameProcess->m_pmmModuleManager->GetClockCount());
#ifdef __PROFILE__
			if (dwTickCount + ((AgsEngine *) pParam->pcsGameProcess->m_pmmModuleManager)->m_AsIOCPServer.GetAcceptableDelay() < timeGetTime())
			{
				FrameProfile_WriteFile();
			}
#endif
			Sleep(pParam->nmsecClockInterval);
			
			if (ApLockManager::Instance()->IsExistData())
				ApLockManager::Instance()->AllUnlock();

			if (*pParam->pcsGameProcess->m_pnServerStatus == GF_SERVER_STOP)
				break;
		}
#ifdef __PROFILE__
	}
	__except(FrameProfile_ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
	}
#endif

	return FALSE;
}

BOOL AgsGameProcess::GameThread(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	AgsGameProcess *pThis = (AgsGameProcess*)pvParam;
	AsServerSocket *pSocket = (AsServerSocket*)pvSocket;

	if (strcmp((char*)pvPacket + sizeof(PACKET_HEADER), SERVER_STRING_ID) == 0)
	{
		printf("\n서버로 설정");
		pSocket->GetPlayerData()->bServer = TRUE;
		return TRUE;
	}

	PlayerData*	pPlayerData	= pSocket->GetPlayerData();
	if (pPlayerData)
	{
		if (!pPlayerData->bServer && pPlayerData->lID != 0 &&
			pPlayerData->bCheckValidation)
		{
			// CID가 세팅되어 있다면 Valid Source인지 검사한다.
			if (!(((PPACKET_HEADER) pvPacket)->lOwnerID && 
				  ((PPACKET_HEADER) pvPacket)->lOwnerID == pPlayerData->lID))
			//if (!((UINT32 *) ((CHAR *) pvPacket + sizeof(UINT16) + sizeof(UINT8)) &&
			//	  *(UINT32 *) ((CHAR *) pvPacket + sizeof(UINT16) + sizeof(UINT8)) == pPlayerData->lID))
			{
				// 잘못된 소스다.
				printf("\n 잘못된 소스에서 패킷이 들어왔다. \n");
				return FALSE;
			}
		}

		pstCOMMAND pstCommand = new stCOMMAND;
		pstCommand->stCheckArg.bReceivedFromServer	= pPlayerData->bServer;
		pstCommand->stCheckArg.lSocketOwnerID		= pPlayerData->lID;
		pstCommand->unType = ASDP_CK_RECEIVE_DATA;
		pstCommand->pvObject = NULL;
		pstCommand->dpnid = pSocket->GetIndex();
		pstCommand->unDataSize = ((PPACKET_HEADER) pvPacket)->unPacketLength;
		CopyMemory(pstCommand->szData, pvPacket, pstCommand->unDataSize);

		pThis->ProcessCommand(pstCommand);

		delete pstCommand;
	}
	
	return TRUE;
}

BOOL AgsGameProcess::CBDisconnect(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	AgsGameProcess *pThis = (AgsGameProcess*)pvParam;
	AsServerSocket *pSocket = (AsServerSocket*)pvSocket;

	PlayerData		*pPlayerData	= pSocket->GetPlayerData();

	pstCOMMAND	pstCommand = new stCOMMAND;
	if (pPlayerData->bServer)
	{
		pstCommand->stCheckArg.bReceivedFromServer	= TRUE;
		pstCommand->stCheckArg.lSocketOwnerID		= pPlayerData->lID;

		pstCommand->unType = ASDP_CK_DELETE_PLAYER;
		pstCommand->pvObject = NULL;
		pstCommand->dpnid = pSocket->GetIndex();
		pstCommand->unDataSize = sizeof(INT32);
		CopyMemory(pstCommand->szData, &pSocket->GetPlayerData()->lID, sizeof(INT32));
	}
	else
	{
		pstCommand->stCheckArg.bReceivedFromServer	= FALSE;
		pstCommand->stCheckArg.lSocketOwnerID		= pPlayerData->lID;

		pstCommand->unType = ASDP_CK_DELETE_PLAYER;
		pstCommand->pvObject = NULL;
		pstCommand->dpnid = pSocket->GetIndex();
		pstCommand->unDataSize = sizeof(INT32) + sizeof(INT32) + sizeof(CHAR) * 32;
		CopyMemory(pstCommand->szData, &pPlayerData->lID, sizeof(INT32));
		INT32 lIndex = pSocket->GetIndex();
		CopyMemory((CHAR *) pstCommand->szData + sizeof(INT32), &lIndex, sizeof(INT32));
		CopyMemory((CHAR *) pstCommand->szData + sizeof(INT32) + sizeof(INT32), pPlayerData->szAccountName, sizeof(CHAR) * 32);
	}	


	BOOL bResult;
	if (pstCommand->stCheckArg.bReceivedFromServer)
		bResult = pThis->CmdServer(pstCommand);	// 서버 패킷 처리
	else
		bResult = pThis->CmdClient(pstCommand);	// 클라이언트 패킷 처리

	delete pstCommand;

	return bResult;
}

BOOL AgsGameProcess::ProcessCommand(pstCOMMAND pstCommand)
{
	switch (pstCommand->unType) 
	{
	case ASDP_CK_CREATE_PLAYER:
		break;

	case ASDP_CK_DELETE_PLAYER:
		{
			if (pstCommand->stCheckArg.bReceivedFromServer)
				return CmdServer(pstCommand);	// 서버 패킷 처리
			else
				return CmdClient(pstCommand);	// 클라이언트 패킷 처리
		}
		break;

	case ASDP_CK_RECEIVE_DATA:
		{
			// 해당 모듈로 Distpatch
			return m_pmmModuleManager->Dispatch(pstCommand->szData, pstCommand->unDataSize, pstCommand->dpnid, &pstCommand->stCheckArg);
		}
		break;

	default:
		break;
	};

	return FALSE;
}

BOOL AgsGameProcess::CmdClient(pstCOMMAND pstCommand)
{
	RemoveClient(pstCommand->dpnid);

	INT32	lID;
	UINT32	ulDPNID;
	CHAR	szAccountName[33];
	ZeroMemory(szAccountName, sizeof(CHAR) * 33);

	CopyMemory(&lID, pstCommand->szData, sizeof(INT32));
	CopyMemory(&ulDPNID, (CHAR *) pstCommand->szData + sizeof(INT32), sizeof(INT32));
	CopyMemory(szAccountName, (CHAR *) pstCommand->szData + sizeof(INT32) + sizeof(INT32), sizeof(CHAR) * 32);

	//if (lID != AP_INVALID_CID)
		((AgsEngine *) m_pmmModuleManager)->Disconnect(lID, ulDPNID);
	//if (szAccountName && szAccountName[0])
		((AgsEngine *) m_pmmModuleManager)->Disconnect(szAccountName, ulDPNID);

//	CHAR	strBuffer[64];
//	wsprintf(strBuffer, "AgsGameProcess : (thread_id : %d) delete player", GetCurrentThreadId());
//	WriteLog(AS_LOG_RELEASE, strBuffer);

	return TRUE;
}

BOOL AgsGameProcess::CmdServer(pstCOMMAND pstCommand)
{
	if (*(INT32 *) pstCommand->szData != AP_INVALID_SERVERID)
	{
		return ((AgsEngine *) m_pmmModuleManager)->DisconnectSvr(*(INT32 *) pstCommand->szData, pstCommand->dpnid);
	}

	return FALSE;
}

BOOL AgsGameProcess::AddNewServer(INT32 dpnidServer)
{
	return TRUE;
}

BOOL AgsGameProcess::RemoveServer(INT32 dpnidServer)
{
	return TRUE;
}

BOOL AgsGameProcess::AddNewClient(INT32 dpnidPlayer)
{
//	if (*m_pnServerStatus != GF_SERVER_START)
//	{
//		m_pDPModule->DestroyClient(dpnidPlayer);
//	}

	return TRUE;
}

BOOL AgsGameProcess::RemoveClient(INT32 dpnidPlayer)
{
	return TRUE;
}
