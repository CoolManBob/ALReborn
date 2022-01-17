// XTPGI_Anim.cpp: implementation of the CXTPGI_Anim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_Anim.h"

#include "XTPGI_UINT.h"
#include "XTPPropertyGridItemEnum.h"

#include "AgcuEffAnimUtil.h"
#include "AgcdEffAnim.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------- CXTPGI_SplineLife ----------------------
class CXTPGI_SplineLife : public CXTPGI_UINT
{
public:
	CXTPGI_SplineLife(CString strCaption, AgcdEffAnim_RpSpline* pEffSpline)
	 : CXTPGI_UINT( strCaption, &pEffSpline->m_dwLifeTime, pEffSpline->m_dwLifeTime)
	 ,m_pEffSpline(pEffSpline)
	{
		ASSERT( m_pEffSpline );
	}

protected:
	virtual void OnValueChanged(CString strVal)
	{
		ASSERT( m_pEffSpline );
		SetValue(strVal);
		m_pEffSpline->bSetLife( *m_pVal );
	}

private:
	AgcdEffAnim_RpSpline*	m_pEffSpline;
};

//------------------------- CXTPGI_Anim ----------------------
CXTPGI_Anim::CXTPGI_Anim(CString strCaption, AgcdEffAnim* pAnim) : CXTPPropertyGridItem(strCaption)
 ,m_pAnim(pAnim)
{
	ASSERT(pAnim);
}

CXTPGI_Anim::~CXTPGI_Anim()
{
}

void CXTPGI_Anim::OnAddChildItem()
{
	ASSERT(m_pAnim);

	CXTPPropertyGridItem* pAnim = AddChildItem(new CXTPPropertyGridItem(_T("-- AgcdEffAnim --"), _T("category")));
	pAnim->SetReadOnly(TRUE);

	if( dynamic_cast<AgcdEffAnim_RpSpline*>(m_pAnim) )
		pAnim->AddChildItem( new CXTPGI_SplineLife(_T("life"), dynamic_cast<AgcdEffAnim_RpSpline*>(m_pAnim)));
	else
	{
		CXTPPropertyGridItem*	pLife =
		pAnim->AddChildItem(new CXTPGI_UINT(_T("life"), &m_pAnim->m_dwLifeTime, m_pAnim->m_dwLifeTime));
		pLife->SetReadOnly(TRUE);
	}

	extern const TCHAR*	SZELOOPOPT[e_TblDir_num];
	pAnim->AddChildItem( new CXTPGI_Enum<E_LOOPOPT>( _T("loop"), e_TblDir_num, m_pAnim->m_eLoopOpt, &m_pAnim->m_eLoopOpt, SZELOOPOPT ) );
	pAnim->Expand();
}
