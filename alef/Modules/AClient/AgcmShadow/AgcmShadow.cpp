#include "AgcmShadow.h"
#include "AcuRpMTexture.h"
#include "AgcmMap.h"
#include "AgcmRender.h"
#include "AgcmCharacter.h"
#include "ApMemoryTracker.h"
#include "AcuRpMatD3DFx.h"

/*
 *  The anti-alias and blur effects are sensitive to alignment between
 *  pixels and texels. This is platform dependent.
 */
#if ( defined(OPENGL_DRVMODEL_H) | defined( SOFTRAS_DRVMODEL_H ) )
#define TEXELOFFSET 0.0f
#else
#define TEXELOFFSET 0.5f
#endif

#define		SHADOW_SPHERE_EXPAND		1.5f

AgcmShadow::AgcmShadow()
{
	SetModuleName("AgcmShadow");

	EnableIdle(TRUE);

	m_pcmMap			= NULL;
	m_pcRender			= NULL;
	m_pAgcmCharacter	= NULL;

	m_fShadowStrength	= 0.9f;
	m_pcCircleShadow	= NULL;		// 원 그림

	ZeroMemory( m_fLODOriginalShadowDistance, sizeof(FLOAT) * 3 * 4 );		// 행 level 열 lod level(3- none)
	m_fShadowDistFactor	= 1.0f;

	m_ulCurTick			= 0;
	m_ulLastTick		= 0;
	m_ulCurTickDiff		= 0;

	m_pGLightAmbient	= NULL;
	m_pGLightDirect		= NULL;
	m_pWorldCamera		= NULL;
	m_pWorld			= NULL;

	m_pcShadowCamera	= NULL;
	m_pcShadowIPCamera	= NULL;
	
	m_bDrawShadow		= TRUE;
	
	m_listShadowInfo.clear();

	m_pMainCharacterTexUpdate	= NULL;
	ZeroMemory( m_listTexUpdate1, sizeof(ShadowInfo*) * SHADOW_T1_UPDATEMAX );

	ZeroMemory( m_vTriFan, sizeof(RwIm2DVertex) * 4 );
	
	m_pcShadowRasterMain512	= NULL;
	m_pcShadowRasterAA256	= NULL;
	m_pcShadowRasterBlur256	= NULL;

	m_iShadowDrawMaxNum	= 5;

	m_colorBackGround.red	= 255;
	m_colorBackGround.green	= 255;
	m_colorBackGround.blue	= 255;
	m_colorBackGround.alpha	= 0;

	m_bDrawShadow		= FALSE;
}

AgcmShadow::~AgcmShadow()
{
	m_listShadowInfo.clear();
}

BOOL AgcmShadow::OnAddModule()
{
	m_pcRender			= (AgcmRender*)GetModule("AgcmRender");
	m_pcmMap			= (AgcmMap*)GetModule("AgcmMap");

	m_pcRender->SetCallbackPostRender( CB_POST_RENDER, this );

	m_pWorld			= GetWorld();
	m_pWorldCamera		= GetCamera();		
	m_pGLightDirect		= GetDirectionalLight();	
	m_pGLightAmbient	= GetAmbientLight();	

	OnCameraStateChange( CSC_INIT );

	return TRUE;
}

BOOL AgcmShadow::OnInit()
{
	m_pAgcmCharacter = (AgcmCharacter*)GetModule("AgcmCharacter");

	m_fLODOriginalShadowDistance[0][0] = 5000.0f;
	m_fLODOriginalShadowDistance[0][1] = 0.0f;
	m_fLODOriginalShadowDistance[0][2] = 0.0f;
	m_fLODOriginalShadowDistance[0][3] = 0.0f;

	m_fLODOriginalShadowDistance[1][0] = 0.0f;
	m_fLODOriginalShadowDistance[1][1] = 2000.0f;
	m_fLODOriginalShadowDistance[1][2] = 4000.0f;
	m_fLODOriginalShadowDistance[1][3] = 5000.0f;

	m_fLODOriginalShadowDistance[2][0] = 0.0f;
	m_fLODOriginalShadowDistance[2][1] = 1000.0f;
	m_fLODOriginalShadowDistance[2][2] = 2500.0f;
	m_fLODOriginalShadowDistance[2][3] = 4000.0f;

	RwIm2DVertexSetIntRGBA(&m_vTriFan[0], 255, 255, 255, 255);
    RwIm2DVertexSetIntRGBA(&m_vTriFan[1], 255, 255, 255, 255);
    RwIm2DVertexSetIntRGBA(&m_vTriFan[2], 255, 255, 255, 255);
    RwIm2DVertexSetIntRGBA(&m_vTriFan[3], 255, 255, 255, 255);

	return TRUE;
}

BOOL AgcmShadow::OnDestroy()
{
	LockFrame();

	if( m_pcShadowCamera )
    {
        RwFrame* frame = RwCameraGetFrame(m_pcShadowCamera);
        if ( frame )
        {
			RwCameraSetFrame(m_pcShadowCamera, NULL);
            RwFrameDestroy(frame);
        }
        
        RwCameraDestroy( m_pcShadowCamera );
		m_pcShadowCamera = NULL;
    }

	if( m_pcShadowIPCamera )
    {
        RwFrame* frame = RwCameraGetFrame(m_pcShadowIPCamera);
        if (frame)
        {
            RwCameraSetFrame(m_pcShadowIPCamera, NULL);
            RwFrameDestroy(frame);
        }
        
        RwCameraDestroy( m_pcShadowIPCamera );
		m_pcShadowIPCamera = NULL;
    }
	
	if( m_pcShadowRasterMain512 )
	{
		RwRasterDestroy(m_pcShadowRasterMain512);
		m_pcShadowRasterMain512 = NULL;
	}

	if( m_pcCircleShadow )
	{
		RwTextureDestroy(m_pcCircleShadow);
		m_pcCircleShadow = NULL;
	}

	UnlockFrame();

	return TRUE;
}

BOOL AgcmShadow::OnIdle(UINT32 ulClockCount)
{
	m_ulCurTickDiff	= ulClockCount - m_ulCurTick;
	m_ulLastTick	= m_ulCurTick;
	m_ulCurTick		= ulClockCount;

	return TRUE;
}

