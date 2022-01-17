/*====================================================================

	AgpaRefinery.cpp

====================================================================*/

#include "AgpaRefinery.h"


/************************************************************/
/*		The Implementation of AgpaRefineTemplate class		*/
/************************************************************/
//
AgpaRefineTemplate::AgpaRefineTemplate()
	{
	}


AgpaRefineTemplate::~AgpaRefineTemplate()
	{
	}


BOOL AgpaRefineTemplate::Add(AgpdRefineTemplate *pAgpdRefineTemplate)
	{
	if (!AddObject((PVOID) &pAgpdRefineTemplate, pAgpdRefineTemplate->m_lItemTID))
		return FALSE;

	return TRUE;	
	}


BOOL AgpaRefineTemplate::Remove(AgpdRefineTemplate *pAgpdRefineTemplate)
	{
	return RemoveObject(pAgpdRefineTemplate->m_lItemTID);
	}


AgpdRefineTemplate* AgpaRefineTemplate::Get(INT32 lID)
	{
	AgpdRefineTemplate **ppAgpdRefineTemplate = (AgpdRefineTemplate **) GetObject(lID);
	if (!ppAgpdRefineTemplate)
		return NULL;	

	return *ppAgpdRefineTemplate;
	}




/********************************************************/
/*		The Implementation of AgpaRefineItem class		*/
/********************************************************/
//
AgpaRefineItem::AgpaRefineItem()
	{
	}


AgpaRefineItem::~AgpaRefineItem()
	{
	}

BOOL AgpaRefineItem::Add(AgpdRefineItem2 *pAgpdRefineItem)
{
    if (!AddObject((PVOID) &pAgpdRefineItem, (char*)pAgpdRefineItem->m_RefineKey.c_str()))
        return FALSE;

    return TRUE;	
}


BOOL AgpaRefineItem::Remove(AgpdRefineItem2 *pAgpdRefineItem)
{
    return RemoveObject((char*)pAgpdRefineItem->m_RefineKey.c_str());
}

AgpdRefineItem2* AgpaRefineItem::Get(const char* key)
{
    //여기다가 파라메타로 들어오는 아이템 TID
    AgpdRefineItem2 **ppAgpdRefineItem = (AgpdRefineItem2 **) GetObject((char*)key);
    if (!ppAgpdRefineItem)
        return NULL;	
    return *ppAgpdRefineItem;
}