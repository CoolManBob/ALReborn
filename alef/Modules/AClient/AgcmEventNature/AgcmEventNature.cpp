// AgcmEventNature.cpp: implementation of the AgcmEventNature class.
//
//////////////////////////////////////////////////////////////////////
#include "ApBase.h"
#include "MagDebug.h"
#include "rpcollis.h"
#include "AgcmEventNature.h"

#include "AgpdCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmCharacter.h"

#include "AuIniManager.h"
#include "rtintsec.h"

#include "AgcEngine.h"
#include "AgcmUIConsole.h"
#include "ApModuleStream.h"

#include <d3d9.h>

#define	CIRCUMSTANCEATOMICDETAIL	12
#define	CIRCUMSTANCEATOMICUVDEPTH	1.0f

// 마고자 (2005-03-16 오후 7:43:33) : 
// 배경음악 디버그용 로그
//#ifdef _DEBUG
//#define	BGM_PLAY_DEBUG
//#endif
// #define	BGM_PLAY_DEBUG

// BGM 재생 주기 , 10초
static INT32 BGM_REPEAT_GAP	= ( 10000 );

const	INT32	CLOUD_DETAIL	= 4;
FLOAT	AgcmEventNature::__fsSunAngle = 16.5f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const	FLOAT	_PI	=	3.1415927f;

//FLOAT	AgcmEventNature::SKY_RADIUS				= 100000.0f			;
//FLOAT	AgcmEventNature::SKY_HEIGHT				= 15000.0f	* 2.5f	;
//FLOAT	AgcmEventNature::CIRCUMSTANCE_RADIUS	= 100000.0f			;
//FLOAT	AgcmEventNature::CIRCUMSTANCE_HEIGHT	= 15000.0f	* 2.5f	;

// 마고자 (2004-03-22 오전 11:28:02) : 승후아저씨 수정. 
FLOAT	AgcmEventNature::SKY_RADIUS				= 700000.0f	;
FLOAT	AgcmEventNature::SKY_HEIGHT				= 30000.0f	;
FLOAT	AgcmEventNature::CIRCUMSTANCE_RADIUS	= 40000.0f	;
FLOAT	AgcmEventNature::CIRCUMSTANCE_HEIGHT	= 25000.0f	;

//static UINT32	__uSkyChangeGap = 1800000;

// 마고자 (2004-05-19 오전 5:03:08) : 5초간 바뀌게 된다.
UINT32	__uSkyChangeGap		= 25000	;
static FLOAT	__cloud_flow_adjust	= 1.5f	;


//#define SPHERE_MESH_SIZE		8
//#define NUM_SPHERE_VERTICES		( 2 + SPHERE_MESH_SIZE * SPHERE_MESH_SIZE * 2 ) 			
//#define NUM_SPHERE_TRIANGLES	( ( SPHERE_MESH_SIZE * 4 + SPHERE_MESH_SIZE * 4 * ( SPHERE_MESH_SIZE - 1 ) ) )
//#define NUM_SPHERE_INDICES		( NUM_SPHERE_TRIANGLES * 3 )

AgcmEventNature* AgcmEventNature::m_pThis = NULL;

rwD3D9DeviceRestoreCallBack AgcmEventNature::OldD3D9RestoreDeviceCB = NULL;
rwD3D9DeviceReleaseCallBack AgcmEventNature::OldD3D9ReleaseDeviceCB = NULL;

#define SPHERE_HORI_MESH_SIZE		16
#define SPHERE_VERT_MESH_SIZE		16

#define NUM_SPHERE_VERTICES			( ( SPHERE_HORI_MESH_SIZE ) * ( SPHERE_VERT_MESH_SIZE ) )
#define NUM_SPHERE_TRIANGLES		( 2 * ( SPHERE_VERT_MESH_SIZE - 3 ) * ( SPHERE_HORI_MESH_SIZE - 1 ) + ( SPHERE_HORI_MESH_SIZE - 1 ) * 2 )
#define NUM_SPHERE_INDICES			( NUM_SPHERE_TRIANGLES * 3 )

#define rnd() ( ( ((FLOAT)rand())-((FLOAT)rand()) ) / RAND_MAX )

static const char	_EVENT_NATURE_SECTIONNAME[]	= "";

static const char g_strINITextureSection				[]	= "Texture"					;
static const char g_strINITextureKey					[]	= "%02d"					;

static const char g_strINISettingSection				[]	= "Setting"					;
static const char g_strINISettingKeySkyTopColor			[]	= "%02d_SkyTop"				;
static const char g_strINISettingKeySkyBottomColor		[]	= "%02d_SkyBottom"			;
static const char g_strINISettingKeyAmbientColor		[]	= "%02d_Ambient"			;
static const char g_strINISettingKeyDirectionalColor	[]	= "%02d_Directional"		;

static const char g_strINISettingKeyFogColor			[]	= "%02d_FogColor"			;

static const char g_strINISettingKeyFogDistance			[]	= "%02d_FogDistance"		;
static const char g_strINISettingKeyFogFarClip			[]	= "%02d_FogFarClip"			;

static const char g_strINISettingKeyCloudTexture1		[]	= "%02d_CloudTexture1"		;
static const char g_strINISettingKeyCloudSpeed1			[]	= "%02d_CloudSpeed1"		;
static const char g_strINISettingKeyCloudTexture2		[]	= "%02d_CloudTexture2"		;
static const char g_strINISettingKeyCloudSpeed2			[]	= "%02d_CloudSpeed2"		;

static const char g_strINISettingKeyCloudColor1			[]	= "%02d_CloudColor1"		;
static const char g_strINISettingKeyCloudColor2			[]	= "%02d_CloudColor2"		;

static const char g_strINISettingCircumstanceTexture	[]	= "%02d_Circumstance"		;
static const char g_strINISettingCircumstanceColor		[]	= "%02d_CircumstanceColor"	;

static const char g_strINISettingSunTexture				[]	= "%02d_Sun"				;
static const char g_strINISettingSunColor				[]	= "%02d_SunColor"			;

static const char g_strINISettingEffectInterval			[]	= "%02d_EffectInterval"		;
static const char g_strINISettingEffectIndex			[]	= "%02d_EffectIndex_%02d"	;
static const char g_strINISettingEffectRate				[]	= "%02d_EffectRate_%02d"	;

static const char g_strINISettingBackGroundMusic		[]	= "%02d_BackGroundMusic"	;

static const char g_strINISettingWeatherEffect1			[]	= "%02d_WeatherEffect1"		;
static const char g_strINISettingWeatherEffect2			[]	= "%02d_WeatherEffect2"		;

static RwV3d	zaxis;
static RwV3d	xaxis;
static RwV3d	yaxis;


extern AgcEngine*	g_pEngine;

// 마고자 2004/05/17
static FLOAT	__fNearClipPlane = 10.0f ;

#ifdef _DEBUG
BOOL	AgcmEventNature::m_bInstanced	= FALSE;
#endif

#define RANDOMFLOAT()	( ( FLOAT ) ( rand() * 2 ) / ( FLOAT ) ( RAND_MAX ) - 1.0f )
#define STARVERTEXSetRGBA(_vert, _r, _g, _b, _a) \
{ \
    ((_vert)->color = ((_a) << 24) | \
                              ((_r) << 16) | \
                              ((_g) << 8) | \
                              ((_b))); \
}

#define RANDINT( nAvg , nMax ) ( ( nAvg - ( nMax >> 1 ) ) + rand() % ( nMax ) )

#define STARVERTEXCALCY( vert ) \
	(vert)->y = 1.0f - sqrt( (vert)->x * (vert)->x + (vert)->z * (vert)->z );

#define STARVERTEXSetAlpha(_vert, _a) \
{ \
    ((_vert)->color =		 ((_a) << 24) |						\
                              ((_vert)->color & 0x00ffffff)) ;	\
}
static int		g_nStarCountPerOneTimeBlinkRate = 20	; // %
static UINT32	g_uStarBlinkGap					= 100	;

static BOOL __IsInSectorRadius		( ApWorldSector * pSector , float x , float z , float radius )
{
	if( radius == 0.0f )
	{
		if( x >= pSector->GetXStart() && x < pSector->GetXEnd() && z >= pSector->GetZStart() && z < pSector->GetZEnd() )
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		RwSphere	sphere;

		// 스피어 설정.
		sphere.center.x	= x		;
		sphere.center.y	= z		;
		sphere.center.z	= 0.0f	;

		sphere.radius	= radius;

		// 트라이엥글 설정
		// 0 1
		// 2 3
		RwV3d	vertex[ 4 ];
		vertex[ 0 ].x	=	pSector->GetXStart	()	;
		vertex[ 0 ].y	=	pSector->GetZStart	()	;
		vertex[ 0 ].z	=	0.0f					;

		vertex[ 1 ].x	=	pSector->GetXEnd	()	;
		vertex[ 1 ].y	=	pSector->GetZStart	()	;
		vertex[ 1 ].z	=	0.0f					;

		vertex[ 2 ].x	=	pSector->GetXStart	()	;
		vertex[ 2 ].y	=	pSector->GetZEnd	()	;
		vertex[ 2 ].z	=	0.0f					;

		vertex[ 3 ].x	=	pSector->GetXEnd	()	;
		vertex[ 3 ].y	=	pSector->GetZEnd	()	;
		vertex[ 3 ].z	=	0.0f					;

		RwV3d	normal;
		normal.x	= 0.0f;
		normal.y	= 0.0f;
		normal.z	= 1.0f;

		RwReal	distance = 0.0f;

		if( RtIntersectionSphereTriangle( &sphere , vertex + 0 , vertex + 1 , vertex + 2 , &normal , &distance ) )
		{
			// 아자!..
			return TRUE;
		}
		else
		if( RtIntersectionSphereTriangle( &sphere , vertex + 1 , vertex + 3 , vertex + 2 , &normal , &distance ) )
		{
			// 아자!..
			return TRUE;
		}
		else return FALSE;
		return FALSE;
	}
	
}

inline int							GETSPHEREINDEX	( int x , int y )
{
	int	index = ( x + y * ( SPHERE_HORI_MESH_SIZE ) );

	ASSERT( index < NUM_SPHERE_VERTICES );
	return index ;
}

inline RtWorldImportVertex		*	GETSPHEREVERT	( RtWorldImportVertex *	pVertices , int x , int y )
{
	return &pVertices[ GETSPHEREINDEX( x , y ) ];
}

void	_SetVertex( RtWorldImportVertex * pVertex , FLOAT x , FLOAT y , FLOAT z , FLOAT nx , FLOAT ny , FLOAT nz , FLOAT u , FLOAT v )
{
	(*pVertex).OC.x		= x		;
	(*pVertex).OC.y		= y		;
	(*pVertex).OC.z		= z		;

	(*pVertex).normal.x	= nx	;
	(*pVertex).normal.y	= ny	;
	(*pVertex).normal.z	= nz	;

	(*pVertex).texCoords[ 0 ].u	= u;
	(*pVertex).texCoords[ 0 ].v	= v;

	(*pVertex).preLitCol.red	= 255;
	(*pVertex).preLitCol.green	= 255;
	(*pVertex).preLitCol.blue	= 255;
	(*pVertex).preLitCol.alpha	= 255;
}

BOOL	AgcmEventNature::__PreRenderCallback(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pClass );
	
	AgcmEventNature	* pAgcmEventNature	= ( AgcmEventNature * ) pClass;

	// 스카이 렌더..
	pAgcmEventNature->RenderSky();

	return TRUE;
}


BOOL	AgcmEventNature::IsMaptoolBuild()
{
#ifdef USE_MFC
	return TRUE;
#else
	return FALSE;
#endif
}

AgcmEventNature::AgcmEventNature():
	m_bNatureEffectPlaying( FALSE )
{
	SetModuleName("AgcmEventNature");

	// 이 Module은 Datamodule이다.
	m_uType = AgcModule::DATAMODULE;

	m_pLightAmbient		= NULL;
	m_pLightDirectional	= NULL;

	//m_fCloudDirection	= AGCEVENTNATURE_DEFAULT_CLOUD_DIRECTION;

	m_pcsApmMap				= NULL	;
	m_pcsAgcmMap			= NULL	;
	m_pcsAgpmEventNature	= NULL	;
	m_pcsAgcmRender			= NULL	;
	m_pcsApmEventManager	= NULL	;
	m_prwTextureDictionary	= NULL	;
	m_pcsAgcmEff2			= NULL	;
	m_pcsAgcmResourceLoader	= NULL	;
	m_pcsApmObject			= NULL	;
	m_pcsAgcmSound			= NULL	;
	m_pcsAgpmTimer			= NULL	;
	m_pcsAgcmNatureEffect	= NULL	;

	m_bShowSky				= FALSE	;

	m_bInit					= FALSE	;
	m_bFogOn				= TRUE	;
	m_fFogDistance			= 10000.0f;

	m_bFogUpdate			= FALSE	;

	m_pcsApmMap				= NULL	;

	m_prwTextureDictionary	= NULL	;

	m_nCurrentTime			= 60 * 1000 * 60 + 1	;

	zaxis.x	= 0.0f;
	zaxis.y	= 0.0f;
	zaxis.z	= 1.0f;

	xaxis.x	= 1.0f;
	xaxis.y	= 0.0f;
	xaxis.z	= 0.0f;

	yaxis.x = 0.0f;
	yaxis.y = 1.0f;
	yaxis.z = 0.0f;

	#ifdef _DEBUG
	ASSERT( !AgcmEventNature::m_bInstanced );
	AgcmEventNature::m_bInstanced	= TRUE;
	#endif

	m_fSkyRadius	= 0;
	m_bShowSky		= 0;	// 현재 하늘이 표시돼고 있는지 저장..

	ZeroMemory(&m_posLastPosition, sizeof(RwV3d));
	ZeroMemory(&m_posSun, sizeof(RwV3d));

	ZeroMemory(&m_stFogColor, sizeof(RwRGBA));

	m_bSkyIsChanging	= 0	;	// 현재 하늘이 변하고 있는지 
	m_nSkyChangeStartTime	= 0	;	// 하늘 별화 시작시간.. 명령을 받은 시간 저장

	m_pCamera				= 0;
	m_bFogUpdate			= 0;	// 포그를 업데이트해야한다..
	m_fFogFarClip			= 50000.0f;
	m_nSkySetAttachIndex	= 0;
	m_fSkyCircleRadius		= AgcmEventNature::SKY_RADIUS;

	for( int i = 0 ; i < SECTOR_MAX_NATURE_OBJECT_COUNT ; i ++ )
	{
		m_skyMixedUpIndex[ i ] = 0;
	}

	m_nCurrentBGMSetIndex = 0;

	EnableIdle( TRUE );

	// 디폴트 BGM 디렉토리 설정.
	SetBGMDirectory( AGCMEVENTNATURE_DEFAULT_BGM_DIR );

	// 마고자 (2004-06-07 오후 3:20:11) : 맵툴 디버그 플래그.
	m_uMaptoolDebugInfoFlag	= SKY_DONOTCHANGE	;

	// 마고자 (2004-06-08 오후 6:37:54) : 별데이타 초기화
	m_pD3DStarVertex			= NULL				;
	m_pStarFrame				= NULL				;
	m_nStarShowLevel			= MAX_STAR			;
	DestroyStar();

	m_nLastPlayedWeatherEffect1 = -1;
	m_nLastPlayedWeatherEffect2 = -1;

	m_fFogAdjustFarClip = AgcmEventNature::SKY_RADIUS;

	// 마고자 (2005-04-22 오전 11:40:52) : 
	m_fNearValue		= 20000.0f;
	m_fNormalValue		= 48000.0f;

	m_bSkyFogDisable	= TRUE;

	//add by dobal {
	m_pThis = this;
	m_bLostDevice = true;
	//}
}

AgcmEventNature::~AgcmEventNature()
{

}

// Virtual 펑션들
BOOL	AgcmEventNature::OnAddModule	()
{
	//add by dobal
	OldD3D9ReleaseDeviceCB = _rwD3D9DeviceGetReleaseCallback();
	OldD3D9RestoreDeviceCB = _rwD3D9DeviceGetRestoreCallback();
	_rwD3D9DeviceSetReleaseCallback( NewD3D9ReleaseDeviceCB );
	_rwD3D9DeviceSetRestoreCallback( NewD3D9RestoreDeviceCB );
	// } dobal

	AgcmCharacter*	pAgcmCharacter = (AgcmCharacter*)GetModule("AgcmCharacter");
			
	ASSERT( NULL != pAgcmCharacter && "케릭터 모듈을먼저 삽입하세요." );

	if (!pAgcmCharacter->SetCallbackSelfUpdatePosition(OnMyCharacterPositionChange, this))
	{
		ASSERT( !"AgcmEventNature::OnAddModule 콜백 지정실패" );
		return FALSE;
	}

	if (!pAgcmCharacter->SetCallbackSetSelfCharacter( CBOnSetSelfCharacter, this))
	{
		ASSERT( !"AgcmEventNature::OnAddModule 콜백 지정실패" );
		return FALSE;
	}

	// 포인터 미리 저장해둠..
	m_pcsAgcmEff2			= ( AgcmEff2 * ) GetModule( "AgcmEff2" );

	m_pcsAgpmEventNature	= ( AgpmEventNature * ) GetModule( "AgpmEventNature" );
	ASSERT( NULL != m_pcsAgpmEventNature && "AgpmEventNature 모듈을먼저 삽입하세요." );
	
	m_pcsAgcmRender			= ( AgcmRender * ) GetModule( "AgcmRender" );
	ASSERT( NULL != m_pcsAgcmRender && "AgcmRender 모듈을먼저 삽입하세요." );

	m_pcsApmMap				= ( ApmMap * ) GetModule( "ApmMap" );
	ASSERT( NULL != m_pcsApmMap && "ApmMap 모듈을먼저 삽입하세요." );

	m_pcsAgcmMap				= ( AgcmMap * ) GetModule( "AgcmMap" );
	ASSERT( NULL != m_pcsAgcmMap && "AgcmMap 모듈을먼저 삽입하세요." );

	m_pcsApmEventManager	= ( ApmEventManager * ) GetModule( "ApmEventManager" );
	ASSERT( NULL != m_pcsApmEventManager && "ApmEventManager 모듈을먼저 삽입하세요." );

	// ASSERT( NULL != m_pcsAgcmEff2 && "AgcmEff2 모듈을먼저 삽입하세요." );

	m_pcsApmObject			= ( ApmObject * ) GetModule( "ApmObject" );
	ASSERT( NULL != m_pcsApmObject && "ApmObject 모듈을먼저 삽입하세요." );

	m_pcsAgcmSound			= ( AgcmSound * ) GetModule( "AgcmSound" );
	ASSERT( NULL != m_pcsAgcmSound && "AgcmSound 모듈을먼저 삽입하세요." );

	m_pcsAgpmTimer			= ( AgpmTimer * ) GetModule( "AgpmTimer" );
	ASSERT( NULL != m_pcsAgpmTimer && "AgpmTimer 모듈을먼저 삽입하세요." );
	
	m_pcsAgcmNatureEffect			= ( AgcmNatureEffect * ) GetModule( "AgcmNatureEffect" );
	ASSERT( NULL != m_pcsAgcmNatureEffect && "AgcmNatureEffect 모듈을먼저 삽입하세요." );

//	// 마고자 (2004-03-04 오후 4:54:23) : 이벤트 텔레포트 모듈..
//	m_pcsAgpmEventTeleport	= ( AgpmEventTeleport * ) GetModule( "AgpmEventTeleport" );
//	if( m_pcsAgpmEventTeleport )
//	{
//		m_pcsAgpmEventTeleport->SetCallbackReceivedTeleportStart( CBOnTeleportStart , this );
//	}
	
	m_pcsAgcmResourceLoader	= ( AgcmResourceLoader * ) GetModule( "AgcmResourceLoader" );
	
	if( m_pcsAgpmEventNature )
	{
		m_nSkySetAttachIndex = m_pcsAgpmEventNature->AttachSkySetData( 
			this , 
			sizeof AgcdSkySet,
			AgcmEventNature::OnSkySetCreate , 
			AgcmEventNature::OnSkySetDestroy );
		if (!m_pcsAgpmEventNature->AddStreamCallback(
			AGPMEVENTNATURE_SKY_DATA_STREAM, SkySetStreamReadCB, SkySetStreamWriteCB, this) ) return FALSE;
		if (!m_pcsAgpmEventNature->SetCallbackNature(CBNature, this)) return FALSE;
	}

	if( m_pcsAgcmMap )
	{
		if (!m_pcsAgcmMap->SetCallbackLoadMap(CBLoadSector, this))
			return FALSE;
		if (!m_pcsAgcmMap->SetCallbackSetAutoLoadChange(CBOnSetAutoLoadChange, this))
			return FALSE;
	}

	if( m_pcsAgcmRender )
	{
		//VERIFY( m_pcsAgcmRender->SetCallbackPostRender( __PreRenderCallback , ( PVOID ) this ) );
		VERIFY( m_pcsAgcmRender->SetCallbackPreRender( __PreRenderCallback , ( PVOID ) this ) );
	}

	if(m_pcsAgcmNatureEffect )
	{
		VERIFY( m_pcsAgcmNatureEffect->SetCallbackEffectStart( CBOnNatureEffectStart , ( PVOID ) this ) );
		VERIFY( m_pcsAgcmNatureEffect->SetCallbackEffectEnd( CBOnNatureEffectEnd , ( PVOID ) this ) );
	}

	ApmObject *	pcsApmObject	= ( ApmObject * ) GetModule( "ApmObject" );
	ASSERT( NULL != pcsApmObject && "ApmObject 모듈을먼저 삽입하세요." );
	if( pcsApmObject )
	{
		// Add Object Callback 등록.
		pcsApmObject->SetCallbackAddObject		( CBOnAddObject		, this );
		pcsApmObject->SetCallbackRemoveObject	( CBOnRemoveObject	, this );
	}

	if( m_pcsAgpmTimer )
	{
		VERIFY( m_pcsAgpmTimer->SetCallbackReceiveTime(CBReceiveTime, this));
	}

	// 마고자 (2004-06-08 오후 6:49:41) : 
	GenerateStarVertex( 500 , 600 , 500 , 50 );
	SetMaptoolDebugInfoFlag( SKY_DONOTCHANGEEFFECT | SKY_DONOTCHANGEBGM );

	return TRUE;
}

