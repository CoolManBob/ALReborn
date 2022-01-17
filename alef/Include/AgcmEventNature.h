// AgcmEventNature.h: interface for the AgcmEventNature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMEVENTNATURE_H__03F92BCE_D3D8_4B8B_88FA_F351422C3D5E__INCLUDED_)
#define AFX_AGCMEVENTNATURE_H__03F92BCE_D3D8_4B8B_88FA_F351422C3D5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rtimport.h"
#include "ApModule.h"
#include "AgcModule.h"
#include "AgpmEventNature.h"
#include "AgcmRender.h"
#include "AgcmEff2.h"
#include "AuList.h"
#include "MagImAtomic.h"
#include "AgcmResourceLoader.h"
#include "AgcmMap.h"
#include "AgcmSound.h"
#include "AgcmNatureEffect.h"
//#include "AgpmEventTeleport.h"

#include <vector>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventNatureD" )
#else
#pragma comment ( lib , "AgcmEventNature" )
#endif
#endif

#define	AGCMEVENTNATURE_SCRIPT_FILE_NAME	"ini\\alefskysetting.ini"
// 스크립트파일에는 다음과 같은 내용이 저장된다.
// 1,사용돼는 텍스쳐 리스트
// 2,시간대별 배경 텍스쳐와 라이트 설정.
// 3,기후별 설정. ( 구름의 양 , 특수효과등 )
#define	AGCMEVENTNATURE_DEFAULT_BGM_DIR		"sound\\bgm"

// 몇개로 쪼갤건지..
#define	AGCMEVENTNATURE_SKY_DETAIL			32
#define	AGCMEVENTNATURE_MAX_EVENT			5
#define SECTOR_MAX_NATURE_OBJECT_COUNT	6

extern FLOAT	AGCEVENTNATURE_DEFAULT_SKY_RADIUS;
const FLOAT	AGCEVENTNATURE_DEFAULT_CLOUD_DIRECTION	= 0.0f		;	// 북쪽으로 흘러감..

const FLOAT	AGCEVENTNATURE_DEFAULT_SUN_RADIUS		= 100.0f	;
const FLOAT	AGCEVENTNATURE_DEFAULT_MOON_RADIUS		= 100.0f	;

const INT32	AGCMEVENTNATURE_EMPTY_SKY				= -1		;

enum AGCMEVENTNATURE_CALLBACK
{
	AGCMEVENTNATURE_CB_ID_SKYCHANGE		,
	AGCMEVENTNATURE_CB_ID_LIGHTUPDATE	,
		// pData		: Directional Light RwRGBAReal
		// pCustData	: Ambient Light RwRGBAReal
	AGCMEVENTNATURE_CB_ID_BGMUPDATE
};

enum CLOUD_TYPE
{
	CLOUDTYPE_NONE		,
	CLOUDTYPE_DEFAULT	,
	CLOUDTYPE_1			,
	CLOUDTYPE_2			,
	CLOUDTYPE_3			,
	CLOUDTYPE_4			,
	CLOUDTYPE_COUNT
};

inline void RwRGBARealSet( RwRGBAReal * pRGB , FLOAT r = 1.0f , FLOAT g = 1.0f , FLOAT b = 1.0f , FLOAT a = 1.0f )
{
	pRGB->red	=r;
	pRGB->green	=g;
	pRGB->blue	=b;
	pRGB->alpha	=a;
}

inline void RwRGBARealSetByte( RwRGBAReal * pRGB , UINT8 r = 255 , UINT8 g = 255 , UINT8 b = 255 , UINT8 a = 255 )
{
	pRGB->red	= ( FLOAT ) r / 255.0f;
	pRGB->green	= ( FLOAT ) g / 255.0f;
	pRGB->blue	= ( FLOAT ) b / 255.0f;
	pRGB->alpha	= ( FLOAT ) a / 255.0f;
}

inline void RwRGBASet( RwRGBA * pRGB , UINT8 r = 255 , UINT8 g = 255 , UINT8 b = 255 , UINT8 a = 255 )
{
	pRGB->red	=r;
	pRGB->green	=g;
	pRGB->blue	=b;
	pRGB->alpha	=a;
}

struct	ASkySetting
{
	// 타임셋 줄이기 작업.
	enum	TimeSet
	{
		TS_NIGHT2	= 0,	// 00 ~ 05 이전
		TS_DAWN		= 1,	// 05 ~ 07 이전
		TS_DAYTIME1	= 2,	// 07 ~ 13 이전
		TS_DAYTIME2	= 3,	// 13 ~ 18 이전
		TS_EVENING	= 4,	// 19 ~ 21 이전
		TS_NIGHT1	= 5,	// 21 ~ 24 이전
		
		TS_MAX		= 6
	};

