// RTMenuSky.cpp : implementation file
//

#include "stdafx.h"
#include "regiontool.h"
#include "RegionToolDlg.h"
#include "RegionMenuDlg.h"
#include "RTMenuSky.h"

#include "EventNatureSettingDlg.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTMenuSky
char *	CRTMenuSky::GetMenuName()
{
	static char _strName[] = "Sky";
	return ( char * ) _strName;
}

CRTMenuSky::CRTMenuSky()
{
	m_nRangeDragMode	= MODE_NONE	;
	m_pcsSelectedObject	= NULL		;
	m_bUnsavedData		= TRUE		;
}

CRTMenuSky::~CRTMenuSky()
{
}

BOOL CRTMenuSky::OnLoadData		()
{
	// 마고자 (2005-06-07 오후 5:19:53) : 
	// 텔레포트 전용이면 데이타 로드 안함.
	if( g_bTeleportOnlyMode ) return TRUE;

	// 마고자 (2004-11-19 오후 6:25:30) : 
	// 템플릿 로딩 
	if( g_pcsAgpmEventNature )
	{
		g_pcsAgpmEventNature->AddDefaultSkySet();

		char	strSkySet[ 1024 ];
		wsprintf( strSkySet , "Ini\\%s" , AGPMNATURE_SKYSET_FILENAME );
		if(  g_pcsAgpmEventNature->SkySet_StreamRead( strSkySet , FALSE ) )
		{
			TRACE( "g_pcsAgpmEventNature->SkySet_StreamRead( strSkySet ) Success\n" );
		}
		else
		{
			TRACE( "스카이 템플릿 로딩실패\n" );
			return FALSE;
		}
	}

	{
		CProgressDlg	dlg;
		dlg.StartProgress( "스카이 데이타 로딩중..." , ( MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ) * ( MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ), this );
		int	nCount = 0 ;

		char strFilename[ FILENAME_MAX ];
		int nDivisionX , nDivisionZ;
		for( nDivisionZ = 0 ; nDivisionZ < MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ; nDivisionZ ++ )
		{
			for( nDivisionX = 0 ; nDivisionX < MAP_WORLD_INDEX_WIDTH / MAP_DEFAULT_DEPTH ; nDivisionX ++ )
			{
				dlg.SetProgress( ++nCount );

				wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST ,  
					MakeDivisionIndex( nDivisionX , nDivisionZ )		);
				g_pcsApmObject->StreamRead( strFilename , FALSE , NULL , TRUE );

				// 마고자 (2005-03-18 오후 3:56:04) : 
				// 락 돼어있는지 점검..
				if( !g_bTeleportOnlyMode )
				{
					CheckLockDivision( MakeDivisionIndex( nDivisionX , nDivisionZ ) , NULL );
				}
			}
		}
		dlg.EndProgress();
	}

	m_bUnsavedData = FALSE;
	// Unsaved Flag 설정.
	return TRUE;
}

static AuBOX	* g_pDivisionBox = NULL;

