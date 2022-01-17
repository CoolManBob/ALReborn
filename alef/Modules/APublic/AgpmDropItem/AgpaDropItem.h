#if !defined(__AGPADROPITEM_H__)
#define __AGPADROPITEM_H__

#include "ApModule.h"
#include "ApAdmin.h"
#include "AgpdDropItem.h"
#include "ApMutualEx.h"

class AgpaDropItemTemplate : public ApAdmin {
public:
	AgpaDropItemTemplate();
	~AgpaDropItemTemplate();

	// item data manage functions
	AgpdDropItemTemplate* AddDropItemTemplate(AgpdDropItemTemplate *pcsDropItemTemplate);
	BOOL RemoveDropItemTemplate(char *pstrTemplateName);
	AgpdDropItemTemplate* GetDropItemTemplate(char *pstrTemplateName);
	AgpdDropItemTemplate *GetDropItemTemplate(INT32 lTID);
	AgpdDropItemTemplate* GetTemplateSequence(INT32 *plIndex);

};

class AgpaDropItemEquipTemplate : public ApAdmin {
public:
	AgpaDropItemEquipTemplate();
	~AgpaDropItemEquipTemplate();

	// item data manage functions
	AgpdDropItemEquipTemplate* AddDropItemEquipTemplate(AgpdDropItemEquipTemplate *pcsDropItemEquipTemplate);
	BOOL RemoveDropItemEquipTemplate(INT32 lTID);
	AgpdDropItemEquipTemplate* GetDropItemEquipTemplate(INT32 lTID);
	AgpdDropItemEquipTemplate* GetEquipTemplateSequence(INT32 *plIndex);
};


#endif //__AGPADROPITEM_H__
