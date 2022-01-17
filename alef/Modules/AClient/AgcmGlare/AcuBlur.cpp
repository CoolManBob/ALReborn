// AcuBlur.cpp: implementation of the AcuBlur class.
//
//////////////////////////////////////////////////////////////////////

#include "AcuBlur.h"

#include "blurvshaderdefs.h"
#include "blurvshaderD3D9.h"
#include "blurpshaderD3D9.h"

#include "AcuTexture.h"

#define TEXWIDTH	1024
#define TEXHEIGHT	1024

static int blurDir = 0;
static int mostBlurred;

static RwRGBA clearColor = {255, 255, 255, 0};

static D3DXVECTOR3 vEyePt;//	= { 0.0f, 0.0f, -5.0f };
static D3DXVECTOR3 vLookatPt;// = { 0.0f, 0.0f, 0.0f };
static D3DXVECTOR3 vUp;//	   = { 0.0f, 1.0f, 0.0f };

static D3DXVECTOR4 pixelshaderLerp;// = {0.5f, 0.5f, 0.5f, 0.5f};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcuBlur::AcuBlur()
{
	BlurVertexShader = NULL;
	BlurPixelShader = NULL;

	BlurPasses = 10;
	BlurType = 0;

	vEyePt.x = vEyePt.y = 0;
	vEyePt.z = -5;

	vLookatPt.x = vLookatPt.y = vLookatPt.z = 0;

	vUp.x = vUp.z = 0;
	vUp.y = 1;

	pixelshaderLerp.x = pixelshaderLerp.y = pixelshaderLerp.z = pixelshaderLerp.w = 0.5f;

	ZeroMemory(RenderedVertex, sizeof(AcuBlurRenderedVertexFormat) * 4);

	Clump	= NULL;

	ZeroMemory(TexCamera, sizeof(RwCamera *) * NUM_CAMERA_TEXTURES);
	ZeroMemory(CameraTexture, sizeof(RwTexture *) * NUM_CAMERA_TEXTURES);
}

AcuBlur::~AcuBlur()
{

}

/*
 *****************************************************************************
 */
RwCamera *	AcuBlur::CreateTextureCamera(RwBool wantZRaster)
{
	RwRaster *raster;

	raster = RwRasterCreate(TEXWIDTH, TEXHEIGHT, 16, rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888);

	if( raster )
	{
		RwRaster *zRaster = 0;

		if (wantZRaster)
		{
			zRaster = RwRasterCreate(TEXWIDTH, TEXHEIGHT, 0, rwRASTERTYPEZBUFFER);
		}

		if( zRaster || ! wantZRaster)
		{
			RwFrame *frame;

			frame = RwFrameCreate();

			if( frame )
			{
				RwCamera *camera = RwCameraCreate();

				if( camera)
				{
					RwV2d vw;

					RwCameraSetRaster(camera, raster);

					if (zRaster)
					{
						RwCameraSetZRaster(camera, zRaster);
					}

					RwCameraSetFrame(camera, frame);

					RwCameraSetNearClipPlane(camera, 0.1f);
					RwCameraSetFarClipPlane(camera, 250.0f);

					vw.x = 0.6f; vw.y = 0.3375f;
					RwCameraSetViewWindow(camera, &vw);

					return camera;
				}

				RwFrameDestroy(frame);
			}

			RwRasterDestroy(zRaster);
		}

		RwRasterDestroy(raster);
	}

	return 0;
}

RwBool	AcuBlur::PShaderOpen(void)
{
	RwBool  status;
	RwInt32  i;
	const D3DCAPS9	  *d3dCaps;

	for (i=0; i<NUM_CAMERA_TEXTURES; ++i)
	{
		TexCamera[i] = CreateTextureCamera(0);
		if (!TexCamera[i])
		{
			return FALSE;
		}

		CameraTexture[i] = RwTextureCreate(RwCameraGetRaster(TexCamera[i]));
		RwCameraClear(TexCamera[i], &clearColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);
	}

	if (!TexCamera[0])
	{
		return FALSE;
	}

	/*
	 * Create the vertex shader
	 */
	d3dCaps = (const D3DCAPS9 *)RwD3D9GetCaps();
	if ( (d3dCaps->VertexShaderVersion & 0xffff) >= 0x0101)
	{
		status = 
			RwD3D9CreateVertexShader((RwUInt32 *)dwBlurvshaderD3D9VertexShader,
									 &BlurVertexShader);
	
		if (FAILED(status))
		{
			return FALSE;
		}
	}
	else
	{
		BlurVertexShader = NULL;
	}

	/*
	 * Create the pixel shader
	 */
	if ( (d3dCaps->PixelShaderVersion & 0xffff) >= 0x0101)
	{
		RwD3D9CreatePixelShader((RwUInt32 *)dwBlurpshaderD3D9PixelShader, 
								&BlurPixelShader);
	}
	else
	{
		BlurPixelShader = NULL;
	}

	/*
	 *   Fill the vertex array.
	 */
	for (i = 0; i < 4; ++i)
	{
		RenderedVertex[i].x = ((i==0 || i==2) ? -1.0f : 1.0f);
		RenderedVertex[i].y = ((i<2)		  ? -1.0f : 1.0f);
		RenderedVertex[i].z = 0.0f;

		RenderedVertex[i].u = ((i==0 || i==2) ? 0.0f : 1.0f);
		RenderedVertex[i].v = ((i<2)		  ? 1.0f : 0.0f);
	}

	return TRUE;
}