//void AgcmShadow::Render()
//{
//	PROFILE("AgcmShadow::Render");
//
//	RwBool bFogEnable;
//	RwRenderStateGet(rwRENDERSTATEFOGENABLE, &bFogEnable);
//	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)FALSE);
//
//	RwCameraEndUpdate( m_pWorldCamera );
//
//	// 주인공 shadow draw
//	RwFrame            *camFrame;
//	RwMatrix           *camMatrix;
//	RwFrame            *lightFrame = RpLightGetFrame( m_pGLightDirect );
//
//	// 
//	//  Disable lights 
//	//
//	UINT32 dFlags = RpLightGetFlags(m_pGLightDirect);
//	RpLightSetFlags(m_pGLightDirect, 0);
//
//	UINT32 aFlags = RpLightGetFlags(m_pGLightAmbient);
//	RpLightSetFlags(m_pGLightAmbient, 0);
//
//	RwRGBA bgColor = { 255, 255, 255, 0 };
//	
//	//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//	// main character shadow update
//	ShadowInfo*	pShadowInfo = m_pMainCharacterTexUpdate;;
//	RpClump* pRideClump = m_pAgcmCharacter->GetSelfRideClump();
//	
//	if(pShadowInfo)
//	{
//		LockFrame();
//
//		RwCameraSetFarClipPlane( m_pcShadowCamera, pShadowInfo->cam_far);
//		RwCameraSetNearClipPlane( m_pcShadowCamera, pShadowInfo->cam_near);
//		RwCameraSetViewWindow( m_pcShadowCamera, &pShadowInfo->view_size);
//			
//		camFrame = RwCameraGetFrame( m_pcShadowCamera);
//
//		UnlockFrame();
//
//		camMatrix = RwFrameGetMatrix(camFrame);
//		RwMatrixCopy(camMatrix ,&pShadowInfo->camMatirx); 
//
//		//////////////////////////////////////////////////Render Texture!
//		RwCameraSetRaster(m_pcShadowCamera, m_pcShadowRasterMain512);
//
//		//
//		//  Clear to white background with alpha = 0.
//		//	
//		RwCameraClear(m_pcShadowCamera, &bgColor, rwCAMERACLEARIMAGE);
//		
//		// 
//		//  Render the clump
//		//
//		LockFrame();
//		if ( RwCameraBeginUpdate( m_pcShadowCamera ) )
//		{
//			UnlockFrame();
//			RpAtomic*	cur_atomic = pShadowInfo->clump->atomicList;
//			do
//			{
//				bool bDisable = false;
//				if(RpAtomicFxIsEnabled(cur_atomic))
//				{
//					RpAtomicFxDisable(cur_atomic);
//					bDisable = true;
//				}
//				
//				m_pcRender->OriginalAtomicRender(cur_atomic);
//
//				if( bDisable )
//					RpAtomicFxEnable(cur_atomic);
//
//				cur_atomic = cur_atomic->next;
//			}while( cur_atomic != pShadowInfo->clump->atomicList );
//
//
//			if( pRideClump && pRideClump->stType.boundingSphere.radius < 2000.0f )		//. 경계구가 2000.0f미만일때만 Shadow on~
//			{
//				RpAtomic*	cur_atomic = pRideClump->atomicList;
//				do
//				{
//					bool bDisable = false;
//					if(RpAtomicFxIsEnabled(cur_atomic))
//					{
//						RpAtomicFxDisable(cur_atomic);
//						bDisable = true;
//					}
//
//					m_pcRender->OriginalAtomicRender(cur_atomic);
//
//					if(bDisable)
//						RpAtomicFxEnable(cur_atomic);
//
//					cur_atomic = cur_atomic->next;
//				}while(cur_atomic != pRideClump->atomicList);
//			}
//
//			RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *) rwFILTERLINEAR);
//			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) FALSE);
//
//			#if ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) )
//				InvertRaster(m_pcShadowRasterMain512);
//			#endif
//
//			RwCameraEndUpdate( m_pcShadowCamera );
//
//			#ifdef DOLPHIN
//				RwGameCubeCameraTextureFlush( RwCameraGetRaster(m_pcShadowCamera), FALSE );
//			#endif
//		}
//		else
//			UnlockFrame();
//
//		//ShadowRasterResample(m_pcShadowRasterAA256, m_pcShadowRasterMain512);		//  Anti-alias and set the raster to be used for shadow rendering. 
//		//ShadowRasterBlur(m_pcShadowRasterAA256, 1);								//  Blur the 
//		
//		// main character shadow draw
//		LockFrame();
//
//		if( RwCameraBeginUpdate(m_pWorldCamera) )
//		{
//			UnlockFrame();
//			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);
//			ShadowDraw( pShadowInfo, m_pcShadowRasterMain512 );
//
//			RwCameraEndUpdate(m_pWorldCamera);
//		}
//		else 
//			UnlockFrame();
//	}
//	
//	for(int i = 0;i<m_iTexUpdate1;++i)
//	{
//		pShadowInfo = m_listTexUpdate1[i];
//
//		if( !pShadowInfo )						continue;
//		if( pShadowInfo->clump == pRideClump )	continue;
//
//		if( pShadowInfo->level < ShadowInfo::eLevelCircle )
//		{
//			LockFrame();
//
//			RwCameraSetFarClipPlane(m_pcShadowCamera, pShadowInfo->cam_far);
//			RwCameraSetNearClipPlane(m_pcShadowCamera, pShadowInfo->cam_near);
//			RwCameraSetViewWindow(m_pcShadowCamera, &pShadowInfo->view_size);
//			
//			camFrame = RwCameraGetFrame(m_pcShadowCamera);
//
//			UnlockFrame();
//
//			camMatrix = RwFrameGetMatrix(camFrame);
//			RwMatrixCopy(camMatrix ,&pShadowInfo->camMatirx); 
//			
//			RwCameraSetRaster( m_pcShadowCamera, m_pcShadowRasterMain512 );		//Render Texture!
//			
//			//
//			//  Clear to white background with alpha = 0.
//			//	
//			RwCameraClear( m_pcShadowCamera, &bgColor, rwCAMERACLEARIMAGE );
//
//			// 
//			//  Render the clump
//			//
//			LockFrame();
//			if( RwCameraBeginUpdate( m_pcShadowCamera ) )
//			{
//				UnlockFrame();
//				RpAtomic* cur_atomic = pShadowInfo->clump->atomicList;
//				do
//				{
//					bool bDisable = false;
//					if(RpAtomicFxIsEnabled(cur_atomic))
//					{
//						RpAtomicFxDisable(cur_atomic);
//						bDisable = true;
//					}
//					
//					m_pcRender->OriginalAtomicRender(cur_atomic);
//
//					if( bDisable )
//						RpAtomicFxEnable( cur_atomic );
//
//					cur_atomic = cur_atomic->next;
//				}while( cur_atomic != pShadowInfo->clump->atomicList );
//
//				RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,(void *) rwFILTERLINEAR);
//				RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) FALSE);
//
//				#if ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) )
//					InvertRaster(m_pcShadowRasterMain512);
//				#endif
//
//				RwCameraEndUpdate( m_pcShadowCamera );
//
//				#ifdef DOLPHIN
//					RwGameCubeCameraTextureFlush( RwCameraGetRaster(m_pcShadowCamera), FALSE);
//				#endif
//			}
//			else 
//				UnlockFrame();
//		}
//
//		// character shadow draw
//		LockFrame();
//		if(RwCameraBeginUpdate(m_pWorldCamera))
//		{
//			UnlockFrame();
//			RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);
//			ShadowDraw(pShadowInfo,m_pcShadowRasterMain512);
//
//			RwCameraEndUpdate(m_pWorldCamera);
//		}
//		else 
//			UnlockFrame();
//	}
//
//	LockFrame();
//	RwCameraBeginUpdate(m_pWorldCamera);
//	RwRenderStateSet( rwRENDERSTATEFOGENABLE, (void*)bFogEnable );
//	RwD3D9SetRenderState( D3DRS_FOGEND, *((DWORD*)(&m_pcRender->m_fFogEnd))); 
//	UnlockFrame();
//
//	RpLightSetFlags(m_pGLightDirect, dFlags);
//	RpLightSetFlags(m_pGLightAmbient, aFlags);
//}

