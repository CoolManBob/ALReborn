#pragma once

#include <list>
#include <string>
#include <hash_map>

using namespace std;
using namespace stdext;

const INT32 MAX_CHECK_POINT = 5;

struct TemplateData
{
	INT32 lID;
	string strGiveName;
	string strConfirmName;
	string strCheckPointID[ MAX_CHECK_POINT ];

	TemplateData()
	{
		lID = 0;
	}
};

class HashMapTemplate
{
public:
	typedef hash_map< INT32, TemplateData* > HashMapQTemplate;
	typedef HashMapQTemplate::iterator IterTemplate;

	HashMapQTemplate m_HashMap;

public:
	HashMapTemplate(void);
	virtual ~HashMapTemplate(void);

	void Destroy();

	TemplateData* AddTemplate( INT32 lID, CHAR* szGiveName, CHAR* szConfirmName );
};
