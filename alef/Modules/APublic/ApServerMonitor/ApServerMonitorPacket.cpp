#include "ApServerMonitorPacket.h"

ApServerMonitorPacket::ApServerMonitorPacket()
{
	m_csApsmServerInfoPacket.SetFlagLength(sizeof(INT32));
	m_csApsmServerInfoPacket.SetFieldType(
/*  1 */		AUTYPE_INT32,		1,						// Server Type, ASSM_SERVER_TYPE_XXX
/*  2 */		AUTYPE_INT32,		1,						// Server Number, added 20050406
/*  3 */		AUTYPE_CHAR,		ASSM_MAX_GROUP_NAME,	// Group Name
/*  4 */		AUTYPE_CHAR,		ASSM_MAX_SERVER_NAME,	// Server Name
/*  5 */		AUTYPE_CHAR,		ASSM_MAX_IP_ADDRESS,	// IP Address
/*  6 */		AUTYPE_INT32,		1,						// Server State, ASSM_SERVER_STATE_XXX
/*  7 */		AUTYPE_INT32,		1,						// User Count
/*  8 */		AUTYPE_CHAR,		ASSM_MAX_TIME_STRING,	// 서버 시작 시간
/*  9 */		AUTYPE_CHAR,		ASSM_MAX_TIME_STRING,	// Version, 실행 파일 수정 시간, added 20050406
/* 10 */		AUTYPE_INT32,		1,						// Login Server Connection Count, From Game Server, added 20050406
/* 11 */		AUTYPE_INT32,		1,						// Relay Server Connection Count, From Game Server, added 20050406
/* 12 */		AUTYPE_INT32,		1,						// Relay Server Status by Game Server, From Game Server, added 20050406
/* 13 */		AUTYPE_INT32,		1,						// Relay Server Query Fail Count, From Relay Server, added 20050406
/* 14 */		AUTYPE_INT32,		1,						// General Param 1, added 20050406
/* 15 */		AUTYPE_INT32,		1,						// General Param 2, added 20050406
/* 16 */		AUTYPE_INT32,		1,						// General Param 3, added 20050406
/* 17 */		AUTYPE_CHAR,		ASSM_MAX_GENERAL_PARAM_STRING,	// General Param String, added 20051010
/* 18 */		AUTYPE_CHAR,		ASSM_MAX_IP_ADDRESS,	// SM Console IP
/* 19 */		AUTYPE_INT32,		1,						// Processor Usage
/* 20 */		AUTYPE_FLOAT,		1,						// Exp Ratio
/* 21 */		AUTYPE_FLOAT,		1,						// Drop Ratio
/* 22 */		AUTYPE_FLOAT,		1,						// Gheld Ratio
				AUTYPE_END,			0
		);

	m_csApsmServerControlPacket.SetFlagLength(sizeof(INT32));
	m_csApsmServerControlPacket.SetFieldType(
/*  1 */		AUTYPE_INT32,		1,						// server control type, eAssmServerControlType
/*  2 */		AUTYPE_INT32,		1,						// client type, eAssmClientType
/*  3 */		AUTYPE_INT32,		1,						// Server Number, added 20050406
/*  4 */		AUTYPE_CHAR,		ASSM_MAX_SERVER_NAME,	// Server Name, added 20050406
/*  5 */		AUTYPE_INT32,		1,						// Server Type, added 20050406
/*  6 */		AUTYPE_INT32,		1,						// General Param 1, added 20050406
/*  7 */		AUTYPE_INT32,		1,						// General Param 2, added 20050406
/*  8 */		AUTYPE_INT32,		1,						// General Param 3, added 20050406
/*  9 */		AUTYPE_CHAR,		ASSM_MAX_ACCOUNT_STRING,	// Account Name
/* 10 */		AUTYPE_CHAR,		ASSM_MAX_PASSWORD_STRING,	// Password
/* 11 */		AUTYPE_INT32,		1,							// ServerManager Version
/* 12 */		AUTYPE_CHAR,		ASSM_MAX_IP_ADDRESS,	// SM Console IP
/* 13 */		AUTYPE_CHAR,		ASSM_MAX_GENERAL_PARAM_STRING,	// General Param String, added 20060707
				AUTYPE_END,			0
		);
}

