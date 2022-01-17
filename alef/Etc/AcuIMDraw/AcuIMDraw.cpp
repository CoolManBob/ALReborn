#include "AcuIMDraw.h"

//@{ kday 20050304
namespace AXISVIEW
{
	std::list<PRIVATE::Axis> g_axisContainer;
};
//@} kday
RwCamera*	AcuIMDraw::m_pCamera = NULL;

AcuIMDraw::AcuIMDraw()
{
}

AcuIMDraw::~AcuIMDraw()
{
}

void 	AcuIMDraw::DrawLine(RwV3d*	p1,RwV3d* p2,DWORD color)
{
	RwIm3DVertex LineList[2];
	RwRGBA	lColor;
	memcpy(&lColor , &color, 4);
	
	RwIm3DVertexSetRGBA(&LineList[0], lColor.red, lColor.green, lColor.blue, lColor.alpha);
	RwIm3DVertexSetRGBA(&LineList[1], lColor.red, lColor.green, lColor.blue, lColor.alpha);

	RwIm3DVertexSetPos(&LineList[0], p1->x, p1->y, p1->z);
	RwIm3DVertexSetPos(&LineList[1], p2->x, p2->y, p2->z);

	RwRenderStateSet( rwRENDERSTATETEXTURERASTER , (void*) NULL);

	if (RwIm3DTransform(LineList, 2, NULL, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA))
	{
		RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
		RwIm3DEnd();
	}
}

void 	AcuIMDraw::DrawLines(RwV3d*	p1,INT32 pNum,DWORD color)		// vertex array를 넘긴다.
{
	RwIm3DVertex LineList[200];
	RwRGBA	lColor;
	memcpy(&lColor , &color, 4);

	int		lindex = 0;
	for(int i=0;i<pNum-1;++i)
	{
		RwIm3DVertexSetPos(&LineList[lindex], p1[i].x, p1[i].y, p1[i].z);
		RwIm3DVertexSetPos(&LineList[lindex+1], p1[i+1].x, p1[i+1].y, p1[i+1].z);

		RwIm3DVertexSetRGBA(&LineList[lindex], lColor.red, lColor.green, lColor.blue, lColor.alpha);
		RwIm3DVertexSetRGBA(&LineList[lindex+1], lColor.red, lColor.green, lColor.blue, lColor.alpha);

		lindex += 2;
	}

	RwRenderStateSet( rwRENDERSTATETEXTURERASTER , (void*) NULL);

	if (RwIm3DTransform(LineList, lindex, NULL, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA))
	{
		RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
		RwIm3DEnd();
	}
}

void	AcuIMDraw::DrawBox(RwBBox*	pBox,DWORD color)
{
	RwIm3DVertex LineList[24];	// 총 12개의 선이 필요
	RwRGBA	lColor;
	memcpy(&lColor , &color, 4);

	for(int i=0;i<24;++i)
	{
		RwIm3DVertexSetRGBA(&LineList[i], lColor.red, lColor.green, lColor.blue, lColor.alpha);
	}

	// bottom box
	RwIm3DVertexSetPos(&LineList[0], pBox->inf.x, pBox->inf.y, pBox->inf.z);
	RwIm3DVertexSetPos(&LineList[1], pBox->sup.x, pBox->inf.y, pBox->inf.z);

	RwIm3DVertexSetPos(&LineList[2], pBox->sup.x, pBox->inf.y, pBox->inf.z);
	RwIm3DVertexSetPos(&LineList[3], pBox->sup.x, pBox->inf.y, pBox->sup.z);

	RwIm3DVertexSetPos(&LineList[4], pBox->sup.x, pBox->inf.y, pBox->sup.z);
	RwIm3DVertexSetPos(&LineList[5], pBox->inf.x, pBox->inf.y, pBox->sup.z);

	RwIm3DVertexSetPos(&LineList[6], pBox->inf.x, pBox->inf.y, pBox->sup.z);
	RwIm3DVertexSetPos(&LineList[7], pBox->inf.x, pBox->inf.y, pBox->inf.z);

	// top box
	RwIm3DVertexSetPos(&LineList[8], pBox->inf.x, pBox->sup.y, pBox->inf.z);
	RwIm3DVertexSetPos(&LineList[9], pBox->sup.x, pBox->sup.y, pBox->inf.z);

	RwIm3DVertexSetPos(&LineList[10], pBox->sup.x, pBox->sup.y, pBox->inf.z);
	RwIm3DVertexSetPos(&LineList[11], pBox->sup.x, pBox->sup.y, pBox->sup.z);

	RwIm3DVertexSetPos(&LineList[12], pBox->sup.x, pBox->sup.y, pBox->sup.z);
	RwIm3DVertexSetPos(&LineList[13], pBox->inf.x, pBox->sup.y, pBox->sup.z);

	RwIm3DVertexSetPos(&LineList[14], pBox->inf.x, pBox->sup.y, pBox->sup.z);
	RwIm3DVertexSetPos(&LineList[15], pBox->inf.x, pBox->sup.y, pBox->inf.z);

	// vertical line
	RwIm3DVertexSetPos(&LineList[16], pBox->inf.x, pBox->inf.y, pBox->inf.z);
	RwIm3DVertexSetPos(&LineList[17], pBox->inf.x, pBox->sup.y, pBox->inf.z);

	RwIm3DVertexSetPos(&LineList[18], pBox->sup.x, pBox->inf.y, pBox->inf.z);
	RwIm3DVertexSetPos(&LineList[19], pBox->sup.x, pBox->sup.y, pBox->inf.z);

	RwIm3DVertexSetPos(&LineList[20], pBox->inf.x, pBox->inf.y, pBox->sup.z);
	RwIm3DVertexSetPos(&LineList[21], pBox->inf.x, pBox->sup.y, pBox->sup.z);

	RwIm3DVertexSetPos(&LineList[22], pBox->sup.x, pBox->inf.y, pBox->sup.z);
	RwIm3DVertexSetPos(&LineList[23], pBox->sup.x, pBox->sup.y, pBox->sup.z);

	RwRenderStateSet( rwRENDERSTATETEXTURERASTER , (void*) NULL);

	if (RwIm3DTransform(LineList, 24, NULL, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA))
	{
		RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
		RwIm3DEnd();
	}
}