BOOL	AgcmEventNature::OnInit			()
{
	Init( GetWorld() , GetAmbientLight(), GetDirectionalLight(), GetCamera() );

	ShowSky( TRUE );

	UINT64	time = m_pcsAgpmEventNature->GetGameTime();
	SetTime( time );
	SkyChange( GetCurrentSkySetting() );
	MoveSky();

	AS_REGISTER_TYPE_BEGIN( AgcmEventNature , AgcmEventNature );
		AS_REGISTER_VARIABLE( float , m_fNearValue		);
		AS_REGISTER_VARIABLE( float , m_fNormalValue	);
		AS_REGISTER_VARIABLE( int	, m_bSkyFogDisable	);
	AS_REGISTER_TYPE_END;

	for( int i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
	{
		m_paEffectPtrAttay [ i ].pEffect = NULL;
	}


	/*
	RwRGBA	rgb;

	RwRGBASet(	&rgb	, 255 , 255 , 255 , 255 );
	ModifyAtomic( &m_aAtomicSun[ FORE_TEXTURE ]	,  1, &rgb	);
	m_aAtomicSun[ FORE_TEXTURE ].SetVisible( TRUE );
	ModifyAtomic( &m_aAtomicSun[ BACK_TEXTURE ]	,  1, &rgb	);
	*/

	return TRUE;
}

BOOL	AgcmEventNature::OnDestroy		()
{
	CleanAllData();
	DestroyStar();

	if( m_prwTextureDictionary )
	{
		RwTexDictionaryDestroy( m_prwTextureDictionary );
		m_prwTextureDictionary	= NULL;
	}

	for( int i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
	{
		if( m_paEffectPtrAttay [ i ].pEffect != NULL )
		{
			if (m_pcsAgcmEff2)
			{
				m_paEffectPtrAttay [ i ].pEffect->End();
				m_paEffectPtrAttay [ i ].pEffect= NULL;
			}
		}
	}

	return TRUE;
}

LuaGlue	LG_SetSkyTemplateID( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	// 스카이셋을 지정.

	INT32	nTemplateID	= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );
	INT32	nFlag		= ( INT32 ) pLua->GetNumberArgument( 2 , ( double ) ASkySetting::DA_ALL );
	AgcmEventNature::m_pThis->SetSkyTemplateID( nTemplateID , nFlag );

	LUA_RETURN( TRUE );
}

LuaGlue	LG_SetNextWeather( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	//	SetNextWeather( eCurrentTimeSet , __uSkyChangeGap , FALSE );
	// 시간대를 설정함.

	INT32	nTimeSet	= ( INT32 ) pLua->GetNumberArgument( 1 , 0 );
	INT32	nDuration	= ( INT32 ) pLua->GetNumberArgument( 2 , 10000 );

	if( nTimeSet < ASkySetting::TS_NIGHT2	) nTimeSet = ASkySetting::TS_NIGHT2;
	if( nTimeSet >= ASkySetting::TS_MAX		) nTimeSet = ASkySetting::TS_MAX - 1;

	AgcmEventNature::m_pThis->SetNextWeather( (ASkySetting::TimeSet) nTimeSet , nDuration );

	LUA_RETURN( TRUE );
}

LuaGlue	LG_ApplySkySetting( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	// 대기중인 스카이셋을 그냥 적용시켜버림

	AgcmEventNature::m_pThis->ApplySkySetting();

	LUA_RETURN( TRUE )
}

LuaGlue	LG_EndSkySet( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	// 스카이셋을 원래대로 돌려놓음.
	RwV3d	vPos = AgcmEventNature::m_pThis->GetLastPosition();
	AuPOS	pos;
	pos.x = vPos.x;
	pos.y = vPos.y;
	pos.z = vPos.z;

	AgcmEventNature::m_pThis->SetCharacterPosition( pos );
	AgcmEventNature::m_pThis->SetTimeForce( AgcmEventNature::m_pThis->GetTime() );
	AgcmEventNature::m_pThis->ApplySkySetting();

	return 0;
}

void	AgcmEventNature::OnLuaInitialize( AuLua * pLua )
{
	luaDef MVLuaGlue[] = 
	{
		{"EndSkySet"			,	LG_EndSkySet			},
		{"ApplySkySetting"		,	LG_ApplySkySetting		},
		{"SetNextWeather"		,	LG_SetNextWeather		},
		{"SetSkyTemplateID"		,	LG_SetSkyTemplateID		},
		{NULL					,	NULL					},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
}

BOOL	AgcmEventNature::OnIdle			(UINT32 ulClockCount)
{
	// 현재 시간을 기준으로 다음 시간으로 넘어갈 준비를 함..
	PROFILE("AgcmEventNature::OnIdle");

	if( IsShowSky() )
	{
		// 태양이랑 구름의 위치를 변경시킨다..
		UINT64		currenttime = m_pcsAgpmEventNature->GetGameTime();
		
		VERIFY( SetTime( currenttime ) );
	}

	// 오브젝트 인덱스 업데이트.
	ApWorldSector * pSector;
	if( m_listUpdateSectorNatureInfo.GetHeadNode() )
	{
		// 뮤텍스 지정..
		m_MutexlistUpdateSectorNatureInfo.Lock		();

		while( m_listUpdateSectorNatureInfo.GetHeadNode() )
		{
			pSector = m_listUpdateSectorNatureInfo.GetHeadNode()->GetData();

			UpdateSectorNatureInfo( pSector );

			m_listUpdateSectorNatureInfo.RemoveHead();
		}

		m_MutexlistUpdateSectorNatureInfo.Unlock	();
	}

	// 스카이 인덱스 업데이트..
	if( m_listUpdateSectorNature.GetHeadNode() )
	{
		SectorNatureUpdateInfo	*pUpdateInfo;
		// 뮤텍스 지정..
		m_MutexlistUpdateSectorNatureInfo.Lock		();

		while( m_listUpdateSectorNature.GetHeadNode() )
		{
			pUpdateInfo = &m_listUpdateSectorNature.GetHeadNode()->GetData();

			pSector = m_pcsApmMap->GetSector( pUpdateInfo->uSectorX , pUpdateInfo->uSectorZ );

			if( pSector )
			{
                UpdateSectorNatureObject( pSector , pUpdateInfo->uObjectID );
			}

			m_listUpdateSectorNature.RemoveHead();
		}

		m_MutexlistUpdateSectorNatureInfo.Unlock	();
	}


	return TRUE;
}

BOOL		AgcmEventNature::Init				( RpLight * pAmbient , RpLight * pDirectional , RwCamera * pCamera )
{
	// 포인터 설정..
	ASSERT( NULL != pAmbient		);
	ASSERT( NULL != pDirectional	);
	ASSERT( NULL != pCamera			);

	if( NULL == pAmbient || NULL == pDirectional || NULL == pCamera ) return FALSE;

	// 텍스쳐 딕셔너리 생성..
	// 가지고 있다고 텍스쳐 리드할때만 사용함.
	VERIFY( m_prwTextureDictionary	= RwTexDictionaryCreate() );


	// 포인터 카피.
	m_pLightAmbient		= pAmbient		;
	m_pLightDirectional	= pDirectional	;
	m_pCamera			= pCamera		;

	// 스카이 스크립트 파일을 로드한다.
	// 로드에 실패할경우 함수 내부에서 디폴트 값으로 초기화를 하게 된다.

//	LoadTextureScriptFile		(); BOB님 주석(130803, 외부에서 한다)
	// 스크립트 로딩하면서 텍스쳐도 로딩한다..

	// 아토믹 생성..
	VERIFY( CreateSkyAtomic	( &m_AtomicSky		) );
//	RwTexture * pTexture;
//	pTexture = RwTextureRead( "SKY00000.png" , NULL );
//	m_AtomicSky.SetTexture( pTexture );
//	RwTextureDestroy( pTexture );

	INT32	i;

	for( i = 0 ; i < CLOUD_ATOMIC_LAYER_MAX ; ++ i )
	{
		VERIFY( CreateCloudAtomic( &m_aAtomicCloud[ i ]	) );
	}
	for( i = 0 ; i < MAX_CIRCUMSTANCE_TEXTURE ; ++ i )
	{
		VERIFY( CreateCircumstanceAtomic( &m_aAtomicCircustance[ i ]	) );
	}
	for( i = 0 ; i < MAX_CIRCUMSTANCE_TEXTURE ; ++ i )
	{
		VERIFY( CreateSunAtomic( &m_aAtomicSun[ i ]	) );
	}

	// Far Clip Plane과 Sky Width 를 계산해냄..
	AgcmMap	* pCmMap = ( AgcmMap * ) GetModule( "AgcmMap" );
	ASSERT( NULL != pCmMap );

	INT32	nMapLoadRadius = pCmMap->GetRoughLoadRadius();

	SetSkyRadius( nMapLoadRadius ) ;

	m_bInit	= TRUE;

	SetSkyCircleRadius();

	return TRUE;
}

BOOL		AgcmEventNature::ShowSky			( BOOL	bShow		)
{
	// 월드에 에드돼어 있는
	// 하늘/해/달/구름 을 월드에서 넣거나 빼는 코드를 삽입한다.
	m_bShowSky	= bShow;

	return TRUE;
}

BOOL		AgcmEventNature::LoadTextureScriptFile		( CHAR	* szPath, BOOL bDecryption	)
{
	// 스카이 스크립트 파일을 읽어 분석한다.
	// AuIniFile 을 이용한다.

	// 우선 데이타 초기화..
	// 디폴트 데이타로 일단 사용을 한다.

	// 디폴트 텍스쳐 등록..


	// 읽어들인다..
	AuIniManagerA	iniManager;

//	iniManager.SetPath( AGCMEVENTNATURE_SCRIPT_FILE_NAME ); BOB님 수정(130803, szPath로 대체)
	iniManager.SetPath(	szPath	);
	if( iniManager.ReadFile(0, bDecryption) )
	{
		ASSERT( 0 == m_listTexture.GetCount() );

		// 읽자!..
		// 텍스쳐 추가..
		AuNode< ASkyTexture >	* pNode				= m_listTexture.GetHeadNode();
		//ASkyTexture				* pSkyTextureInfo	;
		//char					strKey[ 1024 ]		;

		int						nSectionID			;
		int						nKeyCount			;
		int						nIndex				;

		nSectionID	= iniManager.FindSection( ( char * ) g_strINITextureSection	);
		nKeyCount	= iniManager.GetNumKeys	( nSectionID				);

		// 테긋쳐 추가함..

		for( int i = 0 ; i < nKeyCount ; ++i  )
		{
			nIndex	= atoi( iniManager.GetKeyName( nSectionID , i ) );
			AddTexture( iniManager.GetValue( nSectionID , i ) , nIndex );
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL		AgcmEventNature::SaveTextureScriptFile( CHAR	* szPath, BOOL bEncryption	)
{
	AuIniManagerA	iniManager;

	// INI Format
	// [Texture]
	// 00 = aaaaaa.png
	// 01 = bbbbbb.png
	// 02 = cccccc.png
	// ....

//	iniManager.SetPath( AGCMEVENTNATURE_SCRIPT_FILE_NAME ); BOB님 수정(130803, szPath로 대체)
	iniManager.SetPath( szPath );

	// 텍스쳐 추가..
	AuNode< ASkyTexture >	* pNode				= m_listTexture.GetHeadNode();
	ASkyTexture				* pSkyTextureInfo	;
	char					strKey[ 1024 ]			;
	while( pNode )
	{
		pSkyTextureInfo	= &pNode->GetData();

		wsprintf( strKey , g_strINITextureKey , pSkyTextureInfo->nIndex );

		iniManager.SetValue( ( char * ) g_strINITextureSection , strKey , pSkyTextureInfo->strName );

		m_listTexture.GetNext( pNode );
	}

	iniManager.WriteFile(0, bEncryption);

	return TRUE;
}

BOOL	AgcmEventNature::CreateSkyAtomic	( MagImAtomic * pAtomic )
{
	ASSERT( NULL != pAtomic );
	
	RwIm3DVertex			*	pVertex		;
	RwImVertexIndex			*	pIndex		;

    //WORD i, j;

	VERIFY( pAtomic->CreateAtomic( NUM_SPHERE_VERTICES , NUM_SPHERE_TRIANGLES * 3 ) );

#ifdef _DEBUG
	char	fname[ 256 ];
	GetCurrentDirectory( 256 , fname );
#endif
	
	VERIFY( pVertex	= pAtomic->GetVertexBuffer	() );
    VERIFY( pIndex	= pAtomic->GetIndexBuffer	() );
	
	memset( pVertex	, 0 , sizeof(	RwIm3DVertex		) * NUM_SPHERE_VERTICES			);
	memset( pIndex	, 0 , sizeof(	RwImVertexIndex		) * NUM_SPHERE_TRIANGLES * 3 	);
	pAtomic->SetTexture( NULL , -1 );

	// Generate the sphere data
	
	FLOAT dHori	= 2 * _PI	/ ( FLOAT ) ( SPHERE_HORI_MESH_SIZE - 1 )	;
    FLOAT dVert	= _PI		/ ( FLOAT ) ( SPHERE_VERT_MESH_SIZE - 1 )	;
	FLOAT dU	= 1.0f		/ ( FLOAT ) ( SPHERE_HORI_MESH_SIZE - 1 )	;
	FLOAT dV	= 1.0f		/ ( FLOAT ) ( SPHERE_VERT_MESH_SIZE - 1 )	;  

	// Vertex 할당..
    
	RwV3d   p;
	int		i,j;

	for( j = 0 ; j < SPHERE_VERT_MESH_SIZE ; ++ j )
	{
		for( i = 0 ; i < SPHERE_HORI_MESH_SIZE ; ++i  )
		{
            p.y = ( FLOAT ) ( cos( ( FLOAT ) j * dVert	)								);
            p.x = ( FLOAT ) ( sin( ( FLOAT ) i * dHori	) * sin( ( FLOAT ) j * dVert )	);
            p.z = ( FLOAT ) ( cos( ( FLOAT ) i * dHori	) * sin( ( FLOAT ) j * dVert )	);

			pVertex[ GETSPHEREINDEX( i , j ) ].objVertex.x	=	p.x	;
			pVertex[ GETSPHEREINDEX( i , j ) ].objVertex.y	=	p.y	;
			pVertex[ GETSPHEREINDEX( i , j ) ].objVertex.z	=	p.z	;

			pVertex[ GETSPHEREINDEX( i , j ) ].objNormal.x	=	-p.x;
			pVertex[ GETSPHEREINDEX( i , j ) ].objNormal.y	=	-p.y;
			pVertex[ GETSPHEREINDEX( i , j ) ].objNormal.z	=	-p.z;

			pVertex[ GETSPHEREINDEX( i , j ) ].u			=	1 - ( FLOAT ) i * dU;
			pVertex[ GETSPHEREINDEX( i , j ) ].v			=	( FLOAT ) j * dV	;

			pVertex[ GETSPHEREINDEX( i , j ) ].color		=	0xffffffff;
		}
	}

    // Now generate the traingle indices. Strip around north pole first
    for( i = 0 ; i < SPHERE_HORI_MESH_SIZE - 1 ; ++ i )
    {
		pIndex[ i * 3 + 0 ] = GETSPHEREINDEX( i		, 0 )	;
		pIndex[ i * 3 + 1 ] = GETSPHEREINDEX( i		, 1 )	;
		pIndex[ i * 3 + 2 ] = GETSPHEREINDEX( i + 1	, 1 )	;
    }

	// 가운데거 그림..
	int	trioffset;
	for( j = 0 ; j < ( SPHERE_VERT_MESH_SIZE - 3 ) ; ++ j )
	{
		// 옵셋 정해둠..
		trioffset = ( SPHERE_HORI_MESH_SIZE - 1 ) + ( SPHERE_HORI_MESH_SIZE - 1 ) * j * 2	;

		for( i = 0 ; i < ( SPHERE_HORI_MESH_SIZE - 1 ) ; ++ i )
		{
			pIndex[ ( trioffset + i * 2 + 0 ) * 3 + 0 ]	= GETSPHEREINDEX( i		, j	+ 1 )	;
			pIndex[ ( trioffset + i * 2 + 0 ) * 3 + 1 ]	= GETSPHEREINDEX( i		, j + 2 )	;
			pIndex[ ( trioffset + i * 2 + 0 ) * 3 + 2 ]	= GETSPHEREINDEX( i + 1	, j + 2	)	;

			pIndex[ ( trioffset + i * 2 + 1 ) * 3 + 0 ]	= GETSPHEREINDEX( i		, j	+ 1	)	;
			pIndex[ ( trioffset + i * 2 + 1 ) * 3 + 1 ]	= GETSPHEREINDEX( i	+ 1	, j + 2 )	;
			pIndex[ ( trioffset + i * 2 + 1 ) * 3 + 2 ]	= GETSPHEREINDEX( i + 1	, j	+ 1	)	;
		}
	}

    // Finally strip around south pole
	trioffset	=	( SPHERE_HORI_MESH_SIZE - 1 ) + 
					( SPHERE_HORI_MESH_SIZE - 1 ) * ( SPHERE_VERT_MESH_SIZE - 3 ) * 2	;
	
    for( i = 0 ; i < SPHERE_HORI_MESH_SIZE - 1 ; ++ i )
    {
		pIndex[ ( trioffset + i ) * 3 + 0 ] = GETSPHEREINDEX( i		, SPHERE_VERT_MESH_SIZE - 2	)	;
		pIndex[ ( trioffset + i ) * 3 + 1 ] = GETSPHEREINDEX( i		, SPHERE_VERT_MESH_SIZE - 1	)	;
		pIndex[ ( trioffset + i ) * 3 + 2 ] = GETSPHEREINDEX( i + 1	, SPHERE_VERT_MESH_SIZE - 2	)	;
    }

	ASSERT( trioffset + i == NUM_SPHERE_TRIANGLES );
	
	return TRUE;
}

BOOL		AgcmEventNature::ModifySky		( MagImAtomic * pAtomic		, RwRGBA * pRgbTop		, RwRGBA * pRgbBottom )
{
	// 스카이 칼라를 바꾼다..
	// 버택스에 정보만 교체하는 것임둥.
	ASSERT( NULL != pAtomic		);
	ASSERT( NULL != pRgbTop		);
	ASSERT( NULL != pRgbBottom	);

	#ifdef _DEBUG
	if( NULL == pAtomic || NULL == pRgbTop || NULL == pRgbBottom )
		return FALSE;
	#endif

//	// 버택스는 9개지!..
//	ASSERT( 9 == pAtomic->GetVertexCount() );
//
//	#ifdef _DEBUG
//	if( 9 != pAtomic->GetVertexCount() ) return FALSE;
//	#endif
//
//	RwIm3DVertex			*	pVertex		;
//	VERIFY( pVertex	= pAtomic->GetVertexBuffer	() );
//
//	RwRGBA	rgbCenter;
//	rgbCenter.red	=	( pRgbTop->red		+	pRgbBottom->red		)	/ 2.0f;
//	rgbCenter.green	=	( pRgbTop->green	+	pRgbBottom->green	)	/ 2.0f;
//	rgbCenter.blue	=	( pRgbTop->blue		+	pRgbBottom->blue	)	/ 2.0f;
//	rgbCenter.alpha	=	( pRgbTop->alpha	+	pRgbBottom->alpha	)	/ 2.0f;
//
//	RwIm3DVertexSetRGBA	( &pVertex[ 0 ] , pRgbTop->red , pRgbTop->green , pRgbTop->blue , pRgbTop->alpha				);
//	
//	RwIm3DVertexSetRGBA	( &pVertex[ 1 ] , rgbCenter.red , rgbCenter.green , rgbCenter.blue , rgbCenter.alpha	);
//	RwIm3DVertexSetRGBA	( &pVertex[ 2 ] , rgbCenter.red , rgbCenter.green , rgbCenter.blue , rgbCenter.alpha	);
//	RwIm3DVertexSetRGBA	( &pVertex[ 3 ] , rgbCenter.red , rgbCenter.green , rgbCenter.blue , rgbCenter.alpha	);
//	RwIm3DVertexSetRGBA	( &pVertex[ 4 ] , rgbCenter.red , rgbCenter.green , rgbCenter.blue , rgbCenter.alpha	);
//
//	RwIm3DVertexSetRGBA	( &pVertex[ 5 ] , pRgbBottom->red , pRgbBottom->green , pRgbBottom->blue , pRgbBottom->alpha	);
//	RwIm3DVertexSetRGBA	( &pVertex[ 6 ] , pRgbBottom->red , pRgbBottom->green , pRgbBottom->blue , pRgbBottom->alpha	);
//	RwIm3DVertexSetRGBA	( &pVertex[ 7 ] , pRgbBottom->red , pRgbBottom->green , pRgbBottom->blue , pRgbBottom->alpha	);
//	RwIm3DVertexSetRGBA	( &pVertex[ 8 ] , pRgbBottom->red , pRgbBottom->green , pRgbBottom->blue , pRgbBottom->alpha	);

	FLOAT	fRate;
	RwRGBA	rgbCenter;
	RwIm3DVertex			*	pVertex		;
	VERIFY( pVertex	= pAtomic->GetVertexBuffer	() );

	for( int i = 0 ; i < pAtomic->GetVertexCount() ; ++ i )
	{
		fRate			= ( pVertex[ i ].objVertex.y > 0.0f ) ? pVertex[ i ].objVertex.y : 0.0f;

		// Crop
		fRate			= fRate > 1.0f ? 1.0f : fRate;

		rgbCenter.red	=	( RwUInt8 ) ( pRgbTop->red		* fRate		+	pRgbBottom->red		* ( 1.0f - fRate ) );
		rgbCenter.green	=	( RwUInt8 ) ( pRgbTop->green	* fRate		+	pRgbBottom->green	* ( 1.0f - fRate ) );
		rgbCenter.blue	=	( RwUInt8 ) ( pRgbTop->blue		* fRate		+	pRgbBottom->blue	* ( 1.0f - fRate ) );
		rgbCenter.alpha	=	( RwUInt8 ) ( pRgbTop->alpha	* fRate		+	pRgbBottom->alpha	* ( 1.0f - fRate ) );

		RwIm3DVertexSetRGBA	( &pVertex[ i ] , rgbCenter.red , rgbCenter.green , rgbCenter.blue , rgbCenter.alpha	);
	}

	return TRUE;
}

BOOL		AgcmEventNature::CreateCloudAtomic	( MagImAtomic * pAtomic , FLOAT fCloudUVDetail )
{
	// 마고자 (2003-07-08 오후 4:39:42) : 구름 아토믹 생성 루틴을 여기에..
	ASSERT( NULL != pAtomic );
	
	RwIm3DVertex			*	pVertex		;
	RwImVertexIndex			*	pIndex		;

    WORD i, j;

	int	nVertex		= ( CLOUD_DETAIL + 1 ) * ( CLOUD_DETAIL + 1 )	;
	int nTriangle	= CLOUD_DETAIL * CLOUD_DETAIL * 2				;

	pAtomic->DestroyAtomic();
	VERIFY( pAtomic->CreateAtomic( nVertex	, nTriangle * 3 ) );

#ifdef _DEBUG
	char	fname[ 256 ];
	GetCurrentDirectory( 256 , fname );
#endif
	
	VERIFY( pVertex	= pAtomic->GetVertexBuffer	() );
    VERIFY( pIndex	= pAtomic->GetIndexBuffer	() );
	
	memset( pVertex	, 0 , sizeof(	RwIm3DVertex		) * nVertex			);
	memset( pIndex	, 0 , sizeof(	RwImVertexIndex		) * nTriangle * 3 	);

	pAtomic->SetTexture( NULL , -1 );

	FLOAT	fx , fz;
	FLOAT	fCloudRate = 2.0f;

	// Vertex 세팅..
	for( j = 0 ; j < CLOUD_DETAIL + 1 ; ++ j )
	{
		for( i = 0 ; i < CLOUD_DETAIL + 1 ; ++ i )
		{
			fx	= ( ( FLOAT ) i ) / ( ( FLOAT ) CLOUD_DETAIL ) * fCloudRate - 1.0f;
			fz	= ( ( FLOAT ) j ) / ( ( FLOAT ) CLOUD_DETAIL ) * fCloudRate - 1.0f;

			RwIm3DVertexSetPos	( &pVertex[ i + j * ( CLOUD_DETAIL + 1 ) ] , fx , 1.0f , fz			); 
			RwIm3DVertexSetU	( &pVertex[ i + j * ( CLOUD_DETAIL + 1 ) ] , ( ( FLOAT ) i ) / ( ( FLOAT ) CLOUD_DETAIL ) * fCloudUVDetail );    
			RwIm3DVertexSetV	( &pVertex[ i + j * ( CLOUD_DETAIL + 1 ) ] , ( ( FLOAT ) j ) / ( ( FLOAT ) CLOUD_DETAIL ) * fCloudUVDetail );
			RwIm3DVertexSetRGBA	( &pVertex[ i + j * ( CLOUD_DETAIL + 1 ) ] , 255 , 255 , 255 , 255	);
		}
	}

	// Index 세팅..
	for( j = 0 ; j < CLOUD_DETAIL ; ++ j )
	{
		for( i = 0 ; i < CLOUD_DETAIL ; ++ i )
		{
			pIndex[ ( i + j * CLOUD_DETAIL ) * 3 * 2 + 0 ]	= ( i + 0 ) + ( j + 0 ) * ( CLOUD_DETAIL + 1 );
			pIndex[ ( i + j * CLOUD_DETAIL ) * 3 * 2 + 1 ]	= ( i + 0 ) + ( j + 1 ) * ( CLOUD_DETAIL + 1 );
			pIndex[ ( i + j * CLOUD_DETAIL ) * 3 * 2 + 2 ]	= ( i + 1 ) + ( j + 1 ) * ( CLOUD_DETAIL + 1 );

			pIndex[ ( i + j * CLOUD_DETAIL ) * 3 * 2 + 3 ]	= ( i + 1 ) + ( j + 1 ) * ( CLOUD_DETAIL + 1 );
			pIndex[ ( i + j * CLOUD_DETAIL ) * 3 * 2 + 4 ]	= ( i + 1 ) + ( j + 0 ) * ( CLOUD_DETAIL + 1 );
			pIndex[ ( i + j * CLOUD_DETAIL ) * 3 * 2 + 5 ]	= ( i + 0 ) + ( j + 0 ) * ( CLOUD_DETAIL + 1 );
		}
	}

	return TRUE;
}

BOOL		AgcmEventNature::CreateCircumstanceAtomic	( MagImAtomic * pAtomic )
{
	// 마고자 (2003-07-08 오후 4:39:42) : 구름 아토믹 생성 루틴을 여기에..
	ASSERT( NULL != pAtomic );
	
	RwIm3DVertex			*	pVertex		;
	RwImVertexIndex			*	pIndex		;

	int	nVertex		= ( CIRCUMSTANCEATOMICDETAIL	+ 1 ) * 2	;
	int nTriangle	= CIRCUMSTANCEATOMICDETAIL * 2				;

	VERIFY( pAtomic->CreateAtomic( nVertex	, nTriangle * 3 ) );

	VERIFY( pVertex	= pAtomic->GetVertexBuffer	() );
    VERIFY( pIndex	= pAtomic->GetIndexBuffer	() );
	
	memset( pVertex	, 0 , sizeof(	RwIm3DVertex		) * nVertex			);
	memset( pIndex	, 0 , sizeof(	RwImVertexIndex		) * nTriangle * 3 	);

	pAtomic->SetTexture( NULL , -1 );
	
	
	FLOAT	fRadian;
	int		i;

	for( i = 0 ; i < CIRCUMSTANCEATOMICDETAIL + 1  ; ++ i )
	{
		fRadian	= 3.1415927f * 2 * ( FLOAT ) i / ( FLOAT ) CIRCUMSTANCEATOMICDETAIL;

		// 돌면서 설정..
		// 마지막 녀석 예외처리..
		
		// 위에거..
		RwIm3DVertexSetPos	( &pVertex[ i ] , sin( fRadian ) , 1.5f , cos( fRadian ) ); 
		RwIm3DVertexSetU	( &pVertex[ i ] , ( FLOAT ) i * CIRCUMSTANCEATOMICUVDEPTH / ( FLOAT ) CIRCUMSTANCEATOMICDETAIL	);
		RwIm3DVertexSetV	( &pVertex[ i ] , 0.0f					);
		RwIm3DVertexSetRGBA	( &pVertex[ i ] , 255 , 255 , 255 , 255	);

		// 아래거..
		RwIm3DVertexSetPos	( &pVertex[ i + CIRCUMSTANCEATOMICDETAIL + 1 ] , sin( fRadian ) , -0.5f , cos( fRadian )	); 
		RwIm3DVertexSetU	( &pVertex[ i + CIRCUMSTANCEATOMICDETAIL + 1 ] , ( FLOAT ) i * CIRCUMSTANCEATOMICUVDEPTH / ( FLOAT ) CIRCUMSTANCEATOMICDETAIL	);    
		RwIm3DVertexSetV	( &pVertex[ i + CIRCUMSTANCEATOMICDETAIL + 1 ] , 1.0f					);
		RwIm3DVertexSetRGBA	( &pVertex[ i + CIRCUMSTANCEATOMICDETAIL + 1 ] , 255 , 255 , 255 , 255	);
	}

	// 인덱스 설정.
	for( i = 0 ; i < CIRCUMSTANCEATOMICDETAIL ; ++ i )
	{
		pIndex[ ( i ) * 6	+ 0 ]	= i		;
		pIndex[ ( i ) * 6	+ 1 ]	= i + 1	;
		pIndex[ ( i ) * 6	+ 2 ]	= ( CIRCUMSTANCEATOMICDETAIL + 1 ) + i + 1	;

		pIndex[ ( i ) * 6	+ 3 ]	= i		;
		pIndex[ ( i ) * 6	+ 4 ]	= ( CIRCUMSTANCEATOMICDETAIL + 1 ) + i + 1	;
		pIndex[ ( i ) * 6	+ 5 ]	= ( CIRCUMSTANCEATOMICDETAIL + 1 ) + i		;
	}

	return TRUE;
}

BOOL		AgcmEventNature::CreateSunAtomic				( MagImAtomic * pAtomic	)
{
	// 마고자 (2003-07-08 오후 4:39:42) : 구름 아토믹 생성 루틴을 여기에..
	ASSERT( NULL != pAtomic );
	
	RwIm3DVertex			*	pVertex		;
	RwImVertexIndex			*	pIndex		;

	int	nVertex		= 4	;
	int nTriangle	= 2 ;

	VERIFY( pAtomic->CreateAtomic( nVertex	, nTriangle * 3 ) );

	VERIFY( pVertex	= pAtomic->GetVertexBuffer	() );
    VERIFY( pIndex	= pAtomic->GetIndexBuffer	() );
	
	memset( pVertex	, 0 , sizeof(	RwIm3DVertex		) * nVertex			);
	memset( pIndex	, 0 , sizeof(	RwImVertexIndex		) * nTriangle * 3 	);

	pAtomic->SetTexture( NULL , -1 );
	
	FLOAT	fRadius = 0.3f;

	int i = 0;

	RwIm3DVertexSetPos	( &pVertex[ i ] , -1.0f , -fRadius , -fRadius ); 
	RwIm3DVertexSetU	( &pVertex[ i ] , 0.0f	);
	RwIm3DVertexSetV	( &pVertex[ i ] , 0.0f					);
	RwIm3DVertexSetRGBA	( &pVertex[ i ] , 255 , 255 , 255 , 255	);

	i++;

	RwIm3DVertexSetPos	( &pVertex[ i ] , -1.0f , +fRadius , -fRadius ); 
	RwIm3DVertexSetU	( &pVertex[ i ] , 1.0f	);
	RwIm3DVertexSetV	( &pVertex[ i ] , 0.0f					);
	RwIm3DVertexSetRGBA	( &pVertex[ i ] , 255 , 255 , 255 , 255	);

	i++;

	RwIm3DVertexSetPos	( &pVertex[ i ] , -1.0f , +fRadius , +fRadius ); 
	RwIm3DVertexSetU	( &pVertex[ i ] , 1.0f	);
	RwIm3DVertexSetV	( &pVertex[ i ] , 1.0f					);
	RwIm3DVertexSetRGBA	( &pVertex[ i ] , 255 , 255 , 255 , 255	);

	i++;

	RwIm3DVertexSetPos	( &pVertex[ i ] , -1.0f , -fRadius , +fRadius ); 
	RwIm3DVertexSetU	( &pVertex[ i ] , 0.0f	);
	RwIm3DVertexSetV	( &pVertex[ i ] , 1.0f					);
	RwIm3DVertexSetRGBA	( &pVertex[ i ] , 255 , 255 , 255 , 255	);

	i++;

	pIndex[ 0 ] = 0;
	pIndex[ 1 ] = 1;
	pIndex[ 2 ] = 3;
	pIndex[ 3 ] = 3;
	pIndex[ 4 ] = 1;
	pIndex[ 5 ] = 2;

	return TRUE;
}

BOOL	AgcmEventNature::ChangeSunSize	( FLOAT fRadius )
{
	for( int nSun = 0 ; nSun <MAX_CIRCUMSTANCE_TEXTURE ; nSun ++ )
	{
		RwIm3DVertex			*	pVertex		;
		RwImVertexIndex			*	pIndex		;

		int	nVertex		= 4	;
		int nTriangle	= 2 ;

		VERIFY( pVertex	= m_aAtomicSun[ nSun ].GetVertexBuffer	() );
		VERIFY( pIndex	= m_aAtomicSun[ nSun ].GetIndexBuffer	() );
		
		int i = 0;

		RwIm3DVertexSetPos	( &pVertex[ i ] , -1.0f , -fRadius , -fRadius ); 
		i++;

		RwIm3DVertexSetPos	( &pVertex[ i ] , -1.0f , +fRadius , -fRadius ); 

		i++;

		RwIm3DVertexSetPos	( &pVertex[ i ] , -1.0f , +fRadius , +fRadius ); 

		i++;

		RwIm3DVertexSetPos	( &pVertex[ i ] , -1.0f , -fRadius , +fRadius ); 
		i++;
	}

	return TRUE;
}



RtWorldImport *	AgcmEventNature::_CreateHalfSphere()	// 반구 만들기..
{

	RtWorldImport			*	pWorldImport;
	RtWorldImportTriangle	*	pTriangles	;
	RtWorldImportVertex		*	pVertices	;
	RwTexture				*	pTexture	=	NULL;
	RpMaterial				*	pMaterial	;
	RwInt32						matIndex	;

    WORD i, j;

	pWorldImport = RtWorldImportCreate();
	if( pWorldImport == NULL )
    {
        return NULL;
    }

	RtWorldImportAddNumVertices		( pWorldImport, NUM_SPHERE_VERTICES		);
    RtWorldImportAddNumTriangles	( pWorldImport, NUM_SPHERE_TRIANGLES	);

#ifdef _DEBUG
	char	fname[ 256 ];
	GetCurrentDirectory( 256 , fname );
#endif
	
    pMaterial	= RpMaterialCreate();
//    pMaterial2	= RpMaterialCreate();

    RpMaterialSetTexture( pMaterial , pTexture );

    matIndex	= RtWorldImportAddMaterial	( pWorldImport , pMaterial	);
//    matIndex2	= RtWorldImportAddMaterial	( pWorldImport , pMaterial2	);

	pVertices	= RtWorldImportGetVertices	( pWorldImport				);
    pTriangles	= RtWorldImportGetTriangles	( pWorldImport				);
	
	memset( pVertices	, 0 , sizeof(	RtWorldImportVertex		) * NUM_SPHERE_VERTICES		);
	memset( pTriangles	, 0 , sizeof(	RtWorldImportTriangle	) * NUM_SPHERE_TRIANGLES	);

    // Seed the random number generator

	// Generate the sphere data
	
	FLOAT dHori	= 2 * _PI	/ ( FLOAT ) ( SPHERE_HORI_MESH_SIZE - 1 )	;
    FLOAT dVert	= _PI		/ ( FLOAT ) ( SPHERE_VERT_MESH_SIZE - 1 )	;
	FLOAT dU	= 1.0f		/ ( FLOAT ) ( SPHERE_HORI_MESH_SIZE - 1 )	;
	FLOAT dV	= 1.0f		/ ( FLOAT ) ( SPHERE_VERT_MESH_SIZE - 1 )	;  

	// Vertex 할당..
    
	RwV3d   p;

	for( j = 0 ; j < SPHERE_VERT_MESH_SIZE ; ++ j )
	{
		for( i = 0 ; i < SPHERE_HORI_MESH_SIZE ; ++i  )
		{
            p.y = ( FLOAT ) ( cos( ( FLOAT ) j * dVert	)								);
            p.x = ( FLOAT ) ( sin( ( FLOAT ) i * dHori	) * sin( ( FLOAT ) j * dVert )	);
            p.z = ( FLOAT ) ( cos( ( FLOAT ) i * dHori	) * sin( ( FLOAT ) j * dVert )	);

			_SetVertex(
				GETSPHEREVERT( pVertices , i , j ) ,
				p.x , p.y , p.z , 
				-p.x , -p.y , -p.z ,
				1 - ( FLOAT ) i * dU	,
				( FLOAT ) j * dV	);
		}
	}

    // Now generate the traingle indices. Strip around north pole first
    for( i = 0 ; i < SPHERE_HORI_MESH_SIZE - 1 ; ++ i )
    {
		pTriangles[ i ].vertIndex[ 0 ] = GETSPHEREINDEX( i		, 0 )	;
		pTriangles[ i ].vertIndex[ 1 ] = GETSPHEREINDEX( i		, 1 )	;
		pTriangles[ i ].vertIndex[ 2 ] = GETSPHEREINDEX( i + 1	, 1 )	;
		
		pTriangles[ i ].matIndex		= matIndex						;
    }

	// 가운데거 그림..
	int	trioffset;
	for( j = 0 ; j < ( SPHERE_VERT_MESH_SIZE - 3 ) ; ++ j )
	{
		// 옵셋 정해둠..
		trioffset = ( SPHERE_HORI_MESH_SIZE - 1 ) + ( SPHERE_HORI_MESH_SIZE - 1 ) * j * 2	;

		for( i = 0 ; i < ( SPHERE_HORI_MESH_SIZE - 1 ) ; ++ i )
		{
			pTriangles[ trioffset + i * 2 + 0 ].vertIndex[ 0 ]	= GETSPHEREINDEX( i		, j	+ 1 )	;
			pTriangles[ trioffset + i * 2 + 0 ].vertIndex[ 1 ]	= GETSPHEREINDEX( i		, j + 2 )	;
			pTriangles[ trioffset + i * 2 + 0 ].vertIndex[ 2 ]	= GETSPHEREINDEX( i + 1	, j + 2	)	;

			pTriangles[ trioffset + i * 2 + 0 ].matIndex		= matIndex							;

			pTriangles[ trioffset + i * 2 + 1 ].vertIndex[ 0 ]	= GETSPHEREINDEX( i		, j	+ 1	)	;
			pTriangles[ trioffset + i * 2 + 1 ].vertIndex[ 1 ]	= GETSPHEREINDEX( i	+ 1	, j + 2 )	;
			pTriangles[ trioffset + i * 2 + 1 ].vertIndex[ 2 ]	= GETSPHEREINDEX( i + 1	, j	+ 1	)	;

			pTriangles[ trioffset + i * 2 + 1 ].matIndex		= matIndex							;
		}
	}

    // Finally strip around south pole
	trioffset	=	( SPHERE_HORI_MESH_SIZE - 1 ) + 
					( SPHERE_HORI_MESH_SIZE - 1 ) * ( SPHERE_VERT_MESH_SIZE - 3 ) * 2	;
	
    for( i = 0 ; i < SPHERE_HORI_MESH_SIZE - 1 ; ++ i )
    {
		pTriangles[ trioffset + i ].vertIndex[ 0 ] = GETSPHEREINDEX( i		, SPHERE_VERT_MESH_SIZE - 2	)	;
		pTriangles[ trioffset + i ].vertIndex[ 1 ] = GETSPHEREINDEX( i		, SPHERE_VERT_MESH_SIZE - 1	)	;
		pTriangles[ trioffset + i ].vertIndex[ 2 ] = GETSPHEREINDEX( i + 1	, SPHERE_VERT_MESH_SIZE - 2	)	;
		
		pTriangles[ trioffset + i ].matIndex		= matIndex						;
    }

	ASSERT( trioffset + i == NUM_SPHERE_TRIANGLES );

    if( pTexture ) 
	{
		RwTextureDestroy	( pTexture	);
		pTexture = NULL;
	}

    RpMaterialDestroy( pMaterial	);
	pMaterial = NULL;
	
	return pWorldImport;

}

void		AgcmEventNature::CleanAllData		()	// 읽어들였던 데이타 초기화..
{
	// 하늘제거..

	m_AtomicSky	.DestroyAtomic();
	for( int i = 0 ; i < CLOUD_ATOMIC_LAYER_MAX ; ++ i )
	{
		m_aAtomicCloud			[ i ] .DestroyAtomic();
	}

	for( int i = 0 ; i < MAX_CIRCUMSTANCE_TEXTURE ; ++ i )
	{
		m_aAtomicCircustance	[ i ] .DestroyAtomic();
		m_aAtomicSun			[ i ] .DestroyAtomic();
	}

	RemoveAllTexture();

	// 기타 데이타 초기화..
	
	ShowSky( FALSE );
}

INT32	AgcmEventNature::GetDefaultSky	( FLOAT fX , FLOAT fZ )
{
	int nDefaultSky = 0;
	int nRegion = m_pcsApmMap->GetRegion( fX , fZ );

	ApmMap::RegionTemplate * pTemplate = m_pcsApmMap->GetTemplate( nRegion );
	if( pTemplate )
	{
		nDefaultSky = pTemplate->nSkyIndex;
	}
	else
	{
		switch( nRegion )
		{
		case 0	:	nDefaultSky = 13	;break;
		case 1	:	nDefaultSky = 37	;break;
		case 2	:	nDefaultSky = 0		;break;
		case 3	:	nDefaultSky = 9		;break;
		case 4	:	nDefaultSky = 39	;break;
		case 5	:	nDefaultSky = 14	;break;
		case 6	:	nDefaultSky = 11	;break;
		case 7	:	nDefaultSky = 1		;break;
		case 8	:	nDefaultSky = 14	;break;
		case 9	:	nDefaultSky = 13	;break;
		case 10	:	nDefaultSky = 0		;break;
		case 11	:	nDefaultSky = 41	;break;
		case 12	:	nDefaultSky = 41	;break;
		case 13	:	nDefaultSky = 42	;break;
		case 14	:	nDefaultSky = 14	;break;
		case 15	:	nDefaultSky = 44	;break;
		case 16	:	nDefaultSky = 44 	;break;
		case 17	:	nDefaultSky = 44	;break;
		case 18	:	nDefaultSky = 44	;break;
		case 19	:	nDefaultSky = 44	;break;
		case 20	:	nDefaultSky = 44	;break;
		case 21	:	nDefaultSky = 44	;break;
		case 22 :	nDefaultSky = 6		;break;
		case 23 :	nDefaultSky = 44	;break;
		case 24 :	nDefaultSky = 46	;break;

		// 마고자 (2005-05-12 오후 2:07:59) : 
		case 25 :	nDefaultSky = 56	;break;	// 뱀파이어던전 - 던젼_공용던젼_01(폰)
		case 26 :	nDefaultSky = 57	;break; // 노린 - 문엘프(타운)노린
		case 27 :	nDefaultSky = 51	;break; // 바르도터 - 문엘프_바르도터
		case 28 :	nDefaultSky = 52	;break; // 고르곱 - 문엘프(타운)고르곱

		default:
			nDefaultSky = 13;
		}
	}

	return nDefaultSky;
}	

BOOL		AgcmEventNature::SetCharacterPosition			( AuPOS	pos , BOOL bForce )
{
	ASSERT( NULL != m_pcsApmMap );

	BOOL	bChanged = FALSE;
	// 월드를 모두 이 좌표기준으로 트랜스폼을 함..

	m_posLastPosition.x = pos.x;
	m_posLastPosition.y = pos.y;
	m_posLastPosition.z = pos.z;

	MoveSky();
	
	// 마고자 (2003-07-06 오후 10:45:52) : 
	// 여기서 기상변화를 체크해야함..
	// 현재 타일에 스카이 템플릿을 얻어서
	// 현재 타일과 비교 , 다를경우
	// 스카이 변화를 호출함..


	#ifdef USE_MFC
	// 마고자 (2004-06-09 오후 6:36:15) : 맵툴에선 타일방식 안쓴다.
		AuNode< INT32 > * pNode = m_listNatureObject.GetHeadNode();
		ApdObject *					pcsObject	;
		ApdEvent *	pstEvent	;

		INT32	nValue , nTemplateID ;
		FLOAT	fRadius;
		FLOAT	fDistance;

		INT32 nArray[ SECTOR_MAX_NATURE_OBJECT_COUNT ];
		for( int i = 0 ; i < SECTOR_MAX_NATURE_OBJECT_COUNT ; i ++ )
		{
			nArray[ i ] = 0xffff;
		}
		
		while( pNode )
		{
			nValue = pNode->GetData();
			pcsObject	= m_pcsApmObject->GetObject( nValue );
			if( pcsObject )
			{
				// EventNature Event Struct 얻어냄..
				pstEvent	= m_pcsApmEventManager->GetEvent( pcsObject , APDEVENT_FUNCTION_NATURE );

				if( pstEvent )
				{
					// 여기서 범위검사..

					nTemplateID = ( INT32 ) pstEvent->m_pvData;
					// 반지름..
					fRadius		= pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius;

					AgpdSkySet *	pPublicSkySet	;
					AgcdSkySet *	pClientSkySet	;
					VERIFY( pPublicSkySet = m_pcsAgpmEventNature->GetSkySet( nTemplateID ) );
					VERIFY( pClientSkySet = GetSkySetClientData( pPublicSkySet ) );
					ASkySetting	* pSkySet = pClientSkySet->m_aSkySetting;

					// 범위체크
					if( ( fDistance = ( FLOAT )
						sqrt(	( pos.x - pcsObject->m_stPosition.x ) * ( pos.x - pcsObject->m_stPosition.x ) + 
								( pos.z - pcsObject->m_stPosition.z ) * ( pos.z - pcsObject->m_stPosition.z ) ) )
						< fRadius
						)
					{
						INT32 nSkyFlag = pSkySet->nDataAvailableFlag;

						if( nSkyFlag & ASkySetting::DA_LIGHT )
						{
							if( __GetSkyTemplateIndexFromMapData	( nArray[ 0 ] ) != nTemplateID	&&
								__GetSkyTemplateDistanceFromMapData	( nArray[ 0 ] ) > fDistance		)
							{
								nArray [ 0 ] = __MakeSkyTemplateIndexForMapData( nTemplateID , fDistance );
							}
						}
						if( nSkyFlag & ASkySetting::DA_FOG )
						{
							if( __GetSkyTemplateIndexFromMapData	( nArray[ 1 ] ) != nTemplateID	&&
								__GetSkyTemplateDistanceFromMapData	( nArray[ 1 ] ) > fDistance		)
							{
								nArray [ 1 ] = __MakeSkyTemplateIndexForMapData( nTemplateID , fDistance );
							}
						}
						if( nSkyFlag & ASkySetting::DA_CLOUD )
						{
							if( __GetSkyTemplateIndexFromMapData	( nArray[ 2 ] ) != nTemplateID	&&
								__GetSkyTemplateDistanceFromMapData	( nArray[ 2 ] ) > fDistance		)
							{
								nArray [ 2 ] = __MakeSkyTemplateIndexForMapData( nTemplateID , fDistance );
							}
						}
						if( nSkyFlag & ASkySetting::DA_EFFECT )
						{
							if( __GetSkyTemplateIndexFromMapData	( nArray[ 3 ] ) != nTemplateID	&&
								__GetSkyTemplateDistanceFromMapData	( nArray[ 3 ] ) > fDistance		)
							{
								nArray [ 3 ] = __MakeSkyTemplateIndexForMapData( nTemplateID , fDistance );
							}
						}
						if( nSkyFlag & ASkySetting::DA_MUSIC )
						{
							if( __GetSkyTemplateIndexFromMapData	( nArray[ 4 ] ) != nTemplateID	&&
								__GetSkyTemplateDistanceFromMapData	( nArray[ 4 ] ) > fDistance		)
							{
								nArray [ 4 ] = __MakeSkyTemplateIndexForMapData( nTemplateID , fDistance );
							}
						}
						if( nSkyFlag & ASkySetting::DA_WEATHER )
						{
							if( __GetSkyTemplateIndexFromMapData	( nArray[ 5 ] ) != nTemplateID	&&
								__GetSkyTemplateDistanceFromMapData	( nArray[ 5 ] ) > fDistance		)
							{
								nArray [ 5 ] = __MakeSkyTemplateIndexForMapData( nTemplateID , fDistance );
							}
						}
					}


				}
			}

			pNode = pNode->GetNextNode();
		}

		if( __GetSkyTemplateIndexFromMapData( nArray[ 0 ] ) == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 0 ] ) &&
			__GetSkyTemplateIndexFromMapData( nArray[ 1 ] ) == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 1 ] ) &&
			__GetSkyTemplateIndexFromMapData( nArray[ 2 ] ) == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 2 ] ) &&
			__GetSkyTemplateIndexFromMapData( nArray[ 3 ] ) == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 3 ] ) &&
			__GetSkyTemplateIndexFromMapData( nArray[ 4 ] ) == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 4 ] ) &&
			__GetSkyTemplateIndexFromMapData( nArray[ 5 ] ) == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 5 ] ) )
		{
			// do nothing..
		}
		else
		{
			if(	__GetSkyTemplateIndexFromMapData( nArray[ 0 ] ) != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 0 ] ) )
			{
				if( SetSkyTemplateID( __GetSkyTemplateIndexFromMapData( nArray [ 0 ] ) , ASkySetting::DA_LIGHT ) )
				{
					m_skyMixedUpIndex [ 0 ] = nArray[ 0 ];
					bChanged = TRUE;
				}
			}
			if(	__GetSkyTemplateIndexFromMapData( nArray[ 1 ] ) != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 1 ] ) )
			{
				if( SetSkyTemplateID( __GetSkyTemplateIndexFromMapData( nArray [ 1 ] ) , ASkySetting::DA_FOG ) )
				{
					m_skyMixedUpIndex [ 1 ] = nArray[ 1 ];
					bChanged = TRUE;
				}
			}
			if(	__GetSkyTemplateIndexFromMapData( nArray[ 2 ] ) != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 2 ] ) )
			{
				if( SetSkyTemplateID( __GetSkyTemplateIndexFromMapData( nArray [ 2 ] ) , ASkySetting::DA_CLOUD ) )
				{
					m_skyMixedUpIndex [ 2 ] = nArray[ 2 ];
					bChanged = TRUE;
				}
			}
			if(	__GetSkyTemplateIndexFromMapData( nArray[ 3 ] ) != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 3 ] ) )
			{
				if( SetSkyTemplateID( __GetSkyTemplateIndexFromMapData( nArray [ 3 ] ) , ASkySetting::DA_EFFECT ) )
				{
					m_skyMixedUpIndex [ 3 ] = nArray[ 3 ];
					bChanged = TRUE;
				}
			}
			if(	__GetSkyTemplateIndexFromMapData( nArray[ 4 ] ) != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 4 ] ) )
			{
				if( SetSkyTemplateID( __GetSkyTemplateIndexFromMapData( nArray [ 4 ] ) , ASkySetting::DA_MUSIC ) )
				{
					m_skyMixedUpIndex [ 4 ] = nArray[ 4 ];
					bChanged = TRUE;
				}
			}
			if(	__GetSkyTemplateIndexFromMapData( nArray[ 5 ] ) != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 5] ) )
			{
				if( SetSkyTemplateID( __GetSkyTemplateIndexFromMapData( nArray [ 5 ] ) , ASkySetting::DA_WEATHER ) )
				{
					m_skyMixedUpIndex [ 5 ] = nArray[ 5 ];
					bChanged = TRUE;
				}
			}
			
			INT32		nHour		= m_pcsAgpmEventNature->GetHour( m_nCurrentTime );
			ASkySetting::TimeSet eTimeSet = ASkySetting::GetTimeSet( nHour );

			SetNextWeather( eTimeSet , __uSkyChangeGap , bForce ); //&pSkySet->m_aSkySetting[ nHour ] );
			bChanged = TRUE;
		}	
	#else
		/// 마고자 (2004-06-09 오후 6:36:26) : 여기서 클라이언트.
		ApWorldSector	*	pSector			= NULL							;

		pSector	= m_pcsApmMap->GetSector( pos.x , pos.z );

		if( NULL == pSector )
		{
			//PROFILE("AgcmEventNature::SetCharacterPosition() - LoadSector");

			pSector =	m_pcsAgcmMap->LoadSector( 
						PosToSectorIndexX( pos.x )			,
						0										,
						PosToSectorIndexZ( pos.z )			);
			// Callback으로 OnLoadSector 호출됨..
		}
		
		if( NULL == pSector )
		{
			// 섹터가 없네..
			// return FALSE;

			return TRUE; // 일단 임시로..
		}
		else
		{
			int	nSegmentX , nSegmentZ;
			INT32	nCount , aArray[ SECTOR_MAX_NATURE_OBJECT_COUNT ];

			pSector->D_GetSegment( SECTOR_HIGHDETAIL , pos.x , pos.z , &nSegmentX , &nSegmentZ );
			nCount = pSector->GetIndex( ApWorldSector::AWS_SKYOBJECT , nSegmentX , nSegmentZ , aArray , SECTOR_MAX_NATURE_OBJECT_COUNT );

			if( nCount == SECTOR_MAX_NATURE_OBJECT_COUNT )
			{
				INT32 nSkyTemplateIndex0 = __GetSkyTemplateIndexFromMapData( aArray[ 0 ] );
				INT32 nSkyTemplateIndex1 = __GetSkyTemplateIndexFromMapData( aArray[ 1 ] );
				INT32 nSkyTemplateIndex2 = __GetSkyTemplateIndexFromMapData( aArray[ 2 ] );
				INT32 nSkyTemplateIndex3 = __GetSkyTemplateIndexFromMapData( aArray[ 3 ] );
				INT32 nSkyTemplateIndex4 = __GetSkyTemplateIndexFromMapData( aArray[ 4 ] );
				INT32 nSkyTemplateIndex5 = __GetSkyTemplateIndexFromMapData( aArray[ 5 ] );

				INT32 nMixTemplateIndex0 = __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex[ 0 ] );
				INT32 nMixTemplateIndex1 = __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex[ 1 ] );
				INT32 nMixTemplateIndex2 = __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex[ 2 ] );
				INT32 nMixTemplateIndex3 = __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex[ 3 ] );
				INT32 nMixTemplateIndex4 = __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex[ 4 ] );
				INT32 nMixTemplateIndex5 = __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex[ 5 ] );

				if( nSkyTemplateIndex0 == nMixTemplateIndex0 &&	nSkyTemplateIndex1 == nMixTemplateIndex1 &&
					nSkyTemplateIndex2 == nMixTemplateIndex2 &&	nSkyTemplateIndex3 == nMixTemplateIndex3 &&
					nSkyTemplateIndex4 == nMixTemplateIndex4 &&	nSkyTemplateIndex5 == nMixTemplateIndex5 )
				{
					// do nothing..
				}
				else
				{
					if(	nSkyTemplateIndex0 != nMixTemplateIndex0 )
					{
						if( SetSkyTemplateID( nSkyTemplateIndex0, ASkySetting::DA_LIGHT ) )
						{
							m_skyMixedUpIndex [ 0 ] = aArray[ 0 ];
							bChanged = TRUE;
						}
					}
					if(	nSkyTemplateIndex1 != nMixTemplateIndex1 )
					{
						if( SetSkyTemplateID( nSkyTemplateIndex1 , ASkySetting::DA_FOG ) )
						{
							m_skyMixedUpIndex [ 1 ] = aArray[ 1 ];
							bChanged = TRUE;
						}
					}
					if(	nSkyTemplateIndex2 != nMixTemplateIndex2 )
					{
						if( SetSkyTemplateID( nSkyTemplateIndex2 , ASkySetting::DA_CLOUD ) )
						{
							m_skyMixedUpIndex [ 2 ] = aArray[ 2 ];
							bChanged = TRUE;
						}
					}
					if(	nSkyTemplateIndex3 != nMixTemplateIndex3 )
					{
						if( SetSkyTemplateID( nSkyTemplateIndex3 , ASkySetting::DA_EFFECT ) )
						{
							m_skyMixedUpIndex [ 3 ] = aArray[ 3 ];
							bChanged = TRUE;
						}
					}
					if(	nSkyTemplateIndex4 != nMixTemplateIndex4 )
					{
						if( SetSkyTemplateID( nSkyTemplateIndex4 , ASkySetting::DA_MUSIC ) )
						{
							m_skyMixedUpIndex [ 4 ] = aArray[ 4 ];
							bChanged = TRUE;
						}
					}
					if(	nSkyTemplateIndex5 != nMixTemplateIndex5 )
					{
						if( SetSkyTemplateID( nSkyTemplateIndex5 , ASkySetting::DA_WEATHER ) )
						{
							m_skyMixedUpIndex [ 5 ] = aArray[ 5 ];
							bChanged = TRUE;
						}
					}
					
					INT32		nHour		= m_pcsAgpmEventNature->GetHour( m_nCurrentTime );
					ASkySetting::TimeSet eTimeSet = ASkySetting::GetTimeSet( nHour );

					SetNextWeather( eTimeSet , __uSkyChangeGap , bForce ); //&pSkySet->m_aSkySetting[ nHour ] );
					bChanged = TRUE;
				}
			}
			else
			{
				// 마고자 (2004-07-21 오전 11:51:08) : 디폴트 스카이 설정 문제!
				// 현재 상황에서 트릴가드쪽 메인 스카이를 설정할수가 없으므로..
				// 일단은 하드 코딩한다.
				// 후에 스카이의 프라이어리티 ( 또는 디비젼단위 스카이 ) 가 적용이 돼면
				// 삭제될 예정.
				int nDefaultSky = GetDefaultSky( pos.x , pos.z );

				// - -?...
				// 디폴트인 0 을 삽입함..
				if( nDefaultSky == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 0 ] ) &&
					nDefaultSky == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 1 ] ) &&
					nDefaultSky == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 2 ] ) &&
					nDefaultSky == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 3 ] ) &&
					nDefaultSky == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 4 ] ) &&
					nDefaultSky == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 5 ] ) )
				{
					// do nothing..
				}
				else
				{
					if(	nDefaultSky != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 0 ] ) )
					{
						if( SetSkyTemplateID( nDefaultSky , ASkySetting::DA_LIGHT ) )
						{
							m_skyMixedUpIndex [ 0 ] = __MakeSkyTemplateIndexForMapData( nDefaultSky , 0.0f );
							bChanged = TRUE;
						}
					}
					if(	nDefaultSky != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 1 ] ) )
					{
						if( SetSkyTemplateID( nDefaultSky  , ASkySetting::DA_FOG ) )
						{
							m_skyMixedUpIndex [ 1 ] = __MakeSkyTemplateIndexForMapData( nDefaultSky , 0.0f );
							bChanged = TRUE;
						}
					}
					if(	nDefaultSky != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 2 ] ) )
					{
						if( SetSkyTemplateID( nDefaultSky , ASkySetting::DA_CLOUD ) )
						{
							m_skyMixedUpIndex [ 2 ] = __MakeSkyTemplateIndexForMapData( nDefaultSky , 0.0f );
							bChanged = TRUE;
						}
					}
					if(	nDefaultSky != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 3 ] ) )
					{
						if( SetSkyTemplateID( nDefaultSky , ASkySetting::DA_EFFECT ) )
						{
							m_skyMixedUpIndex [ 3 ] = __MakeSkyTemplateIndexForMapData( nDefaultSky , 0.0f );
							bChanged = TRUE;
						}
					}
					if(	nDefaultSky != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 4 ] ) )
					{
						if( SetSkyTemplateID( nDefaultSky , ASkySetting::DA_MUSIC ) )
						{
							m_skyMixedUpIndex [ 4 ] = __MakeSkyTemplateIndexForMapData( nDefaultSky , 0.0f );
							bChanged = TRUE;
						}
					}
					if(	nDefaultSky != __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 5 ] ) )
					{
						if( SetSkyTemplateID( nDefaultSky , ASkySetting::DA_WEATHER ) )
						{
							m_skyMixedUpIndex [ 5 ] = __MakeSkyTemplateIndexForMapData( nDefaultSky , 0.0f );
							bChanged = TRUE;
						}
					}
					
					INT32		nHour		= m_pcsAgpmEventNature->GetHour( m_nCurrentTime );
					ASkySetting::TimeSet eTimeSet = ASkySetting::GetTimeSet( nHour );

					SetNextWeather( eTimeSet , __uSkyChangeGap , bForce ); //&pSkySet->m_aSkySetting[ nHour ] );
					bChanged = TRUE;
				}

			}

		}
	#endif

	return bChanged;
}

