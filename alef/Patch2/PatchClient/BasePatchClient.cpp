#include "StdAfx.h"
#include "BasePatchClient.h"

BasePatchClient::BasePatchClient( CDialog* pDlg )
:	m_pDlg( pDlg )
{
	m_nCurrentServerListCategory	=	0;
	m_bShowPingTime					=	FALSE;
	m_bForceExit					=	FALSE;
}

BasePatchClient::~BasePatchClient(void)
{
	Destroy();
}

VOID BasePatchClient::Destroy( VOID )
{
	m_cPatchClientLib.m_bExit = true;
	m_cPatchClientLib.cleanupWinsock();
	m_cPatchClientLib.m_csAuPackingManager.Destroy();

	WaitForTermination();
}

HBITMAP BasePatchClient::LoadBitmapResource( const char* strFileName )
{

	BOOL	bPackingFile		=	TRUE;

	AuPackingManager& cPackingManager = m_cPatchClientLib.m_csAuPackingManager;
	cPackingManager.SetFilePointer();

	ApdFile		csApdFile;

	if( !cPackingManager.OpenFile( (char*)strFileName , &csApdFile) )
	{	
		cPackingManager.CloseAllFileHandle();
		return NULL;
	}

	UINT32		uLength = cPackingManager.GetFileSize( &csApdFile );
	BYTE		*pBuffer = new BYTE[uLength];
	ZeroMemory( pBuffer , uLength );
	

	if( cPackingManager.ReadFile( pBuffer, uLength, &csApdFile ) != uLength )
	{		
		cPackingManager.CloseFile(&csApdFile);
		cPackingManager.CloseAllFileHandle();
		return NULL;
	}


	// 패킹된 파일인지 아닌지 확인한다
	// fopen으로 열린다면 패킹된 파일이 아니다
	FILE*	pFile		=	fopen( strFileName , "rb" );
	if( pFile )
	{
		bPackingFile	=	FALSE;
		fclose( pFile );
	}


	cPackingManager.CloseFile(&csApdFile);

	if( bPackingFile )
	{
		FILE * pFile = fopen( strFileName , "wb" );

		if( !pFile )
		{
			cPackingManager.CloseAllFileHandle();
			return NULL;
		}

		fwrite( pBuffer , sizeof BYTE , uLength , pFile );
		fclose( pFile );
	}

	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, strFileName , IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE);

	if( bPackingFile )
		DeleteFile( strFileName );

	cPackingManager.CloseAllFileHandle();
	delete [] pBuffer;

	return hBitmap;
}

VOID BasePatchClient::ResetScrollPos( VOID )
{
	CWnd *pcTempWnd = m_pDlg->GetDlgItem( IDC_ScrollThumbBtn );
	pcTempWnd->SetWindowPos( NULL, LISTSCROLL_POSTION_X, 310, 14, 14, SWP_SHOWWINDOW );
}

VOID BasePatchClient::Run( VOID )
{
	switch( m_nOperationThread )
	{
	case PATCH_THREAD:			StartPatch();		break;
	case PING_THREAD:			StartPing();		break;
	}
}

VOID BasePatchClient::PatchThreadStart( VOID )
{

	wchar_t strMsg[256];
	::LoadStringW(g_resInst, IDS_MSG_CONNECTING, strMsg, 256);

	m_cPatchClientLib.InvalidateStatic( &m_StaticStatus		, strMsg );
	m_cPatchClientLib.InvalidateStatic( &m_StaticDetailInfo	, strMsg );

#ifdef _FOR_GM
	//패치성공사실을 알린다.
	CWnd* pcWnd = NULL;

	::LoadStringW(g_resInst, IDS_MSG_COMPLETE, strMsg, 256);
	m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticDetailInfo, strMsg );

	::LoadStringW(g_resInst, IDS_RUN_GAME, strMsg, 256);
	m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticStatus, strMsg );

	if (m_cPatchOptionFile.m_bSaveLoad)
	{
		pcWnd = GetDlgItem( IDC_OptionButton );
		pcWnd->EnableWindow();
	}

	if ( pcWnd )
	{
		pcWnd = GetDlgItem( IDC_START_GAME );
		pcWnd->EnableWindow();
	}

#else
	m_nOperationThread			=	PATCH_THREAD;
	Start();
#endif

}

VOID BasePatchClient::PingThreadStart( VOID )
{
	m_nOperationThread			=	PING_THREAD;
	
	Start();
}

VOID BasePatchClient::StartPing( VOID )
{
	m_cPatchClientLib.DownloadAreaConfig();

	if ( m_bShowPingTime )			m_cPatchClientLib.CheckPingTime();
}

