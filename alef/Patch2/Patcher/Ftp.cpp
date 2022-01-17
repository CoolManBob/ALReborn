#include "stdafx.h"
#include "Ftp.h"

bool CPatcherFTP::UploadFile( const char *pstrFTPURL, const char *pstrID, const char *pstrPassword, char *pstrPath, char *pstrFile, int lPort, BOOL bPassive )
{
	CInternetSession		cSession;
	CFtpConnection			*pcsFTPConnection;

	bool			bResult;
	bool			bConnectToFTP;

	bResult = false;
	bConnectToFTP = false;

	try
	{
		pcsFTPConnection = (CFtpConnection *)cSession.GetFtpConnection( pstrFTPURL, pstrID, pstrPassword, lPort, bPassive ); 

		if( pcsFTPConnection )
		{
			bConnectToFTP = TRUE;
		}
		else
		{
			TCHAR	szErr[MAX_PATH]	= "";
			sprintf( szErr, "cSession.GetFtpConnection( %s, %s, %s, %d, %s ) failed"
				, pstrFTPURL, pstrID, pstrPassword, lPort, bPassive ? "TRUE" : "FALSE" );
			::AfxMessageBox( szErr, MB_OK, 0 );
		}
	}
	catch(CInternetException * pEx)
	{
		TCHAR			lpszError[512];

		pEx->GetErrorMessage( lpszError, 256, NULL );
		//@{ kday 20050629
		// ;)
		::AfxMessageBox( lpszError, MB_OK, 0 );
		//@} kday
	}

	if( bConnectToFTP )
	{
		BOOL		br	= FALSE;
		char		strFileName[255];
		FILE *		fp;

		sprintf( strFileName, "%s/%s", pstrPath, pstrFile );

		br	= pcsFTPConnection->Remove( pstrFile );

		fp = fopen(strFileName, "rb");
		if (fp)
		{
			br	= pcsFTPConnection->PutFile( strFileName, pstrFile );
			if( !br )
			{
				DWORD errcode = GetLastError();
				TCHAR	szErr[MAX_PATH]	= "";
				sprintf( szErr, "pcsFTPConnection->PutFile( %s, %s ) failed! GetLastError()=%d"
					, strFileName, pstrFile, errcode );
				::AfxMessageBox( szErr, MB_OK, 0 );
			}

			fclose(fp);
		}

		//@{ kday 20050630
		// ;)
		////Complete파일 삭제.
		//br	= pcsFTPConnection->Remove( "complete" );
		////@{ kday 20050629
		//// ;)
		//if( !br )
		//{
		//	DWORD errcode = GetLastError();
		//	TCHAR	szErr[MAX_PATH]	= "";
		//	sprintf( szErr, "pcsFTPConnection->Remove( complete ) failed! GetLastError()=%d"
		//		, errcode );
		//	::AfxMessageBox( szErr, MB_OK, 0 );
		//}
		////@} kday
		//@} kday

		cSession.Close();

		bResult = true;
	}

	return bResult;
}

bool CPatcherFTP::MakeFolder( CFtpConnection *pcsFTPConnection, char *pstrPath )
{
	bool				bResult;

	bResult = false;

	if( pcsFTPConnection && pstrPath )
	{
		//폴더 이름이 "root"이 아닌경우만 폴더를 세팅한다.
		if( stricmp( "root", pstrPath ) )
		{
			if( pcsFTPConnection->SetCurrentDirectory( pstrPath ) == FALSE )
			{
				int			lStartPos = 0;
				char		strFolderName[255];

				int lCounter;
				for( lCounter=0; lCounter< ( int ) strlen(pstrPath); lCounter++ )
				{
					if( pstrPath[lCounter] == '\\' )
					{
						memset( strFolderName, 0, sizeof(strFolderName) );
						memcpy( strFolderName, &pstrPath[lStartPos], lCounter-lStartPos );

						pcsFTPConnection->CreateDirectory( strFolderName );
						pcsFTPConnection->SetCurrentDirectory( strFolderName );

						lCounter++;
						lStartPos = lCounter;
					}
				}

				if( lStartPos != lCounter )
				{
					memset( strFolderName, 0, sizeof(strFolderName) );
					memcpy( strFolderName, &pstrPath[lStartPos], lCounter-lStartPos );

					pcsFTPConnection->CreateDirectory( strFolderName );
					pcsFTPConnection->SetCurrentDirectory( strFolderName );
				}
			}
		}

		bResult = true;
	}

	return bResult;
}
