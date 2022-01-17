// XTPGI_PSysSBH.cpp: implementation of the CXTPGI_PSysSBH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_PSysSBH.h"

#include "AgcdEffParticleSystem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXTPGI_PSysSBH::CXTPGI_PSysSBH(CString strCaption, AgcdEffParticleSys_SimpleBlackHole* pPSysSBH)
	: CXTPGI_EffRenderBase(strCaption, pPSysSBH)
	, m_pPSysSBH(pPSysSBH)

	, m_pItemCapacity(NULL)
	, m_pItemNumOfOneShoot(NULL)
	, m_pItemNumOfOneShootOffset(NULL)
	, m_pItemShootDelay(NULL)
	, m_pItemPLife(NULL)
	, m_pItemSpeed(NULL)
	, m_pItemSpeedOffset(NULL)
	, m_pItemRollMin(NULL)
	, m_pItemRollMax(NULL)
	, m_pItemRadius(NULL)

	, m_pItemBool_RANDCOLR		(NULL)
	, m_pItemBool_INTERPOLATION	(NULL)
	, m_pItemBool_CHILDDEPENDANCY(NULL)
	, m_pItemBool_INVS			(NULL)
	, m_pItemBool_REGULARANGLE	(NULL)
	, m_pItemBool_CAPACITYLIMIT	(NULL)
	, m_pItemBool_OLDTYPE		(NULL)
	, m_pItemBool_CIRCLEEMITER	(NULL)
	, m_pItemBool_FILLCIRCLE	(NULL)
	, m_pItemBool_MINANGLE		(NULL)
{
	ASSERT(m_pPSysSBH);
};

CXTPGI_PSysSBH::~CXTPGI_PSysSBH()
{
};

void CXTPGI_PSysSBH::OnAddChildItem()
{
	ASSERT(m_pPSysSBH);

	CXTPGI_EffRenderBase::OnAddChildItem();

	m_pItemCapacity = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("capacity"), m_pPSysSBH->m_nCapacity, (LONG*)&m_pPSysSBH->m_nCapacity));
	m_pItemNumOfOneShoot = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("num of one-shoot"), m_pPSysSBH->m_nNumOfOneShoot, (LONG*)&m_pPSysSBH->m_nNumOfOneShoot));
	m_pItemNumOfOneShootOffset = (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("num of one-shoot offset"), m_pPSysSBH->m_nNumOfOneShootOffset, (LONG*)&m_pPSysSBH->m_nNumOfOneShootOffset));
	m_pItemShootDelay = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("shoot delay"), &m_pPSysSBH->m_dwTimeGap, m_pPSysSBH->m_dwTimeGap));
	m_pItemPLife = (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("particle life"), &m_pPSysSBH->m_dwParticleLife, m_pPSysSBH->m_dwParticleLife));
	m_pItemSpeed = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("partilce speed"), &m_pPSysSBH->m_fInitSpeed,m_pPSysSBH->m_fInitSpeed));
	m_pItemSpeedOffset = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("partilce speed offset"), &m_pPSysSBH->m_fInitSpeedOffset,m_pPSysSBH->m_fInitSpeedOffset));
	m_pItemRollMin = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("angel min"), &m_pPSysSBH->m_fRollMin,m_pPSysSBH->m_fRollMin));
	m_pItemRollMax = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("angel max"), &m_pPSysSBH->m_fRollMax,m_pPSysSBH->m_fRollMax));
	m_pItemRadius = (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T(""), &m_pPSysSBH->m_fRadius, m_pPSysSBH->m_fRadius));

	//flags
	m_pItemBool_RANDCOLR		= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_RANDCOLR		,_T("RANDCOLR")));
	m_pItemBool_INTERPOLATION	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_INTERPOLATION	,_T("INTERPOLATION")));
	m_pItemBool_CHILDDEPENDANCY	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_CHILDDEPENDANCY	,_T("CHILDDEPENDANCY")));
	m_pItemBool_INVS			= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_INVS			,_T("INVS")));
	m_pItemBool_REGULARANGLE	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_REGULARANGLE	,_T("REGULARANGLE")));
	m_pItemBool_CAPACITYLIMIT	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_CAPACITYLIMIT	,_T("CAPACITYLIMIT")));
	m_pItemBool_OLDTYPE			= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_OLDTYPE			,_T("OLDTYPE")));
	m_pItemBool_CIRCLEEMITER	= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_CIRCLEEMITER	,_T("CIRCLEEMITER")));
	m_pItemBool_FILLCIRCLE		= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_FILLCIRCLE		,_T("FILLCIRCLE")));
	m_pItemBool_MINANGLE		= (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pPSysSBH->m_dwBitFlags, FLAG_EFFBASEPSYSSBH_MINANGLE		,_T("MINANGLE")));
};
