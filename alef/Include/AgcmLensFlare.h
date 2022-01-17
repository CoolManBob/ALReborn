#ifndef __AGCMLENSFLARE_H__
#define __AGCMLENSFLARE_H__

#include "AgcModule.h"
#include "ApBase.h"
#include "ApModule.h"

#include "rwcore.h"
#include "rpworld.h"
#include "rpcollis.h"
#include "rtintsec.h"
#include "rtpick.h"
#include "rtimport.h"

#include "AgcmRender.h"
#include "AgcmEventNature.h"
#include "AgcmMap.h"
#include "AgpmTimer.h"

#include "AcDefine.h"

#include "AgcmCharacter.h"

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmLensFlareD" )
#else
#pragma comment ( lib , "AgcmLensFlare" )
#endif
#endif

typedef struct tag_FlareInfo
{
	int		tex_id;
	float	fSize;
	float	fLinePos;
	int		red;
	int		green;
	int		blue;
	int		alpha;
}FlareInfo;

#define		FLARE_NUM		20
#define		FLARE_TEX_NUM	8

#define		FLARE_INTENSITY_MIN		0.0f
#define		FLARE_INTENSITY_MAX		1.0f

#define		FLARE_INI_NAME_FILE_NAME		"FN"
#define		FLARE_INI_FLARE_NUMBERS			"Number"
#define		FLARE_INI_FLARE_SETTINGS		"SET"

#define		FLARE_INI_FLARE_OPTION			"Moon"

class AgcmLensFlare : public AgcModule
{
public:
	void				ResetContent();

	enum FLARETYPE
	{
		FT_SUN	,
		FT_MOON	
	};

	struct	LFSet
	{
		int					nType					;
		int					iFlareNum				;
		RwTexture*			pFlares	[ FLARE_TEX_NUM	];
		FlareInfo			stInfo	[ FLARE_NUM		];

		LFSet():nType( FT_SUN ),iFlareNum( 0 )
		{
			ZeroMemory( stInfo, sizeof(FlareInfo) * FLARE_NUM);
			ZeroMemory( pFlares, sizeof(RwTexture*) * FLARE_TEX_NUM);
		}

		void	Empty()
		{
			nType = FT_SUN;
			ZeroMemory( stInfo, sizeof(FlareInfo) * FLARE_NUM);
			iFlareNum	= 0;

			for( int i = 0 ; i < FLARE_TEX_NUM ; i ++ )
			{
				if( pFlares[ i ] )
				{
					RwTextureDestroy( pFlares[ i ] );
					pFlares[ i ] = NULL;
				}
			}
		}
	};

	LFSet				m_stLFSet;
	LFSet *				GetTemplate() { return & m_stLFSet; };

	RwCamera*			m_pWorldCamera;
	RpWorld*			m_pWorld;
	
	AgcmRender		*	m_pcRender		;
	AgcmEventNature	*	m_pcEventNature	;
	AgcmMap			*	m_pcAgcmMap		;
	AgpmTimer		*	m_pcAgpmTimer	;
	ApmMap			*	m_pcsApmMap		;
	
	float				m_fIntensityBorder;
	
	RwV2d				m_ptrLightScreenPos;
	BOOL				m_bObscured	;
	BOOL				m_bActive	;
	BOOL				m_bInScene	;
	
	BOOL				m_bInRoom;

	bool				m_bUpdateThisFrame;							// Tuner에서 설정

	// Camera관련 variables
	int					m_iScreenWidth;
	int					m_iScreenHeight;
	int					m_iCenterOfScreenX;
	int					m_iCenterOfScreenY;
	float				m_fRecipZ;

	My2DVertex			m_vTriFan4[4];

public:
	AgcmLensFlare();
	~AgcmLensFlare();

	BOOL				OnInit();
	BOOL				OnAddModule();
	BOOL				OnIdle(UINT32 ulClockCount);	
	BOOL				OnDestroy();

	inline	void		SetEnable(BOOL	bEnable) { m_bActive = bEnable; }

	void				OnCameraStateChange(CAMERASTATECHANGETYPE	ctype);

	static 	RpAtomic*	CBLightRayIntersect	(RpIntersection * intersection, RpWorldSector * sector, RpAtomic * atomic, RwReal distance, void *data);
	static	BOOL		CB_PRE_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData );
	static	BOOL		CB_POST_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData );

	static	BOOL		CBSkyChange	( PVOID	pData, PVOID pClass, PVOID pCustData );

	float				GetWorldPosToScreenPos		( RwV3d * pWorldPos , RwV2d * pPoint );	

	void				SetMyWorldCamera(RwCamera* pCam);
	void				CameraStatusChange();						// Camera설정 변경 vertexdata 다시 set

	inline		void	SetMyWorld(RpWorld* pWorld) { m_pWorld = pWorld; }

	BOOL				StreamReadTemplate(CHAR *szFile, BOOL bDecryption);
	BOOL				StreamWriteTemplate(CHAR *szFile, BOOL bEncryption);

	//@{ Jaewon 20050119
private:
	IDirect3DQuery9 *m_pOcclusionQuery;
	// Len flare intensity following the occlusion amount of the sun. (0.0 ~ 1.0)
	FLOAT m_fIntensity;
	FLOAT m_fCurrentIntensity;

	// Check the result of the previous query and if it's done, update the visibility stat & issue a new query.
	void checkAndUpdateOcclusionQuery();

public:
	// d3d stuff
	void onLostDevice();
	void onResetDevice();
	//@} Jaewon

	void	RenderLensFlare();

public:
	// Debugging 용 펑션들
	void	ChangeLensFlareTemplate( const std::string& strFilename );
};


#endif // __AGCMLENSFLARE_H__
