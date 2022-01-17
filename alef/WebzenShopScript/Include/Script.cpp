#include "Script.h"

namespace Webzen
{
	//-----------------------------------------------------------------------
	//

	Script & Script::Instance()
	{
		static Script inst;
		return inst;
	}

	//-----------------------------------------------------------------------
	//

	bool Script::LoadScript()
	{
		FILE * f = fopen( "AlphaTest.arc", "rb" );

		char * serverPath = "/ibs/Game/ProductTransfer";

		char * domain = "image.webzen.co.kr";

		if( f )	{
			serverPath = "/ibs/Game/Kor/ProductTransfer";
			domain = "alpha-image.webzen.co.kr";
			fclose(f);
		}

		SetListManagerInfo( HTTP, domain, "", "", serverPath, "./ShopData", versionInfo );

		WZResult result = LoadScriptList( true );

		if( !result.IsSuccess() )
			return false;

		if( !GetListPtr() )
			return false; 

		return true;
	}

	//-----------------------------------------------------------------------
	//

	Script::~Script()
	{
		//DeleteFiles();
	}

	//-----------------------------------------------------------------------
	//

	CShopList *	Script::GetList()
	{
		return Instance().GetListPtr();
	}

	//-----------------------------------------------------------------------
	//

	bool Script::SetVersion( unsigned short year, unsigned short yearId )
	{
		CListVersionInfo & versionInfo = Instance().versionInfo;

		if( versionInfo.year != year || versionInfo.yearId != yearId )
		{
			versionInfo.year = year;
			versionInfo.yearId = yearId;
			versionInfo.Zone = 825; // 아크로드 국내 세일즈 존

			return Instance().LoadScript();
		}

		return false;
	}

	//-----------------------------------------------------------------------
	//

	bool Script::DeleteFiles( void )
	{
		bool result = true;

		try
		{
			char strCurrentPath[ 4096 ] = { 0, };
			::GetCurrentDirectory( sizeof( char ) * 256, strCurrentPath );

			char strDeletePath[ 2048 ] = { 0, };
			sprintf_s( strDeletePath, sizeof( char ) * 256, "%s\\%s", strCurrentPath, "ShopData" );

			DeleteFolder( strDeletePath );
		}
		catch(...)
		{
			result = false;
		}

		return result;
	}

	//-----------------------------------------------------------------------
	//

	bool Script::DeleteFolder( char* pFolder )
	{
		if( !pFolder || strlen( pFolder ) <= 0 ) return false;

		WIN32_FIND_DATA Find;
		HANDLE hHandle = ::FindFirstFile( pFolder, &Find );
		if( hHandle == INVALID_HANDLE_VALUE ) return false;

		while( ::FindNextFile( hHandle, &Find ) )
		{
			if( strcmp( Find.cFileName, "." ) == 0 || strcmp( Find.cFileName, ".." ) == 0 ) continue;

			if( Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				// 하위디렉토리 재귀호출로 삭제
				char strSubFolder[ 256 ] = { 0, };
				sprintf_s( strSubFolder, sizeof( char ) * 256, "%s\\%s", pFolder, Find.cFileName );

				if( !DeleteFolder( strSubFolder ) ) return false;
			}

			char strFileName[ 256 ] = { 0, };
			sprintf_s( strFileName, sizeof( char ) * 256, "%s\\%s", pFolder, Find.cFileName );

			if( !::DeleteFile( strFileName ) )
			{
				DWORD dwErrorCode = ::GetLastError();
			}
		}

		FindClose( hHandle );

		// 반드시 핸들을 닫고서 폴더를 삭제해야 삭제가 된댑니다 MSDN 님께서
		if( !::RemoveDirectory( pFolder ) )
		{
			DWORD dwErrorCode = ::GetLastError();
		}

		return true;
	}

	//-----------------------------------------------------------------------
}