	static TimeSet	GetTimeSet( int nHour )
	{
		if		( nHour < 5		)	return TS_NIGHT2	;
		else if	( nHour < 7		)	return TS_DAWN		;
		else if	( nHour < 13	)	return TS_DAYTIME1	;
		else if	( nHour < 18	)	return TS_DAYTIME2	;
		else if	( nHour < 21	)	return TS_EVENING	;
		else						return TS_NIGHT1	;
	}
	static INT32 GetRealTime( TimeSet eTime )
	{
		switch( eTime )
		{
		case ASkySetting::TS_NIGHT2		: return 0	;
		case ASkySetting::TS_DAWN		: return 5	;
		case ASkySetting::TS_DAYTIME1	: return 7	;
		case ASkySetting::TS_DAYTIME2	: return 13	;
		case ASkySetting::TS_EVENING	: return 19	;
		case ASkySetting::TS_NIGHT1		: return 21	;
		}

		return 0;
	}
	static INT32 GetRealTime( INT32 nTime ) { return GetRealTime( ( TimeSet ) nTime ); }

	enum	DataAvailable
	{
		DA_NONE		= 0x00,
		DA_LIGHT	= 0x01,
		DA_FOG		= 0x02,
		DA_CLOUD	= 0x04,
		DA_EFFECT	= 0x08,
		DA_MUSIC	= 0x10,
		DA_WEATHER	= 0x20,
		DA_ALL		= 0x3f	
	};
	
	struct CloudSetting
	{
		INT32		nTexture;	// -1은 no texture
		RwRGBA		rgbColor;	// 구름 칼라값 지정..
		FLOAT		fSpeed	;	// 초속..
	};

	enum	CloudLayer
	{
		CLOUD_FIRST		,	// 먼저 보이는 녀석.
		CLOUD_SECOND	,	// 멀리 보이는 녀석
		CLOUD_LAYER_MAX	
	};

	// 어떤데이타를 가지고 있는지 플래그로 저장..
	// 비트연산자로 조합함.
	SHORT			nDataAvailableFlag	;

	// Light Setting..
	// DA_LIGHT
	RwRGBAReal		rgbSkyTop			;
	RwRGBAReal		rgbSkyBottom		;
	RwRGBAReal		rgbAmbient			;
	RwRGBAReal		rgbDirectional		;

	// Fog Setting..
	// DA_FOG
	RwRGBAReal		rgbFog				;
	FLOAT			fFogDistance		;
	FLOAT			fFogFarClip			;

	// DA_CLOUD
	CloudSetting	aCloud[ CLOUD_LAYER_MAX ]	;
	INT32			nCircumstanceTexture;	// 띠텍스쳐 인덱스.
	RwRGBA			rgbCircumstance		;
	INT32			nSunTexture			;	// 태양.
	RwRGBA			rgbSun				;

	// DA_EFFECT
	INT32			nEffectInterval		;
	INT32			aEffectArray	[ AGCMEVENTNATURE_MAX_EVENT ];
	INT32			aEffectRateArray[ AGCMEVENTNATURE_MAX_EVENT ];	// 0~100 
								// 이펙트가 0 이면 안쓰는 녀석임둥..
								// -1 이면 Permanet
								// 나머지는 합쳐서 100을 그리는 확율집단..

	char			strBackMusic[ EFF2_FILE_NAME_MAX ];

	// DA_WEATHER
	// 기상효과 1 ,2 , 안쓰는게 -1

	enum WEATHER_PARAMETER_1
	{
		USE_LENS_FLARE		= 0x00000001,	// 렌즈 플레어
		SHOW_STAR			= 0x00000002,	// 별 보이기
		SHOW_BREATH			= 0x00000004,	// 임김 표현.
		SHOW_MOON			= 0x00000008,	// 달보이기..
		DISABLE_POINT_LIGHT	= 0x00000010,	// 포인트 라이트를 꺼버림.
		DISABLE_WEATHER		= 0x00000020,	// 날씨 변화를 사용치 않는다. 실내,동굴 .. 등등.

		RESERVED_1			= 0x00000040,	// Reserved..
		RESERVED_2			= 0x00000080,	// Reserved.. 
		RESERVED_3			= 0x00000100,	//  Reserved..

		WEA_DEFAULT			= 0x0000000f	
	};

	INT32			nWeatherEffect1;
	INT32			nWeatherEffect2;

	ASkySetting()
	{
		Reset();
	}

