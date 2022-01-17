// AgcuCamDampingForce.cpp: implementation of the AgcuCamDampingForce class.
//
//////////////////////////////////////////////////////////////////////
#include "AgcuEffUtil.h"
#include "AgcuCamDampingForce.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AgcuCamDampingForce::AgcuCamDampingForce()
{
	const	RwReal	KDROTROTX		= 10.f;
	const	RwReal	KDROTROTY		= 12.f;
	const	RwReal	KDZOOM			= 10.f;
	const	RwReal	LIMITROTX		= 660.f;
	const	RwReal	LIMITROTY		= 660.f;
	const	RwReal	LIMITZOOM		= 8000.f;

	m_ulUpdateMask		= 0LU;

	m_v2Omega.x			= 
	m_v2Omega.y			= 0.f;
	m_v2OmegaLimit.x	= LIMITROTX;
	m_v2OmegaLimit.y	= LIMITROTY;
	m_v2KdRot.x			= KDROTROTX;
	m_v2KdRot.y			= KDROTROTY;

	m_zoomspeed			= 0.f;
	m_LimitZoomSpeed	= LIMITZOOM;
	m_KdZoom			= KDZOOM;
}
AgcuCamDampingForce::~AgcuCamDampingForce()
{
}
AgcuCamDampingForce& AgcuCamDampingForce::bGetInst()
{
	static AgcuCamDampingForce inst;
	return inst;
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::bAddOmega(RwReal wx, RwReal wy)
{
	if( wx != 0.f )
	{
		bAddOmegaX(wx);
	}
	
	if( wy != 0.f )
	{
		bAddOmegaY(wy);
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::bAddOmegaX(RwReal wx)
{
	m_v2Omega.x	+= wx;
	if( fabsf(m_v2Omega.x) < 0.01f )
	{
		vUdtMaskOff(e_update_rotcx);
		m_v2Omega.x = 0.f;
	}
	else
	{
		vUdtMaskOn(e_update_rotcx);
		if( m_v2Omega.x < -m_v2OmegaLimit.x )
			m_v2Omega.x = -m_v2OmegaLimit.x;
		else if( m_v2Omega.x > m_v2OmegaLimit.x )
			m_v2Omega.x = m_v2OmegaLimit.x;
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::bAddOmegaY(RwReal wy)
{
	m_v2Omega.y	+= wy;
	if( fabsf(m_v2Omega.y) < 0.01f )
	{
		vUdtMaskOff(e_update_rotcy);
		m_v2Omega.y = 0.f;
	}
	else
	{
		vUdtMaskOn(e_update_rotcy);
		if( m_v2Omega.y < -m_v2OmegaLimit.y )
			m_v2Omega.y = -m_v2OmegaLimit.y;
		else if( m_v2Omega.y > m_v2OmegaLimit.y )
			m_v2Omega.y = m_v2OmegaLimit.y;
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::bAddZoomSpeed(RwReal ds)
{
	m_zoomspeed += ds;
	if( fabsf(m_zoomspeed) < 0.01f )
	{
		vUdtMaskOff(e_update_zoom);
		m_zoomspeed = 0.f;
	}
	else
	{
		vUdtMaskOn(e_update_zoom);
		if( m_zoomspeed < -m_LimitZoomSpeed )
			m_zoomspeed = -m_LimitZoomSpeed;
		else if( m_zoomspeed > m_LimitZoomSpeed )
			m_zoomspeed = m_LimitZoomSpeed;
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::bOnIdle(AgcmCamera2* pAgcmCamera2, RwReal fElasped)
{
	PROFILE("AgcuCamDampingForce::bOnIdle");

	if( fElasped < 0.0001f )
		return;

	vOnIdleOmegaX(pAgcmCamera2, fElasped);
	vOnIdleOmegaY(pAgcmCamera2, fElasped);
	vOnIdleZoom(pAgcmCamera2, fElasped);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::vUdtMaskOn(eMaskUpdate eudt)
{
	m_ulUpdateMask |= eudt;
};
void AgcuCamDampingForce::vUdtMaskOff(eMaskUpdate eudt)
{
	m_ulUpdateMask &= (~eudt);
};
bool AgcuCamDampingForce::vUdtMaskChk(eMaskUpdate eudt)const
{
	return ((m_ulUpdateMask & eudt) ? true : false);
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::vOnIdleOmegaX(AgcmCamera2* pAgcmCamera2, RwReal fElasped)
{
	if( vUdtMaskChk(e_update_rotcx) )
	{
		if( fabsf(m_v2Omega.x) < 0.0001f )
		{
			vUdtMaskOff(e_update_rotcx);
			m_v2Omega.x = 0.f;
			return;
		}

		pAgcmCamera2->vUdtMaskOn(AgcmCamera2::e_mask_pitch);
		RwReal	fPitch = m_v2Omega.x * fElasped;		
		pAgcmCamera2->m_fCPitch += fPitch;
		pAgcmCamera2->vAvailablePitch( pAgcmCamera2->m_fCPitch );
		RwReal	coef = m_v2KdRot.x * fElasped;
		if( coef >= 1.f )
		{
			vUdtMaskOff(e_update_rotcx);
			m_v2Omega.x = 0.f;
		}
		else
			m_v2Omega.x -= coef * m_v2Omega.x;
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::vOnIdleOmegaY(AgcmCamera2* pAgcmCamera2, RwReal fElasped)
{
	if( vUdtMaskChk(e_update_rotcy) )
	{
		if( fabsf(m_v2Omega.y) < 0.0001f )
		{
			vUdtMaskOff(e_update_rotcy);
			m_v2Omega.y = 0.f;
			return;
		}

		pAgcmCamera2->vUdtMaskOn(AgcmCamera2::e_mask_yaw);
		pAgcmCamera2->m_fWYaw += m_v2Omega.y * fElasped;
		RwReal	coef = m_v2KdRot.y * fElasped;
		if( coef >= 1.f )
		{
			vUdtMaskOff(e_update_rotcy);
			m_v2Omega.y = 0.f;
		}
		else
			m_v2Omega.y -= coef * m_v2Omega.y;
	}
};

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////
void AgcuCamDampingForce::vOnIdleZoom(AgcmCamera2* pAgcmCamera2, RwReal fElasped)
{
	if( vUdtMaskChk(e_update_zoom) )
	{
		if( fabsf(m_zoomspeed) < 0.0001f )
		{
			vUdtMaskOff(e_update_zoom);
			m_zoomspeed = 0.f;
			return;
		}

		pAgcmCamera2->vUdtMaskOn(AgcmCamera2::e_mask_zoom);
		pAgcmCamera2->m_fZoom += m_zoomspeed * fElasped;
		pAgcmCamera2->vAvailableZoom(pAgcmCamera2->m_fZoom);

		RwReal	coef = m_KdZoom * fElasped;

		if( coef >= 1.f )
		{
			vUdtMaskOff(e_update_zoom);
			m_zoomspeed = 0.f;
		}
		else
			m_zoomspeed -= coef * m_zoomspeed;
	}
};