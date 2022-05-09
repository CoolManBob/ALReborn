// AgcuEffUtil.cpp
///////////////////////////////////////////////////////////////////////////////


#include "AgcuEffUtil.h"
#include "AgcdEffGlobal.h"

#include <vector>
#include <algorithm>

#include "ApMemoryTracker.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

#include "AcuFrameMemory.h"

// get clump's trinum
RpAtomic* CB_GetAtomicNumTri(RpAtomic* atom, void* pnumtri)
{
	ASSERT( atom && pnumtri );

	if( RpAtomicGetGeometry( atom ) )
		*static_cast<RwInt32*>(pnumtri)
			+= RpGeometryGetNumTriangles ( RpAtomicGetGeometry( atom ) );

	return atom;
}

RwInt32 Eff2Ut_RpClumpGetNumTri(RpClump* clump)
{
	ASSERT( clump );
	RwInt32	numtri	= 0;
	RpClumpForAllAtomics( clump, CB_GetAtomicNumTri, (LPVOID)(&numtri) );

	return numtri;
}
RwInt32 Eff2Ut_RpAtomicGetNumTri(RpAtomic* atomic)
{
	ASSERT( atomic );
	RwInt32 numtri = 0;
	CB_GetAtomicNumTri(atomic, (LPVOID)(&numtri));

	return numtri;
}

struct stClumpVertexParams
{
	RwInt32	m_lGetIndex;
	RwV3d	*m_pstDestVertex;

	stClumpVertexParams(RwInt32 index): m_lGetIndex(index), m_pstDestVertex(NULL)	{		};
};

RpAtomic *CB_GetClumpVertex(RpAtomic *pstAtomic, void *pvData)
{
	ASSERT( pstAtomic && pvData );

	stClumpVertexParams* ret = static_cast<stClumpVertexParams*>(pvData);
	if( ret->m_pstDestVertex )
		return pstAtomic;//이미 구했다.

	RpGeometry *pstGeom	= RpAtomicGetGeometry(pstAtomic);
	if(pstGeom)
	{
		RwInt32 numvtx = RpGeometryGetNumVertices(pstGeom);
		if( numvtx >= (ret->m_lGetIndex+1) )
		{
			RpMorphTarget	*pstMorph	= RpGeometryGetMorphTarget(pstGeom, 0);
			ASSERT(pstMorph);
			if (!pstMorph)
				return NULL; // error!

			ret->m_pstDestVertex = RpMorphTargetGetVertices(pstMorph);
			ASSERT(ret->m_pstDestVertex);
			ret->m_pstDestVertex += ret->m_lGetIndex;
		}

		ret->m_lGetIndex -= numvtx;
	}
	return pstAtomic;
}

RpAtomic *CB_GetAtomicNumVertices(RpAtomic *pstAtomic, void *pnAccumNum)
{
	ASSERT(pstAtomic && pnAccumNum);

	RpGeometry	*pstGeom	= RpAtomicGetGeometry(pstAtomic);
	if( pstGeom )
		*static_cast<RwInt32*>(pnAccumNum) += RpGeometryGetNumVertices(pstGeom);

	return pstAtomic;
}

RwInt32 Eff2Ut_RpClumpGetNumVertices(RpClump* clump)
{
	ASSERT( clump );
	RwInt32	numVertices	= 0;
	RpClumpForAllAtomics( clump, CB_GetAtomicNumVertices, (LPVOID)(&numVertices) );

	return numVertices;
}

const RwV3d* Eff2Ut_RpClumpGetVertex(RpClump* clump, RwInt32 index)
{
	ASSERT( clump );
	stClumpVertexParams ret(index);
	RpClumpForAllAtomics( clump, CB_GetClumpVertex, (LPVOID)(&ret) );

	if( ret.m_pstDestVertex == NULL )
	{
		static RwV3d	dbg	= { 0.f, 0.f, 0.f };
		ret.m_pstDestVertex	= &dbg;
	}
	ASSERT( ret.m_pstDestVertex );
	return ret.m_pstDestVertex;
}

RwInt32 Eff2Ut_RpAtomicGetNumVertices(RpAtomic* atomic)
{
	ASSERT( atomic );
	RwInt32 numvertices = 0;
	CB_GetAtomicNumVertices(atomic, (LPVOID)(&numvertices));

	return numvertices;
}

const RwV3d* Eff2Ut_RpAtomicGetVertex(RpAtomic* atomic, RwInt32 index)
{
	ASSERT( atomic );
	stClumpVertexParams ret(index);
	CB_GetClumpVertex(atomic, (LPVOID)(&ret));

	ASSERT(ret.m_pstDestVertex);
	return ret.m_pstDestVertex;
}

