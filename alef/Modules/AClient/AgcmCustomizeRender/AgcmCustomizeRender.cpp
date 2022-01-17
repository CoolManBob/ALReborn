#include "AgcmCustomizeRender.h"
#include "AgcmCharacter.h"
#include "AgcmRender.h"
#include "AcuMathFunc.h"
#include "AgcmCharacter.h"
//@{ Jaewon 20050630
// ;)
#include "AgcmResourceLoader.h"
//@} Jaewon

#define AGCMITEM_NODE_ID_HEAD						1

extern "C"
{
	extern LPDIRECT3DSURFACE9           _RwD3D9RenderSurface;

	extern RwBool
		_rwD3D9SetRenderTarget(RwUInt32 index,
		LPDIRECT3DSURFACE9 rendertarget);
}

AgcmCustomizeRender* AgcmCustomizeRender::m_pThis = NULL;

//. 2006. 1. 11. Nonstopdj
//. static member가 아닌 local static member로 수정.
static rwD3D9DeviceRestoreCallBack OldD3D9RestoreDeviceCB = NULL;
static rwD3D9DeviceReleaseCallBack OldD3D9ReleaseDeviceCB = NULL;

static
void NewD3D9ReleaseDeviceCB()
{
	((AgcmCustomizeRender*)g_pEngine->GetModule("AgcmCustomizeRender"))->OnLostDevice();

	if( OldD3D9ReleaseDeviceCB )
		OldD3D9ReleaseDeviceCB();
}

static
void NewD3D9RestoreDeviceCB()
{
	if( OldD3D9RestoreDeviceCB )
		OldD3D9RestoreDeviceCB();

	((AgcmCustomizeRender*)g_pEngine->GetModule("AgcmCustomizeRender"))->OnResetDevice();
}

struct SCREEN_VERTEX
{
	D3DXVECTOR4 pos;
	D3DXVECTOR2 tex;

	static const DWORD FVF;
};
const DWORD SCREEN_VERTEX::FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;

struct rwD3D9Palette
{
	PALETTEENTRY    entries[256];
	RwInt32     globalindex;
};

struct rwD3D9RasterExt
{
	LPDIRECT3DTEXTURE9      texture;
	rwD3D9Palette           *palette;
	RwUInt8                 alpha;              /* This texture has alpha */
	RwUInt8                 cube : 4;           /* This texture is a cube texture */
	RwUInt8                 face : 4;           /* The active face of a cube texture */
	RwUInt8                 automipmapgen : 4;  /* This texture uses automipmap generation */
	RwUInt8                 compressed : 4;     /* This texture is compressed */
	RwUInt8                 lockedMipLevel;
	LPDIRECT3DSURFACE9      lockedSurface;
	D3DLOCKED_RECT          lockedRect;
	D3DFORMAT               d3dFormat;          /* D3D format */
	LPDIRECT3DSWAPCHAIN9    swapChain;
	HWND                    window;
};

const float M3D_PI = 3.141592654f;
inline float ToRadian( float fDegree )
{
	return ((fDegree) * (M3D_PI / 180.0f));
}

inline float GetAngleXZ( const RwV3d* vtxV )
{
	float fAngle = acosf(vtxV->z)*180.f/M3D_PI;
	if( vtxV->x < 0 ) {
		fAngle = 360.f - fAngle;
	}
	return fAngle;
}

// renderware hacks
static RwInt32 g_RwD3D9RasterExtOffset = 0;

#define RASTEREXTFROMRASTER(raster) \
	((rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + g_RwD3D9RasterExtOffset))

RwRGBAReal g_colAmbientLightColor;		//temp
RwRGBAReal g_colDirectionalLightColor;	//temp

inline void GetMatrixRactangular( RwMatrix* pOut, const RwV3d *pAt ) //At에 대한 직교 matrix를 구함.
{
	RwV3d *pOutPos	= RwMatrixGetPos( pOut );
	RwV3d *pOutAt	= RwMatrixGetAt( pOut );
	RwV3d *pOutRight= RwMatrixGetRight( pOut );
	RwV3d *pOutUp	= RwMatrixGetUp( pOut );

	pOutAt->x = pAt->x;	pOutAt->y = pAt->y;	pOutAt->z = pAt->z;
	pOutUp->x = 0;		pOutUp->y = 1.f;	pOutUp->x = 0;

	RwV3dNormalize( pOutAt, pOutAt );

	RwV3dCrossProduct( pOutRight, pOutUp, pOutAt );
	RwV3dNormalize( pOutRight, pOutRight );

	RwV3dCrossProduct( pOutUp, pOutAt, pOutRight );
	RwV3dNormalize( pOutUp, pOutUp );
}

AgcmCustomizeRender::AgcmCustomizeRender()
{
	SetModuleName("AgcmCustomizeRender");
	EnableIdle(TRUE);

	m_pThis = this;
	m_bDoNotCreate = true;
	m_pWorld = NULL;

	m_nScreenWidth = m_nScreenHeight = 0;

	Init();
}

