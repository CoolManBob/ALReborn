#include "AcuReClump.h"

/*
//동적 할당으로 변경
RwInt32			g_AtomicCount;
AtomicData		*g_listAData = NULL;

RpAtomic* RearrangeAtomicCB(RpAtomic *atomic, void *data) 
{
	RpGeometry* lGeom = RpAtomicGetGeometry (atomic);
	RwInt32		matnum = RpGeometryGetNumMaterials(lGeom);
	RwInt32		numvert = RpGeometryGetNumVertices(lGeom); 
	RwInt32		i,j,numtri;

	RwBool		bPreLit,bNormal;		//	Geometry 포맷 체크

	RwUInt32	lGeomFlags = RpGeometryGetFlags(lGeom);
	bPreLit = (lGeomFlags & rpGEOMETRYPRELIT) ? TRUE : FALSE;
	bNormal = (lGeomFlags & rpGEOMETRYNORMALS) ? TRUE : FALSE;

	RwUInt32	lAtomicFlags = RpAtomicGetFlags(atomic);

	if(matnum == 1)		// 분류없이 복사한다.
	{
		g_listAData = new AtomicData;
		ClearAData(g_listAData);
				
		RpTriangle* ltriList = RpGeometryGetTriangles(lGeom);

		numtri = RpGeometryGetNumTriangles(lGeom);
		g_listAData->trilist = new RpTriangle[numtri];
		for(i=0;i<numtri;++i)
		{
			g_listAData->trilist[i].vertIndex[0] =  ltriList->vertIndex[0];
			g_listAData->trilist[i].vertIndex[1] =  ltriList->vertIndex[1];
			g_listAData->trilist[i].vertIndex[2] =  ltriList->vertIndex[2];
			g_listAData->trilist[i].matIndex = ltriList->matIndex;

			++ltriList;
		}

		RpMaterial*  lMat = RpGeometryGetMaterial(lGeom, 0);
		RpMatFXMaterialFlags  lMatFxFlag =
			RpMatFXMaterialGetEffects (lMat);

		if(lMatFxFlag == rpMATFXEFFECTNULL)
			g_listAData->texc_num = 1; 
		else
			g_listAData->texc_num = RpGeometryGetNumTexCoordSets (lGeom);
		int		texc_num = g_listAData->texc_num;
		
		// 2중 포인터 할당
		g_listAData->texclist = new RwTexCoords*[texc_num];
		for(i=0;i<texc_num;++i)
			g_listAData->texclist[i] = new RwTexCoords[numvert];
		
		for(i=0;i<g_listAData->texc_num;++i)
		{
			 RwTexCoords* texcoord =  RpGeometryGetVertexTexCoords (lGeom,(enum RwTextureCoordinateIndex)(i+1));
			
			 for(j=0;j<numvert;++j)
			 {
				g_listAData->texclist[i][j].u = texcoord->u;
				g_listAData->texclist[i][j].v = texcoord->v;
		
				++texcoord;
			 }
		}

		g_listAData->morphtarget_num = RpGeometryGetNumMorphTargets (lGeom);
		g_listAData->vertlist = new RwV3d*[g_listAData->morphtarget_num];
		
		if(bNormal)
		g_listAData->normallist = new RwV3d*[g_listAData->morphtarget_num];
		
		for(i=0;i<g_listAData->morphtarget_num;++i)
		{
			g_listAData->vertlist[i] = new RwV3d[numvert];

			RpMorphTarget* lMorph = RpGeometryGetMorphTarget(lGeom,i);
			RwV3d* lVert = RpMorphTargetGetVertices(lMorph);
			
			for(j=0;j<numvert;++j)
			{
				g_listAData->vertlist[i][j].x = lVert->x;
				g_listAData->vertlist[i][j].y = lVert->y;
				g_listAData->vertlist[i][j].z = lVert->z;
				++lVert;
			}
					
			if(bNormal)
			{
				g_listAData->normallist[i] = new RwV3d[numvert];
							
				for(i=0;i<g_listAData->morphtarget_num;++i)
				{
					RwV3d* lNormal = RpMorphTargetGetVertexNormals(lMorph); 

					for(j=0;j<numvert;++j)
					{
						g_listAData->normallist[i][j].x = lNormal->x;
						g_listAData->normallist[i][j].y = lNormal->y;
						g_listAData->normallist[i][j].z = lNormal->z;
						++lNormal;
					}
				}
			}	
		}

		if(bPreLit)
		{
			RwRGBA* lColor = RpGeometryGetPreLightColors(lGeom);
			g_listAData->prelitlist = new RwRGBA[numvert];
				
			for(i=0;i<numvert;++i)
			{
				g_listAData->prelitlist[i].red = lColor->red;
				g_listAData->prelitlist[i].green = lColor->green;
				g_listAData->prelitlist[i].blue = lColor->blue;
				g_listAData->prelitlist[i].alpha = lColor->alpha;
							
				++lColor;
			}
		}

		g_listAData->atomic_flag = lAtomicFlags;
		g_listAData->geom_flag = lGeomFlags;
		g_listAData->pmaterial = lMat;
	}
	else	// atomic분리
	{
		const	int		Split_Capacity  = 128;
		numtri = RpGeometryGetNumTriangles(lGeom);
		
		AtomicData*		pData;
		RwInt16**		tmap = new RwInt16*[Split_Capacity];
		for(i=0;i<Split_Capacity;++i)
			tmap[i] = new RwInt16[numtri];
		
		RwInt16*		tmapIndex = new RwInt16[Split_Capacity];
		for(i=0;i<Split_Capacity;++i) tmapIndex[i] = 0;

		RpTriangle* ltriList = RpGeometryGetTriangles(lGeom);
				
		RwInt32*		numTriList = new RwInt32[Split_Capacity];					// ## 128개 까지 분리~
		for(i=0;i<Split_Capacity;++i) numTriList[i] = 0;

		for(i=0;i<numtri;++i)
		{
			RwInt32		matid = ltriList->matIndex;
			RwInt32		k;
			
			for(j=0;j<3;++j)
			{
				RwBool		bFind = FALSE;
				for(k=0;k<tmapIndex[matid];++k)
				{
					if(tmap[matid][k] == ltriList->vertIndex[j])
					{
						bFind = TRUE;
						break;
					}
				}

				if(!bFind)
				{
					tmap[matid][tmapIndex[matid]] = ltriList->vertIndex[j];
					++tmapIndex[matid];
				}

				pData = GetAData(g_AtomicCount+matid);
				if(!pData->trilist) pData->trilist = new RpTriangle[numtri];
				pData->trilist[numTriList[g_AtomicCount+matid]].vertIndex[j] = k;
			}

			numTriList[g_AtomicCount+matid] += 1;

			pData = GetAData(g_AtomicCount+matid);
			pData->trilist[i].matIndex = ltriList->matIndex;
			++ltriList;
		}
		
		for(i=0;i<matnum;++i)
		{
			RpMaterial*  lMat = RpGeometryGetMaterial(lGeom, i);
			RpMatFXMaterialFlags  lMatFxFlag =
				RpMatFXMaterialGetEffects (lMat);

			pData = GetAData(g_AtomicCount+i);

			pData->tri_num = numTriList[g_AtomicCount+i];
			pData->vert_num = tmapIndex[i];

			pData->atomic_flag = lAtomicFlags;
			pData->geom_flag = lGeomFlags;

			// Texture Coord 복사
			if(lMatFxFlag == rpMATFXEFFECTNULL)
			{
				pData->texc_num = 1;
				if(lGeomFlags & rpGEOMETRYTEXTURED2)		// 텍스쳐 좌표2개 이상 flag시 1개 플래그로 set
				{
					RwInt32 mask = 0x0;
					mask |= rpGEOMETRYTEXTURED2;
					mask = ~mask;
					lGeomFlags &= mask;
					lGeomFlags |= rpGEOMETRYTEXTURED;
				}
			}
			else
				pData->texc_num = RpGeometryGetNumTexCoordSets (lGeom);

			pData->texclist = new RwTexCoords*[pData->texc_num];
			for(int t=0;t<pData->texc_num;++t)
				pData->texclist[t]  = new RwTexCoords[numvert];

			for(j=0;j<pData->texc_num;++j)
			{
				RwTexCoords* texcoord =  RpGeometryGetVertexTexCoords (lGeom,(enum RwTextureCoordinateIndex)(j+1));
			
				for(RwInt32 k=0;k<numTriList[g_AtomicCount+i];++k)
				{
					for(RwInt32 m=0;m<3;++m)
					{
						RwTexCoords  texc;

						RwInt32 lid = tmap[i][pData->trilist[k].vertIndex[m]];
						texc.u = (texcoord+lid)->u;
						texc.v = (texcoord+lid)->v;

						pData->texclist[j][pData->trilist[k].vertIndex[m]] = texc;
					}
				}
			}

			// Vertex와 Normal 복사
			pData->morphtarget_num = RpGeometryGetNumMorphTargets (lGeom);
			
			pData->vertlist = new RwV3d*[pData->morphtarget_num];
			for(t =0;t<pData->morphtarget_num;++t)
				pData->vertlist[t] = new RwV3d[numvert];

			if(bNormal)
			{
				pData->normallist = new RwV3d*[pData->morphtarget_num];
				for(t =0;t<pData->morphtarget_num;++t)
					pData->normallist[t] = new RwV3d[numvert];
			}

			for(j=0;j<pData->morphtarget_num;++j)
			{
				RpMorphTarget* lMorph = RpGeometryGetMorphTarget(lGeom,j);
				RwV3d* lVert = RpMorphTargetGetVertices(lMorph);
				
				for(RwInt32 k=0;k<numTriList[g_AtomicCount+i];++k)
				{
					for(RwInt32 m=0;m<3;++m)
					{
						RwV3d	vert;

						RwInt32 lid = tmap[i][pData->trilist[k].vertIndex[m]];
						vert.x = (lVert+lid)->x;vert.y = (lVert+lid)->y;vert.z = (lVert+lid)->z;
						pData->vertlist[j][pData->trilist[k].vertIndex[m]] = vert;
					}
				}

				if(bNormal)
				{
					RwV3d* lNormal = RpMorphTargetGetVertexNormals(lMorph); 
					for(RwInt32 k=0;k<numTriList[g_AtomicCount+i];++k)
					{
						for(RwInt32 m=0;m<3;++m)
						{
							RwV3d	normal;

							RwInt32 lid = tmap[i][pData->trilist[k].vertIndex[m]];
							normal.x = (lNormal+lid)->x;normal.y = (lNormal+lid)->y;normal.z = (lNormal+lid)->z;
							pData->normallist[j][pData->trilist[k].vertIndex[m]] = normal;
						}
					}
				}
			}

			if(bPreLit)
			{
				RwRGBA* lColor = RpGeometryGetPreLightColors(lGeom);
				pData->prelitlist = new RwRGBA[numvert];

				for(RwInt32 k=0;k<numTriList[g_AtomicCount+i];++k)
				{
					for(RwInt32 m=0;m<3;++m)
					{
						RwInt32 lid = tmap[i][pData->trilist[k].vertIndex[m]];

						RwRGBA	color;
						color.red = (lColor+lid)->red; color.green = (lColor+lid)->green; 
						color.blue = (lColor+lid)->blue; color.alpha = (lColor+lid)->alpha;

						pData->prelitlist[k].red = color.red;
						pData->prelitlist[k].green = color.green;
						pData->prelitlist[k].blue = color.blue;
						pData->prelitlist[k].alpha = color.alpha;
					}
				}
			}
		}

		for(i=0;i<matnum;++i)
		{
			pData = GetAData(g_AtomicCount+i);
			RpMaterial*  lMat = RpGeometryGetMaterial(lGeom, i);
			pData->pmaterial = lMat;
		}

		for(i=0;i<Split_Capacity;++i)
			delete []tmap[i];
		delete []tmap;
			
		delete []tmapIndex;
		delete []numTriList;
	}

	g_AtomicCount += matnum;

	return NULL;
}

RpAtomic* RearrangeLODAtomicCB(RpAtomic *atomic, void *data)
{
//	RpLODAtomicForAllLODGeometries(atomic, CBSetGeometryMaterialAlphaFlagOff, NULL);
	
	return atomic;
}

RpGeometry* RearrangeLODGeometry(RpGeometry *geometry, void *data) 
{







	return geometry;
}

RpClump* RearrangeClump(RpClump* src, int nLodLevel)
{
	g_AtomicCount = 0;
	if(nLodLevel == 0)
		RpClumpForAllAtomics(src,RearrangeAtomicCB,NULL);	
	else
		RpClumpForAllAtomics(src,RearrangeLODAtomicCB,NULL);

	// List에 포함된 정보로 새로운 Clump를 만든다
	RpClump* NewClump;
	RwFrame *frame;
    
	NewClump = RpClumpCreate();
    frame = RwFrameCreate();
    RpClumpSetFrame(NewClump, frame);

	for(RwInt32 i = 0;i<g_AtomicCount;++i)
	{
		AtomicData*		pData = GetAData(i);

		RpAtomic *atomic;
		RpGeometry *geometry;
		RpMorphTarget *morphTarget;
		RwV3d *vlist, *nlist;
		RpTriangle *tlist;
		RwTexCoords *texCoord;

		RwBool bPreLit = (pData->geom_flag & rpGEOMETRYPRELIT) ? TRUE : FALSE;
		RwBool bNormal = (pData->geom_flag & rpGEOMETRYNORMALS) ? TRUE : FALSE;

		RpMaterial *Material;
		Material = RpMaterialClone(pData->pmaterial);

		geometry = RpGeometryCreate(pData->vert_num, pData->tri_num, 
        pData->geom_flag);
		if( geometry == NULL )
		{
	        return NULL;
	    }

		tlist = RpGeometryGetTriangles(geometry);
		
		for(RwInt32 j=0;j<pData->tri_num;++j)
		{
			RpGeometryTriangleSetVertexIndices(geometry, tlist, 
           pData->trilist[j].vertIndex[0], pData->trilist[j].vertIndex[1], (RwUInt16)pData->trilist[j].vertIndex[2]);

			RpGeometryTriangleSetMaterial(geometry, tlist++, Material);
		}

		if(pData->morphtarget_num > 1)
			RpGeometryAddMorphTargets(geometry,pData->morphtarget_num-1);
		
		for(j = 0;j<pData->morphtarget_num;++j)
		{
			morphTarget = RpGeometryGetMorphTarget(geometry, j);
			vlist = RpMorphTargetGetVertices(morphTarget);
			
			for(RwInt32 k=0;k<pData->vert_num;++k)
			{
				vlist->x = pData->vertlist[j][k].x;
				vlist->y = pData->vertlist[j][k].y;
				vlist->z = pData->vertlist[j][k].z;

				++vlist;
			}
			RwSphere boundingSphere;

			RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
			RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);
		}

		if(bNormal)
		{
			for(j = 0;j<pData->morphtarget_num;++j)
			{
				morphTarget = RpGeometryGetMorphTarget(geometry, j);
				nlist = RpMorphTargetGetVertexNormals(morphTarget);
			
				for(RwInt32 k=0;k<pData->vert_num;++k)
				{
					nlist->x = pData->normallist[j][k].x;
					nlist->y = pData->normallist[j][k].y;
					nlist->z = pData->normallist[j][k].z;
	
					++nlist;
				}
			}
		}

		if(bPreLit)
		{
			RwRGBA* color = RpGeometryGetPreLightColors  (geometry);
			for(RwInt32 k=0;k<pData->vert_num;++k)
			{
				color->red = pData->prelitlist[k].red;
				color->green = pData->prelitlist[k].green;
				color->blue = pData->prelitlist[k].blue;
				color->alpha = pData->prelitlist[k].alpha;
				++color;
			}
		}

		for(j = 0;j<pData->texc_num;++j)
		{
			texCoord = RpGeometryGetVertexTexCoords(geometry,(enum RwTextureCoordinateIndex)(j+1));
			for(RwInt32 k=0;k<pData->vert_num;++k)
			{
				texCoord->u = pData->texclist[j][k].u;
				texCoord->v = pData->texclist[j][k].v;
				++texCoord;
			}
		}

		RpGeometryUnlock(geometry);

		atomic = RpAtomicCreate();
		RpAtomicSetFlags(atomic,pData->atomic_flag);
		frame = RwFrameCreate();
		RpAtomicSetFrame(atomic, frame);

		RpAtomicSetGeometry(atomic, geometry, 0);
	    RpClumpAddAtomic(NewClump, atomic);
		RwFrameAddChild(RpClumpGetFrame(NewClump), frame);
		
		RpGeometryDestroy(geometry);
		RpMaterialDestroy(Material);
	}

	RpClumpDestroy(src);

	ReleaseADataAll();

	return NewClump;
}

AtomicData*		GetAData(int index) // 없으면 만든다
{
	AtomicData*	cur_data =	g_listAData;

	while(cur_data && index-- >0)
	{
		cur_data = cur_data->next;
	}

	if(cur_data == NULL) // 없었으므로 만든다.
	{
		while(index-- >= 0)
		{
			AtomicData* nw_AD = new AtomicData;
			ClearAData(nw_AD);
			
			cur_data =	g_listAData;
			if(cur_data)
			{
				while(cur_data->next) cur_data = cur_data->next;
				cur_data->next = nw_AD;		// 후미 추가
			}
			else
			{
				g_listAData = nw_AD;
			}

			cur_data = nw_AD;
		}
	}

	return cur_data;
}

void	ClearAData(AtomicData* pData)
{
	pData->atomic_flag = 0;
	pData->geom_flag = 0;
	pData->morphtarget_num = 0;
	pData->next = NULL;
	pData->normallist = NULL;
	pData->pmaterial = NULL;
	pData->prelitlist = NULL;
	pData->texc_num = 0;
	pData->texclist = NULL;
	pData->tri_num = 0;
	pData->trilist = NULL;
	pData->vert_num = 0;
	pData->vertlist = NULL;
}

void ReleaseADataAll()
{
	AtomicData*	cur_data =	g_listAData;
	int i;

	while(cur_data)
	{
		AtomicData* rdata = cur_data;
		cur_data = cur_data->next;

		for(i=0;i<rdata->morphtarget_num;++i)
		{
			delete []rdata->vertlist[i];
			delete []rdata->normallist[i];
		}

		delete []rdata->vertlist;
		delete []rdata->normallist;

		for(i=0;rdata->texc_num;++i)
			delete []rdata->texclist[i];

		delete []rdata->texclist;
		
		delete []rdata->trilist;
		delete []rdata->prelitlist;

		delete rdata;
	}
}
*/

