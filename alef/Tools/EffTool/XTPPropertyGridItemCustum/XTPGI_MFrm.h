// XTPGI_MFrm.h: interface for the CXTPGI_MFrm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_MFRM_H__7B65C6B0_10FD_4777_813D_519DB0B79CE3__INCLUDED_)
#define AFX_XTPGI_MFRM_H__7B65C6B0_10FD_4777_813D_519DB0B79CE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_EffBase.h"

#include "AgcdEffMFrm.h"

class CXTPGI_MFrm : public CXTPGI_EffBase  
{
	class CXTPGI_ShakeFrm;

public:
	CXTPGI_MFrm(CString strCaption, AgcdEffMFrm* pMFrm);
	virtual ~CXTPGI_MFrm()	{	}

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffMFrm*	m_pMFrm;

	CXTPGI_Enum<AgcdEffMFrm::E_MFRM_TYPE>*	m_pItemType;
	CXTPGI_ShakeFrm*						m_pItemShake;
};

#endif // !defined(AFX_XTPGI_MFRM_H__7B65C6B0_10FD_4777_813D_519DB0B79CE3__INCLUDED_)