AgcmCustomizeRender::~AgcmCustomizeRender()
{
	OnDestroy();
}
//
void AgcmCustomizeRender::Init()
{
	m_pSceneRenderTargetTexture = NULL;
	m_pSceneRenderTarget = NULL;
	m_pZBuffer = NULL;
	m_nWidth = 209;
	m_nHeight = 370;
	//	m_nWidth = 256;
	//	m_nHeight = 256;

	//@{ Jaewon 20050630
	// ;)
	m_pcsAgcmResourceLoader = NULL;
	//@} Jaewon

	m_pcsAgcmCharacter = NULL;
	m_pstAgcdCharacter = NULL;
	m_pcsAgcmRender = NULL;

	m_pClump = NULL;
	m_pCamera = NULL;

	m_bStart = false;

	m_pHair = NULL;
	m_pFace = NULL;

	m_nAttachHairID = 0;
	m_nAttachFaceID = 0;

	m_pInHierarchy = NULL;

	m_fAngle = 0.f;
	m_fClumpAngle = 0.f;

	m_pTexture = NULL;

	m_vtxNearCamera.x = m_vtxNearCamera.y = m_vtxNearCamera.z = 0.f;
	m_vtxFarCamera.x = m_vtxFarCamera.y = m_vtxFarCamera.z = 0.f;
	m_fZoom = 1.f;

	m_fCameraPosY = 0.f;
	m_fCameraPosZ = 0.f;
	m_fTargetPosY = 0.f;

	m_nState = STATE_PREVIEW;
	m_nCamreaState = CS_NONE;

	m_pmtxDirectional = NULL;

	ZeroMemory( &g_colAmbientLightColor,	sizeof(g_colAmbientLightColor) );
	ZeroMemory( &g_colDirectionalLightColor,sizeof(g_colDirectionalLightColor) );
}

//call back
RpAtomic* AgcmCustomizeRender::RenderAtomicCB( RpAtomic* pstAtomic, void* pvData )
{
	switch( pstAtomic->iPartID )
	{
	case AGPMITEM_PART_HEAD:
	case AGPMITEM_PART_HAND_LEFT:
	case AGPMITEM_PART_HAND_RIGHT:
	case AGPMITEM_PART_LANCER:
		return pstAtomic;
		break;
	}
	m_pThis->m_pcsAgcmRender->OriginalAtomicRender( pstAtomic );

	return pstAtomic;
}

RpAtomic* AgcmCustomizeRender::RemoveCustomizeAtomicCB( RpAtomic* pstAtomic, void* pvData )
{
	if( m_pThis->m_pstAgcdCharacter == NULL )
		return pstAtomic;

	if( pstAtomic->iPartID == -1 ) {
		RpClumpRemoveAtomic( m_pThis->m_pClump, pstAtomic );
		RwFrame* pstFrame = RpAtomicGetFrame( pstAtomic );
		RpAtomicSetFrame( pstAtomic, NULL );
		if( pstFrame != RpClumpGetFrame( m_pThis->m_pClump ) )
			RwFrameDestroy( pstFrame );
	}

	return pstAtomic;
}

void AgcmCustomizeRender::SetCameraPos()
{
	RwFrame	*pstClumpFrame = RpClumpGetFrame( m_pClump );
	if( pstClumpFrame == NULL ) {
		return;
	}
	RwFrame *pCameraFrame= RwCameraGetFrame( m_pCamera );
	if( pCameraFrame == NULL ) {
		return;
	}

	RwMatrix* pmtxClump	= RwFrameGetLTM( pstClumpFrame );
	RwV3d *pClumpPos	= RwMatrixGetPos( pmtxClump );

	RwMatrix* pmtxCamera= RwFrameGetLTM( pCameraFrame );
	RwV3d *pCameraPos	= RwMatrixGetPos( pmtxCamera );
	RwV3d *pCameraAt	= RwMatrixGetAt( pmtxCamera );

	RwV3d vtxAngle;
	RwV3dAssign( &vtxAngle, RwMatrixGetAt( RwFrameGetLTM(pstClumpFrame) ) );
	RwV3dNormalize( &vtxAngle, &vtxAngle );
	m_fClumpAngle = GetAngleXZ( &vtxAngle );

	float fAddX, fAddZ;
	RwV3d vtxOrigin;
	RwV3dAssign( &vtxOrigin, pClumpPos );
	fAddX = fAddZ = 0.f;

	switch( m_nState )
	{
	case STATE_SETTING:
		{
			fAddX = sinf( ToRadian( m_fClumpAngle ) )*m_fCameraPosZ;
			fAddZ = cosf( ToRadian( m_fClumpAngle ) )*m_fCameraPosZ;

			vtxOrigin.y += m_fCameraPosY;
		}
		break;
	case STATE_PREVIEW:
		{
			float s		= fabsf( m_fZoom - m_vtxNearCamera.z ) / fabsf( m_vtxFarCamera.z - m_vtxNearCamera.z );
			float fCurY = m_vtxNearCamera.y + s * (m_vtxFarCamera.y - m_vtxNearCamera.y);
			float fCurZ = m_fZoom;

			fAddX = sinf( ToRadian( m_fAngle + m_fClumpAngle ) )*fCurZ;
			fAddZ = cosf( ToRadian( m_fAngle + m_fClumpAngle ) )*fCurZ;

			vtxOrigin.y += fCurY;
		}
		break;
	}

	pCameraPos->x = vtxOrigin.x+fAddX;
	pCameraPos->y = vtxOrigin.y;
	pCameraPos->z = vtxOrigin.z+fAddZ;
	RwV3dSub( pCameraAt, &vtxOrigin, pCameraPos );
	pCameraAt->y += m_fTargetPosY;
	GetMatrixRactangular( pmtxCamera, pCameraAt );

	if( m_pmtxDirectional ) {
		RwV3d *pDirectionalAt = RwMatrixGetAt( m_pmtxDirectional );
		RwV3d vtxDirectionalPos;
		vtxDirectionalPos.x = sinf( ToRadian( m_fAngle + m_fClumpAngle + 45.f ) );
		vtxDirectionalPos.y = .5f;
		vtxDirectionalPos.z = cosf( ToRadian( m_fAngle + m_fClumpAngle + 45.f ) );
		RwV3dNegate( pDirectionalAt, &vtxDirectionalPos );
		RwV3dNormalize( pDirectionalAt, pDirectionalAt );
		GetMatrixRactangular( m_pmtxDirectional, pDirectionalAt );
	}
}

