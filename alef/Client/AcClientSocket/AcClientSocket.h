#pragma once

#include <winsock2.h>
#include "AcQueue.h"
#include "ApModule.h"
#include "AuCircularBuffer.h"
#include "AuCrypt.h"

#pragma comment (lib, "ws2_32")

static LPSTR szClassName = "AcClientSocket";

const INT32 AC_ASYNCSELECT			= (WM_USER + 0x200);		// AsyncSelect socket을 위한 User define Window message
const INT32 MAX_PACKET_COUNT		= 10;
const INT32 MAX_CLIENT_BUFFER_SIZE	= APPACKET_MAX_PACKET_SIZE * MAX_PACKET_COUNT;

enum EnumCallback
{
	ACSOCKET_CALLBACK_CONNECT	 = 0,
	ACSOCKET_CALLBACK_DISCONNECT,
	ACSOCKET_CALLBACK_ERROR,
	ACSOCKET_CALLBACK_MAX,
};

struct CallbackFunc
{
	// pData		-> Socket Class Pointer
	// pCustData	-> OnError일 경우 Error Code 다른경우엔 사용하지 않음
	PVOID pClass;	// User에 의해 넘겨진값.. (User Class Pointer)
	ApModuleDefaultCallBack m_CallBackFunc[ACSOCKET_CALLBACK_MAX];

	CallbackFunc() : pClass(NULL)
	{
		::ZeroMemory(m_CallBackFunc, sizeof(m_CallBackFunc));
	}

	VOID CallFunction(EnumCallback eCallbackID, PVOID pData, PVOID pCustData)
	{
		// 등록된 함수 포인터가 있을 경우에만 호출
		if (m_CallBackFunc[eCallbackID])
			m_CallBackFunc[eCallbackID](pData, pClass, pCustData);
	}
};

enum ENUM_ACSOCKET_STATE
{
	ACSOCKET_STATE_NULL = 0,	//
	ACSOCKET_STATE_INITIALIZE,	// 초기화 진행
	ACSOCKET_STATE_CONNECTING,	// 연결 진행중
	ACSOCKET_STATE_CONNECTED,	// 연결 완료
	ACSOCKET_STATE_DISCONNECTED,// 연결 종료
	ACSOCKET_STATE_LISTENING,	// 리스닝 상태
	ACSOCKET_STATE_ERROR		// 에러 
};

class AcSocketManager;
class AcClientSocket
{
public:
	AcClientSocket();
	virtual ~AcClientSocket();

	VOID Create();
	VOID Close();
	BOOL Connect( LPCTSTR pszIPAddress, INT32 lPort );

	INT32 Send( AcPacketData *pPacketData, INT32 lLength );
	INT32 Recv();

	// FD Event에 의해 호출되는함수
	void OnRecv();
	void OnSend();
	void OnConnect();
	void OnAccept();
	void OnClose();
	void OnError(INT32 lErrorCode);

	VOID SetCallBack( PVOID pClass = NULL, ApModuleDefaultCallBack fpConnect = NULL,
					  ApModuleDefaultCallBack fpDisconnect = NULL, ApModuleDefaultCallBack fpError = NULL );
	
	VOID SetSocketManager( AcSocketManager *pcsSocketManager )	{	m_pcsSocketManager = pcsSocketManager;	}
	VOID SetMaxPacketBufferSize(INT32 lMaxPacketBufferSize)		{	m_AuCircularReceiveBuffer.Init(lMaxPacketBufferSize);	}
	BOOL SetDestroyNormal()										{	return (m_bIsDestroyNormal = TRUE);	}

	INT32	GetState()						{	return m_lSocketSatete;		}
	VOID	SetIndex(INT32 lIndex)			{	m_lIndex = lIndex;			}
	INT32	GetIndex()						{	return m_lIndex;			}

	AuCryptActor&	GetCryptActorRef()		{	return m_csCryptActor;		}
	AuCryptActor*	GetCryptActorPtr()		{	return &m_csCryptActor;		}

	UINT32	GetSendSeqID()					{	return m_ulSendSeqID;		}
	UINT32	GetRecvSeqID()					{	return m_ulRecvSeqID;		}
	void	SetSendSeqID( UINT32 ulNewID )	{	m_ulSendSeqID = ulNewID;	}
	void	SetRecvSeqID( UINT32 ulNewID )	{	m_ulRecvSeqID = ulNewID;	}

	static VOID NetworkInitialize(HINSTANCE hInstance = NULL);
	static VOID NetworkUnInitialize();
	static char* GetIPFromHost( LPCTSTR pstrHostName );

private:
	AcPacketData*	AllocPacket(INT32 lPacketSize);
	BOOL	CreateAsyncWindow();
	VOID	PushCompletePacket(INT32 lRecvLength);

private:
	CallbackFunc	m_stCallbackFunc;	// 콜백함수 설정 및 호출

	SOCKADDR_IN		m_stRemoteInfo;
	INT32			m_lSocketSatete;	// 소켓 상태 저장
	SOCKET			m_hSocket;			// 소켓 핸들
	HWND			m_hWindow;			// Async Socket을 위한 Window Handle
	INT32			m_lIndex;			// AcSocketManager에서 부여해주는 Index

	INT32			m_lBufferInOffset;							// m_BufferIn의 Index Offset
	BYTE			m_BufferIn[MAX_CLIENT_BUFFER_SIZE];			// 우선 이곳으로 패킷을 수신하여 완성된 패킷을
	// Queue로 넘긴다.

	INT32			m_lBufferOutOffset;							// m_BufferOut의 Index Offset
	BYTE			m_BufferOut[MAX_CLIENT_BUFFER_SIZE];		// Send할 데이터를 저장하는곳

	BOOL			m_bIsConnectedSocket;						// 연결이 되었었던 넘인지 여부
	BOOL			m_bIsReceivedPacket;						// 연결후 서버로부터 뭔가의 패킷을 받았는지 여부
	BOOL			m_bIsDestroyNormal;							// 정상 종료된건지 여부

	AcSocketManager *	m_pcsSocketManager;

	AuCircularBuffer	m_AuCircularReceiveBuffer;

	AuCryptActor	m_csCryptActor;		// 2006.04.11. steeple 암호화 관련 추가.
	UINT32			m_ulSendSeqID;
	UINT32			m_ulRecvSeqID;
};