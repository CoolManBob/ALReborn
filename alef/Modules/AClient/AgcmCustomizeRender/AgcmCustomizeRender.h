#ifndef _AGCMCUSTOMIZERENDER_H_
#define	_AGCMCUSTOMIZERENDER_H_

#include "AgcModule.h"
#include "rphanim.h"

//@{ Jaewon 20050630
// ;)
class AgcmResourceLoader;
//@} Jaewon
class AgcmRender;
class AgcmCharacter;
struct AgcdCharacter;

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmCustomizeRenderD" )
#else
#pragma comment ( lib , "AgcmCustomizeRender" )
#endif
#endif

class AgcmCustomizeRender : public AgcModule
{
public:
	enum
	{
		STATE_PREVIEW,
		STATE_SETTING,
	};

	enum {
		CS_NONE,		//camera state
		CS_LEFT_TURN,
		CS_RIGHT_TURN,
		CS_ZOOM_IN,
		CS_ZOOM_OUT,
		CS_AT_UP,
		CS_AT_DOWN,
	};
	AgcmCustomizeRender();
	virtual ~AgcmCustomizeRender();

	static int			GetHairNum( AgcdCharacter* pstAgcdCharacter );
	static int			GetFaceNum( AgcdCharacter* pstAgcdCharacter );

	void				SetState( int nState );
	int					GetState() const;

	void				SetWidth( int nWidth );
	void				SetHeight( int nHeight );

	void				Init();
	void				Start( AgcdCharacter* pstAgcdCharacter );
	void				End();
	bool				IsStart() const { return m_bStart; }

//Preview
	void				LeftTurnOn();
	void				RightTurnOn();
	void				LeftTurnOff();
	void				RightTurnOff();
	bool				IsLeftTurnOn()  const;
	bool				IsRightTurnOn() const;

	void				ZoomInOn();
	void				ZoomOutOn();
	void				ZoomInOff();
	void				ZoomOutOff();
	bool				IsZoomInOn()  const;
	bool				IsZoomOutOn() const;

	void				AtUpOn();
	void				AtDownOn();
	void				AtUpOff();
	void				AtDownOff();
	bool				IsAtUpOn()   const;
	bool				IsAtDownOn() const;

//Setting
	void				SetCameraPosY( float fY );
	void				SetCameraPosZ( float fZ );
	void				SetTargetPosY( float fY );
	void				SetPreviewFar();
	void				SetPreviewNear();
	void				SetZoom( float fZoom );

	void				SetFace( INT32 nFaceID );
	void				SetHair( INT32 nFaceID );

	void				Render();
	RwTexture*			GetTexture();

	void				DrawQuad();

public:
	BOOL				OnAddModule();
	BOOL				OnInit();
	BOOL				OnDestroy();

	//. 2006. 1. 11. Nonstopdj
	//. public meber·Î º¯°æ
	void				OnLostDevice();
	void				OnResetDevice();

	BOOL				SetWorld( RpWorld* pWorld );

	static AgcmCustomizeRender*	m_pThis;

	static RpAtomic*	RenderAtomicCB( RpAtomic*	pstAtomic, void* pvData );
	static RpAtomic*	RemoveCustomizeAtomicCB( RpAtomic*	pstAtomic, void* pvData );


private:
	void				ClumpClone();

	bool				CreateRenderTarget( RwBool bIsInit = true );
	void				DestoryRenderTarget( bool bLostDevice = false );

	bool				CreateCamera();

	void				AttachFace();
	void				AttachHair();
	void				DetachFace();
	void				DetachHair();

	void				SetCameraPos();

	int					m_nState;
	int					m_nCamreaState;
	RwTexture*			m_pTexture;
	int					m_nWidth, m_nHeight;
	int					m_nScreenWidth, m_nScreenHeight;
	RwRaster*			m_pSceneRenderTargetTexture;
	RwRaster*			m_pSceneRenderTarget;
	RwRaster*			m_pZBuffer;
	bool				m_bDoNotCreate;
	RpClump*			m_pClump;
	RwCamera*			m_pCamera;

	bool				m_bStart;

	//@{ Jaewon 20050630
	// ;)
	AgcmResourceLoader*	m_pcsAgcmResourceLoader;
	//@} Jaewon

	AgcmCharacter*		m_pcsAgcmCharacter;
	AgcdCharacter*		m_pstAgcdCharacter;
	AgcmRender*			m_pcsAgcmRender;

	RpHAnimHierarchy*	m_pInHierarchy;

	int					m_nAttachHairID;
	int					m_nAttachFaceID;
	RpAtomic*			m_pHair;
	RpAtomic*			m_pFace;

	RpWorld*			m_pWorld;
	float				m_fAngle, m_fClumpAngle;

	float				m_fCameraPosY, m_fCameraPosZ;
	float				m_fTargetPosY;
	float				m_fZoom;

	RwV3d				m_vtxNearCamera, m_vtxFarCamera;

	RwMatrix			*m_pmtxDirectional;
};

#endif
