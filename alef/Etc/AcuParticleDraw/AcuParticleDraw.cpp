#include "AcuParticleDraw.h"

VERTEX_PARTICLE*	AcuParticleDraw::m_pBuffer = NULL;
VERTEX_PARTICLE_PSIZE*	AcuParticleDraw::m_pBuffer_PSize = NULL;

INT16*	AcuParticleDraw::m_pIndexBuffer = NULL;

INT32	AcuParticleDraw::m_iMaxBufferVerticeIndex = 0;
INT32	AcuParticleDraw::m_iCurBufferVerticeIndex = 0;
INT32	AcuParticleDraw::m_iCurBuffer = 0;

INT32	AcuParticleDraw::m_iMaxBufferPSize = 0;
INT32	AcuParticleDraw::m_iCurBufferPSize = 0;
INT32	AcuParticleDraw::m_iCountPSize[64];
INT32	AcuParticleDraw::m_iMaxPSize = 0;

FLOAT	AcuParticleDraw::m_fWeightPSize[64];

FLOAT	AcuParticleDraw::m_fMaxPointSize = 0.0f;
RwCamera*	AcuParticleDraw::m_pCamera = NULL;
IDirect3DDevice9*	AcuParticleDraw::m_pCurD3D9Device = NULL;

BOOL	AcuParticleDraw::m_bEnablePSize = FALSE;

RwTexture*	AcuParticleDraw::m_pDrawTexture = NULL;
ParticleBillboardType	 AcuParticleDraw::m_eBillboardType = PARTICLE_BILLBOARD;

RwMatrix	AcuParticleDraw::m_matBillboard; 
RwMatrix	AcuParticleDraw::m_matBillboardY;

D3DMATRIX	AcuParticleDraw::m_matD3DIdentity;

AcuParticleDraw::AcuParticleDraw()
{

}

AcuParticleDraw::~AcuParticleDraw()
{

}

void	AcuParticleDraw::Release()
{
	if(m_pBuffer)
	{
		delete	[]m_pBuffer;
		m_pBuffer = NULL;
	}

	if(m_pIndexBuffer)
	{
		delete []m_pIndexBuffer;
		m_pIndexBuffer = NULL;
	}	

	if(m_pBuffer_PSize)
	{
		delete []m_pBuffer_PSize;
		m_pBuffer_PSize = NULL;
	}
}

