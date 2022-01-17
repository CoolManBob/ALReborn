// AsEchoServer.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 05. 07.
//
// 2004.05.10. - AsIOCPServer 를 이용해서 전면 수정함

#include "AsEchoServer.h"
#include <time.h>

AsEchoServer::AsEchoServer()
{
	m_csIOCPServer.SetDispatchGameModule(this, EchoThread, NULL);
	
	m_eServerType = ASECHOSERVER_SERVER_TYPE_GAMESERVER;
	m_nPort = (INT16)ASECHOSERVER_PORT_GAMESERVER;

	m_pGetUserCountClass = NULL;
	m_pfGetUserCountCB = NULL;

	m_csEchoPacket.SetFlagLength(sizeof(INT8));
	m_csEchoPacket.SetFieldType(
								AUTYPE_MEMORY_BLOCK, 1,
								AUTYPE_END, 0
								);

	m_csServerInfoPacket.SetFlagLength(sizeof(INT8));
	m_csServerInfoPacket.SetFieldType(
								AUTYPE_INT32,		1,	// Current User Count
								AUTYPE_END,			0
								);
}

AsEchoServer::~AsEchoServer()
{
	Stop();
}

BOOL AsEchoServer::SetServerTypeLoginServer()
{
	return SetServerType(ASECHOSERVER_SERVER_TYPE_LOGINSERVER, ASECHOSERVER_PORT_LOGINSERVER);
}

BOOL AsEchoServer::SetServerTypeGameServer()
{
	return SetServerType(ASECHOSERVER_SERVER_TYPE_GAMESERVER, ASECHOSERVER_PORT_GAMESERVER);
}

BOOL AsEchoServer::SetServerTypeRelayServer()
{
	return SetServerType(ASECHOSERVER_SERVER_TYPE_RELAYSERVER, ASECHOSERVER_PORT_RELAYSERVER);
}

BOOL AsEchoServer::SetServerType(eAsEchoServerServerType eType, INT16 nPort)
{
	m_eServerType = eType;
	m_nPort = nPort;
	return TRUE;
}

BOOL AsEchoServer::Start()
{
	return m_csIOCPServer.Initialize(ASECHOSERVER_MAX_THREAD, m_nPort, ASECHOSERVER_MAX_CONNECTION);
}

BOOL AsEchoServer::Stop()
{
	return TRUE;
}

BOOL AsEchoServer::EchoThread(PVOID pvPacket, PVOID pvParam, PVOID pvSocket)
{
	AsEchoServer* pThis = (AsEchoServer*)pvParam;
	AsServerSocket* pSocket = (AsServerSocket*)pvSocket;

	if(!pThis || !pSocket)
		return FALSE;

	pThis->ParsePacket(pvPacket, *(INT16*)pvPacket, pSocket->GetIndex());

	return TRUE;
}

BOOL AsEchoServer::ParsePacket(PVOID pvPacket, INT16 nSize, INT32 lIndex)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	PPACKET_HEADER pstPacket = (PPACKET_HEADER)pvPacket;
	switch(pstPacket->cType)
	{
		case ASECHOSERVER_PACKET_ECHO:
			ParseEchoPacket(pvPacket, nSize, lIndex);
			break;

		case ASECHOSERVER_PACKET_SERVER_INFO:
			ParseServerInfoPacket(pvPacket, nSize, lIndex);
			break;
	}

	return TRUE;
}

BOOL AsEchoServer::ParseEchoPacket(PVOID pvPacket, INT16 nSize, INT32 lIndex)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	// 굳이 Parse 하지 않고 그냥 받은 패킷을 그대로 보낸다.
	return SendEchoPacket(pvPacket, nSize, lIndex);
}

BOOL AsEchoServer::ParseServerInfoPacket(PVOID pvPacket, INT16 nSize, INT32 lIndex)
{
	if(!pvPacket || nSize < 1)
		return FALSE;

	// 굳이 Parse 할 필요는 없다. (클라이언트에서 보내주는 정보는 없음. 단지 요청만 함.)
	return SendServerInfoPacket(lIndex);
}

PVOID AsEchoServer::MakeEchoPacket(INT16* pnPacketSize, CHAR* szMessage, INT16 nMessageLength)
{
	if(!pnPacketSize || !szMessage || nMessageLength < 1 || nMessageLength >= 255)
		return NULL;

	return m_csEchoPacket.MakePacket(TRUE, pnPacketSize, ASECHOSERVER_PACKET_ECHO,
											szMessage, &nMessageLength
											);
}

PVOID AsEchoServer::MakeServerInfoPacket(INT16* pnPacketSize, INT32 lCurrentUserCount)
{
	if(!pnPacketSize)
		return NULL;

	return m_csServerInfoPacket.MakePacket(TRUE, pnPacketSize, ASECHOSERVER_PACKET_SERVER_INFO,
											&lCurrentUserCount
											);
}

BOOL AsEchoServer::SendEchoPacket(PVOID pvReceivedPacket, INT16 nReceivedPacketSize, INT32 lIndex)
{
	if(!pvReceivedPacket || nReceivedPacketSize < 1)
		return FALSE;

	AsServerSocket* pSocket = m_csIOCPServer.m_csSocketManager[lIndex];
	if(!pSocket)
		return FALSE;

	return pSocket->AsyncSend((CHAR*)pvReceivedPacket, (INT32)nReceivedPacketSize, PACKET_PRIORITY_1);
}

