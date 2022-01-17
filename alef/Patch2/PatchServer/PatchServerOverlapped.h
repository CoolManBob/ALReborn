#ifndef __OVERLAPPED_H__
#define __OVERLAPPED_H__

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <windows.h>

//IOCP : Overlapped Operation
#define		IOCP_OVERLAP_NONE			0
#define		IOCP_OVERLAP_RECV			1
#define		IOCP_OVERLAP_SEND			2
#define		IOCP_OVERLAP_ACCEPT			4
#define		IOCP_OVERLAP_PATCH_FILE		5

class CAcceptOverlappedEx
{
public:
	OVERLAPPED		m_cOverlapped;
	SOCKET			m_hSocket;
	char			*m_pstrAddrInfo;

	CAcceptOverlappedEx() { init(); }
	~CAcceptOverlappedEx() { cleanup(); }
	void init();
	void recycleOverlapped();
	void cleanup();
};

class COverlappedEx
{
public:
	OVERLAPPED			m_cOverlapped;
	int				m_iOperation;

	bool init( bool bEvent, int iOperation );
	bool setOperation( int iOperation );
	bool cleanup();
	void recycleOverlapped();
};

class COverlappedRecv : public COverlappedEx
{
public:
	WSABUF			m_cWSABuf;

	//Send/Recv시 짤리거나 뭉쳐오는 패킷을 위한 버퍼.
	char			*m_pstrBuffer;
	int				m_iRemainPacketLength;

	COverlappedRecv();
	COverlappedRecv( bool bEvent, int iWSABufSize, int iInternalBufferSize );
	~COverlappedRecv();

	bool init( bool bEvent, int iOperation, int iWSABufSize , int iInternalBufferSize );
	bool cleanup();

};

class COverlappedSend : public COverlappedEx
{
public:
	WSABUF			m_cWSABuf;

	COverlappedSend();
	COverlappedSend( bool bEvent, int iWSABufSize );
	~COverlappedSend();

	bool init( bool bEvent, int iOperation, int iWSABufSize );
	bool cleanup();
};

class COverlappedSendPatchFile : public COverlappedEx
{
public:
	WSABUF			m_cWSABuf;

	COverlappedSendPatchFile();
	COverlappedSendPatchFile( bool bEvent, int iWSABufSize );
	~COverlappedSendPatchFile();

	bool init( bool bEvent, int iOperation, int iWSABufSize );
	bool cleanup();
};

#endif