BOOL	__SkySaveCheckCallBack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ApdObject	* pcsApdObject = ( ApdObject * ) pData;
	ApdEvent	* pEvent			;

	if( NULL == pcsApdObject ) return FALSE;

	pEvent	= g_pcsApmEventManager->GetEvent( pcsApdObject , ( ApdEventFunction ) APDEVENT_FUNCTION_NATURE );

	if( pEvent && g_pDivisionBox )
	{
		// 스폰 정보가 있을때만 저장함
		if( g_pDivisionBox->inf.x			<	pcsApdObject->m_stPosition.x	&&
			pcsApdObject->m_stPosition.x	<=	g_pDivisionBox->sup.x			&&
			g_pDivisionBox->inf.z			<	pcsApdObject->m_stPosition.z	&&
			pcsApdObject->m_stPosition.z	<=	g_pDivisionBox->sup.z			)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL CRTMenuSky::OnSaveData		()
{
	INT32	nCount = 0;
	{
		AuBOX bbox;
		char strFilename[ FILENAME_MAX ];
		int nDivisionX , nDivisionZ;
		int nDivisionIndex;
		for( nDivisionZ = 0 ; nDivisionZ < MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ; nDivisionZ ++ )
		{
			for( nDivisionX = 0 ; nDivisionX < MAP_WORLD_INDEX_WIDTH / MAP_DEFAULT_DEPTH ; nDivisionX ++ )
			{
				if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uFlag &
					DUF_SKY )
				{
					TRACE( "CRTMenuSky::OnSaveData Division %04d 녀석이 변경됐음\n", MakeDivisionIndex( nDivisionX , nDivisionZ ) );
				}
				else
				{
					continue;
				}

				nDivisionIndex = MakeDivisionIndex( nDivisionX , nDivisionZ );
				
				bbox.inf.x	=	GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) ) );
				bbox.inf.z	=	GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) ) );
				bbox.inf.y	=	0.0f;
				bbox.sup.x	=	bbox.inf.x + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH;
				bbox.sup.z	=	bbox.inf.z + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH;
				bbox.sup.y	=	0.0f;

				// 마고자 (2004-11-23 오후 5:13:38) : 갯수 체크 없엠..
				//if( GetObjectCountInBBox( &bbox ) )
				//{
					// 갯수가 몇개인지 체크..
					g_pDivisionBox = &bbox;
					
					wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST ,  
						MakeDivisionIndex( nDivisionX , nDivisionZ )		);
					g_pcsApmObject->StreamWrite( bbox , 0 , strFilename , __SkySaveCheckCallBack );

					// 언세이브 데이타 플래그 제거..
					this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uFlag &= ~DUF_SKY;
					UnlockDivision( MakeDivisionIndex( nDivisionX , nDivisionZ ) );

					nCount++;
				//}

			}
		}
		
		m_bUnsavedData = FALSE;
		// 확인사살..
		this->m_pParent->m_pMainDlg->m_ctlRegion.ClearUnsavedDataCheck( DUF_SKY );
	}

	if( nCount )
	{
		CString	str;
		str.Format( "%d 개의 디비젼 스카이 정보를 저장하였습니다." , nCount );
		MessageBox( str );
	}

	return TRUE;
}

BOOL CRTMenuSky::OnQuerySaveData()
{
	return m_bUnsavedData;
}

BEGIN_MESSAGE_MAP(CRTMenuSky, CWnd)
	//{{AFX_MSG_MAP(CRTMenuSky)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRTMenuSky message handlers

void CRTMenuSky::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 255 , 0 , 0 ) );

	for( int i = 0 ; i < SKY_MESSAGE_LINE ; i ++ )
	{
		dc.TextOut( 0 , i * 20 , m_strMessage[ i ] );
	}
	
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL	CRTMenuSky::Create( CRect * pRect , CWnd * pParent )
{
	return CWnd::Create( NULL , NULL , WS_CHILD | WS_VISIBLE , *pRect , pParent , 1348 );
}

void CRTMenuSky::OnPaintOnMap		( CDC * pDC )
{
	// 오브젝트 정보 출력...
	CPen	penRegion , penSelected;
	penRegion.CreatePen( PS_SOLID , 2 , RGB( 121 , 255 , 255 )	);
	penSelected.CreatePen( PS_SOLID , 2 , RGB( 255 , 120 , 150 )	);
	pDC->SelectObject( penRegion );
	pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
	pDC->SetBkMode( TRANSPARENT );

	{
		
		INT32			lIndex			;
		ApdObject	*	pcsObject		;
		ApdEvent	*	pEvent			;
		AuPOS		*	pPos			;
		INT32			nIndex			;

		CString		strSkyName;

		lIndex = 0;

		INT32		nEventType = APDEVENT_FUNCTION_NATURE	;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject														;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{
			pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

			if( pEvent )
			{
				nIndex = ( INT32 ) pEvent->m_pvData;

				strSkyName.Empty();
				strSkyName = GetTemplateName( nIndex );

				if( pEvent->m_pstCondition )
				{
					ApdEventConditionArea *	pArea = pEvent->m_pstCondition->m_pstArea;
					if( pArea )
					{
						switch( pArea->m_eType )
						{
						case	APDEVENT_AREA_SPHERE	:
							{
								pPos = g_pcsApmEventManager->GetBasePos( pcsObject , NULL );

								if( m_pcsSelectedObject == pcsObject )
								{
									pDC->SelectObject( penSelected );
								}
								else
								{
									pDC->SelectObject( penRegion );
								}

								DrawAreaSphere( pDC , pPos->x , pPos->y , pPos->z , pArea->m_uoData.m_fSphereRadius , 0 , &strSkyName );
							}
							break;
						case	APDEVENT_AREA_FAN		:
						case	APDEVENT_AREA_BOX		:
							{
								TRACE( "지원하지 않는 타입 -_-" );
							}
							break;
						}
					}
				}
				else
				{
					// = =?
				}
				

			}
			else
			{
				// do nothing
			}
		}
	}
	
	// Selection 계산용..
	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		CPen	penSelection;
		penSelection.CreatePen( PS_SOLID , 2 , RGB( 255, 128 , 128 )	);
		pDC->SelectObject( penSelection );
		pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

		int nRadius1 = abs( m_SelectedPosX2 - m_SelectedPosX1 );
		int nRadius2 = abs( m_SelectedPosZ2 - m_SelectedPosZ1 );

		int nRadius = nRadius1 < nRadius2 ? nRadius2 : nRadius1;

		CRect	rectEllipse;
		rectEllipse.SetRect(
			m_SelectedPosX1 - nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
			m_SelectedPosZ1 - nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ,
			m_SelectedPosX1 + nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
			m_SelectedPosZ1 + nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ);

		pDC->Ellipse( rectEllipse );

		if( rectEllipse.Width() < 100 )
		{
			rectEllipse.left	-= 50;
			rectEllipse.right	+= 50;
		}
		if( rectEllipse.Height() < 100 )
		{
			rectEllipse.top		-= 10;
			rectEllipse.bottom	+= 10;
		}

		pDC->SetBkMode( TRANSPARENT );
		
		CString	str;
		str.Format( "%.1fm" , m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_LENGTH( nRadius ) / 100.0f);
		pDC->SetTextColor( RGB( 255 , 255 , 255 ) );
		pDC->DrawText( str , rectEllipse , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}

}

