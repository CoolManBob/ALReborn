// AgcuCamSpringForce.cpp: implementation of the AgcuCamSpringForce class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcuEffUtil.h"
#include "AgcuCamSpringForce.h"

const	RwReal	KSTOCHARAC		= 5.5f;
const	RwReal	LIMITSPRINGLEN	= 200.f;

const	RwReal	EPSILON_LINEAR	= 2.f;	//centi meter
const	RwReal	EPSILON_SPIRAL	= 2.f;	//degree


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AgcuCamSpringForce::AgcuCamSpringForce()
{
	m_ulOptionFlag		= e_option_use_2ndLinearSpring;

	m_ulUpdateMask		= 0LU;

	m_v3DisiredEye.x	= 
	m_v3DisiredEye.y	= 
	m_v3DisiredEye.z	= 0.f;
	m_fKsToCharac		= KSTOCHARAC;
	m_fKdToCharac		= sqrtf(m_fKsToCharac);
	m_fLimitSpringLen	= LIMITSPRINGLEN;

	m_v3dPrevVel.x		= 
	m_v3dPrevVel.y		= 
	m_v3dPrevVel.z		= 0.f;


	m_fKsLinear			= 1.5f;
	m_fKdLinear			= sqrtf(m_fKsLinear);
	m_fPrevLinearSpeed	= 0.f;

	m_fKsSpiral			= 1.5f;
	m_fKdSpiral			= sqrtf(m_fKsSpiral);
	m_fPrevAngularSpeed	= 0.f;
}

