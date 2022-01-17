// Plugin_Water.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "Plugin_Water.h"
#include "myengine.h"
#include "mainwindow.h"
#include "MainFrm.h"
#include "WaterHeightDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	ZANSANG_REMAIN_TIME	200

extern MainWindow			g_MainWindow	;
/////////////////////////////////////////////////////////////////////////////
// CPlugin_Water

CPlugin_Water::CPlugin_Water()
{
	m_bValidPosition	= FALSE	;
	m_fWaterHeight		= 0.0f	;

	m_rectAddWater.		SetRect( 0		, 60 , 150 , 100 );
	m_rectDeleteWater.	SetRect( 150	, 60 , 300 , 100 );
	m_rectHeightWater.	SetRect( 0		, 0 , 300 , 60 );
	m_rectEditHWaterStatus.	SetRect( 160	, 262 , 290 , 278 );

	m_rectHQWaterOnOff.	SetRect( 0		, 280 , 150 , 320 );
	m_rectHQWaterWaveHeightEdit.SetRect( 150	, 280 , 300 , 320 );

	m_bShowSampleWater	= TRUE	;

	m_uLastSetHeightTime	=	GetTickCount() - ZANSANG_REMAIN_TIME;
	m_uWaterZanSangChargeTime	= 0;

	m_bHQWaterMode = FALSE;

	m_iSelID = 0;

	m_strShortName = "Water";
}

CPlugin_Water::~CPlugin_Water()
{
}


BEGIN_MESSAGE_MAP(CPlugin_Water, CWnd)
	//{{AFX_MSG_MAP(CPlugin_Water)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlugin_Water message handlers

