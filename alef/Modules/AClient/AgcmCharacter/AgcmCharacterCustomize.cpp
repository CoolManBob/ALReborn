#include "AgcmCharacter.h"

/*****************************************************************************
* Purpose :
*
* 20050319 dobal
******************************************************************************/

VOID AgcmCharacter::DestoryCustomize( AgcdCharacter* pstAgcdCharacter )
{
	if( pstAgcdCharacter->m_pFace && !RpAtomicGetClump(pstAgcdCharacter->m_pFace) )
	{
		RwFrame *pstpFaceFrame = RpAtomicGetFrame(pstAgcdCharacter->m_pFace);
		if(pstAgcdCharacter->m_pClump && pstpFaceFrame != RpClumpGetFrame(pstAgcdCharacter->m_pClump))
		{
			RpAtomicSetFrame(pstAgcdCharacter->m_pFace, NULL);
			if( pstpFaceFrame ) {

				//. 2006. 3. 9. Nonstopdj
				//. if RwFrameGetParent ( m_pFrm ) have no child, access violation reading location RwFrameRemoveChild().
				RwFrame *curFrame = RwFrameGetParent ( pstpFaceFrame );
				
				//@{ 2006/05/03 burumal
				//if( curFrame && curFrame->child && curFrame->child->next)
					//RwFrameRemoveChild( pstpFaceFrame );
				if( curFrame && curFrame->child )				
					RwFrameRemoveChild( pstpFaceFrame );
				//@}

				RwFrameDestroy(pstpFaceFrame);
			}
		}

		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAgcdCharacter->m_pFace);
		else
			RpAtomicDestroy(pstAgcdCharacter->m_pFace);

		pstAgcdCharacter->m_pFace = NULL;
	}

	if( pstAgcdCharacter->m_pHair && !RpAtomicGetClump(pstAgcdCharacter->m_pHair) )
	{
		RwFrame* pstpHairFrame = RpAtomicGetFrame(pstAgcdCharacter->m_pHair);
		if(pstAgcdCharacter->m_pClump && pstpHairFrame != RpClumpGetFrame(pstAgcdCharacter->m_pClump))
		{
			RpAtomicSetFrame(pstAgcdCharacter->m_pHair, NULL);
			if( pstpHairFrame ) {
				//@{ 2006/05/03 burumal
				//RwFrameRemoveChild(pstpHairFrame);
				RwFrame* pHairParent = RwFrameGetParent(pstpHairFrame);
				if ( pHairParent && pHairParent->child )
					RwFrameRemoveChild(pstpHairFrame);
				//@}

				RwFrameDestroy(pstpHairFrame);
			}
		}

		if (m_pcsAgcmResourceLoader)
			m_pcsAgcmResourceLoader->AddDestroyAtomic(pstAgcdCharacter->m_pHair);
		else
			RpAtomicDestroy(pstAgcdCharacter->m_pHair);

		pstAgcdCharacter->m_pHair = NULL;
	}
}

VOID AgcmCharacter::SetCustomizeRenderType( RpAtomic *pAtomic, int nRenderType )
{
	if( nRenderType == 0 ) {
		nRenderType	= pAtomic->stRenderInfo.renderType;

		if( nRenderType == 0 )
			nRenderType	= R_NONALPHA;
	}

	int	nRenderTypeTemp	= nRenderType & 0x0000ffff;

	int	nBlendMode	= R_BLENDSRCAINVA_ADD;
	if((nRenderTypeTemp == R_BLEND_SORT) || (nRenderTypeTemp == R_BLEND_NSORT))
		nBlendMode = (nRenderType & 0xffff0000) >> 16;

	AcuObject::SetAtomicRenderUDA( 
		pAtomic, 
		nRenderTypeTemp, (RpAtomicCallBackRender)(pAtomic->stRenderInfo.backupCB),
		nBlendMode, 
		0, 
		0 );
}