bool AgcmCustomizeRender::CreateCamera()
{
	m_pCamera = RwCameraCreate();
	if( m_pCamera == NULL )
		return false;

	RwCameraSetFrame( m_pCamera, RwFrameCreate() );

	RwFrame *pCameraFrame;
	RwV3d vtxPos = {0.0f, 0.0f, 0.0f};
	RwV3d vtxUp = { 0, 1.f, 0 };

	pCameraFrame = RwCameraGetFrame( m_pCamera );
	if( pCameraFrame ) {
		RwFrameTranslate( pCameraFrame, &vtxPos, rwCOMBINEREPLACE );
		RwFrameRotate( pCameraFrame, &vtxUp, 180.0f, rwCOMBINEPRECONCAT );
	}

	RwCameraSetNearClipPlane( m_pCamera, 1.0f);
	RwCameraSetFarClipPlane( m_pCamera, 500.0f);

	RwV2d	stViewWindow;
	stViewWindow.x	= 0.5f;
	stViewWindow.y	= 0.5f*(float)m_nHeight/(float)m_nWidth;

	RwCameraSetViewWindow( m_pCamera, &stViewWindow );
	RpWorldAddCamera( m_pWorld, m_pCamera );

	m_fAngle = 0;

	return true;
}

bool AgcmCustomizeRender::CreateRenderTarget( RwBool bIsInit )
{
	m_pSceneRenderTargetTexture = RwRasterCreate( m_nWidth, m_nHeight,	32,	rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888 );
	if( m_pSceneRenderTargetTexture == NULL ) {
		ASSERT(0);
		return false;
	}

	//. 2006. 1. 11. Nonstopdj
	//. rwD3D9SetRasterFormat()에서 main camera format과 다를 경우 create에 실패하게 되므로.
	//. 현재비디오모드(백버퍼)에 지정된 포맷을 설정한다.
	//. 몇몇 ATI Radeon Series에서 나타남.
	//. m_pSceneRenderTarget = RwRasterCreate( m_nWidth, m_nHeight,	32,	rwRASTERTYPECAMERA | rwRASTERFORMAT8888 );
	//. RwVideoMode modeInfo;
	//. RwEngineGetVideoModeInfo(&modeInfo, RwEngineGetCurrentVideoMode());
	m_pSceneRenderTarget = RwRasterCreate( m_nWidth, m_nHeight,	32,	rwRASTERTYPECAMERA | RwD3D9GetBackBufferFomat() );
	if( m_pSceneRenderTarget == NULL ) {
		ASSERT(0);
		return false;
	}

	m_pZBuffer			 = RwRasterCreate( m_nWidth, m_nHeight,  0, rwRASTERTYPEZBUFFER );
	if( m_pZBuffer == NULL ) {
		ASSERT(0);
		return false;
	}

	RwCameraSetRaster( m_pCamera, m_pSceneRenderTarget );
	RwCameraSetZRaster( m_pCamera, m_pZBuffer );

	RwRGBA rgbaColor;
	rgbaColor.red = rgbaColor.green = rgbaColor.blue = rgbaColor.alpha = 0;

	if(bIsInit)
		RwCameraClear( m_pCamera, &rgbaColor, rwCAMERACLEARIMAGE | rwCAMERACLEARZ );

	if( m_pTexture == NULL ) {
		m_pTexture = RwTextureCreate( m_pSceneRenderTargetTexture );
	}
	else {
		RwTextureSetRaster( m_pTexture, m_pSceneRenderTargetTexture );
	}

	RwTextureSetAddressing( m_pTexture, rwTEXTUREADDRESSCLAMP );

	return true;
}

void AgcmCustomizeRender::DestoryRenderTarget( bool bLostDevice )
{
	if( m_pZBuffer ) {
		RwRasterDestroy( m_pZBuffer );
		m_pZBuffer = NULL;
	}

	if( bLostDevice == true ) 
	{
		if(m_pTexture != NULL)
		{
			RwTextureSetRaster( m_pTexture, NULL );

			if(m_pSceneRenderTargetTexture != NULL)
			{
				RwRasterDestroy( m_pSceneRenderTargetTexture );
				m_pSceneRenderTargetTexture = NULL;
			}
		}	
	}
	else if( m_pTexture ) {
		RwTextureDestroy( m_pTexture );
		m_pTexture = NULL;
		m_pSceneRenderTargetTexture = NULL;

		RwRasterDestroy( m_pSceneRenderTarget );
		m_pSceneRenderTarget = NULL;
	}

	RwCameraSetRaster( m_pCamera, NULL );
	RwCameraSetZRaster( m_pCamera, NULL );
}

