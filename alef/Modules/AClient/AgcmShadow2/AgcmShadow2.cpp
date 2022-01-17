#include "AgcmShadow2.h"
#include "AcuRpMTexture.h"
#include "AcuTexture.h"
#include "AgcEngine.h"
#include "AgcmUIConsole.h"


#define SHADOW_INITIAL_DARKEN_VALUE 130

extern AgcEngine *	g_pEngine;

AgcmShadow2::AgcmShadow2():m_pFrameLight( NULL )
{
	SetModuleName("AgcmShadow2");
	EnableIdle(TRUE);

	m_pcRender = NULL;
	m_pcApmMap = NULL;
	m_pcAgcmMap = NULL;
	m_pcShadow = NULL;
		
	m_pcRenderRaster = NULL;
	m_pcShadowTexture = NULL;
	
	m_pcsDXTTexture1 = NULL;
	m_pstRasterTemp = NULL;
	m_pcsDXTTexture2 = NULL;

	m_bUse1 = TRUE;

	m_pGLightAmbient = NULL;	
	m_pGLightDirect = NULL;
	m_pWorldCamera = NULL;
	m_pWorld = NULL;
		
	m_pcShadowCamera2 = NULL;
	m_iShadowTexUpdate = 0;

	m_ulLastTick = 0;

	m_iDrawRange = 0;
	m_pWSector = NULL;

	ZeroMemory(&m_IMatrix, sizeof(RwMatrix));

	m_listShadow2 = NULL;

	m_lLoaderID = 0;
	m_lShadowDimension = 0;

	m_bEnableShadow = FALSE;

	m_pcsApmOcTree = NULL;
	m_pcsAgcmOcTree = NULL;

	ZeroMemory(m_cShadowBuffer, sizeof(BYTE) * 64 * 512);
	ZeroMemory(m_cMaskShadowBuffer, sizeof(BYTE) * 8);

	m_nShadowDarkenValue = SHADOW_INITIAL_DARKEN_VALUE;
}

AgcmShadow2::~AgcmShadow2()
{
}

BOOL AgcmShadow2::OnAddModule()
{
	m_pcRender		= (AgcmRender *)GetModule("AgcmRender");
	m_pcShadow		= (AgcmShadow*)GetModule("AgcmShadow");
	m_pcApmMap		= (ApmMap *)GetModule("ApmMap");
	m_pcAgcmMap		= (AgcmMap*)GetModule("AgcmMap");
	m_pcsApmOcTree	= (ApmOcTree*)GetModule("ApmOcTree");
	m_pcsAgcmOcTree	= (AgcmOcTree*)GetModule("AgcmOcTree");
	
	if ( !m_pcRender ||
		 !m_pcAgcmMap || 
		 !m_pcShadow ||
		 !m_pcAgcmMap ||
		 !m_pcsApmOcTree ||
		 !m_pcsAgcmOcTree )
		return FALSE;

	m_pcRender->SetCallbackPostRender( CB_POST_RENDER, this );
	m_pcAgcmMap->SetCallbackSectorChange( CB_SECTOR_CAHNGE, this );
	m_pcAgcmMap->SetCallbackMapLoadEnd( CB_SECTOR_CAHNGE, this );

	AddWorldAmbientLight( GetAmbientLight() );
	AddWorldDirectLight( GetDirectionalLight() );
	AddWorldCamera( GetCamera() );
	AddRpWorld( GetWorld() );

	m_pcRender->m_iShadow2Range = m_iDrawRange = 4;

	m_lShadowDimension = SHADOW2_TEXTURE_SIZE;

	return TRUE;
}

BOOL AgcmShadow2::OnInit()
{
	if ( !m_pcRender->m_bVertexShaderEnable )		return TRUE;

	m_pFrameLight	= RwFrameCreate();
	ChangeLightAngle( 14.0f );

	//AcuRpMTexture 변수 set
	RpMTextureSetCamera( m_pWorldCamera );
	RpMTextureSetInvSMatrix( &m_IMatrix );

	m_cMaskShadowBuffer[0] = 0x01;
	m_cMaskShadowBuffer[1] = 0x02;
	m_cMaskShadowBuffer[2] = 0x04;
	m_cMaskShadowBuffer[3] = 0x08;
	m_cMaskShadowBuffer[4] = 0x10;
	m_cMaskShadowBuffer[5] = 0x20;
	m_cMaskShadowBuffer[6] = 0x40;
	m_cMaskShadowBuffer[7] = 0x80;

	AS_REGISTER_TYPE_BEGIN( AgcmShadow2 , AgcmShadow2 );
		AS_REGISTER_METHOD1(void, ChangeDarkenValue, int);
		AS_REGISTER_METHOD1(void, ChangeLightAngle, float);
	AS_REGISTER_TYPE_END;

	return TRUE;
}

