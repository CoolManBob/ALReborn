/******************************************************************************
Module:  AgpaItem.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 16
******************************************************************************/

#include "AgpaItem.h"

AgpaItem::AgpaItem()
{
}

AgpaItem::~AgpaItem()
{
}

//		AddItem
//	Functions
//		- add item
//	Arguments
//		- pcsItem : item data pointer
//	Return value
//		- AgpdItem * : 추가된 아이템 데이타 포인터
///////////////////////////////////////////////////////////////////////////////
AgpdItem* AgpaItem::AddItem(AgpdItem *pcsItem)
{
	if (!AddObject((PVOID) &pcsItem, pcsItem->m_lID))
		return NULL;

	return pcsItem;
}

//		RemoveItem
//	Functions
//		- remove item
//	Arguments
//		- ulIID : item IID (key value)
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL AgpaItem::RemoveItem(INT32 lIID)
{
	return RemoveObject(lIID);
}

//		GetItem
//	Functions
//		- search & return item pointer
//	Arguments
//		- ulIID : item IID (key value)
//	Return value
//		- AgpdItem * : item data pointer
//		- NULL : failed
///////////////////////////////////////////////////////////////////////////////
AgpdItem* AgpaItem::GetItem(INT32 lIID)
{
	AgpdItem **ppItem = (AgpdItem **) GetObject(lIID);

	if (!ppItem)
		return NULL;

	return *ppItem;
}