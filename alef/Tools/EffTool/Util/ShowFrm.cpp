// ShowFrm.cpp
// -----------------------------------------------------------------------------
//   _____ _                    ______                                     
//  / ____| |                  |  ____|                                    
// | (___ | |__   ___ __      __ |__   _ __ _ __ ___       ___ _ __  _ __  
//  \___ \| '_ \ / _ \\ \ /\ / /  __| | '__| '_ ` _ \     / __| '_ \| '_ \ 
//  ____) | | | | (_) |\ V  V /| |    | |  | | | | | | _ | (__| |_) | |_) |
// |_____/|_| |_|\___/  \_/\_/ |_|    |_|  |_| |_| |_|(_) \___| .__/| .__/ 
//                                                            | |   | |    
//                                                            |_|   |_|    
//
// show RwFrame
//
// -----------------------------------------------------------------------------
// Originally created on 02/22/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include "stdafx.h"
#include "ShowFrm.h"
#include "AgcdEffGlobal.h"
#include "AcuMathFunc.h"
USING_ACUMATH;

CEffUt_GeoSphere<VTX_PD>	ShowFrm::GEO_SPHERE(50.f, 13, "sphere");
CEffUt_GeoBox<VTX_PD>		ShowFrm::GEO_BOX(100.f, 100.f, 100.f, "box");
// -----------------------------------------------------------------------------
ShowFrm::ShowFrm( const RwV3d& pos
	, eFrmGeoType eGeoType
	, RwUInt32 ulColr
	)
	: m_eGeoType( eGeoType )
	, m_pFrm( NULL )
	, m_ulColr( ulColr )
{
	m_pFrm = RwFrameCreate();
	RwFrameTranslate( m_pFrm, &pos, rwCOMBINEREPLACE );
}
// -----------------------------------------------------------------------------
ShowFrm::~ShowFrm()
{
	Eff2Ut_SAFE_DESTROY_FRAME( m_pFrm );
}
// -----------------------------------------------------------------------------
int ShowFrm::bRender(RwUInt32* pUlColr)
{
	CEffUt_GeoBase< VTX_PD >	*pgeoBase	= NULL;
	switch( m_eGeoType )
	{
	case eShowFrm_box:		pgeoBase	= &GEO_BOX;		break;
	case eShowFrm_sphere:	pgeoBase	= &GEO_SPHERE;	break;
	}

	if( pgeoBase )
	{
		ksutGeo_SetDiffuse( pgeoBase->bGetVB()
			, pgeoBase->bGetVtxNum()
			, pUlColr ? *pUlColr : m_ulColr );
		return pgeoBase->bRender_w( RwFrameGetLTM( m_pFrm ) );
	}

	return -1;
}
// -----------------------------------------------------------------------------
BOOL ShowFrm::bPick( CPickUser& cPick, RwReal* pLen )
{
	CEffUt_GeoBase< VTX_PD >	*pgeoBase	= NULL;
	switch( m_eGeoType )
	{
	case eShowFrm_box:		pgeoBase	= &GEO_BOX;		break;
	case eShowFrm_sphere:	pgeoBase	= &GEO_SPHERE;	break;
	}

	if( pgeoBase )
		return pgeoBase->bPick( cPick, m_pFrm, pLen );

	return FALSE;
}
// -----------------------------------------------------------------------------
RwFrame* ShowFrm::bGetPtrFrm()
{
	return m_pFrm;
};
// -----------------------------------------------------------------------------
RwMatrix* ShowFrm::bGetPtrLTM()
{
	return m_pFrm ? RwFrameGetLTM(m_pFrm) : NULL;
};
// -----------------------------------------------------------------------------
RwV3d* ShowFrm::bGetPtrPos()
{
	return ( m_pFrm ? RwMatrixGetPos( bGetPtrLTM() ) : NULL );
};
// -----------------------------------------------------------------------------
void ShowFrm::bMoveTo(const RwV3d& newpos)
{
	RwV3d	trans;
	RwV3dSub( &trans, &newpos, bGetPtrPos() );
	RwFrameTranslate( m_pFrm, &trans, rwCOMBINEPOSTCONCAT );
};
// -----------------------------------------------------------------------------
void ShowFrm::bSetDir(const RwV3d& dst)
{
	if(!m_pFrm)
		return;
	
	RwFrameOrthoNormalize ( m_pFrm );

	RwV3d	fromto;
	RwV3dSub( &fromto, &dst, bGetPtrPos() );
	if( Eff2Ut_RwV3dSqLength(fromto) < 0.000001f )
		return;
	fromto.y = 0.f;
	RwV3dNormalize(&fromto, &fromto);
	RwV3d	vDir = *RwMatrixGetAt(RwFrameGetLTM(m_pFrm));
	vDir.y = 0.f;
	RwV3dNormalize(&vDir, &vDir);
	RwV3d	vAxis;

	RwReal	fAngle = 0.f;//Eff2Ut_GetAngleFrom2Vector(&vAxis, vDir, fromto);

	if(NSAcuMath::AngleFrom2Vec( &vAxis, &fAngle, vDir, fromto) )
	{
		fAngle = DEF_R2D(fAngle);
		
		RwV3d	pos = *RwMatrixGetPos(bGetPtrLTM());
		RwV3d	npos;
		RwV3dNegate(&npos, &pos);
		RwFrameTranslate(m_pFrm, &npos, rwCOMBINEPOSTCONCAT);
		RwFrameRotate( m_pFrm, &vAxis, fAngle, rwCOMBINEPOSTCONCAT );
		RwFrameTranslate(m_pFrm, &pos, rwCOMBINEPOSTCONCAT);
	}
	if( fAngle == 0.f )
	{
		return;
	}
	else if( fAngle == DEF_PI )
	{
		RwV3d	pos = *RwMatrixGetPos(bGetPtrLTM());
		RwV3d	npos;
		RwV3dNegate(&npos, &pos);
		RwFrameTranslate(m_pFrm, &npos, rwCOMBINEPOSTCONCAT);
		RwFrameRotate( m_pFrm, RwMatrixGetUp(bGetPtrLTM()), fAngle, rwCOMBINEPOSTCONCAT );
		RwFrameTranslate(m_pFrm, &pos, rwCOMBINEPOSTCONCAT);
	}
};
// -----------------------------------------------------------------------------
void ShowFrm::bSetScale(RwReal scale)
{
	if(m_pFrm)
	{	
		RwMatrix	matNoScale;
		Eff2Ut_RwMatrixGetNoScale( *RwFrameGetLTM( m_pFrm ), &matNoScale );
		RwFrameTransform ( m_pFrm, &matNoScale, rwCOMBINEREPLACE );

		RwV3d vscale = { scale, scale, scale };
		RwFrameScale( m_pFrm, &vscale, rwCOMBINEPRECONCAT );
	}
};
// -----------------------------------------------------------------------------
void ShowFrm::bRot(float dx, float dy)
{
	RwV3d	wy		= {0.f, 1.f, 0.f};
	RwV3d	vpos	= RwFrameGetLTM(m_pFrm)->pos;
	RwFrameRotate ( m_pFrm, &wy, dx, rwCOMBINEPOSTCONCAT );
	RwFrameRotate ( m_pFrm
		, &RwFrameGetLTM(AgcdEffGlobal::bGetInst().bGetPtrCamFrm())->right
		, dy
		, rwCOMBINEPOSTCONCAT );
	RwFrameGetMatrix( m_pFrm )->pos = vpos;
};
// -----------------------------------------------------------------------------
void ShowFrm::bSetIdentity(void)
{
	RwFrameSetIdentity( m_pFrm );
};
// -----------------------------------------------------------------------------
void ShowFrm::bAddChildFrame(RwFrame* pFrm)
{
	if(m_pFrm && pFrm)
	{
		RwFrameAddChild(m_pFrm, pFrm);
	}
};

// -----------------------------------------------------------------------------
// ShowFrm.cpp - End of file
// -----------------------------------------------------------------------------
