#include "PatchServerIOCP.h"
#include "ApServerMonitorPacket.h"

CSocketIOContext::CSocketIOContext()
{
	init( 0, false, 0, false );
}

CSocketIOContext::CSocketIOContext(SOCKET hSocket, int iSendBufferSize, bool bSendEvent, int iRecvBufferSize, bool bRecvEvent )
{
	m_hSocket = hSocket;

	init( iSendBufferSize, bSendEvent, iRecvBufferSize, bRecvEvent );
}

CSocketIOContext::~CSocketIOContext()
{
	cleanupContext();
}

void CSocketIOContext::init( int iSendBufferSize, bool bSendEvent, int iRecvBufferSize, bool bRecvEvent )
{
	InitializeCriticalSection( &m_cSendCriticalSection );
	InitializeCriticalSection( &m_cRecvCriticalSection );

	m_pcPatchFileInfo = NULL;
	m_pcPatchFileInfoNode = NULL;
}

void CSocketIOContext::SendLock()
{
	EnterCriticalSection( &m_cSendCriticalSection );
}

void CSocketIOContext::SendUnlock()
{
	LeaveCriticalSection( &m_cSendCriticalSection );
}

void CSocketIOContext::RecvLock()
{
	EnterCriticalSection( &m_cRecvCriticalSection );
}

void CSocketIOContext::RecvUnlock()
{
	LeaveCriticalSection( &m_cRecvCriticalSection );
}

void CSocketIOContext::cleanupContext()
{
	SendLock();
	RecvLock();

	if( m_pcPatchFileInfo != NULL )
	{
		delete m_pcPatchFileInfo;
		m_pcPatchFileInfo = NULL;
	}

/*	if( m_pcPatchFileInfo != NULL )
	{
		m_pcPatchFileInfo->Destroy();
		m_pcPatchFileInfo = NULL;
	}*/

	m_pcPatchFileInfo = NULL;
	m_pcPatchFileInfoNode = NULL;

	if( m_hSocket != NULL )
	{
		closesocket( m_hSocket );
		m_hSocket = NULL;
	}

	SendUnlock();
	RecvUnlock();

	DeleteCriticalSection( &m_cSendCriticalSection );
	DeleteCriticalSection( &m_cRecvCriticalSection );
}


bool CIOCP::CreateIOCP( int iThreadNum )
{
	bool		bResult;
	HANDLE		hTempHandle;

	hTempHandle = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, iThreadNum );

	if( hTempHandle != NULL )
	{
		m_hIOCP = hTempHandle;
		bResult = true;
	}
	else
	{
		bResult = false;
	}

	return bResult;
}

bool CIOCP::AssociateIOCP( HANDLE hAssociateHandle, void *pvKey )
{
	bool		bResult;
	HANDLE		hTempHandle;

	//printf( "(DWORD)pvKey : %d\n", (DWORD)pvKey );

	hTempHandle = CreateIoCompletionPort( hAssociateHandle, m_hIOCP, (DWORD)pvKey, 0 );

	if( hTempHandle != NULL )
	{
		m_hIOCP = hTempHandle;
		bResult = true;
	}
	else
	{
		bResult = false;
	}

	return bResult;
}

bool CIOCP::CleanupIOCP()
{
	CloseHandle( m_hIOCP );

	return true;
}

CLockMgr::CLockMgr()
{
	Init();
}

CLockMgr::~CLockMgr()
{
	CleanUp();
}

void CLockMgr::Init()
{
	m_csContextTree.InitializeObject( sizeof(int), 10000 );
	InitializeCriticalSection( &m_cCriticalSection );
}

void CLockMgr::CleanUp()
{
	DeleteCriticalSection( &m_cCriticalSection );
}

void CLockMgr::Lock()
{
	EnterCriticalSection( &m_cCriticalSection );
}

void CLockMgr::Unlock()
{
	LeaveCriticalSection( &m_cCriticalSection );
}

bool CLockMgr::AddObject( void *pvData, int iKey )
{
	bool			bResult;

	bResult = false;

	Lock();

	if(	m_csContextTree.AddObject( pvData, iKey ) )
	{
		bResult = true;
	}

	Unlock();

	return bResult;
}

bool CLockMgr::RemoveObject( int iKey )
{
	bool			bResult;

	bResult = false;

	Lock();

	if(	m_csContextTree.RemoveObject( iKey ) )
	{
		bResult = true;
	}

	Unlock();

	return bResult;
}

void *CLockMgr::GetObject( int iKey )
{
	void			*pvResult;

	pvResult = NULL;

	Lock();

	pvResult = m_csContextTree.GetObject( iKey );

	Unlock();

	return pvResult;
}

CIOCPNetworkModule::CIOCPNetworkModule()
{
	m_bIPBlockingCheck = false;
	m_iStatus = IOCP_OP_ACTIVE;
	m_iCurrentUsers = 0;

	m_hSendQueueEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hRecvQueueEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	//Patch정보를 읽는다.
	m_cAuPackingManager.LoadResFile( "Patch.res" );
	m_iVersion = m_cAuPackingManager.GetLastVersion();	

	m_ulClientFileCRC = _GetClientFileCRC( "root", "AlefClient.exe" );
}

CIOCPNetworkModule::~CIOCPNetworkModule()
{
	m_cSendQueue.removeAll();
	m_cRecvQueue.removeAll();

	CloseHandle( m_hSendQueueEvent );
	CloseHandle( m_hRecvQueueEvent );
}

char *CIOCPNetworkModule::GetDownloadServerIP()
{
	return m_strServerIP;	
}

unsigned short CIOCPNetworkModule::GetDownloadServerPort()
{
	return m_iServerPort;	
}

bool CIOCPNetworkModule::SetDownloadServer( char *pstrServerIP, unsigned short iServerPort )
{
	memset( m_strServerIP, 0, sizeof(m_strServerIP) );
	strcat( m_strServerIP, pstrServerIP );

	m_iServerPort = iServerPort;

	return true;
}

void CIOCPNetworkModule::SetIPBlocking( bool bUseIPBlocking )
{
	m_bIPBlockingCheck = bUseIPBlocking;
}

bool CIOCPNetworkModule::LoadPatchCode( char *pstrFileName )
{
	return m_cAuPatchCheckCode.LoadPatchCode( pstrFileName );
}

