// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"

#pragma warning(disable:4786) //disable linker key > 255 chars long (for stl map)
#include <map>
#include "RenderWare.h"

#include "mapTool.h"

#include "rwcore.h"
#include "rpworld.h"
//#include "AcuRpDWSector.h"
#include "MainWindow.h"

#include "ChildView.h"
#include "MainFrm.h"

#include <AgcEngine.h>
#include "MainWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView
/////////////////////////////////////////////////////////////////////////////

int	DeleteFiles( char * path )
{
	// 디렉토리를 지우기 위해서..
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	int count = 0;

	// path 의 파일을 찾아본다.
	hFind = FindFirstFile( path , & FindFileData );

	char	strFile[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( path , drive, dir, fname, ext );

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		OutputDebugString( "DeleteFiles : FindFile Handle Invalid\n" );
		return 0;
	} 
	else 
	{
		wsprintf( strFile , "%s%s%s" , drive , dir , FindFileData.cFileName );

		if( DeleteFile( strFile ) )
			count ++;
		else
		{
			// 딜리트 실패,.
			char strDebug[ 256 ];
			wsprintf( strDebug , "Cannot Delete '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
			OutputDebugString( strDebug );
		}

		while( FindNextFile( hFind , & FindFileData ) )
		{
			wsprintf( strFile , "%s%s%s" , drive , dir , FindFileData.cFileName );

			if( DeleteFile( strFile ) )
				count ++;
			else
			{
				// 딜리트 실패,.
				char strDebug[ 256 ];
				wsprintf( strDebug , "Cannot Delete '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
				OutputDebugString( strDebug );
			}
		}

		// 처리 끝

		return count;
	}
}

int	CopyFiles( char * path , char * pathTarget )
{
	// 디렉토리를 지우기 위해서..
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	int count = 0;

	// path 의 파일을 찾아본다.
	hFind = FindFirstFile( path , & FindFileData );

	char	strFile[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( path , drive, dir, fname, ext );

	char	strFileTarget[ 1024 ];
	char	drive2[ 256 ] , dir2 [ 256 ] , fname2 [ 256 ] , ext2[ 256 ];
	_splitpath( pathTarget , drive2 , dir2 , fname2 , ext2 );

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		OutputDebugString( "CopyFiles : FindFile Handle Invalid\n" );
		return 0;
	} 
	else 
	{
		wsprintf( strFile		, "%s%s%s" , drive , dir , FindFileData.cFileName );
		wsprintf( strFileTarget	, "%s%s%s" , drive2 , dir2 , FindFileData.cFileName );

		if( CopyFile( strFile , strFileTarget , FALSE ) )
			count ++;
		else
		{
			// 딜리트 실패,.
			char strDebug[ 256 ];
			wsprintf( strDebug , "Cannot Copy '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
			OutputDebugString( strDebug );
		}

		while( FindNextFile( hFind , & FindFileData ) )
		{
			wsprintf( strFile		, "%s%s%s" , drive , dir , FindFileData.cFileName );
			wsprintf( strFileTarget	, "%s%s%s" , drive2 , dir2 , FindFileData.cFileName );

			if( CopyFile( strFile , strFileTarget , FALSE ) )
				count ++;
			else
			{
				// 딜리트 실패,.
				char strDebug[ 256 ];
				wsprintf( strDebug , "Cannot Copy '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
				OutputDebugString( strDebug );
			}
		}

		// 처리 끝

		return count;
	}
}

extern MainWindow			g_MainWindow	;

using namespace ChildView;

CChildView::CChildView()
{
	// 마고자 (2002-06-11 오전 11:53:36) : 디렉토리 생성.
	MakeDirectory		();
}

//---------------------------------------------------------------------------

CChildView::~CChildView()
{
}

//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
	
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);
	
	return TRUE;
}

//---------------------------------------------------------------------------
int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	this->SetTimer (0, 500,0x0);
	
	CRect rcClient;
	GetClientRect(&rcClient);
	
	//Get the largest client dimension so the renderware client has a ratio of 1:1
	int iMaxDim;
	iMaxDim = ( rcClient.Width() > rcClient.Height() )? rcClient.Width() : rcClient.Height();
	
	//RenderWare initialisation===================================================
	if( !theApp.GetRenderWare().Initialize( this->GetSafeHwnd() ) )
	{
		MessageBox( "RenderWare 인진 초기화 실패 : Direct X의 버젼을 확인하세요 ( 9.0c 이상이어야함" );
		ExitProcess( -1 );
		return -1;
	}

	// 마고자 (2002-06-20 오후 3:36:26) : 텍스쳐 리스트 작성.
	LoadTextureList		();
	// 텍스쳐는 리스트만 작성해두고 , 맵을 생성할때 필요한 텍스쳐만 메모리에 읽어들이는 방식을 사용한다.!!!
	
	g_Const.Update();

	return 0;
}


//---------------------------------------------------------------------------
// Tell RenderWare to close down, BEFORE the HWND is destroyed
void CChildView::OnDestroy( void ) 
{
	CWnd::OnDestroy();
	
	return;
}


//---------------------------------------------------------------------------
//Tell RenderWare to paint
void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	//Get a pointer to renderware and the window dimensions
	CRect rcClient;
	this->GetParentFrame()->GetClientRect(rcClient);
}

