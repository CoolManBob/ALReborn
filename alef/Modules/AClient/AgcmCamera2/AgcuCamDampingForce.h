// AgcuCamDampingForce.h: interface for the AgcuCamDampingForce class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCUDAMPINGFORCE_H__46E21A2C_4244_40AF_8BA9_E177163F19CB__INCLUDED_)
#define AFX_AGCUDAMPINGFORCE_H__46E21A2C_4244_40AF_8BA9_E177163F19CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcmCamera2.h"

class AgcuCamDampingForce  
{
	enum	eMaskUpdate
	{
		e_update_rotcx	= 0x00000001,
		e_update_rotcy	= 0x00000002,
		e_update_zoom	= 0x00000004,
	};
public:
	//rot
	void		bAddOmega(RwReal wx, RwReal wy);
	void		bAddOmegaX(RwReal wx);
	void		bAddOmegaY(RwReal wy);
	//zoom
	void		bAddZoomSpeed(RwReal ds);

	//idle
	void		bOnIdle(AgcmCamera2* pAgcmCamera2, RwReal fElasped);
	
private:
	//updatemask
	void		vUdtMaskOn(eMaskUpdate eudt);
	void		vUdtMaskOff(eMaskUpdate eudt);
	bool		vUdtMaskChk(eMaskUpdate eudt)const;

	//each idle
	void		vOnIdleOmegaX(AgcmCamera2* pAgcmCamera2, RwReal fElasped);
	void		vOnIdleOmegaY(AgcmCamera2* pAgcmCamera2, RwReal fElasped);
	void		vOnIdleZoom(AgcmCamera2* pAgcmCamera2, RwReal fElasped);
private:
	//update flag
	RwUInt32	m_ulUpdateMask;
	//rot
	RwV2d		m_v2Omega;
	RwV2d		m_v2OmegaLimit;
	RwV2d		m_v2KdRot;			//rotation damping coefficient
	//zoom
	RwReal		m_zoomspeed;
	RwReal		m_LimitZoomSpeed;
	RwReal		m_KdZoom;			//zoom damping coefficient

	AgcuCamDampingForce();
public:
	virtual ~AgcuCamDampingForce();

	static AgcuCamDampingForce& bGetInst();

	friend class AgcmCamera2;
};

#endif // !defined(AFX_AGCUDAMPINGFORCE_H__46E21A2C_4244_40AF_8BA9_E177163F19CB__INCLUDED_)