void	AcuBlur::BoxFilterSetup()
{ 
	RwInt32 i;

	RwReal kPerTexelWidth  = 1.0f/(RwReal)TEXWIDTH;
	RwReal kPerTexelHeight = 1.0f/(RwReal)TEXHEIGHT;
	RwReal eps			 = 10.0e-4f;

	/*
	From nVidia's filter blit demo, samples 16 pixels by sampling in the center of 4 pixels
	and letting the bilinear filtering average them together.
	The pixel shader averages the result.
	*/
	RwReal offsetX[4] = { -.5f * kPerTexelWidth + eps,  
						  -.5f * kPerTexelWidth + eps, 
						  1.5f * kPerTexelWidth - eps, 
						  1.5f * kPerTexelWidth - eps };
	RwReal offsetY[4] = { -.5f * kPerTexelHeight+ eps, 
						  1.5f * kPerTexelHeight- eps, 
						  1.5f * kPerTexelHeight- eps, 
						  -.5f * kPerTexelHeight+ eps };

	for (i = 0; i < 4; ++i)
	{
		RwReal stageOffset[4] = { offsetX[i], offsetY[i], 0.0f, 0.0f };
		RwD3D9SetVertexShaderConstant( VSCONST_REG_T0_OFFSET + i * VSCONST_REG_T0_SIZE,
			stageOffset, VSCONST_REG_T0_SIZE );
	}
}

void	AcuBlur::VBoxFilterSetup()
{ 
	RwInt32 i;

	RwReal kPerTexelWidth  = 1.0f/(RwReal)TEXWIDTH;
	RwReal kPerTexelHeight = 1.0f/(RwReal)TEXHEIGHT;
	RwReal eps			 = 10.0e-4f;

	/*
	Adding this fudge factor keeps the texture from crawling across the screen on repeated passes
	*/
	RwReal offsetX[4] = { eps, 
						  eps, 
						  eps, 
						  eps };
	/*
	Sample 8 pixels in a column, sample points are fudged to land in between 2 pixels
	which will be averaged by bilinear filtering, the pixel shader does the rest
	*/
	RwReal offsetY[4] = { -.5f * kPerTexelHeight+ eps, 
						  1.5f * kPerTexelHeight- eps, 
						  3.5f * kPerTexelHeight- eps, 
						  -2.5f * kPerTexelHeight+ eps };


	for (i = 0; i < 4; ++i)
	{
		RwReal stageOffset[4] = { offsetX[i], offsetY[i], 0.0f, 0.0f };
		RwD3D9SetVertexShaderConstant( VSCONST_REG_T0_OFFSET + i * VSCONST_REG_T0_SIZE,
			stageOffset, VSCONST_REG_T0_SIZE );
	}
}

void	AcuBlur::HBoxFilterSetup()
{ 
	RwInt32 i;

	RwReal kPerTexelWidth  = 1.0f/(RwReal)TEXWIDTH;
	RwReal kPerTexelHeight = 1.0f/(RwReal)TEXHEIGHT;
	RwReal eps			 = 10.0e-4f;

	/*
	Sample 8 pixels in a row, sample points are fudged to land in between 2 pixels
	which will be averaged by bilinear filtering, the pixel shader does the rest
	*/
	RwReal offsetX[4] = { -.5f * kPerTexelWidth + eps,  
						  -2.5f * kPerTexelWidth + eps, 
						  1.5f * kPerTexelWidth - eps, 
						  3.5f * kPerTexelWidth - eps };
	/*
	Adding this fudge factor keeps the texture from crawling down the screen on repeated passes
	*/
	RwReal offsetY[4] = { eps, 
						  eps, 
						  eps, 
						  eps };

	for (i = 0; i < 4; ++i)
	{
		RwReal stageOffset[4] = { offsetX[i], offsetY[i], 0.0f, 0.0f };
		RwD3D9SetVertexShaderConstant( VSCONST_REG_T0_OFFSET + i * VSCONST_REG_T0_SIZE,
			stageOffset, VSCONST_REG_T0_SIZE );
	}
}

