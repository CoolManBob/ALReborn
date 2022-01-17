// XTPGI_Tail.h: interface for the CXTPGI_Tail class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTPGI_TAIL_H__6BF023B4_4572_4E05_92D3_4D3D9516960F__INCLUDED_)
#define AFX_XTPGI_TAIL_H__6BF023B4_4572_4E05_92D3_4D3D9516960F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPGI_EffBase.h"

class AgcdEffTail;
class CXTPGI_Tail : public CXTPGI_EffRenderBase  
{
public:
	CXTPGI_Tail(CString strCaption, AgcdEffTail* pTail);
	virtual ~CXTPGI_Tail()	{	}

protected:
	virtual void OnAddChildItem();

private:
	AgcdEffTail*	m_pTail;

	CXTPPropertyGridItemNumber*	m_pItemMax;
	CXTPGI_UINT*				m_pItemGap;
	CXTPGI_UINT*				m_pItemPointLife;
	CXTPGI_Float*				m_pItemHeight1;
	CXTPGI_Float*				m_pItemHeight2;
};

#endif // !defined(AFX_XTPGI_TAIL_H__6BF023B4_4572_4E05_92D3_4D3D9516960F__INCLUDED_)
