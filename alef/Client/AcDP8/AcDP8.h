/******************************************************************************
Module:  AcDP8.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 12. 09
******************************************************************************/

#if !defined(__ACDP8_MODULE_H__)
#define __ACDP8_MODULE_H__

#include "ApBase.h"
#include <dplay8.h>
#include <dpaddr.h>
#include <tchar.h>

#include "ApPacket.h"


#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcDP8D" )
#else
#pragma comment ( lib , "AcDP8" )
#endif
#endif

#pragma comment ( lib , "dplay" )
#pragma comment ( lib , "dxguid" )
#pragma comment ( lib , "dxerr8" )


typedef enum	_eAcDPServerType {
	ACDP_SERVER_TYPE_GAMESERVER		= 0,
	ACDP_SERVER_TYPE_LOGINSERVER,
	ACDP_SERVER_TYPE_DEALSERVER,
	ACDP_SERVER_TYPE_RECRUITSERVER,
	ACDP_SERVER_TYPE_AUCTIONSERVER
} eAcDPServerType;


typedef enum	_eAcDPSendType {
	ACDP_SEND_GAMESERVERS			= 0,
	ACDP_SEND_SERVER,
	ACDP_SEND_LOGINSERVER,
	ACDP_SEND_DEALSERVER,
	ACDP_SEND_RECRUITSERVER,
	ACDP_SEND_AUCTIONSERVER
} eAcDPSendType;


#define ACDP_CONNECT_TIMEOUT		3000	// milisecond
#define ACDP_CONNECT_RETRIES		3
#define	ACDP_TIMEOUT_KEEPALIVE		3000	// milisecond

#define ACDP_MAX_DPCLIENT_INSTANCE	500		// DP Instance를 만들 수 있는 최대값

#define ACDP_CONNECT_ERR			(-1)	// connect error

#define	DISCONNNECT_REASON_CLIENT_NORMAL		2
#define	DISCONNNECT_REASON_CLIENT_OUT_OF_DATE	1

//-----------------------------------------------------------------------------
// Miscellaneous helper functions
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

typedef struct _ACU_QUEUE_DATA {
	ULONG	ulBufferSize;
	CHAR	cBufferData[APPACKET_MAX_PACKET_SIZE];
	UINT32	ulDPNID;
} ACU_QUEUE_DATA;

//	AcuQueueFIFO class
//		- FIFO queue
///////////////////////////////////////////////////////////////////////////////
class AcuQueueFIFO : public ApBase {
private:
	INT16		m_nQueueSize;

	ACU_QUEUE_DATA*	m_pQueueData;

	INT16		m_nNumData;

	INT16		m_nHead;
	INT16		m_nTail;

public:
	AcuQueueFIFO();
	~AcuQueueFIFO();

	BOOL Initialize(INT16 nQueueSize);

	INT16 Push(PVOID pBuffer, ULONG ulDataSize, UINT32 ulDPNID);
	ULONG Pop(PVOID pBuffer);
	INT16 Reset();
};

//	AcDP8 class
//		- DX8 DirectPlay client class
///////////////////////////////////////////////////////////////////////////////
class AcDP8 {
private:
	TCHAR			m_szServerAddr[23];

	BOOL			m_bDPConnected;
	DPNHANDLE		m_hEnumAsyncOp;
	DPNHANDLE		m_hConnectAsyncOp;

	HANDLE			m_hConnectCompleteEvent;

	HRESULT			m_hrConnectComplete;

	VOID DXUtil_ConvertGenericStringToWide(WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar);
	VOID DXUtil_ConvertAnsiStringToWide(WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar);

	INT32			m_lSelfCID;

	HANDLE			m_hCleanUpThread;
	unsigned long	m_ulCleanUpThreadID;

public:
	AcuQueueFIFO	m_queueRecv;

	AcDP8();
	~AcDP8();

	BOOL	StartDP();
	BOOL	StopDP();

	INT16	Open(CHAR *pszServerAddr, INT16 nServerType);
	BOOL	Close(INT16 nServerID);

	INT16	Send(CHAR* pData, INT16 nDataSize, INT16 nSendType = ACDP_SEND_GAMESERVERS, INT16 nServerID = 0);

	static HRESULT WINAPI DirectPlayMessageHandler(PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer);

	BOOL	IsStartDP();

	BOOL	SetSelfCID(INT32 lCID);

	BOOL	m_bStartDP;

	INT16	nCleanUpInstance;
	HANDLE	m_hCleanUpEvent;
	
	static DWORD WINAPI CleanUpThread(PVOID pvParam);
};

#endif //__ACDP8_MODULE_H__
