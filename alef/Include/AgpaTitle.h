#ifndef _AGPA_TITLE_H_
#define _AGPA_TITLE_H_

#include "ApAdmin.h"
#include "AgpdTitle.h"
#include "AgpdTitleTemplate.h"

const INT32 AGPDTITLE_MAX_TITLE_CATEGORY	=	100;
const INT32 AGPDTITLE_MAX_TITLE_NUMBER		=	10000;

class AgpaTitleTemplate : public ApAdmin
{
public:
	AgpaTitleTemplate();
	virtual~AgpaTitleTemplate();

	BOOL Add(AgpdTitleTemplate* pAgpdTitleTemplate);
	BOOL Remove(AgpdTitleTemplate* pAgpdTitleTemplate);
	AgpdTitleTemplate* Get(INT32 lID);

};

class AgpaTitleStringTemplate
{
public:
	vectorTitleStringTemplate m_vtTitleStringTemplate;
	iterTitleStringTemplate iterCurrentTitleStringTemplate;
	INT32 TitleStringTemplateCount;

public:
	AgpaTitleStringTemplate();

	BOOL AddTitleStringTemplate(AgpdTitleStringTemplate TitleStringTemplate);

	BOOL RemoveTitleStringTemplate(INT32 TitleTid);

	iterTitleStringTemplate FindByTitleTid(INT32 TitleTid);
	vectorTitleStringTemplate FindByCategoryID(INT32 TitleCategoryID);
	iterTitleStringTemplate Begin();
	iterTitleStringTemplate Next();
};

class AgpaTitleCategory
{
public:
	VectorTitleCategory m_vtTitleCategory;
	iterTitleCategory iterCurrentCategory;
	INT32 TitleCategoryCount;

public:
	AgpaTitleCategory();

	BOOL AddTitleCategory(AgpdTitleCategory TitleCategory);

	BOOL RemoveTitleCategory(INT32 TitleCategoryID);

	VectorTitleCategory::iterator FindByTitleCategoryID(INT32 TitleCategoryID);
	bool CheckForTitleCategoryID(INT32 TitleCategoryID);
	VectorTitleCategory::iterator Begin();
	VectorTitleCategory::iterator Next();
};

#endif //_AGPA_TITLE_H_