BOOL CRTMenuSky::OnLButtonDblDownMap( CPoint &point , int x , int z )
{
	if( NULL != m_pcsSelectedObject )
	{
		// 선택된것이 있다
		// 이벤트 Dialog 띄움..

		try
		{
			if( LockDivision( GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ) )
			{
				ApdObject * pObject = m_pcsSelectedObject;
				ApdEvent *	pstEvent;

				// EventNature Event Struct 얻어냄..
				pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_NATURE );

				if( pstEvent )
				{
					// 범위설정~
					m_pParent->ReleaseCaptureRegionView();
					CEventNatureSettingDlg	dlg;
					dlg.DoModal( pstEvent );
					m_pParent->ReleaseCaptureRegionView();
				}

				m_bUnsavedData = TRUE;
				// 해당 디비젼 세이브 체크..
				this->m_pParent->m_pMainDlg->m_ctlRegion.
					UnsavedDataCheck(	GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
										DUF_SKY	);
			}
			else
			{
				CString	str;
				str.LoadString( IDS_ALREADY_LOCKED );
				str.Format( "해당 Division(%04d) 을 '%s'아저씨가 쓰고 있어요" , 
					GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
					g_strLastLockOwnerName );
				MessageBox( str );
			}
		}
		catch(...)
		{
			MessageBox( "Sky Dialog를 띄우는데 문제가 발생했어요" );
		}
	}
	return TRUE;
}