void AgcmCustomizeRender::ClumpClone()
{
	AgcmCharacter*	pAgcmCharacter   = (AgcmCharacter *)GetModule( "AgcmCharacter" );
	AgcmLODManager* pAgcmLODManager  = ( AgcmLODManager *) GetModule( "AgcmLODManager" );
	m_pcsAgcmRender = ( AgcmRender*) GetModule( "AgcmRender" );

	AgpdCharacter* pstAgpdCharacter = pAgcmCharacter->GetCharacter( m_pstAgcdCharacter );

	if( m_pstAgcdCharacter == NULL )
		return;

	if( m_pstAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return;

	AgcdCharacterTemplate* pstAgcdCharacterTemplate = m_pstAgcdCharacter->m_pstAgcdCharacterTemplate;

	AgcdLODData* pstLODData = NULL;
	if( pAgcmLODManager )
		pstLODData = pAgcmLODManager->GetLODData( &pstAgcdCharacterTemplate->m_stLOD, 0, FALSE );

	m_pClump = RpClumpClone( m_pstAgcdCharacter->m_pClump );
	m_pClump->pvApBase = m_pstAgcdCharacter;

	RwFrame	*pstClumpFrame = RpClumpGetFrame( m_pClump );
	if( pstClumpFrame == NULL )
		return;

	RwSphere* bs = NULL;

	// 각종 UsrData를 설정한다.
	AcuObject::SetClumpType( m_pClump,
		ACUOBJECT_TYPE_CHARACTER | pstAgcdCharacterTemplate->m_lObjectType,
		pstAgpdCharacter->m_lID,
		pstAgpdCharacter,
		pstAgcdCharacterTemplate,
		((pstLODData) && (pstLODData->m_ulMaxLODLevel)) ? (pstLODData) : (NULL),
		m_pstAgcdCharacter->m_pPickAtomic,
		&pstAgcdCharacterTemplate->m_stBSphere,
		NULL,
		&pstAgcdCharacterTemplate->m_stOcTreeData );

	if( pstAgcdCharacterTemplate->m_csClumpRenderType.m_lSetCount > 0 )	{
		pstAgcdCharacterTemplate->m_csClumpRenderType.m_lCBCount = pstAgcdCharacterTemplate->m_csClumpRenderType.m_lSetCount;
		RpClumpForAllAtomics( m_pClump, AgcmRender::SetRenderTypeCB, (void *)(&pstAgcdCharacterTemplate->m_csClumpRenderType) );
	}

	RpClumpForAllAtomics( m_pClump, RemoveCustomizeAtomicCB, (void*)NULL );

	m_pInHierarchy = pAgcmCharacter->GetHierarchy( m_pClump );
	RpHAnimHierarchySetFlags(m_pInHierarchy,
		(RpHAnimHierarchyFlag)
		( RpHAnimHierarchyGetFlags(m_pInHierarchy) |
		rpHANIMHIERARCHYUPDATELTMS |
		rpHANIMHIERARCHYUPDATEMODELLINGMATRICES ) );

	RpHAnimHierarchyFlag flags = (RpHAnimHierarchyFlag) (m_pInHierarchy)->flags;
	RpHAnimHierarchyAttach(m_pInHierarchy);

	AgcdCharacterAnimation	*pcsCurCharacterAnimation = pstAgcdCharacterTemplate->m_pacsAnimationData[AGCMCHAR_ANIM_TYPE_WAIT][AGCMCHAR_AT2_COMMON_DEFAULT];
	if( pcsCurCharacterAnimation == NULL )
		return;

	if( pcsCurCharacterAnimation->m_pcsAnimation == NULL )
		return;

	if( pcsCurCharacterAnimation->m_pcsAnimation->m_pcsHead == NULL )
		return;

	if( pcsCurCharacterAnimation->m_pcsAnimation->m_pcsHead->m_pcsRtAnim == NULL )
		return;

	if( pcsCurCharacterAnimation->m_pcsAnimation->m_pcsHead->m_pcsRtAnim->m_pstAnimation == NULL )
		return;

	RtAnimAnimation	*pstCurAnim = pcsCurCharacterAnimation->m_pcsAnimation->m_pcsHead->m_pcsRtAnim->m_pstAnimation;
//	pstCurAnim = RpHAnimHierarchyGetCurrentAnim( m_pstAgcdCharacter->m_pInHierarchy );
	RpHAnimHierarchySetCurrentAnim( m_pInHierarchy, pstCurAnim );

	RpClumpForAllAtomics( m_pClump, AgcmCharacter::SetHierarchyForSkinAtomic, (void *)m_pInHierarchy );
	RpHAnimHierarchySetCurrentAnimTime( m_pInHierarchy, 0 );
	RpHAnimHierarchyUpdateMatrices( m_pInHierarchy );
}

void AgcmCustomizeRender::SetState( int nState )
{
	m_nState = nState;
}

int AgcmCustomizeRender::GetState() const
{
	return m_nState;
}

void AgcmCustomizeRender::SetWidth( int nWidth )
{
	m_nWidth = nWidth;
}

void AgcmCustomizeRender::SetHeight( int nHeight )
{
	m_nHeight = nHeight;
}

BOOL AgcmCustomizeRender::OnInit()
{
	return TRUE;
}

BOOL AgcmCustomizeRender::OnDestroy()
{
	if(m_pSceneRenderTargetTexture)
		DestoryRenderTarget();

	return TRUE;
}

BOOL AgcmCustomizeRender::OnAddModule()
{
	g_RwD3D9RasterExtOffset = RwRasterGetPluginOffset(rwID_DEVICEMODULE);

	OldD3D9ReleaseDeviceCB = _rwD3D9DeviceGetReleaseCallback();
	OldD3D9RestoreDeviceCB = _rwD3D9DeviceGetRestoreCallback();
	_rwD3D9DeviceSetReleaseCallback( NewD3D9ReleaseDeviceCB );
	_rwD3D9DeviceSetRestoreCallback( NewD3D9RestoreDeviceCB );

	//@{ Jaewon 20050630
	// ;)
	m_pcsAgcmResourceLoader = (AgcmResourceLoader*)GetModule("AgcmResourceLoader");
	//@} Jaewon

	m_pcsAgcmCharacter = (AgcmCharacter *)(GetModule("AgcmCharacter"));

	return TRUE;
}

void AgcmCustomizeRender::OnLostDevice()
{
	if(m_pSceneRenderTargetTexture) {
		DestoryRenderTarget( true );
		m_bDoNotCreate = false;
	}
	else {
		m_bDoNotCreate = true;
	}
}

void AgcmCustomizeRender::OnResetDevice()
{
	if(m_bDoNotCreate == false)
		CreateRenderTarget(false);
}

void AgcmCustomizeRender::Start( AgcdCharacter* pstAgcdCharacter )
{
	if( pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pcsDefaultHeadData == NULL )
		return;

	Init();
	m_pstAgcdCharacter = pstAgcdCharacter;

	m_vtxNearCamera = m_pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera;
	m_vtxFarCamera = m_pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera;

	if( fabsf( m_vtxFarCamera.z - m_vtxNearCamera.z ) < 0.0001f ) {
		m_vtxFarCamera.z = m_vtxNearCamera.z + 1.f;
	}

	m_fZoom = m_vtxFarCamera.z;

	m_fCameraPosY = 15.f;
	m_fCameraPosZ = 80.f;

	ClumpClone();
	CreateCamera();
	CreateRenderTarget();
	m_bStart = true;

	SetFace( 0 );
	SetHair( 0 );
}

BOOL AgcmCustomizeRender::SetWorld( RpWorld* pWorld )
{
	m_pWorld = pWorld;
	return TRUE;
}

void AgcmCustomizeRender::End()
{
	if (!m_bStart)
		return ;

	if( m_pClump ) {
		RpClumpDestroy( m_pClump );
	}

	DestoryRenderTarget();

	RpWorldRemoveCamera( m_pWorld, m_pCamera );
	RwCameraDestroy( m_pCamera );
	m_pstAgcdCharacter = NULL;

	m_bStart = false;
}

void AgcmCustomizeRender::DrawQuad()
{
	LPDIRECT3DDEVICE9 pD3DDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());

	rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER( RwRasterGetParent(m_pSceneRenderTargetTexture) );

	float fWidth5  = m_nWidth - 0.5f;
	float fHeight5 = m_nHeight - 0.5f;

	float fLeftU, fTopV, fRightU, fBottomV;
	fLeftU = fTopV = 0.f;
	fRightU  = fBottomV = 1.f;

	// Draw the quad
	SCREEN_VERTEX svQuad[4];

	svQuad[0].pos = D3DXVECTOR4( -0.5f, -0.5f, 0.5f, 1.0f );
	svQuad[0].tex = D3DXVECTOR2( fLeftU, fTopV );

	svQuad[1].pos = D3DXVECTOR4( fWidth5, -0.5f, 0.5f, 1.0f );
	svQuad[1].tex = D3DXVECTOR2( fRightU, fTopV );

	svQuad[2].pos = D3DXVECTOR4( -0.5f, fHeight5, 0.5f, 1.0f );
	svQuad[2].tex = D3DXVECTOR2( fLeftU, fBottomV );

	svQuad[3].pos = D3DXVECTOR4( fWidth5, fHeight5, 0.5f, 1.0f );
	svQuad[3].tex = D3DXVECTOR2( fRightU, fBottomV );

	pD3DDevice->SetTexture( 0, rasExt->texture );

	pD3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pD3DDevice->SetFVF( SCREEN_VERTEX::FVF );
	pD3DDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(SCREEN_VERTEX) );
	pD3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}

