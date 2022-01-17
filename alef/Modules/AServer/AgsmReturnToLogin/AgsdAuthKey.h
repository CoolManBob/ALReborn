#pragma once

#include "ApBase.h"
#include "AgsdAccount.h"
#include "AuRandomNumber.h"

class AgsdAuthKey : public ApBase {
public:
	AgsdAuthKey()
	{
		ZeroMemory(m_szAccountName, sizeof(CHAR) * (AGSMACCOUNT_MAX_ACCOUNT_NAME + 1));
		ZeroMemory(m_szWorldName, sizeof(CHAR) * (AGPDWORLD_MAX_WORLD_NAME + 1));
		m_lAuthKey	= (-1);
		m_ulAddTimeMSec	= 0;
	};

private:
	CHAR	m_szAccountName[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];
	CHAR	m_szWorldName[AGPDWORLD_MAX_WORLD_NAME + 1];
	INT32	m_lAuthKey;
	UINT32	m_ulAddTimeMSec;

public:
	CHAR	*GetAccountName() { return m_szAccountName; };
	BOOL	SetAccountName(CHAR *pszAccountName)
			{
				if (!pszAccountName || !pszAccountName[0]) return FALSE;

				ZeroMemory(m_szAccountName, sizeof(CHAR) * (AGSMACCOUNT_MAX_ACCOUNT_NAME + 1));
				strncpy(m_szAccountName, pszAccountName, AGSMACCOUNT_MAX_ACCOUNT_NAME);

				return TRUE;
			};

	CHAR	*GetWorldName() { return m_szWorldName; };
	BOOL	SetWorldName(CHAR *pszWorldName)
			{
				if (!pszWorldName || !pszWorldName[0]) return FALSE;

				ZeroMemory(m_szWorldName, sizeof(CHAR) * (AGPDWORLD_MAX_WORLD_NAME + 1));
				strncpy(m_szWorldName, pszWorldName, AGPDWORLD_MAX_WORLD_NAME);

				return TRUE;
			};

	INT32	GetAuthKey() { return m_lAuthKey; };
	BOOL	SetAuthKey(INT32 lAuthKey)
			{
				if (lAuthKey <= 0) return FALSE;
				m_lAuthKey = lAuthKey;
				return TRUE;
			};

	UINT32	GetAddTimeMSec() { return m_ulAddTimeMSec; };
	BOOL	SetAddTimeMSec(UINT32 ulAddTimeMSec)
			{
				m_ulAddTimeMSec = ulAddTimeMSec;
				return TRUE;
			};
};

class AgsaAuthKeyPool : public ApAdmin {
public:
	AgsaAuthKeyPool() {};
	~AgsaAuthKeyPool() {};

private:
	MTRand	m_csRandom;

public:
	BOOL	Initialize(INT32 lMaxPool = 5000);

	AgsdAuthKey	*AddAuthKey(CHAR *pszAccountName, CHAR *pszWorldName, UINT32 ulAddTimeMSec);
	BOOL	RemoveAuthKey(CHAR *pszAccountName);
	AgsdAuthKey	*GetAuthKey(CHAR *pszAccountName);

private:
	INT32	GetAuthKey();

public:
	BOOL	ProcessTimeout(UINT32 ulCurrentTimeMSec, UINT32 ulTimeoutMSec);
};