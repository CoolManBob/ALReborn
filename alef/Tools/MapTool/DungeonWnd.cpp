// DungeonWnd.cpp : implementation file
//

#include "stdafx.h"
#include "MyEngine.h"
#include "maptool.h"
#include "MainFrm.h"
#include "DungeonWnd.h"
#include "MainWindow.h"
#include ".\dungeonwnd.h"
#include "ApModuleStream.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *		g_pMainFrame	;
extern MainWindow		g_MainWindow	;

static const char g_strINIDungeonTemplateSection	[]	= "%d"	;
static const char g_strINIDungeonTemplateName		[]	= "Name";
static const char g_strINIDungeonTemplateData		[]	= "%d:%d";	// 이전거.. 이제 안씀..
static const char g_strINIDungeonTemplateIndex		[]	= "%d:%d_Index";
static const char g_strINIDungeonTemplateSize		[]	= "%d:%d_Size";
static const char g_strINIDungeonTemplateSampleHeight[]	= "SampleHeight:%02d";

static const char g_strINIDungeonElementSection			[]	= "%d"			;
static const char g_strINIDungeonElementType			[]	= "Type"		;
static const char g_strINIDungeonElementTemplateIndex	[]	= "Template"	;
static const char g_strINIDungeonElementOffsetX			[]	= "OffsetX"		;
static const char g_strINIDungeonElementOffsetZ			[]	= "OffsetZ"		;
static const char g_strINIDungeonElementHeight			[]	= "Height"		;
static const char g_strINIDungeonElementTurnAngle		[]	= "TurnAngle"	;
static const char g_strINIDungeonElementCX				[]	= "CX"			;
static const char g_strINIDungeonElementCY				[]	= "CY"			;

static const char g_strINIDungeonElement_Data			[]	= "Element:%d";
static const char g_strINIDungeonElement_DataFormat		[]	=	"%d %d %d %f %d %d %d %d";
																// TID x z height Angle Width Height Index
// 새 오브젝트 포멧..
static const char g_strINIDungeonObjectStreaming_Dungeon	[]	= "Dungeon"		;
static const char g_strINIDungeonObjectStreaming_Info		[]	= "DungeonInfo"	;
static const char g_strINIDungeonObjectStreaming_Info2		[]	= "DungeonInfo2";
static const char g_strINIDungeonObjectStreaming_DataFormat	[]	= "%d %d %d %d %f %d %d %d %d";
																// TID type x z height Angle Width Height Index


FLOAT	CDungeonWnd::__GetWorldXFromElement( UINT32 uType , DungeonElement * pElement )
{
	// 오브젝트 추가..
	FLOAT	fX ;
	//FLOAT					fDeltaY = 0.0f;

	// 오브젝트의 피봇이 왼쪽 위 포이트에 있어서 뺑뺑 돌려줘야함..

	if( uType == TBM_DOME4	||
		uType == TBM_DOME	)
	{
		// 천자잉 피봇이 중간에 잡혀있다.
		fX = GetXPosFromOffset( pElement->uOffsetX + pElement->uWidth	/ 2	);
	}
	else
	switch( pElement->uTurnAngle )
	{
	case	DTA_0_CW	:
		{
			fX = GetXPosFromOffset( pElement->uOffsetX );
		}
		break;
	case	DTA_90_CW	:
		{
			fX = GetXPosFromOffset( pElement->uOffsetX + pElement->uWidth );
		}
		break;
	case	DTA_180_CW	:
		{
			fX = GetXPosFromOffset( pElement->uOffsetX + pElement->uWidth );
		}
		break;
	case	DTA_270_CW	:
		{
			fX = GetXPosFromOffset( pElement->uOffsetX );
		}
		break;
	}

	/*
	switch( pElement->uTurnAngle )
	{
	case	DTA_0_CW	:	fDeltaY = 0.0f		;	break;
	case	DTA_90_CW	:	fDeltaY = 270.0f	;	break;
	case	DTA_180_CW	:	fDeltaY = 180.0f	;	break;
	case	DTA_270_CW	:	fDeltaY = 90.0f		;	break;
	}
	*/

	return fX;
}

FLOAT	CDungeonWnd::__GetWorldZFromElement( UINT32 uType , DungeonElement * pElement )
{
	// 오브젝트 추가..
	FLOAT	fZ;
	//FLOAT					fDeltaY = 0.0f;

	// 오브젝트의 피봇이 왼쪽 위 포이트에 있어서 뺑뺑 돌려줘야함..

	if( uType == TBM_DOME4	||
		uType == TBM_DOME	)
	{
		// 천자잉 피봇이 중간에 잡혀있다.
		fZ = GetZPosFromOffset( pElement->uOffsetZ + pElement->uHeight	/ 2	);
	}
	else
	switch( pElement->uTurnAngle )
	{
	case	DTA_0_CW	:
		{
			fZ = GetZPosFromOffset( pElement->uOffsetZ );
		}
		break;
	case	DTA_90_CW	:
		{
			fZ = GetZPosFromOffset( pElement->uOffsetZ );
		}
		break;
	case	DTA_180_CW	:
		{
			fZ = GetZPosFromOffset( pElement->uOffsetZ + pElement->uHeight );
		}
		break;
	case	DTA_270_CW	:
		{
			fZ = GetZPosFromOffset( pElement->uOffsetZ + pElement->uHeight );
		}
		break;
	}

	/*
	switch( pElement->uTurnAngle )
	{
	case	DTA_0_CW	:	fDeltaY = 0.0f		;	break;
	case	DTA_90_CW	:	fDeltaY = 270.0f	;	break;
	case	DTA_180_CW	:	fDeltaY = 180.0f	;	break;
	case	DTA_270_CW	:	fDeltaY = 90.0f		;	break;
	}
	*/

	return fZ;
}

FLOAT	CDungeonWnd::__GetWorldDeltaYFromElement( UINT32 uType , DungeonElement * pElement )
{
	// 오브젝트 추가..
	FLOAT					fDeltaY = 0.0f;

	switch( pElement->uTurnAngle )
	{
	case	DTA_0_CW	:	fDeltaY = 0.0f		;	break;
	case	DTA_90_CW	:	fDeltaY = 270.0f	;	break;
	case	DTA_180_CW	:	fDeltaY = 180.0f	;	break;
	case	DTA_270_CW	:	fDeltaY = 90.0f		;	break;
	}

	return fDeltaY;
}

// 마고자 (2004-12-10 오후 4:14:06) : 
// + 1 은 디폴트 ..
COLORREF	__ColorTable[ MAX_DUNGEON_HEIGHT_COUNT + 1 ];

RpAtomic * __GetAtomicHeightCallback(RpAtomic * pAtomic, void *data)
{
	FLOAT	*pfHeight = ( FLOAT * ) data;

	RpGeometry		* pGeometry	= RpAtomicGetGeometry( pAtomic );
	RpMorphTarget	* pMorph	= RpGeometryGetMorphTarget( pGeometry , 0 );

	for( int i = 0 ; i < pGeometry->numVertices ; i ++ )
	{
		if( pMorph->verts[ i ].y > *pfHeight )
		{
			*pfHeight = pMorph->verts[ i ].y;
		}
	}

	return pAtomic;
}

FLOAT	__GetClumpHeight( RpClump * pClump )
{
	FLOAT	fHeight = 0.0f;

	if( pClump )
		RpClumpForAllAtomics( pClump , __GetAtomicHeightCallback , ( void * ) &fHeight );

	return fHeight;
}

/////////////////////////////////////////////////////////////////////////////
// CDungeonWnd

CDungeonWnd::CDungeonWnd()
{
	SetModuleName("DungeonWnd");

	m_colorBack		= RGB( 200 , 200 , 200 );
	m_colorGrid		= RGB( 93 , 197 , 144 );
	m_colorGridEdge	= RGB( 184 , 32 , 0 );
	m_colorGridTile	= RGB( 0 , 32 , 184 );
	m_colorGridHalf	= RGB( 235 , 223 , 27 );

	m_nLeftX	= 0;	// 좌상 좌표..
	m_nTopZ		= 0;
	m_nGridWidth	= 20;
	m_nStepSize		= 1	;

	m_bRbuttonDown		= FALSE	;
	m_nSelectMode		= SM_NONE	;
	m_pSelectedElement	= NULL	;
	m_bControlState		= FALSE	;

	// 마고자 (2004-12-10 오후 3:54:19) : 
	// 칼라 테이블 작성.
	float	h,s,v;
	int		r,g,b;

	h	= 0.0f;
	s	= 1.0f;
	v	= 1.0f;
	// h 가 x 축 s 가 y 축으로..;
	COLORREF	rgb;

	for( int i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
	{
		// s 벨루 구하기..
		h = ( float )( MAX_DUNGEON_HEIGHT_COUNT - i ) / ( float ) MAX_DUNGEON_HEIGHT_COUNT * 360.0f;
		hsv_to_rgb( h , s , v , &r , &g , &b );
		rgb	= RGB( r , g , b );

		__ColorTable[ i ] = rgb;
	}

	__ColorTable[ MAX_DUNGEON_HEIGHT_COUNT ] = RGB( 0 , 0 , 0 );
}

CDungeonWnd::~CDungeonWnd()
{
}

CDungeonTemplate *			CDungeonWnd::GetCurrentTemplate		() { return GetTemplate( g_pMainFrame->m_pDungeonToolbar->m_uCurrentTemplateIndex ); }
UINT32						CDungeonWnd::GetCurrentTemplateID	() { return g_pMainFrame->m_pDungeonToolbar->m_uCurrentTemplateIndex; }


BEGIN_MESSAGE_MAP(CDungeonWnd, CWnd)
	//{{AFX_MSG_MAP(CDungeonWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDungeonWnd message handlers

void CDungeonWnd::OnPaint() 
{
	CPaintDC	dc(this); // device context for painting
	CDC			* pDC = GetMemDC();

	ASSERT( NULL != pDC );
	if( NULL == pDC ) return;

	CRect	rect;
	GetClientRect( rect );
	pDC->FillSolidRect( rect , m_colorBack );

	// 배경출력
	{
		HDC hDC ;
		if( m_bControlState )
			hDC = m_bmpHeight.GetDC();
		else
			hDC = m_bmpDivision.GetDC();

		if( hDC )
		{
			int nx , ny;

			nx = GetScreenX( 0 );
			ny = GetScreenY( 0 );

			::StretchBlt( pDC->GetSafeHdc() , nx , ny , 512 * m_nGridWidth , 512 * m_nGridWidth ,
				hDC , 0 , 0 , 512 , 512 , SRCCOPY );
		}
	}


	CPen	penGrid, penEdge , penTile , penHalf;
	penGrid.CreatePen( PS_SOLID , 1 , m_colorGrid		);
	penEdge.CreatePen( PS_SOLID , 3 , m_colorGridEdge	);
	penTile.CreatePen( PS_SOLID , 1 , m_colorGridTile	);
	penHalf.CreatePen( PS_SOLID , 1 , m_colorGridHalf	);

	// 그리드 표시

	int	nLength = m_nGridWidth * 512;

	int	nOffset;

	if( g_pMainFrame->m_Document.m_nShowGrid )
	{
		int i;

		// 섹터 그리드
		pDC->SelectObject( penEdge	);
		for( i = 0 ; i <= 512 ; i += 32 )
		{
			nOffset = GetScreenX( i );
			if( nOffset >= -m_nGridWidth )
			{
				pDC->MoveTo( nOffset , m_nTopZ				);
				pDC->LineTo( nOffset , nLength + m_nTopZ	);

				if( nOffset >= rect.Width() ) break;
			}
		}

		for( i = 0 ; i <= 512 ; i += 32 )
		{
			nOffset = GetScreenY( i );
			if( nOffset >= -m_nGridWidth )
			{
				pDC->MoveTo( m_nLeftX 			, nOffset	);
				pDC->LineTo( m_nLeftX + nLength	, nOffset	);
				
				if( nOffset >= rect.Height() ) break;
			}
		}

		// 하프 섹터 그리드
		if( m_nGridWidth > 2 )
		{
			pDC->SelectObject( penHalf	);
			for( i = 0 ; i <= 512 ; i += 16 )
			{
				if( i % 32 == 0 ) continue;

				nOffset = GetScreenX( i );
				if( nOffset >= -m_nGridWidth )
				{
					pDC->MoveTo( nOffset , m_nTopZ				);
					pDC->LineTo( nOffset , nLength + m_nTopZ	);

					if( nOffset >= rect.Width() ) break;
				}
			}

			for( i = 0 ; i <= 512 ; i += 16 )
			{
				if( i % 32 == 0 ) continue;

				nOffset = GetScreenY( i );
				if( nOffset >= -m_nGridWidth )
				{
					pDC->MoveTo( m_nLeftX 			, nOffset	);
					pDC->LineTo( m_nLeftX + nLength	, nOffset	);
					
					if( nOffset >= rect.Height() ) break;
				}
			}

			if( m_nGridWidth > 5 )
			{
				// 타일 그리드
				pDC->SelectObject( penTile	);
				for( i = 0 ; i <= 512 ; i += 2 )
				{
					if( i % 32 == 0 ) continue;
					if( i % 16 == 0 ) continue;

					nOffset = GetScreenX( i );
					if( nOffset >= -m_nGridWidth )
					{
						pDC->MoveTo( nOffset , m_nTopZ				);
						pDC->LineTo( nOffset , nLength + m_nTopZ	);

						if( nOffset >= rect.Width() ) break;
					}
				}

				for( i = 0 ; i <= 512 ; i += 2 )
				{
					if( i % 32 == 0 ) continue;
					if( i % 16 == 0 ) continue;

					nOffset = GetScreenY( i );
					if( nOffset >= -m_nGridWidth )
					{
						pDC->MoveTo( m_nLeftX 			, nOffset	);
						pDC->LineTo( m_nLeftX + nLength	, nOffset	);
						
						if( nOffset >= rect.Height() ) break;
					}
				}

				if( m_nGridWidth > 12 )
				{
					// 하프 타일 그리드
					pDC->SelectObject( penGrid );
					for( i = 0 ; i <= 512 ; i ++ )
					{
						if( i % 32 == 0 ) continue;
						if( i % 16 == 0 ) continue;
						if( i % 2 == 0 ) continue;

						nOffset = GetScreenX( i );
						if( nOffset >= -m_nGridWidth )
						{
							pDC->MoveTo( nOffset , m_nTopZ				);
							pDC->LineTo( nOffset , nLength + m_nTopZ	);

							if( nOffset >= rect.Width() ) break;
						}
					}

					for( i = 0 ; i <= 512 ; i ++ )
					{
						if( i % 32 == 0 ) continue;
						if( i % 16 == 0 ) continue;
						if( i % 2 == 0 ) continue;

						nOffset = GetScreenY( i );
						if( nOffset >= -m_nGridWidth )
						{
							pDC->MoveTo( m_nLeftX 			, nOffset	);
							pDC->LineTo( m_nLeftX + nLength	, nOffset	);
							
							if( nOffset >= rect.Height() ) break;
						}
					}
				}
			}
		}
	}

	pDC->SelectObject( penGrid	);

	// 오브젝트 표시..

	DungeonElement * pElement;
	AuNode< DungeonElement >	* pNode;
	UINT32	uSelected = FALSE;

	for( UINT32 uType = TBM_FLOOR ; uType < TBM_SELECT ; ++uType )
	{
		// 돔 메뉴일때만 천장표시..
		if( ( uType == TBM_DOME4 || uType == TBM_DOME )				&&
			g_pMainFrame->m_pDungeonToolbar->GetItem() != TBM_DOME4	&&
			g_pMainFrame->m_pDungeonToolbar->GetItem() != TBM_DOME	) continue;

		pNode = m_listDungeonElement[ uType ].GetHeadNode();
		while( pNode )
		{
			pElement = & pNode->GetData();
			if( pNode == m_pSelectedElement )	uSelected = DET_SELECTED	;
			else								uSelected = DET_NORMAL		;

			DrawElement( pDC , uType , pElement , uSelected );

			pNode = pNode->GetNextNode();
		}
	}

	switch( m_nSelectMode )
	{
	case	SM_RANGE	:
		{
			// 범위 표시줄..
			UINT32 nX1 , nX2 , nZ1 , nZ2;
			if( m_SelectedPosX1 < m_SelectedPosX2 )
			{
				nX1 = m_SelectedPosX1;
				nX2 = m_SelectedPosX2;
			}
			else									
			{
				nX1 = m_SelectedPosX2;
				nX2 = m_SelectedPosX1;
			}
			
			if( m_SelectedPosZ1 < m_SelectedPosZ2 )
			{
				nZ1 = m_SelectedPosZ1;
				nZ2 = m_SelectedPosZ2;
			}
			else									
			{
				nZ1 = m_SelectedPosZ2;
				nZ2 = m_SelectedPosZ1;
			}

			CPen	penRed;
			penRed.CreatePen( PS_SOLID , 3 , RGB( 255 , 170 , 170 ) );
			pDC->SelectObject( penRed );
			pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

			pDC->Rectangle( GetScreenX( nX1 ) , GetScreenY( nZ1 ) ,
				GetScreenX( nX2 + 1 ) , GetScreenY( nZ2 + 1 ) );
			
		}
		break;
	case	SM_LINE		:
		{
            POINT	pt;

			if( m_pSelectedElement )
			{
				UINT32	uWidth	= m_pSelectedElement->GetData().uWidth	;
				UINT32	uHeight	= m_pSelectedElement->GetData().uHeight	;
				
				CPen	penRed;
				penRed.CreatePen( PS_SOLID , 3 , RGB( 255 , 170 , 170 ) );
				pDC->SelectObject( penRed );
				pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

				for( int i = 0 ; i < ( int ) m_vectorSelectedPoint.size() ; i ++ )
				{
					pt = m_vectorSelectedPoint[ i ];
					pDC->Rectangle( GetScreenX( pt.x ) , GetScreenY( pt.y ) ,
						GetScreenX( pt.x + uWidth ) , GetScreenY( pt.y + uHeight ) );
				}
			}
		}
		break;
	}

	// Paste in

	dc.BitBlt( 0 , 0 , rect.Width() , rect.Height() , pDC , 0 , 0 , SRCCOPY );
	
	// Do not call CWnd::OnPaint() for painting messages
}

int CDungeonWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ASSERT( NULL != g_pcsApmObject );

	if ( g_pcsApmObject &&
		!g_pcsApmObject->AddStreamCallback(
			APMOBJECT_DATA_OBJECT			,
			DungeonObjectStreamReadCB		,
			DungeonObjectStreamWriteCB		,
			this							)
		)
		return FALSE;

	
	// 마고자 (2004-10-08 오후 4:54:14) : 뎐젼 템플릿..
	if( g_pMainFrame->m_pDungeonWnd->LoadTemplate() )
	{

	}
	else
	{
		TRACE( "던젼 템플릿 로딩 실패" );
	}

	return 0;
}

void CDungeonWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
}

CDungeonTemplate *	CDungeonWnd::GetTemplate( int TID )
{
	AuNode< CDungeonTemplate >	* pNode		= m_listTemplate.GetHeadNode();
	CDungeonTemplate			* pTemplate	;
	while( pNode )
	{
		pTemplate	= & pNode->GetData();

		if( pTemplate->uTID == TID ) return pTemplate;

		pNode = pNode->GetNextNode();
	}

	return NULL;
}

void CDungeonWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	m_bRbuttonDown		= TRUE	;
	m_pointLastPress	= point	;
	
	CWnd::OnRButtonDown(nFlags, point);
}

void CDungeonWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	if( m_bRbuttonDown )
	{
		m_bRbuttonDown = FALSE;
	}
	
	CWnd::OnRButtonUp(nFlags, point);
}

void CDungeonWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bRbuttonDown )
	{
		// 위치이동..
		int	dx , dz;

		dx	= point.x - m_pointLastPress.x;
		dz	= point.y - m_pointLastPress.y;

		// 이동 처리..

		m_nLeftX	+= dx;
		m_nTopZ		+= dz;
		
		m_pointLastPress = point;

		Invalidate( FALSE );
	}

	CDungeonTemplate * pTemplate = GetCurrentTemplate();
	switch( m_nSelectMode )
	{
	case	SM_RANGE	:
		{
			if( pTemplate )
			{
				// 범위 조절중..
				int nStep;
				int nOffsetX , nOffsetZ;
				int nItem = g_pMainFrame->m_pDungeonToolbar->GetItem();
				int nSize =  GetSize( nItem );

				nOffsetX = GetXOffsetStep( point.x );
				nOffsetZ = GetZOffsetStep( point.y );

				m_SelectedPosX2 = nOffsetX;
				m_SelectedPosZ2 = nOffsetZ;

				if( m_SelectedPosX2 < m_SelectedPosX1 )
				{
					nStep = ( m_SelectedPosX1 - m_SelectedPosX2 ) + 1;
					nStep -= nStep % nSize;

					m_SelectedPosX2 = m_SelectedPosX1 - nStep + 1;
				}
				else
				{
					nStep = ( m_SelectedPosX2 - m_SelectedPosX1 ) + 1;
					nStep -= nStep % nSize ;

					m_SelectedPosX2 = m_SelectedPosX1 + nStep - 1;
				}

				if( m_SelectedPosZ2 < m_SelectedPosZ1 )
				{
					nStep = ( m_SelectedPosZ1 - m_SelectedPosZ2 ) + 1;
					nStep -= nStep % nSize;

					m_SelectedPosZ2 = m_SelectedPosZ1 - nStep + 1;
				}
				else
				{
					nStep = ( m_SelectedPosZ2 - m_SelectedPosZ1 ) + 1;
					nStep -= nStep % nSize;

					m_SelectedPosZ2 = m_SelectedPosZ1 + nStep - 1;
				}

				Invalidate( FALSE );
			}
		}
		break;
	case	SM_LINE		:
		{
			if( m_vectorSelectedPoint.size() > 1 && m_pSelectedElement)
			{
				int nOffsetX , nOffsetZ;

				nOffsetX = GetXOffsetStep( point.x );
				nOffsetZ = GetZOffsetStep( point.y );

				int	nDeltaX , nDeltaZ;

				nDeltaX = nOffsetX - m_vectorSelectedPoint[ m_vectorSelectedPoint.size() - 1 ].x;
				nDeltaZ = nOffsetZ - m_vectorSelectedPoint[ m_vectorSelectedPoint.size() - 1 ].y;

				// 마지막 녀석의 위치.
				POINT	pt = m_vectorSelectedPoint[ m_vectorSelectedPoint.size() - 1 ];

				m_nSelectMode = SM_LINE;

				BOOL	bFound = FALSE;
				
				if( nDeltaX >= ( int ) m_pSelectedElement->GetData().uWidth )
				{
					// X축으로 오른쪽이동
					pt.x += m_pSelectedElement->GetData().uWidth;
					bFound	= TRUE;
				}
				else if( nDeltaX < 0 )
				{
					// X축으로 왼쪽쪽이동
					pt.x -= m_pSelectedElement->GetData().uWidth;
					bFound	= TRUE;
				}
				else if( nDeltaZ >= ( int ) m_pSelectedElement->GetData().uHeight )
				{
					// Z축으로 아래로 이동..
					pt.y += m_pSelectedElement->GetData().uHeight;
					bFound	= TRUE;
				}
				else if( nDeltaZ < 0 )
				{
					// Z축으로 위로 이동..
					pt.y -= m_pSelectedElement->GetData().uHeight;
					bFound	= TRUE;
				}
				else
				{
					// do nothing..
				}

				if( bFound  )
				{
					// 중복 검사
					int i;
					for( i = 0 ; i < ( int ) m_vectorSelectedPoint.size() ; i ++ )
					{
						if(	m_vectorSelectedPoint[ i ].x == pt.x	&&
							m_vectorSelectedPoint[ i ].y == pt.y	)
						{
							break;
						}
					}

					if( i == m_vectorSelectedPoint.size() )
					{
						// 추가
						m_vectorSelectedPoint.push_back( pt );
						Invalidate( FALSE );
					}
					else
					{
						// 겹친다.
					}
				}

			}

		}
		break;

	default:
		{
			int nType = g_pMainFrame->m_pDungeonToolbar->GetItem();

			// 서택된것이 있으면...
			if( NULL == m_pSelectedElement || m_vectorSelectedPoint.size() != 1 ) break;

			switch( m_uSelectedType )
			{
			case	TBM_DOME	:	// 천장
			case	TBM_DOME4	:	// 천장
				{
					int nOffsetX , nOffsetZ;

					nOffsetX = GetXOffsetStep( point.x );
					nOffsetZ = GetZOffsetStep( point.y );

					int	nDeltaX , nDeltaZ;

					nDeltaX = nOffsetX - m_vectorSelectedPoint[ 0 ].x;
					nDeltaZ = nOffsetZ - m_vectorSelectedPoint[ 0 ].y;

					POINT	pt = m_vectorSelectedPoint[ 0 ];

					m_nSelectMode = SM_LINE;
					
					if( nDeltaX >= ( int ) m_pSelectedElement->GetData().uWidth )
					{
						// X축으로 오른쪽이동
						pt.x += m_pSelectedElement->GetData().uWidth;
						m_vectorSelectedPoint.push_back( pt );
					}
					else if( nDeltaX < 0 )
					{
						// X축으로 왼쪽쪽이동
						pt.x -= m_pSelectedElement->GetData().uWidth;
						m_vectorSelectedPoint.push_back( pt );
					}
					else if( nDeltaZ >= ( int ) m_pSelectedElement->GetData().uHeight )
					{
						// Z축으로 아래로 이동..
						pt.y += m_pSelectedElement->GetData().uHeight;
						m_vectorSelectedPoint.push_back( pt );
					}
					else if( nDeltaZ < 0 )
					{
						// Z축으로 위로 이동..
						pt.y -= m_pSelectedElement->GetData().uHeight;
						m_vectorSelectedPoint.push_back( pt );
					}
					else
					{
						m_nSelectMode = SM_NONE;
					}

					if( m_nSelectMode == SM_LINE )
					{
						Invalidate( FALSE );
					}
				}
				break;
			}

		}
		break;
	}



	/*
	#ifdef _DEBUG
	{
		int nOffsetX , nOffsetZ;

		nOffsetX = GetXOffsetStep( point.x );
		nOffsetZ = GetZOffsetStep( point.y );
		TRACE( "던젼툴 내 좌표 %02d,%02d\n" , nOffsetX , nOffsetZ );
	}
	#endif
	*/

	CWnd::OnMouseMove(nFlags, point);
}