void AgcmCustomizeRender::Render()
{
	if( m_pSceneRenderTargetTexture == NULL )
		return;

	if( m_pstAgcdCharacter == NULL ) {
		return;
	}

	if( m_pstAgcdCharacter->m_eCurAnimType != AGCMCHAR_ANIM_TYPE_WAIT ) {
		return;
	}

	//. 2006. 1. 11. Nonstopdj
	//. ATI Radeon series에서 m_pCamera->framebuffer(raster)가 NULL인 경우 RwCameraClear()에서 
	//. crash가 일어난다. m_pCamera가 NULL일 경우 현재의 tick frame을 일단 return 시켜 다음 tick으로
	//. delay시켜본다.
	if(m_pCamera->frameBuffer == NULL )
		return;

	RwCamera* pCameraBackup = RwCameraGetCurrentCamera();
	// backup
	if( pCameraBackup ) {
		RwCameraEndUpdate( pCameraBackup );
	}

/*	RwCamera* pMainCam = GetCamera();
	ASSERT(pMainCam);

	RwRaster* pRaster = RwCameraGetRaster(pMainCam);
	m_nScreenWidth = RwRasterGetWidth(pRaster);
	m_nScreenHeight = RwRasterGetHeight(pRaster);
*/
	static clock_t prev = clock();
	clock_t now = clock();
	float dt = float(now - prev)/CLOCKS_PER_SEC;
	prev = now;

	dt *= 120.f;
	switch( m_nCamreaState )
	{
	case CS_LEFT_TURN:
		{
			m_fAngle += dt;
			if( m_fAngle >= 360.f ) {
				m_fAngle -= 360.f;
			}
		}
		break;
	case CS_RIGHT_TURN:
		{
			m_fAngle -= dt;
			if( m_fAngle < 0 ) {
				m_fAngle = 360.f-m_fAngle;
			}
		}
		break;
	case CS_ZOOM_IN:
		{
			m_fZoom -= dt;
			if( m_fZoom < m_vtxNearCamera.z ) {
				m_fZoom = m_vtxNearCamera.z;
			}
		}
		break;
	case CS_ZOOM_OUT:
		{
			m_fZoom += dt;
			if( m_fZoom > m_vtxFarCamera.z ) {
				m_fZoom = m_vtxFarCamera.z;
			}
		}
		break;
	case CS_AT_UP:
		{
			m_fTargetPosY += dt;
			if( m_fTargetPosY > 30.f ) {
				m_fTargetPosY = 30.f;
			}
		}
		break;
	case CS_AT_DOWN:
		{
			m_fTargetPosY -= dt;
			if( m_fTargetPosY < -30.f ) {
				m_fTargetPosY = -30.f;
			}
		}
		break;
	};
	m_fTargetPosY = 0.f;

// start
	RpLight* pAmbientLight = GetAmbientLight();
	RwRGBAReal colBackupAmbientLightColor;
	RwRGBAReal colAmbientLightColor;
	if( pAmbientLight ) {
		const RwRGBAReal* pBackupAmbientLightRGBA = RpLightGetColor( pAmbientLight );
		colBackupAmbientLightColor.red		= pBackupAmbientLightRGBA->red;
		colBackupAmbientLightColor.green	= pBackupAmbientLightRGBA->green;
		colBackupAmbientLightColor.blue		= pBackupAmbientLightRGBA->blue;
		colBackupAmbientLightColor.alpha	= pBackupAmbientLightRGBA->alpha;
		colAmbientLightColor.red	= colAmbientLightColor.green	= colAmbientLightColor.blue	= 0.22f;
		colAmbientLightColor.alpha	= 1.0f;

		if( g_colAmbientLightColor.red > 0.00001 ) {
			colAmbientLightColor = g_colAmbientLightColor;
		}
		RpLightSetColor( pAmbientLight, &colAmbientLightColor );
	}
	RpLight* pDirectionalLight = GetDirectionalLight();
	RwRGBAReal colBackupDirectionalLightColor;
	RwRGBAReal colDirectionalLightColor;
	RwMatrix   mtxDirectionalBackup;
	if( pDirectionalLight ) {
		const RwRGBAReal* pBackuppDirectionalLightRGBA = RpLightGetColor( pDirectionalLight );
		colBackupDirectionalLightColor.red		= pBackuppDirectionalLightRGBA->red;
		colBackupDirectionalLightColor.green	= pBackuppDirectionalLightRGBA->green;
		colBackupDirectionalLightColor.blue		= pBackuppDirectionalLightRGBA->blue;
		colBackupDirectionalLightColor.alpha	= pBackuppDirectionalLightRGBA->alpha;
		colDirectionalLightColor.red	= colDirectionalLightColor.green = colDirectionalLightColor.blue = 0.7f; 
		colDirectionalLightColor.alpha	= 1.0f;

		if( g_colDirectionalLightColor.red > 0.00001 ) {
			colDirectionalLightColor = g_colDirectionalLightColor;
		}

		RpLightSetColor( pDirectionalLight, &colDirectionalLightColor );

		m_pmtxDirectional = RwFrameGetLTM(RpLightGetFrame( pDirectionalLight ) );
		RwMatrixCopy( &mtxDirectionalBackup, m_pmtxDirectional );
	}

	RwRGBA rgbaColor;
	rgbaColor.red	= 0;
	rgbaColor.green = 0;
	rgbaColor.blue	= 0;

	if( m_nState == STATE_PREVIEW ) {
		rgbaColor.alpha = 0;
	}
	else {
		rgbaColor.alpha = 0xFF;
	}
	RwCameraClear( m_pCamera, &rgbaColor, rwCAMERACLEARIMAGE | rwCAMERACLEARZ );

	SetCameraPos();

	RpHAnimHierarchySetCurrentAnimTime( m_pInHierarchy, 0 );

	LockFrame();
	RpHAnimHierarchyUpdateMatrices( m_pInHierarchy );
	UnlockFrame();

	LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
	RwCameraBeginUpdate( m_pCamera );

	// Render World
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA );
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA );

	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE, (void *)TRUE );

	RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void*)true );
	RwRenderStateSet( rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)220 );
	RwRenderStateSet( rwRENDERSTATEALPHATESTFUNCTION, (void*)rwSTENCILFUNCTIONGREATEREQUAL );

	RwBool bFogEnable;
	RwRenderStateGet(rwRENDERSTATEFOGENABLE, &bFogEnable);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);

	RpClumpForAllAtomics( m_pClump, RenderAtomicCB, (void*)NULL );

	RwRenderStateSet(rwRENDERSTATEFOGENABLE, &bFogEnable);

	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0 );

	RwCameraEndUpdate( m_pCamera );

	// StretchRect
	RECT rtRect;
	rtRect.left = rtRect.top = 0;
	rtRect.right = m_nWidth - 1;
	rtRect.bottom = m_nHeight -1;

	rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTargetTexture));
	LPDIRECT3DSURFACE9 pSourceSurface;
	HRESULT hr = rasExt->texture->GetSurfaceLevel(0, &pSourceSurface);
	ASSERT(hr==D3D_OK);
	hr = pd3dDevice->StretchRect( _RwD3D9RenderSurface, &rtRect, pSourceSurface, &rtRect, D3DTEXF_NONE);
	ASSERT(hr==D3D_OK);
	pSourceSurface->Release();

	if( pCameraBackup ) {
		RwCameraBeginUpdate( pCameraBackup );
	}
	if( pAmbientLight ) {
		RpLightSetColor( pAmbientLight, &colBackupAmbientLightColor );
	}
	if( pDirectionalLight ) {
		RpLightSetColor( pDirectionalLight, &colBackupDirectionalLightColor );
		RwMatrixCopy( m_pmtxDirectional, &mtxDirectionalBackup );
	}
}