BOOL CRTMenuSky::OnLButtonDownMap	( CPoint &pointOriginal , int x , int z )
{
	m_pcsSelectedObject = NULL;
	m_pointLastPress	= pointOriginal ;

	if( GetAsyncKeyState( VK_CONTROL ) >= 0 )
	{
		INT32			lIndex			;
		ApdObject	*	pcsObject		;
		ApdEvent	*	pEvent			;
		AuPOS		*	pPos			;
		AuPOS			posClick		;

		posClick.x	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( x );
		posClick.z	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( z );
		posClick.y	= 0.0f;

		lIndex = 0;

		INT32		nEventType = APDEVENT_FUNCTION_NATURE	;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject														;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{
			pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

			if( pEvent )
			{
				if( pEvent->m_pstCondition )
				{
					ApdEventConditionArea *	pArea = pEvent->m_pstCondition->m_pstArea;
					if( pArea )
					{
						switch( pArea->m_eType )
						{
						case	APDEVENT_AREA_SPHERE	:
							{
								pPos = g_pcsApmEventManager->GetBasePos( pcsObject , NULL );
								
								if( AUPOS_DISTANCE_XZ( * pPos , posClick ) < pArea->m_uoData.m_fSphereRadius )
								{
									// 빙고..
									if( LockDivision( GetDivisionIndexF( pcsObject->m_stPosition.x , pcsObject->m_stPosition.z ) ) )
									{
										m_pcsSelectedObject = pcsObject;
										m_pParent->InvalidateRegionView();
										m_nRangeDragMode = MODE_REGIONMOVE;
									}
									else
									{
										m_nRangeDragMode = MODE_NONE;
										m_pParent->InvalidateRegionView();
										
										CString	str;
										str.LoadString( IDS_ALREADY_LOCKED );
										str.Format( "해당 Division(%04d) 을 '%s'아저씨가 쓰고 있어요" , 
											GetDivisionIndexF( pcsObject->m_stPosition.x , pcsObject->m_stPosition.z ) ,
											g_strLastLockOwnerName );
										MessageBox( str );

										return FALSE;
									}

									break;
								}
							}
							break;
						case	APDEVENT_AREA_FAN		:
						case	APDEVENT_AREA_BOX		:
							{
								TRACE( "지원하지 않는 타입 -_-" );
							}
							break;
						}
					}
				}
				else
				{
					// = =?
				}
				

			}
			else
			{
				// do nothing
			}
		}
	}

	if( NULL == m_pcsSelectedObject )
	{
		FLOAT	fX , fZ;
		fX		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( x );
		fZ		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( z );

		if( LockDivision( GetDivisionIndexF( fX , fZ ) ) )
		{
			m_pParent->SetCaptureRegionView();
		
			int nOffsetX , nOffsetZ;

			nOffsetX = x;
			nOffsetZ = z;

			// 드래그모드..
			m_nRangeDragMode = MODE_RANGESELECTED;
			m_SelectedPosX1	 = m_SelectedPosX2 = nOffsetX;
			m_SelectedPosZ1	 = m_SelectedPosZ2 = nOffsetZ;
			m_pParent->InvalidateRegionView();
		}
		else
		{
			CString	str;
			str.LoadString( IDS_ALREADY_LOCKED );
			MessageBox( str );
		}
	}
	return TRUE;
}

BOOL CRTMenuSky::OnLButtonUpMap		( CPoint &pointOriginal , int x , int z )
{
	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		// 마고자 (2004-09-10 오후 6:04:32) : 
		// 버튼을 뗄때 처리하는 녀석들..
		int nX , nZ ;
		nX = m_SelectedPosX1;
		nZ = m_SelectedPosZ1;

		int nRadius1 = abs( m_SelectedPosX2 - m_SelectedPosX1 );
		int nRadius2 = abs( m_SelectedPosZ2 - m_SelectedPosZ1 );

		int nRadius = nRadius1 < nRadius2 ? nRadius2 : nRadius1;

		FLOAT	fX , fZ , fRadius;

		fX		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( nX );
		fZ		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( nZ );
		fRadius	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_LENGTH( nRadius );

		char	str[ 256 ];
		sprintf( str , "(%.0f,%.0f) Radius = %.0f\n" , fX , fZ , fRadius );
		TRACE( str );

		m_nRangeDragMode = MODE_NONE;

		if( nRadius > 1 )
		{
			if( LockDivision( GetDivisionIndexF( fX , fZ ) ) )
			{
				// 마고자 (2004-11-12 오후 5:36:46) : 
				// 오브젝트 추가.

				ApdObjectTemplate	*	pstApdObjectTemplate	;
				ApdObject			*	pstApdObject			;

				pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate(467);
				if( !pstApdObjectTemplate )
				{
					TRACE( "템플릿 ID에서 템플릿 정보 얻기 실패..\n" );
					return NULL;
				}

				pstApdObject = g_pcsApmObject->AddObject( 
					g_pcsApmObject->GetEmptyIndex( GetDivisionIndexF( fX , fZ ) ) , pstApdObjectTemplate->m_lID );
				ASSERT( NULL != pstApdObject );
				if( !pstApdObject )
				{
					MD_SetErrorMessage( "g_pcsApmObject->AddObject 오브젝트 추가실패.." );
					return NULL;
				}

				// 위치지정..
				pstApdObject->m_stPosition.x	= fX;
				pstApdObject->m_stPosition.z	= fZ;

				try
				{
					ApdObject * pObject = pstApdObject;
					ApdEvent *	pstEvent;

					// EventNature Event Struct 얻어냄..
					pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_NATURE );

					if( NULL == pstEvent )
					{
						// 이벤트 삽입..
						ApdEventAttachData *pstEventAD = g_pcsApmEventManager->GetEventData( pObject );

						pstEvent = g_pcsApmEventManager->AddEvent(pstEventAD, APDEVENT_FUNCTION_NATURE , pObject , FALSE);
					}

					if( pstEvent )
					{
						// 범위설정~
						g_pcsApmEventManager->SetCondition( pstEvent , APDEVENT_COND_AREA );
						pstEvent->m_pstCondition->m_pstArea->m_eType = APDEVENT_AREA_SPHERE;
						pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius = fRadius;

						m_pParent->ReleaseCaptureRegionView();

						CEventNatureSettingDlg	dlg;
						dlg.DoModal( pstEvent );

						m_pParent->ReleaseCaptureRegionView();
					}

					m_bUnsavedData = TRUE;
					// 해당 디비젼 세이브 체크..
					this->m_pParent->m_pMainDlg->m_ctlRegion.
						UnsavedDataCheck(	GetDivisionIndexF( pstApdObject->m_stPosition.x , pstApdObject->m_stPosition.z ) ,
											DUF_SKY	);
				}
				catch(...)
				{
				}
			}
			else
			{
				m_pParent->ReleaseCaptureRegionView();
				
				CString	str;
				str.LoadString( IDS_ALREADY_LOCKED );
				MessageBox( str );
			}
		}
		else
		{
			m_pParent->InvalidateRegionView();
		}
	}

	if( m_nRangeDragMode == MODE_REGIONMOVE)
	{
		m_nRangeDragMode = MODE_NONE;		
	}

	m_pParent->ReleaseCaptureRegionView();

	return TRUE;
}

