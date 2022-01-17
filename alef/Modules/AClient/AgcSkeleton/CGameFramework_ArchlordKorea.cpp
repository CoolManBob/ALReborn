#include "CGameFramework_ArchlordKorea.h"
#include "CGameFramework_Utility.h"
#include "stdio.h"


CGameFramework_ArchlordKorea::CGameFramework_ArchlordKorea( void )
{
}

CGameFramework_ArchlordKorea::~CGameFramework_ArchlordKorea( void )
{
}

BOOL CGameFramework_ArchlordKorea::OnGameStart( void )
{
	// Step 1. 전역변수 초기화 ( rsINITIALIZE, psInitialize() );

	// Step 2. 타임카운터 초기화, 윈도우 생성 준비 ( InitApplication );

	// Step 3. 실행 파라미터 검사, 게임스트링과 패치코드를 검사한다.
	if( !ParseCommandLine() ) return FALSE;

	// Step 4. 윈도우 생성

	// Step 5. 윈도우 단축키 후킹

	// Step 6. 렌더링 디바이스 생성

	return TRUE;
}

BOOL CGameFramework_ArchlordKorea::OnGameLoop( void )
{
	return TRUE;
}

BOOL CGameFramework_ArchlordKorea::OnGameEnd( void )
{
	return TRUE;
}

BOOL CGameFramework_ArchlordKorea::ParseCommandLine( void )
{
	char* pCommand = ::GetCommandLine();
	if( !pCommand || strlen( pCommand ) <= 0 ) return FALSE;

	int nArgumentCount = 0;
	char** ppArguments = FrameworkUtil::CommandLineToArgv( &nArgumentCount );
	if( !ppArguments || nArgumentCount <= 0 ) return FALSE;

	// 패치코드 검사
	if( !CheckPatchCode( nArgumentCount >= 2 ? ppArguments[ 1 ] : NULL ) )
	{
		::MessageBox( NULL, "Cannot run AlefClient.exe alone.", "Invalid Client running", MB_OK );
		return FALSE;
	}

	FrameworkUtil::DeleteCommandLine( ppArguments, nArgumentCount );
	return TRUE;
}
