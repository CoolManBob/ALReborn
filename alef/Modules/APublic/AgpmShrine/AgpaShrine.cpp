/******************************************************************************
Module:  AgpaShrine.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 22
******************************************************************************/

#include "ApBase.h"
#include "AgpaShrine.h"

AgpaShrine::AgpaShrine()
{
}

AgpaShrine::~AgpaShrine()
{
}

AgpdShrine* AgpaShrine::AddShrine(AgpdShrine *pcsShrine)
{
	if (AddObject( (PVOID*) &pcsShrine, pcsShrine->m_lID ))
		return pcsShrine;

	return NULL;
}

BOOL AgpaShrine::RemoveShrine(INT32 lShrineID)
{
	return RemoveObject(lShrineID);
}

AgpdShrine* AgpaShrine::GetShrine(INT32 lShrineID)
{
	AgpdShrine **pvRetVal = (AgpdShrine **) GetObject( lShrineID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}