int CIOCPNetworkModule::GetPatchFiles( int iVersion, unsigned int iPatchCheckCode, CDList<CJZPFileInfo *> *pcList )
{
	int				iResult;

	//현재 버전과 체크코드가 맞나본다.
	if( m_cAuPatchCheckCode.GetCode( iVersion ) == iPatchCheckCode )
	{
		//iVersion값은 현재까지 패치된 버전값. 다음 버전버전부터 받을게 있는지 검색하기 위해서는 1을 증가시킨다.
		iVersion++;

		if( iVersion > m_iVersion )
		{
			iResult = g_dErrorNoPatchFile;
		}
		else
		{
			//어쨌든, 여기로 왔으면 뭔가를 보낸다.
			iResult = 0;

			for( int i=iVersion; i<=m_iVersion; i++ )
			{
				CJZPFileInfo		*pcJZPFileInfo;

				if( (pcJZPFileInfo = m_cAuPackingManager.GetJZPInfo(i) ) != NULL )
				{
					int				iVersion;
					int				iFileNameSize;
					char			*pstrFileName;
					int				iFileSize;
					int				iRawFileSize;
					unsigned long	iCRCCheckSum;

					//버전복사.
					iVersion = pcJZPFileInfo->m_iVersion;
					//File이름크기 복사.
					iFileNameSize = pcJZPFileInfo->m_iFileNameSize;
					//파일이름복사
					pstrFileName = new char[iFileNameSize];
					memcpy( pstrFileName, pcJZPFileInfo->m_pstrFileName, iFileNameSize );
					//FileSize복사
					iFileSize = pcJZPFileInfo->m_iFileSize;
					//iRawFileSize복사
					iRawFileSize = pcJZPFileInfo->m_iRawFileSize;
					//CRC CheckSum복사
					iCRCCheckSum = pcJZPFileInfo->m_iCRCCheckSum;

					pcJZPFileInfo = new CJZPFileInfo( iVersion, iFileNameSize, pstrFileName, iFileSize, iRawFileSize, iCRCCheckSum );

					pcList->Add( pcJZPFileInfo );
				}
			}
		}
	}
	else
	{
		iResult = g_dErrorInvalidPatchCode;
	}

	return iResult;
}

int	CIOCPNetworkModule::postAccept()
{
	unsigned long	iReadBytes;
	unsigned int	iError;

	iReadBytes = 0;
	iError = 0;

	//Overlap구조체를 초기화.
	m_AcceptOverlappedEx.recycleOverlapped();

	//소켓을 만든다.
	m_AcceptOverlappedEx.m_hSocket = WSASocket(AF_INET, SOCK_STREAM,0, NULL, 0, WSA_FLAG_OVERLAPPED );

	//AcceptEx를 실행한다.
	if(!AcceptEx( m_cWinSockLib.m_hSocket,
				m_AcceptOverlappedEx.m_hSocket,
				m_AcceptOverlappedEx.m_pstrAddrInfo,
				0, // read nothing from the socket
				0,
				sizeof(struct sockaddr_in) +16,
				&iReadBytes,
				&m_AcceptOverlappedEx.m_cOverlapped ))
	{
		iError=WSAGetLastError();

		if(iError!=ERROR_IO_PENDING){
			fprintf(stderr,"acceptex failed %d\n",iError);
			return -1;
		}
	}

	return true;
}

int CIOCPNetworkModule::postRecv( CSocketIOContext *pContext, COverlappedRecv *pcOverlappedRecv )
{
	int				iResult;
	unsigned long	lNumOfRecvBytes;
	unsigned long	lFlag = 0;

	//printf( "PostRecv Overlapped : %p\n", &pContext->m_cRecvOverlapped.m_cOverlapped );

	//동시에 여러개의 Thread에서 같은 pContext가 인자로 온다면? Race Condition이다.
	//따라서 동기화!!
	EnterCriticalSection( &pContext->m_cRecvCriticalSection );

	iResult = WSARecv( pContext->m_hSocket, &pcOverlappedRecv->m_cWSABuf, 1, &lNumOfRecvBytes, &lFlag, &pcOverlappedRecv->m_cOverlapped, NULL );

	LeaveCriticalSection( &pContext->m_cRecvCriticalSection );

	if( iResult == SOCKET_ERROR)
    {
		int			iError;

		iError = WSAGetLastError();

        if( iError != WSA_IO_PENDING)
        {
            return -1;
        }
    }

	return 1;
}

int CIOCPNetworkModule::sendPacket( CSocketIOContext *pContext, char *pstrPacket, int iPacketSize )
{
	int				iResult;
	unsigned long	lNumOfSendBytes;
	unsigned long	lFlag = 0;

	COverlappedSend		*pcOverlappedSend;
	pcOverlappedSend = new COverlappedSend( true, 0 );

	//동시에 여러개의 Thread에서 같은 pContext가 인자로 온다면? Race Condition이다.
	//따라서 동기화!!
	EnterCriticalSection( &pContext->m_cSendCriticalSection );

	pcOverlappedSend->m_cWSABuf.buf = pstrPacket;
	pcOverlappedSend->m_cWSABuf.len = iPacketSize;

	iResult = WSASend( pContext->m_hSocket, &pcOverlappedSend->m_cWSABuf, 1, &lNumOfSendBytes, lFlag, &pcOverlappedSend->m_cOverlapped, NULL );

	LeaveCriticalSection( &pContext->m_cSendCriticalSection );

	if( iResult == SOCKET_ERROR)
    {
		int iError;

		iError = WSAGetLastError();

        if( iError != WSA_IO_PENDING)
        {
            return -1;
        }
		else
		{
			printf( "iError:%d\n", iError );
		}
    }

	return 1;
}