BOOL AgcmCharacter::LoadTemplateDefaultFace( AgcdCharacterTemplate *pstAgcdCharacterTemplate )
{
	ReleaseTemplateDefaultFace( pstAgcdCharacterTemplate );

	AgpdCharacterTemplate* pAgpdCharacterTemplate = GetTemplate( pstAgcdCharacterTemplate );

	if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData ) {
		pstAgcdCharacterTemplate->m_vpFace.clear();
		RpHAnimHierarchy* pstCharacterHierarchy	= GetHierarchy( pstAgcdCharacterTemplate->m_pClump );

		char szName[256];
		int nNum = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->GetFaceNum();
		pstAgcdCharacterTemplate->m_nFaceNum = nNum;
		for( int i = 0; i < nNum; i++ )
		{
			if (pstAgcdCharacterTemplate->GetClumpID() > 0)	{
				pstAgcdCharacterTemplate->GetCustomizeFaceName( szName,pAgpdCharacterTemplate->m_lID, i );
			}
			else {
				strcpy( szName, pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecFaceInfo[i].c_str() );
			}

			RpAtomic* pAtomic = LoadAtomic( szName, "DefaultHead\\Face\\", pstCharacterHierarchy );
			if( pAtomic == NULL )
				continue;

			pAtomic->iPartID = -1;
			pstAgcdCharacterTemplate->m_vpFace.push_back( pAtomic );

			int nRenderType, nCustData;
			nRenderType = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecRenderType[i];
			nCustData = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_FaceRenderType.m_vecCustData[i];
			SetCustomizeRenderType( pAtomic, nRenderType );
		}
	}
	return TRUE;
}

BOOL AgcmCharacter::LoadTemplateDefaultHair( AgcdCharacterTemplate *pstAgcdCharacterTemplate )
{
	ReleaseTemplateDefaultHair( pstAgcdCharacterTemplate );

	AgpdCharacterTemplate* pAgpdCharacterTemplate = GetTemplate( pstAgcdCharacterTemplate );

	if( pstAgcdCharacterTemplate->m_pcsDefaultHeadData ) {
		pstAgcdCharacterTemplate->m_vpHair.clear();
		RpHAnimHierarchy* pstCharacterHierarchy	= GetHierarchy( pstAgcdCharacterTemplate->m_pClump );

		char szName[256];
		int nNum = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->GetHairNum();
		pstAgcdCharacterTemplate->m_nHairNum = nNum;
		for( int i = 0; i < nNum; i++ )
		{
			if (pstAgcdCharacterTemplate->GetClumpID() > 0)	{
				pstAgcdCharacterTemplate->GetCustomizeHairName( szName,pAgpdCharacterTemplate->m_lID, i );
			}
			else {
				strcpy( szName, pstAgcdCharacterTemplate->m_pcsDefaultHeadData->vecHairInfo[i].c_str() );
			}

			RpAtomic* pAtomic = LoadAtomic( szName, "DefaultHead\\Hair\\", pstCharacterHierarchy );
			if( pAtomic == NULL )
				continue;
			pAtomic->iPartID = -1;
			pstAgcdCharacterTemplate->m_vpHair.push_back( pAtomic );

			int nRenderType, nCustData;
			nRenderType = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecRenderType[i];
			nCustData = pstAgcdCharacterTemplate->m_pcsDefaultHeadData->m_HairRenderType.m_vecCustData[i];
			SetCustomizeRenderType( pAtomic, nRenderType );
		}
	}
	return TRUE;
}