void AgcmShadow::Render()
{
	PROFILE("AgcmShadow::Render");

	RwBool bFogEnable;
	RwRenderStateGet( rwRENDERSTATEFOGENABLE, &bFogEnable );
	RwRenderStateSet( rwRENDERSTATEFOGENABLE, (void*)FALSE );

	UINT32 dFlags = RpLightGetFlags( m_pGLightDirect );
	RpLightSetFlags( m_pGLightDirect, 0 );

	UINT32 aFlags = RpLightGetFlags( m_pGLightAmbient );
	RpLightSetFlags( m_pGLightAmbient, 0 );

	RwCameraEndUpdate( m_pWorldCamera );

	RwFrame* camFrame;
	for( ShadowInfoListItr Itr = m_listShadowInfo.begin(); Itr != m_listShadowInfo.end(); ++Itr )
	{
		ShadowInfo* pShadowInfo = (*Itr);

		RwCameraSetRaster( m_pcShadowCamera, m_pcShadowRasterMain512 );		//Render Texture!
		RwCameraClear( m_pcShadowCamera, &m_colorBackGround, rwCAMERACLEARIMAGE );

		LockFrame();

		RwCameraSetFarClipPlane( m_pcShadowCamera, pShadowInfo->cam_far);
		RwCameraSetNearClipPlane( m_pcShadowCamera, pShadowInfo->cam_near);
		RwCameraSetViewWindow( m_pcShadowCamera, &pShadowInfo->view_size);
			
		camFrame = RwCameraGetFrame( m_pcShadowCamera );

		UnlockFrame();

		RwMatrix* camMatrix = RwFrameGetMatrix(camFrame);
		RwMatrixCopy( camMatrix, &pShadowInfo->camMatirx );

		LockFrame();
		if ( RwCameraBeginUpdate( m_pcShadowCamera ) )
		{
			UnlockFrame();
			RpAtomic*	cur_atomic = pShadowInfo->clump->atomicList;
			do
			{
				bool bDisable = false;
				if(RpAtomicFxIsEnabled(cur_atomic))
				{
					RpAtomicFxDisable(cur_atomic);
					bDisable = true;
				}
				
				m_pcRender->OriginalAtomicRender(cur_atomic);

				if( bDisable )
					RpAtomicFxEnable(cur_atomic);

				cur_atomic = cur_atomic->next;
			}while( cur_atomic != pShadowInfo->clump->atomicList );

			if( pShadowInfo->clump->stUserData.characterShadowLevel == 1 )	//주인공
			{
				RpClump* pRideClump = m_pAgcmCharacter->GetSelfRideClump();
				if( pRideClump && pRideClump->stType.boundingSphere.radius < 2000.0f )		//. 경계구가 2000.0f미만일때만 Shadow on~
				{
					RpAtomic*	cur_atomic = pRideClump->atomicList;
					do
					{
						bool bDisable = false;
						if(RpAtomicFxIsEnabled(cur_atomic))
						{
							RpAtomicFxDisable(cur_atomic);
							bDisable = true;
						}

						m_pcRender->OriginalAtomicRender(cur_atomic);

						if(bDisable)
							RpAtomicFxEnable(cur_atomic);

						cur_atomic = cur_atomic->next;
					}while(cur_atomic != pRideClump->atomicList);
				}
			}

			RwRenderStateSet( rwRENDERSTATETEXTUREFILTER,(void *) rwFILTERLINEAR );
			RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *) FALSE );

			#if ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) )
				InvertRaster( m_pcShadowRasterMain512 );
			#endif

			RwCameraEndUpdate( m_pcShadowCamera );

			#ifdef DOLPHIN
				RwGameCubeCameraTextureFlush( RwCameraGetRaster(m_pcShadowCamera), FALSE );
			#endif
		}
		else
			UnlockFrame();

		LockFrame();

		if( RwCameraBeginUpdate( m_pWorldCamera ) )
		{
			UnlockFrame();
			RwRenderStateSet( rwRENDERSTATEZTESTENABLE, (void *) TRUE );
			ShadowDraw( pShadowInfo, m_pcShadowRasterMain512 );
			RwCameraEndUpdate(m_pWorldCamera);
		}
		else 
			UnlockFrame();
	}

	LockFrame();
	RwCameraBeginUpdate( m_pWorldCamera );
	RwRenderStateSet( rwRENDERSTATEFOGENABLE, (void*)bFogEnable );
	RwD3D9SetRenderState( D3DRS_FOGEND, *((DWORD*)(&m_pcRender->m_fFogEnd))); 
	UnlockFrame();

	RpLightSetFlags( m_pGLightDirect, dFlags );
	RpLightSetFlags( m_pGLightAmbient, aFlags );

	m_listShadowInfo.clear();
}