int CIOCPNetworkModule::sendPatchFileInfo( CSocketIOContext *pContext )
{
	pContext->SendLock();

	//보낼수 있는 파일이 있다면 보낸다.
	if( pContext->m_pcPatchFileInfoNode != NULL )
	{
		char			*pstrPacket;

		int				iPacketSize;
		int				iCurrentPos;
		int				iFileNameSize;
		int				iFileSize;
		int				iResult;

		unsigned long	lNumOfSendBytes;
		unsigned long	lFlag = 0;

		COverlappedSendPatchFile		*pcOverlappedSendPatchFile;
		pcOverlappedSendPatchFile = new COverlappedSendPatchFile( true, 0 );

		//패킷크기를 시뮬레이션.
		CPacketTypeSC_0x02	cPacketTypeSC_0x02;

		iPacketSize = 0;
		iCurrentPos = 0;

		//File이름크기를 복사한다.
		iFileNameSize = pContext->m_pcPatchFileInfoNode->m_tData->m_iFileNameSize;
		//FileSize를 복사한다.
		iFileSize = pContext->m_pcPatchFileInfoNode->m_tData->m_iFileSize;

		//패킷 헤더.
		iPacketSize += sizeof( cPacketTypeSC_0x02.m_iSize );
		iPacketSize += sizeof( cPacketTypeSC_0x02.m_iCommand );
		//파일이름 크기.
		iPacketSize += sizeof(iFileNameSize);
		//파일이름
		iPacketSize += iFileNameSize;
		//파일크기의 정보를 추가.
		iPacketSize += sizeof( iFileSize );
		//CRCCheckSum을 추가.
		iPacketSize += sizeof( pContext->m_pcPatchFileInfoNode->m_tData->m_iCRCCheckSum );

		//패킷 사이즈를 넣는다.
		cPacketTypeSC_0x02.m_iSize = iPacketSize;

		//파일을 읽어서 패킷을 만든다.
		pstrPacket = new char[iPacketSize];

		//패킷헤더복사.
		memcpy( &pstrPacket[iCurrentPos], &cPacketTypeSC_0x02.m_iSize, sizeof(cPacketTypeSC_0x02.m_iSize) );
		iCurrentPos+=sizeof(cPacketTypeSC_0x02.m_iSize);
		memcpy( &pstrPacket[iCurrentPos], &cPacketTypeSC_0x02.m_iCommand, sizeof(cPacketTypeSC_0x02.m_iCommand) );
		iCurrentPos+=sizeof(cPacketTypeSC_0x02.m_iCommand);
		//파일이름크기복사.
		memcpy( &pstrPacket[iCurrentPos], &iFileNameSize, sizeof(iFileNameSize) );
		iCurrentPos+=sizeof(iFileNameSize);
		//파일이름복사.
		memcpy( &pstrPacket[iCurrentPos], pContext->m_pcPatchFileInfoNode->m_tData->m_pstrFileName, iFileNameSize );
		iCurrentPos+=iFileNameSize;
		//파일크기복사.
		memcpy( &pstrPacket[iCurrentPos], &iFileSize, sizeof(iFileSize) );
		iCurrentPos+=sizeof(iFileSize);
		//CRC CheckSum복사
		memcpy( &pstrPacket[iCurrentPos], &pContext->m_pcPatchFileInfoNode->m_tData->m_iCRCCheckSum, sizeof(pContext->m_pcPatchFileInfoNode->m_tData->m_iCRCCheckSum) );
		iCurrentPos+=sizeof(pContext->m_pcPatchFileInfoNode->m_tData->m_iCRCCheckSum);

		//동시에 여러개의 Thread에서 같은 pContext가 인자로 온다면? Race Condition이다.
		//따라서 동기화!!
		EnterCriticalSection( &pContext->m_cSendCriticalSection );

		pcOverlappedSendPatchFile->m_cWSABuf.buf = pstrPacket;
		pcOverlappedSendPatchFile->m_cWSABuf.len = iPacketSize;

		iResult = WSASend( pContext->m_hSocket, &pcOverlappedSendPatchFile->m_cWSABuf, 1, &lNumOfSendBytes, lFlag, &pcOverlappedSendPatchFile->m_cOverlapped, NULL );

		LeaveCriticalSection( &pContext->m_cSendCriticalSection );

		if( iResult == SOCKET_ERROR)
		{
			int iError;

			iError = WSAGetLastError();

			if( iError != WSA_IO_PENDING)
			{
				iResult = -1;
			}
			else
			{
				iResult = 1;
			}
		}
		else
		{
			iResult = 1;
		}

		//다음으로 넘긴다.
		pContext->m_pcPatchFileInfoNode = pContext->m_pcPatchFileInfoNode->m_pcNextNode;
	}

	pContext->SendUnlock();

	return 0;
}

int CIOCPNetworkModule::cleanup()
{
	BOOL			bResult;
	unsigned int	iReturn;
		
	bResult = PostQueuedCompletionStatus(m_hIOCP, 0, IOCP_OP_DESTROY, NULL);

	if( bResult == FALSE )
	{
		iReturn = GetLastError();
	}
	else
	{
		iReturn = 0;
	}

	return iReturn;
}

int CIOCPNetworkModule::parsePacket( CSocketIOContext *pcContext, COverlappedRecv *pcOverlappedRecv, int iTransferedBytes )
{
	int				iPacketSize;
	int				iRemainPacketLength;
	int				iPacketHeaderSize;
	int				iOffset;

	iOffset = 0;
	iPacketHeaderSize = 5;
	iRemainPacketLength = pcOverlappedRecv->m_iRemainPacketLength;

	while( 1 )
	{
		if( (iRemainPacketLength + iTransferedBytes) >= iPacketHeaderSize )
		{
			//아무일 없다.
		}
		else
		{
			if( iTransferedBytes == 0 )
			{
				//정확히 패킷이 파싱되어 끝났다.
				pcOverlappedRecv->m_iRemainPacketLength = 0;
			}
			else
			{
				//문제가 있는경우. 이럴땐 접속을 해제함이 마땅합니다요~ 호홋~
			}

			break;
		}

		//여기까지 왔다면 패킷사이즈와타입은 최소한 알 수 있다는 뜻이다. 오호호호~~
		//패킷의 크기를 얻어내보자~
		if( iRemainPacketLength == 0 )
		{
			memcpy( &iPacketSize, &pcOverlappedRecv->m_cWSABuf.buf[iOffset], sizeof( iPacketSize) );
		}
		else
		{
			memcpy( &iPacketSize, pcOverlappedRecv->m_pstrBuffer, sizeof( iPacketSize) );
		}

		//iPacketSize를 검사하는 구문이 필요한다.

		//만약 하나의 패킷 단위를 완성할 수 있다면?
		if( (iRemainPacketLength + iTransferedBytes) >= iPacketSize )
		{
			char			*pstrPacket;
			int				iCopySize;

			iCopySize = iPacketSize-iRemainPacketLength;

			//패킷을 Queue에 넣는다.
			pstrPacket = new char[iPacketSize];

			//Packet의 실제 내용을 복사해낸다.
			if( iRemainPacketLength == 0 )
			{
				//printf( "iPacketSize:%d", iPacketSize );

				//받은 버퍼에서 바로 복사해 버린다.
				memcpy( pstrPacket, &pcOverlappedRecv->m_cWSABuf.buf[iOffset], iPacketSize );
			}
			else
			{
				//printf( "iPacketSize:%d", iPacketSize );

				//이전에 받던게있으니 앞쪽은 m_strBuffer에서 복사하고 나머지는 m_cWSABuf.buf에서 복사한다.
				memcpy( pstrPacket, pcOverlappedRecv->m_pstrBuffer, iRemainPacketLength );
				memcpy( &pstrPacket[iRemainPacketLength], &pcOverlappedRecv->m_cWSABuf.buf[iOffset], iCopySize );

				delete [] pcOverlappedRecv->m_pstrBuffer;
				pcOverlappedRecv->m_pstrBuffer = NULL;
				iRemainPacketLength = 0;				
			}

			//Packet을 받았으니 RecvQueue에 넣는다.
			CQueueUnit	*pcQueueUnit;
			pcQueueUnit = new CQueueUnit;
			pcQueueUnit->m_pcContext = pcContext;
			pcQueueUnit->m_pstrPacket = pstrPacket;
			pcQueueUnit->m_hSocket = pcContext->m_hSocket;
			pcQueueUnit->m_iPacketSize = iPacketSize;

			m_cRecvQueue.push( pcQueueUnit );

			if( SetEvent( m_hRecvQueueEvent ) == FALSE )
			{
				//puts( "[Debug] Set Event Failed!!\n" );
			}
			else
			{
				//puts( "[Debug] Signal for m_hRecvQueueEvent.\n" );
			}

			iRemainPacketLength = 0;
			iOffset += iCopySize;
			iTransferedBytes -= iCopySize;
		}
		//하나의 패킷 단위를 완성하기엔 받은 패킷이 작다면?
		else
		{
			//그냥 남은대로 복사해버린다. 그럼 다음에 알아서 처리된다~ 오호호호~
			if( pcOverlappedRecv->m_pstrBuffer == NULL )
				pcOverlappedRecv->m_pstrBuffer = new char[iPacketSize];

			memcpy( &pcOverlappedRecv->m_pstrBuffer[iRemainPacketLength], &pcOverlappedRecv->m_cWSABuf.buf[iOffset], iTransferedBytes );
			pcOverlappedRecv->m_iRemainPacketLength = iRemainPacketLength + iTransferedBytes;

			break;
		}
	}

	return 1;
}

