/******************************************************************************
Module:  ApdItemTemplate.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 15
******************************************************************************/

#if !defined(__APDITEMTEMPLATE_H__)
#define __APDITEMTEMPLATE_H__

#include "ApBase.h"

//class ApdItemTemplate : public ApBase {
class ApdItemTemplate : public ApBase
{
public:
	BOOL			m_bStackable;
	INT32			m_lMaxStackableCount;
};

#endif //__APDITEMTEMPLATE_H__