void	AcuParticleDraw::Init()
{
	// Hardware Caps Check
	LPDIRECT3DDEVICE9	pD3D9Device = (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice();

	D3DCAPS9			caps;
	pD3D9Device->GetDeviceCaps(&caps);

	m_pCurD3D9Device = pD3D9Device;
	m_fMaxPointSize = caps.MaxPointSize;

	m_iMaxBufferVerticeIndex = 12000;
	INT32				iBufferSize = (m_iMaxBufferVerticeIndex/6)*4;
	m_pBuffer = new	VERTEX_PARTICLE[iBufferSize];
	m_pIndexBuffer = new INT16[m_iMaxBufferVerticeIndex];

	// UV값 미리 세팅
	int i;
	for(i=0;i<iBufferSize;i+=4)
	{
		m_pBuffer[i].u = 1.0f;			m_pBuffer[i].v = 1.0f;
		m_pBuffer[i+1].u = 1.0f;		m_pBuffer[i+1].v = 0.0f;
		m_pBuffer[i+2].u = 0.0f;		m_pBuffer[i+2].v = 0.0f;
		m_pBuffer[i+3].u = 0.0f;		m_pBuffer[i+3].v = 1.0f;
	}

	INT32				iBufferIndex = 0;
	for(i=0;i<m_iMaxBufferVerticeIndex;i+=6)
	{
		m_pIndexBuffer[i] = iBufferIndex;
		m_pIndexBuffer[i+1] = iBufferIndex + 1;
		m_pIndexBuffer[i+2] = iBufferIndex + 2;
		m_pIndexBuffer[i+3] = iBufferIndex + 2;
		m_pIndexBuffer[i+4] = iBufferIndex + 3;
		m_pIndexBuffer[i+5] = iBufferIndex;

		iBufferIndex += 4;
	}

	if(m_fMaxPointSize > 8.0f)
	{
		m_bEnablePSize = TRUE;
		m_iMaxBufferPSize = 100;
		m_pBuffer_PSize = new VERTEX_PARTICLE_PSIZE[m_iMaxBufferPSize];
	}

	FLOAT	fVal = 1.0f;
	RwD3D9SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&fVal));
	RwD3D9SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&m_fMaxPointSize));

	fVal = 0.0f;
	RwD3D9SetRenderState(D3DRS_POINTSCALE_A,  *((DWORD*)&fVal));
	fVal = 0.0f;
	RwD3D9SetRenderState(D3DRS_POINTSCALE_B,  *((DWORD*)&fVal));
	fVal = 1.0f;
	RwD3D9SetRenderState(D3DRS_POINTSCALE_C,  *((DWORD*)&fVal));

	m_matD3DIdentity._11 = 1.0f;
	m_matD3DIdentity._12 = 0.0f;
	m_matD3DIdentity._13 = 0.0f;
	m_matD3DIdentity._14 = 0.0f;
	m_matD3DIdentity._21 = 0.0f;
	m_matD3DIdentity._22 = 1.0f;
	m_matD3DIdentity._23 = 0.0f;
	m_matD3DIdentity._24 = 0.0f;
	m_matD3DIdentity._31 = 0.0f;
	m_matD3DIdentity._32 = 0.0f;
	m_matD3DIdentity._33 = 1.0f;
	m_matD3DIdentity._34 = 0.0f;
	m_matD3DIdentity._41 = 0.0f;
	m_matD3DIdentity._42 = 0.0f;
	m_matD3DIdentity._43 = 0.0f;
	m_matD3DIdentity._44 = 1.0f;

	m_fWeightPSize[0] = 0.8f;
	m_fWeightPSize[1] = 0.6f;
	m_fWeightPSize[2] = 0.3f;
	m_fWeightPSize[3] = 0.125f;
	m_fWeightPSize[4] = 0.09f;
	m_fWeightPSize[5] = 0.06f;
	m_fWeightPSize[6] = 0.04f;
	m_fWeightPSize[7] = 0.03f;
	m_fWeightPSize[8] = 0.025f;
	m_fWeightPSize[9] = 0.021f;
	m_fWeightPSize[10] = 0.018f;
	m_fWeightPSize[11] = 0.015f;
	m_fWeightPSize[12] = 0.012f;
	m_fWeightPSize[13] = 0.01f;
	m_fWeightPSize[14] = 0.0091f;
	m_fWeightPSize[15] = 0.0085f;
	m_fWeightPSize[16] = 0.0079f;
	m_fWeightPSize[17] = 0.0072f;
	m_fWeightPSize[18] = 0.0065f;
	m_fWeightPSize[19] = 0.0059f;
	m_fWeightPSize[20] = 0.0054f;
	m_fWeightPSize[21] = 0.0049f;
	m_fWeightPSize[22] = 0.0044f;
	m_fWeightPSize[23] = 0.004f;
	m_fWeightPSize[24] = 0.0035f;
	m_fWeightPSize[25] = 0.0031f;
	m_fWeightPSize[26] = 0.0027f;
	m_fWeightPSize[27] = 0.0024f;
	m_fWeightPSize[28] = 0.0021f;
	m_fWeightPSize[29] = 0.0019f;
	m_fWeightPSize[30] = 0.0017f;
	m_fWeightPSize[31] = 0.0015f;
	m_fWeightPSize[32] = 0.0013f;
	m_fWeightPSize[33] = 0.0012f;
	m_fWeightPSize[34] = 0.00111f;
	m_fWeightPSize[35] = 0.00102f;
	m_fWeightPSize[36] = 0.00093f;
	m_fWeightPSize[37] = 0.00085f;
	m_fWeightPSize[38] = 0.00077f;
	m_fWeightPSize[39] = 0.0007f;
	m_fWeightPSize[40] = 0.00063f;
	m_fWeightPSize[41] = 0.00058f;
	m_fWeightPSize[42] = 0.00054f;
	m_fWeightPSize[43] = 0.000495f;
	m_fWeightPSize[44] = 0.00046f;
	m_fWeightPSize[45] = 0.00043f;
	m_fWeightPSize[46] = 0.0004f;
	m_fWeightPSize[47] = 0.000375f;
	m_fWeightPSize[48] = 0.00035f;
	m_fWeightPSize[49] = 0.00033f;
	m_fWeightPSize[50] = 0.00031f;
	m_fWeightPSize[51] = 0.00029f;
	m_fWeightPSize[52] = 0.000275f;
	m_fWeightPSize[53] = 0.00026f;
	m_fWeightPSize[54] = 0.00025f;
	m_fWeightPSize[55] = 0.00024f;
	m_fWeightPSize[56] = 0.00023f;
	m_fWeightPSize[57] = 0.000221f;
	m_fWeightPSize[58] = 0.000212f;
	m_fWeightPSize[59] = 0.000204f;
	m_fWeightPSize[60] = 0.000196f;
	m_fWeightPSize[61] = 0.000189f;
	m_fWeightPSize[62] = 0.000183f;
	m_fWeightPSize[63] = 0.000178f;
}