RpAtomic* Eff2Ut_AtomicModifyColor(RpAtomic *atomic, void *data)
{
	RpGeometry*	pGeometry = RpAtomicGetGeometry( atomic );
	if ( !pGeometry )		return NULL;

	RwRGBA	cNewColor = (RwRGBA)(*(RwRGBA*)data);

	RpGeometrySetFlags( pGeometry, RpGeometryGetFlags( pGeometry ) | rpGEOMETRYMODULATEMATERIALCOLOR );

	RpGeometryLock( pGeometry, rpGEOMETRYPRELIT )	;

	for (INT32 lMaterialCount = 0; lMaterialCount < RpGeometryGetNumMaterials(pGeometry); ++lMaterialCount)
	{
		RpMaterial* pMaterial = RpGeometryGetMaterial( pGeometry, lMaterialCount );
		if ( !pMaterial )		return NULL;
	
		RwRGBA	pNewRGBA;
		pNewRGBA.red	= cNewColor.red;
		pNewRGBA.green	= cNewColor.green;
		pNewRGBA.blue	= cNewColor.blue;
		pNewRGBA.alpha	=  cNewColor.alpha;

		pMaterial->surfaceProps.specular = 1.0f;		
		RpMaterialSetColor( pMaterial, &pNewRGBA );	
	}

	RpGeometryUnlock( pGeometry );
	
	return atomic;
}

RwInt32 Eff2Ut_ForAllFile( const RwChar* path, const RwChar* exp, fptrProcess pProcess, void* lpOwerClass)
				  
{
	HANDLE			hSrch	= NULL;
	WIN32_FIND_DATA	w32fd;	Eff2Ut_ZEROBLOCK(w32fd);

	BOOL			bRet	= TRUE;

	CHAR			fname[MAX_PATH]			= "";
	CHAR			dir[MAX_PATH]			= "";
	CHAR			drive[_MAX_DRIVE]		= ""; 
	CHAR			newPath[_MAX_FNAME]		= "";

	CHAR			testfname[_MAX_FNAME]	= "";
	CHAR			testexp[_MAX_FNAME]		= "";

	CHAR			currPath[MAX_PATH]		= "";

	RwInt32 ir	= 0;

	::GetCurrentDirectory(_MAX_FNAME,newPath);

	strcpy( newPath, path );
	strcat(newPath, "*");
	strcat(newPath, exp);
	ASSERT( sizeof(newPath) > strlen(newPath) );

	hSrch	= FindFirstFile( newPath, &w32fd );

	if( hSrch == INVALID_HANDLE_VALUE )	
	{
		Eff2Ut_LOG("hSrch == INVALID_HANDLE_VALUE @ Eff2Ut_ForAllFile");
		return 0;//-1;
	}

	_splitpath( newPath, drive, dir, testfname, testexp );
	wsprintf(currPath, "%s%s", drive, dir);

	while( bRet ){
		memcpy( fname, currPath, MAX_PATH );
		strcat( fname, w32fd.cFileName );

		//sub directory
		if( w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
			if( w32fd.cFileName[0] != '.' ){
				memcpy(newPath,fname, _MAX_FNAME);
				strcat(newPath, "\\*.*");

				ir	= Eff2Ut_ForAllFile(newPath, exp, pProcess, lpOwerClass);
				if( T_ISMINUS4(ir) )
				{
					Eff2Ut_ERR( "Eff2Ut_ForAllFile(subDirectory) failed" );
					break;
				}
			}
		}
		else
		{

			//cmpare with exp & load type
			_splitpath( fname, drive, dir, testfname, testexp );
			//ASSERT( !strcmp( exp, testexp ) );
			ir	= pProcess( fname, lpOwerClass );
			if( T_ISMINUS4( ir ) )
			{
				Eff2Ut_ERR( "pProcess( fname ) failed" );
				break;
			}
		}//else

		bRet	= FindNextFile(hSrch, &w32fd);
	}

	FindClose(hSrch);

	return ir;
}

void Eff2Ut_RwMatrixGetScale	( const RwMatrix& matSrc, RwMatrix* pMatScale	)
{
	ASSERT( pMatScale );

	RwV3d	vScale	= 	{	RwV3dLength ( &matSrc.right ), RwV3dLength ( &matSrc.up ), RwV3dLength ( &matSrc.at )	};
	RwMatrixScale( pMatScale, &vScale, rwCOMBINEREPLACE ); 
}

void Eff2Ut_RwMatrixGetInvScale( const RwMatrix& matSrc, RwMatrix* pMatInvScale )
{
	ASSERT( pMatInvScale );

	RwV3d	vScale	= 	{ 1.f/RwV3dLength ( &matSrc.right ), 1.f/RwV3dLength ( &matSrc.up ), 1.f/RwV3dLength ( &matSrc.at )		};
	RwMatrixScale( pMatInvScale, &vScale, rwCOMBINEREPLACE ); 
}

void Eff2Ut_RwMatrixGetNoScale( const RwMatrix& matSrc, RwMatrix* pMatNoScale	)
{
	ASSERT( pMatNoScale );

	*pMatNoScale	= matSrc;
	RwV3dNormalize ( &pMatNoScale->right, &pMatNoScale->right );
	RwV3dNormalize ( &pMatNoScale->up	, &pMatNoScale->up );
	RwV3dNormalize ( &pMatNoScale->at	, &pMatNoScale->at );
}

