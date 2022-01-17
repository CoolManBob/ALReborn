#include "AcuCreateClump.h"

/*****************************************************************************
* Desc :
*****************************************************************************/
RwV3d ACC_UTIL_AVERAGE_V3D(RwV3d *pstV1, RwV3d *pstV2)
{
	RwV3d	stRt;

	RwV3dAdd(&stRt, pstV1, pstV2);
	RwV3dScale(&stRt, &stRt, 0.5f);

	return stRt;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwV3d ACC_UTIL_MAKE_V3D(FLOAT x, FLOAT y, FLOAT z)
{
	RwV3d v3dRt = {x, y, z};

	return v3dRt;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwV3d ACC_UTIL_MAKE_NORMAL(RwV3d *pv1, RwV3d *pv2, RwV3d *pv3)
{
	RwV3d	a, b, normal;

	RwV3dSub(&a, pv3, pv1);
	RwV3dSub(&b, pv2, pv1);

	RwV3dCrossProduct(&normal, &a, &b);
	RwV3dNormalize(&normal, &normal);

	return normal;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwV3d ACC_UTIL_ADD_NORMALIZE(RwV3d *pv1, RwV3d *pv2)
{
	RwV3d	normal;

	RwV3dAdd(&normal, pv1, pv2);
	RwV3dNormalize(&normal, &normal);

	return normal;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL ACC_UTIL_CREATE_CHILD_ATOMIC(RpClump *parent, RpAtomic **atomic)
{
	RwFrame *frame;

	*atomic = RpAtomicCreate();
	if (!(*atomic))
	{
		return FALSE;
	}

	frame = RwFrameCreate();
	if (!frame)
	{
		RpAtomicDestroy(*atomic);
		return FALSE;
	}

	RpAtomicSetFrame(*atomic, frame);

	RpClumpAddAtomic(parent, *atomic);

	RwFrameAddChild(RpClumpGetFrame(parent), frame);

	return TRUE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL ACC_UTIL_CREATE_BASE_CLUMP(RpClump **clump)
{
	RwFrame	*frame;

	*clump = RpClumpCreate();
	if (!(*clump))
	{
		return FALSE;
	}

	frame = RwFrameCreate();
	if (!frame)
	{
		RpClumpDestroy(*clump);
		return FALSE;
	}

	RpClumpSetFrame(*clump, frame);

	return TRUE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL ACC_UTIL_CALCULATE_NORMALS(RwV3d *nlist, RwV3d *vlist, RpTriangle *tlist, RwInt32 vertices, RwInt32 triangles)
{
	RwV3d	v3dNormal = {0.0f, 0.0f, 0.0f};
	INT32	lCount, lOffset1, lOffset2, lOffset3;

	for (lCount = 0; lCount < vertices; ++lCount)
	{
		memcpy(nlist + lCount, &v3dNormal, sizeof(RwV3d));
	}

	for (lCount = 0; lCount < triangles; ++lCount)
	{
		lOffset1			= tlist[lCount].vertIndex[2];
		lOffset2			= tlist[lCount].vertIndex[1];
		lOffset3			= tlist[lCount].vertIndex[0];

		v3dNormal			= ACC_UTIL_MAKE_NORMAL(vlist + lOffset1, vlist + lOffset2, vlist + lOffset3);

		nlist[lOffset1]		= ACC_UTIL_ADD_NORMALIZE(nlist + lOffset1, &v3dNormal);
		nlist[lOffset2]		= ACC_UTIL_ADD_NORMALIZE(nlist + lOffset2, &v3dNormal);
		nlist[lOffset3]		= ACC_UTIL_ADD_NORMALIZE(nlist + lOffset3, &v3dNormal);
	}

	return TRUE;
}

RpAtomic *ACC_UTIL_GET_ATOMICS_CB(RpAtomic *atomic, void *data)
{
	RpAtomic	**dest	= (RpAtomic **)(data);
	*(dest)				= atomic;

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL ACC_UTIL_SET_BOX_VERTICES_TRIANGLES(RpGeometry *geometry, RpMaterial *material, RwV3d *pstInf, RwV3d *pstSup, RwV3d *vlist, RpTriangle *tlist, INT32 *plVertices, INT32 *plTriangles)
{
	RwV3d			*vtemp					= vlist;

	// vertices
	*vlist++								= ACC_UTIL_MAKE_V3D(pstInf->x, pstInf->y, pstInf->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstSup->x, pstInf->y, pstInf->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstSup->x, pstSup->y, pstInf->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstInf->x, pstSup->y, pstInf->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstInf->x, pstInf->y, pstSup->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstSup->x, pstInf->y, pstSup->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstSup->x, pstSup->y, pstSup->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstInf->x, pstSup->y, pstSup->z);
/*
	// normal
	*nlist++								= ACC_UTIL_MAKE_NORMAL(vtemp + 1, vtemp + 3, vtemp + 4);
	*nlist++								= ACC_UTIL_MAKE_NORMAL(vtemp + 0, vtemp + 5, vtemp + 2);
	*nlist++								= ACC_UTIL_MAKE_NORMAL(vtemp + 1, vtemp + 6, vtemp + 3);
	*nlist++								= ACC_UTIL_MAKE_NORMAL(vtemp + 0, vtemp + 2, vtemp + 7);
	*nlist++								= ACC_UTIL_MAKE_NORMAL(vtemp + 0, vtemp + 7, vtemp + 5);
	*nlist++								= ACC_UTIL_MAKE_NORMAL(vtemp + 1, vtemp + 4, vtemp + 6);
	*nlist++								= ACC_UTIL_MAKE_NORMAL(vtemp + 2, vtemp + 5, vtemp + 7);
	*nlist++								= ACC_UTIL_MAKE_NORMAL(vtemp + 3, vtemp + 6, vtemp + 4);*/

	// facet-1
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 4 + *plVertices, 6 + *plVertices, 7 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 4 + *plVertices, 5 + *plVertices, 6 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-2
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 0 + *plVertices, 7 + *plVertices, 3 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 0 + *plVertices, 4 + *plVertices, 7 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-3
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 1 + *plVertices, 3 + *plVertices, 2 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 1 + *plVertices, 0 + *plVertices, 3 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-4
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 5 + *plVertices, 2 + *plVertices, 6 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 5 + *plVertices, 1 + *plVertices, 2 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-5
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 7 + *plVertices, 2 + *plVertices, 3 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 7 + *plVertices, 6 + *plVertices, 2 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-6
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 0 + *plVertices, 5 + *plVertices, 4 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 0 + *plVertices, 1 + *plVertices, 5 + *plVertices);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	*plVertices		+=	8;
	*plTriangles	+=	12;

	return TRUE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL ACC_UTIL_SET_SPHERE_VERTICES_TRIANGLES(RpGeometry *geometry, RpMaterial *material, RwV3d *pstPos, FLOAT fRadius, INT32 lIterations, RwV3d *vlist, RpTriangle *tlist, INT32 *plVertices, INT32 *plTriangles)
{
	INT32	lNum, lStart, i, j;

	vlist[0]								= ACC_UTIL_MAKE_V3D(1.0f, 1.0f, 1.0f);
	vlist[1]								= ACC_UTIL_MAKE_V3D(-1.0f, -1.0f, 1.0f);
	vlist[2]								= ACC_UTIL_MAKE_V3D(1.0f, -1.0f, -1.0f);
	vlist[3]								= ACC_UTIL_MAKE_V3D(-1.0f, 1.0f, -1.0f);

	RpGeometryTriangleSetVertexIndices(geometry, tlist + 0, 0 + *plVertices, 1 + *plVertices, 2 + *plVertices);
	RpGeometryTriangleSetVertexIndices(geometry, tlist + 1, 1 + *plVertices, 0 + *plVertices, 3 + *plVertices);
	RpGeometryTriangleSetVertexIndices(geometry, tlist + 2, 1 + *plVertices, 3 + *plVertices, 2 + *plVertices);
	RpGeometryTriangleSetVertexIndices(geometry, tlist + 3, 0 + *plVertices, 2 + *plVertices, 3 + *plVertices);

	lNum									= 4;
	for (i = 1; i < lIterations; ++i)
	{
		lStart								= lNum;

		for (j = 0; j < lStart; ++j)
		{
			memcpy(tlist + (lNum + 0), tlist + j, sizeof(RpTriangle));
			memcpy(tlist + (lNum + 1), tlist + j, sizeof(RpTriangle));
			memcpy(tlist + (lNum + 2), tlist + j, sizeof(RpTriangle));

			vlist[lNum + 0]					= ACC_UTIL_AVERAGE_V3D(vlist + (tlist[j].vertIndex[0] - *plVertices), vlist + (tlist[j].vertIndex[1] - *plVertices));
			vlist[lNum + 1]					= ACC_UTIL_AVERAGE_V3D(vlist + (tlist[j].vertIndex[1] - *plVertices), vlist + (tlist[j].vertIndex[2] - *plVertices));
			vlist[lNum + 2]					= ACC_UTIL_AVERAGE_V3D(vlist + (tlist[j].vertIndex[2] - *plVertices), vlist + (tlist[j].vertIndex[0] - *plVertices));

			tlist[j].vertIndex[1]			= lNum + *plVertices + 0;
			tlist[j].vertIndex[2]			= lNum + *plVertices + 2;

			tlist[lNum + 0].vertIndex[0]	= lNum + *plVertices + 0;
			tlist[lNum + 0].vertIndex[2]	= lNum + *plVertices + 1;
			tlist[lNum + 1].vertIndex[0]	= lNum + *plVertices + 2;
			tlist[lNum + 1].vertIndex[1]	= lNum + *plVertices + 1;
			tlist[lNum + 2].vertIndex[0]	= lNum + *plVertices + 0;
			tlist[lNum + 2].vertIndex[1]	= lNum + *plVertices + 1;
			tlist[lNum + 2].vertIndex[2]	= lNum + *plVertices + 2;

			lNum += 3;
		}
	}

	for (i = 0; i < lNum; ++i)
	{
		RwV3dNormalize(vlist + i, vlist + i);
		RwV3dScale(vlist + i, vlist + i, fRadius);
		RwV3dAdd(vlist + i, vlist + i, pstPos);
		RpGeometryTriangleSetMaterial(geometry, tlist + i, material);
	}

	*plVertices += lNum;
	*plTriangles += lNum;

	return TRUE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL ACC_UTIL_SET_CYLINDER_VERTICES_TRIANGLES(RpGeometry *geometry, RpMaterial *material, RwV3d *pstPos, FLOAT fRadius, FLOAT fHeight, INT32 lPrecision, RwV3d *vlist, RpTriangle *tlist, INT32 *plVertices, INT32 *plTriangles)
{
	INT32	lCount;
	RwV3d	stTemp					= {0, pstPos->y + fHeight, 0};
	RwV3d	stPerp1, stPerp2;

	INT32	lNumVert				= 2 * (lPrecision + 1);
	INT32	lNumTri					= 4 * lPrecision;

	RwV3d	*vtemplist				= vlist;

	memcpy(&stPerp1, &stTemp, sizeof(RwV3d));

	if ((stTemp.x == 0.0f) && (stTemp.z == 0.0f)) 
		stTemp.x					+= 1.0f;
	else
		stTemp.y					+= 1.0f;

	RwV3dCrossProduct(&stPerp2, &stPerp1, &stTemp);
	RwV3dCrossProduct(&stPerp1, &stTemp, &stPerp2);
	RwV3dNormalize(&stPerp1, &stPerp1);
	RwV3dNormalize(&stPerp2, &stPerp2);

	FLOAT	fTheta;
	RwV3d	stPos;

	memcpy(&stPos, pstPos, sizeof(RwV3d));
	stPos.y							+= fHeight;

	*vlist++						= *(pstPos);
	*vlist++						= stPos;

	INT32	lOffset1, lOffset2, lOffset3, lOffset4;
	for (lCount = 0; lCount < lPrecision; ++lCount)
	{
		// 좌표를 설정한다.
		fTheta						= lCount * (FLOAT)(D_ACC_360_TO_RADIAN) / lPrecision;
		stTemp.x					= (FLOAT)(cos(fTheta)) * stPerp1.x + (FLOAT)(sin(fTheta)) * stPerp2.x;
		stTemp.y					= (FLOAT)(cos(fTheta)) * stPerp1.y + (FLOAT)(sin(fTheta)) * stPerp2.y;
		stTemp.z					= (FLOAT)(cos(fTheta)) * stPerp1.z + (FLOAT)(sin(fTheta)) * stPerp2.z;
		RwV3dNormalize(&stTemp, &stTemp);

		stPos.x						= pstPos->x + fRadius * stTemp.x;
		stPos.y						= pstPos->y + fRadius * stTemp.y;
		stPos.z						= pstPos->z + fRadius * stTemp.z;

		*vlist++					= stPos;

//		stPos.x						= pstPos->x				+ fRadius * stTemp.x;
		stPos.y						= pstPos->y + fHeight	+ fRadius * stTemp.y;
//		stPos.z						= pstPos->z				+ fRadius * stTemp.z;

		*vlist++					= stPos;



		// 인덱스를 설정한다.
		lOffset1					= 2 + (lCount * 2) + *plVertices;	
		lOffset2					= lOffset1 + 1;
		lOffset3					= lOffset1 + 2;
		lOffset4					= lOffset1 + 3;

		if (lOffset3 == (lNumVert + *plVertices))
			lOffset3				= 2 + *plVertices;
		if (lOffset4 > (lNumVert + *plVertices))
			lOffset4				= 3 + *plVertices;

		//RpGeometryTriangleSetVertexIndices(geometry, tlist, 0 + *plVertices, lOffset3, lOffset1);
		RpGeometryTriangleSetVertexIndices(geometry, tlist, 0 + *plVertices, lOffset1, lOffset3);
		RpGeometryTriangleSetMaterial(geometry, tlist++, material);

		//RpGeometryTriangleSetVertexIndices(geometry, tlist, 1 + *plVertices, lOffset2, lOffset4);
		RpGeometryTriangleSetVertexIndices(geometry, tlist, 1 + *plVertices, lOffset4, lOffset2);
		RpGeometryTriangleSetMaterial(geometry, tlist++, material);

		//RpGeometryTriangleSetVertexIndices(geometry, tlist, lOffset1, lOffset4, lOffset2);
		RpGeometryTriangleSetVertexIndices(geometry, tlist, lOffset1, lOffset2, lOffset4);
		RpGeometryTriangleSetMaterial(geometry, tlist++, material);

		//RpGeometryTriangleSetVertexIndices(geometry, tlist, lOffset1, lOffset3, lOffset4);
		RpGeometryTriangleSetVertexIndices(geometry, tlist, lOffset1, lOffset4, lOffset3);
		RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	}

	*plVertices						+= lNumVert;
	*plTriangles					+= lNumTri;

	return TRUE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL ACC_UTIL_CREATE_GEOMETRY(INT32 lVertices, INT32 lTriangles, RpGeometry **ppGeom, RpMaterial **ppMat)
{
	RpMaterial			*material;
	RpGeometry			*geometry;
	RwSurfaceProperties	surfProp;

	*(ppGeom)								= NULL;
	*(ppMat)								= NULL;

	// material을 생성한다.
	material								= RpMaterialCreate();
	if (!material)
		return FALSE;

	*(ppMat)								= material;

	RpMaterialSetTexture(material, NULL);

	surfProp.ambient						= 0.3f;
    surfProp.diffuse						= 0.7f;
    surfProp.specular						= 0.0f;

	RpMaterialSetSurfaceProperties(material, &surfProp);

	// geometry를 생성한다.
	geometry								= RpGeometryCreate(lVertices, lTriangles, rpGEOMETRYNORMALS);
	if (!geometry)
		return FALSE;

	*(ppGeom)								= geometry;

	return TRUE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL ACC_UTIL_COUNT_VERTICES_TRIANGLES(AuBLOCKING *pastBlocking, INT32 lBlockingNum, INT32 lPrecision, INT32 *plVertices, INT32 *plTriangles)
{
	INT32	lIndex, lCount, lStart, i, j;

	*plVertices				= 0;
	*plTriangles			= 0;

	for (lIndex = 0; lIndex < lBlockingNum; ++lIndex)
	{
		if (pastBlocking[lIndex].type == AUBLOCKING_TYPE_BOX)
		{
			*(plVertices)	+= 8;
			*(plTriangles)	+= 12;
		}
		else if (pastBlocking[lIndex].type == AUBLOCKING_TYPE_SPHERE)
		{
			lCount			= 4;
			for (i = 1; i < lPrecision; ++i)
			{
				lStart		= lCount;
				
				for (j = 0; j < lStart; ++j)
				{
					lCount							+= 3;
				}
			}

			*(plVertices)	+= lCount;
			*(plTriangles)	+= lCount;
		}
		else if (pastBlocking[lIndex].type == AUBLOCKING_TYPE_CYLINDER)
		{
			*(plVertices)	+= 2 * ((lPrecision * D_ACC_CREATE_CLUMP_MULTIPLY_CYLINDER_PRECISION) + 1);
			*(plTriangles)	+= 4 * (lPrecision * D_ACC_CREATE_CLUMP_MULTIPLY_CYLINDER_PRECISION);
		}
		else
			continue;
	}

	return TRUE;
}

BOOL ACC_CREATE_CLUMP(CHAR *szDestFileName, AuBLOCKING *pastBlocking, INT32 lBlockingNum, INT32 lPrecision)
{
//@{ 2006/02/14 burumal

//	INT32				lCount, lVertices, lTriangles;
//	RpClump				*clump;
//	RpAtomic			*atomic;
//	RpGeometry			*geometry;
//	RpMaterial			*material;
//	RpMorphTarget		*morphTarget;
//	RwSphere			boundingSphere;
//	RwV3d				vtemp1, vtemp2;
//
//	if (lBlockingNum == 0)
//		return TRUE; // skip
//
//	if (!ACC_UTIL_COUNT_VERTICES_TRIANGLES(pastBlocking, lBlockingNum, lPrecision, &lVertices, &lTriangles))
//		return FALSE;
//
//	if (!ACC_UTIL_CREATE_GEOMETRY(lVertices, lTriangles, &geometry, &material))
//		return FALSE;
//
//	morphTarget						= RpGeometryGetMorphTarget(geometry, 0);
//	if (!morphTarget)
//		return FALSE;
//
//	lVertices = lTriangles			= 0;
//	for (lCount = 0; lCount < lBlockingNum; ++lCount)
//	{
//		if (pastBlocking[lCount].type == AUBLOCKING_TYPE_BOX)
//		{
//			vtemp1.x				= pastBlocking[lCount].data.box.inf.x;
//			vtemp1.y				= pastBlocking[lCount].data.box.inf.y;
//			vtemp1.z				= pastBlocking[lCount].data.box.inf.z;
//	
//			vtemp2.x				= pastBlocking[lCount].data.box.sup.x;
//			vtemp2.y				= pastBlocking[lCount].data.box.sup.y;
//			vtemp2.z				= pastBlocking[lCount].data.box.sup.z;
//
//			if (!ACC_UTIL_SET_BOX_VERTICES_TRIANGLES(
//				geometry,
//				material,
//				&vtemp1,
//				&vtemp2,
//				RpMorphTargetGetVertices(morphTarget) + lVertices,
//				RpGeometryGetTriangles(geometry) + lTriangles,
//				&lVertices,
//				&lTriangles								)	)
//				return FALSE;
//		}
//		else if (pastBlocking[lCount].type == AUBLOCKING_TYPE_SPHERE)
//		{
//			vtemp1.x	= pastBlocking[lCount].data.sphere.center.x;
//			vtemp1.y	= pastBlocking[lCount].data.sphere.center.y;
//			vtemp1.z	= pastBlocking[lCount].data.sphere.center.z;
//			
//			if (!ACC_UTIL_SET_SPHERE_VERTICES_TRIANGLES(
//				geometry,
//				material,
//				&vtemp1,
//				pastBlocking[lCount].data.sphere.radius,
//				lPrecision,
//				RpMorphTargetGetVertices(morphTarget) + lVertices,
//				RpGeometryGetTriangles(geometry) + lTriangles,
//				&lVertices,
//				&lTriangles									)	)
//				return FALSE;
//
////			ACC_UTIL_CALCULATE_NORMALS(
////				RpMorphTargetGetVertexNormals(morphTarget) + lTempVertices,
////				RpMorphTargetGetVertices(morphTarget) + lTempVertices,
////				RpGeometryGetTriangles(geometry) + lTempTriangles,
////				RpGeometryGetNumVertices(geometry),
////				RpGeometryGetNumTriangles(geometry)
////				);
//		}
//		else if (pastBlocking[lCount].type == AUBLOCKING_TYPE_CYLINDER)
//		{
//			vtemp1.x	= pastBlocking[lCount].data.cylinder.center.x;
//			vtemp1.y	= pastBlocking[lCount].data.cylinder.center.y;
//			vtemp1.z	= pastBlocking[lCount].data.cylinder.center.z;
//
//			if (!ACC_UTIL_SET_CYLINDER_VERTICES_TRIANGLES(
//				geometry,
//				material,
//				&vtemp1,
//				pastBlocking[lCount].data.cylinder.radius,
//				pastBlocking[lCount].data.cylinder.height,
//				lPrecision * D_ACC_CREATE_CLUMP_MULTIPLY_CYLINDER_PRECISION,
//				RpMorphTargetGetVertices(morphTarget) + lVertices,
//				RpGeometryGetTriangles(geometry) + lTriangles,
//				&lVertices,
//				&lTriangles									)	)
//				return FALSE;
//
////			ACC_UTIL_CALCULATE_NORMALS(
////				RpMorphTargetGetVertexNormals(morphTarget) + lTempVertices,
////				RpMorphTargetGetVertices(morphTarget) + lTempVertices,
////				RpGeometryGetTriangles(geometry) + lTempTriangles,
////				RpGeometryGetNumVertices(geometry),
////				RpGeometryGetNumTriangles(geometry)
////				);
//		}
//		else
//			continue;
//	}
//
//	// 노말값을 계산한다.
//	ACC_UTIL_CALCULATE_NORMALS(
//		RpMorphTargetGetVertexNormals(morphTarget),
//		RpMorphTargetGetVertices(morphTarget),
//		RpGeometryGetTriangles(geometry),
//		RpGeometryGetNumVertices(geometry),
//		RpGeometryGetNumTriangles(geometry)
//		);
//
//	// Bounding sphere를 설정한다.
//	RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
//    RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);
//    RpGeometryUnlock(geometry);
//
//	// clump를 만든다.
//	if (!ACC_UTIL_CREATE_BASE_CLUMP(&clump))
//		return FALSE;
//
//	// atomic을 만들어서 clump에 붙힌다.
//	if (!ACC_UTIL_CREATE_CHILD_ATOMIC(clump, &atomic))
//		return FALSE;
//
//	// atomic에 geometry를 설정한다.
//	RpAtomicSetGeometry(atomic, geometry, 0);
//
//	// geometry와 material을 해제한다.
//	RpGeometryDestroy(geometry);
//	RpMaterialDestroy(material);
//
//	// 파일을 생성한다.
//	RwStream			*pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, szDestFileName);
//	if (!pstStream)
//		return FALSE;
//
//	if (!RpClumpStreamWrite(clump, pstStream))
//		return FALSE;
//
//	RwStreamClose(pstStream, NULL);
//	RpClumpDestroy(clump);
//
//	return TRUE;

	if ( lBlockingNum == 0 )
		return TRUE; // skip

	RpClump* clump;
	
	RpAtomic*		atomic[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];
	RpGeometry*		geometry[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];
	RpMaterial*		material[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];
	RpMorphTarget*	morphTarget[D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP];

	ZeroMemory(atomic, sizeof(RpAtomic*) * D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP);
	ZeroMemory(geometry, sizeof(RpGeometry*) * D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP);
	ZeroMemory(material, sizeof(RpMaterial*) * D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP);
	ZeroMemory(morphTarget, sizeof(RpMorphTarget*) * D_ACC_MAX_BLOCKING_COUNT_PER_CLUMP);

	INT32 lCount;

	for ( lCount = 0; lCount < lBlockingNum; ++lCount )
	{
		INT32		lVertices, lTriangles;				
		RwSphere	boundingSphere;
		RwV3d		vtemp1, vtemp2;
		
		if ( !ACC_UTIL_COUNT_VERTICES_TRIANGLES(&(pastBlocking[lCount]), 1, lPrecision, &lVertices, &lTriangles) )
		{
			for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
			{
				RpGeometryDestroy(geometry[lIdx]);
				RpMaterialDestroy(material[lIdx]);
				RpAtomicDestroy(atomic[lCount]);
			}
			return FALSE;
		}

		if ( !ACC_UTIL_CREATE_GEOMETRY(lVertices, lTriangles, &geometry[lCount], &material[lCount]) )
		{
			for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
			{
				RpGeometryDestroy(geometry[lIdx]);
				RpMaterialDestroy(material[lIdx]);
				RpAtomicDestroy(atomic[lCount]);
			}
			return FALSE;
		}

		morphTarget[lCount] = RpGeometryGetMorphTarget(geometry[lCount], 0);

		if ( !morphTarget[lCount] )
		{
			for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
			{
				RpGeometryDestroy(geometry[lIdx]);
				RpMaterialDestroy(material[lIdx]);
				RpAtomicDestroy(atomic[lCount]);
			}
			return FALSE;
		}

		lVertices = lTriangles = 0;

		if ( pastBlocking[lCount].type == AUBLOCKING_TYPE_BOX )
		{
			vtemp1.x = pastBlocking[lCount].data.box.inf.x;
			vtemp1.y = pastBlocking[lCount].data.box.inf.y;
			vtemp1.z = pastBlocking[lCount].data.box.inf.z;
	
			vtemp2.x = pastBlocking[lCount].data.box.sup.x;
			vtemp2.y = pastBlocking[lCount].data.box.sup.y;
			vtemp2.z = pastBlocking[lCount].data.box.sup.z;

			if ( !ACC_UTIL_SET_BOX_VERTICES_TRIANGLES(
				geometry[lCount],
				material[lCount],
				&vtemp1,
				&vtemp2,
				RpMorphTargetGetVertices(morphTarget[lCount]) + lVertices,
				RpGeometryGetTriangles(geometry[lCount]) + lTriangles,
				&lVertices,
				&lTriangles) )
			{
				for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
				{
					RpGeometryDestroy(geometry[lIdx]);
					RpMaterialDestroy(material[lIdx]);
					RpAtomicDestroy(atomic[lCount]);
				}
				return FALSE;
			}
		}
		else 
		if ( pastBlocking[lCount].type == AUBLOCKING_TYPE_SPHERE )
		{
			vtemp1.x = pastBlocking[lCount].data.sphere.center.x;
			vtemp1.y = pastBlocking[lCount].data.sphere.center.y;
			vtemp1.z = pastBlocking[lCount].data.sphere.center.z;
			
			if ( !ACC_UTIL_SET_SPHERE_VERTICES_TRIANGLES(
				geometry[lCount],
				material[lCount],
				&vtemp1,
				pastBlocking[lCount].data.sphere.radius,
				lPrecision,
				RpMorphTargetGetVertices(morphTarget[lCount]) + lVertices,
				RpGeometryGetTriangles(geometry[lCount]) + lTriangles,
				&lVertices,
				&lTriangles) )
			{
				for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
				{
					RpGeometryDestroy(geometry[lIdx]);
					RpMaterialDestroy(material[lIdx]);
					RpAtomicDestroy(atomic[lCount]);
				}				
				return FALSE;
			}

//			ACC_UTIL_CALCULATE_NORMALS(
//				RpMorphTargetGetVertexNormals(morphTarget) + lTempVertices,
//				RpMorphTargetGetVertices(morphTarget) + lTempVertices,
//				RpGeometryGetTriangles(geometry) + lTempTriangles,
//				RpGeometryGetNumVertices(geometry),
//				RpGeometryGetNumTriangles(geometry)
//				);
		}
		else 
		if ( pastBlocking[lCount].type == AUBLOCKING_TYPE_CYLINDER )
		{
			vtemp1.x = pastBlocking[lCount].data.cylinder.center.x;
			vtemp1.y = pastBlocking[lCount].data.cylinder.center.y;
			vtemp1.z = pastBlocking[lCount].data.cylinder.center.z;

			if (!ACC_UTIL_SET_CYLINDER_VERTICES_TRIANGLES(
				geometry[lCount],
				material[lCount],
				&vtemp1,
				pastBlocking[lCount].data.cylinder.radius,
				pastBlocking[lCount].data.cylinder.height,
				lPrecision * D_ACC_CREATE_CLUMP_MULTIPLY_CYLINDER_PRECISION,
				RpMorphTargetGetVertices(morphTarget[lCount]) + lVertices,
				RpGeometryGetTriangles(geometry[lCount]) + lTriangles,
				&lVertices,
				&lTriangles) )
			{
				for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
				{
					RpGeometryDestroy(geometry[lIdx]);
					RpMaterialDestroy(material[lIdx]);
					RpAtomicDestroy(atomic[lCount]);
				}				
				return FALSE;
			}

//			ACC_UTIL_CALCULATE_NORMALS(
//				RpMorphTargetGetVertexNormals(morphTarget) + lTempVertices,
//				RpMorphTargetGetVertices(morphTarget) + lTempVertices,
//				RpGeometryGetTriangles(geometry) + lTempTriangles,
//				RpGeometryGetNumVertices(geometry),
//				RpGeometryGetNumTriangles(geometry)
//				);
		}
		else
			continue;

		// 노말값을 계산한다.
		ACC_UTIL_CALCULATE_NORMALS(
			RpMorphTargetGetVertexNormals(morphTarget[lCount]),
			RpMorphTargetGetVertices(morphTarget[lCount]),
			RpGeometryGetTriangles(geometry[lCount]),
			RpGeometryGetNumVertices(geometry[lCount]),
			RpGeometryGetNumTriangles(geometry[lCount])
			);

		// Bounding sphere를 설정한다.
		RpMorphTargetCalcBoundingSphere(morphTarget[lCount], &boundingSphere);
		RpMorphTargetSetBoundingSphere(morphTarget[lCount], &boundingSphere);
		RpGeometryUnlock(geometry[lCount]);
		
		// clump를 만든다.
		if ( lCount == 0 )
		{
			if (!ACC_UTIL_CREATE_BASE_CLUMP(&clump))
			{
				for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
				{
					RpGeometryDestroy(geometry[lIdx]);
					RpMaterialDestroy(material[lIdx]);
					RpAtomicDestroy(atomic[lCount]);
				}
				return FALSE;
			}
		}

		// atomic을 만들어서 clump에 붙힌다.
		if (!ACC_UTIL_CREATE_CHILD_ATOMIC(clump, &atomic[lCount]))
		{
			for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
			{
				RpGeometryDestroy(geometry[lIdx]);
				RpMaterialDestroy(material[lIdx]);
				RpAtomicDestroy(atomic[lCount]);
			}
			return FALSE;
		}

		// atomic에 geometry를 설정한다.
		RpAtomicSetGeometry(atomic[lCount], geometry[lCount], 0);

		// geometry와 material을 해제한다.
		RpGeometryDestroy(geometry[lCount]);
		RpMaterialDestroy(material[lCount]);
	}

	// 파일을 생성한다.
	RwStream* pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, szDestFileName);
	if ( !pstStream )
	{
		for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
		{
			RpGeometryDestroy(geometry[lIdx]);
			RpMaterialDestroy(material[lIdx]);
			RpAtomicDestroy(atomic[lCount]);
		}
		return FALSE;
	}

	if (!RpClumpStreamWrite(clump, pstStream))
	{
		for ( INT32 lIdx = 0; lIdx <= lCount; lIdx++ )
		{
			RpGeometryDestroy(geometry[lIdx]);
			RpMaterialDestroy(material[lIdx]);
			RpAtomicDestroy(atomic[lCount]);
		}
		return FALSE;
	}

	RwStreamClose(pstStream, NULL);
	RpClumpDestroy(clump);

	return TRUE;

//@}
}

BOOL ACC_CALC_VERTEX_NORMAL(RpClump *pstClump, CHAR *szFilePath, BOOL bReplaceNormals)
{
	RpAtomic			*pstAtomic		= NULL;

	RpClumpForAllAtomics(pstClump, ACC_UTIL_GET_ATOMICS_CB, (void *)(&pstAtomic));
	if (!pstAtomic)
		return FALSE;

	RpGeometry			*geometry		= RpAtomicGetGeometry(pstAtomic);
	RpMorphTarget		*morphTarget	= RpGeometryGetMorphTarget(geometry, 0);
	if ((!geometry) || (!morphTarget))
		return FALSE;

	RwV3d				*normal			= RpMorphTargetGetVertexNormals(morphTarget);
	if (!normal)
	{
		morphTarget->normals			= (RwV3d *)(malloc(sizeof(RwV3d) * RpGeometryGetNumVertices(geometry)));
		if (!morphTarget->normals)
			return FALSE;

		RwUInt32			ulFlags			= RpGeometryGetFlags(geometry);
		ulFlags								|= rpGEOMETRYNORMALS;
		RpGeometrySetFlags(geometry, ulFlags);

		normal							= RpMorphTargetGetVertexNormals(morphTarget);
	}
	else if (!bReplaceNormals)
		return TRUE;

	ACC_UTIL_CALCULATE_NORMALS(
		normal,
		RpMorphTargetGetVertices(morphTarget),
		RpGeometryGetTriangles(geometry),
		RpGeometryGetNumVertices(geometry),
		RpGeometryGetNumTriangles(geometry)			);

	// 파일을 생성한다.
	RwStream			*pstStream		= RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, szFilePath);
	if (!pstStream)
		return FALSE;

	if (!RpClumpStreamWrite(pstClump, pstStream))
		return FALSE;

	RwStreamClose(pstStream, NULL);

	return TRUE;
}

BOOL ACC_CREATE_BOX_CLUMP(CHAR *szFilePath, RwV3d *pstInf, RwV3d *pstSup)
{
	RpMaterial			*material	= NULL;
	RpGeometry			*geometry	= NULL;
	RpMorphTarget		*morphTarget;
	RwSphere			boundingSphere;
	RpClump				*clump;
	RpAtomic			*atomic;
	INT32				lVertices, lTriangles;

	lVertices						= 0;
	lTriangles						= 0;

	// geometry를 만든다.
	if (!ACC_UTIL_CREATE_GEOMETRY(8, 12, &geometry, &material))
		return FALSE;

	morphTarget						= RpGeometryGetMorphTarget(geometry, 0);
	if (!morphTarget)
		return FALSE;

	// geometry에 vertices와 triangles를 설정한다.
	if (!ACC_UTIL_SET_BOX_VERTICES_TRIANGLES(
		geometry,
		material,
		pstInf,
		pstSup,
		RpMorphTargetGetVertices(morphTarget),
		RpGeometryGetTriangles(geometry),
		&lVertices,
		&lTriangles								)	)
		return FALSE;

	ACC_UTIL_CALCULATE_NORMALS(
				RpMorphTargetGetVertexNormals(morphTarget),
				RpMorphTargetGetVertices(morphTarget),
				RpGeometryGetTriangles(geometry),
				RpGeometryGetNumVertices(geometry),
				RpGeometryGetNumTriangles(geometry)
				);

	// Bounding sphere를 설정한다.
	RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
    RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);
    RpGeometryUnlock(geometry);

	// clump를 만든다.
	if (!ACC_UTIL_CREATE_BASE_CLUMP(&clump))
		return FALSE;

	// atomic을 만들어서 clump에 붙힌다.
	if (!ACC_UTIL_CREATE_CHILD_ATOMIC(clump, &atomic))
		return FALSE;

	// atomic에 geometry를 설정한다.
	RpAtomicSetGeometry(atomic, geometry, 0);

	// geometry와 material을 해제한다.
	RpGeometryDestroy(geometry);
	RpMaterialDestroy(material);

	// 파일을 생성한다.
	RwStream			*pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, szFilePath);
	if (!pstStream)
		return FALSE;

	if (!RpClumpStreamWrite(clump, pstStream))
		return FALSE;

	RwStreamClose(pstStream, NULL);
	RpClumpDestroy(clump);

	return TRUE;
}

BOOL ACC_CREATE_SPHERE_CLUMP(CHAR *szFilePath, RwV3d *pstPos, FLOAT fRadius, INT32 lIterations)
{
	RpMaterial			*material	= NULL;
	RpGeometry			*geometry	= NULL;
	RpMorphTarget		*morphTarget;
	RwSphere			boundingSphere;
	RpClump				*clump;
	RpAtomic			*atomic;
	INT32				lTemp, lCount, lStart, i, j;

	lTemp							= 0;

	lCount			= 4;
	for (i = 1; i < lIterations; ++i) 
	{
		lStart						= lCount;
		
		for (j = 0; j < lStart; ++j)
		{
			lCount					+= 3;
		}
	}

	// geometry를 만든다.
	if (!ACC_UTIL_CREATE_GEOMETRY(lCount, lCount, &geometry, &material))
		return FALSE;

	morphTarget						= RpGeometryGetMorphTarget(geometry, 0);
	if (!morphTarget)
		return FALSE;

	// geometry에 vertices와 triangles를 설정한다.
	if (!ACC_UTIL_SET_SPHERE_VERTICES_TRIANGLES(
		geometry,
		material,
		pstPos,
		fRadius,
		lIterations,
		RpMorphTargetGetVertices(morphTarget), 
		RpGeometryGetTriangles(geometry),
		&lTemp,
		&lTemp										)	)
		return FALSE;

	ACC_UTIL_CALCULATE_NORMALS(
				RpMorphTargetGetVertexNormals(morphTarget),
				RpMorphTargetGetVertices(morphTarget),
				RpGeometryGetTriangles(geometry),
				RpGeometryGetNumVertices(geometry),
				RpGeometryGetNumTriangles(geometry)
				);

	// Bounding sphere를 설정한다.
	RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
    RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);
    RpGeometryUnlock(geometry);

	// clump를 만든다.
	if (!ACC_UTIL_CREATE_BASE_CLUMP(&clump))
		return FALSE;

	// atomic을 만들어서 clump에 붙힌다.
	if (!ACC_UTIL_CREATE_CHILD_ATOMIC(clump, &atomic))
		return FALSE;

	// atomic에 geometry를 설정한다.
	RpAtomicSetGeometry(atomic, geometry, 0);

	// geometry와 material을 해제한다.
	RpGeometryDestroy(geometry);
	RpMaterialDestroy(material);

	// 파일을 생성한다.
	RwStream			*pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, szFilePath);
	if (!pstStream)
		return FALSE;

	if (!RpClumpStreamWrite(clump, pstStream))
		return FALSE;

	RwStreamClose(pstStream, NULL);
	RpClumpDestroy(clump);

	return TRUE;
}

BOOL ACC_CREATE_CYLINER_CLUMP(CHAR *szFilePath, RwV3d *pstPos, FLOAT fHeight, FLOAT fRadius, INT32 lPrecision, FLOAT fOffset)
{
	RpMaterial			*material	= NULL;
	RpGeometry			*geometry	= NULL;
	RpMorphTarget		*morphTarget;
	RwSphere			boundingSphere;
	RpClump				*clump;
	RpAtomic			*atomic;
	INT32				lTemp, lVertices, lTriangles;

	lTemp							= 0;
	lVertices						= 2 * (lPrecision + 1);
	lTriangles						= 4 * lPrecision;

	// geometry를 만든다.
	if (!ACC_UTIL_CREATE_GEOMETRY(lVertices, lTriangles, &geometry, &material))
		return FALSE;

	morphTarget						= RpGeometryGetMorphTarget(geometry, 0);
	if (!morphTarget)
		return FALSE;

	// geometry에 vertices와 triangles를 설정한다.
	if (!ACC_UTIL_SET_CYLINDER_VERTICES_TRIANGLES(
		geometry,
		material,
		pstPos,
		fRadius,
		fHeight,
		lPrecision,
		RpMorphTargetGetVertices(morphTarget),
		RpGeometryGetTriangles(geometry),
		&lTemp,
		&lTemp										)	)
		return FALSE;

	ACC_UTIL_CALCULATE_NORMALS(
				RpMorphTargetGetVertexNormals(morphTarget),
				RpMorphTargetGetVertices(morphTarget),
				RpGeometryGetTriangles(geometry),
				RpGeometryGetNumVertices(geometry),
				RpGeometryGetNumTriangles(geometry)
				);

	// Bounding sphere를 설정한다.
	RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
    RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);
    RpGeometryUnlock(geometry);

	// clump를 만든다.
	if (!ACC_UTIL_CREATE_BASE_CLUMP(&clump))
		return FALSE;

	// atomic을 만들어서 clump에 붙힌다.
	if (!ACC_UTIL_CREATE_CHILD_ATOMIC(clump, &atomic))
		return FALSE;

	// atomic에 geometry를 설정한다.
	RpAtomicSetGeometry(atomic, geometry, 0);

	// geometry와 material을 해제한다.
	RpGeometryDestroy(geometry);
	RpMaterialDestroy(material);

	// 파일을 생성한다.
	RwStream			*pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, szFilePath);
	if (!pstStream)
		return FALSE;

	if (!RpClumpStreamWrite(clump, pstStream))
		return FALSE;

	RwStreamClose(pstStream, NULL);
	RpClumpDestroy(clump);

	return TRUE;
}


/*
BOOL ACC_UTIL_CREATE_BOX_GEOMETRY(RwV3d *pstInf, RwV3d *pstSup, RpGeometry **ppGeom, RpMaterial **ppMat)
{
	INT32				lNumVert			= 8;
	INT32				lNumTri				= 12;
//	INT32				lNumInd				= 36;

	RwV3d				*vlist;
    RpTriangle			*tlist;
	RpMorphTarget		*morphTarget;
	RpMaterial			*material;
	RpGeometry			*geometry;
	RwSurfaceProperties	surfProp;

	*(ppGeom)								= NULL;
	*(ppMat)								= NULL;

	// material을 생성한다.
	material								= RpMaterialCreate();
	if (!material)
		return FALSE;

	*(ppMat)								= material;

	RpMaterialSetTexture(material, NULL);

	surfProp.ambient						= 0.3f;
    surfProp.diffuse						= 0.7f;
    surfProp.specular						= 0.0f;

	RpMaterialSetSurfaceProperties(material, &surfProp);

	// geometry를 생성한다.
	geometry								= RpGeometryCreate(lNumVert, lNumTri, 0);
	if (!geometry)
		return FALSE;

	*(ppGeom)								= geometry;

	morphTarget								= RpGeometryGetMorphTarget(geometry, 0);
	if (!morphTarget)
		return FALSE;

	vlist									= RpMorphTargetGetVertices(morphTarget);
    tlist									= RpGeometryGetTriangles(geometry);

	if ((!vlist) || (!tlist))
		return FALSE;

	// vertices
	*vlist++								= ACC_UTIL_MAKE_V3D(pstInf->x, pstInf->y, pstInf->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstSup->x, pstInf->y, pstInf->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstSup->x, pstSup->y, pstInf->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstInf->x, pstSup->y, pstInf->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstInf->x, pstInf->y, pstSup->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstSup->x, pstInf->y, pstSup->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstSup->x, pstSup->y, pstSup->z);
	*vlist++								= ACC_UTIL_MAKE_V3D(pstInf->x, pstSup->y, pstSup->z);

	// facet-1
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 4, 6, 7);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 4, 5, 6);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-2
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 0, 7, 3);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 0, 4, 7);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-3
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 1, 3, 2);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 1, 0, 3);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-4
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 5, 2, 6);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 5, 1, 2);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-5
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 7, 2, 3);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 7, 6, 2);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// facet-6
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 0, 5, 4);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	RpGeometryTriangleSetVertexIndices(geometry, tlist, 0, 1, 5);
	RpGeometryTriangleSetMaterial(geometry, tlist++, material);

	// bounding sphere를 구한다.
	{
		RwSphere boundingSphere;

        RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
        RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);

        RpGeometryUnlock(geometry);
	}

	return TRUE;
}

BOOL ACC_UTIL_CYLINDER_GEOMETRY(RwV3d *pstPos, FLOAT fHeight, FLOAT fRadius, INT32 lPrecision, FLOAT fOffset, RpGeometry **ppGeom, RpMaterial **ppMat)
{
	INT32				lNumVert	= 2 * (lPrecision + 1);
	INT32				lNumTri		= 4 * lPrecision;
//	INT32				lNumInd		= lNumTri * 3;

	INT32				lCount;
	RwV3d				*vlist;
    RpTriangle			*tlist;
	RpMorphTarget		*morphTarget;
	RpMaterial			*material;
	RpGeometry			*geometry;
	RwSurfaceProperties	surfProp;

	*(ppGeom)						= NULL;
	*(ppMat)						= NULL;

	// material을 생성한다.
	material						= RpMaterialCreate();
	if (!material)
		return FALSE;

	*(ppMat)						= material;

	RpMaterialSetTexture(material, NULL);

	surfProp.ambient				= 0.3f;
    surfProp.diffuse				= 0.7f;
    surfProp.specular				= 0.0f;

	RpMaterialSetSurfaceProperties(material, &surfProp);

	// geometry를 생성한다.
	geometry						= RpGeometryCreate(lNumVert, lNumTri, 0);
	if (!geometry)
		return FALSE;

	*(ppGeom)						= geometry;

	morphTarget						= RpGeometryGetMorphTarget(geometry, 0);
	if (!morphTarget)
		return FALSE;

	vlist							= RpMorphTargetGetVertices(morphTarget);
    tlist							= RpGeometryGetTriangles(geometry);

	if ((!vlist) || (!tlist))
		return FALSE;



	// 좌표를 계산한다.
	RwV3d	stTemp					= {0, pstPos->y + fHeight, 0};
	RwV3d	stPerp1, stPerp2;

	memcpy(&stPerp1, &stTemp, sizeof(RwV3d));

	if ((stTemp.x == 0.0f) && (stTemp.z == 0.0f)) 
		stTemp.x					+= 1.0f;
	else
		stTemp.y					+= 1.0f;

	RwV3dCrossProduct(&stPerp2, &stPerp1, &stTemp);
	RwV3dCrossProduct(&stPerp1, &stTemp, &stPerp2);
	RwV3dNormalize(&stPerp1, &stPerp1);
	RwV3dNormalize(&stPerp2, &stPerp2);

	FLOAT	fTheta;
	RwV3d	stPos;

	memcpy(&stPos, pstPos, sizeof(RwV3d));
	stPos.y				
	
	  += fHeight;

	*vlist++						= *(pstPos);
	*vlist++						= stPos;

	for (lCount = 0; lCount < lPrecision; ++lCount)
	{
		fTheta						= lCount * fOffset / lPrecision;
		stTemp.x					= (FLOAT)(cos(fTheta)) * stPerp1.x + (FLOAT)(sin(fTheta)) * stPerp2.x;
		stTemp.y					= (FLOAT)(cos(fTheta)) * stPerp1.y + (FLOAT)(sin(fTheta)) * stPerp2.y;
		stTemp.z					= (FLOAT)(cos(fTheta)) * stPerp1.z + (FLOAT)(sin(fTheta)) * stPerp2.z;
		RwV3dNormalize(&stTemp, &stTemp);

		stPos.x						= pstPos->x + fRadius * stTemp.x;
		stPos.y						= pstPos->y + fRadius * stTemp.y;
		stPos.z						= pstPos->z + fRadius * stTemp.z;

		*vlist++					= stPos;

		stPos.x						= pstPos->x				+ fRadius * stTemp.x;
		stPos.y						= pstPos->y + fHeight	+ fRadius * stTemp.y;
		stPos.z						= pstPos->z				+ fRadius * stTemp.z;

		*vlist++					= stPos;
	}

	// 좌표리스트의 포인터를 초기화시킨다.
	vlist							= RpMorphTargetGetVertices(morphTarget);

	// 삼각형의 좌표 인덱스를 설정한다.
	INT32	lOffset1, lOffset2, lOffset3, lOffset4;
	for (lCount = 0; lCount < lPrecision; ++lCount)
	{
		lOffset1					= 2 + (lCount * 2);	
		lOffset2					= lOffset1 + 1;
		lOffset3					= lOffset1 + 2;
		lOffset4					= lOffset1 + 3;

		if (lOffset3 == lNumVert)
			lOffset3				= 2;
		if (lOffset4 > lNumVert)
			lOffset4				= 3;

		RpGeometryTriangleSetVertexIndices(geometry, tlist, 0, lOffset3, lOffset1);
		RpGeometryTriangleSetMaterial(geometry, tlist++, material);

		RpGeometryTriangleSetVertexIndices(geometry, tlist, 1, lOffset2, lOffset4);
		RpGeometryTriangleSetMaterial(geometry, tlist++, material);

		RpGeometryTriangleSetVertexIndices(geometry, tlist, lOffset1, lOffset4, lOffset2);
		RpGeometryTriangleSetMaterial(geometry, tlist++, material);

		RpGeometryTriangleSetVertexIndices(geometry, tlist, lOffset1, lOffset3, lOffset4);
		RpGeometryTriangleSetMaterial(geometry, tlist++, material);
	}

	// bounding sphere를 구한다.
	{
		RwSphere boundingSphere;

        RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
        RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);

        RpGeometryUnlock(geometry);
	}

	return TRUE;
}

BOOL ACC_UTIL_CREATE_SPHERE_GEOMETRY(RwV3d *pstPos, FLOAT fRadius, INT32 lIterations, RpGeometry **ppGeom, RpMaterial **ppMat)
{
	INT32				lNum				= 4;
	INT32				lStart;
	INT32				i, j;

	for (i = 1; i < lIterations; ++i) 
	{
		lStart								= lNum;
		
		for (j = 0; j < lStart; ++j)
		{
			lNum							+= 3;
		}
	}

	INT32				lNumVert			= lNum;
	INT32				lNumTri				= lNum;
//	INT32				lNumInd				= lNum * 3;

	RwV3d				*vlist;
    RpTriangle			*tlist;
	RpMorphTarget		*morphTarget;
	RpMaterial			*material;
	RpGeometry			*geometry;
	RwSurfaceProperties	surfProp;

	*(ppGeom)								= NULL;
	*(ppMat)								= NULL;

	// material을 생성한다.
	material								= RpMaterialCreate();
	if (!material)
		return FALSE;

	*(ppMat)								= material;

	RpMaterialSetTexture(material, NULL);

	surfProp.ambient						= 0.3f;
    surfProp.diffuse						= 0.7f;
    surfProp.specular						= 0.0f;

	RpMaterialSetSurfaceProperties(material, &surfProp);

	// geometry를 생성한다.
	geometry								= RpGeometryCreate(lNumVert, lNumTri, 0);
	if (!geometry)
		return FALSE;

	*(ppGeom)								= geometry;

	morphTarget								= RpGeometryGetMorphTarget(geometry, 0);
	if (!morphTarget)
		return FALSE;

	vlist									= RpMorphTargetGetVertices(morphTarget);
    tlist									= RpGeometryGetTriangles(geometry);

	if ((!vlist) || (!tlist))
		return FALSE;

	
	// 좌표, 인덱스를 계산한다.
	vlist[0]								= ACC_UTIL_MAKE_V3D(1.0f, 1.0f, 1.0f);
	vlist[1]								= ACC_UTIL_MAKE_V3D(-1.0f, -1.0f, 1.0f);
	vlist[2]								= ACC_UTIL_MAKE_V3D(1.0f, -1.0f, -1.0f);
	vlist[3]								= ACC_UTIL_MAKE_V3D(-1.0f, 1.0f, -1.0f);

	RpGeometryTriangleSetVertexIndices(geometry, tlist + 0, 0, 1, 2);
	RpGeometryTriangleSetVertexIndices(geometry, tlist + 1, 1, 0, 3);
	RpGeometryTriangleSetVertexIndices(geometry, tlist + 2, 1, 3, 2);
	RpGeometryTriangleSetVertexIndices(geometry, tlist + 3, 0, 2, 3);

	lNum									= 4;
	for (i = 1; i < lIterations; ++i)
	{
		lStart								= lNum;

		for (j = 0; j < lStart; ++j)
		{
			memcpy(tlist + (lNum + 0), tlist + j, sizeof(RpTriangle));
			memcpy(tlist + (lNum + 1), tlist + j, sizeof(RpTriangle));
			memcpy(tlist + (lNum + 2), tlist + j, sizeof(RpTriangle));

			vlist[lNum + 0]					= ACC_UTIL_AVERAGE_V3D(vlist + tlist[j].vertIndex[0], vlist + tlist[j].vertIndex[1]);
			vlist[lNum + 1]					= ACC_UTIL_AVERAGE_V3D(vlist + tlist[j].vertIndex[1], vlist + tlist[j].vertIndex[2]);
			vlist[lNum + 2]					= ACC_UTIL_AVERAGE_V3D(vlist + tlist[j].vertIndex[2], vlist + tlist[j].vertIndex[0]);

			tlist[j].vertIndex[1]			= lNum + 0;
			tlist[j].vertIndex[2]			= lNum + 2;

			tlist[lNum + 0].vertIndex[0]	= lNum + 0;
			tlist[lNum + 0].vertIndex[2]	= lNum + 1;
			tlist[lNum + 1].vertIndex[0]	= lNum + 2;
			tlist[lNum + 1].vertIndex[1]	= lNum + 1;
			tlist[lNum + 2].vertIndex[0]	= lNum + 0;
			tlist[lNum + 2].vertIndex[1]	= lNum + 1;
			tlist[lNum + 2].vertIndex[2]	= lNum + 2;

			lNum += 3;
		}
	}

	for (i = 0; i < lNum; ++i)
	{
		RwV3dNormalize(vlist + i, vlist + i);
		RwV3dScale(vlist + i, vlist + i, fRadius);
		RwV3dAdd(vlist + i, vlist + i, pstPos);
		RpGeometryTriangleSetMaterial(geometry, tlist + i, material);
	}

	// bounding sphere를 구한다.
	{
		RwSphere boundingSphere;

        RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
        RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);

        RpGeometryUnlock(geometry);
	}

	return TRUE;
}*/