void		AgcmEventNature::MoveSky				()
{
	#ifdef USE_MFC
	if( GetMaptoolDebugInfoFlag() & SKY_DONOTMOVE ) return;
	#endif

	// 실제 트렌스포메이션을 한다..
	RwFrame		*	pFrame		;
	RwV3d			scale		;
	RwMatrix*		pModelling	;

	scale.x		=	AgcmEventNature::SKY_RADIUS	;
	scale.y		=	AgcmEventNature::SKY_HEIGHT	;
	scale.z		=	AgcmEventNature::SKY_RADIUS	;

	LockFrame();

	if( m_AtomicSky.IsAvailable() )
	{
		// First Atomic 부터 수정함..
		pFrame	= m_AtomicSky.GetFrame();

		ASSERT( NULL != pFrame );
		if( NULL == pFrame )
		{
			UnlockFrame();
			return;
		}

		pModelling = RwFrameGetMatrix( pFrame );
		RwMatrixScale		( pModelling , &scale , rwCOMBINEREPLACE		);
		RwMatrixTranslate	( pModelling , &m_posLastPosition , rwCOMBINEPOSTCONCAT	);
		RwFrameUpdateObjects( pFrame );
	}

	for( int i = 0 ; i < CLOUD_ATOMIC_LAYER_MAX ; ++i )
	{
		if( m_aAtomicCloud[ i ].IsAvailable() )
		{
			pFrame	= m_aAtomicCloud[ i ].GetFrame();

			ASSERT( NULL != pFrame );
			if( NULL == pFrame )
			{
				UnlockFrame();
				return;
			}

			pModelling = RwFrameGetMatrix( pFrame );
			RwMatrixScale		( pModelling , &scale	, rwCOMBINEREPLACE		);
			RwMatrixTranslate	( pModelling , &m_posLastPosition	, rwCOMBINEPOSTCONCAT	);
			RwFrameUpdateObjects( pFrame );
		}
	}

	{
		//FLOAT	fRate	=		14.0f / 24.0f;
		FLOAT	fRate	= __fsSunAngle / 24.0f;
		FLOAT	fAngle	= 90.0f + 360.0f * fRate;

		scale.x		=	AgcmEventNature::SKY_RADIUS	* 0.95f;
		scale.y		=	AgcmEventNature::SKY_RADIUS	* 0.95f;
		scale.z		=	AgcmEventNature::SKY_RADIUS	* 0.95f;

		for( int i = 0 ; i < MAX_CIRCUMSTANCE_TEXTURE ; ++i )
		{
			if( m_aAtomicSun[ i ].IsAvailable() )
			{
				pFrame	= m_aAtomicSun[ i ].GetFrame();

				ASSERT( NULL != pFrame );
				if( NULL == pFrame )
				{
					UnlockFrame();
					return;
				}

				RwMatrix	* pModelling	= RwFrameGetMatrix( pFrame );
				RwV3d		* pAtVector		= RwMatrixGetAt		( pModelling	);

				RwMatrixSetIdentity(pModelling);

				RwMatrixRotate( pModelling	, & yaxis	, 50		,	rwCOMBINEREPLACE	);
				RwMatrixRotate( pModelling	, & zaxis	, fAngle	,	rwCOMBINEPOSTCONCAT	);

				RwMatrixScale		( pModelling , &scale	, rwCOMBINEPOSTCONCAT		);
				RwMatrixTranslate	( pModelling , &m_posLastPosition	, rwCOMBINEPOSTCONCAT	);
				RwFrameUpdateObjects( pFrame );
			}
		}
	}

	scale.x	= AgcmEventNature::CIRCUMSTANCE_RADIUS;
	scale.y	= AgcmEventNature::CIRCUMSTANCE_HEIGHT;
	scale.z	= AgcmEventNature::CIRCUMSTANCE_RADIUS;
	
	VERIFY( pFrame	=	m_aAtomicCircustance[ FORE_TEXTURE ].GetFrame() );
	if( pFrame )
	{
		pModelling = RwFrameGetMatrix( pFrame );
		RwMatrixScale		( pModelling , &scale		, rwCOMBINEREPLACE		);
		RwMatrixTranslate	( pModelling , &m_posLastPosition	, rwCOMBINEPOSTCONCAT	);
		RwFrameUpdateObjects( pFrame );
	}

	VERIFY( pFrame	=	m_aAtomicCircustance[ BACK_TEXTURE ].GetFrame() );
	if( pFrame )
	{
		pModelling = RwFrameGetMatrix( pFrame );
		RwMatrixScale		( pModelling , &scale		, rwCOMBINEREPLACE		);
		RwMatrixTranslate	( pModelling , &m_posLastPosition	, rwCOMBINEPOSTCONCAT	);
		RwFrameUpdateObjects( pFrame );
	}

	// 마고자 (2004-06-09 오후 12:10:53) : 스타 프레임..
	if( m_pStarFrame )
	{
		scale.x	= AgcmEventNature::SKY_RADIUS;
		scale.y	= AgcmEventNature::SKY_RADIUS;
		scale.z	= AgcmEventNature::SKY_RADIUS;
		pFrame = m_pStarFrame;
		pModelling = RwFrameGetMatrix( pFrame );
		RwMatrixScale		( pModelling , &scale		, rwCOMBINEREPLACE		);
		RwMatrixTranslate	( pModelling , &m_posLastPosition	, rwCOMBINEPOSTCONCAT	);
		RwFrameUpdateObjects( pFrame );
	}

	UpdateSunPosition();

	UnlockFrame();
}