void	AgcmShadow2::ChangeDarkenValue( int nValue )
{
	m_nShadowDarkenValue = max( 0x00, min( nValue, 0xff ) );
}

void	AgcmShadow2::ChangeLightAngle( float fValue	)
{
	ASSERT( m_pFrameLight );

	RwV3d	zaxis = { 0.f, 0.f, 1.f };
	RwV3d	yaxis = { 0.f, 1.f, 0.f };

	fValue = max( 0.f, min( fValue, 24.f ) );
	fValue /= 24.0f;

	FLOAT	fRate = fValue;//14.0f / 24.0f;
	FLOAT	fAngle	= 90.0f + 360.0f * fRate;

	RwFrame* pFrame	= m_pFrameLight;
	ASSERT( pFrame );

	RwMatrix* pMatrix = RwFrameGetMatrix( pFrame );
	RwV3d* pAtVector = RwMatrixGetAt( pMatrix );

	pAtVector->x	= -1.0f;
	pAtVector->y	= 0.0f;
	pAtVector->z	= 0.0f;

	RwMatrixRotate( pMatrix, &yaxis, 50, rwCOMBINEREPLACE );
	RwMatrixRotate( pMatrix, &zaxis, fAngle, rwCOMBINEPOSTCONCAT );

	RwMatrixUpdate( pMatrix );
	RwMatrixOrthoNormalize( pMatrix, pMatrix );

	LockFrame();
	RwFrameUpdateObjects( pFrame );
	UnlockFrame();
}


BOOL AgcmShadow2::OnDestroy()
{
	LockFrame();

	if ( m_pcShadowCamera2 )
    {
        RwFrame* frame = RwCameraGetFrame(m_pcShadowCamera2);
        if (frame)
        {
            RwCameraSetFrame(m_pcShadowCamera2, NULL);
            RwFrameDestroy(frame);
        }
        
        RwCameraDestroy(m_pcShadowCamera2);
		m_pcShadowCamera2 = NULL;
    }

	if( m_pcShadowTexture )
	{
		RwTextureDestroy( m_pcShadowTexture );
		m_pcShadowTexture = NULL;
	}

	if( m_pstRasterTemp )
	{
		RwRasterDestroy( m_pstRasterTemp );
		m_pstRasterTemp = NULL;
	}

	if( m_pcRenderRaster )
	{
		RwRasterDestroy( m_pcRenderRaster );
		m_pcRenderRaster = NULL;
	}

	UnlockFrame();

	return TRUE;
}

BOOL AgcmShadow2::SetEnable( BOOL bEnable )
{
	if ( !m_pcRender->m_bVertexShaderEnable )	return TRUE;

	if( bEnable && !m_bEnableShadow )		// shadow2 obj 생성
	{
		LockFrame();

		ASSERT( !m_pcShadowCamera2 );

		m_pcShadowCamera2 = RwCameraCreate();
		if ( !m_pcShadowCamera2 )
			goto FAIL;

		RwCameraSetFrame( m_pcShadowCamera2, RwFrameCreate() );
		if( !RwCameraGetFrame( m_pcShadowCamera2 ) )
			goto FAIL;

		RwCameraSetProjection( m_pcShadowCamera2, rwPARALLEL );
		RwCameraSetZRaster( m_pcShadowCamera2, NULL );

		m_pcRenderRaster = RwRasterCreate( m_lShadowDimension, m_lShadowDimension, 32, rwRASTERTYPECAMERATEXTURE );
		if( !m_pcRenderRaster )
			goto FAIL;

		RwCameraSetRaster( m_pcShadowCamera2, m_pcRenderRaster );

		RwRaster* ras = RwRasterCreate( m_lShadowDimension, m_lShadowDimension, 32, rwRASTERTYPECAMERATEXTURE );
		if ( !ras )
			goto FAIL;

		m_pcShadowTexture = RwTextureCreate( ras );
		RwTextureSetFilterMode( m_pcShadowTexture, rwFILTERLINEAR );
		RpMTextureSetShadowTexture( m_pcShadowTexture );

		UnlockFrame();

		m_iShadowTexUpdate	= 1;		// for update shadow texture
		m_bEnableShadow		= TRUE;
	}
	else if( !bEnable && m_bEnableShadow )
	{
		ASSERT( m_pcShadowCamera2 );		// null??
		
		OnDestroy();
		m_bEnableShadow = FALSE;
	}

	return TRUE;

FAIL:
	UnlockFrame();
	OnDestroy();

	return FALSE;
}