void Eff2Ut_RwMatrixGetRot( const RwMatrix& matSrc, RwMatrix* pMatRot	)
{
	ASSERT( pMatRot );

	*pMatRot	= matSrc;
	pMatRot->pos.x	=  pMatRot->pos.y = pMatRot->pos.z = 0.f;
}

void Eff2Ut_RwMatrixGetTrans( const RwMatrix& matSrc, RwMatrix* pMatTrans )
{
	ASSERT( pMatTrans );

	pMatTrans->flags	= matSrc.flags;
	pMatTrans->pad1		= matSrc.pad1;
	pMatTrans->pad2		= matSrc.pad2;
	pMatTrans->pad3		= matSrc.pad3;

	pMatTrans->pos		= matSrc.pos;
}

RwFrame* CB_GetHeirarchy( RwFrame *lpFrm, void *lplpHierarch )
{
	ASSERT( lpFrm );
	if( !lpFrm )		return NULL;

	RpHAnimHierarchy** pHierarchy = (RpHAnimHierarchy **)lplpHierarch;
    *pHierarchy = RpHAnimGetHierarchy(lpFrm);

    if( !(*pHierarchy) )
    {
        RwFrameForAllChildren(lpFrm, CB_GetHeirarchy, lplpHierarch);
        return lpFrm;
    }

    return (RwFrame *)NULL;
}

RpHAnimHierarchy* Eff2Ut_GetHierarchyFromRwFrame( RwFrame* pFrm )
{
	ASSERT( pFrm );
	if( !pFrm )		return NULL;

	RpHAnimHierarchy* pr = RpHAnimGetHierarchy( pFrm );
	if( !pr )
		RwFrameForAllChildren( pFrm, CB_GetHeirarchy, static_cast<void*>(&pr) );

	return pr;
}

RpAtomic* Eff2Ut_SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
#if (defined(RPSKIN310_H))
    RpSkinSetHAnimHierarchy(RpSkinAtomicGetSkin(atomic), (RpHAnimHierarchy *)data);
#else
    RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy *)data);
#endif
	return atomic;
}

RpAtomic* Eff2Ut_GetSkinHierarchy(RpAtomic *atomic, void *data)
{
#if (defined(RPSKIN310_H))
    RpSkin * const skin = RpSkinAtomicGetSkin(atomic);
    *(void **)data = (void *)RpSkinGetHAnimHierarchy(skin);
#else
    *(void **)data = (void *)RpSkinAtomicGetHAnimHierarchy(atomic);
#endif

	return NULL;
}

RpAtomic* Eff2Ut_CalcBBox(RpAtomic* atomic, void* pBBox)
{
	ASSERT( atomic && pBBox && "@ Eff2Ut_CalcBBox" );
	
	//Get the passed bounding box and current geometry
	RwBBox *bbPassed = reinterpret_cast<RwBBox*>(pBBox);
	RpGeometry *pGeometry = RpAtomicGetGeometry(atomic);
	
	if(pGeometry)
	{
		RpMorphTarget *pMorphTarget = RpGeometryGetMorphTarget(pGeometry, 0);
		
		if(pMorphTarget)
		{
			//Calculate the bounding box of the current atomic's geometry
			RwBBox bbThis;
			RwBBoxCalculate(&bbThis, RpMorphTargetGetVertices(pMorphTarget), RpGeometryGetNumVertices(pGeometry));
			
			//Extend the bounding box if this atomic outlies it
			if(bbThis.sup.x > bbPassed->sup.x) bbPassed->sup.x = bbThis.sup.x;
			if(bbThis.sup.y > bbPassed->sup.y) bbPassed->sup.y = bbThis.sup.y;
			if(bbThis.sup.z > bbPassed->sup.z) bbPassed->sup.z = bbThis.sup.z;
			
			if(bbThis.inf.x < bbPassed->inf.x) bbPassed->inf.x = bbThis.inf.x;
			if(bbThis.inf.y < bbPassed->inf.y) bbPassed->inf.y = bbThis.inf.y;
			if(bbThis.inf.z < bbPassed->inf.z) bbPassed->inf.z = bbThis.inf.z;
		}
	}
	return atomic;
}

RpClump*  Eff2Ut_CalcBBox(RpClump* clump, void* pBBox)
{
	ASSERT( clump && pBBox && "@ Eff2Ut_CalcBBox" );
	RpClumpForAllAtomics(clump, Eff2Ut_CalcBBox, static_cast<void*>(pBBox));
	return clump;
}

void Eff2Ut_CalcSphere(RwSphere* pout, const RwBBox* pin)
{
	ASSERT( pout && pin && "@ Eff2Ut_CalcSphere" );

	RwBBox BBox = *pin;

	RwV3dAdd( &pout->center, &BBox.sup, &BBox.inf );
	pout->center.x = pout->center.x *0.5f;
	pout->center.y = pout->center.y *0.5f;
	pout->center.z = pout->center.z *0.5f;

	RwV3dSub( &BBox.sup, &BBox.sup, &pout->center );
	pout->radius	= RwV3dLength( &BBox.sup );
}