void	AcuIMDraw::DrawSphere(RwSphere*	pSphere,DWORD color)
{
	int i,j;

	RwIm3DVertex	LineList[144];	
	RwRGBA	lColor;
	memcpy(&lColor , &color, 4);

	RwV3d			orgVec = {0.0f,1.0f,0.0f};
	RwV3dScale(&orgVec,&orgVec,pSphere->radius);

	RwV3d			xAxis = {1.0f,0.0f,0.0f};
	RwV3d			yAxis = {0.0f,1.0f,0.0f};
	RwMatrix		matY30,matX30;
	
	RwMatrixRotate(&matY30,&yAxis,30.0f,rwCOMBINEREPLACE);
	RwMatrixRotate(&matX30,&xAxis,30.0f,rwCOMBINEREPLACE);

	RwV3d			calcPos[7][13];	// 30' 단위로 회전해서 얻어진 positions
	for(i=0;i<7;++i)
	{
		for(j=0;j<12;++j)
		{
			calcPos[i][j] = orgVec;
			RwV3dTransformPoint(&orgVec,&orgVec,&matY30);
		}

		calcPos[i][12] = calcPos[i][0];

		RwV3dTransformPoint(&orgVec,&orgVec,&matX30);
	}

	for(i=0;i<144;++i)
	{
		RwIm3DVertexSetRGBA(&LineList[i], lColor.red, lColor.green, lColor.blue, lColor.alpha);
	}

	// 가로 선 = 12 * 2 * 5(세로수) = 120, 세로선 = 12 * 2 * 6 = 144
	// 먼저 가로선부터 그리자구요 ^^
	INT32	lIndex = 0;
	for(i=1;i<6;++i)	// 맨위와 맨아래점은 안그린다.
	{
		for(j=0;j<12;++j)
		{
			RwIm3DVertexSetPos(&LineList[lIndex], calcPos[i][j].x,calcPos[i][j].y,calcPos[i][j].z);
			RwIm3DVertexSetPos(&LineList[lIndex+1], calcPos[i][j+1].x,calcPos[i][j+1].y,calcPos[i][j+1].z);

			lIndex += 2;
		}
	}

	if (RwIm3DTransform(LineList, lIndex, NULL, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA))
	{
		RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
		RwIm3DEnd();
	}

	// 세로선 그리자구요 ^^
	lIndex = 0;

	for(j=0;j<12;++j)
	{
		for(i=0;i<6;++i)
		{
			RwIm3DVertexSetPos(&LineList[lIndex], calcPos[i][j].x,calcPos[i][j].y,calcPos[i][j].z);
			RwIm3DVertexSetPos(&LineList[lIndex+1], calcPos[i+1][j].x,calcPos[i+1][j].y,calcPos[i+1][j].z);

			lIndex += 2;
		}
	}

	RwRenderStateSet( rwRENDERSTATETEXTURERASTER , (void*) NULL);

	if (RwIm3DTransform(LineList, lIndex, NULL, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA))
	{
		RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);
		RwIm3DEnd();
	}
}

