/******************************************************************************
Module:  AgpaShrineTemplate.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 22
******************************************************************************/

#if !defined(__AGPASHRINETEMPLATE_H__)
#define __AGPASHRINETEMPLATE_H__

#include "ApBase.h"
#include "ApAdmin.h"
#include "AgpdShrine.h"

class AgpaShrineTemplate : public ApAdmin
{
public:
	AgpaShrineTemplate();
	~AgpaShrineTemplate();

	AgpdShrineTemplate*	AddTemplate(AgpdShrineTemplate *pcsTemplate);
	AgpdShrineTemplate*	GetTemplate(INT32 lTID);
	AgpdShrineTemplate*	GetTemplate(CHAR *szName);
	BOOL				RemoveTemplate(INT32 lTID);

	BOOL				AddTemplateStringKey(INT32 lTID, CHAR *szTName);
};

#endif //__AGPASHRINETEMPLATE_H__