BOOL AgcmShadow::SetShadowEnable( BOOL bVal )
{
	if( bVal && !m_bDrawShadow )			// shadow 관련 obj 생성
	{
		LockFrame();

		ASSERT(!m_pcShadowCamera);			// 기존에 있는데 또 생성?

		m_pcShadowCamera = RwCameraCreate();
		if ( !m_pcShadowCamera )	goto FAIL;

		RwCameraSetFrame( m_pcShadowCamera, RwFrameCreate() );
		if( !RwCameraGetFrame( m_pcShadowCamera ) )	goto FAIL;

		RwCameraSetProjection( m_pcShadowCamera, rwPARALLEL );
		RwCameraSetZRaster( m_pcShadowCamera , NULL );

		m_pcShadowIPCamera = RwCameraCreate();
		if( !m_pcShadowIPCamera )	goto FAIL;

		RwCameraSetFrame(m_pcShadowIPCamera, RwFrameCreate());
		if( !RwCameraGetFrame( m_pcShadowIPCamera ) )	goto FAIL;

		RwCameraSetProjection( m_pcShadowIPCamera, rwPARALLEL );
		RwCameraSetZRaster( m_pcShadowIPCamera , NULL );

		m_pcCircleShadow = RwTextureRead(RWSTRING("ShadowCircle.bmp"), NULL);
		if( !m_pcCircleShadow  )	goto FAIL;

		m_pcShadowRasterMain512 = RwRasterCreate(512, 512, 0, rwRASTERTYPECAMERATEXTURE);

		UnlockFrame();
		m_bDrawShadow = TRUE;
	}
	else if(!bVal && m_bDrawShadow)		// shadow 관련 obj 해제
	{
		ASSERT(m_pcShadowCamera);		// null??

		OnDestroy();
		m_bDrawShadow = FALSE;
	}

	return TRUE;

FAIL:
	UnlockFrame();
	OnDestroy();
	return FALSE;
}

BOOL	SetRwIm3DVertex( RwIm3DVertex* pIm3DVertex, RwV3d* pTri, ShadowInfo* pShadowInfo, UINT8 byAlpha )
{
	RwV3d	vInverseTri[3];
	RwV3dTransformPoints( vInverseTri, pTri, 3, &pShadowInfo->invMatrix );

	if( ( vInverseTri[0].z < 0.f && vInverseTri[1].z < 0.f && vInverseTri[2].z < 0.f ) ||
		( vInverseTri[0].x < 0.f && vInverseTri[1].x < 0.f && vInverseTri[2].x < 0.f ) ||
		( vInverseTri[0].x > 1.f && vInverseTri[1].x > 1.f && vInverseTri[2].x > 1.f ) ||
		( vInverseTri[0].y < 0.f && vInverseTri[1].y < 0.f && vInverseTri[2].y < 0.f ) ||
		( vInverseTri[0].y > 1.f && vInverseTri[1].y > 1.f && vInverseTri[2].y > 1.0f) )
	{
		return FALSE;
	}

	RwIm3DVertexSetPos( pIm3DVertex, pTri[0].x, pTri[0].y, pTri[0].z );
	RwIm3DVertexSetU( pIm3DVertex, vInverseTri[0].x );
	RwIm3DVertexSetV( pIm3DVertex, vInverseTri[0].y );

	RwIm3DVertexSetPos( pIm3DVertex + 1, pTri[1].x, pTri[1].y, pTri[1].z );
	RwIm3DVertexSetU( pIm3DVertex + 1, vInverseTri[1].x );
	RwIm3DVertexSetV( pIm3DVertex + 1, vInverseTri[1].y );

	RwIm3DVertexSetPos( pIm3DVertex + 2, pTri[2].x, pTri[2].y, pTri[2].z );
	RwIm3DVertexSetU( pIm3DVertex + 2, vInverseTri[2].x );
	RwIm3DVertexSetV( pIm3DVertex + 2, vInverseTri[2].y );

	if( pShadowInfo->fade )
	{
		FLOAT fadeVal = 1.0f - vInverseTri[0].z ;//* vShad[0].z;
		UINT8 val = fadeVal < 0.0f ? 0 : (UINT8) (fadeVal * byAlpha);
		RwIm3DVertexSetRGBA( pIm3DVertex, val, val, val, val );

		fadeVal = 1.0f - vInverseTri[1].z ;//* vShad[1].z;
		val = fadeVal < 0.f ? 0 : (UINT8)(fadeVal * byAlpha);
		RwIm3DVertexSetRGBA( pIm3DVertex + 1, val, val, val, val );

		fadeVal = 1.0f - vInverseTri[2].z ;//* vShad[2].z;
		val = fadeVal < 0.f ? 0 : (UINT8)(fadeVal * byAlpha);
		RwIm3DVertexSetRGBA( pIm3DVertex + 2, val, val, val, val );
	}
	else
	{
		RwIm3DVertexSetRGBA( pIm3DVertex, byAlpha, byAlpha, byAlpha, byAlpha );
		RwIm3DVertexSetRGBA( pIm3DVertex + 1, byAlpha, byAlpha, byAlpha, byAlpha );
		RwIm3DVertexSetRGBA( pIm3DVertex + 2, byAlpha, byAlpha, byAlpha, byAlpha );
	}

	return TRUE;
}	