VOID AgcmCharacter::SetFace( AgcdCharacter* pcdCharacter, INT32 nFaceID )
{
	if( !pcdCharacter ) return;
	if( !pcdCharacter->m_pstAgcdCharacterTemplate ) return;
	if( !( pcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_INIT_HIERARCHY ) || ( pcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED ) ) return;
	if( pcdCharacter->m_nAttachFaceID == nFaceID && pcdCharacter->m_pFace ) return;
	if( nFaceID < 0 ) return;

	////@{ Jaewon 20051010
	//// Back up the render infos before detaching.
	//bool backedUp = false;
	//AgcdRenderInfo renderInfo;
	//RpAtomicCallBackRender renderCallBack;

	//if(pstCharacter->m_pFace)
	//{
	//	renderInfo = pstCharacter->m_pFace->stRenderInfo;
	//	RpLODAtomicUnHookRender(pstCharacter->m_pFace);
	//	renderCallBack = RpAtomicGetRenderCallBack(pstCharacter->m_pFace);	
	//	RpLODAtomicHookRender(pstCharacter->m_pFace);
	//	backedUp = true;
	//}
	////@} Jaewon

	DetachFace( pcdCharacter );

	//if( nFaceID >= (int)pstCharacter->m_pstAgcdCharacterTemplate->m_vpFace.size() )
	//{
	//	LoadTemplateDefaultFace( pstCharacter->m_pstAgcdCharacterTemplate );
	//}

	pcdCharacter->m_nAttachFaceID = nFaceID;
	AttachFace( pcdCharacter );

	////@{ Jaewon 20051010
	//// Restore the render infos after attaching.
	//if(backedUp && pstCharacter->m_pFace)
	//{
	//	pstCharacter->m_pFace->stRenderInfo = renderInfo;
	//	RpLODAtomicUnHookRender(pstCharacter->m_pFace);
	//	RpAtomicSetRenderCallBack(pstCharacter->m_pFace, renderCallBack);	
	//	RpLODAtomicHookRender(pstCharacter->m_pFace);
	//}
	////@} Jaewon
}

BOOL AgcmCharacter::AttachFace( AgcdCharacter* pstCharacter )
{
	PROFILE("AgcmCharacter::AttachFace");

	if( pstCharacter == NULL )
		return TRUE;

	if( pstCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED )
		return TRUE;

	if( pstCharacter->m_pstAgcdCharacterTemplate == NULL )
		return TRUE;

	if( pstCharacter->m_nAttachFaceID >= (int)pstCharacter->m_pstAgcdCharacterTemplate->m_vpFace.size() )
		return TRUE;

	//@{ Jaewon 20050502
	RpAtomic * pFaceAtomic = pstCharacter->m_pstAgcdCharacterTemplate->m_vpFace[pstCharacter->m_nAttachFaceID];
	if( !pFaceAtomic ) return TRUE;

	pstCharacter->m_pFace = RpAtomicClone( pFaceAtomic );
	//@} Jaewon

	if( pstCharacter->m_pFace == NULL )
		return FALSE;

	//if( pstCharacter->m_bAttachableFace == FALSE )
	//	return TRUE;

	LockFrame();

	RpAtomic* pstAtomic			= pstCharacter->m_pFace;
	RpClump*  pstCharacterClump	= pstCharacter->m_pClump;
	RpHAnimHierarchy* pstCharacterHierarchy	= pstCharacter->m_pInHierarchy;

	RpGeometry*	pstGeom			= RpAtomicGetGeometry(pstAtomic);
	RwFrame*	pstFrame		= RpClumpGetFrame(pstCharacterClump);
	RwFrame*	pstAtomicFrame	= RwFrameCreate();

	AcuObject::SetAtomicType( pstAtomic, ACUOBJECT_TYPE_RENDER_UDA, 0 );

	RpAtomicSetFrame( pstAtomic, pstAtomicFrame);
	RwFrameAddChild ( pstFrame, pstAtomicFrame);
	RpClumpAddAtomic( pstCharacterClump, pstAtomic);
	SetHierarchyForSkinAtomic( pstAtomic, (void *)pstCharacterHierarchy );

	AcuObject::SetAtomicRenderUDA(
		pstAtomic,
		pstAtomic->stRenderInfo.renderType,
		(RpAtomicCallBackRender)(pstAtomic->stRenderInfo.backupCB),
		pstAtomic->stRenderInfo.blendMode,
		0,
		0							);

	if( m_pWorld )
	{
		if(m_pcsAgcmRender)
			m_pcsAgcmRender->AddAtomicToWorld( pstAtomic, ONLY_ALPHA,AGCMRENDER_ADD_NONE, true );
		else
			RpWorldAddAtomic(m_pWorld, pstAtomic);
	}

	return TRUE;
}

