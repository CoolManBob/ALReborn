// AgcuCamSpringForce.h: interface for the AgcuCamSpringForce class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCUCAMSPRINGFORCE_H__18437D7C_5E11_4714_BE82_F3B64B0253E6__INCLUDED_)
#define AFX_AGCUCAMSPRINGFORCE_H__18437D7C_5E11_4714_BE82_F3B64B0253E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AgcmCamera2.h"

class AgcuCamSpringForce  
{
public:
	enum	eFlagOption
	{
		e_option_use_2ndLinearSpring	= 0x00000001,
		e_option_alwayslookattarget		= 0x00000002,
	};
	enum	eMaskUpdate
	{
		e_update_linear		= 0x00000001,	//linear spring
		e_update_spiral		= 0x00000002,	//wired spring
		e_update_2ndLinear	= 0x00000004,	//follow charac
	};

public:
	//idle
	void		bOnReleaseLinearSpring(AgcmCamera2* pAgcmCamera2, RwReal fElapsed, RwReal fOffset);
	void		bOnReleaseSpiralSpring(AgcmCamera2* pAgcmCamera2, RwReal fElapsed, RwReal fOffset);
	void		bOnMoveCharac(AgcmCamera2* pAgcmCamera2);
	void		bOnIdle(AgcmCamera2* pAgcmCamera2, RwReal fElasped);

	//option
	void		bOptFlagOn(eFlagOption eopt);
	void		bOptFlagOff(eFlagOption eopt);
private:
	bool		vOptFlagChk(eFlagOption eopt)const;

private:
	//updatemask
	void		vUdtMaskOn(eMaskUpdate eudt);
	void		vUdtMaskOff(eMaskUpdate eudt);
	bool		vUdtMaskChk(eMaskUpdate eudt)const;

	void		vOnIdleExt(AgcmCamera2* pAgcmCamera2, RwReal fElasped);
//	void		vOnIdelLinear(AgcmCamera2* pAgcmCamera2, RwReal fElasped);
//	void		vOnIdelSpiral(AgcmCamera2* pAgcmCamera2, RwReal fElasped);

private:
	//option flag
	RwUInt32	m_ulOptionFlag;
	//update flag
	RwUInt32	m_ulUpdateMask;

	//2nd linear spring
	//follow charac
	RwV3d		m_v3DisiredEye;
	RwReal		m_fKsToCharac;
	RwReal		m_fKdToCharac;	//sqrtf(m_fKsToCharac)
	RwReal		m_fLimitSpringLen;	
	RwV3d		m_v3dPrevVel;

	//linear spring
	RwReal		m_fKsLinear;
	RwReal		m_fKdLinear;	//sqrtf(m_fKsLinear)
	RwReal		m_fPrevLinearSpeed;
	//spiral spring
	RwReal		m_fKsSpiral;
	RwReal		m_fKdSpiral;	//sqrtf(m_fKsSpiral)
	RwReal		m_fPrevAngularSpeed;

private:
	AgcuCamSpringForce();
public:
	virtual ~AgcuCamSpringForce();

	static AgcuCamSpringForce& bGetInst();

	friend class AgcmCamera2;
};

#endif // !defined(AFX_AGCUCAMSPRINGFORCE_H__18437D7C_5E11_4714_BE82_F3B64B0253E6__INCLUDED_)