// Callback
BOOL	AgcmEventNature::OnMyCharacterPositionChange( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmEventNature::OnMyCharacterPositionChange");

	AgcmEventNature * pThis = ( AgcmEventNature * ) pClass;

	ASSERT( NULL != pThis );
	ASSERT( NULL != pData );
	if( NULL == pThis || NULL == pData) return FALSE;

	// 케릭터 데이타..
	AgpdCharacter	* pcsAgpdCharacter = (AgpdCharacter *) pData;

	pThis->SetCharacterPosition( pcsAgpdCharacter->m_stPos );

	return TRUE;
}

INT32		AgcmEventNature::AddTexture			( char *	pFilename , INT32	nIndex	)
{
	ASSERT( 0 <= nIndex );
	ASSERT( NULL != pFilename );
	if( NULL == pFilename ) return -1;	// ERROR
	ASSERT( strlen( pFilename ) < 256 );
	if( strlen( pFilename ) >= 256 ) return -1; // ERROR

	// 중복 여부 검사
	AuNode< ASkyTexture >	* pNode = m_listTexture.GetHeadNode();
	ASkyTexture		* pSkyTextureInfo;
	while( pNode )
	{
		pSkyTextureInfo	= &pNode->GetData();
		if( pSkyTextureInfo->nIndex	== nIndex ) return -1; // ERROR

		m_listTexture.GetNext( pNode );
	}

	// 무조건 뒤에다 넣어요.

	ASkyTexture		skyTexture;
	// 데이타 카피..
	skyTexture.nIndex	= nIndex;
	strncpy( skyTexture.strName	, pFilename , 256 );

	m_listTexture.AddTail( skyTexture );

	return nIndex;
}

RwTexture *	AgcmEventNature::GetTexture			( INT32		nIndex		)
{
	if( nIndex == -1 ) return NULL;

	AuNode< ASkyTexture >	*	pNode			= m_listTexture.GetHeadNode();
	ASkyTexture				*	pSkyTextureInfo	;
	RwTexture				*	pTexture		= NULL;
	RwTexDictionary			*	pPrevTextureDictionary	= RwTexDictionaryGetCurrent();

	if(!pNode)
		return NULL;

	while( pNode )
	{
		pSkyTextureInfo	= &pNode->GetData();
		if( pSkyTextureInfo->nIndex	== nIndex )
		{
			if( m_pcsAgcmResourceLoader )
			{
				pTexture = m_pcsAgcmResourceLoader->LoadTexture( pSkyTextureInfo->strName, NULL );
			}

			if( pTexture )
			{
				RwTextureSetFilterMode	( pTexture, rwFILTERLINEAR			);
				RwTextureSetAddressing	( pTexture, rwTEXTUREADDRESSWRAP	);
			}

			return pTexture;
		}

		m_listTexture.GetNext( pNode );
	}

	// 더미 텍스쳐 로딩..

	if( m_pcsAgcmResourceLoader )
		pTexture = m_pcsAgcmResourceLoader->LoadTexture( pSkyTextureInfo->strName, NULL );
	else
		pTexture	=	RwTextureRead	( RWSTRING ( "sky.png" ), NULL	);

	if( pTexture ) RwTextureSetFilterMode	( pTexture, rwFILTERLINEAR );

	return pTexture;
}

BOOL		AgcmEventNature::RemoveTexture		( INT32		nIndex		)
{
	AuNode< ASkyTexture >	* pNode = m_listTexture.GetHeadNode();
	ASkyTexture		* pSkyTextureInfo;
	while( pNode )
	{
		pSkyTextureInfo	= &pNode->GetData();
		if( pSkyTextureInfo->nIndex == nIndex )
		{
			// 발견..

			m_listTexture.RemoveNode( pNode );
			return TRUE;
		}

		m_listTexture.GetNext( pNode );
	}

	return FALSE;
}

INT32		AgcmEventNature::GetTextureIndex	( char *	pFilename	)
{
	AuNode< ASkyTexture >	* pNode = m_listTexture.GetHeadNode();
	ASkyTexture		* pSkyTextureInfo;
	while( pNode )
	{
		pSkyTextureInfo	= &pNode->GetData();
		if( !strncmp( pSkyTextureInfo->strName , pFilename , 256 ) == 0 )
		{
			return pSkyTextureInfo->nIndex; // ERROR
		}

		m_listTexture.GetNext( pNode );
	}

	return -1;

}

void		AgcmEventNature::RemoveAllTexture	(						)
{
	TRACE( "AgcmEventNature::RemoveAllTexture 텍스쳐 제거..\n" );
	
	AuNode< ASkyTexture >	*	pNode			;
	ASkyTexture				*	pSkyTextureInfo	;
	
	while( pNode = m_listTexture.GetHeadNode() )
	{
		pSkyTextureInfo	= &pNode->GetData();

		m_listTexture.RemoveHead();
	}

}

BOOL		AgcmEventNature::SetTime				( UINT64	time				)
{
	PROFILE("AgcmEventNature::SetTime");

	// 마고자 (2004-06-09 오후 3:46:32) : 별 반작임~
	BlinkStar();

	// 배경 처리~
	PlayBGM();

	// 이펙트는 우선처리.

	// Effect 처리.. 이건 틱카운트..
	static DWORD s_LastPlayTime		= GetTickCount();

	static DWORD	s_LastTickCount	= GetTickCount();
	DWORD			uCurrentTick	= GetTickCount();
	DWORD			uTickDelta		= uCurrentTick - s_LastTickCount;
	s_LastTickCount = uCurrentTick;

	// 구름 처리.. 이건 게임타임..
	static UINT64 s__LastTime = time;
	UINT32	nDeltaTime	= ( UINT32 ) ( time - s__LastTime );
	s__LastTime = time;

	#ifdef USE_MFC
	if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGEEFFECT ) )
	#endif
	{
		// 이펙트 얻어내기..
		INT32	nEffect = m_skyCurrentSetting[ CURRENT_SKY ].GetTemporaryEffect();
		int i = 0;

		//// 끝난 이펙트 체크..
		//int i;
		//for(i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
		//{
		//	if( m_paEffectPtrAttay[ i ].pEffect )
		//	{
		//		// 이펙트 딜레이체크.
		//		if( m_pcsAgcmEff2 && m_paEffectPtrAttay[ i ].uStartTime + m_paEffectPtrAttay[ i ].uGap < uCurrentTick )
		//		{
		//			// 이펙트 딜레이 오버 ..
		//			// 디스트로이 시킴..

		//			// 이펙트 직접 디스트로이는 하지 않고 , 이펙트 모듈에서 각각 lifetime을
		//			// 가지기 때문에 주석처리함.

		//			// 마고자 (2004-07-18 오후 5:49:45) : 길이를 정확히 설정하기때문에 , 굳이
		//			// 리무브를 하지 않아도 상관없을걸로 생각된다.
		//			// m_pcsAgcmEff2->RemoveEffectSet( m_paEffectPtrAttay [ i ].pNode );

		//			// 빈슬롯만듬~
		//			m_paEffectPtrAttay [ i ].pEffect	= NULL;
		//		}
		//	}
		//}

		if( nEffect > 0 )
		{
			// 해당이펙트가 플레이중인지..있는지 검사..
			BOOL bIsAlreayRunningEffect = FALSE;
			for( i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
			{
				//if( NULL != m_paEffectPtrAttay[ i ].pEffect &&
				//	m_paEffectPtrAttay[ i ].lEID == nEffect )
				if( m_paEffectPtrAttay[ i ].lEID == nEffect )
				{
					// 있다..
					bIsAlreayRunningEffect = TRUE;
				}
			}

			if( !bIsAlreayRunningEffect )
			{
				// 없으면 빈데 찾아서 플레이.
				for( i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
				{
					if( NULL == m_paEffectPtrAttay[ i ].pEffect && m_pcsAgcmEff2 )
					{
						stEffUseInfo stUseInfo	;

						stUseInfo.m_ulEffID			=	nEffect	;
						stUseInfo.m_pFrmParent	=	NULL	;
						stUseInfo.m_ulFlags		   &= ~stEffUseInfo::E_FLAG_LINKTOPARENT;
	
						m_paEffectPtrAttay[ i ].pEffect		= m_pcsAgcmEff2->UseEffSet( &stUseInfo )	;
						if( m_paEffectPtrAttay[ i ].pEffect )
						{
							AgcdEffCtrl_Set * pEffect = m_paEffectPtrAttay[ i ].pEffect;
							AgcdEffCtrl_Sound * pSound = ( AgcdEffCtrl_Sound* )pEffect->Get1stSound();
							
							//if( pEffect && ( pSound = ( AgcdEffCtrl_Sound * ) pEffect->Get1stSound() ) )
							//{
							//	TRACE( "****************************************************************\n" );
							//	TRACE( "AgcmEventNature::SetTime = '%s' 웨이브 파일 플레이 시작\n" , ( ( AgcdEffSound * ) pSound->GetCPtrEffBase() )->bGetSoundFName() );
							//	TRACE( "****************************************************************\n" );

							//	//pSound->bSetSoundType( AgcdEffSound::EFFECT_SAMPLE_SOUND );
							//	pSound->SetLoopCnt( 1 );
							//}

							const AgcdEffSet* pcdEffSet = m_paEffectPtrAttay[ i ].pEffect->GetPtrEffSet();
							E_LOOPOPT eLoopType = pcdEffSet->bGetLoopOpt();

							//if( pSound->GetSoundType() == AgcdEffSound::EFFECT_STREAM_SOUND && eLoopType == e_TblDir_infinity )
							//{
							//	// 새로 플레이 될 사운드가 스트리밍 환경사운드라면.. 기존에 플레이 되던 것은 제거해준다.
							//	for( int nCount = 0 ; nCount < 	AGCMEVENTNATURE_MAX_EVENT ; nCount++ )
							//	{
							//		// 방금 집어넣은 것은 제외한다...ㅡ.ㅡ.. 물론..
							//		if( m_paEffectPtrAttay[ nCount ].pEffect && nCount != i )
							//		{
							//			const AgcdEffSet* pcdPrevEffSet = m_paEffectPtrAttay[ nCount ].pEffect->GetPtrEffSet();
							//			if( pcdPrevEffSet )
							//			{
							//				E_LOOPOPT eLoopTypePrev = pcdPrevEffSet->bGetLoopOpt();

							//				AgcdEffCtrl_Sound* pSoundPrev = ( AgcdEffCtrl_Sound* )m_paEffectPtrAttay[ nCount ].pEffect->Get1stSound();
							//				if( pSoundPrev )
							//				{
							//					AgcdEffSound::eEffSoundType ePrevType = pSoundPrev->GetSoundType();

							//					if( eLoopTypePrev == e_TblDir_infinity && ePrevType == AgcdEffSound::EFFECT_STREAM_SOUND )
							//					{
							//						m_paEffectPtrAttay[ nCount ].pEffect->End( FALSE );
							//						m_paEffectPtrAttay[ nCount ].pEffect = NULL;
							//					}
							//				}
							//			}
							//		}
							//	}
							//}

							m_paEffectPtrAttay[ i ].uGap	= m_pcsAgcmEff2->GetSoundLength( nEffect );
							m_paEffectPtrAttay[ i ].pEffect->SetLife( m_paEffectPtrAttay[ i ].uGap );

							//{@ kday 20041013
							if( !m_paEffectPtrAttay[ i ].uGap && eLoopType != e_TblDir_infinity )
							{
								char strDebug[ 256 ] = { 0, };
								sprintf_s( strDebug, sizeof( char ) * 256, "End Effect %d, Gap = %d, LoopType = %d\n",
									m_paEffectPtrAttay[ i ].pEffect->GetPtrEffSet()->m_dwEffSetID, m_paEffectPtrAttay[ i ].uGap, eLoopType );
								OutputDebugString( strDebug );

								m_paEffectPtrAttay[ i ].pEffect->End(FALSE);
								m_paEffectPtrAttay[ i ].pEffect	= NULL;
							}

							m_paEffectPtrAttay[ i ].uStartTime	= uCurrentTick									;
							m_paEffectPtrAttay[ i ].lEID		= nEffect										;
						}
						else
						{
							TRACE( "AgcmEventNature::SetTime = ( %d ) 이펙트를 재생하는데 실패하였습니다.\n" , nEffect );
							// 마고자 (2004-07-18 오후 3:50:34) : 값은 대충 주고 넘김..
							m_paEffectPtrAttay[ i ].uGap		= 5000			;
							m_paEffectPtrAttay[ i ].uStartTime	= uCurrentTick	;
							m_paEffectPtrAttay[ i ].lEID		= nEffect		;
						}

						// 이펙트 플레이..
						break;
					}
				}
			}
		}
	}
	// 같으므로 처리하지 않음..
	// if( m_pcsAgpmTimer->TimerIsStop()) return TRUE;	

	INT32	hourPrev	= m_pcsAgpmEventNature->GetHour( m_nCurrentTime )	;
	INT32	nDelta		= ( INT32 ) ( time - m_nCurrentTime	);

	m_nCurrentTime = time;

	INT32	hour , minute	;
	UINT32	milisecond		;

	hour		= m_pcsAgpmEventNature->GetHour			( m_nCurrentTime  );
	minute		= m_pcsAgpmEventNature->GetMinute		( m_nCurrentTime  );
	milisecond	= m_pcsAgpmEventNature->GetHourTimeDWORD	( m_nCurrentTime  );

	// 타임셋 적용..
	ASkySetting::TimeSet	ePrevTimeSet	= ASkySetting::GetTimeSet( hourPrev	);
	ASkySetting::TimeSet	eCurrentTimeSet	= ASkySetting::GetTimeSet( hour		);

	ASSERT( 0 <= hour && hour < 24 );

	// 시간변화로 스카이를 바꾸는 걸 지정함..
	if( ePrevTimeSet != eCurrentTimeSet 
		
	#ifdef USE_MFC
		&& !( GetMaptoolDebugInfoFlag() & SKY_DONOTTIMECHANGE )
	#endif
		
		)
	{
		// 변화를 지정..
		SetNextWeather( eCurrentTimeSet , __uSkyChangeGap , FALSE );
	}

	ASkyQueueData	* pNextSetting		;
	ASkyQueueData	* pCurrentSetting	;

	pNextSetting	= &m_skyCurrentSetting[ NEXT_SKY	];
	pCurrentSetting	= &m_skyCurrentSetting[ CURRENT_SKY	];
	
	// 구름 빙글빙글..
	{		
		// INT32 __uDelta = uTickDelta * 20 ;
		FLOAT __uDelta;

		if( m_pcsAgpmTimer->TimerIsStop() )
		{
			// 정지이면 틱으로..
			__uDelta = ( FLOAT ) uTickDelta * __cloud_flow_adjust ;
		}
		else
		{
			// 흘러가면 타임으로..
			__uDelta = ( FLOAT ) nDeltaTime * __cloud_flow_adjust ;
		}
		
		
		FlowCloud( FIRST_CLOUD			 , ( __uDelta * pCurrentSetting	->aCloud[ ASkySetting::CLOUD_FIRST	].fSpeed ) / 3600000.0f );
		FlowCloud( SECOND_CLOUD			 , ( __uDelta * pCurrentSetting	->aCloud[ ASkySetting::CLOUD_SECOND	].fSpeed ) / 3600000.0f );
		if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_CLOUD )
		{
			FlowCloud( FIRST_CLOUD_FADING	 , ( __uDelta * pNextSetting	->aCloud[ ASkySetting::CLOUD_FIRST	].fSpeed ) / 3600000.0f );
			FlowCloud( SECOND_CLOUD_FADING	 , ( __uDelta * pNextSetting	->aCloud[ ASkySetting::CLOUD_SECOND	].fSpeed ) / 3600000.0f );
		}
	}

	// 태양 위치조절..
	if( m_pLightDirectional

		#ifdef USE_MFC
			&& !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGELIGHTPOSITION )
		#endif
		
		)
	{
//		FLOAT	fRate	=		( FLOAT ) m_pcsAgpmEventNature->GetDayTimeDWORD( m_nCurrentTime	)
//							/	( FLOAT ) m_pcsAgpmEventNature->GetDayMiliSecond()					;

		// 2시경우로 설정..
		FLOAT	fRate	=		14.0f / 24.0f;

		FLOAT	fAngle	= 90.0f + 360.0f * fRate;


		RwFrame * pFrame	= RpLightGetFrame( m_pLightDirectional );

		ASSERT( NULL != pFrame );

		//while( fAngle >= 180.0f ) fAngle -= 180.0f;

		RwMatrix	* pMatrix	;
		RwV3d		* pAtVector	;

		pMatrix		= RwFrameGetMatrix	( pFrame	);
		pAtVector	= RwMatrixGetAt		( pMatrix	);

		// 서쪽 끝에 붙여 넣음..
		//RwMatrixRotate( pMatrix	, & yaxis	, -90		,	rwCOMBINEREPLACE	);

		RwMatrixRotate( pMatrix	, & yaxis	, 50		,	rwCOMBINEREPLACE	);
		RwMatrixRotate( pMatrix	, & zaxis	, fAngle	,	rwCOMBINEPOSTCONCAT	);

		/*
		RwMatrix	* pRotateMatrix;
		pRotateMatrix = RwMatrixCreate();
		RwMatrixRotate( pRotateMatrix , & zaxis , -fAngle  , rwCOMBINEREPLACE );
		RwV3dTransformVector( pAtVector , pAtVector , pRotateMatrix );
		RwMatrixRotate( pRotateMatrix , & xaxis , 10  , rwCOMBINEREPLACE );
		RwV3dTransformVector( pAtVector , pAtVector , pRotateMatrix );
		RwMatrixDestroy( pRotateMatrix );
		*/

		RwMatrixUpdate		( pMatrix );
		RwMatrixOrthoNormalize (pMatrix,pMatrix);

		LockFrame();
		RwFrameUpdateObjects( pFrame );
		UnlockFrame();
	}

	if( IsSkyChanging() )
	{
		ASSERT( m_skyCurrentSetting[ NEXT_SKY		].bAvailable );

		// Light 설정 변경..

		RwV3d	pos;
		pos.x	=	AgcmEventNature::SKY_RADIUS	;
		pos.y	=	0.0f								;
		pos.z	=	0.0f								;

		UINT32	__uCurrentTime	= ( UINT32 ) m_nCurrentTime;
		UINT32	nDeltaTime = ( UINT32 ) ( __uCurrentTime	- m_nSkyChangeStartTime	);

		FLOAT	fRate	=	( FLOAT ) nDeltaTime / 
							( FLOAT ) pNextSetting->nDuration;	// 현재 시간에 , 다음시간까지의 비율을 구해냄.

		if( nDeltaTime )
		{
			if( fRate > 1.0f )
			{
				// 이미 다 바뀌었음..
				m_skyCurrentSetting[ CURRENT_SKY ].nDataAvailableFlag = ASkySetting::DA_ALL;

				// 부분 부분 카피해넣음..

				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_LIGHT	)
				{
					m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyTop		= m_skyCurrentSetting[ NEXT_SKY	].rgbSkyTop			;
					m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyBottom		= m_skyCurrentSetting[ NEXT_SKY	].rgbSkyBottom		;
					m_skyCurrentSetting[ CURRENT_SKY ].rgbAmbient		= m_skyCurrentSetting[ NEXT_SKY	].rgbAmbient		;
					m_skyCurrentSetting[ CURRENT_SKY ].rgbDirectional	= m_skyCurrentSetting[ NEXT_SKY	].rgbDirectional	;

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGELIGHTCOLOR ) )
					#endif
					{
						RpLightSetColor( m_pLightDirectional	, &m_skyCurrentSetting[ CURRENT_SKY ].rgbDirectional	);
						RpLightSetColor( m_pLightAmbient		, &m_skyCurrentSetting[ CURRENT_SKY ].rgbAmbient		);

						this->m_pcsAgcmRender->LightValueUpdate();
						EnumCallback( AGCMEVENTNATURE_CB_ID_LIGHTUPDATE , (PVOID) &m_skyCurrentSetting[ CURRENT_SKY ].rgbDirectional , (PVOID) &m_skyCurrentSetting[ CURRENT_SKY ].rgbAmbient );
					}

					// 스카이 칼라세팅..
					RwRGBA	rwColorTop , rwColorBottom;
					rwColorTop.alpha	=	255;
					rwColorTop.red		=	( RwUInt8 ) ( ( m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyTop.red	) * 255.0f );
					rwColorTop.green	=	( RwUInt8 ) ( ( m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyTop.green	) * 255.0f );
					rwColorTop.blue		=	( RwUInt8 ) ( ( m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyTop.blue	) * 255.0f );

					rwColorBottom.alpha	=	255;
					rwColorBottom.red	=	( RwUInt8 ) ( ( m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyTop.red	) * 255.0f );
					rwColorBottom.green	=	( RwUInt8 ) ( ( m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyTop.green	) * 255.0f );
					rwColorBottom.blue	=	( RwUInt8 ) ( ( m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyTop.blue	) * 255.0f );

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGESKYCOLOR ) )
					#endif
					{
						ModifySky( & m_AtomicSky , & rwColorTop , & rwColorBottom );
					}
				}
				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_FOG		)
				{
					m_skyCurrentSetting[ CURRENT_SKY ].rgbFog			= m_skyCurrentSetting[ NEXT_SKY	].rgbFog			;
					m_skyCurrentSetting[ CURRENT_SKY ].fFogDistance		= m_skyCurrentSetting[ NEXT_SKY	].fFogDistance	;
					m_skyCurrentSetting[ CURRENT_SKY ].fFogFarClip		= m_skyCurrentSetting[ NEXT_SKY	].fFogFarClip	;

					RwRGBA	FogColor	;
					FogColor.alpha	=	255;
					FogColor.red	=	( RwUInt8	)( 255.0f * ( m_skyCurrentSetting[ CURRENT_SKY ].rgbFog.red		) );	
					FogColor.green	=	( RwUInt8	)( 255.0f * ( m_skyCurrentSetting[ CURRENT_SKY ].rgbFog.green	) );
					FogColor.blue	=	( RwUInt8	)( 255.0f * ( m_skyCurrentSetting[ CURRENT_SKY ].rgbFog.blue	) );

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGEFOG ) )
					#endif
					{
						SetFog(	TRUE												,
								m_skyCurrentSetting[ CURRENT_SKY ].fFogDistance		,
								m_skyCurrentSetting[ CURRENT_SKY ].fFogFarClip		,
								& FogColor											);
					}
				}
				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_CLOUD	)
				{
					m_skyCurrentSetting[ CURRENT_SKY ].aCloud[ ASkySetting::CLOUD_FIRST		]	= m_skyCurrentSetting[ NEXT_SKY	].aCloud[ ASkySetting::CLOUD_FIRST	]	;
					m_skyCurrentSetting[ CURRENT_SKY ].aCloud[ ASkySetting::CLOUD_SECOND	]	= m_skyCurrentSetting[ NEXT_SKY	].aCloud[ ASkySetting::CLOUD_SECOND	]	;
					m_skyCurrentSetting[ CURRENT_SKY ].nCircumstanceTexture						= m_skyCurrentSetting[ NEXT_SKY	].nCircumstanceTexture					;
					m_skyCurrentSetting[ CURRENT_SKY ].rgbCircumstance							= m_skyCurrentSetting[ NEXT_SKY	].rgbCircumstance						;
					m_skyCurrentSetting[ CURRENT_SKY ].nSunTexture								= m_skyCurrentSetting[ NEXT_SKY	].nSunTexture							;
					m_skyCurrentSetting[ CURRENT_SKY ].rgbSun									= m_skyCurrentSetting[ NEXT_SKY	].rgbSun								;

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGECLOUD ) )
					#endif
					{
						CloudUVShift( &m_skyCurrentSetting[ NEXT_SKY	] );

						for( int i = 0 ; i < ASkySetting::CLOUD_LAYER_MAX ; ++i )
						{
							//if( pNextSetting->aCloud[ i ].nTexture != pCurrentSetting->aCloud[ i ].nTexture )
							SetCloud	(	i									,
											&m_skyCurrentSetting[ CURRENT_SKY ]	,
											NULL								,
											1.0f								);
						}
					}

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGECIRCUMSTANCE ) )
					#endif
					{
						SetCircumstance	(	&m_skyCurrentSetting[ CURRENT_SKY ]	,
											NULL								,
											1.0f								);
						SetSun			(	&m_skyCurrentSetting[ CURRENT_SKY ]	,
											NULL								,
											1.0f								);
					}

				}
				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_EFFECT	)
				{
					m_skyCurrentSetting[ CURRENT_SKY ].nEffectInterval	= m_skyCurrentSetting[ NEXT_SKY	].nEffectInterval;
				
					for( int k = 0 ; k < AGCMEVENTNATURE_MAX_EVENT ; ++ k )
					{
						m_skyCurrentSetting[ CURRENT_SKY ].aEffectArray		[ k ]	= m_skyCurrentSetting[ NEXT_SKY	].aEffectArray		[ k ];
						m_skyCurrentSetting[ CURRENT_SKY ].aEffectRateArray	[ k ]	= m_skyCurrentSetting[ NEXT_SKY	].aEffectRateArray	[ k ];
					}
				}
				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_MUSIC	)
				{
					strncpy( m_skyCurrentSetting[ CURRENT_SKY ].strBackMusic , m_skyCurrentSetting[ NEXT_SKY	].strBackMusic , EFF2_FILE_NAME_MAX );

					// 백그라운드 뮤직 플레이 명령은 여기서..
					// m_skyCurrentSetting[ CURRENT_SKY ].strBackMusic
				}

				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_WEATHER	)
				{
					m_skyCurrentSetting[ CURRENT_SKY ].nWeatherEffect1 = m_skyCurrentSetting[ NEXT_SKY	].nWeatherEffect1;
					m_skyCurrentSetting[ CURRENT_SKY ].nWeatherEffect2 = m_skyCurrentSetting[ NEXT_SKY	].nWeatherEffect2;
					// 스카이 변경..
				}				
				if( m_skyCurrentSetting[ WAIT_SKY ].bAvailable )
				{
					m_skyCurrentSetting[ NEXT_SKY	]				= m_skyCurrentSetting[ WAIT_SKY ];
					m_skyCurrentSetting[ WAIT_SKY	].bAvailable	= FALSE;
					m_nSkyChangeStartTime							= m_nCurrentTime;
				}
				else
				{
					m_bSkyIsChanging = FALSE;
					m_skyCurrentSetting[ NEXT_SKY	].bAvailable	= FALSE;
				}

				SkyChange( &m_skyCurrentSetting[ CURRENT_SKY ] );
			}
			else
			{
				// 라이트 칼라 세팅..
				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_LIGHT )
				{
					ASSERTONCE( m_pLightAmbient && m_pLightDirectional );

					// Directional Light 칼라 수정..
					RwRGBAReal	rwDirectionalColor , rwAmbient;
					rwDirectionalColor.alpha	=	1.0f;
					rwDirectionalColor.red		=	pCurrentSetting->rgbDirectional.red	+
													( pNextSetting->rgbDirectional.red		- pCurrentSetting->rgbDirectional.red	)	* fRate;
					rwDirectionalColor.green	=	pCurrentSetting->rgbDirectional.green	+
													( pNextSetting->rgbDirectional.green	- pCurrentSetting->rgbDirectional.green	)	* fRate;
					rwDirectionalColor.blue		=	pCurrentSetting->rgbDirectional.blue	+
													( pNextSetting->rgbDirectional.blue		- pCurrentSetting->rgbDirectional.blue	)	* fRate;

					// Ambient Light 칼라 수정..
					rwAmbient.alpha	=	1.0f;
					rwAmbient.red		=	pCurrentSetting->rgbAmbient.red	+
											( pNextSetting->rgbAmbient.red		- pCurrentSetting->rgbAmbient.red	)	* fRate;
					rwAmbient.green		=	pCurrentSetting->rgbAmbient.green	+
											( pNextSetting->rgbAmbient.green	- pCurrentSetting->rgbAmbient.green	)	* fRate;
					rwAmbient.blue		=	pCurrentSetting->rgbAmbient.blue	+
											( pNextSetting->rgbAmbient.blue		- pCurrentSetting->rgbAmbient.blue	)	* fRate;

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGELIGHTCOLOR ) )
					#endif
					{
						if( m_pLightDirectional )
							RpLightSetColor( m_pLightDirectional , &rwDirectionalColor );
						if( m_pLightAmbient )
							RpLightSetColor( m_pLightAmbient , &rwAmbient );
						this->m_pcsAgcmRender->LightValueUpdate();
						EnumCallback( AGCMEVENTNATURE_CB_ID_LIGHTUPDATE , (PVOID) &rwDirectionalColor , (PVOID) &rwAmbient );
					}



					// 스카이 칼라세팅..
					RwRGBA	rwColorTop , rwColorBottom;
					rwColorTop.alpha	=	255;
					rwColorTop.red		=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyTop.red	+
											( pNextSetting->rgbSkyTop.red	- pCurrentSetting->rgbSkyTop.red	)	* fRate ) * 255.0f );
					rwColorTop.green	=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyTop.green	+
											( pNextSetting->rgbSkyTop.green	- pCurrentSetting->rgbSkyTop.green	)	* fRate ) * 255.0f );
					rwColorTop.blue		=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyTop.blue	+
											( pNextSetting->rgbSkyTop.blue	- pCurrentSetting->rgbSkyTop.blue	)	* fRate ) * 255.0f );

					rwColorBottom.alpha	=	255;
					rwColorBottom.red	=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyBottom.red	+
											( pNextSetting->rgbSkyBottom.red	- pCurrentSetting->rgbSkyBottom.red		)	* fRate ) * 255.0f );
					rwColorBottom.green	=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyBottom.green	+
											( pNextSetting->rgbSkyBottom.green	- pCurrentSetting->rgbSkyBottom.green	)	* fRate ) * 255.0f );
					rwColorBottom.blue	=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyBottom.blue	+
											( pNextSetting->rgbSkyBottom.blue	- pCurrentSetting->rgbSkyBottom.blue	)	* fRate ) * 255.0f );

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGESKYCOLOR ) )
					#endif
					{
						ModifySky( & m_AtomicSky , & rwColorTop , & rwColorBottom );
					}
				}

				// 배경설정...
				static	bool	bFirstModify	= true	;
				static	UINT32	uLastModifyTime = 0		;

				// 마고자 (2003-06-16 오후 11:25:49) : 일정시간에 한번씩만 바뀌도록..
				if( bFirstModify )
				{
					bFirstModify = false;
					uLastModifyTime	= __uCurrentTime - 501;
				}

				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_CLOUD
					//&&	__uCurrentTime - uLastModifyTime > 500
					)
				{		

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGECLOUD ) )
					#endif
					{
						for( int i = 0 ; i < ASkySetting::CLOUD_LAYER_MAX ; ++i )
						{
							//if( pNextSetting->aCloud[ i ].nTexture != pCurrentSetting->aCloud[ i ].nTexture )
							SetCloud	(	i					,
											pCurrentSetting		,
											pNextSetting		,
											1.0f - fRate	);
						}
					}

					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGECIRCUMSTANCE ) )
					#endif
					{
						SetCircumstance	(	pCurrentSetting	,
											pNextSetting	,
											1.0f - fRate	);
						SetSun			(	pCurrentSetting	,
											pNextSetting	,
											1.0f - fRate	);
					}
				}

				// 포그 설정..

				if( pNextSetting->nDataAvailableFlag & ASkySetting::DA_FOG )
				{
					RwRGBA	FogColor	;
					FLOAT	fDistance	;
					FLOAT	fFogFarClip	;

					fDistance		=	pCurrentSetting->fFogDistance	+
										( pNextSetting->fFogDistance		- pCurrentSetting->fFogDistance		)	* fRate;
					fFogFarClip		=	pCurrentSetting->fFogFarClip	+
										( pNextSetting->fFogFarClip			- pCurrentSetting->fFogFarClip		)	* fRate;

					FogColor.alpha	=	255;
					FogColor.red	=	( RwUInt8	)( 255.0f * ( pCurrentSetting->rgbFog.red	+	
										( pNextSetting->rgbFog.red		- pCurrentSetting->rgbFog.red	)	* fRate ) );
					FogColor.green	=	( RwUInt8	)( 255.0f * ( pCurrentSetting->rgbFog.green	+
										( pNextSetting->rgbFog.green	- pCurrentSetting->rgbFog.green	)	* fRate ) );
					FogColor.blue	=	( RwUInt8	)( 255.0f * ( pCurrentSetting->rgbFog.blue	+
										( pNextSetting->rgbFog.blue		- pCurrentSetting->rgbFog.blue	)	* fRate ) );
							
					#ifdef USE_MFC
					if( !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGEFOG ) )
					#endif
					{
						SetFog( TRUE , fDistance , fFogFarClip , & FogColor );
					}
				}
			}
			////////////////////////
		}

	}

	return TRUE;
}

