// TextureListStatic.cpp : implementation file
//

#include "stdafx.h"
#include "rwcore.h"
#include "TextureListStatic.h"
#include "AuIniManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextureListStatic

CTextureListStatic::CTextureListStatic()
{
	m_nIndexSelected	= -1		;
	m_bDragging			= FALSE		;
	m_pSelectedElement	= NULL		;
}

CTextureListStatic::~CTextureListStatic()
{
}


BEGIN_MESSAGE_MAP(CTextureListStatic, CStatic)
	//{{AFX_MSG_MAP(CTextureListStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_DROPFILES()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextureListStatic message handlers

void CTextureListStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect , rect2;
	GetClientRect( rect );

	dc.FillSolidRect( rect , TLS_BACKGROUNDCOLOR );

	AuNode< TextureElement * >	*	pNode			= m_listTextureDraw.GetHeadNode();
	TextureElement *				pTextureElement	;

	while( pNode )
	{
		pTextureElement	=	pNode->GetData();

		// 좌표보정..
		if( pTextureElement->x < 0 ) pTextureElement->x = 0;
		if( pTextureElement->y < 0 ) pTextureElement->y = 0;
		if( pTextureElement->x + TLS_IMAGE_SIZE >= rect.Width	() )	pTextureElement->x = rect.Width	() - TLS_IMAGE_SIZE;
		if( pTextureElement->y + TLS_IMAGE_SIZE >= rect.Height	() )	pTextureElement->y = rect.Height() - TLS_IMAGE_SIZE;

		rect2.SetRect( pTextureElement->x - 2 , pTextureElement->y - 2 , pTextureElement->x + TLS_IMAGE_SIZE + 2 , pTextureElement->y + TLS_IMAGE_SIZE + 2 );

		// 선택된거면 바탕깜..
		if( pTextureElement->nIndex	== m_nIndexSelected )
		{
			dc.FillSolidRect( rect2 , TLS_SELECT_COLOR );
		}

		// Bitmap 이미지 출력함... 
//		dc.FillSolidRect( pTextureElement->x , pTextureElement->y , TLS_IMAGE_SIZE , TLS_IMAGE_SIZE , RGB( 255 , 0 , 0 ) );
//		dc.DrawText( pTextureElement->strFilename , rect2 , DT_CENTER | DT_VCENTER | DT_SINGLELINE );

		pTextureElement->bmp.Draw( dc.GetSafeHdc() ,  pTextureElement->x , pTextureElement->y );

		m_listTextureDraw.GetNext( pNode );
	}

	// 인포출력..

	if( m_nIndexSelected != -1 && m_pSelectedElement )
	{
		dc.SetTextColor( RGB( 0 , 255 , 0 ) );
		CString	str;
		str.Format( "%02d , %s" , m_nIndexSelected , m_pSelectedElement->strComment );
		dc.DrawText( str , rect , DT_RIGHT | DT_SINGLELINE | DT_BOTTOM );
	}

	
	// Do not call CStatic::OnPaint() for painting messages
}

void CTextureListStatic::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_bDragging )
	{
		CPoint	delta;
		delta = point - m_pointPrev;
		m_pointPrev	= point;

		ASSERT( NULL != m_pSelectedElement );

		m_pSelectedElement->x	+= delta.x;
		m_pSelectedElement->y	+= delta.y;

		Invalidate( FALSE );
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void CTextureListStatic::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect	rect;

	AuNode< TextureElement * >	*	pNode			= m_listTextureDraw.GetTailNode();
	TextureElement *				pTextureElement	;

	while( pNode )
	{
		pTextureElement	=	pNode->GetData();

		rect.SetRect( pTextureElement->x , pTextureElement->y , pTextureElement->x + TLS_IMAGE_SIZE , pTextureElement->y + TLS_IMAGE_SIZE );

		if( rect.PtInRect( point ) )
		{
			// 선택!
			m_nIndexSelected	= pTextureElement->nIndex	;
			m_bDragging			= TRUE						;
			m_pointPrev			= point						;
			m_pSelectedElement	= pTextureElement			;
			m_pointFirst		= point						;

			//리스트 제일 마지막으로 옮김...

			m_listTextureDraw.RemoveNode( pNode );
			m_listTextureDraw.AddTail( pTextureElement );
			Invalidate( FALSE );

			SetCapture();

			return;
		}

		m_listTextureDraw.GetPrev( pNode );
	}	
	CStatic::OnLButtonDown(nFlags, point);
}

