// XTPGI_MFrm.cpp: implementation of the CXTPGI_MFrm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_MFrm.h"

#include "AgcdEffMFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//----------------- CXTPGI_MFrm::CXTPGI_ShakeFrm -------------------------
class CXTPGI_MFrm::CXTPGI_ShakeFrm : public CXTPPropertyGridItem
{
	AgcdEffMFrm::STSHAKEFRM*	m_pShakeFrm;

public:
	CXTPGI_ShakeFrm(AgcdEffMFrm::STSHAKEFRM* pShakeFrm) : CXTPPropertyGridItem(_T("shake frame"))
	,m_pShakeFrm(pShakeFrm)
	{
		ASSERT(m_pShakeFrm);
	};
	virtual ~CXTPGI_ShakeFrm()	{	};

protected:
	virtual void OnAddChildItem()
	{
		ASSERT(m_pShakeFrm);

		static LPCTSTR	strAxis[AgcdEffMFrm::EOSCILLATIONAXIS_NUM] = {
			"LOCAL_X", "LOCAL_Y", "LOCAL_Z",
			"WORLD_X", "WORLD_Y", "WORLD_Z",
			"CAMERA_X", "CAMERA_Y", "CAMERA_Z",
			"RANDOM",
		};

		AddChildItem( new CXTPGI_Enum<AgcdEffMFrm::eOscillationAxis>(
			_T("axis"), AgcdEffMFrm::EOSCILLATIONAXIS_NUM, m_pShakeFrm->m_eAxis, &m_pShakeFrm->m_eAxis, strAxis ) );
			
		LPCTSTR strWhose[AgcdEffMFrm::EFRM_NUM] = { "CAM", "SRC", "DST", };
		AddChildItem(new CXTPGI_Enum<AgcdEffMFrm::eWhoseFrame>(
			_T("whose"), AgcdEffMFrm::EFRM_NUM, m_pShakeFrm->m_eWhose, &m_pShakeFrm->m_eWhose, strWhose ) );

		AddChildItem(new CXTPGI_Float(_T("amplitude"), &m_pShakeFrm->m_fAmplitude,m_pShakeFrm->m_fAmplitude));
		AddChildItem(new CXTPGI_UINT (_T("duration"), &m_pShakeFrm->m_dwDuration,m_pShakeFrm->m_dwDuration));
		AddChildItem(new CXTPGI_Float(_T("cycle"), &m_pShakeFrm->m_fTotalCycle,m_pShakeFrm->m_fTotalCycle));
		AddChildItem(new CXTPGI_Float(_T("speed"), &m_pShakeFrm->m_fSpeed,m_pShakeFrm->m_fSpeed));
	}
};

//----------------- CXTPGI_MFrm -------------------------
CXTPGI_MFrm::CXTPGI_MFrm(CString strCaption, AgcdEffMFrm* pMFrm) : CXTPGI_EffBase(strCaption, pMFrm)
 ,m_pMFrm(pMFrm)
 ,m_pItemType(NULL)
 ,m_pItemShake(NULL)
{
	ASSERT(m_pMFrm);
}

void CXTPGI_MFrm::OnAddChildItem()
{
	CXTPGI_EffBase::OnAddChildItem();

	static LPCTSTR	strType[AgcdEffMFrm::EMFRM_NUM] = { "SHAKE" "SPLINE" };
	m_pItemType = (CXTPGI_Enum<AgcdEffMFrm::E_MFRM_TYPE>*)AddChildItem(new CXTPGI_Enum<AgcdEffMFrm::E_MFRM_TYPE>
		( _T(""), AgcdEffMFrm::EMFRM_NUM, m_pMFrm->m_eMFrmType, &m_pMFrm->m_eMFrmType ) );
	m_pItemShake = (CXTPGI_ShakeFrm*)AddChildItem(new CXTPGI_ShakeFrm(&m_pMFrm->m_stShakeFrm));
};
