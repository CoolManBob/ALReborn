#include "stdafx.h"
#include "Util.h"

BOOL CUtil::Initialize()
{
	return TRUE;
}

void CUtil::ReallocCopyString( char** szDst, const char* szSrc )
{
	if( !szSrc )		return;
	if( !szSrc[0] )		return;

	if( *szDst )
		delete [] *szDst;

	int size = strlen( szSrc );
	*szDst = new char [size + 1];
	strcpy( *szDst, szSrc );
	(*szDst)[size] = NULL;
}

const RwV3d CRwV3d::s_v3dZero	= {	0.f, 0.f, 0.f };
const RwV3d CRwV3d::s_v3dAt		= {	0.f, 0.f, 1.f };
const RwV3d CRwV3d::s_v3dUp		= {	0.f, 1.f, 0.f };
const RwV3d CRwV3d::s_v3dRight	= {	1.f, 0.f, 0.f };

BOOL	CRwUtil::Initialize( RpWorld* pWorld, RwCamera* pCamera )
{
	m_pWorld	= pWorld;
	m_pCamera	= pCamera;

	return TRUE;
}

BOOL	CRwUtil::MouseTransAtomic( RpAtomic* pAtomic, const RwV2d& vPos )
{
	if( !pAtomic )	return FALSE;

	CRpAtomic atomic( pAtomic );
	CRwMatrix matLTM( atomic.GetLTM() );

	RwMatrix matInvLTM;
	matLTM.GetInverse( &matInvLTM );
	RwV3dAssign( &matInvLTM.pos, &CRwV3d::s_v3dZero );

	CRwCamera camera( m_pCamera );
	CRwV3d	vRight( camera.GetRight() );
	vRight.TransPoint( &matInvLTM );
	vRight.Scale( vPos.x * -1.f );

	CRwV3d	vUp( camera.GetUp() );
	vUp.TransPoint( &matInvLTM );
	vUp.Scale( vPos.y * -1.f );

	vRight.Add( &vUp.GetV3d() );

	atomic.Pos( &vRight.GetV3d(), rwCOMBINEPOSTCONCAT );

	return TRUE;
}

BOOL	CRwUtil::MouseRotAtomic( RpAtomic* pAtomic, int nAxisType, float fAngle )
{
	if( !pAtomic )			return FALSE;
	if( fAngle == 0.0f )	return TRUE;
		
	CRpAtomic atomic( pAtomic );

	RwV3d* axis;
	switch( nAxisType )
	{
	case 0:		axis = atomic.GetRight();	break;
	case 1:		axis = atomic.GetUp();		break;
	case 2:		axis = atomic.GetAt();		break;
	default:	return FALSE;
	}

	CRwV3d otherSide( atomic.GetPos() );

	otherSide.Scale( -1.0f );
	atomic.Pos( &otherSide.GetV3d(), rwCOMBINEPOSTCONCAT );

	atomic.Rot( axis, fAngle, rwCOMBINEPOSTCONCAT );

	otherSide.Scale( -1.0f );
	atomic.Pos( &otherSide.GetV3d(), rwCOMBINEPOSTCONCAT );

	return TRUE;
}

BOOL	CRwUtil::MouseScaleAtomic( RpAtomic* pAtomic, float fScale  )
{
	if( !pAtomic )			return FALSE;
	if( fScale == 0.0f )	return TRUE;

	// 1/10 정도로 변화폭을 줄인다.. 컨트롤하기 힘드니..
	fScale = fScale * 0.1f;

	CRpAtomic atomic( pAtomic );

	//if( atomic.GetRight()->x > -0.05f && atomic.GetRight()->x < 0.05f )
	//	return TRUE;

	CRwV3d otherSide( atomic.GetPos() );

	otherSide.Scale( -1.0f );
	atomic.Pos( &otherSide.GetV3d(), rwCOMBINEPOSTCONCAT );

	atomic.Scale( fScale, rwCOMBINEPOSTCONCAT );

	otherSide.Scale( -1.0f );
	atomic.Pos( &otherSide.GetV3d(), rwCOMBINEPOSTCONCAT );

	return TRUE;
}

