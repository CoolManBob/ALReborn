// AlefMapDocument.cpp: implementation of the CAlefMapDocument class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapTool.h"
#include "AlefMapDocument.h"

#include "ApmMap.h"
#include "AgcmMap.h"

#include "SubDivisionDlg.h"


#include "MyEngine.h"
#include "MainFrm.h"

#include "MainWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CMainFrame *		g_pMainFrame;
extern MainWindow		g_MainWindow;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAlefMapDocument::CAlefMapDocument()
{
	m_bModified				= FALSE					;

	m_nSelectedMode			= EDITMODE_GEOMETRY		;
	m_nShowGrid				= GRIDMODE_D_GETHEIGHT	;
	
	m_nCurrentTileLayer		= TILELAYER_BACKGROUND	;

	m_fBrushRadius			= UIGM_DEFAULT_RADIUS	;
	m_nBrushType			= UIGM_DEFAULT_BRUSHTYPE;
	m_bToggleShadingMode	= false					;

	m_nDWSectorSetting		= DWSECTOR_ALLDETAIL	;
	
	m_nCurrentAlphaTexture	= ALEF_DEFAULT_ALPHA	;


	m_nSubDivisionDetail	= CSubDivisionDlg::SUBDIVISIONDETAIL_ONE	;
	m_bSubDivisionTiled		= TRUE										;
	m_bSubDivisionUpperTiled= FALSE										;

	m_bUseFirstPersonView	= false					;
	m_bFPSEditMode			= FALSE					;

	m_nSelectedDetail		= SECTOR_HIGHDETAIL		;
	
	m_bShowObject			= TRUE					;

	m_bShowObjectBlocking	= FALSE					;	// 오브젝트  블러킹 표시 온오프.
	m_bShowGeometryBlocking	= FALSE					;	// 지형 블러킹  표시 온오프.

	m_nRangeSphereType		= RST_NONE				;
	m_bShowSystemObject		= TRUE					;
	
	m_uAtomicFilter			= AF_SHOWALL			;
	m_uRunSpeed				= 6000					;

	m_fCameraMovingSpeed	= ALEF_SECTOR_WIDTH		;
}

CAlefMapDocument::~CAlefMapDocument()
{

}

void	CAlefMapDocument::NewDocument	()
{
	// -_-
}

BOOL	CAlefMapDocument::Save		()
{
	return FALSE;
}

BOOL	CAlefMapDocument::SaveAs		()
{
	return FALSE;
}

BOOL	CAlefMapDocument::Load		( CString filename )
{
	// -_-+
	return FALSE;
}

BOOL	CAlefMapDocument::SetModified	( BOOL bModify )
{
	BOOL	ret = m_bModified;
	m_bModified = bModify;
	return ret;
}

BOOL	CAlefMapDocument::IsModified	()
{
	return m_bModified;
}

void	CAlefMapDocument::ToggleGrid	()
{
	// 그리드 표시 토글..

	if( ISBUTTONDOWN( VK_SHIFT ) )
	{
		m_nShowGrid	= ( m_nShowGrid + 1 ) % GRIDMODE_COUNT;
	}
	else
	{
		m_nShowGrid	= ( m_nShowGrid + 1 ) % 2;
	}
}

int		CAlefMapDocument::GetSelectedTileIndex()
{
	switch( m_listSelectedTileIndex.GetCount() )
	{
	case 0:
		return 0;

	default:	// 여러개 등록돼도 처음것만..
	case 1:
		{
			switch( GET_TEXTURE_TYPE( m_listSelectedTileIndex[ 0 ] ) )
			{
			case TT_FLOORTILE	:
				{
					// 랜덤하게 조합해서 사용.
					int index;
					index = _GetRandTileOffset	( m_listSelectedTileIndex[ 0 ] );
					return index;
				}
				break;
			case TT_UPPERTILE	:
			case TT_ONE			:
			case TT_ALPHATILE	:
			default:
				return m_listSelectedTileIndex[ 0 ];
			}
			
		}
		
	//default:
		//return m_listSelectedTileIndex[ rand() % m_listSelectedTileIndex.GetCount() ];
	}
}

