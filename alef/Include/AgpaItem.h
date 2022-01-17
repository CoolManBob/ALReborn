/******************************************************************************
Module:  AgpaItem.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 15
******************************************************************************/

#if !defined(__AGPAITEM_H__)
#define __AGPAITEM_H__

#include "ApModule.h"
#include "ApAdmin.h"
#include "AgpdItem.h"
#include "ApMutualEx.h"

class AgpaItem : public ApAdmin {
public:
	AgpaItem();
	~AgpaItem();

	// item data manage functions
	AgpdItem* AddItem(AgpdItem *pcsItem);
	BOOL RemoveItem(INT32 lIID);
	AgpdItem* GetItem(INT32 lIID);
};

//////////////////////////////////////////////////////////////////////////
//
#include "AgpdItemTemplate.h"
#include <map>

typedef map<INT32, AgpdItemTemplate*> AgpaItemTemplate;

#endif //__AGPAITEM_H__
