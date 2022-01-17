#include "RWUtil.h"

/******************************************************************************
* Desc :
******************************************************************************/
RtCharset *RwUtilPrintText(RtCharset* charset, RwChar* string, RwInt32 x, RwInt32 y, RsPrintPos pos)
{
	RtCharsetDesc charsetDesc;
    RwUInt32  width,   height;
    RwUInt32  strLen;
    RwRaster *camRas;

    RSASSERT(pos >= rsPRINTPOSMIDDLE);
    RSASSERT(pos <= rsPRINTPOSBOTTOMRIGHT);
    RSASSERT(!((pos & rsPRINTPOSLEFT) && (pos & rsPRINTPOSRIGHT )));
    RSASSERT(!((pos & rsPRINTPOSTOP ) && (pos & rsPRINTPOSBOTTOM)));

    RSASSERT(NULL != RWSRCGLOBAL(curCamera));
    camRas = RwCameraGetRaster((RwCamera *)RWSRCGLOBAL(curCamera));
    RSASSERT(NULL != camRas);

    width  = RwRasterGetWidth(camRas);
    height = RwRasterGetHeight(camRas);

    RtCharsetGetDesc(charset, &charsetDesc);
    strLen = rwstrlen(string);

    x *= charsetDesc.width;
    y *= charsetDesc.height;

    /* Depending on where on-screen text is being displayed, we position
     * it inside a platform-dependent safety margin, which helps stop
     * text being lost of the side of TVs (i.e for consoles). x and y
     * are now just offsets from the position we calc, used to group
     * strings together. Yeah, it's not foolproof... but hey. You can
     * always use RtCharsetPrint() directly. */
    if (pos & rsPRINTPOSLEFT)
    {
        x += charsetDesc.width*rsPRINTMARGINLEFT;
    }
    else if (pos & rsPRINTPOSRIGHT)
    {
        x += width - charsetDesc.width*(strLen + rsPRINTMARGINRIGHT);
    }

    if ((rsPRINTPOSLEFT   == pos) ||
        (rsPRINTPOSRIGHT  == pos) ||
        (rsPRINTPOSMIDDLE == pos) )
    {
        /* Centre the string vertically */
        y += (height - charsetDesc.height) / 2;
    }

    if (pos & rsPRINTPOSTOP)
    {
        y += charsetDesc.height*rsPRINTMARGINTOP;
    }
    else if (pos & rsPRINTPOSBOTTOM)
    {
        y += height - charsetDesc.height*(1 + rsPRINTMARGINBOTTOM);
    }

    if ((rsPRINTPOSTOP    == pos) ||
        (rsPRINTPOSBOTTOM == pos) ||
        (rsPRINTPOSMIDDLE == pos) )
    {
        /* Centre the string horizontally */
        x += (width - charsetDesc.width*strLen) / 2;
    }

    return(RtCharsetPrint(charset, string, x, y));
}

