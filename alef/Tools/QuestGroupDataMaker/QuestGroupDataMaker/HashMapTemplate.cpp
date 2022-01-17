#include "StdAfx.h"
#include ".\hashmaptemplate.h"

HashMapTemplate::HashMapTemplate(void)
{
}

HashMapTemplate::~HashMapTemplate(void)
{
	Destroy();
}

void HashMapTemplate::Destroy()
{
	IterTemplate iter = m_HashMap.begin();

	while (iter != m_HashMap.end())
	{
		delete iter->second;
		++iter;
	}

	m_HashMap.clear();
}

TemplateData* HashMapTemplate::AddTemplate(INT32 lID, CHAR* szGiveName, CHAR* szConfirmName)
{
	TemplateData* pData = new TemplateData;
	pData->lID = lID;
	if (szGiveName)
		pData->strGiveName = szGiveName;
	if (szConfirmName)
		pData->strConfirmName = szConfirmName;

	m_HashMap.insert(HashMapQTemplate::value_type(lID, pData));

	return pData;
}

