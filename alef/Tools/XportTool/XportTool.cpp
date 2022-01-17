#include "stdafx.h"
#include "XportTool.h"
#include "resource.h"
#include <cassert>
#include <iostream>

//Console
#include <Shellapi.h>
#include <Wincon.h>

//Directory
#include <shellapi.h>
#include <shlobj.h>

//------------------------- CConsole ------------------------- 
CConsole::CConsole()
{
	m_hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	if( !m_hConsole || m_hConsole == INVALID_HANDLE_VALUE)	
	{
		if( m_bMake = AllocConsole() )
		{
			m_hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
			SetConsoleCtrlHandler( CConsole::CtrlHandler, TRUE );

			COORD size; 
			size.X = 80; size.Y = 10000;
			SetConsoleScreenBufferSize( m_hConsole, size );
		}
	}
}

CConsole::~CConsole()
{
	if( m_bMake )
		FreeConsole();
}

//------------------------- CConsole ------------------------- 
XportTool* XportTool::m_pThis = NULL;
XportTool* XportTool::GetInst()		{	return m_pThis;	}

// 서비스 지역 정의 디폴트는 Korea
ApServiceArea	g_eSelectedServiceArea	= AP_SERVICE_AREA_KOREA;
eExportTarget	g_eExportTarget			= SET_DISTRIBUTE;

void	LogConsole( char* szLog )
{
	if( XportTool::GetInst() )
		XportTool::GetInst()->PrintConsole( szLog );
}

void	LogFormatConsole( char* szFormat, ...  )
{
	if( XportTool::GetInst() )
	{
		static char strText[1024 * 8];

		va_list vParams;
		va_start( vParams, szFormat );
		vsprintf( strText, szFormat, vParams );
		va_end( vParams );

		XportTool::GetInst()->PrintConsole( strText );
	}
}

BOOL export_process()
{
	if( !XportTool::GetInst() )							return FALSE;
	if( !XportTool::GetInst()->RecreateExportFolder() )	return FALSE;

	XportTool::GetInst()->PrintConsole("=== Export 시작 ===");
	XportTool::GetInst()->m_cExportResource.SetExportArea( g_eSelectedServiceArea );
	XportTool::GetInst()->m_cExportResource.SetExportTarget( g_eExportTarget );

	BOOL ok = TRUE;
	if( XportTool::GetInst()->m_vecExportNode[eXportCharacter].bUse )	ok = XportTool::GetInst()->exportCharacterTemplate();
	if( XportTool::GetInst()->m_vecExportNode[eXportItem].bUse )		ok = XportTool::GetInst()->exportItemTemplate();
	if( XportTool::GetInst()->m_vecExportNode[eXportObject].bUse )		ok = XportTool::GetInst()->exportObjectTemplate();
	if( XportTool::GetInst()->m_vecExportNode[eXportSkill].bUse )		ok = XportTool::GetInst()->exportSkillTemplate();
	if( XportTool::GetInst()->m_vecExportNode[eXportEffect].bUse )		ok = XportTool::GetInst()->exportEffectResource();
	if( XportTool::GetInst()->m_vecExportNode[eXportUI].bUse )			ok = XportTool::GetInst()->exportUIResource();
	if( XportTool::GetInst()->m_vecExportNode[eXportWorld].bUse )		ok = XportTool::GetInst()->exportWorldResource();
	if( XportTool::GetInst()->m_vecExportNode[eXportETC].bUse )			ok = XportTool::GetInst()->exportEtcTexture();
	if( XportTool::GetInst()->m_vecExportNode[eXportINI].bUse )			ok = XportTool::GetInst()->exportINIResource();

	//if( XportTool::GetInst()->m_vecExportNode[eXportDDS].bUse )		ok = XportTool::GetInst()->exportDDS();
	//if( XportTool::GetInst()->m_vecExportNode[eXportTexture].bUse )	ok = XportTool::GetInst()->exportTexture();

	return ok;
}

XportTool::XportTool() : 
 m_hHwnd(NULL)
{
	m_pThis		= this;

	m_vecExportNode.push_back( ExportNode( "ct", "Character" ) );
	m_vecExportNode.push_back( ExportNode( "it", "Item" ) );
	m_vecExportNode.push_back( ExportNode( "ot", "Object" ) );
	m_vecExportNode.push_back( ExportNode( "st", "Skill" ) );
	m_vecExportNode.push_back( ExportNode( "er", "Effect" ) );
	m_vecExportNode.push_back( ExportNode( "ur", "UI" ) );
	m_vecExportNode.push_back( ExportNode( "wr", "World" ) );
	m_vecExportNode.push_back( ExportNode( "et", "Effect_Texture" ) );
	m_vecExportNode.push_back( ExportNode( "ir", "INI_Files" ) );
	//m_vecExportNode.push_back( ExportNode( "dds", "Other_Textures" ) );
	//m_vecExportNode.push_back( ExportNode( "tx", "Single_Texture" ) );
}

