// AgcuCamConstrain.cpp: implementation of the AgcuCamConstrain class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcuEffUtil.h"
#include "AgcdEffGlobal.h"
#include "AgcuCamConstrain.h"
#include "AcuMathFunc.h"

#include <d3dx9math.h>

//-----------------------------------------------------------------------------
//
// AgcdCamConstrain_Terrain
//
//-----------------------------------------------------------------------------
AgcdCamConstrain_Terrain::stTile::stTile()
{
	memset(this, 0, sizeof(*this));
	m_bTurnEdge	= true;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AgcdCamConstrain_Terrain::AgcdCamConstrain_Terrain()
{
};
AgcdCamConstrain_Terrain::~AgcdCamConstrain_Terrain()
{
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcdCamConstrain_Terrain::bPreChkTerrain( AgcdCamConstrain_Obj* pAgcdComConstrain_Obj
											  , AgcmMap* pAgcmMap
											  , const RwV3d& eye
											  , const RwV3d& at )
{
	ASSERT( pAgcdComConstrain_Obj && pAgcmMap );

	static int cntmax = 0;
	m_nCnt	= 0;

	pAgcdComConstrain_Obj->m_nCnt = 0;

	const	RwReal	TILESIZE	= MAP_STEPSIZE;
	const	RwReal	INVSTILESIZE= 1.f/MAP_STEPSIZE;//1.f/400.f;//
	const	RwReal	THALFSIZE	= MAP_STEPSIZE/2.f;
	const	RwReal	TRADIUS		= sqrtf(2.f)*THALFSIZE;

	int	eyex	= (int)((eye.x	- ((eye.x<0.f) ? TILESIZE : 0.f) ) / MAP_STEPSIZE);
	int	atx		= (int)((at.x	- ((at.x<0.f)  ? TILESIZE : 0.f) ) / MAP_STEPSIZE);
	int	eyez	= (int)((eye.z	- ((eye.z<0.f) ? TILESIZE : 0.f) ) / MAP_STEPSIZE);
	int	atz		= (int)((at.z	- ((at.z<0.f)  ? TILESIZE : 0.f) ) / MAP_STEPSIZE);
	//POINT	ptstart, ptend;
	POINT	ptstart, ptend;
	if( eyex > atx )
		ptstart.x = atx, ptend.x = eyex;
	else
		ptstart.x = eyex, ptend.x = atx;
	if( eyez > atz )
		ptstart.y = atz, ptend.y = eyez;
	else
		ptstart.y = eyez, ptend.y = atz;


	const RwV2d	v2Eye	= {eye.x, eye.z};
	RwV2d		v2Ray	= {at.x-eye.x, at.z-eye.z};
	RwV2dNormalize (&v2Ray, &v2Ray);

	stTile	tile;
	for( int i=ptstart.x; i<ptend.x+1; ++i )
	{
		for( int j=ptstart.y; j<ptend.y+1; ++j )
		{
			tile.m_ltop.x = (float)(i)*TILESIZE;
			tile.m_ltop.z = (float)(j)*TILESIZE;

			//prechk by sphere
			RwV2d	centerSubEye =	{ (tile.m_ltop.x+THALFSIZE) - v2Eye.x 
									, (tile.m_ltop.z+THALFSIZE) - v2Eye.y };
			if( fabsf(NSACUMATH::V2dCrossProduct(&v2Ray, &centerSubEye)) > TRADIUS )
				continue;

			tile.m_ltop.y = pAgcmMap->HP_GetHeightGeometryOnly(tile.m_ltop.x, tile.m_ltop.z);

			tile.m_rtop.x = tile.m_ltop.x + TILESIZE;
			tile.m_rtop.z = tile.m_ltop.z;
			tile.m_rtop.y = pAgcmMap->HP_GetHeightGeometryOnly(tile.m_rtop.x, tile.m_rtop.z);

			tile.m_lbtm.x = tile.m_ltop.x;
			tile.m_lbtm.z = tile.m_ltop.z + TILESIZE;
			tile.m_lbtm.y = pAgcmMap->HP_GetHeightGeometryOnly(tile.m_lbtm.x, tile.m_lbtm.z);

			tile.m_rbtm.x = tile.m_rtop.x;
			tile.m_rbtm.z = tile.m_lbtm.z;
			tile.m_rbtm.y = pAgcmMap->HP_GetHeightGeometryOnly(tile.m_rbtm.x, tile.m_rbtm.z);


			//----------------------------------------------------------------------------
			{
				ApWorldSector* pSector = pAgcmMap->m_pcsApmMap->GetSector( tile.m_ltop.x , tile.m_ltop.z );
				if( !pSector )//err
					continue;
				RwInt32 nx = (RwInt32)((tile.m_ltop.x - pSector->GetXStart() + THALFSIZE ) * INVSTILESIZE );
				RwInt32 nz = (RwInt32)((tile.m_ltop.z - pSector->GetZStart() + THALFSIZE ) * INVSTILESIZE );

				#ifdef USE_MFC
				ApDetailSegment* pSegment = pSector->D_GetSegment( nx, nz );
				#else
				ApCompactSegment* pSegment = pSector->C_GetSegment( nx, nz );
				#endif

				//bTurnEdge
				if( pSegment )
				{
					tile.m_bTurnEdge = pSegment->stTileInfo.GetEdgeTurn()!=0;
				}
				else
				{
					// 그래도 해주는게 좋아용..
					tile.m_bTurnEdge = TRUE;
				}


				//get objlist
				pAgcdComConstrain_Obj->m_nCnt += 
					pSector->GetObjectCollisionID( nx, nz, &pAgcdComConstrain_Obj->m_objlist[pAgcdComConstrain_Obj->m_nCnt] , AgcdCamConstrain_Obj::e_maxObj - pAgcdComConstrain_Obj->m_nCnt );

				//@{ kday 20051005
				// ;)
				// TODO : 추구 stl::vector + std::unique 로 개선
				pAgcdComConstrain_Obj->m_nCnt += 
					pSector->GetIndex( ApWorldSector::AWS_RIDABLEOBJECT , nx, nz, &pAgcdComConstrain_Obj->m_objlist[pAgcdComConstrain_Obj->m_nCnt] , AgcdCamConstrain_Obj::e_maxObj - pAgcdComConstrain_Obj->m_nCnt );
				//@} kday


			}
			//----------------------------------------------------------------------------

			// 마고자 일반 배열이랑 쓰는게 똑같기때문에 오버플로우 돼는것에 대한 처리를 할수없다.
			// 일단은 사전에 카운트 조사로 에러 예방. 
			if( m_nCnt < e_maxtile )
			{
				m_tilelist[m_nCnt++] = tile;
			}
			
#ifdef _DEBUG
			if( cntmax < m_nCnt )
			{
				cntmax = m_nCnt;
				//Eff2Ut_ToConsol( Eff2Ut_FmtMsg( "최대 투영 타일 갯수.. : %d\n", cntmax ) );
			}
#endif //_DEBUG
		}
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
RwBool AgcdCamConstrain_Terrain::bChkTerrain( RwReal* pfDist//RwV3d* pv3Out
										   , const RwV3d& eye
										   , const RwV3d& at
										   , const RwV3d& uv3Ray )
{
	ASSERT( pfDist );

	RwBool	br	= FALSE;
	RwV3d	EyeSubAt;
	RwV3dSub(&EyeSubAt, &eye, &at);
	RwReal	flen = RwV3dLength(&EyeSubAt);

	RwV3d	v0,v1,v2;
	RwReal	fDist=0.f, fMaxDist=0.f;
	RwReal	fu=0.f, fv=0.f;
	RwInt32	nIntersectIndex = -1;

	for( int i=0; i < m_nCnt && i < e_maxtile ; ++i )
	{
		if( !m_tilelist[i].m_bTurnEdge )
		{
			v0 = m_tilelist[i].m_ltop;
			v1 = m_tilelist[i].m_rtop;
			v2 = m_tilelist[i].m_lbtm;
		}
		else
		{
			v0 = m_tilelist[i].m_ltop;
			v1 = m_tilelist[i].m_rbtm;
			v2 = m_tilelist[i].m_lbtm;
		}

		if( !br )
		{
			br = IntersectTriangle(&uv3Ray, &eye, &v0, &v1, &v2, &fMaxDist, &fu, &fv);			
			if( fMaxDist > 0.f && br )
			{
				if( fMaxDist > flen )
				{
					br = false;
					continue;
				}
			}
			else
			{
				br = false;
			}
		}
		else
		{
			if( IntersectTriangle(&uv3Ray, &eye, &v0, &v1, &v2, &fDist, &fu, &fv) )
			{
				if( fDist > 0.f && fDist > fMaxDist && fDist < flen )
				{
					fMaxDist = fDist;
				}
			}
		}
		
		if( !m_tilelist[i].m_bTurnEdge )
		{
			v0 = m_tilelist[i].m_rtop;
			v1 = m_tilelist[i].m_rbtm;
			v2 = m_tilelist[i].m_lbtm;
		}
		else
		{
			v0 = m_tilelist[i].m_ltop;
			v1 = m_tilelist[i].m_rtop;
			v2 = m_tilelist[i].m_rbtm;
		}

		if( !br )
		{
			br = IntersectTriangle(&uv3Ray, &eye, &v0, &v1, &v2, &fMaxDist, &fu, &fv);
			if( fMaxDist > 0.f && br )
			{
				if( fMaxDist > flen )
				{
					br = false;
					continue;
				}
			}
			else
			{
				br = false;
			}
		}
		else
		{
			if( IntersectTriangle(&uv3Ray, &eye, &v0, &v1, &v2, &fDist, &fu, &fv) )
			{
				if( fDist > 0.f && fDist > fMaxDist && fDist < flen )
				{
					fMaxDist = fDist;
				}
			}
		}
	}

	*pfDist = fMaxDist;

	return (br);
};
//-----------------------------------------------------------------------------
//
// AgcdCamConstrain_Terrain
//
//-----------------------------------------------------------------------------
AgcdCamConstrain_Obj::AgcdCamConstrain_Obj()
	: m_nCnt(0),m_bIntsct(FALSE),m_fDist(0.f)
{
}
AgcdCamConstrain_Obj::~AgcdCamConstrain_Obj()
{
}
void AgcdCamConstrain_Obj::bInit(RwReal fDist)
{
	m_bIntsct = FALSE;
	m_fDist = fDist;
}
#include "AgcmCamera2.h"

BOOL AgcdCamConstrain_Obj::vCheckedObj(RwInt32 nIndex, RwInt32 nObjID)
{
	ASSERT( nIndex < m_nCnt );
	for( RwInt32 i=0; i<nIndex; ++i )
		if( m_objlist[i] == nObjID )
			return TRUE;

	return FALSE;
};

BOOL AgcdCamConstrain_Obj::bIntsct(ApmObject* pApmObject
								   , AgcmObject* pAgcmObject
								   , RpIntersection* intersection)
{
	ASSERT(pApmObject && pAgcmObject);

	for( RwInt32 i=0; i < m_nCnt && i < e_maxObj ; ++i )
	{
		if( vCheckedObj(i, m_objlist[i]) )
			continue;

		ApdObject*	pstApdObject	= pApmObject->GetObject(m_objlist[i]);
		if( !pstApdObject )
			continue;
		AgcdObject*	pstAgcdObject	= pAgcmObject->GetObjectData(pstApdObject);
		if( !pstAgcdObject 
		 || !pstAgcdObject->m_pstCollisionAtomic

		 //@{ kday 20050614
		 // ;)
		 // TODO : 오브젝트 플래그 추가시 작업하자.
		 //|| !(pstAgcdObject->m_pstTemplate->m_lObjectType & (ACUOBJECT_TYPE_CAM_ZOOM | ACUOBJECT_TYPE_CAM_ALPHA))
		 //@} kday

		 )
			continue;

		RwSphere sphere = *RpAtomicGetWorldBoundingSphere(pstAgcdObject->m_pstCollisionAtomic);
		RwReal	fdist = 0.f;
		
		if( RtLineSphereIntersectionTest(
				&intersection->t.line
				, &sphere
				, &fdist 				
				)
		  )
		{
			CBIntersectObj(intersection, NULL, pstAgcdObject->m_pstCollisionAtomic, fdist, this);
		}
	}

	return m_bIntsct;
}

RpAtomic* AgcdCamConstrain_Obj::CBIntersectObj(
	  RpIntersection*	intersection
	, RpWorldSector*	sector
	, RpAtomic*			atom
	, RwReal			dist
	, VOID*				pThis
	)
{
	if( !atom )
		return NULL;

	AgcdCamConstrain_Obj*	
				PTHIS = (AgcdCamConstrain_Obj*)pThis;
	ApBase*		pBase = NULL;
	RpAtomic*	pPickAtom = NULL;
	RpGeometry*	pPickGeo = NULL;

	RwMatrix*	pLTM = NULL;
	RwMatrix	matInvs;
	RpClump	*pClump = RpAtomicGetClump(atom);

	pLTM = RwFrameGetLTM( RpAtomicGetFrame(atom) );
	if( !pLTM )
		if( pClump )
			pLTM = RwFrameGetLTM( RpClumpGetFrame(pClump) );

	if( !pLTM )
	{
		//err
		return atom;
	}
	RwMatrixInvert( &matInvs, pLTM );

	if( !pPickAtom )
		pPickAtom = atom;
	pPickGeo = RpAtomicGetGeometry(pPickAtom);
	if( !pPickGeo || !RpGeometryGetTriangles(pPickGeo) )
		return atom;

	RpIntersection intsct = *intersection;
	RwV3dTransformPoint( &(intsct.t.line.start), &(intsct.t.line.start), &matInvs );
	RwV3dTransformPoint( &(intsct.t.line.end), &(intsct.t.line.end), &matInvs );

	RpCollisionGeometryForAllIntersections( pPickGeo, &intsct, CBCmpDist, pThis );

	return atom;
}
RpCollisionTriangle* AgcdCamConstrain_Obj::CBCmpDist( RpIntersection* intersection
									, RpCollisionTriangle* collTri
									, RwReal dist
									, void* pThis)
{
	AgcdCamConstrain_Obj* PTHIS = (AgcdCamConstrain_Obj*)pThis;
	if( dist < PTHIS->m_fDist )
	{
		PTHIS->m_fDist = dist;
		PTHIS->m_bIntsct = TRUE;
	}

	return collTri;
}

//-----------------------------------------------------------------------------
//
// AgcuCamConstrain
//
//-----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AgcuCamConstrain::AgcuCamConstrain()
{
};
AgcuCamConstrain::~AgcuCamConstrain()
{
};
AgcuCamConstrain& AgcuCamConstrain::bGetInst()
{
	static AgcuCamConstrain inst;
	return inst;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamConstrain::bConstrainDistance(AgcmCamera2* pAgcmCamera2)
{
	
	RwV3d	EyeSubAt;
	RwV3dSub(&EyeSubAt, pAgcmCamera2->bGetPtrEye(), pAgcmCamera2->bGetPtrLookat());
	RwReal	fLen = RwV3dLength( &EyeSubAt );

	RpIntersection intsct;

	RwV3d	frustumOffsetY = {0.f
		, (pAgcmCamera2->m_pRwCam->frustumCorners[2].y-pAgcmCamera2->m_pRwCam->frustumCorners[0].y)*0.5f
		, 0.f};
    
	RwV3dAdd( &intsct.t.line.start, pAgcmCamera2->bGetPtrLookat(), &frustumOffsetY );
	RwV3dAdd( &intsct.t.line.end, pAgcmCamera2->bGetPtrEye(), &frustumOffsetY );

	intsct.type = rpINTERSECTLINE;
	m_agcdObj.bInit(fLen);

	
	//all intersecting tile
	m_agcdTerrain.bPreChkTerrain( &m_agcdObj
								, pAgcmCamera2->m_pAgcmMap
								, *pAgcmCamera2->bGetPtrEye()
								, pAgcmCamera2->m_v3dLookat );

	//object
	if( AgcdEffGlobal::bGetInst().bGetPtrAgcmOcTree() && 
		AgcdEffGlobal::bGetInst().bGetPtrApmOcTree() &&
		AgcdEffGlobal::bGetInst().bGetPtrApmOcTree()->m_bOcTreeEnable 
		)
	{
		m_agcdObj.bIntsct(pAgcmCamera2->m_pApmObject, pAgcmCamera2->m_pAgcmObject, &intsct);
	}
	else if(AgcdEffGlobal::bGetInst().bGetPtrAgcmMap())
	{
		//@{ kday 20050510
		//AgcmMap::LoadingSectorsForAllAtomicsIntersection3 use real atomic not CollisionAtomic
		m_agcdObj.bIntsct(pAgcmCamera2->m_pApmObject, pAgcmCamera2->m_pAgcmObject, &intsct);

		//if( AgcdEffGlobal::bGetInst().bGetPtrAgcmMap()->m_bUseCullMode )
		//{
		//	AgcdEffGlobal::bGetInst().bGetPtrAgcmMap()->LoadingSectorsForAllAtomicsIntersection3(
		//		2, &intsct, AgcdCamConstrain_Obj::CBIntersectObj, &m_agcdObj );
		//}
		//@} kday
	}

	//terrain	
	RwReal	fDist = 0.f;
	RwBool	bTerrainChk = m_agcdTerrain.bChkTerrain( &fDist
								, intsct.t.line.end//eye
								, pAgcmCamera2->m_v3dLookat
								, *pAgcmCamera2->bGetPtrCZ() 
								);
	
	if( bTerrainChk )
	{
		if( m_agcdObj.bIntersect() )
		{
			RwReal	fDistObj = fLen - m_agcdObj.bDistance()*fLen;
			if( fDistObj > fDist )
				fDist = fDistObj;
		}
		pAgcmCamera2->m_fLimitedLen	= fLen - fDist;
	}
	else if( m_agcdObj.bIntersect() )
	{
		pAgcmCamera2->m_fLimitedLen	= m_agcdObj.bDistance()*fLen;
	}
	else
	{
		pAgcmCamera2->m_fLimitedLen = pAgcmCamera2->m_fDesiredLen;
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamConstrain::bConstrainPitch(AgcmCamera2* pAgcmCamera2)
{
	//알맞은 pitch
};