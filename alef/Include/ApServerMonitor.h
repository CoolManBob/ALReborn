#ifndef _AP_SERVER_MONITOR_H_
#define _AP_SERVER_MONITOR_H_

#include "ApDefine.h"

#include <vector>

using namespace std;

// ASSM_SERVER_STATE_XXX
const int	ASSM_SERVER_STATE_NOT_ACTIVE	= 0;	// Stopped (not Running)
const int	ASSM_SERVER_STATE_LOADING		= 1;	// Loading Resources (not Running)
const int	ASSM_SERVER_STATE_STARTED		= 2;	// Started (Running)
const int	ASSM_SERVER_STATE_ABNORMAL		= -1;	// Abnormal
int atoi_serverstate(const char *szServerState);
const char* itoa_serverstate(int nServerState);

// ASSM_SERVER_TYPE_XXX
const int	ASSM_SERVER_TYPE_INVALID				= 0;
const int	ASSM_SERVER_TYPE_LOGIN_SERVER			= 9;
const int	ASSM_SERVER_TYPE_GAME_SERVER			= 1;
const int	ASSM_SERVER_TYPE_RELAY_SERVER			= 4;
const int	ASSM_SERVER_TYPE_PATCH_SERVER			= 11;	// patch server
const int	ASSM_SERVER_TYPE_CHATTING_SERVER		= 10;	// not fixed
const int	ASSM_SERVER_TYPE_GENERAL_PARAM_STRING	= 99;	// general param string, raw string output
int atoi_servertype(const char *szServerType);
const char* itoa_servertype(int nServerType);

const int	ASSM_MAX_GROUP_NAME				= 64;
const int	ASSM_MAX_SERVER_NAME			= 64;
const int	ASSM_MAX_IP_ADDRESS				= 30;
const int	ASSM_MAX_ACCOUNT_STRING			= 32;
const int	ASSM_MAX_PASSWORD_STRING		= 32;
const int	ASSM_MAX_TIME_STRING			= 32;
const int	ASSM_MAX_GENERAL_PARAM_STRING	= 256;

const int	ASSM_PORT_LOGIN_SERVER	= 12001;
const int	ASSM_PORT_GAME_SERVER	= 12008;
const int	ASSM_PORT_RELAY_SERVER	= 12004;
const int	ASSM_PORT_PATCH_SERVER	= 11001;
const int	ASSM_PORT_SM_SERVER		= 11010;	// Server Monitoring Server Port

// ASSM_USER_CLASS_XXX
#define ASSM_USER_CLASS_NONE	0		// invalid account class
#define ASSM_USER_CLASS_S		1		// S Class -> can see number of users, can turn ON/OFF server (super user)
#define ASSM_USER_CLASS_1		2		// 1 Class -> cannot see number of users, can turn ON/OFF server
#define ASSM_USER_CLASS_2		3		// 2 Class -> cannot see number of users, cannot turn ON/OFF server
#define ASSM_USER_CLASS_M		4		// M Class -> can see number of users, cannot turn ON/OFF server (monitor)

class AssmNoticeRepeat
{
public:
	INT32	m_lID;
	CHAR *	m_szMessage;
	UINT32	m_ulGap;
	UINT32	m_ulDuration;

	UINT32	m_ulStartTime;
	UINT32	m_ulLastNoticeTime;

	AssmNoticeRepeat()
	{
		m_lID				= 0;
		m_szMessage			= NULL;
		m_ulGap				= 0;
		m_ulDuration		= 0;

		m_ulStartTime		= 0;
		m_ulLastNoticeTime	= 0;
	};
};

typedef vector <AssmNoticeRepeat>		AssmNoticeVector;
typedef AssmNoticeVector::iterator		AssmNoticeIter;

enum eAssmClientType
{
	ASSM_CLIENT_TYPE_AGENT		= 1,
	ASSM_CLIENT_TYPE_CONSOLE	= 2
};