XportTool::~XportTool()
{
	Destory();
}

BOOL	XportTool::Init( HWND hWnd )
{
	m_hHwnd = hWnd;

	if( !m_cRenderWare.initialize(hWnd) )
		PrintConsole("m_cRenderWare.initialize(hWnd) failed!");

	AuPackingManager::MakeInstance();

	if( !m_cEngine.OnRegisterModule() )
		PrintConsole("m_cEngine.OnRegisterModule() failed!");

	if( !m_cEngine.Initialize() )
		PrintConsole("m_cEngine.Initialize() failed!");

	TCHAR szDir[MAX_PATH];
	GetCurrentDirectory( MAX_PATH, szDir );

	_stprintf( m_szBinPath, "%s\\" , szDir );
	_stprintf( m_szExportPath, _T("%s\\..\\Export\\"), szDir );

	m_cExportResource.Initialize( m_szBinPath, m_szExportPath, m_cEngine.pAgcmCharacter_, m_cEngine.pAgcmItem_, m_cEngine.pAgcmObject_, LogConsole, LogFormatConsole );

	return TRUE;
}

void	XportTool::Destory()
{
	//m_cEngine.Destroy();
	//m_cRenderWare.destroy();
	m_vecExportNode.clear();
}

void	XportTool::PrintConsole( const char* pStr )
{
	DWORD dwWrite;
	WriteFile( m_hConsole, (void*)pStr, strlen(pStr), &dwWrite, NULL );
	WriteFile( m_hConsole, (void*)_T("\n"), 1, &dwWrite, NULL );
	if( m_hHwnd )
		SetWindowText( m_hHwnd, pStr );
}

void	XportTool::PrintFormatConsole( char* pFormat, ... )
{
	static char	strTemp[ 1024*5 ];

	va_list vParams;
	va_start( vParams, pFormat );
	vsprintf( strTemp, pFormat, vParams );
	va_end( vParams );

	PrintConsole( strTemp );
}

void	XportTool::PrintLog( char *pFormat, ... )
{
	static char	strTemp[ 1024*5 ];

	va_list vParams;
	va_start( vParams, pFormat );
	vsprintf( strTemp, pFormat, vParams );
	va_end( vParams );

	DWORD dwWrite;
	WriteFile( m_hConsole, (void*)strTemp, strlen(strTemp), &dwWrite, NULL );
}

BOOL	XportTool::RecreateExportFolder()
{
	PrintLog( "디렉토리 정리중입니다...." );
	char szExportPath[MAX_PATH];
	sprintf( szExportPath, "%s", m_szExportPath );
	szExportPath[ strlen(szExportPath)-1 ] = NULL;

	SHFILEOPSTRUCT sfo;
	sfo.hwnd	= NULL;
	sfo.wFunc	= FO_DELETE;
	sfo.pFrom	= szExportPath;
	sfo.pTo		= NULL;
	sfo.fFlags	= FOF_NOCONFIRMATION | FOF_SILENT;

	HRESULT hr = SHFileOperation( &sfo );
	if( !hr )
		PrintConsole("The old export folder has been removed.");

	SHChangeNotify( SHCNE_RMDIR, SHCNF_PATH, m_szExportPath, 0 );
	hr = SHCreateDirectoryEx( NULL, m_szExportPath, NULL );
	SHChangeNotify( SHCNE_MKDIR, SHCNF_PATH, m_szExportPath, 0 );

	if( hr == ERROR_SUCCESS )
		PrintConsole("A new export folder has been created.");
	else
		PrintConsole("A new export folder cannot be created!");
		
	return TRUE;
}


