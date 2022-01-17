// AgcmCamera2.cpp: implementation of the AgcmCamera2 class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcuEffUtil.h"
#include "AgcdEffGlobal.h"
#include "AgcuEffTable.h"

#include "AgcmCamera2.h"

#include "AgcuCamConstrain.h"
#include "AgcuCamDampingForce.h"
#include "AgcuCamSpringForce.h"
#include "AgcuCamMode.h"
#include "AgcuCamPathWork.h"

#include "AgcmShadowmap.h"

#include "AcuMathFunc.h"
#include "AgcmUIConsole.h"

USING_ACUMATH;

#ifdef _DEBUG
#define TOCON(msg)	Eff2Ut_ToConsol(msg)
#else
#define TOCON(msg)
#endif //_DEBUG


//각종 유틸 계수들
RwReal	g_yawcoef	= 5.f;
RwReal	g_pitchcoef	= 4.f;
RwReal	g_zoomcoef	= 6.f;

RwReal	g_yawcoef_nodamp	= 0.3f;
RwReal	g_pitchcoef_nodamp	= 0.4f;
RwReal	g_zoomcoef_nodamp	= 0.6f;


RwReal	g_offsetMin	= 100.f;
RwReal	g_offsetMax	= 160.f;

RwV2d	g_viewWindow = {1.f, 1.f};

