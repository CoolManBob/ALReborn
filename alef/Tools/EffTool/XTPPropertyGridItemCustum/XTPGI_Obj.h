// XTPGI_Obj.h: interface for the CXTPGI_Obj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_OBJ_H__F2552EA9_7B24_48D7_ADF0_670DDFBF78B6__INCLUDED_)
#define AFX_XTPGI_OBJ_H__F2552EA9_7B24_48D7_ADF0_670DDFBF78B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_EffBase.h"

class AgcdEffObj;
class CXTPGI_Obj : public CXTPGI_EffRenderBase  
{
public:
	CXTPGI_Obj(CString strCaption, AgcdEffObj* pObj);
	virtual ~CXTPGI_Obj()	{	}

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffObj*				m_pObj;

	CXTPGI_FileName*		m_pItemClump;
	CXTPGI_RwRGBA*			m_pItemPreLit;
	
	CXTPGI_FlagBool*		m_pItemBool_Dummy;
	CXTPGI_FlagBool*		m_pItemBool_Expand;
};

#endif // !defined(AFX_XTPGI_OBJ_H__F2552EA9_7B24_48D7_ADF0_670DDFBF78B6__INCLUDED_)