BOOL		AgcmEventNature::SetTimeForce		( UINT64	time				)
{
	//if( time == m_nCurrentTime ) return TRUE;	// 같으므로 처리하지 않음..
	INT32	hourPrev	= m_pcsAgpmEventNature->GetHour( m_nCurrentTime );

	m_nCurrentTime = time;

	INT32	hour , minute	;
	UINT32	milisecond		;

	hour		= m_pcsAgpmEventNature->GetHour			( m_nCurrentTime  );
	minute		= m_pcsAgpmEventNature->GetMinute		( m_nCurrentTime  );
	milisecond	= m_pcsAgpmEventNature->GetHourTimeDWORD	( m_nCurrentTime  );

	ASSERT( 0 <= hour && hour < 24 );

	ASkySetting::TimeSet	ePrevTimeSet	= ASkySetting::GetTimeSet( hourPrev	);
	ASkySetting::TimeSet	eCurrentTimeSet	= ASkySetting::GetTimeSet( hour		);

	// 시간변화로 스카이를 바꾸는 걸 지정함..
//	if( hourPrev != hour )
	{
		// 변화를 지정..
		SetNextWeather( eCurrentTimeSet , __uSkyChangeGap , TRUE );
	}

	m_skyCurrentSetting[ NEXT_SKY		].bAvailable	= FALSE;
	m_bSkyIsChanging									= FALSE;

	ASSERT( m_skyCurrentSetting[ CURRENT_SKY		].bAvailable );

		// Light 설정 변경..

	ASkyQueueData	* pCurrentSetting	;

	pCurrentSetting	= &m_skyCurrentSetting[ CURRENT_SKY	];

	RwV3d	pos;
	pos.x	=	AgcmEventNature::SKY_RADIUS	;
	pos.y	=	0.0f								;
	pos.z	=	0.0f								;

	UINT32	nDeltaTime = ( UINT32 ) ( m_nCurrentTime - m_nSkyChangeStartTime );

	FLOAT	fRate	=	0.0f;	// 현재 시간에 , 다음시간까지의 비율을 구해냄.

	if( fRate > 1.0f )	fRate = 1.0f;

	// 태양 위치조절..
	if( m_pLightDirectional )
	{
//		FLOAT	fRate	=		( FLOAT ) m_pcsAgpmEventNature->GetDayTimeDWORD( m_nCurrentTime	)
//							/	( FLOAT ) m_pcsAgpmEventNature->GetDayMiliSecond()					;

		// 2시경우로 설정..
		FLOAT	fRate	=		14.0f / 24.0f;

		FLOAT	fAngle	= 90.0f + 360.0f * fRate;


		RwFrame * pFrame	= RpLightGetFrame( m_pLightDirectional );

		ASSERT( NULL != pFrame );

		//while( fAngle >= 180.0f ) fAngle -= 180.0f;

		RwMatrix	* pMatrix	;
		RwV3d		* pAtVector	;

		pMatrix		= RwFrameGetMatrix	( pFrame	);
		pAtVector	= RwMatrixGetAt		( pMatrix	);

		pAtVector->x	= -1.0f;
		pAtVector->y	= 0.0f;
		pAtVector->z	= 0.0f;

		RwMatrixRotate( pMatrix	, & yaxis	, 50		,	rwCOMBINEREPLACE	);
		RwMatrixRotate( pMatrix	, & zaxis	, fAngle	,	rwCOMBINEPOSTCONCAT	);

		TRACE("Angle :%d\n", (INT32) fAngle);
		/*
		RwMatrix	* pRotateMatrix;
		pRotateMatrix = RwMatrixCreate();
		RwMatrixRotate( pRotateMatrix , & zaxis , -fAngle  , rwCOMBINEREPLACE );
		RwV3dTransformVector( pAtVector , pAtVector , pRotateMatrix );
		RwMatrixRotate( pRotateMatrix , & xaxis , 10  , rwCOMBINEREPLACE );
		RwV3dTransformVector( pAtVector , pAtVector , pRotateMatrix );
		RwMatrixDestroy( pRotateMatrix );
		*/

		RwMatrixUpdate		( pMatrix );
		RwMatrixOrthoNormalize (pMatrix,pMatrix);

		LockFrame();
		RwFrameUpdateObjects( pFrame );
		UnlockFrame();
	}

	// 라이트 칼라 세팅..
	{
		ASSERTONCE( m_pLightAmbient && m_pLightDirectional );

		// Directional Light 칼라 수정..
		RwRGBAReal	rwColor;
		rwColor.alpha	=	1.0f;
		rwColor.red		=	pCurrentSetting->rgbDirectional.red		;
		rwColor.green	=	pCurrentSetting->rgbDirectional.green	;
		rwColor.blue	=	pCurrentSetting->rgbDirectional.blue	;

		if( m_pLightDirectional )
			RpLightSetColor( m_pLightDirectional , &rwColor );

		// Ambient Light 칼라 수정..
		rwColor.alpha	=	1.0f;
		rwColor.red		=	pCurrentSetting->rgbAmbient.red		;
		rwColor.green	=	pCurrentSetting->rgbAmbient.green	;
		rwColor.blue	=	pCurrentSetting->rgbAmbient.blue	;
		if( m_pLightAmbient )
			RpLightSetColor( m_pLightAmbient , &rwColor );


		// 스카이 칼라세팅..
		RwRGBA	rwColorTop , rwColorBottom;
		rwColorTop.alpha	=	255;
		rwColorTop.red		=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyTop.red		) * 255.0f );
		rwColorTop.green	=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyTop.green	) * 255.0f );
		rwColorTop.blue		=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyTop.blue		) * 255.0f );

		rwColorBottom.alpha	=	255;
		rwColorBottom.red	=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyBottom.red	) * 255.0f );
		rwColorBottom.green	=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyBottom.green	) * 255.0f );
		rwColorBottom.blue	=	( RwUInt8 ) ( ( pCurrentSetting->rgbSkyBottom.blue	) * 255.0f );

		ModifySky( & m_AtomicSky , & rwColorTop , & rwColorBottom );

		this->m_pcsAgcmRender->LightValueUpdate();
		EnumCallback( AGCMEVENTNATURE_CB_ID_LIGHTUPDATE , (PVOID) &pCurrentSetting->rgbDirectional , (PVOID) &pCurrentSetting->rgbAmbient );
	}

	for( int i = 0 ; i < ASkySetting::CLOUD_LAYER_MAX ; ++i )
	{
		SetCloud	(	i				,
						pCurrentSetting	,
						NULL			,
						1.0f			);
	}
	SetCircumstance	(	pCurrentSetting	,
						NULL			,
						1.0f			);
	SetSun			(	pCurrentSetting	,
						NULL			,
						1.0f			);

	// 포그 설정..
	{
		FLOAT	fDistance	;
		FLOAT	fDensity	;
		RwRGBA	FogColor	;

		fDistance		=	pCurrentSetting->fFogDistance	;
		fDensity		=	pCurrentSetting->fFogFarClip	;

		FogColor.alpha	=	255;
		FogColor.red	=	( RwUInt8	)( 255.0f * pCurrentSetting->rgbFog.red	);
		FogColor.green	=	( RwUInt8	)( 255.0f * pCurrentSetting->rgbFog.green	);
		FogColor.blue	=	( RwUInt8	)( 255.0f * pCurrentSetting->rgbFog.blue	);

		
		//SetFog( pNextSetting->bFogOn , fDistance , fDensity , & FogColor , rwFOGTYPEEXPONENTIAL2 );
		// 마고자 (2003-06-16 오후 10:53:25) : 포그설정은 현재 상태를 유지함..
		SetFog( TRUE , fDistance , fDensity , & FogColor );
	}

	CloudUVShift( pCurrentSetting );

	return TRUE;
}

void		AgcmEventNature::SetSkyRadius		( FLOAT		fRadius		)
{
	ASSERT( NULL != m_pCamera );

	// 마고자 (2003-08-28 오후 5:00:38) : 충성아저씨 요청으로 하드코딩
	//(FLOAT) AgcmEventNature::SKY_RADIUS	= fRadius			;
	//(FLOAT) AgcmEventNature::SKY_RADIUS	= 100900.0f	* 2.0f	;

//	m_fSkyRadius	=	
//		sqrt(
//						( AgcmEventNature::CIRCUMSTANCE_RADIUS * 1.4145727f ) *
//						( AgcmEventNature::CIRCUMSTANCE_RADIUS * 1.4145727f )
//						+
//						AgcmEventNature::CIRCUMSTANCE_HEIGHT *
//						AgcmEventNature::CIRCUMSTANCE_HEIGHT
//		) * 1.2f; // 1.2배 키워줌.;

//	m_fSkyRadius	= AgcmEventNature::SKY_RADIUS;
	ASSERT( NULL != m_pcsAgcmMap );
//	m_fSkyRadius	= ( FLOAT ) ( m_pcsAgcmMap->GetRoughLoadRadius() + 1 ) * MAP_SECTOR_WIDTH;
//	m_fSkyRadius	*= sqrt( 2.0f );

	// 하늘 넓이 6만으로 고정시켜둠. 
	m_fSkyRadius	= 60000.0f;

	g_pEngine->SetWorldCameraFarClipPlane	( m_fSkyRadius	);
	// Near Clip Plane을 200에서 100으로 줄였음. 너무 먼것 같아서
	g_pEngine->SetWorldCameraNearClipPlane	( __fNearClipPlane = 60.0f		);
}

void		AgcmEventNature::SetSkyRadius		( INT32		nRadius		)
{
	// 마고자 (2003-08-28 오후 5:00:38) : 충성아저씨 요청으로 하드코딩
	//(FLOAT) AgcmEventNature::SKY_RADIUS	= nRadius * MAP_SECTOR_WIDTH;		;
	//(FLOAT) AgcmEventNature::SKY_RADIUS	= 100900.0f			;

//	m_fSkyRadius	=	
//		sqrt(
//						( AgcmEventNature::CIRCUMSTANCE_RADIUS * 1.4145727f ) *
//						( AgcmEventNature::CIRCUMSTANCE_RADIUS * 1.4145727f )
//						+
//						AgcmEventNature::CIRCUMSTANCE_HEIGHT *
//						AgcmEventNature::CIRCUMSTANCE_HEIGHT
//		) * 1.2f; // 1.2배 키워줌.;

//	m_fSkyRadius	= AgcmEventNature::SKY_RADIUS;

	ASSERT( NULL != m_pcsAgcmMap );
	m_fSkyRadius	= ( FLOAT ) ( m_pcsAgcmMap->GetRoughLoadRadius() + 1 ) * MAP_SECTOR_WIDTH;
	m_fSkyRadius	*= sqrt( 2.0f );

	g_pEngine->SetWorldCameraFarClipPlane(m_fSkyRadius);
	// Near Clip Plane을 200에서 100으로 줄였음. 너무 먼것 같아서
	g_pEngine->SetWorldCameraNearClipPlane	( __fNearClipPlane = 30.0f		);

	SetFogUpdate();
}


void		AgcmEventNature::SetFog					( BOOL		bFogOn	, FLOAT		fFogDistance	, FLOAT		fFogFarClip	, RwRGBA	*	pstFogColor		)
{
	// 마고자 (2004-07-20 오후 9:34:07) : m_bFogOn 은 여기서 사용안함..

	//m_bFogOn		= bFogOn		;
	m_fFogDistance	= fFogDistance	;
	m_fFogFarClip	= fFogFarClip	;
	if( pstFogColor )
		m_stFogColor	= *pstFogColor	;

//	RwRenderStateSet		( rwRENDERSTATEFOGENABLE, (void *) m_bFogOn					);

//	if( m_bFogOn && m_pCamera )
//	{
//		RwUInt32				red, green, blue, alpha;
//
//		red		= m_stFogColor.red;
//		green	= m_stFogColor.green;
//		blue	= m_stFogColor.blue;
//		alpha	= m_stFogColor.alpha;
//
//		RwCameraSetFogDistance	( m_pCamera , m_fFogDistance								);
//
//		RwCameraSetFarClipPlane	( m_pCamera , (FLOAT) ( AgcmEventNature::SKY_RADIUS * 1.4 )		);
//		RwCameraSetNearClipPlane( m_pCamera , (FLOAT) ( 200.0 )	);
//
//		RwRenderStateSet		( rwRENDERSTATEFOGCOLOR, (void *) RWRGBALONG(m_stFogColor.red, m_stFogColor.green, m_stFogColor.blue, m_stFogColor.alpha) );
//
//		RwRenderStateSet		( rwRENDERSTATEFOGTYPE, (void *) m_eFogType					);
//		RwRenderStateSet		( rwRENDERSTATEFOGDENSITY, (void *) &m_fFogFarClip			);
//
//		// 다음 렌더 사이클에서 포그를 업데이트함..
//	}

	SetFogUpdate();
}

BOOL	AgcmEventNature::SetCloud		( INT32 nLayer , ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo	, FLOAT fAlpha )
{
	ASSERT( 0.0f <= fAlpha && 1.0f >= fAlpha						);

	unsigned char value = ( unsigned char ) ( fAlpha * 255.0f ) % 256;

	RwRGBA	rgbFirst;
	RwRGBA	rgbSecond;

	switch( nLayer )
	{
	case ASkySetting::CLOUD_FIRST:
		{
			if( pFirstInfo )
			{
				RwRGBASet(	&rgbFirst														,
							pFirstInfo->aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.red	,
							pFirstInfo->aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.green,
							pFirstInfo->aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.blue	,
							(UINT8)( (( FLOAT ) ( pFirstInfo->aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.alpha ) * ( ( FLOAT )value / 256.0f ) ) ) );
				ModifyAtomic( &m_aAtomicCloud[ FIRST_CLOUD ]			, 
					pFirstInfo->aCloud[ ASkySetting::CLOUD_FIRST	].nTexture 	, &rgbFirst		);
			}
			else
			{
				RwRGBASet(	&rgbFirst	, 255	, 255	,255	, 0 );
				ModifyAtomic( &m_aAtomicCloud[ FIRST_CLOUD ]		, -1	, &rgbFirst	);
			}

			if( pSecondInfo )
			{
				RwRGBASet(	&rgbSecond														,
							pSecondInfo->aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.red	,
							pSecondInfo->aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.green,
							pSecondInfo->aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.blue	,
							(UINT8)( (( FLOAT ) ( pSecondInfo->aCloud[ ASkySetting::CLOUD_FIRST	].rgbColor.alpha ) * ( ( FLOAT )( 255 - value ) / 256.0f ) ) ) );
				ModifyAtomic( &m_aAtomicCloud[ FIRST_CLOUD_FADING ]		, 
					pSecondInfo->aCloud[ ASkySetting::CLOUD_FIRST	].nTexture , &rgbSecond	);
			}
			else
			{
				RwRGBASet(	&rgbSecond	, 255	, 255	,255	, 0 );
				ModifyAtomic( &m_aAtomicCloud[ FIRST_CLOUD_FADING ]		, -1	, &rgbSecond	);
			}
		}
		break;
	case ASkySetting::CLOUD_SECOND:
		{
			if( pFirstInfo )
			{
				RwRGBASet(	&rgbFirst														,
							pFirstInfo->aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.red	,
							pFirstInfo->aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.green,
							pFirstInfo->aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.blue	,
							(UINT8)( (( FLOAT ) ( pFirstInfo->aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.alpha ) * ( ( FLOAT )value / 256.0f ) ) ) );
				ModifyAtomic( &m_aAtomicCloud[ SECOND_CLOUD ]			, 
					pFirstInfo->aCloud[ ASkySetting::CLOUD_SECOND	].nTexture 	, &rgbFirst		);
			}
			else
			{
				RwRGBASet(	&rgbFirst	, 255	, 255	,255	, 0 );
				ModifyAtomic( &m_aAtomicCloud[ SECOND_CLOUD ]		, -1	, &rgbFirst	);
			}

			if( pSecondInfo )
			{
				RwRGBASet(	&rgbSecond														,
							pSecondInfo->aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.red	,
							pSecondInfo->aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.green,
							pSecondInfo->aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.blue	,
							(UINT8)( (( FLOAT ) ( pSecondInfo->aCloud[ ASkySetting::CLOUD_SECOND	].rgbColor.alpha ) * ( ( FLOAT )( 255 - value ) / 256.0f ) ) ) );
				ModifyAtomic( &m_aAtomicCloud[ SECOND_CLOUD_FADING ]		, 
					pSecondInfo->aCloud[ ASkySetting::CLOUD_SECOND	].nTexture , &rgbSecond	);
			}
			else
			{
				RwRGBASet(	&rgbSecond	, 255	, 255	,255	, 0 );
				ModifyAtomic( &m_aAtomicCloud[ SECOND_CLOUD_FADING ]		, -1	, &rgbSecond	);
			}
		}
		break;
	}
	
	return TRUE;
}

BOOL	AgcmEventNature::SetSun( ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo 	, FLOAT fAlpha			)
{
	ASSERT( 0.0f <= fAlpha && 1.0f >= fAlpha						);

	unsigned char value = ( unsigned char ) ( fAlpha * 255.0f ) % 256;
	RwRGBA	rgbFirst;
	RwRGBA	rgbSecond;

	if( pFirstInfo )
	{
		RwRGBASet(	&rgbFirst							,
					pFirstInfo->rgbSun.red		,
					pFirstInfo->rgbSun.green	,
					pFirstInfo->rgbSun.blue	,
					(UINT8)( (( FLOAT ) ( pFirstInfo->rgbSun.alpha ) * ( ( FLOAT )value / 256.0f ) ) ) );
		ModifyAtomic( &m_aAtomicSun[ FORE_TEXTURE ]	, pFirstInfo ? pFirstInfo->nSunTexture : -1	, &rgbFirst			);
	}
	else
	{
		RwRGBASet(	&rgbFirst	, 255 , 255 , 255 , 0 );
		ModifyAtomic( &m_aAtomicSun[ FORE_TEXTURE ]	,  -1, &rgbFirst	);
	}

	if( pSecondInfo )
	{
		RwRGBASet(	&rgbSecond							,
					pSecondInfo->rgbSun.red	,
					pSecondInfo->rgbSun.green	,
					pSecondInfo->rgbSun.blue	,
					(UINT8)( (( FLOAT ) ( pSecondInfo->rgbSun.alpha ) * ( ( FLOAT )( 255 - value ) / 256.0f ) ) ) );

		ModifyAtomic( &m_aAtomicSun[ BACK_TEXTURE ]	, pSecondInfo ? pSecondInfo->nSunTexture : -1	, &rgbSecond	);
	}
	else
	{
		RwRGBASet(	&rgbSecond	, 255 , 255 , 255 , 0 );
		ModifyAtomic( &m_aAtomicSun[ BACK_TEXTURE ]	,  -1, &rgbSecond	);
	}

	return TRUE;
}

BOOL	AgcmEventNature::SetCircumstance	( ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo , FLOAT fAlpha			)
{
	ASSERT( 0.0f <= fAlpha && 1.0f >= fAlpha						);

	unsigned char value = ( unsigned char ) ( fAlpha * 255.0f ) % 256;
	RwRGBA	rgbFirst;
	RwRGBA	rgbSecond;

	if( pFirstInfo )
	{
		RwRGBASet(	&rgbFirst							,
					pFirstInfo->rgbCircumstance.red		,
					pFirstInfo->rgbCircumstance.green	,
					pFirstInfo->rgbCircumstance.blue	,
					(UINT8)( (( FLOAT ) ( pFirstInfo->rgbCircumstance.alpha ) * ( ( FLOAT )value / 256.0f ) ) ) );
		ModifyAtomic( &m_aAtomicCircustance[ FORE_TEXTURE ]	, pFirstInfo ? pFirstInfo->nCircumstanceTexture : -1	, &rgbFirst			);
	}
	else
	{
		RwRGBASet(	&rgbFirst	, 255 , 255 , 255 , 0 );
		ModifyAtomic( &m_aAtomicCircustance[ FORE_TEXTURE ]	,  -1, &rgbFirst	);
	}

	if( pSecondInfo )
	{
		RwRGBASet(	&rgbSecond							,
					pSecondInfo->rgbCircumstance.red	,
					pSecondInfo->rgbCircumstance.green	,
					pSecondInfo->rgbCircumstance.blue	,
					(UINT8)( (( FLOAT ) ( pSecondInfo->rgbCircumstance.alpha ) * ( ( FLOAT )( 255 - value ) / 256.0f ) ) ) );

		ModifyAtomic( &m_aAtomicCircustance[ BACK_TEXTURE ]	, pSecondInfo ? pSecondInfo->nCircumstanceTexture : -1	, &rgbSecond	);
	}
	else
	{
		RwRGBASet(	&rgbSecond	, 255 , 255 , 255 , 0 );
		ModifyAtomic( &m_aAtomicCircustance[ BACK_TEXTURE ]	,  -1, &rgbSecond	);
	}

	return TRUE;
}


BOOL	AgcmEventNature::FlowCloud		( INT32 nLayer , FLOAT	fAmount	)	// 구름 흘러가기..
{
	ASSERT( 0 <= nLayer && nLayer < CLOUD_ATOMIC_LAYER_MAX );

	// 구름 아토믹의 UV에니메이션..
	// UV를 특정 량 만큼 밀어 버린다..

	RwIm3DVertex	*	pVertex	;
	INT32				nVertex	;
	INT32				nIndex	;

	FLOAT				v		;
	FLOAT				fOffset	;

	if( m_aAtomicCloud[ nLayer ].IsAvailable() )
	{
		VERIFY( pVertex	= m_aAtomicCloud[ nLayer ].GetVertexBuffer()	);
		nVertex	= m_aAtomicCloud[ nLayer ].GetVertexCount()	;

		v		= pVertex[ 0 ].v + fAmount	;
		fOffset	= ( FLOAT ) ( (INT32) v )	;	// 소수점 제거..
		v		-= fOffset					;
		RwIm3DVertexSetV	( &pVertex[ 0 ] , v );

		for( nIndex = 1 ; nIndex < nVertex ; ++ nIndex )
		{
			v	= pVertex[ nIndex ].v + fAmount	;
			v	-= fOffset						;
			RwIm3DVertexSetV	( &pVertex[ nIndex ] , v );
		}
	}

	return TRUE;
}

BOOL	AgcmEventNature::ModifyAtomic	( MagImAtomic * pAtomic		, INT32 nTexture			, RwRGBA * pRgbColor	)
{
	PROFILE("AgcmEventNature::ModifyAtomic");
	ASSERT( NULL != pAtomic );

	RwIm3DVertex *	pVertex = pAtomic->GetVertexBuffer();
	INT32			nVertex	= pAtomic->GetVertexCount()	;

	if( pRgbColor->alpha == 0 )
	{
		pAtomic->SetVisible( FALSE	);
		return TRUE;
	}
	else
		pAtomic->SetVisible( TRUE	);

	{
		PROFILE("AgcmEventNature::ModifyAtomic-RwIm3DVertexSetRGBA");
		do
		{
			RwIm3DVertexSetRGBA	( &pVertex[ nVertex - 1 ] , pRgbColor->red , pRgbColor->green , pRgbColor->blue , pRgbColor->alpha		);
		}
		while( --nVertex );
	}

	// 머티리얼 변경..
	{
		PROFILE("AgcmEventNature::ModifyAtomic-Material Change");

		if( pAtomic->GetTextureIndex() == nTexture )
		{
			// do nothing..
		}
		else
		{
			RwTexture * pTexture = GetTexture( nTexture );
			pAtomic->SetTexture( pTexture , nTexture );
			// Ref Count Down
			if( pTexture )
			{
				RwTextureDestroy( pTexture );
				pTexture = NULL;
			}
		}
	}

	return TRUE;
}


void	AgcmEventNature::SetSkyHeight	( FLOAT height				)
{
	// AgcmEventNature::SKY_HEIGHT		= height;
}	

BOOL	AgcmEventNature::EnableFog(BOOL bEnable)
{
	RwRenderStateSet		( rwRENDERSTATEFOGENABLE	, (void *) bEnable	);
	
	m_bFogOn = bEnable;
	g_pEngine->m_bFogEnable = bEnable;

	return TRUE;
}