BOOL AgcmCharacter::DetachFace( AgcdCharacter* pcdCharacter )
{
	if( !pcdCharacter || !pcdCharacter->m_pFace || !pcdCharacter->m_pClump ) return TRUE;

	RpAtomic* pFaceAtomic = pcdCharacter->m_pFace;
	RpGeometry*	pFaceGeometry = RpAtomicGetGeometry( pFaceAtomic );

	RpClump* pCharacterClump = pcdCharacter->m_pClump;
	RwFrame* pCharacterFrame = RpClumpGetFrame( pCharacterClump );

	if( m_pcsAgcmRender )
	{
		// 얼굴아토믹을 월드에서 제거해 달라고 렌더러에게 시킴
		m_pcsAgcmRender->RemoveAtomicFromWorld( pFaceAtomic );
	}
	else if( m_pWorld )
	{
		// 얼굴아토믹을 직접 월드에서 제거
		RpWorldRemoveAtomic( m_pWorld, pFaceAtomic );
	}

	// 캐릭터 클럼프에서 얼굴아토믹을 떼어내고..
	RpClumpRemoveAtomic( pCharacterClump, pFaceAtomic );

	// 얼굴아토믹 프레임을 NULL 로 지정
	RwFrame* pFaceFrame = RpAtomicGetFrame( pFaceAtomic );
	RpAtomicSetFrame( pFaceAtomic, NULL );

	if( m_pcsAgcmResourceLoader )
	{
		// 리소스 로더가 있으면 리소스로더에게 지우라고 하고..
		m_pcsAgcmResourceLoader->AddDestroyAtomic( pFaceAtomic );
	}
	else
	{
		// 리소스 로더 없으면 직접 삭제
		RpAtomicDestroy( pFaceAtomic );
	}

	// 현재 얼굴이 없으니 NULL 로 지정
	pFaceAtomic = NULL;
	pcdCharacter->m_pFace = NULL;
	pcdCharacter->m_nAttachFaceID = -1;

	// 얼굴프레임이 캐릭터 프레임에 직접 붙어 있는게 아니라면
	if( pFaceFrame && pFaceFrame != pCharacterFrame )
	{
		// 부모 프레임이 있으면 부모로부터 얼굴프레임을 떼어낸 후에..
		RwFrame* pFaceParentFrame = RwFrameGetParent( pFaceFrame );
		if( pFaceParentFrame )
		{
			RwFrameRemoveChild( pFaceFrame );
		}

		// 얼굴프레임을 삭제
		RwFrameDestroy( pFaceFrame );
		pFaceFrame = NULL;
	}

	return TRUE;
}

VOID AgcmCharacter::SetHair( AgcdCharacter* pcdCharacter, INT32 nHairID )
{
	if( !pcdCharacter ) return;
	if( !pcdCharacter->m_pstAgcdCharacterTemplate ) return;
	if( !( pcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_INIT_HIERARCHY ) || ( pcdCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED ) ) return;
	if( pcdCharacter->m_nAttachHairID == nHairID && pcdCharacter->m_pHair ) return;
	if( nHairID < 0 ) return;

	//bool backedUp = false;
	//AgcdRenderInfo renderInfo;
	//RpAtomicCallBackRender renderCallBack;
	//if(pstCharacter->m_pHair)
	//{
	//	renderInfo = pstCharacter->m_pHair->stRenderInfo;
	//	RpLODAtomicUnHookRender(pstCharacter->m_pHair);
	//	renderCallBack = RpAtomicGetRenderCallBack(pstCharacter->m_pHair);	
	//	RpLODAtomicHookRender(pstCharacter->m_pHair);
	//	backedUp = true;
	//}
	////@} Jaewon

	// 기존머리 삭제
	DetachHair( pcdCharacter );

	//if( nHairID >= (int)pstCharacter->m_pstAgcdCharacterTemplate->m_vpHair.size() )
	//{
	//	LoadTemplateDefaultHair( pstCharacter->m_pstAgcdCharacterTemplate );
	//}

	pcdCharacter->m_nAttachHairID = nHairID;
	AttachHair( pcdCharacter );

	////@{ Jaewon 20051010
	//// Restore the render infos after attaching.
	//if(backedUp && pstCharacter->m_pHair)
	//{
	//	pstCharacter->m_pHair->stRenderInfo = renderInfo;
	//	RpLODAtomicUnHookRender(pstCharacter->m_pHair);
	//	RpAtomicSetRenderCallBack(pstCharacter->m_pHair, renderCallBack);	
	//	RpLODAtomicHookRender(pstCharacter->m_pHair);
	//}
	////@} Jaewon
}

