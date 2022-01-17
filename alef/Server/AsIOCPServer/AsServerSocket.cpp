// AsServerSocket.cpp: implementation of the AsServerSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "AsServerSocket.h"
#include "AsIOCPServer.h"
#include "ApMemory.h"
#include "AuCircularBuffer.h"
#include "AuPacket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define ASSERVERSOCKET_ERROR_FILENAME "LOG\\AsServerSocket_Error.log"
#define ASSERVERSOCKET_PRIORITY_DESTROY "LOG\\AsServerSocket_PriorityDestroy.log"

AuCircularBuffer	g_AuCircularSendBuffer;

BOOL AsServerSocket::AsSendBuffer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	AsServerSocket	*pThis	= (AsServerSocket *) pClass;
	
	AuAutoLock lock(pThis->m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	if (pThis->m_WSABufferManager.bSetTimerEvent)
		pThis->m_WSABufferManager.bSetTimerEvent = FALSE;

	return pThis->SendBufferedPacket();
}

AsServerSocket::AsServerSocket() 
	: AsObject(ASOBJECT_SOCKET), m_BufferIn(NULL)
{
	m_lIndex	= INVALID_INDEX;
	m_pcsTimer	= NULL;
	m_csCriticalSection.Init();

	m_ovlOut.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovlOut.hEvent = UlongToHandle((HandleToUlong(m_ovlOut.hEvent) | 0x1));

	m_ulSendSeqID = m_ulRecvSeqID = 0;
	m_ulInvalidEncPacketCount = 0;

	m_BufferIn = new CHAR[MAX_BUFFER_SIZE+1];
	if( m_BufferIn )
		memset(m_BufferIn, 0, MAX_BUFFER_SIZE+1);

	Create();
}

AsServerSocket::~AsServerSocket()
{
	if (m_ovlOut.hEvent)
	{
		if ( FALSE == ::CloseHandle((HANDLE) ((DWORD_PTR)m_ovlOut.hEvent & ~0x1)) )
		{
			//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "CloseHandle Error : %d, Index : %d", GetLastError(), m_lIndex);
		}

		m_ovlOut.hEvent = NULL;
	}

	if(m_BufferIn)
		delete[] m_BufferIn;

	m_BufferIn = NULL;

	m_csCriticalSection.Destroy();
}

VOID AsServerSocket::SetIndex(INT32 lIndex)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (Lock.Result())
		m_lIndex = lIndex;
}

VOID AsServerSocket::InitData()
{
	m_hSocket	= INVALID_SOCKET;
	m_eState	= ASSERVER_STATE_INIT;
	m_eSendStatus = SEND_OVERLAPPED_STATUS_READY;

	m_lBufferInOffset	= 0;
	m_lProcessIndex		= 0;
//	m_lRemainDataSize	= 0;
//	m_lRemainOverlappedCount = 0;

	::ZeroMemory(&m_ovlIn.Internal, sizeof(OVERLAPPED));
	::ZeroMemory(&m_ovlOut.Internal, sizeof(OVERLAPPED));
	::ZeroMemory(&m_ovlSend.Internal, sizeof(OVERLAPPED));

	memset(m_BufferIn, 0, MAX_BUFFER_SIZE+1);
	::ZeroMemory(&m_stPlayerData, sizeof(PlayerData));

	m_bIsPCRoom	= FALSE;

	m_bCallback = TRUE;
	m_bIsProcessReuse	= FALSE;

	m_bSendReserved = FALSE;

	m_ulConnectedTimeMSec = 0;
	m_ulLastSendTimeStamp = 0;
	m_ulLastReceiveTimeStamp = 0;

	InitializeAll();

	SetSocketType(SOCKET_TYPE_ARCHLORD);
}

SOCKET AsServerSocket::GetHandle()
{
	return m_hSocket;
}

BOOL AsServerSocket::IsRead(OVERLAPPED *povl)
{
	if (&m_ovlIn == povl) return TRUE;
	else return FALSE;
}

BOOL AsServerSocket::IsActive()
{
	if (m_eState == ASSERVER_STATE_ALIVE)
		return TRUE;
	else return FALSE;
}

SOCKADDR_IN* AsServerSocket::GetRemoteInfo()
{
	return &m_stRemoteInfo;
}

char* AsServerSocket::GetRemoteIPAddress(char* szBuff, int nSize)
{
	if (nSize < MAX_IP_ADDRESS_STRING + 1)
	{
		return NULL;
	}

	sprintf(szBuff, "%d.%d.%d.%d", (unsigned long)m_stRemoteInfo.sin_addr.S_un.S_un_b.s_b1,
									(unsigned long)m_stRemoteInfo.sin_addr.S_un.S_un_b.s_b2,
									(unsigned long)m_stRemoteInfo.sin_addr.S_un.S_un_b.s_b3,
									(unsigned long)m_stRemoteInfo.sin_addr.S_un.S_un_b.s_b4);

	return szBuff;
}

