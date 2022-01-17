// AsListener.cpp: implementation of the AsListener class.
//
//////////////////////////////////////////////////////////////////////

#include "AsListener.h"
#include "MagDebug.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AsListener::AsListener() : AsObject(ASOBJECT_LISTENER), m_hSocket(INVALID_SOCKET)
{
	m_lActiveSocketCount = 0;
}

AsListener::~AsListener()
{

}

BOOL AsListener::CreateListener(UINT16 lPort, INT32 lBacklog)
{
	if (INVALID_SOCKET != m_hSocket) return FALSE;

	m_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);

	if (INVALID_SOCKET == m_hSocket)
	{
		ASSERT(!"Listen 소켓을 생성할수 없음");
		return FALSE;
	}

	SOCKADDR_IN stListener;
	memset(&stListener, 0, sizeof(SOCKADDR_IN));
	stListener.sin_family = AF_INET;
	stListener.sin_addr.s_addr = htonl(INADDR_ANY);
	stListener.sin_port = htons(lPort);

	INT32 lResult = ::bind(m_hSocket, (struct sockaddr*)&stListener, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == lResult)
	{
		ASSERT(!"Bind 명령을 수행할수 없음");
		return FALSE;
	}

	lResult = ::listen(m_hSocket, lBacklog);
	if (SOCKET_ERROR == lResult)
	{
		ASSERT(!"Listen 명령을 수행할수 없음");
		return FALSE;
	}

	return TRUE;
}

BOOL AsListener::Initialize(UINT16 lPort, INT32 lBacklog)
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	if (!CreateListener(lPort, lBacklog)) return FALSE;

	m_lPort		= lPort;
	m_lBacklog	= lBacklog;

	return TRUE;
}

BOOL AsListener::AsyncAccept(AsServerSocket *pSocket)
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	if ((ASSERVER_STATE_DEAD == pSocket->GetState()) || (ASSERVER_STATE_INIT == pSocket->GetState()))
	{
		DWORD dwReceived = 0;
		INT32 lResult = ::AcceptEx(m_hSocket, pSocket->GetHandle(), &pSocket->m_BufferIn[0], 0, 0, sizeof(SOCKADDR_IN) + 16, 
									&dwReceived, &pSocket->m_ovlIn);
		if (SOCKET_ERROR == lResult)
		{
			// JNY TODO : 에러 처리
			INT32 lError = ::WSAGetLastError();
			ASSERT(!"AcceptEx 실행에서 오류");
			return FALSE;
		}
		else pSocket->SetState(ASSERVER_STATE_ACCEPT);
	}

	::InterlockedDecrement(&m_lActiveSocketCount);
	return TRUE;
}

BOOL AsListener::SetSocketOption(AsServerSocket *pSocket)
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	INT32 lResult = ::setsockopt(pSocket->GetHandle(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
									(const char *)&m_hSocket, sizeof(SOCKET));
	if (SOCKET_ERROR == lResult)
	{
		// JNY TODO : 에러 처리
		INT32 lError = ::WSAGetLastError();
		ASSERT(!"소켓 옵션 변경 오류");
		return FALSE;
	}

	pSocket->GetRemoteAddress();
	pSocket->SetState(ASSERVER_STATE_ALIVE);

	::InterlockedIncrement(&m_lActiveSocketCount);
	return TRUE;
}