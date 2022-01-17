//	AgpaEventBinding.cpp
//////////////////////////////////////////////////////////////////////

#include "AgpaEventBinding.h"

AgpaEventBinding::AgpaEventBinding()
{
}

AgpaEventBinding::~AgpaEventBinding()
{
}

AgpdBinding* AgpaEventBinding::AddBinding(AgpdBinding *pcsBinding, INT32 lID, CHAR *szBindingName)
{
	if (AddObject((PVOID *) &pcsBinding, lID, szBindingName))
		return pcsBinding;

	return NULL;
}

AgpdBinding* AgpaEventBinding::GetBinding(CHAR *szBindingName)
{
	AgpdBinding **ppBinding = (AgpdBinding **) GetObject(szBindingName);

	if (ppBinding)
		return *ppBinding;

	return NULL;
}

AgpdBinding* AgpaEventBinding::GetBinding(INT32 lID)
{
	AgpdBinding **ppBinding = (AgpdBinding **) GetObject(lID);

	if (ppBinding)
		return *ppBinding;

	return NULL;
}

BOOL AgpaEventBinding::RemoveBinding(INT32 lID, CHAR *szBindingName)
{
	return RemoveObject(lID, szBindingName);
}