void AgcmCustomizeRender::SetCameraPosY( float fY )
{
	m_fCameraPosY = fY;
}

void AgcmCustomizeRender::SetCameraPosZ( float fZ )
{
	m_fCameraPosZ = fZ;
}

void AgcmCustomizeRender::SetTargetPosY( float fY )
{
	m_fTargetPosY = fY;
}

void AgcmCustomizeRender::SetPreviewFar()
{
	RwFrame *pCameraFrame= RwCameraGetFrame( m_pCamera );
	if( pCameraFrame == NULL ) {
		return;
	}
	RwV3d *pCameraPos	= RwMatrixGetPos( RwFrameGetLTM( pCameraFrame ) );

	m_vtxFarCamera = *pCameraPos;
	m_pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxFarCamera = m_vtxFarCamera;
}

void AgcmCustomizeRender::SetPreviewNear()
{
	RwFrame *pCameraFrame= RwCameraGetFrame( m_pCamera );
	if( pCameraFrame == NULL ) {
		return;
	}
	RwV3d *pCameraPos	= RwMatrixGetPos( RwFrameGetLTM( pCameraFrame ) );

	m_vtxNearCamera = *pCameraPos;
	m_pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_vtxNearCamera = m_vtxNearCamera;
}

void AgcmCustomizeRender::SetZoom( float fZ )
{
	m_fZoom = fZ;
}

