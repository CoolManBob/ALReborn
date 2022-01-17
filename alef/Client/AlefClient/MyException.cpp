#include "MyException.h"
#include "MyEngine.h"

#include "AcuDeviceCheck.h"
#include "CSLog.h"

const DWORD CMyException::s_dwOption = /*GSTSO_PARAMS |*/GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE;
MyEngine* CMyException::m_pEngine = NULL;
LPTOP_LEVEL_EXCEPTION_FILTER CMyException::m_pExceptionFilter = NULL;

CMyException::CMyException( MyEngine* pEngine )
{
	m_pEngine			= pEngine;
	m_pExceptionFilter	= SetUnhandledExceptionFilter( ExceptionHandler );
}

CMyException::~CMyException()
{
	if( m_pExceptionFilter )
	{
		SetUnhandledExceptionFilter( m_pExceptionFilter );
		m_pExceptionFilter = NULL;
	}
}

// 디버그 정보추가
extern UINT32 g_uDebugLastRemoveCharacterTID;		// AgcmCharacter.cpp
extern UINT32 g_uRemoveUpdateInfoFromClump2Error;	// AgcmRender.cpp
LONG __stdcall CMyException::ExceptionHandler( EXCEPTION_POINTERS* pExPtrs )
{
	return -1;
	/*// 우선 모드 돌려놓기
	((MyEngine*)m_pEngine)->m_pMainWindow->ReturnScreenMode();

	// some network-related clean-ups
	INT16	nSize;
	PVOID	pvPacket = m_pEngine->MakeSPClientExit(&nSize);
	if ( pvPacket )
	{
		m_pEngine->SendPacket(pvPacket, nSize);
		m_pEngine->FreeSystemPacket(pvPacket);
	}

	m_pEngine->m_SocketManager.DisconnectAll();

	// additional log for graphics card
	char buf[65536];
	sprintf(buf, "Graphics card: %s\r\nDriver version: %d.%d.%d.%d\r\nVendor ID: %d\r\nDevice ID: %d\r\n", 
					AcuDeviceCheck::m_strDeviceName,
					AcuDeviceCheck::m_iProduct,
					AcuDeviceCheck::m_iVersion,
					AcuDeviceCheck::m_iSubVersion,
					AcuDeviceCheck::m_iBuild,
					AcuDeviceCheck::m_iVendorID,
					AcuDeviceCheck::m_iDeviceID);

	//@{ Jaewon 20050211
	// Self character infos
	char buf2[512];
	buf2[0] = '\0';
	sprintf(buf2, "\r\nPlayer character:\r\n" );
	strcat(buf, buf2);
	if(g_pcsAgcmCharacter)
	{
		__try
		{
			AgpdCharacter *pAgpdCharacter = g_pcsAgcmCharacter->GetSelfCharacter();
			if( pAgpdCharacter )
			{
				sprintf(buf2, 	"m_szID = %s\r\n"
								"m_stPos = (%f,%f,%f)\r\n"
								"m_unActionStatus = %d\r\n"
								"m_bIsTrasform = %s\r\n"					,
								pAgpdCharacter->m_szID						,
								pAgpdCharacter->m_stPos.x					,
								pAgpdCharacter->m_stPos.y					,
								pAgpdCharacter->m_stPos.z					,
								pAgpdCharacter->m_unActionStatus			,
								pAgpdCharacter->m_bIsTrasform?"TRUE":"FALSE");

			}
			else
			{
				sprintf( buf2, "No Self Character Info\r\n" );
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			sprintf(buf2, "\r\nException encountered during play character info logging.\r\n");
		}

		buf2[511] = '\0';

		strncat(buf, buf2, sizeof(buf) - strlen(buf) - 1);

		sprintf( buf2, "LastRemoveCharacterTID =%d\r\n", g_uDebugLastRemoveCharacterTID );
		strncat( buf, buf2, sizeof(buf) - strlen(buf) - 1);
		sprintf( buf2, "RemoveUpdateInfoFromClump2Error = %d\r\n", g_uRemoveUpdateInfoFromClump2Error );
		strncat( buf, buf2, sizeof(buf) - strlen(buf) - 1 );

		DebugValueManager* pManager = DebugValueManager::GetInstance();
		for( vector< DebugValue * >::iterator Itr = pManager->vecValue.begin(); Itr != pManager->vecValue.end(); ++Itr )
		{
			(*Itr)->LogString( buf2 );
			strcat( buf2 , "\r\n" ); // 개행 문자 추가.
			strncat( buf, buf2, sizeof(buf) - strlen(buf) - 1 );
		}

		CSLogManager::Print( buf );		// 추가정보.. 콜스텍 정보 있으면 추가.
	}

	DWORD	dwClientVersion = 0;
	HKEY	hRegKey;
	if( !RegOpenKeyEx( HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Archlord_KR", 0, KEY_READ, &hRegKey ) )
	{
		
		DWORD iDataLen = sizeof( dwClientVersion );
		DWORD iType;
		RegQueryValueEx( hRegKey, "Version", 0, &iType, (unsigned char*)&dwClientVersion, &iDataLen );
	}

	sprintf( buf2, "\r\nClient Version: %d",dwClientVersion );
	strcat( buf, buf2 );

	// 서비스 지역 정보 추가.
	sprintf( buf2, "\r\nService Area: %d\r\n", g_eServiceArea );
	strcat( buf, buf2 );

	return RecordExceptionInfo( pExPtrs, "AlefClient", buf, &AddExceptionHandler );*/
}

extern void InitSymEng();
extern void CleanupSymEng();
const char* CMyException::AddExceptionHandler( EXCEPTION_POINTERS* pExPtrs )
{
	// additional stack trace, but this needs the release of pdb
	static char buf[10240];
	const UINT32 limit = sizeof(buf)/sizeof(buf[0])-1;
	buf[limit] = '\0';
	strcpy(buf, "Debug log:\r\n");
	strncat(buf, MD_GetErrorMessage(), limit - strlen(buf));
	strncat(buf, "\r\n", limit-strlen(buf));

	char tmp[1025];
	tmp[1024] = '\0';

	strncat(buf, "\r\n", limit - strlen(buf));
	sprintf(tmp, "Crash Address:\r\n");
	strncat(buf, tmp, limit - strlen(buf));
	sprintf(tmp, "%08x\r\n\r\n" , pExPtrs->ExceptionRecord->ExceptionAddress );
	strncat(buf, tmp, limit - strlen(buf));

	sprintf(tmp, "Stack trace:\r\n");
	strncat(buf, tmp, limit - strlen(buf));

	InitSymEng();

	const char *szBuff = GetFirstStackTraceString( s_dwOption, pExPtrs );
	do
	{
		_snprintf( tmp, sizeof(tmp)/sizeof(tmp[0])-1, "%s\r\n", szBuff );
		strncat( buf, tmp, limit-strlen(buf) );
		szBuff = GetNextStackTraceString( s_dwOption, pExPtrs );
	}
	while(szBuff);
	
	CleanupSymEng();

	return buf;
}