bool CIOCPNetworkModule::registerThread( unsigned int ( CIOCPNetworkModule::*pf )( void * ) )
{
	m_cRegisteredThreadLlist.push( pf );
	return true;
}

void CIOCPNetworkModule::beginRegisteredThread()
{
	int			iRegistedThreads;

	iRegistedThreads = m_cRegisteredThreadLlist.getCount();

	for( int i=0; i<iRegistedThreads; i++ )
	{
		unsigned int	iThreadID;

		_beginthreadex(NULL,
			0,
			(PBEGINTHREADEX_THREADFUNC)CIOCPNetworkModule::executeRegistedThread,
			(LPVOID)this,
			0,
			(PBEGINTHREADEX_THREADID)&iThreadID );
	}

	return;
}

DWORD CIOCPNetworkModule::executeRegistedThread( LPVOID param )
{
	CIOCPNetworkModule	*pcIOCP;
	pcIOCP = (CIOCPNetworkModule *)param;

	return (pcIOCP->*(pcIOCP->m_cRegisteredThreadLlist.pop()))(NULL);
}

unsigned int CIOCPNetworkModule::IOCPGQCSThread( void *pArg )
{
	printf( "Begin IOCPGQCSThread.\n" );

	CSocketIOContext	*pcContext = NULL;
	CAcceptOverlappedEx *pcAcceptOverlappedEx = NULL;
	COverlappedEx		*pcOverlappedEx = NULL;
	COverlappedRecv		*pcOverlappedRecv = NULL;
	COverlappedSend		*pcOverlappedSend = NULL;
	OVERLAPPED			*pcOverlapped =NULL;

	BOOL			bResult;
	unsigned long	iTransferedBytes;

	while( 1 )
	{
		bResult = GetQueuedCompletionStatus( m_hIOCP, &iTransferedBytes, (DWORD *)&pcContext, &pcOverlapped, INFINITE );

		if( bResult == FALSE )
		{
			if( pcOverlapped == NULL )
			{
				//printf( "Fatal Error. Illegal call to GQCS.\n" );
			}
			else//if( lpOverlapped != NULL )
			{
				//2003.12.2 Accept하다가 뻑난경우. 그냥 무시하자.
//				if( (unsigned int)pcContext == IOCP_ACCEPTEX )
//				{
//					;
//				}
//				else
//				{
				if( (unsigned int)pcContext == IOCP_ACCEPTEX )
				{
					printf("IOCP_ACCEPTEX ");
				}
					//Close Connection.
					printf( "Abnormal Closing!!\n" );
					m_iCurrentUsers--;
					printf( "Remove user : %d\n", m_iCurrentUsers );

					m_cLockMgr.Lock();
					m_cLockMgr.RemoveObject( (int) pcContext->m_hSocket );
					delete pcContext;
					m_cLockMgr.Unlock();

					pcOverlappedEx = (COverlappedEx *)pcOverlapped;

					if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_RECV )
					{
						pcOverlappedRecv = (COverlappedRecv *)pcOverlappedEx;
						delete pcOverlappedRecv;
					}
					else if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_SEND )
					{
						pcOverlappedSend = (COverlappedSend *)pcOverlappedEx;
						delete pcOverlappedSend;
					}
//				}
			}
		}
		else //if( bResult == TRUE )
		{
			if( iTransferedBytes == 0 )
			{
				if( (unsigned int)pcContext == IOCP_OP_DESTROY )
				{
					//IOCP 상태를 Destroy로 만든다.
					m_iStatus = IOCP_OP_DESTROY;

					//Send,Recv Thread를 종료하기위해 Signal을준다.
					SetEvent( m_hSendQueueEvent );
					SetEvent( m_hRecvQueueEvent );
					break;
				}
				else if( (unsigned int)pcContext == IOCP_ACCEPTEX )
				{
					SOCKET			hConnectedSocket;
					sockaddr_in		*lpRemoteAddr;
					int				nRemoteSockAddrLen;

					nRemoteSockAddrLen = 0;

					pcAcceptOverlappedEx = (CAcceptOverlappedEx *)pcOverlapped;

					GetAcceptExSockaddrs((LPWSABUF)pcAcceptOverlappedEx->m_pstrAddrInfo, 
										0, 
										0, 
										sizeof(SOCKADDR_IN) + 16, 
										NULL, 
										NULL, 
										(LPSOCKADDR*)&lpRemoteAddr, 
										&nRemoteSockAddrLen);

					printf( "Accept Client Connection.\n" );

					if( lpRemoteAddr != NULL )
					{
						BOOL			bBlockIP;
						unsigned long	ulIP;

						bBlockIP = false;
						ulIP = 0;

						printf( "Client IP : %s\n", inet_ntoa( lpRemoteAddr->sin_addr ) );

						ulIP = lpRemoteAddr->sin_addr.S_un.S_addr;

/*						if( m_bIPBlockingCheck )
						{
							bBlockIP = m_cIPBlockingManager.SearchIP( ulIP );
						}

						//블럭킹된 IP인지 본다.
						if( bBlockIP )*/
						{
							hConnectedSocket = pcAcceptOverlappedEx->m_hSocket;

							CSocketIOContext	*pSocketIOContext;
							pSocketIOContext = new CSocketIOContext( hConnectedSocket, 0, false, 4096, true );

							//Associate with IOCP Port
							AssociateIOCP( (HANDLE)hConnectedSocket, (void *)pSocketIOContext );
							//연결을 맺은 소켓으로부터 들어오는 패킷을 감시한다.

							m_cLockMgr.AddObject( pSocketIOContext, (int) hConnectedSocket );

							//새로 OverlappedRecv를 만들어준다.
							COverlappedRecv		*pcOverlappedRecv;
							pcOverlappedRecv = new COverlappedRecv( true, 4096, 0 );

							postRecv( pSocketIOContext, pcOverlappedRecv );

							m_iCurrentUsers++;
							printf( "Add user : %d\n", m_iCurrentUsers );
						}
/*						else
						{
							printf( "불량IP 짤림\n" );
							closesocket( pcAcceptOverlappedEx->m_hSocket );
						}*/
					}

					//또 Accept를 한다.
					postAccept();
				}
				else
				{
					//Close Connection.
					printf( "Close a connection......\n" );
					m_iCurrentUsers--;
					printf( "Remove user : %d\n", m_iCurrentUsers );

					m_cLockMgr.Lock();
					m_cLockMgr.RemoveObject( (int) pcContext->m_hSocket );
					delete pcContext;
					m_cLockMgr.Unlock();

					pcOverlappedEx = (COverlappedEx *)pcOverlapped;

					if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_RECV )
					{
						pcOverlappedRecv = (COverlappedRecv *)pcOverlappedEx;
						delete pcOverlappedRecv;
					}
					else if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_SEND )
					{
						pcOverlappedSend = (COverlappedSend *)pcOverlappedEx;
						delete pcOverlappedSend;
					}
				}
			}
			//뭔가 잘 받거나 보냈다. 아싸조쿠나!!
			else
			{
				pcOverlappedEx = (COverlappedEx *)pcOverlapped;

				if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_RECV )
				{
					//printf( "(%d)Byte Recv succeeded!!\n", iTransferedBytes );
					pcOverlappedRecv = (COverlappedRecv *)pcOverlappedEx;

					//받은 패킷을 Parsing한다.
					parsePacket( pcContext, pcOverlappedRecv, iTransferedBytes );
					
					//데이터를 받은뒤엔 Overlapped구조체를 꼭 초기화(재활용)해준다. 별표 땡야~
					pcOverlappedRecv->recycleOverlapped();
					postRecv( pcContext, pcOverlappedRecv );
				}
				else if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_SEND )
				{
					//printf( "(%d)Byte Send succeeded!!\n", iTransferedBytes );
					pcOverlappedSend = (COverlappedSend *)pcOverlappedEx;

					//패킷을 보냈으니 해당 버퍼를 없애고 길이를 0으로 초기화한다.
					delete pcOverlappedSend;
				}
				else if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_PATCH_FILE )
				{
					//printf( "[PatchFile] (%d)bytes Send\n", iTransferedBytes );

					//보낸 파일에 대한 정보를 지운다.
					COverlappedSendPatchFile  *pcOverlappedSendPatchFile;
					pcOverlappedSendPatchFile = (COverlappedSendPatchFile *)pcOverlappedEx;

					delete pcOverlappedSendPatchFile;
				}
			}
		}
	}

	printf( "End IOCPGQCSThread.\n" );

	return 0;
}