BOOL AgcmCharacter::AttachHair( AgcdCharacter* pstCharacter )
{
	PROFILE("AgcmCharacter::AttachHair");

	if( pstCharacter == NULL )
		return TRUE;

	if( pstCharacter->m_lStatus & AGCMCHAR_STATUS_FLAG_REMOVED )
		return TRUE;

	if( pstCharacter->m_pstAgcdCharacterTemplate == NULL )
		return TRUE;

	if( pstCharacter->m_nAttachHairID >= (int)pstCharacter->m_pstAgcdCharacterTemplate->m_vpHair.size() )
		return TRUE;

	//@{ Jaewon 20050502
	RpAtomic * pAtomic = pstCharacter->m_pstAgcdCharacterTemplate->m_vpHair[ pstCharacter->m_nAttachHairID ];
	if( !pAtomic ) return TRUE;

	pstCharacter->m_pHair = RpAtomicClone( pAtomic );
	//@} Jaewon

	if( pstCharacter->m_pHair == NULL )
		return FALSE;

	//if( pstCharacter->m_bAttachableHair == FALSE )
	//	return TRUE;

	RpAtomic* pstAtomic						= pstCharacter->m_pHair;
	RpClump*  pstCharacterClump				= pstCharacter->m_pClump;
	RpHAnimHierarchy* pstCharacterHierarchy	= pstCharacter->m_pInHierarchy;

	AcuObject::SetAtomicType( pstAtomic, ACUOBJECT_TYPE_RENDER_UDA, 0 );

	RpGeometry*	pstGeom			= RpAtomicGetGeometry(pstAtomic);
	RwFrame*	pstFrame		= RpClumpGetFrame(pstCharacterClump);
	RwFrame*	pstAtomicFrame	= RwFrameCreate();

	RpAtomicSetFrame( pstAtomic, pstAtomicFrame);
	RwFrameAddChild ( pstFrame, pstAtomicFrame);
	RpClumpAddAtomic( pstCharacterClump, pstAtomic);
	SetHierarchyForSkinAtomic( pstAtomic, (void *)pstCharacterHierarchy );

	AcuObject::SetAtomicRenderUDA(
		pstAtomic,
		pstAtomic->stRenderInfo.renderType,
		(RpAtomicCallBackRender)(pstAtomic->stRenderInfo.backupCB),
		pstAtomic->stRenderInfo.blendMode,
		0,
		0							);

	if( m_pWorld )
	{
		if( m_pcsAgcmRender )
		{
			m_pcsAgcmRender->AddAtomicToWorld( pstAtomic, ONLY_NONALPHA,AGCMRENDER_ADD_NONE, true );
		}
		else
		{
			RpWorldAddAtomic(m_pWorld, pstAtomic);
		}
	}

	return TRUE;
}