RpAtomic* Eff2Ut_CalcSphere(RpAtomic* atomic, void* pSphere)
{
	RwBBox BBox = {{-9999.0f,-9999.0f,-9999.0f},{9999.0f,9999.0f,9999.0}};
	if( Eff2Ut_CalcBBox( atomic, static_cast<void*>(&BBox) ) )
	{
		RwSphere* lpRwSphere	= reinterpret_cast<RwSphere*>(pSphere);
		Eff2Ut_CalcSphere(lpRwSphere, &BBox);
	}
	return atomic;
}

RpClump*  Eff2Ut_CalcSphere(RpClump* clump, void* pSphere)
{
	RwBBox BBox = {{-9999.0f,-9999.0f,-9999.0f},{9999.0f,9999.0f,9999.0}};

	if( Eff2Ut_CalcBBox( clump, static_cast<void*>(&BBox) ) )
	{
		RwSphere* lpRwSphere	= reinterpret_cast<RwSphere*>(pSphere);

		Eff2Ut_CalcSphere(lpRwSphere, &BBox);
	}
	return clump;
}


///////////////////////////////////////////////////////////////////////////////
// Eff2Ut_FindEffTex
// RwTextureSetName( pTexR, szTex + szMask );
///////////////////////////////////////////////////////////////////////////////
RwTexture*	Eff2Ut_FindEffTex(  const char* szTex
					   , const char* szMask		
					   , char* szPath
					   , AgcmResourceLoader* pResLoader
					   , RwUInt32 dwFillterMode )
{
	ASSERT( szTex			&& \
			strlen( szTex )	&& \
			"szTex==NULL || strlen( szTex )==0 @  Eff2Ut_FindEffTex");
	if( !szTex	||
		!strlen(szTex) ){
		//err
		return NULL;
	}

	pResLoader->SetTexturePath( szPath );

	RwTexture*	pTexR	= NULL;

	if( szMask &&
		strlen(szMask) ){// with mask

		char	szTemp[MAX_PATH]	= "";
		strcpy( szTemp, szTex );
		strcat( szTemp, szMask );
		ASSERT( sizeof(szTemp) > strlen(szTemp) );

		pTexR	= RwTextureRead( szTemp, NULL );

		if( pTexR ){// 있으면 바로 리턴..( 필터 모드는? )
			if( dwFillterMode == (RwUInt32)RwTextureGetFilterMode( pTexR ) ){
				ASSERT( !IsBadReadPtr( pTexR, sizeof( RwTexture ) ) && "::Eff2Ut_FindEffTex()" );
				return pTexR;
			}else{
				//이런 경우는 어떻게 하지?
				Eff2Ut_ERR( "dwFillterMode != (RwUInt32)RwTextureGetFilterMode( pTexR )" );

				pTexR	= RwTextureSetFilterMode( pTexR, (RwTextureFilterMode)dwFillterMode );
				ASSERT(pTexR && "RwTextureSetFilterMode() failed @ Eff2Ut_FindEffTex");
				ASSERT( !IsBadReadPtr( pTexR, sizeof( RwTexture ) ) && "::Eff2Ut_FindEffTex()" );
				return pTexR;
			}
		}
		else
		{
			pTexR	= RwTextureRead( szTex, szMask );
			if( pTexR )
			{// 이름 바꾸고..
				pTexR	= RwTextureSetName( pTexR, szTemp );
				if( !pTexR )
				{
					//err
					Eff2Ut_ERR("RwTextureSetName() failed @ Eff2Ut_FindEffTex");
					return NULL;
				}
			}
			else
			{
				//err
				Eff2Ut_ERR("RwTextureRead() failed @ Eff2Ut_FindEffTex");
				return NULL;
			}
		}
	}
	else
	{ // without mask
		pTexR	= RwTextureRead( szTex, NULL );
		if( !pTexR )
		{
			Eff2Ut_ERR("RwTextureRead() failed @ Eff2Ut_FindEffTex");
		}
	}

	if( pTexR )
	{
		//필터모드 셋
		pTexR	= RwTextureSetFilterMode( pTexR, (RwTextureFilterMode)dwFillterMode );
		ASSERT(pTexR && "RwTextureSetFilterMode() failed @ Eff2Ut_FindEffTex");
		ASSERT( !IsBadReadPtr( pTexR, sizeof( RwTexture ) ) && "::Eff2Ut_FindEffTex()" );

		RwTextureSetAddressingV( pTexR, rwTEXTUREADDRESSWRAP );
		RwTextureSetAddressingU( pTexR, rwTEXTUREADDRESSWRAP );
		RwTextureSetAddressing ( pTexR, rwTEXTUREADDRESSWRAP );
	}


	return pTexR;
}

