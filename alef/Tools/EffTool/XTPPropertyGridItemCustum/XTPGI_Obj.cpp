// XTPGI_Obj.cpp: implementation of the CXTPGI_Obj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../EffTool.h"
#include "XTPGI_Obj.h"
#include "../ToolOption.h"

#include "AgcdEffObj.h"
#include "AgcuEffPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CXTPGI_Obj::CXTPGI_Obj(CString strCaption, AgcdEffObj* pObj) : CXTPGI_EffRenderBase(strCaption, pObj)
 ,m_pObj(pObj)
 ,m_pItemClump(NULL)
 ,m_pItemPreLit(NULL)
{
	ASSERT(m_pObj);
}

void CXTPGI_Obj::OnAddChildItem()
{
	ASSERT(m_pObj);

	CXTPGI_EffRenderBase::OnAddChildItem();
	
	CXTPGI_FileName*		m_pItemClump;
	CXTPGI_RwRGBA*			m_pItemPreLit;
	//flag
	CXTPGI_FlagBool*		m_pItemBool_Dummy;
	CXTPGI_FlagBool*		m_pItemBool_Expand;
	
	LPCTSTR	strFilter = "(*.rws)|*.rws|";
	m_pItemClump = (CXTPGI_FileName*)AddChildItem(
		new CXTPGI_FileName(_T("clump")
		, EFF2_FILE_NAME_MAX
		, m_pObj->m_szClumpFName
		, m_pObj->m_szClumpFName
		, AgcuEffPath::bGetPath_Clump()
		, strFilter ) );
	NSUtFn::CSelectedInfo::bGetInst().bOnAddCTXPGI_Clump( m_pItemClump );

	m_pItemPreLit = (CXTPGI_RwRGBA*)AddChildItem(
		new CXTPGI_RwRGBA(_T("preLit"), &m_pObj->m_rgbaPreLit, m_pObj->m_rgbaPreLit ));

	m_pItemBool_Dummy = (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pObj->m_dwBitFlags, FLAG_EFFBASEOBJ_DUMMY, _T("dummy")));
	m_pItemBool_Expand = (CXTPGI_FlagBool*)AddChildItem(new CXTPGI_FlagBool(&m_pObj->m_dwBitFlags, FLAG_EFFBASEOBJ_CHILDOFMISSILE, _T("expand child of missile")));
};
