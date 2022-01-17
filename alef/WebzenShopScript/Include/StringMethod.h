
#pragma once

#include "include.h"
#include <time.h>

class CStringMethod
{
public:
	CStringMethod();
	virtual ~CStringMethod();

	//static void ConvertStringToList(std::vector<int>& List, tstring strdata);
	//static void ConvertStringToList(std::vector<tstring>& List, tstring strdata);
	static void ConvertStringToDateTime(tm& datetime, tstring strdata);
};