///////////////////////////////////////////////////////////////////////////////
// Eff2Ut_SAFE_DESTROY_FRAME
// RwFrame destory
///////////////////////////////////////////////////////////////////////////////
RwFrame* FrameUnLink( RwFrame* pFrmChild, void* pVoid )
{
	pVoid;
	ASSERT(pFrmChild);

	RwFrameRemoveChild( pFrmChild );
	
	return pFrmChild;
}

void Eff2Ut_SAFE_DESTROY_FRAME( RwFrame*& pFrm )
{
	if( pFrm )
	{
		RwFrameRemoveChild( pFrm );
		RwFrameForAllChildren( pFrm, FrameUnLink, NULL );
		RwFrameDestroy( pFrm );
		pFrm	= NULL;
	}
}

void Eff2Ut_SAFE_DESTROY_TEX( RwTexture*& pTex )
{
	if( pTex )
	{
		RwTextureDestroy ( pTex );
		pTex = NULL;
	}
}

void Eff2Ut_SAFE_DESTROY_SPLINE( RpSpline*& pSpline )
{
	if( pSpline )
	{
		RpSplineDestroy ( pSpline );
		pSpline	= NULL;
	}
}

void Eff2Ut_SAFE_DESTROY_RTANIM( RtAnimAnimation*& pRtAnim )
{
	if( pRtAnim )
	{
		RtAnimAnimationDestroy ( pRtAnim );
		pRtAnim	= NULL;
	}
}

void Eff2Ut_SAFE_DESTROY_CLUMP( RpClump*& pClump )
{
	if( pClump )
	{
		RpClumpDestroy ( pClump );
		pClump	= NULL;
	}
}

void Eff2Ut_SAFE_DESTROY_LIGHT( RpLight*& pLight )
{
	if( pLight )
	{
		RwFrame* pFrm	= RpLightGetFrame( pLight );
		RpLightSetFrame(pLight, NULL);
		Eff2Ut_SAFE_DESTROY_FRAME( pFrm );
	}
}

char* Eff2Ut_ShowRwMat( const RwMatrix* pMat, LPCSTR szMatName )
{
	static char szOut[1024] = "";
	sprintf( szOut, "== RwMatrix NAME : %s ==\r\n"
					"%6s : %10.4f,   %10.4f,   %10.4f,   %6s : %8d\r\n"
					"%6s : %10.4f,   %10.4f,   %10.4f,   %6s : %8d\r\n"
					"%6s : %10.4f,   %10.4f,   %10.4f,   %6s : %8d\r\n"
					"%6s : %10.4f,   %10.4f,   %10.4f,   %6s : %8d\r\n"
			, szMatName ? szMatName : "unknown" 
			, "right", pMat->right.x, pMat->right.y	, pMat->right.z	, "flag", pMat->flags
			, "up"	 , pMat->up.x	, pMat->up.y	, pMat->up.z	, "pad1", pMat->pad1
			, "at"	 , pMat->at.x	, pMat->at.y	, pMat->at.z	, "pad2", pMat->pad2
			, "pos"	 , pMat->pos.x	, pMat->pos.y	, pMat->pos.z	, "pad3", pMat->pad3);
	
	return szOut;
}

char* Eff2Ut_ShowRwV3d( const RwV3d* pV3d, LPCSTR szVecName )
{
	static char szOut[1024] = "";
	sprintf(  szOut, "== RwV3d NAME : %s ==\r\nx : %8.4f,	y : %8.4f, z : %8.4f\r\n"
			, szVecName ? szVecName : "unknown"
			, pV3d->x, pV3d->y, pV3d->z);
	return szOut;
}