unsigned int CIOCPNetworkModule::IOCPGQCSThreadSM( void *pArg )
{
	printf( "Begin IOCPGQCSThread.\n" );

	CSocketIOContext	*pcContext = NULL;
	CAcceptOverlappedEx *pcAcceptOverlappedEx = NULL;
	COverlappedEx		*pcOverlappedEx = NULL;
	COverlappedRecv		*pcOverlappedRecv = NULL;
	COverlappedSend		*pcOverlappedSend = NULL;
	OVERLAPPED			*pcOverlapped =NULL;

	BOOL			bResult;
	unsigned long	iTransferedBytes;

	InitLocalIPAddr();

	while( 1 )
	{
		bResult = GetQueuedCompletionStatus( m_hIOCP, &iTransferedBytes, (DWORD *)&pcContext, &pcOverlapped, INFINITE );

		if( bResult == FALSE )
		{
			if( pcOverlapped == NULL )
			{
				//printf( "Fatal Error. Illegal call to GQCS.\n" );
			}
			else//if( lpOverlapped != NULL )
			{
				//2003.12.2 Accept하다가 뻑난경우. 그냥 무시하자.
//				if( (unsigned int)pcContext == IOCP_ACCEPTEX )
//				{
//					;
//				}
//				else
//				{
				if( (unsigned int)pcContext == IOCP_ACCEPTEX )
				{
					printf("IOCP_ACCEPTEX (SM)");
				}
					//Close Connection.
					printf( "Abnormal Closing (SM) !!\n" );
					m_iCurrentUsers--;
					printf( "Remove SM\n" );

					m_cLockMgr.Lock();
					m_cLockMgr.RemoveObject( (int) pcContext->m_hSocket );
					delete pcContext;
					m_cLockMgr.Unlock();

					pcOverlappedEx = (COverlappedEx *)pcOverlapped;

					if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_RECV )
					{
						pcOverlappedRecv = (COverlappedRecv *)pcOverlappedEx;
						delete pcOverlappedRecv;
					}
					else if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_SEND )
					{
						pcOverlappedSend = (COverlappedSend *)pcOverlappedEx;
						delete pcOverlappedSend;
					}
//				}
			}
		}
		else //if( bResult == TRUE )
		{
			if( iTransferedBytes == 0 )
			{
				if( (unsigned int)pcContext == IOCP_OP_DESTROY )
				{
					//IOCP 상태를 Destroy로 만든다.
					m_iStatus = IOCP_OP_DESTROY;

					//Send,Recv Thread를 종료하기위해 Signal을준다.
					SetEvent( m_hSendQueueEvent );
					SetEvent( m_hRecvQueueEvent );
					break;
				}
				else if( (unsigned int)pcContext == IOCP_ACCEPTEX )
				{
					SOCKET			hConnectedSocket;
					sockaddr_in		*lpRemoteAddr;
					int				nRemoteSockAddrLen;

					nRemoteSockAddrLen = 0;

					pcAcceptOverlappedEx = (CAcceptOverlappedEx *)pcOverlapped;

					GetAcceptExSockaddrs((LPWSABUF)pcAcceptOverlappedEx->m_pstrAddrInfo, 
										0, 
										0, 
										sizeof(SOCKADDR_IN) + 16, 
										NULL, 
										NULL, 
										(LPSOCKADDR*)&lpRemoteAddr, 
										&nRemoteSockAddrLen);

					printf( "Accept SM Connection.\n" );

					if( lpRemoteAddr != NULL )
					{
						BOOL			bBlockIP;
						unsigned long	ulIP;

						bBlockIP = false;
						ulIP = 0;

						printf( "Client SM : %s\n", inet_ntoa( lpRemoteAddr->sin_addr ) );

						ulIP = lpRemoteAddr->sin_addr.S_un.S_addr;

						hConnectedSocket = pcAcceptOverlappedEx->m_hSocket;

						CSocketIOContext	*pSocketIOContext;
						pSocketIOContext = new CSocketIOContext( hConnectedSocket, 0, false, 4096, true );

						//Associate with IOCP Port
						AssociateIOCP( (HANDLE)hConnectedSocket, (void *)pSocketIOContext );
						//연결을 맺은 소켓으로부터 들어오는 패킷을 감시한다.

						m_cLockMgr.AddObject( pSocketIOContext, (int) hConnectedSocket );

						//새로 OverlappedRecv를 만들어준다.
						COverlappedRecv		*pcOverlappedRecv;
						pcOverlappedRecv = new COverlappedRecv( true, 4096, 0 );

						postRecv( pSocketIOContext, pcOverlappedRecv );

						m_iCurrentUsers++;
						printf( "Add SM \n" );

						{
							ApServerMonitorPacket	csPacket;

							int		lServerType	= ASSM_SERVER_TYPE_PATCH_SERVER;
							int		lServerStatus = 1;
							short	nPacketLength;
							void *	pvPacket;
							//DWORD	dwSendBytes;

							pvPacket = csPacket.m_csApsmServerInfoPacket.MakeStaticPacket(TRUE, &nPacketLength, ASSM_PACKET_SERVER_INFO,
							/*  1 */		&lServerType,
							/*  2 */		NULL,
							/*  3 */		NULL,				// Group 은 비우기로 했음.
							/*  4 */		m_szLocalAddr[m_nNumAddr-1],
							/*  5 */		m_szLocalAddr[m_nNumAddr-1],
							/*  6 */		&lServerStatus,
							/*  7 */		&m_iCurrentUsers,
							/*  8 */		NULL,
							/*  9 */		NULL,
							/* 10 */		NULL,
							/* 11 */		NULL,
							/* 12 */		NULL,
							/* 13 */		NULL,
							/* 14 */		&m_iVersion,
							/* 15 */		NULL,
							/* 16 */		NULL,
							/* 17 */		NULL,
							/* 18 */		NULL,
							/* 19 */		NULL,
							/* 20 */		NULL,
							/* 21 */		NULL,
							/* 22 */		NULL
									);

							sendPacket( pSocketIOContext, (char *) pvPacket, nPacketLength );
						}
					}

					//또 Accept를 한다.
					postAccept();
				}
				else
				{
					//Close Connection.
					printf( "Close a SM connection......\n" );
					m_iCurrentUsers--;
					printf( "Remove SM\n" );

					m_cLockMgr.Lock();
					m_cLockMgr.RemoveObject( (int) pcContext->m_hSocket );
					delete pcContext;
					m_cLockMgr.Unlock();

					pcOverlappedEx = (COverlappedEx *)pcOverlapped;

					if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_RECV )
					{
						pcOverlappedRecv = (COverlappedRecv *)pcOverlappedEx;
						delete pcOverlappedRecv;
					}
					else if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_SEND )
					{
						pcOverlappedSend = (COverlappedSend *)pcOverlappedEx;
						delete pcOverlappedSend;
					}
				}
			}
			//뭔가 잘 받거나 보냈다. 아싸조쿠나!!
			else
			{
				pcOverlappedEx = (COverlappedEx *)pcOverlapped;

				if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_RECV )
				{
					//printf( "(%d)Byte Recv succeeded!!\n", iTransferedBytes );
					pcOverlappedRecv = (COverlappedRecv *)pcOverlappedEx;

					//받은 패킷을 Parsing한다.
					parsePacket( pcContext, pcOverlappedRecv, iTransferedBytes );
					
					//데이터를 받은뒤엔 Overlapped구조체를 꼭 초기화(재활용)해준다. 별표 땡야~
					pcOverlappedRecv->recycleOverlapped();
					postRecv( pcContext, pcOverlappedRecv );
				}
				else if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_SEND )
				{
					//printf( "(%d)Byte Send succeeded!!\n", iTransferedBytes );
					pcOverlappedSend = (COverlappedSend *)pcOverlappedEx;

					//패킷을 보냈으니 해당 버퍼를 없애고 길이를 0으로 초기화한다.
					delete pcOverlappedSend;
				}
				else if( pcOverlappedEx->m_iOperation == IOCP_OVERLAP_PATCH_FILE )
				{
					//printf( "[PatchFile] (%d)bytes Send\n", iTransferedBytes );

					//보낸 파일에 대한 정보를 지운다.
					COverlappedSendPatchFile  *pcOverlappedSendPatchFile;
					pcOverlappedSendPatchFile = (COverlappedSendPatchFile *)pcOverlappedEx;

					delete pcOverlappedSendPatchFile;
				}
			}
		}
	}

	printf( "End IOCPGQCSThread.\n" );

	return 0;
}

