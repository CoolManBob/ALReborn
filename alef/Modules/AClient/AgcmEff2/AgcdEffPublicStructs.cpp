// AgcdEffPublicStructs.cpp: implementation of the AgcdEffPublicStructs class.
//
//////////////////////////////////////////////////////////////////////

#include "AgcdEffPublicStructs.h"
#include "AcDefine.h"
#include "ApMemoryTracker.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
stEffSetCallBackInfo::stEffSetCallBackInfo()
: m_bCallCB(TRUE)
, m_pBase(NULL)
, m_pfNoticeEffectProcessCB(NULL)
, m_pNoticeCBClass(NULL)
{
}

stTailInfo::stTailInfo()
: m_eType(e_heightbase)
{
	m_stNodeInfo.m_pFrmNode1 = NULL;
	m_stNodeInfo.m_pFrmNode2 = NULL;
}

stMissileTargetInfo::stMissileTargetInfo()
: m_pFrmTarget(NULL)
{
	m_v3dCenter.x	= 0.f;
	m_v3dCenter.y	= 0.f;
	m_v3dCenter.z	= 0.f;

	m_nRadius		=	0;
	m_nZigzagType	=	0;
	m_nRotateSpeed	=	0;
}

stEffUseInfo::stEffUseInfo()
: m_ulEffID(0LU)
, m_fScale(1.f)
, m_fParticleNumScale(1.f)
, m_fTimeScale(1.f)
, m_pFrmParent(NULL)
, m_pFrmTarget(NULL)
, m_pHierarchy(NULL)
, m_pClumpParent(NULL)
, m_pClumpEmiter(NULL)
, m_ulDelay(0LU)
, m_ulLife(-1)
, m_ulFlags(E_FLAG_NOSCALE)
, m_pBase(NULL)
, m_lOwnerCID(0)
, m_lTargetCID(0)
, m_lCustData(0)
, m_pNoticeCBClass(NULL)
, m_fptrNoticeCB(NULL)
, m_pcsData(NULL)
{
	m_rgbScale.red		= 255;
	m_rgbScale.green	= 255;
	m_rgbScale.blue		= 255;
	m_rgbScale.alpha	= 255;

	m_v3dCenter.x = 
	m_v3dCenter.y = 
	m_v3dCenter.z = 0.f;

	//@{ burumal 2006/01/18
	RtQuatInit(&m_quatRotation, 0, 0, 0, 1);
	//@}
}