void	AcuIMDraw::Draw2DLine(RwV2d*	p1,RwV2d*	p2,DWORD color)
{
	RwIm2DVertex		LineList[2];

	RwRGBA	lColor;
	memcpy(&lColor , &color, 4);

	float fScreenZ = RwIm2DGetNearScreenZ();
	float fRecipZ = 1.0f / RwCameraGetNearClipPlane(m_pCamera);

	RwInt32 i;   
    
    for(i=0; i<2; i++)
    {
		RwIm2DVertexSetScreenZ(&LineList[i], fScreenZ);
		RwIm2DVertexSetRecipCameraZ(&LineList[i], fRecipZ);

		RwIm2DVertexSetIntRGBA(&LineList[i], lColor.red, lColor.green, lColor.blue, lColor.alpha);
	}

	RwIm2DVertexSetScreenX(&LineList[0], p1->x);
	RwIm2DVertexSetScreenY(&LineList[0], p1->y);
	RwIm2DVertexSetScreenX(&LineList[1], p2->x);
	RwIm2DVertexSetScreenY(&LineList[1], p2->y);

	RwIm2DVertexSetU(&LineList[0],0.0f,fRecipZ);
	RwIm2DVertexSetV(&LineList[0],0.0f,fRecipZ);

	RwIm2DVertexSetU(&LineList[1],1.0f,fRecipZ);
	RwIm2DVertexSetV(&LineList[1],1.0f,fRecipZ);
      
	RwRenderStateSet( rwRENDERSTATETEXTURERASTER , (void*) NULL);

	RwIm2DRenderPrimitive(rwPRIMTYPELINELIST, LineList, 2);
}

void	AcuIMDraw::SetWorldCamera(RwCamera*	pCam)
{
	m_pCamera = pCam;
}

#include "AgcuVtxType.h"
RwInt32 AcuIMDraw::RenderLine( const RwV3d& v1, const RwV3d& v2, RwUInt32 colr )
{
	USING_AGCUVTX;

	VTX_PD	vb[2];
	vb[0].pos = *reinterpret_cast<const D3DXVECTOR3*>(&v1);
	vb[0].diff = colr;
	vb[1].pos = *reinterpret_cast<const D3DXVECTOR3*>(&v2);
	vb[1].diff = colr;

	
	BOOL	lighting = FALSE;
	RwD3D9GetRenderState(D3DRS_LIGHTING, (void*)&lighting);
	RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);
	RwD3D9SetTexture( NULL, 0LU );
	
	RwMatrix	mat;
	RwMatrixSetIdentity(&mat);
	RwD3D9SetTransformWorld( &mat );
	RwD3D9SetFVF( VTX_PD::FVF );

	
	RwD3D9DrawPrimitiveUP( D3DPT_LINELIST
		, 1LU
		, vb
		, VTX_PD::SIZE
		);

	RwD3D9SetRenderState(D3DRS_LIGHTING, lighting);

	return 0;
};

#include <vector>
RwInt32 AcuIMDraw::RenderLineStrip( const RwV3d varr[], RwInt32 num, RwUInt32 colr )
{
	USING_AGCUVTX;

	std::vector<VTX_PD>	vb;
	VTX_PD	tmp;
	tmp.diff = colr;

	for( int i=0; i<num; ++i )
	{
		tmp.pos = *reinterpret_cast<const D3DXVECTOR3*>(varr++);
		vb.push_back(tmp);
	};

	BOOL	lighting = FALSE;
	RwD3D9GetRenderState(D3DRS_LIGHTING, (void*)&lighting);
	RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);
	RwD3D9SetTexture( NULL, 0LU );
	
	RwMatrix	mat;
	RwMatrixSetIdentity(&mat);
	RwD3D9SetTransformWorld( &mat );
	RwD3D9SetFVF( VTX_PD::FVF );

	
	RwD3D9DrawPrimitiveUP( D3DPT_LINESTRIP
		, num-1
		, &vb.front()
		, VTX_PD::SIZE
		);

	RwD3D9SetRenderState(D3DRS_LIGHTING, lighting);

	return 0;
};