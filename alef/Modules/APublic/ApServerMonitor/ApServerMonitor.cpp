#include "ApServerMonitor.h"

int atoi_serverstate(const char *szServerState)
{
	if (strcmp("Stopped", szServerState) == 0)
	{
		return ASSM_SERVER_STATE_NOT_ACTIVE;
	}
	else if (strcmp("Loading", szServerState) == 0)
	{
		return ASSM_SERVER_STATE_LOADING;
	}
	else if (strcmp("Started", szServerState) == 0)
	{
		return ASSM_SERVER_STATE_STARTED;
	}
	else if (strcmp("Abnormal", szServerState) == 0)
	{
		return ASSM_SERVER_STATE_ABNORMAL;
	}

	return -1;
}

const char* itoa_serverstate(int nServerState)
{
	switch(nServerState) {
	case ASSM_SERVER_STATE_NOT_ACTIVE:
		return "Stopped";
	case ASSM_SERVER_STATE_LOADING:
		return "Loading";
	case ASSM_SERVER_STATE_STARTED:
		return "Started";
	case ASSM_SERVER_STATE_ABNORMAL:
		return "Abnormal";
	default:
		return "?";
	}

	return "?";
}

int atoi_servertype(const char *szServerType)
{
	if (strncmp(szServerType, "login", 20) == 0)
	{
		return ASSM_SERVER_TYPE_LOGIN_SERVER;
	}
	else if (strncmp(szServerType, "relay", 20) == 0)
	{
		return ASSM_SERVER_TYPE_RELAY_SERVER;
	}
	else if (strncmp(szServerType, "game", 20) == 0)
	{
		return ASSM_SERVER_TYPE_GAME_SERVER;
	}
	else if (strncmp(szServerType, "chatting", 20) == 0)
	{
		return ASSM_SERVER_TYPE_CHATTING_SERVER;
	}
	else if (strncmp(szServerType, "patch", 20) == 0)
	{
		return ASSM_SERVER_TYPE_PATCH_SERVER;
	}

	return ASSM_SERVER_TYPE_INVALID;
}

const char* itoa_servertype(int nServerType)
{
	switch(nServerType) {
	case ASSM_SERVER_TYPE_LOGIN_SERVER:
		return "login";
		break;
	case ASSM_SERVER_TYPE_RELAY_SERVER:
		return "relay";
		break;
	case ASSM_SERVER_TYPE_GAME_SERVER:
		return "game";
		break;
	case ASSM_SERVER_TYPE_CHATTING_SERVER:
		return "chatting";
		break;
	case ASSM_SERVER_TYPE_PATCH_SERVER:
		return "patch";
		break;
	default:
		return "???";
		break;
	}

	return "???";
}

bool operator<(const AssmServerID &dt1, const AssmServerID &dt2)
{
	int nRet = 0;

	if ((nRet = strncmp(dt1.m_szServerName, dt2.m_szServerName, ASSM_MAX_SERVER_NAME)) != 0)
	{
		return nRet < 0;
	}

	if (dt1.m_nServerType != dt2.m_nServerType)
	{
		return dt1.m_nServerType < dt2.m_nServerType;
	}

	return false;
}