void CPlugin_Water::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );
	rect.bottom = 60;
	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );

	CString	str;

	if( m_bValidPosition )
	{
		str.Format( "%.0f,%.0f,%.0f" , m_pos.x , m_pos.y , m_pos.z );
	}
	else
	{
		str.Format( "맵위가 아님!" );
	}

	dc.TextOut( 0 , 0 , "여기 클릭하면 높이수정~" );
	dc.TextOut( 0 , 20 , str );

	dc.SetTextColor( RGB( 128 , 128 , 255 ) );
	str.Format( "워터 기준 높이. %.1fM" , m_fWaterHeight / 100.0f );
	dc.TextOut( 0 , 40 , str );

	dc.SetTextColor( 0 );
	dc.FillSolidRect( m_rectAddWater , RGB( 64 , 128 , 64 ) );
	dc.DrawText( "물추가" , m_rectAddWater , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	dc.FillSolidRect( m_rectDeleteWater , RGB( 128 , 128 , 128 ) );
	dc.DrawText( "물삭제" , m_rectDeleteWater , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
	dc.TextOut( 0 , 100	, "마우스 오른쪽 두번클릭");
	dc.TextOut( 0 , 120	, "      --->물 높이지정, Status ID 가져오기");
	dc.TextOut( 0 , 140	, "마우스 왼쪽 버튼 드래그");
	dc.TextOut( 0 , 160	, "      --->물 범위 지정");
	dc.TextOut( 0 , 180	, "CTRL 또는 ALT 누르고 있음");
	dc.TextOut( 0 , 200	, "      --->물 높이 확인");

	CRect	rect_line;
	rect_line.SetRect(0,220,300,221);
	dc.FillSolidRect( rect_line , RGB( 255 , 255 , 255 ) );

	rect_line.SetRect(0,255,300,256);
	dc.FillSolidRect( rect_line , RGB( 255 , 255 , 255 ) );
	dc.TextOut( 0 , 260	, "High-Quality Water Edit");

	dc.SetTextColor( 0 );
	if(m_bHQWaterMode)
	{
		dc.FillSolidRect( m_rectHQWaterOnOff , RGB( 192 , 192 , 192 ) );
		dc.DrawText( "HQ-Water On" , m_rectHQWaterOnOff , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	}
	else
	{
		dc.FillSolidRect( m_rectHQWaterOnOff , RGB( 64 , 64 , 64 ) );
		dc.DrawText( "HQ-Water Off" , m_rectHQWaterOnOff , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	}

	//@{ Jaewon 20050706
	// m_bHQWaterWaveHeightMode -> g_pcsAgcmWater->m_bForce11Shader
	if(g_pcsAgcmWater->m_bForce11Shader)
	{
		dc.FillSolidRect( m_rectHQWaterWaveHeightEdit , RGB( 192 , 192 , 192 ) );
		dc.DrawText( "ps1.1 shader forced" , m_rectHQWaterWaveHeightEdit , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	}
	else
	{
		dc.FillSolidRect( m_rectHQWaterWaveHeightEdit , RGB( 64 , 64 , 64 ) );
		dc.DrawText( "optimal shader used" , m_rectHQWaterWaveHeightEdit , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	}
	//@} Jaewon

	dc.FillSolidRect( m_rectEditHWaterStatus , RGB( 128 , 128 , 128 ) );
	dc.DrawText( "HQ Status Edit" , m_rectEditHWaterStatus , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	
	// Do not call CWnd::OnPaint() for painting messages
}


BOOL CPlugin_Water::OnMouseMoveGeometry	( RwV3d * pPos )
{
	PositionValidation( pPos );

	if( NULL == pPos )
	{
		m_bValidPosition	= FALSE;
		Invalidate( FALSE );
	}
	else
	{
		m_pos = * pPos;
		m_bValidPosition	= TRUE;
		Invalidate( FALSE );

		int				posx, posz		;
		
		// 좌표를 찾는다.
		ApDetailSegment	*pSegment;

		if( m_bGeometrySelection	&&
			g_MainWindow.m_pCurrentGeometry	&&
			( pSegment = g_MainWindow.m_pCurrentGeometry->D_GetSegment( pPos->x , pPos->z , &posx , &posz ) ) )
		{
			m_SelectedPosX2			= posx	;
			m_SelectedPosY2			= posz	;
			m_pSelectedGeometry2	= g_MainWindow.m_pCurrentGeometry;
		}

	}
	return TRUE;
}

BOOL CPlugin_Water::OnWindowRender			()
{
	if( FALSE == m_bShowSampleWater )
		return TRUE;

	ApWorldSector *	pSector			;

	// 버퍼 준비..
	FLOAT	fX1 , fZ1 , fX2 , fZ2;

	// Alt 누르고 있으면 기준 높이 표시함..
	static UINT	uLastTime = GetTickCount();
	UINT	uDelta = GetTickCount() - uLastTime;
	uLastTime = GetTickCount();
	
	if( ISBUTTONDOWN( VK_MENU )		||
		ISBUTTONDOWN( VK_CONTROL )	||
		GetTickCount() - m_uLastSetHeightTime < ZANSANG_REMAIN_TIME )	//잔상시간 안쪽이면 그림..
	{
		m_uWaterZanSangChargeTime += uDelta;
		if( m_uWaterZanSangChargeTime > ZANSANG_REMAIN_TIME )
			m_uWaterZanSangChargeTime = ZANSANG_REMAIN_TIME;
	}
	else
	{
		m_uWaterZanSangChargeTime -= uDelta;
		if( m_uWaterZanSangChargeTime < 0 )
			m_uWaterZanSangChargeTime = 0;
	}

	if( m_uWaterZanSangChargeTime > 0 )
	{
		pSector = g_pcsApmMap->GetSectorByArray( ALEF_LOAD_RANGE_X1 , 0 , ALEF_LOAD_RANGE_Y1 );
		if( NULL == pSector ) return FALSE;

		fX1	= pSector->GetXStart();
		fZ1	= pSector->GetZStart();

		pSector = g_pcsApmMap->GetSectorByArray( ALEF_LOAD_RANGE_X2 , 0 , ALEF_LOAD_RANGE_Y2 );
		if( NULL == pSector ) return FALSE;

		fX2	= pSector->GetXEnd();
		fZ2	= pSector->GetZEnd();

		RenderRectancle( fX1 , fZ1 , fX2 , fZ2 , 0.0f , 64 , 64 , 255 , 
			( INT32 ) ( 172.0f * ( ( FLOAT ) m_uWaterZanSangChargeTime / ( FLOAT ) ZANSANG_REMAIN_TIME ) ) );
	}

	if( m_SelectedPosX1 >= 0 && m_SelectedPosY1 >= 0 && m_pSelectedGeometry1 &&
		m_SelectedPosX2 >= 0 && m_SelectedPosY2 >= 0 && m_pSelectedGeometry2 )
	{
		static bool bToggle = true;

		if( ( uLastTime % 300 ) > 150 )
			bToggle = true;
		else
			bToggle = false;

		m_pSelectedGeometry1->D_GetSegment(
			m_SelectedPosX1 , m_SelectedPosY1 ,
			&fX1 , &fZ1 );
		
		m_pSelectedGeometry2->D_GetSegment(
			m_SelectedPosX2 , m_SelectedPosY2 ,
			&fX2 , &fZ2 );

		if( bToggle )
			RenderRectancle( fX1 , fZ1 , fX2 , fZ2 , 5.0f , 255 , 30 , 30 , 100 , TRUE	);
		else
			RenderRectancle( fX1 , fZ1 , fX2 , fZ2 , 5.0f , 255 , 70 , 70 , 172 , TRUE );
	}

	return TRUE;
}

//BOOL CPlugin_Water::OnPostRender		( RwRaster * pRaster )
//{
////	POINT	point;
////	if( m_bValidPosition && GetWorldPosToScreenPos( &m_pos , &point ) )
////	{
////		char	str[ 256 ];
////		sprintf( str , "%.1fM" , m_pos.y / 100.0f );
////		HanFontDrawText( pRaster , point.x , point.y - 20, str );
////	}
//	return TRUE;
//}

BOOL CPlugin_Water::OnRButtonDblClkGeometry	( RwV3d * pPos )
{
	PositionValidation( pPos );

	if( NULL == pPos )
	{

	}
	else
	{
		// Alt키를 누르고 있을때만..
		FLOAT	fHeight;
		if( g_pcsAgcmWater->GetWaterHeight( pPos->x , pPos->z , & fHeight ) )
		{
			m_fWaterHeight = fHeight;
			DisplayMessage( RGB( 0 , 255 , 0 ) , "워터 플러그인 : 기존 워터값을 읽어들였습니다." );
		}
		else
		{
			m_fWaterHeight = pPos->y;
			DisplayMessage( RGB( 255 , 0 , 0 ) , "워터 플러그인 : 높이값을 읽었습니다." );
		}

		m_uLastSetHeightTime	= GetTickCount(); //잔상 남겨놓기 용..
		Invalidate( FALSE );

		// status 가져오기
		int		stid = g_pcsAgcmWater->GetWaterStatus( pPos->x , pPos->z );
		m_iSelID = stid;
		m_ctlListBox.SetCurSel(m_iSelID);
	}
	return TRUE;
}

BOOL CPlugin_Water::OnLButtonDownGeometry	( RwV3d * pPos )
{
	PositionValidation( pPos );

	int				posx, posz		;
	
	// 좌표를 찾는다.
	ApDetailSegment	*pSegment;

	if( pSegment = g_MainWindow.m_pCurrentGeometry->D_GetSegment( pPos->x , pPos->z , &posx , &posz ) )
	{
		m_bGeometrySelection	= TRUE	;

		m_SelectedPosX1		= posx	;
		m_SelectedPosY1		= posz	;
		m_SelectedPosX2		= posx	;
		m_SelectedPosY2		= posz	;

		m_pSelectedGeometry1	= g_MainWindow.m_pCurrentGeometry	;
		m_pSelectedGeometry2	= g_MainWindow.m_pCurrentGeometry	;
	}
	else
	{
		m_bGeometrySelection	= FALSE	;

		m_SelectedPosX1		= -1	;
		m_SelectedPosY1		= -1	;
		m_SelectedPosX2		= -1	;
		m_SelectedPosY2		= -1	;

		m_pSelectedGeometry1	= NULL	;
		m_pSelectedGeometry2	= NULL	;
	}
	
	return TRUE;
}

BOOL CPlugin_Water::OnLButtonUpGeometry		( RwV3d * pPos )
{
	PositionValidation( pPos );

	int				posx, posz		;
	
	// 좌표를 찾는다.
	ApDetailSegment	*pSegment;

	if( m_bGeometrySelection			&&
		g_MainWindow.m_pCurrentGeometry	&&
		( pSegment = g_MainWindow.m_pCurrentGeometry->D_GetSegment( pPos->x , pPos->z , &posx , &posz ) ) )
	{
		m_bGeometrySelection	= FALSE	;

		m_SelectedPosX2		= posx	;
		m_SelectedPosY2		= posz	;

		m_pSelectedGeometry2	= g_MainWindow.m_pCurrentGeometry	;
	}
	else
	{
		m_bGeometrySelection	= FALSE	;

		m_SelectedPosX1		= -1	;
		m_SelectedPosY1		= -1	;
		m_SelectedPosX2		= -1	;
		m_SelectedPosY2		= -1	;

		m_pSelectedGeometry1	= NULL	;
		m_pSelectedGeometry2	= NULL	;
	}
	
	return TRUE;
}

BOOL CPlugin_Water::OnSelectedPlugin		()
{
	m_SelectedPosX1		= -1	;
	m_SelectedPosY1		= -1	;
	m_SelectedPosX2		= -1	;
	m_SelectedPosY2		= -1	;

	m_pSelectedGeometry1	= NULL	;
	m_pSelectedGeometry2	= NULL	;
	return TRUE;
}

BOOL CPlugin_Water::OnDeSelectedPlugin		()
{
	m_SelectedPosX1		= -1	;
	m_SelectedPosY1		= -1	;
	m_SelectedPosX2		= -1	;
	m_SelectedPosY2		= -1	;

	m_pSelectedGeometry1	= NULL	;
	m_pSelectedGeometry2	= NULL	;
	return TRUE;
}

void CPlugin_Water::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_rectHeightWater.PtInRect( point ) )
	{
		// 마고자 (2003-11-13 오후 5:36:28) : 
		// 높이수정.
		CWaterHeightDlg dlg;
		dlg.m_fHeight	= m_fWaterHeight;
		if( IDOK == dlg.DoModal() )
		{
			m_fWaterHeight = dlg.m_fHeight;
			Invalidate( FALSE );
		}
	}

	if( 
		NULL == m_pSelectedGeometry1 ||
		NULL == m_pSelectedGeometry2
		)
	{
		TRACE( "범위가 지정돼지 않앗습니다." );

		// do nothing..
	}
	else
	{
		if( m_rectAddWater.PtInRect( point ) )
		{
			// 물추가
			TRACE( "물추가!\n" );

			ApDetailSegment	*pSegment;

			FLOAT	x1, x2 , z1 , z2;

			FLOAT	xt , zt;
			FLOAT	width , height;

			pSegment = m_pSelectedGeometry1->D_GetSegment(
				m_SelectedPosX1 , m_SelectedPosY1 ,
				&x1 , &z1 );
			
			pSegment = m_pSelectedGeometry2->D_GetSegment(
				m_SelectedPosX2 , m_SelectedPosY2 ,
				&x2 , &z2 );

			// Left Top 을 구함.
			if( x1 < x2 )
			{
				xt		= x1		;
				width	= x2 - x1	;
			}
			else
			{
				xt		= x2		;
				width	= x1 - x2	;
			}

			if( z1 < z2 )
			{
				zt		= z1		;
				height	= z2 - z1	;
			}
			else
			{
				zt		= z2		;
				height	= z1 - z2	;
			}

			// xt,yt, 부터 width , height 범위만큼 m_fWaterHeight 높이의 워터 추가.
			if(m_iSelID < 0 ) m_iSelID = 0;

			if( g_pcsAgcmWater->AddWater	(
					xt , zt , width , height ,
					m_fWaterHeight ,
					g_pcsAgcmWater->m_stWaterStatus[m_iSelID].WaterType , m_iSelID )
			)
			{
				// Success
				DisplayMessage( RGB( 64 , 64 , 64 ) , "워터 플러그인 : 물이 추가돼었습니다." );
				OnDeSelectedPlugin();

				SetSaveData();
			}
			else
			{
				DisplayMessage( RGB( 255 , 64 , 64 ) , "워터 플러그인 : 물추가에 실패했습니다 -_-;;;;" );
			}

		}
		else
		if( m_rectDeleteWater.PtInRect( point ) )
		{
			TRACE( "물삭제!\n" );

			ApDetailSegment	*pSegment;

			FLOAT	x1, x2 , z1 , z2;
			FLOAT	xt , zt;
			FLOAT	width , height;

			pSegment = m_pSelectedGeometry1->D_GetSegment(
				m_SelectedPosX1 , m_SelectedPosY1 ,
				&x1 , &z1 );
			
			pSegment = m_pSelectedGeometry2->D_GetSegment(
				m_SelectedPosX2 , m_SelectedPosY2 ,
				&x2 , &z2 );

			// Left Top 을 구함.
			if( x1 < x2 )
			{
				xt		= x1		;
				width	= x2 - x1	;
			}
			else
			{
				xt		= x2		;
				width	= x1 - x2	;
			}

			if( z1 < z2 )
			{
				zt		= z1		;
				height	= z2 - z1	;
			}
			else
			{
				zt		= z2		;
				height	= z1 - z2	;
			}

			// xt,zt, 부터 width , height 범위안에 존재하는 워터 삭제.
			if( g_pcsAgcmWater->RemoveWaterRange( xt, zt, width , height ) )
			{
				// Success
				DisplayMessage( RGB( 64 , 64 , 64 ) , "워터 플러그인 : 물이 삭제 돼었습니다." );
				OnDeSelectedPlugin();

				SetSaveData();
			}
			else
			{
				DisplayMessage( RGB( 255 , 64 , 64 ) , "워터 플러그인 : 물삭제에 실패했습니다 -_-;;;;" );
			}			
		}
	}

	if( m_rectHQWaterOnOff.PtInRect( point ))
	{
		if(g_pcsAgcmWater->m_bEnableHWater)
		{
			SetSaveData();

			m_bHQWaterMode = !m_bHQWaterMode;

			if(m_bHQWaterMode)
			{
				g_pcsAgcmWater->MakeHWaterFromWater();
			}

			if(m_bHQWaterMode)
				g_pcsAgcmWater->m_iWaterDetail = WATER_DETAIL_HIGH;
			else
				g_pcsAgcmWater->m_iWaterDetail = WATER_DETAIL_MEDIUM;
			
			g_pcsAgcmWater->ChangeHWaterMode(m_bHQWaterMode);
		}
		else
		{
			DisplayMessage( RGB( 64 , 64 , 64 ) , "High-Quality Water를 지원하지 않습니다.." );
		}
	}
	else if( m_rectHQWaterWaveHeightEdit.PtInRect ( point ))
	{
		if(g_pcsAgcmWater->m_bEnableHWater)
		{
			//@{ Jaewon 20050706
			// m_bHQWaterWaveHeightMode -> g_pcsAgcmWater->m_bForce11Shader
			if(m_bHQWaterMode)
			{
				g_pcsAgcmWater->m_bForce11Shader = !g_pcsAgcmWater->m_bForce11Shader;
			}
			//@} Jaewon
		}
		else 
		{
			DisplayMessage( RGB( 64 , 64 , 64 ) , "High-Quality Water를 지원하지 않습니다.." );
		}
	}
	else if( m_rectEditHWaterStatus.PtInRect( point ))
	{
		if(g_pcsAgcmWater->m_bEnableHWater)
		{
			if(m_bHQWaterMode)
			{
				int		res = g_pcsAgcmWaterDlg2->OpenWaterDlg2(m_iSelID);
				if(res)
				{
					g_pcsAgcmWater->ChangeHWaterStatus(m_iSelID);
				}
			}
		}
	}

	GetParent()->InvalidateRect(NULL);
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CPlugin_Water::RenderRectancle		(	FLOAT fX1 , FLOAT fZ1 , FLOAT fX2 , FLOAT fZ2 , FLOAT fDHeight ,
								INT32 nRed , INT32 nGreen , INT32 nBlue , INT32 nAlpha , BOOL bBox  )
{
	RwIm3DVertex *		pLineList	= new RwIm3DVertex		[ 4	];
	RwImVertexIndex	*	pIndex		= new RwImVertexIndex	[ 6	];

	RwIm3DVertexSetPos	( &pLineList[ 0 ] , fX1 , m_fWaterHeight + fDHeight , fZ1	);
	RwIm3DVertexSetU	( &pLineList[ 0 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 0 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , nRed , nGreen , nBlue , nAlpha  );

	RwIm3DVertexSetPos	( &pLineList[ 1 ] , fX2 , m_fWaterHeight + fDHeight , fZ1	);
	RwIm3DVertexSetU	( &pLineList[ 1 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 1 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 1 ] , nRed , nGreen , nBlue , nAlpha  );
	
	RwIm3DVertexSetPos	( &pLineList[ 2 ] , fX1 , m_fWaterHeight + fDHeight , fZ2	);
	RwIm3DVertexSetU	( &pLineList[ 2 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 2 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 2 ] , nRed , nGreen , nBlue , nAlpha  );
	
	RwIm3DVertexSetPos	( &pLineList[ 3 ] , fX2 , m_fWaterHeight + fDHeight , fZ2	);
	RwIm3DVertexSetU	( &pLineList[ 3 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 3 ] , 1.0f				);
	RwIm3DVertexSetRGBA	( &pLineList[ 3 ] , nRed , nGreen , nBlue , nAlpha  );
	
	pIndex[ 0 ]	= 0;
	pIndex[ 1 ]	= 1;
	pIndex[ 2 ]	= 2;
	pIndex[ 3 ]	= 1;
	pIndex[ 4 ]	= 2;
	pIndex[ 5 ]	= 3;

	if( RwIm3DTransform( pLineList , 4 , NULL, rwIM3D_VERTEXRGBA ) )
	{                         
		RwIm3DRenderIndexedPrimitive( rwPRIMTYPETRILIST,
			pIndex , 6 );

		RwIm3DEnd();
	}

	if( bBox )
	{
		FLOAT	fHeight , fHeightTemp;
		fHeight		= AGCMMAP_THIS->GetHeight( fX1 , fZ1 , SECTOR_MAX_HEIGHT );
		if( ( fHeightTemp = AGCMMAP_THIS->GetHeight( fX2 , fZ1 , SECTOR_MAX_HEIGHT ) ) > fHeight ) fHeight = fHeightTemp;
		if( ( fHeightTemp = AGCMMAP_THIS->GetHeight( fX1 , fZ2 , SECTOR_MAX_HEIGHT ) ) > fHeight ) fHeight = fHeightTemp;
		if( ( fHeightTemp = AGCMMAP_THIS->GetHeight( fX2 , fZ2 , SECTOR_MAX_HEIGHT ) ) > fHeight ) fHeight = fHeightTemp;

		RwIm3DVertexSetPos	( &pLineList[ 0 ] , fX1 , fHeight , fZ1	);
		RwIm3DVertexSetPos	( &pLineList[ 1 ] , fX2 , fHeight , fZ1	);	
		RwIm3DVertexSetPos	( &pLineList[ 2 ] , fX1 , fHeight , fZ2	);
		RwIm3DVertexSetPos	( &pLineList[ 3 ] , fX2 , fHeight , fZ2	);

		pIndex[ 0 ]	= 0;
		pIndex[ 1 ]	= 1;
		pIndex[ 2 ]	= 3;
		pIndex[ 3 ]	= 2;
		pIndex[ 4 ]	= 0;

		if( RwIm3DTransform( pLineList , 4 , NULL, rwIM3D_VERTEXRGBA ) )
		{                         
			RwIm3DRenderIndexedPrimitive( rwPRIMTYPEPOLYLINE,
				pIndex , 5 );

			RwIm3DEnd();
		}
	}

	delete [] pIndex	;
	delete [] pLineList	;
}

BOOL CPlugin_Water::OnQuerySaveData		( char * pStr )
{
	strcpy( pStr , "워터정보저장" );

	return TRUE;
}

BOOL CPlugin_Water::OnSaveData			()
{
	DisplayMessage( "Water 저장.. 저장결과를 확인하세요. 세이브 안될수 있습니다." );
	// g_pcsAgcmWater->SaveToFiles( NULL );

	// 워터 저장
	char strPath[ 1024 ];
	wsprintf( strPath , "%s\\map\\data" , ALEF_CURRENT_DIRECTORY );
	g_pcsAgcmWater->SaveToFiles( strPath );

	// 백업본 저장
	char	strSub[ 1024 ];
	GetSubDataDirectory( strSub );

	_CreateDirectory( strSub );
	_CreateDirectory( "%s\\map"				, strSub );
	_CreateDirectory( "%s\\map\\data"		, strSub );
	_CreateDirectory( "%s\\map\\data\\water", strSub );

	wsprintf( strPath , "%s\\map\\data" , strSub );
	g_pcsAgcmWater->SaveToFiles( strPath );

	// Water Status Info INI auto-save
	g_pcsAgcmWater->SaveStatusInfoT1ToINI("./ini/waterstatust1.ini" , FALSE );
	g_pcsAgcmWater->SaveHWaterStatusToINI("./ini/hwaterstatus.ini" , FALSE );

	return TRUE;
}

BOOL CPlugin_Water::OnLoadData			()
{
	// 섹터 읽을때 자동으로 한다.. 따로 신경 쓸 필요 없음..
	return TRUE;
}

int CPlugin_Water::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	RECT				rect;
	rect.left = 10;		rect.right = 120;
	rect.top  = 230 ;	rect.bottom = 250;

	RECT				rect2;
	rect2.left = 150;	rect2.right = 290;
	rect2.top  = 230 ;	rect2.bottom = 250;

	m_ctlListBox.Create( WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER | LBS_NOTIFY , rect, this , IDC_PULGIN_WATER_LIST);
		
	for(int i=0;i<WMAP_STATUS_NUM;++i)
	{
		char	str[10];
		sprintf(str,"%d",i);
		m_ctlListBox.AddString(str);
	}

	m_iSelID = 0;
	
	m_ctlButton.Create( "Water Status Edit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rect2 , this,IDC_PULGIN_WATER_BUTTON);
		
	return 0;
}


LRESULT CPlugin_Water::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case IDC_PULGIN_WATER_BUTTON:
			switch(HIWORD(wParam))
			{
				case BN_CLICKED:
				int		res = g_pcsAgcmWaterDlg->OpenWaterDlg(m_iSelID);
				if(res)
				{
					g_pcsAgcmWater->ChangeStatus(m_iSelID);
					SetSaveData();
				}
				break;
			}
			break;

			case IDC_PULGIN_WATER_LIST:
			switch(HIWORD(wParam))
			{
				case	LBN_SELCHANGE:
				m_iSelID = m_ctlListBox.GetCurSel();
				break;
			}

			//case IDC_PULGIN_HWATER_BUTTON:
			//switch(HIWORD(wParam))
			//{
			//	case BN_CLICKED:
				//int		res = g_pcsAgcmWaterDlg2->OpenWaterDlg2(m_iSelID);
				//if(res)
				//{
				//	g_pcsAgcmWater->ChangeHWaterStatus(m_iSelID);
				//}
			//	break;
			//}
		}
		break;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

void CPlugin_Water::PositionValidation		( RwV3d	* pPos )	// 로딩 번위 벗어난것을 체크한다.
{
	if( NULL == pPos )
	{
		// do nothing..
	}
	else
	{
		if( pPos->x < GetSectorStartX	( ArrayIndexToSectorIndexX(ALEF_LOAD_RANGE_X1) ) )
			pPos->x = GetSectorStartX	( ArrayIndexToSectorIndexX(ALEF_LOAD_RANGE_X1) );

		if( pPos->x > GetSectorEndX		( ArrayIndexToSectorIndexX(ALEF_LOAD_RANGE_X2-1) ) )
			pPos->x = GetSectorEndX		( ArrayIndexToSectorIndexX(ALEF_LOAD_RANGE_X2-1) );

		if( pPos->z < GetSectorStartZ	( ArrayIndexToSectorIndexZ(ALEF_LOAD_RANGE_Y1) ) )
			pPos->z = GetSectorStartZ	( ArrayIndexToSectorIndexZ(ALEF_LOAD_RANGE_Y1) );

		if( pPos->z > GetSectorEndZ		( ArrayIndexToSectorIndexZ(ALEF_LOAD_RANGE_Y2-1) ) )
			pPos->z = GetSectorEndZ		( ArrayIndexToSectorIndexZ(ALEF_LOAD_RANGE_Y2-1) );


		// 커런트 섹터 수정..
		g_MainWindow.m_pCurrentGeometry	= g_pcsApmMap->GetSector( pPos->x , pPos->z );
	}
}