void	AcuBlur::Blur(int srcTexture)
{
	/*
	 * Render quad with pixel shader operating on input texture.
	 */
	RwInt32 i;
	RwUInt32  cullMode;

	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMATRIX matProj;
	D3DXMATRIX matViewProj;
	D3DXMATRIX matWorldViewProj;

	/* Set World, View, Projection, and combination matrices. */
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
	D3DXMatrixOrthoLH(&matProj, 4.0f, 4.0f, 0.2f, 20.0f);
	D3DXMatrixMultiply(&matViewProj, &matView, &matProj);

	/* draw a single quad to texture: the quad covers the whole "screen" exactly */
	D3DXMatrixScaling(&matWorld, 2.0f, 2.0f, 1.0f);
	D3DXMatrixMultiply(&matWorldViewProj, &matWorld, &matViewProj);
	D3DXMatrixTranspose(&matWorldViewProj, &matWorldViewProj);

	RwD3D9SetVertexShaderConstant(VSCONST_REG_TRANSFORM_OFFSET,
								  &matWorldViewProj,
								  VSCONST_REG_TRANSFORM_SIZE);

	/* Stuff in the right constants to do a box filter */
	if (BlurType == 0)
	{
		/* just the 4x4 box filter */
		BoxFilterSetup();
	}
	else
	{
		/*
		Alternate blurring 8 pixels together in horizontal and vertical directions.
		Gives a more extreme blur in less passes (faster!)
		*/
		if (blurDir & 1)
		{
			HBoxFilterSetup();
		}
		else
		{
			VBoxFilterSetup();
		}

		blurDir++;
	}

	RwD3D9SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

	RwD3D9SetVertexShader((IDirect3DVertexShader9 *) BlurVertexShader);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) FALSE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *) FALSE);

	RwD3D9GetRenderState(D3DRS_CULLMODE, &cullMode);

	RwD3D9SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	/* stick the rendered texture into texture stages 0-3 */
	for (i = 0; i < 4; ++i)
	{
		RwD3D9SetTexture(CameraTexture[srcTexture], i);

		RwD3D9SetSamplerState(i, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP);
		RwD3D9SetSamplerState(i, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP);

		RwD3D9SetSamplerState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		RwD3D9SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		RwD3D9SetSamplerState(i, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	}

	/* turn on the bluring pixel shader */
	RwD3D9SetPixelShader((IDirect3DPixelShader9 *) BlurPixelShader);	  

	RwD3D9SetPixelShaderConstant(0, &pixelshaderLerp, 1);

	/* render the quad, invoking the vertex & pixel shaders */
	RwD3D9DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, RenderedVertex, sizeof(AcuBlurRenderedVertexFormat));

	/* Restore renderstates */
	for (i = 0; i < 4; ++i)
	{
		RwD3D9SetTexture(NULL, i);
	}

	RwD3D9SetRenderState(D3DRS_CULLMODE, cullMode);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *) TRUE);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *) TRUE);

	/* turn off pixel shading */
	RwD3D9SetPixelShader(NULL);	
}

void	AcuBlur::PShaderClose(void)
{
	RwInt32 i;

	if (BlurVertexShader != NULL)
	{
		RwD3D9DeleteVertexShader(BlurVertexShader);
		BlurVertexShader = NULL;
	}

	if (BlurPixelShader != NULL)
	{
		RwD3D9DeletePixelShader(BlurPixelShader);
		BlurPixelShader = NULL;
	}

	for (i=0; i<NUM_CAMERA_TEXTURES; ++i)
	{
		RwRaster *raster;
		RwRaster *zRaster;
		RwFrame  *frame;

		if (TexCamera[i])
		{
			raster = RwCameraGetRaster(TexCamera[i]);
			if (raster)
			{
				RwCameraSetRaster(TexCamera[i], NULL);
				//RwRasterDestroy(raster);
			}

			zRaster = RwCameraGetZRaster(TexCamera[i]);
			if (zRaster)
			{
				RwCameraSetZRaster(TexCamera[i], NULL);
				RwRasterDestroy(zRaster);
			}

			RwTextureDestroy(CameraTexture[i]);
			CameraTexture[i] = NULL;
			
			frame = RwCameraGetFrame(TexCamera[i]);
			if (frame)
			{
				RwCameraSetFrame(TexCamera[i], NULL);
				RwFrameDestroy(frame);
			}

			RwCameraDestroy(TexCamera[i]);
		}
	}
}

void	AcuBlur::MakeBlur(RwRaster *pstRaster)
{
	RwInt32 srcTexture, dstTexture, i;

	RwCameraClear(TexCamera[0], &clearColor, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);
	RwCameraClear(TexCamera[1], &clearColor, rwCAMERACLEARIMAGE);

	RwRasterPushContext(RwCameraGetRaster(TexCamera[0]));
	RwRasterRender(pstRaster, 0, 0);
	RwRasterPopContext();

	blurDir = 0;

	/*
	 * Blur texture repeatedly
	 */
	srcTexture = 0;
	dstTexture = 1;

	for (i=0; i < BlurPasses; ++i)
	{
		 if( RwCameraBeginUpdate(TexCamera[dstTexture]) )
		 {
			Blur( srcTexture );

			RwCameraEndUpdate(TexCamera[dstTexture]);

			mostBlurred = dstTexture;

			++srcTexture;
			if (srcTexture >= NUM_CAMERA_TEXTURES)
			{
				srcTexture = 0;
			}

			++dstTexture;
			if (dstTexture >= NUM_CAMERA_TEXTURES)
			{
				dstTexture = 0;
			}
		 }
	}

	RwRasterPushContext(pstRaster);
	RwRasterRender(RwCameraGetRaster(TexCamera[mostBlurred]), 0, 0);
	RwRasterPopContext();
}