void AgcmCustomizeRender::SetFace( INT32 nFaceID )
{
	if( m_pstAgcdCharacter == NULL || m_pstAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return;
	if( nFaceID >= (int)m_pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_vpFace.size() )
		return;

	DetachFace();
	m_pFace = NULL;

	m_nAttachFaceID = nFaceID;

	AttachFace();

	FILE* pf = fopen( "color.txt", "rt" );
	if( pf ) {
		fscanf( pf, "%f %f %f %f\n", &g_colAmbientLightColor.red,	 &g_colAmbientLightColor.blue,	  &g_colAmbientLightColor.green,		&g_colAmbientLightColor.alpha );
		fscanf( pf, "%f %f %f %f\n", &g_colDirectionalLightColor.red, &g_colDirectionalLightColor.blue, &g_colDirectionalLightColor.green, &g_colAmbientLightColor.alpha );
		fclose( pf );
	}
}

void AgcmCustomizeRender::SetHair( INT32 nHairID )
{
	if( m_pstAgcdCharacter == NULL || m_pstAgcdCharacter->m_pstAgcdCharacterTemplate == NULL )
		return;
	if( nHairID >= (int)m_pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_vpHair.size() )
		return;

	DetachHair();

	m_pHair = NULL;

	m_nAttachHairID = nHairID;

	AttachHair();
}

void AgcmCustomizeRender::AttachFace()
{
	PROFILE("AgcmCustomizeRender::AttachFace");

	//@{ Jaewon 20050630
	// Moved here from SetFace().
	m_pFace = RpAtomicClone( m_pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_vpFace[ m_nAttachFaceID ] );
	//@} Jaewon

	if( m_pFace == NULL )
		return;

	LockFrame();

	RpAtomic* pstAtomic			= m_pFace;
	RpClump*  pstCharacterClump	= m_pClump;
	RpHAnimHierarchy* pstCharacterHierarchy	= m_pInHierarchy;

	RpGeometry*	pstGeom			= RpAtomicGetGeometry(pstAtomic);
	RwFrame*	pstFrame		= RpClumpGetFrame(pstCharacterClump);
	RwFrame*	pstAtomicFrame	= RwFrameCreate();

	AcuObject::SetAtomicType( pstAtomic, ACUOBJECT_TYPE_RENDER_UDA, 0 );

	RpAtomicSetFrame( pstAtomic, pstAtomicFrame);
	RwFrameAddChild ( pstFrame, pstAtomicFrame);
	RpClumpAddAtomic( pstCharacterClump, pstAtomic);

	RpSkinAtomicSetHAnimHierarchy( pstAtomic, pstCharacterHierarchy );

	//. 2006. 3. 16. nonstopdj
	m_pcsAgcmCharacter->GeneralizeAttachedAtomicsNormals(m_pstAgcdCharacter);

	return;
}

void AgcmCustomizeRender::DetachFace()
{
	PROFILE("AgcmCustomizeRender::DetachFace");

	if( m_pFace == NULL )
		return;

	RpAtomic* pstAtomic			= m_pFace;
	RpClump*  pstCharacterClump	= m_pClump;

	RpGeometry*	pstGeom			= RpAtomicGetGeometry(pstAtomic);
	RwFrame*	pstFrame		= RpClumpGetFrame(pstCharacterClump);

	RpClumpRemoveAtomic( pstCharacterClump, pstAtomic );

	pstFrame = RpAtomicGetFrame( pstAtomic );
	RpAtomicSetFrame( pstAtomic, NULL );

	//@{ Jaewon 20050630
	// A leak!, so fix it.
	if(m_pcsAgcmResourceLoader)
		m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAtomic);
	else
		RpAtomicDestroy(pstAtomic);
	//@} Jaewon

	//@{ 2006/05/03 burumal
	//if( pstFrame != RpClumpGetFrame(pstCharacterClump) )
		//RwFrameDestroy(pstFrame);
	if ( pstFrame && pstFrame != RpClumpGetFrame(pstCharacterClump) )
	{
		RwFrame* pParentFrame = RwFrameGetParent(pstFrame);
		if ( pParentFrame && pParentFrame->child )
			RwFrameRemoveChild(pstFrame);
		RwFrameDestroy(pstFrame);
	}
	//@}

	return;
}

