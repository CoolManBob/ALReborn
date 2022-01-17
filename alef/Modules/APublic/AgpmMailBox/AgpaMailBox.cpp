/*============================================================

	AgpaMailBox.cpp
	
============================================================*/


#include "AgpaMailBox.h"


/****************************************************/
/*		The Implementation of AgpaMailBox class		*/
/****************************************************/
//
AgpaMailBox::AgpaMailBox()
	{
	}

	
AgpaMailBox::~AgpaMailBox()
	{
	}
	

BOOL AgpaMailBox::Add(AgpdMail *pAgpdMail)
	{
	if (!pAgpdMail || !AddObject((PVOID) &pAgpdMail, pAgpdMail->m_lID))
		return FALSE;

	return TRUE;		
	}

	
BOOL AgpaMailBox::Remove(AgpdMail *pAgpdMail)
	{
	if (!pAgpdMail)
		return FALSE;

	return RemoveObject(pAgpdMail->m_lID);
	}


BOOL AgpaMailBox::Remove(INT32 lID)
	{
	return RemoveObject(lID);
	}


AgpdMail* AgpaMailBox::Get(INT32 lID)
	{
	AgpdMail **ppAgpdMail = (AgpdMail **) GetObject(lID);
	if (!ppAgpdMail)
		return NULL;	

	return *ppAgpdMail;	
	}

