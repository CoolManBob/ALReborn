// ConstManager.cpp: implementation of the CConstManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "atlbase.h"
#include "ConstManager.h"
#include "resource.h"

#include <skeleton.h>
#include <camera.h>
#include <menu.h>

#include "rwcore.h"
#include "rpworld.h"
#include "MyEngine.h"

#include "AcuObjecWire.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// 글로벌로 선언.
CConstManager g_Const;

CConstManager::CConstManager()
{
	// 디폴트 벨류..
	m_fSectorWidth			= MAP_SECTOR_WIDTH			;
	m_nHoriSectorCount		= MAP_WORLD_INDEX_WIDTH		;
	m_nVertSectorCount		= MAP_WORLD_INDEX_HEIGHT	;
	m_nSectorDepth			= MAP_DEFAULT_DEPTH			;

	m_nGlobalMode			= LOAD_NORMAL	;
	m_bUsePolygonLock		= TRUE			;
	m_bUseBrushPolygon		= TRUE			;
	
	m_bShowCollisionAtomic	= FALSE			;
	
	// 현재 버젼 레지스트리에 설정.


	// 래지스트리에서 얻어와서 크기 설정.
	CRegKey reg;

	CString str;
	str.Format( "Software\\%s" , "AlefMapTool" );
	reg.Create( HKEY_LOCAL_MACHINE , str );

	if( reg.QueryValue( m_nLoading_range_x1 ,	"LoadRangeX1" ) != ERROR_SUCCESS )
	{
		m_nLoading_range_x1	= 384;
	}
	if( reg.QueryValue( m_nLoading_range_y1 ,	"LoadRangeY1" ) != ERROR_SUCCESS )
	{
		m_nLoading_range_y1	= 384;
	}
	if( reg.QueryValue( m_nLoading_range_x2 ,	"LoadRangeX2" ) != ERROR_SUCCESS )
	{
		m_nLoading_range_x2	= 400;
	}
	if( reg.QueryValue( m_nLoading_range_y2 ,	"LoadRangeY2" ) != ERROR_SUCCESS )
	{
		m_nLoading_range_y2	= 400;
	}

	DWORD	value;
	if( reg.QueryValue( value ,	"UsePolygonLock" ) != ERROR_SUCCESS )
	{
		m_bUsePolygonLock = FALSE;
	}
	else
	{
		if( value )	m_bUsePolygonLock = TRUE	;
		else		m_bUsePolygonLock = FALSE	;
	}
	
	m_nPreviewMapSelectSize	= m_nLoading_range_x2 - m_nLoading_range_x1;
	if( m_nPreviewMapSelectSize == 0 )
		m_nPreviewMapSelectSize = 10; 

	// 마고자 (2005-04-06 오전 10:36:33) : 세이브 정보 정리.
	/*
	if( reg.QueryValue( m_bMapInfo ,			"m_bMapInfo"		) != ERROR_SUCCESS )
	{
		m_bMapInfo			= TRUE;
	}
	if( reg.QueryValue( m_bObjectList ,			"m_bObjectList"		) != ERROR_SUCCESS )
	{
		m_bObjectList		= TRUE;
	}
	if( reg.QueryValue( m_bTileList ,			"m_bTileList"		) != ERROR_SUCCESS )
	{
		m_bTileList			= TRUE;
	}
	if( reg.QueryValue( m_bTileVertexColor ,	"m_bTileVertexColor") != ERROR_SUCCESS )
	{
		m_bTileVertexColor	= TRUE;
	}
	*/

	// 전부다 일단 폴스.. 작업을 함으로 설정이 돼도록..
	m_bMapInfo			= FALSE;
	m_bTileVertexColor	= FALSE;
	m_bTileList			= FALSE;
	m_bObjectList		= FALSE;

	// 마고자 (2003-06-17 오후 6:21:10) : 무조건 안돼게
	m_bObjectTemplate = FALSE;
//	if( reg.QueryValue( m_bObjectTemplate ,		"m_bObjectTemplate"	) != ERROR_SUCCESS )
//	{
//		m_bObjectTemplate	= TRUE;
//	}


	
	m_bDungeonData = FALSE;
	//if( reg.QueryValue( m_bDungeonData ,	"m_bDungeonData") != ERROR_SUCCESS )
	//{
	//	m_bDungeonData	= FALSE;
	//}

	char	strValue[ 1024 ];
	DWORD	size ;

	size = 1024;
	if( reg.QueryValue( strValue ,	"SkyWidth" , & size ) != ERROR_SUCCESS )
	{
		m_fSkyWidth = 4.0f * 2500.0f + 2500.0f;
	}
	else
	{
		m_fSkyWidth = atof( strValue );
	}

	size = 1024;
	if( reg.QueryValue( strValue ,	"FogFarClip" , & size ) != ERROR_SUCCESS )
	{
		m_fFogFarClip = AgcmEventNature::SKY_RADIUS * 1.4145727f * 1.2f;
	}
	else
	{
		m_fFogFarClip = atof( strValue );
	}

	size = 1024;
	if( reg.QueryValue( strValue ,	"FogDistance" , & size ) != ERROR_SUCCESS )
	{
		m_fFogDistance = 4.0f * 2500.0f ;
	}
	else
	{
		m_fFogDistance = atof( strValue );
	}
	if( reg.QueryValue( m_bUseFog ,			"UseFog"		) != ERROR_SUCCESS )
	{
		m_bUseFog			= TRUE;
	}
	if( reg.QueryValue( m_bUseSky ,			"UseSky"		) != ERROR_SUCCESS )
	{
		m_bUseSky			= TRUE;
	}

	if( reg.QueryValue( m_nLoadRange_Data	,	"LoadRange_Data"	) != ERROR_SUCCESS )
	{
		m_nLoadRange_Data	= 8;
	}
	if( reg.QueryValue( m_nLoadRange_Rough	,	"LoadRange_Rough"	) != ERROR_SUCCESS )
	{
		m_nLoadRange_Rough	= 8;
	}
	if( reg.QueryValue( m_nLoadRange_Detail	,	"LoadRange_Detail"	) != ERROR_SUCCESS )
	{
		m_nLoadRange_Detail	= 4;
	}

	if( reg.QueryValue( m_bUseBrushPolygon ,	"UseBrushPolygon"		) != ERROR_SUCCESS )
	{
		m_bUseBrushPolygon			= TRUE;
	}
	
	if( reg.QueryValue( m_bUseEffect ,	"bUseEffect"		) != ERROR_SUCCESS )
	{
		m_bUseEffect			= TRUE;
	}
	
	m_bCameraBlocking			= TRUE;

	// 오브젝트 바운딩 스피어 표시
	m_bShowObjectBoundingSphere	= FALSE;
	// 오브젝트 콜리젼 아토믹 표시
	m_bShowObjectCollisionAtomic= FALSE;
	// 오브젝트 피킹 아토믹 표시
	m_bShowObjectPickingAtomic	= FALSE;
	// 오브젝트 실  포리건 정보 표시
	m_bShowObjectRealPolygon	= FALSE;
	m_bShowOctreeInfo			= FALSE;
}

