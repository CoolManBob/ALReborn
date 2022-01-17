#ifndef		_AGCMSHADOW_H_
#define		_AGCMSHADOW_H_

#include "AgcModule.h"

#include <list>
#include <algorithm>

#define		IM3DVERTEXDATASIZE		36
#define		SHADOW_LAND_YOFFSET		4.0f
#define		SHADOW_T1_UPDATEMAX		30

struct ShadowInfo : public ApMemory< ShadowInfo, 2000 >
{
	enum eLevel
	{
		eLevelDetail	= 1,
		eLevelCircle	= 2,
		eLevelNon		= 3,
	};

	ShadowInfo( RpClump* pClump ) : clump(pClump)		{	}

	bool	operator == ( const ShadowInfo&	lhs )	{	return clump == lhs.clump;	}
	bool	operator == ( const RpClump* pclump )	{	return clump == pclump;		}

	INT8		level;					// 그림자 레벨(def - 1)  2 - circle , 3- draw none
	BOOL		fade;
	RpClump*	clump;
	
	RwSphere 	sphere;
	FLOAT		zoneradius;
	RwV3d		spherecenter;			// center offset

	RwV3d		colboxsup;
	RwV3d		colboxinf;

	FLOAT		cam_near;
	FLOAT		cam_far;
	RwV2d		view_size;
	RwMatrix	camMatirx;

	RwMatrix	invMatrix;
	RwV3d		at;						// camera at vector(cull용)
};
typedef std::list< ShadowInfo* >	ShadowInfoList;
typedef ShadowInfoList::iterator	ShadowInfoListItr;

class AgcmMap;
class AgcmRender;
class AgcmCharacter;
class AgcmShadow : public AgcModule
{
public:
	AgcmShadow();
	~AgcmShadow();

	BOOL	OnAddModule();
	BOOL	OnInit();
	BOOL	OnDestroy();
	BOOL	OnIdle(UINT32 ulClockCount);

	BOOL	AddShadow( RpClump* pclump );		// userdata의 level로 detail설정
	void	DeleteShadow( RpClump* pclump );
	void	ShadowDraw( ShadowInfo* pShadowInfo, RwRaster* pRas );

	void	ShadowRender();
	void	InvertRaster(RwRaster* pRas);
	void	Render();
	void	UpdateBoundingSphere(RpClump* pClump);

	BOOL	SetShadowEnable(BOOL bVal);		// Shadow Camera Texture생성/삭제..	
	BOOL	GetShadowEnable()	{	return m_bDrawShadow;	}
	
	//ShadowFunc
	void	ShadowDistUp()		{	m_fShadowDistFactor = min( m_fShadowDistFactor += 0.05f, 1.0f );	}
	void	ShadowDistDown()	{	m_fShadowDistFactor = max( m_fShadowDistFactor -= 0.05f, 0.1f );	}

	static RpAtomic* CalcBoundingSphere(RpAtomic * atomic, void *data);
	static BOOL CB_POST_RENDER( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_Update( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CB_Release( PVOID pData, PVOID pClass, PVOID pCustData );

private:
	int		GetLevel( RpClump* pClump );

	//Not Use
	RwBool	ShadowRasterResample(RwRaster * destRaster,RwRaster * sourceRaster);
	RwBool	ShadowRasterBlur(RwRaster * shadowRaster,RwUInt32 numPass);
	RwBool	Im2DRenderQuad(RwReal x1, RwReal y1, RwReal x2, RwReal y2, RwReal z, RwReal recipCamZ, RwReal uvOffset);

public:
	AgcmMap*			m_pcmMap;
	AgcmRender*			m_pcRender;
	AgcmCharacter*		m_pAgcmCharacter;
		
	//Shadow Render Status
	FLOAT				m_fShadowStrength;		// Shadow 농도(0.0f ~ 1.0f)

	RwTexture*			m_pcCircleShadow;		// 원 그림

	FLOAT				m_fLODOriginalShadowDistance[3][4];		// 행 level 열 lod level(3- none)
	FLOAT				m_fShadowDistFactor;					// 0.1f ~ 1.0f

	UINT32				m_ulCurTick;
	UINT32				m_ulLastTick;
	UINT32				m_ulCurTickDiff;

	RpWorld*			m_pWorld;
	RwCamera*			m_pWorldCamera;
	RpLight*			m_pGLightAmbient;	// 광원
	RpLight*			m_pGLightDirect;
	
	//Shadow Render Devices
	RwCamera*			m_pcShadowCamera;
	RwCamera*			m_pcShadowIPCamera;

	ShadowInfoList		m_listShadowInfo;
	
	int					m_iShadowDrawMaxNum;
	
	ShadowInfo*			m_pMainCharacterTexUpdate;
	ShadowInfo*			m_listTexUpdate1[SHADOW_T1_UPDATEMAX];

	RwIm2DVertex		m_vTriFan[4];
	
	// camera texture pool
	RwRaster*			m_pcShadowRasterMain512;						// 512로 균일하게 생성하자 
	RwRaster*			m_pcShadowRasterAA256;							// 256로 생성(Anti-Aliase용)
	RwRaster*			m_pcShadowRasterBlur256;						// 256로 생성(blur용)

	RwRGBA				m_colorBackGround;

private:
	BOOL				m_bDrawShadow;
};


#endif       //_AGCMSHADOW_H_	