PVOID ApServerMonitorPacket::MakeServerInfoPacket(AssmServerData *pServerData, INT16 *pnPacketLenth)
{
	int nUserCount = ((pServerData->m_nMaxUserCount << 16) & 0xffff0000) | pServerData->m_nUserCount;

	return m_csApsmServerInfoPacket.MakePacket(TRUE, pnPacketLenth, ASSM_PACKET_SERVER_INFO,
		/*  1 */	&pServerData->m_nServerType,
		/*  2 */	&pServerData->m_nServerNumber,
		/*  3 */	&pServerData->m_szGroupName,
		/*  4 */	&pServerData->m_szServerName,
		/*  5 */	&pServerData->m_szIPAddress,
		/*  6 */	&pServerData->m_nServerState,
		/*  7 */	&nUserCount,
		/*  8 */	&pServerData->m_szServerStartTime,
		/*  9 */	&pServerData->m_szServerVersionTime,
		/* 10 */	&pServerData->m_nLoginServerConnCountByGameServer,
		/* 11 */	&pServerData->m_nRelayServerConnCountByGameServer,
		/* 12 */	&pServerData->m_nRelayServerStatusByGameServer,
		/* 13 */	&pServerData->m_nRelayServerQueryFailCountByRelayServer,
		/* 14 */	&pServerData->m_nGeneralParam1,
		/* 15 */	&pServerData->m_nGeneralParam2,
		/* 16 */	&pServerData->m_nGeneralParam3,
		/* 17 */	&pServerData->m_szGeneralParam,
		/* 18 */	&pServerData->m_szConsoleIPAddress,
		/* 19 */	&pServerData->m_nProcessorUsage,
		/* 20 */	&pServerData->m_fExpAdjustmentRatio,
		/* 21 */    &pServerData->m_fDropAdjustmentRatio,
		/* 22 */	&pServerData->m_fGheldAdjustmentRatio
		);
}

BOOL ApServerMonitorPacket::GetServerInfoPacket(PVOID pvPacket, INT16 nPacketLength, AssmServerData *pServerData)
{
	memset(pServerData, 0, sizeof(AssmServerData));

	char* szGroupName			= NULL;
	char* szServerName			= NULL;
	char* szIPAddress			= NULL;
	char* szServerStartTime		= NULL;
	char* szServerVersionTime	= NULL;
	char* szGeneralParam		= NULL;
	char* szConsoleIPAddress	= NULL;

	m_csApsmServerInfoPacket.GetField(TRUE, pvPacket, nPacketLength,
		/*  1 */	&pServerData->m_nServerType,
		/*  2 */	&pServerData->m_nServerNumber,
		/*  3 */	&szGroupName,
		/*  4 */	&szServerName,
		/*  5 */	&szIPAddress,
		/*  6 */	&pServerData->m_nServerState,
		/*  7 */	&pServerData->m_nUserCount,
		/*  8 */	&szServerStartTime,
		/*  9 */	&szServerVersionTime,
		/* 10 */	&pServerData->m_nLoginServerConnCountByGameServer,
		/* 11 */	&pServerData->m_nRelayServerConnCountByGameServer,
		/* 12 */	&pServerData->m_nRelayServerStatusByGameServer,
		/* 13 */	&pServerData->m_nRelayServerQueryFailCountByRelayServer,
		/* 14 */	&pServerData->m_nGeneralParam1,
		/* 15 */	&pServerData->m_nGeneralParam2,
		/* 16 */	&pServerData->m_nGeneralParam3,
		/* 17 */	&szGeneralParam,
		/* 18 */	&szConsoleIPAddress,
		/* 19 */	&pServerData->m_nProcessorUsage,
		/* 20 */	&pServerData->m_fExpAdjustmentRatio,
		/* 21 */	&pServerData->m_fDropAdjustmentRatio,
		/* 22 */	&pServerData->m_fGheldAdjustmentRatio
		);

	pServerData->m_nMaxUserCount = (pServerData->m_nUserCount >> 16) & 0x0000ffff;
	pServerData->m_nUserCount &= 0x0000ffff;

	if (szGroupName != NULL)
	{
		strncpy(pServerData->m_szGroupName, szGroupName, ASSM_MAX_GROUP_NAME);
	}

	if (szServerName != NULL)
	{
		strncpy(pServerData->m_szServerName, szServerName, ASSM_MAX_SERVER_NAME);
	}

	if (szIPAddress != NULL)
	{
		strncpy(pServerData->m_szIPAddress, szIPAddress, ASSM_MAX_IP_ADDRESS);
	}

	if (szServerStartTime != NULL)
	{
		strncpy(pServerData->m_szServerStartTime, szServerStartTime, ASSM_MAX_TIME_STRING);
	}

	if (szServerVersionTime != NULL)
	{
		strncpy(pServerData->m_szServerVersionTime, szServerVersionTime, ASSM_MAX_TIME_STRING);
	}

	if (szGeneralParam != NULL)
	{
		strncpy(pServerData->m_szGeneralParam, szGeneralParam, ASSM_MAX_GENERAL_PARAM_STRING);
	}

	if (szConsoleIPAddress != NULL)
	{
		strncpy(pServerData->m_szConsoleIPAddress, szConsoleIPAddress, ASSM_MAX_IP_ADDRESS);
	}

	return TRUE;
}