void	CConstManager::LoadBmpResource()
{
	m_bitmapLoadNormal	.LoadBitmap			( IDB_BACKGROUND		);
	m_brushLoadNormal	.CreatePatternBrush	( & m_bitmapLoadNormal	);

	m_bitmapLoadExport	.LoadBitmap			( IDB_BACKGROUND2		);
	m_brushLoadExport	.CreatePatternBrush	( & m_bitmapLoadExport	);
}

CConstManager::~CConstManager()
{
}

BOOL	CConstManager::Save()
{
	GetCurrentDirectory( 1024 , m_strCurrentDirectory );	
	CreateDirectory( "Map"		, NULL );
	return TRUE;
}

BOOL	CConstManager::Load()
{
	GetCurrentDirectory( 1024 , m_strCurrentDirectory );	
	return TRUE;
}


BOOL	CConstManager::Update()
{
	// 로딩된 설정으로 세팅해야하는 벨류..

	if( m_bUsePolygonLock )
	{
		m_nGeometryLockMode		= rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKNORMALS | rpGEOMETRYLOCKPOLYGONS	;
		m_nVertexColorLockMode	= rpGEOMETRYLOCKPRELIGHT | rpGEOMETRYLOCKPOLYGONS							;
	}
	else
	{
		m_nGeometryLockMode		= rpGEOMETRYLOCKVERTICES | rpGEOMETRYLOCKNORMALS							;
		m_nVertexColorLockMode	= rpGEOMETRYLOCKPRELIGHT													;
	}

	if( AGCMMAP_THIS )
	{
		AGCMMAP_THIS->SetGeometryLockMode		( m_nGeometryLockMode		);
		AGCMMAP_THIS->SetVertexColorLockMode	( m_nVertexColorLockMode	);
	}

	AcuObjecWire::bGetInst().bOnOffSphere		( m_bShowObjectBoundingSphere	);	// 바운딩 스피어
	AcuObjecWire::bGetInst().bOnOffClump		( m_bShowObjectRealPolygon		);	// 실폴리건 정보
	AcuObjecWire::bGetInst().bOnOffCollision	( m_bShowObjectCollisionAtomic	);	// 콜리젼오브젝트 표시
	AcuObjecWire::bGetInst().bOnOffPicking		( m_bShowObjectPickingAtomic	);	// 피킹 오브젝트 표시

	// 마고자 (2005-05-16 오후 5:54:58) : 
	// 퍼포먼스 관련해서 처리..

	if( m_bShowObjectBoundingSphere	||
		m_bShowObjectRealPolygon	||
		m_bShowObjectCollisionAtomic||
		m_bShowObjectPickingAtomic	)
	{
		g_pcsAgcmRender->SetUseAtomicCustomRender( TRUE );
	}
	else
	{
		g_pcsAgcmRender->SetUseAtomicCustomRender( FALSE );
	}

	return TRUE;
}