	void	Reset()
	{
		// 모든 데이타 조합..
		nDataAvailableFlag	= DA_NONE;	// 일단 아무것도 없는거으로 설정함.

		RwRGBARealSetByte( &rgbSkyTop		, 66 , 131 , 195	, 255 );
		RwRGBARealSetByte( &rgbSkyBottom	, 195 , 223 , 232	, 255 );
		RwRGBARealSetByte( &rgbDirectional	, 119 , 112 , 93	, 255 );
		RwRGBARealSetByte( &rgbAmbient		, 80 , 80 , 80		, 255 );
		RwRGBARealSetByte( &rgbFog			, 193 , 221 , 228	, 255 );

		fFogDistance	= 0.0f	;
		fFogFarClip		= 100000.0f	;

		aCloud[ CLOUD_FIRST		].nTexture	= 3		;
		aCloud[ CLOUD_FIRST		].fSpeed	= 1.0f	;
		RwRGBASet( &aCloud[ CLOUD_FIRST		].rgbColor  , 255 , 255 , 255 , 0 );

		aCloud[ CLOUD_SECOND	].nTexture	= -1	;
		aCloud[ CLOUD_SECOND	].fSpeed	= 1.0f	;
		RwRGBASet( &aCloud[ CLOUD_SECOND	].rgbColor  , 255 , 255 , 255 , 0 );

		nCircumstanceTexture	= -1;
		RwRGBASet( &rgbCircumstance  , 255 , 255 , 255 , 0 );

		nSunTexture	= -1;
		RwRGBASet( &rgbSun  , 255 , 255 , 255 , 0 );
			
		nEffectInterval	= 1000					;
		for( int i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
		{
			aEffectArray		[ i ] = 0	;	// 0이면 안쓰는것..
			aEffectRateArray	[ i ] = 0	;	// 0이면 안쓰는것..
		}

		strncpy( strBackMusic , "" , EFF2_FILE_NAME_MAX );

		nWeatherEffect1 = WEA_DEFAULT;
		nWeatherEffect2 = 0;
	}

	INT32	GetTemporaryEffect()
	{
		INT32	nRandom = rand() % 100;
		INT32	nRate	= 0;
		INT32	i;

		for( i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
		{
			if( aEffectRateArray[ i ] > 0 )
			{
				nRate += aEffectRateArray[ i ];
				if( nRandom < nRate )
				{
					return aEffectArray[ i ];
				}
			}
		}

		// 못찾으면
		// 처음것..
		for( i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
		{
			if( aEffectRateArray[ i ] > 0 )
				return aEffectRateArray[ i ];
		}
		
		// ERROR
		return 0;
	}
};

inline INT32 __MakeSkyTemplateIndexForMapData( int nTemplate , FLOAT fDistance )
{
	// 마고자 (2004-01-02 오후 5:05:00) : nTemplate 는 작은 숫자.
	// 그리고 Distance는 int 변환해서 2바이트로 사용.
	// 즉. 상위 2바이트는 template , 하위 2바이트는 인트화된 거리.
	// Distance는 / 타일 거리 를 해서 저장함.

	fDistance /= MAP_STEPSIZE;

	return	( ( nTemplate % 0xffff ) << 16 ) |
			( ( ( INT32 ) fDistance ) % 0xffff );
}

inline INT32 __GetSkyTemplateIndexFromMapData( INT32 nIndex )
{
	return nIndex >> 16;
}

inline FLOAT __GetSkyTemplateDistanceFromMapData( INT32 nIndex )
{
	// 스탭사이즈를 다시 곱함..
	return ( ( FLOAT ) ( nIndex & 0xffff ) * MAP_STEPSIZE );
}

struct	ASkyQueueData : public ASkySetting
{
	ASkyQueueData() : bAvailable( FALSE ) {}

	BOOL	bAvailable;
	// 듀레이션 추가..
	INT32	nDuration;
};

// 클라이언트 데이타 추가..
struct AgcdSkySet
{
	ASkySetting		m_aSkySetting[ ASkySetting::TS_MAX ]	; // 24시간..
};

struct	ASkyTexture
{
	INT32		nIndex			;
	char		strName[ 256 ]	;
	//RwTexture *	pTexture		;	저장하지 않게 함..
};


// 마고자 (2004-06-09 오전 10:39:09) : 
// 별찍기 버텍스 포멧.
struct D3DStarVertex {
	FLOAT	x,y,z;
	DWORD	color;
};
#define D3DFVF_STARVERTEX	( D3DFVF_XYZ | D3DFVF_DIFFUSE )

class AgcmEventNature : public AgcModule  
{
#ifdef _DEBUG
public:
	static BOOL		m_bInstanced			;
#endif
public:
	enum CLOUD_LAYER
	{
		FIRST_CLOUD			,
		FIRST_CLOUD_FADING	,
		SECOND_CLOUD		,
		SECOND_CLOUD_FADING	,
		CLOUD_ATOMIC_LAYER_MAX
	};
	// Fog에 관련된 Setting들
	enum
	{
		CURRENT_SKY	,	// 현재 스카이.
		NEXT_SKY	,	// 바뀌는 중인 스카이.
		WAIT_SKY	,	// 다음에 바뀌어야할 스카이.
		MAX_SKY		
	};


	// Circumstance 텍스쳐 레이어
	enum
	{
		FORE_TEXTURE	,
		BACK_TEXTURE	,
		MAX_CIRCUMSTANCE_TEXTURE		
	};

	// 마고자 (2004-06-07 오후 3:24:15) : 맵툴 디버그 플래그
	enum
	{
		SKY_DONOTCHANGELIGHTCOLOR	= 0x0001,	// Directional , Ambient 칼라 변경을 막음.
		SKY_DONOTCHANGELIGHTPOSITION= 0x0002,	// 디렉셔널 라이트의 위치 변경을 멈추게 함.
		SKY_DONOTCHANGESKYCOLOR		= 0x0004,	// 하늘 외벽의 칼라 변경을 멈춤
		SKY_DONOTCHANGEFOG			= 0x0008,	// 포그 변경을 막음.
		SKY_DONOTCHANGECLOUD		= 0x0010,	// 하늘 텍스쳐 변경을 막음
		SKY_DONOTCHANGECIRCUMSTANCE	= 0x0020,	// 띠첵스쳐 변경을 막음
		SKY_DONOTCHANGEEFFECT		= 0x0040,	// 이펙트 플레이를 막음.
		SKY_DONOTCHANGEBGM			= 0x0080,	// 배경음악 플레이를 막음.

		SKY_DONOTMOVE				= 0x0100,	// 스카이 박스 이동을 못하게 함..
		SKY_DONOTTIMECHANGE			= 0x0200,	// 시간에 의한 하늘의 변화를 막는다.
												// 즉.. 시간만 멈추고 하늘은 바뀔지도 모른다.

		SKY_DONOTCHANGE				= 0x00ff,	// 스카이 자체를 변화시키는 것을 금지함.
	};
	static	FLOAT	__fsSunAngle;

protected:
	BOOL			m_bInit					;	// Init 돼었나?

	ApmMap			*	m_pcsApmMap			;
	AgcmMap			*	m_pcsAgcmMap		;
	AgpmEventNature	*	m_pcsAgpmEventNature;
	AgcmRender		*	m_pcsAgcmRender		;
	AgpmTimer		*	m_pcsAgpmTimer		;
	ApmEventManager	*	m_pcsApmEventManager;
	RwTexDictionary	*	m_prwTextureDictionary;	// 텍스쳐 딕셔너리 설정.
	AgcmEff2		*	m_pcsAgcmEff2		;
	AgcmResourceLoader *m_pcsAgcmResourceLoader	;
	ApmObject		*	m_pcsApmObject			;
	AgcmSound		*	m_pcsAgcmSound			;

	AgcmNatureEffect *	m_pcsAgcmNatureEffect	;
//	AgpmEventTeleport*	m_pcsAgpmEventTeleport	;

	// BGM이 있는 디렉토리..
	char			m_strBGMDirectory[ 256 ];

	struct	EffectInfo
	{
		AgcdEffCtrl_Set		*	pEffect		;
		UINT32					lEID		;
		UINT32					uGap		;
		UINT32					uStartTime	;

		EffectInfo()
		{
			pEffect		= NULL	;
			uGap		= 0		;
			uStartTime	= 0		;
			lEID		= 0		;
		}
	};

	EffectInfo m_paEffectPtrAttay [ AGCMEVENTNATURE_MAX_EVENT ];
					// 이펙트 포인터 저장..

	// 주어지는 것들..
	RpLight		*	m_pLightAmbient		;
	RpLight		*	m_pLightDirectional	;

	// 하늘을 구현하는데 사용돼는 텍스쳐들의 리스트..
	AuList< ASkyTexture		>	m_listTexture		;

	FLOAT			m_fSkyRadius		;
	BOOL			m_bFogOn			;
	BOOL			m_bShowSky			;	// 현재 하늘이 표시돼고 있는지 저장..
	UINT64			m_nCurrentTime		;	// 현재시간.. 이것을 기준으로 폴리건을 생성함..
	BOOL			m_bNatureEffectPlaying		;	// 눈비효과 중인가?..
	ASkyQueueData	m_skyCurrentSetting[ MAX_SKY ];	// 스카이의 변화에 대한 정보를 저장함..

	INT32			m_skyMixedUpIndex	[ SECTOR_MAX_NATURE_OBJECT_COUNT ];
	ASkySetting		m_skyMixedUp		[ 24	];	// 스카이 설정을 조합하여 메모리에 저장해둔다.

	// 최근 플레이한 웨더 이펙트 정보
	INT32			m_nLastPlayedWeatherEffect1;
	INT32			m_nLastPlayedWeatherEffect2;

	// 마지막으로 움직인 케릭터의 포지션..
	RwV3d			m_posLastPosition	;

	////////////////////////////////////////////////////////
	// 실제 화면을 표현하는데 필요한 데이타들..
	////////////////////////////////////////////////////////
	// 하늘을 표현하는 폴리건 정보.
	MagImAtomic		m_AtomicSky			;	// 먼저 보이는것
	MagImAtomic		m_aAtomicCloud		[ CLOUD_ATOMIC_LAYER_MAX	]	;	// 구름 아토믹 처리..
	MagImAtomic		m_aAtomicCircustance[ MAX_CIRCUMSTANCE_TEXTURE	]	;	// 띠아토믹.
	MagImAtomic		m_aAtomicSun		[ MAX_CIRCUMSTANCE_TEXTURE	]	;	// 태양.. 달..

	// 쩝... Nature 붙은 Object가 Static으루 먼저 Load되어있을때는 어찌 할 것인지... -_-; 이 부분은 전혀 처리 안되있네...-_-;
	// 일단 임시로 되게만 해놓으니.... 고쳐놓을것..
	AuList< INT32 >	m_listNatureObject;
	
	// Move Sky 에서 관리하는 펑션들..
	BOOL			m_bSkyIsChanging		;	// 현재 하늘이 변하고 있는지 
	UINT64			m_nSkyChangeStartTime	;	// 하늘 별화 시작시간.. 명령을 받은 시간 저장

	BOOL			IsSkyChanging()			{ return m_bSkyIsChanging; }

	// Fog Update를 위한 녀석.
	RwCamera	*	m_pCamera				;
	RwBool			m_bFogUpdate			;	// 포그를 업데이트해야한다..

	void			SetFogUpdate()			{ m_bFogUpdate = TRUE;}
	RwBool			GetFogUpdate()			{ return m_bFogUpdate;}

	void			CloudUVShift( ASkySetting * pSkySet );
	void			SkyChange	( ASkySetting * pSkySet );
					// 레이어가 완전히 바뀐경우의 UV의 전환 처리.

	// 마고자 (2004-06-07 오후 3:19:21) : 
	UINT32			m_uMaptoolDebugInfoFlag	;	// 맵툴에서 스카이 컨트롤 플래그 저장.. (릴리즈에도 들어감..)

	// 마고자 (2004-06-08 오후 4:13:24) : 
	// 별들아~
	enum	STARTYPE
	{
		SMALL_STAR	,
		MEDIUM_STAT	,
		BLINK_STAR	,
		BIG_STAR	,
		MAX_STAR	
	};

	int				m_nStarShowLevel			;
	int				m_anStarCount[ MAX_STAR ]	;

	// 마고자 (2004-06-09 오전 10:50:15) : 잡힌 메모리 정보.
	void *					m_pD3DStarVertex	;
	RwUInt32				m_uStarVertexStride	;
	RwUInt32				m_uStarVertexSize	;
	RwUInt32				m_uStarVertexOffset	;
	RwFrame	*				m_pStarFrame		;

	void			RenderStar	();
	void			BlinkStar	();
	void			DestroyStar	();
	
public:
	int				SetStarShowLevel	( int nLevel )
	{
		int prev = m_nStarShowLevel;
		m_nStarShowLevel = nLevel;
		return prev;
	}
	BOOL			GenerateStarVertex	( int nSmall , int nMedium , int nBlink , int nBig );
	BOOL			LoadStarData		( char * pFilename );

	// 마고자 (2004-06-07 오후 3:40:37) : 멥틀용 디버그 펑션.. 클라이언트에선 쓰이지 않음..
	static BOOL		IsMaptoolBuild			();
	UINT32			GetMaptoolDebugInfoFlag	(){ return m_uMaptoolDebugInfoFlag; }
	UINT32			SetMaptoolDebugInfoFlag	( UINT32 uFlag ){ return m_uMaptoolDebugInfoFlag = uFlag; }

	// 스카이오브젝트 리스트 관리용 .. 
	BOOL			AddSkyObject	( INT32 nID );
	BOOL			RemoveSkyObject	( INT32 nID );

	char *			GetBGMDirectory	() { return m_strBGMDirectory; }
	char *			SetBGMDirectory	( char * pDir ) { strcpy( m_strBGMDirectory , pDir ); return m_strBGMDirectory; }

	BOOL			ModifySky		( MagImAtomic * pAtomic		, RwRGBA * pRgbTop			, RwRGBA * pRgbBottom	);
	BOOL			ModifyAtomic	( MagImAtomic * pAtomic		, INT32 nTexture			, INT32 nAlpha = 255	)
	{
		RwRGBA	rgb = { 255 , 255 , 255 , nAlpha };
		return		ModifyAtomic( pAtomic , nTexture , & rgb );
	}
	BOOL			ModifyAtomic	( MagImAtomic * pAtomic		, INT32 nTexture			, RwRGBA * pRgbColor	);
	BOOL			SetCloud		( INT32 nLayer , ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo , FLOAT fAlpha			);
					// NULL 을 넣으면 노 텍스쳐 처리..
	BOOL			SetCircumstance	( ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo 	, FLOAT fAlpha			);
	BOOL			SetSun			( ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo 	, FLOAT fAlpha			);
	BOOL			FlowCloud		( INT32 nLayer , FLOAT fAmount			);	// 구름 흘러가기..

	BOOL			ChangeSunSize	( FLOAT fRadius = 0.2f );
	
	// 직접 렌더하는 함수..
	void			UpdateProjectionMatrix(RwCamera*	camera);		// beginupdate와 endupdate 안하고 far적용 되도록..gemani(04.08.25)
	void			RenderSky		();
	void			SetSkyHeight	( FLOAT height				);

	// 하늘정보 얻어냄.
	ASkySetting *			GetCurrentSkySetting	()
	{
		return &m_skyCurrentSetting[ CURRENT_SKY ];
	}

	AuList< ASkyTexture	> *	GetTextureList	()
	{
		return & m_listTexture;
	}

	// 스카이 템플릿 관련 펑션..

	// 스카이를 천천히 변화시키는 함수..
	//BOOL	SetNextWeather( ASkySetting * pSkySet , INT32 nDuration = 10000 , BOOL bForce = FALSE );
	BOOL	SetNextWeather( ASkySetting::TimeSet eTimeSet , INT32 nDuration = 10000 , BOOL bForce = FALSE );
	// 스카이를 여러 템플릿데이타를 뭉칭 정보에서 시간을 선택하는 방식으로 변경.

	// 현 스카이 설정을 시간대로 강제로 변화시킨다. 확 변하게됨.
	BOOL	ApplySkySetting		();
	
	// 템플릿 지정함수..
	// 이걸로 지정하면 다음 시간이 지날때 적용 됀다.
	INT32	SetSkyTemplateID( INT32 nID					, INT32 nFlag );
	INT32	SetSkyTemplateID( ASkySetting	* pSkySet	, INT32 nFlag );
		// nFlag 는 어플라이 인덱스..

	// 마고자 (2006-03-10 오전 11:18:13) : 
	// 네이쳐 이펙트 스카이 설정.
	INT32	SetNatureEffectSkyTemplateID( INT32 nID , BOOL bForce );
	void	EndNatureEffectSky( BOOL bForce ) ;

	BOOL	IsNatureEffectPlaying() { return m_bNatureEffectPlaying; }

	// 배경음악은 여기에 리스팅 됀다..
	INT32				m_nCurrentBGMSetIndex;
	vector< char * >	m_arrayBGM			;
	UINT32				m_uLastBGMStarted	;

	void	PlayBGM();

public:
	AgcmEventNature();
	virtual ~AgcmEventNature();

	// 초기화 펑션..
	// 하늘데이타도 디폴트 세팅으로 변경함..
	RwCamera *	SetCamera			( RwCamera * pNewCamera )
	{
		RwCamera * pOldCamera = m_pCamera;
		m_pCamera = pNewCamera;
		return m_pCamera;
	}
	BOOL		Init				( RpLight * pAmbient , RpLight * pDirectional , RwCamera * pCamera );
	BOOL		Init				( RpWorld * pWorld , RpLight * pAmbient , RpLight * pDirectional , RwCamera * pCamera )
	{
		// 이전버젼 호완용..
		return Init( pAmbient , pDirectional , pCamera );
	}

	// 설정 펑션..
	BOOL		SetTime				( UINT64	time				);
	UINT64		GetTime				() { return m_nCurrentTime; }
	BOOL		SetTimeForce		( UINT64	time				);	// 강제로 해당시가의하늘로 바꿔버린다.

	BOOL		SetFogSetting		();									// 포그 설정을 렌더사이클 안에 위치시킨다. 이전 설정을 저장하여 자동 수행시킴..
		// 현재 시간을 설정함. 이것으로 태양또는 달의 위치를 바꿈..
		// 물론 라이트 설정도 변경함..

	BOOL		SetCharacterPosition( AuPOS pos	, BOOL bForce = FALSE	);
		// 케릭터가 현재 위치한 부분을 표시함.
		// 이 함수의 호출로 , 하늘반구의 위치를 케릭터의 위치로 옮김.

	BOOL		ShowSky				( BOOL	bShow		);
	BOOL		IsShowSky			() { return m_bShowSky; }
		// 월드에서 하늘 반구를 제거함..

	// 텍스쳐 리스트 관리
	INT32		AddTexture			( char *	pFilename	, INT32	nIndex = 0	);

	RwTexture *	GetTexture			( INT32		nIndex		);
		// 텍스쳐를 읽은다음에 , 꼭! RwTextureDestroy를 해주어야한다.
		// 자체에선 텍스쳐를 가지고 있지 않을것이다..

	BOOL		RemoveTexture		( INT32		nIndex		);
	INT32		GetTextureIndex		( char *	pFilename	);
	void		RemoveAllTexture	(						);

	// Fog 관련..

	FLOAT			m_fFogDistance			;
	FLOAT			m_fFogFarClip			;
	RwRGBA			m_stFogColor			;

	FLOAT		GetFogDistance	()	{ return m_fFogDistance	;	}
	FLOAT		GetFogFarClip	()	{ return m_fFogFarClip	;	}
	RwRGBA *	GetFogColor		()	{ return &m_stFogColor	;	}

	void		SetSkyRadius		( FLOAT		fRadius		);
	void		SetSkyRadius		( INT32		nRadius		);	// 섹터 길이.
	void		SetFog				(
		BOOL		bFogOn			, FLOAT		fFogDistance	, FLOAT		fFogFarClip	,
		RwRGBA *	pstFogColor		);

	// 저장..
	BOOL		LoadTextureScriptFile	( CHAR	* szPath	= AGCMEVENTNATURE_SCRIPT_FILE_NAME, BOOL bDecryption = FALSE	);
	BOOL		SaveTextureScriptFile	( CHAR	* szPath	= AGCMEVENTNATURE_SCRIPT_FILE_NAME, BOOL bEncryption = FALSE	);
	void		CleanAllData			();	// 읽어들였던 데이타 초기화..

	
	// 스카이 업데이트
	void		MoveSky				();

	// 맵툴용 펑션으로 , 하늘의 구름 디테일을 조절하여 아토믹을 재 생성한다.
	void		SetCloudZoomRate	( FLOAT fCloudZoomRate );

public:
	// 스카이 셋 관련..
	// AgpdSkySet로부터 AgcdSkySet를 가져온다.
	AgcdSkySet *	GetSkySetClientData( AgpdSkySet	* pstSkySet );
	INT16			m_nSkySetAttachIndex;


//protected:
public:
		// 하늘 반구 만드는데 필요한 텍스쳐와 스크립트를 읽어들임..

	// 태영의 고도를 설정.. fAngle은 Radian 으로 처리함.
	// 동쪽에서 뜰때가 0 으로 설정 , 그리고 PI값이 돼면 땅으로 가라 앉음..
	// 땅 어느정도 이상내려가면 표시하지 않음..
	BOOL		SetSunAngle			( FLOAT	fAngle		);
	BOOL		SetMoonAngle		( FLOAT	fAngle		);

	// 반구를 만든다.
	BOOL		CreateSkyAtomic				( MagImAtomic * pAtomic );
	BOOL		CreateCloudAtomic			( MagImAtomic * pAtomic , FLOAT fCloudZoomRate = 4.0f );
	BOOL		CreateCircumstanceAtomic	( MagImAtomic * pAtomic );
	BOOL		CreateSunAtomic				( MagImAtomic * pAtomic	);

	// 맵툴에서 쓰는거.
	static RpAtomic		*	CreateSkyAtomic		();
	static RtWorldImport *	_CreateHalfSphere	();	// 반구 만들기..
	static BOOL				ModifyAtomic		( RpAtomic * pAtomic , INT32 nAlpha	);

public:

	// Virtual 펑션들
	BOOL	OnAddModule		();

	BOOL	OnInit			();
	BOOL	OnDestroy		();
	void		OnLuaInitialize( AuLua * pLua );

	BOOL	OnIdle			(UINT32 ulClockCount);


	BOOL	SetCallbackSkyChange		(ApModuleDefaultCallBack pfCallback, PVOID pClass){	return SetCallback( AGCMEVENTNATURE_CB_ID_SKYCHANGE		, pfCallback, pClass);}
	BOOL	SetCallbackLightUpdate		(ApModuleDefaultCallBack pfCallback, PVOID pClass){	return SetCallback( AGCMEVENTNATURE_CB_ID_LIGHTUPDATE	, pfCallback, pClass);}
		// pData		: Directional Light RwRGBAReal
		// pCustData	: Ambient Light RwRGBAReal
	BOOL	SetCallbackBGMUpdate		(ApModuleDefaultCallBack pfCallback, PVOID pClass){	return SetCallback( AGCMEVENTNATURE_CB_ID_BGMUPDATE	, pfCallback, pClass);}
		// pData		: current played BGM file name ( char * )
	
	// Callback
	static BOOL	OnMyCharacterPositionChange	( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBOnSetSelfCharacter		( PVOID pData, PVOID pClass, PVOID pCustData );
//	static BOOL	CBOnTeleportStart			( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBOnSetAutoLoadChange		( PVOID pData, PVOID pClass, PVOID pCustData );

	// SkySet Data 생성자, 파괴자
	static BOOL	OnSkySetCreate	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	OnSkySetDestroy	(PVOID pData, PVOID pClass, PVOID pCustData);

	// SkySet Streaming Callback
	static BOOL	SkySetStreamReadCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	SkySetStreamWriteCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);

	static BOOL	CBOnAddObject		( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBOnRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBOnNatureEffectStart	( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBOnNatureEffectEnd		( PVOID pData, PVOID pClass, PVOID pCustData );

	INT32	GetDefaultSky	( FLOAT fX , FLOAT fZ );
	BOOL	AddEventNatureObject( ApWorldSector * pSector , ApdObject * pcsApdObject , INT32 nTemplate , FLOAT fRadius , UINT32 nSkyFlag , INT32 nDepth = 1 );

	static BOOL	CBLoadSector		(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	UpdateSectorNatureObject( ApWorldSector * pSector , INT32 uID );

	static BOOL CBReceiveTime		(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBNature			(PVOID pData, PVOID pClass, PVOID pCustData);

	// 섹터 오버..
	AuList< ApWorldSector * >	m_listUpdateSectorNatureInfo		;
	ApCriticalSection			m_MutexlistUpdateSectorNatureInfo	;
	void	UpdateSectorNatureInfo	( ApWorldSector * pSector );

	struct SectorNatureUpdateInfo
	{
		INT32	uSectorX	;
		INT32	uSectorZ	;
		INT32	uObjectID	;
	};

	AuList< SectorNatureUpdateInfo >	m_listUpdateSectorNature	;

public:
	// 툴에서 사용하는 펑션들..
	static BOOL	ResampleTexture		( char * pSource , char * pDestination , INT32 nWidth , INT32 nHeight );
	static BOOL	__SetTime			( AgcmEventNature * pEventNature , INT32 hour , INT32 minutes );
	static void	__SetFog			( AgcmEventNature * pEventNature , 
		BOOL		bFogOn			, FLOAT		fFogDistance	, FLOAT		fFogFarClip	,
		RwRGBA *	pstFogColor		);

	static void		__MoveSky		( AgcmEventNature * pEventNature );
	static void		__SetSkyRadius	( AgcmEventNature * pEventNature ,  FLOAT		fRadius		);
	static INT32	__SetSkyTemplateID	( AgcmEventNature * pEventNature , ASkySetting	* pSkySet	, INT32 nFlag );

	static BOOL		__PreRenderCallback(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// 환경변수.
	static FLOAT	AgcmEventNature::SKY_RADIUS				;
	static FLOAT	AgcmEventNature::SKY_HEIGHT				;
	static FLOAT	AgcmEventNature::CIRCUMSTANCE_RADIUS	;
	static FLOAT	AgcmEventNature::CIRCUMSTANCE_HEIGHT	;

	// 태양 위치 
	FLOAT			m_fSkyCircleRadius;
	void			SetSkyCircleRadius();				//환경변수로 sky구의 반지름을 구한다

	RwV3d			m_posSun;
	void			UpdateSunPosition();				//light direction 설정 변경시에 sun position을 다시 구한다.
	BOOL			GetSunPosition	( RwV3d * pV3d);
				// return TRUE	: Sun
				// return FALSE	: Moon

	BOOL			EnableFog	( BOOL bEnable );
	BOOL			IsFogOn		()	{ return m_bFogOn		;	}

	// 마고자 (2005-03-31 오후 6:20:06) : 로드거리에따라 값 변화시킴.
	FLOAT			m_fFogAdjustFarClip		;

	FLOAT			m_fNearValue			;
	FLOAT			m_fNormalValue			;

	BOOL			m_bSkyFogDisable			;

	RwV3d			GetLastPosition() { return m_posLastPosition; }

	//add by dobal
public:
	bool			m_bLostDevice;
	static AgcmEventNature*	m_pThis;
	static rwD3D9DeviceRestoreCallBack OldD3D9RestoreDeviceCB;
	static rwD3D9DeviceReleaseCallBack OldD3D9ReleaseDeviceCB;

	static void		NewD3D9ReleaseDeviceCB();
	static void		NewD3D9RestoreDeviceCB();

	void			OnLostDevice();
	void			OnResetDevice();

private :
	void			_ChangeEnvironmentSound( void );
};

extern FLOAT	AGCEVENTNATURE_DEFAULT_SKY_HEIGHT;
extern UINT32	__uSkyChangeGap;

#endif // !defined(AFX_AGCMEVENTNATURE_H__03F92BCE_D3D8_4B8B_88FA_F351422C3D5E__INCLUDED_)