AtomicData		g_alist[MAX_ATOMIC];
RwInt32			g_AtomicCount;

RpAtomic* RearrangeAtomicCB(RpAtomic *atomic, void *data) 
{
	RpGeometry* lGeom = RpAtomicGetGeometry (atomic);
	RwInt32		matnum = RpGeometryGetNumMaterials(lGeom);
	RwInt32		numvert = RpGeometryGetNumVertices(lGeom); 
	RwInt32		i,j,numtri;

	RwBool		bPreLit,bNormal;		//	Geometry 포맷 체크

	RwUInt32	lGeomFlags = RpGeometryGetFlags(lGeom);
	bPreLit = (lGeomFlags & rpGEOMETRYPRELIT) ? TRUE : FALSE;
	bNormal = (lGeomFlags & rpGEOMETRYNORMALS) ? TRUE : FALSE;

	RwUInt32	lAtomicFlags = RpAtomicGetFlags(atomic);

	if(matnum == 1)		// 분류없이 복사한다.
	{
		RpTriangle* ltriList = RpGeometryGetTriangles(lGeom);
		numtri = RpGeometryGetNumTriangles(lGeom);
		for(i=0;i<numtri;++i)
		{
			g_alist[g_AtomicCount].trilist[i].vertIndex[0] = ltriList->vertIndex[0];
			g_alist[g_AtomicCount].trilist[i].vertIndex[1] = ltriList->vertIndex[1];
			g_alist[g_AtomicCount].trilist[i].vertIndex[2] = ltriList->vertIndex[2];
			g_alist[g_AtomicCount].trilist[i].matIndex = ltriList->matIndex;
		
			++ltriList;
		}

		RpMaterial*  lMat = RpGeometryGetMaterial(lGeom, 0);
		RpMatFXMaterialFlags  lMatFxFlag =
			RpMatFXMaterialGetEffects (lMat);

		if(lMatFxFlag == rpMATFXEFFECTNULL)
			g_alist[g_AtomicCount].texc_num = 1;
		else
			g_alist[g_AtomicCount].texc_num = RpGeometryGetNumTexCoordSets (lGeom);
		
		for(i=0;i<g_alist[g_AtomicCount].texc_num;++i)
		{
			 RwTexCoords* texcoord =  RpGeometryGetVertexTexCoords (lGeom,(enum RwTextureCoordinateIndex)(i+1));
			
			 for(j=0;j<numvert;++j)
			 {
				g_alist[g_AtomicCount].texclist[i][j].u = texcoord->u;
				g_alist[g_AtomicCount].texclist[i][j].v = texcoord->v;
		
				++texcoord;
			 }
		}

		g_alist[g_AtomicCount].morphtarget_num = RpGeometryGetNumMorphTargets (lGeom);
		for(i=0;i<g_alist[g_AtomicCount].morphtarget_num;++i)
		{
			RpMorphTarget* lMorph = RpGeometryGetMorphTarget(lGeom,i);
			RwV3d* lVert = RpMorphTargetGetVertices(lMorph);
			
			for(j=0;j<numvert;++j)
			{
				g_alist[g_AtomicCount].vertlist[i][j].x = lVert->x;
				g_alist[g_AtomicCount].vertlist[i][j].y = lVert->y;
				g_alist[g_AtomicCount].vertlist[i][j].z = lVert->z;
				++lVert;
			}

			if(bNormal)
			{
				RwV3d* lNormal = RpMorphTargetGetVertexNormals(lMorph); 

				for(j=0;j<numvert;++j)
				{
					g_alist[g_AtomicCount].normallist[i][j].x = lNormal->x;
					g_alist[g_AtomicCount].normallist[i][j].y = lNormal->y;
					g_alist[g_AtomicCount].normallist[i][j].z = lNormal->z;
					++lNormal;
				}
			}
		}

		if(bPreLit)
		{
			RwRGBA* lColor = RpGeometryGetPreLightColors(lGeom);
				
			for(i=0;i<numvert;++i)
			{
				g_alist[g_AtomicCount].prelitlist[i].red = lColor->red;
				g_alist[g_AtomicCount].prelitlist[i].green = lColor->green;
				g_alist[g_AtomicCount].prelitlist[i].blue = lColor->blue;
				g_alist[g_AtomicCount].prelitlist[i].alpha = lColor->alpha;
							
				++lColor;
			}
		}

		g_alist[g_AtomicCount].atomic_flag = lAtomicFlags;
		g_alist[g_AtomicCount].geom_flag = lGeomFlags;
		g_alist[g_AtomicCount].pmaterial = lMat;
	}
	else	// atomic분리
	{
		RwInt16		tmap[MAX_ATOMIC][MAX_TRI_NUM];
		RwInt16		tmapIndex[MAX_ATOMIC];
		for(i=0;i<MAX_ATOMIC;++i) tmapIndex[i] = 0;

		RpTriangle* ltriList = RpGeometryGetTriangles(lGeom);
		numtri = RpGeometryGetNumTriangles(lGeom);
		
		RwInt32 numTriList[MAX_ATOMIC];
		for(i=0;i<MAX_ATOMIC;++i) numTriList[i] = 0;

		for(i=0;i<numtri;++i)
		{
			RwInt32		matid = ltriList->matIndex;
			RwInt32		k;
			
			for(j=0;j<3;++j)
			{
				RwBool		bFind = FALSE;
				for(k=0;k<tmapIndex[matid];++k)
				{
					if(tmap[matid][k] == ltriList->vertIndex[j])
					{
						bFind = TRUE;
						break;
					}
				}

				if(!bFind)
				{
					tmap[matid][tmapIndex[matid]] = ltriList->vertIndex[j];
					++tmapIndex[matid];
				}
				
				g_alist[g_AtomicCount+matid].trilist[numTriList[g_AtomicCount+matid]].vertIndex[j] = k;
			}

			numTriList[g_AtomicCount+matid] += 1;
			g_alist[g_AtomicCount+matid].trilist[i].matIndex = ltriList->matIndex;
			++ltriList;
		}

		for(i=0;i<matnum;++i)
		{
			RpMaterial*  lMat = RpGeometryGetMaterial(lGeom, i);
			RpMatFXMaterialFlags  lMatFxFlag =
				RpMatFXMaterialGetEffects (lMat);

			g_alist[g_AtomicCount+i].tri_num = numTriList[g_AtomicCount+i];
			g_alist[g_AtomicCount+i].vert_num = tmapIndex[i];

			g_alist[g_AtomicCount+i].atomic_flag = lAtomicFlags;
			g_alist[g_AtomicCount+i].geom_flag = lGeomFlags;

			// Texture Coord 복사
			if(lMatFxFlag == rpMATFXEFFECTNULL)
			{
				g_alist[g_AtomicCount+i].texc_num = 1;
				if(lGeomFlags & rpGEOMETRYTEXTURED2)		// 텍스쳐 좌표2개 이상 flag시 1개 플래그로 set
				{
					RwInt32 mask = 0x0;
					mask |= rpGEOMETRYTEXTURED2;
					mask = ~mask;
					lGeomFlags &= mask;
					lGeomFlags |= rpGEOMETRYTEXTURED;
				}
			}
			else
				g_alist[g_AtomicCount+i].texc_num = RpGeometryGetNumTexCoordSets (lGeom);

			for(j=0;j<g_alist[g_AtomicCount+i].texc_num;++j)
			{
				RwTexCoords* texcoord =  RpGeometryGetVertexTexCoords (lGeom,(enum RwTextureCoordinateIndex)(j+1));
			
				for(RwInt32 k=0;k<numTriList[g_AtomicCount+i];++k)
				{
					for(RwInt32 m=0;m<3;++m)
					{
						RwTexCoords  texc;

						RwInt32 lid = tmap[i][g_alist[g_AtomicCount+i].trilist[k].vertIndex[m]];
						texc.u = (texcoord+lid)->u;
						texc.v = (texcoord+lid)->v;

						g_alist[g_AtomicCount+i].texclist[j][g_alist[g_AtomicCount+i].trilist[k].vertIndex[m]] = texc;
					}
				}
			}

			// Vertex와 Normal 복사
			g_alist[g_AtomicCount+i].morphtarget_num = RpGeometryGetNumMorphTargets (lGeom);
			for(j=0;j<g_alist[g_AtomicCount+i].morphtarget_num;++j)
			{
				RpMorphTarget* lMorph = RpGeometryGetMorphTarget(lGeom,j);
				RwV3d* lVert = RpMorphTargetGetVertices(lMorph);
				
				for(RwInt32 k=0;k<numTriList[g_AtomicCount+i];++k)
				{
					for(RwInt32 m=0;m<3;++m)
					{
						RwV3d	vert;

						RwInt32 lid = tmap[i][g_alist[g_AtomicCount+i].trilist[k].vertIndex[m]];
						vert.x = (lVert+lid)->x;vert.y = (lVert+lid)->y;vert.z = (lVert+lid)->z;
						g_alist[g_AtomicCount+i].vertlist[j][g_alist[g_AtomicCount+i].trilist[k].vertIndex[m]] = vert;
					}
				}

				if(bNormal)
				{
					RwV3d* lNormal = RpMorphTargetGetVertexNormals(lMorph); 
					for(RwInt32 k=0;k<numTriList[g_AtomicCount+i];++k)
					{
						for(RwInt32 m=0;m<3;++m)
						{
							RwV3d	normal;

							RwInt32 lid = tmap[i][g_alist[g_AtomicCount+i].trilist[k].vertIndex[m]];
							normal.x = (lNormal+lid)->x;normal.y = (lNormal+lid)->y;normal.z = (lNormal+lid)->z;
							g_alist[g_AtomicCount+i].normallist[j][g_alist[g_AtomicCount+i].trilist[k].vertIndex[m]] = normal;
						}
					}
				}
			}

			if(bPreLit)
			{
				RwRGBA* lColor = RpGeometryGetPreLightColors(lGeom);

				for(RwInt32 k=0;k<numTriList[g_AtomicCount+i];++k)
				{
					for(RwInt32 m=0;m<3;++m)
					{
						RwInt32 lid = tmap[i][g_alist[g_AtomicCount+i].trilist[k].vertIndex[m]];

						RwRGBA	color;
						color.red = (lColor+lid)->red; color.green = (lColor+lid)->green; 
						color.blue = (lColor+lid)->blue; color.alpha = (lColor+lid)->alpha;

  						g_alist[g_AtomicCount+i].prelitlist[k].red = color.red;
						g_alist[g_AtomicCount+i].prelitlist[k].green = color.green;
						g_alist[g_AtomicCount+i].prelitlist[k].blue = color.blue;
						g_alist[g_AtomicCount+i].prelitlist[k].alpha = color.alpha;
					}
				}
			}
		}

		for(i=0;i<matnum;++i)
		{
			RpMaterial*  lMat = RpGeometryGetMaterial(lGeom, i);
			g_alist[g_AtomicCount+i].pmaterial = lMat;
		}
	}

	g_AtomicCount += matnum;

	return NULL;
}

