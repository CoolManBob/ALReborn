// AsWSABufManager.cpp: implementation of the AsWSABufManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AsWSABufManager.h"
#include "ApMemory.h"
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////

AsWSABufManager::AsWSABufManager()
{
	Initialize();
}

AsWSABufManager::~AsWSABufManager()
{
}

void AsWSABufManager::Initialize()
{
	m_lCurrentCount = 0;
	m_lTotalBufSize	= 0;
	bSetTimerEvent	= FALSE;
	
	ZeroMemory(m_WsaBuffer, sizeof(m_WsaBuffer));
	ZeroMemory(m_Priority, sizeof(m_Priority));
	ZeroMemory(m_lPriorityCount, sizeof(m_lPriorityCount));
}

PACKET_PRIORITY AsWSABufManager::GetLastPriority(PACKET_PRIORITY ePriority)
{
	for (INT32 iLowestPriority = PACKET_PRIORITY_6; iLowestPriority >= (INT32)ePriority; --iLowestPriority)
	{
		if (m_lPriorityCount[iLowestPriority] > 0)
			return (PACKET_PRIORITY)iLowestPriority;
	}

	return PACKET_PRIORITY_NONE;
}

BOOL AsWSABufManager::RemoveWSABuffer(INT32 lIndex)
{
//	if (MAX_WSABUF_COUNT != (lIndex + 1))
//	{
		--m_lPriorityCount[m_Priority[lIndex]];
		m_lTotalBufSize -= m_WsaBuffer[lIndex].len;
		memmove((PVOID)&m_WsaBuffer[lIndex], (PVOID)&m_WsaBuffer[lIndex + 1], sizeof(WSABUF) * (m_lCurrentCount - lIndex - 1));
		ZeroMemory(&m_WsaBuffer[m_lCurrentCount - 1], sizeof(WSABUF));
		memmove((PVOID)&m_Priority[lIndex], (PVOID)&m_Priority[lIndex + 1], sizeof(PACKET_PRIORITY) * (m_lCurrentCount - lIndex - 1));
		m_Priority[m_lCurrentCount - 1] = PACKET_PRIORITY_NONE;
		--m_lCurrentCount;
//	}

	return TRUE;
}

Enum_ADD_PACKET_RESULT AsWSABufManager::AddPacket(CHAR* pData, INT32 lLength, PACKET_PRIORITY ePriority)
{
	if (MAX_WSABUF_COUNT <= m_lCurrentCount)
	{
		return ADD_PACKET_RESULT_DESTROY;
	}
	else
	{
		m_WsaBuffer[m_lCurrentCount].buf	= pData;
		m_WsaBuffer[m_lCurrentCount].len	= lLength;
		m_Priority[m_lCurrentCount]			= ePriority;
		++m_lPriorityCount[ePriority];

		m_lTotalBufSize += lLength;
		++m_lCurrentCount;

//		ASSERT ( m_lTotalBufSize < 10240 * 2);
//		ASSERT ( m_lTotalBufSize > 8);
	}
	
	return ADD_PACKET_RESULT_SUCCESS;
}

CHAR* AsWSABufManager::GetStartBuffer()
{
	return (CHAR*)m_WsaBuffer;
}

void AsWSABufManager::BufferClear()
{
	m_lCurrentCount = 0;
	m_lTotalBufSize = 0;

	ZeroMemory(m_WsaBuffer, sizeof(m_WsaBuffer));
	ZeroMemory(m_Priority, sizeof(m_Priority));
	ZeroMemory(m_lPriorityCount, sizeof(m_lPriorityCount));
}