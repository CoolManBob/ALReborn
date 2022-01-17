// XTPGI_Tail.cpp: implementation of the CXTPGI_Tail class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_Tail.h"

#include "AgcdEffTail.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CXTPGI_Tail::CXTPGI_Tail(CString strCaption, AgcdEffTail* pTail)
: CXTPGI_EffRenderBase(strCaption, pTail)
, m_pTail(pTail)
, m_pItemMax(NULL)
, m_pItemGap(NULL)
, m_pItemPointLife(NULL)
, m_pItemHeight1(NULL)
, m_pItemHeight2(NULL)
{
	ASSERT(m_pTail);
}

void CXTPGI_Tail::OnAddChildItem()
{
	CXTPGI_EffRenderBase::OnAddChildItem();

	ASSERT(m_pTail);

	m_pItemMax		= (CXTPPropertyGridItemNumber*)AddChildItem(new CXTPPropertyGridItemNumber(_T("max"), m_pTail->m_nMaxNum, (LONG*)&m_pTail->m_nMaxNum));
	m_pItemGap		= (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("gap"), &m_pTail->m_dwTimeGap, m_pTail->m_dwTimeGap));
	m_pItemPointLife= (CXTPGI_UINT*)AddChildItem(new CXTPGI_UINT(_T("point life"), &m_pTail->m_dwPointLife, m_pTail->m_dwPointLife));
	m_pItemHeight1	= (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("height1"), &m_pTail->m_fHeight1,m_pTail->m_fHeight1));
	m_pItemHeight2	= (CXTPGI_Float*)AddChildItem(new CXTPGI_Float(_T("height1"), &m_pTail->m_fHeight2,m_pTail->m_fHeight2));

	//flags
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pTail->m_dwBitFlags, FLAG_EFFBASETAIL_Y45		,_T("rot 45" )));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pTail->m_dwBitFlags, FLAG_EFFBASETAIL_Y90		,_T("rot 90" )));
	(CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pTail->m_dwBitFlags, FLAG_EFFBASETAIL_Y135	,_T("rot 135")));
};