BOOL AgcmShadow2::OnIdle(UINT32 ulClockCount)
{
	UINT32	tickdiff = ulClockCount - m_ulLastTick;
	m_ulLastTick = ulClockCount;

	if( m_iShadowTexUpdate < 3 && m_iShadowTexUpdate > 0 )
		++m_iShadowTexUpdate;

	m_ulUpdateTick += tickdiff;
	if( m_ulUpdateTick > 120000 )		// 2분
	{
		m_ulUpdateTick = 0;
		if( m_pWSector )
			m_iShadowTexUpdate = 1; 
	}

	return TRUE;
}

void	AgcmShadow2::OnCameraStateChange( CAMERASTATECHANGETYPE	ctype )
{
	if(m_pWSector && (CSC_RESIZE == ctype || CSC_ACTIVATE == ctype) )
		m_iShadowTexUpdate = 1;
}

SectorRenderList*	AgcmShadow2::GetSectorData(ApWorldSector *pSector)
{
	return (SectorRenderList*) m_pcApmMap->GetAttachedModuleData( m_pcRender->m_iSectorDataIndex, (PVOID)pSector );
}

RpClump*	AgcmShadow2::ClumpCBSetShadowList( RpClump* clump, void *data )
{
	AgcmShadow2* pThis = (AgcmShadow2*)data;

	if( AcuObject::GetProperty( clump->stType.eType ) & ACUOBJECT_TYPE_OBJECTSHADOW )
	{
		Shadow2Node* nw_node = (Shadow2Node*)AcuFrameMemory::AllocFrameMemory(sizeof(Shadow2Node));
		if( nw_node  )
		{
			nw_node->pClump = clump;

			nw_node->next = pThis->m_listShadow2;
			pThis->m_listShadow2 = nw_node;
		}
	}

	return clump;
}