BOOL	CDungeonWnd::DrawElement( CDC * pDC , UINT32 uType , DungeonElement * pEle , UINT32 uSelected )
{
	int x = GetScreenX( pEle->uOffsetX );
	int y = GetScreenY( pEle->uOffsetZ );

	CRect	rect;
	GetClientRect( rect );

	// 1번째 사각형: AX1, AX2, AY1, AY2
	// 2번째 사각형: BX1, BX2, BY1, BY2
	//
	// 이런 경우에 서로 겹치는지 체크한다 
	//
	// if (AX2 > BX1 && BX2 > AX1)
	//  if (AY2 > BY1 && BY2 > AY1)
	//   return true;
	// else return false;

	// 1 : pEle
	// 2 : pElement

	INT32	AX1 , AX2 , BX1 , BX2 , AY1 , AY2 , BY1 , BY2;
	AX1	= x;
	AX2 = x + pEle->uWidth * m_nGridWidth;
	BX1	= 0;
	BX2 = rect.Width();

	AY1	= y;
	AY2 = y + pEle->uHeight * m_nGridWidth;
	BY1	= 0;
	BY2 = rect.Height();

	int nLayer = MAX_DUNGEON_HEIGHT_COUNT;

	if( m_bControlState )
	{
		uSelected = DET_HEIGHT;
		CDungeonTemplate * pTemplate = GetCurrentTemplate();

		for( int i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
		{
			if( pEle->fHeight == pTemplate->afSampleHeight[ i ] )
			{
				nLayer = i;
				break;
			}
		}
	}

	COLORREF	colorHeight	= __ColorTable[ nLayer ];

	#define SELECTED_COLOR	RGB( 255 , 255 , 34 )
	
	if (AX2 > BX1 && BX2 > AX1)
	{
		if (AY2 > BY1 && BY2 > AY1)
		{
			// 범위에 포함되는경우만..
			switch( uType )
			{
			case	TBM_FLOOR	:	// 바닥
				{
					COLORREF	color;
					switch( uSelected )
					{
					default:
					case DET_NORMAL		: color	= RGB( 34 , 180 , 34 );	break;
					//case DET_SELECTED	: color	= SELECTED_COLOR;	break;
					case DET_HEIGHT		: color = colorHeight; break;
					}

					pDC->FillSolidRect( x , y , m_nGridWidth * pEle->uWidth , m_nGridWidth * pEle->uHeight , color );

					CPen	pen;
					pen.CreatePen( PS_SOLID , 3 , RGB( 0 , 0 , 0 ) );
					pDC->SelectObject( pen );
					pDC->MoveTo( x , y			);
					pDC->LineTo( x + m_nGridWidth * pEle->uWidth , y	);
					pDC->LineTo( x + m_nGridWidth * pEle->uWidth , y + m_nGridWidth * pEle->uHeight	);
					pDC->LineTo( x , y + m_nGridWidth * pEle->uHeight	);
					pDC->LineTo( x , y	);

				}
				break;

			case	TBM_WALL2	:	// 벽
			case	TBM_WALL4	:	// 벽

				{
					#define SET_WALL_RECT( rectInput , x , y , cx , cy ) ( rectInput ).SetRect( (x) , (y) , (x) + (cx) , (y) + (cy) )

					CRect		rectWall	;
					COLORREF	colorWall	;

					switch( uSelected )
					{
					default:
					case DET_NORMAL		: colorWall	= RGB( 34 , 180 , 180 );	break;
					case DET_SELECTED	: colorWall	= SELECTED_COLOR;	break;
					case DET_HEIGHT		: colorWall = colorHeight; break;
					}

					if( m_nGridWidth == 1 )
					{
						SET_WALL_RECT( rectWall , x , y , m_nGridWidth * pEle->uWidth ,  m_nGridWidth * pEle->uHeight );
					}
					else
					{
						int	nMargin = ( m_nGridWidth / 2 ) > 0 ? ( m_nGridWidth / 2 ) : 1;

						switch( pEle->uTurnAngle )
						{
						case	DTA_0_CW	:
							SET_WALL_RECT( rectWall , x , y , m_nGridWidth * pEle->uWidth , nMargin  );	break;
						case	DTA_90_CW	:
							SET_WALL_RECT( rectWall , x + m_nGridWidth * pEle->uWidth - nMargin , y , nMargin , m_nGridWidth * pEle->uHeight );	break;
						case	DTA_180_CW	:
							SET_WALL_RECT( rectWall , x , y + m_nGridWidth * pEle->uHeight - nMargin , m_nGridWidth * pEle->uWidth , nMargin );	break;
						case	DTA_270_CW	:
							SET_WALL_RECT( rectWall , x , y , nMargin , m_nGridWidth * pEle->uHeight );	break;
						}
					}

					CBrush	brush;
					CPen	blackPen;
					//brush.CreateStockObject( HOLLOW_BRUSH );
					brush.CreateSolidBrush( colorWall );
					blackPen.CreateStockObject( BLACK_PEN );

					pDC->SelectObject	( brush			);
					pDC->SelectObject	( blackPen		);
					pDC->Rectangle		( rectWall		);
					pDC->SetBkMode		( TRANSPARENT	);
					//pDC->FillSolidRect( rectWall , colorWall );

					if( pEle->uIndex )
					{
						CString	str;
						str.Format( "%d" , pEle->uIndex );
						pDC->DrawText( str , rectWall , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
					}
				}
				break;	
			case	TBM_PILLAR	:	// 기둥
				{
					rect.left	= x - m_nGridWidth / 2 ;
					rect.top	= y - m_nGridWidth / 2 ;
					rect.right	= x + m_nGridWidth / 2 ;
					rect.bottom	= y + m_nGridWidth / 2 ;

					CBrush brush;
					switch( uSelected )
					{
					default:
					case DET_NORMAL		: brush.CreateStockObject( GRAY_BRUSH );	break;
					case DET_SELECTED	: brush.CreateSolidBrush( SELECTED_COLOR );	break;
					case DET_HEIGHT		: brush.CreateSolidBrush( colorHeight ); break;
					}
					
					pDC->SelectObject( brush );
					pDC->Ellipse( rect );

					if( pEle->uIndex )
					{
						CString	str;
						str.Format( "%d" , pEle->uIndex );
						pDC->DrawText( str , rect , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
					}
				}
				break;
			case	TBM_DOME	:	// 천장
			case	TBM_DOME4	:	// 천장
				{
					#define	SIDE_COLOR	RGB( 200 , 38 , 38 )
					COLORREF	colorDome	;
					COLORREF	colorSide	;

					switch( uSelected )
					{
					default:
					case DET_NORMAL		: colorDome	= RGB( 255 , 34 , 34 )	; colorSide = SIDE_COLOR	;	break;
					case DET_SELECTED	: colorDome	= SELECTED_COLOR		; colorSide = SELECTED_COLOR;	break;
					case DET_HEIGHT		: colorDome = colorHeight			; colorSide = colorHeight	;	break;
					}

					pDC->FillSolidRect( x , y , m_nGridWidth * pEle->uWidth , m_nGridWidth * pEle->uHeight, colorDome );

					{
						// 천장 방향 표시..
						CRect	rectDome;
						#define SET_RECT( rectInput , x , y , cx , cy ) ( rectInput ).SetRect( (x) , (y) , (x) + (cx) , (y) + (cy) )
						int	nMargin = ( m_nGridWidth / 2 * pEle->uWidth ) > 0 ? ( m_nGridWidth / 2 * pEle->uWidth ) : 1;

						switch( pEle->uIndex )
						{
						case	DT_BASE		:
							{
								// do nothing..
							}
							break;
						case	DT_SIDE		:
							{
								switch( pEle->uTurnAngle )
								{
								case	DTA_0_CW	:
									SET_RECT( rectDome , x , y , m_nGridWidth * pEle->uWidth , nMargin  );	break;
								case	DTA_90_CW	:
									SET_RECT( rectDome , x + m_nGridWidth * pEle->uWidth - nMargin , y , nMargin , m_nGridWidth * pEle->uHeight );	break;
								case	DTA_180_CW	:
									SET_RECT( rectDome , x , y + m_nGridWidth * pEle->uHeight - nMargin , m_nGridWidth * pEle->uWidth , nMargin );	break;
								case	DTA_270_CW	:
									SET_RECT( rectDome , x , y , nMargin , m_nGridWidth * pEle->uHeight );	break;
								}
								pDC->FillSolidRect( rectDome , colorSide );
							}
							break;
						case	DT_CORNAR	:
							{
								switch( pEle->uTurnAngle )
								{
								case	DTA_0_CW	:
									SET_RECT( rectDome , x , y , m_nGridWidth * pEle->uWidth , nMargin  );
									pDC->FillSolidRect( rectDome , colorSide );
									SET_RECT( rectDome , x + m_nGridWidth * pEle->uWidth - nMargin , y , nMargin , m_nGridWidth * pEle->uHeight );
									pDC->FillSolidRect( rectDome , colorSide );
									break;
								case	DTA_90_CW	:
									SET_RECT( rectDome , x + m_nGridWidth * pEle->uWidth - nMargin , y , nMargin , m_nGridWidth * pEle->uHeight );
									pDC->FillSolidRect( rectDome , colorSide );
									SET_RECT( rectDome , x , y + m_nGridWidth * pEle->uHeight - nMargin , m_nGridWidth * pEle->uWidth , nMargin );
									pDC->FillSolidRect( rectDome , colorSide );
									break;
								case	DTA_180_CW	:
									SET_RECT( rectDome , x , y + m_nGridWidth * pEle->uHeight - nMargin , m_nGridWidth * pEle->uWidth , nMargin );
									pDC->FillSolidRect( rectDome , colorSide  );
									SET_RECT( rectDome , x , y , nMargin , m_nGridWidth * pEle->uHeight );
									pDC->FillSolidRect( rectDome , colorSide );
									break;
								case	DTA_270_CW	:
									SET_RECT( rectDome , x , y , nMargin , m_nGridWidth * pEle->uHeight );
									pDC->FillSolidRect( rectDome , colorSide );
									SET_RECT( rectDome , x , y , m_nGridWidth * pEle->uWidth , nMargin  );
									pDC->FillSolidRect( rectDome , colorSide );
									break;
								}
							}
							break;
						}
					}

					// 테두리
					CBrush	brush;
					CPen	blackPen;
					brush.CreateStockObject( HOLLOW_BRUSH );
					blackPen.CreateStockObject( BLACK_PEN );

					pDC->SelectObject	( brush			);
					pDC->SelectObject	( blackPen		);
					pDC->Rectangle		( x , y , x + m_nGridWidth * pEle->uWidth , y + m_nGridWidth * pEle->uHeight	);
				}
				break;
			case	TBM_FENCE	:	// 난간
			case	TBM_STAIRS	:	// 계단
			case	TBM_SLOPE	:	// 경사
			default:
				// Error..
				{
					COLORREF	color;
					switch( uSelected )
					{
					default:
					case DET_NORMAL		: color	= RGB( 0 , 0 , 0 );		break;
					//case DET_SELECTED	: color	= SELECTED_COLOR;	break;
					case DET_HEIGHT		: color = colorHeight; break;
					}

					pDC->FillSolidRect( x , y , m_nGridWidth , m_nGridWidth , color );
				}
				break;
			}
		}
	}

	return TRUE;
}

CDungeonWnd::DungeonElement *	CDungeonWnd::GetElementByObjectIndex( INT32 nObjectIndex )
{
	// 오브젝트 인덱스가 있는지 점검..
	DungeonElement				* pElement	= NULL;
	AuNode< DungeonElement >	* pNode		= NULL;

	for( int nType = 0 ; nType < TBM_SELECT ; ++ nType )
	{
		pNode = m_listDungeonElement[ nType ].GetHeadNode();
		while( pNode )
		{
			pElement = & pNode->GetData();

			if( pElement->nObjectIndex == nObjectIndex )
			{
				return pElement;
			}

			pNode = pNode->GetNextNode();
		}
	}

	return NULL;
}

AuNode< CDungeonWnd::DungeonElement > * CDungeonWnd::GetElement( UINT32 uType , int nOffsetX , int nOffsetZ , int nDeltaX , int nDeltaZ )
{
	DungeonElement				* pElement	= NULL;
	AuNode< DungeonElement >	* pNode		= NULL;
	BOOL	bFound = FALSE;

	// 중복 검사..

	pNode = m_listDungeonElement[ uType ].GetTailNode();

	while( pNode )
	{
		pElement = & pNode->GetData();

		// 1번째 사각형: AX1, AX2, AY1, AY2
		// 2번째 사각형: BX1, BX2, BY1, BY2
		//
		// 이런 경우에 서로 겹치는지 체크한다 
		//
		// if (AX2 > BX1 && BX2 > AX1)
		//  if (AY2 > BY1 && BY2 > AY1)
		//   return true;
		// else return false;

		// 1 : pEle
		// 2 : pElement

		UINT32	AX1 , AX2 , BX1 , BX2 , AY1 , AY2 , BY1 , BY2;
		AX1	= nOffsetX;
		AX2 = nOffsetX + 1;
		BX1	= pElement->uOffsetX;
		BX2 = pElement->uOffsetX + pElement->uWidth;

		AY1	= nOffsetZ;
		AY2 = nOffsetZ + 1;
		BY1	= pElement->uOffsetZ;
		BY2 = pElement->uOffsetZ + pElement->uHeight;

		if (AX2 > BX1 && BX2 > AX1)
		{
			if (AY2 > BY1 && BY2 > AY1)
			{
				// 중복
				// 같은거 있어요~
				switch( uType )
				{
				case 	TBM_WALL2	:
					{
						switch( pElement->uTurnAngle )
						{
						case	DTA_0_CW	:
							// 위에만..
							if( nDeltaZ < 50 ) return pNode;
							break;
						case	DTA_90_CW	:
							// 오른쪽..
							if( nDeltaX > 50 ) return pNode;
							break;
						case	DTA_180_CW	:
							// 아래.
							if( nDeltaZ > 50 ) return pNode;
							break;
						case	DTA_270_CW	:
							// 왼쪽
							if( nDeltaX < 50 ) return pNode;
							break;
						}
					}
					break;
		
				default:
					return pNode;
				}
			}
		}

		pNode = pNode->GetPrevNode();
	}

	return NULL;
}

/*
INT32	CDungeonWnd::GetPortion( DungeonElement * , int nOffsetX , int nOffsetZ )
{
	return 0;
}
*/

void CDungeonWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int nOffsetX , nOffsetZ;
	int	nDeltaX , nDeltaZ;

	nOffsetX = GetXOffset( point.x , &nDeltaX );
	nOffsetZ = GetZOffset( point.y , &nDeltaZ );

	TRACE( "%d,%d\n" , nOffsetX , nOffsetZ );
	
	CDungeonTemplate * pTemplate = GetCurrentTemplate();

	if( pTemplate && nOffsetX >= 0 && nOffsetX < 512	&&
		nOffsetZ >= 0 && nOffsetZ < 512	)
	{
		int nType = g_pMainFrame->m_pDungeonToolbar->GetItem();
		switch( nType )
		{
		case	TBM_WALL2	:	// 벽
		case	TBM_WALL4	:	// 벽
			{
				// 분리!
				AuNode< DungeonElement >	* pNode		= NULL;

				pNode = GetElement( TBM_WALL4 , nOffsetX , nOffsetZ , nDeltaX , nDeltaZ );

				if( NULL != pNode )
				{
					// 분리~..
					DungeonElement	* pElement = & pNode->GetData();

					DungeonElement elementA , elementB;

					elementA = * pElement;
					elementB = * pElement;

					elementA.nObjectIndex = 0;
					elementB.nObjectIndex = 0;
		
					// 가로냐 세로냐..
					if( 1 == pElement->uHeight )
					{
						elementA.uWidth		/= 2;
						elementB.uWidth		/= 2;
						elementB.uOffsetX	+= elementA.uWidth;

						//m_listDungeonElement[ TBM_WALL4 ].RemoveNode( pNode );
						RemoveElement( TBM_WALL4 , pNode );

						//m_listDungeonElement[ TBM_WALL2 ].AddTail( elementA );
						AddElement( TBM_WALL2 , &elementA );
						//m_listDungeonElement[ TBM_WALL2 ].AddTail( elementB );
						AddElement( TBM_WALL2 , &elementB );

						Invalidate( FALSE );
					}
					else
					if( 1 == pElement->uWidth )
					{
						elementA.uHeight	/= 2;
						elementB.uHeight	/= 2;
						elementB.uOffsetZ	+= elementA.uHeight;

						//m_listDungeonElement[ TBM_WALL4 ].RemoveNode( pNode );
						RemoveElement( TBM_WALL4 , pNode );

						//m_listDungeonElement[ TBM_WALL2 ].AddTail( elementA );
						AddElement( TBM_WALL2 , &elementA );
						//m_listDungeonElement[ TBM_WALL2 ].AddTail( elementB );
						AddElement( TBM_WALL2 , &elementB );

						Invalidate( FALSE );
					}
					else
					{
					}	
				}
			}
			break;
		default:
			break;
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CDungeonWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nOffsetX , nOffsetZ;
	int	nDeltaX , nDeltaZ;

	nOffsetX = GetXOffset( point.x , &nDeltaX );
	nOffsetZ = GetZOffset( point.y , &nDeltaZ );

	TRACE( "%d,%d\n" , nOffsetX , nOffsetZ );

	CDungeonTemplate * pTemplate = GetCurrentTemplate();

	if( pTemplate && nOffsetX >= 0 && nOffsetX < 512	&&
		nOffsetZ >= 0 && nOffsetZ < 512	)
	{
		int nType = g_pMainFrame->m_pDungeonToolbar->GetItem();
		switch( nType )
		{
		case	TBM_FLOOR	:	// 바닥
		case	TBM_WALL2	:	// 벽
		case	TBM_WALL4	:	// 벽
		case	TBM_FENCE	:	// 난간
			{
				AuNode< DungeonElement >	* pNode		= NULL;

				pNode = GetElement( nType , nOffsetX , nOffsetZ , nDeltaX , nDeltaZ );

				if( NULL == pNode && TBM_WALL2 == nType )
				{
					pNode = GetElement( TBM_WALL4 , nOffsetX , nOffsetZ , nDeltaX , nDeltaZ );
					if( NULL != pNode ) nType = TBM_WALL4;
				}

				if( NULL == pNode || ISBUTTONDOWN( VK_SHIFT ) )
				{
					m_nSelectMode	= SM_RANGE;
					SetCapture();
	
					m_SelectedPosX1	 = m_SelectedPosX2 = nOffsetX;
					m_SelectedPosZ1	 = m_SelectedPosZ2 = nOffsetZ;

					// 스탭 싸이즈 적용..
					m_SelectedPosX2	+= GetSize( nType ) - 1;
					m_SelectedPosZ2	+= GetSize( nType ) - 1;
				}
				else
				{
					// 선택 모드..
					m_pSelectedElement	= pNode;
					m_uSelectedType		= nType;					
				}

				Invalidate( FALSE );
			}
			break;
		case	TBM_DOME	:	// 천장
		case	TBM_DOME4	:	// 천장
			{
				AuNode< DungeonElement >	* pNode		= NULL;

				pNode = GetElement( nType , nOffsetX , nOffsetZ , nDeltaX , nDeltaZ );

				/*
				if( NULL == pNode )
				{
					pNode = GetElement( TBM_DOME , nOffsetX , nOffsetZ , nDeltaX , nDeltaZ );
					if( NULL != pNode ) nType = TBM_DOME;
				}
				*/

				if( NULL == pNode || ISBUTTONDOWN( VK_SHIFT ) )
				{
					/*
					if( ISBUTTONDOWN( VK_MENU ) )
					{
						// 한개짜리 벽 놓기..
						nType = TBM_DOME;
					}
					*/

					switch( nType )
					{
					case TBM_DOME4:
					case TBM_DOME:
						{
							m_nSelectMode	= SM_RANGE;
							SetCapture();
			
							m_SelectedPosX1	 = m_SelectedPosX2 = nOffsetX;
							m_SelectedPosZ1	 = m_SelectedPosZ2 = nOffsetZ;

							// 스탭 싸이즈 적용..
							m_SelectedPosX2	+= GetSize( nType ) - 1;
							m_SelectedPosZ2	+= GetSize( nType ) - 1;
						}
						break;
					/*
					case TBM_DOME:
						{
							nOffsetX = GetXOffset( point.x + m_nGridWidth / 2 );
							nOffsetZ = GetZOffset( point.y + m_nGridWidth / 2 );

							if( nOffsetX >= 0 && nOffsetX < 512	&&
								nOffsetZ >= 0 && nOffsetZ < 512	)
							{
								CDungeonTemplate * pTemplate = GetCurrentTemplate();
								if( NULL == pTemplate ) break;
								// 추가
								DungeonElement element;
								
								element.	uTemplateID		= GetCurrentTemplateID();

								// 위치정보..
								element.	fHeight			= g_pMainFrame->m_pDungeonToolbar->GetHeight();
								element.	uTurnAngle		= 0;

								element.	uOffsetX		= nOffsetX;
								element.	uOffsetZ		= nOffsetZ;

								AddElement( nType , &element );	
							}
						}
						break;
					*/
					default:
						break;
					}
				}
				else
				{
					// 선택 모드..
					m_pSelectedElement	= pNode;
					m_uSelectedType		= nType;

					// 클릭한 포션을 저장해둠..
					m_eSelectedPortion	= LEFTTOP;

					if( ( point.x - GetScreenX( pNode->GetData().uOffsetX ) ) * 100 / ( m_nGridWidth * pNode->GetData().uWidth	) < 50 )
					{
						if( ( point.y - GetScreenY( pNode->GetData().uOffsetZ ) ) * 100 / ( m_nGridWidth * pNode->GetData().uHeight	) < 50 )
						{
							m_eSelectedPortion	= LEFTTOP;
						}
						else
						{
							m_eSelectedPortion	= LEFTBOTTOM;
						}
					}
					else
					{
						if( ( point.y - GetScreenY( pNode->GetData().uOffsetZ ) ) * 100 / ( m_nGridWidth * pNode->GetData().uHeight	) < 50 )
						{
							m_eSelectedPortion	= RIGHTTOP;
						}
						else
						{
							m_eSelectedPortion	= RIGHTBOTTOM;
						}
					}

					m_vectorSelectedPoint.clear();

					//if( m_pSelectedElement->GetData().uIndex == 0 )
					//{
						// 0이 민짜.. 민자에서만 연결된다.
						POINT	pt;
						pt.x	= pNode->GetData().uOffsetX;
						pt.y	= pNode->GetData().uOffsetZ;
						m_vectorSelectedPoint.push_back( pt );
					//}
				}

				Invalidate( FALSE );
			}
			break;
		case	TBM_PILLAR	:	// 기둥
			{
				AuNode< DungeonElement >	* pNode		= NULL;

				pNode = GetElement( nType , nOffsetX , nOffsetZ , nDeltaX , nDeltaZ );
				if( pNode )
				{
					// 선택 모드..
					m_pSelectedElement	= pNode;
					m_uSelectedType		= nType;					
				}
				else
				{
					if( !ISBUTTONDOWN( VK_SHIFT )  )
					{
						nOffsetX = GetXOffset( point.x + m_nGridWidth / 2 );
						nOffsetZ = GetZOffset( point.y + m_nGridWidth / 2 );

						if( nOffsetX >= 0 && nOffsetX < 512	&&
							nOffsetZ >= 0 && nOffsetZ < 512	)
						{
							CDungeonTemplate * pTemplate = GetCurrentTemplate();
							if( NULL == pTemplate ) break;
							// 추가
							DungeonElement element;
							
							element.	uTemplateID		= GetCurrentTemplateID();

							// 위치정보..
							element.	fHeight			= g_pMainFrame->m_pDungeonToolbar->GetHeight();
							element.	uTurnAngle		= 0;

							element.	uOffsetX		= nOffsetX;
							element.	uOffsetZ		= nOffsetZ;

							AddElement( nType , &element );	
						}
					}
					else
					{
						m_nSelectMode = SM_RANGE;
						SetCapture();
						m_SelectedPosX1	 = m_SelectedPosX2 = nOffsetX;
						m_SelectedPosZ1	 = m_SelectedPosZ2 = nOffsetZ;
						// 스탭 싸이즈 적용..
						m_SelectedPosX2	+= GetSize( nType ) - 1;
						m_SelectedPosZ2	+= GetSize( nType ) - 1;
					}
				}				
				Invalidate( FALSE );
			}
			break;
		case	TBM_STAIRS	:	// 계단
		case	TBM_SLOPE	:	// 경사
			
		default:
			break;
		}
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CDungeonWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	switch( m_nSelectMode )
	{
	case SM_NONE:
		if( m_pSelectedElement )
		{
			// 선택된것이 있으면..

			int nOffsetX , nOffsetZ;
			int	nDeltaX , nDeltaZ;

			nOffsetX = GetXOffset( point.x , &nDeltaX );
			nOffsetZ = GetZOffset( point.y , &nDeltaZ );


			DungeonElement				* pPrevElement	= NULL;
			DungeonElement				* pElement		= NULL;

			// 중복 검사..
			AuNode< DungeonElement > *	pNextSelectedElement = NULL;
			pNextSelectedElement = GetElement( m_uSelectedType , nOffsetX , nOffsetZ , nDeltaX , nDeltaZ );

			if( m_pSelectedElement == pNextSelectedElement )
			{
				switch( m_uSelectedType )
				{
				case TBM_DOME4:
				case TBM_DOME:
					break;
				default:
					{
						// 타입변경..
						DungeonElement				* pElement	= NULL;

						pElement = & m_pSelectedElement->GetData();

						CDungeonTemplate * pTemplate = GetTemplate( pElement->uTemplateID );

						if( pTemplate->uTemplateIndex[ m_uSelectedType ][ 0 ].uIndex	||
							pTemplate->uTemplateIndex[ m_uSelectedType ][ 1 ].uIndex	||
							pTemplate->uTemplateIndex[ m_uSelectedType ][ 2 ].uIndex	)
						{
							// 인덱스 변경.. 
							do
							{
								pElement->uIndex = ( pElement->uIndex + 1 ) % MAX_DUNGEON_INDEX_COUNT;
							} while( !pTemplate->uTemplateIndex[ m_uSelectedType ][ pElement->uIndex ].uIndex );

							// 추가 삭제..
							DungeonElement	element = * pElement;
							RemoveElement( m_uSelectedType , m_pSelectedElement );
							AddElement( m_uSelectedType , &element );
						}
					}
					break;
				}
			}
			else
			{
				if( pNextSelectedElement && m_uSelectedType == TBM_WALL2 )
				{
					// 합치기..
					pPrevElement	= & m_pSelectedElement	->GetData();
					pElement		= & pNextSelectedElement->GetData();

					// 바로 붙어있는 녀석인지 확인한다..
					// 벽이므로 가로나 세로로 뻗은 것이어야함..

					if( pPrevElement->uOffsetX == pElement->uOffsetX	)
					{
						// 세로로 뻗은것..
						// 옵셋 체크..
						if( pElement->uOffsetZ == pPrevElement->uOffsetZ + pPrevElement->uHeight )
						{
							pPrevElement->uHeight	+= pElement->uHeight;
							DungeonElement tmpElement;
							tmpElement = * pPrevElement;
							tmpElement.nObjectIndex = 0;

							//m_listDungeonElement[ TBM_WALL2 ].RemoveNode( pNextSelectedElement	);
							RemoveElement( TBM_WALL2 , pNextSelectedElement );
							//m_listDungeonElement[ TBM_WALL2 ].RemoveNode( m_pSelectedElement	);
							RemoveElement( TBM_WALL2 , m_pSelectedElement );

							//m_listDungeonElement[ TBM_WALL4 ].AddTail( tmpElement	);
							AddElement( TBM_WALL4 , &tmpElement );
						}
						else
						if( pPrevElement->uOffsetZ == pElement->uOffsetZ + pElement->uHeight )
						{
							pElement->uHeight	+= pPrevElement->uHeight;
							DungeonElement tmpElement;
							tmpElement = * pElement;
							tmpElement.nObjectIndex = 0;

							//m_listDungeonElement[ TBM_WALL2 ].RemoveNode( pNextSelectedElement	);
							RemoveElement( TBM_WALL2 , pNextSelectedElement );
							//m_listDungeonElement[ TBM_WALL2 ].RemoveNode( m_pSelectedElement	);
							RemoveElement( TBM_WALL2 , m_pSelectedElement );

							//m_listDungeonElement[ TBM_WALL4 ].AddTail( tmpElement	);
							AddElement( TBM_WALL4 , &tmpElement );
						}
						else
						{
							// 넘 멀리 있는거니까 신경 쓰지 않는다..
						}


					}

					if( pPrevElement->uOffsetZ == pElement->uOffsetZ	)
					{
						// 가로로 뻗은것..
						// 옵셋 체크..
						if( pElement->uOffsetX == pPrevElement->uOffsetX + pPrevElement->uWidth )
						{
							pPrevElement->uWidth	+= pElement->uWidth;
							DungeonElement tmpElement;
							tmpElement = * pPrevElement;
							tmpElement.nObjectIndex = 0;

							//m_listDungeonElement[ TBM_WALL2 ].RemoveNode( pNextSelectedElement	);
							RemoveElement( TBM_WALL2 , pNextSelectedElement );
							//m_listDungeonElement[ TBM_WALL2 ].RemoveNode( m_pSelectedElement	);
							RemoveElement( TBM_WALL2 , m_pSelectedElement );

							//m_listDungeonElement[ TBM_WALL4 ].AddTail( tmpElement	);
							AddElement( TBM_WALL4 , &tmpElement );
						}
						else
						if( pPrevElement->uOffsetX == pElement->uOffsetX + pElement->uWidth )
						{
							pElement->uWidth	+= pPrevElement->uWidth;
							DungeonElement tmpElement;
							tmpElement = * pElement;
							tmpElement.nObjectIndex = 0;

							//m_listDungeonElement[ TBM_WALL2 ].RemoveNode( pNextSelectedElement	);
							RemoveElement( TBM_WALL2 , pNextSelectedElement );
							//m_listDungeonElement[ TBM_WALL2 ].RemoveNode( m_pSelectedElement	);
							RemoveElement( TBM_WALL2 , m_pSelectedElement );

							//m_listDungeonElement[ TBM_WALL4 ].AddTail( tmpElement	);
							AddElement( TBM_WALL4 , &tmpElement );
						}
						else
						{
							// 넘 멀리 있는거니까 신경 쓰지 않는다..
						}
					}
				}
				else
				{
					// do nothing..
				}
			}

			m_pSelectedElement	= NULL;
			m_nSelectMode		= SM_NONE;
			Invalidate( FALSE );
		}
		break;
	case SM_RANGE	:
		{
			m_nSelectMode = SM_NONE;
			ReleaseCapture();

			// 마고자 (2004-09-10 오후 6:04:32) : 
			// 버튼을 뗄때 처리하는 녀석들..
			UINT32 nX1 , nX2 , nZ1 , nZ2;

			// 범위 표시줄..
			if( m_SelectedPosX1 < m_SelectedPosX2 )
			{
				nX1 = m_SelectedPosX1;
				nX2 = m_SelectedPosX2;
			}
			else									
			{
				nX1 = m_SelectedPosX2;
				nX2 = m_SelectedPosX1;
			}
			
			if( m_SelectedPosZ1 < m_SelectedPosZ2 )
			{
				nZ1 = m_SelectedPosZ1;
				nZ2 = m_SelectedPosZ2;
			}
			else									
			{
				nZ1 = m_SelectedPosZ2;
				nZ2 = m_SelectedPosZ1;
			}

			if( nX1 < 0 ) nX1 = 0;
			if( nX2 < 0 ) nX2 = 0;
			if( nZ1 < 0 ) nZ1 = 0;
			if( nZ2 < 0 ) nZ2 = 0;

			if( !ISBUTTONDOWN( VK_SHIFT ) )
			{
				if( m_bControlState )
				{		
					DungeonElement				* pElement	= NULL;
					AuNode< DungeonElement >	* pNode		= NULL;
					AuNode< DungeonElement >	* pNodeNext	= NULL;

					for( int nType = 0 ; nType < TBM_SELECT ; ++ nType )
					{
						pNode = m_listDungeonElement[ nType ].GetHeadNode();
						while( pNode )
						{
							pElement = & pNode->GetData();

							if( nX1 <= pElement->uOffsetX	&& pElement->uOffsetX <= nX2	&&
								nZ1 <= pElement->uOffsetZ	&& pElement->uOffsetZ <= nZ2	)
							{
								// 범위 포함된 것이다~..
								pElement->fHeight = g_pMainFrame->m_pDungeonToolbar->GetHeight();

								UpdateElement( nType , pElement );
							}
							else
							{
							}
							pNode = pNode->GetNextNode();
						}
					}

					Invalidate( FALSE );
				}
				else
				{
					CDungeonTemplate * pTemplate = GetCurrentTemplate();
					if( NULL == pTemplate )
					{
						MessageBox( "선택된 템플릿이 없어요." );
						Invalidate( FALSE );
						return;
					}

					INT32	nType = g_pMainFrame->m_pDungeonToolbar->GetItem();
					
					// 추가 통합..
					switch( nType )
					{
					case	TBM_FLOOR	:	// 바닥
						{
							// 추가
							DungeonElement element;
							
							element.	uTemplateID		= GetCurrentTemplateID();
							element.	uOffsetX		= nX1;
							element.	uOffsetZ		= nZ1;
							element.	uWidth			= nX2 - nX1 + 1 ; //TB_SIZES[ nType ]	;
							element.	uHeight			= nZ2 - nZ1 + 1 ; //TB_SIZES[ nType ]	;

							// 위치정보..
							element.	fHeight			= g_pMainFrame->m_pDungeonToolbar->GetHeight();
							element.	uTurnAngle		= 0;

							AddElement( nType , &element );
							Apply( TRUE );

							// 범위 안에 있는 오브젝트 높이 조절..

							{
								DungeonElement				* pElement	= NULL;
								AuNode< DungeonElement >	* pNode		= NULL;
								AuNode< DungeonElement >	* pNodeNext	= NULL;

								for( int nType = TBM_SLOPE ; nType < TBM_SELECT ; ++ nType )
								{
									pNode = m_listDungeonElement[ nType ].GetHeadNode();
									while( pNode )
									{
										pElement = & pNode->GetData();

										if( nX1 <= pElement->uOffsetX	&& pElement->uOffsetX <= nX2	&&
											nZ1 <= pElement->uOffsetZ	&& pElement->uOffsetZ <= nZ2	)
										{
											// 범위 포함된 것이다~..
											pElement->fHeight = g_pMainFrame->m_pDungeonToolbar->GetHeight();

											UpdateElement( nType , pElement );
										}
										else
										{
										}
										pNode = pNode->GetNextNode();
									}
								}
							}
						}
						break;
					case TBM_WALL2:
					case TBM_WALL4:
						{
							if( nX2 - nX1 + 1 < GetSize( nType ) ) break;
							if( nZ2 - nZ1 + 1 < GetSize( nType ) ) break;

							// 추가
							DungeonElement element;
							
							element.	uTemplateID		= GetCurrentTemplateID();

							// 위치정보..
							element.	fHeight			= g_pMainFrame->m_pDungeonToolbar->GetHeight();

							element.	uWidth			= GetSize( nType )	;
							element.	uHeight			= 1	;

							for( UINT32 i = nX1 ; i <= nX2 ; i += GetSize( nType ) )
							{

								element.	uOffsetX		= i			;
								element.	uOffsetZ		= nZ1		;
								element.	uTurnAngle		= DTA_0_CW	;
								AddElement( nType , &element );

								element.	uOffsetX		= i			;
								element.	uOffsetZ		= nZ2		;
								element.	uTurnAngle		= DTA_180_CW;
								AddElement( nType , &element );
							}

							element.	uWidth			= 1	;
							element.	uHeight			= GetSize( nType )	;

							for( UINT32 j = nZ1 ; j <= nZ2 ; j += GetSize( nType ) )
							{
								element.	uOffsetX		= nX1		;
								element.	uOffsetZ		= j			;
								element.	uTurnAngle		= DTA_270_CW;
								AddElement( nType , &element );

								element.	uOffsetX		= nX2		;
								element.	uOffsetZ		= j			;
								element.	uTurnAngle		= DTA_90_CW	;
								AddElement( nType , &element );
							}
						}
						break;
					case TBM_DOME:
					case TBM_DOME4:
						{
							// 템플릿 0 번은 민짜
							// 1번은 싸이드
							// 2번은 모서리..

							// 추가

							DungeonElement element;
							
							element.	uTemplateID		=  GetCurrentTemplateID();;

							// 위치정보..
							element.	fHeight			= g_pMainFrame->m_pDungeonToolbar->GetHeight();

							element.	uWidth			= GetSize( nType )	;
							element.	uHeight			= GetSize( nType )	;

							for( UINT32 i = nX1 ; i <= nX2 ; i += GetSize( nType ) )
							{
								for( UINT32 j = nZ1 ; j <= nZ2 ; j += GetSize( nType ) )
								{
									element.	uOffsetX		= i		;
									element.	uOffsetZ		= j		;

									/*
									if( i == nX1 && j == nZ1 )
									{
										// 왼쪽위 모서리
										element.	uIndex			= 2;
										element.	uTurnAngle		= DTA_270_CW	;
										AddElement( nType , &element );
									}
									else
									if( i + GetSize( nType ) - 1 == nX2  && j == nZ1 )
									{
										// 오른쪽 위 모서리
										element.	uIndex			= 2;
										element.	uTurnAngle		= DTA_0_CW	;
										AddElement( nType , &element );
									}
									else
									if( i + GetSize( nType ) - 1 == nX2 && j + GetSize( nType ) - 1 == nZ2 )
									{
										// 오른족 아래 모서리
										element.	uIndex			= 2;
										element.	uTurnAngle		= DTA_90_CW	;
										AddElement( nType , &element );
									}
									else
									if( i == nX1 && j + GetSize( nType ) - 1 == nZ2 )
									{
										// 왼쪽 아래 모서리.
										element.	uIndex			= 2;
										element.	uTurnAngle		= DTA_180_CW	;
										AddElement( nType , &element );
									}
									else
									if( j == nZ1 )
									{
										// 위쪽 싸이드
										element.	uIndex			= 1;
										element.	uTurnAngle		= DTA_0_CW	;
										AddElement( nType , &element );
									}
									else
									if( i + GetSize( nType ) - 1 == nX2 )
									{
										// 오른쪽 싸이드
										element.	uIndex			= 1;
										element.	uTurnAngle		= DTA_90_CW	;
										AddElement( nType , &element );
									}
									else
									if( j + GetSize( nType ) - 1 == nZ2 )
									{
										// 아래쪽 싸이드
										element.	uIndex			= 1;
										element.	uTurnAngle		= DTA_180_CW	;
										AddElement( nType , &element );
									}
									else
									if( i == nX1 )
									{
										// 왼쪽 싸이드
										element.	uIndex			= 1;
										element.	uTurnAngle		= DTA_270_CW	;
										AddElement( nType , &element );
									}
									else
									*/
									{
										// 민짜..
										element.	uIndex			= 0;
										element.	uTurnAngle		= DTA_0_CW	;
										AddElement( nType , &element );
									}
								}
							}

							element.	uWidth			= GetSize( nType )	;
							element.	uHeight			= GetSize( nType )	;
						}
						break;

					case TBM_PILLAR:
						{
							// 삭제..
							DungeonElement				* pElement	= NULL;
							AuNode< DungeonElement >	* pNode		= NULL;
							AuNode< DungeonElement >	* pNodeNext	= NULL;

							pNode = m_listDungeonElement[ g_pMainFrame->m_pDungeonToolbar->GetItem() ].GetHeadNode();
							while( pNode )
							{
								pElement = & pNode->GetData();

								if( nX1 <= pElement->uOffsetX	&& pElement->uOffsetX <= nX2	&&
									nZ1 <= pElement->uOffsetZ	&& pElement->uOffsetZ <= nZ2	)
								{
									// 범위 포함된 것이다~..
									// 삭제한다.

									pNodeNext = pNode->GetNextNode();

									// m_listDungeonElement[ g_pMainFrame->m_pDungeonToolbar->GetItem() ].RemoveNode( pNode );
									RemoveElement( g_pMainFrame->m_pDungeonToolbar->GetItem() , pNode );

									pNode = pNodeNext;
								}
								else
								{
									pNode = pNode->GetNextNode();
								}
							}
						}
						break;
						
					default:
						break;
					}

					// 추가
					////////////////////////////////
				}
			}
			else
			{
				if( ISBUTTONDOWN( VK_CONTROL ) )
				{
					// Shift와 Control을 동시에 누르고 있으면 
					// 모든타입을 동시에 날린다.
					// 삭제..
					DungeonElement				* pElement	= NULL;
					AuNode< DungeonElement >	* pNode		= NULL;
					AuNode< DungeonElement >	* pNodeNext	= NULL;

					for( int nType = 0 ; nType < TBM_SELECT ; ++ nType )
					{
						pNode = m_listDungeonElement[ nType ].GetHeadNode();
						while( pNode )
						{
							pElement = & pNode->GetData();

							if( nX1 <= pElement->uOffsetX	&& pElement->uOffsetX <= nX2	&&
								nZ1 <= pElement->uOffsetZ	&& pElement->uOffsetZ <= nZ2	)
							{
								// 범위 포함된 것이다~..
								// 삭제한다.

								pNodeNext = pNode->GetNextNode();

								// m_listDungeonElement[ nType ].RemoveNode( pNode );
								RemoveElement( nType , pNode );

								pNode = pNodeNext;
							}
							else
							{
								pNode = pNode->GetNextNode();
							}
						}
					}
				}
				else
				// 삭제..
				switch( g_pMainFrame->m_pDungeonToolbar->GetItem() )
				{
				case	TBM_FLOOR	:	// 바닥
				case	TBM_WALL2	:
				case	TBM_WALL4	:
				case	TBM_PILLAR	:
				case	TBM_DOME4	:
				case	TBM_DOME	:
					{
						// 삭제..
						DungeonElement				* pElement	= NULL;
						AuNode< DungeonElement >	* pNode		= NULL;
						AuNode< DungeonElement >	* pNodeNext	= NULL;

						pNode = m_listDungeonElement[ g_pMainFrame->m_pDungeonToolbar->GetItem() ].GetHeadNode();
						while( pNode )
						{
							pElement = & pNode->GetData();

							if( nX1 <= pElement->uOffsetX	&& pElement->uOffsetX <= nX2	&&
								nZ1 <= pElement->uOffsetZ	&& pElement->uOffsetZ <= nZ2	)
							{
								// 범위 포함된 것이다~..
								// 삭제한다.

								pNodeNext = pNode->GetNextNode();

								// m_listDungeonElement[ g_pMainFrame->m_pDungeonToolbar->GetItem() ].RemoveNode( pNode );
								RemoveElement( g_pMainFrame->m_pDungeonToolbar->GetItem() , pNode );

								pNode = pNodeNext;
							}
							else
							{
								pNode = pNode->GetNextNode();
							}
						}
					}
					break;
				default:
					break;
				}

				// 삭제..
				//////////////////////////
			}

			Invalidate( FALSE );
		}
		break;
	case SM_LINE	:
		{
			switch( m_uSelectedType )
			{
			case TBM_DOME:
				break;
			case TBM_DOME4:
				{
					int nType = g_pMainFrame->m_pDungeonToolbar->GetItem();

					// 일단 선택된 오브젝트 삭제.
					RemoveElement( m_uSelectedType , m_pSelectedElement );
					m_pSelectedElement = NULL;

					// 차곡차곡 추가함..
					if( m_vectorSelectedPoint.size() >= 2 )
					{
						INT32	nDir		= DIR_UP;	// 안쪽이 향하는 방향..
						INT32	nMovement	;	// 현재 이동할 방향..
						INT32	nLastMovement;

						BOOL	bAbort = FALSE;

						nMovement = GetMovement( 0 );
						switch( nMovement )
						{
						case	DIR_UP		:
							switch( m_eSelectedPortion )
							{
							case	LEFTTOP		:	nDir = DIR_LEFT		;	break;
							case	RIGHTTOP	:	nDir = DIR_RIGHT	;	break;
							case	LEFTBOTTOM	:	nDir = DIR_LEFT		;	break;
							case	RIGHTBOTTOM	:	nDir = DIR_RIGHT	;	break;
							}
							break;
						case	DIR_DOWN	:
							switch( m_eSelectedPortion )
							{
							case	LEFTTOP		:	nDir = DIR_LEFT		;	break;
							case	RIGHTTOP	:	nDir = DIR_RIGHT	;	break;
							case	LEFTBOTTOM	:	nDir = DIR_LEFT		;	break;
							case	RIGHTBOTTOM	:	nDir = DIR_RIGHT	;	break;
							}
							break;
						case	DIR_LEFT	:
							switch( m_eSelectedPortion )
							{
							case	LEFTTOP		:	nDir = DIR_UP		;	break;
							case	RIGHTTOP	:	nDir = DIR_UP		;	break;
							case	LEFTBOTTOM	:	nDir = DIR_DOWN		;	break;
							case	RIGHTBOTTOM	:	nDir = DIR_DOWN		;	break;
							}
							break;
						case	DIR_RIGHT	:
							switch( m_eSelectedPortion )
							{
							case	LEFTTOP		:	nDir = DIR_UP		;	break;
							case	RIGHTTOP	:	nDir = DIR_UP		;	break;
							case	LEFTBOTTOM	:	nDir = DIR_DOWN		;	break;
							case	RIGHTBOTTOM	:	nDir = DIR_DOWN		;	break;
							}
							break;
						}

						nLastMovement = nMovement;

						POINT	*pPoint;
						
						DungeonElement element;
						
						element.	uTemplateID		=  GetCurrentTemplateID();;

						// 위치정보..
						element.	fHeight			= g_pMainFrame->m_pDungeonToolbar->GetHeight();

						element.	uWidth			= GetSize( nType )	;
						element.	uHeight			= GetSize( nType )	;				

						for( int i = 0 ; i < ( int ) m_vectorSelectedPoint.size() ; i ++ )
						{
							if( bAbort ) break;
							nMovement = GetMovement( i + 1 );
							pPoint = &m_vectorSelectedPoint[ i ];

							element.uOffsetX = pPoint->x;
							element.uOffsetZ = pPoint->y;

							switch( nDir )
							{
							case	DIR_UP	:
								switch( nMovement )
								{
								case	DIR_UP	:
									switch( nLastMovement )
									{
									case	DIR_UP	:
									case	DIR_DOWN:	bAbort = TRUE ; break; // 이쪽은 처리 안함..
									case	DIR_LEFT:	element.uIndex = DT_CORNAR; element.uTurnAngle = DTA_180_CW ; nDir = DIR_RIGHT; break;
									case	DIR_RIGHT:	element.uIndex = DT_CORNAR; element.uTurnAngle = DTA_90_CW ; nDir = DIR_LEFT; break;
									}
									break;
								case	DIR_DOWN:	bAbort = TRUE ; break; // 이쪽은 처리 안함..
								case	DIR_LEFT:	element.uIndex	= DT_SIDE; element.uTurnAngle = DTA_180_CW ; break;
								case	DIR_RIGHT:	element.uIndex	= DT_SIDE; element.uTurnAngle = DTA_180_CW ; break;
								}
								break;
							case	DIR_DOWN:
								switch( nMovement )
								{
								case	DIR_UP	:	bAbort = TRUE ; break; // 이쪽은 처리 안함..
								case	DIR_DOWN:
									switch( nLastMovement )
									{
									case	DIR_UP	:
									case	DIR_DOWN:	bAbort = TRUE ; break; // 이쪽은 처리 안함..
									case	DIR_LEFT:	element.uIndex = DT_CORNAR; element.uTurnAngle = DTA_270_CW ; nDir = DIR_RIGHT; break;
									case	DIR_RIGHT:	element.uIndex = DT_CORNAR; element.uTurnAngle = DTA_0_CW ; nDir = DIR_LEFT; break;
									}
									break;
								case	DIR_LEFT:	element.uIndex	= DT_SIDE; element.uTurnAngle = DTA_0_CW ; break;
								case	DIR_RIGHT:	element.uIndex	= DT_SIDE; element.uTurnAngle = DTA_0_CW ; break;
								}
								break;
							case	DIR_LEFT:
								switch( nMovement )
								{
								case	DIR_UP	:	element.uIndex	= DT_SIDE; element.uTurnAngle = DTA_90_CW ; break;
								case	DIR_DOWN:	element.uIndex	= DT_SIDE; element.uTurnAngle = DTA_90_CW ; break;
								case	DIR_LEFT:
									switch( nLastMovement )
									{
									case	DIR_UP	:	element.uIndex = DT_CORNAR; element.uTurnAngle = DTA_0_CW ; nDir = DIR_DOWN; break;
									case	DIR_DOWN:	element.uIndex = DT_CORNAR; element.uTurnAngle = DTA_90_CW ; nDir = DIR_UP; break;
									case	DIR_LEFT:	
									case	DIR_RIGHT:	bAbort = TRUE ; break; // 이쪽은 처리 안함..
									}
									break;
								case	DIR_RIGHT:	bAbort = TRUE ; break; // 이쪽은 처리 안함..
								}
								break;
							case	DIR_RIGHT:
								switch( nMovement )
								{
								case	DIR_UP	:	element.uIndex	= DT_SIDE; element.uTurnAngle = DTA_270_CW ; break;
								case	DIR_DOWN:	element.uIndex	= DT_SIDE; element.uTurnAngle = DTA_270_CW ; break;
								case	DIR_LEFT:	bAbort = TRUE ; break; // 이쪽은 처리 안함..
								case	DIR_RIGHT:
									switch( nLastMovement )
									{
									case	DIR_UP	:	element.uIndex = DT_CORNAR; element.uTurnAngle = DTA_270_CW ; nDir = DIR_DOWN; break;
									case	DIR_DOWN:	element.uIndex = DT_CORNAR; element.uTurnAngle = DTA_180_CW ; nDir = DIR_UP; break;
									case	DIR_LEFT:	
									case	DIR_RIGHT:	bAbort = TRUE ; break; // 이쪽은 처리 안함..
									}
									break;
								}
								break;
							}

							if( !bAbort )
								AddElement( nType , &element );

							nLastMovement = nMovement;
						}
						Invalidate( FALSE );
					}
					else
					{
						MessageBox( "라인그을땐 최소 2개 이상.." );
					}
				}
				break;
			default:
				break;
			}

			m_nSelectMode = SM_NONE;
			m_pSelectedElement = NULL;
			m_vectorSelectedPoint.clear();
			ReleaseCapture();

			Invalidate( FALSE );
		}
		break;
	}

	
	CWnd::OnLButtonUp(nFlags, point);
}

INT32	CDungeonWnd::GetMovement( INT32 nOffset )
{
	CDungeonTemplate * pTemplate = GetCurrentTemplate();
	if( NULL == pTemplate ) return DIR_UP;

	int nItem = g_pMainFrame->m_pDungeonToolbar->GetItem();
	int nSize =  GetSize( nItem );

	if( nOffset >= 0 && nOffset <= ( int ) m_vectorSelectedPoint.size() )
	{
		// 클램핑..
		//nOffset %= m_vectorSelectedPoint.size();

		POINT	Last	;
		POINT	Current	;

		if( nOffset == m_vectorSelectedPoint.size() )
		{
			// 처음과 끝..
			Last	= m_vectorSelectedPoint[ m_vectorSelectedPoint.size() - 1 ];
			Current	= m_vectorSelectedPoint[ 0 ];
			if( abs( Current.x - Last.x) + 
				abs( Current.y - Last.y) == nSize )
			{
				// 붙어있는 거라면...
				// 위상 계산해서 리턴..
				if( Current.x > Last.x )	return DIR_RIGHT;
				if( Current.x < Last.x )	return DIR_LEFT	;
				if( Current.y > Last.y )	return DIR_DOWN	;
				if( Current.y < Last.y )	return DIR_UP	;
			}
			else
			{
				// 붙어 있는게 아니라면..
				// 이전거 리턴
				return GetMovement( nOffset - 1 );
			}
		}
		else
		if( nOffset == 0 )
		{
			// 처음과 끝..
			Last	= m_vectorSelectedPoint[ m_vectorSelectedPoint.size() - 1 ];
			Current	= m_vectorSelectedPoint[ 0 ];

			if( abs( Current.x - Last.x) + 
				abs( Current.y - Last.y) == nSize )
			{
				// 붙어있는 거라면...
				// 위상 계산해서 리턴..
				if( Current.x > Last.x )	return DIR_RIGHT;
				if( Current.x < Last.x )	return DIR_LEFT	;
				if( Current.y > Last.y )	return DIR_DOWN	;
				if( Current.y < Last.y )	return DIR_UP	;
			}
			else
			{
				// 붙어 있는게 아니라면..
				// 다음거 리턴..
				ASSERT( m_vectorSelectedPoint.size() > 1 );
				return GetMovement( 1 );
			}

		}
		else
		{
			// 이어진것..
			Last	= m_vectorSelectedPoint[ nOffset - 1 ];
			Current	= m_vectorSelectedPoint[ nOffset ];

			if( Current.x > Last.x )	return DIR_RIGHT;
			if( Current.x < Last.x )	return DIR_LEFT	;
			if( Current.y > Last.y )	return DIR_DOWN	;
			if( Current.y < Last.y )	return DIR_UP	;
		}

		return DIR_UP;
	}
	else
	{
		return DIR_UP;
	}
}


BOOL	CDungeonWnd::UpdateElement	( UINT32 uType , DungeonElement * pElement )
{
	// DungeonElement 기준으로 실 오브젝트를 업데이트함 ( 주로 이동 )
	ApdObject			*	pstApdObject	= NULL;
	if( pElement->nObjectIndex )
		pstApdObject = g_pcsApmObject->GetObject( pElement->nObjectIndex );

	// 좌표계산.

	RwV3d	pos;
	FLOAT	fDeltaY = 0.0f;

	pos.x	= __GetWorldXFromElement( uType , pElement );
	pos.y	= pElement->fHeight	;
	pos.z	= __GetWorldZFromElement( uType , pElement );
	fDeltaY	= __GetWorldDeltaYFromElement( uType , pElement );

	// 오브젝트의 피봇이 왼쪽 위 포이트에 있어서 뺑뺑 돌려줘야함..

	if( uType == TBM_DOME4	||
		uType == TBM_DOME	)
	{
		//CDungeonTemplate * pTemplate = GetCurrentTemplate();
		pos.y += g_pMainFrame->m_pDungeonToolbar->UpdateDungeonWallHeight( pElement->uTemplateID );
	}


	// 오브젝트 월드에 추가하는 과정..
	if( 0 == pElement->nObjectIndex	||
		NULL == pstApdObject		)	// 오브젝트가 없는 경우도 추가함.
	{
		// 오브젝트 추가..
		ApdObjectTemplate	*	pstApdObjectTemplate	;
		ApdObject			*	pstApdObject	;

		// 오브젝트의 피봇이 왼쪽 위 포이트에 있어서 뺑뺑 돌려줘야함..

		CDungeonTemplate * pTemplate = GetTemplate( pElement->uTemplateID );

		if( pTemplate )
		{
			pstApdObjectTemplate	= g_pcsApmObject->GetObjectTemplate( pTemplate->uTemplateIndex[ uType ][ pElement->uIndex ].uIndex );
			if( pstApdObjectTemplate )
			{
				if( uType == TBM_DOME4	||
					uType == TBM_DOME	)
				{
					pstApdObject = g_pMainFrame->m_pTileList->m_pObjectWnd->AddObject( pstApdObjectTemplate , & pos , 
						ACUOBJECT_TYPE_DUNGEON_STRUCTURE | ACUOBJECT_TYPE_DUNGEON_DOME
						, fDeltaY );
				}
				else
				{
					pstApdObject = g_pMainFrame->m_pTileList->m_pObjectWnd->AddObject( pstApdObjectTemplate , & pos , ACUOBJECT_TYPE_DUNGEON_STRUCTURE , fDeltaY );
				}

				if( pstApdObject )
				{
					// 인덱스 저장..
					pElement->nObjectIndex = pstApdObject->m_lID;
				}
				else
				{
					MD_SetErrorMessage( "던젼 오브젝트 추가 실패" );
				}
			}
			else
			{
				TRACE( "템플릿 정보가 없어요 (%d)" , pTemplate->uTemplateIndex[ uType ][ pElement->uIndex ].uIndex );
			}
		}

		return TRUE;
	}
	else
	{
		// 오브젝트가 있는경우..
		// Translate 시킴.

		pstApdObject->m_stPosition.x	= pos.x		;
		pstApdObject->m_stPosition.y	= pos.y		;
		pstApdObject->m_stPosition.z	= pos.z		;
		pstApdObject->m_fDegreeY		= fDeltaY	;

		g_pcsAgcmObject->SetupObjectClump_TransformOnly( pstApdObject );

		return TRUE;
	}
}

BOOL	CDungeonWnd::AddElement( UINT32 uType , DungeonElement * pEle )
{
	ASSERT( uType < TBM_SELECT );

	if( uType >= TBM_SELECT ) return FALSE;

	SaveSetChangeObjectList();

	DungeonElement				* pElement	= NULL;
	AuNode< DungeonElement >	* pNode		= NULL;

	pEle->uType = uType;

	switch( uType )
	{
	case	TBM_DOME4	:	// 천장
	case	TBM_FLOOR	:	// 바닥
		{
			// 중복 검사..

			pNode = m_listDungeonElement[ uType ].GetHeadNode();
			while( pNode )
			{
				pElement = & pNode->GetData();

				// 1번째 사각형: AX1, AX2, AY1, AY2
				// 2번째 사각형: BX1, BX2, BY1, BY2
				//
				// 이런 경우에 서로 겹치는지 체크한다 
				//
				// if (AX2 > BX1 && BX2 > AX1)
				//  if (AY2 > BY1 && BY2 > AY1)
				//   return true;
				// else return false;

				// 1 : pEle
				// 2 : pElement

				UINT32	AX1 , AX2 , BX1 , BX2 , AY1 , AY2 , BY1 , BY2;
				AX1	= pEle->uOffsetX;
				AX2 = pEle->uOffsetX + pEle->uWidth;
				BX1	= pElement->uOffsetX;
				BX2 = pElement->uOffsetX + pElement->uWidth;

				AY1	= pEle->uOffsetZ;
				AY2 = pEle->uOffsetZ + pEle->uHeight;
				BY1	= pElement->uOffsetZ;
				BY2 = pElement->uOffsetZ + pElement->uHeight;

				if (AX2 > BX1 && BX2 > AX1)
				{
					if (AY2 > BY1 && BY2 > AY1)
					{
						// 중복
						// 같은거 있어요~
						return FALSE;
					}
				}

				pNode = pNode->GetNextNode();
			}

			// 겹치는거 없으니 추가~
			m_listDungeonElement[ uType ].AddTail( *pEle );
		}
		break;
	case	TBM_WALL2	:	// 벽
	case	TBM_WALL4	:	// 벽
		{
			// 턴 방향 결정..
				// 주위 바닥 타일을 조사함..
				
			// 해당 방향의 벽이 있는지 체크..
			// 없으면 삽입..

			pNode = m_listDungeonElement[ uType ].GetHeadNode();
			while( pNode )
			{
				pElement = & pNode->GetData();

				if( pEle->uOffsetX		== pElement->uOffsetX	&&
					pEle->uOffsetZ		== pElement->uOffsetZ	&&
					// 턴앵글은 중복될 수 있음..
					pEle->uTurnAngle	== pElement->uTurnAngle	)
				{
					// 같은거 있어요~
					return FALSE;
				}

				pNode = pNode->GetNextNode();
			}

			// 겹치는거 없으니 추가~
			m_listDungeonElement[ uType ].AddTail( *pEle );
		}
		break;
	case	TBM_SLOPE	:	// 경사
	case	TBM_STAIRS	:	// 계단
	case	TBM_FENCE	:	// 난간
	case	TBM_PILLAR	:	// 기둥
	default:
		m_listDungeonElement[ uType ].AddTail( *pEle );
		break;
	}

	// 마지막 데이타를 얻어냄..
	pElement = & m_listDungeonElement[ uType ].GetTailNode()->GetData();

	// 오브젝트 월드에 추가하는 과정..
	//if( pElement->nObjectIndex == 0 )
	{
		// 오브젝트 추가..
		ApdObjectTemplate	*	pstApdObjectTemplate	;
		ApdObject			*	pstApdObject	;
		RwV3d	pos;
		FLOAT	fDeltaY = 0.0f;

		pos.x	= __GetWorldXFromElement( uType , pElement );
		pos.y	= pElement->fHeight	;
		pos.z	= __GetWorldZFromElement( uType , pElement );
		fDeltaY	= __GetWorldDeltaYFromElement( uType , pElement );

		// 오브젝트의 피봇이 왼쪽 위 포이트에 있어서 뺑뺑 돌려줘야함..

		if( uType == TBM_DOME4	||
			uType == TBM_DOME	)
		{
			//CDungeonTemplate * pTemplate = GetCurrentTemplate();
			pos.y += g_pMainFrame->m_pDungeonToolbar->UpdateDungeonWallHeight( pElement->uTemplateID );
		}

		CDungeonTemplate * pTemplate = GetTemplate( pElement->uTemplateID );

		if( pTemplate )
		{
			pstApdObjectTemplate	= g_pcsApmObject->GetObjectTemplate( pTemplate->uTemplateIndex[ uType ][ pElement->uIndex ].uIndex );
			if( pstApdObjectTemplate )
			{
				if( uType == TBM_DOME4	||
					uType == TBM_DOME	)
				{
					pstApdObject = g_pMainFrame->m_pTileList->m_pObjectWnd->AddObject( pstApdObjectTemplate , & pos , 
						ACUOBJECT_TYPE_DUNGEON_STRUCTURE | ACUOBJECT_TYPE_DUNGEON_DOME
						, fDeltaY );
				}
				else
				{
					pstApdObject = g_pMainFrame->m_pTileList->m_pObjectWnd->AddObject( pstApdObjectTemplate , & pos , ACUOBJECT_TYPE_DUNGEON_STRUCTURE , fDeltaY );
				}

				if( pstApdObject )
				{
					// 인덱스 저장..
					pElement->nObjectIndex = pstApdObject->m_lID;
				}
				else
				{
					MD_SetErrorMessage( "던젼 오브젝트 추가 실패" );
				}
			}
			else
			{
				TRACE( "템플릿 정보가 없어요 (%d)" , pTemplate->uTemplateIndex[ uType ][ pElement->uIndex ].uIndex );
			}
		}
	}

	return TRUE;
}

BOOL	CDungeonWnd::RemoveElement	( UINT32 uType , AuNode< DungeonElement > * pNode )
{
	ASSERT( NULL != pNode );
	if( NULL == pNode ) return FALSE;

	DungeonElement * pElement = & pNode->GetData();	

	if( pElement->nObjectIndex )
	{
		// 오브젝트가 있는경우.. 제거한다.
		g_pcsApmObject->DeleteObject( pElement->nObjectIndex , TRUE , TRUE );
		pElement->nObjectIndex = 0;
	}

	// 리스트에서 제거..
	m_listDungeonElement[ uType ].RemoveNode( pNode	);

	SaveSetChangeObjectList();
	return TRUE;
}

int		CDungeonWnd::SetGridWidth( int nWidth )
{
	int	nDelta = m_nGridWidth - nWidth;

	CRect	rect;
	GetClientRect( rect );
	
	int nOffsetX , nOffsetZ ;
	nOffsetX = GetXOffset( rect.CenterPoint().x );
	nOffsetZ = GetZOffset( rect.CenterPoint().y );

	m_nGridWidth = nWidth;

	// 옵셋 수정..
	m_nLeftX	= 0;
	m_nTopZ		= 0;
	
	int	nScreenX = GetScreenX( nOffsetX );
	int	nScreenY = GetScreenY( nOffsetZ );

	m_nLeftX	= rect.CenterPoint().x - nScreenX;
	m_nTopZ		= rect.CenterPoint().y - nScreenY;
	
	Invalidate( FALSE );
	return m_nGridWidth;
}

CDC *	CDungeonWnd::GetMemDC()
{
	// 더블 버퍼링 DC 관리..

	CRect	rect;
	GetClientRect( rect );

	if( rect.Width()	== m_memRect.Width()	&&
		rect.Height()	== m_memRect.Height()	)
	{
	}
	else
	{
		// 메모리 할당.
		m_memDC		.DeleteDC		();
		m_memBitmap	.DeleteObject	();

		CDC	* pDC = GetDC();
		m_memDC		.CreateCompatibleDC		( pDC									)	;
		m_memBitmap	.CreateCompatibleBitmap	( pDC , rect.Width() , rect.Height()	)	;

		m_memDC.SelectObject( m_memBitmap );
		ReleaseDC( pDC );
	}

	return & m_memDC;
}

void	CDungeonWnd::Apply( BOOL bAdjustHeight )
{
	ASSERT( NULL != g_pcsApmObject );
	ASSERT( NULL != g_pcsAgcmObject );
	// 던전 오브젝트 모두 제거.

	/*
	INT32				lIndex = 0;
	ApdObject	*		pcsApdObject;
	AgcdObject	*		pstAgcdObject;

	for (pcsApdObject = g_pcsApmObject->GetObjectSequence(&lIndex); pcsApdObject; pcsApdObject = g_pcsApmObject->GetObjectSequence(&lIndex))
	{
		VERIFY( pstAgcdObject = g_pcsAgcmObject->GetObjectData( pcsApdObject ) );

		if ( pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_DUNGEON_STRUCTURE )
		{
			// 이 오브젝트 삭제..
			//g_pcsApmObject->DestroyObject( pcsApdObject );
			g_pMainFrame->m_pTileList->m_pObjectWnd->DeleteObject( pstAgcdObject->m_stGroup.m_pstList->m_csData.m_pstClump );
		}
	}
	*/

	// 리스트 검사해서 해당 오브젝트 맵에 추가함.
	// 

	DungeonElement * pElement;
	AuNode< DungeonElement >	* pNode;
	RwV3d	pos;

	// for( UINT32 uType = TBM_FLOOR ; uType < TBM_SELECT ; ++uType )
	UINT32 uType = TBM_FLOOR;
	if( m_listDungeonElement[ uType ].GetCount() )
	{
		pNode = m_listDungeonElement[ uType ].GetHeadNode();
		while( pNode )
		{
			pElement = & pNode->GetData();

			pos.x	= __GetWorldXFromElement( uType , pElement );
			pos.y	= pElement->fHeight	;
			pos.z	= __GetWorldZFromElement( uType , pElement );

			// 높이 변경 부분..
			if( bAdjustHeight )
			{
				SaveSetChangeGeometry();

				INT32 nX , nZ;

				for( UINT32 cx = 0 ; cx < pElement->uWidth / 2 + 1 ; cx ++ )
				{
					for( UINT32 cz = 0 ; cz < pElement->uHeight / 2 + 1; cz ++ )
					{
						ApWorldSector * pSector = g_pcsApmMap->GetSector( pos.x + MAP_STEPSIZE * ( FLOAT ) cx, pos.z + MAP_STEPSIZE * ( FLOAT ) cz );

						if( pSector )
						{
							pSector->D_GetSegment( SECTOR_HIGHDETAIL , pos.x + MAP_STEPSIZE * ( FLOAT ) cx, pos.z + MAP_STEPSIZE * ( FLOAT ) cz , &nX , &nZ );
							// 높이값 변경..
							AGCMMAP_THIS->D_SetHeight( pSector , SECTOR_HIGHDETAIL ,
								nX , nZ , pElement->fHeight );
						}
					}
				}
			}
			pNode = pNode->GetNextNode();
		}

		// 바닥 다 지워버림.
		m_listDungeonElement[ uType ].RemoveAll();
		UpdateDungeonHeightBitmap();
	}

	// 노멸 갱신..
	{
		AuNode< AgcmMap::stLockSectorInfo > *	pNode		 = AGCMMAP_THIS->m_listLockedSectorList.GetHeadNode() ;
		AgcmMap::stLockSectorInfo			*	pLockInfo	;

		while( pNode )
		{
			pLockInfo = &pNode->GetData();

			ASSERT( NULL != pLockInfo );

			if( pLockInfo->pSector )
			{
				AGCMMAP_THIS->RecalcNormal( pLockInfo->pSector , SECTOR_HIGHDETAIL , FALSE );
			}
		
			pNode = pNode->GetNextNode();
		}
	}

	g_MainWindow.UnlockSectors( TRUE );
}

LRESULT CDungeonWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( WM_DUNGEON_UPDATE == message )
	{
		TRACE( "적요오옹~\n" );
		// Apply( wParam );
	}

	return CWnd::WindowProc(message, wParam, lParam);
}


FLOAT		CDungeonWnd::GetXPosFromOffset( UINT32 uOffsetX )
{
	// Division Start Time 부터 offset 이 되겠음..
	// 디비젼 스타트 포지션을 구함..
	FLOAT	fStartX ;

	fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( ALEF_LOAD_RANGE_X1 ) );
	
	return fStartX + uOffsetX * MAP_STEPSIZE / 2.0f;
}

FLOAT		CDungeonWnd::GetZPosFromOffset( UINT32 uOffsetZ )
{
	FLOAT	fStartZ ;

	fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexZ( ALEF_LOAD_RANGE_Y1 ) );

	return fStartZ + uOffsetZ * MAP_STEPSIZE / 2.0f;
}

BOOL		CDungeonWnd::LoadTemplate()
{
	AuIniManagerA	iniManager;
	iniManager.SetPath( "Map\\Data\\Object\\" DUNGEON_TEMPLATE_FILE );

	CDungeonTemplate			* pTemplate	;

	char	strKey		[ 256 ];
	
	m_listTemplate.RemoveAll();

	if( iniManager.ReadFile(0, FALSE) )
	{
		int		nSectionCount	;
		int		nKeyIndex		;

		nSectionCount	= iniManager.GetNumSection();	

		int		TID;
		int		i , j , nSection;

		for( nSection = 0 ; nSection < nSectionCount ; ++nSection  )
		{
			pTemplate = new CDungeonTemplate;

			TID = atoi( iniManager.GetSectionName( nSection ) );
			pTemplate->uTID	= TID;


			nKeyIndex	= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonTemplateName		);
			pTemplate->strName = iniManager.GetValue	( nSection , nKeyIndex		);

			for( i = 0 ; i < TBM_SELECT ; ++ i )
			{
				for( j = 0 ; j < MAX_DUNGEON_INDEX_COUNT ; ++ j )
				{
					wsprintf( strKey , g_strINIDungeonTemplateData , i , j );
					nKeyIndex	= iniManager.FindKey( nSection , ( char * ) strKey		);

					if( nKeyIndex == -1 )
					{
						// 마고자 (2005-03-25 오후 3:55:00) : 
						// 신버젼이다~
						wsprintf( strKey , g_strINIDungeonTemplateIndex , i , j );
						nKeyIndex	= iniManager.FindKey( nSection , ( char * ) strKey		);
						pTemplate->uTemplateIndex[ i ][ j ].uIndex	= atoi( iniManager.GetValue	( nSection , nKeyIndex		) );
						wsprintf( strKey , g_strINIDungeonTemplateSize , i , j );
						nKeyIndex	= iniManager.FindKey( nSection , ( char * ) strKey		);
						pTemplate->uTemplateIndex[ i ][ j ].uSize	= atoi( iniManager.GetValue	( nSection , nKeyIndex		) );
					}
					else
					{
						pTemplate->uTemplateIndex[ i ][ j ].uIndex = atoi( iniManager.GetValue	( nSection , nKeyIndex		) );

						// 마고자 (2005-03-25 오후 3:55:44) : 싸이즈는 디폴트 값으로 ..
						pTemplate->uTemplateIndex[ i ][ j ].uSize	= TB_SIZES[ i ] * 2;
					}
				}
			}
			
			for( i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
			{
				wsprintf( strKey , g_strINIDungeonTemplateSampleHeight , i );

				nKeyIndex	= iniManager.FindKey( nSection , ( char * ) strKey		);
				pTemplate->afSampleHeight[ i ] = ( FLOAT ) atof( iniManager.GetValue	( nSection , nKeyIndex		) );
			}
			
			m_listTemplate.AddTail( *pTemplate );

			delete pTemplate;
		}

		AuNode< CDungeonTemplate > * pNode;
		if( pNode = m_listTemplate.GetHeadNode () )
		{
			// 마고자 (2004-10-20 오후 9:55:58) : 
			// 템플릿 아이디를 설정함..

			pTemplate = & pNode->GetData();
			g_pMainFrame->m_pDungeonToolbar->m_uCurrentTemplateIndex = pTemplate->uTID;
		}
		
		return TRUE;
	}
	else
	{
		TRACE( "템플릿 파일이 없심!.\n" );
		return FALSE;
	}

	return FALSE;
}


BOOL		CDungeonWnd::SaveTemplate()
{
	AuIniManagerA	iniManager;
	iniManager.SetPath( "Map\\Data\\Object\\" DUNGEON_TEMPLATE_FILE );
	
	char	strSection	[ 256 ];
	char	strKey		[ 256 ];
	
	AuNode< CDungeonTemplate >	* pNode		= m_listTemplate.GetHeadNode();
	CDungeonTemplate			* pTemplate	;

	int i , j ;

	while( pNode )
	{
		pTemplate	= &pNode->GetData();

		wsprintf( strSection , g_strINIDungeonTemplateSection , pTemplate->uTID );
		
		iniManager.SetValue	( strSection , ( char * ) g_strINIDungeonTemplateName		, ( LPSTR )( LPCTSTR ) pTemplate->strName		);

		for( i = 0 ; i < TBM_SELECT ; i ++ )
		{
			for( j = 0 ; j < MAX_DUNGEON_INDEX_COUNT ; j ++ )
			{
				wsprintf( strKey , g_strINIDungeonTemplateIndex , i , j );
				iniManager.SetValueI	( strSection , ( char * ) strKey		, pTemplate->uTemplateIndex[ i ][ j ].uIndex	);
				wsprintf( strKey , g_strINIDungeonTemplateSize , i , j );
				iniManager.SetValueI	( strSection , ( char * ) strKey		, pTemplate->uTemplateIndex[ i ][ j ].uSize		);
			}
		}
		
		for( i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
		{
			wsprintf( strKey , g_strINIDungeonTemplateSampleHeight , i );
			iniManager.SetValueF	( strSection , ( char * ) strKey	, pTemplate->afSampleHeight[ i ]	);
		}

		pNode = pNode->GetNextNode();
	}

	iniManager.WriteFile(0, FALSE);

	return TRUE;
}

BOOL	CDungeonWnd::LoadDungeon()
{
	/*
	// 싹 날리고..
	RemoveAllElement();

	INT32 nDivisionIndex = GetLeftTopDivisionIndex();
	char	strFilename[ 256 ];

	wsprintf( strFilename , "Ini/Dungeon/" DUNGEON_DATA_FILE , nDivisionIndex );

	AuIniManager	iniManager;
	iniManager.SetPath( strFilename );

	DungeonElement element;

	if( iniManager.ReadFile(0, FALSE) )
	{
		if( IDYES == MessageBox( "지금 던젼 파일 ( 오브젝트 밑 ini 폴더에 있는것 ) 은 앞으로 사용하지 않고 OBDN 파일에 합쳐지게 됍니다."
					"이전 던젼데이타를 현재에 맞게 바꾸어서 읽어 들이겠습니까?. 한번 이 작업을 수행 한 후에는 "
					"오브젝트 파일이 자동으로 저장돼고 던젼 파일이 삭제돼게 됩니다."
					"혹시 모르니 백업을 하신 후 Yes를 눌러주세요" ,
					"던젼툴" ,
					MB_YESNOCANCEL ) )
		{
			int		nSectionCount	;
			int		nKeyIndex		;
			UINT32	uType			;
			int		nKeyCount		;

			nSectionCount	= iniManager.GetNumSection();	

			if( nSectionCount <= TBM_SELECT )
			{
				// 마고자 (2004-12-14 오전 11:10:58) : 
				// 새 버젼이다..
				UINT32	uType;
				for( int nSection = 0 ; nSection < nSectionCount ; nSection++ )
				{
					nKeyCount = iniManager.GetNumKeys( nSection );

					uType = atoi( iniManager.GetSectionName( nSection ) );
					ASSERT( uType >= 0 && uType < TBM_SELECT );

					for( int nKey = 0 ; nKey < nKeyCount ; nKey++ )
					{
						element.Clean();
						sscanf( iniManager.GetValue( nSection , nKey ) , g_strINIDungeonElement_DataFormat
												, &element.uTemplateID	
												, &element.uOffsetX	
												, &element.uOffsetZ	
												, &element.fHeight		
												, &element.uTurnAngle	
												, &element.uWidth		
												, &element.uHeight
												, &element.uIndex		);

						// protection
						if( element.uIndex >= 3 ) element.uIndex = 0;

						AddElement( uType , &element );
					}
				}
			}
			else
			{
				// 구버젼.,
				for( int nSection = 0 ; nSection < nSectionCount ; nSection++ )
				{
					element.Clean();

					nKeyIndex			= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonElementType			);
					uType				= atoi( iniManager.GetValue	( nSection , nKeyIndex		) );
					nKeyIndex			= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonElementTemplateIndex	);
					element.uTemplateID = atoi( iniManager.GetValue	( nSection , nKeyIndex		) );
					nKeyIndex			= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonElementOffsetX		);
					element.uOffsetX	= atoi( iniManager.GetValue	( nSection , nKeyIndex		) );
					nKeyIndex			= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonElementOffsetZ		);
					element.uOffsetZ	= atoi( iniManager.GetValue	( nSection , nKeyIndex		) );
					nKeyIndex			= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonElementHeight		);
					element.fHeight		= ( FLOAT ) atof( iniManager.GetValue	( nSection , nKeyIndex		) );
					nKeyIndex			= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonElementTurnAngle		);
					element.uTurnAngle	= atoi( iniManager.GetValue	( nSection , nKeyIndex		) );
					
					nKeyIndex			= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonElementCX			);
					element.uWidth		= atoi( iniManager.GetValue	( nSection , nKeyIndex		) );
					nKeyIndex			= iniManager.FindKey( nSection , ( char * ) g_strINIDungeonElementCY			);
					element.uHeight		= atoi( iniManager.GetValue	( nSection , nKeyIndex		) );

					AddElement( uType , &element );
				}
				////////////
			}

			// 오브젝트 정보 저장..
			{
				#ifndef _DEBUG
				try
				#endif
				{
					g_pcsAgcmRender->SaveAllOctrees( NULL );
				}
				#ifndef _DEBUG
				catch(...)
				{
					MessageBox( "옥트리 저장시 문제 발생~" , "맵툴 익스포트" , MB_ICONERROR | MB_OK );
				}
				#endif		

				DisplayMessage( "오브젝트 정보 저장......." );
				
				g_pMainFrame->m_pTileList->m_pObjectWnd->SaveScript( FALSE , TRUE );
			}

			// 던젼 데이타 파일 삭제.
			DeleteFile( strFilename );
		}

		return TRUE;
	}
	else
	{
		TRACE( "템플릿 파일이 없심!.\n" );
		return FALSE;
	}

	*/

	UpdateDungeonPreviewBitmap( FALSE );

	return TRUE;
}

BOOL	CDungeonWnd::SaveDungeon()
{
	/*
	INT32 nDivisionIndex = GetLeftTopDivisionIndex();
	char	strFilename[ 256 ];

	wsprintf( strFilename , "Ini/Dungeon/" DUNGEON_DATA_FILE , nDivisionIndex );

	AuIniManager	iniManager;
	iniManager.SetPath( strFilename );

	DungeonElement * pElement;
	AuNode< DungeonElement >	* pNode;

	FLOAT	fDeltaY = 0.0f;

	char	strSection	[ 256 ];
	char	strKey		[ 256 ];
	char	strData		[ 256 ];

	UINT32	uSeqNum = 0;
	for( UINT32 uType = TBM_FLOOR ; uType < TBM_SELECT ; ++uType )
	{
		uSeqNum	= 0;

		wsprintf( strSection , g_strINIDungeonElementSection , uType );

		pNode	= m_listDungeonElement[ uType ].GetHeadNode();
		while( pNode )
		{
			pElement = & pNode->GetData();

			wsprintf( strKey , g_strINIDungeonElement_Data , uSeqNum++ );
			sprintf( strData , g_strINIDungeonElement_DataFormat
									, pElement->uTemplateID	
									, pElement->uOffsetX	
									, pElement->uOffsetZ	
									, pElement->fHeight		
									, pElement->uTurnAngle	
									, pElement->uWidth		
									, pElement->uHeight
									, pElement->uIndex		);

			iniManager.SetValue( strSection , strKey , strData );
			pNode = pNode->GetNextNode();
		}
	}

	iniManager.WriteFile(0, FALSE);

	*/
	return TRUE;
}

BOOL	CDungeonWnd::DeleteSelected()
{
	ASSERT( m_uSelectedType < TBM_SELECT );

	if( m_pSelectedElement )
	{
		// m_listDungeonElement[ m_uSelectedType ].RemoveNode( m_pSelectedElement );
		RemoveElement( m_uSelectedType , m_pSelectedElement );

		m_pSelectedElement = NULL;
		Invalidate( FALSE );
		return TRUE;
	}

	ReleaseCapture();

	return FALSE;
}

void	CDungeonWnd::Idle()
{
	// 컨트롤 입력을 가지고 판단함..
	BOOL	bCurrentControlState;

	if( ISBUTTONDOWN( VK_CONTROL ) )
		bCurrentControlState = TRUE;
	else
		bCurrentControlState = FALSE;

	if( m_bControlState != bCurrentControlState )
	{
		m_bControlState = bCurrentControlState;
		Invalidate( FALSE );
	}
}

void	CDungeonWnd::RemoveAllElement()
{
	for( int i = 0 ; i < TBM_SELECT ; i ++ )
	{
		m_listDungeonElement[ i ].RemoveAll();
	}

	TRACE( "던젼 엘리먼트를 모두 제거합니다.\n" );
}

BOOL	CDungeonWnd::DungeonObjectStreamWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	CDungeonWnd	*pThis = (CDungeonWnd *) pClass;
	ApdObject	*pstApdObject	= (ApdObject *) pData;

	DungeonElement * pElement;

	// 던젼 엘리먼트 정보 얻기..
	pElement = pThis->GetElementByObjectIndex( pstApdObject->m_lID );

	if( pElement )
	{
		//  던젼 오브젝트이다..
		INT32	bDungeon = TRUE;
		if( !pcsStream->WriteValue( ( char * ) g_strINIDungeonObjectStreaming_Dungeon , bDungeon ) )
			return FALSE;

		// 정보 작성..
		char	strData		[ 256 ];
		sprintf( strData , g_strINIDungeonObjectStreaming_DataFormat
								, pElement->uTemplateID	
								, pElement->uType
								, pElement->uOffsetX	
								, pElement->uOffsetZ	
								, pElement->fHeight		
								, pElement->uTurnAngle	
								, pElement->uWidth		
								, pElement->uHeight
								, pElement->uIndex		);

		if( !pcsStream->WriteValue( ( char * ) g_strINIDungeonObjectStreaming_Info2 , strData ) )
			return FALSE;
	}
	else
	{
		//  던젼 오브젝트가 아니다..
		INT32	bDungeon = FALSE;
		if( !pcsStream->WriteValue( ( char * ) g_strINIDungeonObjectStreaming_Dungeon , bDungeon ) )
			return FALSE;
	}

	return TRUE;
}

BOOL	CDungeonWnd::DungeonObjectStreamReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	CDungeonWnd	*pThis = (CDungeonWnd *) pClass;
	ApdObject	*pstApdObject	= (ApdObject *) pData;

	const CHAR *szValueName;
	INT32	bDungeon = TRUE;
	BOOL	bDataAvailable	= FALSE;
	BOOL	bDataOld		= FALSE;
	char	strData		[ 256 ];

	while(pcsStream->ReadNextValue())
	{
		szValueName = pcsStream->GetValueName();

		if (!strcmp(szValueName, ( char * ) g_strINIDungeonObjectStreaming_Dungeon))
		{
			pcsStream->GetValue ((INT32 *) &bDungeon);
		}
		else if (!strcmp(szValueName, ( char * ) g_strINIDungeonObjectStreaming_Info))
		{
			bDataAvailable	= TRUE;
			bDataOld		= TRUE;
			pcsStream->GetValue( strData , 256 );
		}
		else if (!strcmp(szValueName, ( char * ) g_strINIDungeonObjectStreaming_Info2))
		{
			bDataAvailable = TRUE;
			pcsStream->GetValue( strData , 256 );
		}	
	}

	if( bDungeon && bDataAvailable )
	{
		DungeonElement element;

		sscanf( strData , g_strINIDungeonObjectStreaming_DataFormat
								, &element.uTemplateID	
								, &element.uType	
								, &element.uOffsetX	
								, &element.uOffsetZ	
								, &element.fHeight		
								, &element.uTurnAngle	
								, &element.uWidth		
								, &element.uHeight
								, &element.uIndex		);

		if( bDataOld )
		{
			element.uOffsetX	*= 2;
			element.uOffsetZ	*= 2;
			element.uWidth		*= 2;
			element.uHeight		*= 2;
		}

		// protection
		if( element.uIndex >= 3 ) element.uIndex = 0;

		ASSERT( element.uType < TBM_SELECT );

		// 오브젝트 인덱스 저장.
		element.nObjectIndex = pstApdObject->m_lID;

		// 리스트에 추가..
		if( element.uType < TBM_SELECT )
		{
			// 리스트 추가..

			// 검증작업..
			// 마고자 (2005-04-08 오후 2:51:17) : 
			// 1719 디비젼은 데이타가 꼬여있을 가능성이 높으므로..
			// 이 검증작업 자체를 건너 뛰도록함.
			if( GetDivisionIndexF( pstApdObject->m_stPosition.x , pstApdObject->m_stPosition.z ) == 1719 )
			{
				static BOOL _sFirst = true;
				if( _sFirst )
				{
					_sFirst = false;
					MD_SetErrorMessage( "이전 던젼 데이타를 무시하고 읽어 들입니다. 마고자와 상의\n" );
				}
			}
			else
			{
				if( AGCMMAP_THIS->IsLoadedDivision( &pstApdObject->m_stPosition ) )
				{
					RwV3d	pos;
					FLOAT	fDeltaY = 0.0f;

					pos.x	= pThis->__GetWorldXFromElement( element.uType , &element );
					pos.y	= element.fHeight	;
					pos.z	= pThis->__GetWorldZFromElement( element.uType , &element );
					fDeltaY	= pThis->__GetWorldDeltaYFromElement( element.uType , &element );

					// 오브젝트의 피봇이 왼쪽 위 포이트에 있어서 뺑뺑 돌려줘야함..

					if( element.uType == TBM_DOME4	||
						element.uType == TBM_DOME	)
					{
						pos.y += g_pMainFrame->m_pDungeonToolbar->UpdateDungeonWallHeight( element.uTemplateID );
					}

					if( pos.x != pstApdObject->m_stPosition.x	||
						pos.z != pstApdObject->m_stPosition.z	||
						fDeltaY != pstApdObject->m_fDegreeY		)
					{
						// 에잉!
						//pThis->UpdateElement( element.uType , &element );
						pstApdObject->m_stPosition.x	= pos.x;
						pstApdObject->m_stPosition.z	= pos.z;
						pstApdObject->m_fDegreeY		= fDeltaY;

						MD_SetErrorMessage( "던젼 오브젝트 위치 조절\n" );
					}

					pThis->m_listDungeonElement[ element.uType ].AddTail( element );
				}
			}
		}
	}
	else
	{
		/*
		AgcdObject	*pstAgcdObject = g_pcsAgcmObject->GetObjectData(pstApdObject);
		if( AcuObject::GetProperty( pstAgcdObject->m_lObjectType ) & ACUOBJECT_TYPE_DUNGEON_STRUCTURE )
		{
			// 던젼오브젝트이면서 던젼에 대한 정보가 없는경우..
			// 제거해버린다.
			return FALSE;
		}
		*/
	}

	return TRUE;
}

void	CDungeonWnd::UpdateDungeonHeightBitmap	()
{
	// 여기서 높이값 기준으로 칼라 설정.
	m_bmpHeight.Create( 512 , 512 );

	CDC	 * pDC = CDC::FromHandle( m_bmpHeight.GetDC() );

	ASSERT( NULL != pDC );

	if( pDC )
	{
		INT32 nDivisionIndex = GetLeftTopDivisionIndex();

		INT32	nSectorStartX = GetFirstSectorXInDivision( nDivisionIndex );
		INT32	nSectorStartZ = GetFirstSectorZInDivision( nDivisionIndex );

		INT32	nSectorX , nSectorZ;

		ApWorldSector * pSector;

		INT32	nTileWidth = 2;

		CPoint	pointStart;
		CDungeonTemplate * pTemplate = GetCurrentTemplate();

		for( nSectorZ = nSectorStartZ ; nSectorZ < nSectorStartZ + MAP_DEFAULT_DEPTH ; nSectorZ++ )
		{
			for( nSectorX = nSectorStartX ; nSectorX < nSectorStartX + MAP_DEFAULT_DEPTH ; nSectorX++ )
			{
				pSector = g_pcsApmMap->GetSectorByArray( nSectorX , 0 , nSectorZ );

				pointStart.x	= ( nSectorX - nSectorStartX ) * MAP_DEFAULT_DEPTH * nTileWidth;
				pointStart.y	= ( nSectorZ - nSectorStartZ ) * MAP_DEFAULT_DEPTH * nTileWidth;

				if( pSector )
				{
					INT32	nSegmentX , nSegmentZ;
					ApDetailSegment	* pSegment;
					COLORREF	color;

					for( nSegmentZ = 0 ; nSegmentZ < MAP_DEFAULT_DEPTH ; nSegmentZ++ )
					{
						for( nSegmentX = 0 ; nSegmentX < MAP_DEFAULT_DEPTH ; nSegmentX++ )
						{
							pSegment = pSector->D_GetSegment( nSegmentX , nSegmentZ );

							if( pSegment )
							{
								// 레이어 구하기
								int nLayer = 0;
								color = RGB( 0 , 0 , 0 );

								for( int i = 0 ; i < MAX_DUNGEON_HEIGHT_COUNT ; i ++ )
								{
									if( pSegment->height == pTemplate->afSampleHeight[ i ] )
									{
										nLayer = i;
										color = __ColorTable[ nLayer ];
										break;
									}
								}
								// 높이 찾기.
								
							}
							else
							{
								color = RGB( 0 , 0 , 0 );
							}

							// 요기서 비트맵에 그리기.

							pDC->FillSolidRect( pointStart.x + nSegmentX * nTileWidth , 
												pointStart.y + nSegmentZ * nTileWidth ,
												nTileWidth,
												nTileWidth,
												color);
						}
					}

				}
				else
				{

				}
			}
		}
	}
}


void	CDungeonWnd::UpdateDungeonPreviewBitmap( BOOL bReload )
{
	if( bReload )
	{
		// 미니맵 다시 촬영.
		g_pMainFrame->OnFileMinimapexportSmallSize();
		UpdateDungeonHeightBitmap();
	}

	{
		INT32 nDivisionIndex = GetLeftTopDivisionIndex();
		char	strFilename[ 256 ];

		wsprintf( strFilename , "%s\\Map\\Minimap\\MP%04d.bmp" , ALEF_CURRENT_DIRECTORY , nDivisionIndex );

		m_bmpDivision.Load( strFilename , NULL );
	}

}

void CDungeonWnd::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// 해당 위치로 이동..
	int nOffsetX , nOffsetZ;
	int	nDeltaX , nDeltaZ;

	nOffsetX = GetXOffset( point.x , &nDeltaX );
	nOffsetZ = GetZOffset( point.y , &nDeltaZ );

	FLOAT	fX , fZ;
	fX = GetXPosFromOffset( nOffsetX );
	fZ = GetZPosFromOffset( nOffsetZ );

	// 위치로 카메라 이동..

	g_MainWindow.MoveTo( fX , fZ );

	CWnd::OnRButtonDblClk(nFlags, point);
}

