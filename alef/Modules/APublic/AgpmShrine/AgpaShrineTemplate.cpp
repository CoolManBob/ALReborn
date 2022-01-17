/******************************************************************************
Module:  AgpaShrineTemplate.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 22
******************************************************************************/

#include "AgpaShrineTemplate.h"

AgpaShrineTemplate::AgpaShrineTemplate()
{
}

AgpaShrineTemplate::~AgpaShrineTemplate()
{
}

AgpdShrineTemplate* AgpaShrineTemplate::AddTemplate(AgpdShrineTemplate *pcsTemplate)
{
	if (AddObject( (PVOID*) &pcsTemplate, pcsTemplate->m_lID ))
		return pcsTemplate;

	return NULL;
}

BOOL AgpaShrineTemplate::RemoveTemplate(INT32 lTID)
{
	return RemoveObject(lTID);
}

AgpdShrineTemplate* AgpaShrineTemplate::GetTemplate(INT32 lTID)
{
	AgpdShrineTemplate **pvRetVal = (AgpdShrineTemplate **) GetObject( lTID );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

AgpdShrineTemplate* AgpaShrineTemplate::GetTemplate(CHAR *szName)
{
	AgpdShrineTemplate **pvRetVal = (AgpdShrineTemplate **) GetObject( szName );

	if (pvRetVal)
		return *pvRetVal;

	return NULL;
}

BOOL	AgpaShrineTemplate::AddTemplateStringKey(INT32 lTID, CHAR *szTName)
{
	return AddStringKey(lTID, szTName);
}