AgcmCamera2*	g_pAgcmCamera2 = NULL;

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
AgcmCamera2::stTargetInfo::stTargetInfo()
	: m_eState(e_target_static)
	, m_pFrm(NULL)
{
	RwV3d	vzero = { 0.f, 0.f, 0.f };
	m_vOffset	= vzero;
	m_vLookat	= vzero;
	m_vEye		= vzero;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
const RwV3d* AgcmCamera2::stTargetInfo::GetPtrRight(void)
{
	ASSERT( m_pFrm );
	return RwMatrixGetRight( RwFrameGetLTM(m_pFrm) );
};
const RwV3d* AgcmCamera2::stTargetInfo::GetPtrUp(void)
{
	ASSERT( m_pFrm );
	return RwMatrixGetUp( RwFrameGetLTM(m_pFrm) );
};
const RwV3d* AgcmCamera2::stTargetInfo::GetPtrAt(void)
{
	ASSERT( m_pFrm );
	return RwMatrixGetAt( RwFrameGetLTM(m_pFrm) );
};
const RwV3d* AgcmCamera2::stTargetInfo::GetPtrPos(void)
{
	ASSERT( m_pFrm );
	return RwMatrixGetPos( RwFrameGetLTM(m_pFrm) );
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::stTargetInfo::CalcLookat(const RwV3d* pVEyeSubAt)
{
	if( !m_pFrm )
		return;

	if( m_eState == e_target_static )
		return;

	//m_pFrm 에 스케일 팩터가 있어도 그대로 적용하자.
	m_vLookat = *GetPtrPos();
	if( fabs(m_vOffset.x) > 0.0001f )
	{
		RwV3d	vx;
		RwV3dScale( &vx, GetPtrRight(), m_vOffset.x );
		RwV3dAdd( &m_vLookat, &m_vLookat, &vx );
	}
	if( fabs(m_vOffset.y) > 0.0001f )
	{
		RwV3d	vy;
		RwV3dScale( &vy, GetPtrUp(), m_vOffset.y );
		RwV3dAdd( &m_vLookat, &m_vLookat, &vy );
	}
	if( fabs(m_vOffset.z) > 0.0001f )
	{
		RwV3d	vz;
		RwV3dScale( &vz, GetPtrAt(), m_vOffset.z );
		RwV3dAdd( &m_vLookat, &m_vLookat, &vz );
	}

	if( pVEyeSubAt )
	{
		RwV3dAdd( &m_vEye, &m_vLookat, pVEyeSubAt );
	}
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::stTargetInfo::OnMove(const RwV3d* pvEyeSubAt)
{
	m_eState = e_target_moving;
	CalcLookat(pvEyeSubAt);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::stTargetInfo::OnStop(void)
{
	m_eState = e_target_static;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::stTargetInfo::OnSet(RwFrame* pFrmTarget, const RwV3d* pvOffset, const RwV3d* pvEyeSubAt)
{
	m_pFrm = pFrmTarget;
	if( pvOffset )
		m_vOffset = *pvOffset;

	m_eState = e_target_moving;
	CalcLookat( pvEyeSubAt );
	m_eState = e_target_static;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::stTargetInfo::SetYOffset(RwReal fup)
{
	m_vOffset.y = fup;
};

//-----------------------------------------------------------------------------
//
// AgcmCamera2
//
//-----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgcmCamera2::AgcmCamera2() :
	m_fCharacterHeight( 100.0f )
{
	g_pAgcmCamera2 = this;

	const RwV3d	VZERO	= { 0.f, 0.f, 0.f };

	SetModuleName("AgcmCamera2");
	EnableIdle(TRUE);

	m_ulOptFlag			= e_flag_use_dampingforce		|
						  e_flag_use_LimitPitch			|
						  e_flag_use_LimitZoom			|
						  e_flag_use_constrain_zoom		|
						  e_flag_use_reStoreSpring_zoom	;
	
	m_pAgcmRender		= NULL;
	m_pAgcmMap			= NULL;
	m_pApmObject		= NULL;
	m_pAgcmObject		= NULL;
	m_pAgcmCharacter	= NULL;
	m_pAgpmCharacter	= NULL;
	m_pClumpCharacter	= NULL;
	
	m_pRwCam			= NULL;
	m_v3dLookat			= VZERO;

	//m_v3dEyeSubAt		= VZERO;
	m_v3dEyeSubAt.x		= 281.f;
	m_v3dEyeSubAt.y		= 281.f;
	m_v3dEyeSubAt.z		= 450.f;

	m_ulLockMask		= 0LU;
	m_ulUpdateMask		= 0LU;
	m_fWYaw				= 0.f;
	m_fCPitch			= 0.f;
	m_fZoom				= 0.f;
	m_vMove				= VZERO;

	m_fDesiredLen		= RwV3dLength(&m_v3dEyeSubAt);
	m_fCurrLen			= m_fDesiredLen;
	m_fLimitedLen		= 0.f;
	m_fDesiredPitch		= 0.f;
	m_fLimitedPitch		= 0.f;

	m_fMinLen			= 96.0f;
	m_fMaxLen			= 1300.0f;
	m_fMinPitch			= -45.f;
	m_fMaxPitch			= 45.f;
	
	m_fTurnAccumTime	= 0.f;
	m_fTurnAccumAngle	= 0.f;
	m_fTurnTime			= 0.4f;
	m_fTurnAngle		= DEF_PI;
	m_fTurnSpeed		= m_fTurnAngle/m_fTurnTime;
	m_fTurnCoef			= 180.f / ( sinf(m_fTurnAngle) + m_fTurnAngle );

	m_eIdle				= e_idle_default;
	m_fptrOnIdle		= &AgcmCamera2::vOnIdle_default;
}

AgcmCamera2::~AgcmCamera2()
{

}


//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
const RwV3d* AgcmCamera2::bGetPtrCX(void)	
{ 
	if( !m_pRwCam )
		return NULL;

	AgcModule::LockFrame();
	const RwV3d*	pr 
		= RwMatrixGetRight( RwFrameGetLTM(RwCameraGetFrame(m_pRwCam)) );
	AgcModule::UnlockFrame();

	return pr;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
const RwV3d* AgcmCamera2::bGetPtrCY(void)
{ 
	if( !m_pRwCam )
		return NULL;

	AgcModule::LockFrame();
	const RwV3d*	pr 
		= RwMatrixGetUp( RwFrameGetLTM(RwCameraGetFrame(m_pRwCam)) );
	AgcModule::UnlockFrame();

	return pr;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
const RwV3d* AgcmCamera2::bGetPtrCZ(void)
{ 
	if( !m_pRwCam )
		return NULL;

	AgcModule::LockFrame();
	const RwV3d*	pr 
		= RwMatrixGetAt( RwFrameGetLTM(RwCameraGetFrame(m_pRwCam)) );
	AgcModule::UnlockFrame();

	return pr;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
const RwV3d* AgcmCamera2::bGetPtrEye(void)
{ 
	if( !m_pRwCam )
		return NULL;

	AgcModule::LockFrame();
	const RwV3d*	pr 
		= RwMatrixGetPos( RwFrameGetLTM(RwCameraGetFrame(m_pRwCam)) );
	AgcModule::UnlockFrame();

	return pr;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
const RwV3d* AgcmCamera2::bGetPtrLookat(void)
{
	return &m_v3dLookat;
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bRotWY(RwReal deg)
{
	if( fabsf(deg) < 0.0001f		|| 
		vUdtMaskChk(e_mask_turn180) ||
		vLockFlagChk(e_mask_yaw)	)
		return;

	vUdtMaskOn(e_mask_yaw);
	if( vOptFlagChk(e_flag_use_dampingforce) )
	{
		AgcuCamDampingForce::bGetInst().bAddOmegaY(deg*g_yawcoef);
	}
	else
	{
		m_fWYaw += deg*g_yawcoef_nodamp;
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bRotCX(RwReal deg)
{		
	if( fabsf(deg) < 0.0001f		||
		vLockFlagChk(e_mask_pitch)	)
		return;

	vUdtMaskOn(e_mask_pitch);
	if( vOptFlagChk(e_flag_use_dampingforce) )
	{
		AgcuCamDampingForce::bGetInst().bAddOmegaX(deg*g_pitchcoef);
	}
	else
	{
		deg *= g_pitchcoef_nodamp;
		vAvailablePitch(deg);
		m_fCPitch += deg;
	}
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bZoom(RwReal ds)
{
	if( fabsf(ds) < 0.0001f			||
		vLockFlagChk(e_mask_zoom)	)
		return;

	if( vOptFlagChk(e_flag_use_dampingforce) )
	{
		AgcuCamDampingForce::bGetInst().bAddZoomSpeed(ds*g_zoomcoef);// * 10.1f);
	}
	else
	{
		vUdtMaskOn(e_mask_zoom);
		m_fZoom += ds*g_zoomcoef_nodamp;
		vAvailableZoom(m_fZoom);
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bMove(const RwV3d& vdelta)
{
	vUdtMaskOn(e_mask_move);
	RwV3dAdd(&m_vMove, &m_vMove, &vdelta);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bTurnWY_180 (void)
{
	if( vUdtMaskChk(e_mask_turn180)	||
		vLockFlagChk(e_mask_turn180))
		return;

	m_fTurnAccumTime	= 0.f;
	m_fTurnAccumAngle	= 0.f;
	vUdtMaskOn(e_mask_turn180);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bSetTargetFrame(RwFrame* pFrmTarget, const RwV3d& vOffset)
{
	m_stTarget.OnSet( pFrmTarget, &vOffset, &m_v3dEyeSubAt );

	bSetCamFrm(m_stTarget.m_vLookat, m_stTarget.m_vEye);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bSetIdleType(e_idle_type eIdle)
{
	if(m_eIdle == eIdle)
		return;
/*
	void		vOnIdle_default(RwReal fElasped);
	void		vOnIdle_Login(RwReal fElasped);*/
	switch(eIdle)
	{
	case e_idle_default:
		m_fptrOnIdle = &AgcmCamera2::vOnIdle_default;
		if( m_pAgcmRender )
			m_pAgcmRender->SetOcLineCheck(TRUE);
		break;
	case e_idle_login:
		m_fptrOnIdle = &AgcmCamera2::vOnIdle_Login;
		if( m_pAgcmRender )
			m_pAgcmRender->SetOcLineCheck(FALSE);
		break;
	case e_idle_chagemode:
		m_fptrOnIdle = &AgcmCamera2::vOnIdle_modechage;
		if( m_pAgcmRender )
			m_pAgcmRender->SetOcLineCheck(FALSE);
		break;
	default:
		ASSERT( !"unknown idletype!" );
		return;
	}
	m_eIdle	= eIdle;
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
BOOL AgcmCamera2::CBOnTargetMove( PVOID pData, PVOID pClass, PVOID pCustData )
{
	pData;
	pCustData;

	AgcmCamera2*	pThis = static_cast<AgcmCamera2*>(pClass);

	AgpdCharacter*	pAgpdCharacter	= (AgpdCharacter *) pData;

	if (pThis->m_pAgcmCharacter->GetSelfCharacter() != pAgpdCharacter ||
		pThis->m_pAgcmCharacter->IsCharacterTransforming(pAgpdCharacter))
		return TRUE;

	pThis->vUdtMaskOn(e_mask_moveTarget);
	//if( pThis )
	//	pThis->bOnMoveTarget();
	//else
	//	return FALSE;

	return TRUE;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
BOOL AgcmCamera2::CBOnTargetStop( PVOID pData, PVOID pClass, PVOID pCustData )
{
	pData;
	pCustData;
	
	AgcmCamera2*	pThis = static_cast<AgcmCamera2*>(pClass);

	//@{ 2006/12/01 burumal
	pThis->m_pClumpCharacter	= NULL;
	//@}

	if( pThis )
		pThis->m_stTarget.OnStop();
	else
		return FALSE;

	return TRUE;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
BOOL AgcmCamera2::CBOnTargetSet ( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if ((!pClass) || (!pData))
		return FALSE;

	AgcmCamera2		*pcsThis			= (AgcmCamera2 *)(pClass);
	AgpdCharacter	*pstAgpdCharacter	= (AgpdCharacter *)(pData);
	AgcdCharacter	*pstAgcdCharacter	= pcsThis->m_pAgcmCharacter->GetCharacterData(pstAgpdCharacter);
	
	ASSERT( pcsThis );
	if ( (!pstAgcdCharacter)			|| 
		 (!pstAgcdCharacter->m_pClump)	||
		 (!pcsThis)						)
	{
		return FALSE;
	}

	// TID 준비~
	if( pcsThis->m_pAgpmCharacter->IsRideOn( pstAgpdCharacter ) )	// 탈것에 타고 있는지 확인.
	{
		AgpdCharacter	*pstAgpdCharacterRide = pcsThis->m_pAgpmCharacter->GetCharacter( pstAgpdCharacter->m_nRideCID );

		if( pstAgpdCharacterRide )
		{
			pstAgpdCharacter = pstAgpdCharacterRide;
			pstAgcdCharacter = pcsThis->m_pAgcmCharacter->GetCharacterData(pstAgpdCharacter);
		}
	}

	AgcuCamMode::bGetInst().SetCameraInfo( pcsThis , pstAgpdCharacter );

	pcsThis->m_pClumpCharacter	= pstAgcdCharacter->m_pClump;

	RwV3d	vOffset = {0.f, pcsThis->m_fCharacterHeight , 0.f};

	vOffset.y = pcsThis->m_fCharacterHeight;
	pcsThis->bSetTargetFrame( RpClumpGetFrame(pstAgcdCharacter->m_pClump), vOffset );
	pcsThis->bSetIdleType( e_idle_default );

	return TRUE;
};


BOOL AgcmCamera2::CBIsCameraMoving(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmCamera2	*pThis				= (AgcmCamera2 *)	pClass;
	BOOL		*pbIsCameraMoving	= (BOOL *)			pData;

	*pbIsCameraMoving	= pThis->bIsMoving();

	return TRUE;
}


BOOL AgcmCamera2::CBTurnCamera(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmCamera2	*pThis			= (AgcmCamera2 *)	pClass;
	BOOL		*pbTurnLeft		= (BOOL *)			pData;
	BOOL		*pbTurnRight	= (BOOL *)			pCustData;

	if (*pbTurnLeft)
	{
		pThis->bRotWY(7.3f);
	}
	else if (*pbTurnRight)
	{
		pThis->bRotWY(-7.3f);
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bOnMoveTarget(void)
{
	if( !m_stTarget.m_pFrm )
		return;

	m_stTarget.SetYOffset( m_fCharacterHeight );

	m_stTarget.OnMove( &m_v3dEyeSubAt );

	if( vOptFlagChk(e_flag_use_springforce) )
	{
		AgcuCamSpringForce::bGetInst().bOnMoveCharac( this );
	}
	else
	{
		RwV3d	vtrans;
		RwV3dSub( &vtrans, &m_stTarget.m_vEye, bGetPtrEye() );
		bMove( vtrans );
	}

	vUdtMaskOff(e_mask_moveTarget);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bSetCamFrm(const RwV3d& eye, RwReal fYaw, RwReal fPitch)
{
	RwV3d	wx = {1.f, 0.f, 0.f};
	RwV3d	wy = {0.f, 1.f, 0.f};
	AgcModule::LockFrame();
	RwFrameTranslate( RwCameraGetFrame(m_pRwCam), &eye, rwCOMBINEREPLACE );
	RwFrameRotate( RwCameraGetFrame(m_pRwCam), &wy, fYaw, rwCOMBINEPRECONCAT );
	RwFrameRotate( RwCameraGetFrame(m_pRwCam), &wx, fPitch, rwCOMBINEPRECONCAT );
	AgcModule::UnlockFrame();

	RwV3dSub(&m_v3dLookat, &eye, &m_v3dEyeSubAt);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bSetCamFrm(const RwV3d& lookat, const RwV3d& eye)
{
	m_v3dLookat = lookat;
	RwV3dSub( &m_v3dEyeSubAt, &eye, &lookat );
	m_fCurrLen		=
	m_fDesiredLen	= RwV3dLength(&m_v3dEyeSubAt);

	RwFrame*	pFrm	= RwCameraGetFrame(m_pRwCam);
	RwFrameSetIdentity(pFrm);
	RwMatrix*	pMat	= &pFrm->modelling;

	RwFrameTranslate( pFrm, &eye, rwCOMBINEREPLACE );

	RwV3d	wy = {0.f, 1.f, 0.f};
	RwV3dNegate( &pMat->at, &m_v3dEyeSubAt );
	RwV3dNormalize( &pMat->at, &pMat->at );
	RwV3dCrossProduct( &pMat->right, &wy, &pMat->at );
	RwV3dNormalize( &pMat->right, &pMat->right );
	RwV3dCrossProduct( &pMat->up, &pMat->at, &pMat->right );
	RwV3dNormalize( &pMat->up, &pMat->up );

	pMat->flags = 3;
	RwFrameUpdateObjects(pFrm);

	
	RwV2d	xz = { m_v3dEyeSubAt.z, m_v3dEyeSubAt.x };
	RwV2d	xz_y = { sqrtf( m_v3dEyeSubAt.x*m_v3dEyeSubAt.x + m_v3dEyeSubAt.z*m_v3dEyeSubAt.z ), m_v3dEyeSubAt.y };

	RwReal	fRotAngle = atan2f( xz.y, xz.x );
	RwReal	fPanAngle = atan2f( xz_y.y, xz_y.x );

	m_pAgcmRender->m_fRotAngle	= (DEF_R2D(fRotAngle)+180.f);
	m_fCurrPitch				= 
	m_fDesiredPitch				= 
	m_pAgcmRender->m_fPanAngle	= DEF_R2D(fPanAngle);


	m_ulUpdateMask &= (e_mask_turn180 | e_mask_moveTarget | e_mask_alphaTarget | e_mask_transparentTarget);
	//m_ulUpdateMask	= 0LU;
	m_fWYaw			= 0.f;
	m_fCPitch		= 0.f;
	m_fZoom			= 0.f;
	m_vMove.x		= 0.f;
	m_vMove.y		= 0.f;
	m_vMove.z		= 0.f;

	AgcuCamDampingForce::bGetInst().m_ulUpdateMask	= 0LU;
	AgcuCamDampingForce::bGetInst().m_v2Omega.x		= 0.f;
	AgcuCamDampingForce::bGetInst().m_v2Omega.y		= 0.f;
	AgcuCamDampingForce::bGetInst().m_zoomspeed		= 0.f;

	AgcuCamSpringForce::bGetInst().m_ulUpdateMask	= 0LU;
	AgcuCamSpringForce::bGetInst().m_v3dPrevVel.x		= 0.f;
	AgcuCamSpringForce::bGetInst().m_v3dPrevVel.y		= 0.f;
	AgcuCamSpringForce::bGetInst().m_v3dPrevVel.z		= 0.f;
	AgcuCamSpringForce::bGetInst().m_fPrevLinearSpeed	= 0.f;
	AgcuCamSpringForce::bGetInst().m_fPrevAngularSpeed	= 0.f;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bSetCamFrm(RwMatrix& mat)
{
	RwMatrixUpdate (&mat);
	RwFrameTransform ( RwCameraGetFrame(m_pRwCam)
		, &mat
		, rwCOMBINEREPLACE );
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bSetViewWindow(const RwV2d& window)
{
	RwCameraSetViewWindow(m_pRwCam, &window);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bGetViewWindow(RwV2d& window)
{
	window = *RwCameraGetViewWindow(m_pRwCam);
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vOnIdle_default(RwReal fElapsed)
{
	PROFILE("AgcmCamera2::vOnIdle_default");

	ASSERT( m_pRwCam );
	if( !m_pRwCam )
		return;


	if( vUdtMaskChk(e_mask_moveTarget) )
		bOnMoveTarget();
	else
		m_stTarget.CalcLookat(&m_v3dEyeSubAt);


	if( vOptFlagChk( e_flag_use_dampingforce ) )
	{
		AgcuCamDampingForce::bGetInst().bOnIdle(this, fElapsed);
	}

	if( vOptFlagChk( e_flag_use_springforce ) )
	{
		AgcuCamSpringForce::bGetInst().bOnIdle(this, fElapsed);
	}
	
	RwV3d	m_vx	= *bGetPtrCX();
	RwV3d	m_vp	= *bGetPtrEye();

	if( vUdtMaskChk( e_mask_zoom ) )
	{
		RwV3d	vzoom;
		RwV3dScale( &vzoom, bGetPtrCZ(), m_fZoom );
		RwV3dAdd( &m_vp, &m_vp, &vzoom );
		m_fZoom = 0.f;

		RwV3dSub( &m_v3dEyeSubAt, &m_vp, &m_v3dLookat );
		m_fCurrLen		=
		m_fDesiredLen	= RwV3dLength( &m_v3dEyeSubAt );

		vAdjLookAt(m_fCurrLen);
	}
	if( vUdtMaskChk( e_mask_move ) )
	{
		RwV3dAdd(&m_v3dLookat, &m_v3dLookat, &m_vMove);
		m_vMove.x = m_vMove.y = m_vMove.z = 0.f;
	}

	if( vUdtMaskChk(e_mask_turn180) )
	{
		// angularSpeed = coef*(1 + cos ( angle ))
		// fTurnAccumAngle = Integral( angularSpeed ) : 0 ~ m_fTurnAngle == 180
		//		 = coef*( angle + sin( angle ) )
		// coef  = 180.f / ( m_fTurnAngle + sin( m_fTurnAngle ) )
		// angle = time * m_fTurnSpeed
		// angle : sin 함수의 인자로 들어갈 각
		// fTurnAccumAngle : 결과로 나온 실제 카메라가 회전될각.
		m_fTurnAccumTime += fElapsed;
		RwReal	fTurnAccumAngle	= 
			m_fTurnCoef*(sinf( m_fTurnAccumTime * m_fTurnSpeed ) + m_fTurnAccumTime * m_fTurnSpeed);

		RwReal	angle = 0.f;
		if( fTurnAccumAngle >= 180.f )
		{
			vUdtMaskOff(e_mask_turn180);
			angle = 180.f - m_fTurnAccumAngle;
		}else{
			angle = fTurnAccumAngle - m_fTurnAccumAngle;
		}
		m_fTurnAccumAngle = fTurnAccumAngle;

		m_fWYaw = angle;
		vUdtMaskOn( e_mask_yaw );
	}

	if( vUdtMaskChk( e_mask_pitch ) )
	{		

		RwMatrix mat;
		RwMatrixRotate( &mat, &m_vx, m_fCPitch, rwCOMBINEREPLACE );

		RwV3dTransformVector ( &m_v3dEyeSubAt, &m_v3dEyeSubAt, &mat );

		m_stTarget.CalcLookat( &m_v3dEyeSubAt );
		AgcuCamSpringForce::bGetInst().m_v3DisiredEye = m_stTarget.m_vEye;

		m_fCPitch = 0.f;

		m_fDesiredPitch = atan2f( 
				sqrtf(m_v3dEyeSubAt.x*m_v3dEyeSubAt.x + m_v3dEyeSubAt.z*m_v3dEyeSubAt.z)
				, m_v3dEyeSubAt.y 
				); 
	}
	if( vUdtMaskChk( e_mask_yaw ) )
	{		
		RwV3d	vWY = { 0.f, 1.f, 0.f };
		RwMatrix mat;
		RwMatrixRotate( &mat, &vWY, m_fWYaw, rwCOMBINEREPLACE );
			
		RwV3dTransformVector ( &m_vx, &m_vx, &mat );
		RwV3dTransformVector ( &m_v3dEyeSubAt, &m_v3dEyeSubAt, &mat );

		m_stTarget.CalcLookat( &m_v3dEyeSubAt );
		AgcuCamSpringForce::bGetInst().m_v3DisiredEye = m_stTarget.m_vEye;

		m_fWYaw = 0.f;
	}

	vUpdateMatrix();

	//constrain distance
//	if( !vOptFlagChk( e_flag_use_springforce ) )
	if( vOptFlagChk( e_flag_use_constrain_zoom ) )
	{
		//vForcedZoom(m_fDesiredLen-m_fCurrLen);
		AgcuCamConstrain::bGetInst().bConstrainDistance(this);
		if( m_fCurrLen > m_fLimitedLen )
		{
			vForcedZoom((m_fDesiredLen-m_fCurrLen)+(m_fCurrLen-m_fLimitedLen));
		}
		else if( m_fCurrLen < m_fLimitedLen )
		{
			if( this->vOptFlagChk( e_flag_use_reStoreSpring_zoom ) )
			{
				vForcedZoom(m_fDesiredLen-m_fCurrLen);
				AgcuCamSpringForce::bGetInst().bOnReleaseLinearSpring(this, fElapsed, m_fCurrLen-m_fLimitedLen);
			}
			else
			{
				vForcedZoom((m_fDesiredLen-m_fCurrLen)+(m_fCurrLen-m_fLimitedLen));
			}
		}
		else if( !vUdtMaskChk( e_mask_zoom ) )
		{
			vForcedZoom(m_fDesiredLen-m_fCurrLen);
		}
	}
	//costrain pitch
	if( vOptFlagChk( e_flag_use_constrain_pitch ) )
	{
		AgcuCamConstrain::bGetInst().bConstrainPitch(this);
		if( vUdtMaskChk( e_mask_releaseSphiral ) )
		{
			AgcuCamSpringForce::bGetInst().bOnReleaseSpiralSpring(this, fElapsed, m_fCurrPitch-m_fLimitedPitch);
		}
	}

	//for rendermodule
	RwV2d	xz = { m_v3dEyeSubAt.z, m_v3dEyeSubAt.x };
	RwV2d	xz_y = { sqrtf( m_v3dEyeSubAt.x*m_v3dEyeSubAt.x + m_v3dEyeSubAt.z*m_v3dEyeSubAt.z ), m_v3dEyeSubAt.y };

	RwReal	fRotAngle = atan2f( xz.y, xz.x );
	RwReal	fPanAngle = atan2f( xz_y.y, xz_y.x );

	m_pAgcmRender->m_fRotAngle = (DEF_R2D(fRotAngle)+180.f);
	m_pAgcmRender->m_fPanAngle = DEF_R2D(fPanAngle);
	
	//save current state
	//length
	RwV3d	eyeSubAt;
	RwV3dSub( &eyeSubAt, bGetPtrEye(), bGetPtrLookat() );
	m_fCurrLen = RwV3dLength(&eyeSubAt);
	//pitch
	m_fCurrPitch = fPanAngle;

	vAdjAlpha(m_fCurrLen);

	m_ulUpdateMask &= (e_mask_turn180 | e_mask_moveTarget | e_mask_alphaTarget | e_mask_transparentTarget);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vOnIdle_Login(RwReal fElapsed)
{
	//do nothing.
	fElapsed;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vOnIdle_modechage(RwReal fElasped)
{
	if( vUdtMaskChk(e_mask_moveTarget) )
	{
		if( !m_stTarget.m_pFrm )
			return;
		m_stTarget.SetYOffset( m_fCharacterHeight );
		m_stTarget.OnMove( &m_v3dEyeSubAt );

		this->m_v3dLookat  = m_stTarget.m_vLookat;
	}

	AgcuCamMode::bGetInst().bOnIdle( *this, fElasped );
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vUpdateMatrix()
{
	RwFrame*	pFrm	= RwCameraGetFrame(m_pRwCam);
	RwFrameSetIdentity(pFrm);
	if( vUdtMaskChk( e_mask_move ) ){
		if( !vOptFlagChk(e_flag_use_springforce) ){
			m_v3dLookat = m_stTarget.m_vLookat;
		}
	}

	const RwV3d	wx = {1.f, 0.f, 0.f};
	const RwV3d	wy = {0.f, 1.f, 0.f};
	const RwV3d	wz = {0.f, 0.f, 1.f};

	RwV2d	xz = { m_v3dEyeSubAt.z, m_v3dEyeSubAt.x };
	RwV2d	xz_y = { sqrtf( m_v3dEyeSubAt.x*m_v3dEyeSubAt.x + m_v3dEyeSubAt.z*m_v3dEyeSubAt.z ), m_v3dEyeSubAt.y };

	RwReal	fRotAngle = atan2f( xz.y, xz.x );
	RwReal	fPanAngle = atan2f( xz_y.y, xz_y.x );

	RwFrameRotate( pFrm, &wx, DEF_R2D(fPanAngle), rwCOMBINEPOSTCONCAT );
	RwFrameRotate( pFrm, &wy, DEF_R2D(fRotAngle)+180.f, rwCOMBINEPOSTCONCAT );

	RwV3d	vtrans;
	RwV3dAdd(&vtrans, &m_v3dEyeSubAt, &m_v3dLookat);
	RwFrameTranslate( pFrm, &vtrans, rwCOMBINEPOSTCONCAT );
};


//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vForcedZoom(RwReal offset)
{
	if( fabsf(offset) < 0.0001f )
		return;

	RwV3d zoom;
	RwV3dScale( &zoom, bGetPtrCZ(), offset );
	RwFrameTranslate( 
		RwCameraGetFrame( m_pRwCam )
		, &zoom
		, rwCOMBINEPOSTCONCAT 
		);

	if( vOptFlagChk( e_flag_use_constrain_zoom ) )
	{
		AgcuCamDampingForce::bGetInst().m_zoomspeed = 0.f;
		AgcuCamDampingForce::bGetInst().vUdtMaskOff(AgcuCamDampingForce::e_update_zoom);
	}
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vForcedRotCX(RwReal offset)
{
	RwFrameRotate(
		RwCameraGetFrame( m_pRwCam )
		, bGetPtrCX()
		, offset
		, rwCOMBINEPOSTCONCAT
		);
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vAdjLookAt(RwReal currlen)
{
	FLOAT	fGap = m_fMaxLen - m_fMinLen;
	FLOAT	fCloseUpStart = fGap * 0.3f;
	
	if( currlen > fCloseUpStart )
	{
		m_stTarget.SetYOffset(g_offsetMin);
	}
	else
	{
		RwReal yoffset =
			g_offsetMax - (g_offsetMax-g_offsetMin)*currlen/fCloseUpStart;
		this->m_stTarget.SetYOffset(yoffset);
	}
	m_stTarget.CalcLookat( &m_v3dEyeSubAt );
	m_v3dLookat = m_stTarget.m_vLookat;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vAdjAlpha(RwReal currlen)
{
	if( !m_pClumpCharacter )
		return;
	if( vUdtMaskChk(e_mask_transparentTarget) )
		return;

	if( m_fMinLen <= currlen+1.0f	)
	{
		if( vUdtMaskChk(e_mask_alphaTarget) )
		{
			AgcmShadowmap* pAgcmShadowmap = (AgcmShadowmap*)GetModule("AgcmShadowmap");
			if(pAgcmShadowmap)
			{
				pAgcmShadowmap->registerClump(m_pClumpCharacter);
			}

			m_pAgcmRender->EndTransparentClump(m_pClumpCharacter);
			vUdtMaskOff(e_mask_alphaTarget);
		}

	}
	else
	{
		RwReal	fAlpha = (currlen<66.f) ? 0.f : 255.f * (currlen-66.f)/(m_fMinLen-66.f);

		if( !vUdtMaskChk(e_mask_alphaTarget) )
		{
			AgcmShadowmap* pAgcmShadowmap = (AgcmShadowmap*)GetModule("AgcmShadowmap");
			if(pAgcmShadowmap)
			{
				pAgcmShadowmap->unregisterClump(m_pClumpCharacter);
			}

			m_pAgcmRender->StartTransparentClump(m_pClumpCharacter, (RwInt32)fAlpha);
			vUdtMaskOn(e_mask_alphaTarget);
		}
		else
		{
			m_pAgcmRender->ChangeTransparentValue(m_pClumpCharacter, (RwInt32)fAlpha);
		}
	}
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bTransparentMainCharac()
{
	if( !m_pClumpCharacter )
		return;

	static BOOL	bOpaque	= FALSE;

	//set opaque
	vAdjAlpha( m_fMinLen + 1.0f );
	
	AgcmShadowmap* pAgcmShadowmap = (AgcmShadowmap*)GetModule("AgcmShadowmap");

	if( !bOpaque )
	{
		if(pAgcmShadowmap)
		{
			pAgcmShadowmap->unregisterClump(m_pClumpCharacter);
		}

		m_pAgcmRender->StartTransparentClump(m_pClumpCharacter, 0LU);

		vUdtMaskOn(e_mask_transparentTarget);
	}
	else
	{
		if(pAgcmShadowmap)
		{
			pAgcmShadowmap->registerClump(m_pClumpCharacter);
		}

		m_pAgcmRender->EndTransparentClump(m_pClumpCharacter);

		vUdtMaskOff(e_mask_transparentTarget);
	}

	bOpaque = !bOpaque;
};


//virtual
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
BOOL AgcmCamera2::OnAddModule()
{
	m_pAgcmMap = (AgcmMap*)GetModule("AgcmMap");
	ASSERT( NULL != m_pAgcmMap && "From AgcmCamera, AgcmMap Get Module Failed" );

	m_pApmObject = (ApmObject*)GetModule("ApmObject");
	ASSERT( NULL != m_pApmObject && "From AgcmCamera, ApmObject Get Module Failed" );
	m_pAgcmObject = (AgcmObject*)GetModule("AgcmObject");
	ASSERT( NULL != m_pAgcmObject && "From AgcmCamera, AgcmObject Get Module Failed" );

	m_pAgcmRender = (AgcmRender*)GetModule("AgcmRender");
	ASSERT( NULL != m_pAgcmRender && "From AgcmCamera, AgcmRender Get Module Failed" );

	m_pAgcmCharacter = (AgcmCharacter*)GetModule("AgcmCharacter");
	ASSERT( NULL != m_pAgcmCharacter && "From AgcmCharacter, AgcmRender Get Module Failed" );

	m_pAgpmCharacter = (AgpmCharacter*)GetModule("AgpmCharacter");
	ASSERT( NULL != m_pAgpmCharacter && "From m_pAgpmCharacter, AgcmRender Get Module Failed" );

	if (m_pAgpmCharacter)
	{
		if (!m_pAgpmCharacter->SetCallbackUpdatePosition(CBOnTargetMove, this))
			return FALSE;
	}

	if (m_pAgcmCharacter)
	{
		if (!m_pAgcmCharacter->SetCallbackSelfStopCharacter(CBOnTargetStop, this))
			return FALSE;
		if (!m_pAgcmCharacter->SetCallbackReleaseSelfCharacter(CBOnTargetStop, this))
			return FALSE;
		if (!m_pAgcmCharacter->SetCallbackSetSelfCharacter(CBOnTargetSet, this))
			return FALSE;
		//if (!m_pAgcmCharacter->SetCallbackIsCameraMoving(CBIsCameraMoving, this))
		//	return FALSE;
		if (!m_pAgcmCharacter->SetCallbackTurnCamera(CBTurnCamera, this))
			return FALSE;
	}

	m_pRwCam = GetCamera();

	RwV3d	vzero = {0.f, 0.f, 0.f};
	bSetCamFrm( vzero, m_v3dEyeSubAt );

	return TRUE;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
BOOL AgcmCamera2::OnInit()
{
	AS_REGISTER_TYPE_BEGIN(AgcmCamera2, AgcmCamera2);
		AS_REGISTER_METHOD0(void, Info);
		AS_REGISTER_VARIABLE(float, m_fMinLen);
		AS_REGISTER_VARIABLE(float, m_fMaxLen);
		AS_REGISTER_VARIABLE(float, m_fMinPitch);
		AS_REGISTER_VARIABLE(float, m_fMaxPitch);
		AS_REGISTER_METHOD2(void , ChangeHeight , float , float );
	AS_REGISTER_TYPE_END;

	AgpmCharacter * pcsAgpmCharacter = ( AgpmCharacter * ) GetModule( "AgpmCharacter" );
	if( pcsAgpmCharacter )
	{
		pcsAgpmCharacter->SetCallbackBindingRegionChange( CBRegionChange , this );
	}
	else
	{
		// 머시라!
		return FALSE;
	}

	return TRUE;
};

void	AgcmCamera2::Info()
{
	char	str[ 256 ];
	sprintf( str , "m_fMinLen   = %.0f" , m_fMinLen   );
	print_ui_console( str );
	sprintf( str , "m_fMaxLen   = %.0f" , m_fMaxLen   );
	print_ui_console( str );
	sprintf( str , "m_fMinPitch = %.0f" , m_fMinPitch );
	print_ui_console( str );
	sprintf( str , "m_fMaxPitch = %.0f" , m_fMaxPitch );
	print_ui_console( str );

	sprintf( str , "CharacterHeight = %.0f" , m_fCharacterHeight );
	print_ui_console( str );
	sprintf( str , "FaceHeight      = %.0f" , g_offsetMax );
	print_ui_console( str );
}

VOID	AgcmCamera2::ChangeHeight( float fCharacterHeight , float fFaceHeight )
{
	m_fCharacterHeight = g_offsetMin	= fCharacterHeight;
	g_offsetMax = fFaceHeight;
}
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
BOOL AgcmCamera2::OnDestroy()
{
	return TRUE;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
BOOL AgcmCamera2::OnIdle(UINT32 ulClockCount)
{
#ifdef _DEBUG
	if( KEYDOWN( VK_SHIFT ) )
	{
		if( KEYDOWN( 'R' ) )
		{
			// OptionFromFile();

			AgpdCharacter * pstCharacter = m_pAgcmCharacter->GetSelfCharacter();

			if( pstCharacter )
			{
				AgcdCharacter	*pstAgcdCharacter	= m_pAgcmCharacter->GetCharacterData(pstCharacter);

				RwV3d	vOffset = {0.f, 100.0f , 0.f};
				vOffset.y = m_fCharacterHeight;
				bSetTargetFrame( RpClumpGetFrame(pstAgcdCharacter->m_pClump), vOffset );
			}
		}

		//@{ kday 20051219
		// ;)
		// 콘솔창으로 카메라 매트릭스 정보를 볼수있게 함..
		//static BOOL bK = FALSE;
		//if( KEYDOWN_ONECE( 'K', bK ) )
		//	if( this->m_stTarget.m_pFrm )
		//		this->ShowInfo();
		//	else
		//		TOCON( Eff2Ut_ShowRwMat( RwFrameGetLTM( RwCameraGetFrame( m_pRwCam ) ), "CAMERA LTM" ) );
		//@} kday
	}
#endif //_DEBUG

	AgcdEffGlobal::bGetInst().bSetCurrTime( ulClockCount );
	(this->*m_fptrOnIdle)( AgcdEffGlobal::bGetInst().bGetDiffTimeS() );

	if( m_stPathWork.IsPlaying() )
	{
		// 플레이중이면...
		DWORD	uDiffTime = AgcdEffGlobal::bGetInst().bGetDiffTimeMS();
		RwMatrix	mat;
		FLOAT		fProjection;

		if( m_stPathWork.OnIdle( uDiffTime , & mat , & fProjection ) )
		{
			bSetCamFrm(mat);
			g_pEngine->SetProjection( fProjection );
		}
	}
	return TRUE;
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bLockAct(e_upt_mask eMask)
{
	DEF_FLAG_ON( m_ulLockMask, eMask );
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bUnlockAct(e_upt_mask eMask)
{
	DEF_FLAG_OFF( m_ulLockMask, eMask );
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
bool AgcmCamera2::vLockFlagChk(e_upt_mask eMask)
{
	//yaw, pitch, zoom
	return DEF_FLAG_CHK( m_ulLockMask, eMask ) ? true : false;
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::bOptFlagOn(e_opt_flag eopt)
{
	m_ulOptFlag |= eopt;
};
void AgcmCamera2::bOptFlagOff(e_opt_flag eopt)
{
	m_ulOptFlag &= (~eopt);
};
bool AgcmCamera2::vOptFlagChk(e_opt_flag eopt)const
{
	return ((m_ulOptFlag & eopt) ? true : false);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vUdtMaskOn(e_upt_mask eudt)
{
	m_ulUpdateMask |= eudt;
};
void AgcmCamera2::vUdtMaskOff(e_upt_mask eudt)
{
	m_ulUpdateMask &= (~eudt);
};
bool AgcmCamera2::vUdtMaskChk(e_upt_mask eudt)const
{
	return ((m_ulUpdateMask & eudt) ? true : false);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vAvailablePitch(RwReal& pitchDelta)
{
	if( !vOptFlagChk( e_flag_use_LimitPitch ) )
		return;

	RwV3d	at = *bGetPtrCZ();
	RwReal	currPitch	
		= DEF_R2D( atan2f( -at.y, sqrtf(at.x*at.x + at.z*at.z) ) );

	RwReal	min	= m_fMinPitch - currPitch;
	RwReal	max = m_fMaxPitch - currPitch;

	float fin = pitchDelta;
	T_CLAMP(pitchDelta, min, max);
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcmCamera2::vAvailableZoom(RwReal& zoomDelta)
{
	RwReal	currlen = RwV3dLength(&m_v3dEyeSubAt);
	if( vOptFlagChk( e_flag_use_LimitZoom ) )
	{//min and max
		RwReal max = currlen - m_fMinLen;
		RwReal min = currlen - m_fMaxLen;

		T_CLAMP(zoomDelta, min, max);
	}
	else
	{//max
		RwReal max = currlen - m_fMinLen;
		if( zoomDelta > max )
			zoomDelta = max;
	}
};




RwBool	IntersectTriangle(const RwV3d *puvRay	//uv	: unit vector
	, const RwV3d *pvCamPos
	, const RwV3d *pv0
	, const RwV3d *pv1
	, const RwV3d *pv2
	, RwReal* t
	, RwReal* u
	, RwReal* v)
{
	if( NULL == puvRay ) 
		return false;

	// Find vectors for two edges sharing vert0
	RwV3d	edge1,
			edge2;
	RwV3dSub(&edge1, pv1, pv0);
	RwV3dSub(&edge2, pv2, pv0);

    // Begin calculating determinant - also used to calculate U parameter
    RwV3d pvec;
    RwV3dCrossProduct( &pvec, puvRay, &edge2 );//RwV3dCrossProduct( &pvec, &edge2, puvRay );//

    // If determinant is near zero, ray lies in plane of triangle
    RwReal det = RwV3dDotProduct( &edge1, &pvec );

    RwV3d tvec;
    if( det > 0 )  
		RwV3dSub(&tvec, pvCamPos, pv0);
    else{
		RwV3dSub(&tvec, pv0, pvCamPos);
        det = -det;
    }
    if( det < 0.0001f )		
		return FALSE;

    // Calculate U parameter and test bounds
    *u = RwV3dDotProduct ( &tvec, &pvec );
    if( *u < 0.0f || *u > det )	
		return FALSE;

    // Prepare to test V parameter
    RwV3d qvec;
    RwV3dCrossProduct( &qvec, &tvec, &edge1 );//RwV3dCrossProduct( &qvec, &edge1, &tvec );//

    // Calculate V parameter and test bounds
    *v = RwV3dDotProduct( puvRay, &qvec );
    if( *v < 0.0f || *u + *v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    RwReal fInvDet = 1.0f / det;
    *u *= fInvDet;
    *v *= fInvDet;

	//if(t){// 떨어진 거리를 알고 싶지 않을때..
		*t = RwV3dDotProduct( &edge2, &qvec );
		*t *= fInvDet;
	//}

    return TRUE;
}
VOID CalcPickedPoint(RwV3d& v3dOut
	, const RwV3d& v0
	, const RwV3d& v1
	, const RwV3d& v2
	, RwReal u
	, RwReal v)
{
	v3dOut.x = v3dOut.y = v3dOut.z = 0.f;
	RwV3d	edge1,	edge2;

	RwV3dSub	( &edge1, &v1	, &v0	);
	RwV3dScale	( &edge1, &edge1, u		);
	RwV3dSub	( &edge2, &v2	, &v0	);
	RwV3dScale	( &edge2, &edge2, v		);

	RwV3dAdd( &v3dOut, &v0	  , &edge1 );
	RwV3dAdd( &v3dOut, &v3dOut, &edge2 );

	return;
};

void AgcmCamera2::vShowInfo(void)
{
	TOCON( "-------AgcmCamera2::ShowInfo-------\n" );
	TOCON( Eff2Ut_ShowRwMat( RwFrameGetLTM(m_stTarget.m_pFrm), "CHARACTER LTM") );
	TOCON( Eff2Ut_ShowRwMat( RwFrameGetLTM( RwCameraGetFrame( m_pRwCam ) ), "CAMERA LTM" ) );
	TOCON( Eff2Ut_ShowRwV3d( &m_v3dLookat, "m_v3dLookat" ) );
	TOCON( Eff2Ut_ShowRwV3d( &m_v3dEyeSubAt, "m_v3dEyeSubAt" ) );
	TOCON( Eff2Ut_ShowRwV3d( &m_stTarget.m_vLookat, "m_stTarget.m_vLookat" ) );
	TOCON( Eff2Ut_ShowRwV3d( &m_stTarget.m_vEye, "m_stTarget.m_vEye" ) );

	TOCON( Eff2Ut_ShowRwV3d( &m_vMove, "m_vMove" ) );
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fWYaw", m_fWYaw ) );
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fCPitch", m_fCPitch ) );
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fZoom", m_fZoom ) );
	
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fDesiredLen", m_fDesiredLen ) );
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fCurrLen", m_fCurrLen ) );
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fLimitedLen", m_fLimitedLen ) );

	
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fMinLen", m_fMinLen ) );
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fMaxLen", m_fMaxLen ) );
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fMinPitch", m_fMinPitch ) );
	TOCON( Eff2Ut_FmtMsg( "%20s : %f\n", "m_fMaxPitch", m_fMaxPitch ) );

	RwV2d	viewwindow;
	viewwindow = *RwCameraGetViewWindow(m_pRwCam);
	TOCON( Eff2Ut_FmtMsg( "width : %f, height : %f\n", viewwindow.x, viewwindow.y ) );
	
	TOCON( "-----------------------------------\n" );
}

BOOL	AgcmCamera2::PathWorkInfo::Save( const char * pFilename )
{
	// 저장..

	FILE	* pFile = fopen( ( LPCTSTR ) pFilename , "wb" );
	if( NULL == pFile )
	{
		g_pEngine->LuaErrorMessage( "PathWork :: File Open fail" );
		return FALSE;
	}
	else
	{
		// 버젼 정보 기록..
		// fwrite( ( void * ) &version , sizeof version , 1 , pFile );

		// 옵션.
		fwrite( ( void * ) &nType	 , sizeof nType		 , 1 , pFile );
		fwrite( ( void * ) &nSSType	 , sizeof nSSType	 , 1 , pFile );
		fwrite( ( void * ) &bAccel	 , sizeof bAccel	 , 1 , pFile );
		fwrite( ( void * ) &bClosed	 , sizeof bClosed	 , 1 , pFile );
		fwrite( ( void * ) &bLoop	 , sizeof bLoop		 , 1 , pFile );

		// 길이
		fwrite( ( void * ) &uDuration , sizeof uDuration , 1 , pFile );

		// 노드 갯수..
		INT32	nSize = ( INT32 ) vecCtrlMatrix.size();
		fwrite( ( void * ) &nSize, sizeof nSize , 1 , pFile );

		// 매트릭스 카피..
		fwrite( ( void * ) &vecCtrlMatrix[ 0 ] , sizeof RwMatrix , nSize , pFile );

		// 매트릭스 카피..
		fwrite( ( void * ) &vecProjection[ 0 ] , sizeof FLOAT , nSize , pFile );

		fclose( pFile );
		return TRUE;
	}
}

BOOL	AgcmCamera2::PathWorkInfo::Load( const char * pFilename )
{
	FILE	* pFile = fopen( pFilename , "rb" );
	
	if( NULL == pFile )
	{
		g_pEngine->LuaErrorMessage( "PathWork :: File Open fail" );
		return FALSE;
	}
	else
	{
		Clear();

		// 버젼 정보 기록..
		// fwrite( ( void * ) &version , sizeof version , 1 , pFile );

		// 옵션.
		fread( ( void * ) &nType		, sizeof nType		, 1 , pFile );
		fread( ( void * ) &nSSType		, sizeof nSSType	, 1 , pFile );
		fread( ( void * ) &bAccel		, sizeof bAccel		, 1 , pFile );
		fread( ( void * ) &bClosed		, sizeof bClosed	, 1 , pFile );
		fread( ( void * ) &bLoop		, sizeof bLoop		, 1 , pFile );

		// 길이
		fread( ( void * ) &uDuration , sizeof uDuration , 1 , pFile );

		// 노드 갯수..
		INT32	nSize;
		fread( ( void * ) &nSize , sizeof nSize , 1 , pFile );

		// 매트릭스 카피..

		if( nSize > 0 && nSize < 256 )
		{
			RwMatrix	* pMatrix		= new RwMatrix[ nSize ];
			FLOAT		* pProjection	= new FLOAT[ nSize ];

			if( pMatrix && pProjection )
			{
				fread( ( void * ) pMatrix , sizeof RwMatrix , nSize , pFile );
				fread( ( void * ) pProjection , sizeof FLOAT , nSize , pFile );

				for( int i = 0 ; i < nSize ; i++ )
				{
					this->Push( &pMatrix[ i ] , pProjection[ i ] );
				}

				fclose( pFile );

				delete [] pMatrix;
				delete [] pProjection;
				return TRUE;
			}
			else
			{
				fclose( pFile );
				return FALSE;
			}
		}
		else
		{
			fclose( pFile );
			return FALSE;
		}

	}
}

BOOL	AgcmCamera2::PathWorkInfo::Play()
{
	Stop();

	if( !PreparePathWork() ) return FALSE;
	if( !PrepareAngleWork() ) return FALSE;

	return TRUE;
}

BOOL	AgcmCamera2::PathWorkInfo::Stop()
{
	DEF_SAFEDELETE(m_pPathWork	);
	DEF_SAFEDELETE(m_pAngleWork	);

	m_pPathWork		= NULL;
	m_pAngleWork	= NULL;

	return TRUE;
}

BOOL	AgcmCamera2::PathWorkInfo::PreparePathWork	()
{
	AgcuPathWork * pSplinePathWork = NULL;
	if( vecCtrlMatrix.size() >= 4 )
	{
		switch( nType )
		{
		case 	SINGLESPLINE:
			{
				switch( nSSType )
				{
				default:
				case	SST_NORMAL		:
					{
						AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( uDuration );

						if( bLoop )	pSpline->bLoop();
						else			pSpline->bNoLoop();

						pSpline->bReset();

						pSpline->bForward();
						//pSpline->bBackward();

						pSpline->bSetActType(1);		//flight

						if( bAccel )	pSpline->bSetAccel(1);			//accel
						else			pSpline->bSetAccel(0);			//accel

						stSetSpline	stSet;

						if( bClosed ) stSet.closed		= true;
						else			stSet.closed		= false;

						stSet.ctrlMatrices	= &vecCtrlMatrix[ 0 ];
						stSet.numCtrl		= vecCtrlMatrix.size();

						pSpline->bInitSpline(stSet);

						pSplinePathWork = pSpline;
					}
					break;
				case	SST_STARE	:
					{ 
						AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( uDuration );

						// 첫번째 녀석은 위치로 사용..
						if( vecCtrlMatrix.size() >= 5 )
						{
							if( bLoop )	pSpline->bLoop();
							else			pSpline->bNoLoop();

							pSpline->bReset();

							pSpline->bForward();
							//pSpline->bBackward();

							( ( AgcuPathSingleSpline * ) pSpline )->bSetActType(1);		//flight

							if( bAccel )	pSpline->bSetAccel(1);			//accel
							else			pSpline->bSetAccel(0);			//accel

							stSetSpline	stSet;

							if( bClosed ) stSet.closed		= true;
							else			stSet.closed		= false;

							stSet.ctrlMatrices	= &vecCtrlMatrix[ 0 ] + 1;
							stSet.numCtrl		= vecCtrlMatrix.size() - 1;

							pSpline->bInitSpline(stSet);
							pSpline->bSetFixedLookat( &vecCtrlMatrix[ 0 ].pos);

							pSplinePathWork = pSpline;
						}
					}
					break;
				case SST_PARALLEL:
					{
						// ...
					}
					break;
				case SST_FLIGHT:
					{
						// ...
					}
					break;
				}
			}
			break;
		case	DOUBLESPLINE:
			{
				AgcuPathDoubleSpline * pSpline = new AgcuPathDoubleSpline( uDuration );

				if( bLoop )	pSpline->bLoop();
				else			pSpline->bNoLoop();

				pSpline->bReset();

				pSpline->bForward();
				//pSpline->bBackward();

				pSpline->bSetActType(0);	//side vector is parallel with xz_plane

				if( bAccel )	pSpline->bSetAccel(1);			//accel
				else			pSpline->bSetAccel(0);			//accel

				stSetSpline	stSet;

				if( bClosed ) stSet.closed		= true;
				else			stSet.closed		= false;

				stSet.ctrlMatrices	= &vecCtrlMatrix[ 0 ];
				stSet.numCtrl		= vecCtrlMatrix.size();

				pSpline->bInitSpline(stSet);

				pSplinePathWork = pSpline;
			}
			break;
		}

	}
	else
	{
		g_pEngine->LuaErrorMessage( "PathWork :: At least 4 nodes are needed" );
		return FALSE;
	}

	m_pPathWork = pSplinePathWork;
	return TRUE;
}

BOOL	AgcmCamera2::PathWorkInfo::PrepareAngleWork()
{
	AgcuPathWork * pSplineAngleWork = NULL;
	if( vecProjection.size() >= 4 )
	{
		AgcuPathSingleSpline * pSpline = new AgcuPathSingleSpline( uDuration );

		if( bLoop )		pSpline->bLoop();
		else			pSpline->bNoLoop();

		pSpline->bReset();

		pSpline->bForward();
		//pSpline->bBackward();

		pSpline->bSetActType(1);		//flight

		if( bAccel )	pSpline->bSetAccel(1);			//accel
		else			pSpline->bSetAccel(0);			//accel

		stSetSpline	stSet;

		if( bClosed )	stSet.closed		= true;
		else			stSet.closed		= false;

		vector< RwMatrix >	matPos;
		matPos.reserve( vecProjection.size() );
		RwMatrix mat;
		mat.pos.y = 0.0f;
		mat.pos.z = 0.0f;

		// Matrix 에 pos 값만 펑션 내부에서 사용함.
		for( int i = 0 ; i < ( int ) vecProjection.size() ; i ++ )
		{
			mat.pos.x = vecProjection[ i ];
			matPos.push_back( mat );
		}

		stSet.ctrlMatrices	= &matPos[ 0 ];
		stSet.numCtrl		= vecProjection.size();

		pSpline->bInitSpline(stSet);

		pSplineAngleWork = pSpline;
	}
	else
	{
		g_pEngine->LuaErrorMessage( "PathWork :: At least 4 nodes are needed" );
		return FALSE;
	}

	m_pAngleWork	= pSplineAngleWork;
	return TRUE;
}

BOOL	AgcmCamera2::PathWorkInfo::OnIdle( DWORD uDiffTime , RwMatrix * pMatrix , FLOAT * pfProjection )
{
	ASSERT( IsPlaying() );
	ASSERT( pMatrix );
	ASSERT( pfProjection );

	BOOL	bRet = FALSE;

	if( m_pPathWork->GetAccumTime() >= m_pPathWork->GetLife() + uDiffTime )
	{
		Stop();
	}
	else
	{
		if( !m_pPathWork->bOnIdle( *pMatrix , uDiffTime ) )
		{
			bRet = TRUE;
		}
		RwMatrix matTemp;
		if( !m_pAngleWork->bOnIdle( matTemp , uDiffTime ) )
		{
			// 프로젝션은 스플라인에 X좌표 값으로 한다.
			*pfProjection = matTemp.pos.x;
		}
	}
	
	return bRet;
}

void	AgcmCamera2::PathWorkInfo::Clear()
{
	uDuration	= 30000			;
	nType		= DOUBLESPLINE	;
	nSSType		= SST_NORMAL	;
	bAccel		= TRUE			;
	bClosed		= FALSE			;
	bLoop		= FALSE			;

	vecCtrlMatrix.clear();
	vecProjection.clear();
}

INT32	AgcmCamera2::PathWorkInfo::Size()
{
	return ( INT32 ) vecCtrlMatrix.size();
}

BOOL	AgcmCamera2::PathWorkInfo::Push( RwMatrix * pMatrix , FLOAT fProjection )
{
	vecCtrlMatrix.push_back( *pMatrix		);
	vecProjection.push_back( fProjection	);
	return TRUE;
}

BOOL	AgcmCamera2::PathWorkInfo::Pop()
{
	if( Size() )
	{
		//vecCtrlMatrix.erase( vecCtrlMatrix.rbegin() );
		//vecProjection.erase( vecProjection.rbegin() );
		vecCtrlMatrix.pop_back();
		vecProjection.pop_back();
		return TRUE;
	}
	else return FALSE;
}
void	AgcmCamera2::CameraStack::Push()
{
	RwMatrix	*	pMatrix		= RwFrameGetMatrix( RwCameraGetFrame( g_pEngine->m_pCamera ) )	;
	FLOAT			fProjection	= g_pEngine->GetProjection()									;

	Info	stInfo;
	stInfo.matrix		= * pMatrix;
	stInfo.fProjection	= fProjection;
	stack.push( stInfo );
}
void	AgcmCamera2::CameraStack::Pop()
{
	if( stack.size() )
	{
		Info	* pInfo	= &stack.top();
		g_pAgcmCamera2->bSetCamFrm( pInfo->matrix);
		g_pEngine->SetProjection( pInfo->fProjection );

		stack.pop();
	}
}

///////////////////////////////////////////////////////////////////
// Lua !

LuaGlue	LG_ClearCameraState( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nSize = ( INT32 ) g_pAgcmCamera2->m_stPathWork.Size();

	std::string	str;
	stl_printf( str , "Remove all pushed Matrix(%d)." , nSize );
	g_pEngine->LuaErrorMessage( str.c_str() );

	g_pAgcmCamera2->m_stPathWork.Clear();
	
	return 0;
}
LuaGlue	LG_PushCameraState( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	RwMatrix	*	pMatrix		= RwFrameGetMatrix( RwCameraGetFrame( g_pEngine->m_pCamera ) )	;
	FLOAT			fProjection	= g_pEngine->GetProjection()									;

	g_pAgcmCamera2->m_stPathWork.Push( pMatrix , fProjection );

	std::string	str;
	stl_printf( str , "Node Added( Total %d )." , g_pAgcmCamera2->m_stPathWork.Size() );
	g_pEngine->LuaErrorMessage( str.c_str() );

	return 0;
}
LuaGlue	LG_PopCameraState( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	if( g_pAgcmCamera2->m_stPathWork.Size() )
	{
		g_pAgcmCamera2->m_stPathWork.Pop();

		std::string	str;
		stl_printf( str , "Node Removed( Total %d )." , g_pAgcmCamera2->m_stPathWork.Size() );
		g_pEngine->LuaErrorMessage( str.c_str() );
	}
	else
	{
		std::string	str;
		stl_printf( str , "No node left." );
		g_pEngine->LuaErrorMessage( str.c_str() );
	}
	return 0;
}

LuaGlue	LG_SetPathWorkDuration( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	UINT32	uDuration = ( UINT32 ) pLua->GetNumberArgument( 1 , 30000 );

	g_pAgcmCamera2->m_stPathWork.uDuration = uDuration;

	return 0;
}
LuaGlue	LG_PlaySpline( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	INT32	nPlayTime		= ( INT32 ) pLua->GetNumberArgument( 1 , 10000 );

	BOOL bRet = g_pAgcmCamera2->m_stPathWork.Play();

	pLua->PushNumber( ( double ) bRet );
	return 1;
}

LuaGlue	LG_StopSpline( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	BOOL bRet = g_pAgcmCamera2->m_stPathWork.Stop();

	pLua->PushNumber( ( double ) bRet );
	return 0;
}

LuaGlue	LG_SaveSpline( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	const char *pStr = pLua->GetStringArgument( 1 , "*empty*" );

    char   szModName[ MAX_PATH + 1 ] ;
	GetCurrentDirectory( MAX_PATH , szModName );
	std::string	strPath;

	strPath =	szModName		;
	strPath +=	"\\"			;
	strPath +=	pStr			;

	BOOL	bRet = g_pAgcmCamera2->m_stPathWork.Save( strPath.c_str() );

	if( bRet )	g_pEngine->LuaErrorMessage( "Save Spline Successful" );
	else		g_pEngine->LuaErrorMessage( "Save Spline Failure" );

	pLua->PushNumber( ( double ) bRet );
	return 1;
}

LuaGlue	LG_LoadSpline( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();
	const char *pStr = pLua->GetStringArgument( 1 , "*empty*" );

	char   szModName[ MAX_PATH + 1 ] ;
	GetCurrentDirectory( MAX_PATH , szModName );
	std::string	strPath;

	strPath =	szModName		;
	strPath +=	"\\"			;
	strPath +=	pStr			;

	BOOL	bRet = g_pAgcmCamera2->m_stPathWork.Load( strPath.c_str() );

	if( bRet )
	{
		// 로드 성공인건 메시지 표시하지 않음.
		// g_pEngine->LuaErrorMessage( "Load Spline Successful" );
	}
	else		g_pEngine->LuaErrorMessage( "Load Spline Failure" );

	pLua->PushNumber( ( double ) bRet );
	return 1;
}

LuaGlue	LG_PushCameraPosition( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	g_pAgcmCamera2->m_stCameraStack.Push();
	return 0;
}
LuaGlue	LG_PopCameraPosition( lua_State *L )
{
	AuLua * pLua = AuLua::GetPointer();

	g_pAgcmCamera2->m_stCameraStack.Pop();
	return 0;
}

void		AgcmCamera2::OnLuaInitialize( AuLua * pLua )
{
	luaDef MVLuaGlue[] = 
	{
		{"ClearCameraState"		,	LG_ClearCameraState		},
		{"PushCameraState"		,	LG_PushCameraState		},
		{"PopCameraState"		,	LG_PopCameraState		},
		{"SetPathWorkDuration"	,	LG_SetPathWorkDuration	},
		{"PlaySpline"			,	LG_PlaySpline			},
		{"StopSpline"			,	LG_StopSpline			},
		{"SaveSpline"			,	LG_SaveSpline			},
		{"LoadSpline"			,	LG_LoadSpline			},
		{"PushCameraPosition"	,	LG_PushCameraPosition	},
		{"PopCameraPosition"	,	LG_PopCameraPosition	},
		{NULL					,	NULL					},
	};

	for(int i=0; MVLuaGlue[i].name; i++)
	{
		pLua->AddFunction(MVLuaGlue[i].name, MVLuaGlue[i].func);
	}
}

BOOL	AgcmCamera2::CBRegionChange(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmCamera2*	pThis		= (AgcmCamera2*) pClass;
	AgpdCharacter*	pdCharacter = (AgpdCharacter*)	pData;
	static ApmMap *	_spcsApmMap	= ( ApmMap * ) pThis->GetModule( "ApmMap" );

	ApmMap::RegionTemplate*	pTemplate = _spcsApmMap->GetTemplate( pdCharacter->m_nBindingRegionIndex );
	ASSERT( NULL != pTemplate );

	if( pTemplate )
	{
		// 탑뷰 모드에서만 검사하도록 한다.
		if( pTemplate->fMaxCameraHeight < pThis->m_fMaxLen )
		{
			pThis->m_fMaxLen = pTemplate->fMaxCameraHeight;
			if( pThis->m_fMaxLen < pThis->m_fMinLen )
			{
				pThis->m_fMinLen = pThis->m_fMaxLen;
			}
			pThis->bZoom( 0.0f );
		}
	}

	return TRUE;
}

#ifdef _DEBUG

enum e_cam_opt
{
	//option
	e_copt_spring			= 0,
	e_copt_damping			,
	e_copt_constrainZoom	,
	e_copt_constrainPitch	,
	e_copt_limitZoom		,
	e_copt_limitPitch		,
	e_copt_restoreZoom		,
	e_copt_restorePitch		,	//NOT AVIALABLE
	e_copt_180turnTime		,
	e_copt_180turnAngle		,

	//limit
	e_copt_minLen			,
	e_copt_maxLen			,
	e_copt_minPitch			,	// -89 이상
	e_copt_maxPitch			,	//  89 이하

	//damping
	e_copt_kdRotX			,
	e_copt_kdRotY			,
	e_copt_kdZoom			,	
	e_copt_maxOmegaX		,
	e_copt_maxOmegaY		,	
	e_copt_maxZoomSpeed		,

	//spring
	e_copt_ksLinear			,
	e_copt_ksSpiral			,	//NOT AVIALABLE
	e_copt_limitLen			,

	
	e_copt_yawcoef			,
	e_copt_pitchcoef		,
	e_copt_zoomcoef			,
	
	e_copt_yawcoef_nodamp	,
	e_copt_pitchcoef_nodamp	,
	e_copt_zoomcoef_nodamp	,

	e_copt_offsetMin		,
	e_copt_offsetMax		,

	e_copt_viewwindow_width	,
	e_copt_viewwindow_height,

	e_copt_num				,
};

INT GetWord(LPSTR dst, LPSTR src, INT &reads)
{
	INT32	i;

	for(i=reads; i < (int)strlen(src); i++)
		if( src[i] == ' ' || src[i] == '\t' )
			reads++;
		else
			break;

	for(i=reads; i < (int)strlen(src); i++) {
		if( src[i] == ' ' || src[i] == '\t' ) {
			*dst = '\0';
			break;
		} else {			
			*(dst++) = src[i];
			reads++;
		}
	}//for

	*dst = '\0';

	return(1);
}
void AgcmCamera2::OptionFromFile(void)
{
	const char*	SZCAMOPTION = "cam2opt.txt";

	const char* SZKEY[e_copt_num] =	{
		"spring",
		"damping",
		"constrainZoom",
		"constrainPitch",
		"limitZoom",
		"limitPitch",
		"restoreZoom",
		"restorePitch",
		"180turnTime",
		"180turnAngle",

		"minLen",
		"maxLen",
		"minPitch",
		"maxPitch",

		"kdRotX",
		"kdRotY",
		"kdZoom",
		"maxOmegaX",
		"maxOmegaY",
		"maxZoomSpeed",

		"ksLinear",
		"ksSpiral",
		"limitLen",

		"g_yawcoef",
		"g_pitchcoef",
		"g_zoomcoef",
		
		"g_yawcoef_nodamp",
		"g_pitchcoef_nodamp",
		"g_zoomcoef_nodamp",

		"g_offsetMin",
		"g_offsetMax",

		"g_viewWidth",
		"g_viewHeight",
	};



	AuAutoFile	fp(SZCAMOPTION, "r");
	if( !fp )
		return;

	char	lineBuff[MAX_PATH] = "";
	char	wordBuff[MAX_PATH] = "";
	while( !feof(fp) )
	{
		memset(lineBuff, 0, sizeof(lineBuff) );
		if( !fgets(lineBuff, MAX_PATH, fp ) )
		{
			return;
		}

		int nLineLen = strlen(lineBuff);
		if( lineBuff[nLineLen-1] == '\n' )
			lineBuff[nLineLen-1] = '\0';

		int nReadByte = 0;

		GetWord(wordBuff, lineBuff, nReadByte);
		//comment check
		if( wordBuff[0] == '/' && wordBuff[1] == '/' )	continue;
		//null check
		if( wordBuff[0] == '\0' )						continue;


		e_cam_opt cmp = e_copt_num;
		for( int i=0; i<e_copt_num; ++i )
		{
			if( !strcmp(SZKEY[i], wordBuff) ){
				cmp = (e_cam_opt)i;		break;
			}
		}

		GetWord( wordBuff, lineBuff, nReadByte );	// "="
		ASSERT( !strcmp(wordBuff, "=") );
		GetWord( wordBuff, lineBuff, nReadByte );	// value

		switch( cmp )
		{
		//option:
		case e_copt_spring			:{BOOL b = atoi(wordBuff); b ? bOptFlagOn(e_flag_use_springforce		) : bOptFlagOff(e_flag_use_springforce			);}break;
		case e_copt_damping			:{BOOL b = atoi(wordBuff); b ? bOptFlagOn(e_flag_use_dampingforce		) : bOptFlagOff(e_flag_use_dampingforce			);}break;
		case e_copt_constrainZoom	:{BOOL b = atoi(wordBuff); b ? bOptFlagOn(e_flag_use_constrain_zoom		) : bOptFlagOff(e_flag_use_constrain_zoom		);}break;
		case e_copt_constrainPitch	:{BOOL b = atoi(wordBuff); b ? bOptFlagOn(e_flag_use_constrain_pitch	) : bOptFlagOff(e_flag_use_constrain_pitch		);}break;
		case e_copt_limitZoom		:{BOOL b = atoi(wordBuff); b ? bOptFlagOn(e_flag_use_LimitZoom			) : bOptFlagOff(e_flag_use_LimitZoom			);}break;
		case e_copt_limitPitch		:{BOOL b = atoi(wordBuff); b ? bOptFlagOn(e_flag_use_LimitPitch			) : bOptFlagOff(e_flag_use_LimitPitch			);}break;
		case e_copt_restoreZoom		:{BOOL b = atoi(wordBuff); b ? bOptFlagOn(e_flag_use_reStoreSpring_zoom	) : bOptFlagOff(e_flag_use_reStoreSpring_zoom	);}break;
		case e_copt_restorePitch	:{BOOL b = atoi(wordBuff); b ? bOptFlagOn(e_flag_use_reStoreSpring_pitch) : bOptFlagOff(e_flag_use_reStoreSpring_pitch	);}break;
		case e_copt_180turnTime		:{m_fTurnTime = (float)atof(wordBuff);
									  m_fTurnSpeed = m_fTurnAngle/m_fTurnTime;
									  m_fTurnCoef = 180.f / ( sinf(m_fTurnAngle) + m_fTurnAngle );
									 }break;
		case e_copt_180turnAngle	:{m_fTurnAngle = DEF_D2R((float)atof(wordBuff));
									  m_fTurnSpeed = m_fTurnAngle/m_fTurnTime;
									  m_fTurnCoef = 180.f / ( sinf(m_fTurnAngle) + m_fTurnAngle );
									 }break;
		//limit:
		case e_copt_minLen			:{m_fMinLen	  = (float)atof(wordBuff);}break;
		case e_copt_maxLen			:{m_fMaxLen	  = (float)atof(wordBuff);}break;
		case e_copt_minPitch		:{m_fMinPitch = (float)atof(wordBuff);}break;
		case e_copt_maxPitch		:{m_fMaxPitch = (float)atof(wordBuff);}break;
		//damping:
		case e_copt_kdRotX			:{AgcuCamDampingForce::bGetInst().m_v2KdRot.x = (float)atof(wordBuff);}break;
		case e_copt_kdRotY			:{AgcuCamDampingForce::bGetInst().m_v2KdRot.y = (float)atof(wordBuff);}break;
		case e_copt_kdZoom			:{AgcuCamDampingForce::bGetInst().m_KdZoom = (float)atof(wordBuff);}break;
		case e_copt_maxOmegaX		:{AgcuCamDampingForce::bGetInst().m_v2OmegaLimit.x = (float)atof(wordBuff);}break;
		case e_copt_maxOmegaY		:{AgcuCamDampingForce::bGetInst().m_v2OmegaLimit.y = (float)atof(wordBuff);}break;
		case e_copt_maxZoomSpeed	:{AgcuCamDampingForce::bGetInst().m_LimitZoomSpeed = (float)atof(wordBuff);}break;
		//spring:
		case e_copt_ksLinear		:{AgcuCamSpringForce::bGetInst().m_fKsLinear = (float)atof(wordBuff);
									  AgcuCamSpringForce::bGetInst().m_fKdLinear = sqrtf(AgcuCamSpringForce::bGetInst().m_fKsLinear);
									 }break;
		case e_copt_ksSpiral		:{AgcuCamSpringForce::bGetInst().m_fKsSpiral = (float)atof(wordBuff);
									  AgcuCamSpringForce::bGetInst().m_fKdSpiral = sqrtf(AgcuCamSpringForce::bGetInst().m_fKsSpiral);
									 }break;
		case e_copt_limitLen		:{AgcuCamSpringForce::bGetInst().m_fLimitSpringLen = (float)atof(wordBuff);}break;

		case e_copt_yawcoef			:{g_yawcoef		= (float)atof(wordBuff);}break;
		case e_copt_pitchcoef		:{g_pitchcoef	= (float)atof(wordBuff);}break;
		case e_copt_zoomcoef		:{g_zoomcoef	= (float)atof(wordBuff);}break;
			
		case e_copt_yawcoef_nodamp	:{g_yawcoef_nodamp		= (float)atof(wordBuff);}break;
		case e_copt_pitchcoef_nodamp:{g_pitchcoef_nodamp	= (float)atof(wordBuff);}break;
		case e_copt_zoomcoef_nodamp	:{g_zoomcoef_nodamp		= (float)atof(wordBuff);}break;

		case e_copt_offsetMin		:{g_offsetMin	= (float)atof(wordBuff);}break;
		case e_copt_offsetMax		:{g_offsetMax	= (float)atof(wordBuff);}break;

		case e_copt_viewwindow_width:{g_viewWindow.x = (float)atof(wordBuff); bSetViewWindow(::g_viewWindow);}break;
		case e_copt_viewwindow_height:{g_viewWindow.y = (float)atof(wordBuff); bSetViewWindow(::g_viewWindow);}break;
		}
	}
};


struct vtxFrustum
{
	RwV3d		m_v3Pos;
	RwUInt32	m_dwColr;
}VTXFRUSTUM;
void AgcmCamera2::RenderFrustum(RwUInt32 colr)
{
	RwV3d	*pFrustum = m_pRwCam->frustumCorners;

	vtxFrustum	vtx[8];
	vtx[0].m_dwColr	= colr;		vtx[0].m_v3Pos	= *(pFrustum + 0);
	vtx[1].m_dwColr	= colr;		vtx[1].m_v3Pos	= *(pFrustum + 1);
	vtx[2].m_dwColr	= colr;		vtx[2].m_v3Pos	= *(pFrustum + 2);
	vtx[3].m_dwColr	= colr;		vtx[3].m_v3Pos	= *(pFrustum + 3);
	vtx[4].m_dwColr	= colr;		vtx[4].m_v3Pos	= *(pFrustum + 4);
	vtx[5].m_dwColr	= colr;		vtx[5].m_v3Pos	= *(pFrustum + 5);
	vtx[6].m_dwColr	= colr;		vtx[6].m_v3Pos	= *(pFrustum + 6);
	vtx[7].m_dwColr	= colr;		vtx[7].m_v3Pos	= *(pFrustum + 7);

	WORD	index[12*2] = {
		0,1,	1,2,	2,3,	3,0,	//near
		4,5,	5,6,	6,7,	7,4,	//far
		0,4,	1,5,	2,6,	3,7,	//side
	};

	RwD3D9SetRenderState(D3DRS_LIGHTING, FALSE);
	RwD3D9SetTexture ( NULL, 0LU );
	RwD3D9SetFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE);
	RwMatrix matid;
	RwMatrixSetIdentity(&matid);
	RwD3D9SetTransformWorld(&matid);

	RwD3D9DrawIndexedPrimitiveUP( D3DPT_LINELIST
								, 0LU
								, 8LU
								, 12LU
								, index
								, vtx
								, sizeof(vtxFrustum)
								);
}
#endif
