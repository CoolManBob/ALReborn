// AgcmGlare.cpp: implementation of the AgcmGlare class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcmGlare.h"
#include "AcuTexture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmGlare::AgcmGlare()
{
	SetModuleName("AgcmGlare");

	m_bEnableGlare = FALSE;

	m_pRaster = NULL;
	m_pRaster2 = NULL;
}

AgcmGlare::~AgcmGlare()
{

}

BOOL	AgcmGlare::OnAddModule()
{
	m_pcsAgcmRender = (AgcmRender *) GetModule("AgcmRender");
	m_pCamera = GetCamera();

	if (!m_pCamera || !m_pcsAgcmRender)
		return FALSE;

	if (!m_pcsAgcmRender->SetCallbackPostRender(CBPostRender, this))
		return FALSE;

	return TRUE;
}

BOOL	AgcmGlare::OnInit()
{
	if (!m_bEnableGlare)
		return TRUE;

	RwRaster *	pRaster = RwCameraGetRaster(m_pCamera);

	m_pRaster = RwRasterCreate(pRaster->width, pRaster->height, 16, rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888);
	m_pRaster2 = RwRasterCreate(pRaster->width, pRaster->height, 16, rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888);

	m_csBlur.PShaderOpen();

	return TRUE;
}

BOOL	AgcmGlare::OnDestroy()
{
	if (m_pRaster)
		RwRasterDestroy(m_pRaster);

	if (m_pRaster2)
		RwRasterDestroy(m_pRaster2);

	if (m_bEnableGlare)
		m_csBlur.PShaderClose();

	return TRUE;
}

BOOL	AgcmGlare::CBPostRender(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	PROFILE("AgcmGlare::CBPostRender");

	AgcmGlare *	pThis = (AgcmGlare *) pvClass;
	if (!pThis->m_bEnableGlare)
		return TRUE;

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);
	RwRenderStateSet( rwRENDERSTATEZWRITEENABLE , ( void * ) TRUE );

	RwRaster *	pRaster = RwCameraGetRaster(pThis->m_pCamera);

	RwD3D9SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	RwD3D9SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RwD3D9SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	RwRGBA		rgbBackgroundColor = {0,0,0,0};

	{
		IDirect3DSurface9 *	pSrcSurface = (IDirect3DSurface9 *) RwD3D9GetCurrentD3DRenderTarget(0);
		IDirect3DSurface9 *	pDstSurface = NULL;

		AcuTexture::GetDXTextureFromRaster(pThis->m_pRaster)->GetSurfaceLevel(0, &pDstSurface);

		((LPDIRECT3DDEVICE9) RwD3D9GetCurrentD3DDevice())->StretchRect(pSrcSurface, NULL, pDstSurface, NULL, D3DTEXF_NONE);
	}

//	RwRect		stRect = {700, 0, 256, 256};
	RwRect		stRect = {0, 0, 1024, 768};	

	RwCameraEndUpdate(pThis->m_pCamera);

	RwCameraSetRaster(pThis->m_pCamera, pThis->m_pRaster2);

    RwCameraClear(pThis->m_pCamera, &rgbBackgroundColor, rwCAMERACLEARIMAGE);
			RwReal		Recip_Z;

	if (RwCameraBeginUpdate(pThis->m_pCamera))
	{
		{
			RwV3d		CamPos;
			RwMatrix *	pCamMatrix = RwFrameGetMatrix(RwCameraGetFrame(pThis->m_pCamera));
			RwV3d		WPos = *RwMatrixGetPos(pCamMatrix);
			RwV3d		ScrPos;

			WPos.x += pCamMatrix->at.x * 30000;
			WPos.y += pCamMatrix->at.y * 30000;
			WPos.z += pCamMatrix->at.z * 30000;

			g_pEngine->GetWorldPosToScreenPos(&WPos,&CamPos,&ScrPos,&Recip_Z);

			UINT32	red = 255;
			UINT32	green = 255;
			UINT32	blue = 255;

			RwIm2DVertex	BoxFan[4];

			INT32	i;

			for(i=0;i<4;++i)
				RwIm2DVertexSetIntRGBA(&BoxFan[i], red, green, blue, 255);

			for(i=0;i<4;++i)
			{
				RwIm2DVertexSetCameraX(&BoxFan[i],CamPos.x);
				RwIm2DVertexSetCameraY(&BoxFan[i],CamPos.y);
				RwIm2DVertexSetCameraZ(&BoxFan[i],CamPos.z);

				RwIm2DVertexSetRecipCameraZ(&BoxFan[i], Recip_Z);
				RwIm2DVertexSetScreenZ(&BoxFan[i],ScrPos.z);
			}

			RwIm2DVertexSetScreenX(&BoxFan[0],0);
			RwIm2DVertexSetScreenY(&BoxFan[0],0); 
			RwIm2DVertexSetU(&BoxFan[0], 0, Recip_Z);
			RwIm2DVertexSetV(&BoxFan[0], 0, Recip_Z);
			
			
			RwIm2DVertexSetScreenX(&BoxFan[1],1024);
			RwIm2DVertexSetScreenY(&BoxFan[1],0); 
			RwIm2DVertexSetU(&BoxFan[1], 1, Recip_Z);
			RwIm2DVertexSetV(&BoxFan[1], 0, Recip_Z);
			
			RwIm2DVertexSetScreenX(&BoxFan[2],1024);
			RwIm2DVertexSetScreenY(&BoxFan[2],768); 
			RwIm2DVertexSetU(&BoxFan[2], 1 , Recip_Z);
			RwIm2DVertexSetV(&BoxFan[2], 1 , Recip_Z);
			
			RwIm2DVertexSetScreenX(&BoxFan[3],0);
			RwIm2DVertexSetScreenY(&BoxFan[3],768); 
			RwIm2DVertexSetU(&BoxFan[3], 0, Recip_Z);
			RwIm2DVertexSetV(&BoxFan[3], 1, Recip_Z);

			RwRenderStateSet(rwRENDERSTATESRCBLEND,			(void *) rwBLENDSRCALPHA  );
			RwRenderStateSet(rwRENDERSTATEDESTBLEND,		(void *) rwBLENDINVSRCALPHA   );
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER,	(void *) pThis->m_pRaster);
			RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,		(void *) FALSE);
			RwRenderStateSet(rwRENDERSTATEZTESTENABLE,		(void *) TRUE);
			RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,	(void *) rwFILTERLINEAR);
			RwRenderStateSet(rwRENDERSTATEFOGENABLE,		(void *) FALSE);
			RwD3D9SetRenderState(D3DRS_ZFUNC,				D3DCMP_LESS);

			RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, BoxFan, 4);
		}

		RwCameraEndUpdate(pThis->m_pCamera);
	}

	//pThis->m_csBlur.MakeBlur(pThis->m_pRaster2);