/******************************************************************************
* Desc :
******************************************************************************/
VOID RwUtilDebugMessageHandler(RwDebugType type __RWUNUSED__, const RwChar *str)
{
    OutputDebugString(str);
    OutputDebugString(RWSTRING("\n"));
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilDebugMessage(RwChar* strFunc, RwChar* strMsg)
{
	RwDebugSendMessage(rwDEBUGMESSAGE, strFunc, strMsg);
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpWorld *RwUtilCreateEmptyWorld(RwBBox* box)
{
	RpWorld *world;

	if(!box)
	{
		RwBBox bb;

		bb.inf.x = bb.inf.y = bb.inf.z = -100.0f;
		bb.sup.x = bb.sup.y = bb.sup.z = 100.0f;

		world = RpWorldCreate(&bb);
	}
	else
	{
		world = RpWorldCreate(box);
	}

    return world;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpLight *RwUtilCreateAmbientLight(RwRGBAReal color, RpWorld* world)
{
	RpLight* light;

	light = RpLightCreate(rpLIGHTAMBIENT);

	if(light)
	{
		RpLightSetColor(light, &color);

		if(world)
		{
			RpWorldAddLight(world, light);
		}
	}

	return light;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpLight *RwUtilCreateMainLight(RwRGBAReal color, RpWorld *world)
{
    RpLight *light;

    light = RpLightCreate(rpLIGHTDIRECTIONAL);
    if(light)
    {
		RpLightSetColor(light, &color);

        RwFrame *frame = RwFrameCreate();
        if(frame)
        {
            RwV3d yAxis = {0.0f, 1.0f, 0.0f};
			RwV3d xAxis = {1.0f, 0.0f, 0.0f};

            RwFrameRotate(frame, &yAxis, 225.0f, rwCOMBINEREPLACE);
			RwFrameRotate(frame, &xAxis, -45.0f, rwCOMBINEPOSTCONCAT);
            RpLightSetFrame(light, frame);

			if(world)
			{
				RpWorldAddLight(world, light);
			}

            return light;
        }

        RpLightDestroy(light);
    }

    return NULL;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpLight *RwUtilCreateDirectionalLight(RwReal x_rad, RwReal y_rad, RwRGBAReal color, RpWorld* world)
{
    RpLight* light;

	light = RpLightCreate(rpLIGHTDIRECTIONAL);
	if(light)
	{
		RwFrame *frame = RwFrameCreate();
		if(frame)
		{
			RwFrameRotate(frame, &RWUTIL_XAXIS, x_rad, rwCOMBINEREPLACE);
            RwFrameRotate(frame, &RWUTIL_YAXIS, y_rad, rwCOMBINEPOSTCONCAT);

			RpLightSetFrame(light, frame);
			RpLightSetColor(light, &color);
			
			if(world)
				RpWorldAddLight(world, light);
		}

		RpLightDestroy(light);
	}

	return light;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpLight *RwUtilDestroyLightCB(RpLight* light, void* data)
{
    RwFrame *frame;

    RpWorldRemoveLight((RpWorld*)(data), light);

    frame = RpLightGetFrame(light);
    if(frame)
    {
        RpLightSetFrame(light, NULL);
        RwFrameDestroy(frame);
    }

    RpLightDestroy(light);

    return light;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpClump *RwUtilCreateClump(CHAR *szDffPath, RpWorld *world)
{
	RpClump  *clump		= NULL;
	RwStream *stream	= NULL;

	stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, szDffPath);

	if(stream)
	{
		if(RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL)) 
        {
            clump = RpClumpStreamRead(stream);
        }

        RwStreamClose(stream, NULL);
	}

	if((world) && (clump))
	{
		RpWorldAddClump(world, clump);
	}

	return clump;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilClumpGetBoundingSphere(RpClump *pstClump, RwSphere *pstSphere)
{
//@{ 2006/05/16 burumal

//	RwInt32 numAnimSamples;
//
//    /*
//     * First find the mean of all the atomics' bounding sphere centers.
//     * All morph targets of all atomics and all frame animations are taken into account.
//     * The result is the clump's bounding sphere center...
//     */
//    pstSphere->center = RWUTIL_ZEROVEC;
//    numAnimSamples = 0;
//
//    {
//        RwSphere curClumpSphere;
//
//        /*
//         * average over morph targets and atomics
//         */
//        curClumpSphere.center = RWUTIL_ZEROVEC;
//        RpClumpForAllAtomics(pstClump, RwUtilAtomicAddBSphereCentreCB, &curClumpSphere);
//
//        RwV3dScale(&curClumpSphere.center, 
//            &curClumpSphere.center, 1.0f / RpClumpGetNumAtomics (pstClump));
//
//        /*
//         * Sum up the above average in order to calculate the overall
//         * average over the frame animation...
//         */
//        RwV3dAdd(&pstSphere->center,
//            &pstSphere->center, &curClumpSphere.center);
//
//        numAnimSamples++;
//
//    }
//
//    RwV3dScale(&pstSphere->center, &pstSphere->center, 1.0f / numAnimSamples);
//
//    /*
//     * Now, given the clump's bounding sphere center, determine the radius
//     * by finding the greatest distance from the center that encloses all
//     * the atomics' bounding spheres.  All morph targets, atomics and animations
//     * are taken into account
//     */
//    pstSphere->radius = -RwRealMAXVAL;
//    numAnimSamples = 0;
//
//    RpClumpForAllAtomics(pstClump, RwUtilAtomicCompareBSphereCB, pstSphere);
//
//    numAnimSamples++;

	RpAtomic* pAtomic = pstClump->atomicList;
	RpAtomic* pEndAtomic = pAtomic;

	INT32 nBSphereCount = 0;
	RwSphere EachBSphere;

	pstSphere->center.x = pstSphere->center.y = pstSphere->center.z = 0;
	pstSphere->radius = 0;

	// calc center position
	do {
		// transform 된 좌표로 구해준다
		RwUtilCalcSphere(pAtomic, &EachBSphere);

		pstSphere->center.x += EachBSphere.center.x;
		pstSphere->center.y += EachBSphere.center.y;
		pstSphere->center.z += EachBSphere.center.z;

		nBSphereCount++;

		ASSERT(pAtomic->next != NULL);

		pAtomic = pAtomic->next;

	} while (pAtomic != pEndAtomic);

	pstSphere->center.x /= nBSphereCount;
	pstSphere->center.y /= nBSphereCount;
	pstSphere->center.z /= nBSphereCount;

	// calc radius value
	do {

		// 이렇게 하면 간단하게 구할수 있지만 정확하지 않다
		//		RwUtilCalcSphere(pAtomic, &EachBSphere);
		//		float fDist = AUPOS_DISTANCE(pstSphere->center, EachBSphere.center);
		//		fDist += EachBSphere.radius;
		//		if ( fDist > pstSphere->radius )
		//			pstSphere->radius = fDist;

		RpGeometry* pGeometry = RpAtomicGetGeometry(pAtomic);
		if ( pGeometry )
		{
			INT32 nMorphTargetCount = RpGeometryGetNumMorphTargets(pGeometry);
			INT32 nVertCount = RpGeometryGetNumVertices(pGeometry);

			RwMatrix* pTransformMat = RwFrameGetLTM(RpAtomicGetFrame(pAtomic));
			ASSERT(pTransformMat);

			RwV3d vGeoMax, vGeoMin;

			RpMorphTarget* pMorphTarget = NULL;
			RwV3d* pVerts = NULL;

			pMorphTarget = RpGeometryGetMorphTarget(pGeometry, 0);
			if ( pMorphTarget )
				pVerts = RpMorphTargetGetVertices(pMorphTarget);

			if ( pMorphTarget && pVerts )
			{
				{
					RwV3d vTransformed;
					RwV3dTransformPoint(&vTransformed, &(pVerts[0]), pTransformMat);

					vGeoMax.x = vGeoMin.x = vTransformed.x;
					vGeoMax.y = vGeoMin.y = vTransformed.y;
					vGeoMax.z = vGeoMin.z = vTransformed.z;
				}

				for ( INT32 nIdx = 0; nIdx < nMorphTargetCount; nIdx++ )
				{
					pMorphTarget = RpGeometryGetMorphTarget(pGeometry, nIdx);
					if ( !pMorphTarget )
						break;

					pVerts = RpMorphTargetGetVertices(pMorphTarget);
					if ( !pVerts )
						break;

					RwV3d vMax, vMin;

					{
						RwV3d vTransformed;
						RwV3dTransformPoint(&vTransformed, &(pVerts[0]), pTransformMat);

						vMax.x = vMin.x = vTransformed.x;
						vMax.y = vMin.y = vTransformed.y;
						vMax.z = vMin.z = vTransformed.z;
					}

					for ( INT32 nVert = 0; nVert < nVertCount; nVert++ )
					{
						{
							RwV3d vTransformed;
							RwV3dTransformPoint(&vTransformed, &(pVerts[nVert]), pTransformMat);

							// max
							if ( vMax.x < vTransformed.x )
								vMax.x = vTransformed.x;

							if ( vMax.y < vTransformed.y )
								vMax.y = vTransformed.y;

							if ( vMax.z < vTransformed.z )
								vMax.z = vTransformed.z;

							// min
							if ( vMin.x > vTransformed.x )
								vMin.x = vTransformed.x;

							if ( vMin.y > vTransformed.y )
								vMin.y = vTransformed.y;

							if ( vMin.z > vTransformed.z )
								vMin.z = vTransformed.z;
						}
					}

					// max
					if ( vGeoMax.x < vMax.x )
						vGeoMax.x = vMax.x;

					if ( vGeoMax.y < vMax.y )
						vGeoMax.y = vMax.y;

					if ( vGeoMax.z < vMax.z )
						vGeoMax.z = vMax.z;

					// min
					if ( vGeoMin.x > vMin.x )
						vGeoMin.x = vMin.x;

					if ( vGeoMin.y > vMin.y )
						vGeoMin.y = vMin.y;

					if ( vGeoMin.z > vMin.z )
						vGeoMin.z = vMin.z;
				}

				float fMaxDist = AUPOS_DISTANCE(pstSphere->center, vGeoMax);
				float fMinDist = AUPOS_DISTANCE(pstSphere->center, vGeoMin);

				if ( fMaxDist > fMinDist )
				{
					if ( pstSphere->radius < fMaxDist )
						pstSphere->radius = fMaxDist;
				}
				else
				{
					if ( pstSphere->radius < fMinDist )
						pstSphere->radius = fMinDist;
				}
			}
		}

		ASSERT(pAtomic->next != NULL);

		pAtomic = pAtomic->next;

	} while (pAtomic != pEndAtomic);

	RwMatrix invsLTM;
	//RwMatrixInvert( &invsLTM, RwFrameGetLTM( RpClumpGetFrame( pstClump ) ) );
	RwMatrixInvert( &invsLTM, RwFrameGetLTM( RpAtomicGetFrame( pAtomic) ) );
	RwV3dTransformPoint( &pstSphere->center, &pstSphere->center, &invsLTM );
	//@}
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilAtomicAddBSphereCentreCB(RpAtomic *atomic, void *data)
{
	RpGeometry *geometry;

    geometry = RpAtomicGetGeometry(atomic);

    if( geometry )
    {
        RwV3d center;
//        RwMatrix *LTM;
        RpMorphTarget *morphTarget;
        RwInt32 i, numMorphTargets;
        RwV3d atomicCentre;
        RwSphere *clumpSphere;

        clumpSphere = (RwSphere *)data;

        /*
         * Establish the average centre of this atomic over all morph targets
         */
        atomicCentre = RWUTIL_ZEROVEC;

        numMorphTargets = RpGeometryGetNumMorphTargets (geometry);

        for( i = 0; i < numMorphTargets; i++ )
        {
            morphTarget = RpGeometryGetMorphTarget(geometry, i);
            center = RpMorphTargetGetBoundingSphere(morphTarget)->center;
            RwV3dAdd(&atomicCentre, &atomicCentre, &center);
        }

        RwV3dScale(&atomicCentre, &atomicCentre, 1.0f / numMorphTargets);

        /*
         * Tranform the average centre of the atomic to world space
         */
//        LTM = RwFrameGetLTM(RpAtomicGetFrame(atomic));
  //      RwV3dTransformPoints(&atomicCentre, &atomicCentre, 1, LTM);

        /*
         * Add the average centre of the atomic up in order to calculate the centre of the clump
         */
        RwV3dAdd(&clumpSphere->center, &clumpSphere->center, &atomicCentre);
    }

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilAtomicCompareBSphereCB(RpAtomic *atomic, void *data)
{
	RpGeometry *geometry;

    geometry = RpAtomicGetGeometry(atomic);

    if( geometry )
    {
        RwSphere *sphere, morphTargetSphere;
        RwV3d tempVec;
        RpMorphTarget *morphTarget;
        RwReal dist;
        RwMatrix *LTM;
        RwInt32 i, numMorphTargets;

        sphere = (RwSphere *)data;

        LTM = RwFrameGetLTM(RpAtomicGetFrame(atomic));

        numMorphTargets = RpGeometryGetNumMorphTargets(geometry);

        for( i = 0; i < numMorphTargets; i++ )
        {
            morphTarget = RpGeometryGetMorphTarget(geometry, i);
            morphTargetSphere = *RpMorphTargetGetBoundingSphere(morphTarget);

//            RwV3dTransformPoints(&morphTargetSphere.center,
  //              &morphTargetSphere.center, 1, LTM);

            RwV3dSub(&tempVec, &morphTargetSphere.center, &sphere->center);

            dist = RwV3dLength(&tempVec) + morphTargetSphere.radius;
            if( dist > sphere->radius )
            {
                sphere->radius = dist;
            }
        }
    }

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwInt32 RwUtilGetAtomicUsrDataArrayInt32(RpAtomic *pstAtomic, CHAR *pszCmpUsrDatAryName)
{
	RpGeometry	*pGeom	= RpAtomicGetGeometry(pstAtomic);
	RwInt32		nRt		= -1;

	if(pGeom)
	{
		RwInt32 nUsrDatNum = RpGeometryGetUserDataArrayCount(pGeom);

		RpUserDataArray *pstUserDataArray;
		RwChar			*pszUserDataArrayName;

		for(RwInt32 nCount = 0; nCount < nUsrDatNum; ++nCount)
		{
			pstUserDataArray		= RpGeometryGetUserDataArray(pGeom, nCount);
			if(pstUserDataArray)
			{
				pszUserDataArrayName	= RpUserDataArrayGetName(pstUserDataArray);
				if(!strcmp(pszUserDataArrayName, pszCmpUsrDatAryName))
				{
					nRt = RpUserDataArrayGetInt(pstUserDataArray, 0);
					break;
				}
			}
		}
	}

	return nRt;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwV3d RwUtilMakeV3d(FLOAT x, FLOAT y, FLOAT z)
{
	RwV3d v3dRt = {x, y, z};

	return v3dRt;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilAtomicRenderWireMesh(RpAtomic *atomic, RwRGBA *Color)
{
    RpGeometry *geometry;

    geometry = RpAtomicGetGeometry(atomic);

    if( geometry )
    {
        RwReal interpPos = 1.0f, invInterpPos = 0.0f;
        RpMorphTarget *morphTarget;
        RwInt32 nkf;
        RpTriangle *triangle;
        RwV3d *vertPosStart = (RwV3d *)NULL;
        RwV3d *vertPosEnd = (RwV3d *)NULL;
        RwMatrix *LTM;
        RwInt32 numTri, numImmVert, i;
        RwIm3DVertex *imVertex;

        nkf = RpGeometryGetNumMorphTargets(geometry);
        numTri = RpGeometryGetNumTriangles(geometry);
        imVertex = (RwIm3DVertex *)RwMalloc(numTri * 6 * sizeof(RwIm3DVertex), rwMEMHINTDUR_GLOBAL);

        if( nkf > 1 )
        {
            RpInterpolator *interp;
            RwInt32 startMorphTarget, endMorphTarget;

            interp = RpAtomicGetInterpolator(atomic);

            interpPos = RpInterpolatorGetValue(interp) / RpInterpolatorGetScale(interp);

            invInterpPos = 1.0f - interpPos;

            startMorphTarget = RpInterpolatorGetStartMorphTarget(interp);
            endMorphTarget = RpInterpolatorGetEndMorphTarget(interp);

            morphTarget = RpGeometryGetMorphTarget(geometry, startMorphTarget);
            vertPosStart = RpMorphTargetGetVertices(morphTarget);

            morphTarget = RpGeometryGetMorphTarget(geometry, endMorphTarget);
            vertPosEnd = RpMorphTargetGetVertices(morphTarget);
        }
        else
        {
            morphTarget = RpGeometryGetMorphTarget(geometry, 0);
            vertPosStart = RpMorphTargetGetVertices(morphTarget);
        }

        triangle = RpGeometryGetTriangles(geometry);

        for( i = 0; i < numTri; i++ )
        {
            RwUInt16 vert0, vert1, vert2;
            RwV3d vertPos[3];

            RpGeometryTriangleGetVertexIndices(geometry, triangle, &vert0, &vert1, &vert2);

            if( nkf > 1 )
            {
                RwV3d tempVec1, tempVec2;

                RwV3dScale(&tempVec1, &vertPosStart[vert0], invInterpPos);
                RwV3dScale(&tempVec2, &vertPosEnd[vert0], interpPos);
                RwV3dAdd(&vertPos[0], &tempVec1, &tempVec2);

                RwV3dScale(&tempVec1, &vertPosStart[vert1], invInterpPos);
                RwV3dScale(&tempVec2, &vertPosEnd[vert1], interpPos);
                RwV3dAdd(&vertPos[1], &tempVec1, &tempVec2);

                RwV3dScale(&tempVec1, &vertPosStart[vert2], invInterpPos);
                RwV3dScale(&tempVec2, &vertPosEnd[vert2], interpPos);
                RwV3dAdd(&vertPos[2], &tempVec1, &tempVec2);

            }
            else
            {
                vertPos[0] = vertPosStart[vert0];
                vertPos[1] = vertPosStart[vert1];
                vertPos[2] = vertPosStart[vert2];
            }

            RwIm3DVertexSetPos(&imVertex[i*6+0], vertPos[0].x, vertPos[0].y, vertPos[0].z);
            RwIm3DVertexSetRGBA (&imVertex[i*6+0], Color->red, Color->green, Color->blue, Color->alpha);
            RwIm3DVertexSetPos(&imVertex[i*6+1], vertPos[1].x, vertPos[1].y, vertPos[1].z);
            RwIm3DVertexSetRGBA(&imVertex[i*6+1], Color->red, Color->green, Color->blue, Color->alpha);


            RwIm3DVertexSetPos(&imVertex[i*6+2], vertPos[1].x, vertPos[1].y, vertPos[1].z);
            RwIm3DVertexSetRGBA(&imVertex[i*6+2], Color->red, Color->green, Color->blue, Color->alpha);
            RwIm3DVertexSetPos(&imVertex[i*6+3], vertPos[2].x, vertPos[2].y, vertPos[2].z);
            RwIm3DVertexSetRGBA(&imVertex[i*6+3], Color->red, Color->green, Color->blue, Color->alpha);

            RwIm3DVertexSetPos(&imVertex[i*6+4], vertPos[2].x, vertPos[2].y, vertPos[2].z);
            RwIm3DVertexSetRGBA(&imVertex[i*6+4], Color->red, Color->green, Color->blue, Color->alpha);
            RwIm3DVertexSetPos(&imVertex[i*6+5], vertPos[0].x, vertPos[0].y, vertPos[0].z);
            RwIm3DVertexSetRGBA(&imVertex[i*6+5], Color->red, Color->green, Color->blue, Color->alpha);

            triangle++;
        }

        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void *)TRUE);
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void *)TRUE);

        i = 0;
        numImmVert = numTri * 6;
        LTM = RwFrameGetLTM(RpAtomicGetFrame(atomic));
//		{
/*			RwCamera* camera        = CRenderView::GetInstance()->GetRenderWare()->GetCamera();
			RwFrame*  camera_frame  = RwCameraGetFrame(camera);
			RwMatrix  camera_matrix = *(RwFrameGetMatrix(camera_frame));
			RwV3d     pos           = {0.0f, 0.0f, 200.0f};

			RwFrameTranslate(RwCameraGetFrame(camera), &pos, rwCOMBINEREPLACE);
			RwFrameRotate(RwCameraGetFrame(camera), &Yaxis, 180.0f, rwCOMBINEPRECONCAT);*/
//		}

//			LTM = RwFrameGetLTM(camera_frame);

        while (numImmVert > RWUTIL_DISPATCHSIZE)
        {
            if( RwIm3DTransform(&imVertex[i], RWUTIL_DISPATCHSIZE, LTM, 0) )
            {
                RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);

                RwIm3DEnd();
            }

            numImmVert -= RWUTIL_DISPATCHSIZE;
            i += RWUTIL_DISPATCHSIZE;
        }

        if( RwIm3DTransform(&imVertex[i], numImmVert, LTM, 0) )
        {
            RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);

            RwIm3DEnd();
        }

//		{
//			RwFrameTransform(camera_frame, &camera_matrix, rwCOMBINEREPLACE);
//		}

        RwFree(imVertex);
    }
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL RwUtilCopyClumpFile(RpClump *pstClump, CHAR *szPath)
{
	RwStream *pstStream;

	pstStream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, szPath);
	if(!pstStream)
		return FALSE;

	if(!RpClumpStreamWrite(pstClump, pstStream))
	{
		RwStreamClose(pstStream, NULL);
		return FALSE;
	}

	RwStreamClose(pstStream, NULL);

	return TRUE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwInt32	RwUtilClumpGetTriangles(RpClump *pstClump)
{
	RwInt32 lNum = 0;

	RpClumpForAllAtomics(pstClump, RwUtilAtomicGetTrianglesCB, (void *)(&lNum));

	return lNum;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwInt32 RwUtilClumpGetVertices(RpClump *pstClump)
{
	RwInt32 lNum = 0;

	RpClumpForAllAtomics(pstClump, RwUtilAtomicGetVerticesCB, (void *)(&lNum));

	return lNum;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilAtomicGetTrianglesCB(RpAtomic *atomic, void *data)
{
	RwInt32	*plTriNum = (RwInt32 *)(data);

	RpGeometry *pstGeom = RpAtomicGetGeometry(atomic);
	if(pstGeom)
	{
		(*(plTriNum)) += RpGeometryGetNumTriangles(pstGeom);
	}

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilAtomicGetVerticesCB(RpAtomic *atomic, void *data)
{
	RwInt32	*plVerNum = (RwInt32 *)(data);

	RpGeometry *pstGeom = RpAtomicGetGeometry(atomic);
	if(pstGeom)
	{
		(*(plVerNum)) += RpGeometryGetNumVertices(pstGeom);
	}

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpCollisionTriangle* RwUtilAtomicForAllTriangleIntersectionCB(RpIntersection *intersection __RWUNUSED__, RpCollisionTriangle *triangle, RwReal distance,  void *data)
{
	RwReal *minDistance = (RwReal *)(data);

    if(distance < *minDistance)
    {
        *minDistance = distance;
    }

    return triangle;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilWorldForAllAtomicIntersectLineCB(RpIntersection *intersection __RWUNUSED__, RpWorldSector *sector __RWUNUSED__, RpAtomic *atomic,  RwReal distance __RWUNUSED__, void *data)
{
	RwUtilAtomicIntersectParams	*intersectParams	= (RwUtilAtomicIntersectParams*)(data);
	RwReal						oldDistance			= intersectParams->minDistance;

	RpAtomicForAllIntersections(atomic, &intersectParams->intersection, RwUtilAtomicForAllTriangleIntersectionCB, &intersectParams->minDistance);

	if(intersectParams->minDistance < oldDistance)
    {
        intersectParams->pickedAtomic = atomic;
    }

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilPickWorldNearestAtomic(RwCamera *camera, RpWorld * world, RwV2d *pixel)
{
	RwUtilAtomicIntersectParams intersectParams;
    RwLine						pixelRay;

	RwCameraCalcPixelRay(camera, &pixelRay, pixel);

	intersectParams.intersection.t.line = pixelRay;
    intersectParams.intersection.type   = rpINTERSECTLINE;
    intersectParams.pickedAtomic        = NULL;
    intersectParams.minDistance         = RwRealMAXVAL;

	RpWorldForAllAtomicIntersections(world, &intersectParams.intersection, RwUtilWorldForAllAtomicIntersectLineCB, (void *)(&intersectParams));

	return intersectParams.pickedAtomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilAtomicRenderBoundingBox(RpAtomic *atomic)
{
	static RwImVertexIndex index[24] = 
    {
        0, 1,  1, 2,  2, 3,  3, 0,  4, 5,  5, 6,
        6, 7,  7, 4,  0, 4,  3, 7,  1, 5,  2, 6
    };

    RwIm3DVertex boxVertices[8];
    RwUInt8 red, green, blue, alpha;

    RpGeometry *geometry;
    RpMorphTarget *morphTarget;
    RwV3d *vertices;
    RwInt32 numVerts;
    RwBBox bBox;
    RwMatrix *ltm;

    /*
     * Get the atomic's vertices to calculate its bounding box...
     */
    geometry = RpAtomicGetGeometry(atomic);
    morphTarget  = RpGeometryGetMorphTarget(geometry, 0);
    vertices  = RpMorphTargetGetVertices(morphTarget);
    numVerts = RpGeometryGetNumVertices(geometry);

    RwBBoxCalculate(&bBox, vertices, numVerts);

    red = green = 196;
    blue = 0;
    alpha = 255;

    RwIm3DVertexSetRGBA(&boxVertices[0], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[0], bBox.inf.x, bBox.inf.y, bBox.inf.z);

    RwIm3DVertexSetRGBA(&boxVertices[1], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[1], bBox.sup.x, bBox.inf.y, bBox.inf.z);

    RwIm3DVertexSetRGBA(&boxVertices[2], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[2], bBox.sup.x, bBox.sup.y, bBox.inf.z);

    RwIm3DVertexSetRGBA(&boxVertices[3], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[3], bBox.inf.x, bBox.sup.y, bBox.inf.z);

    RwIm3DVertexSetRGBA(&boxVertices[4], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[4], bBox.inf.x, bBox.inf.y, bBox.sup.z);

    RwIm3DVertexSetRGBA(&boxVertices[5], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[5], bBox.sup.x, bBox.inf.y, bBox.sup.z);

    RwIm3DVertexSetRGBA(&boxVertices[6], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[6], bBox.sup.x, bBox.sup.y, bBox.sup.z);

    RwIm3DVertexSetRGBA(&boxVertices[7], red, green, blue, alpha);
    RwIm3DVertexSetPos(&boxVertices[7], bBox.inf.x, bBox.sup.y, bBox.sup.z);

    ltm = RwFrameGetLTM(RpAtomicGetFrame(atomic));

    if( RwIm3DTransform(boxVertices, 8, ltm, rwIM3D_ALLOPAQUE) )
    {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, index, 24);

        RwIm3DEnd();
    }
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilAtomicRenderAxis(RpAtomic *pstAtomic)
{
	RwMatrix		*pLTM	= RwFrameGetLTM(RpAtomicGetFrame(pstAtomic));

	RwV3d			*pPos	= RwMatrixGetPos(pLTM);
	RwV3d			*pRight	= RwMatrixGetRight(pLTM);
	RwV3d			*pUp	= RwMatrixGetUp(pLTM);
	RwV3d			*pAt	= RwMatrixGetAt(pLTM);

	RwIm3DVertex	astCoordinates[6];

	RwIm3DVertexSetRGBA(astCoordinates + 0, 255, 0, 0, 255);
	RwIm3DVertexSetRGBA(astCoordinates + 1, 255, 0, 0, 255);
	RwIm3DVertexSetRGBA(astCoordinates + 2, 0, 255, 0, 255);
	RwIm3DVertexSetRGBA(astCoordinates + 3, 0, 255, 0, 255);
	RwIm3DVertexSetRGBA(astCoordinates + 4, 0, 0, 255, 255);
	RwIm3DVertexSetRGBA(astCoordinates + 5, 0, 0, 255, 255);

	RwV3d vecTemp = RwUtilMakeV3d(pRight->x, pRight->y, pRight->z);
	RwV3dScale(&astCoordinates[0].objVertex, &vecTemp, 100);
	RwV3dAdd(&astCoordinates[0].objVertex, &astCoordinates[0].objVertex, pPos);

	vecTemp = RwUtilMakeV3d(-pRight->x, -pRight->y, -pRight->z);
	RwV3dScale(&astCoordinates[1].objVertex, &vecTemp, 100);
	RwV3dAdd(&astCoordinates[1].objVertex, &astCoordinates[1].objVertex, pPos);

	vecTemp = RwUtilMakeV3d(pUp->x, pUp->y, pUp->z);
	RwV3dScale(&astCoordinates[2].objVertex, &vecTemp, 100);
	RwV3dAdd(&astCoordinates[2].objVertex, &astCoordinates[2].objVertex, pPos);

	vecTemp = RwUtilMakeV3d(-pUp->x, -pUp->y, -pUp->z);
	RwV3dScale(&astCoordinates[3].objVertex, &vecTemp, 100);
	RwV3dAdd(&astCoordinates[3].objVertex, &astCoordinates[3].objVertex, pPos);

	vecTemp = RwUtilMakeV3d(pAt->x, pAt->y, pAt->z);
	RwV3dScale(&astCoordinates[4].objVertex, &vecTemp, 100);
	RwV3dAdd(&astCoordinates[4].objVertex, &astCoordinates[4].objVertex, pPos);

	vecTemp = RwUtilMakeV3d(-pAt->x, -pAt->y, -pAt->z);
	RwV3dScale(&astCoordinates[5].objVertex, &vecTemp, 100);
	RwV3dAdd(&astCoordinates[5].objVertex, &astCoordinates[5].objVertex, pPos);

	RwUtilRenderPrimTypeLineList(astCoordinates, 6);
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilRenderPrimTypeLineList(RwIm3DVertex *ver, RwUInt32 count, RwMatrix *pstLTM)
{
	RwMatrix ltm;
	RwMatrixSetIdentity(&ltm);

	if(RwIm3DTransform(ver, count, (pstLTM) ? (pstLTM) : (&ltm), rwIM3D_ALLOPAQUE))
	{
		if(RwIm3DRenderPrimitive(rwPRIMTYPELINELIST))
		{
			RwIm3DEnd();
		}
	}
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL RwUtilEqualV3d(RwV3d *pV3d, FLOAT x, FLOAT y, FLOAT z)
{
	if(pV3d->x == x)
		if(pV3d->y == y)
			if(pV3d->z == z)
				return TRUE;

	return FALSE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilRenderSphere(RwSphere *sphere, RwMatrix *ltm, RwRGBA *rgba)
{
	RwIm3DVertex	circle[RWUTIL_RENDER_CIRCLE_NUMPOINTS + 1];
    RwV3d			point;
    RwInt32			i;
    RwMatrix		_ltm;
	RwRGBA			_rgba;

	if(ltm)
	{
		RwMatrixCopy(&_ltm, ltm);
	}
	else
	{
		RwMatrixSetIdentity(&_ltm);
	}

	if(rgba)
	{
		memcpy(&_rgba, rgba, sizeof(RwRGBA));
	}
	else
	{
		memcpy(&_rgba, &RWUTIL_RED, sizeof(RwRGBA));
	}

    for(i = 0; i < RWUTIL_RENDER_CIRCLE_NUMPOINTS + 1; i++)
    {
		point.x = /*sphere->center.x + */((RwReal)RwCos(i / (RWUTIL_RENDER_CIRCLE_NUMPOINTS / 2.0f) * rwPI) * sphere->radius);
        point.y = /*sphere->center.y + */((RwReal)RwSin(i / (RWUTIL_RENDER_CIRCLE_NUMPOINTS / 2.0f) * rwPI) * sphere->radius);
        point.z = 0.0f;

        RwIm3DVertexSetPos(&circle[i], point.x, point.y, point.z);
        RwIm3DVertexSetRGBA(&circle[i], _rgba.red, _rgba.green, _rgba.blue, _rgba.alpha);
    }

	if(RwIm3DTransform(circle, RWUTIL_RENDER_CIRCLE_NUMPOINTS + 1, &_ltm, rwIM3D_ALLOPAQUE))
    {
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilAtomicGetWidthCB(RpAtomic *atomic, void *data)
{
	RwUtilAtomicCompareVerticesParams *stData = (RwUtilAtomicCompareVerticesParams *)(data);
	if(!stData)
		return NULL;

	RpGeometry		*pGeom			= RpAtomicGetGeometry(atomic);
	if(!pGeom)
		return NULL;

	RpMorphTarget	*pMorph			= RpGeometryGetMorphTarget(pGeom, 0);
	if(!pMorph)
		return NULL;

	RwV3d			*pVertexList	= RpMorphTargetGetVertices(pMorph);
	if(!pVertexList)
		return NULL;

	RwInt32			lNumVertices	= RpGeometryGetNumVertices(pGeom);
	for(RwInt32 lIndex = 0; lIndex < lNumVertices; ++lIndex)
	{
		if(pVertexList[lIndex].x > stData->max_x)
		{
			stData->max_x = pVertexList[lIndex].x;
		}
		else if(pVertexList[lIndex].x < stData->min_x)
		{
			stData->min_x = pVertexList[lIndex].x;
		}

		if(pVertexList[lIndex].z > stData->max_z)
		{
			stData->max_z = pVertexList[lIndex].z;
		}
		else if(pVertexList[lIndex].z < stData->min_z)
		{
			stData->min_z = pVertexList[lIndex].z;
		}
	}

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilAtomicGetHeightCB(RpAtomic *atomic, void *data)
{
	RwUtilAtomicCompareVerticesParams *pstData = (RwUtilAtomicCompareVerticesParams *)(data);
	if(!pstData)
		return NULL;

	RpGeometry		*pGeom			= RpAtomicGetGeometry(atomic);
	if(!pGeom)
		return NULL;

	RpMorphTarget	*pMorph			= RpGeometryGetMorphTarget(pGeom, 0);
	if(!pMorph)
		return NULL;

	RwV3d			*pVertexList	= RpMorphTargetGetVertices(pMorph);
	if(!pVertexList)
		return NULL;

	RwInt32			lNumVertices	= RpGeometryGetNumVertices(pGeom);
	for(RwInt32 lIndex = 0; lIndex < lNumVertices; ++lIndex)
	{
		if(pVertexList[lIndex].y > pstData->max_y)
			pstData->max_y = pVertexList[lIndex].y;
		else if(pVertexList[lIndex].y < pstData->min_y)
			pstData->min_y = pVertexList[lIndex].y;
	}

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwInt32 RwUtilClumpGetWidth(RpClump *pstClump)
{
	RwUtilAtomicCompareVerticesParams stData;
	memset(&stData, 0, sizeof(RwUtilAtomicCompareVerticesParams));

	RpClumpForAllAtomics(pstClump, RwUtilAtomicGetWidthCB, (void *)(&stData));

	RwInt32 lMax = (stData.max_x > stData.max_z) ? (RwInt32)(stData.max_x) : (RwInt32)(stData.max_z);
	RwInt32 lMin = (stData.min_x < stData.min_z) ? (RwInt32)(stData.min_x) : (RwInt32)(stData.min_z);

	return (abs(lMax) + abs(lMin));
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwInt32	RwUtilClumpGetHeight(RpClump *pstClump)
{
	RwUtilAtomicCompareVerticesParams stData;
	memset(&stData, 0, sizeof(RwUtilAtomicCompareVerticesParams));

	RpClumpForAllAtomics(pstClump, RwUtilAtomicGetHeightCB, (void *)(&stData));

	return (RwInt32)(stData.max_y - stData.min_y);
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpGeometry *RwUtilClumpHasSkinLODAtomicCB(RpGeometry *geom, void *data)
{
	BOOL *bHasSkin = (BOOL *)(data);

	RpSkin *pSkin = RpSkinGeometryGetSkin(geom);
	if(pSkin)
	{
		*(bHasSkin) = TRUE;
	}

	return geom;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL RwUtilAtomicHasSkin(RpAtomic *pstAtomic)
{
	RpGeometry *pGeom = RpAtomicGetGeometry(pstAtomic);
	if(pGeom)
	{
		RpSkin *pSkin = RpSkinGeometryGetSkin(pGeom);
		if(pSkin)
			return TRUE;
	}

	return FALSE;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilClumpHasSkinCB(RpAtomic *atomic, void *data)
{
	BOOL *bHasSkin = (BOOL *)(data);

/*	RpGeometry *pGeom = RpAtomicGetGeometry(atomic);
	if(pGeom)
	{
		RpSkin *pSkin = RpSkinGeometryGetSkin(pGeom);
		if(pSkin)
		{
			*(bHasSkin) = TRUE;
//			return atomic;
		}
	}*/
	if(RwUtilAtomicHasSkin(atomic))
		*(bHasSkin) = TRUE;

	RpLODAtomicForAllLODGeometries(atomic, RwUtilClumpHasSkinLODAtomicCB, data);

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
BOOL RwUtilClumpHasSkin(RpClump *pstClump)
{
	BOOL bHasSkin = FALSE;

	RpClumpForAllAtomics(pstClump, RwUtilClumpHasSkinCB, (void *)(&bHasSkin));

	return bHasSkin;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilClumpFindAtomicIndexCB(RpAtomic *atomic, void *data)
{
	RwUtilClumpFindAtomicIndexParams	*pcsParams = (RwUtilClumpFindAtomicIndexParams *)(data);

	if(atomic == pcsParams->m_pstFindAtomic)
		pcsParams->m_lAtomicIndex = pcsParams->m_lCBCount;

	++pcsParams->m_lCBCount;

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
INT32 RwUtilClumpFindAtomicIndex(RpClump *clump, RpAtomic *find_atomic)
{
	RwUtilClumpFindAtomicIndexParams csParams;
	csParams.m_pstFindAtomic = find_atomic;

	RpClumpForAllAtomics(clump, RwUtilClumpFindAtomicIndexCB, (void *)(&csParams));

	return csParams.m_lAtomicIndex;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilClumpGetUDAIntCB(RpAtomic *atomic, void *data)
{
	RwUtilClumpGetAtomicUDAInt32Params	*params	= (RwUtilClumpGetAtomicUDAInt32Params *)(data);
	if (params->m_pstFindAtomic != atomic)
		return atomic;

	RpGeometry	*geom	= RpAtomicGetGeometry(atomic);
	if (geom)
	{
		
		RwInt32								lUDANum					= RpGeometryGetUserDataArrayCount(geom);
		RpUserDataArray						*pstUserDataArray;
		RwChar								*pszUserDataArrayName;

		for (RwInt32 lCount = 0; lCount < lUDANum; ++lCount)
		{
			pstUserDataArray			= RpGeometryGetUserDataArray(geom, lCount);
			if (pstUserDataArray)
			{
				pszUserDataArrayName	= RpUserDataArrayGetName(pstUserDataArray);
				if (!strcmp(pszUserDataArrayName, params->m_pszUDAName))
				{
					params->m_lUDAInt32	= RpUserDataArrayGetInt(pstUserDataArray, 0);
					return NULL;
				}
			}
		}
	}

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwInt32	RwUtilClumpGetUDAInt(RpClump *clump, RpAtomic *find_atomic, CHAR *uda_name)
{
	RwUtilClumpGetAtomicUDAInt32Params	csParams;
	csParams.m_pszUDAName		= uda_name;
	csParams.m_pstFindAtomic	= find_atomic;

	RpClumpForAllAtomics(clump, RwUtilClumpGetUDAIntCB, (void *)(&csParams));

	return csParams.m_lUDAInt32;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RwV3d RwUtilAverageV3d(RwV3d *pstV1, RwV3d *pstV2)
{
	RwV3d	stRt;

	RwV3dAdd(&stRt, pstV1, pstV2);
	RwV3dScale(&stRt, &stRt, 0.5f);

	return stRt;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilRenderCylinder(RwV3d *pstPos, FLOAT fHeight, FLOAT fRadius, INT32 lPrecision, FLOAT fOffset)
{
	RwV3d	stTemp = {0, pstPos->y + fHeight, 0};

	RwV3d	stPerp1, stPerp2;

	memcpy(&stPerp1, &stTemp, sizeof(RwV3d));

	if ((stTemp.x == 0.0f) && (stTemp.z == 0.0f)) 
		stTemp.x += 1.0f;
	else
		stTemp.y += 1.0f;

	RwV3dCrossProduct(&stPerp2, &stPerp1, &stTemp);
	RwV3dCrossProduct(&stPerp1, &stTemp, &stPerp2);
	RwV3dNormalize(&stPerp1, &stPerp1);
	RwV3dNormalize(&stPerp2, &stPerp2);

	RwIm3DVertex	stIm3DVert[2];
	RwIm3DVertexSetRGBA(&stIm3DVert[0], 255, 255, 255, 255);
	RwIm3DVertexSetRGBA(&stIm3DVert[1], 255, 255, 255, 255);

	FLOAT	fTheta;
	RwV3d	stPos;
	for (INT32 lCount = 0; lCount < lPrecision; ++lCount)
	{
		fTheta	= lCount * fOffset / lPrecision;
		stTemp.x	= (FLOAT)(cos(fTheta)) * stPerp1.x + (FLOAT)(sin(fTheta)) * stPerp2.x;
		stTemp.y	= (FLOAT)(cos(fTheta)) * stPerp1.y + (FLOAT)(sin(fTheta)) * stPerp2.y;
		stTemp.z	= (FLOAT)(cos(fTheta)) * stPerp1.z + (FLOAT)(sin(fTheta)) * stPerp2.z;
		RwV3dNormalize(&stTemp, &stTemp);

		stPos.x	= pstPos->x				+ fRadius * stTemp.x;
		stPos.y	= pstPos->y + fHeight	+ fRadius * stTemp.y;
		stPos.z	= pstPos->z				+ fRadius * stTemp.z;
		RwIm3DVertexSetPos(&stIm3DVert[0], stPos.x, stPos.y, stPos.z);

		stPos.x	= pstPos->x + fRadius * stTemp.x;
		stPos.y	= pstPos->y + fRadius * stTemp.y;
		stPos.z	= pstPos->z + fRadius * stTemp.z;
		RwIm3DVertexSetPos(&stIm3DVert[1], stPos.x, stPos.y, stPos.z);

		if (RwIm3DTransform(stIm3DVert, 2, NULL, 0))
		{
			RwIm3DRenderPrimitive(rwPRIMTYPELINELIST);

			RwIm3DEnd();
		}
	}
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilRenderUnitSphere(int iterations, FLOAT fRadius)
{
	INT32 lNum = 4;
	INT32 lStart;
	INT32 i, j;

	for (i = 1; i < iterations; ++i) 
	{
		lStart = lNum;
		
		for (j = 0; j < lStart; ++j)
		{
			lNum += 3;
		}
	}

	// 점의 갯수는 면의 갯수와 같다.
	RwV3d			*point = (RwV3d *)(malloc(sizeof(RwV3d) * lNum));
	RwImVertexIndex	*index = (RwImVertexIndex  *)(malloc(sizeof(RwImVertexIndex) * (lNum * 3)));

	point[0] = RwUtilMakeV3d(1.0f, 1.0f, 1.0f);
	point[1] = RwUtilMakeV3d(-1.0f, -1.0f, 1.0f);
	point[2] = RwUtilMakeV3d(1.0f, -1.0f, -1.0f);
	point[3] = RwUtilMakeV3d(-1.0f, 1.0f, -1.0f);

	index[(3 * 0) + 0] = 0;
	index[(3 * 0) + 1] = 1;
	index[(3 * 0) + 2] = 2;
	index[(3 * 1) + 0] = 1;
	index[(3 * 1) + 1] = 0;
	index[(3 * 1) + 2] = 3;
	index[(3 * 2) + 0] = 1;
	index[(3 * 2) + 1] = 3;
	index[(3 * 2) + 2] = 2;
	index[(3 * 3) + 0] = 0;
	index[(3 * 3) + 1] = 2;
	index[(3 * 3) + 2] = 3;

	lNum = 4;
	for (i = 1; i < iterations; ++i)
	{
		lStart = lNum;

		for (j = 0; j < lStart; ++j)
		{
			memcpy(index + (3 * (lNum + 0)), index + (3 * j), sizeof(RwImVertexIndex) * 3);
			memcpy(index + (3 * (lNum + 1)), index + (3 * j), sizeof(RwImVertexIndex) * 3);
			memcpy(index + (3 * (lNum + 2)), index + (3 * j), sizeof(RwImVertexIndex) * 3);

			point[lNum + 0] = RwUtilAverageV3d(point + index[(j * 3) + 0], point + index[(j * 3) + 1]);
			point[lNum + 1] = RwUtilAverageV3d(point + index[(j * 3) + 1], point + index[(j * 3) + 2]);
			point[lNum + 2] = RwUtilAverageV3d(point + index[(j * 3) + 2], point + index[(j * 3) + 0]);
			/*point[lNum + 0] = MidPoint(point + index[(j * 3) + 0], point + index[(j * 3) + 1]);
			point[lNum + 1] = MidPoint(point + index[(j * 3) + 1], point + index[(j * 3) + 2]);
			point[lNum + 2] = MidPoint(point + index[(j * 3) + 2], point + index[(j * 3) + 0]);*/

			index[(j * 3) + 1] = lNum + 0;
			index[(j * 3) + 2] = lNum + 2;

			index[3 * (lNum + 0) + 0] = lNum + 0;
			index[3 * (lNum + 0) + 2] = lNum + 1;
			index[3 * (lNum + 1) + 0] = lNum + 2;
			index[3 * (lNum + 1) + 1] = lNum + 1;
			index[3 * (lNum + 2) + 0] = lNum + 0;
			index[3 * (lNum + 2) + 1] = lNum + 1;
			index[3 * (lNum + 2) + 2] = lNum + 2;

			lNum += 3;
		}
	}

	for (i = 0; i < lNum; ++i)
	{
		RwV3dNormalize(point + i, point + i);
	}


	RwIm3DVertex	*pstIm3DVert	= (RwIm3DVertex *)(malloc(sizeof(RwIm3DVertex) * lNum));

	RwV3d			stTemp;
	for (i = 0; i < lNum; ++i)
	{
		RwV3dScale(&stTemp, point + i, fRadius);

		RwIm3DVertexSetRGBA(pstIm3DVert + i, 255, 255, 255, 255);
		RwIm3DVertexSetPos(pstIm3DVert + i, stTemp.x, stTemp.y, stTemp.z);
	}

	if (RwIm3DTransform(pstIm3DVert, lNum, NULL, 0))
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, index, lNum * 3);
		RwIm3DEnd();
	}


	free(pstIm3DVert);
	free(point);
	free(index);
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilClumpGetAtomicsCB(RpAtomic *atomic, void *data)
{
	RwUtilClumpGetAtomicParams		*pstParams	= (RwUtilClumpGetAtomicParams *)(data);
	pstParams->m_pastAtomic[pstParams->m_lCount++] = atomic;

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilClumpGetAtomics(RpClump *pstClump, RwUtilClumpGetAtomicParams *pstParams)
{
	RpClumpForAllAtomics(pstClump, RwUtilClumpGetAtomicsCB, pstParams);
}

/*****************************************************************************
* Desc :
*****************************************************************************/
RpAtomic *RwUtilClumpGetBoundingBoxCB(RpAtomic *atomic, void *data)
{
	RwUtilClumpGetBBoxParams *params = (RwUtilClumpGetBBoxParams *)(data);

	RpGeometry *geometry = RpAtomicGetGeometry(atomic);
	if (geometry)
	{
		RpMorphTarget *morphtarget = RpGeometryGetMorphTarget(geometry, 0);
		if (morphtarget)
		{
			params->m_pastVerticesArray[params->m_lCount] = RpMorphTargetGetVertices(morphtarget);
			params->m_alVertices[params->m_lCount] = RpGeometryGetNumVertices(geometry);

			++params->m_lCount;
		}
	}

	return atomic;
}

/*****************************************************************************
* Desc :
*****************************************************************************/
VOID RwUtilClumpGetBoundingBox(RpClump **clump, RwInt32 count, RwBBox *box)
{
	RwUtilClumpGetBBoxParams	params;

	for (INT32 lCount = 0; lCount < count; ++lCount)
	{
		if (!clump[lCount])
			break;

		RpClumpForAllAtomics(clump[lCount], RwUtilClumpGetBoundingBoxCB, &params);
	}

	if (params.m_lCount < 1)
		return;

	INT32	lMaxVertices	= 0;
	for (lCount = 0; lCount < params.m_lCount; ++lCount)
	{
		lMaxVertices		+= params.m_alVertices[lCount];
	}

	RwV3d	*pstVertices	= (RwV3d *)(malloc(sizeof(RwV3d) * lMaxVertices));
	INT32	lProgress		= 0;
	for (lCount = 0; lCount < params.m_lCount; ++lCount)
	{
		memcpy(pstVertices + lProgress, params.m_pastVerticesArray[lCount], sizeof(RwV3d) * params.m_alVertices[lCount]);
		lProgress			+= params.m_alVertices[lCount];
	}

	RwBBoxCalculate(box, pstVertices, lMaxVertices);

	free(pstVertices);
}

/*****************************************************************************
*****************************************************************************/