BOOL AgcmShadow2::ShadowTextureRender ()			// shadow texture 그린다.
{
	PROFILE("AgcmShadow2::ShadowTextureRender()");

	if( !m_bEnableShadow || !m_pcRender->m_bVertexShaderEnable )	return TRUE;
	if( m_iShadowTexUpdate != 3 || !m_pWSector )					return FALSE;

	static INT32 nCorruptCheckArray[65536];

	m_iShadowTexUpdate	= 0;
	m_ulUpdateTick		= 0;
	m_listShadow2		= NULL;

	int		ix = m_pWSector->GetArrayIndexX();
	int		iz = m_pWSector->GetArrayIndexZ();

	INT32	sx = ix - m_iDrawRange;
	INT32	ex = ix + m_iDrawRange;
	INT32	sz = iz - m_iDrawRange;
	INT32	ez = iz + m_iDrawRange;

	if( m_pcsApmOcTree && m_pcsApmOcTree->m_bOcTreeEnable )
	{
		for( int i=sx; i<=ex; ++i )
		{
			if( i < 0 )		continue;
			for( int j=sz; j<=ez; ++j )
			{
				if( j < 0 )		continue;
		
				m_pcsAgcmOcTree->OcTreeForAllClumps( i, j, ClumpCBSetShadowList, this, nCorruptCheckArray );
			}
		}
	}
	else
	{
		// Shadow2 List 구성
		for(int i=sx;i<=ex;++i)
		{
			if( i<0 )			continue;
			for(int j=sz;j<=ez;++j)
			{
				if( j<0 )		continue;

				ApWorldSector* pSector = m_pcApmMap->GetSectorByArrayIndex( i, j );
				if( !pSector )	continue;

				SectorRenderList* pList = GetSectorData(pSector);

				for( WorldSphereListItr Itr = pList->listNotInStatic->begin(); Itr != pList->listNotInStatic->end(); ++Itr )
				{
					AgcdType* pType = AcuObject::GetAtomicTypeStruct( (*Itr).atomic );
					if( AcuObject::GetProperty( pType->eType ) & ACUOBJECT_TYPE_OBJECTSHADOW )
						if( (*Itr).cSphere.radius > 100.0f )
							AddToShadow2List( (*Itr).atomic );
				}

				for( WorldSphereListItr Itr = pList->listInStatic->begin(); Itr != pList->listInStatic->end(); ++Itr )
				{
					AgcdType* pType = AcuObject::GetAtomicTypeStruct( (*Itr).atomic );
					if( AcuObject::GetProperty( pType->eType ) & ACUOBJECT_TYPE_OBJECTSHADOW )
						if( (*Itr).cSphere.radius > 100.0f )
							AddToShadow2List( (*Itr).atomic );
				}
			}
		}
	}

	// shadow2 camera 위치 변경 
	RwV3d	center;
	center.x = (m_pWSector->GetXEnd()  + m_pWSector->GetXStart()) * 0.5f;
	center.y = 0.0f;
	center.z = (m_pWSector->GetZEnd() + m_pWSector->GetZStart()) * 0.5f;

	float	fsize = MAP_SECTOR_WIDTH * SHADOW2_RADIUS_WEIGHT * m_iDrawRange; // (0.707 -- 1.414(root2) * 0.5f(반지름))
	RwV2d	view_size;
	view_size.x = fsize;
	view_size.y = fsize;

	float far_clip,near_clip;
	far_clip = 2.0f * fsize;
	near_clip = 0.001f * fsize;

	LockFrame();
	RwCameraSetFarClipPlane( m_pcShadowCamera2, far_clip );
	RwCameraSetNearClipPlane( m_pcShadowCamera2, near_clip );
	RwCameraSetViewWindow( m_pcShadowCamera2, &view_size );
	UnlockFrame();
	
	RwFrame* camFrame = RwCameraGetFrame( m_pcShadowCamera2 );
	RwMatrix* camMatrix = RwFrameGetMatrix( camFrame );
	RwMatrixCopy( camMatrix, RwFrameGetMatrix( m_pFrameLight ) );

	RwV3d* at = RwMatrixGetAt( camMatrix );

	RwV3d  tr;
	RwV3dNegate( &tr, RwMatrixGetPos( camMatrix ) );
	RwV3dAdd( &tr, &tr, &center );
	RwV3dIncrementScaled( &tr, at, -0.5f * far_clip );

	LockRender();

	//
	//  Clear to white background with alpha = 0.
	//	

	LockFrame();
	UINT32 dFlags = RpLightGetFlags( m_pGLightDirect );
	RpLightSetFlags( m_pGLightDirect, 0 );

	UINT32 aFlags = RpLightGetFlags( m_pGLightAmbient );
	RpLightSetFlags( m_pGLightAmbient, 0 );

	RwFrameTranslate( camFrame, &tr , rwCOMBINEPOSTCONCAT );
	UnlockFrame();

	// Shadow2 그린다
	RwCameraSetRaster( m_pcShadowCamera2, m_pcRenderRaster );

	RwRGBA bgColor = { 255, 255, 255, 0 };
	RwCameraClear( m_pcShadowCamera2, &bgColor, rwCAMERACLEARIMAGE );

	LockFrame();
	if (RwCameraBeginUpdate(m_pcShadowCamera2))
	{
		UnlockFrame();
		Shadow2Node*		cur_s2 = m_listShadow2;
		RwFrame*			clump_frame;
		float				y_val;
		RwV3d*				pos;
		INT32				orginal_flag;
				
		while(cur_s2)
		{
			clump_frame = RpClumpGetFrame(cur_s2->pClump);
			pos = RwMatrixGetPos(RwFrameGetMatrix(clump_frame));

			orginal_flag	= -1;
			y_val			= pos->y;
			pos->y			= 0.0f;

			if( m_pcRender )	m_pcRender->OriginalClumpRender(cur_s2->pClump);
			else				RpClumpRender( cur_s2->pClump );
				
			pos->y = y_val;
			
			cur_s2 = cur_s2->next;
		}

		#if ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) )
		InvertRaster( m_pcRenderRaster );
		#endif // ( !(defined(SKY) || defined( SOFTRAS_DRVMODEL_H )) ) 

		RwCameraEndUpdate(m_pcShadowCamera2);
	}
	else 
		UnlockFrame();

	DarkerRaster( m_pcRenderRaster );

	UnlockRender();

	RpLightSetFlags( m_pGLightDirect, dFlags );
	RpLightSetFlags( m_pGLightAmbient, aFlags );

	ApplyMatrix();

	UpdateShadowBuffer();

	EnumCallback( AGCMSHADOW2_CB_ID_TEXTURE_UPDATE, NULL, NULL );

	return TRUE;
}