BOOL XportTool::exportCharacterTemplate()
{
	PrintConsole("---Character Template---");

	if(!m_cExportResource.ExportCharacterTemplate(m_cEngine.pAgpmCharacter_, m_cEngine.pAgcmCharacter_, m_cEngine.pAgcmEventEffect_,
												"CharacterTemplatePublic.ini",
												"CharacterTemplateClient.ini",
												"CharacterTemplateAnimation.ini",
												"CharacterTemplateCustomize.ini",
												"CharacterTemplateSkill.ini",
												"CharacterTemplateSkillSound.ini",
												"CharacterTemplateEventEffect.ini",
												"SOCIAL_PC.txt",
												"SOCIAL_GM.txt",
												m_cEngine.pAgcmPreLODManager_,
												"CHARACTERPRELOD.INI",
												m_cEngine.pAgpmSkill_,
												m_cEngine.pAgcmSkill_,
												"SKILLTEMPLATE.INI" ) )
	{
		PrintConsole("m_cExportResource.ExportCharacterTemplate() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportItemTemplate()
{
	PrintConsole("---Item Template---");

	//if( !m_cExportResource.ExportItemTemplates( m_cEngine.pAgpmItem_, m_cEngine.pAgcmItem_, "ITEMTEMPLATE.INI", m_cEngine.pAgcmPreLODManager_, "ITEMPRELOD.INI", NULL ) )
	if( !m_cExportResource.ExportItemTemplates( m_cEngine.pAgpmItem_, m_cEngine.pAgcmItem_, "INI\\ITEMTEMPLATE", "INI\\ItemTemplateEntry.ini", m_cEngine.pAgcmPreLODManager_, "ITEMPRELOD.INI" ) )
	{
		PrintConsole("m_cExportResource.ExportItemTemplate() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportObjectTemplate()
{
	PrintConsole("---Object Template---");

	if( !m_cExportResource.ExportObjectTemplate(m_cEngine.pApmObject_, m_cEngine.pAgcmObject_, "OBJECTTEMPLATE.INI", m_cEngine.pAgcmPreLODManager_, "OBJECTPRELOD.INI" ) )
	{
		PrintConsole("m_cExportResource.ExportObjectTemplate() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportSkillTemplate()
{
	PrintConsole("---Skill Template---");

	if( !m_cExportResource.ExportSkillTemplate( m_cEngine.pAgpmSkill_, m_cEngine.pAgcmSkill_, "SKILLTEMPLATE.INI" ) )
	{
		PrintConsole("m_cExportResource.ExportSkillTemplate() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportEffectResource()
{
	PrintConsole("---Effect---");

	if( !m_cExportResource.ExportEffectResource(m_cEngine .pAgcmEff2_ ) )
	{
		PrintConsole("m_cExportResource.ExportEffectResource() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportUIResource()
{
	PrintConsole("---UI---");

	if( !m_cExportResource.ExportUIResource() )
	{
		PrintConsole("m_cExportResource.ExportUIResource() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportWorldResource()
{
	PrintConsole("---World---");

	if( !m_cExportResource.ExportWorldResource() )
	{
		PrintConsole("m_cExportResource.ExportWorldResource() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportEtcTexture()
{
	PrintConsole("---Etc---");

	if( !m_cExportResource.ExportEtcResource() )
	{
		PrintConsole("m_cExportResource.ExportEtcResource() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportINIResource()
{
	PrintConsole("---INI---");

	if( !m_cExportResource.ExportINIResource() )
	{
		PrintConsole("m_cExportResource.ExportINIResource() failed!");
		return FALSE;
	}

	return TRUE;
}

BOOL XportTool::exportDDS()
{
	PrintConsole("---DDS----");

	m_cEngine.pAgcmResourceLoader_->SetTexturePath("Texture\\ETC\\");
	if( !m_cExportResource.ExportDDSPath( m_cEngine.pAgcmResourceLoader_ ) )
	{
		PrintConsole( "m_cExportResource.ExportINIResource() failed!" );
		return FALSE;
	}

	return TRUE;
}

//------------------------------ XportDlgProc ---------------------------
BOOL CALLBACK XportDlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	switch( message ) 
	{ 
	case WM_INITDIALOG: 
		{
			HWND hwndOwner = GetParent(hwndDlg);
			if( !hwndOwner )
				hwndOwner = GetDesktopWindow(); 

			RECT rc, rcDlg, rcOwner;
			GetWindowRect( hwndOwner, &rcOwner ); 
			GetWindowRect( hwndDlg, &rcDlg );
			CopyRect( &rc, &rcOwner );
			OffsetRect( &rcDlg, -rcDlg.left, -rcDlg.top );
			OffsetRect( &rc, -rc.left, -rc.top );
			OffsetRect( &rc, -rcDlg.right, -rcDlg.bottom );

			SetWindowPos( hwndDlg, HWND_TOP, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), 0, 0, SWP_NOSIZE ); 

			XportTool::GetInst()->Init( hwndDlg );

			ShowWindow( hwndDlg, SW_HIDE );
			EndDialog( hwndDlg, export_process() ? 0 : -1 ); 
			return TRUE; 
		}

	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
		case IDOK: 
		case IDCANCEL: 
			EndDialog(hwndDlg, 0); 
			return TRUE; 
		} 
	} 
	return FALSE; 
}


//------------------------------ _tWinMain ---------------------------
void	GetMySystemTime( SYSTEMTIME& st )
{
	char strFilename[ 1024 ];
	char strFullname[ 1024 ];
	GetModuleFileName( NULL , strFilename , 1024 );
	GetFullPathName( strFilename , 1024 , strFullname , NULL );

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile( strFullname , &fd );
	FindClose( hFind );

	FileTimeToSystemTime( &fd.ftLastWriteTime , &st );
}

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	XportTool	cXportTool;

	int argc;
	LPWSTR* argv = CommandLineToArgvW( GetCommandLineW(), &argc );
	for(int i=1; i<argc; ++i)
	{
		char szArgc[256];
		WideCharToMultiByte( 949 , 0 , argv[i], -1, szArgc, 256, NULL , NULL);

		BOOL bExport = FALSE;
		for( int nExport=eXportCharacter; nExport<eXportMax; ++nExport )
		{
			if( cXportTool.m_vecExportNode[nExport].strCommand == szArgc )
			{	
				cXportTool.m_vecExportNode[nExport].bUse = TRUE;
				bExport			= TRUE;
			}
		}

		if( !bExport )
		{
			if( !strcmp( szArgc, "china" ) )			g_eSelectedServiceArea = AP_SERVICE_AREA_CHINA;
			else if( !strcmp( szArgc, "korea" ) )		g_eSelectedServiceArea = AP_SERVICE_AREA_KOREA;
			else if( !strcmp( szArgc, "western" ) )		g_eSelectedServiceArea = AP_SERVICE_AREA_WESTERN;
			else if( !strcmp( szArgc, "japan" ) )		g_eSelectedServiceArea = AP_SERVICE_AREA_JAPAN;
			else if( !strcmp( szArgc, "internal" ) )	g_eExportTarget = SET_INTERNAL;
			else if( !strcmp( szArgc, "testserver" ) )	g_eExportTarget = SET_TESTSERVER;
			else if( !strcmp( szArgc, "distribute" ) )	g_eExportTarget = SET_DISTRIBUTE;
			else										cXportTool.m_strExportFilename = szArgc;
		}
	}

	// 날자랑 옵션 정보 출력.
	SYSTEMTIME st;
	GetMySystemTime( st );

	cXportTool.PrintLog( "Archlord Xporter ver %04d/%02d/%02d\n" , st.wYear , st.wMonth , st.wDay );
	cXportTool.PrintLog( "========================================\n" );

	cXportTool.PrintLog( "Region Select : " );
	switch( g_eSelectedServiceArea )
	{
	case AP_SERVICE_AREA_KOREA:		cXportTool.PrintLog( "KOREA\n" );	break;
	case AP_SERVICE_AREA_CHINA:		cXportTool.PrintLog( "CHINA\n" );	break;
	case AP_SERVICE_AREA_WESTERN:	cXportTool.PrintLog( "WESTERN\n" );	break;
	case AP_SERVICE_AREA_JAPAN:		cXportTool.PrintLog( "JAPAN\n" );	break;
	default:						cXportTool.PrintLog( "Unknown\n" );	break;
	}

	cXportTool.PrintLog( "Export Mode : " );
	switch( g_eExportTarget )
	{
	case SET_INTERNAL:		cXportTool.PrintLog( "내부 테스트용\n" );	break;
	case SET_TESTSERVER:	cXportTool.PrintLog( "테스트 써버용\n" );	break;
	case SET_DISTRIBUTE:	cXportTool.PrintLog( "외부 배포용\n" );		break;
	default:				cXportTool.PrintLog( "Unknown\n" );			break;
	}

	cXportTool.PrintLog( "Export Data : " );
	for( ExportNodeVecItr Itr = cXportTool.m_vecExportNode.begin(); Itr != cXportTool.m_vecExportNode.end(); ++Itr )
	{
		if( (*Itr).bUse )
		{
			cXportTool.PrintLog( (char*)(*Itr).strString.c_str() );
			cXportTool.PrintLog( " " );
		}
	}
	cXportTool.PrintLog( "\n\n" );

	DialogBox( hInstance, MAKEINTRESOURCE(IDD_XPORTTOOL), NULL, (DLGPROC)XportDlgProc );

	::PostQuitMessage( 0 );
}