BOOL	AgcmEventNature::SetFogSetting		()
{
	// 포그 설정을 적용한다..
	if( GetFogUpdate() )
	{
		// 플래그 끔..
		m_bFogUpdate	= FALSE;
		// 포그 설정을 업데이트함...
		RwUInt32				red, green, blue, alpha;

		red		= m_stFogColor.red		;
		green	= m_stFogColor.green	;
		blue	= m_stFogColor.blue		;
		alpha	= m_stFogColor.alpha	;

		// 일단 Fog 버그 잡기전에 Disable 시킨다. (Parn)
		// 포그를 끄지 않는다.
		// RwRenderStateSet		( rwRENDERSTATEFOGENABLE	, (void *) m_bFogOn					);
		RwRenderStateSet		( rwRENDERSTATEFOGCOLOR		, (void *) RWRGBALONG(m_stFogColor.red, m_stFogColor.green, m_stFogColor.blue, m_stFogColor.alpha) );
		RwRenderStateSet		( rwRENDERSTATEFOGTYPE		, (void *) rwFOGTYPELINEAR				);		

		// RwCameraSetFogDistance	( g_pEngine->m_pCamera , m_fFogDistance	);

		// CameraBeginUpdata에서 fogend를 camera farclip으로 set하므로
	}
	return TRUE;
}

BOOL	AgcmEventNature::OnSkySetCreate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventNature	*pThis			= (AgcmEventNature *)(pClass);
	AgpdSkySet		*pstAgpdSkySet	= (AgpdSkySet *)(pData);
	AgcdSkySet		*pstAgcdSkySet	= pThis->GetSkySetClientData(pstAgpdSkySet);

	memset( pstAgcdSkySet , 0 , sizeof(AgcdSkySet) );

	for( int i = 0 ; i < ASkySetting::TS_MAX ; ++ i )
	{
		pstAgcdSkySet->m_aSkySetting[ i ].Reset();
	}

	return TRUE;
}

BOOL	AgcmEventNature::OnSkySetDestroy(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventNature	*pThis			= (AgcmEventNature *)(pClass);
	AgpdSkySet		*pstAgpdSkySet	= (AgpdSkySet *)(pData);
	AgcdSkySet		*pstAgcdSkySet	= pThis->GetSkySetClientData(pstAgpdSkySet);

	// GG..
	// 별로 신경쓸거 없심...

	return TRUE;
}

BOOL	AgcmEventNature::SkySetStreamWriteCB	( PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgcmEventNature	*pThis			= (AgcmEventNature *)(pClass);
	AgpdSkySet		*pstAgpdSkySet	= (AgpdSkySet *)(pData);
	AgcdSkySet		*pstAgcdSkySet	= pThis->GetSkySetClientData(pstAgpdSkySet);
	ASkySetting		*pSkySetting	;
	
	char	szTemp[ 256 ];
	char	strKey[ 256 ];
	int		nTimeIndex = 0;

	for( int i = 0 ; i < ASkySetting::TS_MAX ; ++ i )
	{
		pSkySetting = & pstAgcdSkySet->m_aSkySetting[ i ];

		nTimeIndex = ASkySetting::GetRealTime( i );

		#ifdef _DEBUG
		if( !pSkySetting->nDataAvailableFlag )
		{
			TRACE( "저장할 스카이템플릿에 정보가 없습니다." );
		}
		#endif

		if( pSkySetting->nDataAvailableFlag & ASkySetting::DA_LIGHT )
		{
			// 라이트 부분만 저장함..
			sprintf( strKey , g_strINISettingKeySkyTopColor , nTimeIndex );
			sprintf( szTemp, "%f,%f,%f",
				pSkySetting->rgbSkyTop.red	,
				pSkySetting->rgbSkyTop.green,
				pSkySetting->rgbSkyTop.blue	);

			if (!pStream->WriteValue(strKey, szTemp) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeySkyBottomColor , nTimeIndex );
			sprintf( szTemp, "%f,%f,%f",
				pSkySetting->rgbSkyBottom.red	,
				pSkySetting->rgbSkyBottom.green,
				pSkySetting->rgbSkyBottom.blue	);

			if (!pStream->WriteValue(strKey, szTemp) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeyAmbientColor , nTimeIndex );
			sprintf( szTemp, "%f,%f,%f",
				pSkySetting->rgbAmbient.red	,
				pSkySetting->rgbAmbient.green,
				pSkySetting->rgbAmbient.blue	);

			if (!pStream->WriteValue(strKey, szTemp) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeyDirectionalColor , nTimeIndex );
			sprintf( szTemp, "%f,%f,%f",
				pSkySetting->rgbDirectional.red	,
				pSkySetting->rgbDirectional.green,
				pSkySetting->rgbDirectional.blue	);

			if (!pStream->WriteValue(strKey, szTemp) )
				return FALSE;
		}

		if( pSkySetting->nDataAvailableFlag & ASkySetting::DA_CLOUD )
		{
			// 클라우드 부분만 저장함..

			sprintf( strKey , g_strINISettingKeyCloudTexture1 , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->aCloud[ ASkySetting::CLOUD_FIRST ].nTexture	) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeyCloudSpeed1 , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->aCloud[ ASkySetting::CLOUD_FIRST ].fSpeed	) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeyCloudColor1 , nTimeIndex );
			sprintf( szTemp, "%d,%d,%d,%d",
				pSkySetting->aCloud[ ASkySetting::CLOUD_FIRST ].rgbColor.red	,
				pSkySetting->aCloud[ ASkySetting::CLOUD_FIRST ].rgbColor.green	,
				pSkySetting->aCloud[ ASkySetting::CLOUD_FIRST ].rgbColor.blue	,
				pSkySetting->aCloud[ ASkySetting::CLOUD_FIRST ].rgbColor.alpha	);
			if (!pStream->WriteValue( strKey , szTemp ) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeyCloudTexture2 , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->aCloud[ ASkySetting::CLOUD_SECOND ].nTexture	) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeyCloudSpeed2 , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->aCloud[ ASkySetting::CLOUD_SECOND ].fSpeed	) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeyCloudColor2 , nTimeIndex );
			sprintf( szTemp, "%d,%d,%d,%d",
				pSkySetting->aCloud[ ASkySetting::CLOUD_SECOND ].rgbColor.red	,
				pSkySetting->aCloud[ ASkySetting::CLOUD_SECOND ].rgbColor.green	,
				pSkySetting->aCloud[ ASkySetting::CLOUD_SECOND ].rgbColor.blue	,
				pSkySetting->aCloud[ ASkySetting::CLOUD_SECOND ].rgbColor.alpha	);
			if (!pStream->WriteValue( strKey , szTemp ) )
				return FALSE;

			// Circumstance 정보..
			sprintf( strKey , g_strINISettingCircumstanceTexture , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->nCircumstanceTexture) )
				return FALSE;

			sprintf( strKey , g_strINISettingCircumstanceColor , nTimeIndex );
			sprintf( szTemp, "%d,%d,%d,%d",
				pSkySetting->rgbCircumstance.red	,
				pSkySetting->rgbCircumstance.green	,
				pSkySetting->rgbCircumstance.blue	,
				pSkySetting->rgbCircumstance.alpha	);
			if (!pStream->WriteValue( strKey , szTemp ) )
				return FALSE;

			sprintf( strKey , g_strINISettingSunTexture , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->nSunTexture) )
				return FALSE;

			sprintf( strKey , g_strINISettingSunColor , nTimeIndex );
			sprintf( szTemp, "%d,%d,%d,%d",
				pSkySetting->rgbSun.red	,
				pSkySetting->rgbSun.green	,
				pSkySetting->rgbSun.blue	,
				pSkySetting->rgbSun.alpha	);
			if (!pStream->WriteValue( strKey , szTemp ) )
				return FALSE;

		}

		if( pSkySetting->nDataAvailableFlag & ASkySetting::DA_FOG )
		{
			sprintf( strKey , g_strINISettingKeyFogColor , nTimeIndex );
			sprintf( szTemp, "%f,%f,%f",
				pSkySetting->rgbFog.red	,
				pSkySetting->rgbFog.green,
				pSkySetting->rgbFog.blue	);

			if (!pStream->WriteValue(strKey, szTemp) )
				return FALSE;

			sprintf( strKey , g_strINISettingKeyFogDistance , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->fFogDistance) )
				return FALSE;
			sprintf( strKey , g_strINISettingKeyFogFarClip , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->fFogFarClip) )
				return FALSE;
		}


		if( pSkySetting->nDataAvailableFlag & ASkySetting::DA_EFFECT )
		{
			sprintf( strKey , g_strINISettingEffectInterval , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->nEffectInterval) )
				return FALSE;

			INT32	nIndex;

			for( nIndex = 0 ; nIndex < AGCMEVENTNATURE_MAX_EVENT ; ++ nIndex )
			{
				sprintf( strKey , g_strINISettingEffectIndex , nTimeIndex , nIndex + 1 );
				
				if (!pStream->WriteValue( strKey , pSkySetting->aEffectArray[ nIndex ] ) )
					return FALSE;
			}

			for( nIndex = 0 ; nIndex < AGCMEVENTNATURE_MAX_EVENT ; ++ nIndex )
			{
				sprintf( strKey , g_strINISettingEffectRate , nTimeIndex , nIndex + 1 );
				
				if (!pStream->WriteValue( strKey , pSkySetting->aEffectRateArray[ nIndex ] ) )
					return FALSE;
			}
		}

		if( pSkySetting->nDataAvailableFlag & ASkySetting::DA_MUSIC )
		{
			// 배경음악..
			sprintf( strKey , g_strINISettingBackGroundMusic , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->strBackMusic ) )
				return FALSE;
		}

		if( pSkySetting->nDataAvailableFlag & ASkySetting::DA_WEATHER )
		{
			// 배경음악..
			sprintf( strKey , g_strINISettingWeatherEffect1 , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->nWeatherEffect1 ) )
				return FALSE;
			sprintf( strKey , g_strINISettingWeatherEffect2 , nTimeIndex );
			if (!pStream->WriteValue( strKey , pSkySetting->nWeatherEffect2 ) )
				return FALSE;
		}
	}

	return TRUE;
}

inline	BOOL __strcmpTimeValue( const char * pStr , char * pFmt , INT32 nTime )
{
	char	str[ 256 ];

	wsprintf( str , pFmt , nTime );

	return strcmp( pStr , str );
}

inline	BOOL __strcmpTimeValue( const char * pStr , char * pFmt , INT32 nTime , INT32 nParam1 )
{
	char	str[ 256 ];

	wsprintf( str , pFmt , nTime , nParam1 );

	return strcmp( pStr , str );
}

inline BOOL __ParseColorValue( char * szStr , FLOAT * pRed , FLOAT * pGreen , FLOAT * pBlue )
{
	sscanf(szStr, "%f,%f,%f", pRed , pGreen , pBlue );
	return TRUE;
}

inline BOOL __ParseColorValue( char * szStr , RwRGBA * pRGBA )
{
	sscanf(szStr, "%d,%d,%d,%d", &pRGBA->red , &pRGBA->green , &pRGBA->blue , &pRGBA->alpha );
	return TRUE;
}

BOOL	AgcmEventNature::SkySetStreamReadCB		( PVOID pData, ApModule *pClass, ApModuleStream *pStream)
{
	AgcmEventNature	*pThis			= (AgcmEventNature *)(pClass);
	AgpdSkySet		*pstAgpdSkySet	= (AgpdSkySet *)(pData);
	AgcdSkySet		*pstAgcdSkySet	= pThis->GetSkySetClientData(pstAgpdSkySet);
	
	const CHAR *szValueName;
	CHAR	szValue[128];
	//INT32	nGroupCount;

	INT32	nTimeIndex	;
	
	while(pStream->ReadNextValue())
	{
		szValueName = pStream->GetValueName();

		nTimeIndex	= atoi( szValueName );
		ASkySetting::TimeSet	eTimeSet = ASkySetting::TS_MAX;

		switch( nTimeIndex )
		{
		case 0	: eTimeSet = ASkySetting::TS_NIGHT2; break;
		case 1	: break;
		case 2	: break;
		case 3	: break;
		case 4	: break;
		case 5	: eTimeSet = ASkySetting::TS_DAWN; break;
		case 6	: break;
		case 7	: eTimeSet = ASkySetting::TS_DAYTIME1; break;
		case 8	: break;
		case 9	: break;
		case 10 : break;
		case 11	: break;
		case 12	: break;
		case 13	: eTimeSet = ASkySetting::TS_DAYTIME2; break;
		case 14	: break;
		case 15	: break;
		case 16	: break;
		case 17	: break;
		case 18	: break;
		case 19	: eTimeSet = ASkySetting::TS_EVENING; break;
		case 20	: break;
		case 21	: eTimeSet = ASkySetting::TS_NIGHT1; break;
		case 22	: break;
		case 23	: break;
		}

		if( eTimeSet >= ASkySetting::TS_MAX ) continue;

		ASSERT( 0 <= nTimeIndex && nTimeIndex < 24 );

		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeySkyTopColor , nTimeIndex ) )
		{
			// 데이타를 읽어들일때 , 해당 당 플래그를 같이 설정한다.
			// 그래서 데이타가 존재하는지 알 수 있다.
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_LIGHT;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue ,	&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbSkyTop.red	,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbSkyTop.green	,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbSkyTop.blue	);
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeySkyBottomColor , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_LIGHT;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue ,	&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbSkyBottom.red	,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbSkyBottom.green	,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbSkyBottom.blue	);
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyAmbientColor , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_LIGHT;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue ,	&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbAmbient.red		,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbAmbient.green	,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbAmbient.blue	);
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyDirectionalColor , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_LIGHT;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue ,	&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbDirectional.red		,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbDirectional.green	,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbDirectional.blue	);
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyCloudTexture1 , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].aCloud[ ASkySetting::CLOUD_FIRST ].nTexture );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyCloudSpeed1 , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].aCloud[ ASkySetting::CLOUD_FIRST ].fSpeed );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyCloudColor1 , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue , &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].aCloud[ ASkySetting::CLOUD_FIRST ].rgbColor );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyCloudTexture2 , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].aCloud[ ASkySetting::CLOUD_SECOND ].nTexture );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyCloudSpeed2 , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].aCloud[ ASkySetting::CLOUD_SECOND ].fSpeed );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyCloudColor2 , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue , &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].aCloud[ ASkySetting::CLOUD_SECOND ].rgbColor );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyFogColor , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_FOG;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue ,	&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbFog.red	,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbFog.green	,
											&pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbFog.blue	);
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyFogDistance , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_FOG;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].fFogDistance );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingKeyFogFarClip , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_FOG;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].fFogFarClip );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingCircumstanceTexture , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nCircumstanceTexture );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingCircumstanceColor , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue , &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbCircumstance );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingSunTexture , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nSunTexture );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingSunColor , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_CLOUD;
			pStream->GetValue( szValue , 128 );
			__ParseColorValue( szValue , &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].rgbSun );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingEffectInterval , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_EFFECT;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nEffectInterval );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingBackGroundMusic , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_MUSIC;
			pStream->GetValue( pstAgcdSkySet->m_aSkySetting[ eTimeSet ].strBackMusic , EFF2_FILE_NAME_MAX );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingWeatherEffect1 , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_WEATHER;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nWeatherEffect1  );
		}
		else
		if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingWeatherEffect2 , nTimeIndex ) )
		{
			pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_WEATHER;
			pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nWeatherEffect2  );
		}
		else
		{
			int	nIndex;

			for( nIndex = 0 ; nIndex < AGCMEVENTNATURE_MAX_EVENT ; ++ nIndex )
			{
				if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingEffectIndex , nTimeIndex , nIndex + 1 ) )
				{
					pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_EFFECT;
					pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].aEffectArray[ nIndex ] );
				}
			}
			
			for( nIndex = 0 ; nIndex < AGCMEVENTNATURE_MAX_EVENT ; ++ nIndex )
			{
				if (!__strcmpTimeValue( szValueName , ( char * ) g_strINISettingEffectRate , nTimeIndex , nIndex + 1 ) )
				{
					pstAgcdSkySet->m_aSkySetting[ eTimeSet ].nDataAvailableFlag |= ASkySetting::DA_EFFECT;
					pStream->GetValue( &pstAgcdSkySet->m_aSkySetting[ eTimeSet ].aEffectRateArray[ nIndex ] );
				}
			}
		}
	}

// 마고자 (2003-02-04 오전 7:41:49) : 
	return TRUE; // pThis->SetupObjectClump(pstApdObject);

}

AgcdSkySet *	AgcmEventNature::GetSkySetClientData( AgpdSkySet * pstSkySet )
{
	return (AgcdSkySet *) m_pcsAgpmEventNature->GetAttachedModuleData(
		m_nSkySetAttachIndex, pstSkySet);
}

BOOL	AgcmEventNature::SetNextWeather( ASkySetting::TimeSet eTimeSet , INT32 nDuration , BOOL bForce )
{
	ASSERT( eTimeSet < ASkySetting::TS_MAX );

	// 현재 강제다..
	// bForce = TRUE;

	ASkySetting * pSkySet = &m_skyMixedUp[ eTimeSet ];

	ASSERT( NULL != pSkySet					);
	ASSERT( NULL != m_pcsAgpmEventNature	);

	if( bForce )
	{
		// 강제다;..
		// 바로 변화적용 시켜야 하는경우..

		// 시작시간 저장..
		m_bSkyIsChanging		= FALSE									;
		// m_nSkyChangeStartTime	= m_pcsAgpmEventNature->GetGameTime()	;

		// memcpy( &m_skyCurrentSetting[ CURRENT_SKY ] , pSkySet , sizeof ASkySetting );

		m_skyCurrentSetting[ CURRENT_SKY ].nDataAvailableFlag = ASkySetting::DA_ALL;

		// 부분 부분 카피해넣음..

		if( pSkySet->nDataAvailableFlag & ASkySetting::DA_LIGHT	)
		{
			m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyTop		= pSkySet->rgbSkyTop			;
			m_skyCurrentSetting[ CURRENT_SKY ].rgbSkyBottom		= pSkySet->rgbSkyBottom		;
			m_skyCurrentSetting[ CURRENT_SKY ].rgbAmbient		= pSkySet->rgbAmbient		;
			m_skyCurrentSetting[ CURRENT_SKY ].rgbDirectional	= pSkySet->rgbDirectional	;
		}
		if( pSkySet->nDataAvailableFlag & ASkySetting::DA_FOG		)
		{
			m_skyCurrentSetting[ CURRENT_SKY ].rgbFog			= pSkySet->rgbFog			;
			m_skyCurrentSetting[ CURRENT_SKY ].fFogDistance		= pSkySet->fFogDistance	;
			m_skyCurrentSetting[ CURRENT_SKY ].fFogFarClip		= pSkySet->fFogFarClip	;
		}
		
		if( pSkySet->nDataAvailableFlag & ASkySetting::DA_CLOUD	)
		{
			m_skyCurrentSetting[ CURRENT_SKY ].aCloud[ ASkySetting::CLOUD_FIRST		]	= pSkySet->aCloud[ ASkySetting::CLOUD_FIRST	]	;
			m_skyCurrentSetting[ CURRENT_SKY ].aCloud[ASkySetting:: CLOUD_SECOND	]	= pSkySet->aCloud[ ASkySetting::CLOUD_SECOND	]	;
			m_skyCurrentSetting[ CURRENT_SKY ].nCircumstanceTexture						= pSkySet->nCircumstanceTexture		;
			m_skyCurrentSetting[ CURRENT_SKY ].rgbCircumstance							= pSkySet->rgbCircumstance			;
			m_skyCurrentSetting[ CURRENT_SKY ].nSunTexture								= pSkySet->nSunTexture				;
			m_skyCurrentSetting[ CURRENT_SKY ].rgbSun									= pSkySet->rgbSun					;
		}

		if( pSkySet->nDataAvailableFlag & ASkySetting::DA_EFFECT	)
		{
			m_skyCurrentSetting[ CURRENT_SKY ].nEffectInterval	= pSkySet->nEffectInterval;
		
			for( int k = 0 ; k < AGCMEVENTNATURE_MAX_EVENT ; ++ k )
			{
				m_skyCurrentSetting[ CURRENT_SKY ].aEffectArray		[ k ]	= pSkySet->aEffectArray		[ k ];
				m_skyCurrentSetting[ CURRENT_SKY ].aEffectRateArray	[ k ]	= pSkySet->aEffectRateArray	[ k ];
			}
		}
		
		if( pSkySet->nDataAvailableFlag & ASkySetting::DA_MUSIC	)
		{
			strncpy( m_skyCurrentSetting[ CURRENT_SKY ].strBackMusic , 
						pSkySet->strBackMusic	, EFF2_FILE_NAME_MAX );
		}

		if( pSkySet->nDataAvailableFlag & ASkySetting::DA_WEATHER	)
		{
			m_skyCurrentSetting[ CURRENT_SKY ].nWeatherEffect1	= pSkySet->nWeatherEffect1;
			m_skyCurrentSetting[ CURRENT_SKY ].nWeatherEffect2	= pSkySet->nWeatherEffect2;
		}

		m_skyCurrentSetting[ CURRENT_SKY ].bAvailable	= TRUE	;
		m_skyCurrentSetting[ NEXT_SKY ].bAvailable		= FALSE	;
		m_skyCurrentSetting[ NEXT_SKY ].nDuration		= 1		;

		// 설정 끝~.

		// 혹시 모르니.. wait 부분 초기화.
		m_skyCurrentSetting[ WAIT_SKY ].bAvailable	= FALSE		;

		// 마고자 (2004-03-09 오전 11:22:21) : 임시 삽입.. 강제 이벤트 호출. 
		// 현재 시간이 가지 않기 때문에 , 정상적인 시간변화에 따른 이벤트 변화가없다..
		SkyChange( GetCurrentSkySetting	() );

		return TRUE;
	}
	else
	{
		// 마고자 (2004-06-17 오후 4:52:02) : 암비언스와 배경음악은 바로 바뀌게 한다.
		if( pSkySet->nDataAvailableFlag & ASkySetting::DA_EFFECT	)
		{
			m_skyCurrentSetting[ CURRENT_SKY ].nEffectInterval	= pSkySet->nEffectInterval;
		
			for( int k = 0 ; k < AGCMEVENTNATURE_MAX_EVENT ; ++ k )
			{
				m_skyCurrentSetting[ CURRENT_SKY ].aEffectArray		[ k ]	= pSkySet->aEffectArray		[ k ];
				m_skyCurrentSetting[ CURRENT_SKY ].aEffectRateArray	[ k ]	= pSkySet->aEffectRateArray	[ k ];
			}
		}
		if( pSkySet->nDataAvailableFlag & ASkySetting::DA_MUSIC	)
		{
			strncpy( m_skyCurrentSetting[ CURRENT_SKY ].strBackMusic , 
						pSkySet->strBackMusic	, EFF2_FILE_NAME_MAX );
		}

		if( IsSkyChanging() )
		{
			// Waiting Sky 에다가 삽입함..
			// 기존에 있는 경우는 덮어 써버림..
			memcpy( &m_skyCurrentSetting[ WAIT_SKY ] , pSkySet , sizeof ASkySetting );
			m_skyCurrentSetting[ WAIT_SKY ].bAvailable	= TRUE		;
			m_skyCurrentSetting[ WAIT_SKY ].nDuration	= nDuration	;

			// 그리곤 대기함..

			TRACE( "스카이 변화중이므로 변화 대기 시켜둡니다.\n" );
			return TRUE;
		}
		else
		{
			// 바로 변화적용 시켜야 하는경우..

			// 시작시간 저장..
			m_bSkyIsChanging		= TRUE									;

			// 마고자 (2004-04-02 오후 12:31:04) : 
			// 게임타임 적용 GetTickCount로 변경..
			// m_nSkyChangeStartTime	= m_pcsAgpmEventNature->GetGameTime()	;
			m_nSkyChangeStartTime	= m_nCurrentTime	;

			memcpy( &m_skyCurrentSetting[ NEXT_SKY ] , pSkySet , sizeof ASkySetting );
			m_skyCurrentSetting[ NEXT_SKY ].bAvailable	= TRUE		;
			m_skyCurrentSetting[ NEXT_SKY ].nDuration	= nDuration	;

			// 설정 끝~.

			// 혹시 모르니.. wait 부분 초기화.
			m_skyCurrentSetting[ WAIT_SKY ].bAvailable	= FALSE		;
			
			return TRUE;
		}
	}
}

BOOL	AgcmEventNature::ResampleTexture		( char * pSource , char * pDestination , INT32 nWidth , INT32 nHeight )
{
	RwImage	* pImage;
	pImage = RwImageRead ( pSource );

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

		RwImage	* pReSample;
		pReSample = RwImageCreateResample( pImage , nWidth , nHeight );

		ASSERT( NULL != pReSample );

		RwImageWrite( pReSample , pDestination );
		RwImageDestroy( pImage		);
		RwImageDestroy( pReSample	);
		return TRUE;
	}	
}

void	AgcmEventNature::UpdateProjectionMatrix(RwCamera*	camera)
{
	//
    //  Projection matrix
    //
	D3DMATRIX	proj;
    proj.m[0][0] = camera->recipViewWindow.x;
	proj.m[0][1] = 0.0f;
	proj.m[0][2] = 0.0f;
	proj.m[0][3] = 0.0f;

	proj.m[1][0] = 0.0f;
    proj.m[1][1] = camera->recipViewWindow.y;
	proj.m[1][2] = 0.0f;
	proj.m[1][3] = 0.0f;
	
    // Shear X, Y by view offset with Z invariant 
    proj.m[2][0] = camera->recipViewWindow.x * camera->viewOffset.x;
    proj.m[2][1] = camera->recipViewWindow.y * camera->viewOffset.y;

    // Translate to shear origin 
    proj.m[3][0] = -camera->recipViewWindow.x * camera->viewOffset.x;
    proj.m[3][1] = -camera->recipViewWindow.y * camera->viewOffset.y;

    // Projection type 
    if (camera->projectionType == rwPARALLEL)
    {
        proj.m[2][2] = 1.0f / (camera->farPlane - camera->nearPlane);
        proj.m[2][3] = 0.0f;
        proj.m[3][3] = 1.0f;
    }
    else
    {
        proj.m[2][2] = camera->farPlane / (camera->farPlane - camera->nearPlane);
        proj.m[2][3] = 1.0f;
        proj.m[3][3] = 0.0f;
    }

    proj.m[3][2] = -proj.m[2][2] * camera->nearPlane;

    RwD3D9SetTransform(D3DTS_PROJECTION, &proj);
}

void	AgcmEventNature::RenderSky		()
{
	PROFILE("AgcmEventNature::RenderSky()");

	if( IsShowSky() )
	{
		LockFrame();

		D3DMATRIX	oldproj;
		RwD3D9GetTransform(D3DTS_PROJECTION, &oldproj);

		/*	gemani
		// End update
		RwCameraEndUpdate( g_pEngine->m_pCamera );

		//임시로 넓게..
		//g_pEngine->SetWorldCameraFarClipPlane	( AgcmEventNature::SKY_RADIUS	);

		RwCameraSetFarClipPlane( g_pEngine->m_pCamera , AgcmEventNature::SKY_RADIUS	);
		if (RwCameraBeginUpdate( g_pEngine->m_pCamera ))
		{*/
			//int	zWriteMode;
			//RwRenderStateGet( rwRENDERSTATEZWRITEENABLE , ( void * ) &zWriteMode );

			RwCameraSetFarClipPlane( g_pEngine->m_pCamera , AgcmEventNature::SKY_RADIUS	);
			UpdateProjectionMatrix( g_pEngine->m_pCamera );

			if( m_bSkyFogDisable )
			{
				// 마고자 (2005-04-22 오후 2:21:58) : 
				// 후야사마가 이러고 싶다함 -_-;;
				RwRenderStateSet( rwRENDERSTATEZWRITEENABLE		, ( void * ) FALSE			);
				RwRenderStateSet( rwRENDERSTATEFOGENABLE, (void *) FALSE				);
				RwRenderStateSet( rwRENDERSTATETEXTUREFILTER , (void*) rwFILTERLINEAR );
			}
			
			// 일시적으로 FarClip을 변화시킴.
			// 하늘과 지형과 따로 클립이 되게 함..
			m_AtomicSky								.Render();

			RenderStar();

			m_aAtomicSun[ FORE_TEXTURE		].Render();
			m_aAtomicSun[ BACK_TEXTURE		].Render();

			m_aAtomicCloud[ FIRST_CLOUD			]	.Render();
			m_aAtomicCloud[ FIRST_CLOUD_FADING	]	.Render();
			m_aAtomicCloud[ SECOND_CLOUD		]	.Render();
			m_aAtomicCloud[ SECOND_CLOUD_FADING	]	.Render();

			//RwRenderStateSet( rwRENDERSTATEZWRITEENABLE , ( void * ) TRUE	);

			// 마고자 (2004-03-17 오후 3:42:12) : 띠폴리건을 Z버퍼에 적용돼도록 한다..
			// 지형이 끝에 짤리는 현상을 띠텍스쳐와 폴리건의 적절한 조화로..
			// 커버해보겠다는 디자인파트의 노력의 결실을 기대함.. >>후야사마.

			static BOOL	bFore = TRUE;
			static BOOL	bBack = TRUE;
			if( bFore ) m_aAtomicCircustance[ FORE_TEXTURE		]	.Render();
			if( bBack ) m_aAtomicCircustance[ BACK_TEXTURE		]	.Render();

			// 이단 Fog 버그 잡기 전에 Disable (Parn)
			if(m_bFogOn)
				RwRenderStateSet( rwRENDERSTATEFOGENABLE	, ( void * )TRUE	);

			/*RwCameraEndUpdate( g_pEngine->m_pCamera) ;
		}*/		//gemani

		// 파클립 돌려놓음..
		RwCameraSetFarClipPlane( g_pEngine->m_pCamera , m_fSkyRadius	);
		RwD3D9SetTransform(D3DTS_PROJECTION, &oldproj);
		//RwCameraBeginUpdate( g_pEngine->m_pCamera );

		RwRenderStateSet( rwRENDERSTATEZTESTENABLE		, ( void * ) TRUE);

		// Fog On일 경우 fogenable만 건드린다.. disable일 경우엔 fogsetting 안함(2004.7.21)
		if(m_bFogOn)
		{
			SetFogSetting();

			// 마고자 (2005-03-31 오후 6:25:47) : 
			// 비율 조절 기능..
			FLOAT	fFarClip;
			FLOAT	fDistance;
			if( m_fFogAdjustFarClip < m_fFogFarClip )
			{
				fFarClip	= m_fFogAdjustFarClip;
				fDistance	= m_fFogDistance * ( m_fFogAdjustFarClip / m_fFogFarClip );
			}
			else
			{
				fFarClip	= m_fFogFarClip		;
				fDistance	= m_fFogDistance	;
			}

			RwD3D9SetRenderState( D3DRS_FOGSTART	,  *( ( DWORD * ) ( &fDistance	) ) );
			RwD3D9SetRenderState( D3DRS_FOGEND		,  *( ( DWORD * ) ( &fFarClip	) ) );
			RwCameraSetFogDistance	( g_pEngine->m_pCamera , fDistance	);
			m_pcsAgcmRender->SetFogEnd( fFarClip );
		}
		
		UnlockFrame();
	}
}