BOOL	CRwUtil::TransAtomic( RpAtomic* pAtomic, const RwV3d& vOffPos )
{
	if( !pAtomic )			return FALSE;
	if( vOffPos.x == 0.f && vOffPos.y == 0.f && vOffPos.z == 0.f )	return FALSE;

	CRpAtomic atomic( pAtomic );

	//RwV3d vPos = *atomic.GetPos();
	//vPos.x += vOffPos.x;
	//vPos.y += vOffPos.y;
	//vPos.z += vOffPos.z;
	atomic.Pos( (RwV3d*)&vOffPos, rwCOMBINEPOSTCONCAT );

	return TRUE;
}

BOOL	CRwUtil::RotAtomic( RpAtomic* pAtomic, const RwV3d& vRot )
{
	if( !pAtomic )			return FALSE;
	if( vRot.x == 0.f && vRot.y == 0.f && vRot.z == 0.f )	return FALSE;

	CRpAtomic atomic( pAtomic );

	CRwV3d otherSide( atomic.GetPos() );

	otherSide.Scale( -1.0f );
	atomic.Pos( &otherSide.GetV3d(), rwCOMBINEPOSTCONCAT );

	if( vRot.x != 0.f )		atomic.Rot( atomic.GetRight(), vRot.x, rwCOMBINEPOSTCONCAT );
	if( vRot.y != 0.f )		atomic.Rot( atomic.GetUp(), vRot.y, rwCOMBINEPOSTCONCAT );
	if( vRot.z != 0.f )		atomic.Rot( atomic.GetAt(), vRot.z, rwCOMBINEPOSTCONCAT );

	otherSide.Scale( -1.0f );
	atomic.Pos( &otherSide.GetV3d(), rwCOMBINEPOSTCONCAT );

	return TRUE;
}

BOOL	CRwUtil::ScaleAtomic( RpAtomic* pAtomic, const RwV3d& vScale )
{
	if( !pAtomic )			return FALSE;
	if( vScale.x == 0.f && vScale.y == 0.f && vScale.z == 0.f )	return FALSE;

	CRpAtomic atomic( pAtomic );

	CRwV3d otherSide( atomic.GetPos() );

	otherSide.Scale( -1.0f );
	atomic.Pos( &otherSide.GetV3d(), rwCOMBINEPOSTCONCAT );

	RwV3d vTemp = vScale;
	//if( vTemp.x != 0.f )	vTemp.x *= 0.1f;
	//vTemp.x += 1.f;

	//if( vTemp.y != 0.f )	vTemp.y *= 0.1f;
	//vTemp.y += 1.f;

	//if( vTemp.z != 0.f )	vTemp.z *= 0.1f;
	//vTemp.z += 1.f;

	atomic.Scale( (RwV3d*)&vTemp, rwCOMBINEPOSTCONCAT );

	otherSide.Scale( -1.0f );
	atomic.Pos( &otherSide.GetV3d(), rwCOMBINEPOSTCONCAT );

	return TRUE;
}

RpClump*	CRwUtil::LoadClump( char* szName )
{
	RwChar* pRwPath = RsPathnameCreate( szName );
	if( !pRwPath )		goto FAILD;

	RwStream* pRwStream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMREAD, pRwPath );
	if( !pRwStream )	goto FAILD;
	
	if( !RwStreamFindChunk( pRwStream, rwID_CLUMP, NULL, NULL ) )
		goto FAILD;

	RpClump* pRpClump = RpClumpStreamRead( pRwStream );

	RsPathnameDestroy( pRwPath );
	RwStreamClose( pRwStream, NULL );

	return pRpClump;

FAILD:
	if( pRwPath )
		RsPathnameDestroy( pRwPath );
	if( pRwStream )
		RwStreamClose( pRwStream, NULL );

	return NULL;

}