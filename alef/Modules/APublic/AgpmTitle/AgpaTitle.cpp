#include "AgpaTitle.h"

AgpaTitleTemplate::AgpaTitleTemplate()
{
}

AgpaTitleTemplate::~AgpaTitleTemplate()
{
}

BOOL AgpaTitleTemplate::Add(AgpdTitleTemplate* pAgpdTitleTemplate)
{
	if (!AddObject((PVOID) &pAgpdTitleTemplate, pAgpdTitleTemplate->m_nTitleTid))
		return FALSE;

	return TRUE;
}

BOOL AgpaTitleTemplate::Remove(AgpdTitleTemplate* pAgpdTitleTemplate)
{
	return RemoveObject(pAgpdTitleTemplate->m_nTitleTid);
}

AgpdTitleTemplate* AgpaTitleTemplate::Get(INT32 lID)
{
	AgpdTitleTemplate **ppAgpdTitleTemplate = (AgpdTitleTemplate **) GetObject(lID);
	if (!ppAgpdTitleTemplate)
		return NULL;	

	return *ppAgpdTitleTemplate;
}

AgpaTitleStringTemplate::AgpaTitleStringTemplate()
{
	m_vtTitleStringTemplate.reserve(AGPDTITLE_MAX_TITLE_NUMBER);
	m_vtTitleStringTemplate.clear();
	TitleStringTemplateCount = 0;
	//iterCurrentTitleStringTemplate = NULL;
}

iterTitleStringTemplate AgpaTitleStringTemplate::FindByTitleTid(INT32 TitleTid)
{
	iterTitleStringTemplate iter;

	for(iter = m_vtTitleStringTemplate.begin(); iter != m_vtTitleStringTemplate.end();++iter)
	{
		if((*iter).m_nTitleTid == TitleTid)
			return iter;
	}

	return iter;
}

vectorTitleStringTemplate AgpaTitleStringTemplate::FindByCategoryID(INT32 TitleCategoryID)
{
	vectorTitleStringTemplate vectorResult;
	vectorResult.clear();

	iterTitleStringTemplate iter;

	for(iter = m_vtTitleStringTemplate.begin(); iter != m_vtTitleStringTemplate.end();++iter)
	{
		if((*iter).m_nFirstCategory == TitleCategoryID)
			vectorResult.push_back(*iter);
	}

	return vectorResult;
}

BOOL AgpaTitleStringTemplate::AddTitleStringTemplate(AgpdTitleStringTemplate TitleStringTemplate)
{
	if(TitleStringTemplateCount >= AGPDTITLE_MAX_TITLE_NUMBER)
		return FALSE;

	iterTitleStringTemplate iter = FindByTitleTid(TitleStringTemplate.m_nTitleTid);

	if(iter != m_vtTitleStringTemplate.end())
		return FALSE;

	m_vtTitleStringTemplate.push_back(TitleStringTemplate);
	TitleStringTemplateCount++;

	return TRUE;
}

BOOL AgpaTitleStringTemplate::RemoveTitleStringTemplate(INT32 TitleTid)
{
	iterTitleStringTemplate iter = FindByTitleTid(TitleTid);

	if (iter == m_vtTitleStringTemplate.end())
		return FALSE;

	m_vtTitleStringTemplate.erase(iter);
	TitleStringTemplateCount--;

	return TRUE;
}

iterTitleStringTemplate AgpaTitleStringTemplate::Begin()
{
	iterCurrentTitleStringTemplate = m_vtTitleStringTemplate.begin();

	return iterCurrentTitleStringTemplate;
}

iterTitleStringTemplate AgpaTitleStringTemplate::Next()
{
	++iterCurrentTitleStringTemplate;

	return iterCurrentTitleStringTemplate;
}



AgpaTitleCategory::AgpaTitleCategory()
{
	m_vtTitleCategory.reserve(AGPDTITLE_MAX_TITLE_CATEGORY);
	m_vtTitleCategory.clear();

	//iterCurrentCategory = NULL;
	TitleCategoryCount = 0;
}

VectorTitleCategory::iterator AgpaTitleCategory::FindByTitleCategoryID(INT32 TitleCategoryID)
{
	iterTitleCategory iter;

	for(iter = m_vtTitleCategory.begin(); iter != m_vtTitleCategory.end();++iter)
	{
		if((*iter).m_nFirstCategory == TitleCategoryID)
			return iter;
	}

	return iter;
}

bool AgpaTitleCategory::CheckForTitleCategoryID(INT32 TitleCategoryID)
{
	iterTitleCategory iter;

	for (iter = m_vtTitleCategory.begin(); iter != m_vtTitleCategory.end(); ++iter)
	{
		if ((*iter).m_nFirstCategory == TitleCategoryID)
			return true;
	}

	return false;
}

BOOL AgpaTitleCategory::AddTitleCategory(AgpdTitleCategory TitleCategory)
{
	if(TitleCategoryCount >= AGPDTITLE_MAX_TITLE_CATEGORY)
		return FALSE;

	iterTitleCategory iter = FindByTitleCategoryID(TitleCategory.m_nFirstCategory);

	if(iter != m_vtTitleCategory.end())
		return FALSE;

	m_vtTitleCategory.push_back(TitleCategory);
	TitleCategoryCount++;

	return TRUE;
}

BOOL AgpaTitleCategory::RemoveTitleCategory(INT32 TitleCategoryID)
{
	iterTitleCategory iter = FindByTitleCategoryID(TitleCategoryID);

	if(iter == m_vtTitleCategory.end())
		return FALSE;

	m_vtTitleCategory.erase(iter);
	TitleCategoryCount--;

	return TRUE;
}

VectorTitleCategory::iterator AgpaTitleCategory::Begin()
{
	iterCurrentCategory = m_vtTitleCategory.begin();

	return iterCurrentCategory;
}

VectorTitleCategory::iterator AgpaTitleCategory::Next()
{
	++iterCurrentCategory;
	return iterCurrentCategory;
}