void	AgcmShadow2::AddToShadow2List(RpAtomic*	pAtomic)
{
	RpClump*	pClump = RpAtomicGetClump(pAtomic);
	if(!pClump) return;

	bool	bFind = false;
	Shadow2Node*	cur_node = m_listShadow2;
	while(cur_node)
	{
		if(cur_node->pClump == pClump)
		{
			bFind = true;
			break;
		}
		cur_node = cur_node->next;
	}

	if(!bFind)
	{
		Shadow2Node*	nw_node = (Shadow2Node*)AcuFrameMemory::AllocFrameMemory(sizeof(Shadow2Node));
		if( nw_node )
		{
			nw_node->pClump = pClump;

			nw_node->next = m_listShadow2;
			m_listShadow2 = nw_node;
		}
	}
}

BOOL	AgcmShadow2::CB_SECTOR_CAHNGE ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmShadow2*		pThis = (AgcmShadow2*)pClass;

	ApWorldSector*		pSector = (ApWorldSector*)pData;
	if(!pSector) return FALSE;
	
	TRACE("AgcmShadow2::CB_SECTOR_CAHNGE (%d,%d) %x\n", pSector->GetArrayIndexX(), pSector->GetArrayIndexZ(), pSector);

	if(pSector != pThis->m_pWSector)
	{
		pThis->m_pWSector = pSector;
	}

	if(pThis->m_bEnableShadow) 
		pThis->m_iShadowTexUpdate = 1;

	return TRUE;
}

BOOL	AgcmShadow2::CB_POST_RENDER( PVOID pData, PVOID pClass , PVOID pCustData)
{
	AgcmShadow2*		pThis = (AgcmShadow2*)pClass;

	if (!pThis->m_bEnableShadow)
		return TRUE;

	return TRUE;
}

VOID AgcmShadow2::ApplyMatrix()
{
	RwMatrix* camMatrix = RwFrameGetMatrix( RwCameraGetFrame( m_pcShadowCamera2 ) );
	RwMatrixInvert( &m_IMatrix, camMatrix );

	FLOAT fsize = MAP_SECTOR_WIDTH * SHADOW2_RADIUS_WEIGHT * m_iDrawRange; // (0.707 -- 1.414(root2) * 0.5f(반지름))

	RwV3d	temp;
	temp.x = temp.y = -0.5f / fsize;
	temp.z = 1.0f / ( fsize + (fsize*0.25f) );						// check
	RwMatrixScale( &m_IMatrix, &temp, rwCOMBINEPOSTCONCAT );

	temp.x = temp.y = 0.5f;
	temp.z = 0.0f;
	RwMatrixTranslate( &m_IMatrix, &temp, rwCOMBINEPOSTCONCAT );
}