BOOL	AgcmEventNature::ApplySkySetting		()
{
	// 현재 스타이 세팅을 적용시킴..
	UINT64	uCurrentTime = m_pcsAgpmEventNature->GetGameTime();

	INT32	hour , minute	;

	hour		= m_pcsAgpmEventNature->GetHour			( uCurrentTime  );
	minute		= m_pcsAgpmEventNature->GetMinute		( uCurrentTime  );

//	AgpdSkySet *	pSkySet			;
//	AgcdSkySet *	pClientSkySet	;
//	VERIFY( pSkySet	= m_pcsAgpmEventNature->GetSkySet( GetSkyTemplateID() ) );
//	VERIFY( pClientSkySet = GetSkySetClientData( pSkySet ) );
//
//	*GetCurrentSkySetting()	= pClientSkySet->m_aSkySetting[ hour ];
	
	*GetCurrentSkySetting()	= m_skyMixedUp[ hour ];
	SetTimeForce( uCurrentTime );
	return TRUE;
}

BOOL	AgcmEventNature::__SetTime			( AgcmEventNature * pEventNature , INT32 hour , INT32 minutes )
{
	ASSERT( NULL != pEventNature );

	pEventNature->m_pcsAgpmEventNature->SetTime( hour , minutes );

	return pEventNature->SetTimeForce( ( hour * 60 * 60 + minutes * 60 ) * 1000 );
}

void	AgcmEventNature::__SetFog			( AgcmEventNature * pEventNature , 
	BOOL		bFogOn			, FLOAT		fFogDistance	, FLOAT		fFogFarClip	,
	RwRGBA *	pstFogColor		)
{
	ASSERT( NULL != pEventNature );

	pEventNature->SetFog( bFogOn	, fFogDistance , fFogFarClip , pstFogColor );
}


// 이전 코드와 호완..
RpAtomic *	AgcmEventNature::CreateSkyAtomic	()
{
	// RpGeometryCreate 를 이용해서
	// 머티리얼 하나짜리 구를 생성한다.
	// 그걸 아토믹에 에드해서 리턴한다.

    RtWorldImport			*	pWorldImport	;
    RtWorldImportParameters		params			;
	RpGeometry				*	pGeometry		;
	RpAtomic				*	pAtomic			= NULL	;

    pWorldImport = _CreateHalfSphere();
    if( pWorldImport == NULL )
    {
        return NULL;
    }

    //RtWorldImportParametersInitialize(&params);
    RtWorldImportParametersInit(&params);

    params.flags					= rpWORLDTEXTURED | rpWORLDNORMALS | rpWORLDPRELIT | rpWORLDTEXTURED2 | rpWORLDMODULATEMATERIALCOLOR;
    params.conditionGeometry		= FALSE	;
    params.calcNormals				= FALSE	;
	params.numTexCoordSets			= 4		; // 멀티텍스쳐 지원.
	params.maxWorldSectorPolygons	= 2		;
	params.maxOverlapPercent		= 1		;

    VERIFY( pGeometry = RtWorldImportCreateGeometry(pWorldImport, &params) );
	if( pGeometry )
	{
		g_pEngine->LockFrame();
		pAtomic	= RpAtomicCreate();
		RpAtomicSetGeometry	( pAtomic , pGeometry , 0	);

		RpAtomicSetFrame	( pAtomic , RwFrameCreate()	);

		RpGeometryDestroy	( pGeometry					);
		g_pEngine->UnlockFrame();
	}

    RtWorldImportDestroy(pWorldImport);

	return pAtomic;
}

BOOL	AgcmEventNature::ModifyAtomic	( RpAtomic * pAtomic , INT32 nAlpha	)
{
	RpGeometry	*	pGeometry	;
	RpMaterial	*	pMaterial	;
	RwRGBA			newRGBA		;
	RwRGBA		*	pRGBA		;
		
	// First Sky 변경..

	VERIFY( pGeometry = RpAtomicGetGeometry( pAtomic ) );
	if ( !pGeometry ) return FALSE;

	RpGeometryLock( pGeometry, rpGEOMETRYLOCKPRELIGHT );
	RpGeometrySetFlags( pGeometry, RpGeometryGetFlags( pGeometry ) | rpGEOMETRYMODULATEMATERIALCOLOR );

	VERIFY( pMaterial = RpGeometryGetMaterial( pGeometry , 0 ) );
	if ( !pMaterial ) return FALSE;

	pRGBA			= (struct RwRGBA*) RpMaterialGetColor( pMaterial )	;
	newRGBA			= * pRGBA							;

	// 머티리얼 변경..
	newRGBA.alpha	= nAlpha							;

	RpMaterialSetColor		( pMaterial	, &newRGBA	);

	RpGeometryUnlock( pGeometry );

	return TRUE;
}


void	AgcmEventNature::__MoveSky		( AgcmEventNature * pEventNature )
{
	ASSERT( NULL != pEventNature );

	pEventNature->MoveSky();	
}

void	AgcmEventNature::__SetSkyRadius	( AgcmEventNature * pEventNature ,  FLOAT		fRadius		)
{
	ASSERT( NULL != pEventNature );

	pEventNature->SetSkyRadius( fRadius );	
}

INT32	AgcmEventNature::__SetSkyTemplateID	( AgcmEventNature * pEventNature , ASkySetting	* pSkySet	, INT32 nFlag )
{
	ASSERT( NULL != pEventNature );

	return pEventNature->SetSkyTemplateID( pSkySet , nFlag );	
}

BOOL	AgcmEventNature::CBOnAddObject		( PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmEventNature	*	pEventNature	= ( AgcmEventNature	* ) pClass	;
	ApdObject		*	pcsApdObject	= ( ApdObject		* ) pData	;

	// 이벤트 네이쳐 오브젝트 인지 점검.. 아닐경우 리턴..

	// 이벤트 네이쳐 정보를 얻어와서..
	// 범위를 알아냄.
	// 범위 안에 있는 섹터에 인덱스를 넣어줌..

	ASSERT( NULL != pEventNature->m_pcsApmEventManager	);

	// 이벤트 스트럭쳐도 얻어냄..
	ApdEvent *	pstEvent	;

	// EventNature Event Struct 얻어냄..
	pstEvent	= pEventNature->m_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_NATURE );

	if( pstEvent )
	{
		// 리스트 추가...
		// 섹터가 넘어가는지 검사..

		// 스카이 이벤트 발견. 
		INT32		nTemplateID	;
		FLOAT		fRadius		;

		// Template Index 
		nTemplateID = ( INT32 ) pstEvent->m_pvData;
		// 반지름..
		fRadius		= pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius;

		TRACE( "스카이 이벤트 등록..(%d)\n" , nTemplateID );

		INT32	nStartX , nStartZ , nEndX , nEndZ;

		nStartX	= PosToSectorIndexX( pcsApdObject->m_stPosition.x - fRadius );
		nEndX	= PosToSectorIndexX( pcsApdObject->m_stPosition.x + fRadius );
		nStartZ	= PosToSectorIndexZ( pcsApdObject->m_stPosition.z - fRadius );
		nEndZ	= PosToSectorIndexZ( pcsApdObject->m_stPosition.z + fRadius );

		AgpdSkySet *	pPublicSkySet	;
		AgcdSkySet *	pClientSkySet	;
		VERIFY( pPublicSkySet = pEventNature->m_pcsAgpmEventNature->GetSkySet( nTemplateID ) );
		
		if( NULL != pPublicSkySet )
		{
			ApWorldSector	*	pSector			= NULL							;
			ApWorldSector	*	pThisSector		= pEventNature->m_pcsApmMap->GetSector( pcsApdObject->m_stPosition.x , pcsApdObject->m_stPosition.z );

			VERIFY( pClientSkySet = pEventNature->GetSkySetClientData( pPublicSkySet ) );
			ASkySetting	* pSkySet = pClientSkySet->m_aSkySetting;
			SectorNatureUpdateInfo	stInfo;

			BOOL	bThisSectorOnly = TRUE;
			
			for( INT32 x = nStartX ; x <= nEndX ; ++x )
			{
				for( INT32 z = nStartZ ; z <= nEndZ ; ++z )
				{
					pSector = pEventNature->m_pcsApmMap->GetSector( x , z );

					// 업데이트가 필요한 섹터..
					if( pSector )
					{
						stInfo.uSectorX		= x;
						stInfo.uSectorZ		= z;
						stInfo.uObjectID	= pcsApdObject->m_lID;

						pEventNature->m_MutexlistUpdateSectorNatureInfo.Lock		();
						pEventNature->m_listUpdateSectorNature.AddTail( stInfo );
						pEventNature->m_MutexlistUpdateSectorNatureInfo.Unlock	();
					}

					if( pSector != pThisSector )
					{
						bThisSectorOnly = FALSE;
					}
				}////////
			}///////

			if( !bThisSectorOnly )
			{
				// 섹터가 걸치는 녀석이라면
				pEventNature->m_MutexlistUpdateSectorNatureInfo.Lock		();
				pEventNature->AddSkyObject( pcsApdObject->m_lID );
				pEventNature->m_MutexlistUpdateSectorNatureInfo.Unlock	();
			}
		}
		else
		{
			// 템플릿이 없는경우..


		}
	}

	return TRUE;
}