BOOL AgcmCharacter::DetachHair( AgcdCharacter* pcdCharacter )
{
	if( !pcdCharacter || !pcdCharacter->m_pHair || !pcdCharacter->m_pClump ) return TRUE;

	RpAtomic* pHairAtomic = pcdCharacter->m_pHair;
	RpGeometry*	pHairGeometry = RpAtomicGetGeometry( pHairAtomic );

	RpClump* pCharacterClump = pcdCharacter->m_pClump;
	RwFrame* pCharacterFrame = RpClumpGetFrame( pCharacterClump );

	if( m_pcsAgcmRender )
	{
		// 머리아토믹을 월드에서 제거해 달라고 렌더러에게 시킴
		m_pcsAgcmRender->RemoveAtomicFromWorld( pHairAtomic );
	}
	else if( m_pWorld )
	{
		// 머리아토믹을 직접 월드에서 제거
		RpWorldRemoveAtomic( m_pWorld, pHairAtomic );
	}

	// 캐릭터 클럼프에서 머리아토믹을 떼어내고..
	RpClumpRemoveAtomic( pCharacterClump, pHairAtomic );

	// 머리아토믹 프레임을 NULL 로 지정
	RwFrame* pHairFrame = RpAtomicGetFrame( pHairAtomic );
	RpAtomicSetFrame( pHairAtomic, NULL );

	if( m_pcsAgcmResourceLoader )
	{
		// 리소스 로더가 있으면 리소스로더에게 지우라고 하고..
		m_pcsAgcmResourceLoader->AddDestroyAtomic( pHairAtomic );
	}
	else
	{
		// 리소스 로더 없으면 직접 삭제
		RpAtomicDestroy( pHairAtomic );
	}

	// 현재 머리가 없으니 NULL 로 지정
	pHairAtomic = NULL;
	pcdCharacter->m_pHair = NULL;
	pcdCharacter->m_nAttachHairID = -1;

	// 얼굴프레임이 캐릭터 프레임에 직접 붙어 있는게 아니라면
	if( pHairFrame && pHairFrame != pCharacterFrame )
	{
		// 부모 프레임이 있으면 부모로부터 얼굴프레임을 떼어낸 후에..
		RwFrame* pHairParentFrame = RwFrameGetParent( pHairFrame );
		if( pHairParentFrame )
		{
			RwFrameRemoveChild( pHairFrame );
		}

		// 얼굴프레임을 삭제
		RwFrameDestroy( pHairFrame );
		pHairFrame = NULL;
	}

	return TRUE;
}

//. 2006. 3. 15. nonstopdj
//. attach되는 face와 hair의 공통vertex들의 normal을 face기준으로 force set.
void AgcmCharacter::GeneralizeAttachedAtomicsNormals(AgcdCharacter* pstCharacter)
{
	if( pstCharacter->m_nAttachFaceID < 0 || pstCharacter->m_nAttachHairID < 0 ) return;

	//. 2006. 3. 16. nonstopdj
	//. edit normal by head atomic normal.
	if(pstCharacter && pstCharacter->m_pstAgcdCharacterTemplate)
	{
		RpAtomic* pFace = NULL;
		RpAtomic* pHair = NULL;
		if((INT32) pstCharacter->m_pstAgcdCharacterTemplate->m_vpFace.size() > pstCharacter->m_nAttachFaceID)
			pFace = pstCharacter->m_pstAgcdCharacterTemplate->m_vpFace[pstCharacter->m_nAttachFaceID];

		if((INT32) pstCharacter->m_pstAgcdCharacterTemplate->m_vpHair.size() > pstCharacter->m_nAttachHairID)
			pHair = pstCharacter->m_pstAgcdCharacterTemplate->m_vpHair[pstCharacter->m_nAttachHairID];

		if(pFace && pHair)
		{
			RpGeometry * pHairGeometry = RpAtomicGetGeometry(pHair);
			RpGeometry * pFaceGeometry = RpAtomicGetGeometry(pFace);

			RpMorphTarget * pMorphTargetHair = NULL;
			RpMorphTarget * pMorphTargetFace = NULL;

			if(!pHairGeometry || !pFaceGeometry)
				return;

			pMorphTargetHair = RpGeometryGetMorphTarget( pHairGeometry , 0 );
			pMorphTargetFace = RpGeometryGetMorphTarget( pFaceGeometry , 0 );

			if(!pMorphTargetHair || !pMorphTargetFace)
				return;

			for(int nHairVertexCount = 0; nHairVertexCount < pHairGeometry->numVertices; ++nHairVertexCount)
			{
				for(int nHeadVertexCount = 0; nHeadVertexCount < pFaceGeometry->numVertices; ++nHeadVertexCount)
				{
					//. if hair vertex and head vertex is equal, change hair vertex normal by head vertex normal.
					RwV3d vLength;

					RwV3dSub(&vLength, &pMorphTargetHair->verts[nHairVertexCount], &pMorphTargetFace->verts[nHeadVertexCount]);
					RwReal fLength = RwV3dLength(&vLength);

					//if(pMorphTargetHair->verts[nHairVertexCount].x == pMorphTargetHead->verts[nHeadVertexCount].x &&
					//	pMorphTargetHair->verts[nHairVertexCount].y == pMorphTargetHead->verts[nHeadVertexCount].y &&
					//   pMorphTargetHair->verts[nHairVertexCount].z == pMorphTargetHead->verts[nHeadVertexCount].z )
					if(fLength <= 0.01f)
					{
						pMorphTargetHair->normals[nHairVertexCount].x = pMorphTargetFace->normals[nHeadVertexCount].x;
						pMorphTargetHair->normals[nHairVertexCount].y = pMorphTargetFace->normals[nHeadVertexCount].y;
						pMorphTargetHair->normals[nHairVertexCount].z = pMorphTargetFace->normals[nHeadVertexCount].z;
					}
				}
			}
		}
	}
}