BOOL AsEchoServer::SendServerInfoPacket(INT32 lIndex)
{
	AsServerSocket* pSocket = m_csIOCPServer.m_csSocketManager[lIndex];
	if(!pSocket)
		return FALSE;

	// Current User Count 를 구한다.
	INT32 lCurrentUser = 0;
	if(m_pfGetUserCountCB)
		m_pfGetUserCountCB(&lCurrentUser, m_pGetUserCountClass, NULL);
	
	//lCurrentUser = 11;

	INT16 nPacketSize = 0;
	PVOID pvPacket = MakeServerInfoPacket(&nPacketSize, lCurrentUser);
	if(!pvPacket || nPacketSize < 1)
		return FALSE;
	
	BOOL bSendResult = pSocket->AsyncSend((CHAR*)pvPacket, (INT32)nPacketSize, PACKET_PRIORITY_1);

	m_csServerInfoPacket.FreePacket(pvPacket);

	return bSendResult;
}

INT32 AsEchoServer::GetCurrentTimeStamp()
{
	time_t timeval;
	(void)time(&timeval);
	
	return timeval;
}

INT32 AsEchoServer::GetCurrentTimeDate(CHAR* szTimeBuf)
{
	if(!szTimeBuf)
		return 0;

	INT32 lCurrentTimeStamp = GetCurrentTimeStamp();

	time_t timeval;
	struct tm* tm_ptr;

	timeval = (time_t)lCurrentTimeStamp;
	tm_ptr = localtime(&timeval);

	strftime(szTimeBuf, 32, "%Y%m%d%H%M%S", tm_ptr);	// 초까지 남긴다.
	return strlen(szTimeBuf);
}

BOOL AsEchoServer::WriteSlowIdleLog(CHAR* szModuleName, INT32 lTick)
{
	if(!szModuleName || !szModuleName[0] || strlen(szModuleName) > 128)
		return FALSE;

	CHAR szFullFileName[MAX_PATH+1];
	CHAR szTimeDate[32];
	
	memset(szFullFileName, 0, MAX_PATH+1);
	memset(szTimeDate, 0, 32);

	if(GetCurrentTimeDate(szTimeDate) == 0)
		return FALSE;

	sprintf(szFullFileName, "./log/%s", ASECHOSERVER_FILENAME_SLOW_IDLE_LOG);
	memcpy(szFullFileName+strlen(szFullFileName), szTimeDate, 8);	// ex)20040521
	strcat(szFullFileName, ".log");

#ifdef _DEBUG
	FILE* pfFile = fopen(szFullFileName, "a+");
	if(!pfFile)
		return FALSE;

	fprintf(pfFile, "%s, Slow Idle Module Name : %s, Elapsed Time : %d(ms)\n", szTimeDate, szModuleName, lTick);

	fclose(pfFile);
#endif
	return TRUE;
}

BOOL AsEchoServer::WriteSlowDispatchLog(CHAR* szModuleName, INT32 lTick)
{
	if(!szModuleName || strlen(szModuleName) > 128)
		return FALSE;

	CHAR szFullFileName[MAX_PATH+1];
	CHAR szTimeDate[32];
	
	memset(szFullFileName, 0, MAX_PATH+1);
	memset(szTimeDate, 0, 32);

	if(GetCurrentTimeDate(szTimeDate) == 0)
		return FALSE;

	sprintf(szFullFileName, "./log/%s", ASECHOSERVER_FILENAME_SLOW_DISPATCH_LOG);
	memcpy(szFullFileName+strlen(szFullFileName), szTimeDate, 8);	// ex)20040521
	strcat(szFullFileName, ".log");

#ifdef _DEBUG
	FILE* pfFile = fopen(szFullFileName, "a+");
	if(!pfFile)
		return FALSE;

	fprintf(pfFile, "%s, Slow Dispatch Module Name : %s, Elapsed Time : %d(ms)\n", szTimeDate, szModuleName, lTick);

	fclose(pfFile);
#endif

	return TRUE;
}

BOOL AsEchoServer::SetCallbackGetUserCount(AsEchoServerCallback pfCallback, PVOID pClass)
{
	m_pGetUserCountClass = pClass;
	m_pfGetUserCountCB = pfCallback;
	return TRUE;
}

BOOL AsEchoServer::CBSlowIdleLog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szModuleName = (CHAR*)pData;
	AsEchoServer* pThis = (AsEchoServer*)pClass;
	INT32* plTick = (INT32*)pCustData;

	if(!szModuleName || !pThis || !plTick)
		return FALSE;

	return pThis->WriteSlowIdleLog(szModuleName, *plTick);
}

BOOL AsEchoServer::CBSlowDispatchLog(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szModuleName = (CHAR*)pData;
	AsEchoServer* pThis = (AsEchoServer*)pClass;
	INT32* plTick = (INT32*)pCustData;

	if(!szModuleName || !pThis || !plTick)
		return FALSE;

	return pThis->WriteSlowDispatchLog(szModuleName, *plTick);
}