void	CAlefMapDocument::ToggleFirstPersonViewMode	( INT32	nTemplateID )
{
	g_MainWindow.Reset();
		// 현재 카메라를 기준으로 방향을 설정한다...
	RwFrame		*pFrame		;
	RwMatrix	*pMatrix	;
	RwV3d		*pLookAt	;
	RwV3d		*pPos		;

	if( FALSE == m_bUseFirstPersonView )
	{
		SaveCameraPosition();
	}
	
	m_bUseFirstPersonView	= ! m_bUseFirstPersonView	;
	
	g_pMainFrame->m_pToolBar->SetFPSEditMode( FALSE );

	if( m_bUseFirstPersonView )
	{
		// 현재 카메라를 기준으로 방향을 설정한다...
		pFrame	= RwCameraGetFrame	( g_pEngine->m_pCamera	);
		pMatrix	= RwFrameGetLTM		( pFrame	);
		pLookAt	= RwMatrixGetAt		( pMatrix	);
		pPos	= RwMatrixGetPos	( pMatrix	);

		AuPOS	pos;
		pos.x	= pPos->x;
		pos.y	= 0.0f;
		pos.z	= pPos->z;

		// 블러킹 체크
		if( g_pcsApmMap->CheckBlockingInfo( pos , ApmMap::GROUND ) )
		{
			g_pMainFrame->MessageBox( "케릭터가 생성될 위치에 블러킹이 지정돼어 있어 케릭터를 추가할 수 없어요! 블러킹 확인해보세요" , "케릭터모드" , MB_ICONERROR );
			m_bUseFirstPersonView = FALSE;
			return;
		}

		g_pcsAgcmCamera2->bSetIdleType( AgcmCamera2::e_idle_default	);

//		g_pcsAgcmRender.SetMainFrame( RwCameraGetFrame( g_MyEngine.m_pCamera ));		// modify by gemani
		
		// 오브젝트 블러킹 정보 새로 작성함..
		g_pcsApmMap->ClearAllObjectBlocking();
		g_pcsApmMap->ClearAllHeightPool();
		
// 마고자 (2003-11-05 오전 10:56:57) : 무슨 문제가 있을지 모름..
//		INT32				lIndex = 0	;
//		ApdObject *			pcsApdObject;
//
//		// 등록된 모든 Object에 대해서...
//		for (	pcsApdObject = g_pcsApmObject.GetObjectSequence(&lIndex)		;
//				pcsApdObject									;
//				pcsApdObject = g_pcsApmObject.GetObjectSequence(&lIndex)		)
//		{
//			// 콜백을 호출해줌..
//			AgcmMap::CBOnAddObject( 
//				( PVOID ) pcsApdObject			,
//				( PVOID ) AGCMMAP_THIS			,
//				NULL							);
//		}		



		// FP 모드 들어가는것 설정은 여기서..
		AgpdCharacter * pCharacter;
		//g_pcsAgcmCharacter->StreamReadTemplate(".\\Ini\\CharacterTemplate.ini");
		g_pcsAgcmCharacter->m_lSelfCID	= 1;

		pCharacter = g_pcsAgpmCharacter->AddCharacter( 1 , nTemplateID , "-_-!" );

		ASSERT( NULL != pCharacter );
		if( NULL == pCharacter )
		{
			// 마고자 (2004-06-15 오후 4:02:08) : 실패..
			m_bUseFirstPersonView	= ! m_bUseFirstPersonView	;

			g_pMainFrame->MessageBox( "케릭터 모드로 바꿀수가 없어요! 문의 바랍니다." );
			return;
		}

		ApWorldSector * pWorldSector;
		pWorldSector = g_pcsApmMap->GetSector( pos.x , pos.z );

		if( pWorldSector )
		{
			double theta;

			double x2 , y2;
			x2 = ( double ) ( pLookAt->x );
			y2 = ( double ) ( pLookAt->z );

			if( x2 >= 0.0 )
			{
				// 0보다 크면..
				theta = acos( -y2 );
			}
			else
			{
				// 0보다 작으면..
				theta = acos( y2 );
				theta = 3.1415927 * 2 - theta;
			}

			pos.y = AGCMMAP_THIS->HP_GetHeight( pos.x , pos.z );
			//g_pcsAgcmCamera.UpdateCamera();
			pCharacter->m_unCurrentStatus	= AGPDCHAR_STATUS_IN_GAME_WORLD	;
			pCharacter->m_stPos				= pos							;

			AGCMMAP_THIS->SetCurrentDetail( SECTOR_EMPTY );
			g_pcsAgpmFactors->CopyFactor( &pCharacter->m_csFactor, &pCharacter->m_pcsCharacterTemplate->m_csFactor , TRUE );
			g_pcsAgpmCharacter->UpdateInit( pCharacter );
			g_pcsAgcmCharacter->SetSelfCharacter( pCharacter );

			g_pcsAgpmCharacter->UpdatePosition	( pCharacter , &pos					); 
			g_pcsAgcmCharacter->TurnSelfCharacter( 0.0f , (float) theta * 180.0f / 3.1415927f	);

			// 최초 러닝 스피드 6000으로..
			g_pcsAgpmFactors->SetValue(&pCharacter->m_csFactor, 6000, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// 창업데이트 ..
			g_pMainFrame->m_Document.m_uRunSpeed	= g_pcsAgpmCharacter->GetFastMoveSpeed( pCharacter );

			if( g_pMainFrame->m_pTileList ) g_pMainFrame->m_pTileList->Invalidate( FALSE );
		}
		else
		{
			m_bUseFirstPersonView = false;
			ASSERT( !"카메라위치가 이상합니다." );
		}
	}
	else
	{
		// FP 모드 빠져 나오는것 설정은 여기서..

		g_pcsAgpmCharacter->RemoveCharacter( 1 );
		
		// 디테일을 원래 설정대로 변경..
		AGCMMAP_THIS->SetCurrentDetail( m_nSelectedDetail );

		// Get Height 정보 초기화..
		AGCMMAP_THIS->RemoveHPInfo();

		// RestoreCameraPosition();
		g_pcsAgcmRender->SetMainFrame( RwCameraGetFrame( g_MyEngine.m_pCamera ));		// modify by gemani

		g_pcsAgcmCamera2->bSetIdleType( AgcmCamera2::e_idle_login	);
	}
}

void	CAlefMapDocument::SaveCameraPosition	()
{
	RwFrame * f		= RwCameraGetFrame( g_pEngine->m_pCamera );
	m_MatrixCamera	= *RwFrameGetMatrix( f );
}

void	CAlefMapDocument::RestoreCameraPosition	()
{
	RwFrame * f = RwCameraGetFrame( g_pEngine->m_pCamera );
	RwFrameTransform( f , &m_MatrixCamera , rwCOMBINEREPLACE );
}
