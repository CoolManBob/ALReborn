// AgsmServerStatus_IOCPServer.h
// (C) NHN - ArchLord Development Team
// steeple, 2005. 03. 31.
//

#include "AgsmServerStatus_IOCPServer.h"

static UINT WINAPI AgsmServerStatus_MainLoop(PVOID pvParam)
{
	AsIOCPServer *pThis = (AsIOCPServer*)pvParam;

	DWORD			dwTransferred = 0;
	LPOVERLAPPED	pOverlapped = NULL;
	AsObject		*pObject = NULL;
	DWORD			dwTickCount = 0;

	while(TRUE)
	{
		BOOL bResult = pThis->GetStatus((PULONG_PTR)&pObject, &dwTransferred, &pOverlapped, INFINITE);

		if (bResult)
		{
			switch (pObject->GetType())
			{
			case ASOBJECT_LISTENER:
				{
					pThis->ProcessListener(pObject, (AsOverlapped*)pOverlapped);	
					printf("[AgsmServerStatus_MainLoop] Accept Socket ID : %d\n", ((AsOverlapped*)pOverlapped)->pSocket->GetIndex());

					break;
				}
			case ASOBJECT_SOCKET:	
				{
					AsOverlapped *pAsOverlapped = (AsOverlapped*)pOverlapped;
					if (pAsOverlapped->pSocket->IsRead(pOverlapped))
					{
						pThis->ProcessSocket(pObject, dwTransferred);
					}
					else
					{
						pAsOverlapped->pSocket->SendComplete();
					}
					break;
				}
			case ASOBJECT_TIMER:
				{
					pThis->ProcessTimer(pObject);					
					break;
				}
				
			case ASOBJECT_SYSTEM:
				{
					// AsIOCPServer의 System Command
					AsSystemCommand *pSystemCommand = (AsSystemCommand*)pObject;

					// 게임 서버를 종료한다.
					if (ASSYSTEM_COMMAND_TERMINATE == pSystemCommand->m_eCommand)
					{
						// 남아있는 다른 스레드를 종료시키기 위해 Post 시킨다.
						pThis->PostStatus((ULONG_PTR)pSystemCommand);
						return TRUE;
					}
				}
			default:
				printf("정의되지 않은 Object Type 입니다.\n");
			}
		}
		else
		{
			// 비정상적인 I/O 작업 완료
			// JNY TODO : Exception 처리가 필요한 부분... 혹은 Socket 연결 종료 시켜야 되는부분
			if (ASOBJECT_LISTENER == pObject->GetType())
			{
				// Error로 들어온 Socket Type이 LISTENER이면 ServerSocket을 ReuseSocket으로 등록
				printf("ASOBJECT_LISTENER Reuse Socket\n");
				((AsOverlapped*)pOverlapped)->pSocket->SetState(ASSERVER_STATE_ALIVE);
				pThis->DestroyClient(((AsOverlapped*)pOverlapped)->pSocket);
			}
			else if (ASOBJECT_SOCKET == pObject->GetType())
			{
				AsOverlapped *pAsOverlapped = (AsOverlapped*)pOverlapped;
				if (pAsOverlapped)
				{
					AsServerSocket* pSocket = pAsOverlapped->pSocket;

					printf("[AgsmServerStatus_MainLoop] bResult FALSE에 의한 종료\n");
					pThis->DestroyClient(pSocket);
				}
			}
		}
	}

	return FALSE;
}

AgsmServerStatus_IOCPServer::AgsmServerStatus_IOCPServer()
{
}

AgsmServerStatus_IOCPServer::~AgsmServerStatus_IOCPServer()
{
}

BOOL AgsmServerStatus_IOCPServer::Initialize(INT32 lThreadCount, INT32 lPort, INT32 lMaxConnectionCount, INT32 lMaxSendBufferMB)
{
	// IOCP 생성
	if (!Create(lThreadCount, AgsmServerStatus_MainLoop, (PVOID)this, "AgsmServerStatus_IOCPServer")) return FALSE;
	
	// 리슨 소켓 생성
	if (!m_csListener.Initialize(lPort)) return FALSE;

	// 소켓 매니저 초기화
	if (!m_csSocketManager.Initialize(lMaxConnectionCount, &m_csTimer)) return FALSE;

	// 생성된 소켓들을 Listener에 Accept를 걸어준다.
	for (INT32 i = 0; i < lMaxConnectionCount; i++)
	{
		m_csListener.AsyncAccept(m_csSocketManager[i]);
	}

	// 리슨 소켓을 IOCP에 연결
	if (!AttachHandle((HANDLE)m_csListener.GetHandle(), (ULONG_PTR)&m_csListener)) return FALSE;

	// lMaxSendBufferMB가 MByte 단위이어서.. byte 단위로 바꾼다.
	m_csSocketManager.SetMaxSendBuffer(lMaxSendBufferMB * 1000000);

	m_csTimer.Initialize(this, 100);

	return TRUE;
}

