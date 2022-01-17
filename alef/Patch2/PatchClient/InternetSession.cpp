#include "stdafx.h"
#include "InternetSession.h"
#include "AutoDetectMemoryLeak.h"

#define FTP_BUFFER_SIZE	1024

bool CInternetLib::ConnectToFTP( char *pstrAddr, char *pstrID, char *pstrPW, int lPort )
{
	bool bResult = false;
	try
	{
		m_pcsFTPConnection = (CFtpConnection*)m_cSession.GetFtpConnection( pstrAddr, pstrID, pstrPW, lPort ); 
		bResult = true;
	}
    catch(CInternetException* e)
    {
        //TCHAR szError[512];
        //e->GetErrorMessage( szError, 256, NULL );
        //MessageBox( szError );
    }

	return bResult;
}

bool CInternetLib::GetFileFromFTP()
{
	if( !m_pcsFTPConnection )	return false;

	bool			bResult;

	bResult = FALSE;

	FILE*			fp;
	CInternetFile*	pcsInternetFile;
	char			szFileName[256];
	int				nRead;
	int				nFileSize;
	int				iTotalRecvByte = 0;
	char			szData[FTP_BUFFER_SIZE];

	while( 1 )
	{
		pcsInternetFile	= m_pcsFTPConnection->OpenFile( szFileName );
		nFileSize		= (int)pcsInternetFile->GetLength();
		pcsInternetFile->SetReadBufferSize( FTP_BUFFER_SIZE );

		fp = fopen( szFileName, "wb" );
		if( fp )
		{
			while(1) 
			{ 
				nRead = pcsInternetFile->Read( szData, FTP_BUFFER_SIZE );
				if( !nRead )				break;

				iTotalRecvByte += nRead;

				int	iWriteBytes = (int)fwrite( szData, 1, nRead, fp );
				if( iWriteBytes != nRead )	break;

				if( nRead < FTP_BUFFER_SIZE && nFileSize != iTotalRecvByte )
					break;
			} 

			fclose( fp );
		}
	}

	m_pcsFTPConnection->Close();

	return true;
}
