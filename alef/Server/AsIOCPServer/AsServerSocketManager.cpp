// AsServerSocketManager.cpp: implementation of the AsServerSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AsServerSocketManager.h"
#include "MagDebug.h"
#include "AuCircularBuffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern AuCircularBuffer	g_AuCircularSendBuffer;

AsServerSocketManager::AsServerSocketManager() : m_pSocketPool(NULL), m_lMaxConnectionCount(0)
{
	m_eDefaultSocketType = SOCKET_TYPE_ARCHLORD;
}

AsServerSocketManager::~AsServerSocketManager()
{
	g_AuCircularSendBuffer.Destroy();

	if (m_pSocketPool) delete [] m_pSocketPool;
}

BOOL AsServerSocketManager::Initialize(INT32 lMaxConnectionCount, AsTimer *pcsTimer)
{
	m_pSocketPool = new AsServerSocket[lMaxConnectionCount];
	m_lMaxConnectionCount = lMaxConnectionCount;

	// Index 부여
	for (INT32 i = 0; i < m_lMaxConnectionCount; i++)
	{
		m_pSocketPool[i].SetSocketType(m_eDefaultSocketType);

		operator[](i)->SetIndex(i);
		operator[](i)->SetTimer(pcsTimer);
	}

	return m_pSocketPool != NULL ? TRUE : FALSE;
}

BOOL AsServerSocketManager::IsValidPointer(AsServerSocket* pSocket)
{
	// 소켓 포인터가 소켓 풀링의 영역에 있으면 유효한 포인터로 간주
	if ((pSocket >= m_pSocketPool) && (pSocket <= (m_pSocketPool + m_lMaxConnectionCount)))
		return TRUE;
	else return FALSE;
}

AsServerSocket* AsServerSocketManager::operator[](INT32 lIndex)
{
	if ((lIndex < 0) || (lIndex >= m_lMaxConnectionCount))
	{
		ASSERT(!"AsServerSocketManager : 유효한 lIndex가 아님");
		return NULL;
	}

	return &m_pSocketPool[lIndex];
}

VOID AsServerSocketManager::DisconnectAll()
{
	for (INT32 i = 0; i < m_lMaxConnectionCount; i++)
	{
		operator[](i)->Remove();
	}
}

BOOL AsServerSocketManager::IsValidArrayIndex(INT32 lArrayIndex)
{
	if (lArrayIndex < 0 || lArrayIndex >= m_lMaxConnectionCount)
		return FALSE;

	return TRUE;
}

BOOL AsServerSocketManager::SetMaxSendBuffer(INT32 lBufferSize)
{
	g_AuCircularSendBuffer.Init(lBufferSize);

	return TRUE;
}

void AsServerSocketManager::SetDefaultSocketType(EnumSocketType eSocketType)
{
	m_eDefaultSocketType = eSocketType;
}

void AsServerSocketManager::SendPacketToAll(PVOID pvPacket, INT32 lPacketLength, PACKET_PRIORITY ePriority)
{
	//INT32	lIndex;

	for (INT32 lIndex = 0; lIndex < m_lMaxConnectionCount; lIndex++)
	{
		EnumAsyncSendResult eResult = m_pSocketPool[lIndex].AsyncSend((CHAR*)pvPacket, lPacketLength, ePriority);
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
				//printf("AsServerSocketManager::SendPacketToAll() Error Sending Packet\n");
				return;
			}
		};
	}
}