VOID BasePatchClient::StartPatch( VOID )
{
	wchar_t strMsg[256] = {0, };

	if( m_cPatchClientLib.m_cPatchReg.InitRegistry() )
	{		
		//Server로 연결.
		bool bConnect	= Connect();

		if( m_cPatchClientLib.m_lErrorCode & 1 )
		{
			::MessageBoxW(m_pDlg->GetSafeHwnd(), L"StartupWinsock Error", MESSAGE_TITLE, MB_OK);
		}
		if( m_cPatchClientLib.m_lErrorCode & 2 )
		{
			::MessageBoxW(m_pDlg->GetSafeHwnd(), L"CreateSocket Error", MESSAGE_TITLE, MB_OK);
		}
		if( m_cPatchClientLib.m_lErrorCode & 4 )
		{
			::LoadStringW(g_resInst, IDS_ERROR_DNS, strMsg, 256);
			::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
		}
		if( m_cPatchClientLib.m_lErrorCode & 8 )
		{
			::LoadStringW(g_resInst, IDS_ERROR_CONNECTION, strMsg, 256);
			::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
		}

		if( bConnect )
		{
			bool bResult = m_cPatchClientLib.ExecuteIOLoop( false );

			if( m_cPatchClientLib.IsPatchClient() )
			{
				::LoadStringW(g_resInst, IDS_RUN_AGAIN, strMsg, 256);
				
				::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);

				m_pDlg->SendMessage( WM_USER+1 );

				return;
			}

			if( bResult )
			{
				//패치성공사실을 알린다.
				::LoadStringW(g_resInst, IDS_MSG_COMPLETE, strMsg, 256);
				m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticDetailInfo, strMsg );

				::LoadStringW(g_resInst, IDS_RUN_GAME, strMsg, 256);
				m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticStatus, strMsg );

				if ( m_cPatchOptionFile.m_bSaveLoad )
				{
					CWnd* pcWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
					pcWnd->EnableWindow();
				}

				CWnd* pcWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
				pcWnd->EnableWindow();


				if( GetForceExit() )
				{
					exit(0);
				}
			}
			else
			{
				if( m_cPatchClientLib.m_lResult == g_lAlreadyPatched )
				{
					//패치성공사실을 알린다.
					m_cPatchClientLib.m_pcCustomProgressRecvBlock->SetMaxCount( 1 );
					m_cPatchClientLib.m_pcCustomProgressRecvBlock->SetCurrentCount( 1 );
					m_pDlg->InvalidateRect( m_cPatchClientLib.m_pcCustomProgressRecvBlock->GetRect(), FALSE );

					m_cPatchClientLib.m_pcCustomProgressRecvFile->SetMaxCount( 1 );
					m_cPatchClientLib.m_pcCustomProgressRecvFile->SetCurrentCount( 1 );
					m_pDlg->InvalidateRect( m_cPatchClientLib.m_pcCustomProgressRecvFile->GetRect(), FALSE );

					::LoadStringW(g_resInst, IDS_MSG_COMPLETE, strMsg, 256);
					m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticDetailInfo, strMsg );

					::LoadStringW(g_resInst, IDS_RUN_GAME, strMsg, 256);
					m_cPatchClientLib.InvalidateStatic( m_cPatchClientLib.m_pcStaticStatus, strMsg );

					//pcPatchClientDlg->m_cPatchReg.BackupRegistry();
					if ( m_cPatchOptionFile.m_bSaveLoad )
					{
						CWnd* pcWnd = m_pDlg->GetDlgItem( IDC_OptionButton );
						pcWnd->EnableWindow();
					}

					CWnd* pcWnd = m_pDlg->GetDlgItem( IDC_START_GAME );
					pcWnd->EnableWindow();

				}
				else if( m_cPatchClientLib.m_lResult == g_lNotEnoughDiskSpace )
				{
					wchar_t			strBuffer[256];

					::LoadStringW(g_resInst, IDS_INSUFFICENT_DISK, strMsg, 256);
					swprintf( strBuffer, strMsg, m_cPatchClientLib.m_iRequireDiskSpace/1000000.0f );

					::MessageBoxW(m_pDlg->GetSafeHwnd(), strBuffer, MESSAGE_TITLE, MB_OK);
				}
				else if( m_cPatchClientLib.m_lResult == g_lCannotConnectToDownloadServer )
				{
					::LoadStringW(g_resInst, IDS_ERROR_FTP_CONNECTION, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
				else if( m_cPatchClientLib.m_lResult == g_lDisconnectFromDownloadServer )
				{
					::LoadStringW(g_resInst, IDS_ERROR_FTP_DISCONNECTED, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
				else if( m_cPatchClientLib.m_lResult == g_lCannotOpenFileFromDownload )
				{
					::LoadStringW(g_resInst, IDS_ERROR_FTP_OPEN, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
				else if( m_cPatchClientLib.m_lResult == g_lCannotDownloadFileFromDownload )
				{
					::LoadStringW(g_resInst, IDS_ERROR_FTP_RECEIVE, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
				else
				{
					::LoadStringW(g_resInst, IDS_ERROR_FAILED, strMsg, 256);
					::MessageBoxW(m_pDlg->GetSafeHwnd(), strMsg, MESSAGE_TITLE, MB_OK);
				}
			}
		}
		else
		{
			::LoadStringW(g_resInst, IDS_ERROR_PATCH_CONNECTION, strMsg, 256);
			::MessageBoxW(m_pDlg->m_hWnd, strMsg, MESSAGE_TITLE, MB_OK);
		}		

		DisConnect();
	}
	else
	{
		//MessageBoxW( "레지스트리에 패치 정보가 없습니다." );
	}
}

BOOL BasePatchClient::IsExistFile( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	FILE* pFile = fopen( pFileName, "r" );
	if( !pFile ) return FALSE;

	fclose( pFile );
	return TRUE;
}

INT GetIntFromStringTable( INT nResourceID )
{
	CString			strTemp;

	strTemp.LoadString( nResourceID );

	return atoi( (LPSTR)(LPCTSTR)strTemp );
}