RwInt32	Eff2Ut_RenderBBox( const RwBBox& bbox, const RwFrame* pFrm )
{
	static const RwUInt32	dwColr	= 0xffffff00;// ( a, r, g, b ) -> yellow
	static const RwReal		fval	= 0.5f;
	static RwIm3DVertex		avtx[8]	= {
		//{objVertex, objNormal, color, u, v},
		{ {-fval,  fval, -fval}, {0.f, 0.f, 0.f}, dwColr, 0.f, 0.f, },	//0
		{ { fval,  fval, -fval}, {0.f, 0.f, 0.f}, dwColr, 0.f, 0.f, },	//1
		{ {-fval,  fval,  fval}, {0.f, 0.f, 0.f}, dwColr, 0.f, 0.f, },	//2
		{ { fval,  fval,  fval}, {0.f, 0.f, 0.f}, dwColr, 0.f, 0.f, },	//3
		
		{ {-fval, -fval, -fval}, {0.f, 0.f, 0.f}, dwColr, 0.f, 0.f, },	//4
		{ { fval, -fval, -fval}, {0.f, 0.f, 0.f}, dwColr, 0.f, 0.f, },	//5
		{ {-fval, -fval,  fval}, {0.f, 0.f, 0.f}, dwColr, 0.f, 0.f, },	//6
		{ { fval, -fval,  fval}, {0.f, 0.f, 0.f}, dwColr, 0.f, 0.f, },	//7
	};
	static RwImVertexIndex	aindice[24] = {		
		0,1, 1,3, 3,2, 2,0,		//top
		0,4, 1,5, 3,7, 2,6,		//side
		4,5, 5,7, 7,6, 6,4,		//bottom
	};

	RwMatrix	mat;
	if( pFrm )
	{
		mat = *RwFrameGetLTM ( const_cast<RwFrame*>(pFrm) );
	}
	else
		RwMatrixSetIdentity ( &mat );

	RwV3d		vPos	= { (bbox.sup.x + bbox.inf.x) * .5f,
							(bbox.sup.y + bbox.inf.y) * .5f,
							(bbox.sup.z + bbox.inf.z) * .5f };

	RwV3d		vScale	= {	bbox.sup.x - bbox.inf.x,
							bbox.sup.y - bbox.inf.y,
							bbox.sup.z - bbox.inf.z,};

	RwMatrixTranslate( &mat, &vPos, rwCOMBINEPRECONCAT );
	RwMatrixScale( &mat, &vScale, rwCOMBINEPRECONCAT );
	
    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);

    if( RwIm3DTransform(avtx, 8, &mat, rwIM3D_ALLOPAQUE) )
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, aindice, 24);
		RwIm3DEnd();
    }

    RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);

	return 0;
}

