// AgsmDBPoolEvents.h: interface for the AgsmDBPoolEvents class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGSMDBPOOLEVENTS_H__F8CBF19E_38E1_49D8_A56F_C153F4380742__INCLUDED_)
#define AFX_AGSMDBPOOLEVENTS_H__F8CBF19E_38E1_49D8_A56F_C153F4380742__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"
#include <memory>

class AgsmDBPoolEvents  
{
private:
	HANDLE* m_pHandles;
	BOOL*	m_pOwner;
	INT32	m_lEventCount;
	
	ApMutualEx m_Muex;

public:
	AgsmDBPoolEvents();
	virtual ~AgsmDBPoolEvents();

	INT32 GetIndex();

	BOOL Create(INT32 lEventCount);
	BOOL Wait(DWORD dwTime = INFINITE);

	BOOL SetDBEvent(INT32 lIndex);
	BOOL ResetDBEvent(INT32 lIndex);
};

#endif // !defined(AFX_AGSMDBPOOLEVENTS_H__F8CBF19E_38E1_49D8_A56F_C153F4380742__INCLUDED_)
