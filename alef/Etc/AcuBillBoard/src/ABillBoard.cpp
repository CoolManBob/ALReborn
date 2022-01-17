#include "ABillBoard.h"
#include "rtbmp.h"
#include "rttiff.h"
#include "rtpng.h"
#include "rtras.h"

RwV3d BillBoardPT[4] = 
{
    {   -10.0f,  0.0f,   0.0f },
    {   -10.0f,  20.0f,   0.0f },
    {   10.0f,  0.0f,   0.00f },
    {   10.0f,  20.0f,   0.00f }		

};

static RwV3d Yaxis = { 0.0f, 1.0f, 0.0f };
static RwV3d Xaxis = { 1.0f, 0.0f, 0.0f };
static RwV3d Zaxis = { 0.0f, 0.0f, 1.0f };


/*****************************************************************
*   Function : AcuBillBoard()
*   Comment  : 생성자 
*                    
*   Date&Time : 2002-04-11, 오후 2:06
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AcuBillBoard::AcuBillBoard()	
{
	sprintf( m_szTPath, "./Textures/");
	m_eTFormat = TFORMAT_TIF;
	m_v3dOriginalPos.x = 0.0f;
	m_v3dOriginalPos.y = 0.0f;
	m_v3dOriginalPos.z = 0.0f;

	m_TexCoords_U = 1.0f;
	m_TexCoords_V = 1.0f;
}

/*****************************************************************
*   Function : ~AcuBillBoard
*   Comment  : 소멸자 
*                    
*   Date&Time : 2002-04-11, 오후 2:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
AcuBillBoard::~AcuBillBoard()
{

}

/*****************************************************************
*   Function : CreateBBAtomic( RpWorld*	ParentWorld );
*   Comment  : 빌보드를 생성한 후에 Atomic을 돌려준다. 
*			   World에 Add도 여기서 해준다.                  
*   Date&Time : 2002-04-11, 오후 2:34
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
RpAtomic*	AcuBillBoard::CreateBBAtomic( )
{
	//Textrue Directory지정
	RwChar*			BBPath;
	RwChar*		Originpath;
	Originpath = RwImageGetPath();
	BBPath = RsPathnameCreate(m_szTPath);
	RwImageSetPath(BBPath);
	RsPathnameDestroy(BBPath);

	//Texture Format결정 
	switch( m_eTFormat )
	{
	case TFORMAT_BMP:
		RwImageRegisterImageFormat("bmp", RtBMPImageRead, NULL); 
		break;

	case TFORMAT_RAS:
		RwImageRegisterImageFormat("ras", RtRASImageRead  , NULL); 
		break;

	case TFORMAT_PNG:
		RwImageRegisterImageFormat("png", RtPNGImageRead, NULL); 
		break;
	
	case TFORMAT_TIF:
		RwImageRegisterImageFormat("tif", RtTIFFImageRead, NULL); 
		break;
	}	

	//Texture읽기 
	RwTexture*		BBTexture;
	BBTexture = RwTextureRead(m_szTName, NULL);
	RwTextureSetFilterMode(BBTexture, rwFILTERLINEAR);

	//TextureCoords등(Material 지정)
	RpMaterial*		BBMaterial;
	RwTexCoords		BBTexCoords[4];
	RwSurfaceProperties		BBSPro;
	BBMaterial = RpMaterialCreate();
	RpMaterialSetTexture( BBMaterial, BBTexture );
	BBTexCoords[2].u = 0.0f;
	BBTexCoords[2].v = m_TexCoords_V;
	BBTexCoords[3].u = 0.0f;
	BBTexCoords[3].v = 0.0f;
	BBTexCoords[0].u = m_TexCoords_U;
	BBTexCoords[0].v = m_TexCoords_V;
	BBTexCoords[1].u = m_TexCoords_U;
	BBTexCoords[1].v = 0.0f;
	BBSPro.ambient =1.0f;
	BBSPro.diffuse = 1.0f;
	BBSPro.specular = 0.0f;	
	
	RpMaterialSetSurfaceProperties(BBMaterial, &BBSPro);
	
	//Geometry생성 list입력 
	RpGeometry*		BBGeometry;
	RpMorphTarget*	BBMorphTarget;
	RwV3d *BBvlist, *BBnlist;
	RpTriangle*		BBTlist;
	RwTexCoords*	BBTCoord;
	BBGeometry = RpGeometryCreate(4, 2, rpGEOMETRYLIGHT | rpGEOMETRYNORMALS | rpGEOMETRYTEXTURED );
	BBMorphTarget = RpGeometryGetMorphTarget(BBGeometry, 0);
	BBvlist = RpMorphTargetGetVertices(BBMorphTarget);
	BBnlist = RpMorphTargetGetVertexNormals(BBMorphTarget);

			//if ( BBGeometry == NULL )	return NULL;

	BBTlist = RpGeometryGetTriangles(BBGeometry);
	BBTCoord = RpGeometryGetVertexTexCoords(BBGeometry, rwTEXTURECOORDINATEINDEX0);
	
	//Geometry Nomal방향 결정과 포인트 리스트 써넣기  
	RwV3d BBa, BBb, BBnormal;
	RwV3dSub(&BBa, &BillBoardPT[0], &BillBoardPT[1]);
	RwV3dSub(&BBb, &BillBoardPT[0], &BillBoardPT[2]);
	RwV3dCrossProduct(&BBnormal, &BBa, &BBb);
	RwV3dNormalize(&BBnormal, &BBnormal);

	*BBvlist++ = BillBoardPT[0];
	*BBnlist++ = BBnormal;
	*BBTCoord++ = BBTexCoords[0];
	*BBvlist++ = BillBoardPT[1];
	*BBnlist++ = BBnormal;
	*BBTCoord++ = BBTexCoords[1];
	*BBvlist++ = BillBoardPT[2];
	*BBnlist++ = BBnormal;
	*BBTCoord++ = BBTexCoords[2];
	*BBvlist++ = BillBoardPT[3];
	*BBnlist++ = BBnormal;
	*BBTCoord++ = BBTexCoords[3];

	RpGeometryTriangleSetVertexIndices(BBGeometry, BBTlist, 0, 1, 2);
	RpGeometryTriangleSetMaterial(BBGeometry, BBTlist++, BBMaterial);
	RpGeometryTriangleSetVertexIndices(BBGeometry, BBTlist, 3, 2, 1);
	RpGeometryTriangleSetMaterial(BBGeometry, BBTlist++, BBMaterial);	
		
	//Atomic Create와 Frame등록 
	RpAtomic*	BBAtomic;
	RwFrame*	BBFrame;
	BBAtomic = RpAtomicCreate();
	BBFrame = RwFrameCreate();
	RpAtomicSetFrame(BBAtomic, BBFrame);

	//Nomalize
	{
        RwSphere boundingSphere;
        RwMatrix *matrix;
        RwV3d temp;

        RpMorphTargetCalcBoundingSphere(BBMorphTarget, &boundingSphere);
        matrix = RwMatrixCreate();

        RwV3dScale(&temp, &boundingSphere.center, -1.0f);
        RwMatrixTranslate(matrix, &temp, rwCOMBINEREPLACE);

        temp.x = temp.y = temp.z = 1.0f / boundingSphere.radius;
        RwMatrixScale(matrix, &temp, rwCOMBINEPOSTCONCAT);

        RpGeometryTransform(BBGeometry, matrix);
        
        RwMatrixDestroy(matrix);
    }

	//Atomic에 Geometry대입 
	RpAtomicSetGeometry(BBAtomic, BBGeometry, 0);
	
	RpGeometryDestroy(BBGeometry);
	RpMaterialDestroy(BBMaterial);
	RwTextureDestroy(BBTexture);
	
	//Image경로 돌려주기 
	RwImageSetPath(Originpath);

	//BillBoard의 Origin바꾸기 , Scale키우기 
	RwFrame* Atomic_Frame;
	RwV3d Trans_ToOrigin = { m_v3dOriginalPos.x, m_v3dOriginalPos.y, m_v3dOriginalPos.z 	};
	Atomic_Frame = RpAtomicGetFrame( BBAtomic );
	RwFrameTranslate( Atomic_Frame, &Trans_ToOrigin, rwCOMBINEPOSTCONCAT );
			
	if ( BBAtomic == NULL )	return NULL;

	return BBAtomic;
	
}



/*****************************************************************
*   Function : RenderIdle()
*   Comment  : Idle에 따라 Index를 선별하여 RotateBillBoard에 
*			   넘겨준다.                     
*   Date&Time : 2002-04-11, 오후 3:18
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
RwBool	AcuBillBoard::RenderIdle( RpAtomic* Atomic, RwCamera* Camera, RwReal fRate1, RwReal fRate2 )
{

		//카메라의 정보 구하기 
	RwV3d*		Camera_Lookat;
	Camera_Lookat = RwMatrixGetAt( RwFrameGetMatrix( RwCameraGetFrame( Camera )) );
	RwBool		bPolar = FALSE;				//카메라의 At벡터가 극지방에 있는가 
	if ( Camera_Lookat->x >= -0.0000000001f && Camera_Lookat->x <= 0.0000000001f && 
		Camera_Lookat->z <= 0.0000000001f && Camera_Lookat->z >= -0.0000000001f)  bPolar = TRUE;

	//Atomic의 위치정보 백업 
	RwMatrix *Atomic_Matrix;
	RwV3d Atomic_Pos;
	Atomic_Matrix = RwFrameGetMatrix( RpAtomicGetFrame( Atomic ) );
	Atomic_Pos = Atomic_Matrix->pos;
	
	/*********************빌보드 돌리기*********************/
	RwFrame *Atomic_Frame = RpAtomicGetFrame(Atomic);
	RwReal	Rotation_Angle = 0.0f;
	RwV3d	Atomic_Scale = { fRate1, fRate2, 0.0f };
	

	// x, z좌표에 의한 평면 돌리기 
	if ( Camera_Lookat->x > 0.0f )
	{
		Rotation_Angle = (RwReal)(90 - atan( Camera_Lookat->z / Camera_Lookat->x ) * 180 / 3.141592654f ) ;
		
		RwFrameRotate( Atomic_Frame, &Yaxis, Rotation_Angle, rwCOMBINEREPLACE         );
	}
	else 
	{	
		if ( Camera_Lookat->x == 0.0f )
				Rotation_Angle = 0.0f;//(RwReal)(-90 - atan( Camera_Lookat->z / 0.0000000000000001f ) * 180 / 3.141592654f ) ;
		else
				Rotation_Angle = (RwReal)(-90 - atan( Camera_Lookat->z / Camera_Lookat->x ) * 180 / 3.141592654f ) ;
		RwFrameRotate( Atomic_Frame, &Yaxis, Rotation_Angle, rwCOMBINEREPLACE         );		
		
	}					

	//y방향에 대하여 돌리기 
	if ( Camera_Lookat->y != 0.0f )				//Camera의 LookAt벡터의 y좌표가 0.0f면 돌리지 않는다. 
	{
		RwV3d	Rotate_Y_Axis = { 0.0f, 0.0f, 0.0f };
		Rotate_Y_Axis.x = Camera_Lookat->z;
		Rotate_Y_Axis.z = Camera_Lookat->x * (-1.0f);					//Lookat 벡터 xz평면에서 수직인 벡터 
	
		RwV2d  v2dXYLine = { 0.0f, 0.0f };					//Camera Lookat 벡터의 XZ평면상의 라인
		v2dXYLine.x = Camera_Lookat->x;		v2dXYLine.y = Camera_Lookat->z;
		RwReal Length_XY_Line = RwV2dLength( &v2dXYLine );
		
		if ( bPolar == (int)(TRUE) )
		{
			Rotate_Y_Axis.x = 1.0f; Rotate_Y_Axis.y = 0.0f; Rotate_Y_Axis.z = 0.0f;
			if ( Camera_Lookat->y > 0.0f )	
					Rotation_Angle = -90.0f;
			else
					Rotation_Angle = 90.0f;
		}
		else		
			Rotation_Angle = (RwReal)(atan( Camera_Lookat->y / Length_XY_Line ) * 180.0f / 3.141592654f ) * (-1.0f);
		
		RwFrameRotate( Atomic_Frame, &Rotate_Y_Axis, Rotation_Angle, rwCOMBINEPOSTCONCAT   );
	}			


	RwFrameScale( Atomic_Frame, &Atomic_Scale, rwCOMBINEPRECONCAT );		
	Atomic_Matrix->pos = Atomic_Pos;		
	
	return TRUE;
}