void	AcuParticleDraw::CameraMatrixUpdate()
{
	if(!m_pCamera) return;

	RwMatrix*	pCamLTM	= RwFrameGetLTM( RwCameraGetFrame( m_pCamera ) );
	const	RwV3d		yAxis = {0.0f,1.0f,0.0f};

	RwMatrixTransform( &m_matBillboard, pCamLTM, rwCOMBINEREPLACE );
	RwV3dScale( &m_matBillboard.right, &pCamLTM->right, -1.0f );
	RwV3dScale( &m_matBillboard.at	 , &pCamLTM->at	  , -1.0f );
	m_matBillboard.pos.x = 	m_matBillboard.pos.y = 	m_matBillboard.pos.z = 0.0f;

	RwMatrixTransform( &m_matBillboardY, pCamLTM, rwCOMBINEREPLACE );
	m_matBillboardY.up	= yAxis;
	RwV3dCrossProduct( &m_matBillboardY.at, &m_matBillboardY.right, &m_matBillboardY.up );		   
	m_matBillboardY.pos.x = m_matBillboardY.pos.y = m_matBillboardY.pos.z = 0.0f;
}

void	AcuParticleDraw::DrawStart(RwTexture*	pTexture,ParticleBillboardType	eBillType)
{
	m_pDrawTexture = pTexture;
	m_eBillboardType = eBillType;

	m_iCurBufferVerticeIndex = 0;
	m_iCurBuffer = 0;
}

void	AcuParticleDraw::AccumulateBuffer(RwV3d*	pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight)
{
	if(m_iCurBufferVerticeIndex >= m_iMaxBufferVerticeIndex)
	{
		DrawEnd();
		m_iCurBufferVerticeIndex = 0;
		m_iCurBuffer = 0;
	}

	RwMatrix*		pCamBillMatrix;
	if(m_eBillboardType == PARTICLE_BILLBOARD)
	{
		pCamBillMatrix = &m_matBillboard;
	}
	else if(m_eBillboardType == PARTICLE_YBILLBOARD)
	{
		pCamBillMatrix = &m_matBillboardY;
	}

	FLOAT	hWidth = fWidth * 0.5f;
	FLOAT	hHeight = fHeight * 0.5f;

	RwV3d	box[4];
	box[0].x = -hWidth;		box[0].y = -hHeight;		box[0].z = 0.0f;
	box[1].x = hWidth;		box[1].y = -hHeight;		box[1].z = 0.0f;
	box[2].x = -hWidth;		box[2].y = hHeight;			box[2].z = 0.0f;
	box[3].x = hWidth;		box[3].y = hHeight;			box[3].z = 0.0f;

	for(int i=0;i<4;++i)
	{
		RwV3dTransformPoint( &box[i] , &box[i] , pCamBillMatrix);
		RwV3dAdd( &box[i], &box[i], pPos );
	}

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[3].x;
	m_pBuffer[m_iCurBuffer].position.y = box[3].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[3].z;		// m_iCurBuffer 증가

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[1].x;
	m_pBuffer[m_iCurBuffer].position.y = box[1].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[1].z;		// m_iCurBuffer 증가
	
	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[0].x;
	m_pBuffer[m_iCurBuffer].position.y = box[0].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[0].z;		// m_iCurBuffer 증가

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[2].x;
	m_pBuffer[m_iCurBuffer].position.y = box[2].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[2].z;		// m_iCurBuffer 증가

	m_iCurBufferVerticeIndex += 6;
}