BOOL AsServerSocket::Connect(LPCTSTR lpAddress, INT32 lPort)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return FALSE;

	m_stRemoteInfo.sin_family = AF_INET;
	m_stRemoteInfo.sin_addr.s_addr = inet_addr(lpAddress);
	m_stRemoteInfo.sin_port = htons(lPort);
#ifdef _DEBUG
	printf("::connect(%s %d)\n",lpAddress, lPort);
#endif
	INT32 lResult = ::connect(m_hSocket, (struct sockaddr*)&m_stRemoteInfo, sizeof(SOCKADDR_IN));
	//if (SOCKET_ERROR == lResult)
	if (0 != lResult)
	{
		int ierror = ::WSAGetLastError();
		printf("AsServerSocket::Connect %s:%d WSAGetLastError %d \n", lpAddress ,lPort, ierror);
		//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "AsServerSocket::Connect %d, bServer %d", dwError, m_stPlayerData.bServer);
		return FALSE;
	}

	m_stPlayerData.bServer	= TRUE;

	return TRUE;
}

// 2006.04.14. steeple
// const char* > CHAR* (암호화를 안에서 진행하기 때문에 상수성이 있으면 안된다.)
EnumAsyncSendResult AsServerSocket::AsyncSend(CHAR* pData, INT32 lLength, PACKET_PRIORITY ePriority)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return ASYNC_SEND_FAIL;

	if (!IsActive()) 
		return ASYNC_SEND_SOCKET_NOT_ACTIVE;

	if (SOCKET_TYPE_ARCHLORD != GetSocketType())
	{
		if (SendRawData((char *) pData, lLength))
			return ASYNC_SEND_SUCCESS;
		else
			return ASYNC_SEND_FAIL;
	}

	PPACKET_HEADER pHeader2 = (PPACKET_HEADER)pData;
	ASSERT ( pHeader2->unPacketLength < APPACKET_MAX_PACKET_SIZE);
	ASSERT ( pHeader2->unPacketLength > sizeof(PACKET_HEADER));
	ASSERT ( lLength > sizeof(PACKET_HEADER));

	if(!IS_ENC_PACKET(pData, lLength))
	{
		ASSERT ( pHeader2->bGuardByte == 0xD6 );
		ASSERT ( *(CHAR*)((CHAR*)pHeader2 + pHeader2->unPacketLength - 1) == 0x6B );
	}
	else
	{
		ASSERT ( *(BYTE*)pData == APENCPACKET_FRONT_PUBLIC_BYTE );
		ASSERT ( *((BYTE*)pData + pHeader2->unPacketLength - 1) == APENCPACKET_REAR_PUBLIC_BYTE );
	}
/*
	if (!m_stPlayerData.bServer)
	{
		AuAutoLock lock(m_csCriticalSection);
		if ((m_lRemainDataSize + lLength) > EMERGENCY_SOCKET_BUFFER_SIZE)
		{
			printf("!!!! 사이즈가 넘어가서 패킷을 무시한다.\n");
			return ASYNC_SEND_BUFFER_FULL;
		}
	}
*/
	if (m_stPlayerData.bServer)
	{
		if (TRUE == SendSystemData((CHAR *)pData, lLength))
			return ASYNC_SEND_SUCCESS;
		else
			return ASYNC_SEND_FAIL;
	}

	// 암호화를 해야하는데, 암호화된 패킷이 아니라면 해준다.
	// 이미 암호화된 패킷이라면 (멀티캐스팅 시) 하지 않는다.
	if(AuPacket::GetCryptManager().IsUseCrypt() && 
		m_csCryptActor.GetCryptType() != AUCRYPT_TYPE_NONE &&
		!IS_ENC_PACKET(pData, lLength))
	{
		// 서버에서 암호화는 Public 으로 한다.
		INT16 nOutputSize = 0;
		PVOID pOutput = AuPacket::EncryptPacket(pData, lLength, &nOutputSize, m_csCryptActor, AUCRYPT_TYPE_PUBLIC);

		(CHAR*)pData = (CHAR*)pOutput;
		lLength = (INT32)nOutputSize;

		// 암호화된 패킷이라면 SendSeqID 증가 후 세팅
		UINT32 ulSeqID = ++m_ulSendSeqID;
		if(m_ulSendSeqID == 0)		// 0이 되었다면 1로 바꾼다.
			ulSeqID = m_ulSendSeqID = 1;

		((PENC_PACKET_HEADER)pData)->ulSeqID = ulSeqID;
	}

	PPACKET_HEADER pHeader = (PPACKET_HEADER)pData;
	if (IS_ENC_PACKET(pData, lLength) || pHeader->lOwnerID == 0 || m_stPlayerData.lID == 0 || pHeader->lOwnerID == m_stPlayerData.lID)
	{
		EnumBufferingResult	eResult	= PushSendBuffer((CHAR *) pData, lLength, PACKET_PRIORITY_1);
		if(eResult != ASSERVER_BUFFERING_SUCCESS)
			return ASYNC_SEND_FAIL;

		AuAutoLock lock(m_csCriticalSection);
		if (lock.Result())
		{
			if (SEND_OVERLAPPED_STATUS_SEND == m_eSendStatus)
			{
				m_bSendReserved = TRUE;
				return ASYNC_SEND_SUCCESS;
			}

			EmptyBuffer();
	//		if (TRUE == SendUniData((CHAR *) pData, lLength))
	//			return ASYNC_SEND_SUCCESS;
	//		else
	//			return ASYNC_SEND_FAIL;
		}
	}
	else
	{
		EnumBufferingResult	eResult	= PushSendBuffer((CHAR *) pData, lLength, ePriority);

		switch (eResult) {
		case ASSERVER_BUFFERING_FULL:
		case ASSERVER_BUFFERING_NOT_USE:
			{
				if (TRUE == SendUniData((CHAR *) pData, lLength))
					return ASYNC_SEND_SUCCESS;
				else
					return ASYNC_SEND_FAIL;
			}
			break;

		case ASSERVER_BUFFERING_FAIL:
			{
				return ASYNC_SEND_FAIL;
			}
		}
	}

	return ASYNC_SEND_SUCCESS;
}