void AgcmShadow::ShadowDraw(ShadowInfo*	pShadowInfo, RwRaster* pRas)
{
	PROFILE("AgcmShadow::ShadowRender");

	RwD3D9SetSamplerState( 0, D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP );
	RwD3D9SetSamplerState( 0, D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP );

    RwRenderStateSet( rwRENDERSTATETEXTUREFILTER,	(void*)rwFILTERLINEAR );
	RwRenderStateSet( rwRENDERSTATESRCBLEND,		(void *) rwBLENDZERO );				 
	RwRenderStateSet( rwRENDERSTATETEXTURERASTER,	ShadowInfo::eLevelCircle == pShadowInfo->level ? (void*)RwTextureGetRaster(m_pcCircleShadow) : (void*)pRas );

	#ifdef SKY
		RwRenderStateSet( rwRENDERSTATEDESTBLEND,	(void *) rwBLENDINVSRCALPHA );
	#else // SKY 
		RwRenderStateSet( rwRENDERSTATEDESTBLEND,	(void *) rwBLENDINVSRCCOLOR );
	#endif // SKY 

	FLOAT shadowStrength = m_fShadowStrength >= 0.f ? m_fShadowStrength : m_fShadowStrength * -1;
	UINT8 alpha = (UINT8) (shadowStrength * 255);  // 같은 알파값을 같는다(해의 고도등에 따른 m_fShadowStrength에 의해) 
	
	INT32 nMapSize = ( INT32 ) MAP_STEPSIZE;

	INT32 nXStart = (INT32)pShadowInfo->colboxinf.x / nMapSize -1;
	INT32 nZStart = (INT32)pShadowInfo->colboxinf.z / nMapSize -1;
	
	INT32 nXEnd = (INT32)pShadowInfo->colboxsup.x / nMapSize +1;
	INT32 nZEnd = (INT32)pShadowInfo->colboxsup.z / nMapSize +1;

	INT32 nXSize = nXEnd - nXStart;
	INT32 nZSize = nZEnd - nZStart;

	INT32 nXSize2 = nXSize + 1;

	int nHightSize = (nXSize + 1) * (nZSize + 1) * 4;
	float* pHeightList = (float*) AcuFrameMemory::AllocFrameMemory( nHightSize );
	if( !pHeightList )		return;

	for( int i = 0; i<= nZSize; ++i )
	{
		int nPos = i * nXSize2;
		for( int j = 0; j<= nXSize; ++j )
			pHeightList[ nPos + j ] = m_pcmMap->HP_GetHeightGeometryOnly( (float)((j+nXStart)*nMapSize), (float)((i+nZStart)*nMapSize) );
	}

	int nVertexSize = ( nXSize * nZSize * 6 ) * IM3DVERTEXDATASIZE;
	RwIm3DVertex* pIm3DVertexList = (RwIm3DVertex*)AcuFrameMemory::AllocFrameMemory(  nVertexSize );
	if( !pIm3DVertexList )		return;
	
	RwV3d	vIn[3];
	UINT32	nDrawCount = 0;
	for( int i = nZStart; i < nZEnd; ++i )
	{
		for( int j = nXStart; j < nXEnd; ++j )
		{
			RwIm3DVertex* pIm3DVertex = pIm3DVertexList + nDrawCount;

			int ti = i - nZStart;
			int tj = j - nXStart;

			int tic1 = ti * nXSize2;
			int tic2 = (ti + 1) * nXSize2;

			vIn[0].x = (FLOAT)j*nMapSize;		vIn[0].y = pHeightList[tic1 + tj] + SHADOW_LAND_YOFFSET;	vIn[0].z = (FLOAT)i*nMapSize;
			vIn[1].x = (FLOAT)(j+1)*nMapSize;	vIn[1].y = pHeightList[tic1 + tj+1] + SHADOW_LAND_YOFFSET;	vIn[1].z = (FLOAT)i*nMapSize;
			vIn[2].x = (FLOAT)j*nMapSize;		vIn[2].y = pHeightList[tic2 + tj] + SHADOW_LAND_YOFFSET;	vIn[2].z = (FLOAT)(i+1)*nMapSize;
			if( SetRwIm3DVertex( pIm3DVertex, vIn, pShadowInfo, alpha ) )
			{
				nDrawCount	+= 3;
				pIm3DVertex	+= 3;
			}

			vIn[0].x = (FLOAT)(j+1)*nMapSize;	vIn[0].y = pHeightList[tic1 + tj+1] + SHADOW_LAND_YOFFSET;	vIn[0].z = (FLOAT)i*nMapSize;
			vIn[1].x = (FLOAT)(j+1)*nMapSize;	vIn[1].y = pHeightList[tic2 + tj+1] + SHADOW_LAND_YOFFSET;	vIn[1].z = (FLOAT)(i+1)*nMapSize;
			vIn[2].x = (FLOAT)j*nMapSize;		vIn[2].y = pHeightList[tic2 + tj] + SHADOW_LAND_YOFFSET;	vIn[2].z = (FLOAT)(i+1)*nMapSize;
			if( SetRwIm3DVertex( pIm3DVertex, vIn, pShadowInfo, alpha ) )
				nDrawCount += 3;
		}
	}

	if( nDrawCount > 0 )
	{
		if( RwIm3DTransform( pIm3DVertexList, nDrawCount, NULL, rwIM3D_VERTEXUV | rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA | rwIM3D_NOCLIP ) )
		{
			RwIm3DRenderPrimitive( rwPRIMTYPETRILIST );
			RwIm3DEnd();
		}
	}

	AcuFrameMemory::DeallocFrameMemory( nVertexSize + nHightSize );

	RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void *) rwBLENDINVSRCALPHA );
	RwRenderStateSet( rwRENDERSTATESRCBLEND, (void *) rwBLENDSRCALPHA );
}

void AgcmShadow::InvertRaster(RwRaster* pRaster)
{
	if( !pRaster )		return;

	float crw = (FLOAT) RwRasterGetWidth(pRaster);
	float crh = (FLOAT) RwRasterGetHeight(pRaster);
    
	float recipZ = 1.0f / RwCameraGetNearClipPlane(m_pcShadowCamera);
	float nearz = RwIm2DGetNearScreenZ();
	
	RwIm2DVertexSetScreenX(&m_vTriFan[0], 0.f);
	RwIm2DVertexSetScreenY(&m_vTriFan[0], 0.f);
	RwIm2DVertexSetScreenZ(&m_vTriFan[0], nearz);
	RwIm2DVertexSetRecipCameraZ(&m_vTriFan[0], recipZ);
	
	RwIm2DVertexSetScreenX(&m_vTriFan[1], 0.f);
	RwIm2DVertexSetScreenY(&m_vTriFan[1], crh);
	RwIm2DVertexSetScreenZ(&m_vTriFan[1], nearz);
	RwIm2DVertexSetRecipCameraZ(&m_vTriFan[1], recipZ);
	
	RwIm2DVertexSetScreenX(&m_vTriFan[2], crw);
	RwIm2DVertexSetScreenY(&m_vTriFan[2], 0.f);
	RwIm2DVertexSetScreenZ(&m_vTriFan[2], nearz);
	RwIm2DVertexSetRecipCameraZ(&m_vTriFan[2], recipZ);
	
	RwIm2DVertexSetScreenX(&m_vTriFan[3], crw);
	RwIm2DVertexSetScreenY(&m_vTriFan[3], crh);
	RwIm2DVertexSetScreenZ(&m_vTriFan[3], nearz);
	RwIm2DVertexSetRecipCameraZ(&m_vTriFan[3], recipZ);
	
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) NULL);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void *) TRUE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *) rwBLENDZERO);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDINVDESTCOLOR);

	RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_vTriFan, 4);
}