BOOL	CConstManager::StoreRegistry()
{
	// 레지스트리 세이브.
	CRegKey reg;

	CString str;
	str.Format( "Software\\%s" , "AlefMapTool" );
	reg.Create( HKEY_LOCAL_MACHINE , str );

	reg.SetValue( m_nLoading_range_x1	, "LoadRangeX1"		);
	reg.SetValue( m_nLoading_range_y1	, "LoadRangeY1"		);
	reg.SetValue( m_nLoading_range_x2	, "LoadRangeX2"		);
	reg.SetValue( m_nLoading_range_y2	, "LoadRangeY2"		);

	DWORD	value;
	value = m_bUsePolygonLock;
	reg.SetValue( value					, "UsePolygonLock"	);

	// 마고자 (2005-04-06 오전 10:38:17) : 레지스트리 저장안함.
	/*
	reg.SetValue( m_bMapInfo			, "m_bMapInfo"			);
	reg.SetValue( m_bObjectList			, "m_bObjectList"		);
	reg.SetValue( m_bObjectTemplate		, "m_bObjectTemplate"	);
	reg.SetValue( m_bTileList			, "m_bTileList"			);
	reg.SetValue( m_bTileVertexColor	, "m_bTileVertexColor"	);
	reg.SetValue( m_bDungeonData		, "m_bDungeonData"		);
	*/

	char	strValue[ 1024 ];
	sprintf( strValue , "%f" , m_fSkyWidth );
	reg.SetValue( strValue				, "SkyWidth"	);
	sprintf( strValue , "%f" , m_fFogFarClip );
	reg.SetValue( strValue				, "FogFarClip"	);
	sprintf( strValue , "%f" , m_fFogDistance );
	reg.SetValue( strValue				, "FogDistance"	);
	reg.SetValue( m_bUseFog				, "UseFog"		);
	reg.SetValue( m_bUseSky				, "UseSky"		);

	reg.SetValue( m_nLoadRange_Data		, "LoadRange_Data"		);
	reg.SetValue( m_nLoadRange_Rough	, "LoadRange_Rough"		);
	reg.SetValue( m_nLoadRange_Detail	, "LoadRange_Detail"	);

	reg.SetValue( m_bUseBrushPolygon	, "UseBrushPolygon"		);
	reg.SetValue( m_bUseEffect			, "bUseEffect"		);

	return TRUE;
}

void	SaveSetChangeGeometry	()
{
	g_Const.m_bMapInfo			= TRUE;
}
void	SaveSetChangeMoonee		()
{
	g_Const.m_bTileVertexColor	= TRUE;
}
void	SaveSetChangeTileList	()
{
	g_Const.m_bTileList			= TRUE;
}
void	SaveSetChangeObjectList	()
{
	g_Const.m_bObjectList		= TRUE;
}

void	ClearAllSaveFlags		()
{
	// 플래그 초기화..
	g_Const.m_bMapInfo			= FALSE;
	g_Const.m_bTileVertexColor	= FALSE;
	g_Const.m_bTileList			= FALSE;
	g_Const.m_bObjectList		= FALSE;
}