#ifdef USE_MFC
void AgcmCharacter::SaveEditHairAtomic(AgcdCharacter* pstCharacter)
{
	//. 2006. 3. 16. nonstopdj
	//. only using Modeltool debug.
	//. edit normal by head atomic normal.
	if(m_bisSaveHairClump && pstCharacter)
	{
		if(pstCharacter->m_pFace && pstCharacter->m_pHair)
		{
			RpGeometry * pHairGeometry = RpAtomicGetGeometry(pstCharacter->m_pHair);
			RpGeometry * pFaceGeometry = RpAtomicGetGeometry(pstCharacter->m_pFace);

			RpMorphTarget * pMorphTargetHair = NULL;
			RpMorphTarget * pMorphTargetFace = NULL;

			if(pHairGeometry)
				pMorphTargetHair = RpGeometryGetMorphTarget( pHairGeometry , 0 );

			if(pFaceGeometry)
				pMorphTargetFace = RpGeometryGetMorphTarget( pFaceGeometry , 0 );

			for(int nHairVertexCount = 0; nHairVertexCount < pHairGeometry->numVertices; ++nHairVertexCount)
			{
				for(int nHeadVertexCount = 0; nHeadVertexCount < pFaceGeometry->numVertices; ++nHeadVertexCount)
				{
					//. if hair vertex and head vertex is equal, change hair vertex normal by head vertex normal.
					RwV3d vLength;

					RwV3dSub(&vLength, &pMorphTargetHair->verts[nHairVertexCount], &pMorphTargetFace->verts[nHeadVertexCount]);
					RwReal fLength = RwV3dLength(&vLength);
					
					//if(pMorphTargetHair->verts[nHairVertexCount].x == pMorphTargetHead->verts[nHeadVertexCount].x &&
					//	pMorphTargetHair->verts[nHairVertexCount].y == pMorphTargetHead->verts[nHeadVertexCount].y &&
					//   pMorphTargetHair->verts[nHairVertexCount].z == pMorphTargetHead->verts[nHeadVertexCount].z )
					if(fLength <= 0.01f)
					{
						pMorphTargetHair->normals[nHairVertexCount].x = pMorphTargetFace->normals[nHeadVertexCount].x;
						pMorphTargetHair->normals[nHairVertexCount].y = pMorphTargetFace->normals[nHeadVertexCount].y;
						pMorphTargetHair->normals[nHairVertexCount].z = pMorphTargetFace->normals[nHeadVertexCount].z;
					}
				}
			}
		}

		m_bisSaveHairClump = FALSE;
	}
}
#endif