// AuCircularBuffer.cpp: implementation of the AuCircularBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "AuCircularBuffer.h"
#include "ApPacket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define AUCIRCULAR_BUFFER_GUARD_BYTE 0xFEFEFEFE


AuCircularBuffer::AuCircularBuffer()
{
	m_pBuffer		= NULL;
	m_lSize			= 0;
	m_lCurrentIndex = 0;
}

AuCircularBuffer::~AuCircularBuffer()
{
	Destroy();
}

BOOL AuCircularBuffer::Init(LONG lSize)
{
	ASSERT(0 < lSize);

	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result()) return FALSE;

	m_lSize		= lSize;
	m_pBuffer	= new CHAR[m_lSize];

	if (m_pBuffer)
	{
		//VirtualLock(m_pBuffer, lSize);
		return TRUE;
	}
	else return FALSE;
}

VOID AuCircularBuffer::Destroy()
{
	AuAutoLock Lock(m_Mutex);
	if (Lock.Result())
	{
		if (m_pBuffer)
		{
			//VirtualUnlock(m_pBuffer, m_lSize);
			delete [] m_pBuffer;
		}

		m_pBuffer = NULL;
	}
}

PVOID AuCircularBuffer::Alloc(LONG lAllocSize)
{
	ASSERT(lAllocSize < 1024 * 1024);
	ASSERT(lAllocSize >= 0);

	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result()) return NULL;

	if (lAllocSize > 1024 * 1024)
	{
#ifdef _DEBUG
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "1MB 이상의 CircularBuffer AllocSize : %d", lAllocSize);
		AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);
#endif
		return NULL;
	}

	if (lAllocSize <= 0)
	{
#ifdef _DEBUG
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "0 이하의 CircularBuffer AllocSize : %d", lAllocSize);
		AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);
#endif
		return NULL;
	}

	// 8을 붙이는 이유는 앞, 뒤로 가드바이트를 붙이기 위한것
	if ((m_lCurrentIndex + lAllocSize + 8) < m_lSize)
	{
		// 만약 할당되어야 하는 크기가 버퍼의 크기를 벗어나지 않으면
		m_lCurrentIndex += (lAllocSize + 8);

		*(DWORD*)(m_pBuffer + m_lCurrentIndex - lAllocSize - 8) = AUCIRCULAR_BUFFER_GUARD_BYTE;
		*(DWORD*)(m_pBuffer + m_lCurrentIndex - 4) = AUCIRCULAR_BUFFER_GUARD_BYTE;
		return (PVOID)(m_pBuffer + m_lCurrentIndex - (lAllocSize + 4));
	}
	else
	{
		// 만약 할당되어야 하는 크기가 버퍼의 크기를 벗어나면
		m_lCurrentIndex = lAllocSize + 8;

		*(DWORD*)(m_pBuffer + m_lCurrentIndex - lAllocSize - 8) = AUCIRCULAR_BUFFER_GUARD_BYTE;
		*(DWORD*)(m_pBuffer + m_lCurrentIndex - 4) = AUCIRCULAR_BUFFER_GUARD_BYTE;

		return (PVOID)((CHAR*)m_pBuffer + 4);
	}
}

void AuCircularBuffer::Free(PVOID pvPacket)
{
	ASSERT(pvPacket);

	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result()) return;

	PPACKET_HEADER pHeader = (PPACKET_HEADER)pvPacket;
	ASSERT(*(DWORD*)((CHAR*)pvPacket - 4) == AUCIRCULAR_BUFFER_GUARD_BYTE);
	ASSERT(*(DWORD*)((CHAR*)pvPacket + pHeader->unPacketLength) == AUCIRCULAR_BUFFER_GUARD_BYTE);
}