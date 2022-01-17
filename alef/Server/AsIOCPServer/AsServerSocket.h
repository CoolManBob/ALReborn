/*
	Notices: Copyright (c) NHN Studio 2003
	Created by: Bryan Jeong (2003/12/24)
 */

// AsServerSocket.h: interface for the AsServerSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASSERVERSOCKET_H__EA08A8C4_82E5_4EE7_9FE2_50716BC15986__INCLUDED_)
#define AFX_ASSERVERSOCKET_H__EA08A8C4_82E5_4EE7_9FE2_50716BC15986__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include "AuCryptManager.h"
#include <WINSOCK2.H>
#include "AsDefine.h"
#include <mswsock.h>
#include "AsObject.h"
#include "AsTimer.h"
#include "AsWSABufManager.h"

#pragma comment (lib, "mswsock")

const INT32 MAX_BUFFER_SIZE			= APPACKET_MAX_PACKET_SIZE;
const INT32 MAX_SEND_BUFFER_SIZE	= APPACKET_MAX_PACKET_SIZE;
const INT32 MAX_IP_ADDRESS_STRING	= 15;

enum EnumSocketState
{
	ASSERVER_STATE_INIT = 0,		// 초기화중
	ASSERVER_STATE_ALIVE,			// 소켓이 사용중
	ASSERVER_STATE_ACCEPT,			// 새로운 접속 기다리는중
	ASSERVER_STATE_DEAD,			// 소켓이 죽었음 -> Accept로 만들어야 됨
	ASSERVER_STATE_RESERVED_CLOSE,	// 소켓을 종료함.
};

enum EnumBufferingResult
{
	ASSERVER_BUFFERING_SUCCESS	= 0,
	ASSERVER_BUFFERING_FULL,
	ASSERVER_BUFFERING_FAIL,
	ASSERVER_BUFFERING_NOT_USE,
};

enum EnumSendOverlappedStatus
{
	SEND_OVERLAPPED_STATUS_NONE	= 0,
	SEND_OVERLAPPED_STATUS_READY,
	SEND_OVERLAPPED_STATUS_SEND
};

enum EnumAsyncSendResult
{
	ASYNC_SEND_SUCCESS		= 0,
	ASYNC_SEND_BUFFER_FULL,
	ASYNC_SEND_SOCKET_NOT_ACTIVE,
	ASYNC_SEND_FAIL
};

enum EnumSocketType
{
	SOCKET_TYPE_NONE	= 0,
	SOCKET_TYPE_ARCHLORD,	// Archlord 
	SOCKET_TYPE_LKSERVER,	// MediaWeb
	SOCKET_TYPE_BILLING,	// Billing
	SOCKET_TYPE_PATCH,		// Patch
};

enum EnumInvalidPacketType
{
	INVALID_PACKET_TYPE_NOT_ENC_PACKET = 1,
	INVALID_PACKET_TYPE_REPLAY_ATTACK,
	INVALID_PACKET_TYPE_AFTER_DEC_SIZE_INVALID,
	INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR,
	INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR,
	INVALID_PACKET_TYPE_CHECK_LIMIT_EXCEED,
	INVALID_PACKET_TYPE_MAX,
};

typedef struct _PlayerData
{
	BOOL		bActive;				// active data 인지 여부

	INT32		lID;					// ID (CID or ServerID)
	BOOL		bCheckValidation;
	CHAR		szAccountName[32+1];	// account name
	BOOL		bServer;				// 서버와의 연결인지 여부
	INT32		lCustomData;
	INT32		lDisconnectReason;
} PlayerData, *pPlayerData;

//typedef struct _SendBuffer
//{
//	BOOL		bIsUseSendBuffer;
//
//	CHAR		*szBuffer;
//	INT32		lBufferedSize;
//
//	BOOL		bSetTimerEvent;
//
//} SendBuffer, *pSendBuffer;

class AsServerSocket;
class AsOverlapped : public OVERLAPPED//, public ApMemory<AsOverlapped, 1000000>
{
public:
	AsServerSocket *pSocket;
	AsOverlapped() : pSocket(NULL)
	{
		Internal		= 0;
		InternalHigh	= 0;
		Offset			= 0;
		OffsetHigh		= 0;
		hEvent			= NULL;
	}
};
/*
class AsSendOverlapped : public OVERLAPPED, public ApMemory<AsOverlapped, 100000>
{
public:
	AsServerSocket *pSocket;
};
*/
// 메모리 풀링을 사용하지 않음
// 왜냐하면 AsServerSocketManager에서 모든 소켓을 생성시켜놓고 
// 재활용 하는 방식을 취하기 때문
class AsServerSocket : public AsObject
{
private:
	SOCKET			m_hSocket;
	AsOverlapped	m_ovlIn;
	AsOverlapped	m_ovlOut;
	AsOverlapped	m_ovlSend;			// Send용 Overlapped
	PlayerData		m_stPlayerData;
	SOCKADDR_IN		m_stRemoteInfo;

	BOOL			m_bSendReserved;
	EnumSendOverlappedStatus	m_eSendStatus;
	CHAR*			m_BufferIn;
	WSABUF			m_wsaBufferIn;

	INT32			m_lBufferInOffset;
	INT32			m_lProcessIndex;
	INT32			m_lIndex;		// AsSocketManager에서 부여해준 ID;

