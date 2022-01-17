#pragma once

#include "ApBase.h"
#include "AcClientSocket.h"

enum eAcDPServerType
{
	ACDP_SERVER_TYPE_GAMESERVER		= 0,
	ACDP_SERVER_TYPE_LOGINSERVER,
	ACDP_SERVER_TYPE_DEALSERVER,
	ACDP_SERVER_TYPE_RECRUITSERVER,
	ACDP_SERVER_TYPE_AUCTIONSERVER,
	ACDP_SERVER_TYPE_CHATTINGSERVER,
};

enum eAcDPSendType
{
	ACDP_SEND_GAMESERVERS			= 0,
	ACDP_SEND_SERVER,
	ACDP_SEND_LOGINSERVER,
	ACDP_SEND_DEALSERVER,
	ACDP_SEND_RECRUITSERVER,
	ACDP_SEND_AUCTIONSERVER,
	ACDP_SEND_CHATTINGSERVER,
};

struct stConnectionList
{
	AcClientSocket	Socket;
	CHAR			szAddress[23];
	INT32			lServerID;
	INT16			nServerType;
};

class AcSocketManager  
{
public:
	AcSocketManager();
	virtual ~AcSocketManager();

	INT32 Connect(LPCTSTR szServerInfo, INT32 lServerType = ACDP_SERVER_TYPE_GAMESERVER, 
				PVOID pClass = NULL, ApModuleDefaultCallBack fpConnect = NULL,
				ApModuleDefaultCallBack fpDisconnect = NULL, ApModuleDefaultCallBack fpError = NULL);
	VOID Disconnect(INT32 lNID);
	BOOL DisconnectAll();
	
	INT32 Send(CHAR* pData, INT16 nDataSize, INT16 nSendType = ACDP_SEND_GAMESERVERS, INT16 nServerID = INVALID_INDEX);

	INT32 GetControlServerID();
	INT32 GetControlServerNID()			{	return m_lControlServerID;	}

	INT32 GetDealServerNID();
	INT32 GetRecruitServerNID();
	INT32 GetLoginServerNID();

	stConnectionList* GetConnectionListPtr(INT32 lIndex);

	VOID SetGameServerID(INT32 lNID)	{	m_lGameServerID = lNID;	}
	BOOL SetMaxConnection(INT32 lMaxConnection, INT32 lMaxPacketBufferSize);
	BOOL SetDestroyNormal();

	VOID SetSelfCID(INT32 lCID)			{	m_lSelfCID = lCID;	}
	INT32 GetSelfCID()					{	return m_lSelfCID;	}

private:
	INT32 GetEmptySocket();

private:
	stConnectionList*	m_pSocketArray;
	INT32				m_lControlServerID;
	INT32				m_lGameServerID;
	INT32				m_lSelfCID;

	INT32				m_lMaxPacket;
	INT32				m_lMaxConnection;
	
	AuCircularBuffer	m_AuCircularReceiveBuffer;
};