BOOL AsServerSocket::AsyncRecv()
{ 
	if (!IsActive()) return FALSE;
	
	DWORD dwBytesRead = 0;
	DWORD dwFlags = 0;
//	BOOL bResult = ::ReadFile((HANDLE)m_hSocket, &m_BufferIn[m_lProcessIndex + m_lBufferInOffset], 
//						MAX_BUFFER_SIZE - m_lProcessIndex - m_lBufferInOffset, &dwBytesRead, &m_ovlIn);
	m_wsaBufferIn.len = MAX_BUFFER_SIZE - m_lProcessIndex - m_lBufferInOffset;
	m_wsaBufferIn.buf = &m_BufferIn[m_lProcessIndex + m_lBufferInOffset];
	if (SOCKET_ERROR == ::WSARecv((SOCKET)m_hSocket, &m_wsaBufferIn, 1, &dwBytesRead, &dwFlags, &m_ovlIn, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
			//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "AsServerSocket::AsyncRecv %d, bServer %d", dwError, m_stPlayerData.bServer);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AsServerSocket::Create()
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return FALSE;

	InitData();

	m_ovlIn.pSocket = this;
	m_ovlOut.pSocket = this;
	m_ovlSend.pSocket = this;

	m_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if( m_hSocket == INVALID_SOCKET )
		return FALSE;

	linger lin;
	lin.l_onoff = 1;
	lin.l_linger = 0; /* hard shutdown */
	if( ::setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (const char*)&lin, sizeof(linger)) == -1 )
		return FALSE;

	struct timeval tv_timeo = { 3, 500000 };  /* 3.5 second */
	if( ::setsockopt( m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv_timeo, sizeof( tv_timeo ) ) == -1 )
		return FALSE;

	if( ::setsockopt( m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv_timeo, sizeof( tv_timeo ) ) == -1 )
		return FALSE;

	m_WSABufferManager.Initialize();

	return TRUE;
}

VOID AsServerSocket::Remove()
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return;

	if (INVALID_SOCKET == m_hSocket) return;

	if (SOCKET_ERROR == ::closesocket(m_hSocket))
	{
		//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "CloseHandle Error : %d, Index : %d", GetLastError(), m_lIndex);
	}

	m_hSocket = INVALID_SOCKET;
}

VOID AsServerSocket::GetRemoteAddress()
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return;

	// 1. 주소 분석 및 설정.
	LPSOCKADDR_IN lpRemoteAddr	= NULL;
	int	nRemoteSockAddrLen		= 0;

	::GetAcceptExSockaddrs((LPWSABUF)&m_BufferIn[0], 0, 0, sizeof(SOCKADDR_IN) + 16, NULL, NULL, 
							(LPSOCKADDR*)&lpRemoteAddr, &nRemoteSockAddrLen);

	if(NULL != lpRemoteAddr)
		m_stRemoteInfo = *lpRemoteAddr;
}

CHAR* AsServerSocket::GetCompletePacket(BOOL *pbIsInvalidPacket)
{
	ASSERT(SOCKET_TYPE_NONE != GetSocketType());

	switch (GetSocketType())
	{
	case SOCKET_TYPE_ARCHLORD:	return GetCompleteArchlordServerPacket(pbIsInvalidPacket);
	case SOCKET_TYPE_LKSERVER:	return GetCompleteLKServerPacket(pbIsInvalidPacket);
	case SOCKET_TYPE_BILLING:	return GetCompleteBillingServerPacket(pbIsInvalidPacket);
	case SOCKET_TYPE_PATCH:		return GetCompletePatchServerPacket(pbIsInvalidPacket);
	default :
		ASSERT(SOCKET_TYPE_NONE != GetSocketType());
	}

	return NULL;
}