// 나중에 코딩
void	AcuParticleDraw::AccumulateBufferRotationSelf(RwV3d*	pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight,FLOAT	rot_val)
{
	if(m_iCurBufferVerticeIndex >= m_iMaxBufferVerticeIndex)
	{
		DrawEnd();
		m_iCurBufferVerticeIndex = 0;
		m_iCurBuffer = 0;
	}

	RwMatrix*		pCamBillMatrix;
	if(m_eBillboardType == PARTICLE_BILLBOARD)
	{
		pCamBillMatrix = &m_matBillboard;
	}
	else if(m_eBillboardType == PARTICLE_YBILLBOARD)
	{
		pCamBillMatrix = &m_matBillboardY;
	}

	FLOAT	hWidth = fWidth * 0.5f;
	FLOAT	hHeight = fHeight * 0.5f;

	RwV3d	box[4];
	box[0].x = -hWidth	+ rot_val+ rot_val;		box[0].y = -hHeight;		box[0].z = 0.0f;
	box[1].x = hWidth	+ rot_val+ rot_val;		box[1].y = -hHeight;		box[1].z = 0.0f;
	box[2].x = -hWidth;		box[2].y = hHeight;			box[2].z = 0.0f;
	box[3].x = hWidth;		box[3].y = hHeight;			box[3].z = 0.0f;

	for(int i=0;i<4;++i)
	{
		RwV3dTransformPoint( &box[i] , &box[i] , pCamBillMatrix);
		RwV3dAdd( &box[i], &box[i], pPos );
	}

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[3].x;
	m_pBuffer[m_iCurBuffer].position.y = box[3].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[3].z;		// m_iCurBuffer 증가

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[1].x;
	m_pBuffer[m_iCurBuffer].position.y = box[1].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[1].z;		// m_iCurBuffer 증가
	
	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[0].x;
	m_pBuffer[m_iCurBuffer].position.y = box[0].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[0].z;		// m_iCurBuffer 증가

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[2].x;
	m_pBuffer[m_iCurBuffer].position.y = box[2].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[2].z;		// m_iCurBuffer 증가

	m_iCurBufferVerticeIndex += 6;
}

void	AcuParticleDraw::AccumulateBufferRotationMatrix(RwV3d*	pPos,DWORD	color,FLOAT	fWidth,FLOAT	fHeight,RwMatrix*	rot_matrix)
{
	if(m_iCurBufferVerticeIndex >= m_iMaxBufferVerticeIndex)
	{
		DrawEnd();
		m_iCurBufferVerticeIndex = 0;
		m_iCurBuffer = 0;
	}

	RwMatrix*		pCamBillMatrix;
	if(m_eBillboardType == PARTICLE_BILLBOARD)
	{
		pCamBillMatrix = &m_matBillboard;
	}
	else if(m_eBillboardType == PARTICLE_YBILLBOARD)
	{
		pCamBillMatrix = &m_matBillboardY;
	}

	FLOAT	hWidth = fWidth * 0.5f;
	FLOAT	hHeight = fHeight * 0.5f;

	RwV3d	box[4];
	box[0].x = -hWidth;		box[0].y = -hHeight;		box[0].z = 0.0f;
	box[1].x = hWidth;		box[1].y = -hHeight;		box[1].z = 0.0f;
	box[2].x = -hWidth;		box[2].y = hHeight;			box[2].z = 0.0f;
	box[3].x = hWidth;		box[3].y = hHeight;			box[3].z = 0.0f;

	for(int i=0;i<4;++i)
	{
		RwV3dTransformPoint( &box[i] , &box[i] , pCamBillMatrix);
		RwV3dAdd( &box[i], &box[i], pPos );
	}

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[3].x;
	m_pBuffer[m_iCurBuffer].position.y = box[3].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[3].z;		// m_iCurBuffer 증가

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[1].x;
	m_pBuffer[m_iCurBuffer].position.y = box[1].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[1].z;		// m_iCurBuffer 증가
	
	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[0].x;
	m_pBuffer[m_iCurBuffer].position.y = box[0].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[0].z;		// m_iCurBuffer 증가

	m_pBuffer[m_iCurBuffer].color = color;
	m_pBuffer[m_iCurBuffer].position.x = box[2].x;
	m_pBuffer[m_iCurBuffer].position.y = box[2].y;
	m_pBuffer[m_iCurBuffer++].position.z = box[2].z;		// m_iCurBuffer 증가

	m_iCurBufferVerticeIndex += 6;
}