AgcuCamSpringForce::~AgcuCamSpringForce()
{
}
AgcuCamSpringForce& AgcuCamSpringForce::bGetInst()
{
	static AgcuCamSpringForce inst;
	return inst;
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::bOnReleaseLinearSpring(AgcmCamera2* pAgcmCamera2, RwReal fElapsed, RwReal fOffset)
{
	RwReal coef = m_fKsLinear*fElapsed;
	pAgcmCamera2->vForcedZoom( fOffset * ( (coef > 1.f ) ? 1.f : coef ) );
	/*
	//이번프레임은 스킵
	if( !vUdtMaskChk(e_update_linear) )
	{
		m_fPrevLinearSpeed = 0.f;
		vUdtMaskOn(e_update_linear);
	}
	/**/
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::bOnReleaseSpiralSpring(AgcmCamera2* pAgcmCamera2, RwReal fElapsed, RwReal fOffset)
{
	//이번프레임은 스킵
	if( !vUdtMaskChk(e_update_spiral) )
	{
		m_fPrevAngularSpeed = 0.f;
		vUdtMaskOn(e_update_spiral);
	}
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::bOnMoveCharac(AgcmCamera2* pAgcmCamera2)
{
	return;
	ASSERT( pAgcmCamera2->m_stTarget.m_pFrm );
	
	if( !vUdtMaskChk(e_update_2ndLinear) )
	{
		m_v3dPrevVel.x = 
		m_v3dPrevVel.y = 
		m_v3dPrevVel.z = 0.f;
		vUdtMaskOn(e_update_2ndLinear);
	}

	m_v3DisiredEye = pAgcmCamera2->m_stTarget.m_vEye;
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::bOnIdle(AgcmCamera2* pAgcmCamera2, RwReal fElasped)
{
	PROFILE("AgcuCamSpringForce::bOnIdle");

	vOnIdleExt(pAgcmCamera2, fElasped);
//	vOnIdelLinear(pAgcmCamera2, fElasped);
//	vOnIdelSpiral(pAgcmCamera2, fElasped);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::vOnIdleExt(AgcmCamera2* pAgcmCamera2, RwReal fElasped)
{
	if( /*!vUdtMaskChk( e_update_2ndLinear ) || */fElasped == 0.f )
		return;

	RwReal	coef = m_fKdToCharac * fElasped;
	if( coef > 1.f )
		coef = 1.f;

	RwV3d	targetSubEye;
	RwV3dSub( &targetSubEye, &pAgcmCamera2->m_stTarget.m_vEye, pAgcmCamera2->bGetPtrEye() );
	RwV3dScale( &targetSubEye, &targetSubEye, coef );
	pAgcmCamera2->vUdtMaskOn( AgcmCamera2::e_mask_move );
	RwV3dAdd( &pAgcmCamera2->m_vMove, &pAgcmCamera2->m_vMove, &targetSubEye );
};
/*
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::vOnIdelLinear(AgcmCamera2* pAgcmCamera2, RwReal fElasped)
{
	if( !vUdtMaskChk( e_update_linear ) || fElasped == 0.f )
		return;

	//RwReal fLen	 = RwV3dLength(&pAgcmCamera2->m_v3dEyeSubAt);
	RwReal fDist = pAgcmCamera2->m_fLimitedLen - pAgcmCamera2->m_fCurrLen;

	if( fDist < EPSILON_LINEAR || fDist <= 0.f ){
		vUdtMaskOff( e_update_linear );
	}else{
		RwReal	spring	= fDist * m_fKsLinear * fElasped;
		RwReal	damp	= m_fPrevLinearSpeed * m_fKdLinear * fElasped;
		fDist	= (spring - damp) * fElasped;
		if( fDist > pAgcmCamera2->m_fLimitedLen - pAgcmCamera2->m_fCurrLen )
		{
			fDist = pAgcmCamera2->m_fLimitedLen - pAgcmCamera2->m_fCurrLen;
			vUdtMaskOff( e_update_linear );
		}
		m_fPrevLinearSpeed = spring - damp;
	}

	RwV3d	zoom;
	RwV3dScale( &zoom, pAgcmCamera2->bGetPtrCZ(), -fDist );
	pAgcmCamera2->LockFrame();
	RwFrameTranslate( RwCameraGetFrame(pAgcmCamera2->m_pRwCam), &zoom, rwCOMBINEPOSTCONCAT );
	pAgcmCamera2->UnlockFrame();

	RwV3d	eyeSubAt;
	RwV3dSub( &eyeSubAt, pAgcmCamera2->bGetPtrEye(), &pAgcmCamera2->m_v3dLookat );
	pAgcmCamera2->m_fCurrLen = RwV3dLength( &eyeSubAt );
};
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::vOnIdelSpiral(AgcmCamera2* pAgcmCamera2, RwReal fElasped)
{
	if( !vUdtMaskChk( e_update_spiral ) || fElasped == 0.f )
		return;

	RwV3d	at = *pAgcmCamera2->bGetPtrCZ();
	RwReal	currpitch = DEF_R2D( atan2f( sqrtf(at.x*at.x + at.z*at.z), at.y ) );
	RwReal	fDPitch = pAgcmCamera2->m_fDesiredPitch - currpitch;

	if( fabsf(fDPitch) < EPSILON_SPIRAL ){
		vUdtMaskOff( e_update_spiral );
	}else{
		RwReal	spring	= fDPitch * m_fKsSpiral * fElasped;
		RwReal	damp	= m_fPrevAngularSpeed * m_fKdSpiral * fElasped;
		fDPitch = (spring + damp) * fElasped;
		m_fPrevAngularSpeed = spring + damp;
	}

	pAgcmCamera2->LockFrame();
	RwFrameRotate( RwCameraGetFrame(pAgcmCamera2->m_pRwCam)
				 , pAgcmCamera2->bGetPtrCX()
				 , fDPitch
				 , rwCOMBINEPRECONCAT );
	pAgcmCamera2->UnlockFrame();
};
/**/

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::bOptFlagOn(eFlagOption eopt)
{
	DEF_FLAG_ON(m_ulOptionFlag, eopt);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::bOptFlagOff(eFlagOption eopt)
{
	DEF_FLAG_OFF(m_ulOptionFlag, eopt);
};
			
//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
bool AgcuCamSpringForce::vOptFlagChk(eFlagOption eopt)const
{
	return ( DEF_FLAG_CHK(m_ulOptionFlag, eopt) ? true : false );
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::vUdtMaskOn(eMaskUpdate eudt)
{
	DEF_FLAG_ON(m_ulUpdateMask, eudt);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamSpringForce::vUdtMaskOff(eMaskUpdate eudt)
{
	DEF_FLAG_OFF(m_ulUpdateMask, eudt);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
bool AgcuCamSpringForce::vUdtMaskChk(eMaskUpdate eudt)const
{
	return ( DEF_FLAG_CHK(m_ulUpdateMask, eudt) ? true : false );
};
