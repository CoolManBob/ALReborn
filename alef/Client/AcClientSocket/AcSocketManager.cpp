// AcSocketManager.cpp: implementation of the AcSocketManager class.
//
//////////////////////////////////////////////////////////////////////

#include "AcSocketManager.h"
#include "AuPacket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcSocketManager::AcSocketManager()
{
	m_lControlServerID	= -1;
	m_lGameServerID		= -1;
	m_pSocketArray		= NULL; 
	m_lMaxConnection	= 0;
//	m_lMaxPacket		= 0;
	
	AcClientSocket::NetworkInitialize();
}

AcSocketManager::~AcSocketManager()
{
	if (m_pSocketArray)	delete [] m_pSocketArray;
	AcClientSocket::NetworkUnInitialize();
}

BOOL AcSocketManager::SetMaxConnection(INT32 lMaxConnection, INT32 lMaxPacketBufferSize)
{
	if (m_pSocketArray) return FALSE;

	m_lMaxConnection = lMaxConnection;
	m_pSocketArray = new stConnectionList[lMaxConnection];
	for (INT32 i = 0; i < m_lMaxConnection; ++i)
	{
		m_pSocketArray[i].Socket.SetIndex(i);
		m_pSocketArray[i].Socket.SetMaxPacketBufferSize(lMaxPacketBufferSize);
	}
	
	return TRUE;
}

INT32 AcSocketManager::GetEmptySocket()
{
	for (INT32 i = 0; i < m_lMaxConnection; ++i)
	{
		if (ACSOCKET_STATE_NULL == m_pSocketArray[i].Socket.GetState())
			return i;
	}

	return INVALID_INDEX;
}

BOOL AcSocketManager::SetDestroyNormal()
{
	for (INT32 i = 0; i < m_lMaxConnection; ++i)
	{
		if (m_pSocketArray[i].Socket.GetState() == ACSOCKET_STATE_CONNECTED)
			m_pSocketArray[i].Socket.SetDestroyNormal();
	}

	return TRUE;
}

INT32 AcSocketManager::Connect(LPCTSTR szServerInfo, INT32 lServerType, PVOID pClass,
							  ApModuleDefaultCallBack fpConnect, ApModuleDefaultCallBack fpDisconnect, 
							  ApModuleDefaultCallBack fpError)
{
	ASSERT( szServerInfo );
	ASSERT( 0 < strlen(szServerInfo) );

	INT32 lIndex = GetEmptySocket();
	ASSERT(INVALID_INDEX != lIndex);

	if( !szServerInfo || !szServerInfo[0] )		return INVALID_INDEX;

	char* strPort = (char*)strchr( szServerInfo, ':');	// pszServerInfo Format :  "xxx.xxx.xxx.xxx:port"
	if( !strPort )								return INVALID_INDEX;

	INT32 lPort = atoi(strPort + 1);
	char strIP[128] = { 0, };
	strncpy( strIP, szServerInfo, strPort - szServerInfo );

	m_pSocketArray[lIndex].Socket.Close();
	m_pSocketArray[lIndex].Socket.Create();
	m_pSocketArray[lIndex].Socket.SetCallBack(pClass, fpConnect, fpDisconnect, fpError);
	m_pSocketArray[lIndex].Socket.Connect(strIP, lPort);
	m_pSocketArray[lIndex].Socket.SetSocketManager(this);

	strncpy( m_pSocketArray[lIndex].szAddress, szServerInfo, 23 );

	if (m_lControlServerID < 0)
		m_lControlServerID = lIndex;

	m_pSocketArray[lIndex].nServerType = lServerType;

	return lIndex;
}

INT32 AcSocketManager::Send(CHAR* pData, INT16 nDataSize, INT16 nSendType, INT16 nServerID)
{
	if (nDataSize > APPACKET_MAX_PACKET_SIZE || nSendType < 0)
		return (-1);

	// 2006.04.14. steeple
	// 다시 제자리로 복귀
	((PPACKET_HEADER) pData)->lOwnerID = GetSelfCID();

	if (nServerID == 0 && nSendType == 0)
	{
		if (-1 == m_lGameServerID)
		{
			ASSERT(!"게임서버가 접속된 상태가 아닙니다.");
			return -1;
		}
		else
			return m_pSocketArray[m_lGameServerID].Socket.Send((AcPacketData*)pData, nDataSize);
	}

	if (nServerID > INVALID_INDEX) 
	{
		return m_pSocketArray[nServerID].Socket.Send((AcPacketData*)pData, nDataSize);
	}

	INT32 lResult = 0;
	switch (nSendType) 
	{
	case ACDP_SEND_GAMESERVERS:
		if (-1 == m_lGameServerID)
		{
			ASSERT(!"게임서버가 접속된 상태가 아닙니다.");
		}
		lResult = m_pSocketArray[m_lGameServerID].Socket.Send((AcPacketData*)pData, nDataSize);
		break;

	case ACDP_SEND_SERVER:
		lResult = m_pSocketArray[nServerID].Socket.Send((AcPacketData*)pData, nDataSize);
		break;

	case ACDP_SEND_LOGINSERVER:
		for (INT32 i = 0; i < m_lMaxConnection; ++i)
		{
			if (ACDP_SERVER_TYPE_LOGINSERVER == m_pSocketArray[i].nServerType)
				lResult = m_pSocketArray[i].Socket.Send((AcPacketData*)pData, nDataSize);
		}
		break;
	}

	return lResult;
}

VOID AcSocketManager::Disconnect(INT32 lNID)
{
	if (m_lMaxConnection > lNID && INVALID_INDEX < lNID )
		m_pSocketArray[lNID].Socket.Close();
}

BOOL AcSocketManager::DisconnectAll()
{
	for (INT32 i = 0; i < m_lMaxConnection; ++i)
	{
		if (ACSOCKET_STATE_NULL != m_pSocketArray[i].Socket.GetState())
		{
			Disconnect(i);
		}
	}

	return TRUE;
}

INT32 AcSocketManager::GetControlServerID()
{
	if (m_lControlServerID < 0 || m_lControlServerID >= m_lMaxConnection)
		return INVALID_INDEX;

	return m_pSocketArray[m_lControlServerID].lServerID;
}

INT32 AcSocketManager::GetDealServerNID()
{
	for (INT32 i = 0; i < m_lMaxConnection; ++i)
		if ( m_pSocketArray[i].nServerType == ACDP_SERVER_TYPE_DEALSERVER )
			return i;

	return INVALID_INDEX;
}

INT32 AcSocketManager::GetRecruitServerNID()
{
	for (INT32 i = 0; i < m_lMaxConnection; ++i)
		if ( m_pSocketArray[i].nServerType == ACDP_SERVER_TYPE_RECRUITSERVER )
			return i;

	return INVALID_INDEX;
}

INT32 AcSocketManager::GetLoginServerNID()
{
	for (INT32 i = 0; i < m_lMaxConnection; ++i)
		if ( m_pSocketArray[i].nServerType == ACDP_SERVER_TYPE_LOGINSERVER )
			return i;

	return INVALID_INDEX;
}

stConnectionList* AcSocketManager::GetConnectionListPtr(INT32 lIndex)
{
	if(lIndex < 0 || lIndex >= m_lMaxConnection)
		return NULL;

	return &m_pSocketArray[lIndex];
}