void	AgcmShadow::UpdateBoundingSphere(RpClump* pClump)
{
	ListUpdateCallback*		pUpdateCallbackList	=	static_cast< ListUpdateCallback* >(pClump->stType.pUpdateList);

	if( pUpdateCallbackList )
	{
		ListUpdateCallbackIter	Iter		=	pUpdateCallbackList->begin();

		for( ; Iter != pUpdateCallbackList->end() ; ++Iter )
		{
			
			if( (*Iter)->pClass == this )
			{
				INT				nType		=	(INT)(*Iter)->data2;
				ShadowInfo*		pShadowInfo	=	(ShadowInfo*)(*Iter)->data1;
				RpClumpForAllAtomics( pClump , CalcBoundingSphere , (PVOID)pShadowInfo );
			}
		}

	}

}

BOOL AgcmShadow::AddShadow(RpClump* pclump)
{
	if( FALSE == DEF_FLAG_CHK( pclump->stType.eType, ACUOBJECT_TYPE_OBJECTSHADOW ) )		return FALSE;

	ShadowInfo*	pShadowInfo = new ShadowInfo( pclump );

	LockFrame();

	pShadowInfo->sphere.center.x	= pShadowInfo->sphere.center.y = pShadowInfo->sphere.center.z = 0.0f;
	pShadowInfo->spherecenter.x		= pShadowInfo->spherecenter.y = pShadowInfo->spherecenter.z = 0.0f;
	pShadowInfo->sphere.radius		= 0.0f;

	RpClumpForAllAtomics( pclump, CalcBoundingSphere, (void *) pShadowInfo );
	if( pShadowInfo->sphere.radius == 0 )
	{
		pShadowInfo->spherecenter.y	= 110.0f;
		pShadowInfo->sphere.radius	= 120.0f;
	}

	pShadowInfo->zoneradius = pShadowInfo->sphere.radius * SHADOW_SPHERE_EXPAND;// * 13.0f; // check later!

	if( !m_pcRender->AddUpdateInfotoClump( pclump, this, CB_Update, CB_Release, (PVOID)pShadowInfo, (PVOID)0 ) )
		delete pShadowInfo;

	UnlockFrame();

	return TRUE;
}

void AgcmShadow::DeleteShadow(RpClump* pclump)
{
	//ShadowInfoListItr Itr = m_listShadowInfo.find( pclump );
	//if( Itr != m_listShadowInfo.end() )
	//	m_listShadowInfo.erase( Itr );

	//for( int i=0; i<m_iTexUpdate1 ;++i )
	//	if( m_listTexUpdate1[i] && m_listTexUpdate1[i]->clump == pclump )
	//		m_listTexUpdate1[i] = NULL;

	//if( m_pMainCharacterTexUpdate && m_pMainCharacterTexUpdate->clump == pclump )
	//	m_pMainCharacterTexUpdate = NULL;

	m_pcRender->RemoveUpdateInfoFromClump2( pclump, this, CB_Update, CB_Release );
}

RpAtomic * AgcmShadow::CalcBoundingSphere(RpAtomic * atomic, void *data)
{
    ShadowInfo*	pShadowInfo = (ShadowInfo *) data;
	RwSphere	bs;
	AcuObject::GetAtomicBSphere(atomic,&bs);

	FLOAT distx = pShadowInfo->spherecenter.x - bs.center.x;
	FLOAT disty = pShadowInfo->spherecenter.y - bs.center.y;
	FLOAT distz = pShadowInfo->spherecenter.z - bs.center.z;
	FLOAT distance = (FLOAT)sqrt(distx*distx + disty*disty + distz*distz);
	if( bs.radius > distance )
		pShadowInfo->sphere.radius = bs.radius;
	else if(distance + bs.radius > pShadowInfo->sphere.radius)
		pShadowInfo->sphere.radius = distance + bs.radius;

	return atomic;
}

BOOL AgcmShadow::CB_POST_RENDER ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmShadow::CB_POST_RENDER");
	AgcmShadow* pThis = (AgcmShadow*)pClass;

	 //  Render a shadow in the given world, parallel projected from the
	 //  given shadow camera and raster.
	 //
	 //  Only the triangles in the given shadowZone intersection region are
	 //  considered. A world collision test is performed using this intersection
	 //  primitive, and any intersected triangles which lie within the shadow
	 //  texture region are use to construct an array of 3D immediate mode
	 //  triangles for drawing the shadow.
	 //
	 //  The shadowStrength sets the opacity of the shadow and should be
	 //  in the range -1 to 1. Negative values produce an inverted shadow
	 //  which clearly shows the full set of immediate mode triangles being
	 //  rendered.
	 //
	 // If enableFading is set to true, then the shadow will be faded out to 
	 //  zero at the distance from the center of the shadow camera specified by 
	 // fadeDist.
	 // This can provide both a natural looking effect, and also hide
	 //  the extent of the shadow zone, if the shadow falls outside of it.
	 //
	 //  This function should be called within a begin/end camera update.
	 //

	// 
	 //  Set renderstate. Use clamping for the texture address mode so that
	 //  we don't get a repeating shadow.
	 //
	if( pThis->m_bDrawShadow )
		pThis->Render();

	return TRUE;
}

