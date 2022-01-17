// ApaEvent.cpp: implementation of the ApaEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "ApaEvent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ApaEvent::ApaEvent()
{

}

ApaEvent::~ApaEvent()
{

}

ApdEvent *	ApaEvent::AddEvent(ApdEvent *pcsEvent, INT32 lEID)
{
	if (AddObject((PVOID*) &pcsEvent, lEID))
		return pcsEvent;

	return NULL;
}

ApdEvent *	ApaEvent::GetEvent(INT32 lEID)
{
	ApdEvent **pvRetVal = (ApdEvent **) GetObject(lEID);

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

BOOL		ApaEvent::RemoveEvent(INT32 lEID)
{
	BOOL	bRemove = FALSE;

	bRemove = RemoveObject(lEID);

	return bRemove;
}

ApdEvent * ApaEvent::GetSequence(INT32 *plIndex)
{
	ApdEvent **ppcsEvent = (ApdEvent **)GetObjectSequence(plIndex);

	if (!ppcsEvent)
		return NULL;

	return  *ppcsEvent;
}