BOOL AgcmEventNature::CBOnRemoveObject( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmEventNature	*	pEventNature	= ( AgcmEventNature	* ) pClass	;
	ApdObject		*	pcsApdObject	= ( ApdObject		* ) pData	;

	// 이벤트 스트럭쳐도 얻어냄..
	ApdEvent *	pstEvent	;

	// EventNature Event Struct 얻어냄..
	pstEvent	= pEventNature->m_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_NATURE );

	if( pstEvent )
	{
		// 리스트에서 제거..

		pEventNature->RemoveSkyObject( pcsApdObject->m_lID );
	}
	return TRUE;
}

 
BOOL AgcmEventNature::AddEventNatureObject( ApWorldSector * pSector , ApdObject * pcsApdObject , INT32 nTemplate , FLOAT fRadius , UINT32 nSkyFlag , INT32 nDepth )
{
	// 아직 테스트중..
	ASSERT( NULL != pSector			);
	ASSERT( NULL != pcsApdObject	);

	// 우선 오브젝트에 이벤트가 붙어있는지 점검..

	if( pSector->IsInSectorRadius( pcsApdObject->m_stPosition.x , pcsApdObject->m_stPosition.z , fRadius ) )
	{
		int		segmentx , segmentz	;
		FLOAT	fSegPosX , fSegPosZ	;
		FLOAT	fDistance			;
		INT32	nCount , aArray[ SECTOR_MAX_NATURE_OBJECT_COUNT ];
		int		i;

		for( segmentz = 0 ; segmentz < MAP_DEFAULT_DEPTH ; ++segmentz  )
		{
			for( segmentx = 0 ; segmentx < MAP_DEFAULT_DEPTH ; ++segmentx  )
			{
				pSector->D_GetSegment( SECTOR_HIGHDETAIL , segmentx , segmentz , &fSegPosX , &fSegPosZ );

				// 범위체크
				if( ( fDistance = ( FLOAT )
					sqrt(	( fSegPosX - pcsApdObject->m_stPosition.x ) * ( fSegPosX - pcsApdObject->m_stPosition.x ) + 
							( fSegPosZ - pcsApdObject->m_stPosition.z ) * ( fSegPosZ - pcsApdObject->m_stPosition.z ) ) )
					> fRadius
					) continue;

				nCount = pSector->GetIndex( ApWorldSector::AWS_SKYOBJECT , segmentx , segmentz , aArray , SECTOR_MAX_NATURE_OBJECT_COUNT );
				if( nCount )
				{	
					if( nSkyFlag & ASkySetting::DA_LIGHT )
					{
						if( __GetSkyTemplateIndexFromMapData	( aArray[ 0 ] ) != nTemplate	&&
							__GetSkyTemplateDistanceFromMapData	( aArray[ 0 ] ) > fDistance		)
						{
							aArray [ 0 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
						}
					}
					if( nSkyFlag & ASkySetting::DA_FOG )
					{
						if( __GetSkyTemplateIndexFromMapData	( aArray[ 1 ] ) != nTemplate	&&
							__GetSkyTemplateDistanceFromMapData	( aArray[ 1 ] ) > fDistance		)
						{
							aArray [ 1 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
						}
					}
					if( nSkyFlag & ASkySetting::DA_CLOUD )
					{
						if( __GetSkyTemplateIndexFromMapData	( aArray[ 2 ] ) != nTemplate	&&
							__GetSkyTemplateDistanceFromMapData	( aArray[ 2 ] ) > fDistance		)
						{
							aArray [ 2 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
						}
					}
					if( nSkyFlag & ASkySetting::DA_EFFECT )
					{
						if( __GetSkyTemplateIndexFromMapData	( aArray[ 3 ] ) != nTemplate	&&
							__GetSkyTemplateDistanceFromMapData	( aArray[ 3 ] ) > fDistance		)
						{
							aArray [ 3 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
						}
					}
					if( nSkyFlag & ASkySetting::DA_MUSIC )
					{
						if( __GetSkyTemplateIndexFromMapData	( aArray[ 4 ] ) != nTemplate	&&
							__GetSkyTemplateDistanceFromMapData	( aArray[ 4 ] ) > fDistance		)
						{
							aArray [ 4 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
						}
					}
					if( nSkyFlag & ASkySetting::DA_WEATHER )
					{
						if( __GetSkyTemplateIndexFromMapData	( aArray[ 5 ] ) != nTemplate	&&
							__GetSkyTemplateDistanceFromMapData	( aArray[ 5 ] ) > fDistance		)
						{
							aArray [ 5 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
						}
					}
				}
				else
				{
					// 마고자 (2004-07-21 오전 11:51:08) : 디폴트 스카이 설정 문제!
					// 현재 상황에서 트릴가드쪽 메인 스카이를 설정할수가 없으므로..
					// 일단은 하드 코딩한다.
					// 후에 스카이의 프라이어리티 ( 또는 디비젼단위 스카이 ) 가 적용이 돼면
					// 삭제될 예정.

					int nDefaultSky = GetDefaultSky( fSegPosX , fSegPosZ );
					
					for( i = 0 ; i < SECTOR_MAX_NATURE_OBJECT_COUNT ; i ++  )
					{
						aArray[ i ] = __MakeSkyTemplateIndexForMapData( nDefaultSky , ( ( FLOAT ) 0xffff ) * MAP_STEPSIZE );
					}

					if( nSkyFlag & ASkySetting::DA_LIGHT )
					{
						aArray [ 0 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
					}
					if( nSkyFlag & ASkySetting::DA_FOG )
					{
						aArray [ 1 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
					}
					if( nSkyFlag & ASkySetting::DA_CLOUD )
					{
						aArray [ 2 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
					}
					if( nSkyFlag & ASkySetting::DA_EFFECT )
					{
						aArray [ 3 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
					}
					if( nSkyFlag & ASkySetting::DA_MUSIC )
					{
						aArray [ 4 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
					}
					if( nSkyFlag & ASkySetting::DA_WEATHER )
					{
						aArray [ 5 ] = __MakeSkyTemplateIndexForMapData( nTemplate , fDistance );
					}
				}

				// 다시 삽입함~
				pSector->DeleteIndex( ApWorldSector::AWS_SKYOBJECT , segmentx , segmentz );
				for( i = 0 ; i < SECTOR_MAX_NATURE_OBJECT_COUNT ; i ++  )
				{
					pSector->AddIndex( ApWorldSector::AWS_SKYOBJECT , segmentx , segmentz , aArray[ i ] );
				}
			}
		}
	}

	// 상위에서 체크하고 넘어오기때문에 더이상 검사할필요없다.

	return TRUE;
}

void	AgcmEventNature::CloudUVShift( ASkySetting * pSkySet )
{
	RwIm3DVertex	*	pVertex			;
	RwIm3DVertex	*	pVertexSource	;
	INT32				nVertex			;
	INT32				nIndex			;

	FLOAT				v				;
	FLOAT				fOffset			;

	SetCloud( ASkySetting::CLOUD_FIRST	, pSkySet , NULL , 1.0f );
	SetCloud( ASkySetting::CLOUD_SECOND	, pSkySet , NULL , 1.0f );

	// UV 좌표 초기화..

	VERIFY( pVertexSource	= m_aAtomicCloud[ FIRST_CLOUD_FADING	].GetVertexBuffer()	);
	VERIFY( pVertex			= m_aAtomicCloud[ FIRST_CLOUD			].GetVertexBuffer()	);

	nVertex	= m_aAtomicCloud[ FIRST_CLOUD ].GetVertexCount()	;

	fOffset		= pVertexSource[ 0 ].v;
	RwIm3DVertexSetV	( &pVertex[ 0 ]			, pVertexSource[ 0 ].v	);
	RwIm3DVertexSetV	( &pVertexSource[ 0 ]	, 0.0f					);

	pVertexSource[ 0 ].color	= 0;

	for( nIndex = 1 ; nIndex < nVertex ; ++ nIndex )
	{
		v	= pVertexSource[ nIndex ].v - fOffset	;
		RwIm3DVertexSetV	( &pVertex[ nIndex ]		, pVertexSource[ nIndex ].v	);
		RwIm3DVertexSetV	( &pVertexSource[ nIndex ]	, v							);
		pVertexSource[ nIndex ].color	= 0;
	}

	// UV 좌표 초기화..

	VERIFY( pVertexSource	= m_aAtomicCloud[ SECOND_CLOUD_FADING	].GetVertexBuffer()	);
	VERIFY( pVertex			= m_aAtomicCloud[ SECOND_CLOUD			].GetVertexBuffer()	);

	nVertex	= m_aAtomicCloud[ SECOND_CLOUD ].GetVertexCount()	;

	fOffset		= pVertexSource[ 0 ].v;
	RwIm3DVertexSetV	( &pVertex[ 0 ]			, pVertexSource[ 0 ].v	);
	RwIm3DVertexSetV	( &pVertexSource[ 0 ]	, 0.0f					);

	pVertexSource[ 0 ].color	= 0;
	for( nIndex = 1 ; nIndex < nVertex ; ++ nIndex )
	{
		v	= pVertexSource[ nIndex ].v - fOffset	;
		RwIm3DVertexSetV	( &pVertex[ nIndex ]		, pVertexSource[ nIndex ].v	);
		RwIm3DVertexSetV	( &pVertexSource[ nIndex ]	, v							);
		pVertexSource[ nIndex ].color	= 0;
	}
}

void			AgcmEventNature::SkyChange	( ASkySetting * pSkySet )
{
//#ifdef _DEBUG
//	OutputDebugString( "[ SkySet ] ---- Change SkySet ----\n" );
//#endif

	// 환경사운드 변경
	_ChangeEnvironmentSound();

	// 음 , 이부분이 , 스카이 템플릿이 완전히 변한 부분이다.
	// 퍼머넌트 이펙트는 이부분에서 플레이 시킨다.
	EnumCallback( AGCMEVENTNATURE_CB_ID_SKYCHANGE , ( PVOID ) pSkySet , NULL );

	//// 이펙트 전부 디스트로이..
	//for( int i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
	//{
	//	if( m_paEffectPtrAttay[ i ].pEffect )
	//	{
	//		// 이펙트 딜레이체크.
	//		// 이펙트 딜레이 오버 ..
	//		// 디스트로이 시킴..

	//		// 이펙트 직접 디스트로이는 하지 않고 , 이펙트 모듈에서 각각 lifetime을
	//		// 가지기 때문에 주석처리함.
	//		if (m_pcsAgcmEff2)
	//		{
	//			m_paEffectPtrAttay [ i ].pEffect->End();
	//			m_paEffectPtrAttay [ i ].pEffect= NULL;
	//		}

	//		// 빈슬롯만듬~
	//		m_paEffectPtrAttay [ i ].pEffect	= NULL;
	//	}
	//}

	// 마고자 (2005-09-28 오전 11:37:18) : 
	// 이펙트 구현부는 일단 블럭.
	/*
	// Weather Effect 요청 처리는 이부분에서..
	// TODO : Weather Effect 처리는 여기서
	if( -1 != pSkySet->nWeatherEffect1 && m_nLastPlayedWeatherEffect1 != pSkySet->nWeatherEffect1)
	{
		// 이전 이펙트 스톱.
		if( -1 != m_nLastPlayedWeatherEffect1 )
		{
			m_pcsAgcmNatureEffect->EndNatureEffect( m_nLastPlayedWeatherEffect1 );
			m_nLastPlayedWeatherEffect1 = -1;
		}

		// 이펙트 요청.
		m_pcsAgcmNatureEffect->StartNatureEffect( pSkySet->nWeatherEffect2 );
		m_nLastPlayedWeatherEffect1 = pSkySet->nWeatherEffect1;
	}
	else if( -1 == pSkySet->nWeatherEffect1 )
	{
		// Effect Stop
		if( -1 != m_nLastPlayedWeatherEffect1 )
		{
			m_pcsAgcmNatureEffect->EndNatureEffect( m_nLastPlayedWeatherEffect1 );
			m_nLastPlayedWeatherEffect1 = -1;
		}
	}
	else
	{
		// 그대로 유지
	}

	if( -1 != pSkySet->nWeatherEffect2 && m_nLastPlayedWeatherEffect2 != pSkySet->nWeatherEffect2 )
	{
		// 이펙트 요청.
		// 이전 이펙트 스톱.
		if( -1 != m_nLastPlayedWeatherEffect2 )
		{
			m_pcsAgcmNatureEffect->EndNatureEffect( m_nLastPlayedWeatherEffect2 );
			m_nLastPlayedWeatherEffect2 = -1;
		}

		// 이펙트 요청.
		m_pcsAgcmNatureEffect->StartNatureEffect( pSkySet->nWeatherEffect2 );
		m_nLastPlayedWeatherEffect2 = pSkySet->nWeatherEffect2;
	}
	else if( -1 == pSkySet->nWeatherEffect2 )
	{
		// Effect Stop
		if( -1 != m_nLastPlayedWeatherEffect2 )
		{
			m_pcsAgcmNatureEffect->EndNatureEffect( m_nLastPlayedWeatherEffect2 );
			m_nLastPlayedWeatherEffect2 = -1;
		}
	}
	else
	{
		// 그대로 유지
	}
	*/

	// 마고자 (2005-03-09 오전 11:53:57) : 
	// 배경음악이 절대 끊어지는경우가 없게한다.

	if( __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 4 ] ) !=
		m_nCurrentBGMSetIndex										)
	{
		m_nCurrentBGMSetIndex	= __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 4 ] );

		// 백뮤직 플레이..
		// PlayBGM에서 자동으로 플레이가 돼게
		// 기존 배경음악이 끝난 것으로 처리한다.
		const char* pCurrentBGMFileName = m_pcsAgcmSound->GetCurrBGMFileName();
		UINT32	uLengthCurrBGM = m_pcsAgcmSound->GetStreamSoundLength( pCurrentBGMFileName );
		m_uLastBGMStarted = ( GetTickCount() + 1000 ) - uLengthCurrBGM; // 1 초 뒤에 변경
	}
}

void	AgcmEventNature::PlayBGM()
{
	const char* pCurrentBGMFileName = m_pcsAgcmSound->GetCurrBGMFileName();
	UINT32	uLengthCurrBGM = m_pcsAgcmSound->GetStreamSoundLength( pCurrentBGMFileName );

	if( GetTickCount() >= m_uLastBGMStarted + uLengthCurrBGM )
	{
		// 플레이주기가 넘었으면..
//#ifdef _DEBUG
//		char strDebug[ 256 ] = { 0, };
//		sprintf_s( strDebug, sizeof( char ) * 256, "[ Sound ] 제한시간이 초과하여 BGM 을 변경합니다. 이전 플레이 시작시간 = %ud, 이전 BGM 길이 = %ud\n",
//			m_uLastBGMStarted, uLengthCurrBGM );
//		OutputDebugString( strDebug );
//#endif

		// 랜덤으로 선택..
		if( m_arrayBGM.size() )
		{
			char * pBGMFile = m_arrayBGM[ rand() % m_arrayBGM.size() ];

			if( m_pcsAgcmSound
				#ifdef USE_MFC
				&& !( GetMaptoolDebugInfoFlag() & SKY_DONOTCHANGEBGM )
				#endif
				)
			{
				if( strlen( pBGMFile ) )
				{
					char	strFileFullPath[ 1024 ];
					wsprintf( strFileFullPath , "%s\\%s" , GetBGMDirectory() , pBGMFile );
					UINT32	uLengthNextBGM = m_pcsAgcmSound->GetStreamSoundLength( strFileFullPath );

					if( m_pcsAgcmSound->PlayBGM( strFileFullPath ) )
					{
						TRACE( "배경음악 '%s' 길이 (%.1f분) 플레이\n" , strFileFullPath , ( FLOAT ) uLengthNextBGM / 60000.0f );

						// 마고자 (2005-03-10 오후 6:52:18) : 
						// BGM 콜백.
						EnumCallback( AGCMEVENTNATURE_CB_ID_BGMUPDATE , (PVOID) pBGMFile , NULL );

						m_uLastBGMStarted = GetTickCount() + uLengthNextBGM + BGM_REPEAT_GAP;
					}
					else
					{
						TRACE( "배경음악 '%s' 플레이 실패!\n" , strFileFullPath );

						// 마고자 (2005-03-16 오후 1:00:04) : 
						// 배경음악 플레이.. 1초 뒤에 다시 한다.
						m_uLastBGMStarted = GetTickCount() + 1000; // 1 초 뒤에 변경
					}
				}
				else
				{
					// 마고자 (2005-03-16 오후 1:00:04) : 
					// 배경음악 플레이.. 1초 뒤에 다시 한다.
					m_uLastBGMStarted = GetTickCount() + 1000; // 1 초 뒤에 변경
				}
			}
			else
			{
				// 리스트가 아무것도 없으면 5초 뒤에 다시 시도..
				m_uLastBGMStarted = GetTickCount() + 5000;
			}
		}
		else
		{
			// 리스트가 아무것도 없으면 5초 뒤에 다시 시도..
            m_uLastBGMStarted = GetTickCount() + 5000;
		}
	}
}


BOOL	AgcmEventNature::CBLoadSector		(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ApWorldSector *		pSector		= (ApWorldSector *)		pData;
	AgcmEventNature *	pThis		= (AgcmEventNature *)	pClass;

	if ((INT32) pCustData == SECTOR_LOWDETAIL)
	{
		if( pThis->m_pcsAgcmResourceLoader->m_bForceImmediate )
		{
			pThis->UpdateSectorNatureInfo( pSector );
		}
		else
		{
			// 리스트 추가...
			pThis->m_MutexlistUpdateSectorNatureInfo.Lock		();
			pThis->m_listUpdateSectorNatureInfo.AddTail( pSector );
			pThis->m_MutexlistUpdateSectorNatureInfo.Unlock	();
		}
	}

	return TRUE;
}

BOOL	AgcmEventNature::UpdateSectorNatureObject( ApWorldSector * pSector , INT32 uID )
{
	ApdObject *	pcsObject = m_pcsApmObject->GetObject( uID );

	if( NULL == pcsObject )
	{
		return FALSE;
	}

	ApdEvent *	pstEvent	= m_pcsApmEventManager->GetEvent( pcsObject , APDEVENT_FUNCTION_NATURE );

	if( pstEvent )
	{
		// 스카이 이벤트 발견. 
		INT32		nTemplateID	;
		FLOAT		fRadius		;

		// Template Index 
		nTemplateID = ( INT32 ) pstEvent->m_pvData;
		fRadius		= pstEvent->m_pstCondition->m_pstArea->m_uoData.m_stPan.m_fRadius;

		AgpdSkySet *	pPublicSkySet	;
		AgcdSkySet *	pClientSkySet	;
		
		pPublicSkySet = m_pcsAgpmEventNature->GetSkySet( nTemplateID );

		if( pPublicSkySet )
		{
			pClientSkySet = GetSkySetClientData( pPublicSkySet );
			ASkySetting	* pSkySet = pClientSkySet->m_aSkySetting;

			AddEventNatureObject(pSector, pcsObject, nTemplateID, fRadius , pSkySet->nDataAvailableFlag );
		}
	}

	return TRUE;
}

// 2006.10.26. steeple
BOOL	AgcmEventNature::CBReceiveTime( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if(!pClass)
		return FALSE;

	AgcmEventNature* pThis = static_cast<AgcmEventNature*>(pClass);

	pThis->SetTime(pThis->m_pcsAgpmEventNature->GetGameTime());
	return TRUE;
}

// 2006.11.08. steeple
BOOL	AgcmEventNature::CBNature( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if(!pClass || !pData)
		return FALSE;

	AgcmEventNature* pThis = static_cast<AgcmEventNature*>(pClass);
	AgpdNature* pcsNature = static_cast<AgpdNature*>(pData);

	pThis->m_pcsAgcmNatureEffect->SetWindEnable(TRUE);

	switch(pcsNature->m_eWeather)
	{
		case AGPDNATURE_WEATHER_TYPE_NONE:
			pThis->m_pcsAgcmNatureEffect->StopNatureEffect();
			break;

		case AGPDNATURE_WEATHER_TYPE_RAINY:
			pThis->m_pcsAgcmNatureEffect->StopNatureEffect(TRUE);
			pThis->m_pcsAgcmNatureEffect->StartNatureEffect(NATURE_EFFECT_RAIN);
			break;

		case AGPDNATURE_WEATHER_TYPE_SNOWY:
			pThis->m_pcsAgcmNatureEffect->StopNatureEffect(TRUE);
			pThis->m_pcsAgcmNatureEffect->StartNatureEffect(NATURE_EFFECT_SNOW);
			break;
	}

	return TRUE;
}

void	AgcmEventNature::UpdateSectorNatureInfo	( ApWorldSector * pSector )
{
	AuNode< INT32	 > *		pNode = m_listNatureObject.GetHeadNode();
	INT32						nID			;

	while (pNode)
	{
		nID = pNode->GetData();

		if( UpdateSectorNatureObject( pSector , nID ) )
		{
			// do nothing..
		}
		else
		{
			// 없어진 오브젝투!
			AuNode< INT32	 > *	pNext;
			pNext = pNode->GetNextNode();

			m_listNatureObject.RemoveNode( pNode );
			
			pNode = pNext;

			continue;
		}

		pNode = pNode->GetNextNode();
	}
}

INT32	AgcmEventNature::SetSkyTemplateID( INT32 nID , INT32 nFlag )
{
	AgpdSkySet *	pPublicSkySet	;
	AgcdSkySet *	pClientSkySet	;
	pPublicSkySet = m_pcsAgpmEventNature->GetSkySet( nID );

	// 템플릿이 없어요..
	if( NULL == pPublicSkySet ) return 0;

	VERIFY( pClientSkySet = GetSkySetClientData( pPublicSkySet ) );
	ASkySetting	* pSkySet = pClientSkySet->m_aSkySetting;

	return SetSkyTemplateID( pSkySet , nFlag );
}

INT32	AgcmEventNature::SetSkyTemplateID( ASkySetting	* pSkySet , INT32 nFlag )
{
	if( !( pSkySet->nDataAvailableFlag & nFlag ) )
	{
		// 에러다.
		return 0;
	}

	if( IsNatureEffectPlaying() )
	{
		if( pSkySet->nWeatherEffect1 & ASkySetting::DISABLE_WEATHER )
		{
			// do nothing.
		}
		else
		{
			// 이펙트가 엔드 될때까지 스카이를 바꾸지 않음.
			return 0;
		}
	}

	vector< char * >	arrayBGM			;

	for( int i = 0 ; i < ASkySetting::TS_MAX ; i ++ )
	{
		m_skyMixedUp[ i ].nDataAvailableFlag = ASkySetting::DA_ALL;

		// 부분 부분 카피해넣음..

		if( nFlag & ASkySetting::DA_LIGHT	)
		{
			m_skyMixedUp[ i ].rgbSkyTop			= pSkySet[ i ].rgbSkyTop		;
			m_skyMixedUp[ i ].rgbSkyBottom		= pSkySet[ i ].rgbSkyBottom		;
			m_skyMixedUp[ i ].rgbAmbient		= pSkySet[ i ].rgbAmbient		;
			m_skyMixedUp[ i ].rgbDirectional	= pSkySet[ i ].rgbDirectional	;
		}
		if( nFlag & ASkySetting::DA_FOG		)
		{
			m_skyMixedUp[ i ].rgbFog			= pSkySet[ i ].rgbFog			;
			m_skyMixedUp[ i ].fFogDistance		= pSkySet[ i ].fFogDistance		;
			m_skyMixedUp[ i ].fFogFarClip		= pSkySet[ i ].fFogFarClip		;
		}
		if( nFlag & ASkySetting::DA_CLOUD	)
		{
			m_skyMixedUp[ i ].aCloud[ ASkySetting::CLOUD_FIRST	]	= pSkySet[ i ].aCloud[ ASkySetting::CLOUD_FIRST	]	;
			m_skyMixedUp[ i ].aCloud[ASkySetting:: CLOUD_SECOND	]	= pSkySet[ i ].aCloud[ ASkySetting::CLOUD_SECOND	]	;
			m_skyMixedUp[ i ].nCircumstanceTexture					= pSkySet[ i ].nCircumstanceTexture		;
			m_skyMixedUp[ i ].rgbCircumstance						= pSkySet[ i ].rgbCircumstance			;
			m_skyMixedUp[ i ].nSunTexture							= pSkySet[ i ].nSunTexture				;
			m_skyMixedUp[ i ].rgbSun								= pSkySet[ i ].rgbSun					;
		}
		if( nFlag & ASkySetting::DA_EFFECT	)
		{
			m_skyMixedUp[ i ].nEffectInterval	= pSkySet[ i ].nEffectInterval;
		
			for( int k = 0 ; k < AGCMEVENTNATURE_MAX_EVENT ; ++ k )
			{
				m_skyMixedUp[ i ].aEffectArray		[ k ]	= pSkySet[ i ].aEffectArray		[ k ];
				m_skyMixedUp[ i ].aEffectRateArray	[ k ]	= pSkySet[ i ].aEffectRateArray	[ k ];
			}
		}
		if( nFlag & ASkySetting::DA_MUSIC	)
		{
			strncpy( m_skyMixedUp[ i ].strBackMusic , pSkySet[ i ].strBackMusic , EFF2_FILE_NAME_MAX );

			if( strlen( pSkySet[ i ].strBackMusic ) )
			{
				// 배경음악이 있는 거라면..
				// 해당 포이터를 추가해둠..

				// 중복 검사..
				int nCount = arrayBGM.size();
				BOOL bFound = FALSE;
				for( int j = 0 ; j < nCount ; j ++ )
				{
					// 대소문자 체크는 안함.
					if( !strncmp( arrayBGM[ j ] , pSkySet[ i ].strBackMusic , EFF2_FILE_NAME_MAX ) )
					{
						// found
						bFound = TRUE;
						break;
					}
				}

				if( !bFound )
				{
					// 배열에 추가함..
					arrayBGM.push_back( pSkySet[ i ].strBackMusic );
				}
			}
		}
		if( nFlag & ASkySetting::DA_WEATHER		)
		{
			m_skyMixedUp[ i ].nWeatherEffect1	= pSkySet[ i ].nWeatherEffect1;
			m_skyMixedUp[ i ].nWeatherEffect2	= pSkySet[ i ].nWeatherEffect2;
		}
	}

	if( nFlag & ASkySetting::DA_MUSIC )
	{
		// 배경음악이 설정이면
		// 공용 배경음악 리스트를 초기화한다.
		if( arrayBGM.size() )
		{
			m_arrayBGM.clear();
			m_arrayBGM = arrayBGM;
		}
	}

	return 1;
}

void	AgcmEventNature::SetSkyCircleRadius()
{
	FLOAT	half1 = SKY_RADIUS*0.5f;
	FLOAT	pow1 = half1 * half1;
	FLOAT	pow2 = SKY_HEIGHT * SKY_HEIGHT;
	FLOAT	sqr1 = sqrt(pow1 + pow2);
	m_fSkyCircleRadius = half1 * SKY_HEIGHT / sqr1;
}

void	AgcmEventNature::UpdateSunPosition()
{
	RwFrame*            lightFrame = RpLightGetFrame(m_pLightDirectional);
	RwMatrix*			lightMatrix = RwFrameGetMatrix(lightFrame);
	RwV3d*				at = RwMatrixGetAt(lightMatrix);
	RwV3d				negated_at;
	RwV3dNegate (&negated_at,at);

	RwV3dScale(&negated_at,&negated_at,m_fSkyCircleRadius);
	
	RwV3dAdd(&m_posSun,&m_posLastPosition,&negated_at);
}

BOOL		AgcmEventNature::GetSunPosition	( RwV3d * pV3d )
{
	// 마고자 (2003-05-27 오후 4:47:50) : 아직 구현돼지 않음..
	*pV3d = m_posSun;
	return TRUE;
}

void		AgcmEventNature::SetCloudZoomRate	( FLOAT fCloudZoomRate )
{
	for( int i = 0 ; i < CLOUD_ATOMIC_LAYER_MAX ; ++ i )
	{
		VERIFY( CreateCloudAtomic( &m_aAtomicCloud[ i ]	, fCloudZoomRate ) );
	}
}

BOOL	AgcmEventNature::CBOnSetSelfCharacter		( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmEventNature * pThis = ( AgcmEventNature * ) pClass;

	ASSERT( NULL != pThis );
	ASSERT( NULL != pData );
	if( NULL == pThis || NULL == pData ) return FALSE;

	// 케릭터 데이타..
	// 케릭터 데이타..
	AgpdCharacter	* pcsAgpdCharacter = (AgpdCharacter *) pData		;
	AgcdCharacter	* pcsAgcdCharacter = (AgcdCharacter *) pCustData	;

	pThis->SetCharacterPosition( pcsAgpdCharacter->m_stPos );
	pThis->ApplySkySetting();
	
	return TRUE;
}

//BOOL	AgcmEventNature::CBOnTeleportStart			( PVOID pData, PVOID pClass, PVOID pCustData )
//{
//	AgcmEventNature * pThis = ( AgcmEventNature * ) pClass;
//
//	ASSERT( NULL != pThis );
//	ASSERT( NULL != pData );
//	if( NULL == pThis || NULL == pData ) return FALSE;
//
//	// 케릭터 데이타..
//	pThis->ApplySkySetting();
//	
//	return TRUE;
//}

void	AgcmEventNature::RenderStar		()
{
	ASkySetting * pCurrent = GetCurrentSkySetting();
	ASSERT( NULL != pCurrent );
	if( NULL == pCurrent ) return;

	if( !( pCurrent->nWeatherEffect1 & ASkySetting::SHOW_STAR ) ) return;
	// 별옵션이 없으면 별을 렌더하지 않음,

	int	nShowStar = 0;
//#ifdef USE_MFC
//	if( this->m_pcsAgpmTimer->TimerIsStop() )
//	{
//		nShowStar = m_nStarShowLevel;
//	}
//	else
//	{
//		switch( this->m_pcsAgpmTimer->GetCurHour() )
//		{
//		case 18	:
//		case 19	: nShowStar = 3 ; break;
//		case 20	: nShowStar = 2 ; break;
//		case 21	: nShowStar = 1 ; break;
//		case 22	: 
//		case 23	: 
//		case 0	: 
//		case 1	: nShowStar = 0 ; break;
//		case 2	: 
//		case 3	: nShowStar = 1 ; break;
//		case 4	: nShowStar = 2 ; break;
//		case 5	: nShowStar = 3 ; break;
//		default:
//			nShowStar = 4;
//		}
//	}
//#else
	switch( m_pcsAgpmEventNature->GetHour( m_nCurrentTime ) )
	{
	case 18	:
	case 19	: nShowStar = BIG_STAR		; break;
	case 20	: 
	case 21	: nShowStar = BLINK_STAR	; break;
	case 22	:
	case 23	: nShowStar = MEDIUM_STAT	; break;
	case 24	: 
	case 0	: nShowStar = SMALL_STAR	; break;
	case 1	:
	case 2	: nShowStar = MEDIUM_STAT	; break;
	case 3	:
	case 4	: nShowStar = BLINK_STAR	; break;
	case 5	: nShowStar = BIG_STAR		; break;
	default:
		nShowStar = MAX_STAR;
	}
//#endif
	if( nShowStar < MAX_STAR )
	{
		int	nStart , nEnd = m_anStarCount[ SMALL_STAR	] + m_anStarCount[ MEDIUM_STAT] + m_anStarCount[ BLINK_STAR	] + m_anStarCount[ BIG_STAR	];
		nStart = nEnd;
		switch( m_nStarShowLevel )
		{
		case SMALL_STAR	:	nStart -= m_anStarCount[ SMALL_STAR	];
		case MEDIUM_STAT:	nStart -= m_anStarCount[ MEDIUM_STAT];
		case BLINK_STAR	:	nStart -= m_anStarCount[ BLINK_STAR	];
		case BIG_STAR	:	nStart -= m_anStarCount[ BIG_STAR	]; 
			break;
		default			:
			return;
		}

		RwD3D9SetTexture(NULL, 0);

		RwD3D9SetStreamSource( 0 , m_pD3DStarVertex , m_uStarVertexOffset , m_uStarVertexStride );		//Set the stream sources

		RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE					);
		RwD3D9SetRenderState( D3DRS_SRCBLEND		,	D3DBLEND_SRCALPHA		);
		RwD3D9SetRenderState( D3DRS_DESTBLEND		,	D3DBLEND_INVSRCALPHA	);	
		
		RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE	);
		RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_CURRENT	);
		RwD3D9SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_ADD		);
		RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE	);

		RwD3D9SetFVF( D3DFVF_STARVERTEX );		// Vertex Declaration
		
		LockFrame();
	    RwD3D9SetTransformWorld( RwFrameGetLTM( m_pStarFrame ) );
		UnlockFrame();

		RwD3D9DrawPrimitive( D3DPT_POINTLIST , nStart , nEnd - nStart );
	}
}

void	AgcmEventNature::BlinkStar		()
{
	//{ add by dobal
	// lost device때에는 lock할수 없다.
	if( m_bLostDevice )
		return;
	// } dobal

	if( NULL == m_pD3DStarVertex ) return;
	static UINT32 __suLastChange = GetTickCount();
	if( GetTickCount() - __suLastChange < g_uStarBlinkGap ) return;
	__suLastChange = GetTickCount();

	// 반짝 반짝 작은별~

	void	* pBuffer;
	D3DStarVertex * pVertexStar , * pTmp ;
    IDirect3DVertexBuffer9_Lock(
						(LPDIRECT3DVERTEXBUFFER9)m_pD3DStarVertex,
                        m_uStarVertexOffset	,
						m_uStarVertexSize	,
						&(pBuffer)		,
                        D3DLOCK_NOSYSLOCK	);
	pVertexStar = ( D3DStarVertex * ) pBuffer;

	int	nCount	= m_anStarCount[ SMALL_STAR	] + m_anStarCount[ MEDIUM_STAT	];
	int nTarget	= m_anStarCount[ BLINK_STAR	];

	int	nChangeCount = m_anStarCount[ BLINK_STAR	] * g_nStarCountPerOneTimeBlinkRate / 100;

	for( int i = 0 ; i < nChangeCount ; ++ i )
	{
		pTmp = & pVertexStar[ nCount + rand() % nTarget ];

		STARVERTEXSetAlpha( pTmp , RANDINT( 200 , 110 ) );
	}

	IDirect3DVertexBuffer9_Unlock( (LPDIRECT3DVERTEXBUFFER9)m_pD3DStarVertex );
}

void	AgcmEventNature::DestroyStar	()
{
	if( m_pD3DStarVertex )
	{
		RwD3D9DestroyVertexBuffer( m_uStarVertexStride , m_uStarVertexSize , m_pD3DStarVertex , m_uStarVertexOffset );
		m_pD3DStarVertex = NULL;
	}

	if( m_pStarFrame )
	{
		RwFrameDestroy( m_pStarFrame );
		m_pStarFrame = NULL;
	}

	m_uStarVertexStride	= 0;
	m_uStarVertexSize	= 0;
	m_uStarVertexOffset	= 0;

	for( int i = 0 ; i < MAX_STAR ; ++ i )
	{
		m_anStarCount[ i ] = 0;
	}
}

BOOL	AgcmEventNature::GenerateStarVertex	( int nSmall , int nMedium , int nBlink , int nBig )
{
	ASSERT( nSmall	> 0 );
	ASSERT( nMedium	> 0 );
	ASSERT( nBlink	> 0 );
	ASSERT( nBig	> 0 );

	ASSERT( nSmall + nMedium + nBlink + nBig	< 65530 );
	if( nSmall + nMedium + nBlink + nBig >= 65530 ) return FALSE;

	DestroyStar();

	int	nCount , nTarget = nSmall;

	// 버택스 생성..
	m_uStarVertexStride	= sizeof( D3DStarVertex );
	m_uStarVertexSize	= sizeof( D3DStarVertex ) * ( nSmall + nMedium + nBlink + nBig );
	m_uStarVertexOffset	= 0;

	if( !RwD3D9CreateVertexBuffer( m_uStarVertexStride , m_uStarVertexSize , &m_pD3DStarVertex ,  &m_uStarVertexOffset ) )
	{
		TRACE( "AgcmEventNature::GenerateStarVertex 별생성실패\n" );
		return FALSE;
	}

	void	* pBuffer;
	D3DStarVertex * pVertexStar;
    IDirect3DVertexBuffer9_Lock(
						(LPDIRECT3DVERTEXBUFFER9)m_pD3DStarVertex,
                        m_uStarVertexOffset	,
						m_uStarVertexSize	,
						&(pBuffer)		,
                        D3DLOCK_NOSYSLOCK	);
	pVertexStar = ( D3DStarVertex * ) pBuffer;

	for( nCount = 0 ; nCount < nTarget ; ++ nCount )
	{
		pVertexStar[ nCount ].x	=	RANDOMFLOAT()	;
		pVertexStar[ nCount ].z	=	RANDOMFLOAT()	;
		STARVERTEXCALCY( &pVertexStar[ nCount ] );

		STARVERTEXSetRGBA( &pVertexStar[ nCount ] , RANDINT( 64 , 20 ), RANDINT( 64 , 20 ) , RANDINT( 64 , 20 ) , 255 );
	}

	nTarget += nMedium;

	for( ; nCount < nTarget ; ++ nCount )
	{
		pVertexStar[ nCount ].x	=	RANDOMFLOAT()	;
		pVertexStar[ nCount ].z	=	RANDOMFLOAT()	;
		STARVERTEXCALCY( &pVertexStar[ nCount ] );

		STARVERTEXSetRGBA( &pVertexStar[ nCount ] , RANDINT( 100 , 50 ) , RANDINT( 100 , 50 ) , RANDINT( 100 , 50 ) , 255 );
	}
	
	nTarget += nBlink;

	for( ; nCount < nTarget ; ++ nCount )
	{
		pVertexStar[ nCount ].x	=	RANDOMFLOAT()	;
		pVertexStar[ nCount ].z	=	RANDOMFLOAT()	;
		STARVERTEXCALCY( &pVertexStar[ nCount ] );

		STARVERTEXSetRGBA( &pVertexStar[ nCount ] , RANDINT( 192 , 40 ) , RANDINT( 192 , 40 ) , RANDINT( 192 , 40 ) , 255 );
	}

	nTarget += nBig;

	for( ; nCount < nTarget ; ++ nCount )
	{
		pVertexStar[ nCount ].x	=	RANDOMFLOAT()	;
		pVertexStar[ nCount ].z	=	RANDOMFLOAT()	;
		STARVERTEXCALCY( &pVertexStar[ nCount ] );

		STARVERTEXSetRGBA( &pVertexStar[ nCount ] , RANDINT( 200 , 110 ) , RANDINT( 200 , 110 ) , RANDINT( 200 , 110 ) , 255 );
	}

	m_anStarCount[ SMALL_STAR	] = nSmall	;
	m_anStarCount[ MEDIUM_STAT	] = nMedium	;
	m_anStarCount[ BLINK_STAR	] = nBlink	;
	m_anStarCount[ BIG_STAR		] = nBig	;

	IDirect3DVertexBuffer9_Unlock( (LPDIRECT3DVERTEXBUFFER9)m_pD3DStarVertex );

	m_pStarFrame = RwFrameCreate();

	// 마고자 (2004-06-09 오후 2:17:22) : 
	if( m_aAtomicCircustance[ FORE_TEXTURE ].GetFrame() )
		MoveSky();

	m_nStarShowLevel			= 0;

	return TRUE;
}

BOOL	AgcmEventNature::LoadStarData		( char * pFilename )
{
	// 
	return TRUE;
}

BOOL	AgcmEventNature::AddSkyObject		( INT32 nID )
{
	AuNode< INT32 > * pNode = m_listNatureObject.GetHeadNode();
	INT32	nValue ;
	while( pNode )
	{
		nValue = pNode->GetData();

		if( nValue == nID )
		{
			// 중복되는 거 있으르로 삽입안함..
			return FALSE;
		}
		
		pNode = pNode->GetNextNode();
	}

	m_listNatureObject.AddTail( nID );
	return TRUE;
}

BOOL	AgcmEventNature::RemoveSkyObject	( INT32 nID )
{
	AuNode< INT32 > * pNode = m_listNatureObject.GetHeadNode();
	INT32	nValue ;

	while( pNode )
	{
		nValue = pNode->GetData();

		if( nValue == nID )
		{
			// 이거다..
			// 삭제..
			m_listNatureObject.RemoveNode( pNode );
			return TRUE;
		}
	
		pNode = pNode->GetNextNode();
	}

	return FALSE;
}

BOOL	AgcmEventNature::CBOnSetAutoLoadChange		( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmEventNature	* pThis = ( AgcmEventNature * ) pClass;
	INT32	eMapView	= ( INT32 ) pData;
	INT32	nDetail		= ( INT32 ) pCustData;

	//static FLOAT	_sNearValue		= 15000.0f;
	//static FLOAT	_sNormalValue	= 30000.0f;

	switch( eMapView )
	{
	case 0:
		// 마고자 (2005-03-31 오후 7:09:51) : 최소 거리일때만 
		pThis->m_fFogAdjustFarClip = pThis->m_fNearValue;
		break;
	case 1:
		// 마고자 (2005-03-31 오후 7:09:51) : 최소 거리일때만 
		pThis->m_fFogAdjustFarClip = pThis->m_fNormalValue;
		break;
	case 2:
	case 3:
	default:
		// 마고자 (2005-04-01 오후 3:35:12) : 디폴트값.
		pThis->m_fFogAdjustFarClip = AgcmEventNature::SKY_RADIUS;
		break;
	}

	pThis->ApplySkySetting();

	return TRUE;
}

//add by dobal {
void AgcmEventNature::NewD3D9ReleaseDeviceCB()
{
	m_pThis->OnLostDevice();

	if( m_pThis->OldD3D9ReleaseDeviceCB )
		m_pThis->OldD3D9ReleaseDeviceCB();
}

void AgcmEventNature::NewD3D9RestoreDeviceCB()
{
	if( m_pThis->OldD3D9RestoreDeviceCB )
		m_pThis->OldD3D9RestoreDeviceCB();

	m_pThis->OnResetDevice();
}


void AgcmEventNature::OnLostDevice()
{
	m_bLostDevice = true;
}

void AgcmEventNature::OnResetDevice()
{
	m_bLostDevice = false;
}
// } dobal

INT32	AgcmEventNature::SetNatureEffectSkyTemplateID( INT32 nID , BOOL bForce )
{
	if( IsNatureEffectPlaying() && 
		nID == __GetSkyTemplateIndexFromMapData( m_skyMixedUpIndex [ 0 ] ) )
	{
		// 이미 플레이중이고 같은거면..
		return TRUE;
	}

	BOOL bChanged = FALSE;

	if( SetSkyTemplateID( nID , ASkySetting::DA_LIGHT	 ) )
	{
		m_skyMixedUpIndex [ 0 ] = __MakeSkyTemplateIndexForMapData( nID , 0.0f );
		bChanged = TRUE;
	}
	if( SetSkyTemplateID( nID , ASkySetting::DA_FOG	 ) )
	{
		m_skyMixedUpIndex [ 1 ] = __MakeSkyTemplateIndexForMapData( nID , 0.0f );
		bChanged = TRUE;
	}
	if( SetSkyTemplateID( nID , ASkySetting::DA_CLOUD	 ) )
	{
		m_skyMixedUpIndex [ 2 ] = __MakeSkyTemplateIndexForMapData( nID , 0.0f );
		bChanged = TRUE;
	}
	if( SetSkyTemplateID( nID , ASkySetting::DA_EFFECT	 ) )
	{
		m_skyMixedUpIndex [ 3 ] = __MakeSkyTemplateIndexForMapData( nID , 0.0f );
		bChanged = TRUE;
	}
	if( SetSkyTemplateID( nID , ASkySetting::DA_MUSIC	 ) )
	{
		m_skyMixedUpIndex [ 4 ] = __MakeSkyTemplateIndexForMapData( nID , 0.0f );
		bChanged = TRUE;
	}
	if( SetSkyTemplateID( nID , ASkySetting::DA_WEATHER	 ) )
	{
		m_skyMixedUpIndex [ 5 ] = __MakeSkyTemplateIndexForMapData( nID , 0.0f );
		bChanged = TRUE;
	}

	if( bChanged )
	{
		INT32		nHour		= m_pcsAgpmEventNature->GetHour( m_nCurrentTime );
		ASkySetting::TimeSet eTimeSet = ASkySetting::GetTimeSet( nHour );

		SetNextWeather( eTimeSet , __uSkyChangeGap , bForce ); //&pSkySet->m_aSkySetting[ nHour ] );

		// 플래그 설정.
		m_bNatureEffectPlaying = TRUE;

		return TRUE;
	}
	else
	{
		// 설정할수 없음..
		return FALSE;
	}
}

void	AgcmEventNature::EndNatureEffectSky( BOOL bForce ) 
{
	m_bNatureEffectPlaying = FALSE;

	AuPOS pos;
	pos.x = m_posLastPosition.x;
	pos.y = m_posLastPosition.y;
	pos.z = m_posLastPosition.z;
	SetCharacterPosition( pos , bForce );

	if( bForce )
	{
		ApplySkySetting();
	}
}

BOOL	AgcmEventNature::CBOnNatureEffectStart	( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmEventNature	* pThis = ( AgcmEventNature * ) pClass;
	NatureEffectInfo * pstNEInfo = (NatureEffectInfo * ) pData;
	BOOL * pbForce = ( BOOL * ) pCustData;
	ASSERT( pstNEInfo );
	ASSERT( pbForce );

	return pThis->SetNatureEffectSkyTemplateID( pstNEInfo->nSkySet , * pbForce );
}

BOOL	AgcmEventNature::CBOnNatureEffectEnd		( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmEventNature	* pThis = ( AgcmEventNature * ) pClass;
	BOOL * pbForce = ( BOOL * ) pCustData;
	ASSERT( pbForce );

	if( pThis->IsNatureEffectPlaying() )
	{
		pThis->EndNatureEffectSky( *pbForce );
	}

	return TRUE;
}

void AgcmEventNature::_ChangeEnvironmentSound( void )
{
	AgcmSound* pcmSound = ( AgcmSound* )g_pEngine->GetModule( "AgcmSound" );
	AgcmEff2* pcmEffect = ( AgcmEff2* )g_pEngine->GetModule( "AgcmEff2" );
	if( !pcmSound || !pcmEffect ) return;

	// 기존에 플레이중인 환경사운드 제거
	pcmSound->ClearStream();

	// 현재의 SkySet에 지정된 환경사운드 재생
	for( int nCount = 0 ; nCount < AGCMEVENTNATURE_MAX_EVENT ; nCount++ )
	{
		int nEffectID = m_skyCurrentSetting[ CURRENT_SKY ].aEffectArray[ nCount ];
		LPEFFSET pEffectSet = pcmEffect->GetEffectSet( nEffectID );
		if( pEffectSet )
		{
			int nEffectCount = pEffectSet->m_vecLPEffectBase.size();
			for( int nCount = 0 ; nCount < nEffectCount ; nCount++ )
			{
				LPEFFBASE pEffectBase = pEffectSet->bGetPtrEffBase( nCount );
				if( pEffectBase && pEffectBase->bGetBaseType() == AgcdEffBase::E_EFFBASE_SOUND )
				{
					AgcdEffSound* pEffSound	= static_cast< AgcdEffSound* >( pEffectBase );
					const char* pFileName = pEffSound->bGetSoundFName();
					if( pFileName && strlen( pFileName ) > 0 )
					{
						char strFileName[ 256 ] = { 0, };
						sprintf_s( strFileName, sizeof( char ) * 256, "%s\\%s", "SOUND\\STREAM", pFileName );
						pcmSound->PlayStream( strFileName );
					}
				}
			}
		}
	}
}