CHAR* AsServerSocket::GetCompleteArchlordServerPacket(BOOL *pbIsInvalidPacket)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return NULL;

	if (!pbIsInvalidPacket)
		return NULL;

	CHAR* pResult = NULL;
	if (0 == m_lBufferInOffset) return NULL;

	if (MAX_BUFFER_SIZE - m_lProcessIndex < sizeof(PACKET_HEADER))
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// 버퍼의 크기가 부족하면 메모리를 이동시킨다.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}
	
	if (m_lBufferInOffset < sizeof(PACKET_HEADER))
		return NULL;

	UINT16 lPacketLength = ((PPACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->unPacketLength;

	if (lPacketLength <= sizeof(PACKET_HEADER) ||
		lPacketLength >= MAX_BUFFER_SIZE)
	{
		*pbIsInvalidPacket	= FALSE;
		return NULL;
	}

	// 하나의 패킷을 완성할수 있는 충분한 버퍼를 확보하고 있는지 확인
	if (lPacketLength >= MAX_BUFFER_SIZE - m_lProcessIndex)	
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// 버퍼의 크기가 부족하면 메모리를 이동시킨다.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}

	// 수신된 패킷의 길이를 확인하여 하나의 완전한 패킷이 수신 되었는지 확인한다.
	if (lPacketLength <= m_lBufferInOffset)
	{
		// 첫 바이트가 암호화된 놈이라면 암호화 바이트로 검사한다.
		if(AuPacket::GetCryptManager().IsUseCrypt() &&
			IS_ENC_FRONT_BYTE(*(BYTE*) &m_BufferIn[m_lProcessIndex]))
		{
			if (!IS_ENC_PACKET((BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength))
			{
				ASSERT(!"첫 바이트가 암호화 바이트 인데, 암호화 패킷이 아니삼");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				WriteInvalidPacketLog(INVALID_PACKET_TYPE_NOT_ENC_PACKET, (BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength);
				return NULL;
			}

			// 일렬번호 검사도 한다. 리플레이 공격을 방지한다.
			if (((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID != 1 &&
				((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID <= GetRecvSeqID())
			{
				ASSERT(!"암호화 패킷의 SeqID 가 또 들어왔삼. 리플레이 공격이 의심됨.");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				WriteInvalidPacketLog(INVALID_PACKET_TYPE_REPLAY_ATTACK, (BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength);
				return NULL;
			}
			else if(((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID == 1 &&
					GetRecvSeqID() == 1)
			{
				ASSERT(!"암호화 패킷의 SeqID 가 1이 계속 들어옴. 리플레이 공격이 의심됨.");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				WriteInvalidPacketLog(INVALID_PACKET_TYPE_REPLAY_ATTACK, (BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength);
				return NULL;
			}

			// 일렬번호가 1 로 들어오면 RecvSeqID 를 1로 세팅해준다.
			if (((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID == 1)
				SetRecvSeqID(1);

			// 아니면 들어온 값으로 세팅
			else
				SetRecvSeqID(((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID);

			// 복호화 한다.
			INT16 nOutputSize = 0;
			pResult = (CHAR*)AuPacket::DecryptPacket(&m_BufferIn[m_lProcessIndex], lPacketLength, &nOutputSize, GetCryptActorRef());
			if(!pResult)
			{
				*pbIsInvalidPacket	= FALSE;

				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - DecryptPacket fail", __FUNCTION__, __LINE__);
				AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);

				return NULL;
			}

			// 복호화한 패킷을 다시 확인한다.
			if (nOutputSize <= sizeof(PACKET_HEADER) ||
				nOutputSize >= MAX_BUFFER_SIZE)
			{
				//ASSERT(!"복호화 후, 패킷 사이즈가 우울함");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				//WriteInvalidPacketLog(INVALID_PACKET_TYPE_AFTER_DEC_SIZE_INVALID, (BYTE*)pResult, nOutputSize);
				
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - DecryptPacket fail (2)", __FUNCTION__, __LINE__);
				AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);

				return NULL;
			}

			// check guard byte
			if (((PPACKET_HEADER)pResult)->bGuardByte != APPACKET_FRONT_GUARD_BYTE ||
				*((BYTE*)pResult + nOutputSize - 1) != APPACKET_REAR_GUARD_BYTE)
			{
				//TRACE("복호화 후, 가드바이트 에러났음");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				//WriteInvalidPacketLog(INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR, (BYTE*)pResult, nOutputSize);

				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - DecryptPacket fail (3)", __FUNCTION__, __LINE__);
				AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);

				return NULL;
			}
		}
		else
		{
			// check guard byte
			if (((PPACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->bGuardByte != APPACKET_FRONT_GUARD_BYTE ||
				*(BYTE *) &m_BufferIn[m_lProcessIndex + lPacketLength - 1] != APPACKET_REAR_GUARD_BYTE)
			{
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				//WriteInvalidPacketLog(INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR, (BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength);
				return NULL;
			}

			pResult = &m_BufferIn[m_lProcessIndex];
		}

		m_lProcessIndex = m_lProcessIndex + lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}
	else
	{
		// 완전한 패킷이 아니면 다음 패킷을 기다린다.
		pResult = NULL;
	}

	return pResult;
}

CHAR* AsServerSocket::GetCompleteLKServerPacket(BOOL *pbIsInvalidPacket)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return NULL;

	if (!pbIsInvalidPacket)
		return NULL;

	CHAR* pResult = NULL;
	if (0 == m_lBufferInOffset) return NULL;

	// 헤더 모양이 다르다. 길이라던가,, 하는 부분에 대한 체크를 다르게 가져가야 한다.

	if (MAX_BUFFER_SIZE - m_lProcessIndex < sizeof(LK_PACKET_HEADER))
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// 버퍼의 크기가 부족하면 메모리를 이동시킨다.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}
	
	if (m_lBufferInOffset < sizeof(LK_PACKET_HEADER))
		return NULL;

	UINT16 lPacketLength = ::ntohs(((PLK_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->unLen) + sizeof(LK_PACKET_HEADER);

	if (lPacketLength < sizeof(LK_PACKET_HEADER) ||
		lPacketLength >= MAX_BUFFER_SIZE)
	{
		*pbIsInvalidPacket	= FALSE;
		return NULL;
	}

	// 하나의 패킷을 완성할수 있는 충분한 버퍼를 확보하고 있는지 확인
	if (lPacketLength >= MAX_BUFFER_SIZE - m_lProcessIndex)	
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// 버퍼의 크기가 부족하면 메모리를 이동시킨다.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}

	// 수신된 패킷의 길이를 확인하여 하나의 완전한 패킷이 수신 되었는지 확인한다.
	if (lPacketLength <= m_lBufferInOffset)
	{
		pResult = &m_BufferIn[m_lProcessIndex];
		m_lProcessIndex = m_lProcessIndex + lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}
	else
	{
		// 완전한 패킷이 아니면 다음 패킷을 기다린다.
		pResult = NULL;
	}

	return pResult;
}

CHAR* AsServerSocket::GetCompleteBillingServerPacket(BOOL *pbIsInvalidPacket)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return NULL;

	if (!pbIsInvalidPacket)
		return NULL;

	CHAR* pResult = NULL;
	if (0 == m_lBufferInOffset) return NULL;

	// Billing format
	// 2byte (return code)
	// 5byte (payload size - end flag 포함 size)
	// nbyte (payload) 
	// end flag (\n)

	if (MAX_BUFFER_SIZE - m_lProcessIndex < sizeof(BILLING_PACKET_HEADER))
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// 버퍼의 크기가 부족하면 메모리를 이동시킨다.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}
	
	if (m_lBufferInOffset < sizeof(BILLING_PACKET_HEADER))
		return NULL;


	UINT16 lPacketLength = ::atoi(((PBILLING_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->cLength) + 2;

	if (lPacketLength < sizeof(BILLING_PACKET_HEADER) ||
		lPacketLength >= MAX_BUFFER_SIZE)
	{
		*pbIsInvalidPacket	= FALSE;
		return NULL;
	}

	// 하나의 패킷을 완성할수 있는 충분한 버퍼를 확보하고 있는지 확인
	if (lPacketLength >= MAX_BUFFER_SIZE - m_lProcessIndex)	
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// 버퍼의 크기가 부족하면 메모리를 이동시킨다.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}

	// 수신된 패킷의 길이를 확인하여 하나의 완전한 패킷이 수신 되었는지 확인한다.
	if (lPacketLength <= m_lBufferInOffset)
	{
		pResult = &m_BufferIn[m_lProcessIndex];
		m_lProcessIndex = m_lProcessIndex + lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}
	else
	{
		// 완전한 패킷이 아니면 다음 패킷을 기다린다.
		pResult = NULL;
	}

	return pResult;
}

CHAR* AsServerSocket::GetCompletePatchServerPacket(BOOL *pbIsInvalidPacket)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return NULL;

	if (!pbIsInvalidPacket)
		return NULL;

	CHAR* pResult = NULL;
	if (0 == m_lBufferInOffset) return NULL;

	// Billing format
	// 2byte (return code)
	// 5byte (payload size - end flag 포함 size)
	// nbyte (payload) 
	// end flag (\n)

	if (MAX_BUFFER_SIZE - m_lProcessIndex < sizeof(PATCH_PACKET_HEADER))
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// 버퍼의 크기가 부족하면 메모리를 이동시킨다.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}
	
	if (m_lBufferInOffset < sizeof(PATCH_PACKET_HEADER))
		return NULL;


	UINT32 lPacketLength = ((PPATCH_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->m_iSize;

	if (lPacketLength < sizeof(PATCH_PACKET_HEADER) ||
		lPacketLength >= MAX_BUFFER_SIZE)
	{
		*pbIsInvalidPacket	= FALSE;
		return NULL;
	}

	// 하나의 패킷을 완성할수 있는 충분한 버퍼를 확보하고 있는지 확인
	if ((INT32)lPacketLength >= MAX_BUFFER_SIZE - m_lProcessIndex)	
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// 버퍼의 크기가 부족하면 메모리를 이동시킨다.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}

	// 수신된 패킷의 길이를 확인하여 하나의 완전한 패킷이 수신 되었는지 확인한다.
	if ((INT32)lPacketLength <= m_lBufferInOffset)
	{
		pResult = &m_BufferIn[m_lProcessIndex];
		m_lProcessIndex = m_lProcessIndex + lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}
	else
	{
		// 완전한 패킷이 아니면 다음 패킷을 기다린다.
		pResult = NULL;
	}

	return pResult;
}

EnumBufferingResult AsServerSocket::PushSendBuffer(CHAR *szPacket, INT32 nSize, PACKET_PRIORITY ePriority)
{
	if (!m_pcsTimer)
		return ASSERVER_BUFFERING_NOT_USE;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) 
		return ASSERVER_BUFFERING_FAIL;

	if (ADD_PACKET_RESULT_SUCCESS != m_WSABufferManager.AddPacket(szPacket, nSize, ePriority))
	{
#ifdef _DEBUG
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AsServerSocket::PushSendBuffer SocketIndex : %d, AccountName : %s", m_lIndex, m_stPlayerData.szAccountName);
		AuLogFile_s(ASSERVERSOCKET_PRIORITY_DESTROY, strCharBuff);
#endif

		return ASSERVER_BUFFERING_FAIL;
	}

	if (m_bSendReserved) 
		return ASSERVER_BUFFERING_SUCCESS;

	if (!m_WSABufferManager.bSetTimerEvent)
	{
		m_pcsTimer->AddTimer(200, m_lIndex, (PVOID) this, AsSendBuffer, NULL);
		m_WSABufferManager.bSetTimerEvent = TRUE;
	}

	return ASSERVER_BUFFERING_SUCCESS;
}

BOOL AsServerSocket::SendBufferedPacket()
{
	return EmptyBuffer();
}

BOOL AsServerSocket::EmptyBuffer()
{
	if (SEND_OVERLAPPED_STATUS_SEND == m_eSendStatus)
	{
		m_bSendReserved = TRUE;
		return TRUE;
	}

	if (m_WSABufferManager.GetTotalBufferSize() < 1)
		return TRUE;

	INT32 lSize = sizeof(WSABUF) * m_WSABufferManager.GetCurrentCount();
	PVOID pBuffer = g_AuCircularSendBuffer.Alloc(lSize);
	memcpy(pBuffer, m_WSABufferManager.GetStartBuffer(), lSize);
	SendMultiData((LPWSABUF)pBuffer, m_WSABufferManager.GetCurrentCount(), m_WSABufferManager.GetTotalBufferSize());

	m_WSABufferManager.BufferClear();
//	m_WSABufferManager.bSetTimerEvent = FALSE;

	return TRUE;
}

BOOL AsServerSocket::SendUniData(CHAR *pData, INT32 lSize)
{
	if (!pData || lSize < 1)
		return FALSE;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	// Send쪽 Overlapped 객체는 별도로 관리
	LPWSABUF lpBuf = (LPWSABUF)g_AuCircularSendBuffer.Alloc(sizeof(WSABUF));
	lpBuf->buf = pData;
	lpBuf->len = lSize;

	PPACKET_HEADER pHeader = (PPACKET_HEADER)pData;
	ASSERT ( pHeader->unPacketLength < APPACKET_MAX_PACKET_SIZE);
	ASSERT ( pHeader->unPacketLength > sizeof(PACKET_HEADER));

	DWORD dwBytesSent = 0;
//	AsOverlapped* pSendOverlapped = new AsOverlapped;
//	pSendOverlapped->pSocket = this;
//	InterlockedExchangeAdd(&m_lRemainDataSize, (LONG)lSize);
//	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, pSendOverlapped, NULL))
	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, &m_ovlOut, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
			//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "AsServerSocket::SendUniData %d, bServer %d", dwError, m_stPlayerData.bServer);
			// Send할수 없는 상황이면 DestroyClient시킨다.
//			delete pSendOverlapped;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AsServerSocket::SendSystemData(CHAR *pData, INT32 lSize)
{
	if (!pData || lSize < 1)
		return FALSE;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	// Send쪽 Overlapped 객체는 별도로 관리
	LPWSABUF lpBuf = (LPWSABUF)g_AuCircularSendBuffer.Alloc(sizeof(WSABUF));
	lpBuf->buf = pData;
	lpBuf->len = lSize;

	PPACKET_HEADER pHeader = (PPACKET_HEADER)pData;
	ASSERT ( pHeader->unPacketLength < APPACKET_MAX_PACKET_SIZE);
	ASSERT ( pHeader->unPacketLength > sizeof(PACKET_HEADER));

	DWORD dwBytesSent = 0;
//	AsOverlapped* pSendOverlapped = new AsOverlapped;
//	pSendOverlapped->pSocket = this;
//	InterlockedExchangeAdd(&m_lRemainDataSize, (LONG)lSize);
	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, &m_ovlOut, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
#ifdef _DEBUG
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AsServerSocket::SendSystemData %d, bServer %d", dwError, m_stPlayerData.bServer);
			AuLogFile_s(ASSERVERSOCKET_ERROR_FILENAME, strCharBuff);
#endif
			// Send할수 없는 상황이면 DestroyClient시킨다.
//			delete pSendOverlapped;
			return FALSE;
		}
	}
	return TRUE;
}

BOOL AsServerSocket::SendMultiData(LPWSABUF lpBuffers, INT32 lBufferCount, INT32 lSize)
{
	DWORD dwBytesSent = 0;
//	AsOverlapped* pSendOverlapped = new AsOverlapped;
//	pSendOverlapped->pSocket = this;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

#ifdef _DEBUG
	for (INT32 i = 0; i < lBufferCount; ++i)
	{
		PPACKET_HEADER pHeader = (PPACKET_HEADER)lpBuffers[i].buf;
		ASSERT ( pHeader->unPacketLength < APPACKET_MAX_PACKET_SIZE);
		ASSERT ( pHeader->unPacketLength > sizeof(PACKET_HEADER));
	}
#endif

//	InterlockedExchangeAdd(&m_lRemainDataSize, (LONG)lSize);
//	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuffers, lBufferCount, &dwBytesSent, 0, pSendOverlapped, NULL))
	m_eSendStatus = SEND_OVERLAPPED_STATUS_SEND;	
	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuffers, lBufferCount, &dwBytesSent, 0, &m_ovlSend, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
#ifdef _DEBUG
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AsServerSocket::SendMultiData %d, bServer %d", dwError, m_stPlayerData.bServer);
			AuLogFile_s(ASSERVERSOCKET_ERROR_FILENAME, strCharBuff);
#endif
			// Send할수 없는 상황이면 DestroyClient시킨다.
//			delete pSendOverlapped;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AsServerSocket::SendRawData(CHAR *pData, INT32 lSize)
{
	if (!pData || lSize < 1)
		return FALSE;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	// Send쪽 Overlapped 객체는 별도로 관리
	LPWSABUF lpBuf = (LPWSABUF)g_AuCircularSendBuffer.Alloc(sizeof(WSABUF));
	lpBuf->buf = pData;
	lpBuf->len = lSize;

	DWORD dwBytesSent = 0;
//	AsOverlapped* pSendOverlapped = new AsOverlapped;
//	pSendOverlapped->pSocket = this;
//	InterlockedExchangeAdd(&m_lRemainDataSize, (LONG)lSize);
//	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, pSendOverlapped, NULL))
	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, &m_ovlOut, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
			//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "AsServerSocket::SendUniData %d, bServer %d", dwError, m_stPlayerData.bServer);
			// Send할수 없는 상황이면 DestroyClient시킨다.
//			delete pSendOverlapped;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AsServerSocket::SetPCRoom()
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	m_bIsPCRoom	= TRUE;

	return TRUE;
}

BOOL AsServerSocket::IsPCRoom()
{
	return m_bIsPCRoom;
}

BOOL AsServerSocket::SetSocketType(EnumSocketType eType)
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	m_eSocketType	= eType;

	return TRUE;
}

EnumSocketType AsServerSocket::GetSocketType()
{
	return m_eSocketType;
}

//void AsServerSocket::SendCompletionSize(LONG lSize)
//{
//	InterlockedExchangeAdd(&m_lRemainDataSize, -lSize);
//}

VOID AsServerSocket::SendComplete()
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return;

	m_eSendStatus = SEND_OVERLAPPED_STATUS_READY;
	if (m_bSendReserved)
		EmptyBuffer();

	m_bSendReserved = FALSE;
}

// 2006.06.05. steeple
// 암호화 하는 지 안하는 지에 따라, 몇 번 틀렸느냐에 따라 보다 정확하게 결정.
//
// 2006.04.15. steeple
// 암호화 처리를 해야 되는 소켓인데 쓸데없는 패킷이 들어올 때.
// AuCryptManager::GetCryptCheckLevel 을 사용하여서 얼마 이상 들어오면 클라이언트를 짤라낼 지 결정한다.
BOOL AsServerSocket::CheckInvalidPacketCount()
{
	// 암호화를 하지 않아도 되는 상황일 때는 가드바이트 한번이라도 미스나면 짤라버린다.
	if(AuPacket::GetCryptManager().IsUseCrypt() == FALSE ||
		/*m_stPlayerData.bServer == TRUE ||*/  // 서버는 연결을 끊지 않음 - arycoat 2009.12.24
		m_csCryptActor.GetCryptType() == AUCRYPT_TYPE_NONE)
	{
		// 한번이라도 미스나면 짤라버린다.
		if(m_ulInvalidEncPacketCount > 0)
			return FALSE;

		return TRUE;
	}

	// Check Level 검사
	eAuCryptCheckLevel eCheckLevel = AuPacket::GetCryptManager().GetCryptCheckLevel();
	switch(eCheckLevel)
	{
		case AUCRYPT_CHECK_LEVEL_NONE:
			return TRUE;

		case AUCRYPT_CHECK_LEVEL_LOW:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_LOW ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_MIDDLE:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_MIDDLE ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_HIGH:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_HIGH ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_ULTRA:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_ULTRA ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_SUPER:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_SUPER ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_EXTREME:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_EXTREME ? TRUE : FALSE;
	}

	return TRUE;
}

void AsServerSocket::WriteInvalidPacketLog(EnumInvalidPacketType eType, BYTE* pPacket, INT32 lPacketLength)
{
	LPCTSTR szMessage = NULL;
	switch(eType)
	{
		case INVALID_PACKET_TYPE_NOT_ENC_PACKET:				szMessage = _T("Not Encrypted Packet");					break;
		case INVALID_PACKET_TYPE_REPLAY_ATTACK:					szMessage = _T("Replay Attack");						break;
		case INVALID_PACKET_TYPE_AFTER_DEC_SIZE_INVALID:		szMessage = _T("After Decrypt, Packet Size Invalid");	break;
		case INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR:	szMessage = _T("After Decrypt, Guard Byte Error");		break;
		case INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR:	szMessage = _T("Non-Enc Mode. Guard Byte Error");		break;
		case INVALID_PACKET_TYPE_CHECK_LIMIT_EXCEED:			szMessage = _T("Check Limit Exceed");					break;
		default:												szMessage = _T("Unknown Error");						break;
	}

	// 어떤 공유도 하지 않는다.
	// 즉, 하나의 쓰레드가 잡고 있으면 다른 쓰레드는 Write 하지 못한다.
	HANDLE hFile = CreateFile(AUCRYPT_MANAGER_INVALID_PACKET_LOG_FILENAME,
								GENERIC_READ | GENERIC_WRITE, 0, NULL,
								OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(hFile, 0, 0, FILE_END);		// 얘 없어서 계속 덮어쓰고 있었다. -_-;

	CHAR pBuffer[512];
	sprintf(pBuffer, "Account : %s, %s", m_stPlayerData.szAccountName, szMessage);
	DWORD dwWritten = 0;
	WriteFile(hFile, pBuffer, (DWORD)strlen(pBuffer), &dwWritten, NULL);

	memset(pBuffer, 0, sizeof(pBuffer));
	if(eType == INVALID_PACKET_TYPE_NOT_ENC_PACKET || eType == INVALID_PACKET_TYPE_REPLAY_ATTACK)
	{
		sprintf(pBuffer, "\r\n Dump : ");
		for(size_t i = 0; i < sizeof(ENC_PACKET_HEADER); ++i)
		{
			if(pPacket && pPacket[i])
				sprintf(pBuffer, "0x%02X ", pPacket[i]);
			else
				sprintf(pBuffer, "0x   ");
		}
	}
	else if(eType == INVALID_PACKET_TYPE_AFTER_DEC_SIZE_INVALID ||
			eType == INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR ||
			eType == INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR)
	{
		sprintf(pBuffer, "\r\n Dump : ");
		for(size_t i = 0; i < sizeof(PACKET_HEADER); ++i)
		{
			if(pPacket && pPacket[i])
				sprintf(pBuffer, "0x%02X ", pPacket[i]);
			else
				sprintf(pBuffer, "0x   ");
		}

		if(eType == INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR || eType == INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR)
		{
			if(pPacket && pPacket[lPacketLength - 1])
			{
				sprintf(pBuffer, "\r\n        ");	// \r\n next 7 spaces
				sprintf(pBuffer, "Index[lPacketLength - 1] : 0x%02X", pPacket[lPacketLength - 1]); 
			}
		}
	}
	// eType == INVALID_PACKET_TYPE_CHECK_LIMIT_EXCEED 이 경우는 pPacket 이 NULL 이기 때문에 Dump 뜨지 않는다.

	sprintf(pBuffer, "\r\n");
	WriteFile(hFile, pBuffer, (DWORD)strlen(pBuffer), &dwWritten, NULL);

	CloseHandle(hFile);
}