INT32	CDungeonWnd::GetXOffsetFromPos( FLOAT fPosX )
{
	// Division Start Time 부터 offset 이 되겠음..
	// 디비젼 스타트 포지션을 구함..
	FLOAT	fStartX ;

	fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( ALEF_LOAD_RANGE_X1 ) );

	return ( INT32 ) ( ( fPosX - fStartX ) / ( MAP_STEPSIZE / 2.0f ) );
}

INT32	CDungeonWnd::GetZOffsetFromPos( FLOAT fPosZ )
{
	// Division Start Time 부터 offset 이 되겠음..
	// 디비젼 스타트 포지션을 구함..
	FLOAT	fStartZ ;

	fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexZ( ALEF_LOAD_RANGE_Y1 ) );

	return ( INT32 ) ( ( fPosZ - fStartZ ) / ( MAP_STEPSIZE / 2.0f ) );
}

void	CDungeonWnd::MoveToPosition( RwV3d * pPos )
{
	CRect	rect;
	GetClientRect( rect );

	m_nLeftX	= - GetXOffsetFromPos( pPos->x ) * m_nGridWidth + rect.Width() / 2 ;
	m_nTopZ		= - GetZOffsetFromPos( pPos->z ) * m_nGridWidth + rect.Height() / 2 ;

	Invalidate( FALSE );
}