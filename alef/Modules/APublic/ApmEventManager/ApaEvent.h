// ApaEvent.h: interface for the ApaEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APAEVENT_H__272FC9D6_1B20_4B6D_9C20_393EFE9799B4__INCLUDED_)
#define AFX_APAEVENT_H__272FC9D6_1B20_4B6D_9C20_393EFE9799B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApAdmin.h"

#include "ApdEvent.h"


class ApaEvent : public ApAdmin  
{
public:
	ApaEvent();
	virtual ~ApaEvent();

	ApdEvent *	AddEvent(ApdEvent* pcsEvent, INT32 lEID);
	ApdEvent *	GetEvent(INT32 lEID);
	BOOL		RemoveEvent(INT32 lEID);

	ApdEvent * GetSequence(INT32 *plIndex);
};

#endif // !defined(AFX_APAEVENT_H__272FC9D6_1B20_4B6D_9C20_393EFE9799B4__INCLUDED_)