	ApCriticalSection	m_csCriticalSection;
	AsWSABufManager	m_WSABufferManager;

	AsTimer*		m_pcsTimer;

	EnumSocketState	m_eState;
	BOOL			m_bIsPCRoom;

	EnumSocketType	m_eSocketType;

	AuCryptActor	m_csCryptActor;	// 2006.04.11. steeple
	UINT32			m_ulSendSeqID;
	UINT32			m_ulRecvSeqID;

	UINT32			m_ulInvalidEncPacketCount;

public:
	UINT32			m_ulConnectedTimeMSec;
	UINT32			m_ulLastSendTimeStamp;
	UINT32			m_ulLastReceiveTimeStamp;

//	LONG			m_lRemainDataSize;
//	LONG			m_lRemainOverlappedCount;

public:
	// Disconnect 콜백을 안받기 위해 추가함.
	BOOL			m_bCallback;
	BOOL			m_bIsProcessReuse;

private:
	VOID InitData();

private:
	EnumBufferingResult	PushSendBuffer(CHAR *szPacket, INT32 nSize, PACKET_PRIORITY ePriority);
	BOOL				EmptyBuffer();

	BOOL				SendUniData(CHAR *pData, INT32 lSize);
	BOOL				SendMultiData(LPWSABUF lpBuffers, INT32 lBufferCount, INT32 lSize);
	BOOL				SendSystemData(CHAR *pData, INT32 lSize);
	BOOL				SendRawData(CHAR *pData, INT32 lSize);

	// Socket type 
	CHAR* GetCompleteArchlordServerPacket(BOOL *pbIsInvalidPacket);
	CHAR* GetCompleteBillingServerPacket(BOOL *pbIsInvalidPacket);
	CHAR* GetCompleteLKServerPacket(BOOL *pbIsInvalidPacket);
	CHAR* GetCompletePatchServerPacket(BOOL *pbIsInvalidPacket);

public:
	BOOL				SetPCRoom();
	BOOL				IsPCRoom();

	BOOL				SetSocketType(EnumSocketType eType);
	EnumSocketType		GetSocketType();

	BOOL				SendBufferedPacket();

	static BOOL AsSendBuffer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);
	
public:
	AsServerSocket();
	virtual ~AsServerSocket();

	BOOL Create();
	VOID Remove();
	
	SOCKADDR_IN* GetRemoteInfo();
	char* GetRemoteIPAddress(char* szBuff, int nSize);

	SOCKET GetHandle();
	BOOL IsRead(OVERLAPPED *povl);
	AsOverlapped* GetOutOverlapped() 
	{
		return &m_ovlOut;
	}
	BOOL IsActive();

	VOID SetIndex(INT32 lIndex);
	INT32 GetIndex() {return m_lIndex;}

	VOID SetTimer(AsTimer *pcsTimer) 
	{ 
		AuAutoLock Lock(m_csCriticalSection);
		if (Lock.Result())
			m_pcsTimer = pcsTimer; 
	}

	VOID SetReadBytes(INT32 lReadBytes) 
	{
		AuAutoLock Lock(m_csCriticalSection);
		if (Lock.Result())
			m_lBufferInOffset += lReadBytes;
	}

	PlayerData* GetPlayerData() {return &m_stPlayerData;}

	VOID SetState(EnumSocketState eState) 
	{
		AuAutoLock Lock(m_csCriticalSection);
		if (Lock.Result())
			m_eState = eState;
	}

	EnumSocketState GetState() {return m_eState;}

	BOOL Connect(LPCTSTR lpAddress, INT32 lPort);
	EnumAsyncSendResult AsyncSend(CHAR* pData, INT32 lLength, PACKET_PRIORITY ePriority);
	
	BOOL AsyncRecv();
	VOID GetRemoteAddress();

	CHAR* GetCompletePacket(BOOL *pbIsInvalidPacket);

	VOID SendComplete();

	void	InitializeAll()
	{
		GetCryptActorRef().Initialize();
		SetSendSeqID(0);
		SetRecvSeqID(0);
		SetInvalidEncPacketCount(0);
	}

	AuCryptActor& GetCryptActorRef() { return m_csCryptActor; }
	AuCryptActor* GetCryptActorPtr() { return &m_csCryptActor; }

	UINT32	GetSendSeqID() { return m_ulSendSeqID; }
	UINT32	GetRecvSeqID() { return m_ulRecvSeqID; }
	void	SetSendSeqID(UINT32 ulNewID) { m_ulSendSeqID = ulNewID; }
	void	SetRecvSeqID(UINT32 ulNewID) { m_ulRecvSeqID = ulNewID; }
	void	SetInvalidEncPacketCount(UINT32 lCount) { m_ulInvalidEncPacketCount = lCount; }
	UINT32	IncreasetInvalidEncPacketCount() { return ++m_ulInvalidEncPacketCount; }
	BOOL	CheckInvalidPacketCount();

	void	WriteInvalidPacketLog(EnumInvalidPacketType eType, BYTE* pPacket, INT32 lPacketLength);

//	void SendCompletionSize(LONG lSize);

	friend class AsListener;
};

#endif // !defined(AFX_ASSERVERSOCKET_H__EA08A8C4_82E5_4EE7_9FE2_50716BC15986__INCLUDED_)