//---------------------------------------------------------------------------
//Change the navigation view for the next tick frame
void CChildView::OnTimer(UINT nIDEvent) 
{
	// 마고자 (2002-06-20 오후 3:15:00) : 
	// OnFirstTime() 처리용 코드 
	{
		static	bool	bFirst = true;
		if( bFirst )
		{
			bFirst = false;
			OnFirstTime();
		}
	}
	CWnd::OnTimer(nIDEvent);
}

//----------------------------------------------------------------------------------
//Tell RenderWare to resize itself accordingly
void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);
	
	//Get the maximum dimension of the child window
	CRect rcClient;
	GetClientRect(&rcClient);
	int iMaxDim;
	iMaxDim = ( rcClient.Width() > rcClient.Height() )? rcClient.Width() : rcClient.Height();
	//Assing the viewport as a square to renderware
	//theApp.GetRenderWare().SetCameraViewSize(iMaxDim, iMaxDim);
}



BOOL CChildView::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_MOUSEWHEEL )
	{
		theApp.GetRenderWare().MessageProc( pMsg );
		return TRUE;
	}
	else
	if( pMsg->message == WM_KEYDOWN		||
		pMsg->message == WM_KEYUP		)
	{
		// 다음 키가 입력을 받지 않아서 ( 어딘가에서 먹어버리고 있는듯하다 )
		// 부득이하게 여기서 포워딩 시켜버림
		switch( pMsg->wParam )
		{
		case 'C'		:
		case 'W'		:
		case 'A'		:
		case 'S'		:
		case 'D'		:
		case 'Q'		:
		case 'E'		:
		case 'T'		:
		case VK_TAB		:
				/********************* Parn님이 여기다가 VK_DEL 넣었당. **********/
		case VK_DELETE	:
		case VK_LEFT	:
		case VK_UP		:
		case VK_RIGHT	:
		case VK_DOWN	:
			{
				theApp.GetRenderWare().MessageProc( pMsg );
			}
			return TRUE;
		case 'Z'		:
			{
				if( ISBUTTONDOWN( VK_CONTROL ) ) 
					break;
				else 
					theApp.GetRenderWare().MessageProc( pMsg );
				return TRUE;
			}
		case VK_SPACE	:
			{
				theApp.GetRenderWare().MessageProc( pMsg );
			}
			return TRUE;
		default:
			// do no op
			break;
		}
		
	}
	return CWnd ::PreTranslateMessage(pMsg);
}