RwInt32	Eff2Ut_RenderBSphere( const RwSphere& bsphere, const RwUInt32 colr, const RwFrame* pFrm )
{
	static const RwInt32	NumAngle	= 13;
	static const RwInt32	NumVtx		= NumAngle*(NumAngle+NumAngle)+1;

	static RwIm3DVertex		avtx[NumVtx];
	static RwBool			bFirst(TRUE);

	int nc, i, j , k;

	for( nc = 0; nc<NumVtx; ++nc )
		avtx[nc].color = colr;
	//Initialize
	if(bFirst){
		bFirst	= FALSE;

		RwReal fStep	= DEF_2PI/static_cast<RwReal>(NumAngle+NumAngle);
		RwReal fCurr	= DEF_D2R(90.f);

		RwIm3DVertex	temp	= { {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, colr, 0.f, 0.f, };
		
		RwIm3DVertex		atempvtx[NumAngle+NumAngle];
		memset( atempvtx, 0, sizeof( atempvtx ) );

		for( i=0; i<NumAngle+NumAngle; ++i ){
			
			atempvtx[i].objVertex.x	= static_cast<RwReal>(cos(fCurr));
			atempvtx[i].objVertex.y	= static_cast<RwReal>(sin(fCurr));

			fCurr	+= fStep;
		}

		RwMatrix	matR;
		RwV3d		axis	= { 0.f, 1.f, 0.f };

		fStep	= 360.f/static_cast<RwReal>(NumAngle);
		fCurr	= fStep;
		i=0;
		for( j=0; j<NumAngle; ++j, fCurr += fStep ){
			RwMatrixRotate( &matR, &axis, fCurr, rwCOMBINEREPLACE );
			
			for( k=0; k<NumAngle+NumAngle; ++k, ++i ){
				RwV3dTransformPoint( &temp.objVertex, &atempvtx[k].objVertex, &matR );
				avtx[i]	= temp;
			}
		}
		temp.objVertex = atempvtx[0].objVertex;
		avtx[i]	= temp;
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);

	RwV3d		vScale	= { bsphere.radius, bsphere.radius, bsphere.radius };
	RwMatrix	mat;
	RwMatrixScale( &mat, &vScale, rwCOMBINEREPLACE );

	if( pFrm )
	{
		RwMatrix*	pLTM = RwFrameGetLTM( const_cast<RwFrame*>(pFrm) );

		RwV3d	vPos	= { bsphere.center.x + RwMatrixGetPos( pLTM )->x,
							bsphere.center.y + RwMatrixGetPos( pLTM )->y,
							bsphere.center.z + RwMatrixGetPos( pLTM )->z};
		RwMatrixTranslate( &mat, &vPos, rwCOMBINEPOSTCONCAT );
	}
	else
	{
		RwMatrixTranslate( &mat, &bsphere.center, rwCOMBINEPOSTCONCAT );
	}

    if( RwIm3DTransform(avtx, NumVtx, &mat, rwIM3D_ALLOPAQUE) )
	{
        RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
        RwIm3DEnd();
    }

    RwRenderStateSet( rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD );
	return 0;
}

const char* Eff2Ut_GetTimeStamp(void)
{
	static char buff[MAX_PATH]	= "";
	
	SYSTEMTIME st;	
	memset(&st, 0, sizeof(st));
	
	GetLocalTime(&st);
	sprintf(buff,"ymdt : %4d,%2d,%2d  %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		
	return buff;
}

int	Eff2Ut_TimeStampToFile(FILE* fp)
{
	if( !fp )
		return -1;
	int ir	= 0;
	ir	= 
	fputs("\r\n/------------------------------------------------------------------------------\r\n", fp);
	if( ir == EOF )
		return -1;
	ir	= 
	fputs(Eff2Ut_GetTimeStamp(), fp);
	if( ir == EOF )
		return -1;
	ir	= 
	fputs("\r\n------------------------------------------------------------------------------/\r\n", fp);
	if( ir == EOF )
		return -1;
	return 0;
}

int	Eff2Ut_TimeStampToFile(const char* fname)
{
	FILE*	fp	= fopen(fname, "a");
	if( fp )
	{
		int ir = Eff2Ut_TimeStampToFile(fp);
		fclose( fp );
		return ir;
	}
	else
		return -1;
}

#include <dxerr.h>
#pragma comment(lib, "Dxerr.lib")
void Eff2Ut_DbgToFile( const char* outputfile, const char* srcfile, int srcline, const char* msg, const char* func )
{
	AuAutoFile fp( outputfile, "a" );
	if( fp )
	{
		fprintf( fp
			   , "\r\n====> BEGIN\r\nfile : %s\r\nline : %d\r\nfunc : %s\r\ninfo : %s\r\n<==== END\r\n"
			   , srcfile
			   , srcline
			   , func ? func : "unknown"
			   , msg ? msg : "unknown" );
	}
	else
		ASSERT( !"fopen failed @ Eff2Ut_DbgToFile" );
}

void Eff2Ut_ToFile( LPCSTR fname, LPCSTR msg )
{
	FILE* fp	= fopen( fname, "a" );
	if( fp )
	{
		fputs( msg, fp );
		fclose(fp);
	}
}

class Eff2Ut_CCON
{
	HANDLE	m_hCon;

	Eff2Ut_CCON(LPCSTR szTitle="msg window")
	{
		AllocConsole();
		m_hCon = GetStdHandle( STD_OUTPUT_HANDLE);
		if( m_hCon )
		{
			::SetConsoleTitle( szTitle );
		}
	}
public:
	~Eff2Ut_CCON()
	{
		::FreeConsole();
	}

	void ShowMsg(LPCSTR msg)
	{
		::WriteConsole( m_hCon, (void*)msg, strlen(msg), NULL, NULL );
	}

	static Eff2Ut_CCON& bGetInst();
};

Eff2Ut_CCON& Eff2Ut_CCON::bGetInst()
{
	static Eff2Ut_CCON	ccon;
	return ccon;
}

void Eff2Ut_ToConsol( LPCSTR msg )
{
	Eff2Ut_CCON::bGetInst().ShowMsg(msg);
}

void Eff2Ut_ToMsgBox( LPCSTR msg, HWND hWnd )
{
	::MessageBox( hWnd, msg, "Eff2Ut_ToMsgBox", MB_OK );
}

void Eff2Ut_ToWnd( const char* msg, int nx, int ny, HWND hWnd )
{
	HDC	dc	= ::GetDC(hWnd);
	::TextOut( dc, nx, ny, msg, strlen( msg ) );
	::ReleaseDC(hWnd, dc);
}

const char* Eff2Ut_DxErrMSG(HRESULT hr)
{
	static char	buff[256]		= "";
	Eff2Ut_ZEROBLOCK( buff );

	//wsprintf( buff, "Error code: 0x%x ====>> DX err msg : %s", hr, DXGetErrorString( hr ) );
	return buff;
}

const char* Eff2Ut_FmtMsg( const char* szFmt, ... )
{
	static char	buff[1024]		= "";
	Eff2Ut_ZEROBLOCK( buff );

	int			bufcnt			= 0;
	DWORD		dwCntWritten	= 0;

	va_list	ap;

	va_start(ap, szFmt);
	bufcnt	= vsprintf(buff, szFmt, ap);
	va_end(ap);

	if( bufcnt > 1024 ){
		//err
	}

	return buff;
}

RwChar *Eff2Ut_EffectPathnameCreate(const RwChar *srcBuffer)
{
    RwChar *dstBuffer;
    RwChar *charToConvert;

    /* 
     * First duplicate the string 
     */
    dstBuffer = (RwChar *)malloc(sizeof(RwChar) * (strlen(srcBuffer) + 1));

    if( dstBuffer )
    {
        strcpy(dstBuffer, srcBuffer);

        /* 
         * Convert a path for use on Windows. 
         * Convert all /s and :s into \s 
         */
        while( (charToConvert = strchr(dstBuffer, '/')) )
        {
            *charToConvert = '\\';
        }
#if 0
        while( (charToConvert = strchr(dstBuffer, ':')) )
        {
            *charToConvert = '\\';
        }
#endif
    }

    return dstBuffer;
}

void	Eff2Ut_EffectPathnameDestroy(RwChar *buffer)
{
    if( buffer )
        free(buffer);
}

RwInt32	Eff2Ut_RenderSphereXZ(const RwSphere& stSphere, const RwUInt32 nColor, const RwFrame* pFrm)
{
	static const RwInt32	NumAngle	= 13;
	static const RwInt32	NumVtx		= (NumAngle+NumAngle)+1;

	static RwIm3DVertex		avtx[NumVtx];
	static RwBool			bFirst(TRUE);

	int nc, i, k;

	for( nc = 0; nc<NumVtx; ++nc )
		avtx[nc].color = nColor;

	if(bFirst)
	{
		bFirst	= FALSE;

		RwReal fStep	= DEF_2PI / static_cast<RwReal>(NumAngle+NumAngle);
		RwReal fCurr	= DEF_D2R(90.f);

		RwIm3DVertex	temp	= { {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, };

		RwIm3DVertex		atempvtx[NumAngle + NumAngle];
		memset( atempvtx, 0, sizeof( atempvtx ) );

		for( i=0; i<NumAngle+NumAngle; ++i )
		{
			atempvtx[i].objVertex.x	= static_cast<RwReal>(cos(fCurr));
			atempvtx[i].objVertex.z	= static_cast<RwReal>(sin(fCurr));

			fCurr += fStep;
		}

		i = 0;

		for( k=0; k<NumAngle+NumAngle; ++k, ++i ){
			temp.objVertex = atempvtx[k].objVertex;
			avtx[i] = temp;
		}

		temp.objVertex = atempvtx[0].objVertex;
		avtx[i]	= temp;
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);

	RwV3d		vScale	= { stSphere.radius, stSphere.radius, stSphere.radius };
	RwMatrix	mat;
	RwMatrixScale( &mat, &vScale, rwCOMBINEREPLACE );

	if( pFrm )
	{
		RwMatrix*	pLTM = RwFrameGetLTM( const_cast<RwFrame*>(pFrm) );
		RwV3d	vPos	= { stSphere.center.x + RwMatrixGetPos( pLTM )->x,
			stSphere.center.y + RwMatrixGetPos( pLTM )->y,
			stSphere.center.z + RwMatrixGetPos( pLTM )->z};

		RwMatrixTranslate( &mat, &vPos, rwCOMBINEPOSTCONCAT );
	}
	else
	{
		RwMatrixTranslate( &mat, &stSphere.center, rwCOMBINEPOSTCONCAT );
	}

	if( RwIm3DTransform(avtx, NumVtx, &mat, rwIM3D_ALLOPAQUE) )
	{
		RwIm3DRenderPrimitive(rwPRIMTYPEPOLYLINE);
		RwIm3DEnd();
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);

	return 0;
}

RwInt32	Eff2Ut_RenderBoxXZ(const RwBBox& stBox, const RwUInt32 nColor, const RwFrame* pFrm)
{
	//static const RwUInt32	dwColr	= 0xffffff00;// ( a, r, g, b ) -> yellow
	static const RwReal		fval	= 0.5f;
	static RwIm3DVertex		avtx[8]	= {
		//{objVertex, objNormal, color, u, v},
		{ {-fval,  fval, -fval}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, },	//0
		{ { fval,  fval, -fval}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, },	//1
		{ {-fval,  fval,  fval}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, },	//2
		{ { fval,  fval,  fval}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, },	//3

		{ {-fval, -fval, -fval}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, },	//4
		{ { fval, -fval, -fval}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, },	//5
		{ {-fval, -fval,  fval}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, },	//6
		{ { fval, -fval,  fval}, {0.f, 0.f, 0.f}, nColor, 0.f, 0.f, },	//7
	};
	//static RwImVertexIndex	aindice[24] = {		
	static RwImVertexIndex	aindice[8] = {
		//0,1, 1,3, 3,2, 2,0,		//top
		//0,4, 1,5, 3,7, 2,6,		//side
		4,5, 5,7, 7,6, 6,4,		//bottom
	};

	for ( int i = 0; i < 8; i++ )
		avtx[i].color = nColor;

	RwMatrix	mat;//		= *RwFrameGetLTM ( const_cast<RwFrame*>(pFrm) );

	if( pFrm )
		mat = *RwFrameGetLTM ( const_cast<RwFrame*>(pFrm) );
	else
		RwMatrixSetIdentity ( &mat );

	RwV3d	vScale	= {	stBox.sup.x - stBox.inf.x, stBox.sup.y - stBox.inf.y, stBox.sup.z - stBox.inf.z,};
	RwV3d	vPos	= { (stBox.sup.x + stBox.inf.x) * .5f,
						(stBox.sup.y + stBox.inf.y) * .5f,
						(stBox.sup.z + stBox.inf.z) * .5f };

	RwMatrixTranslate( &mat, &vPos, rwCOMBINEPRECONCAT );
	RwMatrixScale( &mat, &vScale, rwCOMBINEPRECONCAT );

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEFLAT);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void *)NULL);

	if( RwIm3DTransform(avtx, 8, &mat, rwIM3D_ALLOPAQUE) )
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, aindice, 8);
		RwIm3DEnd();
	}

	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void *)rwSHADEMODEGOURAUD);

	return 0;
}