RpClump* RearrangeClump(RpClump* src,int nLodLevel)
{
	g_AtomicCount = 0;	
	RpClumpForAllAtomics(src,RearrangeAtomicCB,NULL);	

	// List에 포함된 정보로 새로운 Clump를 만든다
	RpClump* NewClump;
	RwFrame *frame;
    
	NewClump = RpClumpCreate();
    frame = RwFrameCreate();
    RpClumpSetFrame(NewClump, frame);

	for(RwInt32 i = 0;i<g_AtomicCount;++i)
	{
		RpAtomic *atomic;
		RpGeometry *geometry;
		RpMorphTarget *morphTarget;
		RwV3d *vlist, *nlist;
		RpTriangle *tlist;
		RwTexCoords *texCoord;

		RwBool bPreLit = (g_alist[i].geom_flag & rpGEOMETRYPRELIT) ? TRUE : FALSE;
		RwBool bNormal = (g_alist[i].geom_flag & rpGEOMETRYNORMALS) ? TRUE : FALSE;

		RpMaterial *Material;
		Material = RpMaterialClone(g_alist[i].pmaterial);

		geometry = RpGeometryCreate(g_alist[i].vert_num, g_alist[i].tri_num, 
        g_alist[i].geom_flag);
		if( geometry == NULL )
		{
	        return NULL;
	    }

		tlist = RpGeometryGetTriangles(geometry);
		
		for(RwInt32 j=0;j<g_alist[i].tri_num;++j)
		{
			RpGeometryTriangleSetVertexIndices(geometry, tlist, 
            g_alist[i].trilist[j].vertIndex[0], g_alist[i].trilist[j].vertIndex[1], (RwUInt16)g_alist[i].trilist[j].vertIndex[2]);

			RpGeometryTriangleSetMaterial(geometry, tlist++, Material);
		}

		if(g_alist[i].morphtarget_num > 1)
			RpGeometryAddMorphTargets(geometry,g_alist[i].morphtarget_num-1);
		
		for(j = 0;j<g_alist[i].morphtarget_num;++j)
		{
			morphTarget = RpGeometryGetMorphTarget(geometry, j);
			vlist = RpMorphTargetGetVertices(morphTarget);
			
			for(RwInt32 k=0;k<g_alist[i].vert_num;++k)
			{
				vlist->x = g_alist[i].vertlist[j][k].x;
				vlist->y = g_alist[i].vertlist[j][k].y;
				vlist->z = g_alist[i].vertlist[j][k].z;

				++vlist;
			}
			RwSphere boundingSphere;

			RpMorphTargetCalcBoundingSphere(morphTarget, &boundingSphere);
			RpMorphTargetSetBoundingSphere(morphTarget, &boundingSphere);
		}

		if(bNormal)
		{
			for(j = 0;j<g_alist[i].morphtarget_num;++j)
			{
				morphTarget = RpGeometryGetMorphTarget(geometry, j);
				nlist = RpMorphTargetGetVertexNormals(morphTarget);
			
				for(RwInt32 k=0;k<g_alist[i].vert_num;++k)
				{
					nlist->x = g_alist[i].normallist[j][k].x;
					nlist->y = g_alist[i].normallist[j][k].y;
					nlist->z = g_alist[i].normallist[j][k].z;
	
					++nlist;
				}
			}
		}

		if(bPreLit)
		{
			RwRGBA* color = RpGeometryGetPreLightColors  (geometry);
			for(RwInt32 k=0;k<g_alist[i].vert_num;++k)
			{
				color->red = g_alist[i].prelitlist[k].red;
				color->green = g_alist[i].prelitlist[k].green;
				color->blue = g_alist[i].prelitlist[k].blue;
				color->alpha = g_alist[i].prelitlist[k].alpha;
				++color;
			}
		}

		for(j = 0;j<g_alist[i].texc_num;++j)
		{
			texCoord = RpGeometryGetVertexTexCoords(geometry,(enum RwTextureCoordinateIndex)(j+1));
			for(RwInt32 k=0;k<g_alist[i].vert_num;++k)
			{
				texCoord->u = g_alist[i].texclist[j][k].u;
				texCoord->v = g_alist[i].texclist[j][k].v;
				++texCoord;
			}
		}

		RpGeometryUnlock(geometry);

		atomic = RpAtomicCreate();
		RpAtomicSetFlags(atomic,g_alist[i].atomic_flag);
		frame = RwFrameCreate();
		RpAtomicSetFrame(atomic, frame);

		RpAtomicSetGeometry(atomic, geometry, 0);
	    RpClumpAddAtomic(NewClump, atomic);
		RwFrameAddChild(RpClumpGetFrame(NewClump), frame);
		
		RpGeometryDestroy(geometry);
		RpMaterialDestroy(Material);
	}

	RpClumpDestroy(src);

	return NewClump;
}