void AgcmCustomizeRender::AttachHair()
{
	PROFILE("AgcmCustomizeRender::AttachHair");

	//@{ Jaewon 20050630
	// Moved here from SetHair().
	m_pHair = RpAtomicClone( m_pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_vpHair[ m_nAttachHairID ] );
	//@} Jaewon

	if( m_pHair == NULL )
		return;

	RpAtomic* pstAtomic						= m_pHair;
	RpClump*  pstCharacterClump				= m_pClump;
	RpHAnimHierarchy* pstCharacterHierarchy	= m_pInHierarchy;

	AcuObject::SetAtomicType( pstAtomic, ACUOBJECT_TYPE_RENDER_UDA, 0 );

	RpGeometry*	pstGeom			= RpAtomicGetGeometry(pstAtomic);
	RwFrame*	pstFrame		= RpClumpGetFrame(pstCharacterClump);
	RwFrame*	pstAtomicFrame	= RwFrameCreate();

	RpAtomicSetFrame( pstAtomic, pstAtomicFrame);
	RwFrameAddChild ( pstFrame, pstAtomicFrame);
	RpClumpAddAtomic( pstCharacterClump, pstAtomic);
	RpSkinAtomicSetHAnimHierarchy( pstAtomic, pstCharacterHierarchy );

	//. 2006. 3. 16. nonstopdj
	m_pcsAgcmCharacter->GeneralizeAttachedAtomicsNormals(m_pstAgcdCharacter);

	return;
}

void AgcmCustomizeRender::DetachHair()
{
	PROFILE("AgcmCustomizeRender::DetachHair");

	if( m_pHair == NULL )
		return;

	RpAtomic*	pstAtomic			= m_pHair;
	RpClump*	pstCharacterClump	= m_pClump;

	RpGeometry*	pstGeom				= RpAtomicGetGeometry( pstAtomic );
	RwFrame*	pstFrame			= RpClumpGetFrame( pstCharacterClump );
	RwFrame*	pstAtomicFrame		= RwFrameCreate();

	RpClumpRemoveAtomic( pstCharacterClump, pstAtomic );

	pstFrame = RpAtomicGetFrame( pstAtomic );
	RpAtomicSetFrame( pstAtomic, NULL );

	//@{ Jaewon 20050630
	// A leak!, so fix it.
	if(m_pcsAgcmResourceLoader)
		m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAtomic);
	else
		RpAtomicDestroy(pstAtomic);
	//@} Jaewon

	//@{ 2006/05/03 burumal
	//if( pstFrame != RpClumpGetFrame(pstCharacterClump) )
		//RwFrameDestroy(pstFrame);
	if ( pstFrame && pstFrame != RpClumpGetFrame(pstCharacterClump) )
	{
		RwFrame* pParentFrame = RwFrameGetParent(pstFrame);
		if ( pParentFrame && pParentFrame->child )
			RwFrameRemoveChild(pstFrame);
		RwFrameDestroy(pstFrame);
	}
	//@}

	return;
}

void AgcmCustomizeRender::LeftTurnOn()
{
	m_nCamreaState = CS_LEFT_TURN;
}

void AgcmCustomizeRender::RightTurnOn()
{
	m_nCamreaState = CS_RIGHT_TURN;
}

void AgcmCustomizeRender::LeftTurnOff()
{
	if( m_nCamreaState == CS_LEFT_TURN ) {
		m_nCamreaState = CS_NONE;
	}
}

void AgcmCustomizeRender::RightTurnOff()
{
	if( m_nCamreaState == CS_RIGHT_TURN ) {
		m_nCamreaState = CS_NONE;
	}
}

bool AgcmCustomizeRender::IsLeftTurnOn() const
{
	return (m_nCamreaState == CS_LEFT_TURN);
}

bool AgcmCustomizeRender::IsRightTurnOn() const
{
	return (m_nCamreaState == CS_RIGHT_TURN);
}

void AgcmCustomizeRender::ZoomInOn()
{
	m_nCamreaState = CS_ZOOM_IN;
}

void AgcmCustomizeRender::ZoomOutOn()
{
	m_nCamreaState = CS_ZOOM_OUT;
}

void AgcmCustomizeRender::ZoomInOff()
{
	if( m_nCamreaState == CS_ZOOM_IN ) {
		m_nCamreaState = CS_NONE;
	}
}

void AgcmCustomizeRender::ZoomOutOff()
{
	if( m_nCamreaState == CS_ZOOM_OUT ) {
		m_nCamreaState = CS_NONE;
	}
}

bool AgcmCustomizeRender::IsZoomInOn()  const
{
	return ( m_nCamreaState == CS_ZOOM_IN );
}

bool AgcmCustomizeRender::IsZoomOutOn() const
{
	return ( m_nCamreaState == CS_ZOOM_OUT );
}

void AgcmCustomizeRender::AtUpOn()
{
	m_nCamreaState = CS_AT_UP;
}

void AgcmCustomizeRender::AtDownOn()
{
	m_nCamreaState = CS_AT_DOWN;
}

void AgcmCustomizeRender::AtUpOff()
{
	if( m_nCamreaState == CS_AT_UP ) {
		m_nCamreaState = CS_NONE;
	}
}

void AgcmCustomizeRender::AtDownOff()
{
	if( m_nCamreaState == CS_AT_DOWN ) {
		m_nCamreaState = CS_NONE;
	}
}

bool AgcmCustomizeRender::IsAtUpOn()   const
{
	return (m_nCamreaState == CS_AT_UP);
}

bool AgcmCustomizeRender::IsAtDownOn() const
{
	return (m_nCamreaState == CS_AT_DOWN);
}

int	AgcmCustomizeRender::GetHairNum( AgcdCharacter* pstAgcdCharacter )
{
	return (int)pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_vpHair.size();
}

int AgcmCustomizeRender::GetFaceNum( AgcdCharacter* pstAgcdCharacter )
{
	return (int)pstAgcdCharacter->m_pstAgcdCharacterTemplate->m_vpFace.size();
}

RwTexture* AgcmCustomizeRender::GetTexture()
{
	return m_pTexture;
}