BOOL CRTMenuSky::OnMouseMoveMap		( CPoint &pointOriginal , int x , int z )
{
	FLOAT	fX , fZ ;

	fX		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( x );
	fZ		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( z );
	
	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		// 범위 조절중..
		m_SelectedPosX2 = x;
		m_SelectedPosZ2 = z;

		m_pParent->InvalidateRegionView();
	}
	
	if( m_pcsSelectedObject && m_nRangeDragMode == MODE_REGIONMOVE )
	{


		if( LockDivision( GetDivisionIndexF( fX , fZ ) ) )
		{
			// 해당 디비젼 세이브 체크..
			this->m_pParent->m_pMainDlg->m_ctlRegion.
				UnsavedDataCheck(	GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
									DUF_SKY	);

			m_pcsSelectedObject->m_stPosition.x	= fX;
			m_pcsSelectedObject->m_stPosition.z	= fZ;

			// 해당 디비젼 세이브 체크..
			this->m_pParent->m_pMainDlg->m_ctlRegion.
				UnsavedDataCheck(	GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
									DUF_SKY	);

			m_pParent->InvalidateRegionView();

			m_bUnsavedData = TRUE;
		}
	}

	// 마고자 (2005-03-18 오후 2:35:40) : 
	// 메시지 표시
	{
		static UINT32 _suLastDivision = 0;
        UINT32 uDivision = GetDivisionIndexF( fX , fZ );

		if( uDivision != _suLastDivision )
		{
			_suLastDivision = uDivision;

			CString	strCheck;
			CheckLockDivision( uDivision , &m_strMessage[ 1 ] );
			m_strMessage[ 0 ].Format( "%04d Division" , uDivision );

			Invalidate( FALSE );
		}
	}

	return TRUE;
}

