#include "stdafx.h"

#include "BasePatchClient.h"

#include <shlwapi.h>
#include <atlbase.h>
#include <Winerror.h>

#include "PatchClientRegistry.h"
#include "AutoDetectMemoryLeak.h"

int		g_iPatchServerPort = 11000;
int		g_iLoginServerPort = 11002;

#ifndef _TEST_SERVER_
	#ifdef _JPN
		#define LOGIN_SERVER "archlordlogin.hangame.co.jp"
		#define PATCH_SERVER "archlordpatch.hangame.co.jp"
	#else
		#define LOGIN_SERVER "login.archlord.com"
		#define PATCH_SERVER "patch.archlord.com"
	#endif
#else
	#define LOGIN_SERVER "login2.archlord.com"
	#define PATCH_SERVER "patch2.archlord.com"
#endif

#ifndef _TEST_SERVER_
	char*	g_strPatchServerIP		= PATCH_SERVER;
	char*	g_strLoginServerIP		= LOGIN_SERVER;
#else
	char *	g_strPatchServerIP		= PATCH_SERVER;
	char *	g_strLoginServerIP		= LOGIN_SERVER;

#endif // _TEST_SERVER_

#define sREG_SELF_PATCH_FLAG	"SelfPatchFlag"
#define	ALEF_REGISTRY_FILE		"Backup.reg"
#define MAX_TEMP_BUFFER_SIZE	256

#ifdef _KOR
	#define ALEF_REGKEY_HKEY	HKEY_CURRENT_USER
#endif

#ifdef _ENG
	#define ALEF_REGKEY_HKEY	HKEY_LOCAL_MACHINE
#endif

#ifdef _JPN
	#define ALEF_REGKEY_HKEY	HKEY_LOCAL_MACHINE
#endif

#ifdef _CHN
	#define ALEF_REGKEY_HKEY	HKEY_LOCAL_MACHINE
#endif

#ifdef _TIW
	#define ALEF_REGKEY_HKEY	HKEY_CURRENT_USER
#endif

CPatchClientRegistry::CPatchClientRegistry()
{
#ifdef _KOR
	FILE * f = fopen( "AlphaTest.arc", "rb" );

	if( f )
	{
		fclose( f );
		strcpy_s( m_strRegKeyBase, sizeof(m_strRegKeyBase), "Software\\Webzen\\Archlord_KR_Alpha" );
	}
	else
#endif
	{
		strcpy_s( m_strRegKeyBase, sizeof(m_strRegKeyBase), REG_KEY_NAME_ARCHLORD );
	}

	strcpy(m_strPatchServerIP, g_strPatchServerIP);
	m_iPatchServerPort = g_iPatchServerPort;
	strcpy(m_strLoginServerIP, g_strLoginServerIP);
	m_iLoginServerPort = g_iLoginServerPort;

#ifdef _KOR
	FILE* pFile = pFile = fopen(".\\ini\\archtest.ini", "r");
	if( pFile )
	{
		char strBuffer[512];
		fscanf( pFile, "%s", strBuffer );				//REG_KEY_NAME_ARCHLORD
		strcpy( m_strRegKeyBase, strBuffer );
		fscanf( pFile, "%s", strBuffer );				//Patch2.archlord.com
		strcpy( m_strPatchServerIP, strBuffer );
		fscanf( pFile, "%s", strBuffer );				//Login2.archlord.com
		strcpy( m_strLoginServerIP, strBuffer );
		fclose(pFile);
	}
#endif

	m_bVersionFromBackup	= FALSE;
	m_iSelfPatchFlag		= 0;
}

CPatchClientRegistry::~CPatchClientRegistry()
{
}