PVOID ApServerMonitorPacket::MakeServerControlPacket(AssmControlData *pControlData, INT16 *pnPacketLenth)
{
	return m_csApsmServerControlPacket.MakePacket(TRUE, pnPacketLenth, ASSM_PACKET_SERVER_CONTROL,
		/*  1 */	&pControlData->m_nServerControlType,
		/*  2 */	&pControlData->m_nClientType,
		/*  3 */	&pControlData->m_nServerNumber,
		/*  4 */	&pControlData->m_szServerName,
		/*  5 */	&pControlData->m_nServerType,
		/*  6 */	&pControlData->m_nGeneralParam1,
		/*  7 */	&pControlData->m_nGeneralParam2,
		/*  8 */	&pControlData->m_nGeneralParam3,
		/*  9 */	&pControlData->m_szAccount,
		/* 10 */	&pControlData->m_szPassword,
		/* 11 */	&pControlData->m_nServerManagerVersion,
		/* 12 */	&pControlData->m_szConsoleIPAddress,
		/* 13 */	&pControlData->m_szGeneralParam
		);
}

BOOL ApServerMonitorPacket::GetServerContolPacket(PVOID pvPacket, INT16 nPacketLength, AssmControlData *pControlData)
{
	memset(pControlData, 0, sizeof(AssmControlData));

	char* szServerName			= NULL;
	char* szAccount				= NULL;
	char* szPassword			= NULL;
	char* szGeneralParam		= NULL;
	char* szConsoleIPAddress	= NULL;

	m_csApsmServerControlPacket.GetField(TRUE, pvPacket, nPacketLength,
		/*  1 */	&pControlData->m_nServerControlType,
		/*  2 */	&pControlData->m_nClientType,
		/*  3 */	&pControlData->m_nServerNumber,
		/*  4 */	&szServerName,
		/*  5 */	&pControlData->m_nServerType,
		/*  6 */	&pControlData->m_nGeneralParam1,
		/*  7 */	&pControlData->m_nGeneralParam2,
		/*  8 */	&pControlData->m_nGeneralParam3,
		/*  9 */	&szAccount,
		/* 10 */	&szPassword,
		/* 11 */	&pControlData->m_nServerManagerVersion,
		/* 12 */	&szConsoleIPAddress,
		/* 13 */	&szGeneralParam
		);

	if (szServerName != NULL)
	{
		strncpy(pControlData->m_szServerName, szServerName, ASSM_MAX_SERVER_NAME);
	}

	if (szAccount != NULL)
	{
		strncpy(pControlData->m_szAccount, szAccount, ASSM_MAX_ACCOUNT_STRING);
	}

	if (szPassword != NULL)
	{
		strncpy(pControlData->m_szPassword, szPassword, ASSM_MAX_PASSWORD_STRING);
	}

	if (szConsoleIPAddress != NULL)
	{
		strncpy(pControlData->m_szConsoleIPAddress, szConsoleIPAddress, ASSM_MAX_IP_ADDRESS-1);
	}

	if (szGeneralParam)
	{
		strncpy(pControlData->m_szGeneralParam, szGeneralParam, ASSM_MAX_GENERAL_PARAM_STRING-1);
	}

	return TRUE;
}