BOOL	AgcmShadow::CB_Update( PVOID pData, PVOID pClass, PVOID pCustData )
{
	PROFILE("AgcmShadow::Shadow1 Update CB");
	AgcmShadow*	pThis = (AgcmShadow*)pClass;

	if( !pThis->m_bDrawShadow || !pThis->m_pGLightDirect )		return TRUE;
	ShadowInfo*	pShadowInfo = (ShadowInfo*) pData;

	if( DEF_FLAG_CHK( pShadowInfo->clump->stType.eType, ACUOBJECT_TYPE_INVISIBLE ) )		return TRUE;
	if( !pShadowInfo->clump->stUserData.characterShadowLevel )	return TRUE;	// 그림자 그려지는 캐릭터?

	int nLevel = pThis->GetLevel( pShadowInfo->clump );
	if( nLevel >= ShadowInfo::eLevelNon )						return TRUE;

	if( pShadowInfo->clump->stUserData.characterShadowLevel != 1 && (int)pThis->m_listShadowInfo.size() > pThis->m_iShadowDrawMaxNum )
		return TRUE;

	RwFrame* lightFrame = RpLightGetFrame(pThis->m_pGLightDirect);
	RwFrame* frame = RpClumpGetFrame(pShadowInfo->clump);
	RwMatrix* matrix = RwFrameGetLTM(frame);
	RwV3d vt = { 0.f, 0.f, 0.0f };
	
	RwV3dTransformPoints( &pShadowInfo->sphere.center, &vt, 1, matrix );
	pShadowInfo->sphere.center.x += pShadowInfo->spherecenter.x;
	pShadowInfo->sphere.center.z += pShadowInfo->spherecenter.z;
	pShadowInfo->level = nLevel;

	if( pShadowInfo->level < ShadowInfo::eLevelCircle )
	{
		pShadowInfo->sphere.center.y += pShadowInfo->spherecenter.y;
		pShadowInfo->fade = true;
	}
	else 
		pShadowInfo->fade = false;

	FLOAT	fsize = pShadowInfo->sphere.radius;
	
	RpClump* pRideClump = pThis->m_pAgcmCharacter->GetSelfRideClump();
	if( pShadowInfo->clump->stUserData.characterShadowLevel == 1 && pRideClump )
		fsize = pRideClump->stType.boundingSphere.radius;

	fsize *= 2.5f;
	pShadowInfo->cam_far		= 2.0f * fsize;
	pShadowInfo->cam_near		= 0.001f * fsize;
	pShadowInfo->view_size.x	= fsize;
	pShadowInfo->view_size.y	= fsize;

	RwMatrixCopy(&pShadowInfo->camMatirx, RwFrameGetMatrix(lightFrame));
	
	//  Set the center of the shadow camera frustum volume.
	RwV3d tr;
	RwV3dNegate(&tr, RwMatrixGetPos(&pShadowInfo->camMatirx));
	RwV3dAdd(&tr, &tr, &pShadowInfo->sphere.center);

	RwV3dIncrementScaled( &tr, RwMatrixGetAt(&pShadowInfo->camMatirx), -0.5f * pShadowInfo->cam_far );

	RwMatrixTranslate(&pShadowInfo->camMatirx, &tr , rwCOMBINEPOSTCONCAT);

	pShadowInfo->at = *RwMatrixGetAt( &pShadowInfo->camMatirx );

	RwMatrixInvert( &pShadowInfo->invMatrix, &pShadowInfo->camMatirx );

	RwV3d   scl;
	scl.x = scl.y = -0.5f / fsize;
	scl.z = 1.0f / (2.6f * fsize);//(pShadowInfo->zoneradius*0.25f + fsize);
	RwMatrixScale(&pShadowInfo->invMatrix, &scl, rwCOMBINEPOSTCONCAT);

	tr.x = tr.y = 0.5f;
	tr.z = 0.0f;
	RwMatrixTranslate( &pShadowInfo->invMatrix, &tr, rwCOMBINEPOSTCONCAT );
	
	RwV3d	boxDiag;
	boxDiag.x = boxDiag.y = boxDiag.z = pShadowInfo->zoneradius * 1.5f;

	RwV3dAdd( &pShadowInfo->colboxsup, &pShadowInfo->sphere.center, &boxDiag );
	RwV3dSub( &pShadowInfo->colboxinf, &pShadowInfo->sphere.center, &boxDiag );

	RwV3d	zoneVector;
	RwV3dScale( &zoneVector, &pShadowInfo->at, pShadowInfo->zoneradius );
	if( zoneVector.x > 0.0f )	pShadowInfo->colboxsup.x += zoneVector.x;
	else						pShadowInfo->colboxinf.x += zoneVector.x;

	if( zoneVector.y > 0.0f )	pShadowInfo->colboxsup.y += zoneVector.y;
	else						pShadowInfo->colboxinf.y += zoneVector.y;

	if( zoneVector.z > 0.0f )	pShadowInfo->colboxsup.z += zoneVector.z;
	else						pShadowInfo->colboxinf.z += zoneVector.z;

	pThis->m_listShadowInfo.push_back( pShadowInfo );

	return TRUE;
}

BOOL	AgcmShadow::CB_Release( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmShadow* pThis = (AgcmShadow*) pClass;
	ShadowInfo* pShadowInfo = (ShadowInfo*) pData;

	//그려질 목록에서 제거하고..
	ShadowInfoListItr Itr = find( pThis->m_listShadowInfo.begin(), pThis->m_listShadowInfo.end(), pShadowInfo );
	if( Itr != pThis->m_listShadowInfo.end() )
		pThis->m_listShadowInfo.erase( Itr );

	//없어진정보니깐 그냥 지워버린다
	DEF_SAFEDELETE( pShadowInfo );

	return TRUE;
}

int AgcmShadow::GetLevel( RpClump* pClump )
{
	if( !pClump )	return ShadowInfo::eLevelNon;

	for( int i = 0; i < 4; ++i )
		if( m_fLODOriginalShadowDistance[ pClump->stUserData.characterShadowLevel - 1][i] * m_fShadowDistFactor > pClump->stUserData.calcDistance )
			return i;

	return ShadowInfo::eLevelNon;
}

