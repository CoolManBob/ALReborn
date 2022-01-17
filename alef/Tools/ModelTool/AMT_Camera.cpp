#include "stdafx.h"
#include "ModelTool.h"

BOOL CModelToolApp::ResetCamera()
{
	RwCamera* pCamera = GetRenderWare()->GetCamera();
	if( !pCamera )			return FALSE;
	
	RwUtilClumpArray* pClumpArray = GetRenderClump();
	if( !pClumpArray )		return FALSE;

	FLOAT fOffset = 0.0f;
	for( INT32 lCount = 0; lCount < pClumpArray->num; ++lCount )
	{
		if( RpClumpGetNumAtomics( pClumpArray->array[lCount] ) )
		{
			RwSphere sSphere;
			RwUtilClumpGetBoundingSphere( pClumpArray->array[lCount], &sSphere );
			fOffset = max( fOffset, sSphere.radius );
		}
	}

	RwV3d vPos = { 0.0f, 0.0f, 50.0f };
	if( fOffset != 0.0f )
	{
		vPos.z += ( fOffset * AMT_RESET_CAMERA_OFFSET );
		vPos.z = min( vPos.z, 49000.0f );
	}
	else
		vPos.z += 300.0f;

	CRwCamera	camera( pCamera );
	camera.Pos( &vPos, rwCOMBINEREPLACE );
	camera.Rot( &RWUTIL_YAXIS, 180.0f, rwCOMBINEPRECONCAT );

	m_fUpRotAngle = 180.0f;

	return TRUE;
}

BOOL CModelToolApp::TranslateCamera(RwV2d *pv2dDelta)
{
	RwCamera* pCamera = GetRenderWare()->GetCamera();
	if( !pCamera )			return FALSE;
	
	CRwCamera	camera( pCamera );

	RwV3d v3dScaleX, v3dScaleY, v3dPos;
	RwV3dScale( &v3dScaleX, camera.GetRight(), (RwReal)(pv2dDelta->x * m_csCameraOffset.m_fMove) );
	RwV3dScale( &v3dScaleY, camera.GetUp(), (RwReal)(pv2dDelta->y * m_csCameraOffset.m_fMove) );
	RwV3dAdd( &v3dPos, &v3dScaleX, &v3dScaleY );

	camera.Pos( &v3dPos, rwCOMBINEPOSTCONCAT );

	return TRUE;
}

BOOL CModelToolApp::RotateCamera(RwV2d *pv2dDelta)
{
	RwCamera* pCamera = GetRenderWare()->GetCamera();
	if( !pCamera )			return FALSE;
	
	CRwCamera	camera( pCamera );

	RwReal	UpRotVal = -(pv2dDelta->x * m_csCameraOffset.m_fRotate);
	camera.Rot( camera.GetRight(), (RwReal)(pv2dDelta->y * m_csCameraOffset.m_fRotate), rwCOMBINEPOSTCONCAT );
	camera.Rot( camera.GetUp(), UpRotVal , rwCOMBINEPOSTCONCAT );

	m_fUpRotAngle += UpRotVal;

	return TRUE;
}

BOOL CModelToolApp::ZoomCamera(FLOAT fDelta)
{
	RwCamera* pCamera = GetRenderWare()->GetCamera();
	if( !pCamera )			return FALSE;
	
	CRwCamera	camera( pCamera );

	RwV3d v3dScale;
	RwV3dScale( &v3dScale, camera.GetAt(), (RwReal)(fDelta * m_csCameraOffset.m_fZoom) );
	camera.Pos( &v3dScale, rwCOMBINEPOSTCONCAT );

	return TRUE;
}