void	CRTMenuSky::DrawAreaSphere( CDC * pDC , FLOAT fX , FLOAT fY , FLOAT fZ , FLOAT fRadius , INT32 nType , CString *pStr )
{
	fX -= GetSectorStartX( ArrayIndexToSectorIndexX( 0 ) );
	fZ -= GetSectorStartZ( ArrayIndexToSectorIndexZ( 0 ) );

	INT32	nRegionX , nRegionZ;
	nRegionX = ( INT32 ) ( fX / ( 200.0f / m_fScale ) );
	nRegionZ = ( INT32 ) ( fZ / ( 200.0f / m_fScale ) );

	INT32	nRadius;
	nRadius = ( INT32 ) ( fRadius / ( 200.0f / m_fScale ) );

	CRect	rectMap;

	rectMap.SetRect(
		nRegionX + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX - nRadius	,
		nRegionZ + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ - nRadius	,
		nRegionX + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX + nRadius ,
		nRegionZ + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ + nRadius	);

	CRect	rect;
	this->m_pParent->m_pMainDlg->m_ctlRegion.GetClientRect( rect );

	if ( rect.right > rectMap.left && rectMap.right > rect.left)
	{
		if (rect.bottom > rectMap.top && rectMap.bottom > rect.top )
		{
			// 화면에 포함된다..
			// 
			pDC->Ellipse( rectMap );			

			CRect	rectText;
			rectText.left	= nRegionX - 100	+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX;
			rectText.top	= nRegionZ - 10		+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ;
			rectText.right	= nRegionX + 100	+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX;
			rectText.bottom	= nRegionZ + 10		+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ;
			pDC->SetTextColor( RGB( 255 , 255 , 255 ) );
			pDC->DrawText( *pStr , rectText , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		}
	}
}

BOOL CRTMenuSky::OnDeleteKeyDown()
{
	if( m_pcsSelectedObject )
	{
		m_bUnsavedData = TRUE;
		// 해당 디비젼 세이브 체크..
		this->m_pParent->m_pMainDlg->m_ctlRegion.
			UnsavedDataCheck(	GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
								DUF_SKY	);
		g_pcsApmObject->DeleteObject( m_pcsSelectedObject );
		m_pcsSelectedObject = NULL;
		m_pParent->InvalidateRegionView();
	}

	return TRUE;
}

char *	CRTMenuSky::GetTemplateName( INT32 nTemplateIndex )
{
	static char __sErrorString[] = "No Template";

	AuList< AgpdSkySet * >	* pList = g_pcsAgpmEventNature->GetSkySetList();
	AuNode< AgpdSkySet * >	* pNode	= pList->GetHeadNode();
	AgpdSkySet				* pSkySet	;

	while( pNode )
	{
		pSkySet	= pNode->GetData();

		if( nTemplateIndex == pSkySet->m_nIndex )
		{
			// 이놈이렸다!..
			return pSkySet->m_strName;
		}

		pList->GetNext( pNode );
	}

	return __sErrorString;
}

int	CRTMenuSky::GetObjectCountInBBox( AuBOX * pBox )
{
	if( NULL == pBox ) return 0;

	int nCount = 0;
	INT32			lIndex			;
	ApdObject	*	pcsObject		;
	ApdEvent	*	pEvent			;

	lIndex = 0;

	INT32		nEventType = APDEVENT_FUNCTION_NATURE	;

	for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject														;
			pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

		if( pEvent )
		{
			if( pBox->inf.x					<	pcsObject->m_stPosition.x	&&
				pcsObject->m_stPosition.x	<=	pBox->sup.x					&&
				pBox->inf.z					<	pcsObject->m_stPosition.z	&&
				pcsObject->m_stPosition.z	<=	pBox->sup.z					)
			{
				nCount ++;
			}
		}
		else
		{
			// do nothing
		}
	}

	return nCount;
}