unsigned int CIOCPNetworkModule::IOCPSendThread( void *pArg )
{
	printf( "Begin IOCPSendThread.\n" );

	CQueueUnit		*pcQueueUnit;

	while( 1 )
	{
		if( WaitForSingleObject( m_hSendQueueEvent, INFINITE ) == WAIT_FAILED )
		{
			//printf( "Error:%d\n", GetLastError() );
			getchar();
		}
		else
		{
			if( m_iStatus == IOCP_OP_DESTROY )
				break;
		}

		pcQueueUnit = m_cSendQueue.pop();

		if( pcQueueUnit != NULL )
		{
			sendPacket( pcQueueUnit->m_pcContext, pcQueueUnit->m_pstrPacket, pcQueueUnit->m_iPacketSize );

			delete pcQueueUnit;
		}
		else
		{
			ResetEvent( m_hSendQueueEvent );
		}
	}

	printf( "End IOCPSendThread.\n" );

	return 0;
}

unsigned int CIOCPNetworkModule::IOCPRecvThread( void *pArg )
{
	printf( "Begin IOCPRecvThread.\n" );

	CQueueUnit		*pcQueueUnit;

	while( 1 )
	{
		if( WaitForSingleObject( m_hRecvQueueEvent, INFINITE ) == WAIT_FAILED )
		{
			printf( "Error:%d\n", GetLastError() );
			getchar();
		}
		else
		{
			if( m_iStatus == IOCP_OP_DESTROY )
				break;
		}

		pcQueueUnit = m_cRecvQueue.pop();

		if( pcQueueUnit != NULL )
		{
			m_cLockMgr.Lock();

			if( m_cLockMgr.GetObject( (int) pcQueueUnit->m_hSocket ) )
			{
				pcQueueUnit->m_pcContext->RecvLock();

				// Echo Packet
				if( (char) 0xff == pcQueueUnit->m_pstrPacket[4] )
				{
					CPacketTypeCS_0xff	cPacketTypeCS_0xff;
					CPacketTypeSC_0xff	cPacketTypeSC_0xff;

					int				iPacketSize;
					int				iIndex;
					char			*pstrData;

					iIndex = 0;

					//cPacketTypeCS_0x00을 풀어낸다.
					memcpy( &cPacketTypeCS_0xff.m_iSize, &pcQueueUnit->m_pstrPacket[iIndex], sizeof(cPacketTypeCS_0xff.m_iSize) );
					iIndex += sizeof(cPacketTypeCS_0xff.m_iSize);

					memcpy( &cPacketTypeCS_0xff.m_iCommand, &pcQueueUnit->m_pstrPacket[iIndex], sizeof(cPacketTypeCS_0xff.m_iCommand) );
					iIndex += sizeof(cPacketTypeCS_0xff.m_iCommand);

					memcpy( &cPacketTypeCS_0xff.m_iSeq, &pcQueueUnit->m_pstrPacket[iIndex], sizeof(cPacketTypeCS_0xff.m_iSeq) );
					iIndex += sizeof(cPacketTypeCS_0xff.m_iSeq);

					iIndex = 0;
					cPacketTypeSC_0xff.m_iSeq = cPacketTypeCS_0xff.m_iSeq;

					iPacketSize = cPacketTypeSC_0xff.m_iSize;

					pstrData = new char[iPacketSize];

					memcpy( &pstrData[iIndex], &cPacketTypeSC_0xff.m_iSize, sizeof(cPacketTypeSC_0xff.m_iSize) );
					iIndex += sizeof(cPacketTypeSC_0xff.m_iSize);

					memcpy( &pstrData[iIndex], &cPacketTypeSC_0xff.m_iCommand, sizeof(cPacketTypeSC_0xff.m_iCommand) );
					iIndex += sizeof(cPacketTypeSC_0xff.m_iCommand);

					memcpy( &pstrData[iIndex], &cPacketTypeSC_0xff.m_iSeq, sizeof(cPacketTypeSC_0xff.m_iSeq) );
					iIndex += sizeof(cPacketTypeSC_0xff.m_iSeq);

					sendPacket(pcQueueUnit->m_pcContext, (char *)pstrData, iPacketSize );
				}
				// Patch 정보 Packet
				else if( 0x00 == pcQueueUnit->m_pstrPacket[4] )
				{
					CDList< CJZPFileInfo * > *pcPatchFileInfo;
					CPacketTypeCS_0x00	cPacketTypeCS_0x00;

					int				iVersion;
					unsigned int	iPatchCheckCode;
					int				iResult;
					int				iIndex;

					iIndex = 0;

					//cPacketTypeCS_0x00을 풀어낸다.
					memcpy( &cPacketTypeCS_0x00.m_iSize, &pcQueueUnit->m_pstrPacket[iIndex], sizeof(cPacketTypeCS_0x00.m_iSize) );
					iIndex += sizeof(cPacketTypeCS_0x00.m_iSize);

					memcpy( &cPacketTypeCS_0x00.m_iCommand, &pcQueueUnit->m_pstrPacket[iIndex], sizeof(cPacketTypeCS_0x00.m_iCommand) );
					iIndex += sizeof(cPacketTypeCS_0x00.m_iCommand);

					memcpy( &cPacketTypeCS_0x00.m_iVersion, &pcQueueUnit->m_pstrPacket[iIndex], sizeof(cPacketTypeCS_0x00.m_iVersion) );
					iIndex += sizeof(cPacketTypeCS_0x00.m_iVersion);

					memcpy( &cPacketTypeCS_0x00.m_iPatchCheckCode, &pcQueueUnit->m_pstrPacket[iIndex], sizeof(cPacketTypeCS_0x00.m_iPatchCheckCode) );
					iIndex += sizeof(cPacketTypeCS_0x00.m_iPatchCheckCode);

					//클라이언트로부터 버전을 얻어냈다.
					iVersion = cPacketTypeCS_0x00.m_iVersion;
					iPatchCheckCode = cPacketTypeCS_0x00.m_iPatchCheckCode;

					pcPatchFileInfo = new CDList< CJZPFileInfo * >;

					//최신패치버전과 받아야할 파일의 수를 보낸다.
					iResult = GetPatchFiles( iVersion, iPatchCheckCode, pcPatchFileInfo );

					if( iResult == 0 )
					{
						CListNode< CJZPFileInfo * >	*pcNode;
						__int64			iTotalSize;

						iTotalSize = 0;

						for( pcNode = pcPatchFileInfo->GetStartNode(); pcNode; pcNode=pcNode->m_pcNextNode )
						{
							iTotalSize += pcNode->m_tData->m_iFileSize;
							iTotalSize += pcNode->m_tData->m_iRawFileSize;
						}

						//패치 버전과 패치받을 파일의 수를 전달한다.
						CPacketTypeSC_0x01		cPacketTypeSC_0x01;

						int				iPacketSize;
						int				iIndex;
						char			*pstrData;

						iIndex = 0;
						cPacketTypeSC_0x01.m_iRecvFiles = pcPatchFileInfo->getCount(); //받아야하는 파일이 몇개인가?
						cPacketTypeSC_0x01.m_iVersion = m_iVersion;                    //올려야하는 버전은?
						cPacketTypeSC_0x01.m_iPatchCheckCode =  m_cAuPatchCheckCode.GetCode(m_iVersion);
						cPacketTypeSC_0x01.m_iTotalSize = iTotalSize;
						memset( cPacketTypeSC_0x01.m_strServerIP, 0, sizeof(cPacketTypeSC_0x01.m_strServerIP) );
						strcat( cPacketTypeSC_0x01.m_strServerIP, GetDownloadServerIP() );
						cPacketTypeSC_0x01.m_iServerPort = GetDownloadServerPort();

						iPacketSize = cPacketTypeSC_0x01.m_iSize;

						pstrData = new char[iPacketSize];

						memcpy( &pstrData[iIndex], &cPacketTypeSC_0x01.m_iSize, sizeof(cPacketTypeSC_0x01.m_iSize) );
						iIndex += sizeof(cPacketTypeSC_0x01.m_iSize);

						memcpy( &pstrData[iIndex], &cPacketTypeSC_0x01.m_iCommand, sizeof(cPacketTypeSC_0x01.m_iCommand) );
						iIndex += sizeof(cPacketTypeSC_0x01.m_iCommand);

						memcpy( &pstrData[iIndex], &cPacketTypeSC_0x01.m_iRecvFiles, sizeof(cPacketTypeSC_0x01.m_iRecvFiles) );
						iIndex += sizeof(cPacketTypeSC_0x01.m_iRecvFiles);

						memcpy( &pstrData[iIndex], &cPacketTypeSC_0x01.m_iVersion, sizeof(cPacketTypeSC_0x01.m_iVersion) );
						iIndex += sizeof(cPacketTypeSC_0x01.m_iVersion);

						memcpy( &pstrData[iIndex], &cPacketTypeSC_0x01.m_iPatchCheckCode, sizeof(cPacketTypeSC_0x01.m_iPatchCheckCode) );
						iIndex += sizeof(cPacketTypeSC_0x01.m_iPatchCheckCode);

						memcpy( &pstrData[iIndex], &cPacketTypeSC_0x01.m_iTotalSize, sizeof(cPacketTypeSC_0x01.m_iTotalSize) );
						iIndex += sizeof(cPacketTypeSC_0x01.m_iTotalSize);

						memcpy( &pstrData[iIndex], &cPacketTypeSC_0x01.m_strServerIP, sizeof(cPacketTypeSC_0x01.m_strServerIP) );
						iIndex += sizeof(cPacketTypeSC_0x01.m_strServerIP);

						memcpy( &pstrData[iIndex], &cPacketTypeSC_0x01.m_iServerPort, sizeof(cPacketTypeSC_0x01.m_iServerPort) );
						iIndex += sizeof(cPacketTypeSC_0x01.m_iServerPort);

						sendPacket(pcQueueUnit->m_pcContext, (char *)pstrData, iPacketSize );

						//실제 패치 파일을 보낸다.
						//1. 패치 파일 리스트를 붙인다.
						pcQueueUnit->m_pcContext->m_pcPatchFileInfo = pcPatchFileInfo;
						pcQueueUnit->m_pcContext->m_pcPatchFileInfoNode = pcQueueUnit->m_pcContext->m_pcPatchFileInfo->GetStartNode();
					}
					else
					{
						int				iPacketSize;
						int				iIndex;
						char			*pstrData;

						iIndex = 0;

						CPacketTypeSC_0xEE  cPacketType0xEE;

						iPacketSize = cPacketType0xEE.m_iSize;

						pstrData = new char[iPacketSize];

						memcpy( &pstrData[iIndex], &cPacketType0xEE.m_iSize, sizeof(cPacketType0xEE.m_iSize) );
						iIndex += sizeof(cPacketType0xEE.m_iSize);

						memcpy( &pstrData[iIndex], &cPacketType0xEE.m_iCommand, sizeof(cPacketType0xEE.m_iCommand) );
						iIndex += sizeof(cPacketType0xEE.m_iCommand);

						memcpy( &pstrData[iIndex], &iResult, sizeof(iResult) );
						iIndex += sizeof(iResult);

						sendPacket(pcQueueUnit->m_pcContext, (char *)pstrData, iPacketSize );
					}
				}
				//패치파일 정보를 달라는 패킷을 받았다면?
				else if( 0x04 == pcQueueUnit->m_pstrPacket[4] )
				{
					if( pcQueueUnit->m_pcContext->m_pcPatchFileInfoNode != NULL )
					{
						sendPatchFileInfo( pcQueueUnit->m_pcContext );
					}
					else //if( pcQueueUnit->m_pcContext->m_pcPatchFileInfoNode == NULL )
					{
						//파일을 다 전송했으니 접속을 끊는다.
						closesocket( pcQueueUnit->m_pcContext->m_hSocket );
					}
				}
				//패치파일 정보를 달라는 패킷을 받았다면?
				else if( 0x05 == pcQueueUnit->m_pstrPacket[4] )
				{
					SendClientFileCRC( pcQueueUnit->m_pcContext );
				}

				pcQueueUnit->m_pcContext->RecvUnlock();

				delete [] pcQueueUnit->m_pstrPacket;
				delete pcQueueUnit;
			}

			m_cLockMgr.Unlock();
		}
		else
		{
			ResetEvent( m_hRecvQueueEvent );
		}
	}

	printf( "End IOCPRecvThread.\n" );

	return 0;
}