//	((LPDIRECT3DDEVICE9) RwD3D9GetCurrentD3DDevice())->SetDepthStencilSurface(NULL);
	//raster = RwRasterRenderScaled(m_pRaster, &stRect);

	RwCameraSetRaster(pThis->m_pCamera, pRaster);
    //RwCameraClear(pThis->m_pCamera, &rgbBackgroundColor, rwCAMERACLEARIMAGE);

	RwRasterPushContext(pRaster);
	RwRasterRender(pThis->m_pRaster2, 0, 0);
	RwRasterPopContext();

	RwCameraBeginUpdate(pThis->m_pCamera);
	/*
	if (RwCameraBeginUpdate(pThis->m_pCamera))
	{
		{
			RwV3d		CamPos;
			RwMatrix *	pCamMatrix = RwFrameGetMatrix(RwCameraGetFrame(pThis->m_pCamera));
			RwV3d		WPos = *RwMatrixGetPos(pCamMatrix);
			RwV3d		ScrPos;
			RwReal		Recip_Z;

			WPos.x += pCamMatrix->at.x * 50000;
			WPos.y += pCamMatrix->at.y * 50000;
			WPos.z += pCamMatrix->at.z * 50000;

			g_pEngine->GetWorldPosToScreenPos(&WPos,&CamPos,&ScrPos,&Recip_Z);

			UINT32	red = 255;
			UINT32	green = 255;
			UINT32	blue = 255;

			RwIm2DVertex	BoxFan[4];

			for(int i=0;i<4;++i)
				RwIm2DVertexSetIntRGBA(&BoxFan[i], red, green, blue, 255);

			for(i=0;i<4;++i)
			{
				RwIm2DVertexSetCameraX(&BoxFan[i],CamPos.x);
				RwIm2DVertexSetCameraY(&BoxFan[i],CamPos.y);
				RwIm2DVertexSetCameraZ(&BoxFan[i],CamPos.z);

				RwIm2DVertexSetRecipCameraZ(&BoxFan[i], Recip_Z);
				RwIm2DVertexSetScreenZ(&BoxFan[i],ScrPos.z);
			}

			RwIm2DVertexSetScreenX(&BoxFan[0],stRect.x);
			RwIm2DVertexSetScreenY(&BoxFan[0],stRect.y); 
			RwIm2DVertexSetU(&BoxFan[0], 0, Recip_Z);
			RwIm2DVertexSetV(&BoxFan[0], 0, Recip_Z);
			
			
			RwIm2DVertexSetScreenX(&BoxFan[1],stRect.x + stRect.w);
			RwIm2DVertexSetScreenY(&BoxFan[1],stRect.y); 
			RwIm2DVertexSetU(&BoxFan[1], 1, Recip_Z);
			RwIm2DVertexSetV(&BoxFan[1], 0, Recip_Z);
			
			RwIm2DVertexSetScreenX(&BoxFan[2],stRect.x + stRect.w);
			RwIm2DVertexSetScreenY(&BoxFan[2],stRect.y + stRect.h); 
			RwIm2DVertexSetU(&BoxFan[2], 1 , Recip_Z);
			RwIm2DVertexSetV(&BoxFan[2], 1 , Recip_Z);
			
			RwIm2DVertexSetScreenX(&BoxFan[3],stRect.x);
			RwIm2DVertexSetScreenY(&BoxFan[3],stRect.y + stRect.h); 
			RwIm2DVertexSetU(&BoxFan[3], 0, Recip_Z);
			RwIm2DVertexSetV(&BoxFan[3], 1, Recip_Z);

			RwRenderStateSet(rwRENDERSTATESRCBLEND,			(void *) rwBLENDSRCALPHA  );
			RwRenderStateSet(rwRENDERSTATEDESTBLEND,		(void *) rwBLENDINVSRCALPHA   );
			RwRenderStateSet(rwRENDERSTATETEXTURERASTER,	(void *) pThis->m_pRaster2);
			RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,		(void *) FALSE);
			RwRenderStateSet(rwRENDERSTATEZTESTENABLE,		(void *) FALSE);
			RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,	(void *) rwFILTERLINEAR);
//			RwRenderStateSet(rwRENDERSTATEFOGENABLE,		(void *) FALSE);
//			RwD3D9SetRenderState(D3DRS_ZFUNC,				D3DCMP_GREATER);

			RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, BoxFan, 4);
		}
	}
	*/

	return TRUE;
}