void	AgcmShadow2::UpdateShadowBuffer()
{
	RwRaster* pRas = RwTextureGetRaster(m_pcShadowTexture);
	BYTE* pSrc = (BYTE*) RwRasterLock(pRas,0,rwRASTERLOCKREAD);
	int SrcPitch = RwRasterGetStride(pRas);

	int	width = SHADOW2_TEXTURE_SIZE, height = SHADOW2_TEXTURE_SIZE;
	int pixsize = RwRasterGetDepth(pRas) / 8;

	memset( m_cShadowBuffer, 0, 64*512 );
	
	for( int y=0;y < height; ++y )
	{
		int ty = SrcPitch * y;
		for( int x = 0; x < width; x+=8 )
		{
			int xtemp = x>>3;	// x/8
			int tx = x * pixsize;
			
			for(int x2=0;x2<8;++x2)
			{
				if( pSrc[ty + tx + x2*pixsize ] < 32 && 
					pSrc[ty + tx + x2*pixsize + 1] < 32 && 
					pSrc[ty + tx + x2*pixsize + 2] < 32 )	// 쉐도우 없음
				{
					// do nothing
				}
				else
				{
					m_cShadowBuffer[xtemp][y] |= (0x1 << x2);
				}
			}
		}
	}
				
	RwRasterUnlock( pRas );
}

//해당 월드 좌표의 shadow 영역이 true인지 false인지 리턴한다..
BOOL	AgcmShadow2::GetShadowBuffer(RwV3d*	pos)
{
	if(!m_bEnableShadow || !m_pcRender->m_bVertexShaderEnable) return FALSE;

	BOOL	rval;
	RwV3d	pix_uv;
	RwV3dTransformPoint( &pix_uv, pos, &m_IMatrix );

	if(pix_uv.x < 0.0f || pix_uv.x > 1.0f || pix_uv.y < 0.0f || pix_uv.y > 1.0f)		// clip
		return FALSE;

	INT32	x = (INT32)(SHADOW2_TEXTURE_SIZE * pix_uv.x);
	INT32	y = (INT32)(SHADOW2_TEXTURE_SIZE * pix_uv.y);

	INT32	x1 = x >> 3;
	INT32	x2 = x % 8;

	rval = m_cShadowBuffer[x1][y] & m_cMaskShadowBuffer[x2]; 		

	return rval;
}

BOOL	AgcmShadow2::SetCallbackTextureUpdate( ApModuleDefaultCallBack pfCallback, PVOID pClass )
{
	return SetCallback( AGCMSHADOW2_CB_ID_TEXTURE_UPDATE, pfCallback, pClass );
}

void AgcmShadow2::BrightenShadow(RwRaster* pRaster)
{
	RwRenderStateSet( rwRENDERSTATETEXTURERASTER, (void*)NULL );
	RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE );
	RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE );
	RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDONE );

	RenderRaster( pRaster, 255 - 120 );
}

void	AgcmShadow2::DarkerRaster(RwRaster*	pRaster)
{
	RwCameraSetRaster( m_pcShadowCamera2, RwTextureGetRaster( m_pcShadowTexture ) );
	if (RwCameraBeginUpdate(m_pcShadowCamera2))
    {
        RwRenderStateSet( rwRENDERSTATETEXTURERASTER, (void*)pRaster );
		RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE );
		RwRenderStateSet( rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA );
		RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA );

		RenderRaster( pRaster, m_nShadowDarkenValue );

		RwCameraEndUpdate( m_pcShadowCamera2 );
    }

    RwCameraSetRaster( m_pcShadowCamera2, NULL );
}

void AgcmShadow2::InvertRaster(RwRaster* pRaster)
{
	RwRenderStateSet( rwRENDERSTATETEXTURERASTER, (void *) NULL );
	RwRenderStateSet( rwRENDERSTATEVERTEXALPHAENABLE, (void *) TRUE );
	RwRenderStateSet( rwRENDERSTATEDESTBLEND, (void *) rwBLENDZERO );
	RwRenderStateSet( rwRENDERSTATESRCBLEND, (void *)rwBLENDINVDESTCOLOR );

	RenderRaster( pRaster, 255 );
}