/*****************************************************************
*   Function : SetBillBoardTexturePath
*   Comment  : 텍스처있는 곳의 path를 입력한다. 
*                    
*   Date&Time : 2002-04-11, 오후 5:08
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcuBillBoard::SetTexturePath( RwChar sz_SetPath[30] )
{
	sprintf( m_szTPath, sz_SetPath );
}

/*****************************************************************
*   Function : SetBillBoardTextureName
*   Comment  : 텍스처 파일의 이름만 입력한다. (확장자 노~)
*                    
*   Date&Time : 2002-04-11, 오후 5:10
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AcuBillBoard::SetTextureName( RwChar sz_SetTName[30] )
{
	sprintf( m_szTName, sz_SetTName );
}


/*****************************************************************
*   Function : CreateDoubleSideBoardAtomic()
*   Comment  : 양면인 Plane을 생성하여 Atomic을 돌려준다. 
*   Date&Time : 2002-05-16, 오후 1:21
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
RpAtomic*	AcuBillBoard::CreateDoubleSideBoardAtomic()
{
		//Textrue Directory지정
	RwChar*			BBPath;
	RwChar*		Originpath;
	Originpath = RwImageGetPath();
	BBPath = RsPathnameCreate(m_szTPath);
	RwImageSetPath(BBPath);
	RsPathnameDestroy(BBPath);

	//Texture Format결정 
	switch( m_eTFormat )
	{
	case TFORMAT_BMP:
		RwImageRegisterImageFormat("bmp", RtBMPImageRead, NULL); 
		break;

	case TFORMAT_RAS:
		RwImageRegisterImageFormat("ras", RtRASImageRead  , NULL); 
		break;

	case TFORMAT_PNG:
		RwImageRegisterImageFormat("png", RtPNGImageRead, NULL); 
		break;
	
	case TFORMAT_TIF:
		RwImageRegisterImageFormat("tif", RtTIFFImageRead, NULL); 
		break;
	}	

	//Texture읽기 
	RwTexture*		BBTexture;
	BBTexture = RwTextureRead(m_szTName, NULL);
	RwTextureSetFilterMode(BBTexture, rwFILTERLINEAR);

	//TextureCoords등(Material 지정)
	RpMaterial*		BBMaterial;
	RwTexCoords		BBTexCoords[4];//BBTexCoords[4];
	RwSurfaceProperties		BBSPro;
	BBMaterial = RpMaterialCreate();
	RpMaterialSetTexture( BBMaterial, BBTexture );
	BBTexCoords[2].u = 0.0f;
	BBTexCoords[2].v = m_TexCoords_V;
	BBTexCoords[3].u = 0.0f;
	BBTexCoords[3].v = 0.0f;
	BBTexCoords[0].u = m_TexCoords_U;
	BBTexCoords[0].v = m_TexCoords_V;
	BBTexCoords[1].u = m_TexCoords_U;
	BBTexCoords[1].v = 0.0f;
	BBSPro.ambient =1.0f;
	BBSPro.diffuse = 1.0f;
	BBSPro.specular = 0.0f;	
	
	RpMaterialSetSurfaceProperties(BBMaterial, &BBSPro);
	
	//Geometry생성 list입력 
	RpGeometry*		BBGeometry;
	RpMorphTarget*	BBMorphTarget;
	RwV3d *BBvlist, *BBnlist;
	RpTriangle*		BBTlist;
	RwTexCoords*	BBTCoord;
	BBGeometry = RpGeometryCreate(8, 4, rpGEOMETRYLIGHT | rpGEOMETRYNORMALS | rpGEOMETRYTEXTURED );
	BBMorphTarget = RpGeometryGetMorphTarget(BBGeometry, 0);
	BBvlist = RpMorphTargetGetVertices(BBMorphTarget);
	BBnlist = RpMorphTargetGetVertexNormals(BBMorphTarget);

			//if ( BBGeometry == NULL )	return NULL;

	BBTlist = RpGeometryGetTriangles(BBGeometry);
	BBTCoord = RpGeometryGetVertexTexCoords(BBGeometry, rwTEXTURECOORDINATEINDEX0);
	
	//Geometry Nomal방향 결정과 포인트 리스트 써넣기  
	RwV3d BBa, BBb, BBnormal, BBnormal2;
	RwV3dSub(&BBa, &BillBoardPT[0], &BillBoardPT[1]);
	RwV3dSub(&BBb, &BillBoardPT[0], &BillBoardPT[2]);
	RwV3dCrossProduct(&BBnormal, &BBa, &BBb);
	RwV3dNormalize(&BBnormal, &BBnormal);
	
	*BBvlist++ = BillBoardPT[0];
	*BBnlist++ = BBnormal;
	*BBTCoord++ = BBTexCoords[0];
	*BBvlist++ = BillBoardPT[1];
	*BBnlist++ = BBnormal;
	*BBTCoord++ = BBTexCoords[1];
	*BBvlist++ = BillBoardPT[2];
	*BBnlist++ = BBnormal;
	*BBTCoord++ = BBTexCoords[2];
	*BBvlist++ = BillBoardPT[3];
	*BBnlist++ = BBnormal;
	*BBTCoord++ = BBTexCoords[3];

	RwV3dNegate(&BBnormal2, &BBnormal);

	*BBvlist++ = BillBoardPT[0];
	*BBnlist++ = BBnormal2;
	*BBTCoord++ = BBTexCoords[0];
	*BBvlist++ = BillBoardPT[1];
	*BBnlist++ = BBnormal2;
	*BBTCoord++ = BBTexCoords[1];
	*BBvlist++ = BillBoardPT[2];
	*BBnlist++ = BBnormal2;
	*BBTCoord++ = BBTexCoords[2];
	*BBvlist++ = BillBoardPT[3];
	*BBnlist++ = BBnormal2;
	*BBTCoord++ = BBTexCoords[3];

	RpGeometryTriangleSetVertexIndices(BBGeometry, BBTlist, 0, 1, 2);
	RpGeometryTriangleSetMaterial(BBGeometry, BBTlist++, BBMaterial);
	RpGeometryTriangleSetVertexIndices(BBGeometry, BBTlist, 3, 2, 1);
	RpGeometryTriangleSetMaterial(BBGeometry, BBTlist++, BBMaterial);
	RpGeometryTriangleSetVertexIndices(BBGeometry, BBTlist, 4, 6, 5);
	RpGeometryTriangleSetMaterial(BBGeometry, BBTlist++, BBMaterial);
	RpGeometryTriangleSetVertexIndices(BBGeometry, BBTlist, 7, 5, 6);
	RpGeometryTriangleSetMaterial(BBGeometry, BBTlist++, BBMaterial);
		
	//Atomic Create와 Frame등록 
	RpAtomic*	BBAtomic;
	RwFrame*	BBFrame;
	BBAtomic = RpAtomicCreate();
	BBFrame = RwFrameCreate();
	RpAtomicSetFrame(BBAtomic, BBFrame);

	//Nomalize
	{
        RwSphere boundingSphere;
        RwMatrix *matrix;
        RwV3d temp;

        RpMorphTargetCalcBoundingSphere(BBMorphTarget, &boundingSphere);
        matrix = RwMatrixCreate();

        RwV3dScale(&temp, &boundingSphere.center, -1.0f);
        RwMatrixTranslate(matrix, &temp, rwCOMBINEREPLACE);

        temp.x = temp.y = temp.z = 1.0f / boundingSphere.radius;
        RwMatrixScale(matrix, &temp, rwCOMBINEPOSTCONCAT);

        RpGeometryTransform(BBGeometry, matrix);
        
        RwMatrixDestroy(matrix);
    }

	//Atomic에 Geometry대입 
	RpAtomicSetGeometry(BBAtomic, BBGeometry, 0);
	
	RpGeometryDestroy(BBGeometry);
	RpMaterialDestroy(BBMaterial);
	RwTextureDestroy(BBTexture);
	
	//Image경로 돌려주기 
	RwImageSetPath(Originpath);

	//BillBoard의 Origin바꾸기 , Scale키우기 
	RwFrame* Atomic_Frame;
	RwV3d Trans_ToOrigin = { m_v3dOriginalPos.x, m_v3dOriginalPos.y, m_v3dOriginalPos.z 	};
	Atomic_Frame = RpAtomicGetFrame( BBAtomic );
	RwFrameTranslate( Atomic_Frame, &Trans_ToOrigin, rwCOMBINEPOSTCONCAT );
			
	if ( BBAtomic == NULL )	return NULL;

	return BBAtomic;


}