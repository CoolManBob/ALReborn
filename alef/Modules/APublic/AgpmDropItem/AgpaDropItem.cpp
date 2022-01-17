#include "AgpaDropItem.h"

AgpaDropItemTemplate::AgpaDropItemTemplate()
{
}

AgpaDropItemTemplate::~AgpaDropItemTemplate()
{
}

AgpdDropItemTemplate *AgpaDropItemTemplate::AddDropItemTemplate(AgpdDropItemTemplate *pcsDropItemTemplate)
{
	if (!AddObject((PVOID) &pcsDropItemTemplate, pcsDropItemTemplate->m_lTID, pcsDropItemTemplate->m_strTemplateName))
		return NULL;

	return pcsDropItemTemplate;
}

BOOL AgpaDropItemTemplate::RemoveDropItemTemplate(char *pstrTemplateName)
{
	return RemoveObject(pstrTemplateName);
}

AgpdDropItemTemplate *AgpaDropItemTemplate::GetDropItemTemplate(char *pstrTemplateName)
{
	AgpdDropItemTemplate **ppItem = (AgpdDropItemTemplate **) GetObject(pstrTemplateName);

	if (!ppItem)
		return NULL;

	return *ppItem;
}

AgpdDropItemTemplate *AgpaDropItemTemplate::GetDropItemTemplate(INT32 lTID)
{
	AgpdDropItemTemplate **ppItem = (AgpdDropItemTemplate **) GetObject(lTID);

	if (!ppItem)
		return NULL;

	return *ppItem;
}

AgpdDropItemTemplate *AgpaDropItemTemplate::GetTemplateSequence(INT32 *plIndex)
{
	AgpdDropItemTemplate **ppcsTemplate = (AgpdDropItemTemplate **)GetObjectSequence(plIndex);

	if (!ppcsTemplate)
		return NULL;

	return  *ppcsTemplate;
}

//EquipTemplate

AgpaDropItemEquipTemplate::AgpaDropItemEquipTemplate()
{
}

AgpaDropItemEquipTemplate::~AgpaDropItemEquipTemplate()
{
}

AgpdDropItemEquipTemplate *AgpaDropItemEquipTemplate::AddDropItemEquipTemplate(AgpdDropItemEquipTemplate *pcsDropItemEquipTemplate)
{
	if (!AddObject((PVOID) &pcsDropItemEquipTemplate, pcsDropItemEquipTemplate->m_lTID))
		return NULL;

	return pcsDropItemEquipTemplate;
}

BOOL AgpaDropItemEquipTemplate::RemoveDropItemEquipTemplate(INT32 lTID)
{
	return RemoveObject(lTID);
}

AgpdDropItemEquipTemplate *AgpaDropItemEquipTemplate::GetDropItemEquipTemplate(INT32 lTID)
{
	AgpdDropItemEquipTemplate **ppItem = (AgpdDropItemEquipTemplate **) GetObject(lTID);

	if (!ppItem)
		return NULL;

	return *ppItem;
}

AgpdDropItemEquipTemplate *AgpaDropItemEquipTemplate::GetEquipTemplateSequence(INT32 *plIndex)
{
	AgpdDropItemEquipTemplate **ppcsEquipTemplate = (AgpdDropItemEquipTemplate **)GetObjectSequence(plIndex);

	if (!ppcsEquipTemplate)
		return NULL;

	return  *ppcsEquipTemplate;
}