void	AcuParticleDraw::DrawEnd()
{
	RwD3D9SetFVF( D3DFVF_PARTICLE );
	RwD3D9SetVertexShader( NULL );
	RwD3D9SetPixelShader( NULL );

	RwD3D9SetTransform( D3DTS_WORLD, &m_matD3DIdentity);

	RwD3D9SetTexture( m_pDrawTexture , 0 );

	RwD3D9DrawIndexedPrimitiveUP ( D3DPT_TRIANGLELIST, 0 , m_iCurBufferVerticeIndex , m_iCurBufferVerticeIndex/3, 
								m_pIndexBuffer, m_pBuffer, sizeof(VERTEX_PARTICLE));
}

void	AcuParticleDraw::DrawStartPoint(RwTexture*	pTexture,INT32	iMaxPointSize)
{
	m_iCurBufferPSize = 0;
	m_pDrawTexture = pTexture;

	memset(m_iCountPSize,0,sizeof(m_iCountPSize));
	m_iMaxPSize = iMaxPointSize;
}

void	AcuParticleDraw::AccumulateBufferPoint(RwV3d*		pPos,DWORD	color,FLOAT	fSize)
{
	if(m_iCurBufferPSize >= m_iMaxBufferPSize)
	{
		DrawEndPoint();
		m_iCurBufferPSize = 0;
		memset(m_iCountPSize,0,sizeof(m_iCountPSize));
	}

	m_pBuffer_PSize[m_iCurBufferPSize].color = color;
	m_pBuffer_PSize[m_iCurBufferPSize].pointsize = fSize;
	m_pBuffer_PSize[m_iCurBufferPSize].position.x = pPos->x;
	m_pBuffer_PSize[m_iCurBufferPSize].position.y = pPos->y;
	m_pBuffer_PSize[m_iCurBufferPSize++].position.z = pPos->z;		// m_iCurBufferPSize 증가

	++ m_iCountPSize[(INT32)fSize];
}


void	AcuParticleDraw::DrawEndPoint()
{
	RwD3D9SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	RwD3D9SetRenderState( D3DRS_POINTSCALEENABLE, TRUE );

	FLOAT	fVal = 1.0f;
	RwD3D9SetRenderState(D3DRS_POINTSIZE_MIN, *((DWORD*)&fVal));
	RwD3D9SetRenderState(D3DRS_POINTSIZE_MAX, *((DWORD*)&m_fMaxPointSize));

	fVal = 0.0f;
	RwD3D9SetRenderState(D3DRS_POINTSCALE_A,  *((DWORD*)&fVal));
	fVal = 0.0f;
	RwD3D9SetRenderState(D3DRS_POINTSCALE_B,  *((DWORD*)&fVal));

	RwD3D9SetFVF( D3DFVF_PARTICLE_PSIZE );
	RwD3D9SetVertexShader( NULL );
	RwD3D9SetPixelShader( NULL );
	RwD3D9SetTexture( m_pDrawTexture , 0 );

	RwD3D9SetTransform( D3DTS_WORLD, &m_matD3DIdentity);

	INT32		draw_index = 0;
	for(int i=0;i<m_iMaxPSize;++i)
	{
		if(m_iCountPSize[i] == 0) continue;

		//fVal = 0.00012f * (64-i);
		RwD3D9SetRenderState(D3DRS_POINTSCALE_C,  *((DWORD*)&m_fWeightPSize[i]));

		RwD3D9DrawPrimitiveUP  ( D3DPT_POINTLIST , m_iCountPSize[i], &m_pBuffer_PSize[draw_index], sizeof(VERTEX_PARTICLE_PSIZE));
		draw_index += m_iCountPSize[i];
	}
	
	RwD3D9SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	RwD3D9SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
}

void	AcuParticleDraw::SetWorldCamera(RwCamera*	pCam)
{
	m_pCamera = pCam;
}