void CTextureListStatic::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if( m_bDragging )
	{
		m_bDragging	= FALSE;
		ReleaseCapture();

		ClientToScreen( &point );
		GetParent()->PostMessage( WM_TEXTURE_DROP_EVENT , point.x , point.y );
	}
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CTextureListStatic::OnDropFiles(HDROP hDropInfo) 
{
	char	lpstr[ 1024 ];
	// 우선 몇개의 파일이 드래그 되는 지 확인한다.
	UINT dragcount = ::DragQueryFile( hDropInfo , 0xffffffff , lpstr , 1024 );

	INT32	nIndex;
	char	strFilename[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

	if( dragcount > 0 )
	{
		// 모두다 검사 하려면
		for ( int i = 0 ; i < ( int )  dragcount ; i ++ )
		{
			::DragQueryFile( hDropInfo , i , lpstr , 1024 );
			TRACE( "드롭된 %d 번째 파일 -'%s'\n" , i + 1 , lpstr );

			// 타일 추가함..

			nIndex = GetEmptyIndex();

			if ( nIndex == -1 ) continue ; // -_-;;;

			// 텍스쳐를 카피함..
			_splitpath( lpstr , drive, dir, fname, ext );

			wsprintf( strFilename , "%s\\map\\sky\\SKY%05d%s" , ALEF_CURRENT_DIRECTORY , nIndex , ext );

			VERIFY( CopyFile( lpstr , strFilename , FALSE ) );
		
			AddTexture( -1 , strFilename , lpstr );
		}
	}
	
	CStatic::OnDropFiles(hDropInfo);
}


BOOL	CTextureListStatic::AddTexture		( INT32 nIndex , char * pFilename , char * pComment , INT32 x , INT32 y)
{
	// 중복체크..
	if( nIndex == -1 )
	{
		// 인덱스 생성함..

		nIndex = GetEmptyIndex();


		// 인덱스 완성됌..
	}

	char	strFileNameImage[ 1024 ];
	char	strFileNameOnly[ 256];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( pFilename , drive, dir, fname, ext );

	wsprintf( strFileNameOnly , "%s%s" , fname , ext );
	wsprintf( strFileNameImage , "%s\\map\\sky\\SKY%05d%s" , ALEF_CURRENT_DIRECTORY , nIndex , ext );
	
	RwImage	* pImage;

	pImage = RwImageRead ( strFileNameImage );

	if( pImage == NULL )
	{
		// -_-;;;
		// 나보고 어쩌라고 - -;;
		ASSERT( !"Bitmap 생성 실패!\n" );
		return FALSE;
	}
	else
	{
		// 생각없이 한개만 생성.
		// 로딩성공.
		char strFilename[ 1024 ];
		wsprintf( strFilename , "%s\\Map\\Temp\\SKY%05d.bmp" , ALEF_CURRENT_DIRECTORY , nIndex );

		RwImage	* pReSample;
		pReSample = RwImageCreateResample( pImage , 50 , 50 );

		ASSERT( NULL != pReSample );

		VERIFY( RwImageWrite( pReSample , (LPCTSTR) strFilename ) );

		RwImageDestroy( pImage		);
		RwImageDestroy( pReSample	);

		// 리스트에 추가함..
		TextureElement *	pTextureElement;
		pTextureElement	= new TextureElement;
		pTextureElement->nIndex	= nIndex;
		strncpy( pTextureElement->strFilename , strFileNameOnly , 256 );
		if( pComment )
			strncpy( pTextureElement->strComment , pComment , 256 );
		else
			strncpy( pTextureElement->strComment , fname , 256 );

		pTextureElement->x = x;
		pTextureElement->y = y;

		m_nIndexSelected	= nIndex;

		m_listTextureDraw.AddTail( pTextureElement );

		pTextureElement->bmp.Load( strFilename );
	}


 	if( ::IsWindow(m_hWnd) ) Invalidate( FALSE );

	return TRUE;
}

BOOL	CTextureListStatic::RemoveTexture	( INT32	nIndex	)
{
	AuNode< TextureElement * >	*	pNode			= m_listTextureDraw.GetHeadNode();
	TextureElement *				pTextureElement	;

	while( pNode  )
	{
		pTextureElement	=	pNode->GetData();

		if( pTextureElement->nIndex == nIndex )
		{
			// 제거과정..
			DeleteFile( pTextureElement->strFilename );
			delete pTextureElement;
			m_listTextureDraw.RemoveNode( pNode );
			

			return TRUE;
		}

		m_listTextureDraw.GetNext( pNode );
	}

	Invalidate( FALSE );

	return FALSE;
}

void	CTextureListStatic::RemoveAll		()
{
	AuNode< TextureElement * >	*	pNode			;
	TextureElement *				pTextureElement	;

	while( pNode			= m_listTextureDraw.GetHeadNode() )
	{
		pTextureElement	=	pNode->GetData();

		delete pTextureElement;

		m_listTextureDraw.RemoveHead();
	}
}

static char g_strINITextureSection	[]	= "%02d"					;
static char g_strINIKey_Index		[]	= "Index"					;
static char g_strINIKey_Filename	[]	= "Filename"				;
static char g_strINIKey_Comment		[]	= "Comment"					;
static char g_strINIKey_Position_X	[]	= "X"						;
static char g_strINIKey_Position_Y	[]	= "Y"						;

BOOL	CTextureListStatic::LoadScript		( char *pFilename )
{
	// 리스트 초기화..
	m_listTextureDraw.RemoveAll();

	AuIniManager	iniManager;

	iniManager.SetPath( pFilename );


	char	strSectionName	[ 1024 ];
	INT32	nIndex;
	char	strFilename		[ 1024 ];
	char	strComment		[ 1024 ];
	INT32	xPos;
	INT32	yPos;

	if( iniManager.ReadFile() )
	{
		int	sectioncount = iniManager.GetNumSection();

		for( int i = 0 ; i < sectioncount ; i ++ )
		{
			strncpy( strSectionName , iniManager.GetSectionName( i ) , 1024 );

			nIndex = iniManager.GetValueI( strSectionName , g_strINIKey_Index , -1 );
			strncpy( strFilename	, iniManager.GetValue( strSectionName , g_strINIKey_Filename	, "Not Found" ) , 1024 );
			strncpy( strComment		, iniManager.GetValue( strSectionName , g_strINIKey_Comment		, "Not Found" ) , 1024 );
			xPos = iniManager.GetValueI( strSectionName , g_strINIKey_Position_X , 0 );
			yPos = iniManager.GetValueI( strSectionName , g_strINIKey_Position_Y , 0 );

			AddTexture( nIndex , strFilename , strComment , xPos , yPos );
		}
	}
	else
		return FALSE;

	return TRUE;
}

BOOL	CTextureListStatic::SaveScript		( char *pFilename )
{
	AuNode< TextureElement * >	*	pNode			= m_listTextureDraw.GetHeadNode();
	TextureElement *				pTextureElement	;

	AuIniManager	iniManager;

	iniManager.SetPath( pFilename );

	char			strSection	[ 1024 ];

	while( pNode )
	{
		pTextureElement	=	pNode->GetData();

		wsprintf( strSection , g_strINITextureSection , pTextureElement->nIndex );

		iniManager.SetValueI( strSection , g_strINIKey_Index		, pTextureElement->nIndex		);
		iniManager.SetValue	( strSection , g_strINIKey_Filename		, pTextureElement->strFilename	);
		iniManager.SetValue	( strSection , g_strINIKey_Comment		, pTextureElement->strComment	);
		iniManager.SetValueI( strSection , g_strINIKey_Position_X	, pTextureElement->x			);
		iniManager.SetValueI( strSection , g_strINIKey_Position_Y	, pTextureElement->y			);

		m_listTextureDraw.GetNext( pNode );
	}

	iniManager.WriteFile();

	return TRUE;
}


INT32	CTextureListStatic::GetEmptyIndex	()
{
	INT32	nIndex = -1;

	AuNode< TextureElement * >	*	pNode			= m_listTextureDraw.GetHeadNode();
	TextureElement *				pTextureElement	;

	// 최대치..
	ASSERT( m_listTextureDraw.GetCount() < 65535 );
	if( m_listTextureDraw.GetCount() == 0 ) nIndex = 0;

	for( int i = 0 ; i < 65536 ; i ++ )
	{
		// 그냥 하나씩 검사한다.
		pNode			= m_listTextureDraw.GetHeadNode();
		while( pNode )
		{
			pTextureElement	=	pNode->GetData();

			if( i == pTextureElement->nIndex )
			{
				break;
			}
			m_listTextureDraw.GetNext( pNode );
		}

		if( pNode )
		{
			// Do nothing;
		}
		else
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

void CTextureListStatic::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// 선택됀 텍스쳐 삭제..
	if( m_pSelectedElement )
	{
		RemoveTexture( m_pSelectedElement->nIndex );		
	}

	CStatic::OnRButtonDblClk(nFlags, point);
}