struct AssmServerData
{
	int		m_nServerType;										// ASSM_SERVER_TYPE_XXX
	int		m_nServerNumber;									// Server Number
	char	m_szGroupName[ASSM_MAX_GROUP_NAME];					// Server Group (maybe equal to Server Name)
	char	m_szServerName[ASSM_MAX_SERVER_NAME];				// Server Name (World)
	char	m_szIPAddress[ASSM_MAX_IP_ADDRESS];					// Server IP, Port
	int		m_nServerState;										// Server Status, ASSM_SERVER_STATE_XXX
	int		m_nUserCount;										// User Count
	int		m_nMaxUserCount;
	char	m_szServerStartTime[ASSM_MAX_TIME_STRING];			// Server Start Time
	char	m_szServerVersionTime[ASSM_MAX_TIME_STRING];		// Server EXE ModTime
	int		m_nLoginServerConnCountByGameServer;				// Login Server Connection Count, From Game Server, added 20050406
	int		m_nRelayServerConnCountByGameServer;				// Relay Server Connection Count, From Game Server, added 20050406
	int		m_nRelayServerStatusByGameServer;					// Relay Server Status by Game Server, From Game Server, added 20050406
	int		m_nRelayServerQueryFailCountByRelayServer;			// Relay Server Query Fail Count, From Relay Server, added 20050406
	int		m_nGeneralParam1;									// General Param 1, added 20050406
	int		m_nGeneralParam2;									// General Param 2, added 20050406
	int		m_nGeneralParam3;									// General Param 3, added 20050406
	char	m_szGeneralParam[ASSM_MAX_GENERAL_PARAM_STRING];	// String General Param, add 20051010
	char	m_szConsoleIPAddress[ASSM_MAX_IP_ADDRESS];			// Console IP
	int		m_nProcessorUsage;									// Processor Usage
	float	m_fExpAdjustmentRatio;								// Exp Adjustment Ratio
	float	m_fDropAdjustmentRatio;								// Drop Adjustment Ratio
	float	m_fGheldAdjustmentRatio;							// Gheld Adjustment Ratio

	AssmServerData() { Reset(); }
	void Reset() { memset(this, 0, sizeof(AssmServerData)); }
};

struct AssmControlData
{
	INT32		m_nServerControlType;
	INT32		m_nClientType;
	INT32		m_nServerNumber;
	char		m_szServerName[ASSM_MAX_SERVER_NAME];
	INT32		m_nServerType;
	INT32		m_nGeneralParam1;
	INT32		m_nGeneralParam2;
	INT32		m_nGeneralParam3;
	char		m_szAccount[ASSM_MAX_ACCOUNT_STRING];
	char		m_szPassword[ASSM_MAX_PASSWORD_STRING];
	INT32		m_nServerManagerVersion;
	char		m_szConsoleIPAddress[ASSM_MAX_IP_ADDRESS];			// Console IP
	char		m_szGeneralParam[ASSM_MAX_GENERAL_PARAM_STRING];	// String General Param, add 20060707
};

struct AssmServerID
{
	char	m_szServerName[ASSM_MAX_SERVER_NAME];	// Server Name (World)
	int		m_nServerType;							// ASSM_SERVER_TYPE_XXX

	AssmServerID() { Reset(); };

	inline void Reset()
	{
		memset(m_szServerName, 0, ASSM_MAX_GROUP_NAME);
		m_nServerType = ASSM_SERVER_TYPE_INVALID;
	}

	inline AssmServerID& operator=(const AssmServerData& rhs)
	{
		strncpy(m_szServerName, rhs.m_szServerName, ASSM_MAX_SERVER_NAME);
		this->m_nServerType = rhs.m_nServerType;

		return *this;
	}

	inline AssmServerID& operator=(const AssmControlData& rhs)
	{
		strncpy(m_szServerName, rhs.m_szServerName, ASSM_MAX_SERVER_NAME);
		this->m_nServerType = rhs.m_nServerType;

		return *this;
	}

	inline bool operator==(const AssmServerID & dt)
	{
		if (strncmp(m_szServerName, dt.m_szServerName, ASSM_MAX_SERVER_NAME) == 0
			&& m_nServerType == dt.m_nServerType
			)
		{
			return true;
		}

		return false;
	}

	inline bool operator!=(const AssmServerID & dt)
	{
		return !(*this == dt);
	}

	inline bool operator<(const AssmServerID & dt)
	{
		int nRet = 0;

		if ((nRet = strncmp(m_szServerName, dt.m_szServerName, ASSM_MAX_SERVER_NAME)) != 0)
		{
			return nRet < 0;
		}

		if (m_nServerType != dt.m_nServerType)
		{
			return m_nServerType < dt.m_nServerType;
		}

		return false;
	}

	inline bool operator>(const AssmServerID & dt)
	{
		int nRet = 0;

		if ((nRet = strncmp(m_szServerName, dt.m_szServerName, ASSM_MAX_SERVER_NAME)) != 0)
		{
			return nRet > 0;
		}

		if (m_nServerType != dt.m_nServerType)
		{
			return m_nServerType > dt.m_nServerType;
		}

		return false;
	}

	inline bool operator<=(const AssmServerID &dt) { return !(*this > dt); }
	inline bool operator>=(const AssmServerID &dt) { return !(*this < dt); }
};

bool operator<(const AssmServerID &dt1, const AssmServerID &dt2);

#endif // _AP_SERVER_MONITOR_H_
