// XTPGI_PSys.h: interface for the CXTPGI_PSys class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_PSYS_H__04E2817E_A017_454A_9477_4ED82A4A032C__INCLUDED_)
#define AFX_XTPGI_PSYS_H__04E2817E_A017_454A_9477_4ED82A4A032C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_EffBase.h"

//------------------ CXTPGI_CofEnvrn ----------------------
struct stCofEnvrnParam;
class CXTPGI_CofEnvrn : public CXTPPropertyGridItem
{
public:
	CXTPGI_CofEnvrn(CString strCaption, stCofEnvrnParam* pCofEnvrn);
	virtual ~CXTPGI_CofEnvrn()	{	}

protected:
	virtual void OnAddChildItem();

private:
	stCofEnvrnParam*	m_pCofEnvrn;

	CXTPGI_Float*		m_pItemCofGrav;
	CXTPGI_Float*		m_pItemCofAirResistance;
};

//------------------ CXTPGI_PSys ----------------------
class AgcdEffParticleSystem;
class CXTPGI_PSys : public CXTPGI_EffRenderBase  
{
	class CXTPGI_Emiter;
	class CXTPGI_PtcProp;

public:
	CXTPGI_PSys(CString strCaption, AgcdEffParticleSystem* pPSys);
	virtual ~CXTPGI_PSys();

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffParticleSystem*		m_pPSys;

	CXTPPropertyGridItemNumber*	m_pItemCapacity;
	CXTPGI_UINT*				m_pItemShootDelay;
	CXTPGI_FileName*			m_pItemClumpName;
	CXTPGI_Emiter*				m_pItemEmiter;
	CXTPGI_PtcProp*				m_pItemPtcProp;
};

#endif // !defined(AFX_XTPGI_PSYS_H__04E2817E_A017_454A_9477_4ED82A4A032C__INCLUDED_)