void CChildView::MakeDirectory()
{
	// 없는 디렉토리생성.

	// 맵툴관리하..
	CreateDirectory( "map"					, NULL );
	CreateDirectory( "map\\temp"			, NULL );
	CreateDirectory( "map\\temp\\rough"		, NULL );
	CreateDirectory( "map\\data"			, NULL );
	CreateDirectory( "map\\data\\blocking"	, NULL );
	CreateDirectory( "map\\data\\geometry"	, NULL );
	CreateDirectory( "map\\data\\moonee"	, NULL );
	CreateDirectory( "map\\data\\compact"	, NULL );
	CreateDirectory( "map\\data\\grass"		, NULL );
	CreateDirectory( "map\\data\\water"		, NULL );
	CreateDirectory( "map\\tile"			, NULL );
	CreateDirectory( "map\\sky"				, NULL );
	CreateDirectory( "world"				, NULL );
	CreateDirectory( "texture\\world"		, NULL );

	// 보스 스폰 저장용..
	CreateDirectory( "ini"					, NULL );
	CreateDirectory( "ini\\object"			, NULL );
	CreateDirectory( "ini\\object\\design"	, NULL );

	// 마고자 (2004-07-05 오후 3:27:53) : 추가~
	CreateDirectory( "map\\data\\object"			, NULL );
	CreateDirectory( "map\\data\\object\\artist"	, NULL );
	CreateDirectory( "map\\data\\object\\design"	, NULL );
	CreateDirectory( "map\\data\\object\\pointlight", NULL );
	
	CreateDirectory( "map\\subdata"			, NULL );
}

void CChildView::OnFirstTime( void )
{
	// 맵선택창 띄움
	g_MainWindow.LoadPolygonData();

	CRect rcClient;
	GetClientRect(&rcClient);
	int iMaxDim;
	iMaxDim = (rcClient.Width() > rcClient.Height())? rcClient.Width() : rcClient.Height() ;
	theApp.GetRenderWare().SetCameraViewSize( rcClient.Width() , rcClient.Height() );
}

void CChildView::LoadTextureList		( void )	// 디렉토리 구조를 읽어서 파일을찾아냄..
{
	// 기존리스트 삭제.
	// 이미 사용돼고 있는 텍스쳐들을 날려서 문제가 생길 가능성은 충분히 있음.
//	g_MainWindow.m_TextureList.DeleteAll();
//
//	// 리스트 작성.
//	char filename [ 1024 ];
//	wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , ALEF_TILE_LIST_FILE_NAME );
//	if( g_MainWindow.m_TextureList.LoadScript( filename ) )
//	{
//		// 성공
//		// 월드 새로생성?..
//	}
//	else
//	{
//		// 실패.
//		MessageBox( "타일 스크립트 파일에 문제가 있습니다." );
//	}
}

LRESULT CChildView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	MSG messagestruct;
	messagestruct.hwnd		= GetSafeHwnd();
	messagestruct.message	= message;
	messagestruct.wParam	= wParam;
	messagestruct.lParam	= lParam;
	
	if( theApp.GetRenderWare().MessageProc( & messagestruct ) ) return TRUE;

	switch( message )
	{
	case WM_COMMAND:
		{
			int	menuid = LOWORD( wParam );

//			if( menuid >= IDM_OBJECT_POPUP_OFFSET && menuid < IDM_OBJECT_POPUP_OFFSET + g_MainWindow.m_listClumpSorted.GetCount() )
//			{
//				g_MainWindow.PopupMenuSelect( menuid - IDM_OBJECT_POPUP_OFFSET );
//			}
//			else
			if( menuid == IDM_OBJECT_PROPERTY		||
				menuid == IDM_OBJECT_EVENT			||
				menuid == IDM_OBJECTPOP_GROUPSAVE	||
				menuid == IDM_OBJECTPOP_OCTREEROOTDISPLAY )
			{
				g_MainWindow.PopupMenuObject(menuid);
			}
		}
		break;	
	default:
		break;
	}
	return CWnd ::WindowProc(message, wParam, lParam);
}