VOID CIOCPNetworkModule::InitLocalIPAddr()
{
    CHAR strLocalHostName[MAX_PATH];
    gethostname( strLocalHostName, MAX_PATH );
    HOSTENT* pHostEnt = gethostbyname( strLocalHostName );

	printf("Local Hostname : %s\n", strLocalHostName);

	m_nNumAddr = 0;

    if( pHostEnt )
    {
		for (int i = 0; i < MAX_IP; i++)
		{
			_tcscpy( m_szLocalAddr[i], TEXT("") );

			in_addr* pInAddr = (in_addr*) pHostEnt->h_addr_list[i];
			if (!pInAddr)
				break;

			char* strLocalIP = inet_ntoa( *pInAddr );
			if( strLocalIP )
			{
				printf("Local IP : %s\n", strLocalIP);
				strncpy(m_szLocalAddr[i], strLocalIP, strlen(strLocalIP) + 1);
			}

			m_nNumAddr++;
		}
    }
}

BOOL CIOCPNetworkModule::SendClientFileCRC( CSocketIOContext *pContext )
{
	if( !pContext ) return FALSE;

	CPacketTypeSC_0x05 cPacket;

	unsigned int nSize = cPacket.m_iSize;
	unsigned int nIndex = 0;

	char* pData = new char[ nSize ];
	memset( pData, 0, sizeof( char ) * nSize );

	memcpy( &pData[ nIndex ], &cPacket.m_iSize, sizeof( cPacket.m_iSize ) );
	nIndex += sizeof( cPacket.m_iSize );

	memcpy( &pData[ nIndex ], &cPacket.m_iCommand, sizeof( cPacket.m_iCommand ) );
	nIndex += sizeof( cPacket.m_iCommand );

	memcpy( &pData[ nIndex ], &m_ulClientFileCRC, sizeof( m_ulClientFileCRC ) );
	nIndex += sizeof( m_ulClientFileCRC );

	if( !sendPacket( pContext, pData, nSize ) ) return FALSE;
	return TRUE;
}

unsigned long CIOCPNetworkModule::_GetClientFileCRC( char* pFolderName, char* pFileName )
{
	unsigned long ulCRC = 0;
	if( !pFolderName || strlen( pFolderName ) <= 0 ) return ulCRC;
	if( !pFileName || strlen( pFileName ) <= 0 ) return ulCRC;

	char strFileName[ 256 ] = { 0, };
	strcpy_s( strFileName, sizeof( char ) * 256, pFileName );

	CPackingFile* pFile = m_cAuPackingManager.GetFile( pFolderName, strFileName, false );
	if( !pFile ) return ulCRC;

	ulCRC = pFile->m_lFileCheckSum;
	return ulCRC;
}