BOOL	CRTMenuSky::CheckLockDivision( UINT32 uDivisionIndex , CString * pMsg )
{
	return TRUE;
	/*
	if( g_bTeleportOnlyMode )
	{
		if( pMsg ) pMsg->Format( "OffLine Mode" );
		return TRUE;
	}

	UINT32	nDivisionX , nDivisionZ ;

	nDivisionX	= GetDivisionXIndex( uDivisionIndex );
	nDivisionZ	= GetDivisionZIndex( uDivisionIndex );

	if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock & DUF_SKY )
	{
		// 이미 락 되어있다.
		if( pMsg ) pMsg->Format( "Lock 中" );
		return TRUE;
	}
	else
	{
		char strFilename[ FILENAME_MAX ];
		wsprintf( strFilename , RM_RK_DIRECTORY_BIN APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST , uDivisionIndex );

		// 해당 파일 이 락 가능한지 검사..
		// 이미 락이 되어 있는지 검사..
		// 안되어있으면 락을 하고..
		// 안돼는경우 리턴 FALSE.

		if( IsLock( strFilename ) )
		{
			CString	strOwner;
			GetLockOwner( strFilename , strOwner );

			if( strOwner == this->m_pParent->m_pMainDlg->m_strUserName )
			{
				// 이미 락하고 있는거.

				// 락 플레그 설정..
				this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock |= DUF_SKY;
				if( pMsg ) pMsg->Format( "Lock 中" );
				this->m_pParent->m_pMainDlg->m_ctlRegion.UnsavedDataCheck( uDivisionIndex , DUF_SKY );
				return TRUE;
			}
			else
			{
				// 누가 쓰고 있어.
				if( pMsg ) pMsg->Format( "%s가 Lock中" , strOwner );
				return FALSE;
			}
		}

		// 파일이 없으면 업로드 시킨다..
		if( IsExistFileToRemote( strFilename ) )
		{
			if( pMsg ) pMsg->Format( "Lock 가능" );
			return TRUE;
		}
		else
		{
			if( pMsg ) pMsg->Format( "파일없음" );
			return TRUE;
		}
	}
	*/
}
BOOL	CRTMenuSky::LockDivision	( UINT32 uDivisionIndex )
{
	return TRUE;
	/*
	if( g_bTeleportOnlyMode ) return TRUE;

	UINT32	nDivisionX , nDivisionZ ;

	nDivisionX	= GetDivisionXIndex( uDivisionIndex );
	nDivisionZ	= GetDivisionZIndex( uDivisionIndex );

	if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock & DUF_SKY )
	{
		// 이미 락 되어있다.
		return TRUE;
	}
	else
	{
		char strFilename[ FILENAME_MAX ];
		wsprintf( strFilename , RM_RK_DIRECTORY_BIN APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST , uDivisionIndex );

		// 해당 파일 이 락 가능한지 검사..
		// 이미 락이 되어 있는지 검사..
		// 안되어있으면 락을 하고..
		// 안돼는경우 리턴 FALSE.

		if( IsLock( strFilename ) )
		{
			CString	strOwner;
			GetLockOwner( strFilename , strOwner );

			if( strOwner == this->m_pParent->m_pMainDlg->m_strUserName )
			{
				// 이미 락하고 있는거.

				// 락 플레그 설정..
				this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock |= DUF_SKY;
				return TRUE;
			}
			else
			{
				// 누가 쓰고 있어.
				g_strLastLockOwnerName = strOwner;
				return FALSE;
			}
		}

		// 파일이 없으면 업로드 시킨다..
		if( IsExistFileToRemote( strFilename ) )
		{
			// 최신파일인지 확인..
			if( !IsLatestFile( strFilename ) )
			{
				if( FileDownLoad( strFilename ) )
				{
					// 파일 받기 성공..
				}
				else
				{
					MessageBox( "최신파일 받기 실패!" );
					return FALSE;
				}
			}

			if( Lock( strFilename ) )
			{
				// 락 플레그 설정..
				this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock |= DUF_SKY;
				return TRUE;
			}
			else
			{
				// 락 실패
				return FALSE;
			}
		}
		else
		{
			// 파일자체가 없으니까..
			// 그냥 락 설정.
			// 락 플레그 설정..
			this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock |= DUF_SKY;
			return TRUE;
		}

	}
	*/
}

BOOL	CRTMenuSky::UnlockDivision( UINT32 uDivisionIndex )
{
	return TRUE;
	/*
	if( g_bTeleportOnlyMode ) return TRUE;

	UINT32	nDivisionX , nDivisionZ ;

	nDivisionX	= GetDivisionXIndex( uDivisionIndex );
	nDivisionZ	= GetDivisionZIndex( uDivisionIndex );

	if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock & DUF_SKY )
	{
		// 락이 되어 있는것만 처리..
		if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uFlag & DUF_SKY )
		{
			// 세이브 돼지 않은 데이타가 있는 경우...
			// 이 경우는 언락을 할 수 없음..
			return FALSE;
		}
		else
		{
			// 없는 경우...
			// 데이타를 서버에 올리고 락을 푼다.
			char strFilename[ FILENAME_MAX ];
			wsprintf( strFilename , RM_RK_DIRECTORY_BIN APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST , uDivisionIndex );

			if( FileUpload( strFilename ) )
			{
				if( UnLock( strFilename ) )
				{
					this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock &= ~DUF_SKY;
				}
//				else
//				{
//					MessageBox( "락을 푸는게 실패!" );
//				}
				return TRUE;
			}
			else
			{
				TRACE( "파일 업로드 실패..\n" );
				return FALSE;
			}
		}
	}
	else
	{
		// 락이 돼지 않은 녀석이므로... 처리하지 않음..
		return FALSE;
	}
	*/
}