RwBool AgcmShadow::ShadowRasterResample(RwRaster * destRaster, RwRaster * sourceRaster)
{
    /*
     *  This resamples the sourceRaster into the destRaster which should
     *  be half the size in both dimensions, using the sourceRaster as a
     *  texture and the destRaster as a rendering target. Bilinear filtering
     *  achieves an anti-alias effect by averaging four pixels down into one.
     *
     *  A 'dummy' camera is required for the rendering which should
     *  have a z-raster of the same size as the destination raster.
     */

    FLOAT size = (FLOAT) RwRasterGetWidth(destRaster);
    FLOAT uvOffset = TEXELOFFSET / size;
    FLOAT recipCamZ = 1.0f / RwCameraGetFarClipPlane(m_pcShadowCamera);

	RwCameraSetRaster(m_pcShadowCamera, destRaster);
	if (RwCameraBeginUpdate(m_pcShadowCamera))
    {
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void *) rwBLENDONE);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void *) rwBLENDZERO);

        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) sourceRaster);
                         
        Im2DRenderQuad( 0.0f, 0.0f, size, size, RwIm2DGetFarScreenZ(), recipCamZ, uvOffset );

        RwCameraEndUpdate(m_pcShadowCamera);

#ifdef DOLPHIN
        RwGameCubeCameraTextureFlush( RwCameraGetRaster(m_pcShadowCamera), FALSE );
#endif
    }

    RwCameraSetRaster(m_pcShadowCamera, NULL);

    return TRUE;
}

/*
 ******************************************************************************
 */
RwBool AgcmShadow::ShadowRasterBlur(RwRaster * shadowRaster,UINT32 numPass)
{
    /*
     *  Blur the image in shadowRaster using tempRaster as a temporary
     *  buffer (both should be the same size and created with 
     *  ShadowRasterCreate). A 2D immediate mode rendering method is used
     *  which requires a 'dummy' camera to work with. This should have an
     *  appropriately sized Z-raster.
     *
     *  The technique used is one that will work on PS2 where methods of
     *  blurring the alpha channel are limited, but will also work on other
     *  platforms where color channels are used.
     *
     *  It works by effectively blitting one raster into another but with
     *  half a pixel offset in U and V so that bilinear filtering causes
     *  each pixel to be an average of four from the source. This is
     *  repeated with the opposite UV offset to increase the blur and
     *  leave the texture with no net displacement. The overall effect is
     *  equivalent to applying a 3x3 convolution matrix of
     *
     *      1 2 1
     *      2 4 2
     *      1 2 1
     */
    FLOAT size = (FLOAT) RwRasterGetWidth(shadowRaster);
    FLOAT uvStep = 1.0f / size;
    FLOAT recipCamZ = 1.0f / RwCameraGetFarClipPlane(m_pcShadowIPCamera);
	RwRaster* tempRaster = m_pcShadowRasterBlur256;
	
    for( UINT32 iPass = 0; iPass < numPass; ++iPass )
    {
        RwCameraSetRaster( m_pcShadowIPCamera, tempRaster );

        if( RwCameraBeginUpdate( m_pcShadowIPCamera ) )
        {
            RwRenderStateSet( rwRENDERSTATESRCBLEND, (void *) rwBLENDONE);
            RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void *) rwBLENDZERO);
			RwRenderStateSet( rwRENDERSTATETEXTURERASTER, (void *) shadowRaster);
            Im2DRenderQuad( 0.f, 0.f, size, size, RwIm2DGetFarScreenZ(), recipCamZ, (TEXELOFFSET + 0.5f) * uvStep);
            RwCameraEndUpdate( m_pcShadowIPCamera );

#ifdef DOLPHIN
            RwGameCubeCameraTextureFlush( RwCameraGetRaster(m_pcShadowIPCamera), FALSE );
#endif
        }

        RwCameraSetRaster(m_pcShadowIPCamera, shadowRaster);

        if (RwCameraBeginUpdate(m_pcShadowIPCamera))
        {
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *) tempRaster);
            Im2DRenderQuad( 0.f, 0.f, size, size, RwIm2DGetFarScreenZ(), recipCamZ, (TEXELOFFSET - 0.5f) * uvStep );
            RwCameraEndUpdate(m_pcShadowIPCamera);

#ifdef DOLPHIN
            RwGameCubeCameraTextureFlush( RwCameraGetRaster(m_pcShadowIPCamera), FALSE) ;
#endif
        }
    }

    RwCameraSetRaster(m_pcShadowIPCamera, NULL);

    return TRUE;
}

RwBool AgcmShadow::Im2DRenderQuad(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, FLOAT z, FLOAT recipCamZ, FLOAT uvOffset)
{
    // Render an opaque white 2D quad at the given coordinates and spanning a whole texture.
    RwIm2DVertexSetScreenX(&m_vTriFan[0], x2);
    RwIm2DVertexSetScreenY(&m_vTriFan[0], y2);
    RwIm2DVertexSetScreenZ(&m_vTriFan[0], z);
	RwIm2DVertexSetRecipCameraZ(&m_vTriFan[0], recipCamZ);
    RwIm2DVertexSetU(&m_vTriFan[0], 1.0f + uvOffset, recipCamZ);
    RwIm2DVertexSetV(&m_vTriFan[0], 1.0f + uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&m_vTriFan[1], x2);
    RwIm2DVertexSetScreenY(&m_vTriFan[1], y1);
    RwIm2DVertexSetScreenZ(&m_vTriFan[1], z);
	RwIm2DVertexSetRecipCameraZ(&m_vTriFan[1], recipCamZ);
    RwIm2DVertexSetU(&m_vTriFan[1], 1.0f + uvOffset, recipCamZ);
    RwIm2DVertexSetV(&m_vTriFan[1], uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&m_vTriFan[2], x1);
    RwIm2DVertexSetScreenY(&m_vTriFan[2], y2);
    RwIm2DVertexSetScreenZ(&m_vTriFan[2], z);
	RwIm2DVertexSetRecipCameraZ(&m_vTriFan[2], recipCamZ);
    RwIm2DVertexSetU(&m_vTriFan[2], uvOffset, recipCamZ);
    RwIm2DVertexSetV(&m_vTriFan[2], 1.0f + uvOffset, recipCamZ);

    RwIm2DVertexSetScreenX(&m_vTriFan[3], x1);
    RwIm2DVertexSetScreenY(&m_vTriFan[3], y1);
    RwIm2DVertexSetScreenZ(&m_vTriFan[3], z);
    RwIm2DVertexSetRecipCameraZ(&m_vTriFan[3], recipCamZ);
    RwIm2DVertexSetU(&m_vTriFan[3], uvOffset, recipCamZ);
    RwIm2DVertexSetV(&m_vTriFan[3], uvOffset, recipCamZ);

    RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, m_vTriFan, 4);

    return TRUE;
}