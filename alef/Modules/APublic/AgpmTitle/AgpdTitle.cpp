#include "AgpdTitle.h"

AgpdTitle::AgpdTitle()
{
	m_bLoadedTitle = FALSE;
}

vector<AgpdHasTitle>::iterator AgpdTitle::FindByTitleID(INT32 lTitleID)
{
	IterTitle iter;

	for(iter = m_vtTitle.begin(); iter != m_vtTitle.end(); ++iter)
	{
		if ((*iter).lTitleID == lTitleID) return iter;
	}

	return iter;
}

BOOL AgpdTitle::IsFullTitle()
{
	if(m_vtTitle.size() >= AGPDTITLE_TITLE_NUMBER_MAX)
		return TRUE;

	return FALSE;
}

BOOL AgpdTitle::AddTitle(INT32 lTitleID)
{
	if(m_vtTitle.size() >= AGPDTITLE_TITLE_NUMBER_MAX)
		return FALSE;

	IterTitle iter = FindByTitleID(lTitleID);

	if(iter == m_vtTitle.end())
	{
		AgpdHasTitle AddTitle;
		AddTitle.lTitleID = lTitleID;
		AddTitle.lSetTime = 0;

		m_vtTitle.push_back(AddTitle);
		return TRUE;
	}

	return FALSE;
}

BOOL AgpdTitle::RemoveTitle(INT32 lTitleID)
{
	IterTitle iter = FindByTitleID(lTitleID);

	if(iter == m_vtTitle.end())
		return FALSE;

	m_vtTitle.erase(iter);

	return TRUE;
}

BOOL AgpdTitle::IsHaveTitle(INT32 lTitleID)
{
	IterTitle iter = FindByTitleID(lTitleID);

	if(iter == m_vtTitle.end())
		return FALSE;
	
	return TRUE;
}

BOOL AgpdTitle::UpdateSetTime(INT32 lTitleID, UINT32 lSetTime)
{
	IterTitle iter = FindByTitleID(lTitleID);

	if(iter == m_vtTitle.end())
		return FALSE;

	iter->lSetTime = lSetTime;
	
	return TRUE;
}

BOOL AgpdTitle::UpdateUsingTitle(INT32 lTitleID, BOOL bUsingTitle)
{
	IterTitle iter = FindByTitleID(lTitleID);

	if(iter == m_vtTitle.end())
		return FALSE;

	iter->bUsingTitle = bUsingTitle;

	return TRUE;
}

UINT32 AgpdTitle::GetSetTime(INT32 lTitleID)
{
	IterTitle iter = FindByTitleID(lTitleID);

	if(iter == m_vtTitle.end())
		return FALSE;

	return iter->lSetTime;
}

BOOL AgpdTitle::IsUsingTitle(INT32 lTitleID)
{
	IterTitle iter = FindByTitleID(lTitleID);

	if(iter == m_vtTitle.end())
		return FALSE;

	return iter->bUsingTitle;
}

INT32 AgpdTitle::GetUseTitle()
{
	IterTitle iter;

	for(iter = m_vtTitle.begin(); iter != m_vtTitle.end(); ++iter)
	{
		if ((*iter).bUsingTitle == TRUE) return iter->lTitleID;
	}

	return 0;
}

vector<AgpdHasTitle>::iterator AgpdTitle::GetUsingTitle()
{
	IterTitle iter;

	for(iter = m_vtTitle.begin(); iter != m_vtTitle.end(); ++iter)
	{
		if ((*iter).bUsingTitle == TRUE) return iter;
	}

	return iter;
}

UINT32 AgpdTitle::GetLastSetTime()
{
	UINT32 ulResult = 0;

	IterTitle iter;

	for(iter = m_vtTitle.begin(); iter != m_vtTitle.end(); ++iter)
	{
		if ((*iter).lSetTime > ulResult)
			ulResult = (*iter).lSetTime;
	}

	return ulResult;
}

INT32 AgpdTitle::GetHaveTitleNumber()
{
	return m_vtTitle.size();
}