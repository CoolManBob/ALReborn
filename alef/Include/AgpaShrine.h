/******************************************************************************
Module:  AgpaShrine.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 22
******************************************************************************/

#if !defined(__AGPASHRINE_H__)
#define __AGPASHRINE_H__

#include "ApBase.h"
#include "ApAdmin.h"
#include "AgpdShrine.h"

class AgpaShrine : public ApAdmin
{
public:
	AgpaShrine();
	~AgpaShrine();

	AgpdShrine*	AddShrine(AgpdShrine *pcsShrine);
	AgpdShrine*	GetShrine(INT32 lShrineID);
	BOOL		RemoveShrine(INT32 lShrineID);
};

#endif //__AGPASHRINE_H__
