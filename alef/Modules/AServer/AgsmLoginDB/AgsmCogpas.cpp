#include "AgsmCogpas.h"
#include <stdio.h>

CCogpas g_cogpas;

CCogpas::CCogpas()
:	m_dll(0),
	m_lpfnAuthenticate(0),
	m_lpfnAuthUser(0)
{
	InitializeCriticalSection(&m_cs);
}

CCogpas::~CCogpas()
{
	DeleteCriticalSection(&m_cs);
}

bool CCogpas::LoadDLL()
{
	m_dll = LoadLibrary("COGPAS.dll");
	if (NULL == m_dll)
	{
		printf("ERROR: can't load COGPASS.dll\n");
	}
	else
	{
		m_lpfnAuthenticate = (LPFNAUTHENTICATE)GetProcAddress(m_dll, "Authenticate");
		m_lpfnAuthUser = ( LPFNAUTHENTICATEUSER )GetProcAddress( m_dll, "AuthenticateUsername" );
	}

	return m_lpfnAuthUser != NULL && m_lpfnAuthenticate != NULL;
}

int CCogpas::AuthenticateUser(char* userName, bool verbose)
{
	wchar_t uniUserName[33];
	MultiByteToWideChar(CP_ACP, 0, userName, -1, uniUserName, 33);

	EnterCriticalSection(&m_cs);
	int result = m_lpfnAuthUser(uniUserName, true);
	LeaveCriticalSection(&m_cs);

	return result;
}

int CCogpas::Authenticate(char* account, char* password)
{
	wchar_t uniAccount[33];
	wchar_t uniPassword[33];
	MultiByteToWideChar(CP_ACP, 0, account, -1, uniAccount, 33);
	MultiByteToWideChar(CP_ACP, 0, password, -1, uniPassword, 33);

	EnterCriticalSection(&m_cs);
	int result = m_lpfnAuthenticate(uniAccount, uniPassword, true);
	LeaveCriticalSection(&m_cs);

	if (result > 0)
		return AGSMLOGINDB_AUTH_RESULT_SUCCESS;

	if (-1 == result)
		return AGSMLOGINDB_AUTH_RESULT_PWD_FAIL;

	if (-8 == result)
		return AGSMLOGINDB_AUTH_RESULT_IN_STOP;

	return AGSMLOGINDB_AUTH_RESULT_UNKNOWN;
}
