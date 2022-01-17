// XTPGI_PSysSBH.h: interface for the CXTPGI_PSysSBH class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_PSYSSBH_H__2824A957_3724_4F49_8E58_5371FF1A44DD__INCLUDED_)
#define AFX_XTPGI_PSYSSBH_H__2824A957_3724_4F49_8E58_5371FF1A44DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_EffBase.h"

class AgcdEffParticleSys_SimpleBlackHole;
class CXTPGI_PSysSBH : public CXTPGI_EffRenderBase  
{
public:
	CXTPGI_PSysSBH(CString strCaption, AgcdEffParticleSys_SimpleBlackHole* pPSysSBH);
	virtual ~CXTPGI_PSysSBH();

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffParticleSys_SimpleBlackHole*	m_pPSysSBH;

	CXTPPropertyGridItemNumber*	m_pItemCapacity;
	CXTPPropertyGridItemNumber*	m_pItemNumOfOneShoot;
	CXTPPropertyGridItemNumber*	m_pItemNumOfOneShootOffset;
	CXTPGI_UINT*				m_pItemShootDelay;
	CXTPGI_UINT*				m_pItemPLife;
	CXTPGI_Float*				m_pItemSpeed;
	CXTPGI_Float*				m_pItemSpeedOffset;
	CXTPGI_Float*				m_pItemRollMin;
	CXTPGI_Float*				m_pItemRollMax;
	CXTPGI_Float*				m_pItemRadius;

	//flags
	CXTPGI_FlagBool*			m_pItemBool_RANDCOLR;
	CXTPGI_FlagBool*			m_pItemBool_INTERPOLATION;
	CXTPGI_FlagBool*			m_pItemBool_CHILDDEPENDANCY;
	CXTPGI_FlagBool*			m_pItemBool_INVS;
	CXTPGI_FlagBool*			m_pItemBool_REGULARANGLE;
	CXTPGI_FlagBool*			m_pItemBool_CAPACITYLIMIT;
	CXTPGI_FlagBool*			m_pItemBool_OLDTYPE;
	CXTPGI_FlagBool*			m_pItemBool_CIRCLEEMITER;
	CXTPGI_FlagBool*			m_pItemBool_FILLCIRCLE;
	CXTPGI_FlagBool*			m_pItemBool_MINANGLE;
};

#endif // !defined(AFX_XTPGI_PSYSSBH_H__2824A957_3724_4F49_8E58_5371FF1A44DD__INCLUDED_)