BOOL AgcmShadow2::CBBlitter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgcmShadow2 *	pThis = (AgcmShadow2 *) pvClass;
	RwTexture *		pstTexture = (RwTexture *) pvData;
	UINT32 *		pPixelSrc;
	UINT8 *			pPixelDst;

	TRACE("AgcmShadow2::CBBlitter() Start...\n");

	AcuTexture::CopyRaster(pThis->m_pstRasterTemp, RwTextureGetRaster(pThis->m_pcShadowTexture), ACUTEXTURE_COPY_RENDERTARGETDATA);

	pPixelSrc = (UINT32 *) RwRasterLock(pThis->m_pstRasterTemp, 0, rwRASTERLOCKREAD);
	pPixelDst = RwRasterLock(RwTextureGetRaster(pstTexture), 0, rwRASTERLOCKWRITE);

	for ( int i = 0; i < pThis->m_lShadowDimension * pThis->m_lShadowDimension; ++i)
	{
		*pPixelDst = ((*pPixelSrc & 0xff) + ((*pPixelSrc >> 8) & 0xff) + ((*pPixelSrc >> 16) & 0xff)) / 3;

		++pPixelSrc;
		++pPixelDst;
	}

	RwRasterUnlock(pThis->m_pstRasterTemp);
	RwRasterUnlock(RwTextureGetRaster(pstTexture));

	TRACE("AgcmShadow2::CBBlitter() CopyRaster...\n");

	pThis->LockRender();

	pThis->ApplyMatrix();
	RpMTextureSetShadowTexture(pstTexture);

	pThis->UnlockRender();

	TRACE("AgcmShadow2::CBBlitter() End...\n");

	return TRUE;
}


void	AgcmShadow2::RenderRaster( RwRaster* pRaster, RwUInt8 byVal )
{
	FLOAT crw = (FLOAT) RwRasterGetWidth(pRaster);
	FLOAT crh = (FLOAT) RwRasterGetHeight(pRaster);
    
	FLOAT recipCamZ = 1.0f / RwCameraGetNearClipPlane( m_pcShadowCamera2 );
	FLOAT nearz = RwIm2DGetNearScreenZ();

	RwIm2DVertex vTriFan[4];
    RwIm2DVertexSetIntRGBA(&vTriFan[0], byVal, byVal, byVal, 255);
	RwIm2DVertexSetScreenX(&vTriFan[0], crw);
    RwIm2DVertexSetScreenY(&vTriFan[0], crh);
    RwIm2DVertexSetScreenZ(&vTriFan[0], nearz);
	RwIm2DVertexSetRecipCameraZ(&vTriFan[0], recipCamZ);
    RwIm2DVertexSetU(&vTriFan[0], 1.0f, recipCamZ);
    RwIm2DVertexSetV(&vTriFan[0], 1.0f, recipCamZ);

	RwIm2DVertexSetIntRGBA(&vTriFan[1], byVal, byVal, byVal, 255);
    RwIm2DVertexSetScreenX(&vTriFan[1], crw);
    RwIm2DVertexSetScreenY(&vTriFan[1], 0.0f);
    RwIm2DVertexSetScreenZ(&vTriFan[1], nearz);
	RwIm2DVertexSetRecipCameraZ(&vTriFan[1], recipCamZ);
    RwIm2DVertexSetU(&vTriFan[1], 1.0f, recipCamZ);
    RwIm2DVertexSetV(&vTriFan[1], 0.0f, recipCamZ);

	RwIm2DVertexSetIntRGBA(&vTriFan[2], byVal, byVal, byVal, 255);
    RwIm2DVertexSetScreenX(&vTriFan[2], 0.0f);
    RwIm2DVertexSetScreenY(&vTriFan[2], crh);
    RwIm2DVertexSetScreenZ(&vTriFan[2], nearz);
	RwIm2DVertexSetRecipCameraZ(&vTriFan[2], recipCamZ);
    RwIm2DVertexSetU(&vTriFan[2], 0.0f, recipCamZ);
    RwIm2DVertexSetV(&vTriFan[2], 1.0f, recipCamZ);

    RwIm2DVertexSetIntRGBA(&vTriFan[3], byVal, byVal, byVal, 255);
	RwIm2DVertexSetScreenX(&vTriFan[3], 0.0f);
    RwIm2DVertexSetScreenY(&vTriFan[3], 0.0f);
    RwIm2DVertexSetScreenZ(&vTriFan[3], nearz);
    RwIm2DVertexSetRecipCameraZ(&vTriFan[3], recipCamZ);
    RwIm2DVertexSetU(&vTriFan[3], 0.0f, recipCamZ);
    RwIm2DVertexSetV(&vTriFan[3], 0.0f, recipCamZ);

	RwIm2DRenderPrimitive( rwPRIMTYPETRISTRIP, vTriFan, 4 );
}