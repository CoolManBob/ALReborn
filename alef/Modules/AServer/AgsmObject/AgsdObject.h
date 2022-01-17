/******************************************************************************
Module:  AgsdObject.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 24
******************************************************************************/

#if !defined(__AGSDOBJECT_H__)
#define __AGSDOBJECT_H__

#include "ApBase.h"
#include "ApmMap.h"
#include "ApmObject.h"

class AgsdObjectADSector {
public:
//	INT32			lObjectID[MAX_OBJECT_COUNT_IN_SECTOR];
//	ApdObject*		pcsObject[MAX_OBJECT_COUNT_IN_SECTOR];

	ApSafeArray<INT32, MAX_OBJECT_COUNT_IN_SECTOR>			lObjectID;
	ApSafeArray<ApdObject *, MAX_OBJECT_COUNT_IN_SECTOR>	pcsObject;
};

#endif	//__AGSDOBJECT_H__