BOOL CPatchClientRegistry::GetProfileDword( DWORD& dwValue,  LPCTSTR cszSection, LPCTSTR cszEntry, DWORD dwDefault )
{
	dwValue = dwDefault;

	CRegKey	regKey;
	char	szRegKey[ MAX_TEMP_BUFFER_SIZE ];
	char	szValue[ MAX_TEMP_BUFFER_SIZE ];
	DWORD	dwType;
	DWORD	dwBytes	= MAX_TEMP_BUFFER_SIZE;

	sprintf( szRegKey, "%s%s", m_strRegKeyBase, cszSection );

	if ( ERROR_SUCCESS == regKey.Open( ALEF_REGKEY_HKEY, szRegKey )  )
	{
		if ( ERROR_SUCCESS == regKey.QueryValue( cszEntry, &dwType, szValue, &dwBytes ) )
		{
			dwValue	= atoi( szValue );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CPatchClientRegistry::SetProfileDword( LPCTSTR cszSection, LPCTSTR cszEntry, DWORD dwValue )
{
	char	szRegKey[ MAX_TEMP_BUFFER_SIZE ];
	sprintf( szRegKey, "%s%s", m_strRegKeyBase, cszSection );

	CRegKey	regKey;
	if ( ERROR_SUCCESS != regKey.Open( ALEF_REGKEY_HKEY, szRegKey ) )
		if ( ERROR_SUCCESS != regKey.Create( ALEF_REGKEY_HKEY, szRegKey ) )
			return FALSE;

	return ERROR_SUCCESS == regKey.SetValue( cszEntry, REG_DWORD, &dwValue, sizeof(dwValue) ) ? TRUE : FALSE;
}

BOOL CPatchClientRegistry::GetUseDriverUpdate()
{
	DWORD dwResult = 0;
	GetProfileDword( dwResult, "", "UseDriverUpdate", TRUE );
	m_bUseDriverUpdate = (BOOL)dwResult;
	return m_bUseDriverUpdate;
}

void CPatchClientRegistry::SetUseDriverUpdate( BOOL bUse )
{
	m_bUseDriverUpdate = bUse;
	SetProfileDword( "", "UseDriverUpdate", (DWORD)m_bUseDriverUpdate );
}

void CPatchClientRegistry::SetRegKeyBase( char *pstrRegkey )
{
	bool		bResult;

	int iIPSize = (int) strlen( pstrRegkey ) + 1; //NULL문자포함
	if( iIPSize > sizeof(m_strRegKeyBase) )
	{
		bResult = false;
	}
	else
	{
		memcpy( m_strRegKeyBase, pstrRegkey, iIPSize );
		bResult = true;
	}
}


bool CPatchClientRegistry::SetPatchInfo( char *pstrIP, int iPort )
{
	bool		bResult;

	int	iIPSize = (int) strlen( pstrIP ) + 1; //NULL문자포함
	if( iIPSize > sizeof(m_strPatchServerIP) )
	{
		bResult = false;
	}
	else
	{
		memcpy( m_strPatchServerIP, pstrIP, iIPSize );
		bResult = true;
	}

	m_iPatchServerPort = iPort;

	return bResult;
}

bool CPatchClientRegistry::SetLoginInfo( char *pstrIP, int iPort )
{
	bool		bResult;
	int			iIPSize;

	iIPSize = (int) strlen( pstrIP ) + 1; //NULL문자포함

	if( iIPSize > sizeof(m_strLoginServerIP) )
	{
		bResult = false;
	}
	else
	{
		memcpy( m_strLoginServerIP, pstrIP, iIPSize );
		bResult = true;
	}

	m_iLoginServerPort = iPort;

	return bResult;
}

bool CPatchClientRegistry::CleanRegistry( char *pstrSubKey )
{
	if( GetVersion() < 0x80000000 )              // Windows NT/2000/XP
	{
		//NT,2K,XP일경우는 서브키가 있으면 안지워지므로 서브키를 다 지우고 시도해야한다.
		SHDeleteKey( ALEF_REGKEY_HKEY, pstrSubKey );
	}
	else                               // Windows 95/98/Me
	{
		//95,98,ME일경우는 subkey가 있어도 지운다.
		RegDeleteKey( ALEF_REGKEY_HKEY, pstrSubKey );
	}

	return true;
}

bool CPatchClientRegistry::InitRegistry()
{
	RestoreRegistry();

#ifdef _CHN
	//레지스트리가 제대로 존재하는지 확인한다.
	if( m_bVersionFromBackup )
		return true;
#endif

	if( GetRegistryInfo( m_strRegKeyBase ) )
		return true;

	CleanRegistry( m_strRegKeyBase );

	return BuildRegistry( m_strRegKeyBase ) && GetRegistryInfo( m_strRegKeyBase ) ? true : false;
}

bool CPatchClientRegistry::FinishPatch( int iVersion , int iCheckCode)
{
	HKEY	hRegKey;
	if( ERROR_SUCCESS != RegOpenKeyEx( ALEF_REGKEY_HKEY, m_strRegKeyBase, 0, KEY_WRITE, &hRegKey ) )
	{
		BackupRegistry();
		return false;
	}

	m_iVersion			= iVersion;
	m_iPatchCheckCode	= iCheckCode ? iCheckCode : 0;

	RegSetValueEx( hRegKey, "Version", 0, REG_DWORD, (unsigned char*)&m_iVersion, sizeof(m_iVersion) );
	RegSetValueEx( hRegKey, "Code", 0, REG_DWORD, (unsigned char*)&m_iPatchCheckCode, sizeof(m_iPatchCheckCode) );
	RegSetValueEx( hRegKey, sREG_SELF_PATCH_FLAG, 0, REG_DWORD, (unsigned char*) &m_iSelfPatchFlag, sizeof(m_iSelfPatchFlag) );
	RegCloseKey( hRegKey );

	BackupRegistry();

	return true;
}

bool CPatchClientRegistry::GetRegistryInfo( char *pstrSubKey )
{
	HKEY hRegKey;
	if ( ERROR_SUCCESS != RegOpenKeyEx( ALEF_REGKEY_HKEY, pstrSubKey, 0, KEY_READ, &hRegKey ) )
		return false;

	DWORD	iType;
	DWORD	iDataLen = sizeof( m_iVersion );
	RegQueryValueEx( hRegKey, "Version", 0, &iType, (unsigned char*)&m_iVersion, &iDataLen );
	iDataLen = sizeof( m_strPatchServerIP );
	RegQueryValueEx( hRegKey, "PatchServerIP", 0, &iType, (unsigned char *)m_strPatchServerIP, &iDataLen );
	iDataLen = sizeof( m_iPatchServerPort );
	RegQueryValueEx( hRegKey, "PatchServerPort", 0, &iType, (unsigned char*)&m_iPatchServerPort, &iDataLen );
	iDataLen = sizeof( m_iLoginServerPort );
	RegQueryValueEx( hRegKey, "LoginServerPort", 0, &iType, (unsigned char*)&m_iLoginServerPort, &iDataLen );
	iDataLen = sizeof( m_iPatchCheckCode );
	RegQueryValueEx( hRegKey, "Code", 0, &iType, (unsigned char*)&m_iPatchCheckCode, &iDataLen );
	iDataLen = sizeof(m_iSelfPatchFlag);
	RegQueryValueEx( hRegKey, sREG_SELF_PATCH_FLAG, 0, &iType, (unsigned char*) &m_iSelfPatchFlag, &iDataLen);

	RegCloseKey( hRegKey );

	return true;
}


bool CPatchClientRegistry::BuildRegistry( char *pstrSubKey, bool bReset )
{
	HKEY hRegKey;
	if( ERROR_SUCCESS != RegOpenKeyEx( ALEF_REGKEY_HKEY, pstrSubKey, 0, KEY_WRITE, &hRegKey ) )
	{
		if ( ERROR_SUCCESS != RegCreateKey( ALEF_REGKEY_HKEY, pstrSubKey, &hRegKey ) )
			return false;

		int iVer = 0;
		RegSetValueEx( hRegKey, "Version", 0, REG_DWORD, (unsigned char*)&iVer, (DWORD) sizeof(iVer) );

		int iPatchCheckCode = 0;
		RegSetValueEx( hRegKey, "Code", 0, REG_DWORD, (unsigned char*)&iPatchCheckCode, (DWORD) sizeof( iPatchCheckCode ) );
	}

	if( bReset )
	{
		int iVer = 0;
		RegSetValueEx( hRegKey, "Version", 0, REG_DWORD, (unsigned char*)&iVer, (DWORD) sizeof(iVer) );

		int iPatchCheckCode = 0;
		RegSetValueEx( hRegKey, "Code", 0, REG_DWORD, (unsigned char*)&iPatchCheckCode, (DWORD) sizeof( iPatchCheckCode ) );
	}

	RegSetValueEx( hRegKey, "PatchServerIP", 0, REG_SZ, (unsigned char*)m_strPatchServerIP, (DWORD) strlen( m_strPatchServerIP ) );
	RegSetValueEx( hRegKey, "PatchServerPort", 0, REG_DWORD, (unsigned char*)&m_iPatchServerPort, (DWORD) sizeof( m_iPatchServerPort ) );
	RegSetValueEx( hRegKey, "LoginServerIP", 0, REG_SZ, (unsigned char*)m_strLoginServerIP, (DWORD) strlen( m_strLoginServerIP ) );
	RegSetValueEx( hRegKey, "LoginServerPort", 0, REG_DWORD, (unsigned char*)&m_iLoginServerPort, (DWORD) sizeof( m_iLoginServerPort ) );
	RegSetValueEx( hRegKey, sREG_SELF_PATCH_FLAG, 0, REG_DWORD, (unsigned char*) &m_iSelfPatchFlag, (DWORD) sizeof(m_iSelfPatchFlag) );

	return true;
}

bool	CPatchClientRegistry::BuildPathRegistry( char* pstrSubKey )
{
	HKEY hRegKey;

	//레지스트리가 없다면 만들어준다.
	if( ERROR_SUCCESS != RegOpenKeyEx( ALEF_REGKEY_HKEY, pstrSubKey, 0, KEY_WRITE, &hRegKey ) )
		if ( ERROR_SUCCESS != RegCreateKey( ALEF_REGKEY_HKEY, pstrSubKey, &hRegKey ) )
			return false;

	const char	keynameDir[]	= "folder";
	const char	keynameFile[]	= "file";

	// 한게임용 레지스트리 설정함..
	char strDir[ 2048 ] = "";
	GetCurrentDirectory( 2048 , strDir );
	RegSetValueEx( hRegKey, keynameDir, 0, REG_SZ, (unsigned char*)strDir, (DWORD) strlen( strDir ) );
	char strFile[ 2048 ] = "Archlord.exe";
	RegSetValueEx( hRegKey, keynameFile, 0, REG_SZ, (unsigned char*)strFile, (DWORD) strlen( strFile ) );

	RegCloseKey(hRegKey);

	return true;
}

void CPatchClientRegistry::BackupRegistry()
{
	FILE *			fp;
	HKEY			hRegKey;

	if (!(fp = fopen(ALEF_REGISTRY_FILE, "wt")))
		return;

	//Key에서 값을 읽는다. 이번엔 읽어야한다. -_-;
	if( ERROR_SUCCESS == RegOpenKeyEx( ALEF_REGKEY_HKEY, m_strRegKeyBase, 0, KEY_READ, &hRegKey ) )
	{
		DWORD			iType;
		DWORD			iDataLen;
		DWORD			iPatchCheckCode;
		DWORD			iVersion;

		//Version
		iDataLen = sizeof( iVersion );
		if( ERROR_SUCCESS != RegQueryValueEx( hRegKey, "Version", 0, &iType, (unsigned char*)&iVersion, &iDataLen ) )
			AfxMessageBox("BackupRegistry() Error No Version Info!!!", MB_OK);
		else
			fprintf(fp, "Version=%d\n", iVersion);

		//iPatchCheckCode
		iDataLen = sizeof( iPatchCheckCode );
		if( ERROR_SUCCESS != RegQueryValueEx( hRegKey, "Code", 0, &iType, (unsigned char*)&iPatchCheckCode, &iDataLen ) )
			AfxMessageBox("BackupRegistry() Error No Code Info!!!", MB_OK);
		else
			fprintf(fp, "Code=%d\n", iPatchCheckCode);

		RegCloseKey(hRegKey); 
	}

	fclose(fp);
}

void CPatchClientRegistry::RestoreRegistry()
{
	FILE *			fp;
	HKEY			hRegKey;
	char			szLine[128];
	char *			szValue;
	size_t			nLength;

	if (!(fp = fopen(ALEF_REGISTRY_FILE, "rt")))
		return;

	m_bVersionFromBackup = TRUE;

	//레지스트리가 없다면 만들어준다.
	if( ERROR_SUCCESS != RegOpenKeyEx( ALEF_REGKEY_HKEY, m_strRegKeyBase, 0, KEY_WRITE, &hRegKey ) )
		RegCreateKey( ALEF_REGKEY_HKEY, m_strRegKeyBase, &hRegKey );

	while (fgets(szLine, 128, fp))
	{
		szValue = strchr(szLine, '=');
		if (!szValue)
			continue;

		*szValue = 0;

		++szValue;
		nLength = strlen(szValue);

		while ((nLength--))
		{
			if (szValue[nLength] == '\r' || szValue[nLength] == '\n')
				szValue[nLength] = 0;
			else
				break;
		}

		if (!strcmp(szLine, "Version"))
		{
			m_iVersion = atoi(szValue);
			RegSetValueEx( hRegKey, "Version", 0, REG_DWORD, (unsigned char*)&m_iVersion, (DWORD) sizeof(m_iVersion) );
		}
		else if (!strcmp(szLine, "Code"))
		{
			m_iPatchCheckCode = atoi(szValue);
			RegSetValueEx( hRegKey, "Code", 0, REG_DWORD, (unsigned char*)&m_iPatchCheckCode, (DWORD) sizeof( m_iPatchCheckCode ) );
		}
		else if ( !strcmp(szLine, sREG_SELF_PATCH_FLAG) )
		{
			m_iSelfPatchFlag = atoi(szValue);
			RegSetValueEx( hRegKey, sREG_SELF_PATCH_FLAG, 0, REG_DWORD, (unsigned char*) &m_iSelfPatchFlag, (DWORD) sizeof(m_iSelfPatchFlag) );
		}
	}

	RegCloseKey(hRegKey); 

	fclose(fp);
}

int CPatchClientRegistry::GetDXVersionFromReg()
{
	char	strBuffer[256];
	sprintf( strBuffer, REG_KEY_NAME_DIRECTX );

	HKEY	hRegKey;
	if( ERROR_SUCCESS != RegOpenKeyEx( ALEF_REGKEY_HKEY, strBuffer, 0, KEY_READ, &hRegKey ) )
		return 0;

	char	strDXVersion[80];
	DWORD	iType;
	DWORD	iDataLen = sizeof(strDXVersion);
	memset( strDXVersion, 0, sizeof(strDXVersion) );

	if( ERROR_SUCCESS != RegQueryValueEx( hRegKey, "Version", 0, &iType, (unsigned char*)&strDXVersion, &iDataLen ) )
		goto FAIL;

	RegCloseKey(hRegKey); 

	int	iVersion1, iVersion2, iVersion3, iVersion4;
	sscanf( strDXVersion, "%d.%d.%d.%d", &iVersion1, &iVersion2, &iVersion3, &iVersion4 );

	return iVersion1 == 4 ? iVersion4 : 0;

FAIL:
	RegCloseKey(hRegKey);
	return 0;
}
